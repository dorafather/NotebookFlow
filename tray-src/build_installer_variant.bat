@echo off
setlocal
cd /d "%~dp0"

call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 (
  echo VCVARS_FAILED
  exit /b 1
)

if not exist build-installer mkdir build-installer

rc.exe /nologo /fo build-installer\tray.res /I resources resources\notebookflow_tray.rc
if errorlevel 1 (
  echo RC_FAILED
  exit /b 1
)

cl.exe /nologo /EHsc /MT /Zi /utf-8 /std:c++17 /D_UNICODE /DUNICODE /DNOMINMAX /W3 /I src /c ^
  src\TrayApp.cpp src\ProcessManager.cpp ^
  /Fobuild-installer\ /Fdbuild-installer\tray.pdb
if errorlevel 1 (
  echo CL_FAILED
  exit /b 1
)

link.exe /nologo /OUT:build-installer\tray-flow.exe /SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup /MANIFEST:EMBED ^
  /MANIFESTDEPENDENCY:"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'" ^
  build-installer\TrayApp.obj build-installer\ProcessManager.obj build-installer\tray.res ^
  mfcs140u.lib user32.lib gdi32.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winspool.lib winhttp.lib ws2_32.lib
if errorlevel 1 (
  echo LINK_FAILED
  exit /b 1
)

echo BUILD_OK
