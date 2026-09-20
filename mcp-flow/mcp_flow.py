#!/usr/bin/env python3
"""
mcp_flow.py — Claude Code ↔ NotebookFlow MCP 브릿지

mcp-rag.exe(NotebookRAG-release/src/mcp_rag.py)의 검증된 구조를 그대로
재사용한다 — 설정(mcp_tools.json) 기반으로 도구 목록/스키마를 만들고,
코드에는 "요청을 어떻게 조립해서 보낼지"만 남긴다.

[업무지침_mcp_flow_재설계_제안승인방식.md] 2026-08-27 전면 재설계 — 이전
설계(create_rule/list_rules/stop_rule, 세션별로 규칙을 직접 만들어 File
RA에 바로 전달)는 완전히 폐기됐다. 배경: 2026-08-26 실사용 중 조건 필터
미적용으로 실제 개인 문서가 이동되는 사고가 있었고, 그 후 Tray 저장
시점에 삼중 안전장치(빈 조건 방지/배치 상한/최소 주기)를 넣었다. 하지만
자연어 경로(mcp-flow)가 규칙을 직접 만들어버리면 이 Tray 관문을
우회하게 되므로, 같은 위험이 다시 열린다.

새 원칙 — **규칙을 실제로 저장·실행하는 마지막 관문은 항상 Tray
(rules.json)여야 한다.** mcp-flow는 "이런 규칙을 만들고 싶다"는 제안 하나
(propose_rule)만 NotebookFlow의 기존 Tray 관리 세션(AS_ID 고정값
FLOW_AS_ID)에 얹어두고, 승인/거부는 전부 Tray 쪽(user.sce의
대기중요청목록 + Tray의 폴링·팝업·기존 규칙 추가 코드 경로)에서 사용자가
직접 한다. mcp-flow는 승인 여부를 기다리지 않는다(fire-and-forget) —
"제안이 전달됐다"는 것만 확인하고 바로 응답한다.

AS_ID를 세션 ID로 새로 발급하지 않고 FLOW_AS_ID 고정값을 쓰는 이유: 이
제안은 Tray가 이미 열어놓은 그 세션(rules.json을 들고 있는 세션)에
쌓여야만 Tray의 폴링이 찾을 수 있다 — mcp-flow가 자기만의 새 세션을
열면 완전히 다른 세션의 메모리에 쌓여 아무도 못 본다.

준비:
  pip install "mcp<2" httpx --break-system-packages
  # [주의] mcp 2.0.0부터 Server.list_tools()/call_tool() 데코레이터 API가
  # 제거됨(mcp_rag.py의 동일 경고 참고) - 반드시 1.x로 고정 설치할 것.
  mcp_tools.json을 이 스크립트와 같은 디렉터리에 둘 것

Claude Code 등록:
  claude mcp add mcp-flow -s user -- python3 /path/to/mcp_flow.py
"""

from __future__ import annotations

import asyncio
import json
import os
import sys
from pathlib import Path
from typing import Any

import httpx
import mcp.types as types
from mcp.server.lowlevel import Server
from mcp.server.stdio import stdio_server

# 배포 레이아웃: mcp-flow.exe와 mcp_tools.json이 같은 디렉터리(형제) -
# mcp_rag.py처럼 상위/config 구조를 흉내내지 않고 File RA와 같은 평평한
# 배치를 그대로 따른다.
if getattr(sys, "frozen", False):
    APP_DIR = Path(sys.executable).parent
else:
    APP_DIR = Path(__file__).parent
CONFIG_FILE = Path(os.getenv("MCP_TOOLS_FILE", APP_DIR / "mcp_tools.json"))
FLOW_HTTP_HOST = os.getenv("FLOW_HTTP_HOST", "127.0.0.1")
FLOW_HTTP_PORT = int(os.getenv("FLOW_HTTP_PORT", "18099"))
# [업무지침_mcp_flow_재설계_제안승인방식.md] Tray(tray-flow.exe)가 여는
# 세션의 고정 AS_ID와 반드시 같아야 한다 - tray-src/src/FileFlowConfig.h의
# kFileFlowAsId 참고. 둘 중 하나만 바꾸면 제안이 전달돼도 아무도 못 보는
# 상태가 되므로, 바꿀 일이 생기면 두 곳을 항상 같이 바꿀 것.
FLOW_AS_ID = os.getenv("FLOW_AS_ID", "filetray")
TIMEOUT_SEC = 20

