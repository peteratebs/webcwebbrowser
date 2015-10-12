 /*
 | RTPKERN.C - Runtime Platform Network Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: tony $
 |  $Date: 2004/12/21 22:13:39 $
 |  $Name:  $
 |  $Revision: 1.1 $
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
#include "rtpkern.h"

#include <windows.h>
#include <process.h>

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
                             rtp_kern_init
 *----------------------------------------------------------------------*/
void rtp_kern_init (void)
{
    /* do nothing on win32 */
}

/*----------------------------------------------------------------------*
                             rtp_kern_run
 *----------------------------------------------------------------------*/
void rtp_kern_run (void)
{
	for (;;)
	{
        Sleep(10000);
    }
    exit(0);
}

/*----------------------------------------------------------------------*
                             rtp_kern_abort
 *----------------------------------------------------------------------*/
void rtp_kern_abort (void)
{
    abort();
}


/*----------------------------------------------------------------------*
                             rtp_kern_abort
 *----------------------------------------------------------------------*/
void rtp_kern_exit (int exitvalue)
{
    exit(exitvalue);
}



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */