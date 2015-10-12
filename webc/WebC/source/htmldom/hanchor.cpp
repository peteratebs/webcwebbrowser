//
// HANCHOR.CPP - Methods for HTML DOM class HTMLAnchor
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
#include "webc.h"
#include "util.h"
#include "tuenum.hpp"
#include "webload.hpp"
#include "webfonts.hpp"
#include "wgkeys.hpp"
#include "hanchor.hpp"
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
// HTMLAnchor Constructors
/*****************************************************************************/

HTMLAnchor::HTMLAnchor(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
	mpHRef = 0;
	mpName = 0;
	mpTarget = 0;
	miTabIndex = 0;
	mAnchorState = HANCHOR_STATE_NORMAL;
	mCharset = WEBC_CHARSET_UNKNOWN;
	mpStrCharset = 0;
	mAccessKey = 0;
	mCoordinateList = 0;
	mCoordinateCount = 0;
	mpRel = 0;
	mpRev = 0;
	mpHRefLang = 0;
	mShape = HTML_SHAPE_UNKNOWN;
	mpStrShape = 0;
	mpMimeType = 0;
}

HTMLAnchor::HTMLAnchor(HTMLAnchor &copy)
	: HTMLFlow(copy)
{
	mpHRef = 0;
	SetHRef(copy.mpHRef);
	mpName = 0;
	SetName(copy.mpName);
	mpTarget = 0;
	SetTarget(copy.mpTarget);
	miTabIndex = copy.miTabIndex;
	mAnchorState = copy.mAnchorState;
	mCharset = copy.mCharset;
	mpStrCharset = 0;
	SetCharSet(copy.mpStrCharset);
	mAccessKey = copy.mAccessKey;
	// TODO need a deeper copy here
	mCoordinateList = 0;
	mCoordinateCount = 0;

	mpRel = 0;
	SetRel(copy.mpRel);
	mpRev = 0;
	SetRev(copy.mpRev);
	mpHRefLang = 0;
	SetHRefLang(copy.mpHRefLang);
	mShape = copy.mShape;
	mpStrShape = 0;
	mpMimeType = 0;
	SetMimeType(copy.mpMimeType);
}

/*****************************************************************************/
// HTMLAnchor Destructor
/*****************************************************************************/

