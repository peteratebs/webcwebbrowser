/*********************************************************************
*  
*   IAR PowerPac
*
*   (c) Copyright IAR Systems 2008.  All rights reserved.
*
**********************************************************************
----------------------------------------------------------------------
File    : DCC_Process.c
Purpose : Data handler for ARM J-Link type communication via DCC
Notes   : (1) How to use
              In order to use the DCC communication to read / write memory, the
                following needs to be done:
              * Add this file to the project / make-file
              * Make sure this data handler is called regularly
              * Add the JLINKDCC data abort handler (optional)
                For details, refer to the documentation or see file JLINKDCC_HandleDataAbort.s79.
          (2) Compatibility
              The J-Link ARM DCC handler is compatible to the DCC communication
              protocol used by IAR in the embedded workbench for ARM and allows
              using the live data window in C-Spy
---------------------------END-OF-HEADER------------------------------
*/

#include <intrinsics.h>
#include "JLINKDCC.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/

#define SUPPORT_READ_U8     1   // Support byte reads if 1. 0 to disable.
#define SUPPORT_READ_U16    1   // Support half word reads if 1. 0 to disable.
#define SUPPORT_READ_U32    1   // Support word reads if 1. 0 to disable.
#define SUPPORT_WRITE_U8    1   // Support byte writes if 1. 0 to disable.
#define SUPPORT_WRITE_U16   1   // Support half word writes if 1. 0 to disable.
#define SUPPORT_WRITE_U32   1   // Support word writes if 1. 0 to disable.
#define SUPPORT_ABORT       1   // Support aborts.

/*********************************************************************
*
*       Defines, non- configurable
*
**********************************************************************
*/

#define U8  unsigned char
#define U16 unsigned short
#define U32 unsigned int

#define DCC_OP_READ_U32   0x01000000
#define DCC_OP_READ_U16   0x02000000
#define DCC_OP_READ_U8    0x04000000
#define DCC_OP_GET_CAPS   0x08000000
#define DCC_OP_WRITE_U32  0x10000000
#define DCC_OP_WRITE_U16  0x20000000
#define DCC_OP_WRITE_U8   0x40000000
#define DCC_OP_ODD_ADDR   0x80000000
#define DCC_OP_COMMAND    0x00000001

#define DCC_CAP_READ_U32  0x01
#define DCC_CAP_READ_U16  0x02
#define DCC_CAP_READ_U8   0x04
#define DCC_CAP_ABORT     0x08
#define DCC_CAP_WRITE_U32 0x10
#define DCC_CAP_WRITE_U16 0x20
#define DCC_CAP_WRITE_U8  0x40

#define DCC_SIGNATURE     0x91CA0000

#define DCC_CONFIG       ((DCC_CAP_READ_U8   * SUPPORT_READ_U8)    \
                        | (DCC_CAP_READ_U16  * SUPPORT_READ_U16)   \
                        | (DCC_CAP_READ_U32  * SUPPORT_READ_U32)   \
                        | (DCC_CAP_WRITE_U8  * SUPPORT_WRITE_U8)   \
                        | (DCC_CAP_WRITE_U16 * SUPPORT_WRITE_U16)  \
                        | (DCC_CAP_WRITE_U32 * SUPPORT_WRITE_U32)  \
                        | (DCC_CAP_ABORT     * SUPPORT_ABORT))

#define SUPPORT_WRITE   (SUPPORT_WRITE_U8 | SUPPORT_WRITE_U16 | SUPPORT_WRITE_U32)

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/

U8 JLINKDCC_IsInHandler;
U8 JLINKDCC_AbortOccurred;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static U16 _NumReadItems;
static U32 _Command;
static U32 _Addr;

static char _acBuffer[256];
static int  _RdPos;
static int  _WrPos;

#if (SUPPORT_WRITE)
  static U32 _Data;
#endif

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _ReadDCCStat
*/
static __interwork __arm int _ReadDCCStat(void) {
  return __MRC( 14, 0, 0, 0, 0 );       // __asm("mrc P14,0,R0,C0,C0,0");
}

/*********************************************************************
*
*       _ReadDCC
*/
static __interwork __arm U32 _ReadDCC(void) {
  return __MRC( 14, 0, 1, 0, 0 );       // __asm("mrc P14,0,R0,C1,C0,0");
}

