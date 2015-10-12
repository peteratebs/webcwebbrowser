//
// HSUBMIT.CPP - Methods for HTML DOM class HTMLInputSubmit
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
#include "hsubmit.hpp"
#include "hform.hpp"
#include "util.h"
#include "webload.hpp"
#include "wcconst.hpp"
#include "wgkeys.hpp"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "htmlfind.hpp"


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
// HTMLInputSubmit Constructors
/*****************************************************************************/

HTMLInputSubmit::HTMLInputSubmit(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLInputButton(td, pDocument)
{
	SetValue((WEBC_CHAR *) WEBC_STR_SUBMIT);
}


/*****************************************************************************/
// HTMLInputSubmit Destructor
/*****************************************************************************/

HTMLInputSubmit::~HTMLInputSubmit(void)
{
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
void HTMLInputSubmit::Refresh(PegThing *pThing)
{
}
*/

HTMLEventStatus HTMLInputSubmit::DefaultEvent(HTMLEvent *pEvent)
{
HTMLEvent e;
HTMLForm *pForm = Form();
HTMLBrowser *pBrowser = GetBrowser();
HTMLDocument *pDoc = GetDocument();
int x = pEvent->data.position.x, y = pEvent->data.position.y;
WebKey key;
HTMLEventStatus retval;

    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	switch (pEvent->type)
	{
		case HTML_EVENT_CLICK:
			if (pForm)
			{
				e.type = HTML_EVENT_SUBMIT;
				e.data.position.x = x;
				e.data.position.y = y;
				e.elem = (HELEMENT_HANDLE)this;

				return (pForm->Event(&e));
			}
			break;

		case HTML_EVENT_KEYDOWN:
			retval = DefaultInputKeyHandler(pEvent);
			if (retval != HTML_EVENT_STATUS_CONTINUE)
			{
				return (retval);
			}
			break;

		case HTML_EVENT_KEYUP:
			key = pEvent->data.key;
			if (pForm && key ==  WGK_CR)
			{
				e.type = HTML_EVENT_SUBMIT;
				e.data.position.x = x;
				e.data.position.y = y;
				e.elem = (HELEMENT_HANDLE)this;

				return (pForm->Event(&e));
			}
			break;

		case HTML_EVENT_FOCUS:
			if (pBrowser)
			{
				WEBC_CHAR str[256 + 12];
				webc_c_strcpy(str, "Submit to ");
				if (pForm && pForm->Action())
				{
					webc_strncat(str, pForm->Action(), 256);
				}
				pBrowser->SetTempStatus(str, this);
			}
			break;

		case HTML_EVENT_UNFOCUS:
			if (pBrowser)
			{
				pBrowser->ClearTempStatus(this);
			}
			break;

		default:
			// ignore other event types
			break;
	}

	return (HTMLInputButton::DefaultEvent(pEvent));
}

/*
HTMLDocument *HTMLInputSubmit::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLInputSubmit::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
int HTMLInputSubmit::SetProperty(HTMLAttributeType Property, PFCHAR value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		default:
			break;
	}

	if (call_base)
	{
		return (HTMLInput::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}
*/


/*****************************************************************************/
// HTMLInputSubmit-Specific Methods
/*****************************************************************************/
