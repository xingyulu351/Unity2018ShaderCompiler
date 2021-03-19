; Windows editor installer

!addplugindir Plugins

!include "WinVer.nsh"
!include "FileFunc.nsh"
!include "Plugins\UAC.nsh"
!include "LogicLib.nsh"
!include "StrFunc.nsh"
!include x64.nsh
!include "..\WindowsShared\WinDetection.nsh"
!include 'RecFind.nsh'

!include UnityPaths.nsh
!if ${ARCHITECTURE} = "x64"
    !define PRODUCT_NAME "Unity${IN_CUSTOM_NAME} ${IN_VERSION_NICE} (64-bit)"
    !define PRODUCT_NAME_IN_APPS "${IN_INSTALL_FOLDER}"
!else
    !define PRODUCT_NAME "Unity${IN_CUSTOM_NAME} ${IN_VERSION_NICE} (32-bit)"
    !define PRODUCT_NAME_IN_APPS "${IN_INSTALL_FOLDER} (32-bit)"
!endif

!if "${PRODUCT_NAME_IN_APPS}" = ""
    !error "PRODUCT_NAME_IN_APPS is empty string"
!endif

!define PRODUCT_VERSION "${IN_VERSION}"
!define PRODUCT_PUBLISHER "Unity Technologies ApS"
!define PRODUCT_WEB_SITE "http://www.unity3d.com"
!define UNITY_PREFS_REGKEY "Software\Unity Technologies\Unity Editor 5.x" ; This is used by Editor to look up e.g. project path
!define UNITY_WEBPLAYER_PREFS_REGKEY "Software\Unity\WebPlayer"
!define INSTALL_FOLDER "${IN_INSTALL_FOLDER}"
!define UNITY_INSTALLER_REGKEY_ROOT "Software\Unity Technologies\Installer"
!define UNITY_INSTALLER_REGKEY "${UNITY_INSTALLER_REGKEY_ROOT}\${IN_INSTALL_FOLDER}"

!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME_IN_APPS}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_EXE_NAME "Unity.exe"

!define DOTNET_ROOT_KEY "HKLM"
!define DOTNET_PARENT_KEY "SOFTWARE\Microsoft\NET Framework Setup\NDP"
!define DOTNET_4_PARENT_KEY "SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\Full"

!define DOTNET_35_KEY "v3.5"
!define DOTNET_4_KEY "Release"

Var ResultFile
var VSSectionIndex ; dummy var for lockedlist

; WindowsBootstrapper can read the code from RESULT_FILE, and call SetRebootFlag if the code is 3000
!define REBOOT_CODE 3000

!include Utilities.nsh
!include "registerExtension.nsh"

; Initialize used macros
${StrStr}

; Do some trickery here to be able to generate just the uninstaller, ready for code signing and inclusion in real installer
!ifdef DO_UNINSTALLER

RequestExecutionLevel admin
OutFile "..\..\..\build\temp\temp_UnityUnnstallerCreator.exe"
SetCompress off

!else

; Must start with user privileges, then the UAC plugin elevates to admin privileges in the init function
RequestExecutionLevel user
!if ${ARCHITECTURE} = "x64"
    OutFile "..\..\..\build\Windows64EditorInstaller\UnitySetup64.exe"
!else
    OutFile "..\..\..\build\Windows32EditorInstaller\UnitySetup32.exe"
!endif
;SetCompressor /SOLID lzma
SetCompressor lzma
!endif

; Version information for installer
VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey "LegalCopyright" "${IN_COPYRIGHT}"
VIAddVersionKey "FileDescription" "Unity ${IN_VERSION_NICE} Installer"
VIAddVersionKey "FileVersion" "${IN_VERSION_WIN}"
VIAddVersionKey "Unity Version" "${IN_VERSION}"
VIProductVersion "${IN_VERSION_WIN}"

Name "${PRODUCT_NAME}"
!if ${ARCHITECTURE} = "x64"
    InstallDir "$PROGRAMFILES64\${INSTALL_FOLDER}"
    InstallDirRegKey HKCU "${UNITY_INSTALLER_REGKEY}" "Location x64"
!else
    InstallDir "$PROGRAMFILES\${INSTALL_FOLDER}"
    InstallDirRegKey HKCU "${UNITY_INSTALLER_REGKEY}" "Location"
!endif
;ShowInstDetails show
;ShowUnInstDetails show

; MUI Settings
!include "MUI2.nsh"

