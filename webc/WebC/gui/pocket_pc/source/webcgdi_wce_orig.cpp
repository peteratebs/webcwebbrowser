/*
|  WEBCGDI_WCE.C - WebC Graphics Driver for GDI interface on Windows CE platform
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

#include "windows.h"
#include "webcgdi_wce.h"
#include "wfgdi_wce.h"
#include "webcmem.h"
#include "wgdifont.h"
#include "webrect.hpp"
#include "webcmem.h"
#include "webcassert.h"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

#if !defined(WEBC_BUFFER_SCROLLING)
#define GDI_DOUBLE_BUFFER
#endif

#define RECT_TO_HTML(H,R)     ((H).left  = (R).X,  (H).top   = (R).Y, \
                               (H).right = (R).X + (R).Width - 1, (H).bottom = (R).Y + (R).Height - 1)
#define HTML_TO_RECT(R,H)     RECT R; \
                              R.left   = (H)->left; \
                              R.top    = (H)->top; \
                              R.right  = (H)->right; \
                              R.bottom = (H)->bottom;

#define XY_TO_POINT(P,X,Y)    PointF P((REAL) X, (REAL) Y)

#define GDI_BITMAP_ROW_ALIGN  4

/*****************************************************************************/
/* Types
 *****************************************************************************/

typedef struct
{
	HBITMAP  hBitmap;
	HBITMAP  hMask;
	int      bitmapWidth;
	int      bitmapHeight;
	int      hasTrans;
	COLORREF transColor;
}
WebcGDIBitmap;

/*****************************************************************************/
/* Function Prototypes
/*****************************************************************************/

static int GDI_WCE_ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	);

/*****************************************************************************/
/* Data
 *****************************************************************************/

extern int giAllowTimerTick;

HTMLGraphicsContext webcGDIWCEGraphicsClass;

int (*gdiWceProcessInputQueueFn) (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode) = GDI_WCE_ProcessInputQueue;

#define WG_NUM_GDI_WCE_KEYS 32

static WEBC_UINT32 gdiWceKeyData [WG_NUM_GDI_WCE_KEYS*2] =
{
	WGK_TAB         ,  0x09 ,
	WGK_CR          ,  0x0d ,
	WGK_PGUP        ,  604  ,
	WGK_PGDN        ,  605  ,
	WGK_LNUP        ,  606  ,
	WGK_LNDN        ,  607  ,
	WGK_ALT         ,  610  ,
	WGK_CTRL        ,  611  ,
	WGK_SHIFT       ,  612  ,
	WGK_CAPS        ,  613  ,
	WGK_NUMLOCK     ,  614  ,
	WGK_SCROLLLOCK  ,  615  ,
	WGK_F1          ,  650  ,
	WGK_F2          ,  651  ,
	WGK_F3          ,  652  ,
	WGK_F4          ,  653  ,
	WGK_F5          ,  654  ,
	WGK_F6          ,  655  ,
	WGK_F7          ,  656  ,
	WGK_F8          ,  657  ,
	WGK_F9          ,  658  ,
	WGK_F10         ,  659  ,
	WGK_F11         ,  660  ,
	WGK_F12         ,  661  ,
	WGK_INSERT      ,  601  ,
	WGK_DELETE      ,  600  ,
	WGK_BACKSPACE   ,  0x08 ,
	WGK_LEFT        ,  608  ,
	WGK_RIGHT       ,  609  ,
	WGK_HOME        ,  602  ,
	WGK_END         ,  603  ,
	WGK_ESC         ,  0x1b	 
};

