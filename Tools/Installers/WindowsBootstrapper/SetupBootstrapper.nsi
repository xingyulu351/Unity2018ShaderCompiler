; Windows editor installer

!include nsDialogs.nsh

!include "WinVer.nsh"
!include "FileFunc.nsh"
!include "StrFunc.nsh"
!include "LogicLib.nsh"
!include "InstallOptions.nsh"
!include x64.nsh
!include "..\WindowsShared\WinDetection.nsh"

!include ..\WindowsEditor\UnityPaths.nsh

${StrStr}
${StrRep}
${StrLoc}

Var /Global BOOTSTRAPPER_INI_URL1
Var /Global BOOTSTRAPPER_INI_URL2
Var /Global BOOTSTRAPPER_INI_URL3
Var /Global BOOTSTRAPPER_INI_URL4
Var /Global IniFileName
Var /Global ActualIniUrl
Var /Global ComponentName

Var /Global VSWasInstalled ; VS was installed during this session
Var /Global VSInstalled ; VS was pre-installed
Var /Global VSEdition ; Pre-installed VS Edition: Community, Professional or Enterprise.
Var /Global VSWorkloadInstalled ; VS Workload for Unity is pre-installed
Var /Global VSDevEnvPath ; Full path to VS devenv.exe
Var /Global VSSectionIndex ; Section index of VS component.

Var /Global AutomationScriptFile ; generated bat file handle

Var /Global SpaceAvailableLabel ; label with available disk space, for dynamic updates, used from UpdateSpaceAvailableLabel

Var /Global DownloadFolder ; The actual download folder, based on the user selection (can be either $PLUGINDIR, or $CustomDownloadFolder)
Var /Global DownloadToCustom ; True if the user selected the "custom location" radio button
Var /Global CustomDownloadFolder ; User-configured custom download location

RequestExecutionLevel admin

OutFile "..\..\..\build\WindowsBootstrapper\UnityDownloadAssistant.exe" ; TODO: Move this into shared config at some point
SetCompressor lzma

!define PRODUCT_NAME "Unity ${IN_VERSION_NICE} Download Assistant"
!define PRODUCT_EXE_NAME "Unity.exe"
!define INSTALL_FOLDER "${IN_INSTALL_FOLDER}"
!define UNITY_INSTALLER_REGKEY_ROOT "Software\Unity Technologies\Installer"
!define UNITY_INSTALLER_REGKEY "${UNITY_INSTALLER_REGKEY_ROOT}\${IN_INSTALL_FOLDER}"
!define UNITY_PREFS_REGKEY "Software\Unity Technologies\Unity Editor 5.x" ; This is used by Editor to look up e.g. project path
!define UNITY_INSTALL_64BIT_SETTING_REGKEY "Select_Install_64bit"

Var ResultFile
!define REBOOT_CODE 3000

Name "Unity"
Caption "${PRODUCT_NAME}"
BrandingText "${PRODUCT_NAME}"

!define INSTALLER_ABORT_MESSAGE "The installation was aborted. The installer will quit now."


; MUI Settings
!include "MUI2.nsh"

!define MUI_WELCOMEFINISHPAGE_BITMAP "..\WindowsEditor\Background.bmp"

!define MUI_ABORTWARNING
!define MUI_ICON "..\..\..\PlatformDependent\Win\res\UnityIcon.ico"
!define MUI_UNICON "..\..\..\PlatformDependent\Win\res\UnityIcon.ico"

; Installer pages
!define MUI_WELCOMEPAGE_TITLE "${PRODUCT_NAME}"
!define MUI_WELCOMEPAGE_TEXT "This assistant will guide you through downloading and installing Unity.$\n$\nPlease make sure you stay connected to the Internet during the installation process, as Unity components will need to be downloaded.$\n$\nClick Next to continue."
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE WelLeaveDownloadEULA_Custom
!insertmacro MUI_PAGE_WELCOME

!define MUI_LICENSEPAGE
!define MUI_TEXT_LICENSE_SUBTITLE "Please review and accept the license terms before downloading and installing Unity."
!include "..\WindowsShared\UnityEula.nsh"

!define MUI_COMPONENTSPAGE_LARGEDESC ; Shows custom version of this page stored in \Tools\NSIS\Contrib\UIs\modern_largedesc.exe
!define MUI_TEXT_COMPONENTS_SUBTITLE "Choose which Unity components you want to download and install."
LangString ^SpaceRequired ${LANG_ENGLISH} "Install space required: "
!insertmacro MUI_PAGE_COMPONENTS

Page Custom FolderSettingsPage FolderSettingsPageLeave

Page Custom ComponentEULA_Show0 ComponentEULA_Leave
Page Custom ComponentEULA_Show1 ComponentEULA_Leave
Page Custom ComponentEULA_Show2 ComponentEULA_Leave
Page Custom ComponentEULA_Show3 ComponentEULA_Leave
Page Custom ComponentEULA_Show4 ComponentEULA_Leave
Page Custom ComponentEULA_Show5 ComponentEULA_Leave
Page Custom ComponentEULA_Show6 ComponentEULA_Leave
Page Custom ComponentEULA_Show7 ComponentEULA_Leave
Page Custom ComponentEULA_Show8 ComponentEULA_Leave
Page Custom ComponentEULA_Show9 ComponentEULA_Leave
Page Custom ComponentEULA_Show10 ComponentEULA_Leave
Page Custom ComponentEULA_Show11 ComponentEULA_Leave
Page Custom ComponentEULA_Show12 ComponentEULA_Leave
Page Custom ComponentEULA_Show13 ComponentEULA_Leave
Page Custom ComponentEULA_Show14 ComponentEULA_Leave
Page Custom ComponentEULA_Show15 ComponentEULA_Leave
Page Custom ComponentEULA_Show16 ComponentEULA_Leave
Page Custom ComponentEULA_Show17 ComponentEULA_Leave
Page Custom ComponentEULA_Show18 ComponentEULA_Leave
Page Custom ComponentEULA_Show19 ComponentEULA_Leave
Page Custom ComponentEULA_Show20 ComponentEULA_Leave
Page Custom ComponentEULA_Show21 ComponentEULA_Leave
Page Custom ComponentEULA_Show22 ComponentEULA_Leave
Page Custom ComponentEULA_Show23 ComponentEULA_Leave
Page Custom ComponentEULA_Show24 ComponentEULA_Leave
Page Custom ComponentEULA_Show25 ComponentEULA_Leave
Page Custom ComponentEULA_Show26 ComponentEULA_Leave
Page Custom ComponentEULA_Show27 ComponentEULA_Leave

!define MUI_TEXT_INSTALLING_TITLE "Downloading and Installing"
!define MUI_TEXT_INSTALLING_SUBTITLE "Please wait while Unity is being downloaded and installed."
Page Custom LockedListShow
!insertmacro MUI_PAGE_INSTFILES

!define MUI_PAGE_CUSTOMFUNCTION_SHOW ModifyRunCheckbox
!insertmacro MUI_PAGE_FINISH

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

; keen.io urls/keys
; example query:
; inetc::post '{"installed":"true"}' /HEADER "Content-Type: application/json" /TOSTACK 'http://api.keen.io/3.0/projects/54ca9ed9d2eaaa7993efbf37/events/VSInstaller?api_key=d03872b5c7ecb88a43f8812aee81e52089517fd650fe6d11a3d70d71136244183dd279ec185b65a1ae9a2026b35b22a84b1141d4f4b59a442a6ff27c2c33f47b4956dec654b85332c571605ed0e936df5c8c6056ecaa318fba5c8966e9791b7a62e1e260ddbf448ccb06259dab53e5bb' "" /END

!define KEENIO_API_URL "http://api.keen.io/3.0"

!define KEENIO_PROJECT_RELEASE "54ca9c8d96773d2f95121ba9"
!define KEENIO_KEY_RELEASE "b25fa8ef0a9cd1f229c788b69ef38f46c5aca71bf388806bba417281de4ea2634e913cb544e5bd529431905eb4d29b9a195e0bb5edd6406c395aa2889f3e1cfc95ab002ec98bb78177471a257fec2f310b93592b544530304582c5af9afcb140dcce0e9b220976329b91329a2410ee9f"

!define KEENIO_PROJECT_BETA "54d20f83d2eaaa6b7dd257b0"
!define KEENIO_KEY_BETA "669fea5a19c7edae826e6a5ba113773708ec84fc8cb5ca0bd92391b83c2e9fa9072cd50cc79805ebac3a0f70f279a252ecbc1750b00bd2952571f15e7149f0b38f28a8a38986a79ee358159ed7ee4d078fff2217eff9c147d896df283a9ed338416442e73ff1b7c3d5dc6b89506516f1"