!define MUI_WELCOMEFINISHPAGE_BITMAP "Background.bmp"

!define MUI_ABORTWARNING
!define MUI_ICON "..\..\..\PlatformDependent\Win\res\UnityIcon.ico"
!define MUI_UNICON "..\..\..\PlatformDependent\Win\res\UnityIcon.ico"
!define MUI_COMPONENTSPAGE_NODESC

; Installer pages
!define MUI_PAGE_CUSTOMFUNCTION_PRE SkipPageIfReducedUIMode
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE WelLeaveDownloadEULA
!insertmacro MUI_PAGE_WELCOME

!define MUI_PAGE_CUSTOMFUNCTION_PRE SkipPageIfReducedUIMode
!define MUI_LICENSEPAGE
!include "..\WindowsShared\UnityEula.nsh"

!define MUI_PAGE_CUSTOMFUNCTION_PRE SkipPageIfReducedUIMode
!insertmacro MUI_PAGE_COMPONENTS

!define MUI_PAGE_CUSTOMFUNCTION_PRE SkipPageIfReducedUIMode
!insertmacro MUI_PAGE_DIRECTORY

Page Custom LockedListShowCustom

!insertmacro MUI_PAGE_INSTFILES

!define MUI_PAGE_CUSTOMFUNCTION_PRE SkipPageIfReducedUIMode
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!ifdef DO_UNINSTALLER
UninstPage Custom un.LockedListShow
!insertmacro MUI_UNPAGE_INSTFILES
!endif

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

!insertmacro RefreshShellIcons

Var ReturnCode

Function LockedListShowCustom
    Call ReducedOrSilentUIMode
    Pop $0
    ${If} $0 == True
        Abort
    ${EndIf}
    Call LockedListShow
FunctionEnd

Function .onInit
    ; Just write out the uninstaller, the file can then be picked up later
!ifdef DO_UNINSTALLER
    WriteUninstaller "$EXEDIR\Uninstall.exe"
    Quit
!endif

    ; prevent multiple installers running
    ; NOTE: Disabled because of the UAC plugin which launches an extra installer process (with privilege elevation)
    ;System::Call 'kernel32::CreateMutexA(i 0, i 0, t "UnityEditorInstallerMutex") i .r1 ?e'
    ;Pop $R0
    ;StrCmp $R0 0 +3
    ;  MessageBox MB_OK|MB_ICONEXCLAMATION "Unity installer is already running!"
    ;  Abort

    ; check windows version
    ; check windows version
    !insertmacro CheckWindowsVersion

    System::Call 'kernel32::LoadLibrary(t "msftedit.dll")i.r0'

    ; detect if trying to install 64-bit Unity on 32-bit Windows
    !if ${ARCHITECTURE} = "x64"
        ${Unless} ${RunningX64}
            MessageBox MB_OK|MB_ICONEXCLAMATION "You're about to install 64-bit Unity on 32-bit Windows. The installer will work, but you won't be able to use Unity."
        ${EndIf}
    !endif

    ; Vista/Win7 - Require administrative privileges
    ; XP should just pass through when user is in admin group, otherwise shows a RunAs dialog for admin credentials
    UAC::RunElevated
    StrCmp 1223 $0 UAC_ElevationAborted ; UAC dialog aborted by user?
    StrCmp 0 $0 0 UAC_Err                ; If not 0 then error
    StrCmp 1 $1 0 UAC_Success            ; If 1 was returned we are a seperated elevated wrapper process and should continue, if not then quit (which happens after the elevated process has finished).
    Quit

    UAC_Err:
        MessageBox mb_iconstop "Unable to elevate installer ($0), it requires administrator access."
        Abort

    UAC_ElevationAborted:
        MessageBox mb_iconstop "This installer requires administrator access to install successfully."
        Abort

    UAC_Success:
        ;MessageBox MB_OK "UAC_Success 0:$0 1:$1 2:$2 3:$3"
        StrCmp 1 $3 +4                         ; Admin?
        StrCmp 3 $1 0 UAC_ElevationAborted
        MessageBox mb_iconstop "This installer requires administrator access."
        Abort

    ; set component options
    IntOp $0 ${SF_SELECTED} | ${SF_RO}
    SectionSetFlags SEC_UNITY $0

    InitPluginsDir

    StrCpy $ResultFile "$PLUGINSDIR\..\UnityInstallerResult.txt"
    Delete $ResultFile
FunctionEnd

Function .OnInstFailed
    UAC::Unload
FunctionEnd

