/*
 | RTP.C - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: sarah $
 |  $Date: 2005/05/14 17:35:21 $
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

/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/
#if (0)  /* SPRZ */
PRTP_CALLBACKS rtp_callbacks;
#endif

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/

/************************************************************************
* Function Bodies
************************************************************************/

#if (0)  /* SPRZ */
// ********************************************************************
/* xn_register_callbacks() - register all callbacks from RTIP to application           */
/*                                                                                     */
/* Summary:                                                                            */
/*   #include "rtipapi.h"                                                              */
/*                                                                                     */
/*   void xn_register_callbacks(PRTIP_CALLBACKS rcb)								   */
/*         PRTIP_CALLBACKS rcb - structure which contains functions so that            */ 
/*                               RTIP can make call backs to the application		   */
/*                        															   */
/*                                                                                     */
/* Description:                                                                        */
/*                                                                                     */
/*   Registers functions in the application so RTIP can call them when				   */
/*   certain events occurs.  The parameter rcb is filled with the 					   */
/*   functions which should be called.  If and entry in the parameter rcb			   */
/*   is 0, a callback to the application will not be made for that event.              */
/*                                                                                     */
/* Returns:                                                                            */
/*   Nothing                                                                           */
/*                                                                                     */
void rtp_register_callbacks(PRTP_CALLBACKS rtp_cb)
{
	rtp_callbacks = rtp_cb;
}
#endif


/*----------------------------------------------------------------------*
                     rtp_not_yet_implemented
 *----------------------------------------------------------------------*/
/** @memo   Debug aid to determine function use.

    @doc    This function is called if a porting function is referenced 
    but not yet ported to the target platform.  Place a breakpoint here 
    and view the call stack to determine which function still needs 
    porting.

    @return void
 */
void rtp_not_yet_implemented (void)
{
    /* ----------------------------------- */
    /* ----------------------------------- */
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
