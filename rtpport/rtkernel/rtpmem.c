 /*
 | RTPMEM.C - Runtime Platform Memory System Services
 |
 |   PORTED TO THE EBSnet Inc RTIP TCP/IP PLATFORM
 |
 | EBSnet - EBS_RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:31 $
 |  $Name:  $
 |  $Revision: 1.2 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/


/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"
#include "rtptypes.h"

//#include "rtpmem.h"
//#include "rtip.h"
/************************************************************************
* Defines
************************************************************************/
#define ALIGN		64
/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                              _rtp_malloc
 *----------------------------------------------------------------------*/
void * rtp_malloc (unsigned long size)
{
RTP_PFINT8 ptr;

	size = ((size+ALIGN-1)/ALIGN)*ALIGN;
   
   //ptr = (PFBYTE)malloc(size);
	ptr = ((RTP_PFINT8)bget(size));
	if (!ptr)
	{
		RTIP_DEBUG_ERROR("_rtp_malloc: Failed to alloc", EBS_NOVAR, 0, 0);
	}
	return(ptr);

}


/*----------------------------------------------------------------------*
                             _rtp_realloc
 *----------------------------------------------------------------------*/
void * rtp_realloc (void * ptr, unsigned long size)
{
	ptr = ((RTP_PFINT8)bgetr(ptr, size));
//  ptr = (PFBYTE)realloc(ptr,size);
  	return(ptr);
}


/*----------------------------------------------------------------------*
                              _rtp_free
 *----------------------------------------------------------------------*/
void   rtp_free    (void * ptr)
{
   brel(ptr);
   //free(ptr);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */