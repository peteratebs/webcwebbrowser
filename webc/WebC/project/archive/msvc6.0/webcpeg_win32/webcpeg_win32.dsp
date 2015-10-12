# Microsoft Developer Studio Project File - Name="webcpeg_win32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=webcpeg_win32 - Win32 Debug JavaScript Unicode 8bpp
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "webcpeg_win32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "webcpeg_win32.mak" CFG="webcpeg_win32 - Win32 Debug JavaScript Unicode 8bpp"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "webcpeg_win32 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "webcpeg_win32 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "webcpeg_win32 - Win32 Debug JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "webcpeg_win32 - Win32 Debug JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "webcpeg_win32 - Win32 Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "webcpeg_win32 - Win32 Release JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "webcpeg_win32 - Win32 Release JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "webcpeg_win32 - Win32 Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "webcpeg_win32 - Win32 Debug JavaScript Unicode 8bpp" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "webcpeg_win32 - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\gui\peg\include" /I "..\..\..\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "NDEBUG" /D WEBC_SUPPORT_UNICODE=0 /D "WIN32" /D "_MBCS" /D "_LIB" /D "WEBC_USE_PEG" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "webcpeg_win32 - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\gui\peg\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "_DEBUG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /D "WIN32" /D "_MBCS" /D "_LIB" /D "WEBC_USE_PEG" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\webcpeg.lib"

!ELSEIF  "$(CFG)" == "webcpeg_win32 - Win32 Debug JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "webcpeg_win32___Win32_Debug_JavaScript"
# PROP BASE Intermediate_Dir "webcpeg_win32___Win32_Debug_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "webcpeg_win32___Win32_Debug_JavaScript"
# PROP Intermediate_Dir "webcpeg_win32___Win32_Debug_JavaScript"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\gui\peg\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "WEBC_USE_PEG" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\gui\peg\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "_DEBUG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /D "WIN32" /D "_MBCS" /D "_LIB" /D "WEBC_USE_PEG" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\webcpeg.lib"
# ADD LIB32 /nologo /out:"Debug\webcpeg.lib"

!ELSEIF  "$(CFG)" == "webcpeg_win32 - Win32 Debug JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "webcpeg_win32___Win32_Debug_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "webcpeg_win32___Win32_Debug_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "webcpeg_win32___Win32_Debug_JavaScript_Unicode"
# PROP Intermediate_Dir "webcpeg_win32___Win32_Debug_JavaScript_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\gui\peg\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "WEBC_USE_PEG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\gui\peg\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "_DEBUG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /D WEBC_SUPPORT_UNICODE=1 /D "PEG_UNICODE" /D "WIN32" /D "_MBCS" /D "_LIB" /D "WEBC_USE_PEG" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\webcpeg.lib"
# ADD LIB32 /nologo /out:"Debug\webcpeg.lib"

!ELSEIF  "$(CFG)" == "webcpeg_win32 - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "webcpeg_win32___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "webcpeg_win32___Win32_Debug_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "webcpeg_win32___Win32_Debug_Unicode"
# PROP Intermediate_Dir "webcpeg_win32___Win32_Debug_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\gui\peg\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "WEBC_USE_PEG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /D WEBC_SUPPORT_UNICODE=1 /D "PEG_UNICODE" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\gui\peg\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "_DEBUG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /D WEBC_SUPPORT_UNICODE=1 /D "PEG_UNICODE" /D "WIN32" /D "_MBCS" /D "_LIB" /D "WEBC_USE_PEG" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\webcpeg.lib"
# ADD LIB32 /nologo /out:"Debug\webcpeg.lib"

!ELSEIF  "$(CFG)" == "webcpeg_win32 - Win32 Release JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "webcpeg_win32___Win32_Release_JavaScript"
# PROP BASE Intermediate_Dir "webcpeg_win32___Win32_Release_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "webcpeg_win32___Win32_Release_JavaScript"
# PROP Intermediate_Dir "webcpeg_win32___Win32_Release_JavaScript"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "..\..\..\gui\peg\include" /I "..\..\..\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_UNICODE=0 /D "WEBC_USE_PEG" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\gui\peg\include" /I "..\..\..\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "NDEBUG" /D WEBC_SUPPORT_UNICODE=0 /D "WIN32" /D "_MBCS" /D "_LIB" /D "WEBC_USE_PEG" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "webcpeg_win32 - Win32 Release JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "webcpeg_win32___Win32_Release_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "webcpeg_win32___Win32_Release_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "webcpeg_win32___Win32_Release_JavaScript_Unicode"
# PROP Intermediate_Dir "webcpeg_win32___Win32_Release_JavaScript_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "..\..\..\gui\peg\include" /I "..\..\..\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_UNICODE=0 /D "WEBC_USE_PEG" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\gui\peg\include" /I "..\..\..\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_UNICODE=1 /D "WEBC_USE_PEG" /D "PEG_UNICODE" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "webcpeg_win32 - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "webcpeg_win32___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "webcpeg_win32___Win32_Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "webcpeg_win32___Win32_Release_Unicode"
# PROP Intermediate_Dir "webcpeg_win32___Win32_Release_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "..\..\..\gui\peg\include" /I "..\..\..\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_UNICODE=0 /D "WEBC_USE_PEG" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\gui\peg\include" /I "..\..\..\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "NDEBUG" /D WEBC_SUPPORT_UNICODE=1 /D "WIN32" /D "_MBCS" /D "_LIB" /D "WEBC_USE_PEG" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /D "PEG_UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "webcpeg_win32 - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "webcpeg_win32___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Intermediate_Dir "webcpeg_win32___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "webcpeg_win32___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Intermediate_Dir "webcpeg_win32___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\gui\peg\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "WEBC_USE_PEG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /D WEBC_SUPPORT_UNICODE=1 /D "PEG_UNICODE" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\gui\peg\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "WEBC_USE_PEG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /D WEBC_SUPPORT_UNICODE=1 /D "PEG_UNICODE" /D PEG_NUM_COLORS=256 /D PSCREEN=<l8scrn.hpp> /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\webcpeg.lib"
# ADD LIB32 /nologo /out:"Debug\webcpeg.lib"

