@echo off

set UnityPaths=..\WindowsEditor\UnityPaths.nsh
set Architecture=x64

set OutputFolder=..\..\..\build\WindowsBootstrapper\
set OutputExe=UnityDownloadAssistant.exe

echo !define IN_VERSION "5.0.0b12" > %UnityPaths%
echo !define IN_VERSION_WIN "5.0.0.0" >> %UnityPaths%
echo !define IN_VERSION_NICE "5.0.0b12" >> %UnityPaths%
echo !define IN_CUSTOM_NAME "" >> %UnityPaths%
echo !define IN_COMPANY_NAME "Unity Technologies ApS" >> %UnityPaths%
echo !define IN_COPYRIGHT "(c) 2014 Unity Technologies ApS. All rights reserved." >> %UnityPaths%
echo !define IN_EXAMPLE_NAME "Sample-Assets" >> %UnityPaths%
echo !define IN_EXAMPLE_SCENE "Assets/Menu/Scenes/MainMenu.unity" >> %UnityPaths%
echo !define IN_INSTALL_FOLDER "Unity 5.0.0b12" >> %UnityPaths%
echo !define IN_BOOTSTRAPPER_INI_URL1 "http://netstorage.unity3d.com/unity/640b34381707/unity-5.0.0b12-win.ini" >> %UnityPaths%
echo !define IN_BOOTSTRAPPER_INI_URL2 "http://download.unity3d.com/download_unity/640b34381707/unity-5.0.0b12-win.ini" >> %UnityPaths%
echo !define IN_BOOTSTRAPPER_INI_URL3 "http://beta.unity3d.com/download/c70982df466c/unity-5.0.0b15-win.ini" >> %UnityPaths%
echo !define IN_BOOTSTRAPPER_INI_URL4 "http://files.unity3d.com/bootstrapper/test/unity-5.0.0b14-win.ini" >> %UnityPaths%

if not exist "%OutputFolder%" (
	mkdir "%OutputFolder%"
)

..\..\..\External\Installers\NSIS\builds\makensis.exe /V3 SetupBootstrapper.nsi

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
	@rem "%OutputFolder%\%OutputExe%" -ini="http://artifact.hq.unity3d.com/artifacts/proj0-Build_WindowsEditorInstallerSet_839140_04_11_2014_15_21_44_+0000/build/WindowsEditorInstallerSet/unity-5.0.0b12-win.ini" -CMD2=value
)
