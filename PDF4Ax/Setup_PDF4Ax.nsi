; example1.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The installer simply 
; prompts the user asking them where to install, and drops a copy of example1.nsi
; there. 

;--------------------------------

Unicode true

!define APP "PDF4Ax"

!system 'DefineAsmVer.exe "..\Release\${APP}.ocx" "!define VER ""[SVER]"" " > Appver.tmp'
!include "Appver.tmp"
!searchreplace APV ${VER} "." "_"

!system 'MySign "..\Release\${APP}.ocx" "..\x64\Release\${APP}_x64.ocx"'
!finalize 'MySign "%1"'

!define CLSID "{FE687896-F410-4D10-8740-D584DA23C74D}"

!ifdef FDP
 !define NAME "FDP4Ax"
 !define SPECNAME "FDP"

 !define FDPE_INSTALL
 !define FDP_INSTALL
!else
 !define NAME "PDF4Ax"
 !define SPECNAME "PDF"

 !define PDF_INSTALL
!endif

!define PDF_FILEEXT ".pdf"
!define PDF_MEDIATYPE "application/pdf"

!define FDP_FILEEXT ".fdp"
!define FDP_MEDIATYPE "application/x-fdp"

!define FDPE_FILEEXT ".fdpe"
!define FDPE_MEDIATYPE "application/x-fdp-e"

!include "LogicLib.nsh"
!include "FileFunc.nsh"
!include "x64.nsh"

; The name of the installer
Name "${NAME} ${VER}"

; The file to write
OutFile "Setup_${NAME}_${APV}.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\${APP}"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

SetOverwrite ifdiff

XPStyle on

LoadLanguageFile "${NSISDIR}\Contrib\Language files\Japanese.nlf"

;--------------------------------

; Pages

Page license
Page directory
Page components
Page instfiles

LicenseData GNUGPL2.txt

;--------------------------------

InstType "導入(${SPECNAME})"
InstType "削除"
InstType "再導入"

!macro RemoveAssociation FILEEXT MEDIATYPE
  WriteRegStr    HKCR "${FILEEXT}" "Content Type" "${MEDIATYPE}"
  DeleteRegValue HKCR "Mime\Database\Content Type\${MEDIATYPE}" "CLSID"
!macroend

Section "ActiveX 関連付け 削除 (${SPECNAME})"
  SectionIn 2 3

!ifdef PDF_INSTALL
  !insertmacro RemoveAssociation "${PDF_FILEEXT}" "${PDF_MEDIATYPE}"
!endif

!ifdef FDP_INSTALL
  !insertmacro RemoveAssociation "${FDP_FILEEXT}" "${FDP_MEDIATYPE}"
!endif

!ifdef FDPE_INSTALL
  !insertmacro RemoveAssociation "${FDPE_FILEEXT}" "${FDPE_MEDIATYPE}"
!endif
SectionEnd

Section "本体 削除"
  SectionIn 2 3

  ${If} 1 == 1
    ExecWait 'regsvr32.exe /s /u "$INSTDIR\PDF4Ax.ocx"'
    Delete                       "$INSTDIR\PDF4Ax.ocx"
  ${EndIf}
  ${If} ${RunningX64}
    ExecWait 'regsvr32.exe /s /u "$INSTDIR\PDF4Ax_x64.ocx"'
    Delete                       "$INSTDIR\PDF4Ax_x64.ocx"
  ${EndIf}

  RMDir /r "$INSTDIR\share\poppler"
  RMDir    "$INSTDIR\share"
  RMDir    "$INSTDIR"
SectionEnd

; The stuff to install
Section "本体 導入" ;No components page, name is not important
  SectionIn 1 3 4

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  ${If} 1 == 1
    File                    "..\Release\PDF4Ax.ocx"
    ExecWait 'regsvr32.exe /s "$INSTDIR\PDF4Ax.ocx"'
  ${EndIf}
  ${If} ${RunningX64}
    File                "..\x64\Release\PDF4Ax_x64.ocx"
    ExecWait 'regsvr32.exe /s "$INSTDIR\PDF4Ax_x64.ocx"'
  ${EndIf}

  SetOutPath "$INSTDIR\share\poppler"
  File /r /x ".svn" "..\poppler-data\*.*"

SectionEnd ; end the section

Section "ActiveX 関連付け 設定 (${SPECNAME})"
  SectionIn 1 3

!macro AddAssociation FILEEXT MEDIATYPE CLSID
  WriteRegStr HKCR "${FILEEXT}" "Content Type" "${MEDIATYPE}"

  WriteRegStr HKCR "Mime\Database\Content Type\${MEDIATYPE}" "CLSID" "${CLSID}"
  WriteRegStr HKCR "Mime\Database\Content Type\${MEDIATYPE}" "Extension" "${FILEEXT}"

  WriteRegStr HKCR "CLSID\${CLSID}\EnableFullPage\${FILEEXT}" "" ""
!macroend

!ifdef PDF_INSTALL
  !insertmacro AddAssociation  "${PDF_FILEEXT}" "${PDF_MEDIATYPE}" "${CLSID}"
!endif

!ifdef FDP_INSTALL
  !insertmacro AddAssociation  "${FDP_FILEEXT}" "${FDP_MEDIATYPE}" "${CLSID}"
!endif

!ifdef FDPE_INSTALL
  !insertmacro AddAssociation  "${FDPE_FILEEXT}" "${FDPE_MEDIATYPE}" "${CLSID}"
!endif

SectionEnd

;--------------------------------
