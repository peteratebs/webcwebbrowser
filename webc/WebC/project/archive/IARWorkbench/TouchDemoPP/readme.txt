########################################################################
#
#                           TouchDemoPP.eww
#
# $Revision: 26684 $
#
########################################################################

DESCRIPTION
===========
  This example project shows how to use the IAR Embedded Workbench for ARM
 to develop code for the IAR-LPC-2478 board.
 The example is based on the IAR PowerPac realtime operating system.
 It shows basic use of I/O, timer, interrupt and LCD controllers
 and LCD touch screen.
 

  The LCD hardware cursor moves as the LCD touching.
  
COMPATIBILITY
=============

   The Touch demo project is compatible with the IAR-LPC-2478
  evaluation board. By default, the project is configured to use the
  J-Link JTAG interface.

CONFIGURATION
=============

   After power-up the controller get clock from internal RC oscillator that
  is unstable and may fail with J-Link auto detect, therefore adaptive clocking
  should always be used. The adaptive clock can be select from menu:
  Project->Options..., section Debugger->J-Link/J-Trace  JTAG Speed - Adaptive.

   The Touch demo application is downloaded to the iFlash.
  
  Jumpers:
    EXT/JLINK  - depending of power source
    ISP_E      - unfilled
    RST_E      - unfilled
    BDS_E      - unfilled
    C/SC       - SC
  
GETTING STARTED
===============

  1) Start the IAR Embedded Workbench for ARM.

  2) Select File->Open->Workspace...
     Open the following workspace:

      <installation-root>\arm\examples\NXP\
     LPC24xx\IAR-LPC-2478\TouchDemoPP\TouchDemoPP.eww

  3) Run the program.
