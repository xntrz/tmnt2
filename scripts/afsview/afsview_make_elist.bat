@echo off
setlocal enabledelayedexpansion

set "input=%~1"
set "output=%2"
set "prefix=%~3"

echo %input%

if not exist "%input%" (
    exit /b
)

(for /f "tokens=*" %%a in (%input%) do (
    set "line=%%a"    
    set "line=!line:"=!"
    set "line=!line:,=!"
    echo %prefix%!line!
)) > "%output%"