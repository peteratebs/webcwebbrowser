#ifndef __HFRAMSET_HPP__
#define __HFRAMSET_HPP__

#include "webcfg.h"

#if (WEBC_SUPPORT_FRAMES)

#include "helement.hpp"
#include "dhframeset.hpp"

/*************************************************************************
* class HTMLFrameSet
* 
*************************************************************************/

#define HFRAMESET_FLAG_BORDER    HELEM_FLAG_UNUSED_0
#define HFRAMESET_FLAG_UNUSED_0  HELEM_FLAG_UNUSED_1

class HTMLFrameSet : public HTMLElementContainer
{
public:
	HTMLFrameSet(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLFrameSet();

	// TextUnit interface
	virtual WEBC_UINT16 GetUnitType (void) { return (TU_TYPE_FRAMESET); }

	// HTMLElement methods
	virtual HTMLElementType Type() { return HTML_FRAMESET_ELEMENT; }	
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);

	// HTMLFrameSet utility routines
	virtual WEBC_UINT16 Border(void) {return(mwBorder);}
	virtual void SetBorder(int iSize);

	virtual void SetFrameborder(WEBC_BOOL yes);
	virtual WEBC_BOOL Frameborder();
	
	virtual WEBC_UINT16 NumCols() {return(mwColumns);}
	virtual void SetNumCols(WEBC_UINT16 cols);
	
	virtual WEBC_UINT16 NumRows() {return(mwRows);}
	virtual void SetNumRows(WEBC_UINT16 rows);
	
	virtual void SetColsRows(WEBC_UINT16 cols, WEBC_UINT16 rows);

	HTMLLength GetColumnWidth (int col);
	HTMLLength GetRowHeight (int row);

	HTMLLength* GetColumnWidths (void);
	HTMLLength* GetRowHeights (void);

	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual TextUnit       *GetTextUnit (void);

protected:
	WEBC_UINT16 mwBorder;
	WEBC_UINT16 mwColumns;
	WEBC_UINT16 mwRows;
	HTMLLength* mpColumnWidth;
	HTMLLength* mpRowHeight;

	HTMLFrameSetDisplay *GetFrameSetDisplay(WEBC_BOOL create = WEBC_TRUE);
	HTMLFrameSetDisplay *mpFrameSetDisplay;
};

#endif // WEBC_SUPPORT_FRAMES

#endif // __HFRAMSET_HPP__
