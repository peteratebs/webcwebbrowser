/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// winpeg.cpp - PegTask implementation for running PEG as a Win16 or
//    Win32 application.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2000 Swell Software
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
// This module creates the fundamental PEG objects (PegScreen,
// PegPresentation, PegMessageQueue), calls PegAppInitialize, and then
// PegPresentationManager::Execute(), which runs forever.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

extern "C" int giPegScreenWidth;
extern "C" int giPegScreenHeight;

int giPegScreenWidth = 800;
int giPegScreenHeight = 570;

#define INCLUDE_PEG_KEYTABLE
#include "peg.hpp"
#ifndef WEBC_PEG_190_OR_LATER
// #include "winpeg.h"
#endif // WEBC_PEG_190_OR_LATER

#include "webc.h"
//#include "webcdemo.hpp"
#include "webcassert.h"
#include "resource.h"
#include "pwebwin.hpp"
#include "rtpprint.h"
//#include "w32scrn.hpp"



#if defined(_PEGWINSCREEN_)      // normal Win32 operation?
#define PEGSCREEN PegWinScreen
#elif defined(_L16SCREEN_)       // TESTING LINEAR-16 DRIVER??
#define PEGSCREEN L16Screen
#elif defined(_L8SCREEN_)        // TESTING LINEAR-8 DRIVER??
#define PEGSCREEN L8Screen
#elif defined(_L4SCREEN_)        // TESTING LINEAR-4 DRIVER??
#define PEGSCREEN L4Screen
#elif defined(_L2SCREEN_)        // TESTING LINEAR-2 DRIVER??
#define PEGSCREEN L2Screen
#elif defined(_MONOSCREEN_)      // TESTING MONO DRIVER?
#define PEGSCREEN MonoScreen
#elif defined(_PROMONOSCREEN_)
#define PEGSCREEN ProMonoScreen  // TESTING PROFILE-MODE MONOCHROME?
#elif defined(_PRO2SCREEN_)
#define PEGSCREEN Pro2Screen
#elif defined(_PRO4SCREEN_)
#define PEGSCREEN Pro4Screen
#elif defined(_PRO8SCREEN_)
#define PEGSCREEN Pro8Screen
#elif defined(_SED1356SCRN_)     // Running SDU1356 under Windows?
#define PEGSCREEN SED1356Screen
#else

#error ** Error: no screen driver included **
#endif

#ifdef RTP_TRACK_LOCAL_MEMORY
 #include "rtpmemdb.h"
#endif

char *gpPegCmdLine = NULL;
char windowTitleString[] = "EBS WebC " WEBC_VERSION_STR " Embedded Web Browser Demo (PEG Graphics) - www.ebsembeddedsoftware.com ";
unsigned long lastSizeAlloc = 0;

extern "C" int giEscapeDown;

void PegAppInitialize(PegPresentationManager *);
long FAR WINAPI WndProc(HWND, UINT, UINT, LONG);

extern const char* gpHomePage;

/*---------------------------------------------------------------------------*/
void StartWebcDemo (void)
{
  #ifdef WEBC_DEBUG
	//WEBC_ASSERT(webc_test_WebString());
  #endif // WEBC_DEBUG

//	webc_RegisterEventCallback (chromeLoadHandler, "chromeLoadHandler");
//	webc_define_url_macro("google", GoogleUrlMacro);

    PegRect Rect;
    Rect.Set(0, 0, giPegScreenWidth - 1, giPegScreenHeight - 1);

	  #ifdef RTP_TRACK_LOCAL_MEMORY
		int before = rtp_debug_get_mem_used();
		rtp_debug_print_mem_usage("before.csv", "w+");
	  #endif
	{
		PegWebWindow* webWindow = new PegWebWindow(Rect, WEBC_NULL);

		if (webWindow)
		{
			webWindow->Presentation()->Add(webWindow, TRUE);
#if (0)
			if (webWindow->mBrowser)
			{
				HBROWSER_HANDLE hbrowser = webWindow->mBrowser;

				webc_BrowserSetTagEventHandler (
						hbrowser,
						HTML_TAG_INPUT,
						inputHandler
					);

				webc_BrowserSetTagEventHandler (
						hbrowser,
						HTML_TAG_BODY,
						bodyHandler
					);

			  #if (WEBC_SUPPORT_JSCRIPT)
				HDOC_HANDLE hdoc = webc_BrowserGetDocument(hbrowser);

				if (hdoc)
				{
					webc_JScriptDefineFunction(hdoc, "DebugLog", AppendFile, 2);
					webc_JScriptDefineFunction(hdoc, "getDefaultUrl", GetDefaultUrl, 0);
				}

				webc_BrowserLoadUrl(hbrowser, "webc://loadChrome.html");
			  #else
				webc_BrowserLoadUrl(hbrowser, (char*) gpHomePage);
			  #endif /* WEBC_SUPPORT_JSCRIPT */
			}
#endif
		}
#if (0)
	  #ifdef RTP_TRACK_LOCAL_MEMORY
		webc_FreeTempStore();
		int after = rtp_debug_get_mem_used();
		if (before != after)
		{
			rtp_debug_print_mem_usage("leaks.csv", "a+");
		}
		WEBC_ASSERT((n == 0) || (n == giBrowserLoop-1) || (before == after));
	  #endif
#endif
	}
}

