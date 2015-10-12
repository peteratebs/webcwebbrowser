/*
|  WEBCGUI.C -
|
|  EBSnet -
|
|   $Author: manish $
|   $Date: 2006/10/17 06:09:26 $
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

#include "webcwndml.h"
#include "webcmem.h"
#include "webrect.hpp"
#include "webcassert.h"
#include "wcconst.hpp"
#include "webfonts.hpp"

#include "rtpdebug.h"

#include <stdio.h>
#include "stdlib.h"

#if !defined(WEBC_BUFFER_SCROLLING)
/* Set double buffering mode..
	WINDML_PAGE_MODE_DOUBLE_BUFFER uses windML page mode
	WINDML_BMP_MODE_DOUBLE_BUFFER  uses a bitmap for buffering
	WINDML_BMP_MODE_DOUBLE_BUFFER  Is the default. It seems appears to be
	higher performance 	but the best method may depend on confiuration
Select only one
*/
#define WINDML_PAGE_MODE_DOUBLE_BUFFER	0
#define WINDML_BMP_MODE_DOUBLE_BUFFER	1
#else
#define WINDML_PAGE_MODE_DOUBLE_BUFFER	0
#define WINDML_BMP_MODE_DOUBLE_BUFFER	0
#endif

#define WNDML_VERBOSE 0
#define WNDML_BMP_VERBOSE 0
#define WNDML_CLIP_VERBOSE 0
#define WNDML_BUFFER_VERBOSE 0

/*****************************************************************************/
/* Macros
 *****************************************************************************/
#define GET_WINDML_DEVICE_ID(C) (((WebcWindMLGraphics*)(C))->m_DevId)
#define GET_WINDML_SERVICE_ID(C) (((WebcWindMLGraphics*)(C))->m_InputServiceId)
#define GET_WINDML_DEVICE_COLORS(C) (((WebcWindMLGraphics*)(C))->m_pDeviceAllocColors)
#define GET_WINDML_GRAPHICS_CONTEXT(C) (((WebcWindMLGraphics*)(C))->m_gc)

#define WEBC_SMALLEST_FONT_SIZE_PT   6
#define WEBC_LARGEST_FONT_SIZE_PT    36

/*****************************************************************************/
/* Types
 *****************************************************************************/

typedef struct
{
	HTMLGraphicsContext* ctx;

	UGL_DEVICE_ID m_DevId;
	UGL_INPUT_SERVICE_ID m_InputServiceId;
	UGL_FONT_DRIVER_ID m_FontDrvId;
	UGL_COLOR *m_pDeviceAllocColors;
	UGL_GC_ID m_gc;
#if (WINDML_PAGE_MODE_DOUBLE_BUFFER)
	UGL_PAGE_ID page[2];
	int page_number;
#endif
#if (WINDML_BMP_MODE_DOUBLE_BUFFER)
	UGL_DDB_ID DoubleBufferBitmapId;
#endif
	UGL_INT32 m_iScreenColors;

	WebRect invalidRect;
	int drawNesting;
}
WebcWindMLGraphics;


/*****************************************************************************/
/* Function Prototypes
/*****************************************************************************/

static int WindML_ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	);

/*****************************************************************************/
/* Data
 *****************************************************************************/
static UGL_ARGB * pScreenClut = UGL_NULL;

HTMLGraphicsContext webcWindMLGraphicsClass;

int (*WindMLProcessInputQueueFn) (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode) = WindML_ProcessInputQueue;

#define WG_NUM_WindML_KEYS 32

static WEBC_UINT32 WindMLKeyData [WG_NUM_WindML_KEYS*2] =
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

static WebcKeyMap WindMLKeyMap;
static WEBC_BOOL  WindMLKeyMapInitialized = WEBC_FALSE;


static WEBC_READONLY WEBC_CHAR* gpFontFamilyName[WEBC_CFG_MAX_FONT_FAMILIES] = {
		WEBC_STR_ARIAL,
		WEBC_STR_COURIER,
		WEBC_STR_TIMES,
		WEBC_STR_VERDANA
	};

static WEBC_READONLY WEBC_CHAR* gpFontFamilyGenericName[WEBC_CFG_MAX_FONT_FAMILIES] = {
		WEBC_STR_SANS_SERIF,
		WEBC_STR_MONOSPACE,
		WEBC_STR_SERIF,
		WEBC_STR_SANS_SERIF
	};

static UGL_FONT_ID* gWindMLFontFamilyTable[WEBC_CFG_MAX_FONT_FAMILIES] = {
		0
	};


static UGL_FONT_ID gWindMLDefaultFont = UGL_NULL;

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

#if (WINDML_PAGE_MODE_DOUBLE_BUFFER)
static void WindML_CreateDoubleBuffer (HTMLGraphicsContext** ctx)
{
	WebcWindMLGraphics* tempCtx = (WebcWindMLGraphics*) ctx;
	tempCtx->page[0] = uglPageCreate (tempCtx->m_DevId);
	tempCtx->page[1] = uglPageCreate (tempCtx->m_DevId);
	tempCtx->page_number = 0;
	uglPageDrawSet (tempCtx->m_DevId, tempCtx->page[0]);
	uglPageVisibleSet (tempCtx->m_DevId, tempCtx->page[1]);
}

static void WindML_DrawDoubleBuffer (HTMLGraphicsContext** ctx)
{
	int current_page, new_page;
	WebcWindMLGraphics* tempCtx = (WebcWindMLGraphics*) ctx;

	new_page = current_page = tempCtx->page_number;
	new_page += 1;
	if (new_page == 2)
		new_page = 0;
	uglPageVisibleSet (tempCtx->m_DevId, tempCtx->page[current_page]);
	uglPageCopy(tempCtx->m_DevId,tempCtx->page[current_page],tempCtx->page[new_page]);
	uglPageDrawSet (tempCtx->m_DevId, tempCtx->page[new_page]);
	tempCtx->page_number = new_page;
}

static void WindML_DestroyDoubleBuffer (HTMLGraphicsContext** ctx)
{
/* Must be implemented when dynamic windows are supported */
}

#endif

#if (WINDML_BMP_MODE_DOUBLE_BUFFER)
static void WindML_GetCanvasRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	);

static void WindML_CreateDoubleBuffer (HTMLGraphicsContext** ctx)
{
	UGL_DIB localDIB;
	WebcWindMLGraphics* tempCtx = (WebcWindMLGraphics*) ctx;
	WebRect canvasRect;

	if (tempCtx->DoubleBufferBitmapId)
		return;

	WindML_GetCanvasRect(ctx, (HTMLRect*) &canvasRect);
	localDIB.width = canvasRect.Width();
	localDIB.height = canvasRect.Height();
	localDIB.stride = canvasRect.Width();
	localDIB.imageFormat = UGL_DIRECT;
	localDIB.colorFormat = UGL_ARGB8888; // Peter
    localDIB.stride = canvasRect.Width();
	localDIB.clutSize = 0;
	localDIB.pClut = UGL_NULL;
	localDIB.pImage = 0;
	/* Set with left and right switwed top and bottom switched to indicate empty and allow And()
	   method to work correctly */
	tempCtx->invalidRect.Set(canvasRect.right,canvasRect.bottom,canvasRect.left,canvasRect.top);
	tempCtx->DoubleBufferBitmapId = uglBitmapCreate(tempCtx->m_DevId, &localDIB, UGL_DIB_INIT_VALUE, 0, UGL_NULL);
	uglDefaultBitmapSet(GET_WINDML_GRAPHICS_CONTEXT(ctx),tempCtx->DoubleBufferBitmapId);
}

