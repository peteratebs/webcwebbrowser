#include "webc.h"
#include "FB_HWInterface.h"


// impelementation of FBHWInterface for a generic embedded target.  MS Windows window/bitmap
class EmbeddedBitMapFrameBuffer : public FBHWInterface
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
; // for end disaplay access
	void DoDisplay(); // for thread timer access only
//	void *hWnd;		  // Not used
protected:

	EmbeddedBitMapFrameBuffer();
	~EmbeddedBitMapFrameBuffer();

	//Thread Ids Change these types for your individual implementation
// 	unsigned long timeoutManagerThreadId;//id for thread sending out timer messages

	//Variables to display to the frame buffer
	int m_displayBuffer;
	static EmbeddedBitMapFrameBuffer *m_only; // singleton instance
};
