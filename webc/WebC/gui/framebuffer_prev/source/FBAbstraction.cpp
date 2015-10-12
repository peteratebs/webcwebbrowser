#ifndef WINDOWSGDI		/* So we can build GDI and Framebuffer from the same project */
#include "webc.h"
#include "webcmem.h"
#include "webrect.hpp"
#include "rtpstr.h"
#include "webcassert.h"
#include "webcstr.h"
#include "webcfg.h"
#include "webcLog.h"
#include "FBAbstraction.h"
#include "FB_HWInterface.h"
#include "FBConfig.h"

#include "FBAbstractionFonts.h"
#include "Filters.h"
#include "FBGraphicsContext.h"
#include <stdlib.h>
#include <math.h>
#include "rtpstr.h"
#include "Remoteterminal.h"
#include "einkterminal.h"

#include "rtptime.h"
#include "rtpprint.h"


extern void * currentActivatedWindow;//current window which is active
static void FBWIN_DrawThisonly(void *_pW);
void FBWM_closeWindow(HTMLGraphicsContext **ctx);
int FBWIN_ClipByWindowClipList(HTMLGraphicsContext **ctx, HTMLRect *inClipRect);
extern void *grabbedFocusedWindow;
extern int grabbedFocusedCount;


#if(WEBC_IMAGE_DRAW_EXTENSIONS)
static int FB_CreateGenericBitmap (
		void* webCBitmapSrc,
		HTMLGraphicsContext** ctx,
		WebGraphBitmapInfo** genericBitMapDst);
void FBWIN_GetCanvasRect(HTMLGraphicsContext **ctx,HTMLRect *rectangle);
#endif
static void * FB_PushCanvas(HTMLGraphicsContext** ctx, HTMLRect* rect);
static void FB_PopCanvas(HTMLGraphicsContext** ctx, void *pvretPrevCtx, int opacity);
static WEBC_UINT8 ALPHATRANSFORM(WEBC_UINT8 to, WEBC_UINT8  from, int opacity);



#define ENTERTIMEDFBOPERATION(W)
#define EXITTIMEDFBOPERATION(W)
void FB_resetDirtyRect(HTMLGraphicsContext **ctx);

/* Browser timout handller, called from an external timer handle */
extern "C" { void doFbBrowserTimeOut(unsigned long lBrowser); }

void doFbBrowserTimeOut(unsigned long lBrowser)
{
void *pW;
HBROWSER_HANDLE hBrowser;

	for (int i = 0; ; i++)
	{
		pW = FBWM_FindWindowByWindowNumber(i);
		if (!pW)
			break;
		hBrowser = FBWM_FindBrowserByWindow(pW);
		webc_BrowserProcessTimers(hBrowser);
		webc_BrowserDraw(hBrowser);
		webc_BrowserProcessLoadQueueAll(hBrowser);
		webc_BrowserDraw(hBrowser);
	}
}


#define GUARD_BUFFER_LIMITS 1

#ifdef GUARD_BUFFER_LIMITS
WEBC_UCHAR8 *fb0MaxAddress = 0;
WEBC_UCHAR8 *fb1MaxAddress = 0;
#endif

#define RECT_TO_HTML(H, R)				 \
	((H).left = (R).X, (H).top = (R).Y, \
	 (H).right = (R).X + (R).Width - 1, (H).bottom = (R).Y + (R).Height - 1)
#define HTML_TO_RECT(R, H)		 Rect R((H).left, (H).top, (H).right - (H).left + 1, (H).bottom - (H).top + 1);

#define GET_FBGRAPHICS_CTX(X)	 ((FBGraphicsContext *)(X))

static int FB_clipRectCoords(HTMLGraphicsContext **ctx, DISPLAY_INT *x,DISPLAY_INT *y, DISPLAY_INT *w,DISPLAY_INT *h);

HTMLGraphicsContext graphicsLayerCalls;

int (*fbProcessInputQueueFn)(
	void		 *fn,
	WEBC_BOOL  block,
	WEBC_INT32 *exitCode) = FB_ProcessInputQueue;


typedef struct tagWEBC_RGBQUAD
{
	WEBC_UCHAR8 rgbBlue;
	WEBC_UCHAR8 rgbGreen;
	WEBC_UCHAR8 rgbRed;
	WEBC_UCHAR8 rgbReserved;
} WEBC_RGBQUAD;
typedef WEBC_RGBQUAD * LPWEBC_RGBQUAD;


/// Private prototypes

static void FB_setColor(HTMLGraphicsContext **ctx,
	WEBC_UINT32 color);

static void FB_setColor(HTMLGraphicsContext **ctx,
	WEBC_UINT8 red,
	WEBC_UINT8 green,
	WEBC_UINT8 blue);
static void FB_setColor(HTMLGraphicsContext **ctx,
	WEBC_UINT8 red,
	WEBC_UINT8 green,
	WEBC_UINT8 blue,
	WEBC_UINT8 opacity
	);

static void FB_setTransparency(HTMLGraphicsContext **ctx);

static void FB_getBGColor(HTMLGraphicsContext **ctx, int posX, int posY, WEBC_UINT8 *bgRed, WEBC_UINT8 *bgGreen, WEBC_UINT8 *bgBlue);

static void FB_drawHorizontalLine(HTMLGraphicsContext **ctx,
	DISPLAY_INT xPos,
	DISPLAY_INT yPos,
	DISPLAY_INT length);

void FB_drawRectangle(HTMLGraphicsContext **ctx,
	DISPLAY_INT x,
	DISPLAY_INT y,
	DISPLAY_INT width,
	DISPLAY_INT height,
	DISPLAY_INT BorderThickness,
	WEBC_BOOL  fill,
	WEBC_UINT32 lineColor,
	WEBC_UINT32 fillColor);

static void FB_drawBitmap(HTMLGraphicsContext **ctx,
	WebGraphBitmapInfo *bitmapPtr,
	DISPLAY_INT		  dispX,
	DISPLAY_INT		  dispY
	);

static void FB_drawBitmap(HTMLGraphicsContext **ctx,
	WebGraphBitmapInfo *bitmapPtr,
	DISPLAY_INT		  dispX,
	DISPLAY_INT		  dispY,
	DISPLAY_INT		  dispWidth,
	DISPLAY_INT		  dispHeight);

static void FB_setClipRect(HTMLGraphicsContext **ctx,
	HTMLRect rect);

static HTMLRect FB_getClipRect(HTMLGraphicsContext **ctx);

static void FB_addDirtyRect(HTMLGraphicsContext **ctx,
	HTMLRect passedRect);


HTMLRect FB_getDirtyRect(HTMLGraphicsContext **ctx);

// static void FB_resetDirtyRect(HTMLGraphicsContext **ctx);

//void DoPrintOut(char *cVal);
////  end private prototypes



// Get the base address of the frame buffer to draw into.
// If we are headless remote terminal client we'll draw into a transfer buffer
WEBC_UINT8 *lGetBaseAddress(HTMLGraphicsContext **ctx, FBHWInterface *dstFrameInfo, WEBC_INT whichBuffer)
{
	return dstFrameInfo->GetBaseAddress(whichBuffer);
}
// Display the frame buffer we just drew into.
// If we are headless remote terminal client we'll send the buffer
void lDoDisplay(	HTMLGraphicsContext **ctx )
{
FBHWInterface *dstFrameInfo = GET_FBGRAPHICS_CTX(ctx)->frameBufferItf;
	dstFrameInfo->DoDisplay();
}

/*********************BASIC DRAW FUNCTIONS BEGIN HERE**********************************/

/**
 *	This function is called by WebC when it needs to draw a rectangle.
 *	The rectangle must be drawn in the native color passed in outlineColor
 *	and if "fill" is non-zero the rectangular area should be filled with
 *	the native color passed in fillcolor.This function is static void EBS_
 */
void FB_DrawRectangle(
	HTMLGraphicsContext **ctx,
	HTMLRect				*rect,
	WebColor				outlineColor,
	WebColor				fillColor,
	WEBC_BOOL			  fill
	)
{
    ENTERTIMEDFBOPERATION(FBDRAWRECT)
	FB_drawRectangle(ctx, rect->left, rect->top, rect->right - rect->left + 1,
														rect->bottom - rect->top + 1, 1, fill, outlineColor, fillColor);
    EXITTIMEDFBOPERATION(FBDRAWRECT)
}


/**
 *	This function is used to draw a Horizontal line
 */
static void FB_DrawHorizontalLine(
	HTMLGraphicsContext **ctx,
	DISPLAY_INT			y,
	DISPLAY_INT			left,
	DISPLAY_INT			right,
	WebColor				color,
	DISPLAY_INT			thickness)
{
	for (int i = 0; i < thickness; i++)
	{
		FB_drawHorizontalLine(ctx, left, y + i, right - left + 1);
	}
}


/**
 *	This function is used to draw a vertical line
 */
static void FB_DrawVerticalLine(
	HTMLGraphicsContext **ctx,
	DISPLAY_INT			x,
	DISPLAY_INT			top,
	DISPLAY_INT			bottom,
	WebColor			color,
	DISPLAY_INT			thickness)
{
	for (int i = top; i < bottom; i++)
	{
		FB_drawHorizontalLine(ctx, x, i, thickness);
	}
}


/*******************BITMAP FUNCTIONS BEGIN HERE***********************************?
 *
 *	This function is called by WebC when it needs to scale a bitmap (pixmap)
 *	that was returned from GUINAME_CreateWebBitmap.
 *
 *	The return value must be a valid void* pointer to the bitmap scaled to the
 *	correct size and height.
 *	If the routine fails 0 will be returned.
 *
 *
 */

// The source for FB_createStretchedBitmap is large it is provided below.
static void *FB_createStretchedBitmap(
	HTMLGraphicsContext **ctx,
	void				*b,
	DISPLAY_INT			w,
	DISPLAY_INT			h
	);


void *FB_CreateStretchedBitmap(
	HTMLGraphicsContext **ctx,
	void					 *b,
	DISPLAY_INT			w,
	DISPLAY_INT			h
	)
{
	void *r;
    ENTERTIMEDFBOPERATION(FBCREATESTRETCHEDBITMAP)
	r = FB_createStretchedBitmap(ctx,b,w,h);
    EXITTIMEDFBOPERATION(FBCREATESTRETCHEDBITMAP)
	return r;

}




/**
 * This function is called by WebC when it needs a bitmap (pixmap) that can be displayed,
 * stretched and measured by native graphics implementation. The routine must allocate
 * data to hold the native formatted bitmap and populate it using the content provided
 * in the WebGraphBitmapInfo structure. The format of WebGraphBitmapInfo is provided
 * above in the WebC type descriptions and should provide a framework for how to create
 * a native bitmap from the input.
 *
 * This routine must return a void pointer to the native bitmap or 0 if it fails.
 */
void *FB_CreateWebBitmap(
	HTMLGraphicsContext		**ctx,
	const WebGraphBitmapInfo *info
	)
{
    ENTERTIMEDFBOPERATION(FBCREATEBITMAP)
	// check format -  new 3-2010
	switch (info->pixelFormat)
	{
		case WB_PIXEL_FORMAT_RGB_24:
		case WB_PIXEL_FORMAT_INDEX_8:
#if (WEBC_SUPPORT_BMP_16BITS)
		case WB_PIXEL_FORMAT_RGB_16:
#endif
			break;
		case WB_PIXEL_FORMAT_RGBA_32:
			break;
		default:
			// error: unsupported bit depth (our image converters should
			//  always do 8bit or 24bit)
			EXITTIMEDFBOPERATION(FBCREATEBITMAP)
			return (WEB_BITMAP_NULL);
	}
	WebGraphBitmapInfo *intern = (WebGraphBitmapInfo *)WEBC_MALLOC_VERBOSE(sizeof(WebGraphBitmapInfo),"BitMapHeader");
	if (!intern)
	{
		EXITTIMEDFBOPERATION(FBCREATEBITMAP)
		return(0);
	}
	*intern		 = *info;
	intern->pPalette = 0;
	intern->pData = (WEBC_UINT8 *)WEBC_MALLOC_VERBOSE(info->iBytesPerRow * info->iHeight,"BitMapData");
	if (!intern->pData)
	{
		WEBC_FREE((void *)intern);
		EXITTIMEDFBOPERATION(FBCREATEBITMAP)
		return(0);
	}
	rtp_memcpy((void *)intern->pData, (void *)info->pData, (info->iBytesPerRow * info->iHeight));


#if (WEBC_SUPPORT_BMP_16BITS)
	if (intern->pixelFormat != WB_PIXEL_FORMAT_RGB_24 && intern->pixelFormat != WB_PIXEL_FORMAT_RGBA_32 && intern->pixelFormat != WB_PIXEL_FORMAT_RGB_16)
#else
	if (intern->pixelFormat != WB_PIXEL_FORMAT_RGB_24 && intern->pixelFormat != WB_PIXEL_FORMAT_RGBA_32)
#endif
	{
		intern->pPalette = (WEBC_UINT8 *)WEBC_MALLOC_VERBOSE(info->iPaletteSize * 3,"BitMapPallet");			//the 3 comes from the fact that each entry in a the pallete is three bytes, so the numbner of entries times three gives the correct number
		if (!intern->pPalette)
		{
			WEBC_FREE((void *)intern->pData);
			WEBC_FREE((void *)intern);
			EXITTIMEDFBOPERATION(FBCREATEBITMAP)
			return(0);
		}
		rtp_memcpy((void *)intern->pPalette, (void *)info->pPalette, (info->iPaletteSize * 3));
	}
	else
	{
		intern->pPalette = NULL;
	}

	EXITTIMEDFBOPERATION(FBCREATEBITMAP)
	return((void *)intern);
}


/**
 *	This function is called by WebC to release (free) a native bitmap
 *	structure that was created by GUINAME_CreateWebBitmap.
 */
