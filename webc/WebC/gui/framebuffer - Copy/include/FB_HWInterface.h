#pragma once
/*
|  FB_GRAPHICS.H -
|
|	This is the graphics class for the WebC Graphics Layer. What this does
|   is it creates a surface based on the constructor to be drawn on.  It also
|   offers various methods to draw onto the surface.
|
|  EBS -
|
|  Copyright EBS Inc. , 2009
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|	Description: Hardware abstraction for a dumb frame buffer.
|
|
*/
#include "webc.h"

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
	virtual void DoDisplay() = 0;
	virtual void SwitchToBuffer(WEBC_INT whichBuffer) = 0;
};

FBHWInterface *FBInitFrameBufferDriver(void);