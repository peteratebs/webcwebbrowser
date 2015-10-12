/*
|  WEBCGDIP.CPP -
|
|  EBS -
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/


/*****************************************************************************/
/* Header files
 *****************************************************************************/
 // #error Fix double buffering for performance boost
#ifndef GDIFRAMEBUFFER

#define _WIN32_WINNT 0x0501
#include <stdio.h>
#define UNICODE
#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include "webc.h"
#include "webcgdip.h"
#include "wfgdip.h"
#include "webcmem.h"
#include "webrect.hpp"
#include "webcassert.h"
#include "webcstr.h"
#include "webcfg.h"
#include "webcLog.h"

#define RTP_MEM_DEBUG_PRINTF
//#define RTP_TRACK_LOCAL_MEMORY
#define RTP_MEM_RESTRAIN 0


#define USE_DRIVER_METHODS 0


extern int giAllowTimerTick;
static int g_depthVal = 0;

void wcProcessTimedCallbacks(void);

/*****************************************************************************/
/* Macros
 *****************************************************************************/

#if !defined(WEBC_BUFFER_SCROLLING)
#define GDIPLUS_DOUBLE_BUFFER
#endif

#define GET_GRAPHICS_OBJ(C)   (((WebcGDIPlusGraphics*)(C))->outputGraphics)
#define GET_WINDOW_HANDLE(C)  (((WebcGDIPlusGraphics*)(C))->windowHandle)

#define RECT_TO_HTML(H,R)     ((H).left  = (R).X,  (H).top   = (R).Y, \
                               (H).right = (R).X + (R).Width - 1, (H).bottom = (R).Y + (R).Height - 1)
#define HTML_TO_RECT(R,H)     Rect R((H).left, (H).top, (H).right - (H).left + 1, (H).bottom - (H).top + 1);

#define XY_TO_POINT(P,X,Y)    PointF P((REAL) X, (REAL) Y)

#define GDI_BITMAP_ROW_ALIGN  4

/*****************************************************************************/
/* Types
 *****************************************************************************/

typedef struct S_WebcGDIPlusGraphics
{
	HTMLGraphicsContext* ctx;
	struct S_WebcGDIPlusGraphics  *pnext;
    void                 *privatedata;
    HBROWSER_HANDLE      BrowserHandle;
	HWND                 windowHandle;
	Graphics*            outputGraphics;
	Graphics*            windowGraphics;
  #if defined(GDIPLUS_DOUBLE_BUFFER) || defined(WEBC_BUFFER_SCROLLING)
	Bitmap*              bufferImage;
	Graphics*            bufferGraphics;
  #endif // GDIPLUS_DOUBLE_BUFFER
  #ifdef WEBC_BUFFER_SCROLLING
	Bitmap*              scratchBitmap;
	Graphics*            scratchGraphics;
	WebRect              scratchContents;
  #endif
	WebRect              invalidRect;
	int                  drawNesting;
	WEBC_UINT8           blendLevelInt;
	REAL                 blendLevelReal;
}
WebcGDIPlusGraphics;


typedef struct
{
	Bitmap*              bitmap;
	CachedBitmap*        cachedBitmap;
	WEBC_UINT8*          bitmapData;
}
WebcGDIPlusBitmap;

/*****************************************************************************/
/* Function Prototypes
/*****************************************************************************/

static int GDIPlus_ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	);

/*****************************************************************************/
/* Data
 *****************************************************************************/

HTMLGraphicsContext webcGDIPlusGraphicsClass = {0};

int (*gdiplusProcessInputQueueFn) (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode) = GDIPlus_ProcessInputQueue;

#define WG_NUM_GDIPLUS_KEYS 32

static WEBC_UINT32 gdiplusKeyData [WG_NUM_GDIPLUS_KEYS*2] =
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

static WebcKeyMap gdiplusKeyMap;
static WEBC_BOOL  gdiplusKeyMapInitialized = WEBC_FALSE;

#define MAX_LOADSTRING 100
extern HINSTANCE hInst;								// current instance
extern TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name


/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

void webc_InitGDIPlus(void) // not called
{
}

void GDIPlus_ScreenInfo (HTMLGraphicsContext** ctx, HTMLScreenInfo *pinfo)
{
    pinfo->availHeight = GetSystemMetrics(SM_CYFULLSCREEN);
    pinfo->availWidth  = GetSystemMetrics(SM_CXFULLSCREEN);
    pinfo->height      = GetSystemMetrics(SM_CYFULLSCREEN);
    pinfo->width       = GetSystemMetrics(SM_CXFULLSCREEN);
    pinfo->colorDepth  = 24;
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

int GDIPlus_WindowOpen (
		HTMLGraphicsContext** ctx,
		HTMLBrowserConfig *pconfig,
        const WebChar *title
        )
{
	DWORD dwStyle;
	int x,y,width,height;
	HWND hWndParent = NULL;

    /* User supplied parameters */
    x = pconfig->WindowConfig.top;
    y = pconfig->WindowConfig.left;
    height =pconfig->WindowConfig.height;
    width = pconfig->WindowConfig.width;

    /* If height or width are 0, use defauts */
    if (height == 0 || width == 0)
    {
        dwStyle = WS_OVERLAPPED;
        x = CW_USEDEFAULT;
        y = 0;
        width = CW_USEDEFAULT;;
        height = 0;
    }
    else
    {
		dwStyle = WS_OVERLAPPEDWINDOW;
//		dwStyle = WS_POPUP;
        // dwStyle = WS_OVERLAPPED;
//        x = CW_USEDEFAULT;
//        y = 0;
    }

    if (pconfig->WindowConfig.is_resizable)
        dwStyle |= WS_THICKFRAME;


LPCTSTR lpWindowName = (LPCTSTR) pconfig->WindowTitle;
    if (!lpWindowName)
        lpWindowName        = L"WebC";
LPCTSTR lpWindowClass = szWindowClass;

    HWND NewWindowHandle = CreateWindow(lpWindowClass,
	    0,// lpWindowName,
	    dwStyle,
		x,
		y,
		width,
		height,
		hWndParent,
		NULL,
		hInst,
		NULL);

	if (!NewWindowHandle)
		return -1;
//	if (!ghWndParent)
//		ghWndParent=NewWindowHandle;
	SetClientSize( NewWindowHandle, width, height );
#if (WEBC_SUPPORT_SMALL_SCREEN)
// Positioning code
		SetWindowPos(NewWindowHandle,HWND_TOP, 0, 0, Rect.right-Rect.left, Rect.bottom-Rect.top, SWP_SHOWWINDOW);
#endif
     if (webc_InitGDIPlusBrowserConfig(pconfig, NewWindowHandle) >=0)
     {
        // Set WebcGDIPlusGraphics Gcs BrowserHandle filed
        ((WebcGDIPlusGraphics*)pconfig->graphics)->BrowserHandle = pconfig->BrowserHandle;
        // Bind the handle to the browser. Also required in config structure, for backwards compatibility
		pconfig->WindowHandle = (void *) NewWindowHandle;
        webc_SetBrowserWindow (pconfig->BrowserHandle, (void *) NewWindowHandle);

		ShowWindow(NewWindowHandle, SW_SHOW);
		if ((pconfig->WindowConfig.WindowFlags & WF_STARTINBG)!=0)
			SetWindowPos( NewWindowHandle, HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		else
			SetWindowPos( NewWindowHandle, HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

//		if ((pconfig->WindowConfig.WindowFlags & WF_STARTINBG)!=0)
//		{
//			HWND LowerWindowHandle =GetNextWindow(NewWindowHandle, GW_HWNDNEXT);
//			if (LowerWindowHandle)
//			{
//				BringWindowToTop(LowerWindowHandle);
//			}
//		}


        SetTimer(NewWindowHandle, 1, 50, NULL);
//        /* Experimental */
//		if (pconfig->WindowConfig.WindowFlags.is_modal)
//            SetCapture(NewWindowHandle);

        return(0);
     }

     return(-1);
}

static void GDIPlus_WindowClose (       /* Called */
		HTMLGraphicsContext** ctx
        )
{
        // The browser destructor called us. Unregister gc and destroy underlying window
	    HWND hWnd = GET_WINDOW_HANDLE(ctx);
		WEBC_FREE((WebcGDIPlusGraphics*)ctx);
		DestroyWindow(hWnd);
}
/*---------------------------------------------------------------------------*/
// Put the current window on top. Called when the browser focus changes.
void guiWmBringToTop(HTMLGraphicsContext** ctx)
{
	BringWindowToTop(GET_WINDOW_HANDLE(ctx));
}

void guiWmGetPos(HTMLGraphicsContext** ctx,int *left, int *top, int *width, int *height)
{
    RECT rcWindow;
    if (GetWindowRect(GET_WINDOW_HANDLE(ctx), &rcWindow))
    {
		*left = rcWindow.left;
		*top = rcWindow.top;
        *width =  (rcWindow.right - rcWindow.left);
        *height = (rcWindow.bottom - rcWindow.top);
	}
	else
		*left = *top = *width = *height = 0;
}
void guiWmSetPos(HTMLGraphicsContext** ctx, int left, int top, int width, int height)
{
	MoveWindow(GET_WINDOW_HANDLE(ctx),left, top, width, height, 1); // 1 is repaint.
}


/*---------------------------------------------------------------------------*/
void GDIPlus_MoveByorTo (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
        WEBC_BOOL is_absolute
        )
{
    RECT rcWindow;

    if (GetWindowRect(GET_WINDOW_HANDLE(ctx), &rcWindow))
    {
        int width =  (rcWindow.right - rcWindow.left);
        int height = (rcWindow.bottom - rcWindow.top);
        if (!is_absolute)
        {
            x += rcWindow.right;
            y += rcWindow.top;
        }
        MoveWindow(GET_WINDOW_HANDLE(ctx),x, y, width, height, 1); // 1 is repaint.
    }
}


/*---------------------------------------------------------------------------*/
static void GDIPlus_SetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	HTML_TO_RECT(gdiRect, *rect);

	GET_GRAPHICS_OBJ(ctx)->SetClip(gdiRect, CombineModeReplace);
}


/*---------------------------------------------------------------------------*/
static void GDIPlus_GetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	Rect clipRect;
	GET_GRAPHICS_OBJ(ctx)->GetClipBounds(&clipRect);
	RECT_TO_HTML(*rect, clipRect);
}


/*---------------------------------------------------------------------------*/
static void GDIPlus_GetCanvasRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	RECT winRect;

	GetClientRect(GET_WINDOW_HANDLE(ctx), &winRect);

	rect->left = 0;
	rect->top = 0;
	rect->right = winRect.right - winRect.left - 1;
	rect->bottom = winRect.bottom - winRect.top - 1;
}

