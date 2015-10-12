#ifndef __HTR_HPP__
#define __HTR_HPP__

#include "webcfg.h"

#if (WEBC_SUPPORT_TABLES)

#include "helement.hpp"

/******************************************************************************
* class HTMLTableRow
*
******************************************************************************/

class HTMLTable;
class HTMLTableCell;

#define HTR_FLAG_TEXTALIGN      HELEM_FLAG_UNUSED_0
#define HTR_FLAG_VALIGN         HELEM_FLAG_UNUSED_1
#define HTR_FLAG_BGCOLOR        HELEM_FLAG_UNUSED_2
#define HTR_FLAG_NOWRAP         HELEM_FLAG_UNUSED_3

class HTMLTableRow : public HTMLElementContainer
{
protected:	
	CSSTextAlign GetRowTextAlign (void);
	CSSVerticalAlign GetRowVAlign (void);

	WEBC_UINT16      mRowIndex;
    HTMLLength       mRowHeight;
	HTMLColor        mRowBackgroundColor;
	CSSTextAlign     mTextAlign;
	CSSVerticalAlign mVerticalAlign;

	HTMLTable*       mTable;
	HTMLTableRow*    mPrevRow;
	HTMLTableRow*    mNextRow;
	HTMLTableCell*   mFirstCell;
	HTMLTableCell*   mLastCell;

public:
	HTMLTableRow(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLTableRow();

	void SetRowTextAlign (CSSTextAlign align);
	void SetRowVAlign (CSSVerticalAlign valign);

	virtual void NotifyChildAdded(void);
	virtual void NotifyChildRemoved(void);

	virtual HTMLElementType Type() { return HTML_TABLE_ROW_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
		
	HTMLTableCell *InsertCell(int iIndex);
	HTMLTableCell *GetCell(int iIndex);
	void DeleteCell(int iIndex);
	int RowIndex(void) { return mRowIndex; }
	void SetRowIndex (long i) { mRowIndex = (WEBC_UINT16) i; }
	int GetNumColumns(void);
	void SetRowHeight(HTMLLength height);
    HTMLLength GetRowHeight(void) { return mRowHeight; }
	void SetRowBackgroundColor(HTMLColor color);
	HTMLColor RowBackgroundColor (void);

	HTMLTable*     GetTable(void)  { return mTable; }
	HTMLTableCell* FirstCell(void) { return mFirstCell; }
	HTMLTableCell* LastCell(void)  { return mLastCell; }
	HTMLTableRow*  NextRow(void)   { return mNextRow; }
	HTMLTableRow*  PrevRow(void)   { return mPrevRow; }

	void SetTable     (HTMLTable* table)    { mTable = table; }
	void SetFirstCell (HTMLTableCell* cell)	{ mFirstCell = cell; }
	void SetLastCell  (HTMLTableCell* cell) { mLastCell = cell; }
	void SetNextRow   (HTMLTableRow* row)   { mNextRow = row; }
	void SetPrevRow   (HTMLTableRow* row)   { mPrevRow = row; }
};


#endif // WEBC_SUPPORT_TABLES
#endif // __HTR_HPP__