/*********************************************************************
*
*       _WriteDCC
*/
static __interwork __arm void _WriteDCC(U32 Data) {
  __MCR( 14, 0, Data, 1, 0, 0 );        // __asm("mcr P14,0,R0,C1,C0,0");
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       DCC_Process
*
*  Function description
*    This function should be called more or less regularily to allow
*    memory reads while the application progam is running.
*    The more often it is called, the higher the memory read speed.
*/
void DCC_Process(void) {
  U32 Data;

  //
  // Avoid problems if this code is called from multiple threads or interrupts
  //
  if (JLINKDCC_IsInHandler) {
    return;
  }
  JLINKDCC_IsInHandler = 1;
  //
  // Perform communication for memory read / write as far as configured
  //
  if (_ReadDCCStat() & 1) {
    Data = _ReadDCC();
    if (Data & DCC_OP_COMMAND) {
      _Command = Data;
#if (DCC_CONFIG & (DCC_CAP_READ_U8 | DCC_CAP_WRITE_U8))
      if (_Command & DCC_OP_ODD_ADDR) {
        _Addr |= 1;
      }
#endif
      if (_Command & (DCC_OP_READ_U32 | DCC_OP_READ_U16 | DCC_OP_READ_U8 | DCC_OP_GET_CAPS)) {
        _NumReadItems = (_Command >> 2) & 0xffff;
      } else {
#if (DCC_CONFIG & DCC_CAP_WRITE_U32)
        if (_Command & DCC_OP_WRITE_U32) {
          _Data |= (_Command << 14) & 0xffff0000;
        } else
#endif
#if (SUPPORT_WRITE)
        {
          _Data = (_Command >> 2) & 0xffff;
        }
#endif
#if (DCC_CONFIG & DCC_CAP_WRITE_U8)
        if (_Command & DCC_OP_WRITE_U8) {
          *(U8*)_Addr = _Data;
          _Addr += 1;
        }
#endif
#if (DCC_CONFIG & DCC_CAP_WRITE_U16)
        if (_Command & DCC_OP_WRITE_U16) {
          *(U16*)_Addr = _Data;
          _Addr += 2;
        }
#endif
#if (DCC_CONFIG & DCC_CAP_WRITE_U32)
        if (_Command & DCC_OP_WRITE_U32) {
          *(U32*)_Addr =_Data;
          _Addr += 4;
        }
#endif
      }
      goto Done;
    }
    _Addr = Data;
  }
  if (_NumReadItems) {
    if ((_ReadDCCStat() & 2) == 0) {
      Data = (DCC_CONFIG | DCC_SIGNATURE);
#if (DCC_CONFIG & DCC_CAP_ABORT)
      Data |= (JLINKDCC_AbortOccurred << 8);
      if (_Command & DCC_OP_GET_CAPS) {
        JLINKDCC_AbortOccurred = 0;
      }
#endif
#if (DCC_CONFIG & DCC_CAP_READ_U8)
      if (_Command & DCC_OP_READ_U8) {
        Data = *(U8*)_Addr;
        _Addr += 1;
      }
#endif
#if (DCC_CONFIG & DCC_CAP_READ_U16)
      if (_Command & DCC_OP_READ_U16) {
        Data = *(U16*)_Addr;
        _Addr += 2;
      }
#endif
#if (DCC_CONFIG & DCC_CAP_READ_U32)
      if (_Command & DCC_OP_READ_U32) {
        Data = *(U32*)_Addr;
        _Addr += 4;
      }
#endif
      _WriteDCC(Data);
      _NumReadItems--;
    }
  }
Done:
  //
  // Handle terminal out. Up to 3 bytes in one 32-bit unit
  //
  if ((_ReadDCCStat() & 2) == 0) {
    int NumBytes;
    NumBytes = _WrPos - _RdPos;
    if (NumBytes < 0) {
      NumBytes += sizeof(_acBuffer);
    }
    if (NumBytes) {
      int i;
      if (NumBytes > 3) {
        NumBytes = 3;
      }
      Data = 0;
      for (i = 0; i < NumBytes; i++) {
        Data |= _acBuffer[_RdPos] << (8*i);
        _RdPos++;
        if (_RdPos == sizeof(_acBuffer)) {
          _RdPos = 0;
        }
      }
      Data |= (NumBytes - 1) << 24;
      _WriteDCC(Data);
    }
  }
  JLINKDCC_IsInHandler = 0;
}

/*********************************************************************
*
*       JLINKDCC_SendChar
*
*  Function description
*/
void JLINKDCC_SendChar(char c) {
  int Pos;

  Pos = _WrPos + 1;
  if (Pos == sizeof(_acBuffer)) {
    Pos = 0;
  }
  if (Pos == _RdPos) {
    return;
  }
  _acBuffer[_WrPos] = c;
  _WrPos = Pos;
}

/*********************************************************************
*
*       JLINKDCC_SendString
*
*  Function description
*/
void JLINKDCC_SendString(const char * s) {
  char c;
  while (1) {
    c = *s++;
    if (c == 0) {
      break;
    }
    JLINKDCC_SendChar(c);
  }
}




/*************************** end of file ****************************/
