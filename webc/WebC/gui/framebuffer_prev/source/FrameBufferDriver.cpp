
#include "FBConfig.h"
#include "FBAbstraction.h"
#include "FrameBufferDriver.h"


FBHWInterface *FBInitFrameBufferDriver(void)
{
	return FrameBufferDriver::Only();
}

FrameBufferDriver * FrameBufferDriver::m_only=0;

FBHWInterface *FrameBufferDriver::Only()
{
	if(!m_only)
	{
		m_only = new FrameBufferDriver();

	}
	return m_only;
}


// ===
/**
*	This method is used to initialize the OS specific code for the frame buffer layer.
*   The threads needed will be started here and the display section will also be set.
*/

FrameBufferDriver::FrameBufferDriver(){
	target_framebuffer = new TargetFrameBuffer();
}

/**
*	Destroys os specific threads and screen displaying mechanism.
*/
FrameBufferDriver::~FrameBufferDriver(){
	if (target_framebuffer)
		target_framebuffer->~TargetFrameBuffer();
}

WEBC_UINT8 *FrameBufferDriver::GetBaseAddress(WEBC_INT whichBuffer)
{
		if (target_framebuffer)
			return target_framebuffer->GetBaseAddress(whichBuffer);
		return 0;
}

WEBC_INT FrameBufferDriver::GetNumberOfBuffers()
{
#if (USE_DOUBLE_BUFFERING)
	return 2;
#else
	return 1;
#endif
}

WEBC_INT FrameBufferDriver::GetWidth()
{
	return WEBC_SCREEN_WIDTH;
}

WEBC_INT FrameBufferDriver::GetHeight()
{
	return WEBC_SCREEN_WIDTH;
}

WEBC_INT FrameBufferDriver::GetDepth()
{
	return BYTES_PER_PIXEL;
}

WEBC_INT FrameBufferDriver::GetRowBytes()
{
	return WEBC_BYTES_PER_ROW;
}

void FrameBufferDriver::SwitchToBuffer(WEBC_INT whichBuffer)
{
		if (target_framebuffer)
			return target_framebuffer->SwitchToBuffer(whichBuffer);
}


/**
*	This method will take the internal display that WebC has been working with
*   and transform it to an understandable format for the target device.
*
*	Params: disp - internal webc bitmap for screen display, this bitmap represents what must be
*				   displayed to the screen device
*/
void FrameBufferDriver::DoDisplay()
{
	if (target_framebuffer)
			return target_framebuffer->DoDisplay();
}





