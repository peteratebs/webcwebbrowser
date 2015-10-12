/*********************************************************************
*  
*   IAR PowerPac
*
*   (c) Copyright IAR Systems 2008.  All rights reserved.
*
**********************************************************************
----------------------------------------------------------------------
File    : BSP.c
Purpose : BSP interface for Olimex LPC2478 eval board
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "BSP.h"
#include "RTOS.h"
#include "Global.h"
//#include "GUI.h"
//#include "LCDConf.h"

/*********************************************************************
*
*       Configuration
*
**********************************************************************
*/
#define TOUCHSAMPLEWATERMARK         (32)
#define TOUCHTIMER_INTERVAL          (10)
#define TS_ADC_N                   (1024)
#define TS_SETUP_DELAY               (50)  // 100us
#define TS_SAMPLE_DELAY           (10000)  // 10ms
#define TS_INIT_DELAY             (50000)  // 50ms
#define PCLK                   (18000000)  // @ 72MHz

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

/****** Interrupt sources *******************************************/
#define INT_TIM1                      (5)  // Timer1
#define INT_EINT3                    (17)  // External interrupt 3 (EINT3)
#define INT_AD0                      (18)  // A/D converter 0

/****** Port interrupt sources **************************************/
#define EINT3_BIT                     (3)

/****** LEDs ***********************************/
#define LED0_BIT                     (13)  // USB host LED
#define LED1_BIT                     (18)  // USB device LED

/****** Pin Select / Pin Mode identifiers ****/
#define TS_X1_PINBLOCK_BIT           (16)
#define TS_X2_PINBLOCK_BIT           (12)
#define TS_Y1_PINBLOCK_BIT           (14)
#define TS_Y2_PINBLOCK_BIT           (10)

/****** GPIO identifiers *********************/
#define TS_X1_PORT_BIT               (24)
#define TS_X2_PORT_BIT               (22)
#define TS_Y1_PORT_BIT               (23)
#define TS_Y2_PORT_BIT               (21)

/****** Peripheral identifiers ***************/
#define TIM1_BIT                      (2)
#define ADC_BIT                      (12)

/****** Addresses of sfrs and peripheral identifiers ****************/
#define PINSEL_BASE_ADDR             (0xE002C000)
#define PINMODE_BASE_ADDR            (0xE002C040)
#define IO_BASE_ADDR                 (0xE0028000)
#define LCDC_BASE_ADDR               (0xFFE10000)
#define ADC_BASE_ADDR                (0xE0034000)
#define TIM1_BASE_ADDR               (0xE0008000)

/****** SFRs and identifiers used for timer initialization **********/
#define TIM1_IR                      (*(volatile U32*) (TIM1_BASE_ADDR    + 0x0000))
#define TIM1_TCR                     (*(volatile U32*) (TIM1_BASE_ADDR    + 0x0004))
#define TIM1_PR                      (*(volatile U32*) (TIM1_BASE_ADDR    + 0x000C))
#define TIM1_MCR                     (*(volatile U32*) (TIM1_BASE_ADDR    + 0x0014))
#define TIM1_MR0                     (*(volatile U32*) (TIM1_BASE_ADDR    + 0x0018))
#define TIM1_CTCR                    (*(volatile U32*) (TIM1_BASE_ADDR    + 0x0070))

/****** SFRs and identifiers used for LED and LCD demo **************/
#define PINSEL0                      (*(volatile U32 *)(PINSEL_BASE_ADDR  + 0x0000))
#define PINSEL1                      (*(volatile U32 *)(PINSEL_BASE_ADDR  + 0x0004))
#define PINSEL2                      (*(volatile U32 *)(PINSEL_BASE_ADDR  + 0x0008))
#define PINSEL3                      (*(volatile U32 *)(PINSEL_BASE_ADDR  + 0x000C))
#define PINSEL4                      (*(volatile U32 *)(PINSEL_BASE_ADDR  + 0x0010))
#define PINSEL9                      (*(volatile U32 *)(PINSEL_BASE_ADDR  + 0x0024))
#define PINSEL11                     (*(volatile U32 *)(PINSEL_BASE_ADDR  + 0x002C))


#define PINMODE0                     (*(volatile U32 *)(PINMODE_BASE_ADDR + 0x0000))
#define PINMODE1                     (*(volatile U32 *)(PINMODE_BASE_ADDR + 0x0004))
#define PINMODE3                     (*(volatile U32 *)(PINMODE_BASE_ADDR + 0x000C))
#define PINMODE4                     (*(volatile U32 *)(PINMODE_BASE_ADDR + 0x0010))
#define PINMODE9                     (*(volatile U32 *)(PINMODE_BASE_ADDR + 0x0024))

