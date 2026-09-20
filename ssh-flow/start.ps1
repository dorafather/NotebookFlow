# start.ps1
# SSH API 서버 실행 스크립트

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  SSH Command API 서버 시작 중..." -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "  주소 : http://127.0.0.1:8450" -ForegroundColor Green
Write-Host "  종료 : Ctrl + C" -ForegroundColor Yellow
Write-Host ""

# 현재 스크립트가 있는 폴더로 이동
Set-Location $PSScriptRoot

# 서버 실행 (파일명이 ssh_flow.py 인 경우, 2026-09-17 하이픈->밑줄 개명 -
# PyInstaller Analysis가 하이픈 포함 모듈명을 import 못 해서 배포 빌드
# 대상으로 바꾸며 함께 변경)
python -m uvicorn ssh_flow:app --host 127.0.0.1 --port 8450 --reload