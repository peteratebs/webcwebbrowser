/*************************************************************************
 *
*    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2008
 *
 *    File name   : drv_glcd.c
 *    Description : Graphical LCD driver
 *
 *    History :
 *    1. Date        : 6, March 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *
 *    $Revision: 24636 $
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "board.h"
#include "sys.h"
#include "drv_glcd.h"

#define C_GLCD_CLK_PER_LINE     (C_GLCD_H_SIZE + C_GLCD_H_PULSE + C_GLCD_H_FRONT_PORCH + C_GLCD_H_BACK_PORCH)
#define C_GLCD_LINES_PER_FRAME  (C_GLCD_V_SIZE + C_GLCD_V_PULSE + C_GLCD_V_FRONT_PORCH + C_GLCD_V_BACK_PORCH)
#define C_GLCD_PIX_CLK          (6.4MHZ)

#define LCD_CURSOR_BASE_ADDR ((Int32U)0xFFE10800)

static pFontType_t pCurrFont = NULL;
static LdcPixel_t TextColour;
static LdcPixel_t TextBackgndColour;

static Int32U TextX_Pos = 0;
static Int32U TextY_Pos = 0;

static Int32U XL_Win = 0;
static Int32U YU_Win = 0;
static Int32U XR_Win = C_GLCD_H_SIZE-1;
static Int32U YD_Win = C_GLCD_V_SIZE-1;

static Int32U TabSize = TEXT_DEF_TAB_SIZE;

static Int32U WindY_Size, WindX_Size;
static Int32U CurrY_Size, CurrX_Size;
static pInt32U pWind;
static pInt32U pPix;

unsigned char LCD_VRAM_BASE_ADDR[(320*240*4)+16];
unsigned char LCD_VRAM_BASE_ADDR2[(320*240*4)+16];

unsigned long getVideoBaseAddr(int which) // get an address with the proper alignment
{
  unsigned long retVal;
//  if(which)
//	  retVal = (unsigned long)LCD_VRAM_BASE_ADDR2 + 8 - ((unsigned long)LCD_VRAM_BASE_ADDR2 % 8);
//  else
	  retVal = (unsigned long)LCD_VRAM_BASE_ADDR + 8 - ((unsigned long)LCD_VRAM_BASE_ADDR % 8);
  return retVal;
}

/*************************************************************************
 * Function Name: GLCD_Cursor_Cnfg
 * Parameters:
 *
 * Return: none
 *
 * Description: Configure the cursor
 *
 *************************************************************************/
void GLCD_Cursor_Cfg(int Cfg)
{
  CRSR_CFG = Cfg;
}
/*************************************************************************
 * Function Name: GLCD_Cursor_En
 * Parameters: cursor - Cursor Number
 *
 * Return: none
 *
 * Description: Enable Cursor
 *
 *************************************************************************/
void GLCD_Cursor_En(int cursor)
{
  CRSR_CTRL_bit.CrsrNum = cursor;
  CRSR_CTRL_bit.CrsrOn = 1;
}

/*************************************************************************
 * Function Name: GLCD_Cursor_Dis
 * Parameters: None
 *
 * Return: none
 *
 * Description: Disable Cursor
 *
 *************************************************************************/
void GLCD_Cursor_Dis(int cursor)
{
  CRSR_CTRL_bit.CrsrOn = 0;
}

/*************************************************************************
 * Function Name: GLCD_Move_Cursor
 * Parameters: x - cursor x position
 *             y - cursor y position
 *
 * Return: none
 *
 * Description: Moves cursor on position (x,y). Negativ values are posible.
 *
 *************************************************************************/
void GLCD_Move_Cursor(int x, int y)
{
  if(0 <= x)
  {//no clipping
    CRSR_CLIP_bit.CrsrClipX = 0;
    CRSR_XY_bit.CrsrX = x;
  }
  else
  {//clip x
    CRSR_CLIP_bit.CrsrClipX = -x;
    CRSR_XY_bit.CrsrX = 0;
  }

  if(0 <= y)
  {//no clipping
    CRSR_CLIP_bit.CrsrClipY = 0;
    CRSR_XY_bit.CrsrY = y;
  }
  else
  {//clip y
    CRSR_CLIP_bit.CrsrClipY = -y;
    CRSR_XY_bit.CrsrY = 0;
  }
}