void FB_DestroyWebBitmap(
	HTMLGraphicsContext **ctx,
	void					 *b
	)
{
	WebGraphBitmapInfo *ptr = (WebGraphBitmapInfo *)b;

	if (ptr->pData)
	{
		WEBC_FREE((void *)ptr->pData);
	}
	if (ptr->pPalette)
	{
		WEBC_FREE((void *)ptr->pPalette);
	}
	WEBC_FREE(ptr);
}


/**
 *	This function is called by WebC when it needs to render a bitmap. The bitmap
 *	is passed through the void pointer at *bmp. This bitmap is the void pointer
 *	that was returned from GUINAME_CreateWebBitmap().
 */
void FB_DrawBitmap(
	HTMLGraphicsContext **ctx,
	DISPLAY_INT			dispX,
	DISPLAY_INT			dispY,
	void					 *bmp
	)
{
	WebGraphBitmapInfo *bitmapPtr = (WebGraphBitmapInfo *)bmp;
    ENTERTIMEDFBOPERATION(FBDRAWBITMAP)
	FB_drawBitmap(ctx, bitmapPtr, dispX, dispY);
    EXITTIMEDFBOPERATION(FBDRAWBITMAP)
}


/**
 *	This function is called by WebC to retrieve the height of a native bitmap
 *	structure that was created by FB_CreateWebBitmap or FB_CreateStretchedBitmap.
 */
DISPLAY_INT FB_GetBitmapHeight(
	HTMLGraphicsContext **ctx,
	void					 *bmp
	)
{
	return(((WebGraphBitmapInfo *)bmp)->iHeight);
}


/**
 *	This function is called by WebC to retrieve the width of a native bitmap
 *	structure that was created by FB_CreateWebBitmap or FB_CreateStretchedBitmap.
 */
DISPLAY_INT FB_GetBitmapWidth(
	HTMLGraphicsContext **ctx,
	void					 *bmp
	)
{
	return(((WebGraphBitmapInfo *)bmp)->iWidth);
}


/********FONT FUNCTIONS BEGIN HERE**********************************/

/**
 *	This function is called by WebC when it needs to draw a NULL terminated text string
 *	at X, Y. The text itself should be drawn in the native color passed as TextColor.
 *	If Fill is non-zero the text background should be drawn in the native color passed
 *	as backgroundColor. The font to use is passed in the void pointer "font". This is
 *	the native font family pointer or handle that was returned from the functions
 *	createFont or getDefaultFont.
 */


void FB_DrawText(
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
	WebChar *ch	 = (WebChar *)text;
	long	 length = 0;
    ENTERTIMEDFBOPERATION(FBDRAWTEXT)
	while (ch[0] != '\0')
	{
		ch++;
		length++;
	}
	FB_DrawTextLen(ctx, x, y, text, textColor, backgroundColor, fill, font, length);
    EXITTIMEDFBOPERATION(FBDRAWTEXT)
}


#define INT_MULT(a,b,t) ( (t) = (a) * (b) + 0x80, ( ( ( (t)>>8 ) + (t) )>>8 ) )
//#define INT_MULT(a,b,t) ( (t) = (a) * (b), ( ( ( (t)>>8 ) + (t) ) >>8 ) )

/**
 *	This function is identical to GUINAME_DrawText except the text to
 *	display is not terminated andthe function must display "textlen" characters only.
 */
static void FB_FillTextBgLen(
	HTMLGraphicsContext **ctx,
	DISPLAY_INT			x,
	DISPLAY_INT			y,
	const WebChar		 *text,
	WebColor				backgroundColor,
	void					 *font,
	long					 textLen
	);

void FB_DrawTextLen(
	HTMLGraphicsContext **ctx,
	DISPLAY_INT			x,
	DISPLAY_INT			y,
	const WebChar		 *text,
	WebColor				textColor,
	WebColor				backgroundColor,
	WEBC_BOOL			  fill,
	void					 *font,
	long					 textLen
	)
{
#if (RENDERRGB24||RENDERRGBA)
#elif (RENDER8BITINDEX)
WEBC_UINT8 fgcolor = (WEBC_UINT8) (textColor&0xff);
#endif
	register WEBC_UINT8 opacity = GET_FBGRAPHICS_CTX(ctx)->opacity;
	register WEBC_UINT8 hasopacity = GET_FBGRAPHICS_CTX(ctx)->hasopacity;
	HTMLRect	clipBounds = GET_FBGRAPHICS_CTX(ctx)->clipBounds;
    ENTERTIMEDFBOPERATION(FBDRAWTEXTLEN)
	// Fill the background if requested.
	if (FBWIN_ClipByWindowClipList(ctx, &clipBounds) < 0)
		return;
	if (fill)
		FB_FillTextBgLen(ctx, x, y, text, backgroundColor, font, textLen);

	FB_setColor(ctx, textColor);
	opacity = GET_FBGRAPHICS_CTX(ctx)->opacity;
	hasopacity = GET_FBGRAPHICS_CTX(ctx)->hasopacity;

// HEREHERE - Need support for n bit fonts and n byte grayscale
	WEBC_UINT32 redmask, greenmask, bluemask;
	redmask	= 0x00ff0000;
	greenmask = 0x0000ff00;
	bluemask  = 0x000000ff;

	WEBC_UINT8 textRed	= (WEBC_UINT8) ((textColor & redmask) >> (16));  //shift over 16 bits to remove excess
	WEBC_UINT8 textGreen = (WEBC_UINT8)((textColor & greenmask) >> (8)); //shift over 8 bits to remove excess
	WEBC_UINT8 textBlue  = (WEBC_UINT8)(textColor & bluemask);
	WEBC_UINT8 bgColorRed, bgColorGreen, bgColorBlue;
	if (fill)
	{
		bgColorRed	= (WEBC_UINT8)((backgroundColor & redmask) >> (16));		 //shift over 16 bits to remove excess
		bgColorGreen = (WEBC_UINT8)((backgroundColor & greenmask) >> (8));		//shift over 8 bits to remove excess
		bgColorBlue  = (WEBC_UINT8)((backgroundColor & bluemask));
	}
	else if (textRed != 0xFF || textGreen != 0xFF || textBlue != 0xFF)
	{
		bgColorRed	= 0xFF;			 //shift over 16 bits to remove excess
		bgColorGreen = 0xFF;			 //shift over 8 bits to remove excess
		bgColorBlue  = 0xFF;
	}
	else
	{
		bgColorRed	= 0x00;			 //shift over 16 bits to remove excess
		bgColorGreen = 0x00;			 //shift over 8 bits to remove excess
		bgColorBlue  = 0x00;
	}

	FB_Font *f = (FB_Font *)font;
	WebGraphBitmapInfo *srcBitmap = f->charMap;
	FBHWInterface * dstFrameInfo = GET_FBGRAPHICS_CTX(ctx)->frameBufferItf;
	int whichBuffer = GET_FBGRAPHICS_CTX(ctx)->currentGBuffer;

	WEBC_UINT8			*srcRowPtr, *srcColPtr, *destRowPtr, *destColPtr;
	WEBC_UINT8 bgPixelRed=bgColorRed, bgPixelGreen=bgColorGreen, bgPixelBlue=bgColorBlue;


	DisplayChar current;
	WebChar	  *ch  = (WebChar *)text;
	int dstBytesPerPixel = dstFrameInfo->GetDepth();
	DISPLAY_INT unclippedposX = x, posY = y, dstCols, dstLines;
	for (int j = 0; j < textLen; j++, ch++)
	{
	DISPLAY_INT posX = unclippedposX;
	int remainder;
	int bitPos, bitReset;
	int numRasterLines;


		f = (FB_Font *)font;
		if((ch[0] < f->startChar) || (ch[0] > f->endChar))
		{
			f = FB_SubstituteFont(ctx, f, ch[0]);
		}

		if(!f)
		{ // no glyph no, width, go to the next character
			continue;
		}

		srcBitmap = f->charMap;

		current = f->charData[ch[0] - f->startChar + 1];
		// clip the destination
		if(clipBounds.left > posX)
		{
			DISPLAY_INT newWidth;
			current.x += clipBounds.left - posX; // take less of the source bitmap also
			newWidth = (current.width - (clipBounds.left - posX));
			if(newWidth <= 0) // skip this character if it's entirely off the screen
				goto next_character;
			current.width = newWidth;
			posX = clipBounds.left;
		}

		posY = y;
		if(clipBounds.top > posY)
		{
			current.y += clipBounds.top - posY;
			current.height -= clipBounds.top - posY;
			if(current.height <= 0)
				goto next_character;
			posY = clipBounds.top;
		}

		dstCols = current.width;
		dstLines = current.height;

		// now clip the right and bottom edges
		if((posX + dstCols) > clipBounds.right)
			dstCols = (clipBounds.right - posX);

		if(dstCols <= 0)
			goto next_character;

		if((posY + dstLines) > clipBounds.bottom)
			dstLines = (clipBounds.bottom - posY);

		if(dstLines <= 0)
			goto next_character;

		// find the pointers to the image data
#if 0
		srcRowPtr  = (WEBC_UINT8 *)srcBitmap->pData + (current.y * srcBitmap->iBytesPerRow) + (current.x * 3);
#else // one bit font map
		srcRowPtr  = (WEBC_UINT8 *)srcBitmap->pData + (current.y * srcBitmap->iBytesPerRow) + (current.x / 8); // the bitmapped font image
		remainder = (current.x % 8);
		bitPos = 7;
		if(remainder)
		{
			bitPos = 7 - remainder;
		}
		bitReset = bitPos;
#endif

		destRowPtr  = lGetBaseAddress(ctx, dstFrameInfo,whichBuffer) + (posY * dstFrameInfo->GetRowBytes()) + (posX * dstBytesPerPixel);


		numRasterLines = dstLines;

		// probably don't need this check
		if(dstFrameInfo->GetHeight() < (posY + numRasterLines))
		  numRasterLines -= (posY + numRasterLines) - dstFrameInfo->GetHeight();

		for (int i = 0; i < numRasterLines; i++)
		{
			srcColPtr  = srcRowPtr;
			destColPtr = destRowPtr;
			for (int j = 0; j < dstCols; j++, destColPtr += dstBytesPerPixel)
			{
#ifdef GUARD_BUFFER_LIMITS
			  if(&(destColPtr[2]) > (whichBuffer ? fb1MaxAddress : fb0MaxAddress))
			  {
					return;
					//exit(0);
			  }
#endif
				WEBC_UINT8 tempFontRed,tempFontBlue,tempFontGreen;
#if 0
				WEBC_UINT8 tempFontGreen, tempFontBlue;

				tempFontRed = srcColPtr[0];
				tempFontGreen = srcColPtr[1];
				tempFontBlue = srcColPtr[2];
				srcColPtr += 3;
#else // one bit font map
				tempFontRed = (*srcColPtr & (1 << bitPos)) ? 0xFF : 0;
				tempFontBlue = tempFontGreen = tempFontRed;
				if(--bitPos < 0)
				{
					srcColPtr++;
					bitPos = 7;
			  }
#endif
#if (RENDERRGB24||RENDERRGBA)

				unsigned int w2,tmp,tmp2;
				if (!fill)
				{
					bgPixelRed = destColPtr[0];
					bgPixelGreen = destColPtr[1];
					bgPixelBlue = destColPtr[2];
				}
				if ((tempFontRed == 0xFF) && (tempFontBlue == 0xFF) && (tempFontGreen == 0xFF))
				{
#if 0	// Don't draw the background color. use transparent
					destColPtr[0] = bgColorRed;
					destColPtr[1] = bgColorGreen;
					destColPtr[2] = bgColorBlue;
#endif
				}
				else
				{
					// RED
					if(0 == tempFontRed)
					{
						if (hasopacity)
							destColPtr[0] = ALPHATRANSFORM(destColPtr[0], textRed, opacity);
						else
							destColPtr[0] = textRed;
					}
					else if (255 == tempFontRed)
					{
						destColPtr[0] = bgPixelRed;
					}
					else
					{
						destColPtr[0] = ALPHATRANSFORM(destColPtr[0], bgPixelRed, opacity);
						WEBC_UINT8 w2 = (255 - tempFontRed);
						destColPtr[0] = INT_MULT((unsigned int)bgPixelRed, (unsigned int)tempFontRed, tmp) +  INT_MULT((unsigned int)textRed, (unsigned int)w2, tmp2);
					}

					// GREEN
					if(0 == tempFontGreen)
					{
						if (hasopacity)
							destColPtr[1] = ALPHATRANSFORM(destColPtr[1], textGreen, opacity);
						else
							destColPtr[1] = textGreen;
					}
					else if (255 == tempFontGreen)
					{
						destColPtr[1] = bgPixelGreen;
					}
					else
					{
						w2 = (255 - tempFontGreen);
						destColPtr[1] = INT_MULT((unsigned int)bgPixelGreen, (unsigned int)tempFontGreen, tmp) + INT_MULT((unsigned int)textGreen, (unsigned int)w2, tmp2);
					}

					// GREEN
					if(0 == tempFontBlue)
					{
						if (hasopacity)
							destColPtr[2] = ALPHATRANSFORM(destColPtr[2], textBlue, opacity);
						else
							destColPtr[2] = textBlue;
					}
					else if (255 == tempFontBlue)
					{
						destColPtr[2] = bgPixelBlue;
					}
					else
					{
						w2 = (255 - tempFontBlue);
						destColPtr[2] = INT_MULT((unsigned int)bgPixelBlue, (unsigned int)tempFontBlue, tmp) + INT_MULT((unsigned int)textBlue, (unsigned int)w2, tmp2);
					}

				}
#elif (RENDER8BITINDEX)
//				if (!fill)
//					bgPixelRed = 0xff; // destColPtr[0];
//				else
//					bgPixelRed  = bgColorRed;
				// RED
				if(tempFontRed)
					/* destColPtr[0] = bgcolor */; // remove for transparent
				else
					destColPtr[0] = fgcolor;



//				if(0 == tempFontRed)
//					destColPtr[0] = textRed;
//				else if (255 == tempFontRed)
//					destColPtr[0] = bgPixelRed;
#endif
			}
			destRowPtr += dstFrameInfo->GetRowBytes();
			srcRowPtr  += srcBitmap->iBytesPerRow;
			bitPos = bitReset;
		}
next_character:
		unclippedposX += current.width;
	}

    EXITTIMEDFBOPERATION(FBDRAWTEXTLEN)
}

