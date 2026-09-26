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

요청 스키마: {"instruction": "...", "callback_chat_id": "...", "session_key":
"..."(선택), "agent_name": "..."(선택)} - session_key를 안 주면 기존과
동일하게 매번 완전히 새 세션(--no-session-persistence)으로 실행된다.
session_key를 주면(예: "claude_persistent") 그 키에 고정된 세션을 계속
이어간다 - 세션 ID는 최초 호출 시 새로 발급해 SESSION_MAP_PATH에 영구
저장하고, 이후 호출은 그 ID를 --resume한다. agent_name을 주면(예:
"flow-dev-lead") 그 이름의 커스텀 에이전트 정의로 --agent를 지정해 특정
역할/시스템 프롬프트를 갖는 서브에이전트로 실행한다. 완료되면 스스로
NotebookFlow.exe(FLOW_PUT_URL)에 fire-and-forget PUT을 보내 결과를
되돌려준다({"ACTION_EVENT":"클루드코드완료","chat_id":...,"result":...,
"ok":...}) - rest.sce의 상태::FLOW.초기에 이 이벤트명을 받는 조건을 추가해
처리::FLOW.클루드코드완료처리로 라우팅한다.

session_key별 누적 대화 횟수(2026-09-26 도입, "클루드코드" 지속 세션 통합
티켓): 매 호출마다 SESSION_MAP_PATH의 해당 레코드 turn_count를 +1 하고,
RESET_TURN_THRESHOLD(기본 20)의 배수에 도달하면 정상 응답 뒤에 "초기화할까요?
(예/아니오)" 확인 메시지를 한 번 더 보낸다(report_done을 두 번 부르는 것 뿐,
새 콜백 경로 없음). 그 확인 대기 중(awaiting_reset_confirm=true)에 다음
"클루드코드 [텍스트]" 호출이 오면 그 텍스트를 지시문이 아니라 예/아니오
응답으로 해석한다(claude -p 미실행, 비용 없음) - 자세한 판정 로직은
_classify_confirm_reply()/handle_reset_confirm_reply() 참고.

PUT/POST /notebookflow/agent/reset — "클루드초기화" Telegram 명령용. 요청
스키마: {"session_key": "...", "callback_chat_id": "..."(선택)}. 확인 없이
즉시 그 session_key를 세션 맵에서 제거한다(다음 호출은 새 세션으로 재부트).

환경변수:
  AGENT_CWD                 — claude 작업 디렉터리 (기본: 이 스크립트와 같은
                             부모 폴더 밑의 notebookflow/ - NotebookFlow.exe/
                             CLAUDE.md가 있는 폴더와 형제 구조 배포 레이아웃 가정)
  AGENT_TIMEOUT_SEC         — claude -p 서브프로세스 타임아웃 초 (기본: 600)
  AGENT_MAX_BUDGET          — 최대 비용 USD (선택)
  AGENT_HOST                — 바인드 호스트 (기본: 127.0.0.1 - 로컬 전용 호출)
  AGENT_PORT                — 포트 (기본: 8430 - addr.ini [AGENT] domain과 일치)
  AGENT_RESET_TURN_THRESHOLD — session_key별 리셋 확인을 물어볼 대화 횟수
                             배수 (기본: 20)
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
# session_key -> claude 세션 UUID 영구 매핑. CWD(실제 프로젝트 폴더) 안에
# 두어서 agent-flow.exe를 재빌드/재설치해도 유지된다(agent-flow 자신의
# dist 폴더는 재빌드마다 통째로 새로 만들어지므로 거기 두면 안 됨).
SESSION_MAP_PATH = os.path.join(CWD, "agent_flow_sessions.json")
TIMEOUT_SEC = int(os.getenv("AGENT_TIMEOUT_SEC", "600"))
MAX_BUDGET_USD = os.getenv("AGENT_MAX_BUDGET")  # None 또는 float 문자열
HOST = os.getenv("AGENT_HOST", "127.0.0.1")
PORT = int(os.getenv("AGENT_PORT", "8430"))

