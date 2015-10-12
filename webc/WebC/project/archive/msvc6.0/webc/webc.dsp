# Microsoft Developer Studio Project File - Name="webc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=webc - Win32 Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "webc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "webc.mak" CFG="webc - Win32 Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "webc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "webc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "webc - Win32 Release JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "webc - Win32 Debug JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "webc - Win32 Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "webc - Win32 Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "webc - Win32 Release JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "webc - Win32 Debug JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "webc - Win32 Debug JavaScript Unicode 8bpp" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "webc - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\html\include" /I "..\..\..\lib\libjpeg" /I "..\..\..\lib\zlib" /I "..\..\..\lib\libmng" /I "..\..\..\lib\libpng" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "NDEBUG" /D WEBC_SUPPORT_JSCRIPT=0 /D "__WEBC__" /D "WIN32" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_UNICODE=0 /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\html\include" /I "..\..\..\lib\libjpeg" /I "..\..\..\lib\zlib" /I "..\..\..\lib\libmng" /I "..\..\..\lib\libpng" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "_DEBUG" /D WEBC_SUPPORT_JSCRIPT=0 /D WEBC_SUPPORT_UNICODE=0 /D "WEBC_DEBUG" /D "__WEBC__" /D "WIN32" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "webc___Win32_Release_JavaScript"
# PROP BASE Intermediate_Dir "webc___Win32_Release_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "webc___Win32_Release_JavaScript"
# PROP Intermediate_Dir "webc___Win32_Release_JavaScript"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\html\include" /I "..\..\..\lib\libjpeg" /I "..\..\..\lib\zlib" /I "..\..\..\lib\libmng" /I "..\..\..\lib\libpng" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\mozilla" /D "NDEBUG" /D WEBC_SUPPORT_JSCRIPT=1 /D "__WEBC__" /D "WIN32" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_UNICODE=0 /D WEBC_SUPPORT_HTTPS=1 /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "webc___Win32_Debug_JavaScript"
# PROP BASE Intermediate_Dir "webc___Win32_Debug_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "webc___Win32_Debug_JavaScript"
# PROP Intermediate_Dir "webc___Win32_Debug_JavaScript"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\html\include" /I "..\..\..\lib\mozilla" /I "..\..\..\lib\libjpeg" /I "..\..\..\lib\zlib" /I "..\..\..\lib\libmng" /I "..\..\..\lib\libpng" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_UNICODE=0 /D "__WEBC__" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\lib\mozilla" /I "..\..\..\html\include" /I "..\..\..\lib\libjpeg" /I "..\..\..\lib\zlib" /I "..\..\..\lib\libmng" /I "..\..\..\lib\libpng" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "_DEBUG" /D WEBC_SUPPORT_UNICODE=0 /D WEBC_SUPPORT_JSCRIPT=1 /D "WEBC_DEBUG" /D "__WEBC__" /D "WIN32" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "webc___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "webc___Win32_Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "webc___Win32_Release_Unicode"
# PROP Intermediate_Dir "webc___Win32_Release_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\html\include" /I "..\..\..\lib\libjpeg" /I "..\..\..\lib\zlib" /I "..\..\..\lib\libmng" /I "..\..\..\lib\libpng" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "NDEBUG" /D WEBC_SUPPORT_JSCRIPT=0 /D "__WEBC__" /D "WIN32" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_UNICODE=1 /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "webc___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "webc___Win32_Debug_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "webc___Win32_Debug_Unicode"
# PROP Intermediate_Dir "webc___Win32_Debug_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\html\include" /I "..\..\..\lib\mozilla" /I "..\..\..\lib\libjpeg" /I "..\..\..\lib\zlib" /I "..\..\..\lib\libmng" /I "..\..\..\lib\libpng" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_UNICODE=0 /D "__WEBC__" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\html\include" /I "..\..\..\lib\libjpeg" /I "..\..\..\lib\zlib" /I "..\..\..\lib\libmng" /I "..\..\..\lib\libpng" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /D "_DEBUG" /D WEBC_SUPPORT_JSCRIPT=0 /D WEBC_SUPPORT_UNICODE=1 /D "WEBC_DEBUG" /D "__WEBC__" /D "WIN32" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "webc___Win32_Release_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "webc___Win32_Release_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "webc___Win32_Release_JavaScript_Unicode"
# PROP Intermediate_Dir "webc___Win32_Release_JavaScript_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\html\include" /I "..\..\..\lib\libjpeg" /I "..\..\..\lib\zlib" /I "..\..\..\lib\libmng" /I "..\..\..\lib\libpng" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\mozilla" /D "NDEBUG" /D WEBC_SUPPORT_JSCRIPT=1 /D "__WEBC__" /D "WIN32" /D "_MBCS" /D "_LIB" /D WEBC_SUPPORT_UNICODE=1 /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "webc___Win32_Debug_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "webc___Win32_Debug_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "webc___Win32_Debug_JavaScript_Unicode"
# PROP Intermediate_Dir "webc___Win32_Debug_JavaScript_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\html\include" /I "..\..\..\lib\libjpeg" /I "..\..\..\lib\zlib" /I "..\..\..\lib\libmng" /I "..\..\..\lib\libpng" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\mozilla" /D "_DEBUG" /D "WEBC_DEBUG" /D WEBC_SUPPORT_JSCRIPT=1 /D WEBC_SUPPORT_UNICODE=1 /D "__WEBC__" /D "WIN32" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /D WEBC_SUPPORT_HTTPS=1 /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "webc___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Intermediate_Dir "webc___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "webc___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Intermediate_Dir "webc___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\html\include" /I "..\..\..\lib\libjpeg" /I "..\..\..\lib\zlib" /I "..\..\..\lib\libmng" /I "..\..\..\lib\libpng" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\mozilla" /D "_DEBUG" /D "WEBC_DEBUG" /D WEBC_SUPPORT_JSCRIPT=1 /D WEBC_SUPPORT_UNICODE=1 /D "__WEBC__" /D "WIN32" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /FR /YX /FD /c
# ADD CPP /nologo /W3 /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\html\include" /I "..\..\..\lib\libjpeg" /I "..\..\..\lib\zlib" /I "..\..\..\lib\libmng" /I "..\..\..\lib\libpng" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\lib\mozilla" /D "_DEBUG" /D "WEBC_DEBUG" /D WEBC_SUPPORT_JSCRIPT=1 /D WEBC_SUPPORT_UNICODE=1 /D "__WEBC__" /D "WIN32" /D "_MBCS" /D "_LIB" /D "RTP_TRACK_LOCAL_MEMORY" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "webc - Win32 Release"
# Name "webc - Win32 Debug"
# Name "webc - Win32 Release JavaScript"
# Name "webc - Win32 Debug JavaScript"
# Name "webc - Win32 Release Unicode"
# Name "webc - Win32 Debug Unicode"
# Name "webc - Win32 Release JavaScript Unicode"
# Name "webc - Win32 Debug JavaScript Unicode"
# Name "webc - Win32 Debug JavaScript Unicode 8bpp"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Auto-generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\html\source\alert.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\alert_small.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\archive.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\archive_large.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\archive_selected.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\archives.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\archives_pda.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\back_default.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\back_disable.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\back_halo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\back_over.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\back_over_halo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\bookmark_default.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\bookmark_selected.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\bookmarks.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\bookmarks_large.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\bookmarks_pda.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\bottom_border.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\brushedmetal.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\brushedmetal_side.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\chrome.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\chrome_pda.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\close_default.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\close_over.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\disabled_gradiant.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\document.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\ebsnetlogo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\forward_default.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\forward_disable.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\forward_halo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\forward_over.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\forward_over_halo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\home_default.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\home_halo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\home_over.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\home_over_halo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\icon64x64.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\loadChrome.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\loadChrome_pda.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\new_page.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\new_page_over.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\offline.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\offline_disabled.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\offline_toggle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\online.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\online_disabled.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\online_toggle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\prompt_small.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\refresh_default.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\refresh_halo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\refresh_over.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\refresh_over_halo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\savetoarchive.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\savetoarchive_disabled.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\search_bottom.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\search_left.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\search_left_menu.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\search_right.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\search_top.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\select_gradiant.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\slogan.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\small_back.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\small_forward.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\small_refresh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\smallGlobe.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\smallGlobe_gray.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\stop_default.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\stop_disable.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\stop_halo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\stop_over.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\stop_over_halo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\stripes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\top_border.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\url_left.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\url_right.c
# End Source File
# Begin Source File

