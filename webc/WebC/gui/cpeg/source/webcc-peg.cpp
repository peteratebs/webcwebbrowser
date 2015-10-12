#ifdef WEBC_USE_CPEG
#define WEBC_PEG_190_OR_LATER
/*
|  WEBCC-PEG.CPP -
|
|  EBS -
|
|  Copyright EBS Inc. , 2008
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "webcc-peg.h"
#include "webgraph.hpp"
#include "util.h"
#include "imgconv.hpp"
#include "webcmem.h"


#include "wfcpeg.h"
#include "rtpstr.h"
#include "webcassert.h"
#include "webimage.hpp"
#include "webcfg.h"
extern "C" {
#include "cpeg_resample.h"
}
#if (PEG_NUM_COLORS == 0xffffff)
 #ifdef WEBC_PEG_32_BITS_PER_PIXEL
  #include "24to32.hpp"
  #include "8to32.hpp"
 #else
  #include "24to24.hpp"
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
 #include "8to24.hpp"

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

#define GET_PEG_THING(X)    (((WebcCPegGraphics*)(X))->parentThing)
#define GET_PALETTE(X)      (((WebcCPegGraphics*)(X))->palette)
#define GET_PALETTE_SIZE(X) (((WebcCPegGraphics*)(X))->paletteSize)
#define GET_CLIP_RECT(X) (&(((WebcCPegGraphics*)(X))->clipRect))
//#define GET_CLIP_RECT(X) (&(((PegPanel*)(((WebcCPegGraphics*)(X))->parentThing))->Clip))
#define GET_CLIENT_RECT(X) (&(((PegPanel*)(((WebcCPegGraphics*)(X))->parentThing))->Client))
#define GET_REAL_RECT(X) (&(((PegPanel*)(((WebcCPegGraphics*)(X))->parentThing))->Real))


/*****************************************************************************/
/* Types
 *****************************************************************************/

// webcPegBitmap is an extension of PEG's PegBitmap so we can store our own data
class webcPegBitmap : public PegBitmap
{
public:
	unsigned char *m_pOrigData;
};

/*****************************************************************************/
/* Function Prototypes
/*****************************************************************************/

#if defined(ROTATE_CCW) || defined(ROTATE_CW)
static void _RotatePegBitmap(PegBitmap *pBitmap);
#endif

static int CPEG_Win32ProcessInputQueue (
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
		WEBC_INT32* exitCode) = CPEG_Win32ProcessInputQueue;

#if (WEBC_SUPPORT_RGB_TABLE && 0)
 static RGBMatch gColorMatchTable[1024] = {0};
#endif // WEBC_SUPPORT_RGB_TABLE

static HTMLGraphicsContext webcCPegGraphicsClass = {0};

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
static void CPEG_DrawRectangle (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor outlineColor,
		WebColor fillColor,
		WEBC_BOOL fill
	)
{
    PegBrush brush;
	PegRect where;

   #if (PEG_NUM_COLORS == 2)
    PegBrushSet(&brush, WHITE, BLACK, CF_FILL, 1);
   #elif (PEG_NUM_COLORS >= 256)
	PegBrushSet(&brush, (PEGCOLORVAL) outlineColor, (PEGCOLORVAL) fillColor, (fill) ? CF_FILL : CF_NONE, 1);
   #endif   /* PEG_NUM_COLORS == 2 */

	// set the clip rectangle
	PegBrushClipRectSet(&brush, GET_CLIP_RECT(ctx));

    PegRectSet(&where,  
			DISPLAY_INT_TO_PEG_INT(rect->left),
			DISPLAY_INT_TO_PEG_INT(rect->top),
			DISPLAY_INT_TO_PEG_INT(rect->right),
			DISPLAY_INT_TO_PEG_INT(rect->bottom)
		);

	PegDrawRectangle(GET_PEG_THING(ctx),&where, &brush);
}

/*---------------------------------------------------------------------------*/
static void CPEG_DrawBitmap (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		void* bmp
	)
{
	PegPoint pt;
    PegBrush brush;

	pt.x = DISPLAY_INT_TO_PEG_INT(x);
	pt.y = DISPLAY_INT_TO_PEG_INT(y);

    PegBrushFlagsSet(&brush, CF_NONE);
    PegBrushBitmapSet(&brush, (PegBitmap*) bmp);
	// set the clip rectangle
	PegBrushClipRectSet(&brush, GET_CLIP_RECT(ctx));

	PegDrawBitmap(GET_PEG_THING(ctx), &pt, &brush, FALSE);
}

/*---------------------------------------------------------------------------*/
static void CPEG_DrawStretchedBitmap (
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

	// do we actually need to resize?
	if (w != bmp->sWidth || h != bmp->sHeight)
	{
#if 0
// TODO it's not obvious how image resize is supported
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
#endif
	}
	else // bypass resize
	{
		PegBrush brush;
		PegBrushFlagsSet(&brush, CF_NONE);
		PegBrushBitmapSet(&brush, bmp);
		// set the clip rectangle
		PegBrushClipRectSet(&brush, GET_CLIP_RECT(ctx));
		PegDrawBitmap(GET_PEG_THING(ctx), &pt, &brush, FALSE);
	}
}