TYPE_MAP = {"string": "string", "integer": "integer", "number": "number", "boolean": "boolean"}

# ── 설정 로더 (mcp_rag.py의 fail-fast 원칙 그대로) ───────────────────────────


def load_config(path: Path) -> dict[str, dict]:
    with open(path, encoding="utf-8") as f:
        cfg = json.load(f)
    registry: dict[str, dict] = {}
    for t in cfg.get("tools", []):
        name = t.get("name", "")
        if not name or not t.get("description"):
            raise ValueError(f"{path}: 도구 정의 불완전 — {t}")
        # [업무지침_알람Default시나리오_1단계.md] 2026-09-11 - list_connections/
        # trigger_alert는 propose_rule과 달리 SCE_EVENT/ACTION_EVENT 봉투로
        # Flow 엔진에 PUT하는 기존 flow_put() 경로를 안 쓴다(config/addr GET,
        # rest/:namespace/:action PUT을 직접 호출) - "transport": "direct_http"로
        # 선언한 도구는 이 필드가 없어도 되게 검증을 완화한다(기존 propose_rule
        # 등 명시 안 한 도구는 기존과 동일하게 "flow_envelope"이 기본값이라
        # 검증이 그대로 적용됨 - 회귀 없음).
        transport = t.get("transport", "flow_envelope")
        if transport == "flow_envelope" and (not t.get("sce_event") or not t.get("action_event")):
            raise ValueError(f"{path}: 도구 '{name}'에 sce_event/action_event가 없습니다")
        if name in registry:
            raise ValueError(f"{path}: 도구 이름 '{name}' 중복")
        registry[name] = t
    if not registry:
        raise ValueError(f"{path}: 도구가 하나도 없습니다")
    return registry


def build_input_schema(params: dict) -> dict:
    """MCP inputSchema(JSON Schema) 생성. [주의] 여기 키(pname)는 그대로
    Claude API 도구 스키마의 property 이름이 되므로 ASCII만 허용된다
    (mcp_rag.py의 동일 주석 참고) - 한글이 필요한 내부 필드명은
    mcp_tools.json의 dsl_key로만 매핑한다."""
    properties, required = {}, []
    for pname, p in params.items():
        prop: dict[str, Any] = {
            "type": TYPE_MAP.get(p.get("type", "string"), "string"),
            "description": p.get("description", ""),
        }
        if "enum" in p:
            prop["enum"] = p["enum"]
        if "default" in p:
            prop["default"] = p["default"]
        properties[pname] = prop
        if p.get("required"):
            required.append(pname)
    return {"type": "object", "properties": properties, "required": required}


# ── Flow HTTP 요청 (전체 봉투 - File RA와 동일 규칙) ─────────────────────────


async def flow_put(payload: dict) -> tuple[int, dict]:
    """PUT /notebookflow/flow에 전체 봉투를 그대로 보낸다. 네트워크 오류
    (연결 실패, 타임아웃 등)는 여기서 흡수해야 MCP 서버 자체가 죽지 않는다
    (mcp_rag.py의 http_request()와 동일 원칙)."""
    # claude-code 2026-09-06 (업무지침_REST브릿지_도메인기반분기_경로네임스페이스.md) -
    # 내부 전용 경로 네임스페이스 예약("/notebookflow/" 접두어)
    url = f"http://{FLOW_HTTP_HOST}:{FLOW_HTTP_PORT}/notebookflow/flow"
    try:
        async with httpx.AsyncClient(timeout=TIMEOUT_SEC) as client:
            resp = await client.put(url, json=payload)
        return resp.status_code, resp.json()
    except Exception as exc:
        return 0, {"_error": str(exc)}


# ── 도구 핸들러 (1개 - propose_rule) ─────────────────────────────────────────


