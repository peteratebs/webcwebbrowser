#ifndef WINDOWSGDI		/* So we can build GDI and Framebuffer from the same project */
#include "webc.h"
#include "webcmem.h"
#include "FBConfig.h"
#include "FBAbstraction.h"
#include "rtptime.h"
#include "rtpprint.h"

void wcProcessTimedCallbacks(void);
HTMLEventStatus call_BrowserDispatchEvent (HBROWSER_HANDLE hbrowser,	HTMLEvent* event);

static HBROWSER_HANDLE _FindBrowserByTarget(void *target);

#if (INCLUDE_WINDOW_MANAGER == 1)
void * currentFocusedWindow;//current window to which events are sent
void * currentActivatedWindow;//current window which is active
int mouseMoveMessageExists;//eliminates duplicate mouse movements
#endif

//
//
// Event to webC input handoff processing.
//
// 	mouseCB	- To be called by the mouse driver to queue an event
//
// void mouseCB(int buttonwithevent, int buttonupdownmask, int x, int y)
//   int buttonwithevent  0x1,0x2,0x3 == left right, middle. webC only uses left, right is handy for diagnostics
//   int buttonupdownmask
//    (buttonwithevent&0x1) &&  (buttonupdownmask&0x1) means left button down
//    (buttonwithevent&0x1) &&  ((buttonupdownmask&0x1)==0) means left button up
//    (buttonwithevent&0x2) &&  (buttonupdownmask&0x2) means left button down
//    (buttonwithevent&0x2) &&  ((buttonupdownmask&0x2)==0) means left button up
//    (buttonwithevent==0) means move event.
//   int x,y	X Y coordinates of the mouse.
//
//
// keyboardCB - To be called by the keyboard driver to queue a press or release event
//
// void keyboardCB(unsigned short keyCode,int isKeyPress)
//
// 	unsigned short keyCode - Keyboard scan code
//  int isKeyPress - 1 if pressed 0 if released
//
//
// PeriodicTimerCB - To be called by the timer thread or timer to queue a time processing
// 	void PeriodiCTimerCB(unsigned long current_tick_count)
//
//
// webCPollingLoopCB - To be called by the main thread to force processing of events queued to webC
//
//
//



// TODO -
//
// Need to implement FBWM_XyToWindow(x,y);
// Need to clean up usage of currentActivatedWindow and currentFocusedWindow and mouseMoveMessageExists
//
//
//
//
//
//
//

static void QueCopyEventForWebCFromIsr(HTMLEvent *pe);
static HTMLEvent *deQueEventForWebCPollingLoop(void);



//used for key strokes
static int giShift = 0;
static int giCaps = 0;
static int giCtrl = 0;
static int giAlt = 0;
static int giEscapeDown = 0;


#if (INCLUDE_WINDOW_MANAGER == 1)
extern void * currentFocusedWindow;//current window to which events are sent
extern void * currentActivatedWindow;//current window which is active
extern int mouseMoveMessageExists;//eliminates duplicate mouse movements
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

/* Browser timout handller, called from an external timer handle */
extern "C" { void doFbBrowserTimeOut(void); }


// Beging callback processing for clock, keyboard and timer


// 	mouseCB	- To be called by the mouse driver to queue an event
//
// void mouseCB(int buttonwithevent, int buttonupdownmask, int x, int y)
//   int buttonwithevent  0x1,0x2,0x3 == left right, middle. webC only uses left, right is handy for diagnostics
//   int buttonupdownmask
//    (buttonwithevent&0x1) &&  (buttonupdownmask&0x1) means left button down
//    (buttonwithevent&0x1) &&  ((buttonupdownmask&0x1)==0) means left button up
//    (buttonwithevent&0x2) &&  (buttonupdownmask&0x2) means left button down
//    (buttonwithevent&0x2) &&  ((buttonupdownmask&0x2)==0) means left button up
//    (buttonwithevent==0) means move event.
//   int x,y	X Y coordinates of the mouse.

void * FBWM_FindWindowXyPos(int*x, int*y);
// Returns the window to which the event should be mapped and translates x, y into window coordinates.
static void * MouseEventToWindow(int *x, int *y)
{

#if (INCLUDE_WINDOW_MANAGER == 0)
	return (void *) &fakeWindowHandle;
#else
	if(currentFocusedWindow){
		return (void *)currentFocusedWindow;
	}
    else{
		// FBWM_XyToWindow(x,y) must return the window we are associated with and move x,y to be at the origin of the window
		return (void *)FBWM_FindWindowXyPos(x,y);
	}
#endif
}

