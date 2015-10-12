//
// HIMAGE.CPP - Methods for HTML DOM class HTMLImage
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
#include "webcfg.h"
#include "htmldoc.hpp"
#include "himage.hpp"
#include "hanchor.hpp"
#include "hform.hpp"
#include "webload.hpp"
#include "css.h"
#include "wgkeys.hpp"
#include "htmlfind.hpp"
#include "htmlbrow.hpp"
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
// HTMLImage Constructors
/*****************************************************************************/

#if (WEBC_SUPPORT_JSCRIPT)
HTMLImage::HTMLImage(HTMLTagParse *td, HTMLDocument *pDocument, JSObject *pJSObj)
	: HTMLElement(td, pDocument, pJSObj)
#else
HTMLImage::HTMLImage(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElement(td, pDocument)
#endif // WEBC_SUPPORT_JSCRIPT
{
	mAlt = 0;
	mLongDesc = 0;
	mMap = 0;
	mUseMap = 0;
	mName = 0;
	mSrc = 0;
	mTabIndex = 0;
	HTML_LengthSet(mImageWidth, 0, HTML_LENGTH_UNIT_NONE);
	HTML_LengthSet(mImageHeight, 0, HTML_LENGTH_UNIT_NONE);
	mImgAlign = HTML_ALIGN_NONE;
	mImage = 0;
	mHSpace = 0;
	mVSpace = 0;

	mImageDisplay = 0;

	mBorderStyle = CSS_BORDER_STYLE_NONE;
	mBorderWidth = 0;

#if(WEBC_IMAGE_DRAW_EXTENSIONS)
	m_drawColor = 0;
	m_brushWidth = 1;
	m_pBackstoreBitMap = 0;
#endif
}


/*****************************************************************************/
// HTMLImage Destructor
/*****************************************************************************/

HTMLImage::~HTMLImage(void)
{
	SetForm(WEBC_NULL);

	webc_free_string_copy(mAlt, __FILE__, __LINE__);
	webc_free_string_copy(mLongDesc, __FILE__, __LINE__);
	webc_free_string_copy(mUseMap, __FILE__, __LINE__);
	webc_free_string_copy(mName, __FILE__, __LINE__);
	webc_free_string_copy(mSrc, __FILE__, __LINE__);

	if (mImage)
	{
		mImage->Release();
	}

	if (mImageDisplay)
	{
		WEBC_DELETE(mImageDisplay);
	}
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

void HTMLImage::Update(WEBC_PFBYTE o, WEBC_PFBYTE data)
{
	// if the src dirty bit is set, then reload our bitmap
	if (mFlags & HIMG_FLAG_SRC_DIRTY)
	{
		ClearFlag(HIMG_FLAG_SRC_DIRTY);
		if (Src())
		{
			HTMLDocument *pDoc = GetDocument();
			if (pDoc)
			{
				SetFlag(HELEM_FLAG_LOADING_IMAGE);
				pDoc->LoadImage(Src(), this);
			}
		}
	}

	HTMLElement::Update(o,data);
}

void HTMLImage::RefreshFromSrc()
{
	if (Src())
	{
		HTMLDocument *pDoc = GetDocument();
		if (pDoc)
		{
			SetFlag(HELEM_FLAG_LOADING_IMAGE);
			pDoc->LoadImage(Src(), this, WEBC_TRUE);
		}
	}
}


HTMLEventStatus HTMLImage::DefaultEvent(HTMLEvent *pEvent)
{
HTMLBrowser *pBrowser = GetBrowser();
HTMLDocument *pDoc = GetDocument();
HTMLForm *pForm = Form();
HTMLNthOfTypeFinder anchorFinder(HTML_ANCHOR_ELEMENT, 0);
HTMLAnchor *pAnchor = (HTMLAnchor *) FindParent(&anchorFinder);
WEBC_CHAR *pHRef = (pAnchor)? pAnchor->HRef() : 0, *pTarget = 0;
HTMLEvent e;

    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	switch (pEvent->type)
	{
		case HTML_EVENT_FOCUS:
		{
			HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 1);
			FindElement(&f);
			SetFlag(HIMG_FLAG_HAS_FOCUS);
		}
			// fall through

		case HTML_EVENT_MOUSEOVER:
		case HTML_EVENT_MOUSEMOVE:
			StyleUpdateChildren();
			if (pBrowser && pAnchor)
			{
				if (GetMapType() == HTML_IMAGE_MAP_SERVER)
				{
					DISPLAY_INT x=0,y=0;
					GetDocumentPosition(&x, &y);
					x = pEvent->data.position.x - x;
					y = pEvent->data.position.y - y;

					pHRef = InsertXYCoords(pHRef,x,y);
					pBrowser->SetTempStatus(pHRef, this);
					WEBC_FREE(pHRef);
				}
				else
				{
					pBrowser->SetTempStatus(pHRef, this);
				}
			}
			return (HTML_EVENT_STATUS_CONTINUE);

		case HTML_EVENT_UNFOCUS:
		{
			HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 0);
			FindElement(&f);
			ClearFlag(HIMG_FLAG_HAS_FOCUS);
		}
			// fall through

		case HTML_EVENT_MOUSEOUT:
			StyleUpdateChildren();
			if (pBrowser)
			{
				pBrowser->ClearTempStatus(this);
			}
			return(HTML_EVENT_STATUS_CONTINUE);

		case HTML_EVENT_KEYDOWN:
			if (pEvent->data.key != WGK_CR)
			{
				HTMLEventStatus retval = DefaultInputKeyHandler(pEvent);
				if (retval != HTML_EVENT_STATUS_CONTINUE)
				{
					return (retval);
				}
				break;
			}
			//else fall-through
		case HTML_EVENT_CLICK:
			if (pDoc)
			{
				if (pHRef)
				{
					pTarget = pAnchor->Target();
					if (!pTarget)
					{
						pTarget = pDoc->BaseTarget();
					}

					WEBC_BOOL freeHRef = WEBC_FALSE;

					if (GetMapType() == HTML_IMAGE_MAP_SERVER)
					{
						DISPLAY_INT x=0,y=0;
						GetDocumentPosition(&x, &y);
						x = pEvent->data.position.x - x;
						y = pEvent->data.position.y - y;

						pHRef = InsertXYCoords(pHRef, x, y);
						freeHRef = WEBC_TRUE;
					}

					HTMLDocument* pTargDoc = GetFrameDocument(pTarget);

					URLDescriptor urlParse(pHRef, pDoc->BaseURL());
					// don't save the current url to the history if protocol is JavaScript,
					//  because javascript:// url's are not really page locations
					if (urlParse.getProtocol() != WEBC_PROTOCOL_JSCRIPT)
					{
						pTargDoc->SaveLocation();
					}

					pTargDoc->SetSrc(pHRef);
#if (WEBC_SUPPORT_REFERER_HEADER)
//					WEBC_CHAR *referer;
//					UrlStringBuilder ubld(pDoc->CurrentURL());
//					referer= ubld.MallocString(__FILE__, __LINE__);
//					pTargDoc->Update(1,referer);
//					WEBC_FREE(referer);
#endif //#if (WEBC_SUPPORT_REFERER_HEADER)
					pTargDoc->Update(1,0);  //if you want to restore the commneted out code, you have to remove this one

					if (freeHRef)
					{
						WEBC_FREE(pHRef);
					}

					return (HTML_EVENT_STATUS_HALT);
				}

				if (pForm)
				{
					DISPLAY_INT x=0,y=0;
					GetDocumentPosition(&x, &y);

					e.type = HTML_EVENT_SUBMIT;
					e.data.position.x = pEvent->data.position.x - x;
					e.data.position.y = pEvent->data.position.y - y;
					e.elem = (HELEMENT_HANDLE)this;

					return (pForm->Event(&e));
				}
			}
			break;
		case HTML_EVENT_MOUSEDOWN:
			if(Disabled())
				return (HTML_EVENT_STATUS_DONE);
			break;
		default:
			// ignore other messages
			break;
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

HTMLEventStatus HTMLImage::JSEvent(HTMLEvent *pEvent)
{
	if(Disabled())
		return HTML_EVENT_STATUS_DONE;
	else
		return HTMLElement::JSEvent(pEvent);
}
/*
HTMLDocument *HTMLImage::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLImage::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLImage::Reset(void)
{
}
*/

int HTMLImage::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_HSPACE:
			SetHSpace(webc_atoi(value));
			return (1);

		case HTML_ATTRIB_VSPACE:
			SetVSpace(webc_atoi(value));
			return (1);
		case HTML_ATTRIB_ALT:
			SetAlt(value);
			return (1);

		case HTML_ATTRIB_LONGDESC:
			SetLongDesc(value);
			return (1);

		case HTML_ATTRIB_USEMAP:
			if (value)
			{
				while(*value && (*value==' '||*value=='#'))
				{
					value++;
				}
				SetUseMap(value);
			}
			return (1);

		case HTML_ATTRIB_ISMAP:
			SetIsMap(WEBC_TRUE);
			return (1);

		case HTML_ATTRIB_ALIGN:
			mImgAlign = HTML_ParseAlignType(value, 0);
			StyleUpdateChildren();
			return (1);

		case HTML_ATTRIB_SRC:
			SetSrc(value);
			return (1);

		case HTML_ATTRIB_NAME:
			SetName(value);
			return (1);

		case HTML_ATTRIB_WIDTH:
			HTML_ParseLength(value, 0, &mImageWidth);
			return (1);

		case HTML_ATTRIB_HEIGHT:
			HTML_ParseLength(value, 0, &mImageHeight);
			return (1);

		case HTML_ATTRIB_TABINDEX:
		{
			long l = webc_atoi(value);
			SetTabIndex(UTIL_SAFE_LONG_TO_SHORT(l));
			return (1);
		}

		case HTML_ATTRIB_BORDER:
			{
				long l = webc_atoi(value);
				if(l)
				{
					mBorderStyle = CSS_BORDER_STYLE_SOLID;
					mBorderWidth = (short)l;
				}
				else
				{
					mBorderStyle = CSS_BORDER_STYLE_NONE;
					mBorderWidth = 0;
				}
				return (1);
			}
		case HTML_ATTRIB_DISABLED:
			SetFlag(HINPUT_FLAG_DISABLED);
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

void HTMLImage::SetTabIndex(short index)
{
	mTabIndex = index;
}

WEBC_INT16 HTMLImage::TabIndex()
{
	if (GetUnitType() == TU_TYPE_NODISPLAY ||
	    GetVisibility() == TU_VISIBILITY_HIDE ||
	    GetTag() != HTML_TAG_INPUT)
	{
		return (-1);
	}

	return mTabIndex;
}

void HTMLImage::SetLongDesc(const WEBC_CHAR *pLongDesc)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(pLongDesc, __FILE__, __LINE__);
	webc_free_string_copy(mLongDesc, __FILE__, __LINE__);
	mLongDesc = pStr;
}

void HTMLImage::SetAlt(const WEBC_CHAR *pAlt)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(pAlt, __FILE__, __LINE__);
	webc_free_string_copy(mAlt, __FILE__, __LINE__);
	mAlt = pStr;
}

