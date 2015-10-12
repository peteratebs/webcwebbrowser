/*
|  WEBCPEG.CPP -
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

#include "webcpeg.h"
#include "webgraph.hpp"
#include "util.h"
#include "imgconv.hpp"
#include "webcmem.h"
#include "peg.hpp"
#include "wfpeg.h"
#include "rtpstr.h"
#include "webcassert.h"
#include "webimage.hpp"
#include "webcfg.h"
#include "webcfg.h"
#include "pwebwin.hpp"

#if (PEG_NUM_COLORS == 0xffffff)
 #ifdef WEBC_PEG_32_BITS_PER_PIXEL
  #include "24to32.hpp"
  #include "8to32.hpp"
 #else
  #include "24to24.hpp"
  #include "32to24.hpp"
  #include "8to24.hpp"
 #endif

#elif (PEG_NUM_COLORS == 0xffff)
 #ifdef RGB_USE_555
  #include "24to555.hpp"
  #include "8to555.hpp"

 #else // RGB_USE_565
  #include "24to565.hpp"
  #include "8to565.hpp"
 #endif

#elif (PEG_NUM_COLORS < 0xffff && PEG_NUM_COLORS > 16)
 #include "24to8.hpp"
 #include "8to8.hpp"

#else // PEG_NUM_COLORS <= 16
 // tbd - bitmap converters!!!
  #ifdef PEG_GRAYSCALE
    #include "24togs.hpp"
    #include "8togs.hpp"
  #else
    #error unsupported color scheme
  #endif // Gray scale
#endif

#if (WEBC_SUPPORT_UNICODE && !defined(PEG_UNICODE))
#error turn on PEG_UNICODE
#endif

/*****************************************************************************/
/* Macros
 *****************************************************************************/

#ifdef _EFI_
#define WEBC_PEG_32_BITS_PER_PIXEL
#define BLUE_BYTE_FIRST
#endif // EFI

// define this symbol for grayscale displays
#ifdef PEG_GRAYSCALE
  #ifdef PEG_GRAY_INVERT
    // define this symbol if PEG_GRAYSCALE and display uses 0==WHITE
    #define GRAYSCALE_INVERTED
  #endif // invert gray
#endif // PEG_GRAYSCALE


#define COLOR_HASH_FUNCTION(X) (((X >> 14) ^ (X >> 10) ^ X) & 0x3ff)

#if (PEG_NUM_COLORS == 0xffffff) // 24 bit color
  #define PEG_RGB(r,g,b)  (((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff)
  #define PEG_RED(c)      (((c) & 0xff0000) >> 16)
  #define PEG_GREEN(c)    (((c) & 0x00ff00) >> 8)
  #define PEG_BLUE(c)     (((c) & 0x0000ff))

#elif (PEG_NUM_COLORS == 0xffff)   // 16 bit color
 #ifdef RGB_USE_555
  #define PEG_RGB(r,g,b)  (((r) & 0xf8) << 7) | (((g) & 0xf8) << 2) | (((b) & 0xf8) >> 3)
  #define PEG_RED(c)      (((c) & 0x7c00) >> 7)
  #define PEG_GREEN(c)    (((c) & 0x03e0) >> 2)
  #define PEG_BLUE(c)     (((c) & 0x001f) << 3)

 #else // RGB_USE_565
  #define PEG_RGB(r,g,b)  (((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | (((b) & 0xf8) >> 3)
  #define PEG_RED(c)      (((c) & 0xf800) >> 8)
  #define PEG_GREEN(c)    (((c) & 0x07e0) >> 3)
  #define PEG_BLUE(c)     (((c) & 0x001f) << 3)
 #endif
#endif

#define DISPLAY_INT_TO_PEG_INT(X) ((WEBC_INT16)(X))

#define GET_PEG_THING(X)    (((WebcPegGraphics*)(X))->parentThing)
#define GET_PALETTE(X)      (((WebcPegGraphics*)(X))->palette)
#define GET_PALETTE_SIZE(X) (((WebcPegGraphics*)(X))->paletteSize)

/*****************************************************************************/
/* Types
 *****************************************************************************/


/*****************************************************************************/
/* Function Prototypes
/*****************************************************************************/

#if defined(ROTATE_CCW) || defined(ROTATE_CW)
static void _RotatePegBitmap(PegBitmap *pBitmap);
#endif

static int PEG_Win32ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	);

/*****************************************************************************/
/* Data
 *****************************************************************************/

extern int giAllowTimerTick;
int giMsgPending = 0;

int (*pegProcessInputQueueFn) (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode) = PEG_Win32ProcessInputQueue;

#if (WEBC_SUPPORT_RGB_TABLE && 0)
 static RGBMatch gColorMatchTable[1024] = {0};
#endif // WEBC_SUPPORT_RGB_TABLE

static HTMLGraphicsContext webcPegGraphicsClass = {0};

static WEBC_READONLY unsigned long _cssBorderStyleToPattern [] = {
	0xf0f0f0f0, // CSS_BORDER_STYLE_DASHED = 0,
	0xaaaaaaaa, // CSS_BORDER_STYLE_DOTTED,
	0xffffffff, // CSS_BORDER_STYLE_DOUBLE,
	0xffffffff, // CSS_BORDER_STYLE_GROOVE,
	0xffffffff, // CSS_BORDER_STYLE_INSET,
	0x00000000, // CSS_BORDER_STYLE_NONE,
	0xffffffff, // CSS_BORDER_STYLE_OUTSET,
	0xffffffff, // CSS_BORDER_STYLE_RIDGE,
	0xffffffff  // CSS_BORDER_STYLE_SOLID
};

#define WG_NUM_PEG_KEYS 32

