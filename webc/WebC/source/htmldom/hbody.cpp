//
// HBODY.CPP - Methods for HTML DOM class HTMLBody
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
#include "htmldom.hpp"
#include "dhbody.hpp"
#include "wgkeys.hpp"
#include "hbody.hpp"
#include "htmldoc.hpp"
#include "htmlfind.hpp"
#include "htmlbrow.hpp"
#if (WEBC_SUPPORT_FRAMES)
#include "hframe.hpp"
#endif
#include "webcstr.h"
#include "webcmem.h"
#include "objref.cpp"
#include "webcdefault.h"

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
// HTMLBody Constructors
/*****************************************************************************/

HTMLBody::HTMLBody(HTMLDocument* document, WEBC_BOOL bIsFragment)
	: HTMLFlow(0, document)
{
	RestoreDefaults();

	mTag = HTML_TAG_BODY;
	mbIsFragment = bIsFragment;
}

HTMLBody::HTMLBody(HTMLTagParse *td, HTMLDocument *document)
	: HTMLFlow(td, document)
{
	RestoreDefaults();
	mbIsFragment = WEBC_FALSE;
}

/*****************************************************************************/
// HTMLBody Destructor
/*****************************************************************************/

HTMLBody::~HTMLBody(void)
{
}

/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/

WEBC_UINT16 HTMLBody::GetUnitType(void)
{
	return (TU_TYPE_DEFAULT);
}

/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/

/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

HTMLEventStatus HTMLBody::DefaultEvent(HTMLEvent *pEvent)
{
	HTMLDocument *pDoc;
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	switch (pEvent->type)
	{
		case HTML_EVENT_KEYDOWN:
			switch (pEvent->data.key)
			{
				case WGK_F2:
				{
					pDoc = GetDocument();
					if (!pDoc)
					{
						break;
					}

					// HTMLFirstVisibleTabFinder f;
					HTMLFirstTabFinder f;
					pDoc->FindElement(&f);
					HTMLElement *pNewFocus = f.Found();
					if (pNewFocus)
					{
						pNewFocus->Focus();
					}
					else
					{
						HTMLFirstTabFinder f;
						pDoc->FindElement(&f);
						HTMLElement *pNewFocus = f.Found();
						if (pNewFocus)
						{
							pNewFocus->Focus();
						}
						else
						{
							// no tab-able elements in this document; if it is a frame,
							//  try jumping to another frame
							pNewFocus = GetNextInTabOrder();
							if (pNewFocus)
							{
								pNewFocus->Focus();
							}
						}
					}
				}
				return (HTML_EVENT_STATUS_DONE);

				case WGK_F1:
				{
					pDoc = GetDocument();
					if (!pDoc)
					{
						break;
					}

					// HTMLLastVisibleTabFinder f;
					HTMLLastTabFinder f;
					pDoc->FindElement(&f);
					HTMLElement *pNewFocus = f.Found();
					if (pNewFocus)
					{
						pNewFocus->Focus();
					}
					else
					{
						HTMLLastTabFinder f;
						pDoc->FindElement(&f);
						HTMLElement *pNewFocus = f.Found();
						if (pNewFocus)
						{
							pNewFocus->Focus();
						}
						else
						{
							// no tab-able elements in this document; if it is a frame,
							//  try jumping to another frame
							pNewFocus = GetPrevInTabOrder();
							if (pNewFocus)
							{
								pNewFocus->Focus();
							}
						}
					}
				}
				return (HTML_EVENT_STATUS_DONE);

				case WGK_LNDN:
				{
					pDoc = GetDocument();
					if (!pDoc)
					{
						break;
					}

					pDoc->ScrollBy(0, WEBC_CFG_DEFAULT_VSCROLL_STEP);
				}
				break;

				case WGK_LNUP:
				{
					pDoc = GetDocument();
					if (!pDoc)
					{
						break;
					}

					pDoc->ScrollBy(0, -(EBSMIN(WEBC_CFG_DEFAULT_VSCROLL_STEP, pDoc->GetScrollTop())));
				}
				break;

				case WGK_PGDN:
				{
					pDoc = GetDocument();
					if (!pDoc)
					{
						break;
					}

					if (pDoc->GetDisplayManager())
					{
						pDoc->ScrollBy(0, pDoc->GetDisplayManager()->mViewRect.Height() - (WEBC_CFG_DEFAULT_VSCROLL_STEP));
					}
				}
				break;

				case WGK_PGUP:
				{
					pDoc = GetDocument();
					if (!pDoc)
					{
						break;
					}

					if (pDoc->GetDisplayManager())
					{
						pDoc->ScrollBy(0, EBSMIN(pDoc->GetScrollTop(), WEBC_CFG_DEFAULT_VSCROLL_STEP - pDoc->GetDisplayManager()->mViewRect.Height()));
					}
				}
				break;

				case WGK_LEFT:
				{
					pDoc = GetDocument();
					if (!pDoc)
					{
						break;
					}

					pDoc->ScrollBy(-(WEBC_CFG_DEFAULT_HSCROLL_STEP), 0);
				}
				break;

				case WGK_RIGHT:
				{
					pDoc = GetDocument();
					if (!pDoc)
					{
						break;
					}

					pDoc->ScrollBy(WEBC_CFG_DEFAULT_HSCROLL_STEP, 0);
				}
				break;

				case WGK_F3:
				{
					HTMLBrowser *pBrowser = GetBrowser();
					if (pBrowser)
					{
						pBrowser->Back();
						return (HTML_EVENT_STATUS_HALT);
					}
				}
				break;

				case WGK_F4:
				{
					HTMLBrowser *pBrowser = GetBrowser();
					if (pBrowser)
					{
						pBrowser->Forward();
						return (HTML_EVENT_STATUS_HALT);
					}
				}
				break;

				case WGK_F5:
				{
					HTMLBrowser *pBrowser = GetBrowser();
					if (pBrowser)
					{
						pBrowser->Refresh();
						return (HTML_EVENT_STATUS_HALT);
					}
				}
				break;

				case WGK_F6:
				{
					HTMLBrowser *pBrowser = GetBrowser();
					if (pBrowser)
					{
						pBrowser->Home();
						return (HTML_EVENT_STATUS_HALT);
					}
				}
				break;

				case WGK_F7:
				{
					HTMLBrowser *pBrowser = GetBrowser();
					if (pBrowser)
					{
						pBrowser->StopAll();
						return (HTML_EVENT_STATUS_HALT);
					}
				}
				break;

			  #if (WEBC_SUPPORT_OFFLINE_BROWSING)

				case WGK_F8:
				{
					extern int giWebcOnline;

					giWebcOnline = 1 - giWebcOnline;
				}
				break;

				case WGK_F9:
				{
					HTMLDocument* doc = GetDocument();
					if (doc)
					{
						doc->SaveContentForOffline();
					}
				}
				break;

			  #endif /* WEBC_SUPPORT_OFFLINE_BROWSING */
			}
			break;

		default:
			break;
	}
	return (HTMLFlow::DefaultEvent(pEvent));
}

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

