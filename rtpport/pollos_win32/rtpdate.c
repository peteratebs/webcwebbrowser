 /*
 | RTPDATE.C - Runtime Platform Date Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - EBS_RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/11/09 22:03:36 $
 |  $Name:  $
 |  $Revision: 1.2 $
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
#include "rtpdate.h"
#include "rtpdebug.h"

#include <time.h>

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

/*----------------------------------------------------------------------
Function:

    Set the current system date.

Summary:

    #include �rtpdate.h�
    int rtp_set_date (RTP_DATE *date)

        RTP_DATE *date - The new system date to be set.

Description:

    Sets the system date to the new date passed in.

Returns:

    Returns 0 if successful, -1 on failure.  For debugging purposes;
    if the cause of the error is obtainable at the native System Date
    layer, turn on RTP_DEBUG in rtpdebug.h to display the
    native error value.
----------------------------------------------------------------------*/

int rtp_set_date (RTP_DATE * date)
{
    /* ----------------------------------- */
    /*      Not supported by windows.      */
            /* NOT YET IMPLEMENTED */
    /* ----------------------------------- */
    return (0);
}


/*----------------------------------------------------------------------
Function:

    Retrieve the current system date.

Summary:

    #include �rtpdate.h�
    int rtp_get_date (RTP_DATE *date)

        RTP_DATE *date - Storage location for the current system date.

Description:

    Retrieves the current system date and returns it in the
    storage location.

Returns:

    Returns 0 if successful, -1 on failure.  For debugging purposes;
    if the cause of the error is obtainable at the native System Date
    layer, turn on RTP_DEBUG in rtpdebug.h to display the
    native error value.
----------------------------------------------------------------------*/

int rtp_get_date (RTP_DATE *date)
{
struct tm *winTime;
time_t timer;

    time (&timer);
    winTime = localtime ((const time_t *)&timer);
    if (winTime == NULL)
    {
        RTP_DEBUG_OUTPUT_STR("rtp_get_system_time: error returned.\n");
        return (-1);
    }

    (*date).year   = winTime->tm_year + 1900;     /* year, AD                   */
    (*date).month  = winTime->tm_mon + 1;         /* 1 = January, 12 = December */
    (*date).day    = winTime->tm_mday;            /* 1 = first of the month     */
    (*date).hour   = winTime->tm_hour;            /* 0 = midnight, 12 = noon    */
    (*date).minute = winTime->tm_min;             /* minutes past the hour      */
    (*date).second = winTime->tm_sec;             /* seconds in minute          */
	(*date).msec    = 0;							  /* milliseconds in second     */

    /* These two fields help interpret  */
    /* the absolute time meaning of the */
    /* above values.                    */
    (*date).dlsTime  = winTime->tm_isdst;         /* boolean; daylight savings time is in effect - always 0 for gmtime */
    (*date).tzOffset = _timezone;                 /* signed int; difference in seconds imposed by timezone (from GMT)  */

    return (0);
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