void HTMLImage::SetUseMap(const WEBC_CHAR *pUseMap)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(pUseMap, __FILE__, __LINE__);
	webc_free_string_copy(mUseMap, __FILE__, __LINE__);
	mUseMap = pStr;
	mMap = 0;
}

HTMLMap *HTMLImage::GetMap(void)
{
HTMLDocument *pDoc = GetDocument();

	if (!mMap && mUseMap && pDoc)
	{
		HTMLMapFinder f(mUseMap);
		mMap = (HTMLMap *) pDoc->FindElement(&f);
	}

	return (mMap);
}

WEBC_CHAR *HTMLImage::Name(void)
{
	return (mName);
}

WEBC_CHAR *HTMLImage::Src(void)
{
	return (mSrc);
}

void HTMLImage::SetName(const WEBC_CHAR* name)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(name, __FILE__, __LINE__);
	webc_free_string_copy(mName, __FILE__, __LINE__);
	mName = pStr;
}

void HTMLImage::SetSrc(const WEBC_CHAR* src)
{
	// Set to load bitmap when we update
	if (!src || !mSrc || (webc_stricmp(src, mSrc)))
	{
		SetFlag(HIMG_FLAG_SRC_DIRTY);
	}

	WEBC_CHAR *pStr = webc_malloc_string_copy(src, __FILE__, __LINE__);
	webc_free_string_copy(mSrc, __FILE__, __LINE__);
	mSrc = pStr;
}


