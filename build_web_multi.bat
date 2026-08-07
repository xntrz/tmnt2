::
::  Example of usage:
::      build_web_multi.bat debug
::

@echo off
setlocal enabledelayedexpansion

set "CMAKE_BUILD_TYPE=%1" & rem arg[1]: Build type (debug OR release)
set "CMAKE_BUILD_PRESET=web-%CMAKE_BUILD_TYPE%"
set "CMAKE_BUILD_CORES=%NUMBER_OF_PROCESSORS%"
set "CMAKE_BUILD_OPT=-DOPT_MULTI=ON -DOPT_CLOSURE_COMPILER=ON -DOPT_TEST=ON"

set "BASE_DIR=bin\web\opengl\%CMAKE_BUILD_TYPE%"
set "BIN_DIR=%BASE_DIR%"
set "BIN_DIR_TMP=tmp"
set "BUILD_DIR=build\web_multi"

if "%CMAKE_BUILD_TYPE%"=="" (
    echo Please specify build type for exec: release or debug
    echo Example: build_web_multi.bat debug
    exit /b
)

:: Generate build files
if not exist %BUILD_DIR% (
    (
        start "GENERATE EU" cmd /c cmake --preset %CMAKE_BUILD_PRESET% -B %BUILD_DIR%\eu %CMAKE_BUILD_OPT% -DOPT_BIN_DIR=%BIN_DIR_TMP%\full\eu -DOPT_EU_BUILD=ON
        start "GENERATE NA" cmd /c cmake --preset %CMAKE_BUILD_PRESET% -B %BUILD_DIR%\na %CMAKE_BUILD_OPT% -DOPT_BIN_DIR=%BIN_DIR_TMP%\full\na -DOPT_EU_BUILD=OFF
        start "GENERATE TR" cmd /c cmake --preset %CMAKE_BUILD_PRESET% -B %BUILD_DIR%\tr %CMAKE_BUILD_OPT% -DOPT_BIN_DIR=%BIN_DIR_TMP%\trial   -DOPT_EU_BUILD=OFF -DOPT_TRIAL=ON
    ) | pause
)

:: Parallel build 
(
    start "BUILD EU" cmd /c cmake --build %BUILD_DIR%\eu -j2
    start "BUILD NA" cmd /c cmake --build %BUILD_DIR%\na -j2
    start "BUILD TR" cmd /c cmake --build %BUILD_DIR%\tr -j2
) | pause

:: Now unpack all binaries 
:: (trial should be unpacked in last for proper js)
call :UnpackAndMove full\eu
call :UnpackAndMove full\na
call :UnpackAndMove trial

:: Build end
exit /b

:: Unpacks binaries from tmp dir to root bin dir
:: then cleans no needed garbage
:: then moves binaries to build specific dir
:UnpackAndMove
set "NAME=%1"
set "WASMDIR=wasm\%1"
robocopy "%BIN_DIR%\%BIN_DIR_TMP%\%NAME%" "%BIN_DIR%" /E /NFL /NDL /NJH /NJS /nc /ns /np
if not exist "%BASE_DIR%\%WASMDIR%" mkdir "%BASE_DIR%\%WASMDIR%"
copy /Y "%BIN_DIR%\index.wasm" "%BASE_DIR%\%WASMDIR%\"
copy /Y "%BIN_DIR%\index.js" "%BASE_DIR%\%WASMDIR%\"
if exist "%BIN_DIR%\index.data" copy /Y "%BIN_DIR%\index.data" "%BASE_DIR%\%WASMDIR%\"
del /Q "%BIN_DIR%\index.wasm" ^
       "%BIN_DIR%\index.data"
if not "%NAME%" == "trial" ( 
    del /Q "%BIN_DIR%\index.js" & rem Remain index.js just for run dummy workerfs to determine assets type.
)
goto :eof