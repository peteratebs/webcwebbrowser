//
// HBUTTON.CPP - Methods for HTML DOM class HTMLButton
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
#include "hform.hpp"
#include "wgkeys.hpp"
#include "hbutton.hpp"
#include "dhbutton.hpp"
#include "htmldoc.hpp"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "textstyle.hpp"

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
// HTMLButton Constructors
/*****************************************************************************/

HTMLButton::HTMLButton(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
	mpName = 0;
	mpValue = 0;
	mpType = 0;
	mTextStyle = 0;
	miTabIndex = 0;
	mButtonState = 0;
	mInputType = HTML_INPUT_TYPE_BUTTON;
	mAccessKey = 0;
	ClearFlag(HBUTTON_FLAG_ACTIVE|HBUTTON_FLAG_DISABLED|HBUTTON_FLAG_ACTIVE);
}


/*****************************************************************************/
// HTMLButton Destructor
/*****************************************************************************/

HTMLButton::~HTMLButton(void)
{
	webc_free_string_copy(mpName, __FILE__, __LINE__);
	webc_free_string_copy(mpValue, __FILE__, __LINE__);
	webc_free_string_copy(mpType, __FILE__, __LINE__);
	if (mTextStyle)
	{
		mTextStyle->release();
		mTextStyle = 0;
	}

}


/*****************************************************************************/
// TextUnit Interface
//*****************************************************************************/


/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/



/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

HTMLEventStatus HTMLButton::DefaultEvent(HTMLEvent *pEvent)
{
HTMLBrowser *pBrowser = GetBrowser();
HTMLDocument *pDoc = GetDocument();
HTMLForm *pForm = Form();
HTMLEventStatus retval;
HTMLEvent e;

    if (pForm && GetFormEventType() == HTML_EVENT_NONE)
        pForm = 0;

    WEBC_DEBUG_ReceiveEvent(this, pEvent);

	if(Disabled())
		return (HTML_EVENT_STATUS_HALT);

	switch (pEvent->type)
	{
		case HTML_EVENT_CLICK:
            break;
	    case HTML_EVENT_MOUSEDOWN:
		    SetFlag(HBUTTON_FLAG_ACTIVE);
			StyleUpdate();
			return (HTML_EVENT_STATUS_HALT);
	    case HTML_EVENT_MOUSEUP:
			ClearFlag(HBUTTON_FLAG_ACTIVE);
			StyleUpdate();

			if (pForm)
			{
				DISPLAY_INT x=0,y=0;
				GetDocumentPosition(&x, &y);
				e.type = GetFormEventType();
				e.data.position.x = x;
				e.data.position.y = y;
				e.elem = (HELEMENT_HANDLE)this;

				return (pForm->Event(&e));
			}
			{
            HTMLEvent clickEvent = *pEvent;
            clickEvent.type = HTML_EVENT_CLICK;
			GetDisplayElement()->Event(&clickEvent);
			return (HTML_EVENT_STATUS_HALT);
            }


		case HTML_EVENT_MOUSEOVER:
	  		mButtonState |= HBUTTON_STATE_HOVER;
			StyleUpdate();
			return (HTML_EVENT_STATUS_CONTINUE);

		case HTML_EVENT_FOCUS:

			mButtonState |= HBUTTON_STATE_FOCUS;
			StyleUpdate();
			return (HTML_EVENT_STATUS_CONTINUE);

		case HTML_EVENT_MOUSEOUT:

			mButtonState &= ~HBUTTON_STATE_HOVER;
			StyleUpdate();
			return (HTML_EVENT_STATUS_CONTINUE);

		case HTML_EVENT_UNFOCUS:

			mButtonState &= ~HBUTTON_STATE_FOCUS;
			StyleUpdate();
			return (HTML_EVENT_STATUS_CONTINUE);


		case HTML_EVENT_KEYDOWN:
			retval = DefaultInputKeyHandler(pEvent);
			if (retval != HTML_EVENT_STATUS_CONTINUE)
			{
				return (retval);
			}
			break;

		case HTML_EVENT_KEYUP:
			if (pForm && pEvent->data.key ==  WGK_CR)
			{
				DISPLAY_INT x=0,y=0;
				GetDocumentPosition(&x, &y);
				e.type = GetFormEventType();
				e.data.position.x = x;
				e.data.position.y = y;
				e.elem = (HELEMENT_HANDLE)this;

				return (pForm->Event(&e));
			}
			break;

		default:
			// ignore other events
			break;
	}

	return (HTMLFlow::DefaultEvent(pEvent));
}

/*
HTMLElement *HTMLButton::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLButton::Reset(void)
{
}
*/

