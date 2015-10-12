;
; IRQ.INC  (IAR version)                                    Version 3.7.1
;
; ENTER_ISR and EXIT_ISR macros for ARM CPU's that support hardware
; dispatching of interrupts, such as the Atmel AT91 series. ARM's that
; do dispatching in software use the isr shell in IRQ.S instead.
;
; Unlike most ARM CPU's that require a software dispatcher, there is
; truly a separate vector for each IRQ. Because of this, isr's are hooked
; to each level, just as on CPU's such as x86 and 68K/ColdFire. (The ARM
; core still has only 1 IRQ vector in the EVT, but the Atmel interrupt
; controller has its own registers to store each interrupt vector and
; the IRQ slot is used to redirect to the appropriate handler via the
; Atmel IRQ Vector Register.)
;
; Other ARM CPU's have a single IRQ vector which must point to a single
; isr which then calls the dispatcher routine, which in turn calls the
; appropriate isr function. IRQ.S is used for those CPU's.
;
; Copyright (c) 2002-2007 Micro Digital Associates, Inc.
;               All Rights Reserved.
;
; Author: David Moore
; v3.7.0: David Moore: Changed LDRB/STRB to LDR/STR for smx globals that
;         were 8-bit but are now 32-bit (app_busy, lq_ctr, sched, srnest).
;
;**************************************************************************

   #include "armdefs_iar.inc"
   #include "conf_iar.inc"  ; could include conf.h but see comment in this file

   EXTERN   r14_irq_sav
   EXTERN   SPSR_irq_sav
   EXTERN   app_busy
   EXTERN   lq_ctr
   EXTERN   sched
   EXTERN   srnest
   EXTERN   runLsr
   EXTERN   runTask

OFFSET_PC  EQU  28  ; see comment for this symbol in code below

   ; Register usage in the code below:
   ;   r0  scratch register
   ;   r1  pointer to globals to get/set values in them
   ;
   ; Key:
   ;   ;*   interrupts are disabled
   ;   ;(*) interrupts may be disabled
   ; 

   ; DEBUG NOTE: Don't try to break near the top of ENTER_ISR. Break
   ; after the point where it has switched from IRQ mode back to the
   ; task's mode (see "END OF IRQ MODE" below). We observed that the
   ; debugger was somehow causing r13_svc and r14_svc to be corrupted
   ; even though running in IRQ mode! This may not be a problem for
   ; all boards, debuggers, and JTAG units, but we observed it for
   ; the Atmel EB55 using AXD as the debugger and the Abatron BDI2000
   ; JTAG unit.

;----------------------------------------------------------------------------

