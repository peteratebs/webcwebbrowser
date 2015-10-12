# Microsoft Developer Studio Project File - Name="peg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=peg - Win32 Debug JavaScript Unicode 8bpp
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "peg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "peg.mak" CFG="peg - Win32 Debug JavaScript Unicode 8bpp"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "peg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "peg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "peg - Win32 Debug JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "peg - Win32 Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "peg - Win32 Debug JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "peg - Win32 Release JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "peg - Win32 Release JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "peg - Win32 Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "peg - Win32 Debug JavaScript Unicode 8bpp" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "peg - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "peg - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "peg - Win32 Debug JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "peg___Win32_Debug_JavaScript"
# PROP BASE Intermediate_Dir "peg___Win32_Debug_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "peg___Win32_Debug_JavaScript"
# PROP Intermediate_Dir "peg___Win32_Debug_JavaScript"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "peg - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "peg___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "peg___Win32_Debug_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "peg___Win32_Debug_Unicode"
# PROP Intermediate_Dir "peg___Win32_Debug_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "_DEBUG" /D "PEG_UNICODE" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "peg - Win32 Debug JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "peg___Win32_Debug_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "peg___Win32_Debug_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "peg___Win32_Debug_JavaScript_Unicode"
# PROP Intermediate_Dir "peg___Win32_Debug_JavaScript_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "_DEBUG" /D "PEG_UNICODE" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "peg - Win32 Release JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "peg___Win32_Release_JavaScript"
# PROP BASE Intermediate_Dir "peg___Win32_Release_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "peg___Win32_Release_JavaScript"
# PROP Intermediate_Dir "peg___Win32_Release_JavaScript"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "peg - Win32 Release JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "peg___Win32_Release_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "peg___Win32_Release_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "peg___Win32_Release_JavaScript_Unicode"
# PROP Intermediate_Dir "peg___Win32_Release_JavaScript_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "NDEBUG" /D "PEG_UNICODE" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "peg - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "peg___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "peg___Win32_Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "peg___Win32_Release_Unicode"
# PROP Intermediate_Dir "peg___Win32_Release_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "NDEBUG" /D "PEG_UNICODE" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "peg - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "peg___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Intermediate_Dir "peg___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "peg___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Intermediate_Dir "peg___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /D "PEG_UNICODE" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WEBC__" /D "PEG_UNICODE" /D PEG_NUM_COLORS=256 /D PSCREEN=<l8scrn.hpp> /YX /FD /GZ /c
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

# Name "peg - Win32 Release"
# Name "peg - Win32 Debug"
# Name "peg - Win32 Debug JavaScript"
# Name "peg - Win32 Debug Unicode"
# Name "peg - Win32 Debug JavaScript Unicode"
# Name "peg - Win32 Release JavaScript"
# Name "peg - Win32 Release JavaScript Unicode"
# Name "peg - Win32 Release Unicode"
# Name "peg - Win32 Debug JavaScript Unicode 8bpp"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\scrndrv\l16scrn.cpp

!IF  "$(CFG)" == "peg - Win32 Release"

!ELSEIF  "$(CFG)" == "peg - Win32 Debug"

!ELSEIF  "$(CFG)" == "peg - Win32 Debug JavaScript"

!ELSEIF  "$(CFG)" == "peg - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "peg - Win32 Debug JavaScript Unicode"

!ELSEIF  "$(CFG)" == "peg - Win32 Release JavaScript"

!ELSEIF  "$(CFG)" == "peg - Win32 Release JavaScript Unicode"

!ELSEIF  "$(CFG)" == "peg - Win32 Release Unicode"

!ELSEIF  "$(CFG)" == "peg - Win32 Debug JavaScript Unicode 8bpp"

# PROP Exclude_From_Build 1
# ADD CPP /D PEG_NUM_COLORS=256 /D PSCREEN="l8scrn.hpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\scrndrv\l8scrn.cpp

!IF  "$(CFG)" == "peg - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "peg - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "peg - Win32 Debug JavaScript"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "peg - Win32 Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "peg - Win32 Debug JavaScript Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "peg - Win32 Release JavaScript"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "peg - Win32 Release JavaScript Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "peg - Win32 Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "peg - Win32 Debug JavaScript Unicode 8bpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\pal256.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\pbitmaps.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\pbutton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\pdecwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\pfonts.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\picon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\pliteral.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\pmenfont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\pmenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\pmessage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\ppresent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\prect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\pscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\pscroll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\psysfont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\pthing.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\ptitle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\ptxtthng.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\peg\source\pwindow.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
