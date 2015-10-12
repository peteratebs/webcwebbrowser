 /*
 | RTP.H - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: tony $
 |  $Date: 2005/07/12 15:41:15 $
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

#ifndef __RTP_H__
#define __RTP_H__


/* ----------------------------------- */
/*  Shared definitions for the         */
/*  RT-Platform interface              */
/* ----------------------------------- */
/* FOR EXAMPLE: (RTPLATFORM_VERSION)
		0004 == 00.04 == 0.4
		0201 == 02.01 == 2.1
		0415 == 04.15 == 4.15
		1003 == 10.03 == 10.3 
*/
#define RTPLATFORM_VERSION             0004 /* 0.4 */
#define RTP_TIMEOUT_INFINITE           -1

/* ----------------------------------- */
/* Various platform-specific defines.  */
/* ----------------------------------- */
typedef long RTP_HANDLE;

/************************************************************************
 * API functions                                                        *
 ************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void rtp_not_yet_implemented (void);

#ifdef __cplusplus
}
#endif

#endif /*__RTP_H__*/



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
