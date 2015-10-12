 /*
 | RTPMEMDB.C - Runtime Platform Memory System Debug Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE (except for debug defines)
 |
 | EBSnet - RT-Platform
 |
 |  $Author: tony $
 |  $Date: 2005/07/12 15:41:15 $
 |  $Name:  $
 |  $Revision: 1.12 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/

/* define this symbol to enable memory debugging in a multi-threaded system */
/*#define RTP_MEM_DEBUG_MULTITHREAD*/

/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"
#include "rtpmem.h"

#ifdef RTP_TRACK_LOCAL_MEMORY

 #ifdef RTP_MEM_DEBUG_MULTITHREAD
  #include "rtpsignl.h"
 #endif

 #ifdef RTP_MEM_DEBUG_FILE
  #ifdef RTP_MEM_DEBUG_USE_STDIO
   #include <stdio.h>
  #else
   #include "rtpfio.h"
  #endif
 #endif

 #ifdef RTP_MEM_DEBUG_PRINTF
  #include "rtpprint.h"
 #endif

 #include "rtpstr.h"
 #include "rtprand.h"

/************************************************************************
* Defines
************************************************************************/
#define RTP_MEM_GUARD_BYTE          0xab
#define RTP_MEM_BLOCK_ID_BYTE       0xcd
#define RTP_MEM_PREPAD_BYTE         0xef
#define RTP_MEM_BLOCK_PREPAD_BYTE   0x01

#ifdef RTP_MEM_DEBUG_FILE
 #ifdef RTP_MEM_DEBUG_USE_STDIO
  #define RTP_MEM_FOPEN(F,N,M)  (((*(F) = fopen(N,M)) != 0)? 0 : -1)
  #define RTP_MEM_FPRINTF       fprintf
  #define RTP_MEM_FCLOSE        fclose
 #else
  #define RTP_MEM_FOPEN(F,N,M) rtp_stdio_fopen(F,N,M)
  #define RTP_MEM_FPRINTF      rtp_stdio_fprintf
  #define RTP_MEM_FCLOSE       rtp_stdio_fclose
 #endif
#endif

#define CSV_STR    "\"%s\""
#define CSV_DEC    "\"%d\""
#define CSV_PTR    "\"0x%08x\""

/************************************************************************
* Structures
************************************************************************/


#ifdef RTP_MEM_DEBUG_LOG_SIZE
typedef struct _RTPMemAllocRecord
{
	void*          location;
	unsigned long  size;
	const char*    allocedFromFile;
	long           allocedFromLine;
	unsigned long  allocedAtTime;
	const char*    freedFromFile;
	long           freedFromLine;
	unsigned long  freedAtTime;
	int            freed;
} RTPMemAllocRecord;
#endif

/************************************************************************
* Global Data
************************************************************************/
static unsigned long giMemLimit       = 1024*1024*4;
static unsigned long giMemBase        = 400*1024;
static unsigned char gcMallocFailRate = 15; /* out of 1000 */
static unsigned long giMemAlloced     = 0;
static unsigned long giMemFreed       = 0;
static          long giMemUsed        = 0;
static          long giPrevMemAndNewUsed   = 0;		/* For printing change in memory use run to run */
static unsigned long giMaxMemUsed     = 0;
static unsigned long giMemClipped     = 0;
static unsigned long giMemNewed     	= 0;

unsigned long guMallocSequence = 0;
unsigned long guMaxUsageSeq    = 0;
unsigned long guLastPrintedSequence = 0;

/* gpMemBlockList is a pointer to the most recently added block.          */
/* follow the chain through the prev pointers to get to the other blocks. */
RTPMemBlockHeader *gpMemBlockList = 0;

void (*gMemHighWaterCallbackFn)(const char *, long, void *) = 0;
void* gpMemHighWaterCallbackCtx         = 0;
unsigned long gpMemHighWaterThreshhold;

