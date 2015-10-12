;
; CONF.INC  (CodeWarrior version)                           Version 3.7.0
;
; CONFIGURATION PARAMETERS for Assembly files. IAR assembler allows
; including a .h file, so it should be possible to include conf.h
; instead of this, but currently none of the symbols are needed,
; and this requires us to define some of the symbols for the assembler
; that are defined in the C preinclude files. Unfortunately, IAR
; does not support preinclude files for assembly, so this would require
; us to enter them into the Defined symbols box in the project files.
;
; Copyright (c) 2002-2005 Micro Digital Associates, Inc.
;               All Rights Reserved.
;
; Author: David Moore
;
;**********************************************************************

;
; Keep these settings in sync with CONF.H.
;
; Settings that were here are no longer needed. This file has been
; retained since there may be other settings to add in the future.
;