async def do_propose_rule(call_params: dict) -> str:
    t = REGISTRY["propose_rule"]
    params = dict(call_params)
    if "주기" in params:
        # 이 파이프라인 전체(Tray -> NotebookFlow -> File RA)가 "주기"를
        # 문자열로 다룬다(FileFlowRulesStore::BuildActiveRulesJsonArray 등과
        # 통일 - 여기만 숫자로 보내면 Tray/File RA 쪽 파싱과 어긋난다).
        params["주기"] = str(params["주기"])

    payload = {
        "SCE_EVENT": t["sce_event"],
        "AS_ID": FLOW_AS_ID,
        "SCE_ID": f"IC-{FLOW_AS_ID}",  # 결정론적 규칙(FLOW.cpp) - Tray가 이미 연 세션과 반드시 일치해야 함
        "ACTION_EVENT": t["action_event"],
        "PARAMS": params,
    }
    status, p = await flow_put(payload)
    # [확정 원칙] fire-and-forget - 승인/거부 결과를 기다리지 않는다(범위
    # 밖). 여기서 확인하는 건 "제안이 세션에 무사히 전달됐는지"뿐이고,
    # 그 이후는 전부 사용자가 트레이에서 직접 처리한다.
    if status != 200:
        reason = p.get("REASON", p.get("_error", f"status={status}"))
        return (f"제안 전달 실패: {reason}. NotebookFlow.exe와 트레이 앱이 "
                f"둘 다 실행 중인지 확인해주세요.")
    return "제안이 트레이에 전달되었습니다. 사용자가 승인하면 활성화됩니다."


# ── 알람 직접 트리거 (업무지침_알람Default시나리오_1단계.md, 2026-09-11) ──────────
# propose_rule과 달리 Tray 승인 관문을 거치지 않는다 - "규칙"이 아니라
# "지금 한 번 알림을 보내라"는 즉시성 명령이라 안전장치(조건/배치상한/주기)가
# 애초에 적용될 대상이 아니기 때문(대상 폴더/파일을 건드리는 게 아니라
# 외부 알람 채널로 문구 하나를 보낼 뿐).


async def flow_get_config_addr() -> tuple[int, dict]:
    """GET /notebookflow/config/addr - addr.ini 전체를 JSON으로 그대로
    반환한다(Flow::queryAddrConfig, 기존 설정 조회 API 재사용 - 신규 라우트
    없음). 섹션마다 값이 {key,val} 오브젝트의 배열로 온다(IniCategory::JSON
    참고 - 평평한 dict가 아님)."""
    url = f"http://{FLOW_HTTP_HOST}:{FLOW_HTTP_PORT}/notebookflow/config/addr"
    try:
        async with httpx.AsyncClient(timeout=TIMEOUT_SEC) as client:
            resp = await client.get(url)
        return resp.status_code, resp.json()
    except Exception as exc:
        return 0, {"_error": str(exc)}


async def flow_put_rest(action_event: str, body: dict) -> tuple[int, dict]:
    """PUT /notebookflow/flow (propose_rule과 동일한 기존 경로 - 신규 HTTP 라우트
    없음). 처음에는 PUT /notebookflow/rest/{namespace}/{action} 식으로 URL 경로에
    namespace/action을 실어 rest.sce가 "수신메시지.주소.경로 == 리터럴"로 직접
    매치하게 시도했으나, 실측 결과 항상 504 타임아웃이었다 - 원인: STATEPARSER.cpp/
    ACTIONPARSER.cpp는 .sce 안의 "/"가 든 리터럴을 파싱 시점에 serialPath()로
    "^^^"치환해서 저장하는데, genericHandler가 실제 요청에서 뽑아내는 주소.경로는
    Main.cpp::s_fnBuildAddrObjFromReq()가 원본 슬래시 그대로 채워넣는다(치환 없음) -
    양쪽이 서로 다른 인코딩이라 절대 같아질 수 없다(실제로 이 조합으로 경로를
    매치하는 기존 코드가 이 저장소 어디에도 없다 - LINEAR의 "/graphql" 매치는
    RA로 보낸 요청이 그대로 echo되어 돌아온 응답 전용이라 양쪽 다 .sce 파싱을
    거쳐 대칭적으로 치환됨). 대신 ACTION_EVENT 필드(EXESTATE.cpp가 이걸 읽어서
    "이벤트명"으로 바꿔치기 - 슬래시가 안 들어가 이 문제 자체가 없음, 기존
    propose_rule/ping/sse_connect가 이미 쓰는 검증된 경로)로 라우팅한다."""
    url = f"http://{FLOW_HTTP_HOST}:{FLOW_HTTP_PORT}/notebookflow/flow"
    payload = dict(body)
    payload["ACTION_EVENT"] = action_event
    try:
        async with httpx.AsyncClient(timeout=TIMEOUT_SEC) as client:
            resp = await client.put(url, json=payload)
        return resp.status_code, resp.json()
    except Exception as exc:
        return 0, {"_error": str(exc)}


