# Microsoft Developer Studio Project File - Name="Start" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Start - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Start.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Start.mak" CFG="Start - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Start - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Start - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Start - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Start___Win32_Release"
# PROP BASE Intermediate_Dir "Start___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Output\Release"
# PROP Intermediate_Dir "Output\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /O2 /I "inc" /I "GenOSSrc" /I "System\Simulation\embOS" /I "System\Simulation\Win32" /I "System" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib shell32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Start - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Start___Win32_Debug0"
# PROP BASE Intermediate_Dir "Start___Win32_Debug0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Output\Debug"
# PROP Intermediate_Dir "Output\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /ML /W3 /Gm /Zi /Od /Gy /I "System\Simulation\Win32" /I "inc" /I "GenOSSrc" /I "System\Simulation\embOS" /I "System" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib shell32.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Start - Win32 Release"
# Name "Start - Win32 Debug"
# Begin Group "Application"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Application\main.c
# End Source File
# Begin Source File

SOURCE=.\Application\OS_Error.c
# End Source File
# End Group
# Begin Group "System"

# PROP Default_Filter ""
# Begin Group "Simulation"

# PROP Default_Filter ""
# Begin Group "embOS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\System\Simulation\embOS\rtosinit.c
# End Source File
# Begin Source File

SOURCE=.\System\Simulation\embOS\embOSDP.lib
# End Source File
# End Group
# Begin Group "Win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\System\Simulation\Win32\Device.bmp
# End Source File
# Begin Source File

SOURCE=.\System\Simulation\Win32\resource.h
# End Source File
# Begin Source File

SOURCE=.\System\Simulation\Win32\SIM.c
# End Source File
# Begin Source File

SOURCE=.\System\Simulation\Win32\SIM.h
# End Source File
# Begin Source File

SOURCE=.\System\Simulation\Win32\Simulation.ico
# End Source File
# Begin Source File

SOURCE=.\System\Simulation\Win32\Simulation.rc
# End Source File
# Begin Source File

SOURCE=.\System\Simulation\Win32\Small.ico
# End Source File
# Begin Source File

SOURCE=.\System\Simulation\Win32\WinMain.c
# End Source File
# End Group
# Begin Group "Hardware"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\System\Simulation\Hardware\HW_LED.c
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\System\HW_LED.h
# End Source File
# End Group
# End Target
# End Project
