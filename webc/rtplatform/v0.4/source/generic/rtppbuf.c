 /*
 | RTPPBUF.C - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:30 $
 |  $Name:  $
 |  $Revision: 1.5 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/



/*****************************************************************************
 * Header files
 *****************************************************************************/
#include "rtp.h"
#include "rtppbuf.h"
#include "rtputil.h"
#include "rtpsignl.h"
#include "rtpirq.h"

/*****************************************************************************
 * Defines
 *****************************************************************************/
#define RTP_PBUF_ELEMENT_START_OFFSET		((unsigned long) sizeof(void*))

/*****************************************************************************
 * Macros
 *****************************************************************************/

/*****************************************************************************
 * Types
 *****************************************************************************/

/*****************************************************************************
 * Data
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/
static void   _rtp_pbuf_ctx_add     (RTP_PBUF_CTX* pbufCtx, RTP_PBUF_RESOURCE* resource);
static void   _rtp_pbuf_ctx_destroy (RTP_PBUF_CTX* pbufCtx);
static void*  _rtp_pbuf_ctx_pop     (RTP_PBUF_CTX* pbufCtx);
static void   _rtp_pbuf_ctx_push    (void* data);

static void   _rtp_pbuf_ctx_add_ts     (RTP_PBUF_CTX* pbufCtx, RTP_PBUF_RESOURCE* resource);
static void   _rtp_pbuf_ctx_destroy_ts (RTP_PBUF_CTX* pbufCtx);
static void*  _rtp_pbuf_ctx_pop_ts     (RTP_PBUF_CTX* pbufCtx);
static void   _rtp_pbuf_ctx_push_ts    (void* data);

static void*  _rtp_pbuf_ctx_pop_is     (RTP_PBUF_CTX* pbufCtx);
static void   _rtp_pbuf_ctx_push_is    (void* data);

/*****************************************************************************
 * Partitioned Buffer API
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
void rtp_pbuf_ctx_init (RTP_PBUF_CTX* pbufCtx, short alignment, short elementSize)
{
	tc_memset(pbufCtx, 0, sizeof(RTP_PBUF_CTX));
	pbufCtx->alignment   = alignment;
	pbufCtx->elementSize = elementSize;

	pbufCtx->add	= _rtp_pbuf_ctx_add;
	pbufCtx->destroy= _rtp_pbuf_ctx_destroy;
	pbufCtx->pop	= _rtp_pbuf_ctx_pop;
	pbufCtx->push	= _rtp_pbuf_ctx_push;
}

/*---------------------------------------------------------------------------*/
void rtp_pbuf_ctx_init_ts (RTP_PBUF_CTX* pbufCtx, short alignment, short elementSize, RTP_MUTEX lock)
{
	tc_memset(pbufCtx, 0, sizeof(RTP_PBUF_CTX));
	pbufCtx->alignment   = alignment;
	pbufCtx->elementSize = elementSize;

	pbufCtx->lock   = lock;
	
	pbufCtx->add	= _rtp_pbuf_ctx_add_ts;
	pbufCtx->destroy= _rtp_pbuf_ctx_destroy_ts;
	pbufCtx->pop	= _rtp_pbuf_ctx_pop_ts;
	pbufCtx->push	= _rtp_pbuf_ctx_push_ts;
}

/*---------------------------------------------------------------------------*/
void rtp_pbuf_ctx_init_is (RTP_PBUF_CTX* pbufCtx, short alignment, short elementSize)
{
	tc_memset(pbufCtx, 0, sizeof(RTP_PBUF_CTX));
	pbufCtx->alignment   = alignment;
	pbufCtx->elementSize = elementSize;

	pbufCtx->add	= _rtp_pbuf_ctx_add;
	pbufCtx->destroy= _rtp_pbuf_ctx_destroy;
	pbufCtx->pop	= _rtp_pbuf_ctx_pop_is;
	pbufCtx->push	= _rtp_pbuf_ctx_push_is;
}

/*---------------------------------------------------------------------------*/
static void _rtp_pbuf_ctx_add (RTP_PBUF_CTX* pbufCtx, RTP_PBUF_RESOURCE* resource)
{
RTP_PBUF_RESOURCE tempResource;
unsigned long totalElementSize;
unsigned long padding;

	tc_memcpy(&tempResource, resource, sizeof(RTP_PBUF_RESOURCE));
	resource->numElements = 0;
	resource->next = 0;

	for (;;)
	{
		while (((unsigned long)(tempResource.mem) & (sizeof(void*)-1)) && tempResource.size)
		{
			/* ----------------------------------- */
			/*  Alligning available resource pool  */
			/*  for worst case pointer alignement. */
			/* ----------------------------------- */
			(unsigned long)(tempResource.mem) = (unsigned long)(tempResource.mem) + 1;
			tempResource.size--;
		}
		
		/* ----------------------------------- */
		/*  Padding is equal to the sum of the */
		/*  element structure size and a void* */
		/*  to the start of the element plus   */
		/*  any additional padding need to     */
		/*  align the data area with the list  */
		/*  alignment requirement.             */
		/* ----------------------------------- */
		padding = sizeof(RTP_PBUF_ELEMENT) + RTP_PBUF_ELEMENT_START_OFFSET;
		while (((unsigned long)(tempResource.mem) + padding) & (pbufCtx->alignment - 1))
		{
			padding++;
		}
		
		totalElementSize = padding + pbufCtx->elementSize;
		if (tempResource.size >= totalElementSize)
		{
			void* temp;
			
			((RTP_PBUF_ELEMENT*)(tempResource.mem))->elementOwner = pbufCtx;
			((RTP_PBUF_ELEMENT*)(tempResource.mem))->listOwner    = pbufCtx;
			((RTP_PBUF_ELEMENT*)(tempResource.mem))->next         = pbufCtx->firstElement;
			pbufCtx->firstElement = ((RTP_PBUF_ELEMENT*)(tempResource.mem));
			
			((RTP_PBUF_ELEMENT*)(tempResource.mem))->data = (void*) (((unsigned long)tempResource.mem) + padding);
			
#ifdef RTP_TRACK_PACKET_BUFFER
			((RTP_PBUF_ELEMENT*)(tempResource.mem))->tracker = (void*)0;
#endif
#ifdef RTP_TRACE_PACKET_BUFFER
			((RTP_PBUF_ELEMENT*)(tempResource.mem))->tracer = (void*)0;
#endif	
			
			temp = (void*) (((unsigned long)((RTP_PBUF_ELEMENT*)(tempResource.mem))->data) - RTP_PBUF_ELEMENT_START_OFFSET);
			tempResource.mem = (void*) (((unsigned long)tempResource.mem) + totalElementSize);
			tempResource.size -= totalElementSize;
			resource->numElements++;
			pbufCtx->elementsAlloced++;
			pbufCtx->elementsAvailable++;
		}
		else
		{
			resource->next = pbufCtx->firstResource;
			pbufCtx->firstResource = resource;
			return;
		}
	}
}