extern "C" void mouseCB(int buttonwithevent, int buttonupdownmask, int x, int y)
{
HTMLEventType mEvent = (HTMLEventType) 0;
int send_to_webc = 0;
void *hWnd=0;

	if ((buttonwithevent&0x1) &&  (buttonupdownmask&0x1))
		mEvent = HTML_EVENT_MOUSEDOWN;
	else if (buttonwithevent&0x1)
		mEvent = HTML_EVENT_MOUSEUP;
	else if (buttonwithevent==0)
		mEvent = HTML_EVENT_MOUSEMOVE;

	if (mEvent)
	{
		hWnd =  MouseEventToWindow(&x, &y);
		if (hWnd)
		{
		HTMLEvent e;
			rtp_memset(&e, 0, sizeof(e));
			e.data.position.x = x;
			e.data.position.y = y;
			e.type = mEvent;
			e.target = (WEBC_UINT8*) hWnd;
			//set modifier flags
			if (giShift)
				e.flags |= HTML_EVENT_FLAG_SHIFT_DOWN;
			if (giCtrl)
				e.flags |= HTML_EVENT_FLAG_CTRL_DOWN;
			if (giAlt)
				e.flags |= HTML_EVENT_FLAG_ALT_DOWN;
			QueCopyEventForWebCFromIsr(&e);				// Queue the event for dispatch when the polling loop runs.
		}
	}
}



// Returns the window to which the event should be mapped and translates x, y into window coordinates.
static void * KeyboardEventToWindow(void)
{

#if (INCLUDE_WINDOW_MANAGER == 0)
	return (void *) &fakeWindowHandle;
#else
	return (void *)currentFocusedWindow;
#endif
}

//
//
// The USB driver will call this function passing up down status and the keyCode
//
// void keyboardCB(int isdown, unsigned short keyCode)
//
WebKey pcKbToWebKey(short keyValue);

