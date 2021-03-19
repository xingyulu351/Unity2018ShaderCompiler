; Windows Target Support Installer

!addplugindir ..\WindowsEditor\Plugins

!include "WinVer.nsh"
!include "LogicLib.nsh"
!include "..\WindowsEditor\Plugins\UAC.nsh"
!include "..\WindowsShared\WinDetection.nsh"

!include UnityPaths.nsh
!define PRODUCT_NAME "Unity${IN_CUSTOM_NAME} ${IN_VERSION_NICE} ${IN_ENGINE_NICE_NAME} Support"
!define PRODUCT_NAME_IN_APPS "${IN_INSTALL_FOLDER}"

!if "${PRODUCT_NAME_IN_APPS}" = ""
    !error "PRODUCT_NAME_IN_APPS is empty string"
!endif

!define PRODUCT_VERSION "${IN_VERSION}"
!define PRODUCT_PUBLISHER "Unity Technologies ApS"
!define PRODUCT_WEB_SITE "http://www.unity3d.com"
!define UNITY_INSTALLER_REGKEY_ROOT "Software\Unity Technologies\Installer"
!define UNITY_INSTALLER_REGKEY "${UNITY_INSTALLER_REGKEY_ROOT}\${IN_INSTALL_FOLDER}"
!define INSTALL_FOLDER "${IN_INSTALL_FOLDER}"

; Do some trickery here to be able to generate just the uninstaller, ready for code signing and inclusion in real installer
!ifdef DO_UNINSTALLER
    ; TODO: uninstaller
!else
    RequestExecutionLevel admin
    OutFile "..\..\..\build\TargetSupportInstaller\${IN_FINAL_FILE_NAME}"
    ; can't use /SOLID as intermediate file grows more than 2GB for XB1 and iOS installers
    SetCompressor lzma
!endif

; Version information for installer
VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey "LegalCopyright" "${IN_COPYRIGHT}"
VIAddVersionKey "FileDescription" "Unity ${IN_VERSION_NICE} ${IN_ENGINE_NICE_NAME} Support Installer ${IN_TARGET_SUPPORT_VERSION} for Editor ${IN_BOUND_EDITOR_VERSION}"
VIAddVersionKey "FileVersion" "${IN_BOUND_EDITOR_VERSION}"
VIAddVersionKey "Unity Version" "${IN_VERSION}"
VIProductVersion "${IN_BOUND_EDITOR_VERSION}"

Name "${PRODUCT_NAME}"

;ShowInstDetails show
;ShowUnInstDetails show

; MUI Settings
!include "MUI2.nsh"

!define MUI_WELCOMEFINISHPAGE_BITMAP "${IN_ENGINE_BACKGROUND_IMG}"

!define MUI_ABORTWARNING
!define MUI_ICON "..\..\..\PlatformDependent\Win\res\UnityIconDown.ico"
!define MUI_UNICON "..\..\..\PlatformDependent\Win\res\UnityIconDown.ico"
!define MUI_COMPONENTSPAGE_NODESC

; Installer pages
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE WelLeaveDownloadEULA
!insertmacro MUI_PAGE_WELCOME
!define MUI_LICENSEPAGE
!include "..\WindowsShared\UnityEula.nsh"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
Page Custom LockedListShow
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!ifdef DO_UNINSTALLER
    ; TODO: uninstaller
    ;!insertmacro MUI_UNPAGE_INSTFILES
!endif

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

var installPath
var installPathX64
var docsInstallPath
var InstallSuffix
var VSSectionIndex ; dummy var for lockedlist

Function SplitFirstStrPart
    Exch $R0
    Exch
    Exch $R1
    Push $R2
    Push $R3
    StrCpy $R3 $R1
    StrLen $R1 $R0
    IntOp $R1 $R1 + 1
loop:
    IntOp $R1 $R1 - 1
    StrCpy $R2 $R0 1 -$R1
    StrCmp $R1 0 exit0
    StrCmp $R2 $R3 exit1 loop
exit0:
    StrCpy $R1 ""
    Goto exit2
exit1:
    IntOp $R1 $R1 - 1
    StrCmp $R1 0 0 +3
    StrCpy $R2 ""
    Goto +2
    StrCpy $R2 $R0 "" -$R1
    IntOp $R1 $R1 + 1
    StrCpy $R0 $R0 -$R1
    StrCpy $R1 $R2
exit2:
    Pop $R3
    Pop $R2
    Exch $R1 ;rest
    Exch
    Exch $R0 ;first
FunctionEnd

