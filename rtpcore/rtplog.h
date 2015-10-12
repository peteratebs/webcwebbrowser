 /*
 | RTPGUI.H - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBS - RT-Platform
 |
 |  $Author: vmalaiya $
 |  $Date: 2006/07/17 15:29:00 $
 |  $Name:  $
 |  $Revision: 1.3 $
 |
 | Copyright EBS Inc. , 2006
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/

#ifndef __RTPLOG_H__
#define __RTPLOG_H__

#include "rtp.h"


/************************************************************************
 * Terminal API                                                         *
 ************************************************************************/
/* Comment out to rebuilt the applistcation without logging */
#define RTP_ENABLE_LOGGING

#ifdef RTP_ENABLE_LOGGING
#ifdef __cplusplus
extern "C" {
#endif
void rtp_log_open(void);
void rtp_log_close(void);
void rtp_log_write(char *prompt, void *pData, long nBytes);
#ifdef __cplusplus
}
#endif
#endif


#ifdef RTP_ENABLE_LOGGING
#define RTP_LOG_OPEN() 			rtp_log_open();
#define RTP_LOG_CLOSE() 		rtp_log_close();
#define RTP_LOG_WRITE(A,B,C) 	rtp_log_write((char *) A, (void *) B, (long) C);
#else
#define RTP_LOG_OPEN()
#define RTP_LOG_CLOSE()
#define RTP_LOG_WRITE(A,B,C)
#endif

#endif /* __RTPLOG_H__ */

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
