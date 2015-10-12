 /*
 | RTPPBUF.H - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:30 $
 |  $Name:  $
 |  $Revision: 1.4 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |	RTP_PBUF_ELEMENT
 |            |----------------------------------------------|
 |       _____V______________________________________________|_____________________________________________
 |      | elementOwner | listOwner | \\\ padding \\\ | start pointer |       data       | tracker | tracer |
 |      |______________|___________|_________________|_______________|__________________|_________|________|
 |
 |	RTP_PBUF_RESOURCE
 |		Resource pool that may be added to.  Each pool that is added as a resource 
 |		is then broken down into Elements of elementSize according to the associated
 |		Partitioned Buffer Context.  Therefor, every object that needs to make use of 
 |		this module needs to setup its own Partitioned Buffer Context (Element List).  
 |		The resource pools that are added may be statically allocated and/or dynamically
 |		allocated. This pool allocation is done at the application level to ensure 
 |		the greatest level of flexability to the application programmer.
 |
 |	RTP_PBUF_CTX
 |		The Partitioned Buffer Context is used as the manager to the elements and consequently
 |		the resource pool.  The context may be initialized using one of three initializers
 |		which load: add, destroy, pop, and push functions that are implemented, 
 |		generically, thread safe, or interrupt safe depending on the API call referenced.
 |		This structure is accessible outside this module so that the function pointers
 |		can be overridden.
*/

#ifndef __RTPPBUF_H__
#define __RTPPBUF_H__

/*****************************************************************************
 * Header files
 *****************************************************************************/
#include "rtp.h"
#include "rtpsignl.h"

/*****************************************************************************
 * Defines
 *****************************************************************************/
/************************************************************************
 * If RTP_TRACK_PACKET_BUFFER is turned on, the debug implementaion of  *
 * the buffer memory system will be implemented.  This provides debug   *
 * information otherwise not available by most platforms.               *
 ************************************************************************/
 
/* -------------------------------------------- */
/* Define this flag to include buffer tracking. */
/* -------------------------------------------- */
#define RTP_TRACK_PACKET_BUFFER

/* -------------------------------------------- */
/* Define this flag to include buffer tracing.  */
/* -------------------------------------------- */
#define RTP_TRACE_PACKET_BUFFER

/*****************************************************************************
 * Macros
 *****************************************************************************/

/*****************************************************************************
 * Types
 *****************************************************************************/
typedef struct s_RTP_PBUF_RESOURCE       RTP_PBUF_RESOURCE;
typedef struct s_RTP_PBUF_ELEMENT        RTP_PBUF_ELEMENT;
typedef struct s_RTP_PBUF_CTX            RTP_PBUF_CTX;

typedef void   (*RTP_PBUF_CTX_ADD)       (RTP_PBUF_CTX* pbufCtx, RTP_PBUF_RESOURCE* resource);
typedef void   (*RTP_PBUF_CTX_DESTROY)   (RTP_PBUF_CTX* pbufCtx);
typedef void*  (*RTP_PBUF_CTX_POP)       (RTP_PBUF_CTX* pbufCtx);
typedef void   (*RTP_PBUF_CTX_PUSH)      (void* element);

typedef void   (*RTP_PBUF_RESOURCE_FREE) (void* mem);

/*****************************************************************************
 * Structures
 *****************************************************************************/
struct s_RTP_PBUF_RESOURCE
{
	void*         mem;
	unsigned long size;
	short         numElements;
	
	RTP_PBUF_RESOURCE_FREE      free;
	struct s_RTP_PBUF_RESOURCE* next;
};

struct s_RTP_PBUF_ELEMENT
{
	RTP_PBUF_CTX* elementOwner;
	RTP_PBUF_CTX* listOwner;
	
	struct s_RTP_PBUF_ELEMENT* next;
	void*                      data;
	
#ifdef RTP_TRACK_PACKET_BUFFER
	void* tracker;			/* [tbd] */
#endif
#ifdef RTP_TRACE_PACKET_BUFFER
	void* tracer;			/* [tbd] */
#endif	
};

struct s_RTP_PBUF_CTX
{
	const char* name;
	short       alignment;
	short       elementSize;
	short       elementsAlloced;
	short       elementsAvailable;
	
	RTP_MUTEX          lock;
	RTP_PBUF_ELEMENT*  firstElement;
	RTP_PBUF_RESOURCE* firstResource;
	
	RTP_PBUF_CTX_ADD     add;
	RTP_PBUF_CTX_DESTROY destroy;
	RTP_PBUF_CTX_POP     pop;
	RTP_PBUF_CTX_PUSH    push;
};

/*****************************************************************************
 * Data
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Partitioned Buffer API
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

void rtp_pbuf_ctx_init    (RTP_PBUF_CTX* pbufCtx, short alignment, short elementSize);
void rtp_pbuf_ctx_init_ts (RTP_PBUF_CTX* pbufCtx, short alignment, short elementSize, RTP_MUTEX lock);
void rtp_pbuf_ctx_init_is (RTP_PBUF_CTX* pbufCtx, short alignment, short elementSize);

#ifdef __cplusplus
}
#endif
#endif /* __RTPPBUF_H__ */



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
