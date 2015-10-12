 /*
 | RTPFILE.C - Runtime Platform Network Services
 |
 |   PORTED TO THE EBSnet Inc RTIP TCP/IP PLATFORM
 |
 | EBSnet - EBS_RT-Platform 
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
#include "rtpfile.h"
#include "rtpdebug.h"
#include "rtpdutil.h"
#include "rtpstr.h"
#include "rtpmem.h" /* _yi_ */
#include "rtipapi.h"
//#include <rtip.h>
#include <vfapi.h>


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
static char *            _rtp_nativename_to_name      (const char * name);
static char *            _rtp_name_to_nativename      (const char * name);
static int               _rtp_flag_to_operation       (unsigned short mode);
static int               _rtp_mode_to_permission      (unsigned short mode);
   
// vf_api is a global variable that points to a vfileapi
// structure. This structure contains pointers to functions that 
// implement the file system. The vf_xxx macros in vfile.h call these
// functions
#if (INCLUDE_RTIP)
PVFILEAPI vf_api; /* initialized in xn_rtip_init() */
#else
PVFILEAPI vf_api = NULL; 
#endif
#define RTP_DEBUG
/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                               rtp_file_open
 *----------------------------------------------------------------------*/
int rtp_file_open (RTP_HANDLE  * fdPtr, const char * name, unsigned short flag, unsigned short mode)
{
long fileHandle;

#ifdef RTP_DEBUG
	int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    name = (char *) _rtp_name_to_nativename((char *) name);
    fileHandle = (long) vf_open((RTP_PFCHAR) name, (RTP_INT32) _rtp_flag_to_operation(flag), (RTP_INT32) _rtp_mode_to_permission(mode));

    if (fileHandle == (-1))
    {
        result = xn_getlasterror( );
        dc_log_printf("rtp_file_open: error returned %d", result);
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
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
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    if (vf_close((int) fileHandle) != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
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
    set_errno (0);
#endif

    if ((result = vf_read((int) fileHandle, (RTP_PFUINT8) buffer, (RTP_INT32) count)) < 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
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
    set_errno (0);
#endif

    if ((result = vf_write((int) fileHandle, (RTP_PFUINT8) buffer, (RTP_INT32) count)) < 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
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
    set_errno (0);
#endif

    switch (origin)
    {
        case 0:
            relative_to = VSEEK_SET;
            break;
        case 1:
            relative_to = VSEEK_CUR;
            break;
        case 2:
            relative_to = VSEEK_END;
            break;
        default:
#ifdef RTP_DEBUG
			RTP_DEBUG_OUTPUT_STR("rtp_file_lseek: error returned.\n");
#endif
            return (-1);
            break;
    }

    if ((result = vf_lseek((int) fd, offset, relative_to)) < 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
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
    set_errno (0);
#endif

	if (!vf_truncate((int) fd, offset))
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
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
#ifdef RTP_DEBUG
	int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    /* ----------------------------------- */
    /*         trick to flush file         */
    /* ----------------------------------- */
    if (!vf_flush(fd))
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
        RTP_DEBUG_OUTPUT_STR("rtp_file_flush: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
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
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    name    = _rtp_name_to_nativename(name);
    newname = _rtp_name_to_nativename(newname);

	if (!vf_rename((RTP_PFCHAR) name, (RTP_PFCHAR) newname))
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
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
    set_errno (0);
#endif

    name = _rtp_name_to_nativename(name);

    if (!vf_delete((RTP_PFCHAR)name))
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
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
    set_errno (0);
#endif

    name = _rtp_name_to_nativename(name);

    if (!vf_mkdir((RTP_PFCHAR)name))
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
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
    set_errno (0);
#endif

    name = _rtp_name_to_nativename(name);

    if (!vf_rmdir((RTP_PFCHAR)name))
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
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
    set_errno (0);
#endif

    name = _rtp_name_to_nativename(name);

    if (!vf_set_cwd((RTP_PFCHAR)name))
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
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
    set_errno (0);
#endif

	if (!vf_pwd(name))
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_file_pwd: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    name = _rtp_nativename_to_name(name);
    return (0);
}


/*----------------------------------------------------------------------*
                             rtp_file_gfirst
 *----------------------------------------------------------------------*/
int rtp_file_gfirst (void ** dirobj, char * name)
{
VDSTAT* nativeDirObj;

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif
//    dc_log_printf("rtp_file_gfirst");
    nativeDirObj = (VDSTAT*) rtp_malloc(sizeof(VDSTAT), RTP_VFILE_MALLOC);
    rtp_memset(nativeDirObj, 0, sizeof(VDSTAT));

	if (!vf_gfirst(nativeDirObj, (RTP_PFCHAR) name))
    {
        rtp_free(nativeDirObj);
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
        RTP_DEBUG_OUTPUT_STR("rtp_file_gfirst: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *dirobj = (void*) nativeDirObj;
    return (0);
}


/*----------------------------------------------------------------------*
                             rtp_file_gnext
 *----------------------------------------------------------------------*/
int rtp_file_gnext (void * dirobj)
{
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif
	
	if (!vf_gnext((VDSTAT*) dirobj))
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
        RTP_DEBUG_OUTPUT_STR("rtp_file_gnext: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
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
	vf_gdone((VDSTAT*) dirobj);
    rtp_free(dirobj);
}


/*----------------------------------------------------------------------*
                            rtp_file_get_size
 *----------------------------------------------------------------------*/
int rtp_file_get_size (void * dirobj, unsigned long * size)
{
    if (!dirobj)
    {
    	RTP_DEBUG_OUTPUT_STR("rtp_file_get_size: error returned.\n");
        return (-1);
    }

	if (size)
	{
		*size = (unsigned long) (((VDSTAT*)dirobj)->fsize);
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
    if (!dirobj)
    {
    	RTP_DEBUG_OUTPUT_STR("rtp_file_get_attrib: error returned.\n");
        return (-1);
    }

    *attributes  = (((VDSTAT*)dirobj)->fattributes & VF_ATTRIB_ISDIR)  ? RTP_FILE_ATTRIB_ISDIR   : 0;
    *attributes |= (((VDSTAT*)dirobj)->fattributes & VF_ATTRIB_RDONLY) ? RTP_FILE_ATTRIB_RDONLY  : RTP_FILE_ATTRIB_RDWR;
/*    *attributes |= (((VDSTAT*)dirobj)->fattributes & unsupported)      ? RTP_FILE_ATTRIB_ARCHIVE : 0; */
/*    *attributes |= (((VDSTAT*)dirobj)->fattributes & unsupported)      ? RTP_FILE_ATTRIB_HIDDEN  : 0; */
/*    *attributes |= (((VDSTAT*)dirobj)->fattributes & unsupported)      ? RTP_FILE_ATTRIB_SYSTEM  : 0; */
    *attributes |= (((VDSTAT*)dirobj)->fattributes & VF_ATTRIB_ISVOL)  ? RTP_FILE_ATTRIB_ISVOL   : 0;
    dc_log_printf("in attribute %#x(VF), out: %#x(RTP)", ((VDSTAT*)dirobj)->fattributes, (*attributes));

    return (0);
}


/*----------------------------------------------------------------------*
                           rtp_file_get_name
 *----------------------------------------------------------------------*/
int rtp_file_get_name (void * dirobj, char * name, int size)
{
unsigned int sizelimit;

    if (!dirobj)
    {
    	RTP_DEBUG_OUTPUT_STR("rtp_file_get_name: error returned.");
        return (-1);
    }

    sizelimit = rtp_strlen(((VDSTAT*)dirobj)->filename);
    if (size > 0)
    {
        if (sizelimit > (unsigned int)(size - 1))
        {
            sizelimit = size - 1;
        }
    }
    rtp_strncpy(name, (const char *) (((VDSTAT*)dirobj)->filename), sizelimit);
	name[sizelimit] = '\0';

    name = _rtp_nativename_to_name(name);
    return (0);
}


/*----------------------------------------------------------------------*
                           rtp_file_get_time
 *----------------------------------------------------------------------*/
int rtp_file_get_time (void * dirobj, RTP_DATE * adate, RTP_DATE * wdate, RTP_DATE * cdate, RTP_DATE * hdate)
{
RTP_TIMESTAMP rtpTimeStamp;

    if (!dirobj)
    {
    	RTP_DEBUG_OUTPUT_STR("rtp_file_get_time: error returned.");
        return (-1);
    }

	/* Temporarily returning this value for all inquiries. */
	rtpTimeStamp.year   = ((VDSTAT*)dirobj)->ebs_mod_time.year;
	rtpTimeStamp.second = ((VDSTAT*)dirobj)->ebs_mod_time.second;
    
    if (adate)
    {
    	rtp_timestamp_to_date(adate, &rtpTimeStamp);
    }
    if (wdate)
    {
        rtp_timestamp_to_date(wdate, &rtpTimeStamp);
    }
    if (cdate)
    {
        rtp_timestamp_to_date(cdate, &rtpTimeStamp);
    }
    if (hdate)
    {
        rtp_timestamp_to_date(hdate, &rtpTimeStamp);
    }
    return (0);
}


/*----------------------------------------------------------------------*
                           rtp_file_chmode
 *----------------------------------------------------------------------*/
int rtp_file_chmode (const char * name, unsigned char attributes)
{
int nativemode;

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    nativemode  = 0;
    nativemode |= (attributes & RTP_FILE_ATTRIB_RDONLY)  ? VF_ATTRIB_RDONLY  : 0;
    nativemode |= (attributes & RTP_FILE_ATTRIB_WRONLY)  ? VF_ATTRIB_WRONLY  : 0;
    nativemode |= (attributes & RTP_FILE_ATTRIB_RDWR)    ? VF_ATTRIB_RDWR    : 0;
/*    nativemode |= (attributes & RTP_FILE_ATTRIB_ARCHIVE) ? unsupported       : 0; */
/*    nativemode |= (attributes & RTP_FILE_ATTRIB_HIDDEN)  ? unsupported       : 0; */
/*    nativemode |= (attributes & RTP_FILE_ATTRIB_SYSTEM)  ? unsupported       : 0; */

    name = _rtp_name_to_nativename(name);

	if (!vf_chmode((RTP_PFCHAR) name, nativemode))
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
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
RTP_UINT32 blocks;
RTP_UINT32 bfree;

#ifdef RTP_DEBUG
	int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

	name = _rtp_name_to_nativename(name);
	if (!vf_get_free((RTP_PFCHAR) name, &blocks, &bfree))
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror( );
        RTP_DEBUG_OUTPUT_STR("rtp_file_get_free: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *total = (unsigned long) blocks;
    *free  = (unsigned long) bfree;
    //JC: this is a disconnect from apiinfo.c pc_free(...) function.
    //the information is there, just not passed to this point
    
    //*sectors_per_unit = (unsigned long)  0; /* [tbd] */
    //*bytes_per_sector = (unsigned short) 0; /* [tbd] */
    //but, with these set to 0, mac will crash upon connect..
    //using the values that HP uses here:
    *sectors_per_unit = (unsigned long)  1;
    *bytes_per_sector = (unsigned short) 512;
    return (0);
}


/*----------------------------------------------------------------------*
                           rtp_file_set_time
 *----------------------------------------------------------------------*/

int rtp_file_set_time (RTP_HANDLE fd, RTP_DATE * adate, RTP_DATE * wdate, RTP_DATE * cdate, RTP_DATE * hdate)
{
    rtp_not_yet_implemented( );
    return (-1);
}


/************************************************************************
* Utility Function Bodies
************************************************************************/

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static char * _rtp_nativename_to_name (const char * name)
{
char * p;

    if (!name)
    {
        return (0);
    }

    for (p = (char *)name; *p; p++)
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
    return ((char *)name);
}


/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static char * _rtp_name_to_nativename (const char * name)
{
char * p;

    if (!name)
    {
        return (0);
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
        result |= VO_APPEND;
    }
    if (flag & RTP_FILE_O_RDONLY)
    {
        result |= VO_RDONLY;
    }
    if (flag & RTP_FILE_O_WRONLY)
    {
        result |= VO_WRONLY;
    }
    if (flag & RTP_FILE_O_RDWR)
    {
        result |= VO_RDWR;
    }
    if (flag & RTP_FILE_O_CREAT)
    {
        result |= VO_CREAT;
    }
    if (flag & RTP_FILE_O_TRUNC)
    {
        result |= VO_TRUNC;
    }
    if (flag & RTP_FILE_O_EXCL)
    {
        result |= VO_EXCL;
    }

    if (flag & RTP_FILE_O_BINARY)
    {
        result |= VO_BINARY;
    }
    else
    {
        result |= VO_TEXT;
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

    if (mode & RTP_FILE_S_IWRITE)
    {
        result |= VS_IWRITE;
    }
    if (mode & RTP_FILE_S_IREAD)
    {
        result |= VS_IREAD;
    }

    return (result);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