static void FB_FillTextBgLen(
	HTMLGraphicsContext **ctx,
	DISPLAY_INT			x,
	DISPLAY_INT			y,
	const WebChar		 *text,
	WebColor				backgroundColor,
	void					 *font,
	long					 textLen
	)

{
	HTMLRect rect;


	DISPLAY_INT	w = FB_GetTextWidthLen(ctx, text, font, textLen);
	DISPLAY_INT	h = FB_GetTextHeightLen(ctx, text, font, textLen);
	DISPLAY_INT	_x = x;
	DISPLAY_INT	_y = y;

	if (FB_clipRectCoords(ctx, &_x, &_y, &w, &h) == 1)
	{
		rect.left	= _x;
		rect.right  = _x + w;
		rect.top	 = _y;
		rect.bottom = _y + h;
		FB_DrawRectangle(ctx, &rect, backgroundColor, backgroundColor, true);
	}
}



/**
 *	This function is called by WebC to retrieve the height of the rectangle
 *	occupied by the null terminated string when it is rendered in the provided
 *	font. If the font height is fixed this will be the character height of the
 *	font, otherwise it will be the distance from the lowest decender to the
 *	highest ascender in the string.
 */
DISPLAY_INT FB_GetTextHeight(
	HTMLGraphicsContext **ctx,
	const WebChar		 *text,
	void					 *font
	)
{
	WebChar *ch  = (WebChar *)text;
	long					 textLen = 0;

    ENTERTIMEDFBOPERATION(FBGETTEXTHEIGHT)
	while (ch[0] != '\0')
	{
		textLen ++;
		ch += 1;
	}
	DISPLAY_INT r = FB_GetTextHeightLen(ctx, text, font, textLen);
    EXITTIMEDFBOPERATION(FBGETTEXTHEIGHT)
	return r;
}


/**
 *	This function is called by WebC to retrieve the height of the rectangle occupied
 *	by the provided non terminated sequence of characters of the specified length.
 *	It is identical to FB_GetTextHeight() except the string is length specified
 *	rather than NULL terminated.
 */
DISPLAY_INT FB_GetTextHeightLen(
	HTMLGraphicsContext **ctx,
	const WebChar		 *text,
	void					 *font,
	long					 textLen
	)
{
	FB_Font *f	= (FB_Font *)font;
	WebChar *ch  = (WebChar *)text;
	WebChar temp = ch[0];

    ENTERTIMEDFBOPERATION(FBGETTEXTHEIGHTLEN)
	DISPLAY_INT maxHeight = 0;
	for (int i = 0; i < textLen; i++)
	{
		f = (FB_Font *)font;
		if((temp < f->startChar) || (temp > f->endChar))
		{
			f = FB_SubstituteFont(ctx, f, temp);
		}

		if (f && f->charData[temp - f->startChar + 1].height > maxHeight)
		{
			maxHeight = f->charData[temp - f->startChar + 1].height;
		}
		ch += 1;
		temp = ch[0];
	}
    EXITTIMEDFBOPERATION(FBGETTEXTHEIGHTLEN)

	return(maxHeight);
}


/**
 *	This function is called by WebC to retrieve the width of the rectangle occupied
 *	by the null terminated string when it is rendered in the provided font.
 */
DISPLAY_INT FB_GetTextWidth(
	HTMLGraphicsContext **ctx,
	const WebChar		 *text,
	void					 *font
	)
{
	WebChar *ch  = (WebChar *)text;
	long					 textLen = 0;

    ENTERTIMEDFBOPERATION(FBGETTEXTWIDTH)
	while (ch[0] != '\0')
	{
		textLen++;
		ch += 1;
	}
	DISPLAY_INT r = FB_GetTextWidthLen(ctx, text,  font, textLen);
    EXITTIMEDFBOPERATION(FBGETTEXTWIDTH)
	return r;
}


/**
 *	This function is called by WebC to retrieve the width of the rectangle occupied by
 *	the length specified string when it is rendered in the provided font. It is
 *	identical to GUINAME_GetTextWidth() except the string is length specified
 *	rather than NULL terminated.
 */
DISPLAY_INT FB_GetTextWidthLen(
	HTMLGraphicsContext **ctx,
	const WebChar		 *text,
	void					 *font,
	long					 size
	)
{
	FB_Font *f	= (FB_Font *)font;
	WebChar *ch  = (WebChar *)text;
	WebChar temp = ch[0];

	DISPLAY_INT width = 0;
    ENTERTIMEDFBOPERATION(FBGETTEXTWIDTHLEN)
	for (int i = 0; i < size; i++)
	{
		f = (FB_Font *)font;
		if((temp < f->startChar) || (temp > f->endChar))
		{
			f = FB_SubstituteFont(ctx, f, temp);
		}

		if(f)
			width += (f->charData[temp - f->startChar + 1].width);
		ch	 += 1;
		temp = ch[0];
	}
    EXITTIMEDFBOPERATION(FBGETTEXTWIDTHLEN)
	return(width);
}


/**
 *	This function is called by WebC to retrieve the height of a rectangle occupied
 *	by the provided font. If the font is fixed height the returned height should
 *	be the fixed size. If characters have different heights it should return the
 *	distance from the lowest decender in the font to the highest ascender.
 */
DISPLAY_INT FB_GetFontHeight(
	HTMLGraphicsContext **ctx,
	void					 *font
	)
{
	return(((FB_Font *)font)->height);
}


/**
 *	This function is called by WebC to retrieve the distance in pixels from the top of
 *	the cell ascent part of a character to the base of the ascent part. Take for example
 *	the letters M and Q. There is no descender for M so the font baseline is the
 *	height of the letter. For Q the font baseline is the base base of the large circle.
 *	If the pointer to this function is not populated WebC defaults to
 *	using four fifths of the font height + 1 as the font baseline.
 */
DISPLAY_INT FB_GetFontBaseline(
	HTMLGraphicsContext **ctx,
	void					 *font
	)
{
	return(((FB_Font *)font)->baseline);
}


/**
 *	This function is called by WebC when a mouse button down event is
 *	detected while the pointer is over a slider or button. This routine must
 *	insure that all mouse events are delivered to WebC until
 *	GUINAME_ReleasePointer() is called.
 */
static void FB_ClaimPointer(
	HTMLGraphicsContext **ctx
	)
{
	if (grabbedFocusedWindow==0)
		grabbedFocusedWindow = (void *) (*((FBGraphicsContext *)ctx)).windowHandle;
	grabbedFocusedCount++;
}


/**
 *  This function is called by WebC when a mouse button up event is detected
 *  and it had previpusly called GUINAME_ClaimPointer(). This routine must
 *  inform the GUI framework that WebC no longer requires all mouse events
 *  and if the pointer focus has changed it may deliver mouse events to the
 *  appropriate window.
 */
static void FB_ReleasePointer(
	HTMLGraphicsContext **context
	)
{
	if (grabbedFocusedCount)
		grabbedFocusedCount--;
	if (grabbedFocusedCount==0)
		grabbedFocusedWindow = 0;
}


/**
 *	This function is called by WebC when it requires a rectangle to be
 *	added to drawing operations it must ensure that the GUI framework will
 *	allow draw operations may be performed in the region and if it double
 *	buffers draw operations it must ensure that all regions marked invalid
 *	after a call to GUINAME_BeginDrawing() are refreshed to the display
 *	when GUINAME_EndDrawing() is called and the regions are marked valid by
 *	GUINAME_EndDrawing(). All invalidated regions must be OR�ed together to
 *	create the smallest rectangle that will contain all of the rectangles
 *	passed to GUINAME_InvalidateRect.
 */
void FB_InvalidateRect(
	HTMLGraphicsContext **ctx,
	HTMLRect				*rect
	)
{
	FB_addDirtyRect(ctx, (*rect));
}

/**
 *	 This function is called by WebC when it is beginning a sequence of
 *	invalidate and draw operation that will end with a call to GUINAME_
 *	EndDrawing. If an off-screen buffer is being used for drawing the
 *	underlying draw engine should be informed that a sequence of draws is
 *	beginning.
 */
WEBC_DEBUG_LOG_TIME_HANDLE(TimeInsideDraw)

void FB_BeginDrawing(
	HTMLGraphicsContext **ctx
	)
{

	if (((FBGraphicsContext *)ctx)->drawNesting == 0)
	{
		WEBC_DEBUG_LOG_TIME_ENTER(TimeInsideDraw,"Begin Draw"," ")
    	ENTERTIMEDFBOPERATION(FBBEGINANDENDDRAW)
	}

	((FBGraphicsContext *)ctx)->drawNesting++;
}

#if (BUFFER_INVALID_REGIONS_ONLY)
static void FB_CopyFrameBuffer(HTMLGraphicsContext **ctx,int toBufferNo, int fromBufferNo, HTMLRect &r);
#endif

/**
 *	This function is called by WebC when it is beginning a sequence of
 *	invalidate and draw operation has completed and the underlying
 *	framework need to update the screen. If necessary the framework should
 *	build all invalidated regions to the screen buffer and clear the
 *	invalidated region list.
 */

void FB_EndDrawing(
	HTMLGraphicsContext **ctx
	)
{
int inBackGroundDraw = 0;

#if (USE_DOUBLE_BUFFERING)
	if (((FBGraphicsContext *)ctx)->drawNesting == 1)
	{
		// future draws go to the back (other) buffer
		if(((FBGraphicsContext *)ctx)->currentGBuffer)
		{
#if (!BUFFER_INVALID_REGIONS_ONLY)
			// copy buffer 1 to buffer 0 (so the next time we draw, we have the current contents)
			rtp_memcpy(lGetBaseAddress(ctx,((FBGraphicsContext *)ctx)->frameBufferItf,0)
			, lGetBaseAddress(ctx,((FBGraphicsContext *)ctx)->frameBufferItf,1), ((FBGraphicsContext *)ctx)->frameBufferItf->GetRowBytes() * ((FBGraphicsContext *)ctx)->frameBufferItf->GetHeight());
#endif
			if (!inBackGroundDraw)
			{
				// show the back buffer
				((FBGraphicsContext *)ctx)->frameBufferItf->SwitchToBuffer(1);
#if (BUFFER_INVALID_REGIONS_ONLY)
			// Copy the rectangle that was modified from buffer 1 to bufer 0
			HTMLRect r = FB_getDirtyRect(ctx);
			if (r.top >=  0)
				FB_CopyFrameBuffer(ctx, 0, 1, r);
#endif
				// now do all remaining draws to the other (back) buffer
				((FBGraphicsContext *)ctx)->currentGBuffer = 0;
			}
		}
		else
		{
#if (!BUFFER_INVALID_REGIONS_ONLY)
			// copy buffer 0 to buffer 1
			rtp_memcpy(lGetBaseAddress(ctx,((FBGraphicsContext *)ctx)->frameBufferItf,1), lGetBaseAddress(ctx,((FBGraphicsContext *)ctx)->frameBufferItf,0), ((FBGraphicsContext *)ctx)->frameBufferItf->GetRowBytes() * ((FBGraphicsContext *)ctx)->frameBufferItf->GetHeight());
#endif
			if (!inBackGroundDraw)
			{
				// show the back buffer
				((FBGraphicsContext *)ctx)->frameBufferItf->SwitchToBuffer(0);
#if (BUFFER_INVALID_REGIONS_ONLY)
				// Copy the rectangle that was modified from buffer 0 to bufer 1
				HTMLRect r = FB_getDirtyRect(ctx);
				if (r.top >=  0)
					FB_CopyFrameBuffer(ctx, 1, 0, r);
#endif
				// redirect draws to the other buffer
				((FBGraphicsContext *)ctx)->currentGBuffer = 1;
			}
		}
		// clear the back buffer
		//rtp_memset(((FBGraphicsContext *)ctx)->frameBufferItf->GetBaseAddress(((FBGraphicsContext *)ctx)->currentGBuffer), 0, ((FBGraphicsContext *)ctx)->frameBufferItf->GetRowBytes() * ((FBGraphicsContext *)ctx)->frameBufferItf->GetHeight());
		WEBC_DEBUG_LOG_TIME_EXIT(TimeInsideDraw,"End Draw"," ")
		WEBC_DEBUG_LOG_TIME_RESET("End Draw")
		WEBC_DEBUG_CACHE_STATISTICS("End Draw")
	}
#endif
	if (((FBGraphicsContext *)ctx)->drawNesting == 1)
	{
		lDoDisplay(ctx);
	}
	if (((FBGraphicsContext *)ctx)->drawNesting)
	{
		((FBGraphicsContext *)ctx)->drawNesting--;
	}

	FB_resetDirtyRect(ctx);
}


/**
 *	This function is called by WebC when it needs to know what the current
 *	clipping region is set to.
 */
void FB_SetClipRect(
	HTMLGraphicsContext **ctx,
	HTMLRect				*rect
	)
{
	FB_setClipRect(ctx, *rect);
}


/**
 *	This function is called by WebC when it needs to convert one of its named
 *	colors to a color that can be passed to native draw functions. The passed
 *	argument is a named color and the return value is a 32 bit color value that
 *	can be passed directly to the draw functions. The returned color may be a
 *	color constant used by the GUI package or an RGB triplet if the GUI package
 *	used RGB triplets for drawing colors.
 */

// HEREHERE - Need grayscale versions, look in peg ports

