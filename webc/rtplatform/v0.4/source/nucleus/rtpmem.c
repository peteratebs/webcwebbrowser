 /*
 | RTPMEM.C - Runtime Platform Memory System Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: tony $
 |  $Date: 2005/04/01 15:40:15 $
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



/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"
#include "rtpmem.h"
#include "stdlib.h"

/************************************************************************
* Defines
************************************************************************/

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
/** @memo   Allocate memory.

    @doc    Allocates, if possible, a portion of memory of 
    'size' bytes.

    @precondition <b>Only use if dynamic memory is required. MUST</b> 
    not be called directly. Use the <b>rtp_malloc</b> macro defined 
    in rtpmem.h and turn off RTP_TRACK_LOCAL_MEMORY.

    @return Pointer to the location of the allocated spcae, 
    0 otherwise. For debugging purposes; if the cause of the
    error is obtainable at the native Memory System layer, 
    turn on RTP_DISPLAY_ERROR in rtpmem.c to display the native 
    error value.
 */
void * _rtp_malloc (
  unsigned long size                     /** Number of bytes to allocate. */
  )
{
    return malloc(size);
}


/*----------------------------------------------------------------------*
                             _rtp_realloc
 *----------------------------------------------------------------------*/
/** @memo   Reallocate memory.

    @doc    Allocates, if possible, a portion of memory of 'size' 
    bytes, then moves 'size' bytes, if possible, of the old region 
    ptr was pointing to, to the newly allocated space.

    @precondition <b>Only use if dynamic memory is required. MUST</b> 
    not be called directly. Use the <b>rtp_realloc</b> macro defined 
    in rtpmem.h and turn off RTP_TRACK_LOCAL_MEMORY.

    @return Pointer to the location of the newly allocated spcae, 
    0 otherwise. For debugging purposes; if the cause of the
    error is obtainable at the native Memory System layer, 
    turn on RTP_DISPLAY_ERROR in rtpmem.c to display the native 
    error value.
 */
void * _rtp_realloc (
  void * ptr,                           /** Pointer to currently allocated space. */
  unsigned long size                     /** New number of bytes to allocate. */
  )
{
	return realloc(ptr, size);
}


/*----------------------------------------------------------------------*
                              _rtp_free
 *----------------------------------------------------------------------*/
/** @memo   Free memory.

    @doc    Frees the portion of memory that ptr was allocated 
    to by _rtp_malloc or _rtp_realloc.

    @precondition <b>Only use if dynamic memory is required. MUST</b> 
    not be called directly. Use the <b>rtp_free</b> macro defined in 
    rtpmem.h and turn off RTP_TRACK_LOCAL_MEMORY.

    @return void
 */
void _rtp_free (
  void * ptr                            /** Location that needs to be freed. */
  )
{
	free(ptr);
}

void * _rtp_calloc  (unsigned long num, unsigned long size)
{
	return calloc(num, size);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
