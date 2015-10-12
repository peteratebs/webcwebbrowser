//
// HINPUT.CPP - Methods for HTML DOM class HTMLInput
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
#include "hinput.hpp"
#include "hform.hpp"
#include "util.h"
#include "webload.hpp"
#include "webcstr.h"
#include "textstyle.hpp"
#include "htmlfind.hpp"
#include "hlabel.hpp"

/*****************************************************************************/
// Local Constants
/*****************************************************************************/

/*****************************************************************************/
// Local type definitions
/*****************************************************************************/

class HTMLLabelForFinder : public HTMLElementFinder
{
public:
	HTMLElement* forWhom;

	HTMLLabelForFinder (HTMLElement* elem)
	{
		forWhom = elem;
	}

	~HTMLLabelForFinder ()
	{
	}

	WEBC_BOOL Check (HTMLElement* elem)
	{
		HTMLLabel* label = (HTMLLabel*) elem;

		return (label->Type() == HTML_LABEL_ELEMENT && label->ForElement() == forWhom);
	}
};

/*****************************************************************************/
// Local Functions
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// HTMLInput Constructors
/*****************************************************************************/

#if (WEBC_SUPPORT_JSCRIPT)
HTMLInput::HTMLInput(HTMLTagParse *td, HTMLDocument *pDocument, JSObject *pJSObj)
	: HTMLElementContainer(td, pDocument, pJSObj)
