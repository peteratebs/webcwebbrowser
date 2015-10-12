#ifndef WIN32

#include "EmbeddedBitMapFrameBuffer.h"
#include "webcmem.h"
#include "assert.h"
#include "FBConfig.h"
#include "Remoteterminal.h"
#include "FBAbstraction.h"

// Conmfigure parameters here
#define NUMBER_OF_BUFFERS 2
#define COLOR_DEPTH	4
#define BYTES_PER_ROW (WEBC_SCREEN_WIDTH*COLOR_DEPTH)



WEBC_UINT8 *EmbeddedBitmapVideoBaseAddr(WEBC_INT whichBuffer)
{
	return ((WEBC_UINT8 *)  getVideoBaseAddr(whichBuffer));
}

WEBC_INT EmbeddedBitMapGetNumberOfBuffers()
{
	return 2;
}

WEBC_INT EmbeddedBitMapGetWidth()
{
	return WEBC_SCREEN_WIDTH;
}

WEBC_INT EmbeddedBitMapGetHeight()
{
	return WEBC_SCREEN_HEIGHT;
}

WEBC_INT EmbeddedBitMapGetDepth()
{
	return COLOR_DEPTH;
}

WEBC_INT EmbeddedBitMapGetRowBytes()
{
	return BYTES_PER_ROW;
}

void EmbeddedBitMapFrameBuffer::SwitchToBuffer(WEBC_INT whichBuffer)
{
	m_displayBuffer = whichBuffer;
	LCD_UPBASE = (unsigned long) GetBaseAddress(whichBuffer) & ~7UL ;
	LCD_LPBASE = (unsigned long) GetBaseAddress(whichBuffer) & ~7UL ;
}


void wcProcessTimedCallbacks(void);

HTMLEventStatus call_BrowserDispatchEvent (HBROWSER_HANDLE hbrowser,	HTMLEvent* event);

int threadExit;  //when the window manager is shutdown set this to true to stop the threads

EmbeddedBitMapFrameBuffer * EmbeddedBitMapFrameBuffer::m_only=WEBC_NULL;

FBHWInterface *EmbeddedBitMapFrameBuffer::Only()
{
	if(!m_only)
		m_only = new EmbeddedBitMapFrameBuffer();

	return m_only;
}
// Resizes a window so the client area contains the specified size.
static void SetClientSize( HWND hwnd, int clientWidth, int clientHeight);


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
	return EmbeddedBitmapGetNumberOfBuffers();
}

WEBC_INT EmbeddedBitMapFrameBuffer::GetWidth()
{
	return EmbeddedBitmapGetWidth();
}

WEBC_INT EmbeddedBitMapFrameBuffer::GetHeight()
{
	return EmbeddedBitmapGetHeight();
}

WEBC_INT EmbeddedBitMapFrameBuffer::GetDepth()
{
	return EmbeddedBitmapGetDepth();
}