ENTER_ISR: MACRO
   ;
   ; This is the entry from the IRQ vector. At this point:
   ;   1. We are using the IRQ stack
   ;   2. r14 is r14_irq (i.e. the banked r14 for IRQ mode); the task's r14 is not accessible.
   ;      r14 contains the address to return to (the point of interrupt), but needs to be adjusted below (see note 4).
   ;   3. Interrupts are DISABLED
   ;
   STMFD    r13!, {r0-r1}           ;* push scratch registers
   ;
   ; Only use r0 and r1 while in the IRQ mode/stack. After the mode switch
   ; below we save more registers.
   ;

   #ifdef PROCESSOR_AT91
   ;
   ; Write to the IVR to support Protect Mode (no effect in Normal Mode).
   ; De-assert the NIRQ and clear the source in Protect Mode.
   ;
   LDR      r0, =AIC_BASE           ;*
   STR      r0, [r0, #AIC_IVR]      ;* value written does not matter
   #endif

   ;
   ; ***** MODE & STACK SWITCH *****
   ;
   ; Save r14_irq in a global so after the mode/stack switch we can get it
   ; and push it onto the task's stack.
   ;
   LDR      r0, =r14_irq_sav        ;* r0 -> r14_irq_sav
   STR      r14, [r0]               ;* r14_irq_sav = r14_irq
   ;
   ; Save SPSR_irq in a global so after the mode/stack switch we can get it
   ; and push it onto the task's stack. We need to preserve T (Thumb) bit
   ; for proper return to Thumb or ARM code. (Also it preserves the I bit,
   ; but we know I must have been clear or we wouldn't be here.)
   ; Can use r14 as scratch register because we just saved it above.
   ;
   MRS      r14, SPSR               ;* r14 = SPSR_irq
   LDR      r0,  =SPSR_irq_sav      ;* r0 -> SPSR_irq_sav
   STR      r14, [r0]               ;* SPSR_irq_sav = SPSR_irq
   ;
   ; Pop scratch regs.
   ;
   LDMFD    r13!, {r0-r1}           ;* pop r0-r1; done using as scratch regs
   ;
   ; IRQ stack is empty now. All registers are as they were at the time the
   ; interrupt occurred, except r14_irq which we saved in a global, above.
   ; We also saved SPSR_irq above, since we need to change it here.
   ;
   ; Set I bit in SPSR so interrupts will still be inhibited when written
   ; to CPSR in the mode switch below. Also, clear T (Thumb) bit since we
   ; need to continue running in ARM mode.
   ;
   ; r14 = SPSR_irq from above. Ok to use r14 as a scratch register since
   ; we saved it in a global, above.
   ORR      r14, r14, #0x80         ;* set I bit in SPSR_irq to disable IRQ interrupts for mode switch
   BIC      r14, r14, #0x20         ;* clear T bit in SPSR_irq to ensure we run in ARM mode not Thumb mode
   MSR      CPSR_csxf, r14          ;* MODE SWITCH. CPSR_irq = r14
   ;                                ;* CPSR was restored to what it was at time of interrupt (except I bit set and T bit clear).
   ; ***** END OF IRQ MODE *****
   ;
   ;........................................................................
   ;
   ; ***** Back in TASK'S MODE and TASK'S STACK. Interrupts DISABLED *****
   ;
   ; Push volatile registers and r14_svc so code can make calls. r0 and r1
   ; are used below as scratch registers in the operations which follow.
   ; These need to be saved separately from the rest anyway, since they
   ; are needed until the end of this routine, where it exits.
   ;
   ; Push CPSR, which has the original value it had when the interrupt
   ; occurred because SPSR_irq was already moved into it above by the MOVS
   ; used to do the jump (and mode switch). However, above we set the I bit
   ; in it to disable interrupts during the mode switch, so below we clear
   ; the I bit which will reenable them when we write it back into the
   ; register just before exiting this routine.
   ;
   ; Push value in SPSR_irq_sav so we can restore CPSR to what it was at
   ; the time of interrupt.
   ;
   ; Push value in r14_irq_sav so we can return to it after we restore all
   ; the registers, which we save below. Now that we're back in the task's
   ; mode, we can't change r14 because it is now r14_svc and will hold the
   ; return address from the function that was interrupted to its caller.
   ;
   ; *** Change OFFSET_PC above if any registers are added/removed from
   ; *** next STMFD. It is the offset to pc that is pushed in the next
   ; *** STMFD, after also pushing SPSR_irq_sav.
   ;
   STMFD    r13!,{r0-r3,r12,r14,pc} ;* push volatile registers and r14_svc for calls
   ;                                ;* pushing pc reserves space for r14_irq_sav (return addr to point of interrupt)
   ;                                ;*
   ; Push SPSR_irq_sav on the stack. CPSR will be restored to this on exit.
   ;                                ;*
   LDR      r1, =SPSR_irq_sav       ;* r1 -> SPSR_irq_sav
   LDR      r0, [r1]                ;* r0 = SPSR_irq_sav
   STMFD    r13!, {r0}              ;* push SPSR_irq_sav
   ;                                ;*
   ; Save r14_irq_sav (adjusted) in the slot on the stack.
   ;                                ;*
   LDR      r1, =r14_irq_sav        ;* r1 -> r14_irq_sav
   LDR      r0, [r1]                ;* r0 = r14_irq_sav
   SUB      r0, r0, #4              ;* r0 = address of point of interrupt. See note 4 about subtracting 4.
   STR      r0, [r13, #OFFSET_PC]   ;* write r14_irq_sav (adjusted) to reserved slot on stack (where pc pushed, above)

   ;
   ; srnest++ (This is usually all ENTER_ISR does!)
   ;
   LDR      r1, =srnest             ;* r1 -> srnest
   LDR      r0, [r1]                ;* r0 = srnest
   ADD      r0, r0, #1              ;* r0 += 1
   STR      r0, [r1]                ;* srnest = r0

   ENDM

;----------------------------------------------------------------------------

EXIT_ISR: MACRO
          LOCAL sih_bypass_task_sched
          LOCAL sih_exit
   
   #ifdef PROCESSOR_AT91
   ; DISABLE interrupts before signaling EOI to AIC. See "Standard
   ; Interrupt Sequence" in Atmel databook for AT91 CPU's, step 7.
   MRS      r0, CPSR                ;(*) r0 = status register
   ORR      r0, r0, #0x80           ;(*) disable IRQ interrupts
   MSR      CPSR_c, r0              ;(*) write back to disable interrupts
   ; End of Interrupt to the AIC
   LDR      r0, =AIC_BASE           ;*
   STR      r0, [r0, #AIC_EOICR]    ;*
   #endif

   ; if srnest != 1 goto sih_exit (label 1)
   ; Means nested isr, so return immediately to point of interrupt
   ; (outermost isr will go to scheduler).
   ;
   LDR      r1, =srnest             ;(*) r1 -> srnest
   LDR      r0, [r1]                ;(*) r0 = srnest
   CMP      r0, #1                  ;(*) srnest == 1?
   BNE      sih_exit                ;(*) no, exit

   #ifdef PROCESSOR_AT91
   ; Disabled interrupts above.
   #else
   ; DISABLE interrupts before checking lq_ctr.
   MRS      r0, CPSR                ;(*) r0 = status register
   ORR      r0, r0, #0x80           ;(*) disable IRQ interrupts
   MSR      CPSR_c, r0              ;(*) write back to disable interrupts
   #endif

   ; if lq_ctr == 0 goto sih_exit (label 1)
   LDR      r1, =lq_ctr             ;* r1 -> lq_ctr
   LDR      r0, [r1]                ;* r0 = lq_ctr
   CMP      r0, #0                  ;* lq_ctr == 0?
   BEQ      sih_exit                ;* yes, exit

   ; PROFILE_START_OVH() (app_busy = 0)
   LDR      r1, =app_busy           ;* r1 -> app_busy
   MOV      r0, #0                  ;* r0 = 0
   STR      r0, [r1]                ;* app_busy = r0

   #if 1 ; Thumb arch
   LDR      r0, =runLsr
   MOV      lr, pc
   BX       r0                      ;* ***** CALL LSR SCHEDULER *****
   #else ; ARM-only arch
   BL       runLsr                  ;* ***** CALL LSR SCHEDULER *****
   #endif
                                    ;        ENABLES Interrupts (also, lsr's call ssr's which enable ints)
   ; Interrupts are DISABLED        ;*       DISABLES on exit
   ;
   ; r14 was saved on the task's stack right after the mode switch.
   ; runLsr() automatically saves any nonvolatile regs it uses (r4-r11)

   ; if sched == 0 goto sih_bypass_task_sched (label 0)
   ; Means task reschedule is not necessary since no tasks became ready.
   ;
   LDR      r1, =sched              ;* r1 -> sched
   LDR      r0, [r1]                ;* r0 = sched
   CMP      r0, #0                  ;* sched == 0?
   BEQ      sih_bypass_task_sched   ;* yes (sched == 0), skip task scheduler and exit

   ;
   ; Go to the Task Scheduler
   ; Push all the other registers in case a task switch occurs in runTask().
   ;
   STMFD    r13!, {r4-r11}          ;* push r4-r11
                                    ;*
   #if 1 ; Thumb arch
   LDR      r0, =runTask
   MOV      lr, pc
   BX       r0                      ;* ***** CALL TASK SCHEDULER *****
   #else ; ARM-only arch
   BL       runTask                 ;* ***** CALL TASK SCHEDULER *****
   #endif
                                    ;        ENABLES Interrupts (also, tasks call ssr's which enable ints)
                                    ;*       DISABLES on exit
                                    ;*
   ; Pop the registers saved before the call to runTask()
   ;                                ;*
   LDMFD    r13!, {r4-r11}          ;* pop r4-r11

sih_bypass_task_sched:

   ; Final Operations: Mark end of overhead and decrement srnest.
   ;
   ; PROFILE_END_OVH() (app_busy = 1)
   LDR      r1, =app_busy           ;* r1 -> app_busy
   MOV      r0, #1                  ;* r0 = 1
   STR      r0, [r1]                ;* app_busy = r0
   ; fall through

sih_exit:

   ;
   ; srnest--
   ;
   LDR      r1, =srnest             ;(*) r1 -> srnest
   LDR      r0, [r1]                ;(*) r0 = srnest
   SUB      r0, r0, #1              ;(*) r0 -= 1
   STR      r0, [r1]                ;(*) srnest = r0
   ;
   #ifdef PROCESSOR_AT91
   ; For Atmel AT91, interrupts were already disabled before branching
   ; to sih_exit and in the fall-through case.
   #else
   ; DISABLE interrupts before restoring SPSR
   MRS      r0, CPSR                ;(*) r0 = status register
   ORR      r0, r0, #0x80           ;(*) disable IRQ interrupts
   MSR      CPSR_c, r0              ;(*) write back to disable interrupts
   #endif                            ;*
                                    ;*
   ;
   ; Restore SPSR since it will be moved into CPSR on exit below. This
   ; is the SPSR at the time of interrupt. If Thumb code was interrupted,
   ; it will have the T bit set. We needed to save it on the stack rather
   ; than load it into CPSR because we can't have the T bit set while
   ; running in this interrupt handler (which is written for ARM mode),
   ; and because the code above may change flags. CPSR must be restored
   ; exactly how it was at the time of interrupt.
   ;
   ; ASSUMPTION: Application/SMX runs in Supervisor (SVC) mode since
   ;             there is no SPSR in System or User mode.
   ;
   LDMFD    r13!, {r0}                ;* pop SPSR into r0
   MSR      SPSR_csxf, r0             ;* SPSR_svc = r0

   ; ***** RETURN TO POINT OF INTERRUPT *****
   ;
   LDMFD    r13!,{r0-r3,r12,r14,pc}^  ;* restore r0-r3,r12,r14_svc and RETURN
   ;                                  ;* top of stack holds r14_irq-4 (return address from isr)
   ;                                  ;* ^ means to load CPSR from SPSR

   ENDM

;----------------------------------------------------------------------------
; Notes
;
; 1. Here is what this code does:
;
;    ENTER_ISR:
;       push r0-r1 on IRQ stack
;       (commented out: push r14 so calls can be made here such as calling LED routines)
;       (commented out: pop r14)
;       save r14_irq in r14_irq_sav
;       save SPSR_irq in SPSR_irq_sav
;       pop r0 and r1 (from IRQ stack)
;       set CPSR to SPSR with I bit set and T bit clear, to switch to task's mode/stack, ints disabled, ARM mode (not Thumb)
;
;       (USING TASK'S STACK NOW and in TASK'S MODE, not IRQ MODE)
;       reserve slot for r14_irq on task stack
;       push r0-r3,r12,r14,pc (pc is placeholder; see note 2 below)
;       push SPSR_irq_sav (this is what was in CPSR at point of interrupt)
;       write r14_irq into reserved slot
;       srnest++
;
;    EXIT_ISR:
;       if(srnest == 1)
;       {
;          DISABLE interrupts
;          if(lq_ctr != 0)
;          {
;             PROFILE_START_OVH() (app_busy = 0)
;             runLsr(); (ENABLES interrupts then exits DISABLED)
;             if (sched != 0)
;             {
;                push r4-r11
;                call runTask()
;                pop r4-r11
;             }
;             PROFILE_END_OVH() (app_busy = 1)
;          }
;       }
;       srnest--
;       DISABLE interrupts
;       pop SPSR
;       return with LDMFD, which pops r0-r3,r12,r14,pc (ret addr into pc), and SPSR written to CPSR 
;
; 2. At the top of ENTER_ISR (after the mode/stack switch), we only
;    save r0-r3, r12, and r14 because the compiler automatically saves
;    r4-r11 (the "nonvolatile" registers) in the prolog of any called
;    routines that use them. For example, if the dispatcher routine uses
;    r4 and r5 they will be saved in its prolog and restored in its epilog.
;    pc is pushed as a placeholder; the return address (r14_irq_sav) is
;    patched into this location by the code a few statements below.
;
; 3. Before entering the task scheduler, we have to save all registers
;    (on the task stack) because a task switch could occur and we don't
;    know what registers the other tasks might change before this task
;    is eventually resumed.
;
; 4. The reason we subtract 4 from the link register (r14) on return is
;    because it is pointing 1 instruction beyond the instruction to return
;    to, due to the instruction pipeline. The ARM Architecture Reference Manual
;    section 2.6.6 Interrupt Request (IRQ) Exception shows this. The ARM
;    interrupt handling whitepaper near figure 1.13 explains this more.
;
; 5. In the initial implementation, we did the mode/stack switch much
;    later so that the isr's and lsr's ran on the IRQ stack. This would
;    have allowed task stacks to be smaller since they wouldn't have to
;    allow for space needed for isr's. Unfortunately, this approach
;    didn't work in the case where an isr or lsr called a "leaf" function
;    (one that doesn't call any other functions). Leaf functions don't save
;    r14 in their prologs, so if an interrupt occurs there, while in IRQ
;    mode already, r14_irq would get overwritten by the processor due to
;    the interrupt and the leaf function could no longer return to who
;    called it.
;

