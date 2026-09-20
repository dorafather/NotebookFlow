"""
File RA - 순수 무상태(stateless) 명령 실행 서버 (FastAPI + uvicorn, 포트 8425).

업무지침_FileRA_동시성재설계.md + 설계_2차확정_RA무상태화_세션생애주기.md 구현
(dsl-process-developer, 2026-08-30).

⚠️ 이전 버전(업무지침_FileRA_HTTP버전.md 구현, git 이력 참고)과 방향이 완전히
바뀌었다:
    - 예전: File RA가 watch_list.json/내부 스케줄러로 "언제 스캔할지" 스스로
      판단하고, NotebookFlow.exe에 PUT을 쏘는 능동적 HTTP 클라이언트였다.
    - 지금: File RA는 "언제"를 전혀 모른다. 명령이 오면(POST /command) 그
      순간 실행하고 결과만 돌려주는 순수 함수 서버다. "언제 부를지"는 전부
      NotebookFlow.exe의 filesystem_watch.sce(자기재무장 타이머 세션)가
      결정한다. watch_list.json, fingerprint_state.json, 내부 스케줄러
      스레드는 전부 폐기 - 이 파일 전체가 완전히 새로 작성됨.

무상태 원칙:
    - Job은 메모리에만 존재(active_jobs 딕셔너리), 디스크에 아무것도 안 남는다.
    - 프로세스가 재시작되면 진행 중이던 Job은 그냥 사라진다 - 정상 동작이다
      (호출자인 NotebookFlow가 재시작 후 다시 명령을 내리는 구조를 전제).
    - "삭제"는 명령 목록에 없다(설계 문서 4번 - 되돌릴 수 없는 작업이라 범위
      밖으로 명시적으로 제외됨). 이 파일에 삭제 관련 코드가 있으면 버그다.

동시성 모델: asyncio 협조적 멀티태스킹(Go의 goroutine ↔ asyncio.Task, Go
런타임 스케줄러 ↔ asyncio 이벤트루프). 각 배치 작업(Job)을 asyncio.Task로
등록하면 이벤트루프가 자동으로 여러 Job을 번갈아 실행한다 - 별도 라운드로빈
로직을 직접 구현하지 않는다. BATCH_SIZE(100)개마다 progress 보고 +
asyncio.sleep(0)으로 제어권을 양보해 다른 Job/다른 요청이 끼어들 수 있게
한다. active_jobs 딕셔너리는 단일 이벤트루프 안에서만 접근되므로(코루틴
사이에 await 지점 없이는 서로 안 끼어듦) 별도 락이 불필요하다.

명령 목록(설계 2차확정 4번, 확정) - 삭제는 이번 범위에서 완전히 제외:
    파일/폴더 조작: 이동, 복사, 이름변경, 폴더생성, 압축, 압축해제
    조회(읽기 전용): 목록조회, 존재확인, 정보조회
    제어: 중단(진행 중인 job_id 취소)

filesystem_watch.sce와의 계약(설계 결정, dsl-process-developer 2026-08-30):
    자기재무장 세션은 매 tick마다 "원본폴더 전체에 명령을 적용"이라는 배치
    하나만 호출한다(예: 명령=이동, 원본경로=<폴더>, 대상경로=<폴더>) - 파일
    목록을 NotebookFlow가 미리 골라서 넘기지 않는다. 이렇게 설계한 이유:
      1) 이 엔진(EXEPARAM.cpp)의 전송메시지.PARAMS.xxx = 수신메시지.PARAMS.yyy
         할당은 KSTRING 기반 스칼라 문자열 복사만 지원한다(오늘 세션 소스
         확인) - RA가 돌려준 파일 배열을 세션 변수에 담았다가 다음 RA 호출의
         PARAMS로 그대로 재주입하는 건 이 엔진에서 검증되지 않은(사실상
         불가능한) 경로라서 피했다.
      2) "이동" 계열 명령은 원본 폴더에서 파일을 실제로 들어내므로, 자연히
         멱등적이다 - 이미 옮겨진 파일은 다음 tick의 스캔에서 아예 안 보인다.
         그래서 File RA가 "이미 처리한 파일"을 기억할 필요 자체가 없다(예전
         버전의 fingerprint_state.json이 하던 일이 통째로 불필요해짐) -
         RA 무상태 원칙과 완전히 맞아떨어지는 부수 효과.
    따라서 File RA의 "이동"/"복사" 명령은 파일 목록(items) 대신 원본 폴더
    경로 하나를 받고, 스캔(os.scandir)해서 그 안의 모든 파일을 Job.items로
    잡아 처리한다. "목록조회"만 예외적으로 진짜 파일 배열을 응답 PARAMS에
    싣는다(그 결과를 다음 RA 호출로 다시 넣지 않고, 사람/LLM이 읽는 최종
    응답이라는 전제이므로 위 1)번 제약과 무관하다).

진행 보고/완료 콜백(PUT NotebookFlow.exe:18099/flow, fire-and-forget,
httpx.AsyncClient 비동기 - 이벤트루프를 막지 않는다):
    File RA는 인바운드(POST /command)에서는 명령의 PARAMS 내용만 받는
    클린룸 RA다(SCE_EVENT/AS_ID/SCE_ID 같은 봉투 개념을 모른다 - Main.cpp의
    App::ACTION()이 RestParam::BODY()로 PARAMS만 뽑아 보내주기 때문).
    그런데 진행 상황을 NotebookFlow에 되돌려주려면(오래 걸리는 배치 도중
    여러 번 PUT) 그 세션의 AS_ID를 알아야 한다 - 그래서 명령 요청 바디에
    "callback_as_id"라는 필드 하나를 "PARAMS 안의 평범한 데이터"로 실어
    보낸다(.sce가 전송메시지.PARAMS.callback_as_id = 세션.AS_ID로 채움).
    File RA는 이 필드를 NotebookFlow의 SCE_ID/AS_ID 프로토콜 이름으로
    알아서가 아니라, "콜백 보낼 때 쓸 opaque 문자열 토큰"으로만 다룬다 -
    클린룸 분리 원칙과 충돌하지 않는다(같은 재사용 패턴이 File RA HTTP버전
    구현의 put_flow()에도 있었음 - 그 "PUT 바디 JSON 스키마 구성" 코드를
    그대로 재사용했다. 아래 build_flow_action_body() 참고).
"""
from __future__ import annotations

