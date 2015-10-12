 /*
 | RTPSTDUP.C - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/11/01 17:26:15 $
 |  $Name:  $
 |  $Revision: 1.1 $
 |
 | Copyright EBSnet Inc. , 2004
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
#include "rtpmem.h"
#include "rtpstdup.h"
#include "rtpstr.h"
#ifdef RTP_TRACK_LOCAL_MEMORY
 #include "rtpmemdb.h"
#endif

/************************************************************************
 * Type definitions
 ************************************************************************/

/************************************************************************
 * Data
 ************************************************************************/

/************************************************************************
 * API functions
 ************************************************************************/

/*---------------------------------------------------------------------------*/
char           * _rtp_strdup (const char *str)
{
	long len = (rtp_strlen(str) + 1) * sizeof(char);
	char *s = (char *) rtp_malloc(len);  /* SPRZ */
	if (s)
	{
		rtp_memcpy(s, str, len);
	}
	return (s);
}

/*---------------------------------------------------------------------------*/
unsigned short * _rtp_wcsdup (unsigned short *wstr)
{
	return (0);
}

#ifdef RTP_TRACK_LOCAL_MEMORY

char           * _rtp_debug_strdup (const char *str, const char *file, long line)
{
	long len = (rtp_strlen(str) + 1) * sizeof(char);
	char *s = (char *) _rtp_debug_malloc(len, _rtp_malloc, file, line);
	if (s)
	{
		rtp_memcpy(s, str, len);
	}

	return (s);
}

unsigned short * _rtp_debug_wcsdup (unsigned short *wstr, const char *file, long line)
{
	return (0);
}

#endif

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
