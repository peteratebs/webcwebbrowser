/*
|  RTPDOBJ.C -
|
|  EBSnet -
|
|   $Author: vmalaiya $
|   $Date: 2005/01/07 15:18:48 $
|   $Name:  $
|   $Revision: 1.1 $
|
|  Copyright EBSnet Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/
#include "rtpdobj.h"
#include "rtpmem.h"
#include "rtpstr.h"
#include <rtfsapi.h>

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
/*****************************************************************************/
static int _rtp_ertfsdate_to_date (DATESTR *ertfsdate, RTP_DATE *rtpdate);
static int _rtp_date_to_ertfsdate (DATESTR *ertfsdate, RTP_DATE *rtpdate);
static char *     _rtp_ertfsname_to_name  (char * name);
static char *     _rtp_name_to_ertfsname  (const char * name);

/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*----------------------------------------------------------------------*
                              rtp_file_gfirst
*----------------------------------------------------------------------*/

int rtp_file_gfirst (void ** dirobj, char * name)
{
DSTAT *fdata;

#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

    fdata = (DSTAT*) rtp_malloc(sizeof(DSTAT));
    memset(fdata, 0, sizeof(DSTAT));
    
	name = _rtp_name_to_ertfsname (name);

	if (!pc_gfirst (fdata, name))
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
		rtp_term_cputs("rtp_file_gfirst: error returned ");
		rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		rtp_free (fdata);
		return (-1);
	}
	*dirobj = fdata;
	return (0);
}


/*----------------------------------------------------------------------*
                              rtp_file_gnext
*----------------------------------------------------------------------*/
int rtp_file_gnext (void * dirobj)
{
#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	if (!pc_gnext ((DSTAT *)dirobj))
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
		rtp_term_cputs("rtp_file_gnext: error returned ");
		rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}

	return (0);
}


/*----------------------------------------------------------------------*
                              rtp_file_gdone
*----------------------------------------------------------------------*/
void rtp_file_gdone (void * dirobj)
{
	pc_gdone ((DSTAT *)dirobj);
	rtp_free (dirobj);
}


/*----------------------------------------------------------------------*
                              rtp_file_get_size
*----------------------------------------------------------------------*/
int rtp_file_get_size (void * dirobj, unsigned long * size)
{
#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	if (!dirobj)
	{
#ifdef RTP_DISPLAY_ERROR
		rtfs_set_errno (PEINVALIDPARMS);
		result = get_errno ();
		rtp_term_cputs("rtp_file_get_size: error returned ");
		rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}

	*size = ((DSTAT *)dirobj)->fsize;
	return (0);
}


/*----------------------------------------------------------------------*
                              rtp_file_get_attrib
*----------------------------------------------------------------------*/
int rtp_file_get_attrib (void * dirobj, unsigned char * attributes)
{
#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	if (!dirobj)
	{
#ifdef RTP_DISPLAY_ERROR
		rtfs_set_errno (PEINVALIDPARMS);
		result = get_errno ();
		rtp_term_cputs("rtp_file_get_attrib: error returned ");
		rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}

	*attributes = (((DSTAT *)dirobj)->fattribute & ADIRENT) ? RTP_FILE_ATTRIB_ISDIR : 0;
	*attributes |= (((DSTAT *)dirobj)->fattribute & ARDONLY) ? RTP_FILE_ATTRIB_RDONLY : RTP_FILE_ATTRIB_RDWR;
	*attributes |= (((DSTAT *)dirobj)->fattribute & ARCHIVE) ? RTP_FILE_ATTRIB_ARCHIVE : 0;
	*attributes |= (((DSTAT *)dirobj)->fattribute & AHIDDEN) ? RTP_FILE_ATTRIB_HIDDEN : 0;
	*attributes |= (((DSTAT *)dirobj)->fattribute & ASYSTEM) ? RTP_FILE_ATTRIB_SYSTEM : 0;
	*attributes |= (((DSTAT *)dirobj)->fattribute & AVOLUME) ? RTP_FILE_ATTRIB_ISVOL : 0;

	return (0);
}


