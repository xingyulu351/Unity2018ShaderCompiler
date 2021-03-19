@echo off

set MONO_PREFIX=%~dp0/../../External/Mono/builds/monodistribution
set MONO=%MONO_PREFIX%/bin/mono
"%MONO%" %*
exit /b %ERRORLEVEL%