#ifdef RTP_MEM_DEBUG_LOG_SIZE
static unsigned long     rtpMemDebugLogTime = 0;
static long              rtpMemDebugLogIndex = 0;
static int               rtpMemDebugLogWrap = 0;
int                      rtpMemDebugLogPrint = 0;
void*                    rtpMemDebugLogPrintFilter = 0;
static RTPMemAllocRecord rtpMemDebugLog[RTP_MEM_DEBUG_LOG_SIZE];
#endif

#ifdef RTP_MEM_DEBUG_MULTITHREAD
 unsigned  rtpMemDebugInitialized = 0;
 RTP_MUTEX rtpMemDebugLock;
#endif

/************************************************************************
* Function Prototypes
************************************************************************/
static unsigned      _rtp_InitMemDebug  (void);
static void          _rtp_AddMemBlock   (void *ptr, unsigned long size, const char *file, long line,const char *comment,unsigned long flags);
static void          _rtp_FreeMemBlock  (void *ptr, const char* file,  long line);
static unsigned int  _rtp_CheckMemBlock (RTPMemBlockHeader *pBlock);

/************************************************************************
* Function Bodies
************************************************************************/



/*----------------------------------------------------------------------*
                              _rtp_malloc
 *----------------------------------------------------------------------*/
/** @memo   Allocate memory.

    @doc    Allocates, if possible, a portion of memory of
    'size' bytes.

    @precondition <b>Only use if dynamic memory is required. MUST</b>
    not be called directly. Use the <b>rtp_malloc</b> macro defined
    in rtpmem.h and turn on RTP_TRACK_LOCAL_MEMORY. If the preprocessor
    macros used are unavailable in the target environment, define
    them at the compiler to a chosen dummy value.

    @return Pointer to the location of the allocated spcae,
    0 otherwise. For debugging purposes; if the cause of the
    error is obtainable at the native Memory System layer,
    turn on RTP_DEBUG in rtpmem.c to display the native
    error value.
 */
void * _rtp_debug_malloc (
  unsigned long size,                   /** Number of bytes to allocate. */
  RTP_MALLOC_FN allocFn,                /** Memory allocation function pointer. */
  const char *file,                     /** The preprocessor macro __FILE__. */
  long line_num,                         /** The preprocessor macro __LINE__. */
  const char *comment,
  unsigned long flags
  )
{
	void * ptr;

	if (!size)
	{
		return ((void *)0);
	}

  #if (RTP_MEM_RESTRAIN)
	/* ----------------------------------- */
	/*  when we have more than giMemBase   */
	/*  used, randomly deny malloc         */
	/*  requests for torture purposes      */
	/* ----------------------------------- */
	if ((size + (unsigned long) giMemUsed > giMemLimit) ||
	    ((giMemUsed > (long) giMemBase) && (rtp_rand() % 1000 < gcMallocFailRate)))
	{
		giMemClipped++;
		return ((void *)0);
	}
  #endif

	ptr = allocFn (size +
	               RTP_MEM_BUFFER_GUARD +
	               RTP_MEM_BUFFER_PREPAD +
	               sizeof(RTPMemBlockHeader));

	if (ptr)
	{
		_rtp_AddMemBlock (ptr, size, file, line_num, comment, flags);
		ptr = (void *)(((unsigned char *)ptr) + sizeof(RTPMemBlockHeader));
	}

	if (!ptr)
	{
		return ((void *)0);
	}

  #if (RTP_MEM_BUFFER_PREPAD > 0)
	rtp_memset (ptr, RTP_MEM_PREPAD_BYTE, RTP_MEM_BUFFER_PREPAD);
	ptr = (void *)(((unsigned char *)ptr) + RTP_MEM_BUFFER_PREPAD);
  #endif

  #if (RTP_MEM_BUFFER_GAURD > 0)
	rtp_memset ((void *)(((unsigned char *)ptr) + size), RTP_MEM_GUARD_BYTE, RTP_MEM_BUFFER_GUARD);
  #endif

	return (ptr);
}

/*----------------------------------------------------------------------*
                              _rtp_malloc
 *----------------------------------------------------------------------*/
