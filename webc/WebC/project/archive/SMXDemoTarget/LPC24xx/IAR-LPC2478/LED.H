/*
* LED.H                                                   Version 3.7.1
*
* Defines for LEDs on IAR LPC2478 KickStart / Olimex Board.
*
* Copyright (c) 2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author:  Wan Xingsheng
*
***********************************************************************/

#ifndef SMX_LED_H
#define SMX_LED_H

#define BOARD_LED_NUM_7SEG    0
#define BOARD_LED_NUM_IN_ROW  0


/* Patterns */

#define LED_PATTERN_NONE     0x0
#define LED_PATTERN_ALL      0x0
#define LED_PATTERN_ODD      0x0
#define LED_PATTERN_EVEN     0x0

#ifdef __cplusplus
 extern "C" {
#endif

void InitLEDs(void);
#if (BOARD_LED_NUM_7SEG)
void Write7Seg(dword val);
void Write7SegNum(int num);
#endif
#if (BOARD_LED_NUM_IN_ROW)
void WriteLEDs(dword val);
#endif

#ifdef __cplusplus
 }
#endif

#endif /* SMX_LED_H */
