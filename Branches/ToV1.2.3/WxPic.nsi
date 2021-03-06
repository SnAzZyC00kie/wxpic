; WxPic Installer Generator Script
;
; BUILD may be defined with /DBUILD=DEBUG to use the Debug executable in the generation
; Else by default the Release executable is used

;--------------------------------

XPStyle on

!define PRODUCT_NAME "WxPic"
!define EXE_NAME "${PRODUCT_NAME}.exe"
!define UNINSTALL "uninstall-${EXE_NAME}"

Name "${PRODUCT_NAME}"

RequestExecutionLevel highest

InstallDir $PROGRAMFILES\${PRODUCT_NAME}

SetCompressor /SOLID lzma

;--------------------------------
; If the build is not defined, then define it to Release
!ifndef ${BUILD}
   !define BUILD Win32Release
!else
	!if ${BUILD} != "Win32Debug"
		!if ${BUILD} != "Win32Release"
			!error "The build '${BUILD}' is not a valid build name"
		!endif
	!endif
!endif
!if ${BUILD} == "Win32Debug"
	!define SHORT_BUILD Dbg
!else
	!define SHORT_BUILD
!endif


; Read the version number in version.autogen file
!searchparse /file src\version.autogen '#define SVN_DATE (_T("' SVN_DATE '"'
!searchparse /file src\version.autogen '#define SVN_REVISION (' SVN_REVISION ")" 
!searchparse /file src\version.autogen '#define SVN_VERSION (_T("' SVN_VERSION '"'
!searchparse /file src\version.autogen '#define FULL_SVNVER (_T("' FULL_VERSION '"'
!searchparse /file src\version.autogen '#define SVN_MANAGED (' SVN_MANAGED ")"

!echo "SVN_DATE = ${SVN_DATE}"
!echo "SVN_REVISION = ${SVN_REVISION}"
!echo "SVN_VERSION = ${SVN_VERSION}"
!echo "FULL_SVNVER = ${FULL_VERSION}"
!echo "SVN_MANAGED = ${SVN_MANAGED}"

!if "${SVN_MANAGED}" == "true"
	OutFile "${PRODUCT_NAME}-${SVN_VERSION}R${SVN_REVISION}${SHORT_BUILD}-installer.exe"
	!define VERSION_TEXT "${SVN_VERSION}R${SVN_REVISION}${SHORT_BUILD}"
	!define VERSION "${FULL_VERSION}"
;	!searchparse /noerrors ${SVN_VERSION} "V" VER1 "." VER2 "." VER3 
;	!ifdef VER2
;		!if "${VER3}" == ""
;			!undef VER3 
;		!endif
;		!ifndef VER3
;			!define VER3 0
;		!endif
;	!else
;		!ifdef VER1
;			!undef VER1
;		!endif
;		!define VER1 0
;		!define VER2 0
;		!define VER3 0
;	!endif
;	!define VERSION "${VER1}.${VER2}.${VER3}.${SVN_REVISION}"
!else
	OutFile "${PRODUCT_NAME}${SHORT_BUILD}-setup.exe"
	!define VERSION_TEXT "Unknown.${SHORT_BUILD}"
	!define VERSION 0.0.0.0
!endif
!echo "   ==> VERSION_TEXT = ${VERSION_TEXT} / ${VERSION}"

VIProductVersion "${VERSION}"

BrandingText "${PRODUCT_NAME} ${VERSION_TEXT}"

;--------------------------------
; Include all the language
!include "Lang\English.nsh"  ; This is the default
!include "Lang\fr\French.nsh"
; Add include for additional language here

LicenseData $(LicenseData)

;--------------------------------

Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------
var CONTEXT

!macro INIT_CONTEXT
	ClearErrors
	UserInfo::GetAccountType
	IfErrors Win9x
	Pop $CONTEXT
	StrCmp $CONTEXT "Admin" Admin
	SetShellVarContext current	
	Goto ContextDone
Admin:
	SetShellVarContext all	
	Goto ContextDone
	
Win9x:
	MessageBox MB_OK $(Win9xError)
	Return
	
ContextDone:
!macroend


Function .onInit
	!insertmacro INIT_CONTEXT
	
	Push ""
	Push ${LANG_ENGLISH}
	Push English
	;------------------
	Push ${LANG_FRENCH}
	Push French
	;-- Add next languages following the same pattern
	Push A ; A means auto count languages
	       ; for the auto count to work the first empty push (Push "") must remain
	LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

	Pop $LANGUAGE
	StrCmp $LANGUAGE "cancel" 0 +2
		Abort
		
FunctionEnd

Function .onGUIEnd
	FileClose $0
FunctionEnd


Function un.onInit
	!insertmacro INIT_CONTEXT
FunctionEnd

;--------------------------------

