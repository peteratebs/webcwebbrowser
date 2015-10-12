#ifdef WIN32
#ifndef WINDOWSGDI		/* So we can build GDI and Framebuffer from the same project */

#include "WindowsBitMapFrameBuffer.h"
#include "webcmem.h"
#include "assert.h"
#include "FBConfig.h"
#include "Remoteterminal.h"
#include "FBAbstraction.h"

void wcProcessTimedCallbacks(void);


using namespace Gdiplus;

HTMLEventStatus call_BrowserDispatchEvent (HBROWSER_HANDLE hbrowser,	HTMLEvent* event);



bool threadExit;  //when the window manager is shutdown set this to true to stop the threads

WindowsBitMapFrameBuffer * WindowsBitMapFrameBuffer::m_only=WEBC_NULL;


DWORD WINAPI timerThread(LPVOID* param);

FBHWInterface *FBInitFrameBufferDriver(void)
{
	return WindowsBitMapFrameBuffer::Only();

}

FBHWInterface *WindowsBitMapFrameBuffer::Only()
{
	if(!m_only)
		m_only = new WindowsBitMapFrameBuffer();

	return m_only;
}
// Resizes a window so the client area contains the specified size.
static void SetClientSize( HWND hwnd, int clientWidth, int clientHeight );


// ===
/**
*	This method is used to initialize the OS specific code for the frame buffer layer.
*   The threads needed will be started here and the display section will also be set.
*/


	//windows variables for creating a window simulator
extern HINSTANCE hInst;								// current instance
extern TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
WindowsBitMapFrameBuffer *pself;

WindowsBitMapFrameBuffer::WindowsBitMapFrameBuffer(){

	//OS SPECIFIC CODE
	hWnd = CreateWindow(szWindowClass,
	    (LPCWSTR)L"webC Frame Buffer QVGA Color",
		WS_OVERLAPPED,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WEBC_SCREEN_WIDTH,
		WEBC_SCREEN_HEIGHT,
		NULL,
		NULL,
		hInst,
		NULL);

	SetClientSize( hWnd, WEBC_SCREEN_WIDTH, WEBC_SCREEN_HEIGHT );
	SetTimer(hWnd, 1, 50, NULL);
	ShowWindow(hWnd,1);
	threadExit = false;
	g = Graphics::FromHWND(hWnd);
	SolidBrush fillBrush(0xFFFFFFFF);
	Rect re(0,0,WEBC_SCREEN_WIDTH,WEBC_SCREEN_HEIGHT);
	g->FillRectangle(&fillBrush,re);

	//Bitmap used to display the interior of the client portion of the window
	clientDisplayBuffer.bHasTransparentColor = false;
#if (MAPRGBTOGRAY)
	clientDisplayBuffer.pixelFormat = WB_PIXEL_FORMAT_RGBA_32;//this will need to be changed to accomadate different color depths
#elif (MAPINDEXED8TOINDEXED4)
	clientDisplayBuffer.pixelFormat = WB_PIXEL_FORMAT_MONOCHROME_8;//this will need to be changed to accomadate different color depths
#else
	clientDisplayBuffer.pixelFormat = WB_PIXEL_FORMAT_RGBA_32;//this will need to be changed to accomadate different color depths
#endif
	clientDisplayBuffer.iHeight =  (re.GetBottom() - re.GetTop());
	clientDisplayBuffer.iWidth = (re.GetRight() - re.GetLeft());
#if (MAPRGBTOGRAY)
	clientDisplayBuffer.iBytesPerRow = clientDisplayBuffer.iWidth * BYTES_PER_PIXEL;
#elif (MAPINDEXED8TOINDEXED4)
	clientDisplayBuffer.iBytesPerRow = clientDisplayBuffer.iWidth * BYTES_PER_PIXEL;
#else
	clientDisplayBuffer.iBytesPerRow = clientDisplayBuffer.iWidth * BYTES_PER_PIXEL;
#endif
	int buffersize = (clientDisplayBuffer.iBytesPerRow) * (clientDisplayBuffer.iHeight); //  * 2;
	clientDisplayBuffer.pData = (WEBC_UINT8*)WEBC_MALLOC(buffersize);

	clientDisplayBuffer2 = clientDisplayBuffer;
	clientDisplayBuffer2.pData = (WEBC_UINT8*)WEBC_MALLOC(buffersize);

	m_displayBuffer = 0;

	memset((void*)clientDisplayBuffer.pData, 0,buffersize);
	memset((void*)clientDisplayBuffer2.pData, 0,buffersize);
	pself = this;
//	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)timerThread,this,NULL,&timeoutManagerThreadId);
	//END OS_SPECIFIC CODE
}

