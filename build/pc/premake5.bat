@echo off
setlocal

for %%f in (%*) do (
    echo Calling: %%f
    call %%f
)

if not defined ARG_VS (
    echo.
    echo ERROR: ARG_VS is not specified! 
    echo        Please specify ARG_VS before calling premake5.bat
    echo.
    echo Usage example:
    echo        set ARG_VS=vs2015
    echo        premake5.bat
    echo.
    echo OR call directly by premake5.exe vs2015
    echo OR drag and drop set_vs* file onto premake5.bat
    echo.
    endlocal
    exit /b 
)

premake5.exe %ARG_VS% %ARG_IDFS% %ARG_RWDRV%
endlocal