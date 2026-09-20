# -*- mode: python ; coding: utf-8 -*-
# [업무지침_TrayFlow_프로세스관리.md] mcp-flow.spec(NotebookFlow-release/
# mcp-flow/mcp-flow.spec) 구조를 그대로 따름 - file_ra.py는 FastAPI+uvicorn
# 서버라 fastapi/uvicorn을 collect_all로 명시 포함(PyInstaller 기본 탐지가
# uvicorn의 동적 로더(loops.auto/protocols.*.auto)를 놓치는 사례가 흔함 -
# notebookrag.spec의 llama_cpp/hwp5 때와 같은 이유로 안전하게 명시).

from PyInstaller.utils.hooks import collect_all

datas = []
binaries = []
hiddenimports = []
tmp_ret = collect_all('uvicorn')
datas += tmp_ret[0]; binaries += tmp_ret[1]; hiddenimports += tmp_ret[2]
tmp_ret = collect_all('fastapi')
datas += tmp_ret[0]; binaries += tmp_ret[1]; hiddenimports += tmp_ret[2]

a = Analysis(
    ['file_ra.py'],
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
    name='file-ra',
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
    name='file-ra',
)