/**
*	Destroys os specific threads and screen displaying mechanism.
*/
WindowsBitMapFrameBuffer::~WindowsBitMapFrameBuffer(){
	threadExit = true;
	delete(g);
	DestroyWindow(hWnd);
}


WEBC_UINT8 *WindowsBitMapFrameBuffer::GetBaseAddress(WEBC_INT whichBuffer)
{
	return (WEBC_UINT8 *)(whichBuffer ? clientDisplayBuffer2.pData : clientDisplayBuffer.pData);
}

WEBC_INT WindowsBitMapFrameBuffer::GetNumberOfBuffers()
{
	return 1;
}

WEBC_INT WindowsBitMapFrameBuffer::GetWidth()
{
	return clientDisplayBuffer.iWidth;
}

WEBC_INT WindowsBitMapFrameBuffer::GetHeight()
{
	return clientDisplayBuffer.iHeight;
}

WEBC_INT WindowsBitMapFrameBuffer::GetDepth()
{
	return BYTES_PER_PIXEL;
}

WEBC_INT WindowsBitMapFrameBuffer::GetRowBytes()
{
	return clientDisplayBuffer.iBytesPerRow;
}

void WindowsBitMapFrameBuffer::SwitchToBuffer(WEBC_INT whichBuffer)
{
	m_displayBuffer = whichBuffer;
}
/**
*	This method will take the internal display that WebC has been working with
*   and transform it to an understandable format for the target device.
*
*	Params: disp - internal webc bitmap for screen display, this bitmap represents what must be
*				   displayed to the screen device
*/
void WindowsBitMapFrameBuffer::DoDisplay(void)
{
	WebcGDIPlusBitmap* bitmap;

	if(m_displayBuffer)
		bitmap = convertBitmap(&clientDisplayBuffer2, 0, 0,clientDisplayBuffer.iWidth, clientDisplayBuffer.iHeight);
	else
		bitmap = convertBitmap(&clientDisplayBuffer, 0, 0,clientDisplayBuffer.iWidth, clientDisplayBuffer.iHeight);

	if(bitmap != NULL){
		Status stat = g->DrawImage(bitmap->bitmap, 0,0);//put it on the screen
		delete(bitmap->bitmap);
		WEBC_FREE((void*)bitmap->bitmapData);
		WEBC_FREE((void*)bitmap);
	}
}