Function .OnInstSuccess
    UAC::Unload
FunctionEnd

Function .onVerifyInstDir
    ${StrStr} $0 $INSTDIR " \" ; $R0 = non-zero if invalid space in path
    StrCmp $0 "" PathGood
        Abort
    PathGood:

    ${StrStr} $0 $INSTDIR "\ "
    StrCmp $0 "" PathGood2
        Abort
    PathGood2:
FunctionEnd

Function RegisterURIHandler
    Pop $R2 ; Extra
    Pop $R1 ; Executable Name
    Pop $R0 ; Scheme Name
    DetailPrint "Registering $R0 URI Handler"
    DeleteRegKey HKCR "$R0"
    WriteRegStr HKCR "$R0" "" "URL:$R0"
    WriteRegStr HKCR "$R0" "URL Protocol" ""
    WriteRegStr HKCR "$R0\DefaultIcon" "" "$R1"
    WriteRegStr HKCR "$R0\shell" "" ""
    WriteRegStr HKCR "$R0\shell\Open" "" ""
    WriteRegStr HKCR "$R0\shell\Open\command" "" '"$R1" $R2 "%1"'
FunctionEnd

Section "Unity" SEC_UNITY
    Call InstallVC2010Runtime
    Call InstallVC2013Runtime
    Call InstallVC2015Runtime

    SetOutPath "$INSTDIR\Editor"
    SetOverwrite try

    !ifndef DO_UNINSTALLER
    !include "FilelistInstall.nsh"
    !endif

    ; .unity and .unityPackage extensions
    ${registerExtension} "$INSTDIR\Editor\${PRODUCT_EXE_NAME}" ".unity" "Unity scene file" "-openfile"
    ${registerExtension} "$INSTDIR\Editor\${PRODUCT_EXE_NAME}" ".unityPackage" "Unity package file" "-openfile"

    ; Custom assetstore url scheme
    Push "com.unity3d.kharma"
    Push "$INSTDIR\Editor\${PRODUCT_EXE_NAME}"
    Push "-openurl"
    Call RegisterURIHandler
    ${RefreshShellIcons}

    ; Create start menu and desktop shortcuts for all users
    SetShellVarContext all

    ; Delete previously created shortcuts pointing to the same target
    Call StartMenuClean

    ; Start menu shortcuts
    CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
    CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Unity.lnk" "$INSTDIR\Editor\${PRODUCT_EXE_NAME}"
    CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Report a Problem with Unity.lnk" "$INSTDIR\Editor\BugReporter\unity.bugreporter.exe"
    CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Unity Documentation.lnk" "$INSTDIR\Editor\Data\Documentation\en\Manual\index.html"
    WriteINIStr "$SMPROGRAMS\${PRODUCT_NAME}\Unity Release Notes.url" "InternetShortcut" "URL" "http://unity3d.com/whatsnew.html"

    ; Desktop shortcut
    CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\Editor\${PRODUCT_EXE_NAME}"


    ${If} ${AtMostWinVista}
        # Check for .NET Framework 3.5 - see http://msdn.microsoft.com/library/cc160716.aspx
        !insertmacro IfKeyExists "${DOTNET_ROOT_KEY}" "${DOTNET_PARENT_KEY}" "${DOTNET_35_KEY}"
        pop $R0

        ${If} $R0 == "0"
            DetailPrint "Installing .NET Framework 3.5 Prerequisite..."
            SetDetailsPrint both
            ClearErrors

            File "..\..\..\External\Microsoft\dotnet35\builds\dotnetfx35setup.exe"
            ExecWait "dotnetfx35setup.exe /q /norestart" $0
            ${If} $0 = 1641
            ${OrIf} $0 = 3010
                # Success, but restart required
                SetRebootFlag true
                StrCpy $ReturnCode ${REBOOT_CODE}
            ${EndIf}
            SetDetailsPrint lastused
        ${EndIf}
    ${Else}
        #on windows7 and forward, we do not check the registry key, but use dism to turn on the windows feature always.
        #this is a pretty fast operation, and testing has shown that use registry key is not a very reliable way to detect
        #if the framework is present or not, especially if it has been turned on by dism / windows features window.
        #we use DisableX64FSRedirection, because without that, running the 32bit installer, on a 64bit os, will try to run
        #the 32bit dism, which is actually existent on a win64 install, but does not actually function. so we want to always
        #call the 64bit version, and never go through this redirecting windows backward compat nonsense.
        ${DisableX64FSRedirection}
        nsExec::ExecToLog /TIMEOUT=60000 "dism.exe /NoRestart /online /enable-feature /featurename:NetFx3 /all"
        Pop $0
        ${EnableX64FSRedirection}
    ${EndIf}

    # Check for .NET Framework 4.5. See https://msdn.microsoft.com/en-us/library/ee942965
    ReadRegDWORD $0 "${DOTNET_ROOT_KEY}" "${DOTNET_4_PARENT_KEY}" "${DOTNET_4_KEY}"
    ${If} $0 < 378389
        DetailPrint "Installing .NET Framework 4.5 Prerequisite..."
        SetDetailsPrint listonly

        ClearErrors
        File "..\..\..\External\Microsoft\dotnet45\builds\dotNetFx45_Full_setup.exe"
        ExecWait "dotNetFx45_Full_setup.exe /passive /norestart /chainingpackage ${PRODUCT_NAME}" $0

        ${If} $0 = 1641
        ${OrIf} $0 = 3010
            # Success, but restart required
            SetRebootFlag true
            StrCpy $ReturnCode ${REBOOT_CODE}
        ${EndIf}

        SetDetailsPrint lastused
    ${EndIf}

    Call InstallFirewallRules