import asyncio
import logging
import os
import shutil
import time
import uuid
import zipfile
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Optional

from contextlib import asynccontextmanager

import httpx
from fastapi import FastAPI
from fastapi.responses import JSONResponse
from pydantic import BaseModel

# ── 상수 ─────────────────────────────────────────────────────────────────────
HTTP_PORT = 8425
FLOW_BASE = "http://127.0.0.1:18099"
# claude-code 2026-09-06 (업무지침_REST브릿지_도메인기반분기_경로네임스페이스.md) -
# 내부 전용 경로 네임스페이스 예약("/notebookflow/" 접두어)
FLOW_PUT_URL = f"{FLOW_BASE}/notebookflow/flow"
BATCH_SIZE = 100  # 업무지침 요구사항 5 - 1차는 고정값, 설정 가능하게 빼지 않는다.
FLOW_PUT_TIMEOUT = 5.0  # 진행 보고는 fire-and-forget이므로 짧게 - 실패해도 Job 진행에 영향 없음.

LOG_PATH = Path(__file__).resolve().parent / "file_ra.log"
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(threadName)s] %(message)s",
    handlers=[
        logging.FileHandler(LOG_PATH, encoding="utf-8"),
        logging.StreamHandler(),
    ],
)
log = logging.getLogger("file-ra")

# 진행 보고용 공유 httpx 클라이언트(비동기, 이벤트루프 안 막음). 서버 시작시 생성.
_http_client: Optional[httpx.AsyncClient] = None


@asynccontextmanager
async def lifespan(_app: FastAPI):
    global _http_client
    _http_client = httpx.AsyncClient()
    log.info("File RA 기동 - 포트 %d, 무상태 명령 서버(watch_list.json/내부 스케줄러 없음)", HTTP_PORT)
    yield
    if _http_client:
        await _http_client.aclose()


app = FastAPI(title="File RA (stateless command server)", lifespan=lifespan)


# ── Job 객체 (요구사항 1) ────────────────────────────────────────────────────
# 휘발성 - 메모리에만 존재, 파일로 저장 안 함(RA 무상태 원칙). 프로세스
# 재시작 시 진행 중이던 Job은 사라지는 게 정상(호출자가 다시 명령을 내림).
@dataclass
class Job:
    job_id: str
    kind: str
    items: list
    callback_as_id: Optional[str] = None
    cancelled: bool = False
    processed: int = 0
    total: int = 0
    results: list = field(default_factory=list)  # 항목별 성공/실패 기록(정보조회 등은 결과 자체)
    started_at: float = field(default_factory=time.time)
    error: Optional[str] = None
    extra: dict = field(default_factory=dict)  # 명령별 부가 파라미터(대상경로 등)