extern "C" void keyboardCB(unsigned short keyCode,int isKeyPress)
{

int send_to_webc = 0;
HTMLEvent e;
int mEvent;
int x,y;
void *hWnd=0;
WebKey keyPressed;

	/*
	*  keyValue is the driver value of the key where keyValue must be set to either an ascii value from the keyBoard
	*  i.e. 'a' or 'A' or keyValue is one of the values defined in keyData i.e. WGK_TAB or WGK_SHIFT
	*  The assignment of keyValue must be changed to extract the keypressed from the drivers sent value
	*/
	hWnd = KeyboardEventToWindow();
	keyPressed = pcKbToWebKey(keyCode);
	rtp_memset(&e, 0, sizeof(e));

	e.target = (WEBC_UINT8*) hWnd;
	e.data.key = keyPressed;

	if (isKeyPress)
	{
		e.type = HTML_EVENT_KEYDOWN;
		//set modifier variables
		if(keyPressed == WGK_SHIFT)
			giShift = 1;
		else if(keyPressed == WGK_CTRL)
			giCtrl = 1;
		else if(keyPressed == WGK_CAPS)
			giCaps = ~giCaps;
		else if(keyPressed == WGK_ALT)
			giAlt = 1;
		else if(keyPressed == WGK_ESC)
			giEscapeDown = 1;
		//set modifier flags
		if (giShift)
			e.flags |= HTML_EVENT_FLAG_SHIFT_DOWN;
		if (giCtrl)
			e.flags |= HTML_EVENT_FLAG_CTRL_DOWN;
		if (giAlt)
			e.flags |= HTML_EVENT_FLAG_ALT_DOWN;
	}
	else
	{
		e.type = HTML_EVENT_KEYUP;
		if(keyPressed == WGK_SHIFT)
			giShift = 0;
		else if(keyPressed == WGK_CTRL)
			giCtrl = 0;
		else if(keyPressed == WGK_ALT)
			giAlt = 0;
		else if(keyPressed == WGK_ESC)
			giEscapeDown = 0;
	}
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
// PeriodicTimerCB - Should be called once every hundred milliseconds, but longer duty cycles will be okay.
extern "C" void PeriodicTimerCB(void)
{
HTMLEvent e;
unsigned long current_tick_count;
	current_tick_count = rtp_get_system_msec();
	if ((current_tick_count - last_timer_expiration) >= WEBCTIMERFREQ)
	{
		e.type = HTML_EVENT_TIMER;
		last_timer_expiration=current_tick_count;
		QueCopyEventForWebCFromIsr(&e);
	}
}

// End callback processing for clock, keyboard and timer

//
// yieldCB	- Called when webC is goes idle waiting for data from sockets. Should yield or poll the network layer
// void yieldCB(void)

void yieldCB(void)
{
  return;
}

//
// webCPollingLoopCB - To be called by the main thread to force processing of events queued to webC
//
//
//
#define MAXWEBCMESSAGESPERLOOP 32
extern "C" int webcinitializationcomplete;
extern "C" void webCPollingLoopCB(void)
{
HTMLEvent *e=0;
int loopcount = 0;
int r;
	do {
		e = deQueEventForWebCPollingLoop();
		if (webcinitializationcomplete && e)
		{
			void *target;
			HBROWSER_HANDLE which_Browser;
			if (e->type == HTML_EVENT_TIMER)
			{
				doFbBrowserTimeOut();
				continue;
			}
			target = (void *)e->target;
			which_Browser = _FindBrowserByTarget(target);
			if (!which_Browser)
			{
				rtp_printf("Problem here folks !!!\n");
				continue;
			}
			webc_BrowserSuspendDraw(which_Browser);
			r = webc_BrowserDispatchEvent(which_Browser, e);
			which_Browser = _FindBrowserByTarget(target);		/* Search for the browser again in case it was destroyed by the event handler */
			if (which_Browser)
			{
				if (webc_BrowserGetDrawState (which_Browser) > 0)
					webc_BrowserResumeDraw(which_Browser);
			}
			webc_BrowserSuspendDraw(which_Browser); // new
			webc_BrowserProcessLoadQueueAll(which_Browser);
			if (webc_BrowserGetDrawState (which_Browser) > 0) // new
	    		webc_BrowserResumeDraw(which_Browser);
			which_Browser = _FindBrowserByTarget(target);
			if (which_Browser)
				webc_BrowserDraw(which_Browser);
		}
	} while (e && loopcount++ < MAXWEBCMESSAGESPERLOOP);

}


struct s_HTMLEventContainer
{
	HTMLEvent e;
	struct s_HTMLEventContainer *pnext;
};
typedef struct s_HTMLEventContainer HTMLEventContainer;

static HTMLEventContainer *pIsrEvents;
static HTMLEventContainer *pIsrEventsEnd;;
static int nIsrEventsOutstanding;
static int maxIsrEventsOutstanding;
static HTMLEventContainer *pIsrEventsHeap;
#define MAXISREVENTSOUTSTANDING 512


HTMLEventContainer eventHeap[MAXISREVENTSOUTSTANDING];
#define ATOMIC
#define ENDATOMIC

static void InitEventHeap(void)
{
int i;
	pIsrEventsHeap=&eventHeap[0];
	for (i = 1; i < MAXISREVENTSOUTSTANDING; i++)
	{
		pIsrEventsHeap->pnext=&eventHeap[i];
		pIsrEventsHeap=pIsrEventsHeap->  pnext;
	}
	eventHeap[MAXISREVENTSOUTSTANDING-1].pnext=0;
	pIsrEventsHeap=&eventHeap[0];
}

static HTMLEvent *deQueEventForWebCPollingLoop(void)
{
HTMLEventContainer *e;
int loopcount = 0;
	e=0;
ATOMIC
	if (!pIsrEventsHeap)
		InitEventHeap();
ENDATOMIC

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
		e=0;
ENDATOMIC
	if (e)
		return &e->e;
	else
		return 0;
}


static void QueCopyEventForWebCFromIsr(HTMLEvent *pe)
{
ATOMIC
	if (!pIsrEventsHeap)
		InitEventHeap();
	if (nIsrEventsOutstanding < MAXISREVENTSOUTSTANDING)
	{
		HTMLEventContainer *e;
		nIsrEventsOutstanding += 1;
		if (nIsrEventsOutstanding > maxIsrEventsOutstanding)
			maxIsrEventsOutstanding=nIsrEventsOutstanding;
		e=pIsrEventsHeap;
		if (e)
		{
			pIsrEventsHeap=e->pnext;	// Unlink from the heap
			e->e=*pe;					// Copy the input message onto our copy
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
}


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



#define AT_KEYTABLE_LEN 0x59


/**
*	Translates an unreadable key into a readable one via the mappings created here in portfb.cpp
*
*	Params: rawKey - the unreadable key
*	Return: WebKey - the readable key
*/

//this is platform specific
#define WG_FB_NUM_KEYS 32//define for keys

//assignes key strokes to numbers, this is platform specific
static WEBC_UINT32 keyData [WG_FB_NUM_KEYS*2] =
{
	WGK_TAB         , 0x09 ,
	WGK_CR          , 0x0d ,
	WGK_PGUP        , 604  ,
	WGK_PGDN        , 605  ,
	WGK_LNUP        , 606  ,
	WGK_LNDN        , 607  ,
	WGK_ALT         , 610  ,
	WGK_CTRL        , 611  ,
	WGK_SHIFT       , 612  ,
	WGK_CAPS        , 613  ,
	WGK_NUMLOCK     , 614  ,
	WGK_SCROLLLOCK  , 615  ,
	WGK_F1          , 650  ,
	WGK_F2          , 651  ,
	WGK_F3          , 652  ,
	WGK_F4          , 653  ,
	WGK_F5          , 654  ,
	WGK_F6          , 655  ,
	WGK_F7          , 656  ,
	WGK_F8          , 657  ,
	WGK_F9          , 658  ,
	WGK_F10         , 659  ,
	WGK_F11         , 660  ,
	WGK_F12         , 661  ,
	WGK_INSERT      , 601  ,
	WGK_DELETE      , 600  ,
	WGK_BACKSPACE   , 0x08 ,
	WGK_LEFT        , 608  ,
	WGK_RIGHT       , 609  ,
	WGK_HOME        , 602  ,
	WGK_END         , 603  ,
	WGK_ESC         , 0x1b
};


//key map variables
static WebcKeyMap keyMap;
static WEBC_BOOL  keyMapInitialized = WEBC_FALSE;

// each key in non-shifted and shifted state
static int keyboardCharacters[AT_KEYTABLE_LEN][2] = {
	{0x00, 0x00},
	{0x1b, 0x1b},
	{'1', '!'},
	{'2', '@'},
	{'3', '#'},
	{'4', '$'},
	{'5', '%'},
	{'6', '^'},
	{'7', '&'},
	{'8', '*'},
	{'9', '('},
	{'0', ')'},
	{'-', '_'},
	{'=', '+'},
	{0x08, 0x08},
	{0x09, 0x09},
	{'q', 'Q'},
	{'w', 'W'},
	{'e', 'E'},
	{'r', 'R'},
	{'t', 'T'},
	{'y', 'Y'},
	{'u', 'U'},
	{'i', 'I'},
	{'o', 'O'},
	{'p', 'P'},
	{'[', '{'},
	{']', '}'},
	{0x0d, 0x0d},
	{611, 611},
	{'a', 'A'},
	{'s', 'S'},
	{'d', 'D'},
	{'f', 'F'},
	{'g', 'G'},
	{'h', 'H'},
	{'j', 'J'},
	{'k', 'K'},
	{'l', 'L'},
	{';', ':'},
	{'\'','\"'},
	{'`', '`'},
	{612, 612},
	{'\\','|'},
	{'z', 'Z'},
	{'x', 'X'},
	{'c', 'C'},
	{'v', 'V'},
	{'b', 'B'},
	{'n', 'N'},
	{'m', 'M'},
	{',', '<'},
	{'.', '>'},
	{'/', '?'},
	{612, 612},
	{'*', '*'},
	{610, 610},
	{' ', ' '},
	{613, 613},
	{650, 650},
	{651, 651},
	{652, 652},
	{653, 653},
	{654, 654},
	{655, 655},
	{656, 656},
	{657, 657},
	{658, 658},
	{659, 659},
	{614, 614},
	{615, 615},
	{602, 602},
	{606, 606},
	{604, 604},
	{'-', '_'},
	{608, 608},
	{0, 0},
	{609, 609},
	{'+', '+'},
	{603,603},
	{607, 607},
	{605, 605},
	{601, 601},
	{600, 600},
	{0, 0},
	{0, 0},
	{0, 0},
	{660,660},
	{661,661}
};


static WebKey webc_TranslateFBKey(WEBC_UINT32 rawKey)
{
	if (!keyMapInitialized)
	{
		webc_CreateKeyMap(&keyMap, keyData, WG_FB_NUM_KEYS);
		keyMapInitialized = WEBC_TRUE;
	}

	return webc_TranslateKey(&keyMap, rawKey);
}
WebKey pcKbToWebKey(short keyValue)
{
	short oldkeyValue=keyValue;
		keyValue = keyboardCharacters[keyValue][(giShift || giCaps)? 1 : 0]; //turn keyValue into ascii or keyData value
//			keyValue = HIWORD(lParam) & 0xff;
//			keyValue = keyboardCharacters[keyValue][(giShift || giCaps)? 1 : 0]; //turn keyValue into ascii or keyData value
//			keyboard_event(KEY_DOWN, webc_TranslateFBKey(keyValue), e);

		return(webc_TranslateFBKey(keyValue));
}
#endif // WINDOWSGDI