/*---------------------------------------------------------------------------*/
static void* CPEG_CreateStretchedBitmap (
		HTMLGraphicsContext** ctx,
		void* b,
		DISPLAY_INT w,
		DISPLAY_INT h)
{
	webcPegBitmap* src = (webcPegBitmap*) b;
	webcPegBitmap* stretched = NULL;
	
	WEBC_NEW(stretched, webcPegBitmap);
	if (stretched)
	{
		stretched->ubFlags = src->ubFlags;
		stretched->ubBitsPix    = src->ubBitsPix;
		stretched->sWidth      = w;
		stretched->sHeight     = h;
		stretched->ulTransColor = src->ulTransColor;
		stretched->pStart      = NULL;
		stretched->m_pOrigData = NULL;
	}

	if (stretched)
	{
#if (PEG_NUM_COLORS > 256) || defined(PEG_GRAYSCALE)
		stretched->pStart = (PEGUBYTE*) WEBC_MALLOC(w * h * src->ubBitsPix / 8);
		if(stretched->pStart)
		{
			rszImage srcDef, dstDef;
			srcDef.xsize = src->sWidth;
			srcDef.ysize = src->sHeight;
			srcDef.channels = src->ubBitsPix / 8;
			srcDef.span = src->sWidth * srcDef.channels;
			srcDef.data = src->pStart;

			dstDef.xsize = stretched->sWidth;
			dstDef.ysize = stretched->sHeight;
			dstDef.channels = src->ubBitsPix / 8;
			dstDef.span = stretched->sWidth * dstDef.channels;
			dstDef.data = stretched->pStart;

			resample(&dstDef, &srcDef, filter, 2.0);
		}
#else // (PEG_NUM_COLORS <= 256)

		stretched->pStart = (PEGUBYTE*) WEBC_MALLOC(w * h * src->ubBitsPix / 8);
		if(stretched->pStart)
		{
			rszImage srcDef, dstDef;
			size_t srcSize = src->sWidth * src->sHeight;
			size_t dstSize = stretched->sWidth * stretched->sHeight * 3;
			unsigned char * origData = stretched->m_pOrigData; // we put the 24-bit image here when we created this bitmap
			unsigned char *pTempStretched = NULL, *pTempFull = NULL;
			RGB24ToIndexed *pConverter=NULL;
#if (WEBC_SUPPORT_DITHERING)
			WebcImageDitherMode ditherMode = WEBIMAGE_ERROR_DIFFUSION;
#else
			WebcImageDitherMode ditherMode = WEBIMAGE_NO_DITHERING;
#endif // WEBC_SUPPORT_DITHERING

			if(!origData) //  if the original image isn't available, do a conversion
			{
				indexedToRGB24 *p8To24Converter=NULL;

				WEBC_NEW(p8To24Converter, indexedToRGB24());

				// convert 8 bit images to 24
				pTempFull = (unsigned char *) WEBC_MALLOC(srcSize * 3);
				if (p8To24Converter && pTempFull)
				{
					WEBC_UINT32 ulPaletteSize;
					WEBC_UINT8 *pPalette = PegPaletteGet(&ulPaletteSize);
					p8To24Converter->setPalette(pPalette);
					p8To24Converter->go(pTempFull, src->pStart, src->sWidth, src->sHeight, WEBIMAGE_NO_DITHERING);
					WEBC_DELETE(p8To24Converter);
				}
			}

			pTempStretched  = (unsigned char *) WEBC_MALLOC(dstSize);

			srcDef.xsize = src->sWidth;
			srcDef.ysize = src->sHeight;
			srcDef.channels = 3;
			srcDef.span = src->sWidth * srcDef.channels;
			srcDef.data = pTempFull ? pTempFull : origData;

			dstDef.xsize = stretched->sWidth;
			dstDef.ysize = stretched->sHeight;
			dstDef.channels = 3;
			dstDef.span = stretched->sWidth * dstDef.channels;
			dstDef.data = pTempStretched;

			// do the actual resize in 24-bit space
//			resample(&dstDef, &srcDef, filter, 2.0);
printf("Resample and filter are unresolved\n");
			if(pTempFull)
				WEBC_FREE(pTempFull);

			WEBC_NEW(pConverter, RGB24ToIndexed());

			// convert back to 8 bit
			if (pConverter)
			{
				pConverter->setGraphicsContext(ctx);
				pConverter->go(stretched->pStart, pTempStretched, stretched->sWidth, stretched->sHeight, ditherMode);
				WEBC_DELETE(pConverter);
				WEBC_FREE(pTempStretched);
			}

		}

#endif
		
	}
	return ((void *) stretched);
}

/*---------------------------------------------------------------------------*/
static void CPEG_DestroyStretchedBitmap(
		HTMLGraphicsContext** ctx,
		void* bmp)
{
	webcPegBitmap* stretched = (webcPegBitmap*) bmp;
	if (stretched)
	{
		if(stretched->m_pOrigData)
			WEBC_FREE(stretched->m_pOrigData);
		WEBC_FREE(stretched->pStart);
	}
}


