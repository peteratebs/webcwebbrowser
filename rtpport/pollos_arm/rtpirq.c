 /*
 | RTPIRQ.C - Runtime Platform IRQ Services
 |
 |   PORTED TO THE EBSnet Inc RTIP TCP/IP PLATFORM
 |
 | EBSnet - RT-Platform 
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
#include "rtip.h"
#include "os.h"
#include "rtipext.h"
#include "rtpmem.h"

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
	return((RTP_HANDLE)0);
}

/*----------------------------------------------------------------------*
                              rtp_irq_pop
 *----------------------------------------------------------------------*/
void rtp_irq_pop (RTP_HANDLE previousLevel)
{
    
}


/*----------------------------------------------------------------------*
                              rtp_irq_disable
 *----------------------------------------------------------------------*/
void rtp_irq_disable(void)
{
    
}


/*----------------------------------------------------------------------*
                              rtp_irq_enable
 *----------------------------------------------------------------------*/
void rtp_irq_enable(void)
{
	
}

/*----------------------------------------------------------------------*
                              rtp_irq_hook_interrupt
 *----------------------------------------------------------------------*/
RTP_BOOL rtp_irq_hook_interrupt(RTP_PFVOID pIface,
                                RTP_IRQ_FN_POINTER c_strategy,
                                RTP_IRQ_FN_POINTER c_interrupt)
{
    RTP_IRQCNTXT sp;
    int         vector_no;
    PIFACE      pi;
    
    pi = (PIFACE) pIface;
    
    /* Make the index to the isr strategy the same as the interface
        index.                                          
     */
    pi->irq_val          = pi->ctrl.index;
    
    if (pi->irq_val > (KS_NUM_INTS-1))
        return(RTP_FALSE);

    sp = rtp_irq_push_disable();
    
    
 #if (INCLUDE_SLIP || INCLUDE_CSLIP || INCLUDE_PPP || INCLUDE_TASK_ISRS)
    glb_os_data.rtip_isr_strategy[pi->irq_val]  = c_strategy;
    glb_os_data.rtip_args[pi->irq_val]          = pIface;
#endif
    
    
    
    rtp_irq_pop(sp);
    return(RTP_TRUE);

}

#ifdef INCLUDE_POLLOS_COOPERATIVE
/*----------------------------------------------------------------------*
                          rtp_irq_push_disable_cooperative
 *----------------------------------------------------------------------*/
RTP_HANDLE rtp_irq_push_disable_cooperative (void)
{
  return((RTP_HANDLE)0);
	
}

/*----------------------------------------------------------------------*
                              rtp_irq_pop_cooperative
 *----------------------------------------------------------------------*/
void rtp_irq_pop_cooperative(RTP_HANDLE previousLevel)
{
    

}


/*----------------------------------------------------------------------*
                              rtp_irq_disable_cooperative
 *----------------------------------------------------------------------*/
void rtp_irq_disable_cooperative(void)
{
	
}


/*----------------------------------------------------------------------*
                              rtp_irq_enable_hybrid
 *----------------------------------------------------------------------*/
void rtp_irq_enable_cooperative(void)
{
    
}
#endif
/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
