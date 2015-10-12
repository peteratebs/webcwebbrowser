#pragma once



#include "webc.h"

#include "FB_HWInterface.h"

#define MAX_LOADSTRING 100 //windows variable


// impelementation of FBHWInterface for locosto device
class LocostoFrameBuffer : public FBHWInterface
{
public:

	static FBHWInterface *Only(); // Singleton

	virtual WEBC_UINT8 *GetBaseAddress(WEBC_INT whichBuffer=0);
	virtual WEBC_INT GetNumberOfBuffers();
	virtual WEBC_INT GetWidth();
	virtual WEBC_INT GetHeight();
	virtual WEBC_INT GetDepth();
	virtual WEBC_INT GetRowBytes();
	virtual void DoDisplay();
	virtual void SwitchToBuffer(WEBC_INT whichBuffer);

	void DoDisplay(); // for thread timer access only
    unsigned long hWnd;
protected:

	LocostoFrameBuffer();
	~LocostoFrameBuffer();

	//WebcGDIPlusBitmap* convertBitmap(WebGraphBitmapInfo* info, DISPLAY_INT xPos, DISPLAY_INT yPos, DISPLAY_INT width, DISPLAY_INT height);


	//Thread Ids Change these types for your individual implementation
	unsigned long timeoutManagerThreadId;//id for thread sending out timer messages

	WebGraphBitmapInfo clientDisplayBuffer, clientDisplayBuffer2;
	int m_displayBuffer;

	static LocostoFrameBuffer *m_only; // singleton instance
};
