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
File    : LED.c
Purpose : Helper functions to drive LEDs on eva board
---------------------------END-OF-HEADER------------------------------
*/

#include "HW_LED.h"
#include "SIM.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static int     _LEDs;
  
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
void HW_LED_Init(void);

/*********************************************************************
*
*       HW_LED_Set0
*/
void LED_SetLED0(void) {
  _LEDs |= (1 << 0);
}

/*********************************************************************
*
*       HW_LED_Clr0
*/
void HW_LED_Clr0(void) {
  _LEDs &= ~(1 << 0);
}

/*********************************************************************
*
*       HW_LED_Toggle0
*/
void HW_LED_Toggle0(void) {
  _LEDs ^= (1 << 0);
}

/*********************************************************************
*
*       HW_LED_Set1
*/
void LED_SetLED1(void) {
  _LEDs |= (1 << 1);
}

/*********************************************************************
*
*       HW_LED_Clr1
*/
void HW_LED_Clr1(void) {
  _LEDs &= ~(1 << 1);
}

/*********************************************************************
*
*       HW_LED_Toggle1
*/
void HW_LED_Toggle1(void) {
  _LEDs ^= (1 << 1);
}

/*********************************************************************
*
*       HW_LED_Set2
*/
void LED_SetLED2(void) {
  _LEDs |= (1 << 2);
}

/*********************************************************************
*
*       HW_LED_Clr2
*/
void HW_LED_Clr2(void) {
  _LEDs &= ~(1 << 2);
}

/*********************************************************************
*
*       HW_LED_Toggle2
*/
void HW_LED_Toggle2(void) {
  _LEDs ^= (1 << 2);
}

/*********************************************************************
*
*       HW_LED_Set3
*/
void LED_SetLED3(void) {
  _LEDs |= (1 << 3);
}

/*********************************************************************
*
*       HW_LED_Clr3
*/
void HW_LED_Clr3(void) {
  _LEDs &= ~(1 << 3);
}

/*********************************************************************
*
*       HW_LED_Toggle3
*/
void HW_LED_Toggle3(void) {
  _LEDs ^= (1 << 3);
}

/*********************************************************************
*
*       LED_ShowByte
*/
void HW_LED_ShowByte(unsigned char Data) {
  _LEDs = Data;
}

/*********************************************************************
*
*       LED_GetState
*
* For simulation purposes only
*/
char HW_LED_GetState(unsigned Index) {
  return _LEDs & (1 << Index);
}

/*************************** end of file ****************************/
