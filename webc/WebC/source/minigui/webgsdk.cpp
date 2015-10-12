//
// WEBGSDK.CPP -
//
// EBS - WebC
//
// Copyright EBS Inc. , 2010
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/*****************************************************************************/
// Header files
/*****************************************************************************/
#if (0)
#include "htmlsdk.h"
#include "webgraph.hpp"
#include "webfonts.hpp"
#include "webcmem.h"
#include "webc.h"

typedef void * WEBGCTXT;
typedef void * WEBGIMAGE;
typedef void * PWEBGIMAGEINFO;			// Fix me
typedef unsigned short WEBGFONTATTRIBS;
typedef unsigned short WEBGPIXARRAYATTRIBS;
typedef unsigned long  WEBGCOLOR;
#define WEBGCTRANSPARENT 0xffffffff
#define WEBGCHANDLETOGC(G) ((WEBGCONTEXT *)(G))->gc
#define WEBGCHANDLETOWEBGCONTEXT(G) ((WEBGCONTEXT *)(G))
#define POINTS_TO_HTMLRECT(H,X1, Y1, X2, Y2)     ((H).left  = X1,  (H).top   = Y1, (H).right = X2,(H).bottom = Y2)

typedef struct s_WebGContext {
WebGraphics *gc;
WebColor textColor;
WebColor textBgColor;
WebGraphFont *font;
} WEBGCONTEXT;





void webg_BeginDraw(WEBGCTXT Ctx);
void webg_EndDraw(WEBGCTXT Ctx);
void webg_DrawText(WEBGCTXT Ctx, unsigned int x, unsigned int y, char *txt);
void webg_DrawLine(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2, int penWidth, WEBGCOLOR penColor);
void webg_DrawRect(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2, int penWidth, WEBGCOLOR frameColor, WEBGCOLOR fillColor);
WEBGIMAGE webg_LoadImage(WEBGCTXT Ctx, char *ImageUrl);
WEBGIMAGE webg_NewImage(WEBGCTXT Ctx, unsigned int Height, unsigned int Width);
void webg_GetImageInfo(WEBGCTXT Ctx, WEBGIMAGE Img, PWEBGIMAGEINFO pInfo);
void webg_DrawImage(WEBGCTXT Ctx, unsigned int x, unsigned int y, WEBGIMAGE Img);
void webg_SetFont(WEBGCTXT Ctx,	const char *Name, unsigned int PointSize, WEBGFONTATTRIBS attribs, WEBGCOLOR BGColor, WEBGCOLOR Color);
void webg_SetPen(WEBGCTXT Ctx, WEBGCOLOR Color, int Thickness);
void webg_SetBgColor(WEBGCTXT Ctx, WEBGCOLOR Color);
void webg_SetColor(WEBGCTXT Ctx, WEBGCOLOR Color);
void webg_SetClip(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2);
unsigned long webg_GetPix(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2, void *pixels, WEBGPIXARRAYATTRIBS attribs);
void webg_SetPix(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2, void *pixels, WEBGPIXARRAYATTRIBS attribs);



static WebGraphFont * _webg_GetCurrentFont(WEBGCTXT Ctx);
static WebcFontDescriptor _webg_GetFontDescriptor (WEBGCTXT Ctx,const WEBC_CHAR *FontName,int  PointSize,	WEBGFONTATTRIBS  StyleAndWeight);

/* void webg_BeginDraw(WEBGCTXT Ctx);

*/
void webg_BeginDraw(WEBGCTXT Ctx)
{
     WEBGCHANDLETOGC(Ctx)->StartBuffer();
}


/* void webg_EndDraw(WEBGCTXT Ctx);

*/
void webg_EndDraw(WEBGCTXT Ctx)
{
     WEBGCHANDLETOGC(Ctx)->EndBuffer();
}

