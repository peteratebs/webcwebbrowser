//
// HSELECT.CPP - Methods for HTML DOM class HTMLSelect
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
#include "hselect.hpp"
#include "hform.hpp"
#include "hoption.hpp"
#include "util.h"
#include "webload.hpp"
#include "htmlfind.hpp"
#include "wgkeys.hpp"
#include "webcmem.h"

/* Patches to correct select box formatting errors */
#define SELECT_INCLUDE_PADDING_HTML   0
#define SELECT_INCLUDE_MARGIN_HTML    1

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
// HTMLSelect Constructors
/*****************************************************************************/

HTMLSelect::HTMLSelect(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLInput(td, pDocument)
{
	miSize = 1;
	miNumOptions = 0;
	miDefSelected = 0;
	miNumOptions = 0;
	mpListBoxDisplay = 0;
	mpDropBoxDisplay = 0;
}

HTMLSelect::HTMLSelect(HTMLSelect &copy)
	: HTMLInput(copy)
{
	miSize = copy.miSize;
	miNumOptions = copy.miNumOptions;
	miDefSelected = copy.miDefSelected;
	mpListBoxDisplay = 0;
	mpDropBoxDisplay = 0;
}


/*****************************************************************************/
// HTMLSelect Destructor
/*****************************************************************************/

HTMLSelect::~HTMLSelect(void)
{
	if (mpListBoxDisplay)
	{
		WEBC_DELETE(mpListBoxDisplay);
	}

	if (mpDropBoxDisplay)
	{
		WEBC_DELETE(mpDropBoxDisplay);
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
void HTMLSelect::Refresh(PegThing *pThing)
{
}
*/

HTMLEventStatus HTMLSelect::DefaultEvent(HTMLEvent *pEvent)
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

			switch (pEvent->data.key)
			{
				case WGK_LNDN:
				case WGK_LNUP:
				case WGK_LEFT:
				case WGK_RIGHT:
					pEvent->flags |= HTML_EVENT_FLAG_CANCEL_BUBBLE;
					break;
			}
			break;
		}

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

		case HTML_EVENT_MOUSEDOWN:
			{
				if(Disabled())
					return (HTML_EVENT_STATUS_HALT);
				break;
			}
		default:
			break;
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

/*
HTMLDocument *HTMLSelect::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLSelect::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/


void HTMLSelect::Reset(void)
{
	HTMLSelectedOptionFinder f;
	FindElement(&f);
	SetSelectedIndex(f.Index());
}

/*
int HTMLSelect::SetProperty(enum html_params Property, PFCHAR value, HTMLDocument *pDocument, int call_base)
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

CSSValueType HTMLSelect::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{

	switch (Property)
	{
	case CSS_PROPERTY_PADDING_LEFT:
	case CSS_PROPERTY_PADDING_RIGHT:
	case CSS_PROPERTY_PADDING_BOTTOM:
	case CSS_PROPERTY_PADDING_TOP:
		pValue->length.type = CSS_LENGTH_UNIT_PX;
#if (SELECT_INCLUDE_PADDING_HTML)
		pValue->length.val.dec = DROPBOX_PADDING;
#else
		pValue->length.val.dec = 0;
#endif
		pValue->length.val.frac = 0;
		return (CSS_VALUE_SPECIFIED);

	case CSS_PROPERTY_BORDER_LEFT_STYLE:
	case CSS_PROPERTY_BORDER_RIGHT_STYLE:
	case CSS_PROPERTY_BORDER_TOP_STYLE:
	case CSS_PROPERTY_BORDER_BOTTOM_STYLE:
		pValue->borderStyle = CSS_BORDER_STYLE_INSET;
		return (CSS_VALUE_SPECIFIED);

	case CSS_PROPERTY_BORDER_LEFT_WIDTH:
	case CSS_PROPERTY_BORDER_RIGHT_WIDTH:
	case CSS_PROPERTY_BORDER_TOP_WIDTH:
	case CSS_PROPERTY_BORDER_BOTTOM_WIDTH:
		pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
		pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
		pValue->borderThickness.val.length.val.dec = 2;
		pValue->borderThickness.val.length.val.frac = 0;
		return (CSS_VALUE_SPECIFIED);

	case CSS_PROPERTY_BORDER_LEFT_COLOR:
	case CSS_PROPERTY_BORDER_RIGHT_COLOR:
	case CSS_PROPERTY_BORDER_TOP_COLOR:
	case CSS_PROPERTY_BORDER_BOTTOM_COLOR:
	{
		pValue->color = HTML_RGBAToColor(0x99,0x99,0x99,0);  // gray
		return (CSS_VALUE_SPECIFIED);
	}

	case CSS_PROPERTY_FONT_FAMILY:
		pValue->name = webc_GetDefaultString(WEBC_DEFAULT_TEXT_INPUT_FONT_FAMILY);
		return (CSS_VALUE_SPECIFIED);

	case CSS_PROPERTY_FONT_STYLE:
		pValue->fontStyle = webc_GetDefaultFontStyle(WEBC_DEFAULT_TEXT_INPUT_FONT_STYLE);
		return (CSS_VALUE_SPECIFIED);

	case CSS_PROPERTY_FONT_VARIANT:
		break;

	case CSS_PROPERTY_FONT_WEIGHT:
		pValue->fontWeight =  webc_GetDefaultFontWeight(WEBC_DEFAULT_TEXT_INPUT_FONT_WEIGHT);
		return (CSS_VALUE_SPECIFIED);

	case CSS_PROPERTY_FONT_SIZE:
		pValue->fontSize.type = CSS_FONT_SIZE_ABSOLUTE;
		pValue->fontSize.val.absolute =  webc_GetDefaultUint16(WEBC_DEFAULT_TEXT_INPUT_FONT_SIZE); 
		return (CSS_VALUE_SPECIFIED);

	case CSS_PROPERTY_COLOR:
	{
		pValue->color = HTML_RGBAToColor(0,0,0,0);  // black
		return (CSS_VALUE_SPECIFIED);
	}

	case CSS_PROPERTY_BACKGROUND_COLOR:
	{
		pValue->color = HTML_RGBAToColor(0xff,0xff,0xff,0);  // white
		return (CSS_VALUE_SPECIFIED);
	}
#if (SELECT_INCLUDE_MARGIN_HTML)
    // New 3-1-07
	case CSS_PROPERTY_MARGIN:
	case CSS_PROPERTY_MARGIN_BOTTOM:
	case CSS_PROPERTY_MARGIN_LEFT:
	case CSS_PROPERTY_MARGIN_RIGHT:
	case CSS_PROPERTY_MARGIN_TOP:
		pValue->length.type = CSS_LENGTH_UNIT_PX;
		pValue->length.val.dec = DROPBOX_MARGIN;
		pValue->length.val.frac = 0;
		return (CSS_VALUE_SPECIFIED);
#endif
	default:
		break;
	}

	return (CSS_VALUE_UNKNOWN);
}

WEBC_CHAR *HTMLSelect::Value(void)
{
	HTMLNthOfTypeFinder f(HTML_OPTION_ELEMENT, SelectedIndex());
	HTMLOption *pOption = (HTMLOption *) FindElement(&f);

	if (pOption)
	{
		if (pOption->Value())
		{
			return (pOption->Value());
		}

		return (pOption->Text());
	}

	return (0);
}


long HTMLSelect::Size(void)
{
	return (miSize);
}

void HTMLSelect::SetSize(long i)
{
	miSize = i;
}


/*****************************************************************************/
// HTMLSelect-Specific Methods
/*****************************************************************************/

short HTMLSelect::NumOptions()
{
HTMLCountByTypeFinder f(HTML_OPTION_ELEMENT);

	FindElement(&f);

	miNumOptions = f.Length();

	return (miNumOptions);
}

void HTMLSelect::SetNumOptions(short numOptions)
{
	// tbd ?
}

short HTMLSelect::SelectedIndex(void)
{
	if (Size() > 1)
	{
		HTMLListBoxDisplay *pDisplay = GetListBoxDisplay(WEBC_FALSE);

		if (pDisplay)
		{
			return UTIL_SAFE_LONG_TO_SHORT(pDisplay->GetSelected());
		}
	}
	else
	{
		HTMLDropBoxDisplay *pDisplay = GetDropBoxDisplay(WEBC_FALSE);

		if (pDisplay)
		{
			return UTIL_SAFE_LONG_TO_SHORT(pDisplay->GetSelected());
		}
	}

	return (0);
}

void HTMLSelect::SetSelectedIndex(short index)
{
	if (index >= NumOptions())
	{
		return;
	}

	if (Size() > 1)
	{
		HTMLListBoxDisplay *pDisplay = GetListBoxDisplay();

		if (pDisplay)
		{
			pDisplay->SetSelected(index);
		}
	}
	else
	{
		HTMLDropBoxDisplay *pDisplay = GetDropBoxDisplay();

		if (pDisplay)
		{
			pDisplay->SetSelected(index);
		}
	}
}

void HTMLSelect::SetDefSelected(short index)
{
	if (index > 0)
	{
		SetFlag(HSELECT_FLAG_DEFAULT_EXPLICIT);
		miDefSelected = index;
	}
	else
	{
		ClearFlag(HSELECT_FLAG_DEFAULT_EXPLICIT);
	}
}

short HTMLSelect::DefSelected()
{
	return (miDefSelected);
}


HTMLOption *HTMLSelect::GetOptionIndex(int iIndex)
{
HTMLNthOfTypeFinder f(HTML_OPTION_ELEMENT, iIndex);

	return ((HTMLOption *) FindElement(&f));
}

int HTMLSelect::GetIndexOfOption(HTMLOption *pOption)
{
HTMLElement *pChild = pOption->mpPrev;
int index = 0;

	while (pChild)
	{
		if (pChild->Type() == HTML_OPTION_ELEMENT)
		{
			index++;
		}
		pChild = pChild->mpPrev;
	}

	return (index);
}


DisplayElement *HTMLSelect::GetDisplayElement (WEBC_BOOL create)
{
	if (Size() > 1)
	{
		return (GetListBoxDisplay(create));
	}

	return (GetDropBoxDisplay(create));
}

TextUnit *HTMLSelect::GetTextUnit (void)
{
	if (Size() > 1)
	{
		return (GetListBoxDisplay());
	}

	return (GetDropBoxDisplay());
}

HTMLListBoxDisplay *HTMLSelect::GetListBoxDisplay (WEBC_BOOL create)
{
	if (!mpListBoxDisplay && create)
	{
		WEBC_NEW_VERBOSE(mpListBoxDisplay, HTMLListBoxDisplay(this),"HTMLListBoxDisplay");
		if (mpListBoxDisplay)
		{
			mpListBoxDisplay->SetFont(GetDefaultFont());
			mpListBoxDisplay->AddListener(this);
			Reset();
		}
	}

	return (mpListBoxDisplay);
}

HTMLDropBoxDisplay *HTMLSelect::GetDropBoxDisplay (WEBC_BOOL create)
{
	if (!mpDropBoxDisplay && create)
	{
		WEBC_NEW_VERBOSE(mpDropBoxDisplay, HTMLDropBoxDisplay(this),"HTMLDropBoxDisplay");
		if (mpDropBoxDisplay)
		{
			mpDropBoxDisplay->SetFont(GetDefaultFont());
			mpDropBoxDisplay->AddListener(this);
			Reset();
		}
	}

	return (mpDropBoxDisplay);
}

void HTMLSelect::Notify (Notifier *who, NotifyEvent *what)
{
    WEBC_DEBUG_ReceiveNotify(this, who, what);
	switch (what->type)
	{
		case NOTIFIER_TYPE_SELECT:
		{
			switch (what->event)
			{
				case NOTIFY_SELECT_CHANGE:
				{
					HTMLEvent e;
					e.type = HTML_EVENT_CHANGE;
					Event(&e);
				}
				break;
                default:
					break;
			}
		}
		break;

		case NOTIFIER_TYPE_LIST_BOX:
		{
			switch (what->event)
			{
				case NOTIFY_SELECT_CHANGE:
				{
					HTMLEvent e;
					e.type = HTML_EVENT_CHANGE;
					Event(&e);
				}
				break;
            default:
				break;
			}
		}
		break;
        default:
			break;
	}
}
