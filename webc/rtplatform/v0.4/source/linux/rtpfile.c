 /*
 | RTPFILE.C - Runtime Platform Filesystem Services
 |
 |   PORTED TO THE LINUX PLATFORM
 |
 | EBSnet - RT-Platform
 |
 |  $Author: tony $
 |  $Date: 2005/06/30 00:42:02 $
 |  $Name:  $
 |  $Revision: 1.2 $
 |
 | Copyright EBSnet Inc. , 2003-2005
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

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>


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
static int          _rtp_flag_to_operation  (unsigned short mode);
static int          _rtp_mode_to_permission (unsigned short mode);

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                               rtp_file_open
 *----------------------------------------------------------------------*/
int rtp_file_open (RTP_HANDLE  * fdPtr, const char * name, unsigned short flag, unsigned short mode)
{
int fileHandle;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    fileHandle = open (name, _rtp_flag_to_operation(flag), _rtp_mode_to_permission(mode));

    if (fileHandle == (-1))
    {
        /* ----------------------------------- */
        /*  If trying to open a directory or   */
        /*  opening a read only file with      */
        /*  write privilages.  This can be     */
        /*  non-fatal if doing an open to      */
        /*  determine the existance of a       */
        /*  directory.                         */
        /* ----------------------------------- */
        if (errno == EACCES)
        {
#ifdef RTP_DEBUG
        	RTP_DEBUG_OUTPUT_STR("rtp_file_open: non-fatal error returned ");
        	RTP_DEBUG_OUTPUT_INT(errno);
        	RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_open: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
     /* Ignore RTP_FILE_S_HIDDEN, RTP_FILE_S_SYSTEM, and RTP_FILE_S_ARCHIVE. */

    *fdPtr = (RTP_HANDLE) fileHandle;
    return (0);
}



/*----------------------------------------------------------------------*
                              rtp_file_close
 *----------------------------------------------------------------------*/
int rtp_file_close (RTP_HANDLE fileHandle)
{
#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (close ((int) fileHandle) != 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_close: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
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
ssize_t result;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if ((result = read ((int) fileHandle, (void *) buffer, (size_t) count)) < 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_read: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return ((long) result);
}


/*----------------------------------------------------------------------*
                              rtp_file_write
 *----------------------------------------------------------------------*/
long rtp_file_write (RTP_HANDLE fileHandle, const unsigned char * buffer, long count)
{
ssize_t result;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if ((result = write ((int) fileHandle, (void *) buffer, (size_t) count)) < 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_write: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return ((long) result);
}


/*----------------------------------------------------------------------*
                              rtp_file_lseek
 *----------------------------------------------------------------------*/
long rtp_file_lseek (RTP_HANDLE fd, long offset, int origin)
{
off_t result;
int   relative_to;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
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
            errno = EINVAL;
            RTP_DEBUG_OUTPUT_STR("rtp_file_lseek: error returned ");
            RTP_DEBUG_OUTPUT_INT(errno);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-1);
            break;
    }

    if ((result = lseek ((int) fd, (off_t) offset, relative_to)) == (off_t)-1)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_lseek: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return ((long) result);
}


/*----------------------------------------------------------------------*
                             rtp_file_truncate
 *----------------------------------------------------------------------*/
int rtp_file_truncate (RTP_HANDLE fd, long offset)
{
#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (ftruncate((int) fd, (off_t) offset) < 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_truncate: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
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
#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (fsync ((int) fd) == -1)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_flush: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------*
                             rtp_file_rename
 *----------------------------------------------------------------------*/
int rtp_file_rename (const char * name, char * newname)
{
#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

	if (rename ((const char *)name, (const char *)newname) != 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_rename: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
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
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (unlink ((const char *)name) != 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_delete: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
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
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    /* Owner of file is granted all permissions. */
    if (mkdir ((const char *)name, 0700) != 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_mkdir: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
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
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (rmdir ((const char *)name) != 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_rmdir: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
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
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (chdir ((const char *)name) != 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_setcwd: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
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
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (getcwd (name, (size_t) size) == NULL)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_pwd: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
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
int linmode;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    /* Ignore RTP_FILE_ATTRIB_ARCHIVE, RTP_FILE_ATTRIB_HIDDEN, and
       RTP_FILE_ATTRIB_SYSTEM. */
    if (attributes & RTP_FILE_ATTRIB_WRONLY)
    {
        linmode = S_IWRITE;
    }
    else if (attributes & RTP_FILE_ATTRIB_RDWR)
    {
        linmode = S_IREAD | S_IWRITE;
    }
    else /* default to RDONLY */
    {
        linmode = S_IREAD;
    }

    if (chmod ((const char *)name, linmode) < 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_chmode: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
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
struct statfs stats;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (statfs ((const char *) name, &stats) != 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_file_get_free: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *total = (unsigned long) stats.f_blocks;
    *free  = (unsigned long) stats.f_bavail;
    *sectors_per_unit = (unsigned long)  1;
    *bytes_per_sector = (unsigned short) stats.f_bsize;

    return (0);
}


/*----------------------------------------------------------------------*
                           rtp_file_set_time
 *----------------------------------------------------------------------*/

int rtp_file_set_time (RTP_HANDLE fd, RTP_DATE * adate, RTP_DATE * wdate, RTP_DATE * cdate, RTP_DATE * hdate)
{
    /* ----------------------------------- */
    /*  Linux does not support             */
	/*  modifying times on open files.     */
	/*  Use utime(2) to operate on file    */
	/*  names.                             */
    /* ----------------------------------- */

    return (0);
}

/*----------------------------------------------------------------------*
                           rtp_file_get_path_seperator
 *----------------------------------------------------------------------*/
char rtp_file_get_path_seperator (void)
{
    return ('/');
}

/************************************************************************
* Utility Function Bodies
************************************************************************/

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
        result |= O_APPEND;
    }
    if (flag & RTP_FILE_O_CREAT)
    {
        result |= O_CREAT;
    }
    if (flag & RTP_FILE_O_TRUNC)
    {
        result |= O_TRUNC;
    }
    if (flag & RTP_FILE_O_EXCL)
    {
        result |= O_EXCL;
    }

    if (flag & RTP_FILE_O_WRONLY)
    {
        result |= O_WRONLY;
    }
    else if (flag & RTP_FILE_O_RDWR)
    {
        result |= O_RDWR;
    }
    else /* default to RDONLY */
    {
        result |= O_RDONLY;
    }

    /* Ignore RTP_FILE_O_BINARY and RTP_FILE_O_TEXT */
    /* Linux is always BINARY */

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

    if (mode & RTP_FILE_S_IWRITE)
    {
        result |= S_IWRITE;
    }
    if (mode & RTP_FILE_S_IREAD)
    {
        result |= S_IREAD;
    }

    return (result);
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
