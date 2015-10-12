 /*
 | RTPIRQ.C - Runtime Platform IRQ Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: manish $
 |  $Date: 2006/10/17 06:08:51 $
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
#include "rtpirq.h"

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
                          rtp_irq_push_disable
 *----------------------------------------------------------------------*/
RTP_HANDLE rtp_irq_push_disable (void)
{
	/* do nothing on win32 */
	return ((RTP_HANDLE)0);
}

/*----------------------------------------------------------------------*
                              rtp_irq_pop
 *----------------------------------------------------------------------*/
void rtp_irq_pop (RTP_HANDLE previousLevel)
{
	/* do nothing on win32 */
}

/*----------------------------------------------------------------------*
                              rtp_irq_disable
 *----------------------------------------------------------------------*/
void rtp_irq_disable (void)
{
	/* do nothing on win32 */
}

/*----------------------------------------------------------------------*
                              rtp_irq_enable
 *----------------------------------------------------------------------*/
void rtp_irq_enable (void)
{
	/* do nothing on win32 */
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