!define KEENIO_PROJECT_DEV "54ca9ed9d2eaaa7993efbf37"
!define KEENIO_KEY_DEV "d03872b5c7ecb88a43f8812aee81e52089517fd650fe6d11a3d70d71136244183dd279ec185b65a1ae9a2026b35b22a84b1141d4f4b59a442a6ff27c2c33f47b4956dec654b85332c571605ed0e936df5c8c6056ecaa318fba5c8966e9791b7a62e1e260ddbf448ccb06259dab53e5bb"

# Keep "list" of mapped section names from ini file. Used when downloading component
# Sizes are in Kbytes

!macro DEF_SECTION_VARS nr
Var /Global SectionComponentName${nr}
Var /Global SectionComponentTitle${nr}
Var /Global SectionComponentDesc${nr}
Var /Global SectionComponentSize${nr}
Var /Global SectionComponentSync${nr}
Var /Global EULABanner${nr} ; displayed in the banner area of the page

; each component license page can have 3 links to eulas, each with its own label and URL
Var /Global EULA_URL_1_${nr}
Var /Global EULA_URL_2_${nr}
Var /Global EULA_URL_3_${nr}
Var /Global EULA_LABEL_1_${nr}
Var /Global EULA_LABEL_2_${nr}
Var /Global EULA_LABEL_3_${nr}
!macroend

!insertmacro DEF_SECTION_VARS 0
!insertmacro DEF_SECTION_VARS 1
!insertmacro DEF_SECTION_VARS 2
!insertmacro DEF_SECTION_VARS 3
!insertmacro DEF_SECTION_VARS 4
!insertmacro DEF_SECTION_VARS 5
!insertmacro DEF_SECTION_VARS 6
!insertmacro DEF_SECTION_VARS 7
!insertmacro DEF_SECTION_VARS 8
!insertmacro DEF_SECTION_VARS 9
!insertmacro DEF_SECTION_VARS 10
!insertmacro DEF_SECTION_VARS 11
!insertmacro DEF_SECTION_VARS 12
!insertmacro DEF_SECTION_VARS 13
!insertmacro DEF_SECTION_VARS 14
!insertmacro DEF_SECTION_VARS 15
!insertmacro DEF_SECTION_VARS 16
!insertmacro DEF_SECTION_VARS 17
!insertmacro DEF_SECTION_VARS 18
!insertmacro DEF_SECTION_VARS 19
!insertmacro DEF_SECTION_VARS 20
!insertmacro DEF_SECTION_VARS 21
!insertmacro DEF_SECTION_VARS 22
!insertmacro DEF_SECTION_VARS 23
!insertmacro DEF_SECTION_VARS 24
!insertmacro DEF_SECTION_VARS 25
!insertmacro DEF_SECTION_VARS 26
!insertmacro DEF_SECTION_VARS 27

Var /Global CurrentSelection
Var /Global NewSelection

!define SECTIONCOUNT 27 ; total - 1

;--------------------------------
;Save selected sections
!macro SaveSections VAR
    StrCpy ${VAR} 0
    ${ForEach} $R0 ${SECTIONCOUNT} 0 - 1
        IntOp ${VAR} ${VAR} << 1
        ${If} ${SectionIsSelected} $R0
            IntOp ${VAR} ${VAR} + 1
        ${EndIf}
    ${Next}
!macroend

Function EnableSectionByText
    Pop $0
    Push $R0
    Push $R6
    ${For} $R0 0 ${SECTIONCOUNT}
        SectionGetText $R0 $R6
        ${If} $R6 == $0
            SectionGetFlags $R0 $R6
            IntOp $R6 $R6 | ${SF_SELECTED}
            SectionSetFlags $R0 $R6
        ${EndIf}
    ${Next}
    Pop $R6
    Pop $R0
FunctionEnd

Function SelChangedHandler
    ; save selected sections to compare
    !insertmacro SaveSections $NewSelection

    ; save original number for comparison
    Push $NewSelection
    ; XOR both results, find out which sections changed
    IntOp $R3 $CurrentSelection ^ $NewSelection

    ${For} $R0 0 ${SECTIONCOUNT}
        ; check if section has changed
        IntOp $R4 $R3 & 1
        ${If} $R4 == 1
            IntOp $R5 $NewSelection & 1
            SectionGetText $R0 $R6
            ${If} $R5 == 1
                ${If} $R6 == "Facebook Games Build Support"
                    Push "WebGL Build Support"
                    Call EnableSectionByText
                ${EndIf}
            ${EndIf}
        ; :
        ; :
        ${EndIf}
        ; go to next section
        IntOp $R3 $R3 >> 1
        IntOp $NewSelection $NewSelection >> 1
    ${Next}

    Pop $CurrentSelection
FunctionEnd

Function .onInit
    ; check windows version
    !insertmacro CheckWindowsVersion

    SetShellVarContext all

    ; allow only one instance
    System::Call 'kernel32::CreateMutex(i 0, i 0, t "UnityDownloadAssistant") ?e'
    Pop $R0
    IntCmpU $R0 183 0 +3 +3
        MessageBox MB_OK "The Unity Download Assistant is already running."
        Abort

    System::Call 'kernel32::LoadLibrary(t "msftedit.dll")i.r0'

    StrCpy $BOOTSTRAPPER_INI_URL1 ""
    StrCpy $BOOTSTRAPPER_INI_URL2 ""
    StrCpy $BOOTSTRAPPER_INI_URL3 ""
    StrCpy $BOOTSTRAPPER_INI_URL4 ""

    ; Get ini location from command line, if specified
    ${GetOptions} $CMDLINE "-ini=" $R0
    ${If} $R0 == ""
        ; default ini locations specified in Configuration\BuildConfig.pm
        !ifdef IN_BOOTSTRAPPER_INI_URL1
            StrCpy $BOOTSTRAPPER_INI_URL1 "${IN_BOOTSTRAPPER_INI_URL1}"
        !endif

        !ifdef IN_BOOTSTRAPPER_INI_URL2
            StrCpy $BOOTSTRAPPER_INI_URL2 "${IN_BOOTSTRAPPER_INI_URL2}"
        !endif

        !ifdef IN_BOOTSTRAPPER_INI_URL3
            StrCpy $BOOTSTRAPPER_INI_URL3 "${IN_BOOTSTRAPPER_INI_URL3}"
        !endif

        !ifdef IN_BOOTSTRAPPER_INI_URL4
            StrCpy $BOOTSTRAPPER_INI_URL4 "${IN_BOOTSTRAPPER_INI_URL4}"
        !endif
    ${Else}
        StrCpy $BOOTSTRAPPER_INI_URL1 $R0
    ${EndIf}

    ; Get License.rtf location from command line, if specified
    ${GetOptions} $CMDLINE "-eula=" $R0
    ${If} $R0 != ""
        StrCpy $CmdLineEULAFile $R0
    ${EndIf}

    InitPluginsDir
    SetOutPath "$PLUGINSDIR\VisualStudioInstallChecker"
    File "..\..\..\build\VisualStudioInstallChecker\Release\VisualStudioInstallChecker.exe"

    # Check for VS 15 (2017) install with ManagedGame (Unity) workload
    nsExec::ExecToStack '"$PLUGINSDIR\VisualStudioInstallChecker\VisualStudioInstallChecker.exe" 15 Microsoft.VisualStudio.Workload.ManagedGame'
    Pop $0

    ${If} $0 < 0
        MessageBox MB_OK|MB_ICONSTOP "Unity Download Assistant was not able to verify whether Visual Studio Community 2017 is installed. Please manually select whether you want to install Visual Studio Community 2017."
    ${ElseIf} $0 = 0 # VS not installed
        StrCpy $VSInstalled "false"
        StrCpy $VSWorkloadInstalled "false"
    ${ElseIf} $0 = 1 # VS and Unity workload installed
        Pop $VSDevEnvPath # stdout
        StrCpy $VSInstalled "true"
        StrCpy $VSWorkloadInstalled "true"
    ${ElseIf} $0 > 1 # VS installed without Unity workload
        Pop $VSDevEnvPath # stdout
        StrCpy $VSInstalled "true"
        StrCpy $VSWorkloadInstalled "false"

        ${If} $0 == 2
            StrCpy $VSEdition "Community"
        ${ElseIf} $0 == 3
            StrCpy $VSEdition "Professional"
        ${ElseIf} $0 == 4
            StrCpy $VSEdition "Enterprise"
        ${Else}
            MessageBox MB_OK|MB_ICONSTOP "Visual Studio Install Checker return unexpected value ($0)"
        ${Endif}
    ${EndIf}

    Call DownloadAndReadIniFile

    StrCpy $ResultFile "$PLUGINSDIR\..\UnityInstallerResult.txt"
    StrCpy $DownloadFolder $PLUGINSDIR ; Default store downloaded files in temp folder (which automatically gets deleted after completion)

    !insertmacro SaveSections $CurrentSelection
