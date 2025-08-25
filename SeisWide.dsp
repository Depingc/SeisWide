# Microsoft Developer Studio Project File - Name="SeisWide" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SeisWide - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SeisWide.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SeisWide.mak" CFG="SeisWide - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SeisWide - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SeisWide - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SeisWide - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE F90 /include:".\Release/" /compile_only /nologo /I "Release/"
# ADD F90 /include:".\Release/" /compile_only /nologo /debug:full /I "Release/"
# SUBTRACT F90 /libdir:noauto
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /profile /debug

!ELSEIF  "$(CFG)" == "SeisWide - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE F90 /include:".\Debug/" /compile_only /nologo /debug:full /I "Debug/"
# ADD F90 /include:".\Debug/" /compile_only /nologo /debug:full /I "Debug/"
# SUBTRACT F90 /libdir:noauto
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /profile /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "SeisWide - Win32 Release"
# Name "SeisWide - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\bmp24.cpp
# End Source File
# Begin Source File

SOURCE=.\BoundNodes.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\CntrItem.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorButton.cpp
# End Source File
# Begin Source File

SOURCE=.\DIB.cpp
# End Source File
# Begin Source File

SOURCE=.\Digitize.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgAddRange.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgChangeRanges.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgChangeReducingBMP.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgCohMix.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgConvertRawSgy.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgDEPTH1.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgDigitizeShift.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgDigLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgDigResize.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgDigResolution.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgDigSelectCurrent.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgDispBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgExportBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgFillArea.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgFilter1.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgHydrateDisp.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgImportbitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgInitAmp.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgInOut.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgInputValie.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgKdmig.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgKDMigPost.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgLabelSizes.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgLatLong.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgLatShift.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgLayerNodes.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgLineDrawDep.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgLineDrawTim.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgLoadReid.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgLoadZelt.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgMergeModels.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgMigLineDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgMigPS.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgModelResize.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgModelShiftBoundary.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgNewLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgNewModel.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgNewVelFile.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgPrint.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgRayAuto.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgRaytrace1.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgRayTracing.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgReorganize.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgResample.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgSegyAppend.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgSegyColor.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgSegyInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgSegyPreview.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgSonobouy.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgSourcePos.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgTablizeLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgTim2Dpth.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgTimDepthBMP.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgTimeByTrc.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgTIMESEC1.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgTTCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgUnix2PC.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgVel2Den.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgVel2Den1.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgVELOCITY1.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgVred.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgVZCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgWienerFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\IniReader.cpp
# End Source File
# Begin Source File

SOURCE=.\IniWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\IpFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\SeisWide.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\SeisWide.hpj

!IF  "$(CFG)" == "SeisWide - Win32 Release"

# Begin Custom Build - Making help file...
OutDir=.\Release
ProjDir=.
TargetName=SeisWide
InputPath=.\hlp\SeisWide.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "SeisWide - Win32 Debug"

# Begin Custom Build - Making help file...
OutDir=.\Debug
ProjDir=.
TargetName=SeisWide
InputPath=.\hlp\SeisWide.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(ProjDir)\makehelp.bat"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SeisWide.rc
# End Source File
# Begin Source File

SOURCE=.\SeisWideDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SeisWideView.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvrItem.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TipDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\bmp24.h
# End Source File
# Begin Source File

SOURCE=.\BoundNodes.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\CIMAGE\CIMAGE.H
# End Source File
# Begin Source File

SOURCE=.\CIMAGE\CIMAGEB.H
# End Source File
# Begin Source File

SOURCE=.\CIMAGE\CMAP.H
# End Source File
# Begin Source File

SOURCE=.\CntrItem.h
# End Source File
# Begin Source File

SOURCE=.\ColorButton.h
# End Source File
# Begin Source File

SOURCE=.\DIB.h
# End Source File
# Begin Source File

SOURCE=.\CIMAGE\DIBUTILS.H
# End Source File
# Begin Source File

SOURCE=.\Digitize.h
# End Source File
# Begin Source File

SOURCE=.\dlgAddRange.h
# End Source File
# Begin Source File

SOURCE=.\dlgChangeRanges.h
# End Source File
# Begin Source File

SOURCE=.\dlgChangeReducingBMP.h
# End Source File
# Begin Source File

SOURCE=.\dlgCohMix.h
# End Source File
# Begin Source File

SOURCE=.\dlgConvertRawSgy.h
# End Source File
# Begin Source File

SOURCE=.\dlgDEPTH1.h
# End Source File
# Begin Source File

