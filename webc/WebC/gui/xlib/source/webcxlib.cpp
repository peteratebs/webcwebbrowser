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
#include "webcxlib.h"
#include "wfxlib.h"
#include "webcmem.h"
#include "rtpstr.h"
#include "webcassert.h"
#include "webcstr.h"
#include "webcfg.h"
#include <pthread.h>    /* webc helper thread for generating timer events */

#ifdef WEBC_BUFFER_SCROLLING
#undef WEBC_BUFFER_SCROLLING
#endif

/*  Miscellaneous X variables  */
static    XSizeHints *  size_hints=0;
static    XWMHints   *  wm_hints=0;
static    XClassHint *  class_hints=0;
static char * window_name = "webCX";
static char * icon_name   = "webCX";
static char * appname = "webCX";
static int directConv[256];
/*****************************************************************************/
/* Macros
 *****************************************************************************/
typedef WEBC_UCHAR8 byte;
#define DEFAULT_FONT 	"lucidasans-10"

#define HTML_TO_XRECT(x, h) (x->x = (short)h->top, x->y = (short)h->left, \
                            x->width = (unsigned short)(h->right - h->left), \
                            x->height = (unsigned short)(h->bottom - h->top))

#define RGB(R,G,B) HTML_RGBAToColor(R,G,B,0xff)

/*****************************************************************************/
/* Types
 *****************************************************************************/

 struct raster_draw {
	long x; long y; int color; int length;};


struct webc_ximage {
XImage *pxim;
int height_if_raster;
int width_if_raster;
int raster_list_size;
WebBitmapPixelFormat pixelFormat;
WebColor *colormap;
struct raster_draw *pxraster;
};


/*****************************************************************************/
/* Function Prototypes
/*****************************************************************************/

static int XLIB_ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	);

WEBC_BOOL xlib_create_window (WebcXLIBGraphics *ctx, HTMLBrowserConfig* config);
void xlib_bind_window_to_browser(HTMLBrowserConfig *config, HBROWSER_HANDLE browser);

/*****************************************************************************/
/* Data
 *****************************************************************************/

HTMLGraphicsContext webcXLIBGraphicsClass = {0};

int (*xlibProcessInputQueueFn) (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode) = XLIB_ProcessInputQueue;

#define WG_NUM_XLIB_KEYS 35
// DONE - Key translations
static WEBC_UINT32 xlibKeyData [WG_NUM_XLIB_KEYS*2] =
{
	WGK_TAB         , 0x09 ,
	WGK_CR          , XK_Return ,
	WGK_PGUP        , XK_Prior  ,
	WGK_PGDN        , XK_Next  ,
	WGK_LNUP        , XK_Up   ,
	WGK_LNDN        , XK_Down  ,
	WGK_ALT         , XK_Alt_L  ,
	WGK_ALT         , XK_Alt_R  ,
	WGK_CTRL        , XK_Control_L  ,
	WGK_CTRL        , XK_Control_R  ,
	WGK_SHIFT       , XK_Shift_L  ,
	WGK_SHIFT       , XK_Shift_R  ,
	WGK_CAPS        , XK_Caps_Lock  ,
	WGK_NUMLOCK     , XK_Num_Lock ,
	WGK_SCROLLLOCK  , XK_Meta_R  ,
	WGK_F1          , XK_F1	  ,
	WGK_F2          , XK_F2	  ,
	WGK_F3          , XK_F3	  ,
	WGK_F4          , XK_F4	  ,
	WGK_F5          , XK_F5	  ,
	WGK_F6          , XK_F6	  ,
	WGK_F7          , XK_F7	  ,
	WGK_F8          , XK_F8	  ,
	WGK_F9          , XK_F9	  ,
	WGK_F10         , XK_F10  ,
	WGK_F11         , XK_F11  ,
	WGK_F12         , XK_F12  ,
	WGK_INSERT      , XK_Insert  ,
	WGK_DELETE      , XK_Delete ,
	WGK_BACKSPACE   , XK_BackSpace ,
	WGK_LEFT        , XK_Left ,
	WGK_RIGHT       , XK_Right  ,
	WGK_HOME        , XK_Home  ,
	WGK_END         , XK_End  ,
	WGK_ESC         , 0x1b
};

static WebcKeyMap xlibKeyMap;
static WEBC_BOOL  xlibKeyMapInitialized = WEBC_FALSE;


/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

#define DONE 0
/*---------------------------------------------------------------------------*/
static void XLIB_SetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
    WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
	XRectangle xrect;

	pxctx->current_cliprect = *rect;
	xrect.x = (short)pxctx->current_cliprect.left;
    xrect.y = (short)pxctx->current_cliprect.top;
    xrect.width = (unsigned short)(pxctx->current_cliprect.right - pxctx->current_cliprect.left);
    xrect.height = (unsigned short)(pxctx->current_cliprect.bottom - pxctx->current_cliprect.top);
	XSetClipRectangles(pxctx->display, pxctx->gc, 0, 0, &xrect, 1, Unsorted);
}

void XLIB_ScreenInfo (HTMLGraphicsContext** ctx, HTMLScreenInfo *pinfo)
{
    pinfo->availHeight = 480; // TODO hardcode for now
    pinfo->availWidth  = 640;
    pinfo->height      = 480;
    pinfo->width       = 640;
    pinfo->colorDepth  = 24;
}

int XLIB_WindowOpen (
		HTMLGraphicsContext** ctx,
		HTMLBrowserConfig *pconfig,
        const WebChar *title
        )
{
	WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)*ctx;
    int retVal = 0;

	retVal = webc_InitXLIBBrowserConfig(pconfig, WEBC_TRUE);
	if(retVal >= 0)
	{
        // Bind the handle to the browser. Also required in config structure, for backwards compatibility
		pconfig->WindowHandle = (void *) pxctx->windowHandle;
        webc_SetBrowserWindow (pconfig->BrowserHandle, (void *) pxctx->windowHandle);
        xlib_bind_window_to_browser(pconfig, pconfig->BrowserHandle);
	}
	return retVal;
}

static void XLIB_WindowClose (       /* Called */
		HTMLGraphicsContext** ctx
        )
{
	WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)*ctx;
	XDestroyWindow(pxctx->display, pxctx->windowHandle);
}

void XLIB_MoveByorTo (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
        WEBC_BOOL is_absolute
        )
{
	WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)*ctx;
      XWindowAttributes window_attributes;

	    if (XGetWindowAttributes(pxctx->display, pxctx->windowHandle, &window_attributes))
	    {
	        int width =  (window_attributes.width);
	        int height = (window_attributes.height);
	        if (!is_absolute)
	        {
	            x += window_attributes.x;
	            y += window_attributes.y;
	        }
	        XMoveWindow(pxctx->display, pxctx->windowHandle, x, y);
		}
}



/*---------------------------------------------------------------------------*/
static void XLIB_GetCanvasRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
    WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
	XWindowAttributes window_attributes;
	rect->left = 0;
	rect->top = 0;
	rect->right = pxctx->xlib_width;
	rect->bottom = pxctx->xlib_height;
}

/*---------------------------------------------------------------------------*/
static void XLIB_GetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
    WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
    *rect = pxctx->current_cliprect;
}

/*---------------------------------------------------------------------------*/
void xlib_set_pen_color(WebcXLIBGraphics *pxctx, WebColor penColor)
{
    XSetForeground(pxctx->display, pxctx->gc, penColor);
}

