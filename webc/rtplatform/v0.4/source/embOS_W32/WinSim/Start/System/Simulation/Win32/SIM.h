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
File        : App.h
Purpose     : 
---------------------------END-OF-HEADER------------------------------
*/
#ifndef SIM_H
#define SIM_H

#include <windows.h>

void SIM_Paint(HDC hDC);
int  SIM_Init (HWND hMainWindow);
void SIM_Update(void);

#endif   /* Avoid multiple inclusion */

/*************************** end of file ****************************/
