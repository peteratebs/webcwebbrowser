#ifndef __WEBCEBS_H__
#define __WEBCEBS_H__

#ifdef __cplusplus

#include "webc.h"

int webc_InitFBBrowserConfig(
	HTMLBrowserConfig* config,
	WEBC_UINT8* windowHandle
);

void webc_DestroyFBBrowserConfig (
	HTMLGraphicsContext** ctx
);


extern void FB_DrawRectangle(
	HTMLGraphicsContext** ctx,
	HTMLRect* rect,
	WebColor outlineColor,
	WebColor fillColor,
	WEBC_BOOL fill
);

/**
*	This function is used to draw a Horizontal line
*/
static void FB_DrawHorizontalLine (
			HTMLGraphicsContext** ctx,
			DISPLAY_INT y,
			DISPLAY_INT left,
			DISPLAY_INT right,
			WebColor color,
			DISPLAY_INT thickness);

/**
*	This function is used to draw a vertical line
*/
static void FB_DrawVerticalLine (
			HTMLGraphicsContext** ctx,
			DISPLAY_INT x,
			DISPLAY_INT top,
			DISPLAY_INT bottom,
			WebColor color,
			DISPLAY_INT thickness);

/**
*   This function must make available the coordinates for webc's drawing
*   area.  The coordinates will be placed in the rectangle parameter.
*
*/
void FB_GetCanvasRect(
    HTMLGraphicsContext** context,
    HTMLRect* rectangle
);

/**
*   This function is called by WebC when a mouse button down event is
*   detected while the pointer is over a slider or button. This routine must
*   insure that all mouse events are delivered to WebC until
*   GUINAME_ReleasePointer() is called.
*/
static void FB_ClaimPointer(
    HTMLGraphicsContext** context
);

/**
*  This function is called by WebC when a mouse button up event is detected
*  and it had previpusly called GUINAME_ClaimPointer(). This routine must
*  inform the GUI framework that WebC no longer requires all mouse events
*  and if the pointer focus has changed it may deliver mouse events to the
*  appropriate window.
*/
static void FB_ReleasePointer(
    HTMLGraphicsContext** context
);



/**
*	This function is called by WebC when it requires a rectangle to be
*   added to drawing operations it must ensure that the GUI framework will
*   allow draw operations may be performed in the region and if it double
*   buffers draw operations it must ensure that all regions marked invalid
*   after a call to GUINAME_BeginDrawing() are refreshed to the display
*   when GUINAME_EndDrawing() is called and the regions are marked valid by
*   GUINAME_EndDrawing(). All invalidated regions must be OR’ed together to
*   create the smallest rectangle that will contain all of the rectangles
*   passed to GUINAME_InvalidateRect.
*/
void FB_InvalidateRect(
	HTMLGraphicsContext** ctx,
	HTMLRect* rect
);

/**
*  	This function is called by WebC when it is beginning a sequence of
*   invalidate and draw operation that will end with a call to GUINAME_
*   EndDrawing. If an off-screen buffer is being used for drawing the
*   underlying draw engine should be informed that a sequence of draws is
*   beginning.
*/
void FB_BeginDrawing(
	HTMLGraphicsContext** ctx
);

/**
*   This function is called by WebC when it is beginning a sequence of
*   invalidate and draw operation has completed and the underlying
*   framework need to update the screen. If necessary the framework should
*   build all invalidated regions to the screen buffer and clear the
*   invalidated region list.
*/
void FB_EndDrawing(
    HTMLGraphicsContext** ctx
);

/**
*   This function is called by WebC when it needs to know what the current
*   clipping region is set to.
*/
void FB_SetClipRect (
	HTMLGraphicsContext** ctx,
	HTMLRect* rect
);

/**
*	This function is called by WebC when it needs to convert one of its named
*	colors to a color that can be passed to native draw functions. The passed
*	argument is a named color and the return value is a 32 bit color value that
*	can be passed directly to the draw functions. The returned color may be a
*   color constant used by the GUI package or an RGB triplet if the GUI package
*	used RGB triplets for drawing colors.
*/
static WebColor FB_GetColorByIndex(
	HTMLGraphicsContext** ctx,
	int color
);
/**
*	This function is called by WebC when it needs to convert and RGB triplet
*	to a natively displayable color that may be passed to native draw
*	functions.
*	The error argument is not used and should be processed as follows.
*	if (*error)
*	{
*		*error = 0;
*	}
*	The following macros are available for extracting R,G and B components from the HTMLColor
*	argument.
*
*	HTML_ColorGetRed(color),
*	HTML_ColorGetGreen(color),
*	HTML_ColorGetBlue(color)
*/
WebColor FB_RGBToColor (
	HTMLGraphicsContext** ctx,
	HTMLColor color,
	HTMLColor* error
);
/**
*	This function is called by WebC when it needs to convert a natively displayable
*	color to an RGB triplet in WebC’s internal form HTMLColor.
*
*	The following macros is available for stuffing R,G, B and A (alpha) components into the
*	HTMLColor 32 bit internal color representation.
*		HTML_RGBAToColor (R, G, B, A)
*	A should always be zero when used in this context.
*/
static HTMLColor FB_ColorToRGB(
	HTMLGraphicsContext** ctx,
	WebColor colorVal
);



