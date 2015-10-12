# Microsoft Developer Studio Project File - Name="mozjs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mozjs - Win32 Debug JavaScript Unicode 8bpp
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mozjs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mozjs.mak" CFG="mozjs - Win32 Debug JavaScript Unicode 8bpp"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mozjs - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mozjs - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mozjs - Win32 Debug JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "mozjs - Win32 Debug JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "mozjs - Win32 Release JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "mozjs - Win32 Release JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "mozjs - Win32 Debug JavaScript Unicode 8bpp" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mozjs - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_JSCRIPT=0 /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mozjs - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D WEBC_SUPPORT_JSCRIPT=1 /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mozjs - Win32 Debug JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mozjs___Win32_Debug_JavaScript"
# PROP BASE Intermediate_Dir "mozjs___Win32_Debug_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "mozjs___Win32_Debug_JavaScript"
# PROP Intermediate_Dir "mozjs___Win32_Debug_JavaScript"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "WEBC_SUPPORT_JSCRIPT" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WEBC_SUPPORT_JSCRIPT" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mozjs - Win32 Debug JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mozjs___Win32_Debug_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "mozjs___Win32_Debug_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "mozjs___Win32_Debug_JavaScript_Unicode"
# PROP Intermediate_Dir "mozjs___Win32_Debug_JavaScript_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "WEBC_SUPPORT_JSCRIPT" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WEBC_SUPPORT_JSCRIPT" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mozjs - Win32 Release JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mozjs___Win32_Release_JavaScript"
# PROP BASE Intermediate_Dir "mozjs___Win32_Release_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "mozjs___Win32_Release_JavaScript"
# PROP Intermediate_Dir "mozjs___Win32_Release_JavaScript"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_JSCRIPT=0 /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_JSCRIPT=0 /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mozjs - Win32 Release JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mozjs___Win32_Release_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "mozjs___Win32_Release_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "mozjs___Win32_Release_JavaScript_Unicode"
# PROP Intermediate_Dir "mozjs___Win32_Release_JavaScript_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_JSCRIPT=0 /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_JSCRIPT=0 /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mozjs - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mozjs___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Intermediate_Dir "mozjs___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "mozjs___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Intermediate_Dir "mozjs___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "WEBC_SUPPORT_JSCRIPT" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WEBC_SUPPORT_JSCRIPT" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DEBUG" /YX /FD /GZ /c
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

# Name "mozjs - Win32 Release"
# Name "mozjs - Win32 Debug"
# Name "mozjs - Win32 Debug JavaScript"
# Name "mozjs - Win32 Debug JavaScript Unicode"
# Name "mozjs - Win32 Release JavaScript"
# Name "mozjs - Win32 Release JavaScript Unicode"
# Name "mozjs - Win32 Debug JavaScript Unicode 8bpp"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsapi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsarena.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsarray.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsatom.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsbool.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jscntxt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsdate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsdbgapi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsdhash.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsdtoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsemit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsexn.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsfile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsfun.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsgc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jshash.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsinterp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jslock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jslog2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jslong.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsmath.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsnum.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsobj.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsopcode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsparse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsprf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsregexp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsscan.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsscope.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsscript.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsstr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsutil.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\jsxdrapi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mozilla\prmjtime.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
