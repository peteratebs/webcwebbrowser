//
// DHTD.CPP - Methods for class HTMLTableCellDisplay
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "htable.hpp"
#include "htr.hpp"
#include "htd.hpp"
#include "dflow.hpp"
#include "margins.hpp"
#include "tuenum.hpp"
#include "dhframeset.hpp"
#include "zdelem.hpp"
#include "dhtd.hpp"
#include "dhelem.cpp"

/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#if (WEBC_SUPPORT_TABLES)

#define TD_ELEMENT   ((HTMLTableCell *) mpHtmlElement)


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/

HTMLTableCellDisplay::HTMLTableCellDisplay(HTMLTableCell * f)
	: HTMLFlowDisplay (f)
{
}

HTMLTableCellDisplay::~HTMLTableCellDisplay()
{
}

void HTMLTableCellDisplay::SetTdElement(HTMLTableCell * f)
{
	SetHtmlElement(f);
}

DISPLAY_INT HTMLTableCellDisplay::GetMinWidth(void)
{
	HTMLLength width = TD_ELEMENT->CellWidth();

	if ((HTML_LengthGetUnit(width) == HTML_LENGTH_UNIT_PIXELS) && (GetChildWrap() == TU_WRAP_NONE))
	{
		return (EBSMAX(HTMLFlowDisplay::GetMinWidth(), HTML_LengthGetValue(width)));
	}

	return (HTMLFlowDisplay::GetMinWidth());
}

DISPLAY_INT HTMLTableCellDisplay::GetMaxWidth(void)
{
	if (!MaxWidthValid())
	{
		if (HTML_LengthGetUnit(TD_ELEMENT->CellWidth()) == HTML_LENGTH_UNIT_PIXELS)
		{
			miMaxWidth = EBSMAX(GetMinWidth(), HTML_LengthGetValue(TD_ELEMENT->CellWidth()));
		}
		else
		{
			miMaxWidth = FindMaxWidth(0x7fff);
		}

		mFlowFlags |= FD_FLAG_MAXWIDTH_VALID;
	}

	return (miMaxWidth);
}

void HTMLTableCellDisplay::FormatTableCell (DISPLAY_INT cellWidth)
{
	DISPLAY_INT minWidth = GetMinWidth();

	cellWidth = EBSMAX(minWidth, cellWidth);
	if (cellWidth != Width())
	{
		SetWidth(cellWidth);
		Format();
	}
}

/*---------------------------------------------------------------------------*/
void HTMLTableCellDisplay::UpdateHeight (
		DISPLAY_INT topPad,
		DISPLAY_INT bottomPad
	)
{
	DISPLAY_INT heightValue;
	WEBC_BOOL heightIsAuto;

	GetSpecifiedHeight(&heightValue, &heightIsAuto);

	if (heightIsAuto)
	{
		heightValue = miContentHeight;
	}
	else
	{
		if (heightValue < miContentHeight)
		{
			heightValue = miContentHeight;
		}
	}

	SetHeight(topPad + heightValue + bottomPad);
}

#endif // WEBC_SUPPORT_TABLES
