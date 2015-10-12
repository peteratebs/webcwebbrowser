
#include "FB_HWInterface.h"
#include "FBConfig.h"

extern "C" {
#include "board.h"
}

extern "C"  { extern unsigned long getVideoBaseAddr(int which); }

// impelementation of FBHWInterface for the NXP/Olimex/IAR LPC2478 development board
class LPC2478FrameBuffer : public FBHWInterface
{
public:
	WEBC_UINT8 *GetBaseAddress(WEBC_INT whichBuffer=0) { return ((WEBC_UINT8 *)  getVideoBaseAddr(whichBuffer)); }
	WEBC_INT GetNumberOfBuffers() { return 2; }
	WEBC_INT GetWidth() { return HORIZONTAL_RESOLUTION;} //320; }// fixme pmb
	WEBC_INT GetHeight() { return VERTICAL_RESOLUTION; }//240; }// fixme pmb
	WEBC_INT GetDepth() { return 4; }
	WEBC_INT GetRowBytes() { return 1280; }
	virtual void DoDisplay();
	void SwitchToBuffer(WEBC_INT whichBuffer)
	{
		LCD_UPBASE = (unsigned long) GetBaseAddress(whichBuffer) & ~7UL ;
		LCD_LPBASE = (unsigned long) GetBaseAddress(whichBuffer) & ~7UL ;
	}

	static FBHWInterface *Only()
		{
			if(!m_only)
				m_only = new LPC2478FrameBuffer();

			return m_only;
		}

protected:
  static LPC2478FrameBuffer * LPC2478FrameBuffer::m_only;
};
