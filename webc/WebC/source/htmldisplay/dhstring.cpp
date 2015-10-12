//
// DHSTRING.CPP - Methods for class HTMLInputButtonDisplay
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "dhelem.cpp"
#include "dhstring.hpp"
#include "hstring.hpp"
#include "webcutf.h"
#include "wgcolors.hpp"
#include "dmanager.hpp"
#include "dbgtimer.h"
#include "util.h"
#include "textstyle.hpp"
#include "hlabel.hpp"
//#include "delement.cpp"



/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define STRING_ELEMENT            ((HTMLString *) mpHtmlElement)
#define STRING_PTR                (STRING_ELEMENT->GetString() + miIndex);
#define STRING_PADDING            1
#define STRING_CLIP_VISIBLE(P,L)  if(P[0]=='\r'){if(P[L-1]=='\n'){P++;L-=2;}\
                                  else{P++;L--;}}else{if(P[L-1]=='\n'){L--;}}


/*****************************************************************************/
// Local Types
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// Local functions
/*****************************************************************************/

HTMLStringDisplay::HTMLStringDisplay (HTMLString *pStrElem)
	: HTMLElementDisplay<DisplayElement> (pStrElem)
{
	mTextStyle = 0;
	mpNextSub = 0;
	miIndex = -1;
	miLength = 0;
	miFirstBreak = -1;
	miLastBreak = -1;

	SetStringElement(pStrElem);
}

HTMLStringDisplay::~HTMLStringDisplay ()
{
	if (mTextStyle)
	{
		mTextStyle->release();
	}
}

WEBC_UINT16 HTMLStringDisplay::GetUnitType(void)
{
	WEBC_CHAR* str = STRING_PTR;
	if (str && miLength == 1 && *str == (WEBC_CHAR) ' ')
	{
		return (TU_TYPE_NODISPLAY);
	}
	return (TU_TYPE_TEXT);
}

DisplayElement* HTMLStringDisplay::FocusDefault (void)
{
	HTMLLabel* label = (HTMLLabel*) (mpHtmlElement? mpHtmlElement->mpParent : 0);

	if (label && label->Type() == HTML_LABEL_ELEMENT)
	{
		HTMLElement* elem = label->ForElement();
		DisplayElement* display = elem? elem->GetDisplayElement(WEBC_FALSE) : 0;
		if (display)
		{
			return display;
		}
	}

	return (HTMLElementDisplay<DisplayElement>::FocusDefault());
}


void HTMLStringDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLSTRINGDISPLAY_DRAW);

	if (GetVisibility() == TU_VISIBILITY_HIDE)
	{
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLSTRINGDISPLAY_DRAW);
		return;
	}

	WebRect clip, screenRect(mRect);
	pGC->GetClip(&clip);
	screenRect.MoveTo(x,y);
	if (!clip.Overlaps(&screenRect))
	{
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLSTRINGDISPLAY_DRAW);
		return;
	}

	HTMLString *pStringElement = (HTMLString *) mpHtmlElement;
	WebChar *str = pStringElement->GetString() + miIndex;
	WebChar *fillJStr = WEBC_NULL;
	long fillJStrLen = 0;
	long strLen = miLength;
	WebFont font = GetFont();

	// find out if this text needs to be fill-justified
	if(mpNextSub) // make sure this is not the last line of the block, so we don't try to justify it
	{
		CSSPropertyValue propVal;

		if( ((CSS_VALUE_SPECIFIED == GetStringElement()->Parent()->GetStyleFromCSS(CSS_PROPERTY_TEXT_ALIGN, &propVal)) &&
		    (propVal.textAlign == CSS_TEXT_ALIGN_JUSTIFY)) ||
		    ((CSS_VALUE_SPECIFIED == GetStringElement()->Parent()->GetStyleFromHTML(CSS_PROPERTY_TEXT_ALIGN, &propVal)) &&
		    (propVal.textAlign == CSS_TEXT_ALIGN_JUSTIFY))  )
		{
			DISPLAY_INT desiredWidth = mpParent->Width();
			fillJStr = MakeFillJustifiedString(str, strLen, desiredWidth, pGC, fillJStrLen);
		}
	}

	if (font)
	{
		STRING_CLIP_VISIBLE(str,strLen);

		WebRect textRect(screenRect);
		textRect.Shift(STRING_PADDING, STRING_PADDING);
		textRect.SizeBy(-(STRING_PADDING*2), -(STRING_PADDING*2));

		if(fillJStr && fillJStrLen)
		{
			mTextStyle->renderText(&textRect, fillJStr, fillJStrLen, pGC);
			WEBC_FREE(fillJStr);
		}
		else
		{
			mTextStyle->renderText(&textRect, str, strLen, pGC);
		}
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLSTRINGDISPLAY_DRAW);
}

