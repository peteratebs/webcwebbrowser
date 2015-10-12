 /*
 | RTPIRQ.H - Runtime Platform IRQ Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/21 16:04:17 $
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

#ifndef __RTPIRQ_H__
#define __RTPIRQ_H__

#include "rtp.h"

/************************************************************************
 * Type definitions
 ************************************************************************/
typedef RTP_HANDLE RTP_IRQCNTXT;

/************************************************************************
 * Kernel API                                                           *
 ************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

RTP_IRQCNTXT rtp_irq_push_disable (void);
void         rtp_irq_pop          (RTP_IRQCNTXT previousLevel);
void         rtp_irq_disable      (void);
void         rtp_irq_enable       (void);

#ifdef __cplusplus
}
#endif

#endif /* __RTPIRQ_H__ */

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
