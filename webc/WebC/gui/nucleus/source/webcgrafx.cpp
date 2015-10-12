/*
|  WEBCGRAFX.C -
|
|  EBS -
|
|   $Author: vmalaiya $
|   $Date: 2006/08/31 22:34:35 $
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

#include "webcgrafx.h"
#include "wfgrafx.h"
#include "webcmem.h"
#include "winlib.h"
#include "keyboard.h"
#include "webrect.hpp"
#include "webimage.hpp"
#include "24togs.hpp"
#include "8togs.hpp"
#include "gs8togs.hpp"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

// set this to the number of palette indices
#define WG_NUM_COLORS           4

// define this symbol to speed up RGB to palette conversions
#define WG_GRAYSCALE_PALETTE

// define this symbol if white is 0
#define WG_GRAYSCALE_INVERTED

#define WCG_OBJ(X)              ((WebcNucleusGraphics*)(X))
#define B_PTR(X)                ((WebcNucleusBitmap*)(X))
#define F_PTR(X)                ((WebcNucleusFont*)(X))

#define B_IMG(X)                (B_PTR(X)->img)
#define B_WIDTH(X)              ((imageHeader*)B_IMG(X))->imWidth
#define B_HEIGHT(X)             ((imageHeader*)B_IMG(X))->imHeight

#define F_FONT(X)               (F_PTR(X)->fnt)
#define F_FACE(X)               (F_PTR(X)->face)

/*****************************************************************************/
/* Types
 *****************************************************************************/

typedef struct
{
	HTMLGraphicsContext* ctx;
	grafPort*            devicePort;
	grafPort*            bufferPort;
	WebRect              clipRect;
	WebRect              invalidRect;
	int                  bufferDepth;
	palData              paletteData[WG_NUM_COLORS];	
}
WebcNucleusGraphics;

typedef struct
{
	image    * img;
	region   * clip;
	WebColor   transColor;
}
WebcNucleusBitmap;


/*****************************************************************************/
/* Function Prototypes
/*****************************************************************************/

static int GRAFX_ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	);

/*****************************************************************************/
/* Data
 *****************************************************************************/

HTMLGraphicsContext webcNucleusGraphicsClass;

int (*grafxProcessInputQueueFn) (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode) = GRAFX_ProcessInputQueue;

#define WG_NUM_GRAFX_KEYS 32

static WEBC_UINT32 nucleusKeyData [WG_NUM_GRAFX_KEYS*2] =
{
	WGK_TAB         , TAB		    ,
	WGK_CR          , ENTER         ,
	WGK_PGUP        , PGUP          ,
	WGK_PGDN        , PGDN          ,
	WGK_LNUP        , UP_ARROW      ,
	WGK_LNDN        , DOWN_ARROW    ,
	WGK_ALT         , 0             ,
	WGK_CTRL        , 1             ,
	WGK_SHIFT       , 2             ,
	WGK_CAPS        , 3             ,
	WGK_NUMLOCK     , 4             ,
	WGK_SCROLLLOCK  , 5             ,
	WGK_F1          , F1            ,
	WGK_F2          , F2            ,
	WGK_F3          , F3            ,
	WGK_F4          , F4            ,
	WGK_F5          , F5            ,
	WGK_F6          , F6            ,
	WGK_F7          , F7            ,
	WGK_F8          , F8            ,
	WGK_F9          , F9            ,
	WGK_F10         , F10           ,
	WGK_F11         , 6             ,
	WGK_F12         , 7			    ,
	WGK_INSERT      , INSERT        ,
	WGK_DELETE      , DELETE        ,
	WGK_BACKSPACE   , BACKSPACE     ,
	WGK_LEFT        , LEFT_ARROW    ,
	WGK_RIGHT       , RIGHT_ARROW   ,
	WGK_HOME        , HOME          ,
	WGK_END         , END           ,
	WGK_ESC         , ESCAPE		
};