int HTMLButton::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_ACCESSKEY:
			SetAccessKey(value[0]);
			return(1);
		case HTML_ATTRIB_NAME:
			SetName(value);
			return (1);

		case HTML_ATTRIB_VALUE:
			SetValue(value);
			return (1);

		case HTML_ATTRIB_TYPE:
			SetType(value);
			return (1);

		case HTML_ATTRIB_TABINDEX:
		{
			long i = webc_atoi(value);
			SetTabIndex(UTIL_SAFE_LONG_TO_SHORT(i));
			return (1);
		}
		case HTML_ATTRIB_NOTAB:
			SetFlag(HBUTTON_FLAG_NOTAB);
			return (1);
		case HTML_ATTRIB_DISABLED:
			SetFlag(HBUTTON_FLAG_DISABLED);
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
* void HTMLButton::SetName(char *pName) - Set name attribute
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

void HTMLButton::SetName(const WEBC_CHAR *pName)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pName, __FILE__, __LINE__);

	webc_free_string_copy(mpName, __FILE__, __LINE__);
	mpName = pStr;
}

/******************************************************************************
* void HTMLButton::SetType(char *pvalue) - Set type attribute
*
* Parameters
*
* Description
*    Creates a local copy of the given string.
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLButton::SetType(const WEBC_CHAR *pType)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pType, __FILE__, __LINE__);

	webc_free_string_copy(mpType, __FILE__, __LINE__);
	mpType = pStr;
}

/******************************************************************************
* void HTMLButton::SetValue(char *pvalue) - Set type attribute
*
* Parameters
*
* Description
*    Creates a local copy of the given string.
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLButton::SetValue(const WEBC_CHAR *pValue)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pValue, __FILE__, __LINE__);

	webc_free_string_copy(mpValue, __FILE__, __LINE__);
	mpValue = pStr;
}


/******************************************************************************
* char* HTMLButton::Name() - Access name attribute
*
* Parameters
*
* Description
*
* Returns (char*)
*    NULL-terminated string; the anchor's name
******************************************************************************/

WEBC_CHAR *HTMLButton::Name(void)
{
	return (mpName);
}


/******************************************************************************
* char* HTMLButton::Type() - Access name attribute
*
* Parameters
*
* Description
*
* Returns (char*)
*    NULL-terminated string; the anchor's name
******************************************************************************/

WEBC_CHAR *HTMLButton::GetType(void)
{
	return (mpType);
}
WebColor HTMLButton::GetBackgroundColor (void)
{
    return HTML_RGBAToColor(0xb6,0xb6,0xb6,0);  // a light gray color
}

WEBC_BOOL HTMLButton::HasBackgroundColor (void)
{
	return (WEBC_TRUE);
}


HTMLEventType HTMLButton::GetFormEventType(void)
{
    if(GetType())
    {
        if (webc_c_strcmp(GetType(), "submit") == 0)
	        return(HTML_EVENT_SUBMIT);
        if (webc_c_strcmp(GetType(), "reset") == 0)
	        return(HTML_EVENT_RESET);
    }
	return(HTML_EVENT_NONE);
}
/******************************************************************************
* char* HTMLButton::Value() - Access name attribute
*
* Parameters
*
* Description
*
* Returns (char*)
*    NULL-terminated string; the anchor's name
******************************************************************************/

WEBC_CHAR *HTMLButton::Value(void)
{
	return (mpValue);
}

/******************************************************************************
* void HTMLButton::SetTabIndex(short iIndex) - Set tab index
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

void HTMLButton::SetTabIndex(short iIndex)
{
	miTabIndex = iIndex;
}

/******************************************************************************
* short HTMLButton::TabIndex() - Access tab index
*
* Parameters
*
* Description
*
* Returns (short)
*    Order of this element in the tab-key sequence
******************************************************************************/

short HTMLButton::TabIndex(void)
{
	if ((GetUnitType() == TU_TYPE_NODISPLAY) ||
	    (GetVisibility() == TU_VISIBILITY_HIDE) ||
	    (mFlags & (HBUTTON_FLAG_NOTAB | HBUTTON_FLAG_DISABLED)))
	{
		return (-1);
	}

	return (miTabIndex);
}

/******************************************************************************
* HTMLButton::GetWidth() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
void HTMLButton::GetWidth (CSSLength *width)
{
	CSSPropertyValue value;

	if (GetStyleFromCSS(CSS_PROPERTY_WIDTH, &value) != CSS_VALUE_SPECIFIED)
	{
		if (GetStyleFromHTML(CSS_PROPERTY_WIDTH, &value) != CSS_VALUE_SPECIFIED)
		{
			width->type = CSS_LENGTH_UNIT_AUTO;
			return;
		}
	}
	*width = value.length;
}

/******************************************************************************
* HTMLButton::GetHeight() -
*
* Parameters
*
*
* Description
*
* Returns ()
*
******************************************************************************/
void HTMLButton::GetHeight (CSSLength *height)
{
	CSSPropertyValue value;

	if (GetStyleFromCSS(CSS_PROPERTY_HEIGHT, &value) != CSS_VALUE_SPECIFIED)
	{
		if (GetStyleFromHTML(CSS_PROPERTY_HEIGHT, &value) != CSS_VALUE_SPECIFIED)
		{
			height->type = CSS_LENGTH_UNIT_AUTO;
			return;
		}
	}

	*height = value.length;
}


