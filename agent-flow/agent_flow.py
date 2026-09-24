#!/usr/bin/env python3
"""
agent_flow.py — Claude Code 헤드리스 실행 REST 브릿지 (비동기 접수 + 완료 콜백판)

PUT /notebookflow/agent/execute 요청을 받으면 claude -p 종료를 기다리지 않고
수십 ms 안에 {"RESULT":"0","job_id":...}만 응답한다(OutboundClient.cpp의
http 호출 read_timeout=10초를 넘기지 않기 위함 - claude -p 자체는 몇 초~몇 분
걸릴 수 있으므로 응답을 기다리게 하면 반드시 10초 타임아웃에 걸린다).
같은 엔드포인트에 @app.put과 @app.post를 모두 등록한다 - 이 DSL 엔진은
GET/POST 두 갈래로만 나가고 "PUT"이라는 실제 wire-level HTTP verb 자체가
없다(전송메시지.메소드=PUT이라고 써도 실제로는 POST로 나감).

요청 스키마: {"instruction": "...", "callback_chat_id": "..."} 두 필드만
받는다. 완료되면 스스로 NotebookFlow.exe(FLOW_PUT_URL)에 fire-and-forget
PUT을 보내 결과를 되돌려준다({"ACTION_EVENT":"클루드코드완료","chat_id":...,
"result":...,"ok":...}) - rest.sce의 상태::FLOW.초기에 이 이벤트명을 받는
조건을 추가해 처리::FLOW.클루드코드완료처리로 라우팅한다.

환경변수:
  AGENT_CWD          — claude 작업 디렉터리 (기본: 이 스크립트와 같은 부모
                       폴더 밑의 notebookflow/ - NotebookFlow.exe/CLAUDE.md가
                       있는 폴더와 형제 구조라는 배포 레이아웃을 가정)
  AGENT_TIMEOUT_SEC  — claude -p 서브프로세스 타임아웃 초 (기본: 600)
  AGENT_MAX_BUDGET   — 최대 비용 USD (선택)
  AGENT_HOST         — 바인드 호스트 (기본: 127.0.0.1 - 로컬 전용 호출)
  AGENT_PORT         — 포트 (기본: 8430 - addr.ini [AGENT] domain과 일치)
"""

from __future__ import annotations

import asyncio
import json
import logging
import os
import subprocess
import sys
import time
import uuid
from contextlib import asynccontextmanager
from typing import Any, Optional

import httpx
from fastapi import FastAPI, Request
from fastapi.responses import JSONResponse
import uvicorn

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)-5s] [agent-flow] %(message)s",
    datefmt="%Y-%m-%dT%H:%M:%S",
)
log = logging.getLogger("agent-flow")

# ── 상수 ──────────────────────────────────────────────────────────
# PyInstaller onedir로 얼린 실행 파일에서는 __file__이 exe 자신의 폴더가
# 아니라 그 안의 _internal/ 폴더를 가리킨다(부트로더가 스크립트를 그
# 안에서 압축 해제해 실행하기 때문) - "이 폴더 밑의 notebookflow/"라는
# 원래 의도가 실제로는 "exe 폴더/notebookflow/"가 아니라
# "exe 폴더/_internal/../notebookflow/" = "exe 폴더/notebookflow"가 아니라
# 상위 폴더 계산이 한 단계 어긋나 "exe 폴더 자신의 하위" 잘못된 경로가
# 되는 버그가 있었다(2026-09-25 실사용자 테스트로 재현 - "클루드코드"
# 명령이 "cwd 디렉터리가 없습니다"로 항상 실패). sys.executable은 얼린
# 상태여도 항상 실제 exe 파일 경로를 정확히 가리키므로 그 기준으로
# 계산한다(PyInstaller 공식 관용구).
_SELF_DIR = os.path.dirname(os.path.abspath(sys.executable)) if getattr(sys, "frozen", False) \
    else os.path.dirname(os.path.abspath(__file__))
_DEFAULT_CWD = os.path.normpath(
    os.path.join(_SELF_DIR, "..", "notebookflow")
)
CWD = os.getenv("AGENT_CWD", _DEFAULT_CWD)
TIMEOUT_SEC = int(os.getenv("AGENT_TIMEOUT_SEC", "600"))
MAX_BUDGET_USD = os.getenv("AGENT_MAX_BUDGET")  # None 또는 float 문자열
HOST = os.getenv("AGENT_HOST", "127.0.0.1")
PORT = int(os.getenv("AGENT_PORT", "8430"))

