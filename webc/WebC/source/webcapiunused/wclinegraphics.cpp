//
// wclinegraphics.cpp -
//
// EBS - WebC
//
// Copyright EBS Inc. , 2011
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

//#include "../include/NewApi.h"
#include "wcapi.h"

#include "dflow.hpp"
#include "helement.hpp"
#include "htmlbrow.hpp"
#include "webcassert.h"
#include "htmlfind.hpp"
#include "htmldoc.hpp"
#include "hoption.hpp"
#include "webcmem.h"
#include "rtptime.h"
#include "hselect.hpp"
#include "himage.hpp"
#include "rtpprint.h"



// =====================================================

// PRIVATE UTILITY FUNCTIONS

#include <math.h>
#ifndef PI
#define PI 3.14159265358979323846
#endif /* PI */

WebGraphBitmapInfo* establishBackstore(HTMLImage *pHtmlImg)
{
		WebGraphBitmapInfo* retVal = 0;

		retVal = pHtmlImg->GetBackstoreImage();

		if(0 == retVal)
		{

			WebcBitmapImage *pBitmapImg = (WebcBitmapImage *)(pHtmlImg->GetImage()); // TODO this will fail for GIFs and MNGs

			if(pBitmapImg)
			{
				pBitmapImg->GetDrawableBitmap(&retVal);

				pHtmlImg->SetBackstoreImage(retVal);
			}
		}

		return retVal;
}

void releaseBackstore(HTMLImage * pHtmlImg)
{
		WebGraphBitmapInfo* pBackstore = 0;

		pBackstore = pHtmlImg->GetBackstoreImage();

		if(pBackstore)
		{
			WebcBitmapImage *pBitmapImg = (WebcBitmapImage *)(pHtmlImg->GetImage()); // TODO this will fail for GIFs and MNGs

			pBitmapImg->ReplaceBitmap(pBackstore);
			pHtmlImg->GetDisplayElement()->Invalidate();
			WEBC_FREE((void *)(pBackstore->pData));
			WEBC_FREE((void *)pBackstore);
			pHtmlImg->SetBackstoreImage(0);
			pHtmlImg->SetFlag(HIMG_FLAG_SRC_DIRTY);
		}

}

// Drawing primitives

void _setPixel(WebGraphBitmapInfo* pGenericBitMap, HTMLColor color,  int x, int y)
{
	// do some bounds checking
	if((x >= pGenericBitMap->iWidth) || (y >= pGenericBitMap->iHeight) || (x < 0) || (y < 0))
		return;
	int bytesPerPixel = pGenericBitMap->iBytesPerRow / pGenericBitMap->iWidth;
	WEBC_UINT8* pLocation = (WEBC_UINT8*)(pGenericBitMap->pData + ((pGenericBitMap->iBytesPerRow * y) + (bytesPerPixel * x)));
	// WEBC_UINT8* pLocation = (WEBC_UINT8*)(pGenericBitMap->pData) + (pGenericBitMap->iBytesPerRow * (pGenericBitMap->iHeight - y - 1)) + (bytesPerPixel * x);
	switch(pGenericBitMap->pixelFormat)
	{
	case WB_PIXEL_FORMAT_RGB_24:
		*pLocation++ = (WEBC_UINT8) ((color >> 16) & 0xFF);
		*pLocation++ = (WEBC_UINT8) ((color >> 8) & 0xFF);
		*pLocation++ = (WEBC_UINT8) (color & 0xFF);
		break;
	case WB_PIXEL_FORMAT_RGBA_32:
		*pLocation++ = (WEBC_UINT8) (color & 0xFF);
		*pLocation++ = (WEBC_UINT8) ((color >> 8) & 0xFF);
		*pLocation++ = (WEBC_UINT8) ((color >> 16) & 0xFF);
		*pLocation++ = (WEBC_UINT8) ((color >> 24) & 0xFF);
		break;
	case WB_PIXEL_FORMAT_INDEX_8:
		//TODO
	case WB_PIXEL_FORMAT_MONOCHROME_8:
		*pLocation = (WEBC_UINT8) (color & 0xFF);
		break;
	}
}

