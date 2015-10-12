 /*
 | RTPWFILE.C - Runtime Platform Network Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: vmalaiya $
 |  $Date: 2006/04/28 17:52:28 $
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
#include "rtpfile.h"
#include "rtpmem.h"
#include "rtpdebug.h"

#include <io.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <direct.h>
#include <sys/stat.h>
#include <windows.h>
#include <string.h>
#include <malloc.h>
#include <wchar.h>
#include <ctype.h>


/************************************************************************
* Defines
************************************************************************/
#define _WIN32_WINNT 0x0401  //temp 
/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Structures
************************************************************************/
typedef struct NativeFileSystemObj
{
    long                         handle;
    struct _wfinddata_t          fsObject;
} WFSOBJ;


/************************************************************************
* Data
************************************************************************/

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Utility Function Prototypes
************************************************************************/
static int               _rtp_windate_to_date         (time_t * windate, RTP_DATE * rtpdate);
static int               _rtp_date_to_windate         (RTP_DATE * rtpdate, time_t * windate);
static unsigned short *  _rtp_unicode_winname_to_name (unsigned short * name);
static unsigned short *  _rtp_unicode_name_to_winname (unsigned short * name);
static int               _rtp_flag_to_operation       (unsigned short mode);
static int               _rtp_mode_to_permission      (unsigned short mode);
extern unsigned int      _rtp_flag_was_created        (unsigned short flag);   

/************************************************************************
* Function Bodies
************************************************************************/


/*----------------------------------------------------------------------*
                               rtp_wfile_open
 *----------------------------------------------------------------------*/
