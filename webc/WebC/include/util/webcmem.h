/*
|  WEBCMEM.H - Dynamic memory allocation services
|
|  EBS -
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#ifndef __WEBCMEM_H__
#define __WEBCMEM_H__

#include "webcfg.h"
#include "rtpmem.h"


#define WEBC_MALLOC(S)              rtp_malloc(S)
#define WEBC_CALLOC(N,S)            rtp_calloc(N,S)
#define WEBC_REALLOC(P,S)           rtp_realloc(P,S)
#define WEBC_FREE(P)                rtp_free(P)
#define WEBC_DEBUG_FREE(P,F,L)      WEBC_FREE(P)

#ifdef RTP_TRACK_LOCAL_MEMORY


// BUGBUG
#define WEBC_DEBUG_MALLOC(S,F,L,C,FL)    _rtp_debug_malloc(S, _rtp_malloc, F, L, C, FL)
#define WEBC_MALLOC_VERBOSE(S,C) _rtp_debug_malloc(S, _rtp_malloc, __FILE__, __LINE__, C, 0)

#else
#define WEBC_DEBUG_MALLOC(S,F,L,C,FL)    WEBC_MALLOC(S)
#define WEBC_MALLOC_VERBOSE(S,C)  WEBC_MALLOC(S)
#endif


#ifdef __TSDPROJECT__		/* Generic solution provided below */
#define WEBC_FAST_MALLOC(S)          _rtp_fast_malloc(S)
#define WEBC_FAST_FREE(P)           _rtp_fast_free(P)
#else
#define WEBC_FAST_MALLOC(S)         WEBC_MALLOC(S)
#define WEBC_FAST_FREE(P)           WEBC_FREE(P)
#endif

/* Define this symbol to track memory used by the Mozilla JavaScript
   library */

 #define WEBC_JSMALLOC(X)            WEBC_MALLOC(X)
 #define WEBC_JSCALLOC(X,Y)          WEBC_CALLOC(X,Y)
 #define WEBC_JSREALLOC(P,X)         WEBC_REALLOC(P,X)
 #define WEBC_JSFREE(X)              WEBC_FREE(X)

#ifdef __cplusplus

 #ifdef WEBC_DELETE
  #undef WEBC_DELETE
 #endif

 #define _WEBC_NEW(C)				(new C)
 #define _WEBC_DELETE(P) 			delete P

#ifdef RTP_TRACK_LOCAL_MEMORY

void _mdbSetFile(const char *F);
void _mdbSetLine(long l);

#define WEBC_MEMORY_RTPDEBUG 1
void _rtp_register_new(const char *file, long line, void *p, long size);
void _rtp_register_delete(void *p);
  /* tbd - make this threadsafe */
#define WEBC_DEBUG_NEW_SIZED(P,C,F,L,SZ) (_mdbSetFile(F), _mdbSetLine(L), P=_WEBC_NEW(C), _rtp_debug_register_new(__FILE__, __LINE__, (void *) P, (long) SZ,0), P)
#define WEBC_DEBUG_NEW(P,C,F,L,CO)   (_mdbSetFile(F), _mdbSetLine(L), P=_WEBC_NEW(C), _rtp_debug_register_new(F, L, (void *) P, (long) sizeof(* P),CO), P)
#define WEBC_DEBUG_DELETE(P)  {if(P) {_rtp_debug_register_delete((void *) P); _WEBC_DELETE(P);}}
#define WEBC_NEW(P,C)             WEBC_DEBUG_NEW(P,C,__FILE__,__LINE__,0)
#define WEBC_NEW_VERBOSE(P,C,CO)  WEBC_DEBUG_NEW(P,C,__FILE__,__LINE__,CO)
#define WEBC_NEW_SIZED(P,C,SZ)	  WEBC_DEBUG_NEW_SIZED(P,C,__FILE__,__LINE__,SZ)
#define WEBC_DELETE(P)            WEBC_DEBUG_DELETE(P)
#else
   #define WEBC_NEW_SIZED(P,C,SZ)			(P=_WEBC_NEW(C))
  #define WEBC_NEW_VERBOSE(P,C,CO)			 (P=_WEBC_NEW(C))
  #define WEBC_NEW(P,C)						(P=_WEBC_NEW(C))
  #define WEBC_DELETE(P)					{if(P) _WEBC_DELETE(P);}
  #define WEBC_DEBUG_NEW(P,C,F,L,CO)		(P=_WEBC_NEW(C))
  #define WEBC_DEBUG_DELETE(P)				WEBC_DELETE(P)
#endif

#endif /* __cplusplus */

#endif /* __WEBCMEM_H__ */
