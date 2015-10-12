#include "webcjhconfig.h"

#if (WEBC_SUPPORT_JSCRIPT)

#include "webcfg.h"
#include "jhimg_ext.h"
#include "jhutils.h"
#include "himage.hpp"
#include "wjscript.hpp"
#include "webcmem.h"
#include "jsobj.h"
#include <math.h>


#if (WEBC_IMAGE_DRAW_EXTENSIONS)


#ifndef PI
#define PI 3.14159265358979323846
#endif /* PI */

// PROTOTYPES FOR GRAPHICS PRIMATIVES

void _line(WebGraphBitmapInfo* pGenericBitMap, int x0, int y0, int x1, int y1, HTMLColor color);
void _polyLine(WebGraphBitmapInfo* pGenericBitMap, int numCoords, int *coordArray, HTMLColor color);
void _ellipse(WebGraphBitmapInfo* pGenericBitMap, double dx, double dy, double wx, double hy, HTMLColor color);
void _fillRect(WebGraphBitmapInfo* pGenericBitMap, int x, int y, int w, int h, HTMLColor color);
void _arc(WebGraphBitmapInfo* pGenericBitMap, double x, double y, double r1, double r2, double a1, double a2, HTMLColor color);
void _fillPolygon(WebGraphBitmapInfo* pGenericBitMap, int numCoords, int *coordArray, HTMLColor color);

// UTILITY FUNCTIONS
WebGraphBitmapInfo* establishBackstore(HTMLImage *pHtmlImg);
void releaseBackstore(HTMLImage * pHtmlImg);


JSBool jhtml_img_ext_webCBeginDraw(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElem  = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLImage *pHtmlImg = (pElem)? (HTMLImage *) pElem : 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!pElem || !jsmgr || !pHtmlImg)
	{
		return JS_TRUE;
	}

	establishBackstore(pHtmlImg);
	return JS_TRUE;
}

JSBool jhtml_img_ext_webCEndDraw(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElem  = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLImage *pHtmlImg = (pElem)? (HTMLImage *) pElem : 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!pElem || !jsmgr || !pHtmlImg)
	{
		return JS_TRUE;
	}

	releaseBackstore(pHtmlImg);
	return JS_TRUE;
}

JSBool jhtml_img_ext_webCRefreshSrc(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElem  = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLImage *pHtmlImg = (pElem)? (HTMLImage *) pElem : 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!pElem || !jsmgr || !pHtmlImg)
	{
		return JS_TRUE;
	}

	pHtmlImg->RefreshFromSrc();
	//pHtmlImg->GetDisplayElement()->Invalidate();

	return JS_TRUE;
}

