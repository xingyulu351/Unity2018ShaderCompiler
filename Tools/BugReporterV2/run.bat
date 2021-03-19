@echo off
for %%I in (%0) do set SCRIPT_DIR=%%~dpI
call %SCRIPT_DIR%build.bat -sCONFIG=release bugreporter
%SCRIPT_DIR%..\..\build\WindowsEditor\BugReporter\unity.bugreporter.exe %*
