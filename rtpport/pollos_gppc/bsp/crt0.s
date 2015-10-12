/**************************************************************************#
#*                                                                        *#
#* CRT0.S 02/08 TVO                                                     *#
#**************************************************************************/

	.file		"crt0"
##	PowerPC assembler header file
##
##=============================================================================
##=============================================================================
##
##=============================================================================

#------------------------------------------------------------------------------
# Easier to read names for the registers

	.equ	r0, 0
	.equ	r1, 1
	.equ	r2, 2
	.equ	r3, 3
	.equ	r4, 4
	.equ	r5, 5
	.equ	r6, 6
	.equ	r7, 7
	.equ	r8, 8
	.equ	r9, 9
	.equ	r10, 10
	.equ	r11, 11
	.equ	r12, 12
	.equ	r13, 13
	.equ	r14, 14
	.equ	r15, 15
	.equ	r16, 16
	.equ	r17, 17
	.equ	r18, 18
	.equ	r19, 19
	.equ	r20, 20
	.equ	r21, 21
	.equ	r22, 22
	.equ	r23, 23
	.equ	r24, 24
	.equ	r25, 25
	.equ	r26, 26
	.equ	r27, 27
	.equ	r28, 28
	.equ	r29, 29
	.equ	r30, 30
	.equ	r31, 31

	.equ	sp, r1
	.equ	XER, 1
	.equ	LR,  8
	.equ	lr,  8
	.equ	CTR, 9
	.equ	DEC, 22
	.equ	SRR0, 26
	.equ	SRR1, 27
	.equ	SPRG0, 272
	.equ	SPRG1, 273

.extern   __bss_start
.extern   __end /* defined by the linker */
.extern   __stacksize /* defined by the linker */
.extern   __sbss_start
.extern   __sbss_end
.extern   _bdata
.extern   _edata
.extern   _bsavedata
.extern   _esavedata

.globl c__bss_start
.globl c__end
.globl c__stack
.globl c__sbss_start
.globl c__sbss_end
.globl c_bdata 
.globl c_edata 
.globl c_bsavedata 
.globl c_esavedata 

	.data
	.align		4
c__bss_start:  .long 0
c__end:        .long 0 
c__stack:      .long 0
c__sbss_start: .long 0
c__sbss_end:   .long 0
c_bdata:       .long 0
c_edata:       .long 0
c_bsavedata:   .long 0
c_esavedata:   .long 0


	.text
.globl  _start
.globl  _halt
	.align		2

_start:
    mtspr  81, r0 		/* disable interrupts */
    lis	   r1,__end@h	    /* Initialize stack pointer r1 to */
    ori	   r1,r1,__end@l	/* value in linker command file.  */
    lis	   r3,c__end@h
    ori	   r3,r3,c__end@l
    stw    r1,0(r3)

    addi   r1,r1,__stacksize@l /* fill stack with pattern */
    mr	   r2,r1	
    lis	   r3,c__stack@h
    ori	   r3,r3,c__stack@l
    stw    r1,0(r3)
    lis	   r4,0x5353           /* Pattern = SS */
    ori	   r4,r4,0x5353
.L1:
    cmplw  cr7,r2,r3
    blt    cr7,.L2
    b      .L3
.L2:
    stw    r4,0(r2)
    addi   r2,r2,4
    b      .L1
.L3:
    lis	   r4,__sbss_start@h
    ori	   r4,r4,__sbss_start@l
    lis	   r3,c__sbss_start@h
    ori	   r3,r3,c__sbss_start@l
    stw    r4,0(r3)
           
    lis	   r4,__sbss_end@h
    ori	   r4,r4,__sbss_end@l
    lis	   r3,c__sbss_end@h
    ori	   r3,r3,c__sbss_end@l
    stw    r4,0(r3)
           
    lis	   r4,__bss_start@h
    ori	   r4,r4,__bss_start@l
    lis	   r3,c__bss_start@h
    ori	   r3,r3,c__bss_start@l
    stw    r4,0(r3)
           
    lis	   r4,_bdata@h
    ori	   r4,r4,_bdata@l
    lis	   r3,c_bdata@h
    ori	   r3,r3,c_bdata@l
    stw    r4,0(r3)
           
    lis	   r4,_edata@h
    ori	   r4,r4,_edata@l
    lis	   r3,c_edata@h
    ori	   r3,r3,c_edata@l
    stw    r4,0(r3)
           
    lis	   r4,_bsavedata@h
    ori	   r4,r4,_bsavedata@l
    lis	   r3,c_bsavedata@h
    ori	   r3,r3,c_bsavedata@l
    stw    r4,0(r3)
           
    lis	   r4,_esavedata@h
    ori	   r4,r4,_esavedata@l
    lis	   r3,c_esavedata@h
    ori	   r3,r3,c_esavedata@l
    stw    r4,0(r3)
    
    li	   r3,0		/* load r3 (0) */
    stw	   r3,0(r1)	/* init sp points to null */
    stwu   r1,-4(r1)	/* Build a init stack frame */
    bl     startc

_halt:
    b      _halt