# ── Job 레지스트리 (요구사항 2) ──────────────────────────────────────────────
# job_id -> Job. asyncio 단일 이벤트루프 안에서만 접근하므로(코루틴 사이에
# await 지점 없이는 서로 안 끼어듦) 별도 락 불필요 - 업무지침 원문 그대로.
active_jobs: dict[str, Job] = {}


# ── NotebookFlow.exe PUT /flow 콜백 (진행 보고 + 완료 보고) ──────────────────
def build_flow_action_body(action_event: str, callback_as_id: str, params: dict) -> dict:
    """업무지침_FileRA_HTTP버전.md의 put_flow() 바디 스키마를 그대로 재사용
    (dsl-process-developer 2026-08-29 구현분) - SCE_ID는 "IC-"+AS_ID로
    결정론적이다(Flow::procStartReq, FLOW.cpp 176번 줄 szKey.PRINT("IC-%s",...)
    와 동일 규칙)."""
    return {
        "SCE_EVENT": "ACTION",
        "ACTION_EVENT": action_event,
        "AS_ID": callback_as_id,
        "SCE_ID": f"IC-{callback_as_id}",
        "PARAMS": params,
    }


async def report_to_flow(action_event: str, callback_as_id: Optional[str], params: dict) -> None:
    """fire-and-forget 진행/완료 보고. callback_as_id가 없으면(콜백을 원치
    않는 단발성 호출) 조용히 스킵한다. 실패해도(NotebookFlow가 응답 안
    하거나 타임아웃) 예외를 삼키고 로그만 남긴다 - File RA의 배치 처리
    자체가 이 보고 실패로 인해 멈추면 안 된다(업무지침 검증 5 요구사항과
    같은 원칙: 관찰자 쪽 실패가 본 작업에 영향을 주면 안 된다)."""
    if not callback_as_id:
        return
    body = build_flow_action_body(action_event, callback_as_id, params)
    global _http_client
    try:
        assert _http_client is not None
        resp = await _http_client.put(FLOW_PUT_URL, json=body, timeout=FLOW_PUT_TIMEOUT)
        if resp.status_code != 200:
            log.warning("PUT /flow 진행보고 실패(HTTP %s): %s", resp.status_code, resp.text[:300])
    except Exception as e:
        log.warning("PUT /flow 진행보고 실패(연결/타임아웃, 무시): %s", e)


async def report_progress(job: Job) -> None:
    await report_to_flow(
        "fs_job_progress",
        job.callback_as_id,
        {"job_id": job.job_id, "kind": job.kind, "processed": job.processed, "total": job.total},
    )


async def report_done(job: Job) -> None:
    await report_to_flow(
        "fs_job_done",
        job.callback_as_id,
        {
            "job_id": job.job_id,
            "kind": job.kind,
            "processed": job.processed,
            "total": job.total,
            "cancelled": job.cancelled,
            "error": job.error or "",
        },
    )


# ── 파일 단위 작업(원자적 연산 사용 - 검증 4) ────────────────────────────────
def _ensure_parent(dst: Path) -> None:
    dst.parent.mkdir(parents=True, exist_ok=True)


def do_move(src: Path, dst_dir: Path) -> str:
    _ensure_parent(dst_dir / src.name)
    dst = dst_dir / src.name
    os.replace(src, dst)  # os.rename 계열 - 원자적(같은 볼륨 기준). 중단돼도 반쯤 옮겨진 상태 없음.
    return str(dst)


def do_copy(src: Path, dst_dir: Path) -> str:
    _ensure_parent(dst_dir / src.name)
    tmp = dst_dir / (src.name + f".tmp-{uuid.uuid4().hex[:8]}")
    shutil.copy2(src, tmp)  # 임시이름으로 먼저 복사
    dst = dst_dir / src.name
    os.replace(tmp, dst)  # 원자적 rename으로 최종 이름으로 교체 - 복사 중 중단돼도 목적지에 반쪽 파일이 정식 이름으로 안 남음
    return str(dst)


