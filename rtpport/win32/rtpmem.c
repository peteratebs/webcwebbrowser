 /*
 | RTPMEM.C - Runtime Platform Memory System Services
 |
 |   PORTED TO THE EBSnet Inc RTIP TCP/IP PLATFORM
 |
 | EBSnet - EBS_RT-Platform 
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
#include "rtpprint.h"

/************************************************************************
* Defines
************************************************************************/
#define ALIGN       4
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
static keeptrack = 0;
    if (size > 200)
    {
        keeptrack += size;
    }
    size = ((size+ALIGN-1)/ALIGN)*ALIGN;

#if (INCLUDE_BGET)
    ptr = ((RTP_PFINT8)bget(size));
#else   
    ptr = (RTP_PFUINT8)malloc(size);
#endif
    if (!ptr)
    {
        rtp_printf("_rtp_malloc: Failed to alloc");
    }
    return(ptr);

}

/*----------------------------------------------------------------------*
                             _rtp_calloc
 *----------------------------------------------------------------------*/
void * _rtp_calloc (unsigned long num, unsigned int size)
{
void * ptr;

	
#if (INCLUDE_BGET)
	ptr = (bget_calloc ((size_t) num, (size_t) size));
#else   
	ptr = (calloc ((size_t) num, (size_t) size));
#endif
	

    return(ptr);
}



/*----------------------------------------------------------------------*
                             _rtp_realloc
 *----------------------------------------------------------------------*/
void * _rtp_realloc (void * ptr, unsigned int size)
{
#if (INCLUDE_BGET)
    ptr = ((RTP_PFINT8)bgetr(ptr, size));
#else
    ptr = (RTP_PFUINT8)realloc(ptr,size);
#endif
    return(ptr);
}


/*----------------------------------------------------------------------*
                              _rtp_free
 *----------------------------------------------------------------------*/
void _rtp_free (void * ptr)
{
#if (INCLUDE_BGET)
    brel(ptr);
#else
    free(ptr);
#endif
}

/*----------------------------------------------------------------------*
                              _rtp_debug_free
 *----------------------------------------------------------------------*/

void _rtp_debug_free (void )
{
#if (INCLUDE_BGET)
    int currentlyAllocated;
    int totalFree;
    int maxFree;
    int numberGot;
    int numberReleased;

#ifdef BufStats
    bstats(    &currentlyAllocated,
               &totalFree,
               &maxFree,
               &numberGot,
               &numberReleased );    
#endif

    printf(" total free %d\n", totalFree);
#endif
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */