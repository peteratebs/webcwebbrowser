//
// RTPKERN.C - Runtime Platform Kernel Services
//
//   PORTED TO THE POLLOS system
//
// EBSnet - RT-Platform 
//
//  $Author: sarah $
//  $Date: 2005/05/14 02:43:30 $
//  $Name:  $
//  $Revision: 1.1 $
//
// Copyright EBSnet Inc. , 2003
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/************************************************************************
* Headers
************************************************************************/
#include "rtpport.h"

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
void pollos_main(void);
/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                             rtp_kern_init
 *----------------------------------------------------------------------*/
void rtp_kern_init (void)
{

#if (POLMR60)
    CICR =0x00939F60;   /* set int levels, and vba 3 bits to 011 for int vectors */
    SDCR =0x740;
    ks_enable();		// enable processor interrupts in polled mode
#endif

#if (defined(POLCWM5272))
	setup_vector_table();
	rtp_irq_enable();

    /* set clock period to 20 millisconds   */
    mcf5272_set_clock(MCF52XX_CLOCK_PERIOD);
#endif
#ifdef INCLUDE_POLLOS_COOPERATIVE
	RTKernelInit(16);
#endif
}

/*----------------------------------------------------------------------*
                             rtp_kern_run
 *----------------------------------------------------------------------*/
void rtp_kern_run (void)
{
#ifdef INCLUDE_POLLOS_COOPERATIVE
	RTKSuspend(RTKCurrentTaskHandle());
#endif
}


/*----------------------------------------------------------------------*
                             rtp_kern_abort
 *----------------------------------------------------------------------*/
void rtp_kern_abort (void)
{
   ;// rtp_not_yet_implemented();
}


/*----------------------------------------------------------------------*
                             rtp_kern_exit
 *----------------------------------------------------------------------*/
void rtp_kern_exit (int exitvalue)
{
   ;//  rtp_not_yet_implemented();
}
#ifdef INCLUDE_POLLOS_COOPERATIVE
/*----------------------------------------------------------------------*
                             rtp_kern_run
 *----------------------------------------------------------------------*/
void rtp_kern_run_cooperative(void)
{
    RTKSuspend(RTKCurrentTaskHandle());
}
#endif
/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
