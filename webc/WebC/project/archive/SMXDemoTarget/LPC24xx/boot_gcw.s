/*
* BOOT_GCW.S  (CrossWorks version)                          Version 3.7.1
*
* Boot code for NXP (Philips) LPC24xx processors.
*
* Copyright (c) 2007-2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author: Wan Xingsheng
*
***********************************************************************/

/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs */
.set Mode_USR        ,      0x10
.set Mode_FIQ        ,      0x11
.set Mode_IRQ        ,      0x12
.set Mode_SVC        ,      0x13
.set Mode_ABT        ,      0x17
.set Mode_UND        ,      0x1B
.set Mode_SYS        ,      0x1F

.set I_Bit           ,      0x80    /* when I bit is set, IRQ is disabled */
.set F_Bit           ,      0x40    /* when F bit is set, FIQ is disabled */

.set ABT_Stack_Size  ,     0x00000010
.set UND_Stack_Size  ,     0x00000010
.set SYS_Stack_Size  ,     0x00000010
.set FIQ_Stack_Size  ,     0x00000010
.set IRQ_Stack_Size  ,     0x00000010
.set SVC_Stack_Size  ,     0x00000100  /* initial stack until multitasking */

.set __stack_end__   ,     RAMEND - 0x0000000F

/* Reset Code must be linked first at Address at which it expects to run. */
    .section .vectors, "x"
    .arm

    .global  InitReset
    .global  startup
    .extern  main

InitReset:

/* Pre-defined interrupt handlers that may be directly */
/* overwritten by C interrupt functions */
/* Exception Vectors */
/* Mapped to Address 0. */
/* Absolute addressing mode must be used. */

  .section .init, "ax"
  .arm
  .align 0

Vectors:
__vector:
                LDR       PC,Reset_Addr
                LDR       PC,Undef_Addr
                LDR       PC,SWI_Addr
                LDR       PC,PAbt_Addr
                LDR       PC,DAbt_Addr