HTMLAnchor::~HTMLAnchor(void)
{
	webc_free_string_copy(mpHRef, __FILE__, __LINE__);
	webc_free_string_copy(mpName, __FILE__, __LINE__);
	webc_free_string_copy(mpTarget, __FILE__, __LINE__);
	webc_free_string_copy(mpStrCharset, __FILE__, __LINE__);
	if (mCoordinateList != 0)
	{
		WEBC_FREE(mCoordinateList);
		mCoordinateList = 0;
		mCoordinateCount = 0;
	}
	webc_free_string_copy(mpRel, __FILE__, __LINE__);
	webc_free_string_copy(mpRev, __FILE__, __LINE__);
	webc_free_string_copy(mpHRefLang, __FILE__, __LINE__);
	webc_free_string_copy(mpStrShape, __FILE__, __LINE__);
	webc_free_string_copy(mpMimeType, __FILE__, __LINE__);
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

/*
void HTMLAnchor::Refresh(PegThing *pThing)
{
}
*/

HTMLEventStatus HTMLAnchor::DefaultEvent(HTMLEvent *pEvent)
{
HTMLBrowser *pBrowser = GetBrowser();
HTMLDocument *pDoc = GetDocument();
WEBC_CHAR *pHRef = mpHRef;
HTMLEventStatus retval;

    WEBC_DEBUG_ReceiveEvent(this, pEvent);

	switch (pEvent->type)
	{
		case HTML_EVENT_MOUSEOVER:

	  		mAnchorState |= HANCHOR_STATE_HOVER;
			StyleUpdateChildren();

			if (HRef())
			{
				if (pBrowser)
				{
					pBrowser->SetTempStatus(HRef(), this);
				}
			}

			return (HTML_EVENT_STATUS_CONTINUE);

		case HTML_EVENT_FOCUS:

			mAnchorState |= HANCHOR_STATE_FOCUS;
			StyleUpdateChildren();

			if (HRef())
			{
				HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 1);

				FindElement(&f);

				if (pBrowser)
				{
					pBrowser->SetTempStatus(HRef(), this);
				}
			}

			return (HTML_EVENT_STATUS_CONTINUE);

		case HTML_EVENT_MOUSEOUT:

			mAnchorState &= ~HANCHOR_STATE_HOVER;
			StyleUpdateChildren();

			if (HRef())
			{
				if (pBrowser)
				{
					pBrowser->ClearTempStatus(this);
				}
			}

			return (HTML_EVENT_STATUS_CONTINUE);

		case HTML_EVENT_UNFOCUS:

			mAnchorState &= ~HANCHOR_STATE_FOCUS;
			StyleUpdateChildren();

			if (HRef())
			{
				HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 0);

				FindElement(&f);

				if (pBrowser)
				{
					pBrowser->ClearTempStatus(this);
				}
			}

			return (HTML_EVENT_STATUS_CONTINUE);

		case HTML_EVENT_KEYDOWN:
			retval = DefaultInputKeyHandler(pEvent);
			if (retval != HTML_EVENT_STATUS_CONTINUE)
			{
				return (retval);
			}
			break;
		case HTML_EVENT_MOUSEDOWN:
	  		mAnchorState |= HANCHOR_STATE_ACTIVE;
			StyleUpdateChildren();
			return (HTML_EVENT_STATUS_CONTINUE);
			break;
		case HTML_EVENT_MOUSEUP:
	  		mAnchorState &= ~HANCHOR_STATE_ACTIVE;
			StyleUpdateChildren();
			return (HTML_EVENT_STATUS_CONTINUE);
			break;

		case HTML_EVENT_KEYPRESS:
			if (pEvent->data.key != WGK_CR)
			{
				break;
			}
			//else fallthrough
		case HTML_EVENT_CLICK:

			if (pDoc && pHRef)
			{
				HTMLDocument *pTargDoc = GetFrameDocument(Target());
				URLDescriptor urlParse(pHRef, pDoc->BaseURL());
				// don't save the current url to the history if protocol is JavaScript,
				//  because javascript:// url's are not really page locations
				if (urlParse.getProtocol() != WEBC_PROTOCOL_JSCRIPT)
				{
					pTargDoc->SaveLocation();
				}
				pTargDoc->SetSrc(pHRef, pDoc->BaseURL());
				pTargDoc->SetCharset(mCharset, HTML_CHARSET_SRC_REFERER);
#if (WEBC_SUPPORT_REFERER_HEADER)
				WEBC_CHAR *referer;
				UrlStringBuilder ubld(pDoc->CurrentURL());
				referer= ubld.MallocString(__FILE__, __LINE__);
				pTargDoc->Update(1,referer);
				WEBC_FREE(referer);
#else
				pTargDoc->Update(1,0);
#endif //WEBC_SUPPORT_REFERER_HEADER

				return (HTML_EVENT_STATUS_HALT);
			}

			return (HTML_EVENT_STATUS_CONTINUE);

		default:
			// ignore other events
			break;
	}

	return (HTMLFlow::DefaultEvent(pEvent));
}

/*
HTMLElement *HTMLAnchor::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLAnchor::Reset(void)
{
}
*/

int HTMLAnchor::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_CHARSET:
			SetCharSet(value);
			return (1);

		case HTML_ATTRIB_HREF:
			SetHRef(value);
			return (1);

		case HTML_ATTRIB_NAME:
			SetName(value);
			return (1);

		case HTML_ATTRIB_TARGET:
			SetTarget(value);
			return (1);

		case HTML_ATTRIB_REL:
			SetRel(value);
			return (1);

		case HTML_ATTRIB_REV:
			SetRev(value);
			return (1);

		case HTML_ATTRIB_HREFLANG:
			SetHRefLang(value);
			return (1);

		case HTML_ATTRIB_TYPE:
			SetMimeType(value);
			return (1);

		case HTML_ATTRIB_SHAPE:
			SetShape(value);
			return (1);

		case HTML_ATTRIB_TABINDEX:
		{
			long i = webc_atoi(value);
			SetTabIndex(UTIL_SAFE_LONG_TO_SHORT(i));
			return (1);
		}

		case HTML_ATTRIB_ACCESSKEY:
			SetAccessKey(value[0]);
			return (1);

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLFlow::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}