WebChar * HTMLStringDisplay::MakeFillJustifiedString(WebChar *str, long strLen, DISPLAY_INT desiredWidth, WebGraphics *pGC, long &retStrLen)
{
	const WebChar *strSpace = (const WebChar *)L" ";
	const WebChar *test1 = (const WebChar *)L". .";
	const WebChar *test2 = (const WebChar *)L"..";
	long breakCount=0, numSpaces, numSpacePerBreak, remainderSpacePerBreak;
	WebChar *retVal, *curPos;

	// figure out how many pixels wide a space is
	//DISPLAY_INT spaceWidth = pGC->TextWidth(strSpace, GetFont());
	DISPLAY_INT spaceWidth = pGC->TextWidth(test1, GetFont()) - pGC->TextWidth(test2, GetFont());
	DISPLAY_INT remainingPad = desiredWidth - pGC->TextWidthLen(str, GetFont(), strLen);
	// figure out how many spaces/breaks are in this run of text
	for(int i = 0; i < strLen; i++)
	{
		if(str[i] == strSpace[0])
			breakCount++;
	}

	// figure out how many spaces we need to add
	numSpaces = remainingPad / spaceWidth;
	numSpacePerBreak = numSpaces / breakCount;
	remainderSpacePerBreak = numSpaces % breakCount;

	// distribute the extra spaces amoungst the breaks
	retVal = (WebChar *) WEBC_MALLOC_VERBOSE((strLen + numSpaces + 1) * sizeof(WebChar),"MakeFillJustifiedString");
	curPos = retVal;
	retStrLen = 0;
	for(int i = 0; i < strLen; i++)
	{
		*curPos++ = str[i];
		retStrLen++;
		if(str[i] == strSpace[0])
		{
			for(int n = 0; n < numSpacePerBreak; n++)
			{
				*curPos++ = strSpace[0];
				retStrLen++;
			}
			breakCount--;
			if(remainderSpacePerBreak)
			{
				*curPos++ = strSpace[0];
				retStrLen++;
				remainderSpacePerBreak--;
			}
		}
	}

	*curPos = 0;

	// return the resultant string
	return retVal;
}

HTMLEventStatus HTMLStringDisplay::Event (HTMLEvent *e)
{
    WEBC_DEBUG_ReceiveEvent(this, e);
	return (mpHtmlElement->Event(e));
}

WEBC_UINT16 HTMLStringDisplay::GetBreak(void)
{
	HTMLString *pStringElement = (HTMLString *) mpHtmlElement;
	WebChar *str = pStringElement->GetString() + miIndex;
	WEBC_UINT16 tu_break = 0;

	if (str[0] == (WebChar) '\r')
	{
		tu_break |= TU_BREAK_BEFORE;
	}

	if (str[miLength-1] == (WebChar) '\n')
	{
		tu_break |= TU_BREAK_AFTER;
	}

	return (tu_break);
}

DISPLAY_INT HTMLStringDisplay::GetBaseline(void)
{
	WebFont font = GetFont();
	if (font)
	{
		return WEB_FONT_BASELINE(font);
	}
	return ((GetPixelHeight() << 2) / 5 + 1);
}

static int _GetBreakIndexBefore(WebChar* str, int index)
{
	WebChar *s = str + index;

	if ((s[0] == '\0') || (s[0] == '\r'))
	{
		return (0);
	}

	while ((s > str) &&
	       (s[0] != '\n') &&
	       (s[1] != '\r') &&
	       (!IS_BREAK_CHAR(WEBCHAR_TO_UNICHAR(s[0]))))
	{
		s--;
	}

	return (s - str);
}

void HTMLStringDisplay::SetDimensions (WebGraphics *gc)
{
	WebFont font = GetFont();
	if (font)
	{
		HTMLString *pStringElement = (HTMLString *) mpHtmlElement;
		WebChar *pStr = pStringElement->GetString() + miIndex;
		long strLen = miLength;

		STRING_CLIP_VISIBLE(pStr,strLen);

		DISPLAY_INT w = (strLen > 0)? gc->TextWidthLen(pStr, font, strLen) : 0;

		if (w > 0)
		{
			w += (STRING_PADDING * 2);
		}

		SizeTo(w, WEB_FONT_HEIGHT(font) + (STRING_PADDING * 2));
	}
}