/*************************************************************************
 * Function Name: GLCD_Copy_Cursor
 * Parameters: pCursor - pointer to cursor conts image
 *             cursor - cursor Number (0,1,2 or 3)
 *                      for 64x64(size 256) pix cursor always use 0
 *             size - cursor size in words
 * Return: none
 *
 * Description: Copy Cursor from const image to LCD RAM image
 *
 *************************************************************************/
void GLCD_Copy_Cursor (const Int32U *pCursor, int cursor, int size)
{
   Int32U * pDst = (Int32U *)LCD_CURSOR_BASE_ADDR;
   pDst += cursor*64;

   for(int i = 0; i < size ; i++) *pDst++ = *pCursor++;
}
/*************************************************************************
 * Function Name: GLCD_Init
 * Parameters: const Int32U *pPain, const Int32U * pPallete
 *
 * Return: none
 *
 * Description: GLCD controller init
 *
 *************************************************************************/
void GLCD_Init (const Int32U *pPain, const Int32U * pPallete)
{
pInt32U pDst = (pInt32U) getVideoBaseAddr(0);
  // Assign pin
  PINSEL0 &= BIN32(11111111,11110000,00000000,11111111);
  PINSEL0 |= BIN32(00000000,00000101,01010101,00000000);
  PINMODE0&= BIN32(11111111,11111100,00000000,11111111);
  PINMODE0|= BIN32(00000000,00000010,10101010,00000000);
  PINSEL3 &= BIN32(11110000,00000000,00000000,11111111);
  PINSEL3 |= BIN32(00000101,01010101,01010101,00000000);
  PINMODE3&= BIN32(11110000,00000000,00000000,11111111);
  PINMODE3|= BIN32(00001010,10101010,10101010,00000000);
  PINSEL4 &= BIN32(11110000,00110000,00000000,00000000);
  PINSEL4 |= BIN32(00000101,01001111,11111111,11111111);
  PINMODE4&= BIN32(11110000,00110000,00000000,00000000);
  PINMODE4|= BIN32(00001010,10001010,10101010,10101010);
  PINSEL9 &= BIN32(11110000,11111111,11111111,11111111);
  PINSEL9 |= BIN32(00001010,00000000,00000000,00000000);
  PINMODE9&= BIN32(11110000,11111111,11111111,11111111);
  PINMODE9|= BIN32(00001010,00000000,00000000,00000000);
  PINSEL11&= BIN32(11111111,11111111,11111111,11110000);
  PINSEL11|= BIN32(00000000,00000000,00000000,00001111);
  // Init GLCD cotroller
  PCONP_bit.PCLCD = 1;      // enable LCD controller clock
  CRSR_CTRL_bit.CrsrOn = 0; // Disable cursor
  LCD_CTRL_bit.LcdEn = 0;   // disable GLCD controller
  LCD_CTRL_bit.LcdBpp= 5;   // 24 bpp
  LCD_CTRL_bit.LcdTFT= 1;   // TFT panel
  LCD_CTRL_bit.LcdDual=0;   // single panel
  LCD_CTRL_bit.BGR   = 0;   // notmal output
  LCD_CTRL_bit.BEBO  = 0;   // little endian byte order
  LCD_CTRL_bit.BEPO  = 0;   // little endian pix order
  LCD_CTRL_bit.LcdPwr= 0;   // disable power
  // init pixel clock
  LCD_CFG_bit.CLKDIV =  SYS_GetFsclk() / (Int32U)C_GLCD_PIX_CLK;
  LCD_POL_bit.BCD    = 1;   // bypass inrenal clk divider
  LCD_POL_bit.CLKSEL = 0;   // clock source for the LCD block is HCLK
  LCD_POL_bit.IVS    = 1;   // LCDFP pin is active LOW and inactive HIGH
  LCD_POL_bit.IHS    = 1;   // LCDLP pin is active LOW and inactive HIGH
  LCD_POL_bit.IPC    = 1;   // data is driven out into the LCD on the falling edge
  LCD_POL_bit.IOE    = 0;   // active high
  LCD_POL_bit.CPL    = C_GLCD_H_SIZE-1;
  // init Horizontal Timing
  LCD_TIMH_bit.HBP   =  C_GLCD_H_BACK_PORCH - 1;
  LCD_TIMH_bit.HFP   =  C_GLCD_H_FRONT_PORCH - 1;
  LCD_TIMH_bit.HSW   =  C_GLCD_H_PULSE - 1;
  LCD_TIMH_bit.PPL   = (C_GLCD_H_SIZE/16) - 1;
  // init Vertical Timing
  LCD_TIMV_bit.VBP   =  C_GLCD_V_BACK_PORCH;
  LCD_TIMV_bit.VFP   =  C_GLCD_V_FRONT_PORCH;
  LCD_TIMV_bit.VSW   =  C_GLCD_V_PULSE;
  LCD_TIMV_bit.LPP   =  C_GLCD_V_SIZE - 1;
  // Frame Base Address doubleword aligned
  LCD_UPBASE         =  getVideoBaseAddr(0) & ~7UL ;
  LCD_LPBASE         =  getVideoBaseAddr(0) & ~7UL ;
  // init colour pallet

  if(NULL != pPallete)
  {
    GLCD_SetPallet(pPallete);
  }

  if (NULL == pPain)
  {
    // clear display memory
    for(Int32U i = 0; (C_GLCD_H_SIZE * C_GLCD_V_SIZE) > i; i++)
    {
      *pDst++ = 0xffffff;
    }
  }
  else
  {
    // set display memory
    for(Int32U i = 0; (C_GLCD_H_SIZE * C_GLCD_V_SIZE) > i; i++)
    {
      *pDst++ = *pPain++;
    }
  }

  for(volatile Int32U i = C_GLCD_ENA_DIS_DLY; i; i--);
}