#define ROUND(x) ((x > 0.5) ? ceil(x) : floor(x))

void _notline(WebGraphBitmapInfo* pGenericBitMap, int x0, int y0, int x1, int y1, HTMLColor color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;

    _setPixel(pGenericBitMap, color, x0, y0);
    if (dy != 0) {
        float m = (float) dx / (float) dy;
        float b = x0 - m*y0;
        dy = (y1 > y0) ? 1 : -1;
        while (y0 != y1) {
            y0 += dy;
            x0 = (int) ROUND(m*y0 + b);
            _setPixel(pGenericBitMap, color, x0, y0);
        }
    }
}
void _line(WebGraphBitmapInfo* pGenericBitMap, int x0, int y0, int x1, int y1, HTMLColor color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
	if (EBSABS(dy) > EBSABS(dx))
	{
		_notline(pGenericBitMap, x0, y0, x1, y1, color);
		return;
	}


    _setPixel(pGenericBitMap, color, x0, y0);
    if (dx != 0) {
        float m = (float) dy / (float) dx;
        float b = y0 - m*x0;
        dx = (x1 > x0) ? 1 : -1;
        while (x0 != x1) {
            x0 += dx;
            y0 = (int) ROUND(m*x0 + b);
            _setPixel(pGenericBitMap, color, x0, y0);
        }
    }
}

void _polyLine(WebGraphBitmapInfo* pGenericBitMap, int numCoords, int *coordArray, HTMLColor color)
{
	int currCoordinate=0, x0, y0, x1, y1;

	// numCoords is the number of coordinate pairs

	x0 = coordArray[currCoordinate++];
	y0 = coordArray[currCoordinate++];
	for(int i=1; i < numCoords; i++)
	{
		x1 = coordArray[currCoordinate++];
		y1 = coordArray[currCoordinate++];
		_line(pGenericBitMap, x0, y0, x1, y1, color);
		x0 = x1;
		y0=y1;
	}
}


void _fillRect(WebGraphBitmapInfo* pGenericBitMap, int x, int y, int w, int h, HTMLColor color)
{
	for(int row = 0; row < h; row++)
	{
		_line(pGenericBitMap, x, y+row, x+w, y+row, color);
	}
}
void _drawRect(WebGraphBitmapInfo* pGenericBitMap, int x, int y, int w, int h, HTMLColor color)
{
	_line(pGenericBitMap, x, y, x+w, y, color);
	_line(pGenericBitMap, x, y, x, y+h, color);
	_line(pGenericBitMap, x+w, y, x+w, y+h, color);
	_line(pGenericBitMap, x, y+h, x+w, y+h, color);
}

#define pi (3.1415926535897932384626433832795)

