//
// webcdemo_gdip.cpp - WebC Browser demo on Win32/GDI+
//
// EBS - WebC
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

#define _WIN32_WINNT 0x0501
#include <stdio.h>
#define UNICODE
#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include "webcdemo_gdip.h"
#include "webc.h"
#include "webcgdip.h"

void Show_Wigits(HTMLBrowserConfig *);

#define RTP_MEM_DEBUG_PRINTF
#define RTP_TRACK_LOCAL_MEMORY
#define RTP_MEM_RESTRAIN 0
#include "rtpmemdb.h"
/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define MAX_LOADSTRING 100
#define AT_KEYTABLE_LEN 0x59

#define CREATE_DOS_CONSOLE 1
#define SPAWN_SAMPLE_APPLICATION 1


void RedirectIOToConsole(void);

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

HWND hGlobalWnd[2];
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				WebcDemoRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WebcDemoWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;
	RECT Rect;
	// GDI+ initialization
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// WebC library initialization
//	webc_Init();

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WEBCDEMO_GDIP, szWindowClass, MAX_LOADSTRING);
	WebcDemoRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, iCmdShow))
	{
		return FALSE;
	}

#if (WEBC_SUPPORT_SMALL_SCREEN)
#define BROWSER_X  0
#define BROWSER_Y  0
#define BROWSER_WIDTH  WEBC_SCREEN_WIDTH
#define BROWSER_HEIGHT 64
#else
#define BROWSER_X  CW_USEDEFAULT
#define BROWSER_Y  0
#define BROWSER_WIDTH  CW_USEDEFAULT
#define BROWSER_HEIGHT 0
#endif

	hGlobalWnd[1] = CreateWindow(szWindowClass, L"EBS WebC 2.6 GUI programmimg examples - www.ebsembeddedsoftware.com", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hGlobalWnd[1])
		return FALSE;
    else
    {
	    HTMLBrowserConfig config;
        if (webc_InitGDIPlusBrowserConfig(&config, hGlobalWnd[1]) >= 0)
	    {
        // gBrowser[1] = webc_start_webc_demo(&config);
        }
        SetTimer(hGlobalWnd[1], 1, 50, NULL);
        ShowWindow(hGlobalWnd[1], iCmdShow);
        UpdateWindow(hGlobalWnd[1]);
        Show_Wigits(&config);
    }
    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_WEBCDEMO_GDIP);

	// Main message loop:
	int status = GetMessage(&msg, NULL, 0, 0);
	while (status)
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		status = GetMessage(&msg, NULL, 0, 0);
	}

	// WebC shutdown
//	webc_Exit();

	// GDI+ shutdown
	GdiplusShutdown(gdiplusToken);

	return (int) msg.wParam;
}



//
//  FUNCTION: WebcDemoRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM WebcDemoRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_PARENTDC;
	wcex.lpfnWndProc	= (WNDPROC) WebcDemoWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_WEBCDEMO_GDIP);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL; // <default> (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_WEBCDEMO_GDIP;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

#if (CREATE_DOS_CONSOLE)
    RedirectIOToConsole();

    printf("Hello world\n");
    printf("WEBC_SUPPORT_HTTP == %d\n", WEBC_SUPPORT_HTTP);
    printf("WEBC_SUPPORT_FILE == %d\n", WEBC_SUPPORT_FILE);
    printf("WEBC_SUPPORT_INTERNAL == %d\n", WEBC_SUPPORT_INTERNAL);
    printf("WEBC_SUPPORT_UNICODE == %d\n", WEBC_SUPPORT_UNICODE);
    printf("WEBC_SUPPORT_GIFS == %d\n", WEBC_SUPPORT_GIFS);
    printf("WEBC_SUPPORT_JPGS == %d\n", WEBC_SUPPORT_JPGS);
    printf("WEBC_SUPPORT_PNG == %d\n", WEBC_SUPPORT_PNG);
    printf("WEBC_SUPPORT_MNG == %d\n", WEBC_SUPPORT_MNG);
    printf("WEBC_SUPPORT_BMP == %d\n", WEBC_SUPPORT_BMP);
    printf("WEBC_SUPPORT_JSCRIPT == %d\n", WEBC_SUPPORT_JSCRIPT);
    printf("WEBC_CFG_ENABLE_CHROME == %d\n", WEBC_CFG_ENABLE_CHROME);