SOURCE=.\dlgDigitizeShift.h
# End Source File
# Begin Source File

SOURCE=.\dlgDigLabel.h
# End Source File
# Begin Source File

SOURCE=.\dlgDigResize.h
# End Source File
# Begin Source File

SOURCE=.\dlgDigResolution.h
# End Source File
# Begin Source File

SOURCE=.\dlgDigSelectCurrent.h
# End Source File
# Begin Source File

SOURCE=.\dlgDispBitmap.h
# End Source File
# Begin Source File

SOURCE=.\dlgExportBitmap.h
# End Source File
# Begin Source File

SOURCE=.\dlgFillArea.h
# End Source File
# Begin Source File

SOURCE=.\dlgFilter1.h
# End Source File
# Begin Source File

SOURCE=.\dlgHydrateDisp.h
# End Source File
# Begin Source File

SOURCE=.\dlgImportbitmap.h
# End Source File
# Begin Source File

SOURCE=.\dlgInitAmp.h
# End Source File
# Begin Source File

SOURCE=.\dlgInOut.h
# End Source File
# Begin Source File

SOURCE=.\dlgInputValie.h
# End Source File
# Begin Source File

SOURCE=.\dlgKdmig.h
# End Source File
# Begin Source File

SOURCE=.\dlgKDMigPost.h
# End Source File
# Begin Source File

SOURCE=.\dlgLabelSizes.h
# End Source File
# Begin Source File

SOURCE=.\dlgLatLong.h
# End Source File
# Begin Source File

SOURCE=.\dlgLatShift.h
# End Source File
# Begin Source File

SOURCE=.\dlgLayerNodes.h
# End Source File
# Begin Source File

SOURCE=.\dlgLineDrawDep.h
# End Source File
# Begin Source File

SOURCE=.\dlgLineDrawTim.h
# End Source File
# Begin Source File

SOURCE=.\dlgLoadReid.h
# End Source File
# Begin Source File

SOURCE=.\dlgLoadZelt.h
# End Source File
# Begin Source File

SOURCE=.\dlgMergeModels.h
# End Source File
# Begin Source File

SOURCE=.\dlgMigLineDraw.h
# End Source File
# Begin Source File

SOURCE=.\dlgMigPS.h
# End Source File
# Begin Source File

SOURCE=.\dlgModelResize.h
# End Source File
# Begin Source File

SOURCE=.\dlgModelShiftBoundary.h
# End Source File
# Begin Source File

SOURCE=.\dlgNewLayer.h
# End Source File
# Begin Source File

SOURCE=.\DlgNewModel.h
# End Source File
# Begin Source File

SOURCE=.\dlgNewVelFile.h
# End Source File
# Begin Source File

SOURCE=.\dlgPrint.h
# End Source File
# Begin Source File

SOURCE=.\dlgRayAuto.h
# End Source File
# Begin Source File

SOURCE=.\dlgRaytrace1.h
# End Source File
# Begin Source File

SOURCE=.\dlgRayTracing.h
# End Source File
# Begin Source File

SOURCE=.\dlgReorganize.h
# End Source File
# Begin Source File

SOURCE=.\dlgResample.h
# End Source File
# Begin Source File

SOURCE=.\dlgSegyAppend.h
# End Source File
# Begin Source File

SOURCE=.\dlgSegyColor.h
# End Source File
# Begin Source File

SOURCE=.\dlgSegyInfo.h
# End Source File
# Begin Source File

SOURCE=.\dlgSegyPreview.h
# End Source File
# Begin Source File

SOURCE=.\dlgSonobouy.h
# End Source File
# Begin Source File

SOURCE=.\dlgSourcePos.h
# End Source File
# Begin Source File

SOURCE=.\dlgTablizeLayer.h
# End Source File
# Begin Source File

SOURCE=.\dlgTim2Dpth.h
# End Source File
# Begin Source File

SOURCE=.\dlgTimDepthBMP.h
# End Source File
# Begin Source File

SOURCE=.\dlgTimeByTrc.h
# End Source File
# Begin Source File

SOURCE=.\dlgTIMESEC1.h
# End Source File
# Begin Source File

SOURCE=.\dlgTTCurve.h
# End Source File
# Begin Source File

SOURCE=.\dlgUnix2PC.h
# End Source File
# Begin Source File

SOURCE=.\dlgVel2Den.h
# End Source File
# Begin Source File

SOURCE=.\dlgVel2Den1.h
# End Source File
# Begin Source File

SOURCE=.\dlgVELOCITY1.h
# End Source File
# Begin Source File