int HTMLBody::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	HTMLColor color;

	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	if (!value)
	{
		return (0);
	}

	switch (Property)
	{
		case HTML_ATTRIB_BGCOLOR:
			if (HTML_ParseColor(value, 0, &color) > 0)
			{
				SetBodyBackgroundColor(color);
			}
			return (1);

		case HTML_ATTRIB_TEXT:
			if (HTML_ParseColor(value, 0, &color) > 0)
			{
				SetTextColor(color);
			}
			return (1);

		case HTML_ATTRIB_LINK:
			if (HTML_ParseColor(value, 0, &color) > 0)
			{
				SetLinkColor(color);
			}
			return (1);

		case HTML_ATTRIB_ALINK:
			if (HTML_ParseColor(value, 0, &color) > 0)
			{
				SetALinkColor(color);
			}
			return (1);

		case HTML_ATTRIB_VLINK:
			if (HTML_ParseColor(value, 0, &color) > 0)
			{
				SetVLinkColor(color);
			}
			return (1);

		case HTML_ATTRIB_NOBACKGROUND:
			SetFlag(HBODY_FLAG_NOBACKGROUND);
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
* void HTMLBody::SetTextColor(HTMLColor color) - Set document's
     default unselected text color
*
* Parameters
*    pColor - text color (see html.h for html_color definition)
*
* Description
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLBody::SetTextColor(HTMLColor color)
{
	mTextColor = color;
}

/******************************************************************************
* html_color* HTMLBody::TextColor() - Access document's default unselected
*    text color
*
* Parameters
*
* Description
*
* Returns (html_color*)
*    Pointer to the unselected text color if specified; NULL otherwise
******************************************************************************/

HTMLColor HTMLBody::TextColor(void)
{
	return mTextColor;
}


/******************************************************************************
* void HTMLBody::SetLinkColor(html_color *pColor) - Set document's
     default unselected link color
*
* Parameters
*    pColor - link color (see html.h for html_color definition)
*
* Description
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLBody::SetLinkColor(HTMLColor color)
{
	mLinkColor = color;
}

/******************************************************************************
* html_color* HTMLBody::LinkColor() - Access document's default unselected
*    link color
*
* Parameters
*
* Description
*
* Returns (html_color*)
*    Pointer to the unselected link color if specified; NULL otherwise
******************************************************************************/

HTMLColor HTMLBody::LinkColor(void)
{
	return mLinkColor;
}

/******************************************************************************
* void HTMLBody::SetALinkColor(HTMLColor color) - Set document's
     active link color
*
* Parameters
*    pColor - link color (see html.h for html_color definition)
*
* Description
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLBody::SetALinkColor(HTMLColor color)
{
	mALinkColor = color;
}

/******************************************************************************
* html_color* HTMLBody::ALinkColor() - Access document's active link color
*
* Parameters
*
* Description
*
* Returns (html_color*)
*    Pointer to the active link color if specified; NULL otherwise
******************************************************************************/

HTMLColor HTMLBody::ALinkColor(void)
{
	return mALinkColor;
}

/******************************************************************************
* void HTMLBody::SetVLinkColor(html_color *pColor) - Set document's
     default visited link color
*
* Parameters
*    pColor - link color (see html.h for html_color definition)
*
* Description
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLBody::SetVLinkColor(HTMLColor color)
{
	mVLinkColor = color;
}

/******************************************************************************
* HTMLColor HTMLBody::VLinkColor() - Access document's default visited
*    link color
*
* Parameters
*
* Description
*
* Returns (html_color*)
*    Pointer to the visited link color if specified; NULL otherwise
******************************************************************************/

HTMLColor HTMLBody::VLinkColor(void)
{
	return mVLinkColor;
}

/******************************************************************************
* void HTMLBody::SetBodyBackgroundColor() -
*
* Parameters
*
* Description
*
* Returns (void)
*    nothing
******************************************************************************/

void HTMLBody::SetBodyBackgroundColor(HTMLColor color)
{
	SetFlag(HBODY_FLAG_BGCOLOR);
	mBodyBackgroundColor = color;
}

/******************************************************************************
* html_color* HTMLBody::VLinkColor() - Access document's background color
*
* Parameters
*
* Description
*
* Returns (html_color*)
*    Pointer to the background color if specified; NULL otherwise
******************************************************************************/

HTMLColor HTMLBody::BodyBackgroundColor(void)
{
	return mBodyBackgroundColor;
}


/******************************************************************************
* HTMLBody::GetStyleFromHTML() -
*
* Parameters
*
* Description
*
* Returns ()
*
******************************************************************************/


CSSValueType HTMLBody::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_VISIBILITY:
			pValue->visibility = CSS_VISIBILITY_VISIBLE;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_FAMILY:
			pValue->name = webc_GetDefaultString(WEBC_DEFAULT_BODY_FONT_FAMILY);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_STYLE:
			pValue->fontStyle = webc_GetDefaultFontStyle(WEBC_DEFAULT_BODY_FONT_STYLE);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_VARIANT:
			break;

		case CSS_PROPERTY_FONT_WEIGHT:
			pValue->fontWeight = webc_GetDefaultFontWeight(WEBC_DEFAULT_BODY_FONT_WEIGHT);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_SIZE:
			pValue->fontSize.type = CSS_FONT_SIZE_ABSOLUTE;
			pValue->fontSize.val.absolute = webc_GetDefaultUint16(WEBC_DEFAULT_BODY_FONT_SIZE);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_WHITE_SPACE:
			pValue->whitespace = CSS_WHITE_SPACE_NORMAL;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_COLOR:
			pValue->color = TextColor();
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BACKGROUND_COLOR:
			if (mFlags & HBODY_FLAG_BGCOLOR)
			{
				pValue->color = mBodyBackgroundColor;
				return (CSS_VALUE_SPECIFIED);
			}
			else
			{
				pValue->color = webc_GetDefaultColor(WEBC_DEFAULT_BACKGROUND_COLOR);
				return (CSS_VALUE_SPECIFIED);
			}

	  #if (WEBC_SUPPORT_FRAMES)
		case CSS_PROPERTY_PADDING_TOP:    // intentional fall-through
		case CSS_PROPERTY_PADDING_BOTTOM:
			if (GetDocument())
			{
				HTMLFrame *pFrame = GetDocument()->GetFrame();
				if (pFrame)
				{
					pValue->length.type = CSS_LENGTH_UNIT_PX;
					pValue->length.val.dec = (short) pFrame->GetFrameMarginHeight();
					pValue->length.val.frac = 0;
					return (CSS_VALUE_SPECIFIED);
				}
			}
			break;

		case CSS_PROPERTY_PADDING_RIGHT:  // intentional fall-through
		case CSS_PROPERTY_PADDING_LEFT:
			if (GetDocument())
			{
				HTMLFrame *pFrame = GetDocument()->GetFrame();
				if (pFrame)
				{
					pValue->length.type = CSS_LENGTH_UNIT_PX;
					pValue->length.val.dec = (short) pFrame->GetFrameMarginWidth();
					pValue->length.val.frac = 0;
					return (CSS_VALUE_SPECIFIED);
				}
			}
			break;
	  #endif // WEBC_SUPPORT_FRAMES

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}


