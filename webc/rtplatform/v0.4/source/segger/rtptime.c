 /*
 | RTPTIME.C - Runtime Platform Timing Services
 |
 |   PORTED TO THE LINUX PLATFORM
 |
 | EBSnet - RT-Platform
 |
 |  $Author: tony $
 |  $Date: 2005/01/25 17:11:59 $
 |  $Name:  $
 |  $Revision: 1.1 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/



/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"
#include "rtptime.h"

#include "rtos.h"

//#include <sys/time.h>

/************************************************************************
* Defines
************************************************************************/

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
unsigned long rtp_get_system_msec (void)
{
	return OS_GetTime32();
}


/*----------------------------------------------------------------------*
                         rtp_get_system_sec
 *----------------------------------------------------------------------*/
unsigned long rtp_get_system_sec (void)
{
	return (OS_GetTime32() / 1000);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