/**
*	Window simulator specific. This method takes an internal bitmap representation and returns a Window bitmap.
*/
WebcGDIPlusBitmap* WindowsBitMapFrameBuffer::convertBitmap(WebGraphBitmapInfo* info, DISPLAY_INT xPos, DISPLAY_INT yPos, DISPLAY_INT width, DISPLAY_INT height)
{

#if (MAPRGBTOGRAY||MAPINDEXED8TOINDEXED4)
	WEBC_SIZE dstBytesPerRow = width/2;
	int paletteSizeNeeded = 16;
#else
	int paletteSizeNeeded = (info->pixelFormat == WB_PIXEL_FORMAT_INDEX_8)? info->iPaletteSize : 0;
	WEBC_SIZE dstBytesPerRow = (width * 3 + (4-1)) & ~(4-1);
#endif
	size_t dataSize = height * dstBytesPerRow;

	WEBC_UINT8* buffer = (WEBC_UINT8*) WEBC_MALLOC (sizeof(BITMAPINFOHEADER) +
		                                            sizeof(RGBQUAD) * paletteSizeNeeded +
		                                            dataSize);

	if (!buffer)
	{
		return (NULL);
	}

	BITMAPINFOHEADER* bmpHeader = (BITMAPINFOHEADER*) buffer;

	static int bitCountMap[] =
	{
		24, // WB_PIXEL_FORMAT_RGB_24       = 0,  // 24bit color, stored red-green-blue
		8,  // WB_PIXEL_FORMAT_INDEX_8      = 1,  // palette indexed, 8 bits per pixel
		8,  // WB_PIXEL_FORMAT_MONOCHROME_8 = 2,  // monochrome, 8 bits per pixel
		32,  // WB_PIXEL_FORMAT_RGBA_32

#if (WEBC_SUPPORT_BMP_16BITS)
		16,
#endif
	};

	bmpHeader->biSize = sizeof(BITMAPINFOHEADER);
	bmpHeader->biWidth = width;
	bmpHeader->biHeight = 0 - height; // negative means top-down
	bmpHeader->biPlanes = 1;
#if (MAPRGBTOGRAY||MAPINDEXED8TOINDEXED4)
	bmpHeader->biBitCount = 4;
#else
	bmpHeader->biBitCount = bitCountMap[0];
#endif
	bmpHeader->biCompression = BI_RGB;
	bmpHeader->biSizeImage = 0;
	bmpHeader->biXPelsPerMeter = 72 * 39; // 72dpi * 39 inches/meter
	bmpHeader->biYPelsPerMeter = 72 * 39; // 72dpi * 39 inches/meter
	bmpHeader->biClrUsed = paletteSizeNeeded;
	bmpHeader->biClrImportant = 0;

	RGBQUAD* bmpColors = (RGBQUAD*) (buffer + bmpHeader->biSize);
	int n;
#if (MAPRGBTOGRAY||MAPINDEXED8TOINDEXED4)
	unsigned char c = 0;
	for (n = 0; n < paletteSizeNeeded; n++, c += 16)
	{
		bmpColors[n].rgbRed   = c;	bmpColors[n].rgbGreen = c;	bmpColors[n].rgbBlue  = c;
		bmpColors[n].rgbReserved = 0;
	}
	c = 0;
	bmpColors[0].rgbRed   = c;	bmpColors[0].rgbGreen = c;	bmpColors[0].rgbBlue  = c;
	c = 0xff;
	bmpColors[15].rgbRed   = c;	bmpColors[15].rgbGreen = c;	bmpColors[15].rgbBlue  = c;
#else
	for (n = 0; n < paletteSizeNeeded; n++)
	{
		bmpColors[n].rgbRed   = info->pPalette[n + (n<<1) + 0];
		bmpColors[n].rgbGreen = info->pPalette[n + (n<<1) + 1];
		bmpColors[n].rgbBlue  = info->pPalette[n + (n<<1) + 2];
		bmpColors[n].rgbReserved = 0;
	}
#endif
	WEBC_UINT8 *pData = buffer + (sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * paletteSizeNeeded);
	WEBC_UINT8 *dstPtr, *remotePtr;
	const WEBC_UINT8* srcPtr;
	DISPLAY_INT x, y;

	/* If we are a headless device we'll get a buffer to put the data in that will
	   be sent to the terminal. The raw input (output of the render stage is send
	   accross */
// HEREHERE
//	remotePtr = (WEBC_UINT8*) get_TerminalDisplayBuffer();
	remotePtr = 0; // (WEBC_UINT8*) get_TerminalDisplayBuffer();
	dstPtr = pData;

	srcPtr = (const WEBC_UINT8*) get_TerminalDisplayLine(xPos, yPos);



	if (!srcPtr)
		srcPtr = (const WEBC_UINT8*) info->pData + (yPos * info->iBytesPerRow) + (xPos * 3);

	if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_24) // Should work over the network as is
	{
		WEBC_UINT8* destRowPtr = dstPtr;
		WEBC_UINT8* srcRowPtr = (WEBC_UINT8*)srcPtr;
		WEBC_UINT8* srcColPtr,*destColPtr;
		for (y = 0; y < height; y++, destRowPtr += dstBytesPerRow, srcRowPtr += info->iBytesPerRow)
		{
			srcColPtr = srcRowPtr;
			destColPtr = destRowPtr;
			for (x = 0; x < width; x++, srcColPtr += 3, destColPtr += 3)
			{
				destColPtr[0] = srcColPtr[2];
				destColPtr[1] = srcColPtr[1];
				destColPtr[2] = srcColPtr[0];
			}
		}
	}
	else if (info->pixelFormat == WB_PIXEL_FORMAT_RGBA_32)
	{
#if (MAPRGBTOGRAY)  // Will not work over the network as is
		WEBC_UINT8* destRowPtr = dstPtr;
		WEBC_UINT8* srcRowPtr = (WEBC_UINT8*)srcPtr;
		WEBC_UINT8* srcColPtr,*destColPtr;

		for (y = 0; y < height; y++, destRowPtr += dstBytesPerRow, srcRowPtr += info->iBytesPerRow)
		{
		WEBC_UINT8 c,c2;
			srcColPtr = srcRowPtr;
			destColPtr = destRowPtr;
			for (x = 0; x < width/2; x++)
			{
				c = Rgbto4BitGray(srcColPtr[2], srcColPtr[1], srcColPtr[0]);
				c <<= 4;
				srcColPtr += 4;
				c2 = Rgbto4BitGray(srcColPtr[2], srcColPtr[1], srcColPtr[0]);
				srcColPtr += 4;
				destColPtr[0] = c | (c2&0x0f);
				destColPtr += 1;
			}
		}
#else
		WEBC_UINT8* destRowPtr = dstPtr;
		WEBC_UINT8* srcRowPtr = (WEBC_UINT8*)srcPtr;
		WEBC_UINT8* srcColPtr,*destColPtr;
		for (y = 0; y < height; y++, destRowPtr += dstBytesPerRow, srcRowPtr += info->iBytesPerRow)
		{
			srcColPtr = srcRowPtr;
			destColPtr = destRowPtr;
			for (x = 0; x < width; x++, srcColPtr += 4, destColPtr += 3)
			{
				destColPtr[0] = srcColPtr[2];
				destColPtr[1] = srcColPtr[1];
				destColPtr[2] = srcColPtr[0];
			}
		}
#endif
	}
