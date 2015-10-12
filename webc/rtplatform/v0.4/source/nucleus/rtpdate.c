 /*
 | RTPDATE.C - Runtime Platform Date Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: tony $
 |  $Date: 2005/04/01 15:40:15 $
 |  $Name:  $
 |  $Revision: 1.7 $
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
#include "rtpdate.h"
#include "tri_types.h"
#include "rtc.h"

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

    #include “rtp.h”
    #include “rtpdate.h”
    int rtp_set_date (RTP_DATE *date)

        RTP_DATE *date - The new system date to be set.

Description:

    Sets the system date to the new date passed in.

Returns:

    Returns 0 if successful, -1 on failure.  For debugging purposes;
    if the cause of the error is obtainable at the native System Date
    layer, turn on RTP_DEBUG in rtpdate.c to display the
    native error value.
----------------------------------------------------------------------*/

int rtp_set_date (RTP_DATE * date)
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------
Function:

    Retrieve the current system date.

Summary:

    #include “rtp.h”
    #include “rtpdate.h”
    int rtp_get_date (RTP_DATE *date)

        RTP_DATE *date - Storage location for the current system date.

Description:

    Retrieves the current system date and returns it in the
    storage location.

Returns:

    Returns 0 if successful, -1 on failure.  For debugging purposes;
    if the cause of the error is obtainable at the native System Date
    layer, turn on RTP_DEBUG in rtpdate.c to display the
    native error value.
----------------------------------------------------------------------*/

int rtp_get_date (RTP_DATE *date)
{
extern RTC_DATA now;

	date->year = now.year;
	date->month = now.month;
	date->day = now.day;
	date->hour = now.hour;
	date->minute = now.minute;
	date->second = now.second;
	date->msec = 0;
	date->dlsTime = 0;
	date->tzOffset = 0;

    return (0);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
