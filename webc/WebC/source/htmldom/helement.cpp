//
// HELEMENT.CPP - Methods for HTML DOM class HTMLElement
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

#include "webcjhconfig.h"
#include "htmldoc.hpp"
#include "helement.hpp"
#include "hframe.hpp"
#include "hstring.hpp"
#include "webc.h"
#if (WEBC_SUPPORT_JSCRIPT)
 #include "wjscript.hpp"
 #include "jhutils.h"
 #include "wjsdoc.h"

#endif
#include "wgkeys.hpp"
#include "htmlfind.hpp"
#include "htmlbrow.hpp"
#include "dbgtimer.h"
#include "webcstr.h"
#include "webstr.hpp"
#include "objref.cpp"
#include "hdstream.hpp"
#include "hform.hpp"
#include "webcassert.h"
#include "webcmem.h"
#include "webcLog.h"


WEBC_UINT8 HTMLElement::mbDepth = 0;
HTMLEventQueue *gpEventQueue = 0;

/*****************************************************************************/
// Macros
/*****************************************************************************/


/*****************************************************************************/
// Data
/*****************************************************************************/

WEBC_READONLY WEBC_UINT16 rCssToTextUnitClear[] =
{
	TU_CLEAR_ALL,
	TU_CLEAR_LEFT,
	TU_CLEAR_NONE,
	TU_CLEAR_RIGHT
};


/*****************************************************************************/
// Local Types
/*****************************************************************************/

#if (WEBC_SUPPORT_JSCRIPT)

struct JSHandlerInfo
{
	HTMLEventType       eventType;
	WebcJSEventHandler *handler;
	WEBC_CHAR           *scriptSource; //to save javascript source of event handlers
};

struct s_EventHandlerSpec
{
	HTMLEventType       eventType;
	const WEBC_CHAR *functionText;
};
typedef struct s_EventHandlerSpec EventHandlerSpec;

#endif


/*****************************************************************************/
// HTMLElement Constructors
/*****************************************************************************/

#if (WEBC_SUPPORT_JSCRIPT)
HTMLElement::HTMLElement(HTMLTagParse *td, HTMLDocument *pDocument, JSObject *pJSObj)
#else
HTMLElement::HTMLElement(HTMLTagParse *td, HTMLDocument *pDocument)
#endif // WEBC_SUPPORT_JSCRIPT
{
	WEBC_ASSERT(pDocument);

	mpNext = 0;
	mpPrev = 0;
	mpParent = 0;
	mpId = 0;
	mpTitle = 0;
	mpTextDirection = 0;
	mpLanguage = 0;
	mPrivateData = 0;
	mFlags = 0;
	mDocRef.Set(pDocument);
	mTag = HTML_UNKNOWN_TAG;
	mFontDesc = 0;
	mFont = 0;
	mTextUnitType = TU_TYPE_DEFAULT;
  #if (WEBC_SUPPORT_INNER_HTML)
	mpTagAttributes = 0;
  #endif // WEBC_SUPPORT_INNER_HTML

  #if (WEBC_SUPPORT_STYLE_SHEETS)
	mpIdLowercase = 0;
  #endif

  #if (WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION)
	mpFocusBoxElement = 0;
	miFocusBoxIndex = 0;
  #endif // WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION

  #if (WEBC_SUPPORT_JSCRIPT)
	vector_init(&mJSHandlerVector, sizeof(JSHandlerInfo));
	if (pJSObj)
	{
		SetJSObject(pJSObj);
	}
  #endif // WEBC_SUPPORT_JSCRIPT

	vector_init(&mClassList, sizeof(const WEBC_CHAR *));
	vector_init(&mPrivateDataStack,sizeof(void *));

  #if (WEBC_SUPPORT_USER_EVENTS)
	mpUserParam = 0;
	mpUserEventHandler = 0;
	mpWebCElementEventHandler = 0;
	vector_init(&mHandlerVectorStack, sizeof(void *));

	if (pDocument && pDocument->GetBrowser())
	{
		if (td)
		{
			mTag = td->htmlTag;
		}

		mpUserEventHandler = pDocument->GetBrowser()->GetDefaultElementEventHandler(this);

		if (mpUserEventHandler)
		{
			HTMLEvent e;
			e.type = HTML_EVENT_OBJECT_CREATED;
			mpUserEventHandler->Event(this, &e, mpUserParam);
		}
	}
  #endif // WEBC_SUPPORT_USER_EVENTS
}


HTMLElement::HTMLElement(class HTMLElement &copy)
{
	mpNext = 0;
	mpPrev = 0;
	mpParent = 0;
	mpId = 0;
	mpIdLowercase = 0;
	SetId(copy.mpId);
	mpTitle = 0;
	SetTitle(copy.mpTitle);
	mpTextDirection = 0;
	SetTextDirection(copy.mpTextDirection);
	mpLanguage = 0;
	SetLanguage(copy.mpLanguage);
	mPrivateData = 0;
	mFlags = copy.mFlags;
	mDocRef.Set(copy.GetDocument());
	mTag = copy.mTag;
	mFontDesc = copy.mFontDesc;
	mFont = copy.mFont;
	mTextUnitType = copy.mTextUnitType;

  #if (WEBC_SUPPORT_INNER_HTML)
	mpTagAttributes = 0;
  #endif // WEBC_SUPPORT_INNER_HTML

  #if (WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION)
	mpFocusBoxElement = 0;
	miFocusBoxIndex = 0;
  #endif // WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION

  #if (WEBC_SUPPORT_JSCRIPT)
	vector_init(&mJSHandlerVector, sizeof(JSHandlerInfo));
  #endif // WEBC_SUPPORT_JSCRIPT

	vector_init(&mClassList, sizeof(const WEBC_CHAR *));
	vector_init(&mPrivateDataStack,sizeof(void *));

  #if (WEBC_SUPPORT_USER_EVENTS)
	mpUserParam = 0;
	mpUserEventHandler = 0;
	mpWebCElementEventHandler = 0;
	vector_init(&mHandlerVectorStack, sizeof(void *));

	if (copy.GetDocument() && copy.GetDocument()->GetBrowser())
	{

		mpUserEventHandler = copy.GetDocument()->GetBrowser()->GetDefaultElementEventHandler(this);

		if (mpUserEventHandler)
		{
			HTMLEvent e;
			e.type = HTML_EVENT_OBJECT_CREATED;
			mpUserEventHandler->Event(this, &e, mpUserParam);
		}
	}
  #endif // WEBC_SUPPORT_USER_EVENTS
}


/*****************************************************************************/
// HTMLElement Destructor
/*****************************************************************************/

HTMLElement::~HTMLElement(void)
{
	HTMLDocument* document = mDocRef.Get();

	vector_delete(&mPrivateDataStack);

  #if (WEBC_SUPPORT_USER_EVENTS)
	if (mpUserEventHandler)
	{
		HTMLEvent e;
		e.type = HTML_EVENT_OBJECT_DESTROYED;
		mpUserEventHandler->Event(this, &e, mpUserParam);
	}
	SetEventHandler(0); // Deletes current handler if there is one
	vector_iterator vi;
	HTMLEventHandler **prevUserEventHandler = (HTMLEventHandler **) vector_get_first(&mHandlerVectorStack, &vi);
	while (prevUserEventHandler)
	{
		WEBC_DELETE(*prevUserEventHandler);
		vector_delete_node(&mHandlerVectorStack,0);
		prevUserEventHandler = (HTMLEventHandler **) vector_get_first(&mHandlerVectorStack, &vi);
	}

	webc_free_string_copy(mpUserParam, __FILE__, __LINE__);
    if (mpWebCElementEventHandler)
    {
		WEBC_DELETE(mpWebCElementEventHandler);
		mpWebCElementEventHandler = 0;
    }
	vector_delete(&mHandlerVectorStack);
  #endif

  #if (WEBC_SUPPORT_JSCRIPT)
	ClearJSEventHandlers();
#if (WEBC_INCLUDE_SEPT2010_BUGFIXES)
	// 10/05/2010 Set the javascript ojects Private pointer to null so there is no chance of referencing us.
	JSObject *pobj = GetJSObject();
	if (pobj)
	{
		WebcJSDocumentContext* pMgr = document->GetJSMgr();
		JSContext *pCx = (pMgr)? pMgr->AllocContext() : 0;
		if (pCx)
		{
			jhutil_SetPrivate(pCx, pobj, 0);
			pMgr->FreeContext(pCx);
		}
	}
#endif
	SetJSObject(0);
#endif

  #if (WEBC_SUPPORT_INNER_HTML)
	if (mpTagAttributes)
	{
		WEBC_FREE(mpTagAttributes);
	}
  #endif // WEBC_SUPPORT_INNER_HTML

	if (document)
	{
		HTMLBody *body = document->Body();
		HTMLBodyDisplay *bodyDisplay = (body)? (HTMLBodyDisplay *) body->GetDisplayElement(WEBC_FALSE) : 0;

		if (bodyDisplay)
		{
			bodyDisplay->RemoveHtmlElementReferences(this);
		}

		// if we are still (somehow) listening on an image load, remove ourselves
		//  from the document image listener list now
		if (mFlags & HELEM_FLAG_LOADING_IMAGE)
		{
			document->RemovePendingImageListener(this);
			ClearFlag(HELEM_FLAG_LOADING_IMAGE);
		}
	}

  #if (WEBC_SUPPORT_STYLE_SHEETS)
	if (mFlags & HELEM_FLAG_HAS_INLINE_STYLE)
	{
		if (document && document->GetCSSContext())
		{
			document->GetCSSContext()->RemoveElementStyle(this);
		}
	}
	webc_string_table_remove(mpIdLowercase, __FILE__, __LINE__);
  #endif

	webc_free_string_copy(mpId, __FILE__, __LINE__);
	webc_free_string_copy(mpTitle, __FILE__, __LINE__);
	webc_free_string_copy(mpTextDirection, __FILE__, __LINE__);
	webc_free_string_copy(mpLanguage, __FILE__, __LINE__);
	ResetClass();
	webc_ElementCancelTimers((HBROWSER_HANDLE )document->GetBrowser(), (HELEMENT_HANDLE) this);
	gpEventQueue->Purge(this);
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/

/******************************************************************************
* WEBC_UINT16 HTMLElement::GetUnitType() -
*
* Parameters
*    none
*
* Description - returns TextUnit format/display type (see textunit.hpp)
*
* Returns (WEBC_UINT16)
*	unit type
******************************************************************************/
WEBC_UINT16 HTMLElement::GetUnitType(void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETUNITTYPE);

	if (mFlags & HELEM_FLAG_UNIT_TYPE_CACHED)
	{
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETUNITTYPE);
		return (mTextUnitType);
	}

	mTextUnitType = TU_TYPE_DEFAULT;

	CSSPropertyValue value;
	if ((GetStyleFromCSS(CSS_PROPERTY_DISPLAY, &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(CSS_PROPERTY_DISPLAY, &value) == CSS_VALUE_SPECIFIED))
	{
		switch (value.display)
		{
			case CSS_DISPLAY_NONE:
				mTextUnitType = TU_TYPE_NODISPLAY;
				break;

			default:
				break;
		}
	}

	//SetFlag(HELEM_FLAG_UNIT_TYPE_CACHED);

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETUNITTYPE);

	return (mTextUnitType);
}

/******************************************************************************
* WEBC_UINT16 HTMLElement::GetBreak() -
*
* Parameters
*    none
*
* Description - returns TextUnit line break behavior (see textunit.hpp)
*
* Returns (WEBC_UINT8)
*	line break type
******************************************************************************/
WEBC_UINT16 HTMLElement::GetBreak(void)
{
	return (TU_BREAK_NONE);
}

/******************************************************************************
* WEBC_UINT16 HTMLElement::GetClear() -
*
* Parameters
*    none
*
* Description - Sets Textunit flags. See textunit.hpp for description.
*
* Returns (WEBC_UINT16)
*	textunit flags
******************************************************************************/

WEBC_UINT16 HTMLElement::GetClear(void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETCLEAR);

	CSSPropertyValue value;

	if (GetStyleFromCSS(CSS_PROPERTY_CLEAR, &value) != CSS_VALUE_SPECIFIED)
	{
		if (GetStyleFromHTML(CSS_PROPERTY_CLEAR, &value) != CSS_VALUE_SPECIFIED)
		{
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETCLEAR);
			return (TU_CLEAR_NONE);
		}
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETCLEAR);
	return (rCssToTextUnitClear[value.clear]);
}

/******************************************************************************
* WEBC_UINT16 HTMLElement::GetDir() -
*
* Parameters
*    none
*
* Description - Sets Textunit flags. See textunit.hpp for description.
*
* Returns (WEBC_UINT16)
*	textunit flags
******************************************************************************/
WEBC_UINT16 HTMLElement::GetDir(void)
{
	return (TU_DIR_LTR);
}

/******************************************************************************
* WEBC_UINT16 HTMLElement::GetWrap() -
*
* Parameters
*    none
*
* Description - Sets Textunit flags. See textunit.hpp for description.
*
* Returns (WEBC_UINT16)
*	textunit flags
******************************************************************************/
WEBC_UINT16 HTMLElement::GetWrap(void)
{
	return (mpParent? mpParent->GetChildWrap() : TU_WRAP_NORMAL);
}


/******************************************************************************
* void HTMLElement::GetChildWrap() -
*
* Parameters
*
* Description
*
* Returns ()
******************************************************************************/
WEBC_UINT16 HTMLElement::GetChildWrap(void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETCHILDWRAP);

	CSSPropertyValue value;
	WEBC_UINT16 wrap;

	if ((GetStyleFromCSS(CSS_PROPERTY_WHITE_SPACE, &value) == CSS_VALUE_SPECIFIED) ||
		(GetStyleFromHTML(CSS_PROPERTY_WHITE_SPACE, &value) == CSS_VALUE_SPECIFIED))
	{
		wrap = (value.whitespace == CSS_WHITE_SPACE_NORMAL)? TU_WRAP_NORMAL : TU_WRAP_NONE;
	}
	else
	{
		wrap = (mpParent)? mpParent->GetChildWrap() : TU_WRAP_NORMAL;
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETCHILDWRAP);

	return (wrap);
}


/******************************************************************************
* WEBC_UINT16 HTMLElement::GetVisibility() -
*
* Parameters
*    none
*
* Description - Sets Textunit flags. See textunit.hpp for description.
*
* Returns (WEBC_UINT16)
*	textunit flags
******************************************************************************/
WEBC_UINT16 HTMLElement::GetVisibility(void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETVISIBILITY);

	if (!(mFlags & HELEM_FLAG_VISIBILITY_CACHED))
	{
		WEBC_UINT16 visibility = FindVisibility();

		// Disable this optimization because it doesn't propigate through the element's children
		// SetFlag(HELEM_FLAG_VISIBILITY_CACHED);

		if (visibility == TU_VISIBILITY_HIDE)
		{
			SetFlag(HELEM_FLAG_HIDDEN);
		}
		else
		{
			ClearFlag(HELEM_FLAG_HIDDEN);
		}

		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETVISIBILITY);
		return (visibility);
	}

	if (mFlags & HELEM_FLAG_HIDDEN)
	{
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETVISIBILITY);
		return (TU_VISIBILITY_HIDE);
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETVISIBILITY);
	return (TU_VISIBILITY_SHOW);
}

WEBC_UINT16 HTMLElement::FindVisibility(void)
{
	CSSPropertyValue value;

	// Check if the element is hidden or is style display==none
	// If so return visibility none. Without this change the
	// display==none does not work.
		if (!GetVisibleFromDisplay())
			return (TU_VISIBILITY_HIDE);

	if ((GetStyleFromCSS(CSS_PROPERTY_VISIBILITY, &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(CSS_PROPERTY_VISIBILITY, &value) == CSS_VALUE_SPECIFIED))
	{
		switch (value.visibility)
		{
			case CSS_VISIBILITY_VISIBLE:
			case CSS_VISIBILITY_SHOW:
				return (TU_VISIBILITY_SHOW);

			case CSS_VISIBILITY_HIDDEN:
			case CSS_VISIBILITY_HIDE:
				return (TU_VISIBILITY_HIDE);

			default:
				break;
		}
	}

	WEBC_UINT16 visible = mpParent? mpParent->GetVisibility() : TU_VISIBILITY_INHERIT;

	return (visible);
}


/******************************************************************************
* DISPLAY_INT HTMLElement::GetMinWidth() -
*
* Parameters
*    none
*
* Description - This method gets the minimum width that this element can be.
*				This method will return 0 unless this method or GetWidth is
*               overridden
*
* Returns (DISPLAY_INT)
*	value of GetPixelWidth()
******************************************************************************/
DISPLAY_INT HTMLElement::GetMinWidth(void)
{
	return (GetPixelWidth());
}

/******************************************************************************
* DISPLAY_INT HTMLElement::GetMaxWidth() -
*
* Parameters
*    none
*
* Description - This method gets the maximum width this element can be.  This
*				method will return 0 unless this method or GetWidth is
*               overridden
*
* Returns (DISPLAY_INT)
*	value of GetPixelWidth()
******************************************************************************/
DISPLAY_INT HTMLElement::GetMaxWidth(void)
{
	return (GetPixelWidth());
}

/******************************************************************************
* DISPLAY_INT HTMLElement::GetPixelWidth() -
*
* Parameters
*    none
*
* Description - This method get the width of the current element.  This method
*               returns 0 unless it is overridden.
*
*
* Returns (DISPLAY_INT)
*	the width
******************************************************************************/
DISPLAY_INT HTMLElement::GetPixelWidth(void)
{
	return (0);
}

/******************************************************************************
* DISPLAY_INT HTMLElement::GetPixelHeight() -
*
* Parameters
*    none
*
* Description - This method returns the height of this element.  This method
*               returns 0 unless it is overridden.
*
*
* Returns (DISPLAY_INT)
*	the height
******************************************************************************/
DISPLAY_INT HTMLElement::GetPixelHeight(void)
{
	return (0);
}


/******************************************************************************
* DISPLAY_INT HTMLElement::GetBaseline() -
*
* Parameters
*    none
*
* Description - This method is used to get the Baseline. The Baseline is
*               where an element is to vertically align itself with other
*               elements.
*
*
* Returns (DISPLAY_INT)
*	the baseline
******************************************************************************/
DISPLAY_INT HTMLElement::GetBaseline(void)
{
	return (GetPixelHeight());
}

/******************************************************************************
* WEBC_UINT8 HTMLElement::GetFloat() -
*
* Parameters
*    none
*
* Description - This method gets the Float value of this element.  The Float
*               is general horizontal positioning information of the element.
*
*
* Returns (WEBC_UINT8)
*	the float value
******************************************************************************/
WEBC_UINT8 HTMLElement::GetFloat(void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETFLOAT);

	CSSPropertyValue value;
	CSSValueType result;

	result = GetStyleFromCSS(CSS_PROPERTY_FLOAT, &value);
	if (result != CSS_VALUE_SPECIFIED)
	{
		if (GetStyleFromHTML(CSS_PROPERTY_FLOAT, &value) != CSS_VALUE_SPECIFIED)
		{
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETFLOAT);
			return (TU_FLOAT_NONE);
		}
	}

	switch (value.floatType)
	{
		case CSS_FLOAT_LEFT:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETFLOAT);
			return (TU_FLOAT_LEFT);

		case CSS_FLOAT_RIGHT:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETFLOAT);
			return (TU_FLOAT_RIGHT);
        default:
            break;
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETFLOAT);
	return (TU_FLOAT_NONE);
}