!macro TargetSupportSection InstallDir
    SectionIn RO ; section is read-only in UI

    StrCpy $INSTDIR "${InstallDir}"
    SetOutPath "$INSTDIR"
    SetOverwrite try

    ; default suffix for in-editor-folder install
    StrCpy $InstallSuffix "\Editor\Data\PlaybackEngines\${IN_ENGINE_FOLDER}"

    DetailPrint "Checking Unity installation folder..."
    ${IfNot} ${FileExists} "$INSTDIR\Editor\Unity.Exe"
        MessageBox MB_OK "Failed to locate Unity.exe!"
        Quit
    ${EndIf}

    ; Get "Unity Version" field from Unity.exe
    ; It's stored in the format "5.4.0a3_3128ec4be841"
    MoreInfo::GetUserDefined "$INSTDIR\Editor\Unity.Exe" "Unity Version"
    pop $R1

    ; Extract version in the "5.4.0a3" format
    Push "_"
    Push $R1
    Call SplitFirstStrPart
    Pop $R0
    Pop $R1

    ; Compare with the expected version
    ${If} $R0 == ""
        MessageBox MB_OK|MB_ICONEXCLAMATION "A matching version of Unity could not be found.$\n$\nUnable to install platform support."
        Quit
    ${ElseIf} $R0 != ${IN_VERSION_NICE}
        MessageBox MB_YESNO|MB_ICONEXCLAMATION "Version mismatch: Expected Unity ${IN_VERSION_NICE} installed, found $R0.$\n$\nInstall anyway?" IDYES InstallAnyway
        Quit
    ${EndIf}

    InstallAnyway:
        DetailPrint "Removing old Unity ${IN_ENGINE_NICE_NAME} Support installation..."
        SetDetailsPrint listonly

    ; MetroSupport/StandaloneSupport is split into installers per scripting backend but they all install into same PlaybackEngines folder
    ${If} "${IN_ENGINE_FOLDER}" != "MetroSupport"
    ${AndIf} "${IN_ENGINE_FOLDER}" != "MacStandaloneSupport"
    ${AndIf} "${IN_ENGINE_FOLDER}" != "WindowsStandaloneSupport"
    RemoveFiles:
        ClearErrors
        RMDir /r "$INSTDIR$InstallSuffix"
        IfErrors 0 doneremove
            MessageBox MB_OK|MB_ABORTRETRYIGNORE "Failed to delete old Unity ${IN_ENGINE_NICE_NAME} Support installation files. Maybe Unity or some of its tools are still running?" IDRETRY removefiles IDIGNORE doneremove
            Quit
    ${EndIf}

    DoneRemove:
        SetDetailsPrint lastused


    !ifndef DO_UNINSTALLER
        SetShellVarContext all

        DetailPrint "Installing Unity ${IN_ENGINE_NICE_NAME} Support..."
        SetOutPath "$INSTDIR$InstallSuffix"
        File /r "..\..\..\build\${IN_ENGINE_FOLDER}\*.*"

        DetailPrint "Installing documentation for ${IN_ENGINE_NICE_NAME} ..."

        StrCpy $docsInstallPath "$INSTDIR$InstallSuffix\Documentation"
        SetOutPath $docsInstallPath
        File /nonfatal /r "..\..\..\build\UserDocumentation${IN_ENGINE_FOLDER}\Documentation\*.*"

        DetailPrint "Indexing documentation ..."
        ExecWait "$\"$docsInstallPath\DocCombiner.exe$\" -autopaths $\"$INSTDIR$\"" $0
    !endif
!macroend

Section "Unity ${IN_ENGINE_NICE_NAME} Support" SEC_UNITY
       !insertmacro TargetSupportSection "$INSTDIR"
SectionEnd

Function .onInit
    ; check windows version
    !insertmacro CheckWindowsVersion

    ; Discover the locations of one or both x64 and 32 installations of Unity
    ReadRegStr $installPath HKCU "${UNITY_INSTALLER_REGKEY}" "Location"
    ReadRegStr $installPathX64 HKCU "${UNITY_INSTALLER_REGKEY}" "Location x64"

    ${If} $INSTDIR == ""
        ${If} $installPathX64 != ""
            StrCpy $INSTDIR $installPathX64
        ${ElseIf} $installPath != ""
            StrCpy $INSTDIR $installPath
        ${EndIf}
    ${EndIf}

    System::Call 'kernel32::LoadLibrary(t "msftedit.dll")i.r0'
FunctionEnd

Function .OnInstFailed
    UAC::Unload
FunctionEnd
 
Function .OnInstSuccess
    UAC::Unload
FunctionEnd

Section -Post
    #!ifndef DO_UNINSTALLER
    #SetOutPath "$INSTDIR"
    #File "Uninstall.exe"
    #!endif
SectionEnd

!ifdef DO_UNINSTALLER
Function un.onUninstSuccess
    HideWindow
    MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
    Abort
FunctionEnd

Section Uninstall

    !include "FilelistUninstall.nsh"
    ; If some files could not be removed, just leave them. Don't want to reboot.
    SetRebootFlag false

    SetAutoClose true

SectionEnd
!endif

!include "..\WindowsEditor\LockedList.nsh"