/* void webg_DrawText(WEBGCTXT Ctx, unsigned int x, unsigned int y, char *txt);

*/
void webg_DrawText(WEBGCTXT Ctx, unsigned int x, unsigned int y, char *txt)
{
WebGraphics *gc =WEBGCHANDLETOGC(Ctx);
const WebChar* text;
WEBC_BOOL fill;
WebGraphFont *font = _webg_GetCurrentFont(Ctx);

	fill = WEBGCHANDLETOWEBGCONTEXT(Ctx)->textBgColor == WEBGCTRANSPARENT ? WEBC_TRUE : WEBC_FALSE;
	(*gc->mGraphics)->drawText(gc->mGraphics, x, y, text, WEBGCHANDLETOWEBGCONTEXT(Ctx)->textColor, WEBGCHANDLETOWEBGCONTEXT(Ctx)->textBgColor, fill, font);

}

/* void webg_DrawRect(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2);

*/
void webg_DrawRect(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2, int penWidth, WEBGCOLOR frameColor, WEBGCOLOR fillColor)
{
WebGraphics *gc =WEBGCHANDLETOGC(Ctx);
HTMLRect rect;
WEBC_BOOL fill;


	POINTS_TO_HTMLRECT(rect ,x1, y1, x2, y2);
	fill = fillColor == WEBGCTRANSPARENT ? WEBC_TRUE : WEBC_FALSE;
	(*gc->mGraphics)->drawRectangle(gc->mGraphics, &rect, frameColor, fillColor, fill);
}

/* void webg_SetClip(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2);

*/
void webg_SetClip(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2)
{
WebGraphics *gc =WEBGCHANDLETOGC(Ctx);
HTMLRect rect;
	POINTS_TO_HTMLRECT(rect ,x1, y1, x2, y2);
	(*gc->mGraphics)->setClipRect(gc->mGraphics, &rect);
}

/* void webg_SetFont(WEBGCTXT Ctx,	const char *Name, unsigned int PointSize, WEBGFONTATTRIBS attribs, WEBGCOLOR BGColor, WEBGCOLOR Color);


*/
void webg_SetFont(WEBGCTXT Ctx,	const WEBC_CHAR *Name, const WEBC_CHAR *GenericName,unsigned int PointSize, WEBGFONTATTRIBS StyleAndWeight, WEBGCOLOR BGColor, WEBGCOLOR Color)
{
WebFont wf = (WebFont) WEBC_MALLOC(sizeof(struct s_WebFont));
if (!wf)
	return;
WebGraphics *gc =WEBGCHANDLETOGC(Ctx);
WebcFontDescriptor fdesc = 	_webg_GetFontDescriptor (Ctx, Name, PointSize,	StyleAndWeight);
WEBC_UINT16 fam;
	fam = WEBC_FONT_FAMILY(fdesc);
	fam = EBSCLIP(fam,0,WEBC_CFG_MAX_FONT_FAMILIES-1);

	// GetWebGraphFont(fdesc);
	wf->font = (*gc->mGraphics)->createFont (
			gc->mGraphics,
			Name,
			GenericName,
			PointSize,
			fdesc & 0x3,
			fam
		);

}


/* void webg_DrawLine(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2, int penWidth, WEBGCOLOR penColor)


*/
void webg_DrawLine(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2, int penWidth, WEBGCOLOR penColor)
{

}

/* WEBGIMAGE webg_LoadImage(WEBGCTXT Ctx, char *ImageUrl);

*/
WEBGIMAGE webg_LoadImage(WEBGCTXT Ctx, char *ImageUrl)
{
	return 0;

}

/* WEBGIMAGE webg_NewImage(WEBGCTXT Ctx, unsigned int Height, unsigned int Width);

*/
WEBGIMAGE webg_NewImage(WEBGCTXT Ctx, unsigned int Height, unsigned int Width)
{
	return 0;

}

/* void webg_GetImageInfo(WEBGCTXT Ctx, WEBGIMAGE Img, PWEBGIMAGEINFO pInfo);

*/
void webg_GetImageInfo(WEBGCTXT Ctx, WEBGIMAGE Img, PWEBGIMAGEINFO pInfo)
{

}

/* void webg_DrawImage(WEBGCTXT Ctx, unsigned int x, unsigned int y, WEBGIMAGE Img);

*/
void webg_DrawImage(WEBGCTXT Ctx, unsigned int x, unsigned int y, WEBGIMAGE Img)
{

}


