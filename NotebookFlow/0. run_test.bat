@echo off
chcp 65001 >nul
title dsl
::pushd "%~dp0..\TEST"
.\NotebookFlow.exe
popd
pause