def do_rename(src: Path, name_template: str) -> str:
    """검증 1(동시 다발 Job)에서 이동/복사와 나란히 배치로 돌리기 위해
    이름변경도 "폴더 하나 통째로" 처리하는 Job 명령으로 통일했다(파일
    하나짜리 단발 rename이 아니라, name_template의 {stem}/{ext}/{name}
    자리표시자를 각 파일명으로 채워 폴더 안 전부를 일괄 개명한다)."""
    newname = name_template.format(name=src.name, stem=src.stem, ext=src.suffix.lstrip("."))
    dst = src.parent / newname
    os.replace(src, dst)
    return str(dst)


def do_zip_one(zf: zipfile.ZipFile, src: Path, arcname: str) -> None:
    zf.write(src, arcname)


# ── Job 처리 코루틴 (요구사항 3, asyncio.create_task로 등록) ─────────────────
async def process_job(job: Job) -> None:
    """각 배치 작업을 하나의 asyncio.Task로 등록 - 이벤트루프가 자동으로
    여러 Job을 번갈아 실행한다(Go goroutine ↔ asyncio.Task 대응, 업무지침
    원문). BATCH_SIZE(100)개마다 progress 보고 + sleep(0)으로 제어권 양보
    (다른 Job/다른 요청이 여기서 끼어듦) + 중단 신호 확인."""
    try:
        for i, item in enumerate(job.items):
            if job.cancelled:
                break
            try:
                job.results.append(_apply_one(job.kind, item, job.extra))
            except Exception as e:
                job.results.append({"item": str(item), "ok": False, "error": str(e)})
                log.exception("[%s] 항목 처리 실패: %s", job.job_id, item)
            job.processed = i + 1
            if job.processed % BATCH_SIZE == 0:
                await report_progress(job)
                await asyncio.sleep(0)  # 이벤트루프에 제어권 양보 - 다른 Job이 여기서 끼어듦
                if job.cancelled:
                    break
        if job.cancelled:
            log.info("[%s] 중단됨 (처리 %d/%d)", job.job_id, job.processed, job.total)
        else:
            log.info("[%s] 완료 (처리 %d/%d)", job.job_id, job.processed, job.total)
    except Exception as e:
        job.error = str(e)
        log.exception("[%s] Job 처리 중 예외", job.job_id)
    finally:
        await report_done(job)
        # 완료/취소/에러 무엇이든 결과가 확정되면 레지스트리에서 제거 -
        # 무상태 원칙(Job은 진행 중에만 메모리에 존재). 완료 직후 상태를
        # 조회하고 싶을 수 있으니 짧게(30초) 유예 후 제거한다.
        await asyncio.sleep(30)
        active_jobs.pop(job.job_id, None)


def _apply_one(kind: str, item: Any, extra: dict) -> dict:
    """파일 하나에 대해 명령을 적용. 반환값은 Job.results에 쌓이는 항목별 결과."""
    if kind == "이동":
        src = Path(item)
        dst_dir = Path(extra["대상경로"])
        newpath = do_move(src, dst_dir)
        return {"item": str(src), "ok": True, "result": newpath}
    if kind == "복사":
        src = Path(item)
        dst_dir = Path(extra["대상경로"])
        newpath = do_copy(src, dst_dir)
        return {"item": str(src), "ok": True, "result": newpath}
    if kind == "이름변경":
        src = Path(item)
        newpath = do_rename(src, extra["새이름"])
        return {"item": str(src), "ok": True, "result": newpath}
    if kind == "정보조회":
        p = Path(item)
        st = p.stat()
        return {
            "item": str(p),
            "ok": True,
            "result": {"size": st.st_size, "mtime": st.st_mtime, "is_dir": p.is_dir()},
        }
    if kind == "존재확인":
        p = Path(item)
        return {"item": str(p), "ok": True, "result": p.exists()}
    raise ValueError(f"_apply_one에서 처리할 수 없는 명령 종류: {kind}")


# ── 명령별 Job 준비 (스캔은 여기서, 실제 처리는 process_job 코루틴에서) ──────
def _scan_folder(folder: Path) -> list[Path]:
    if not folder.is_dir():
        return []
    return sorted(p for p in folder.iterdir() if p.is_file())


# ── 규칙 자체 스케줄러 (업무지침_FileRA_자체스케줄링_중지재개전파.md) ────────
# 설계 갈림길 ② 채택: File RA가 규칙목록을 메모리에 들고 스스로 스케줄링한다
# (Flow의 자기재무장 타이머 세션 방식은 완전 폐기). 무상태 원칙 그대로 -
# rules는 메모리에만 있고 재시작하면 사라진다(Tray가 다시 규칙목록을 보내는
# 것을 전제, 요구사항/범위 밖 참고).
DEFAULT_RULE_PERIOD_SEC = 30.0
SCHEDULER_TICK_SEC = 5.0