#if (WEBC_SUPPORT_BMP_16BITS)
	else if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_16) // Should work over the network as is
	{
		WEBC_UINT8* destRowPtr = dstPtr;
		WEBC_UINT8* srcRowPtr = (WEBC_UINT8*)srcPtr;
		WEBC_UINT8* srcColPtr,*destColPtr;
		for (y = 0; y < height; y++, destRowPtr += dstBytesPerRow, srcRowPtr += info->iBytesPerRow)
		{
			srcColPtr = srcRowPtr;
			destColPtr = destRowPtr;
			for (x = 0; x < width; x++, srcColPtr += 2, destColPtr += 2)
			{
				destColPtr[0] = srcColPtr[2];
				destColPtr[1] = srcColPtr[1];
			}
		}
	}
#endif
	else
	{
// #if (MAPINDEXED8TOINDEXED4)

		if (remotePtr)
		{	// Send data over the network as it was rendered
		WEBC_UINT8* destRowPtr = remotePtr;
		WEBC_UINT8* srcRowPtr = (WEBC_UINT8*)srcPtr;
		WEBC_UINT8* srcColPtr,*destColPtr;

			for (y = 0; y < height; y++, destRowPtr += info->iBytesPerRow, srcRowPtr += info->iBytesPerRow)
			{
				srcColPtr = srcRowPtr;
				destColPtr = destRowPtr;
				for (x = 0; x < width; x++)
				{
					destColPtr[0] = srcColPtr[0];
					srcColPtr += 1;
					destColPtr += 1;
				}
			}
		}
		// Uncomment in the next line when running on truly headless system (needs to be moved anyway
//		else
		{ // Render it natively
		WEBC_UINT8* destRowPtr = dstPtr;
		WEBC_UINT8* srcRowPtr = (WEBC_UINT8*)srcPtr;
		WEBC_UINT8* srcColPtr,*destColPtr;
			for (y = 0; y < height; y++, destRowPtr += dstBytesPerRow, srcRowPtr += info->iBytesPerRow)
			{
			WEBC_UINT8 c,c2;
				srcColPtr = srcRowPtr;
				destColPtr = destRowPtr;
				for (x = 0; x < width/2; x++)
				{
					c = srcColPtr[0] & 0x0f;
					c <<= 4;
					srcColPtr += 1;
					c2 = srcColPtr[0];
					destColPtr[0] = c | (c2&0x0f);
					srcColPtr += 1;
					destColPtr += 1;
				}
			}
		}

// #endif
	}

	WebcGDIPlusBitmap* ret = (WebcGDIPlusBitmap*)WEBC_MALLOC(sizeof(WebcGDIPlusBitmap));
	ret->bitmap = new Bitmap((BITMAPINFO*)bmpHeader,(void*) pData);
	ret->bitmapData = buffer;
	if (ret->bitmap)
	{
		return(ret);
	}
	WEBC_FREE(buffer);



	return NULL;
}