FLOW_BASE = "http://127.0.0.1:18099"
FLOW_PUT_URL = f"{FLOW_BASE}/notebookflow/flow"
FLOW_PUT_TIMEOUT = 5.0  # 완료 콜백은 fire-and-forget - 실패해도 무시(로그만)

# session_key별 누적 대화 횟수가 이 값의 배수에 도달할 때마다(정상 응답을
# 보낸 직후) "초기화할까요? (예/아니오)" 확인 메시지를 한 번 더 보낸다
# (2026-09-26 "클루드코드" 지속 세션 통합 티켓, 요구사항 3/8). 기본값 20은
# 이 프로젝트 실측(호출당 $0.03~0.45, 3~15초)을 근거로 고른 절충값이다 -
# 10 정도로 너무 잦으면 확인 질문 자체가 스팸처럼 느껴지고(한 작업 세션
# 안에서 여러 번 물어볼 수 있음), 반대로 너무 크면(50+) 컨텍스트가 길어진
# 채로 계속 이어져 호출당 비용이 조용히 불어난다. 20은 개인 dorafather님의
# 통상적인 "한 차례 작업 묶음"보다는 넉넉하게 여유를 주면서도, 방치된 세션이
# 무한정 커지는 걸 막는 선이라고 판단했다. AGENT_MAX_BUDGET처럼 환경변수로
# 오버라이드 가능(하드코딩 금지 요구사항).
RESET_TURN_THRESHOLD = int(os.getenv("AGENT_RESET_TURN_THRESHOLD", "20"))

# 리셋 확인 질문("...초기화할까요? (예/아니오)")에 대한 다음 응답을 판정하는
# 어휘집 - 자연스러운 변형(네/넵/응/아니요/아니 등)까지 느슨하게 인정하되,
# _classify_confirm_reply()가 "첫 어절 전체 일치"만 인정하므로 "예약
# 취소해줘"의 "예약"처럼 우연히 접두어가 겹치는 실제 지시문을 오인하지
# 않는다.
_YES_WORDS = {"예", "네", "넵", "웅", "응", "오케이", "okay", "ok", "yes", "y", "ㅇ", "ㅇㅋ"}
_NO_WORDS = {"아니오", "아니요", "아니", "노", "no", "n", "ㄴ", "ㄴㄴ"}

# claude -p 동시 실행 직렬화 - 같은 cwd에 대해 여러 claude -p가 동시에
# 파일을 건드리는 경쟁을 피하기 위해 실제 서브프로세스 실행 자체는 한 번에
# 하나씩만 돈다. HTTP 응답은 이 락과 무관하게 즉시 나간다(락은 백그라운드
# 태스크 안에서만 걸림).
_lock = asyncio.Lock()

# 이 _lock은 "동시 실행"만 막을 뿐 "중복 실행"은 막지 않는다 - 같은
# session_key로 첫 요청이 아직 처리 중일 때 사용자가 응답이 없어 보여
# 똑같은 지시를 한 번 더 보내면, 두 번째 요청은 이 락 뒤에서 얌전히
# 대기했다가 첫 번째가 끝난 뒤 "또 한 번" 실행되어 같은 세션에 지시가
# 두 번 반영된다(2026-09-25 실사용 중 발견 - turn_count가 의도치 않게
# 2씩 올라감, 비용도 두 배). 이를 막기 위해 지금 claude -p가 실행 중인
# session_key 집합을 따로 추적한다 - 이미 처리 중인 session_key로 또
# 요청이 오면 claude -p를 새로 실행하지 않고 "처리 중" 안내만 보낸다.
_busy_session_keys: set[str] = set()

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


