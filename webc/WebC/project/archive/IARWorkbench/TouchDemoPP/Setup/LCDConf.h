/*********************************************************************
*  
*   IAR PowerPac
*
*   (c) Copyright IAR Systems 2008.  All rights reserved.
*
**********************************************************************
----------------------------------------------------------------------
File        : LCDConf.h
Purpose     : LCD configuration for Olimex LPC-2478STK

----------------------------------------------------------------------
*/

#ifndef LCDCONF_H
#define LCDCONF_H

#define LCD_ALLOW_NON_OPTIMIZED_MODE 0

/*********************************************************************
*
*       Common display controller configuration
*/
#define LCD_CONTROLLER            32168
#define LCD_BITSPERPIXEL            16
#define LCD_ENDIAN_BIG               0

/*********************************************************************
*
*       Display orientation
*/
#define LCD_MIRROR_Y                 0
#define LCD_SWAP_XY                  0

/*********************************************************************
*
*       Optional configuration macros
*/
void LCD_X_On(void);
void LCD_X_Off(void);
void LCD_X_SetLUTEntry(unsigned Pos, unsigned long Color);

#define LCD_ON()                      LCD_X_On()
#define LCD_OFF()                     LCD_X_Off()
#define LCD_SET_LUT_ENTRY(Pos, Color) LCD_X_SetLUTEntry(Pos, Color)

#endif /* LCDCONF_H */
