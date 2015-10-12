 /*
 | RTPWFILE.C - Runtime Platform Network Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - RT-Platform
 |
 |  $Author: vmalaiya $
 |  $Date: 2005/01/07 15:18:48 $
 |  $Name:  $
 |  $Revision: 1.1 $
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
#include "rtpwfile.h"
#include "rtpwcs.h"
#include "rtpstr.h"
#include "rtfs.h"
#include "rtpmem.h"
#include "rtpterm.h"
#ifdef RTP_DISPLAY_ERROR
#include "rtpterm.h"
#include "rtpscnv.h"
#endif
//#include "rtfsapi.h"

/************************************************************************
* Defines
************************************************************************/

#define MAX_FILENAME_SIZE	180

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
static char * _rtp_ertfsname_to_name (char * name);
static char * _rtp_name_to_ertfsname (char * name);
static int _rtp_flag_to_operation (unsigned short flag);
static int _rtp_mode_to_permission (unsigned short mode);
static void map_ascii_to_unicode(byte *unicode_to, byte *ascii_from);
static void map_unicode_to_ascii(byte *to, byte *from);

/************************************************************************
* Function Bodies
************************************************************************/
#define byte unsigned char
static void map_ascii_to_unicode(byte *unicode_to, byte *ascii_from)
{
byte *p;

    p = unicode_to;
    while (*ascii_from)
    {
        *(p+BYTE_LOW) = 0;
        *(p+BYTE_HIGH) = *ascii_from++;
        p+= 2;
    }
    *p++ = 0;
    *p++ = 0;
}

static void map_unicode_to_ascii(byte *to, byte *from)
{
    while (*from || *(from+1))
    {
        if (*(from+BYTE_LOW) == 0)
            *to++ = *(from+BYTE_HIGH);
        else
            *to++ = '_';
        from += 2;
    }
    *to = 0;
}

/*----------------------------------------------------------------------*
                               rtp_wfile_open
 *----------------------------------------------------------------------*/
int rtp_wfile_open (RTP_HANDLE  * fdPtr, const unsigned short * name, unsigned short flag, unsigned short mode)
{
PCFD fileHandle;

char ertfsName[MAX_FILENAME_SIZE] = {0};
char * temp = ertfsName;

#ifdef RTP_DISPLAY_ERROR
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	/* Convert unicode string to ASCII */
	map_unicode_to_ascii((byte *)ertfsName, (byte *) name);	

	temp = _rtp_name_to_ertfsname ((char *)temp);	
	
	fileHandle = po_open ((char *)ertfsName, _rtp_flag_to_operation(flag), _rtp_mode_to_permission(mode));
	
	if (fileHandle == (PCFD)(-1))
	{
		int result;

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
			//rtp_term_puts(rtp_ltoa(result, error, 10));
#endif
			return (-2);
		}
#ifdef RTP_DISPLAY_ERROR
		rtp_term_cputs("rtp_file_open: error returned ");
		//rtp_term_puts(rtp_itoa(result, error, 10));
	#endif
		
		return (-1);
	}

	*fdPtr = (RTP_HANDLE) fileHandle;
	
	return (0);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_rename
 *----------------------------------------------------------------------*/
