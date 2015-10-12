#ifdef WIN32
#ifndef WINDOWSGDI		/* So we can build GDI and Framebuffer from the same project */

#include "WindowsBitMapFrameBuffer.h"
#include "webcmem.h"
#include "assert.h"
#include "FBConfig.h"
#include "FBAbstraction.h"

using namespace Gdiplus;

//windows variables for creating a window simulator
extern HINSTANCE hInst;								// current instance
extern TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name


// Resizes a window so the client area contains the specified size.
static void SetClientSize( HWND hwnd, int clientWidth, int clientHeight );

// ===
/**
*	This method is used to initialize the OS specific code for the frame buffer layer.
*   The threads needed will be started here and the display section will also be set.
*/

TargetBitMapFrameBuffer::TargetBitMapFrameBuffer(){

	// Create a window to display into
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

	// Set it to the proper screen size
	SetClientSize( hWnd, WEBC_SCREEN_WIDTH, WEBC_SCREEN_HEIGHT);
	// Set a timer to go off every 50 msecs
	SetTimer(hWnd, 1, 50, NULL);
	ShowWindow(hWnd,1);

	g = Graphics::FromHWND(hWnd);
	SolidBrush fillBrush(0xFFFFFFFF);
	Rect re(0,0,WEBC_SCREEN_WIDTH,WEBC_SCREEN_HEIGHT);
	g->FillRectangle(&fillBrush,re);

	// Configure a Bitmap used to display the interior of the client portion of the window
	clientDisplayBuffer.bHasTransparentColor = false;
#if (MAPRGBTOGRAY)
	clientDisplayBuffer.pixelFormat = WB_PIXEL_FORMAT_RGBA_32;//this will need to be changed to accomadate different color depths
#elif (MAPINDEXED8TOINDEXED4)
	clientDisplayBuffer.pixelFormat = WB_PIXEL_FORMAT_MONOCHROME_8;//this will need to be changed to accomadate different color depths
#else
	clientDisplayBuffer.pixelFormat = WB_PIXEL_FORMAT_RGBA_32;//this will need to be changed to accomadate different color depths
#endif

	clientDisplayBuffer.iHeight =  WEBC_SCREEN_HEIGHT;
	clientDisplayBuffer.iWidth =   WEBC_SCREEN_WIDTH;
	clientDisplayBuffer.iBytesPerRow = clientDisplayBuffer.iWidth * BYTES_PER_PIXEL;
#define SPARE_FOR_BUGS 0
	int buffersize = (clientDisplayBuffer.iBytesPerRow) * (clientDisplayBuffer.iHeight + SPARE_FOR_BUGS); //  * 2;
	clientDisplayBuffer.pData = (WEBC_UINT8*)WEBC_MALLOC(buffersize);
	clientDisplayBuffer2 = clientDisplayBuffer;
	clientDisplayBuffer2.pData = (WEBC_UINT8*)WEBC_MALLOC(buffersize);
	m_displayBuffer = 0;
	memset((void*)clientDisplayBuffer.pData, 0,buffersize);
	memset((void*)clientDisplayBuffer2.pData, 0,buffersize);
	//END OS_SPECIFIC CODE
}

/**
*	Destroys os specific threads and screen displaying mechanism.
*/
TargetBitMapFrameBuffer::~TargetBitMapFrameBuffer(){
	delete(g);
	DestroyWindow(hWnd);
}


WEBC_UINT8 *TargetBitMapFrameBuffer::GetBaseAddress(WEBC_INT whichBuffer)
{
	return (WEBC_UINT8 *)(whichBuffer ? clientDisplayBuffer2.pData : clientDisplayBuffer.pData);
}


void TargetBitMapFrameBuffer::SwitchToBuffer(WEBC_INT whichBuffer)
{
	m_displayBuffer = whichBuffer;
}

WEBC_INT TargetBitMapFrameBuffer::GetNumberOfBuffers(void)
{
	return 2;
}

WEBC_INT TargetBitMapFrameBuffer::GetWidth(void)
{
	return HORIZONTAL_RESOLUTION;
}

WEBC_INT TargetBitMapFrameBuffer::GetHeight(void)
{
	return VERTICAL_RESOLUTION;
}

WEBC_INT TargetBitMapFrameBuffer::GetDepth(void)
{
	return COLOR_DEPTH;
}

WEBC_INT TargetBitMapFrameBuffer::GetRowBytes(void)
{
	return BYTES_PER_ROW;
}

/**
*	This method will take the internal display that WebC has been working with
*   and transform it to an understandable format for the target device.
*
*	Params: disp - internal webc bitmap for screen display, this bitmap represents what must be
*				   displayed to the screen device
*/
void TargetBitMapFrameBuffer::DoDisplay(void)
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
WebcGDIPlusBitmap* TargetBitMapFrameBuffer::convertBitmap(WebGraphBitmapInfo* info, DISPLAY_INT xPos, DISPLAY_INT yPos, DISPLAY_INT width, DISPLAY_INT height)
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
	WEBC_UINT8 *dstPtr;
	const WEBC_UINT8* srcPtr;
	DISPLAY_INT x, y;


	dstPtr = pData;

	srcPtr = (const WEBC_UINT8*) info->pData + (yPos * info->iBytesPerRow) + (xPos * 3);

	if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
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

	WebcGDIPlusBitmap* ret = (WebcGDIPlusBitmap*)WEBC_MALLOC(sizeof(WebcGDIPlusBitmap));
	ret->bitmap = new Bitmap((BITMAPINFO*)bmpHeader,(void*) pData);
	ret->bitmapData = buffer;
	if (ret->bitmap)
	{
		return(ret);
	}
	WEBC_FREE(buffer);
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
#include "webcstr.h"
// Alert box for Javascript.. (badly in need of rework)
extern "C" void webc_user_alert_box(char *str)
{
WEBC_CHAR UnicodeStr[512];
int i=0;
	while (*str) UnicodeStr[i++]=*str++;
  //  webc_c_strcpy (&UnicodeStr[0],str);
    MessageBox(0, (LPCTSTR) UnicodeStr, (LPCTSTR) L"webC Error", MB_OKCANCEL);
}


#endif /* WINDOWSGDI	*/
#endif // WIN32