/**
*	Timer thread, deals with timers.
*
*	To convert this method to run on your specific platform change the call to make it
*   a threadable method.
*
*	Params & Returns: OS specific depends on threading implementation
*   This seems not to be working
*/
DWORD WINAPI timerThread(LPVOID* param){
	while(threadExit != true){
//		((WindowsBitMapFrameBuffer *)param)->DoDisplay();
		((FBHWInterface *)param)->DoDisplay();
		Sleep(100);//This may be changed to be customizable;
	}
	return 0;
}


// Resizes a window so the client area contains the specified size.
static void SetClientSize( HWND hwnd, int clientWidth, int clientHeight )
{
  if ( IsWindow( hwnd ) )
  {

    DWORD dwStyle = GetWindowLongPtr( hwnd, GWL_STYLE ) ;
    DWORD dwExStyle = GetWindowLongPtr( hwnd, GWL_EXSTYLE ) ;
    HMENU menu = GetMenu( hwnd ) ;

    RECT rc = { 0, 0, clientWidth, clientHeight } ;

    if(!AdjustWindowRectEx( &rc, dwStyle, menu ? TRUE : FALSE, dwExStyle ))
      MessageBox(NULL,(LPCWSTR)L"AdjustWindowRectEx Failed!",(LPCWSTR)L"Error",MB_OK);

    SetWindowPos( hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
                  SWP_NOZORDER | SWP_NOMOVE ) ;
#ifdef _DEBUG
    RECT newClientRC;
    GetClientRect(hwnd,&newClientRC);
    assert((newClientRC.right-newClientRC.left)==clientWidth);
    assert((newClientRC.bottom-newClientRC.top)==clientHeight);
#endif
  }
}