static void WindML_DrawDoubleBuffer (HTMLGraphicsContext** ctx)
{
	int current_page, new_page;
	WebcWindMLGraphics* tempCtx = (WebcWindMLGraphics*) ctx;
	if (tempCtx->invalidRect.Empty())
		return;
#if (WNDML_VERBOSE || WNDML_BUFFER_VERBOSE)
	printf("Bliiting %d,%d,%d,%d\n",
		tempCtx->invalidRect.left, /* left boundary of the source rect */
		tempCtx->invalidRect.top,/* top boundary of the source rect */
		tempCtx->invalidRect.right,/* right boundary of the source rect */
		tempCtx->invalidRect.bottom/* bottom boundary of the source rect */
	);
#endif

	uglBitmapBlt(
		GET_WINDML_GRAPHICS_CONTEXT(ctx),
		tempCtx->DoubleBufferBitmapId, /* source bitmap */
		tempCtx->invalidRect.left, /* left boundary of the source rect */
		tempCtx->invalidRect.top,/* top boundary of the source rect */
		tempCtx->invalidRect.right,/* right boundary of the source rect */
		tempCtx->invalidRect.bottom,/* bottom boundary of the source rect */
		UGL_DISPLAY_ID, 			/* destination bitmap */
		tempCtx->invalidRect.left,  /* left boundary of destination */
		tempCtx->invalidRect.top    /* top boundary of destination */
	);
}
static void WindML_DestroyDoubleBuffer (HTMLGraphicsContext** ctx)
{
/* Must be implemented when dynamic windows are supported */
}
#endif // WINDML_BMP_MODE_DOUBLE_BUFFER

/*---------------------------------------------------------------------------*/
static void WindML_DrawRectangle (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor outlineColor,
		WebColor fillColor,
		WEBC_BOOL fill
	)
{
	UGL_COLOR tempForegroundColor;
	UGL_COLOR tempBackgroundColor;

	if( uglForegroundColorGet( GET_WINDML_GRAPHICS_CONTEXT(ctx), &tempForegroundColor) != UGL_STATUS_OK)
	{
	}

	if( tempForegroundColor != outlineColor)
	{
		if( uglForegroundColorSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), outlineColor) != UGL_STATUS_OK)
		{
		}
	}
	if (!fill)
	{
		fillColor = UGL_COLOR_TRANSPARENT; // ByIndex (ctx, 16);
	}

	if( uglBackgroundColorGet( GET_WINDML_GRAPHICS_CONTEXT(ctx), &tempBackgroundColor) != UGL_STATUS_OK)
	{
	}
	if( tempBackgroundColor != fillColor)
	{
		if( uglBackgroundColorSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), fillColor) != UGL_STATUS_OK)
		{
		}
	}
#if (WNDML_VERBOSE)
	printf("Draw uglRectangle [%d %d %d %d]\n",rect->left, rect->top, rect->right, rect->bottom);
#endif
	if( uglBatchStart( GET_WINDML_GRAPHICS_CONTEXT(ctx)) != UGL_STATUS_OK)
	{
	}

	if( uglRectangle( GET_WINDML_GRAPHICS_CONTEXT(ctx), rect->left, rect->top, rect->right, rect->bottom) != UGL_STATUS_OK)
	{
	}

	if( uglBatchEnd( GET_WINDML_GRAPHICS_CONTEXT(ctx)) != UGL_STATUS_OK)
	{
	}

	if( tempForegroundColor != outlineColor)
	{
		if( uglForegroundColorSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), tempForegroundColor) != UGL_STATUS_OK)
		{
		}
	}
	{
		if( tempBackgroundColor != fillColor)
		{
			if( uglBackgroundColorSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), tempBackgroundColor) != UGL_STATUS_OK)
			{
			}
		}
	}
}


/*---------------------------------------------------------------------------*/
static void WindML_DrawBitmap (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		void* bmp
	)
{
	WebcWindMLBitmap* wcBmp = (WebcWindMLBitmap*) bmp;
	WebcWindMLBitmap * wcBmpUnstretched = (WebcWindMLBitmap *)wcBmp->punstretched;

#if (WNDML_VERBOSE || WNDML_BMP_VERBOSE)
	printf("Draw bmp (%X) at  [x %d,y %d,w %d,h %d]\n",bmp, x, y, wcBmp->m_iWidth, wcBmp->m_iHeight);
#endif

	if( uglBatchStart( GET_WINDML_GRAPHICS_CONTEXT(ctx)) != UGL_STATUS_OK)
	{
	}

	if (wcBmpUnstretched == wcBmp)
	{
		if (uglBitmapBlt( GET_WINDML_GRAPHICS_CONTEXT(ctx), wcBmp->m_DDBitmap, 0, 0, wcBmp->m_iWidth, wcBmp->m_iHeight, UGL_DEFAULT_ID, x, y) != UGL_STATUS_OK)
		{
		}
    }
    else
    {
		if (uglBitmapStretchBlt( GET_WINDML_GRAPHICS_CONTEXT(ctx), wcBmpUnstretched->m_DDBitmap, 0, 0, wcBmpUnstretched->m_iWidth, wcBmpUnstretched->m_iHeight, UGL_DEFAULT_ID,
				x, y, x+wcBmp->m_iWidth-1,  (y+wcBmp->m_iHeight-1)) != UGL_STATUS_OK)
		{

		}
	}

	if( uglBatchEnd( GET_WINDML_GRAPHICS_CONTEXT(ctx)) != UGL_STATUS_OK)
	{
	}
}

/*---------------------------------------------------------------------------*/

static void WindML_TileBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect *rect,
		DISPLAY_INT xOffset,
		DISPLAY_INT yOffset,
		void* bmp
	)
{
	WebcWindMLBitmap * wcBmp = (WebcWindMLBitmap*) bmp;
	WebcWindMLBitmap * wcBmpUnstretched = (WebcWindMLBitmap *)wcBmp->punstretched;
	WebRect *pwr;
	WebRect clip;
	WebRect r;
	pwr = (WebRect*) rect;
	r = *pwr;

	if (wcBmpUnstretched != wcBmp)	/* Not supporting stretched bmps as Tiled background */
		return;

	if (wcBmp->m_iWidth < 1 || wcBmp->m_iHeight < 1) /* Bad things would happen */
		return;

	if( uglBatchStart( GET_WINDML_GRAPHICS_CONTEXT(ctx)) != UGL_STATUS_OK)
	{
		return;
	}
	if( uglClipRectGet( GET_WINDML_GRAPHICS_CONTEXT(ctx), (UGL_POS *)&(clip.left), (UGL_POS *)&(clip.top), (UGL_POS *)&(clip.right), (UGL_POS *)&(clip.bottom)) != UGL_STATUS_OK)
	{
	}
	r.And(&clip);
	if( uglClipRectSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), (UGL_POS)(r.left), (UGL_POS)(r.top), (UGL_POS)(r.right), (UGL_POS)(r.bottom)) != UGL_STATUS_OK)
	{
	}

UGL_POS x, x_offset, prev_width, width;

	/* Find the x offset into the source bitmap for the first blit
		The xOffset argument specifies pixel offset in the source to start from
		r.left is the destination pixel */
	x_offset = ((r.left+xOffset) % wcBmp->m_iWidth);
	width = wcBmp->m_iWidth - x_offset;
	for (x = r.left; x <= r.right; x += prev_width)
	{
		if ((x + width) > r.right)
			width = (r.right - x) + 1;

UGL_POS y, y_offset, prev_height, height;
		/* Find the Y offset into the source and the height for the first blit */

		/* Find the y offset into the source bitmap for the first blit
			The yOffset argument specifies pixel offset in the source to start from
			r.top is the destination pixel */
		y_offset = ((r.top+yOffset) % wcBmp->m_iHeight);
		height = wcBmp->m_iHeight - y_offset;
		for (y = r.top; y <= r.bottom; y += prev_height)
		{
			if ((y + height) > r.bottom)
				height = (r.bottom - y) + 1;

			if (uglBitmapBlt(GET_WINDML_GRAPHICS_CONTEXT(ctx),
				wcBmp->m_DDBitmap,
				x_offset, /* left boundary of the source rect */
				y_offset,/* top boundary of the source rect */
				(width+x_offset-1),/* right boundary of the source rect */
				(height+y_offset-1),/* bottom boundary of the source rect */
				UGL_DEFAULT_ID,	/* destination bitmap */
				x,  /* left boundary of destination */
				y    /* top boundary of destination */
				)!= UGL_STATUS_OK)
				{
				}
				/* Now increment start point by the height */
				prev_height = height;
				/* The next image will be on a Y boundary */
				y_offset = 0;
				height = wcBmp->m_iHeight;

		}
		/* Now increment start point by the width */
		prev_width = width;
		/* The next image will be on a x boundary */
		x_offset = 0;
		width = wcBmp->m_iWidth;
	}
	if( uglClipRectSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), (UGL_POS)(clip.left), (UGL_POS)(clip.top), (UGL_POS)(clip.right), (UGL_POS)(clip.bottom)) != UGL_STATUS_OK)
	{
	}
	if( uglBatchEnd( GET_WINDML_GRAPHICS_CONTEXT(ctx)) != UGL_STATUS_OK)
	{
	}
}