def _load_session_map() -> dict[str, dict[str, Any]]:
    """session_key -> {"uuid","turn_count","awaiting_reset_confirm"} 레코드
    맵을 돌려준다. 2026-09-25 "플로우개발" 최초 도입 당시엔 session_key ->
    uuid 문자열 하나만 저장했다(대화 횟수/리셋 확인 상태를 담을 자리가
    없었음) - 2026-09-26 지속 세션 통합 티켓에서 dict 포맷으로 확장하면서,
    기존 파일에 문자열 값이 남아 있으면 이 자리에서 새 포맷으로 승격한다
    (agent_flow_sessions.json을 수동으로 지우지 않아도 하위호환)."""
    try:
        with open(SESSION_MAP_PATH, "r", encoding="utf-8") as f:
            raw = json.load(f)
    except (FileNotFoundError, json.JSONDecodeError):
        return {}
    out: dict[str, dict[str, Any]] = {}
    for k, v in raw.items():
        if isinstance(v, str):
            out[k] = {"uuid": v, "turn_count": 0, "awaiting_reset_confirm": False}
        elif isinstance(v, dict):
            out[k] = {
                "uuid": v.get("uuid"),
                "turn_count": int(v.get("turn_count", 0) or 0),
                "awaiting_reset_confirm": bool(v.get("awaiting_reset_confirm", False)),
            }
    return out


def _save_session_map(m: dict[str, dict[str, Any]]) -> None:
    with open(SESSION_MAP_PATH, "w", encoding="utf-8") as f:
        json.dump(m, f, ensure_ascii=False, indent=2)


def _get_session_record(session_key: str) -> dict[str, Any]:
    m = _load_session_map()
    return m.get(session_key, {"uuid": None, "turn_count": 0, "awaiting_reset_confirm": False})


def _get_or_create_session_uuid(session_key: str) -> tuple[str, bool]:
    """session_key에 대응하는 세션 UUID를 돌려준다. 반환값 두 번째는 "이미
    있던 세션인지"(True) / "방금 새로 발급했는지"(False) - 새로 발급한
    경우 아직 claude 쪽에 그 세션이 실존하지 않으므로 --resume이 아니라
    --session-id로 처음 만들어야 한다(2026-09-25 실측 확인 - --resume을
    존재하지 않는 세션 ID에 쓰면 exit 1 + "No conversation found"로 실패,
    자동으로 새로 만들어주지 않는다)."""
    m = _load_session_map()
    rec = m.get(session_key)
    if rec and rec.get("uuid"):
        return rec["uuid"], True
    new_id = str(uuid.uuid4())
    prev_turn_count = rec.get("turn_count", 0) if rec else 0
    m[session_key] = {"uuid": new_id, "turn_count": prev_turn_count, "awaiting_reset_confirm": False}
    _save_session_map(m)
    return new_id, False


def _bump_turn_count(session_key: str) -> int:
    """이 session_key의 누적 대화 횟수(turn_count)를 1 증가시키고 새 값을
    반환한다(요구사항 2/3 - 매 "클루드코드 [지시]" 호출마다 +1)."""
    m = _load_session_map()
    rec = m.setdefault(session_key, {"uuid": None, "turn_count": 0, "awaiting_reset_confirm": False})
    rec["turn_count"] = int(rec.get("turn_count", 0) or 0) + 1
    _save_session_map(m)
    return rec["turn_count"]


def _set_awaiting_reset_confirm(session_key: str, value: bool) -> None:
    m = _load_session_map()
    rec = m.setdefault(session_key, {"uuid": None, "turn_count": 0, "awaiting_reset_confirm": False})
    rec["awaiting_reset_confirm"] = value
    _save_session_map(m)


def _reset_session(session_key: str) -> None:
    """session_key를 세션 맵에서 완전히 제거한다(uuid/turn_count/확인상태
    전부 폐기) - 다음 호출은 _get_or_create_session_uuid()에서 새 UUID를
    --session-id로 처음부터 부트스트랩한다("예" 응답 또는 "클루드초기화"
    명령, 요구사항 5/6)."""
    m = _load_session_map()
    if session_key in m:
        del m[session_key]
        _save_session_map(m)


