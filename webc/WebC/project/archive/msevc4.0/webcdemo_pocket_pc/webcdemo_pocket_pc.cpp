// webcdemo_pocket_pc.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "webc.h"
#include "webcgdi_wce.h"
#include "webcdemo_pocket_pc.h"
#include <commctrl.h>
#include <sipapi.h>
#include <aygshell.h>
#include "htmldoc.hpp"

#define ATROAD_NET960_POSITIONING
#define ATROAD_NET960_KEY_HANDLING
#ifdef ATROAD_NET960_POSITIONING
	#define NET960CE_BW_LCD_H_OFFSET	0
	#define NET960CE_BW_LCD_V_OFFSET	0
	#define NET960CE_BW_LCD_H_PIXELS	640
	#define NET960CE_BW_LCD_V_PIXELS	480
#endif
#define MAX_LOADSTRING 100
#define AT_KEYTABLE_LEN 0x59
#define SPAWN_EXAMPLE        0
#define SPAWN_CHROME_BROWSER 1

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

HBROWSER_HANDLE webc_start_chrome_browser(HTMLBrowserConfig *pconfig);
HBROWSER_HANDLE webc_start_webc_demo(HTMLBrowserConfig *pconfig);

extern "C" int giEscapeDown;

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// WebC library initialization
	webc_Init();

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
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
#if (SPAWN_CHROME_BROWSER)
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
#ifdef	ATROAD_NET960_POSITIONING
	SetWindowPos(hWnd, 0, NET960CE_BW_LCD_H_OFFSET, NET960CE_BW_LCD_V_OFFSET, NET960CE_BW_LCD_H_PIXELS, NET960CE_BW_LCD_V_PIXELS, SWP_SHOWWINDOW);
#endif//#ifdef	ATROAD_NET960_POSITIONING

	HTMLBrowserConfig config;

	if (webc_InitGDIWCEBrowserConfig(&config, hWnd) >= 0)
	{
		gBrowser = webc_start_chrome_browser(&config);
	}

	SetTimer(hWnd, 1, 50, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
#endif //#if (SPAWN_CHROME_BROWSER)
#if (SPAWN_EXAMPLE)
	hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}
	else
    {
#ifdef	ATROAD_NET960_POSITIONING
	    SetWindowPos(hWnd, 0, NET960CE_BW_LCD_H_OFFSET, NET960CE_BW_LCD_V_OFFSET, NET960CE_BW_LCD_H_PIXELS, NET960CE_BW_LCD_V_PIXELS, SWP_SHOWWINDOW);
#endif //#ifdef	ATROAD_NET960_POSITIONING
	    HTMLBrowserConfig config;
        if (webc_InitGDIWCEBrowserConfig(&config, hWnd) >= 0)
	    {
            gBrowser = webc_start_webc_demo(&config);
        }
        SetTimer(hWnd, 1, 50, NULL);
	    ShowWindow(hWnd, nCmdShow);
	    UpdateWindow(hWnd);
    }
#endif //#if (SPAWN_EXAMPLE)
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

int wince_scancode_to_webc(unsigned long scancode, int keydown);

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
#ifndef	ATROAD_NET960_KEY_HANDLING
		case WM_KEYDOWN:
		{
unsigned long scancode = (lParam >> 16) & 0x0fff;
int webc_mapping;
            webc_mapping = wince_scancode_to_webc(scancode, 1);
            if (webc_mapping)
            {
			    e.type = HTML_EVENT_KEYDOWN;
			    e.data.key = webc_mapping;
			    if (giShift)
			        e.flags |= HTML_EVENT_FLAG_SHIFT_DOWN;
		        if (giCtrl)
			        e.flags |= HTML_EVENT_FLAG_CTRL_DOWN;
		        if (giAlt)
			        e.flags |= HTML_EVENT_FLAG_ALT_DOWN;
		        if (e.data.key == WGK_ESC)
				    giEscapeDown = 1;
            }
			break;
		}

		case WM_KEYUP:
		{
unsigned long scancode = (lParam >> 16) & 0x0fff;
int webc_mapping;
            webc_mapping = wince_scancode_to_webc(scancode, 0);
            if (webc_mapping)
            {
			    e.type = HTML_EVENT_KEYUP;
			    e.data.key = webc_mapping;
			    if (giShift)
			        e.flags |= HTML_EVENT_FLAG_SHIFT_DOWN;
		        if (giCtrl)
			        e.flags |= HTML_EVENT_FLAG_CTRL_DOWN;
		        if (giAlt)
			        e.flags |= HTML_EVENT_FLAG_ALT_DOWN;
		        if (e.data.key == WGK_ESC)
				    giEscapeDown = 0;
            }
		break;
		}
#endif//#ifndef	ATROAD_NET960_KEY_HANDLING
#ifdef	ATROAD_NET960_KEY_HANDLING
		case WM_CHAR:
			e.type = HTML_EVENT_KEYDOWN;
			e.data.key = webc_TranslateGDIWCEKey(0xff&wParam);
			break;
#endif//#ifdef	ATROAD_NET960_KEY_HANDLING

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




#include "wgkeys.hpp"

