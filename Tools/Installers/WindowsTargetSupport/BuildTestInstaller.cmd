@echo off

set UnityPaths=UnityPaths.nsh
set Architecture=x64

set OutputFolder=..\..\..\build\TargetSupportInstaller\
set OutputExe=TestInstaller.exe

echo !define IN_VERSION "5.0.0b12" > %UnityPaths%
echo !define IN_VERSION_WIN "5.0.0.0" >> %UnityPaths%
echo !define IN_VERSION_NICE "5.0.0b12" >> %UnityPaths%
echo !define IN_CUSTOM_NAME "" >> %UnityPaths%
echo !define IN_COMPANY_NAME "Unity Technologies ApS" >> %UnityPaths%
echo !define IN_ENGINE_NICE_NAME "Xbox 720" >> %UnityPaths%
echo !define IN_COPYRIGHT "(c) 2014 Unity Technologies ApS. All rights reserved." >> %UnityPaths%
echo !define IN_EXAMPLE_NAME "Sample-Assets" >> %UnityPaths%
echo !define IN_EXAMPLE_SCENE "Assets/Menu/Scenes/MainMenu.unity" >> %UnityPaths%
echo !define IN_INSTALL_FOLDER "Unity 5.0.0b12" >> %UnityPaths%
echo !define IN_ENGINE_BACKGROUND_IMG "..\\WindowsEditor\\Background.bmp"  >> %UnityPaths%
echo !define IN_ENGINE_FOLDER "WebGLSupport"  >> %UnityPaths%
echo !define IN_BOUND_EDITOR_VERSION "5.0.0.12"  >> %UnityPaths%
echo !define IN_FINAL_FILE_NAME %OutputExe%  >> %UnityPaths%

if not exist "%OutputFolder%" (
	mkdir "%OutputFolder%"
)

..\..\..\External\Installers\NSIS\builds\makensis.exe /V3 SetupTargetSupport.nsi

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
