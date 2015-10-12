//
// HFRAME.CPP - Methods for HTML DOM class HTMLFrame
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

#include "webcfg.h"
#if (WEBC_SUPPORT_FRAMES)
#include "htmldoc.hpp"
#include "hframe.hpp"
#include "webload.hpp"
#include "htmlfind.hpp"
#include "hframset.hpp"
#include "webcstr.h"
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
// HTMLFrame Constructors
/*****************************************************************************/

HTMLFrame::HTMLFrame(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElement(td, pDocument)
{
	mpFrameDisplay = 0;
	mpName = 0;
	mpSrc = 0;
	mpMode = 0;
	miFrameMarginWidth = 0;
	miFrameMarginHeight = 0;
	mCharset = WEBC_CHARSET_UNKNOWN;
	mpLongDesc = 0;

	WEBC_NEW_VERBOSE(mpFrameDocument,HTMLDocument(),"HTMLDocument");
	if (mpFrameDocument)
	{
		// Initialize frame/document relationships
		mpFrameDocument->SetBrowser(pDocument->GetBrowser());
		mpFrameDocument->SetFrame(this);
	}
}


/*****************************************************************************/
// HTMLFrame Destructor
/*****************************************************************************/

HTMLFrame::~HTMLFrame(void)
{
	if (mpFrameDisplay)
	{
		if (mpFrameDocument)
		{
			mpFrameDocument->SetDisplayManager(0);
		}
		WEBC_DELETE(mpFrameDisplay);
	}
	if (mpFrameDocument)
	{
		WEBC_DELETE(mpFrameDocument);
	}
	webc_free_string_copy(mpMode, __FILE__, __LINE__);
	webc_free_string_copy(mpName, __FILE__, __LINE__);
	webc_free_string_copy(mpSrc, __FILE__, __LINE__);
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

void HTMLFrame::Update(WEBC_PFBYTE o, WEBC_PFBYTE data)
{
	if (mFlags & HFRAME_FLAG_SRC_DIRTY)
	{
		ClearFlag(HFRAME_FLAG_SRC_DIRTY);
		if (Src())
		{
			if (mpFrameDocument)
			{
				mpFrameDocument->SetSrc(Src());
				mpFrameDocument->SetCharset(mCharset, HTML_CHARSET_SRC_REFERER);

				mpFrameDocument->Update(0,(WEBC_CHAR *) data);
			}
		}
	}

	HTMLElement::Update(o,data);
}

CSSValueType HTMLFrame::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_BORDER_BOTTOM_STYLE: // intentional fall-through
		case CSS_PROPERTY_BORDER_LEFT_STYLE:   // intentional fall-through
		case CSS_PROPERTY_BORDER_RIGHT_STYLE:  // intentional fall-through
		case CSS_PROPERTY_BORDER_TOP_STYLE:
		{
			WEBC_BOOL hasBorder = WEBC_FALSE;
			HTMLFrameSet *pFrameSet = GetFrameSet();

			if (mFlags & HFRAME_FLAG_BORDER_DEFINED)
			{
				hasBorder = Frameborder();
			}
			else
			{
				hasBorder = (pFrameSet)? pFrameSet->Frameborder() : WEBC_FALSE;
			}

			if (hasBorder)
			{
				// tbd - if num colors is less than 4 or 3D widgets is disabled,
				//  style should be CSS_BORDER_STYLE_SOLID
				pValue->borderStyle = CSS_BORDER_STYLE_INSET;
			}
			else
			{
				pValue->borderStyle = CSS_BORDER_STYLE_NONE;
			}
			return (CSS_VALUE_SPECIFIED);
		}

		case CSS_PROPERTY_BORDER_BOTTOM_COLOR: // intentional fall-through
		case CSS_PROPERTY_BORDER_LEFT_COLOR:   // intentional fall-through
		case CSS_PROPERTY_BORDER_RIGHT_COLOR:  // intentional fall-through
		case CSS_PROPERTY_BORDER_TOP_COLOR:
			HTML_ColorSet(pValue->color, 0x80, 0x80, 0x80, 0);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_BOTTOM_WIDTH: // intentional fall-through
		case CSS_PROPERTY_BORDER_LEFT_WIDTH:   // intentional fall-through
		case CSS_PROPERTY_BORDER_RIGHT_WIDTH:  // intentional fall-through
		case CSS_PROPERTY_BORDER_TOP_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = Border();
			pValue->borderThickness.val.length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}

HTMLElement *HTMLFrame::FindElement(HTMLElementFinder *pFinder, int iSearchFrames, WEBC_BOOL bIncludeSelf)
{
	if(bIncludeSelf)
	{
		if (pFinder->Check(this))
			return this;
	}

	if (iSearchFrames)
	{
		return ((mpFrameDocument)? mpFrameDocument->FindElement(pFinder, iSearchFrames) : 0);
	}

	return (0);
}


/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLFrame::Reset(void)
{
}
*/

int HTMLFrame::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_CHARSET:
			mCharset = webc_charset_lookup(value);
			return(1);

		case HTML_ATTRIB_NAME:
			SetName(value);
			return (1);

		case HTML_ATTRIB_SRC:
			SetSrc(value);
			return (1);

		case HTML_ATTRIB_SCROLLING:
			SetScrollMode(value);
			return (1);

		case HTML_ATTRIB_MARGINHEIGHT:
			SetFrameMarginHeight(value);
			return (1);

		case HTML_ATTRIB_MARGINWIDTH:
			SetFrameMarginWidth(value);
			return (1);

		case HTML_ATTRIB_FRAMEBORDER:
			if (value && (!webc_c_stricmp(value, "yes") || !webc_c_strcmp(value, "1")))
			{
				SetFrameborder(WEBC_TRUE);
			}
			else if (!webc_c_stricmp(value, "no") || !webc_c_strcmp(value, "0"))
			{
				SetFrameborder(WEBC_FALSE);
			}
			else
			{
				ClearFlag(HFRAME_FLAG_BORDER_DEFINED);
			}
			return (1);

		case HTML_ATTRIB_QUARANTINE:
			SetFlag(HFRAME_FLAG_QUARANTINED);
			return (1);

		case HTML_ATTRIB_LONGDESC:
			SetLongDesc(value);
			return (1);

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLElement::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}


void HTMLFrame::SetName(const WEBC_CHAR *pName)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pName, __FILE__, __LINE__);
	webc_free_string_copy(mpName, __FILE__, __LINE__);
	mpName = pStr;
}


WEBC_CHAR *HTMLFrame::Name(void)
{
	return (mpName);
}


void HTMLFrame::SetSrc(const WEBC_CHAR *pSrc)
{
	// Set to load bitmap when we update
	if (!pSrc || !mpSrc || (webc_stricmp(pSrc, mpSrc)))
	{
		SetFlag(HFRAME_FLAG_SRC_DIRTY);
	}
#if (WEBC_INCLUDE_SEPT2010_BUGFIXES)
	// Previous test fails to refresh always so set dirty unconditionally
	SetFlag(HFRAME_FLAG_SRC_DIRTY);
#endif
	WEBC_CHAR *pStr = webc_malloc_string_copy(pSrc, __FILE__, __LINE__);
	webc_free_string_copy(mpSrc, __FILE__, __LINE__);
	mpSrc = pStr;
}


WEBC_CHAR *HTMLFrame::Src()
{
	return mpSrc;
}


void HTMLFrame::SetScrollMode(WEBC_CHAR *pMode)
{
	WEBC_CHAR* str = webc_malloc_string_copy(pMode, __FILE__, __LINE__);
	webc_free_string_copy(mpMode, __FILE__, __LINE__);
	mpMode = str;

	NotifyDisplay(NOTIFY_STYLE_CHANGE);
}

WEBC_CHAR *HTMLFrame::ScrollMode()
{
	return mpMode;
}


