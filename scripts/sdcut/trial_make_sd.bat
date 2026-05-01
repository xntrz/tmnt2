::
::  Simple script to cut all needed sounds for trial
::
::  Example of usage:
::      trial_make_sd.bat Z:\tmnt2_na
::

@echo off
setlocal enabledelayedexpansion

set "sound_path=%1"             & rem arg[1]: Path to game assets dir
set "trial_result_dir=trial"    & rem Trial result store dir


::  Prepare trial dir
if not exist %trial_result_dir% ( 
    mkdir %trial_result_dir%
)


:: Now cut all things
call :CutSE trial_banktable_se.txt WaveData.bin
call :CutSTR trial_codetable_str.txt strbgm.vas 0x3000
call :CutSTR trial_codetable_str.txt voice0.bin 0x4000
call :CutSTR trial_codetable_str.txt voice1.bin 0x5000
call :CutSTR trial_codetable_str.txt voice2.bin 0x6000


:CutSE
set "CUTLIST=%1"
set "CONTAINER=%2"
python sdcut.py %CUTLIST% --secut %sound_path%\%CONTAINER%
move /y %CONTAINER% "%trial_result_dir%"
goto :eof


:CutSTR
set "CUTLIST=%1"
set "CONTAINER=%2"
set "BASECODE=%3"
python sdcut.py %CUTLIST% --strcut %sound_path%\%CONTAINER% %BASECODE%
move /y %CONTAINER% "%trial_result_dir%"
goto :eof