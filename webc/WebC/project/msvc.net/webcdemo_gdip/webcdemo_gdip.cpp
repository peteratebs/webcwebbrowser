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

#include "webcdemo_gdip.h"
#include "webc.h"
#include "webcgdip.h"
#include "webcLog.h"

#define RTP_MEM_DEBUG_PRINTF
#define RTP_TRACK_LOCAL_MEMORY
#define RTP_MEM_RESTRAIN 0
#include "rtpmemdb.h"
/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define MAX_LOADSTRING 100

#define CREATE_DOS_CONSOLE 1
#define SPAWN_EXAMPLE 0
#define SPAWN_CHROME_BROWSER 0
#define SPAWN_TEST_SUITE 0
#define SPAWN_APP_BUILDER 1

HBROWSER_HANDLE  webc_start_app_builder(HTMLBrowserConfig *pconfig);

void RedirectIOToConsole(void);

/*****************************************************************************/
// Local Types
/*****************************************************************************/


HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HBROWSER_HANDLE gBrowser[3] = {0,0,0};

// Forward declarations of functions included in this code module:
ATOM				WebcDemoRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
/* Window proc provided by abstraction layer */
LRESULT CALLBACK WebcWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HBROWSER_HANDLE webc_start_chrome_browser(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow);
HBROWSER_HANDLE webc_start_webc_demo(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow);
HBROWSER_HANDLE webc_start_webc_tests(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow, char * pTestsuiteURL);
HBROWSER_HANDLE webc_start_webc_splash(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;
	HTMLBrowserConfig config;
    RECT screenRect;
	// GDI+ initialization
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

#ifdef _DEBUG

	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF);


//	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
//	_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDERR );
#endif

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// WebC library initialization
	webc_Init();

    rtp_memset(&config, 0, sizeof(config));
	if (webc_InitGDIPlusBrowserConfig(&config, 0) < 0)
	{
		return FALSE; // error
	}

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WEBCDEMO_GDIP, szWindowClass, MAX_LOADSTRING);
	WebcDemoRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, iCmdShow))
	{
		return FALSE;
	}

	// create the directories
	CreateDirectoryA(WEBC_CFG_CACHE_PREFIX, NULL);
	CreateDirectoryA(WEBC_CFG_OFFLINE_CACHE_PREFIX, NULL);

#if (SPAWN_APP_BUILDER)
	gBrowser[0] = webc_start_app_builder(&config);
#endif
#if (SPAWN_CHROME_BROWSER)
    gBrowser[0] = webc_start_chrome_browser(&config, WEBC_TRUE);
#endif
#if (SPAWN_EXAMPLE)
    gBrowser[1] = webc_start_webc_demo(&config, WEBC_TRUE);
#endif
#if (SPAWN_TEST_SUITE)
//	gBrowser[2] = webc_start_webc_tests(&config, WEBC_TRUE, 0);
		gBrowser[2] = webc_start_webc_tests(&config, WEBC_TRUE, "file://\\color_demo_pages\\OpacityIndex.html");

		// gBrowser[2] = webc_start_webc_tests(&config, WEBC_TRUE, "webc://welcome.html");
		//gBrowser[2] = webc_start_webc_tests(&config, WEBC_TRUE, "file://\\motopages\\frametest.html");
		//gBrowser[2] = webc_start_webc_tests(&config, WEBC_TRUE, "file://\\motopages\\bike_bmp\\Bike_GIMP_X1.bmp");
		// gBrowser[2] = webc_start_webc_tests(&config, WEBC_TRUE, "file://\\Test_page\\Firstpage.html");
		//   gBrowser[2] = webc_start_webc_tests(&config, WEBC_TRUE,"http://10.1.10.6/CGI_TEST2.html");
	   // gBrowser[2] = webc_start_webc_tests(&config, WEBC_TRUE,"http://10.1.10.6/embed_test3.html");

#endif

#if (!(SPAWN_APP_BUILDER || SPAWN_EXAMPLE || SPAWN_CHROME_BROWSER || SPAWN_TEST_SUITE))
   // rtp_memset(&config, 0, sizeof(config));
    webc_start_webc_splash(&config, WEBC_TRUE);