/*---------------------------------------------------------------------------*/
static void WindML_DrawStretchedBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		void* b
	)
{
	WebcWindMLBitmap* wcBmp = (WebcWindMLBitmap*) b;

	DISPLAY_INT iWidth = -1;
	DISPLAY_INT iHeight = -1;

#if (WNDML_VERBOSE)
	printf("Draw strecthed bmp (%X) at  [%d %d]\n",b, rect->right, rect->top);
#endif

	if ( uglBitmapSizeGet( wcBmp->m_DDBitmap, (UGL_SIZE* )&iWidth, (UGL_SIZE* )&iHeight) != UGL_STATUS_OK)
	{
	}


	if ( wcBmp->m_iWidth != (rect->right -rect->left) )
	{
	}

	if ( wcBmp->m_iHeight != (rect->bottom-rect->top) )
	{
	}

	if( uglBatchStart( GET_WINDML_GRAPHICS_CONTEXT(ctx)) != UGL_STATUS_OK)
	{
	}

	if (uglBitmapStretchBlt( GET_WINDML_GRAPHICS_CONTEXT(ctx), wcBmp->m_DDBitmap, 0, 0, iWidth, iHeight, UGL_DEFAULT_ID, rect->left, rect->top, rect->right,  rect->bottom) != UGL_STATUS_OK)
	{
	}


	if( uglBatchEnd( GET_WINDML_GRAPHICS_CONTEXT(ctx)) != UGL_STATUS_OK)
	{
	}
}

/*---------------------------------------------------------------------------*/
static void* WindML_CreateStretchedBitmap (
		HTMLGraphicsContext** ctx,
		void* b,
		DISPLAY_INT w,
		DISPLAY_INT h
	)
{
	WebcWindMLBitmap * pWebcWindMLBitmap = (WebcWindMLBitmap *)b;
	WebcWindMLBitmap * pStrectchedWebcWindMLBitmap;

	pStrectchedWebcWindMLBitmap = (WebcWindMLBitmap*) WEBC_MALLOC(sizeof(WebcWindMLBitmap));
#if (WNDML_VERBOSE)
	printf("Created strecthed bmp (%X)\n",pStrectchedWebcWindMLBitmap);
#endif
	if (!pStrectchedWebcWindMLBitmap)
		return(0);
	*pStrectchedWebcWindMLBitmap = *pWebcWindMLBitmap;

	pWebcWindMLBitmap->reference_count += 1;
	pStrectchedWebcWindMLBitmap->punstretched = b;
	pStrectchedWebcWindMLBitmap->m_iHeight = h;
	pStrectchedWebcWindMLBitmap->m_iWidth = w;
	return ((void *)pStrectchedWebcWindMLBitmap);
}

/*---------------------------------------------------------------------------*/
static void WindML_DestroyWebBitmap (
		HTMLGraphicsContext** ctx,
		void* b);

static void WindML_DestroyStretchedBitmap(
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	WindML_DestroyWebBitmap (ctx, bmp);
}


/*---------------------------------------------------------------------------*/
static DISPLAY_INT WindML_GetBitmapWidth (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	WebcWindMLBitmap * pWebcWindMLBitmap = (WebcWindMLBitmap *)bmp;
	return (DISPLAY_INT) pWebcWindMLBitmap->m_iWidth;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT WindML_GetBitmapHeight (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	WebcWindMLBitmap * pWebcWindMLBitmap = (WebcWindMLBitmap *)bmp;
	return (DISPLAY_INT) pWebcWindMLBitmap->m_iHeight;
}



static WebColor WindML_GetColorByIndex (
		HTMLGraphicsContext** ctx,
		int i
	);

/*---------------------------------------------------------------------------*/
static void WindML_DrawText (
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
	UGL_COLOR tempForegroundColor;
	UGL_COLOR tempBackgroundColor;
	UGL_FONT_ID fontId = (UGL_FONT_ID) font;

#if (WNDML_VERBOSE)
	printf("Draw Text (%s)\n",text);
#endif

	if( uglForegroundColorGet( GET_WINDML_GRAPHICS_CONTEXT(ctx), &tempForegroundColor) != UGL_STATUS_OK)
	{
	}

	if( tempForegroundColor != textColor)
	{
		if( uglForegroundColorSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), textColor) != UGL_STATUS_OK)
		{
		}
	}

	if (!fill)
	{
		backgroundColor = UGL_COLOR_TRANSPARENT; // ByIndex (ctx, 16);
	}

	if( uglBackgroundColorGet( GET_WINDML_GRAPHICS_CONTEXT(ctx), &tempBackgroundColor) != UGL_STATUS_OK)
	{
	}


	if( tempBackgroundColor != backgroundColor)
	{
		if( uglBackgroundColorSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), backgroundColor) != UGL_STATUS_OK)
		{
		}
	}

	if ( uglFontSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), fontId) != UGL_STATUS_OK )
	{
	}

	if( uglBatchStart( GET_WINDML_GRAPHICS_CONTEXT(ctx)) != UGL_STATUS_OK)
	{
	}

	if( uglTextDraw( GET_WINDML_GRAPHICS_CONTEXT(ctx), (UGL_POS)(x), (UGL_POS)(y), (UGL_SIZE)-1, (const UGL_CHAR *)(text)) != UGL_STATUS_OK)
	{
	}

	if( uglBatchEnd( GET_WINDML_GRAPHICS_CONTEXT(ctx)) != UGL_STATUS_OK)
	{
	}

	if( tempForegroundColor != textColor)
	{
		if( uglForegroundColorSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), tempForegroundColor) != UGL_STATUS_OK)
		{
		}
	}

	if( tempBackgroundColor != backgroundColor)
	{
		if( uglBackgroundColorSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), tempBackgroundColor) != UGL_STATUS_OK)
		{
		}
	}
}

/*---------------------------------------------------------------------------*/