WEBC_INT EmbeddedBitMapFrameBuffer::GetRowBytes()
{
	return EmbeddedBitmapGetRowBytes();
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

// Alert box for Javascript.. (badly in need of rework)
extern "C" void webc_user_alert_box(char *str)
{
	rtp_printf(str);
}

#define IDM_EXIT				105

//Following variables are used for the mouse/keyboard messages to be sent to WebC

#define KEY_DOWN     0
#define KEY_UP       1
#define RBUTTON_UP   2
#define LBUTTON_UP   3
#define RBUTTON_DOWN 4
#define LBUTTON_DOWN 5
#define MOUSE_MOVE   6


//used for key strokes
static int giShift = 0;
static int giCaps = 0;
static int giCtrl = 0;
static int giAlt = 0;
static int giEscapeDown = 0;



#if (INCLUDE_WINDOW_MANAGER == 1)
extern void * currentActivatedWindow;//current window which is active
#endif


/**
*	This method's job is to receive messages from a driver and queue them up.
*	If, at the end of the method, a message does not have a destination or has no specific type it is
*   discarded.  This method should be called from the main messaging loop.
*
*	To change this method to your specific target it must be made to receive messages from keyboard and mouse drivers.
*
*
*
*	Params: OS specific, whatever is required by thread mechanism/drivers
*	Return: OS specific, whatever is required by thread mechanism/drivers
*/
int mouse_button_event(HWND hWnd, int type, DISPLAY_INT x, DISPLAY_INT y, HTMLEvent* e);
int keyboard_event(HWND hWnd, int type, WebKey keyPressed, HTMLEvent* e);
int mouse_move_event(HWND hWnd, DISPLAY_INT x, DISPLAY_INT y, HTMLEvent* e);
WebKey webc_TranslateFBKey(WEBC_UINT32 rawKey);
WebKey pcKbToWebKey(short keyValue);

/* Browser timout handller, called from an external timer handle */
extern "C" { void doFbBrowserTimeOut(unsigned long lBrowser); }

static void QueEventForWebCFromIsr(HTMLEvent *e)


#define WM_LBUTTON 0x1
#define WM_RBUTTON 0x2
int ButtonMap;

// New code


/* Fix these for export from USB driver */
//
// #define RBUTTON_UP    //change this case to be the mouse drivers right button up
// #define RBUTTON_DOWN  //change this case to be the mouse drivers right button up
// #define LBUTTON_DOWN  //change this case to be the mouse drivers left button down
// #define LBUTTON_UP    //change this case to be the mouse drivers left button up
//
// The USB driver will call this function passing the event type and x,y coordinates
// void mouseCB(int mEvent, int x, int y)
//

void mouseCB(int mEvent, int x, int y)
{
int mEvent;
int x,y;
int send_to_webc = 0;
HTMLEvent e;
	mEvent = DecodeMouseEvent(mouseargs,&x,&y);
	switch(mEvent){
		case WM_RBUTTONDOWN: //change this case to be mouse drivers right button down
#if (MAX_VIRTUAL_DISPLAYS > 0)
#endif
		break;
		case RBUTTON_UP: //change this case to be the mouse drivers right button up
			break;
		case RBUTTON_DOWN: //change this case to be the mouse drivers right button up
			break;
		case LBUTTON_DOWN: //change this case to be the mouse drivers left button down
			send_to_webc = mouse_button_event(hWnd, LBUTTON_DOWN,x,y,&e);
			break;
		case LBUTTON_UP: //change this case to be the mouse drivers left button up
			send_to_webc = mouse_button_event(hWnd, LBUTTON_UP,x,y,&e);
			break;
		default: //Must be a move event
			send_to_webc = mouse_move_event(hWnd,x,y,&e);
			break;
		default:
			break;
		}
	}
	if (send_to_webc)
		QueCopyEventForWebCFromIsr(&e);
}
//
//
// The USB driver will call this function passing up down status and the keyCode
//
// void keyboardCB(int isdown, unsigned short keyCode)
//

void keyboardCB(int isdown, unsigned short keyCode)
{
int send_to_webc = 0;
HTMLEvent e;
int mEvent;
int x,y;
	/*
	*  keyValue is the driver value of the key where keyValue must be set to either an ascii value from the keyBoard
	*  i.e. 'a' or 'A' or keyValue is one of the values defined in keyData i.e. WGK_TAB or WGK_SHIFT
	*  The assignment of keyValue must be changed to extract the keypressed from the drivers sent value
	*/
	if (isdown)
		send_to_webc = keyboard_event(hWnd,KEY_DOWN, pcKbToWebKey(keyCode), &e);
	else
		send_to_webc = keyboard_event(hWnd,KEY_UP, pcKbToWebKey(keyCode), &e);
	break;
	if (send_to_webc)
		QueCopyEventForWebCFromIsr(&e);
}

//
//
// The Clock tick ISR call this function ideally every one hundred miliseconds
//
//
// void PeriodiTimerCB(unsigned long current_tick_count)
//

#define WEBCTIMERFREQ 100
unsigned long last_timer_expiration;
void PeriodiTimerCB(unsigned long current_tick_count)
{
HTMLEvent e;
	if ((current_tick_count - last_timer_expiration) >= WEBCTIMERFREQ)
	{
		e.type = HTML_EVENT_TIMER;
		last_timer_expiration=rtp_get_msecs();
		QueCopyEventForWebCFromIsr(&e);
	}
}

static HTMLEvent *pIsrEvents;
static HTMLEvent *pIsrEventsEnd;;
static int nIsrEventsOutstanding;
static int maxIsrEventsOutstanding;
static HTMLEvent *pIsrEventsHeap;
#define MAXISREVENTSOUTSTANDING 512
#define MAXWEBCMESSAGESPERLOOP 64
HTMLEvent eventHeap[MAXISREVENTSOUTSTANDING];
#define ATOMIC
#define ENDATOMIC