static WebColor FB_GetColorByIndex(
	HTMLGraphicsContext **ctx,
	int					  color
	)

	{
	static WEBC_UINT32 colorMap [] =
	{
		0x00000000,							 // WGC_BLACK
		0x00FF0000,							 // WGC_RED
		0x0000FF00,							 // WGC_GREEN
		0x00A52A2A,							 // WGC_BROWN
		0x000000FF,							 // WGC_BLUE
		0x00FF00FF,							 // WGC_MAGENTA
		0x0000FFFF,							 // WGC_CYAN
		0x00D3D3D3,							 // WGC_LIGHTGRAY
		0x00A9A9A9,							 // WGC_DARKGRAY
		0x00FFC0CB,							 // WGC_LIGHTRED
		0x0090EE90,							 // WGC_LIGHTGREEN
		0x00FFFF00,							 // WGC_YELLOW
		0x00ADD8E6,							 // WGC_LIGHTBLUE
		0x00FFE0FF,							 // WGC_LIGHTMAGENTA
		0x00E0FFFF,							 // WGC_LIGHTCYAN
		0x00FFFFFF,							 // WGC_WHITE
		0xFFFFFFFF							  // WGC_TRANSPARENT
	};
#if (RENDER8BITINDEX)
WebColor rgbval;
unsigned char grayval;

//		if (colorMap[color] == WGC_TRANSPARENT)
//			;
		grayval = Rgbto4BitGray(HTML_ColorGetRed(colorMap[color]), HTML_ColorGetGreen(colorMap[color]), HTML_ColorGetBlue(colorMap[color]));
		rgbval = (WebColor) grayval;
		rgbval &= 0xff;

		return(rgbval);
#else
	return(colorMap[color]);
#endif
}


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

// HEREHERE - Need grayscale versions, look in peg ports

WebColor FB_RGBToColor(
	HTMLGraphicsContext **ctx,
	HTMLColor			  color,
	HTMLColor			  *error
	)
{
WebColor rgbval;

	if (error && *error)
	{
		*error = 0;
	}

	 rgbval = color;

#if (RENDER8BITINDEX)
unsigned char grayval;

//		if (colorMap[color] == WGC_TRANSPARENT)
//			;
		grayval = Rgbto4BitGray(HTML_ColorGetRed(rgbval), HTML_ColorGetGreen(rgbval), HTML_ColorGetBlue(rgbval));
		rgbval = (WebColor) grayval;
		rgbval &= 0xff;

		return(rgbval);
#else
	return(rgbval);
#endif
}


/**
 *	This function is called by WebC when it needs to convert a natively displayable
 *	color to an RGB triplet in WebCs internal form HTMLColor.
 *
 *	The following macros is available for stuffing R,G, B and A (alpha) components into the
 *	HTMLColor 32 bit internal color representation.
 *		HTML_RGBAToColor (R, G, B, A)
 *	A should always be zero when used in this context.
 */
// HEREHERE - Need grayscale versions, look in peg ports
static HTMLColor FB_ColorToRGB(
	HTMLGraphicsContext **ctx,
	WebColor				colorVal
	)
{
#if (RENDER8BITINDEX)
	return((0xff000000 - (colorVal & HTML_COLOR_ALPHA_MASK)) | (colorVal & ~HTML_COLOR_ALPHA_MASK));
#else
	return(0xff); // HEREHERE need to implemetn a least sqares search of pallet ertries to find the best match.
#endif
}


/**
 *	This function opens and configures a new window for a browser to be
 *	displayed in.
 *
 *	Parameters: HTMLGraphicsContext - graphics associated with window, HTMLBrowserConfig -
 *					configurations of the browser for the window, WEBChar title - title of
 *					the window.
 */

int FB_WindowOpen(
	HTMLGraphicsContext **ctx,
	HTMLBrowserConfig	*pconfig,
	const WebChar		 *title
	)
{
	int		  x, y, width, height;
	WEBC_UINT8 *hWndParent = NULL;


	/* User supplied parameters */
	x		= pconfig->WindowConfig.left;
	y		= pconfig->WindowConfig.top;
	height = pconfig->WindowConfig.height;
	width  = pconfig->WindowConfig.width;

	/* If height or width are 0, use defauts */
	if ((height == 0) || (width == 0))
	{
		pconfig->WindowConfig.left	= x	= 0;
		pconfig->WindowConfig.top   	= y	= 0;
		pconfig->WindowConfig.width 	= width  = WEBC_SCREEN_WIDTH;
		pconfig->WindowConfig.height = height = WEBC_SCREEN_HEIGHT;
	}
	WEBC_CHAR *windowName = pconfig->WindowTitle;
	if (!windowName)
	{
		windowName = (WEBC_CHAR *)"WebC";
	}

	void* windowHndl = FBWM_createWindow(ctx,pconfig);
	currentActivatedWindow = windowHndl;

	if (webc_InitFBBrowserConfig(pconfig, (WEBC_UINT8 *)windowHndl) >= 0)
	{

		pconfig->WindowHandle = windowHndl;
		webc_SetBrowserWindow(pconfig->BrowserHandle, (void *)windowHndl);
		/* Experimental
		 * if (pconfig->is_modal)
		 *  setCapture(windowHndl);*/


		return(0);
	}
//HEREHERE - Can we set the clip region here ??

	return(-1);
}


/**
 *	This function is called by the browser destructor to close the window
 *
 *	Parameters: ctx - graphics context
 */
static void FB_WindowClose(
	HTMLGraphicsContext **ctx
	)
{

	FBWM_closeWindow(ctx);
}


/**
 *	This function is used to move the window. Is this ever used?? I don't know, not sure how it would
 *	end up working either, may be dangerous
 */
void FB_MoveByorTo(
	HTMLGraphicsContext **ctx,
	DISPLAY_INT			x,
	DISPLAY_INT			y,
	WEBC_BOOL			  is_absolute
	)
{

//	HTMLRect rcWindow;
}


/**
 *	This function is called to get the total area of the available screen.
 *
 *	Parameters: ctx - graphics context, pinfo - screeninfo for the browser
 */
void FB_ScreenInfo(
	HTMLGraphicsContext **ctx,
	HTMLScreenInfo		*pinfo
	)
{
	pinfo->availHeight = GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetHeight();
	pinfo->availWidth  = GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetWidth();
	pinfo->height		= GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetHeight();
	pinfo->width		 = GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetWidth();
	pinfo->colorDepth  = GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetDepth();
}


/**
 *	Gets the clipping region of the window
 *
 *	Parameters: ctx - graphics context, rect - rectangle representing the clipping
 *				region.
 */
void FB_GetClipRect(
	HTMLGraphicsContext **ctx,
	HTMLRect				*rect
	)
{
	HTMLRect tmp = FB_getClipRect(ctx);
	rect->bottom = tmp.bottom;
	rect->top	 = tmp.top;
	rect->left	= tmp.left;
	rect->right  = tmp.right;
}


/**
 *	This function must make available the coordinates for webc's drawing
 *	area.  The coordinates will be placed in the rectangle parameter.
 *
 */
void FB_GetCanvasRect(
	HTMLGraphicsContext **ctx,
	HTMLRect				*rectangle
	)
{
#if(WEBC_IMAGE_DRAW_EXTENSIONS)
	FBWIN_GetCanvasRect(ctx,rectangle);
	return;
#else
	rectangle->left	= 0;
	rectangle->top	 = 0;
	rectangle->right  = GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetWidth(); //  -1;
	rectangle->bottom = GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetHeight(); // -1;
	return;
#endif

}

#if (WEBC_SUPPORT_DEBUG_PROFILE)
extern "C" {
extern unsigned long total_time_blocked;
}
#endif

int Target_ProcessInputQueue(void *fn,WEBC_BOOL  block,WEBC_INT32 *exitCode);

static int FB_ProcessInputQueue(
	void		 *fn,
	WEBC_BOOL  block,
	WEBC_INT32 *exitCode
	)
{
	return Target_ProcessInputQueue(fn,block,exitCode);
}

int webc_GuiInitConfig(HTMLBrowserConfig* config)
{
	rtp_memset(config, 0, sizeof(*config));
	return webc_InitFBBrowserConfig (config, 0);
}


int webc_InitFBBrowserConfig(HTMLBrowserConfig *config, WEBC_UINT8 *windowHandle)
{

	graphicsLayerCalls.WindowOpen  = FB_WindowOpen;
	graphicsLayerCalls.WindowClose = FB_WindowClose;
	graphicsLayerCalls.MoveTo		= FB_MoveByorTo;
	graphicsLayerCalls.ScreenInfo  = FB_ScreenInfo;

	graphicsLayerCalls.getCanvasRect = FB_GetCanvasRect;

	graphicsLayerCalls.claimPointer	= FB_ClaimPointer;
	graphicsLayerCalls.releasePointer = FB_ReleasePointer;

	graphicsLayerCalls.invalidateRect = FB_InvalidateRect;
	graphicsLayerCalls.beginDrawing	= FB_BeginDrawing;
	graphicsLayerCalls.getClipRect	 = FB_GetClipRect;
	graphicsLayerCalls.setClipRect	 = FB_SetClipRect;
	graphicsLayerCalls.endDrawing	  = FB_EndDrawing;

	graphicsLayerCalls.colorToRGBA	  = FB_ColorToRGB;
	graphicsLayerCalls.RGBAToColor	  = FB_RGBToColor;
	graphicsLayerCalls.getColorByIndex = FB_GetColorByIndex;

	graphicsLayerCalls.drawText	 = FB_DrawText;
	graphicsLayerCalls.drawTextLen = FB_DrawTextLen;


	graphicsLayerCalls.drawBitmap = FB_DrawBitmap;
	graphicsLayerCalls.drawBitmapStretchedToRect = 0;		//must implement

	graphicsLayerCalls.drawRectangle = FB_DrawRectangle;

	graphicsLayerCalls.createFont					 = FB_CreateFont;
	graphicsLayerCalls.destroyFont					= FB_DestroyFont;
	graphicsLayerCalls.getDefaultFont				= FB_GetDefaultFont;
	graphicsLayerCalls.getNumFontFamilies		  = FB_NumFontFamilies;
	graphicsLayerCalls.getFontFamilyNames		  = FB_GetFontFamilyNames;
	graphicsLayerCalls.getFontFamilyGenericNames = FB_GetFontFamilyGenericNames;

	graphicsLayerCalls.getTextHeight	 = FB_GetTextHeight;
	graphicsLayerCalls.getTextWidth	  = FB_GetTextWidth;
	graphicsLayerCalls.getTextHeightLen = FB_GetTextHeightLen;
	graphicsLayerCalls.getTextWidthLen  = FB_GetTextWidthLen;
	graphicsLayerCalls.getFontHeight	 = FB_GetFontHeight;
	graphicsLayerCalls.getFontBaseline  = FB_GetFontBaseline;

	graphicsLayerCalls.createWebBitmap		  = FB_CreateWebBitmap;
	graphicsLayerCalls.destroyWebBitmap		 = FB_DestroyWebBitmap;
	graphicsLayerCalls.createStretchedBitmap  = FB_CreateStretchedBitmap;
	graphicsLayerCalls.destroyStretchedBitmap = FB_DestroyWebBitmap;
	graphicsLayerCalls.getBitmapWidth			= FB_GetBitmapWidth;
	graphicsLayerCalls.getBitmapHeight		  = FB_GetBitmapHeight;

	graphicsLayerCalls.optionalDrawHorizontalLine = FB_DrawHorizontalLine;
	graphicsLayerCalls.optionalDrawVerticalLine	= FB_DrawVerticalLine;
	graphicsLayerCalls.optionalDrawStyledFrame	 = 0;
	graphicsLayerCalls.optionalDrawBitmapTiled	 = 0;
	graphicsLayerCalls.optionalSetCanvasRect		= 0;
#if(WEBC_IMAGE_DRAW_EXTENSIONS)
	graphicsLayerCalls.createGenericBitmap      = 	FB_CreateGenericBitmap;
#endif
	graphicsLayerCalls.pushCanvas      			=  FB_PushCanvas;
	graphicsLayerCalls.popCanvas     			 = FB_PopCanvas;

#ifdef WEBC_BUFFER_SCROLLING
	graphicsLayerCalls.createBuffer	  = FB_CreateBuffer;
	graphicsLayerCalls.resizeBuffer	  = FB_ResizeBuffer;
	graphicsLayerCalls.destroyBuffer	 = FB_DestroyBuffer;
	graphicsLayerCalls.copyBufferRegion = FB_CopyBufferRegion;
#endif	// WEBC_BUFFER_SCROLLING

	FBHWInterface *fbItf=NULL;
	FBGraphicsContext *ctx=0;

	if (config->graphics)
	{
		// printf("Not reinitializing the frame buffer if graphics already configured !!!!!!!!!!!!\n");
		ctx = (FBGraphicsContext *) config->graphics;
		fbItf = (FBHWInterface *) GET_FBGRAPHICS_CTX(ctx)->frameBufferItf;
	}
	else
	{
		fbItf = FBInitFrameBufferDriver();
	}

	/* If a remote terminal display or headless is active call it to establish it's canvas and allow it to allocate buffer. */
	set_TerminalCanvas(WEBC_SCREEN_HEIGHT, WEBC_SCREEN_WIDTH * BYTES_PER_PIXEL, BYTES_PER_PIXEL);


	if(fbItf)
	{

		if (!ctx)
		{
			WEBC_NEW_VERBOSE(ctx, FBGraphicsContext(config,fbItf->GetWidth(), fbItf->GetHeight()),"webc_InitFBBrowserConfig");
		}
		if (ctx)
		{
			GET_FBGRAPHICS_CTX(ctx)->frameBufferItf = fbItf;
			ctx->graphicsLayerCalls = &graphicsLayerCalls;
			ctx->browserHandle		= config->BrowserHandle;
			config->WindowHandle = windowHandle;
			ctx->windowHandle		 = (WEBC_UINT8 *)config->WindowHandle;
			ctx->drawNesting		  = 0;
#if (USE_DOUBLE_BUFFERING)
			// initially draw to the back buffer
			GET_FBGRAPHICS_CTX(ctx)->currentGBuffer = 1;
#else
			GET_FBGRAPHICS_CTX(ctx)->currentGBuffer = 0;
#endif
			config->graphics			 = (HTMLGraphicsContext **)ctx;
			config->processInputQueue = &fbProcessInputQueueFn;

#ifdef GUARD_BUFFER_LIMITS
			fb0MaxAddress = lGetBaseAddress((HTMLGraphicsContext **)ctx,fbItf,0) + (fbItf->GetRowBytes() * fbItf->GetHeight());
			fb1MaxAddress =  lGetBaseAddress((HTMLGraphicsContext **)ctx,fbItf,1) + (fbItf->GetRowBytes() * fbItf->GetHeight());
#endif

#if (FB_FONTS_STORED_IN_FILES)
			ctx->fontCacheCurrentMemory = 0;
			ctx->fontCacheMaxMemory = DEFAULT_FONT_CACHE_SIZE;
			ctx->fontCacheCurrentUsageIndex = 1;
#endif
		}
#if (FORCE_HEADLESS_DEVICE)
		if (windowHandle == 0)
			start_HeadlessTerminal();
#endif
		return(0);
	}

	return(-1);
}


