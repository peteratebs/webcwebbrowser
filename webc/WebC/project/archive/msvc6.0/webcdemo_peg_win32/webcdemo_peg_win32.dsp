# Microsoft Developer Studio Project File - Name="webcdemo_peg_win32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=webcdemo_peg_win32 - Win32 Debug JavaScript Unicode 8bpp
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "webcdemo_peg_win32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "webcdemo_peg_win32.mak" CFG="webcdemo_peg_win32 - Win32 Debug JavaScript Unicode 8bpp"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "webcdemo_peg_win32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "webcdemo_peg_win32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "webcdemo_peg_win32 - Win32 Debug JavaScript" (based on "Win32 (x86) Application")
!MESSAGE "webcdemo_peg_win32 - Win32 Debug JavaScript Unicode" (based on "Win32 (x86) Application")
!MESSAGE "webcdemo_peg_win32 - Win32 Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "webcdemo_peg_win32 - Win32 Release JavaScript" (based on "Win32 (x86) Application")
!MESSAGE "webcdemo_peg_win32 - Win32 Release JavaScript Unicode" (based on "Win32 (x86) Application")
!MESSAGE "webcdemo_peg_win32 - Win32 Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "webcdemo_peg_win32 - Win32 Debug JavaScript Unicode 8bpp" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "webcdemo_peg_win32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\gui\peg\include" /D "NDEBUG" /D WEBC_SUPPORT_JSCRIPT=0 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib ..\..\..\lib\openssl\openssl32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "webcdemo_peg_win32 - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\gui\peg\include" /D WEBC_SUPPORT_JSCRIPT=0 /D "_DEBUG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib ..\..\..\lib\openssl\openssl32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "webcdemo_peg_win32 - Win32 Debug JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug JavaScript"
# PROP BASE Intermediate_Dir "Debug JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug JavaScript"
# PROP Intermediate_Dir "Debug JavaScript"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\gui\peg\include" /I "..\..\..\lib\mozilla" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D WEBC_SUPPORT_JSCRIPT=1 /D "_DEBUG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib ..\..\..\lib\openssl\openssl32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "webcdemo_peg_win32 - Win32 Debug JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug JavaScript Unicode"
# PROP BASE Intermediate_Dir "Debug JavaScript Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug JavaScript Unicode"
# PROP Intermediate_Dir "Debug JavaScript Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\lib\mozilla" /I "..\..\..\gui\peg\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D WEBC_SUPPORT_JSCRIPT=1 /D "_DEBUG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /D WEBC_SUPPORT_UNICODE=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /D WEBC_SUPPORT_HTTPS=1 /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\..\lib\openssl\openssl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "webcdemo_peg_win32 - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug Unicode"
# PROP BASE Intermediate_Dir "Debug Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug Unicode"
# PROP Intermediate_Dir "Debug Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\gui\peg\include" /D WEBC_SUPPORT_JSCRIPT=0 /D "_DEBUG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /D WEBC_SUPPORT_UNICODE=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib ..\..\..\lib\openssl\openssl32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "webcdemo_peg_win32 - Win32 Release JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release JavaScript"
# PROP BASE Intermediate_Dir "Release JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release JavaScript"
# PROP Intermediate_Dir "Release JavaScript"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\lib\mozilla" /I "..\..\..\gui\peg\include" /D "NDEBUG" /D WEBC_SUPPORT_JSCRIPT=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib ..\..\..\lib\openssl\openssl32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "webcdemo_peg_win32 - Win32 Release JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release JavaScript Unicode"
# PROP BASE Intermediate_Dir "Release JavaScript Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release JavaScript Unicode"
# PROP Intermediate_Dir "Release JavaScript Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\gui\peg\include" /I "..\..\..\lib\mozilla" /D "NDEBUG" /D WEBC_SUPPORT_JSCRIPT=1 /D WEBC_SUPPORT_UNICODE=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib ..\..\..\lib\openssl\openssl32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "webcdemo_peg_win32 - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release Unicode"
# PROP BASE Intermediate_Dir "Release Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release Unicode"
# PROP Intermediate_Dir "Release Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /I "..\..\..\gui\peg\include" /D "NDEBUG" /D WEBC_SUPPORT_JSCRIPT=0 /D WEBC_SUPPORT_UNICODE=1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WEBC__" /D PEG_NUM_COLORS=65535 /D PSCREEN=<l16scrn.hpp> /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib ..\..\..\lib\openssl\openssl32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "webcdemo_peg_win32 - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "webcdemo_peg_win32___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Intermediate_Dir "webcdemo_peg_win32___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "webcdemo_peg_win32___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Intermediate_Dir "webcdemo_peg_win32___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\lib\mozilla" /I "..\..\..\gui\peg\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D WEBC_SUPPORT_JSCRIPT=1 /D "_DEBUG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WEBC__" /D WEBC_SUPPORT_UNICODE=1 /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\lib\mozilla" /I "..\..\..\gui\peg\include" /I "..\..\..\..\..\rtplatform\v0.4\include\ms32" /I "..\..\..\..\..\rtplatform\v0.4\include" /I "..\..\..\..\..\peg\include" /I "..\..\..\..\..\peg\include\scrndrv" /D WEBC_SUPPORT_JSCRIPT=1 /D "_DEBUG" /D "WEBC_DEBUG" /D "RTP_TRACK_LOCAL_MEMORY" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WEBC__" /D WEBC_SUPPORT_UNICODE=1 /D "PEG_UNICODE" /D PEG_NUM_COLORS=256 /D PSCREEN=<l8scrn.hpp> /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib ..\..\..\lib\openssl\openssl32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "webcdemo_peg_win32 - Win32 Release"
# Name "webcdemo_peg_win32 - Win32 Debug"
# Name "webcdemo_peg_win32 - Win32 Debug JavaScript"
# Name "webcdemo_peg_win32 - Win32 Debug JavaScript Unicode"
# Name "webcdemo_peg_win32 - Win32 Debug Unicode"
# Name "webcdemo_peg_win32 - Win32 Release JavaScript"
# Name "webcdemo_peg_win32 - Win32 Release JavaScript Unicode"
# Name "webcdemo_peg_win32 - Win32 Release Unicode"
# Name "webcdemo_peg_win32 - Win32 Debug JavaScript Unicode 8bpp"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\webcdemo.cpp
# End Source File
# Begin Source File

SOURCE=.\windemo.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon.rc
# End Source File
# End Group
# End Target
# End Project
