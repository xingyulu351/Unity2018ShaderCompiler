; Windows Unity Example installer

!include WinVer.nsh
!include x64.nsh
!include "..\WindowsShared\WinDetection.nsh"

!include UnityPaths.nsh
!define PRODUCT_NAME "Documentation for Unity${IN_CUSTOM_NAME} ${IN_VERSION_NICE}"

!define INSTALL_FOLDER "${IN_INSTALL_FOLDER}"
!define UNITY_INSTALLER_REGKEY_ROOT "Software\Unity Technologies\Installer"
!define UNITY_INSTALLER_REGKEY "${UNITY_INSTALLER_REGKEY_ROOT}\${IN_INSTALL_FOLDER}"

OutFile "..\..\..\build\WindowsDocumentationInstaller\UnityDocumentationSetup.exe"
SetCompressor lzma

; Version information for installer
VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey "LegalCopyright" "${IN_COPYRIGHT}"
VIAddVersionKey "FileDescription" "Unity ${IN_VERSION_NICE} Installer"
VIAddVersionKey "FileVersion" "${IN_VERSION_WIN}"
VIAddVersionKey "Unity Version" "${IN_VERSION}"
VIProductVersion "${IN_VERSION_WIN}"

RequestExecutionLevel admin

Name "${PRODUCT_NAME}"

; MUI Settings
!include "MUI2.nsh"

!define MUI_WELCOMEFINISHPAGE_BITMAP "Background.bmp"

!define MUI_ABORTWARNING
!define MUI_ICON "..\..\..\PlatformDependent\Win\res\UnityIcon.ico"
!define MUI_UNICON "..\..\..\PlatformDependent\Win\res\UnityIcon.ico"
!define MUI_COMPONENTSPAGE_NODESC

; Installer pages
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE WelLeaveDownloadEULA
!insertmacro MUI_PAGE_WELCOME
!define MUI_LICENSEPAGE
!include "..\WindowsShared\UnityEula.nsh"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

var installPath
var installPathX64
Var DocumentationInstallDir
var VSSectionIndex ; dummy var for lockedlist

!macro DocumentationSection InstallDir
    SectionIn RO ; section is read-only in UI

    StrCpy $INSTDIR "${InstallDir}"
    SetOutPath "$INSTDIR"
    SetOverwrite try

    DetailPrint "Installing Documentation..."
    SetDetailsPrint listonly

    StrCpy $DocumentationInstallDir "$INSTDIR\Editor\Data\Documentation"

    IfFileExists "$DocumentationInstallDir" 0 no_project_there
        MessageBox MB_YESNO "Folder already exists:$\n$DocumentationInstallDir$\n$\nOverwrite it with Documentation?" /SD IDYES IDYES 0 IDNO skip_install

    no_project_there:
    RMDir /r "$DocumentationInstallDir"
    SetOutPath "$DocumentationInstallDir"

    File /a /r "..\..\..\build\UserDocumentation\Documentation\*"

    DetailPrint "Indexing documentation ..."
    ExecWait "$\"$DocumentationInstallDir\DocCombiner.exe$\" -autopaths $\"$DocumentationInstallDir\..$\"" $0

skip_install:

    SetDetailsPrint lastused
!macroend


Section "Unity Documentation" SEC_UNITY
       !insertmacro DocumentationSection "$INSTDIR"
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
        ${Else}
            MessageBox mb_iconstop "No Unity Editor installations could be found. Looking for registry key '${UNITY_INSTALLER_REGKEY}'.$\n$\nIf you want to install anyway, please specify the install location on the command line, like this: 'UnityDocumentationSetup.exe /D=C:\Program Files\Unity'."
            Abort
        ${EndIf}
    ${EndIf}

    System::Call 'kernel32::LoadLibrary(t "msftedit.dll")i.r0'

    IntOp $0 ${SF_SELECTED} | ${SF_RO}
    SectionSetFlags 0 $0
FunctionEnd