static void InitEventHeap(void)
{
int i;
	pIsrEventsHeap=&eventHeap[0];
	for (i = 1; i < MAXISREVENTSOUTSTANDING; i++)
	{
		pIsrEventsHeap->pnext=&eventHeap[i];
		pIsrEventsHeap=pIsrEventsHeap->pnext;
	}
	eventHeap[MAXISREVENTSOUTSTANDING-1].pnext=0;
	pIsrEventsHeap=&eventHeap[0];
}

static void EmbeddedBitMapMessageDispatch(HTMLEvent *pe);
HTMLEvent *deQueEventForWebCPollingLoop(void)
{
HTMLEvent *e
	e=0;
	if (!pIsrEventsHeap)
		InitEventHeap();
	do
	{
ATOMIC
		if (pIsrEvents)
		{
			nIsrEventsOutstanding -= 1;
			e= pIsrEvents;
			pIsrEvents=pIsrEvents->pnext;
			if (!pIsrEvents)
				pIsrEventsEnd=0;
		}
		else
		{
			e=0;
		}
ENDATOMIC
		if (e)
		{
			EmbeddedBitMapMessageDispatch(&e);
		}
		if (e)
		{
		ATOMIC
			e->pnext = pIsrEventsHeap;
			pIsrEventsHeap = e;
		ENDATOMIC
		}
	} while (e && loopcount++ < MAXWEBCMESSAGESPERLOOP);

)


static void QueCopyEventForWebCFromIsr(HTMLEvent *pe)
{
ATOMIC
	if (!pIsrEventsHeap)
		InitEventHeap();
	if (nIsrEventsOutstanding < MAXISREVENTSOUTSTANDING)
	{
		nIsrEventsOutstanding += 1;
		if (nIsrEventsOutstanding > maxIsrEventsOutstanding)
			maxIsrEventsOutstanding=nIsrEventsOutstanding;
		e=pIsrEventsHeap;
		if (e)
		{
			pIsrEventsHeap=e->pnext;	// Unlink from the heap
			*e=*pe;						// Copy the input message onto our copy
			e->pnext = 0;				// Put it on the tail of the input queue
			if (pIsrEventsEnd)
			{
				pIsrEventsEnd->pnext=e;
			}
			else
			{
				pIsrEvents=e;
				pIsrEventsEnd=e;
			}
		}
	}
ENDATOMIC
)

// End New code

#if (INCLUDE_WINDOW_MANAGER)
	void* windowHndl = FBWM_createWindow(ctx,pconfig);
	currentActivatedWindow = windowHndl;
#else
#ifdef WIN32
	WEBC_UINT8 *windowHndl = (WEBC_UINT8 *) ((WindowsBitMapFrameBuffer *)GET_FBGRAPHICS_CTX(ctx)->frameBufferItf)->hWnd;
#else
	WEBC_UINT8 *windowHndl = (WEBC_UINT8 *) &fakeWindowHandle;  // createWindow(x, y, width, height, hWndParent, windowName);
#endif
#endif



static HBROWSER_HANDLE _FindBrowserByWindow(void *hWnd)
{
#if (INCLUDE_WINDOW_MANAGER == 1)
	return FBWM_FindBrowserByWindow(currentActivatedWindow);
#else
#if (MAX_VIRTUAL_DISPLAYS > 0)
	return(VirtualDisplayGetCurrentBrowser());
#else
	return(webc_FindBrowserByWindow(hWnd));
#endif
#endif
}


static HBROWSER_HANDLE _FindBrowserByTarget(void *target)
{
#if (INCLUDE_WINDOW_MANAGER == 1)
	return FBWM_FindBrowserByWindow(target);
#else
	return _FindBrowserByWindow(target);
#endif
}