/*---------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
		   		   LPSTR pCmd, int nCmdShow)
{
    static char szAppName[] = "Peg Application";
    HWND hwnd;

    WNDCLASS wndclass;

    // Save any command line parameters. We don't use them, but the
    // application may want them:

    if (pCmd)
    {
        if (*pCmd)
        {
            gpPegCmdLine = new char[strlen(pCmd) + 1];
            strcpy(gpPegCmdLine, pCmd);
        }
    }

    if (!hPrevInstance)
    {
        wndclass.style = CS_HREDRAW|CS_VREDRAW|CS_PARENTDC;
        wndclass.lpfnWndProc = WndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = LoadIcon(hInstance, (LPCTSTR) IDI_ICON1);
        wndclass.hCursor = NULL;
        wndclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = szAppName;
        RegisterClass(&wndclass);
    }

    hwnd = CreateWindow(szAppName, windowTitleString,
         WS_VISIBLE|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU,
        20, 20,
        giPegScreenWidth + 8,
        giPegScreenHeight + 25, NULL, NULL, hInstance, NULL);

    // create the screen interface object:

    PegRect Rect;
    Rect.Set(0, 0, giPegScreenWidth - 1, giPegScreenHeight - 1);

    PegScreen *pScreen = 0;
    pScreen = new PEGSCREEN(hwnd, Rect);

    PegThing::SetScreenPtr(pScreen);

    // create the PEG message Queue:

    PegMessageQueue *pMsgQueue = new PegMessageQueue();
    PegThing::SetMessageQueuePtr(pMsgQueue);

    // create the screen manager:

    PegPresentationManager *pPresentation = new PegPresentationManager(Rect);
    PegThing::SetPresentationManagerPtr(pPresentation);

   #ifdef PEG_FULL_CLIPPING
    pScreen->GenerateViewportList(pPresentation);
   #endif

	webc_Init();

    // re-load the screen, PresentationManager, and MessageQueue pointers
    // in case the application changed them (WindowBuilder does):

    PegThing *pTest = NULL;
    pPresentation = pTest->Presentation();
    pScreen = pTest->Screen();

	pPresentation->Invalidate(pPresentation->mReal);

	SetTimer(hwnd, 1, 50, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	pScreen->SetPointerType(PPT_NORMAL);

	PegRect rect;
	rect.Set(0,0,0,0);
	PegWindow* placeHolder = new PegWindow(rect);
	if (placeHolder)
	{
		pPresentation->Add(placeHolder);
	}

	StartWebcDemo();

	if (placeHolder)
	{
		pPresentation->Remove(placeHolder);
	}

	pPresentation->Execute();        // do message processing

	pPresentation = pTest->Presentation();
	pScreen = pTest->Screen();
	pMsgQueue = pTest->MessageQueue();

	delete pPresentation;
	delete pMsgQueue;
    delete pScreen;

	webc_Exit();

    if (gpPegCmdLine)
    {
        delete gpPegCmdLine;
    }

    return(0);
}


/*--------------------------------------------------------------------------*/
// PegIdleFunction-
//
// This function gets called by PegPresentationManager when there is nothing
// left for PEG to do. For running with Windows, we return to the Windows
// message loop until a new Windows message arrives.
/*--------------------------------------------------------------------------*/
extern int giAllowTimerTick;
void PegIdleFunction(void)
{
	 PegThing *SomeThing = NULL;
	 MSG msg;
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
		  PegMessage NewMessage;
		  NewMessage.wType = PM_EXIT;
		  NewMessage.pTarget = SomeThing->Presentation();
		  NewMessage.pSource = NULL;
		  SomeThing->MessageQueue()->Push(NewMessage);
	 }
}

int giShift = 0;
int giCaps = 0;
int giCtrl = 0;
int giAlt = 0;