#else
HTMLInput::HTMLInput(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElementContainer(td, pDocument)
#endif // WEBC_SUPPORT_JSCRIPT
{
	mName = 0;
	mAccessKey = 0;
	mTabIndex = 0;
	mTextStyle = 0;
	mInitialValue = 0;
	mAlt = 0;
	mInputType = HTML_INPUT_TYPE_UNKNOWN;
	mAccept = 0;
	mAlign = HTML_ALIGN_NONE;
}

HTMLInput::HTMLInput(HTMLInput &copy)
	: HTMLElementContainer(copy)
{
	mName = 0;
	SetName(copy.mName);

	mAccessKey = copy.mAccessKey;
	mTabIndex = copy.mTabIndex;

	mTextStyle = 0;
	if(copy.mTextStyle)
	{
		mTextStyle = copy.mTextStyle;
		copy.mTextStyle->claim();
	}

	mInitialValue = 0;
	if(copy.mInitialValue)
		mInitialValue = webc_malloc_string_copy(Value(), __FILE__, __LINE__);

	mAlt = 0;
	SetAlt(copy.mAlt);

	mInputType = copy.mInputType;

	mAccept = 0;
	SetAccept(copy.mAccept);
	mAlign = copy.mAlign;
}



/*****************************************************************************/
// HTMLInput Destructor
/*****************************************************************************/

HTMLInput::~HTMLInput(void)
{
	SetForm(WEBC_NULL);

	webc_free_string_copy(mName, __FILE__, __LINE__);

	if (mTextStyle)
	{
		mTextStyle->release();
	}

	if (mInitialValue)
	{
		webc_free_string_copy(mInitialValue, __FILE__, __LINE__);
	}
	if(mAlt)
	{
		webc_free_string_copy(mAlt, __FILE__, __LINE__);
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

HTMLEventStatus HTMLInput::DefaultEvent(HTMLEvent *pEvent)
{
HTMLEvent e;

    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	switch (pEvent->type)
	{
		case HTML_EVENT_FOCUS:
			if (mInitialValue)
			{
				webc_free_string_copy(mInitialValue, __FILE__, __LINE__);
			}
			mInitialValue = webc_malloc_string_copy(Value(), __FILE__, __LINE__);

			if (Name() || Id())
			{
				HTMLDocument* doc = GetDocument();
				HTMLLabelForFinder f(this);

				HTMLElement* label = doc->FindElement(&f);
				if (label)
				{
					label->StyleUpdateChildren(NOTIFY_DISPLAY_STYLE_CHANGE, NOTIFY_PARENT_DISPLAY_STYLE_CHANGE);
				}
			}
			break;

		case HTML_EVENT_UNFOCUS:
			if (Value() && mInitialValue)
			{
				if (webc_strcmp(Value(), mInitialValue))
				{
					webc_free_string_copy(mInitialValue, __FILE__, __LINE__);
					mInitialValue = 0;
					e.type = HTML_EVENT_CHANGE;
					return (Event(&e));
				}
			}

			if (Name() || Id())
			{
				HTMLDocument* doc = GetDocument();
				HTMLLabelForFinder f(this);

				HTMLElement* label = doc->FindElement(&f);
				if (label)
				{
					label->StyleUpdateChildren(NOTIFY_DISPLAY_STYLE_CHANGE, NOTIFY_PARENT_DISPLAY_STYLE_CHANGE);
				}
			}
			break;

		case HTML_EVENT_MOUSEDOWN:
		case HTML_EVENT_MOUSEUP:
			if(Disabled())
				return (HTML_EVENT_STATUS_HALT);
			break;

		default:
			// ignore other event types
			break;
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

HTMLEventStatus HTMLInput::JSEvent(HTMLEvent *pEvent)
{
	if(Disabled())
		return HTML_EVENT_STATUS_DONE;
	else
		return HTMLElement::JSEvent(pEvent);
}

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

int HTMLInput::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_NAME:
			SetName(value);
			return (1);

		case HTML_ATTRIB_VALUE:
			SetValue(value);
			SetDefValue(Value());
			return (1);

		case HTML_ATTRIB_SIZE:
			SetSize(webc_atoi(value));
			return (1);

		case HTML_ATTRIB_TABINDEX:
		{
			long i = webc_atoi(value);
			SetTabIndex(UTIL_SAFE_LONG_TO_SHORT(i));
			return (1);
		}

		case HTML_ATTRIB_MAXLENGTH:
			SetMaxLength(webc_atoi(value));
			return (1);

		case HTML_ATTRIB_DISABLED:
			SetFlag(HINPUT_FLAG_DISABLED);
			return (1);

		case HTML_ATTRIB_READONLY:
			SetFlag(HINPUT_FLAG_READONLY);
			return (1);

		case HTML_ATTRIB_NOTAB:
			SetFlag(HINPUT_FLAG_NOTAB);
			return (1);

		case HTML_ATTRIB_ALT:
			SetAlt(value);
			return (1);
		case HTML_ATTRIB_ACCEPT:
			SetAccept(value);
			return (1);
		case HTML_ATTRIB_ACCESSKEY:
			SetAccessKey(value[0]);
			return (1);

		case HTML_ATTRIB_ALIGN:
			SetHtmlAlign(HTML_ParseAlignType(value, 0));
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

WEBC_UINT16 HTMLInput::AccessKey(void)
{
	return (mAccessKey);
}

WEBC_CHAR *HTMLInput::Name(void)
{
	return (mName);
}

WEBC_CHAR *HTMLInput::Accept(void)
{
	return (mAccept);
}

void HTMLInput::SetAccessKey(WEBC_UINT16 key)
{
	mAccessKey = key;
}

void HTMLInput::SetName(const WEBC_CHAR *pName)
{
	if(!pName)
		return;
	WEBC_CHAR *pStr = webc_malloc_string_copy(pName, __FILE__, __LINE__);
	webc_free_string_copy(mName, __FILE__, __LINE__);
	mName = pStr;
}

void HTMLInput::SetAccept(const WEBC_CHAR *pAccept)
{
	if(!pAccept)
		return;
	WEBC_CHAR *pStr = webc_malloc_string_copy(pAccept, __FILE__, __LINE__);
	webc_free_string_copy(mAccept, __FILE__, __LINE__);
	mAccept = pStr;
}

void HTMLInput::SetHtmlAlign(WEBC_UINT8 bAlign)
{
	SetFlag(HDIV_FLAG_ALIGN_SET);
	mAlign = bAlign;
}

void HTMLInput::SetTabIndex(short iIndex)
{
	mTabIndex = iIndex;
}

short HTMLInput::TabIndex(void)
{
	if ((GetUnitType() == TU_TYPE_NODISPLAY) ||
	    (GetVisibility() == TU_VISIBILITY_HIDE) ||
	    (mFlags & (HINPUT_FLAG_NOTAB | HINPUT_FLAG_DISABLED)))
	{
		return (-1);
	}

	return (mTabIndex);
}

void HTMLInput::SetReadOnly(WEBC_BOOL on)
{
	if (on)
	{
		SetFlag(HINPUT_FLAG_READONLY);
	}
	else
	{
		ClearFlag(HINPUT_FLAG_READONLY);
	}
}

WEBC_BOOL HTMLInput::ReadOnly(void)
{
	return ((mFlags & HINPUT_FLAG_READONLY)? 1 : 0);
}

void HTMLInput::SetDisabled(WEBC_BOOL d)
{
	DisplayElement *pDisplay = GetDisplayElement(WEBC_FALSE);
	if (d)
	{
		if(pDisplay)
		{
			pDisplay->Disable();
		}

		SetFlag(HINPUT_FLAG_DISABLED);
	}
	else
	{
		if(pDisplay)
		{
			pDisplay->Enable();
		}

		ClearFlag(HINPUT_FLAG_DISABLED);
	}
}

void HTMLInput::SetAlt(const WEBC_CHAR *pAlt)
{
	if(!pAlt)
		return;

	WEBC_CHAR *pStr = webc_malloc_string_copy(pAlt, __FILE__, __LINE__);
	webc_free_string_copy(mAlt, __FILE__, __LINE__);
	mAlt = pStr;
}

ITextDisplayStyle* HTMLInput::GetTextDisplayStyle (WEBC_BOOL create)
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

void HTMLInput::InheritedStyleUpdate (void)
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
// HTMLInput-Specific Methods
/*****************************************************************************/

#if (WEBC_SUPPORT_STYLE_SHEETS)
int HTMLInput::InPseudoClass(CSSPseudoClass ps)
{
	switch (ps)
	{
		case CSS_PSEUDO_CLASS_ACTIVE:
			return ((mFlags & HINPUT_FLAG_ACTIVE) != 0);

		case CSS_PSEUDO_CLASS_HOVER:
			return ((mFlags & HINPUT_FLAG_HOVER) != 0);

		case CSS_PSEUDO_CLASS_FOCUS:
			return ((mFlags & HELEM_FLAG_HAS_FOCUS) != 0);

		case CSS_PSEUDO_CLASS_NONE:
			return (1);

		default:
			break;
	}

	return (0);
}
#endif

/******************************************************************************
* HTMLInput::SetForm() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLInput::SetForm(HTMLForm* pForm)
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

CSSValueType HTMLInput::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_FLOAT:
			switch (mAlign)
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
			switch (mAlign)
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
	}
	return (CSS_VALUE_UNKNOWN);
}
