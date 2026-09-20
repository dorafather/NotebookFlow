# NotebookFlow 프론트+백엔드 통합 기동 스크립트
# 백엔드(NotebookFlow.exe/File RA)는 tray-flow.exe 하나가 Job Object로
# 같이 띄우고 관리한다(이미 부팅 시 자동 기동되지만, 안 떠 있으면 이 스크립트로
# 수동 기동 가능) - 프론트엔드(React+Vite dev server)는 아직 tray-flow에
# 연결 안 돼 있어서(업무지침_ReactVite프론트엔드_목업5차기준_최종.md 범위 밖)
# 여기서 별도로 띄운다.

$root = $PSScriptRoot
$trayExe = Join-Path $root "tray-src\tray-flow.exe"
$frontendDir = Join-Path $root "frontend"

# ── 백엔드(tray-flow.exe가 NotebookFlow.exe/File RA를 같이 관리) ──
if (Get-Process -Name "tray-flow" -ErrorAction SilentlyContinue) {
    Write-Host "[백엔드] tray-flow.exe 이미 실행 중 - 건너뜀"
} else {
    Write-Host "[백엔드] tray-flow.exe 기동..."
    Start-Process -FilePath $trayExe
    Start-Sleep -Seconds 3
}

# ── 프론트엔드(Vite dev server) ──
# Invoke-WebRequest는 로컬에서도 간헐적으로 실패/지연되어 오탐(이미 떠
# 있는데 중복 기동)을 일으킨 적이 있어 - TCP 포트 리슨 여부로 직접 확인.
$viteRunning = [bool](Get-NetTCPConnection -LocalPort 5180 -State Listen -ErrorAction SilentlyContinue)

if ($viteRunning) {
    Write-Host "[프론트] Vite dev server 이미 실행 중 - 건너뜀"
} else {
    Write-Host "[프론트] Vite dev server 기동..."
    Start-Process -FilePath "npm.cmd" -ArgumentList "run","dev" -WorkingDirectory $frontendDir `
        -RedirectStandardOutput (Join-Path $frontendDir "_vite_stdout.log") `
        -RedirectStandardError (Join-Path $frontendDir "_vite_stderr.log") `
        -WindowStyle Hidden
    Start-Sleep -Seconds 3
}

Write-Host ""
Write-Host "=== 상태 ==="
Get-Process -Name tray-flow,NotebookFlow,file-ra,node -ErrorAction SilentlyContinue |
    Select-Object Id,ProcessName,StartTime | Format-Table -AutoSize
Write-Host "프론트엔드: http://localhost:5180"
Write-Host "백엔드 API: http://localhost:18099"