void _ellipse(WebGraphBitmapInfo* pGenericBitMap, double dx, double dy, double wx, double hy, HTMLColor color)
{
    double x, y, _x, _y, __x, __y, z;
	const int nSeg = 400;

    z = 0.99; //Point coordinate affinity
    for(int i = 0; i < int(nSeg); i++) {
        x = wx * sin((double(i) / double(nSeg)) * (pi*2.0));
        y = hy * cos((double(i) / double(nSeg)) * (pi*2.0));
        if(i > 0) {
            //Draw a line connecting the last point to the one being calculated now
			_line(pGenericBitMap, int(dx+_x+z), int(dy+-_y+z), int(dx+x+z), int(dy+-y+z), color);
        } else {
            //Save the first point coordinate to link it with the last one
            __x = x;
            __y = y;
        }
        //Save the actual point coordinate to link it with the next one
        _x = x;
        _y = y;
    }
    //Draw a line between the last point and the first one
	_line(pGenericBitMap, int(dx+x+z), int(dy+-y+z), int(dx+__x+z), int(dy+-__y+z), color);
}
void _fillPolygon(WebGraphBitmapInfo* pGenericBitMap,int numCoords, int *coordArray,HTMLColor color);
void _fillarc(WebGraphBitmapInfo* pGenericBitMap, double x, double y, double r1, double r2, double a1, double a2, HTMLColor color)
{
	double a, da;
	int *pt;
	int N, i;
	N = (a2==a1)? 360 : (int)fabs(a2-a1) + 8;
	a = a1 * 2.0 * PI / 360.0;
	da = ((a2>a1) ? (a2-a1) :360.0 - (a1-a2)) * 2.0 * PI / 360.0 / (N-1);
	pt = (int *) WEBC_MALLOC(2 * (N+1) * sizeof(int));
	for(i=0;i<N;i++)
	{
	    pt[2*i+0]=(int) (x+r1*cos(a+i*da));
	    pt[2*i+1]=(int) (y+r2*sin(a+i*da));
	}
    //Draw a line between the last point and the first one
    pt[2*N]=pt[2*N-2];(int) (x+r1*cos(a+i*da));
    pt[2*N+1]=pt[2*N-1];
	_fillPolygon(pGenericBitMap,N+1, pt, color);
	WEBC_FREE(pt);

}
void _arc(WebGraphBitmapInfo* pGenericBitMap, double x, double y, double r1, double r2, double a1, double a2, HTMLColor color)
{
	double a, da;
	int *pt;
	int N, i;
	N = (a2==a1)? 360 : (int)fabs(a2-a1) + 8;
	a = a1 * 2.0 * PI / 360.0;
	da = ((a2>a1) ? (a2-a1) :360.0 - (a1-a2)) * 2.0 * PI / 360.0 / (N-1);
	pt = (int *) WEBC_MALLOC(2 * N * sizeof(int));
	for(i=0;i<N;i++)
	{
	    pt[2*i+0]=(int) (x+r1*cos(a+i*da));
	    pt[2*i+1]=(int) (y+r2*sin(a+i*da));
	}
	_polyLine(pGenericBitMap, N, pt, color);		  /* using poly_line */
	WEBC_FREE(pt);

}
void _fillPolygon(WebGraphBitmapInfo* pGenericBitMap,int numCoords, int *coordArray,HTMLColor color);
void _fillellipse(WebGraphBitmapInfo* pGenericBitMap, double dx, double dy, double wx, double hy, HTMLColor color)
{
    double x, y, _x, _y, __x, __y, z;
	const int nSeg = 400;
	int *_pt;
	int *pt;

	_pt = (int *) WEBC_MALLOC(4 * nSeg* sizeof(int));
	pt=_pt;

    z = 0.99; //Point coordinate affinity
    for(int i = 0; i < int(nSeg); i++) {
        x = wx * sin((double(i) / double(nSeg)) * (pi*2.0));
        y = hy * cos((double(i) / double(nSeg)) * (pi*2.0));
        if(i > 0) {
            //Draw a line connecting the last point to the one being calculated now
			_pt[0] = int(dx+_x+z); _pt[1]=int(dy+-_y+z); _pt[2]=int(dx+x+z);_pt[3]=int(dy+-y+z);
			_pt+=4;
        } else {
            //Save the first point coordinate to link it with the last one
            __x = x;
            __y = y;
        }
        //Save the actual point coordinate to link it with the next one
        _x = x;
        _y = y;
    }
    //Draw a line between the last point and the first one
	_pt[0] = int(dx+x+z); _pt[1]=int(dy+-y+z); _pt[2]=int(dx+__x+z);_pt[3]=int(dy+-__y+z);
	_fillPolygon(pGenericBitMap,nSeg*2, pt, color);
	//_polyLine(pGenericBitMap,nSeg*2, pt, color);

	WEBC_FREE(pt);
}
// GENERALIZED POLYGON FILL