SOURCE=.\dlgVred.h
# End Source File
# Begin Source File

SOURCE=.\dlgWienerFilter.h
# End Source File
# Begin Source File

SOURCE=.\IniReader.h
# End Source File
# Begin Source File

SOURCE=.\IniWriter.h
# End Source File
# Begin Source File

SOURCE=.\IpFrame.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\SeisWide.h
# End Source File
# Begin Source File

SOURCE=.\SeisWideDoc.h
# End Source File
# Begin Source File

SOURCE=.\SeisWideView.h
# End Source File
# Begin Source File

SOURCE=.\SrvrItem.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TipDlg.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Deping3.BMP
# End Source File
# Begin Source File

SOURCE=.\filter.rct
# End Source File
# Begin Source File

SOURCE=.\RES\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\IToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\litebulb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SeisWide.ico
# End Source File
# Begin Source File

SOURCE=.\res\SeisWide.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SeisWideDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\SeisWide.idl
# ADD MTL /h "SeisWide_i.h" /iid "SeisWide_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\SeisWide.rgs
# End Source File
# End Target
# End Project
# Section SeisWide : {0058CB54-0000-0000-60CD-580078CB5800}
# 	1:17:CG_IDS_DIDYOUKNOW:108
# 	1:22:CG_IDS_TIPOFTHEDAYMENU:107
# 	1:18:CG_IDS_TIPOFTHEDAY:106
# 	1:22:CG_IDS_TIPOFTHEDAYHELP:111
# 	1:19:CG_IDP_FILE_CORRUPT:110
# 	1:7:IDD_TIP:105
# 	1:13:IDB_LIGHTBULB:104
# 	1:18:CG_IDS_FILE_ABSENT:109
# 	2:17:CG_IDS_DIDYOUKNOW:CG_IDS_DIDYOUKNOW
# 	2:7:CTipDlg:CTipDlg
# 	2:22:CG_IDS_TIPOFTHEDAYMENU:CG_IDS_TIPOFTHEDAYMENU
# 	2:18:CG_IDS_TIPOFTHEDAY:CG_IDS_TIPOFTHEDAY
# 	2:12:CTIP_Written:OK
# 	2:22:CG_IDS_TIPOFTHEDAYHELP:CG_IDS_TIPOFTHEDAYHELP
# 	2:2:BH:
# 	2:19:CG_IDP_FILE_CORRUPT:CG_IDP_FILE_CORRUPT
# 	2:7:IDD_TIP:IDD_TIP
# 	2:8:TipDlg.h:TipDlg.h
# 	2:13:IDB_LIGHTBULB:IDB_LIGHTBULB
# 	2:18:CG_IDS_FILE_ABSENT:CG_IDS_FILE_ABSENT
# 	2:10:TipDlg.cpp:TipDlg.cpp
# End Section
# Section SeisWide : {00000064-0000-0000-0000-000000000000}
# 	1:10:IDB_SPLASH:103
# 	2:21:SplashScreenInsertKey:4.0
# End Section
# Section SeisWide : {00A40002-0061-0000-0001-000024B5EF00}
# 	1:12:IDR_SEISWIDE:112
# End Section
# Section SeisWide : {D30C1661-CDAF-11D0-8A3E-00C04FC9E26E}
# 	2:5:Class:CWebBrowser2
# 	2:10:HeaderFile:webbrowser2.h
# 	2:8:ImplFile:webbrowser2.cpp
# End Section
# Section SeisWide : {248DD892-BB45-11CF-9ABC-0080C7E7B78D}
# 	2:5:Class:CMSWinsockControl
# 	2:10:HeaderFile:mswinsockcontrol.h
# 	2:8:ImplFile:mswinsockcontrol.cpp
# End Section
# Section SeisWide : {248DD896-BB45-11CF-9ABC-0080C7E7B78D}
# 	2:21:DefaultSinkHeaderFile:mswinsockcontrol.h
# 	2:16:DefaultSinkClass:CMSWinsockControl
# End Section
# Section SeisWide : {8856F961-340A-11D0-A96B-00C04FD705A2}
# 	2:21:DefaultSinkHeaderFile:webbrowser2.h
# 	2:16:DefaultSinkClass:CWebBrowser2
# End Section
# Section SeisWide : {1A8AD555-FF9A-11D1-B034-0000C04E5DE8}
# 	2:19:CLASS: CColorButton:CColorButton
# 	2:15:ColorButton.cpp:ColorButton1.cpp
# 	2:19:Application Include:SeisWide.h
# 	2:13:ColorButton.h:ColorButton1.h
# End Section
