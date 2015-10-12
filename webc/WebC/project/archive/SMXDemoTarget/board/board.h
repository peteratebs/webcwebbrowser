/***************************************************************************
 **
 **    This file defines the board specific definition
 **
 **    Used with ARM IAR C/C++ Compiler and Assembler.
 **
 **    (c) Copyright IAR Systems 2007
 **
 **    $Revision: 24636 $
 **
 ***************************************************************************/
#include <intrinsics.h>
#include <nxp/iolpc2478.h>
#include "arm_comm.h"

#ifndef __BOARD_H
#define __BOARD_H

#define I_RC_OSC_FREQ   (4MHZ)
#define MAIN_OSC_FREQ   (12MHZ)
#define RTC_OSC_FREQ    (32768UL)

#define SYS_FREQ        (72MHZ)

#define I2C0_INTR_PRIORITY  1
#define DLY_I2C_TIME_OUT    1000

#if defined(IAR_LPC_2478_STK)

// USB Data Link LED
#define USB_D_LINK_LED_MASK (1UL<<18)
#define USB_D_LINK_LED_DIR  IO1DIR
#define USB_D_LINK_LED_FDIR FIO1DIR
#define USB_D_LINK_LED_SET  IO1SET
#define USB_D_LINK_LED_FSET FIO1SET
#define USB_D_LINK_LED_CLR  IO1CLR
#define USB_D_LINK_LED_FCLR FIO1CLR
#define USB_D_LINK_LED_IO   IO1PIN
#define USB_D_LINK_LED_FIO  FIO1PIN
#define USB_D_LINK_LED_SEL  PINSEL3_bit.P1_18
#define USB_D_LINK_LED_MODE PINMODE3_bit.P1_18

// USB Host Link LED
#define USB_H_LINK_LED_MASK (1UL<<13)
#define USB_H_LINK_LED_DIR  IO1DIR
#define USB_H_LINK_LED_FDIR FIO1DIR
#define USB_H_LINK_LED_SET  IO1SET
#define USB_H_LINK_LED_FSET FIO1SET
#define USB_H_LINK_LED_CLR  IO1CLR
#define USB_H_LINK_LED_FCLR FIO1CLR
#define USB_H_LINK_LED_IO   IO1PIN
#define USB_H_LINK_LED_FIO  FIO1PIN
#define USB_H_LINK_LED_SEL  PINSEL2_bit.P1_13
#define USB_H_LINK_LED_MODE PINMODE2_bit.P1_13

// Buttons
#define BUT1_MASK           (1UL<<19)
#define BUT1_FDIR           FIO2DIR
#define BUT1_FIO            FIO2PIN

#define BUT2_MASK           (1UL<<21)
#define BUT2_FDIR           FIO2DIR
#define BUT2_FIO            FIO2PIN

// MMC/SD card switches
// Card present
#define MMC_CP_MASK         (1UL << 11)
#define MMC_CP_FDIR         FIO2DIR
#define MMC_CP_FIO          FIO2PIN
#define MMC_CP_MODE         PINMODE0_bit.P0_11

// Write protect
#define MMC_WP_MASK         (1UL << 19)
#define MMC_WP_FDIR         FIO4DIR
#define MMC_WP_FIO          FIO4PIN
#define MMC_WP_MODE         PINMODE9_bit.P4_19

// Analog trim
#define ANALOG_TRIM_CHANNEL 7
#define ANALOG_TRIM_CHANNEL_SEL   PINSEL0_bit.P0_13

// VS1002
#define VS1002_CS_AU_MASK   (1UL << 15)
#define VS1002_CS_AU_FDIR   FIO4DIR
#define VS1002_CS_AU_FSET   FIO4SET
#define VS1002_CS_AU_FCLR   FIO4CLR
#define VS1002_CS_AU_FIO    FIO4PIN
#define VS1002_CS_AU_SEL    PINSEL8_bit.P4_15
#define VS1002_CS_AU_MODE   PINMODE8_bit.P4_15

#define VS1002_DREQ_MASK    (1UL << 16)
#define VS1002_DREQ_FDIR    FIO4DIR
#define VS1002_DREQ_FIO     FIO4PIN
#define VS1002_DREQ_SEL     PINSEL9_bit.P4_16
#define VS1002_DREQ_MODE    PINMODE9_bit.P4_16

#define VS1002_SS_MASK      (1UL << 23)
#define VS1002_SS_FDIR      FIO2DIR
#define VS1002_SS_FIO       FIO2PIN
#define VS1002_SS_SEL       PINSEL5_bit.P2_23
#define VS1002_SS_MODE      PINMODE5_bit.P2_23

#define VS1002_MISO_SEL     PINSEL5_bit.P2_26
#define VS1002_MISO_MODE    PINMODE5_bit.P2_26