struct edge {
    struct edge *next;
    long yTop, yBot;
    long xNowWhole, xNowNum, xNowDen, xNowDir;
    long xNowNumStep;
};

class PolygonPoint {
public:
	int x,y;
	static PolygonPoint RefNth(int *coordArray, int index)
	{
		PolygonPoint retVal;
		retVal.x = coordArray[index<<1];
		retVal.y = coordArray[(index<<1) + 1];
		return retVal;
	}
} ;

#define SGN(x) (x ? (x < 0 ? (-1) : 1) : 0)

#define MAXVERTICAL     1024

static void FillEdges(int numCoords, int *coordArray, struct edge *edgeTable[])
{
    int i, j, n = numCoords;

    for (i = 0; i < MAXVERTICAL; i++)
        edgeTable[i] = NULL;

    for (i = 0; i < n; i++) {
        PolygonPoint p1, p2, p3;
        struct edge *e;
		p1 = PolygonPoint::RefNth(coordArray, i);
        p2 = PolygonPoint::RefNth(coordArray, (i + 1) % n);
        if (p1.y == p2.y)
            continue;   /* Skip horiz. edges */
        /* Find next vertex not level with p2 */
        for (j = (i + 2) % n; ; j = (j + 1) % n) {
            p3 = PolygonPoint::RefNth(coordArray, j);
            if (p2.y != p3.y)
                break;
        }
        e = (struct edge *) WEBC_MALLOC(sizeof(struct edge));
        e->xNowNumStep = abs((double)(p1.x - p2.x));
        if (p2.y > p1.y) {
            e->yTop = p1.y;
            e->yBot = p2.y;
            e->xNowWhole = p1.x;
            e->xNowDir = SGN(p2.x - p1.x);
            e->xNowDen = e->yBot - e->yTop;
            e->xNowNum = (e->xNowDen >> 1);
            if (p3.y > p2.y)
                e->yBot--;
        } else {
            e->yTop = p2.y;
            e->yBot = p1.y;
            e->xNowWhole = p2.x;
            e->xNowDir = SGN(p1.x - p2.x);
            e->xNowDen = e->yBot - e->yTop;
            e->xNowNum = (e->xNowDen >> 1);
            if (p3.y < p2.y) {
                e->yTop++;
                e->xNowNum += e->xNowNumStep;
                while (e->xNowNum >= e->xNowDen) {
                    e->xNowWhole += e->xNowDir;
                    e->xNowNum -= e->xNowDen;
                }
            }
        }
        e->next = edgeTable[e->yTop];
        edgeTable[e->yTop] = e;
    }
}

/*
 * UpdateActive first removes any edges which curY has entirely
 * passed by.  The removed edges are freed.
 * It then removes any edges from the edge table at curY and
 * places them on the active list.
 */

static struct edge *UpdateActive(struct edge *active, struct edge *edgeTable[], long curY)
{
    struct edge *e, **ep;
    for (ep = &active, e = *ep; e != NULL; e = *ep)
        if (e->yBot < curY) {
            *ep = e->next;
            WEBC_FREE(e);
        } else
            ep = &e->next;
    *ep = edgeTable[curY];
    return active;
}

/*
 * DrawRuns first uses an insertion sort to order the X
 * coordinates of each active edge.  It updates the X coordinates
 * for each edge as it does this.
 * Then it draws a run between each pair of coordinates,
 * using the specified fill pattern.
 *
 * This routine is very slow and it would not be that
 * difficult to speed it way up.
 */