#endif
	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//


void Draw_Widgits(void);
void widgit_DispatchEvent(HTMLEvent* event);

LRESULT CALLBACK WebcDemoWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HTMLEvent e;

	e.type = HTML_EVENT_NONE;
	e.flags = 0;

	switch (message)
	{
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
			e.data.position.x = LOWORD(lParam);
			e.data.position.y = HIWORD(lParam);
			break;

		default:
			break;
	};

	switch (message)
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				{
					break;
				}
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;

				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			Draw_Widgits();

//			if (which_Browser)
//			{
//				webc_BrowserInvalidate(which_Browser);
//				webc_BrowserDraw(which_Browser);
//			}

			EndPaint(hWnd, &ps);
			break;
		}

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			e.type = HTML_EVENT_MOUSEDOWN;
			break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			e.type = HTML_EVENT_MOUSEUP;
			break;

		case WM_MOUSEMOVE:
			e.type = HTML_EVENT_MOUSEMOVE;
			break;

		case WM_KEYDOWN:
		{
			if (lParam == 0x002a0001L ||    // left shift
			    lParam == 0x00360001L)      // right shift
			{
				giShift = 1;
			}
			else
			{
				if (lParam == 0x001d0001 || lParam == 0x011d0001)
				{
					giCtrl = 1;
				}
				else
				{
					if (lParam == 0x003a0001L)      // caps lock
					{
						giCaps = ~giCaps;
					}
				}
			}

			e.type = HTML_EVENT_KEYDOWN;

			short iScan = HIWORD(lParam) & 0xff;

			if (iScan < AT_KEYTABLE_LEN)
			{
				iScan = _scanCodeToChar[iScan][(giShift || giCaps)? 1 : 0];

				if (iScan)
				{
					// this is the scan code for the escape key
					if (iScan == WGK_ESC)
					{
						giEscapeDown = 1;
					}

					e.data.key = webc_TranslateGDIPlusKey(iScan);

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

			break;
		}

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

			e.type = HTML_EVENT_KEYUP;

			short iScan = HIWORD(lParam) & 0xff;

			if (iScan < AT_KEYTABLE_LEN)
			{
				iScan = _scanCodeToChar[iScan][(giShift || giCaps)? 1 : 0];

				if (iScan)
				{
					// this is the scan code for the escape key
					if (iScan == WGK_ESC)
					{
						giEscapeDown = 0;
					}

					e.data.key = iScan;
					e.data.key = webc_TranslateGDIPlusKey(iScan);

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
			break;
		}

		case WM_TIMER:
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			return 0;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	if (e.type != HTML_EVENT_NONE)
	{
//		webc_BrowserSuspendDraw(which_Browser);
		widgit_DispatchEvent(&e);
//		if (webc_BrowserGetDrawState (which_Browser) > 0)
//		    webc_BrowserResumeDraw(which_Browser);
		Draw_Widgits();
	}
	return DefWindowProc(hWnd, message, wParam, lParam);;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}


#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <stdio.h>
#include <process.h>    /* _beginthread, _endthread */

HWND hConsoleWnd;
#define MAX_CONSOLE_LINES 25


void RedirectIOToConsole(void)
{
    int                          hConHandle;
    HANDLE                        lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE                       *fp;
	BOOL res;
    // allocate a console for this app
	FreeConsole();
    res = AllocConsole();

    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
                               &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES*1000;

    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
                               coninfo.dwSize);
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
                               &coninfo);
    // redirect unbuffered STDOUT to the console
    lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );

    // redirect unbuffered STDIN to the console
    lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "r" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );

    // redirect unbuffered STDERR to the console
    lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stderr = *fp;

  hConsoleWnd = GetConsoleWindow();

}
