// webcdemo_pocket_pc.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "webc.h"
#include "webcgdi_wce.h"
#include "webcdemo_pocket_pc.h"
#include <commctrl.h>
#include <aygshell.h>
#include <sipapi.h>
#include "htmldoc.hpp"
#include "webcdemo.hpp"

//#define ATROAD_NET960 

#ifdef ATROAD_NET960
	#define NET960CE_BW_LCD_H_OFFSET	1280
	#define NET960CE_BW_LCD_V_OFFSET	0
	#define NET960CE_BW_LCD_H_PIXELS	240
	#define NET960CE_BW_LCD_V_PIXELS	64
#endif

#define MAX_LOADSTRING 100
#define AT_KEYTABLE_LEN 0x59
//#define WEBC_SCROLL_PERFORMANCE_TEST

// Global Variables:
HINSTANCE			g_hInst;             // The current instance
HWND				g_hwndCB;            // The command bar handle
HBROWSER_HANDLE     gBrowser = 0;        // Browser instance

static SHACTIVATEINFO s_sai;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass	(HINSTANCE, LPTSTR);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About			(HWND, UINT, WPARAM, LPARAM);
HWND				CreateRpCommandBar(HWND);

int giShift = 0;
int giCaps = 0;
int giCtrl = 0;
int giAlt = 0;
extern "C" int giEscapeDown;

