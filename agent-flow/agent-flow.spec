# -*- mode: python ; coding: utf-8 -*-
# [업무지침_TrayFlow_ssh-flow_agent-flow_통합.md, 2026-09-17]
# file-ra.spec(NotebookFlow-release/file-ra/file-ra.spec) 구조를 그대로
# 따름 - agent_flow.py는 FastAPI+uvicorn 서버라 fastapi/uvicorn을
# collect_all로 명시 포함. httpx는 완료 콜백(report_done)에 쓰는 HTTP
# 클라이언트라 함께 collect_all로 포함(httpcore 등 하위 의존성 동적 로딩
# 대비). agent_flow.py가 subprocess로 호출하는 "claude" CLI는 시스템 PATH의
# 별도 실행파일이므로 PyInstaller 번들링 대상이 아님(그대로 시스템 명령
# 호출).

from PyInstaller.utils.hooks import collect_all

datas = []
binaries = []
hiddenimports = []
tmp_ret = collect_all('uvicorn')
datas += tmp_ret[0]; binaries += tmp_ret[1]; hiddenimports += tmp_ret[2]
tmp_ret = collect_all('fastapi')
datas += tmp_ret[0]; binaries += tmp_ret[1]; hiddenimports += tmp_ret[2]
tmp_ret = collect_all('httpx')
datas += tmp_ret[0]; binaries += tmp_ret[1]; hiddenimports += tmp_ret[2]
# httpx가 SSL 컨텍스트 생성 시 certifi.where()로 CA 번들을 찾는데,
# collect_all('httpx')는 httpx 자체 데이터만 담고 의존 패키지인 certifi의
# cacert.pem은 안 담아서 실행 시 FileNotFoundError로 기동이 죽었다(2026-09-24
# 실측, 재부팅/신규설치 환경에서 재현).
tmp_ret = collect_all('certifi')
datas += tmp_ret[0]; binaries += tmp_ret[1]; hiddenimports += tmp_ret[2]

a = Analysis(
    ['agent_flow.py'],
    pathex=[],
    binaries=binaries,
    datas=datas,
    hiddenimports=hiddenimports,
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='agent-flow',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=True,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)
coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='agent-flow',
)
