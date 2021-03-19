@echo off
set START_DIR=%cd%
for %%I in (%0) do set SCRIPT_DIR=%%~dpI
cd %SCRIPT_DIR%..\..
.\External\Jamplus\builds\bin\win32\jam.exe Qt %*
cd %START_DIR%