@echo off

set UnityPaths=UnityPaths.nsh
set Architecture=x64

if "%Architecture%" == "x86" (
	set OutputFolder=..\..\..\build\Windows32EditorInstaller\
	set OutputExe=UnitySetup32.exe
) else (
	set OutputFolder=..\..\..\build\Windows64EditorInstaller\
	set OutputExe=UnitySetup64.exe
)

echo !define IN_VERSION "5.0.0a7" > %UnityPaths%
echo !define IN_VERSION_WIN "5.0.0.0" >> %UnityPaths%
echo !define IN_VERSION_NICE "5.0.0a7" >> %UnityPaths%
echo !define IN_CUSTOM_NAME "" >> %UnityPaths%
echo !define IN_COMPANY_NAME "Unity Technologies ApS" >> %UnityPaths%
echo !define IN_COPYRIGHT "(c) 2014 Unity Technologies ApS. All rights reserved." >> %UnityPaths%
echo !define IN_EXAMPLE_NAME "Sample-Assets" >> %UnityPaths%
echo !define IN_EXAMPLE_SCENE "Assets/Menu/Scenes/MainMenu.unity" >> %UnityPaths%
echo !define IN_INSTALL_FOLDER "Unity 5.0.0a7" >> %UnityPaths%
echo !define IN_MODULES_FILE_URL "http://homes.hq.unity3d.com/rasmuss/WindowsExampleInstaller/UnityInstallerModuleUrls.ini" >> %UnityPaths%

if not exist "%OutputFolder%" (
	mkdir "%OutputFolder%"
)

..\..\..\External\Installers\NSIS\builds\makensis.exe /V3 /DSKIP_WEBPLAYER /DARCHITECTURE=%Architecture% /DBUILD_TEST_INSTALLER /DDO_UNINSTALLER SetupUnity.nsi

if %ERRORLEVEL% NEQ 0 (
	echo ERROR: Building uninstaller failed!
	pause
	exit /b
)

..\..\..\build\temp\temp_UnityUnnstallerCreator.exe

..\..\..\External\Installers\NSIS\builds\makensis.exe /V3 /DSKIP_WEBPLAYER /DARCHITECTURE=%Architecture% /DBUILD_TEST_INSTALLER SetupUnity.nsi

if %ERRORLEVEL% NEQ 0 (
	echo.
	echo ERROR: Building installer failed!
	echo.
	pause
	exit /b
)

echo.

set RunInstaller=y
set /p RunInstaller=Installer build completed. Would you like to run the installer now [Y/n]?

if "%RunInstaller%" neq "n" (
	"%OutputFolder%\%OutputExe%" /P
	echo "%OutputFolder%\%OutputExe% /P returned %ERRORLEVEL%"
	rem "%OutputFolder%\%OutputExe%" /D=C:\UnityTest
)