CSSValueType HTMLImage::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_FLOAT:
			switch (mImgAlign)
			{
				case HTML_ALIGN_LEFT:
					pValue->floatType = CSS_FLOAT_LEFT;
					break;

				case HTML_ALIGN_RIGHT:
					pValue->floatType = CSS_FLOAT_RIGHT;
					break;

				default:
					pValue->floatType = CSS_FLOAT_NONE;
					break;
			}
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_VERTICAL_ALIGN:
			switch (mImgAlign)
			{
				case HTML_ALIGN_TOP:
					// if IE, texttop, if Mozilla, top
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_TOP;
					break;

				case HTML_ALIGN_BOTTOM:
					// if IE, textbottom, if Mozilla, bottom
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_BOTTOM;
					break;

				case HTML_ALIGN_ABSMIDDLE:
				case HTML_ALIGN_MIDDLE:
				case HTML_ALIGN_CENTER:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_MIDDLE;
					break;

				case HTML_ALIGN_TEXTTOP:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_TEXT_TOP;
					break;

				case HTML_ALIGN_TEXTBOTTOM:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_TEXT_BOTTOM;
					break;

				case HTML_ALIGN_ABSBOTTOM:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_BOTTOM;
					break;

				case HTML_ALIGN_BASELINE:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_BASELINE;
					break;

				default:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_BASELINE;
					break;
			}
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_OUTLINE_COLOR:
			return (CSS_VALUE_INHERIT);

		case CSS_PROPERTY_OUTLINE_STYLE:
			if (GetFlags() & HIMG_FLAG_HAS_FOCUS)
			{
		  		pValue->borderStyle = CSS_BORDER_STYLE_DOTTED;
				return (CSS_VALUE_SPECIFIED);
			}
			return (CSS_VALUE_INHERIT);

		case CSS_PROPERTY_OUTLINE_WIDTH:
			return (CSS_VALUE_INHERIT);

		case CSS_PROPERTY_WIDTH:
			if (CSS_HTMLLengthToCSS(&pValue->length, mImageWidth) < 0)
			{
				pValue->length.type = CSS_LENGTH_UNIT_AUTO;
			}
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_HEIGHT:
			if (CSS_HTMLLengthToCSS(&pValue->length, mImageHeight) < 0)
			{
				pValue->length.type = CSS_LENGTH_UNIT_AUTO;
			}
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_MARGIN_LEFT:
		case CSS_PROPERTY_MARGIN_RIGHT:
			if (mFlags & HIMG_FLAG_HSPACE_SPECIFIED)
			{
				pValue->length.type = CSS_LENGTH_UNIT_PX;
				pValue->length.val.dec = mHSpace;
				pValue->length.val.frac = 0;
				return (CSS_VALUE_SPECIFIED);
			}
			break;

		case CSS_PROPERTY_MARGIN_TOP:
		case CSS_PROPERTY_MARGIN_BOTTOM:
			if (mFlags & HIMG_FLAG_VSPACE_SPECIFIED)
			{
				pValue->length.type = CSS_LENGTH_UNIT_PX;
				pValue->length.val.dec = mVSpace;
				pValue->length.val.frac = 0;
				return (CSS_VALUE_SPECIFIED);
			}
			break;

		case CSS_PROPERTY_BORDER_BOTTOM_STYLE:
		case CSS_PROPERTY_BORDER_LEFT_STYLE:
		case CSS_PROPERTY_BORDER_RIGHT_STYLE:
		case CSS_PROPERTY_BORDER_TOP_STYLE:
			{
				pValue->borderStyle = mBorderStyle;
				return (CSS_VALUE_SPECIFIED);
			}
		case CSS_PROPERTY_BORDER_BOTTOM_WIDTH:
		case CSS_PROPERTY_BORDER_LEFT_WIDTH:
		case CSS_PROPERTY_BORDER_RIGHT_WIDTH:
		case CSS_PROPERTY_BORDER_TOP_WIDTH:
			{
				pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
				pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
				pValue->borderThickness.val.length.val.dec = mBorderWidth;
				pValue->borderThickness.val.length.val.frac = 0;
				return (CSS_VALUE_SPECIFIED);
			}
		case CSS_PROPERTY_BORDER_BOTTOM_COLOR:
		case CSS_PROPERTY_BORDER_LEFT_COLOR:
		case CSS_PROPERTY_BORDER_RIGHT_COLOR:
		case CSS_PROPERTY_BORDER_TOP_COLOR:
			{
				pValue->color = HTML_RGBAToColor(0x00, 0x00, 0x00, 0x00); // black
				return (CSS_VALUE_SPECIFIED);
			}

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}