def _classify_confirm_reply(text: str) -> str:
    """리셋 확인 질문("...초기화할까요? (예/아니오)")에 대한 응답 텍스트를
    "yes"/"no"/"ambiguous" 중 하나로 판정한다. 첫 어절만, 구두점을 뗀 뒤
    _YES_WORDS/_NO_WORDS와 "완전 일치"로만 비교한다 - 부분/접두어 일치를
    허용하면 "예약 취소해줘"의 "예약"이 "예"로 오인되는 등 실제 지시문을
    확인 응답으로 잘못 삼킬 위험이 있어(요구사항 5의 "애매한 응답" 처리
    판단 근거), 일부러 엄격하게 잡았다."""
    words = text.strip().split()
    if not words:
        return "ambiguous"
    first = words[0].strip(".,!?~…·\"'")
    if first in _YES_WORDS:
        return "yes"
    if first in _NO_WORDS:
        return "no"
    return "ambiguous"


def build_command(
    instruction: str,
    session_uuid: Optional[str] = None,
    resume_existing: bool = False,
    agent_name: Optional[str] = None,
) -> list[str]:
    """헤드리스 Claude 호출. session_uuid가 없으면 기존과 동일한 완전
    무상태 1회성 호출(--no-session-persistence). session_uuid가 있으면
    resume_existing에 따라 그 세션을 새로 만들거나(--session-id)
    이어간다(--resume). agent_name을 주면 그 이름의 커스텀 에이전트
    정의(예: 전역 ~/.claude/agents/<name>.md)로 --agent를 지정한다
    (2026-09-25 도입 - "플로우개발" 명령이 flow-dev-lead 에이전트로
    뜨도록 하는 용도)."""
    cmd = [
        "claude",
        "-p",
        instruction,
        "--permission-mode",
        "acceptEdits",
        "--output-format",
        "json",
    ]
    if session_uuid:
        cmd += (["--resume", session_uuid] if resume_existing else ["--session-id", session_uuid])
    else:
        cmd += ["--no-session-persistence"]
    if agent_name:
        cmd += ["--agent", agent_name]
    if MAX_BUDGET_USD:
        cmd += ["--max-budget-usd", str(MAX_BUDGET_USD)]
    return cmd


def _run_claude_subprocess(cmd: list[str]) -> tuple[Optional[subprocess.CompletedProcess], Optional[dict[str, Any]]]:
    """subprocess.run 실행 + 타임아웃/실행파일-미발견 에러를 표준 실패
    dict로 변환. 성공 시 (proc, None), 실패 시 (None, 실패dict)를 돌려준다."""
    try:
        proc = subprocess.run(
            cmd, cwd=CWD, capture_output=True, text=True, timeout=TIMEOUT_SEC,
        )
        return proc, None
    except subprocess.TimeoutExpired:
        return None, {
            "ok": False, "text": "", "cost": 0.0, "duration_ms": 0,
            "raw_error": f"시간 초과 ({TIMEOUT_SEC}초)",
        }
    except FileNotFoundError:
        return None, {
            "ok": False, "text": "", "cost": 0.0, "duration_ms": 0,
            "raw_error": (
                "claude 실행파일을 찾을 수 없습니다 - "
                "'where claude'로 PATH 확인 필요 (cwd는 존재함)"
            ),
        }


def run_claude(
    instruction: str,
    session_key: Optional[str] = None,
    agent_name: Optional[str] = None,
) -> dict[str, Any]:
    """블로킹 호출(subprocess.run) - 반드시 asyncio.to_thread로 감싸서 호출할
    것(이벤트루프를 막지 않기 위해). 반환: {"ok","text","cost","duration_ms","raw_error"}."""
    if not os.path.isdir(CWD):
        return {
            "ok": False, "text": "", "cost": 0.0, "duration_ms": 0,
            "raw_error": f"cwd 디렉터리가 없습니다: {CWD}",
        }

    session_uuid = None
    resume_existing = False
    if session_key:
        session_uuid, resume_existing = _get_or_create_session_uuid(session_key)

    cmd = build_command(instruction, session_uuid, resume_existing, agent_name)
    proc, err = _run_claude_subprocess(cmd)
    if err is not None:
        return err

    # 세션이 있다고 기록해뒀는데(SESSION_MAP_PATH) 실제로는 claude 쪽 세션
    # 기록이 없어졌을 수 있다(사용자가 직접 지웠거나, claude 자체의 보관
    # 정책으로 만료된 경우 등) - --resume이 "No conversation found"로 실패하면
    # 한 번만 --session-id로 새로 부트스트랩해서 자동 복구한다.
    if (
        resume_existing
        and proc.returncode != 0
        and "No conversation found" in (proc.stderr or "")
    ):
        log.warning(
            "session_key=%s: 저장된 세션(%s)을 찾을 수 없어 새로 부트스트랩합니다",
            session_key, session_uuid,
        )
        cmd = build_command(instruction, session_uuid, resume_existing=False, agent_name=agent_name)
        proc, err = _run_claude_subprocess(cmd)
        if err is not None:
            return err

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


