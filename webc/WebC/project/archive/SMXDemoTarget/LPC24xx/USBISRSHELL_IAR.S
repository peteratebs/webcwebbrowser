;
; USBISRSHELL.S  (IAR version)                              Version 3.7.1
;
; ISR shells for USB of LPC24XX. See PROTOARM\ISRSHELS_IAR.S for additional
; discussion.
;
; Copyright (c) 2007 Micro Digital Inc.
; All rights reserved. www.smxrtos.com
;
; This software is confidential and proprietary to Micro Digital Inc.
; It has been furnished under a license and may be used, copied, or
; disclosed only in accordance with the terms of that license and with
; the inclusion of this header. No title to nor ownership of this
; software is hereby transferred.
;
; Author: Yingbo Hu
;
;*****************************************************************************

            #include    "irq_iar.inc"

            RSEG        CODE:CODE:NOROOT(2)
            CODE32

            EXTERN      LPC24XXUSBInterruptHandlerCore
            PUBLIC      LPC24XXUSBInterruptHandler
LPC24XXUSBInterruptHandler:
            ENTER_ISR
            #if 1 ; Thumb arch
            LDR         r0, =LPC24XXUSBInterruptHandlerCore
            MOV         lr, pc
            BX          r0
            #else ; ARM-only arch
            BL          LPC24XXUSBInterruptHandlerCore
            #endif
            EXIT_ISR


;**************************************************************************
; Notes
;
; 1. Do not set a breakpoint on ENTER_ISR or the debugger may corrupt
;    execution of the application. It is ok to break inside ENTER_ISR
;    after the point where it does the mode switch. See the DEBUG NOTE
;    in irq_iar.inc for more information.
;

            END
