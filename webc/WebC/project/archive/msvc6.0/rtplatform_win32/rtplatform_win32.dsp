# Microsoft Developer Studio Project File - Name="rtplatform_win32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=rtplatform_win32 - Win32 Debug JavaScript Unicode 8bpp
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rtplatform_win32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rtplatform_win32.mak" CFG="rtplatform_win32 - Win32 Debug JavaScript Unicode 8bpp"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rtplatform_win32 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "rtplatform_win32 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "rtplatform_win32 - Win32 Debug JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "rtplatform_win32 - Win32 Debug JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "rtplatform_win32 - Win32 Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "rtplatform_win32 - Win32 Release JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "rtplatform_win32 - Win32 Release JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "rtplatform_win32 - Win32 Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "rtplatform_win32 - Win32 Debug JavaScript Unicode 8bpp" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rtplatform_win32 - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\openssl\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "rtplatform_win32 - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\openssl\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /D "RTP_MEM_DEBUG_FILE" /D "RTP_MEM_DEBUG_USE_STDIO" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "rtplatform_win32 - Win32 Debug JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "rtplatform_win32___Win32_Debug_JavaScript"
# PROP BASE Intermediate_Dir "rtplatform_win32___Win32_Debug_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "rtplatform_win32___Win32_Debug_JavaScript"
# PROP Intermediate_Dir "rtplatform_win32___Win32_Debug_JavaScript"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\openssl\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /D "RTP_MEM_DEBUG_FILE" /D "RTP_MEM_DEBUG_USE_STDIO" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "rtplatform_win32 - Win32 Debug JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "rtplatform_win32___Win32_Debug_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "rtplatform_win32___Win32_Debug_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "rtplatform_win32___Win32_Debug_JavaScript_Unicode"
# PROP Intermediate_Dir "rtplatform_win32___Win32_Debug_JavaScript_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\openssl\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /D "RTP_MEM_DEBUG_FILE" /D "RTP_MEM_DEBUG_USE_STDIO" /D RTP_MEM_RESTRAIN=0 /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "rtplatform_win32 - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "rtplatform_win32___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "rtplatform_win32___Win32_Debug_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "rtplatform_win32___Win32_Debug_Unicode"
# PROP Intermediate_Dir "rtplatform_win32___Win32_Debug_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /D "RTP_MEM_DEBUG_FILE" /D "RTP_MEM_DEBUG_USE_STDIO" /D RTP_MEM_RESTRAIN=0 /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\openssl\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /D "RTP_MEM_DEBUG_FILE" /D "RTP_MEM_DEBUG_USE_STDIO" /D RTP_MEM_RESTRAIN=0 /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "rtplatform_win32 - Win32 Release JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "rtplatform_win32___Win32_Release_JavaScript"
# PROP BASE Intermediate_Dir "rtplatform_win32___Win32_Release_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "rtplatform_win32___Win32_Release_JavaScript"
# PROP Intermediate_Dir "rtplatform_win32___Win32_Release_JavaScript"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\openssl\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "rtplatform_win32 - Win32 Release JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "rtplatform_win32___Win32_Release_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "rtplatform_win32___Win32_Release_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "rtplatform_win32___Win32_Release_JavaScript_Unicode"
# PROP Intermediate_Dir "rtplatform_win32___Win32_Release_JavaScript_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\openssl\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "rtplatform_win32 - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "rtplatform_win32___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "rtplatform_win32___Win32_Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "rtplatform_win32___Win32_Release_Unicode"
# PROP Intermediate_Dir "rtplatform_win32___Win32_Release_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\openssl\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "rtplatform_win32 - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "rtplatform_win32___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Intermediate_Dir "rtplatform_win32___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "rtplatform_win32___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Intermediate_Dir "rtplatform_win32___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /D "RTP_MEM_DEBUG_FILE" /D "RTP_MEM_DEBUG_USE_STDIO" /D RTP_MEM_RESTRAIN=0 /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\openssl\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /D "RTP_MEM_DEBUG_FILE" /D "RTP_MEM_DEBUG_USE_STDIO" /D RTP_MEM_RESTRAIN=0 /YX /FD /GZ /c
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

# Name "rtplatform_win32 - Win32 Release"
# Name "rtplatform_win32 - Win32 Debug"
# Name "rtplatform_win32 - Win32 Debug JavaScript"
# Name "rtplatform_win32 - Win32 Debug JavaScript Unicode"
# Name "rtplatform_win32 - Win32 Debug Unicode"
# Name "rtplatform_win32 - Win32 Release JavaScript"
# Name "rtplatform_win32 - Win32 Release JavaScript Unicode"
# Name "rtplatform_win32 - Win32 Release Unicode"
# Name "rtplatform_win32 - Win32 Debug JavaScript Unicode 8bpp"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\generic\rtpchar.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\win32\rtpdate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\generic\rtpdutil.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\win32\rtpfile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\win32\rtpmem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\generic\rtpmemdb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\win32\rtpnet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\generic\rtprand.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\openssl\rtpssl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\generic\rtpstr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\win32\rtptime.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\generic\rtpwchar.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\generic\rtpwcs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\rtplatform\v0.4\source\generic\rtpwscnv.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
