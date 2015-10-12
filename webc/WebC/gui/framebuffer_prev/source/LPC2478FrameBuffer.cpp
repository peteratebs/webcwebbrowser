

#include "LPC2478FrameBuffer.h"


LPC2478FrameBuffer * LPC2478FrameBuffer::m_only=WEBC_NULL;

// FB_HWInterface represents the typical hardware functionality of a dumb frame buffer
class FBHWInterface
{
public:
	virtual WEBC_UINT8 *GetBaseAddress(WEBC_INT whichBuffer=0) = 0;
	virtual WEBC_INT GetNumberOfBuffers() = 0;
	virtual WEBC_INT GetWidth() = 0;
	virtual WEBC_INT GetHeight() = 0;
	virtual WEBC_INT GetDepth() = 0;
	virtual WEBC_INT GetRowBytes() = 0;
	virtual void SwitchToBuffer(WEBC_INT whichBuffer) = 0;
};