/*---------------------------------------------------------------------------*/
static void GDIPlus_DrawRectangle (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor outlineColor,
		WebColor fillColor,
		WEBC_BOOL fill
	)
{
	Pen outlinePen(outlineColor, 1);
	SolidBrush fillBrush(fillColor);
	HTML_TO_RECT(drawRect, *rect);


	if (fill)
	{
		GET_GRAPHICS_OBJ(ctx)->FillRectangle(&fillBrush, drawRect);
	}

	if (!fill || outlineColor != fillColor)
	{
		// who knows why this is necessary... but it is.
		drawRect.Width--;
		drawRect.Height--;
		GET_GRAPHICS_OBJ(ctx)->DrawRectangle(&outlinePen, drawRect);
	}
}


/*---------------------------------------------------------------------------*/
static void GDIPlus_DrawBitmap (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		void* bmp
	)
{
	WebcGDIPlusGraphics* g = (WebcGDIPlusGraphics*) ctx;
	WebcGDIPlusBitmap* wcBmp = (WebcGDIPlusBitmap*) bmp;

	if (wcBmp->cachedBitmap && g->blendLevelInt == 255)
	{
		GET_GRAPHICS_OBJ(ctx)->DrawCachedBitmap(wcBmp->cachedBitmap, x, y);
	}
	else
	{
		ImageAttributes imgAttrib;
		ColorMatrix colorMatrix =
		{{
			{1.0,0.0,0.0,0.0,0.0},
			{0.0,1.0,0.0,0.0,0.0},
			{0.0,0.0,1.0,0.0,0.0},
			{1.0,1.0,1.0,0.0,0.0},
			{0.0,0.0,0.0,0.0,1.0},
		}};


		if (g->blendLevelReal != 1.0)
		{
			colorMatrix.m[3][3] = g->blendLevelReal;
			imgAttrib.SetColorMatrix(&colorMatrix);
		}

		Rect destRect(x,y, wcBmp->bitmap->GetWidth(), wcBmp->bitmap->GetHeight());
		GET_GRAPHICS_OBJ(ctx)->DrawImage (
					wcBmp->bitmap,
					destRect,
					0,
					0,
					wcBmp->bitmap->GetWidth(),
					wcBmp->bitmap->GetHeight(),
					UnitPixel,
					&imgAttrib
				);
	}
}

/*---------------------------------------------------------------------------*/
static void GDIPlus_DrawStretchedBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		void* b
	)
{
	WebcGDIPlusBitmap* wcBmp = (WebcGDIPlusBitmap*) b;

	GET_GRAPHICS_OBJ(ctx)->DrawImage (
				wcBmp->bitmap,
				rect->left,
				rect->top,
				rect->right - rect->left + 1,
				rect->bottom - rect->top + 1
			);
}