static void WindML_DrawTextLen (
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
	UGL_COLOR tempForegroundColor;
	UGL_COLOR tempBackgroundColor;
	UGL_FONT_ID fontId = (UGL_FONT_ID) font;

#if (WNDML_VERBOSE)
	printf("Draw Tex len (%s)\n",text);
#endif

	if( uglForegroundColorGet( GET_WINDML_GRAPHICS_CONTEXT(ctx), &tempForegroundColor) != UGL_STATUS_OK)
	{
	}

	if( tempForegroundColor != textColor)
	{
		if( uglForegroundColorSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), textColor) != UGL_STATUS_OK)
		{
		}
	}

	if( uglBackgroundColorGet( GET_WINDML_GRAPHICS_CONTEXT(ctx), &tempBackgroundColor) != UGL_STATUS_OK)
	{
	}

	if (!fill)
	{
		fillColor = UGL_COLOR_TRANSPARENT;
	}


	if( tempBackgroundColor != fillColor)
	{
		if( uglBackgroundColorSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), fillColor) != UGL_STATUS_OK)
		{
		}
	}

	if ( uglFontSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), fontId) != UGL_STATUS_OK )
	{
	}

	if( uglBatchStart( GET_WINDML_GRAPHICS_CONTEXT(ctx)) != UGL_STATUS_OK)
	{
	}

	if( uglTextDraw( GET_WINDML_GRAPHICS_CONTEXT(ctx), (UGL_POS)(x), (UGL_POS)(y), (UGL_SIZE)textLen, (const UGL_CHAR *)(text)) != UGL_STATUS_OK)
	{
	}

	if( uglBatchEnd( GET_WINDML_GRAPHICS_CONTEXT(ctx)) != UGL_STATUS_OK)
	{
	}

	if( tempForegroundColor != textColor)
	{
		if( uglForegroundColorSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), tempForegroundColor) != UGL_STATUS_OK)
		{
		}
	}

	if( tempBackgroundColor != fillColor)
	{
		if( uglBackgroundColorSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), tempBackgroundColor) != UGL_STATUS_OK)
		{
		}
	}
}


/*---------------------------------------------------------------------------*/
static void WindML_SetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
#if (WNDML_CLIP_VERBOSE)
	printf("Set clip to [left %d,right %d,top %d,bottom %d]\n", rect->left,  rect->right, rect->top, rect->bottom);
#endif

	if( uglClipRectSet( GET_WINDML_GRAPHICS_CONTEXT(ctx), (UGL_POS)(rect->left), (UGL_POS)(rect->top), (UGL_POS)(rect->right), (UGL_POS)(rect->bottom)) != UGL_STATUS_OK)
	{
	}
}

/*---------------------------------------------------------------------------*/
static void WindML_GetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	if( uglClipRectGet( GET_WINDML_GRAPHICS_CONTEXT(ctx), (UGL_POS *)&(rect->left), (UGL_POS *)&(rect->top), (UGL_POS *)&(rect->right), (UGL_POS *)&(rect->bottom)) != UGL_STATUS_OK)
	{
	}
#if (WNDML_CLIP_VERBOSE)
	printf("Get clip got [left %d,right %d,top %d,bottom %d]\n", rect->left,  rect->right, rect->top, rect->bottom);
#endif
}

/*---------------------------------------------------------------------------*/
static void WindML_GetCanvasRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	UGL_MODE_INFO modeInfo;

	rect->left = 0;
	rect->top = 0;

	if ( uglInfo(GET_WINDML_DEVICE_ID(ctx), UGL_MODE_INFO_REQ, &modeInfo) != UGL_STATUS_OK )
	{
	}

	rect->right = modeInfo.width;
	rect->bottom = modeInfo.height;
}


/*---------------------------------------------------------------------------*/
static WebColor WindML_RGBToColor (
		HTMLGraphicsContext** ctx,
		HTMLColor color,
		HTMLColor* error
	)
{
	WebcWindMLGraphics* tempCtx = (WebcWindMLGraphics*) ctx;
	int iIndex = 0;
	int iPos = 0;
	int iLoopc = 0;
	unsigned int uMindiff = 0xffffffff;
	unsigned int uDiff = 0;
	int iDr = 0;
	int iDg = 0;
	int iBb = 0;

	unsigned int uRed = HTML_ColorGetRed(color);
	unsigned int uGreen = HTML_ColorGetGreen(color);
	unsigned int uBlue = HTML_ColorGetBlue(color);

	UGL_COLOR retColor;

	for ( iLoopc = 0x0; iLoopc < tempCtx->m_iScreenColors ; iLoopc++ )
	{
		iDr = UGL_ARGB_RED(pScreenClut[iLoopc]) - uRed;
		iDg = UGL_ARGB_GREEN(pScreenClut[iLoopc]) - uGreen;
		iBb = UGL_ARGB_BLUE(pScreenClut[iLoopc]) - uBlue;

		uDiff = iDr*iDr + iDg*iDg + iBb*iBb;

		if ( uDiff < uMindiff )
		{
			iIndex = iLoopc;
			if ( !uDiff )
			{
				break;
			}
			uMindiff = uDiff;
		}
	}

	if ( uglColorConvert(GET_WINDML_DEVICE_ID(ctx), &(pScreenClut[iIndex]), UGL_ARGB8888, &retColor, UGL_DEVICE_COLOR_32, 1) != UGL_STATUS_OK )
	{
	}

	if (error)
	{
		*error = HTML_RGBAToColor (
				EBSABS(UGL_ARGB_RED(pScreenClut[iIndex]) - HTML_ColorGetRed(color)),
				EBSABS(UGL_ARGB_GREEN(pScreenClut[iIndex]) - HTML_ColorGetGreen(color)),
				EBSABS(UGL_ARGB_BLUE(pScreenClut[iIndex]) - HTML_ColorGetBlue(color)),
				EBSABS(UGL_ARGB_ALPHA(pScreenClut[iIndex]) - HTML_ColorGetAlpha(color)));
	}

	return retColor;
}

