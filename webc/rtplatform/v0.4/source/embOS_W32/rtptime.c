 /*
 | RTPTIME.C - Runtime Platform Timing Services
 |
 |   PORTED TO THE Segger embOS simulation for Windows 32 bit system
 |                 and Visual C++.
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: tom $
 |  $Date: 2004/10/22 18:11:57 $
 |  $Name:  $
 |  $Revision: 1.3 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |
 |  For an embedded environment (not simulated), timer tick scaling
 |  (ie. number of msec per tick) and timer rollovers must be considered. 
 |  For the simulation, a 1 msec per tick time base is derived from a 
 |  timed Windows task. ( ref. Rtosinit.c) 
*/



/************************************************************************
* Headers
************************************************************************/
#define RTP_INCLUDE_TIME
#include "rtpconfig.h"

/************************************************************************
* Defines
************************************************************************/

#define TICKS_PER_SEC 1000

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                         rtp_get_system_msec
 *----------------------------------------------------------------------*/
/** @memo   Retrieves the elapsed milliseconds since the 
    application started.

    @doc    Retrieves the elapsed milliseconds since the 
    application started. Can be used as a millisecond timer 
    by storing the first call and periodically checking 
    against successive calls until the timeout period has 
    been reached.

    @return The elapsed milliseconds. There is no error return.
 */
unsigned long rtp_get_system_msec (void)
{
    return((unsigned long) OS_GetTime32());
}


/*----------------------------------------------------------------------*
                         rtp_get_system_sec
 *----------------------------------------------------------------------*/
/** @memo   Retrieves the elapsed seconds since the 
    application started.

    @doc    Retrieves the elapsed seconds since the 
    application started. Can be used as a second timer 
    by storing the first call and periodically checking 
    against successive calls until the timeout period 
    has been reached.

    @return The elapsed seconds. There is no error return.
 */
unsigned long rtp_get_system_sec (void)
{
    return ((unsigned long) (OS_GetTime32() / TICKS_PER_SEC));
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