FunctionEnd


!macro CHECK_REQ_UNITY nr
    SectionGetFlags ${nr} $0

    IntOp $0 $0 & ${SF_SELECTED}

    ${If} $requires_unity != "true"
    ${AndIf} $0 != 0
        ${If} $SectionComponentName${nr} == "Unity"
            StrCpy $unity_selected "true"
        ${Else}
            ReadINIStr $0 $IniFileName $SectionComponentName${nr} "requires_unity"

            ${If} $0 == "true"
                StrCpy $requires_unity "true"
            ${EndIf}
        ${EndIf}
    ${EndIf}

    # While we are going through all components, save the state to registry for the next run
    ${If} $SectionComponentName${nr} != ""
        ${If} $0 != 0
            WriteRegDWORD HKCU "${UNITY_INSTALLER_REGKEY_ROOT}" "Select_$SectionComponentName${nr}" 1
        ${Else}
            WriteRegDWORD HKCU "${UNITY_INSTALLER_REGKEY_ROOT}" "Select_$SectionComponentName${nr}" 0
        ${EndIf}
    ${EndIf}
!macroend

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

Function RelGotoPage
    IntCmp $R9 0 0 Move Move
    StrCmp $R9 "X" 0 Move
    StrCpy $R9 "120"
Move:
    SendMessage $HWNDPARENT "0x408" "$R9" ""
FunctionEnd

Function CheckUnityRequirement
    ; check whether any "requires_unity" components are selected
    Var /global requires_unity
    Var /global unity_selected

    StrCpy $requires_unity "false"
    StrCpy $unity_selected "false"

    !insertmacro CHECK_REQ_UNITY 0
    !insertmacro CHECK_REQ_UNITY 1
    !insertmacro CHECK_REQ_UNITY 2
    !insertmacro CHECK_REQ_UNITY 3
    !insertmacro CHECK_REQ_UNITY 4
    !insertmacro CHECK_REQ_UNITY 5
    !insertmacro CHECK_REQ_UNITY 6
    !insertmacro CHECK_REQ_UNITY 7
    !insertmacro CHECK_REQ_UNITY 8
    !insertmacro CHECK_REQ_UNITY 9
    !insertmacro CHECK_REQ_UNITY 10
    !insertmacro CHECK_REQ_UNITY 11
    !insertmacro CHECK_REQ_UNITY 12
    !insertmacro CHECK_REQ_UNITY 13
    !insertmacro CHECK_REQ_UNITY 14
    !insertmacro CHECK_REQ_UNITY 15
    !insertmacro CHECK_REQ_UNITY 16
    !insertmacro CHECK_REQ_UNITY 17
    !insertmacro CHECK_REQ_UNITY 18
    !insertmacro CHECK_REQ_UNITY 19
    !insertmacro CHECK_REQ_UNITY 20
    !insertmacro CHECK_REQ_UNITY 21
    !insertmacro CHECK_REQ_UNITY 22
    !insertmacro CHECK_REQ_UNITY 23
    !insertmacro CHECK_REQ_UNITY 24
    !insertmacro CHECK_REQ_UNITY 25
    !insertmacro CHECK_REQ_UNITY 26
    !insertmacro CHECK_REQ_UNITY 27

    ${If} $requires_unity == "true"
    ${AndIf} $unity_selected != "true"
        ${IfNot} ${FileExists} "$INSTDIR\Editor\Unity.Exe"
            Goto RequestUnityInstall
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
        ${If} $R0 != ${IN_VERSION_NICE}
            Goto RequestUnityInstall
        ${EndIf}
    ${EndIf}
    goto NormalInstall
RequestUnityInstall:
    MessageBox MB_OK|MB_ICONSTOP "You tried to install items that require Unity ${IN_VERSION_NICE}, but Unity is not installed at the specified location. Please either add Unity to your selection, or deselect the components which require Unity."
    StrCpy $R9 -1
    Call RelGotoPage
    Abort
NormalInstall:
FunctionEnd

Function CheckIDESelection
    # R6=monodev, R7=VS
    StrCpy $R6 1
    StrCpy $R7 1

    Var /Global i
    ${ForEach} $i 0 27 + 1
        SectionGetFlags $i $0

        IntOp $0 $0 & ${SF_SELECTED}
        SectionGetText $i $1

        ${If} $0 == 0
        ${AndIf} $1 == 'MonoDevelop / Unity Debugger'
            StrCpy $R6 0
        ${EndIf}

        ${If} $0 == 0
        ${AndIf} $1 == 'Microsoft Visual Studio Community 2017'
            StrCpy $R7 0
        ${EndIf}
    ${Next}

    ${If} $R6 == 0
    ${AndIf} $R7 == 0
        MessageBox MB_OKCANCEL "It is recommended to install either Visual Studio Community 2017, or MonoDevelop. Without them, you won't be able to debug your scripts. A different version of MonoDevelop would not work. If you change your mind - please press Cancel, and select one of them to be installed." IDOK proceed

    StrCpy $R9 -1
    Call RelGotoPage
    Abort

proceed:
    ${EndIf}

FunctionEnd

!macro SectionIsSelected nr
    ${If} ${SectionIsSelected} ${nr}
        IntOp $0 $0 + 1
    ${EndIf}
!macroend

Function CountSelectedSections
    StrCpy $0 "0"
    ${ForEach} $1 0 27 + 1
        ${If} ${SectionIsSelected} $1
            IntOp $0 $0 + 1
        ${EndIf}
    ${Next}
    Push $0
FunctionEnd

Function FolderSettingsPage
    ; Make sure that something is selected for installation
    Call CountSelectedSections
    Pop $0
    ${If} $0 == 0
        MessageBox MB_OK|MB_ICONEXCLAMATION "Please select one or more components"
        StrCpy $R9 -1
        Call RelGotoPage
        Abort
    ${EndIf}

    call CheckUnityRequirement
    call CheckIDESelection

    Var /Global Dialog
    Var /Global DownloadFolderText
    Var /Global DownloadFolderBrowseButton
    Var /Global InstallFolderText
    Var /Global InstallFolderBrowseButton
    Var /Global DownloadTempFolderRadioButton
    Var /Global DownloadToFolderRadioButton

    nsDialogs::Create 1018
    Pop $Dialog
    !insertmacro MUI_HEADER_TEXT "Choose Download and Install locations" "Specify where to download and install files."

    ${If} $Dialog == error
        Abort
    ${EndIf}

    ; Download folder settings
    ${NSD_CreateGroupBox} 0u 0u 100% 45u "Specify location of files downloaded during installation"

    ${NSD_CreateRadioButton} 7u 12u 280u 10u "Download files to &temporary location (will automatically be removed when done)"
    Pop $DownloadTempFolderRadioButton
    ${NSD_OnClick} $DownloadTempFolderRadioButton DownloadTempFolderRadioButton_OnClick

    ${NSD_CreateRadioButton} 7u 26u 60u 10u "&Download to:"
    Pop $DownloadToFolderRadioButton
    ${NSD_OnClick} $DownloadToFolderRadioButton DownloadToFolderRadioButton_OnClick

    ${If} $CustomDownloadFolder == ""
        Call GetUserDownloadsFolder
        Pop $0
        StrCpy $CustomDownloadFolder $0
    ${EndIf}

    ${NSD_CreateText} 69u 25u 170u 12u $CustomDownloadFolder
    Pop $DownloadFolderText

    ${NSD_OnChange} $DownloadFolderText DownloadFolderChanged

    ${NSD_CreateButton} 248u 24u 48u 14u "B&rowse..."
    Pop $DownloadFolderBrowseButton
    ${NSD_OnClick} $DownloadFolderBrowseButton DownloadFolderBrowse_OnClick

    ; Install folder setting
    ${NSD_CreateGroupBox} 0u 60u 100% 40u "Unity install folder"
    ${NSD_CreateText} 7u 75u 232u 12u ""
    Pop $InstallFolderText
    ${NSD_SetText} $InstallFolderText $INSTDIR
    ${NSD_OnChange} $InstallFolderText UpdateSpaceAvailableLabel

    ${NSD_CreateButton} 248u 74u 48u 14u "B&rowse..."
    Pop $InstallFolderBrowseButton
    ${NSD_OnClick} $InstallFolderBrowseButton InstallFolderBrowse_OnClick

    ; Find required space (in GB)
    Call GetInstalledSizeOfSelectedComponentsInMB
    Pop $5 ; Required install disk space
    Call GetDownloadSizeOfSelectedComponentsInMB
    Pop $6 ; Required download disk space
    IntOp $0 $5 + $6

    IntOp $1 $0 / 1024
    IntOp $2 $0 % 1024

    StrLen $3 $2
    ${If} "$3" == "3"
        StrCpy $2 $2 1
    ${Else}
        StrCpy $2 "0"
    ${EndIf}

    ${NSD_CreateLabel} 0 110u 100% 30u "Total space required: $1.$2 GB"

    Call GetSpaceAvailableText
    Pop $0
    ${NSD_CreateLabel} 0 120u 100% 30u $0
    Pop $SpaceAvailableLabel

    ; Update UI
    ${If} $DownloadToCustom != "true"
        ; Downloading to temp folder
        ${NSD_Check} $DownloadTempFolderRadioButton
        EnableWindow $DownloadFolderText 0
        EnableWindow $DownloadFolderBrowseButton 0
    ${Else}
        ; Downloading to specified folder
        ${NSD_Check} $DownloadToFolderRadioButton
        EnableWindow $DownloadFolderText 1
        EnableWindow $DownloadFolderBrowseButton 1
    ${EndIf}

    nsDialogs::Show
