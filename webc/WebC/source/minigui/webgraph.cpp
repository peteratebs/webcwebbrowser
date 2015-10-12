//
// WEBGRAPH.CPP - Web Client Graphics Abstraction Layer
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "webgraph.hpp"
#include "util.h"
#include "css.h"
#include "webcassert.h"
#include "webcmem.h"
#include "wgbmp.hpp"
#include "webcLog.h"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/
int webc_GuiInitConfig(HTMLBrowserConfig *pconfig);
/* Constructor, called by an existing graphics context to create a new underlying window and graphics context */
WebGraphics::WebGraphics (HTMLGraphicsContext **pGraphics, HTMLBrowserConfig *pconfig, const WebChar *title)
{
    int ret_val;
    /* Call the GUI abstraction layer to create a new GC */
	if (!pconfig->graphics)
		webc_GuiInitConfig(pconfig);
	if(pGraphics && (*pGraphics)->WindowOpen)
	{
		ret_val = (*pGraphics)->WindowOpen(pGraphics, pconfig, title);
		WEBC_ASSERT(ret_val >= 0);
// <EDIT 20080120 What if Window open fails ?
	}
{
	mGraphics = pconfig->graphics;

	int i;
	for (i = 0; i < WGB_NUM_BITMAPS; i++)
	{
		mBitmap[i] = CreateWebBitmap(gWebcDefaultBitmapInfo[i]);
	}

	mbDeleted = 0;
	miUseCount = 0;
}
}


WebGraphics::WebGraphics (HTMLGraphicsContext** ctx)
{
	mGraphics = ctx;

	int i;
	for (i = 0; i < WGB_NUM_BITMAPS; i++)
	{
		mBitmap[i] = CreateWebBitmap(gWebcDefaultBitmapInfo[i]);
	}

	mbDeleted = 0;
	miUseCount = 0;
}

WebGraphics::~WebGraphics ()
{
	WEBC_ASSERT(miUseCount == 0 && mbDeleted);

	int i;
	for (i = 0; i < WGB_NUM_BITMAPS; i++)
	{
		if (mBitmap[i])
		{
			DestroyWebBitmap(mBitmap[i]);
		}
	}
    // Close the underlying window
	if(mGraphics && (*mGraphics)->WindowClose)
		(*mGraphics)->WindowClose(mGraphics);
}

void webc_DestroyFBBrowserConfig(HTMLGraphicsContext **ctx);
void WebGraphics::Destroy (void)
{
	mbDeleted = 1;

	if (miUseCount == 0)
	{
		WEBC_DELETE(this);
	}
}

void WebGraphics::Claim (void)
{
	// try to detect overflow
	WEBC_ASSERT(miUseCount + 1 > miUseCount);

	miUseCount++;
}

void WebGraphics::Release (void)
{
	WEBC_ASSERT(miUseCount > 0);

	miUseCount--;
	if (miUseCount == 0 && mbDeleted)
	{
		WEBC_DELETE(this);
	}
}


WebColor WebGraphics::RGBToColor (
		HTMLColor rgb,
		HTMLColor* error
	)
{
	return ((*mGraphics)->RGBAToColor(mGraphics, rgb, error));
}

HTMLColor WebGraphics::ColorToRGB (
		WebColor c
	)
{
	return (*mGraphics)->colorToRGBA(mGraphics, c);
}




// Font services
DISPLAY_INT WebGraphics::TextHeight (
		const WebChar *text,
		WebFont font
	)
{
	return (*mGraphics)->getTextHeight(mGraphics, text, font->font);
}

DISPLAY_INT WebGraphics::TextWidth (
		const WebChar *text,
		WebFont font
	)
{
	return (*mGraphics)->getTextWidth(mGraphics, text, font->font);
}

DISPLAY_INT WebGraphics::TextHeightLen (
		const WebChar *text,
		WebFont font,
		long textLen
	)
{
	return (*mGraphics)->getTextHeightLen(mGraphics, text, font->font, textLen);
}

DISPLAY_INT WebGraphics::TextWidthLen (
		const WebChar *text,
		WebFont font,
		long textLen
	)
{
	return (*mGraphics)->getTextWidthLen(mGraphics, text, font->font, textLen);
}

