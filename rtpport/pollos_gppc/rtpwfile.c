 /*
 | RTPWFILE.C - Runtime Platform Network Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:31 $
 |  $Name:  $
 |  $Revision: 1.3 $
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

/************************************************************************
* Defines
************************************************************************/
#if (RTP_LITTLE_ENDIAN)
#define BYTE_LOW  1 /* Numeric low byte of a character */
#define BYTE_HIGH 0 /* Numeric high byte of a character */
#else
#define BYTE_LOW  0 /* Numeric low byte of a character */
#define BYTE_HIGH 1 /* Numeric high byte of a character */
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
void map_unicode_to_ascii( unsigned char *to, unsigned char *from);
void map_ascii_to_unicode(unsigned char *to, unsigned char *from);
/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                               rtp_wfile_open
 *----------------------------------------------------------------------*/
int rtp_wfile_open (RTP_HANDLE  * fdPtr, const unsigned short * name, unsigned short flag, unsigned short mode)
{
	unsigned char realName[32];
	
	map_unicode_to_ascii(realName, (unsigned char *) name);
	return(rtp_file_open (fdPtr, realName, flag, mode));
}


/*----------------------------------------------------------------------*
                             rtp_wfile_rename
 *----------------------------------------------------------------------*/
int rtp_wfile_rename (unsigned short * name, unsigned short * newname)
{
	unsigned char realName[32];
	unsigned char newRealName[32];
	
	map_unicode_to_ascii((unsigned char *)realName, (unsigned char *)name);
	map_unicode_to_ascii((unsigned char *)newRealName,(unsigned char *) newname);
	return(rtp_file_rename (realName, newRealName));
}



/*----------------------------------------------------------------------*
                             rtp_wfile_delete
 *----------------------------------------------------------------------*/
int rtp_wfile_delete (unsigned short * name)
{
	unsigned char realName[32];
	
	map_unicode_to_ascii((unsigned char *)realName,(unsigned char *) name);
	return(rtp_file_delete ((unsigned char *)realName));
}


/*----------------------------------------------------------------------*
                             rtp_wfile_mkdir
 *----------------------------------------------------------------------*/
int rtp_wfile_mkdir (unsigned short * name)
{
	unsigned char realName[32];
	
	map_unicode_to_ascii((unsigned char *)realName,(unsigned char *) name);
	return(rtp_file_mkdir ((unsigned char *)realName));
}


/*----------------------------------------------------------------------*
                             rtp_wfile_rmdir
 *----------------------------------------------------------------------*/
int rtp_wfile_rmdir (unsigned short * name)
{

	unsigned char realName[32];
	
	map_unicode_to_ascii((unsigned char *)realName, (unsigned char *)name);
	return(rtp_file_rmdir ((unsigned char *)realName));
}


/*----------------------------------------------------------------------*
                             rtp_wfile_setcwd
 *----------------------------------------------------------------------*/
int rtp_wfile_setcwd (unsigned short * name)
{
	unsigned char realName[32];
	
	map_unicode_to_ascii((unsigned char *)realName, (unsigned char *)name);	
	return(rtp_file_setcwd ((unsigned char *)realName));
}



/*----------------------------------------------------------------------*
                             rtp_wfile_pwd
 *----------------------------------------------------------------------*/
int rtp_wfile_pwd (unsigned short * name, long size)
{
	unsigned char realName[32];
	if (rtp_file_pwd ((unsigned char *)realName, size) == -1)
	{
	  return (-1);
	}
	map_ascii_to_unicode((unsigned char *)name, realName);
	return(0);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_gfirst
 *----------------------------------------------------------------------*/
int rtp_wfile_gfirst (void ** dirobj, unsigned short * name)
{
	unsigned char realName[32];
	
	map_unicode_to_ascii((unsigned char *)realName,(unsigned char *) name);	
	return(rtp_file_gfirst (dirobj, realName));
}



/*----------------------------------------------------------------------*
                             rtp_wfile_gnext
 *----------------------------------------------------------------------*/
int rtp_wfile_gnext (void * dirobj)
{
	return(rtp_file_gnext (dirobj));
}



/*----------------------------------------------------------------------*
                           rtp_wfile_get_name
 *----------------------------------------------------------------------*/
int rtp_wfile_get_name (void * dirobj, unsigned short * name, int size)
{
	unsigned char realName[32];
	if (rtp_file_get_name (dirobj, realName, size) == -1)
	{
	  return (-1);
	}
	map_ascii_to_unicode((unsigned char *)name, (unsigned char *)realName);	
	return(0);
}


/*----------------------------------------------------------------------*
                           rtp_wfile_chmode
 *----------------------------------------------------------------------*/
int rtp_wfile_chmode (unsigned short * name, unsigned char attributes)
{
	unsigned char realName[32];
	
	map_unicode_to_ascii((unsigned char *)realName,(unsigned char *) name);
	return(rtp_file_chmode (realName, attributes));
}



/*----------------------------------------------------------------------*
                           rtp_wfile_get_free
 *----------------------------------------------------------------------*/
int rtp_wfile_get_free (unsigned short * name, unsigned long *total, unsigned long *free,
                       unsigned long *sectors_per_unit, unsigned short *bytes_per_sector)
{
	unsigned char realName[32];
	
	map_unicode_to_ascii((unsigned char *)realName,(unsigned char *) name);
	
	return(rtp_file_get_free (realName, total, free, sectors_per_unit, bytes_per_sector));
}



void map_unicode_to_ascii(unsigned char *to, unsigned char *from)
{
    unsigned char *b;
    b = to;
    while (*from || *(from+1))
    {
        if (*(from) != 0)
        {
            *to = *(from);
            to++;
        }
        from ++;
    }
    *to = 0;
    to = b;
}

void map_ascii_to_unicode(unsigned char *to, unsigned char *from)
{
    unsigned char *b;
    b = to;
    while (*from)
    {
		*to++ = (unsigned char)0;
		*to++ = *from++;
    }
    *to++ = 0;
    to = b;
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
