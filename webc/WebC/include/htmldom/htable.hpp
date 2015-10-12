#ifndef __HTABLE_HPP__
#define __HTABLE_HPP__

#include "webcfg.h"

#if (WEBC_SUPPORT_TABLES)

#include "helement.hpp"
#include "dhtable.hpp"

/******************************************************************************
* class HTMLTable
*
******************************************************************************/

class HTMLTableRow;
class HTMLTableCell;

#define HTABLE_FLAG_BG_DIRTY              HELEM_FLAG_UNUSED_0
#define HTABLE_FLAG_CELL_POS_INVALID      HELEM_FLAG_UNUSED_1
#define HTABLE_FLAG_CELLSPACING_DEFINED   HELEM_FLAG_UNUSED_2
#define HTABLE_FLAG_CELLPADDING_DEFINED   HELEM_FLAG_UNUSED_3
#define HTABLE_FLAG_BORDER_DEFINED        HELEM_FLAG_UNUSED_4
#define HTABLE_FLAG_WIDTH_FROM_CSS        HELEM_FLAG_UNUSED_5
#define HTABLE_FLAG_HEIGHT_FROM_CSS       HELEM_FLAG_UNUSED_6

class HTMLTable : public HTMLElementContainer
{
private:
	HTMLColor   mTableBackgroundColor;
	WEBC_UINT16 mBorder[4];
	WEBC_UINT16 mCellSpacing;
	WEBC_UINT16 mCellPadding;  
	WEBC_UINT16 mCellBorder;
	HTMLLength  mWidth;
	HTMLLength  mHeight;
	HTMLLength  mWidthFromCSS;
	HTMLLength  mHeightFromCSS;
	WEBC_UINT8  mTableAlign;
	
	HTMLTableRow* mFirstRow;
	HTMLTableRow* mLastRow;
	
#if(WEBC_SUPPORT_BACKGROUND_IMAGES)
	WebcImage*  mBackgroundImage;
	WEBC_CHAR*  mBackgroundURL;
#endif // WEBC_SUPPORT_BACKGROUND_IMAGES

	WEBC_UINT8 mChildPropertyFlags;
	WEBC_UINT8 mChildAlign;

	HTMLTableDisplay* GetTableDisplay (WEBC_BOOL create = WEBC_TRUE);
	HTMLTableDisplay* mTableDisplay;

public:
	HTMLTable(HTMLTagParse *td, HTMLDocument *pDocument); 
	HTMLTable(HTMLTable &copy);
	virtual ~HTMLTable();

	virtual void NotifyChildAdded(void);
	virtual void NotifyChildRemoved(void);

	virtual HTMLElementType Type() { return HTML_TABLE_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual void Update(WEBC_PFBYTE o, WEBC_PFBYTE data);
	virtual void LocalStyleUpdate (void);

	// Listener interface
	virtual void Notify (Notifier *who, NotifyEvent *what);

	virtual WEBC_UINT16 GetUnitType(void);
	virtual WEBC_UINT16 GetBreak(void);
	virtual WEBC_UINT8  GetTextAlign(void);
	virtual WEBC_UINT8  GetVAlign(void);

	WEBC_UINT8 GetTableAlign() { return mTableAlign; }
	void SetTableAlign(WEBC_UINT8 align) { mTableAlign = align; }

  #if (WEBC_SUPPORT_BACKGROUND_IMAGES)
	WEBC_CHAR*  Background(void) { return mBackgroundURL; }
	WebcImage*  GetBackgroundImage(void) { return mBackgroundImage; }
	void        SetBackground(const WEBC_CHAR* url);
	void        SetBackgroundImage(WebcImage* image);
  #endif // WEBC_SUPPORT_BACKGROUND_IMAGES
	
	HTMLTableRow* InsertRow(int iIndex);
	HTMLTableRow* GetRow(int iIndex);
	void DeleteRow(int iIndex);
	int GetNumRows(void);
	int GetNumColumns(void);
	void SetBorderWidth(int iBorder);
	void SetCellSpacing(int iSpacing);
	void SetCellPadding(int iPadding);
	void SetTableWidth(HTMLLength width);
	void SetTableHeight(HTMLLength height);
	void SetTableBackgroundColor(HTMLColor color);
	HTMLColor TableBackgroundColor (void);
	
	WEBC_UINT16 GetCellPadding(void) { return mCellPadding; }
	WEBC_BOOL CellPaddingDefined (void) { return ((mFlags & HTABLE_FLAG_CELLPADDING_DEFINED) != 0); }
	WEBC_UINT16 GetCellSpacing(void) 
		{ return (mFlags & HTABLE_FLAG_CELLSPACING_DEFINED)? 
				mCellSpacing :
				(((mFlags & HTABLE_FLAG_BORDER_DEFINED) && mBorder[0] > 0)? 2 : 0); }
	WEBC_UINT16 TableBorder(CSSSide whichSide) { return mBorder[whichSide < 4 ?  whichSide : 0]; }
	HTMLLength TableWidth(void);
	HTMLLength TableHeight(void);
	
	WEBC_UINT8 GetChildTextAlign(void);
	void SetChildTextAlign(WEBC_UINT8 bAlign);

	HTMLTableRow* FirstRow (void) { return (mFirstRow); }
	HTMLTableRow* LastRow  (void) { return (mLastRow); }
	
	void SetFirstRow (HTMLTableRow* row) { mFirstRow = row; }
	void SetLastRow  (HTMLTableRow* row) { mLastRow = row; }
	
	int CalculateCellPositions (void);
		
	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual TextUnit       *GetTextUnit (void);	
};

#endif // WEBC_SUPPORT_TABLES
#endif // __HTABLE_HPP__