!ENDIF 

# Begin Target

# Name "webcpeg_win32 - Win32 Release"
# Name "webcpeg_win32 - Win32 Debug"
# Name "webcpeg_win32 - Win32 Debug JavaScript"
# Name "webcpeg_win32 - Win32 Debug JavaScript Unicode"
# Name "webcpeg_win32 - Win32 Debug Unicode"
# Name "webcpeg_win32 - Win32 Release JavaScript"
# Name "webcpeg_win32 - Win32 Release JavaScript Unicode"
# Name "webcpeg_win32 - Win32 Release Unicode"
# Name "webcpeg_win32 - Win32 Debug JavaScript Unicode 8bpp"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Fonts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial10.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial10b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial10bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial10i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial11.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial11b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial11bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial11i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial12.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial12b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial12bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial12i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial13.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial13b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial13bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial13i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial14.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial14b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial14bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial14i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial16.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial16b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial16bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial16i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial18.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial18b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial18bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial18i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial20.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial20b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial20bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial20i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial24.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial24b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial24bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial24i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial30.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial30b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial30bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial30i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial36.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial36b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial36bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial36i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial6.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial6b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial6bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial6i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial7.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial7b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial7bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial7i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial8.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial8b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial8bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial8i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial9.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial9b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial9bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Arial9i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\ArialUnicode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier10.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier10b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier10bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier10i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier11.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier11b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier11bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier11i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier12.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier12b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier12bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier12i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier13.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier13b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier13bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier13i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier14.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier14b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier14bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier14i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier16.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier16b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier16bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier16i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier18.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier18b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier18bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier18i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier20.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier20b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier20bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier20i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier24.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier24b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier24bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier24i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier30.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier30b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier30bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier30i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier36.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier36b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier36bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier36i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier6.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier6b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier6bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier6i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier7.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier7b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier7bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier7i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier8.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier8b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier8bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier8i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier9.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier9b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier9bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Courier9i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES1.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times10.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times10b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times10bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times10i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times11.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times11b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times11bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times11i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times12.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times12b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times12bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times12i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times13.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times13b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times13bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times13i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times14.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times14b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times14bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times14i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times16.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times16b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times16bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times16i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times18.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times18b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times18bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times18i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES1B.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES1BI.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES1I.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES2.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times20.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times20b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times20bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times20i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times24.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times24b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times24bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times24i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES2B.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES2BI.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES2I.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES3.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times30.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times30b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times30bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times30i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times36.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times36b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times36bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times36i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES3B.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES3BI.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES3I.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES4.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES4B.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES4BI.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES4I.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES5.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES5B.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES5BI.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES5I.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES6.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES6B.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES6BI.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES6I.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES7.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES7B.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES7BI.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\TIMES7I.CPP
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times8.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times8b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times8bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times8i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times9.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times9b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times9bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Times9i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana10.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana10b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana10bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana10i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana11.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana11b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana11bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana11i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana12.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana12b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana12bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana12i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana13.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana13b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana13bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana13i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana14.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana14b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana14bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana14i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana16.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana16b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana16bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana16i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana18.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana18b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana18bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana18i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana20.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana20b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana20bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana20i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana24.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana24b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana24bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana24i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana30.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana30b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana30bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana30i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana36.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana36b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana36bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana36i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana6.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana6b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana6bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana6i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana7.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana7b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana7bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana7i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana8.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana8B.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana8bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana8i.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana9.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana9b.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana9bi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\fonts\Verdana9i.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\gui\peg\source\pwebwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\webcpeg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\gui\peg\source\wfpeg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
