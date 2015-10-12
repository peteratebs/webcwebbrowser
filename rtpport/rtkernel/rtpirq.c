 /*
 | RTPIRQ.C - Runtime Platform IRQ Services
 |
 |   PORTED TO THE EBSnet Inc RTIP TCP/IP PLATFORM
 |
 | EBSnet - EBS_RT-Platform 
 |
 |  $Author: yohannes $
 |  $Date: 2005//06/14 16:06:52 $
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
#include "sock.h"
#include "rtip.h"
#include "rtipext.h"
#include "rtp.h"
#include "rtpirq.h"
#include <rtk32.h>
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
	RTP_HANDLE old_level;
	
	old_level = _rtkIRQSaveAndDisableIFFlag();
	return(old_level);
}

/*----------------------------------------------------------------------*
                              rtp_irq_pop
 *----------------------------------------------------------------------*/
void rtp_irq_pop (RTP_HANDLE previousLevel)
{
    _rtkIRQRestoreIFFlag(previousLevel);
}


/*----------------------------------------------------------------------*
                              rtp_irq_disable
 *----------------------------------------------------------------------*/
void rtp_irq_disable(void)
{
	RTKDisableInterrupts();
	
}


/*----------------------------------------------------------------------*
                              rtp_irq_enable
 *----------------------------------------------------------------------*/
void rtp_irq_enable(void)
{
    RTKEnableInterrupts();
 }

/*----------------------------------------------------------------------*
                              rtp_irq_hook_interrupt
 *----------------------------------------------------------------------*/
RTP_BOOL rtp_irq_hook_interrupt(RTP_PFVOID pIface,
                                RTP_IRQ_FN_POINTER c_strategy,
                                RTP_IRQ_FN_POINTER c_interrupt)
{
    int         sp;
    PIFACE      pi;
    int 	    IRQ     =   RTKMAXIRQS - 24; 

    pi = (PIFACE) pIface;
    
    /* Make the index to the isr strategy the same as the interface index. */
    
    
    sp = rtp_irq_push_disable();
    
    _rtkIRQSetSysHandler(IRQ, (RTKIRQHandler) c_interrupt); 
    _rtkIRQSetSysHandlerArgs(IRQ, 1, pi->minor_number, 0);               
    RTKEnableIRQ(IRQ);

#if (INCLUDE_TASK_ISRS)
    pi->irq_no          = pi->ctrl.index;
    
    if (pi->irq_no > (KS_NUM_INTS-1))
    {
        return(RTP_FALSE);
    }

    rtip_isr_strategy[pi->irq_no]  = c_strategy;
    rtip_args[pi->irq_no]          = pi->minor_number;
     
    OS_SET_INTERRUPT_SIGNAL(pi);
#endif

    rtp_irq_pop(sp);
    
    return(RTP_TRUE);

}
/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