def _parse_ini_section(items) -> dict[str, str]:
    """addr.ini 응답의 한 섹션 값({key,val} 오브젝트 배열)을 평평한 dict로."""
    if not isinstance(items, list):
        return {}
    return {
        item.get("key"): item.get("val")
        for item in items
        if isinstance(item, dict) and item.get("key") is not None
    }


def _alert_sections(cfg: dict) -> dict[str, dict[str, str]]:
    """category가 '알람'인 섹션만 추려 {섹션명(namespace): {category,message_field,
    target_action,...}} 형태로 반환한다. namespace 목록을 코드에 하드코딩하지
    않고, 매 호출마다 addr.ini의 현재 상태로 동적 구성한다(업무지침 요구사항) -
    addr.ini에 알람 섹션이 새로 추가/제거되면 코드 수정 없이 즉시 반영됨."""
    result: dict[str, dict[str, str]] = {}
    for section, items in cfg.items():
        kv = _parse_ini_section(items)
        if kv.get("category") == "알람" and kv.get("target_action"):
            result[section] = kv
    return result


async def do_list_connections(call_params: dict) -> str:
    status, cfg = await flow_get_config_addr()
    if status != 200:
        reason = cfg.get("REASON", cfg.get("_error", f"status={status}"))
        return f"연동 목록 조회 실패: {reason}. NotebookFlow.exe가 실행 중인지 확인해주세요."
    return json.dumps(cfg, ensure_ascii=False, indent=2)


async def do_trigger_alert(call_params: dict) -> str:
    namespace = call_params.get("namespace", "")
    message = call_params.get("message", "")

    status, cfg = await flow_get_config_addr()
    if status != 200:
        reason = cfg.get("REASON", cfg.get("_error", f"status={status}"))
        return f"연동 설정 조회 실패: {reason}"

    alert_sections = _alert_sections(cfg)
    if namespace not in alert_sections:
        available = ", ".join(alert_sections.keys()) or "(없음)"
        return (f"'{namespace}'는 알람(category=알람) 연동이 아니거나 존재하지 "
                f"않습니다. 사용 가능한 namespace: {available}")

    target_action = alert_sections[namespace]["target_action"]
    status2, resp = await flow_put_rest(target_action, {"message": message})
    if status2 != 200:
        reason = resp.get("REASON", resp.get("_error", f"status={status2}"))
        return f"{namespace} 알람 전송 요청 실패: {reason}"
    return f"{namespace}로 알람 전송을 요청했습니다: {message}"


HANDLERS = {
    "propose_rule": do_propose_rule,
    "list_connections": do_list_connections,
    "trigger_alert": do_trigger_alert,
}

# ── MCP 저수준 서버 ───────────────────────────────────────────────────────

REGISTRY = load_config(CONFIG_FILE)
server = Server("mcp-flow")


@server.list_tools()
async def list_tools() -> list[types.Tool]:
    return [
        types.Tool(
            name=name,
            description=t["description"],
            inputSchema=build_input_schema(t.get("params", {})),
        )
        for name, t in REGISTRY.items()
    ]


@server.call_tool()
async def call_tool(name: str, arguments: dict) -> list[types.TextContent]:
    t = REGISTRY.get(name)
    if not t:
        return [types.TextContent(type="text", text=f"알 수 없는 도구: {name}")]

    call_params: dict[str, Any] = {}
    for pname, p in t.get("params", {}).items():
        val = arguments.get(pname, p.get("default"))
        if val is None and p.get("required"):
            return [types.TextContent(
                type="text", text=f"필수 파라미터 누락: {pname}")]
        if val is not None:
            call_params[p.get("dsl_key", pname)] = val

    handler = HANDLERS.get(name)
    if handler is None:
        return [types.TextContent(type="text", text=f"핸들러 미구현: {name}")]
    text = await handler(call_params)
    return [types.TextContent(type="text", text=text)]


async def main():
    async with stdio_server() as (read, write):
        await server.run(read, write, server.create_initialization_options())


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass
    except Exception as exc:
        print(f"치명적 오류: {exc}", file=sys.stderr)
        sys.exit(1)