#define VS1002_MOSI_SEL     PINSEL5_bit.P2_27
#define VS1002_MOSI_MODE    PINMODE5_bit.P2_27

#define VS1002_SCLK_SEL     PINSEL5_bit.P2_22
#define VS1002_SCLK_MODE    PINMODE5_bit.P2_22

// Touch screen
#define TS_X1_MASK          (1UL << 24)
#define TS_X1_IO            IO0PIN
#define TS_X1_FIO           FIO0PIN
#define TS_X1_DIR           IO0DIR
#define TS_X1_FDIR          FIO0DIR
#define TS_X1_SET           IO0SET
#define TS_X1_CLR           IO0CLR
#define TS_X1_FSET          FIO0SET
#define TS_X1_FCLR          FIO0CLR
#define TS_X1_INTR_R        IO0INTENR
#define TS_X1_INTR_CLR      IO0INTCLR
#define TS_X1_SEL           PINSEL1_bit.P0_24
#define TS_X1_MODE          PINMODE1_bit.P0_24

#define TS_X2_MASK          (1UL << 22)
#define TS_X2_IO            IO0PIN
#define TS_X2_FIO           FIO0PIN
#define TS_X2_DIR           IO0DIR
#define TS_X2_FDIR          FIO0DIR
#define TS_X2_SET           IO0SET
#define TS_X2_CLR           IO0CLR
#define TS_X2_FSET          FIO0SET
#define TS_X2_FCLR          FIO0CLR
#define TS_X2_INTR_R        IO0INTENR
#define TS_X2_INTR_CLR      IO0INTCLR
#define TS_X2_SEL           PINSEL1_bit.P0_22
#define TS_X2_MODE          PINMODE1_bit.P0_22

#define TS_Y1_MASK          (1UL << 23)
#define TS_Y1_IO            IO0PIN
#define TS_Y1_FIO           FIO0PIN
#define TS_Y1_DIR           IO0DIR
#define TS_Y1_FDIR          FIO0DIR
#define TS_Y1_SET           IO0SET
#define TS_Y1_CLR           IO0CLR
#define TS_Y1_FSET          FIO0SET
#define TS_Y1_FCLR          FIO0CLR
#define TS_Y1_INTR_R        IO0INTENR
#define TS_Y1_INTR_CLR      IO0INTCLR
#define TS_Y1_SEL           PINSEL1_bit.P0_23
#define TS_Y1_MODE          PINMODE1_bit.P0_23

#define TS_Y2_MASK          (1UL << 21)
#define TS_Y2_IO            IO0PIN
#define TS_Y2_FIO           FIO0PIN
#define TS_Y2_DIR           IO0DIR
#define TS_Y2_FDIR          FIO0DIR
#define TS_Y2_SET           IO0SET
#define TS_Y2_CLR           IO0CLR
#define TS_Y2_FSET          FIO0SET
#define TS_Y2_FCLR          FIO0CLR
#define TS_Y2_INTR_R        IO0INTENR
#define TS_Y2_INTR_CLR      IO0INTCLR
#define TS_Y2_SEL           PINSEL1_bit.P0_21
#define TS_Y2_MODE          PINMODE1_bit.P0_21

#else
#error Define type of the board
#endif

// PCLK offset
#define WDT_PCLK_OFFSET     0
#define TIMER0_PCLK_OFFSET  2
#define TIMER1_PCLK_OFFSET  4
#define UART0_PCLK_OFFSET   6
#define UART1_PCLK_OFFSET   8
#define PWM0_PCLK_OFFSET    10
#define PWM1_PCLK_OFFSET    12
#define I2C0_PCLK_OFFSET    14
#define SPI_PCLK_OFFSET     16
#define RTC_PCLK_OFFSET     18
#define SSP1_PCLK_OFFSET    20
#define DAC_PCLK_OFFSET     22
#define ADC_PCLK_OFFSET     24
#define CAN1_PCLK_OFFSET    26
#define CAN2_PCLK_OFFSET    28
#define ACF_PCLK_OFFSET     30
#define BAT_RAM_PCLK_OFFSET 32
#define GPIO_PCLK_OFFSET    34
#define PCB_PCLK_OFFSET     36
#define I2C1_PCLK_OFFSET    38
//#define                   40
#define SSP0_PCLK_OFFSET    42
#define TIMER2_PCLK_OFFSET  44
#define TIMER3_PCLK_OFFSET  46
#define UART2_PCLK_OFFSET   48
#define UART3_PCLK_OFFSET   50
#define I2C2_PCLK_OFFSET    52
#define I2S_PCLK_OFFSET     54
#define MCI_PCLK_OFFSET     56
//#define                   58
#define PCLK_PCLK_OFFSET    60
//#define                   62

#endif /* __BOARD_H */
