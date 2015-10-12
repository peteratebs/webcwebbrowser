/*
|  RTPROT.H - 
| 
|  EBSnet - 
| 
|   $Author: tony $
|   $Date: 2004/08/23 16:20:47 $
|   $Name:  $
|   $Revision: 1.1 $
| 
|  Copyright EBSnet Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __RTPROT_H__
#define __RTPROT_H__

#ifdef __cplusplus
extern "C" {
#endif

unsigned long   rtp_lrotl    (unsigned long, int);
unsigned long   rtp_lrotr    (unsigned long, int);

#ifdef __cplusplus
}
#endif

#endif /* __RTPROT_H__ */