// Bitmap services
WebBitmap WebGraphics::CreateWebBitmap (
		const WebGraphBitmapInfo *info
	)
{
	WebBitmap bmp = (WebBitmap) WEBC_MALLOC_VERBOSE(sizeof(struct s_WebBitmap),"CreateWebBitmap");
	if (bmp)
	{
		bmp->bitmap = (*mGraphics)->createWebBitmap(mGraphics, info);
		if (bmp->bitmap)
		{
			bmp->width  = (*mGraphics)->getBitmapWidth(mGraphics, bmp->bitmap);
			bmp->height = (*mGraphics)->getBitmapHeight(mGraphics, bmp->bitmap);
			return bmp;
		}

		WEBC_FREE(bmp);
	}

	return (WEB_BITMAP_NULL);
}

void WebGraphics::DestroyWebBitmap (
		WebBitmap bmp
	)
{
	(*mGraphics)->destroyWebBitmap(mGraphics, bmp->bitmap);
	WEBC_FREE(bmp);
}



// Required rendering services
void WebGraphics::Bitmap (
		DISPLAY_INT x,
		DISPLAY_INT y,
		WebBitmap bmp
	)
{
	if (bmp)
	{
		(*mGraphics)->drawBitmap(mGraphics, x, y, bmp->bitmap);
	}
}

void WebGraphics::Rectangle (
		WebRect* rect,
		WebColor frameColor,
		WebColor fillColor,
		WEBC_BOOL fill
	)
{
	(*mGraphics)->drawRectangle(mGraphics, (HTMLRect*) rect, frameColor, fillColor, fill);
}

void * WebGraphics::PushCanvas (
		WebRect* rect
	)
{
	if((*mGraphics)->pushCanvas)
		return((*mGraphics)->pushCanvas(mGraphics, (HTMLRect*) rect));
	else
		return 0;
}
void WebGraphics::PopCanvas (
	void *pvretPrevCtx,
	int opacity
	)
{
	if((*mGraphics)->popCanvas)
		(*mGraphics)->popCanvas(mGraphics, pvretPrevCtx, opacity);
}

void WebGraphics::SetClip (
		WebRect *Rect
	)
{
	(*mGraphics)->setClipRect(mGraphics, (HTMLRect*) Rect);
}

void WebGraphics::Text (
		DISPLAY_INT x,
		DISPLAY_INT y,
		const WebChar* text,
		WebColor textColor,
		WebColor fillColor,
		WEBC_BOOL fill,
		WebFont font
	)
{
	(*mGraphics)->drawText(mGraphics, x, y, text, textColor, fillColor, fill, font->font);
}

void WebGraphics::TextLen (
		DISPLAY_INT x,
		DISPLAY_INT y,
		const WebChar *text,
		WebColor textColor,
		WebColor fillColor,
		WEBC_BOOL fill,
		WebFont font,
		long size
	)
{
	(*mGraphics)->drawTextLen(mGraphics, x, y, text, textColor, fillColor, fill, font->font, size);
}

// Optional rendering services (depending on feature configuration)
void WebGraphics::StretchBitmap (
		DISPLAY_INT x,
		DISPLAY_INT y,
		DISPLAY_INT w,
		DISPLAY_INT h,
		WebBitmap bmp
	)
{
	PresetWebRect rect(x,y,x+w-1,y+h-1);
	(*mGraphics)->drawBitmapStretchedToRect(mGraphics, (HTMLRect*) &rect, bmp->bitmap);
}

WebBitmap WebGraphics::CreateStretchedBitmap (
		const WebBitmap src,
		DISPLAY_INT w,
		DISPLAY_INT h,
		char *file,
		long line
	)
{
	WebBitmap bmp = (WebBitmap) WEBC_MALLOC_VERBOSE(sizeof(struct s_WebBitmap),"CreateStretchedBitmap");
	if (bmp)
	{
		bmp->bitmap = (*mGraphics)->createStretchedBitmap(mGraphics, src->bitmap, w, h);
		if (bmp->bitmap)
		{
			bmp->width  = (*mGraphics)->getBitmapWidth(mGraphics, bmp->bitmap);
			bmp->height = (*mGraphics)->getBitmapHeight(mGraphics, bmp->bitmap);

			WEBC_ASSERT(bmp->width == w && bmp->height == h);

			return bmp;
		}

		WEBC_FREE(bmp);
	}

	return WEB_BITMAP_NULL;
}

void WebGraphics::DestroyStretchedBitmap (
		WebBitmap bmp,
		char *file,
		long line
	)
{
	(*mGraphics)->destroyStretchedBitmap(mGraphics, bmp->bitmap);
	WEBC_FREE(bmp);
}

