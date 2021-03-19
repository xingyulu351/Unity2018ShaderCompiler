Var EULABanner
Var RichEditLicense
Var CheckBoxAgree
Var HaveDownloadedEULA
Var DownloadedEULAPath
Var CmdLineEULAFile

Page Custom UnityEULA_Show UnityEULA_Leave

Function DownloadUnityEULAIfPresent
	${If} $HaveDownloadedEULA == "true"
		Return
	${EndIf}

	Var /Global EULA
!ifdef CUSTOM_EULA_URL
	StrCpy $EULA ${CUSTOM_EULA_URL}
!else
	StrCpy $EULA "http://unity3d.com/files/eula/License.rtf"
!endif

	${If} $CmdLineEulaFile != ""
		Return
	${EndIf}

	inetc::get $EULA "$PLUGINSDIR\unityeula.rtf" /End
	Pop $0
	${If} $0 == "OK"
		StrCpy $DownloadedEULAPath "$PLUGINSDIR\unityeula.rtf"
	${ElseIf} $0 == "Cancelled"
		StrCpy $DownloadedEULAPath ""
	${Else}
		StrCpy $DownloadedEULAPath ""
	${EndIf}

	StrCpy $HaveDownloadedEULA "true"
FunctionEnd

Function WelLeaveDownloadEULA
	# Disable Cancel/Next buttons while Eula is downloaded
	GetDlgItem $0 $HWNDPARENT 1
	EnableWindow $0 0
	GetDlgItem $0 $HWNDPARENT 2
	EnableWindow $0 0

	call DownloadUnityEULAIfPresent

    # Re-enable the cancel button, in case of error
    GetDlgItem $0 $HWNDPARENT 2
    EnableWindow $0 1
FunctionEnd

Function UnityEULA_Show
	!ifdef MUI_PAGE_CUSTOMFUNCTION_PRE
		Call ${MUI_PAGE_CUSTOMFUNCTION_PRE}
	!endif

!ifdef CUSTOM_EULA_FILE
	File /oname=$PLUGINSDIR\License.rtf ${CUSTOM_EULA_FILE}
!else
	File /oname=$PLUGINSDIR\License.rtf ..\..\Licenses\License.rtf
!endif

	StrCpy $EULABanner "$(MUI_TEXT_LICENSE_SUBTITLE)"

	!insertmacro MUI_HEADER_TEXT `$(MUI_TEXT_LICENSE_TITLE)` $EULABanner

	GetDlgItem $R0 $HWNDPARENT 1
	EnableWindow $R0 0

	nsDialogs::Create 1018
	Pop $R0

	call DownloadUnityEULAIfPresent

	${If} $CmdLineEulaFile != ""
		IfFileExists $CmdLineEulaFile +3 0
		MessageBox MB_OK|MB_ICONSTOP "The specified EULA file $CmdLineEulaFile could not be found"
		Quit
		StrCpy $DownloadedEULAPath $CmdLineEulaFile
	${ElseIf} $DownloadedEULAPath == ""
		StrCpy $DownloadedEULAPath "$PLUGINSDIR\License.rtf"
	${EndIf}

	!define /math EM_AUTOURLDETECT ${WM_USER} + 91
	!define /math ENM_LINK ${WM_USER} + 0x4000000
	!define EM_SETEVENTMASK 0x0445
	!define ES_NOOLEDRAGDROP 8

	nsDialogs::CreateControl RichEdit50W ${WS_VISIBLE}|${WS_CHILD}|${WS_TABSTOP}|${WS_VSCROLL}|${ES_MULTILINE}|${ES_WANTRETURN}|${ES_READONLY}|${ES_NOOLEDRAGDROP} ${WS_EX_NOPARENTNOTIFY}|${__NSD_Text_EXSTYLE} 0 0 100% -24u ``
	Pop $RichEditLicense

	GetFunctionAddress $0 UnityEULA_Notify
	nsDialogs::OnNotify $RichEditLicense $0

	SendMessage $RichEditLicense ${EM_AUTOURLDETECT} 1 0
	SendMessage $RichEditLicense ${EM_SETEVENTMASK} 0 ${ENM_LINK}

	nsRichEdit::Load $RichEditLicense $DownloadedEULAPath


	${NSD_CreateCheckBox} 0 -20u 160u 12u `$(^AcceptBtn)`
	Pop $CheckBoxAgree
	${NSD_OnClick} $CheckBoxAgree UnityEULA_CheckBoxAgree_Click

	${NSD_CreateButton} -51u -20u 50u 14u `&Print`
	Pop $R0
	${NSD_OnClick} $R0 UnityEULA_ButtonPrint_Click

	nsDialogs::Show
FunctionEnd

Function UnityEULA_Leave
FunctionEnd

Function UnityEULA_Notify
	Pop $0
	Pop $1
	Pop $2

	!define EN_LINK 0x70B
	!define /math EM_GETTEXTRANGE ${WM_USER} + 75
	${If} $1 = ${EN_LINK}
		System::Call "*$2(i,i,i,i.r1,i,i,i.r2,i.r3)"
		${If} $3 >= 0 ; Is it a CHARRANGE we can work with?
			${If} $1 = ${WM_LBUTTONDOWN}
				IntOp $1 $3 - $2 ; length
				IntOp $1 $1 + 1 ; \0
				System::Call '*(ir2,ir3,i,i,&t$1)i.r1' ; TEXTRANGE + align + string
				IntOp $2 $1 + 16
				System::Call '*$1(i,i,ir2)'
				SendMessage $0 ${EM_GETTEXTRANGE} 0 $1
				System::Call "*$2(&t999.r2)"
				ExecShell open $2
				System::Free $1
			${EndIf}
		${EndIf}
	${EndIf}
FunctionEnd

Function UnityEULA_ButtonPrint_Click
    nsRichEdit::Print $RichEditLicense `$(^Name) License Agreement`
    Pop $R0
FunctionEnd

Function UnityEULA_CheckBoxAgree_Click
    ${NSD_GetState} $CheckBoxAgree $R0
    ${If} $R0 == ${BST_CHECKED}
        GetDlgItem $R0 $HWNDPARENT 1
        EnableWindow $R0 1
    ${Else}
        GetDlgItem $R0 $HWNDPARENT 1
        EnableWindow $R0 0
    ${EndIf}
FunctionEnd

!ifndef MUI_PAGE_CUSTOMFUNCTION_PRE
	!define MUI_PAGE_CUSTOMFUNCTION_PRE ""
!endif

!undef MUI_PAGE_CUSTOMFUNCTION_PRE