FunctionEnd

Function DownloadFolderChanged
    ${NSD_GetText} $DownloadFolderText $CustomDownloadFolder
FunctionEnd

Function GetSpaceAvailableText
    ; Find free disk space (in GB)
    StrCpy $0 $INSTDIR 2
    ${DriveSpace} $0 "/D=F /S=M" $0
    IntOp $1 $0 / 1024
    IntOp $2 $0 % 1024

    StrLen $3 $2
    ${If} "$3" == "3"
        StrCpy $2 $2 1
    ${Else}
        StrCpy $2 "0"
    ${EndIf}
    Push "Space available: $1.$2 GB"
FunctionEnd

Function UpdateSpaceAvailableLabel
    ${NSD_GetText} $InstallFolderText $INSTDIR
    Call GetSpaceAvailableText
    Pop $0
    ${NSD_SetText} $SpaceAvailableLabel $0
FunctionEnd

Function FolderSettingsPageLeave
    ${If} $DownloadToCustom != "true"
        StrCpy $DownloadFolder $PLUGINSDIR
    ${Else}
        StrCpy $DownloadFolder $CustomDownloadFolder
    ${EndIf}

    ${NSD_GetText} $InstallFolderText $INSTDIR

    call DownloadRemoteEULAs

    ; Verify free disk space before we can continue
    Call GetInstalledSizeOfSelectedComponentsInMB
    Pop $5 ; Required disk space
    Call GetDownloadSizeOfSelectedComponentsInMB
    Pop $0 ; Required disk space
    IntOp $0 $5 + $0

    StrCpy $1 $INSTDIR 2
    ${DriveSpace} $1 "/D=F /S=M" $1 ; Available disk space (in MB)

    ${If} $0 > $1
        ; Show disk space in GB
        IntOp $2 $0 / 1024
        IntOp $3 $0 % 1024
        StrCpy $3 $3 1
        IntOp $4 $1 / 1024
        IntOp $5 $1 % 1024
        StrCpy $5 $5 1
        MessageBox MB_YESNO|MB_ICONEXCLAMATION "It seems you do not have enough available disk space to download and install the selected components.$\n(Required: $2.$3 GB; Available: $4.$5 GB)$\n$\nInstall anyway?" IDYES +2
        Abort
    ${EndIf}

    ; Verify that specified download folder exists
    IfFileExists $DownloadFolder DontCreateFolder
        MessageBox MB_YESNO|MB_ICONQUESTION "The specified download folder does not exist. Create it?" IDYES DoCreateFolder
        Abort

DoCreateFolder:

        ClearErrors
        CreateDirectory $DownloadFolder
        ${If} ${Errors}
        ${OrIfNot} ${FileExists} $DownloadFolder
            MessageBox MB_ICONEXCLAMATION "The specified download folder could not be created"
            Abort
        ${EndIf}

DontCreateFolder:

    FileOpen $AutomationScriptFile "$DownloadFolder\install.bat" w
    FileWrite $AutomationScriptFile "REM Run this script to automatically install or reinstall all packages,$\r$\n"
    FileWrite $AutomationScriptFile "REM as it's performed by the Unity Download Assistant.$\r$\n"
    FileWrite $AutomationScriptFile "REM Needs to be run with superuser permissions.$\r$\n$\r$\n"

FunctionEnd

Function DownloadTempFolderRadioButton_OnClick
    EnableWindow $DownloadFolderText 0
    EnableWindow $DownloadFolderBrowseButton 0
    StrCpy $DownloadToCustom ""
FunctionEnd

Function DownloadToFolderRadioButton_OnClick
    EnableWindow $DownloadFolderText 1
    EnableWindow $DownloadFolderBrowseButton 1
    StrCpy $DownloadToCustom "true"
FunctionEnd

Function DownloadFolderBrowse_OnClick
    nsDialogs::SelectFolderDialog "Select download folder" "$DOCUMENTS"
    Pop $0
    ${If} $0 != "error"
        ${NSD_SetText} $DownloadFolderText $0
    ${EndIf}
FunctionEnd

Function InstallFolderBrowse_OnClick
    nsDialogs::SelectFolderDialog "Select Unity install folder" "$INSTDIR"
    Pop $0
    ${If} $0 != "error"
        ${NSD_SetText} $InstallFolderText $0
    ${EndIf}
FunctionEnd

Function ModifyRunCheckbox
    Var /Global keenioprojectkey
    Var /Global keenioapikey

    StrCpy $keenioprojectkey ${KEENIO_PROJECT_RELEASE}
    StrCpy $keenioapikey ${KEENIO_KEY_RELEASE}

    ${StrStr} $0 ${IN_VERSION_NICE} "b"
    ${StrStr} $1 ${IN_VERSION_NICE} "a"

    StrCmp "$0$1" "" NotBeta

        StrCpy $keenioprojectkey ${KEENIO_PROJECT_BETA}
        StrCpy $keenioapikey ${KEENIO_KEY_BETA}

NotBeta:

    ${If} $VSWasInstalled == "true"
        inetc::post '{"installed":"true"}' /HEADER "Content-Type: application/json" /TOSTACK '${KEENIO_API_URL}/projects/$keenioprojectkey/events/VSInstaller?api_key=$keenioapikey' "" /END
        Pop $0 ; error code
        Pop $0 ; response body
    ${Else}
        inetc::post '{"installed":"false"}' /HEADER "Content-Type: application/json" /TOSTACK '${KEENIO_API_URL}/projects/$keenioprojectkey/events/VSInstaller?api_key=$keenioapikey' "" /END
        Pop $0 ; error code
        Pop $0 ; response body
    ${EndIf}

    ; Don't show "Launch Unity" option on final page if we didn't install Unity
    IfFileExists "$INSTDIR\Editor\${PRODUCT_EXE_NAME}" +3 0
        SendMessage $mui.FinishPage.Run ${BM_SETCHECK} ${BST_UNCHECKED} 0
        ShowWindow $mui.FinishPage.Run 0
FunctionEnd

!macro DEF_SECTION nr
Section /o "" SEC_COMPONENT${nr}
    Push $SectionComponentName${nr}
    Call DownloadAndExecuteComponent
SectionEnd
!macroend