# [업무지침_FileRA_안전장치_빈조건방지_배치상한.md] 2026-09-03 실제 사고
# (조건 필터 미적용으로 Downloads 폴더 전체가 이동 대상이 됨) 재발 방지용
# 삼중 안전장치의 값 2개 - 1차는 고정값(범위 밖 - 설정 가능하게 빼지 않음).
MIN_RULE_PERIOD_SEC = 30.0
BATCH_LIMIT_PER_TICK = 5


@dataclass
class Rule:
    원본디렉토리: str
    대상디렉토리: str
    명령: str
    주기: float
    파일명포함: str = ""  # 비어있으면 필터 없음(원본디렉토리 전체) - 2026-09-03
                          # 사고 이후 추가: Tray의 "조건" 필드가 이전엔
                          # 미리보기에만 적용되고 실제 이동/복사는 항상
                          # 폴더 전체를 대상으로 해서, 실사용자 Downloads
                          # 폴더 전체가 옮겨지는 사고가 실제로 발생함(사용자
                          # 확인). 이제 스케줄 실행 시에도 동일하게 적용한다.
    마지막실행: float = 0.0
    처리된파일: set = field(default_factory=set)  # [안전장치 2] "복사"는 원본이
        # 남으므로, 배치 상한으로 여러 틱에 걸쳐 처리할 때 이미 복사한 파일을
        # 또 복사하지 않도록 이 규칙 인스턴스 안에서만 기억한다("이동"은 원본이
        # 사라지므로 재스캔에서 자연히 빠져 이 추적이 필요 없음). set_rules()가
        # 매번 새 Rule 인스턴스를 만들므로(완전 교체 원칙), 규칙목록이 갱신되면
        # 자동으로 초기화된다 - 별도 리셋 로직 불필요.


