//
// webcdemo_fb_gdip.cpp - WebC Browser demo on Win32/GDI+
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
//#define FB_WINDOW_MANAGER

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <stdio.h>
#define UNICODE
#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include "webcdemo_fb_gdip.h"
#include "webc.h"
#include "webcLog.h"
#include "FBAbstraction.h"
#include "webcmem.h"
//#include "FB_Window_Manager.h"
// #include "portfb.h"


#define RTP_MEM_DEBUG_PRINTF
#define RTP_TRACK_LOCAL_MEMORY
#define RTP_MEM_RESTRAIN 0
#include "rtpmemdb.h"
#include "rtpprint.h"
/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/



#define MAX_LOADSTRING 100

#define CREATE_DOS_CONSOLE 1
#define SPAWN_EXAMPLE 0
#define SPAWN_CHROME_BROWSER 0
#define SPAWN_TEST_SUITE 1



void RedirectIOToConsole(void);

/*****************************************************************************/
// Local Types
/*****************************************************************************/

HWND hConsoleWnd;
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HBROWSER_HANDLE gBrowser[2] = {0,0};

// Forward declarations of functions included in this code module:
ATOM				WebcDemoRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
/* Window proc provided by abstraction layer */
LRESULT CALLBACK messageReceive(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HBROWSER_HANDLE webc_start_chrome_browser(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow);
HBROWSER_HANDLE webc_start_webc_demo(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow);
HBROWSER_HANDLE webc_start_webc_tests(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow, char * pTestsuiteURL);
HBROWSER_HANDLE webc_start_webc_splash(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow);



void VirtualDisplaySelectPageNumber(int newForeGroundPage);

static int webc_start_demo_apps(void);


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	hInst = hInstance; // Store instance handle in our global variable

	// Perform application initialization:
	if (!InitInstance (hInstance, iCmdShow))
	{
		return FALSE;
	}

	// GDI+ initialization
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

 	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WEBCDEMO_GDIP, szWindowClass, MAX_LOADSTRING);
	WebcDemoRegisterClass(hInstance);

	// Create windows and browser, load initial pages, register event handlers etc.
	webc_start_demo_apps();




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

//		LRESULT CALLBACK messageReceive(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	}

	// WebC shutdown
//	windowManagerShutdown();
//	webc_DestroyFBBrowserConfig(config.graphics);
	webc_Exit();
	// GDI+ shutdown
	GdiplusShutdown(gdiplusToken);

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return (int) msg.wParam;
}
int log_number = 0;
void webc_user_debug_window(char *str)
{
  	if (!hConsoleWnd)
  		RedirectIOToConsole();
	printf("[%5.5d] %s\n", log_number++, str);
}

//LRESULT CALLBACK messageReceive(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	return DefWindowProc(hWnd, message, wParam, lParam);//change this to execute default process not related to the keyboard and mouse drivers
//}

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
	wcex.lpfnWndProc	= (WNDPROC) messageReceive;

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

#if (CREATE_DOS_CONSOLE)
    RedirectIOToConsole();

    LOG_PRINTF("Hello world\n");
    LOG_PRINTF("WEBC_SUPPORT_HTTP == %d\n", WEBC_SUPPORT_HTTP);
    LOG_PRINTF("WEBC_SUPPORT_FILE == %d\n", WEBC_SUPPORT_FILE);
    LOG_PRINTF("WEBC_SUPPORT_INTERNAL == %d\n", WEBC_SUPPORT_INTERNAL);
    LOG_PRINTF("WEBC_SUPPORT_UNICODE == %d\n", WEBC_SUPPORT_UNICODE);
    LOG_PRINTF("WEBC_SUPPORT_GIFS == %d\n", WEBC_SUPPORT_GIFS);
    LOG_PRINTF("WEBC_SUPPORT_JPGS == %d\n", WEBC_SUPPORT_JPGS);
    LOG_PRINTF("WEBC_SUPPORT_PNG == %d\n", WEBC_SUPPORT_PNG);
    LOG_PRINTF("WEBC_SUPPORT_MNG == %d\n", WEBC_SUPPORT_MNG);
    LOG_PRINTF("WEBC_SUPPORT_BMP == %d\n", WEBC_SUPPORT_BMP);
    LOG_PRINTF("WEBC_SUPPORT_JSCRIPT == %d\n", WEBC_SUPPORT_JSCRIPT);
    LOG_PRINTF("WEBC_CFG_ENABLE_CHROME == %d\n", WEBC_CFG_ENABLE_CHROME);
    LOG_PRINTF("SPAWN_EXAMPLE == %d\n", SPAWN_EXAMPLE);
    LOG_PRINTF("SPAWN_CHROME_BROWSER == %d\n", SPAWN_CHROME_BROWSER);


