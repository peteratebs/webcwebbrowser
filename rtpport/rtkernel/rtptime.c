 /*
 | RTPTIME.C - Runtime Platform Timing Services
 |
 |   PORTED TO THE EBSnet Inc RTIP TCP/IP PLATFORM
 |
 | EBSnet - EBS_RT-Platform
 |
 |  $Author: yohannes $
 |  $Date: 2005/06/14 16:06:52 $
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
#include "rtpdate.h"
#include "rtk32.h"

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
int rtp_get_gmt_date (RTP_DATE * gmDate, unsigned long utcSec);
unsigned long rtp_get_ticks_p_sec (void);

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                         rtp_get_system_msec
 *----------------------------------------------------------------------*/
unsigned long rtp_get_system_msec (void)
{
unsigned long temp;
	
	temp = (unsigned long)CLKSecondsToTicks(1); /* returns ticks/second */
	
	return(1000*RTKGetTime()/temp);   
	
}


/*----------------------------------------------------------------------*
                         rtp_get_system_sec
 *----------------------------------------------------------------------*/
unsigned long rtp_get_system_sec ()
{
    unsigned long temp;
	
	temp = rtp_get_ticks_p_sec(); /* returns ticks/second */
	
	return(RTKGetTime()/temp);  
}

/*----------------------------------------------------------------------*
                         rtp_get_system_sec
 *----------------------------------------------------------------------*/
unsigned long rtp_get_ticks_p_sec ()
{
	return((unsigned long)CLKSecondsToTicks(1));
}

int rtp_get_gmt_date (RTP_DATE * gmDate, unsigned long utcSec)
{
  return (1);
}

/*----------------------------------------------------------------------*
                         rtp_get_system_ticks
 *----------------------------------------------------------------------*/
unsigned long rtp_get_system_ticks(void)
{
    return(rtp_get_ticks_p_sec()*rtp_get_system_sec());
}
/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