SOURCE=..\..\..\html\source\welcome.c
# End Source File
# End Group
# Begin Group "Binary Data"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\html\Images\alert.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\alert.png
InputName=alert

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\alert.png
InputName=alert

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\alert.png
InputName=alert

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\alert.png
InputName=alert

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\alert.png
InputName=alert

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\alert.png
InputName=alert

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\alert.png
InputName=alert

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\alert.png
InputName=alert

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\alert.png
InputName=alert

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\alert_small.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\alert_small.png
InputName=alert_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\alert_small.png
InputName=alert_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\alert_small.png
InputName=alert_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\alert_small.png
InputName=alert_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\alert_small.png
InputName=alert_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\alert_small.png
InputName=alert_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\alert_small.png
InputName=alert_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\alert_small.png
InputName=alert_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\alert_small.png
InputName=alert_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\archive.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive.png
InputName=archive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive.png
InputName=archive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive.png
InputName=archive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive.png
InputName=archive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive.png
InputName=archive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive.png
InputName=archive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive.png
InputName=archive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive.png
InputName=archive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive.png
InputName=archive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\archive_large.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_large.png
InputName=archive_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_large.png
InputName=archive_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_large.png
InputName=archive_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_large.png
InputName=archive_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_large.png
InputName=archive_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_large.png
InputName=archive_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_large.png
InputName=archive_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_large.png
InputName=archive_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_large.png
InputName=archive_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\archive_selected.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_selected.png
InputName=archive_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_selected.png
InputName=archive_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_selected.png
InputName=archive_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_selected.png
InputName=archive_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_selected.png
InputName=archive_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_selected.png
InputName=archive_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_selected.png
InputName=archive_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_selected.png
InputName=archive_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archive_selected.png
InputName=archive_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\archives.html

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives.html
InputName=archives

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives.html
InputName=archives

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives.html
InputName=archives

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives.html
InputName=archives

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives.html
InputName=archives

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives.html
InputName=archives

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives.html
InputName=archives

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives.html
InputName=archives

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives.html
InputName=archives

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\archives_pda.html

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives_pda.html
InputName=archives_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives_pda.html
InputName=archives_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives_pda.html
InputName=archives_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives_pda.html
InputName=archives_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives_pda.html
InputName=archives_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives_pda.html
InputName=archives_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives_pda.html
InputName=archives_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives_pda.html
InputName=archives_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\archives_pda.html
InputName=archives_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\back_default.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_default.png
InputName=back_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_default.png
InputName=back_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_default.png
InputName=back_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_default.png
InputName=back_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_default.png
InputName=back_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_default.png
InputName=back_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_default.png
InputName=back_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_default.png
InputName=back_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_default.png
InputName=back_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\back_disable.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_disable.png
InputName=back_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_disable.png
InputName=back_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_disable.png
InputName=back_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_disable.png
InputName=back_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_disable.png
InputName=back_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_disable.png
InputName=back_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_disable.png
InputName=back_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_disable.png
InputName=back_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_disable.png
InputName=back_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\back_halo.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_halo.png
InputName=back_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_halo.png
InputName=back_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_halo.png
InputName=back_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_halo.png
InputName=back_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_halo.png
InputName=back_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_halo.png
InputName=back_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_halo.png
InputName=back_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_halo.png
InputName=back_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_halo.png
InputName=back_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\back_over.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over.png
InputName=back_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over.png
InputName=back_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over.png
InputName=back_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over.png
InputName=back_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over.png
InputName=back_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over.png
InputName=back_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over.png
InputName=back_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over.png
InputName=back_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over.png
InputName=back_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\back_over_halo.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over_halo.png
InputName=back_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over_halo.png
InputName=back_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over_halo.png
InputName=back_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over_halo.png
InputName=back_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over_halo.png
InputName=back_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over_halo.png
InputName=back_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over_halo.png
InputName=back_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over_halo.png
InputName=back_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\back_over_halo.png
InputName=back_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\bookmark_default.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_default.png
InputName=bookmark_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_default.png
InputName=bookmark_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_default.png
InputName=bookmark_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_default.png
InputName=bookmark_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_default.png
InputName=bookmark_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_default.png
InputName=bookmark_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_default.png
InputName=bookmark_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_default.png
InputName=bookmark_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_default.png
InputName=bookmark_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\bookmark_selected.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_selected.png
InputName=bookmark_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_selected.png
InputName=bookmark_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_selected.png
InputName=bookmark_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_selected.png
InputName=bookmark_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_selected.png
InputName=bookmark_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_selected.png
InputName=bookmark_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_selected.png
InputName=bookmark_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_selected.png
InputName=bookmark_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmark_selected.png
InputName=bookmark_selected

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\bookmarks.html

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks.html
InputName=bookmarks

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks.html
InputName=bookmarks

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks.html
InputName=bookmarks

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks.html
InputName=bookmarks

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks.html
InputName=bookmarks

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks.html
InputName=bookmarks

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks.html
InputName=bookmarks

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks.html
InputName=bookmarks

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks.html
InputName=bookmarks

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\bookmarks_large.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_large.png
InputName=bookmarks_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_large.png
InputName=bookmarks_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_large.png
InputName=bookmarks_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_large.png
InputName=bookmarks_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_large.png
InputName=bookmarks_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_large.png
InputName=bookmarks_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_large.png
InputName=bookmarks_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_large.png
InputName=bookmarks_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_large.png
InputName=bookmarks_large

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\bookmarks_pda.html

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_pda.html
InputName=bookmarks_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_pda.html
InputName=bookmarks_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_pda.html
InputName=bookmarks_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_pda.html
InputName=bookmarks_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_pda.html
InputName=bookmarks_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_pda.html
InputName=bookmarks_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_pda.html
InputName=bookmarks_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_pda.html
InputName=bookmarks_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bookmarks_pda.html
InputName=bookmarks_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\bottom_border.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bottom_border.png
InputName=bottom_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bottom_border.png
InputName=bottom_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bottom_border.png
InputName=bottom_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bottom_border.png
InputName=bottom_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bottom_border.png
InputName=bottom_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bottom_border.png
InputName=bottom_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bottom_border.png
InputName=bottom_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bottom_border.png
InputName=bottom_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\bottom_border.png
InputName=bottom_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\brushedmetal.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal.png
InputName=brushedmetal

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal.png
InputName=brushedmetal

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal.png
InputName=brushedmetal

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal.png
InputName=brushedmetal

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal.png
InputName=brushedmetal

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal.png
InputName=brushedmetal

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal.png
InputName=brushedmetal

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal.png
InputName=brushedmetal

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal.png
InputName=brushedmetal

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\brushedmetal_side.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal_side.png
InputName=brushedmetal_side

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal_side.png
InputName=brushedmetal_side

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal_side.png
InputName=brushedmetal_side

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal_side.png
InputName=brushedmetal_side

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal_side.png
InputName=brushedmetal_side

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal_side.png
InputName=brushedmetal_side

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal_side.png
InputName=brushedmetal_side

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal_side.png
InputName=brushedmetal_side

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\brushedmetal_side.png
InputName=brushedmetal_side

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\chrome.html

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome.html
InputName=chrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome.html
InputName=chrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome.html
InputName=chrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome.html
InputName=chrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome.html
InputName=chrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome.html
InputName=chrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome.html
InputName=chrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome.html
InputName=chrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome.html
InputName=chrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\chrome_pda.html

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome_pda.html
InputName=chrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome_pda.html
InputName=chrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome_pda.html
InputName=chrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome_pda.html
InputName=chrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome_pda.html
InputName=chrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome_pda.html
InputName=chrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome_pda.html
InputName=chrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome_pda.html
InputName=chrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\chrome_pda.html
InputName=chrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\close_default.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_default.png
InputName=close_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_default.png
InputName=close_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_default.png
InputName=close_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_default.png
InputName=close_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_default.png
InputName=close_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_default.png
InputName=close_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_default.png
InputName=close_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_default.png
InputName=close_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_default.png
InputName=close_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\close_over.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_over.png
InputName=close_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_over.png
InputName=close_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_over.png
InputName=close_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_over.png
InputName=close_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_over.png
InputName=close_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_over.png
InputName=close_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_over.png
InputName=close_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_over.png
InputName=close_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\close_over.png
InputName=close_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\disabled_gradiant.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\disabled_gradiant.png
InputName=disabled_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\disabled_gradiant.png
InputName=disabled_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\disabled_gradiant.png
InputName=disabled_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\disabled_gradiant.png
InputName=disabled_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\disabled_gradiant.png
InputName=disabled_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\disabled_gradiant.png
InputName=disabled_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\disabled_gradiant.png
InputName=disabled_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\disabled_gradiant.png
InputName=disabled_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\disabled_gradiant.png
InputName=disabled_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\document.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\document.png
InputName=document

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\document.png
InputName=document

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\document.png
InputName=document

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\document.png
InputName=document

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\document.png
InputName=document

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\document.png
InputName=document

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\document.png
InputName=document

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\document.png
InputName=document

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\document.png
InputName=document

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\Images\ebsnetlogo.jpg

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\ebsnetlogo.jpg
InputName=ebsnetlogo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\ebsnetlogo.jpg
InputName=ebsnetlogo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\ebsnetlogo.jpg
InputName=ebsnetlogo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\ebsnetlogo.jpg
InputName=ebsnetlogo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\ebsnetlogo.jpg
InputName=ebsnetlogo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\ebsnetlogo.jpg
InputName=ebsnetlogo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\ebsnetlogo.jpg
InputName=ebsnetlogo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\ebsnetlogo.jpg
InputName=ebsnetlogo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\ebsnetlogo.jpg
InputName=ebsnetlogo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\forward_default.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_default.png
InputName=forward_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_default.png
InputName=forward_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_default.png
InputName=forward_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_default.png
InputName=forward_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_default.png
InputName=forward_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_default.png
InputName=forward_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_default.png
InputName=forward_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_default.png
InputName=forward_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_default.png
InputName=forward_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\forward_disable.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_disable.png
InputName=forward_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_disable.png
InputName=forward_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_disable.png
InputName=forward_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_disable.png
InputName=forward_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_disable.png
InputName=forward_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_disable.png
InputName=forward_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_disable.png
InputName=forward_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_disable.png
InputName=forward_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_disable.png
InputName=forward_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\forward_halo.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_halo.png
InputName=forward_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_halo.png
InputName=forward_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_halo.png
InputName=forward_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_halo.png
InputName=forward_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_halo.png
InputName=forward_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_halo.png
InputName=forward_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_halo.png
InputName=forward_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_halo.png
InputName=forward_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_halo.png
InputName=forward_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\forward_over.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over.png
InputName=forward_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over.png
InputName=forward_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over.png
InputName=forward_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over.png
InputName=forward_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over.png
InputName=forward_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over.png
InputName=forward_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over.png
InputName=forward_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over.png
InputName=forward_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over.png
InputName=forward_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\forward_over_halo.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over_halo.png
InputName=forward_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over_halo.png
InputName=forward_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over_halo.png
InputName=forward_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over_halo.png
InputName=forward_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over_halo.png
InputName=forward_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over_halo.png
InputName=forward_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over_halo.png
InputName=forward_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over_halo.png
InputName=forward_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\forward_over_halo.png
InputName=forward_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\home_default.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_default.png
InputName=home_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_default.png
InputName=home_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_default.png
InputName=home_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_default.png
InputName=home_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_default.png
InputName=home_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_default.png
InputName=home_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_default.png
InputName=home_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_default.png
InputName=home_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_default.png
InputName=home_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\home_halo.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_halo.png
InputName=home_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_halo.png
InputName=home_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_halo.png
InputName=home_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_halo.png
InputName=home_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_halo.png
InputName=home_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_halo.png
InputName=home_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_halo.png
InputName=home_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_halo.png
InputName=home_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_halo.png
InputName=home_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\home_over.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over.png
InputName=home_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over.png
InputName=home_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over.png
InputName=home_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over.png
InputName=home_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over.png
InputName=home_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over.png
InputName=home_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over.png
InputName=home_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over.png
InputName=home_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over.png
InputName=home_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\home_over_halo.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over_halo.png
InputName=home_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over_halo.png
InputName=home_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over_halo.png
InputName=home_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over_halo.png
InputName=home_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over_halo.png
InputName=home_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over_halo.png
InputName=home_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over_halo.png
InputName=home_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over_halo.png
InputName=home_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\home_over_halo.png
InputName=home_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\images\icon64x64.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\images\icon64x64.png
InputName=icon64x64

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\images\icon64x64.png
InputName=icon64x64

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\images\icon64x64.png
InputName=icon64x64

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\images\icon64x64.png
InputName=icon64x64

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\images\icon64x64.png
InputName=icon64x64

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\images\icon64x64.png
InputName=icon64x64

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\images\icon64x64.png
InputName=icon64x64

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\images\icon64x64.png
InputName=icon64x64

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\images\icon64x64.png
InputName=icon64x64

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\loadChrome.html

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome.html
InputName=loadChrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome.html
InputName=loadChrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome.html
InputName=loadChrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome.html
InputName=loadChrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome.html
InputName=loadChrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome.html
InputName=loadChrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome.html
InputName=loadChrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome.html
InputName=loadChrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome.html
InputName=loadChrome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\loadChrome_pda.html

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome_pda.html
InputName=loadChrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome_pda.html
InputName=loadChrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome_pda.html
InputName=loadChrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome_pda.html
InputName=loadChrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome_pda.html
InputName=loadChrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome_pda.html
InputName=loadChrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome_pda.html
InputName=loadChrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome_pda.html
InputName=loadChrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\loadChrome_pda.html
InputName=loadChrome_pda

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\new_page.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page.png
InputName=new_page

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page.png
InputName=new_page

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page.png
InputName=new_page

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page.png
InputName=new_page

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page.png
InputName=new_page

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page.png
InputName=new_page

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page.png
InputName=new_page

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page.png
InputName=new_page

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page.png
InputName=new_page

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\new_page_over.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page_over.png
InputName=new_page_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page_over.png
InputName=new_page_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page_over.png
InputName=new_page_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page_over.png
InputName=new_page_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page_over.png
InputName=new_page_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page_over.png
InputName=new_page_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page_over.png
InputName=new_page_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page_over.png
InputName=new_page_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\new_page_over.png
InputName=new_page_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\offline.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline.png
InputName=offline

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline.png
InputName=offline

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline.png
InputName=offline

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline.png
InputName=offline

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline.png
InputName=offline

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline.png
InputName=offline

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline.png
InputName=offline

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline.png
InputName=offline

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline.png
InputName=offline

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\offline_disabled.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_disabled.png
InputName=offline_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_disabled.png
InputName=offline_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_disabled.png
InputName=offline_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_disabled.png
InputName=offline_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_disabled.png
InputName=offline_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_disabled.png
InputName=offline_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_disabled.png
InputName=offline_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_disabled.png
InputName=offline_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_disabled.png
InputName=offline_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\offline_toggle.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_toggle.png
InputName=offline_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_toggle.png
InputName=offline_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_toggle.png
InputName=offline_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_toggle.png
InputName=offline_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_toggle.png
InputName=offline_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_toggle.png
InputName=offline_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_toggle.png
InputName=offline_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_toggle.png
InputName=offline_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\offline_toggle.png
InputName=offline_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\online.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online.png
InputName=online

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online.png
InputName=online

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online.png
InputName=online

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online.png
InputName=online

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online.png
InputName=online

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online.png
InputName=online

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online.png
InputName=online

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online.png
InputName=online

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online.png
InputName=online

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\online_disabled.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_disabled.png
InputName=online_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_disabled.png
InputName=online_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_disabled.png
InputName=online_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_disabled.png
InputName=online_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_disabled.png
InputName=online_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_disabled.png
InputName=online_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_disabled.png
InputName=online_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_disabled.png
InputName=online_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_disabled.png
InputName=online_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\online_toggle.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_toggle.png
InputName=online_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_toggle.png
InputName=online_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_toggle.png
InputName=online_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_toggle.png
InputName=online_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_toggle.png
InputName=online_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_toggle.png
InputName=online_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_toggle.png
InputName=online_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_toggle.png
InputName=online_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\online_toggle.png
InputName=online_toggle

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\Images\prompt.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\prompt.png
InputName=prompt

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\prompt.png
InputName=prompt

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\prompt.png
InputName=prompt

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\prompt.png
InputName=prompt

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\prompt.png
InputName=prompt

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\prompt.png
InputName=prompt

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\prompt.png
InputName=prompt

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\prompt.png
InputName=prompt

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\prompt.png
InputName=prompt

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\prompt_small.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\prompt_small.png
InputName=prompt_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\prompt_small.png
InputName=prompt_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\prompt_small.png
InputName=prompt_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\prompt_small.png
InputName=prompt_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\prompt_small.png
InputName=prompt_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\prompt_small.png
InputName=prompt_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\prompt_small.png
InputName=prompt_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\prompt_small.png
InputName=prompt_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\prompt_small.png
InputName=prompt_small

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\refresh_default.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_default.png
InputName=refresh_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_default.png
InputName=refresh_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_default.png
InputName=refresh_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_default.png
InputName=refresh_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_default.png
InputName=refresh_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_default.png
InputName=refresh_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_default.png
InputName=refresh_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_default.png
InputName=refresh_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_default.png
InputName=refresh_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\refresh_halo.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_halo.png
InputName=refresh_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_halo.png
InputName=refresh_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_halo.png
InputName=refresh_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_halo.png
InputName=refresh_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_halo.png
InputName=refresh_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_halo.png
InputName=refresh_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_halo.png
InputName=refresh_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_halo.png
InputName=refresh_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_halo.png
InputName=refresh_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\refresh_over.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over.png
InputName=refresh_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over.png
InputName=refresh_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over.png
InputName=refresh_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over.png
InputName=refresh_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over.png
InputName=refresh_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over.png
InputName=refresh_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over.png
InputName=refresh_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over.png
InputName=refresh_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over.png
InputName=refresh_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\refresh_over_halo.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over_halo.png
InputName=refresh_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over_halo.png
InputName=refresh_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over_halo.png
InputName=refresh_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over_halo.png
InputName=refresh_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over_halo.png
InputName=refresh_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over_halo.png
InputName=refresh_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over_halo.png
InputName=refresh_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over_halo.png
InputName=refresh_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\refresh_over_halo.png
InputName=refresh_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\savetoarchive.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive.png
InputName=savetoarchive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive.png
InputName=savetoarchive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive.png
InputName=savetoarchive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive.png
InputName=savetoarchive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive.png
InputName=savetoarchive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive.png
InputName=savetoarchive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive.png
InputName=savetoarchive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive.png
InputName=savetoarchive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive.png
InputName=savetoarchive

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\savetoarchive_disabled.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive_disabled.png
InputName=savetoarchive_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive_disabled.png
InputName=savetoarchive_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive_disabled.png
InputName=savetoarchive_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive_disabled.png
InputName=savetoarchive_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive_disabled.png
InputName=savetoarchive_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive_disabled.png
InputName=savetoarchive_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive_disabled.png
InputName=savetoarchive_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive_disabled.png
InputName=savetoarchive_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\savetoarchive_disabled.png
InputName=savetoarchive_disabled

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\search_bottom.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_bottom.png
InputName=search_bottom

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_bottom.png
InputName=search_bottom

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_bottom.png
InputName=search_bottom

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_bottom.png
InputName=search_bottom

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_bottom.png
InputName=search_bottom

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_bottom.png
InputName=search_bottom

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_bottom.png
InputName=search_bottom

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_bottom.png
InputName=search_bottom

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_bottom.png
InputName=search_bottom

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\search_left.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left.png
InputName=search_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left.png
InputName=search_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left.png
InputName=search_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left.png
InputName=search_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left.png
InputName=search_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left.png
InputName=search_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left.png
InputName=search_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left.png
InputName=search_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left.png
InputName=search_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\search_left_menu.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left_menu.png
InputName=search_left_menu

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left_menu.png
InputName=search_left_menu

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left_menu.png
InputName=search_left_menu

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left_menu.png
InputName=search_left_menu

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left_menu.png
InputName=search_left_menu

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left_menu.png
InputName=search_left_menu

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left_menu.png
InputName=search_left_menu

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left_menu.png
InputName=search_left_menu

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_left_menu.png
InputName=search_left_menu

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\search_right.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_right.png
InputName=search_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_right.png
InputName=search_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_right.png
InputName=search_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_right.png
InputName=search_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_right.png
InputName=search_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_right.png
InputName=search_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_right.png
InputName=search_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_right.png
InputName=search_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_right.png
InputName=search_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\search_top.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_top.png
InputName=search_top

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_top.png
InputName=search_top

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_top.png
InputName=search_top

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_top.png
InputName=search_top

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_top.png
InputName=search_top

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_top.png
InputName=search_top

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_top.png
InputName=search_top

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_top.png
InputName=search_top

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\search_top.png
InputName=search_top

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\select_gradiant.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\select_gradiant.png
InputName=select_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\select_gradiant.png
InputName=select_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\select_gradiant.png
InputName=select_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\select_gradiant.png
InputName=select_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\select_gradiant.png
InputName=select_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\select_gradiant.png
InputName=select_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\select_gradiant.png
InputName=select_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\select_gradiant.png
InputName=select_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\select_gradiant.png
InputName=select_gradiant

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\Images\slogan.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\slogan.png
InputName=slogan

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\slogan.png
InputName=slogan

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\slogan.png
InputName=slogan

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\slogan.png
InputName=slogan

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\slogan.png
InputName=slogan

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\slogan.png
InputName=slogan

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\slogan.png
InputName=slogan

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\slogan.png
InputName=slogan

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\Images\slogan.png
InputName=slogan

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\small_back.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_back.png
InputName=small_back

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_back.png
InputName=small_back

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_back.png
InputName=small_back

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_back.png
InputName=small_back

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_back.png
InputName=small_back

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_back.png
InputName=small_back

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_back.png
InputName=small_back

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_back.png
InputName=small_back

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_back.png
InputName=small_back

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\small_forward.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_forward.png
InputName=small_forward

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_forward.png
InputName=small_forward

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_forward.png
InputName=small_forward

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_forward.png
InputName=small_forward

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_forward.png
InputName=small_forward

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_forward.png
InputName=small_forward

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_forward.png
InputName=small_forward

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_forward.png
InputName=small_forward

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_forward.png
InputName=small_forward

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\small_refresh.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_refresh.png
InputName=small_refresh

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_refresh.png
InputName=small_refresh

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_refresh.png
InputName=small_refresh

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_refresh.png
InputName=small_refresh

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_refresh.png
InputName=small_refresh

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_refresh.png
InputName=small_refresh

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_refresh.png
InputName=small_refresh

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_refresh.png
InputName=small_refresh

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\small_refresh.png
InputName=small_refresh

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\smallGlobe.gif

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe.gif
InputName=smallGlobe

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe.gif
InputName=smallGlobe

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe.gif
InputName=smallGlobe

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe.gif
InputName=smallGlobe

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe.gif
InputName=smallGlobe

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe.gif
InputName=smallGlobe

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe.gif
InputName=smallGlobe

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe.gif
InputName=smallGlobe

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe.gif
InputName=smallGlobe

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\smallGlobe_gray.gif

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe_gray.gif
InputName=smallGlobe_gray

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe_gray.gif
InputName=smallGlobe_gray

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe_gray.gif
InputName=smallGlobe_gray

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe_gray.gif
InputName=smallGlobe_gray

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe_gray.gif
InputName=smallGlobe_gray

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe_gray.gif
InputName=smallGlobe_gray

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe_gray.gif
InputName=smallGlobe_gray

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe_gray.gif
InputName=smallGlobe_gray

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\smallGlobe_gray.gif
InputName=smallGlobe_gray

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\stop_default.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_default.png
InputName=stop_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_default.png
InputName=stop_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_default.png
InputName=stop_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_default.png
InputName=stop_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_default.png
InputName=stop_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_default.png
InputName=stop_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_default.png
InputName=stop_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_default.png
InputName=stop_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_default.png
InputName=stop_default

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\stop_disable.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_disable.png
InputName=stop_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_disable.png
InputName=stop_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_disable.png
InputName=stop_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_disable.png
InputName=stop_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_disable.png
InputName=stop_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_disable.png
InputName=stop_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_disable.png
InputName=stop_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_disable.png
InputName=stop_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_disable.png
InputName=stop_disable

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\stop_halo.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_halo.png
InputName=stop_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_halo.png
InputName=stop_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_halo.png
InputName=stop_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_halo.png
InputName=stop_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_halo.png
InputName=stop_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_halo.png
InputName=stop_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_halo.png
InputName=stop_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_halo.png
InputName=stop_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_halo.png
InputName=stop_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\stop_over.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over.png
InputName=stop_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over.png
InputName=stop_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over.png
InputName=stop_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over.png
InputName=stop_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over.png
InputName=stop_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over.png
InputName=stop_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over.png
InputName=stop_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over.png
InputName=stop_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over.png
InputName=stop_over

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\stop_over_halo.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over_halo.png
InputName=stop_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over_halo.png
InputName=stop_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over_halo.png
InputName=stop_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over_halo.png
InputName=stop_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over_halo.png
InputName=stop_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over_halo.png
InputName=stop_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over_halo.png
InputName=stop_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over_halo.png
InputName=stop_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stop_over_halo.png
InputName=stop_over_halo

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\stripes.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stripes.png
InputName=stripes

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stripes.png
InputName=stripes

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stripes.png
InputName=stripes

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stripes.png
InputName=stripes

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stripes.png
InputName=stripes

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stripes.png
InputName=stripes

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stripes.png
InputName=stripes

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stripes.png
InputName=stripes

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\stripes.png
InputName=stripes

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\top_border.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\top_border.png
InputName=top_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\top_border.png
InputName=top_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\top_border.png
InputName=top_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\top_border.png
InputName=top_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\top_border.png
InputName=top_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\top_border.png
InputName=top_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\top_border.png
InputName=top_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\top_border.png
InputName=top_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\top_border.png
InputName=top_border

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\url_left.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_left.png
InputName=url_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_left.png
InputName=url_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_left.png
InputName=url_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_left.png
InputName=url_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_left.png
InputName=url_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_left.png
InputName=url_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_left.png
InputName=url_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_left.png
InputName=url_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_left.png
InputName=url_left

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\chrome\url_right.png

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_right.png
InputName=url_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_right.png
InputName=url_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_right.png
InputName=url_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_right.png
InputName=url_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_right.png
InputName=url_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_right.png
InputName=url_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_right.png
InputName=url_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_right.png
InputName=url_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\chrome\url_right.png
InputName=url_right

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\html\welcome.html