FLOW_BASE = "http://127.0.0.1:18099"
FLOW_PUT_URL = f"{FLOW_BASE}/notebookflow/flow"
FLOW_PUT_TIMEOUT = 5.0  # 완료 콜백은 fire-and-forget - 실패해도 무시(로그만)

# claude -p 동시 실행 직렬화 - 같은 cwd에 대해 여러 claude -p가 동시에
# 파일을 건드리는 경쟁을 피하기 위해 실제 서브프로세스 실행 자체는 한 번에
# 하나씩만 돈다. HTTP 응답은 이 락과 무관하게 즉시 나간다(락은 백그라운드
# 태스크 안에서만 걸림).
_lock = asyncio.Lock()

# job_id -> dict. 메모리에만 존재 - 재시작되면 진행 중 job은 사라진다. 완료
# 후 30초 유예를 두고 제거(디버깅/조회용, /jobs/{id}).
active_jobs: dict[str, dict[str, Any]] = {}

_http_client: Optional[httpx.AsyncClient] = None


@asynccontextmanager
async def lifespan(_app: FastAPI):
    global _http_client
    _http_client = httpx.AsyncClient()
    log.info(
        "agent-flow 기동 - host=%s port=%s cwd=%s timeout=%ss",
        HOST, PORT, CWD, TIMEOUT_SEC,
    )
    yield
    if _http_client:
        await _http_client.aclose()


app = FastAPI(title="agent-flow", version="2.0", lifespan=lifespan)


def build_command(instruction: str) -> list[str]:
    """헤드리스 Claude 호출. --agent 플래그 없이 단일 세션으로 실행한다."""
    cmd = [
        "claude",
        "-p",
        instruction,
        "--permission-mode",
        "acceptEdits",
        "--output-format",
        "json",
        "--no-session-persistence",
    ]
    if MAX_BUDGET_USD:
        cmd += ["--max-budget-usd", str(MAX_BUDGET_USD)]
    return cmd


def run_claude(instruction: str) -> dict[str, Any]:
    """블로킹 호출(subprocess.run) - 반드시 asyncio.to_thread로 감싸서 호출할
    것(이벤트루프를 막지 않기 위해). 반환: {"ok","text","cost","duration_ms","raw_error"}."""
    if not os.path.isdir(CWD):
        return {
            "ok": False, "text": "", "cost": 0.0, "duration_ms": 0,
            "raw_error": f"cwd 디렉터리가 없습니다: {CWD}",
        }

    cmd = build_command(instruction)
    try:
        proc = subprocess.run(
            cmd, cwd=CWD, capture_output=True, text=True, timeout=TIMEOUT_SEC,
        )
    except subprocess.TimeoutExpired:
        return {
            "ok": False, "text": "", "cost": 0.0, "duration_ms": 0,
            "raw_error": f"시간 초과 ({TIMEOUT_SEC}초)",
        }
    except FileNotFoundError:
        return {
            "ok": False, "text": "", "cost": 0.0, "duration_ms": 0,
            "raw_error": (
                "claude 실행파일을 찾을 수 없습니다 - "
                "'where claude'로 PATH 확인 필요 (cwd는 존재함)"
            ),
        }

    # --output-format json은 실패 상황(인증 만료 등)에서도 exit code 1과
    # 함께 stdout에 유효한 JSON({"is_error":true,"result":"..."})을 남기는
    # 경우가 있다. 항상 stdout JSON 파싱을 먼저 시도하고, 파싱 자체가
    # 실패할 때만(JSON이 아예 없는 크래시 등) exit code/stderr 기반
    # 메시지로 폴백한다.
    try:
        data = json.loads(proc.stdout)
    except json.JSONDecodeError:
        if proc.returncode != 0:
            return {
                "ok": False, "text": "", "cost": 0.0, "duration_ms": 0,
                "raw_error": f"claude 종료 코드 {proc.returncode}: {proc.stderr[:500]}",
            }
        return {
            "ok": False, "text": "", "cost": 0.0, "duration_ms": 0,
            "raw_error": f"JSON 파싱 실패: {proc.stdout[:300]}",
        }

    denials = data.get("permission_denials", [])
    if denials:
        log.warning("permission_denials 발생: %s", denials)

    is_error = data.get("is_error", True)
    return {
        "ok": not is_error,
        "text": data.get("result", ""),
        "cost": data.get("total_cost_usd", 0.0),
        "duration_ms": data.get("duration_ms", 0),
        "raw_error": "" if not is_error else data.get("result", "알 수 없는 오류"),
    }


