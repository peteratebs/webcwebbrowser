//
// wctableapi.CPP -
//
// EBS - WebC
//
// Copyright EBS Inc. , 2011
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

#include "wcapi.h"
#include "dflow.hpp"
#include "helement.hpp"
#include "htmlbrow.hpp"
#include "webcassert.h"
#include "htmlfind.hpp"
#include "htmldoc.hpp"
#include "hoption.hpp"
#include "webcmem.h"
#include "rtptime.h"
#include "hselect.hpp"
#include "htable.hpp"
#include "htr.hpp"
#include "htd.hpp"
#include "rtpprint.h"

void ApplyCommonStyles(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, wcEL element, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style);

/*****************************************************************************/
// wcNewStyledTable
/**

 @memo Constructor function for an "image" element with common styles applied to it.
 @doc Creates an image element and customize it with CSS based on the the values original in the function arguments. If requested the new element is also inserted into the document.
 If you can't conveniently completely style the element your needs with this constructor function you can modify the returned element handle by calling  wcElSetStyle or one of its derivatives.


 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL ParentEl		If insert is wcTRUE and this value is non-zero, insert the new element into this container element.
 @param  wcBOOL insert
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is non-zero insert the new element into ParentEl.
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is zero then insert the new element into the document.
 <br> &nbsp;&nbsp;If insert is wcFALSE, the element is not inserted and must be inserted later by calling one of webC explicit element insert methods, the most common being \Ref{wcDocInsertLast} and \Ref{wcElInsertLast}.
 @param  int top            top position in pixels, absolute.
 @param  int left			left position in pixels, absolute.
 @param  int height			height in pixels or zero to not apply the height style and rely on values retrieved from the image.
 @param  int width          width in pixels or zero to not apply the width style and rely on values retrieved from the image.
 @param  int borderwidth    borderwidth in pixels or zero for no border.
 @param  int cellspacing    Space between cells or zero for no spacing.
 @param  int cellpadding    Pading around cell content or zero for no padding.
 @param  int zindex			zindex value.
 @param  char *color		Named foreground (text) color or zero to use default, see \Ref{wcElSetStyleColorNamed} for available colors.
 @param  char *bgcolor		named background color or zero to use default.
 @param  char *bgimage      URL of background image (example: "file://images/mainpagebackground.gif") or , or zero for none.
 @param  char *style		Additional styles to apply or zero for none. Any style string may be provided for example" "visibilty:hidden;border-color:black;border-style:solid;border-thickness:2px;"

 @return wcEl the new element or zero if an error is detected or the element can't be created.
 @see ExCalculatorDynamic wcTableInsertRow wcTableInsertCell wcTableInsertHeaderCell wcTableCellSetWidth wcTableCellSetHeight wcTableCellSetSpans wcTableGetCell wcTableGetRow

 */
/*****************************************************************************/

wcEL wcNewStyledTable(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int borderwidth, int cellspacing,int cellpadding,int zindex, char *color, char *bgcolor,char *bgimage, char *style)
{
	if (!Ctx)
		return 0;
	wcEL table = wcElNew(Ctx,"table");
	if (table)
	{
		HTMLTable *pTable = (HTMLTable *)table;
		pTable->SetBorderWidth(borderwidth);
		pTable->SetCellSpacing(cellspacing);
		pTable->SetCellPadding(cellpadding);
		ApplyCommonStyles(Ctx, ParentEl, insert, table, top, left, height, width, zindex, color, bgcolor, bgimage, style);
	}
	return table;
}

