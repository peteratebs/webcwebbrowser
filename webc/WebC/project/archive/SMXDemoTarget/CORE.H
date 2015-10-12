/*
* CORE.H                                                    Version 3.7.1
*
* CORE HEADER FILE. This file is included in all Protosystem C files.
*
* Copyright (c) 1989-2008 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author: Ralph Moore
*
***********************************************************************/

#ifndef SMX_CORE_H
#define SMX_CORE_H

#include "armdefs.h"    /* ARM definitions */

#if defined(WIN32)
#include "bspw32.h"     /* BSP defines */
#else
#include "bsp.h"        /* BSP defines */
#endif

#include "conf.h"       /* configuration */

#if defined(SMXAWARE)
#include "smxaware.h"
#endif

/* Definitions */

#ifdef SMXNET_DEMO
#define  Ctrl_A    1    /* Ctrl_A starts the server test (smxNet demo) */
#define  Ctrl_B    2    /* Ctrl_B starts the client test (smxNet demo) */
#endif
#define  Esc      27    /* Escape */

#ifdef __cplusplus
 extern "C" {
#endif

/* Externals. in_clib is near because it is frequently referenced. */

extern struct DAR near adar;        /* adar structure */
extern char display[DISPLAY_ROWS_MAX][DISPLAY_COLUMNS_MAX+1];
extern TCB_PTR         exitx;       /* end operation task */
extern SCB_PTR    near in_clib;     /* in C library call */
extern PXCB_PTR        kbd_pipe;    /* generic kbd pipe */
extern PXCB_PTR        op_pipe;     /* operation input pipe */

/* Function Prototypes */

void        appl_init(void);
void        appl_exit(void);
#if defined(WIN32) && (DEVICE_CONSOLE_OUT)
void        show_etime(void);
#endif
void        tick(void);

#if (DEVICE_CONSOLE_OUT)
#define WR_STRING(col, row, F_color, B_color, blinking, in_string) \
   { \
   wr_string(col, row, F_color, B_color, blinking, in_string); \
   }
#else
#define WR_STRING(col, row, F_color, B_color, blinking, in_string)
#endif

#ifdef __cplusplus
}
#endif

#endif /* SMX_CORE_H */

