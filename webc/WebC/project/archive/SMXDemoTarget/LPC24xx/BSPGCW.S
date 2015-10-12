/*
* BSPGCW.S                                                  Version 3.7.1
*
* Board Support Package API Assembly Routines for these boards which use
* NXP (Philips) LPC24xx processor. All are similar so just use conditionals
* here for differences.
* Currently supported:  LPC2468, LPC2478.
*
* I originally tried to implement these as inline assembly (in C), but
* the IAR compiler's inline assembler is weak. See the comment in the
* IAR section of XARM.H for details.
*
* Copyright (c) 2005-2008 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author: David Moore
*
***********************************************************************/

  .section .text, "ax"
  .arm
  .align 0

        .global  bsplDelayLoop


bsplDelayLoop:
        /* When I attempted to write bspDelayUsec() in inline assembly
         * in C, IAR gave errors for almost every line! Doing the number
         * of loops calculation in C saves having to create a bsp.inc
         * file and keep in sync with bsp.h.
         *
         * r0 = num loops since 1st par is passed in r0
         */
loop_top:
        subs    r0, r0, #1              // book says 1 clock
        bne     loop_top                // book says 3 clocks
        bx      lr                      // return (use mov pc, lr for non-Thumb arch)

        .end

