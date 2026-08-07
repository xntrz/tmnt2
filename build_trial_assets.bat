@echo off
setlocal enabledelayedexpansion

set "game_dir=Z:\tmnt2_na"
if not "%~1"=="" set "game_dir=%~1"

set "dat_dir=%cd%\scripts\afsview"
set "dat_script=%dat_dir%\trial_make_dat.bat"
set "sd_dir=%cd%\scripts\sdcut"
set "sd_script=%sd_dir%\trial_make_sd.bat"
set "data_dir=%cd%\data\trial"
set "src_dir=%cd%\src"
set "fnametable_name=FnameTable_PC_TRIAL.hpp"


::  Prepare trial data dir
rd /s /q %data_dir%
mkdir %data_dir%


::  Make and move trial dat assets
pushd %dat_dir%
call "%dat_script%" "%game_dir%\TMNT.DAT" ^
                    "%dat_dir%\fname_na_common.txt" ^
                    "%game_dir%\TMNTE.DAT" ^
                    "%dat_dir%\fname_na_language.txt" ^
                    "%dat_dir%\trial_fname_common.txt" ^
                    "%dat_dir%\trial_fname_language.txt"
popd
move /y "%dat_dir%\trial\*.DAT" "%data_dir%"
move /y "%dat_dir%\trial\*.hpp" "%src_dir%\System\Common\File\%fnametable_name%"


::  Make and move trial sd assets
pushd %sd_dir%
call "%sd_script%" "%game_dir%"                       
popd
move /y "%sd_dir%\trial\*.bin" "%data_dir%"
move /y "%sd_dir%\trial\*.vas" "%data_dir%"


::  Now copy last 2 files
copy /y "%game_dir%\TrSeData.bin" "%data_dir%"
copy /y "%game_dir%\SeData.bin"   "%data_dir%"