/** @memo   Allocate memory.

    @doc    Allocates, if possible, a portion of memory of
    'size' bytes.

    @precondition <b>Only use if dynamic memory is required. MUST</b>
    not be called directly. Use the <b>rtp_malloc</b> macro defined
    in rtpmem.h and turn off RTP_TRACK_LOCAL_MEMORY.

    @return Pointer to the location of the allocated spcae,
    0 otherwise. For debugging purposes; if the cause of the
    error is obtainable at the native Memory System layer,
    turn on RTP_DEBUG in rtpdebug.h to display the native
    error value.
 */
// SYMBOL Provide by memalloc file in main project
// void * _rtp_malloc (
//   unsigned long size                     /** Number of bytes to allocate. */
//   )
// {
//     rtp_not_yet_implemented ();
//     return ((void *)0);
// }

/*----------------------------------------------------------------------*
                             _rtp_debug_calloc
*----------------------------------------------------------------------*/

void * _rtp_debug_calloc  (unsigned long num, unsigned long size, RTP_CALLOC_FN callocMethod, const char* file, long line_num)
{
void * p;
	p = (void *) _rtp_debug_malloc(size*num, _rtp_malloc, file, line_num, "rt__debug_calloc", 0);
	if (p)
		rtp_memset(p, 0, size*num);
    return ((void *)p);
}


/*----------------------------------------------------------------------*
                            _rtp_debug_realloc
 *----------------------------------------------------------------------*/
/** @memo   Reallocate memory.

    @doc    Allocates, if possible, a portion of memory of 'size'
    bytes, then moves 'size' bytes, if possible, of the old region
    ptr was pointing to, to the newly allocated space.

    @precondition <b>Only use if dynamic memory is required. MUST</b>
    not be called directly. Use the <b>rtp_realloc</b> macro defined
    in rtpmem.h and turn on RTP_TRACK_LOCAL_MEMORY. If the preprocessor
    macros used are unavailable in the target environment, define
    them at the compiler to a chosen dummy value.

    @return Pointer to the location of the newly allocated spcae,
    0 otherwise. For debugging purposes; if the cause of the
    error is obtainable at the native Memory System layer,
    turn on RTP_DEBUG in rtpmem.c to display the native
    error value.
 */
void * _rtp_debug_realloc (
  void * ptr,                           /** Pointer to currently allocated space. */
  unsigned long size,                   /** New number of bytes to allocate. */
  RTP_REALLOC_FN reallocFn,             /** Memory reallocation function pointer. */
  const char *file,                     /** The preprocessor macro __FILE__. */
  long line_num                         /** The preprocessor macro __LINE__. */
  )
{
void * result;
unsigned long oldSize=0;

    if (size <= 0)
    {
        return ((void *)0);
    }

    if (ptr)
    {
        ptr = ((char *)ptr) - sizeof(RTPMemBlockHeader);
        oldSize = ((RTPMemBlockHeader *)ptr)->size;
        _rtp_FreeMemBlock(ptr, file, line_num);
    }

  #if (RTP_MEM_RESTRAIN)
    /* ----------------------------------- */
	/*  when we have more than giMemBase   */
	/*  used, randomly deny malloc         */
	/*  requests for torture purposes      */
	/* ----------------------------------- */
	if (size > oldSize)
	{
    	if (((size - oldSize + (unsigned long) giMemUsed) > giMemLimit) ||
		    ((giMemUsed > (long) giMemBase) && (rtp_rand() % 100 < gcMallocFailRate)))
    	{
    		giMemClipped++;
    		_rtp_AddMemBlock(ptr, oldSize, file, line_num, "Realloc", 0);
    		return ((void *)0);
    	}
    }
  #endif

    result = reallocFn(ptr, size +
                       RTP_MEM_BUFFER_GUARD +
                       RTP_MEM_BUFFER_PREPAD +
                       sizeof(RTPMemBlockHeader));

    if (result)
    {
        _rtp_AddMemBlock(result, size, file, line_num,"Realloc", 0);
        result = (void *)((char *)result + sizeof(RTPMemBlockHeader));

	  #if (RTP_MEM_BUFFER_PREPAD > 0)
        rtp_memset (ptr, RTP_MEM_PREPAD_BYTE, RTP_MEM_BUFFER_PREPAD);
        ptr = (void *)(((unsigned char *)ptr) + RTP_MEM_BUFFER_PREPAD);
	  #endif

	  #if (RTP_MEM_BUFFER_GUARD > 0)
        rtp_memset ((void *)(((unsigned char *)ptr) + size), RTP_MEM_GUARD_BYTE, RTP_MEM_BUFFER_GUARD);
	  #endif
    }
    else
    {
        if (ptr)
        {
            _rtp_AddMemBlock(ptr, oldSize, file, line_num,"Realloc", 0);
        }
    }

    return (result);
}