class RuleScheduler:
    """중지/재개는 전체 스케줄러 단위(규칙 개별 아님 - Tray의 단일 버튼
    UX와 일치, 업무지침 확정 사항)."""

    def __init__(self) -> None:
        self.rules: list[Rule] = []
        self.paused: bool = False
        self._task: Optional[asyncio.Task] = None

    def set_rules(self, raw_rules: list[dict]) -> None:
        # "완전 교체"(업무지침 확정) - 이전 규칙의 마지막실행 이력을 안 옮기고
        # 전부 0으로 새로 시작한다(단순함 우선, 과설계 금지 원칙).
        self.rules = []
        for r in raw_rules:
            period = float(r.get("주기") or DEFAULT_RULE_PERIOD_SEC)
            # [안전장치 3] 최소 주기 강제 - Tray가 이미 30초 미만을 막지만,
            # 저장 시점 검사를 우회하는 경로(과거 데이터, 다른 클라이언트)에
            # 대비한 방어적 이중 검사 - 거부하지 않고 보정 후 그대로 실행한다
            # (Tray와 일관된 정책: 저장을 막기보다 안전한 값으로 고쳐서 진행).
            if period < MIN_RULE_PERIOD_SEC:
                log.warning(
                    "RuleScheduler: 규칙 주기(%.1f초)가 최소값(%.1f초) 미만이라 보정함 - %s",
                    period, MIN_RULE_PERIOD_SEC, r.get("원본디렉토리", ""),
                )
                period = MIN_RULE_PERIOD_SEC
            self.rules.append(Rule(
                원본디렉토리=r.get("원본디렉토리", ""),
                대상디렉토리=r.get("대상디렉토리", ""),
                명령=r.get("명령", "이동"),
                주기=period,
                파일명포함=r.get("파일명포함") or "",
            ))
        log.info("RuleScheduler: 규칙목록 교체 (%d건)", len(self.rules))
        self._ensure_started()

    def pause(self) -> None:
        self.paused = True
        log.info("RuleScheduler: 일시정지")

    def resume(self) -> None:
        self.paused = False
        log.info("RuleScheduler: 재개")

    def _ensure_started(self) -> None:
        if self._task is None or self._task.done():
            self._task = asyncio.create_task(self.run_forever())

    async def run_forever(self) -> None:
        log.info("RuleScheduler: 백그라운드 루프 시작(틱 %ss)", SCHEDULER_TICK_SEC)
        while True:
            if not self.paused:
                now = time.time()
                for rule in self.rules:
                    if now - rule.마지막실행 >= rule.주기:
                        # [안전장치 2, 방식 A] 이번 틱에서 조건에 맞는 파일을
                        # 다 처리하지 못했으면(배치 상한 초과분이 남음)
                        # 마지막실행을 갱신하지 않는다 - 그러면 다음 스케줄러
                        # 틱(SCHEDULER_TICK_SEC=5초 후)에 곧바로 다시 조건을
                        # 재평가해 이어서 처리한다("주기"만큼 기다리지 않고
                        # 밀린 걸 먼저 소진). 다 처리했을 때만(또는 처리할
                        # 대상이 원래 없었을 때만) 갱신해 정상적으로 "주기"
                        # 간격을 지킨다.
                        done = await self.execute_rule(rule)
                        if done:
                            rule.마지막실행 = now
            await asyncio.sleep(SCHEDULER_TICK_SEC)

    async def execute_rule(self, rule: Rule) -> bool:
        """이번 틱에서 이 규칙에 대해 더 할 일이 남았으면 False, 완전히
        끝났으면(또는 애초에 처리할 게 없었으면) True를 반환한다 - 호출자
        (run_forever)가 이 값으로 마지막실행 갱신 여부를 결정한다(안전장치 2,
        방식 A)."""
        # 기존 이동/복사 Job/배치 파이프라인을 그대로 재사용한다(요구사항 2 -
        # 새 실행 경로를 만들지 않음, 동시성/디스크 경합 방지 원칙 그대로
        # 적용됨 - process_job()이 이 Job도 다른 수동 명령 Job과 똑같이
        # asyncio.Task로 실행되므로 서로 자연스럽게 인터리빙된다).
        if rule.명령 not in ("이동", "복사"):
            log.warning("RuleScheduler: 지원하지 않는 규칙 명령 '%s' 건너뜀", rule.명령)
            return True

        # [안전장치 1] 조건(파일명포함)이 비어있는 규칙은 실행하지 않는다 -
        # Tray가 저장 시점에 이미 막지만, 과거 저장된 데이터나 다른
        # 클라이언트가 우회할 수 있으므로 실행 시점에도 방어적으로 한 번 더
        # 검사한다(2026-09-03 사고 재발 방지 삼중 안전장치 중 하나).
        if not rule.파일명포함:
            log.warning(
                "RuleScheduler: 조건(파일명포함)이 비어있는 규칙은 안전을 위해 건너뜀 - %s -> %s",
                rule.원본디렉토리, rule.대상디렉토리,
            )
            return True

        items = [str(p) for p in _scan_folder(Path(rule.원본디렉토리))]
        items = [p for p in items if rule.파일명포함 in Path(p).name]
        if rule.명령 == "복사":
            # [안전장치 2 - "확인 필요" 항목] "복사"는 원본이 그대로 남아
            # 다음 틱 재스캔에서도 계속 매칭되므로, 이 규칙이 이미 처리한
            # 파일은 제외해야 배치 상한으로 나뉜 처리가 같은 5개를 무한
            # 반복하지 않고 실제로 새 파일 쪽으로 진행한다("이동"은 원본이
            # 사라지므로 이 처리가 필요 없음).
            items = [p for p in items if p not in rule.처리된파일]
        if not items:
            return True

        # [안전장치 2] 한 틱당 최대 BATCH_LIMIT_PER_TICK개만 처리 - 조건에
        # 맞는 파일이 아무리 많아도 한 번에 다 처리되는 상황 자체를 구조적으로
        # 없앤다(2026-09-03 사고 재발 방지). 나머지는 남겨두고 False를
        # 반환해 다음 스케줄러 틱에 이어서 처리되게 한다.
        to_process = items[:BATCH_LIMIT_PER_TICK]
        remaining = len(items) - len(to_process)

        job = _new_job(rule.명령, to_process, None, {"대상경로": rule.대상디렉토리})
        asyncio.create_task(process_job(job))
        if rule.명령 == "복사":
            rule.처리된파일.update(to_process)
        log.info(
            "RuleScheduler: 규칙 실행 [%s] %s -> %s (%d개 처리, %d개 남음)",
            rule.명령, rule.원본디렉토리, rule.대상디렉토리, len(to_process), remaining,
        )
        return remaining == 0


scheduler = RuleScheduler()


