/*************************************************************************
 *
*    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2007
 *
 *    File name   : sdram_32M_16bit_drv.c
 *    Description : SDRAM driver K4S561632E-TC(L)75 16MBx16bit
 *
 *    History :
 *    1. Date        : 6, March 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *
 *    $Revision: 26684 $
 **************************************************************************/

#include <stdio.h>
#include "board.h"
#include "sdram_32M_16bit_drv.h"

extern Int32U SDRAM_BASE_ADDR;

#if   SYS_FREQ == (72MHZ)
#define SDRAM_PERIOD          13.8  // 72MHz
#elif   SYS_FREQ == (57MHZ)
#define SDRAM_PERIOD          17.4  // 57.6MHz
#elif SYS_FREQ == (48MHZ)
#define SDRAM_PERIOD          20.8  // 48MHz
#elif SYS_FREQ == (36MHZ)
#define SDRAM_PERIOD          27.8  // 36MHz
#elif SYS_FREQ == (24MHZ)
#define SDRAM_PERIOD          41.7  // 24MHz
#else
#error Frequency not defined
#endif

#define P2C(Period)           (((Period<SDRAM_PERIOD)?0:(Int32U)((Flo32)Period/SDRAM_PERIOD))+1)

#define SDRAM_REFRESH         7813
#define SDRAM_TRP             20
#define SDRAM_TRAS            45
#define SDRAM_TAPR            1
#define SDRAM_TDAL            3
#define SDRAM_TWR             3
#define SDRAM_TRC             65
#define SDRAM_TRFC            66
#define SDRAM_TXSR            67
#define SDRAM_TRRD            15
#define SDRAM_TMRD            3

/*************************************************************************
 * Function Name: SDRAM_Init
 * Parameters: none
 *
 * Return: none
 *
 * Description: SDRAM controller and memory init
 *
 *************************************************************************/
void SDRAM_Init (void)
{
  // Assign pins to SDRAM controller
  PINSEL5 &= BIN32(11110000,11111100,11111100,11000000);
  PINSEL5 |= BIN32(00000101,00000001,00000001,00010101);
  PINMODE5&= BIN32(11110000,11111100,11111100,11000000);
  PINMODE5|= BIN32(00001010,00000010,00000010,00101010);
  PINSEL6  = BIN32(01010101,01010101,01010101,01010101);
  PINMODE6 = BIN32(10101010,10101010,10101010,10101010);
  PINSEL8 &= BIN32(11000000,00000000,00000000,00000000);
  PINSEL8 |= BIN32(00010101,01010101,01010101,01010101);
  PINMODE8&= BIN32(11000000,00000000,00000000,00000000);
  PINMODE8|= BIN32(00101010,10101010,10101010,10101010);
  PINSEL9 &= BIN32(11111111,11110011,11111111,11111111);
  PINSEL9 |= BIN32(00000000,00000100,00000000,00000000);
  PINMODE9&= BIN32(11111111,11110011,11111111,11111111);
  PINMODE9|= BIN32(00000000,00001000,00000000,00000000);

  // Init SDRAM controller
  // Enable EMC clock
  PCONP_bit.PCEMC = 1;
  EMCCONTROL      = 1;         // enable EMC
  EMCDINAMICRDCFG = 1;
  EMCDYNAMICRASCAS0_bit.CAS = 3;
  EMCDYNAMICRASCAS0_bit.RAS = 3;
  EMCDYNAMICRP = P2C(SDRAM_TRP);
  EMCDYNAMICRAS = P2C(SDRAM_TRAS);
  EMCDYNAMICSREX = P2C(SDRAM_TXSR);
  EMCDYNAMICAPR = SDRAM_TAPR;
  EMCDYNAMICDAL = SDRAM_TDAL+P2C(SDRAM_TRP);
  EMCDYNAMICWR = SDRAM_TWR;
  EMCDYNAMICRC = P2C(SDRAM_TRC);
  EMCDYNAMICRFC = P2C(SDRAM_TRFC);
  EMCDYNAMICXSR = P2C(SDRAM_TXSR);
  EMCDYNAMICRRD = P2C(SDRAM_TRRD);
  EMCDYNAMICMRD = SDRAM_TMRD;
  EMCDYNAMICCFG0 = 0x0000680;        // 13 row, 9 - col, SDRAM
  // JEDEC General SDRAM Initialization Sequence
  // DELAY to allow power and clocks to stabilize ~100 us
  // NOP
  EMCDINAMICCTRL = 0x0183;
  for(volatile Int32U i = 200*30; i;i--);
  // PALL
  EMCDINAMICCTRL_bit.I = 2;
  EMCDINAMICRFR = 1;
  for(volatile Int32U i= 128; i; --i); // > 128 clk
  EMCDINAMICRFR = P2C(SDRAM_REFRESH) >> 4;
  // COMM
  EMCDINAMICCTRL_bit.I = 1;
  // Burst 8, Sequential, CAS-2
  volatile unsigned long Dummy = *(volatile unsigned short *)
                                  ((Int32U)&SDRAM_BASE_ADDR + (0x33UL << (12)));
  // NORM
  EMCDINAMICCTRL = 0x0000;
  EMCDYNAMICCFG0_bit.B = 1;
  for(volatile Int32U i = 10000; i;i--);
}

Boolean SDRAM_Test (void)
{
  // 32 bits access
  for (Int32U i = 0; i < 0x2000000; i+=sizeof(Int32U))
  {
    *(Int32U*)((Int32U)&SDRAM_BASE_ADDR+i) = i;
  }

  for (Int32U i = 0; i < 0x2000000; i+=sizeof(Int32U))
  {
    if (*(Int32U*)((Int32U)&SDRAM_BASE_ADDR+i) != i)
    {
      printf("Verification error on address : 0x%x\n",(Int32U)&SDRAM_BASE_ADDR+i);
      return(FALSE);
    }
  }

  // 16 bits access
  for (Int32U i = 0; i < 0x10000; i+=sizeof(Int16U))
  {
    *(Int16U*)((Int32U)&SDRAM_BASE_ADDR+i) = i;
  }

  for (Int32U i = 0; i < 0x10000; i+=sizeof(Int16U))
  {
    if (*(Int16U*)((Int32U)&SDRAM_BASE_ADDR+i) != i)
    {
      printf("Verification error on address : 0x%x\n",(Int32U)&SDRAM_BASE_ADDR+i);
      return(FALSE);
    }
  }

  // 8 bits access
  for (Int32U i = 0; i < 0x100; i+=sizeof(Int8U))
  {
    *(Int8U*)((Int32U)&SDRAM_BASE_ADDR+i) = i;
  }

  for (Int32U i = 0; i < 0x100; i+=sizeof(Int8U))
  {
    if (*(Int8U*)((Int32U)&SDRAM_BASE_ADDR+i) != i)
    {
      printf("Verification error on address : 0x%x\n",(Int32U)&SDRAM_BASE_ADDR+i);
      return(FALSE);
    }
  }

  return(TRUE);
}