#define IO0PIN                       (*(volatile U32 *)(IO_BASE_ADDR      + 0x0000))
#define IO0SET                       (*(volatile U32 *)(IO_BASE_ADDR      + 0x0004))
#define IO0DIR                       (*(volatile U32 *)(IO_BASE_ADDR      + 0x0008))
#define IO0CLR                       (*(volatile U32 *)(IO_BASE_ADDR      + 0x000C))
#define IO0INTENR                    (*(volatile U32 *)(IO_BASE_ADDR      + 0x0090))
#define IO0INTCLR                    (*(volatile U32 *)(IO_BASE_ADDR      + 0x008C))
#define IO1SET                       (*(volatile U32 *)(IO_BASE_ADDR      + 0x0014))
#define IO1DIR                       (*(volatile U32 *)(IO_BASE_ADDR      + 0x0018))
#define IO1CLR                       (*(volatile U32 *)(IO_BASE_ADDR      + 0x001C))

#define LCD_CFG                      (*(volatile U32 *)(0xE01FC1B8))

#define LCD_TIMH                     (*(volatile U32 *)(LCDC_BASE_ADDR    + 0x0000))
#define LCD_TIMV                     (*(volatile U32 *)(LCDC_BASE_ADDR    + 0x0004))
#define LCD_POL                      (*(volatile U32 *)(LCDC_BASE_ADDR    + 0x0008))
#define LCD_LE                       (*(volatile U32 *)(LCDC_BASE_ADDR    + 0x000C))
#define LCD_UPBASE                   (*(volatile U32 *)(LCDC_BASE_ADDR    + 0x0010))
#define LCD_LPBASE                   (*(volatile U32 *)(LCDC_BASE_ADDR    + 0x0014))
#define LCD_CTRL                     (*(volatile U32 *)(LCDC_BASE_ADDR    + 0x0018))
#define LCD_INTMSK                   (*(volatile U32 *)(LCDC_BASE_ADDR    + 0x001C))

#define AD0CR                        (*(volatile U32 *)(ADC_BASE_ADDR     + 0x0000))
#define AD0GDR                       (*(volatile U32 *)(ADC_BASE_ADDR     + 0x0004))
#define AD0STAT                      (*(volatile U32 *)(ADC_BASE_ADDR     + 0x0030))
#define AD0INTEN                     (*(volatile U32 *)(ADC_BASE_ADDR     + 0x000C))

#define PCONP                        (*(volatile U32 *)(0xE01FC0C4))

#define EXTINT                       (*(volatile U32 *)(0xE01FC140))
#define EXTMODE                      (*(volatile U32 *)(0xE01FC148))

/* Display size */
#define _XSIZE                       (320)
#define _YSIZE                       (240)

/* Video RAM address */
#define LCD_VRAM_ADDR                (U32)&_aVRAM[0]


#define MAC_MODULEID  *(volatile unsigned int*)(0xFFE00FFC) // MAC ID register
#define INT_INDEX_MAC  21

#if 0

/****** Video RAM ***************************************************/
static U32 _aVRAM[_XSIZE*_YSIZE];

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
enum {
  TS_INTERRUPT_SETUP_DELAY = 0,
  TS_WAIT_FOR_TOUCH,
  TS_X1_SETUP_DELAY,
  TS_X1_MEASURE,
  TS_X2_SETUP_DELAY,
  TS_X2_MEASURE,
  TS_Y1_SETUP_DELAY,
  TS_Y1_MEASURE,
  TS_Y2_SETUP_DELAY,
  TS_Y2_MEASURE,
};

static OS_TIMER _TouchTimer;
static OS_U16   _X         = 0;
static OS_U16   _Y         = 0;
static OS_U16   _XTemp;
static OS_U16   _YTemp;
static OS_U8    _State     = 0;
static OS_U8    _Touch     = 0;
static OS_U8    _TouchTemp = 0;
static OS_I32   _KoefY1;
static OS_I32   _KoefY2;
static OS_I32   _KoefX1;
static OS_I32   _KoefX2;
static OS_U32   _Samples;

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _IsTouch()
*/
static OS_U8 _IsTouch(OS_U16 * pX, OS_U16 * pY) {
  OS_I32 X;
  OS_I32 Y;

  if (_Touch) {
    X = _X;
    Y = _Y;
    //
    // Calculate X coordinate in pixels
    //
    X = (X * _KoefX1) + _KoefX2;
    X = (X >> 16);
    if (X < 0) {
      X = 0;
    } else if (X > _XSIZE) {
      X = _XSIZE;
    }
    //
    // Calculate Y coordinate in pixels
    //
    Y = (Y * _KoefY1) + _KoefY2;
    Y = (Y >> 16);
    if (Y < 0) {
      Y = 0;
    } else if (Y > _YSIZE) {
      Y = _YSIZE;
    }
    *pX = X;
    *pY = Y;
  }
  return _Touch;
}