static WebcKeyMap nucleusKeyMap;
static WEBC_BOOL  nucleusKeyMapInitialized = WEBC_FALSE;

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
static void GRAFX_DrawRectangle (
		HTMLGraphicsContext** ctx,
		HTMLRect* r,
		WebColor outlineColor,
		WebColor fillColor,
		WEBC_BOOL fill
	)
{
	rect box;
	box.Xmin = r->left;
	box.Ymin = r->top;
	box.Xmax = r->right;
	box.Ymax = r->bottom;

	if (outlineColor == fillColor)
	{
		if (fill)
		{
			BackColor(fillColor);
			box.Xmax++;
			box.Ymax++;
			FillRect(&box,0);
		}
		else
		{
			PenColor(outlineColor);
			FrameRect(&box);
		}
	}
	else
	{
		PenColor(outlineColor);
		FrameRect(&box);
		if (fill)
		{
			box.Xmin++;
			box.Ymin++;
			BackColor(fillColor);
			FillRect(&box,0);
		}
	}	
}

/*---------------------------------------------------------------------------*/
static void GRAFX_DrawBitmap (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
		void* bmp
	)
{
	/* this has to do with transparent bitmaps
	if (bmp->clip)
	{
		// tbd - how to translate a clip region to arbitrary (x,y) ? 
		//ClipRegion(bmp->clip);
	}	
	*/
	
	if (bmp)
	{
		rect box;
		box.Xmin = x;
		box.Ymin = y;
		box.Xmax = x + B_WIDTH(bmp);
		box.Ymax = y + B_HEIGHT(bmp);
		
		WriteImage(&box, B_IMG(bmp));
	}

	/* see comment above
	if (bmp->clip)
	{
		//ClipRegion(NULL);
	}
	*/
}

/*---------------------------------------------------------------------------*/
static void* GRAFX_CreateStretchedBitmap (
		HTMLGraphicsContext** ctx,
		void* src,
		DISPLAY_INT w,
		DISPLAY_INT h
	)
{
	WebcNucleusBitmap* stretched = 0;
	
	grafPort* savePort;
	GetPort(&savePort);
	
	grafPort* srcPort = CreateBitmap( cMEMORY, B_WIDTH(src), B_HEIGHT(src) );
	if (srcPort)
	{
		grafPort* dstPort = CreateBitmap( cMEMORY, w, h );
		if (dstPort)
		{
			stretched = (WebcNucleusBitmap*) WEBC_MALLOC(sizeof(WebcNucleusBitmap));
			if (stretched)
			{
				tc_memset(stretched, 0, sizeof(s_WebBitmap));
				
				rect srcRect, dstRect;
				SetRect(&srcRect, 0, 0, B_WIDTH(src), B_HEIGHT(src));
				SetRect(&dstRect, 0, 0, w, h);
				
				SetPort(dstPort);
				long imageSize = ImageSize(&dstRect);
				stretched->img = (image*) WEBC_MALLOC(imageSize);
				if (stretched->img)
				{
					SetPort(srcPort);
					WriteImage(&srcRect, B_IMG(src));
					
					ZoomBlit(srcPort, dstPort, &srcRect, &dstRect);
					
					SetPort(dstPort);
					ReadImage(&dstRect, stretched->img);
					
					/* transparent images 
					if (src->clip)
					{
						SetPort(dstPort);
						stretched->clip = BitmapToRegion(&dstRect, src->transColor);
						stretched->transColor = src->transColor;
					}
					*/
				}
				else
				{
					WEBC_FREE(stretched);
					stretched = 0;
				}
			}				
			DestroyBitmap(dstPort);
		}
		DestroyBitmap(srcPort);
	}

	SetPort(savePort);

	return (stretched);
}