/*---------------------------------------------------------------------------*/
static HTMLColor WindML_ColorToRGB (
		HTMLGraphicsContext** ctx,
		WebColor colorval
	)
{
	UGL_ARGB retColor;

	if ( uglColorConvert(GET_WINDML_DEVICE_ID(ctx), &colorval, UGL_DEVICE_COLOR_32, &retColor, UGL_ARGB8888, 1) != UGL_STATUS_OK )
	{
	}

	return HTML_RGBAToColor (
			UGL_ARGB_RED(retColor),
			UGL_ARGB_GREEN(retColor),
			UGL_ARGB_BLUE(retColor),
			UGL_ARGB_ALPHA(retColor));
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT WindML_GetTextHeight (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font
	)
{
	UGL_SIZE width = 0;   /* value to receive the text width */
	UGL_SIZE height = 0;  /* value to receive the text height */

	UGL_FONT_ID fontId = (UGL_FONT_ID) font;

	if( uglTextSizeGet( fontId, &width, &height, -1, (UGL_CHAR*)text) != UGL_STATUS_OK)
	{
	}

	return height;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT WindML_GetTextHeightLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font,
		long textLen
	)
{
	UGL_SIZE width = 0;   /* value to receive the text width */
	UGL_SIZE height = 0;  /* value to receive the text height */

	UGL_FONT_ID fontId = (UGL_FONT_ID) font;

	if( uglTextSizeGet( fontId, &width, &height, textLen, (UGL_CHAR*)text) != UGL_STATUS_OK)
	{
	}

	return height;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT WindML_GetTextWidth (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font
	)
{
	UGL_SIZE width = 0;   /* value to receive the text width */
	UGL_SIZE height = 0;  /* value to receive the text height */

	UGL_FONT_ID fontId = (UGL_FONT_ID) font;

	if( uglTextSizeGet( fontId, &width, &height, -1, (UGL_CHAR*)text) != UGL_STATUS_OK)
	{
	}

	return width;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT WindML_GetTextWidthLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font,
		long size
	)
{
	UGL_SIZE width = 0;   /* value to receive the text width */
	UGL_SIZE height = 0;  /* value to receive the text height */

	UGL_FONT_ID fontId = (UGL_FONT_ID) font;

	if( uglTextSizeGet( fontId, &width, &height, size, (UGL_CHAR*)text) != UGL_STATUS_OK)
	{
	}

	return width;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT WindML_GetFontHeight (
		HTMLGraphicsContext** ctx,
		void* font
	)
{
	UGL_SIZE width = 0;   /* value to receive the text width */
	UGL_SIZE height = 0;  /* value to receive the text height */

	UGL_FONT_ID fontId = (UGL_FONT_ID) font;

	if( uglTextSizeGet( fontId, &width, &height, 1, "A" ) != UGL_STATUS_OK)
	{
	}

	return height;
}

#if (WNDML_VERBOSE || WNDML_BUFFER_VERBOSE)
void show_web_rect(char *prompt,WebRect * Rect)
{
	printf("(%s) ==  %d,%d,%d,%d\n", prompt,
		Rect->left, /* left boundary of the source rect */
		Rect->top,/* top boundary of the source rect */
		Rect->right,/* right boundary of the source rect */
		Rect->bottom);
}
void show_html_rect(char *prompt,HTMLRect* Rect)
{
	printf("(%s) ==  %d,%d,%d,%d\n", prompt,
		Rect->left, /* left boundary of the source rect */
		Rect->top,/* top boundary of the source rect */
		Rect->right,/* right boundary of the source rect */
		Rect->bottom);
}
#endif
/*---------------------------------------------------------------------------*/
static void WindML_InvalidateRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	WebcWindMLGraphics* g = (WebcWindMLGraphics*) ctx;
#if (WNDML_VERBOSE || WNDML_BUFFER_VERBOSE)
show_web_rect("invalidRect before",&g->invalidRect);
show_html_rect("Oring in ",rect);
#endif
	g->invalidRect.Or((WebRect*) rect);
#if (WNDML_VERBOSE || WNDML_BUFFER_VERBOSE)
show_web_rect("invalidRect after",&g->invalidRect);
#endif
}

/*---------------------------------------------------------------------------*/
static void WindML_BeginDrawing (HTMLGraphicsContext** ctx)
{
	WebcWindMLGraphics* g = (WebcWindMLGraphics*) ctx;
#if (WNDML_VERBOSE)
	printf("Begin draw nest == %d \n",g->drawNesting);
#endif

	if (g->drawNesting == 0)
	{
		WebRect canvasRect;
		WindML_GetCanvasRect(ctx, (HTMLRect*) &canvasRect);
#if (WINDML_BMP_MODE_DOUBLE_BUFFER)
		WindML_CreateDoubleBuffer(ctx);
#endif
	}

	g->drawNesting++;
}

/*---------------------------------------------------------------------------*/
static void WindML_EndDrawing (HTMLGraphicsContext** ctx)
{
	WebcWindMLGraphics* g = (WebcWindMLGraphics*) ctx;
	WebRect canvasRect;

	WEBC_ASSERT(g->drawNesting > 0);

	g->drawNesting--;
#if (WNDML_VERBOSE)
	printf("End draw nest == %d \n",g->drawNesting);
#endif
	if (g->drawNesting == 0)
	{
#if (WINDML_PAGE_MODE_DOUBLE_BUFFER || WINDML_BMP_MODE_DOUBLE_BUFFER)
		WindML_DrawDoubleBuffer (ctx);
#endif
		WindML_GetCanvasRect(ctx, (HTMLRect*) &canvasRect);
		/* Set with left and right switwed top and bottom switched to indicate empty and allow And()
			method to work correctly */
		g->invalidRect.Set(canvasRect.right,canvasRect.bottom,canvasRect.left,canvasRect.top);
	}
}

#if (0)
/*---------------------------------------------------------------------------*/
static void WindML_TileBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect *Rect,
		DISPLAY_INT xOffset,
		DISPLAY_INT yOffset,
		void* bmp
	)
{
}

/*---------------------------------------------------------------------------*/
static void WindML_DrawPatternFrame (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor color,
		DISPLAY_INT thickness,
		CSSBorderStyle pattern
	)
{
}

#endif


/*---------------------------------------------------------------------------*/
static void* WindML_CreateWebBitmap (
		HTMLGraphicsContext** ctx,
		const WebGraphBitmapInfo* info
	)
{
	UGL_DIB localDIB;
	UGL_DDB_ID localDDBId = UGL_NULL;
    UGL_MDIB transMdib;

	WebcWindMLBitmap * pWebcWindMLBitmap = UGL_NULL;
	WebcWindMLGraphics* tempCtx = (WebcWindMLGraphics*) ctx;
	unsigned char *pdata = 0;
	unsigned char *ptransdata = 0;
	int datasize;
	int transdatasize;

	localDIB.width = info->iWidth;
	localDIB.height = info->iHeight;
	localDIB.stride = info->iBytesPerRow;

	if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_24)	// 24bit color, stored red-green-blue
	{
		localDIB.imageFormat = UGL_DIRECT;
// 		localDIB.colorFormat = UGL_RGB888;
		localDIB.colorFormat = UGL_ARGB8888;
	    localDIB.stride = info->iBytesPerRow/3;
		localDIB.clutSize = 0;
		localDIB.pClut = UGL_NULL;
		localDIB.pImage = (void *)info->pData;
{
	int i;
	unsigned char *pfr, *pto;

	localDIB.stride = info->iWidth;
	datasize = info->iWidth*info->iHeight*4;

	pto = pdata = (unsigned char *) WEBC_MALLOC(datasize);

	pfr = (unsigned char *)info->pData;

#define USE_BYTES
// Uncomment the following line if big endian processor
// #define TEST_BIGENDIAN
// Populate the 4 byte unsigned long sequence with
//		l = (unsigned long) (0xff<<24) |
//		    ((unsigned long)*pfr)<<16 |
//		    ((unsigned long)*(pfr+1))<<8 |
//		    ((unsigned long)*(pfr+1))

	for (i = 0; i < info->iWidth*info->iHeight; i++)
	{
#ifdef TEST_BIGENDIAN
	/* Big endian of TTRRGGBB ?? */
	*pto++ = 0xff;	    // trans
	*pto++ = *pfr;	   // red
	*pto++ = *(pfr+1); // green
	*pto++ = *(pfr+2); // blue
#else
	/* Little endian of TTRRGGBB */
	*pto++ = *(pfr+2); // blue
	*pto++ = *(pfr+1); // green
	*pto++ = *pfr;	   // red
	*pto++ = 0xff;	    // trans
	pfr += 3;
	}
#endif

	localDIB.pImage = (void *) pdata; // NOT FREED
}

	}
	else if (info->pixelFormat == WB_PIXEL_FORMAT_INDEX_8)	// palette indexed, 8 bits per pixel
	{
		localDIB.imageFormat = UGL_INDEXED_8;
		localDIB.colorFormat = UGL_RGB888;
		localDIB.clutSize = info->iPaletteSize;
		localDIB.pClut = (void *)info->pPalette;
		localDIB.pImage = (void *)info->pData;
		if (info->bHasTransparentColor)
		{
			int i;
			unsigned char *pfr, *pto, bmask;
			localDIB.stride = info->iWidth;
			transdatasize = ((info->iWidth*info->iHeight)+8)/8;

			pto = ptransdata = (unsigned char *) WEBC_MALLOC(transdatasize);
			pfr = (unsigned char *)info->pData;
			transMdib.width =  localDIB.width;
			transMdib.stride = localDIB.stride;
			transMdib.height = localDIB.height;
			transMdib.pImage = pto;
			bmask = 0x80;
			*pto = 0;
			for (i = 0; i < info->iWidth*info->iHeight; i++)
			{
				if (*pfr != info->iTransparentIndex)
				{
					*pto |= bmask;
				}
				pfr++;
				bmask >>= 1;
				if (!bmask)
				{
					bmask = 0x80;
					pto += 1;
					*pto = 0;
				}
			}
		}
	}
	else if (info->pixelFormat == WB_PIXEL_FORMAT_MONOCHROME_8)	// monochrome, 8 bits per pixel
	{
		localDIB.imageFormat = UGL_INDEXED_8;
		localDIB.colorFormat = UGL_RGB888;
		localDIB.clutSize = info->iPaletteSize;
		localDIB.pClut = (void *)info->pPalette;
		localDIB.pImage = (void *)info->pData;
	}
	else if (info->pixelFormat == WB_PIXEL_FORMAT_UNKNOWN)
	{
		return (0);
	}

	if (ptransdata)
	{
    	localDDBId = (UGL_DDB_ID) uglTransBitmapCreate(tempCtx->m_DevId, &localDIB, &transMdib, UGL_DIB_INIT_DATA, 0, UGL_NULL);

		WEBC_FREE(ptransdata);
	}
	else
	{
		localDDBId = uglBitmapCreate( tempCtx->m_DevId, &localDIB, UGL_DIB_INIT_DATA, 0, UGL_NULL);
		WEBC_FREE(pdata);
	}

	if ( localDDBId == UGL_NULL )
	{
		return (0);
	}

	pWebcWindMLBitmap = (WebcWindMLBitmap*) WEBC_MALLOC(sizeof(WebcWindMLBitmap));
	pWebcWindMLBitmap->m_DDBitmap = localDDBId;
	if (ptransdata)
		pWebcWindMLBitmap->mtrans_DDBitmap = (UGL_TDDB_ID) localDDBId;
	else
		pWebcWindMLBitmap->mtrans_DDBitmap = UGL_NULL;
	pWebcWindMLBitmap->m_iWidth = localDIB.width;
	pWebcWindMLBitmap->m_iHeight= localDIB.height;

	pWebcWindMLBitmap->reference_count = 1;
	pWebcWindMLBitmap->punstretched = (void *)pWebcWindMLBitmap;
	return pWebcWindMLBitmap;
}