void webc_DestroyFBBrowserConfig(HTMLGraphicsContext **ctx)
{
	FBGraphicsContext *g = (FBGraphicsContext *)ctx;
	if (g)
	{
		WEBC_DELETE(g);
	}
}


////////////////////// PRIVATE FB Abstraction Functions ///////////////////////



/**
 *	Sets the color for the next element to be drawn.
 *
 *	Param: color - a 32 bit color where the color values are as such transparentRedGreenBlue
 */
void FB_setColor(HTMLGraphicsContext **ctx,
	WEBC_UINT32 color
	)
{
// HEREHERE - Need grayscale version how is it called ?
	WEBC_UINT32 opacitymask	= 0xff000000;
	if (color & opacitymask)
	{
		GET_FBGRAPHICS_CTX(ctx)->hasopacity  = 1;
		GET_FBGRAPHICS_CTX(ctx)->opacity  =  ((color & opacitymask)>>24);
	}
	else
	{
		GET_FBGRAPHICS_CTX(ctx)->hasopacity  = 0;
	}
#if (RENDERRGB24||RENDERRGBA)
	WEBC_UINT32 redmask, greenmask, bluemask;
	redmask	= 0x00ff0000;
	greenmask = 0x0000ff00;
	bluemask  = 0x000000ff;
	GET_FBGRAPHICS_CTX(ctx)->redPen	 = (WEBC_UINT8)((color & redmask) >> (16));  //shift over 16 bits to remove excess
	GET_FBGRAPHICS_CTX(ctx)->greenPen  = (WEBC_UINT8)((color & greenmask) >> (8)); //shift over 8 bits to remove excess
	GET_FBGRAPHICS_CTX(ctx)->bluePen	= (WEBC_UINT8)(color & bluemask);
#elif (RENDER8BITINDEX)
//	GET_FBGRAPHICS_CTX(ctx)->grayPen	 = ((color & 0xff)*8); //Hack for the display tool, need to restore the line below for the board, grayscale display
	GET_FBGRAPHICS_CTX(ctx)->grayPen	 = (color & 0xff);
#endif

}


/**
 *	Sets the color for the next element to be drawn.
 *
 *	Param: red - amount of red, green - amount of green, blue - amount of blue
 */
void FB_setColor(HTMLGraphicsContext **ctx,
	WEBC_UINT8 red,
	WEBC_UINT8 green,
	WEBC_UINT8 blue
	)
{
#if (RENDERRGB24||RENDERRGBA)
	// HEREHERE - Need grayscale version how is it called ?
	GET_FBGRAPHICS_CTX(ctx)->redPen	= red;
	GET_FBGRAPHICS_CTX(ctx)->greenPen = green;
	GET_FBGRAPHICS_CTX(ctx)->bluePen  = blue;
	GET_FBGRAPHICS_CTX(ctx)->hasopacity  = 0;
	GET_FBGRAPHICS_CTX(ctx)->opacity  = 0;
#elif (RENDER8BITINDEX)
	GET_FBGRAPHICS_CTX(ctx)->grayPen	= Rgbto4BitGray(red, green, blue);
#endif
}

static void FB_setColor(HTMLGraphicsContext **ctx,
	WEBC_UINT8 red,
	WEBC_UINT8 green,
	WEBC_UINT8 blue,
	WEBC_UINT8 opacity
	)
{
#if (RENDERRGB24||RENDERRGBA)
	// HEREHERE - Need grayscale version how is it called ?
	GET_FBGRAPHICS_CTX(ctx)->redPen	= red;
	GET_FBGRAPHICS_CTX(ctx)->greenPen = green;
	GET_FBGRAPHICS_CTX(ctx)->bluePen  = blue;
	GET_FBGRAPHICS_CTX(ctx)->opacity  = opacity;
	if (opacity)
		GET_FBGRAPHICS_CTX(ctx)->hasopacity  = 1;
	else
		GET_FBGRAPHICS_CTX(ctx)->hasopacity  = 0;



#elif (RENDER8BITINDEX)
	GET_FBGRAPHICS_CTX(ctx)->grayPen	= Rgbto4BitGray(red, green, blue);
#endif
}

#if (WEBC_SUPPORT_BMP_16BITS)
//The FB_setColor function for the 16bit bmp format (or 16bit image format), the pixel size is 2-bytes and the color information is stored as R:5bits, G:5bits and B:5bits
//we have on X(don't care) bit in the MSB in the high order byte (where the Red and Green color information are)
void FB_setColor(HTMLGraphicsContext **ctx, WEBC_UINT8 color1, WEBC_UINT8 color2)
{
WEBC_UINT8 red, green, blue;
WEBC_UINT8 temp1,temp2;
temp1 = color1 & 0x7C;
temp1 = temp1 << 1;
red = temp1;
temp1 = color1 & 0x03;
temp1 = temp1 << 6;
temp2 = color2 & 0xE0;
temp2 = temp2 >> 2;
green = temp1 | temp2;
temp1 = color2 & 0x1F;
temp1 = temp1 << 3;
blue = temp1 ;
#if (RENDERRGB24||RENDERRGBA)
	// HEREHERE - Need grayscale version how is it called ?
	GET_FBGRAPHICS_CTX(ctx)->redPen	= red;
	GET_FBGRAPHICS_CTX(ctx)->greenPen = green;
	GET_FBGRAPHICS_CTX(ctx)->bluePen  = blue;
#elif (RENDER8BITINDEX)
	GET_FBGRAPHICS_CTX(ctx)->grayPen	= Rgbto4BitGray(red, green, blue);
#endif
}
#endif
/**
 *	Indicates that the next element to be drawn will be transparent or just not drawn.
 */
void FB_setTransparency(HTMLGraphicsContext **ctx)
{
	GET_FBGRAPHICS_CTX(ctx)->transparentPen = true;
}

void FB_getBGColor(HTMLGraphicsContext **ctx, int posX, int posY, WEBC_UINT8 *bgRed, WEBC_UINT8 *bgGreen, WEBC_UINT8 *bgBlue)
{
// HEREHERE - Need grayscale version how is it called ?
#if (RENDERRGB24||RENDERRGBA)
#elif (RENDER8BITINDEX)
#endif
	if (posX > GET_FBGRAPHICS_CTX(ctx)->clipBounds.right)
		posX = GET_FBGRAPHICS_CTX(ctx)->clipBounds.right;
	else if (posX < GET_FBGRAPHICS_CTX(ctx)->clipBounds.left)
		posX = GET_FBGRAPHICS_CTX(ctx)->clipBounds.left;
	if (posY > GET_FBGRAPHICS_CTX(ctx)->clipBounds.bottom)
		posY = GET_FBGRAPHICS_CTX(ctx)->clipBounds.bottom;
	else if (posY < GET_FBGRAPHICS_CTX(ctx)->clipBounds.top)
		posY = GET_FBGRAPHICS_CTX(ctx)->clipBounds.top;
	WEBC_UINT8 *tempPtr = (WEBC_UINT8 *) lGetBaseAddress(ctx,GET_FBGRAPHICS_CTX(ctx)->frameBufferItf,GET_FBGRAPHICS_CTX(ctx)->currentGBuffer) + (posY * GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetRowBytes()) + (posX * 4);
#ifdef GUARD_BUFFER_LIMITS
			  if(&(tempPtr[2]) > (GET_FBGRAPHICS_CTX(ctx)->currentGBuffer ? fb1MaxAddress : fb0MaxAddress))
			  {
				return;
				//exit(0);
			  }
#endif
	*bgRed	= tempPtr[0];
	*bgGreen = tempPtr[1];
	*bgBlue  = tempPtr[2];
}


void FB_drawHorizontalLine(HTMLGraphicsContext **ctx,
	DISPLAY_INT xPos,
	DISPLAY_INT yPos,
	DISPLAY_INT length
	)
{
	register WEBC_UINT8 *colPtr;
#if (RENDERRGB24||RENDERRGBA)
	register WEBC_UINT8 redPen = GET_FBGRAPHICS_CTX(ctx)->redPen, bluePen = GET_FBGRAPHICS_CTX(ctx)->bluePen, greenPen = GET_FBGRAPHICS_CTX(ctx)->greenPen;
	register WEBC_UINT8 opacity = GET_FBGRAPHICS_CTX(ctx)->opacity;
	register WEBC_UINT8 hasopacity = GET_FBGRAPHICS_CTX(ctx)->hasopacity;
	if (hasopacity && opacity==0xff)
		return;
#elif (RENDER8BITINDEX)
	register WEBC_UINT8 grayPen = GET_FBGRAPHICS_CTX(ctx)->grayPen;
#endif
	HTMLRect	clipBounds = GET_FBGRAPHICS_CTX(ctx)->clipBounds;

	if (FBWIN_ClipByWindowClipList(ctx, &clipBounds) < 0)
		return;

	//Draw info off screen frame buffer to be displayed later
	//xPos * 3 represents that there are 3 bytes to every pixel and there for to get to the right pixel we must multiply by three
	if (!GET_FBGRAPHICS_CTX(ctx)->transparentPen)
	{
		if ((xPos > clipBounds.right) || (yPos > clipBounds.bottom) || (yPos < clipBounds.top))
		{
			return;
		}
		if (xPos < clipBounds.left)
		{
			length = length - (clipBounds.left - xPos + 1);
			xPos	= clipBounds.left;
		}

		if (length > (clipBounds.right - xPos))
		{
			length = clipBounds.right - xPos;
		}

#if (RENDERRGB24||RENDERRGBA)
		colPtr = (WEBC_UINT8 *) lGetBaseAddress(ctx,GET_FBGRAPHICS_CTX(ctx)->frameBufferItf,GET_FBGRAPHICS_CTX(ctx)->currentGBuffer) + ((yPos * GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetRowBytes()) + (xPos * 4));

#elif (RENDER8BITINDEX)

		colPtr = (WEBC_UINT8 *) lGetBaseAddress(ctx,GET_FBGRAPHICS_CTX(ctx)->frameBufferItf,GET_FBGRAPHICS_CTX(ctx)->currentGBuffer) + ((yPos * GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetRowBytes()) + xPos);
#endif
#if (RENDER8BITINDEX)
		rtp_memset(colPtr, grayPen, length); // Optimize by filling
#else
		for (int i = length; i > 0; i--, colPtr += BYTES_PER_PIXEL)
		{
#ifdef GUARD_BUFFER_LIMITS
#if (RENDERRGB24||RENDERRGBA)
		  if(&(colPtr[2]) > (GET_FBGRAPHICS_CTX(ctx)->currentGBuffer ? fb1MaxAddress : fb0MaxAddress))
#elif (RENDER8BITINDEX)
		  if(&(colPtr[0]) > (GET_FBGRAPHICS_CTX(ctx)->currentGBuffer ? fb1MaxAddress : fb0MaxAddress))
#endif
		  {
				return;
				//exit(0);
		  }
#endif

#if (RENDERRGB24||RENDERRGBA)
		if (hasopacity)
		{
			colPtr[0] = ALPHATRANSFORM(colPtr[0], redPen,    opacity);
			colPtr[1] = ALPHATRANSFORM(colPtr[1], greenPen,  opacity);
			colPtr[2] = ALPHATRANSFORM(colPtr[2], bluePen,   opacity);
		}
		else
		{
		  colPtr[0] = redPen;
		  colPtr[1] = greenPen;
		  colPtr[2] = bluePen;
		}
#elif (RENDER8BITINDEX)
		  colPtr[0] = grayPen;
#endif
		}
#endif

	}
	else
	{
		GET_FBGRAPHICS_CTX(ctx)->transparentPen = false;
	}
}

#define TRANSPARENTCOLOR 0xff000000
/**
 *	Used to draw a rectangle on the graphics surface.
 *
 *	Params: x - left side position, y - top position, width - width of the rectangle
 *			height - height of the rectangle, fill - whether the rectangle will be filled,
 *			lineColor - the color of the line that will border the rectangle,
 *			fillColor - the color of the interior of the rectangle if it is filled
 */
void FB_drawRectangle(HTMLGraphicsContext **ctx,
	DISPLAY_INT x,
	DISPLAY_INT y,
	DISPLAY_INT width,
	DISPLAY_INT height,
	DISPLAY_INT BorderThickness,
	WEBC_BOOL		  fill,
	WEBC_UINT32 lineColor,
	WEBC_UINT32 fillColor
	)
{
	static int callcount=0;
	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"FB_drawRectangle"," ")

	// Clip the coordinates
	if (!FB_clipRectCoords(ctx, &x,&y, &width, &height))
		return;

	if (fill)
	{
		FB_setColor(ctx, fillColor);
#if (RENDER8BITINDEX)
		// optimize for filling the whole background
		if (width  == GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetWidth() && height == GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetHeight() )
		{
			WEBC_UINT8 * colPtr = (WEBC_UINT8 *) lGetBaseAddress(ctx,GET_FBGRAPHICS_CTX(ctx)->frameBufferItf,GET_FBGRAPHICS_CTX(ctx)->currentGBuffer);
			rtp_memset(colPtr, fillColor, height); // Optimize by filling
		}
		else
#endif
		{
			for (int j = y; j < y + height; j++)
			{
				FB_drawHorizontalLine(ctx, x, j, width);
			}
		}
	}
	if (!fill || (lineColor != fillColor))
	{
		FB_setColor(ctx, lineColor);
		//top line
		for (DISPLAY_INT i = 0; i < BorderThickness; i++)
			FB_drawHorizontalLine(ctx, x, y+i, width);
		//sides
		for (int k = y + BorderThickness; k < y + height - BorderThickness; k++)
		{
			FB_drawHorizontalLine(ctx, x, k, BorderThickness);
			FB_drawHorizontalLine(ctx, x + BorderThickness + width, k, BorderThickness);
		}
		//bottom line
		for (DISPLAY_INT i = 0; i < BorderThickness;i++)
			FB_drawHorizontalLine(ctx, x, y + BorderThickness + height + i, width);
	}
	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"FB_drawRectangle"," ")
}

