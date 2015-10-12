#include "webc.h"
#include "FB_HWInterface.h"
#ifdef WIN32
#include "WindowsBitMapFrameBuffer.h"
#else
#include "TargetFrameBuffer.h"
#endif

// Implementation of FBHWInterface for a generic embedded target which calls TargetBitMapFrameBuffer for target specific actions. 
class FrameBufferDriver : public FBHWInterface
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

protected:

	FrameBufferDriver();
	~FrameBufferDriver();

	//Thread Ids Change these types for your individual implementation
// 	unsigned long timeoutManagerThreadId;//id for thread sending out timer messages

	//Variables to display to the frame buffer
	int m_displayBuffer;
	static FrameBufferDriver *m_only; // singleton instance

	TargetFrameBuffer *target_framebuffer;
};
