/*************************************************************************
 *
*    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2008
 *
 *    File name   : drv_glcd.h
 *    Description : Graphical LCD driver include file
 *
 *    History :
 *    1. Date        : 6, March 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *
 *    $Revision: 24636 $
 **************************************************************************/
#include "arm_comm.h"

#ifndef __GLCD_DRV_H
#define __GLCD_DRV_H

typedef struct _Bmp_t {
  Int32U  H_Size;
  Int32U  V_Size;
  Int32U  BitsPP;
  Int32U  BytesPP;
  pInt32U pPalette;
  pInt32U pPicStream;
  pInt8U  pPicDesc;
} Bmp_t, *pBmp_t;

typedef struct _FontType_t {
  Int32U H_Size;
  Int32U V_Size;
  Int32U CharacterOffset;
  Int32U CharactersNuber;
  pInt8U pFontStream;
  pInt8U pFontDesc;
} FontType_t, *pFontType_t;

typedef Int32U LdcPixel_t, *pLdcPixel_t;

// pixel clock frequency < 6.4MHz
// ~5.4Mhz = 408 clk per line * (240 line + 3 Vsynch pulse
//           + 5 front porch + 15 back porch)

#define C_GLCD_REFRESH_FREQ     (50HZ)
#define C_GLCD_H_SIZE           320
#define C_GLCD_H_PULSE          30
#define C_GLCD_H_FRONT_PORCH    20
#define C_GLCD_H_BACK_PORCH     38
#define C_GLCD_V_SIZE           240
#define C_GLCD_V_PULSE          3
#define C_GLCD_V_FRONT_PORCH    5
#define C_GLCD_V_BACK_PORCH     15

#define C_GLCD_PWR_ENA_DIS_DLY  10000
#define C_GLCD_ENA_DIS_DLY      10000

#define CRSR_PIX_32     0
#define CRSR_PIX_64     1
#define CRSR_ASYNC      0
#define CRSR_FRAME_SYNC 2

#define TEXT_DEF_TAB_SIZE 5

#define TEXT_BEL1_FUNC()

void GLCD_Init (const Int32U *pPain, const Int32U * pPallete);
void GLCD_SetPallet (const Int32U * pPallete);
void GLCD_Ctrl (Boolean bEna);
void GLCD_Cursor_Cfg(int Cfg);
void GLCD_Cursor_En(int cursor);
void GLCD_Cursor_Dis(int cursor);
void GLCD_Move_Cursor(int x, int y);
void GLCD_Copy_Cursor (const Int32U *pCursor, int cursor, int size);
void GLCD_SetFont(pFontType_t pFont, LdcPixel_t Color, LdcPixel_t BackgndColor);
void GLCD_SetWindow(Int32U X_Left, Int32U Y_Up,
                    Int32U X_Right, Int32U Y_Down);
void GLCD_TextSetPos(Int32U X, Int32U Y);
void GLCD_TextSetTabSize(Int32U Size);
static void LCD_SET_WINDOW (Int32U X_Left, Int32U X_Right,
                            Int32U Y_Up, Int32U Y_Down);
static void LCD_WRITE_PIXEL (Int32U Pixel);
static Boolean GLCD_TextCalcWindow (pInt32U pXL, pInt32U pXR,
                                    pInt32U pYU, pInt32U pYD,
                                    pInt32U pH_Size, pInt32U pV_Size);
void GLCD_LoadPic (Int32U X_Left, Int32U Y_Up, Bmp_t * pBmp, Int32U Mask);
int putchar (int c);


#endif // __GLCD_DRV_H
