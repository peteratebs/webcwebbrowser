# Microsoft Developer Studio Project File - Name="libjpeg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libjpeg - Win32 Debug JavaScript Unicode 8bpp
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libjpeg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libjpeg.mak" CFG="libjpeg - Win32 Debug JavaScript Unicode 8bpp"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libjpeg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libjpeg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libjpeg - Win32 Debug JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "libjpeg - Win32 Debug JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "libjpeg - Win32 Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "libjpeg - Win32 Release JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "libjpeg - Win32 Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "libjpeg - Win32 Release JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "libjpeg - Win32 Debug JavaScript Unicode 8bpp" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libjpeg - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libjpeg - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\lib\libjpeg" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libjpeg - Win32 Debug JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libjpeg___Win32_Debug_JavaScript"
# PROP BASE Intermediate_Dir "libjpeg___Win32_Debug_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libjpeg___Win32_Debug_JavaScript"
# PROP Intermediate_Dir "libjpeg___Win32_Debug_JavaScript"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\lib\libjpeg" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\lib\libjpeg" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libjpeg - Win32 Debug JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libjpeg___Win32_Debug_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "libjpeg___Win32_Debug_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libjpeg___Win32_Debug_JavaScript_Unicode"
# PROP Intermediate_Dir "libjpeg___Win32_Debug_JavaScript_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\lib\libjpeg" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\lib\libjpeg" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libjpeg - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libjpeg___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "libjpeg___Win32_Debug_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libjpeg___Win32_Debug_Unicode"
# PROP Intermediate_Dir "libjpeg___Win32_Debug_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\lib\libjpeg" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\lib\libjpeg" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libjpeg - Win32 Release JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libjpeg___Win32_Release_JavaScript"
# PROP BASE Intermediate_Dir "libjpeg___Win32_Release_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libjpeg___Win32_Release_JavaScript"
# PROP Intermediate_Dir "libjpeg___Win32_Release_JavaScript"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libjpeg - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libjpeg___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "libjpeg___Win32_Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libjpeg___Win32_Release_Unicode"
# PROP Intermediate_Dir "libjpeg___Win32_Release_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libjpeg - Win32 Release JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libjpeg___Win32_Release_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "libjpeg___Win32_Release_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libjpeg___Win32_Release_JavaScript_Unicode"
# PROP Intermediate_Dir "libjpeg___Win32_Release_JavaScript_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libjpeg - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libjpeg___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Intermediate_Dir "libjpeg___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libjpeg___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Intermediate_Dir "libjpeg___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\lib\libjpeg" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\lib\libjpeg" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libjpeg - Win32 Release"
# Name "libjpeg - Win32 Debug"
# Name "libjpeg - Win32 Debug JavaScript"
# Name "libjpeg - Win32 Debug JavaScript Unicode"
# Name "libjpeg - Win32 Debug Unicode"
# Name "libjpeg - Win32 Release JavaScript"
# Name "libjpeg - Win32 Release Unicode"
# Name "libjpeg - Win32 Release JavaScript Unicode"
# Name "libjpeg - Win32 Debug JavaScript Unicode 8bpp"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jcapimin.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jcapistd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jccoefct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jccolor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jchuff.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jcinit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jcmainct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jcmarker.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jcmaster.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jcomapi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jcparam.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jcphuff.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jcprepct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jcsample.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jctrans.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdapimin.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdapistd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdatadst.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdatasrc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdcolor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdhuff.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdinput.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdmainct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdmarker.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdmaster.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdmerge.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdphuff.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdpostct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdsample.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jdtrans.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jerror.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jfdctint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jidctflt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jidctfst.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jidctint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jidctred.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jmemnobs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jquant1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jquant2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\libjpeg\jutils.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
