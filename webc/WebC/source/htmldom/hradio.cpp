//
// HRADIO.CPP - Methods for HTML DOM class HTMLRadioButton
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
#include "hradio.hpp"
#include "util.h"
#include "webload.hpp"
#include "htmlfind.hpp"
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
// HTMLRadioButton Constructors
/*****************************************************************************/

HTMLRadioButton::HTMLRadioButton(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLInput(td, pDocument)
{
	mpRadioDisplay = 0;
	mpDefValue = 0;
	mpValue = 0;
	miDefChecked = 0;

	mInputType = HTML_INPUT_TYPE_RADIO;

	// radio group is a circular list
	mpGroupNext = this;
	mpGroupPrev = this;
	
	//  make default value for radio buttons "on"
	SetValue(WEBC_STR_ON);
}


/*****************************************************************************/
// HTMLRadioButton Destructor
/*****************************************************************************/

HTMLRadioButton::~HTMLRadioButton(void)
{
	webc_free_string_copy(mpDefValue, __FILE__, __LINE__);
	webc_free_string_copy(mpValue, __FILE__, __LINE__);

	// unlink myself from my group of radio buttons
	GroupUnlink();

	if (mpRadioDisplay)
	{
		WEBC_DELETE(mpRadioDisplay);
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


void HTMLRadioButton::Update(WEBC_PFBYTE o, WEBC_PFBYTE data)
{
	if ((mpGroupNext == this) || webc_stricmp(Name(), mpGroupNext->Name()))
	{
		// in case we are in the wrong group
		GroupUnlink();

		HTMLDocument *pDoc = GetDocument();
		if (pDoc)
		{
			if (Name())
			{
				// try to find the group we belong to; if we find it, link ourself in
				HTMLNameTypeFinder f (Name(), Type());
				HTMLRadioButton *pRadio = (HTMLRadioButton *) pDoc->FindElement(&f);

				// if we are the first in the group, find the next one (indices
				//  begin with 0)
				if (pRadio == this)
				{
					HTMLIdNameTypeIndexFinder f2 (0, Name(), Type(), 1);
					pRadio = (HTMLRadioButton *) pDoc->FindElement(&f2);
				}

				if (pRadio)
				{
					// link into our group
					GroupInsertBefore(pRadio);
					if (Checked())
					{
						UnselectSiblings();
					}
				}
			}
		}
	}
}


/*
void HTMLRadioButton::Refresh(PegThing *pThing)
{
}
*/

HTMLEventStatus HTMLRadioButton::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	switch (pEvent->type)
	{
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
				SetChecked(WEBC_TRUE);
			return (HTML_EVENT_STATUS_DONE);

		case HTML_EVENT_FOCUS:
		{
			HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 1);
			FindElement(&f);
			SetFlag(HINPUT_FLAG_ACTIVE);
			Update(0,0);
			break;
		}

		case HTML_EVENT_UNFOCUS:
		{
			HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 0);
			FindElement(&f);
			ClearFlag(HINPUT_FLAG_ACTIVE);
			Update(0,0);
			break;
		}

		default:
			break;
	}

	return (HTMLInput::DefaultEvent(pEvent));
}

/*
HTMLDocument *HTMLRadioButton::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLRadioButton::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/


void HTMLRadioButton::Reset(void)
{
	SetChecked(miDefChecked);
	SetValue(DefValue());
}


int HTMLRadioButton::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_VALUE:
			SetValue(value);
			break;

		case HTML_ATTRIB_CHECKED:
			miDefChecked = 1;
			SetChecked(1);
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


WEBC_BOOL HTMLRadioButton::Checked(void)
{
HTMLRadioButtonDisplay *pDisplay = GetRadioButtonDisplay();

	return (pDisplay && pDisplay->GetSelected());
}


void HTMLRadioButton::SetDefValue(const WEBC_CHAR *pValue)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(pValue, __FILE__, __LINE__);
	webc_free_string_copy(mpDefValue, __FILE__, __LINE__);
	mpDefValue = pStr;
}


void HTMLRadioButton::SetChecked(WEBC_BOOL on)
{
HTMLRadioButtonDisplay *pDisplay = GetRadioButtonDisplay();

	if (pDisplay)
	{
		pDisplay->SetSelected(on);
	}
}

WEBC_CHAR *HTMLRadioButton::Value(void)
{
	return (mpValue);
}

WEBC_CHAR *HTMLRadioButton::DefValue(void)
{
	return mpDefValue;
}

void HTMLRadioButton::SetValue(const WEBC_CHAR *pValue)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(pValue, __FILE__, __LINE__);
	webc_free_string_copy(mpValue, __FILE__, __LINE__);
	mpValue = pStr;

	if (!DefValue())
	{
		SetDefValue(pValue);
	}
}


/*****************************************************************************/
// HTMLRadioButton-Specific Methods
/*****************************************************************************/

void HTMLRadioButton::GroupInsertBefore (HTMLRadioButton *pRadio)
{
	if (pRadio)
	{
		GroupUnlink();
		mpGroupPrev = pRadio->mpGroupPrev;
		mpGroupPrev->mpGroupNext = this;
		pRadio->mpGroupPrev = this;
		mpGroupNext = pRadio;
	}
}

void HTMLRadioButton::GroupUnlink (void)
{
	if (mpGroupNext != this)
	{
		mpGroupNext->mpGroupPrev = mpGroupPrev;
	}
	if (mpGroupPrev != this)
	{
		mpGroupPrev->mpGroupNext = mpGroupNext;
	}
}

void HTMLRadioButton::UnselectSiblings (void)
{
HTMLRadioButton *pCurrent = mpGroupNext;

	while (pCurrent != this)
	{
		pCurrent->SetChecked(0);
		pCurrent = pCurrent->mpGroupNext;
	}
}

DisplayElement *HTMLRadioButton::GetDisplayElement (WEBC_BOOL create)
{
	return (GetRadioButtonDisplay(create));
}

TextUnit *HTMLRadioButton::GetTextUnit (void)
{
	return (GetRadioButtonDisplay());
}

HTMLRadioButtonDisplay *HTMLRadioButton::GetRadioButtonDisplay (WEBC_BOOL create)
{
	if (!mpRadioDisplay && create)
	{
		WEBC_NEW_VERBOSE(mpRadioDisplay, HTMLRadioButtonDisplay(this),"HTMLRadioButtonDisplay");
		if (mpRadioDisplay)
		{
			mpRadioDisplay->SizeTo(22, 18);
			Reset();
		}
	}

	return (mpRadioDisplay);
}

