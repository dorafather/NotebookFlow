# build_installer.ps1 — NotebookFlow 설치 프로그램 빌드
# (업무지침_InnoSetup_설계_1차릴리스.md, NotebookRAG.iss/build_installer.ps1 패턴 재사용)
# ------------------------------------------------------------------------
# 순서:
#   1. frontend(React+Vite) 빌드 (npm run build) — frontend/dist 생성.
#   2. agent-flow/ssh-flow PyInstaller onedir 산출물이 소스보다 최신인지
#      확인 — 오래됐으면 재빌드(pyinstaller *.spec), 최신이면 건너뜀.
#   3. tray-flow.exe를 "설치 레이아웃 지원" 소스(TrayApp.cpp의 경로 탐색
#      로직이 bin/notebookflow, bin/agent-flow, bin/ssh-flow 형태의 설치
#      구조도 인식하도록 2026-09-19에 갱신됨)로 재빌드.
#      ⚠️ 절대로 tray-src\tray-flow.exe(라이브 운영 중인 감시 프로세스
#      바이너리)를 직접 덮어쓰지 않는다 — tray-src\build-installer\ 라는
#      별도 출력 폴더에만 빌드한다.
#   4. NotebookFlow-release\bin\ 아래에 배포 디렉토리 구조를 조립한다
#      (패키징설계정리_1차릴리스_2026-09-19.md의 "2. 배포 디렉토리 구조"
#      그대로) — 개발 트리의 지저분한 백업/로그/테스트 파일은 전부
#      제외하고, 실제로 필요한 파일만 화이트리스트로 복사한다.
#      ⚠️ addr.ini(운영 중인 실제 자격증명 파일)는 여기서 "절대" bin/으로
#      복사하지 않는다 — 배포되는 건 항상 config/addr.ini.template(빈 값)
#      뿐이다.
#   5. NotebookFlow.iss를 UTF-8 BOM으로 강제(한글 깨짐 방지) 후 ISCC로 컴파일.
#
# 사용법: installer\ 안에서 실행
#   .\build_installer.ps1
# ------------------------------------------------------------------------

$ErrorActionPreference = "Stop"
$installerDir = $PSScriptRoot
$root = Split-Path $installerDir -Parent   # NotebookFlow-release\

Write-Host "[1/5] frontend(Vite) 빌드"
$frontendDir = Join-Path $root "frontend"
Push-Location $frontendDir
try {
    npm run build
    if ($LASTEXITCODE -ne 0) { throw "vite build 실패 (종료 코드 $LASTEXITCODE)" }
} finally {
    Pop-Location
}
$frontendDist = Join-Path $frontendDir "dist"
if (-not (Test-Path (Join-Path $frontendDist "index.html"))) {
    throw "$frontendDist\index.html 없음 — vite build가 정상적으로 산출물을 만들지 못함"
}

Write-Host "[2/5] agent-flow / ssh-flow PyInstaller 산출물 최신 여부 확인"
function Test-NeedsRebuild($srcPy, $distExe) {
    if (-not (Test-Path $distExe)) { return $true }
    return (Get-Item $srcPy).LastWriteTime -gt (Get-Item $distExe).LastWriteTime
}

$agentFlowSrc  = Join-Path $root "agent-flow\agent_flow.py"
$agentFlowExe  = Join-Path $root "agent-flow\dist\agent-flow\agent-flow.exe"
if (Test-NeedsRebuild $agentFlowSrc $agentFlowExe) {
    Write-Host "  -> agent-flow 재빌드 필요, pyinstaller 실행"
    Push-Location (Join-Path $root "agent-flow")
    try {
        pyinstaller agent-flow.spec
        if ($LASTEXITCODE -ne 0) { throw "agent-flow pyinstaller 빌드 실패" }
    } finally { Pop-Location }
} else {
    Write-Host "  -> agent-flow dist가 최신 상태 — 건너뜀"
}

