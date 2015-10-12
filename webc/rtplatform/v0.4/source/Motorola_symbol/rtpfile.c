 /*
 | RTPFILE.C - Runtime Platform Network Services
 |
 |   PORTED TO THE ERTFS PLATFORM
 |
 | EBS - RT-Platform
 |
 |  $Author:
 |  $Date:
 |  $Name:
 |  $Revision:
 |
 | Copyright EBS Inc. , 2010
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
#include "rtfs.h"
#include "rtpprint.h"

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
static char *     _rtp_ertfsname_to_name  (char * name);
static char *     _rtp_name_to_ertfsname  (const char * name);
static int        _rtp_flag_to_operation  (unsigned short mode);
static int        _rtp_mode_to_permission (unsigned short mode);

/************************************************************************
* Function Bodies
************************************************************************/
/*----------------------------------------------------------------------*
                              rtp_file_init
*-----------------------------------------------------------------------*/
int rtp_file_init(void)
{
static int isInited = 0;
        if(isInited == 0)
		{
		    isInited = 1;
			pc_ertfs_run();
		}
		return 0;
}

/*----------------------------------------------------------------------*
                              rtp_file_open
*----------------------------------------------------------------------*/
int rtp_file_open (RTP_HANDLE * fdPtr, const char * name, unsigned short flag, unsigned short mode)
{
int fileHandle;
int result;

#ifdef RTP_DISPLAY_ERROR
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	name = _rtp_name_to_ertfsname ((char *)name);
	fileHandle = po_open ((byte *)name, _rtp_flag_to_operation(flag), _rtp_mode_to_permission(mode));
//rtp_printf("Opened file name %s returned %d\n", name, fileHandle);

	if (fileHandle == (int)(-1))
	{
		result = get_errno();
		/* ----------------------------------- */
		/* If trying to open a directory or */
		/* opening a read only file with */
		/* write privilages. This can be */
		/* non-fatal if doing an open to */
		/* determine the existance of a */
		/* directory. */
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
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	if (po_close ((int) fileHandle) != 0)
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
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
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	if ((result = po_read ((int) fileHandle, buffer, (int) count)) < 0)
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
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
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	if ((result = po_write ((int) fileHandle, (byte *)buffer, (int) count)) < 0)
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
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
int relative_to;

#ifdef RTP_DISPLAY_ERROR
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	switch (origin)
	{
		case 0:
			relative_to = PSEEK_SET;
			break;
		case 1:
			relative_to = PSEEK_CUR;
			break;
		case 2:
			relative_to = PSEEK_END;
			break;
		default:
#ifdef RTP_DISPLAY_ERROR
			rtfs_set_errno (PEINVALIDPARMS);
			result = get_errno ();
			rtp_term_cputs("rtp_file_lseek: error returned ");
			rtp_term_puts(rtp_itoa(result, error, 10));
#endif
			return (-1);
	}

	if ((result = (long) po_lseek ((int) fd, offset, relative_to)) < 0)
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
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
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	if (po_chsize((int) fd, offset) < 0)
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
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
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	if (!po_flush((int) fd))
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
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
int rtp_file_rename (const char * name, char * newname)
{
	byte * ertfsname;
	byte * ertfsnewname;
#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	ertfsname = (byte *)_rtp_name_to_ertfsname (name);
	ertfsnewname = (byte *) _rtp_name_to_ertfsname (newname);

	if (!pc_mv (ertfsname, ertfsnewname))
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
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
int rtp_file_delete (const char * name)
{
	byte * ertfsname;
#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	ertfsname = (byte *) _rtp_name_to_ertfsname (name);
//rtp_printf("removing %s\n", name);

	if (!pc_unlink (ertfsname))
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
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
int rtp_file_mkdir (const char * name)
{
	byte * ertfsname;
#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	ertfsname = (byte *)_rtp_name_to_ertfsname (name);

	if (!pc_mkdir (ertfsname))
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
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
int rtp_file_rmdir (const char * name)
{
	byte * ertfsname;
#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	ertfsname = (byte *) _rtp_name_to_ertfsname (name);

	if (!pc_rmdir (ertfsname))
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
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
int rtp_file_setcwd (const char * name)
{
	byte * ertfsname;
#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	ertfsname = (byte *) _rtp_name_to_ertfsname (name);

	if (!pc_set_cwd (ertfsname))
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
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
#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	if (!pc_pwd ((byte *) 0, (byte *)name))
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
		rtp_term_cputs("rtp_file_pwd: error returned ");
		rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}

	name = _rtp_ertfsname_to_name (name);
	return (0);
}


/*----------------------------------------------------------------------*
                              rtp_file_chmode
*----------------------------------------------------------------------*/
int rtp_file_chmode (const char * name, unsigned char attributes)
{
int ertfsmode;
byte * ertfsname;

#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	ertfsmode = 0;
	ertfsmode |= (attributes & RTP_FILE_ATTRIB_RDONLY) ? ARDONLY : 0;
	ertfsmode |= (attributes & RTP_FILE_ATTRIB_ARCHIVE) ? ARCHIVE : 0;
	ertfsmode |= (attributes & RTP_FILE_ATTRIB_HIDDEN) ? AHIDDEN : 0;
	ertfsmode |= (attributes & RTP_FILE_ATTRIB_SYSTEM) ? ASYSTEM : 0;
	ertfsname = (byte *) _rtp_name_to_ertfsname (name);

	if (!pc_set_attributes (ertfsname, ertfsmode))
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
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
int rtp_file_get_free (const char * name, unsigned long *total, unsigned long *free,
unsigned long *sectors_per_unit, unsigned short *bytes_per_sector)
{
dword total_blocks, free_blocks;
long bytes_free;
byte * ertfsname;

#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	ertfsname = (byte *)_rtp_name_to_ertfsname (name);
	bytes_free = pc_free ((byte *)ertfsname, &total_blocks, &free_blocks);

	if (bytes_free < 0)
	{
#ifdef RTP_DISPLAY_ERROR
		result = get_errno ();
		rtp_term_cputs("rtp_file_get_free: error returned ");
		rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}

	*total = (unsigned long) total_blocks;
	*free = (unsigned long) free_blocks;
	*sectors_per_unit = (unsigned long) 1;

	if(free_blocks)
	{
		*bytes_per_sector = (unsigned short) ((bytes_free / free_blocks) & 0xffff);
	}
	else
	{
		*bytes_per_sector = (unsigned short) 0;
	} /* _yi_ */

	return (0);
}


/*----------------------------------------------------------------------
----------------------------------------------------------------------*/

int rtp_file_set_time (RTP_HANDLE fd, RTP_DATE * adate, RTP_DATE * wdate, RTP_DATE * cdate, RTP_DATE * hdate)
{
	/* ----------------------------------- */
	/* Not supported by the ertfs fs. */
	/* rtp_not_yet_implemented (); */
	/* ----------------------------------- */
	return (0); /* TRUE returned */
}


/*----------------------------------------------------------------------*
                           rtp_file_get_path_seperator
 *----------------------------------------------------------------------*/
char rtp_file_get_path_seperator (void)
{
    return ('\\');
}


/************************************************************************
* Utility Function Bodies
************************************************************************/

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


/*----------------------------------------------------------------------
flag - for the flag argument:
RTP_FILE_O_APPEND All writes will be appended to the file
RTP_FILE_O_RDONLY Open a file for reading only (default)
RTP_FILE_O_WRONLY Open a file for writing only
RTP_FILE_O_RDWR Open a file for reading and writing
RTP_FILE_O_CREAT Create a file if it does not exist
RTP_FILE_O_TRUNC Truncate a file to 0 bytes after opening
RTP_FILE_O_EXCL If creating a file, fail if it already exists
----------------------------------------------------------------------*/

static int _rtp_flag_to_operation (unsigned short flag)
{
int result = 0;

	if (flag & RTP_FILE_O_APPEND)
	{
		result |= PO_APPEND;
	}
	if (flag & RTP_FILE_O_RDONLY)
	{
		result |= PO_RDONLY;
	}
	if (flag & RTP_FILE_O_WRONLY)
	{
		result |= PO_WRONLY;
	}
	if (flag & RTP_FILE_O_RDWR)
	{
		result |= PO_RDWR;
	}
	if (flag & RTP_FILE_O_CREAT)
	{
		result |= PO_CREAT;
	}
	if (flag & RTP_FILE_O_TRUNC)
	{
		result |= PO_TRUNC;
	}
	if (flag & RTP_FILE_O_EXCL)
	{
		result |= PO_EXCL;
	}

	if (result == 0)
	{
		result = PO_RDONLY;
	}

	return (result);
}


/*----------------------------------------------------------------------
mode - for the mode argument:
RTP_FILE_S_IWRITE Create a file with write permissions
RTP_FILE_S_IREAD Create a file with read permissions
----------------------------------------------------------------------*/

static int _rtp_mode_to_permission (unsigned short mode)
{
int result = 0;

	/* ----------------------------------- */
	/* Because of a limitation of windows */
	/* _S_IWRITE is the same as _S_IREAD */
	/* or'ed with _S_IWRITE. There is no */
	/* write only permission. */
	/* ----------------------------------- */

	if (mode & RTP_FILE_S_IWRITE)
	{
		result |= PS_IWRITE;
	}
	if (mode & RTP_FILE_S_IREAD)
	{
		result |= PS_IREAD;
	}

	return (result);
}


/* ----------------------------------- */
/* END OF FILE */
/* ----------------------------------- */