!IF  "$(CFG)" == "webc - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\welcome.html
InputName=welcome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\welcome.html
InputName=welcome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\welcome.html
InputName=welcome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\welcome.html
InputName=welcome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\welcome.html
InputName=welcome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\welcome.html
InputName=welcome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Release JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\welcome.html
InputName=welcome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\welcome.html
InputName=welcome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "webc - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\html\welcome.html
InputName=welcome

BuildCmds= \
	..\..\..\bin\bintoc $(InputPath) $(InputName) \
	move "$(InputName).c" "..\..\..\html\source" \
	move "$(InputName).h" "..\..\..\html\include" \
	

"..\..\..\html\source\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\html\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\source\24to24.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\24to32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\24to555.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\24to565.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\24to8.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\24togs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\8to24.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\8to32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\8to555.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\8to565.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\8to8.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\8togs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\bmpdec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\source\buffer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\source\cookie.c
# End Source File
# Begin Source File

SOURCE=..\..\..\source\css.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dcontent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\delement.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dflow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dharea.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhbody.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhcheck.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhedbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhedit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhedstr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhelem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhframe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhframeset.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhhr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhibutton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhiframe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhimg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhli.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhpasswd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhradio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhselect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhstring.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhtable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhtd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dhtumirror.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\dmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\fileext.c
# End Source File
# Begin Source File

