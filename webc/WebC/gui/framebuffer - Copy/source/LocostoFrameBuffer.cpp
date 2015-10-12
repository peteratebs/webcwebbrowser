#include "LocostoFrameBuffer.h"
#include "FBConfig.h"
#include "webcmem.h"
#include "Remoteterminal.h"

//Target specific implementation defines
#define BYTES_PER_ROW	 (WEBC_SCREEN_WIDTH * BYTES_PER_PIXEL)


//#define HORIZONTAL_RESOLUTION 640
//#define VERTICAL_RESOLUTION 480

// bool threadExit;  //when the window manager is shutdown set this to true to stop the threads

LocostoFrameBuffer * LocostoFrameBuffer::m_only=WEBC_NULL;


// unsigned long timerThread(void* param);

#define USE_FAST_MEMORY 0
#if (USE_FAST_MEMORY)
unsigned char *fake_fb0;
unsigned char *fake_fb1;
#else
unsigned char fake_fb0[WEBC_SCREEN_WIDTH * WEBC_SCREEN_HEIGHT];
unsigned char fake_fb1[WEBC_SCREEN_WIDTH * WEBC_SCREEN_HEIGHT];
#endif


extern unsigned long fakeWindowHandle;
// needs to be called every 50 to 100 msecs.


extern "C" {


#if (MAX_VIRTUAL_DISPLAYS > 0)
HBROWSER_HANDLE VirtualDisplayGetCurrentBrowser(void);
#endif

void webc_timer_pulse(void)
{
HBROWSER_HANDLE which_Browser;
#if (MAX_VIRTUAL_DISPLAYS > 0)
	which_Browser = VirtualDisplayGetCurrentBrowser();
#else
	which_Browser = (HBROWSER_HANDLE)&fakeWindowHandle;
#endif
	call_TerminalTimout(which_Browser);	/* Check for input message */
	webc_BrowserProcessTimers(which_Browser);
	webc_BrowserDraw(which_Browser);
	webc_BrowserProcessLoadQueueAll(which_Browser);
	webc_BrowserDraw(which_Browser);
}

}


FBHWInterface *LocostoFrameBuffer::Only()
{
	if(!m_only)
	{
#if (USE_FAST_MEMORY)
		fake_fb0 = (unsigned char *) WEBC_FAST_MALLOC(WEBC_SCREEN_WIDTH * WEBC_SCREEN_HEIGHT);
		fake_fb1 = (unsigned char *) WEBC_FAST_MALLOC(WEBC_SCREEN_WIDTH * WEBC_SCREEN_HEIGHT);
		if (!fake_fb0 || !fake_fb1)
			return(0);
#endif
		m_only = new LocostoFrameBuffer();
	}

	return m_only;
}

/**
*	This method is used to initialize the OS specific code for the frame buffer layer.
*   The threads needed will be started here and the display section will also be set.
*/

	//windows variables for creating a window simulator
extern unsigned long hInst;								// current instance
extern char szWindowClass[MAX_LOADSTRING];			// the main window class name

LocostoFrameBuffer::LocostoFrameBuffer(){
	m_displayBuffer = 0;

}

/**
*	Destroys os specific threads and screen displaying mechanism.
*/
LocostoFrameBuffer::~LocostoFrameBuffer(){
}


WEBC_UINT8 *LocostoFrameBuffer::GetBaseAddress(WEBC_INT whichBuffer)
{
	return (WEBC_UINT8 *)(whichBuffer ? (WEBC_UINT8 *)fake_fb1 : (WEBC_UINT8 *)fake_fb0);
}

WEBC_INT LocostoFrameBuffer::GetNumberOfBuffers()
{
	return 1;
}

WEBC_INT LocostoFrameBuffer::GetWidth()
{
	return WEBC_SCREEN_WIDTH;
}

WEBC_INT LocostoFrameBuffer::GetHeight()
{
	return WEBC_SCREEN_HEIGHT;
}

WEBC_INT LocostoFrameBuffer::GetDepth()
{
	return BYTES_PER_PIXEL;
}

WEBC_INT LocostoFrameBuffer::GetRowBytes()
{
	return BYTES_PER_ROW;
}

void LocostoFrameBuffer::SwitchToBuffer(WEBC_INT whichBuffer)
{
	m_displayBuffer = whichBuffer;
}

/**
*	Window simulator specific. This method takes an internal bitmap representation and returns a Window bitmap.
*/


/**
*	This method will take the internal display that WebC has been working with
*   and transform it to an understandable format for the target device.
*
*	Params: disp - internal webc bitmap for screen display, this bitmap represents what must be
*				   displayed to the screen device
*
* Note: Not being used at the moment but perhaps should be considered.
*/
void LocostoFrameBuffer::DoDisplay()
{
}





















