/**
 *	Used to draw a bitmap on the graphics surface. This method is used when there
 *	does not need to be a restriction on the width and height of a bitmap
 *
 *	Params: bitmapPtr - bitmap to be drawn, dispX - the x position of where
 *			  the bitmap must be drawn, dispY - the y position of where the
 *			bitmap must be drawn.
 */
void FB_drawBitmap(HTMLGraphicsContext **ctx,
	WebGraphBitmapInfo *bitmapPtr,
	DISPLAY_INT		  dispX,
	DISPLAY_INT		  dispY
	)
{
	FB_drawBitmap(ctx, bitmapPtr, dispX, dispY, bitmapPtr->iWidth, bitmapPtr->iHeight);
}


/**
 *	Used to draw a bitmap on the graphics surface. This method is used when there
 *	needs to be a restriction on the width and height of a bitmap
 *
 *	Params: bitmapPtr - bitmap to be drawn, dispX - the x position of where
 *			  the bitmap must be drawn, dispY - the y position of where the
 *			bitmap must be drawn, dispWidth - the width of the bitmap,
 *			dispHeight - the height of the bitmap
 */
void FB_drawBitmap(HTMLGraphicsContext **ctx,
	WebGraphBitmapInfo *bitmapPtr,
	DISPLAY_INT		  dispX,
	DISPLAY_INT		  dispY,
	DISPLAY_INT		  dispWidth,
	DISPLAY_INT		  dispHeight
	)
{
	DISPLAY_INT width, height;
	HTMLRect	clipBounds = GET_FBGRAPHICS_CTX(ctx)->clipBounds;
	WEBC_RGBQUAD *bmpColors = 0;
	int bytes_per_pix_source = 1;
	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"FB_drawBitmap", "")

	if (FBWIN_ClipByWindowClipList(ctx, &clipBounds) < 0)
		return;

	width  = dispWidth;
	height = dispHeight;
	DISPLAY_INT		  UnclippeddispX = dispX;
	DISPLAY_INT		  UnclippeddispY= dispY;

	// Clip
	if (FB_clipRectCoords(ctx, &dispX,&dispY, &width,&height)==0)
		return;

	if (bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_INDEX_8)
	{
		int			 temp = sizeof(WEBC_RGBQUAD);
		bmpColors = (WEBC_RGBQUAD *)WEBC_MALLOC(temp * bitmapPtr->iPaletteSize);
		if (!bmpColors)
			return;
		int n;
		for (n = 0; n < (int)bitmapPtr->iPaletteSize; n++)
		{
			bmpColors[n].rgbRed		= bitmapPtr->pPalette[(n * 3) + 0];
			bmpColors[n].rgbGreen	 = bitmapPtr->pPalette[(n * 3) + 1];
			bmpColors[n].rgbBlue	  = bitmapPtr->pPalette[(n * 3) + 2];
			bmpColors[n].rgbReserved = 0;
		}
		bytes_per_pix_source = 1;
	}
	else if (bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
		bytes_per_pix_source = 3;
	else if (bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_RGBA_32)
		bytes_per_pix_source = 4;


	WEBC_UINT8 transRed, transBlue, transGreen, transIndex;
	if (bitmapPtr->bHasTransparentColor)
	{
		transRed	= bitmapPtr->uTransparentRed;
		transGreen = bitmapPtr->uTransparentGreen;
		transBlue  = bitmapPtr->uTransparentBlue;
		transIndex = (WEBC_UINT8) bitmapPtr->iTransparentIndex;

	}
	const WEBC_UINT8 *rowDataPtr		 = (bitmapPtr->pData);
	int				  paletteSizeNeeded = (bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_INDEX_8) ? bitmapPtr->iPaletteSize : 0;

	int				  length = 0;
	int				  startx = 0;
	const WEBC_UINT8 *current, *next;
#if (WEBC_SUPPORT_BMP_16BITS)
	if (bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_RGB_24 || bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_INDEX_8 || bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_RGB_16 || bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_RGBA_32)
#else
	if (bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_RGB_24 || bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_INDEX_8)
#endif
	{
		// if(bitmapPtr->bHasTransparentColor)
		// if (1)
		{
			// clip the destination on the left
			if (dispX > UnclippeddispX)
					rowDataPtr += bytes_per_pix_source * (dispX - UnclippeddispX);
			if (dispY > UnclippeddispY)
					rowDataPtr += bitmapPtr->iBytesPerRow * (dispY - UnclippeddispY);
#if (0)
			if(clipBounds.left > dispX)
			{
				width -= clipBounds.left - dispX;
				if(width <= 0) // skip if it's entirely off the screen
					return;
				rowDataPtr += bytes_per_pix_source * (clipBounds.left - dispX);
				dispX = clipBounds.left;
			}
			// clip on the top
			if(clipBounds.top > dispY)
			{
				height -= clipBounds.top - dispY;
				if(height <= 0)
					return;
				rowDataPtr += bitmapPtr->iBytesPerRow * (clipBounds.top - dispY);
				dispY = clipBounds.top;
			}
#endif		
			for (int y = 0; y < height; y++, rowDataPtr += (bitmapPtr->iBytesPerRow))
			{
				current = rowDataPtr;
				length  = 1;
				startx  = 0;
			    if (bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_INDEX_8)
			    {
				    	next	 = current + 1;
			    	for (int x = 0; x < width; x++)
			    	{
			    		if ((next[0] != current[0]) || (x == width - 1))
			    		{
			    			int index = current[0];
			    			if (bitmapPtr->bHasTransparentColor && (current[0] == bitmapPtr->iTransparentIndex))
			    				FB_setTransparency(ctx);
			    			else if (bitmapPtr->iPaletteSize == 0) // Add 7/19/2010 to support 8 bit color with no Palette
								FB_setColor(ctx, index, index, index);
							else
								FB_setColor(ctx, bmpColors[index].rgbRed, bmpColors[index].rgbGreen, bmpColors[index].rgbBlue);
			    			FB_drawHorizontalLine(ctx, dispX + startx, dispY + y, length);
			    			length = 1;
			    			startx = x + 1;
			    		}
			    		else
			    			length += 1;
			    		current += 1;
			    		if (x != width - 1)
			    			next += 1;
			    	}
			    }
#if (WEBC_SUPPORT_BMP_16BITS)
			    else if (bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_RGB_16)
			    {
					next	 = current + 2;
			    	for (int x = 0; x < width; x++)
			    	{
			    		if ((next[0] != current[0]) || (next[1] != current[1]) || (x == width - 1))
			    		{
			    			if (bitmapPtr->bHasTransparentColor &&
			    				 (current[0] == transRed) &&
			    				 (current[1] == transGreen) &&
			    				 (current[2] == transBlue))
			    			{
			    				FB_setTransparency(ctx);
			    			}
			    			else
								FB_setColor(ctx, current[0], current[1]);
			    				//FB_setColor16(ctx, current[0], current[1]);

			    			FB_drawHorizontalLine(ctx, dispX + startx, dispY + y, length);
			    			length = 1;
			    			startx = x + 1;
			    		}
			    		else
			    			length += 1;
			    		current += 2;
			    		if (x != width - 1)
			    			next += 2;
			    	}
			    }
#endif
			    else if (bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
			    {
			    	next	 = current + 3;
			    	for (int x = 0; x < width; x++)
			    	{
			    		if ((next[0] != current[0]) || (next[1] != current[1]) || (next[2] != current[2]) || (x == width - 1))
			    		{
			    			if (bitmapPtr->bHasTransparentColor &&
			    				 (current[0] == transRed) &&
			    				 (current[1] == transGreen) &&
			    				 (current[2] == transBlue))
			    			{
			    				FB_setTransparency(ctx);
			    			}
			    			else
			    				FB_setColor(ctx, current[0], current[1], current[2]);

			    			FB_drawHorizontalLine(ctx, dispX + startx, dispY + y, length);
			    			length = 1;
			    			startx = x + 1;
			    		}
			    		else
			    			length += 1;
			    		current += 3;
			    		if (x != width - 1)
			    			next += 3;
			    	}
			    }
			    else if (bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_RGBA_32)
			    {
			    	next	 = current + 4;
			    	for (int x = 0; x < width; x++)
			    	{
			    		if ((next[0] != current[0]) || (next[1] != current[1]) || (next[2] != current[2]) || (next[3] != current[3]) || (x == width - 1))
			    		{
			    			if (1) //current[3])
			    			{
			    				// Pass in 0xff - opacity.. how png is encoded at least
								FB_setColor(ctx, current[0], current[1], current[2], 0xff- current[3]);
			    				FB_drawHorizontalLine(ctx, dispX + startx, dispY + y, length);
			    			}
			    			else
							{
			    				FB_setColor(ctx, current[0], current[1], current[2]);
			    				FB_drawHorizontalLine(ctx, dispX + startx, dispY + y, length);
							}
			    			length = 1;
			    			startx = x + 1;
			    		}
			    		else
			    			length += 1;
			    		current += 4;
			    		if (x != width - 1)
			    			next += 4;
			    	}
			    }

			}
		}
	}
	GET_FBGRAPHICS_CTX(ctx)->hasopacity  = 0;
	if (bmpColors)
	{
		WEBC_FREE((void *)bmpColors);
	}
	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"FB_drawBitmap", "")
}




/**
 *	Sets the clipping region for the graphics surface.
 *	The clipping region is the bounding region where can not be drawn.
 *
 *	Params: rect - the new rect for the clipping region
 */
void FB_setClipRect(HTMLGraphicsContext **ctx,
	HTMLRect rect
	)
{
	GET_FBGRAPHICS_CTX(ctx)->clipBounds.bottom = rect.bottom;
	GET_FBGRAPHICS_CTX(ctx)->clipBounds.top	 = rect.top;
	GET_FBGRAPHICS_CTX(ctx)->clipBounds.left	= rect.left;
	GET_FBGRAPHICS_CTX(ctx)->clipBounds.right  = rect.right;
}


/**
 *	Gets the clipping region for the graphics surface. Returns the
 *	bounding region for the non-drawing area.
 *
 *	Return: returns the rectangle representing where the class is
 *			limited in drawing.
 */
HTMLRect FB_getClipRect(HTMLGraphicsContext **ctx)
{
	return(GET_FBGRAPHICS_CTX(ctx)->clipBounds);
}


/**
 *	This method adds this rectangle to the overall region that
 *	needs to be redrawn or redisplayed.
 *
 *	Params: rect - the rectangle to add to the overall region to be redrawn
 */
void FB_addDirtyRect(HTMLGraphicsContext **ctx,
	HTMLRect passedRect
	)
{
	HTMLRect rect = passedRect;

	if(GET_FBGRAPHICS_CTX(ctx)->invalidRect.bottom == -1)
	{
			GET_FBGRAPHICS_CTX(ctx)->invalidRect.bottom = rect.bottom;
			GET_FBGRAPHICS_CTX(ctx)->invalidRect.right = rect.right;
			GET_FBGRAPHICS_CTX(ctx)->invalidRect.left = rect.left;
			GET_FBGRAPHICS_CTX(ctx)->invalidRect.top = rect.top;
	}
	else
	{
		if (rect.bottom > GET_FBGRAPHICS_CTX(ctx)->invalidRect.bottom)
		{
			GET_FBGRAPHICS_CTX(ctx)->invalidRect.bottom = rect.bottom;
		}
		if (rect.right > GET_FBGRAPHICS_CTX(ctx)->invalidRect.right)
		{
			GET_FBGRAPHICS_CTX(ctx)->invalidRect.right = rect.right;
		}
		if (rect.left < GET_FBGRAPHICS_CTX(ctx)->invalidRect.left)
		{
			GET_FBGRAPHICS_CTX(ctx)->invalidRect.left = rect.left;
		}
		if (rect.top < GET_FBGRAPHICS_CTX(ctx)->invalidRect.top)
		{
			GET_FBGRAPHICS_CTX(ctx)->invalidRect.top = rect.top;
		}
    }
}


/**
 *	Returns the area that needs to be redrawn if beign displayed
 *
 *	Return: the area that needs to be redrawn
 */
HTMLRect FB_getDirtyRect(HTMLGraphicsContext **ctx)
{
	return(GET_FBGRAPHICS_CTX(ctx)->invalidRect);
}


/**
 *	Resets the invalid region indicating that all displays are up to date for
 *	this graphics class.
 */
void FB_resetDirtyRect(HTMLGraphicsContext **ctx)
{
	GET_FBGRAPHICS_CTX(ctx)->invalidRect.bottom = -1;
	GET_FBGRAPHICS_CTX(ctx)->invalidRect.left	= -1;
	GET_FBGRAPHICS_CTX(ctx)->invalidRect.right  = -1;
	GET_FBGRAPHICS_CTX(ctx)->invalidRect.top	 = -1;
}