void HTMLStringDisplay::SetStringPosition(WEBC_SIZE iIndex, WEBC_SIZE iLength, WebGraphics *gc)
{
	if ((miIndex != iIndex) || (miLength != iLength))
	{
		miIndex = iIndex;
		miLength = iLength;

		HTMLString *pStringElement = (HTMLString *) mpHtmlElement;
		WebChar *str = pStringElement->GetString() + miIndex;

		miFirstBreak = -1;
		miLastBreak = -1;
		for (WEBC_SIZE n=0; n<miLength; n++)
		{
			if (IS_BREAK_CHAR(str[n]))
			{
				if (miFirstBreak < 0)
				{
					miFirstBreak = n;
				}

				miLastBreak = n;
			}
		}

		gc = gc? gc : (GetManager()? GetManager()->GetGraphics() : 0);

		if (gc)
		{
			SetDimensions(gc);
		}
	}
}

WEBC_SIZE HTMLStringDisplay::GetStringIndex (void)
{
	return (miIndex);
}

WEBC_SIZE HTMLStringDisplay::GetStringLength (void)
{
	return (miLength);
}

HTMLStringDisplay *HTMLStringDisplay::GetNextSubString()
{
	return (mpNextSub);
}

void HTMLStringDisplay::SetNextSubString(HTMLStringDisplay *pNext)
{
	mpNextSub = pNext;
}

WEBC_SIZE HTMLStringDisplay::FindMaxLength (WebChar *pStr)
{
	WebChar *s = (pStr[0] == '\r')? pStr+1 : pStr;

	for (; ; s++)
	{
		if (*s == (WebChar) '\n')
		{
			return (s - pStr + 1);
		}

		if ((*s == (WebChar) 0) || (*s == (WebChar) '\r'))
		{
			return (s - pStr);
		}
	}
}

WEBC_SIZE HTMLStringDisplay::FindMinLength (WebChar *pStr, WEBC_BOOL bWrap)
{
	if (!bWrap)
	{
		return (FindMaxLength(pStr));
	}

	WebChar *s = (pStr[0] == '\r')? pStr+1 : pStr;

	for (; ; s++)
	{
		if ((*s == (WebChar) 0) || (*s == (WebChar) '\r'))
		{
			return (s - pStr);
		}

		if ((*s == (WebChar) '\n') || IS_BREAK_CHAR(WEBCHAR_TO_UNICHAR(*s)))
		{
			return (s - pStr + 1);
		}
	}
}

HTMLString *HTMLStringDisplay::GetStringElement (void)
{
	return ((HTMLString *) mpHtmlElement);
}

void HTMLStringDisplay::SetStringElement (HTMLString *s)
{
	SetHtmlElement(s);
	if (mpHtmlElement && mpHtmlElement->mpParent)
	{
		if (mTextStyle)
		{
			mTextStyle->release();
		}
		mTextStyle = mpHtmlElement->mpParent->GetTextDisplayStyle();
	}
	miIndex = -1;
}

void HTMLStringDisplay::SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore)
{
	if (GetManager() != parent->GetManager())
	{
		HTMLElementDisplay<DisplayElement>::SetParentDisplay(parent, insertBefore);
		SetElementStyle();
		WebGraphics *gc = GetGraphics();
		if (gc)
		{
			SetDimensions(gc);
		}
	}
	else
	{
		HTMLElementDisplay<DisplayElement>::SetParentDisplay(parent, insertBefore);
	}
}

void HTMLStringDisplay::SetDisplayContext(DisplayElement *parent)
{
	if (GetManager() != parent->GetManager())
	{
		HTMLElementDisplay<DisplayElement>::SetDisplayContext(parent);
		SetElementStyle();
		WebGraphics *gc = GetGraphics();
		if (gc)
		{
			SetDimensions(gc);
		}
	}
	else
	{
		HTMLElementDisplay<DisplayElement>::SetDisplayContext(parent);
	}
}