Section "${PRODUCT_NAME}"

	SectionIn RO
  
	; Set output path to the installation directory.
	SetOutPath $INSTDIR
  
	; Open the lang list file
	FileOpen $0 "$INSTDIR\Lang.lst" w
  
	; Put executable file there
	File "bin\${BUILD}\${EXE_NAME}"
	
	; Put the additinal files (parameters, DLL, ...)
	File "Install\devices.ini"
	File "Install\DisablePolling.reg"
	File "Install\EnablePollingBack.reg"
	File "Install\SampleInterfaceOnSerialPort.xml"
	File "Install\SampleInterfaceOnLptPort.xml"
	File "Install\WinRing0.vxd"
	File "Install\WinRing0.sys"
	File "Install\WinRing0.dll"
	File "Install\WinRing0.COPYRIGHT.txt"

	CreateDirectory "$INSTDIR\Devices"
	
	; Write the uninstall keys for Windows
	WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayName" "${PRODUCT_NAME} ${VERSION_TEXT}"
	WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString" '"$INSTDIR\uninstall-${PRODUCT_NAME}.exe"'
	WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoModify" 1
	WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoRepair" 1
	WriteUninstaller "uninstall-${PRODUCT_NAME}.exe"
  
SectionEnd

Section "$(ShortCutSectionName)"
  
	SetOutPath $INSTDIR  ;-- Define Current directory for Shortcuts
	CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall ${PRODUCT_NAME}.lnk" "$INSTDIR\${UNINSTALL}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${EXE_NAME}" 

SectionEnd

;-- One section per optional language (the default language - English - is managed in the main section)
SectionGroup "$(LangSectionName)"

Section "English"
	;-- English is default Language and can't be deactivated
	SectionIn RO
	; Put The default Help and license files
	SetOutPath "$INSTDIR\Help"
	File "bin\${Build}\Help\*.*"
	SetOutPath "$INSTDIR"
	File "bin\${Build}\License.txt"
	File "bin\${Build}\Author.txt"
	File "bin\${Build}\Translator.txt"
	File "bin\${Build}\DocWriter.txt"
SectionEnd

!macro LANGUAGE_SECTION LANG_NAME LANG_TAG
Section "${LANG_NAME}"
	; Copy the localized files
	SetOutPath "$INSTDIR\${LANG_TAG}"
	File "bin\${Build}\${LANG_TAG}\WxPic.mo"
	SetOutPath "$INSTDIR\${LANG_TAG}"
	File /nonfatal "bin\${Build}\${LANG_TAG}\*.txt"
	
	; Copy the Help files if any
	SetOutPath "$INSTDIR\Help\${LANG_TAG}"
	File /nonfatal "bin\${Build}\Help\${LANG_TAG}\*.*"
	
	; Memorize the language has been installed
	FileWrite $0 "${LANG_TAG}$\r$\n"
SectionEnd
!macroend

!insertmacro LANGUAGE_SECTION "French" "fr"

;-- Add more LANGUAGE_SECTION here to add new Languages
;-- The only data to adapt from one optional language to another
;-- are the values assigned to $LANGUAGE_SECTION parameters

SectionGroupEnd


;--------------------------------

; Uninstaller

UninstallText $(UninstallWarning)
UninstallIcon "${NSISDIR}\Contrib\Graphics\Icons\nsis1-uninstall.ico"

Section "Uninstall"

	;ReadRegStr "$INSTDIR SHCTX SOFTWARE\${PRODUCT_NAME}" "Install_Dir"
	
RetryDeleteExe:
	ClearErrors
	Delete "$INSTDIR\${EXE_NAME}"
	IfErrors 0 ExeDeleted
	MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION $(CantDeleteExe) /SD IDCANCEL IDRETRY RetryDeleteExe
	Abort
ExeDeleted:	

	; Uninstall the additinal files
	Delete "$INSTDIR\devices.ini"
	Delete /REBOOTOK "$INSTDIR\WinRing0.sys"
	Delete /REBOOTOK "$INSTDIR\WinRing0.vxd"
	Delete /REBOOTOK "$INSTDIR\WinRing0.dll"
	Delete "$INSTDIR\SampleInterfaceOnSerialPort.xml"
	Delete "$INSTDIR\SampleInterfaceOnLptPort.xml"
	Delete "$INSTDIR\WinRing0.COPYRIGHT.txt"
	Delete "$INSTDIR\DisablePolling.reg"
	Delete "$INSTDIR\EnablePollingBack.reg"
   
	RMDir "$INSTDIR\Devices"
	IfFileExists "$INSTDIR\Devices" +1 NoDeviceFile
		MessageBox MB_YESNO|MB_ICONQUESTION $(DeviceFilesExist) /SD IDYES IDYES KeepDeviceFiles 
		RMDir /r "$INSTDIR\Devices"
NoDeviceFile:
KeepDeviceFiles:
   
	RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"
	
	RMDir /r "$INSTDIR\Help"
	Delete "$INSTDIR\License.txt"
	Delete "$INSTDIR\Author.txt"
	Delete "$INSTDIR\Translator.txt"
	Delete "$INSTDIR\DocWriter.txt"
	
	ClearErrors
	FileOpen $0 "$INSTDIR\Lang.lst" r
	IfErrors NoFileOpen
DeleteNextLang:
	FileRead $0 $1
	IfErrors AllLangCompleted
	RMDir /r "$INSTDIR\$1"
	Goto DeleteNextLang
AllLangCompleted:
	FileClose $0
NoFileOpen:
	Delete "$INSTDIR\Lang.lst"
	
	DeleteRegKey SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
	;DeleteRegKey SHCTX "SOFTWARE\${PRODUCT_NAME}"
	
	Delete "$INSTDIR\${UNINSTALL}"
	
	RMDir "$INSTDIR"
  
SectionEnd