/*---------------------------------------------------------------------------*/
static void XLIB_DrawRectangle (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor outlineColor,
		WebColor fillColor,
		WEBC_BOOL fill
	)
{
    WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;

    xlib_set_pen_color(pxctx, fillColor);
	XDrawRectangle(
            pxctx->display,
            pxctx->windowHandle,
            pxctx->gc,
            (short)rect->left,
            (short)rect->top,
            (unsigned short)(rect->right-rect->left),
            (unsigned short)(rect->bottom-rect->top));

	if (fill || outlineColor == fillColor)
	{
        xlib_set_pen_color(pxctx,outlineColor); //Vikram .. does not affect
		XFillRectangle(
				pxctx->display,
				pxctx->windowHandle,
				pxctx->gc,
	        	(short)rect->left,
				(short)rect->top,
				(unsigned short)(rect->right-rect->left),
				(unsigned short)(rect->bottom-rect->top)
			);
    }
}

static WebColor XLIB_GetColorByIndex (
		HTMLGraphicsContext** ctx,
		int i
									 );

static void _XLIB_DrawRasterList (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		struct webc_ximage *pwebxim)
{
	int i;
    WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
	struct raster_draw *praster = pwebxim->pxraster;

	for (i = 0; i < pwebxim->raster_list_size; i++, praster++)
	{
		// HEREHERE - color is index value, (0, 4 etc)... should be BLACK, RED etc..
		if(pwebxim->pixelFormat == WB_PIXEL_FORMAT_INDEX_8 || pwebxim->pixelFormat == WB_PIXEL_FORMAT_MONOCHROME_8)
			xlib_set_pen_color(pxctx, pwebxim->colormap[praster->color]);
		else
			xlib_set_pen_color(pxctx, praster->color);

    	//xlib_set_pen_color(pxctx, praster->color);
		// HEREHERE - Use line draw
		XFillRectangle(
				pxctx->display,
				pxctx->windowHandle,
				pxctx->gc,
	        	(short)praster->x+x,
				(short)praster->y+y,
				(unsigned short)(praster->length),
				(unsigned short)(1)
			);
	}
}

/*---------------------------------------------------------------------------*/
static void XLIB_DrawBitmap (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		void* bmp
	)
{
    XImage *pimage;
    WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
    struct webc_ximage *pwebxim;

    pwebxim = (struct webc_ximage *) bmp;

    pimage = pwebxim->pxim;

	if (pwebxim->pxraster)
	{
		_XLIB_DrawRasterList (ctx, x, y, pwebxim);
	}
	else
	{
    	int rv = XPutImage(pxctx->display, pxctx->windowHandle, pxctx->gc,
        	pimage, 0, 0, x, y, pimage->width, pimage->height);
	}
}

/*---------------------------------------------------------------------------*/
static void XLIB_DrawStretchedBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		void* b
	)
{
    XLIB_DrawBitmap (ctx, rect->left, rect->top, b);
}

