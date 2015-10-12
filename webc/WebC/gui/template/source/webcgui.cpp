/*
|  WEBCGUI.C -
|
|  EBS -
|
|   $Author: vmalaiya $
|   $Date: 2006/08/31 22:49:21 $
|   $Name:  $
|   $Revision: 1.1 $
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "webcgui.h"
#include "wfgui.h"
#include "webcmem.h"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

typedef struct
{
	HTMLGraphicsContext* ctx;
	void*                nativeObject;
}
WebcGuinameGraphics;

/*****************************************************************************/
/* Function Prototypes
/*****************************************************************************/

static int GUINAME_ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	);

/*****************************************************************************/
/* Data
 *****************************************************************************/

HTMLGraphicsContext webcGuinameGraphicsClass;

int (*guinameProcessInputQueueFn) (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode) = GUINAME_ProcessInputQueue;

#define WG_NUM_GUINAME_KEYS 32

static WEBC_UINT32 guinameKeyData [WG_NUM_GUINAME_KEYS*2] =
{
	WGK_TAB         , TAB        ,
	WGK_CR          , CR         ,
	WGK_PGUP        , PGUP       ,
	WGK_PGDN        , PGDN       ,
	WGK_LNUP        , LNUP       ,
	WGK_LNDN        , LNDN       ,
	WGK_ALT         , ALT        ,
	WGK_CTRL        , CTRL       ,
	WGK_SHIFT       , SHIFT      ,
	WGK_CAPS        , CAPS       ,
	WGK_NUMLOCK     , NUMLOCK    ,
	WGK_SCROLLLOCK  , SCROLLLOCK ,
	WGK_F1          , F1         ,
	WGK_F2          , F2         ,
	WGK_F3          , F3         ,
	WGK_F4          , F4         ,
	WGK_F5          , F5         ,
	WGK_F6          , F6         ,
	WGK_F7          , F7         ,
	WGK_F8          , F8         ,
	WGK_F9          , F9         ,
	WGK_F10         , F10        ,
	WGK_F11         , F11        ,
	WGK_F12         , F12        ,
	WGK_INSERT      , INSERT     ,
	WGK_DELETE      , DELETE     ,
	WGK_BACKSPACE   , BACKSPACE  ,
	WGK_LEFT        , LEFT       ,
	WGK_RIGHT       , RIGHT      ,
	WGK_HOME        , HOME       ,
	WGK_END         , END        ,
	WGK_ESC         , ESC
};

static WebcKeyMap guinameKeyMap;
static WEBC_BOOL  guinameKeyMapInitialized = WEBC_FALSE;

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
static void GUINAME_DrawRectangle (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor outlineColor,
		WebColor fillColor,
		WEBC_BOOL fill
	)
{
}

/*---------------------------------------------------------------------------*/
static void GUINAME_DrawBitmap (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		void* bmp
	)
{
}

/*---------------------------------------------------------------------------*/
static void GUINAME_DrawStretchedBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		void* b
	)
{
}

/*---------------------------------------------------------------------------*/
static void* GUINAME_CreateStretchedBitmap (
		HTMLGraphicsContext** ctx,
		void* b,
		DISPLAY_INT w,
		DISPLAY_INT h
	)
{
	return 0;
}

/*---------------------------------------------------------------------------*/
static void GUINAME_DestroyStretchedBitmap(
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
}


/*---------------------------------------------------------------------------*/
static DISPLAY_INT GUINAME_GetBitmapWidth (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	return 0;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GUINAME_GetBitmapHeight (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	return 0;
}

/*---------------------------------------------------------------------------*/
static void GUINAME_DrawText (
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
}

/*---------------------------------------------------------------------------*/
static void GUINAME_DrawTextLen (
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
}

/*---------------------------------------------------------------------------*/
static void GUINAME_SetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
}

/*---------------------------------------------------------------------------*/
static void GUINAME_GetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
}

/*---------------------------------------------------------------------------*/
static void GUINAME_GetCanvasRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
}


/*---------------------------------------------------------------------------*/
static WebColor GUINAME_RGBToColor (
		HTMLGraphicsContext** ctx,
		HTMLColor color,
		HTMLColor* error
	)
{
	return 0;
}

/*---------------------------------------------------------------------------*/
static HTMLColor GUINAME_ColorToRGB (
		HTMLGraphicsContext** ctx,
		WebColor colorval
	)
{
	return 0;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GUINAME_GetTextHeight (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font
	)
{
	return 0;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GUINAME_GetTextHeightLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font,
		long textLen
	)
{
	return 0;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GUINAME_GetTextWidth (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font
	)
{
	return 0;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GUINAME_GetTextWidthLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font,
		long size
	)
{
	return 0;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GUINAME_GetFontHeight (
		HTMLGraphicsContext** ctx,
		void* font
	)
{
	return 0;
}

/*---------------------------------------------------------------------------*/
static void GUINAME_InvalidateRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
}

/*---------------------------------------------------------------------------*/
static void GUINAME_BeginDrawing (HTMLGraphicsContext** ctx)
{
}

/*---------------------------------------------------------------------------*/
static void GUINAME_EndDrawing (HTMLGraphicsContext** ctx)
{
}

/*---------------------------------------------------------------------------*/
static void GUINAME_TileBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect *Rect,
		DISPLAY_INT xOffset,
		DISPLAY_INT yOffset,
		void* bmp
	)
{
}

/*---------------------------------------------------------------------------*/
static void GUINAME_DrawPatternFrame (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor color,
		DISPLAY_INT thickness,
		CSSBorderStyle pattern
	)
{
}

/*---------------------------------------------------------------------------*/
static void* GUINAME_CreateWebBitmap (
		HTMLGraphicsContext** ctx,
		const WebGraphBitmapInfo* info
	)
{
	return 0;
}

/*---------------------------------------------------------------------------*/
static void GUINAME_DestroyWebBitmap (
		HTMLGraphicsContext** ctx,
		void* b
	)
{
}

/*---------------------------------------------------------------------------*/
static int GUINAME_Win32ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	)
{
	return (0);
}

