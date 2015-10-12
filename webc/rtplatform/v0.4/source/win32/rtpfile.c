 /*
 | RTPFILE.C - Runtime Platform Network Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - RT-Platform
 |
 |  $Author: tony $
 |  $Date: 2005/03/17 19:49:48 $
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
#include "rtpfile.h"
#include "rtpdebug.h"

#include <io.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <direct.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include <windows.h>
#include <ctype.h>


/************************************************************************
* Defines
************************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Structures
************************************************************************/

/************************************************************************
* Data
************************************************************************/

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Utility Function Prototypes
************************************************************************/
static int          _rtp_date_to_windate    (RTP_DATE * rtpdate, time_t * windate);
static void         _rtp_time_t_to_filetime (time_t t, LPFILETIME pft);
static int          _rtp_flag_to_operation  (unsigned short mode);
static int          _rtp_mode_to_permission (unsigned short mode);
static unsigned int _rtp_flag_was_created   (unsigned short flag);

/************************************************************************
* Function Bodies
************************************************************************/
int rtp_file_init(void)
{
	return(0);
}
/*----------------------------------------------------------------------*
                               rtp_file_open
 *----------------------------------------------------------------------*/
int rtp_file_open (RTP_HANDLE  * fdPtr, const char * name, unsigned short flag, unsigned short mode)
{
unsigned short attribs;
long fileHandle;
int result;
int rv;


#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif
    fileHandle = (long) _open (name, _rtp_flag_to_operation(flag), _rtp_mode_to_permission(mode));

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
        	RTP_DEBUG_OUTPUT_STR("rtp_file_open: non-fatal error returned ");
        	RTP_DEBUG_OUTPUT_INT(result);
        	RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_open: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
	/*TEST FLAG TO SEE IF WE CREATED*/
	if (_rtp_flag_was_created(flag))
	{
		attribs = 0;
		attribs |= ((mode & RTP_FILE_S_IREAD) && !(mode & RTP_FILE_S_IWRITE)) ? RTP_FILE_ATTRIB_RDONLY : 0;
		attribs |= (mode & RTP_FILE_S_HIDDEN) ? RTP_FILE_ATTRIB_HIDDEN : 0;
		attribs |= (mode & RTP_FILE_S_SYSTEM) ? RTP_FILE_ATTRIB_SYSTEM : 0;
		attribs |= (mode & RTP_FILE_S_ARCHIVE) ? RTP_FILE_ATTRIB_ARCHIVE : 0;

		rv = rtp_file_chmode ((char *) name, (unsigned char) attribs);
		if (rv < 0)
		{
#ifdef RTP_DEBUG
			result = GetLastError();
			RTP_DEBUG_OUTPUT_STR("rtp_file_open: non-fatal error returned from chmode ");
			RTP_DEBUG_OUTPUT_INT(result);
			RTP_DEBUG_OUTPUT_STR(".\n");
#endif
			return (-2);
		}
	}
    *fdPtr = (RTP_HANDLE) fileHandle;
    return (0);
}



/*----------------------------------------------------------------------*
                              rtp_file_close
 *----------------------------------------------------------------------*/