/*---------------------------------------------------------------------------*/
static void* GDIPlus_CreateStretchedBitmap (
		HTMLGraphicsContext** ctx,
		void* b,
		DISPLAY_INT w,
		DISPLAY_INT h
	)
{
	WebcGDIPlusBitmap* wcBmp = (WebcGDIPlusBitmap*) b;
	Bitmap* stretchedBmp = new Bitmap(w, h, GET_GRAPHICS_OBJ(ctx));
	if (stretchedBmp)
	{
		Graphics* imageGraphics = Graphics::FromImage(stretchedBmp);
		if (imageGraphics)
		{
			REAL srcW = (REAL) wcBmp->bitmap->GetWidth();
			REAL srcH = (REAL) wcBmp->bitmap->GetHeight();
			REAL dstW = (REAL) w;
			REAL dstH = (REAL) h;

			if (srcW * srcH > 20)
			{
				imageGraphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
			}
			else
			{
				imageGraphics->SetInterpolationMode(InterpolationModeNearestNeighbor);
			}

			// fix for strange GDI+ interpolation effect on small images
			REAL x = (srcW == 1)? 0 : ((dstW < srcW)? 1 : (dstW / (srcW - 1)));
			REAL y = (srcW == 1)? 0 : ((dstH < srcH)? 1 : (dstH / (srcH - 1)));

			dstW = (REAL) ((srcW == 1)? (dstW * 256.0) : (dstW + 2.0 * x));
			dstH = (REAL) ((srcH == 1)? (dstH * 256.0) : (dstH + 2.0 * y));

			RectF dstRect((REAL) -x, (REAL) -y, (REAL) dstW, (REAL) dstH);

			imageGraphics->DrawImage(wcBmp->bitmap, dstRect, 0, 0, (REAL) srcW, (REAL) srcH, UnitPixel);
			delete imageGraphics;

			WebcGDIPlusBitmap* stretched = (WebcGDIPlusBitmap*) WEBC_MALLOC(sizeof(WebcGDIPlusBitmap));
			if (stretched)
			{
				stretched->bitmap = stretchedBmp;
				stretched->bitmapData = 0;
#ifdef GDIPLUS_DOUBLE_BUFFER

				if(32 == g_depthVal)
				{
					stretched->cachedBitmap = new CachedBitmap(stretchedBmp, GET_GRAPHICS_OBJ(ctx));
				}
				else
				{
					// CachedBitmap's don't work with offscreen bitmaps when the color depth is not 24 bit
					stretched->cachedBitmap = 0;
				}
#else
				stretched->cachedBitmap = new CachedBitmap(stretchedBmp, GET_GRAPHICS_OBJ(ctx));
#endif
				return (stretched);
			}
		}

		delete stretchedBmp;
	}

	return 0;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDIPlus_GetBitmapWidth (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	return ((WebcGDIPlusBitmap*) bmp)->bitmap->GetWidth();
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDIPlus_GetBitmapHeight (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	return ((WebcGDIPlusBitmap*) bmp)->bitmap->GetHeight();
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDIPlus_GetFontHeight (
		HTMLGraphicsContext** ctx,
		void* f
	)
{
	Font* font = (Font*) f;
	Font altFont(L"Arial Unicode MS", font->GetSize(), font->GetStyle(), font->GetUnit());

	DISPLAY_INT fontHeight = (DISPLAY_INT) font->GetHeight(GET_GRAPHICS_OBJ(ctx));
	DISPLAY_INT altFontHeight = (DISPLAY_INT) altFont.GetHeight(GET_GRAPHICS_OBJ(ctx));

	return (fontHeight > altFontHeight)? fontHeight : altFontHeight;
}

/*---------------------------------------------------------------------------*/
#if (!WEBC_SUPPORT_UNICODE)
unsigned char unicode_text_buffer[2048];
long text_to_unicode(const WebChar* _text, long textLen)
{
long i;
	if (textLen < 0)
		textLen = webc_strlen(_text);
    for (i = 0; i < textLen; i++)
    {
        unicode_text_buffer[i*2] = *_text++;
        unicode_text_buffer[(i*2)+1] = 0;
    }
    unicode_text_buffer[i*2] = *_text++;
    unicode_text_buffer[(i*2)+1] = 0;
    return(textLen);

}

#endif
static void _GDIPlus_MeasureTextWithFallback (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		const WebChar* _text,
		long textLen,
		void* f,
		RectF* rect
	)
{
	Font* font = (Font*) f;
	Font altFont(L"Arial Unicode MS", font->GetSize(), font->GetStyle(), font->GetUnit());
	UINT16 *text = (UINT16 *) _text;
	StringFormat genericStringFormat(StringFormat::GenericTypographic());
	genericStringFormat.SetTrimming(StringTrimmingNone);

#if (!WEBC_SUPPORT_UNICODE)
    textLen = text_to_unicode(_text, textLen);
	text = (UINT16 *) &unicode_text_buffer[0];
#endif

	XY_TO_POINT(where, x, y);
	const UINT16 * segmentStart = text;
	long segmentLength = 0;
	WEBC_BOOL firstSegment = WEBC_TRUE;
	RectF tempRect;

#if (WEBC_SUPPORT_UNICODE)
	if (textLen < 0)
	{
		textLen = webc_strlen(text);
	}
#endif

	while (segmentLength + segmentStart < text + textLen)
	{
		for (segmentLength = 0;
			 segmentLength + segmentStart < text + textLen &&
			 (segmentStart[segmentLength] & 0xff80) == 0;
		     segmentLength++)
		{;}

		if (segmentLength > 0)
		{
			// measure non-unicode portion
			if (firstSegment)
			{
				firstSegment = WEBC_FALSE;
#if USE_DRIVER_METHODS
				GET_GRAPHICS_OBJ(ctx)->MeasureDriverString (
						(const UINT16 *) segmentStart,
						segmentLength,
						font,
						&where,
						DriverStringOptionsRealizedAdvance|DriverStringOptionsCmapLookup,
						NULL,
						rect
					);
#else
				GET_GRAPHICS_OBJ(ctx)->MeasureString((const WCHAR *)segmentStart,
						segmentLength,
						font,
						where,
						&genericStringFormat,
						rect);
#endif
			}
			else
			{
#if USE_DRIVER_METHODS
				GET_GRAPHICS_OBJ(ctx)->MeasureDriverString (
						(const UINT16 *)segmentStart,
						segmentLength,
						font,
						&where,
						DriverStringOptionsRealizedAdvance|DriverStringOptionsCmapLookup,
						NULL,
						&tempRect
					);
#else
				GET_GRAPHICS_OBJ(ctx)->MeasureString((const WCHAR *)segmentStart,
						segmentLength,
						font,
						where,
						&genericStringFormat,
						&tempRect);
#endif
				rect->Width += tempRect.Width;
				if (tempRect.Height > rect->Height)
				{
					rect->Height = tempRect.Height;
				}
			}

			segmentStart += segmentLength;
			segmentLength = 0;
		}

		for (segmentLength = 0;
			 segmentLength + segmentStart < text + textLen &&
			 (segmentStart[segmentLength] & 0xff80) != 0;
		     segmentLength++)
		{;}

		if (segmentLength > 0)
		{
			// measure unicode portion
			if (firstSegment)
			{
				firstSegment = WEBC_FALSE;
#if USE_DRIVER_METHODS
				GET_GRAPHICS_OBJ(ctx)->MeasureDriverString (
						(const UINT16 *)segmentStart,
						segmentLength,
						&altFont,
						&where,
						DriverStringOptionsRealizedAdvance|DriverStringOptionsCmapLookup,
						NULL,
						rect
					);
#else
				GET_GRAPHICS_OBJ(ctx)->MeasureString((const WCHAR *)segmentStart,
						segmentLength,
						&altFont,
						where,
						&genericStringFormat,
						rect);
#endif
			}
			else
			{
#if USE_DRIVER_METHODS
				GET_GRAPHICS_OBJ(ctx)->MeasureDriverString (
						(const UINT16 *)segmentStart,
						segmentLength,
						&altFont,
						&where,
						DriverStringOptionsRealizedAdvance|DriverStringOptionsCmapLookup,
						NULL,
						&tempRect
					);
#else
				GET_GRAPHICS_OBJ(ctx)->MeasureString((const WCHAR *)segmentStart,
						segmentLength,
						&altFont,
						where,
						&genericStringFormat,
						&tempRect);
#endif
				rect->Width += tempRect.Width;
				if (tempRect.Height > rect->Height)
				{
					rect->Height = tempRect.Height;
				}
			}

			segmentStart += segmentLength;
			segmentLength = 0;
		}
	}
}

/*---------------------------------------------------------------------------*/
static void _GDIPlus_DrawStringSegment (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		const UINT16 * segmentStart,
		long segmentLength,
		Font* font,
		REAL* xOffset,
		Brush* textBrush,
		Brush* fillBrush,
		WEBC_BOOL doFill,
		REAL fontHeight
	)
{
	XY_TO_POINT(where, x, y);
	RectF tempRect;
	StringFormat genericStringFormat(StringFormat::GenericTypographic());
	genericStringFormat.SetTrimming(StringTrimmingNone);

#if USE_DRIVER_METHODS
	// draw non-unicode portion
	GET_GRAPHICS_OBJ(ctx)->MeasureDriverString (
			segmentStart,
			segmentLength,
			font,
			&where,
			DriverStringOptionsRealizedAdvance|DriverStringOptionsCmapLookup,
			NULL,
			&tempRect
		);
#else
GET_GRAPHICS_OBJ(ctx)->MeasureString((const WCHAR *)segmentStart,
			segmentLength,
			font,
			where,
			&genericStringFormat,
			&tempRect);

#endif
	where.X += *xOffset;
	where.Y += y - tempRect.Y;

	if (doFill)
	{
		tempRect.X = where.X;
		tempRect.Y = (REAL) y;
		tempRect.Height = fontHeight;

		GET_GRAPHICS_OBJ(ctx)->FillRectangle(fillBrush, tempRect);
	}
#if USE_DRIVER_METHODS
	GET_GRAPHICS_OBJ(ctx)->DrawDriverString (
			(const UINT16 *)segmentStart,
			segmentLength,
			font,
			textBrush,
			&where,
			DriverStringOptionsRealizedAdvance|DriverStringOptionsCmapLookup,
			NULL
		);
#else
		XY_TO_POINT(stringPoint, x + *xOffset, y);

		GET_GRAPHICS_OBJ(ctx)->DrawString((const WCHAR *)segmentStart,
			segmentLength,
			font,
			stringPoint,
			&genericStringFormat,
			textBrush);
#endif

	*xOffset += tempRect.Width;
}

/*---------------------------------------------------------------------------*/
static void _GDIPlus_DrawTextWithFallback (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		const WebChar* _text,
		long textLen,
		void* f,
		WebColor textColor,
		WebColor fillColor,
		WEBC_BOOL fill
	)
{
	Font* font = (Font*) f;
	Font altFont(L"Arial Unicode MS", font->GetSize(), font->GetStyle(), font->GetUnit());
	UINT16 * text = (UINT16 *) _text;

#if (!WEBC_SUPPORT_UNICODE)
    textLen = text_to_unicode(_text, textLen);
	text = (UINT16 *) &unicode_text_buffer[0];
#endif

	const UINT16 * segmentStart = text;
	long segmentLength = 0;
	RectF tempRect;
	SolidBrush textBrush(textColor);
	SolidBrush fillBrush(fillColor);
	REAL xOffset = 0;
	REAL fontHeight = (REAL) GDIPlus_GetFontHeight(ctx, f);

#if (WEBC_SUPPORT_UNICODE)
	if (textLen < 0)
	{
		textLen = webc_strlen(text);
	}
#endif
	while (segmentLength + segmentStart < text + textLen)
	{
		for (segmentLength = 0;
			 segmentLength + segmentStart < text + textLen &&
			 (segmentStart[segmentLength] & 0xff80) == 0;
		     segmentLength++) {;}

		if (segmentLength > 0)
		{
			_GDIPlus_DrawStringSegment (
					ctx, x, y, segmentStart, segmentLength, font,
					&xOffset, &textBrush, &fillBrush, fill, fontHeight
				);

			segmentStart += segmentLength;
			segmentLength = 0;
		}

		for (segmentLength = 0;
			 segmentLength + segmentStart < text + textLen &&
			 (segmentStart[segmentLength] & 0xff80) != 0;
		     segmentLength++) {;}

		if (segmentLength > 0)
		{
			_GDIPlus_DrawStringSegment (
					ctx, x, y, segmentStart, segmentLength, &altFont,
					&xOffset, &textBrush, &fillBrush, fill, fontHeight
				);

			segmentStart += segmentLength;
			segmentLength = 0;
		}
	}
}

/*---------------------------------------------------------------------------*/
static void GDIPlus_DrawText (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		const WebChar* text,
		WebColor textColor,
		WebColor fillColor,
		WEBC_BOOL fill,
		void* f
	)
{
	Font* font = (Font*) f;
	Font altFont(L"Arial Unicode MS", font->GetSize(), font->GetStyle(), font->GetUnit());

	if (fill)
	{
		//SolidBrush fillBrush(fillColor);
		//drawRect.Y = (REAL) y;
		//GET_GRAPHICS_OBJ(ctx)->FillRectangle(&fillBrush, drawRect);
	}
	_GDIPlus_DrawTextWithFallback(ctx, x, y, text, -1, f, textColor, fillColor, fill);
}

/*---------------------------------------------------------------------------*/
static void GDIPlus_DrawTextLen (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		const WebChar* text,
		WebColor textColor,
		WebColor fillColor,
		WEBC_BOOL fill,
		void* f,
		long textLen
	)
{
	_GDIPlus_DrawTextWithFallback(ctx, x, y, text, textLen, f, textColor, fillColor, fill);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDIPlus_GetTextHeight (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* f
	)
{
	RectF bounds;
	_GDIPlus_MeasureTextWithFallback(ctx, 0, 0, text, -1, f, &bounds);
	return ((DISPLAY_INT) bounds.Height);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDIPlus_GetTextHeightLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* f,
		long textLen
	)
{
	RectF bounds;
	_GDIPlus_MeasureTextWithFallback(ctx, 0, 0, text, textLen, f, &bounds);

	return ((DISPLAY_INT) bounds.Height);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDIPlus_GetTextWidth (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* f
	)
{
	RectF bounds;
	_GDIPlus_MeasureTextWithFallback(ctx, 0, 0, text, -1, f, &bounds);
	return ((DISPLAY_INT) bounds.Width);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDIPlus_GetTextWidthLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* f,
		long textLen
	)
{
	RectF bounds;
	_GDIPlus_MeasureTextWithFallback(ctx, 0, 0, text, textLen, f, &bounds);
	return ((DISPLAY_INT) bounds.Width);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDIPlus_GetFontBaseline (
		HTMLGraphicsContext** ctx,
		void* f
	)
{
	/*
	WebChar emStr[] = {'M',0};
	return GDIPlus_GetTextHeight(ctx, emStr, f);
	*/
	Font* font = (Font*) f;
	FontFamily fontFamily;
	font->GetFamily(&fontFamily);

	return (DISPLAY_INT) ((font->GetHeight(GET_GRAPHICS_OBJ(ctx)) * fontFamily.GetCellAscent(font->GetStyle())) /
	                        fontFamily.GetEmHeight(font->GetStyle())) - 1;
}

/*---------------------------------------------------------------------------*/
static WebColor GDIPlus_RGBToColor (
		HTMLGraphicsContext** ctx,
		HTMLColor color,
		HTMLColor* error
	)
{
	if (*error)
	{
		*error = 0;
	}
	return (0xff000000 - (color & HTML_COLOR_ALPHA_MASK)) | (color & ~HTML_COLOR_ALPHA_MASK);
}

/*---------------------------------------------------------------------------*/
static HTMLColor GDIPlus_ColorToRGB (
		HTMLGraphicsContext** ctx,
		WebColor colorval
	)
{
	return (0xff000000 - (colorval & HTML_COLOR_ALPHA_MASK)) | (colorval & ~HTML_COLOR_ALPHA_MASK);
}

/*---------------------------------------------------------------------------*/
static void GDIPlus_InvalidateRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	WebcGDIPlusGraphics* g = (WebcGDIPlusGraphics*) ctx;

	g->invalidRect.Or((WebRect*) rect);
}

/*---------------------------------------------------------------------------*/
static void GDIPlus_BeginDrawing (HTMLGraphicsContext** ctx)
{
	WebcGDIPlusGraphics* g = (WebcGDIPlusGraphics*) ctx;

	if (g->drawNesting == 0)
	{
		WebRect canvasRect;
		GDIPlus_GetCanvasRect(ctx, (HTMLRect*) &canvasRect);

	  #ifdef GDIPLUS_DOUBLE_BUFFER
		Graphics* nwg = new Graphics(g->windowHandle);
		if (nwg)
		{
			delete g->windowGraphics;
			g->windowGraphics = nwg;
		}

		g->bufferImage = new Bitmap(canvasRect.Width(), canvasRect.Height(), g->windowGraphics);
		if (g->bufferImage)
		{
			g->bufferGraphics = Graphics::FromImage(g->bufferImage);
			if (g->bufferGraphics)
			{
				g->outputGraphics = g->bufferGraphics;
			}
			else
			{
				delete g->bufferImage;
				g->bufferImage = 0;
			}
		}
	  #endif // GDIPLUS_DOUBLE_BUFFER
	}

	g->drawNesting++;
}

/*---------------------------------------------------------------------------*/
static void GDIPlus_EndDrawing (HTMLGraphicsContext** ctx)
{
	WebcGDIPlusGraphics* g = (WebcGDIPlusGraphics*) ctx;

	WEBC_ASSERT(g->drawNesting > 0);

	g->drawNesting--;
	if (g->drawNesting == 0)
	{
	  #ifdef GDIPLUS_DOUBLE_BUFFER
		// copy the off-screen buffer to the window and clean up
		if (g->bufferImage && g->bufferGraphics)
		{
			g->windowGraphics->DrawImage (
					g->bufferImage,           // image
					g->invalidRect.left,      // x
					g->invalidRect.top,       // y
					g->invalidRect.left,      // srcX
					g->invalidRect.top,       // srcY
					g->invalidRect.Width(),   // srcWidth
					g->invalidRect.Height(),  // srcHeight
					UnitPixel                 // srcUnit
				);

			delete g->bufferGraphics;
			delete g->bufferImage;

			g->bufferGraphics = 0;
			g->bufferImage = 0;
			g->outputGraphics = g->windowGraphics;
		}
	  #endif // GDIPLUS_DOUBLE_BUFFER

		g->invalidRect.Set(0,0,0,0);
	}
}

/*---------------------------------------------------------------------------*/
static void GDIPlus_TileBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect *rect,
		DISPLAY_INT xOffset,
		DISPLAY_INT yOffset,
		void* bmp
	)
{
	TextureBrush fillBrush(((WebcGDIPlusBitmap*)bmp)->bitmap);
	HTML_TO_RECT(dstRect, *rect);

	Rect srcRect(xOffset, yOffset, xOffset + rect->right - rect->left + 1, yOffset + rect->bottom - rect->top + 1);

	GraphicsContainer container;
	container = GET_GRAPHICS_OBJ(ctx)->BeginContainer(dstRect, srcRect, UnitPixel);

	GET_GRAPHICS_OBJ(ctx)->FillRectangle(&fillBrush, srcRect);

	// End the container.
	GET_GRAPHICS_OBJ(ctx)->EndContainer(container);
}

/*---------------------------------------------------------------------------*/
static void GDIPlus_DrawPatternFrame (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor color,
		DISPLAY_INT thickness,
		CSSBorderStyle pattern
	)
{
	HTML_TO_RECT(drawRect, *rect);
	HatchBrush penBrush(HatchStyle50Percent, color, Color::Transparent);
	Pen outlinePen(&penBrush, (REAL) thickness);

	drawRect.Width--;
	drawRect.Height--;
	GET_GRAPHICS_OBJ(ctx)->DrawRectangle(&outlinePen, drawRect);
}

/*---------------------------------------------------------------------------*/
static void* GDIPlus_CreateWebBitmap (
		HTMLGraphicsContext** ctx,
		const WebGraphBitmapInfo* info
	)
{
	int paletteSizeNeeded = (info->pixelFormat == WB_PIXEL_FORMAT_INDEX_8)? info->iPaletteSize : 0;
	WEBC_SIZE dstBytesPerRow = (info->iBytesPerRow + (GDI_BITMAP_ROW_ALIGN-1)) & ~(GDI_BITMAP_ROW_ALIGN-1);
	size_t dataSize = info->iHeight * dstBytesPerRow;

	WEBC_UINT8* buffer = (WEBC_UINT8*) WEBC_MALLOC (sizeof(BITMAPINFOHEADER) +
		                                            sizeof(RGBQUAD) * paletteSizeNeeded +
		                                            dataSize);

	if (!buffer)
	{
		return (0);
	}

	BITMAPINFOHEADER* bmpHeader = (BITMAPINFOHEADER*) buffer;

	static int bitCountMap[] =
	{
		24, // WB_PIXEL_FORMAT_RGB_24       = 0,  // 24bit color, stored red-green-blue
		8,  // WB_PIXEL_FORMAT_INDEX_8      = 1,  // palette indexed, 8 bits per pixel
		8,  // WB_PIXEL_FORMAT_MONOCHROME_8 = 2,  // monochrome, 8 bits per pixel
		32, // WB_PIXEL_FORMAT_RGBA_32
#if (WEBC_SUPPORT_BMP_16BITS)
		16,
#endif
	};

	bmpHeader->biSize = sizeof(BITMAPINFOHEADER);
	bmpHeader->biWidth = info->iWidth;
	bmpHeader->biHeight = 0 - info->iHeight; // negative means top-down
	bmpHeader->biPlanes = 1;
	bmpHeader->biBitCount = bitCountMap[info->pixelFormat];
	bmpHeader->biCompression = BI_RGB;
	bmpHeader->biSizeImage = 0;
	bmpHeader->biXPelsPerMeter = 72 * 39; // 72dpi * 39 inches/meter
	bmpHeader->biYPelsPerMeter = 72 * 39; // 72dpi * 39 inches/meter
	bmpHeader->biClrUsed = paletteSizeNeeded;
	bmpHeader->biClrImportant = 0;

	RGBQUAD* bmpColors = (RGBQUAD*) (buffer + bmpHeader->biSize);
	int n;
	for (n = 0; n < paletteSizeNeeded; n++)
	{
		bmpColors[n].rgbRed   = info->pPalette[n + (n<<1) + 0];
		bmpColors[n].rgbGreen = info->pPalette[n + (n<<1) + 1];
		bmpColors[n].rgbBlue  = info->pPalette[n + (n<<1) + 2];
		bmpColors[n].rgbReserved = 0;
	}

	WEBC_UINT8* pData = buffer + (sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * paletteSizeNeeded);
	WEBC_UINT8* dstPtr = pData;
	const WEBC_UINT8* srcPtr = info->pData;
	DISPLAY_INT x, y;

	if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
	{
		WEBC_UINT8* rowPtr = dstPtr;

		for (y = 0; y < info->iHeight; y++, rowPtr += dstBytesPerRow)
		{
			dstPtr = rowPtr;
			for (x = 0; x < info->iWidth; x++, srcPtr += 3, dstPtr += 3)
			{
				dstPtr[0] = srcPtr[2];
				dstPtr[1] = srcPtr[1];
				dstPtr[2] = srcPtr[0];
			}
		}
	}
	else if (info->pixelFormat == WB_PIXEL_FORMAT_RGBA_32)
	{
		WEBC_UINT8* rowPtr = dstPtr;

		for (y = 0; y < info->iHeight; y++, rowPtr += dstBytesPerRow)
		{
			dstPtr = rowPtr;
			for (x = 0; x < info->iWidth; x++, srcPtr += 4, dstPtr += 4)
			{
				dstPtr[0] = srcPtr[2];
				dstPtr[1] = srcPtr[1];
				dstPtr[2] = srcPtr[0];
				dstPtr[3] = srcPtr[3];
			}
		}
	}
#if (WEBC_SUPPORT_BMP_16BITS)
	else if(info->pixelFormat == WB_PIXEL_FORMAT_RGB_16)
	{
			WEBC_UINT8* rowPtr = dstPtr;
			for (y = 0; y < info->iHeight; y++, rowPtr += dstBytesPerRow)
			{
				for (x = 0; x < info->iWidth; x++, srcPtr += 2, dstPtr += 2)
				{
					dstPtr[1]=srcPtr[0];
					dstPtr[0]=srcPtr[1];
				}
			}

	}
#endif
	else
	{
		for (y = 0; y < info->iHeight; y++, dstPtr += dstBytesPerRow, srcPtr += info->iBytesPerRow)
		{
			rtp_memcpy(dstPtr, srcPtr, info->iBytesPerRow);
		}
	}

	WebcGDIPlusBitmap* newBitmap = (WebcGDIPlusBitmap*) WEBC_MALLOC(sizeof(WebcGDIPlusBitmap));
	if (newBitmap)
	{
		if(info->pixelFormat == WB_PIXEL_FORMAT_RGBA_32)
			newBitmap->bitmap = new Bitmap(info->iWidth, info->iHeight, bitCountMap[info->pixelFormat] / 8 * info->iWidth, PixelFormat32bppARGB, (BYTE *)(pData));
		else
			newBitmap->bitmap = new Bitmap((BITMAPINFO*) bmpHeader, (void*) pData);

		if (newBitmap->bitmap)
		{
			if (info->bHasTransparentColor)
			{
				Bitmap* alphaBitmap = newBitmap->bitmap->Clone(0, 0, info->iWidth, info->iHeight, PixelFormat32bppPARGB);
				if (alphaBitmap)
				{
					delete newBitmap->bitmap;
					newBitmap->bitmap = alphaBitmap;
				}

				switch (info->pixelFormat)
				{
					case WB_PIXEL_FORMAT_INDEX_8:
					{
						Status result;
						const WEBC_UINT8* pixelPtr = info->pData;
						for (y = 0; y < info->iHeight; y++)
						{
							for (x = 0; x < info->iWidth; x++, pixelPtr++)
							{
								if (*pixelPtr == info->iTransparentIndex)
								{
									result = newBitmap->bitmap->SetPixel(x, y, Color::Transparent);
								}
							}
						}
						break;
					}
				}
			}

#ifdef GDIPLUS_DOUBLE_BUFFER

			if(32 == g_depthVal)
			{
				newBitmap->cachedBitmap = new CachedBitmap(newBitmap->bitmap, GET_GRAPHICS_OBJ(ctx));
			}
			else
			{
				// CachedBitmap's don't work with offscreen bitmaps when the color depth is not 24 bit
				newBitmap->cachedBitmap = 0;
			}
#else
			newBitmap->cachedBitmap = new CachedBitmap(newBitmap->bitmap, GET_GRAPHICS_OBJ(ctx));
#endif

			newBitmap->bitmapData = buffer;

			return (newBitmap);
		}

		WEBC_FREE(newBitmap);
	}

	WEBC_FREE(buffer);

	return 0;
}

/*---------------------------------------------------------------------------*/
static void GDIPlus_DestroyWebBitmap (
		HTMLGraphicsContext** ctx,
		void* b
	)
{
	WebcGDIPlusBitmap* bmp = (WebcGDIPlusBitmap*) b;

	if (bmp->bitmap)
	{
		delete bmp->bitmap;
	}

	if (bmp->bitmapData)
	{
		WEBC_FREE(bmp->bitmapData);
	}

	if (bmp->cachedBitmap)
	{
		delete bmp->cachedBitmap;
	}

	WEBC_FREE(bmp);
}

#ifdef WEBC_IMAGE_DRAW_EXTENSIONS

/*---------------------------------------------------------------------------*/
static int GDIPlus_CreateGenericBitmap (
		void* webCBitmapSrc,
		HTMLGraphicsContext** ctx,
		WebGraphBitmapInfo** genericBitMapDst)
{
	WebcGDIPlusGraphics* g = (WebcGDIPlusGraphics*) ctx;
	WebcGDIPlusBitmap* wcBmp = (WebcGDIPlusBitmap*) webCBitmapSrc;
	WEBC_SIZE bufSize;

	*genericBitMapDst = (WebGraphBitmapInfo *) WEBC_MALLOC(sizeof(WebGraphBitmapInfo));

	// TODO need to make this work for all image depths
	(*genericBitMapDst)->iWidth = wcBmp->bitmap->GetWidth();
	(*genericBitMapDst)->iHeight = wcBmp->bitmap->GetHeight();
	(*genericBitMapDst)->iBytesPerRow = wcBmp->bitmap->GetWidth() * 3;
	(*genericBitMapDst)->pixelFormat = WB_PIXEL_FORMAT_RGB_24;
	(*genericBitMapDst)->bHasTransparentColor = WEBC_FALSE;
	(*genericBitMapDst)->iTransparentIndex = 0;
	(*genericBitMapDst)->uTransparentRed = 0;
	(*genericBitMapDst)->uTransparentGreen = 0;
	(*genericBitMapDst)->uTransparentBlue = 0;
	(*genericBitMapDst)->iPaletteSize = 0;
	(*genericBitMapDst)->pPalette = 0;
	bufSize = (*genericBitMapDst)->iBytesPerRow * (*genericBitMapDst)->iHeight;
	(*genericBitMapDst)->pData = (WEBC_UINT8 *) WEBC_MALLOC(bufSize);

	//memcpy((void *)((*genericBitMapDst)->pData), (void *)(wcBmp->bitmapData + sizeof(BITMAPINFOHEADER)), bufSize);

		WEBC_UINT8* srcPtr = wcBmp->bitmapData + sizeof(BITMAPINFOHEADER);
		WEBC_UINT8* dstPtr = (WEBC_UINT8 *)((*genericBitMapDst)->pData);
		WEBC_UINT8* rowPtr = dstPtr;

		for (int y = 0; y < (*genericBitMapDst)->iHeight; y++, rowPtr += (*genericBitMapDst)->iBytesPerRow)
		{
			dstPtr = rowPtr;
			for (int x = 0; x < (*genericBitMapDst)->iWidth; x++, srcPtr += 3, dstPtr += 3)
			{
				dstPtr[0] = srcPtr[2];
				dstPtr[1] = srcPtr[1];
				dstPtr[2] = srcPtr[0];
			}
		}


	return 0;
}

#endif

/*---------------------------------------------------------------------------*/
static int GDIPlus_ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
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

	return (0);
}

/*---------------------------------------------------------------------------*/
static void GDIPlus_ClaimPointer (
		HTMLGraphicsContext** ctx
	)
{
	WebcGDIPlusGraphics* g = (WebcGDIPlusGraphics*) ctx;

	SetCapture(g->windowHandle);
}

/*---------------------------------------------------------------------------*/
static void GDIPlus_ReleasePointer (
		HTMLGraphicsContext** ctx
	)
{
	ReleaseCapture();
}

/*---------------------------------------------------------------------------*/
static WebColor GDIPlus_GetColorByIndex (
		HTMLGraphicsContext** ctx,
		int i
	)
{
	static UINT colorMap [] =
	{
		Color::Black,          // WGC_BLACK
		Color::Red,            // WGC_RED
		Color::Green,          // WGC_GREEN
		Color::Brown,          // WGC_BROWN
		Color::Blue,           // WGC_BLUE
		Color::Magenta,        // WGC_MAGENTA
		Color::Cyan,           // WGC_CYAN
		Color::LightGray,      // WGC_LIGHTGRAY
		Color::DarkGray,       // WGC_DARKGRAY
		Color::Pink,           // WGC_LIGHTRED
		Color::LightGreen,     // WGC_LIGHTGREEN
		Color::Yellow,         // WGC_YELLOW
		Color::LightBlue,      // WGC_LIGHTBLUE
		Color::LightPink,      // WGC_LIGHTMAGENTA
		Color::LightCyan,      // WGC_LIGHTCYAN
		Color::White,          // WGC_WHITE
		Color::Transparent     // WGC_TRANSPARENT
	};

	return colorMap[i];
}

#ifdef WEBC_BUFFER_SCROLLING

/*---------------------------------------------------------------------------*/
HTMLGraphicsContext**  GDIPlus_CreateBuffer (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT w,
		DISPLAY_INT h)
{
	WebcGDIPlusGraphics* g = (WebcGDIPlusGraphics*) WEBC_MALLOC(sizeof(WebcGDIPlusGraphics));
	if (g)
	{
		rtp_memset(g, 0, sizeof(WebcGDIPlusGraphics));

		g->ctx = &webcGDIPlusGraphicsClass;
		g->windowHandle   = ((WebcGDIPlusGraphics*)ctx)->windowHandle;
		g->outputGraphics = ((WebcGDIPlusGraphics*)ctx)->windowGraphics;
		g->blendLevelInt  = 255;
		g->blendLevelReal = 1.0;

		g->bufferImage = new Bitmap(w, h, ((WebcGDIPlusGraphics*)ctx)->windowGraphics);
		if (g->bufferImage)
		{
			g->bufferGraphics = Graphics::FromImage(g->bufferImage);
			if (g->bufferGraphics)
			{
				g->outputGraphics = g->bufferGraphics;
			}
			else
			{
				delete g->bufferImage;
				g->bufferImage = 0;
			}
		}

		return &(g->ctx);
	}

	return 0;
}

/*---------------------------------------------------------------------------*/
void GDIPlus_DestroyBuffer (
		HTMLGraphicsContext** ctx,
		HTMLGraphicsContext** buffer)
{
	WebcGDIPlusGraphics* g = (WebcGDIPlusGraphics*) buffer;

	if (g->scratchGraphics)
	{
		delete g->scratchGraphics;
	}

	if (g->scratchBitmap)
	{
		delete g->scratchBitmap;
	}

	if (g->bufferGraphics)
	{
		delete g->bufferGraphics;
	}

	if (g->bufferImage)
	{
		delete g->bufferImage;
	}

	WEBC_FREE(buffer);
}

/*---------------------------------------------------------------------------*/
HTMLGraphicsContext**  GDIPlus_ResizeBuffer (
		HTMLGraphicsContext** ctx,
		HTMLGraphicsContext** buffer,
		DISPLAY_INT w,
		DISPLAY_INT h)
{
	HTMLGraphicsContext** dst = GDIPlus_CreateBuffer(ctx, w, h);
	if (dst)
	{
		// if successful, destroy the old buffer
		GDIPlus_DestroyBuffer (ctx, buffer);
	}
	return dst;
}

/*---------------------------------------------------------------------------*/
void  GDIPlus_CopyBufferRegion (
		HTMLGraphicsContext** dstCtx,
		HTMLRect* dstRect,
		HTMLGraphicsContext** srcBuffer,
		DISPLAY_INT srcX,
		DISPLAY_INT srcY)
{
	WebcGDIPlusGraphics* dst = (WebcGDIPlusGraphics*) dstCtx;
	WebcGDIPlusGraphics* src = (WebcGDIPlusGraphics*) srcBuffer;

	DISPLAY_INT w = dstRect->right  - dstRect->left + 1;
	DISPLAY_INT h = dstRect->bottom - dstRect->top  + 1;



	if (dst == src)
	{
		// check to make sure existing scratch bitmap is large enough
		if (dst->scratchBitmap &&
		    (w > (DISPLAY_INT) dst->scratchBitmap->GetWidth() ||
			 h > (DISPLAY_INT) dst->scratchBitmap->GetHeight()))
		{
			delete dst->scratchGraphics;
			delete dst->scratchBitmap;
			dst->scratchGraphics = 0;
			dst->scratchBitmap = 0;
		}

		// create a temporary copy space
		if (!dst->scratchBitmap)
		{
			dst->scratchBitmap = new Bitmap(w, h, dst->outputGraphics);
			if (dst->scratchBitmap)
			{
				dst->scratchGraphics = Graphics::FromImage(dst->scratchBitmap);
				if (!dst->scratchGraphics)
				{
					delete dst->scratchBitmap;
					dst->scratchGraphics = 0;
					dst->scratchBitmap = 0;
				}
			}
		}

		if (dst->scratchBitmap)
		{
			dst->scratchGraphics->DrawImage (
					src->bufferImage,  // image
					0,                 // x
					0,                 // y
					srcX,              // srcX
					srcY,              // srcY
					w,                 // srcWidth
					h,                 // srcHeight
					UnitPixel          // srcUnit
				);

			dst->outputGraphics->DrawImage (
					dst->scratchBitmap,// image
					dstRect->left,     // x
					dstRect->top,      // y
					0,                 // srcX
					0,                 // srcY
					w,                 // srcWidth
					h,                 // srcHeight
					UnitPixel          // srcUnit
				);
		}
	}
	else
	{
		dst->outputGraphics->DrawImage (
				src->bufferImage,  // image
				dstRect->left,     // x
				dstRect->top,      // y
				srcX,              // srcX
				srcY,              // srcY
				w,                 // srcWidth
				h,                 // srcHeight
				UnitPixel          // srcUnit
			);
	}

}

#endif // WEBC_BUFFER_SCROLLING
/*---------------------------------------------------------------------------*/
typedef struct
{
	Rect    rect;	// Area being pushed. This is the only area where drawing will take place but the coordinate will
	Graphics*            windowGraphics;
	Bitmap*              bufferImage;
	Graphics*            bufferGraphics;
	Graphics*            outputGraphics;
} WebcGDIPushedWindowGraphic;
void * GDIPlus_PushCanvas(HTMLGraphicsContext** ctx, HTMLRect* rect)
{
	WebcGDIPushedWindowGraphic *retPrevCtx;
	WebcGDIPlusGraphics* g = (WebcGDIPlusGraphics*) ctx;
	HTML_TO_RECT(gdiRect, *rect);
	retPrevCtx = (WebcGDIPushedWindowGraphic *) WEBC_MALLOC(sizeof(WebcGDIPushedWindowGraphic));
	WEBC_ASSERT(retPrevCtx);
	if (!retPrevCtx)
		return(0);
	retPrevCtx->windowGraphics 	= g->windowGraphics;
	retPrevCtx->bufferImage 	= g->bufferImage;
	retPrevCtx->bufferGraphics 	= g->bufferGraphics;
	retPrevCtx->outputGraphics 	= g->outputGraphics;
	retPrevCtx->rect = gdiRect;
	WebRect canvasRect;
	GDIPlus_GetCanvasRect(ctx, (HTMLRect*) &canvasRect);
	Graphics* nwg = new Graphics(g->windowHandle);
	WEBC_ASSERT(nwg);
	g->windowGraphics = nwg;
	if (!g->windowGraphics)
		return(0);
	g->bufferImage = new Bitmap(canvasRect.Width(), canvasRect.Height(), g->windowGraphics);
	WEBC_ASSERT(g->bufferImage);
	if (!g->bufferImage)
		return(0);
	g->bufferGraphics = Graphics::FromImage(g->bufferImage);
	WEBC_ASSERT(g->bufferGraphics);
	if (!g->bufferGraphics)
		return(0);
	g->outputGraphics = g->bufferGraphics;
	return((void *)retPrevCtx);
}
void GDIPlus_PopCanvas(HTMLGraphicsContext** ctx, void *pvretPrevCtx, int opacity)
{
	WebcGDIPushedWindowGraphic *PrevCtx = (WebcGDIPushedWindowGraphic *) pvretPrevCtx;
	WebcGDIPlusGraphics* g = (WebcGDIPlusGraphics*) ctx;
	float opacityreal;
	opacityreal = (float) opacity;
	opacityreal /= 255.0f;
	if (g->bufferImage && g->bufferGraphics)
	{
		ImageAttributes imgAttrib;
		ColorMatrix colorMatrix = {
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
		colorMatrix.m[3][3] = opacityreal;
		imgAttrib.SetColorMatrix(
			&colorMatrix,
			ColorMatrixFlagsDefault,
			ColorAdjustTypeBitmap);
		PrevCtx->outputGraphics->DrawImage (
					g->bufferImage,           // image
					PrevCtx->rect,
					PrevCtx->rect.GetLeft(),      // srcX
					PrevCtx->rect.GetTop(),       // srcY
					PrevCtx->rect.Width,
					PrevCtx->rect.Height,
					UnitPixel,
					&imgAttrib
				);
		delete g->bufferGraphics; // Free buffer graphics. outputGraphics is a copy of this so it is not freed.
		delete g->bufferImage;	  // Free buffer image
		delete g->windowGraphics; // Free buffer image
		g->windowGraphics  = PrevCtx->windowGraphics;
		g->bufferImage     = PrevCtx->bufferImage;
		g->bufferGraphics  = PrevCtx->bufferGraphics;
		g->outputGraphics  = PrevCtx->outputGraphics;
		WEBC_FREE(PrevCtx);
	}
}

int webc_GuiInitConfig(HTMLBrowserConfig* config)
{
	rtp_memset(config, 0, sizeof(*config));
	return webc_InitGDIPlusBrowserConfig (config, 0);
}

int webc_InitGDIPlusBrowserConfig (HTMLBrowserConfig* config, HWND windowHandle)
{
#if (WEBC_SUPPORT_UNICODE)
    LOG_PRINTF("Init GDI called, Unicode enabled\n");
#else
    LOG_PRINTF("Init GDI called, Unicode disabled\n");
    LOG_PRINTF("!!!! Incomplete Font support provided under Windows when Unicode is disabled\n");
#endif
	webcGDIPlusGraphicsClass.WindowOpen                 = GDIPlus_WindowOpen;
	webcGDIPlusGraphicsClass.WindowClose                = GDIPlus_WindowClose;
	webcGDIPlusGraphicsClass.MoveTo						= GDIPlus_MoveByorTo;
	webcGDIPlusGraphicsClass.ScreenInfo					= GDIPlus_ScreenInfo;

	webcGDIPlusGraphicsClass.getCanvasRect              = GDIPlus_GetCanvasRect;

	webcGDIPlusGraphicsClass.claimPointer               = GDIPlus_ClaimPointer;
	webcGDIPlusGraphicsClass.releasePointer             = GDIPlus_ReleasePointer;

	webcGDIPlusGraphicsClass.invalidateRect             = GDIPlus_InvalidateRect;
	webcGDIPlusGraphicsClass.beginDrawing               = GDIPlus_BeginDrawing;
	webcGDIPlusGraphicsClass.getClipRect                = GDIPlus_GetClipRect;
	webcGDIPlusGraphicsClass.setClipRect                = GDIPlus_SetClipRect;
	webcGDIPlusGraphicsClass.endDrawing                 = GDIPlus_EndDrawing;

	webcGDIPlusGraphicsClass.colorToRGBA                = GDIPlus_ColorToRGB;
	webcGDIPlusGraphicsClass.RGBAToColor                = GDIPlus_RGBToColor;
	webcGDIPlusGraphicsClass.getColorByIndex            = GDIPlus_GetColorByIndex;

	webcGDIPlusGraphicsClass.drawText                   = GDIPlus_DrawText;
	webcGDIPlusGraphicsClass.drawTextLen                = GDIPlus_DrawTextLen;
	webcGDIPlusGraphicsClass.drawBitmap                 = GDIPlus_DrawBitmap;
	webcGDIPlusGraphicsClass.drawBitmapStretchedToRect  = GDIPlus_DrawStretchedBitmap;
	webcGDIPlusGraphicsClass.drawRectangle              = GDIPlus_DrawRectangle;

	webcGDIPlusGraphicsClass.createFont                 = GDIPlus_CreateFont;
	webcGDIPlusGraphicsClass.getDefaultFont             = GDIPlus_GetDefaultFont;
	webcGDIPlusGraphicsClass.destroyFont                = GDIPlus_DestroyFont;
	webcGDIPlusGraphicsClass.getNumFontFamilies         = GDIPlus_NumFontFamilies;
	webcGDIPlusGraphicsClass.getFontFamilyNames         = GDIPlus_GetFontFamilyNames;
	webcGDIPlusGraphicsClass.getFontFamilyGenericNames  = GDIPlus_GetFontFamilyGenericNames;
	webcGDIPlusGraphicsClass.getTextHeight              = GDIPlus_GetTextHeight;
	webcGDIPlusGraphicsClass.getTextWidth               = GDIPlus_GetTextWidth;
	webcGDIPlusGraphicsClass.getTextHeightLen           = GDIPlus_GetTextHeightLen;
	webcGDIPlusGraphicsClass.getTextWidthLen            = GDIPlus_GetTextWidthLen;
	webcGDIPlusGraphicsClass.getFontHeight              = GDIPlus_GetFontHeight;
	webcGDIPlusGraphicsClass.getFontBaseline            = GDIPlus_GetFontBaseline;

	webcGDIPlusGraphicsClass.createWebBitmap            = GDIPlus_CreateWebBitmap;
	webcGDIPlusGraphicsClass.destroyWebBitmap           = GDIPlus_DestroyWebBitmap;
	webcGDIPlusGraphicsClass.createStretchedBitmap      = GDIPlus_CreateStretchedBitmap;
	webcGDIPlusGraphicsClass.destroyStretchedBitmap     = GDIPlus_DestroyWebBitmap;
	webcGDIPlusGraphicsClass.getBitmapWidth             = GDIPlus_GetBitmapWidth;
	webcGDIPlusGraphicsClass.getBitmapHeight            = GDIPlus_GetBitmapHeight;


	webcGDIPlusGraphicsClass.optionalDrawHorizontalLine = 0;
	webcGDIPlusGraphicsClass.optionalDrawVerticalLine   = 0;
	webcGDIPlusGraphicsClass.optionalDrawStyledFrame    = GDIPlus_DrawPatternFrame;
	webcGDIPlusGraphicsClass.optionalDrawBitmapTiled    = GDIPlus_TileBitmap;
	webcGDIPlusGraphicsClass.optionalSetCanvasRect      = 0;
	webcGDIPlusGraphicsClass.pushCanvas      = GDIPlus_PushCanvas;
	webcGDIPlusGraphicsClass.popCanvas      = GDIPlus_PopCanvas;

#if(WEBC_IMAGE_DRAW_EXTENSIONS)
	webcGDIPlusGraphicsClass.createGenericBitmap      = 	GDIPlus_CreateGenericBitmap;
#endif

  #ifdef WEBC_BUFFER_SCROLLING
	webcGDIPlusGraphicsClass.createBuffer               = GDIPlus_CreateBuffer;
	webcGDIPlusGraphicsClass.resizeBuffer  	            = GDIPlus_ResizeBuffer;
	webcGDIPlusGraphicsClass.destroyBuffer              = GDIPlus_DestroyBuffer;
	webcGDIPlusGraphicsClass.copyBufferRegion	  	    = GDIPlus_CopyBufferRegion;
  #endif // WEBC_BUFFER_SCROLLING

	WebcGDIPlusGraphics* ctx = (WebcGDIPlusGraphics*) WEBC_MALLOC(sizeof(WebcGDIPlusGraphics));
	if (ctx)
	{
		rtp_memset(ctx, 0, sizeof(WebcGDIPlusGraphics));
		ctx->ctx = &webcGDIPlusGraphicsClass;
		ctx->windowGraphics = new Graphics(windowHandle, FALSE);
		ctx->BrowserHandle = config->BrowserHandle;
		ctx->windowHandle = windowHandle;
		ctx->outputGraphics = ctx->windowGraphics;
		ctx->blendLevelInt = 255;
		ctx->blendLevelReal = 1.0;
		config->graphics = (HTMLGraphicsContext**) ctx;
		config->processInputQueue = &gdiplusProcessInputQueueFn;

		if(!g_depthVal)
			g_depthVal = GetDeviceCaps(((WebcGDIPlusGraphics*)ctx)->windowGraphics->GetHDC(), BITSPIXEL);

		return (0);
	}

	return (-1);

}

/*---------------------------------------------------------------------------*/
void webc_DestroyGDIPlusBrowserConfig (HTMLBrowserConfig* config)
{
	WebcGDIPlusGraphics* ctx = (WebcGDIPlusGraphics*) config->graphics;

	delete ctx->windowGraphics;
	WEBC_FREE(ctx);
}

/*---------------------------------------------------------------------------*/
void   webc_DestroyGDIPlusBrowser       (HBROWSER_HANDLE hbrowser)
{
	HTMLBrowserConfig config;

	if (webc_BrowserGetConfig(hbrowser, &config) >= 0)
	{
		webc_DestroyBrowser(hbrowser, 0);
		webc_DestroyGDIPlusBrowserConfig(&config);
	}
}

/*---------------------------------------------------------------------------*/
WebKey webc_TranslateGDIPlusKey         (WEBC_UINT32 rawKey)
{
	if (!gdiplusKeyMapInitialized)
	{
		webc_CreateKeyMap(&gdiplusKeyMap, gdiplusKeyData, WG_NUM_GDIPLUS_KEYS);
		gdiplusKeyMapInitialized = WEBC_TRUE;
	}

	return webc_TranslateKey(&gdiplusKeyMap, rawKey);
}


/*****************************************************************************/
// Global Data
/*****************************************************************************/
// Cheating, copied from resource.h
#define IDM_ABOUT				104
#define IDM_EXIT				105

int giShift = 0;
int giCaps = 0;
int giCtrl = 0;
int giAlt = 0;
extern "C" int giEscapeDown;
#define AT_KEYTABLE_LEN 0x59

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

//
//  FUNCTION: WebcWindowProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

/* Set to one to increase the frequency of WM_TIMER messages to the window
   Doesn't seem to change the cycle times of the browser timeouts */
#define INJECT_EXTRA_TIMER_MESSAGES 0
/* If INJECT_EXTRA_TIMER_MESSAGES is 1, Set to the number of extra WM_PSEUDO_TIMER (WM_APP+99) messages to post  */
#define NUM_EXTRA_TIMER_MESSAGES 15
void webc_check_clipboard(HWND hWnd, char *pastebuffer);

LRESULT CALLBACK WebcWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HBROWSER_HANDLE which_Browser;
	HTMLEvent e;
	char pastebuffer[1024];

#if (WEBC_SUPPORT_POSTFILE)
	char *DestUrl = "10.1.10.6";
	char *DestFieldName		=  "../html/uploads/copyofQWRT_water.jpg";
	char *DestAppName		=  "/cgi-bin/simplecgi.sh";
	//char *SourceFieldName	= "/test/badge.gif";
	char *SourceFieldName	= "/test/QWRT_water.jpg";
#endif
	pastebuffer[0] = 0;
    which_Browser  = webc_FindBrowserByWindow((void *)hWnd);
    if (!which_Browser)
    {
	    return DefWindowProc(hWnd, message, wParam, lParam);
    }

	e.type = HTML_EVENT_NONE;
	e.flags = 0;

	switch (message)
	{
		case WM_RBUTTONDOWN:
#if	(WEBC_SUPPORT_POSTFILE)
			 webc_post_UploadFileASCII(which_Browser,SourceFieldName, DestUrl,DestAppName, DestFieldName);
#endif

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
					HELEMENT_HANDLE hframe;
					HDOC_HANDLE hdoc = webc_BrowserGetDocument(which_Browser);

					hframe = webc_DocFindElement(hdoc, "contentFrame", 0, HTML_ELEMENT_ANY, 0);
					if (hframe)
					{
						hdoc = webc_ElementGetFrameDocument(hframe);
						if (hdoc == WEBC_NULL)
						{
							hdoc = webc_BrowserGetDocument(which_Browser);
						}
					}

					webc_DocSetUrl(hdoc, "webc://welcome.html", WEBC_TRUE);
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

			if (which_Browser)
			{
				webc_BrowserInvalidate(which_Browser);
				webc_BrowserDraw(which_Browser);
			}

			EndPaint(hWnd, &ps);
			break;
		}

		case WM_DESTROY:
            /* Release browser memory if the browser is not already free */
            webc_DestroyBrowser(which_Browser, 0);
            /* Quit if we've closed all browser windows */
            if (!webc_FindBrowserByIndex(0))
			    PostQuitMessage(0);
			break;

		case WM_LBUTTONDOWN:
			e.type = HTML_EVENT_MOUSEDOWN;
			break;

		case WM_LBUTTONUP:
			e.type = HTML_EVENT_MOUSEUP;
			break;
		case WM_RBUTTONDOWN:
			break;
		case WM_RBUTTONUP:
			webc_check_clipboard(hWnd, pastebuffer);
			if (pastebuffer[0])
				e.type = HTML_EVENT_KEYDOWN;
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

#if (INJECT_EXTRA_TIMER_MESSAGES)
		case WM_APP+99:
#endif
		case WM_TIMER:
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			wcProcessTimedCallbacks();
#if (USE_EXPERIMENTAL_TIMER_EVENT)
			e.type = HTML_EVENT_TIMER;
			break;
#else
			webc_BrowserProcessTimers(which_Browser);
			webc_BrowserDraw(which_Browser);
			webc_BrowserProcessLoadQueueAll(which_Browser);
			webc_BrowserDraw(which_Browser);
#if (INJECT_EXTRA_TIMER_MESSAGES)
            if (message == WM_TIMER)
            {
                for (int i = 0; i < NUM_EXTRA_TIMER_MESSAGES; i++)
                    PostMessage(hWnd, WM_APP+99, wParam, lParam);
            }
#endif
			return 0;
#endif
		case WM_CLOSE:
            // Someone clicked the close button
            // call webc_DestroyBrowser(which_Browser, 0); which frees memory
            // and calls GDIPlus_WindowClose to destroy he window
            //webc_DestroyBrowser(which_Browser, 0);
			return 0;
//            LOG_PRINTF("Got close\n");
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
    /* Search for the browser again in case it was destroyed */
    which_Browser  = webc_FindBrowserByWindow((void *)hWnd);
	if (!which_Browser)
        return 0;
//    HEREHERE is this right... needs to be fixed
	if (which_Browser && (e.type != HTML_EVENT_NONE))
	{
		webc_BrowserSuspendDraw(which_Browser);
		if (e.type == HTML_EVENT_KEYDOWN && pastebuffer[0])
		{
		int i;
			for (i = 0; pastebuffer[i]; i++)
			{
				e.data.key = pastebuffer[i];
				e.type = HTML_EVENT_KEYDOWN;
				webc_BrowserDispatchEvent(which_Browser, &e);
				e.data.key = pastebuffer[i];
				e.type = HTML_EVENT_KEYUP;
				webc_BrowserDispatchEvent(which_Browser, &e);
			}
		}
		else
		webc_BrowserDispatchEvent(which_Browser, &e);
		/* Search for the browser again in case it was destroyed by the event handler */
		which_Browser  = webc_FindBrowserByWindow((void *)hWnd);
		if (!which_Browser)
	        return 0;
		if (webc_BrowserGetDrawState (which_Browser) > 0)
		    webc_BrowserResumeDraw(which_Browser);
		which_Browser  = webc_FindBrowserByWindow((void *)hWnd);
		if (!which_Browser)
	        return 0;
		webc_BrowserDraw(which_Browser);
		which_Browser  = webc_FindBrowserByWindow((void *)hWnd);
		if (!which_Browser)
	        return 0;
		webc_BrowserProcessLoadQueueAll(which_Browser);
		which_Browser  = webc_FindBrowserByWindow((void *)hWnd);
		if (!which_Browser)
	        return 0;
		webc_BrowserDraw(which_Browser);
		which_Browser  = webc_FindBrowserByWindow((void *)hWnd);
		if (!which_Browser)
	        return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);;
}


// Alert box for Javascript.. (badly in need of rework)
extern "C" void webc_user_alert_box(char *str)
{
WEBC_CHAR UnicodeStr[512];

    webc_c_strcpy (&UnicodeStr[0],str);
    MessageBox(0, (LPCTSTR) UnicodeStr, (LPCTSTR) L"webC Error", MB_OKCANCEL);
}


void webc_check_clipboard(HWND hWnd, char *pastebuffer)
{
    HGLOBAL   hglb;
    LPTSTR    lptstr;

    {
        if (!IsClipboardFormatAvailable(CF_TEXT))
            return;
        if (!OpenClipboard(hWnd))
            return;

        hglb = GetClipboardData(CF_TEXT);
        if (hglb != NULL)
        {
            lptstr = (LPTSTR) GlobalLock(hglb);
            if (lptstr != NULL)
            {
			char *ptext;
			ptext = (char *) lptstr;
				strcpy(pastebuffer, ptext);
                GlobalUnlock(hglb);
            }
        }
        CloseClipboard();

        return;
    }
}
#endif // GDIFRAMEBUFFER