/*****************************************************************************/
// HTMLImage-Specific Methods
/*****************************************************************************/

WEBC_CHAR* HTMLImage::InsertXYCoords (
		WEBC_CHAR *base_str,
		DISPLAY_INT x,
		DISPLAY_INT y
	)
{
WEBC_CHAR xystr[36];
int sl;
WEBC_CHAR *str;

	xystr[0] = '?';
	xystr[35] = 0;
	webc_print_dec(&(xystr[1]), x, 1, 16);
	webc_c_strcat(xystr, ",");
	webc_print_dec(&(xystr[webc_strlen(xystr)]), y, 1, 16);

	sl = webc_strlen(base_str) + webc_strlen(xystr);

	str = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR) * (sl+1));
	if(!str)
	{
		return(0);
	}

	webc_strcpy(str, base_str);
	webc_strcat(str, xystr);

	return (str);
}


void HTMLImage::SetImage(WebcImage* pImage)
{
	if (pImage == mImage)
	{
		return;
	}

	if (pImage)
	{
		pImage->Claim();
	}

	if (mImage)
	{
		mImage->Release();
	}

	mImage = pImage;

	if (mImageDisplay)
	{
		NotifyEvent what(NOTIFIER_TYPE_HTML_ELEMENT, NOTIFY_BITMAP_CHANGE);
		mImageDisplay->Notify(this, &what);
	}
}

