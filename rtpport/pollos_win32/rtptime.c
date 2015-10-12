 /*
 | RTPTIME.C - Runtime Platform Timing Services
 |
 |   PORTED TO THE WIN32 PLATFORM
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
#include "rtptime.h"
#include "rtpdate.h"
#include <windows.h>


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
/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                         rtp_get_system_msec
 *----------------------------------------------------------------------*/
unsigned long rtp_get_system_msec (void)
{
unsigned long elapsed_msec;

    /* get seconds then multiply by 1000 to get msec */
    elapsed_msec = (GetTickCount()/rtp_get_ticks_p_sec()) * 1000;
    return (elapsed_msec);
}


/*----------------------------------------------------------------------*
                         rtp_get_system_sec
 *----------------------------------------------------------------------*/
unsigned long rtp_get_system_sec (void)
{
unsigned long elapsed_sec;

    elapsed_sec = GetTickCount()/rtp_get_ticks_p_sec();
    return (elapsed_sec);
}

/*----------------------------------------------------------------------*
                         rtp_get_system_sec
 *----------------------------------------------------------------------*/
unsigned long rtp_get_ticks_p_sec (void)
{
    return(1000);
}

int rtp_get_gmt_date (RTP_DATE * gmDate, unsigned long utcSec)
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	gmDate->year		= st.wYear;
	gmDate->month		= st.wMonth;
	gmDate->day			= st.wDay;
	gmDate->hour		= st.wHour;
	gmDate->minute		= st.wMinute;	
	gmDate->second		= st.wSecond;
	gmDate->msec		= st.wMilliseconds;
	gmDate->dlsTime		= 0;
	gmDate->tzOffset	= -4;
	return (1);
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