#endif
	return TRUE;
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
#if (!(SPAWN_EXAMPLE || SPAWN_CHROME_BROWSER))

char * webc_splash_main_html =
"<html>\n\
<body style=background-color:DeepSkyBlue;>\n\
<center><h1> Hello From WebC</h1></center>\n\
</body>\n\
</html>\n";


/* Utility function that reload the HTML source code of a document from an
   array of chars. After loading the document the display is refreshed */
int webc_splash_load_string(HDOC_HANDLE hDoc, char *html_src)
{
    long written, html_src_length;
    html_src_length = (long) rtp_strlen(html_src) + 1;;
    /* First clear it , don't refresh */
    webc_DocClear(hDoc, WEBC_FALSE);
    webc_DocOpen(hDoc);
    written = webc_DocWriteHtml(hDoc, html_src, html_src_length, WEBC_FALSE);
    if (written < 0)
        return(-1);
    webc_DocClose(hDoc);
    webc_DocRefresh(hDoc);
    return(0);
}


HDOC_HANDLE hSsplashDoc;
/* Main demo entry point */
static int webc_splash_main(HBROWSER_HANDLE hBrowser)
{
    /* Get the root document from the browser. HTML code is loaded into the
       document view and individual elements are accessed from the tree of
       elements contained withing the document view */
    hSsplashDoc = webc_BrowserGetDocument(hBrowser);
    if (!hSsplashDoc)
        return(-1);
    /* Call a helper function to load the root page, which breaks the browser into frames */
    if (webc_splash_load_string(hSsplashDoc, webc_splash_main_html) < 0)
 	{
       return(-1);
  	}

    return(0);
}

HBROWSER_HANDLE webc_start_webc_splash(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow)
{
HBROWSER_HANDLE Browser;

/* 	pconfig->browserEventHandler = webcDemoEventHandler; */
	pconfig->privateData = 0;

	Browser = webc_CreateBrowser(pconfig, CreateNewWindow, "splash", "");
    if (Browser)
    {
	    webc_splash_main(Browser);
    }
    return(Browser);
}


#endif /* (!(SPAWN_EXAMPLE || SPAWN_CHROME_BROWSER)) */



static int webc_register_demo_app_metatags(void);

int SpectrometerDemoStart(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);

static int webc_start_demo_apps(void)
{
	HTMLBrowserConfig config;
	HBROWSER_HANDLE hBrowser;

	WEBC_DEBUG_LOG_TIME_RESET("LOAD STARTED")
	// WebC library initialization
	webc_Init();
	/* Redirect url errors to our own application specific error page */
	webc_setErrorRedirect(URL_ERROR_SERVER_NOT_FOUND, "file://\\demo_pages\\SpectrometerUrlError.html");
	webc_setErrorRedirect(URL_ERROR_PAGE_NOT_FOUND, "file://\\demo_pages\\SpectrometerUrlError.html");
	webc_setErrorRedirect(URL_ERROR_PAYMENT_REQ, "file://\\demo_pages\\SpectrometerUrlError.html");
	webc_setErrorRedirect(URL_ERROR_FORBIDDEN, "file://\\demo_pages\\SpectrometerUrlError.html");

	// Initialize the frame buffer driver
    memset(&config, 0, sizeof(config));
   	webc_InitFBBrowserConfig(&config, 0);


   	//config->browserEventHandler = browserHandler;
   	config.browserEventHandler = 0;
   	config.privateData 		= 0;
   	hBrowser =  webc_CreateBrowser(&config,  true, "Explore" , "");
   	if (!hBrowser)
		return -1;


   	HDOC_HANDLE document;
	document = webc_BrowserGetDocument(hBrowser);
	if (!document)
		return -1;
	SpectrometerDemoStart(hBrowser,document);

	WEBC_DEBUG_LOG_TIME_RESET("LOAD FINISHED")
	return 0;
}

static HTMLMetaStatus SampleMetaTagHandler(const char* pContent, HDOC_HANDLE hdoc);


static int webc_register_demo_app_metatags(void)
{
	webc_RegisterMetaCallback("SampleMetaTag", SampleMetaTagHandler);

	return 0;
}

static HTMLMetaStatus SampleMetaTagHandler(const char* pContent, HDOC_HANDLE hdoc)
{
		// Ignore HTML_META_STATUS_CONTINUE:
		// HTML_META_STATUS_STOP:
		// HTML_META_STATUS_HALT: Error condition
		return(HTML_META_STATUS_STOP);
}





