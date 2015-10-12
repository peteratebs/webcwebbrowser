//
// HSTRING.CPP - Methods for HTML DOM class HTMLString
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

#include "htmldoc.hpp"
#include "hstring.hpp"
#include "webfonts.hpp"
#include "webcutf.h"
#include "dhstring.hpp"
#include "webcstr.h"
#include "webcmem.h"
#include "webcassert.h"

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
// HTMLString Constructors
/*****************************************************************************/

HTMLString::HTMLString(HTMLDocument* document, WEBC_CHAR* str)
	: HTMLElement(0, document)
{
	miLength = 0;
	mpString = 0;
	mFont = 0;
	mpFirstDisplay = 0;
	mTag = HTML_TAG_STRING;
	SetString(str);
}

HTMLString::HTMLString(HTMLTagParse *td, HTMLDocument* document)
{
	WEBC_ASSERT(0);
}
HTMLString::HTMLString(HTMLString &copy)
	: HTMLElement(copy)
{
	miLength = 0;
	mpString = 0;
	if (copy.mpString)
		SetString(copy.mpString);
	// Copy the font elements
	mFont.Set(copy.GetFont());
	mpFirstDisplay = GetStringDisplay(0);
	mTag = copy.mTag;
}
/*****************************************************************************/
// HTMLString Destructor
/*****************************************************************************/

