#ifndef __WEBGRAPH_H__
#define __WEBGRAPH_H__

#include "webc.h"
#include "wgtypes.hpp"
#include "webrect.hpp"
#include "css.h"
#include "webcstr.h"
#include "wgbmp.hpp"

class WebGraphics
{
private:
	int       mbDeleted;
	long      miUseCount;
// <EDIT 20080120 Add get private data to hold browser

	WebBitmap mBitmap[WGB_NUM_BITMAPS];

public:
	HTMLGraphicsContext** mGraphics;

// <EDIT 20080120 Added constructors with Window create and destroy methods
    WebGraphics (HTMLGraphicsContext **pGraphics, HTMLBrowserConfig *pconfig, const WebChar *title);

	WebGraphics (HTMLGraphicsContext** ctx);
	~WebGraphics (void);

	// Must be deleted using this method
	void Destroy (void);

    void MoveByorTo (DISPLAY_INT x, DISPLAY_INT y, WEBC_BOOL is_absolute);
    void GetScreenInfo (HTMLScreenInfo *pinfo);
	void BringToTop(void);
	void GetPos(int *left, int *top, int *width, int *height);
	void SetPos(int left, int top, int width, int height);

	// For preventing premature deletion
	void Claim (void);
	void Release (void);

	// Color services
	WebColor GetColorByName (
			const WEBC_CHAR *name
		);

	WebColor RGBToColor (
			HTMLColor rgb,
			HTMLColor* error
		);

	HTMLColor ColorToRGB (
			WebColor c
		);

	// Font services
	DISPLAY_INT TextHeight (
			const WebChar *text,
			WebFont font
		);

	DISPLAY_INT TextWidth (
			const WebChar *text,
			WebFont font
		);

	DISPLAY_INT TextHeightLen (
			const WebChar *text,
			WebFont font,
			long textLen
		);

	DISPLAY_INT TextWidthLen (
			const WebChar *text,
			WebFont font,
			long textLen
		);

	// Bitmap services
	WebBitmap CreateWebBitmap (
			const WebGraphBitmapInfo *info
		);

	void DestroyWebBitmap (
			WebBitmap bmp
		);

	// Required rendering services
	void Bitmap (
			DISPLAY_INT x,
			DISPLAY_INT y,
			WebBitmap bmp
		);

	void Rectangle (
			WebRect* rect,
			WebColor frameColor,
			WebColor fillColor,
			WEBC_BOOL fill
		);

	void * PushCanvas (
		WebRect* rect
	);

	void PopCanvas (
		void *pvretPrevCtx,
		int opacity
	);
	void SetClip (
			WebRect *Rect
		);

	void Text (
			DISPLAY_INT x,
			DISPLAY_INT y,
			const WebChar* text,
			WebColor textColor,
			WebColor fillColor,
			WEBC_BOOL fill,
			WebFont font
		);

	void TextLen (
			DISPLAY_INT x,
			DISPLAY_INT y,
			const WebChar *text,
			WebColor textColor,
			WebColor fillColor,
			WEBC_BOOL fill,
			WebFont font,
			long size
		);

	// Optional rendering services (depending on feature configuration)
	void StretchBitmap (
			DISPLAY_INT x,
			DISPLAY_INT y,
			DISPLAY_INT w,
			DISPLAY_INT h,
			WebBitmap bmp
		);

	WebBitmap CreateStretchedBitmap (
			const WebBitmap src,
			DISPLAY_INT w,
			DISPLAY_INT h,
			char *file,
			long line
		);

	void DestroyStretchedBitmap (
			WebBitmap bmp,
			char *file,
			long line
		);

	// Information services
	void GetClip (
			WebRect *Rect
		);

	void GetRect (
			WebRect *Rect
		);

	// Invalidate adds a rectangular region to the portion of the graphics context already marked dirty.
	//  The entire dirty region is what will be updated when EndBuffer is called.
	void Invalidate (
			WebRect *Rect
		);

	void StartBuffer (
			void
		);

	void EndBuffer (
			void
		);

	// These methods build on the primitives defined above; these DO NOT need to be ported
	//  (although there may be some performance to be gained by specifically implementing each one)
	void Frame3D (
			WebRect *Rect,
			WebColor hiLite,
			WebColor loLite,
			int thickness
		);

	long GetCharOffset (
			DISPLAY_INT x,
			const WebChar* text,
			WebFont font,
			long size = -1
		);

	void TileBitmap (
			WebRect *Rect,
			DISPLAY_INT xOffset,
			DISPLAY_INT yOffset,
			WebBitmap bmp
		);

	void HLine (
			DISPLAY_INT y,
			DISPLAY_INT left,
			DISPLAY_INT right,
			WebColor color,
			DISPLAY_INT thickness = 1
		);

	void VLine (
			DISPLAY_INT x,
			DISPLAY_INT top,
			DISPLAY_INT bottom,
			WebColor color,
			DISPLAY_INT thickness = 1
		);

	void PatternFrame (
			WebRect* rect,
			WebColor color,
			DISPLAY_INT thickness,
			CSSBorderStyle pattern
		);

	WebColor RGBToColor (
			HTMLColor rgb
		);

	WebColor RGBToColor (
			WEBC_UINT8 red,
			WEBC_UINT8 green,
			WEBC_UINT8 blue
		);

	WebColor RGBToLightColor (
			HTMLColor rgb
		);

	WebColor RGBToLightColor (
			WEBC_UINT8 red,
			WEBC_UINT8 green,
			WEBC_UINT8 blue
		);

	WebColor RGBToDarkColor (
			HTMLColor rgb
		);

	WebColor RGBToDarkColor (
			WEBC_UINT8 red,
			WEBC_UINT8 green,
			WEBC_UINT8 blue
		);

	WebColor LightColor (
			WebColor color
		);

	WebColor DarkColor (
			WebColor color
		);

	void ClaimPointer (void);
	void ReleasePointer (void);

	WEBC_BOOL HasNativeVScrollDraw ();
	WEBC_BOOL HasNativeHScrollDraw ();

	WEBC_BOOL DrawVScroll (
			WebRect* rect,
			WEBC_UINT32 buttonLength,
			WEBC_UINT32 sliderBegin,
			WEBC_UINT32 sliderLength,
			WEBC_UINT32 scrollFlags);

	WEBC_BOOL DrawHScroll (
			WebRect* rect,
			WEBC_UINT32 buttonLength,
			WEBC_UINT32 sliderBegin,
			WEBC_UINT32 sliderLength,
			WEBC_UINT32 scrollFlags);

	// see wgcolors.hp
	WebColor  GetColorByIndex  (int wgColorIndex);

	// see wgbmp.hpp
	WebBitmap GetBitmapByIndex (int wgBitmapIndex);

#ifdef WEBC_BUFFER_SCROLLING
	WebGraphics* CreateBuffer (DISPLAY_INT w, DISPLAY_INT h);
	WebGraphics* ResizeBuffer (WebGraphics* buffer, DISPLAY_INT w, DISPLAY_INT h);
	void DestroyBuffer (WebGraphics* buffer);
	void CopyBufferRegion (WebRect* dstRect, WebGraphics* fromBuffer, DISPLAY_INT srcX, DISPLAY_INT srcY);
#endif // WEBC_BUFFER_SCROLLING

#if(WEBC_IMAGE_DRAW_EXTENSIONS)
	int CreateGenericBitmap(void* webCBitmapSrc, WebGraphBitmapInfo** genericBitMapDst);
#endif

};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // __WEBGRAPH_H__
