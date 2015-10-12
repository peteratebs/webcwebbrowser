#pragma once

#include <windows.h>
#include <gdiplus.h>
#include "webc.h"

#include "FB_HWInterface.h"

#define MAX_LOADSTRING 100 //windows variable


typedef struct
{
	Gdiplus::Bitmap*              bitmap;
	WEBC_UINT8*          bitmapData;
} WebcGDIPlusBitmap;


// implementation of EmbeddedBitMapFrameBuffer
class TargetFrameBuffer
{
public:
	virtual WEBC_UINT8 *GetBaseAddress(WEBC_INT whichBuffer=0);
	virtual void DoDisplay(void);
	virtual void SwitchToBuffer(WEBC_INT whichBuffer);
	HWND hWnd;
	TargetFrameBuffer();
	~TargetFrameBuffer();
protected:
	WebcGDIPlusBitmap* convertBitmap(WebGraphBitmapInfo* info, DISPLAY_INT xPos, DISPLAY_INT yPos, DISPLAY_INT width, DISPLAY_INT height);

	//Thread Ids Change these types for your individual implementation
	DWORD timeoutManagerThreadId;//id for thread sending out timer messages

	//Variables to display to the frame buffer
	Gdiplus::Graphics* g;//windows graphics to display on a windows system
	WebGraphBitmapInfo clientDisplayBuffer, clientDisplayBuffer2;
	int m_displayBuffer;
///	static WindowsBitMapFrameBuffer *m_only; // singleton instance
};
