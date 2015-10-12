//
// HHR.CPP - Methods for HTML DOM class HTMLHorizontalRule
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
#include "hhr.hpp"
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
// HTMLHorizontalRule Constructors
/*****************************************************************************/

HTMLHorizontalRule::HTMLHorizontalRule(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElement(td, pDocument)
{
	mbAlign = TU_ALIGN_CENTER;
	HTML_LengthSet(mWidth, 100, HTML_LENGTH_UNIT_PERCENT);
	miSize = WEBC_HR_HEIGHT;
	mpHRDisplay = 0;
}


/*****************************************************************************/
// HTMLHorizontalRule Destructor
/*****************************************************************************/

HTMLHorizontalRule::~HTMLHorizontalRule(void)
{
	if (mpHRDisplay)
	{
		WEBC_DELETE(mpHRDisplay);
	}
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/

WEBC_UINT16 HTMLHorizontalRule::GetBreak (void)
{
	return (TU_BREAK_BOTH);
}


DISPLAY_INT HTMLHorizontalRule::GetMinWidth(void)
{
	return (0);
}


DISPLAY_INT HTMLHorizontalRule::GetMaxWidth(void)
{
	return (0);
}


WEBC_UINT8 HTMLHorizontalRule::GetTextAlign(void)
{
	return (mbAlign);
}


DISPLAY_INT HTMLHorizontalRule::GetTopMargin(void)
{
	return (6);
}


DISPLAY_INT HTMLHorizontalRule::GetBottomMargin(void)
{
	return (6);
}


/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/



/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

/*
void HTMLHorizontalRule::Update(Observable o, WEBC_PFBYTE data)
{
}
*/
/*
int HTMLHorizontalRule::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLElement *HTMLHorizontalRule::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLHorizontalRule::Reset(void)
{
}
*/

int HTMLHorizontalRule::SetProperty (
		HTMLAttributeType Property,
		WEBC_CHAR *value,
		HTMLDocument *pDocument,
		int call_base
	)
{
HTMLAlignType align;

	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_ALIGN:
			align = HTML_ParseAlignType(value, 0);
			switch (align)
			{
				case HTML_ALIGN_CENTER:
					SetTextAlign(TU_ALIGN_CENTER);
					break;

				case HTML_ALIGN_RIGHT:
					SetTextAlign(TU_ALIGN_RIGHT);
					break;

				case HTML_ALIGN_LEFT:
				default:
					SetTextAlign(TU_ALIGN_LEFT);
					break;
			}
			return (1);

		case HTML_ATTRIB_WIDTH:
			HTML_ParseLength(value, 0, &mWidth);
			return (1);

		case HTML_ATTRIB_SIZE:
		{
			long i;
			HTML_ParseDecNumber(value, 12, &i);
			miSize = (WEBC_UINT8) EBSMAX(0, i);
			return (1);
		}

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLElement::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}


/*****************************************************************************/
// HTMLHorizontalRule-Specific Methods
/*****************************************************************************/

void HTMLHorizontalRule::SetTextAlign(WEBC_UINT8 bAlign)
{
	mbAlign = bAlign;
}

WEBC_UINT16 HTMLHorizontalRule::RuleSize(void)
{
	return (miSize);
}

HTMLLength HTMLHorizontalRule::RuleWidth(void)
{
	return (mWidth);
}

DisplayElement *HTMLHorizontalRule::GetDisplayElement (WEBC_BOOL create)
{
	return (GetHRDisplay(create));
}

TextUnit *HTMLHorizontalRule::GetTextUnit (void)
{
	return (GetHRDisplay());
}

HTMLHRDisplay *HTMLHorizontalRule::GetHRDisplay (WEBC_BOOL create)
{
	if (!mpHRDisplay && create)
	{
		WEBC_NEW_VERBOSE(mpHRDisplay, HTMLHRDisplay(this),"HTMLHRDisplay");
	}

	return (mpHRDisplay);
}
