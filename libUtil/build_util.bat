@echo off
setlocal
cd /d "%~dp0"

if not exist build-util mkdir build-util

call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 (
  echo VCVARS_FAILED
  pause
  exit /b 1
)

cl /nologo /EHsc /MT /Zi /utf-8 /c /D_STATIC ^
  /I "." /I "data" /I "json" /I "kstring" /I "thread" /I "dtime" /I "parser" /I "interpretor" /I "flow" /I "dfile" ^
  @sources_util.rsp /Fobuild-util\
if errorlevel 1 (
  echo CL_FAILED
  pause
  exit /b 1
)

cd build-util
lib /nologo /OUT:util.lib *.obj
if errorlevel 1 (
  echo LIB_FAILED
  pause
  exit /b 1
)

echo BUILD_OK
pause