JSBool jhtml_img_ext_webCBrushWidth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc == 1 && JSVAL_IS_INT(argv[0]))
	{
		HTMLElement *pElem  = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		HTMLImage *pHtmlImg = (pElem)? (HTMLImage *) pElem : 0;
		WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
		WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
		if (!pElem || !jsmgr || !pHtmlImg)
		{
			return JS_TRUE;
		}

		int32 width=0;
		if (JS_ValueToInt32(cx, argv[0], &width) != JS_TRUE)
		{
			return JS_TRUE;
		}

		// store the width inside the HTMLImage object for later use
		pHtmlImg->SetBrushWidth(width);
	}

	*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSBool jhtml_img_ext_webCColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElem  = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLImage *pHtmlImg = (pElem)? (HTMLImage *) pElem : 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!pElem || !jsmgr || !pHtmlImg)
	{
		return JS_TRUE;
	}

	if (argc == 1 && JSVAL_IS_STRING(argv[0]))
	{
		JSString *jsStr;
		HTMLColor colorVal;
		jsStr = JSVAL_TO_STRING(argv[0]);
		WEBC_CHAR *str = WEBC_JS_STRING_TO_WEBC_STRING(jsStr);
		HTML_ParseColor(str, webc_strlen(str), &colorVal);
		// store the color inside the HTMLImage object for later use
		pHtmlImg->SetDrawColor(colorVal);
	}
	else if(argc == 1 && JSVAL_IS_INT(argv[0]))
	{
		int32 color=0;
		if (JS_ValueToInt32(cx, argv[0], &color) != JS_TRUE)
		{
			return JS_TRUE;
		}
		pHtmlImg->SetDrawColor((HTMLColor)color);
	}

	*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSBool jhtml_img_ext_webCLine(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	// see http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html

	if (argc == 4 && JSVAL_IS_INT(argv[0]))
	{
		HTMLElement *pElem  = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		HTMLImage *pHtmlImg = (pElem)? (HTMLImage *) pElem : 0;
		WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
		WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
		if (!pElem || !jsmgr || !pHtmlImg)
		{
			return JS_TRUE;
		}

		int32 x0=0, y0=0, x1=0, y1=0;
		if (JS_ValueToInt32(cx, argv[0], &x0) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToInt32(cx, argv[1], &y0) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToInt32(cx, argv[2], &x1) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToInt32(cx, argv[3], &y1) != JS_TRUE)
		{
			return JS_TRUE;
		}

		WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);

		_line(pGenericBitMap, x0, y0, x1, y1, pHtmlImg->GetDrawColor());

	}

	//*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSBool jhtml_img_ext_webCFillPolygon(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc == 2)
	{
		HTMLElement *pElem  = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		HTMLImage *pHtmlImg = (pElem)? (HTMLImage *) pElem : 0;
		WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
		WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
		if (!pElem || !jsmgr || !pHtmlImg)
		{
			return JS_TRUE;
		}

		struct JSObject *objXArray, *objYArray;
		if (JS_ValueToObject(cx, argv[0], &objXArray) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToObject(cx, argv[1], &objYArray) != JS_TRUE)
		{
			return JS_TRUE;
		}

		if(!JS_IsArrayObject(cx, objXArray))
			return JS_TRUE;
		if(!JS_IsArrayObject(cx, objYArray))
			return JS_TRUE;


		jsuint xArrLen=0, yArrLen=0;
		JS_GetArrayLength(cx, objXArray, &xArrLen);
		JS_GetArrayLength(cx, objYArray, &yArrLen);

		if(xArrLen != yArrLen)
			return JS_TRUE;

		int *coordArray = (int *) WEBC_MALLOC(sizeof(int) * xArrLen * 2);
		int arrayIndex=0;

		for(int i=0; i < (int)xArrLen; i++)
		{
			jsval tempVal;
			int32 tempInt=0;
			// X
			JS_GetElement(cx, objXArray, i, &tempVal);
			if (JS_ValueToInt32(cx, tempVal, &tempInt) != JS_TRUE)
			{
				return JS_TRUE;
			}
			coordArray[arrayIndex++] = tempInt;
			// Y
			JS_GetElement(cx, objYArray, i, &tempVal);
			if (JS_ValueToInt32(cx, tempVal, &tempInt) != JS_TRUE)
			{
				return JS_TRUE;
			}
			coordArray[arrayIndex++] = tempInt;
		}

		WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);

		_fillPolygon(pGenericBitMap, xArrLen, coordArray, pHtmlImg->GetDrawColor());

		WEBC_FREE(coordArray);

	}
	*rval = JSVAL_NULL;
	return JS_TRUE;

}

JSBool jhtml_img_ext_webCFillRect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc == 4)
	{
		HTMLElement *pElem  = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		HTMLImage *pHtmlImg = (pElem)? (HTMLImage *) pElem : 0;
		WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
		WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
		if (!pElem || !jsmgr || !pHtmlImg)
		{
			return JS_TRUE;
		}

		int32 x=0, y=0, w=0, h=0;
		if (JS_ValueToInt32(cx, argv[0], &x) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToInt32(cx, argv[1], &y) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToInt32(cx, argv[2], &w) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToInt32(cx, argv[3], &h) != JS_TRUE)
		{
			return JS_TRUE;
		}

		WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);

		_fillRect(pGenericBitMap, x, y, w, h, pHtmlImg->GetDrawColor());

	}
	*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSBool jhtml_img_ext_webCEllipse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc == 4)
	{
		HTMLElement *pElem  = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		HTMLImage *pHtmlImg = (pElem)? (HTMLImage *) pElem : 0;
		WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
		WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
		if (!pElem || !jsmgr || !pHtmlImg)
		{
			return JS_TRUE;
		}

		double dx = 0.0, dy = 0.0, wx = 0.0, hy = 0.0;
		if (JS_ValueToNumber(cx, argv[0], &dx) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToNumber(cx, argv[1], &dy) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToNumber(cx, argv[2], &wx) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToNumber(cx, argv[3], &hy) != JS_TRUE)
		{
			return JS_TRUE;
		}

		WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);

		_ellipse(pGenericBitMap, dx, dy, wx, hy, pHtmlImg->GetDrawColor());
	}
	*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSBool jhtml_img_ext_webCFillEllipse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return JS_TRUE;
}

