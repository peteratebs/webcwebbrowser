/*
|  RTPRAND.H - Runtime Platform Pseudo-random number generator
| 
|  EBSnet - 
| 
|   $Author: tony $
|   $Date: 2004/08/23 16:20:47 $
|   $Name:  $
|   $Revision: 1.2 $
| 
|  Copyright EBSnet Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __RTPRAND_H__
#define __RTPRAND_H__

/*#define rtp_rand  */
/*#define rtp_srand */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef rtp_rand
int  rtp_rand  (void);
#endif
#ifndef rtp_srand
void rtp_srand (unsigned int seed);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RTPRAND_H__ */