// Alert box for Javascript.. (badly in need of rework)
extern "C" void webc_user_alert_box(char *str)
{
WEBC_CHAR UnicodeStr[512];

    webc_c_strcpy (&UnicodeStr[0],str);
    MessageBox(0, (LPCTSTR) UnicodeStr, (LPCTSTR) L"webC Error", MB_OKCANCEL);
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
extern void * currentFocusedWindow;//current window to which events are sent
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

#if (MAX_VIRTUAL_DISPLAYS > 0)
void VirtualDisplayRotateBrowserNumber(void);

extern "C" {
HBROWSER_HANDLE VirtualDisplayGetCurrentBrowser(void);
};
#endif

/* Browser timout handller, called from an external timer handle */
extern "C" { void doFbBrowserTimeOut(unsigned long lBrowser); }


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

LRESULT CALLBACK messageReceive(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	int send_to_webc = 0;
	int wmId; //variable for windows exit function

	HTMLEvent e;
	e.target = NULL;
	e.type = HTML_EVENT_NONE;
	e.flags = 0;

	DISPLAY_INT x,y;
//	short keyValue;


	switch(message){
		case WM_RBUTTONDOWN: //change this case to be mouse drivers right button down
			//x = LOWORD(lParam);  //change to extract the x position related with event
			//y = HIWORD(lParam);	//change to extract the y position related with event
			//send_to_webc = mouse_button_event(hWnd, RBUTTON_DOWN,x,y,e);
//			rtp_debug_print_mem_usage (0,0,"Clicked");
#if (MAX_VIRTUAL_DISPLAYS > 0)
			VirtualDisplayRotateBrowserNumber();
			UpdateWindow(hWnd);
#endif
			break;
		case WM_LBUTTONDOWN: //change this case to be the mouse drivers left button down
			x = LOWORD(lParam); //change to extract the x position related with event
			y = HIWORD(lParam); //change to extract the y position related with event
			send_to_webc = mouse_button_event(hWnd, LBUTTON_DOWN,x,y,&e);
			break;
		case WM_LBUTTONUP: //change this case to be the mouse drivers left button up
			x = LOWORD(lParam); //change to extract the x position related with event
			y = HIWORD(lParam); //change to extract the y position related with event
			send_to_webc = mouse_button_event(hWnd, LBUTTON_UP,x,y,&e);
			break;
		case WM_RBUTTONUP: //change this case to be the mouse drivers right button up
			//x = LOWORD(lParam); //change to extract the x position related with event
			//y = HIWORD(lParam); //change to extract the y position related with event
			//send_to_webc = mouse_button_event(hWnd, RBUTTON_UP,x,y,&e);
			break;
		case WM_MOUSEMOVE: //change this case to be the enumeration for when driver detects the mouse moves
			x = LOWORD(lParam); //change to extract the x position related with the event
			y = HIWORD(lParam); //change to extract the y position related with the event
			send_to_webc = mouse_move_event(hWnd,x,y,&e);
			break;
		case WM_KEYDOWN://change this case to be the keyboard drivers key down
			/*
			*  keyValue is the driver value of the key where keyValue must be set to either an ascii value from the keyBoard
			*  i.e. 'a' or 'A' or keyValue is one of the values defined in keyData i.e. WGK_TAB or WGK_SHIFT
			*  The assignment of keyValue must be changed to extract the keypressed from the drivers sent value
			*/
			send_to_webc = keyboard_event(hWnd,KEY_DOWN, pcKbToWebKey((short) (HIWORD(lParam) & 0xff)), &e);
//			keyValue = HIWORD(lParam) & 0xff;
//			keyValue = keyboardCharacters[keyValue][(giShift || giCaps)? 1 : 0]; //turn keyValue into ascii or keyData value
//			keyboard_event(KEY_DOWN, webc_TranslateFBKey(keyValue), e);
			break;
		case WM_KEYUP: //change this case to be the keyboard drivers key up
			/*
			*  keyValue is the driver value of the key where keyValue must be set to either an ascii value from the keyBoard
			*  i.e. 'a' or 'A' or keyValue is one of the values defined in keyData i.e. WGK_TAB or WGK_SHIFT
			*  The assignment of keyValue must be changed to extract the key pressed from the drivers sent value
			*/
			send_to_webc = keyboard_event(hWnd,KEY_UP, pcKbToWebKey((short) (HIWORD(lParam) & 0xff)), &e);
			break;
// ===
		case WM_TIMER:
		{
		PAINTSTRUCT ps;
		HBROWSER_HANDLE which_Browser = 0;
			/* doFbBrowserTimeOut() only needs which_Browser when window manager is not present */
#if (INCLUDE_WINDOW_MANAGER == 0)
			which_Browser  = _FindBrowserByWindow((void *)hWnd);
#endif
			call_TerminalTimout(which_Browser);	/* See if we have any remote screen data to display */

			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			wcProcessTimedCallbacks();
#if (USE_EXPERIMENTAL_TIMER_EVENT)
			send_to_webc = 1;
			e.type = HTML_EVENT_TIMER;
			break;
#else
			doFbBrowserTimeOut((unsigned long )which_Browser);
			// webc_BrowserProcessTimers(which_Browser);
			// webc_BrowserDraw(which_Browser);
			// webc_BrowserProcessLoadQueueAll(which_Browser);
			// webc_BrowserDraw(which_Browser);
			return 0;
#endif
		}

// ====
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_COMMAND://windows specific case, close the simulator and free stuff
			wmId  = LOWORD(wParam);
			switch (wmId)
			{
				case IDM_EXIT:
					DestroyWindow(hWnd);
				default:
					break;
			}
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
		target = (void *)e.target;
//		which_Browser  = FBWM_FindBrowserByWindow((void *)e.target);
#else
		target = (void *)hWnd;
//		which_Browser  = _FindBrowserByWindow((void *)hWnd);
#endif
		which_Browser = _FindBrowserByTarget(target);
		if (!which_Browser)
			return DefWindowProc(hWnd, message, wParam, lParam);//change this to execute default process not related to the keyboard and mouse drivers
        	//    HEREHERE is this right... needs to be fixed
		if (e.type != HTML_EVENT_NONE)
		{
			webc_BrowserSuspendDraw(which_Browser);
			if (e.type == HTML_EVENT_KEYDOWN && pastebuffer[0])
			{
			int i;
				for (i = 0; pastebuffer[i]; i++)
				{
					e.data.key = pastebuffer[i];
					e.type = HTML_EVENT_KEYDOWN;
					r = call_BrowserDispatchEvent(which_Browser, &e);
					e.data.key = pastebuffer[i];
					e.type = HTML_EVENT_KEYUP;
					r = call_BrowserDispatchEvent(which_Browser, &e);
				}
			}
			else
				r = call_BrowserDispatchEvent(which_Browser, &e);
//			if (r != HTML_EVENT_STATUS_CONTINUE)
//				return 0;

			/* Search for the browser again in case it was destroyed by the event handler */
			which_Browser = _FindBrowserByTarget(target);
			if (!which_Browser)
	        	return 0;
		    if (webc_BrowserGetDrawState (which_Browser) > 0)
		    	webc_BrowserResumeDraw(which_Browser);

			which_Browser = _FindBrowserByTarget(target);
		    if (!which_Browser)
	        	return 0;
			which_Browser = _FindBrowserByTarget(target);
		    if (!which_Browser)
	        	return 0;
			webc_BrowserSuspendDraw(which_Browser); // new
		    webc_BrowserProcessLoadQueueAll(which_Browser);
		    if (webc_BrowserGetDrawState (which_Browser) > 0) // new
		    	webc_BrowserResumeDraw(which_Browser);
			which_Browser = _FindBrowserByTarget(target);
		    if (!which_Browser)
	        	return 0;
		    webc_BrowserDraw(which_Browser);
		}
	    return 0;
// this is how gdip does it
		// return DefWindowProc(hWnd, message, wParam, lParam);;
	}
	else
	{
		return DefWindowProc(hWnd, message, wParam, lParam);//change this to execute default process not related to the keyboard and mouse drivers
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
#else

#if (0)
// Variables defined in FB_Window_Manager needed for various methods, DO NOT CHANGE
extern WebcTimeoutManager* timeoutManager;//timer manager for windows to see if they are dead
extern FB_List* messageList;//list of messages for windows
extern FB_List* windowList;//list of current open windows
extern DISPLAY_INT prevCursorPositionX, prevCursorPositionY;//this is for when moving a window

boolean variable to control certain behavior of windowmanager
extern bool timerMessageExists;//eliminates duplicate timer messages
extern bool repaintExists;
extern bool windowCaptureEvents;//events go to the window as opposed to the client

extern FB_Graphics* screenBuffer;//buffer to display to screen

Unsure about this, may need to tie in the window manager
/**
*	Process the messages from the message queue.
*
*	To convert this method to run on your specific platform change the call to make it
*   a threadable method.
*
*	Params & Returns: OS specific depending on threading implementations.
*/
DWORD WINAPI messageProcess(LPVOID* param){

	WEBC_UINT8* which_Browser;
	HTMLEvent* e;
	while(threadExit != true){
	e = (HTMLEvent*)messageList->pop();
	if(e != NULL){

		which_Browser = _FindBrowserByWindow((void*)e->target);
		if(which_Browser){
			switch(e->type){
				case FBWM_TIMER:
					/* printf("Message at messageProcess: %s\n", "FBWM_TIMER");
					webc_BrowserProcessTimers(which_Browser);
					webc_BrowserDraw(which_Browser);
					webc_BrowserProcessLoadQueueAll(which_Browser);
					webc_BrowserDraw(which_Browser);
					timerMessageExists = false;*/

					break;
				case FBWM_PAINT:
					//printf("Message at messageProcess: %s\n", "FBWM_PAINT");
					((FB_Window*)e->target)->setFrameRedraw(true);
					webc_BrowserInvalidate(which_Browser);
					webc_BrowserDraw(which_Browser);
					repaintExists = false;
					break;
				case HTML_EVENT_MOUSEMOVE:
					if(windowCaptureEvents == true){
						moveWindowBy(e->target,(e->data.position.x - prevCursorPositionX), (e->data.position.y - prevCursorPositionY));
						prevCursorPositionX = e->data.position.x;
						prevCursorPositionY = e->data.position.y;
						break;
					}
				case HTML_EVENT_MOUSEDOWN:
					if(e->type == HTML_EVENT_MOUSEDOWN && ((FB_Window*)e->target)->inWindowBar(e->data.position.x, e->data.position.y)){
						windowCaptureEvents = true;
						currentFocusedWindow = ((FB_Window*)e->target);
						prevCursorPositionX = e->data.position.x;
						prevCursorPositionY = e->data.position.y;
						break;
					}

				case HTML_EVENT_MOUSEUP:
					if( e->type == HTML_EVENT_MOUSEUP && windowCaptureEvents ){
						moveWindowBy(e->target,(e->data.position.x - prevCursorPositionX), (e->data.position.y - prevCursorPositionY));
						currentFocusedWindow = NULL;
						windowCaptureEvents = false;
						break;
					}
					//This adjusment is because the browser is aware of it's corrdinates relative to 0,0
					//The events are triggerd and are given as corrdinates relative to location on screen
					e->data.position.x -= ((FB_Window*)e->target)->getClientOffsetX();
					e->data.position.y -= ((FB_Window*)e->target)->getClientOffsetY();
				default:
					//printf("Message at messageProcess: %s\n", "Windows Driver Message");
					webc_BrowserSuspendDraw(which_Browser);
					call_BrowserDispatchEvent(which_Browser, e);

					/*Search for the browser again in case it was destroyed by the event handler*/
					which_Browser = _FindBrowserByWindow((void*)e->target);
					if (!which_Browser)
						break;
					if (webc_BrowserGetDrawState (which_Browser) > 0)
						webc_BrowserResumeDraw(which_Browser);
					which_Browser = _FindBrowserByWindow((void*)e->target);
					if (!which_Browser)
						break;
					webc_BrowserDraw(which_Browser);
					which_Browser = _FindBrowserByWindow((void*)e->target);
					if (!which_Browser)
						break;
					webc_BrowserProcessLoadQueueAll(which_Browser);
					which_Browser = _FindBrowserByWindow((void*)e->target);
					if (!which_Browser)
						break;
					webc_BrowserDraw(which_Browser);
					break;
			}

		}
		WEBC_FREE((void*)e);
	}


	}
	return 0;
}
#endif
#endif /* WINDOWSGDI	*/
#endif // WIN32
