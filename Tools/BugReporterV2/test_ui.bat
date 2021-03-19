@echo off
for %%I in (%0) do set SCRIPT_DIR=%%~dpI
call %SCRIPT_DIR%build.bat -sCONFIG=debug bugreporter.test_ui
%SCRIPT_DIR%..\..\build\BugReporter\win32-debug\bugreporter\test_ui\app\test_ui.debug.exe %*