!insertmacro DEF_SECTION 0
!insertmacro DEF_SECTION 1
!insertmacro DEF_SECTION 2
!insertmacro DEF_SECTION 3
!insertmacro DEF_SECTION 4
!insertmacro DEF_SECTION 5
!insertmacro DEF_SECTION 6
!insertmacro DEF_SECTION 7
!insertmacro DEF_SECTION 8
!insertmacro DEF_SECTION 9
!insertmacro DEF_SECTION 10
!insertmacro DEF_SECTION 11
!insertmacro DEF_SECTION 12
!insertmacro DEF_SECTION 13
!insertmacro DEF_SECTION 14
!insertmacro DEF_SECTION 15
!insertmacro DEF_SECTION 16
!insertmacro DEF_SECTION 17
!insertmacro DEF_SECTION 18
!insertmacro DEF_SECTION 19
!insertmacro DEF_SECTION 20
!insertmacro DEF_SECTION 21
!insertmacro DEF_SECTION 22
!insertmacro DEF_SECTION 23
!insertmacro DEF_SECTION 24
!insertmacro DEF_SECTION 25
!insertmacro DEF_SECTION 26
!insertmacro DEF_SECTION 27

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT0} $SectionComponentDesc0
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT1} $SectionComponentDesc1
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT2} $SectionComponentDesc2
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT3} $SectionComponentDesc3
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT4} $SectionComponentDesc4
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT5} $SectionComponentDesc5
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT6} $SectionComponentDesc6
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT7} $SectionComponentDesc7
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT8} $SectionComponentDesc8
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT9} $SectionComponentDesc9
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT10} $SectionComponentDesc10
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT11} $SectionComponentDesc11
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT12} $SectionComponentDesc12
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT13} $SectionComponentDesc13
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT14} $SectionComponentDesc14
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT15} $SectionComponentDesc15
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT16} $SectionComponentDesc16
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT17} $SectionComponentDesc17
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT18} $SectionComponentDesc18
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT19} $SectionComponentDesc19
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT20} $SectionComponentDesc20
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT21} $SectionComponentDesc21
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT22} $SectionComponentDesc22
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT23} $SectionComponentDesc23
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT24} $SectionComponentDesc24
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT25} $SectionComponentDesc25
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT26} $SectionComponentDesc26
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPONENT27} $SectionComponentDesc27
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; Sections/components are compile-time in NSIS, so in order to have ini-defined
; inter-component dependencies, we need to unroll everything, and put it in macros.
;
; What you see below is essentially an unrolled loop, similar to this pseudocode:
;
; for (i = 0; i <= MAX_SECTIONS; i++) {
;     for (j = 0; j <= MAX_SECTIONS; j++) {
;         if (inisections[j].sync == inisections[i].name) {
;             sections[j].flags = sections[i].flags;
;         }
;     }
; }

!macro SYNC_INNER SyncJ SectionNameI SecComponentJ SecComponentI
    ; skip unused sections
    StrCmp "" ${SectionNameI} +5

    ; if "sync" from ini file matches the [section name] of the component -- copy the flags
    StrCmp ${SyncJ} ${SectionNameI} 0 +4

    Push $0
    SectionGetFlags ${SecComponentI} $0
    SectionSetFlags ${SecComponentJ} $0
    Pop $0
!macroend

!macro SYNC_ITERATE_I SectionNameI SecComponentI
    !insertmacro SYNC_INNER $SectionComponentSync0 ${SectionNameI} ${SEC_COMPONENT0} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync1 ${SectionNameI} ${SEC_COMPONENT1} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync2 ${SectionNameI} ${SEC_COMPONENT2} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync3 ${SectionNameI} ${SEC_COMPONENT3} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync4 ${SectionNameI} ${SEC_COMPONENT4} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync5 ${SectionNameI} ${SEC_COMPONENT5} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync6 ${SectionNameI} ${SEC_COMPONENT6} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync7 ${SectionNameI} ${SEC_COMPONENT7} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync8 ${SectionNameI} ${SEC_COMPONENT8} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync9 ${SectionNameI} ${SEC_COMPONENT9} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync10 ${SectionNameI} ${SEC_COMPONENT10} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync11 ${SectionNameI} ${SEC_COMPONENT11} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync12 ${SectionNameI} ${SEC_COMPONENT12} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync13 ${SectionNameI} ${SEC_COMPONENT13} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync14 ${SectionNameI} ${SEC_COMPONENT14} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync15 ${SectionNameI} ${SEC_COMPONENT15} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync16 ${SectionNameI} ${SEC_COMPONENT16} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync17 ${SectionNameI} ${SEC_COMPONENT17} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync18 ${SectionNameI} ${SEC_COMPONENT18} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync19 ${SectionNameI} ${SEC_COMPONENT19} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync20 ${SectionNameI} ${SEC_COMPONENT20} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync21 ${SectionNameI} ${SEC_COMPONENT21} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync22 ${SectionNameI} ${SEC_COMPONENT22} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync23 ${SectionNameI} ${SEC_COMPONENT23} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync24 ${SectionNameI} ${SEC_COMPONENT24} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync25 ${SectionNameI} ${SEC_COMPONENT25} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync26 ${SectionNameI} ${SEC_COMPONENT26} ${SecComponentI}
    !insertmacro SYNC_INNER $SectionComponentSync27 ${SectionNameI} ${SEC_COMPONENT27} ${SecComponentI}
!macroend


Function .onSelChange
    call SelChangedHandler

    ; If we end up here, means a component was enabled or disabled.
    ; We need to find all components, which depend on the one changed, and sync them.

    !insertmacro SYNC_ITERATE_I $SectionComponentName0 ${SEC_COMPONENT0}
    !insertmacro SYNC_ITERATE_I $SectionComponentName1 ${SEC_COMPONENT1}
    !insertmacro SYNC_ITERATE_I $SectionComponentName2 ${SEC_COMPONENT2}
    !insertmacro SYNC_ITERATE_I $SectionComponentName3 ${SEC_COMPONENT3}
    !insertmacro SYNC_ITERATE_I $SectionComponentName4 ${SEC_COMPONENT4}
    !insertmacro SYNC_ITERATE_I $SectionComponentName5 ${SEC_COMPONENT5}
    !insertmacro SYNC_ITERATE_I $SectionComponentName6 ${SEC_COMPONENT6}
    !insertmacro SYNC_ITERATE_I $SectionComponentName7 ${SEC_COMPONENT7}
    !insertmacro SYNC_ITERATE_I $SectionComponentName8 ${SEC_COMPONENT8}
    !insertmacro SYNC_ITERATE_I $SectionComponentName9 ${SEC_COMPONENT9}
    !insertmacro SYNC_ITERATE_I $SectionComponentName10 ${SEC_COMPONENT10}
    !insertmacro SYNC_ITERATE_I $SectionComponentName11 ${SEC_COMPONENT11}
    !insertmacro SYNC_ITERATE_I $SectionComponentName12 ${SEC_COMPONENT12}
    !insertmacro SYNC_ITERATE_I $SectionComponentName13 ${SEC_COMPONENT13}
    !insertmacro SYNC_ITERATE_I $SectionComponentName14 ${SEC_COMPONENT14}
    !insertmacro SYNC_ITERATE_I $SectionComponentName15 ${SEC_COMPONENT15}
    !insertmacro SYNC_ITERATE_I $SectionComponentName16 ${SEC_COMPONENT16}
    !insertmacro SYNC_ITERATE_I $SectionComponentName17 ${SEC_COMPONENT17}
    !insertmacro SYNC_ITERATE_I $SectionComponentName18 ${SEC_COMPONENT18}
    !insertmacro SYNC_ITERATE_I $SectionComponentName19 ${SEC_COMPONENT19}
    !insertmacro SYNC_ITERATE_I $SectionComponentName20 ${SEC_COMPONENT20}
    !insertmacro SYNC_ITERATE_I $SectionComponentName21 ${SEC_COMPONENT21}
    !insertmacro SYNC_ITERATE_I $SectionComponentName22 ${SEC_COMPONENT22}
    !insertmacro SYNC_ITERATE_I $SectionComponentName23 ${SEC_COMPONENT23}
    !insertmacro SYNC_ITERATE_I $SectionComponentName24 ${SEC_COMPONENT24}
    !insertmacro SYNC_ITERATE_I $SectionComponentName25 ${SEC_COMPONENT25}
    !insertmacro SYNC_ITERATE_I $SectionComponentName26 ${SEC_COMPONENT26}
    !insertmacro SYNC_ITERATE_I $SectionComponentName27 ${SEC_COMPONENT27}
FunctionEnd

!macro PROCESS_SECTION_INIDATA nr
    ${Case} ${nr}
        StrCpy $SectionComponentName${nr} $IniSectionName
        StrCpy $SectionComponentTitle${nr} $SectionText
        StrCpy $SectionComponentDesc${nr} "$2$\n$\nDownload size: $5"
        StrCpy $SectionComponentSize${nr} $7
        StrCpy $SectionComponentSync${nr} $1
        ${Break}
!macroend