static WEBC_UINT32 pegKeyData [WG_NUM_PEG_KEYS*2] =
{
	WGK_TAB         , PK_TAB        ,
	WGK_CR          , PK_CR         ,
	WGK_PGUP        , PK_PGUP       ,
	WGK_PGDN        , PK_PGDN       ,
	WGK_LNUP        , PK_LNUP       ,
	WGK_LNDN        , PK_LNDN       ,
	WGK_ALT         , PK_ALT        ,
	WGK_CTRL        , PK_CTRL       ,
	WGK_SHIFT       , PK_SHIFT      ,
	WGK_CAPS        , PK_CAPS       ,
	WGK_NUMLOCK     , PK_NUMLOCK    ,
	WGK_SCROLLLOCK  , PK_SCROLLLOCK ,
	WGK_F1          , PK_F1         ,
	WGK_F2          , PK_F2         ,
	WGK_F3          , PK_F3         ,
	WGK_F4          , PK_F4         ,
	WGK_F5          , PK_F5         ,
	WGK_F6          , PK_F6         ,
	WGK_F7          , PK_F7         ,
	WGK_F8          , PK_F8         ,
	WGK_F9          , PK_F9         ,
	WGK_F10         , PK_F10        ,
	WGK_F11         , PK_F11        ,
	WGK_F12         , PK_F12        ,
	WGK_INSERT      , PK_INSERT     ,
	WGK_DELETE      , PK_DELETE     ,
	WGK_BACKSPACE   , PK_BACKSPACE  ,
	WGK_LEFT        , PK_LEFT       ,
	WGK_RIGHT       , PK_RIGHT      ,
	WGK_HOME        , PK_HOME       ,
	WGK_END         , PK_END        ,
	WGK_ESC         , PK_ESC
};

static WebcKeyMap pegKeyMap;
static WEBC_BOOL  pegKeyMapInitialized = WEBC_FALSE;

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

#if (defined(PEG_UNICODE) && !WEBC_SUPPORT_UNICODE)

/*---------------------------------------------------------------------------*/
static void _ASCIItoUC (unsigned short *ucStr, const char *asciiStr, int n)
{
	while (n > 0)
	{
		*ucStr = (unsigned short) *asciiStr;

		if (!*asciiStr)
		{
			return;
		}

		ucStr++;
		asciiStr++;
		n--;
	}

	*ucStr = (unsigned short) 0;
}

/*---------------------------------------------------------------------------*/
static void _PegDrawText8(PegThing *pThing, PegPoint &pt, const char *text, PegColor &Color, PegFont* font, int len)
{
	if (!text)
	{
		return;
	}

	if (len < 64)
	{
		PEGCHAR textUC[64];
		_ASCIItoUC(textUC, text, len);
		pThing->DrawText(pt, textUC, Color, font, len);
	}
	else if (len < 128)
	{
		PEGCHAR textUC[128];
		_ASCIItoUC(textUC, text, len);
		pThing->DrawText(pt, textUC, Color, font, len);
	}
	else if (len < 256)
	{
		PEGCHAR textUC[256];
		_ASCIItoUC(textUC, text, len);
		pThing->DrawText(pt, textUC, Color, font, len);
	}
	else
	{
		PEGCHAR textUC[512];

		if (len > 511)
		{
			len = 511;
		}

		_ASCIItoUC(textUC, text, len);
		pThing->DrawText(pt, textUC, Color, font, len);
	}
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT _PegTextWidth8(PegThing *pThing, const char *text, PegFont* font, int len)
{
	if (!text)
	{
		return (0);
	}

	if (len < 64)
	{
		PEGCHAR textUC[64];
		_ASCIItoUC(textUC, text, len);
		return pThing->Screen()->TextWidth(textUC, font, len);
	}
	else if (len < 128)
	{
		PEGCHAR textUC[128];
		_ASCIItoUC(textUC, text, len);
		return pThing->Screen()->TextWidth(textUC, font, len);
	}
	else if (len < 256)
	{
		PEGCHAR textUC[256];
		_ASCIItoUC(textUC, text, len);
		return pThing->Screen()->TextWidth(textUC, font, len);
	}
	else
	{
		PEGCHAR textUC[512];

		if (len > 511)
		{
			len = 511;
		}

		_ASCIItoUC(textUC, text, len);
		return pThing->Screen()->TextWidth(textUC, font, len);
	}

	return (0); // just to be clean
}

#endif



/*---------------------------------------------------------------------------*/



void PEG_ScreenInfo (HTMLGraphicsContext** ctx, HTMLScreenInfo *pinfo)
{
    pinfo->availHeight = GetSystemMetrics(SM_CYFULLSCREEN);
    pinfo->availWidth  = GetSystemMetrics(SM_CXFULLSCREEN);
    pinfo->height      = GetSystemMetrics(SM_CYFULLSCREEN);
    pinfo->width       = GetSystemMetrics(SM_CXFULLSCREEN);
    pinfo->colorDepth  = 24;
}

int PEG_WindowOpen (
		HTMLGraphicsContext** ctx,
		HTMLBrowserConfig *pconfig,
        const WebChar *title
        )
{
	int x, y, width, height;
	PegWebWindow* newWindow=0;
    PegRect Rect;
	WEBC_BOOL bNeedsCenter = WEBC_FALSE;

    // User supplied parameters
	x = pconfig->WindowConfig.top;
    y = pconfig->WindowConfig.left;
    height =pconfig->WindowConfig.height;
    width = pconfig->WindowConfig.width;

    // If height or width are 0, use defauts
    if (height == 0 || width == 0)
    {
		bNeedsCenter = WEBC_TRUE;
	    Rect.Set(0, 0, 480 - 1, 450 - 1);
	}
	else
	{
	    Rect.Set(x, y, x + width - 1, y + height - 1);
	}

	// make a new window
	newWindow = new PegWebWindow(Rect);
	if(pconfig->WindowTitle)
		newWindow->Add(new PegTitle((PEGCHAR *)pconfig->WindowTitle));
	else
		newWindow->Add(new PegTitle((PEGCHAR *)L""));

	if(bNeedsCenter)
		newWindow->Presentation()->Center(newWindow);

	newWindow->Presentation()->Add(newWindow, TRUE);

	newWindow->mBrowser = pconfig->BrowserHandle;

     if (webc_InitPegBrowserConfig(pconfig, newWindow) >= 0)
     {

		 webc_SetBrowserWindow (pconfig->BrowserHandle, (void *) newWindow);

		 pconfig->WindowHandle = (void *) newWindow;

		return(0);
     }

     return(-1);
}

static void PEG_WindowClose (HTMLGraphicsContext** ctx)
{
        // The browser destructor called us.
}

void PEG_MoveByorTo (HTMLGraphicsContext** ctx, DISPLAY_INT x, DISPLAY_INT y, WEBC_BOOL is_absolute)
{
}

/*---------------------------------------------------------------------------*/
static void PEG_DrawRectangle (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor outlineColor,
		WebColor fillColor,
		WEBC_BOOL fill
	)
{
	PegColor color((COLORVAL) outlineColor, (COLORVAL) fillColor, (fill)? CF_FILL : CF_NONE);
	PegRect where;

	where.Set (
			DISPLAY_INT_TO_PEG_INT(rect->left),
			DISPLAY_INT_TO_PEG_INT(rect->top),
			DISPLAY_INT_TO_PEG_INT(rect->right),
			DISPLAY_INT_TO_PEG_INT(rect->bottom)
		);

	GET_PEG_THING(ctx)->Rectangle(where, color);
}

/*---------------------------------------------------------------------------*/
static void PEG_DrawBitmap (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		void* bmp
	)
{
	PegPoint pt;

	pt.x = DISPLAY_INT_TO_PEG_INT(x);
	pt.y = DISPLAY_INT_TO_PEG_INT(y);

	GET_PEG_THING(ctx)->Bitmap(pt, (PegBitmap*) bmp);
}

/*---------------------------------------------------------------------------*/
static void PEG_DrawStretchedBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		void* b
	)
{
	PegPoint pt;
	DISPLAY_INT w = rect->right - rect->left + 1;
	DISPLAY_INT h = rect->bottom - rect->top + 1;
	PegBitmap* bmp = (PegBitmap*) b;

	pt.x = DISPLAY_INT_TO_PEG_INT(rect->left);
	pt.y = DISPLAY_INT_TO_PEG_INT(rect->top);

	if (w != bmp->wWidth || h != bmp->wHeight)
	{
		PegBitmap *stretched = GET_PEG_THING(ctx)->Screen()->ResizeImage(
				bmp,
				DISPLAY_INT_TO_PEG_INT(w),
				DISPLAY_INT_TO_PEG_INT(h)
			);

		if (stretched)
		{
			if (bmp->uFlags & BMF_HAS_TRANS)
			{
				stretched->uFlags |= BMF_HAS_TRANS;
			}
			stretched->dTransColor = bmp->dTransColor;
			GET_PEG_THING(ctx)->Bitmap(pt, stretched);
			GET_PEG_THING(ctx)->Screen()->DestroyBitmap(stretched);
		}
	}
	else
	{
		GET_PEG_THING(ctx)->Bitmap(pt, bmp);
	}
}