WEBC_BOOL HTMLStringDisplay::SetElementStyle (void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLSTRINGDISPLAY_SETELEMENTSTYLE);

	WEBC_BOOL result = WEBC_FALSE;
	WebGraphics *gc = GetGraphics();

	if (mpHtmlElement && mpHtmlElement->mpParent)
	{
		ITextDisplayStyle* newTextStyle = mpHtmlElement->mpParent->GetTextDisplayStyle();
		if (!mTextStyle || !mTextStyle->equals(newTextStyle))
		{
			result = WEBC_TRUE;
			if (mTextStyle)
			{
				mTextStyle->release();
			}

			mTextStyle = newTextStyle;

			if (gc)
			{
				SetDimensions(gc);
			}
		}
		else
		{
			if (newTextStyle)
			{
				newTextStyle->release();
			}
		}
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLSTRINGDISPLAY_SETELEMENTSTYLE);
	return (result);
}

WebFont HTMLStringDisplay::GetFont (void)
{
	return (mTextStyle? mTextStyle->getFont() : 0);
}

WEBC_BOOL HTMLStringDisplay::BreakAtOrBefore(DISPLAY_INT iPosition, TextUnit **firstHalf, TextUnit **secondHalf)
{
	if (miFirstBreak < 0)
	{
		return (WEBC_FALSE);
	}

	HTMLString *pStringElement = (HTMLString *) mpHtmlElement;
	WebChar *pStr = pStringElement->GetString() + miIndex;

	WebGraphics *gc = GetGraphics();
	if (!gc)
	{
		return (WEBC_FALSE);
	}

	WebFont font = GetFont();
	if (!font)
	{
		return (WEBC_FALSE);
	}

	long lowIndex = 0;
	long highIndex = miLength - 1;
	long midIndex=0;
	DISPLAY_INT w = 0;

	while (highIndex >= lowIndex)
	{
		midIndex = (highIndex + lowIndex) >> 1;
		if (midIndex < miFirstBreak)
		{
			return (WEBC_FALSE);
		}

		WebChar* str = pStr;
		long strLen = midIndex + 1;
		STRING_CLIP_VISIBLE(str,strLen);
		w = gc->TextWidthLen(str, font, strLen) + STRING_PADDING*2;

		if (w == iPosition)
		{
			break;
		}

		if (w > iPosition)
		{
			highIndex = midIndex - 1;
		}
		else
		{
			lowIndex = midIndex + 1;
		}
	}

	midIndex = _GetBreakIndexBefore(pStr, midIndex);
	if (midIndex < miFirstBreak)
	{
		return (WEBC_FALSE);
	}

	return (_BreakAt(midIndex, firstHalf, secondHalf));
}

DISPLAY_INT HTMLStringDisplay::MinBreakWidth (void)
{
	if (miFirstBreak < 0)
	{
		return (mRect.Width());
	}

	WebGraphics *gc = GetGraphics();
	WebFont font = GetFont();
	if (gc && font)
	{
		WebChar *s = STRING_PTR;
		long len = miFirstBreak;
		STRING_CLIP_VISIBLE(s,len);
		return (gc->TextWidthLen(s, font, len) + (STRING_PADDING*2));
	}

	return (0);
}


WEBC_BOOL HTMLStringDisplay::BreakAtFirstPoint(TextUnit **firstHalf, TextUnit **secondHalf)
{
	return (_BreakAt(miFirstBreak, firstHalf, secondHalf));
}

WEBC_BOOL HTMLStringDisplay::BreakAtLastPoint(TextUnit **firstHalf, TextUnit **secondHalf)
{
	return (_BreakAt(miLastBreak, firstHalf, secondHalf));
}

WEBC_BOOL HTMLStringDisplay::_BreakAt (int index, TextUnit **firstHalf, TextUnit **secondHalf)
{
	if (index < 0)
	{
		return (WEBC_FALSE);
	}

	HTMLStringDisplay *nextDisplay;
	WebChar *s = STRING_PTR;

	if ((s[index] == ' ') || (s[index] == '\t'))
	{
		if ((index + 1) < miLength)
		{
			if (index > 0)
			{
				// space or tab in the middle of the string
				nextDisplay = STRING_ELEMENT->GetStringDisplay(this);
				if (nextDisplay)
				{
					nextDisplay->SetStringPosition(miIndex + (index + 1), miLength - (index + 1));
					nextDisplay->SetParentDisplay(mpParent, mpNext);
				}

				SetStringPosition(miIndex, index);

				*firstHalf = this;
				*secondHalf = nextDisplay;
			}
			else
			{
				// space or tab at the beginning of the string
				*firstHalf = 0;
				*secondHalf = this;
				SetStringPosition(miIndex+1, miLength-1);
			}
		}
		else
		{
			// space or tab at the end of the string
			SetStringPosition(miIndex, index);
			*firstHalf = this;
			*secondHalf = 0;
		}

		return (WEBC_TRUE);
	}

	if ((index == miLength-1) || (s[index] == '\0'))
	{
		*firstHalf = this;
		*secondHalf = 0;
		return (WEBC_TRUE);
	}

	nextDisplay = STRING_ELEMENT->GetStringDisplay(this);
	if (nextDisplay)
	{
		nextDisplay->SetStringPosition(miIndex + index + 1, miLength - (index + 1));
		nextDisplay->SetParentDisplay(mpParent, mpNext);
	}

	SetStringPosition(miIndex, index + 1);

	*firstHalf = this;
	*secondHalf = nextDisplay;

	return (WEBC_TRUE);
}

