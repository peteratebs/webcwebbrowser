;
; ARMDEFS.INC  (IAR version)                                Version 3.7.1
;
; ARM main definitions file. Contains:
;
;    1. Includes of the appropriate processor .inc files for I/O register
;       definitions and function prototypes.
;
;    2. Processor peripheral configuration settings.
;
; Copyright (c) 2001-2009 Micro Digital Associates, Inc.
;               All Rights Reserved.
;
; Author: David Moore
;
;**************************************************************************


;---- Derived Defines -----------------------------------------------------

; Atmel AT91 processors

; Atmel AT91X40 processors are very much the same (they all share 1
; datasheet for the bulk of the info). Define PROCESSOR_AT91X40 to
; avoid needless repetition.
;

   #ifdef PROCESSOR_AT91M40800
#define PROCESSOR_AT91
#define PROCESSOR_AT91X40
#define PROCESSOR_AT91LIB
   #endif
   #ifdef PROCESSOR_AT91M40807
#define PROCESSOR_AT91
#define PROCESSOR_AT91X40
#define PROCESSOR_AT91LIB
   #endif
   #ifdef PROCESSOR_AT91R40807
#define PROCESSOR_AT91
#define PROCESSOR_AT91X40
#define PROCESSOR_AT91LIB
   #endif
   #ifdef PROCESSOR_AT91R40008
#define PROCESSOR_AT91
#define PROCESSOR_AT91X40
#define PROCESSOR_AT91LIB
   #endif
   #ifdef PROCESSOR_AT91F40816
#define PROCESSOR_AT91
#define PROCESSOR_AT91X40
#define PROCESSOR_AT91LIB
   #endif
   #ifdef PROCESSOR_AT91FR4081
#define PROCESSOR_AT91
#define PROCESSOR_AT91X40
#define PROCESSOR_AT91LIB
   #endif

   #ifdef PROCESSOR_AT91M43300
#define PROCESSOR_AT91
#define PROCESSOR_AT91LIB
   #endif
   #ifdef PROCESSOR_AT91M63200
#define PROCESSOR_AT91
#define PROCESSOR_AT91LIB
   #endif
   #ifdef PROCESSOR_AT91M42800A
#define PROCESSOR_AT91
#define PROCESSOR_AT91LIB
   #endif
   #ifdef PROCESSOR_AT91M55800A
#define PROCESSOR_AT91
#define PROCESSOR_AT91LIB
   #endif
   #ifdef PROCESSOR_AT91RM3400
#define PROCESSOR_AT91
#define PROCESSOR_AT91LIB
   #endif

; Newer processors are not supported by the AT91 library.
   #ifndef PROCESSOR_AT91
   #ifdef PROCESSOR_AT91RM9200
#define PROCESSOR_AT91
   #endif
   #ifdef PROCESSOR_AT91SAM7
#define PROCESSOR_AT91
   #endif
   #ifdef PROCESSOR_AT91SAM9
#define PROCESSOR_AT91
   #endif
   #endif


;---- Master Includes -----------------------------------------------------

   #ifdef PROCESSOR_AT91R40008
   #include "targets\eb40a\eb40a_iar.inc"   ; Master .inc for board and processor
   #endif

   #ifdef PROCESSOR_AT91M55800A
   #include "targets\eb55\eb55_iar.inc"     ; Master .inc for board and processor
   #endif

; Add includes here like armdefs.h if you write assembly application code.


;---- Other Defines -------------------------------------------------------

; IRQ Dispatch in Hardware or Software
;
; Some ARM's support hardware vectoring of interrupts. For these, our
; ENTER_ISR and EXIT_ISR macros are used. Otherwise, smx_irq_handler
; is hooked to the IRQ slot and it calls the IRQ dispatcher routine.
;
   #ifdef PROCESSOR_AT91
#define IRQ_DISPATCH_IN_HDW  1
   #else
   #ifdef PROCESSOR_LH75401
#define IRQ_DISPATCH_IN_HDW  1
   #else
#define IRQ_DISPATCH_IN_HDW  0
   #endif
   #endif

; Peripheral Register Addresses
;
; Only a few defs are needed so they are defined here. It would be better
; to create a separate .inc file for each processor if more assembly code
; is written.
;
   #ifdef PROCESSOR_AT91RM9200
AIC_BASE   EQU  0xFFFFF000
AIC_IVR    EQU  0x100
AIC_EOICR  EQU  0x130
   #endif
   #ifdef PROCESSOR_AT91SAM7
AIC_BASE   EQU  0xFFFFF000
AIC_IVR    EQU  0x100
AIC_EOICR  EQU  0x130
   #endif
   #ifdef PROCESSOR_AT91SAM9
AIC_BASE   EQU  0xFFFFF000
AIC_IVR    EQU  0x100
AIC_EOICR  EQU  0x130
   #endif

