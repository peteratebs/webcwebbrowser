#ifndef WIN32

#include "EmbeddedBitMapFrameBuffer.h"
#include "webcmem.h"
#include "assert.h"
#include "FBConfig.h"
#include "Remoteterminal.h"
#include "FBAbstraction.h"
#include "rtptime.h"

static void QueCopyEventForWebCFromIsr(HTMLEvent *pe);

// Conmfigure p[arameters here
#define BYTES_PER_ROW 1280
#define HORIZONTAL_RESOLUTION 320
#define VERTICAL_RESOLUTION   240
#define NUMBER_OF_BUFFERS 2
#define COLOR_DEPTH	4
#if(__OLIMEX__)
//#include "lpc2478.h"
extern unsigned char lcdram0[320*240*4];
extern unsigned char lcdram1[320*240*4];
#endif
WEBC_UINT8 *getVideoBaseAddr(WEBC_INT whichBuffer);

extern "C" {
typedef void (*POLLFUNPTR)(void);

typedef struct rtp_poll_record_s {
  POLLFUNPTR proutine;
  unsigned long poll_period;
  unsigned long next_expiration;
  int 	   is_active;
  int 	   is_inside;
  struct rtp_poll_record_s *pnext;
} rtp_poll_record;

rtp_poll_record webc_timer_container;
rtp_poll_record webc_poll_container;
void rtp_poll_arm(rtp_poll_record *pslot, POLLFUNPTR proutine, unsigned long period);
}
extern "C" void PeriodicTimerCB(void);
extern "C" void webCPollingLoopCB(void);
static int EmbeddedBitmapCallbacksArmed;
void EmbeddedBitmapArmCallbacks(void)
{
	if (EmbeddedBitmapCallbacksArmed)
		return;
	EmbeddedBitmapCallbacksArmed=1;
	rtp_poll_arm(&webc_timer_container, PeriodicTimerCB, 100);
	rtp_poll_arm(&webc_poll_container, webCPollingLoopCB, 0);
}

WEBC_UINT8 *EmbeddedBitmapVideoBaseAddr(WEBC_INT whichBuffer)
{
	return ((WEBC_UINT8 *)  getVideoBaseAddr(whichBuffer));
}

WEBC_UINT8 *getVideoBaseAddr(WEBC_INT whichBuffer)
{
	if (whichBuffer)
		return (WEBC_UINT8*) &lcdram1[0];
	else
		return (WEBC_UINT8*) &lcdram0[0];
}

WEBC_INT EmbeddedBitMapGetNumberOfBuffers(void)
{
	return 2;
}

WEBC_INT EmbeddedBitMapGetWidth(void)
{
	return HORIZONTAL_RESOLUTION;
}

WEBC_INT EmbeddedBitMapGetHeight(void)
{
	return VERTICAL_RESOLUTION;
}

WEBC_INT EmbeddedBitMapGetDepth(void)
{
	return COLOR_DEPTH;
}

WEBC_INT EmbeddedBitMapGetRowBytes(void)
{
	return BYTES_PER_ROW;
}

void EmbeddedBitmapSwitchToBuffer(WEBC_INT whichBuffer)
{
#if(__OLIMEX__)
	LCD_UPBASE = (unsigned long) getVideoBaseAddr(whichBuffer) & ~7UL ;
	LCD_LPBASE = (unsigned long) getVideoBaseAddr(whichBuffer) & ~7UL ;
#endif //__OLIMEX__
}


void wcProcessTimedCallbacks(void);

HTMLEventStatus call_BrowserDispatchEvent (HBROWSER_HANDLE hbrowser,	HTMLEvent* event);

int threadExit;  //when the window manager is shutdown set this to true to stop the threads

EmbeddedBitMapFrameBuffer * EmbeddedBitMapFrameBuffer::m_only=WEBC_NULL;

FBHWInterface *EmbeddedBitMapFrameBuffer::Only()
{
	if(!m_only)
		m_only = new EmbeddedBitMapFrameBuffer();
	EmbeddedBitmapArmCallbacks();
	return m_only;
}
// Resizes a window so the client area contains the specified size.
static void SetClientSize( void * hwnd, int clientWidth, int clientHeight);


// ===
/**
*	This method is used to initialize the OS specific code for the frame buffer layer.
*   The threads needed will be started here and the display section will also be set.
*/


	//windows variables for creating a window simulator
EmbeddedBitMapFrameBuffer *pself;
EmbeddedBitMapFrameBuffer::EmbeddedBitMapFrameBuffer(){

	pself = this;
//	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)timerThread,this,NULL,&timeoutManagerThreadId);
	//END OS_SPECIFIC CODE
}

/**
*	Destroys os specific threads and screen displaying mechanism.
*/
EmbeddedBitMapFrameBuffer::~EmbeddedBitMapFrameBuffer(){
}

WEBC_UINT8 *EmbeddedBitMapFrameBuffer::GetBaseAddress(WEBC_INT whichBuffer)
{
	return ((WEBC_UINT8 *)  EmbeddedBitmapVideoBaseAddr(whichBuffer));
}

WEBC_INT EmbeddedBitMapFrameBuffer::GetNumberOfBuffers()
{
	return EmbeddedBitMapGetNumberOfBuffers();
}

WEBC_INT EmbeddedBitMapFrameBuffer::GetWidth()
{
	return EmbeddedBitMapGetWidth();
}

WEBC_INT EmbeddedBitMapFrameBuffer::GetHeight()
{
	return EmbeddedBitMapGetHeight();
}

WEBC_INT EmbeddedBitMapFrameBuffer::GetDepth()
{
	return EmbeddedBitMapGetDepth();
}

WEBC_INT EmbeddedBitMapFrameBuffer::GetRowBytes()
{
	return EmbeddedBitMapGetRowBytes();
}

void EmbeddedBitMapFrameBuffer::SwitchToBuffer(WEBC_INT whichBuffer)
{
	m_displayBuffer = whichBuffer;
	EmbeddedBitmapSwitchToBuffer(whichBuffer);
}


/**
*	This method will take the internal display that WebC has been working with
*   and transform it to an understandable format for the target device.
*
*	Params: disp - internal webc bitmap for screen display, this bitmap represents what must be
*				   displayed to the screen device
*/
void EmbeddedBitMapFrameBuffer::DoDisplay()
{
}
#include "rtpprint.h"
// Alert box for Javascript.. (badly in need of rework)
extern "C" void webc_user_alert_box(char *str)
{
	rtp_printf(str);
}


#endif //  WIN32