/*----------------------------------------------------------------------*
                              rtp_file_get_name
*----------------------------------------------------------------------*/
int rtp_file_get_name (void * dirobj, char * name, int size)
{
int sizelimit;

#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	if (!dirobj)
	{
#ifdef RTP_DISPLAY_ERROR
		rtfs_set_errno (PEINVALIDPARMS);
		result = get_errno ();
		rtp_term_cputs("rtp_file_get_name: error returned ");
		rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}

#if (VFAT)
	sizelimit = rtp_strlen(((DSTAT *)dirobj)->lfname);
	if (sizelimit > size)
	{
		sizelimit = size;
	}
	strncpy (name, (const char *) (((DSTAT *)dirobj)->lfname), sizelimit);
#else
	sizelimit = strlen(((DSTAT *)dirobj)->filename);
	if (sizelimit > size)
	{
		sizelimit = size;
	}
	strncpy (name, (const char *) (((DSTAT *)dirobj)->filename), sizelimit);
#endif

	return (0);
}


/*----------------------------------------------------------------------*
                              rtp_file_get_time
*----------------------------------------------------------------------*/
int rtp_file_get_time (void * dirobj, RTP_DATE * adate, RTP_DATE * wdate, RTP_DATE * cdate, RTP_DATE * hdate)
{
DATESTR ertfsdate;
RTP_DATE tmpdate;

#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	if (!dirobj)
	{
		return (-1);
	}

	ertfsdate.date = ((DSTAT *)dirobj)->fdate;
	ertfsdate.time = ((DSTAT *)dirobj)->ftime;

	if ( _rtp_ertfsdate_to_date (&ertfsdate, &tmpdate) < 0)
	{
		return (-1);
	}

	if (adate)
	{
		rtp_memcpy (adate, &tmpdate, sizeof(RTP_DATE));
	}
	if (wdate)
	{
		rtp_memcpy (wdate, &tmpdate, sizeof(RTP_DATE));
	}
	if (cdate)
	{
		rtp_memcpy (cdate, &tmpdate, sizeof(RTP_DATE));
	}
	if (hdate)
	{
		/* ----------------------------------- */
		/* Not supported by the ertfs fs. */
		/* rtp_not_yet_implemented (); */
		/* ----------------------------------- */
	}

	return (0);
}

/************************************************************************
* Utility Function Bodies
************************************************************************/
/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
int _rtp_ertfsdate_to_date (DATESTR *ertfsdate, RTP_DATE *rtpdate)
{
	if (!ertfsdate)
	{
		rtpdate = ((RTP_DATE *)0);
		return(-1);
	}

	/* ---------------------------------- */
	/* year month day */
	/* date 0000000 0000 00000 */
	/* ---------------------------------- */

	rtpdate->year = (ertfsdate->date >> 9) & 0xff;
	rtpdate->month = (ertfsdate->date >> 5) & 0xf;
	rtpdate->day = ertfsdate->date & 0x1f;

	/* ---------------------------------- */
	/* hour minute sec */
	/* time 00000 000000 00000 */
	/* ---------------------------------- */

	rtpdate->hour = (ertfsdate->time >> 11) & 0x1f;
	rtpdate->minute = (ertfsdate->time >> 5) & 0x3f;
	rtpdate->second = ertfsdate->time & 0x1f;

	rtpdate->dlsTime = 0; /* always 0 for gmtime */
	rtpdate->tzOffset = 0;

	return (0);
}


/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
int _rtp_date_to_ertfsdate (DATESTR * ertfsdate, RTP_DATE * rtpdate)
{

	if (!rtpdate)
	{
		ertfsdate = (DATESTR *)0;
		return (-1);
	}

	/* ---------------------------------- */
	/* hour minute sec */
	/* time 00000 000000 00000 */
	/* ---------------------------------- */

	ertfsdate->time = (word) ( (rtpdate->hour << 11) |
	(rtpdate->minute << 5) |
	rtpdate->second );

	/* ---------------------------------- */
	/* year month day */
	/* date 0000000 0000 00000 */
	/* ---------------------------------- */

	ertfsdate->date = (word) ( (rtpdate->year << 9) |
	(rtpdate->month << 5) |
	rtpdate->day );

	return (0);
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static char * _rtp_ertfsname_to_name (char * name)
{
char * p;

	if (!name)
	{
		return ((char *) 0);
	}

	for (p = name; *p; p++)
	{
		switch (*p)
		{
			case '\\':
				*p = '/';
				continue;

			default:
			break;
		}
	}
	return (name);
}


/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static char * _rtp_name_to_ertfsname (const char * name)
{
char * p;

	if (!name)
	{
		return ((char *)0);
	}

	for (p = (char *)name; *p; p++)
	{
		switch (*p)
		{
			case '/':
				*p = '\\';
				continue;

			default:
				break;
		}
	}
	return ((char *)name);
}

/* ----------------------------------- */
/* END OF FILE */
/* ----------------------------------- */