Function DownloadAndReadIniFile
    Var /Global IniFile
    Var /Global IniLine
    Var /Global IniSectionName
    Var /Global SectionIndex
    Var /Global SectionText
    Var /Global SectionFlags
    Var /Global HaveDownloadedIni

    ${If} $HaveDownloadedIni == "true"
        Return
    ${EndIf}

    StrCpy $SectionIndex 0

    StrCpy $IniFileName "$PLUGINSDIR\UnityDownloadAssistant.ini"
    Push $IniFileName
    Call DownloadIniFile
    Pop $ActualIniUrl

    ; read through file and find sections (which then are read using ReadIni methods)
    ClearErrors
    FileOpen $IniFile "$IniFileName" r
    IfErrors 0 +3
        MessageBox MB_ICONSTOP "Failed to read from file '$IniFileName' (download succeeded).$\n$\nCannot continue. Alternatively download and install the individual components separately from http://www.unity3d.com. Thanks."
        Abort

    ReadLine:
        FileRead $IniFile $IniLine
        StrCmp $IniLine "" EndOfIniFile

        ${StrStr} $IniSectionName $IniLine "[" ; contains "["?
        StrCmp $IniSectionName "" ReadLine

        ${StrRep} $IniSectionName $IniSectionName "[" "" ; remove start [
        ${StrLoc} $0 $IniSectionName "]" ">"             ; find "]" in line
        StrCmp $0 "" ReadLine                            ; if no "]" found, continue to next line

        StrCpy $IniSectionName $IniSectionName $0        ; copy until "]"

        ; hide the section if hidden==true
        ReadINIStr $1 $IniFileName "$IniSectionName" "hidden"
        ${If} $1 == "true"
            StrCpy $SectionText ""
        ${Else}
            ReadINIStr $SectionText $IniFileName "$IniSectionName" "title"
        ${EndIf}

        ; sync the enable state if specified
        ReadINIStr $1 $IniFileName "$IniSectionName" "sync"
        ReadINIStr $2 $IniFileName "$IniSectionName" "description"

        ReadINIStr $4 $IniFileName "$IniSectionName" "mandatory"
        ReadINIStr $5 $IniFileName "$IniSectionName" "size_h"         ; human-readable download size (in GB, MB, KB)
        ReadINIStr $6 $IniFileName "$IniSectionName" "installedsize"  ; Kbytes
        ReadINIStr $7 $IniFileName "$IniSectionName" "size"           ; download size in Kbytes


        ReadRegDWORD $3 HKCU "${UNITY_INSTALLER_REGKEY_ROOT}" "Select_$IniSectionName"
        ${If} $3 == 1
            StrCpy $3 "true"
        ${ElseIf} $3 == 0
            StrCpy $3 "false"
        ${Else}
            ReadINIStr $3 $IniFileName "$IniSectionName" "install"
        ${EndIf}

        ; hide and disable VisualStudio if already installed
        ${If} $IniSectionName == "VisualStudio"
        ${AndIf} $VSInstalled == "true"
            ${If} $VSWorkloadInstalled == "true"
            ${OrIf} $VSEdition != "Community"
                StrCpy $SectionText ""
                StrCpy $3 "false"
            ${Endif}
        ${EndIf}
        
        ${If} $IniSectionName == "VisualStudio"
        ${AndIf} $VSInstalled != "true"
            StrCpy $VSSectionIndex $SectionIndex
        ${EndIf}

        ; install VS2017 Professional Unity workload if VS2017 Professional
        ; was found and Unity workload was not installed.
        ${If} $IniSectionName == "VisualStudioProfessionalUnityWorkload"
            ${If} $VSWorkloadInstalled != "true"
            ${AndIf} $VSEdition == "Professional"
                StrCpy $3 "true"
                StrCpy $VSSectionIndex $SectionIndex
            ${Else}
                StrCpy $SectionText ""
                StrCpy $3 "false"
            ${EndIf}
        ${EndIf}

        ; install VS2017 Enterprise Unity workload if VS2017 Enterprise
        ; was found and Unity workload was not installed.
        ${If} $IniSectionName == "VisualStudioEnterpriseUnityWorkload"
            ${If} $VSWorkloadInstalled != "true"
            ${AndIf} $VSEdition == "Enterprise"
                StrCpy $3 "true"
                StrCpy $VSSectionIndex $SectionIndex
            ${Else}
                StrCpy $SectionText ""
                StrCpy $3 "false"
            ${EndIf}
        ${EndIf}

        # Set flags (selected, readonly)
        StrCpy $SectionFlags 0
        ${If} $3 == "true" ; install?
            IntOp $SectionFlags $SectionFlags | ${SF_SELECTED}
        ${EndIf}

        ${If} $4 == "true" ; mandatory?
            IntOp $SectionFlags $SectionFlags | ${SF_RO}
        ${EndIf}

        # Set installer component options
        SectionSetText $SectionIndex $SectionText
        SectionSetFlags $SectionIndex $SectionFlags
        SectionSetSize $SectionIndex $6

        ${Switch} $SectionIndex
            !insertmacro PROCESS_SECTION_INIDATA 0
            !insertmacro PROCESS_SECTION_INIDATA 1
            !insertmacro PROCESS_SECTION_INIDATA 2
            !insertmacro PROCESS_SECTION_INIDATA 3
            !insertmacro PROCESS_SECTION_INIDATA 4
            !insertmacro PROCESS_SECTION_INIDATA 5
            !insertmacro PROCESS_SECTION_INIDATA 6
            !insertmacro PROCESS_SECTION_INIDATA 7
            !insertmacro PROCESS_SECTION_INIDATA 8
            !insertmacro PROCESS_SECTION_INIDATA 9
            !insertmacro PROCESS_SECTION_INIDATA 10
            !insertmacro PROCESS_SECTION_INIDATA 11
            !insertmacro PROCESS_SECTION_INIDATA 12
            !insertmacro PROCESS_SECTION_INIDATA 13
            !insertmacro PROCESS_SECTION_INIDATA 14
            !insertmacro PROCESS_SECTION_INIDATA 15
            !insertmacro PROCESS_SECTION_INIDATA 16
            !insertmacro PROCESS_SECTION_INIDATA 17
            !insertmacro PROCESS_SECTION_INIDATA 18
            !insertmacro PROCESS_SECTION_INIDATA 19
            !insertmacro PROCESS_SECTION_INIDATA 20
            !insertmacro PROCESS_SECTION_INIDATA 21
            !insertmacro PROCESS_SECTION_INIDATA 22
            !insertmacro PROCESS_SECTION_INIDATA 23
            !insertmacro PROCESS_SECTION_INIDATA 24
            !insertmacro PROCESS_SECTION_INIDATA 25
            !insertmacro PROCESS_SECTION_INIDATA 26
            !insertmacro PROCESS_SECTION_INIDATA 27
        ${EndSwitch}
        IntOp $SectionIndex $SectionIndex + 1

        Goto ReadLine
    EndOfIniFile:

    FileClose $IniFile

    StrCpy $HaveDownloadedIni "true"
FunctionEnd

Function DownloadAndExecuteComponent
    Var /Global ComponentTitle
    Var /Global Url
    Var /Global IniFileUrlWithoutFileName
    Var /Global FileName
    Var /Global Command
    Var /Global ScriptCommand
    Var /Global use_vs_editor
    Var /Global md5

    Pop $ComponentName ; e.g. "ExampleProjectInstaller"

    ${If} $DownloadFolder == ""
        StrCpy $DownloadFolder $PLUGINSDIR
    ${EndIf}

    ReadINIStr $ComponentTitle "$IniFileName" "$ComponentName" "title"
    ReadINIStr $Url "$IniFileName" "$ComponentName" "url"
    ReadINIStr $md5 "$IniFileName" "$ComponentName" "md5"
    ReadINIStr $FileName "$IniFileName" "$ComponentName" "filename"
    ReadINIStr $Command "$IniFileName" "$ComponentName" "cmd"
    ReadINIStr $use_vs_editor "$IniFileName" "$ComponentName" "use_vs_editor"

    ${If} $Command == ""
        MessageBox MB_OK|MB_ICONEXCLAMATION "Ini file doesn't contain cmd for component $ComponentName. Don't know how to install this"
        Return
    ${EndIf}

    Push $ActualIniUrl
    Call GetUrlWithoutFileName
    Pop $IniFileUrlWithoutFileName

    ; do we have full url specified?
    ${StrLoc} $0 $Url "://" ">"  ; search for "http(s)://"
    ${If} $0 == ""               ; not a full url -> append ini file location
        StrCpy $Url "$IniFileUrlWithoutFileName/$Url"
    ${EndIf}

    ${If} $FileName == ""
        Push $Url
        Call GetFileNameFromUrl
        Pop $FileName
    ${EndIf}

    StartDownloadAndInstall:
        DetailPrint "Downloading $Url"

        inetc::get /Resume "Internet connection lost.$\nPlease re-connect and click Retry." /Question "Are you sure you want to cancel the download?" /Caption "Downloading $ComponentTitle" $Url "$DownloadFolder\$FileName" /End
        Pop $0

        ${If} $0 == "OK"
            ${If} $md5 != ""
                DetailPrint "Checking $ComponentTitle..."
                md5dll::GetMD5File "$DownloadFolder\$FileName"
                Pop $0

                ${If} $0 != $md5
                    MessageBox MB_CANCELTRYCONTINUE|MB_ICONEXCLAMATION "Download of '$ComponentTitle' is corrupted. This could be caused by a connection problem, or by a corrupt or incomplete file on the server.$\n$\nDo you want to try again to download it, continue without this module, or cancel the entire Unity installation?" IDTRYAGAIN StartDownloadAndInstall IDCONTINUE SkipExecutingComponent
                    Abort
                ${EndIf}
            ${EndIf}

            DetailPrint "Installing $ComponentTitle..."

            ${StrRep} $Command $Command "{INSTDIR}" "$INSTDIR"

            ${StrRep} $Command $Command "{MODULEDIR}" "$INSTDIR"
            ${StrRep} $Command $Command "{DOCDIR}" "$INSTDIR"

            StrCpy $ScriptCommand $Command
            ${StrRep} $Command $Command "{FILENAME}" "$DownloadFolder\$FileName"
            ${StrRep} $ScriptCommand $ScriptCommand "{FILENAME}" "$FileName"

            ClearErrors
            ExecWait $Command $1

            ; code 3010 (0xbc2) is "Successful installation, restart required".
            ${If} $1 == "3010"
            ${AndIf} $ComponentName == "VisualStudio"
                StrCpy $1 "0"
                SetRebootFlag true
            ${EndIf}
            ${If} $1 != "0"
                MessageBox MB_CANCELTRYCONTINUE|MB_ICONEXCLAMATION "Error while installing $ComponentName. (Error code: $1)" IDTRYAGAIN StartDownloadAndInstall IDCONTINUE EndDownloadAndExecuteComponent
                MessageBox MB_OK|MB_ICONEXCLAMATION "${INSTALLER_ABORT_MESSAGE}"
                Quit
            ${EndIf}

            Delete $ResultFile
            ${If} $1 == "0"
                IfFileExists $ResultFile 0 PastUnityInstallerResultCheck

                FileOpen $4 $ResultFile r
                FileRead $4 $1
                FileClose $4

                ${If} $1 == ${REBOOT_CODE}
                    SetRebootFlag true
                ${EndIf}
            ${EndIf}

        PastUnityInstallerResultCheck:

        ${ElseIf} $0 == "Cancelled"
            MessageBox MB_OK|MB_ICONEXCLAMATION "${INSTALLER_ABORT_MESSAGE}"
            Quit
        ${ElseIf} $0 != "SendRequest Error"
            MessageBox MB_CANCELTRYCONTINUE|MB_ICONEXCLAMATION "There is a problem downloading $ComponentName from $Url. The error message is: $0." IDTRYAGAIN StartDownloadAndInstall IDCONTINUE EndDownloadAndExecuteComponent
            Quit
        ${Else}
            Quit
        ${EndIf}

    EndDownloadAndExecuteComponent:

    FileWrite $AutomationScriptFile "$ScriptCommand$\r$\n"
    ${If} $use_vs_editor == "true" ; ini file hack, which tells that this component installation should set VS as Unity script editor
        WriteRegStr HKCU "${UNITY_PREFS_REGKEY}" "kScriptsDefaultApp_h2657262712" "$VSDevEnvPath"
    ${EndIf}

    ${If} $use_vs_editor == "true" ; ini file hack, which tells that this component installation should set VS as Unity script editor
        ; for analytics (keen.io)
        StrCpy $VSWasInstalled "true"
    ${EndIf}

    SkipExecutingComponent:

FunctionEnd

Function DownloadIniFile
    ; $0: Ini file path (on local machine)
    ; $1: Download status
    ; $2: Actual ini url

    ; try to load from file, specified via -ini
    IfFileExists $BOOTSTRAPPER_INI_URL1 0 NotLocalFile
        ; File found, copy to the expected location
        Pop $0
        CopyFiles $BOOTSTRAPPER_INI_URL1 $0
        Push $BOOTSTRAPPER_INI_URL1
        Return

    NotLocalFile:

    # Disable Cancel/Next buttons while ini is downloaded
    GetDlgItem $0 $HWNDPARENT 1
    EnableWindow $0 0
    GetDlgItem $0 $HWNDPARENT 2
    EnableWindow $0 0

    Pop $0

    DoDownloadIniFile:
        inetc::get $BOOTSTRAPPER_INI_URL1 "$0" /End
        Pop $1
        StrCpy $2 $BOOTSTRAPPER_INI_URL1
        ${If} $1 != "OK"
        ${AndIf} $BOOTSTRAPPER_INI_URL2 != ""
            inetc::get $BOOTSTRAPPER_INI_URL2 "$0" /End
            Pop $1
            StrCpy $2 $BOOTSTRAPPER_INI_URL2

            ${If} $1 != "OK"
            ${AndIf} $BOOTSTRAPPER_INI_URL3 != ""
                inetc::get $BOOTSTRAPPER_INI_URL3 "$0" /End
                Pop $1
                StrCpy $2 $BOOTSTRAPPER_INI_URL3

                ${If} $1 != "OK"
                ${AndIf} $BOOTSTRAPPER_INI_URL4 != ""
                    inetc::get $BOOTSTRAPPER_INI_URL4 "$0" /End
                    Pop $1
                    StrCpy $2 $BOOTSTRAPPER_INI_URL4
                ${EndIf}
            ${EndIf}
        ${EndIf}

        # Re-enable the Cancel button
        GetDlgItem $0 $HWNDPARENT 2
        EnableWindow $0 1

        ${If} $1 != "OK"
            MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION "Error '$1' while downloading ini file from $2$\n$\nPlease verify that your are connected to Internet and your firewall allows downloading files. Alternatively download and install the individual components separately from http://www.unity3d.com" IDRETRY DoDownloadIniFile
            Quit
        ${EndIf}

        Push $2 ; Actual ini file url
FunctionEnd

Function GetUrlWithoutFileName
    ; $0: Full ini url (argument)
    ; $1: IniFileUrlWithoutFileName (result)
    ; $2: Length of full url
    ; $3: Temp char
    Pop $0

    StrCpy $1 $0
    StrLen $2 $1
    ; remove from end, until "/" is found
    LoopGetUrlWithoutFileName:
        IntOp $2 $2 - 1
        StrCpy $3 $1 1 $2
        StrCmp $3 "/" 0 LoopGetUrlWithoutFileName

    StrCpy $1 $0 $2
    Push $1
FunctionEnd

Function GetFileNameFromUrl
    ; Find filename part of url (for storing locally) - by searching for "/" from end of url
    ; $0: Full ini url (argument)
    ; $1: FileName (result)
    ; $2: Length of url
    ; $3: Temp char
    Pop $0

    StrLen $2 $0
    LoopGetFileNameFromUrl:
        IntOp $2 $2 - 1
        StrCpy $3 $0 1 $2
        StrCmp $3 "/" 0 LoopGetFileNameFromUrl

    IntOp $2 $2 + 1
    StrCpy $1 $0 "" $2

    Push $1
FunctionEnd

Function GetUserDownloadsFolder
    ; Code from http://forums.winamp.com/showthread.php?t=288087
    System::Call 'shell32::SHGetKnownFolderPath(g "{374DE290-123F-4565-9164-39C4925E467B}", i 0x1000, in, *i.r1)i.r0'
    ${If} $0 == 0
        System::Call /NoUnload 'kernel32::lstrlenW(i $1)i.r2'
        IntOp $2 $2 * 2
        System::Call /NoUnload '*$1(&w$2 .r2)'
        System::Call 'ole32::CoTaskMemFree(i $1)'
    ${endif}
    Push $2
FunctionEnd

; Returns installed size in MB of specified section if selected
!macro AddInstalledSizeIfComponentIsSelected SectionName
    ${If} ${SectionIsSelected} ${SectionName}
        SectionGetSize ${SectionName} $1
        IntOp $1 $1 / 1024
        IntOp $0 $0 + $1
    ${EndIf}
!macroend

Function GetInstalledSizeOfSelectedComponentsInMB
    StrCpy $0 "0" ; total size of selected sections

    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT0}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT1}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT2}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT3}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT4}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT5}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT6}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT7}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT8}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT9}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT10}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT11}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT12}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT13}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT14}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT15}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT16}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT17}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT18}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT19}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT20}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT21}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT22}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT23}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT24}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT25}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT26}
    !InsertMacro AddInstalledSizeIfComponentIsSelected ${SEC_COMPONENT27}

    Push $0