/*----------------------------------------------------------------------*
                            _rtp_debug_free
 *----------------------------------------------------------------------*/
/** @memo   Free memory.

    @doc    Frees the portion of memory that ptr was allocated
    to by _rtp_debug_malloc or _rtp_debug_realloc.

    @precondition <b>Only use if dynamic memory is required. MUST</b>
    not be called directly. Use the <b>rtp_free</b> macro defined in
    rtpmem.h and turn on RTP_TRACK_LOCAL_MEMORY. If the preprocessor
    macros used are unavailable in the target environment, define
    them at the compiler to a chosen dummy value.

    @return void
 */

void _rtp_debug_free (
  void * ptr,                           /** Location that needs to be freed. */
  RTP_FREE_FN _free,                    /** Memory deallocation function pointer. */
  const char *file,                     /** The preprocessor macro __FILE__. */
  long line_num                         /** The preprocessor macro __LINE__. */
  )
{
    if (ptr)
    {
	  #if (RTP_MEM_BUFFER_PREPAD > 0)
        ptr = (void *)((unsigned char *)ptr - RTP_MEM_BUFFER_PREPAD);
	  #endif
		ptr = (void *)((unsigned char *)ptr - sizeof(RTPMemBlockHeader));
        _rtp_FreeMemBlock (ptr, file, line_num);
        _free (ptr);
    }
}


/*----------------------------------------------------------------------*
                      _rtp_debug_SetHighWaterCallback
 *----------------------------------------------------------------------*/
/** @memo

    @doc

    @precondition

    @return void
 */

void _rtp_debug_set_high_water_callback (void (*fn) (const char *file, long line_num, void *ctx), void* ctx, unsigned long threshold)
{
	gMemHighWaterCallbackFn = fn;
	gpMemHighWaterCallbackCtx = ctx;
	gpMemHighWaterThreshhold = threshold;
}

/*----------------------------------------------------------------------*
                        _rtp_debug_CheckMemBlockList
 *----------------------------------------------------------------------*/
/** @memo   Checks the memory block list.

    @doc    Checks the memory block list and print the memory debug
    session information to the indicated logFile.

    @precondition <b>Only use if dynamic memory is required. MUST</b>
    not be called directly. Use the <b>rtp_debug_CheckMemBlockList</b>
    macro defined in rtpmem.h and turn on RTP_TRACK_LOCAL_MEMORY.
    The rtpstdio file system support and/or the rtpterm terminal
    layer must be present to make use of this debugging feature.

    @return void
 */
