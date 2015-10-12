 /*
 | RTPMEM.H - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform
 |
 |  $Author: tony $
 |  $Date: 2005/07/12 15:41:15 $
 |  $Name:  $
 |  $Revision: 1.7 $
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
#define RTP_TRACK_LOCAL_MEMORY

#ifdef RTP_MEM_DEBUG_LOG_SIZE
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
//#define RTP_TRACK_LOCAL_MEMORY


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

void * _rtp_malloc  (unsigned long size);
void * _rtp_calloc  (unsigned long num, unsigned long size);
void * _rtp_realloc (void * ptr, unsigned long size);
void   _rtp_free    (void * ptr);

/* Allocate routines manage heap in fast internal ram. 320 * 240 bytes in the heap now, need to balance with using fast memory for code */
#ifdef __TIARM__
void * _rtp_fast_malloc ( unsigned long size );
void * _rtp_fast_realloc ( void * ptr, unsigned long size);
void _rtp_fast_free (void * ptr);
#endif


#ifdef __cplusplus
}
#endif



#ifdef RTP_TRACK_LOCAL_MEMORY
#include "rtpmemdb.h"
#else


/************************************************************************
 * If RTP_TRACK_LOCAL_MEMORY is 0, memory access will be directed to    *
 * the rtpmem.c porting file and the memory debug utility functions     *
 * will be defined to nothing to remove their use.                      *
 ************************************************************************/
#define rtp_malloc(size)       _rtp_malloc(size)
#define rtp_calloc(num, size)  _rtp_calloc(num, size)
#define rtp_realloc(ptr, size) _rtp_realloc(ptr, size)
#define rtp_free(ptr)          _rtp_free(ptr)
#define rtp_debug_print_mem_usage(logFile,mode,message)

#define RTP_DEBUG_LOG_MEM_USAGE(M)
#define RTP_DEBUG_LOG_MEM_CHANGES(M)
#define WEBC_DEBUG_LOG_MEM_USAGE(M)
#define WEBC_DEBUG_LOG_MEM_CHANGES(M)

#endif /* !RTP_TRACK_LOCAL_MEMORY */

// Memdbstats structure populate by calling wcMemDbGetStats 
typedef struct s_MemDbStats  {
	unsigned long MemAlloced;     // Total bytes allocated since startup
	unsigned long MemFreed;       // Total bytes freed since startup
    unsigned long MemUsed;        // Total bytes currently in use
    unsigned long MaxMemUsed;     // High water mark of maximum bytes ever used.
	unsigned long MemClipped;     // Number of times a memory allocation failed (in debug mode an assert is also generated).
	unsigned long MemNewed;       // Total number of bytes currently allocated by the new operator and not yet released by the free operator. Note: what about detail about newed data.
	unsigned long MallocSequence; // Current sequence number, keep this around if you with to later on report on a range of memory allocattion calls starting from this point.
	unsigned long MaxUsageSeq;	// Sequence number of the allocation when the maximum allocation was reached. (see RTPMemBlockHeader);
#ifdef RTP_TRACK_LOCAL_MEMORY
	// This list provides access to all current oustanding allocations in a linked list.
	// see /Ref{wcMemDbGetChangesSinceWatchPoint} for a description of the RTPMemBlockHeader structure
	// MemBlockList is a pointer to the most recently added block.          
	// follow the chain through the prev pointers to get to the other blocks. 
	RTPMemBlockHeader *MemBlockList; // A list of the currently outstanding memory starting with the most recent allocation. 
#endif
} MemDbStats;



#endif /* __RTPMEM_H__ */

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