/* Win CE scan codes, unshifted, shifted, control */
int wince_scancode[][4] =
{
{0x001C, 'a', 'A', 1},
{0x0032, 'b', 'B', 2},
{0x0021, 'c', 'C', 3},
{0x0023, 'd', 'D', 4},
{0x0024, 'e', 'E', 5},
{0x002B, 'f', 'F', 6},
{0x0034, 'g', 'G', 7},
{0x0033, 'h', 'H', 9},
{0x0043, 'i', 'I', 10},
{0x003B, 'j', 'J', 11},
{0x0042, 'k', 'K', 12},
{0x004B, 'l', 'L', 13},
{0x003A, 'm', 'M', 14},
{0x0031, 'n', 'N', 15},
{0x0044, 'o', 'O', 16},
{0x004D, 'p', 'P', 17},
{0x0015, 'q', 'Q', 18},
{0x002D, 'r', 'R', 19},
{0x001B, 's', 'S', 20},
{0x002C, 't', 'T', 21},
{0x003C, 'u', 'U', 22},
{0x002A, 'v', 'V', 23},
{0x001D, 'w', 'W', 24},
{0x0022, 'x', 'X', 25},
{0x0035, 'y', 'Y', 26},
{0x001A, 'z', 'Z', 27},
{0x0016, '1', '!', 0},
{0x001E, '2', '@', 0},
{0x0026, '3', '#', 0},
{0x0025, '4', '$', 0},
{0x002E, '5', '%', 0},
{0x0036, '6', '^', 0},
{0x003D, '7', '&', 0},
{0x003E, '8', '*', 0},
{0x0046, '9', '(', 0},
{0x0045, '0', ')', 0},
{0x016B, WGK_LEFT, WGK_LEFT, WGK_LEFT},
{0x0175, WGK_LNUP, WGK_LNUP, WGK_LNUP},
{0x0174, WGK_RIGHT, WGK_RIGHT, WGK_RIGHT},
{0x0172, WGK_LNDN, WGK_LNDN, WGK_LNDN},
{0x0066, WGK_BACKSPACE,WGK_BACKSPACE,WGK_BACKSPACE},
{0x005A, WGK_CR,WGK_CR,WGK_CR},
{0x0170, WGK_INSERT,WGK_INSERT,WGK_INSERT},
{0x016C, WGK_HOME,WGK_HOME,WGK_HOME},
{0x017D, WGK_PGUP,WGK_PGUP,WGK_PGUP},
{0x0171, WGK_DELETE,WGK_DELETE,WGK_DELETE},
{0x0169, WGK_END,WGK_END,WGK_END},
{0x017A, WGK_PGDN,WGK_PGDN,WGK_PGDN,},
{0x0005, WGK_F1,  WGK_F1, WGK_F1},
{0x0006, WGK_F2,  WGK_F2, WGK_F2},
{0x0004, WGK_F3,  WGK_F3, WGK_F3},
{0x000C, WGK_F4,  WGK_F4, WGK_F4},
{0x0003, WGK_F5,  WGK_F5, WGK_F5},
{0x000B, WGK_F6,  WGK_F6, WGK_F6},
{0x0083, WGK_F7,  WGK_F7, WGK_F7},
{0x000A, WGK_F8,  WGK_F8, WGK_F8},
{0x0001, WGK_F9,  WGK_F9, WGK_F9},
{0x0009, WGK_F10, WGK_F10, WGK_F10},
{0x0078, WGK_F11, WGK_F11, WGK_F11},
{0x0007, WGK_F12, WGK_F12, WGK_F12},
{0x0014, WGK_CTRL, WGK_CTRL, WGK_CTRL},
{0x0012, WGK_SHIFT, WGK_SHIFT, WGK_SHIFT},
{0x000D, WGK_TAB, WGK_TAB, WGK_TAB},
{0x0076, WGK_ESC, WGK_ESC, WGK_ESC},
{0x0029, ' ', ' ',  0},
{0x0054, '[', '{' , 0},
{0x005b, ']', '}' , 0},
{0x005d, '\\', '|', 0},
{0x004e, '-', '_',  0},
{0x0055, '=', '+',  0},
{0x0041, ',', '<',  0},
{0x0049, '.', '>',  0},
{0x004a, '/', '?',  0},
{0x004c, ';', ':',  0},
{0x0052, '\'', '\"',0},
{0x000e, '\`', '~',0},
{0x0000, 0, 0,0}
};


int wince_scancode_to_webc(unsigned long scancode, int keydown)
{
int code = (int) scancode;
int retcode, i;


    retcode = -1;
    for (i = 0; wince_scancode[i][0] != 0; i++)
    {
        if (wince_scancode[i][0] == code)
        {
            retcode = i;
        }
    }
    if (retcode == -1)
        return(0);

    if (wince_scancode[retcode][1] == WGK_CTRL)
    {
        giCtrl = keydown;
        retcode = -1;
    }
    else if (wince_scancode[retcode][1] == WGK_SHIFT)
    {
        giShift = keydown;
        retcode = -1;
    }
    else if (wince_scancode[retcode][1] == WGK_CAPS)
    {
        if (!keydown)
            giCaps = !giCaps;
        retcode = -1;
    }
    if (retcode != -1)
    {
    int column;

        if (giCtrl)
	        column = 3;
	    if (giShift || giCaps)
	        column = 2;
	    else
	        column = 1;
	    return (wince_scancode[retcode][column]);
    }
    else
        return(0);
}
