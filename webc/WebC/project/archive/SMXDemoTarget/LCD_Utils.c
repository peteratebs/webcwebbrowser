#include "includes.h"

///// HAULED IN FROM IAR POWERPAC DEMO ////////////////////////////////


void BSP_Init()
{
  Int32U cursor_x = (C_GLCD_H_SIZE - CURSOR_H_SIZE)/2, cursor_y = (C_GLCD_V_SIZE - CURSOR_V_SIZE)/2;
  
  // GLCD init
  GLCD_Init (NULL, NULL);

  GLCD_Cursor_Dis(0);

  GLCD_Copy_Cursor ((Int32U *)Cursor, 0, sizeof(Cursor)/sizeof(Int32U));

  GLCD_Cursor_Cfg(CRSR_FRAME_SYNC | CRSR_PIX_32);

  GLCD_Move_Cursor(cursor_x, cursor_y);

  GLCD_Cursor_En(0);

  // Init touch screen
  TouchScrInit();

  // Touched indication LED
  USB_H_LINK_LED_SEL = 0; // GPIO
  USB_H_LINK_LED_FSET = USB_H_LINK_LED_MASK;
  USB_H_LINK_LED_FDIR |= USB_H_LINK_LED_MASK;

  GLCD_Ctrl (TRUE);

#if 0
  GLCD_SetFont(&Terminal_18_24_12,0x0000FF,0x000cd4ff);
  GLCD_SetWindow(95,10,255,33);

  GLCD_SetWindow(55,195,268,218);
  GLCD_TextSetPos(0,0);
  printf("\fTouch screen demo");
  
  GLCD_LoadPicTransparent ((C_GLCD_H_SIZE-IAR.H_Size)/2, 10, &IAR, 0x00ffffff);
  GLCD_LoadPicTransparent ((C_GLCD_H_SIZE-PowerPac.H_Size)/2, IAR.V_Size+20, &PowerPac, 0x00ffffff);
#endif
}