/*****************************************************************************/
// wcTableInsertRow
/**

 @memo Insert a row into a table.
 @doc Creates and inserts a row into a table.

 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL TableEl		The table element returned from \Ref{wcNewStyledTable}
 @param  CSSTextAlign align Horizontal alignment CSS_TEXT_ALIGN_CENTER,CSS_TEXT_ALIGN_JUSTIFY,CSS_TEXT_ALIGN_LEFT or CSS_TEXT_ALIGN_RIGHT
 @param  CSSVerticalAlign valign Vertical alignment CSS_VERTICAL_ALIGN_BASELINE, CSS_VERTICAL_ALIGN_BOTTOM, CSS_VERTICAL_ALIGN_MIDDLE,	CSS_VERTICAL_ALIGN_SUB,	CSS_VERTICAL_ALIGN_SUPER, CSS_VERTICAL_ALIGN_TEXT_BOTTOM, CSS_VERTICAL_ALIGN_TEXT_TOP, CSS_VERTICAL_ALIGN_TOP
 @param  char *color		Named foreground (text) color or zero to use default, see \Ref{wcElSetStyleColorNamed} for available colors.
 @param  char *bgcolor		named background color or zero to use default.
 @param  char *bgimage      URL of background image (example: "file://images/mainpagebackground.gif") or , or zero for none.
 @param  char *style		Additional styles to apply or zero for none. Any style string may be provided for example" "visibilty:hidden;border-color:black;border-style:solid;border-thickness:2px;"

 @return wcEl the new element or zero if an error is detected or the element can't be created.
 @see ExCalculatorDynamic wcTableInsertRow wcTableInsertCell wcTableInsertHeaderCell wcTableCellSetWidth wcTableCellSetHeight wcTableCellSetSpans wcTableGetCell wcTableGetRow

 */
/*****************************************************************************/

wcEL wcTableInsertRow(wcCtx* Ctx, wcEL TableEl, CSSTextAlign align, CSSVerticalAlign valign, char *color, char *bgcolor,char *bgimage, char *style)
{
	HTMLTableRow *pRow;
	if (!Ctx || !TableEl)
		return 0;
	//int HTMLTableRow::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
	wcEL tr	= wcElNew(Ctx, "tr");
	if (!tr)
		return 0;
	wcElInsertLast(TableEl,tr, wcFALSE);
	pRow = (HTMLTableRow *) tr;
	pRow->SetRowTextAlign(align);
	pRow->SetRowVAlign(valign);

	if (color)
		wcElSetStyleColorNamed(tr,CSS_PROPERTY_COLOR, color, wcFALSE);
	if (bgcolor)
		wcElSetStyleColorNamed(tr,CSS_PROPERTY_BACKGROUND_COLOR, bgcolor, wcFALSE);
	if (bgimage)
		wcElSetStyleBgImage(tr, bgimage, wcFALSE);
	if (style)
		wcElSetStyle(tr, style, wcFALSE);
	return tr;
}


/*****************************************************************************/
// wcTableInsertCell
/**

 @memo Insert a cell into a table row.
 @doc Creates and inserts a row into a table row.

 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL TableRow		The table row element returned from \Ref{wcTableInsertRow}
 @param  char *color		Named foreground (text) color or zero to use default, see \Ref{wcElSetStyleColorNamed} for available colors.
 @param  char *bgcolor		named background color or zero to use default.
 @param  char *bgimage      URL of background image (example: "file://images/mainpagebackground.gif") or , or zero for none.
 @param  char *style		Additional styles to apply or zero for none. Any style string may be provided for example" "visibilty:hidden;border-color:black;border-style:solid;border-thickness:2px;"

 @return wcEl the new element or zero if an error is detected or the element can't be created.
 @see ExCalculatorDynamic wcTableInsertRow wcTableInsertCell wcTableInsertHeaderCell wcTableCellSetWidth wcTableCellSetHeight wcTableCellSetSpans wcTableGetCell wcTableGetRow

 */
/*****************************************************************************/

wcEL wcTableInsertCell(wcCtx* Ctx, wcEL TableRow, char *color, char *bgcolor,char *bgimage, char *style)
{
	if (!Ctx || !TableRow)
		return 0;
	wcEL td	= wcElNew(Ctx, "td");
	if (!td)
		return 0;
	HTMLTableCell *pCell = (HTMLTableCell *) td;

	wcElInsertLast(TableRow,td,wcFALSE);
	if (color)
		wcElSetStyleColorNamed(td,CSS_PROPERTY_COLOR, color, wcFALSE);
	if (bgcolor)
		wcElSetStyleColorNamed(td,CSS_PROPERTY_BACKGROUND_COLOR, bgcolor, wcFALSE);
	if (bgimage)
		wcElSetStyleBgImage(td, bgimage, wcFALSE);
	if (style)
		wcElSetStyle(td, style, wcFALSE);
	return td;
}


