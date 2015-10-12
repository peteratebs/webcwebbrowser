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
#include "rtpwfile.h"

/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/
typedef __int64 time_t;
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

/************************************************************************
* Function Bodies
************************************************************************/

void rtp_not_yet_implemented()
{
}

/*----------------------------------------------------------------------*
                               rtp_wfile_open
 *----------------------------------------------------------------------*/
int rtp_wfile_open (RTP_HANDLE  * fdPtr, const unsigned short * name, unsigned short flag, unsigned short mode)
{
	rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_rename
 *----------------------------------------------------------------------*/
int rtp_wfile_rename (unsigned short * name, unsigned short * newname)
{
	rtp_not_yet_implemented ();
    return (-1);
}



/*----------------------------------------------------------------------*
                             rtp_wfile_delete
 *----------------------------------------------------------------------*/
int rtp_wfile_delete (unsigned short * name)
{
	rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_mkdir
 *----------------------------------------------------------------------*/
int rtp_wfile_mkdir (unsigned short * name)
{
	rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_rmdir
 *----------------------------------------------------------------------*/
int rtp_wfile_rmdir (unsigned short * name)
{
	rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_setcwd
 *----------------------------------------------------------------------*/
int rtp_wfile_setcwd (unsigned short * name)
{
	rtp_not_yet_implemented ();
    return (-1);
}



/*----------------------------------------------------------------------*
                             rtp_wfile_pwd
 *----------------------------------------------------------------------*/
int rtp_wfile_pwd (unsigned short * name, long size)
{
	rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_wfile_gfirst
 *----------------------------------------------------------------------*/
int rtp_wfile_gfirst (void ** dirobj, unsigned short * name)
{
	rtp_not_yet_implemented ();
    return (-1);
}



/*----------------------------------------------------------------------*
                             rtp_wfile_gnext
 *----------------------------------------------------------------------*/
int rtp_wfile_gnext (void * dirobj)
{
	rtp_not_yet_implemented ();
    return (-1);
}



/*----------------------------------------------------------------------*
                           rtp_wfile_get_name
 *----------------------------------------------------------------------*/
int rtp_wfile_get_name (void * dirobj, unsigned short * name, int size)
{
	rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_wfile_chmode
 *----------------------------------------------------------------------*/
int rtp_wfile_chmode (unsigned short * name, unsigned char attributes)
{
	rtp_not_yet_implemented ();
    return (-1);
}



/*----------------------------------------------------------------------*
                           rtp_wfile_get_free
 *----------------------------------------------------------------------*/
int rtp_wfile_get_free (unsigned short * name, unsigned long *total, unsigned long *free,
                       unsigned long *sectors_per_unit, unsigned short *bytes_per_sector)
{
	rtp_not_yet_implemented ();
    return (-1);
}


/************************************************************************
* Utility Function Bodies
************************************************************************/

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static int _rtp_windate_to_date (time_t * windate, RTP_DATE * rtpdate)
{
	rtp_not_yet_implemented ();
    return (-1);
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static int _rtp_date_to_windate (RTP_DATE * rtpdate, time_t * windate)
{
	rtp_not_yet_implemented ();
    return (-1);
}
/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static unsigned short * _rtp_unicode_name_to_winname (unsigned short * name)
{
	rtp_not_yet_implemented ();
    return (name);
}

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
static unsigned short * _rtp_unicode_winname_to_name (unsigned short * name)
{
	rtp_not_yet_implemented ();
    return (name);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
