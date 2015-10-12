/*
|  WEBCGDIP.CPP -
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

#include "webcgdip.h"
#include "wfgdip.h"
#include "webcmem.h"
#include "webrect.hpp"
#include "rtpstr.h"
#include "webcassert.h"
#include "webcstr.h"
#include "webcfg.h"
#define UNICODE
#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

#if (!WEBC_SUPPORT_UNICODE)
#error GDI plus integration may only be run with WEBC_SUPPORT_UNICODE=1
#endif

extern int giAllowTimerTick;

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

typedef struct
{
	HTMLGraphicsContext* ctx;
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

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

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
			{0.0,0.0,0.0,1.0,0.0},
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
				stretched->cachedBitmap = new CachedBitmap(stretchedBmp, GET_GRAPHICS_OBJ(ctx));

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
static void _GDIPlus_MeasureTextWithFallback (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		const WebChar* text,
		long textLen,
		void* f,
		RectF* rect
	)
{
	Font* font = (Font*) f;
	Font altFont(L"Arial Unicode MS", font->GetSize(), font->GetStyle(), font->GetUnit());

	XY_TO_POINT(where, x, y);
	const WebChar* segmentStart = text;
	long segmentLength = 0;
	WEBC_BOOL firstSegment = WEBC_TRUE;
	RectF tempRect;

	if (textLen < 0)
	{
		textLen = webc_strlen(text);
	}

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
				GET_GRAPHICS_OBJ(ctx)->MeasureDriverString (
						segmentStart,
						segmentLength,
						font,
						&where,
						DriverStringOptionsRealizedAdvance|DriverStringOptionsCmapLookup,
						NULL,
						rect
					);
			}
			else
			{
				GET_GRAPHICS_OBJ(ctx)->MeasureDriverString (
						segmentStart,
						segmentLength,
						font,
						&where,
						DriverStringOptionsRealizedAdvance|DriverStringOptionsCmapLookup,
						NULL,
						&tempRect
					);

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
				GET_GRAPHICS_OBJ(ctx)->MeasureDriverString (
						segmentStart,
						segmentLength,
						&altFont,
						&where,
						DriverStringOptionsRealizedAdvance|DriverStringOptionsCmapLookup,
						NULL,
						rect
					);
			}
			else
			{
				GET_GRAPHICS_OBJ(ctx)->MeasureDriverString (
						segmentStart,
						segmentLength,
						&altFont,
						&where,
						DriverStringOptionsRealizedAdvance|DriverStringOptionsCmapLookup,
						NULL,
						&tempRect
					);

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
		const WebChar* segmentStart,
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

	where.X += *xOffset;
	where.Y += y - tempRect.Y;

	if (doFill)
	{
		tempRect.X = where.X;
		tempRect.Y = (REAL) y;
		tempRect.Height = fontHeight;

		GET_GRAPHICS_OBJ(ctx)->FillRectangle(fillBrush, tempRect);
	}

	GET_GRAPHICS_OBJ(ctx)->DrawDriverString (
			segmentStart,
			segmentLength,
			font,
			textBrush,
			&where,
			DriverStringOptionsRealizedAdvance|DriverStringOptionsCmapLookup,
			NULL
		);

	*xOffset += tempRect.Width;
}

/*---------------------------------------------------------------------------*/
static void _GDIPlus_DrawTextWithFallback (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		const WebChar* text,
		long textLen,
		void* f,
		WebColor textColor,
		WebColor fillColor,
		WEBC_BOOL fill
	)
{
	Font* font = (Font*) f;
	Font altFont(L"Arial Unicode MS", font->GetSize(), font->GetStyle(), font->GetUnit());

	const WebChar* segmentStart = text;
	long segmentLength = 0;
	RectF tempRect;
	SolidBrush textBrush(textColor);
	SolidBrush fillBrush(fillColor);
	REAL xOffset = 0;
	REAL fontHeight = (REAL) GDIPlus_GetFontHeight(ctx, f);

	if (textLen < 0)
	{
		textLen = webc_strlen(text);
	}

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

	static bitCountMap[] =
	{
		24, // WB_PIXEL_FORMAT_RGB_24       = 0,  // 24bit color, stored red-green-blue
		8,  // WB_PIXEL_FORMAT_INDEX_8      = 1,  // palette indexed, 8 bits per pixel
		8,  // WB_PIXEL_FORMAT_MONOCHROME_8 = 2,  // monochrome, 8 bits per pixel
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

			newBitmap->cachedBitmap = new CachedBitmap(newBitmap->bitmap, GET_GRAPHICS_OBJ(ctx));
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
int webc_InitGDIPlusBrowserConfig (HTMLBrowserConfig* config, HWND windowHandle)
{
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
		ctx->windowHandle = windowHandle;
		ctx->outputGraphics = ctx->windowGraphics;
		ctx->blendLevelInt = 255;
		ctx->blendLevelReal = 1.0;

		rtp_memset(config, 0, sizeof(HTMLBrowserConfig));
		config->graphics = (HTMLGraphicsContext**) ctx;
		config->processInputQueue = &gdiplusProcessInputQueueFn;

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