JSBool jhtml_img_ext_webCArc(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc == 6)
	{
		HTMLElement *pElem  = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		HTMLImage *pHtmlImg = (pElem)? (HTMLImage *) pElem : 0;
		WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
		WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
		if (!pElem || !jsmgr || !pHtmlImg)
		{
			return JS_TRUE;
		}

		double x=0.0, y=0.0, r1=0.0, r2=0.0, a1=0.0, a2;
		if (JS_ValueToNumber(cx, argv[0], &x) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToNumber(cx, argv[1], &y) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToNumber(cx, argv[2], &r1) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToNumber(cx, argv[3], &r2) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToNumber(cx, argv[4], &a1) != JS_TRUE)
		{
			return JS_TRUE;
		}
		if (JS_ValueToNumber(cx, argv[5], &a2) != JS_TRUE)
		{
			return JS_TRUE;
		}

		WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);

		_arc(pGenericBitMap, x, y, r1, r2, a1, a2, pHtmlImg->GetDrawColor());

	}
	*rval = JSVAL_NULL;
	return JS_TRUE;
}

#if(0)
// PRIVATE UTILITY FUNCTIONS


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

/////////////////////////// DRAWING PRIMATIVES //////////////////////////////////////

void _setPixel(WebGraphBitmapInfo* pGenericBitMap, HTMLColor color,  int x, int y)
{
	// do some bounds checking
	if((x >= pGenericBitMap->iWidth) || (y >= pGenericBitMap->iHeight) || (x < 0) || (y < 0))
		return;

	int bytesPerPixel = pGenericBitMap->iBytesPerRow / pGenericBitMap->iWidth;
	WEBC_UINT8* pLocation = (WEBC_UINT8*)(pGenericBitMap->pData) + (pGenericBitMap->iBytesPerRow * y) + (bytesPerPixel * x);
	//WEBC_UINT8* pLocation = (WEBC_UINT8*)(pGenericBitMap->pData) + (pGenericBitMap->iBytesPerRow * (pGenericBitMap->iHeight - y - 1)) + (bytesPerPixel * x);
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
	if(numCoords % 4)
		return;

	for(int i=0; i < numCoords/4; i++)
	{
		x0 = coordArray[currCoordinate++];
		y0 = coordArray[currCoordinate++];
		x1 = coordArray[currCoordinate++];
		y1 = coordArray[currCoordinate++];
		_line(pGenericBitMap, x0, y0, x1, y1, color);
	}
}


void _fillRect(WebGraphBitmapInfo* pGenericBitMap, int x, int y, int w, int h, HTMLColor color)
{
	for(int row = 0; row < h; row++)
	{
		_line(pGenericBitMap, x, y+row, x+w, y+row, color);
	}
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

/////////////////////////// GENERALIZED POLYGON FILL //////////////////////////////////////

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

void FillEdges(int numCoords, int *coordArray, struct edge *edgeTable[])
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
        e->xNowNumStep = abs(p1.x - p2.x);
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

struct edge *UpdateActive(struct edge *active, struct edge *edgeTable[], long curY)
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

extern "C" {

void webc_ImageGraphBeginDraw(HELEMENT_HANDLE ImageElement)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
	establishBackstore(pHtmlImg);
}
void webc_ImageGraphEndDraw(HELEMENT_HANDLE ImageElement)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
	releaseBackstore(pHtmlImg);
}