#endif

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

#if 0
	// Destroy the browser(s) 
	if (gBrowser[0])
	    webc_DestroyGDIPlusBrowser(gBrowser[0]);
	if (gBrowser[1])
	    webc_DestroyGDIPlusBrowser(gBrowser[1]);
	if (gBrowser[2])
	    webc_DestroyGDIPlusBrowser(gBrowser[1]);
#endif

	// WebC shutdown
	webc_Exit();

	// GDI+ shutdown
	GdiplusShutdown(gdiplusToken);

#ifdef _DEBUG
	//_CrtDumpMemoryLeaks();
#endif

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
	wcex.lpfnWndProc	= (WNDPROC) WebcWindowProc;
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

HWND hConsoleWnd;
int log_number = 0;
#define MAX_CONSOLE_LINES 25
void webc_user_debug_window(char *str)
{
  	if (!hConsoleWnd)
  		RedirectIOToConsole();
	printf("[%5.5d] %s\n", log_number++, str);
}

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

struct SaveFileStream
{
	WebcWriteStream* vptr;
	HANDLE hFile;
};

static WEBC_INT32 _saveFileWrite (WebcWriteStream** s, const WEBC_UINT8* data, WEBC_INT32 len)
{
	DWORD dwBytesWritten;
	SaveFileStream* obj = (SaveFileStream*) s;
	WriteFile(obj->hFile, data, len, &dwBytesWritten, NULL);
    return (WEBC_INT32) dwBytesWritten;
}

static void _saveFileClose (WebcWriteStream** s, int status)
{
	SaveFileStream* obj = (SaveFileStream*) s;
	CloseHandle(obj->hFile);
	free(obj);
}

static WebcWriteStream _saveFileClass =
{
	_saveFileWrite,
	_saveFileClose
};

#if (0)
NOT USED -
   HTMLEventStatus webcWin32BrowserEventHandler (
        HBROWSER_HANDLE hbrowser,
		HTMLBrowserEvent* event
	)
{
	OPENFILENAME dialogInfo;
	TCHAR fileName[256] = L"";
	SaveFileStream* saveStream;

	switch (event->type)
	{
		case HTML_BROWSER_EVENT_UNKNOWN_CONTENT_TYPE:
			// open a file dialog and save the data to a file
			dialogInfo.lStructSize       = sizeof(OPENFILENAME); // DWORD
			dialogInfo.hwndOwner         = hGlobalWnd;// HWND
			dialogInfo.hInstance         = hInst;     // HINSTANCE
			dialogInfo.lpstrFilter       = NULL;      // LPCTSTR
			dialogInfo.lpstrCustomFilter = NULL;      // LPTSTR
			dialogInfo.nMaxCustFilter    = 0;         // DWORD
			dialogInfo.nFilterIndex      = 0;         // DWORD
			dialogInfo.lpstrFile         = fileName;  // LPTSTR
			dialogInfo.nMaxFile          = 255;       // DWORD
			dialogInfo.lpstrFileTitle    = NULL;      // LPTSTR
			dialogInfo.nMaxFileTitle     = 0;         // DWORD
			dialogInfo.lpstrInitialDir   = NULL;      // LPCTSTR
			dialogInfo.lpstrTitle        = L"Select a location to save downloaded file...";  // LPCTSTR
			dialogInfo.Flags             = OFN_CREATEPROMPT|OFN_ENABLESIZING|OFN_LONGNAMES|OFN_OVERWRITEPROMPT; // DWORD
			dialogInfo.nFileOffset       = 0;         // WORD
			dialogInfo.nFileExtension    = 0;         // WORD
			dialogInfo.lpstrDefExt       = NULL;      // LPCTSTR
			dialogInfo.lCustData         = 0;         // LPARAM
			dialogInfo.lpfnHook          = NULL;      // LPOFNHOOKPROC
			dialogInfo.lpTemplateName    = NULL;      // LPCTSTR

			// start at the desktop folder
			//if (SHGetSpecialFolderLocation(gWindowHandle, CSIDL_DESKTOP, LPITEMIDLIST *ppidl) == S_OK)
			//{
			//}

			if ((saveStream = (SaveFileStream*) malloc(sizeof(SaveFileStream))) != NULL)
			{
				saveStream->vptr = &_saveFileClass;

				if (GetSaveFileName(&dialogInfo))
				{
					saveStream->hFile = CreateFile((LPTSTR) fileName,  // file name
					        GENERIC_READ | GENERIC_WRITE, // open for read/write
					        0,                            // do not share
					        NULL,                         // default security
					        CREATE_ALWAYS,                // overwrite existing file
					        FILE_ATTRIBUTE_NORMAL,        // normal file
					        NULL);

					if (saveStream->hFile != NULL)
					{
						event->out.redirectTo = &saveStream->vptr;
						return HTML_EVENT_STATUS_CONTINUE;
					}
				}
				free(saveStream);
			}

			return HTML_EVENT_STATUS_DONE;
	}

	return (chromeBrowserEventHandler(hbrowser, event));
}
#endif

