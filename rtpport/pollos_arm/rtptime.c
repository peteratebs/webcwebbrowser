 /*
 | RTPTIME.C - Runtime Platform Timing Services
 |
 |   PORTED TO THE EBSnet Inc RTIP TCP/IP PLATFORM
 |
 | EBSnet - RT-Platform
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
#include "rtpport.h"

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
unsigned long rtp_get_ticks_p_sec (void);

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                         rtp_get_system_msec
 *----------------------------------------------------------------------*/
unsigned long rtp_get_system_msec (void)
{
#if (MS_FRAMEWORK)
    UINT64  currentTime;
    
    /* convert usec to msec */
    currentTime = Time_CurrentTime() / 1000;
    
    return ((unsigned long)(currentTime));

#else
    static unsigned long x = 0;
    return(x++);
#endif
    
}


/*----------------------------------------------------------------------*
                         rtp_get_system_sec
 *----------------------------------------------------------------------*/
unsigned long rtp_get_system_sec ()
{
    /* convert msec to sec */
    return((unsigned long) (rtp_get_system_msec() /1000));  
}

/*----------------------------------------------------------------------*
                         rtp_get_system_sec
 *----------------------------------------------------------------------*/
unsigned long rtp_get_ticks_p_sec ()
{
#if (MS_FRAMEWORK)
    return CPU_TicksPerSecond();
#else
    return (1000);
#endif
}

int rtp_get_gmt_date (RTP_DATE * gmDate, unsigned long utcSec)
{
#if (MS_FRAMEWORK)
    SSL_GetTime( (DATE_TIME_INFO*) gmDate );
    
    return (1);
#else
    return (1);
#endif
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */

