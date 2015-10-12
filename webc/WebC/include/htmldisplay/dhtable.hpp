#ifndef __DHTABLE_HPP__
#define __DHTABLE_HPP__

#include "dhelem.hpp"
#include "hcbox.hpp"

class HTMLTable;
class HTMLTableCell;
class ITableCellWidthMethod;
class ITableWidthDistributeMethod;

typedef WEBC_UINT8 TABLEDISPLAY_FLAGS;

#define TD_FLAG_MAXWIDTH_VALID    0x01
#define TD_FLAG_MINWIDTH_VALID    0x02

class HTMLTableDisplay : public HTMLElementDisplay<DisplayElement>
{
public:
	HTMLTableDisplay(HTMLTable * t);
	virtual ~HTMLTableDisplay();

	virtual void Draw(DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual void DrawThisOnly(DISPLAY_INT x, DISPLAY_INT y, WebGraphics * gc);
	virtual HTMLEventStatus Event(HTMLEvent * event);
	virtual DisplayOverflow GetOverflow(void);
	virtual void Notify(Notifier *who, NotifyEvent *what);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_TABLEDISPLAY; }

	// methods overriden from TextUnit
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);
	virtual DISPLAY_INT GetBaseline(void);
	virtual DISPLAY_INT GetTopMargin(void);
	virtual DISPLAY_INT GetLeftMargin(void);
	virtual DISPLAY_INT GetBottomMargin(void);
	virtual DISPLAY_INT GetRightMargin(void);
	virtual DISPLAY_INT GetParentWidth(void);
	virtual void SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail);
	virtual void SetParentHeight(DISPLAY_INT iHeight);

	virtual IPositionedFormatContext* FormatPositioned (
			DisplayElement* displayParent,
			IPositionedFormatContext* rootContext,
			IPositionedFormatContext* parentContext,
			FormatContextFactory* childContextFactory
		);

	void SetTableElement(HTMLTable * t);

	TUFormatStatus Formatted (void);

protected:
	HTMLTable*         mpTable;
	HTMLContentBox     mBorder;
	TUFormatStatus     mFormatted;
	DISPLAY_INT        miMaxWidth;
	DISPLAY_INT        miMinWidth;
	TABLEDISPLAY_FLAGS mTableFlags;

	// Overridden from HTMLElementDisplay
	virtual WEBC_BOOL SetElementStyle(void);

	void DrawBackground(WebRect *fillRect, WebRect *viewRect, WebGraphics *pGC);
	void SetFormatStatus(TUFormatStatus status);
	WEBC_BOOL MinWidthValid (void);
	WEBC_BOOL MaxWidthValid (void);

	virtual IPositionedFormatContext* FormatSelfPositioned (
			DisplayElement* displayParent,
			IPositionedFormatContext* rootContext,
			IPositionedFormatContext* parentContext,
			FormatContextFactory* childContextFactory,
			WEBC_BOOL generateChildContext
		);

	// returns the formatted content height
	virtual DISPLAY_INT FormatContentPositioned (
			DISPLAY_INT widthValue,
			DISPLAY_INT heightValue,
			WEBC_BOOL   heightIsAuto,
			DISPLAY_INT borderLeftValue,
			DISPLAY_INT borderTopValue,
			DISPLAY_INT borderRightValue,
			DISPLAY_INT borderBottomValue,
			DISPLAY_INT paddingLeftValue,
			DISPLAY_INT paddingTopValue,
			DISPLAY_INT paddingRightValue,
			DISPLAY_INT paddingBottomValue
		);

	void Format (void);

	void AttachTableCells (void);

	WEBC_BOOL HasSpanningCells (int numColumns);

	DISPLAY_INT CalculateMinWidth (
			DISPLAY_INT* minStart,
			int numColumns,
			int numRows,
			HTMLTableCell** rowArray,
			ITableCellWidthMethod* widthMethod
		);

	DISPLAY_INT CalculateMinStarts (
			DISPLAY_INT* minStart,
			int numColumns,
			int numRows,
			ITableCellWidthMethod* widthMethod,
			ITableWidthDistributeMethod* distributeMethod,
			DISPLAY_INT* maxStart,    // scratch data
			HTMLTableCell** rowArray, // scratch data
			DISPLAY_INT* columnWidth  // scratch data
		);

	WEBC_INT8 CalculateColumnFlexibility (
			WEBC_INT8* columnFlexibility,
			int numColumns,
			WEBC_BOOL includeSpan
		);

	void CalculateDistribution (
			DISPLAY_INT* columnWidth,
			DISPLAY_INT* baseStart,
			DISPLAY_INT* columnWeight,
			DISPLAY_INT* minStart,
			WEBC_INT8* columnFlex,
			int numColumns,
			WEBC_INT8 flexLevel,
			DISPLAY_INT totalToDistribute // targetWidth - baseWidth
		);

	void CalculateCompromise (
			DISPLAY_INT* columnWidth,
			DISPLAY_INT* lowStart,
			DISPLAY_INT* highStart,
			WEBC_INT8* columnFlex,
			int numColumns,
			WEBC_INT8 highestFlex,
			DISPLAY_INT targetWidth
		);

	void ExpandColumns (
			DISPLAY_INT* columnStart,            // current start; update this value
			int colIndex,                        // start at this column
			int colSpan,                         // distribute to this many columns
			int numColumns,
			DISPLAY_INT* maxStart,               // ensure columnStart[c] <= maxStart[c]
			DISPLAY_INT targetWidth,             // target width for the spanned columns
			ITableWidthDistributeMethod* distributeMethod,
			DISPLAY_INT* columnWidth             // scratch data used by this algorithm
		);

	void FindSingleColumnWidths (
			DISPLAY_INT* columnWidth,
			int numColumns,
			ITableCellWidthMethod* widthMethod
		);
};

#endif // __DFLOW_HPP__
