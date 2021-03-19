@echo off
set START_DIR=%cd%
for %%I in (%0) do set SCRIPT_DIR=%%~dpI
set GENERATOR=%1
if [%GENERATOR%]==[] set GENERATOR=vs2012
cd %SCRIPT_DIR%
..\..\External\Jamplus\builds\bin\win32\jam.exe --workspace --config=win.config -gen=%GENERATOR% Jamfile.jam workspace
cd %START_DIR%