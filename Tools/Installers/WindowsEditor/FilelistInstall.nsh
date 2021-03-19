DetailPrint "Removing old Unity installation..."
SetDetailsPrint listonly
removefiles:
ClearErrors
RMDir /r "$INSTDIR\Editor\Data"
RMDir /r "$INSTDIR\Editor\Standard Packages"
RMDir /r "$INSTDIR\Editor\BugReporter"
RMDir /r "$INSTDIR\Editor\${IN_EXAMPLE_NAME}"
Delete "$INSTDIR\Editor\*.dll"
Delete "$INSTDIR\Editor\*.exe"
Delete "$INSTDIR\Editor\*.manifest"
IfErrors 0 doneremove
	MessageBox MB_OK|MB_ABORTRETRYIGNORE "Failed to delete old Unity installation files. Maybe Unity or some of its tools are still running?" IDRETRY removefiles IDIGNORE doneremove
	Quit
doneremove:
SetDetailsPrint lastused

!ifndef BUILD_TEST_INSTALLER

DetailPrint "Installing Unity executables..."
SetDetailsPrint listonly
SetOutPath "$INSTDIR\Editor"

File /x *.pdb /x *.ilk /x *.testLog "..\..\..\build\WindowsEditor\*.*"

; To get nice stacktraces, just include the Unity pdb file
!if ${ARCHITECTURE} = "x64"
	File /oname=unity_x64_mono.pdb "..\..\..\build\WindowsEditor\unity_x64_mono_s.pdb"
!else
	File /oname=unity_x86_mono.pdb "..\..\..\build\WindowsEditor\unity_x86_mono_s.pdb"
!endif
File "..\..\..\build\WindowsEditor\Data\Mono\EmbedRuntime\mono.pdb"
File "..\..\..\build\WindowsEditor\Data\MonoBleedingEdge\EmbedRuntime\mono-2.0-bdwgc.pdb"

SetOutPath "$INSTDIR\Editor\Data"
!cd ..\..\..
File /r /x *.pdb /x *.ilk /x PlaybackEngines "build\WindowsEditor\Data\*.*"
!cd "Tools\Installers\WindowsEditor"
; if we ever update nsis to after the july 2010 version, we can use {__FILEDIR__}
; !cd "${__FILEDIR__}"

SetOutPath "$INSTDIR\Editor\BugReporter"
File /r "..\..\..\build\WindowsEditor\BugReporter\*.*"

SetOutPath "$INSTDIR\Editor\locales"
File /r "..\..\..\build\WindowsEditor\locales\*.*"

SetDetailsPrint lastused

DetailPrint "Installing playback engines..."
SetDetailsPrint listonly
SetOutPath "$INSTDIR\Editor\Data\PlaybackEngines"
; Playback engines are optional since WebPlayer has been removed.
File /nonfatal /r /x *.ilk /x *_s.pdb "..\..\..\build\temp\PlaybackEngines\*.*"

; include both x86 and x64 pdb files (e.g. could be including 32-bit platforms with 32-bit symbols)
!include "FilelistInstall_pdb.nsh"
!endif ; BUILD_TEST_INSTALLER

!ifndef DO_UNINSTALLER
SetOutPath "$INSTDIR\Editor"
File "..\..\..\build\temp\Uninstall.exe"
!endif