/*---------------------------------------------------------------------------*/
static void* PEG_CreateStretchedBitmap (
		HTMLGraphicsContext** ctx,
		void* b,
		DISPLAY_INT w,
		DISPLAY_INT h
	)
{
	PegBitmap* src = (PegBitmap*) b;
	PegBitmap* stretched = GET_PEG_THING(ctx)->Screen()->ResizeImage(
			(PegBitmap*) src,
			DISPLAY_INT_TO_PEG_INT(w),
			DISPLAY_INT_TO_PEG_INT(h)
		);

	if (stretched)
	{
		if (src->uFlags & BMF_HAS_TRANS)
		{
			stretched->uFlags |= BMF_HAS_TRANS;
		}
		else
		{
			stretched->uFlags &= ~BMF_HAS_TRANS;
		}
		stretched->dTransColor = src->dTransColor;
	}

	return (stretched);
}

/*---------------------------------------------------------------------------*/
static void PEG_DestroyStretchedBitmap(
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	GET_PEG_THING(ctx)->Screen()->DestroyBitmap((PegBitmap*) bmp);
}


/*---------------------------------------------------------------------------*/
static DISPLAY_INT PEG_GetBitmapWidth (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	return ((PegBitmap*)bmp)->wWidth;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT PEG_GetBitmapHeight (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	return ((PegBitmap*)bmp)->wHeight;
}

/*---------------------------------------------------------------------------*/
static void PEG_DrawText (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		const WebChar* text,
		WebColor textColor,
		WebColor backgroundColor,
		WEBC_BOOL fill,
		void* font
	)
{
	PegPoint pt;
	PegColor Color((COLORVAL) textColor, (COLORVAL) backgroundColor, (fill)? CF_FILL : CF_NONE);

	pt.x = DISPLAY_INT_TO_PEG_INT(x);
	pt.y = DISPLAY_INT_TO_PEG_INT(y);

  #if (defined(PEG_UNICODE) && !WEBC_SUPPORT_UNICODE)
	_PegDrawText8(GET_PEG_THING(ctx), pt, text, Color, (PegFont*) font, tc_strlen(text));
  #else
	GET_PEG_THING(ctx)->DrawText(pt, text, Color, (PegFont*) font);
  #endif
}

/*---------------------------------------------------------------------------*/
static void PEG_DrawTextLen (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		const WebChar* text,
		WebColor textColor,
		WebColor fillColor,
		WEBC_BOOL fill,
		void* font,
		long textLen
	)
{
	PegPoint pt;
	PegColor color((COLORVAL) textColor, (COLORVAL) fillColor, (fill)? CF_FILL : CF_NONE);

	pt.x = DISPLAY_INT_TO_PEG_INT(x);
	pt.y = DISPLAY_INT_TO_PEG_INT(y);

  #if (defined(PEG_UNICODE) && !WEBC_SUPPORT_UNICODE)
	_PegDrawText8(GET_PEG_THING(ctx), pt, text, color, (PegFont*) font, textLen);
  #else
	GET_PEG_THING(ctx)->DrawText(pt, text, color, (PegFont*) font, DISPLAY_INT_TO_PEG_INT(textLen));
  #endif
}

/*---------------------------------------------------------------------------*/
static void PEG_SetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	PegRect clip;

	clip.Set (
			DISPLAY_INT_TO_PEG_INT(rect->left),
			DISPLAY_INT_TO_PEG_INT(rect->top),
			DISPLAY_INT_TO_PEG_INT(rect->right),
			DISPLAY_INT_TO_PEG_INT(rect->bottom)
		);

	GET_PEG_THING(ctx)->mClip = clip;
	if (GET_PEG_THING(ctx)->Presentation()->mReal.Overlap(GET_PEG_THING(ctx)->mClip))
	{
		GET_PEG_THING(ctx)->mClip &= GET_PEG_THING(ctx)->Presentation()->mReal;
	}
	else
	{
		GET_PEG_THING(ctx)->mClip.Set(0,0,-1,-1);
	}
}

/*---------------------------------------------------------------------------*/
static void PEG_GetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	rect->left   = GET_PEG_THING(ctx)->mClip.iLeft;
	rect->top    = GET_PEG_THING(ctx)->mClip.iTop;
	rect->right  = GET_PEG_THING(ctx)->mClip.iRight;
	rect->bottom = GET_PEG_THING(ctx)->mClip.iBottom;
}

