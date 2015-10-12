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
#include "rtp.h"
#include "rtpirq.h"
#include "rtp.h"
#define TX_DISABLE_ERROR_CHECKING 1
#include "tx_api.h"
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
    TX_INTERRUPT_SAVE_AREA    
    TX_DISABLE
    return TX_INTERRUPT_SAVED_VAL;
}

/*----------------------------------------------------------------------*
                              rtp_irq_pop
 *----------------------------------------------------------------------*/
void rtp_irq_pop (RTP_HANDLE previousLevel)
{
    TX_INTERRUPT_SAVE_AREA
    TX_INTERRUPT_SAVED_VAL = previousLevel;
    TX_RESTORE
}


/*----------------------------------------------------------------------*
                              rtp_irq_disable
 *----------------------------------------------------------------------*/
void rtp_irq_disable(void)
{
    TX_INTERRUPT_SAVE_AREA
    TX_DISABLE
}


/*----------------------------------------------------------------------*
                              rtp_irq_enable
 *----------------------------------------------------------------------*/
void rtp_irq_enable(void)
{
    TX_INTERRUPT_SAVE_AREA
#if 0
    TX_INTERRUPT_SAVED_VAL = TX_INT_ENABLE;
#else
    TX_DISABLE
    TX_INTERRUPT_SAVED_VAL &= ~(TX_INT_ENABLE | TX_INT_DISABLE);
    TX_INTERRUPT_SAVED_VAL |= TX_INT_ENABLE;
#endif
    TX_RESTORE
}

/*----------------------------------------------------------------------*
                          rtp_irq_push_disable_cooperative
 *----------------------------------------------------------------------*/
RTP_HANDLE rtp_irq_push_disable_cooperative (void)
{
    TX_INTERRUPT_SAVE_AREA
    TX_DISABLE
    return (RTP_HANDLE)TX_INTERRUPT_SAVED_VAL;
}

/*----------------------------------------------------------------------*
                              rtp_irq_pop_cooperative
 *----------------------------------------------------------------------*/
void rtp_irq_pop_cooperative(RTP_HANDLE previousLevel)
{
    TX_INTERRUPT_SAVE_AREA
    TX_INTERRUPT_SAVED_VAL = previousLevel;
    TX_RESTORE
}


/*----------------------------------------------------------------------*
                              rtp_irq_disable_cooperative
 *----------------------------------------------------------------------*/
void rtp_irq_disable_cooperative(void)
{
    TX_INTERRUPT_SAVE_AREA
    TX_DISABLE
}


/*----------------------------------------------------------------------*
                              rtp_irq_enable_hybrid
 *----------------------------------------------------------------------*/
void rtp_irq_enable_cooperative(void)
{
    TX_INTERRUPT_SAVE_AREA
#if 0
    TX_INTERRUPT_SAVED_VAL = TX_INT_ENABLE;
#else
    TX_DISABLE
    TX_INTERRUPT_SAVED_VAL &= ~(TX_INT_ENABLE | TX_INT_DISABLE);
    TX_INTERRUPT_SAVED_VAL |= TX_INT_ENABLE;
#endif
    TX_RESTORE
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
