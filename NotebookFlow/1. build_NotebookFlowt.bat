@echo off
setlocal
cd /d "%~dp0"

if not exist build mkdir build

call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 (
  echo VCVARS_FAILED
  pause
  exit /b 1
)

rem claude-code 2026-08-29: added /I httplib include path + ConnRegistry.cpp/OutboundClient.cpp to build
rem claude-code 2026-09-11: added TraceHub.cpp (realtime call tracing feature)
cl /nologo /EHsc /MT /Zi /utf-8 /c /D_STATIC ^
  /I "." /I "..\libUtil" /I "..\libUtil\data" /I "..\libUtil\json" /I "..\libUtil\kstring" /I "..\libUtil\thread" /I "..\libUtil\dtime" /I "..\libUtil\parser" /I "..\libUtil\interpretor" /I "..\libUtil\flow" /I "..\libUtil\dfile" /I "..\libUtil\httplib" ^
  Main.cpp ConnRegistry.cpp OutboundClient.cpp TraceHub.cpp /Fobuild\
if errorlevel 1 (
  echo CL_FAILED
  pause
  exit /b 1
)

cd build
rem claude-code 2026-08-29: added ConnRegistry.obj/OutboundClient.obj to link
rem claude-code 2026-09-11: added TraceHub.obj to link (realtime call tracing feature)
rem claude-code 2026-09-18: added gdiplus.lib (help 명령어 표 이미지 생성, GDI+)
link /nologo /OUT:NotebookFlow.exe Main.obj ConnRegistry.obj OutboundClient.obj TraceHub.obj ^
  "..\..\libUtil\build-util\util.lib" ^
  ws2_32.lib iphlpapi.lib dbghelp.lib gdiplus.lib
if errorlevel 1 (
  echo LINK_FAILED
  pause
  exit /b 1
)

copy /Y NotebookFlow.exe "..\NotebookFlow.exe"
echo BUILD_OK
pause
