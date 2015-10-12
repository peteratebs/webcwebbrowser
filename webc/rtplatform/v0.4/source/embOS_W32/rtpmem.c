 /*
 | RTPMEM.C - Runtime Platform Memory System Services
 |
 |   PORTED TO THE Segger embOS simulation for Windows 32 bit system
 |                 and Visual C++.
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: tom $
 |  $Date: 2004/10/22 18:11:57 $
 |  $Name:  $
 |  $Revision: 1.3 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |
 | Thread safe heap type memory management
*/



/************************************************************************
* Headers
************************************************************************/
#define RTP_INCLUDE_MEM
#include "rtpconfig.h"

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
void * _rtp_malloc (unsigned long size)
{
    return (OS_malloc ((size_t) size));
}


/*----------------------------------------------------------------------*
                             _rtp_calloc
 *----------------------------------------------------------------------*/
void * _rtp_calloc (unsigned long num, unsigned long size)
{
    rtp_not_yet_implemented();
    return (0);
}


/*----------------------------------------------------------------------*
                             _rtp_realloc
 *----------------------------------------------------------------------*/
void * _rtp_realloc (void * ptr, unsigned long size)
{
    return (OS_realloc (ptr, (size_t) size));
}


/*----------------------------------------------------------------------*
                              _rtp_free
 *----------------------------------------------------------------------*/
void _rtp_free (void * ptr)
{
    OS_free (ptr);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
