;
; ISRSHELS.S  (IAR version)                                 Version 3.7.1
;
; Define shells here for all isr's. These use ENTER_ISR and EXIT_ISR
; macros and call the C function to do the work.
;
; Copyright (c) 2002-2007 Micro Digital Associates, Inc.
;               All Rights Reserved.
;
; Author: David Moore
;
;**************************************************************************

   #include  "irq_iar.inc"

   PUBLIC   tick
   EXTERN   tickC

   RSEG     CODE:CODE:NOROOT(2)
   CODE32


tick:
   ENTER_ISR       ; Don't breakpoint on ENTER_ISR or debugger may fail.
   #if 1 ; Thumb arch
   LDR      r0, =tickC
   MOV      lr, pc
   BX       r0
   #else ; ARM-only arch
   BL       tickC
   #endif
   EXIT_ISR


; Add other isr shells here.


;**************************************************************************
; Notes
;
; 1. Do not set a breakpoint on ENTER_ISR or the debugger may corrupt
;    execution of the application. It is ok to break inside ENTER_ISR
;    after the point where it does the mode switch. See the DEBUG NOTE
;    in irq_iar.inc for more information.
;

   END
