/*
 | RTPLOG.C - Runtime Platform Logging Implementation
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBS - RT-Platform
 |
 |  $Author: pvanoudenaren $
 |  $Date: 2009/07817 15:29:00 $
 |  $Name:  $
 |  $Revision: 1.3 $
 |
 | Copyright EBS Inc. , 2009
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |   This module makes use of the RTPlatform ported modules to
 |   implement higher level file system functions.      __st__
 |
 | Functions implemented:
 |
 |
 |
 | Note:
 |
 |
 |
 |
*/


/************************************************************************
* Headers
************************************************************************/
#include "rtplog.h"
#ifdef RTP_ENABLE_LOGGING

#include "rtpprint.h"
#include "rtpgui.h"
#include "rtpstr.h"
#include "rtpfile.h"
#include "rtptime.h"

/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Structs
************************************************************************/

/************************************************************************
* Data
************************************************************************/
static RTP_HANDLE debugfd;
static int debugfd_valid;
static unsigned long start_system_msec;

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/

/************************************************************************
* Function Bodies
************************************************************************/


/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/


#define LOG_FILENAME "logfile.txt"

void rtp_log_open(void)
{
	if (rtp_file_open( &debugfd, LOG_FILENAME, RTP_FILE_O_RDWR|RTP_FILE_O_CREAT |RTP_FILE_O_TRUNC|RTP_FILE_O_BINARY, RTP_FILE_S_IWRITE|RTP_FILE_S_IREAD)==0)
	{
		start_system_msec = rtp_get_system_msec ();
		debugfd_valid = 1;
		rtp_printf("Logging enabled logging to file:%s\n", LOG_FILENAME);
	}
	else
	{
		rtp_printf("Logging disabled could not open log file:%s\n", LOG_FILENAME);
		debugfd_valid = 0;
	}
}

void rtp_log_close(void)
{
	if (debugfd_valid)
	{
		rtp_file_close(debugfd);
	}
}

void rtp_log_write(char *prompt, void *pData, long nBytes)
{
	if (debugfd_valid)
	{
	unsigned long elapsed;
	char outbuff[256];
        
		elapsed = rtp_get_system_msec () - start_system_msec;
		rtp_sprintf(outbuff,"\r\n ==<%s (t=%8d) (c=%4d) >== \r\n", (const unsigned char*) prompt,elapsed, nBytes);
		rtp_file_write (debugfd, (const unsigned char*) outbuff, (long) rtp_strlen(outbuff));
		if (pData && nBytes > 0)
		{
			rtp_file_write (debugfd, (const unsigned char*) pData, (long) nBytes);
			rtp_sprintf(outbuff,"\r\n ==<===========================================>==\r\n");
			rtp_file_write (debugfd, (const unsigned char*) outbuff, (long) rtp_strlen(outbuff));
		}
		rtp_file_flush (debugfd);
	}
}

#endif /* RTP_ENABLE_LOGGING */