/*************************************************************************
 * Function Name: GLCD_SetPallet
 * Parameters: const Int32U * pPallete
 *
 * Return: none
 *
 * Description: GLCD init colour pallete
 *
 *************************************************************************/
void GLCD_SetPallet (const Int32U * pPallete)
{
pInt32U pDst = (pInt32U)LCD_PAL_BASE;
  assert(pPallete);
  for (Int32U i = 0; i < 128; i++)
  {
    *pDst++ = *pPallete++;
  }
}

/*************************************************************************
 * Function Name: GLCD_Ctrl
 * Parameters: Boolean bEna
 *
 * Return: none
 *
 * Description: GLCD enable disabe sequence
 *
 *************************************************************************/
void GLCD_Ctrl (Boolean bEna)
{
  if (bEna)
  {
    LCD_CTRL_bit.LcdEn = 1;
    for(volatile Int32U i = C_GLCD_PWR_ENA_DIS_DLY; i; i--);
    LCD_CTRL_bit.LcdPwr= 1;   // enable power
  }
  else
  {
    LCD_CTRL_bit.LcdPwr= 0;   // disable power
    for(volatile Int32U i = C_GLCD_PWR_ENA_DIS_DLY; i; i--);
    LCD_CTRL_bit.LcdEn = 0;
  }
}

/*************************************************************************
 * Function Name: GLCD_SetFont
 * Parameters: pFontType_t pFont, LdcPixel_t Color
 *              LdcPixel_t BackgndColor
 *
 * Return: none
 *
 * Description: Set current font, font color and background color
 *
 *************************************************************************/
void GLCD_SetFont(pFontType_t pFont, LdcPixel_t Color, LdcPixel_t BackgndColor)
{
  pCurrFont = pFont;
  TextColour = Color;
  TextBackgndColour = BackgndColor;
}

/*************************************************************************
 * Function Name: GLCD_SetWindow
 * Parameters: Int32U X_Left, Int32U Y_Up,
 *             Int32U X_Right, Int32U Y_Down
 *
 * Return: none
 *
 * Description: Set draw window XY coordinate in pixels
 *
 *************************************************************************/
void GLCD_SetWindow(Int32U X_Left, Int32U Y_Up,
                    Int32U X_Right, Int32U Y_Down)
{
  assert(X_Right < C_GLCD_H_SIZE);
  assert(Y_Down < C_GLCD_V_SIZE);
  assert(X_Left < X_Right);
  assert(Y_Up < Y_Down);
  XL_Win = X_Left;
  YU_Win = Y_Up;
  XR_Win = X_Right;
  YD_Win = Y_Down;
}

/*************************************************************************
 * Function Name: GLCD_TextSetPos
 * Parameters: Int32U X_UpLeft, Int32U Y_UpLeft,
 *             Int32U X_DownLeft, Int32U Y_DownLeft
 *
 * Return: none
 *
 * Description: Set text X,Y coordinate in characters
 *
 *************************************************************************/
void GLCD_TextSetPos(Int32U X, Int32U Y)
{
  TextX_Pos = X;
  TextY_Pos = Y;
}

/*************************************************************************
 * Function Name: GLCD_TextSetTabSize
 * Parameters: Int32U Size
 *
 * Return: none
 *
 * Description: Set text tab size in characters
 *
 *************************************************************************/