$sshFlowSrc = Join-Path $root "ssh-flow\ssh_flow.py"
$sshFlowExe = Join-Path $root "ssh-flow\dist\ssh-flow\ssh-flow.exe"
if (Test-NeedsRebuild $sshFlowSrc $sshFlowExe) {
    Write-Host "  -> ssh-flow 재빌드 필요, pyinstaller 실행"
    Push-Location (Join-Path $root "ssh-flow")
    try {
        pyinstaller ssh-flow.spec
        if ($LASTEXITCODE -ne 0) { throw "ssh-flow pyinstaller 빌드 실패" }
    } finally { Pop-Location }
} else {
    Write-Host "  -> ssh-flow dist가 최신 상태 — 건너뜀"
}

Write-Host "[3/5] tray-flow.exe 빌드 (설치 레이아웃 지원 버전, 격리된 출력 폴더)"
$traySrcDir = Join-Path $root "tray-src"
$trayInstallerExe = Join-Path $traySrcDir "build-installer\tray-flow.exe"
$trayBuildBat = Join-Path $traySrcDir "build_installer_variant.bat"
if (-not (Test-Path $trayBuildBat)) {
    throw "$trayBuildBat 없음 — tray-src\build_installer_variant.bat(격리 출력 빌드 스크립트)가 먼저 있어야 함"
}
cmd /c "`"$trayBuildBat`""
if ($LASTEXITCODE -ne 0) { throw "tray-flow.exe(설치용) 빌드 실패" }
if (-not (Test-Path $trayInstallerExe)) { throw "$trayInstallerExe 산출물이 안 보임" }
# 안전 확인: 라이브 운영 중인 tray-src\tray-flow.exe는 절대 건드리지 않았어야 한다.
$liveTrayExe = Join-Path $traySrcDir "tray-flow.exe"
Write-Host "  -> 빌드 산출물: $trayInstallerExe"
Write-Host "  -> (참고) 라이브 감시 프로세스 바이너리 $liveTrayExe 는 이 스크립트가 절대 건드리지 않음"

Write-Host "[4/5] bin\ 배포 디렉토리 구조 조립"
$binDir = Join-Path $root "bin"

# --- bin\notebookflow\ ---
$nfDir = Join-Path $binDir "notebookflow"
New-Item -ItemType Directory -Path $nfDir -Force | Out-Null
Copy-Item (Join-Path $root "NotebookFlow\NotebookFlow.exe") (Join-Path $nfDir "NotebookFlow.exe") -Force
Copy-Item (Join-Path $root "NotebookFlow\rest.sce") (Join-Path $nfDir "rest.sce") -Force
Copy-Item (Join-Path $root "NotebookFlow\user.sce") (Join-Path $nfDir "user.sce") -Force
Copy-Item (Join-Path $root "CLAUDE.md") (Join-Path $nfDir "CLAUDE.md") -Force
Copy-Item (Join-Path $root "NotebookFlow\help.json") (Join-Path $nfDir "help.json") -Force
Copy-Item (Join-Path $root "NotebookFlow\cacert.pem") (Join-Path $nfDir "cacert.pem") -Force
$nfFrontendDir = Join-Path $nfDir "frontend"
if (Test-Path $nfFrontendDir) { Remove-Item $nfFrontendDir -Recurse -Force }
Copy-Item $frontendDist $nfFrontendDir -Recurse -Force
# ⚠️ addr.ini는 절대 여기로 복사하지 않는다 — 배포판은 addr.ini 없이
# 시작하고, 최초 실행 시 NotebookFlow.exe가 스스로 ..\config\addr.ini.template
# 에서 생성하거나(설치 마법사가 건너뛰기를 선택한 경우), 설치 마법사가
# ssPostInstall에서 검증된 Telegram 값을 채워 미리 만들어 준다(아래 .iss 참고).
Write-Host "  -> bin\notebookflow\ 조립 완료 (NotebookFlow.exe/rest.sce/user.sce/CLAUDE.md/help.json/cacert.pem/frontend)"

# --- bin\agent-flow\ (PyInstaller onedir 결과물 flat 복사) ---
$agentFlowDistDir = Join-Path $root "agent-flow\dist\agent-flow"
$binAgentFlowDir = Join-Path $binDir "agent-flow"
if (Test-Path $binAgentFlowDir) { Remove-Item $binAgentFlowDir -Recurse -Force }
Copy-Item $agentFlowDistDir $binAgentFlowDir -Recurse -Force
Write-Host "  -> bin\agent-flow\ 조립 완료"

# --- bin\ssh-flow\ ---
$sshFlowDistDir = Join-Path $root "ssh-flow\dist\ssh-flow"
$binSshFlowDir = Join-Path $binDir "ssh-flow"
if (Test-Path $binSshFlowDir) { Remove-Item $binSshFlowDir -Recurse -Force }
Copy-Item $sshFlowDistDir $binSshFlowDir -Recurse -Force
Write-Host "  -> bin\ssh-flow\ 조립 완료"

# --- bin\tray\ ---
$binTrayDir = Join-Path $binDir "tray"
New-Item -ItemType Directory -Path $binTrayDir -Force | Out-Null
Copy-Item $trayInstallerExe (Join-Path $binTrayDir "tray-flow.exe") -Force
Write-Host "  -> bin\tray\ 조립 완료 (설치 레이아웃 지원 버전)"

# --- bin\config\ ---
$binConfigDir = Join-Path $binDir "config"
New-Item -ItemType Directory -Path $binConfigDir -Force | Out-Null
Copy-Item (Join-Path $root "config\addr.ini.template") (Join-Path $binConfigDir "addr.ini.template") -Force
Write-Host "  -> bin\config\addr.ini.template 조립 완료"

# --- bin\assets\ ---
$binAssetsDir = Join-Path $binDir "assets"
New-Item -ItemType Directory -Path $binAssetsDir -Force | Out-Null
Copy-Item (Join-Path $root "assets\notebookflow.ico") (Join-Path $binAssetsDir "notebookflow.ico") -Force
Copy-Item (Join-Path $root "assets\NotebookFLOW.jpg") (Join-Path $binAssetsDir "NotebookFLOW.jpg") -Force
Write-Host "  -> bin\assets\ 조립 완료"

# --- bin\onboarding\ (이미 존재 — 최신 상태만 재확인) ---
$binOnboardingDir = Join-Path $binDir "onboarding"
if (-not (Test-Path (Join-Path $binOnboardingDir "welcome.md"))) {
    throw "$binOnboardingDir\welcome.md 없음 — 사전 산출물이어야 하는데 확인 필요"
}
Write-Host "  -> bin\onboarding\welcome.md 확인됨 (기존 산출물 그대로 사용)"

Write-Host "[5/5] Inno Setup 컴파일"
$issPath = Join-Path $installerDir "NotebookFlow.iss"
# ISCC가 한글을 깨지지 않게 읽으려면 UTF-8 BOM이 필요 — 매 빌드마다 강제.
$issText = Get-Content -Raw -Encoding UTF8 $issPath
[System.IO.File]::WriteAllText($issPath, $issText, (New-Object System.Text.UTF8Encoding($true)))

$iscc = "$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe"
if (-not (Test-Path $iscc)) {
    $iscc = "C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
}
if (-not (Test-Path $iscc)) {
    throw "ISCC.exe를 찾을 수 없음 — Inno Setup 6가 설치돼 있는지 확인할 것"
}

& $iscc $issPath
if ($LASTEXITCODE -ne 0) {
    throw "ISCC 컴파일 실패 (종료 코드 $LASTEXITCODE)"
}

$outExe = Join-Path $installerDir "output\NotebookFlow_Setup.exe"
if (Test-Path $outExe) {
    $sizeMb = [Math]::Round((Get-Item $outExe).Length / 1MB, 1)
    Write-Host ""
    Write-Host "완료 — $outExe ($sizeMb MB)"
} else {
    throw "컴파일은 성공했다는데 출력 파일이 안 보임: $outExe"
}