SOURCE=..\..\..\source\filetype.c
# End Source File
# Begin Source File

SOURCE=..\..\..\source\format.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\formatctx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\gs8togs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\haddress.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hanchor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\harea.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hashtbl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hbig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hblquote.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hbody.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hbold.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hbr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hcbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hcenter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hchkbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hcite.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hcode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hdd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hdel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hdiv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hdl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hdstream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hdt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hedit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\helement.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hemphsz.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hflow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hfont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hframe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hframset.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hheading.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hhidden.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hhr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hibutton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hiframe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\himage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hinput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hins.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hitalic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hkbd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hlabel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hli.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hlist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hnobr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hoption.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hpara.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hpasswd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hpre.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hradio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hreset.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hscript.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hselect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hsmall.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hspacer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hspan.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hstrike.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hstring.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hstrong.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hsubmit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\htable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\htd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\html.c
# End Source File
# Begin Source File

SOURCE=..\..\..\source\htmlbld.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\htmlbrow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\htmldoc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\htmldom.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\htmlfind.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\htmlsdk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\htr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\httpc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\source\htxtarea.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\hundline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\imgconv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jevent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhanchor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhbody.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhcoll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhdoc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhelem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhflow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhframe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhfrset.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhiframe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhimg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhinput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhistory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhoption.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhselect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhtable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhtarea.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhtblrow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhtcell.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhtitle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jhutils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jloc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jnav.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jrule.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jsobjref.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jssheet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jstyle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\jwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\keycatch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\margins.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\objref.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\protfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\prothssl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\prothttp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\protoffl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\ref.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\refcount.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\sramdisk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\strbld.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\strtable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\textstyle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\tuenum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\urldesc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\util.c
# End Source File
# Begin Source File

SOURCE=..\..\..\source\vector.c
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wcache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wcconst.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wcfilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wcstream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webbmp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webcbig5.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webcbmp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webcdefault.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webceuckr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webcfile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webcmem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webcsjis.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webcsock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webcstr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webctmo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webcutf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webfonts.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webgif.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webgraph.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webimage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webjpeg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webload.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webmng.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webpng.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webrect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webstr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\webvfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wgbmp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wjsbrowser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wjscript.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wjsdoc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wtbutton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wtcheck.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wtdrop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wtedbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wtedstr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wtlist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wtpasswd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wtradio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wtscroll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\wtslider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\source\xml.c
# End Source File
# Begin Source File

SOURCE=..\..\..\source\xmlutil.c
# End Source File
# Begin Source File

SOURCE=..\..\..\source\zdelem.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\webcfg.h
# End Source File
# End Group
# End Target
# End Project
