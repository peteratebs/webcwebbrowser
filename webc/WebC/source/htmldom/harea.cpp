//
// HAREA.CPP - Methods for HTML DOM class HTMLArea
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
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

#include "htmldom.hpp"
#include "wgkeys.hpp"
#include "harea.hpp"
#include "htmldoc.hpp"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "htmlfind.hpp"
#include "webcmem.h"


/*****************************************************************************/
// Local Constants
/*****************************************************************************/

/*****************************************************************************/
// Local type definitions
/*****************************************************************************/

/*****************************************************************************/
// Local Functions
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// HTMLArea Constructors
/*****************************************************************************/

HTMLArea::HTMLArea(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElement(td, pDocument)
{
	mpAreaDisplayList = 0;
	mpHRef = 0;
	mpName = 0;
	mpTarget = 0;
	mCoordinateList = 0;
	mCoordinateCount = 0;
	mShape = HTML_SHAPE_UNKNOWN;
	mpStrShape = 0;
	mAlt = 0;
	mbNoHRef = WEBC_FALSE;
	mAccessKey = 0;
	mTabIndex = 0;
}

/*****************************************************************************/
// HTMLArea Destructor
/*****************************************************************************/

HTMLArea::~HTMLArea(void)
{
	webc_free_string_copy(mpHRef, __FILE__, __LINE__);
	webc_free_string_copy(mpName, __FILE__, __LINE__);
	webc_free_string_copy(mpTarget, __FILE__, __LINE__);
	if(mAlt)
	{
		webc_free_string_copy(mAlt, __FILE__, __LINE__);
	}
	WEBC_FREE(mCoordinateList);

	HTMLAreaDisplay *next;
	while (mpAreaDisplayList)
	{
		next = mpAreaDisplayList->mpAreaNext;
		WEBC_DELETE(mpAreaDisplayList);
		mpAreaDisplayList = next;
	}
	webc_free_string_copy(mpStrShape, __FILE__, __LINE__);
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/


/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/


/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

HTMLEventStatus HTMLArea::DefaultEvent(HTMLEvent *pEvent)
{
HTMLBrowser *pBrowser = GetBrowser();
HTMLDocument *pDoc = GetDocument();
WEBC_CHAR *pHRef = HRef(), *pTarget = Target();

    WEBC_DEBUG_ReceiveEvent(this, pEvent);

	switch (pEvent->type)
	{
		case HTML_EVENT_FOCUS:
		{
			HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 1);

			FindElement(&f);
		}
		// intentional fall-through
		case HTML_EVENT_MOUSEMOVE:
			if (pBrowser)
			{
				pBrowser->SetTempStatus(HRef(), this);
			}
			return (HTML_EVENT_STATUS_DONE);

		case HTML_EVENT_UNFOCUS:
		{
			HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 0);

			FindElement(&f);
		}
		// intentional fall-through
		case HTML_EVENT_MOUSEOUT:
			if (pBrowser)
			{
				pBrowser->ClearTempStatus(this);
			}
			return(HTML_EVENT_STATUS_DONE);

		case HTML_EVENT_KEYDOWN:
			{
				HTMLEventStatus retval = DefaultInputKeyHandler(pEvent);
				if (retval != HTML_EVENT_STATUS_CONTINUE)
				{
					return (retval);
				}
			}
			break;

		case HTML_EVENT_KEYPRESS:
			if (pEvent->data.key != WGK_CR)
			{
				break;
			}
			//else fall-through
		case HTML_EVENT_CLICK:
			if (!pDoc)
			{
				return(HTML_EVENT_STATUS_DONE);
			}

			if (!pTarget)
			{
				pTarget = pDoc->BaseTarget();
			}

			if (pHRef)
			{
				HTMLDocument *pTargDoc;
				pTargDoc = GetFrameDocument(pTarget);

				URLDescriptor urlParse(pHRef, pDoc->BaseURL());
				// don't save the current url to the history if protocol is JavaScript,
				//  because javascript:// url's are not really page locations
				if (urlParse.getProtocol() != WEBC_PROTOCOL_JSCRIPT)
				{
					pTargDoc->SaveLocation();
				}

				pTargDoc->SetSrc(pHRef);
				pTargDoc->Update(1,0);
				return(HTML_EVENT_STATUS_HALT);
			}
			return (HTML_EVENT_STATUS_DONE);

		default:
			// ignore other messages
			break;
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

int HTMLArea::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_HREF:
			SetHRef(value);
			return (1);

		case HTML_ATTRIB_NOHREF:
			SetNoHRef(WEBC_TRUE);
			return (1);

		case HTML_ATTRIB_NAME:
			SetName(value);
			return (1);

		case HTML_ATTRIB_TARGET:
			SetTarget(value);
			return (1);

		case HTML_ATTRIB_SHAPE:
			mShape = HTML_ParseShapeType(value, 0);
			return (1);

		case HTML_ATTRIB_COORDS:
			SetCoordinates(value);
			return (1);

		case HTML_ATTRIB_ALT:
			SetAlt(value);
			return (1);

		case HTML_ATTRIB_ACCESSKEY:
			SetAccessKey(value[0]);
			return (1);

		case HTML_ATTRIB_TABINDEX:
		{
			long i = webc_atoi(value);
			SetTabIndex(UTIL_SAFE_LONG_TO_SHORT(i));
			return (1);
		}

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLElement::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}

/******************************************************************************
* void HTMLArea::SetHRef(WEBC_CHAR *pHRef) - Set href attribute
*
* Parameters
*    pHRef - NULL-terminated string: the new value of href
*
* Description
*    Creates a local copy of the given string.
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLArea::SetHRef(WEBC_CHAR *pHRef)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pHRef, __FILE__, __LINE__);

	webc_free_string_copy(mpHRef, __FILE__, __LINE__);
	mpHRef = pStr;
}

/******************************************************************************
* void HTMLArea::SetName(WEBC_CHAR *pName) - Set name attribute
*
* Parameters
*    pName - NULL-terminated string: the new value of name
*
* Description
*    Creates a local copy of the given string.
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLArea::SetName(const WEBC_CHAR *pName)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pName, __FILE__, __LINE__);

	webc_free_string_copy(mpName, __FILE__, __LINE__);
	mpName = pStr;
}

/******************************************************************************
* void HTMLArea::SetTarget(WEBC_CHAR *pTarget) - Set target attribute
*
* Parameters
*    pName - NULL-terminated string: the new value of target
*
* Description
*    Creates a local copy of the given string.
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLArea::SetTarget(WEBC_CHAR *pName)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pName, __FILE__, __LINE__);

	webc_free_string_copy(mpTarget, __FILE__, __LINE__);
	mpTarget = pStr;
}

/******************************************************************************
* void HTMLArea::SetAlt(WEBC_CHAR *pTarget) - Set alternat text attribute
*
* Parameters
*    pAlt - NULL-terminated string: the new value
*
* Description
*    Creates a local copy of the given string.
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLArea::SetAlt(const WEBC_CHAR *pAlt)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(pAlt, __FILE__, __LINE__);
	webc_free_string_copy(mAlt, __FILE__, __LINE__);
	mAlt = pStr;
}

void HTMLArea::SetShape(const WEBC_CHAR *pStrShape)
{
	mShape = HTML_ParseShapeType((HTML_CHAR *)pStrShape, 0);
}


WEBC_CHAR *HTMLArea::ShapeStr(void)
{
	const char *pCShape = HTML_ShapeTypeToString(mShape);

	if(mpStrShape)
		WEBC_FREE(mpStrShape);

	if(pCShape)
	{
		mpStrShape = (WEBC_CHAR*) WEBC_MALLOC((webc_c_strlen(pCShape) + 1) * sizeof(WEBC_CHAR));
		if(mpStrShape)
			webc_c_strcpy(mpStrShape, pCShape);
	}
	else
	{
		mpStrShape = (WEBC_CHAR*) WEBC_MALLOC(8 * sizeof(WEBC_CHAR));
		if(mpStrShape)
			webc_c_strcpy(mpStrShape, "default");
	}
	return mpStrShape;
}
/*****************************************************************************/
// HTMLArea-Specific Methods
/*****************************************************************************/

/******************************************************************************
* WEBC_UINT8 HTMLArea::Contains(int x, int y) - Check if a point lies within area
*
* Parameters
*    x - X coordinate of the point to check
*    y - Y coordinate of the point to check
*
* Description
*    Checks to see if (x,y) lies within this area.  All coordinates are
*    relative to the upper left corner of the parent object associated with
*    this area.
*
* Returns (WEBC_UINT8)
*    Zero if the point lies outside the area; non-zero otherwise
******************************************************************************/

WEBC_UINT8 HTMLArea::Contains(DISPLAY_INT x, DISPLAY_INT y)
{
	int coor_cnt;
	int left = 0, top = 0, right = 0, bottom = 0;
	int xdiff, ydiff;

	// do sanity check on number of points
	switch (mShape)
	{
		case HTML_SHAPE_CIRCLE:
			if (mCoordinateCount != 3)
			{
				// mCoordinateCount must be 3
				return 0;
			}
			break;

		case HTML_SHAPE_POLY:
			if ((mCoordinateCount & 1) != 0)
			{
				// mCoordinateCount must be even
				return 0;
			}
			break;

		case HTML_SHAPE_RECT:
			if (mCoordinateCount != 4)
			{
				// mCoordinateCount must be 4
				return 0;
			}
			break;
        default:
            break;
	}

	if (!mCoordinateList)
	{
		return (0);
	}

	// for rectangle, the coordinate where specified as
	// (left,top);(right,bottom) boundaries of square
	if (mShape == HTML_SHAPE_RECT)
	{
		left	= HTML_LengthGetValue(mCoordinateList[0]);
		top     = HTML_LengthGetValue(mCoordinateList[1]);
		right	= HTML_LengthGetValue(mCoordinateList[2]);
		bottom  = HTML_LengthGetValue(mCoordinateList[3]);
	}
	// for poly, the coordinate where specified as
	// a set of (x,y) coordinates defining the end points
	// of the shape
	else if (mShape == HTML_SHAPE_POLY)
	{
		DISPLAY_INT cx, cy;

		left  = right = HTML_LengthGetValue(mCoordinateList[0]);
		top   = bottom = HTML_LengthGetValue(mCoordinateList[1]);

		// convert the (x,y) points to boundaries of
		// the smallest square the shape will fit into
		// tbd: this is not the optimal solution but
		//      possibly it is good enough (and it is easy to do)
		for (coor_cnt = 0; coor_cnt < mCoordinateCount; coor_cnt+=2)
		{
			// left = min x; right = max x
			cx = HTML_LengthGetValue(mCoordinateList[coor_cnt]);
			cy = HTML_LengthGetValue(mCoordinateList[coor_cnt+1]);

			if (cx < left)
			{
				left = cx;
			}
			if (cx > right)
			{
				right = cx;
			}

			// top = min y; bottom = max y
			if (cy < top)
			{
				top = cy;
			}
			if (cy > bottom)
			{
				bottom = cy;
			}
		}
	}

	if ( (mShape == HTML_SHAPE_RECT) ||
         (mShape == HTML_SHAPE_POLY) )
	{
		// check if (x,y) is within the area
		if(x>=left && x<=right && y>=top && y<=bottom)
		{
			return(1);
		}
	}
	else if (mShape == HTML_SHAPE_CIRCLE)
	{
		DISPLAY_INT r = HTML_LengthGetValue(mCoordinateList[2]);

		// determine if (x,y) is within (xc,yc) with radius r
		// if pow(x-xc,2) + pow(y-yc,2) <= pow(r,2) then
		// it is within the circle
		xdiff = x - HTML_LengthGetValue(mCoordinateList[0]);
		ydiff = y - HTML_LengthGetValue(mCoordinateList[1]);
		if (xdiff*xdiff + ydiff*ydiff <= r*r)
		{
			return (1);
		}
	}

	return (0);
}

DisplayElement *HTMLArea::GetAreaDisplay (HTMLElement *parent, WEBC_BOOL create)
{
HTMLAreaDisplay *display = mpAreaDisplayList;

	// first search our list of existing display elements associated with this
	//  element
	while (display)
	{
		if (display->GetAreaParent() == parent)
		{
			break;
		}
		display = display->mpAreaNext;
	}

	// if not found, and create flag is set, then create a new one and link
	//  it into the list
	if (!display && create)
	{
		WEBC_NEW_VERBOSE(display, HTMLAreaDisplay(this, parent),"GetAreaDisplay");
		if (display)
		{
			display->mpAreaNext = mpAreaDisplayList;
			mpAreaDisplayList = display;
		}
	}

	return (display);
}

void HTMLArea::SetCoordinates(WEBC_CHAR *pStrCoordValues)
{
	if (mCoordinateList != 0)
	{
		WEBC_FREE(mCoordinateList);
		mCoordinateList = 0;
	}

	mCoordinateCount = HTML_CountLengthList(pStrCoordValues);
	if (mCoordinateCount > 0)
	{
		mCoordinateList = (HTMLLength*) WEBC_MALLOC(sizeof(HTMLLength)*mCoordinateCount);
		if (mCoordinateList)
		{
			HTML_ParseLengthList(pStrCoordValues, mCoordinateList, mCoordinateCount);
			if (mShape == HTML_SHAPE_UNKNOWN)
			{
				if (mCoordinateCount == 4)
				{
					mShape = HTML_SHAPE_RECT;
				}
				else if (mCoordinateCount == 3)
				{
					mShape = HTML_SHAPE_CIRCLE;
				}
				else if ((mCoordinateCount & 1) == 0)
				{
					mShape = HTML_SHAPE_POLY;
				}
			}
		}
		else
		{
			mCoordinateCount = 0;
		}
	}
}
WEBC_UINT16 HTMLArea::AccessKey(void)
{
	return (mAccessKey);
}

void HTMLArea::SetAccessKey(WEBC_UINT16 key)
{
	mAccessKey = key;
}

void HTMLArea::SetTabIndex(short iIndex)
{
	mTabIndex = iIndex;
}

short HTMLArea::TabIndex(void)
{
	return (mTabIndex);
}