int rtp_wfile_open (RTP_HANDLE  * fdPtr, const unsigned short * name, unsigned short flag, unsigned short mode)
{

#if (_WIN32_WINNT) >= 0x0400

long fileHandle;
int result;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError(0);
#endif

    name = (unsigned short *)_rtp_unicode_name_to_winname ((unsigned short *) name);
    fileHandle = (long) _wopen (name, _rtp_flag_to_operation(flag), _rtp_mode_to_permission(mode));

    if (fileHandle == (-1))
    {
        result = GetLastError();                
        /* ----------------------------------- */
        /*  If trying to open a directory or   */
        /*  opening a read only file with      */
        /*  write privilages.  This can be     */
        /*  non-fatal if doing an open to      */
        /*  determine the existance of a       */
        /*  directory.                         */
        /* ----------------------------------- */
        if (result == ERROR_ACCESS_DENIED)
        {
#ifdef RTP_DEBUG
            RTP_DEBUG_OUTPUT_STR("rtp_wfile_open: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(result);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_wfile_open: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    *fdPtr = (RTP_HANDLE) fileHandle;
    return (0);

#endif
	return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_rename
 *----------------------------------------------------------------------*/
int rtp_wfile_rename (unsigned short * name, unsigned short * newname)
{
#if (_WIN32_WINNT) >= 0x0400
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    name    = _rtp_unicode_name_to_winname (name);
    newname = _rtp_unicode_name_to_winname (newname);

    if (_wrename ((const unsigned short *)name, (const unsigned short *)newname) != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_wfile_rename: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
#endif
	return (-1);
}



/*----------------------------------------------------------------------*
                             rtp_wfile_delete
 *----------------------------------------------------------------------*/
int rtp_wfile_delete (unsigned short * name)
{
#if (_WIN32_WINNT) >= 0x0400

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    name = _rtp_unicode_name_to_winname (name);

    if (_wremove ((const unsigned short *)name) != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_wfile_delete: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);

#endif
	return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_mkdir
 *----------------------------------------------------------------------*/
int rtp_wfile_mkdir (unsigned short * name)
{
#if (_WIN32_WINNT) >= 0x0400

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    name = _rtp_unicode_name_to_winname (name);

    if (_wmkdir ((const unsigned short *)name) != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_wfile_mkdir: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
#endif
	return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_rmdir
 *----------------------------------------------------------------------*/
int rtp_wfile_rmdir (unsigned short * name)
{
#if (_WIN32_WINNT) >= 0x0400

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    name = _rtp_unicode_name_to_winname (name);

    if (_wrmdir ((const unsigned short *)name) != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_wfile_rmdir: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
#endif
	return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_setcwd
 *----------------------------------------------------------------------*/
int rtp_wfile_setcwd (unsigned short * name)
{
#if (_WIN32_WINNT) >= 0x0400
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    name = _rtp_unicode_name_to_winname (name);

    if (_wchdir ((const unsigned short *)name) != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_wfile_setcwd: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
#endif
	return (-1);
}



/*----------------------------------------------------------------------*
                             rtp_wfile_pwd
 *----------------------------------------------------------------------*/
int rtp_wfile_pwd (unsigned short * name, long size)
{
#if (_WIN32_WINNT) >= 0x0400
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (_wgetcwd (name, (int)size) == NULL)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_wfile_pwd: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    name = _rtp_unicode_winname_to_name (name);
    return (0);
#endif
	return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_gfirst
 *----------------------------------------------------------------------*/
int rtp_wfile_gfirst (void ** dirobj, unsigned short * name)
{
#if (_WIN32_WINNT) >= 0x0400
WFSOBJ* winDirObj;

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    winDirObj = (WFSOBJ*) malloc(sizeof(WFSOBJ));
    memset(winDirObj, 0, sizeof(WFSOBJ));

    name = _rtp_unicode_name_to_winname(name);
    winDirObj->fsObject.attrib = _A_NORMAL | _A_SUBDIR;
    winDirObj->handle = _wfindfirst((const unsigned short *) name, &(winDirObj->fsObject));
    if (winDirObj->handle == (long)(-1))
    {
        rtp_free (winDirObj);
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_wfile_gfirst: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *dirobj = (void*) winDirObj;
    return (0);
#endif
	return (-1);
}



/*----------------------------------------------------------------------*
                             rtp_wfile_gnext
 *----------------------------------------------------------------------*/
int rtp_wfile_gnext (void * dirobj)
{
#if (_WIN32_WINNT) >= 0x0400
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (_wfindnext (((WFSOBJ *)dirobj)->handle, &(((WFSOBJ *)dirobj)->fsObject)) != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_wfile_gnext: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    return (0);
#endif
	return (-1);
}



/*----------------------------------------------------------------------*
                           rtp_wfile_get_name
 *----------------------------------------------------------------------*/
int rtp_wfile_get_name (void * dirobj, unsigned short * name, int size)
{
#if (_WIN32_WINNT) >= 0x0400
unsigned int sizelimit;

#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (!dirobj)
    {
#ifdef RTP_DEBUG
        SetLastError(ERROR_INVALID_HANDLE);
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_wfile_get_name: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    sizelimit = wcslen(((WFSOBJ *)dirobj)->fsObject.name);
    if (size > 0)
    {
        if (sizelimit > (unsigned int)(size - 1))
        {
            sizelimit = size - 1;
        }
    }
    wcsncpy(name, (const unsigned short *) (((WFSOBJ *)dirobj)->fsObject.name), sizelimit);
	name[sizelimit] = '\0';

    name = _rtp_unicode_winname_to_name(name);
    return (0);
#endif
	return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_wfile_chmode
 *----------------------------------------------------------------------*/
int rtp_wfile_chmode (unsigned short * name, unsigned char attributes)
{
#if (_WIN32_WINNT) >= 0x0400
int winmode;

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    winmode  = 0;
    winmode |= (attributes & RTP_FILE_ATTRIB_RDONLY)  ? FILE_ATTRIBUTE_READONLY  : 0;
    winmode |= (attributes & RTP_FILE_ATTRIB_ARCHIVE) ? FILE_ATTRIBUTE_ARCHIVE   : 0;
    winmode |= (attributes & RTP_FILE_ATTRIB_HIDDEN)  ? FILE_ATTRIBUTE_HIDDEN    : 0;
    winmode |= (attributes & RTP_FILE_ATTRIB_SYSTEM)  ? FILE_ATTRIBUTE_SYSTEM    : 0;
    
	/* ----------------------------------- */
    /*  If no attributes are specified,    */
	/*  set to normal, which is 0x80, not  */
	/*  0x00                               */
    /* ----------------------------------- */
	if (winmode == 0)  
	{
		winmode = FILE_ATTRIBUTE_NORMAL;
	}

        name = _rtp_unicode_name_to_winname (name);

    if (SetFileAttributesW((const unsigned short *)name, winmode) < 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_wfile_chmode: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
#endif
	return (-1);
}



/*----------------------------------------------------------------------*
                           rtp_wfile_get_free
 *----------------------------------------------------------------------*/
int rtp_wfile_get_free (unsigned short * name, unsigned long *total, unsigned long *free,
                       unsigned long *sectors_per_unit, unsigned short *bytes_per_sector)
{
#if (_WIN32_WINNT) >= 0x0400
struct _diskfree_t dtable_entry = {0};
int tmpDrive;
int result;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    name = _rtp_unicode_name_to_winname (name);
	tmpDrive = towlower((int) name[0]);
	tmpDrive = (tmpDrive - 'a') + 1;
    result = _getdiskfree (tmpDrive, &dtable_entry);

    if (result != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_wfile_get_free: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *total = (unsigned long) dtable_entry.total_clusters;
    *free  = (unsigned long) dtable_entry.avail_clusters;
    *sectors_per_unit = (unsigned long)  dtable_entry.sectors_per_cluster;
    *bytes_per_sector = (unsigned short) dtable_entry.bytes_per_sector;


    return (0);
#endif
	return (-1);
}


/************************************************************************
* Utility Function Bodies
************************************************************************/

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static int _rtp_windate_to_date (time_t * windate, RTP_DATE * rtpdate)
{
struct tm * ptime;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    ptime = localtime ((const time_t *)windate);

    if (!ptime)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("_rtp_windate_to_date: error returned.\n");
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
    (*rtpdate).tzOffset = _timezone;

    return (0);

}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static int _rtp_date_to_windate (RTP_DATE * rtpdate, time_t * windate)
{
struct tm ptime = {0,0,0,0,0,0,0,0,0};

    ptime.tm_year    = (*rtpdate).year - 1900;
    ptime.tm_mon     = (*rtpdate).month - 1;
    ptime.tm_mday    = (*rtpdate).day;    
    ptime.tm_hour    = (*rtpdate).hour;
	ptime.tm_min     = (*rtpdate).minute;    
	ptime.tm_sec     = (*rtpdate).second;
	ptime.tm_isdst   = (*rtpdate).dlsTime;
    _daylight   = (*rtpdate).dlsTime;
    _timezone        = (*rtpdate).tzOffset;   
    *windate = mktime (&ptime);        
    if (*windate == -1)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("_rtp_date_to_windate: error returned.\n");
#endif
        return (-1);
    }
    return (0);
}
/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static unsigned short * _rtp_unicode_name_to_winname (unsigned short * name)
{
unsigned short * p;

    if (!name)
    {
        return ((unsigned short *) NULL);
    }

    for (p = name; *p; p++)
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
    return (name);
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static unsigned short * _rtp_unicode_winname_to_name (unsigned short * name)
{
unsigned short * p;

    if (!name)
    {
        return ((unsigned short *) NULL);
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
    flag  - for the flag argument:
            RTP_FILE_O_APPEND   All writes will be appended to the file
            RTP_FILE_O_RDONLY   Open a file for reading only (default)
            RTP_FILE_O_WRONLY   Open a file for writing only
            RTP_FILE_O_RDWR     Open a file for reading and writing
            RTP_FILE_O_CREAT    Create a file if it does not exist
            RTP_FILE_O_TRUNC    Truncate a file to 0 bytes after opening
            RTP_FILE_O_EXCL     If creating a file, fail if it already exists
            RTP_FILE_O_BINARY   Create the file in binary mode.
            RTP_FILE_O_TEXT     Create the file in text mode.
    Note:   If neither the RTP_FILE_O_BINARY, or the RTP_FILE_O_TEXT are 
            used, the default behavior is to open the file in text mode.
----------------------------------------------------------------------*/

static int _rtp_flag_to_operation (unsigned short flag)
{
int result = 0;

    if (flag & RTP_FILE_O_APPEND)
    {
        result |= _O_APPEND;
    }
    if (flag & RTP_FILE_O_RDONLY)
    {
        result |= _O_RDONLY;
    }
    if (flag & RTP_FILE_O_WRONLY)
    {
        result |= _O_WRONLY;
    }
    if (flag & RTP_FILE_O_RDWR)
    {
        result |= _O_RDWR;
    }
    if (flag & RTP_FILE_O_CREAT)
    {
        result |= _O_CREAT;
    }
    if (flag & RTP_FILE_O_TRUNC)
    {
        result |= _O_TRUNC;
    }
    if (flag & RTP_FILE_O_EXCL)
    {
        result |= _O_EXCL;
    }

    if (flag & RTP_FILE_O_BINARY)
    {
        result |= _O_BINARY;
    }
    else
    {
        result |= _O_TEXT;
    }

    return (result);
}


/*----------------------------------------------------------------------
    mode  - for the mode argument:
            RTP_FILE_S_IWRITE   Create a file with write permissions
            RTP_FILE_S_IREAD    Create a file with read permissions
----------------------------------------------------------------------*/

static int _rtp_mode_to_permission (unsigned short mode)
{
int result = 0;

    /* ----------------------------------- */
    /*  Because of a limitation of windows */
    /*  _S_IWRITE is the same as _S_IREAD  */
    /*  or'ed with _S_IWRITE. There is no  */
    /*  write only permission.             */
    /* ----------------------------------- */

    if (mode & RTP_FILE_S_IWRITE)
    {
        result |= _S_IWRITE;
    }
    if (mode & RTP_FILE_S_IREAD)
    {
        result |= _S_IREAD;
    }
    if (mode & (RTP_FILE_S_IWRITE | RTP_FILE_S_IREAD))
    {
        result = _S_IREAD | _S_IWRITE;
    }
    if (mode == 0) /* mode not assigned */
    {
        result = _S_IREAD | _S_IWRITE;
    }

    return (result);
}
/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static unsigned int _rtp_flag_was_created(unsigned short flag)
{
	if (flag & RTP_FILE_O_CREAT)
	{
		if (flag & RTP_FILE_O_EXCL)
		{
			return 1;
		}
		else
		{
			if (GetLastError () != ERROR_ALREADY_EXISTS)
			{
				return 1;
			}
			else 
			{
				return 0;
			}
		}
	}
	return 0;

}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
