/*
|  RTPDOBJ.C -
|
|  EBS -
|
|   $Author:
|   $Date:
|   $Name:
|   $Revision:
|
|  Copyright EBS Inc.
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/
#include "rtpdobj.h"
#include "rtpdebug.h"
#include "rtpstr.h"
#include "rtpmem.h"
#include "rtpprint.h"
#include "rtfs.h"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
*****************************************************************************/

//static int _rtp_windate_to_date (time_t * windate, RTP_DATE * rtpdate);
static RTP_DATE *     _rtp_ertfsdate_to_date  (DATESTR  * ertfsdate);
static DATESTR *      _rtp_date_to_ertfsdate  (RTP_DATE * rtpdate);
static unsigned short _rtp_flag_to_operation  (unsigned short mode);
static unsigned short _rtp_mode_to_permission (unsigned short mode);
static char * _rtp_name_to_ertfsname (const char * name);
static char * _rtp_ertfsname_to_name (char * name);

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
DSTAT *fdata ;
char * temp = name;

	temp = _rtp_name_to_ertfsname (temp);

	fdata = (DSTAT*) rtp_malloc(sizeof(DSTAT));

	if (!pc_gfirst (fdata, name))
	{
#ifdef RTP_DISPLAY_ERROR
		int result;
		result = get_errno ();
		rtp_printf("rtp_file_gfirst: error returned %d\n", result);
#endif
		rtp_free (fdata);
		return (-1);
	}

	//_rtp_ertfs_to_native(&fdata->fsObject, &ertfsNative);
	*dirobj = (void *)fdata;
	return (0);
}


/*----------------------------------------------------------------------*
                             rtp_file_gnext
 *----------------------------------------------------------------------*/
