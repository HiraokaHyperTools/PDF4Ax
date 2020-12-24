; example1.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The installer simply 
; prompts the user asking them where to install, and drops a copy of example1.nsi
; there. 

;--------------------------------

Unicode true

!define APP "PDF4Ax"

!system 'DefineAsmVer.exe "..\Release\${APP}.ocx" "!define VER ""[SVER]"" " > Tmpver.nsh'
!include "Tmpver.nsh"
!searchreplace APV ${VER} "." "_"

!system 'MySign "..\Release\${APP}.ocx"'
!finalize 'MySign "%1"'

!define CLSID "{FE687896-F410-4D10-8740-D584DA23C74D}"

!ifndef FDP
 !define TTL  "PDF4Ax"
 !define EXTN "PDF"
 !define EXT ".pdf"
 !define MIME "application/pdf"
!else
 !define TTL  "FDP4Ax"
 !define EXTN "FDP"
 !define EXT ".fdpe"
 !define MIME "application/x-fdp-e"

 !define EXT2 ".fdp"
 !define MIME2 "application/x-fdp"
!endif

!include "LogicLib.nsh"
!include "FileFunc.nsh"
!include "x64.nsh"

; The name of the installer
Name "${TTL} ${VER}"

; The file to write
OutFile "Setup_${TTL}_${APV}.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\${APP}"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

SetOverwrite ifdiff

XPStyle on

;--------------------------------

; Pages

Page license
Page directory
Page components
Page instfiles

LicenseData GNUGPL2.txt

;--------------------------------

InstType "導入(${EXTN})"
InstType "削除"
InstType "再導入"

Section "ActiveX 関連付け 削除 (${EXTN})"
  SectionIn 2 3

  WriteRegStr    HKCR "${EXT}" "Content Type" "${MIME}"
  DeleteRegValue HKCR "Mime\Database\Content Type\${MIME}" "CLSID"

!ifdef EXT2
  WriteRegStr    HKCR "${EXT2}" "Content Type" "${MIME2}"
  DeleteRegValue HKCR "Mime\Database\Content Type\${MIME2}" "CLSID"
!endif
SectionEnd

Section "本体 削除"
  SectionIn 2 3

  UnRegDLL "$INSTDIR\PDF4Ax.ocx"

  Delete "$INSTDIR\PDF4Ax.ocx"

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
  File "..\Release\PDF4Ax.ocx"

  RegDLL "$INSTDIR\PDF4Ax.ocx"
  
  File "..\GPL\pdftocairo.exe"

  SetOutPath "$INSTDIR\share\poppler"
  File /r /x ".svn" "..\poppler-data\*.*"

SectionEnd ; end the section

Section "ActiveX 関連付け 設定 (${EXTN})"
  SectionIn 1 3

  WriteRegStr HKCR "${EXT}" "Content Type" "${MIME}"

  WriteRegStr HKCR "Mime\Database\Content Type\${MIME}" "CLSID" "${CLSID}"
  WriteRegStr HKCR "Mime\Database\Content Type\${MIME}" "Extension" "${EXT}"

  WriteRegStr HKCR "CLSID\${CLSID}\EnableFullPage\${EXT}" "" ""

!ifdef EXT2
  WriteRegStr HKCR "${EXT2}" "Content Type" "${MIME2}"

  WriteRegStr HKCR "Mime\Database\Content Type\${MIME2}" "CLSID" "${CLSID}"
  WriteRegStr HKCR "Mime\Database\Content Type\${MIME2}" "Extension" "${EXT2}"

  WriteRegStr HKCR "CLSID\${CLSID}\EnableFullPage\${EXT2}" "" ""
!endif

  ${If} 32 < 8086
    WriteRegStr   HKLM "SOFTWARE\Microsoft\Internet Explorer\Low Rights\ElevationPolicy\${CLSID}" "AppName" "pdftocairo.exe"
    WriteRegStr   HKLM "SOFTWARE\Microsoft\Internet Explorer\Low Rights\ElevationPolicy\${CLSID}" "AppPath" "$INSTDIR"
    WriteRegDWord HKLM "SOFTWARE\Microsoft\Internet Explorer\Low Rights\ElevationPolicy\${CLSID}" "Policy" 1
  ${EndIf}
  ${If} ${RunningX64}
    SetRegView 64
    WriteRegStr   HKLM "SOFTWARE\Microsoft\Internet Explorer\Low Rights\ElevationPolicy\${CLSID}" "AppName" "pdftocairo.exe"
    WriteRegStr   HKLM "SOFTWARE\Microsoft\Internet Explorer\Low Rights\ElevationPolicy\${CLSID}" "AppPath" "$INSTDIR"
    WriteRegDWord HKLM "SOFTWARE\Microsoft\Internet Explorer\Low Rights\ElevationPolicy\${CLSID}" "Policy" 1
    SetRegView lastused
  ${EndIf}
SectionEnd

;--------------------------------