// Information services
void WebGraphics::GetClip (WebRect *rect)
{
	(*mGraphics)->getClipRect(mGraphics, (HTMLRect*) rect);
}

void WebGraphics::GetRect (WebRect *rect)
{
	(*mGraphics)->getCanvasRect(mGraphics, (HTMLRect*) rect);
}

// Invalidate adds a rectangular region to the portion of the graphics context already marked dirty.
//  The entire dirty region is what will be updated when EndBuffer is called.
void WebGraphics::Invalidate (WebRect* rect)
{
	(*mGraphics)->invalidateRect(mGraphics, (HTMLRect*) rect);
}

void WebGraphics::StartBuffer (void)
{
	(*mGraphics)->beginDrawing(mGraphics);
}

void WebGraphics::EndBuffer (void)
{
	(*mGraphics)->endDrawing(mGraphics);
}

void WebGraphics::ClaimPointer (void)
{
	(*mGraphics)->claimPointer(mGraphics);
}

void WebGraphics::ReleasePointer (void)
{
	(*mGraphics)->releasePointer(mGraphics);
}

WebColor WebGraphics::GetColorByIndex (int i)
{
	return (*mGraphics)->getColorByIndex(mGraphics, i);
}

WebBitmap WebGraphics::GetBitmapByIndex (int i)
{
	return (i >= 0 && i < WGB_NUM_BITMAPS)? mBitmap[i] : 0;
}


/*****************************************************************************/
// Frame3D - Draw a shaded frame in a graphics context
//
//   Rect - the rectangle bounds
//   hiLite - the color for the top and left sides of the frame
//   loLite - the color for the bottom and right sides of the frame
//   thickness - the pixel thickness of the frame (thickness grows inward from
//     Rect)
//
// Returns
//   nothing
/*****************************************************************************/

void WebGraphics::Frame3D(WebRect *Rect, WebColor hiLite, WebColor loLite, int thickness)
{
WebRect lineRect;

	// tbd - we could do this more efficiently

	for (int n=0; n<thickness; n++)
	{
		// top
		lineRect.Set(Rect->left+n, Rect->top+n, Rect->right-n, Rect->top+n);
		Rectangle(&lineRect, hiLite, hiLite, 1);

		// left
		lineRect.Set(Rect->left+n, Rect->top+n, Rect->left+n, Rect->bottom-n);
		Rectangle(&lineRect, hiLite, hiLite, 1);

		// bottom
		lineRect.Set(Rect->left+n, Rect->bottom-n, Rect->right-n, Rect->bottom-n);
		Rectangle(&lineRect, loLite, loLite, 1);

		// right
		lineRect.Set(Rect->right-n, Rect->top+n, Rect->right-n, Rect->bottom-n);
		Rectangle(&lineRect, loLite, loLite, 1);
	}
}

void WebGraphics::PatternFrame(WebRect *Rect, WebColor color, DISPLAY_INT thickness,
		CSSBorderStyle pattern)
{
	if ((*mGraphics)->optionalDrawStyledFrame)
	{
		(*mGraphics)->optionalDrawStyledFrame (
				mGraphics,
				(HTMLRect*) Rect,
				color,
				thickness,
				pattern
			);
	}
	else
	{
		Frame3D(Rect, color, color, thickness);
	}
}

long WebGraphics::GetCharOffset(DISPLAY_INT x, const WebChar *text, WebFont font, long size)
{
DISPLAY_INT minDist = EBSABS(x), thisDist, w;

	// size<0 implies search to the end of the string (to the null terminator)
	long n;
	for (n=1; (n<size || size<0) && text[n-1] != (WebChar) 0; n++)
	{
		w = TextWidthLen(text, font, n);
		thisDist = EBSABS(w - x);
		if (thisDist > minDist)
		{
			break;
		}
		minDist = thisDist;
	}

	return (n-1);
}