/*****************************************************************************/
// wcTableInsertHeaderCell
/**

 @memo Insert a cell into a table row and then inserts a table header inside of it.
 @doc Insert a cell into a table row and then inserts a table header inside of it and returns the table header element.<br>
<i>
 Note: The element returned is the table header element, not a table cell element.<br>
 Note: wcTableInsertHeaderCell creates a table cell so it will be reported by \Ref{wcTableGetCell}.
</i>

 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL TableRow		The table row element returned from \Ref{wcTableInsertRow}
 @param  char *color		Named foreground (text) color or zero to use default, see \Ref{wcElSetStyleColorNamed} for available colors.
 @param  char *bgcolor		named background color or zero to use default.
 @param  char *bgimage      URL of background image (example: "file://images/mainpagebackground.gif") or , or zero for none.
 @param  char *style		Additional styles to apply or zero for none. Any style string may be provided for example" "visibilty:hidden;border-color:black;border-style:solid;border-thickness:2px;"

 @return wcEl the new element or zero if an error is detected or the element can't be created.
 @see ExCalculatorDynamic wcTableInsertRow wcTableInsertCell wcTableInsertHeaderCell wcTableCellSetWidth wcTableCellSetHeight wcTableCellSetSpans wcTableGetCell wcTableGetRow

 */
/*****************************************************************************/

wcEL wcTableInsertHeaderCell(wcCtx* Ctx, wcEL TableRow, char *color, char *bgcolor,char *bgimage, char *style)
{
	if (!Ctx || !TableRow)
		return 0;
	wcEL Cell=wcTableInsertCell(Ctx, TableRow, color, bgcolor,bgimage,style);
	if (!Cell)
		return 0;
	wcEL td	= wcElNew(Ctx, "th");
	if (!td)
		return 0;
	wcElInsertFirst(Cell, td, wcFALSE);
	return td;
}


/*****************************************************************************/
// wcTableCellSetWidth
/**

 @memo Set the width of a cell in a table.
 @doc Set the width of a cell in a table.<br>

 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL TableCell		The table cell element returned from \Ref{wcTableInsertCell}
 @param  int  width			Cell width

 @return nothing
 @see ExCalculatorDynamic wcTableInsertRow wcTableInsertCell wcTableInsertHeaderCell wcTableCellSetWidth wcTableCellSetHeight wcTableCellSetSpans wcTableGetCell wcTableGetRow

 */
/*****************************************************************************/

void wcTableCellSetWidth(wcCtx* Ctx, wcEL TableCell, int width)
{
	if (!Ctx || !TableCell)
		return;
	if (width)
		wcElSetStyleWidth(TableCell, width, wcFALSE);
}

/*****************************************************************************/
// wcTableCellSetHeight
/**

 @memo Set the height of a cell in a table.
 @doc Set the height of a cell in a table.<br>

 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL TableCell		The table cell element returned from \Ref{wcTableInsertCell}
 @param  int  height		Cell height

 @return nothing
 @see ExCalculatorDynamic wcTableInsertRow wcTableInsertCell wcTableInsertHeaderCell wcTableCellSetWidth wcTableCellSetHeight wcTableCellSetSpans wcTableGetCell wcTableGetRow

 */
/*****************************************************************************/

void wcTableCellSetHeight(wcCtx* Ctx, wcEL TableCell, int height)
{
	if (!Ctx || !TableCell)
		return;
	if (height)
		wcElSetStyleHeight(TableCell, height, wcFALSE);
}

/*****************************************************************************/
// wcTableCellSetSpans
/**

 @memo Set the colspan and rowspan property of a cell in a table.
 @doc Set the height of a cell in a table.<br>

 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL TableCell		The table cell element returned from \Ref{wcTableInsertCell}
 @param  int  colspan		New colspan value.
 @param  int  rowspan		New rowspan value.

 @return nothing
 @see ExCalculatorDynamic wcTableInsertRow wcTableInsertCell wcTableInsertHeaderCell wcTableCellSetWidth wcTableCellSetHeight wcTableCellSetSpans wcTableGetCell wcTableGetRow

 */