/*---------------------------------------------------------------------------*/
static void GUINAME_ClaimPointer (
		HTMLGraphicsContext** ctx
	)
{
}

/*---------------------------------------------------------------------------*/
static void GUINAME_ReleasePointer (
		HTMLGraphicsContext** ctx
	)
{
}

/*---------------------------------------------------------------------------*/
static WebColor GUINAME_GetColorByIndex (
		HTMLGraphicsContext** ctx,
		int i
	)
{
	static colorMap [] =
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
int webc_InitGuinameBrowserConfig (HTMLBrowserConfig* config)
{
	webcGuinameGraphicsClass.getCanvasRect              = GUINAME_GetCanvasRect;

	webcGuinameGraphicsClass.claimPointer               = GUINAME_ClaimPointer;
	webcGuinameGraphicsClass.releasePointer             = GUINAME_ReleasePointer;

	webcGuinameGraphicsClass.invalidateRect             = GUINAME_InvalidateRect;
	webcGuinameGraphicsClass.beginDrawing               = GUINAME_BeginDrawing;
	webcGuinameGraphicsClass.getClipRect                = GUINAME_GetClipRect;
	webcGuinameGraphicsClass.setClipRect                = GUINAME_SetClipRect;
	webcGuinameGraphicsClass.endDrawing                 = GUINAME_EndDrawing;

	webcGuinameGraphicsClass.colorToRGBA                = GUINAME_ColorToRGB;
	webcGuinameGraphicsClass.RGBAToColor                = GUINAME_RGBToColor;
	webcGuinameGraphicsClass.getColorByIndex            = GUINAME_GetColorByIndex;

	webcGuinameGraphicsClass.drawText                   = GUINAME_DrawText;
	webcGuinameGraphicsClass.drawTextLen                = GUINAME_DrawTextLen;
	webcGuinameGraphicsClass.drawBitmap                 = GUINAME_DrawBitmap;
	webcGuinameGraphicsClass.drawBitmapStretchedToRect  = GUINAME_DrawStretchedBitmap;
	webcGuinameGraphicsClass.drawRectangle              = GUINAME_DrawRectangle;

	webcGuinameGraphicsClass.createFont                 = GUINAME_CreateFont;
	webcGuinameGraphicsClass.getDefaultFont             = GUINAME_GetDefaultFont;
	webcGuinameGraphicsClass.destroyFont                = GUINAME_DestroyFont;
	webcGuinameGraphicsClass.getNumFontFamilies         = GUINAME_NumFontFamilies;
	webcGuinameGraphicsClass.getFontFamilyNames         = GUINAME_GetFontFamilyNames;
	webcGuinameGraphicsClass.getFontFamilyGenericNames  = GUINAME_GetFontFamilyGenericNames;
	webcGuinameGraphicsClass.getTextHeight              = GUINAME_GetTextHeight;
	webcGuinameGraphicsClass.getTextWidth               = GUINAME_GetTextWidth;
	webcGuinameGraphicsClass.getTextHeightLen           = GUINAME_GetTextHeightLen;
	webcGuinameGraphicsClass.getTextWidthLen            = GUINAME_GetTextWidthLen;
	webcGuinameGraphicsClass.getFontHeight              = GUINAME_GetFontHeight;

	webcGuinameGraphicsClass.createWebBitmap            = GUINAME_CreateWebBitmap;
	webcGuinameGraphicsClass.destroyWebBitmap           = GUINAME_DestroyWebBitmap;
	webcGuinameGraphicsClass.createStretchedBitmap      = GUINAME_CreateStretchedBitmap;
	webcGuinameGraphicsClass.destroyStretchedBitmap     = GUINAME_DestroyStretchedBitmap;
	webcGuinameGraphicsClass.getBitmapWidth             = GUINAME_GetBitmapWidth;
	webcGuinameGraphicsClass.getBitmapHeight            = GUINAME_GetBitmapHeight;

	webcGuinameGraphicsClass.optionalDrawHorizontalLine = 0;
	webcGuinameGraphicsClass.optionalDrawVerticalLine   = 0;
	webcGuinameGraphicsClass.optionalDrawStyledFrame    = GUINAME_DrawPatternFrame;
	webcGuinameGraphicsClass.optionalDrawBitmapTiled    = GUINAME_TileBitmap;

	WebcGuinameGraphics* ctx = (WebcGuinameGraphics*) WEBC_MALLOC(sizeof(WebcGuinameGraphics));
	if (ctx)
	{
		ctx->ctx = &webcGuinameGraphicsClass;

		rtp_memset(config, 0, sizeof(HTMLBrowserConfig));
		config->graphics = (HTMLGraphicsContext**) ctx;
		config->processInputQueue = &guinameProcessInputQueueFn;

		return (0);
	}

	return (-1);

}

/*---------------------------------------------------------------------------*/
void webc_DestroyGuinameBrowserConfig (HTMLBrowserConfig* config)
{
	WEBC_FREE(config->graphics);
}

/*---------------------------------------------------------------------------*/
WebKey webc_TranslateGuinameKey         (WEBC_UINT32 rawKey)
{
	if (!guinameKeyMapInitialized)
	{
		webc_CreateKeyMap(&guinameKeyMap, guinameKeyData, WG_NUM_GUINAME_KEYS);
		guinameKeyMapInitialized = WEBC_TRUE;
	}

	return webc_TranslateKey(&guinameKeyMap, rawKey);
}
