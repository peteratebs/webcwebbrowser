 /*
 | RTPKERN.H - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform 
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/

#ifndef __RTPKERN_H__
#define __RTPKERN_H__

#include "rtp.h"

/************************************************************************
 * Type definitions
 ************************************************************************/

/************************************************************************
 * Kernel API                                                           *
 ************************************************************************/

void rtp_kern_init (void);
void rtp_kern_run (void);
void rtp_kern_abort (void);
void rtp_kern_exit  (int exitvalue);


#endif /* __RTPKERN_H__ */

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