/******************************************************************************
* void HTMLAnchor::SetHRef(char *pHRef) - Set href attribute
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

void HTMLAnchor::SetHRef(WEBC_CHAR *pHRef)
{
	if(!pHRef)
		return;
WEBC_CHAR *pStr = (pHRef && pHRef[0])?
                 webc_malloc_string_copy(pHRef, __FILE__, __LINE__) : 0;

	webc_free_string_copy(mpHRef, __FILE__, __LINE__);
	mpHRef = pStr;
}

/******************************************************************************
* void HTMLAnchor::SetName(char *pName) - Set name attribute
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

void HTMLAnchor::SetName(const WEBC_CHAR *pName)
{
	if(!pName)
		return;

	WEBC_CHAR *pStr = webc_malloc_string_copy(pName, __FILE__, __LINE__);

	webc_free_string_copy(mpName, __FILE__, __LINE__);
	mpName = pStr;
}

/******************************************************************************
* void HTMLAnchor::SetTarget(char *pTarget) - Set target attribute
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

void HTMLAnchor::SetTarget(WEBC_CHAR *pTarget)
{
	if(!pTarget)
		return;

	WEBC_CHAR *pStr = webc_malloc_string_copy(pTarget, __FILE__, __LINE__);

	webc_free_string_copy(mpTarget, __FILE__, __LINE__);
	mpTarget = pStr;
}

/******************************************************************************
* void HTMLAnchor::SetRel(char *pRel) - Set rel attribute
*
* Parameters
*    pName - NULL-terminated string: the new value of rel
*
* Description
*    Creates a local copy of the given string.
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLAnchor::SetRel(WEBC_CHAR *pRel)
{
	if(!pRel)
		return;

	WEBC_CHAR *pStr = webc_malloc_string_copy(pRel, __FILE__, __LINE__);

	webc_free_string_copy(mpRel, __FILE__, __LINE__);
	mpRel = pStr;
}

/******************************************************************************
* void HTMLAnchor::SetRev(char *pRev) - Set rev attribute
*
* Parameters
*    pName - NULL-terminated string: the new value of rev
*
* Description
*    Creates a local copy of the given string.
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLAnchor::SetRev(WEBC_CHAR *pRev)
{
	if(!pRev)
		return;

	WEBC_CHAR *pStr = webc_malloc_string_copy(pRev, __FILE__, __LINE__);

	webc_free_string_copy(mpRev, __FILE__, __LINE__);
	mpRev = pStr;
}

/******************************************************************************
* void HTMLAnchor::SetHRefLang(char *pRev) - Set hreflang attribute
*
* Parameters
*    pName - NULL-terminated string: the new value of hreflang
*
* Description
*    Creates a local copy of the given string.
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLAnchor::SetHRefLang(WEBC_CHAR *pHRefLang)
{
	if(!pHRefLang)
		return;

	WEBC_CHAR *pStr = webc_malloc_string_copy(pHRefLang, __FILE__, __LINE__);

	webc_free_string_copy(mpHRefLang, __FILE__, __LINE__);
	mpHRefLang = pStr;
}

/******************************************************************************
* void HTMLAnchor::SetMimeType(char *pRev) - Set hreflang attribute
*
* Parameters
*    pName - NULL-terminated string: the new value of type
*
* Description
*    Creates a local copy of the given string.
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLAnchor::SetMimeType(WEBC_CHAR *pMimeType)
{
	if(!pMimeType)
		return;

	WEBC_CHAR *pStr = webc_malloc_string_copy(pMimeType, __FILE__, __LINE__);

	webc_free_string_copy(mpMimeType, __FILE__, __LINE__);
	mpMimeType = pStr;
}

/******************************************************************************
* void HTMLAnchor::SetShape(char *pStrShape) - Set shape attribute
*
* Parameters
*    pName - NULL-terminated string: the new value of shape
*
* Description
*
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLAnchor::SetShape(const WEBC_CHAR *pStrShape)
{
	mShape = HTML_ParseShapeType((HTML_CHAR *)pStrShape, 0);
}

/******************************************************************************
* char* HTMLAnchor::HRef() - Access href attribute
*
* Parameters
*
* Description
*
* Returns (char*)
*    NULL-terminated string; the anchor's href
******************************************************************************/

WEBC_CHAR *HTMLAnchor::HRef(void)
{
	return (mpHRef);
}

/******************************************************************************
* char* HTMLAnchor::Name() - Access name attribute
*
* Parameters
*
* Description
*
* Returns (char*)
*    NULL-terminated string; the anchor's name
******************************************************************************/

WEBC_CHAR *HTMLAnchor::Name(void)
{
	return (mpName);
}

/******************************************************************************
* char* HTMLAnchor::Target() - Access target attribute
*
* Parameters
*
* Description
*
* Returns (char*)
*    NULL-terminated string; the anchor's target
******************************************************************************/