// Clip x, y, w, h to the current clip bounds, return 1 if any of it is visible
static int FB_clipRectCoords(HTMLGraphicsContext **ctx, DISPLAY_INT *x,DISPLAY_INT *y, DISPLAY_INT *w,DISPLAY_INT *h)
{
	HTMLRect	clipBounds = GET_FBGRAPHICS_CTX(ctx)->clipBounds;

	if (FBWIN_ClipByWindowClipList(ctx, &clipBounds) < 0)
		return 0;

	DISPLAY_INT sx, sy,  sw, sh;
	sx=*x;sy=*y;sw=*w;sh=*h;
	
	// clip the destination
	if(clipBounds.right < *x+*w)
		*w = clipBounds.right-*x+1;
	if(clipBounds.bottom < *y+*h)
		*h = clipBounds.bottom-*y+1;
	if(clipBounds.left > *x)
	{
		*w -= (clipBounds.left - *x);
		*x = clipBounds.left;
	}
	if(clipBounds.top > *y)
	{
		*h -= (clipBounds.top - *y);
		*y = clipBounds.top;
	}
	if (*h <= 0 || *w <= 0)
		return 0;
	return 1;
}

static int FB_clipRectCoords(HTMLGraphicsContext **ctx, HTMLRect* rect)
{
	DISPLAY_INT y= rect->top;
	DISPLAY_INT x= rect->left;
	DISPLAY_INT h= rect->bottom-y+1;
	DISPLAY_INT w= rect->right-x+1;

	if (FB_clipRectCoords(ctx, &x,&y, &w, &h)==0)
		return 0; // Completely clipped
	rect->top=y;
	rect->bottom=y+h-1;
	rect->left=x;
	rect->right=x+w-1;
	return 1;
}

// Implementation of FB_createStretchedBitmap provided here, it is too large to leave in the main body of code
static void *FB_createStretchedBitmap(
	HTMLGraphicsContext **ctx,
	void					 *b,
	DISPLAY_INT			w,
	DISPLAY_INT			h
	)
{
	double xscale, yscale;							  /* zoom scale factors */
	int	 i, j, k, channel;							/* loop variables */
	int	 n;												/* pixel number */
	double center, left, right;						/* filter calculation variables */
	double width, fscale, weight;					 /* filter calculation variables */
	//filters
	double fwidth = bell_support;

	double (*filterf)(double) = bell_filter;


	WebGraphBitmapInfo *sourceBmp  = (WebGraphBitmapInfo *)b;
	WebGraphBitmapInfo *resizedBmp = (WebGraphBitmapInfo *)WEBC_MALLOC_VERBOSE(sizeof(WebGraphBitmapInfo),"StretchedBitmap");

	if (!resizedBmp)
	{
		return(0);
	}

	*resizedBmp			= *sourceBmp;
	resizedBmp->iWidth  = w;
	resizedBmp->iHeight = h;
// HEREHERE
	if (resizedBmp->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
	{
		resizedBmp->iBytesPerRow = (WEBC_SIZE)resizedBmp->iWidth * 3;
	}
	else
	{
		resizedBmp->iBytesPerRow = (WEBC_SIZE)resizedBmp->iWidth;
	}
	resizedBmp->pData = (WEBC_UINT8 *)WEBC_MALLOC_VERBOSE(resizedBmp->iBytesPerRow * resizedBmp->iHeight,"StretchedBitmap Data");
	if (!resizedBmp->pData)
	{
		return(0);
	}

	if (sourceBmp->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
	{
		WebGraphBitmapInfo *tmp = (WebGraphBitmapInfo *)WEBC_MALLOC(sizeof(WebGraphBitmapInfo));
		*tmp				  = *sourceBmp;
		tmp->iWidth		 = w;
		tmp->iBytesPerRow = (WEBC_SIZE)w * 3;			//3 means 3 bytes per pixel (3*8) = 24 bit depth

		tmp->pData = (WEBC_UINT8 *)WEBC_MALLOC(tmp->iBytesPerRow * tmp->iHeight);
		if (!tmp->pData)
		{
			return(0);
		}

		//precalculate contributions for rows
		xscale = (double)resizedBmp->iWidth / (double)sourceBmp->iWidth;
		yscale = (double)resizedBmp->iHeight / (double)sourceBmp->iHeight;
		CLIST *contrib = (CLIST *)WEBC_CALLOC(resizedBmp->iWidth, sizeof(CLIST));

		if (xscale < 1.0)
		{
			width  = fwidth / xscale;
			fscale = 1.0 / xscale;
			for (i = 0; i < resizedBmp->iWidth; ++i)
			{
				contrib[i].n = 0;
				contrib[i].p = (CONTRIB *)WEBC_CALLOC((int)(width * 2 + 1),
																  sizeof(CONTRIB));
				center = (double)i / xscale;
				left	= ceil(center - width);
				right  = floor(center + width);
				for (j = (int)left; j <= right; ++j)
				{
					weight = center - (double)j;
					weight = (*filterf)(weight / fscale) / fscale;
					if (j < 0)
					{
						n = -j;
					}
					else if (j >= sourceBmp->iWidth)
					{
						n = (sourceBmp->iWidth - j) + sourceBmp->iWidth - 1;
					}
					else
					{
						n = j;
					}
					k = contrib[i].n++;
					contrib[i].p[k].pixel  = n;
					contrib[i].p[k].weight = weight;
				}
			}
		}
		else
		{
			for (i = 0; i < resizedBmp->iWidth; ++i)
			{
				contrib[i].n = 0;
				contrib[i].p = (CONTRIB *)WEBC_CALLOC((int)(fwidth * 2 + 1),
																  sizeof(CONTRIB));
				center = (double)i / xscale;
				left	= ceil(center - fwidth);
				right  = floor(center + fwidth);
				for (j = (int)left; j <= right; ++j)
				{
					weight = center - (double)j;
					weight = (*filterf)(weight);
					if (j < 0)
					{
						n = -j;
					}
					else if (j >= sourceBmp->iWidth)
					{
						n = (sourceBmp->iWidth - j) + sourceBmp->iWidth - 1;
					}
					else
					{
						n = j;
					}
					k = contrib[i].n++;
					contrib[i].p[k].pixel  = n;
					contrib[i].p[k].weight = weight;
				}
			}
		}

		/* apply filter to zoom horizontally from src to tmp */
		WEBC_UINT8 *sourceRow;

		for (k = 0; k < tmp->iHeight; ++k)
		{
			sourceRow = (WEBC_UINT8 *)sourceBmp->pData + (k * sourceBmp->iBytesPerRow);
			for (channel = 0; channel < 3; channel++)
			{
				for (i = 0; i < tmp->iWidth; ++i)
				{
					weight = 0.0;
					for (j = 0; j < contrib[i].n; ++j)
					{
						weight = weight + (sourceRow[(contrib[i].p[j].pixel * 3) + channel] * contrib[i].p[j].weight);
					}
					((WEBC_UINT8 *)tmp->pData)[k * tmp->iBytesPerRow + (i * 3) + channel] = (WEBC_UINT8) weight;
				}
			}
		}
		/* free the memory allocated for horizontal filter weights */
		for (i = 0; i < tmp->iWidth; ++i)
		{
			WEBC_FREE((void *)contrib[i].p);
		}
		WEBC_FREE((void *)contrib);

		/* pre-calculate filter contributions for a column */
		contrib = (CLIST *)WEBC_CALLOC(resizedBmp->iHeight, sizeof(CLIST));
		if (yscale < 1.0)
		{
			width  = fwidth / yscale;
			fscale = 1.0 / yscale;
			for (i = 0; i < resizedBmp->iHeight; ++i)
			{
				contrib[i].n = 0;
				contrib[i].p = (CONTRIB *)WEBC_CALLOC((int)(width * 2 + 1),
																  sizeof(CONTRIB));
				center = (double)i / yscale;
				left	= ceil(center - width);
				right  = floor(center + width);
				for (j = (int)left; j <= right; ++j)
				{
					weight = center - (double)j;
					weight = (*filterf)(weight / fscale) / fscale;
					if (j < 0)
					{
						n = -j;
					}
					else if (j >= tmp->iHeight)
					{
						n = (tmp->iHeight - j) + tmp->iHeight - 1;
					}
					else
					{
						n = j;
					}
					k = contrib[i].n++;
					contrib[i].p[k].pixel  = n;
					contrib[i].p[k].weight = weight;
				}
			}
		}
		else
		{
			for (i = 0; i < resizedBmp->iHeight; ++i)
			{
				contrib[i].n = 0;
				contrib[i].p = (CONTRIB *)WEBC_CALLOC((int)(fwidth * 2 + 1),
																  sizeof(CONTRIB));
				center = (double)i / yscale;
				left	= ceil(center - fwidth);
				right  = floor(center + fwidth);
				for (j = (int)left; j <= right; ++j)
				{
					weight = center - (double)j;
					weight = (*filterf)(weight);
					if (j < 0)
					{
						n = -j;
					}
					else if (j >= tmp->iHeight)
					{
						n = (tmp->iHeight - j) + tmp->iHeight - 1;
					}
					else
					{
						n = j;
					}
					k = contrib[i].n++;
					contrib[i].p[k].pixel  = n;
					contrib[i].p[k].weight = weight;
				}
			}
		}

		/* apply filter to zoom vertically from tmp to dst */
		WEBC_UINT8 *colPtr;
		for (k = 0; k < resizedBmp->iWidth; ++k)
		{
			for (channel = 0; channel < 3; channel++)
			{
				colPtr = (WEBC_UINT8 *)tmp->pData + (k * 3);
				for (i = 0; i < resizedBmp->iHeight; ++i)
				{
					weight = 0.0;
					for (j = 0; j < contrib[i].n; ++j)
					{
						weight = weight + (colPtr[contrib[i].p[j].pixel * tmp->iBytesPerRow + channel] * contrib[i].p[j].weight);
					}
					((WEBC_UINT8 *)resizedBmp->pData)[i * resizedBmp->iBytesPerRow + (k * 3) + channel] = (WEBC_UINT8) weight;
				}
			}
		}

		/* free the memory allocated for vertical filter weights */
		for (i = 0; i < resizedBmp->iHeight; ++i)
		{
			WEBC_FREE((void *)contrib[i].p);
		}
		WEBC_FREE((void *)contrib);

		WEBC_FREE((void *)tmp->pData);
		WEBC_FREE((void *)tmp);
	}
	else		//this probably needs to be tweaked
	{
		//(resizedBmp->pData) + (resizedBmp->iBytesPerRow * resizedBmp->iHeight));
		float xIncrement = (float)sourceBmp->iWidth / (float)resizedBmp->iWidth;
	 	float yIncrement = (float)sourceBmp->iHeight / (float)resizedBmp->iHeight;

		WEBC_UINT8 *srcRowPtr, *srcColPtr;

		WEBC_UINT8 *destRowPtr, *destColPtr;

//		WEBC_UINT8  color1, color2, color3, color4, avgColor;
//		WEBC_UINT32 tmpColor;
		int			xCount, yCount;

		destRowPtr = (WEBC_UINT8 *)resizedBmp->pData;
		for (int i = 0; i < resizedBmp->iHeight; i++)
		{
			yCount = (int)(yIncrement * i) < sourceBmp->iHeight ? (int)(yIncrement * i) : resizedBmp->iHeight;
			//the new value of the srcRowPtr will be the new source row
			srcRowPtr  = (WEBC_UINT8 *)sourceBmp->pData + (yCount * (sourceBmp->iBytesPerRow));
			srcColPtr  = srcRowPtr;
			destColPtr = destRowPtr;

			for (int j = 0; j < resizedBmp->iWidth; j++)
			{
				xCount = (((int)(j * xIncrement)) < sourceBmp->iWidth) ? ((int)(j * xIncrement)) : sourceBmp->iWidth;

				srcColPtr = srcRowPtr + xCount;

				destColPtr[0] = srcColPtr[0];

				destColPtr += 1;
			}
			destRowPtr += resizedBmp->iBytesPerRow;
		}
	}

	if (sourceBmp->iPaletteSize != 0)
	{
		resizedBmp->pPalette = (WEBC_UINT8 *)WEBC_MALLOC((sourceBmp->iPaletteSize * 3));					  //the 3 comes from the fact that each entry in a the pallete is three bytes, so the numbner of entries times three gives the correct number
		if (!resizedBmp->pPalette)
		{
			return(0);
		}
		rtp_memcpy((void *)resizedBmp->pPalette, (void *)sourceBmp->pPalette, (sourceBmp->iPaletteSize * 3));
	}
	return((void *)resizedBmp);
}


#if (MAPRGBTOGRAY||MAPINDEXED8TOINDEXED4)


// This is the windows value with the debug visualization tool. The one below is for windows.. let's see what happens.
#define RGBTOGREY(r,g,b)        ((((unsigned long)(r)) + (((unsigned long)(r)) << 2) + \
                                 (((unsigned long)(g)) << 3) + ((unsigned long)(b)) + \
                                 (((unsigned long)(b)) << 1)) >> 8)

// This is the TI value with the debug visualization tool. The one below is for windows.. let's see what happens.
//#define RGBTOGREY(r,g,b)        ((((unsigned long)(r)) + (((unsigned long)(r)) << 2) + \
//                                 (((unsigned long)(g)) << 3) + ((unsigned long)(b)) + \
//                                 (((unsigned long)(b)) << 1)) >> 5)



// grayLevel8bit = (red * 5 + green * 8 + blue * 3) / 16
// grayLevel4bit = ((red * 5 + green * 8 + blue * 3) / 16)/32
//
WEBC_UINT8 Rgbto4BitGray(WEBC_UINT8 r, WEBC_UINT8 g, WEBC_UINT8 b)
{
unsigned long R = r;
unsigned long G = g;
unsigned long MYB = b;
unsigned long GREY;
	GREY = RGBTOGREY(R,G,MYB);
	return(WEBC_UINT8)(GREY&0xff);
}
#endif

#if(WEBC_IMAGE_DRAW_EXTENSIONS)

