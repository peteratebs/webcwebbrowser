 /*
 | PORTTARGET.H - Target specific setup and configuration parameters
 | HW_PPC83XX target, L3EDPB board 
 |
 | Copyright EBSnet Inc. , 2008
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/

#ifndef __PORTTARGET_H__
#define __PORTTARGET_H__

/************************************************************************
* PORTING DEFINES
**********************************************************************/
#define POLLOS  1
#define RTP_LONGWORD_ALLIGNED 
#define IMMRBASE  0xf0000000

/************************************************************************
* PORTTARGET.C - Runtime Platform Services
************************************************************************/
void port_target_init(void);

/************************************************************************
* RTPIRQ.C - Runtime Platform IRQ Services
************************************************************************/
#ifdef RTP_INCLUDE_IRQ

#endif /* RTP_INCLUDE_IRQ */

/************************************************************************
* RTPKERN.C - Runtime Platform Kernel Services
************************************************************************/
#ifdef RTP_INCLUDE_KERN

#endif /* RTP_INCLUDE_KERN */

/************************************************************************
* RTPMEM.C - Runtime Platform Memory System Services
************************************************************************/
#ifdef RTP_INCLUDE_MEM

#endif /* RTP_INCLUDE_MEM */

/************************************************************************
* RTPSIGNL.C - Runtime Platform Semaphore and Mutex Services
************************************************************************/
#ifdef RTP_INCLUDE_SIGNL

#endif /* RTP_INCLUDE_SIGNL */

/************************************************************************
* RTPTERM.C - Runtime Platform Terminal I/O Services
************************************************************************/
#ifdef RTP_INCLUDE_TERM

int     port_term_init           (void);
int     port_term_kbhit          (void);
int     port_term_getch          (void);
void    port_term_putc           (char ch);

#endif /* RTP_INCLUDE_TERM */

/************************************************************************
* RTPTHRD.C - Runtime Platform Thread Services
************************************************************************/
#ifdef RTP_INCLUDE_THRD

#endif /* RTP_INCLUDE_THRD */

/************************************************************************
* RTPTIME.C - Runtime Platform Timing Services
************************************************************************/
#ifdef RTP_INCLUDE_TIME
/* Use real time clock. Clock is set up by the debug config/boot code */
#define MPC83XX_CLOCK_FREQUENCY 10000 /* ticks/sec (100 usec tick) */

unsigned long port_get_system_msec (void);
unsigned long port_get_system_sec  (void);
unsigned long port_get_ticks_p_sec (void);

#endif /* RTP_INCLUDE_TIME */

/************************************************************************
************************************************************************/
#endif /* __PORTTARGET_H__ */

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