WEBC_CHAR *HTMLAnchor::Target(void)
{
	return (mpTarget);
}

/******************************************************************************
* char* HTMLAnchor::Rel() - Access rel attribute
*
* Parameters
*
* Description
*
* Returns (char*)
*    NULL-terminated string; the anchor's relationship type
******************************************************************************/

WEBC_CHAR *HTMLAnchor::Rel(void)
{
	return (mpRel);
}
/******************************************************************************
* char* HTMLAnchor::Rev() - Access rev attribute
*
* Parameters
*
* Description
*
* Returns (char*)
*    NULL-terminated string; the anchor's reverse link type
******************************************************************************/

WEBC_CHAR *HTMLAnchor::Rev(void)
{
	return (mpRev);
}
/******************************************************************************
* char* HTMLAnchor::HRefLang() - Access hreflang attribute
*
* Parameters
*
* Description
*
* Returns (char*)
*    NULL-terminated string; the anchor's reference language property
******************************************************************************/

WEBC_CHAR *HTMLAnchor::HRefLang(void)
{
	return (mpHRefLang);
}

/******************************************************************************
* char* HTMLAnchor::MimeType() - Access type attribute
*
* Parameters
*
* Description
*
* Returns (char*)
*    NULL-terminated string; the anchor's type property
******************************************************************************/

WEBC_CHAR *HTMLAnchor::MimeType(void)
{
	return (mpMimeType);
}

/******************************************************************************
* char* HTMLAnchor::ShapeStr() - Access shape attribute
*
* Parameters
*
* Description
*
* Returns (char*)
*    NULL-terminated string; the anchor's shape property
******************************************************************************/

WEBC_CHAR *HTMLAnchor::ShapeStr(void)
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

/******************************************************************************
* void HTMLAnchor::SetTabIndex(short iIndex) - Set tab index
*
* Parameters
*    iIndex - Order of this element in the tab-key sequence
*
* Description
*
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLAnchor::SetTabIndex(short iIndex)
{
	miTabIndex = iIndex;
}

/******************************************************************************
* short HTMLAnchor::TabIndex() - Access tab index
*
* Parameters
*
* Description
*
* Returns (short)
*    Order of this element in the tab-key sequence
******************************************************************************/

short HTMLAnchor::TabIndex(void)
{
	if ((GetUnitType() == TU_TYPE_NODISPLAY) || (GetVisibility() == TU_VISIBILITY_HIDE) ||
		!HRef() || !GetFirstDisplayChild())
	{
		return (-1);
	}
	return (miTabIndex);
}

WEBC_CHAR *HTMLAnchor::CharSet(void)
{
	const char *pCharSetVal = webc_charset_string(mCharset);
	if(pCharSetVal)
	{
		if(mpStrCharset)
			WEBC_FREE(mpStrCharset);

		mpStrCharset = (WEBC_CHAR*) WEBC_MALLOC((webc_c_strlen(pCharSetVal) + 1) * sizeof(WEBC_CHAR));

		webc_c_strcpy(mpStrCharset, pCharSetVal);
	}
	return mpStrCharset;
}

WEBC_BOOL HTMLAnchor::SetCharSet(const WEBC_CHAR *pCharSet)
{
	if(pCharSet)
		mCharset = webc_charset_lookup(pCharSet);
	return (WEBC_CHARSET_UNKNOWN != mCharset);
}

CSSValueType HTMLAnchor::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_DISPLAY:
			pValue->display = CSS_DISPLAY_INLINE;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_TEXT_DECORATION:
			if (HRef())
			{
				pValue->textDecoration = CSS_TEXT_DECORATION_UNDERLINE;
				return (CSS_VALUE_SPECIFIED);
			}
			break;

		case CSS_PROPERTY_OUTLINE_STYLE:
			if (HRef())
			{
			  	switch (mAnchorState)
			  	{
			  		case HANCHOR_STATE_FOCUS:
			  			pValue->borderStyle = CSS_BORDER_STYLE_DOTTED;
			  			break;

			  		default:
			  			pValue->borderStyle = CSS_BORDER_STYLE_NONE;
						break;
			  	}

			  	return (CSS_VALUE_SPECIFIED);
			}
			break;

		case CSS_PROPERTY_OUTLINE_COLOR:
			if (HRef())
			{
				HTML_ColorSet(pValue->color, 0, 0, 0, 0);
			  	return (CSS_VALUE_SPECIFIED);
			}
			break;

		case CSS_PROPERTY_OUTLINE_WIDTH:
			if (HRef())
			{
				pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
				pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
				pValue->borderThickness.val.length.val.dec = 1;
				pValue->borderThickness.val.length.val.frac = 0;
			  	return (CSS_VALUE_SPECIFIED);
			}
			break;

		case CSS_PROPERTY_COLOR:
			if (HRef())
			{
			  	switch (mAnchorState)
			  	{
			  		case HANCHOR_STATE_NORMAL:
						pValue->color = GetDocument()->Body()->LinkColor();
			  			break;

			  		default:
						pValue->color = GetDocument()->Body()->ALinkColor();
						break;
			  	}

				return (CSS_VALUE_SPECIFIED);
			}
			break;
		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}