async def run_and_callback(
    job_id: str, instruction: str, callback_chat_id: Optional[str],
    session_key: Optional[str] = None, agent_name: Optional[str] = None,
) -> None:
    """백그라운드 태스크(asyncio.create_task로 등록, HTTP 응답과 무관하게
    돈다). claude -p 실행 자체만 락으로 직렬화 - 동시에 여러 명령이 접수돼도
    한 번에 하나씩 순서대로 실행된다(요청은 즉시 접수되어 대기열에 쌓임).
    session_key는 claude -p가 실제로 끝날 때까지만 busy 처리한다(그 뒤의
    report_done/turn_count 집계까지 묶으면 다음 요청이 불필요하게 더
    오래 막힌다)."""
    job = active_jobs.get(job_id)
    try:
        async with _lock:
            r = await asyncio.to_thread(run_claude, instruction, session_key, agent_name)
    finally:
        if session_key:
            _busy_session_keys.discard(session_key)

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

    # 대화 횟수 누적 + 임계값 배수 도달 시 리셋 확인 질문(요구사항 2/3/4) -
    # session_key가 있는(=지속 세션) 호출에만 적용한다. 정상 응답을 먼저
    # 보낸 뒤(위 report_done) 이어서 두 번째 fire-and-forget 콜백을 한 번
    # 더 보내는 구조 - 같은 report_done()을 그대로 재사용하므로 rest.sce
    # 쪽에 새 배선이 전혀 필요 없다(기존 처리::FLOW.클루드코드완료처리 →
    # 전송.클루드코드완료텔레그램전송 파이프라인이 그대로 처리). 호출 자체가
    # 실패(ok=False)했어도 claude -p가 한 번 돌아 컨텍스트가 늘었을 수
    # 있으므로 동일하게 +1 한다.
    if session_key:
        turn_count = _bump_turn_count(session_key)
        if RESET_TURN_THRESHOLD > 0 and turn_count % RESET_TURN_THRESHOLD == 0:
            _set_awaiting_reset_confirm(session_key, True)
            confirm_text = (
                f"이 세션이 {turn_count}회 대화를 이어왔습니다. "
                f"문맥이 길어졌는데 초기화할까요? (예/아니오)"
            )
            await report_done(callback_chat_id, confirm_text, True)

    # 완료 직후 짧은 유예(30초) 후 레지스트리에서 제거 - 무상태 원칙과 완료
    # 직후 조회 가능성을 둘 다 만족시키기 위함.
    await asyncio.sleep(30)
    active_jobs.pop(job_id, None)