void HTMLStringDisplay::BreakBefore(void)
{
	WebChar *s = STRING_PTR;
	if ((*s == ' ') || (*s == '\t'))
	{
		SetStringPosition(miIndex+1, miLength-1);
	}
}

void HTMLStringDisplay::BreakAfter(void)
{
	WebChar *s = STRING_PTR;
	s += (miLength - 1);
	if ((*s == ' ') || (*s == '\t'))
	{
		SetStringPosition(miIndex, miLength-1);
	}
}

WEBC_BOOL HTMLStringDisplay::BreakAllowedBefore(void)
{
	WebChar *s = STRING_PTR;
	return ( (*s == ' ') || (*s == '\t') );
}

WEBC_BOOL HTMLStringDisplay::BreakAllowedAfter(void)
{
	WebChar *s = STRING_PTR;
	s += (miLength - 1);
	return (IS_BREAK_CHAR(*s));
}

WEBC_BOOL HTMLStringDisplay::BreakAllowedInside(void)
{
	// WEBC_TRUE if this string contains a breaking char
	return (!(miFirstBreak < 0));
}


/*****************************************************************************/
// class HTMLMinStringDisplay
/*****************************************************************************/

void HTMLMinStringDisplay::SetStringPosition(WEBC_SIZE iIndex, WEBC_SIZE iLength, WebGraphics *gc)
{
	miIndex = iIndex;

	HTMLString *pStringElement = (HTMLString *) mpHtmlElement;
	WebChar *str = pStringElement->GetString() + miIndex;

	miLength = FindMinLength(str, (GetWrap() == TU_WRAP_NORMAL));

	// Set Width/Height for this section of the string
	gc = gc? gc : GetGraphics();

	if (gc)
	{
		WebFont font = GetFont();
		if (font)
		{
			long len = miLength;
			STRING_CLIP_VISIBLE(str,len);

			DISPLAY_INT w = (len > 0)? gc->TextWidthLen(str, font, len) : 0;
			if (w > 0)
			{
				w += STRING_PADDING * 2;
			}

			mRect.SizeTo(w, WEB_FONT_HEIGHT(font) + (STRING_PADDING*2));
		}
	}
	else
	{
		mRect.SizeTo(0,0);
	}
}

WEBC_BOOL HTMLMinStringDisplay::BreakAllowedBefore(void)
{
	WebChar *s = STRING_PTR;
	return ( (s[0] == ' ') || (s[0] == '\t') || ((miIndex > 0) && IS_BREAK_CHAR(s[-1])) );
}


/*****************************************************************************/
// class HTMLMaxStringDisplay
/*****************************************************************************/

void HTMLMaxStringDisplay::SetStringPosition(WEBC_SIZE iIndex, WEBC_SIZE iLength, WebGraphics *gc)
{
	miIndex = iIndex;

	HTMLString *pStringElement = (HTMLString *) mpHtmlElement;
	WebChar *str = pStringElement->GetString() + miIndex;

	miLength = FindMaxLength(str);

	// Set Width/Height for this section of the string
	gc = gc? gc : GetGraphics();

	if (gc)
	{
		WebFont font = GetFont();
		if (font)
		{
			long len = miLength;
			STRING_CLIP_VISIBLE(str,len);

			DISPLAY_INT w = (len > 0)? gc->TextWidthLen(str, font, len) : 0;
			if (w > 0)
			{
				w += STRING_PADDING * 2;
			}

			mRect.SizeTo(w, WEB_FONT_HEIGHT(font) + (STRING_PADDING*2));
		}
	}
	else
	{
		mRect.SizeTo((STRING_PADDING*2),(STRING_PADDING*2));
	}
}