/*---------------------------------------------------------------------------*/
static void PEG_GetCanvasRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	rect->left   = GET_PEG_THING(ctx)->mClient.iLeft;
	rect->top    = GET_PEG_THING(ctx)->mClient.iTop;
	rect->right  = GET_PEG_THING(ctx)->mClient.iRight;
	rect->bottom = GET_PEG_THING(ctx)->mClient.iBottom;
}


/*---------------------------------------------------------------------------*/
static WebColor PEG_RGBToColor (
		HTMLGraphicsContext** ctx,
		HTMLColor color,
		HTMLColor* error
	)
{
  #if (PEG_NUM_COLORS > 256)

	return (PEG_RGB (
			HTML_ColorGetRed(color),
			HTML_ColorGetGreen(color),
			HTML_ColorGetBlue(color)
		));

  #elif defined(PEG_GRAYSCALE)

	WEBC_UINT32 i = RGBTOGREY(HTML_ColorGetRed(color),
	                          HTML_ColorGetGreen(color),
	                          HTML_ColorGetBlue(color));

   #if (PEG_NUM_COLORS != 256)
	i = (i * PEG_NUM_COLORS) >> 8;
   #endif

  	if (error)
  	{
		HTML_ColorSet(*error, 0, 0, 0, 0);
	}

	return ((WEBC_UINT8) i);

  #elif (PEG_NUM_COLORS == 256)

	int r = (HTML_ColorGetRed(color)   + 26);
	int g = (HTML_ColorGetGreen(color) + 26);
	int b = (HTML_ColorGetBlue(color)  + 26);

	r = ((r << 2) + r) >> 8;
	g = ((g << 2) + g) >> 8;
	b = ((b << 2) + b) >> 8;

	int index = (r + (g << 2) + (g << 1) + (b << 5) + (b << 2)) + 16;

	// ... is equivalent to this formula:
   #if (0)
	int indexCheck = ((HTML_ColorGetRed(color) + 25) / 51 + (HTML_ColorGetGreen(color) + 25) / 51 * 6 + (HTML_ColorGetBlue(color) + 25) / 51 * 36) + 16;
	WEBC_ASSERT(indexCheck == index);
   #endif
	// but MUCH faster ;-)

	if (error)
	{
		// tbd - three seperate arrays, mpRed, mpGreen, mpBlue
		*error = HTML_RGBAToColor (
				GET_PALETTE(ctx)[index + (index << 1)]     - HTML_ColorGetRed(color),
				GET_PALETTE(ctx)[index + (index << 1) + 1] - HTML_ColorGetGreen(color),
				GET_PALETTE(ctx)[index + (index << 1) + 2] - HTML_ColorGetBlue(color),
				0
			);
	}

	return index;

  #else

	unsigned int n; // sizeof(n) needs to be big enough for color palette size+1
	long lLeastDistance,lThisDistance;

  #if (WEBC_SUPPORT_RGB_TABLE)
    WebColor colorIndex;
	if (RetrieveRGBMatch(&colorIndex, color, error))
	{
		 return (iColorNum);
	}
  #endif

	WEBC_UINT8 *pPalette = (*ctx)->palette;
	WEBC_UINT8 bRed,bGreen,bBlue;

	lLeastDistance=255L*255L*3L; // maximal squared distance value

	if ((*ctx)->paletteSize <= 256)
	{
		for(n=0; n<(*ctx)->paletteSize; n++)
		{
			bRed   = *(pPalette++);
			bGreen = *(pPalette++);
			bBlue  = *(pPalette++);

			lThisDistance=((long)(pColor->red)-(long)bRed)
					*((long)(pColor->red)-(long)bRed)
				+((long)(pColor->green)-(long)bGreen)
					*((long)(pColor->green)-(long)bGreen)
				+((long)(pColor->blue)-(long)bBlue)
					*((long)(pColor->blue)-(long)bBlue);

			if (lThisDistance == 0)
			{
				iColorNum = n;
				error->red = 0;
				error->green = 0;
				error->blue=0;

				break;
			}

			if(lThisDistance<lLeastDistance)
			{
				lLeastDistance=lThisDistance;
				iColorNum=n;
				error->red = (bRed - pColor->red);
				error->green =(bGreen - pColor->green);
				error->blue = (bBlue - pColor->blue);
			}
		}
	}

  #if (WEBC_SUPPORT_RGB_TABLE)
	StoreRGBMatch(iColorNum, pColor, error);
  #endif

	return(iColorNum);

  #endif	// PEG_NUM_COLORS
}