int rtp_file_close (RTP_HANDLE fileHandle)
{
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif
	if (_close ((int) fileHandle) != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_close: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------*
                              rtp_file_read
 *----------------------------------------------------------------------*/
long rtp_file_read (RTP_HANDLE fileHandle, unsigned char * buffer, long count)
{
int result;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if ((result = _read ((int) fileHandle, (void *) buffer, (unsigned int) count)) < 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_read: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (result);
}


/*----------------------------------------------------------------------*
                              rtp_file_write
 *----------------------------------------------------------------------*/
long rtp_file_write (RTP_HANDLE fileHandle, const unsigned char * buffer, long count)
{
int result;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if ((result = _write ((int) fileHandle, (void *) buffer, (unsigned int) count)) < 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_write: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (result);
}


/*----------------------------------------------------------------------*
                              rtp_file_lseek
 *----------------------------------------------------------------------*/
long rtp_file_lseek (RTP_HANDLE fd, long offset, int origin)
{
long result;
int  relative_to;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    switch (origin)
    {
        case 0:
            relative_to = SEEK_SET;
            break;
        case 1:
            relative_to = SEEK_CUR;
            break;
        case 2:
            relative_to = SEEK_END;
            break;
        default:
#ifdef RTP_DEBUG
            SetLastError (ERROR_BAD_COMMAND);
            result = GetLastError();
            RTP_DEBUG_OUTPUT_STR("rtp_file_lseek: error returned ");
            RTP_DEBUG_OUTPUT_INT(result);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-1);
            break;
    }

    if ((result = (long) _lseek ((int) fd, offset, relative_to)) < 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_lseek: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (result);
}


/*----------------------------------------------------------------------*
                             rtp_file_truncate
 *----------------------------------------------------------------------*/
int rtp_file_truncate (RTP_HANDLE fd, long offset)
{	
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif
	if (_chsize((int) fd, offset) < 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_truncate: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }    
    return (0);
}


/*----------------------------------------------------------------------*
                             rtp_file_flush
 *----------------------------------------------------------------------*/
int rtp_file_flush (RTP_HANDLE fd)
{
int result;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    /* ----------------------------------- */
    /*         trick to flush file         */
    /* ----------------------------------- */
    result = dup (fd);
    if (result == -1)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_flush: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    close (result);
    return (0);
}


/*----------------------------------------------------------------------*
                             rtp_file_rename
 *----------------------------------------------------------------------*/
int rtp_file_rename (const char * name, char * newname)
{
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

	if (rename ((const char *)name, (const char *)newname) != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_rename: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}




/*----------------------------------------------------------------------*
                             rtp_file_delete
 *----------------------------------------------------------------------*/
int rtp_file_delete (const char * name)
{
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif
	if (remove ((const char *)name) != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_delete: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}



/*----------------------------------------------------------------------*
                             rtp_file_mkdir
 *----------------------------------------------------------------------*/
int rtp_file_mkdir (const char * name)
{
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif
	if (_mkdir ((const char *)name) != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_mkdir: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}



/*----------------------------------------------------------------------*
                             rtp_file_rmdir
 *----------------------------------------------------------------------*/
int rtp_file_rmdir (const char * name)
{
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (_rmdir ((const char *)name) != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_rmdir: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}



/*----------------------------------------------------------------------*
                             rtp_file_setcwd
 *----------------------------------------------------------------------*/
int rtp_file_setcwd (const char * name)
{
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (_chdir ((const char *)name) != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_setcwd: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}



/*----------------------------------------------------------------------*
                             rtp_file_pwd
 *----------------------------------------------------------------------*/
int rtp_file_pwd (char * name, long size)
{
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (_getcwd (name, (int)size) == NULL)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_pwd: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}



/*----------------------------------------------------------------------*
                           rtp_file_chmode
 *----------------------------------------------------------------------*/
int rtp_file_chmode (const char * name, unsigned char attributes)
{
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

    if (SetFileAttributesA ((const char *)name, winmode) < 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_chmode: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}



/*----------------------------------------------------------------------*
                           rtp_file_get_free
 *----------------------------------------------------------------------*/
int rtp_file_get_free (const char * name, unsigned long *total, unsigned long *free,
                       unsigned long *sectors_per_unit, unsigned short *bytes_per_sector)
{
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

    tmpDrive = tolower((int) name[0]);
    tmpDrive = (tmpDrive - 'a') + 1;
    result = _getdiskfree (tmpDrive, &dtable_entry);

    if (result != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_get_free: error returned ");
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
}


/*----------------------------------------------------------------------*
                           rtp_file_set_time
 *----------------------------------------------------------------------*/

int rtp_file_set_time (RTP_HANDLE fd, RTP_DATE * adate, RTP_DATE * wdate, RTP_DATE * cdate, RTP_DATE * hdate)
{
	struct _utimbuf utime;
    int result = 0;
	time_t tC;

	//_rtp_date_to_windate (adate, &tA);
	_rtp_date_to_windate (wdate, &tC);

    utime.actime = 0;
    utime.modtime = tC;
    result = _futime(fd ,&utime);
    return (result);
}


/************************************************************************
* Utility Function Bodies
************************************************************************/

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
static void _rtp_time_t_to_filetime(time_t t, LPFILETIME pft)
{
/* Note that LONGLONG is a 64-bit value */
LONGLONG xl;

	xl = Int32x32To64(t, 10000000) + 116444736000000000;
	pft->dwLowDateTime = (DWORD) xl;
	pft->dwHighDateTime = (DWORD) (xl >> 32);
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
			if (GetLastError() != ERROR_ALREADY_EXISTS)
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