void DrawRuns(
WebGraphBitmapInfo* pGenericBitMap,
struct edge *active,
long curY,
HTMLColor color)
{
    struct edge *e;
    static long xCoords[100];
    long numCoords = 0;
    int i;
    for (e = active; e != NULL; e = e->next) {
        for (i = numCoords; i > 0 &&
          xCoords[i - 1] > e->xNowWhole; i--)
            xCoords[i] = xCoords[i - 1];
        xCoords[i] = e->xNowWhole;
        numCoords++;
        e->xNowNum += e->xNowNumStep;
        while (e->xNowNum >= e->xNowDen) {
            e->xNowWhole += e->xNowDir;
            e->xNowNum -= e->xNowDen;
        }
    }
    if (numCoords % 2)  /* Protect from degenerate polygons */
        xCoords[numCoords] = xCoords[numCoords - 1],
        numCoords++;
    for (i = 0; i < numCoords; i += 2) {
        /* Here's the graphics-dependent part. */
        /* All we need is to draw a horizontal line */
        /* from (xCoords[i], curY) to (xCoords[i + 1], curY). */
        /* Example: I want to fill the polygon with a pattern. */
        /* (This example is very slow because it's done point by */
        /* point, thus not taking advantage of the potential for */
        /* speed afforded by the scan conversion...) */
		_line(pGenericBitMap, xCoords[i], curY, xCoords[i + 1], curY, color);
    }
}

/*
 * This version of the fill takes a fill pattern argument.  It may
 * be removed throughout for straight single-color fills.
 */

void _fillPolygon(
WebGraphBitmapInfo* pGenericBitMap,
int numCoords, int *coordArray,
HTMLColor color)
{
    static struct edge *edgeTable[MAXVERTICAL];
    struct edge *active;
    long curY;

    FillEdges(numCoords, coordArray, edgeTable);

    for (curY = 0; edgeTable[curY] == NULL; curY++)
        if (curY == MAXVERTICAL - 1)
            return;     /* No edges in polygon */

    for (active = NULL; (active = UpdateActive(active,
      edgeTable, curY)) != NULL; curY++)
        DrawRuns(pGenericBitMap, active, curY, color);
}

extern "C" const HTMLColor rHTMLColorValues [];

/*****************************************************************************/
// wcImageGraphNameToColor
/**

 @memo   Convert a color index value to a 32 bit html value that may be used as a color argument to \Ref{wcImageGraphSetPenColor}.
 @doc    Convert color names to 32 bit rgb values. The names are the same as those passed in strings to \Ref{wcElSetStyleColorNamed} except theey are integer constants instead of strings.

 <pre>

 Example:
	wcImageGraphSetPenColor(ImageElement, wcImageGraphNameToColor(DarkSlateBlue)); // Set the pen to DarkSlateBlue

 These named color values are supported.

AliceBlue,AntiqueWhite,Aqua,Aquamarine,Azure,Beige,Bisque,Black,BlanchedAlmond,Blue,BlueViolet,Brown,BurlyWood,CadetBlue,Chartreuse,Chocolate,Coral,CornflowerBlue,Cornsilk,
Crimson,Cyan,DarkBlue,DarkCyan,DarkGoldenRod,DarkGray,DarkGreen,DarkKhaki,DarkMagenta,DarkOliveGreen,Darkorange,DarkOrchid,DarkRed,DarkSalmon,DarkSeaGreen,DarkSlateBlue,
DarkSlateGray,DarkTurquoise,DarkViolet,DeepPink,DeepSkyBlue,DimGray,DodgerBlue,Feldspar,FireBrick,FloralWhite,ForestGreen,Fuchsia,Gainsboro,GhostWhite,Gold,GoldenRod,Gray,
Green,GreenYellow,HoneyDew,HotPink,IndianRed,Indigo,Ivory,Khaki,Lavender,LavenderBlush,LawnGreen,LemonChiffon,LightBlue,LightCoral,LightCyan,LightGoldenRodYellow,LightGrey,
LightGreen,LightPink,LightSalmon,LightSeaGreen,LightSkyBlue,LightSlateBlue,LightSlateGray,LightSteelBlue,LightYellow,Lime,LimeGreen,Linen,Magenta,Maroon,MediumAquaMarine,
MediumBlue,MediumOrchid,MediumPurple,MediumSeaGreen,MediumSlateBlue,MediumSpringGreen,MediumTurquoise,MediumVioletRed,MidnightBlue,MintCream,MistyRose,Moccasin,NavajoWhite,
Navy,OldLace,Olive,OliveDrab,Orange,OrangeRed,Orchid,PaleGoldenRod,PaleGreen,PaleTurquoise,PaleVioletRed,PapayaWhip,PeachPuff,Peru,Pink,Plum,PowderBlue,Purple,Red,
RosyBrown,RoyalBlue,SaddleBrown,Salmon,SandyBrown,SeaGreen,SeaShell,Sienna,Silver,SkyBlue,SlateBlue,SlateGray,Snow,SpringGreen,SteelBlue,Tan,Teal,Thistle,Tomato,
Transparent,Turquoise,Violet,VioletRed,Wheat,White,WhiteSmoke,Yellow,YellowGreen


 </pre>

 @param  HtmlColorIndex ColorName 	- The color index to convert to a color value.


 @return The HTML color value

 @see ExGraphics,wcImageGraphSetPenColor,wcElSetStyleColorNamed

 */