/**
*	This function is called by WebC when it needs to draw a NULL terminated text string
*	at X, Y. The text itself should be drawn in the native color passed as TextColor.
*	If Fill is non-zero the text background should be drawn in the native color passed
*	as backgroundColor. The font to use is passed in the void pointer "font". This is
*	the native font family pointer or handle that was returned from the functions
*	createFont or getDefaultFont.
*/
extern void FB_DrawText(
	HTMLGraphicsContext** ctx,
	DISPLAY_INT x,
	DISPLAY_INT y,
	const WebChar* text,
	WebColor textColor,
	WebColor backgroundColor,
	WEBC_BOOL fill,
	void* font
);
/**
*	This function is identical to GUINAME_DrawText except the text to
*	display is not terminated andthe function must display "textlen" characters only.
*/
extern void FB_DrawTextLen(
    HTMLGraphicsContext** ctx,
	DISPLAY_INT x,
	DISPLAY_INT y,
	const WebChar* text,
	WebColor textColor,
	WebColor backgroundColor,
	WEBC_BOOL fill,
	void* font,
	long textLen
);
/**
*	This function is called by WebC to retrieve the height of the rectangle
*	occupied by the null terminated string when it is rendered in the provided
*	font. If the font height is fixed this will be the character height of the
*	font, otherwise it will be the distance from the lowest decender to the
*	highest ascender in the string.
*/
extern DISPLAY_INT FB_GetTextHeight (
	HTMLGraphicsContext** ctx,
	const WebChar* text,
	void* font
);
/**
*	This function is called by WebC to retrieve the height of the rectangle occupied
*	by the provided non terminated sequence of characters of the specified length.
*	It is identical to GUINAME_GetTextHeight() except the string is length specified
*	rather than NULL terminated.
*/
extern DISPLAY_INT FB_GetTextHeightLen (
	HTMLGraphicsContext** ctx,
	const WebChar* text,
	void* font,
	long textLen
);
/**
*	This function is called by WebC to retrieve the width of the rectangle occupied
*	by the null terminated string when it is rendered in the provided font.
*/
extern DISPLAY_INT FB_GetTextWidth (
	HTMLGraphicsContext** ctx,
	const WebChar* text,
	void* font
);

/**
*	This function is called by WebC to retrieve the width of the rectangle occupied by
*	the length specified string when it is rendered in the provided font. It is
*	identical to GUINAME_GetTextWidth() except the string is length specified
*	rather than NULL terminated.
*/
extern DISPLAY_INT FB_GetTextWidthLen (
	HTMLGraphicsContext** ctx,
	const WebChar* text,
	void* font,
	long size
);
/**
*	This function is called by WebC to retrieve the height of a rectangle occupied
*	by the provided font. If the font is fixed height the returned height should
*	be the fixed size. If characters have different heights it should return the
*	distance from the lowest decender in the font to the highest ascender.
*/
extern DISPLAY_INT FB_GetFontHeight (
	HTMLGraphicsContext** ctx,
	void* font
);
/**
*	This function is called by WebC to retrieve the distance in pixels from the top of
*	the cell ascent part of a character to the base of the ascent part. Take for example
*	the letters M and Q. There is no descender for M so the font baseline is the
*	height of the letter. For Q the font baseline is the base base of the large circle.
*	If the pointer to this function is not populated WebC defaults to
*	using four fifths of the font height + 1 as the font baseline.
*/
extern DISPLAY_INT FB_GetFontBaseline(
	HTMLGraphicsContext** ctx,
	void* font
);

