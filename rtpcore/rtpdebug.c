 /*
 | RTPDEBUG.C - Runtime Platform Debug Services
 |
 | EBSnet - EBS_RT-Platform 
 |
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  This module will output the debug information to memory.
 |  From within a debugger, dump the section of memory that 
 |  the global array refers to to see the latest debug 
 |  information. You can also extern this array so that it
 |  may be displayed in any form necessary.
*/


/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"
#include "rtpscnv.h"

 #ifdef RTP_DEBUG_MULTITHREAD
  #include "rtpsignl.h"
 #endif

/************************************************************************
* Configuration
************************************************************************/
/*#define RTP_DEBUG_OUTPUT_FILE_AND_LINE*/

/************************************************************************
* Defines
************************************************************************/
#define RTP_DEBUG_STRING_LEN 512

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/
/* string output messages are written to */
char gcRTPDebugOutputStr[RTP_DEBUG_STRING_LEN];

/* current position in the output string */
static int giRTPDebugOutputPos = 0;

#ifdef RTP_DEBUG_MULTITHREAD
 unsigned  rtpDebugInitialized = 0;
 RTP_MUTEX rtpDebugLock;
#endif

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                         rtp_debug_output_str
 *----------------------------------------------------------------------*/
void _rtp_debug_output_str (
	char* msg,
	const char *file,
	long line_num
	)
{
char* buffer;
int i;

  #ifdef RTP_DEBUG_MULTITHREAD
	if (_rtp_InitDebugOutput())
	{
		rtp_sig_mutex_claim(rtpDebugLock);
  #endif

	  #ifdef RTP_DEBUG_OUTPUT_FILE_AND_LINE
		buffer = file;
		for (i = 0; buffer[i]; i++)
		{
			if (giRTPDebugOutputPos >= RTP_DEBUG_STRING_LEN)
			{
				giRTPDebugOutputPos = 0;
			}
			gcRTPDebugOutputStr[giRTPDebugOutputPos++] = buffer[i];
		}
		
		if (line_num)
		{
			char tmpbuffer[33]; /* MAXINT can have 32 digits max, base 2 */
			
			rtp_itoa(line_num, tmpbuffer, 2);
			buffer = tmpbuffer;
			for (i = 0; buffer[i]; i++)
			{
				if (giRTPDebugOutputPos >= RTP_DEBUG_STRING_LEN)
				{
					giRTPDebugOutputPos = 0;
				}
				gcRTPDebugOutputStr[giRTPDebugOutputPos++] = buffer[i];
			}
		}
	  #endif

		buffer = msg;
		for (i = 0; buffer[i]; i++)
		{
			if (giRTPDebugOutputPos >= RTP_DEBUG_STRING_LEN)
			{
				giRTPDebugOutputPos = 0;
			}
			gcRTPDebugOutputStr[giRTPDebugOutputPos++] = buffer[i];
		}

  #ifdef RTP_DEBUG_MULTITHREAD
		rtp_sig_mutex_release(rtpDebugLock);
	}
  #endif
}


/*----------------------------------------------------------------------*
                         rtp_debug_output_int
 *----------------------------------------------------------------------*/
void _rtp_debug_output_int (
	long val
	)
{
char* buffer;
int i;

  #ifdef RTP_DEBUG_MULTITHREAD
	if (_rtp_InitDebugOutput())
	{
		rtp_sig_mutex_claim(rtpDebugLock);
  #endif
		
		if (val)
		{
			char tmpbuffer[33]; /* MAXINT can have 32 digits max, base 2 */
			
			rtp_itoa(val, tmpbuffer, 2);
			buffer = tmpbuffer;
			for (i = 0; buffer[i]; i++)
			{
				if (giRTPDebugOutputPos >= RTP_DEBUG_STRING_LEN)
				{
					giRTPDebugOutputPos = 0;
				}
				gcRTPDebugOutputStr[giRTPDebugOutputPos++] = (char)buffer[i];
			}
		}

  #ifdef RTP_DEBUG_MULTITHREAD
		rtp_sig_mutex_release(rtpDebugLock);
	}
  #endif
}


/************************************************************************
* Internal utility functions
************************************************************************/
unsigned _rtp_InitDebugOutput (void)
{
#ifdef RTP_DEBUG_MULTITHREAD
	if (!rtpDebugInitialized)
	{
		if (rtp_sig_mutex_alloc(&rtpDebugLock, 0) >= 0)
		{
			rtpDebugInitialized = 1;
		}
	}

	return (rtpDebugInitialized);
#else
	return (1);
#endif
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