void HTMLButton::SetDisabled(WEBC_BOOL disabled)
{ 
	if(disabled) 
		SetFlag(HBUTTON_FLAG_DISABLED); 
	else
		ClearFlag(HBUTTON_FLAG_DISABLED);
}

WEBC_BOOL HTMLButton::Disabled(void) 
{ 
	return (GetFlags() & HBUTTON_FLAG_DISABLED ? WEBC_TRUE : WEBC_FALSE); 
}

/*****************************************************************************/
// Local Constants
/*****************************************************************************/

#define WEBC_CFG_BUTTON_PADDING_LEFT          4
#define WEBC_CFG_BUTTON_PADDING_TOP           2
#define WEBC_CFG_BUTTON_PADDING_RIGHT         4
#define WEBC_CFG_BUTTON_PADDING_BOTTOM        1

#define WEBC_CFG_BUTTON_ACTIVE_PADDING_LEFT   5
#define WEBC_CFG_BUTTON_ACTIVE_PADDING_TOP    3
#define WEBC_CFG_BUTTON_ACTIVE_PADDING_RIGHT  3
#define WEBC_CFG_BUTTON_ACTIVE_PADDING_BOTTOM 0

#define WEBC_CFG_BUTTON_BORDER_WIDTH_LEFT     2
#define WEBC_CFG_BUTTON_BORDER_WIDTH_TOP      2
#define WEBC_CFG_BUTTON_BORDER_WIDTH_RIGHT    2
#define WEBC_CFG_BUTTON_BORDER_WIDTH_BOTTOM   2

#define WEBC_CFG_BUTTON_MARGIN_LEFT           0
#define WEBC_CFG_BUTTON_MARGIN_TOP            0
#define WEBC_CFG_BUTTON_MARGIN_RIGHT          0
#define WEBC_CFG_BUTTON_MARGIN_BOTTOM         0



