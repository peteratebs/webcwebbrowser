 /*
 | PORTTARGET.C - Target specific porting functions
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

#include "rtpport.h"

/* This provides a function to do any board/port specific initialization before
   calling the main application */
void port_target_init(void)
{


}

#ifdef RTP_INCLUDE_TERM
/* Use on chip duart. */
#define DUART1OFF 0x00004500

#define TXRDY 0x02
#define RXRDY 0x01
#define THRE  0x20

/* Define DUART register struct. */
typedef struct {
  union
  {
    RTP_UINT8 URBR;
    RTP_UINT8 UTHR;
    RTP_UINT8 UDLB;
  } b00;
  union
  {
    RTP_UINT8 UIER;
    RTP_UINT8 UDMB;
  } b01;
  union
  {
    RTP_UINT8 UIIR;
    RTP_UINT8 UFCR;
    RTP_UINT8 UAFR;
  } b02;
 
    RTP_UINT8 ULCR;
    RTP_UINT8 UMCR;
    RTP_UINT8 ULSR;
    RTP_UINT8 UMSR;
    RTP_UINT8 USCR;
    RTP_UINT8 UDSR;
} _port_duart_regs_struct;

int     port_term_init(void)
{
_port_duart_regs_struct *pduart_regs;
    pduart_regs = (_port_duart_regs_struct *)(IMMRBASE + DUART1OFF);

    pduart_regs->ULCR = 0x80;
    pduart_regs->b01.UDMB = 0x03;
    pduart_regs->b00.UDLB = 0x64;
    pduart_regs->ULCR = 0x03;
    
    pduart_regs->b02.UFCR = 0x01; /* enable fifo's */
    pduart_regs->b02.UFCR = 0x47;

    return(0);
}

int     port_term_kbhit(void)
{
_port_duart_regs_struct *pduart_regs;
    pduart_regs = (_port_duart_regs_struct *)(IMMRBASE + DUART1OFF);
    if (pduart_regs->ULSR & RXRDY)
	return(1);
    else
	return(0);
}

int     port_term_getch(void)
{
RTP_UINT8  ch;
_port_duart_regs_struct *pduart_regs;
    pduart_regs = (_port_duart_regs_struct *)(IMMRBASE + DUART1OFF);

    ch = pduart_regs->b00.URBR;
    return((int)ch);
}

void    port_term_putc(char ch)
{
int i;
_port_duart_regs_struct *pduart_regs;
    pduart_regs = (_port_duart_regs_struct *)(IMMRBASE + DUART1OFF);

    for (i=0; i<20000; i++)
    {
       if(!(pduart_regs->UDSR & TXRDY) && (pduart_regs->ULSR & THRE))
       {
          pduart_regs->b00.UTHR = (RTP_UINT8)ch;
          return;
       }
    }
}
#endif /* RTP_INCLUDE_TERM */

#ifdef RTP_INCLUDE_TIME
/* Use real time clock. Clock is set up by the debug config/boot code */
#define RTCTROFF 0x0000030c

unsigned long port_get_system_msec (void)
{
unsigned long *prtctr_regs;
    prtctr_regs = (unsigned long *)(IMMRBASE + RTCTROFF);
    
    return(*prtctr_regs/(MPC83XX_CLOCK_FREQUENCY/1000));
}

unsigned long port_get_system_sec  (void)
{
unsigned long *prtctr_regs;
    prtctr_regs = (unsigned long *)(IMMRBASE + RTCTROFF);

    return(*prtctr_regs/MPC83XX_CLOCK_FREQUENCY);
}

unsigned long port_get_ticks_p_sec (void)
{
    return((unsigned long)MPC83XX_CLOCK_FREQUENCY);
}
#endif /* RTP_INCLUDE_TIME */
