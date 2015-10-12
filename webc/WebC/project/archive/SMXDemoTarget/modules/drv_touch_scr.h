/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2008
 *
 *    File name   : drv_touch_scr.h
 *    Description : Touch screen driver include file
 *
 *    History :
 *    1. Date        : August, 8 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 24636 $
 **************************************************************************/
#include "includes.h"

#ifndef __DRV_TOUCH_SCR_H
#define __DRV_TOUCH_SCR_H

#define TS_INTR_PRIORITY  2
#define TS_SETUP_DLY      50     // 100us
#define TS_SAMPLE_DLY     10000   // 10ms
#define TS_INIT_DLY       50000   // 50ms
#define TS_SAMPLES        32
#define IMAGE_WIDTH       C_GLCD_H_SIZE
#define IMAGE_HEIGHT      C_GLCD_V_SIZE

typedef union
{
  Int32U XY;
  struct
  {
    Int16U  X;
    Int16U  Y;
  };
} ToushRes_t;

/*************************************************************************
 * Function Name: TimerIntr_Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Sample timer interrupt handler
 *
 *************************************************************************/
void TimerIntr_Handler (void);

/*************************************************************************
 * Function Name: OnTouchIntr_Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: On touch interrupt handler
 *
 *************************************************************************/
void OnTouchIntr_Handler (void);

/*************************************************************************
 * Function Name: TouchScrInit
 * Parameters: none
 *
 * Return: none
 *
 * Description: Init Touch screen
 *
 *************************************************************************/
void TouchScrInit (void);

/*************************************************************************
 * Function Name: TouchScrGetStatus
 * Parameters: ToushRes_t * pData X,Y data
 *
 * Return: Int32U 0 - untouched
 *                1 - touched
 *
 * Description: Return current state of the touchscreen
 *
 *************************************************************************/
Boolean TouchGet (ToushRes_t * pData);

#endif // __DRV_TOUCH_SCR_H
