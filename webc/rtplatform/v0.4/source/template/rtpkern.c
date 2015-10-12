 /*
 | RTPKERN.C - Runtime Platform Network Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:31 $
 |  $Name:  $
 |  $Revision: 1.4 $
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
/** @memo   Initialize a kernel.

    @doc    Initialize a kernel.

    @return void
 */
void rtp_kern_init (void)
{
    rtp_not_yet_implemented ();
}

/*----------------------------------------------------------------------*
                             rtp_kern_run
 *----------------------------------------------------------------------*/
/** @memo   Begin multitasking.

    @doc    Begin multitasking.

    @return void
 */
void rtp_kern_run (void)
{
    rtp_not_yet_implemented ();
}


/*----------------------------------------------------------------------*
                             rtp_kern_abort
 *----------------------------------------------------------------------*/
/** @memo   Abort a process.

    @doc    Abort from the process.

    @return void
 */
void rtp_kern_abort (void)
{
    rtp_not_yet_implemented ();
}


/*----------------------------------------------------------------------*
                             rtp_kern_abort
 *----------------------------------------------------------------------*/
/** @memo   Exit an application.

    @doc    Exit an application.  The exit value is 0 if successfully 
    exiting, and -1 if exiting with a failure.

    @return void
 */
void rtp_kern_exit (
  int exitvalue                         /** Exit value; 0 for success, -1 otherwise. */
  )
{
    rtp_not_yet_implemented ();
}



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
