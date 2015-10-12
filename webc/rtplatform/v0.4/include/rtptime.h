 /*
 | RTPTIME.H - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:30 $
 |  $Name:  $
 |  $Revision: 1.2 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/

#ifndef __RTPTIME_H__
#define __RTPTIME_H__

#include "rtp.h"


/************************************************************************
 * API functions
 ************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

unsigned long rtp_get_system_msec (void);
unsigned long rtp_get_system_sec  (void);

#ifdef __cplusplus
}
#endif

#endif /*__RTPTIME_H__*/

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
