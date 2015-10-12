# Microsoft Developer Studio Project File - Name="RTPtest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=RTPtest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RTPtest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RTPtest.mak" CFG="RTPtest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RTPtest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "RTPtest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RTPtest - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "RTPtest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\." /I "..\..\..\..\include" /I "System\Simulation\Win32" /I "inc" /I "GenOSSrc" /I "System\Simulation\embOS" /I "System" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ws2_32.lib libcmtd.lib /nologo /subsystem:console /map /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no /nodefaultlib

!ENDIF 

# Begin Target

# Name "RTPtest - Win32 Release"
# Name "RTPtest - Win32 Debug"
# Begin Group "Application"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Application\OS_Error.c
# End Source File
# Begin Source File

SOURCE=.\Application\RTPtest.c
# End Source File
# End Group
# Begin Group "RTPgeneric"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\generic\rtp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\generic\rtpbsearch.c
# End Source File
# Begin Source File

SOURCE=..\..\..\generic\rtpchar.c
# End Source File
# Begin Source File

SOURCE=..\..\..\generic\rtpdllist.c
# End Source File
# Begin Source File

SOURCE=..\..\..\generic\rtpmemdb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\generic\rtpnetsm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\generic\rtprot.c
# End Source File
# Begin Source File

SOURCE=..\..\..\generic\rtpstr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\generic\rtpwchar.c
# End Source File
# Begin Source File

SOURCE=..\..\..\generic\rtpwcs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\generic\rtpwscnv.c
# End Source File
# End Group
# Begin Group "RTPtarget"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\rtpirq.c
# End Source File
# Begin Source File

SOURCE=..\..\rtpkern.c
# End Source File
# Begin Source File

SOURCE=..\..\rtpmem.c
# End Source File
# Begin Source File

SOURCE=..\..\rtpsignl.c
# End Source File
# Begin Source File

SOURCE=..\..\rtpterm.c
# End Source File
# Begin Source File

SOURCE=..\..\rtpthrd.c
# End Source File
# Begin Source File

SOURCE=..\..\rtptime.c
# End Source File
# End Group
# Begin Group "embOS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\System\Simulation\embOS\Rtosinit.c
# End Source File
# Begin Source File

SOURCE=.\System\Simulation\embOS\embOSDP.lib
# End Source File
# End Group
# End Target
# End Project