static void EmbeddedBitMapMessageDispatch(HTMLEvent *pe)
{
	int send_to_webc = 0;
	int wmId; //variable for windows exit function
	void *hWnd;
	HTMLEvent e;
	e = *pe;
	e.target = NULL;
	e.type = HTML_EVENT_NONE;
	e.flags = 0;

	hWnd = (void *) &fakeWindowHandle; // Only used when no window manager is present
	DISPLAY_INT x,y;
//	short keyValue;


	switch(message){
		case RBUTTONDOWN: //change this case to be mouse drivers right button down
#if (MAX_VIRTUAL_DISPLAYS > 0)
			VirtualDisplayRotateBrowserNumber();
			UpdateWindow(hWnd);
#endif
			break;
		case LBUTTONDOWN: //change this case to be the mouse drivers left button down
			send_to_webc = 1;
			break;
		case LBUTTONUP: //change this case to be the mouse drivers left button up
			send_to_webc = 1;
			break;
		case RBUTTONUP: //change this case to be the mouse drivers right button up
			//x = LOWORD(lParam); //change to extract the x position related with event
			//y = HIWORD(lParam); //change to extract the y position related with event
			//send_to_webc = mouse_button_event(hWnd, RBUTTON_UP,x,y,&e);
			break;
		case MOUSEMOVE: //change this case to be the enumeration for when driver detects the mouse moves
			send_to_webc = 1;
			break;
		case KEYDOWN://change this case to be the keyboard drivers key down
			send_to_webc = 1;
		case KEYUP: //change this case to be the keyboard drivers key up
			send_to_webc = 1;
		case HTML_EVENT_TIMER:
#if (USE_EXPERIMENTAL_TIMER_EVENT)
			send_to_webc = 1;
//			e.type = HTML_EVENT_TIMER;
#else
			doFbBrowserTimeOut((unsigned long )which_Browser);
			// webc_BrowserProcessTimers(which_Browser);
			// webc_BrowserDraw(which_Browser);
			// webc_BrowserProcessLoadQueueAll(which_Browser);
			// webc_BrowserDraw(which_Browser);
#endif
			break;
		default:
			send_to_webc = 0;
			break;
	}

	if (send_to_webc)
	{
	void *target;
	HBROWSER_HANDLE which_Browser;
	char pastebuffer[1024];
	HTMLEventStatus r;
	pastebuffer[0] = 0;

//		webc_BrowserDispatchEvent(_FindBrowserByWindow ((void *) hWnd), e);
		// ====
		/* Search for the browser again in case it was destroyed */
#if (INCLUDE_WINDOW_MANAGER == 1)
		target = (void *)e->target;
//		which_Browser  = FBWM_FindBrowserByWindow((void *)e.target);
#else
		target = (void *)hWnd;
//		which_Browser  = _FindBrowserByWindow((void *)hWnd);
#endif
		which_Browser = _FindBrowserByTarget(target);
		if (!which_Browser)
		{
			rtp_printf("Problem here folks !!!\n");
			return;
		}
		{
			webc_BrowserSuspendDraw(which_Browser);
			r = call_BrowserDispatchEvent(which_Browser, &e);
			/* Search for the browser again in case it was destroyed by the event handler */
			which_Browser = _FindBrowserByTarget(target);
			if (!which_Browser)
	        	return;
		    if (webc_BrowserGetDrawState (which_Browser) > 0)
		    	webc_BrowserResumeDraw(which_Browser);
//			which_Browser = _FindBrowserByTarget(target);
//		    if (!which_Browser)
//	        	return 0;
//			which_Browser = _FindBrowserByTarget(target);
//		    if (!which_Browser)
//	        	return 0;
			webc_BrowserSuspendDraw(which_Browser); // new
		    webc_BrowserProcessLoadQueueAll(which_Browser);
		    if (webc_BrowserGetDrawState (which_Browser) > 0) // new
		    	webc_BrowserResumeDraw(which_Browser);
			which_Browser = _FindBrowserByTarget(target);
		    if (!which_Browser)
	        	return;
		    webc_BrowserDraw(which_Browser);
		}
}

extern int giAllowTimerTick;  // Not used but perpetuating a bad habit

int WindowsFB_ProcessInputQueue(
	void		 *fn,
	WEBC_BOOL  block,
	WEBC_INT32 *exitCode
	)
{
	MSG msg;
	if (block || PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE|PM_NOYIELD))
	{
		int status = GetMessage(&msg, NULL, 0, 0);
		if (status)
		{
			TranslateMessage(&msg);
			giAllowTimerTick++;
			DispatchMessage(&msg);
			giAllowTimerTick--;
		}
		else
		{
			return (-1);
		}
	}
	return 0;
}
#endif //  WIN32
