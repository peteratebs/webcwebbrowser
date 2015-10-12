#pragma once

#include "webc.h"

#include "FB_HWInterface.h"

#define MAX_LOADSTRING 100 //windows variable

// implementation of EmbeddedBitMapFrameBuffer
class TargetFrameBuffer
{
public:
	virtual WEBC_UINT8 *GetBaseAddress(WEBC_INT whichBuffer=0);
	virtual void DoDisplay(void);
	virtual void SwitchToBuffer(WEBC_INT whichBuffer);
	TargetFrameBuffer();
	~TargetFrameBuffer();
protected:
	int m_displayBuffer;
};