/*****************************************************************************/
// HTMLBody-Specific Methods
/*****************************************************************************/

/******************************************************************************
* void HTMLBody::RestoreDefaults() - Restore document defaults
*
* Parameters
*
* Description
*    Resets text color, background color, active, unselected and selected
*    link colors, font size, and background image to their default values.
*    This method is called when a document is re-loaded.
*
* Returns (void)
*    Nothing.
******************************************************************************/

void HTMLBody::RestoreDefaults(void)
{
	StyleUpdate();

	SetTextColor(webc_GetDefaultColor(WEBC_DEFAULT_TEXT_COLOR));
	SetBodyBackgroundColor(webc_GetDefaultColor(WEBC_DEFAULT_BACKGROUND_COLOR));
	SetLinkColor(webc_GetDefaultColor(WEBC_DEFAULT_LINK_COLOR));
	SetALinkColor(webc_GetDefaultColor(WEBC_DEFAULT_ACTIVE_LINK_COLOR));
	SetVLinkColor(webc_GetDefaultColor(WEBC_DEFAULT_VISITED_LINK_COLOR));

	ResetClass();

	ClearFlag(HBODY_FLAG_NOBACKGROUND);

  #if (WEBC_SUPPORT_USER_EVENTS)
	if (mpUserParam)
	{
		webc_free_string_copy(mpUserParam, __FILE__, __LINE__);
		mpUserParam = 0;
	}

	if (GetDocument() && GetDocument()->GetBrowser())
	{
		mTag = HTML_TAG_BODY;
		mpUserEventHandler = GetDocument()->GetBrowser()->GetDefaultElementEventHandler(this);
	}
 #endif

  #if(WEBC_SUPPORT_BACKGROUND_IMAGES)
	SetBackgroundImage(0);
	SetBackground(0);
  #endif

  #if (WEBC_SUPPORT_JSCRIPT)
	ClearJSEventHandlers();
	SetJSObject(0);
  #endif

	mbLoaded = 0;

	StyleUpdateChildren();
}

DisplayElement *HTMLBody::GetDisplayElement (WEBC_BOOL create)
{
	if (!mpFlowDisplay && create)
	{
		WEBC_NEW_VERBOSE(mpFlowDisplay, HTMLBodyDisplay(this),"HTMLBody::GetDisplayElement");
	}
	return mpFlowDisplay;

}

#if (WEBC_SUPPORT_FRAMES)

HTMLFrameSet* HTMLBody::GetFrameSet (void)
{
	return ((HTMLFrameSet*) mFrameSet.Get());
}

void HTMLBody::SetFrameSet (HTMLFrameSet* frameset)
{
	mFrameSet.Set(frameset);
}

#endif // WEBC_SUPPORT_FRAMES
