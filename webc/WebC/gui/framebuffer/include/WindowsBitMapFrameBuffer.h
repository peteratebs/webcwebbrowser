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
}
WebcGDIPlusBitmap;


// impelementation of FBHWInterface for a MS Windows window/bitmap
class TargetBitMapFrameBuffer : public FBHWInterface
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
; // for end disaplay access
	void DoDisplay(); // for thread timer access only
	HWND hWnd;
    static TargetBitMapFrameBuffer *m_only; // singleton instance


	TargetBitMapFrameBuffer();
	~TargetBitMapFrameBuffer();
protected:
	WebcGDIPlusBitmap* convertBitmap(WebGraphBitmapInfo* info, DISPLAY_INT xPos, DISPLAY_INT yPos, DISPLAY_INT width, DISPLAY_INT height);


	//Thread Ids Change these types for your individual implementation
	DWORD timeoutManagerThreadId;//id for thread sending out timer messages

	//Variables to display to the frame buffer
	Gdiplus::Graphics* g;//windows graphics to display on a windows system
	WebGraphBitmapInfo clientDisplayBuffer, clientDisplayBuffer2;
	int m_displayBuffer;
///	static TargetBitMapFrameBuffer *m_only; // singleton instance
};

