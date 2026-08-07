@echo off
setlocal

set "build_config=debug"
if not "%~1"=="" set "build_config=%~1"

rd "%cd%\bin\web\opengl\%build_config%\data"
rd "%cd%\bin\web\opengl\%build_config%\Common"
mklink /d "%cd%\bin\web\opengl\%build_config%\data" "%cd%\data"
mklink /d "%cd%\bin\web\opengl\%build_config%\Common" "%cd%\data\Common"
pushd "%cd%\bin\web\opengl\%build_config%"
python -m RangeHTTPServer --bind 0.0.0.0 6931
popd