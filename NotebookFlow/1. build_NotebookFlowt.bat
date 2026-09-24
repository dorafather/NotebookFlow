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
rem claude-code 2026-09-25: added CPPHTTPLIB_OPENSSL_SUPPORT + OpenSSL include path
rem (libUtil\openssl-win-setup.md 참고) - OutboundClient가 WinHTTP/OS 인증서
rem 저장소 대신 httplib+OpenSSL+자체 CA 번들로 TLS 검증하도록 바뀌었다. 이
rem 매크로는 httplib.h를 포함하는 모든 TU(Main.cpp도 포함)에 동일하게 걸어야
rem 한다 - TU마다 다르면 SSLClient 관련 클래스 레이아웃이 어긋나는 ODR 위반이
rem 생길 수 있어서, 파일별 #define이 아니라 이 공용 커맨드라인 한 곳에서만 켠다.
cl /nologo /EHsc /MT /Zi /utf-8 /c /D_STATIC /D CPPHTTPLIB_OPENSSL_SUPPORT ^
  /I "." /I "..\libUtil" /I "..\libUtil\data" /I "..\libUtil\json" /I "..\libUtil\kstring" /I "..\libUtil\thread" /I "..\libUtil\dtime" /I "..\libUtil\parser" /I "..\libUtil\interpretor" /I "..\libUtil\flow" /I "..\libUtil\dfile" /I "..\libUtil\httplib" /I "..\libUtil\openssl-win\include" ^
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
rem claude-code 2026-09-25: added libssl.lib/libcrypto.lib(libUtil\openssl-win-setup.md,
rem vcpkg로 로컬 빌드) + OpenSSL이 요구하는 시스템 라이브러리(crypt32/advapi32/user32,
rem OpenSSL 3.6.1 libcrypto.pc의 Libs: 기준)
link /nologo /OUT:NotebookFlow.exe Main.obj ConnRegistry.obj OutboundClient.obj TraceHub.obj ^
  "..\..\libUtil\build-util\util.lib" ^
  "..\..\libUtil\openssl-win\lib\libssl.lib" "..\..\libUtil\openssl-win\lib\libcrypto.lib" ^
  ws2_32.lib iphlpapi.lib dbghelp.lib gdiplus.lib crypt32.lib advapi32.lib user32.lib
if errorlevel 1 (
  echo LINK_FAILED
  pause
  exit /b 1
)

copy /Y NotebookFlow.exe "..\NotebookFlow.exe"
echo BUILD_OK
pause
