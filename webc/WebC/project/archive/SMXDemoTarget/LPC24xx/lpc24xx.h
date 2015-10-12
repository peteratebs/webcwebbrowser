/*
* LPC24xx.H                                               Version 3.7.1
*
* Defines for NXP (Philips) LPC24xx projects.
* All are similar so just use conditionals here for differences.
* Currently supported:  LPC2468, LPC2478.
*
* Copyright (c) 2007-2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author:  Wan Xingsheng
*
***********************************************************************/

#ifndef LPC24XX_H
#define LPC24XX_H

#include "lpc24xx_reg.h"

// PLL Configuration. Please check BSP.C if these values are changed.
//
// The 246x/7x PLL has a lot of limitations specified in the manual and
// errata, plus there are limitations on speeds supported by the different
// memory devices for various revisions of the chip, which severely limits
// our flexibility here. The settings below may not work on all versions
// of the chip. On the first revision '-', we could run at 72MHz from
// SDRAM. On newer chips 60MHz was the maximum that worked, but this put
// the PLLCLK value out of range, which is a tight 275 to 290 MHz. Although
// the system seemed to run ok, when we added USB it became unstable.
// A further restriction is that CCLK_DIV must be even since CCLK_DIV-1
// is loaded into the CCLKSEL field of CCLKCFG, which must be odd!

// Peripheral Bus Speed Divider
#if defined(BOARD_EA_LPC2468) || defined(BOARD_EA_LPC2478)
#define FOSC            12000000  // Crystal Freq.
#define PLL_M           12        // Must be 12 for FOSC=12 for PLLCLK to be in range 275 to 290 (erratum PLL.1).
#define PLL_N           1
#define PLLCLK          (((FOSC * 2) / PLL_N) * PLL_M)  // PLL output clock.

#define CCLK_DIV        6         // Must be >= 6 since it must be even (since
                                  // CCLK_DIV-1 is loaded into CCLKSEL must be
                                  // odd) and CCLK must be <= 60 MHz to run from
                                  // flash on older chips (erratum Flash.1) and
                                  // SDRAM on newer chips.
#define CCLK            (PLLCLK / CCLK_DIV) // CPU Clock

#define PCLK_DIV        2 
#define PCLK            (CCLK / PCLK_DIV)   // Peripheral Clock

#define USBCLK_DIV      6
#define USBCLK          (PLLCLK / USBCLK_DIV) // USB Clock (Should be 48MHz)

#elif defined(BOARD_IAR_LPC2478KS)
#define FOSC            12000000  // Crystal Freq.
#define PLL_M           12        // Must be 12 for FOSC=12 for PLLCLK to be in range 275 to 290 (erratum PLL.1).
#define PLL_N           1
#define PLLCLK          (((FOSC * 2) / PLL_N) * PLL_M)  // PLL output clock.

#define CCLK_DIV        4         // Must be even (since CCLK_DIV-1 is loaded
                                  // into CCLKSEL and must be odd).
#define CCLK            (PLLCLK / CCLK_DIV) // CPU Clock

#define PCLK_DIV        2 
#define PCLK            (CCLK / PCLK_DIV)   // Peripheral Clock

#define USBCLK_DIV      6
#define USBCLK          (PLLCLK / USBCLK_DIV) // USB Clock (Should be 48MHz)

#else
#error Define FOSC, PLL_M, PLL_N, CCLK_DIV, PCLK_DIV and USBCLK_DIV in LPC23xx.h for your board.
#endif

#include "uart.h"
#include "led.h"

#endif // LPC24XX_H
