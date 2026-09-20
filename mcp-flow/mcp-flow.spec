# -*- mode: python ; coding: utf-8 -*-
# mcp-rag.spec(NotebookRAG-release/src/mcp-rag.spec) 구조를 그대로 따름.
# 버전 정보 파일(gen_version_file.py)은 NotebookRAG 전용 버전 상수에
# 묶여있어 여기서는 생략 - 필요해지면 나중에 추가.

from PyInstaller.utils.hooks import copy_metadata

a = Analysis(
    ['mcp_flow.py'],
    pathex=[],
    binaries=[],
    datas=copy_metadata('mcp'),
    hiddenimports=[],
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
    name='mcp-flow',
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
    name='mcp-flow',
)
