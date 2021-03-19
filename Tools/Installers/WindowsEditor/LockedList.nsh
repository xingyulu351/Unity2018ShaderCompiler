Function LockedListShow
    !insertmacro MUI_HEADER_TEXT `Applications are blocking the installer` `Some applications are preventing Unity from being installed, and must be closed.`
    LockedList::AddModule \Unity.exe

    ; Check for running Visual Studio Instances (devenv.exe) if Visual Studio was selected for install.
    ${If} $VSSectionIndex != ''
    ${AndIf} ${SectionIsSelected} $VSSectionIndex
    	LockedList::AddModule \devenv.exe
    ${EndIf}

    LockedList::Dialog /autonext /ignore Ignore /heading `Please close the following applications in order to continue with setup:`
    Pop $R0
FunctionEnd

