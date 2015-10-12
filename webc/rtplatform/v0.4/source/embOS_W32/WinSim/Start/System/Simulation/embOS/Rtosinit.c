/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
**********************************************************************

File    : RTOSInit.c   (for embOS Simulation on Windows 32bit)
Purpose : Initializes and handles the hardware for embOS as far
          as required by embOS.
          Feel free to modify this file acc. to your
          target system.
--------- END-OF-HEADER ----------------------------------------------
*/

#include "RTOS.H"
#define _WIN32_WINNT 0x0400   // Required for windows.h
#include <windows.h>
#include <winbase.h>


/*********************************************************************
*
*       Local (static) functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _ISRTickThread()
*
*/
static void _ISRTickThread(void) {
  int tDiffMax = 5;
  int tDiff, t;
  int tLast = 0;

  while (1) {
    t = timeGetTime();
    tDiff = t - tLast;
    tLast = t;
    //
    // Limit the time to a certain amount of ms so that single
    // stepping does not mess things up to badly
    //
    if (tDiff > tDiffMax) {
      tDiff = tDiffMax;
    }
    //
    // Execute the actual ISR one time for every ms
    //
    while (tDiff > 0) {
      OS_EnterInterrupt();
      OS_HandleTickDI();
      OS_LeaveInterrupt();
      tDiff--;
    }
    SleepEx(1, TRUE);
  }
}

/*********************************************************************
*
*       voidAPC()
*
* Function description
*   Dummy APC function. Is required because we (ab)use the
*   WIN32 QueueUserAPC() API function to wake up a thread
*/
static void APIENTRY _voidAPC(DWORD Dummy) {}

static void CALLBACK _CbSignalTickProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
  QueueUserAPC(_voidAPC, (void*)dwUser, 0);
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       OS_InitHW()
*
*   Initialize the hardware required for the OS to run.
*   In a simulation environment, all we do is create an ISR thread for
*   the timer tick.
*/
void OS_InitHW(void) {
  HANDLE hISRThread;
  hISRThread = (HANDLE)OS_SIM_CreateISRThread(_ISRTickThread);
  timeSetEvent(1,0, _CbSignalTickProc,  (int)hISRThread, (TIME_PERIODIC | TIME_CALLBACK_FUNCTION));         
}

/*********************************************************************
*
*       OS_GetTime_Cycles()
*
*   This routine is required for task-info via embOSView.
*   or high precision time measurements.
*   It returns the system time in clock cycles.
*   Time measured in cycles can not be simulated.
*/

OS_U32 OS_GetTime_Cycles(void) { return 0;}


/*********************************************************************
*
*       OS_ConvertCycles2us()
*
* Purpose:
*   Convert Cycles into micro seconds.
*   Dummy in simulation
*/

OS_U32 OS_ConvertCycles2us(OS_U32 Cycles) {
  return 0;
}

/*********************************************************************
*
*       Communication for embOSView not implemented
*
**********************************************************************
*/

void OS_COM_Init(void) {}
void OS_COM_Send1(OS_U8 c) {
  OS_USEPARA(c);
  OS_COM_ClearTxActive();     /* Free OS transmitter buffer */
}

/****** EOF *********************************************************/


