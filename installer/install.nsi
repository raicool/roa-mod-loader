!include MUI2.nsh

Name "Rivals Mod Loader"
OutFile "loader-install.exe"
InstallDir "$PROGRAMFILES32\Steam\steamapps\common\Rivals of Aether\" ; set default path to the most common one
Unicode true
SetCompressor /FINAL /SOLID lzma

!define VERSION "1.0.4"
!define BIN_DIR "..\bin\release"
!define EXT_DIR "..\ext"
!define MUI_ICON res\small_logo_64x.ico

# REGULAR STRINGS
LangString MUI_TEXT_WELCOME_INFO_TEXT ${LANG_ENGLISH} "Welcome to the Rivals Mod Loader installer"
LangString TEXT_ROA_IS_RUNNING ${LANG_ENGLISH} "Rivals is currently running! Please close Rivals before continuing with this setup."
LangString TEXT_VC_REDIST_FOUND ${LANG_ENGLISH} "Visual C++ Redistributables 2015-2022 x86 14.44.35211 Found!"
# SECTION DESCRIPTORS
LangString TEXT_VC_REDIST_DESCR ${LANG_ENGLISH} "Download and install Visual C++ Redistributables 2015-2022 x86 14.44.35211 (Required for the mod loader to run, uncheck if you already have this)"
LangString TEXT_REQUIRED_FILES_SECTION_DESCR ${LANG_ENGLISH} "Main mod loader files (Required)"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "../LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_LANGUAGE "English"

Function inst_vcredist
	SetOutPath "$INSTDIR"
	inetc::get "https://aka.ms/vc14/vc_redist.x86.exe" $INSTDIR/vcredist_x86.exe
	ExecWait "$INSTDIR/vcredist_x86.exe /norestart"
	
FunctionEnd

SectionGroup "roa-mod-loader"
	Section "Required Files" REQUIRED_FILES_SECTION
		SectionIn RO
		roa_check_open:
			nsProcess::_FindProcess "RivalsofAether.exe"
			Pop $R0
			${If} $R0 = 0
				MessageBox MB_ABORTRETRYIGNORE|MB_ICONEXCLAMATION $(TEXT_ROA_IS_RUNNING) IDRETRY roa_check_open IDIGNORE resume
				Quit
			${EndIf}
		resume:
			SetOutPath "$INSTDIR"
			File "${BIN_DIR}\loader.dll"
			File "${EXT_DIR}\catch_error_mini.dll"
	SectionEnd
SectionGroupEnd

Section "Visual C++ Redistributable x86 14.44.35211" VC_REDIST_SECTION
	ClearErrors
	ReadRegStr $0 HKLM "Computer/HKEY_LOCAL_MACHINE/SOFTWARE/WOW6432Node/Microsoft/VisualStudio/14.0/VC/Runtimes/X86" ""
	${If} ${Errors}
		Call inst_vcredist
	${Else}
		DetailPrint $(TEXT_VC_REDIST_FOUND)
	${EndIf}
SectionEnd

Section /o "roa-texture-loader" ROA_TEXTURE_LOADER_SECTION
	CreateDirectory $INSTDIR\mods
	inetc::get "https://github.com/raicool/roa-texture-loader/releases/download/1.0.4/roa-texture-loader-v1.0.4-release-msvc.zip" "$INSTDIR\roa-texture-loader.zip"
	nsisunz::UnzipToLog "$INSTDIR\roa-texture-loader.zip" "$INSTDIR"
	Delete "$INSTDIR\roa-texture-loader.zip"
SectionEnd

Function .onVerifyInstDir
    IfFileExists "$INSTDIR\RivalsofAether.exe" FileIsThere
    Abort
    FileIsThere:
FunctionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${VC_REDIST_SECTION} $(TEXT_VC_REDIST_DESCR)
!insertmacro MUI_DESCRIPTION_TEXT ${REQUIRED_FILES_SECTION} $(TEXT_REQUIRED_FILES_SECTION_DESCR)
!insertmacro MUI_FUNCTION_DESCRIPTION_END