/*---------------------------------------------------------------------------*/
static void WindML_DestroyWebBitmap (
		HTMLGraphicsContext** ctx,
		void* b
	)
{
	WebcWindMLBitmap * pWebcWindMLBitmap = (WebcWindMLBitmap *)b;
	WebcWindMLBitmap * pWebcUnstretchedBitmap = (WebcWindMLBitmap *)pWebcWindMLBitmap->punstretched;
	WebcWindMLGraphics* tempCtx = (WebcWindMLGraphics*) ctx;

	if (pWebcUnstretchedBitmap->reference_count)
		pWebcUnstretchedBitmap->reference_count -= 1;

	if (!pWebcUnstretchedBitmap->reference_count)
	{
		if (pWebcUnstretchedBitmap->mtrans_DDBitmap != UGL_NULL)
		{
			if (uglTransBitmapDestroy( tempCtx->m_DevId, pWebcUnstretchedBitmap->mtrans_DDBitmap) != UGL_STATUS_OK)
			{
			}
		}
		else
		{
			if (uglBitmapDestroy( tempCtx->m_DevId, pWebcUnstretchedBitmap->m_DDBitmap) != UGL_STATUS_OK)
			{
			}
		}
		WEBC_FREE(pWebcUnstretchedBitmap);
	}

	if (pWebcWindMLBitmap != pWebcUnstretchedBitmap)
	{
		WEBC_FREE(pWebcWindMLBitmap);
	}
}

/*---------------------------------------------------------------------------*/
static int WindML_Win32ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	)
{
	return (0);
}

/*---------------------------------------------------------------------------*/
static void WindML_ClaimPointer (
		HTMLGraphicsContext** ctx
	)
{
}

/*---------------------------------------------------------------------------*/
static void WindML_ReleasePointer (
		HTMLGraphicsContext** ctx
	)
{
}

/*---------------------------------------------------------------------------*/
static WebColor WindML_GetColorByIndex (
		HTMLGraphicsContext** ctx,
		int i
	)
{
	static UGL_COLOR colorMap [17] = {0};
	static UGL_BOOL done = false;

	if( done == false)
	{
		UGL_ARGB colorTable[] = {
			UGL_MAKE_ARGB(0xff, 0, 0, 0),											// WGC_BLACK
			UGL_MAKE_ARGB(0xff, 0, 0, 168),											// WGC_RED
			UGL_MAKE_ARGB(0xff, 0, 168, 0),		/* Initialize alpha to 255 for */	// WGC_GREEN
			UGL_MAKE_ARGB(0xff, 0, 168, 168),	/* now (opaque).               */	// WGC_BROWN
			UGL_MAKE_ARGB(0xff, 168, 0, 0),											// WGC_BLUE
			UGL_MAKE_ARGB(0xff, 168, 0, 168),										// WGC_MAGENTA
			UGL_MAKE_ARGB(0xff, 168, 84, 0),										// WGC_CYAN
			UGL_MAKE_ARGB(0xff, 168, 168, 168),										// WGC_LIGHTGRAY
			UGL_MAKE_ARGB(0xff, 84, 84, 84),										// WGC_DARKGRAY
			UGL_MAKE_ARGB(0xff, 84, 84, 255),										// WGC_LIGHTRED
			UGL_MAKE_ARGB(0xff, 84, 255, 84),										// WGC_LIGHTGREEN
			UGL_MAKE_ARGB(0xff, 84, 255, 255),										// WGC_YELLOW
			UGL_MAKE_ARGB(0xff, 255, 84, 84),										// WGC_LIGHTBLUE
			UGL_MAKE_ARGB(0xff, 255, 84, 255),										// WGC_LIGHTMAGENTA
			UGL_MAKE_ARGB(0xff, 255, 255, 84),										// WGC_LIGHTCYAN
			UGL_MAKE_ARGB(0xff, 255, 255, 255),										// WGC_WHITE
			UGL_MAKE_ARGB(0xff, 255, 255, 255)										// WGC_TRANSPARENT
		};

		if ( uglColorConvert(GET_WINDML_DEVICE_ID(ctx), colorTable, UGL_ARGB8888, colorMap, UGL_DEVICE_COLOR_32, 17) != UGL_STATUS_OK )
		{
		}

		done = true;
	}

	return colorMap[i];
}