FunctionEnd

; Returns download size in MB of specified section if selected
!macro AddDownloadSizeIfComponentIsSelected SectionName ComponentNumber
    ${If} ${SectionIsSelected} ${SectionName}
        StrCpy $1 $SectionComponentSize${ComponentNumber}
        IntOp $1 $1 / 1024
        IntOp $0 $0 + $1
    ${EndIf}
!macroend

Function GetDownloadSizeOfSelectedComponentsInMB
    StrCpy $0 "0" ; total size of selected sections

    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT0} 0
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT1} 1
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT2} 2
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT3} 3
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT4} 4
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT5} 5
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT6} 6
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT7} 7
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT8} 8
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT9} 9
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT10} 10
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT11} 11
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT12} 12
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT13} 13
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT14} 14
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT15} 15
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT16} 16
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT17} 17
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT18} 18
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT19} 19
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT20} 20
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT21} 21
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT22} 22
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT23} 23
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT24} 24
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT25} 25
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT26} 26
    !InsertMacro AddDownloadSizeIfComponentIsSelected ${SEC_COMPONENT27} 27

    Push $0
FunctionEnd

!macro CheckComponentEULA nr
    StrCpy $EULA_URL_1_${nr} ""
    StrCpy $EULA_URL_2_${nr} ""
    StrCpy $EULA_URL_3_${nr} ""
    StrCpy $EULA_LABEL_1_${nr} ""
    StrCpy $EULA_LABEL_2_${nr} ""
    StrCpy $EULA_LABEL_3_${nr} ""
    ${If} ${SectionIsSelected} ${SEC_COMPONENT${nr}}
        ReadINIStr $EULA_URL_1_${nr} $IniFileName $SectionComponentName${nr} "eulaurl1"
        ReadINIStr $EULA_URL_2_${nr} $IniFileName $SectionComponentName${nr} "eulaurl2"
        ReadINIStr $EULA_URL_3_${nr} $IniFileName $SectionComponentName${nr} "eulaurl3"
        ReadINIStr $EULA_LABEL_1_${nr} $IniFileName $SectionComponentName${nr} "eulalabel1"
        ReadINIStr $EULA_LABEL_2_${nr} $IniFileName $SectionComponentName${nr} "eulalabel2"
        ReadINIStr $EULA_LABEL_3_${nr} $IniFileName $SectionComponentName${nr} "eulalabel3"
        ReadINIStr $EULABanner${nr} $IniFileName $SectionComponentName${nr} "eulamessage"
    ${EndIf}
