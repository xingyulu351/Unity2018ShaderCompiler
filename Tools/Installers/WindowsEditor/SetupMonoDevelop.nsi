; Windows Unity Example installer

!include WinVer.nsh
!include x64.nsh
!include "..\WindowsShared\WinDetection.nsh"

!include UnityPaths.nsh
!define PRODUCT_NAME "MonoDevelop for Unity ${IN_VERSION_NICE}"

!define INSTALL_FOLDER "${IN_INSTALL_FOLDER}"
!define UNITY_INSTALLER_REGKEY_ROOT "Software\Unity Technologies\Installer"
!define UNITY_INSTALLER_REGKEY "${UNITY_INSTALLER_REGKEY_ROOT}\${IN_INSTALL_FOLDER}"

Var ResultFile
var VSSectionIndex ; dummy var for lockedlist

!define REBOOT_CODE 3000

OutFile "..\..\..\build\WindowsMonoDevelopInstaller\UnityMonoDevelopSetup.exe"
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

Var ReturnCode

Function .onInit
    ; check windows version
    !insertmacro CheckWindowsVersion

    System::Call 'kernel32::LoadLibrary(t "msftedit.dll")i.r0'

    ; Set install dir (find Unity install location in registry)
    StrCmp $INSTDIR "" 0 InstDirSet ; If specified on command line, $INSTDIR will already be set here

    ReadRegStr $INSTDIR HKCU "${UNITY_INSTALLER_REGKEY}" "Location x64"
    StrCmp $INSTDIR "" 0 InstDirSet
    ReadRegStr $INSTDIR HKCU "${UNITY_INSTALLER_REGKEY}" "Location" ; 32-bit location
    StrCmp $INSTDIR "" 0 InstDirSet

    ; No installation found, suggest default location
    ${If} ${RunningX64}
        StrCpy $INSTDIR "$PROGRAMFILES64\Unity"
    ${Else}
        StrCpy $INSTDIR "$PROGRAMFILES\Unity"
    ${EndIf}

    InstDirSet:

    StrCpy $ResultFile "$PLUGINSDIR\..\UnityInstallerResult.txt"
    Delete $ResultFile
FunctionEnd

Section "MonoDevelop" SEC_MONODEVELOP
    SectionIn RO ; section is read-only in UI
    !ifndef BUILD_TEST_INSTALLER
    DetailPrint "Installing MonoDevelop..."
    SetDetailsPrint listonly
    RMDir /r "$INSTDIR\MonoDevelop"
    SetOutPath "$INSTDIR\MonoDevelop"
    !ifndef DO_UNINSTALLER
    File /a /r "..\..\..\build\WindowsMonoDevelop\MonoDevelop\*.*"
    !endif
    CreateShortCut "$SMPROGRAMS\Unity\MonoDevelop.lnk" "$OUTDIR\bin\MonoDevelop.exe" "" "$OUTDIR\bin\MonoDevelop.exe"
    CreateShortCut "$OUTDIR\MonoDevelop.lnk" "$OUTDIR\bin\MonoDevelop.exe" "" "$OUTDIR\bin\MonoDevelop.exe"

    ClearErrors
    ExecWait 'msiexec /i "$OUTDIR\GTKSharp\gtk-sharp.msi" /passive /norestart' $0

    ${If} $0 = 1641
    ${OrIf} $0 = 3010
        # Success, but restart required
        SetRebootFlag true
        StrCpy $ReturnCode ${REBOOT_CODE}
    ${EndIf}

    !endif

    ClearErrors
    SetDetailsPrint lastused
SectionEnd

Section "-Final" SecFinal
    ${If} $ReturnCode != ""
        FileOpen $4 $ResultFile w
        FileWrite $4 "$ReturnCode"
        FileClose $4
    ${EndIf}
SectionEnd