/*********************************************************************
*
*       _CalcTouchKoef()
*/
static void _CalcTouchKoef(OS_U32 R1, OS_U16 N1, OS_U32 R2, OS_U16 N2, OS_I32 * pKoef1, OS_I32 * pKoef2) {
  OS_I32 Tmp;

  Tmp     = ((R2 - R1) << 16) / (N2 - N1);
  *pKoef1 = Tmp;
  *pKoef2 = (R1 << 16) - (Tmp * N1);
}

/*********************************************************************
*
*       _ExecTouch()
*/
static void _ExecTouch(void) {
  OS_U16 X;
  OS_U16 Y;

  if (_IsTouch(&X, &Y)) {
    GUI_TOUCH_StoreState(X, Y);
  } else {
    GUI_TOUCH_StoreState(-1, -1);
  }
  OS_RetriggerTimer(&_TouchTimer);
}

/*********************************************************************
*
*       _HandleTouch()
*/
static void _HandleTouch(void) {
  OS_U32 v;

  //
  // Disable and clear interrupt
  //
  IO0INTENR &= ~(1 << TS_X2_PORT_BIT);
  IO0INTCLR  =  (1 << TS_X2_PORT_BIT);
  //
  // Init ADC measure setup delay
  //
  if (_State == TS_WAIT_FOR_TOUCH) {
    IO0CLR = (1 << TS_Y1_PORT_BIT);  // Y1 = 0, Y2 = 1
    // Disable X2 pull down
    v         = PINMODE1;
    v         = (PINMODE1 & ~(3 << 12));
    v        |= (2 << 12);  // Disable pull-down for GPIO P0[22]
    PINMODE1  = v;
    //
    // Reset sample counter
    //
    _Samples = 0;
    //
    // Clear accumulators
    //
    _XTemp = 0;
    _YTemp = 0;
    // Init setup delay
    if (_Touch) {
      TIM1_MR0 = TS_SETUP_DELAY;
    } else {
      TIM1_MR0 = TS_INIT_DELAY;
    }
    _State = TS_X1_SETUP_DELAY;
    TIM1_TCR = (1 << 0);  // Start timer 1
  }
}