/*****************************************************************************/

HTMLColor wcImageGraphNameToColor(HtmlColorIndex ColorName)
{
	 return rHTMLColorValues [ColorName];
}


/*****************************************************************************/
// wcImageGraphBeginDraw
/**

 @memo   Prepare an image for a sequence of one or more wcImageGraphDraw commands to follow.
 @doc 	 wcImageGraphBeginDraw clears the pallet to the orignial image it was created from so that a clean set of draw subroutines can be executed.
 The draw commands to follow will not be visible until \Ref{wcImageGraphEndDraw} is called.

<i><b> Note: ImageElement is an image element that was either created with an html &lt;div&gt; tag or by a call \Ref{wcNewStyledDiv} or some other Javascript or C method
that loads a valid image file.
 <br> Note: The following restrictions and limitations should be kept in mind.</i></b>
<ul>
<li>The image content must exist, wcImageGraph does not support drawing to dynamically created elements with source not successfully set and loaded.</li>
<li>The image must be either 24 bit or 32 bit, not all image types and combinations are tested.</li>
<li>If you have any doubts try using a 24 or 32 bit .png image.</li>
<li>Position the image using its exact height and width. Do not scale the image using width and height properties or styles. Doing so will distort the pitch of drawn graphics.</li>
<li>Only operations sandwiched between the previous wcImageGraphBeginDraw, wcImageGraphEndDraw pair are visible.</li>
</ul>
<pre>
Example: Create a pallet to draw into and draw crossing diagonal lines on it.

	wcEL ImageElement =  wcNewStyledImage(&childwindowCtx, 0, wcTRUE, "webc:///Example/Images/ExampleCanvas.png", 0, 0, 0, 200,200, 0, 0);
	wcImageGraphBeginDraw(ImageElement);
	wcImageGraphSetPenColor(ImageElement, wcImageGraphNameToColor(Blue));
	wcImageGraphDrawLine(ImageElement, 200, 200, 0, 0);
	wcImageGraphDrawLine(ImageElement, 0,  0, 200, 200);
	wcImageGraphEndDraw(ImageElement);
</pre>

 @param  wcEL ImageElement 	- An image element that is the background canvas for our draw operations.

 @return nothing

 @see ExGraphics,wcImageGraphEndDraw

 */
/*****************************************************************************/

void wcImageGraphBeginDraw(wcEL ImageElement)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
	establishBackstore(pHtmlImg);
}

/*****************************************************************************/
// wcImageGraphEndDraw
/**

 @memo   Close a sequence of draw commands and update the display.
 @doc 	 wcImageGraphEndDraw updates the screen from the image pallet that was just drawn on and then restores the image's original content.

 @param  wcEL ImageElement 	- An image element that is the background canvas for our draw operations and was previously an argument to \Ref{wcImageGraphBeginDraw}.

 @return nothing

 @see ExGraphics

 */
/*****************************************************************************/
void wcImageGraphEndDraw(wcEL ImageElement)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
	releaseBackstore(pHtmlImg);
}