/* void webg_SetPen(WEBGCTXT Ctx, WEBGCOLOR Color, int Thickness);

*/
void webg_SetPen(WEBGCTXT Ctx, WEBGCOLOR Color, int Thickness)
{

}

/* void webg_SetBgColor(WEBGCTXT Ctx, WEBGCOLOR Color);

*/
void webg_SetBgColor(WEBGCTXT Ctx, WEBGCOLOR Color)
{

}

/* void webg_SetColor(WEBGCTXT Ctx, WEBGCOLOR Color);

*/
void webg_SetColor(WEBGCTXT Ctx, WEBGCOLOR Color)
{

}


/* unsigned long webg_GetPix(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2, void *pixels, WEBGPIXARRAYATTRIBS attribs);

*/
unsigned long webg_GetPix(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2, void *pixels, WEBGPIXARRAYATTRIBS attribs)
{
HTMLRect rect;
	POINTS_TO_HTMLRECT(rect ,x1, y1, x2, y2);

	return 0;
}

/* void webg_SetPix(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2, void *pixels, WEBGPIXARRAYATTRIBS attribs);

*/
void webg_SetPix(WEBGCTXT Ctx, unsigned int x1, unsigned int y1,unsigned int x2, unsigned int y2, void *pixels, WEBGPIXARRAYATTRIBS attribs)
{
HTMLRect rect;
	POINTS_TO_HTMLRECT(rect ,x1, y1, x2, y2);

}


static WebGraphFont * _webg_GetCurrentFont(WEBGCTXT Ctx)
{
WebGraphics *gc =WEBGCHANDLETOGC(Ctx);
WebGraphFont *DefaultFont;

	if (WEBGCHANDLETOWEBGCONTEXT(Ctx)->font)
		return(WEBGCHANDLETOWEBGCONTEXT(Ctx)->font);
    void* font = (*gc->mGraphics)->getDefaultFont(gc->mGraphics);
	if (font)
	{
		WebFont defFont = (WebFont) WEBC_MALLOC(sizeof(s_WebFont));
		if (defFont)
		{
			defFont->font = font;
			defFont->height = (*gc->mGraphics)->getFontHeight(gc->mGraphics, font);
			WEBC_NEW(DefaultFont, WebGraphFont(defFont));
			if (!DefaultFont)
			{
				WEBC_FREE(defFont);
			}
			else
				WEBGCHANDLETOWEBGCONTEXT(Ctx)->font = DefaultFont;

		}
	}

	return(WEBGCHANDLETOWEBGCONTEXT(Ctx)->font);
}

static int _webg_GetFontFamilyIndex (WEBGCTXT Ctx,const WEBC_CHAR* name,const WEBC_CHAR* genericName);
static WebcFontDescriptor _webg_GetFontDescriptor (
		WEBGCTXT Ctx,
		const WEBC_CHAR *FontName,
		int  PointSize,
		WEBGFONTATTRIBS  StyleAndWeight
		)
{
WebcFontDescriptor baseDesc = 0;

	if (!FontName)
		return 0;  // is this right ????

	{
		int n;

		n = _webg_GetFontFamilyIndex(Ctx, FontName, 0);
		if (n < 0)
			n = 0;

		WEBC_SET_FONT_FAMILY(baseDesc, n);
	}
	WEBC_SET_FONT_SIZE(baseDesc, PointSize);
	baseDesc |= (StyleAndWeight&(WEBC_FONT_STYLE_MASK|WEBC_FONT_WEIGHT_MASK));
	return(baseDesc);
}

static int _webg_GetFontFamilyIndex (
		WEBGCTXT Ctx,
		const WEBC_CHAR* name,
		const WEBC_CHAR* genericName
	)
{
WebGraphics *gc =WEBGCHANDLETOGC(Ctx);

	int numFonts =  (*gc->mGraphics)->getNumFontFamilies(gc->mGraphics);
	const WEBC_CHAR** fontNames = (*gc->mGraphics)->getFontFamilyNames(gc->mGraphics);
	const WEBC_CHAR** fontGenericNames = (*gc->mGraphics)->getFontFamilyGenericNames(gc->mGraphics);

	return webc_font_family_index(name, genericName, fontNames, fontGenericNames, numFonts);
}
#endif