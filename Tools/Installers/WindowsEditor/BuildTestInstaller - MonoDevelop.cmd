@echo off

set UnityPaths=UnityPaths.nsh
set Architecture=x64

set OutputFolder=..\..\..\build\WindowsMonoDevelopInstaller
set OutputExe=UnityMonoDevelopSetup.exe

echo !define IN_VERSION "5.0.0a7" > %UnityPaths%
echo !define IN_VERSION_WIN "5.0.0.0" >> %UnityPaths%
echo !define IN_VERSION_NICE "5.0.0a7" >> %UnityPaths%
echo !define IN_CUSTOM_NAME "" >> %UnityPaths%
echo !define IN_COMPANY_NAME "Unity Technologies ApS" >> %UnityPaths%
echo !define IN_COPYRIGHT "(c) 2014 Unity Technologies ApS. All rights reserved." >> %UnityPaths%
echo !define IN_INSTALL_FOLDER "Unity 5.0.0a7" >> %UnityPaths%

if not exist "%OutputFolder%" (
	mkdir "%OutputFolder%"
)

..\..\..\External\Installers\NSIS\builds\makensis.exe /V3 SetupMonoDevelop.nsi

if %ERRORLEVEL% NEQ 0 (
	echo.
	echo ERROR: Building installer failed!
	echo.
	pause
	exit /b
)

set RunInstaller=y
set /p RunInstaller=Installer build completed. Would you like to run the installer now [Y/n]?

if "%RunInstaller%" neq "n" (
	"%OutputFolder%\%OutputExe%"
)