/*****************************************************************************/
// wcImageGraphRefreshSrc
/**

 @memo   Reload the image canvas from source code.
 @doc 	 Do not use, this function will be used when the behavior of \Ref{wcImageGraphBeginDraw} is changed.

 @param  wcEL ImageElement 	- An image element that is the background canvas for our draw operations.

 @return nothing
 */
/*****************************************************************************/
void wcImageGraphRefreshSrc(wcEL ImageElement)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
	pHtmlImg->RefreshFromSrc();
}

/*****************************************************************************/
// wcImageGraphSetBrushWidth
/**

 @memo   Set the line width for subsequent draw operations.
 @doc    Sets the line width for future draw operations.<br>
 <i><b>Note: wcImageGraphSetBrushWidth is currently not implemented, all line widths are 1. </i></b>

 @param  wcEL ImageElement 	- An image element that is the background canvas for our draw operations.
 @param  int  width - The new line width.

 @return nothing

 @see wcImageGraphSetPenColor
 */
/*****************************************************************************/
void wcImageGraphSetBrushWidth(wcEL ImageElement, int width)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
		// store the width inside the HTMLImage object for later use
		pHtmlImg->SetBrushWidth(width);
}

/*****************************************************************************/
// wcImageGraphSetPenColor
/**

 @memo   Set the color for subsequent draw operations.
 @doc    Sets the 32 bit color for subsequent draw operations.
 <pre>
 The bit positions in the color parameter are:
 0-7	Blue
 8-15	Green
 15-23  Red
 24-31	Alpha
 <i>note: The current release does not support Alpah channel for wcImageGraph operation.</i>
 </pre>

 @param  wcEL ImageElement 	- An image element that is the background canvas for our draw operations.
 @param  HTMLColor color  	- The  bit color ARGB color value to draw in. You may also used named colors by calling \Ref{wcImageGraphNameToColor}.

 @return nothing

 @see ExGraphics
 */
/*****************************************************************************/
void wcImageGraphSetPenColor(wcEL ImageElement, HTMLColor color)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
		pHtmlImg->SetDrawColor((HTMLColor)color);
}

/*****************************************************************************/
// wcImageGraphDrawLine
/**

 @memo   Draw a line to an image element background canvas.
 @doc	 Draws a line at the current pen width and color onto the background canvas element at the requested coordinates.

 @param  wcEL ImageElement 	- An image element that is the background canvas for our draw operations.
 @param  int x0 - Starting point x,y coordinate.
 @param  int y0
 @param  int x1 - End point x,y coordinate.
 @param  int y1

 @return nothing

 @see ExGraphics
 */
/*****************************************************************************/
void wcImageGraphDrawLine(wcEL ImageElement,int x0, int y0, int x1, int y1)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
	WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);
	_line(pGenericBitMap, x0, y0, x1, y1, pHtmlImg->GetDrawColor());
}

/*****************************************************************************/
// wcImageGraphDrawPolygon
/**

 @memo   Draw a sequence of lines to an image element background canvas.
 @doc    Draws a sequence of lines at the current pen width and color onto the background canvas element at coordinates provided by the caller in an array.

 @param  wcEL ImageElement 	- An image element that is the background canvas for our draw operations.
 @param  int nPoints - Number of x,y coordinate pairs stored at the location in the pointArray argument.
 @param  int *pointArray - A pointer to an array of (nPoints*2) integers containing the x,y offsets in the image array of a connected set of lines to draw.
 @param  wcBOOL fill - Fill the area bounded by pointArray if this argument is wcTRUE.

 @return nothing
 @see ExGraphics
 */
/*****************************************************************************/
void wcImageGraphDrawPolygon(wcEL ImageElement, int nPoints, int *pointArray, wcBOOL fill)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
	WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);
	if (fill)
		_fillPolygon(pGenericBitMap, nPoints, pointArray, pHtmlImg->GetDrawColor());
	else
		_polyLine(pGenericBitMap, nPoints, pointArray, pHtmlImg->GetDrawColor());
}

