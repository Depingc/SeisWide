# Microsoft Developer Studio Project File - Name="psTramp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) QuickWin Application" 0x0107

CFG=psTramp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "psTramp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "psTramp.mak" CFG="psTramp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "psTramp - Win32 Release" (based on\
 "Win32 (x86) QuickWin Application")
!MESSAGE "psTramp - Win32 Debug" (based on "Win32 (x86) QuickWin Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
F90=df.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "psTramp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE F90 /include:"Release/" /compile_only /nologo /libs:qwin /warn:nofileopt
# ADD F90 /include:"Release/" /compile_only /nologo /libs:qwin /warn:nofileopt
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"WinMainCRTStartup" /subsystem:windows /machine:I386 /nodefaultlib:"dfconsol.lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"WinMainCRTStartup" /subsystem:windows /machine:I386 /nodefaultlib:"dfconsol.lib"

!ELSEIF  "$(CFG)" == "psTramp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE F90 /include:"Debug/" /compile_only /nologo /libs:qwin /debug:full /optimize:0 /warn:nofileopt
# ADD F90 /include:"Debug/" /compile_only /nologo /libs:qwin /debug:full /optimize:0 /warn:nofileopt
# SUBTRACT F90 /f77rtl
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"WinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"dfconsol.lib" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"WinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"dfconsol.lib" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "psTramp - Win32 Release"
# Name "psTramp - Win32 Debug"
# Begin Source File

SOURCE=.\adjpt.f

!IF  "$(CFG)" == "psTramp - Win32 Release"

!ELSEIF  "$(CFG)" == "psTramp - Win32 Debug"

DEP_F90_ADJPT=\
	".\tramp.com"\
	".\tramp.par"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\amp.f

!IF  "$(CFG)" == "psTramp - Win32 Release"

!ELSEIF  "$(CFG)" == "psTramp - Win32 Debug"

DEP_F90_AMP_F=\
	".\tramp.com"\
	".\tramp.par"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\atrc.f

!IF  "$(CFG)" == "psTramp - Win32 Release"

!ELSEIF  "$(CFG)" == "psTramp - Win32 Debug"

DEP_F90_ATRC_=\
	".\tramp.com"\
	".\tramp.par"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\blkdat.f

!IF  "$(CFG)" == "psTramp - Win32 Release"

!ELSEIF  "$(CFG)" == "psTramp - Win32 Debug"

DEP_F90_BLKDA=\
	".\tramp.com"\
	".\tramp.par"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\calmod.f

!IF  "$(CFG)" == "psTramp - Win32 Release"

!ELSEIF  "$(CFG)" == "psTramp - Win32 Debug"

DEP_F90_CALMO=\
	".\tramp.com"\
	".\tramp.par"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hdw.f

!IF  "$(CFG)" == "psTramp - Win32 Release"

!ELSEIF  "$(CFG)" == "psTramp - Win32 Debug"

DEP_F90_HDW_F=\
	".\tramp.com"\
	".\tramp.par"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\main.f

!IF  "$(CFG)" == "psTramp - Win32 Release"

!ELSEIF  "$(CFG)" == "psTramp - Win32 Debug"

DEP_F90_MAIN_=\
	".\tramp.com"\
	".\tramp.par"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\misc.f

!IF  "$(CFG)" == "psTramp - Win32 Release"

!ELSEIF  "$(CFG)" == "psTramp - Win32 Debug"

DEP_F90_MISC_=\
	".\tramp.com"\
	".\tramp.par"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\plt.f

!IF  "$(CFG)" == "psTramp - Win32 Release"

!ELSEIF  "$(CFG)" == "psTramp - Win32 Debug"

DEP_F90_PLT_F=\
	".\tramp.com"\
	".\tramp.par"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PLTSUB.FOR
# End Source File
# Begin Source File

SOURCE=.\psPltlib.f
# End Source File
# Begin Source File

SOURCE=.\rngkta.f

!IF  "$(CFG)" == "psTramp - Win32 Release"

!ELSEIF  "$(CFG)" == "psTramp - Win32 Debug"

DEP_F90_RNGKT=\
	".\tramp.com"\
	".\tramp.par"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\trc.f

!IF  "$(CFG)" == "psTramp - Win32 Release"

!ELSEIF  "$(CFG)" == "psTramp - Win32 Debug"

DEP_F90_TRC_F=\
	".\tramp.com"\
	".\tramp.par"\
	

!ENDIF 

# End Source File
# End Target
# End Project
