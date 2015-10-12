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
#define RTP_MALLOC_ALIGN 64

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
void * _rtp_malloc (unsigned int size)
{
    RTP_PFINT8 ptr;

	size = ((size+RTP_MALLOC_ALIGN-1) & ~(RTP_MALLOC_ALIGN-1L));
   
	ptr = ((RTP_PFINT8)bget(size));
	if (!ptr)
	{
        bufsize maxalloc = 0;
        bufsize curalloc = 0;
        bufsize totfree  = 0;
        bufsize maxfree  = 0;
        long nget = 0;
        long nrel = 0;
	    dc_log_printf("bget(%d)->NULL", size);

        // maxalloc = The peak amount of space allocated
        // curalloc = The amount of space currently allocated
        // totfree  = The total free space (sum of all free blocks in the pool)
        // maxfree  = The size of the largest single block in the free space pool
        // nget     = The number of successful (non-NULL return) bget calls
        // nrel     = The number of brel() calls.
    
        bstats(&maxalloc, &curalloc, &totfree, &maxfree, &nget, &nrel);
        dc_log_printf (" Used: %d", curalloc);
        dc_log_printf (" Free: %d", totfree);
        dc_log_printf (" Largest Chunk: %d", maxfree);
	}
	return(ptr);

}


/*----------------------------------------------------------------------*
                             _rtp_realloc
 *----------------------------------------------------------------------*/
void * _rtp_realloc (void * ptr, unsigned int size)
{
	ptr = ((RTP_PFINT8)bgetr(ptr, size));
//  ptr = (PFBYTE)realloc(ptr,size);
	if (!ptr)
	{
	    dc_log_printf("bgetr(%p,%d)->NULL", ptr, size);
	}
  	return(ptr);
}


/*----------------------------------------------------------------------*
                              _rtp_free
 *----------------------------------------------------------------------*/
void _rtp_free (void * ptr)
{
    if (!ptr)
    {
        dc_log_printf("brel(NULL)");
    }
    else  
        brel(ptr);
    //free(ptr);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