// each key in non-shifted and shifted state
int _scanCodeToChar[AT_KEYTABLE_LEN][2] = {
	{0x00, 0x00}, {0x1b, 0x1b}, {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'}, {'7', '&'}, 
	{'8', '*'}, {'9', '('}, {'0', ')'}, {'-', '_'}, {'=', '+'}, {0x08, 0x08}, {0x09, 0x09},	{'q', 'Q'}, {'w', 'W'}, 
	{'e', 'E'}, {'r', 'R'}, {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'}, {'[', '{'}, 
	{']', '}'}, {0x0d, 0x0d}, {611, 611}, {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'}, 
	{'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {';', ':'}, {'\'','\"'}, {'`', '`'}, {612, 612}, {'\\','|'}, {'z', 'Z'}, 
	{'x', 'X'}, {'c', 'C'}, {'v', 'V'}, {'b', 'B'}, {'n', 'N'}, {'m', 'M'}, {',', '<'}, {'.', '>'}, {'/', '?'}, 
	{612, 612}, {'*', '*'}, {610, 610}, {' ', ' '}, {613, 613}, {650, 650}, {651, 651}, {652, 652}, {653, 653},    
	{654, 654}, {655, 655}, {656, 656}, {657, 657}, {658, 658}, {659, 659}, {614, 614}, {615, 615}, {602, 602},    
	{606, 606}, {604, 604}, {'-', '_'}, {608, 608}, {0, 0}, {609, 609}, {'+', '+'}, {603,603}, {607, 607},    
	{605, 605}, {601, 601}, {600, 600}, {0, 0}, {0, 0}, {0, 0}, {660,660}, {661,661}};          

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// WebC library initialization
	webc_Init();
	webc_RegisterEventCallback (chromeLoadHandler, "chromeLoadHandler");

   HTMLColor white = HTML_RGBAToColor(0xff,0xff,0xff,0x00);
   HTMLColor black = HTML_RGBAToColor(0x00,0x00,0x00,0x00);
   HTMLColor gray  = HTML_RGBAToColor(0xd0,0xd0,0xd0,0x00);

   webc_SetDefault(WEBC_DEFAULT_BUTTON_BACKGROUND_COLOR, gray);
   webc_SetDefault(WEBC_DEFAULT_BUTTON_FRAME_WIDTH, 1);
   webc_SetDefault(WEBC_DEFAULT_BUTTON_HILITE_COLOR, black);
   webc_SetDefault(WEBC_DEFAULT_BUTTON_SHADOW_COLOR, black);

   webc_SetDefault(WEBC_DEFAULT_SLIDER_BACKGROUND_COLOR, white);
   webc_SetDefault(WEBC_DEFAULT_SLIDER_BACKGROUND_FRAME_WIDTH, 1);
   webc_SetDefault(WEBC_DEFAULT_SLIDER_COLOR, gray);
   webc_SetDefault(WEBC_DEFAULT_SLIDER_FRAME_HILITE_COLOR, black);
   webc_SetDefault(WEBC_DEFAULT_SLIDER_FRAME_SHADOW_COLOR, black);
   webc_SetDefault(WEBC_DEFAULT_SLIDER_FRAME_WIDTH, 1);
   webc_SetDefault(WEBC_DEFAULT_SLIDER_HILITE_COLOR, black);
   webc_SetDefault(WEBC_DEFAULT_SLIDER_PADDING, 0);
   webc_SetDefault(WEBC_DEFAULT_SLIDER_SHADOW_COLOR, black);
   webc_SetDefault(WEBC_DEFAULT_SLIDER_WIDTH, 11);

   webc_SetDefault(WEBC_DEFAULT_SCROLL_BAR_BUTTON_LENGTH, 16);
   webc_SetDefault(WEBC_DEFAULT_SCROLL_BAR_BUTTON_OVERLAP, 1); 

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	if (gBrowser)
	{
		HDOC_HANDLE hdoc = webc_BrowserGetDocument(gBrowser);

		if (hdoc)
		{
			webc_JScriptDefineFunction(hdoc, "DebugLog", AppendFile, 2);			
			webc_JScriptDefineFunction(hdoc, "getDefaultUrl", GetDefaultUrl, 0);
		}

		//webc_BrowserLoadUrl(gBrowser, "http://www.amazon.com/");
		//webc_BrowserLoadUrl(gBrowser, "http://news.google.com/");
		//webc_BrowserLoadUrl(gBrowser, "webc://loadChrome_pda.html");
		webc_BrowserLoadUrl(gBrowser, "webc://loadChrome.html");
		//webc_BrowserLoadUrl(gBrowser, "file:///temp/test/div_case3a.html");

	  #ifdef WEBC_SCROLL_PERFORMANCE_TEST
		HTMLDocument* doc = (HTMLDocument*) hdoc;
		for (int y=0; y<100; y++)
		{
			doc->ScrollTo(0,y);
			webc_BrowserDraw(gBrowser);
		}

		// WebC shutdown
		webc_Exit();

		return 0;
	  #endif
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_WEBCDEMO_POCKET_PC);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// WebC shutdown
	webc_Exit();

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    It is important to call this function so that the application 
//    will get 'well formed' small icons associated with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS	wc;

    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WEBCDEMO_POCKET_PC));
    wc.hCursor			= 0;
    wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);
}

//
//  FUNCTION: InitInstance(HANDLE, int)
//
//  PURPOSE: Saves instance handle and creates main window
//
//  COMMENTS:
//
//    In this function, we save the instance handle in a global variable and
//    create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND	hWnd = NULL;
	TCHAR	szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name

	g_hInst = hInstance;		// Store instance handle in our global variable
	// Initialize global strings
	LoadString(hInstance, IDC_WEBCDEMO_POCKET_PC, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	//If it is already running, then focus on the window
	hWnd = FindWindow(szWindowClass, szTitle);	
	if (hWnd) 
	{
		// set focus to foremost child window
		// The "| 0x01" is used to bring any owned windows to the foreground and
		// activate them.
		SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
		return 0;
	} 

	MyRegisterClass(hInstance, szWindowClass);
	
	hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{	
		return FALSE;
	}
	//When the main window is created using CW_USEDEFAULT the height of the menubar (if one
	// is created is not taken into account). So we resize the window after creating it
	// if a menubar is present
	if (g_hwndCB)
    {
		RECT rc;
        RECT rcMenuBar;

		GetWindowRect(hWnd, &rc);
        GetWindowRect(g_hwndCB, &rcMenuBar);
		rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
		
		MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
	}

#ifdef	ATROAD_NET960
	SetWindowPos(hWnd, 0, NET960CE_BW_LCD_H_OFFSET, NET960CE_BW_LCD_V_OFFSET, NET960CE_BW_LCD_H_PIXELS, NET960CE_BW_LCD_V_PIXELS, SWP_SHOWWINDOW); 
#endif
	HTMLBrowserConfig config;

	if (webc_InitGDIWCEBrowserConfig(&config, hWnd) >= 0)
	{
		config.browserEventHandler = chromeBrowserEventHandler;
		config.privateData = 0;

		gBrowser = webc_CreateBrowser(&config, "");
	}

	SetTimer(hWnd, 1, 50, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	//TCHAR szHello[MAX_LOADSTRING];

	HTMLEvent e;

	e.type = HTML_EVENT_NONE;
	e.flags = 0;

	switch (message)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
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
				case IDM_HELP_ABOUT:
					DialogBox(g_hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				    break;
				case IDOK:
					SendMessage (hWnd, WM_CLOSE, 0, 0);
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_CREATE:
			g_hwndCB = CreateRpCommandBar(hWnd);
            // Initialize the shell activate info structure
            memset (&s_sai, 0, sizeof (s_sai));
            s_sai.cbSize = sizeof (s_sai);
			break;

		case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);

			if (gBrowser)
			{
				webc_BrowserInvalidate(gBrowser);
				webc_BrowserDraw(gBrowser);
			}

			EndPaint(hWnd, &ps);
			break;
		}

		case WM_DESTROY:
			CommandBar_Destroy(g_hwndCB);
			PostQuitMessage(0);
			break;

		case WM_ACTIVATE:
            // Notify shell of our activate message
			//SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE); /* commented to remove aygshell.dll dependency */
     		break;

		case WM_SETTINGCHANGE:
			//SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai); /* commented to remove aygshell.dll dependency */
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

#if 0
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

					e.data.key = webc_TranslateGDIWCEKey(iScan);
					
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
					e.data.key = webc_TranslateGDIWCEKey(iScan);

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

#endif
		case WM_CHAR:
					e.type = HTML_EVENT_KEYDOWN;			
					e.data.key = webc_TranslateGDIWCEKey(0xff&wParam);
			break;
		case WM_TIMER:
			BeginPaint(hWnd, &ps);
			webc_BrowserProcessTimers(gBrowser);
			webc_BrowserDraw(gBrowser);
			webc_BrowserProcessLoadQueueAll(gBrowser);
			webc_BrowserDraw(gBrowser);
			EndPaint(hWnd, &ps);
			return 0;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }

	if (gBrowser && (e.type != HTML_EVENT_NONE))
	{
		webc_BrowserSuspendDraw(gBrowser);
		webc_BrowserDispatchEvent(gBrowser, &e);
		webc_BrowserResumeDraw(gBrowser);
		webc_BrowserDraw(gBrowser);
		webc_BrowserProcessLoadQueueAll(gBrowser);
		webc_BrowserDraw(gBrowser);
	}

	return 0;
}

HWND CreateRpCommandBar(HWND hwnd)
{
	SHMENUBARINFO mbi;

	memset(&mbi, 0, sizeof(SHMENUBARINFO));
	mbi.cbSize     = sizeof(SHMENUBARINFO);
	mbi.hwndParent = hwnd;
	mbi.nToolBarId = IDM_MENU;
	mbi.hInstRes   = g_hInst;
	mbi.nBmpId     = 0;
	mbi.cBmpImages = 0;
	//if (!SHCreateMenuBar(&mbi)) /* commented to remove aygshell.dll dependency */
		return NULL;

	return mbi.hwndMB;
}

// Mesage handler for the About box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	SHINITDLGINFO shidi;

	switch (message)
	{
		case WM_INITDIALOG:
			// Create a Done button and size it.  
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hDlg;
			//SHInitDialog(&shidi); /* commented to remove aygshell.dll dependency */
			return TRUE; 

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