/*---------------------------------------------------------------------------*/
static HTMLColor PEG_ColorToRGB (
		HTMLGraphicsContext** ctx,
		WebColor colorval
	)
{
  #if (PEG_NUM_COLORS > 256)

	return HTML_RGBAToColor(PEG_RED(colorval), PEG_GREEN(colorval), PEG_BLUE(colorval), 0);

  #elif defined(PEG_GRAYSCALE)

	WEBC_UINT32 i = (((WEBC_UINT32)colorval) << 8) / PEG_NUM_COLORS;

   #ifdef WEBIMAGE_INVERT_GREYSCALE
	i = 255 - i;
   #endif

	return HTML_RGBAToColor(i, i, i, 0);

  #else // (PEG_NUM_COLORS <= 256)

	PegThing *pThing=0;
	WEBC_UINT32 ulPaletteSize;
	WEBC_UINT8 *pPalette = pThing->Screen()->GetPalette(&ulPaletteSize);
	ulPaletteSize = EBSMIN(ulPaletteSize,255);
	WEBC_UINT8 bColor = (WEBC_UINT8) EBSCLIP(colorval, 0, ulPaletteSize);
	bColor += bColor << 1;

	return HTML_RGBAToColor(pPalette[bColor], pPalette[bColor+1], pPalette[bColor+2], 0);

  #endif // PEG_NUM_COLORS

	return (0);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT PEG_GetTextHeight (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font
	)
{
	return ((PegFont*)font)->uHeight;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT PEG_GetTextHeightLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font,
		long textLen
	)
{
	return ((PegFont*)font)->uHeight;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT PEG_GetTextWidth (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font
	)
{
  #if (defined(PEG_UNICODE) && !WEBC_SUPPORT_UNICODE)
	DISPLAY_INT width = _PegTextWidth8(GET_PEG_THING(ctx), text, (PegFont*) font, tc_strlen(text));
  #else
	DISPLAY_INT width = GET_PEG_THING(ctx)->Screen()->TextWidth(text, (PegFont*) font);
  #endif

	return (width);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT PEG_GetTextWidthLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font,
		long size
	)
{
  #if (defined(PEG_UNICODE) && !WEBC_SUPPORT_UNICODE)
	DISPLAY_INT width = _PegTextWidth8(GET_PEG_THING(ctx), text, (PegFont*) font, DISPLAY_INT_TO_PEG_INT(size));
  #else
	DISPLAY_INT width = GET_PEG_THING(ctx)->Screen()->TextWidth(text, (PegFont*) font, DISPLAY_INT_TO_PEG_INT(size));
  #endif

	return (width);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT PEG_GetFontHeight (
		HTMLGraphicsContext** ctx,
		void* font
	)
{
	return ((PegFont*)font)->uHeight;
}

/*---------------------------------------------------------------------------*/
static void PEG_InvalidateRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	PegRect invalid;

	invalid.Set (
			DISPLAY_INT_TO_PEG_INT(rect->left),
			DISPLAY_INT_TO_PEG_INT(rect->top),
			DISPLAY_INT_TO_PEG_INT(rect->right),
			DISPLAY_INT_TO_PEG_INT(rect->bottom)
		);

	GET_PEG_THING(ctx)->Invalidate(invalid);
}

/*---------------------------------------------------------------------------*/
static void PEG_BeginDrawing (HTMLGraphicsContext** ctx)
{
	GET_PEG_THING(ctx)->BeginDraw();
}

/*---------------------------------------------------------------------------*/
static void PEG_EndDrawing (HTMLGraphicsContext** ctx)
{
	GET_PEG_THING(ctx)->EndDraw();
}

/*---------------------------------------------------------------------------*/
static void PEG_TileBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect *Rect,
		DISPLAY_INT xOffset,
		DISPLAY_INT yOffset,
		void* bmp
	)
{
	WebRect r = *((WebRect*)Rect);
	WebRect clip;
	PEG_GetClipRect(ctx, (HTMLRect*) &clip);
	r.And(&clip);
	PEG_SetClipRect(ctx, (HTMLRect*) &r);

	PegRect fillRect;

	DISPLAY_INT width  = ((PegBitmap*)bmp)->wWidth;
	DISPLAY_INT height = ((PegBitmap*)bmp)->wHeight;

	xOffset = (xOffset - (r.left - Rect->left)) % width;
	yOffset = (yOffset - (r.top  - Rect->top))  % height;

	fillRect.Set (
			DISPLAY_INT_TO_PEG_INT(r.left + xOffset - width),
			DISPLAY_INT_TO_PEG_INT(r.top  + yOffset - height),
			DISPLAY_INT_TO_PEG_INT(r.right),
			DISPLAY_INT_TO_PEG_INT(r.bottom)
		);

	GET_PEG_THING(ctx)->BitmapFill(fillRect, (PegBitmap*) bmp);

	// restore saved clipping rectangle
	PEG_SetClipRect(ctx, (HTMLRect*) &clip);
}

/*---------------------------------------------------------------------------*/
static void PEG_DrawPatternFrame (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor color,
		DISPLAY_INT thickness,
		CSSBorderStyle pattern
	)
{
	PegColor pegColor((COLORVAL) color, (COLORVAL) color, CF_NONE);

	GET_PEG_THING(ctx)->Screen()->PatternLine(GET_PEG_THING(ctx),
	                               DISPLAY_INT_TO_PEG_INT(rect->left),
	                               DISPLAY_INT_TO_PEG_INT(rect->top),
								   DISPLAY_INT_TO_PEG_INT(rect->right),
								   DISPLAY_INT_TO_PEG_INT(rect->top),
	                               pegColor, DISPLAY_INT_TO_PEG_INT(thickness),
								   _cssBorderStyleToPattern[pattern]);

	GET_PEG_THING(ctx)->Screen()->PatternLine(GET_PEG_THING(ctx),
	                               DISPLAY_INT_TO_PEG_INT(rect->left),
	                               DISPLAY_INT_TO_PEG_INT(rect->bottom),
								   DISPLAY_INT_TO_PEG_INT(rect->right),
								   DISPLAY_INT_TO_PEG_INT(rect->bottom),
	                               pegColor, DISPLAY_INT_TO_PEG_INT(thickness),
								   _cssBorderStyleToPattern[pattern]);

	GET_PEG_THING(ctx)->Screen()->PatternLine(GET_PEG_THING(ctx),
	                               DISPLAY_INT_TO_PEG_INT(rect->left),
	                               DISPLAY_INT_TO_PEG_INT(rect->top + thickness),
	                               DISPLAY_INT_TO_PEG_INT(rect->left),
								   DISPLAY_INT_TO_PEG_INT(rect->bottom - thickness),
	                               pegColor, DISPLAY_INT_TO_PEG_INT(thickness),
								   _cssBorderStyleToPattern[pattern]);

	GET_PEG_THING(ctx)->Screen()->PatternLine(GET_PEG_THING(ctx),
	                               DISPLAY_INT_TO_PEG_INT(rect->right),
	                               DISPLAY_INT_TO_PEG_INT(rect->top + thickness),
	                               DISPLAY_INT_TO_PEG_INT(rect->right),
								   DISPLAY_INT_TO_PEG_INT(rect->bottom - thickness),
	                               pegColor, DISPLAY_INT_TO_PEG_INT(thickness),
								   _cssBorderStyleToPattern[pattern]);
}

/*---------------------------------------------------------------------------*/
static void* PEG_CreateWebBitmap (
		HTMLGraphicsContext** ctx,
		const WebGraphBitmapInfo* info
	)
{
	PegBitmap* pNewBitmap = 0;

	switch (info->pixelFormat)
	{
		case WB_PIXEL_FORMAT_RGB_24:
		case WB_PIXEL_FORMAT_INDEX_8:
		case WB_PIXEL_FORMAT_RGBA_32:
			break;

		default:
			// error: unsupported bit depth (our image converters should
			//  always do 8bit or 24bit)
			return (WEB_BITMAP_NULL);
	}

	WebcBitmapConverter * converter;
	unsigned char * convertedData;
	short bitsPerPixel;
	WebcImageDitherMode ditherMode;

  #if (PEG_NUM_COLORS == 0xffffff)

   #ifdef WEBC_PEG_32_BITS_PER_PIXEL

	bitsPerPixel = 32;
	ditherMode = WEBIMAGE_NO_DITHERING;

	size_t size = info->iWidth * info->iHeight * 4;
	WEBC_NEW(convertedData, unsigned char[size]);

	if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
	{
	  #ifdef BLUE_BYTE_FIRST
		WEBC_NEW(converter, RGB24ToBGRA32());
	  #else
		WEBC_NEW(converter, RGB24ToRGBA32());
	  #endif
	}
	else
	{
		if (info->bHasTransparentColor)
		{
		  #ifdef BLUE_BYTE_FIRST
			WEBC_NEW(converter, indexedToBGRA32(info->pPalette, info->iTransparentIndex));
		  #else
			WEBC_NEW(converter, indexedToRGBA32(info->pPalette, info->iTransparentIndex));
		  #endif
		}
		else
		{
		  #ifdef BLUE_BYTE_FIRST
			WEBC_NEW(converter, indexedToBGRA32(info->pPalette));
		  #else
			WEBC_NEW(converter, indexedToRGBA32(info->pPalette));
		  #endif
		}
	}

   #else
	// 24-bpp screen driver

	bitsPerPixel = 24;
	ditherMode = WEBIMAGE_NO_DITHERING;

	size_t size = info->iWidth * info->iHeight * 3;
	WEBC_NEW(convertedData, unsigned char[size]);

	if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
	{
	  #ifdef BLUE_BYTE_FIRST
		WEBC_NEW(converter, RGB24ToBGR24());
	  #else
		WEBC_NEW(converter, RGB24ToRGB24());
	  #endif
	}
	else
	if (info->pixelFormat == WB_PIXEL_FORMAT_RGBA_32)
	{
	  #ifdef BLUE_BYTE_FIRST
		WEBC_NEW(converter, RGBA32ToBGR24());
	  #else
		WEBC_NEW(converter, RGBA32ToRGB24());
	  #endif
	}
	else
	{
		if (info->bHasTransparentColor)
		{
		  #ifdef BLUE_BYTE_FIRST
			WEBC_NEW(converter, indexedToBGR24((unsigned char *)info->pPalette, info->iTransparentIndex));
		  #else
			WEBC_NEW(converter, indexedToRGB24((unsigned char *)info->pPalette, info->iTransparentIndex));
		  #endif
		}
		else
		{
		  #ifdef BLUE_BYTE_FIRST
			WEBC_NEW(converter, indexedToBGR24((unsigned char *)info->pPalette));
		  #else
			WEBC_NEW(converter, indexedToRGB24((unsigned char *)info->pPalette));
		  #endif
		}
	}
   #endif // WEBC_PEG_32_BITS_PER_PIXEL

  #elif (PEG_NUM_COLORS > 256)

	// 16-bpp screen driver

	bitsPerPixel = 16;
	ditherMode = WEBIMAGE_NO_DITHERING;

	size_t size = info->iWidth * info->iHeight * 2;
	WEBC_NEW(convertedData, unsigned char[size]);

	if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
	{
	  #ifdef RGB_USE_555
		WEBC_NEW(converter, RGB24To555());
	  #else
		WEBC_NEW(converter, RGB24To565());
	  #endif
	}
	else
	{
		if (info->bHasTransparentColor)
		{
		  #ifdef RGB_USE_555
			WEBC_NEW(converter, indexedTo555(info->pPalette, info->iTransparentIndex));
		  #else
			WEBC_NEW(converter, indexedTo565(info->pPalette, info->iTransparentIndex));
		  #endif
		}
		else
		{
		  #ifdef RGB_USE_555
			WEBC_NEW(converter, indexedTo555(info->pPalette));
		  #else
			WEBC_NEW(converter, indexedTo565(info->pPalette));
		  #endif
		}
	}

  #elif defined(PEG_GRAYSCALE)

	ditherMode = WEBIMAGE_NO_DITHERING;

	// tbd - create converters for the various pixel formats.
   #if (PEG_NUM_COLORS == 256)

	// round up nearest even number
	size_t size = info->iWidth * info->iHeight;
	bitsPerPixel = 8;
	converter = 0;
	convertedData = 0;

   #elif (PEG_NUM_COLORS == 16)

	// round up nearest even number
	size_t size = ((info->iWidth + 1) >> 1) * info->iHeight;
	bitsPerPixel = 4;
	converter = 0;
	convertedData = 0;

   #elif (PEG_NUM_COLORS == 4)

	// round up nearest even number
	size_t size = ((info->iWidth + 3) >> 2) * info->iHeight;
	bitsPerPixel = 2;
	converter = 0;
	convertedData = 0;

   #elif (PEG_NUM_COLORS == 2)

	// round up nearest even number
	size_t size = ((info->iWidth + 7) >> 3) * info->iHeight;
	bitsPerPixel = 1;
	converter = 0;
	convertedData = 0;

   #else
    #error Unsupported display type!
   #endif

	WEBC_NEW(convertedData, unsigned char[size]);

	if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
	{
		RGB24ToGreyscale* c;
		WEBC_NEW(c, RGB24ToGreyscale());
		if (c)
		{
			c->setBitDepth((char)bitsPerPixel);
		}
		converter = c;
	}
	else
	{
		indexedToGreyscale* c;
		WEBC_NEW(c, indexedToGreyscale());
		if (c)
		{
			c->setBitDepth((char)bitsPerPixel);
			c->setPalette((unsigned char*)info->pPalette,info->iPaletteSize);  // craig compile hack!!!!!  Question into EBS.
			if (info->bHasTransparentColor)
			{
				c->setTransparentIndex(info->iTransparentIndex);
			}
		}
		converter = c;
	}

  #elif (PEG_NUM_COLORS > 16)

	// 8-bpp screen driver

	bitsPerPixel = 8;
  #if (WEBC_SUPPORT_DITHERING)
	ditherMode = WEBIMAGE_ERROR_DIFFUSION;
  #else
	ditherMode = WEBIMAGE_NO_DITHERING;
  #endif // WEBC_SUPPORT_DITHERING

	size_t size = info->iWidth * info->iHeight;
	WEBC_NEW(convertedData, unsigned char[size]);

	// 8-bpp screen driver
	if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
	{
		WEBC_NEW(converter, RGB24ToIndexed());

		if (converter)
		{
			RGB24ToIndexed* c = (RGB24ToIndexed*) converter;
			c->setGraphicsContext(ctx);
		}
	}
	else
	{
		WEBC_NEW(converter, indexedToIndexed());

		if (converter)
		{
			indexedToIndexed* c = (indexedToIndexed*) converter;
			c->setGraphicsContext(ctx);
			c->setPalette((WEBC_UINT8*) info->pPalette);
			if (info->bHasTransparentColor)
			{
				c->setTransparentIndex(0, info->iTransparentIndex);
			}
		}
	}

  #else
   #error PEG color depth not supported!
  #endif

	if (!converter || !convertedData)
	{
		if (converter)
		{
			WEBC_DELETE(converter);
		}

		if (convertedData)
		{
			WEBC_DELETE(convertedData);
		}

		return (0);
	}

	converter->go(convertedData,
	              (unsigned char*) info->pData,
	              info->iWidth,
	              info->iHeight,
	              ditherMode);

	WEBC_NEW(pNewBitmap, PegBitmap);
	if (pNewBitmap)
	{
		pNewBitmap->uFlags      = (info->bHasTransparentColor)? BMF_HAS_TRANS : 0;
		pNewBitmap->uBitsPix    = (WEBC_UINT8) bitsPerPixel;
		pNewBitmap->wWidth      = DISPLAY_INT_TO_PEG_INT(info->iWidth);
		pNewBitmap->wHeight     = DISPLAY_INT_TO_PEG_INT(info->iHeight);
		pNewBitmap->dTransColor = (info->bHasTransparentColor)? converter->getTransparentColor() : 0;
		pNewBitmap->pStart      = convertedData;
	}
	else
	{
		WEBC_DELETE(convertedData);
	}

	WEBC_DELETE(converter);

  #if defined(ROTATE_CCW) || defined(ROTATE_CW)
	_RotatePegBitmap(pNewBitmap);
  #endif

	return (pNewBitmap);
}

/*---------------------------------------------------------------------------*/
static void PEG_DestroyWebBitmap (
		HTMLGraphicsContext** ctx,
		void* b
	)
{
	PegBitmap* bmp = (PegBitmap*) b;

	if (bmp)
	{
		if (bmp->pStart)
		{
			WEBC_DELETE(bmp->pStart);
		}

		WEBC_DELETE(bmp);
	}
}

/*---------------------------------------------------------------------------*/
#if defined(ROTATE_CCW) || defined(ROTATE_CW)
void _RotatePegBitmap(PegBitmap *pMap)
{
	if(pMap->uBitsPix <= 8)
	{
		DWORD dSize = (DWORD) pMap->wWidth * (DWORD) pMap->wHeight;

		WEBC_UINT8 *pCopy = new WEBC_UINT8[dSize];

		if (!pCopy)
		{
			return;
		}

		memcpy(pCopy, pMap->pStart, dSize);
		WEBC_UINT8 *pGetStart = pCopy;
		WEBC_UINT8 *pPut = pMap->pStart;
		pMap->uFlags |= BMF_ROTATED;

		for (WORD wLoop = 0; wLoop < pMap->wWidth; wLoop++)
		{
			WEBC_UINT8 *pGet = pGetStart;

#ifdef ROTATE_CCW
			pGet += (pMap->wHeight - 1) * pMap->wWidth;
			pGet += wLoop;

			for (WORD wLoop1 = 0; wLoop1 < pMap->wHeight; wLoop1++)
			{
				*pPut++ = *pGet;
				pGet -= pMap->wWidth;
			}
#else
			pGet += wLoop;
			for (WORD wLoop1 = 0; wLoop1 < pMap->wHeight; wLoop1++)
			{
				*pPut++ = *pGet;
				pGet += pMap->wWidth;
			}
#endif
		}
		delete pCopy;
	}

	else if(pMap->uBitsPix==16)
	{
		DWORD dSize = (DWORD) pMap->wWidth * (DWORD) pMap->wHeight * 2;

		WEBC_UINT8 *pCopy = new WEBC_UINT8[dSize];

		if (!pCopy)
		{
			return;
		}

		memcpy(pCopy, pMap->pStart, dSize);
		COLORVAL *pGetStart = (COLORVAL *)pCopy;
		COLORVAL *pPut = (COLORVAL *)pMap->pStart;
		pMap->uFlags |= BMF_ROTATED;

		for (WORD wLoop = 0; wLoop < pMap->wWidth; wLoop++)
		{
			COLORVAL *pGet = pGetStart;

#ifdef ROTATE_CCW
			pGet += (pMap->wHeight - 1) * pMap->wWidth;
			pGet += wLoop;

			for (WORD wLoop1 = 0; wLoop1 < pMap->wHeight; wLoop1++)
			{
				*pPut++ = *pGet;
				pGet -= pMap->wWidth;
			}
#else
			pGet += wLoop;

			for (WORD wLoop1 = 0; wLoop1 < pMap->wHeight; wLoop1++)
			{
				*pPut++ = *pGet;
				pGet += pMap->wWidth;
			}
#endif
		}
		delete pCopy;
	}
}

#endif

/*---------------------------------------------------------------------------*/
static int PEG_Win32ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	)
{
	PegThing *SomeThing = NULL;
	PegMessage pSend;
	PEGINT iStatus = PM_EXIT-1;
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
			if (giMsgPending)
			{
				SomeThing->MessageQueue()->Pop(&pSend);

			  #ifndef PEG_MULTITHREAD

				iStatus = SomeThing->Presentation()->DispatchMessage(SomeThing->Presentation(), &pSend);

				if (iStatus == PM_EXIT)
				{
					*exitCode = iStatus;

					PegMessage NewMessage;
					NewMessage.wType = PM_EXIT;
					NewMessage.pTarget = SomeThing->Presentation();
					NewMessage.pSource = NULL;
					SomeThing->MessageQueue()->Push(NewMessage);

					return (-1);
				}

			  #endif

				return (1);
			}
		}
		else
		{
			PegMessage NewMessage;
			NewMessage.wType = PM_EXIT;
			NewMessage.pTarget = SomeThing->Presentation();
			NewMessage.pSource = NULL;
			SomeThing->MessageQueue()->Push(NewMessage);

			return (-1);
		}
	}

	return (0);
}

