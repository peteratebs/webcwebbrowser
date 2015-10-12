 /*
 | RTPDEBUG.C - Runtime Platform Debug Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: tony $
 |  $Date: 2005/06/30 00:42:02 $
 |  $Name:  $
 |  $Revision: 1.2 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  If not running within a debugger that redirects OutputDebugString()
 |  it is recommended that an application similar to DebugView is 
 |  used to capture the output for later viewing.
 |
 |  DebugView is a freeware application that can be found at Sysinternals
 |  (www.sysinternals.com).  As described on www.sysinternals.com:
 |
 |     "DebugView is an application that lets you monitor debug 
 |      output on your local system, or any computer on the 
 |      network that you can reach via TCP/IP. It is capable of 
 |      displaying both kernel-mode and Win32 debug output, so 
 |      you don’t need a debugger to catch the debug output your 
 |      applications or device drivers generate, nor do you need 
 |      to modify your applications or drivers to use 
 |      non-standard debug output APIs."
*/


/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"
#include "rtpdebug.h"

#include <stdio.h>

/************************************************************************
* Configuration
************************************************************************/
/* #define RTP_DEBUG_OUTPUT_FILE_AND_LINE */

/************************************************************************
* Defines
************************************************************************/
#define RTP_DEBUG_STRING_LEN 4096

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
char buffer[RTP_DEBUG_STRING_LEN];
int len = 0;

#ifdef RTP_DEBUG_OUTPUT_FILE_AND_LINE
	len  = snprintf(buffer,       RTP_DEBUG_STRING_LEN,       "File: %s\n", file);
	len += snprintf(buffer + len, RTP_DEBUG_STRING_LEN - len, "Line: %d\n", line_num);
#endif

	len += snprintf(buffer + len, RTP_DEBUG_STRING_LEN - len, msg);
	fprintf(stdout, "%s", buffer);
}


/*----------------------------------------------------------------------*
                         rtp_debug_output_int
 *----------------------------------------------------------------------*/
void _rtp_debug_output_int (
	long val
	)
{
char buffer[RTP_DEBUG_STRING_LEN];
int len;

	len = snprintf(buffer, RTP_DEBUG_STRING_LEN, "%d", (int) val);
	fprintf(stdout, "%s", buffer);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