/*---------------------------------------------------------------------------*/
static int FB_CreateGenericBitmap (
		void* webCBitmapSrc,
		HTMLGraphicsContext** ctx,
		WebGraphBitmapInfo** genericBitMapDst)
{
	WebGraphBitmapInfo *wcBmp = (WebGraphBitmapInfo*) webCBitmapSrc;
	WEBC_SIZE bufSize;
#define DO_RGBA 0

#if (DO_RGBA)
	int bytesperpix = 4;
#else
	int bytesperpix = 3;
#endif

	int srcbytesperpix;
		if (wcBmp->pixelFormat==WB_PIXEL_FORMAT_RGB_24)
			srcbytesperpix=3;
		else // if (wcBmp->pixelFormat==WB_PIXEL_FORMAT_RGBA_32)
			srcbytesperpix=4;

	*genericBitMapDst = (WebGraphBitmapInfo *) WEBC_MALLOC_VERBOSE(sizeof(WebGraphBitmapInfo),"FB_CreateGenericBitmap");

	// TODO need to make this work for all image depths
	(*genericBitMapDst)->iWidth = wcBmp->iWidth;
	(*genericBitMapDst)->iHeight = wcBmp->iHeight;
	(*genericBitMapDst)->iBytesPerRow = wcBmp->iWidth * bytesperpix;
#if (DO_RGBA)
	(*genericBitMapDst)->pixelFormat = WB_PIXEL_FORMAT_RGBA_32;
#else
	(*genericBitMapDst)->pixelFormat = WB_PIXEL_FORMAT_RGB_24;
#endif
	(*genericBitMapDst)->bHasTransparentColor = WEBC_FALSE;
	(*genericBitMapDst)->iTransparentIndex = 0;
	(*genericBitMapDst)->uTransparentRed = 0;
	(*genericBitMapDst)->uTransparentGreen = 0;
	(*genericBitMapDst)->uTransparentBlue = 0;
	(*genericBitMapDst)->iPaletteSize = 0;
	(*genericBitMapDst)->pPalette = 0;
	bufSize = (*genericBitMapDst)->iBytesPerRow * (*genericBitMapDst)->iHeight;
	(*genericBitMapDst)->pData = (WEBC_UINT8 *) WEBC_MALLOC_VERBOSE(bufSize,"FB_CreateGenericBitmap");

	//memcpy((void *)((*genericBitMapDst)->pData), (void *)(wcBmp->bitmapData + sizeof(BITMAPINFOHEADER)), bufSize);
		WEBC_UINT8* srcPtr = (WEBC_UINT8 *)wcBmp->pData;
		WEBC_UINT8* dstPtr = (WEBC_UINT8 *)((*genericBitMapDst)->pData);
		WEBC_UINT8* rowPtr = dstPtr;
		WEBC_UINT8* rowSrcPtr = srcPtr;
		for (int y = 0; y < (*genericBitMapDst)->iHeight; y++, rowPtr += (*genericBitMapDst)->iBytesPerRow,rowSrcPtr+= (wcBmp)->iBytesPerRow)
		{
			dstPtr = rowPtr;
			srcPtr = rowSrcPtr;
			for (int x = 0; x < (*genericBitMapDst)->iWidth; x++, srcPtr += srcbytesperpix, dstPtr += bytesperpix)
			{
				dstPtr[0] = srcPtr[0];
				dstPtr[1] = srcPtr[1];
				dstPtr[2] = srcPtr[2];
#if (DO_RGBA)
				dstPtr[3] = srcPtr[3];
#endif
			}
		}


	return 0;
}

#endif

// These are used for save under window and alpha blending. We need to publish them to the API.
void FB_ReadFrameBuffer(HTMLGraphicsContext **ctx,WEBC_UINT8 *pdata,DISPLAY_INT xPos,DISPLAY_INT yPos,DISPLAY_INT width,DISPLAY_INT height)
{
	if (pdata)
	{
	register WEBC_UINT8 *colPtr;
		for (int h = 0; h < height; h++,yPos++)
		{
			colPtr = (WEBC_UINT8 *) lGetBaseAddress(ctx,GET_FBGRAPHICS_CTX(ctx)->frameBufferItf,GET_FBGRAPHICS_CTX(ctx)->currentGBuffer) + ((yPos * GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetRowBytes()) + (xPos * BYTES_PER_PIXEL));
				for (int i = 0; i < width; i++, colPtr += BYTES_PER_PIXEL)
					for (int j = 0; j < BYTES_PER_PIXEL; j++)
						*pdata++ = colPtr[j];
		}
	}
}
void FB_WriteFrameBuffer(HTMLGraphicsContext **ctx, WEBC_UINT8 *pdata, DISPLAY_INT xPos,DISPLAY_INT yPos,DISPLAY_INT width,DISPLAY_INT height)
{
register WEBC_UINT8 *colPtr;
	for (int h = 0; h < height; h++,yPos++)
	{
		colPtr = (WEBC_UINT8 *) lGetBaseAddress(ctx,GET_FBGRAPHICS_CTX(ctx)->frameBufferItf,GET_FBGRAPHICS_CTX(ctx)->currentGBuffer) + ((yPos * GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetRowBytes()) + (xPos * BYTES_PER_PIXEL));
		for (int i = 0; i < width; i++, colPtr += BYTES_PER_PIXEL)
			for (int j = 0; j < BYTES_PER_PIXEL; j++)
				colPtr[j] = *pdata++;
	}
}
/*---------------------------------------------------------------------------*/
typedef struct
{
	HTMLRect	rect;
	WEBC_UINT8 *savedBackStore;
//	Rect    rect;	// Area being pushed. This is the only area where drawing will take place but the coordinate will
//	Graphics*            windowGraphics;
//	Bitmap*              bufferImage;
//	Graphics*            bufferGraphics;
//	Graphics*            outputGraphics;
} WebcFBPushedWindowGraphic;

static void * FB_PushCanvas(HTMLGraphicsContext** ctx, HTMLRect* rect)
{
WEBC_UINT8 *newBackStore;
WebcFBPushedWindowGraphic *retPrevCtx;
#if (RENDERRGB24||RENDERRGBA)
	int bytesperpix = 4;
#elif (RENDER8BITINDEX)
	int bytesperpix = 1;
#endif

	retPrevCtx = (WebcFBPushedWindowGraphic *) WEBC_MALLOC_VERBOSE(sizeof(WebcFBPushedWindowGraphic),"PushedCanvas");
	if (!retPrevCtx)
		return(void *) 0;
	retPrevCtx->rect = *rect;
	if (!FB_clipRectCoords(ctx, &retPrevCtx->rect))
	{ // Clip to our region, if we are completely clipped then do nothing.
		WEBC_FREE(retPrevCtx);
		return(void *) 0;
	}
	else
	{
		newBackStore = (WEBC_UINT8 *) WEBC_MALLOC_VERBOSE(((FBGraphicsContext *)ctx)->frameBufferItf->GetRowBytes() * ((FBGraphicsContext *)ctx)->frameBufferItf->GetHeight(),"WindowBackStore");
		if (!newBackStore)
		{
			WEBC_FREE(retPrevCtx);
			return(void *) 0;
		}
	}
	retPrevCtx->savedBackStore = newBackStore;

	rtp_memcpy(retPrevCtx->savedBackStore, lGetBaseAddress(ctx,GET_FBGRAPHICS_CTX(ctx)->frameBufferItf,GET_FBGRAPHICS_CTX(ctx)->currentGBuffer), ((FBGraphicsContext *)ctx)->frameBufferItf->GetRowBytes() * ((FBGraphicsContext *)ctx)->frameBufferItf->GetHeight());
//	rtp_memset(lGetBaseAddress(ctx,GET_FBGRAPHICS_CTX(ctx)->frameBufferItf,GET_FBGRAPHICS_CTX(ctx)->currentGBuffer), 0x00, ((FBGraphicsContext *)ctx)->frameBufferItf->GetRowBytes() * ((FBGraphicsContext *)ctx)->frameBufferItf->GetHeight());

	return((void *)retPrevCtx);
}
static void FB_Blend(HTMLGraphicsContext** ctx, WebcFBPushedWindowGraphic *PrevCtx, int opacity);
static void FB_PopCanvas(HTMLGraphicsContext** ctx, void *pvretPrevCtx, int opacity)
{
WebcFBPushedWindowGraphic *PrevCtx;

	if (!pvretPrevCtx)
		return;
	PrevCtx = (WebcFBPushedWindowGraphic *) pvretPrevCtx;
	FB_Blend(ctx, PrevCtx, opacity);
	rtp_memcpy(lGetBaseAddress(ctx,GET_FBGRAPHICS_CTX(ctx)->frameBufferItf,GET_FBGRAPHICS_CTX(ctx)->currentGBuffer), PrevCtx->savedBackStore, ((FBGraphicsContext *)ctx)->frameBufferItf->GetRowBytes() * ((FBGraphicsContext *)ctx)->frameBufferItf->GetHeight());
	WEBC_FREE(PrevCtx->savedBackStore);
	WEBC_FREE(PrevCtx);
}

static WEBC_UINT8 ALPHATRANSFORM(WEBC_UINT8 to, WEBC_UINT8  from, int opacity)
{
unsigned short w1,w2;
WEBC_UINT8 c;

	w1 = to;
	w2 = from;
	static int outcount = 0;

	w2 *= opacity;
	w1 *= (255-opacity);
	w1 += w2;
	w1>>=8;
	c = (unsigned char) w1;
	outcount += 1;
	return c;
}

static void FB_Blend(HTMLGraphicsContext** ctx, WebcFBPushedWindowGraphic *PrevCtx, int opacity)
{
	WEBC_UINT8* destRowPtr = PrevCtx->savedBackStore;
	WEBC_UINT8* srcRowPtr = lGetBaseAddress(ctx,GET_FBGRAPHICS_CTX(ctx)->frameBufferItf,GET_FBGRAPHICS_CTX(ctx)->currentGBuffer);
	WEBC_UINT8* srcColPtr,*destColPtr;
	int y,x,width;
#if (RENDERRGB24||RENDERRGBA)
	int bytesperpix = 4;
#elif (RENDER8BITINDEX)
	int bytesperpix = 1;
#endif
	y = PrevCtx->rect.top;
	x = PrevCtx->rect.left;
	width = PrevCtx->rect.right - PrevCtx->rect.left + 1;


	for (y = 0; y < WEBC_SCREEN_HEIGHT; y++, destRowPtr += (WEBC_SCREEN_WIDTH*bytesperpix), srcRowPtr += (WEBC_SCREEN_WIDTH*bytesperpix))
	{
		srcColPtr = srcRowPtr;
		destColPtr = destRowPtr;
		if (y < PrevCtx->rect.top || y > PrevCtx->rect.bottom)
		{
			rtp_memcpy(destColPtr,srcColPtr,((FBGraphicsContext *)ctx)->frameBufferItf->GetRowBytes());
			continue;
		}
int residual_width = ((FBGraphicsContext *)ctx)->frameBufferItf->GetRowBytes()/bytesperpix;
	residual_width -= PrevCtx->rect.left;
	residual_width -= width;

		if (bytesperpix == 1)
		{
			for (x = 0; x < PrevCtx->rect.left; x++)
			{
				destColPtr[0] = srcColPtr[0];
				srcColPtr += 1;
				destColPtr += 1;
			}
			for (x = 0; x < width; x++)
			{
				destColPtr[0] = ALPHATRANSFORM(destColPtr[0], srcColPtr[0], opacity);
				srcColPtr += 1;
				destColPtr += 1;
			}
			for (x = 0; x < residual_width; x++)
			{
				destColPtr[0] = srcColPtr[0];
				srcColPtr += 1;
				destColPtr += 1;
			}
		}
		else
		{
			for (x = 0; x < PrevCtx->rect.left; x++)
			{
				destColPtr[0] = srcColPtr[0];
				destColPtr[1] = srcColPtr[1];
				destColPtr[2] = srcColPtr[2];
				destColPtr[3] = srcColPtr[3];
				srcColPtr += 4;
				destColPtr += 4;
			}
			for (x = 0; x < width; x++)
			{
				if (opacity == 255)
				{
					destColPtr[0] = srcColPtr[0];
					destColPtr[1] = srcColPtr[1];
					destColPtr[2] = srcColPtr[2];
					destColPtr[3] = srcColPtr[3];
				}
				else
				{
					destColPtr[0] = ALPHATRANSFORM(destColPtr[0], srcColPtr[0], opacity);
					destColPtr[1] = ALPHATRANSFORM(destColPtr[1], srcColPtr[1], opacity);
					destColPtr[2] = ALPHATRANSFORM(destColPtr[2], srcColPtr[2], opacity);
					destColPtr[3] = srcColPtr[3];
				}
				srcColPtr += 4;
				destColPtr += 4;
			}
			for (x = 0; x < residual_width; x++)
			{
				destColPtr[0] = srcColPtr[0];
				destColPtr[1] = srcColPtr[1];
				destColPtr[2] = srcColPtr[2];
				destColPtr[3] = srcColPtr[3];
				srcColPtr += 4;
				destColPtr += 4;
			}
		}
	}

}


#if (BUFFER_INVALID_REGIONS_ONLY)
static void FB_CopyFrameBuffer(HTMLGraphicsContext **ctx,int toBufferNo, int fromBufferNo, HTMLRect &r)
{
DISPLAY_INT height, width, yPos, xPos;

	yPos = r.top;
	height = r.bottom-r.top+1;
	width  = r.right-r.left+1;
	xPos = r.left;

	register WEBC_UINT8 *frPtr;
	register WEBC_UINT8 *toPtr;
		for (int h = 0; h < height; h++,yPos++)
		{
			frPtr = (WEBC_UINT8 *) lGetBaseAddress(ctx,GET_FBGRAPHICS_CTX(ctx)->frameBufferItf,fromBufferNo) + ((yPos * GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetRowBytes()) + (xPos * BYTES_PER_PIXEL));
			toPtr = (WEBC_UINT8 *) lGetBaseAddress(ctx,GET_FBGRAPHICS_CTX(ctx)->frameBufferItf,toBufferNo) + ((yPos * GET_FBGRAPHICS_CTX(ctx)->frameBufferItf->GetRowBytes()) + (xPos * BYTES_PER_PIXEL));
				for (int i = 0; i < width; i++, frPtr += BYTES_PER_PIXEL,toPtr += BYTES_PER_PIXEL)
					for (int j = 0; j < BYTES_PER_PIXEL; j++)
						toPtr[j] = frPtr[j];
		}
}
#endif

#endif /* WINDOWSGDI	*/