/******************************************************************************
* WEBC_UINT16 HTMLElement::GetPosition(void) -
*
* Parameters
*
* Description -
*
* Returns (WEBC_UINT16)
******************************************************************************/
WEBC_UINT16 HTMLElement::GetPosition(void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETPOSITION);

	CSSPropertyValue value;

	if (mFlags & HELEM_FLAG_USERPOSITION)
	{
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETPOSITION);
		return (TU_POSITION_ABSOLUTE);
	}

	if (GetStyleFromCSS(CSS_PROPERTY_POSITION, &value) != CSS_VALUE_SPECIFIED)
	{
		if (GetStyleFromHTML(CSS_PROPERTY_POSITION, &value) != CSS_VALUE_SPECIFIED)
		{
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETPOSITION);
			return (TU_POSITION_STATIC);
		}
	}

	switch (value.position)
	{
		case CSS_POSITION_ABSOLUTE:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETPOSITION);
			return (TU_POSITION_ABSOLUTE);

		case CSS_POSITION_RELATIVE:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETPOSITION);
			return (TU_POSITION_RELATIVE);

		case CSS_POSITION_FIXED:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETPOSITION);
			return (TU_POSITION_FIXED);

		default:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETPOSITION);
			return (TU_POSITION_STATIC);
	}
}

/******************************************************************************
* DISPLAY_INT HTMLElement::GetLeft(void) -
*
* Parameters
*
* Description -
*
* Returns (DISPLAY_INT)
******************************************************************************/
DISPLAY_INT HTMLElement::GetLeft(void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETLEFT);

	CSSPropertyValue value;

	if (mFlags & HELEM_FLAG_USERPOSITION)
	{
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETLEFT);
		return (0);
	}

	if (GetStyleFromCSS(CSS_PROPERTY_LEFT, &value) != CSS_VALUE_SPECIFIED)
	{
		if (GetStyleFromHTML(CSS_PROPERTY_LEFT, &value) != CSS_VALUE_SPECIFIED)
		{
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETLEFT);
			return (0);
		}
	}

	DISPLAY_INT x = CSS_LengthToPixels(&value.length, 0, WEBC_CFG_DEFAULT_TEXT_HEIGHT, WEBC_CFG_DEFAULT_TEXT_HEIGHT/2);

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETLEFT);

	return (x);
}

/******************************************************************************
* DISPLAY_INT HTMLElement::GetTop(void) -
*
* Parameters
*
* Description -
*
* Returns (DISPLAY_INT)
******************************************************************************/
DISPLAY_INT HTMLElement::GetTop(void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETTOP);

	CSSPropertyValue value;

	if (mFlags & HELEM_FLAG_USERPOSITION)
	{
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETTOP);
		return (0);
	}

	if (GetStyleFromCSS(CSS_PROPERTY_TOP, &value) != CSS_VALUE_SPECIFIED)
	{
		if (GetStyleFromHTML(CSS_PROPERTY_TOP, &value) != CSS_VALUE_SPECIFIED)
		{
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETTOP);
			return (0);
		}
	}

	DISPLAY_INT y = CSS_LengthToPixels(&value.length, 0, WEBC_CFG_DEFAULT_TEXT_HEIGHT, WEBC_CFG_DEFAULT_TEXT_HEIGHT/2);

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETTOP);

	return (y);
}

/******************************************************************************
* WEBC_UINT8 HTMLElement::GetTextAlign() -
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
WEBC_UINT8 HTMLElement::GetTextAlign(void)
{
	return (TU_ALIGN_NONE);
}


/******************************************************************************
* WEBC_UINT8 HTMLElement::GetChildTextAlign() -
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
WEBC_UINT8 HTMLElement::GetChildTextAlign(void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETCHILDTEXTALIGN);

	CSSPropertyValue value;

	if ((GetStyleFromCSS(CSS_PROPERTY_TEXT_ALIGN, &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(CSS_PROPERTY_TEXT_ALIGN, &value) == CSS_VALUE_SPECIFIED))
	{
		switch (value.textAlign)
		{
			case CSS_TEXT_ALIGN_RIGHT:
				WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETCHILDTEXTALIGN);
				return (TU_ALIGN_RIGHT);

			case CSS_TEXT_ALIGN_CENTER:
				WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETCHILDTEXTALIGN);
				return (TU_ALIGN_CENTER);

			case CSS_TEXT_ALIGN_LEFT:  // intentional fall-through
			default:
				WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETCHILDTEXTALIGN);
				return (TU_ALIGN_LEFT);
		}
	}

	WEBC_UINT8 a = mpParent? mpParent->GetChildTextAlign() : TU_ALIGN_LEFT;

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETCHILDTEXTALIGN);

	return (a);
}


/******************************************************************************
* WEBC_UINT8 HTMLElement::GetVAlign() -
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
WEBC_UINT8 HTMLElement::GetVAlign(void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETVALIGN);

	CSSPropertyValue value;

	if (GetStyleFromCSS(CSS_PROPERTY_VERTICAL_ALIGN, &value) != CSS_VALUE_SPECIFIED)
	{
		if (GetStyleFromHTML(CSS_PROPERTY_VERTICAL_ALIGN, &value) != CSS_VALUE_SPECIFIED)
		{
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETVALIGN);
			return (TU_VALIGN_BASELINE);
		}
	}

	switch (value.verticalAlign)
	{
		case CSS_VERTICAL_ALIGN_BOTTOM:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETVALIGN);
			return (TU_VALIGN_BOTTOM);

		case CSS_VERTICAL_ALIGN_MIDDLE:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETVALIGN);
			return (TU_VALIGN_MIDDLE);

		case CSS_VERTICAL_ALIGN_SUB:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETVALIGN);
			return (TU_VALIGN_SUB);

		case CSS_VERTICAL_ALIGN_SUPER:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETVALIGN);
			return (TU_VALIGN_SUPER);

		case CSS_VERTICAL_ALIGN_TEXT_TOP:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETVALIGN);
			return (TU_VALIGN_TEXTTOP);

		case CSS_VERTICAL_ALIGN_TEXT_BOTTOM:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETVALIGN);
			return (TU_VALIGN_TEXTBOTTOM);

		case CSS_VERTICAL_ALIGN_TOP:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETVALIGN);
			return (TU_VALIGN_TOP);

		default:
		case CSS_VERTICAL_ALIGN_BASELINE:
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETVALIGN);
			return (TU_VALIGN_BASELINE);
	}
}


/******************************************************************************
* short HTMLElement::GetZIndex() -
*
* Parameters
*    none
*
* Description - This method returns the three-dimensional position of the
*               given element.
*
*
* Returns (long)
*	the z-index
******************************************************************************/

short HTMLElement::GetZIndex(void)
{
  #if (WEBC_SUPPORT_ZINDEX)

	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETZINDEX);

	CSSPropertyValue value;

  	if (GetStyleFromCSS(CSS_PROPERTY_ZINDEX, &value) != CSS_VALUE_SPECIFIED)
  	{
		if (GetStyleFromHTML(CSS_PROPERTY_ZINDEX, &value) != CSS_VALUE_SPECIFIED)
		{
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETZINDEX);
			return (0);
		}
  	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETZINDEX);
	return (UTIL_SAFE_LONG_TO_SHORT(value.number));

  #else

	return (0);

  #endif // WEBC_SUPPORT_ZINDEX
}


/******************************************************************************
* void HTMLElement::SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail) -
*
* Parameters
*    DISPLAY_INT iWidth - The new width of the parent element.
*
* Description - This method sets the width of this element's parent.
*               This method does nothing unless it is overriden.
*
* Returns (void)
*
******************************************************************************/
void HTMLElement::SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail)
{
}

/******************************************************************************
* void HTMLElement::SetParentHeight(DISPLAY_INT iHeight) -
*
* Parameters
*    DISPLAY_INT iHeight - The new height of the parent element.
*
* Description - This method sets the height of this element's parent.
*               This method does nothing unless it is overriden.
*
*
* Returns (void)
*
******************************************************************************/
void HTMLElement::SetParentHeight(DISPLAY_INT iHeight)
{
}

/******************************************************************************
* void HTMLElement::SetPosition(DISPLAY_INT iXPos, DISPLAY_INT iYPos) -
*
* Parameters
*    DISPLAY_INT iXPos - The new X-position for this element
*    DISPLAY_INT iYPos - The new Y-position for this element
*
* Description - This method sets a new position relative to its parent element
*               This method does nothing unless it is overridden.
*
* Returns (void)
*
******************************************************************************/
void HTMLElement::SetXYPosition(DISPLAY_INT iXPos, DISPLAY_INT iYPos)
{
}

/******************************************************************************
* void HTMLElement::Release() -
*
* Parameters
*    none
*
* Description - This method releases and element once it is done being used.
*				This method does nothing unless it is overridden.
*
* Returns (void)
*
******************************************************************************/
void HTMLElement::Release(void)
{
}


/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

/******************************************************************************
* void HTMLElement::Update(Observable o, WEBC_PFBYTE data) -
*
* Parameters
*    Observable o - reserved for future use, pass 0
*    WEBC_PFBYTE data  - reserved for future use, pass 0
*
* Description - This method should be called when the state of the element
*               is explicitly changed, and internal and visual representations
*               need to be updated.
*
* Returns (void)
*
******************************************************************************/

void HTMLElement::Update(WEBC_PFBYTE o, WEBC_PFBYTE data)
{
	LocalStyleUpdate();
	InheritedStyleUpdate();
}


/******************************************************************************
* int HTMLElement::DefaultEvent(HTMLEvent *pEvent)
*
* Parameters
*    HTMLEvent *pEvent - a pointer to the event that occured on this object.
*
* Description - This method executes when a default event occurs on this
*               element.  This method does nothing unless it is overridden.
*
*
* Returns (int)
*	0 on success -1 on failure
******************************************************************************/
HTMLEventStatus HTMLElement::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);

	return (HTML_EVENT_STATUS_CONTINUE);
}


/******************************************************************************
* HTMLElement *HTMLElement::EventParent(void)
*
* Parameters
*
* Description -
*
* Returns (HTMLElement)
*	The HTMLElement to pass events to if they aren't handled by this element.
******************************************************************************/
HTMLElement *HTMLElement::EventParent (void)
{
	return (mpParent);
}

/******************************************************************************
* HTMLDocument* HTMLElement::GetDocument(void)
*
* Parameters
*    none
*
* Description - This method gets the Document that this element belongs to.
*               NOTE: It searches up the element tree until it finds the Body
*               element and gets the document from that.
*
*
* Returns (HTMLDocument*)
*	the document associated with this element
******************************************************************************/
HTMLDocument *HTMLElement::GetDocument(void)
{
	return (mDocRef.Get());
}

/******************************************************************************
* HTMLElement* HTMLElement::FindElement(HTMLElementFinder *pFinder) -
*
* Parameters
*    HTMLElementFinder *pFinder - this is the the finder that specifies what
*                                 element or element type that is being
*                                 searched for.
*
* Description - This method does a depth first search through the element tree
*               starting at this element.  The type of element that is being
*               searched for is specified by the type of HTMLElementFinder
*				that is passed in.  See htmlfind.hpp for the various types of
*				finders.
*
* Returns (HTMLElement*)
*	the first element that satisfied the HTMLElementFinder::Check query.
******************************************************************************/
HTMLElement *HTMLElement::FindElement(HTMLElementFinder *pFinder, int iSearchFrames, WEBC_BOOL bIncludeSelf)
{
	HTMLElement *pNode, *pSearch;

	if(bIncludeSelf)
	{
		if (pFinder->Check(this))
			return this;
	}

	pNode = FirstChild();
	while (pNode)
	{
		pSearch = pNode->FindElement(pFinder, iSearchFrames, WEBC_TRUE);
		if (pSearch)
			return (pSearch);
		pNode = pNode->mpNext;
	}

	return (0);
}


/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/******************************************************************************
* void HTMLElement::Reset()
*
* Parameters
*
* Description
*
*
* Returns (RETURNTYPE)
*
******************************************************************************/
void HTMLElement::Reset(void)
{
}


/******************************************************************************
* void HTMLElement::Select()
*
* Parameters
*
* Description
*
*
* Returns (RETURNTYPE)
*
******************************************************************************/
void HTMLElement::Select(void)
{
}


/******************************************************************************
* int HTMLElement::SetProperty(HTMLAttributeType, WEBC_CHAR*, HTMLDocument*, int)
*
* Parameters
*    Property  - DESCRIPTION
*    value     - DESCRIPTION
*    pDocument - DESCRIPTION
*    call_base - DESCRIPTION
*
* Description
*
*
* Returns (int)
*
******************************************************************************/
int HTMLElement::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_ID:
			SetId(value);
			return (1);

		case HTML_ATTRIB_TITLE:
			SetTitle(value);
			return (1);

		case HTML_ATTRIB_DIR:
			SetTextDirection(value);
			return (1);

		case HTML_ATTRIB_LANG:
			SetLanguage(value);
			return (1);

		case HTML_ATTRIB_CLASS:
			SetClass(value, pDocument);
			return (1);

#if (WEBC_SUPPORT_STYLE_SHEETS)
		case HTML_ATTRIB_STYLE:
			if (!pDocument || !pDocument->GetCSSContext())
			{
				break;
			}
			SetFlag(HELEM_FLAG_HAS_INLINE_STYLE);
			pDocument->GetCSSContext()->ParseElementStyle(this, value, pDocument->CurrentURL());
			StyleUpdate();
			return (1);
#endif // (WEBC_SUPPORT_STYLE_SHEETS)

#if (WEBC_SUPPORT_USER_EVENTS)
		case HTML_ATTRIB_EVENTHANDLER:
			SetEventHandlerName(value);
			return (1);
#endif