/*********************************************************************
*
*       _HandleADC()
*/
static void _HandleADC(void) {
  OS_U32 Data;
  OS_U32 v;

  AD0CR &= ~(7 << 24);              // Stop ADC
  Data = ((AD0GDR & 0xFFFF) >> 6);  // ADC result
  switch (_State) {
  case TS_X1_MEASURE:
    _YTemp += Data;
    if (++_Samples >= TOUCHSAMPLEWATERMARK) {
      _Samples = 0;
      _State   = TS_X2_SETUP_DELAY;
      //
      // Y2 = 0, Y1 = 1
      //
      IO0CLR = (1 << TS_Y2_PORT_BIT);
      IO0SET = (1 << TS_Y1_PORT_BIT);
      //
      // Init setup delay
      //
      TIM1_MR0 = TS_SETUP_DELAY;
      TIM1_TCR = (1 << 0);  // Start timer 1
    } else {
      AD0CR |= (1 << 24);  // Start ADC conversion
    }
    break;
  case TS_X2_MEASURE:
    _YTemp += 1023UL - Data;
    if (++_Samples >= TOUCHSAMPLEWATERMARK) {
      _Samples = 0;
      _State   = TS_Y1_SETUP_DELAY;
      //
      // X1 = 0, X2 = 1
      //
      IO0CLR   =  (1 << TS_X1_PORT_BIT);
      IO0SET   =  (1 << TS_X2_PORT_BIT);
      IO0DIR  |= ((1 << TS_X1_PORT_BIT) |
                  (1 << TS_X2_PORT_BIT));
      PINSEL1 &= ~(3 << TS_X1_PINBLOCK_BIT);  // GPIO P0[24]
      //
      // Y1 - ADC Ch0, Y2 input
      //
      IO0DIR  &= ~((1 << TS_Y1_PORT_BIT) |
                   (1 << TS_Y2_PORT_BIT));
      v        = PINSEL1;
      v       &= ~(3 << TS_Y1_PINBLOCK_BIT);
      v       |=  (1 << TS_Y1_PINBLOCK_BIT);  // Assign to ADC0 Ch0
      PINSEL1  = v;
      //
      // Select Ch0
      //
      v      = AD0CR;
      v     &= ~(0xFF);
      v     |= (1 <<  0);
      AD0CR  = v;
      //
      // Init setup delay
      //
      TIM1_MR0 = TS_SETUP_DELAY;
      TIM1_TCR = (1 << 0);  // Start timer 1
    } else {
      AD0CR |= (1 << 24);  // Start ADC conversion
    }
    break;
  case TS_Y1_MEASURE:
    _XTemp += 1023UL - Data;
    if (++_Samples >= TOUCHSAMPLEWATERMARK) {
      _Samples = 0;
      _State   = TS_Y2_SETUP_DELAY;
      //
      // X2 = 0, X1 = 1
      //
      IO0CLR = (1 << TS_X2_PORT_BIT);
      IO0SET = (1 << TS_X1_PORT_BIT);
      //
      // Init setup delay
      //
      TIM1_MR0 = TS_SETUP_DELAY;
      TIM1_TCR = (1 << 0);  // Start timer 1
    } else {
      AD0CR |= (1 << 24);   // Start ADC conversion
    }
    break;
  case TS_Y2_MEASURE:
    _XTemp += Data;
    if (++_Samples >= TOUCHSAMPLEWATERMARK) {
      _State = TS_INTERRUPT_SETUP_DELAY;
      //
      // Y1 = 1, Y2 = 1
      //
      IO0SET  = ((1 << TS_Y1_PORT_BIT) |
                 (1 << TS_Y2_PORT_BIT));
      IO0DIR |= ((1 << TS_Y1_PORT_BIT) |
                 (1 << TS_Y2_PORT_BIT));
      PINSEL1 &= ~(3 << TS_Y1_PINBLOCK_BIT);  // GPIO P0[23]
      //
      // X1 - ADC Ch1, X2 input with pull down
      //
      IO0DIR &= ~((1 << TS_X1_PORT_BIT) |
                  (1 << TS_X2_PORT_BIT));
      v        = PINSEL1;
      v       &= ~(3 << TS_X1_PINBLOCK_BIT);
      v       |=  (1 << TS_X1_PINBLOCK_BIT);  // Assign to ADC0 Ch1
      PINSEL1  = v;
      PINMODE1 |= (3 << TS_X2_PINBLOCK_BIT);  // Enable pull-down for GPIO P0[22]
      v      = AD0CR;
      v     &= ~(0xFF);
      v     |= (1 << 1);                      // Select channel 1
      AD0CR  = v;
      //
      // Init setup delay
      //
      TIM1_MR0   = TS_SAMPLE_DELAY;
      TIM1_TCR   = (1 << 0);   // Start timer 1
      _TouchTemp = 1;
    } else {
      AD0CR     |= (1 << 24);  // Start ADC conversion
    }
    break;
  default:
    break;
  }
}

/*********************************************************************
*
*       _HandleTimer1()
*/
static void _HandleTimer1(void) {
  OS_U32 v;

  TIM1_IR  |= (1 << 0);  // Clear pending interrupt
  TIM1_TCR |= (1 << 1);  // Reset counter
  switch (_State) {
  case TS_X1_SETUP_DELAY:
  case TS_X2_SETUP_DELAY:
  case TS_Y1_SETUP_DELAY:
  case TS_Y2_SETUP_DELAY:
    ++_State;
    AD0CR |= (1 << 24);  // Start ADC conversion
    break;
  case TS_INTERRUPT_SETUP_DELAY:
    ++_State;
    IO0INTCLR = (1 << TS_X2_PORT_BIT);
    if ((IO0PIN & (1 << TS_X2_PORT_BIT)) == 0) {
      _Touch      = 0;
      _TouchTemp  = 0;
      IO0INTENR  |= (1 << TS_X2_PORT_BIT);
    } else {
      //
      // Update X and Y
      //
      if (_TouchTemp) {
        _X = _XTemp;
        _Y = _YTemp;
      }
      _Touch = _TouchTemp;
      IO0CLR = (1 << TS_Y1_PORT_BIT);  // Y1 = 0, Y2 = 1
      //
      // Disable X2 pull down
      //
      v         = PINMODE1;
      v        &= ~(3 << TS_X2_PINBLOCK_BIT);
      v        |=  (2 << TS_X2_PINBLOCK_BIT);
      PINMODE1  = v;
      //
      // Reset sample counter
      //
      _Samples = 0;
      //
      // Clear accumulators
      //
      _XTemp = 0;
      _YTemp = 0;
      //
      // Init setup delay
      //
      if (_Touch) {
        TIM1_MR0 = TS_SETUP_DELAY;
      } else {
        TIM1_MR0 = TS_INIT_DELAY;
      }
      _State = TS_X1_SETUP_DELAY;
      TIM1_TCR = (1 << 0);  // Start timer 1
    }
    break;
  default:
    break;
  }
}

