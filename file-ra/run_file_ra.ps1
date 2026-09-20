# File RA 실행 스크립트 (2026-08-30 - 무상태 명령 서버로 전면 재작성됨).
# File RA는 이제 스스로 아무것도 스케줄링하지 않는다 - POST/PUT /command로
# 명령이 오면 그 즉시 처리하는 FastAPI+uvicorn 서버(포트 8425)일 뿐이다.
# "언제 부를지"는 NotebookFlow.exe의 filesystem_watch.sce(자기재무장 타이머
# 세션)가 결정한다. 전제조건: 이 서버 자체는 NotebookFlow.exe 없이도 단독
# 기동 가능(진행 보고 PUT만 실패할 뿐 - fire-and-forget이라 무해).
$ErrorActionPreference = "Stop"
Set-Location -Path $PSScriptRoot
& "C:\dsl-pm2\.venv\Scripts\python.exe" file_ra.py