async def handle_reset_confirm_reply(
    job_id: str, session_key: str, instruction: str, callback_chat_id: Optional[str],
) -> None:
    """session_key가 "리셋할까요? (예/아니오)" 확인 대기 상태(요구사항 4)일
    때, 다음 "클루드코드 [지시]" 호출이 오면 execute_agent()가 claude -p를
    돌리는 대신 이 함수로 우회시킨다(요구사항 5) - 그 instruction 텍스트를
    새 지시문이 아니라 확인 질문에 대한 응답으로 해석한다. claude -p를
    전혀 실행하지 않으므로 비용이 들지 않는다.

    "아니오" 처리 방침(요구사항 5의 "판단해서 정하라"는 부분): 확인 플래그만
    내리고 이번 메시지 자체는 소비한다 - 뒤에 실제 지시문이 더 있어도
    이어서 실행하지 않는다. "아니오" 한 단어짜리 응답 뒤에 우연히 붙은
    텍스트를 진짜 지시로 오인해 예상 못한 비용/부작용을 만드는 것보다,
    사용자가 다음 메시지로 원하는 지시를 다시 명확히 보내게 하는 쪽이
    더 안전하다고 판단했다.

    애매한 응답(예/아니오 둘 다 아님) 처리 방침: 확인 대기 상태를 그대로
    유지한 채 다시 물어본다 - 지시문인지 확인 응답인지 불확실한 텍스트를
    섣불리 "새 지시"로 실행해버리면(claude -p 비용 발생) 사용자가 의도치
    않은 과금/부작용을 겪을 수 있고, 반대로 "아니오"로 임의 해석해도
    사용자 의도와 다를 수 있다 - 명확한 신호가 올 때까지 기다리는 게
    가장 보수적이고 예측 가능한 선택이다."""
    verdict = _classify_confirm_reply(instruction)
    job = active_jobs.get(job_id)

    if verdict == "yes":
        _reset_session(session_key)
        reply = "초기화했습니다. 다음 대화부터 새 세션으로 시작합니다."
    elif verdict == "no":
        _set_awaiting_reset_confirm(session_key, False)
        reply = "계속 이어갑니다."
    else:
        reply = (
            '"예" 또는 "아니오"로 답해주세요. '
            '(문맥을 초기화하려면 "예", 계속 이어가려면 "아니오")'
        )

    if job is not None:
        job["finished_at"] = time.time()
        job["ok"] = True
        job["cost"] = 0.0
        job["duration_ms"] = 0

    log.info("[%s] session_key=%s 리셋 확인 응답 처리: %s", job_id, session_key, verdict)
    await report_done(callback_chat_id, reply, True)

    await asyncio.sleep(30)
    active_jobs.pop(job_id, None)


async def _notify_busy(job_id: str, callback_chat_id: Optional[str]) -> None:
    """이미 처리 중인 session_key로 또 요청이 들어왔을 때(응답이 늦어 보여
    사용자가 같은 지시를 재전송하는 경우 등) - claude -p를 새로 실행하지
    않고 즉시 안내만 보낸다. 비용 없음."""
    job = active_jobs.get(job_id)
    if job is not None:
        job["finished_at"] = time.time()
        job["ok"] = True
        job["cost"] = 0.0
        job["duration_ms"] = 0

    await report_done(callback_chat_id, "이전 요청이 아직 처리 중입니다. 완료되면 알려드릴게요.", True)

    await asyncio.sleep(30)
    active_jobs.pop(job_id, None)