#if (WEBC_SUPPORT_JSCRIPT)
		case HTML_ATTRIB_ONCLICK:
			AddJSEventHandler(HTML_EVENT_CLICK, "onclick", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONDBLCLICK:
			AddJSEventHandler(HTML_EVENT_DBLCLICK, "ondblclick", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONKEYDOWN:
			AddJSEventHandler(HTML_EVENT_KEYDOWN, "onkeydown", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONKEYPRESS:
			AddJSEventHandler(HTML_EVENT_KEYPRESS, "onkeypress", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONKEYUP:
			AddJSEventHandler(HTML_EVENT_KEYUP, "onkeyup", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONMOUSEDOWN:
			AddJSEventHandler(HTML_EVENT_MOUSEDOWN, "onmousedown", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONMOUSEMOVE:
			AddJSEventHandler(HTML_EVENT_MOUSEMOVE, "onmousemove", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONMOUSEOUT:
			AddJSEventHandler(HTML_EVENT_MOUSEOUT, "onmouseout", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONMOUSEOVER:
			AddJSEventHandler(HTML_EVENT_MOUSEOVER, "onmouseover", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONMOUSEUP:
			AddJSEventHandler(HTML_EVENT_MOUSEUP, "onmouseup", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONLOSECAPTURE:
			AddJSEventHandler(HTML_EVENT_CHANGE, "onlosecapture", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONFOCUS:
			AddJSEventHandler(HTML_EVENT_FOCUS, "onfocus", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONBLUR:
			AddJSEventHandler(HTML_EVENT_UNFOCUS, "onunfocus", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONLOAD:
			AddJSEventHandler(HTML_EVENT_LOAD, "onload", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONUNLOAD:
			AddJSEventHandler(HTML_EVENT_UNLOAD, "onunload", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONSUBMIT:
			AddJSEventHandler(HTML_EVENT_SUBMIT, "onsubmit", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONCHANGE:
			AddJSEventHandler(HTML_EVENT_CHANGE, "onchange", value, webc_strlen(value), pDocument);
			return (1);

		case HTML_ATTRIB_ONRESET:
			AddJSEventHandler(HTML_EVENT_RESET, "onreset", value, webc_strlen(value), pDocument);
			return (1);

#endif // WEBC_SUPPORT_JSCRIPT

		default:
			break;
	}

	return (0);
}

/*
int HTMLElement::GetProperty(HTMLAttributeType Property, HTMLProperty *pValue)
{
	switch (Property)
	{
		case HTML_ATTRIB_ID:
			pValue->type = HTML_PROPERTY_TYPE_TEXT;
			pValue->value.text = Id(value);
			return (1);

		case HTML_ATTRIB_CLASS:
			return (0);

		case HTML_ATTRIB_DISPLAY:
			return (0);

#if (WEBC_SUPPORT_USER_EVENTS)
		case HTML_ATTRIB_EVENTHANDLER:
			return (0);
#endif

		default:
			break;
	}

	return (0);
}
*/

CSSValueType HTMLElement::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	return (CSS_VALUE_UNKNOWN);
}

CSSValueType HTMLElement::GetStyleFromParent(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETSTYLEFROMPARENT);

	if (!mpParent)
	{
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETSTYLEFROMPARENT);
		return (CSS_VALUE_UNKNOWN);
	}

	if ((mpParent->GetStyleFromCSS(Property, pValue) == CSS_VALUE_SPECIFIED) ||
	    (mpParent->GetStyleFromHTML(Property, pValue) == CSS_VALUE_SPECIFIED) ||
	    (mpParent->GetStyleFromParent(Property, pValue) == CSS_VALUE_SPECIFIED))
	{
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETSTYLEFROMPARENT);
		return (CSS_VALUE_SPECIFIED);
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETSTYLEFROMPARENT);

	return (CSS_VALUE_UNKNOWN);
}

void HTMLElement::SetId(const WEBC_CHAR *pId)
{
	if(!pId)
		return;
	WEBC_CHAR *pStr = webc_malloc_string_copy(pId, __FILE__, __LINE__);
	webc_free_string_copy(mpId, __FILE__, __LINE__);
 	mpId = pStr;

#if (WEBC_SUPPORT_STYLE_SHEETS)

	const WEBC_CHAR *cstr = webc_string_table_add(pId, __FILE__, __LINE__);
	webc_string_table_remove(mpIdLowercase, __FILE__, __LINE__);
	mpIdLowercase = cstr;

#endif

}

WEBC_CHAR *HTMLElement::Id(void)
{
	return (mpId);
}

void HTMLElement::SetTitle(const WEBC_CHAR* pTitle)
{
	if(!pTitle)
		return;
	WEBC_CHAR *pStr = webc_malloc_string_copy(pTitle, __FILE__, __LINE__);
	webc_free_string_copy(mpTitle, __FILE__, __LINE__);
 	mpTitle = pStr;
}

WEBC_CHAR *HTMLElement::Title(void)
{
	return (mpTitle);
}

void HTMLElement::SetTextDirection(const WEBC_CHAR* pTextDirection)
{
	if(!pTextDirection)
		return;
	WEBC_CHAR *pStr = webc_malloc_string_copy(pTextDirection, __FILE__, __LINE__);
	webc_free_string_copy(mpTextDirection, __FILE__, __LINE__);
 	mpTextDirection = pStr;
}

WEBC_CHAR *HTMLElement::TextDirection(void)
{
	return (mpTextDirection);
}

void HTMLElement::SetLanguage(const WEBC_CHAR* pLanguage)
{
	if(!pLanguage)
		return;
	WEBC_CHAR *pStr = webc_malloc_string_copy(pLanguage, __FILE__, __LINE__);
	webc_free_string_copy(mpLanguage, __FILE__, __LINE__);
 	mpLanguage = pStr;
}

WEBC_CHAR *HTMLElement::Language(void)
{
	return (mpLanguage);
}

WEBC_BOOL HTMLElement::IsNameOrId(WEBC_CHAR *str)
{
	return (str && ((Name() && webc_stricmp(Name(), str) == 0) || (Id() && webc_stricmp(Id(), str) == 0)));
}

int HTMLElement::IsInput()
{
	switch (Type())
	{
		case HTML_EDITBOX_ELEMENT:
		case HTML_EDIT_STR_ELEMENT:
		case HTML_RADIO_BUTTON_ELEMENT:
		case HTML_BUTTON_ELEMENT:
		case HTML_CHECKBOX_ELEMENT:
		case HTML_SELECT_ELEMENT:
		case HTML_HIDDEN_INPUT_ELEMENT:
		case HTML_INPUT_BUTTON_ELEMENT:
		case HTML_INPUT_PASSWORD_ELEMENT:
			return 1;
        default:
            break;
	}
	return 0;
}

void HTMLElement::SetClass(WEBC_CHAR *pName, HTMLDocument *pDoc)
{
	if ((!pDoc && !mpParent) || !pName)
	{
		return;
	}

	if (!pDoc)
	{
		pDoc = GetDocument();
		if (!pDoc)
		{
			return;
		}
	}
	ResetClass();
	WEBC_CHAR ch;
	while (pName[0])
	{
		WEBC_CHAR *end = pName;
		while (end[0] && end[0] != ',' && end[0] != ' ')
		{
			end++;
		}

		ch = end[0];
		end[0] = 0;

		const WEBC_CHAR *className = webc_string_table_add(pName, __FILE__, __LINE__);
		if (className)
		{
			if (vector_add_node_to_front(&mClassList, (unsigned char*)&className) < 0)
			{
				webc_string_table_remove(className, __FILE__, __LINE__);
			}
		}

		end[0] = ch;
		while (end[0] == ',' || end[0] == ' ')
		{
			end++;
		}
		pName = end;
	}
}


#if (WEBC_SUPPORT_USER_EVENTS)
void HTMLElement::SetEventHandlerName(WEBC_CHAR *pName)
{
WEBC_CHAR *pParamStart, *pParam;
int n, paren_nesting;

	if (!pName)
	{
		return;
	}

	pParamStart = webc_strchr(pName,'(');
	if (!pParamStart)
	{
		mpUserEventHandler = HTML_FindEventHandler(pName, this);
		mpUserParam = 0;
		return;
	}

	pParam = 0;
	*pParamStart = '\0';
	pParamStart++;

	paren_nesting = 1;
	for (n=0; pParamStart[n]; n++)
	{
		if (pParamStart[n] == '(')
		{
			paren_nesting++;
		}

		if (pParamStart[n] == ')')
		{
			paren_nesting = EBSMAX(0,paren_nesting-1);
			if (paren_nesting == 0)
			{
				pParamStart[n] = '\0';
				pParam = webc_malloc_string_copy(pParamStart, __FILE__, __LINE__);
				pParamStart[n] = ')';
				break;
			}
		}
	}
	webc_free_string_copy(mpUserParam, __FILE__, __LINE__);
	if (!pParam)
	{
		pParam = webc_malloc_string_copy(pParamStart, __FILE__, __LINE__);
	}
	mpUserParam = pParam;
	mpUserEventHandler = HTML_FindEventHandler(pName, this);
	if (!mpUserEventHandler)
	{
		webc_free_string_copy(mpUserParam, __FILE__, __LINE__);
		mpUserParam = 0;
	}

	pParamStart[-1] = '(';
}
#endif


/*****************************************************************************/
// HTMLElement-Specific Methods
/*****************************************************************************/

void HTMLElement::SetTagProperties (
		HTMLTagParse* tagParse,
		HTMLDocument* document,
		int callBase
	)
{
	WebString valueBuffer;
	SetTagProperties(tagParse, document, &valueBuffer, callBase);
}

void HTMLElement::SetTagProperties (
		HTMLTagParse* tagParse,
		HTMLDocument* document,
		WebString* valueBuffer,
		int callBase
	)
{
	if (tagParse)
	{
		int n;
		HTMLAttributeType attrib;
		XMLAttribListParse* attribList;

	  #if (WEBC_SUPPORT_INNER_HTML)
		WEBC_CHAR* savedAttribs = (WEBC_CHAR*) WEBC_MALLOC(2048);
		long savedAttribsSize = 0;
	  #endif // WEBC_SUPPORT_INNER_HTML

		if (document == 0)
		{
			document = GetDocument();
		}

		mTag = tagParse->htmlTag;

		attribList = &tagParse->xmlParse.parse.tag.attribList;

		for (n=0; n < attribList->count; n++)
		{
			attrib = HTML_ParseAttributeType (
					attribList->array[n].name.str,
					attribList->array[n].name.len
				);

			// Copy the value string to a temporary buffer, converting HTML
			//  escape characters to Ascii.
			if (valueBuffer->copy(attribList->array[n].value.str, attribList->array[n].value.len) >= 0)
			{
				static WEBC_CHAR eolChars[] = {'\r', '\n', 0};

				valueBuffer->unescapeHtmlChars();
				valueBuffer->removeChars(eolChars);
				valueBuffer->removeLeadingWhitespace();
				valueBuffer->removeTrailingWhitespace();

				if (attrib == HTML_UNKNOWN_ATTRIB)
				{
				  #if (WEBC_SUPPORT_USER_EVENTS)
					if (mpUserEventHandler)
					{
						HTMLEvent e;
						e.type = HTML_EVENT_ATTRIBUTE_PARSED;
						e.data.attribute.name = attribList->array[n].name.str;
						e.data.attribute.value = valueBuffer->getChars();
						mpUserEventHandler->Event(this, &e, mpUserParam);
					}
				  #endif // WEBC_SUPPORT_USER_EVENTS
				}
				else
				{
				  #if (WEBC_SUPPORT_INNER_HTML)

					if (savedAttribs && (savedAttribsSize+1+valueBuffer->getLength()+1)*sizeof(WEBC_CHAR)<2048 )
					{
						savedAttribs[savedAttribsSize] = (WEBC_CHAR) attrib;
						savedAttribsSize++;

						tc_memcpy (
								savedAttribs + savedAttribsSize,
								valueBuffer->getChars(),
								sizeof(WEBC_CHAR) * (valueBuffer->getLength() + 1)
							);

						savedAttribsSize += (valueBuffer->getLength() + 1);
					}
				  #endif // WEBC_SUPPORT_INNER_HTML

					SetProperty(attrib, valueBuffer->getChars(), document, callBase);
				}
			}
		}

	  #if (WEBC_SUPPORT_INNER_HTML)
		if (savedAttribs)
		{
			if (savedAttribsSize > 0)
			{
				if (mpTagAttributes)
				{
					WEBC_FREE(mpTagAttributes);
				}
				mpTagAttributes = (WEBC_CHAR*) WEBC_MALLOC_VERBOSE(sizeof(WEBC_CHAR)*(savedAttribsSize + 1),"InnerhtmlSavedAttributes");
				if (mpTagAttributes)
				{
					tc_memcpy(mpTagAttributes, savedAttribs, sizeof(WEBC_CHAR)*savedAttribsSize);
					mpTagAttributes[savedAttribsSize] = (WEBC_CHAR) HTML_UNKNOWN_ATTRIB;
				}
			}
			WEBC_FREE(savedAttribs);
		}
	  #endif // WEBC_SUPPORT_INNER_HTML
	}
	else
	{
		mTag = HTML_UNKNOWN_TAG;
	}
}


HTMLElement *HTMLElement::Root(void)
{
HTMLElement *pNode = this;

	while (pNode)
	{
		if (pNode->mpParent == 0)
			return (pNode);
		pNode = pNode->mpParent;
	}

	return (0);
}

HTMLBrowser * HTMLElement::MakeNewTargetWindow(HTMLDocument *pDoc, char *pName, char *pURL)
{
	// Clone the graphics abstraction layer function pointers so we know who to call
	HTMLBrowserConfig config, docConfig;
	rtp_memset(&config, 0, sizeof(config));
	rtp_memset(&docConfig, 0, sizeof(docConfig));
	pDoc->GetBrowser()->GetConfig(&docConfig);
	config.graphics = docConfig.graphics;

	config.WindowConfig = docConfig.WindowConfig;

	HTMLBrowser *pBrowser = 0;
	// Find an existing browser (window) by name or create one
	pBrowser = (HTMLBrowser *) webc_FindBrowserByNameAscii(pName);
	if (!pBrowser)
	{
		pBrowser = (HTMLBrowser *) webc_CreateBrowser(&config,  WEBC_TRUE, pName, "");
	}

	if (pURL && pBrowser)
	{
		if (pBrowser->GetDocument())
		{
			webc_BrowserLoadUrl((HBROWSER_HANDLE) pBrowser, pURL);
		}
	}

	return pBrowser;
}

HTMLDocument *HTMLElement::GetFrameDocument(WEBC_CHAR *pTargName)
{
HTMLDocument *pDoc = GetDocument();
HTMLDocument *pTargDoc = pDoc;

	if (!pTargName)
	{
		pTargName = pDoc->BaseTarget();
	}

	if (pTargName)
	{
		if (!webc_c_stricmp(pTargName, "_top"))
		{
			while (!pTargDoc->IsQuarantined() && pTargDoc->Parent())
			{
				pTargDoc = pTargDoc->Parent();
			}
		}
		else if (!webc_c_stricmp(pTargName, "_parent"))
		{
			if (!pDoc->IsQuarantined() && pDoc->Parent())
			{
				pTargDoc = pDoc->Parent();
			}
		}
		else if (!webc_c_stricmp(pTargName, "_self"))
		{
			pTargDoc = pDoc;
		}
		else if (!webc_c_stricmp(pTargName, "_blank"))
		{
#if 0
			HTMLBrowser* browser = pDoc->GetBrowser();

			if (browser)
			{
				HTMLBrowserEvent e;

				e.type = HTML_BROWSER_EVENT_BLANK_DOCUMENT_REQUESTED;
				e.hdoc = (HDOC_HANDLE) pDoc;
				e.out.blankDocument = WEBC_NULL;

				if (browser->TriggerBrowserEvent(&e) != HTML_EVENT_STATUS_HALT)
				{
					if (e.out.blankDocument != WEBC_NULL)
					{
						pTargDoc = (HTMLDocument*) e.out.blankDocument;
						pTargDoc->SetURL(pDoc->BaseURL());
					}
				}
			}
#else
			HTMLBrowser* browser = MakeNewTargetWindow(pDoc, "SOME TEMP NAME");

			pTargDoc = browser->GetDocument();

#endif
		}
		else
		{
            // <EDIT 20080201 - Added Search for a window name if GetFrameDocument() does not find a frame by name
		    int match = 0;
#if (WEBC_SUPPORT_FRAMES)

			HTMLFrame *pFrame;
			HTMLFrameByNameFinder f(pTargName);

			// First look in this document; in case we are in a frameset document or the frame is inline (<IFRAME>)
			pFrame = (HTMLFrame *) pDoc->FindElement(&f);
			if (pFrame)
			{
		        match = 1;
				pTargDoc = pFrame->FrameDocument();
			}
			else
			{
				// Search up the document tree, looking for a document which
				//  contains a certain named frame; go from local to global,
				//  because there may be more than one frame with the specified
				//  name, and we want the most local one

				while (!pTargDoc->IsQuarantined() && pTargDoc->Parent())
				{
					pTargDoc = pTargDoc->Parent();
					pFrame = (HTMLFrame *) pTargDoc->FindElement(&f);
					if (pFrame)
					{
					    match = 1;
						pTargDoc = pFrame->FrameDocument();
						break;
					}
				}
			}
#endif
            // <EDIT 20080201 -  Search for a window name if GetFrameDocument() did not find a frame by name
            if (!match)
            {
                HBROWSER_HANDLE Browser = webc_FindBrowserByName(pTargName);
                if (Browser)
                {   // Replace what we found if we do not have an exact frame match but we do have an exact window name match
                    HTMLDocument *pWinTargDoc = 0;
                    pWinTargDoc = (HTMLDocument *) webc_BrowserGetDocument(Browser);
                    if (pWinTargDoc)
                        pTargDoc = pWinTargDoc;
                }
				else
				{
					int len=webc_strlen(pTargName);
					char *pTempChar = (char *)WEBC_MALLOC(sizeof(char) * (len+1));
					webc_to_c_strcpy(pTempChar, pTargName);

					// make a new window with the given name
					HTMLBrowser* browser = MakeNewTargetWindow(pDoc, pTempChar);

					WEBC_FREE(pTempChar);

					pTargDoc = browser->GetDocument();
				}

            }
		}
	}

	if (pTargDoc == 0)
	{
		pTargDoc = pDoc;
	}

	return (pTargDoc);
}


HTMLBrowser *HTMLElement::GetBrowser(void)
{
HTMLDocument *pDoc = GetDocument();

	if (pDoc)
	{
		return (pDoc->GetBrowser());
	}

	return (0);
}


HTMLElement *HTMLElement::FindParent(HTMLElementFinder* f)
{
HTMLElement *pNode = this;

	while (pNode)
	{
		if (f->Check(pNode))
		{
			return (pNode);
		}
		pNode = pNode->mpParent;
	}

	return (0);
}


HTMLEventStatus HTMLElement::UserEvent(HTMLEvent *pEvent)
{
HTMLEventStatus webcEventStatus = HTML_EVENT_STATUS_CONTINUE;
#if (WEBC_SUPPORT_USER_EVENTS)
    if (mpWebCElementEventHandler)
    {
		webcEventStatus = mpWebCElementEventHandler->Event(this, pEvent, mpUserParam);
    }
	if (mpUserEventHandler && webcEventStatus == HTML_EVENT_STATUS_CONTINUE)
	{
		webcEventStatus = mpUserEventHandler->Event(this, pEvent, mpUserParam);
	}
#endif
    return (webcEventStatus);
}

HTMLEventStatus HTMLElement::JSEvent(HTMLEvent *pEvent)
{
	HTMLEventStatus retVal = HTML_EVENT_STATUS_DONE;
	int n=0;
	do
	{
		retVal = JSEvent_Internal(pEvent, n++);
	}
	while(retVal == HTML_EVENT_STATUS_DONE); // keep going so long as we're doing something
	return retVal;
}

HTMLEventStatus HTMLElement::JSEvent_Internal(HTMLEvent *pEvent, int n)
{
#if (WEBC_SUPPORT_JSCRIPT)

	WebcJSEventHandler *pHandler = FindJSEventHandler(pEvent->type, NULL, n);
	HTMLEventStatus result = HTML_EVENT_STATUS_CONTINUE;


	if (pHandler)
	{
		return (pHandler->Execute(pEvent));
	}

	if(n > 0)
		return (HTML_EVENT_STATUS_CONTINUE);

  #ifdef WEBC_NO_MOUSE
	if (pEvent->type == HTML_EVENT_KEYDOWN)
	{
		if (pEvent->data.key == WGK_CR)
		{
			//  check html tag to see if OnClick
			//	create new HTMLEvent and set key to Click, event is OnClick.
			//	init to all zeros and then set appropriate
			HTMLEvent EventTmp = *pEvent;
			EventTmp.data.position.x = 0;
			EventTmp.data.position.y = 0;
			EventTmp.type = HTML_EVENT_CLICK;
			pHandler = FindJSEventHandler(EventTmp.type);
			if (pHandler)
			{
				return (pHandler->Execute(&EventTmp));
			}
		}
	}
  #endif // WEBC_NO_MOUSE

	const char *pHandlerName = jutils_EventTypeToString(pEvent->type);

	JSObject *pJSObj = GetJSObject();
	if (pHandlerName && pJSObj)
	{
		HTMLDocument *pDoc = GetDocument();
		if (!pDoc)
		{
			return result;
		}

		WebcJSDocumentContext* pMgr = pDoc->GetJSMgr();
		JSContext *pCx = (pMgr)? pMgr->AllocContext() : 0;
		if (!pCx)
		{
			return result;
		}

		jsval jval;
		if (JS_GetProperty(pCx, pJSObj, pHandlerName, &jval) == JS_TRUE)
		{
			if (!JSVAL_IS_VOID(jval))
			{
				JSFunction *pFunc = (JSFunction *)JS_ValueToFunction(pCx, jval);
				JSBool result;
				if (pFunc)
				{
					WEBC_NEW_VERBOSE(pHandler, WebcJSEventHandler(pMgr, pJSObj, pHandlerName),"WebcJSEventHandler");
					if (pHandler)
					{
						result = pHandler->ExecuteEventHandler(pFunc, pEvent);
						pHandler->Destroy();
					}
				}
			}
		}

		pMgr->FreeContext(pCx);

		return (result);
	}

#endif //WEBC_SUPPORT_JSCRIPT

	return (HTML_EVENT_STATUS_CONTINUE);
}

HTMLEventStatus HTMLElement::Event(HTMLEvent *pEvent)
{
	HTMLEventStatus result = HTML_EVENT_STATUS_CONTINUE;
	HTMLElement *pElement = 0;
	HTMLEvent *pCurEvent = 0;
	HTMLEvent e;
	int iEventsProcessed = 0;

    WEBC_DEBUG_ReceiveEvent(this, pEvent);

	gpEventQueue->Fold(pEvent, this);

	WEBC_ASSERT(mbDepth < WEBC_CFG_EVENT_MAX_RECURSE_DEPTH);
	if (mbDepth < WEBC_CFG_EVENT_MAX_RECURSE_DEPTH)
	{
		mbDepth++;
		while (gpEventQueue->Dequeue(&pCurEvent, &pElement) != -1 )
		{
			e = *pCurEvent;
			result = pElement->ProcessEvent(&e);
			iEventsProcessed++;
		}
		mbDepth--;
	}


	return result;
}


HTMLEventStatus HTMLElement::ProcessEvent(HTMLEvent *pEvent)
{
HTMLEventStatus result;
int inherit_event = 1;
HTMLElement *pElement = this;
ObjectReference<HTMLElement> self(this);
ObjectReference<HTMLElement> elemRef;

	if (pEvent->type == HTML_EVENT_LOAD)
	{
		inherit_event = 0;
	}
	HTMLDocument *pDoc = pElement->GetDocument();

	pEvent->flags &= ~(HTML_EVENT_FLAG_CANCEL_BUBBLE | HTML_EVENT_FLAG_CANCEL_DEFAULT);
	pEvent->srcElement = (HELEMENT_HANDLE) this;

	// First look for a user event handler for this event
	while (pElement)
	{
		pEvent->target = (HELEMENT_HANDLE) pElement;

		elemRef.Set(pElement);
		result = pElement->UserEvent(pEvent);
		if (!elemRef.Get())
		{
			return (HTML_EVENT_STATUS_HALT);
		}

		if (result == HTML_EVENT_STATUS_HALT ||
			result == HTML_EVENT_STATUS_DONE)
		{
			return (result);
		}

		if (!inherit_event)
		{
			break;
		}

		pElement = pElement->EventParent();
	}

	if ((pEvent->type != HTML_EVENT_KEYDOWN) ||
	    ((pEvent->data.key != WGK_F1) && (pEvent->data.key != WGK_F2)))
	{
		// No user handler specified; process javascript handlers
		pElement = self.Get();
		if (!pElement)
		{
			return (HTML_EVENT_STATUS_HALT);
		}

		while (pElement)
		{
			pEvent->target = (HELEMENT_HANDLE) pElement;

			elemRef.Set(pElement);
			result = pElement->JSEvent(pEvent);
			if (!elemRef.Get())
			{
				return (HTML_EVENT_STATUS_HALT);
			}

			if (result == HTML_EVENT_STATUS_HALT)
			{
				return (result);
			}

			if ((pEvent->flags & HTML_EVENT_FLAG_CANCEL_BUBBLE) || !inherit_event)
			{
				break;
			}

			pElement = pElement->EventParent();
		}

		if (pEvent->flags & HTML_EVENT_FLAG_CANCEL_DEFAULT)
		{
			return (HTML_EVENT_STATUS_DONE);
		}
	}

	// Default behavior
	pElement = self.Get();
	if (!pElement)
	{
		return (HTML_EVENT_STATUS_HALT);
	}

	while (pElement)
	{
		pEvent->target = (HELEMENT_HANDLE) pElement;

		elemRef.Set(pElement);
		result = pElement->DefaultEvent(pEvent);
		if (!elemRef.Get())
		{
			return (HTML_EVENT_STATUS_HALT);
		}

		if (result == HTML_EVENT_STATUS_HALT ||
			result == HTML_EVENT_STATUS_DONE)
		{
			return (result);
		}

		// don't pass up load events
		if ((pEvent->flags & HTML_EVENT_FLAG_CANCEL_BUBBLE) || !inherit_event)
		{
			break;
		}

		pElement = pElement->EventParent();
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

#if (WEBC_SUPPORT_USER_EVENTS)


void HTMLElement::SetEventHandler(HTMLEventHandler *pUserEventHandler)
{
    if (mpUserEventHandler)
    {
		HTML_UnregisterEventHandler(mpUserEventHandler);
        WEBC_DELETE(mpUserEventHandler);
    }
	mpUserEventHandler = pUserEventHandler;
	mpUserParam = 0;
}


void HTMLElement::SetElementEventHandler(HTMLEventHandler *pUserEventHandler)
{
    if (mpWebCElementEventHandler)
    {
        WEBC_DELETE(mpWebCElementEventHandler);
    }
	mpWebCElementEventHandler = pUserEventHandler;
}


void HTMLElement::PushElementEventHandler(HTMLEventHandler *pUserEventHandler)
{
HTMLEventHandler *prevUserEventHandler;
    prevUserEventHandler = mpUserEventHandler;
    vector_add_node_to_front(&mHandlerVectorStack, (WEBC_UINT8 *) &prevUserEventHandler);
	mpUserEventHandler = 0;
	SetEventHandler(pUserEventHandler);

}
void HTMLElement::PopElementEventHandler(void)
{
	vector_iterator vi;
	if (mpUserEventHandler)
    {
        SetEventHandler(0);
    }
	HTMLEventHandler **prevUserEventHandler = (HTMLEventHandler **) vector_get_first(&mHandlerVectorStack, &vi);
	if (prevUserEventHandler)
	{
		SetEventHandler(*prevUserEventHandler);
		vector_delete_node(&mHandlerVectorStack,0);
	}
}
#endif

void HTMLElement::PushPrivateData(void* data)
{
    vector_add_node_to_front(&mPrivateDataStack, (WEBC_PFBYTE) &data);
}
void HTMLElement::PopPrivateData (void)
{
	if (GetFromPrivateDataStack())
		vector_delete_node(&mPrivateDataStack,0);
}
void * HTMLElement::GetFromPrivateDataStack (void)
{
vector_iterator vi;
void **pdata;
	pdata = (void**) vector_get_first(&mPrivateDataStack,&vi);
	return *pdata;
}

/*****************************************************************************/
// CSSStyleElement-interface methods
/*****************************************************************************/

#if (WEBC_SUPPORT_STYLE_SHEETS)

const WEBC_CHAR * HTMLElement::IdLowercase(void)
{
	return (mpIdLowercase);
}

CSSStyleElement * HTMLElement::GetStyleParent(void)
{
	return (mpParent);
}

#endif // WEBC_SUPPORT_STYLE_SHEETS

Enumerator * HTMLElement::GetClasses(void)
{
	HTMLElementClassEnumerator *e = 0;
	return (e->Alloc(&mClassList, __FILE__, __LINE__));
}

HTMLTagType HTMLElement::GetTag(void)
{
	return mTag;
}


/******************************************************************************
* void HTMLElement::StyleUpdate()
*
* Parameters
*
* Description
*    Called to notify an element that its visual style has changed
*
* Returns (void)
*
******************************************************************************/
void HTMLElement::StyleUpdate (NotifyEventType eventType)
{
	// invalidate cached style values
	LocalStyleUpdate();
	InheritedStyleUpdate();

	// notify display element that style has changed
	NotifyDisplay(eventType);
}


/******************************************************************************
* void HTMLElement::StyleUpdateChildren()
*
* Parameters
*
* Description
*    Called to notify an element that its visual style has changed; also
*    notifies its children that inherited styles must be updated
*
* Returns (void)
*
******************************************************************************/
void HTMLElement::StyleUpdateChildren (NotifyEventType eventType, NotifyEventType childEvent)
{
	StyleUpdate(eventType);

	HTMLElement *child = FirstChild();
	while (child)
	{
		child->ParentStyleUpdate(childEvent);
		child = child->mpNext;
	}
}


/******************************************************************************
* void HTMLElement::ParentStyleUpdate()
*
* Parameters
*
* Description
*    Called to notify an element that inherited styles must be updated;
*     Calls ParentStyleUpdate on each child
*
* Returns (void)
*
******************************************************************************/
void HTMLElement::ParentStyleUpdate (NotifyEventType eventType)
{
	// invalidate cached inherited style values
	InheritedStyleUpdate();

	// notify display element that parent's style has changed
	NotifyDisplay(eventType);

	HTMLElement *child = FirstChild();
	while (child)
	{
		child->ParentStyleUpdate(eventType);
		child = child->mpNext;
	}
}


/******************************************************************************
* void HTMLElement::LocalStyleUpdate()
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
void HTMLElement::LocalStyleUpdate (void)
{
	ClearFlag(HELEM_FLAG_UNIT_TYPE_CACHED);
}


/******************************************************************************
* void HTMLElement::InheritedStyleUpdate()
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
void HTMLElement::InheritedStyleUpdate (void)
{
	ClearFlag(HELEM_FLAG_FONT_CACHED |
	          HELEM_FLAG_FONT_FAMILY_CACHED |
	          HELEM_FLAG_FONT_SIZE_CACHED |
	          HELEM_FLAG_FONT_STYLE_CACHED |
	          HELEM_FLAG_FONT_WEIGHT_CACHED |
	          HELEM_FLAG_VISIBILITY_CACHED);
}

#if (WEBC_SUPPORT_JSCRIPT)

extern "C" {

int _matchJSHandler(WEBC_PFBYTE data, WEBC_PFBYTE param)
{
JSHandlerInfo *pjs_a = (JSHandlerInfo *) data;
EventHandlerSpec *pjs_b = (EventHandlerSpec *) param;

WEBC_BOOL bFuncMatches = true;
if(pjs_b->functionText && pjs_a->scriptSource)
{
	bFuncMatches = (0 == webc_strcmp(pjs_a->scriptSource, pjs_b->functionText));
}
return ((pjs_a->eventType == pjs_b->eventType) && bFuncMatches);
}

int _deleteJSHandler(WEBC_PFBYTE data, WEBC_PFBYTE param)
{
JSHandlerInfo *pjs_a = (JSHandlerInfo *) data;
EventHandlerSpec *pjs_b = (EventHandlerSpec *) param;

	pjs_a->handler->Destroy();
	pjs_a->handler = 0;

	// delete the script source code
	if (pjs_a->scriptSource)
	{
		WEBC_FREE(pjs_a->scriptSource);
		pjs_a->scriptSource = 0;
	}

	if(pjs_b)
		WEBC_DELETE(pjs_b);

	return (0);
}

int _deleteMatchingJSHandler(WEBC_PFBYTE data, WEBC_PFBYTE param)
{
	JSHandlerInfo *pjs_a = (JSHandlerInfo *) data;
	EventHandlerSpec *pjs_b = (EventHandlerSpec *) param;

	WEBC_BOOL bFuncMatches = true;
	if(pjs_b->functionText && pjs_a->scriptSource)
	{
		bFuncMatches = (0 == webc_strcmp(pjs_a->scriptSource, pjs_b->functionText));
	}

	if ((pjs_a->eventType == pjs_b->eventType) && bFuncMatches)
	{
		pjs_a->handler->Destroy();
		pjs_a->handler = 0;

		// delete the script source code
		if (pjs_a->scriptSource)
		{
			WEBC_FREE(pjs_a->scriptSource);
			pjs_a->scriptSource = 0;
		}

		return (1);
	}

	return (0);
}

} // exter C

WebcJSEventHandler * HTMLElement::AddJSEventHandler(HTMLEventType eventType, const char *handlerName, const WEBC_CHAR *source, long sourceLen, HTMLDocument *pDoc)
{
JSHandlerInfo jsinfo;
WebcJSEventHandler *pHandler;

	if (!pDoc)
	{
		pDoc = GetDocument();
		if (!pDoc)
		{
			return 0;
		}
	}

	pHandler = FindJSEventHandler(eventType, source);
	if (pHandler)
	{
		// if we already have an event handler for this type of event, then just
		//  clear the old code and compile the new
		pHandler->SetSource(source, sourceLen);
		return 0;
	}

	jsinfo.eventType = eventType;
	WebcJSDocumentContext *pMgr = pDoc->GetJSMgr();
	if (pMgr)
	{
		JSObject *jsobj = CreateJSObject();
		if (jsobj)
		{
			WEBC_NEW_VERBOSE(pHandler, WebcJSEventHandler(pMgr, jsobj, handlerName, source, sourceLen),"WebcJSEventHandler");
			if (pHandler)
			{
				jsinfo.handler = pHandler;

				// save the script source code so we can associate it with the handler (to get future matches)
				jsinfo.scriptSource = (WEBC_CHAR *) WEBC_MALLOC_VERBOSE(sizeof(WEBC_CHAR) * (sourceLen + 1),"ScriptSource");
				if (jsinfo.scriptSource)
				{
					tc_movebytes((unsigned char *) jsinfo.scriptSource,
					             (unsigned char *) source,
					             sourceLen * sizeof(WEBC_CHAR));
					jsinfo.scriptSource[sourceLen] = 0;
				}

				if (vector_add_node_to_front(&mJSHandlerVector, (WEBC_PFBYTE) &jsinfo) < 0)
				{
					// don't leak the script source code memory
					if (jsinfo.scriptSource)
					{
						WEBC_FREE(jsinfo.scriptSource);
					}

					pHandler->Destroy();
					pHandler = 0;
				}
			}
		}
	}

	return (pHandler);
}

WebcJSEventHandler * HTMLElement::AddJSEventHandler(HTMLEventType eventType, const char *handlerName, JSFunction *pFunc, const WEBC_CHAR *functionText, HTMLDocument *pDoc)
{
JSHandlerInfo jsinfo;
WebcJSEventHandler *pHandler;

	if (!pDoc)
	{
		pDoc = GetDocument();
		if (!pDoc)
		{
			return 0;
		}
	}

	pHandler = FindJSEventHandler(eventType, functionText);
	if (pHandler)
	{
		// if we already have an event handler for this type of event, then just
		//  clear the old code and compile the new
		pHandler->SetFunction(pFunc);
		return 0;
	}

	jsinfo.eventType = eventType;
	WebcJSDocumentContext *pMgr = pDoc->GetJSMgr();
	if (pMgr)
	{
		JSObject *jsobj = CreateJSObject();
		if (jsobj)
		{
			WEBC_NEW_VERBOSE(pHandler, WebcJSEventHandler(pMgr, jsobj, handlerName, pFunc),"WebcJSEventHandler");
			if (pHandler)
			{
				jsinfo.handler = pHandler;

				// save the script source code so we can associate it with the handler (matching logic)
				int funcTextLen = webc_strlen(functionText);
				jsinfo.scriptSource = (WEBC_CHAR *) WEBC_MALLOC_VERBOSE(sizeof(WEBC_CHAR) * (funcTextLen + 1),"EvenHandlerSource");
				if (jsinfo.scriptSource)
				{
					tc_movebytes((unsigned char *) jsinfo.scriptSource,
								 (unsigned char *) functionText,
								 funcTextLen * sizeof(WEBC_CHAR));
					jsinfo.scriptSource[funcTextLen] = 0;
				}

				if (vector_add_node_to_front(&mJSHandlerVector, (WEBC_PFBYTE) &jsinfo) < 0)
				{
					// don't leak the script source code
					if (jsinfo.scriptSource)
					{
						WEBC_FREE(jsinfo.scriptSource);
					}
					pHandler->Destroy();
					pHandler = 0;
				}
			}
		}
	}

	return (pHandler);
}

WebcJSEventHandler *HTMLElement::FindJSEventHandler(HTMLEventType eventType, const WEBC_CHAR *functionText, int n)
{
	EventHandlerSpec matchEntry={eventType, functionText};
	JSHandlerInfo *pjs =0;

	pjs = (JSHandlerInfo *) vector_find_nth_node(&mJSHandlerVector, _matchJSHandler, (WEBC_PFBYTE) &matchEntry, n);

	if (pjs)
	{
		return pjs->handler;
	}

	return 0;
}

void HTMLElement::RemoveJSEventHandler(HTMLEventType eventType, const WEBC_CHAR *functionText)
{
	EventHandlerSpec matchEntry={eventType, functionText};
	vector_delete_cond(&mJSHandlerVector, _deleteMatchingJSHandler, (WEBC_PFBYTE) &matchEntry);
}

void HTMLElement::ClearJSEventHandlers(void)
{
	vector_find_node(&mJSHandlerVector, _deleteJSHandler, 0);
	vector_delete(&mJSHandlerVector);
}

void HTMLElement::SetJSObject(JSObject *obj)
{
	mJSObject.SetPrivate(0);
	mJSObject.Set(obj);
	if (obj)
	{
		mJSObject.SetPrivate((HTMLElement*) this);
	}
}

WebcJSDocumentContext *HTMLElement::GetJSMgr(void)
{
HTMLDocument *pDoc = GetDocument();

	if (pDoc)
	{
		return (pDoc->GetJSMgr());
	}

	return 0;
}

#endif // WEBC_SUPPORT_JSCRIPT

#if (WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION)

#define DISTANCE(X,Y) (((X+1)>>1)*((X+1)>>1) + (Y*Y))

DISPLAY_INT _GetLeftDistance(
		DISPLAY_INT myLeft,
		DISPLAY_INT myRight,
		DISPLAY_INT herRight)
{
	if (myLeft > herRight)
	{
		return (myLeft - herRight);
	}

	if (herRight > ((myLeft + myRight) >> 1))
	{
		return (-1);
	}

	return (0);
}

DISPLAY_INT _GetVerticalDistance(
		DISPLAY_INT myTop,
		DISPLAY_INT myBottom,
		DISPLAY_INT herTop,
		DISPLAY_INT herBottom)
{
	if (myTop > herBottom)
	{
		return (myTop - herBottom);
	}

	if (herTop > myBottom)
	{
		return (herTop - myBottom);
	}

	return (0);
}

static int _BetterLeftMatch(HTMLElement *bestMatch, WebRect *from, WebRect *match, WebRect *contender)
{
	DISPLAY_INT xDistance, yDistance;

	xDistance = _GetLeftDistance(from->left,
	                             from->right,
	                             contender->right);

	if (xDistance < 0)
	{
		return (0);
	}

	yDistance = _GetVerticalDistance(from->top,
									 from->bottom,
									 contender->top,
									 contender->bottom);

	if (yDistance > xDistance)
	{
		return (0);
	}

	if (!bestMatch)
	{
		return (1);
	}

	int matchVOverlap = (from->bottom >= match->top && match->bottom >= from->top);
	int contenderVOverlap = (from->bottom >= contender->top && contender->bottom >= from->top);

	// if match overlaps vertically and contender doesn't return WEBC_FALSE
	if (matchVOverlap && !contenderVOverlap)
	{
		return (0);
	}

	if (!matchVOverlap && contenderVOverlap)
	{
		return (1);
	}

	if (contenderVOverlap && matchVOverlap)
	{
		DISPLAY_INT matchScore = 0;
		DISPLAY_INT contenderScore = 0;

		// if the contender and the current best match overlap horizontally, judge based on
		//  vertical midpoint distance
		if (contender->right >= match->left && match->right >= contender->left)
		{
			// if match is entirely within from's vertical range and contender isn't
			//  return WEBC_FALSE
			if (match->top >= from->top && match->top <= from->bottom &&
			    match->bottom >= from->top && match->bottom <= from->bottom)
			{
				matchScore++;
			}

			if (match->top <= from->top && match->top >= from->bottom &&
			    match->bottom >= from->top && match->bottom <= from->bottom)
			{
				matchScore++;
			}

			if (contender->top >= from->top && contender->top <= from->bottom &&
			    contender->bottom >= from->top && contender->bottom <= from->bottom)
			{
				contenderScore++;
			}

			if (contender->top <= from->top && contender->top >= from->bottom &&
			    contender->bottom >= from->top && contender->bottom <= from->bottom)
			{
				contenderScore++;
			}

			if (contenderScore > matchScore)
			{
				return (1);
			}

			if (contenderScore < matchScore)
			{
				return (0);
			}

			if (contenderScore > 0)
			{
				// both overlap vertically - choose the closer one on the horizontal axis only
				return ((from->left - contender->right) < (from->left - match->right));
			}
		}

		contenderScore = from->left - contender->right;
		matchScore = from->left - match->right;

		if (contenderScore < matchScore)
		{
			return (1);
		}

		if (contenderScore > matchScore)
		{
			return (0);
		}

		// give it to whoever has a greater overlap
		if (contender->top < from->top)
		{
			contenderScore = contender->bottom - from->top;
		}
		else
		{
			contenderScore = from->bottom - contender->top;
		}

		if (match->top < from->top)
		{
			matchScore = match->bottom - from->top;
		}
		else
		{
			matchScore = from->bottom - match->top;
		}

		if (contenderScore > matchScore)
		{
			return (1);
		}
	}
	else
	{
		// neither overlap
		DISPLAY_INT matchXDistance = from->left - match->right;
		DISPLAY_INT matchYDistance;

		if (match->bottom < from->top)
		{
			matchYDistance = from->top - match->bottom;
		}
		else
		{
			matchYDistance = match->top - from->bottom;
		}

		if (matchYDistance < yDistance && matchXDistance <= xDistance)
		{
			return (0);
		}

		if (matchYDistance > yDistance && matchXDistance >= xDistance)
		{
			return (1);
		}

		if (DISTANCE(matchXDistance, matchYDistance) > DISTANCE(xDistance, yDistance))
		{
			return (1);
		}

		return (0);
	}

	return (0);
}


class HTMLTabUpElementFinder : public HTMLElementFinder
{
private:
	HTMLElement *mpFrom;
	HTMLElement *mpBestMatch;
	HTMLElement *mpBestMatchChild;
	int miBestMatchDisplayIndex;
	WebRect bestRect;
	WebRect myDisplayRect;
	HTMLDisplayElementFinder f;

public:
	HTMLTabUpElementFinder(HTMLElement *elem, HTMLElement *focusBoxElement, int focusBoxIndex)
	{
		mpFrom = elem;
		if (focusBoxElement)
		{
			focusBoxElement->GetDocumentDisplayRect(&myDisplayRect, focusBoxIndex);
		}
		else
		{
			mpFrom->GetDocumentDisplayRect(&myDisplayRect, 0);
		}
		mpBestMatchChild = 0;
		miBestMatchDisplayIndex = 0;
		mpBestMatch = 0;

		DISPLAY_INT temp1, temp2;

		temp1 = myDisplayRect.left;
		temp2 = myDisplayRect.right;

		myDisplayRect.left   = myDisplayRect.top;
		myDisplayRect.right  = myDisplayRect.bottom;
		myDisplayRect.top    = -temp2;
		myDisplayRect.bottom = -temp1;
	}

	~HTMLTabUpElementFinder()
	{
	}

	WEBC_BOOL Check(HTMLElement *tabElement)
	{
		WebRect tabDisplayRect;

		if (mpFrom != tabElement && tabElement->TabIndex() != INVALID_TABINDEX)
		{
			HTMLElementTreeWalker w(tabElement, tabElement, &f, 0);
			int n;

			HTMLElement *tabChild = w.getNext();
			while (tabChild)
			{
				n = 0;
				while (tabChild->GetDocumentDisplayRect(&tabDisplayRect, n++) >= 0)
				{
					DISPLAY_INT temp1, temp2;

					temp1 = tabDisplayRect.left;
					temp2 = tabDisplayRect.right;

					tabDisplayRect.left   = tabDisplayRect.top;
					tabDisplayRect.right  = tabDisplayRect.bottom;
					tabDisplayRect.top    = -temp2;
					tabDisplayRect.bottom = -temp1;

					if (_BetterLeftMatch(mpBestMatch, &myDisplayRect, &bestRect, &tabDisplayRect))
					{
						bestRect = tabDisplayRect;
						mpBestMatchChild = tabChild;
						miBestMatchDisplayIndex = n-1;
						mpBestMatch = tabElement;
					}
				}

				tabChild = w.getNext();
			}
		}

		return (0);
	}

	HTMLElement *GetBestMatch (void)
	{
		return (mpBestMatch);
	}

	HTMLElement *GetBestMatchChild (void)
	{
		return (mpBestMatchChild);
	}

	int GetBestMatchDisplayIndex (void)
	{
		return (miBestMatchDisplayIndex);
	}
};

class HTMLTabDownElementFinder : public HTMLElementFinder
{
private:
	HTMLElement *mpFrom;
	HTMLElement *mpBestMatch;
	HTMLElement *mpBestMatchChild;
	int miBestMatchDisplayIndex;
	WebRect bestRect;
	WebRect myDisplayRect;
	HTMLDisplayElementFinder f;

public:
	HTMLTabDownElementFinder(HTMLElement *elem, HTMLElement *focusBoxElement, int focusBoxIndex)
	{
		mpFrom = elem;
		if (focusBoxElement)
		{
			focusBoxElement->GetDocumentDisplayRect(&myDisplayRect, focusBoxIndex);
		}
		else
		{
			mpFrom->GetDocumentDisplayRect(&myDisplayRect, 0);
		}
		mpBestMatchChild = 0;
		miBestMatchDisplayIndex = 0;
		mpBestMatch = 0;

		DISPLAY_INT temp1, temp2;

		temp1 = myDisplayRect.left;
		temp2 = myDisplayRect.right;

		myDisplayRect.left   = -myDisplayRect.bottom;
		myDisplayRect.right  = -myDisplayRect.top;
		myDisplayRect.top    = temp1;
		myDisplayRect.bottom = temp2;
	}

	~HTMLTabDownElementFinder()
	{
	}

	WEBC_BOOL Check(HTMLElement *tabElement)
	{
		WebRect tabDisplayRect;

		if (mpFrom != tabElement && tabElement->TabIndex() != INVALID_TABINDEX)
		{
			HTMLElementTreeWalker w(tabElement, tabElement, &f, 0);
			int n;

			HTMLElement *tabChild = w.getNext();
			while (tabChild)
			{
				n = 0;
				while (tabChild->GetDocumentDisplayRect(&tabDisplayRect, n++) >= 0)
				{
					DISPLAY_INT temp1, temp2;

					temp1 = tabDisplayRect.left;
					temp2 = tabDisplayRect.right;

					tabDisplayRect.left   = -tabDisplayRect.bottom;
					tabDisplayRect.right  = -tabDisplayRect.top;
					tabDisplayRect.top    = temp1;
					tabDisplayRect.bottom = temp2;

					if (_BetterLeftMatch(mpBestMatch, &myDisplayRect, &bestRect, &tabDisplayRect))
					{
						bestRect = tabDisplayRect;
						mpBestMatchChild = tabChild;
						miBestMatchDisplayIndex = n-1;
						mpBestMatch = tabElement;
					}
				}

				tabChild = w.getNext();
			}
		}

		return (0);
	}

	HTMLElement *GetBestMatch (void)
	{
		return (mpBestMatch);
	}

	HTMLElement *GetBestMatchChild (void)
	{
		return (mpBestMatchChild);
	}

	int GetBestMatchDisplayIndex (void)
	{
		return (miBestMatchDisplayIndex);
	}
};

class HTMLTabLeftElementFinder : public HTMLElementFinder
{
private:
	HTMLElement *mpFrom;
	HTMLElement *mpBestMatch;
	HTMLElement *mpBestMatchChild;
	int miBestMatchDisplayIndex;
	WebRect myDisplayRect;
	WebRect bestRect;
	HTMLDisplayElementFinder f;

public:
	HTMLTabLeftElementFinder(HTMLElement *elem, HTMLElement *focusBoxElement, int focusBoxIndex)
	{
		mpFrom = elem;
		if (focusBoxElement)
		{
			focusBoxElement->GetDocumentDisplayRect(&myDisplayRect, focusBoxIndex);
		}
		else
		{
			mpFrom->GetDocumentDisplayRect(&myDisplayRect, 0);
		}
		mpBestMatchChild = 0;
		miBestMatchDisplayIndex = 0;
		mpBestMatch = 0;
	}

	~HTMLTabLeftElementFinder()
	{
	}

	WEBC_BOOL Check(HTMLElement *tabElement)
	{
		WebRect tabDisplayRect;

		if (mpFrom != tabElement && tabElement->TabIndex() != INVALID_TABINDEX)
		{
			HTMLElementTreeWalker w(tabElement, tabElement, &f, 0);
			int n;

			HTMLElement *tabChild = w.getNext();
			while (tabChild)
			{
				n = 0;
				while (tabChild->GetDocumentDisplayRect(&tabDisplayRect, n++) >= 0)
				{
					if (_BetterLeftMatch(mpBestMatch, &myDisplayRect, &bestRect, &tabDisplayRect))
					{
						bestRect = tabDisplayRect;
						mpBestMatchChild = tabChild;
						miBestMatchDisplayIndex = n-1;
						mpBestMatch = tabElement;
					}
				}

				tabChild = w.getNext();
			}
		}

		return (0);
	}

	HTMLElement *GetBestMatch (void)
	{
		return (mpBestMatch);
	}

	HTMLElement *GetBestMatchChild (void)
	{
		return (mpBestMatchChild);
	}

	int GetBestMatchDisplayIndex (void)
	{
		return (miBestMatchDisplayIndex);
	}
};

class HTMLTabRightElementFinder : public HTMLElementFinder
{
private:
	HTMLElement *mpFrom;
	HTMLElement *mpBestMatch;
	HTMLElement *mpBestMatchChild;
	int miBestMatchDisplayIndex;
	WebRect myDisplayRect;
	WebRect bestRect;
	HTMLDisplayElementFinder f;

public:
	HTMLTabRightElementFinder(HTMLElement *elem, HTMLElement *focusBoxElement, int focusBoxIndex)
	{
		mpFrom = elem;
		if (focusBoxElement)
		{
			focusBoxElement->GetDocumentDisplayRect(&myDisplayRect, focusBoxIndex);
		}
		else
		{
			mpFrom->GetDocumentDisplayRect(&myDisplayRect, 0);
		}
		mpBestMatchChild = 0;
		miBestMatchDisplayIndex = 0;

		DISPLAY_INT temp = -myDisplayRect.left;
		myDisplayRect.left  = -myDisplayRect.right;
		myDisplayRect.right = temp;

		mpBestMatch = 0;
	}

	~HTMLTabRightElementFinder()
	{
	}

	WEBC_BOOL Check(HTMLElement *tabElement)
	{
		WebRect tabDisplayRect;

		if (mpFrom != tabElement && tabElement->TabIndex() != INVALID_TABINDEX)
		{
			HTMLElementTreeWalker w(tabElement, tabElement, &f, 0);
			int n;

			HTMLElement *tabChild = w.getNext();
			while (tabChild)
			{
				n = 0;
				while (tabChild->GetDocumentDisplayRect(&tabDisplayRect, n++) >= 0)
				{
					DISPLAY_INT temp = -tabDisplayRect.left;
					tabDisplayRect.left  = -tabDisplayRect.right;
					tabDisplayRect.right = temp;

					if (_BetterLeftMatch(mpBestMatch, &myDisplayRect, &bestRect, &tabDisplayRect))
					{
						bestRect = tabDisplayRect;
						mpBestMatchChild = tabChild;
						miBestMatchDisplayIndex = n-1;
						mpBestMatch = tabElement;
					}
				}

				tabChild = w.getNext();
			}
		}

		return (0);
	}

	HTMLElement *GetBestMatch (void)
	{
		return (mpBestMatch);
	}

	HTMLElement *GetBestMatchChild (void)
	{
		return (mpBestMatchChild);
	}

	int GetBestMatchDisplayIndex (void)
	{
		return (miBestMatchDisplayIndex);
	}
};

void HTMLElement::FocusTabUp(void)
{
	HTMLTabUpElementFinder f(this, mpFocusBoxElement, miFocusBoxIndex);

	if (GetDocument())
	{
		GetDocument()->FindElement(&f, 0);
		if (f.GetBestMatch())
		{
			f.GetBestMatch()->Focus(f.GetBestMatchChild(), f.GetBestMatchDisplayIndex());
		}
	}
}

void HTMLElement::FocusTabDown(void)
{
	HTMLTabDownElementFinder f(this, mpFocusBoxElement, miFocusBoxIndex);

	if (GetDocument())
	{
		GetDocument()->FindElement(&f, 0);
		if (f.GetBestMatch())
		{
			f.GetBestMatch()->Focus(f.GetBestMatchChild(), f.GetBestMatchDisplayIndex());
		}
	}
}

void HTMLElement::FocusTabLeft(void)
{
	HTMLTabLeftElementFinder f(this, mpFocusBoxElement, miFocusBoxIndex);

	if (GetDocument())
	{
		GetDocument()->FindElement(&f, 0);
		if (f.GetBestMatch())
		{
			f.GetBestMatch()->Focus(f.GetBestMatchChild(), f.GetBestMatchDisplayIndex());
		}
	}
}

void HTMLElement::FocusTabRight(void)
{
	HTMLTabRightElementFinder f(this, mpFocusBoxElement, miFocusBoxIndex);

	if (GetDocument())
	{
		GetDocument()->FindElement(&f, 0);
		if (f.GetBestMatch())
		{
			f.GetBestMatch()->Focus(f.GetBestMatchChild(), f.GetBestMatchDisplayIndex());
		}
	}
}

#endif // WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION
HTMLElement *HTMLElement::GetNextInTabOrder(WEBC_BOOL traverseFrames, WEBC_BOOL wrap)
{
HTMLDocument *pDoc = GetDocument();
HTMLElement *pFocusElement = 0;

	if (pDoc)
	{
		//get the next tab element
		HTMLNextTabFinder nextTab(this);
		pDoc->FindElement(&nextTab, 0 /* don't search frames */);
		pFocusElement = nextTab.Found();

		if (!pFocusElement)
		{
		  #if (WEBC_SUPPORT_FRAMES)
			if (traverseFrames)
			{
				HTMLFrame *pFrame = pDoc->GetFrame();
				HTMLDocument *root = pDoc->RootParent();
				HTMLDocument *startDoc = pDoc;

				// starting with this frame, go forwards through all frames until
				//  we find one with a non-negative tab-index element in it

				HTMLElementTypeFinder frameFinder (HTML_FRAME_ELEMENT);
				HTMLElementTypeFinder iframeFinder (HTML_IFRAME_ELEMENT);
				HTMLElementOrFinder frameOrIframeFinder (&frameFinder, &iframeFinder);

				do
				{
					// Get the next document
					do
					{
						if (pDoc == root)
						{
							pFrame = (HTMLFrame*) root->FindElement(&frameOrIframeFinder);
						}
						else
						{
							HTMLExactElementFinder currentElementFinder (pFrame);
							HTMLSequenceFinder nextFrameOrIframeFinder (&currentElementFinder, &frameOrIframeFinder);

							pFrame = (HTMLFrame*) root->FindElement(&nextFrameOrIframeFinder);
						}

						pDoc = (pFrame)? pFrame->FrameDocument() : root;
					}
					while (pFrame && (pFrame->GetUnitType() == TU_TYPE_NODISPLAY || pFrame->GetVisibility() == TU_VISIBILITY_HIDE));

					// Get the first tab element in this document
					HTMLFirstTabFinder firstTab;
					pDoc->FindElement(&firstTab, 0 /*don't search inside subframes*/);
					pFocusElement = firstTab.Found();

					if (!pFocusElement)
					{
						return pDoc->Body();
					}
				}
				while (!pFocusElement && pDoc != startDoc);
			}
			else
			{
		  #endif // WEBC_SUPPORT_FRAMES

				if (wrap)
				{
					HTMLFirstTabFinder firstTab;
					pDoc->FindElement(&firstTab);
					pFocusElement = firstTab.Found();
				}

		  #if (WEBC_SUPPORT_FRAMES)
			}
		  #endif // WEBC_SUPPORT_FRAMES
		}
	}

	return (pFocusElement);
}

HTMLElement *HTMLElement::GetPrevInTabOrder(WEBC_BOOL traverseFrames, WEBC_BOOL wrap)
{
HTMLDocument *pDoc = GetDocument();
HTMLElement *pFocusElement = 0;

	if (pDoc)
	{
		//get the prev tab element
		HTMLPrevTabFinder prevTab(this);
		pDoc->FindElement(&prevTab, 0 /* don't search frames */);
		pFocusElement = prevTab.Found();

		if (!pFocusElement)
		{
		  #if (WEBC_SUPPORT_FRAMES)
			if (traverseFrames)
			{
				HTMLFrame *pFrame = pDoc->GetFrame();
				HTMLDocument *root = pDoc->RootParent();
				HTMLDocument *startDoc = pDoc;

				// starting with this frame, go backwards through all frames until
				//  we find one with a non-negative tab-index element in it

				do
				{
					// Get the previous document
					do
					{
						HTMLElementTypeFinder frameFinder (HTML_FRAME_ELEMENT);
						HTMLElementTypeFinder iframeFinder (HTML_IFRAME_ELEMENT);
						HTMLElementOrFinder frameOrIframeFinder (&frameFinder, &iframeFinder);

						if (pDoc == root)
						{
							HTMLLastFinder lastFrame(&frameOrIframeFinder);
							root->FindElement(&lastFrame);
							pFrame = (HTMLFrame *) lastFrame.Found();
						}
						else
						{
							HTMLExactElementFinder exactFinder(pFrame);
							HTMLPrevFinder prevFrame(&frameOrIframeFinder, &exactFinder);
							root->FindElement(&prevFrame);
							pFrame = (prevFrame.Found())? ((HTMLFrame*) prevFrame.Found()) : 0;
						}

						pDoc = (pFrame)? pFrame->FrameDocument() : root;
					}
					while (pFrame && (pFrame->GetUnitType() == TU_TYPE_NODISPLAY || pFrame->GetVisibility() == TU_VISIBILITY_HIDE));

					// Get the last tab element in this document
					HTMLLastTabFinder lastTab;
					pDoc->FindElement(&lastTab, 0 /*don't search inside subframes*/);
					pFocusElement = lastTab.Found();

					if (!pFocusElement)
					{
						return pDoc->Body();
					}
				}
				while (!pFocusElement && pDoc != startDoc);
			}
			else
			{
		  #endif // WEBC_SUPPORT_FRAMES

				if (wrap)
				{
					HTMLLastTabFinder lastTab;
					pDoc->FindElement(&lastTab);
					pFocusElement = lastTab.Found();
				}

		  #if (WEBC_SUPPORT_FRAMES)
			}
		  #endif // WEBC_SUPPORT_FRAMES
		}
	}

	return (pFocusElement);
}


/*****************************************************************************/
// DefaultInputKeyHandler (HTMLEvent *pEvent)
//
// This method, callable from any HTMLElement, defines the default key
//  handling behavior for input-type elements, such as anchors, text areas,
//  and buttons, that typically participate in the tab order.
/*****************************************************************************/

HTMLEventStatus HTMLElement::DefaultInputKeyHandler(HTMLEvent *pEvent)
{
HTMLElement *pNewFocus = 0;
HTMLDocument *pMyDoc = GetDocument();

	switch (pEvent->data.key)
	{
		case WGK_F2:
		{
			ObjectReference<HTMLElement> self(this);

			Unfocus();

			if (!self.Get())
			{
				return (HTML_EVENT_STATUS_HALT);
			}

			if (!this->IsVisible())
			{
				if (pMyDoc)
				{
					//HTMLFirstVisibleTabFinder f;
					HTMLFirstTabFinder f;
					pMyDoc->FindElement(&f);
					pNewFocus = f.Found();
				}
			}

			if (!pNewFocus)
			{
				pNewFocus = GetNextInTabOrder();
			}

			if (pNewFocus)
			{
				pNewFocus->Focus();
			}

			while (!pNewFocus->IsVisible())
			{
				pNewFocus = pNewFocus->GetNextInTabOrder();
				if (pNewFocus)
				{
					pNewFocus->Focus();
					continue;
				}
				break;
			}

			return (HTML_EVENT_STATUS_DONE);
		}

		// go to next element in the tab order
		case WGK_F1:
		{
			ObjectReference<HTMLElement> self(this);

			Unfocus();

			if (!self.Get())
			{
				return (HTML_EVENT_STATUS_HALT);
			}

			if (!this->IsVisible())
			{
				if (pMyDoc)
				{
					//HTMLLastVisibleTabFinder f;
					HTMLLastTabFinder f;
					pMyDoc->FindElement(&f);
					pNewFocus = f.Found();
				}
			}

			if (!pNewFocus)
			{
				pNewFocus = GetPrevInTabOrder();
			}

			if (pNewFocus)
			{
				pNewFocus->Focus();
			}

			while (!pNewFocus->IsVisible())
			{
				pNewFocus = pNewFocus->GetPrevInTabOrder();
				if (pNewFocus)
				{
					pNewFocus->Focus();
					continue;
				}
				break;
			}

			return (HTML_EVENT_STATUS_DONE);
		}

	  #if (WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION)

		case WGK_LEFT:
			if (pEvent->flags & HTML_EVENT_FLAG_CTRL_DOWN)
			{
				ObjectReference<HTMLElement> self(this);

				Unfocus();

				if (!self.Get())
				{
					return (HTML_EVENT_STATUS_HALT);
				}

				HTMLTabLeftElementFinder f(this, mpFocusBoxElement, miFocusBoxIndex);

				if (pMyDoc)
				{
					pMyDoc->FindElement(&f, 0);
					if (f.GetBestMatch())
					{
						f.GetBestMatch()->Focus(f.GetBestMatchChild(), f.GetBestMatchDisplayIndex());
						return (HTML_EVENT_STATUS_DONE);
					}
				}

				Focus(this);

				return (HTML_EVENT_STATUS_DONE);
			}
			break;

		case WGK_RIGHT:
			if (pEvent->flags & HTML_EVENT_FLAG_CTRL_DOWN)
			{
				ObjectReference<HTMLElement> self(this);

				Unfocus();

				if (!self.Get())
				{
					return (HTML_EVENT_STATUS_HALT);
				}

				HTMLTabRightElementFinder f(this, mpFocusBoxElement, miFocusBoxIndex);

				if (pMyDoc)
				{
					pMyDoc->FindElement(&f, 0);
					if (f.GetBestMatch())
					{
						f.GetBestMatch()->Focus(f.GetBestMatchChild(), f.GetBestMatchDisplayIndex());
						return (HTML_EVENT_STATUS_DONE);
					}
				}

				Focus(this);

				return (HTML_EVENT_STATUS_DONE);
			}
			break;

		case WGK_LNUP:
			if (pEvent->flags & HTML_EVENT_FLAG_CTRL_DOWN)
			{
				ObjectReference<HTMLElement> self(this);

				Unfocus();

				if (!self.Get())
				{
					return (HTML_EVENT_STATUS_HALT);
				}

				HTMLTabUpElementFinder f(this, mpFocusBoxElement, miFocusBoxIndex);

				if (pMyDoc)
				{
					pMyDoc->FindElement(&f, 0);
					if (f.GetBestMatch())
					{
						f.GetBestMatch()->Focus(f.GetBestMatchChild(), f.GetBestMatchDisplayIndex());
						return (HTML_EVENT_STATUS_DONE);
					}
				}

				Focus(this);

				return (HTML_EVENT_STATUS_DONE);
			}
			break;

		case WGK_LNDN:
			if (pEvent->flags & HTML_EVENT_FLAG_CTRL_DOWN)
			{
				ObjectReference<HTMLElement> self(this);

				Unfocus();

				if (!self.Get())
				{
					return (HTML_EVENT_STATUS_HALT);
				}

				HTMLTabDownElementFinder f(this, mpFocusBoxElement, miFocusBoxIndex);

				if (pMyDoc)
				{
					pMyDoc->FindElement(&f, 0);
					if (f.GetBestMatch())
					{
						f.GetBestMatch()->Focus(f.GetBestMatchChild(), f.GetBestMatchDisplayIndex());
						return (HTML_EVENT_STATUS_DONE);
					}
				}

				Focus(this);

				return (HTML_EVENT_STATUS_DONE);
			}
			break;

	  #endif // WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION

		default:
			break;
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

CSSValueType HTMLElement::GetStyleFromCSS(CSSPropertyType Property, CSSPropertyValue *pValue)
{
  #if (WEBC_SUPPORT_STYLE_SHEETS)

	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETSTYLEFROMCSS);

	HTMLDocument * doc = GetDocument();
	if (doc)
	{
		CSSValueType result = doc->GetCSSProperty(this, Property, pValue);

		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETSTYLEFROMCSS);

		return (result);
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETSTYLEFROMCSS);

  #endif // WEBC_SUPPORT_STYLE_SHEETS

	return (CSS_VALUE_UNKNOWN);
}

int HTMLElement::IsVisible()
{
	HTMLDocument *pDoc = GetDocument();
	if (pDoc)
	{
		DisplayManager *pDisplayManager = pDoc->GetDisplayManager();
		if (pDisplayManager)
		{
			DisplayElement *pDisplayElement = GetFirstDisplayChild();
			if (pDisplayElement)
			{
				return (pDisplayManager->IsVisible(pDisplayElement));
			}
		}
	}

	return (0);
}


/*****************************************************************************/
// support classes for HTMLElement
/*****************************************************************************/

/*****************************************************************************/
// class HTMLElementClassEnumerator : public Enumerator
/*****************************************************************************/

const int HTMLElementClassEnumerator::MAX_POOL_SIZE = 10;
int HTMLElementClassEnumerator::miPoolSize = 0;
HTMLElementClassEnumerator *HTMLElementClassEnumerator::mpPool = 0;

HTMLElementClassEnumerator::HTMLElementClassEnumerator (vector *classVector)
{
	Init(classVector);
}

void HTMLElementClassEnumerator::Init (vector *classVector)
{
	const WEBC_CHAR **ptr = (const WEBC_CHAR **) vector_get_first(classVector, &mIterator);
	mpNext = (ptr)? *ptr : 0;
}

unsigned char * HTMLElementClassEnumerator::Next(void)
{
	unsigned char *pCurrent = (unsigned char *) mpNext;

	if (mpNext)
	{
		const WEBC_CHAR **ptr = (const WEBC_CHAR **) vector_get_next(&mIterator);
		mpNext = (ptr)? *ptr : 0;
	}

	return (pCurrent);
}

HTMLElementClassEnumerator *HTMLElementClassEnumerator::Alloc(vector *classVector, char *file, long line)
{
HTMLElementClassEnumerator *pNew;

	if (mpPool)
	{
		// if there is one in the pool, grab that one
		pNew = mpPool;
		mpPool = mpPool->mpPoolNext;
		pNew->Init(classVector);
		miPoolSize--;
	}
	else
	{
		WEBC_DEBUG_NEW(pNew, HTMLElementClassEnumerator(classVector), file, line,"HTMLElementClassEnumerator::Alloc");
	}

	return (pNew);
}

void HTMLElementClassEnumerator::Dispose (void)
{
	if (miPoolSize >= MAX_POOL_SIZE)
	{
		WEBC_DELETE(this);
	}
	else
	{
		miPoolSize++;
		mpPoolNext = mpPool;
		mpPool = this;
	}
}

void HTMLElementClassEnumerator::CleanUp (void)
{
	HTMLElementClassEnumerator *nextEnum;

	while (mpPool)
	{
		nextEnum = mpPool->mpPoolNext;
		WEBC_DELETE(mpPool);
		mpPool = nextEnum;
	}
}


DisplayElement *HTMLElement::GetDisplayElement (WEBC_BOOL create)
{
	return (0);
}

TextUnit *HTMLElement::GetTextUnit (void)
{
	return (this);
}

void HTMLElement::SetParentDisplay(DisplayElement* parent, DisplayElement* insertBefore)
{
}

void HTMLElement::SetDisplayContext(DisplayElement *parent)
{
}

WebFont HTMLElement::GetWebFont (void)
{
	WebGraphFont *font = GetFont();
	return (font? font->mFont : 0);
}

WebGraphFont* HTMLElement::GetFont (void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETFONT);

	if (!(mFlags & HELEM_FLAG_FONT_CACHED))
	{
		GetFontFamilyDesc();
		GetFontSizeDesc();
		GetFontStyleDesc();
		GetFontWeightDesc();

		mFont.Set(GetWebGraphFont(mFontDesc));

		//SetFlag(HELEM_FLAG_FONT_CACHED);
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETFONT);

	return (mFont.Get());
}

const WEBC_CHAR * HTMLElement::GetFontFamily (void)
{
CSSPropertyValue family;

	if (GetFontProperty(GetFontFamilyDesc(), CSS_PROPERTY_FONT_FAMILY, &family) >= 0)
	{
		return (family.name);
	}

	return (0);
}

void HTMLElement::GetFontSize (CSSPropertyValue *size)
{
	GetFontProperty(GetFontSizeDesc(), CSS_PROPERTY_FONT_SIZE, size);
}

CSSFontWeight HTMLElement::GetFontWeight (void)
{
CSSPropertyValue weight;

	if (GetFontProperty(GetFontWeightDesc(), CSS_PROPERTY_FONT_WEIGHT, &weight) >= 0)
	{
		return (weight.fontWeight);
	}

	return (CSS_FONT_WEIGHT_NORMAL);
}

CSSFontStyle HTMLElement::GetFontStyle (void)
{
CSSPropertyValue style;

	if (GetFontProperty(GetFontStyleDesc(), CSS_PROPERTY_FONT_STYLE, &style) >= 0)
	{
		return (style.fontStyle);
	}

	return (CSS_FONT_STYLE_NORMAL);
}

/******************************************************************************
* void HTMLElement::GetFontFamilyDesc() -
*
* Parameters
*    none
*
* Description
*
* Returns (WebcFontDescriptor)
*
******************************************************************************/
WebcFontDescriptor HTMLElement::GetFontFamilyDesc (void)
{
	if (!(mFlags & HELEM_FLAG_FONT_FAMILY_CACHED))
	{
		CSSPropertyValue value;

		if ((GetStyleFromCSS(CSS_PROPERTY_FONT_FAMILY, &value) == CSS_VALUE_SPECIFIED) ||
			(GetStyleFromHTML(CSS_PROPERTY_FONT_FAMILY, &value) == CSS_VALUE_SPECIFIED))
		{
			mFontDesc = GetFontDescriptor(&value, 0, 0, 0, 0, mFontDesc);
		}
		else
		{
			WEBC_COPY_FONT_FAMILY(mFontDesc, (mpParent)? mpParent->GetFontFamilyDesc() : 0);
		}

		//mFlags |= HELEM_FLAG_FONT_FAMILY_CACHED;
	}

	return (mFontDesc);
}

/******************************************************************************
* void HTMLElement::GetFontSizeDesc() -
*
* Parameters
*    none
*
* Description
*
* Returns (WebcFontDescriptor)
*
******************************************************************************/

WebcFontDescriptor HTMLElement::GetFontSizeDesc (void)
{
	if (!(mFlags & HELEM_FLAG_FONT_SIZE_CACHED))
	{
		CSSPropertyValue value;

		if ((GetStyleFromCSS(CSS_PROPERTY_FONT_SIZE, &value) == CSS_VALUE_SPECIFIED) ||
		    (GetStyleFromHTML(CSS_PROPERTY_FONT_SIZE, &value) == CSS_VALUE_SPECIFIED))
		{
			if ((value.fontSize.type == CSS_FONT_SIZE_RELATIVE) ||
				(value.fontSize.type == CSS_FONT_SIZE_LENGTH &&
				 (value.fontSize.val.length.type == CSS_LENGTH_UNIT_PERCENT ||
				  value.fontSize.val.length.type == CSS_LENGTH_UNIT_EM ||
  				  value.fontSize.val.length.type == CSS_LENGTH_UNIT_PX ||
				  value.fontSize.val.length.type == CSS_LENGTH_UNIT_EX)))
			{
				if (mpParent)
				{
					WEBC_COPY_FONT_SIZE(mFontDesc, mpParent->GetFontSizeDesc());
				}
				else
				{
					int ptSize = CSS_FontSizeAbsoluteToPoints(CSS_FONT_SIZE_MEDIUM);
					WEBC_SET_FONT_SIZE(mFontDesc, ptSize);
				}
			}

			mFontDesc = GetFontDescriptor(0, &value, 0, 0, 0, mFontDesc);
		}
		else
		{
			WEBC_COPY_FONT_SIZE(mFontDesc, (mpParent)? mpParent->GetFontSizeDesc() : 0);
		}

		//mFlags |= HELEM_FLAG_FONT_SIZE_CACHED;
	}

	return (mFontDesc);
}

/******************************************************************************
* void HTMLElement::GetFontWeightDesc() -
*
* Parameters
*    none
*
* Description
*
* Returns (WebcFontDescriptor)
*
******************************************************************************/
WebcFontDescriptor HTMLElement::GetFontWeightDesc (void)
{
	if (!(mFlags & HELEM_FLAG_FONT_WEIGHT_CACHED))
	{
		CSSPropertyValue value;

		if ((GetStyleFromCSS(CSS_PROPERTY_FONT_WEIGHT, &value) == CSS_VALUE_SPECIFIED) ||
		    (GetStyleFromHTML(CSS_PROPERTY_FONT_WEIGHT, &value) == CSS_VALUE_SPECIFIED))
		{
			mFontDesc = GetFontDescriptor(0, 0, 0, &value, 0, mFontDesc);
		}
		else
		{
			WEBC_COPY_FONT_WEIGHT(mFontDesc, (mpParent)? mpParent->GetFontWeightDesc() : 0);
		}

		//mFlags |= HELEM_FLAG_FONT_WEIGHT_CACHED;
	}

	return (mFontDesc);
}

/******************************************************************************
* void HTMLElement::GetFontStyleDesc() -
*
* Parameters
*    none
*
* Description
*
* Returns (WebcFontDescriptor)
*
******************************************************************************/
WebcFontDescriptor HTMLElement::GetFontStyleDesc (void)
{
	if (!(mFlags & HELEM_FLAG_FONT_STYLE_CACHED))
	{
		CSSPropertyValue value;

		if ((GetStyleFromCSS(CSS_PROPERTY_FONT_STYLE, &value) == CSS_VALUE_SPECIFIED) ||
		    (GetStyleFromHTML(CSS_PROPERTY_FONT_STYLE, &value) == CSS_VALUE_SPECIFIED))
		{
			mFontDesc = GetFontDescriptor(0, 0, &value, 0, 0, mFontDesc);
		}
		else
		{
			WEBC_COPY_FONT_STYLE(mFontDesc, (mpParent)? mpParent->GetFontStyleDesc() : 0);
		}

		//mFlags |= HELEM_FLAG_FONT_STYLE_CACHED;
	}

	return (mFontDesc);
}

/******************************************************************************
* DISPLAY_INT HTMLElement::GetFontHeight() - return font height in pixels
*
* Parameters
*    none
*
* Description
*
* Returns (DISPLAY_INT)
*    the font height in pixels
******************************************************************************/
DISPLAY_INT HTMLElement::GetFontHeight (void)
{
	return (CSS_LengthPointsToPixels(WEBC_FONT_SIZE(GetFontSizeDesc())));
}

/******************************************************************************
* void HTMLElement::GetColor() - get color for text within this
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

HTMLColor HTMLElement::GetColor (void)
{
	CSSPropertyValue value;

	CSSValueType result = GetStyleFromCSS(CSS_PROPERTY_COLOR, &value);

	if (result == CSS_VALUE_UNKNOWN)
	{
		result = GetStyleFromHTML(CSS_PROPERTY_COLOR, &value);
	}

	switch (result)
	{
	case CSS_VALUE_SPECIFIED:
		return (value.color);

	case CSS_VALUE_INHERIT:
		return ((mpParent)? mpParent->GetColor() : HTML_RGBAToColor(0,0,0,0xff));

	default:
		if (mpParent)
		{
			return (mpParent->GetColor());
		}
		else
		{
			return (HTML_RGBAToColor(0,0,0,0xff));
		}
	}

}


/******************************************************************************
* html_color* HTMLElement::BackgroundColor() - Gets the background color
*
* Parameters
*    none
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/

HTMLColor HTMLElement::GetBackgroundColor (void)
{
	CSSPropertyValue value;

	if ((GetStyleFromCSS(CSS_PROPERTY_BACKGROUND_COLOR, &value) == CSS_VALUE_SPECIFIED) ||
		(GetStyleFromHTML(CSS_PROPERTY_BACKGROUND_COLOR, &value) == CSS_VALUE_SPECIFIED))
	{
		return (value.color);
	}
	else
	{
		// set to transparent
		return (HTML_RGBAToColor(0,0,0,0xff));
	}
}


/******************************************************************************
* CSSListStyleType HTMLElement::GetListStyleType() -
*
* Parameters
*    none
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/

CSSListStyleType HTMLElement::GetListStyleType(void)
{
CSSPropertyValue value;

	if ((GetStyleFromCSS(CSS_PROPERTY_LIST_STYLE_TYPE, &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(CSS_PROPERTY_LIST_STYLE_TYPE, &value) == CSS_VALUE_SPECIFIED))
	{
		return (value.listStyleType);
	}

	// default is disc
	return (CSS_LIST_STYLE_DISC);
}


/******************************************************************************
* CSSListStylePosition HTMLElement::GetListStylePosition() -
*
* Parameters
*    none
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/

CSSListStylePosition HTMLElement::GetListStylePosition(void)
{
CSSPropertyValue value;

	if ((GetStyleFromCSS(CSS_PROPERTY_LIST_STYLE_POSITION, &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(CSS_PROPERTY_LIST_STYLE_POSITION, &value) == CSS_VALUE_SPECIFIED))
	{
		return (value.listStylePosition);
	}

	// default is inside
	return (CSS_LIST_STYLE_POSITION_INSIDE);
}


/******************************************************************************
* CSSTextDecoration HTMLElement::GetTextDecoration() -
*
* Parameters
*    none
*
* Description
*
* Returns (CSSTextDecoration)
*
******************************************************************************/
CSSTextDecoration HTMLElement::GetTextDecoration (void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETTEXTDECORATION);

	CSSPropertyValue value;

	if ((GetStyleFromCSS(CSS_PROPERTY_TEXT_DECORATION, &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(CSS_PROPERTY_TEXT_DECORATION, &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromParent(CSS_PROPERTY_TEXT_DECORATION, &value) == CSS_VALUE_SPECIFIED))
	{
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETTEXTDECORATION);
		return (value.textDecoration);
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETTEXTDECORATION);
	return (CSS_TEXT_DECORATION_NONE);
}

/******************************************************************************
* HTMLElement::GetWidth() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
void HTMLElement::GetWidth (CSSLength *width)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETWIDTH);

	CSSPropertyValue value;

	if (GetStyleFromCSS(CSS_PROPERTY_WIDTH, &value) != CSS_VALUE_SPECIFIED)
	{
		if (GetStyleFromHTML(CSS_PROPERTY_WIDTH, &value) != CSS_VALUE_SPECIFIED)
		{
			width->type = CSS_LENGTH_UNIT_AUTO;
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETWIDTH);
			return;
		}
	}
	*width = value.length;

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETWIDTH);
}

/******************************************************************************
* HTMLElement::GetHeight() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
void HTMLElement::GetHeight (CSSLength *height)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENT_GETHEIGHT);

	CSSPropertyValue value;

	if (GetStyleFromCSS(CSS_PROPERTY_HEIGHT, &value) != CSS_VALUE_SPECIFIED)
	{
		if (GetStyleFromHTML(CSS_PROPERTY_HEIGHT, &value) != CSS_VALUE_SPECIFIED)
		{
			height->type = CSS_LENGTH_UNIT_AUTO;
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETHEIGHT);
			return;
		}
	}

	*height = value.length;

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENT_GETHEIGHT);
}

/******************************************************************************
* HTMLElement::GetHeight() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
void HTMLElement::GetLengthProperty (CSSPropertyType type, CSSLength *len)
{
CSSPropertyValue value;

	if (GetStyleFromCSS(type, &value) != CSS_VALUE_SPECIFIED)
	{
		if (GetStyleFromHTML(type, &value) != CSS_VALUE_SPECIFIED)
		{
			len->type = CSS_LENGTH_UNIT_AUTO;
			return;
		}
	}

	*len = value.length;
}

/******************************************************************************
* HTMLElement::GetBorderColor() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
HTMLColor HTMLElement::GetBorderColor(CSSSide side)
{
CSSPropertyValue value;
static WEBC_READONLY CSSPropertyType _p[] = {
	CSS_PROPERTY_BORDER_TOP_COLOR,
	CSS_PROPERTY_BORDER_RIGHT_COLOR,
	CSS_PROPERTY_BORDER_BOTTOM_COLOR,
	CSS_PROPERTY_BORDER_LEFT_COLOR
	};

	if ((GetStyleFromCSS(_p[(int) side], &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(_p[(int) side], &value) == CSS_VALUE_SPECIFIED))
	{
		return (value.color);
	}

	// default color is black
	return (HTML_RGBAToColor(0,0,0,0));
}

/******************************************************************************
* HTMLElement::GetBorderStyle() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
CSSBorderStyle HTMLElement::GetBorderStyle(CSSSide side)
{
CSSPropertyValue value;
static WEBC_READONLY CSSPropertyType _p[] = {
	CSS_PROPERTY_BORDER_TOP_STYLE,
	CSS_PROPERTY_BORDER_RIGHT_STYLE,
	CSS_PROPERTY_BORDER_BOTTOM_STYLE,
	CSS_PROPERTY_BORDER_LEFT_STYLE
	};

	if ((GetStyleFromCSS(_p[(int) side], &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(_p[(int) side], &value) == CSS_VALUE_SPECIFIED))
	{
		return (value.borderStyle);
	}

	return (CSS_BORDER_STYLE_NONE);
}

/******************************************************************************
* HTMLElement::GetBorderWidth() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
void HTMLElement::GetBorderWidth(CSSSide side, CSSBorderThickness *width)
{
CSSPropertyValue value;
static WEBC_READONLY CSSPropertyType _p[] = {
	CSS_PROPERTY_BORDER_TOP_WIDTH,
	CSS_PROPERTY_BORDER_RIGHT_WIDTH,
	CSS_PROPERTY_BORDER_BOTTOM_WIDTH,
	CSS_PROPERTY_BORDER_LEFT_WIDTH
	};

	if ((GetStyleFromCSS(_p[(int) side], &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(_p[(int) side], &value) == CSS_VALUE_SPECIFIED))
	{
		*width = value.borderThickness;
		return;
	}

	// default width is 7 pixels ?
	width->type = CSS_BORDER_THICKNESS_ABSOLUTE;
	width->val.absolute = CSS_BORDER_THICKNESS_MEDIUM;
}


/******************************************************************************
* HTMLElement::GetOutlineColor() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
HTMLColor HTMLElement::GetOutlineColor (void)
{
	CSSPropertyValue value;
	CSSValueType result = GetStyleFromCSS(CSS_PROPERTY_OUTLINE_COLOR, &value);

	if (result == CSS_VALUE_UNKNOWN)
	{
		result = GetStyleFromHTML(CSS_PROPERTY_OUTLINE_COLOR, &value);
	}

	switch (result)
	{
		case CSS_VALUE_SPECIFIED:
			return (value.color);

		case CSS_VALUE_INHERIT:
			if (mpParent)
			{
				return mpParent->GetOutlineColor();
			}
			// intentional fall-thru
		default:
			// transparent
			return (HTML_RGBAToColor(0,0,0,0xff));
	}
}


/******************************************************************************
* HTMLElement::GetOutlineStyle() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
CSSBorderStyle HTMLElement::GetOutlineStyle(void)
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
			break;
	}

	return (CSS_BORDER_STYLE_NONE);
}


/******************************************************************************
* HTMLElement::GetOutlineWidth() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
void HTMLElement::GetOutlineWidth(CSSBorderThickness* width)
{
	CSSPropertyValue value;
	CSSValueType result = GetStyleFromCSS(CSS_PROPERTY_OUTLINE_WIDTH, &value);

	if (result == CSS_VALUE_UNKNOWN)
	{
		result = GetStyleFromHTML(CSS_PROPERTY_OUTLINE_STYLE, &value);
	}

	switch (result)
	{
		case CSS_VALUE_SPECIFIED:
			*width = value.borderThickness;
			break;

		case CSS_VALUE_INHERIT:
			if (mpParent)
			{
				mpParent->GetOutlineWidth(width);
			}
			break;

		default:
			width->type = CSS_BORDER_THICKNESS_ABSOLUTE;
			width->val.absolute = CSS_BORDER_THICKNESS_THIN;
			break;
	}
}


/******************************************************************************
* HTMLElement::GetMarginWidth() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
void HTMLElement::GetMarginWidth(CSSSide side, CSSLength* width)
{
CSSPropertyValue value;
static WEBC_READONLY CSSPropertyType _p[] = {
		CSS_PROPERTY_MARGIN_TOP,
		CSS_PROPERTY_MARGIN_RIGHT,
		CSS_PROPERTY_MARGIN_BOTTOM,
		CSS_PROPERTY_MARGIN_LEFT
	};

	if ((GetStyleFromCSS(_p[(int) side], &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(_p[(int) side], &value) == CSS_VALUE_SPECIFIED))
	{
		*width = value.length;
		return;
	}

	// default width is 0 pixels
	width->type = CSS_LENGTH_UNIT_PX;
	width->val.dec = 0;
	width->val.frac = 0;
}

/******************************************************************************
* HTMLElement::GetTopMargin() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT HTMLElement::GetTopMargin(void)
{
	CSSLength length;
	GetMarginWidth(CSS_TOP, &length);
	return CSS_LengthToPixels(&length, 0, 0, 0);
}

/******************************************************************************
* HTMLElement::GetLeftMargin() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT HTMLElement::GetLeftMargin(void)
{
	CSSLength length;
	GetMarginWidth(CSS_LEFT, &length);
	return CSS_LengthToPixels(&length, 0, 0, 0);
}

/******************************************************************************
* HTMLElement::GetBottomMargin() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT HTMLElement::GetBottomMargin(void)
{
	CSSLength length;
	GetMarginWidth(CSS_BOTTOM, &length);
	return CSS_LengthToPixels(&length, 0, 0, 0);
}

/******************************************************************************
* HTMLElement::GetRightMargin() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT HTMLElement::GetRightMargin(void)
{
	CSSLength length;
	GetMarginWidth(CSS_RIGHT, &length);
	return CSS_LengthToPixels(&length, 0, 0, 0);
}

/******************************************************************************
* HTMLElement::GetPaddingWidth() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
void HTMLElement::GetPaddingWidth(CSSSide side, CSSLength* width)
{
CSSPropertyValue value;
static WEBC_READONLY CSSPropertyType _p[] = {
	CSS_PROPERTY_PADDING_TOP,
	CSS_PROPERTY_PADDING_RIGHT,
	CSS_PROPERTY_PADDING_BOTTOM,
	CSS_PROPERTY_PADDING_LEFT
	};

	if ((GetStyleFromCSS(_p[(int) side], &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(_p[(int) side], &value) == CSS_VALUE_SPECIFIED))
	{
		*width = value.length;
		return;
	}

	// default width is 0 pixels
	width->type = CSS_LENGTH_UNIT_PX;
	width->val.dec = 0;
	width->val.frac = 0;
}


/******************************************************************************
* void HTMLElement::NotifyDisplay() -
*
* Parameters
*    none
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/
void HTMLElement::NotifyDisplay (NotifyEventType event)
{
	DisplayElement *display = GetDisplayElement(WEBC_FALSE);

	if (display)
	{
		NotifyEvent what(NOTIFIER_TYPE_HTML_ELEMENT, event);
		display->Notify((HTMLElement*)this, &what);
	}
	else
	{
		display = GetDisplayParent();
		if (display)
		{
			NotifyEvent what(NOTIFIER_TYPE_NODISPLAY_HTML_CHILD, event);
			display->Notify((HTMLElement*)this, &what);
		}
	}
}


/******************************************************************************
* DisplayElement * HTMLElement::GetDisplayParent() -
*
* Parameters
*    none
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/
DisplayElement *HTMLElement::GetDisplayParent (void)
{
	// find the closest parent with a display element
	HTMLElement *parent = mpParent;
	DisplayElement *parentDisplay;
	while (parent)
	{
		parentDisplay = parent->GetDisplayElement(WEBC_FALSE);
		if (parentDisplay)
		{
			return (parentDisplay);
		}
		parent = parent->mpParent;
	}

	return (0);
}


/******************************************************************************
* void HTMLElement::GetFirstDisplayChild() -
*
* Parameters
*    none
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/
DisplayElement *HTMLElement::GetFirstDisplayChild (void)
{
DisplayElement *e = GetDisplayElement(WEBC_FALSE);

	if (!e)
	{
		HTMLElement *child = FirstChild();
		while (child)
		{
			e = child->GetFirstDisplayChild();
			if (e)
			{
				return e;
			}
			child = child->mpNext;
		}
	}

	return e;
}

/******************************************************************************
* void HTMLElement::Refresh() -
*
* Parameters
*    none
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/
void HTMLElement::Refresh(WEBC_BOOL draw)
{
DisplayElement *display = GetDisplayElement(WEBC_FALSE);

	if (display)
	{
		display->Invalidate();
	}
	else
	{
		for (HTMLElement *child = FirstChild(); child; child = child->mpNext)
		{
			child->Refresh(WEBC_FALSE);
		}
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

/******************************************************************************
* void HTMLElement::Focus() - Set the input focus to this element
*
* Parameters
*    none
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/

#if (WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION)
void HTMLElement::Focus (HTMLElement *focusBoxElement, int focusBoxIndex)
#else
void HTMLElement::Focus (void)
#endif
{
  #if (WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION)
	mpFocusBoxElement = focusBoxElement;
	miFocusBoxIndex = focusBoxIndex;
  #endif

	HTMLDocument* doc = GetDocument();
	HTMLBody* body = (doc)? doc->Body() : 0;
	HTMLBodyDisplay* bodyDisplay = (body)? ((HTMLBodyDisplay*) body->GetDisplayElement(WEBC_FALSE)) : 0;

	if (doc)
	{
		doc->QueueFocus(0);
	}

	if (!bodyDisplay || bodyDisplay->NeedsFormat())
	{
		doc->QueueFocus(this);
		return;
	}

	ObjectReference<HTMLElement> self(this);
	DisplayElement* display = GetDisplayElement(WEBC_FALSE);

	if (display && display->GetManager())
	{
		display->GetManager()->SetFocus(display);

		if (self.Get() && Type() != HTML_BODY_ELEMENT)
		{
			display = GetDisplayElement(WEBC_FALSE);
		}
		else
		{
			display = 0;
		}
	}
	else
	{
		bodyDisplay->SetHtmlFocus(this);

		if (self.Get())
		{
			HTMLDisplayElementFinder f;
			HTMLElement* firstVisibleChild = FindElement(&f);
			if (firstVisibleChild)
			{
				display = firstVisibleChild->GetDisplayElement(WEBC_FALSE);
			}
		}
		else
		{
			display = 0;
		}
	}

	if (display && display->GetManager())
	{
		display->GetManager()->EnsureVisible(display);
	}
}

/******************************************************************************
* void HTMLElement::Unfocus() - Remove the input focus from this element
*
* Parameters
*    none
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/

void HTMLElement::Unfocus (void)
{
DisplayElement *display = GetDisplayElement(WEBC_FALSE);

	if (display && display->GetManager())
	{
		display->GetManager()->RemoveFocus(display);
	}
	else
	{
		HTMLDocument *doc = GetDocument();
		HTMLBody *body = (doc)? doc->Body() : 0;
		HTMLBodyDisplay *bodyDisplay = (body)? (HTMLBodyDisplay *) body->GetDisplayElement(WEBC_FALSE) : 0;

		if (bodyDisplay)
		{
			bodyDisplay->RemoveHtmlFocus(this);
		}
	}
}

/******************************************************************************
* int HTMLElement::GetDocumentPosition() - Get document-relative position of
*	this element's DisplayElement, if it has one
*
* Parameters
*    px - pointer to DISPLAY_INT to store x-value; MUST be non-NULL
*    py - pointer to DISPLAY_INT to store y-value: MUST be non-NULL
*
* Description
*
* Returns (int)
*    0 if successful, -1 otherwise (no DisplayElement)
******************************************************************************/

int HTMLElement::GetDocumentPosition (DISPLAY_INT *px, DISPLAY_INT *py)
{
	DisplayElement *pDisplay = GetFirstDisplayChild();
	if (pDisplay)
	{
		if (pDisplay->mpParent)
		{
			pDisplay->mpParent->GetDisplayPosition(pDisplay, px, py);
		}
		else
		{
			*px = 0;
			*py = 0;
		}
		return (0);
	}
	else
	{
		return (-1);
	}
}

/******************************************************************************
* int HTMLElement::GetDocumentDisplayRect() -
*
* Parameters
*
* Description
*
* Returns (int)
*    0 if successful, -1 otherwise (no DisplayElement)
******************************************************************************/

int HTMLElement::GetDocumentDisplayRect (WebRect *rect, int n)
{
DisplayElement *display = GetDisplayElement(WEBC_FALSE);

	if (n > 0)
	{
		return (-1);
	}

	if (display)
	{
		rect->Set(&display->mRect);

		if (display->mpParent)
		{
			DISPLAY_INT x, y;

			display->mpParent->GetDisplayPosition(display, &x, &y);
			rect->MoveTo(x,y);
		}

		return (0);
	}
	else
	{
		HTMLElement *child = FirstChild();
		while (child)
		{
			if (child->GetDocumentDisplayRect(rect, 0) >= 0)
			{
				return (0);
			}

			child = child->mpNext;
		}
	}

	return (-1);
}


/******************************************************************************
* int HTMLElement::SetDocumentDisplayRect() -
*
* Parameters
*
* Description
*
* Returns (int)
*    0 if successful, -1 otherwise (no DisplayElement)
******************************************************************************/

int HTMLElement::SetDocumentDisplayRect (WebRect *rect)
{
	DisplayElement *display = GetDisplayElement(WEBC_FALSE);

	if (display)
	{
		display->mRect.Set(rect);
		//AK_TODO more here? need to offset by parent coordinates?

		return (0);
	}
	else
	{
		HTMLElement *child = FirstChild();
		while (child)
		{
			if (child->SetDocumentDisplayRect(rect) >= 0)
			{
				return (0);
			}

			child = child->mpNext;
		}
	}

	return (-1);
}

/******************************************************************************
* WEBC_BOOL HTMLElement::GetVisibleFromDisplay (void) - Test if "display: none;"
* is true.
*
* Parameters
*
* Description
*
* Returns (WEBC_BOOL)
* 	WEBC_TRUE if display: none; does not apply to the element.
* 	If style sheets are not available always retuirns WEBC_TRUE
******************************************************************************/
WEBC_BOOL HTMLElement::GetVisibleFromDisplay (void)
{
	WEBC_BOOL visible = true;
  #if (WEBC_SUPPORT_STYLE_SHEETS)
	CSSPropertyValue value;
	if ((GetStyleFromCSS(CSS_PROPERTY_DISPLAY, &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(CSS_PROPERTY_DISPLAY, &value) == CSS_VALUE_SPECIFIED))
	{
		visible = (WEBC_BOOL) (value.display!=CSS_DISPLAY_NONE);
	}
  #endif
	return(visible);
}

/******************************************************************************
* HTMLElement::GetClientHeight() - Get client height (meaning is specific
*   to each DisplayElement type) of associated DisplayElement, if present
*
* Parameters
*
* Description
*
* Returns (DISPLAY_INT)
*    client height or 0 if no DisplayElement
******************************************************************************/

// Note: jQuery calls elem.offsetHeight
// which eventually gets here. This and height should return zero
// if it is hidden. But.. if we do that it fails.
// If we enable the patches things don;t work.
// With he patches disabled jQery's function
// Named Sizzle.selectors.filters.hidden must be changed
// To test for display == none;

DISPLAY_INT HTMLElement::GetClientHeight (void)
{
	DisplayElement *pDisplay = GetDisplayElement(WEBC_FALSE);
	if (pDisplay)
	{
		WebRect rect;
		pDisplay->GetClientRect(&rect);
		return (rect.Height());
	}
	return (0);
}


/******************************************************************************
* HTMLElement::GetClientWidth() - Get client width (meaning is specific
*   to each DisplayElement type) of associated DisplayElement, if present
*
* Parameters
*
* Description
*
* Returns (DISPLAY_INT)
*    client width or 0 if no DisplayElement
******************************************************************************/

DISPLAY_INT HTMLElement::GetClientWidth (void)
{
	DisplayElement *pDisplay = GetDisplayElement(WEBC_FALSE);
	if (pDisplay)
	{
		WebRect rect;
		pDisplay->GetClientRect(&rect);
		return (rect.Width());
	}
	return (0);
}


/******************************************************************************
* HTMLElement::GetClientLeft() - Get client left (meaning is specific
*   to each DisplayElement type) of associated DisplayElement, if present
*
* Parameters
*
* Description
*
* Returns (DISPLAY_INT)
*    client left or 0 if no DisplayElement
******************************************************************************/

DISPLAY_INT HTMLElement::GetClientLeft (void)
{
	DisplayElement *pDisplay = GetDisplayElement(WEBC_FALSE);
	if (pDisplay)
	{
		WebRect rect;
		pDisplay->GetClientRect(&rect);
		return (rect.left);
	}
	return (0);
}


/******************************************************************************
* HTMLElement::GetClientTop() - Get client top (meaning is specific
*   to each DisplayElement type) of associated DisplayElement, if present
*
* Parameters
*
* Description
*
* Returns (DISPLAY_INT)
*    client top or 0 if no DisplayElement
******************************************************************************/

DISPLAY_INT HTMLElement::GetClientTop (void)
{
	DisplayElement *pDisplay = GetDisplayElement(WEBC_FALSE);
	if (pDisplay)
	{
		WebRect rect;
		pDisplay->GetClientRect(&rect);
		return (rect.top);
	}
	return (0);
}


/******************************************************************************
* HTMLElement::CreateJSObject() - Create a JSObject for this element if it
*   does not already have one
*
* Parameters
*
* Description
*
* Returns (JSObject *)
*   the HTMLElement's JSObject pointer or NULL if unable to create a JSObject
******************************************************************************/

#if (WEBC_SUPPORT_JSCRIPT)
JSObject *HTMLElement::CreateJSObject(void)
{
	JSObject *pJSObj = GetJSObject();

	if (!pJSObj)
	{
		HTMLDocument *pDocument = GetDocument();
		WebcJSDocumentContext *pJSMgr = (pDocument)? pDocument->GetJSMgr() : 0;
		if (pJSMgr)
		{
			pJSObj = pJSMgr->CreateElement(TagToHTMLElementType[mTag]);
			SetJSObject(pJSObj);
		}
	}

	return (pJSObj);
}
#endif


/******************************************************************************
* HTMLElement::DocumentNext() - Retrieve next element in document order
*
* Parameters
*
* Description
*
* Returns (HTMLElement *)
*   the next element or NULL if this is the last one
******************************************************************************/

HTMLElement *HTMLElement::DocumentNext(void)
{
	if (FirstChild())
	{
		return (FirstChild());
	}

	if (mpNext)
	{
		return (mpNext);
	}

	HTMLElement *parent = mpParent;
	while (parent)
	{
		if (parent->mpNext)
		{
			return (parent->mpNext);
		}
		parent = parent->mpParent;
	}

	return (0);
}

/******************************************************************************
* HTMLElement::ResetClass() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLElement::ResetClass(void)
{
	vector_iterator vi;
	const WEBC_CHAR **ptr = (const WEBC_CHAR **) vector_get_first(&mClassList, &vi);
	while (ptr)
	{
		if (*ptr)
		{
			webc_string_table_remove(*ptr, __FILE__, __LINE__);
		}
		ptr = (const WEBC_CHAR **) vector_get_next(&vi);
	}

	vector_delete(&mClassList);
}

#if (WEBC_SUPPORT_INNER_HTML)

/******************************************************************************
* HTMLElement::SetInnerHTML() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLElement::SetInnerHTML (WEBC_CHAR* innerHTML)
{
	if (!(mFlags & HELEM_FLAG_PARSING_CONTENT))
	{
		DeleteChildren();

		HTMLDocumentStream htmlStream(this);

		htmlStream.Write(innerHTML);

		const char* tagName = HTML_TagTypeToString(mTag);
		if (tagName)
		{
			WebString closeTag;

			closeTag.append("</", 2);
			closeTag.append(tagName, tc_strlen(tagName));
			closeTag.append(">", 1);

			htmlStream.Write(closeTag.getChars(), closeTag.getLength());
			this->GetDocument()->Refresh(); // April2013 refresh the doc on 
		}
	}
}

/******************************************************************************
* HTMLElement::SetOuterHTML() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLElement::SetOuterHTML (WEBC_CHAR* _outerHTML)
{
	if (!(mFlags & HELEM_FLAG_PARSING_CONTENT))
	{
		WEBC_CHAR* outerHTML = webc_malloc_string_copy(_outerHTML, __FILE__, __LINE__);
		if (!outerHTML)
		{
			return;
		}

		HTMLParseContext htmlParser;

		if (HTML_InitParse(&htmlParser) >= 0)
		{
			HTMLTagParse tagParse;
			long charsParsed;
			long outerHTMLLen = webc_strlen(outerHTML);

			charsParsed = HTML_ParseUnit (
					&htmlParser,
					&tagParse,
					outerHTML,
					outerHTMLLen
				);

			if (charsParsed > 0 &&
			    tagParse.xmlParse.type == XML_UNIT_TAG &&
			    (tagParse.xmlParse.parse.tag.type == XML_TAG_START ||
			     tagParse.xmlParse.parse.tag.type == XML_TAG_EMPTY) &&
			    tagParse.htmlTag == mTag)
			{
				XML_TokenizeStrings(&tagParse.xmlParse);
				SetTagProperties(&tagParse, GetDocument());
				XML_RestoreStrings(&tagParse.xmlParse);

				SetInnerHTML(outerHTML + charsParsed);
			}

			HTML_DestroyParse(&htmlParser);
		}

		webc_free_string_copy(outerHTML, __FILE__, __LINE__);
	}
}

/******************************************************************************
* HTMLElement::AppendInnerHTML() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLElement::AppendInnerHTML (WebString* toString)
{
	HTMLElement* child = FirstChild();
	while (child)
	{
		child->AppendOuterHTML(toString);
		child = child->mpNext;
	}
}

/******************************************************************************
* HTMLElement::AppendOuterHTML() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLElement::AppendOuterHTML (WebString* toString)
{
	const char* tagName = HTML_TagTypeToString(mTag);

	// append the begin tag
	if (tagName)
	{
		toString->append("<", 1);
		toString->append(tagName, tc_strlen(tagName));

		if (mpTagAttributes)
		{
			long savedAttribsSize = 0;
			long valueLength = 0;
			const char* attribName;

			while (mpTagAttributes[savedAttribsSize] != (WEBC_CHAR) HTML_UNKNOWN_ATTRIB)
			{
				attribName = HTML_AttributeTypeToString((HTMLAttributeType) mpTagAttributes[savedAttribsSize]);

				toString->append(" ", 1);
				toString->append(attribName, tc_strlen(attribName));
				toString->append("=\"", 2);
				savedAttribsSize++;
				valueLength = webc_strlen(mpTagAttributes + savedAttribsSize);
				toString->appendHtmlEscaped(mpTagAttributes + savedAttribsSize, valueLength);
				toString->append("\"", 1);
				savedAttribsSize += (valueLength + 1);
			}
		}

		toString->append(">", 1);
	}

	AppendInnerHTML(toString);

	// append the end tag
	if (tagName)
	{
		toString->append("</", 2);
		toString->append(tagName, tc_strlen(tagName));
		toString->append(">", 1);
	}
}

/******************************************************************************
* HTMLElement::SetRenderedText() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLElement::SetRenderedText(WEBC_CHAR* textVal)
{
	HTMLElement* child = FirstChild();
	while (child)
	{
		// run through the tree of HTML elements until we find the string
		if(child->Type() == HTML_STRING_ELEMENT)
		{
			HTMLString *pString = (HTMLString *)child;
			pString->SetString(textVal);
			pString->GetTextUnit(); // side-effect is to set the proper size of the string in the display (not great)
			break;
		}
		child = child->mpNext;
	}
}

/******************************************************************************
* HTMLElement::AppendRenderedText() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLElement::AppendRenderedText (WebString* toString)
{
	HTMLElement* child = FirstChild();
	while (child)
	{
		// run through the tree of HTML elements until we hit a string
		// and then concatenate all of the strings
		if(child->Type() == HTML_STRING_ELEMENT)
			child->AppendOuterHTML(toString);
		child = child->mpNext;
	}
}

#endif // WEBC_SUPPORT_INNER_HTML


/******************************************************************************
* HTMLElement::Form() -
*
* Parameters
*
* Description
*
* Returns (HTMLForm*)
*   HTMLForm element containing this element
******************************************************************************/

HTMLForm* HTMLElement::Form(void)
{
	return (mForm.Get());
}


/******************************************************************************
* HTMLElement::SetForm() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLElement::SetForm(HTMLForm* pForm)
{
	mForm.Set(pForm);
}

WebcFontDescriptor HTMLElement::GetFontDescriptor (
		CSSPropertyValue* family,
		CSSPropertyValue* size,
		CSSPropertyValue* style,
		CSSPropertyValue* weight,
		CSSPropertyValue* variant,
		WebcFontDescriptor baseDesc
	)
{
	HTMLBrowser* browser = GetBrowser();
	if (browser)
	{
		return browser->GetFontDescriptor(family, size, style, weight, variant, baseDesc);
	}

	return baseDesc;
}

int HTMLElement::GetFontProperty (
		WebcFontDescriptor desc,
		CSSPropertyType property,
		CSSPropertyValue* value
	)
{
	HTMLBrowser* browser = GetBrowser();
	if (browser)
	{
		return browser->GetFontProperty(desc, property, value);
	}

	return -1;
}

WebGraphFont* HTMLElement::GetWebGraphFont (WebcFontDescriptor desc)
{
	HTMLBrowser* browser = GetBrowser();
	if (browser)
	{
		return browser->GetWebGraphFont(desc);
	}

	return (WEBC_NULL);
}

WebGraphFont* HTMLElement::GetDefaultFont (void)
{
	HTMLBrowser* browser = GetBrowser();
	if (browser)
	{
		return browser->GetDefaultFont();
	}

	return (WEBC_NULL);
}

#ifdef WEBC_DEBUG
void HTMLElement::Dump(int d)
{
char indentbuffer[256];
char *tagname;

	for (int i = 0; i < 4*(d+1); i++)
	{
		indentbuffer[i] = ' ';
	    indentbuffer[i+1] = 0;
	}
	if (GetTag() == HTML_TAG_BODY && ((HTMLBody *)this)->IsFragment() )
			tagname = "body(FRAGMENT)";
	else
		tagname = (char *) HTML_TagTypeToString(GetTag());

	if (d == 0)
	LOG_HTMLDOM("DUMP  %s %s #(%S) @ 0x%08x\n", indentbuffer, tagname, (this->Id() ? this->Id() : (WEBC_CHAR *)L""), (int)this);
	else
	LOG_HTMLDOM("      %s %s #(%S) @ 0x%08x\n", indentbuffer, tagname, (this->Id() ? this->Id() : (WEBC_CHAR *)L""), (int)this);

}
#endif
/*****************************************************************************/
// HTMLElementContainer Constructors
/*****************************************************************************/

HTMLElementContainer::HTMLElementContainer(
		HTMLTagParse *td,
	  #if (WEBC_SUPPORT_JSCRIPT)
		HTMLDocument *pDocument,
		JSObject *pJSObj)
	: HTMLElement (td, pDocument, pJSObj)
	  #else
		HTMLDocument *pDocument)
	: HTMLElement (td, pDocument)
	  #endif // WEBC_SUPPORT_JSCRIPT
{
	mpFirstChild = 0;
	mpLastChild = 0;
}

HTMLElementContainer::HTMLElementContainer(HTMLElementContainer &copy)
	:HTMLElement(copy)
{
	mpFirstChild = 0;
	mpLastChild = 0;
}

/*****************************************************************************/
// HTMLElementContainer Destructor
/*****************************************************************************/

HTMLElementContainer::~HTMLElementContainer(void)
{
	DeleteChildren();
}


/*****************************************************************************/
// HTMLElementContainer Methods
/*****************************************************************************/

// Methods for adding/removing children

void HTMLElementContainer::DeleteChildren(void)
{
HTMLElement *pDoomed;

	while (mpFirstChild)
	{
		pDoomed = mpFirstChild;
		Remove(pDoomed);
		WEBC_DELETE(pDoomed);
	}
}

void HTMLElementContainer::InsertFirst(HTMLElement *pNode)
{
	WEBC_ASSERT(pNode && (pNode->GetDocument() == this->GetDocument() || !pNode->GetDocument() || !this->GetDocument()));

	//LOG_HTMLDOM("HTMLElementContainer::InsertFirst inserting 0x%08x\n", (int) pNode);

	if (mpFirstChild)
	{
		mpFirstChild->mpPrev = pNode;
	}
	else
	{
		mpLastChild = pNode;
	}
	pNode->mpPrev = 0;
	pNode->mpNext = mpFirstChild;
	pNode->mpParent = this;
	mpFirstChild = pNode;

	NotifyChildAdded();
}

void HTMLElementContainer::InsertAfter(HTMLElement *pNode, HTMLElement *pRef)
{
	WEBC_ASSERT(pNode && (pNode->GetDocument() == this->GetDocument() || !pNode->GetDocument() || !this->GetDocument()));

	//LOG_HTMLDOM("HTMLElementContainer::InsertAfter inserting 0x%08x after 0x%08x\n", (int) pNode, (int) pRef);

	if (!pRef)
	{
		InsertFirst(pNode);
	}
	else
	{
		pNode->mpNext = pRef->mpNext;
		pNode->mpPrev = pRef;
		if (pNode->mpNext)
		{
			pNode->mpNext->mpPrev = pNode;
		}
		else
		{
			mpLastChild = pNode;
		}
		pRef->mpNext = pNode;
		pNode->mpParent = this;

		NotifyChildAdded();
	}
}

void HTMLElementContainer::InsertLast(HTMLElement *pNode)
{
	WEBC_ASSERT(pNode && (pNode->GetDocument() == this->GetDocument() || !pNode->GetDocument() || !this->GetDocument()));

	//LOG_HTMLDOM("HTMLElementContainer::InsertLast inserting 0x%08x into 0x%08x\n", (int) pNode, (int) this);

	if (mpLastChild)
	{
		mpLastChild->mpNext = pNode;
	}
	else
	{
		mpFirstChild = pNode;
	}
	pNode->mpPrev = mpLastChild;
	pNode->mpNext = 0;
	pNode->mpParent = this;
	mpLastChild = pNode;

	NotifyChildAdded();
}

void HTMLElementContainer::InsertBefore(HTMLElement *pNode, HTMLElement *pRef)
{
	if (pRef)
	{
		InsertAfter(pNode, pRef->mpPrev);
	}
	else
	{
		InsertLast(pNode);
	}
}

WEBC_BOOL HTMLElementContainer::IsInserted()
{
	return (!( (0 == mpNext) && (0 == mpPrev) && (0 == mpParent) ));
}

void HTMLElementContainer::Remove(HTMLElement *pNode)
{
	if (pNode && pNode->mpParent == this)
	{
		if (pNode->mpNext)
		{
			pNode->mpNext->mpPrev = pNode->mpPrev;
		}
		else
		{
			mpLastChild = pNode->mpPrev;
		}

		if (pNode->mpPrev)
		{
			pNode->mpPrev->mpNext = pNode->mpNext;
		}
		else
		{
			mpFirstChild = pNode->mpNext;
		}

		pNode->mpNext = 0;
		pNode->mpPrev = 0;
		pNode->mpParent = 0;

		NotifyChildRemoved();
	}
}

void HTMLElementContainer::NotifyChildRemoved(void)
{
	NotifyDisplay(NOTIFY_CONTENT_REMOVED);
}

void HTMLElementContainer::NotifyChildAdded(void)
{
	NotifyDisplay(NOTIFY_CONTENT_ADDED);
}

HTMLElement *HTMLElementContainer::FirstChild (void)
{
	return (mpFirstChild);
}

HTMLElement *HTMLElementContainer::LastChild (void)
{
	return (mpLastChild);
}

#ifdef WEBC_DEBUG
void HTMLElementContainer::Dump(int d)
{
	HTMLElement *pCurrentElem = FirstChild();

char indentbuffer[256];
char *tagname;
	for (int i = 0; i < 4*(d+1); i++)
	{
		indentbuffer[i] = ' ';
	    indentbuffer[i+1] = 0;
	}

	if (GetTag() == HTML_TAG_BODY && ((HTMLBody *)this)->IsFragment() )
			tagname = "body(FRAGMENT)";
	else
		tagname = (char *) HTML_TagTypeToString(GetTag());

	if (d == 0)
	LOG_HTMLDOM("DUMP  %s %s #(%S) @ 0x%08x\n", indentbuffer, tagname, (this->Id() ? this->Id() : (WEBC_CHAR *)L""), (int)this);
	else
	LOG_HTMLDOM("      %s %s #(%S) @ 0x%08x\n", indentbuffer, tagname, (this->Id() ? this->Id() : (WEBC_CHAR *)L""), (int)this);

	if(pCurrentElem)
	{
		LOG_HTMLDOM("%s%s", indentbuffer, "       Children:\n");
	}

	while(pCurrentElem)
	{
		pCurrentElem->Dump(d+1);
		pCurrentElem = pCurrentElem->mpNext;
	}

}
#endif



/*****************************************************************************/
// HTMLElementTreeWalker Methods
/*****************************************************************************/

HTMLElementTreeWalker::HTMLElementTreeWalker (
		HTMLElement* root,
		HTMLElement* first,
		HTMLElementFinder* filter,
		HTMLElementFinder* stepInto
	)
{
	mpRoot = root;
	mpPrev = 0;
	mpNext = first;
	mpFilter = filter;
	mpStepIntoFilter = stepInto;
}

HTMLElementTreeWalker::~HTMLElementTreeWalker(void)
{
}

HTMLElement* HTMLElementTreeWalker::getNext()
{
	mpPrev = mpNext;

	while (mpNext)
	{
		if (mpNext->FirstChild() &&
		    (!mpStepIntoFilter || mpStepIntoFilter->Check(mpNext)))
		{
			mpNext = mpNext->FirstChild();
		}
		else
		{
			while (mpNext != mpRoot && !mpNext->mpNext)
			{
				mpNext = mpNext->mpParent;
			}

			if (mpNext == mpRoot)
			{
				mpNext = 0;
			}
			else
			{
				mpNext = mpNext->mpNext;
			}
		}

		if (!mpFilter || mpFilter->Check(mpPrev))
		{
			break;
		}

		mpPrev = mpNext;
	}

	return (mpPrev);
}

#ifdef WEBC_DEBUG

void PrintRecievedHTMLEvent(HTMLElement *TheReciever, HTMLEvent *TheEvent);

void HTMLElement::WEBC_DEBUG_ReceiveEvent(HTMLElement *TheReciever, HTMLEvent *TheEvent)
{
    PrintRecievedHTMLEvent(TheReciever, TheEvent);
}

void PrintRecievedHTMLNotifyEvent(HTMLElement *TheReciever, Notifier *who, NotifyEvent *what);

void HTMLElement::WEBC_DEBUG_ReceiveNotify(HTMLElement *TheReciever, Notifier *who, NotifyEvent *what)
{
    PrintRecievedHTMLNotifyEvent(TheReciever, who, what);
}

#endif