/*---------------------------------------------------------------------------*/
int webc_InitWindMLBrowserConfig (HTMLBrowserConfig* config)
{
	webcWindMLGraphicsClass.getCanvasRect              = WindML_GetCanvasRect;

	webcWindMLGraphicsClass.claimPointer               = WindML_ClaimPointer;
	webcWindMLGraphicsClass.releasePointer             = WindML_ReleasePointer;

	webcWindMLGraphicsClass.invalidateRect             = WindML_InvalidateRect;
	webcWindMLGraphicsClass.beginDrawing               = WindML_BeginDrawing;
	webcWindMLGraphicsClass.getClipRect                = WindML_GetClipRect;
	webcWindMLGraphicsClass.setClipRect                = WindML_SetClipRect;
	webcWindMLGraphicsClass.endDrawing                 = WindML_EndDrawing;

	webcWindMLGraphicsClass.colorToRGBA                = WindML_ColorToRGB;
	webcWindMLGraphicsClass.RGBAToColor                = WindML_RGBToColor;
	webcWindMLGraphicsClass.getColorByIndex            = WindML_GetColorByIndex;

	webcWindMLGraphicsClass.drawText                   = WindML_DrawText;
	webcWindMLGraphicsClass.drawTextLen                = WindML_DrawTextLen;
	webcWindMLGraphicsClass.drawBitmap                 = WindML_DrawBitmap;
	webcWindMLGraphicsClass.drawBitmapStretchedToRect  = WindML_DrawStretchedBitmap;
	webcWindMLGraphicsClass.drawRectangle              = WindML_DrawRectangle;

	webcWindMLGraphicsClass.createFont                 = WindML_CreateFont;
	webcWindMLGraphicsClass.getDefaultFont             = WindML_GetDefaultFont;
	webcWindMLGraphicsClass.destroyFont                = WindML_DestroyFont;
	webcWindMLGraphicsClass.getNumFontFamilies         = WindML_NumFontFamilies;
	webcWindMLGraphicsClass.getFontFamilyNames         = WindML_GetFontFamilyNames;
	webcWindMLGraphicsClass.getFontFamilyGenericNames  = WindML_GetFontFamilyGenericNames;
	webcWindMLGraphicsClass.getTextHeight              = WindML_GetTextHeight;
	webcWindMLGraphicsClass.getTextWidth               = WindML_GetTextWidth;
	webcWindMLGraphicsClass.getTextHeightLen           = WindML_GetTextHeightLen;
	webcWindMLGraphicsClass.getTextWidthLen            = WindML_GetTextWidthLen;
	webcWindMLGraphicsClass.getFontHeight              = WindML_GetFontHeight;

	webcWindMLGraphicsClass.createWebBitmap            = WindML_CreateWebBitmap;
	webcWindMLGraphicsClass.destroyWebBitmap           = WindML_DestroyWebBitmap;
	webcWindMLGraphicsClass.createStretchedBitmap      = WindML_CreateStretchedBitmap;
	webcWindMLGraphicsClass.destroyStretchedBitmap     = WindML_DestroyStretchedBitmap;
	webcWindMLGraphicsClass.getBitmapWidth             = WindML_GetBitmapWidth;
	webcWindMLGraphicsClass.getBitmapHeight            = WindML_GetBitmapHeight;

	webcWindMLGraphicsClass.optionalDrawHorizontalLine = 0;
	webcWindMLGraphicsClass.optionalDrawVerticalLine   = 0;
	webcWindMLGraphicsClass.optionalDrawStyledFrame    = 0;
	webcWindMLGraphicsClass.optionalDrawBitmapTiled    = WindML_TileBitmap;
	size_t uiSize = sizeof(WebcWindMLGraphics);
	WebcWindMLGraphics* ctx = (WebcWindMLGraphics*) (WEBC_MALLOC(uiSize));
	if (ctx)
	{

		rtp_memset(ctx, 0, uiSize);
		ctx->ctx = &webcWindMLGraphicsClass;

//The custom stuff STARTS
// Initialize WindML
		if ( uglInitialize() != UGL_STATUS_OK )
		{
#ifdef RTP_DEBUG
	        RTP_DEBUG_OUTPUT_STR("ERROR :- WindML did not initialize\n");
#endif
		 return (-1);
		}

//Get device identifier for the font engine.
		ctx->m_FontDrvId =  (UGL_FONT_DRIVER_ID)uglRegistryFind (UGL_FONT_ENGINE_TYPE,  UGL_NULL, UGL_NULL, UGL_NULL)->id;
		if ( ctx->m_FontDrvId == UGL_NULL )
		{
#ifdef RTP_DEBUG
			RTP_DEBUG_OUTPUT_STR("ERROR :- Can not get device identifier for the font engine\n");
#endif
			return (-1);
		}

/*
* UGL will render fonts using different origins.  Somewhat like the
* "hot spot" in the cursor.  Here we are setting the origin of the font
* rendering to be at the upper left corner of the text.
* uglFontDriverInfo is a general purpose function that can return
* information.  It also can be used to affect configuration data, as
* we are doing here.
*/

		UGL_ORD textOrigin = UGL_FONT_TEXT_UPPER_LEFT;	//IS this correct ?
		uglFontDriverInfo( ctx->m_FontDrvId, UGL_FONT_TEXT_ORIGIN, &textOrigin);
//Get device identifier of display device
		ctx->m_DevId = (UGL_DEVICE_ID)uglRegistryFind (UGL_DISPLAY_TYPE,  UGL_NULL, UGL_NULL, UGL_NULL)->id;

		if ( ctx->m_DevId == UGL_NULL )
		{
#ifdef RTP_DEBUG
			RTP_DEBUG_OUTPUT_STR("ERROR :- Can not get device identifier of display device\n");
#endif
			return (-1);
		}

//Get input service identifier
		ctx->m_InputServiceId = (UGL_INPUT_SERVICE_ID)uglRegistryFind (UGL_INPUT_SERVICE_TYPE,  UGL_NULL, UGL_NULL, UGL_NULL)->id;
		if ( ctx->m_InputServiceId == UGL_NULL )
		{
#ifdef RTP_DEBUG
			RTP_DEBUG_OUTPUT_STR("ERROR :- Can not get input service identifier of display device\n");
#endif
			return (-1);
		}

//Get color information
		UGL_COLOR_INFO colorInfo;
		if ( uglInfo(ctx->m_DevId, UGL_COLOR_INFO_REQ, &colorInfo) != UGL_STATUS_OK )
		{
#ifdef RTP_DEBUG
			RTP_DEBUG_OUTPUT_STR("ERROR :- Can not get color information of display device\n");
#endif
			return (-1);
		}
//If color space is gray the error
		if ( colorInfo.cspace == UGL_CSPACE_GRAY)
		{
#ifdef RTP_DEBUG
			RTP_DEBUG_OUTPUT_STR("ERROR :- Color space is gray\n");
#endif
			return (-1);
		}
//Get mode information
		UGL_MODE_INFO modeInfo;

		if ( uglInfo(ctx->m_DevId, UGL_MODE_INFO_REQ, &modeInfo) != UGL_STATUS_OK )
		{
#ifdef RTP_DEBUG
			RTP_DEBUG_OUTPUT_STR("ERROR :- Can not get mode information of display device\n");
#endif
			return (-1);
		}
		if ( modeInfo.colorModel == UGL_INDEXED_8 )
		{
			if ( modeInfo.clutSize >= 256 )
			{
				ctx->m_iScreenColors = 256;
			}
		}
		else if ( modeInfo.colorModel == UGL_DIRECT )
		{
			if ( modeInfo.colorDepth >= 8 )
			{
				ctx->m_iScreenColors = 256;
			}
			else
			{
				ctx->m_iScreenColors = 0;
			}
		}
		else
		{
			ctx->m_iScreenColors = 0;
		}

		if ( ctx->m_iScreenColors < 256)
		{
#ifdef RTP_DEBUG
			RTP_DEBUG_OUTPUT_STR("ERROR :- The device is supporting colors less than 256\n");
#endif
			return (-1);
		}
//Allocate the color cube
		int iIndex = 0;
		int iLoopc = 0;
		int iAlpha = 0xff;
		int iRed = 0x0;
		int iGreen = 0x0;
		int iBlue = 0x0;

		pScreenClut = new UGL_ARGB[ctx->m_iScreenColors];

		if ( ctx->m_iScreenColors == 256 )
		{
			iIndex = 0;
			// Initialize 6x6x6 = 216 colors first ( Color Cube)
			for ( iRed = 0x0; iRed <= 0xff; iRed+=0x33 )
				{
				for ( iGreen = 0x0; iGreen <= 0xff; iGreen+=0x33 )
				{
					for ( iBlue = 0x0; iBlue <= 0xff; iBlue+=0x33 )
					{
						pScreenClut[iIndex] = UGL_MAKE_ARGB( iAlpha, iRed, iGreen, iBlue);
						iIndex++;
					}
				}
			}

			// Initialize rest 40 colors to UGL_MAKE_ARGB( iAlpha, ZERO, ZERO, ZERO)
			for ( iLoopc = 0; iLoopc < 40; iLoopc++ )
			{
				pScreenClut[iIndex] = UGL_MAKE_ARGB( iAlpha, 0x0, 0x0, 0x0);
				iIndex++;
			}
		}
		ctx->m_pDeviceAllocColors = new UGL_COLOR[ ctx->m_iScreenColors ];
		UGL_ORD * pIndex = UGL_NULL;
		pIndex = new UGL_ORD[ ctx->m_iScreenColors ];

		for( iIndex = 0; iIndex < ctx->m_iScreenColors; iIndex++)
		{
			ctx->m_pDeviceAllocColors[ iIndex ] = 0x0;
			pIndex[ iIndex ] = iIndex;
		}
		if ( uglColorAlloc( ctx->m_DevId, pScreenClut, pIndex, ctx->m_pDeviceAllocColors, ctx->m_iScreenColors ) != UGL_STATUS_OK )
		{
#ifdef RTP_DEBUG
			RTP_DEBUG_OUTPUT_STR("ERROR :- Can not allocate colors\n");
#endif
			return (-1);
		}

		if ( pIndex )
		{
			delete pIndex;
			pIndex = UGL_NULL;
		}

//Create graphics context
		ctx->m_gc = uglGcCreate( ctx->m_DevId);
		if ( ctx->m_gc == UGL_NULL )
		{
#ifdef RTP_DEBUG
			RTP_DEBUG_OUTPUT_STR("ERROR :- Can not create graphics context\n");
#endif
			return (-1);
		}
//The default bit map is UGL_DEFAULT_ID we set it to UGL_DISPLAY_ID
		if ( uglDefaultBitmapSet( ctx->m_gc, UGL_DISPLAY_ID) != UGL_STATUS_OK )
		{
#ifdef RTP_DEBUG
			RTP_DEBUG_OUTPUT_STR("ERROR :- Can not set the default bitmap\n");
#endif
			return (-1);
		}

//The custom stuff ENDS
		rtp_memset(config, 0, sizeof(HTMLBrowserConfig));
		config->graphics = (HTMLGraphicsContext**) ctx;
		config->processInputQueue = &WindMLProcessInputQueueFn;
#if (WINDML_PAGE_MODE_DOUBLE_BUFFER)
		WindML_CreateDoubleBuffer(config->graphics);
#endif
		return (0);
	}

	return (-1);

}

