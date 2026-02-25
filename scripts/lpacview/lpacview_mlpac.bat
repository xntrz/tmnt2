@echo off
set "drop_dir=%~dp1"
set "bat_dir=%~dp0"
PUSHD %bat_dir%
python "%bat_dir%lpacview.py" %1 --mlpac
POPD
pause