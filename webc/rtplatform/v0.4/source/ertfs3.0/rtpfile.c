 /*
 | RTPFILE.C - Runtime Platform Network Services
 |
 |   PORTED TO A CUSTOMER MODIFIED VERSION OF ERTFS 3.0 PLATFORM
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:30 $
 |  $Name:  $
 |  $Revision: 1.5 $
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
#ifdef RTP_DISPLAY_ERROR
 #include "rtpterm.h"
 #include "rtpscnv.h"
#endif
#include <ertfs.h>

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
* Utility Function Prototypes
************************************************************************/
static RTP_DATE *     _rtp_ertfsdate_to_date  (DATESTR  * ertfsdate);
static DATESTR *      _rtp_date_to_ertfsdate  (RTP_DATE * rtpdate);
static unsigned short _rtp_flag_to_operation  (unsigned short mode);
static unsigned short _rtp_mode_to_permission (unsigned short mode);

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                               rtp_file_open
 *----------------------------------------------------------------------*/
int rtp_file_open (RTP_HANDLE  * fdPtr, const char * name, unsigned short flag, unsigned short mode)
{
PCFD fileHandle;
int result;

#ifdef RTP_DISPLAY_ERROR
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    fileHandle = open (name, _rtp_flag_to_operation(flag), _rtp_mode_to_permission(mode));

    if (fileHandle == (PCFD)(-1))
    {
        result = errno;
        /* ----------------------------------- */
        /*  If trying to open a directory or   */
        /*  opening a read only file with      */
        /*  write privilages.  This can be     */
        /*  non-fatal if doing an open to      */
        /*  determine the existance of a       */
        /*  directory.                         */
        /* ----------------------------------- */
        if (result == PEACCES)
        {
#ifdef RTP_DISPLAY_ERROR
            rtp_term_cputs("rtp_file_open: non-fatal error returned ");
            rtp_term_puts(rtp_ltoa(result, error, 10));
#endif
            return (-2);
        }
#ifdef RTP_DISPLAY_ERROR
        rtp_term_cputs("rtp_file_open: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
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
#ifdef RTP_DISPLAY_ERROR
    int result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (po_close ((PCFD) fileHandle) != 0)
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_close: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
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
    
#ifdef RTP_DISPLAY_ERROR
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if ((result = read ((int) fileHandle, buffer, (int) count)) < 0)
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_read: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
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
    
#ifdef RTP_DISPLAY_ERROR
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if ((result = write ((int) fileHandle, buffer, (int) count)) < 0)
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_write: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
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

#ifdef RTP_DISPLAY_ERROR
    char error[32];

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
#ifdef RTP_DISPLAY_ERROR
            errno = PEINVALIDPARMS;
            result = errno;
            rtp_term_cputs("rtp_file_lseek: error returned ");
            rtp_term_puts(rtp_itoa(result, error, 10));
#endif
            return (-1);
            break;
    }

    if ((result = (long) lseek ((int) fd, offset, relative_to)) < 0)
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_lseek: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
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
#ifdef RTP_DISPLAY_ERROR
    int result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (!truncate((int) fd, offset))
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_truncate: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
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
#ifdef RTP_DISPLAY_ERROR
    int result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (!flush_fd((int) fd))
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_flush: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------*
                             rtp_file_rename
 *----------------------------------------------------------------------*/
int rtp_file_rename (char * name, char * newname)
{
#ifdef RTP_DISPLAY_ERROR
    int result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif
    
    if (!pc_mv (name, newname))
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_rename: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------*
                             rtp_file_delete
 *----------------------------------------------------------------------*/
int rtp_file_delete (char * name)
{
#ifdef RTP_DISPLAY_ERROR
    int result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (!pc_unlink (name))
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_delete: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------*
                             rtp_file_mkdir
 *----------------------------------------------------------------------*/
int rtp_file_mkdir (char * name)
{
#ifdef RTP_DISPLAY_ERROR
    int result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif
    
    if (!pc_mkdir (name))
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_mkdir: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------*
                             rtp_file_rmdir
 *----------------------------------------------------------------------*/
int rtp_file_rmdir (char * name)
{
#ifdef RTP_DISPLAY_ERROR
    int result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (!pc_rmdir (name))
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_rmdir: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------*
                             rtp_file_setcwd
 *----------------------------------------------------------------------*/
int rtp_file_setcwd (char * name)
{
#ifdef RTP_DISPLAY_ERROR
    int result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (!pc_set_cwd (name))
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_setcwd: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
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
	char drv_name[10];
	int drive;

#ifdef RTP_DISPLAY_ERROR
    int result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

	drive = pc_getdfltdrvno();		/* get the current drive */
	sprintf(drv_name,"%c:", drive + 'A');
    if (!pc_pwd (drv_name, name))
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_pwd: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
#endif
        return (-1);
    }
    
    return (0);
}


/*----------------------------------------------------------------------*
                             rtp_file_gfirst
 *----------------------------------------------------------------------*/
int rtp_file_gfirst (void ** dirobj, char * name)
{
DSTAT fdata;

#ifdef RTP_DISPLAY_ERROR
    int result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif
    
    if (!pc_gfirst (&fdata, name))
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_gfirst: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
#endif
        return (-1);
    }
    
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
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (!pc_gnext ((DSTAT *)dirobj))
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
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
}


/*----------------------------------------------------------------------*
                            rtp_file_get_size
 *----------------------------------------------------------------------*/
int rtp_file_get_size (void * dirobj, unsigned long * size)
{
#ifdef RTP_DISPLAY_ERROR
    int  result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (!dirobj)
    {
#ifdef RTP_DISPLAY_ERROR
        errno = PEINVALIDPARMS;
        result = errno;
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
    int  result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (!dirobj)
    {
#ifdef RTP_DISPLAY_ERROR
        errno = PEINVALIDPARMS;
        result = errno;
        rtp_term_cputs("rtp_file_get_attrib: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
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
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (!dirobj)
    {
#ifdef RTP_DISPLAY_ERROR
        errno = PEINVALIDPARMS;
        result = errno;
        rtp_term_cputs("rtp_file_get_name: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
#endif
        return (-1);
    }

#if (VFAT)
    sizelimit = strlen(((DSTAT *)dirobj)->lfname);
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
RTP_DATE * tmpdate;
    
#ifdef RTP_DISPLAY_ERROR
    int  result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

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


/*----------------------------------------------------------------------*
                           rtp_file_chmode
 *----------------------------------------------------------------------*/
int rtp_file_chmode (char * name, unsigned char attributes)
{
int ertfsmode;

#ifdef RTP_DISPLAY_ERROR
    int result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    ertfsmode  = 0;
    ertfsmode |= (attributes & RTP_FILE_ATTRIB_RDONLY)  ?  ARDONLY : 0;
    ertfsmode |= (attributes & RTP_FILE_ATTRIB_ARCHIVE) ?  ARCHIVE : 0;
    ertfsmode |= (attributes & RTP_FILE_ATTRIB_HIDDEN)  ?  AHIDDEN : 0;
    ertfsmode |= (attributes & RTP_FILE_ATTRIB_SYSTEM)  ?  ASYSTEM : 0;
    
    if (!pc_set_attributes ((const char *)name, ertfsmode))
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_chmode: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------*
                           rtp_file_get_free
 *----------------------------------------------------------------------*/
int rtp_file_get_free (char * name, unsigned long *total, unsigned long *free,
                       unsigned long *sectors_per_unit, unsigned short *bytes_per_sector)
{
dword total_blocks, free_blocks;
long bytes_free;
word size;

#ifdef RTP_DISPLAY_ERROR
    int result;
    char error[32];

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif
    bytes_free = pc_lfree (name, &total_blocks, &free_blocks, &size, FALSE);
    
    if (bytes_free < 0)
    {
#ifdef RTP_DISPLAY_ERROR
        result = errno;
        rtp_term_cputs("rtp_file_get_free: error returned ");
        rtp_term_puts(rtp_itoa(result, error, 10));
#endif
        return (-1);
    }

    *total = (unsigned long) total_blocks;
    *free  = (unsigned long) free_blocks;
    *sectors_per_unit = (unsigned long) 1;
    *bytes_per_sector = (unsigned short) size;

    return (0);
}



int rtp_file_set_time (RTP_HANDLE fd, RTP_DATE * adate, RTP_DATE * wdate, RTP_DATE * cdate, RTP_DATE * hdate)
{
    /* ----------------------------------- */
    /*   Not supported by the ertfs fs.    */
    /*    rtp_not_yet_implemented ();      */
    /* ----------------------------------- */
    return (0);         /* TRUE  returned */
}


/************************************************************************
* Utility Function Bodies
************************************************************************/

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static RTP_DATE * _rtp_ertfsdate_to_date (DATESTR *ertfsdate)
{
RTP_DATE rtpdate;

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


/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static DATESTR * _rtp_date_to_ertfsdate (RTP_DATE * rtpdate)
{
DATESTR ertfsdate;

    if (!rtpdate)
    {
        return ((DATESTR *)NULL);
    }
    
    /* ---------------------------------- */
    /*              hour  minute      sec */
    /* time        00000  000000    00000 */
    /* ---------------------------------- */
    
    ertfsdate.time = (word) ( (rtpdate->hour << 11)  |
                              (rtpdate->minute << 5) |
                               rtpdate->second );

    /* ---------------------------------- */
    /*              year   month      day */
    /* date      0000000    0000    00000 */
    /* ---------------------------------- */

    ertfsdate.date = (word) ( (rtpdate->year << 9)  |
                              (rtpdate->month << 5) | 
                               rtpdate->day );

    return (&ertfsdate);
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
----------------------------------------------------------------------*/

static unsigned short _rtp_flag_to_operation (unsigned short flag)
{
unsigned short result = 0;

    if (flag & RTP_FILE_O_APPEND)
    {
        result |= O_APPEND;
    }
    if (flag & RTP_FILE_O_RDONLY)
    {
        result |= O_RDONLY;
    }
    if (flag & RTP_FILE_O_WRONLY)
    {
        result |= O_WRONLY;
    }
    if (flag & RTP_FILE_O_RDWR)
    {
        result |= O_RDWR;
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
    
    if (result == 0)
    {
        result = O_RDONLY;
    }
    
    result |= PO_BINARY;
    
    return (result);
}


/*----------------------------------------------------------------------
    mode  - for the mode argument:
            RTP_FILE_S_IWRITE   Create a file with write permissions
            RTP_FILE_S_IREAD    Create a file with read permissions
----------------------------------------------------------------------*/

static unsigned short _rtp_mode_to_permission (unsigned short mode)
{
unsigned short result = 0;

    /* ----------------------------------- */
    /*  Because of a limitation of windows */
    /*  _S_IWRITE is the same as _S_IREAD  */
    /*  or'ed with _S_IWRITE. There is no  */
    /*  write only permission.             */
    /* ----------------------------------- */

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
