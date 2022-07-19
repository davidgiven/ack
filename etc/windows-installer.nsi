!include MUI2.nsh

Name "The Amsterdam Compiler Kit"
OutFile "${OUTFILE}"
Unicode True

InstallDir "$PROGRAMFILES\Amsterdam Compiler Kit"

RequestExecutionLevel admin
SetCompressor /solid lzma

;--------------------------------

!define MUI_WELCOMEPAGE_TITLE "The Amsterdam Compiler Kit"
!define MUI_WELCOMEPAGE_TEXT "The ACK is a compiler toolchain supporting a \
	variety of frontend and backends. It's not easy to use, so if you don't \
	know what you're doing you shouldn't install this.$\r$\n\
	$\r$\n\
	This wizard will install the ACK on your computer.$\r$\n\
	$\r$\n\
	$_CLICK"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\nsis.bmp"
!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_WELCOME

!define MUI_COMPONENTSPAGE_NODESC
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_TITLE "Installation complete"
!define MUI_FINISHPAGE_TEXT_LARGE
!define MUI_FINISHPAGE_TEXT "The ACK is now ready to use, but \
	hasn't been added to your path.$\r$\n\
	$\r$\n\
	Have fun!"

!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

; The stuff to install
Section "The ACK (required)"
	SectionIn RO
	SetOutPath $INSTDIR
	File /r "${BUILDDIR}\*.*"
	;File /oname=wordgrinder.exe "bin\wordgrinder-builtin-sdl-release.exe"
	;File /oname=cwordgrinder.exe "bin\wordgrinder-builtin-wincon-release.exe"
	;File "README.wg"
	;File "licenses\COPYING.*"

	;CreateDirectory $INSTDIR\Dictionaries
	;File /oname=Dictionaries\British.dictionary "extras\british.dictionary"
	;File /oname=Dictionaries\American-Canadian.dictionary "extras\american-canadian.dictionary"

	; Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WordGrinder" "DisplayName" "WordGrinder for Windows"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WordGrinder" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WordGrinder" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WordGrinder" "NoRepair" 1
	WriteUninstaller "uninstall.exe"
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
	; Remove registry keys
	; Remove files and uninstaller
	RMDir /r $INSTDIR
SectionEnd
