#ifndef __HTD_HPP__
#define __HTD_HPP__

#include "webcfg.h"

#if (WEBC_SUPPORT_TABLES)

#include "hflow.hpp"
#include "htr.hpp"

/******************************************************************************
* class HTMLTableCell
*
******************************************************************************/

class HTMLTable;
class HTMLTableRow;

#define HTD_FLAG_NOWRAP           HFLOW_FLAG_UNUSED_0
#define HTD_FLAG_BGCOLOR          HFLOW_FLAG_UNUSED_1
#define HTD_FLAG_WIDTH_FROM_CSS   HFLOW_FLAG_UNUSED_2
#define HTD_FLAG_HEIGHT_FROM_CSS  HFLOW_FLAG_UNUSED_3

class HTMLTableCell : public HTMLFlow
{
protected:
	WEBC_UINT16 mwColumnIndex;
	WEBC_UINT16 mwColSpan;
	WEBC_UINT16 mwRowSpan;
	WEBC_UINT8  mbTdAlign;
	WEBC_UINT8  mbTdVAlign;
	
	HTMLLength mCellWidth;
	HTMLLength mCellHeight;
	HTMLLength mWidthFromCSS;
	HTMLLength mHeightFromCSS;
	
	HTMLColor mTdBackgroundColor;
	
	HTMLTableRow*  mRow;
	HTMLTableCell* mPrevCell;
	HTMLTableCell* mNextCell;

	virtual HTMLFlowDisplay *GetFlowDisplay (WEBC_BOOL create = WEBC_TRUE);

public: 
	HTMLTableCell(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLTableCell();

	virtual HTMLElementType Type() { return HTML_TABLE_CELL_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	virtual WEBC_UINT16 GetUnitType(void);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual WEBC_UINT8 GetChildVAlign(void);
	virtual void LocalStyleUpdate(void);

	virtual WebcFontDescriptor   GetFontSizeDesc   (void);

	int ColIndex(void) { return mwColumnIndex; }
	void SetColIndex(long i) { mwColumnIndex = (WEBC_UINT16) i; }
	int ColSpan(void) { return mwColSpan; }
	int RowSpan(void) { return mwRowSpan; }
	void SetColSpan(int iColSpan);
	void SetRowSpan(int iRowSpan);
	HTMLLength CellWidth(void); 
	HTMLLength CellHeight(void); 
	void SetCellWidth(HTMLLength width);
	void SetCellHeight(HTMLLength height);
	
	HTMLTable*     GetTable(void) { return mRow? mRow->GetTable() : 0; }
	HTMLTableRow*  GetRow(void)   { return mRow; }
	HTMLTableCell* NextCell(void) { return mNextCell; }
	HTMLTableCell* PrevCell(void) { return mPrevCell; }

	void SetRow      (HTMLTableRow* row)   { mRow = row; }
	void SetNextCell (HTMLTableCell* cell) { mNextCell = cell; }
	void SetPrevCell (HTMLTableCell* cell) { mPrevCell = cell; }

	void SetTdBackgroundColor(HTMLColor color);
};

#endif // WEBC_SUPPORT_TABLES
#endif // __HTD_HPP__
