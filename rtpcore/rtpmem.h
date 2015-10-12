 /*
 | RTPMEM.H - Runtime Platform Services
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

#ifndef __RTPMEM_H__
#define __RTPMEM_H__

#include "rtp.h"

#ifdef RTP_MEM_RTP_DEBUG_LOG_SIZE
#ifndef RTP_TRACK_LOCAL_MEMORY
#define RTP_TRACK_LOCAL_MEMORY
#endif
#endif

/************************************************************************
 * If RTP_TRACK_LOCAL_MEMORY is turned on, the debug implementaion of   *
 * the memory system will be implemented.  This provides debug          *
 * information otherwise not available by most platforms.               *
 ************************************************************************/
 
/* -------------------------------------------- */
/* Enable this flag to include memory tracking. */
/* -------------------------------------------- */
/* #define RTP_TRACK_LOCAL_MEMORY */

/************************************************************************
 * Memory System API
 ************************************************************************/

/************************************************************************
 * Memory System API which should be
 * used via the macros indicated below.
 ************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
                                                         
void * _rtp_malloc  (unsigned int size);
void * _rtp_calloc  (unsigned long num, unsigned int size);
void * _rtp_realloc (void * ptr, unsigned int size);
void   _rtp_free    (void * ptr);
 
#ifdef __cplusplus
}
#endif


#ifdef RTP_TRACK_LOCAL_MEMORY
#include "rtpmemdb.h"
#else
/************************************************************************
 * Memory System Debug API macros
 ************************************************************************/
#define rtp_malloc(size)  _rtp_malloc(size)
#define rtp_calloc(num, size)  _rtp_calloc(num, size)
#define rtp_realloc(ptr, size) _rtp_realloc(ptr, size)
#define rtp_free(ptr)          _rtp_free(ptr)
 
#define rtp_debug_print_mem_usage(logFile,mode)

#endif /* !RTP_TRACK_LOCAL_MEMORY */


#endif /* __RTPMEM_H__ */

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