/*---------------------------------------------------------------------------*/
static void PEG_ClaimPointer (
		HTMLGraphicsContext** ctx
	)
{
	GET_PEG_THING(ctx)->CapturePointer();
}

/*---------------------------------------------------------------------------*/
static void PEG_ReleasePointer (
		HTMLGraphicsContext** ctx
	)
{
	GET_PEG_THING(ctx)->ReleasePointer();
}

/*---------------------------------------------------------------------------*/
static WebColor PEG_GetColorByIndex (
		HTMLGraphicsContext** ctx,
		int i
	)
{
	static COLORVAL colorMap [] =
	{
		BLACK,          // WGC_BLACK
		RED,            // WGC_RED
		GREEN,          // WGC_GREEN
		BROWN,          // WGC_BROWN
		BLUE,           // WGC_BLUE
		MAGENTA,        // WGC_MAGENTA
		CYAN,           // WGC_CYAN
		LIGHTGRAY,      // WGC_LIGHTGRAY
		DARKGRAY,       // WGC_DARKGRAY
		LIGHTRED,       // WGC_LIGHTRED
		LIGHTGREEN,     // WGC_LIGHTGREEN
		YELLOW,         // WGC_YELLOW
		LIGHTBLUE,      // WGC_LIGHTBLUE
		LIGHTMAGENTA,   // WGC_LIGHTMAGENTA
		LIGHTCYAN,      // WGC_LIGHTCYAN
		WHITE,          // WGC_WHITE
		TRANSPARENCY    // WGC_TRANSPARENT
	};

	return colorMap[i];
}