/*---------------------------------------------------------------------------*/
static void  _rtp_pbuf_ctx_destroy (RTP_PBUF_CTX* pbufCtx)
{
RTP_PBUF_RESOURCE* tempResource;

	while (pbufCtx->firstResource)
	{
		tempResource = pbufCtx->firstResource->next;
		if (pbufCtx->firstResource->free)
		{
			pbufCtx->firstResource->free(pbufCtx->firstResource);
		}
		pbufCtx->firstResource = tempResource;
	}
}

/*---------------------------------------------------------------------------*/
static void* _rtp_pbuf_ctx_pop (RTP_PBUF_CTX* pbufCtx)
{
RTP_PBUF_ELEMENT* tempElement;

	if (pbufCtx->elementsAvailable)
	{
		(pbufCtx->elementsAvailable)--;
		tempElement = pbufCtx->firstElement;
		pbufCtx->firstElement = tempElement->next;
		return (tempElement->data);
	}
	return ((void*)0);
}

/*---------------------------------------------------------------------------*/
static void _rtp_pbuf_ctx_push (void* data)
{
RTP_PBUF_ELEMENT* tempElement;

	tempElement = (RTP_PBUF_ELEMENT*) (((unsigned long)data) - RTP_PBUF_ELEMENT_START_OFFSET);
	tempElement->next = tempElement->elementOwner->firstElement;
	tempElement->elementOwner->firstElement = tempElement;
	(tempElement->elementOwner->elementsAvailable)++;
}

/*---------------------------------------------------------------------------*/
static void _rtp_pbuf_ctx_add_ts (RTP_PBUF_CTX* pbufCtx, RTP_PBUF_RESOURCE* resource)
{
	if (rtp_sig_mutex_claim(pbufCtx->lock) >= 0)
	{
		_rtp_pbuf_ctx_add(pbufCtx, resource);
		rtp_sig_mutex_release(pbufCtx->lock);
	}
}

/*---------------------------------------------------------------------------*/
static void _rtp_pbuf_ctx_destroy_ts (RTP_PBUF_CTX* pbufCtx)
{
	if (rtp_sig_mutex_claim(pbufCtx->lock) >= 0)
	{
		_rtp_pbuf_ctx_destroy(pbufCtx);
		rtp_sig_mutex_release(pbufCtx->lock);
	}
}

/*---------------------------------------------------------------------------*/
static void* _rtp_pbuf_ctx_pop_ts (RTP_PBUF_CTX* pbufCtx)
{
RTP_PBUF_ELEMENT* tempElement;

	if (rtp_sig_mutex_claim(pbufCtx->lock) >= 0)
	{
		tempElement = _rtp_pbuf_ctx_pop(pbufCtx);
		rtp_sig_mutex_release(pbufCtx->lock);
		return ((void*)tempElement);
	}
	return ((void*)0);
}

/*---------------------------------------------------------------------------*/
static void _rtp_pbuf_ctx_push_ts (void* data)
{
RTP_PBUF_ELEMENT* tempElement;

	tempElement = (RTP_PBUF_ELEMENT*) (((unsigned long)data) - RTP_PBUF_ELEMENT_START_OFFSET);
	if (rtp_sig_mutex_claim(tempElement->elementOwner->lock) >= 0)
	{
		tempElement->next = tempElement->elementOwner->firstElement;
		tempElement->elementOwner->firstElement = tempElement;
		(tempElement->elementOwner->elementsAvailable)++;
		rtp_sig_mutex_release(tempElement->elementOwner->lock);
	}
}

/*---------------------------------------------------------------------------*/
static void* _rtp_pbuf_ctx_pop_is (RTP_PBUF_CTX* pbufCtx)
{
RTP_PBUF_ELEMENT* tempElement;
RTP_HANDLE irqHandle;

	irqHandle   = rtp_irq_push_disable( );
	tempElement = _rtp_pbuf_ctx_pop(pbufCtx);
	rtp_irq_pop(irqHandle);
	return ((void*)tempElement);
}

/*---------------------------------------------------------------------------*/
static void _rtp_pbuf_ctx_push_is (void* data)
{
RTP_HANDLE irqHandle;

	irqHandle = rtp_irq_push_disable( );
	_rtp_pbuf_ctx_push(data);
	rtp_irq_pop(irqHandle);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