/*****************************************************************************/
// wcImageGraphDrawRect
/**

 @memo   Draws a rectangle to an image canvas element.
 @doc    Draws a rectangle at the current pen width and color onto the background canvas element at coordinates provided by the caller.

 @param  wcEL ImageElement 	- An image element that is the background canvas for our draw operations.
 @param  int x - x,y coordinate of the top left corner of the rectangle.
 @param  int y
 @param  int w - height and width
 @param  int h
 @param  wcBOOL fill - Fill the area bounded by the rectangle if this argument is wcTRUE.

 @return nothing
 @see ExGraphics
 */
/*****************************************************************************/

void wcImageGraphDrawRect(wcEL ImageElement, int x, int y,int w,  int h, wcBOOL fill)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
	WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);
	if (fill)
		_fillRect(pGenericBitMap, x, y, w, h, pHtmlImg->GetDrawColor());
	else
		_drawRect(pGenericBitMap, x, y, w, h, pHtmlImg->GetDrawColor());

}

/*****************************************************************************/
// wcImageGraphDrawEllipse
/**

 @memo   Draws an ellipse to an image canvas element.
 @doc    Draws an ellipse at the current pen width and color onto the background canvas element at coordinates provided by the caller.

 @param  wcEL ImageElement 	- An image element that is the background canvas for our draw operations.
 @param  int x - x,y coordinate of the intersect of the diagonal of the rectangle that bouds the ellispe.
 @param  int y
 @param  int w - height and width of the rectangle the bounds the ellipse
 @param  int h
 @param  wcBOOL fill - Fill the area bounded by the ellipse if this argument is wcTRUE.

 @return nothing
 @see ExGraphics
 */
/*****************************************************************************/
void wcImageGraphDrawEllipse(wcEL ImageElement, int x, int y, int w, int h, wcBOOL fill)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
	w /= 2;	h/=2; //  Internall operates on the half w half h
	double dx = 0.0, dy = 0.0, wx = 0.0, hy = 0.0;
	dx = (double) x;
	dy = (double) y;
	wx = (double) w;
	hy = (double) h;
	WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);

	if (fill)
		_fillellipse(pGenericBitMap, dx, dy, wx, hy, pHtmlImg->GetDrawColor());
	else
		_ellipse(pGenericBitMap, dx, dy, wx, hy, pHtmlImg->GetDrawColor());
}



/*****************************************************************************/
// wcImageGraphArc
/**

 @memo   Draws an arc to an image canvas element.
 @doc    Draws an arc at the current pen width and color onto the background canvas element at coordinates provided by the caller.

 @param  wcEL ImageElement 	- An image element that is the background canvas for our draw operations.
 @param  int ix - x,y coordinate of the top left corner of the rectangle.
 @param  int iy
 @param  int ir1
 @param  int ir2
 @param  int ia1 - Start angle in degrees.
 @param  int ia2 - End angle in degrees.

 @param  int w - height and width of the ellipse.
 @param  int h
 @param  wcBOOL fill - Fill the area bounded by the arc if this argument is wcTRUE.

 @return nothing
 @see ExGraphics
 */
/*****************************************************************************/
void wcImageGraphArc(wcEL ImageElement, int ix, int iy, int ir1, int ir2, int ia1, int ia2, wcBOOL fill)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;

	double x=0.0, y=0.0, r1=0.0, r2=0.0, a1=0.0, a2;
	x=(double)ix; y=(double)iy; r1=(double)ir1; r2=(double)ir2; a1=(double)ia1; a2=(double)ia2;
	WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);

	if (fill)
		_fillarc(pGenericBitMap, x, y, r1, r2, a1, a2, pHtmlImg->GetDrawColor());
	else
		_arc(pGenericBitMap, x, y, r1, r2, a1, a2, pHtmlImg->GetDrawColor());
}