async def report_done(callback_chat_id: Optional[str], text: str, ok: bool) -> None:
    """완료 콜백 - fire-and-forget PUT to NotebookFlow.exe. callback_chat_id가
    없으면(콜백 대상 없는 단발 호출) 조용히 스킵. 실패해도 예외를 삼키고
    로그만 남긴다(관찰자 쪽 실패가 본 작업 결과 자체에 영향을 주면 안 됨.
    claude -p는 이미 끝났으므로)."""
    if not callback_chat_id:
        log.info("callback_chat_id 없음 - 완료 콜백 스킵")
        return
    body = {
        "ACTION_EVENT": "클루드코드완료",
        "chat_id": callback_chat_id,
        "result": text,
        "ok": "1" if ok else "0",
    }
    global _http_client
    try:
        assert _http_client is not None
        resp = await _http_client.put(FLOW_PUT_URL, json=body, timeout=FLOW_PUT_TIMEOUT)
        if resp.status_code != 200:
            log.warning("완료 콜백 실패(HTTP %s): %s", resp.status_code, resp.text[:300])
    except Exception as e:
        log.warning("완료 콜백 실패(연결/타임아웃, 무시): %s", e)


async def run_and_callback(job_id: str, instruction: str, callback_chat_id: Optional[str]) -> None:
    """백그라운드 태스크(asyncio.create_task로 등록, HTTP 응답과 무관하게
    돈다). claude -p 실행 자체만 락으로 직렬화 - 동시에 여러 명령이 접수돼도
    한 번에 하나씩 순서대로 실행된다(요청은 즉시 접수되어 대기열에 쌓임)."""
    job = active_jobs.get(job_id)
    async with _lock:
        r = await asyncio.to_thread(run_claude, instruction)

    if job is not None:
        job["finished_at"] = time.time()
        job["ok"] = r["ok"]
        job["cost"] = r.get("cost", 0.0)
        job["duration_ms"] = r.get("duration_ms", 0)

    text_result = r["text"] if r["ok"] else f"[클루드코드 실행 실패] {r['raw_error']}"
    log.info(
        "[%s] 완료 (%s, $%.4f, %.1fs)",
        job_id, "성공" if r["ok"] else "실패", r.get("cost", 0), r.get("duration_ms", 0) / 1000,
    )
    await report_done(callback_chat_id, text_result, r["ok"])

    # 완료 직후 짧은 유예(30초) 후 레지스트리에서 제거 - 무상태 원칙과 완료
    # 직후 조회 가능성을 둘 다 만족시키기 위함.
    await asyncio.sleep(30)
    active_jobs.pop(job_id, None)


@app.put("/notebookflow/agent/execute")
@app.post("/notebookflow/agent/execute")
async def execute_agent(request: Request):
    """즉시 접수 응답 - claude -p 실행을 기다리지 않는다.
    요청 스키마: {"instruction": "...", "callback_chat_id": "..."}."""
    try:
        req = await request.json()
    except Exception:
        return JSONResponse(status_code=400, content={"RESULT": "1", "error": "JSON parse 실패"})

    if not isinstance(req, dict):
        return JSONResponse(
            status_code=400,
            content={"RESULT": "1", "error": "요청 body는 JSON object여야 합니다"},
        )

    instruction = (req.get("instruction") or "").strip()
    callback_chat_id = req.get("callback_chat_id") or None

    if not instruction:
        return JSONResponse(status_code=400, content={"RESULT": "1", "error": "instruction이 비어 있습니다"})

    job_id = f"job-{uuid.uuid4().hex[:12]}"
    active_jobs[job_id] = {
        "instruction": instruction,
        "callback_chat_id": callback_chat_id,
        "started_at": time.time(),
    }
    log.info("[%s] 작업 접수 (callback_chat_id=%s): %s", job_id, callback_chat_id, instruction[:80])

    asyncio.create_task(run_and_callback(job_id, instruction, callback_chat_id))

    return JSONResponse(content={"RESULT": "0", "job_id": job_id})


@app.get("/jobs/{job_id}")
async def get_job(job_id: str):
    job = active_jobs.get(job_id)
    if job is None:
        return JSONResponse({"RESULT": "1", "status": "not found"}, status_code=404)
    return {"job_id": job_id, **job}


@app.get("/health")
async def health():
    return {
        "status": "ok",
        "cwd": CWD,
        "timeout_sec": TIMEOUT_SEC,
        "active_jobs": len(active_jobs),
    }


def main():
    uvicorn.run(app, host=HOST, port=PORT, log_level="info")


if __name__ == "__main__":
    main()
