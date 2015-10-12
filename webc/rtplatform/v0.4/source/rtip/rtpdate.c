 /*
 | RTPDATE.C - Runtime Platform Date Services
 |
 |   PORTED TO THE EBSnet Inc RTIP TCP/IP PLATFORM
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/14 18:33:37 $
 |  $Name:  $
 |  $Revision: 1.6 $
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
#include "rtpdutil.h"
#include "rtpdebug.h"

#include <rtip.h>
#include <timeapi.h>

/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/
typedef struct _rtp_timeBase
{
    RTP_DATE       referenceDate;
    unsigned long  seconds;
}
rtp_timeBase;

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
	xn_ebs_set_time (
			0, 
			(int) date->year, 
			(int) date->month, 
			(int) date->day, 
			(int) date->hour,
			(int) date->minute,
			(int) date->second
		);
		
    return (0);
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
RTP_TIMESTAMP timestamp;
EBSTIME time;

	if (xn_ebs_get_system_time(&time) < 0)
    {
		RTP_DEBUG_OUTPUT_STR("rtp_get_system_time: error returned.");
        return (-1);
    }

	timestamp.year   = time.year;
	timestamp.second = time.second;
	
	rtp_timestamp_to_date(date, &timestamp);
    return (0);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
