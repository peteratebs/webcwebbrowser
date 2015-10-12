//
// HFLOW.CPP - Methods for HTML DOM class HTMLFlow
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
#include "hflow.hpp"
#include "webc.h"
#include "util.h"
#include "tuenum.hpp"
#include "webload.hpp"
#include "webfonts.hpp"
#include "htmlbrow.hpp"
#include "dflow.hpp"
#include "webcstr.h"
#include "webcmem.h"
#include "textstyle.hpp"


/*****************************************************************************/
// Local Constants
/*****************************************************************************/
// const long HTML_ZINDEX_MAX = 0x7fffffffL;


/*****************************************************************************/
// Local type definitions
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// HTMLFlow Constructors
/*****************************************************************************/

HTMLFlow::HTMLFlow(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElementContainer(td, pDocument)
{
  #if(WEBC_SUPPORT_BACKGROUND_IMAGES)
	mpBackgroundImage = 0;
	mpBackground = 0;
  #endif

	mWidth.type = CSS_LENGTH_UNIT_AUTO;
	mWidth.val.dec = 0;
	mWidth.val.frac = 0;

	mTextStyle = 0;
	mpFlowDisplay = 0;
}

HTMLFlow::HTMLFlow(HTMLFlow &copy)
	:HTMLElementContainer(copy)
{
  #if(WEBC_SUPPORT_BACKGROUND_IMAGES)
	mpBackgroundImage = 0;
	mpBackground = 0;
  #endif

	mWidth = copy.mWidth;

	mTextStyle = 0;
	mpFlowDisplay = 0;
}

/*****************************************************************************/
// HTMLFlow Destructor
/*****************************************************************************/

HTMLFlow::~HTMLFlow(void)
{
  #if(WEBC_SUPPORT_BACKGROUND_IMAGES)
	SetBackgroundImage(0);
	SetBackground(0);
  #endif

	if (mpFlowDisplay)
	{
		WEBC_DELETE(mpFlowDisplay);
	}

	if (mTextStyle)
	{
		mTextStyle->release();
	}
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/

WEBC_UINT8 HTMLFlow::ResolveFlowUnitType (void)
{
	CSSPropertyValue value;

	if ((GetStyleFromCSS(CSS_PROPERTY_DISPLAY, &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(CSS_PROPERTY_DISPLAY, &value) == CSS_VALUE_SPECIFIED))
	{
		switch (value.display)
		{
			case CSS_DISPLAY_INLINE_BLOCK:
			{
// Forcing this to TU_TYPE_INLINE did not help
// Forcing this to TU_TYPE_BLOCK did not help
				return (TU_TYPE_INLINE_BLOCK);
			}
			case CSS_DISPLAY_INLINE:
				if ((GetStyleFromCSS(CSS_PROPERTY_WIDTH, &value) == CSS_VALUE_SPECIFIED) ||
	    			(GetStyleFromHTML(CSS_PROPERTY_WIDTH, &value) == CSS_VALUE_SPECIFIED))
    			{
    				if (value.length.type != CSS_LENGTH_UNIT_AUTO &&
    				    value.length.type != CSS_LENGTH_UNIT_PERCENT)
    				{
    					return TU_TYPE_DEFAULT;
    				}
    			}
				return (TU_TYPE_INLINE);

//			case CSS_DISPLAY_NONE:
//				return (TU_TYPE_NODISPLAY);

			default:
				break;
		}
	}

	return (TU_TYPE_BLOCK);
}

WEBC_UINT16 HTMLFlow::GetUnitType(void)
{
	if (!(mFlags & HELEM_FLAG_UNIT_TYPE_CACHED))
	{
		mTextUnitType = ResolveFlowUnitType();
		//SetFlag(HELEM_FLAG_UNIT_TYPE_CACHED);
	}

	if ((GetFloat() != TU_FLOAT_NONE) && (mTextUnitType != TU_TYPE_NODISPLAY))
	{
		return (TU_TYPE_DEFAULT);
	}

	return (mTextUnitType);
}

WEBC_UINT16 HTMLFlow::GetBreak(void)
{
	if (!(mFlags & HELEM_FLAG_UNIT_TYPE_CACHED))
	{
		mTextUnitType = ResolveFlowUnitType();
		//SetFlag(HELEM_FLAG_UNIT_TYPE_CACHED);
	}
	return ((mTextUnitType == TU_TYPE_BLOCK)? TU_BREAK_BOTH : TU_BREAK_NONE);
}


/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/


/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

/*
void HTMLFlow::Refresh(PegThing *pThing)
{
}
*/

HTMLEventStatus HTMLFlow::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (HTML_EVENT_STATUS_CONTINUE);
}

/*
HTMLElement *HTMLFlow::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLFlow::Reset(void)
{
}
*/

int HTMLFlow::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

  #if (WEBC_SUPPORT_BACKGROUND_IMAGES)
	switch (Property)
	{
		case HTML_ATTRIB_BACKGROUND:
			SetBackground(value);
			return (1);

		default:
			break;
	}
  #endif // WEBC_SUPPORT_BACKGROUND_IMAGES

	if (call_base)
	{
		return (HTMLElement::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}


/******************************************************************************
* void HTMLFlow::SetBackground(const WEBC_CHAR *pUrl) - Set background image URL
*
* Parameters
*    pUrl - NULL-terminated string: URL of background image
*
* Description
*    Only sets the background URL string: to load the actual image,
*    call Update().
*
* Returns (void)
*    Nothing
******************************************************************************/
#if (WEBC_SUPPORT_BACKGROUND_IMAGES)
void HTMLFlow::SetBackground(const WEBC_CHAR *pUrl)
{
	if (!mpBackground || !pUrl || webc_stricmp(pUrl, mpBackground))
	{
		SetFlag(HFLOW_FLAG_BGIMGUPDATED);
	}

	WEBC_CHAR *pStr = webc_malloc_string_copy(pUrl, __FILE__, __LINE__);
	webc_free_string_copy(mpBackground, __FILE__, __LINE__);
 	mpBackground = pStr;
}
#endif


/******************************************************************************
* void HTMLFlow::GetChildWrap() -
*
* Parameters
*
* Description
*
* Returns ()
******************************************************************************/
WEBC_UINT16 HTMLFlow::GetChildWrap(void)
{
	if (!(mFlags & HFLOW_FLAG_CHILD_WRAP_CACHED))
	{
		HFLOW_SET_CHILD_WRAP(HTMLElement::GetChildWrap());
		SetFlag(HFLOW_FLAG_CHILD_WRAP_CACHED);
	}

	return (HFLOW_GET_CHILD_WRAP);
}


/******************************************************************************
* void HTMLFlow::GetColor() - get color for text within this
*    element.
*
* Parameters
*    none
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/

HTMLColor HTMLFlow::GetColor (void)
{
	if (!(mFlags & HFLOW_FLAG_COLOR_CACHED))
	{
		mColor = HTMLElement::GetColor();
		SetFlag(HFLOW_FLAG_COLOR_CACHED);
	}

	return (mColor);
}


/******************************************************************************
* CSSTextDecoration HTMLFlow::GetTextDecoration() -
*
* Parameters
*    none
*
* Description
*
* Returns (CSSTextDecoration)
*
******************************************************************************/
CSSTextDecoration HTMLFlow::GetTextDecoration (void)
{
	if (!(mFlags & HFLOW_FLAG_TEXT_DECORATION_CACHED))
	{
		HFLOW_SET_TEXT_DECORATION(HTMLElement::GetTextDecoration());
		SetFlag(HFLOW_FLAG_TEXT_DECORATION_CACHED);
	}

	return (HFLOW_GET_TEXT_DECORATION);
}


/*****************************************************************************/
// HTMLFlow-Specific Methods
/*****************************************************************************/

#if (WEBC_SUPPORT_BACKGROUND_IMAGES)

/******************************************************************************
* void HTMLFlow::SetBackgroundImage(WebcImage *pBitmap) - Set the background
*    bitmap for this element.
*
* Parameters
*    pBitmap - Pointer to bitmap to use (see webcbmp.hpp)
*
* Description
*    This method will not display the new bitmap; to redraw, call Refresh().
*
* Returns (void)
*    Nothing.
******************************************************************************/

void HTMLFlow::SetBackgroundImage(WebcImage *pBitmap)
{
	if (pBitmap)
	{
		pBitmap->Claim();
	}

	if (mpBackgroundImage)
	{
		mpBackgroundImage->Release();
	}

	mpBackgroundImage = pBitmap;

	if (mpFlowDisplay)
	{
		NotifyEvent what(NOTIFIER_TYPE_HTML_ELEMENT, NOTIFY_DISPLAY_STYLE_CHANGE);
		mpFlowDisplay->Notify(this, &what);
	}
}

WebcImage * HTMLFlow::GetBackgroundImage(void)
{
	return mpBackgroundImage;
}

#endif // WEBC_SUPPORT_BACKGROUND_IMAGES


/*****************************************************************************/
// Local Function Bodies
/*****************************************************************************/

/******************************************************************************
* void HTMLFlow::SetHtmlAlign(WEBC_UINT8 wAlign) -
*
* Parameters
*
* Description -
*
* Returns (void)
*
******************************************************************************/
void HTMLFlow::SetHtmlAlign(WEBC_UINT8 bAlign)
{
}

/******************************************************************************
* WEBC_UINT8 HTMLFlow::GetChildTextAlign() -
*
* Parameters
*    none
*
* Description - This method gets the horizontal text alignment type.
*
*
* Returns (WEBC_UINT8)
*	the horizontal text alignment
******************************************************************************/
WEBC_UINT8 HTMLFlow::GetChildTextAlign(void)
{
	if (!(mFlags & HFLOW_FLAG_CHILD_TEXTALIGN_CACHED))
	{
		HFLOW_SET_CHILD_TEXTALIGN(HTMLElement::GetChildTextAlign());
		SetFlag(HFLOW_FLAG_CHILD_TEXTALIGN_CACHED);
	}

	return (HFLOW_GET_CHILD_TEXTALIGN);
}

/******************************************************************************
* WEBC_UINT8 HTMLFlow::GetChildVAlign() -
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
WEBC_UINT8 HTMLFlow::GetChildVAlign(void)
{
	return (TU_VALIGN_TOP);
}


/******************************************************************************
* HTMLFlow::GetWidth() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
void HTMLFlow::GetWidth (CSSLength *width)
{
	*width = mWidth;
}

/******************************************************************************
* HTMLFlow::GetBackgroundRepeat() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
#if (WEBC_SUPPORT_BACKGROUND_IMAGES)
CSSBackgroundRepeat HTMLFlow::GetBackgroundRepeat (void)
{
CSSPropertyValue value;

	if (GetStyleFromCSS(CSS_PROPERTY_BACKGROUND_REPEAT, &value) == CSS_VALUE_SPECIFIED)
	{
		return (value.backgroundRepeat);
	}

	return (CSS_BACKGROUND_REPEAT_REPEAT);
}

/******************************************************************************
* HTMLFlow::GetBackgroundAttachment() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
CSSBackgroundAttachment HTMLFlow::GetBackgroundAttachment (void)
{
CSSPropertyValue value;

	if (GetStyleFromCSS(CSS_PROPERTY_BACKGROUND_ATTACHMENT, &value) == CSS_VALUE_SPECIFIED)
	{
		return (value.backgroundAttachment);
	}

	return (CSS_BACKGROUND_ATTACHMENT_SCROLL);
}


/******************************************************************************
* HTMLFlow::GetBackgroundXPos() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
void HTMLFlow::GetBackgroundXPos (CSSLength *pos)
{
CSSPropertyValue value;

	if (GetStyleFromCSS(CSS_PROPERTY_BACKGROUND_X_POSITION, &value) == CSS_VALUE_SPECIFIED)
	{
		*pos = value.length;
		return;
	}

	pos->type = CSS_LENGTH_UNIT_PX;
	pos->val.dec = 0;
	pos->val.frac = 0;
}

/******************************************************************************
* HTMLFlow::GetBackgroundYPos() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
void HTMLFlow::GetBackgroundYPos (CSSLength *pos)
{
CSSPropertyValue value;

	if (GetStyleFromCSS(CSS_PROPERTY_BACKGROUND_Y_POSITION, &value) == CSS_VALUE_SPECIFIED)
	{
		*pos = value.length;
		return;
	}

	pos->type = CSS_LENGTH_UNIT_PX;
	pos->val.dec = 0;
	pos->val.frac = 0;
}
#endif // WEBC_SUPPORT_BACKGROUND_IMAGES

DisplayElement *HTMLFlow::GetDisplayElement (WEBC_BOOL create)
{
	return (GetFlowDisplay(create));
}

TextUnit *HTMLFlow::GetTextUnit (void)
{
	WEBC_UINT16 unitType = GetUnitType();
// Exclude TU_TYPE_INLINE_BLOCK from get text unit had no effect
	if (unitType == TU_TYPE_INLINE || unitType == TU_TYPE_INLINE_BLOCK || unitType == TU_TYPE_NODISPLAY)
	{
		return (this);
	}

	// WEBC_TRUE == create display element if necessary
	return (GetFlowDisplay(WEBC_TRUE));
}

HTMLFlowDisplay *HTMLFlow::GetFlowDisplay (WEBC_BOOL create)
{
	if (!mpFlowDisplay && create)
	{
		WEBC_NEW_VERBOSE(mpFlowDisplay, HTMLFlowDisplay(this),"HTMLFlowDisplay");
	}
	return (mpFlowDisplay);
}

ITextDisplayStyle* HTMLFlow::GetTextDisplayStyle (WEBC_BOOL create)
{
	if (!mTextStyle && create)
	{
		HTMLDocumentTextDisplayStyleFactory factory(GetDocument());
		static HTMLColor transparent = HTML_RGBAToColor(0, 0, 0, 0xff);

		mTextStyle = factory.newTextStyle (
				this->GetFont(),
				this->GetColor(),
				// we only need the background color here if this is an inline flow,
				//  since in any other case the flow display will draw the background
				//  behind all text
				(GetUnitType() == TU_TYPE_INLINE)? this->GetBackgroundColor() : transparent,
				this->GetTextDecoration(),
				this->GetOutlineStyle()
			);
	}

	if (mTextStyle)
	{
		mTextStyle->claim();
	}

	return (mTextStyle);
}


/******************************************************************************
* void HTMLFlow::LocalStyleUpdate()
*
* Parameters
*
* Description
*    Update cached properties that can ONLY be defined locally (i.e. which are
*     NEVER inherited from a parent).
*
* Returns (void)
*
******************************************************************************/
void HTMLFlow::LocalStyleUpdate (void)
{
	WEBC_BOOL callAgain = WEBC_FALSE;
	CSSPropertyValue value;

	HTMLElement::LocalStyleUpdate();

	ClearFlag(HFLOW_FLAG_BGCOLOR_CACHED);

  #if (WEBC_SUPPORT_STYLE_SHEETS)
	if ((GetStyleFromCSS(CSS_PROPERTY_WIDTH, &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(CSS_PROPERTY_WIDTH, &value) == CSS_VALUE_SPECIFIED))
	{
		mWidth = value.length;
	}
	else
	{
		mWidth.type = CSS_LENGTH_UNIT_AUTO;
	}
	callAgain = WEBC_TRUE;
  #endif //(WEBC_SUPPORT_STYLE_SHEETS)

	if (GetUnitType() != TU_TYPE_INLINE)
	{
	  #if (WEBC_SUPPORT_BACKGROUND_IMAGES)
		if (GetStyleFromCSS(CSS_PROPERTY_BACKGROUND_IMAGE, &value) == CSS_VALUE_SPECIFIED)
		{
			SetBackground(value.url);
		}

		// if the src dirty bit is set, then reload our bitmap
		if (mFlags & HFLOW_FLAG_BGIMGUPDATED)
		{
			ClearFlag(HFLOW_FLAG_BGIMGUPDATED);
			if (Background())
			{
				HTMLDocument *pDoc = GetDocument();
				if (pDoc)
				{
					SetFlag(HELEM_FLAG_LOADING_IMAGE);
					pDoc->LoadImage(Background(), this);
				}
			}
			else
			{
				SetBackgroundImage(WEBC_NULL);
			}
		}
	  #endif // WEBC_SUPPORT_BACKGROUND_IMAGES

	} // !TU_TYPE_INLINE

	if (callAgain)
	{
		HTMLElement::LocalStyleUpdate();
	}
}


/******************************************************************************
* void HTMLFlow::InheritedStyleUpdate()
*
* Parameters
*
* Description
*    Update cached properties that can be defined locally or inherited from a
*     parent.
*
* Returns (void)
*
******************************************************************************/
void HTMLFlow::InheritedStyleUpdate (void)
{
	ClearFlag(HFLOW_FLAG_CHILD_WRAP_CACHED | HFLOW_FLAG_CHILD_TEXTALIGN_CACHED |
	          HFLOW_FLAG_COLOR_CACHED | HFLOW_FLAG_TEXT_DECORATION_CACHED);

	if (mTextStyle)
	{
		HTMLDocumentTextDisplayStyleFactory factory(GetDocument());

		mTextStyle->release();
		mTextStyle = factory.newTextStyle (
				this->GetFont(),
				this->GetColor(),
				this->GetBackgroundColor(),
				this->GetTextDecoration(),
				this->GetOutlineStyle()
			);
	}

	HTMLElement::InheritedStyleUpdate();
}

/******************************************************************************
* HTMLFlow::GetOutlineStyle() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
CSSBorderStyle HTMLFlow::GetOutlineStyle(void)
{
	CSSPropertyValue value;
	CSSValueType result = GetStyleFromCSS(CSS_PROPERTY_OUTLINE_STYLE, &value);

	if (result == CSS_VALUE_UNKNOWN)
	{
		result = GetStyleFromHTML(CSS_PROPERTY_OUTLINE_STYLE, &value);
	}

	switch (result)
	{
		case CSS_VALUE_SPECIFIED:
			return (value.borderStyle);

		case CSS_VALUE_INHERIT:
			return ((mpParent)? mpParent->GetOutlineStyle() : CSS_BORDER_STYLE_NONE);

		default:
			if (GetUnitType() == TU_TYPE_INLINE)
			{
				return ((mpParent)? mpParent->GetOutlineStyle() : CSS_BORDER_STYLE_NONE);
			}
			break;
	}

	return (CSS_BORDER_STYLE_NONE);
}

/******************************************************************************
* HTMLFlow::GetBackgroundColor() - Gets the background color
*
* Parameters
*    color - pointer to struct to fill with color info
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/

HTMLColor HTMLFlow::GetBackgroundColor(void)
{
	if (!(mFlags & HFLOW_FLAG_BGCOLOR_CACHED))
	{
		mBackgroundColor = HTMLElement::GetBackgroundColor();
		SetFlag(HFLOW_FLAG_BGCOLOR_CACHED);
	}

	return (mBackgroundColor);
}


/******************************************************************************
* HTMLFlow::Notify() -
*
* Parameters
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/

void HTMLFlow::Notify (Notifier *who, NotifyEvent *what)
{
  if (what->event > NOTIFY_RENDER_FIRST_STATE && what->event < NOTIFY_RENDER_LAST_STATE)
  {
  // HEREHERE
	HTMLEvent e;
	e.type = HTML_EVENT_RENDER;
	e.data.NotifyMessage.Notifier = what->type;
	e.data.NotifyMessage.NotifyEvent  = what->event;
	e.elem = (HELEMENT_HANDLE) (HTMLElement *)this;
	(HTMLElement *)this->Event(&e);
  }
  #if (WEBC_SUPPORT_BACKGROUND_IMAGES)
    WEBC_DEBUG_ReceiveNotify(this, who, what);
	switch (what->type)
	{
		case NOTIFIER_TYPE_IMAGE:
			switch (what->event)
			{
				case NOTIFY_DONE:
					ClearFlag(HELEM_FLAG_LOADING_IMAGE);
					SetBackgroundImage((WebcImage *) who);
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}
  #endif // WEBC_SUPPORT_BACKGROUND_IMAGES
}
