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

#include "rtp.h"
#include "rtpfile.h"
#include "rtpdebug.h"

#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <windows.h>
#include <ctype.h>


/************************************************************************
* Defines
************************************************************************/

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

void _rtp_rtpflag_to_winmode (unsigned short flag, char * winMode);
/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                               rtp_file_open
 *----------------------------------------------------------------------*/
int rtp_file_open (RTP_HANDLE  * fdPtr, const char * name, unsigned short flag, unsigned short mode)
{
FILE * fileHandle;
char winMode[4];

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif
	_rtp_rtpflag_to_winmode (flag, winMode);
    fileHandle = fopen (name, (const char*) winMode);

    if (!fileHandle)
    {
#ifdef RTP_DEBUG
		result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_open: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *fdPtr = (RTP_HANDLE) fileHandle;

    return (0);
}

/*----------------------------------------------------------------------*
                              rtp_file_close
 *----------------------------------------------------------------------*/
int rtp_file_close (RTP_HANDLE fileHandle)
{

FILE *fdPtr = (FILE*) fileHandle;

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (fclose (fdPtr) != 0)
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
int initialPosition;
int finalPosition;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

	initialPosition = ftell ((FILE*)fileHandle);

    if (result = fread ((void *) buffer, sizeof(char), (unsigned int) count, (FILE*)fileHandle ) < 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_read: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

	finalPosition = ftell ((FILE*)fileHandle);

	/* return number of bytes read */
    return ((finalPosition - initialPosition));
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

    if (result = fwrite ((void *) buffer, sizeof(char), (unsigned int) count, (FILE*)fileHandle ) < 0)
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
long  result;
int   relative_to;
FILE *fileHandle = ( FILE *)fd;

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

    result = (long) fseek(fileHandle, offset, relative_to );

    if(result)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_lseek: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    result = ftell( fileHandle );

    return (result);
}


/*----------------------------------------------------------------------*
                             rtp_file_truncate
 *----------------------------------------------------------------------*/
int rtp_file_truncate (RTP_HANDLE fd, long offset)
{
    return (-1);
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
    result = fflush ((FILE*)fd);
    if (result)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_file_flush: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    fclose ((FILE*)fd);
    return (0);
}


/*----------------------------------------------------------------------*
                             rtp_file_rename
 *----------------------------------------------------------------------*/
int rtp_file_rename (const char * name, char * newname)
{

    return (-1);
}




/*----------------------------------------------------------------------*
                             rtp_file_delete
 *----------------------------------------------------------------------*/
int rtp_file_delete (const char * name)
{
    return (-1);
}



/*----------------------------------------------------------------------*
                             rtp_file_mkdir
 *----------------------------------------------------------------------*/
int rtp_file_mkdir (const char * name)
{
    return (-1);
}



/*----------------------------------------------------------------------*
                             rtp_file_rmdir
 *----------------------------------------------------------------------*/
int rtp_file_rmdir (const char * name)
{
    return (-1);
}



/*----------------------------------------------------------------------*
                             rtp_file_setcwd
 *----------------------------------------------------------------------*/
int rtp_file_setcwd (const char * name)
{
	return (-1);
}



/*----------------------------------------------------------------------*
                             rtp_file_pwd
 *----------------------------------------------------------------------*/
int rtp_file_pwd (char * name, long size)
{
	return (-1);
}



/*----------------------------------------------------------------------*
                           rtp_file_chmode
 *----------------------------------------------------------------------*/
int rtp_file_chmode (const char * name, unsigned char attributes)
{
	return (-1);
}



/*----------------------------------------------------------------------*
                           rtp_file_get_free
 *----------------------------------------------------------------------*/
int rtp_file_get_free (const char * name, unsigned long *total, unsigned long *free,
                       unsigned long *sectors_per_unit, unsigned short *bytes_per_sector)
{
	return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_file_set_time
 *----------------------------------------------------------------------*/

int rtp_file_set_time (RTP_HANDLE fd, RTP_DATE * adate, RTP_DATE * wdate, RTP_DATE * cdate, RTP_DATE * hdate)
{
	return (-1);
}


/************************************************************************
* Utility Function Bodies
************************************************************************/

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/




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

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void _rtp_rtpflag_to_winmode (unsigned short flag, char * winMode)
{
	int index = 0;

	if ((flag & RTP_FILE_O_RDONLY) == RTP_FILE_O_RDONLY)
	{
		winMode[index++] = 'r';
	}
	if ((flag & RTP_FILE_O_WRONLY) == RTP_FILE_O_WRONLY)
	{
		winMode[index++] = 'w';
	}
	if ((flag & RTP_FILE_O_RDWR) == RTP_FILE_O_RDWR)
	{
		if ((flag & RTP_FILE_O_TRUNC) == RTP_FILE_O_TRUNC)
		{
			winMode[index++] = 'w';
			winMode[index++] = '+';
		}
		else if ((flag & RTP_FILE_O_APPEND) == RTP_FILE_O_APPEND)
		{
			winMode[index++] = 'a';
			winMode[index++] = '+';
		}
		else
		{
			winMode[index++] = 'r';
			winMode[index++] = '+';
		}
	}
	if ((flag & RTP_FILE_O_RDWR) != RTP_FILE_O_RDWR)
	{
		if ((flag & RTP_FILE_O_APPEND) == RTP_FILE_O_APPEND)
		{
			winMode[index++] = 'a';
		}
	}

	if ((flag & RTP_FILE_O_BINARY) == RTP_FILE_O_BINARY)
	{
		winMode[index++] = 'b';
	}

	else if ((flag & RTP_FILE_O_TEXT) == RTP_FILE_O_TEXT)
	{
		winMode[index++] = 't';
	}

	/* terminate the string */
	winMode[index] = '\0';
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */

