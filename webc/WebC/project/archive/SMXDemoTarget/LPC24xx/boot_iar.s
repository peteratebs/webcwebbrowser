/*
* BOOT_IAR.S  (IAR version)                                Version 3.7.1
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
Mode_USR        EQU      0x10
Mode_FIQ        EQU      0x11
Mode_IRQ        EQU      0x12
Mode_SVC        EQU      0x13
Mode_ABT        EQU      0x17
Mode_UND        EQU      0x1B
Mode_SYS        EQU      0x1F

I_Bit           EQU      0x80    /* when I bit is set, IRQ is disabled */
F_Bit           EQU      0x40    /* when F bit is set, FIQ is disabled */

#if __VER__ >= 5010005

    ;; Forward declaration of sections.
    SECTION IRQ_STACK:DATA:NOROOT(3)
    SECTION FIQ_STACK:DATA:NOROOT(3)
    SECTION ABT_STACK:DATA:NOROOT(3)
    SECTION UND_STACK:DATA:NOROOT(3)
    SECTION SYS_STACK:DATA:NOROOT(3)
    SECTION SVC_STACK:DATA:NOROOT(3)

#else /* __VER__ */

ABT_Stack_Size  EQU     0x00000010
UND_Stack_Size  EQU     0x00000010
SYS_Stack_Size  EQU     0x00000010
FIQ_Stack_Size  EQU     0x00000010
IRQ_Stack_Size  EQU     0x00000010
SVC_Stack_Size  EQU     0x00000100  /* initial stack until multitasking */

#endif /* __VER__ */

/* Reset Code must be linked first at Address at which it expects to run. */
    RSEG        ICODE:CODE:NOROOT(2)
                CODE32

    PUBLIC  InitReset
    PUBLIC  __vector
    PUBLIC  __vector_0x14
    EXTERN  ?main
    EXTERN  bspInitClocks
    EXTERN  bsplDelayLoop

#if __VER__ < 5010005
    EXTERN  RAMEND
__stack_end__   EQU     RAMEND - 0x0000000F
#endif


InitReset

/* Pre-defined interrupt handlers that may be directly */
/* overwritten by C interrupt functions */
/* Exception Vectors */
/* Mapped to Address 0. */
/* Absolute addressing mode must be used. */

Vectors:
__vector:
                LDR       PC,Reset_Addr
                LDR       PC,Undef_Addr
                LDR       PC,SWI_Addr
                LDR       PC,PAbt_Addr
                LDR       PC,DAbt_Addr
__vector_0x14:
                DC32      0xB9206E50  /* Reserved Vector */
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

Reset_Addr:     DC32      reset_handler
Undef_Addr:     DC32      undef_handler
SWI_Addr:       DC32      swi_handler
PAbt_Addr:      DC32      prefetch_handler
DAbt_Addr:      DC32      data_handler
                DC32      0                  /* Reserved Address */
IRQ_Addr:       DC32      irq_handler        /* IRQ */
FIQ_Addr:       DC32      fiq_handler

reset_handler
        b       startup
undef_handler
        b       undef_handler
swi_handler
        b       swi_handler
prefetch_handler
        b       prefetch_handler
data_handler
        b       data_handler
reserved_handler
        b       reserved_handler
irq_handler
        b       irq_handler
fiq_handler
        b       fiq_handler

startup
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

#if __VER__ >= 5010005

/* Enter Undefined Instruction Mode and set its Stack Pointer */
                MSR     CPSR_c, #Mode_UND|I_Bit|F_Bit
                LDR     SP, =SFE(UND_STACK)
                BIC     SP, SP, #0x07

/* Enter Abort Mode and set its Stack Pointer */
                MSR     CPSR_c, #Mode_ABT|I_Bit|F_Bit
                LDR     SP, =SFE(ABT_STACK)
                BIC     SP, SP, #0x07

/* Enter FIQ Mode and set its Stack Pointer */
                MSR     CPSR_c, #Mode_FIQ|I_Bit|F_Bit
                LDR     SP, =SFE(FIQ_STACK)
                BIC     SP, SP, #0x07

/* Enter IRQ Mode and set its Stack Pointer */
                MSR     CPSR_c, #Mode_IRQ|I_Bit|F_Bit
                LDR     SP, =SFE(IRQ_STACK)
                BIC     SP, SP, #0x07

/* Enter System/User Mode and set its Stack Pointer */
                MSR     CPSR_c, #Mode_SYS|I_Bit|F_Bit
                LDR     SP, =SFE(SYS_STACK)
                BIC     SP, SP, #0x07

/* Enter Supervisor Mode and set its Stack Pointer */
                MSR     CPSR_c, #Mode_SVC|I_Bit|F_Bit
                LDR     SP, =SFE(SVC_STACK)
                BIC     SP, SP, #0x07

/* Branch to the C library startup function that clears uninitialized data,
   copies initialized data from ROM to RAM, runs C++ initializers, and
   branches to the C main() function.
*/
                BL      bspInitClocks
                BL      ?main

#else /* __VER__ */

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

/* Branch to the C library startup function that clears uninitialized data,
   copies initialized data from ROM to RAM, runs C++ initializers, and
   branches to the C main() function.
*/
                BL      bspInitClocks
                BL      ?main

LC0:            DC32     __stack_end__
#endif /* __VER__ */

                END

