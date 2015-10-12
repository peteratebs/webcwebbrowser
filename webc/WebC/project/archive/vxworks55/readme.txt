
wind river, windML, readme file.

File contents:

readme.txt   - This file
main.cpp     - Main entry point
webcdemo.hpp - Required by main.cpp
makefile,mkgnu.inc,rulesgnu.inc - Make file for webC, vxworks, windML.. for vxsim.. modify for other targets
..\simpc_RGB565_800x600_windml.o - vxsim image to link in

Windml GUI abstractions is in:
..\..\gui\wndml


Build instructions:

1. Copy ..\simpc_RGB565_800x600_windml.o .
2. type make

Running vxsim

1. Set tornado env vars and start license manager
\Tornado2.2\host\x86-win32\bin\torvars.bat
\Tornado2.2\license\runlmgr
2. Start tornado from windows explorer
3. Start simulator..
4. Start shell
5. select project\download and download webcdemo.out
6. type webc_vxw_entry from the shell

To restart.. kill vxsim and go to step 3.