/**
*	This function is called by WebC when it needs a bitmap (pixmap) that
*	can be displayed, stretched and measured by native graphics implementation.
*	The routine must allocate data to hold the native formatted bitmap and
*	populate it using the content provided in the WebGraphBitmapInfo
*	structure. The format of WebGraphBitmapInfo is provided above in the WebC
*	type descriptions and should provide a framework for how to create a native
*	bitmap from the input.
*
*	This routine must return a void pointer to the native bitmap or 0 if it fails.
*/
extern void* FB_CreateWebBitmap (
	HTMLGraphicsContext** ctx,
	const WebGraphBitmapInfo* info
);
/**
*	This function is called by WebC when it needs to render a bitmap. The bitmap
*	is passed through the void pointer at *bmp. This bitmap is the void pointer
*	that was returned from GUINAME_CreateWebBitmap().
*/
extern void FB_DrawBitmap (
	HTMLGraphicsContext** ctx,
	DISPLAY_INT x,
	DISPLAY_INT y,
	void* bmp
);
/**
*	This function is called by WebC to release (free) a native bitmap
*	structure that was created by GUINAME_CreateWebBitmap.
*/
extern void FB_DestroyWebBitmap (
	HTMLGraphicsContext** ctx,
	void* b
);

/**
*	This function is called by WebC when it needs to scale a bitmap (pixmap)
*	that was returned from GUINAME_CreateWebBitmap.
*
*   The return value must be a valid void* pointer to the bitmap scaled to the
*   correct size and height.
*   If the routine fails 0 will be returned.
*/
extern void* FB_CreateStretchedBitmap (
		HTMLGraphicsContext** ctx,
		void* b,
		DISPLAY_INT w,
		DISPLAY_INT h
);
/**
*	This function is called by WebC to retrieve the width of a native bitmap
*	structure that was created by FB_CreateWebBitmap or FB_CreateStretchedBitmap.
*/
extern DISPLAY_INT FB_GetBitmapWidth (
	HTMLGraphicsContext** ctx,
	void* bmp
);
/**
*	This function is called by WebC to retrieve the height of a native bitmap
*	structure that was created by FB_CreateWebBitmap or FB_CreateStretchedBitmap.
*/
extern DISPLAY_INT FB_GetBitmapHeight (
	HTMLGraphicsContext** ctx,
	void* bmp
);

/**
*	This function opens and configures a new window for a browser to be
*   displayed in.
*
*   Parameters: HTMLGraphicsContext - graphics associated with window, HTMLBrowserConfig -
*               configurations of the browser for the window, WEBChar title - title of
*               the window.
*/
int FB_WindowOpen (
		HTMLGraphicsContext** ctx,
		HTMLBrowserConfig *pconfig,
        const WebChar *title
);

/**
*	This function is called by the browser destructor to close the window
*
*   Parameters: ctx - graphics context
*/
static void FB_WindowClose (
		HTMLGraphicsContext** ctx
);

/**
*	This function is used to move the window.
*/
void FB_MoveByorTo (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
        WEBC_BOOL is_absolute
);

/**
*	This function is called to get the total area of the available screen.
*
*	Parameters: ctx - graphics context, pinfo - screeninfo for the browser
*/
void FB_ScreenInfo (
		HTMLGraphicsContext** ctx,
		HTMLScreenInfo *pinfo
);

/**
*	Gets the clipping region of the window
*
*	Parameters: ctx - graphics context, rect - rectangle representing the clipping
*				region.
*/
void FB_GetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
);

static int FB_ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
);

#define GET_FBGRAPHICS_CTX(X)	 ((FBGraphicsContext *)(X))
// These should be changed to take HTMLrects
void FB_ReadFrameBuffer(HTMLGraphicsContext **ctx,WEBC_UINT8 *pdata,DISPLAY_INT xPos,DISPLAY_INT yPos,DISPLAY_INT width,DISPLAY_INT height);
void FB_WriteFrameBuffer(HTMLGraphicsContext **ctx, WEBC_UINT8 *pdata, DISPLAY_INT xPos,DISPLAY_INT yPos,DISPLAY_INT width,DISPLAY_INT height);

void *FBWM_createWindow(HTMLGraphicsContext **ctx,HTMLBrowserConfig *pconfig);
void *FBWM_getReceivingWindow(HTMLEvent* e);
void FBWM_setPointerCoordsToFocusWindow(void * FocusWindow, HTMLEvent* e);
void FBWM_moveWindowToTop(void *WindowToActivate, WEBC_BOOL redraw);
HBROWSER_HANDLE FBWM_FindBrowserByWindow(void *target);
void *FBWM_FindWindowByWindowNumber(int WindowNumber);


#endif /* __cplusplus */
#endif /* __WEBCEBS_H__*/