SectionEnd

Section -Post
    !if ${ARCHITECTURE} = "x64"
        WriteRegStr HKCU "${UNITY_INSTALLER_REGKEY}" "Location x64" "$INSTDIR"
    !else
        WriteRegStr HKCU "${UNITY_INSTALLER_REGKEY}" "Location" "$INSTDIR"
    !endif
    WriteRegStr HKCU "${UNITY_INSTALLER_REGKEY}" "Version" "${IN_VERSION_NICE}" ; Used by e.g. Target Support installers to verify installed version
    WriteRegStr HKCU "${UNITY_WEBPLAYER_PREFS_REGKEY}" "UnityWebPlayerReleaseChannel" "Dev"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME_IN_APPS}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Editor\Uninstall.exe"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Editor\${PRODUCT_EXE_NAME}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"

    ; if this registy key is not set, Unity editor opens up "too large". If gets fixed in editor some day, we can remove this from installer
    ReadRegDWORD $R0 HKCU "${UNITY_PREFS_REGKEY}" "IsMainWindowMaximized_h1679813210" ; Editor uses hash value for allowing case-sensitive registry keys
    ${If} $R0 == ""
        WriteRegDWORD HKCU "${UNITY_PREFS_REGKEY}" "IsMainWindowMaximized_h1679813210" 1
    ${EndIf}

    IfRebootFlag 0 NoReboot
        Call ReducedOrSilentUIMode
        Pop $0
        ${If} $0 == False
            MessageBox MB_YESNO "A reboot is required to finish the installation. Do you wish to reboot now?" IDNO NoReboot
                Reboot
        ${EndIf}

    NoReboot:
SectionEnd

Section "-Final" SecFinal
    ${If} $ReturnCode != ""
        FileOpen $4 $ResultFile w
        FileWrite $4 "$ReturnCode"
        FileClose $4
    ${EndIf}
SectionEnd

!ifdef DO_UNINSTALLER
Function un.onUninstSuccess
    HideWindow
    IfSilent +2
    MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
    IfSilent +3
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
    Abort
FunctionEnd

Function un.LockedListShow
    !insertmacro MUI_HEADER_TEXT `Applications are blocking the uninstaller` `Some applications are preventing Unity from being uninstalled, and must be closed.`
    LockedList::AddModule \Unity.exe
    LockedList::Dialog /autonext /ignore Ignore /heading `Please close the following applications in order to continue with setup:`
    Pop $R0
FunctionEnd

Section Uninstall
    ; unregister extensions
    ${unregisterExtension} ".unity" "Unity scene file"
    ${unregisterExtension} ".unityPackage" "Unity package file"

    SetShellVarContext all

    Call un.DeleteFirewallRules

    Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
    Delete "$SMPROGRAMS\${PRODUCT_NAME}\Unity.lnk"
    Delete "$SMPROGRAMS\${PRODUCT_NAME}\Report a Problem with Unity.lnk"
    Delete "$SMPROGRAMS\${PRODUCT_NAME}\Unity Documentation.lnk"
    Delete "$SMPROGRAMS\${PRODUCT_NAME}\MonoDevelop.lnk"
    RMDir "$SMPROGRAMS\${PRODUCT_NAME}"

    !include "FilelistUninstall.nsh"

    DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
    !if ${ARCHITECTURE} = "x64"
        DeleteRegValue HKCU "${UNITY_INSTALLER_REGKEY}" "Location x64"
    !else
        DeleteRegValue HKCU "${UNITY_INSTALLER_REGKEY}" "Location"
    !endif
    DeleteRegKey /ifempty HKCU "${UNITY_INSTALLER_REGKEY}"

    ; If some files could not be removed, just leave them. Don't want to reboot.
    SetRebootFlag false

    SetAutoClose true