void webc_ImageGraphRefreshSrc(HELEMENT_HANDLE ImageElement)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
	pHtmlImg->RefreshFromSrc();
}
void webc_ImageGraphSetBrushWidth(HELEMENT_HANDLE ImageElement, int width)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
		// store the width inside the HTMLImage object for later use
		pHtmlImg->SetBrushWidth(width);
}

void webc_ImageGraphSetPenColor(HELEMENT_HANDLE ImageElement, HTMLColor color)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
		pHtmlImg->SetDrawColor((HTMLColor)color);
}

void webc_ImageGraphDrawLine(HELEMENT_HANDLE ImageElement,int x0, int y0, int x1, int y1)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
	WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);
	_line(pGenericBitMap, x0, y0, x1, y1, pHtmlImg->GetDrawColor());
}
void webc_ImageGraphFillPolygon(HELEMENT_HANDLE ImageElement, int ArrayLength, int *coordArray)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
	WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);
	_fillPolygon(pGenericBitMap, ArrayLength, coordArray, pHtmlImg->GetDrawColor());

}
void webc_ImageGraphFillRect(HELEMENT_HANDLE ImageElement, int x, int y, int w, int h)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;
	WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);
	_fillRect(pGenericBitMap, x, y, w, h, pHtmlImg->GetDrawColor());
}
void webc_ImageGraphEllipse(HELEMENT_HANDLE ImageElement, int x, int y, int w, int h)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;

	double dx = 0.0, dy = 0.0, wx = 0.0, hy = 0.0;
	dx = (double) x;
	dy = (double) y;
	wx = (double) w;
	hy = (double) h;
	WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);

	_ellipse(pGenericBitMap, dx, dy, wx, hy, pHtmlImg->GetDrawColor());
}
void webc_ImageGraphFillEllipse(HELEMENT_HANDLE ImageElement, int x, int y, int w, int h)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;

	webc_ImageGraphEllipse(ImageElement, x, y, w, h);
}
void webc_ImageGraphArc(HELEMENT_HANDLE ImageElement, int ix, int iy, int ir1, int ir2, int ia1, int ia2)
{
	HTMLImage *pHtmlImg = (HTMLImage *) ImageElement;
	if (!ImageElement)
		return;

	double x=0.0, y=0.0, r1=0.0, r2=0.0, a1=0.0, a2;
	x=(double)ix; y=(double)iy; r1=(double)ir1; r2=(double)ir2; a1=(double)ia1; a2=(double)ia2;
	WebGraphBitmapInfo* pGenericBitMap = establishBackstore(pHtmlImg);
	_arc(pGenericBitMap, x, y, r1, r2, a1, a2, pHtmlImg->GetDrawColor());
}

}

#endif
#endif // WEBC_IMAGE_DRAW_EXTENSIONS


void webc_ImageGraphBeginDraw(HELEMENT_HANDLE ImageElement);
void webc_ImageGraphEndBeginDraw(HELEMENT_HANDLE ImageElement);
void webc_ImageGraphRefreshSrc(HELEMENT_HANDLE ImageElement);
void webc_ImageGraphSetBrushWidth(HELEMENT_HANDLE ImageElement, int width);
void webc_ImageGraphSetPenColor(HELEMENT_HANDLE ImageElement, HTMLColor color);
void webc_ImageGraphDrawLine(HELEMENT_HANDLE ImageElement,int x0, int y0, int x1, int y1);
void webc_ImageGraphFillPolygon(HELEMENT_HANDLE ImageElement, int ArrayLength, int *coordArray);
void webc_ImageGraphFillRect(HELEMENT_HANDLE ImageElement, int x, int y, int w, int h);
void webc_ImageGraphEllipse(HELEMENT_HANDLE ImageElement, int x, int y, int w, int h);
void webc_ImageGraphFillEllipse(HELEMENT_HANDLE ImageElement, int x, int y, int w, int h);
void webc_ImageGraphArc(HELEMENT_HANDLE ImageElement, int ix, int iy, int ir1, int ir2, int ia1, int ia2);

#endif // (WEBC_SUPPORT_JSCRIPT)