void WebGraphics::TileBitmap(WebRect *Rect, DISPLAY_INT xOffset, DISPLAY_INT yOffset, WebBitmap bmp)
{
	if ((*mGraphics)->optionalDrawBitmapTiled)
	{
		(*mGraphics)->optionalDrawBitmapTiled (
				mGraphics,
				(HTMLRect*) Rect,
				xOffset,
				yOffset,
				bmp->bitmap
			);
	}
	else
	{
		WebRect r = *Rect;
		WebRect clip;
		GetClip(&clip);
		r.And(&clip);
		SetClip(&r);

 		xOffset -= r.left - Rect->left;
		yOffset -= r.top - Rect->top;

		DISPLAY_INT firstX = r.left - WEB_BITMAP_WIDTH(bmp) + (xOffset % WEB_BITMAP_WIDTH(bmp));
		DISPLAY_INT firstY = r.top - WEB_BITMAP_HEIGHT(bmp) + (yOffset % WEB_BITMAP_HEIGHT(bmp));

	/* <EDIT Modified March 2007. Don't display bitmaps that are outside of the viewport.
	   <EDIT	   Improves performance by not relying on underlying clipping */
		for (DISPLAY_INT x = firstX; x < r.right; x += WEB_BITMAP_WIDTH(bmp))
		{
			if ((x + WEB_BITMAP_WIDTH(bmp)) > r.left)
			{
				for (DISPLAY_INT y = firstY; y < r.bottom; y += WEB_BITMAP_HEIGHT(bmp))
				{
					if ((y + WEB_BITMAP_HEIGHT(bmp)) > r.top)
					{
						Bitmap(x, y, bmp);
					}
				}
			}
		}

		// restore saved clipping rectangle
		SetClip(&clip);
	}
}

void WebGraphics::HLine (DISPLAY_INT y, DISPLAY_INT left, DISPLAY_INT right, WebColor color, DISPLAY_INT thickness)
{
	PresetWebRect box(left, y, right, y + thickness - 1);
	Rectangle(&box, color, color, WEBC_TRUE);
}

void WebGraphics::VLine (DISPLAY_INT x, DISPLAY_INT top, DISPLAY_INT bottom, WebColor color, DISPLAY_INT thickness)
{
	PresetWebRect box(x, top, x + thickness - 1, bottom);
	Rectangle(&box, color, color, WEBC_TRUE);
}

WebColor WebGraphics::RGBToColor (HTMLColor rgb)
{
	HTMLColor error;
	return (RGBToColor(rgb, &error));
}

WebColor WebGraphics::RGBToColor (WEBC_UINT8 red, WEBC_UINT8 green, WEBC_UINT8 blue)
{
	return (RGBToColor(HTML_RGBAToColor(red,green,blue,0)));
}

WebColor WebGraphics::RGBToLightColor (HTMLColor rgb)
{
	return (RGBToLightColor (
			HTML_ColorGetRed(rgb),
			HTML_ColorGetGreen(rgb),
			HTML_ColorGetBlue(rgb)
		));
}

WebColor WebGraphics::RGBToLightColor (WEBC_UINT8 red, WEBC_UINT8 green, WEBC_UINT8 blue)
{
	return (RGBToColor(((WEBC_UINT16)red + 720) >> 2,
	                   ((WEBC_UINT16)green + 720) >> 2,
	                   ((WEBC_UINT16)blue + 720) >> 2));
}

WebColor WebGraphics::RGBToDarkColor (HTMLColor rgb)
{
	return (RGBToDarkColor (
			HTML_ColorGetRed(rgb),
			HTML_ColorGetGreen(rgb),
			HTML_ColorGetBlue(rgb)
		));
}

WebColor WebGraphics::RGBToDarkColor (WEBC_UINT8 red, WEBC_UINT8 green, WEBC_UINT8 blue)
{
	return (RGBToColor((WEBC_UINT16)red*4/7, (WEBC_UINT16)green*4/7, (WEBC_UINT16)blue*4/7));
}

WebColor WebGraphics::LightColor (WebColor color)
{
	return (RGBToLightColor(ColorToRGB(color)));
}

WebColor WebGraphics::DarkColor (WebColor color)
{
	return (RGBToDarkColor(ColorToRGB(color)));
}

WebColor WebGraphics::GetColorByName (const WEBC_CHAR *name)
{
	return (0);
}

WEBC_BOOL WebGraphics::HasNativeVScrollDraw ()
{
	return (*mGraphics)->optionalDrawVScroll != WEBC_NULL;
}

WEBC_BOOL WebGraphics::HasNativeHScrollDraw ()
{
	return (*mGraphics)->optionalDrawHScroll != WEBC_NULL;
}