static WebcKeyMap gdiWceKeyMap;
static WEBC_BOOL  gdiWceKeyMapInitialized = WEBC_FALSE;

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
static void GDI_WCE_DrawRectangle (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor outlineColor,
		WebColor fillColor,
		WEBC_BOOL fill
	)
{
	HBRUSH hfillbrush = CreateSolidBrush((COLORREF) fillColor);

	if (hfillbrush != NULL)
	{
		RECT box;
		box.left   = rect->left;
		box.top    = rect->top;
		box.right  = rect->right;
		box.bottom = rect->bottom;
	
		if (outlineColor == fillColor)
		{
			if (fill)
			{
				box.right++;
				box.bottom++;
				FillRect(GET_OUTPUT_DC(ctx), &box, hfillbrush);
			}
			else
			{
				/* tbd -	BOOL Rectangle(
				  HDC hdc, 
				  int nLeftRect, 
				  int nTopRect, 
				  int nRightRect, 
				  int nBottomRect
				); 
				*/
				//FrameRect(GET_OUTPUT_DC(ctx), &box, hfillbrush);
			}
		}
		else
		{
			HBRUSH houtlinebrush = CreateSolidBrush((COLORREF) outlineColor);
			if (houtlinebrush != NULL)
			{
				// tbd - see above
				//FrameRect(GET_OUTPUT_DC(ctx), &box, houtlinebrush);
				DeleteObject((HGDIOBJ) houtlinebrush);
			}
			
			if (fill)
			{
				box.left++;
				box.top++;
				FillRect(GET_OUTPUT_DC(ctx), &box, hfillbrush);
			}
		}
	
		DeleteObject((HGDIOBJ) hfillbrush);
	}
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_DrawBitmap (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		void* b
	)
{
	if (GET_BITMAP_DC(ctx) != NULL)
	{
		WebcGDIBitmap* bmp = (WebcGDIBitmap*) b;
		HGDIOBJ hSaveBitmap = SelectObject(GET_BITMAP_DC(ctx), (HGDIOBJ) bmp->hBitmap);

#if 1
		if (bmp->hasTrans)
		{
			TransparentImage (
					GET_OUTPUT_DC(ctx), 
					x, 
					y, 
					bmp->bitmapWidth, 
					bmp->bitmapHeight,
					GET_BITMAP_DC(ctx), 
					0, 
					0, 
					bmp->bitmapWidth, 
					bmp->bitmapHeight, 
					bmp->transColor);			
		}
#else
		if (bmp->hMask != NULL)
		{
			/* can't use this because it has a memory leak.  Too bad MS doesn't test things
			   before shipping them
			TransparentBlt(mhBufferDC, x, y, bmp->bitmapWidth, bmp->bitmapHeight, 
						   bmp->hDeviceContext, 0, 0, bmp->bitmapWidth, bmp->bitmapHeight,
						   (UINT) bmp->transparent);
			*/
			
			// tbd - create the mask DC once and keep in the graphics context
			if (GET_MASK_DC(ctx) != NULL)
			{
				HGDIOBJ hSaveMask = SelectObject(GET_MASK_DC(ctx), (HGDIOBJ) bmp->hMask);

			#if 1
				// apply the masks
				BitBlt(GET_OUTPUT_DC(ctx), x, y, bmp->bitmapWidth, bmp->bitmapHeight, GET_MASK_DC(ctx), 0, 0, SRCAND);

				// paint the bitmap
				BitBlt(GET_OUTPUT_DC(ctx), x, y, bmp->bitmapWidth, bmp->bitmapHeight, GET_BITMAP_DC(ctx), 0, 0, SRCPAINT);		
			#else
				BitBlt(GET_OUTPUT_DC(ctx), x, y, bmp->bitmapWidth, bmp->bitmapHeight, GET_MASK_DC(ctx), 0, 0, SRCCOPY);
			#endif

				//SelectObject(GET_MASK_DC(ctx), hSaveMask);
			}
		}
#endif			
		else	
		{
			BitBlt(GET_OUTPUT_DC(ctx), x, y, bmp->bitmapWidth, bmp->bitmapHeight, GET_BITMAP_DC(ctx), 0, 0, SRCCOPY);
		}

		SelectObject(GET_BITMAP_DC(ctx), hSaveBitmap);
	}
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_DrawStretchedBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		void* b
	)
{
	// tbd - use bitmapDC
	HDC hBitmapDC = CreateCompatibleDC(GET_OUTPUT_DC(ctx));
	if (hBitmapDC != NULL)
	{
		WebcGDIBitmap* bmp = (WebcGDIBitmap*) b;
		HGDIOBJ hSave = SelectObject(hBitmapDC, bmp->hBitmap);
		// tbd - handle mask case
		StretchBlt (GET_OUTPUT_DC(ctx), 
		            rect->left, 
					rect->top, 
					rect->right - rect->left + 1, 
					rect->bottom - rect->top + 1, 
					hBitmapDC, 
					0, 
					0, 
					bmp->bitmapWidth, 
					bmp->bitmapHeight, 
					SRCCOPY);

		SelectObject(hBitmapDC, hSave);
		DeleteDC(hBitmapDC);
	}	
}

/*---------------------------------------------------------------------------*/
static void* GDI_WCE_CreateStretchedBitmap (
		HTMLGraphicsContext** ctx,
		void* b,
		DISPLAY_INT w,
		DISPLAY_INT h
	)
{
	WebcGDIBitmap* stretched = 0;
	HDC hDefaultDC = GetDC(NULL);

	if (hDefaultDC)
	{
		HDC hBitmapDC = CreateCompatibleDC(hDefaultDC);
		if (hBitmapDC)
		{
			HDC hStretchedDC = CreateCompatibleDC(hDefaultDC);
			if (hStretchedDC)
			{
				stretched = (WebcGDIBitmap*) WEBC_MALLOC(sizeof(WebcGDIBitmap));
				if (stretched)
				{
					WebcGDIBitmap* src = (WebcGDIBitmap*) b;
					stretched->hMask = NULL;
					stretched->bitmapWidth = w;
					stretched->bitmapHeight = h;
					stretched->hBitmap = CreateCompatibleBitmap(hDefaultDC, w, h);
					if (stretched->hBitmap)
					{
						HGDIOBJ hSaveBitmap = SelectObject(hBitmapDC, src->hBitmap);
						HGDIOBJ hSaveStretch = SelectObject(hStretchedDC, stretched->hBitmap);
						StretchBlt(hStretchedDC, 0, 0, w, h, 
						           hBitmapDC, 0, 0, src->bitmapWidth, src->bitmapHeight, SRCCOPY);

					  #if 1
					  	stretched->hasTrans = src->hasTrans;
					  	stretched->transColor = src->transColor;
					  #else						           
						if (src->hMask)
						{
							stretched->hMask = CreateCompatibleBitmap(hDefaultDC, w, h);
							if (stretched->hMask)
							{
								HGDIOBJ hSave2 = SelectObject(hBitmapDC, src->hMask);
								HGDIOBJ hSave3 = SelectObject(hStretchedDC, stretched->hMask);
								
								StretchBlt(hStretchedDC, 0, 0, w, h, 
								           hBitmapDC, 0, 0, src->bitmapWidth, src->bitmapHeight, SRCCOPY);	
								
								SelectObject(hBitmapDC, hSave2);
								SelectObject(hStretchedDC, hSave3);				
							}
						}
					  #endif
						
						SelectObject(hStretchedDC, hSaveStretch);
						SelectObject(hBitmapDC, hSaveBitmap);
					}
				}
				
				DeleteDC(hStretchedDC);
			}
	
			DeleteDC(hBitmapDC);
		}

		ReleaseDC(NULL, hDefaultDC);
	}

	return (stretched);
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_DestroyStretchedBitmap(
		HTMLGraphicsContext** ctx,
		void* b
	)
{
	WebcGDIBitmap* bmp = (WebcGDIBitmap*) b;
	DeleteObject((HGDIOBJ) bmp->hBitmap);
	if (bmp->hMask != NULL)
	{
		DeleteObject((HGDIOBJ) bmp->hMask);
	}
	WEBC_FREE(bmp);
}


/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDI_WCE_GetBitmapWidth (
		HTMLGraphicsContext** ctx,
		void* b
	)
{
	WebcGDIBitmap* bmp = (WebcGDIBitmap*) b;
	return bmp->bitmapWidth;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDI_WCE_GetBitmapHeight (
		HTMLGraphicsContext** ctx,
		void* b
	)
{
	WebcGDIBitmap* bmp = (WebcGDIBitmap*) b;
	return bmp->bitmapHeight;
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_DrawTextLen (
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
	WebcGDIFont* font = (WebcGDIFont*) f;
	HGDIOBJ hSaveFont = SelectObject(GET_OUTPUT_DC(ctx), (HGDIOBJ) font->hFont);
	SetTextColor(GET_OUTPUT_DC(ctx), textColor);
	if (fill)
	{
		SetBkColor(GET_OUTPUT_DC(ctx), fillColor);
		SetBkMode(GET_OUTPUT_DC(ctx), OPAQUE);
	}
	else
	{
		SetBkMode(GET_OUTPUT_DC(ctx), TRANSPARENT);
	}

	ExtTextOut(GET_OUTPUT_DC(ctx), x, y, (fill)? ETO_OPAQUE : 0, 0, text, textLen, 0);

	SelectObject(GET_OUTPUT_DC(ctx), hSaveFont);
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_DrawText (
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
	GDI_WCE_DrawTextLen(ctx, x, y, text, textColor, backgroundColor, fill, font, wcslen(text));
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_SetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	//GET_CLIP_RECT(ctx).Set((WebRect*)rect);
	if (!GET_CLIP_RECT(ctx).Equals((WebRect*)rect))
	{
		GET_CLIP_RECT(ctx).Set((WebRect*)rect);
		HRGN hClip = CreateRectRgn(rect->left, rect->top, rect->right+1, rect->bottom+1);
		if (hClip != NULL)
		{
			SelectClipRgn(GET_OUTPUT_DC(ctx), hClip);
			DeleteObject((HGDIOBJ) hClip);
		}
	}
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_GetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	*rect = *((HTMLRect*) &GET_CLIP_RECT(ctx));	
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_GetCanvasRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	RECT box;
	
	GetClientRect(GET_WINDOW(ctx), &box);

	rect->left   = box.left;
	rect->top    = box.top;
	rect->right  = box.right-1;
	rect->bottom = box.bottom-1;
	
}

/*---------------------------------------------------------------------------*/
static WebColor GDI_WCE_RGBToColor (
		HTMLGraphicsContext** ctx,
		HTMLColor color,
		HTMLColor* error
	)
{
	COLORREF c = RGB (
			HTML_ColorGetRed(color), 
			HTML_ColorGetGreen(color), 
			HTML_ColorGetBlue(color)
		);
	
	if (error)
	{
		*error = HTML_RGBAToColor (
				EBSABS(GetRValue(c) - HTML_ColorGetRed(color)),
				EBSABS(GetGValue(c) - HTML_ColorGetGreen(color)),
				EBSABS(GetBValue(c) - HTML_ColorGetBlue(color)),
				0
			);
	}
	
	return (c);	
}

/*---------------------------------------------------------------------------*/
static HTMLColor GDI_WCE_ColorToRGB (
		HTMLGraphicsContext** ctx,
		WebColor colorval
	)
{
	return HTML_RGBAToColor (
			GetRValue(colorval),
			GetGValue(colorval),
			GetBValue(colorval),
			0
		);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDI_WCE_GetTextHeight (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font
	)
{
	return (((WebcGDIFont*)font)->fontHeight);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDI_WCE_GetTextHeightLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font,
		long textLen
	)
{
	return (((WebcGDIFont*)font)->fontHeight);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDI_WCE_GetTextWidthLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* f,
		long textLen
	)
{
	SIZE size;
	DISPLAY_INT width = 0;
	WebcGDIFont* font = (WebcGDIFont*) f;

	HGDIOBJ hOldFont = SelectObject(GET_FONT_DC(ctx), (HGDIOBJ) font->hFont);
	if (hOldFont != NULL)
	{
		if (GetTextExtentPoint32(GET_FONT_DC(ctx), text, textLen, &size))
		{
			width = size.cx;
		}
		SelectObject(GET_FONT_DC(ctx), (HGDIOBJ) hOldFont);
	}
	
	return (width);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDI_WCE_GetTextWidth (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* f
	)
{
	return GDI_WCE_GetTextWidthLen (ctx, text, f, wcslen(text));
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GDI_WCE_GetFontHeight (
		HTMLGraphicsContext** ctx,
		void* font
	)
{
	return (((WebcGDIFont*)font)->fontHeight);
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_InvalidateRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
	if (!INVALID_RECT_SET(ctx))
	{
		GET_INVALID_RECT(ctx).Set((WebRect*) rect);
		INVALID_RECT_SET(ctx) = WEBC_TRUE;
	}
	else
	{
		// add the new rect to the invalid region
		GET_INVALID_RECT(ctx).Or((WebRect*) rect);
	}	
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_BeginDrawing (HTMLGraphicsContext** c)
{
	WebcGDIWCEGraphics* ctx = (WebcGDIWCEGraphics*) c;
	
	if (!ctx->bufferDepth)
	{
		// if the window has been re-sized, then allocate a new buffer
		// tbd - 
		//RECT box;
		//GetClientRect(ctx->window, &box);
		
#ifdef GDI_DOUBLE_BUFFER
		ctx->outputDC = ctx->bufferDC;
#endif // GDI_DOUBLE_BUFFER
	}
	
	ctx->bufferDepth++;
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_EndDrawing (HTMLGraphicsContext** ctx)
{
	if (GET_BUFFER_DEPTH(ctx) > 0)
	{
		if (GET_BUFFER_DEPTH(ctx) == 1)
		{
#ifdef GDI_DOUBLE_BUFFER
			WebcGDIWCEGraphics* g = (WebcGDIWCEGraphics*) ctx;
			
			// Blit the buffer to the window (only the invalidated region)
			BitBlt(g->windowDC,                    // destination device
			       GET_INVALID_RECT(ctx).left,     // destination x
			       GET_INVALID_RECT(ctx).top,      // destination y
			       GET_INVALID_RECT(ctx).Width(),  // destination width
			       GET_INVALID_RECT(ctx).Height(), // destination height
			       g->bufferDC,                    // source device
			       GET_INVALID_RECT(ctx).left,     // source x
			       GET_INVALID_RECT(ctx).top,      // source y
			       SRCCOPY                         // mode
				);

			g->outputDC = g->windowDC;
#endif // GDI_DOUBLE_BUFFER

			GET_INVALID_RECT(ctx).Set(0,0,-1,-1);
			INVALID_RECT_SET(ctx) = WEBC_FALSE;
		}
		
		GET_BUFFER_DEPTH(ctx)--;
	}	
}

#ifdef WEBC_BUFFER_SCROLLING

/*---------------------------------------------------------------------------*/
HTMLGraphicsContext**  GDI_WCE_CreateBuffer (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT w,
		DISPLAY_INT h)
{
	WebcGDIWCEGraphics* parent = (WebcGDIWCEGraphics*) ctx;
	WebcGDIWCEGraphics* g = (WebcGDIWCEGraphics*) WEBC_MALLOC(sizeof(WebcGDIWCEGraphics));
	if (g)
	{
		rtp_memset(g, 0, sizeof(WebcGDIWCEGraphics));

		HDC hdc = GetDC(parent->window);

		g->ctx          = &webcGDIWCEGraphicsClass;
		g->window       = parent->window;
		g->bufferDC     = CreateCompatibleDC(hdc);
		g->outputDC     = g->bufferDC;
		g->bitmapDC     = CreateCompatibleDC(hdc);
		g->maskDC       = CreateCompatibleDC(hdc);
		g->fontDC       = CreateCompatibleDC(hdc);		
		g->bufferBitmap = CreateCompatibleBitmap(hdc, w, h);
		g->bufferWidth  = w;
		g->bufferHeight = h;

		if (g->bufferBitmap != NULL)
		{
			g->bufferSave = (HBITMAP) SelectObject(g->bufferDC, g->bufferBitmap);
		}
		else
		{
			g->bufferSave = NULL;
		}
		
		g->windowWidth  = parent->windowWidth;
		g->windowHeight = parent->windowHeight;

		ReleaseDC(parent->window, hdc);

		return &(g->ctx);
	}

	return 0;
}

/*---------------------------------------------------------------------------*/
void GDI_WCE_DestroyBuffer (
		HTMLGraphicsContext** ctx,
		HTMLGraphicsContext** buffer)
{
	WebcGDIWCEGraphics* g = (WebcGDIWCEGraphics*) buffer;
	
	if (g->scratchDC)
	{
		DeleteDC(g->scratchDC);
	}

	if (g->scratchBitmap)
	{
		DeleteObject((HGDIOBJ) g->scratchBitmap);
	}

	if (g->bufferDC)
	{
		DeleteDC(g->bufferDC);
	}

	if (g->bufferBitmap)
	{
		DeleteObject((HGDIOBJ) g->bufferBitmap);
	}

	if (g->maskDC)
	{
		DeleteDC(g->maskDC);
	}

	if (g->fontDC)
	{
		DeleteDC(g->fontDC);
	}

	if (g->bitmapDC)
	{
		DeleteDC(g->bitmapDC);
	}

	WEBC_FREE(buffer);	
}

/*---------------------------------------------------------------------------*/
HTMLGraphicsContext**  GDI_WCE_ResizeBuffer (
		HTMLGraphicsContext** ctx,
		HTMLGraphicsContext** buffer,
		DISPLAY_INT w,
		DISPLAY_INT h)
{
	HTMLGraphicsContext** dst = GDI_WCE_CreateBuffer(ctx, w, h);
	if (dst)
	{
		// if successful, destroy the old buffer
		GDI_WCE_DestroyBuffer (ctx, buffer);
	}
	return dst;
}

/*---------------------------------------------------------------------------*/
void  GDI_WCE_CopyBufferRegion (
		HTMLGraphicsContext** dstCtx,
		HTMLRect* dstRect,
		HTMLGraphicsContext** srcBuffer,
		DISPLAY_INT srcX,
		DISPLAY_INT srcY)
{
	WebcGDIWCEGraphics* dst = (WebcGDIWCEGraphics*) dstCtx;
	WebcGDIWCEGraphics* src = (WebcGDIWCEGraphics*) srcBuffer;

	DISPLAY_INT w = dstRect->right  - dstRect->left + 1;
	DISPLAY_INT h = dstRect->bottom - dstRect->top  + 1;\

  #if 0
	if (dst == shello.<br>hello.<br>hello.<br>hello.<br>hello.<br>hello.<br>hello.<br>hello.<br>hello.<br>hello.<br>hello.
rc)
	{
		// check to make sure existing scratch bitmap is large enough
		if (dst->scratchBitmap &&
		    (w > (DISPLAY_INT) dst->scratchWidth || 
			 h > (DISPLAY_INT) dst->scratchHeight))
		{
			DeleteDC(dst->scratchDC);
			DeleteObject((HGDIOBJ) dst->scratchBitmap);
			dst->scratchDC = 0;
			dst->scratchBitmap = 0;
		}

		// create a temporary copy space
		if (!dst->scratchBitmap)
		{
			dst->scratchBitmap = CreateCompatibleBitmap(dst->outputDC, w, h);
			if (dst->scratchBitmap)
			{
				dst->scratchWidth = w;
				dst->scratchHeight = h;
				dst->scratchDC = CreateCompatibleDC(dst->outputDC);
				if (dst->scratchDC)
				{
					SelectObject(dst->scratchDC, dst->scratchBitmap);
				}
				else
				{
					DeleteObject((HGDIOBJ) dst->scratchBitmap);
					dst->scratchDC = 0;
					dst->scratchBitmap = 0;
				}
			}
		}

		if (dst->scratchBitmap)
		{
			BitBlt (dst->scratchDC,
			        0, 
			        0,
			        w,
			        h,
			        src->bufferDC,
			        srcX,
			        srcY,
			        SRCCOPY);

			BitBlt (dst->outputDC,
			        dstRect->left, 
			        dstRect->top,
			        w,
			        h,
			        dst->scratchDC,
			        0,
			        0,
			        SRCCOPY);
		}
	}
	else
  #endif
	{
		BitBlt (dst->outputDC,
		        dstRect->left, 
		        dstRect->top,
		        w,
		        h,
		        src->bufferDC,
		        srcX,
		        srcY,
		        SRCCOPY);	
	}

}

#endif // WEBC_BUFFER_SCROLLING

/*---------------------------------------------------------------------------*/
static void GDI_WCE_TileBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect *Rect,
		DISPLAY_INT xOffset,
		DISPLAY_INT yOffset,
		void* bmp
	)
{
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_DrawPatternFrame (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor color,
		DISPLAY_INT thickness,
		CSSBorderStyle pattern
	)
{
}

/*---------------------------------------------------------------------------*/
static void* GDI_WCE_CreateWebBitmap (
		HTMLGraphicsContext** ctx,
		const WebGraphBitmapInfo* info
	)
{
	WebcGDIBitmap* pNewBitmap = 0;
	int bitsPerPixel = 0;
	
	switch (info->pixelFormat)
	{
		case WB_PIXEL_FORMAT_RGB_24:
			bitsPerPixel = 24;
			break;

		case WB_PIXEL_FORMAT_INDEX_8:
			bitsPerPixel = 8;
			break;

		default:
			// error: unsupported bit depth (our image converters should
			//  always do 8bit or 24bit)
			return (0);
	}

	WEBC_UINT8* lineBuffer = 0;
	if (bitsPerPixel <= 8)
	{
		// allocate a buffer to convert palette type bitmaps to RGB;
		//  for some reason, only 24-bit DIBs convert correctly on 
		//  an actual PocketPC device.  On the emulator, 8-bit DIBs
		//  work just fine.  Go figure.
		lineBuffer = (WEBC_UINT8*) WEBC_MALLOC(info->iWidth * 3);
		if (!lineBuffer)
		{
			return 0;
		}
	}	
	size_t s = sizeof(BITMAPINFO) + sizeof(RGBQUAD)*(info->iPaletteSize);
	PBITMAPINFO pBmpInfo = (PBITMAPINFO) WEBC_MALLOC(s);
	if (pBmpInfo)
	{
		pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pBmpInfo->bmiHeader.biWidth = info->iWidth;
		pBmpInfo->bmiHeader.biHeight = -1; //-info->iHeight; // negative (inexplicibly) signifies top-down
		pBmpInfo->bmiHeader.biPlanes = 1;
		pBmpInfo->bmiHeader.biBitCount = 24; //bitsPerPixel;
		pBmpInfo->bmiHeader.biCompression = BI_RGB;
		pBmpInfo->bmiHeader.biSizeImage = 0;
		pBmpInfo->bmiHeader.biXPelsPerMeter = 1000;
		pBmpInfo->bmiHeader.biYPelsPerMeter = 1000;
		/*
		if (bitsPerPixel <= 8)
		{
			pBmpInfo->bmiHeader.biClrUsed      = info->iPaletteSize;
			pBmpInfo->bmiHeader.biClrImportant = info->iPaletteSize;
		}
		else
		*/
		{
			pBmpInfo->bmiHeader.biClrUsed      = 0;
			pBmpInfo->bmiHeader.biClrImportant = 0;
		}
	
		// set the palette
		for (unsigned c=0; c<info->iPaletteSize; c++)
		{
			pBmpInfo->bmiColors[c].rgbRed      = info->pPalette[c*3];
			pBmpInfo->bmiColors[c].rgbGreen    = info->pPalette[c*3 + 1];
			pBmpInfo->bmiColors[c].rgbBlue     = info->pPalette[c*3 + 2];
			pBmpInfo->bmiColors[c].rgbReserved = 0;
		}

		pNewBitmap = (WebcGDIBitmap*) WEBC_MALLOC(sizeof(WebcGDIBitmap));
		if (pNewBitmap)
		{
			HDC hDefaultDC = GetDC(NULL);
			HDC hBitmapDC = CreateCompatibleDC(hDefaultDC);

			pNewBitmap->hBitmap = CreateCompatibleBitmap(hDefaultDC, info->iWidth, info->iHeight);
			pNewBitmap->hMask = NULL; // see below
			pNewBitmap->bitmapWidth = info->iWidth;
			pNewBitmap->bitmapHeight = info->iHeight;			
			pNewBitmap->hasTrans = 0;

			HGDIOBJ hSave = SelectObject(hBitmapDC, pNewBitmap->hBitmap);
			WEBC_UINT8* linePtr;
			DWORD err;
			for (DISPLAY_INT _y = 0; _y < info->iHeight; _y++)
			{
				linePtr = (WEBC_UINT8*) info->pData + info->iBytesPerRow * _y;

				if (bitsPerPixel == 24)
				{
					WEBC_UINT8 temp;
					int _w = info->iWidth;
					WEBC_UINT8* line = linePtr;
					
					while (_w > 0)
					{
						temp = line[0];
						line[0] = line[2];
						line[2] = temp;
						line += 3;
						_w--;
					}
				}
				else if (bitsPerPixel == 8)
				{									
					int _w = info->iWidth;
					WEBC_UINT8* colorIndex = linePtr;
					WEBC_UINT8* outPixel = lineBuffer;
					
					while (_w != 0)
					{
						*(outPixel++) = pBmpInfo->bmiColors[*colorIndex].rgbBlue;
						*(outPixel++) = pBmpInfo->bmiColors[*colorIndex].rgbGreen;
						*(outPixel++) = pBmpInfo->bmiColors[*colorIndex].rgbRed;
						colorIndex++;
						_w--;
					}

					linePtr = lineBuffer;
				}
				
				int retval = SetDIBitsToDevice(
						hBitmapDC,              // dest device
						0,                      // dest x
						_y,                     // dest y
						info->iWidth,           // src width
						1,                      // src height
						0,                      // src x
						0,                      // src y
						0,                      // scan start
						1,                      // scan lines
						linePtr,                // pixels
						pBmpInfo,               // bitmap info header
						DIB_RGB_COLORS);        // color table type (literal RGB's)

				if (retval == 0)
				{
					err = GetLastError();
				}
			}

			if (info->bHasTransparentColor)
			{
				COLORREF transparent;
				
				if (bitsPerPixel == 8)
				{
					WEBC_UINT8 r,g,b;
					r = info->pPalette[info->iTransparentIndex*3];
					g = info->pPalette[info->iTransparentIndex*3+1];
					b = info->pPalette[info->iTransparentIndex*3+2];
					transparent = RGB(r,g,b);
				}
				else
				{
					transparent = RGB(info->uTransparentRed, info->uTransparentGreen, info->uTransparentBlue);
				}

				WebcGDIBitmap* bmp = pNewBitmap;
				
			  #if 1
				bmp->hasTrans = 1;
				bmp->transColor = transparent;
			  #else								
		   		HBITMAP dstMaskBitmap = CreateBitmap(bmp->bitmapWidth, bmp->bitmapHeight, 1, 1, NULL);
				HDC dstMaskDC = CreateCompatibleDC(hDefaultDC);
				HGDIOBJ hSaveDst = SelectObject(dstMaskDC, dstMaskBitmap);
		
		   		HBITMAP srcMaskBitmap = CreateBitmap(bmp->bitmapWidth, bmp->bitmapHeight, 1, 1, NULL);
				HDC srcMaskDC = CreateCompatibleDC(hDefaultDC);
				HGDIOBJ hSaveSrc = SelectObject(srcMaskDC, srcMaskBitmap);
				
				// create the masks
				COLORREF cColor = SetBkColor(hBitmapDC, transparent);
		  		BitBlt(dstMaskDC, 0, 0, bmp->bitmapWidth, bmp->bitmapHeight, hBitmapDC, 0, 0, SRCCOPY);
				SetBkColor(hBitmapDC, cColor);
				BitBlt(srcMaskDC, 0, 0, bmp->bitmapWidth, bmp->bitmapHeight, dstMaskDC, 0, 0, NOTSRCCOPY);
		
				// apply the mask to the bitmap
				BitBlt(hBitmapDC, 0, 0, bmp->bitmapWidth, bmp->bitmapHeight, srcMaskDC, 0, 0, SRCAND);		
		
				bmp->hMask = dstMaskBitmap;
		
				SelectObject(dstMaskDC, hSaveDst);
				SelectObject(srcMaskDC, hSaveSrc);
		
				DeleteDC(dstMaskDC);
				DeleteDC(srcMaskDC);
				DeleteObject(srcMaskBitmap);
			  #endif
			}
			
			SelectObject(hBitmapDC, hSave);

			DeleteDC(hBitmapDC);
			ReleaseDC(NULL, hDefaultDC);
		}					
		
		WEBC_FREE(pBmpInfo);			
	}	  	
	
	if (lineBuffer)
	{
		WEBC_FREE(lineBuffer);
	}

	return (pNewBitmap);
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_DestroyWebBitmap (
		HTMLGraphicsContext** ctx,
		void* b
	)
{
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_ClaimPointer (
		HTMLGraphicsContext** ctx
	)
{
}

/*---------------------------------------------------------------------------*/
static void GDI_WCE_ReleasePointer (
		HTMLGraphicsContext** ctx
	)
{
}

/*---------------------------------------------------------------------------*/
static WebColor GDI_WCE_GetColorByIndex (
		HTMLGraphicsContext** ctx,
		int i
	)
{
	static colorMap [] =
	{
		RGB(0,0,0)       , // WGC_BLACK
		RGB(255,0,0)     , // WGC_RED
		RGB(0,255,0)     , // WGC_GREEN
		RGB(127,127,0)   , // WGC_BROWN
		RGB(0,0,255)     , // WGC_BLUE
		RGB(255,0,255)   , // WGC_MAGENTA
		RGB(0,127,127)   , // WGC_CYAN
		RGB(168,168,168) , // WGC_LIGHTGRAY
		RGB(96,96,96)    , // WGC_DARKGRAY
		RGB(255,127,127) , // WGC_LIGHTRED
		RGB(127,155,127) , // WGC_LIGHTGREEN
		RGB(255,255,0)   , // WGC_YELLOW
		RGB(127,127,255) , // WGC_LIGHTBLUE
		RGB(255,127,255) , // WGC_LIGHTMAGENTA
		RGB(0,255,255)   , // WGC_LIGHTCYAN
		RGB(255,255,255) , // WGC_WHITE
		RGB(0,0,0)         // WGC_TRANSPARENT
	};

	return colorMap[i];
}

/*---------------------------------------------------------------------------*/
int GDI_WCE_ProcessInputQueue (
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
int webc_InitGDIWCEBrowserConfig (HTMLBrowserConfig* config, HWND windowHandle)
{
	webcGDIWCEGraphicsClass.getCanvasRect              = GDI_WCE_GetCanvasRect;

	webcGDIWCEGraphicsClass.claimPointer               = GDI_WCE_ClaimPointer;
	webcGDIWCEGraphicsClass.releasePointer             = GDI_WCE_ReleasePointer;

	webcGDIWCEGraphicsClass.invalidateRect             = GDI_WCE_InvalidateRect;
	webcGDIWCEGraphicsClass.beginDrawing               = GDI_WCE_BeginDrawing;
	webcGDIWCEGraphicsClass.getClipRect                = GDI_WCE_GetClipRect;
	webcGDIWCEGraphicsClass.setClipRect                = GDI_WCE_SetClipRect;
	webcGDIWCEGraphicsClass.endDrawing                 = GDI_WCE_EndDrawing;

	webcGDIWCEGraphicsClass.colorToRGBA                = GDI_WCE_ColorToRGB;
	webcGDIWCEGraphicsClass.RGBAToColor                = GDI_WCE_RGBToColor;
	webcGDIWCEGraphicsClass.getColorByIndex            = GDI_WCE_GetColorByIndex;

	webcGDIWCEGraphicsClass.drawText                   = GDI_WCE_DrawText;
	webcGDIWCEGraphicsClass.drawTextLen                = GDI_WCE_DrawTextLen;
	webcGDIWCEGraphicsClass.drawBitmap                 = GDI_WCE_DrawBitmap;
	webcGDIWCEGraphicsClass.drawBitmapStretchedToRect  = GDI_WCE_DrawStretchedBitmap;
	webcGDIWCEGraphicsClass.drawRectangle              = GDI_WCE_DrawRectangle;

	webcGDIWCEGraphicsClass.createFont                 = GDI_WCE_CreateFont;
	webcGDIWCEGraphicsClass.getDefaultFont             = GDI_WCE_GetDefaultFont;
	webcGDIWCEGraphicsClass.destroyFont                = GDI_WCE_DestroyFont;
	webcGDIWCEGraphicsClass.getNumFontFamilies         = GDI_WCE_NumFontFamilies;
	webcGDIWCEGraphicsClass.getFontFamilyNames         = GDI_WCE_GetFontFamilyNames;
	webcGDIWCEGraphicsClass.getFontFamilyGenericNames  = GDI_WCE_GetFontFamilyGenericNames;
	webcGDIWCEGraphicsClass.getTextHeight              = GDI_WCE_GetTextHeight;
	webcGDIWCEGraphicsClass.getTextWidth               = GDI_WCE_GetTextWidth;
	webcGDIWCEGraphicsClass.getTextHeightLen           = GDI_WCE_GetTextHeightLen;
	webcGDIWCEGraphicsClass.getTextWidthLen            = GDI_WCE_GetTextWidthLen;
	webcGDIWCEGraphicsClass.getFontHeight              = GDI_WCE_GetFontHeight;

	webcGDIWCEGraphicsClass.createWebBitmap            = GDI_WCE_CreateWebBitmap;
	webcGDIWCEGraphicsClass.destroyWebBitmap           = GDI_WCE_DestroyWebBitmap;
	webcGDIWCEGraphicsClass.createStretchedBitmap      = GDI_WCE_CreateStretchedBitmap;
	webcGDIWCEGraphicsClass.destroyStretchedBitmap     = GDI_WCE_DestroyStretchedBitmap;
	webcGDIWCEGraphicsClass.getBitmapWidth             = GDI_WCE_GetBitmapWidth;
	webcGDIWCEGraphicsClass.getBitmapHeight            = GDI_WCE_GetBitmapHeight;

	webcGDIWCEGraphicsClass.optionalDrawHorizontalLine = 0;
	webcGDIWCEGraphicsClass.optionalDrawVerticalLine   = 0;
	webcGDIWCEGraphicsClass.optionalDrawStyledFrame    = GDI_WCE_DrawPatternFrame;
	//webcGDIWCEGraphicsClass.optionalDrawBitmapTiled    = GDI_WCE_TileBitmap;
	webcGDIWCEGraphicsClass.optionalDrawBitmapTiled    = 0;

  #ifdef WEBC_BUFFER_SCROLLING
	webcGDIWCEGraphicsClass.createBuffer               = GDI_WCE_CreateBuffer;
	webcGDIWCEGraphicsClass.resizeBuffer  	           = GDI_WCE_ResizeBuffer;
	webcGDIWCEGraphicsClass.destroyBuffer              = GDI_WCE_DestroyBuffer;
	webcGDIWCEGraphicsClass.copyBufferRegion	  	   = GDI_WCE_CopyBufferRegion;
  #endif // WEBC_BUFFER_SCROLLING

	WebcGDIWCEGraphics* ctx = (WebcGDIWCEGraphics*) WEBC_MALLOC(sizeof(WebcGDIWCEGraphics));
	if (ctx)
	{
		ctx->ctx = &webcGDIWCEGraphicsClass;
	
		HDC hdc = GetDC(windowHandle);
		
		WEBC_ASSERT(hdc != NULL);
				
		ctx->window = windowHandle;
		ctx->windowDC = GetDC(windowHandle);
		
		// create device contexts
		ctx->bufferDC = CreateCompatibleDC(hdc);
		ctx->bitmapDC = CreateCompatibleDC(hdc);
		ctx->maskDC   = CreateCompatibleDC(hdc);
		ctx->fontDC   = CreateCompatibleDC(hdc);

		ctx->outputDC = ctx->windowDC;
		
		RECT box;
		GetClientRect(windowHandle, &box);
		
		ctx->windowWidth = box.right - box.left;
		ctx->windowHeight = box.bottom - box.top;
		
		ctx->bufferBitmap = CreateCompatibleBitmap(hdc, ctx->windowWidth, ctx->windowHeight);
		if (ctx->bufferBitmap != NULL)
		{
			ctx->bufferSave = (HBITMAP) SelectObject(ctx->bufferDC, ctx->bufferBitmap);
		}
		else
		{
			ctx->bufferSave = NULL;
		}

		ctx->bufferDepth = 0;
		ctx->invalidRectSet = WEBC_FALSE;
		ctx->clipRect.Set(0,0,0,0);
		ctx->invalidRect.Set(0,0,0,0);
		
		ReleaseDC(windowHandle, hdc);

		rtp_memset(config, 0, sizeof(HTMLBrowserConfig));
		config->graphics = (HTMLGraphicsContext**) ctx;
		config->processInputQueue = &gdiWceProcessInputQueueFn;

		return (0);
	}

	return (-1);
}

/*---------------------------------------------------------------------------*/
void webc_DestroyGDIWCEBrowserConfig (HTMLBrowserConfig* config)
{
	// tbd - clean up properly
	WEBC_FREE(config->graphics);
}

/*---------------------------------------------------------------------------*/
WebKey webc_TranslateGDIWCEKey (WEBC_UINT32 rawKey)
{
	if (!gdiWceKeyMapInitialized)
	{
		webc_CreateKeyMap(&gdiWceKeyMap, gdiWceKeyData, WG_NUM_GDI_WCE_KEYS);
		gdiWceKeyMapInitialized = WEBC_TRUE;
	}

	return webc_TranslateKey(&gdiWceKeyMap, rawKey);
}