/*---------------------------------------------------------------------------*/
static void GRAFX_DestroyWebBitmap (
		HTMLGraphicsContext** ctx,
		void* _bmp
	)
{
	WebcNucleusBitmap* bmp = (WebcNucleusBitmap*) _bmp;
	WEBC_FREE(bmp->img);
	if (bmp->clip)
	{
		DestroyRegion(bmp->clip);
	}
	WEBC_FREE(bmp);	
}

/*---------------------------------------------------------------------------*/
static void GRAFX_DestroyStretchedBitmap(
		HTMLGraphicsContext** ctx,
		void* _bmp
	)
{
	GRAFX_DestroyWebBitmap(ctx, _bmp);
}

/*---------------------------------------------------------------------------*/
static void GRAFX_DrawStretchedBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect* r,
		void* b
	)
{
	DISPLAY_INT w = r->right - r->left + 1;
	DISPLAY_INT h = r->bottom - r->top + 1;
	
	if (w > 0 && h > 0)
	{
		void* stretched = GRAFX_CreateStretchedBitmap(ctx, b, w, h);
		if (stretched != WEBC_NULL)
		{
			GRAFX_DrawBitmap(ctx, r->left, r->top, stretched);
			GRAFX_DestroyStretchedBitmap(ctx, stretched);
		}
	}	
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GRAFX_GetBitmapWidth (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	return B_WIDTH(bmp);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GRAFX_GetBitmapHeight (
		HTMLGraphicsContext** ctx,
		void* bmp
	)
{
	return B_HEIGHT(bmp);
}

/*---------------------------------------------------------------------------*/
static void GRAFX_DrawText (
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
	PenColor(textColor);
	if (fill)
	{
		BackColor(backgroundColor);
	}
	else
	{
		RasterOp(xREPx);
	}
	
	SetFont(F_FONT(font));
	TextFace(F_FACE(font));
	TextAlign(alignLeft, alignTop);
	TextSpace(0);
	TextExtra(0);
	MoveTo(x,y);
	DrawString((char*) text);
	
	if (!fill)
	{
		RasterOp(zREPz);
	}	
}

/*---------------------------------------------------------------------------*/
static void GRAFX_DrawTextLen (
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
	PenColor(textColor);
	if (fill)
	{
		BackColor(fillColor);
	}
	else
	{
		RasterOp(xREPx);
	}
	
	SetFont(F_FONT(font));
	TextFace(F_FACE(font));
	TextAlign(alignLeft, alignTop);
	TextSpace(0);
	TextExtra(0);
	MoveTo(x,y);
	DrawText((char*) text, 0, textLen);
	
	if (!fill)
	{
		RasterOp(zREPz);
	}	
}

/*---------------------------------------------------------------------------*/
static void GRAFX_SetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* r
	)
{
	rect temp;

	WCG_OBJ(ctx)->clipRect.Set((WebRect*) r);	
	SetRect(&temp, r->left, r->top, r->right + 1, r->bottom + 1);
	ClipRect(&temp);	
}

/*---------------------------------------------------------------------------*/
static void GRAFX_GetClipRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* r
	)
{
	*r = *((HTMLRect*) &(WCG_OBJ(ctx)->clipRect));	
}

/*---------------------------------------------------------------------------*/
static void GRAFX_GetCanvasRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* r
	)
{
	r->left   = WCG_OBJ(ctx)->devicePort->portRect.Xmin;
	r->top    = WCG_OBJ(ctx)->devicePort->portRect.Ymin;
	r->right  = WCG_OBJ(ctx)->devicePort->portRect.Xmax - 2;
	r->bottom = WCG_OBJ(ctx)->devicePort->portRect.Ymax - 2;	
}