long HTMLAnchor::CoordinateCount(void)
{
	if(0 == mCoordinateCount)
		PopulateCoordinates();
	return mCoordinateCount;
}

HTMLLength *HTMLAnchor::Coordinates(void)
{
	if(0 == mCoordinateList)
		PopulateCoordinates();
	return mCoordinateList;
}

void HTMLAnchor::SetCoordinates(WEBC_CHAR *pStrCoordValues)
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
		}
		else
		{
			mCoordinateCount = 0;
		}
	}

	// it seems that no other browsers (Firefox, Opera, IE) have any effect when the script sets
	// the anchor's coord poperty. Therefore, we'll just punt on this for now
#if 0
	if(mCoordinateList && (mCoordinateCount > 1))
	{
		WebRect rect;

		GetDocumentDisplayRect(&rect, 0);
		rect.MoveTo(HTML_LengthGetValue(mCoordinateList[0]), HTML_LengthGetValue(mCoordinateList[1]));
		SetDocumentDisplayRect(&rect);
	}
#endif

}

/*****************************************************************************/
// HTMLAnchor-Specific Methods
/*****************************************************************************/

void HTMLAnchor::PopulateCoordinates()
{
	if (mCoordinateList != 0)
	{
		WEBC_FREE(mCoordinateList);
		mCoordinateList = 0;
		mCoordinateCount = 0;
	}

	mCoordinateList = (HTMLLength*) WEBC_MALLOC(sizeof(HTMLLength)*4);
	if (mCoordinateList)
	{
		WebRect rect;
		GetDocumentDisplayRect(&rect, 0);
		mCoordinateList[mCoordinateCount] = 0;
		HTML_LengthSetUnit(mCoordinateList[mCoordinateCount], HTML_LENGTH_UNIT_PIXELS);
		HTML_LengthSetValue(mCoordinateList[mCoordinateCount], rect.left);
		mCoordinateCount++;
		mCoordinateList[mCoordinateCount] = 0;
		HTML_LengthSetUnit(mCoordinateList[mCoordinateCount], HTML_LENGTH_UNIT_PIXELS);
		HTML_LengthSetValue(mCoordinateList[mCoordinateCount], rect.top);
		mCoordinateCount++;
		mCoordinateList[mCoordinateCount] = 0;
		HTML_LengthSetUnit(mCoordinateList[mCoordinateCount], HTML_LENGTH_UNIT_PIXELS);
		HTML_LengthSetValue(mCoordinateList[mCoordinateCount], rect.right);
		mCoordinateCount++;
		mCoordinateList[mCoordinateCount] = 0;
		HTML_LengthSetUnit(mCoordinateList[mCoordinateCount], HTML_LENGTH_UNIT_PIXELS);
		HTML_LengthSetValue(mCoordinateList[mCoordinateCount], rect.bottom);
		mCoordinateCount++;
	}
}


#if (WEBC_SUPPORT_STYLE_SHEETS)
int HTMLAnchor::InPseudoClass(CSSPseudoClass ps)
{
	switch (ps)
	{
		case CSS_PSEUDO_CLASS_ACTIVE:
			return ((mAnchorState & (HANCHOR_STATE_ACTIVE|HANCHOR_STATE_FOCUS)) != 0);

		case CSS_PSEUDO_CLASS_VISITED:
			return ((mAnchorState & HANCHOR_STATE_VISITED) != 0);

		case CSS_PSEUDO_CLASS_HOVER:
			return ((mAnchorState & HANCHOR_STATE_HOVER) != 0);

		case CSS_PSEUDO_CLASS_LINK:
			return (HRef() != 0);

		case CSS_PSEUDO_CLASS_NONE:
			return (1);

		default:
			break;
	}

	return (0);
}
#endif // WEBC_SUPPORT_STYLE_SHEETS