/*--------------------------------------------------------------------------*/
long FAR WINAPI WndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
	 PAINTSTRUCT ps;
	 PegThing *SomeThing = NULL;

	 switch (message)
	 {
	 case WM_PAINT:
		  {
		  BeginPaint(hwnd, &ps);
		  EndPaint(hwnd, &ps);

		  PegMessage NewMessage;
		  NewMessage.wType = PM_DRAW;
		  NewMessage.pTarget = SomeThing->Presentation();
		  SomeThing->MessageQueue()->Push(NewMessage);
		  }
		  return 0;

#if defined(PEG_MOUSE_SUPPORT) || defined(PEG_TOUCH_SUPPORT)
	 case WM_LBUTTONDOWN:
		  {
		  PEGSCREEN *pWin = (PEGSCREEN *) SomeThing->Screen();
		  if (pWin)
		  {
				SetCapture(hwnd);
				PegMessage NewMessage;
				NewMessage.wType = PM_LBUTTONDOWN;
				NewMessage.Point.x = LOWORD(lParam) - pWin->mwWinRectXOffset;
				NewMessage.Point.y = HIWORD(lParam) - pWin->mwWinRectYOffset;
				NewMessage.iData = giShift|giCtrl;
				SomeThing->MessageQueue()->Push(NewMessage);
		  }
		  }
		  return 0;

	 case WM_LBUTTONUP:
		  {
		  ReleaseCapture();
		  PEGSCREEN *pWin = (PEGSCREEN *) SomeThing->Screen();
		  if (pWin)
		  {
		  PegMessage NewMessage;
		  NewMessage.wType = PM_LBUTTONUP;
		  NewMessage.Point.x = LOWORD(lParam) - pWin->mwWinRectXOffset;
		  NewMessage.Point.y = HIWORD(lParam) - pWin->mwWinRectYOffset;
		  NewMessage.iData = giShift|giCtrl;
		  SomeThing->MessageQueue()->Push(NewMessage);
		  }
		  }
		  return 0;
#endif

#if defined(PEG_MOUSE_SUPPORT)

	 case WM_RBUTTONDOWN:
		  {
		  PEGSCREEN *pWin = (PEGSCREEN *) SomeThing->Screen();
		  if (pWin)
		  {
		  PegMessage NewMessage;
		  NewMessage.wType = PM_RBUTTONDOWN;
		  NewMessage.Point.x = LOWORD(lParam) - pWin->mwWinRectXOffset;
		  NewMessage.Point.y = HIWORD(lParam) - pWin->mwWinRectYOffset;
		  NewMessage.iData = giShift|giCtrl;
		  SomeThing->MessageQueue()->Push(NewMessage);
		  }
		  }
		  return 0;

	 case WM_RBUTTONUP:
		  {
		  PEGSCREEN *pWin = (PEGSCREEN *) SomeThing->Screen();
		  if (pWin)
		  {
		  PegMessage NewMessage;
		  NewMessage.wType = PM_RBUTTONUP;
		  NewMessage.Point.x = LOWORD(lParam) - pWin->mwWinRectXOffset;
		  NewMessage.Point.y = HIWORD(lParam) - pWin->mwWinRectYOffset;
		  NewMessage.iData = giShift|giCtrl;
		  SomeThing->MessageQueue()->Push(NewMessage);
		  }
		  }
		  return 0;

	 case WM_MOUSEMOVE:
		  {
		  PEGSCREEN *pWin = (PEGSCREEN *) SomeThing->Screen();
		  if (pWin)
		  {
		  PegMessage NewMessage;
		  NewMessage.wType = PM_POINTER_MOVE;
		  NewMessage.Point.x = LOWORD(lParam) - pWin->mwWinRectXOffset;
		  NewMessage.Point.y = HIWORD(lParam) - pWin->mwWinRectYOffset;
		  NewMessage.iData = giShift|giCtrl;
		  SomeThing->MessageQueue()->Fold(&NewMessage);
		  }
		  }
		  return 0;
#endif

	 case WM_SIZE:
		  #ifdef _PEGWINSCREEN_
		  {
		  PegWinScreen *pWin = (PegWinScreen *) SomeThing->Screen();
				if (pWin)
				{
					 pWin->SetNewWindowsWinSize ((PEGUINT) (LOWORD(lParam)), (PEGUINT) (HIWORD(lParam)));
				}
		  }
		  #endif
		  return 0;
	 case WM_KEYDOWN:
		  {
		  if (lParam == 0x002a0001L ||    // left shift
				lParam == 0x00360001L)      // right shift
		  {
				giShift = KF_SHIFT;
		  }
		  else
		  {
				if (lParam == 0x001d0001 || lParam == 0x011d0001)
				{
					 giCtrl = KF_CTRL;
				}
				else
				{
					 if(lParam == 0x003a0001L)      // caps lock
					 {
						  giCaps = ~giCaps;
					 }
				}
		  }

		  PegMessage NewMessage;
		  NewMessage.wType = PM_KEY;
		#ifdef WEBC_PEG_190_OR_LATER
		  PEGINT iScan = HIWORD(lParam) & 0xff;
		#else
		  SIGNED iScan = HIWORD(lParam) & 0xff;
		#endif

		  if (giCtrl)     // if control key is held down
		  {
				switch(iScan)
				{
				case 0x2d:
					 NewMessage.wType = PM_CUT;
					 break;

				case 0x2e:
					 NewMessage.wType = PM_COPY;
					 break;

				case 0x2f:
					 NewMessage.wType = PM_PASTE;
					 break;
				}
		  }

		  if (iScan < AT_KEYTABLE_LEN)
		  {
				if (giShift || giCaps)
				{
					 iScan = ATKeyTable[iScan][1];
				}
				else
				{
					 iScan = ATKeyTable[iScan][0];
				}
				if (iScan)
				{
					  // this is the scan code for the escape key
			          if (iScan == PK_ESC)
			          {
						  giEscapeDown = 1;
					  }

					 NewMessage.pTarget = NULL;
					 NewMessage.iData = iScan;
					 NewMessage.lData = giShift|giCtrl|giAlt;
					 SomeThing->MessageQueue()->Push(NewMessage);
				}
		  }
		  }
		  return 0;

	 case WM_KEYUP:
		  {
		  if (lParam == 0xc0360001 || lParam == 0xc02a0001)
		  {
				giShift = 0;
		  }
		  else
		  {
				if (lParam == 0xc01d0001 || lParam == 0xc11d0001)
				{
					 giCtrl = 0;
				}
		  }
		  PegMessage NewMessage;
		  NewMessage.wType = PM_KEY_RELEASE;
		#ifdef WEBC_PEG_190_OR_LATER
		  PEGINT iScan = HIWORD(lParam) & 0xff;
		#else
		  SIGNED iScan = HIWORD(lParam) & 0xff;
		#endif

		  if (iScan < AT_KEYTABLE_LEN)
		  {
				if (giShift || giCaps)
				{
					 iScan = ATKeyTable[iScan][1];
				}
				else
				{
					 iScan = ATKeyTable[iScan][0];
				}
				if (iScan)
				{
					  // this is the scan code for the escape key
			          if (iScan == PK_ESC)
			          {
						  giEscapeDown = 0;
					  }

					 NewMessage.wType = PM_KEY_RELEASE;
					 NewMessage.pTarget = NULL;
					 NewMessage.iData = iScan;
					 NewMessage.lData = giShift|giCtrl|giAlt;
					 SomeThing->MessageQueue()->Push(NewMessage);
				}
		  }
		  }
		  return 0;

	 case WM_TIMER:
		  SomeThing->MessageQueue()->TimerTick();

#ifdef RTP_TRACK_LOCAL_MEMORY
			{
				static int loop = 0;

				loop++;
				if (loop > 3 && lastSizeAlloc != rtp_debug_get_mem_used())
				{
					char titleString[256];
					lastSizeAlloc = rtp_debug_get_mem_used();
					sprintf(titleString, "%s (%d bytes allocated)", windowTitleString, rtp_debug_get_mem_used());
					SetWindowText(hwnd, titleString);
					loop = 0;
				}
			}
#endif
		  return 0;

	 case WM_CLOSE:
        {
		    PegPresentationManager *pPresent = SomeThing->Presentation();
            PegMessage ExitMessage(pPresent, PM_EXIT);
            ExitMessage.pSource = NULL;
			pPresent->Message(ExitMessage);
        }
		break;

	 case WM_DESTROY:
		  PostQuitMessage(0);
		  return 0;
	 }

	 return DefWindowProc(hwnd, message, wParam, lParam);
}