/*---------------------------------------------------------------------------*/
static DISPLAY_INT CPEG_GetBitmapWidth (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	return ((PegBitmap*)bmp)->sWidth;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT CPEG_GetBitmapHeight (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	return ((PegBitmap*)bmp)->sHeight;
}

/*---------------------------------------------------------------------------*/
static void CPEG_DrawText (
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
    PegBrush brush;

   #if (PEG_NUM_COLORS == 2)
    PegBrushSet(&brush, WHITE, BLACK, CF_FILL, 1);
   #elif (PEG_NUM_COLORS >= 256)
	PegBrushSet(&brush, (PEGCOLORVAL) textColor, (PEGCOLORVAL) backgroundColor, (fill) ? CF_FILL : CF_NONE, 1);
   #endif   /* PEG_NUM_COLORS == 2 */

	// set the clip rectangle
	PegBrushClipRectSet(&brush, GET_CLIP_RECT(ctx));

	pt.x = DISPLAY_INT_TO_PEG_INT(x);
	pt.y = DISPLAY_INT_TO_PEG_INT(y);


  #if (defined(PEG_UNICODE) && !WEBC_SUPPORT_UNICODE)
	_PegDrawText8(GET_PEG_THING(ctx), &pt, text, &brush, (PegFont*) font, tc_strlen(text));
  #else
	PegDrawText(GET_PEG_THING(ctx), &pt, (PEGCHAR *) text, &brush, (PegFont*) font, PEG_TEXT_ALL);
  #endif
}

/*---------------------------------------------------------------------------*/
static void CPEG_DrawTextLen (
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
    PegBrush brush;

   #if (PEG_NUM_COLORS == 2)
    PegBrushSet(&brush, WHITE, BLACK, CF_FILL, 1);
   #elif (PEG_NUM_COLORS >= 256)
	PegBrushSet(&brush, (PEGCOLORVAL) textColor, (PEGCOLORVAL) fillColor, (fill) ? CF_FILL : CF_NONE, 1);
   #endif   /* PEG_NUM_COLORS == 2 */

	// set the clip rectangle
	PegBrushClipRectSet(&brush, GET_CLIP_RECT(ctx));

	pt.x = DISPLAY_INT_TO_PEG_INT(x);
	pt.y = DISPLAY_INT_TO_PEG_INT(y);

  #if (defined(PEG_UNICODE) && !WEBC_SUPPORT_UNICODE)
	_PegDrawText8(GET_PEG_THING(ctx), pt, text, brush, (PegFont*) font, textLen);
  #else
	PegDrawText(GET_PEG_THING(ctx), &pt, (PEGCHAR *)text, &brush, (PegFont*) font, DISPLAY_INT_TO_PEG_INT(textLen));
  #endif
}

/*---------------------------------------------------------------------------*/
static void CPEG_SetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
    PegRectSet(GET_CLIP_RECT(ctx),  
			DISPLAY_INT_TO_PEG_INT(rect->left),
			DISPLAY_INT_TO_PEG_INT(rect->top),
			DISPLAY_INT_TO_PEG_INT(rect->right),
			DISPLAY_INT_TO_PEG_INT(rect->bottom)
		);
}

/*---------------------------------------------------------------------------*/
static void CPEG_GetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	rect->left   = GET_CLIP_RECT(ctx)->sLeft;
	rect->top    = GET_CLIP_RECT(ctx)->sTop;
	rect->right  = GET_CLIP_RECT(ctx)->sRight;
	rect->bottom = GET_CLIP_RECT(ctx)->sBottom;
}

/*---------------------------------------------------------------------------*/
static void CPEG_GetCanvasRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	rect->left   = GET_CLIENT_RECT(ctx)->sLeft;
	rect->top    = GET_CLIENT_RECT(ctx)->sTop;
	rect->right  = GET_CLIENT_RECT(ctx)->sRight;
	rect->bottom = GET_CLIENT_RECT(ctx)->sBottom;
}


