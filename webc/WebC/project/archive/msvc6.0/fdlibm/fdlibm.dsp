# Microsoft Developer Studio Project File - Name="fdlibm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=fdlibm - Win32 Debug JavaScript Unicode 8bpp
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fdlibm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fdlibm.mak" CFG="fdlibm - Win32 Debug JavaScript Unicode 8bpp"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fdlibm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fdlibm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "fdlibm - Win32 Debug JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "fdlibm - Win32 Debug JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "fdlibm - Win32 Release JavaScript" (based on "Win32 (x86) Static Library")
!MESSAGE "fdlibm - Win32 Release JavaScript Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "fdlibm - Win32 Debug JavaScript Unicode 8bpp" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fdlibm - Win32 Release"

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

!ELSEIF  "$(CFG)" == "fdlibm - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "fdlibm - Win32 Debug JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "fdlibm___Win32_Debug_JavaScript"
# PROP BASE Intermediate_Dir "fdlibm___Win32_Debug_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "fdlibm___Win32_Debug_JavaScript"
# PROP Intermediate_Dir "fdlibm___Win32_Debug_JavaScript"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "fdlibm - Win32 Debug JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "fdlibm___Win32_Debug_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "fdlibm___Win32_Debug_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "fdlibm___Win32_Debug_JavaScript_Unicode"
# PROP Intermediate_Dir "fdlibm___Win32_Debug_JavaScript_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "fdlibm - Win32 Release JavaScript"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "fdlibm___Win32_Release_JavaScript"
# PROP BASE Intermediate_Dir "fdlibm___Win32_Release_JavaScript"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "fdlibm___Win32_Release_JavaScript"
# PROP Intermediate_Dir "fdlibm___Win32_Release_JavaScript"
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

!ELSEIF  "$(CFG)" == "fdlibm - Win32 Release JavaScript Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "fdlibm___Win32_Release_JavaScript_Unicode"
# PROP BASE Intermediate_Dir "fdlibm___Win32_Release_JavaScript_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "fdlibm___Win32_Release_JavaScript_Unicode"
# PROP Intermediate_Dir "fdlibm___Win32_Release_JavaScript_Unicode"
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

!ELSEIF  "$(CFG)" == "fdlibm - Win32 Debug JavaScript Unicode 8bpp"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "fdlibm___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Intermediate_Dir "fdlibm___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "fdlibm___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Intermediate_Dir "fdlibm___Win32_Debug_JavaScript_Unicode_8bpp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
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

# Name "fdlibm - Win32 Release"
# Name "fdlibm - Win32 Debug"
# Name "fdlibm - Win32 Debug JavaScript"
# Name "fdlibm - Win32 Debug JavaScript Unicode"
# Name "fdlibm - Win32 Release JavaScript"
# Name "fdlibm - Win32 Release JavaScript Unicode"
# Name "fdlibm - Win32 Debug JavaScript Unicode 8bpp"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_acos.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_acosh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_asin.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_atan2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_atanh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_cosh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_exp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_fmod.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_gamma.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_gamma_r.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_hypot.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_j0.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_j1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_jn.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_lgamma.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_lgamma_r.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_log.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_log10.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_pow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_rem_pio2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_remainder.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_scalb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_sinh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\e_sqrt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\k_cos.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\k_rem_pio2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\k_sin.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\k_standard.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\k_tan.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_asinh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_atan.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_cbrt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_ceil.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_copysign.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_cos.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_erf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_expm1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_fabs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_finite.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_floor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_frexp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_ilogb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_isnan.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_ldexp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_lib_version.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_log1p.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_logb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_matherr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_modf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_nextafter.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_rint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_scalbn.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_signgam.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_significand.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_sin.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_tan.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\s_tanh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_acos.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_acosh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_asin.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_atan2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_atanh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_cosh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_exp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_fmod.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_gamma.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_gamma_r.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_hypot.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_j0.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_j1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_jn.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_lgamma.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_lgamma_r.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_log.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_log10.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_pow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_remainder.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_scalb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_sinh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fdlibm\w_sqrt.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