#if (defined(PEG_UNICODE) && defined(WEBC_PEG_190_OR_LATER))
/*---------------------------------------------------------------*/
ROMDATA PEGCHAR gsIDL_ENG_TITLE[] =
{   0x0048, 0x0065, 0x006c, 0x006c, 0x006f, 0x0020, 0x0057, 0x006f,
    0x0072, 0x006c, 0x0064, 0};
ROMDATA PEGCHAR gsIDL_ENG_MESSAGE[] =
{   0x004d, 0x0079, 0x0020, 0x0046, 0x0069, 0x0072, 0x0073, 0x0074,
    0x0020, 0x0045, 0x0046, 0x0049, 0x0020, 0x0041, 0x0070, 0x0070,
    0x006c, 0x0069, 0x0063, 0x0061, 0x0074, 0x0069, 0x006f, 0x006e,
    0x0021, 0};



/*---------------------------------------------------------------*/
ROMDATA PEGCHAR *IDL_ENGLISH_Table[] = {
    gsIDL_ENG_TITLE,
    gsIDL_ENG_MESSAGE
};


/*---------------------------------------------------------------*/
ROMDATA PEGCHAR **wbStringTable[1] = {
    IDL_ENGLISH_Table
};

/*---------------------------------------------------------------*/

#endif