int rtp_wfile_rename (unsigned short * name, unsigned short * newname)
{
char ertfsName1[MAX_FILENAME_SIZE] = {0};
char * temp1 = ertfsName1;

char ertfsName2[MAX_FILENAME_SIZE] = {0};
char * temp2 = ertfsName2;

#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif
	
	/* Convert unicode string to ASCII */
	map_unicode_to_ascii((byte *)ertfsName1, (byte *) name);	

	temp1 = _rtp_name_to_ertfsname ((char *)temp1);	
	
	/* Convert unicode string to ASCII */
	map_unicode_to_ascii((byte *)ertfsName2, (byte *) name);	

	temp2 = _rtp_name_to_ertfsname ((char *)temp2);	
	
	if (!pc_mv ((byte *)ertfsName2, (byte *)ertfsName1))
	{
#ifdef RTP_DISPLAY_ERROR
		int result;

		result = get_errno ();
		rtp_term_cputs("rtp_file_rename: error returned ");
		//rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}

	return (0);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_delete
 *----------------------------------------------------------------------*/
int rtp_wfile_delete (unsigned short * name)
{
char ertfsName[MAX_FILENAME_SIZE] = {0};
char * temp = ertfsName;

#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	/* Convert unicode string to ASCII */
	map_unicode_to_ascii((byte *)ertfsName, (byte *) name);	

	temp =_rtp_name_to_ertfsname (temp);	
	
	if (!pc_unlink ((byte*)ertfsName))
	{
#ifdef RTP_DISPLAY_ERROR
		int result;

		result = get_errno ();
		rtp_term_cputs("rtp_file_delete: error returned ");
		//rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}
	
	return (0);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_mkdir
 *----------------------------------------------------------------------*/
int rtp_wfile_mkdir (unsigned short * name)
{

char ertfsName[MAX_FILENAME_SIZE] = {0};
char * temp = ertfsName;

#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif
	
	/* Convert unicode string to ASCII */
	map_unicode_to_ascii((byte *)ertfsName, (byte *) name);	

	temp = _rtp_name_to_ertfsname ((char *)temp);	
	
    if (!pc_mkdir ((byte*)ertfsName))
	{
#ifdef RTP_DISPLAY_ERROR
		int result;
		
		result = get_errno ();
		rtp_term_cputs("rtp_file_mkdir: error returned ");
//		rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}
	return (0);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_rmdir
 *----------------------------------------------------------------------*/
int rtp_wfile_rmdir (unsigned short * name)
{

char ertfsName[MAX_FILENAME_SIZE] = {0};
char * temp = ertfsName;

#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif
	
	/* Convert unicode string to ASCII */
	map_unicode_to_ascii((byte *)ertfsName, (byte *) name);	

	temp = _rtp_name_to_ertfsname (temp);	
	
    
	if (!pc_rmdir ((byte*)ertfsName))
	{
#ifdef RTP_DISPLAY_ERROR
		int result;

		result = get_errno ();
		rtp_term_cputs("rtp_file_rmdir: error returned ");
//		rtp_term_puts(rtp_itoa(result, error, 10));
#endif

		return (-1);
	}
	return (0);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_setcwd
 *----------------------------------------------------------------------*/
int rtp_wfile_setcwd (unsigned short * name)
{

char ertfsName[MAX_FILENAME_SIZE] = {0};
char * temp = ertfsName;

#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	/* Convert unicode string to ASCII */
	map_unicode_to_ascii((byte *)ertfsName, (byte *) name);	

	temp = _rtp_name_to_ertfsname ((char *)temp);	
	
	if (!pc_set_cwd ((byte*)ertfsName))
	{
#ifdef RTP_DISPLAY_ERROR
		int result;

		result = get_errno ();
		rtp_term_cputs("rtp_file_setcwd: error returned ");
//		rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}
	return (0);
}



/*----------------------------------------------------------------------*
                             rtp_wfile_pwd
 *----------------------------------------------------------------------*/
int rtp_wfile_pwd (unsigned short * name, long size)
{

char ertfsName[MAX_FILENAME_SIZE] = {0};
char * temp = ertfsName;


#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	/* Convert unicode string to ASCII */
	map_unicode_to_ascii((byte *)ertfsName, (byte *) name);	

	temp = _rtp_name_to_ertfsname ((char *)temp);	

	if (!pc_pwd ((byte *) 0, (byte*)ertfsName))
	{
#ifdef RTP_DISPLAY_ERROR
		int result;

		result = get_errno ();
		rtp_term_cputs("rtp_file_pwd: error returned ");
//		rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}
	return (0);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_gfirst
 *----------------------------------------------------------------------*/
int rtp_wfile_gfirst (void ** dirobj, unsigned short * name)
{
DSTAT *fdata ;
char ertfsName[MAX_FILENAME_SIZE] = {0};
char * temp = ertfsName;

#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	/* Convert unicode string to ASCII */
	map_unicode_to_ascii((byte *)ertfsName, (byte *) name);	

	temp = _rtp_name_to_ertfsname ((char *)temp);	

	fdata = (DSTAT*) rtp_malloc(sizeof(DSTAT));
    memset(fdata, 0, sizeof(DSTAT));
    
	if (!pc_gfirst (fdata, (byte*)ertfsName))
	{
#ifdef RTP_DISPLAY_ERROR
		int result;

		result = get_errno ();
		rtp_term_cputs("rtp_file_gfirst: error returned ");
//		rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		rtp_free (fdata);
		return (-1);
	}
	
	//_rtp_ertfs_to_native(&fdata->fsObject, &ertfsNative);
	*dirobj = (void *)fdata;
	return (0);
}



/*----------------------------------------------------------------------*
                             rtp_wfile_gnext
 *----------------------------------------------------------------------*/
int rtp_wfile_gnext (void * dirobj)
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
		int result;

		result = get_errno ();
		rtp_term_cputs("rtp_file_gnext: error returned ");
		//rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}
	//map_ascii_to_unicode(to,dirobj->);
	return (0);
}

/*----------------------------------------------------------------------*
                           rtp_wfile_get_name
 *----------------------------------------------------------------------*/
int rtp_wfile_get_name (void * dirobj, unsigned short * name, int size)
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
		int result;

		rtfs_set_errno (PEINVALIDPARMS);
		result = get_errno ();
		rtp_term_cputs("rtp_file_get_name: error returned ");
		//rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}

#if (VFAT)
	sizelimit = rtp_strlen(((DSTAT *)dirobj)->lfname);
	if (sizelimit > size)
	{
		sizelimit = size;
	}
	map_ascii_to_unicode((byte*)name, (byte *) (((DSTAT *)dirobj)->lfname));
	//strncpy ((byte*)name, (const char *) (((DSTAT *)dirobj)->lfname), sizelimit);
#else
	sizelimit = strlen(((DSTAT *)dirobj)->filename);
	if (sizelimit > size)
	{
		sizelimit = size;
	}
	map_ascii_to_unicode((byte*)name, (byte *) (((DSTAT *)dirobj)->lfname));
	//strncpy (name, (const char *) (((DSTAT *)dirobj)->filename), sizelimit);
#endif

	return (0);
}


/*----------------------------------------------------------------------*
                           rtp_wfile_chmode
 *----------------------------------------------------------------------*/
int rtp_wfile_chmode (unsigned short * name, unsigned char attributes)
{
int ertfsmode;
char ertfsName[MAX_FILENAME_SIZE] = {0};
char * temp = ertfsName;

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
	
	/* Convert unicode string to ASCII */
	map_unicode_to_ascii((byte *)ertfsName, (byte *) name);	

	temp = _rtp_name_to_ertfsname (temp);	

	if (!pc_set_attributes ((byte*)ertfsName, ertfsmode))
	{
#ifdef RTP_DISPLAY_ERROR
		int result;

		result = get_errno ();
		rtp_term_cputs("rtp_file_chmode: error returned ");
//		rtp_term_puts(rtp_itoa(result, error, 10));
#endif
		return (-1);
	}
	return (0);
}




/*----------------------------------------------------------------------*
                           rtp_wfile_get_free
 *----------------------------------------------------------------------*/
int rtp_wfile_get_free (unsigned short * name, unsigned long *total, unsigned long *free,
                       unsigned long *sectors_per_unit, unsigned short *bytes_per_sector)
{
dword total_blocks, free_blocks;
long bytes_free;
char ertfsName[MAX_FILENAME_SIZE] = {0};
char * temp = ertfsName;

#ifdef RTP_DISPLAY_ERROR
	int result;
	char error[32];

	/* ----------------------------------- */
	/* Clear the error state by setting */
	/* to 0. */
	/* ----------------------------------- */
	rtfs_set_errno (0);
#endif

	/* Convert unicode string to ASCII */
	map_unicode_to_ascii((byte *)ertfsName, (byte *) name);	

	temp = _rtp_name_to_ertfsname (temp);	

	bytes_free = pc_free ((byte*)ertfsName, &total_blocks, &free_blocks);

	if (bytes_free < 0)
	{
#ifdef RTP_DISPLAY_ERROR
		int result;

		result = get_errno ();
		rtp_term_cputs("rtp_file_get_free: error returned ");
//		rtp_term_puts(rtp_itoa(result, error, 10));
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
		*bytes_per_sector = 0;
	}
	return (0);
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/

int rtp_wfile_set_time (RTP_HANDLE fd, RTP_DATE * adate, RTP_DATE * wdate, RTP_DATE * cdate, RTP_DATE * hdate)
{
	/* ----------------------------------- */
	/* Not supported by the ertfs fs. */
	/* rtp_not_yet_implemented (); */
	/* ----------------------------------- */
	return (0); /* TRUE returned */
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
static char * _rtp_name_to_ertfsname (char * name)
{
char * p;

	if (!name)
	{
		return ((char *)0);
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