/*---------------------------------------------------------------------------*/
static void* XLIB_CreateStretchedBitmap (
		HTMLGraphicsContext** ctx,
		void* b,
		DISPLAY_INT w,
		DISPLAY_INT h
	)
{
#if (DONE)
#endif
	return 0;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT XLIB_GetBitmapWidth (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
    XImage *pimage;
	struct webc_ximage *pwebxim = (struct webc_ximage *) bmp;

    pimage = pwebxim->pxim;
	if (pimage)
    	return(pimage->width);
	else
    	return(pwebxim->width_if_raster);
}


/*---------------------------------------------------------------------------*/
static DISPLAY_INT XLIB_GetBitmapHeight (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
    XImage *pimage;
	struct webc_ximage *pwebxim = (struct webc_ximage *) bmp;


    pimage = pwebxim->pxim;
	if (pimage)
    	return(pimage->height);
	else
    	return(pwebxim->height_if_raster);

}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT XLIB_GetFontHeight (
		HTMLGraphicsContext** ctx,
		void* f
	)
{
XFontStruct *pfont_info;
DISPLAY_INT h;
    pfont_info = (XFontStruct *) f;
    h = pfont_info->max_bounds.ascent + pfont_info->max_bounds.descent;
    return(h);
}

/*---------------------------------------------------------------------------*/
static void XLIB_DrawTextLen (
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
	XFontStruct *pfont_info;
	WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
    pfont_info = (XFontStruct *) f;

	//y += (pfont_info->max_bounds.ascent + pfont_info->max_bounds.descent); // PETER
	//y += (pfont_info->ascent + pfont_info->descent); // Vikram
	y += pfont_info->max_bounds.ascent;
	XSetForeground(pxctx->display, pxctx->gc, textColor);
    XSetBackground(pxctx->display, pxctx->gc, fillColor);

	//XSetForeground(pxctx->display, pxctx->gc, BlackPixel(pxctx->display, pxctx->screen_num));
	//XSetBackground(pxctx->display, pxctx->gc, WhitePixel(pxctx->display, pxctx->screen_num));
	XSetFont(pxctx->display, pxctx->gc, pfont_info->fid); //Vikram affects big time..
#if (WEBC_SUPPORT_UNICODE)
	XDrawString16(pxctx->display, pxctx->windowHandle, pxctx->gc, x, y, (const XChar2b*) text, textLen);
#else
    XDrawString(pxctx->display, pxctx->windowHandle, pxctx->gc, x, y, text, textLen);
#endif
}

/*---------------------------------------------------------------------------*/
static void XLIB_DrawText (
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
long textLen;
    textLen = webc_strlen(text);
    XLIB_DrawTextLen (ctx,	x,	y,	text,	textColor,	fillColor,	fill,	f, textLen);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT XLIB_GetTextHeight (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* f
	)
{

	int height = XLIB_GetFontHeight (ctx, f);
	return (height);
	//return(XLIB_GetFontHeight (ctx, f)); orig
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT XLIB_GetTextHeightLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* f,
		long textLen
	)
{
	int height = XLIB_GetFontHeight (ctx, f);
	return (height);

	//return(XLIB_GetFontHeight (ctx, f));
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT XLIB_GetTextWidth (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* f
	)
{
XFontStruct *pfont_info;
DISPLAY_INT w;
    pfont_info = (XFontStruct *) f;
    //w = pfont_info->max_bounds.width; vikram
	w = pfont_info->max_bounds.rbearing - pfont_info->min_bounds.lbearing;
	//printf("GTW: Text == %S, rbearing == %d, lbearing == %d w == %d res == %d\n", text, pfont_info->max_bounds.rbearing, pfont_info->min_bounds.lbearing, w, w * webc_strlen(text));
#ifdef WEBC_SUPPORT_UNICODE
	w = XTextWidth16(pfont_info, (XChar2b*)text, webc_strlen(text));
#else
	w = XTextWidth(pfont_info, text, webc_strlen(text));
#endif
	//printf("New w == %d\n", w);
return(w);


    return(w * webc_strlen(text));
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT XLIB_GetTextWidthLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* f,
		long textLen
	)
{
XFontStruct *pfont_info;
DISPLAY_INT w;
    pfont_info = (XFontStruct *) f;
    //w = pfont_info->max_bounds.width; //vikram
	w = pfont_info->max_bounds.rbearing - pfont_info->min_bounds.lbearing;
	//printf("GTWL:  rbearing == %d, lbearing == %d w == %d lextlen = %d res == %d\n", pfont_info->max_bounds.rbearing, pfont_info->min_bounds.lbearing, w, textLen, w * textLen);
#ifdef WEBC_SUPPORT_UNICODE
	w = XTextWidth16(pfont_info, (XChar2b*)text, textLen);
#else
	w = XTextWidth(pfont_info, text, textLen);
#endif
	//printf("New w == %d\n", w);
	//printf("Text = |%s|, len == %d\n", text, textLen);
	return(w);

    return(w * textLen);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT XLIB_GetFontBaseline (
		HTMLGraphicsContext** ctx,
		void* f
	)
{
XFontStruct *pfont_info;
    pfont_info = (XFontStruct *) f;
	return((DISPLAY_INT)(pfont_info->max_bounds.ascent + pfont_info->max_bounds.descent));
}

/*---------------------------------------------------------------------------*/
static WebColor XLIB_RGBToColor (
		HTMLGraphicsContext** ctx,
		HTMLColor color,
		HTMLColor* error
	)
{
XColor color_cell;

	WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
   	unsigned short red, green, blue;
   	/* Open display, etc. */
	color_cell.flags= DoRed | DoGreen | DoBlue;
	color_cell.red = HTML_ColorGetRed(color);
	color_cell.green = HTML_ColorGetGreen(color);
	color_cell.blue = HTML_ColorGetBlue(color);

	if (XAllocColor(pxctx->display,DefaultColormap(pxctx->display, pxctx->screen_num),&color_cell)==0)
	if (error)
	{
		*error = 0;
	}
	return((WebColor) color);
}

/*---------------------------------------------------------------------------*/
static HTMLColor XLIB_ColorToRGB (
		HTMLGraphicsContext** ctx,
		WebColor colorval
	)
{
HTMLColor hcolor;
XColor color;
   WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;

    color.pixel = colorval;
    XQueryColor(pxctx->display, DefaultColormap(pxctx->display, pxctx->screen_num), &color);
    HTML_ColorSet(hcolor,color.red,color.green,color.blue,0xff);
    return(hcolor);
}

/*---------------------------------------------------------------------------*/
static void XLIB_InvalidateRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect
	)
{
WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
#if (DONE)
PETERPETER
	WebcXLIBGraphics* g = (WebcXLIBGraphics*) ctx;
	g->invalidRect.Or((WebRect*) rect);
#endif
}

/*---------------------------------------------------------------------------*/
static void XLIB_BeginDrawing (HTMLGraphicsContext** ctx)
{
WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;

	pxctx->current_cliprect.top =
    pxctx->current_cliprect.left =
   	pxctx->current_cliprect.right =
	pxctx->current_cliprect.left = 0;

#ifdef XLIB_DOUBLE_BUFFER
#endif // XLIB_DOUBLE_BUFFER
}

/*---------------------------------------------------------------------------*/
static void XLIB_EndDrawing (HTMLGraphicsContext** ctx)
{
WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;

	pxctx->current_cliprect.top =
    pxctx->current_cliprect.left =
   	pxctx->current_cliprect.right =
	pxctx->current_cliprect.left = 0;
#ifdef XLIB_DOUBLE_BUFFER
#endif
}

/*---------------------------------------------------------------------------*/
static void XLIB_TileBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect *rect,
		DISPLAY_INT xOffset,
		DISPLAY_INT yOffset,
		void* bmp
	)
{
#if (DONE)
PETERPETER
#endif
}

/*---------------------------------------------------------------------------*/
static void XLIB_DrawPatternFrame (
		HTMLGraphicsContext** ctx,
		HTMLRect* rect,
		WebColor color,
		DISPLAY_INT thickness,
		CSSBorderStyle pattern
	)
{
#if (DONE)
PETERPETER
#endif
}


static void XLIB_FreeXImage (XImage *pxim)
{
    if (pxim->data)
        WEBC_FREE(pxim->data);
    pxim->data = 0;
    XDestroyImage(pxim);
}

/*---------------------------------------------------------------------------*/
int highbit( unsigned long ul )
{
  int i;  unsigned long hb;
  hb = 0x8000;  hb = (hb<<16);  /* hb = 0x80000000UL */
  for (i=31; ((ul & hb) == 0) && i>=0;  i--, ul<<=1);
  return i;
}

/*---------------------------------------------------------------------------*/
static int xlib_MakeDirectColorMap(
		HTMLGraphicsContext** ctx
	)
{
	WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
	int    i, j, cmaplen, numgot;
	byte   origgot[256];
	XColor c;
	u_long rmask, gmask, bmask;
	int    rshift, gshift, bshift;
	Visual *theVisual;
	theVisual = DefaultVisual(pxctx->display,pxctx->screen_num);

	rmask = theVisual->red_mask;
	gmask = theVisual->green_mask;
	bmask = theVisual->blue_mask;

	rshift = highbit(rmask) - 15;
	gshift = highbit(gmask) - 15;
	bshift = highbit(bmask) - 15;

	if (rshift<0) rmask = rmask << (-rshift);
		else rmask = rmask >> rshift;

	if (gshift<0) gmask = gmask << (-gshift);
		else gmask = gmask >> gshift;

	if (bshift<0) bmask = bmask << (-bshift);
		else bmask = bmask >> bshift;


	cmaplen = theVisual->map_entries;
	if (cmaplen>256)
		cmaplen=256;


	/* try to alloc a 'cmaplen' long grayscale colormap.  May not get all
	    entries for whatever reason.  Build table 'directConv[]' that
	    maps range [0..(cmaplen-1)] into set of colors we did get */

	for (i=0; i<256; i++)
	{
		origgot[i] = 0;
		directConv[i] = 0;
	}

	for (i=numgot=0; i<cmaplen; i++)
	{
		c.red = c.green = c.blue = (i * 0xffff) / (cmaplen - 1);
		c.red   = c.red   & rmask;
		c.green = c.green & gmask;
		c.blue  = c.blue  & bmask;
		c.flags = DoRed | DoGreen | DoBlue;

		if (XAllocColor(
				pxctx->display,
				DefaultColormap(pxctx->display, DefaultScreen(pxctx->display)),
				&c))
		{
		    /* fprintf(stderr,"%3d: %4x,%4x,%4x\n", i, c.red,c.green,c.blue); */
		    directConv[i] = i;
		    origgot[i] = 1;
		    numgot++;
		}
	}


	if (numgot == 0)
		return(-1);

	/* directConv may or may not have holes in it. */
	for (i=0; i<cmaplen; i++)
	{
		if (!origgot[i])
		{
		    int numbak, numfwd;
		    numbak = numfwd = 0;
		    while ((i - numbak) >= 0       && !origgot[i-numbak]) numbak++;
		    while ((i + numfwd) <  cmaplen && !origgot[i+numfwd]) numfwd++;

		    if (i-numbak<0        || !origgot[i-numbak]) numbak = 999;
		    if (i+numfwd>=cmaplen || !origgot[i+numfwd]) numfwd = 999;

		    if      (numbak<numfwd) directConv[i] = directConv[i-numbak];
		    else if (numfwd<999)    directConv[i] = directConv[i+numfwd];
		    else return(-1);
		}
	}
	return(0);
}

static int _XLIB_CreateTransparentBitmap (
		HTMLGraphicsContext** ctx,
		const WebGraphBitmapInfo* info,
		struct webc_ximage *pwebxim
										 );

/*---------------------------------------------------------------------------*/
static void* XLIB_CreateWebBitmap (
		HTMLGraphicsContext** ctx,
		const WebGraphBitmapInfo* info
	)
{
Pixmap xpix;
XImage *pxim;
WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
Visual *theVisual;
struct webc_ximage *pwebxim;
WEBC_UINT8   *bcolormap;

	pwebxim = (struct webc_ximage *) WEBC_MALLOC(sizeof(struct webc_ximage));

	if (!pwebxim)
		return (0);
	rtp_memset(pwebxim, 0, sizeof(struct webc_ximage));

	pwebxim->pixelFormat  = info->pixelFormat;

    if (info->bHasTransparentColor)
	{
		if (info->pixelFormat == WB_PIXEL_FORMAT_INDEX_8)
		{
			if (!_XLIB_CreateTransparentBitmap (ctx, info, pwebxim))
			{
				if(pwebxim->colormap)
				{
					WEBC_FREE(pwebxim->colormap);
				}
				WEBC_FREE(pwebxim);
				return(0);
			}

		}
		else
		{
			if(pwebxim->colormap)
			{
				WEBC_FREE(pwebxim->colormap);
			}
			WEBC_FREE(pwebxim);
			return(0);
		}
		return(pwebxim);
	}

    pxim     = (XImage *) NULL;
    theVisual = DefaultVisual(pxctx->display,pxctx->screen_num);

	// TBDTBD
	if(info->pixelFormat == WB_PIXEL_FORMAT_INDEX_8 || info->pixelFormat == WB_PIXEL_FORMAT_MONOCHROME_8)
	{
		int i,j;
		bcolormap = (WEBC_UINT8*)WEBC_MALLOC (info->iPaletteSize * 4);

		if (!bcolormap)
		{
			WEBC_FREE(pwebxim);
			return(0);
		}
		pwebxim->colormap = (WebColor *) bcolormap;

		for (i = 0; i < info->iPaletteSize; i++)
		{
			*(bcolormap + (i*4))   = info->pPalette[i + (i<<1) + 0]; // r
			*(bcolormap + (i*4)+1) = info->pPalette[i + (i<<1) + 1]; // g
			*(bcolormap + (i*4)+2) = info->pPalette[i + (i<<1) + 2]; // g
		}
	}


    if (theVisual->c_class != TrueColor && theVisual->c_class != DirectColor)
    {
		if(pwebxim->colormap)
		{
			WEBC_FREE(pwebxim->colormap);
		}
		WEBC_FREE(pwebxim);
        return(0);
    }
	/* make linear colormap for DirectColor visual */
	if (theVisual->c_class == DirectColor)
	{
		xlib_MakeDirectColorMap(ctx);
	}

/*
Note: Prelimary, not handling the following

	WEBC_BOOL            info->bHasTransparentColor;
	WEBC_INT16           info->iTransparentIndex;
	WEBC_UINT8           info->uTransparentRed;
	WEBC_UINT8           info->uTransparentGreen;
	WEBC_UINT8           info->uTransparentBlue;
	WEBC_UINT32          info->iPaletteSize;
	const WEBC_UINT8*    info->pPalette;
*/
    unsigned long r, g, b, rmask, gmask, bmask, xcol;
    int           rshift, gshift, bshift, bperpix, bperpix_input, bperline, border, cshift;
    int           maplen, i, j;
    WEBC_UINT8    *imagedata, *lip, *ip, *lpp, *pp;


	if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
        bperpix_input = 3;
	else if (info->pixelFormat == WB_PIXEL_FORMAT_INDEX_8)
        bperpix_input = 1;
	else if (info->pixelFormat == WB_PIXEL_FORMAT_MONOCHROME_8)
        bperpix_input = 1;
	else
	{
		if(pwebxim->colormap)
		{
			WEBC_FREE(pwebxim->colormap);
		}
		WEBC_FREE(pwebxim);
	    return(0);
	}


    /************************************************************************/
    /* Non-Mapped Visuals:  TrueColor, DirectColor                     */
    /************************************************************************/

    /* compute various shifting constants that we'll need... */
    rmask = theVisual->red_mask;
    gmask = theVisual->green_mask;
    bmask = theVisual->blue_mask;

    rshift = 7 - highbit(rmask);
    gshift = 7 - highbit(gmask);
    bshift = 7 - highbit(bmask);

    maplen = theVisual->map_entries;
    if (maplen>256) maplen=256;
    cshift = 7 - highbit((u_long) (maplen-1));

    pxim = XCreateImage(pxctx->display, theVisual,
        DisplayPlanes(pxctx->display, pxctx->screen_num), ZPixmap, 0, NULL,
		            info->iWidth,  info->iHeight, 32, 0);
    if (!pxim)
	{
		if(pwebxim->colormap)
		{
			WEBC_FREE(pwebxim->colormap);
		}
		WEBC_FREE(pwebxim);
        return(0);
	}

    bperline = pxim->bytes_per_line;
    bperpix  = pxim->bits_per_pixel;
    border   = pxim->byte_order;

    imagedata = (byte *) WEBC_MALLOC((size_t) (info->iHeight * bperline));
    if (!imagedata)
    {
        XLIB_FreeXImage (pxim);
		if(pwebxim->colormap)
		{
			WEBC_FREE(pwebxim->colormap);
		}
		WEBC_FREE(pwebxim);
        return(0);
    }

    pxim->data = (char *) imagedata;

    if (bperpix != 8 && bperpix != 16 && bperpix != 24 && bperpix != 32)
    {
        XLIB_FreeXImage (pxim);
		if(pwebxim->colormap)
		{
			WEBC_FREE(pwebxim->colormap);
		}
		WEBC_FREE(pwebxim);
        return(0);
    }
    lpp = (WEBC_UINT8*)info->pData;
    lip = imagedata;

    for (i=0; i<info->iHeight; i++, lip+=bperline, lpp+=info->iBytesPerRow)
    {
        for (j=0,pp = lpp, ip=lip; j<info->iWidth; j++)
        {
            if (bperpix_input == 3)
            {
				r = *pp++;  g = *pp++;  b = *pp++;
            }
            else
            {
				// r = *pp;  g = *pp;  b = *pp++; // HACK
				WEBC_UINT8 *pcm;
				pcm = bcolormap + (*pp * 4);
				r = *pcm++;
				g = *pcm++;
				b = *pcm;
				pp++;
	        }

            /* shift r,g,b so that high bit of 8-bit color specification is
             * aligned with high bit of r,g,b-mask in visual,
             * AND each component with its mask,
             * and OR the three components together
             */

            if (theVisual->c_class == DirectColor)
            {
                r = (u_long) directConv[(r>>cshift) & 0xff] << cshift;
                g = (u_long) directConv[(g>>cshift) & 0xff] << cshift;
                b = (u_long) directConv[(b>>cshift) & 0xff] << cshift;
            }
            /* shift the bits around */
            if (rshift<0) r = r << (-rshift);
                     else r = r >> rshift;
            if (gshift<0) g = g << (-gshift);
                     else g = g >> gshift;
            if (bshift<0) b = b << (-bshift);
                     else b = b >> bshift;
            r = r & rmask;
            g = g & gmask;
            b = b & bmask;

            xcol = r | g | b;
//xcol |= 0xff000000;
            if (bperpix == 32)
            {
                if (border == MSBFirst)
                {
                    *ip++ = (xcol>>24) & 0xff;
                    *ip++ = (xcol>>16) & 0xff;
                    *ip++ = (xcol>>8)  & 0xff;
                    *ip++ =  xcol      & 0xff;
                }
                else
                {  /* LSBFirst */
                    *ip++ =  xcol      & 0xff;
                    *ip++ = (xcol>>8)  & 0xff;
                    *ip++ = (xcol>>16) & 0xff;
                    *ip++ = (xcol>>24) & 0xff;
                }
            }
            else if (bperpix == 24)
            {
                if (border == MSBFirst)
                {
                    *ip++ = (xcol>>16) & 0xff;
                    *ip++ = (xcol>>8)  & 0xff;
                    *ip++ =  xcol      & 0xff;
                }
                else
                {  /* LSBFirst */
                    *ip++ =  xcol      & 0xff;
                    *ip++ = (xcol>>8)  & 0xff;
                    *ip++ = (xcol>>16) & 0xff;
                }
            }
            else if (bperpix == 16)
            {
                if (border == MSBFirst)
                {
                    *ip++ = (xcol>>8)  & 0xff;
                    *ip++ =  xcol      & 0xff;
                }
                else
                {  /* LSBFirst */
                    *ip++ =  xcol      & 0xff;
                    *ip++ = (xcol>>8)  & 0xff;
                }
            }
            else if (bperpix == 8)
            {
              *ip++ =  xcol      & 0xff;
            }
        }
    }
	pwebxim->pxim = pxim;
    return pwebxim;
}




/*---------------------------------------------------------------------------*/
static int _XLIB_CreateTransparentBitmap (
		HTMLGraphicsContext** ctx,
		const WebGraphBitmapInfo* info,
		struct webc_ximage *pwebxim
	)
{
    unsigned long r, g, b, rmask, gmask, bmask, xcol;
    int           bperpix_input;
	WebColor     index_val;
    int           row, column;
    WEBC_UINT8    *lpp, *pp;
	struct raster_draw *prasterop,*temppxraster;
	int num_free;
	int raster_count;
	WebColor transparent_val;

	pwebxim->pxim = 0;
    pwebxim->raster_list_size = 0;
    pwebxim->pxraster = 0;
	pwebxim->pxim = 0;
// put above
    if (info->bHasTransparentColor)
	{
		if (info->pixelFormat != WB_PIXEL_FORMAT_INDEX_8)
		{
			printf("Can not make transparent from type %d \n",info->pixelFormat);
			return(0);
		}
	}

	pwebxim->pixelFormat  = info->pixelFormat;

	if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
        bperpix_input = 3;
	else if (info->pixelFormat == WB_PIXEL_FORMAT_INDEX_8)
        bperpix_input = 1;
	else if (info->pixelFormat == WB_PIXEL_FORMAT_MONOCHROME_8)
        bperpix_input = 1;


	pwebxim->pxim = 0;
    pwebxim->raster_list_size = 0;
    pwebxim->pxraster = 0;

  	if(info->pixelFormat == WB_PIXEL_FORMAT_INDEX_8 || info->pixelFormat == WB_PIXEL_FORMAT_MONOCHROME_8)
	{
		int i,j;
		pwebxim->colormap = (WebColor*)WEBC_MALLOC (info->iPaletteSize * sizeof (WebColor));
		for (i = 0; i < info->iPaletteSize; i++)
		{
			r = info->pPalette[i + (i<<1) + 0];
			g = info->pPalette[i + (i<<1) + 1];
			b = info->pPalette[i + (i<<1) + 2];
			pwebxim->colormap[i] = XLIB_RGBToColor(ctx, RGB(r, g, b), 0);
		}
		transparent_val = (WebColor) info->iTransparentIndex;

	}
	else if (info->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
	{

		transparent_val = (WebColor) XLIB_RGBToColor(ctx, RGB(info->uTransparentRed, info->uTransparentGreen, info->uTransparentBlue), 0);
	}


    /************************************************************************/
    /* Non-ColorMapped Visuals:  TrueColor, DirectColor                     */
    /************************************************************************/

    lpp = (WEBC_UINT8*)info->pData;
	/* Scan each row, create draw list from non transparent data */
	raster_count = 0;
	num_free = info->iHeight*info->iWidth;

	temppxraster = (struct raster_draw *) WEBC_MALLOC(sizeof(struct raster_draw) * (info->iHeight*info->iWidth)); // Allocate an excess
	prasterop = temppxraster;

	if (!prasterop)
		return(0);


	prasterop->length = 0;
	prasterop->color = transparent_val;

    pwebxim->width_if_raster =  info->iWidth;
    pwebxim->height_if_raster = info->iHeight;

    for (row=0; row<info->iHeight; row++, lpp+=info->iBytesPerRow)
    {
        for (column=0,pp = lpp; column<info->iWidth; column++)
        {
           if (bperpix_input == 3)
           { /* Not done */
                r = *pp++;  g = *pp++;  b = *pp++;
				index_val = (WebColor)XLIB_RGBToColor(ctx, RGB(r,g,b), 0);
            }
            else
				index_val = (WebColor)*pp++;


			if (index_val == transparent_val && prasterop->color == transparent_val)
			{
				/* just skip processing, we are in a not draw state */
			}
			else
			{
				if (index_val == prasterop->color)
				{
					prasterop->length += 1;
				}
				else
				{
					if (prasterop->length)
					{
						if (--num_free == 0)
							goto too_many_rasters;
						raster_count += 1;
						prasterop += 1;
						prasterop->length = 0;
					}
					prasterop->color = index_val;
					if (index_val != transparent_val)
					{
						prasterop->y = row;
						prasterop->x = column;
						prasterop->length = 1;
					}
				}
			}
        }
		/* Column done, if any queued up requests, adavance */
		if (prasterop->length)
		{
			if (--num_free == 0)
				goto too_many_rasters;
			raster_count += 1;
			prasterop += 1;
			prasterop->length = 0;
		}
		prasterop->color = transparent_val;

	}
    pwebxim->raster_list_size = raster_count;


too_many_rasters:

    if (!pwebxim->raster_list_size)
    {

		WEBC_FREE(temppxraster);
		pwebxim->pxraster = 0;
    	return(0);
    }
	else
	{
		pwebxim->pxraster = (struct raster_draw *) WEBC_MALLOC(sizeof(struct raster_draw) * raster_count);
		rtp_memcpy(pwebxim->pxraster, temppxraster, sizeof(struct raster_draw) * raster_count);
		WEBC_FREE(temppxraster);
   		return(1);
	}
}
/*---------------------------------------------------------------------------*/
static void XLIB_DestroyWebBitmap (
		HTMLGraphicsContext** ctx,
		void* b
	)
{
    struct webc_ximage *pwebxim;
    XImage *pimage;

	pwebxim = (struct webc_ximage *) b;

    pimage = pwebxim->pxim;
	if (pimage)
    	XLIB_FreeXImage(pimage);
	if (pwebxim->pxraster)
	{
		WEBC_FREE(pwebxim->pxraster);
	}
	if(pwebxim->colormap)
	{
		WEBC_FREE(pwebxim->colormap);
	}
	WEBC_FREE(pwebxim);
}


/*---------------------------------------------------------------------------*/
static void XLIB_ClaimPointer (
		HTMLGraphicsContext** ctx
	)
{
WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
    XGrabPointer(pxctx->display, pxctx->windowHandle,
                True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask
                , GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
}

/*---------------------------------------------------------------------------*/
static void XLIB_ReleasePointer (
		HTMLGraphicsContext** ctx
	)
{
    WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
    XUngrabPointer(pxctx->display, CurrentTime);
}


/*---------------------------------------------------------------------------*/
static WebColor XLIB_GetColorByIndex (
		HTMLGraphicsContext** ctx,
		int i
	)
{
//HTML_RGBAToColor
	static HTMLColor colorMap [] =
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
    return(XLIB_RGBToColor (ctx, colorMap[i], 0));
}


#ifdef WEBC_BUFFER_SCROLLING
/* See below */
HTMLGraphicsContext**  XLIB_CreateBuffer (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT w,
		DISPLAY_INT h);

void XLIB_DestroyBuffer (
		HTMLGraphicsContext** ctx,
		HTMLGraphicsContext** buffer);

HTMLGraphicsContext**  XLIB_ResizeBuffer (
		HTMLGraphicsContext** ctx,
		HTMLGraphicsContext** buffer,
		DISPLAY_INT w,
		DISPLAY_INT h);

void  XLIB_CopyBufferRegion (
		HTMLGraphicsContext** dstCtx,
		HTMLRect* dstRect,
		HTMLGraphicsContext** srcBuffer,
		DISPLAY_INT srcX,
		DISPLAY_INT srcY);
#endif

/*---------------------------------------------------------------------------*/
/* Helper thread to generate and send timer events to WebC */
void *GenerateTime(void *userData)
{
	WebcXLIBGraphics *ctx = (WebcXLIBGraphics *)userData;
	XClientMessageEvent xclient;
	memset(&xclient,0,sizeof(xclient));
	xclient.type = ClientMessage;
	xclient.display = ctx->display;
	xclient.window = ctx->windowHandle;
	xclient.format = 32;

	while(1)
	{
		usleep(1000); /* 1 milliseconds */
		/*
		XSendEvent(
		      ctx->display,
		      ctx->windowHandle,
		      0,
		      //SubstructureRedirectMask,
			  0,
		      (XEvent *)&xclient
			);
		*/
	}
}

Display *g_pDisplay=0;

/*---------------------------------------------------------------------------*/
WEBC_BOOL xlib_create_window (WebcXLIBGraphics *ctx, HTMLBrowserConfig* config)
{
    /*  Window variables  */
    int          x, y;
    unsigned int width, height;
    unsigned int border_width;
    /*  Display variables  */
    char *       display_name = NULL;
    unsigned int display_width, display_height;
    /*  Miscellaneous X variables  */
    XTextProperty windowName, iconName;
    XGCValues     values;

  //  if(!size_hints)
    {
	    /*  Allocate memory for our structures  */
	    if ( !( size_hints  = XAllocSizeHints() ) ||
		 !( wm_hints    = XAllocWMHints()   ) ||
		 !( class_hints = XAllocClassHint() )    )
	    {
	    	printf("%s: couldn't allocate memory.\n", appname);
		    return(WEBC_FALSE);
	    }
    }

    if(!g_pDisplay) // use the same display for everyone
    {    	
	    /*  Connect to X server  */
	    if ( (g_pDisplay = XOpenDisplay(display_name)) == NULL )
	    {
	    	printf("%s: couldn't connect to X server\n", appname);
		    return(WEBC_FALSE);
	    }
    }
	ctx->display = g_pDisplay;
   	    
    /*  Get screen size from display structure macro  */
    ctx->screen_num = DefaultScreen(ctx->display);
    display_width  = DisplayWidth(ctx->display, ctx->screen_num);
    display_height = DisplayHeight(ctx->display, ctx->screen_num);

    /*  Set initial window size and position, and create it  */
    if(config->width && config->height)
    {
	    x = config->left;
	    y = config->top;
	    width  = config->width;
	    height = config->height;    	
    }
    else
    {
	    x = y = 0;
	    width  = display_width;
	    height = display_height;
    }

    ctx->xlib_width = display_width;
	ctx->xlib_height = display_height;
    
    ctx->windowHandle = XCreateSimpleWindow(ctx->display, RootWindow(ctx->display, ctx->screen_num),
			      x, y, width, height, border_width,
			      WhitePixel(ctx->display, ctx->screen_num),
			      BlackPixel(ctx->display, ctx->screen_num));
    
#if (0)
	{
		unsigned long myvaluemask;
		XSetWindowAttributes myattributes;

		myvaluemask = CWBackingStore|CWBackingPlanes|CWBackingPixel|CWSaveUnder;

		myattributes.backing_store = Always;
		myattributes.backing_planes = 0xffffffff;
		myattributes.backing_pixel = 0;
		myattributes.save_under = 1;

		XChangeWindowAttributes(ctx->display, ctx->windowHandle, myvaluemask, &myattributes);

		XClearWindow(ctx->display, ctx->windowHandle);
	}
#endif

    /*  Set hints for window manager before mapping window  */
    if (XStringListToTextProperty (&window_name, 1, &windowName) == 0 )
    {
    	printf("%s: structure allocation for windowName failed.\n",
		        appname);
	    return(WEBC_FALSE);
    }

    if (XStringListToTextProperty (&icon_name, 1, &iconName) == 0 )
    {
    	printf( "%s: structure allocation for iconName failed.\n",
		        appname);
	    return(WEBC_FALSE);
    }

    size_hints->flags       = PPosition | PSize | PMinSize;
    size_hints->min_width   = 200;
    size_hints->min_height  = 100;

    wm_hints->flags         = StateHint | InputHint;
    wm_hints->initial_state = NormalState;
    wm_hints->input         = True;

    class_hints->res_name   = appname;
    class_hints->res_class  = "webcx";

    XSetWMProperties(ctx->display, ctx->windowHandle, &windowName, &iconName, 0, 0,
		     size_hints, wm_hints, class_hints);

    /*  Choose which events we want to handle  */
    XSelectInput(ctx->display, ctx->windowHandle,
        ExposureMask |
        KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
        PointerMotionMask | StructureNotifyMask);

	if ( (ctx->xlib_font_info = XLoadQueryFont(ctx->display, DEFAULT_FONT)) == NULL )
    {
    	return(WEBC_FALSE);
    }

    /*  Create graphics context  */
    ctx->gc = XCreateGC(ctx->display, ctx->windowHandle, 0, &values);

    //XSetFont(ctx->display, ctx->gc, ctx->xlib_font_info->fid); //Vikram .. no need
    XSetForeground(ctx->display, ctx->gc, BlackPixel(ctx->display, ctx->screen_num));

    /*  Display Window  */
    XMapWindow(ctx->display, ctx->windowHandle);

	/* start a helper thread to generate and send timer events to WebC Xlib Client */
   	//pthread_t timerThread;
   	//int rc;
	//rc = pthread_create(&timerThread, NULL, GenerateTime, ctx);
    //if (rc)
	//{
         //return(-1);
    //}
	return (0);
}

/*---------------------------------------------------------------------------*/
int webc_InitXLIBBrowserConfig (HTMLBrowserConfig* config, WEBC_BOOL bCreateWindow)
{
	webcXLIBGraphicsClass.WindowOpen                 = XLIB_WindowOpen;
	webcXLIBGraphicsClass.WindowClose                = XLIB_WindowClose;
	webcXLIBGraphicsClass.MoveTo						= XLIB_MoveByorTo;
	webcXLIBGraphicsClass.ScreenInfo					= XLIB_ScreenInfo;

	webcXLIBGraphicsClass.getCanvasRect              = XLIB_GetCanvasRect;

	webcXLIBGraphicsClass.claimPointer               = XLIB_ClaimPointer;
	webcXLIBGraphicsClass.releasePointer             = XLIB_ReleasePointer;

	webcXLIBGraphicsClass.invalidateRect             = XLIB_InvalidateRect;
	webcXLIBGraphicsClass.beginDrawing               = XLIB_BeginDrawing;
	webcXLIBGraphicsClass.getClipRect                = XLIB_GetClipRect;
	webcXLIBGraphicsClass.setClipRect                = XLIB_SetClipRect;
	webcXLIBGraphicsClass.endDrawing                 = XLIB_EndDrawing;

	webcXLIBGraphicsClass.colorToRGBA                = XLIB_ColorToRGB;
	webcXLIBGraphicsClass.RGBAToColor                = XLIB_RGBToColor;
	webcXLIBGraphicsClass.getColorByIndex            = XLIB_GetColorByIndex;

	webcXLIBGraphicsClass.drawText                   = XLIB_DrawText;
	webcXLIBGraphicsClass.drawTextLen                = XLIB_DrawTextLen;
	webcXLIBGraphicsClass.drawBitmap                 = XLIB_DrawBitmap;
	webcXLIBGraphicsClass.drawBitmapStretchedToRect  = XLIB_DrawStretchedBitmap;
	webcXLIBGraphicsClass.drawRectangle              = XLIB_DrawRectangle;

	webcXLIBGraphicsClass.createFont                 = XLIB_CreateFont;
	webcXLIBGraphicsClass.getDefaultFont             = XLIB_GetDefaultFont;
	webcXLIBGraphicsClass.destroyFont                = XLIB_DestroyFont;
	webcXLIBGraphicsClass.getNumFontFamilies         = XLIB_NumFontFamilies;
	webcXLIBGraphicsClass.getFontFamilyNames         = XLIB_GetFontFamilyNames;
	webcXLIBGraphicsClass.getFontFamilyGenericNames  = XLIB_GetFontFamilyGenericNames;
	webcXLIBGraphicsClass.getTextHeight              = XLIB_GetTextHeight;
	webcXLIBGraphicsClass.getTextWidth               = XLIB_GetTextWidth;
	webcXLIBGraphicsClass.getTextHeightLen           = XLIB_GetTextHeightLen;
	webcXLIBGraphicsClass.getTextWidthLen            = XLIB_GetTextWidthLen;
	webcXLIBGraphicsClass.getFontHeight              = XLIB_GetFontHeight;
	webcXLIBGraphicsClass.getFontBaseline            = XLIB_GetFontBaseline;

	webcXLIBGraphicsClass.createWebBitmap            = XLIB_CreateWebBitmap;
	webcXLIBGraphicsClass.destroyWebBitmap           = XLIB_DestroyWebBitmap;
	webcXLIBGraphicsClass.createStretchedBitmap      = XLIB_CreateStretchedBitmap;
	webcXLIBGraphicsClass.destroyStretchedBitmap     = XLIB_DestroyWebBitmap;
	webcXLIBGraphicsClass.getBitmapWidth             = XLIB_GetBitmapWidth;
	webcXLIBGraphicsClass.getBitmapHeight            = XLIB_GetBitmapHeight;

	webcXLIBGraphicsClass.optionalDrawHorizontalLine = 0;
	webcXLIBGraphicsClass.optionalDrawVerticalLine   = 0;
//	webcXLIBGraphicsClass.optionalDrawStyledFrame    = XLIB_DrawPatternFrame;
//	webcXLIBGraphicsClass.optionalDrawBitmapTiled    = XLIB_TileBitmap;
	webcXLIBGraphicsClass.optionalSetCanvasRect      = 0;

  #ifdef WEBC_BUFFER_SCROLLING
	webcXLIBGraphicsClass.createBuffer               = XLIB_CreateBuffer;
	webcXLIBGraphicsClass.resizeBuffer  	            = XLIB_ResizeBuffer;
	webcXLIBGraphicsClass.destroyBuffer              = XLIB_DestroyBuffer;
	webcXLIBGraphicsClass.copyBufferRegion	  	    = XLIB_CopyBufferRegion;
  #endif // WEBC_BUFFER_SCROLLING

	WebcXLIBGraphics* ctx = (WebcXLIBGraphics*) WEBC_MALLOC(sizeof(WebcXLIBGraphics));
	if (ctx)
	{
		rtp_memset(ctx, 0, sizeof(WebcXLIBGraphics));
		ctx->ctx = &webcXLIBGraphicsClass;
		if(bCreateWindow)
			xlib_create_window (ctx, config);
		config->graphics = (HTMLGraphicsContext**) ctx;
		config->processInputQueue = &xlibProcessInputQueueFn;
		return (0);
	}

	return (-1);

}

/*---------------------------------------------------------------------------*/
void webc_DestroyXLIBBrowserConfig (HTMLBrowserConfig* config)
{
//PETERPETER
	WebcXLIBGraphics* ctx = (WebcXLIBGraphics*) config->graphics;
#if (DONE)
	delete ctx->windowGraphics;
#endif
	WEBC_FREE(ctx);
}

/*---------------------------------------------------------------------------*/
void   webc_DestroyXLIBBrowser       (HBROWSER_HANDLE hbrowser)
{
//PETERPETER
	HTMLBrowserConfig config;

	if (webc_BrowserGetConfig(hbrowser, &config) >= 0)
	{
		webc_DestroyBrowser(hbrowser, 0);
		webc_DestroyXLIBBrowserConfig(&config);
	}
}

/*---------------------------------------------------------------------------*/
WebKey webc_TranslateXLIBKey         (WEBC_UINT32 rawKey)
{
	if (!xlibKeyMapInitialized)
	{
		webc_CreateKeyMap(&xlibKeyMap, xlibKeyData, WG_NUM_XLIB_KEYS);
		xlibKeyMapInitialized = WEBC_TRUE;
	}

	return webc_TranslateKey(&xlibKeyMap, rawKey);
}



#ifdef WEBC_BUFFER_SCROLLING
/*---------------------------------------------------------------------------*/
HTMLGraphicsContext**  XLIB_CreateBuffer (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT w,
		DISPLAY_INT h)
{
	WebcXLIBGraphics* g = (WebcXLIBGraphics*) WEBC_MALLOC(sizeof(WebcXLIBGraphics));
	if (g)
	{
		rtp_memset(g, 0, sizeof(WebcXLIBGraphics));

		g->ctx = &webcXLIBGraphicsClass;
		g->windowHandle   = ((WebcXLIBGraphics*)ctx)->windowHandle;
		g->outputGraphics = ((WebcXLIBGraphics*)ctx)->windowGraphics;
		g->blendLevelInt  = 255;
		g->blendLevelReal = 1.0;

		g->bufferImage = new Bitmap(w, h, ((WebcXLIBGraphics*)ctx)->windowGraphics);
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
void XLIB_DestroyBuffer (
		HTMLGraphicsContext** ctx,
		HTMLGraphicsContext** buffer)
{
	WebcXLIBGraphics* g = (WebcXLIBGraphics*) buffer;

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
HTMLGraphicsContext**  XLIB_ResizeBuffer (
		HTMLGraphicsContext** ctx,
		HTMLGraphicsContext** buffer,
		DISPLAY_INT w,
		DISPLAY_INT h)
{
	HTMLGraphicsContext** dst = XLIB_CreateBuffer(ctx, w, h);
	if (dst)
	{
		// if successful, destroy the old buffer
		XLIB_DestroyBuffer (ctx, buffer);
	}
	return dst;
}

/*---------------------------------------------------------------------------*/
void  XLIB_CopyBufferRegion (
		HTMLGraphicsContext** dstCtx,
		HTMLRect* dstRect,
		HTMLGraphicsContext** srcBuffer,
		DISPLAY_INT srcX,
		DISPLAY_INT srcY)
{
	WebcXLIBGraphics* dst = (WebcXLIBGraphics*) dstCtx;
	WebcXLIBGraphics* src = (WebcXLIBGraphics*) srcBuffer;

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
#define MAX_X_BROWSERS 20
static HBROWSER_HANDLE	global_browser[MAX_X_BROWSERS];
static Window	global_window[MAX_X_BROWSERS];
static int g_numBindings=0;


static HBROWSER_HANDLE xlib_window_to_webc_browser(Window win)
{
	int i;
	for (i = 0; i < g_numBindings; i++)
	{
		if (win == global_window[i])
			return(global_browser[i]);
	}
	return global_browser[0];
}

void xlib_bind_window_to_browser(HTMLBrowserConfig *config, HBROWSER_HANDLE browser)
{
	int i;
	WebcXLIBGraphics* ctx = (WebcXLIBGraphics*)config->graphics;
#if 0
	for (i = 0; i < MAX_X_BROWSERS; i++)
	{
		if (ctx->windowHandle != global_window[i])
		{

			global_window[i] = ctx->windowHandle ;
			global_browser[i] = browser;
		}
	}
#else
	global_window[g_numBindings] = ctx->windowHandle;
	global_browser[g_numBindings] = browser;
	g_numBindings++;
#endif
}

static HTMLEventFlags xlib_keyboard_state;

static void set_keyboard_state(WebKey key, HTMLEventType eventtype);

static int XLIB_ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	)
{
// NOT DONE

}


Bool draw_predicate( Display *display, XEvent *event, XPointer arg)
{
	if(event->type == Expose)
	{
		if(event->xexpose.window == (* (Window *)arg))
			return true;
	}
	return false;
}


WEBC_BOOL NoPendingDrawsInQueue(Display *display, Window window)
{
	XEvent event_return;
	return XCheckIfEvent(display, &event_return, draw_predicate, (XPointer)&window) ? WEBC_FALSE : WEBC_TRUE;	
}

Bool mmove_predicate( Display *display, XEvent *event, XPointer arg)
{
	if(event->type == MotionNotify)
	{
		if(event->xexpose.window == (* (Window *)arg))
			return true;
	}
	return false;
}


WEBC_BOOL NoPendingMotionNotifysInQueue(Display *display, Window window)
{
	XEvent event_return;
	return XCheckIfEvent(display, &event_return, mmove_predicate, (XPointer)&window) ? WEBC_FALSE : WEBC_TRUE;	
}

     
/*  Enter event loop  */
void xlib_dispatch_event(WebcXLIBGraphics *ctx, XEvent *preport)
{
	HBROWSER_HANDLE browser;
	HTMLEvent e;
	e.type = HTML_EVENT_NONE;
	e.flags = 0;
	WEBC_BOOL bDraw = WEBC_TRUE;

	browser = xlib_window_to_webc_browser(preport->xany.window);
    if (!browser)
        return;



	switch ( preport->type ) {

	case Expose:
    {
		char * message = "Hello, X Window System!";
		int    length;
		int    font_height;
		int    msg_x, msg_y;


		if ( preport->xexpose.count != 0 )
			break;
	    /*  Output message centrally in window  */
	    if (browser)
	    {
	    	// make sure there are no other expose events in the queue for this window.
	    	// because we're drawing the entire window every time, we don't want to do this
	    	// over-and-over again to avoid flashing. Once is enough.
	    	if(NoPendingDrawsInQueue(ctx->display, preport->xexpose.window))
	    	{
	    		webc_BrowserInvalidate(browser);
	    		webc_BrowserDraw(browser);
	    	}
		}
	    length = XTextWidth(ctx->xlib_font_info, message, strlen(message));
	    msg_x  = (ctx->xlib_width - length) / 2;

		font_height = ctx->xlib_font_info->max_bounds.ascent + ctx->xlib_font_info->max_bounds.descent;
	    msg_y  = (ctx->xlib_height + font_height) / 2;

	    //Note: msg_x should be 0; the curr value does not make any difference
		XDrawString(ctx->display, ctx->windowHandle, ctx->gc, msg_x, msg_y, message, strlen(message));
	    break;
	}
	case ConfigureNotify:
	    /*  Store new window width & height  */
	    //ctx->xlib_width  = preport->xconfigure.width;
	    //ctx->xlib_height = preport->xconfigure.height;
	    break;
	case ButtonPress:            /*  Fall through  */
	case ButtonRelease:            /*  Fall through  */
    {
        e.data.position.x = preport->xbutton.x;
		e.data.position.y = preport->xbutton.y;
        if (preport->type == ButtonPress)
            e.type = HTML_EVENT_MOUSEDOWN;
        else
            e.type = HTML_EVENT_MOUSEUP;
	    break;
	}
    case MotionNotify:
        e.data.position.x = preport->xbutton.x;
		e.data.position.y = preport->xbutton.y;
        e.type = HTML_EVENT_MOUSEMOVE;
        if(!NoPendingMotionNotifysInQueue(ctx->display, preport->xbutton.window))
        	bDraw = WEBC_FALSE;
	    break;
	case KeyPress:
    case KeyRelease:
    {
        KeySym keysym;
        XKeyEvent *pKeyEvent;
        int length;
        char string[10];
		XComposeStatus compose_status;

        pKeyEvent = (XKeyEvent *) &preport->xkey;
        length = XLookupString(pKeyEvent,string,9,
                    &keysym, &compose_status);
        if((length > 0) && (length <=9))
        {
            string[length]='\0';
            printf("result of xlookupstring [%s] ", string);
        }
        printf("keysym [%s] \n", XKeysymToString(keysym));
	    if (preport->type == KeyPress)
			e.type = HTML_EVENT_KEYDOWN;
        else
			e.type = HTML_EVENT_KEYUP;

		e.data.key = webc_TranslateXLIBKey ((WEBC_UINT32) keysym);
		set_keyboard_state(e.data.key, e.type);

	    break;
	}
    }

	if (browser && (e.type != HTML_EVENT_NONE))
	{
        //e.flags = xlib_key_flags;
		webc_BrowserSuspendDraw(browser);
		webc_BrowserDispatchEvent(browser, &e);
		webc_BrowserResumeDraw(browser);
		if(bDraw)
			webc_BrowserDraw(browser);
		webc_BrowserProcessLoadQueueAll(browser);
		if(bDraw)
			webc_BrowserDraw(browser);
	}
}

void xlib_execute_webc(HTMLGraphicsContext** ctx,
					   HBROWSER_HANDLE gbrowser)
{
    WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
	XEvent report;
    while (1)
    {
	    XNextEvent(pxctx->display, &report);
	    xlib_dispatch_event(pxctx, &report);
    }
}

static void set_keyboard_state(WebKey key, HTMLEventType eventtype)
{
HTMLEventFlags mask;

    if (key == WGK_ALT)
        mask = HTML_EVENT_FLAG_ALT_DOWN;
    else if (key == WGK_CTRL)
        mask = HTML_EVENT_FLAG_CTRL_DOWN;
    else if (key == WGK_SHIFT)
        mask = HTML_EVENT_FLAG_SHIFT_DOWN;
    else
        return;

    if (eventtype == HTML_EVENT_KEYUP)
        xlib_keyboard_state != ~mask;
    else
         xlib_keyboard_state |= mask;
}