/*---------------------------------------------------------------------------*/
static WebColor GRAFX_RGBToColor (
		HTMLGraphicsContext** ctx,
		HTMLColor color,
		HTMLColor* error
	)
{
  #if 0
  #elif defined(WG_GRAYSCALE_PALETTE)
  
	#if (WG_NUM_COLORS == 4)
	
	  #ifdef WG_GRAYSCALE_INVERTED
	  
		return (3 - ((HTML_ColorGetRed(color) + HTML_ColorGetGreen(color) + HTML_ColorGetBlue(color)) / 192));
		
	  #else

		return ((HTML_ColorGetRed(color) + HTML_ColorGetGreen(color) + HTML_ColorGetBlue(color)) / 192);
	  
	  #endif 
	
	#endif

  #else
	return 0;
  #endif
}

/*---------------------------------------------------------------------------*/
static HTMLColor GRAFX_ColorToRGB (
		HTMLGraphicsContext** ctx,
		WebColor colorval
	)
{
	HTMLColor color;

#if   (WG_NUM_COLORS == (1<<24))

#elif (WG_NUM_COLORS == (1<<16))

	color = HTML_RGBAToColor(((colorval >> 8) & 0xf8), ((colorval >> 2) & 0xfc), ((colorval << 3) & 0xf8), 0);

#elif (WG_NUM_COLORS == (1<<15))

	color = HTML_RGBAToColor(((colorval >> 7) & 0xf8), ((colorval >> 2) & 0xf8), ((colorval << 3) & 0xf8), (colorval & 0x8000) ? 0xff : 0);

#else // 

	WEBC_UINT8 bColor = EBSCLIP(colorval, 0, WG_NUM_COLORS);

	color = HTML_RGBAToColor (
			(WCG_OBJ(ctx)->paletteData[bColor].palRed   >> 8), 
			(WCG_OBJ(ctx)->paletteData[bColor].palGreen >> 8),
			(WCG_OBJ(ctx)->paletteData[bColor].palBlue  >> 8),
			0);
		
#endif // 

	return (color);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GRAFX_GetTextHeight (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font
	)
{
	return (F_FONT(font)->chHeight);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GRAFX_GetTextHeightLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font,
		long textLen
	)
{
	return (F_FONT(font)->chHeight);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GRAFX_GetTextWidth (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font
	)
{
	SetFont(F_FONT(font));
	TextFace(F_FACE(font));
	DISPLAY_INT width = StringWidth((char *) text);
	
	return width;
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GRAFX_GetTextWidthLen (
		HTMLGraphicsContext** ctx,
		const WebChar* text,
		void* font,
		long size
	)
{
	SetFont(F_FONT(font));
	TextFace(F_FACE(font));
	DISPLAY_INT width = TextWidth((char *) text, 0, (int) size);

	return (width);
}

/*---------------------------------------------------------------------------*/
static DISPLAY_INT GRAFX_GetFontHeight (
		HTMLGraphicsContext** ctx,
		void* font
	)
{
	return (F_FONT(font)->chHeight);
}

/*---------------------------------------------------------------------------*/
static void GRAFX_InvalidateRect (
		HTMLGraphicsContext** ctx,
		HTMLRect* r
	)
{
	WCG_OBJ(ctx)->invalidRect.Or((WebRect*) r);	
}

/*---------------------------------------------------------------------------*/
static void GRAFX_BeginDrawing (HTMLGraphicsContext** ctx)
{
	if (!WCG_OBJ(ctx)->bufferDepth)
	{
		SetPort(WCG_OBJ(ctx)->bufferPort);
		MovePortTo(WCG_OBJ(ctx)->invalidRect.left, WCG_OBJ(ctx)->invalidRect.top);
		PortSize(WCG_OBJ(ctx)->invalidRect.Width(), WCG_OBJ(ctx)->invalidRect.Height());
	}
	
	WCG_OBJ(ctx)->bufferDepth++;	
}

/*---------------------------------------------------------------------------*/
static void GRAFX_EndDrawing (HTMLGraphicsContext** ctx)
{
	if (WCG_OBJ(ctx)->bufferDepth > 0)
	{
		if (WCG_OBJ(ctx)->bufferDepth == 1)
		{
			rect srcRect;
			rect dstRect;
			SetRect(&srcRect, WCG_OBJ(ctx)->invalidRect.left,      WCG_OBJ(ctx)->invalidRect.top, 
			                  WCG_OBJ(ctx)->invalidRect.right + 1, WCG_OBJ(ctx)->invalidRect.bottom + 1);
			SetRect(&dstRect, WCG_OBJ(ctx)->invalidRect.left+1,    WCG_OBJ(ctx)->invalidRect.top, 
			                  WCG_OBJ(ctx)->invalidRect.right + 2, WCG_OBJ(ctx)->invalidRect.bottom + 1);
			CopyBlit(WCG_OBJ(ctx)->bufferPort, WCG_OBJ(ctx)->devicePort, &srcRect, &dstRect);
			WCG_OBJ(ctx)->invalidRect.Set(0,0,-1,-1);
		}
		
		WCG_OBJ(ctx)->bufferDepth--;
	}
}

/*---------------------------------------------------------------------------*/
static void GRAFX_TileBitmap (
		HTMLGraphicsContext** ctx,
		HTMLRect* box,
		DISPLAY_INT xOffset,
		DISPLAY_INT yOffset,
		void* bmp
	)
{
	WebRect r = *((WebRect*) box);
	WebRect clip;
	GRAFX_GetClipRect(ctx, (HTMLRect*) &clip);
	r.And(&clip);
	GRAFX_SetClipRect(ctx, (HTMLRect*) &r);
	
	rect fillRect;

	xOffset = (xOffset - (r.left - box->left)) % B_WIDTH(bmp);
	yOffset = (yOffset - (r.top  - box->top))  % B_HEIGHT(bmp);

	SetRect(&fillRect, r.left + xOffset - B_WIDTH(bmp), 
	                   r.top  + yOffset - B_HEIGHT(bmp),
	                   r.right + 1, 
	                   r.bottom + 1);

	DefinePattern(0, (patRcd*) B_IMG(bmp));
	FillRect(&fillRect, 0);
	DefinePattern(0, 0);
	
	// restore saved clipping rectangle
	GRAFX_SetClipRect(ctx, (HTMLRect*) &clip);
}

/*---------------------------------------------------------------------------*/
static void GRAFX_DrawPatternFrame (
		HTMLGraphicsContext** ctx,
		HTMLRect* r,
		WebColor color,
		DISPLAY_INT thickness,
		CSSBorderStyle pattern
	)
{
	static Byte _dashedBits[] = {4,4};
	static dashRcd _dashedPattern = {2, 0, _dashedBits};
	
	static Byte _dottedBits[] = {1,1};
	static dashRcd _dottedPattern = {2, 0, _dottedBits};

	PenColor(color);

	switch (pattern)
	{
		case CSS_BORDER_STYLE_DASHED:
			DefineDash(1, &_dashedPattern);
			PenDash(1);
			break;
			
		case CSS_BORDER_STYLE_DOTTED:
			DefineDash(1, &_dottedPattern);
			PenDash(1);
			break;
	}
	
	MoveTo(r->left,  r->top);
	LineTo(r->right, r->top);
	LineTo(r->right, r->bottom);
	LineTo(r->left,  r->bottom);
	LineTo(r->left,  r->top);
	
	PenDash(0);
	DefineDash(1, 0);	
}

/*---------------------------------------------------------------------------*/
static void* GRAFX_CreateWebBitmap (
		HTMLGraphicsContext** ctx,
		const WebGraphBitmapInfo* info
	)
{
	WebcBitmapConverter* converter;
	WebcImageDitherMode ditherMode = WEBIMAGE_NO_DITHERING;
	imageHeader* imgHead = 0;
	
  #ifdef WG_GRAYSCALE_PALETTE

   #if (WG_NUM_COLORS == 4)
	
	ditherMode = WEBIMAGE_ERROR_DIFFUSION;

	size_t bytesPerRow = ((info->iWidth + 3) >> 2);
	size_t size = bytesPerRow * info->iHeight;
	imgHead = (imageHeader*) WEBC_MALLOC(sizeof(imageHeader) + size);
	if (imgHead)
	{	
		imgHead->imWidth    = info->iWidth;
		imgHead->imHeight   = info->iHeight;
		imgHead->imAlign    = 0;
		imgHead->imFlags    = 0;
		imgHead->imRowBytes = bytesPerRow;
		imgHead->imBits     = 2;
		imgHead->imPlanes   = 1;
	}

	switch (info->pixelFormat)
	{
		case WB_PIXEL_FORMAT_RGB_24:
			WEBC_NEW(converter, RGB24ToGreyscale);
			if (converter)
			{
				RGB24ToGreyscale *toGs = (RGB24ToGreyscale *) converter;
				toGs->setBitDepth(2);
			}
			break;
			
		case WB_PIXEL_FORMAT_INDEX_8:
			WEBC_NEW(converter, indexedToGreyscale);
			if (converter)
			{
				indexedToGreyscale *toGs = (indexedToGreyscale *) converter;
				toGs->setBitDepth(2);
				toGs->setPalette((unsigned char*) info->pPalette);
				if (info->bHasTransparentColor)
				{
					toGs->setTransparentIndex(info->iTransparentIndex);
				}
			}
			break;
			
		case WB_PIXEL_FORMAT_MONOCHROME_8:
			WEBC_NEW(converter, greyscale8ToGreyscale);
			if (converter)
			{
				greyscale8ToGreyscale *toGs = (greyscale8ToGreyscale *) converter;
				toGs->setBitDepth(2);
			}
			break;
		
		default:
			// some unsupported pixel format
			return WEBC_NULL;
	}
	
   #else
    #error number of colors not supported
   #endif
  
  #else
   #error only grayscale supported right now
  #endif

	WebcNucleusBitmap* bmp = (WebcNucleusBitmap*) WEBC_MALLOC(sizeof(WebcNucleusBitmap));

	if (!converter || !imgHead)
	{
		if (converter)
		{
			WEBC_DELETE(converter);
		}
	
		if (imgHead)
		{
			WEBC_FREE(imgHead);
		}

		if (bmp)
		{
			WEBC_FREE(bmp);
		}
		
		return (WEBC_NULL);
	}

	converter->go((WEBC_UCHAR8*) &imgHead[1], 
	              (unsigned char*) info->pData, 
	              info->iWidth, 
	              info->iHeight, 
	              ditherMode);

	bmp->img = (image *) imgHead;
	bmp->clip = 0;

	/* this is commented out until we figure out how to make transparent images work on Nucleus
	if (img && (pImage->transparent >= 0))
	{
		WEBC_UINT8 *pal;
		WEBC_UINT16 palSize;

		if (pImage->flags & GIF_FLAG_LOCAL_PALETTE)
		{
			pal = pImage->palette;
			palSize = 1 << ((pImage->flags & 0x7) + 1);
		}
		else if (mcHeaderFlags & GIF_FLAG_GLOBAL_PALETTE)
		{			
			pal = mpPalette;
			palSize = muPaletteSize;
		}
	
		// create a transparent region for clipping the image
		grafPort *tempPort = CreateBitmap( cMEMORY, pImage->width, pImage->height );
		if (tempPort)
		{
			grafPort *savePort;
			rect r;
			
			GetPort(&savePort);
			SetPort(tempPort);
			SetRect(&r, 0, 0, pImage->width, pImage->height);
			WriteImage(&r, img);						

			int red = pal[pImage->transparent*3];
			int grn = pal[pImage->transparent*3 + 1];
			int blu = pal[pImage->transparent*3 + 2];
			
			// tbd - this should really go through the graphics context,
			//  since we are not always a grayscale device
			WebColor nativeTrans = (RGBTOGREY(red,grn,blu) & 0xc0) >> 6;
			bmpList[t]->clip = BitmapToRegion(&r, nativeTrans);
			bmpList[t]->transColor = nativeTrans;

			DestroyBitmap(tempPort);
			SetPort(savePort);
		}
	}			
	*/
		
	WEBC_DELETE(converter);

	return (bmp);
}

/*---------------------------------------------------------------------------*/
unsigned int System_Process_One_Event (void);

static int GRAFX_ProcessInputQueue (
		void* fn,
		WEBC_BOOL block,
		WEBC_INT32* exitCode
	)
{
	return System_Process_One_Event ();
}

/*---------------------------------------------------------------------------*/
static void GRAFX_ClaimPointer (
		HTMLGraphicsContext** ctx
	)
{
}

/*---------------------------------------------------------------------------*/
static void GRAFX_ReleasePointer (
		HTMLGraphicsContext** ctx
	)
{
}

/*---------------------------------------------------------------------------*/
static WebColor GRAFX_GetColorByIndex (
		HTMLGraphicsContext** ctx,
		int i
	)
{
	static int colorMap [] =
	{
		Black,       // WGC_BLACK
		Red,         // WGC_RED
		Green,       // WGC_GREEN
		Brown,       // WGC_BROWN
		Blue,        // WGC_BLUE
		Magenta,     // WGC_MAGENTA
		Cyan,        // WGC_CYAN
		LtGray,      // WGC_LIGHTGRAY
		Gray,        // WGC_DARKGRAY
		LtRed,       // WGC_LIGHTRED
		LtGreen,     // WGC_LIGHTGREEN
		Yellow,      // WGC_YELLOW
		LtBlue,      // WGC_LIGHTBLUE
		LtMagenta,   // WGC_LIGHTMAGENTA
		LtCyan,      // WGC_LIGHTCYAN
		White,       // WGC_WHITE
		Black        // WGC_TRANSPARENT
	};

	return colorMap[i];
}

/*---------------------------------------------------------------------------*/
int webc_InitNucleusBrowserConfig (HTMLBrowserConfig* config, WINDOW nucleusWindow)
{
	webcNucleusGraphicsClass.getCanvasRect              = GRAFX_GetCanvasRect;

	webcNucleusGraphicsClass.claimPointer               = GRAFX_ClaimPointer;
	webcNucleusGraphicsClass.releasePointer             = GRAFX_ReleasePointer;

	webcNucleusGraphicsClass.invalidateRect             = GRAFX_InvalidateRect;
	webcNucleusGraphicsClass.beginDrawing               = GRAFX_BeginDrawing;
	webcNucleusGraphicsClass.getClipRect                = GRAFX_GetClipRect;
	webcNucleusGraphicsClass.setClipRect                = GRAFX_SetClipRect;
	webcNucleusGraphicsClass.endDrawing                 = GRAFX_EndDrawing;

	webcNucleusGraphicsClass.colorToRGBA                = GRAFX_ColorToRGB;
	webcNucleusGraphicsClass.RGBAToColor                = GRAFX_RGBToColor;
	webcNucleusGraphicsClass.getColorByIndex            = GRAFX_GetColorByIndex;

	webcNucleusGraphicsClass.drawText                   = GRAFX_DrawText;
	webcNucleusGraphicsClass.drawTextLen                = GRAFX_DrawTextLen;
	webcNucleusGraphicsClass.drawBitmap                 = GRAFX_DrawBitmap;
	webcNucleusGraphicsClass.drawBitmapStretchedToRect  = GRAFX_DrawStretchedBitmap;
	webcNucleusGraphicsClass.drawRectangle              = GRAFX_DrawRectangle;

	webcNucleusGraphicsClass.createFont                 = GRAFX_CreateFont;
	webcNucleusGraphicsClass.getDefaultFont             = GRAFX_GetDefaultFont;
	webcNucleusGraphicsClass.destroyFont                = GRAFX_DestroyFont;
	webcNucleusGraphicsClass.getNumFontFamilies         = GRAFX_NumFontFamilies;
	webcNucleusGraphicsClass.getFontFamilyNames         = GRAFX_GetFontFamilyNames;
	webcNucleusGraphicsClass.getFontFamilyGenericNames  = GRAFX_GetFontFamilyGenericNames;
	webcNucleusGraphicsClass.getTextHeight              = GRAFX_GetTextHeight;
	webcNucleusGraphicsClass.getTextWidth               = GRAFX_GetTextWidth;
	webcNucleusGraphicsClass.getTextHeightLen           = GRAFX_GetTextHeightLen;
	webcNucleusGraphicsClass.getTextWidthLen            = GRAFX_GetTextWidthLen;
	webcNucleusGraphicsClass.getFontHeight              = GRAFX_GetFontHeight;

	webcNucleusGraphicsClass.createWebBitmap            = GRAFX_CreateWebBitmap;
	webcNucleusGraphicsClass.destroyWebBitmap           = GRAFX_DestroyWebBitmap;
	webcNucleusGraphicsClass.createStretchedBitmap      = GRAFX_CreateStretchedBitmap;
	webcNucleusGraphicsClass.destroyStretchedBitmap     = GRAFX_DestroyStretchedBitmap;
	webcNucleusGraphicsClass.getBitmapWidth             = GRAFX_GetBitmapWidth;
	webcNucleusGraphicsClass.getBitmapHeight            = GRAFX_GetBitmapHeight;

	webcNucleusGraphicsClass.optionalDrawHorizontalLine = 0;
	webcNucleusGraphicsClass.optionalDrawVerticalLine   = 0;
	webcNucleusGraphicsClass.optionalDrawStyledFrame    = GRAFX_DrawPatternFrame;
	webcNucleusGraphicsClass.optionalDrawBitmapTiled    = 0; //GRAFX_TileBitmap;

	WebcNucleusGraphics* ctx = (WebcNucleusGraphics*) WEBC_MALLOC(sizeof(WebcNucleusGraphics));
	if (ctx)
	{
		grafPort* pDevice = nucleusWindow->drawPort;
		
		ctx->ctx = &webcNucleusGraphicsClass;
		ctx->devicePort = pDevice;
		ctx->bufferPort = CreateBitmap(cMEMORY, 
		                               pDevice->portRect.Xmax - pDevice->portRect.Xmin,
		                               pDevice->portRect.Ymax - pDevice->portRect.Ymin);
		if (ctx->bufferPort)
		{
			ctx->bufferDepth = 0;
			
			SetPort(ctx->devicePort);
			ReadPalette(0, 0, WG_NUM_COLORS-1, ctx->paletteData);
	
			rtp_memset(config, 0, sizeof(HTMLBrowserConfig));
			config->graphics = (HTMLGraphicsContext**) ctx;
			config->processInputQueue = &grafxProcessInputQueueFn;
	
			return (0);
		}
		
		WEBC_FREE(ctx);
	}

	return (-1);

}

/*---------------------------------------------------------------------------*/
void webc_DestroyNucleusBrowserConfig (HTMLBrowserConfig* config)
{
	WebcNucleusGraphics* ctx = (WebcNucleusGraphics*) config->graphics;

	DestroyBitmap(ctx->bufferPort);

	WEBC_FREE(config->graphics);
}

/*---------------------------------------------------------------------------*/
WebKey webc_TranslateNucleusKey         (WEBC_UINT32 rawKey)
{
	if (!nucleusKeyMapInitialized)
	{
		webc_CreateKeyMap(&nucleusKeyMap, nucleusKeyData, WG_NUM_GRAFX_KEYS);
		nucleusKeyMapInitialized = WEBC_TRUE;
	}

	return webc_TranslateKey(&nucleusKeyMap, rawKey);
}
