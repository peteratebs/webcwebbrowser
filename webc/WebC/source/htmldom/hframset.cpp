//
// HFRAMSET.CPP - Methods for HTML DOM class HTMLFrameSet
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
#include "hframset.hpp"
#include "webc.h"
#include "webcstr.h"
#include "webcmem.h"

#if (WEBC_SUPPORT_FRAMES)

/*****************************************************************************/
// HTMLFrameSet Constructors
/*****************************************************************************/

HTMLFrameSet::HTMLFrameSet(HTMLTagParse *td, HTMLDocument *document)
	: HTMLElementContainer(td, document)
{
static WEBC_CHAR defaultBorder[] = {'6', 0};
static WEBC_CHAR defaultCols[]   = {'1', '0', '0', '%', 0};
static WEBC_CHAR defaultRows[]   = {'1', '0', '0', '%', 0};

	mpFrameSetDisplay = 0;
	mwBorder = 0;
	mwColumns = 0;
	mwRows = 0;
	mpColumnWidth = 0;
	mpRowHeight = 0;	
	
	SetFlag(HFRAMESET_FLAG_BORDER);
	
	SetProperty(HTML_ATTRIB_BORDER, defaultBorder, document);
	SetProperty(HTML_ATTRIB_COLS, defaultCols, document);
	SetProperty(HTML_ATTRIB_ROWS, defaultRows, document);

	HTMLBody* bodyElement = (document)? document->Body() : 0;
	
	if (bodyElement && !bodyElement->GetFrameSet())
	{
		bodyElement->SetFrameSet(this);
	}
}


/*****************************************************************************/
// HTMLFrameSet Destructor
/*****************************************************************************/

HTMLFrameSet::~HTMLFrameSet(void)
{
	WEBC_FREE(mpColumnWidth);
	WEBC_FREE(mpRowHeight);
	
	if (mpFrameSetDisplay)
	{
		WEBC_DELETE(mpFrameSetDisplay);
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

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

int HTMLFrameSet::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
int iNumCols=0, iNumRows=0;
	
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_ROWS:
		{			
			static WEBC_CHAR defValue[] = {'1','0','0','%',0};
			if (!value)
			{
				value = defValue;
			}
			if (mpRowHeight)
			{
				WEBC_FREE(mpRowHeight);
				mpRowHeight = 0;
				mwRows = 0;
			}
			iNumRows = HTML_CountLengthList(value);
			if (iNumRows > 0)
			{
				mpRowHeight = (HTMLLength*) WEBC_MALLOC(sizeof(HTMLLength)*iNumRows);
				if (mpRowHeight)
				{
					mwRows = iNumRows;	
					HTML_ParseLengthList(value, mpRowHeight, iNumRows);
				}
				else
				{
					mwRows = 0;
				}
			}
			return (1);
		}
		
		case HTML_ATTRIB_COLS:
		{
			static WEBC_CHAR defValue[] = {'1','0','0','%',0};
			if (!value)
			{
				value = defValue;
			}
			if (mpColumnWidth)
			{
				WEBC_FREE(mpColumnWidth);
				mpColumnWidth = 0;
				mwColumns = 0;
			}
			iNumCols = HTML_CountLengthList(value);
			if (iNumCols > 0)
			{
				mpColumnWidth = (HTMLLength*) WEBC_MALLOC(sizeof(HTMLLength)*iNumCols);
				if (mpColumnWidth)
				{
					mwColumns = iNumCols;	
					HTML_ParseLengthList(value, mpColumnWidth, iNumCols);
				}
				else
				{
					mwColumns = 0;
				}
			}
			return (1);
		}
			
		case HTML_ATTRIB_BORDER:
		{
			static WEBC_CHAR defValue[] = {'6',0};
			if (!value)
			{
				value = defValue;
			}
			SetBorder(webc_atoi(value));
			return (1);
		}

		case HTML_ATTRIB_FRAMEBORDER:
			if (value && !webc_c_stricmp(value, "no") || !webc_c_strcmp(value, "0"))
			{
				SetFrameborder(WEBC_FALSE);
			}
			else
			{
				SetFrameborder(WEBC_TRUE);
			}
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


void HTMLFrameSet::SetBorder(int iSize)
{
	iSize = 0;
	mwBorder = EBSMAX(0,iSize);
	if (mwBorder > 0)
	{
		mwBorder = EBSMAX(2,mwBorder);
	}
}


void HTMLFrameSet::SetColsRows(WEBC_UINT16 cols, WEBC_UINT16 rows)
{
	mwColumns = cols;
	mwRows = rows;
	// tbd - notify display element
}


void HTMLFrameSet::SetNumCols(WEBC_UINT16 cols)
{
	SetColsRows(cols, mwRows);
}


void HTMLFrameSet::SetNumRows(WEBC_UINT16 rows)
{
	SetColsRows(mwColumns, rows);
}

void HTMLFrameSet::SetFrameborder(WEBC_BOOL yes)
{
	if (yes)
	{
		SetFlag(HFRAMESET_FLAG_BORDER);
	}
	else
	{
		ClearFlag(HFRAMESET_FLAG_BORDER);		
	}
}

WEBC_BOOL HTMLFrameSet::Frameborder()
{
	return ((mFlags & HFRAMESET_FLAG_BORDER) != 0);
}

/*****************************************************************************/
// HTMLFrameSet-Specific Methods
/*****************************************************************************/

HTMLLength HTMLFrameSet::GetColumnWidth (int col)
{
	if (mpColumnWidth && col >= 0 && col < mwColumns)
	{
		return (mpColumnWidth[col]);
	}

	HTMLLength defaultLength;

	HTML_LengthSet(defaultLength, 1, HTML_LENGTH_UNIT_STAR);
	
	return (defaultLength);
}

HTMLLength HTMLFrameSet::GetRowHeight (int row)
{
	if (mpRowHeight && row >= 0 && row < mwRows)
	{
		return (mpRowHeight[row]);
	}
	
	HTMLLength defaultLength;

	HTML_LengthSet(defaultLength, 1, HTML_LENGTH_UNIT_STAR);
	
	return (defaultLength);
}

HTMLLength* HTMLFrameSet::GetColumnWidths (void)
{
	return mpColumnWidth;
}

HTMLLength* HTMLFrameSet::GetRowHeights (void)
{
	return mpRowHeight;
}

DisplayElement *HTMLFrameSet::GetDisplayElement (WEBC_BOOL create)
{
	return (GetFrameSetDisplay(create));
}

TextUnit *HTMLFrameSet::GetTextUnit (void)
{
	return (GetFrameSetDisplay());
}

HTMLFrameSetDisplay *HTMLFrameSet::GetFrameSetDisplay (WEBC_BOOL create)
{
	if (!mpFrameSetDisplay && create)
	{
		WEBC_NEW_VERBOSE(mpFrameSetDisplay, HTMLFrameSetDisplay(this),"HTMLFrameSetDisplay");
	}
	
	return (mpFrameSetDisplay);
}

#endif // WEBC_SUPPORT_FRAMES
