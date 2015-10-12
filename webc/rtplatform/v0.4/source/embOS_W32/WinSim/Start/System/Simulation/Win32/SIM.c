/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*         (C) 1996-2004 SEGGER Microcontroller Systeme GmbH          *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : SIM.c
Purpose     : Collection of function to simulate the look & feel
              of the target device
---------------------------END-OF-HEADER------------------------------
*/

#include <stdio.h>
#include "SIM.h"
#include "HW_LED.h"

/*********************************************************************
*
*       static data
*
**********************************************************************
*/
static HWND _hWnd;

/*********************************************************************
*
*       static code, helpers
*
**********************************************************************
*/
/*********************************************************************
*
*       _DrawRect
*/
static void _DrawRect(HDC hDC, int x, int y, int w, int h, COLORREF Color) {
  HBRUSH hBrush;
  HPEN hPen;
  hBrush = CreateSolidBrush(Color);
  hPen = CreatePen(PS_SOLID, 1, Color);
  SelectObject(hDC, hBrush);
  SelectObject(hDC, hPen);
  Rectangle(hDC, x, y, x + w - 1, y + h - 1);
  DeleteObject(hPen);
  DeleteObject(hBrush);
}

/*********************************************************************
*
*       _MixColors
*/
static COLORREF _MixColors(COLORREF Color, COLORREF BkColor, unsigned Intens) {
  /* Calc Color seperations for FgColor first */
  unsigned R = (Color & 0xff0000)* Intens;
  unsigned G = (Color & 0xff00)  * Intens;
  unsigned B = (Color & 0xff)    * Intens;
  /* Add Color seperations for BkColor */
  Intens = 256 - Intens;
  R += (BkColor & 0xff0000) * Intens;
  G += (BkColor & 0xff00)   * Intens;
  B += (BkColor & 0xff)     * Intens;
  R = (R >> 8) & 0xff0000;
  G = (G >> 8) & 0xff00;
  B = (B >> 8);
  return R + G + B;
}

/*********************************************************************
*
*       _DrawLED
*/
static void _DrawLED(HDC hDC, int x, int y, int w, int h, COLORREF Color) {
  int R, G, B, Light;

  R = (Color & 0xFF0000) >> 16;
  G = (Color & 0x00FF00) >>  8;
  B = (Color & 0x0000FF);
  Light = ((R + G + B) > 0xA0) ? 1 : 0;
  _DrawRect(hDC, x,     y,     w,     h,     0);
  _DrawRect(hDC, x + 1, y + 1, w - 2, h - 2, _MixColors(0x000000, Color, 100));
  _DrawRect(hDC, x + 2, y + 2, w - 4, h - 4, _MixColors(0x000000, Color,  50));
  _DrawRect(hDC, x + 2, y + 3, w - 4, h - 6, Color);
  _DrawRect(hDC, x + 2, y + 5, w - 4, h -10, _MixColors(0xFFFFFF, Color, Light ?  90 : 40));
  _DrawRect(hDC, x + 3, y + 7, w - 6, h -14, _MixColors(0xFFFFFF, Color, Light ? 150 : 75));
}

/*********************************************************************
*
*       _cbTimer
*
* Function description
*   This is a timer callback routine.
*   It is periodically called by windows (thru a WM_TIMER message).
*   We use it to periodically invalidate the main window.
*/
void __stdcall _cbTimer (HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime) {
  InvalidateRect(hWnd, NULL, FALSE);
}

/*********************************************************************
*
*       public code
*
**********************************************************************
*/
/*********************************************************************
*
*       SIM_Paint
*
* Function description
*   This function is called from the callback of the main window when
*   handling the WM_PAINT message.
*   It gives the application a chance to draw the variable elements.
*   Per default, these are 8 LEDs on the PCB board.
*   This routine can be modified to show anything your application
*   requires, e.g. a graphical display (LCD).
*/
void SIM_Paint(HDC hDC) {
  int i, x, y, w, h;
  double dx;
  //
  // Position parameters for LEDs. These depend on the image used.
  //
  x  = 213;
  y  = 46;
  w  = 10;
  h  = 20;
  dx = 17.3;
  //
  // Draw LEDs
  //
  for (i = 0; i < 8; i++) {
    COLORREF Color;
    Color = HW_LED_GetState(i) ? 0x00FF80 : 0x005000;    // Select color depending on state of LED
    _DrawLED(hDC, (int)(x + i*dx), y, w, h, Color);
  }
}

/*********************************************************************
*
*       SIM_Init
*
* Function description
*   
*/
int  SIM_Init (HWND hMainWindow) {
  _hWnd = hMainWindow;
  SetTimer(hMainWindow, 0, 20, _cbTimer);
  return 0;      // No problem
}

/*********************************************************************
*
*       SIM_Update
*
* Function description
*   Immidiately updates the entire device on the display
*/
void SIM_Update(void) {
  InvalidateRect(_hWnd, NULL, FALSE);
  UpdateWindow(_hWnd);
}

/*************************** end of file ****************************/