/*********************************************************************
*
*       _TouchInit()
*/
static void _TouchInit(void) {
  volatile U32 Dummy;
           U32 v;
           U32 Orientation;

  //
  // Init GPIOs
  //
  v  = PINSEL1;
  v &= ~((3 << TS_X1_PINBLOCK_BIT) |
         (3 << TS_X2_PINBLOCK_BIT) |
         (3 << TS_Y1_PINBLOCK_BIT) |
         (3 << TS_Y2_PINBLOCK_BIT));
  v |=  ((1 << TS_X1_PINBLOCK_BIT) |    // AD0[1]
         (0 << TS_X2_PINBLOCK_BIT) |    // GPIO
         (0 << TS_Y1_PINBLOCK_BIT) |    // GPIO
         (0 << TS_Y2_PINBLOCK_BIT));    // GPIO
  PINSEL1 = v;
  v = PINMODE1;
  v &= ~((3 << TS_X1_PINBLOCK_BIT) |
         (3 << TS_X2_PINBLOCK_BIT) |
         (3 << TS_Y1_PINBLOCK_BIT) |
         (3 << TS_Y2_PINBLOCK_BIT));
  v |=  ((2 << TS_X1_PINBLOCK_BIT) |    // Disbale pulls
         (3 << TS_X2_PINBLOCK_BIT) |    // Enable pull-down
         (2 << TS_Y1_PINBLOCK_BIT) |    // Disbale pulls
         (2 << TS_Y2_PINBLOCK_BIT));    // Disbale pulls
  PINMODE1 = v;
  //
  // Set port pins to input
  //
  IO0DIR &= ~((1 << TS_X1_PORT_BIT) |
              (1 << TS_X2_PORT_BIT));
  //
  // Set port pins to output
  //
  IO0DIR |=  ((1 << TS_Y1_PORT_BIT) |
              (1 << TS_Y2_PORT_BIT));
  //
  // Set port pins
  //
  IO0SET  =  ((1 << TS_Y1_PORT_BIT) |
              (1 << TS_Y2_PORT_BIT));
  //
  // calculate touch coefficients
  //
  _CalcTouchKoef(15, (((TOUCHSAMPLEWATERMARK * TS_ADC_N * 2) *  47UL) / 1000UL) + (TOUCHSAMPLEWATERMARK *  80 * 2),
                305, (((TOUCHSAMPLEWATERMARK * TS_ADC_N * 2) * 953UL) / 1000UL) - (TOUCHSAMPLEWATERMARK *  80 * 2),
                &_KoefX1, &_KoefX2);
  _CalcTouchKoef(15, (((TOUCHSAMPLEWATERMARK * TS_ADC_N * 2) *  63UL) / 1000UL) + (TOUCHSAMPLEWATERMARK * 113 * 2),
                225, (((TOUCHSAMPLEWATERMARK * TS_ADC_N * 2) * 938UL) / 1000UL) - (TOUCHSAMPLEWATERMARK * 113 * 2),
                &_KoefY1, &_KoefY2);
  //
  // Configure touch orientation
  //
  Orientation = (GUI_MIRROR_X * LCD_GetMirrorXEx(0)) |
                (GUI_MIRROR_Y * LCD_GetMirrorYEx(0)) |
                (GUI_SWAP_XY  * LCD_GetSwapXYEx (0)) ;
  GUI_TOUCH_SetOrientation(Orientation);
  //
  // Calibrate touch
  //
  GUI_TOUCH_Calibrate(GUI_COORD_X, 15, 305, 15, 305);
  GUI_TOUCH_Calibrate(GUI_COORD_Y, 15, 225, 15, 225);
  //
  // Init port interrupt
  //
  IO0INTENR &= ~(1 << TS_X2_PORT_BIT);  // Disable X2 rising edge interrupt
  IO0INTCLR  =  (1 << TS_X2_PORT_BIT);
  EXTMODE    =  (1 << EINT3_BIT);
  EXTINT     =  (1 << EINT3_BIT);
  OS_ARM_InstallISRHandler(INT_EINT3, &_HandleTouch);
  OS_ARM_ISRSetPrio(INT_EINT3, 15);     // Lowest priority
  OS_ARM_EnableISR(INT_EINT3);
  //
  // Init ADC
  //
  PCONP   |=  (1 << ADC_BIT);               // Enable peripheral clock for ADC
  AD0CR    = ((1 << 21) |                   // Converter is operational
              (0 << 24) |                   // Do not start
              (1 <<  1) |                   // Select channel 1
              ((PCLK / 500000) << 8) |      // CLKDIV
              (0 << 16) |                   // Disable burst
              (0 << 17));                   // 10 bits resolution
  while ((AD0STAT & (1 << 16)) != 0) {
    Dummy = AD0GDR;  // Clear all pending interrupts
  }
  AD0INTEN |= (1 << 8);            // Enable global interrupt
  OS_ARM_InstallISRHandler(INT_AD0, &_HandleADC);
  OS_ARM_ISRSetPrio(INT_AD0, 15);  // Lowest priority
  OS_ARM_EnableISR(INT_AD0);
  //
  // Init delay timer
  //
  PCONP     |= (1 << TIM1_BIT);           // Enable peripheral clock for TIM1
  TIM1_TCR   = (0 << 0) |                 // Disbale timer
               (1 << 1) ;                 // Reset timer
  TIM1_CTCR &= ~(3 << 0);                 // Timer Mode: every rising PCLK edge
  TIM1_MCR  |= ((1 << 0) |                // Interrupt on MR0
                (1 << 1) |                // Reset on MR0
                (1 << 2));                // Stop on MR0S
  TIM1_PR    = (PCLK / 1000000) - 1;      // 1us resolution
  TIM1_MR0   = TS_SETUP_DELAY;
  TIM1_IR   |= (1 << 0);                  // Clear pending interrupt
  OS_ARM_InstallISRHandler (INT_TIM1, &_HandleTimer1);
  OS_ARM_ISRSetPrio(INT_TIM1, 15);        // Lowest priority
  OS_ARM_EnableISR(INT_TIM1);
  TIM1_TCR   = (1 << 0);                  // Start timer 1
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       BSP_Init()
*/
void BSP_Init(void) {
  IO1DIR |= ((1 << LED0_BIT) |                    // Set LED ports to output
             (1 << LED1_BIT));
  IO1SET  = ((1 << LED0_BIT) | (1 << LED1_BIT));  // Initially clear all LEDs
  IO0DIR |= (1 << 23);
}
#endif
/*********************************************************************
*
*       ETH_Init
*/
void BSP_ETH_Init(unsigned Unit) {

  /*------------------------------------------------------------------------------
  * write to PINSEL2/3 to select the PHY functions on P1[17:0]
  *-----------------------------------------------------------------------------*/
  /* P1.6, ENET-TX_CLK, has to be set for EMAC to address a BUG in
  the rev"xx-X" or "xx-Y" silicon(see errata). On the new rev.(xxAY, released
  on 06/22/2007), P1.6 should NOT be set. */
  if (MAC_MODULEID == 0x39022000) {     // Older chip ?
    PINSEL2 = 0x50151105;	/* selects P1[0,1,4,6,8,9,10,14,15] */
  } else {
    PINSEL2 = 0x50150105;	/* selects P1[0,1,4,8,9,10,14,15] */
  }
  PINSEL3 = (PINSEL3 & ~0x0000000f) | 0x5;
}


/*********************************************************************
*
*       BSP_ETH_InstallISR
*/
void BSP_ETH_InstallISR(void (*pfISR)(void)) {
  OS_ARM_InstallISRHandler(INT_INDEX_MAC, pfISR);     // Timer/counter interrupt vector.
  OS_ARM_EnableISR(INT_INDEX_MAC);                    // Enable timer/counter 0 interrupt.
  OS_ARM_ISRSetPrio(INT_INDEX_MAC, 15);               // lowest priority
}

#if 0
/*********************************************************************
*
*       LED switching routines
*/

void BSP_SetLED(int Index) {
  if (Index > 1) {
    return;
  }
  IO1CLR = (1 << (LED0_BIT + (Index * 5)));
}

void BSP_ClrLED(int Index) {
  if (Index > 1) {
    return;
  }
  IO1SET = (1 << (LED0_BIT + (Index * 5)));
}

void BSP_ToggleLED(int Index) {
  if (Index > 1) {
    return;
  }
  if ((IO1SET & (1 << (LED0_BIT + (Index * 5)))) == 0) {  // LED is on
    BSP_ClrLED(Index);
  } else {
    BSP_SetLED(Index);
  }
}
/*********************************************************************
*
*       LCD_X_On
*/
void LCD_X_On(void) {
}

/*********************************************************************
*
*       LCD_X_Off
*/
void LCD_X_Off(void) {
}

/*********************************************************************
*
*       LCD_X_SetLUTEntry
*/
void LCD_X_SetLUTEntry(unsigned Pos, unsigned long Color) {
}

/*********************************************************************
*
*       LCD_X_InitController
*
* Function description:
*   Initializes the display controller
*/
void LCD_X_InitController(unsigned LayerIndex) {
  volatile U32 i;

  //
  // Set display size and video-RAM address
  //
  LCD_SetSizeEx (_XSIZE, _YSIZE, LayerIndex);
  LCD_SetVSizeEx(_XSIZE, _YSIZE, LayerIndex);
  LCD_SetVRAMAddrEx(LCD_VRAM_ADDR, LayerIndex);
  //
  // Setup port pins for LCD
  //
  LCD_CTRL &= ~(1 << 11);  // Disable LCD power
  for (i = 10000; i; i--);
  LCD_CTRL &= ~(1 << 0);   // Disable LCD
  PINSEL0  = (PINSEL0 & 0xFFF000FF)
             | (1 <<  8)    // I2SRX_CLK  / LCDVD[0]
             | (1 << 10)    // I2SRX_WS   / LCDVD[1]
             | (1 << 12)    // I2SRX_SDA  / LCDVD[8]
             | (1 << 14)    // I2STX_CLK  / LCDVD[9]
             | (1 << 16)    // I2STX_WS   / LCDVD[16]
             | (1 << 18)    // I2STX_SDA  / LCDVD[17]
             ;
  PINMODE0 = (PINMODE0 & 0xFFFC00FF)
             | (0x2 <<  8)    // Pin floating
             | (0x2 << 10)    // Pin floating
             | (0x2 << 12)    // Pin floating
             | (0x2 << 14)    // Pin floating
             | (0x2 << 16)    // Pin floating
             ;
  PINSEL3  = (PINSEL3 & 0xF00000FF)
             | (1 <<  8)    // USB_TX_DP1 / LCDVD[6]  / LCDVD[10]
             | (1 << 10)    // USB_TX_DM1 / LCDVD[7]  / LCDVD[11]
             | (1 << 12)    // USB_RCV1   / LCDVD[8]  / LCDVD[12]
             | (1 << 14)    // USB_RX_DP1 / LCDVD[9]  / LCDVD[13]
             | (1 << 16)    // USB_RX_DM1 / LCDVD[10] / LCDVD[14]
             | (1 << 18)    // USB_LS1    / LCDVD[11] / LCDVD[15]
             | (1 << 20)    // USB_SSPND1 / LCDVD[12] / LCDVD[20]
             | (1 << 22)    // USB_INT1   / LCDVD[13] / LCDVD[21]
             | (1 << 24)    // USB_SCL1   / LCDVD[14] / LCDVD[22]
             | (1 << 26)    // USB_SDA1   / LCDVD[15] / LCDVD[23]
             ;
  PINMODE3 = (PINMODE3 & 0xF00000FF)
             | (0x2 <<  8)    // Pin floating
             | (0x2 << 10)    // Pin floating
             | (0x2 << 12)    // Pin floating
             | (0x2 << 14)    // Pin floating
             | (0x2 << 16)    // Pin floating
             | (0x2 << 18)    // Pin floating
             | (0x2 << 20)    // Pin floating
             | (0x2 << 22)    // Pin floating
             | (0x2 << 24)    // Pin floating
             | (0x2 << 26)    // Pin floating
             ;
  PINSEL4  = (PINSEL4 & 0xF0300000)
             | (0x3 <<  0)  // TRACECLK   / LCDPWR
             | (0x3 <<  2)  // PIPESTAT0  / LCDLE
             | (0x3 <<  4)  // PIPESTAT1  / LCDDCLK
             | (0x3 <<  6)  // PIPESTAT2  / LCDFP
             | (0x3 <<  8)  // TRACESYNC  / LCDENAB   / LCDM
             | (0x3 << 10)  // TRACEPKT0  / LCDLP
             | (0x3 << 12)  // TRACEPKT1  / LCDVD[0]  / LCDVD[4]
             | (0x3 << 14)  // TRACEPKT2  / LCDVD[1]  / LCDVD[5]
             | (0x3 << 16)  // TRACEPKT3  / LCDVD[2]  / LCDVD[6]
             | (0x3 << 18)  // EXTIN0     / LCDVD[3]  / LCDVD[7]
             | (1   << 22)  // EINT1      / LCDCLKIN
             | (1   << 24)  // EINT2      / LCDVD[4]  / LCDVD[3]  / LCDVD[8]  / LCDVD[18]
             | (1   << 26)  // EINT3      / LCDVD[5]  / LCDVD[9]  / LCDVD[19]
             ;
  PINMODE4 = (PINMODE4 & 0xF0300000)
             | (0x2 <<  0)  // Pin floating
             | (0x2 <<  2)  // Pin floating
             | (0x2 <<  4)  // Pin floating
             | (0x2 <<  6)  // Pin floating
             | (0x2 <<  8)  // Pin floating
             | (0x2 << 10)  // Pin floating
             | (0x2 << 12)  // Pin floating
             | (0x2 << 14)  // Pin floating
             | (0x2 << 16)  // Pin floating
             | (0x2 << 18)  // Pin floating
             | (0x2 << 22)  // Pin floating
             | (0x2 << 24)  // Pin floating
             | (0x2 << 26)  // Pin floating
             ;
  PINSEL9  = (PINSEL9 & 0xF0FFFFFF)
             | (0x2 << 24)    // MAT2[0]    / LCDVD[6]  / LCDVD[10] / LCDVD[2]
             | (0x2 << 26)    // MAT2[1]    / LCDVD[7]  / LCDVD[11] / LCDVD[3]
             ;
  PINMODE9 = (PINMODE9 & 0xF0FFFFFF)
             | (0x2 << 24)    // Pin floating
             | (0x2 << 26)    // Pin floating
             ;
  PINSEL11 = (PINSEL11 & 0xFFFFFFF0)
             | (1   << 0)   // Enable LCD port
             | (0x7 << 1)   // LCD Mode: TFT 24-bit
             ;
  //
  // Initialize LCDC
  //
  PCONP   |= (1 << 20);   // Enable peripheral clock for LCDC
  LCD_CTRL &= ~(1 << 0);  // Disable the display
  LCD_CTRL   = 0
               | (0x6 << 1)        // 24 bits per pixel
               | (  1 << 5)        // TFT
               ;
  LCD_CTRL &= ~(1 << 11);  // Disable LCD power
  LCD_CFG    = 9;          // Clock divider
  LCD_POL    = 0
               |         (1 << 11)  // Invert vertical synchronization
               |         (1 << 12)  // Invert horizontal synchronization
               |         (1 << 13)  // Invert panel clock
               | ((320 - 1) << 16)  // Clocks per line = (Pixels per line / 1) - 1
               |         (1 << 26)  // Bypass pixel clock divider
               ;
  LCD_TIMH   = 0
               | (((320 / 16) - 1) <<  2)  // (Pixels per line / 16) - 1
               | ((30 - 1)         <<  8)  // Horizontal synchronization pulse width - 1
               | ((20 - 1)         << 16)  // Horizontal front porch - 1
               | ((38 - 1)         << 24)  // Horizontal back porch  - 1
               ;
  LCD_TIMV   = 0
               | ((240 - 1)        <<  0)  // Lines per panel - 1
               | ((  4 - 1)        << 10)  // Vertical synchronization pulse width - 1
               | ((  6 - 1)        << 16)  // Vertical front porch
               | (( 16 - 1)        << 24)  // Vertical back porch
               ;
  LCD_UPBASE = LCD_VRAM_ADDR;
  LCD_LPBASE = LCD_VRAM_ADDR;
  LCD_CTRL |= (1 << 0);   // Enable LCD
  for (i = 10000; i; i--);
  LCD_CTRL |= (1 << 11);  // Enable LCD power
  //
  // Start touchscreen
  //
  OS_IncDI();                      /* Initially disable interrupts  */
  _TouchInit();
  OS_DecRI();
  OS_CREATETIMER(&_TouchTimer, _ExecTouch, TOUCHTIMER_INTERVAL);
}

#endif
/*************************** End of file ****************************/