void _rtp_debug_check_mem (const char *logFile, const char* mode)
{
#ifdef RTP_MEM_DEBUG_FILE
#ifdef RTP_MEM_DEBUG_PRINTF
	RTPMemBlockHeader *pBlock = gpMemBlockList;

	if (!pBlock)
	{
		void * fp;
		char buffer[RTP_MEM_REPORT_BUF_SIZE];
		if (rtp_stdio_fopen (&fp, logFile, "a+") == 0)
		{
    		rtp_memset (buffer, 0, sizeof(buffer));
    	    rtp_sprintf (buffer, "\n***** Beginning new memory debug session *****\n\n");
    	    rtp_stdio_fwrite (buffer, sizeof(char *), rtp_strlen(buffer), &fp);
    		rtp_stdio_fclose (fp);
    	}
	}

	while (pBlock)
	{
		if (!__rtp_CheckMemBlock(pBlock))
		{
			void * fp;
			char buffer[RTP_MEM_REPORT_BUF_SIZE];
			if (rtp_stdio_fopen (&fp, logFile, "a+") == 0)
			{
    			rtp_memset (buffer, 0, sizeof(buffer));
    	        rtp_sprintf (buffer, "Memory Corruption Detected at: %08Xl\n", (long)(pBlock + sizeof(RTPMemBlockHeader)));
    	        rtp_stdio_fwrite (buffer, sizeof(char *), rtp_strlen(buffer), &fp);
    			rtp_stdio_fclose (fp);
    		}
			break;
		}

		pBlock = pBlock->prev;
	}
#endif
#endif
}


/*----------------------------------------------------------------------*
                         _rtp_debug_PrintMemLeaks
 *----------------------------------------------------------------------*/
/** @memo   Identify any memory corruption.

    @doc    Identify any memory corruptions. If RTP_MEM_DEBUG_FILE
    this prints the information to a file indicated by the logFile
    name passed in. If the logFile is NULL and RTP_MEM_DEBUG_PRINTF
    is defined, the information will be sent to rtp_printf.

    @precondition <b>Only use if dynamic memory is required. MUST</b>
    not be called directly. Use the <b>rtp_debug_PrintMemLeaks</b>
    macro defined in rtpmem.h and turn on RTP_TRACK_LOCAL_MEMORY.
    The rtpstdio file system support and/or the rtpterm terminal
    layer must be present to make use of this debugging feature.

    @return void
 */

#define MEM_DEBUGSHOW_NOTHING 		0
#define MEM_DEBUGSHOW_TOTALS		1
#define MEM_DEBUGSHOW_DETAIL		2
#define MEM_DEBUGSHOW_CHANGES		3

int mem_profile_detail_level = MEM_DEBUGSHOW_DETAIL;



/*----------------------------------------------------------------------*
                              _rtp_debug_register_new
 *----------------------------------------------------------------------*/
/** @memo   Register a new so we can track usage.

    @doc    Registers a new. Works with _rtp_debug_register_delete to detect leaks
	in new - delete.

    @precondition <b>See webmem.h for specific solution.

 */

static void rtp_newentry_add(const char *file, long line, void *p, unsigned long size,const char *comment);
static void rtp_newentry_remove(void *p);

void _rtp_debug_register_new(const char *file, long line, void *p, long size,const char *comment)
{
	rtp_newentry_add(file, line, p, size, comment);
}
void _rtp_debug_register_delete(void *p)
{
	rtp_newentry_remove(p);
}

// Allocate a (4 byte) void pointer, but set its file, line and size to the paramaters from the new operator. 
// Reporting routines will see this as a memory allocation of "size" bytes. rtp_newentry_remove() unravels it.
static void rtp_newentry_add(const char *file, long line, void *p, unsigned long size,const char *comment)
{
	void *ptr;
	return;
	ptr = _rtp_malloc (sizeof(void *) +
	               RTP_MEM_BUFFER_GUARD +
	               RTP_MEM_BUFFER_PREPAD +
	               sizeof(RTPMemBlockHeader));
	if (ptr)
	{
		if (comment)
			_rtp_AddMemBlock (ptr, size, file, line,comment, dbMFlagNewEntry);
		else
			_rtp_AddMemBlock (ptr, size, file, line,"New", dbMFlagNewEntry);
		ptr = (void *)(((unsigned char *)ptr) + sizeof(RTPMemBlockHeader));

	}
	*((void **)ptr) = p;
}

static void rtp_newentry_remove(void *p)
{
RTPMemBlockHeader *pBlock = gpMemBlockList;
	return;
    while (pBlock)
   	{
		void *ptr = (void *)(((unsigned char *)pBlock) + sizeof(RTPMemBlockHeader));

		if (*((void**)ptr)==p)
		{
			_rtp_debug_free ( ptr,                           /** Location that needs to be freed.*/
							_rtp_free,							/** Memory deallocation function pointer. */
							__FILE__,							/** Set file and line so the statistics are kept. */
							__LINE__);							/** The preprocessor macro __LINE__. */
 			break;
		}
		pBlock = pBlock->prev;
	}
}