int rtp_file_gnext (void * dirobj)
{
#ifdef RTP_DISPLAY_ERROR
    int result;
#endif

    if (!pc_gnext ((DSTAT *)dirobj))
    {
#ifdef RTP_DISPLAY_ERROR
        result = get_errno() ;
        rtp_printf("rtp_file_gnext: error returned %d\n",result);
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
}



/*----------------------------------------------------------------------*
                            rtp_file_get_size
 *----------------------------------------------------------------------*/
int rtp_file_get_size (void * dirobj, unsigned long * size)
{
#ifdef RTP_DISPLAY_ERROR
    int  result;
#endif

    if (!dirobj)
    {
#ifdef RTP_DISPLAY_ERROR
        result = get_errno() ;
        rtp_printf("rtp_file_get_size: error returned %d\n", result);
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
    int  result;
#endif

    if (!dirobj)
    {
#ifdef RTP_DISPLAY_ERROR
        result = get_errno ();
        rtp_printf("rtp_file_get_attrib: error returned %d\n", result);
#endif
        return (-1);
    }

    *attributes  = (((DSTAT *)dirobj)->fattribute & ADIRENT) ? RTP_FILE_ATTRIB_ISDIR   : 0;
    *attributes |= (((DSTAT *)dirobj)->fattribute & ARDONLY) ? RTP_FILE_ATTRIB_RDONLY  : RTP_FILE_ATTRIB_RDWR;
    *attributes |= (((DSTAT *)dirobj)->fattribute & ARCHIVE) ? RTP_FILE_ATTRIB_ARCHIVE : 0;
    *attributes |= (((DSTAT *)dirobj)->fattribute & AHIDDEN) ? RTP_FILE_ATTRIB_HIDDEN  : 0;
    *attributes |= (((DSTAT *)dirobj)->fattribute & ASYSTEM) ? RTP_FILE_ATTRIB_SYSTEM  : 0;
    *attributes |= (((DSTAT *)dirobj)->fattribute & AVOLUME) ? RTP_FILE_ATTRIB_ISVOL   : 0;

    return (0);
}


/*----------------------------------------------------------------------*
                           rtp_file_get_name
 *----------------------------------------------------------------------*/
int rtp_file_get_name (void * dirobj, char * name, int size)
{
unsigned int sizelimit;

#ifdef RTP_DISPLAY_ERROR
    int  result;
#endif

    if (!dirobj)
    {
#ifdef RTP_DISPLAY_ERROR
        result = get_errno ();
       	rtp_printf("rtp_file_get_name: error returned %d\n", result);
#endif
        return (-1);
    }

#if (VFAT)
    sizelimit = rtp_strlen(((DSTAT *)dirobj)->lfname);
	if(size > 0)
	{
		if (sizelimit > (unsigned int)(size - 1))
		{
			sizelimit = size - 1;
		}
	}
    rtp_strncpy (name, (const char *) (((DSTAT *)dirobj)->lfname), sizelimit);
#else
    sizelimit = strlen(((DSTAT *)dirobj)->filename);
    if(size > 0)
	{
		if (sizelimit > (unsigned int)(size - 1))
		{
			sizelimit = size - 1;
		}
	}
    strncpy (name, (const char *) (((DSTAT *)dirobj)->filename), sizelimit);
#endif
	name[sizelimit] = '\0';
    return (0);
}




/*----------------------------------------------------------------------*
                           rtp_file_get_time
 *----------------------------------------------------------------------*/
int rtp_file_get_time (void * dirobj, RTP_DATE * adate, RTP_DATE * wdate, RTP_DATE * cdate, RTP_DATE * hdate)
{
DATESTR ertfsdate;
RTP_DATE * tmpdate;

    if (!dirobj)
    {
        return (-1);
    }

    ertfsdate.date = ((DSTAT *)dirobj)->fdate;
    ertfsdate.time = ((DSTAT *)dirobj)->ftime;

    if ( !(tmpdate = _rtp_ertfsdate_to_date (&ertfsdate)) )
    {
        return (-1);
    }

    if (adate)
    {
        memcpy (adate, tmpdate, sizeof(RTP_DATE));
    }
    if (wdate)
    {
        memcpy (wdate, tmpdate, sizeof(RTP_DATE));
    }
    if (cdate)
    {
        memcpy (cdate, tmpdate, sizeof(RTP_DATE));
    }
    if (hdate)
    {
        /* ----------------------------------- */
        /*   Not supported by the ertfs fs.    */
        /*     rtp_not_yet_implemented ()      */
        /* ----------------------------------- */
    }

    return (0);
}


/***********************************************************/
static RTP_DATE * _rtp_ertfsdate_to_date (DATESTR *ertfsdate)
{
static RTP_DATE rtpdate;

    if (!ertfsdate)
    {
        return ((RTP_DATE *)NULL);
    }

    /* ---------------------------------- */
    /*              year   month      day */
    /* date      0000000    0000    00000 */
    /* ---------------------------------- */

    rtpdate.year   = (ertfsdate->date >> 9)  & 0xff;
    rtpdate.month  = (ertfsdate->date >> 5)  & 0xf;
    rtpdate.day    =  ertfsdate->date        & 0x1f;

    /* ---------------------------------- */
    /*              hour  minute      sec */
    /* time        00000  000000    00000 */
    /* ---------------------------------- */

    rtpdate.hour   = (ertfsdate->time >> 11) & 0x1f;
    rtpdate.minute = (ertfsdate->time >> 5)  & 0x3f;
    rtpdate.second =  ertfsdate->time        & 0x1f;

    rtpdate.dlsTime  = 0;   /* always 0 for gmtime */
    rtpdate.tzOffset = 0;

    return (&rtpdate);
}




/************************************************************************
* Utility Function Bodies
************************************************************************/



/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static char * _rtp_name_to_ertfsname (const char * name)
{
char * p;
char * ertfsname;

ertfsname = (char *) name;

	if (!ertfsname)
	{
		return ((char *)0);
	}

	for (p = ertfsname; *p; p++)
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
	return (ertfsname);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */

