/*
* USBISR.C                                                  Version 2.16
*
* USB Interrupt dispatcher for NXP LPC24XX.
*
* Copyright (c) 2007 Micro Digital Inc.
* All rights reserved. www.smxrtos.com
*
* This software is confidential and proprietary to Micro Digital Inc.
* It has been furnished under a license and may be used, copied, or
* disclosed only in accordance with the terms of that license and with
* the inclusion of this header. No title to nor ownership of this
* software is hereby transferred.
*
* Author: Yingbo Hu
*
* Portable to any ANSI compliant C compiler.
*
*****************************************************************************/

#include "smx.h"
#include "armdefs.h"
#include "bsp.h"

#if defined(SMXUSB) || defined(SMXUSBD) || defined(SMXUSBO)

#ifdef __cplusplus
extern "C" {
#endif

extern void LPC24XXUSBInterruptHandler(void);

#ifdef SMXUSB
extern void su_OHCIHostInterruptHandlerCore(void);
#endif

#ifdef SMXUSBD
extern void sud_LPCInterruptHandlerCore(void);
#endif

#ifdef SMXUSBO
extern void suo_LPC2468OTGIntHandler(void);
extern void suo_LPC2468ATXIntHandler(void);
#endif

extern void LPC24XXUSBInterruptHandler(void);


void LPC24XXUSBInitInterrupt(void)
{
    bspConfigureIRQ(22);
    bspSetIRQVect(22, (ISR_PTR)LPC24XXUSBInterruptHandler);
    USB_INT_STAT |= (u32)(0x1 << 31);
}


void LPC24XXUSBInterruptHandlerCore(void)
{
    u32 USBIntSt;
    USBIntSt = USB_INT_STAT;
#ifdef SMXUSB
    if(USBIntSt & (0x1 << 3)) /* Host Interrupt */
        su_OHCIHostInterruptHandlerCore();
#endif
#ifdef SMXUSBD
    if(USBIntSt & (0x1 << 0)) /* Device Interrupt */
        sud_LPCInterruptHandlerCore();
#endif
#ifdef SMXUSBO
    if(USBIntSt & (0x1 << 5)) /* OTG Interrupt */
      suo_LPC2468OTGIntHandler();
    if(USBIntSt & (0x1 << 4)) /* ATX Interrupt */
      suo_LPC2468ATXIntHandler();
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* SMXUSB || SMXUSBD || SMXUSBO */