#if (SPAWN_APP_BUILDER)
int AppBuilderStart(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);



HBROWSER_HANDLE  webc_start_app_builder(HTMLBrowserConfig *config)
{
    HBROWSER_HANDLE browser;
    HDOC_HANDLE document;
//	rtp_debug_print_mem_usage (0,0,"webC initialized")
    //config->browserEventHandler = browserHandler;
    config->browserEventHandler = 0;
    config->privateData = 0;
    browser = webc_CreateBrowser(config,  WEBC_TRUE, "Explore" , "");
//	rtp_debug_print_mem_usage (0,0,"Browser Created")
    document = webc_BrowserGetDocument(browser);

	if (document)
		AppBuilderStart(browser,document);
	
	return browser;

}
#endif

#if (!(SPAWN_APP_BUILDER || SPAWN_EXAMPLE || SPAWN_CHROME_BROWSER))

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

#include "webc.h"

#if WEBC_SUPPORT_LOCAL
#include "rtpprint.h"
extern "C" int webc_localGet(char *path, void **phandle);
extern "C" int webc_localRead(void *phandle, char *buffer, long length);
extern "C" int webc_localWrite(void *phandle, char *buffer, long length);
extern "C" int webc_localClose(void *phandle);

char *errors[] = { 
	"<html><body>local:error Unknown</body></html>",
	"<html><body>local:servernotfound url server not found error occurred</body></html>",
	"<html><body>local:error404 page not found error occurred</body></html>",
	"<html><body>local:error403 url 402 error occurred</body></html>",
	"<html><body>local:error403 url 403 error occurred</body></html>",
	};

int  bytes_returned[5];

int webc_localGet(char *path, void **phandle)
{
	int which_error = 0;
	if (rtp_strcmp(path,"servernotfound.html") == 0)
	{
		which_error = 1;
	}
	if (rtp_strcmp(path,"pagenotfound.html") == 0)
	{
		which_error = 2;
	}
	if (rtp_strcmp(path,"paymentreq.html") == 0)
	{
		which_error = 3;
	}
	if (rtp_strcmp(path,"forbidden.html") == 0)
	{
		which_error = 4;
	}

	*phandle = (void *) which_error;
	bytes_returned[which_error] = 0;
	return(1);
}

int webc_localRead(void *phandle, char *buffer, long length)
{
	int which_error = (int) phandle;
    /* Return len on the first and zero on the next so it terminates */
		if (bytes_returned[which_error]== 0)
		{
			rtp_strncpy(buffer, errors[which_error], length);
			bytes_returned[which_error] = rtp_strlen((char *) buffer);
			return(bytes_returned[which_error]);
		}
	return(0);
}

int webc_localWrite(void *phandle, char *buffer, long length)
{
	return(-1);
}

int webc_localClose(void *phandle)
{
	return(0);
	//return (rtp_file_close((RTP_FILE)phandle));
}
#endif//WEBC_SUPPORT_LOCAL
