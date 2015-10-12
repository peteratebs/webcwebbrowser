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
#include "dc_type.h"

#define TX_DISABLE_ERROR_CHECKING 1
#include "tx_api.h"
// #include "rtk32.h" // Oasis

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
unsigned long rtp_get_system_msec (void) // Oasis
{
    return(tx_time_get()) ;
}


/*----------------------------------------------------------------------*
                         rtp_get_system_sec
 *----------------------------------------------------------------------*/
unsigned long rtp_get_system_sec () // Oasis
{
    return(tx_time_get()/1000) ;
}

/*----------------------------------------------------------------------*
                         rtp_get_system_sec
 *----------------------------------------------------------------------*/
unsigned long rtp_get_ticks_p_sec () // Oasis
{
    UINT32 ticks, utime=1;
    
    dc_utime_get_from_tick(&utime);

    /* compute number of time slices to sleep */
    ticks = (1000*1000)/utime;

	return(ticks);
}

int rtp_get_gmt_date (RTP_DATE * gmDate, unsigned long utcSec)
{
    return(0);
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
