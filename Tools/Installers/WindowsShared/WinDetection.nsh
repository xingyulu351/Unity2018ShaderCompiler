!include ..\WindowsShared\CompareVersion.nsh

!macro CheckWindowsVersion
    ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
    IfErrors 0 lbl_winnt

    MessageBox MB_OK|MB_ICONSTOP "Unity requires Windows 7 or later!"
    Abort

lbl_winnt:

    ${VersionCheckNew} $R0 "6.0" "$R1"
    ${Unless} $R1 == "1"
        MessageBox MB_OK|MB_ICONSTOP "Unity requires Windows 7 or later!"
        Abort
    ${EndUnless}
!macroend