static char *fnamefrompath(char *fpath)
{
char *plast = fpath;

	while (*fpath)
	{
		if(*fpath == '\\' || *fpath == '/')
		{
			plast = fpath + 1;
		}
		fpath++;
	}
	return(plast);
}


/* ================================================ */

/************************************************************************
* Internal utility functions
************************************************************************/

static void _rtp_AddMemBlock(void *ptr, unsigned long size, const char *file, long line,const char *comment, unsigned long flags)
{
RTPMemBlockHeader *pNewBlock = (RTPMemBlockHeader *) ptr;
void (*fn)(const char *, long, void *);
void* ctx;

  fn = 0;
  ctx = 0;
  rtp_memset((void *) pNewBlock,0, sizeof(*pNewBlock));
  #ifdef RTP_MEM_DEBUG_MULTITHREAD
	if (_rtp_InitMemDebug())
	{
		rtp_sig_mutex_claim(rtpMemDebugLock);
  #endif

	  #if (RTP_MEM_BLOCK_ID_SIZE > 0)
		memset(pNewBlock->id, RTP_MEM_BLOCK_ID_BYTE, RTP_MEM_BLOCK_ID_SIZE);
	  #endif

	  #if (RTP_MEM_BLOCK_PREPAD_SIZE > 0)
		memset(pNewBlock->prepad, RTP_MEM_BLOCK_PREPAD_BYTE, RTP_MEM_BLOCK_PREPAD_SIZE);
	  #endif

		if (file && (flags&dbMFlagIgnore)==0)
		{
			giMemAlloced += size;
			if (flags&dbMFlagNewEntry)
				giMemNewed += size;
			else
				giMemUsed += size;
			if (giMemUsed+giMemNewed > giMaxMemUsed)
			{
				guMaxUsageSeq = guMallocSequence;
				giMaxMemUsed = giMemUsed+giMemNewed;
				if (giMaxMemUsed>gpMemHighWaterThreshhold)
				{
					fn = gMemHighWaterCallbackFn;
					ctx = gpMemHighWaterCallbackCtx;
				}
			}
		}

		pNewBlock->size = size;
		pNewBlock->seq  = guMallocSequence++;
		pNewBlock->file = file;
		pNewBlock->line = line;
		pNewBlock->comment = comment;
		pNewBlock->flags  = flags;
	  #ifdef RTP_MEM_DEBUG_LOG_SIZE
		pNewBlock->logIndex = rtpMemDebugLogIndex++;

		if (rtpMemDebugLogIndex >= RTP_MEM_DEBUG_LOG_SIZE)
		{
			rtpMemDebugLogWrap = 1;
			rtpMemDebugLogIndex = 0;
		}

		rtpMemDebugLog[pNewBlock->logIndex].location = ptr;
		rtpMemDebugLog[pNewBlock->logIndex].size = size;
		rtpMemDebugLog[pNewBlock->logIndex].allocedFromFile = file;
		rtpMemDebugLog[pNewBlock->logIndex].allocedFromLine = line;
		rtpMemDebugLog[pNewBlock->logIndex].allocedAtTime = rtpMemDebugLogTime++;
		rtpMemDebugLog[pNewBlock->logIndex].freedFromFile = 0;
		rtpMemDebugLog[pNewBlock->logIndex].freedFromLine = 0;
		rtpMemDebugLog[pNewBlock->logIndex].freedAtTime = 0;
		rtpMemDebugLog[pNewBlock->logIndex].freed = 0;

		if (rtpMemDebugLogPrint)
		{
			_rtp_debug_print_log("memlog.csv", "wb+");
			rtpMemDebugLogPrint = 0;
			rtpMemDebugLogPrintFilter = 0;
		}

	  #endif /* RTP_MEM_DEBUG_LOG_SIZE */

		/* ----------------------------------- */
		/*        link pNewBlock onto          */
		/*        the end of the list          */
		/* ----------------------------------- */
		pNewBlock->prev = gpMemBlockList;
		pNewBlock->next = 0;

		if (gpMemBlockList)
		{
			gpMemBlockList->next = pNewBlock;
		}
		gpMemBlockList = pNewBlock;

  #ifdef RTP_MEM_DEBUG_MULTITHREAD
		rtp_sig_mutex_release(rtpMemDebugLock);
	}
  #endif

	if (fn)
	{
		fn(file, line, ctx);
	}
}

