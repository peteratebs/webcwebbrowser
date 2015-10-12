//
// HRESET.CPP - Methods for HTML DOM class HTMLInputReset
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
#include "hreset.hpp"
#include "hform.hpp"
#include "util.h"
#include "webload.hpp"
#include "wcconst.hpp"
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
// HTMLInputReset Constructors
/*****************************************************************************/

HTMLInputReset::HTMLInputReset(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLInputButton(td, pDocument)
{
	SetValue(WEBC_STR_RESET);
}


/*****************************************************************************/
// HTMLInputReset Destructor
/*****************************************************************************/

HTMLInputReset::~HTMLInputReset(void)
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
void HTMLInputReset::Refresh(PegThing *pThing)
{
}
*/

HTMLEventStatus HTMLInputReset::DefaultEvent(HTMLEvent *pEvent)
{
HTMLEvent e;
HTMLForm *pForm = Form();
HTMLDocument *pDoc = GetDocument();
int x = pEvent->data.position.x, y = pEvent->data.position.y;
WebKey key;

    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	switch (pEvent->type)
	{
		case HTML_EVENT_CLICK:
			if (pForm)
			{
				e.type = HTML_EVENT_RESET;
				e.data.position.x = x;
				e.data.position.y = y;
				e.elem = (HELEMENT_HANDLE)this;

				return (pForm->Event(&e));
			}
			return (HTML_EVENT_STATUS_DONE);

		case HTML_EVENT_KEYDOWN:
		{
			HTMLEventStatus retval = DefaultInputKeyHandler(pEvent);
			if (retval != HTML_EVENT_STATUS_CONTINUE)
			{
				return (retval);
			}
			break;
		}

		case HTML_EVENT_KEYPRESS:
			key = pEvent->data.key;
			if (pForm && key ==  '\r')
			{
				e.type = HTML_EVENT_RESET;
				e.data.position.x = x;
				e.data.position.y = y;
				e.elem = (HELEMENT_HANDLE)this;

				HTMLEventStatus status = pForm->Event(&e);
				if (status == HTML_EVENT_STATUS_HALT)
				{
					return (status);
				}
				return (HTML_EVENT_STATUS_CONTINUE);
			}
			break;

		default:
			// ignore other event types
			break;
	}

	return (HTMLInputButton::DefaultEvent(pEvent));
}

/*
HTMLDocument *HTMLInputReset::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLInputReset::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLInputReset::Reset(void)
{
}
*/
/*
int HTMLInputReset::SetProperty(HTMLAttributeType Property, PFCHAR value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_VALUE:
			((PegTextButton*)mpThing)->DataSet(value);
			return (1);

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
// HTMLInputReset-Specific Methods
/*****************************************************************************/
