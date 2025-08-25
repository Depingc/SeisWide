@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by SEISWIDE.HPJ. >"hlp\SeisWide.hm"
echo. >>"hlp\SeisWide.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\SeisWide.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\SeisWide.hm"
echo. >>"hlp\SeisWide.hm"
echo // Prompts (IDP_*) >>"hlp\SeisWide.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\SeisWide.hm"
echo. >>"hlp\SeisWide.hm"
echo // Resources (IDR_*) >>"hlp\SeisWide.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\SeisWide.hm"
echo. >>"hlp\SeisWide.hm"
echo // Dialogs (IDD_*) >>"hlp\SeisWide.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\SeisWide.hm"
echo. >>"hlp\SeisWide.hm"
echo // Frame Controls (IDW_*) >>"hlp\SeisWide.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\SeisWide.hm"
REM -- Make help for Project SEISWIDE


echo Building Win32 Help files
start /wait hcrtf -x "hlp\SeisWide.hpj"
echo.
if exist Debug\nul copy "hlp\SeisWide.hlp" Debug
if exist Debug\nul copy "hlp\SeisWide.cnt" Debug
if exist Release\nul copy "hlp\SeisWide.hlp" Release
if exist Release\nul copy "hlp\SeisWide.cnt" Release
echo.


