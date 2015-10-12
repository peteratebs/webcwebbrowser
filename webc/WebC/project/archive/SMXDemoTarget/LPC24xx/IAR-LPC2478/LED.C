/*
* LED.C                                                   Version 3.7.1
*
* Routines to write to LEDs on IAR LPC2478 KickStart / Olimex Board.
*
* Copyright (c) 2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author: David Moore
*
***********************************************************************/

#include "smx.h"
#include "core.h"
#include "led.h"

/* Initializes all LED's. */
void InitLEDs(void)
{
   /* no user LEDs */
}


/* Writes hex number (2 digits). */
void Write7SegNum(int num)
{
   (void)num;
   /* no 7-segment LED */
}


/* Writes any pattern passed in, to both LED's (see defines in LED.H). */
void Write7Seg(dword val)
{
   (void)val;
   /* no 7-segment LED */
}


/* Light LEDs in row of LEDs. Bits in val indicate which to light. */
void WriteLEDs(dword val)
{
   (void)val;
   /* no user LEDs */
}