HTMLString::~HTMLString(void)
{
	webc_free_string_copy(mpString,__FILE__,__LINE__);

	// Find the first display element with index >= iIndex
	HTMLStringDisplay *pDisplay;
	while (mpFirstDisplay)
	{
		pDisplay = mpFirstDisplay->GetNextSubString();
		WEBC_DELETE(mpFirstDisplay);
		mpFirstDisplay = pDisplay;
	}
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/

WEBC_UINT16 HTMLString::GetUnitType(void)
{
	return (TU_TYPE_TEXT);
}

/******************************************************************************
* WEBC_UINT16 HTMLString::GetClear() -
*
* Parameters
*    none
*
* Description -
*
* Returns ()
******************************************************************************/
WEBC_UINT16 HTMLString::GetClear(void)
{
	return (TU_CLEAR_NONE);
}


/******************************************************************************
* WEBC_UINT16 HTMLString::GetVisibility() -
*
* Parameters
*    none
*
* Description -
*
* Returns ()
******************************************************************************/
WEBC_UINT16 HTMLString::GetVisibility(void)
{
	return (mpParent? mpParent->GetVisibility() : TU_VISIBILITY_INHERIT);
}


/******************************************************************************
* WEBC_UINT8 HTMLString::GetFloat() -
*
* Parameters
*    none
*
* Description - This method gets the Float value of this element.  The Float
*               is general horizontal positioning information of the element.
*
* Returns (WEBC_UINT8)
*	the float value
******************************************************************************/
WEBC_UINT8 HTMLString::GetFloat(void)
{
	return (TU_FLOAT_NONE);
}


/******************************************************************************
* WEBC_UINT16 HTMLString::GetPosition(void) -
*
* Parameters
*
* Description -
*
* Returns (WEBC_UINT16)
******************************************************************************/
WEBC_UINT16 HTMLString::GetPosition(void)
{
	return (TU_POSITION_STATIC);
}

/******************************************************************************
* long HTMLString::GetLeft(void) -
*
* Parameters
*
* Description -
*
* Returns (long)
******************************************************************************/
DISPLAY_INT HTMLString::GetLeft(void)
{
	return (0);
}

/******************************************************************************
* long HTMLString::GetTop(void) -
*
* Parameters
*
* Description -
*
* Returns (long)
******************************************************************************/
DISPLAY_INT HTMLString::GetTop(void)
{
	return (0);
}


/******************************************************************************
* WEBC_UINT8 HTMLString::GetVAlign() -
*
* Parameters
*    none
*
* Description - This method gets the vertical alignment type for this element
*
*
* Returns (WEBC_UINT8)
*	the vertical alignment
******************************************************************************/
WEBC_UINT8 HTMLString::GetVAlign(void)
{
	return (TU_VALIGN_BASELINE);
}


/******************************************************************************
* short HTMLString::GetZIndex() -
*
* Parameters
*    none
*
* Description - This method returns the three-dimensional position of the
*               given element.
*
*
* Returns (short)
*	the z-index
******************************************************************************/

short HTMLString::GetZIndex(void)
{
	return (0);
}


/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/


/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/


void HTMLString::Update(WEBC_PFBYTE o, WEBC_PFBYTE data)
{
}

/*
void HTMLString::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLString::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLElement *HTMLString::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLString::Reset(void)
{
}
*/
/*
int HTMLString::SetProperty(HTMLAttributeType Property, PFCHAR value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		default:
			break;
	}

	if (call_base)
	{
		return (HTMLVisible::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}
*/

CSSValueType HTMLString::GetStyleFromCSS (
		CSSPropertyType property,
		CSSPropertyValue* value
	)
{
	return (CSS_VALUE_UNKNOWN);
}

/*****************************************************************************/
// HTMLString-Specific Methods
/*****************************************************************************/
#define WEBC_SUPPORT_EXTENDED_CHARS 1

void HTMLString::SetString(WEBC_CHAR *pStr)
{
	if (pStr)
	{
		WEBC_CHAR *pStrCopy = webc_malloc_string_copy(pStr,__FILE__,__LINE__);

		if (pStrCopy)
		{
			webc_free_string_copy(mpString, __FILE__, __LINE__);
			miLength = 0;
			mpString = pStrCopy;

			WEBC_CHAR *from = mpString, *to = mpString;
			for (; *from; from++)
			{
			  #if (!WEBC_SUPPORT_UNICODE && !WEBC_SUPPORT_EXTENDED_CHARS)
	 			if (((*from < 127) && (*from > 0)) || (*from == (WEBC_CHAR) HTML_NBSP_CHAR))
			  #endif // WEBC_SUPPORT_UNICODE
	 			{
					*(to++) = *from;
					miLength++;
				}
			}
			*to = '\0';
		}
	}
}


WEBC_CHAR *HTMLString::GetString(void)
{
	return (mpString);
}

WEBC_CHAR *HTMLString::GetString(WEBC_SIZE iCharOffset)
{
	return (&mpString[iCharOffset]);
}


WEBC_SIZE HTMLString::GetStringLength()
{
	return (miLength);
}

WebGraphFont *HTMLString::GetFont(void)
{
	if (mFlags & HELEM_FLAG_FONT_CACHED)
	{
		return (mFont.Get());
	}

	mFont.Set((mpParent)? mpParent->GetFont() : 0);
	//SetFlag(HELEM_FLAG_FONT_CACHED);
	return (mFont.Get());
}

CSSTextDecoration HTMLString::GetTextDecoration (void)
{
	return (mpParent->GetTextDecoration());
}

HTMLColor HTMLString::GetColor (void)
{
	if (mpParent)
	{
		return (mpParent->GetColor());
	}

	// default color is black
	return (HTML_RGBAToColor(0,0,0,0));
}

HTMLColor HTMLString::GetBackgroundColor (void)
{
	// if we have a non-inline parent, then it will draw the background color for us
	//  therefore, only get the background color if our parent is inline
	if (mpParent && (mpParent->GetUnitType() == TU_TYPE_INLINE))
	{
		return mpParent->GetBackgroundColor();
	}

	// default background is transparent
	return (HTML_RGBAToColor(0,0,0,255));
}

/******************************************************************************
* HTMLString::GetOutlineColor() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
HTMLColor HTMLString::GetOutlineColor (void)
{
	if (mpParent)
	{
		return mpParent->GetOutlineColor();
	}

	// default is transparent
	return (HTML_RGBAToColor(0,0,0,255));
}


/******************************************************************************
* HTMLString::GetOutlineStyle() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
CSSBorderStyle HTMLString::GetOutlineStyle(void)
{
	return ((mpParent)? mpParent->GetOutlineStyle() : CSS_BORDER_STYLE_NONE);
}


/******************************************************************************
* HTMLString::GetOutlineWidth() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
void HTMLString::GetOutlineWidth(CSSBorderThickness *width)
{
	if (mpParent)
	{
		mpParent->GetOutlineWidth(width);
	}
	else
	{
		width->type = CSS_BORDER_THICKNESS_ABSOLUTE;
		width->val.absolute = CSS_BORDER_THICKNESS_THIN;
	}
}

/*****************************************************************************/
// Peg String transforms
/*****************************************************************************/

HTMLStringDisplay *HTMLString::GetStringDisplay (HTMLStringDisplay *pPrev)
{
	HTMLStringDisplay *pDisplay;

	if (pPrev)
	{
		pDisplay = pPrev->GetNextSubString();
	}
	else
	{
		pDisplay = mpFirstDisplay;
	}

	// if there is no display element with index >= iIndex, create one
	if (!pDisplay)
	{
		WEBC_NEW_VERBOSE(pDisplay, HTMLStringDisplay(this),"HTMLStringDisplay");
		if (pPrev)
		{
			pPrev->SetNextSubString(pDisplay);
		}
		else
		{
			mpFirstDisplay = pDisplay;
		}
	}

	return (pDisplay);
}

TextUnit *HTMLString::GetTextUnit (void)
{
	HTMLStringDisplay *display = GetStringDisplay(0);
	if (display)
	{
		display->SetStringPosition(0, miLength);
	}
	return (display);
}


/******************************************************************************
* void HTMLString::NotifyDisplay() -
*
* Parameters
*    none
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/
void HTMLString::NotifyDisplay (NotifyEventType event)
{
	NotifyEvent what(NOTIFIER_TYPE_HTML_ELEMENT, event);
	HTMLStringDisplay *display = mpFirstDisplay;
	while (display)
	{
		display->Notify(this, &what);
		display = display->GetNextSubString();
	}
}

/******************************************************************************
* void HTMLString::Refresh() -
*
* Parameters
*    none
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/
void HTMLString::Refresh(WEBC_BOOL draw)
{
	for (HTMLStringDisplay *display = mpFirstDisplay; display; display = display->GetNextSubString())
	{
		display->Invalidate();
	}

	if (draw)
	{
		if (GetDocument())
		{
			if (GetDocument()->GetDisplayManager())
			{
				GetDocument()->GetDisplayManager()->Refresh();
			}
		}
	}
}

DisplayElement *HTMLString::GetDisplayElement (WEBC_BOOL create)
{
	return mpFirstDisplay;
}

DisplayElement* HTMLString::GetLastDisplayElement (void)
{
	HTMLStringDisplay* current;
	HTMLStringDisplay* prev = 0;

	for (current = mpFirstDisplay;
	     current && current->mpParent;
	     current = current->GetNextSubString())
	{
		prev = current;
	}

	return (prev);
}

/******************************************************************************
* int HTMLString::GetDocumentDisplayRect() -
*
* Parameters
*
* Description
*
* Returns (int)
*    0 if successful, -1 otherwise (no DisplayElement)
******************************************************************************/

int HTMLString::GetDocumentDisplayRect (WebRect *rect, int n)
{
WebRect displayRect;
HTMLStringDisplay *display = mpFirstDisplay;

	while (display && n > 0)
	{
		if (display->GetStringIndex() + display->GetStringLength() >= miLength)
		{
			display = 0;
			break;
		}
		n--;
		display = display->GetNextSubString();
	}

	if (display)
	{
		displayRect.Set(&display->mRect);

		if (display->mpParent)
		{
			DISPLAY_INT x, y;

			display->mpParent->GetDisplayPosition(display, &x, &y);
			displayRect.MoveTo(x,y);
		}

		rect->Set(&displayRect);

		return (0);
	}

	return (-1);
}

#if (WEBC_SUPPORT_INNER_HTML)

/******************************************************************************
* HTMLString::SetInnerHTML() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLString::SetInnerHTML (WEBC_CHAR* innerHTML)
{
}

/******************************************************************************
* HTMLString::SetOuterHTML() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLString::SetOuterHTML (WEBC_CHAR* outerHTML)
{
}

/******************************************************************************
* HTMLString::AppendOuterHTML() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLString::AppendOuterHTML (WebString* toString)
{
	toString->appendHtmlEscaped(GetString(), GetStringLength());
}

/******************************************************************************
* HTMLString::AppendInnerHTML() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLString::AppendInnerHTML (WebString* toString)
{
}

/******************************************************************************
* HTMLString::AppendRenderedText() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLString::AppendRenderedText (WebString* toString)
{
}

/******************************************************************************
* HTMLString::SetRenderedText() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLString::SetRenderedText(WEBC_CHAR*  textVal)
{

}

#endif // WEBC_SUPPORT_INNER_HTML
