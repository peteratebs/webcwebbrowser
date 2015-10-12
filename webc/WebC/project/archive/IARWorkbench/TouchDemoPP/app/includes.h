/***************************************************************************
 **
 **
 **    Master inlude file
 **
 **    Used with ARM IAR C/C++ Compiler
 **
 **    (c) Copyright IAR Systems 2007
 **
 **    $Revision: 26684 $
 **
 ***************************************************************************/

#ifndef __INCLUDES_H
#define __INCLUDES_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <intrinsics.h>
#include <assert.h>
#include <nxp/iolpc2478.h>


#ifdef __cplusplus
extern "C" {
#endif
#include "arm_comm.h"

#include "board.h"

#include "iar_logo.h"
#include "cursor_arrow.h"

#include "sys.h"
#include "sdram_32M_16bit_drv.h"
#include "drv_glcd.h"
#include "drv_touch_scr.h"

#include "rtos.h"
#include "myRTOS.h"

#ifdef __cplusplus
}
#endif

#endif  // __INCLUDES_H