/*****************************************************************************/

void wcTableCellSetSpans(wcCtx* Ctx, wcEL TableCell, int colspan, int rowspan)
{
	if (!Ctx || !TableCell)
		return;
	HTMLTableCell *pCell = (HTMLTableCell *) TableCell;
	pCell->SetColSpan(colspan);
	pCell->SetRowSpan(rowspan);
}


/*****************************************************************************/
// wcTableGetCell
/**

 @memo Retrieve a Table cell element by row,column index.
 @doc  Retrieves a table cell element by row,column index.

 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL TableEl		The table element returned from \Ref{wcNewStyledTable}
 @param  int row			The row
 @param  int collumn		The collumn

 @return wcEl The table cell element at row,collumn or zero row,collumn does not address a cell.
 @see ExCalculatorDynamic wcTableInsertRow wcTableInsertCell wcTableInsertHeaderCell wcTableCellSetWidth wcTableCellSetHeight wcTableCellSetSpans wcTableGetCell wcTableGetRow

 */
/*****************************************************************************/

wcEL wcTableGetCell(wcCtx* Ctx, wcEL TableEl, int row, int collumn)
{
	if (!Ctx || !TableEl)
		return 0;
	HTMLTable *pTable = (HTMLTable *)TableEl;
	HTMLTableCell *pCell=0;
	HTMLTableRow *pRow=pTable->GetRow(row);
	if (pRow)
		pCell = pRow->GetCell(collumn);
	return (wcEL) pCell;
}

/*****************************************************************************/
// wcTableGetCell
/**

 @memo Retrieve a Table row element by row number.
 @doc  Retrieves a table row element by row number.

 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL TableEl		The table element returned from \Ref{wcNewStyledTable}
 @param  int row			The row

 @return wcEl The table row element at row.
 @see ExCalculatorDynamic wcTableInsertRow wcTableInsertCell wcTableInsertHeaderCell wcTableCellSetWidth wcTableCellSetHeight wcTableCellSetSpans wcTableGetCell wcTableGetRow

 */
/*****************************************************************************/

wcEL wcTableGetRow(wcCtx* Ctx, wcEL TableEl, int row)
{
	if (!Ctx || !TableEl)
		return 0;
	HTMLTable *pTable = (HTMLTable *)TableEl;
	HTMLTableRow *pRow=pTable->GetRow(row);
	return (wcEL) pRow;
}


#if (0)
wcEL wcNewStyledTable(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int cols, int rows, int top, int left, int height, int width, int borderwidth, int cellspacing,int cellpadding,int zindex, char *color, char *bgcolor,char *bgimage, char *style)
{
	int cellwidth;
	int cellheight;

	if (!Ctx || !cols || !rows)
		return 0;
// 	(cellwidth+(2*borderwidth))*cols = width; so..
	cellwidth = width/cols-(2*borderwidth+cellspacing+(cellpadding*2));
	cellheight = height/rows-(2*borderwidth+cellspacing+(cellpadding*2));
	if (cellwidth < 1 || cellheight < 1)
		return 0;
	wcEL table = wcElNew(Ctx,"table");
	if (table)
	{
		HTMLTable *pTable = (HTMLTable *)table;
		pTable->SetBorderWidth(borderwidth);
		pTable->SetCellSpacing(cellspacing);
		pTable->SetCellPadding(cellpadding);
		ApplyCommonStyles(Ctx, ParentEl, insert, table, top, left, height, width, zindex, color, bgcolor, bgimage, style);
		for (int rownumber = 0; rownumber < rows; rownumber++)
		{
			wcEL tr	= wcElNew(Ctx, "tr");
			if (!tr)
				return 0;
			wcElInsertLast(table,tr);
			for (int colnumber = 0; colnumber < cols; colnumber++)
			{
				wcEL td	= wcElNew(Ctx, "td");
				if (!td)
					return 0;
			    wcElSetInnerHtml(td,"X");
				wcElSetStyleDim(td, cellheight, cellwidth,wcFALSE);
//				wcElSetStyle(td, style, wcFALSE);

				wcElInsertLast(tr,td);
			}
		}
		wcElRefresh(table);

	}
	return table;
}
#endif
