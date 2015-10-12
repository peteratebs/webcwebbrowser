 /*
 | RTPIRQ.C - Runtime Platform IRQ Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - EBS_RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/11/09 22:03:36 $
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
#include "rtpirq.h"
#if (INCLUDE_RTIP)
#include "os.h"
#include "rtip.h"     
#include "rtipext.h" 
#endif

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
    /* Get critical semaphore */
#if (INCLUDE_RTIP)
    OS_ENTER_CRITICAL(RTPLATFORM_IRQ_CRITICAL)
#else
    /* do nothing on win32 */
#endif
    return ((RTP_HANDLE)0);
}

/*----------------------------------------------------------------------*
                              rtp_irq_pop
 *----------------------------------------------------------------------*/
void rtp_irq_pop (RTP_HANDLE previousLevel)
{
    /* Free critical semaphore */
#if (INCLUDE_RTIP)
    OS_EXIT_CRITICAL()
#else
    /* do nothing on win32 */
#endif
}

/*----------------------------------------------------------------------*
                              rtp_irq_disable
 *----------------------------------------------------------------------*/
void rtp_irq_disable (void)
{

    /* Get critical semaphore */
#if (INCLUDE_RTIP)
    OS_ENTER_CRITICAL(RTPLATFORM_IRQ_CRITICAL)
#else
    /* do nothing on win32 */
#endif

}

/*----------------------------------------------------------------------*
                              rtp_irq_enable
 *----------------------------------------------------------------------*/
void rtp_irq_enable (void)
{
    /* Free critical semaphore */
#if (INCLUDE_RTIP)
    OS_EXIT_CRITICAL()
#else
    /* do nothing on win32 */
#endif

}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
