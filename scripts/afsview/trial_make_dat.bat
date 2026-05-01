::
::  Simple script to pack required assets for trial by list.
::
::  We should extract original game assets first by specified list of all assets
::  (see src/System/File/FnameTable.hpp for eu or na)
::  Then we pack required assets for trial by custom handmade list.
::
::  Command format:
::      trial_make_dat.bat <path to TMNT.DAT>
::                         <path to TMNT.DAT file list>
::                         <path to TMNTE.DAT>
::                         <path to TMNTE.DAT file list>
::                         <path to trial common file list>
::                         <path to trial language file list>
::
::  Example of usage:
::      trial_make_dat.bat Z:/tmnt2_na/TMNT.DAT fname_na_common.txt Z:/tmnt2_na/TMNTE.DAT fname_na_language.txt trial_fname_common.txt trial_fname_language.txt
::

@echo off
setlocal enabledelayedexpansion

set "common_dat_in=%1"       & rem arg[1]: Path to TMNT.DAT
set "common_list_in=%2"      & rem arg[2]: Path to fnametable of TMNT.DAT
set "language_dat_in=%3"     & rem arg[3]: Path to TMNTE.DAT
set "language_list_in=%4"    & rem arg[4]: Path to fnametable of TMNTE.DAT
set "trial_common_list=%5"   & rem arg[5]: Path to trial fnametable for common
set "trial_language_list=%6" & rem arg[6]: Path to trial fnametable for language

::  Temporary converted fnametable for afsview.py script
set "common_list_out=%~n2.tmp"
set "language_list_out=%~n4.tmp"

set "afsview_extract_dir=extract"   & rem Directory where afsview.py stores result of extract
set "trial_result_dir=trial"        & rem Trial result store dir


::  Extract original DAT archives first
if not exist %afsview_extract_dir% (
    call afsview_make_elist.bat %common_list_in% %common_list_out% common/
    call afsview_make_elist.bat %language_list_in% %language_list_out% language/
    call python afsview.py --eall %common_dat_in% --elist %common_list_out%
    call python afsview.py --eall %language_dat_in% --elist %language_list_out%
    del %common_list_out%
    del %language_list_out%
)


::  Prepare trial dir
if not exist %trial_result_dir% ( 
    mkdir %trial_result_dir%
)


::  Make list for file write
call :MakeAFSList %trial_common_list% common
call :MakeAFSList %trial_language_list% language


::  Now make TMNT.DAT & TMNTE.DAT from common & language lists
::  and also cut full paths in output fnametable.
call python afsview.py --mafs "common.list" ^
                              "language.list" ^
                       --mpcut "%~dp0%afsview_extract_dir%\common" ^
                               "%~dp0%afsview_extract_dir%\language"


::  Save trial output & cleanup tmp & exit
move /y "common.dat"     "%trial_result_dir%\TMNT.DAT"
move /y "language.dat"   "%trial_result_dir%\TMNTE.DAT"
move /y "FnameTable.hpp" "%trial_result_dir%"
del "common.list"
del "language.list"
exit /b


:MakeAFSList
set "LIST_IN=%1"
set "LABEL=%2"
call afsview_make_elist.bat %LIST_IN% %LABEL%.list %~dp0%afsview_extract_dir%\%LABEL%\
goto :eof