static void _rtp_FreeMemBlock(void *ptr, const char* file, long line)
{
RTPMemBlockHeader *pFreedBlock = (RTPMemBlockHeader *) ptr;

  #ifdef RTP_MEM_DEBUG_MULTITHREAD
	if (_rtp_InitMemDebug())
	{
		rtp_sig_mutex_claim(rtpMemDebugLock);
  #endif

	  #ifdef RTP_MEM_DEBUG_LOG_SIZE

		if (ptr == rtpMemDebugLog[pFreedBlock->logIndex].location &&
		    pFreedBlock->file == rtpMemDebugLog[pFreedBlock->logIndex].allocedFromFile &&
		    pFreedBlock->line == rtpMemDebugLog[pFreedBlock->logIndex].allocedFromLine)
		{
			rtpMemDebugLog[pFreedBlock->logIndex].freedFromFile = file;
			rtpMemDebugLog[pFreedBlock->logIndex].freedFromLine = line;
			rtpMemDebugLog[pFreedBlock->logIndex].freedAtTime = rtpMemDebugLogTime++;
			rtpMemDebugLog[pFreedBlock->logIndex].freed = 1;
		}

		if (rtpMemDebugLogPrint)
		{
			_rtp_debug_print_log("memlog.csv", "wb+");
			rtpMemDebugLogPrint = 0;
			rtpMemDebugLogPrintFilter = 0;
		}

	  #endif /* RTP_MEM_DEBUG_LOG_SIZE */

		if (pFreedBlock->file && (pFreedBlock->flags & dbMFlagIgnore)==0)
		{
			giMemFreed += pFreedBlock->size;
			if (pFreedBlock->flags&dbMFlagNewEntry)
				giMemNewed -= pFreedBlock->size;
			else
				giMemUsed -= pFreedBlock->size;
		}

		if (pFreedBlock->prev)
		{
			pFreedBlock->prev->next = pFreedBlock->next;
		}

		if (pFreedBlock->next)
		{
			pFreedBlock->next->prev = pFreedBlock->prev;
		}
		else
		{
			gpMemBlockList = pFreedBlock->prev;
		}

  #ifdef RTP_MEM_DEBUG_MULTITHREAD
		rtp_sig_mutex_release(rtpMemDebugLock);
	}
  #endif
}

#ifdef RTP_MEM_DEBUG_LOG_SIZE

#include <stdio.h>
#include <stdlib.h>

void _rtp_debug_print_log_entry (FILE* fp, RTPMemAllocRecord* record)
{
	// print to file in this format:
	//  "location","size","allocFile","allocLine","allocTime","freed","freeFile","freeLine","freeTime"\n
	if (record->freed)
		return;
	if (rtpMemDebugLogPrintFilter == 0 ||
	    (record->location <= rtpMemDebugLogPrintFilter && (char*)rtpMemDebugLogPrintFilter <= ((char*)record->location + record->size)))
	{
		fprintf ( fp, "\"0x%08x\",\"%d\",\"%s\",\"%d\",\"%d\",\"%s\",\"%s\",\"%d\",\"%d\"\n",
				(long) record->location,
				record->size,
				record->allocedFromFile? record->allocedFromFile : "",
				record->allocedFromLine,
				record->allocedAtTime,
				record->freed? "free" : "in use",
				record->freedFromFile? record->freedFromFile : "",
				record->freedFromLine,
				record->freedAtTime);
	}
}

