/*********************************************************************
*  
*   IAR PowerPac
*
*   (c) Copyright IAR Systems 2008.  All rights reserved.
*
**********************************************************************
----------------------------------------------------------------------
File    : OS_Error.c
Purpose : OS error handler. Not required in "R" (release) builds
          Feel free to modify this file acc. to your target system.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"

/*********************************************************************
*
*       Run-time error reaction (OS_Error)
*
**********************************************************************

 Run-time error reaction

 When this happens, a fatal error has occured and the kernel
 can not continue. In linux, the equivalent would be a
 "kernel panic"

 This routine can be modified to suit your needs ...
 E.g. a red LED could light up. When using an
 emulator, you may set a breakpoint here.
 In the release builds of the library (R), this routine is not required
 (as no checks are performed).
 In the stack check builds (S/SP), only error 120 may occur.
 In the debug builds(D/DP), all of the listed errors may occur.

 The following are the current errorcodes:
 (Please refer to the documentation for more info)
  
*** Port 100..119 ********************************
OS_ERR_ISR_INDEX                      (100 = 0x64)
OS_ERR_ISR_VECTOR                     (101 = 0x65)

*** OS generic ***********************************

*** Stack overflow or invalid stack **************
OS_ERR_STACK                          (120 = 0x78)

*** Counting semaphore overflow ******************
OS_ERR_CSEMA_OVERFLOW                 (121 = 0x79)
                                                 
*** invalid or non-initialized data structures ***
OS_ERR_INV_TASK                       (128 = 0x80)
OS_ERR_INV_TIMER                      (129 = 0x81)
OS_ERR_INV_MAILBOX                    (130 = 0x82)
OS_ERR_INV_CSEMA                      (132 = 0x84)
OS_ERR_INV_RSEMA                      (133 = 0x85)

*** Using GetMail1, PutMail1, GetMailCond1 or PutMailCond1
*** on a non-1 byte mailbox  *********************
OS_ERR_MAILBOX_NOT1                   (135 = 0x87)

*** Waitable objects deleted with waiting tasks or occupied by task ***
OS_ERR_MAILBOX_DELETE                 (136 = 0x88)
OS_ERR_CSEMA_DELETE                   (137 = 0x89)
OS_ERR_RSEMA_DELETE                   (138 = 0x8A)

*** internal errors, please contact SEGGER Microcontrollersysteme ***
OS_ERR_MAILBOX_NOT_IN_LIST            (140 = 0x8C)
OS_ERR_TASKLIST_CORRUPT               (142 = 0x8E)

*** not matching routine calls or macro usage ****
OS_ERR_UNUSE_BEFORE_USE               (150 = 0x96)
OS_ERR_LEAVEREGION_BEFORE_ENTERREGION (151 = 0x97)
OS_ERR_LEAVEINT                       (152 = 0x98)
OS_ERR_DICNT                          (153 = 0x99)
OS_ERR_INTERRUPT_DISABLED             (154 = 0x9A)
OS_ERR_TASK_ENDS_WITHOUT_TERMINATE    (155 = 0x9B)
OS_ERR_RESOURCE_OWNER                 (156 = 0x9C)
                                                 
*** not a legal system call during interrupt *****
OS_ERR_ILLEGAL_IN_ISR                 (160 = 0xA0)

*** not a legal system call during timer *********
OS_ERR_ILLEGAL_IN_TIMER               (161 = 0xA1)

*** not a legal system call outside interrupt ****
OS_ERR_ILLEGAL_OUT_ISR                (162 = 0xA2) 

*** double used data structures **
OS_ERR_2USE_TASK                      (170 = 0xAA)
OS_ERR_2USE_TIMER                     (171 = 0xAB)
OS_ERR_2USE_MAILBOX                   (172 = 0xAC)
OS_ERR_2USE_BSEMA                     (173 = 0xAD)
OS_ERR_2USE_CSEMA                     (174 = 0xAE)
OS_ERR_2USE_RSEMA                     (175 = 0xAF)
OS_ERR_2USE_MEMF                      (176 = 0xB0)

*** Communication errors (OSCOM) **
OS_ERR_NESTED_RX_INT                  (180 = 0xB4)

*** Fixed block memory pool  ***
OS_ERR_MEMF_INV                       (190 = 0xBE)
OS_ERR_MEMF_INV_PTR                   (191 = 0xBF)
OS_ERR_MEMF_PTR_FREE                  (192 = 0xC0)
OS_ERR_MEMF_RELEASE                   (193 = 0xC1)
OS_ERR_POOLADDR                       (194 = 0xC2)
OS_ERR_BLOCKSIZE                      (195 = 0xC3)


*** Task suspend / resume errors ***
OS_ERR_SUSPEND_TOO_OFTEN              (200 = 0xC8)
OS_ERR_RESUME_BEFORE_SUSPEND          (201 = 0xC9)

*** Other task related errors ***
OS_ERR_TASK_PRIORITY                  (202 = 0xCA)

*** Event object ***
OS_ERR_EVENTOBJ_INV                   (210 = 0xD2)
OS_ERR_2USE_EVENTOBJ                  (211 = 0xD3)

*/

void OS_Error(int ErrCode) {
  OS_EnterRegion();     /* Avoid further task switches            */
  OS_DICnt = 0;         /* Allow interrupts so we can communicate */
  OS_EI();
  OS_Status = (OS_U8) ErrCode;
  while (OS_Status);
}

/****** EOF *********************************************************/