SectionEnd
!endif

Function SkipPageIfReducedUIMode
    Call ReducedOrSilentUIMode
    Pop $0
    ${If} $0 == True
        Abort
    ${EndIf}
FunctionEnd

Function ReducedOrSilentUIMode
    IfSilent 0 +3
        Push True
        Return

    ${GetOptions} $CMDLINE "-UI=" $R0
    ${If} $R0 == "reduced"
        Push True
        Return
    ${EndIf}

    Push False
FunctionEnd

Function InstallVC2010Runtime
    # Check for VC++ 2010 redist - see http://blogs.msdn.com/b/astebner/archive/2010/10/20/10078468.aspx

    !if ${ARCHITECTURE} = "x64"
        ReadRegDWORD $0 "HKLM" "SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x64" "Installed"
    !else
        ReadRegDWORD $0 "HKLM" "SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x86" "Installed"
    !endif
    ${If} $0 != "1"
        DetailPrint "Installing VC++ 2010 redistributable package..."
        SetDetailsPrint listonly

        ClearErrors
        SetOutPath "$PLUGINSDIR"
        !if ${ARCHITECTURE} = "x64"
            File "..\..\..\External\Microsoft\vcredist2010\builds\vcredist_x64.exe"
            ExecWait '"$PLUGINSDIR\vcredist_x64.exe" /q /norestart' $0
        !else
            File "..\..\..\External\Microsoft\vcredist2010\builds\vcredist_x86.exe"
            ExecWait '"$PLUGINSDIR\vcredist_x86.exe" /q /norestart' $0
        !endif

        ${If} $0 = 3010
            # Success, but restart required
            SetRebootFlag true
            StrCpy $ReturnCode ${REBOOT_CODE}
        ${ElseIf} $0 <> 0
            # If VC++ installation fails, show warning, but allow installation to continue
            IfSilent +2
            MessageBox MB_OK|MB_ICONEXCLAMATION "Installing VC++ 2010 runtime failed with error code $0.$\n$\nUnity will still be installed, however you should make sure to install Visual C++ runtime on your machine."
        ${EndIf}

        SetDetailsPrint lastused
    ${EndIf}
FunctionEnd

Function InstallVC2013Runtime
    !if ${ARCHITECTURE} = "x64"
        ReadRegDword $0 HKLM "SOFTWARE\Wow6432Node\Microsoft\VisualStudio\12.0\VC\Runtimes\x64" "Installed"
    !else
        ReadRegDword $0 HKLM "SOFTWARE\Wow6432Node\Microsoft\VisualStudio\12.0\VC\Runtimes\x86" "Installed"
    !endif
    ${If} $0 != "1"
        DetailPrint "Installing VC++ 2013 redistributable package..."
        SetDetailsPrint listonly

        ClearErrors
        SetOutPath "$PLUGINSDIR\vcredist2013"
        !if ${ARCHITECTURE} = "x64"
            File "..\..\..\External\Microsoft\vcredist2013\builds\vcredist_x64.exe"
            ExecWait '"$PLUGINSDIR\vcredist2013\vcredist_x64.exe" /q /norestart' $0
        !else
            File "..\..\..\External\Microsoft\vcredist2013\builds\vcredist_x86.exe"
            ExecWait '"$PLUGINSDIR\vcredist2013\vcredist_x86.exe" /q /norestart' $0
        !endif

        ${If} $0 = 3010
            # Success, but restart required
            SetRebootFlag true
            StrCpy $ReturnCode ${REBOOT_CODE}
        ${ElseIf} $0 <> 0
            # If VC++ installation fails, show warning, but allow installation to continue
            IfSilent +2
            MessageBox MB_OK|MB_ICONEXCLAMATION "Installing VC++ 2013 runtime failed with error code $0.$\n$\nUnity will still be installed, however you should make sure to install Visual C++ runtime on your machine."
        ${EndIf}

        SetDetailsPrint lastused
    ${EndIf}
FunctionEnd