void _rtp_debug_print_log  (const char *logFile, const char* mode)
{
	FILE* fp = fopen(logFile, mode);

	if (fp)
	{
		long n;

		fprintf (fp, "\"location\",\"size\",\"allocFile\",\"allocLine\",\"allocTime\",\"status\",\"freeFile\",\"freeLine\",\"freeTime\"\n");

		for (n = rtpMemDebugLogIndex; n >= 0; n--)
		{
			_rtp_debug_print_log_entry(fp, rtpMemDebugLog + n);
		}

		if (rtpMemDebugLogWrap)
		{
			for (n = RTP_MEM_DEBUG_LOG_SIZE-1; n > rtpMemDebugLogIndex; n--)
			{
				_rtp_debug_print_log_entry(fp, rtpMemDebugLog + n);
			}
		}

		fclose(fp);
	}
}

#endif /* RTP_MEM_DEBUG_LOG_SIZE */

static unsigned int __rtp_CheckMemBlock(RTPMemBlockHeader *pBlock)
{
unsigned char *padding = ((unsigned char *)pBlock) + sizeof(RTPMemBlockHeader) + pBlock->size;

#if (RTP_MEM_BLOCK_ID_SIZE > 0)
	for (n=0; n<RTP_MEM_BLOCK_ID_SIZE; n++)
	{
		if (pBlock->id[n] != RTP_MEM_BLOCK_ID_BYTE)
		{
			return (0);
		}
	}
#endif /* (RTP_MEM_BLOCK_ID_SIZE > 0) */

#if (RTP_MEM_BLOCK_PREPAD_SIZE > 0)
	for (n=0; n<RTP_MEM_BLOCK_PREPAD_SIZE; n++)
	{
		if (pBlock->prepad[n] != RTP_MEM_BLOCK_PREPAD_BYTE)
		{
			return (0);
		}
	}
#endif /* (RTP_MEM_BLOCK_PREPAD_SIZE > 0) */

#if (RTP_MEM_BUFFER_GUARD > 0)
	for (n=0; n<RTP_MEM_BUFFER_GUARD; n++)
	{
		if (padding[n] != RTP_MEM_GUARD_BYTE)
		{
			return (0);
		}
	}
#endif /* (RTP_MEM_BUFFER_GUARD > 0) */

	return (1);
}


unsigned _rtp_InitMemDebug  (void)
{
#ifdef RTP_MEM_DEBUG_MULTITHREAD
	if (!rtpMemDebugInitialized)
	{
		if (rtp_sig_mutex_alloc(&rtpMemDebugLock, 0) >= 0)
		{
			rtpMemDebugInitialized = 1;
		}
	}

	return (rtpMemDebugInitialized);
#else
	return (1);
#endif
}


// Called by wcMemDbGetStats to populate the memory stats structure
void rtp_debug_get_mem_stats(MemDbStats *pStats)
{
	pStats->MemAlloced =giMemAlloced;   // Total bytes allocated since startup
	pStats->MemFreed =giMemFreed;       // Total bytes freed since startup
	pStats->MemUsed =giMemUsed;         // Total bytes currently in use
	pStats->MaxMemUsed =giMaxMemUsed;   // High water mark of maximum bytes ever used.
	pStats->MemClipped =giMemClipped;   // Number of times a memory allocation fails.  HEREHERE - Add failur diagnostic
	pStats->MemNewed =giMemNewed;       // Total number of bytes currently allocated by the new operator and not yet released by the free operator. Note: what about detail about newed data.
	pStats->MallocSequence =guMallocSequence; // Current sequence number, keep this around if you with to later on report on a range of memory allocation calls starting from this point.
	pStats->MaxUsageSeq =guMaxUsageSeq; // Sequence number of the allocation when the maximum allocation was reached. (see RTPMemBlockHeader);
	pStats->MemBlockList =gpMemBlockList; // A list of the currently outstanding memory starting with the most recent allocation.
}
#endif /* RTP_TRACK_LOCAL_MEMORY */


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