WEBC_BOOL WebGraphics::DrawVScroll (
		WebRect* rect,
		WEBC_UINT32 buttonLength,
		WEBC_UINT32 sliderBegin,
		WEBC_UINT32 sliderLength,
		WEBC_UINT32 scrollFlags)
{
	if ((*mGraphics)->optionalDrawVScroll)
	{
		return (*mGraphics)->optionalDrawVScroll (
				mGraphics,
				(HTMLRect*) rect,
				buttonLength,
				sliderBegin,
				sliderLength,
				scrollFlags);
	}

	return WEBC_FALSE;
}

WEBC_BOOL WebGraphics::DrawHScroll (
		WebRect* rect,
		WEBC_UINT32 buttonLength,
		WEBC_UINT32 sliderBegin,
		WEBC_UINT32 sliderLength,
		WEBC_UINT32 scrollFlags)
{
	if ((*mGraphics)->optionalDrawHScroll)
	{
		return (*mGraphics)->optionalDrawHScroll (
				mGraphics,
				(HTMLRect*) rect,
				buttonLength,
				sliderBegin,
				sliderLength,
				scrollFlags);
	}

	return WEBC_FALSE;
}

#ifdef WEBC_BUFFER_SCROLLING

WebGraphics* WebGraphics::CreateBuffer (DISPLAY_INT w, DISPLAY_INT h)
{
	HTMLGraphicsContext** buffer = (*mGraphics)->createBuffer (
			mGraphics,
			w,
			h);

	if (buffer)
	{
		WebGraphics* bufferGC;

		WEBC_NEW(bufferGC, WebGraphics(buffer));
		if (bufferGC)
		{
			return bufferGC;
		}

		(*mGraphics)->destroyBuffer (
				mGraphics,
				buffer);
	}

	return 0;
}

WebGraphics* WebGraphics::ResizeBuffer (WebGraphics* buffer, DISPLAY_INT w, DISPLAY_INT h)
{
	HTMLGraphicsContext** resizedBuffer = (*mGraphics)->resizeBuffer (
			mGraphics,
			buffer->mGraphics,
			w,
			h);

	if (resizedBuffer)
	{
		buffer->mGraphics = resizedBuffer;
		return buffer;
	}

	return 0;
}

void WebGraphics::DestroyBuffer (WebGraphics* buffer)
{
	(*mGraphics)->destroyBuffer(mGraphics, buffer->mGraphics);
	WEBC_DELETE(buffer);
}

void WebGraphics::CopyBufferRegion (WebRect* dstRect, WebGraphics* fromBuffer, DISPLAY_INT srcX, DISPLAY_INT srcY)
{
	(*mGraphics)->copyBufferRegion (
			mGraphics,
			(HTMLRect*) dstRect,
			fromBuffer->mGraphics,
			srcX,
			srcY);
}

#endif // WEBC_BUFFER_SCROLLING

#if(WEBC_IMAGE_DRAW_EXTENSIONS)

int WebGraphics::CreateGenericBitmap (
		void* webCBitmapSrc,
		WebGraphBitmapInfo** genericBitMapDst)
{
	if(mGraphics && (*mGraphics)->createGenericBitmap)
	{
		return (*mGraphics)->createGenericBitmap(webCBitmapSrc, mGraphics, genericBitMapDst);
	}
	return -1;
}

#endif

void WebGraphics::GetScreenInfo (HTMLScreenInfo *pinfo)
{
	if(mGraphics && (*mGraphics)->ScreenInfo)
	{
		(*mGraphics)->ScreenInfo(mGraphics, pinfo);
	}
}

void WebGraphics::MoveByorTo (
		DISPLAY_INT x,
		DISPLAY_INT y,
        WEBC_BOOL is_absolute)
{
	if(mGraphics && (*mGraphics)->MoveTo)
	{
		(*mGraphics)->MoveTo(mGraphics, x, y, is_absolute);
	}
}

void guiWmBringToTop(HTMLGraphicsContext** ctx);
void WebGraphics::BringToTop(void)
{
	guiWmBringToTop(mGraphics);
}

void guiWmGetPos(HTMLGraphicsContext** ctx,int *top, int *left, int *width, int *height);
void WebGraphics::GetPos(int *left, int *top, int *width, int *height)
{
	guiWmGetPos(mGraphics, left, top,  width, height);
}
void guiWmSetPos(HTMLGraphicsContext** ctx, int top, int left, int width, int height);
void WebGraphics::SetPos(int left, int top, int width, int height)
{
	guiWmSetPos(mGraphics, left, top, width, height);
}
