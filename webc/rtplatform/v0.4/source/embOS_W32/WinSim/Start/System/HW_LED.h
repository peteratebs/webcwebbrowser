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
File    : HW_LED.h
Purpose : Helper functions to drive LEDs on eva board
---------------------------END-OF-HEADER------------------------------
*/

#ifndef HW_LED_H
#define HW_LED_H
  
void HW_LED_Init(void);

void HW_LED_Set0(void);
void HW_LED_Clr0(void);
void HW_LED_Toggle0(void);

void HW_LED_Set1(void);
void HW_LED_Clr1(void);
void HW_LED_Toggle1(void);

void HW_LED_Set2(void);
void HW_LED_Clr2(void);
void HW_LED_Toggle2(void);

void HW_LED_Set3(void);
void HW_LED_Clr3(void);
void HW_LED_Toggle3(void);

void HW_LED_Set4(void);
void HW_LED_Clr4(void);
void HW_LED_Toggle4(void);

void HW_LED_Set5(void);
void HW_LED_Clr5(void);
void HW_LED_Toggle5(void);

void HW_LED_Set6(void);
void HW_LED_Clr6(void);
void HW_LED_Toggle6(void);

void HW_LED_Set7(void);
void HW_LED_Clr7(void);
void HW_LED_Toggle7(void);

void HW_LED_ShowByte(unsigned char data);

char HW_LED_GetState(unsigned Index);      /* For simulation purposes only */

#endif        /* Avoid multiple inclusion */

/*************************** end of file ****************************/