Function InstallVC2015Runtime
    # 2015 not detectable, but throws an error if a newer version is installed, so just run always and ignore the error
    DetailPrint "Installing VC++ 2015 redistributable package..."
    SetDetailsPrint listonly

    ClearErrors
    SetOutPath "$PLUGINSDIR\vcredist2015"
    !if ${ARCHITECTURE} = "x64"
        File "..\..\..\External\Microsoft\vcredist2015\builds\vc_redist.x64.exe"
        ExecWait '"$PLUGINSDIR\vcredist2015\vc_redist.x64.exe" /q /norestart' $0
    !else
        File "..\..\..\External\Microsoft\vcredist2015\builds\vc_redist.x86.exe"
        ExecWait '"$PLUGINSDIR\vcredist2015\vc_redist.x86.exe" /q /norestart' $0
    !endif

    ${If} $0 = 3010
        # Success, but restart required
        SetRebootFlag true
        StrCpy $ReturnCode ${REBOOT_CODE}
    ${ElseIf} $0 = 1638
        # newer version is already installed
    ${ElseIf} $0 <> 0
        # If VC++ installation fails, show warning, but allow installation to continue
        IfSilent +2
        MessageBox MB_OK|MB_ICONEXCLAMATION "Installing VC++ 2015 runtime failed with error code $0.$\n$\nUnity will still be installed, however you should make sure to install Visual C++ runtime on your machine."
    ${EndIf}

    SetDetailsPrint lastused
FunctionEnd

Function InstallFirewallRules
    ; NOTE: during the install, $INSTDIR does not contain "\Editor" subpath, but it does during uninstall!
    ExecWait 'netsh advfirewall firewall delete rule name=all program="$INSTDIR\Editor\Unity.exe"'
    ExecWait 'netsh advfirewall firewall delete rule name=all program="$INSTDIR\Editor\Data\Tools\nodejs\node.exe"'
    ExecWait 'netsh advfirewall firewall add rule name="Unity ${PRODUCT_VERSION} Editor" dir=in action=allow program="$INSTDIR\Editor\Unity.exe" profile=domain protocol=any'
    ExecWait 'netsh advfirewall firewall add rule name="Unity ${PRODUCT_VERSION} Editor" dir=in action=block program="$INSTDIR\Editor\Unity.exe" profile=public protocol=any'
FunctionEnd

Function un.DeleteFirewallRules
    ExecWait 'netsh advfirewall firewall delete rule name=all program="$INSTDIR\Unity.exe"'
    ExecWait 'netsh advfirewall firewall delete rule name=all program="$INSTDIR\Data\Tools\nodejs\node.exe"'
FunctionEnd

; Recursively find and delete any start menu shortcuts pointing to Unity.exe or
; Uninstall.exe which this installer session is replacing, including the
; containing start menu folder
!define ICON_SEARCH '$SMPROGRAMS$R0\$R1'
!define ShortcutUnity '$INSTDIR\Editor\${PRODUCT_EXE_NAME}'
!define ShortcutUninstaller 'Uninstall.exe'
!define ShortcutDocumentation '$INSTDIR\Editor\Data\Documentation\en\Manual\index.html'
!define ShortcutBugreporter '$INSTDIR\Editor\BugReporter\unity.bugreporter.exe'
var SM_FOLDER

Function StartMenuClean
    ${RecFindOpen} '$SMPROGRAMS' '$R0' '$R1'
    ${RecFindFirst}
    ShellLink::GetShortCutTarget '${ICON_SEARCH}'
    Pop $0
    StrCmp '$0' '${ShortcutUnity}' 0 clean_uninstaller
    StrCpy '$SM_FOLDER' '$R0'
    Delete '${ICON_SEARCH}'
clean_uninstaller:
    StrCmp '$0' '${ShortcutUninstaller}' 0 clean_doc
    Delete '${ICON_SEARCH}'
clean_doc:
    StrCmp '$0' '${ShortcutDocumentation}' 0 clean_bugreporter
    Delete '${ICON_SEARCH}'
clean_bugreporter:
    StrCmp '$0' '${ShortcutBugreporter}' 0 find_next
    Delete '${ICON_SEARCH}'
find_next:
    ${RecFindNext}
    ${RecFindClose}
    StrCmp '$SM_FOLDER' '' end
    Delete '$SMPROGRAMS$SM_FOLDER\Unity Release Notes.url'
    RmDir '$SMPROGRAMS$SM_FOLDER'
end:
FunctionEnd


!include "LockedList.nsh"