class RaCommandParams(BaseModel):
    """실제 명령 필드. 명령마다 필요한 필드가 달라 대부분 Optional로 둔다."""
    명령: str
    job_id: Optional[str] = None          # 중단 명령 때 대상 job_id로 사용
    callback_as_id: Optional[str] = None  # 진행/완료 보고를 받을 NotebookFlow 세션의 AS_ID
    원본경로: Optional[str] = None
    대상경로: Optional[str] = None
    새이름: Optional[str] = None  # 이름변경: {name}/{stem}/{ext} 자리표시자를 쓸 수 있는 템플릿(예: "renamed_{name}")
    압축파일명: Optional[str] = None
    exe_rules: Optional[list[dict]] = None  # 명령=규칙목록일 때만 사용 - Tray가 만든 규칙 배열 그대로


class CommandRequest(BaseModel):
    """claude-code 2026-09-05: 이전엔 PARAMS 최상위 필드 그대로 = 요청 바디
    최상위 필드였다(App::ACTION()이 RestParam::BODY()로 PARAMS 하위 내용만
    최상위 JSON으로 벗겨서 보내는 클린룸 방식). Main.cpp가 "엔진은 JSON을
    판단/가공하지 않는다" 원칙으로 바뀌면서 이제 Flow 봉투(SCE_EVENT/
    ACTION_EVENT/AS_ID/SCE_ID/ADDR 등) 전체를 그대로 릴레이한다 - 그래서
    File RA가 받는 쪽을 맞춘다. 실제 명령 필드는 PARAMS 하위에 그대로 있다."""
    PARAMS: RaCommandParams


def _new_job(kind: str, items: list, callback_as_id: Optional[str], extra: dict) -> Job:
    job_id = f"job-{uuid.uuid4().hex[:12]}"
    job = Job(job_id=job_id, kind=kind, items=items, callback_as_id=callback_as_id,
              total=len(items), extra=extra)
    active_jobs[job_id] = job
    return job


