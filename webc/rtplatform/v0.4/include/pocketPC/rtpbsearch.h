/*
|  RTPBSEARCH.H - 
| 
|  EBSnet - 
| 
|   $Author: tony $
|   $Date: 2004/12/21 22:13:39 $
|   $Name:  $
|   $Revision: 1.1 $
| 
|  Copyright EBSnet Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __RTPBSEARCH_H__
#define __RTPBSEARCH_H__

#include <stdlib.h>
#include <search.h>

#define rtp_bsearch bsearch

#ifdef __cplusplus
extern "C" {
#endif

#ifndef rtp_bsearch
typedef int (*RTP_BSEARCH_COMPARISON_FN)(const void *node1, const void *node2);
void* rtp_bsearch (const void *obj, const void *head, unsigned int num, unsigned int size, int (*compfunc)(const void *node1, const void *node2));
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RTPBSEARCH_H__ */