WebcImage *HTMLImage::GetImage()
{
	return (mImage);
}

DisplayElement *HTMLImage::GetDisplayElement (WEBC_BOOL create)
{
	return (GetImageDisplay(create));
}

TextUnit *HTMLImage::GetTextUnit (void)
{
	return (GetImageDisplay());
}

HTMLImageDisplay *HTMLImage::GetImageDisplay (WEBC_BOOL create)
{
	if (!mImageDisplay && create)
	{
		WEBC_NEW_VERBOSE(mImageDisplay, HTMLImageDisplay(this),"HTMLImageDisplay");
	}

	return (mImageDisplay);
}

void HTMLImage::SetHSpace (DISPLAY_INT h)
{
	SetFlag(HIMG_FLAG_HSPACE_SPECIFIED);
	mHSpace = UTIL_SAFE_LONG_TO_SHORT(EBSMAX(0, h));
}

void HTMLImage::SetVSpace (DISPLAY_INT v)
{
	SetFlag(HIMG_FLAG_VSPACE_SPECIFIED);
	mVSpace = UTIL_SAFE_LONG_TO_SHORT(EBSMAX(0, v));
}

DISPLAY_INT HTMLImage::GetPixelWidth(void)
{
	return (GetImageDisplay(WEBC_TRUE)->GetPixelWidth());
}

DISPLAY_INT HTMLImage::GetPixelHeight(void)
{
	return (GetImageDisplay(WEBC_TRUE)->GetPixelHeight());
}

void HTMLImage::Notify (Notifier *who, NotifyEvent *what)
{
    WEBC_DEBUG_ReceiveNotify(this, who, what);
	switch (what->type)
	{
		case NOTIFIER_TYPE_IMAGE:
			switch (what->event)
			{
				case NOTIFY_DONE:
					ClearFlag(HELEM_FLAG_LOADING_IMAGE);
					SetImage((WebcImage *) who);
// <EDIT> 20080120 _Added Call to Event(HTML_EVENT_LOAD) implements onload event for images
// <EDIT> 20080120 _
                    {
                		HTMLEvent e;
                		e.type = HTML_EVENT_LOAD;
                		e.data.position.x = 0;
                		e.data.position.y = 0;
                        Event(&e);
                    }
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}
}

/******************************************************************************
* HTMLImage::SetForm() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLImage::SetForm(HTMLForm* pForm)
{
	if (Form())
	{
		Form()->RemoveField(this);
	}
	HTMLElement::SetForm(pForm);
	if (Form() && GetTag() == HTML_TAG_INPUT)
	{
		Form()->AddField(this);
	}
}