void HTMLFrame::SetFrameborder(WEBC_BOOL yes)
{
	SetFlag(HFRAME_FLAG_BORDER_DEFINED);

	if (yes)
	{
		SetFlag(HFRAME_FLAG_BORDER);
	}
	else
	{
		ClearFlag(HFRAME_FLAG_BORDER);
	}

	NotifyDisplay(NOTIFY_STYLE_CHANGE);
}

WEBC_BOOL HTMLFrame::Frameborder()
{
	return ((mFlags & HFRAME_FLAG_BORDER) != 0);
}

void HTMLFrame::SetBorder(WEBC_UINT8 b)
{
	if (b)
	{
		SetFlag(HFRAME_FLAG_BORDER);
	}
	else
	{
		ClearFlag(HFRAME_FLAG_BORDER);
	}

	NotifyDisplay(NOTIFY_STYLE_CHANGE);
}


WEBC_UINT8 HTMLFrame::Border()
{
	WEBC_BOOL hasBorder = WEBC_FALSE;
	HTMLFrameSet *pFrameSet = GetFrameSet();

	if (mFlags & HFRAME_FLAG_BORDER_DEFINED)
	{
		hasBorder = Frameborder();
	}
	else
	{
		hasBorder = (pFrameSet)? pFrameSet->Frameborder() : WEBC_FALSE;
	}

	return (hasBorder)? 1 : 0;
}

void HTMLFrame::SetLongDesc(const WEBC_CHAR *pLongDesc)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(pLongDesc, __FILE__, __LINE__);
	webc_free_string_copy(mpLongDesc, __FILE__, __LINE__);
	mpLongDesc = pStr;
}

WEBC_CHAR *HTMLFrame::LongDesc()
{
	return (mpLongDesc);
}

/*****************************************************************************/
// HTMLFrame-Specific Methods
/*****************************************************************************/

HTMLDocument *HTMLFrame::FrameDocument(void)
{
	return (mpFrameDocument);
}

HTMLFrameSet *HTMLFrame::GetFrameSet(void)
{
HTMLElement *parent = mpParent;

	while (parent)
	{
		if (parent->Type() == HTML_FRAMESET_ELEMENT)
		{
			return (HTMLFrameSet *) parent;
		}
		parent = parent->mpParent;
	}

	return (0);
}


void HTMLFrame::SetFrameMarginWidth(WEBC_CHAR *pValue)
{
	if (pValue)
	{
		miFrameMarginWidth = webc_atoi(pValue);
	}
}


void HTMLFrame::SetFrameMarginHeight(WEBC_CHAR *pValue)
{
	if (pValue)
	{
		miFrameMarginHeight = webc_atoi(pValue);
	}
}


DISPLAY_INT HTMLFrame::GetFrameMarginWidth()
{
	return (miFrameMarginWidth);
}


DISPLAY_INT HTMLFrame::GetFrameMarginHeight()
{
	return (miFrameMarginHeight);
}


void HTMLFrame::SetNoResize(int iOn)
{
	// not supported
}


void HTMLFrame::SetBorderColor(HTMLColor color)
{
	// not supported
}


DisplayElement *HTMLFrame::GetDisplayElement (WEBC_BOOL create)
{
	return (GetFrameDisplay(create));
}

TextUnit *HTMLFrame::GetTextUnit (void)
{
	return (GetFrameDisplay());
}

HTMLFrameDisplay *HTMLFrame::GetFrameDisplay (WEBC_BOOL create)
{
	if (!mpFrameDisplay && create)
	{
		WEBC_NEW_VERBOSE(mpFrameDisplay, HTMLFrameDisplay(this),"HTMLFrameDisplay");
		if (mpFrameDocument)
		{
			mpFrameDocument->SetDisplayManager(mpFrameDisplay);
		}
	}

	return (mpFrameDisplay);
}

#endif // WEBC_SUPPORT_FRAMES