/*---------------------------------------------------------------------------*/
static WebColor CPEG_RGBToColor (
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
static HTMLColor CPEG_ColorToRGB (
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
	WEBC_UINT8 *pPalette = PegPaletteGet(&ulPaletteSize);
	ulPaletteSize = EBSMIN(ulPaletteSize,255);
	WEBC_UINT8 bColor = (WEBC_UINT8) EBSCLIP(colorval, 0, ulPaletteSize);
	bColor += bColor << 1;

	return HTML_RGBAToColor(pPalette[bColor], pPalette[bColor+1], pPalette[bColor+2], 0);

  #endif // PEG_NUM_COLORS

	return (0);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT CPEG_GetTextHeight (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font
	)
{
	return ((PegFont*)font)->ubHeight;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT CPEG_GetTextHeightLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font,
		long textLen
	)
{
	return ((PegFont*)font)->ubHeight;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT CPEG_GetTextWidth (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font
	)
{
#if (defined(PEG_UNICODE) && !WEBC_SUPPORT_UNICODE)
	DISPLAY_INT width = _PegTextWidth8(GET_PEG_THING(ctx), text, (PegFont*) font, tc_strlen(text));
#else
	DISPLAY_INT width = PegTextWidth(text, (PegFont*) font, PEG_TEXT_ALL);
#endif

	return (width);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT CPEG_GetTextWidthLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font,
		long size
	)
{
#if (defined(PEG_UNICODE) && !WEBC_SUPPORT_UNICODE)
	DISPLAY_INT width = _PegTextWidth8(GET_PEG_THING(ctx), text, (PegFont*) font, DISPLAY_INT_TO_PEG_INT(size));
#else
	DISPLAY_INT width = PegTextWidth(text, (PegFont*) font, DISPLAY_INT_TO_PEG_INT(size));
#endif

	return (width);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT CPEG_GetFontHeight (
		HTMLGraphicsContext** ctx,
		void* font
	)
{
	return ((PegFont*)font)->ubHeight;
}

/*---------------------------------------------------------------------------*/
static void CPEG_InvalidateRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	PegRect invalid;

	PegRectSet(&invalid,
			DISPLAY_INT_TO_PEG_INT(rect->left),
			DISPLAY_INT_TO_PEG_INT(rect->top),
			DISPLAY_INT_TO_PEG_INT(rect->right),
			DISPLAY_INT_TO_PEG_INT(rect->bottom)
		);

	PegRectRegionInvalidate(&invalid);
}

/*---------------------------------------------------------------------------*/
static void CPEG_BeginDrawing (HTMLGraphicsContext** ctx)
{
PegThing *pThing = GET_PEG_THING(ctx);

    PegDrawBegin(pThing);
}

/*---------------------------------------------------------------------------*/
static void CPEG_EndDrawing (HTMLGraphicsContext** ctx)
{
    PegDrawEnd(GET_PEG_THING(ctx));
}

/*---------------------------------------------------------------------------*/
static void CPEG_TileBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect *Rect,
		DISPLAY_INT xOffset,
		DISPLAY_INT yOffset,
		void* bmp
	)
{
	WebRect r = *((WebRect*)Rect);
	WebRect clip;
	PegRect fillRect;
    PegBrush brush;

	CPEG_GetClipRect(ctx, (HTMLRect*) &clip);
	r.And(&clip);
	CPEG_SetClipRect(ctx, (HTMLRect*) &r);

	DISPLAY_INT width  = ((PegBitmap*)bmp)->sWidth;
	DISPLAY_INT height = ((PegBitmap*)bmp)->sHeight;

	xOffset = (xOffset - (r.left - Rect->left)) % width;
	yOffset = (yOffset - (r.top  - Rect->top))  % height;

	PegRectSet(&fillRect,
			DISPLAY_INT_TO_PEG_INT(r.left + xOffset - width),
			DISPLAY_INT_TO_PEG_INT(r.top  + yOffset - height),
			DISPLAY_INT_TO_PEG_INT(r.right),
			DISPLAY_INT_TO_PEG_INT(r.bottom)
		);

    PegBrushFlagsSet(&brush, CF_NONE);
    PegBrushBitmapSet(&brush, (PegBitmap*) bmp);
	// set the clip rectangle
	PegBrushClipRectSet(&brush, GET_CLIP_RECT(ctx));

	PegDrawBitmapFill(GET_PEG_THING(ctx), &brush);

	// restore saved clipping rectangle
	CPEG_SetClipRect(ctx, (HTMLRect*) &clip);
}

/*---------------------------------------------------------------------------*/
static void CPEG_DrawPatternFrame (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor color,
		DISPLAY_INT thickness,
		CSSBorderStyle pattern
	)
{
	PegBrush brush;

	PegBrushSet(&brush, (PEGCOLORVAL) color, (PEGCOLORVAL) color, CF_NONE, thickness);
	PegBrushPatternSet(&brush, _cssBorderStyleToPattern[pattern]);


void PegDrawLine(void *pThing, PEGINT ixStart, PEGINT iyStart, PEGINT ixEnd, PEGINT iyEnd, PegBrush *pBrush);


	PegDrawLine(GET_PEG_THING(ctx),
	                               DISPLAY_INT_TO_PEG_INT(rect->left),
	                               DISPLAY_INT_TO_PEG_INT(rect->top),
								   DISPLAY_INT_TO_PEG_INT(rect->right),
								   DISPLAY_INT_TO_PEG_INT(rect->top),
	                               &brush);

	PegDrawLine(GET_PEG_THING(ctx),
	                               DISPLAY_INT_TO_PEG_INT(rect->left),
	                               DISPLAY_INT_TO_PEG_INT(rect->bottom),
								   DISPLAY_INT_TO_PEG_INT(rect->right),
								   DISPLAY_INT_TO_PEG_INT(rect->bottom),
	                               &brush);

	PegDrawLine(GET_PEG_THING(ctx),
	                               DISPLAY_INT_TO_PEG_INT(rect->left),
	                               DISPLAY_INT_TO_PEG_INT(rect->top + thickness),
	                               DISPLAY_INT_TO_PEG_INT(rect->left),
								   DISPLAY_INT_TO_PEG_INT(rect->bottom - thickness),
	                               &brush);

	PegDrawLine(GET_PEG_THING(ctx),
	                               DISPLAY_INT_TO_PEG_INT(rect->right),
	                               DISPLAY_INT_TO_PEG_INT(rect->top + thickness),
	                               DISPLAY_INT_TO_PEG_INT(rect->right),
								   DISPLAY_INT_TO_PEG_INT(rect->bottom - thickness),
	                               &brush);
}

/*---------------------------------------------------------------------------*/
static void* CPEG_CreateWebBitmap (
		HTMLGraphicsContext** ctx,
		const WebGraphBitmapInfo* info
	)
{
	webcPegBitmap* pNewBitmap = 0;

	switch (info->pixelFormat)
	{
		case WB_PIXEL_FORMAT_RGB_24:
		case WB_PIXEL_FORMAT_INDEX_8:
			break;

		default:
			// error: unsupported bit depth (our image converters should
			//  always do 8bit or 24bit)
			return (WEB_BITMAP_NULL);
	}

	WebcBitmapConverter * converter;
	unsigned char * convertedData;
	unsigned char * origData=NULL;
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

	// we're being passed a 24-bit image that we need to downsample
	if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
	{
		size_t srcSize = info->iHeight * info->iBytesPerRow;
		WEBC_NEW(origData, unsigned char[srcSize] );
		if(origData)
		{
			// make a copy of the original, full-depth image data in 
			// case we need to stretch it later
			memcpy(origData, info->pData, srcSize);
		}

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

	WEBC_NEW(pNewBitmap, webcPegBitmap);
	if (pNewBitmap)
	{
		pNewBitmap->ubFlags = (info->bHasTransparentColor)? BMF_HAS_TRANS : 0;
		pNewBitmap->ubBitsPix    = (WEBC_UINT8) bitsPerPixel;
		pNewBitmap->sWidth      = DISPLAY_INT_TO_PEG_INT(info->iWidth);
		pNewBitmap->sHeight     = DISPLAY_INT_TO_PEG_INT(info->iHeight);
		pNewBitmap->ulTransColor = (info->bHasTransparentColor)? converter->getTransparentColor() : 0;
		pNewBitmap->pStart      = convertedData;
		pNewBitmap->m_pOrigData = origData;
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
static void CPEG_DestroyWebBitmap (
		HTMLGraphicsContext** ctx,
		void* b
	)
{
	webcPegBitmap* bmp = (webcPegBitmap*) b;

	if (bmp)
	{
		if (bmp->m_pOrigData)
		{
			WEBC_DELETE(bmp->m_pOrigData);
		}
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
static int CPEG_Win32ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	)
{
	PegThing *SomeThing = NULL;
	PegMessage pSend;
  #if defined(WEBC_PEG_190_OR_LATER)
	PEGINT iStatus = PM_EXIT-1;
  #else
	SIGNED iStatus = PM_EXIT-1;
  #endif
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

				PegMessageQueuePop(PegMessageQueuePtr(), &pSend);

			  #ifndef PEG_MULTITHREAD

				iStatus = PegMessageDispatch(PegPresentationPtr(), &pSend);

				if (iStatus == PM_EXIT)
				{
					*exitCode = iStatus;

					PegMessage NewMessage;
					NewMessage.usType = PM_EXIT;
					NewMessage.pTarget = PegPresentationPtr();
					NewMessage.pSource = NULL;
					PegMessageQueuePush(PegMessageQueuePtr(), &NewMessage);

					return (-1);
				}

			  #endif

				return (1);
			}
		}
		else
		{
			PegMessage NewMessage;
			NewMessage.usType = PM_EXIT;
			NewMessage.pTarget = PegPresentationPtr();
			NewMessage.pSource = NULL;
			PegMessageQueuePush(PegMessageQueuePtr(), &NewMessage);

			return (-1);
		}
	}

	return (0);
}

/*---------------------------------------------------------------------------*/
static void CPEG_ClaimPointer (
		HTMLGraphicsContext** ctx
	)
{
	PegPointerCapture(GET_PEG_THING(ctx));
}

/*---------------------------------------------------------------------------*/
static void CPEG_ReleasePointer (
		HTMLGraphicsContext** ctx
	)
{
	PegPointerRelease(GET_PEG_THING(ctx));
}

/*---------------------------------------------------------------------------*/
static WebColor CPEG_GetColorByIndex (
		HTMLGraphicsContext** ctx,
		int i
	)
{
	static PEGCOLORVAL colorMap [] =
	{
		PCLR_BLACK,          // WGC_BLACK
		PCLR_RED,            // WGC_RED
		PCLR_GREEN,          // WGC_GREEN
		PCLR_BROWN,          // WGC_BROWN
		PCLR_BLUE,           // WGC_BLUE
		PCLR_MAGENTA,        // WGC_MAGENTA
		PCLR_CYAN,           // WGC_CYAN
		PCLR_LIGHTGRAY,      // WGC_LIGHTGRAY
		PCLR_DARKGRAY,       // WGC_DARKGRAY
		PCLR_LIGHTRED,       // WGC_LIGHTRED
		PCLR_LIGHTGREEN,     // WGC_LIGHTGREEN
		PCLR_YELLOW,         // WGC_YELLOW
		PCLR_LIGHTBLUE,      // WGC_LIGHTBLUE
		PCLR_LIGHTMAGENTA,   // WGC_LIGHTMAGENTA
		PCLR_LIGHTCYAN,      // WGC_LIGHTCYAN
		PCLR_WHITE,          // WGC_WHITE
		PCLR_TRANSPARENT    // WGC_TRANSPARENT
	};

	return colorMap[i];
}

/*---------------------------------------------------------------------------*/
int webc_InitCPegBrowserConfig (HTMLBrowserConfig* config, PegThing* parentThing)
{
	webcCPegGraphicsClass.getCanvasRect              = CPEG_GetCanvasRect;

	webcCPegGraphicsClass.claimPointer               = CPEG_ClaimPointer;
	webcCPegGraphicsClass.releasePointer             = CPEG_ReleasePointer;

	webcCPegGraphicsClass.invalidateRect             = CPEG_InvalidateRect;
	webcCPegGraphicsClass.beginDrawing               = CPEG_BeginDrawing;
	webcCPegGraphicsClass.getClipRect                = CPEG_GetClipRect;
	webcCPegGraphicsClass.setClipRect                = CPEG_SetClipRect;
	webcCPegGraphicsClass.endDrawing                 = CPEG_EndDrawing;

	webcCPegGraphicsClass.colorToRGBA                = CPEG_ColorToRGB;
	webcCPegGraphicsClass.RGBAToColor                = CPEG_RGBToColor;
	webcCPegGraphicsClass.getColorByIndex            = CPEG_GetColorByIndex;

	webcCPegGraphicsClass.drawText                   = CPEG_DrawText;
	webcCPegGraphicsClass.drawTextLen                = CPEG_DrawTextLen;
	webcCPegGraphicsClass.drawBitmap                 = CPEG_DrawBitmap;
	webcCPegGraphicsClass.drawBitmapStretchedToRect  = CPEG_DrawStretchedBitmap;
	webcCPegGraphicsClass.drawRectangle              = CPEG_DrawRectangle;

#if 1
	// TODO ???
	webcCPegGraphicsClass.createFont                 = CPEG_CreateFont; 
	webcCPegGraphicsClass.getDefaultFont             = CPEG_GetDefaultFont;
	webcCPegGraphicsClass.destroyFont                = CPEG_DestroyFont;
	webcCPegGraphicsClass.getNumFontFamilies         = CPEG_NumFontFamilies;
	webcCPegGraphicsClass.getFontFamilyNames         = CPEG_GetFontFamilyNames;
	webcCPegGraphicsClass.getFontFamilyGenericNames  = CPEG_GetFontFamilyGenericNames;
	///////////
#endif

	webcCPegGraphicsClass.getTextHeight              = CPEG_GetTextHeight;
	webcCPegGraphicsClass.getTextWidth               = CPEG_GetTextWidth;
	webcCPegGraphicsClass.getTextHeightLen           = CPEG_GetTextHeightLen;
	webcCPegGraphicsClass.getTextWidthLen            = CPEG_GetTextWidthLen;
	webcCPegGraphicsClass.getFontHeight              = CPEG_GetFontHeight;

	webcCPegGraphicsClass.createWebBitmap            = CPEG_CreateWebBitmap;
	webcCPegGraphicsClass.destroyWebBitmap           = CPEG_DestroyWebBitmap;
	webcCPegGraphicsClass.createStretchedBitmap      = CPEG_CreateStretchedBitmap;
	webcCPegGraphicsClass.destroyStretchedBitmap     = CPEG_DestroyStretchedBitmap;
	webcCPegGraphicsClass.getBitmapWidth             = CPEG_GetBitmapWidth;
	webcCPegGraphicsClass.getBitmapHeight            = CPEG_GetBitmapHeight;

	webcCPegGraphicsClass.optionalDrawHorizontalLine = 0;
	webcCPegGraphicsClass.optionalDrawVerticalLine   = 0;
	webcCPegGraphicsClass.optionalDrawStyledFrame    = CPEG_DrawPatternFrame;
	webcCPegGraphicsClass.optionalDrawBitmapTiled    = CPEG_TileBitmap;
	webcCPegGraphicsClass.optionalSetCanvasRect      = 0;

	WebcCPegGraphics* ctx = (WebcCPegGraphics*) WEBC_MALLOC(sizeof(WebcCPegGraphics));
	if (ctx)
	{
		ctx->ctx = &webcCPegGraphicsClass;
		ctx->parentThing = parentThing;
	  #if (PEG_NUM_COLORS < 0xffff)
		ctx->palette = PegPaletteGet(&ctx->paletteSize);
	  #endif // PEG_NUM_COLORS

		rtp_memset(config, 0, sizeof(HTMLBrowserConfig));
		config->graphics = (HTMLGraphicsContext**) ctx;
		config->processInputQueue = &pegProcessInputQueueFn;

		return (0);
	}

	return (-1);

}

/*---------------------------------------------------------------------------*/
void webc_DestroyCPegBrowserConfig (HTMLBrowserConfig* config)
{
	WEBC_FREE(config->graphics);
}

/*---------------------------------------------------------------------------*/
WebKey webc_TranslateCPegKey         (WEBC_UINT32 pegKey)
{
	if (!pegKeyMapInitialized)
	{
		webc_CreateKeyMap(&pegKeyMap, pegKeyData, WG_NUM_PEG_KEYS);
		pegKeyMapInitialized = WEBC_TRUE;
	}

	return webc_TranslateKey(&pegKeyMap, pegKey);
}


//===================
PEGINT KeyPadNotify(void *pThing, const PegMessage *pMesg);
static void WebcMain(PegThing *pThing);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegPanel *KeyPadCreate(void)
{
    PegRect r;
    PegRectSet(&r, 0, 0, 320, 240);
    PegPanel *pp = PegPanelCreate(&r, 100, FF_THIN);
    PegFuncPtrSet(pp, PFP_NOTIFY, KeyPadNotify);

#if (PEG_NUM_COLORS > 256)
#elif (PEG_NUM_COLORS > 16)
    PegColorSet(pp, PCI_NORMAL, 224);
#else
    PegColorSet(pp, PCI_NORMAL, PCLR_BUTTON_FACE);
#endif  /* PEG_NUM_COLORS */

    /* add the title */
//    PegAdd(pp, PegTitleCreate("Enter the date", 100, TT_COPY), FALSE);

	WebcMain((PegThing *)pp);
    return(pp);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEGINT KeyPadNotify(void *pThing, const PegMessage *pMesg)
{
    /* this is where most of the work gets done. If the
     * message was a clicked signal from one of the key pad buttons
     * that cause input data, then we pass the key message to
     * the object which currently has keyboard focus. Notice that
     * we don't bother getting the text from the object and
     * concatenating it ourselves. That way, it behaves exactly 
     * the same whether the input really came from a keyboard, 
     * or from the keypad input button.
     *
     * This 'if' statement works because we lined up the ID's 
     * of the buttons in the KeyPadChildIds enum above.
     */
	switch (pMesg->usType) {

	case PM_POINTER_MOVE:
		printf("Mouse move x,y = %d %d \n", pMesg->u.Point.x, pMesg->u.Point.y);
		break;
	case PM_LBUTTONDOWN:     /* 10 */
		printf("Mouse LDOWN x,y = %d %d \n", pMesg->u.Point.x, pMesg->u.Point.y);
		break;
	case PM_LBUTTONUP:
		printf("Mouse LU x,y = %d %d \n", pMesg->u.Point.x, pMesg->u.Point.y);
		break;
	case PM_RBUTTONDOWN:
		printf("Mouse RDOWN\n");
		break;
	case PM_RBUTTONUP:
		printf("Mouse RUP\n");
		break;
	case PM_KEY:
		printf("Mouse KEYDOWN KEY == %d\n", pMesg->sData);
		break;
	case PM_KEY_RELEASE:
		printf("Mouse KEYUP KEY == %d\n", pMesg->sData);
		break;
	case PM_DRAW:
		printf("Draw \n");
//		return -1;

	}
    return(PegPanelNotify(pThing, pMesg));

}

void RedirectIOToConsole(void);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
extern "C" void PegAppInitialize(PegPresentation *pPresentation)
{
	RedirectIOToConsole();
    PegAdd(pPresentation, KeyPadCreate(), FALSE);

}


// ===============
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



#include <stdio.h>
#define UNICODE
#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include "webc.h"

//#include "FBAbstraction.h"
#include "webcmem.h"
#include "webcLog.h"

//#define RTP_MEM_DEBUG_PRINTF
//#define RTP_TRACK_LOCAL_MEMORY
#define RTP_MEM_RESTRAIN 0
#include "rtpmem.h"
#include "rtpprint.h"

int webc_GuiInitConfig(HTMLBrowserConfig* config);

/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/


/*****************************************************************************/
// Local Types
/*****************************************************************************/


HBROWSER_HANDLE gBrowser[3] = {0,0,0};

// Forward declarations of functions included in this code module:
ATOM				WebcDemoRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
#ifdef GDIFRAMEBUFFER
LRESULT CALLBACK messageReceive(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#else
/* Window proc provided by abstraction layer */
LRESULT CALLBACK WebcWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

HBROWSER_HANDLE webc_start_chrome_browser(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow);
HBROWSER_HANDLE webc_start_webc_demo(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow);
HBROWSER_HANDLE webc_start_webc_tests(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow, char * pTestsuiteURL);
HBROWSER_HANDLE webc_start_webc_splash(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow);
void webcApiExampleRun(void);
extern "C" void StartEMglDemo(void);
extern "C" void TestEmglBrowserStart(void);
extern "C" int DoMainMenu;
extern "C" int DoWebcRestart;
void DoRestartWebc(PegThing *pThing);
#if(WEBC_SUPPORT_ERROR_PAGE_REDIRECT)
static int webc_redirectInit(void);
#endif //WEBC_SUPPORT_ERROR_PAGE_REDIRECT


extern "C" int rtp_net_init (void);

static void WebcMain(PegThing *pThing)
{
	HTMLBrowserConfig config;
    RECT screenRect;

	rtp_net_init();
//    StartEMglForWebc();
	// create the directories
//	CreateDirectoryA(WEBC_CFG_CACHE_PREFIX, NULL);
//	CreateDirectoryA(WEBC_CFG_OFFLINE_CACHE_PREFIX, NULL);
	// WebC library initialization
	webc_Init();
#if (WEBC_SUPPORT_ERROR_PAGE_REDIRECT)
    if(!webc_redirectInit())
	{
		return FALSE;
	}
#endif

	DoRestartWebc(pThing);
}
void * start_webc_browser_from_cpeg(HTMLBrowserConfig *config,PegThing *pThing, char * pURL);

static void PegThingDraw(void *pThing)
{
HTMLRect rect;
HTMLGraphicsContext *pg;
rect.top = 20;
rect.bottom=80;
rect.left=20;
rect.right = 80;
 PegDrawBegin(pThing);
 //===
{
    PegBrush brush;
	PegRect where;

	PegBrushSet(&brush, (PEGCOLORVAL) 128, (PEGCOLORVAL) 128, CF_FILL, 1);

    PegRectSet(&where,  
			DISPLAY_INT_TO_PEG_INT(rect.left),
			DISPLAY_INT_TO_PEG_INT(rect.top),
			DISPLAY_INT_TO_PEG_INT(rect.right),
			DISPLAY_INT_TO_PEG_INT(rect.bottom)
		);

	PegDrawRectangle((PegThing *)pThing,&where, &brush);
}
 
 PegDrawEnd(pThing);
	printf("In peg draw\n");
}
void DoRestartWebc(PegThing *pThing)
{
char *url="file://\\dev\\EMGL\\EmglDemo\\content\\index.html";
PegRect Rect;
HTMLGraphicsContext** ctx;
struct HTMLBrowserConfig config;
const WebChar *title;
int		  x, y, width, urlBarHeight, height;
WEBC_UINT8 *hWndParent = NULL;
HBROWSER_HANDLE gBrowser;
PegScreen *pPegScreen = PegScreenPtr();
	Rect = pThing->Client;
	PegFuncPtrSet(pThing, PFP_DRAW, PegThingDraw);
	memset(&config,sizeof(config),0);
	/* User supplied parameters */
	config.WindowConfig.left = Rect.sLeft;
	config.WindowConfig.top =  Rect.sTop;
	config.WindowConfig.height =  PegRectHeight(&Rect);
	config.WindowConfig.width =   PegRectWidth(&Rect);
	config.WindowTitle= (WEBC_CHAR *)"WebC";
    urlBarHeight=0;
	/* Open al the windows and then put webC inside the client area of the webC window. */
	/* WindowExample() re-initializes so do not set any properties or styles before this point */

	config.WindowHandle=(void *)pThing;
	/* Start webc now that the console is up and running */
//	rtp_net_init ();
	if (webc_InitCPegBrowserConfig(&config, pThing) >= 0)
	{
//	EMGLWebGraphicsContext *ctx;
		webc_SetBrowserWindow(config.BrowserHandle, (void *)pThing);

//		EMGLDisplayObjectQueueTimedEvent(pDivObject,50);
//		EMGLDisplayObjectSetFocusObject(pDivObject);
     }
	gBrowser = (HBROWSER_HANDLE) start_webc_browser_from_cpeg(&config, pThing, url);		
//	EMGLDisplayObjectSetPropertyPointer(pDivObject,EMGL_PROPERTY_USER_DATA, (void *) gBrowser);
	//FB_InCDoDisplay();
	PegDraw(pThing);
}
PegThing *pGlobalThing;
void * start_webc_browser_from_cpeg(HTMLBrowserConfig *config,PegThing *pThing, char * pURL)
{
    HBROWSER_HANDLE browser;
    HDOC_HANDLE document;
    //config->browserEventHandler = browserHandler;
//	config->WindowConfig.
	config->WindowHandle=(void *) pThing;
    config->browserEventHandler = 0;
    config->privateData = 0;
    browser = webc_CreateBrowser(config,  0, "Explore" , "");
    document = webc_BrowserGetDocument(browser);
	
	pGlobalThing=pThing;

    if (document)
    {
		webc_BrowserLoadUrl(browser, pURL);
    }
	return (void *)browser;
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


#if(WEBC_SUPPORT_ERROR_PAGE_REDIRECT)
static int webc_redirectInit(void)
{
	/* The following url can follow any protocol i.e. local://, http://, file:// etc..*/
	if(webc_setErrorRedirect(URL_ERROR_SERVER_NOT_FOUND, "local://servernotfound.html")<0)
	{
		rtp_printf("webc_setErrorRedirect failed\r\n");
		return FALSE; // error
	}
	if(webc_setErrorRedirect(URL_ERROR_PAGE_NOT_FOUND, "local://pagenotfound.html"/*"http://10.1.10.6:8080/tsd_new/home.html"*/)<0)
    {
		rtp_printf("webc_setErrorRedirect failed\r\n");
		return FALSE; // error
	}
	if(webc_setErrorRedirect(URL_ERROR_PAYMENT_REQ, "local://paymentreq.html"/*"http://10.1.10.6:8080/tsd_new/home2.html"*/)<0)
	{
		rtp_printf("webc_setErrorRedirect failed\r\n");
		return FALSE; // error
	}
	if(webc_setErrorRedirect(URL_ERROR_FORBIDDEN, "local://forbidden.html"/*"http://10.1.10.6:8080/tsd_new/home2.html"*/)<0)
	{
		rtp_printf("webc_setErrorRedirect failed\r\n");
		return FALSE; // error
	}
	return TRUE;
}
#endif

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


int webc_GuiInitConfig(struct HTMLBrowserConfig *p)
{
	return 0;
}
void guiWmBringToTop(struct s_HTMLGraphicsContext **p){}
void guiWmGetPos(struct s_HTMLGraphicsContext **p,int *a,int *b,int *c,int *d){};
void guiWmSetPos(struct s_HTMLGraphicsContext **p,int a,int b,int c,int d){}
extern "C" void webc_user_alert_box(char *str){}
void webc_user_debug_window(char *str){}
extern "C" void EMGLSysSoftKeyBoardShow(){}

#endif /* WEBC_USE_PEG */
