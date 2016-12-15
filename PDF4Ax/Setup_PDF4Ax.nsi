; example1.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The installer simply 
; prompts the user asking them where to install, and drops a copy of example1.nsi
; there. 

;--------------------------------

!define APP "PDF4Ax"

!define VER "0.0.12"
!define APV "0_0_12"

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

; The name of the installer
Name "${TTL} ${VER}"

; The file to write
OutFile "Setup_${TTL}_${APV}.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\${APP}"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

SetOverwrite ifdiff

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

  Delete "$INSTDIR\mfc80.dll"
  Delete "$INSTDIR\mfc80CHS.dll"
  Delete "$INSTDIR\mfc80CHT.dll"
  Delete "$INSTDIR\mfc80DEU.dll"
  Delete "$INSTDIR\mfc80ENU.dll"
  Delete "$INSTDIR\mfc80ESP.dll"
  Delete "$INSTDIR\mfc80FRA.dll"
  Delete "$INSTDIR\mfc80ITA.dll"
  Delete "$INSTDIR\mfc80JPN.dll"
  Delete "$INSTDIR\mfc80KOR.dll"
  Delete "$INSTDIR\mfc80u.dll"
  Delete "$INSTDIR\mfcm80.dll"
  Delete "$INSTDIR\mfcm80u.dll"
  Delete "$INSTDIR\Microsoft.VC80.CRT.manifest"
  Delete "$INSTDIR\Microsoft.VC80.MFC.manifest"
  Delete "$INSTDIR\Microsoft.VC80.MFCLOC.manifest"
  Delete "$INSTDIR\msvcm80.dll"
  Delete "$INSTDIR\msvcp80.dll"
  Delete "$INSTDIR\msvcr80.dll"

  Delete "$INSTDIR\PDF4Ax.ocx"
  Delete "$INSTDIR\PDF4Ax.pdb"

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
  File "..\Release\PDF4Ax.pdb"

  File "8.0.50727.6195\mfc80.dll"
  File "8.0.50727.6195\mfc80CHS.dll"
  File "8.0.50727.6195\mfc80CHT.dll"
  File "8.0.50727.6195\mfc80DEU.dll"
  File "8.0.50727.6195\mfc80ENU.dll"
  File "8.0.50727.6195\mfc80ESP.dll"
  File "8.0.50727.6195\mfc80FRA.dll"
  File "8.0.50727.6195\mfc80ITA.dll"
  File "8.0.50727.6195\mfc80JPN.dll"
  File "8.0.50727.6195\mfc80KOR.dll"
  File "8.0.50727.6195\mfc80u.dll"
  File "8.0.50727.6195\mfcm80.dll"
  File "8.0.50727.6195\mfcm80u.dll"
  File "8.0.50727.6195\Microsoft.VC80.CRT.manifest"
  File "8.0.50727.6195\Microsoft.VC80.MFC.manifest"
  File "8.0.50727.6195\Microsoft.VC80.MFCLOC.manifest"
  File "8.0.50727.6195\msvcm80.dll"
  File "8.0.50727.6195\msvcp80.dll"
  File "8.0.50727.6195\msvcr80.dll"

  RegDLL "$INSTDIR\PDF4Ax.ocx"

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
SectionEnd

;--------------------------------