async def do_reset_and_notify(job_id: str, session_key: str, callback_chat_id: Optional[str]) -> None:
    """"클루드초기화" 명령(요구사항 6) - 확인 절차 없이 즉시 session_key를
    리셋하고 완료 메시지만 보낸다. claude -p를 실행하지 않으므로 비용이
    들지 않는다."""
    _reset_session(session_key)
    job = active_jobs.get(job_id)
    if job is not None:
        job["finished_at"] = time.time()
        job["ok"] = True
        job["cost"] = 0.0
        job["duration_ms"] = 0

    log.info("[%s] session_key=%s 즉시 초기화(클루드초기화 명령)", job_id, session_key)
    await report_done(callback_chat_id, "초기화했습니다. 다음 대화부터 새 세션으로 시작합니다.", True)

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
    session_key = (req.get("session_key") or "").strip() or None
    agent_name = (req.get("agent_name") or "").strip() or None

    if not instruction:
        return JSONResponse(status_code=400, content={"RESULT": "1", "error": "instruction이 비어 있습니다"})

    job_id = f"job-{uuid.uuid4().hex[:12]}"

    # session_key가 "리셋할까요?" 확인 대기 상태면, 이 instruction은 새
    # 지시가 아니라 그 확인에 대한 예/아니오 응답으로 취급한다(요구사항 5) -
    # claude -p를 돌리지 않고 handle_reset_confirm_reply()로 우회.
    if session_key and _get_session_record(session_key).get("awaiting_reset_confirm"):
        active_jobs[job_id] = {
            "instruction": instruction,
            "callback_chat_id": callback_chat_id,
            "session_key": session_key,
            "agent_name": agent_name,
            "kind": "reset_confirm_reply",
            "started_at": time.time(),
        }
        log.info(
            "[%s] 리셋 확인 응답으로 접수 (callback_chat_id=%s, session_key=%s): %s",
            job_id, callback_chat_id, session_key, instruction[:80],
        )
        asyncio.create_task(handle_reset_confirm_reply(job_id, session_key, instruction, callback_chat_id))
        return JSONResponse(content={"RESULT": "0", "job_id": job_id})

    # 같은 session_key로 claude -p가 지금 실행 중이면(응답이 늦어 보여
    # 사용자가 같은 지시를 재전송한 경우 등) 또 실행시키지 않고 "처리 중"
    # 안내만 보낸다 - _lock은 순서만 보장할 뿐 이 중복 실행 자체는 막지
    # 않으므로(위 _busy_session_keys 정의 참고) 별도로 확인해야 한다.
    if session_key and session_key in _busy_session_keys:
        active_jobs[job_id] = {
            "instruction": instruction,
            "callback_chat_id": callback_chat_id,
            "session_key": session_key,
            "kind": "busy_reject",
            "started_at": time.time(),
        }
        log.info(
            "[%s] session_key=%s 이미 처리 중 - 중복 요청으로 보고 안내만 전송: %s",
            job_id, session_key, instruction[:80],
        )
        asyncio.create_task(_notify_busy(job_id, callback_chat_id))
        return JSONResponse(content={"RESULT": "0", "job_id": job_id})

    if session_key:
        _busy_session_keys.add(session_key)

    active_jobs[job_id] = {
        "instruction": instruction,
        "callback_chat_id": callback_chat_id,
        "session_key": session_key,
        "agent_name": agent_name,
        "started_at": time.time(),
    }
    log.info(
        "[%s] 작업 접수 (callback_chat_id=%s, session_key=%s, agent_name=%s): %s",
        job_id, callback_chat_id, session_key, agent_name, instruction[:80],
    )

    asyncio.create_task(run_and_callback(job_id, instruction, callback_chat_id, session_key, agent_name))

    return JSONResponse(content={"RESULT": "0", "job_id": job_id})


@app.put("/notebookflow/agent/reset")
@app.post("/notebookflow/agent/reset")
async def reset_agent(request: Request):
    """"클루드초기화" Telegram 명령용 엔드포인트(요구사항 6). 요청 스키마:
    {"session_key": "...", "callback_chat_id": "..."(선택)}. 확인 절차 없이
    즉시 session_key를 리셋하고 완료 메시지만 콜백으로 보낸다."""
    try:
        req = await request.json()
    except Exception:
        return JSONResponse(status_code=400, content={"RESULT": "1", "error": "JSON parse 실패"})

    if not isinstance(req, dict):
        return JSONResponse(
            status_code=400,
            content={"RESULT": "1", "error": "요청 body는 JSON object여야 합니다"},
        )

    session_key = (req.get("session_key") or "").strip() or None
    callback_chat_id = req.get("callback_chat_id") or None

    if not session_key:
        return JSONResponse(status_code=400, content={"RESULT": "1", "error": "session_key가 비어 있습니다"})

    job_id = f"job-{uuid.uuid4().hex[:12]}"
    active_jobs[job_id] = {
        "session_key": session_key,
        "callback_chat_id": callback_chat_id,
        "kind": "reset_command",
        "started_at": time.time(),
    }
    log.info("[%s] 클루드초기화 명령 접수 (session_key=%s)", job_id, session_key)

    asyncio.create_task(do_reset_and_notify(job_id, session_key, callback_chat_id))

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