!macroend

Function DownloadRemoteEULAs
    !InsertMacro CheckComponentEULA 0
    !InsertMacro CheckComponentEULA 1
    !InsertMacro CheckComponentEULA 2
    !InsertMacro CheckComponentEULA 3
    !InsertMacro CheckComponentEULA 4
    !InsertMacro CheckComponentEULA 5
    !InsertMacro CheckComponentEULA 6
    !InsertMacro CheckComponentEULA 7
    !InsertMacro CheckComponentEULA 8
    !InsertMacro CheckComponentEULA 9
    !InsertMacro CheckComponentEULA 10
    !InsertMacro CheckComponentEULA 11
    !InsertMacro CheckComponentEULA 12
    !InsertMacro CheckComponentEULA 13
    !InsertMacro CheckComponentEULA 14
    !InsertMacro CheckComponentEULA 15
    !InsertMacro CheckComponentEULA 16
    !InsertMacro CheckComponentEULA 17
    !InsertMacro CheckComponentEULA 18
    !InsertMacro CheckComponentEULA 19
    !InsertMacro CheckComponentEULA 20
    !InsertMacro CheckComponentEULA 21
    !InsertMacro CheckComponentEULA 22
    !InsertMacro CheckComponentEULA 23
    !InsertMacro CheckComponentEULA 24
    !InsertMacro CheckComponentEULA 25
    !InsertMacro CheckComponentEULA 26
    !InsertMacro CheckComponentEULA 27
FunctionEnd

Var EULA_URL_1
Var EULA_URL_2
Var EULA_URL_3
Var EULA_LABEL_1
Var EULA_LABEL_2
Var EULA_LABEL_3

Function ComponentRemoteEULA_Show
    Pop $ComponentTitle
    Pop $EULABanner
    Pop $EULA_LABEL_3
    Pop $EULA_LABEL_2
    Pop $EULA_LABEL_1
    Pop $EULA_URL_3
    Pop $EULA_URL_2
    Pop $EULA_URL_1

    ${If} $EULA_URL_1 == ""
        Return
    ${EndIf}

    !insertmacro MUI_HEADER_TEXT `$(MUI_TEXT_LICENSE_TITLE)` $EULABanner

    GetDlgItem $R0 $HWNDPARENT 1
    EnableWindow $R0 0

    nsDialogs::Create 1018
    Pop $R0

    ${NSD_CreateLabel} 0 10 100% 12u $EULA_LABEL_1
    Pop $0
    Linker::link /NOUNLOAD $0 $EULA_URL_1

    ${If} $EULA_URL_2 != ""
        ${NSD_CreateLabel} 0 35 100% 12u $EULA_LABEL_2
        Pop $0
        Linker::link /NOUNLOAD $0 $EULA_URL_2
    ${EndIf}

    ${If} $EULA_URL_3 != ""
        ${NSD_CreateLabel} 0 60 100% 12u $EULA_LABEL_3
        Pop $0
        Linker::link /NOUNLOAD $0 $EULA_URL_3
    ${EndIf}

    ${NSD_CreateCheckBox} 0 -20u 160u 12u `$(^AcceptBtn)`
    Pop $CheckBoxAgree
    ${NSD_OnClick} $CheckBoxAgree ComponentEULA_CheckBoxAgree_Click

    nsDialogs::Show

FunctionEnd

!macro DEF_COMPEULAPAGE nr
Function ComponentEULA_Show${nr}
    Push $EULA_URL_1_${nr}
    Push $EULA_URL_2_${nr}
    Push $EULA_URL_3_${nr}
    Push $EULA_LABEL_1_${nr}
    Push $EULA_LABEL_2_${nr}
    Push $EULA_LABEL_3_${nr}
    Push $EULABanner${nr}
    Push $SectionComponentTitle${nr}
    call ComponentRemoteEULA_Show
FunctionEnd
!macroend

!insertmacro DEF_COMPEULAPAGE 0
!insertmacro DEF_COMPEULAPAGE 1
!insertmacro DEF_COMPEULAPAGE 2
!insertmacro DEF_COMPEULAPAGE 3
!insertmacro DEF_COMPEULAPAGE 4
!insertmacro DEF_COMPEULAPAGE 5
!insertmacro DEF_COMPEULAPAGE 6
!insertmacro DEF_COMPEULAPAGE 7
!insertmacro DEF_COMPEULAPAGE 8
!insertmacro DEF_COMPEULAPAGE 9
!insertmacro DEF_COMPEULAPAGE 10
!insertmacro DEF_COMPEULAPAGE 11
!insertmacro DEF_COMPEULAPAGE 12
!insertmacro DEF_COMPEULAPAGE 13
!insertmacro DEF_COMPEULAPAGE 14
!insertmacro DEF_COMPEULAPAGE 15
!insertmacro DEF_COMPEULAPAGE 16
!insertmacro DEF_COMPEULAPAGE 17
!insertmacro DEF_COMPEULAPAGE 18
!insertmacro DEF_COMPEULAPAGE 19
!insertmacro DEF_COMPEULAPAGE 20
!insertmacro DEF_COMPEULAPAGE 21
!insertmacro DEF_COMPEULAPAGE 22
!insertmacro DEF_COMPEULAPAGE 23
!insertmacro DEF_COMPEULAPAGE 24
!insertmacro DEF_COMPEULAPAGE 25
!insertmacro DEF_COMPEULAPAGE 26
!insertmacro DEF_COMPEULAPAGE 27

Function ComponentEULA_Leave

FunctionEnd

Function WelLeaveDownloadEULA_Custom
    call WelLeaveDownloadEULA

    ; Set install dir
    ${If} $INSTDIR == "" ; If specified on command line, $INSTDIR will already be set here
        ReadRegStr $INSTDIR HKCU "${UNITY_INSTALLER_REGKEY}" "Location x64"
        ${If} $INSTDIR == ""
            ; No installation found, suggest default location
            StrCpy $INSTDIR "$PROGRAMFILES64\${INSTALL_FOLDER}"
        ${EndIf}
    ${EndIf}

    Call DownloadAndReadIniFile
FunctionEnd

Function ComponentEULA_CheckBoxAgree_Click
    ${NSD_GetState} $CheckBoxAgree $R0
    ${If} $R0 == ${BST_CHECKED}
        GetDlgItem $R0 $HWNDPARENT 1
        EnableWindow $R0 1
    ${Else}
        GetDlgItem $R0 $HWNDPARENT 1
        EnableWindow $R0 0
    ${EndIf}
FunctionEnd

!include "..\WindowsEditor\LockedList.nsh"