CSSValueType HTMLButton::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_DISPLAY:
			break;
        // These are all thje same as button
		case CSS_PROPERTY_PADDING_LEFT:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = Active()? WEBC_CFG_BUTTON_ACTIVE_PADDING_LEFT : WEBC_CFG_BUTTON_PADDING_LEFT;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_PADDING_RIGHT:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = Active()? WEBC_CFG_BUTTON_ACTIVE_PADDING_RIGHT : WEBC_CFG_BUTTON_PADDING_RIGHT;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_PADDING_TOP:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = Active()? WEBC_CFG_BUTTON_ACTIVE_PADDING_TOP : WEBC_CFG_BUTTON_PADDING_TOP;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_PADDING_BOTTOM:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = Active()? WEBC_CFG_BUTTON_ACTIVE_PADDING_BOTTOM : WEBC_CFG_BUTTON_PADDING_BOTTOM;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_MARGIN_LEFT:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = WEBC_CFG_BUTTON_MARGIN_LEFT;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_MARGIN_RIGHT:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = WEBC_CFG_BUTTON_MARGIN_RIGHT;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_MARGIN_TOP:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = WEBC_CFG_BUTTON_MARGIN_TOP;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_MARGIN_BOTTOM:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = WEBC_CFG_BUTTON_MARGIN_BOTTOM;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_FAMILY:
			pValue->name = webc_GetDefaultString(WEBC_DEFAULT_BUTTON_FONT_FAMILY);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_STYLE:
			pValue->fontStyle = webc_GetDefaultFontStyle(WEBC_DEFAULT_BUTTON_FONT_STYLE);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_VARIANT:
			break;

		case CSS_PROPERTY_FONT_WEIGHT:
			pValue->fontWeight =  webc_GetDefaultFontWeight(WEBC_DEFAULT_BUTTON_FONT_WEIGHT);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_SIZE:
			pValue->fontSize.type = CSS_FONT_SIZE_ABSOLUTE;
			pValue->fontSize.val.absolute =  webc_GetDefaultUint16(WEBC_DEFAULT_BUTTON_FONT_SIZE); 
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_COLOR:
			pValue->color = HTML_RGBAToColor(0,0,0,0); // black
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BACKGROUND_COLOR:
			pValue->color = HTML_RGBAToColor(0xb6,0xb6,0xb6,0);  // a light gray color
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_BOTTOM_STYLE: // intentional fall-through
		case CSS_PROPERTY_BORDER_LEFT_STYLE:   // intentional fall-through
		case CSS_PROPERTY_BORDER_RIGHT_STYLE:  // intentional fall-through
		case CSS_PROPERTY_BORDER_TOP_STYLE:
			if (Active())
			{
				// tbd - if num colors is less than 4 or 3D widgets is disabled,
				//  style should be CSS_BORDER_STYLE_SOLID
				pValue->borderStyle = CSS_BORDER_STYLE_INSET;
			}
			else
			{
				pValue->borderStyle = CSS_BORDER_STYLE_OUTSET;
			}
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_BOTTOM_COLOR: // intentional fall-through
		case CSS_PROPERTY_BORDER_LEFT_COLOR:   // intentional fall-through
		case CSS_PROPERTY_BORDER_RIGHT_COLOR:  // intentional fall-through
		case CSS_PROPERTY_BORDER_TOP_COLOR:
            if (mButtonState & HBUTTON_STATE_HOVER)
            {
			    HTML_ColorSet(pValue->color, 0x0, 0x0, 0x0, 0); // black
			}
            else
            {
			    pValue->color = HTML_RGBAToColor(0xb6,0xb6,0xb6,0);  // a light gray color
//			    HTML_ColorSet(pValue->color, 0x80, 0x80, 0x80, 0);
			}
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_BOTTOM_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = WEBC_CFG_BUTTON_BORDER_WIDTH_BOTTOM;
			pValue->borderThickness.val.length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_LEFT_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = WEBC_CFG_BUTTON_BORDER_WIDTH_LEFT;
			pValue->borderThickness.val.length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_RIGHT_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = WEBC_CFG_BUTTON_BORDER_WIDTH_RIGHT;
			pValue->borderThickness.val.length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_TOP_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = WEBC_CFG_BUTTON_BORDER_WIDTH_TOP;
			pValue->borderThickness.val.length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_OUTLINE_STYLE:
  			pValue->borderStyle = (mFlags & HELEM_FLAG_HAS_FOCUS)? CSS_BORDER_STYLE_DOTTED : CSS_BORDER_STYLE_NONE;
		  	return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_OUTLINE_COLOR:
			HTML_ColorSet(pValue->color, 0, 0, 0, 0);
		  	return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_OUTLINE_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = 1;
			pValue->borderThickness.val.length.val.frac = 0;
		  	return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_WIDTH:
            break;

		case CSS_PROPERTY_HEIGHT:
            break;

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}

/******************************************************************************
* HTMLButton::SetForm() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLButton::SetForm(HTMLForm* pForm)
{
	if (Form())
	{
		Form()->RemoveField(this);
	}
	HTMLElement::SetForm(pForm);
	if (Form())
	{
		Form()->AddField(this);
	}
}

ITextDisplayStyle* HTMLButton::GetTextDisplayStyle (WEBC_BOOL create)
{
	if (!mTextStyle && create)
	{
		HTMLDocumentTextDisplayStyleFactory factory(GetDocument());

		mTextStyle = factory.newTextStyle (
				this->GetFont(),
				this->GetColor(),
				this->GetBackgroundColor(),
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

void HTMLButton::InheritedStyleUpdate (void)
{
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


/*****************************************************************************/
// HTMLButton-Specific Methods
/*****************************************************************************/

HTMLFlowDisplay* HTMLButton::GetFlowDisplay (WEBC_BOOL create)
{
	if (!mpFlowDisplay && create)
	{
		WEBC_NEW_VERBOSE(mpFlowDisplay, HTMLButtonDisplay(this),"HTMLButton::GetFlowDisplay");
	}

	return (mpFlowDisplay);
}

WEBC_BOOL HTMLButton::Active (void)
{
	return ((GetFlags() & HBUTTON_FLAG_ACTIVE) != 0);
}


#if (WEBC_SUPPORT_STYLE_SHEETS)
int HTMLButton::InPseudoClass(CSSPseudoClass ps)
{
	switch (ps)
	{
		case CSS_PSEUDO_CLASS_ACTIVE:
			return ((mButtonState & (HBUTTON_STATE_ACTIVE|HBUTTON_STATE_FOCUS)) != 0);

		case CSS_PSEUDO_CLASS_VISITED:
			return (0);

		case CSS_PSEUDO_CLASS_HOVER:
			return ((mButtonState & HBUTTON_STATE_HOVER) != 0);

		case CSS_PSEUDO_CLASS_LINK:
			return (0);

		case CSS_PSEUDO_CLASS_NONE:
			return (1);

		default:
			break;
	}

	return (0);
}
#endif // WEBC_SUPPORT_STYLE_SHEETS