/*---------------------------------------------------------------------------*/
void webc_DestroyWindMLBrowserConfig (HTMLBrowserConfig* config)
{
	WebcWindMLGraphics* ctx = (WebcWindMLGraphics*) config->graphics;


	if ( uglColorFree( ctx->m_DevId, ctx->m_pDeviceAllocColors, ctx->m_iScreenColors) != UGL_STATUS_OK )
	{
#ifdef RTP_DEBUG
			RTP_DEBUG_OUTPUT_STR("ERROR :- :- Can not free colors\n");
#endif
	}
#if (WINDML_PAGE_MODE_DOUBLE_BUFFER || WINDML_BMP_MODE_DOUBLE_BUFFER)
	/* Must be implemented when dynamic windows are supported */
	WindML_DestroyDoubleBuffer(config->graphics);
#endif

	uglDeinitialize();

	delete ctx->m_pDeviceAllocColors;

	if ( pScreenClut )
	{
		delete pScreenClut;
		pScreenClut = UGL_NULL;
	}

	WEBC_FREE(config->graphics);
	config->graphics = 0; //MANK
}

/*---------------------------------------------------------------------------*/
void   webc_DestroyWindMLBrowser(HBROWSER_HANDLE hbrowser)
{
	HTMLBrowserConfig config;

	if (webc_BrowserGetConfig(hbrowser, &config) >= 0)
	{
		webc_DestroyBrowser(hbrowser, 0);
		webc_DestroyWindMLBrowserConfig(&config);
	}
}

/*---------------------------------------------------------------------------*/
WebKey webc_TranslateWindMLKey         (WEBC_UINT32 rawKey)
{
WebKey result;
	if (!WindMLKeyMapInitialized)
	{
		webc_CreateKeyMap(&WindMLKeyMap, WindMLKeyData, WG_NUM_WindML_KEYS);
		WindMLKeyMapInitialized = WEBC_TRUE;
	}


	result = webc_TranslateKey(&WindMLKeyMap, rawKey);
	return (result);
}

/*---------------------------------------------------------------------------*/
int WindML_ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	)
{
	rtp_not_yet_implemented ();
	//Do need to implement this function
	return (0);
	return (-1);
}

/*---------------------------------------------------------------------------*/
void* WindML_CreateFont (
		HTMLGraphicsContext** ctx,
		const WEBC_CHAR* familyName,
		const WEBC_CHAR* genericName,
		WEBC_UINT16 pointSize,
		WEBC_UINT16 attrib,
		int familyIndex
	)
{
	static char * fontStyleMap[4] =
	{
		"",
		" ; bold",
		" ; italic",
		" ; bold; italic"
	};

	char fontPriorityString[512]={0};

	UGL_FONT_ID retFontId = UGL_NULL;
	UGL_FONT_DEF fontDef;

	WebcWindMLGraphics* tempCtx = (WebcWindMLGraphics*) ctx;

	if (familyIndex < 0)
	{
		familyIndex = webc_font_family_index (familyName, genericName, gpFontFamilyName, gpFontFamilyGenericName, WEBC_CFG_MAX_FONT_FAMILIES);
		if (familyIndex < 0)
		{
			familyIndex = 0;
		}
	}

	sprintf( fontPriorityString, "familyName=%s; pixelSize=%d%s", familyName, pointSize, fontStyleMap[attrib & 0x3]);

	if ( uglFontFindString( tempCtx->m_FontDrvId, fontPriorityString, &fontDef) != UGL_STATUS_OK )
	{
	}

	if ((retFontId = uglFontCreate( tempCtx->m_FontDrvId, &fontDef)) == UGL_NULL)
	{
	}

	if(retFontId == UGL_NULL)
	{
		retFontId = gWindMLDefaultFont;
	}

/*
may not be required

	if ( uglFontSet( tempCtx->m_gc, retFontId) != UGL_STATUS_OK )
	{
	}
*/
	return retFontId;
}

/*---------------------------------------------------------------------------*/
void* WindML_GetDefaultFont (
		HTMLGraphicsContext** ctx
	)
{
	WebcWindMLGraphics* tempCtx = (WebcWindMLGraphics*) ctx;

	if (gWindMLDefaultFont == UGL_NULL)
	{
/*
* A variable to hold the data sent to and returned from
* uglFontDriverInfo.  In this example we are going to pass
* data to the function related to the positioning of the
* rendered text.
*/
		UGL_FONT_DEF fontDef;

/*
* This function allows us to find a font from the set of available
* fonts.  A search parameter string is used to specify the font we
* want.  Incomplete specifications leave the function to make the
* choice.  Here we specify Lucida, but haven't specified a size, so
* the font function will choose one for us.  There are several para-
* meters that can be used.  They are semicolon delimited.
*/
		if ( uglFontFindString( tempCtx->m_FontDrvId, "familyName=Lucida; pixelSize=10", &fontDef) != UGL_STATUS_OK )
		{
			return (0);
		}

/*
* Now that we have found a font and it is specified in fontDef,
* we can "create" the font.  What is accomplished here is dependent
* up upon the specific font engine being used.  This step could
* include conversion of the font data from a platform independent
* format to a platform dependent format; similar to a DIB being used
* to create a DDB.  For the BMF font engine a glyph cache and other
* resources are configured.
*/
		if ((gWindMLDefaultFont = uglFontCreate( tempCtx->m_FontDrvId, &fontDef)) == UGL_NULL)
		{
			return(0);
		}
	}

/*
may not be required

	if ( uglFontSet( tempCtx->m_gc, gWindMLDefaultFont) != UGL_STATUS_OK )
	{
	}
*/
	return gWindMLDefaultFont;
}

/*---------------------------------------------------------------------------*/
void WindML_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* font
	)
{
	if( font != gWindMLDefaultFont)
	{
		if (uglFontDestroy ((UGL_FONT_ID)font) != UGL_STATUS_OK)
		{
		}
	}
}

/*---------------------------------------------------------------------------*/
int WindML_NumFontFamilies (
		HTMLGraphicsContext** ctx
	)
{
	return WEBC_CFG_MAX_FONT_FAMILIES;
}

/*---------------------------------------------------------------------------*/
const WEBC_CHAR** WindML_GetFontFamilyNames (
		HTMLGraphicsContext** ctx
	)
 {
	return gpFontFamilyName;
}

/*---------------------------------------------------------------------------*/
const WEBC_CHAR** WindML_GetFontFamilyGenericNames (
		HTMLGraphicsContext** ctx
	)
{
	return gpFontFamilyGenericName;
}

UGL_MSG webc_GetWindMLMessages(HTMLBrowserConfig* config)
{
	UGL_MSG uglMsg;
	rtp_memset(&uglMsg, 0, sizeof(uglMsg));
	uglInputMsgGet(GET_WINDML_SERVICE_ID(config->graphics), &uglMsg, 10000);
	return uglMsg;
}