@app.put("/command")
@app.post("/command")
async def handle_command(req: CommandRequest):
    p = req.PARAMS
    cmd = p.명령

    # ── 관리: 규칙목록/일시정지/재개 (업무지침_FileRA_자체스케줄링_
    #    중지재개전파.md) - 즉시 완료되는 동기 명령, Job/asyncio Task
    #    큐를 안 씀(요구사항 - 기존 9종과 성격이 다름) ────────────────────
    if cmd == "규칙목록":
        scheduler.set_rules(p.exe_rules or [])
        return {"RESULT": "0", "status": "완료", "count": len(scheduler.rules)}

    if cmd == "일시정지":
        scheduler.pause()
        return {"RESULT": "0", "status": "일시정지됨"}

    if cmd == "재개":
        scheduler.resume()
        return {"RESULT": "0", "status": "재개됨"}

    # ── 제어: 중단 ───────────────────────────────────────────────────────
    if cmd == "중단":
        job = active_jobs.get(p.job_id or "")
        if job is None:
            return JSONResponse({"RESULT": "1", "status": "job not found", "job_id": p.job_id})
        job.cancelled = True
        return {"RESULT": "0", "status": "중단 신호 전송됨", "job_id": job.job_id}

    # ── 배치형 명령: 폴더 전체 스캔 후 Job 등록, 즉시 응답 + Task로 위임 ──
    if cmd in ("이동", "복사"):
        if not p.원본경로 or not p.대상경로:
            return JSONResponse(
                {"RESULT": "1", "status": "원본경로/대상경로 필수"}, status_code=400
            )
        items = [str(f) for f in _scan_folder(Path(p.원본경로))]
        job = _new_job(cmd, items, p.callback_as_id, {"대상경로": p.대상경로})
        asyncio.create_task(process_job(job))  # 등록만, 블로킹 없음 - 요구사항 3
        log.info("[%s] %s Job 시작: %s -> %s (%d개)", job.job_id, cmd, p.원본경로, p.대상경로, len(items))
        return {"RESULT": "0", "status": "시작됨", "job_id": job.job_id, "total": len(items)}

    if cmd == "이름변경":
        # 검증 1(동시 다발 Job 격리)에서 이동/복사와 나란히 배치로 테스트하기
        # 위해 이름변경도 Job/asyncio.Task 경로로 통일했다(파일 하나짜리
        # 단발 rename이 아니라 원본경로 폴더 전체를 새이름 템플릿으로 일괄
        # 개명 - do_rename()의 {name}/{stem}/{ext} 자리표시자 참고).
        if not p.원본경로 or not p.새이름:
            return JSONResponse({"RESULT": "1", "status": "원본경로/새이름 필수"}, status_code=400)
        items = [str(f) for f in _scan_folder(Path(p.원본경로))]
        job = _new_job(cmd, items, p.callback_as_id, {"새이름": p.새이름})
        asyncio.create_task(process_job(job))
        log.info("[%s] 이름변경 Job 시작: %s (%d개, 템플릿=%s)", job.job_id, p.원본경로, len(items), p.새이름)
        return {"RESULT": "0", "status": "시작됨", "job_id": job.job_id, "total": len(items)}

    if cmd == "폴더생성":
        if not p.원본경로:
            return JSONResponse({"RESULT": "1", "status": "원본경로(생성할 경로) 필수"}, status_code=400)
        Path(p.원본경로).mkdir(parents=True, exist_ok=True)
        return {"RESULT": "0", "status": "완료", "result": p.원본경로}

    if cmd == "압축":
        if not p.원본경로 or not p.압축파일명:
            return JSONResponse({"RESULT": "1", "status": "원본경로/압축파일명 필수"}, status_code=400)
        src_dir = Path(p.원본경로)
        files = _scan_folder(src_dir)
        zip_path = Path(p.압축파일명)
        _ensure_parent(zip_path)
        with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
            for f in files:
                do_zip_one(zf, f, f.name)
        return {"RESULT": "0", "status": "완료", "result": str(zip_path), "count": len(files)}

    if cmd == "압축해제":
        if not p.압축파일명 or not p.대상경로:
            return JSONResponse({"RESULT": "1", "status": "압축파일명/대상경로 필수"}, status_code=400)
        dst_dir = Path(p.대상경로)
        dst_dir.mkdir(parents=True, exist_ok=True)
        with zipfile.ZipFile(p.압축파일명, "r") as zf:
            names = zf.namelist()
            zf.extractall(dst_dir)
        return {"RESULT": "0", "status": "완료", "result": str(dst_dir), "count": len(names)}

    if cmd == "목록조회":
        if not p.원본경로:
            return JSONResponse({"RESULT": "1", "status": "원본경로 필수"}, status_code=400)
        files = _scan_folder(Path(p.원본경로))
        return {"RESULT": "0", "status": "완료", "count": len(files), "files": [str(f) for f in files]}

    if cmd == "존재확인":
        if not p.원본경로:
            return JSONResponse({"RESULT": "1", "status": "원본경로 필수"}, status_code=400)
        return {"RESULT": "0", "status": "완료", "result": Path(p.원본경로).exists()}

    if cmd == "정보조회":
        if not p.원본경로:
            return JSONResponse({"RESULT": "1", "status": "원본경로 필수"}, status_code=400)
        target = Path(p.원본경로)
        if not target.exists():
            return JSONResponse({"RESULT": "1", "status": "not found"}, status_code=200)
        st = target.stat()
        return {
            "RESULT": "0",
            "status": "완료",
            "result": {"size": st.st_size, "mtime": st.st_mtime, "is_dir": target.is_dir()},
        }

    return JSONResponse({"RESULT": "1", "status": f"알 수 없는 명령: {cmd}"}, status_code=400)


@app.get("/jobs/{job_id}")
async def get_job(job_id: str):
    """디버깅/검증용 - 진행 중이거나 완료 후 30초 이내인 Job의 현재 상태 조회.
    무상태 원칙과 무관(Job은 여전히 메모리에만 있고, 재시작하면 사라짐)."""
    job = active_jobs.get(job_id)
    if job is None:
        return JSONResponse({"RESULT": "1", "status": "not found"}, status_code=404)
    return {
        "job_id": job.job_id,
        "kind": job.kind,
        "processed": job.processed,
        "total": job.total,
        "cancelled": job.cancelled,
        "error": job.error,
    }


@app.get("/health")
async def health():
    # 업무지침_Tray_FileFlow탭.md 요구사항 5 - Tray의 "일시정지 전체/재개 전체"
    # 버튼은 낙관적 업데이트가 금지라(자동시작 체크박스 때 확립한 원칙과 동일),
    # 실제 스케줄러 상태를 조회할 방법이 있어야 한다. 이미 있는 /health에
    # 필드 2개만 얹는다(새 엔드포인트 불필요).
    return {
        "status": "ok",
        "active_jobs": len(active_jobs),
        "scheduler_paused": scheduler.paused,
        "scheduler_rule_count": len(scheduler.rules),
    }


if __name__ == "__main__":
    import uvicorn

    uvicorn.run(app, host="127.0.0.1", port=HTTP_PORT, log_level="info")