void GLCD_TextSetTabSize(Int32U Size)
{
  TabSize = Size;
}

/*************************************************************************
 * Function Name: LCD_SET_WINDOW
 * Parameters: int c
 *
 * Return: none
 *
 * Description: Put char function
 *
 *************************************************************************/
static
void LCD_SET_WINDOW (Int32U X_Left, Int32U X_Right,
                     Int32U Y_Up, Int32U Y_Down)
{
  pPix = pWind = ((pInt32U)getVideoBaseAddr(0)) + X_Left + (Y_Up*C_GLCD_H_SIZE);
  WindX_Size = X_Right - X_Left;
  WindY_Size = Y_Down - Y_Up;
  CurrX_Size = CurrY_Size = 0;
}

/*************************************************************************
 * Function Name: LCD_SET_WINDOW
 * Parameters: int c
 *
 * Return: none
 *
 * Description: Put char function
 *
 *************************************************************************/
static
void LCD_WRITE_PIXEL (Int32U Pixel)
{
  *pPix++ = Pixel;
  if (++CurrX_Size > WindX_Size)
  {
    CurrX_Size = 0;
    if(++CurrY_Size > WindY_Size)
    {
      CurrY_Size = 0;
    }
    pPix = pWind + CurrY_Size * C_GLCD_H_SIZE;
  }
}

/*************************************************************************
 * Function Name: GLCD_TextCalcWindow
 * Parameters: pInt32U pXL, pInt32U pXR,
 *             pInt32U pYU, pInt32U pYD,
 *             pInt32U pH_Size, pInt32U pV_Size
 *
 * Return: Boolean
 *          FALSE - out of window coordinate aren't valid
 *          TRUE  - the returned coordinate are valid
 *
 * Description: Calculate character window
 *
 *************************************************************************/
static
Boolean GLCD_TextCalcWindow (pInt32U pXL, pInt32U pXR,
                             pInt32U pYU, pInt32U pYD,
                             pInt32U pH_Size, pInt32U pV_Size)
{
  *pH_Size = pCurrFont->H_Size;
  *pV_Size = pCurrFont->V_Size;
  *pXL = XL_Win + (TextX_Pos*pCurrFont->H_Size);
  if(*pXL > XR_Win)
  {
    return(FALSE);
  }
  *pYU = YU_Win + (TextY_Pos*pCurrFont->V_Size);
  if(*pYU > YD_Win)
  {
    return(FALSE);
  }

  *pXR   = XL_Win + ((TextX_Pos+1)*pCurrFont->H_Size) - 1;
  if(*pXR > XR_Win)
  {
  	*pH_Size -= *pXR - XR_Win;
    *pXR = XR_Win;
  }

  *pYD = YU_Win + ((TextY_Pos+1)*pCurrFont->V_Size) - 1;
  if(*pYD > YD_Win)
  {
    *pV_Size -= *pYD - YD_Win;
    *pYD = YD_Win;
  }

  return(TRUE);
}

/*************************************************************************
 * Function Name: putchar
 * Parameters: int c
 *
 * Return: none
 *
 * Description: Put char function
 *
 *************************************************************************/
