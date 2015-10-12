//
// webcdemo_gdip.cpp - WebC Browser demo on Win32/GDI+
//
// EBSnet - WebC
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//
#include "vxWorks.h"
#include "taskLib.h"
#include "webc.h"
#include "webcwndml.h"
#include "webcdemo.hpp"
#include "webc_ctrl_demo.h"



/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define MAX_LOADSTRING 100
#define AT_KEYTABLE_LEN 0x59

/*****************************************************************************/
// Local Types
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/

int giShift = 0;
int giCaps = 0;
int giCtrl = 0;
int giAlt = 0;
extern "C" int giEscapeDown;

// each key in non-shifted and shifted state
int _scanCodeToChar[AT_KEYTABLE_LEN][2] = {
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

HBROWSER_HANDLE gBrowser = 0;
int gBrowser_in_dispatch = 0;

// Forward declarations of functions included in this code module:
bool InitInstance(void);
int WebcDemoEventLoopHandler( UGL_MSG uglMessage);

// Prototype for ebc entry point
int WebMain(void);
int WebcTimer(void);
#define PRIO_HIGH 102 /* high priority */



void webc_vxw_entry(void)
{
// spawn the  tasks
int iWebcTaskid;

if((iWebcTaskid = taskSpawn("webctask",PRIO_HIGH,VX_FP_TASK,80000,(FUNCPTR)WebMain,0,0,0,0,0,0,0,
	0,0,0)) == ERROR)
	printf("\nTaskSpawn failed. FUNCTION: %s,  FILE: %s, LINE: %d   \n",__FUNCTION__,__FILE__,__LINE__);

}
HBROWSER_HANDLE webc_start_chrome_browser(HTMLBrowserConfig *pconfig);
int WebcTimer(void)
{
	for(;;)
	{
		/* Process timers if we are not currently dispatching a message */
		if (gBrowser && !gBrowser_in_dispatch)
		{
			webc_BrowserProcessTimers(gBrowser);
			webc_BrowserDraw(gBrowser);
			webc_BrowserProcessLoadQueueAll(gBrowser);
			webc_BrowserDraw(gBrowser);
		}
		taskDelay(sysClkRateGet()/100);
	}
}
HBROWSER_HANDLE webc_start_webc_demo(HTMLBrowserConfig *pconfig);

int WebMain(void)
{

	HTMLBrowserConfig config;
	HTMLEvent e;
	int iWebcTimerId;

	// WebC library initialization
	webc_Init();

	webc_InitWindMLBrowserConfig(&config);

	if((iWebcTimerId = taskSpawn("webctimer",PRIO_HIGH,VX_FP_TASK,80000,(FUNCPTR)WebcTimer,0,0,0,0,0,0,0,
		0,0,0)) == ERROR)
		printf("\nTaskSpawn failed. FUNCTION: %s,  FILE: %s, LINE: %d   \n",__FUNCTION__,__FILE__,__LINE__);

	gBrowser = webc_start_webc_demo(&config);
//    gBrowser = webc_start_chrome_browser(&config);
	/*
	#if(WEBC_CFG_ENABLE_CHROME)
		gBrowser = webc_start_chrome_browser(&config);
	#else
		gBrowser = webc_start_webc_demo(&config);
	#endif
*/
	gBrowser_in_dispatch = 0;
	while(1)
		{
			WebcDemoEventLoopHandler(webc_GetWindMLMessages(&config));
		}
}

bool InitInstance(void)
{
	HTMLBrowserConfig config;
	if (webc_InitWindMLBrowserConfig(&config) >= 0)
	{
		config.browserEventHandler = chromeBrowserEventHandler;
		config.privateData = 0;

		gBrowser = webc_CreateBrowser(&config, "");
//		printf("\nIn FUNCTION: %s,  FILE: %s,  LINE: %d BROWSER created\n", __FUNCTION__,__FILE__,__LINE__);
	}

	return true;
}


int WebcDemoEventLoopHandler( UGL_MSG uglMessage)
{
	int wmId, wmEvent;

	HTMLEvent e;

	gBrowser_in_dispatch += 1;	/* Mark that we are in the dispatcher, tell background timer to skip */

	e.type = HTML_EVENT_NONE;
	e.flags = 0;
	switch (uglMessage.type)
	{
		case MSG_KEYBOARD:
			{
				//printf("\nIn FUNCTION: %s,  FILE: %s,  LINE: %d MSG_KEYBOARD", __FUNCTION__,__FILE__,__LINE__);
				if ( uglMessage.data.keyboard.modifiers & UGL_KBD_KEYDOWN )
				{
					if (uglMessage.data.keyboard.modifiers & UGL_KBD_SHIFT)
					{
						giShift = 1;
					}
					else
					{
						if (uglMessage.data.keyboard.modifiers & UGL_KBD_CTRL)
						{
							giCtrl = 1;
						}
						else
						{
							if (uglMessage.data.keyboard.modifiers & UGL_KBD_CAPS_LOCK)
							{
								giCaps = ~giCaps;
							}
						}
					}


					unsigned short iScan = uglMessage.data.keyboard.rawKbd.value.scanCode;

// printf("Iscan == %x (%d) key == %x \n", iScan, iScan, uglMessage.data.keyboard.key);
//					if (iScan < AT_KEYTABLE_LEN)
					if (iScan <= 0xff)
					{
//						iScan = _scanCodeToChar[iScan][(giShift || giCaps)? 1 : 0];

						if (iScan)
						{
							// this is the scan code for the escape key

							e.type = HTML_EVENT_KEYDOWN;
							e.data.key = webc_TranslateWindMLKey(iScan);

							if (e.data.key == WGK_ESC)
							{
								giEscapeDown = 1;
							}

							if (giShift)
							{
								e.flags |= HTML_EVENT_FLAG_SHIFT_DOWN;
							}

							if (giCtrl)
							{
								e.flags |= HTML_EVENT_FLAG_CTRL_DOWN;
							}

							if (giAlt)
							{
								e.flags |= HTML_EVENT_FLAG_ALT_DOWN;
							}
						}
					}
				}
				else
				{
					if (uglMessage.data.keyboard.modifiers & UGL_KBD_SHIFT)
					{
						giShift = 0;
					}
					else
					{
						if (uglMessage.data.keyboard.modifiers & UGL_KBD_CTRL)
						{
							giCtrl = 0;
						}
					}


					unsigned short iScan = uglMessage.data.keyboard.rawKbd.value.scanCode;

//					if (iScan < AT_KEYTABLE_LEN)
					if (iScan <= 0xff)
					{
	//					iScan = _scanCodeToChar[iScan][(giShift || giCaps)? 1 : 0];

						if (iScan)
						{
							e.type = HTML_EVENT_KEYUP;
							e.data.key = webc_TranslateWindMLKey(iScan);

							// this is the scan code for the escape key
							if (e.data.key == WGK_ESC)
							{
								giEscapeDown = 0;
							}

							if (giShift)
							{
								e.flags |= HTML_EVENT_FLAG_SHIFT_DOWN;
							}

							if (giCtrl)
							{
								e.flags |= HTML_EVENT_FLAG_CTRL_DOWN;
							}

							if (giAlt)
							{
								e.flags |= HTML_EVENT_FLAG_ALT_DOWN;
							}
						}
					}
				}
			}
			break;

		case MSG_POINTER:
			{
				//printf("\nIn FUNCTION: %s,  FILE: %s,  LINE: %d MSG_POINTER", __FUNCTION__,__FILE__,__LINE__);
				e.data.position.x = uglMessage.data.pointer.position.x;
				e.data.position.y =  uglMessage.data.pointer.position.y;

				if ( (uglMessage.data.pointer.delta.x != 0) || (uglMessage.data.pointer.delta.y != 0) )
				{
					e.type = HTML_EVENT_MOUSEMOVE;
				}

				UGL_UINT32 pressedButtons = uglMessage.data.pointer.buttonState & uglMessage.data.pointer.buttonChange;
				UGL_UINT32 releasedButtons = ~uglMessage.data.pointer.buttonState & uglMessage.data.pointer.buttonChange;

				if( pressedButtons )
				{
					e.type = HTML_EVENT_MOUSEDOWN;
				}

				if( releasedButtons )
				{
					e.type = HTML_EVENT_MOUSEUP;
				}
			}
			break;

		default:
			break;
	};

/*
case WM_TIMER:
BeginPaint(hWnd, &ps);
EndPaint(hWnd, &ps);
webc_BrowserProcessTimers(gBrowser);
webc_BrowserDraw(gBrowser);
webc_BrowserProcessLoadQueueAll(gBrowser);
webc_BrowserDraw(gBrowser);
return 0;
*/
//	if (gBrowser)
//	{
//		webc_BrowserInvalidate(gBrowser);
//	}

	if (gBrowser && (e.type != HTML_EVENT_NONE))
	{
		webc_BrowserSuspendDraw(gBrowser);
		webc_BrowserDispatchEvent(gBrowser, &e);
		if (webc_BrowserGetDrawState (gBrowser) > 0)	// New - May 14, 2007
			webc_BrowserResumeDraw(gBrowser);
		webc_BrowserDraw(gBrowser);
		webc_BrowserProcessLoadQueueAll(gBrowser);
		webc_BrowserDraw(gBrowser);
	}
	gBrowser_in_dispatch -= 1;	/* Reduce the dispatch count */
	if (gBrowser_in_dispatch < 0)
	{
		printf("Dispatch count wrong \n");
		gBrowser_in_dispatch = 0;	/* Reduce the dispatch count */
	}

}