/*---------------------------------------------------------------------------*/
int webc_InitPegBrowserConfig (HTMLBrowserConfig* config, PegThing* parentThing)
{
	webcPegGraphicsClass.WindowOpen                 = PEG_WindowOpen;
	webcPegGraphicsClass.WindowClose                = PEG_WindowClose;
	webcPegGraphicsClass.MoveTo						= PEG_MoveByorTo;
	webcPegGraphicsClass.ScreenInfo					= PEG_ScreenInfo;

	webcPegGraphicsClass.getCanvasRect              = PEG_GetCanvasRect;

	webcPegGraphicsClass.claimPointer               = PEG_ClaimPointer;
	webcPegGraphicsClass.releasePointer             = PEG_ReleasePointer;

	webcPegGraphicsClass.invalidateRect             = PEG_InvalidateRect;
	webcPegGraphicsClass.beginDrawing               = PEG_BeginDrawing;
	webcPegGraphicsClass.getClipRect                = PEG_GetClipRect;
	webcPegGraphicsClass.setClipRect                = PEG_SetClipRect;
	webcPegGraphicsClass.endDrawing                 = PEG_EndDrawing;

	webcPegGraphicsClass.colorToRGBA                = PEG_ColorToRGB;
	webcPegGraphicsClass.RGBAToColor                = PEG_RGBToColor;
	webcPegGraphicsClass.getColorByIndex            = PEG_GetColorByIndex;

	webcPegGraphicsClass.drawText                   = PEG_DrawText;
	webcPegGraphicsClass.drawTextLen                = PEG_DrawTextLen;
	webcPegGraphicsClass.drawBitmap                 = PEG_DrawBitmap;
	webcPegGraphicsClass.drawBitmapStretchedToRect  = PEG_DrawStretchedBitmap;
	webcPegGraphicsClass.drawRectangle              = PEG_DrawRectangle;

	webcPegGraphicsClass.createFont                 = PEG_CreateFont;
	webcPegGraphicsClass.getDefaultFont             = PEG_GetDefaultFont;
	webcPegGraphicsClass.destroyFont                = PEG_DestroyFont;
	webcPegGraphicsClass.getNumFontFamilies         = PEG_NumFontFamilies;
	webcPegGraphicsClass.getFontFamilyNames         = PEG_GetFontFamilyNames;
	webcPegGraphicsClass.getFontFamilyGenericNames  = PEG_GetFontFamilyGenericNames;
	webcPegGraphicsClass.getTextHeight              = PEG_GetTextHeight;
	webcPegGraphicsClass.getTextWidth               = PEG_GetTextWidth;
	webcPegGraphicsClass.getTextHeightLen           = PEG_GetTextHeightLen;
	webcPegGraphicsClass.getTextWidthLen            = PEG_GetTextWidthLen;
	webcPegGraphicsClass.getFontHeight              = PEG_GetFontHeight;

	webcPegGraphicsClass.createWebBitmap            = PEG_CreateWebBitmap;
	webcPegGraphicsClass.destroyWebBitmap           = PEG_DestroyWebBitmap;
	webcPegGraphicsClass.createStretchedBitmap      = PEG_CreateStretchedBitmap;
	webcPegGraphicsClass.destroyStretchedBitmap     = PEG_DestroyStretchedBitmap;
	webcPegGraphicsClass.getBitmapWidth             = PEG_GetBitmapWidth;
	webcPegGraphicsClass.getBitmapHeight            = PEG_GetBitmapHeight;

	webcPegGraphicsClass.optionalDrawHorizontalLine = 0;
	webcPegGraphicsClass.optionalDrawVerticalLine   = 0;
	webcPegGraphicsClass.optionalDrawStyledFrame    = PEG_DrawPatternFrame;
	webcPegGraphicsClass.optionalDrawBitmapTiled    = PEG_TileBitmap;
	webcPegGraphicsClass.optionalSetCanvasRect      = 0;

	WebcPegGraphics* ctx = (WebcPegGraphics*) WEBC_MALLOC(sizeof(WebcPegGraphics));
	if (ctx)
	{
		rtp_memset(ctx, 0, sizeof(WebcPegGraphics));
		ctx->ctx = &webcPegGraphicsClass;
		ctx->parentThing = parentThing;
	  #if (PEG_NUM_COLORS < 0xffff)
		ctx->palette = parentThing->Screen()->GetPalette(&ctx->paletteSize);;
	  #endif // PEG_NUM_COLORS


		config->graphics = (HTMLGraphicsContext**) ctx;
		config->processInputQueue = &pegProcessInputQueueFn;

		return (0);
	}

	return (-1);

}

/*---------------------------------------------------------------------------*/
void webc_DestroyPegBrowserConfig (HTMLBrowserConfig* config)
{
	WEBC_FREE(config->graphics);
}

/*---------------------------------------------------------------------------*/
WebKey webc_TranslatePegKey         (WEBC_UINT32 pegKey)
{
	if (!pegKeyMapInitialized)
	{
		webc_CreateKeyMap(&pegKeyMap, pegKeyData, WG_NUM_PEG_KEYS);
		pegKeyMapInitialized = WEBC_TRUE;
	}

	return webc_TranslateKey(&pegKeyMap, pegKey);
}