int putchar (int c)
{
pInt8U pSrc;
Int32U H_Line;
Int32U xl,xr,yu,yd,Temp,V_Size, H_Size, SrcInc = 1;
Int32U WhiteSpaceNumb;
  if(pCurrFont == NULL)
  {
    return(EOF);
  }
  H_Line = (pCurrFont->H_Size / 8) + ((pCurrFont->H_Size % 8)?1:0);
  switch(c)
  {
  case '\n':  // go to begin of next line (NewLine)
    ++TextY_Pos;
    break;
  case '\r':  // go to begin of this line (Carriage Return)
  	// clear from current position to end of line
  	while(GLCD_TextCalcWindow(&xl,&xr,&yu,&yd,&H_Size,&V_Size))
  	{
      LCD_SET_WINDOW(xl,xr,yu,yd);
	    for(Int32U i = 0; i < V_Size; ++i)
	    {
	      for(Int32U j = 0; j < H_Size; ++j)
	      {
	        LCD_WRITE_PIXEL(TextBackgndColour);
	      }
	    }
  		++TextX_Pos;
  	}
    TextX_Pos = 0;
    break;
  case '\b': // go back one position (BackSpace)
    if(TextX_Pos)
    {
      --TextX_Pos;
      // del current position
	  	if(GLCD_TextCalcWindow(&xl,&xr,&yu,&yd,&H_Size,&V_Size))
	  	{
        LCD_SET_WINDOW(xl,xr,yu,yd);
		    for(Int32U i = 0; i < V_Size; ++i)
		    {
		      for(Int32U j = 0; j < H_Size; ++j)
		      {
		        LCD_WRITE_PIXEL(TextBackgndColour);
		      }
		    }
	  	}
    }
    break;
  case '\t':  // go to next Horizontal Tab stop
  	WhiteSpaceNumb = TabSize - (TextX_Pos%TabSize);
  	for(Int32U k = 0; k < WhiteSpaceNumb; ++k)
  	{
      LCD_SET_WINDOW(xl,xr,yu,yd);
	  	if(GLCD_TextCalcWindow(&xl,&xr,&yu,&yd,&H_Size,&V_Size))
	  	{
		    for(Int32U i = 0; i < V_Size; ++i)
		    {
		      for(Int32U j = 0; j < H_Size; ++j)
		      {
		        LCD_WRITE_PIXEL(TextBackgndColour);
		      }
		    }
		    ++TextX_Pos;
	  	}
	  	else
	  	{
	  		break;
	  	}
  	}
    break;
  case '\f':  // go to top of page (Form Feed)
  	// clear entire window
  	H_Size = XR_Win - XL_Win;
  	V_Size = YD_Win - YU_Win;
    // set character window X left, Y right
    LCD_SET_WINDOW(XL_Win,XR_Win,YU_Win,YD_Win);
    // Fill window with background font color
    for(Int32U i = 0; i <= V_Size; ++i)
    {
      for(Int32U j = 0; j <= H_Size; ++j)
      {
        LCD_WRITE_PIXEL(TextBackgndColour);
      }
    }
  	
  	TextX_Pos = TextY_Pos = 0;
    break;
  case '\a':  // signal an alert (BELl)
    TEXT_BEL1_FUNC();
    break;
  default:
    // Calculate the current character base address from stream
    // and the character position
    if((c <  pCurrFont->CharacterOffset) &&
    	 (c >= pCurrFont->CharactersNuber))
   	{
   		c = 0;
    }
    else
    {
    	c -= pCurrFont->CharacterOffset;
    }
    pSrc = pCurrFont->pFontStream + (H_Line * pCurrFont->V_Size * c);
    // Calculate character window and fit it in the text window
    if(GLCD_TextCalcWindow(&xl,&xr,&yu,&yd,&H_Size,&V_Size))
    {
	    // set character window X left, Y right
	    LCD_SET_WINDOW(xl,xr,yu,yd);
	    // Send char data
	    for(Int32U i = 0; i < V_Size; ++i)
	    {
        SrcInc = H_Line;
        for(Int32U j = 0; j < H_Size; ++j)
	      {
	        Temp = (*pSrc & (1UL << (j&0x7)))?TextColour:TextBackgndColour;
	        LCD_WRITE_PIXEL(Temp);
	        if((j&0x7) == 7)
	        {
	          ++pSrc;
            --SrcInc;
	        }
	      }
        // next line of character
	      pSrc += SrcInc;
	    }
    }
    ++TextX_Pos;
  }
  return(c);
}

/*************************************************************************
 * Function Name: GLCD_LoadPic
 * Parameters: Int32U X_Left, Int32U Y_Up, Bmp_t * pBmp
 *
 * Return: none
 *
 * Description: Load picture in VRAM memory area
 *
 *************************************************************************/
void GLCD_LoadPic (Int32U X_Left, Int32U Y_Up, Bmp_t * pBmp, Int32U Mask)
{
pInt32U pData = ((pInt32U) getVideoBaseAddr(0)) + X_Left + (Y_Up * C_GLCD_H_SIZE);
pInt32U pSrc = pBmp->pPicStream;
Int32U X_LeftHold;
  for(Int32U i = 0; i < pBmp->V_Size; i++)
  {
    if(Y_Up++ >= C_GLCD_V_SIZE)
    {
      break;
    }
    for(Int32U j = 0; j < pBmp->H_Size; j++)
    {
      if(X_LeftHold++ >= C_GLCD_H_SIZE)
      {
        pSrc += pBmp->H_Size - j;
        break;
      }
      *(pData+j) = *pSrc++ ^ Mask;
    }
    X_LeftHold = X_Left;
    pData += C_GLCD_H_SIZE;
  }
}

