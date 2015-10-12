//
// HCHKBOX.CPP - Methods for HTML DOM class HTMLCheckbox
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
#include "hchkbox.hpp"
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
// HTMLCheckbox Constructors
/*****************************************************************************/

HTMLCheckbox::HTMLCheckbox(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLInput(td, pDocument)
{
	mDefaultValue = WEBC_NULL;
	mDefaultChecked = WEBC_FALSE;
	mCheckBoxDisplay = WEBC_NULL;
	mInputType = HTML_INPUT_TYPE_CHECKBOX;

	// fix for B6 - make default value for checkboxes "on"
	SetValue(WEBC_STR_ON);
}


/*****************************************************************************/
// HTMLCheckbox Destructor
/*****************************************************************************/

HTMLCheckbox::~HTMLCheckbox(void)
{
	webc_free_string_copy(mDefaultValue, __FILE__, __LINE__);

	if (mCheckBoxDisplay)
	{
		WEBC_DELETE(mCheckBoxDisplay);
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

/*
void HTMLCheckbox::Refresh(PegThing *pThing)
{
}
*/

HTMLEventStatus HTMLCheckbox::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	switch (pEvent->type)
	{
		case HTML_EVENT_FOCUS:
		{
			HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 1);
			FindElement(&f);
			break;
		}

		case HTML_EVENT_UNFOCUS:
		{
			HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 0);
			FindElement(&f);
			break;
		}

		case HTML_EVENT_KEYDOWN:
		{
			HTMLEventStatus retval = DefaultInputKeyHandler(pEvent);
			if (retval != HTML_EVENT_STATUS_CONTINUE)
			{
				return (retval);
			}
			break;
		}

		case HTML_EVENT_MOUSEDOWN:
			if(!Disabled())
				SetChecked(!Checked());
			return (HTML_EVENT_STATUS_DONE);

		default:
			break;
	}

	return (HTMLInput::DefaultEvent(pEvent));
}

/*
HTMLDocument *HTMLCheckbox::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLCheckbox::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/


void HTMLCheckbox::Reset(void)
{
	SetChecked(mDefaultChecked);
}

int HTMLCheckbox::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_CHECKED:
			mDefaultChecked = WEBC_TRUE;
			break;

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLInput::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}

WEBC_CHAR *HTMLCheckbox::Value(void)
{
	return mDefaultValue;
}


void HTMLCheckbox::SetValue(const WEBC_CHAR *pValue)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pValue, __FILE__, __LINE__);
	webc_free_string_copy(mDefaultValue, __FILE__, __LINE__);
	mDefaultValue = pStr;
}


WEBC_BOOL HTMLCheckbox::Checked(void)
{
	HTMLCheckBoxDisplay *pCheck = GetCheckBoxDisplay(WEBC_FALSE);

	if (pCheck && pCheck->GetChecked())
	{
		return (WEBC_TRUE);
	}

	return (WEBC_FALSE);
}


void HTMLCheckbox::SetDefValue(const WEBC_CHAR *pValue)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pValue, __FILE__, __LINE__);
	webc_free_string_copy(mDefaultValue, __FILE__, __LINE__);
	mDefaultValue = pStr;
}


void HTMLCheckbox::SetChecked(WEBC_BOOL on)
{
HTMLCheckBoxDisplay *pCheck = GetCheckBoxDisplay(WEBC_FALSE);

	if (pCheck)
	{
		pCheck->SetChecked(on);
	}
}


/*****************************************************************************/
// HTMLCheckbox-Specific Methods
/*****************************************************************************/

DisplayElement *HTMLCheckbox::GetDisplayElement (WEBC_BOOL create)
{
	return (GetCheckBoxDisplay(create));
}

TextUnit *HTMLCheckbox::GetTextUnit (void)
{
	return (GetCheckBoxDisplay());
}

HTMLCheckBoxDisplay *HTMLCheckbox::GetCheckBoxDisplay (WEBC_BOOL create)
{
	if (!mCheckBoxDisplay && create)
	{
		WEBC_NEW_VERBOSE(mCheckBoxDisplay, HTMLCheckBoxDisplay(this),"GetCheckBoxDisplay");

		if (mCheckBoxDisplay)
		{
			mCheckBoxDisplay->SizeTo(22, 18);
			Reset();
		}
	}

	return (mCheckBoxDisplay);
}

