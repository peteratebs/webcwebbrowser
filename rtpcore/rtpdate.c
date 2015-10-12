 /*
 | RTPDATE.C - Runtime Platform Date Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform
 |
 | Copyright EBSnet Inc. , 2004
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
 | 
 |  NOTE: This module should only be included if there is not a version
 |        in rtpport.
*/



/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"
#include "rtpdate.h"
#include "rtptime.h"

/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/
typedef struct _rtp_timeBase
{
    RTP_DATE        referenceDate;
    unsigned long   syncTime;
}
rtp_timeBase;

/************************************************************************
* Data
************************************************************************/
static rtp_timeBase baseReference;

static const short month_start[] = 
{
	0,    /* January */
	31,   /* February */
	59,   /* March */
	90,   /* April */
	120,  /* May */
	151,  /* June */
	181,  /* July */
	212,  /* August */
	243,  /* September */
	273,  /* October */
	304,  /* November */
	334,  /* December */
	365   /* January (of next year) */
};

static const short month_start_leap[] =
{
	0,    /* January */
	31,   /* February */
	60,   /* March */
	91,   /* April */
	121,  /* May */
	152,  /* June */
	182,  /* July */
	213,  /* August */
	244,  /* September */
	274,  /* October */
	305,  /* November */
	335,  /* December */
	366   /* January (of next year) */
};

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/
static int _is_leap_year (short year);

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                             rtp_set_date
 *----------------------------------------------------------------------*/
/** @memo   Set the current system date.

    @doc    Sets the system date to the new date passed in.

    @return 0 on success. -1 otherwise.
 */
int rtp_set_date (
  RTP_DATE * date               /** The new system date to be set. */
  )
{
    baseReference.syncTime = rtp_get_system_sec();
    baseReference.referenceDate.year     = date->year;
    baseReference.referenceDate.month    = date->month;
    baseReference.referenceDate.day      = date->day;
    baseReference.referenceDate.hour     = date->hour;
    baseReference.referenceDate.minute   = date->minute;
    baseReference.referenceDate.second   = date->second;
    baseReference.referenceDate.msec     = date->msec;
    baseReference.referenceDate.dlsTime  = date->dlsTime;
    baseReference.referenceDate.tzOffset = date->tzOffset;

	return (0);
}


/*----------------------------------------------------------------------*
                            rtp_get_date
 *----------------------------------------------------------------------*/
/** @memo   Retrieve the current system date.

    @doc    Retrieves the current system date and returns it in the
    storage location.

    @precondition The baseReference must refer to a valid date.

    @return 0 on success. -1 otherwise.
 */
int rtp_get_date (
  RTP_DATE *date                /** Storage location for the current system date. */
  )
{
unsigned long deltaSecs;

	*date = baseReference.referenceDate;

    deltaSecs = rtp_get_system_sec() - baseReference.syncTime;
    
    if (deltaSecs > 0)
    {
		date->second += deltaSecs;  
		date->second %= 60;    
		deltaSecs /= 60;

	    if (deltaSecs > 0)
	    {
			date->minute += deltaSecs;  
			date->minute %= 60;    
			deltaSecs /= 60;

		    if (deltaSecs > 0)
		    {
				date->hour += deltaSecs;  
				date->hour %= 24;    
				deltaSecs /= 24;

			    if (deltaSecs > 0)
			    {
			    	int total_days;
			    	
					date->day += deltaSecs;  
					date->day %= 24;    
					deltaSecs /= 24;
					
					if (_is_leap_year(date->year))
					{
						total_days = month_start_leap[date->month] + date->day + deltaSecs;
					}
					else
					{
						total_days = month_start[date->month] + date->day + deltaSecs;
					}
					
					while (1)
					{
						if (_is_leap_year(date->year))
						{
							if (total_days < 366)
							{
								break;
							}							
							total_days -= 366;
						}
						else
						{
							if (total_days < 365)
							{
								break;
							}
							total_days -= 365;
						}
						
						date->year++;
					}

					if (_is_leap_year(date->year))
					{
						for (date->month=0; 
						     date->month < 11 && month_start_leap[date->month + 1] < total_days; 
						     date->month++) 
						{;}
						
						date->day = total_days - month_start_leap[date->month] + 1;
					}
					else
					{
						for (date->month=0; 
						     date->month < 11 && month_start[date->month + 1] < total_days; 
						     date->month++) 
						{;}
						
						date->day = total_days - month_start[date->month] + 1;
					}
					
					date->month++;					
			    }
		    }
	    }
	}

	return (0);
}

/*---------------------------------------------------------------------------*/
int _is_leap_year (short year)
{
	return ((year & 0x3) == 0 && ((year % 100 != 0) || (year % 400 == 0)));
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
