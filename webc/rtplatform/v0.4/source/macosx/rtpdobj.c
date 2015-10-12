/*
|  RTPDOBJ.C -
| 
|  EBSnet -
| 
|   $Author: tony $
|   $Date: 2005/04/26 18:35:52 $
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
#include "rtpdebug.h"

#include <errno.h>
#include <glob.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

typedef struct NativeFileSystemObj
{
    int currentPath;
	glob_t globdata;
    struct stat statdata;
} FSOBJ;

/*****************************************************************************/
/* Function Prototypes
/*****************************************************************************/
static int _rtp_lindate_to_date (time_t * lindate, RTP_DATE * rtpdate);

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
FSOBJ* linDirObj;
int result;

    linDirObj = (FSOBJ*) malloc(sizeof(FSOBJ));
    memset(linDirObj, 0, sizeof(FSOBJ));

    result = glob((const char *) name, 0, NULL, &(linDirObj->globdata));
    if (result != 0)
    {
        free (linDirObj);
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_gfirst: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    linDirObj->currentPath = 0;
    if (stat (linDirObj->globdata.gl_pathv[linDirObj->currentPath], &linDirObj->statdata) == -1)
    {
        globfree(&linDirObj->globdata);
        free (linDirObj);
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_gfirst: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *dirobj = (void*) linDirObj;
    return (0);
}



/*----------------------------------------------------------------------*
                             rtp_file_gnext
 *----------------------------------------------------------------------*/
int rtp_file_gnext (void * dirobj)
{
    ((FSOBJ *)dirobj)->currentPath++;
    if (((FSOBJ *)dirobj)->currentPath >= ((FSOBJ *)dirobj)->globdata.gl_pathc)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_gnext: error no more files.\n");
#endif
        return (-1);
    }

    if (stat (((FSOBJ *)dirobj)->globdata.gl_pathv[((FSOBJ *)dirobj)->currentPath], &((FSOBJ *)dirobj)->statdata) == -1)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_gnext: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
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
    globfree(&((FSOBJ *)dirobj)->globdata);
	free(dirobj);
}


/*----------------------------------------------------------------------*
                            rtp_file_get_size
 *----------------------------------------------------------------------*/
int rtp_file_get_size (void * dirobj, unsigned long * size)
{
    if (!dirobj || ((FSOBJ *)dirobj)->currentPath >= ((FSOBJ *)dirobj)->globdata.gl_pathc)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_get_size: error invalid dirobj.\n");
#endif
        return (-1);
    }

	if (size)
	{
		*size = (unsigned long) (((FSOBJ *)dirobj)->statdata.st_size);
	}
#ifdef RTP_DEBUG
	else
	{
        RTP_DEBUG_OUTPUT_STR("rtp_file_get_size: error no storage location.\n");
	}
#endif
    return (0);
}


/*----------------------------------------------------------------------*
                           rtp_file_get_attrib
 *----------------------------------------------------------------------*/
int rtp_file_get_attrib (void * dirobj, unsigned char * attributes)
{
    int readable, writable;

    if (!dirobj || ((FSOBJ *)dirobj)->currentPath >= ((FSOBJ *)dirobj)->globdata.gl_pathc)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_get_size: error invalid dirobj.\n");
#endif
        return (-1);
    }

	*attributes  = S_ISDIR (((FSOBJ *)dirobj)->statdata.st_mode) ? RTP_FILE_ATTRIB_ISDIR : 0;
	*attributes |= S_ISBLK (((FSOBJ *)dirobj)->statdata.st_mode) ? RTP_FILE_ATTRIB_ISVOL : 0;

    readable = ((FSOBJ *)dirobj)->statdata.st_mode & S_IRUSR;
    writable = ((FSOBJ *)dirobj)->statdata.st_mode & S_IWUSR;

    if (readable && writable)
        *attributes |= RTP_FILE_ATTRIB_RDWR;
    else if (readable)
        *attributes |= RTP_FILE_ATTRIB_RDONLY;
    else if (writable)
        *attributes |= RTP_FILE_ATTRIB_WRONLY;

    return (0);
}


/*----------------------------------------------------------------------*
                           rtp_file_get_name
 *----------------------------------------------------------------------*/
int rtp_file_get_name (void * dirobj, char * name, int size)
{
unsigned int sizelimit;
const char *end;

    if (!dirobj || ((FSOBJ *)dirobj)->currentPath >= ((FSOBJ *)dirobj)->globdata.gl_pathc)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_get_name: error invalid dirobj.\n");
#endif
        return (-1);
    }

    if (size < 1)
    {
        return (0);
    }

    end = strrchr(((FSOBJ *)dirobj)->globdata.gl_pathv[((FSOBJ *)dirobj)->currentPath], '/');
    if (end)
    {
        end++;
        sizelimit = strlen(end);
        if (sizelimit > (unsigned int)(size - 1))
        {
            sizelimit = size - 1;
        }
        strncpy(name, end, sizelimit);
    	name[sizelimit] = '\0';
    }
    else
        name[0] = '\0';
    return (0);
}



/*----------------------------------------------------------------------*
                           rtp_file_get_time
 *----------------------------------------------------------------------*/
int rtp_file_get_time (void * dirobj, RTP_DATE * adate, RTP_DATE * wdate, RTP_DATE * cdate, RTP_DATE * hdate)
{
    if (!dirobj || ((FSOBJ *)dirobj)->currentPath >= ((FSOBJ *)dirobj)->globdata.gl_pathc)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_get_name: error invalid dirobj.\n");
#endif
        return (-1);
    }

    if (adate)
    {
        if (_rtp_lindate_to_date(&((FSOBJ *)dirobj)->statdata.st_atime, adate) != 0)
        {
            return (-1);
        }
    }
    if (wdate)
    {
        if (_rtp_lindate_to_date(&((FSOBJ *)dirobj)->statdata.st_mtime, wdate) != 0)
        {
            return (-1);
        }
    }
    if (cdate)
    {
        /* ----------------------------------- */
        /*  Not supported by the Linux fs.     */
        /*  Use write date to avoid bogus      */
        /*  data.                              */
        /* ----------------------------------- */
        if (_rtp_lindate_to_date(&((FSOBJ *)dirobj)->statdata.st_mtime, cdate) != 0)
        {
            return (-1);
        }
    }
    if (hdate)
    {
        if (_rtp_lindate_to_date(&((FSOBJ *)dirobj)->statdata.st_ctime, hdate) != 0)
        {
            return (-1);
        }
    }
    return (0);
}

/************************************************************************
* Utility Function Bodies
************************************************************************/

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static int _rtp_lindate_to_date (time_t * lindate, RTP_DATE * rtpdate)
{
struct tm * ptime;

    ptime = localtime((const time_t *)lindate);

    if (!ptime)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("_rtp_lindate_to_date: error returned.\n");
#endif
        return (-1);
    }

    (*rtpdate).year   = ptime->tm_year + 1900;
    (*rtpdate).month  = ptime->tm_mon + 1;
    (*rtpdate).day    = ptime->tm_mday;
    (*rtpdate).hour   = ptime->tm_hour;
    (*rtpdate).minute = ptime->tm_min;
    (*rtpdate).second = ptime->tm_sec;
	(*rtpdate).msec   = 0;

    (*rtpdate).dlsTime  = ptime->tm_isdst;    /* always 0 for gmtime */
    (*rtpdate).tzOffset = timezone;

    return (0);
}
