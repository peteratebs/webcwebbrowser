#if(0)
#include "FB_HWInterface.h"
#include "FBConfig.h"

extern "C" {
#include "board.h"
}

extern "C"  { extern unsigned long getVideoBaseAddr(int which); } 

// impelementation of FBHWInterface for the NXP/Olimex/IAR LPC2478 development board
class LPC2478FrameBuffer : public FBHWInterface{
public:
	WEBC_UINT8 *GetBaseAddress(WEBC_INT whichBuffer=0) { return ((WEBC_UINT8 *)  getVideoBaseAddr(whichBuffer)); }
	WEBC_INT GetNumberOfBuffers() { return 2; }
	WEBC_INT GetWidth() { return HORIZONTAL_RESOLUTION;} //320; }// fixme pmb
	WEBC_INT GetHeight() { return VERTICAL_RESOLUTION; }//240; }// fixme pmb
	WEBC_INT GetDepth() { return 4; }
	WEBC_INT GetRowBytes() { return 1280; }
	void SwitchToBuffer(WEBC_INT whichBuffer)
	{
#if(0)
		LCD_UPBASE = (unsigned long) GetBaseAddress(whichBuffer) & ~7UL ;
		LCD_LPBASE = (unsigned long) GetBaseAddress(whichBuffer) & ~7UL ;
#endif //CS3, might modify
	}

	static FBHWInterface *Only()
		{
			if(!m_only)
				m_only = new LPC2478FrameBuffer();
		
			return m_only;
		}

protected:
	//static LPC2478FrameBuffer * LPC2478FrameBuffer::m_only;
	static LPC2478FrameBuffer * m_only;
};


#else
///new CS3

#pragma once



#include "webc.h"

#include "FB_HWInterface.h"

#define MAX_LOADSTRING 100 //windows variable


// impelementation of FBHWInterface for locosto device
class LPC2478FrameBuffer : public FBHWInterface
{
public:

	static FBHWInterface *Only(); // Singleton

	virtual WEBC_UINT8 *GetBaseAddress(WEBC_INT whichBuffer=0);
	virtual WEBC_INT GetNumberOfBuffers();
	virtual WEBC_INT GetWidth();
	virtual WEBC_INT GetHeight();
	virtual WEBC_INT GetDepth();
	virtual WEBC_INT GetRowBytes();
	virtual void SwitchToBuffer(WEBC_INT whichBuffer);

	void DoDisplay(); // for thread timer access only
    unsigned long hWnd;
protected:

    LPC2478FrameBuffer();
	~LPC2478FrameBuffer();

	//WebcGDIPlusBitmap* convertBitmap(WebGraphBitmapInfo* info, DISPLAY_INT xPos, DISPLAY_INT yPos, DISPLAY_INT width, DISPLAY_INT height);


	//Thread Ids Change these types for your individual implementation
	unsigned long timeoutManagerThreadId;//id for thread sending out timer messages

	WebGraphBitmapInfo clientDisplayBuffer, clientDisplayBuffer2;
	int m_displayBuffer;

	static LPC2478FrameBuffer *m_only; // singleton instance
};
#endif