__vector_0x14:
               .word      0xB9206E50  /* Reserved Vector */
                                      /* For LPC24xx, here is saved valid user code: which should contain the 2's
                                         complement of the check-sum of the remaining interrupt vectors. This causes
                                         the checksum of all of the vectors together to be 0. The boot loader code
                                         disables the overlaying of the interrupt vectors from the boot block, then
                                         checksums the interrupt vectors in sector 0 of the Flash. If the signatures
                                         match then the execution control is transferred to the user code by loading
                                         the program counter with 0x00000000.
                                       */
                LDR       PC,[PC, #-0x0120]  /* Vector from VicVectAddr (0xFFFFFF00) */
                LDR       PC,FIQ_Addr

Reset_Addr:     .word      reset_handler
Undef_Addr:     .word      undef_handler
SWI_Addr:       .word      swi_handler
PAbt_Addr:      .word      prefetch_handler
DAbt_Addr:      .word      data_handler
                .word      0                  /* Reserved Address */
IRQ_Addr:       .word      irq_handler        /* IRQ */
FIQ_Addr:       .word      fiq_handler

reset_handler:
        b       startup
undef_handler:
        b       undef_handler
swi_handler:
        b       swi_handler
prefetch_handler:
        b       prefetch_handler
data_handler:
        b       data_handler
reserved_handler:
        b       reserved_handler
irq_handler:
        b       irq_handler
fiq_handler:
        b       fiq_handler

  .section .init, "ax"
  .arm
  .align 0

startup:
#ifndef SMX_BT_ROM
                /* Remap address 0x0 to Internal RAM */
                LDR     R0, =0xE01FC040
                LDR     R1, =0x2
                STR     R1, [R0]
                
                /* copy Interrupt Vectors to 0x0 */
                LDR     R1, =Vectors       /* Source IRQ Vector Address */
                MOV     R2, #0x0           /* Internal SRAM Address */
                MOV     R0, #0x40          /* Third arg: length of block */
copy_loop:
                LDR     R3, [R1]
                STR     R3, [R2]
                ADD     R1, R1, #4
                ADD     R2, R2, #4
                SUBS    R0, R0, #4
                BGT     copy_loop
#else
                /* Init Configure EMC for external SDRAM, NAND and NOR FLASH */

                /* Set External Memory Controller Reset Disable. (SCS |= 0x2;)*/
                LDR     R0, =0xE01FC1A0
                LDR     R1, [R0]
                ORR     R1, R1, #0x2
                STR     R1, [R0]

                /* Enable EMC ((EMCControl = 0x1;) */
                LDR     R0, =0xFFE08000
                LDR     R1, =0x1
                STR     R1, [R0]

                /* Turn On EMC PCLK (PCONP |= 0x800;) */
                LDR     R0, =0xE01FC0C4
                LDR     R1, [R0]
                ORR     R1, R1, #0x800
                STR     R1, [R0]

                /* Assign GPIO to EMC. */
                /* PINSEL4 = 0x50000000; */
                LDR     R0, =0xE002C010
                LDR     R1, =0x50000000
                STR     R1, [R0]

                /* PINSEL5 = 0x05050555; */
                LDR     R0, =0xE002C014
                LDR     R1, =0x05050555
                STR     R1, [R0]
                
                /* PINSEL6 = 0x55555555; */
                LDR     R0, =0xE002C018
                LDR     R1, =0x55555555
                STR     R1, [R0]

                /* PINSEL8 = 0x55555555; */
                LDR     R0, =0xE002C020
                LDR     R1, =0x55555555
                STR     R1, [R0]

                /* PINSEL9 = 0x50555555; */
                LDR     R0, =0xE002C024
                LDR     R1, =0x50555555
                STR     R1, [R0]

                /* TODO: Add init NAND and NOR FLASH here. */

                /* Init SDRAM */
                /* Send command: NOP (EMCDynamicControl = 0x183;) */
                LDR     R0, =0xFFE08020
                LDR     R1, =0x183
                STR     R1, [R0]

                /* Wait a while */
                LDR     R0, =20000
                BL      bsplDelayLoop

                /* Send command: PRECHARGE-ALL, shortest possible refresh period (EMCDynamicControl = 0x103;) */
                LDR     R0, =0xFFE08020
                LDR     R1, =0x103
                STR     R1, [R0]

                /* Set Dynamic Memory Refresh Timer (EMCDynamicRefresh = 0x2;) */
                LDR     R0, =0xFFE08024
                LDR     R1, =0x2
                STR     R1, [R0]
                
                /* Wait a while */
                LDR     R0, =200
                BL      bsplDelayLoop

                /* Set Dynamic Memory Refresh Timer (EMCDynamicRefresh = 20;) */
                LDR     R0, =0xFFE08024
                LDR     R1, =0x14
                STR     R1, [R0]

                /* Set Dynamic Memory RAS & CAS Delay (EMCDynamicRasCas0 = 0x303;) */
                LDR     R0, =0xFFE08104
                LDR     R1, =0x303
                STR     R1, [R0]

                /* Set Dynamic Memory Read Configuration (EMCDynamicReadConfig = 0x1;) */
                LDR     R0, =0xFFE08028
                LDR     R1, =0x1
                STR     R1, [R0]

                /* Set Dynamic Memory Percentage Command Period (EMCDynamictRP = 0x2;) */
                LDR     R0, =0xFFE08030
                LDR     R1, =0x2
                STR     R1, [R0]

                /* Set Dynamic Memory Active to Precharge Command Period (EMCDynamictRAS = 0x3;) */
                LDR     R0, =0xFFE08034
                LDR     R1, =0x3
                STR     R1, [R0]

                /* Set Dynamic Memory Self-refresh Exit Time (EMCDynamictSREX = 0x7;) */
                LDR     R0, =0xFFE08038
                LDR     R1, =0x7
                STR     R1, [R0]

                /* Set Dynamic Memory Last Data Out to Active Time (EMCDynamictAPR = 0x2;) */
                LDR     R0, =0xFFE0803C
                LDR     R1, =0x2
                STR     R1, [R0]

                /* Set Dynamic Memory Data-in to Active Command Time (EMCDynamictDAL = 0x5;) */
                LDR     R0, =0xFFE08040
                LDR     R1, =0x5
                STR     R1, [R0]

                /* Set Dynamic Memory Write Recovery Time (EMCDynamictWR = 0x1;) */
                LDR     R0, =0xFFE08044
                LDR     R1, =0x1
                STR     R1, [R0]

                /* Set Dynamic Memory Active to Active Command Period (EMCDynamictRC = 0x5;) */
                LDR     R0, =0xFFE08048
                LDR     R1, =0x5
                STR     R1, [R0]

                /* Set Dynamic Memory Auto-refresh Period (EMCDynamictRFC = 0x5;) */
                LDR     R0, =0xFFE0804C
                LDR     R1, =0x5
                STR     R1, [R0]

                /* Set Dynamic Memory Exit Self-refresh (EMCDynamictXSR = 0x7;) */
                LDR     R0, =0xFFE08050
                LDR     R1, =0x7
                STR     R1, [R0]

                /* Set Dynamic Memory Active Bank A to Active Bank B Time (EMCDynamictRRD = 0x1;) */
                LDR     R0, =0xFFE08054
                LDR     R1, =0x1
                STR     R1, [R0]

                /* Set Dynamic Memory Load Mode register to Active Command Time (EMCDynamictMRD = 0x2;) */
                LDR     R0, =0xFFE08058
                LDR     R1, =0x2
                STR     R1, [R0]

                /* Set Dynamic Memory Configuration (EMCDynamicConfig0 = 0x680;) */
                LDR     R0, =0xFFE08100
                LDR     R1, =0x680
                STR     R1, [R0]
                
                /* Send command: MODE (EMCDynamicControl = 0x83;) */
                LDR     R0, =0xFFE08020
                LDR     R1, =0x83
                STR     R1, [R0]

                /* Wait a while */
                LDR     R0, =10000
                BL      bsplDelayLoop

                /* Set mode register in SDRAM */
                NOP
                LDR     R0, =0xA0033000
                LDRB    R1, [R0]
                NOP

                /* Send command: NORMAL (EMCDynamicControl = 0x0;) */
                LDR     R0, =0xFFE08020
                LDR     R1, =0x0
                STR     R1, [R0]

                /* Enable buffer */
                LDR     R0, =0xFFE08100
                LDR     R1, =0x80680
                STR     R1, [R0]
                
                /* Wait a while */
                LDR     R0, =100
                BL      bsplDelayLoop
#endif


/* Setup Stack for each mode */
                LDR     R0, LC0

/* Enter Undefined Instruction Mode and set its Stack Pointer */
                MSR     CPSR_c, #Mode_UND|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #UND_Stack_Size

/* Enter Abort Mode and set its Stack Pointer */
                MSR     CPSR_c, #Mode_ABT|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #ABT_Stack_Size

/* Enter FIQ Mode and set its Stack Pointer */
                MSR     CPSR_c, #Mode_FIQ|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #FIQ_Stack_Size

/* Enter IRQ Mode and set its Stack Pointer */
                MSR     CPSR_c, #Mode_IRQ|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #IRQ_Stack_Size

/* Enter System/User Mode and set its Stack Pointer */
                MSR     CPSR_c, #Mode_SYS|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #SYS_Stack_Size

/* Enter Supervisor Mode and set its Stack Pointer */
                MSR     CPSR_c, #Mode_SVC|I_Bit|F_Bit
                MOV     SP, R0

/* The following code is copied from the Rowley startup code crt0.s.
   It clears uninitialized data, copies initialized data from ROM to RAM,
   and runs C++ initializers. We don't need most of what is in the file
   since it is done above or it is not needed for SMX.
*/

/* Initialize BSS */
                LDR     R0, =__bss_start__
                LDR     R1, =__bss_end__
                MOV     R2, #0
                BL      MemorySet

/* Copy initialized memory sections into RAM (if necessary). */
                LDR     R0, =__data_load_start__
                LDR     R1, =__data_start__
                LDR     R2, =__data_end__
                BL      MemoryCopy
//                LDR     R0, =__text_load_start__
//                LDR     R1, =__text_start__
//                LDR     R2, =__text_end__
//                BL      MemoryCopy
//                LDR     R0, =__fast_load_start__
//                LDR     R1, =__fast_start__
//                LDR     R2, =__fast_end__
//                BL      MemoryCopy
                LDR     R0, =__ctors_load_start__
                LDR     R1, =__ctors_start__
                LDR     R2, =__ctors_end__
                BL      MemoryCopy
                LDR     R0, =__dtors_load_start__
                LDR     R1, =__dtors_start__
                LDR     R2, =__dtors_end__
                BL      MemoryCopy
//                LDR     R0, =__rodata_load_start__
//                LDR     R1, =__rodata_start__
//                LDR     R2, =__rodata_end__
//                BL      MemoryCopy

/* Run C++ Initializers/Constructors */
               LDR      R0, =__ctors_start__
               LDR      R1, =__ctors_end__
ctor_loop:
               CMP      R0, R1
               BEQ      ctor_end
               LDR      R2, [R0], #+4
               STMFD    SP!, {R0-R1}
               MOV      LR, PC
               BX       R2
               LDMFD    SP!, {R0-R1}
               B        ctor_loop
ctor_end:

                BL      bspInitClocks
/* Branch to the C main() function. */
                BL      main


LC0:
                .word   __stack_end__

/* Memory Copy */
MemoryCopy:
                CMP     R0, R1
                MOVEQ   PC, LR
                Subs    R2, R2, R1
MemoryCopyLoop:
                LDRB    R3, [R0], #+1
                STRB    R3, [R1], #+1
                SUBS    R2, R2, #1
                BNE     MemoryCopyLoop
                MOV     PC, LR
/* Memory Set */
MemorySet:
                CMP     R0, R1
                MOVEQ   PC, LR
                STRB    R2, [R0], #1
                B       MemorySet

                .end

