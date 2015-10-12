//
// wcdom.CPP -
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
// wcNewStyledTable
/**

 @memo Constructor function for an "table" element with common styles applied to it.
 @doc Creates an table element and customize it with CSS based on the the values original in the function arguments. If requested the new element is also inserted into the document.
 If you can't conveniently completely style the element your needs with this constructor function you can modify the returned element handle by calling  wcElSetStyle or one of its derivatives.


 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL ParentEl		If insert is wcTRUE and this value is non-zero, insert the new element into this container element.
 @param  wcBOOL insert
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is non-zero insert the new element into ParentEl.
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is zero then insert the new element into the document.
 <br> &nbsp;&nbsp;If insert is wcFALSE, the element is not inserted and must be inserted later by calling one of webC explicit element insert methods, the most common being \Ref{wcDocInsertLast} and \Ref{wcElInsertLast}.
 @param  TableTemplate      *CellTemplate Describes the URL of the image. Still needs design work
 @param  int top            top position in pixels, absolute.
 @param  int left			left position in pixels, absolute.
 @param  int height			height in pixels.
 @param  int width          width in pixels.
 @param  int zindex			zindex value.
 @param  char *style		Additional styles to apply or zero for none. Any style string may be provided for example" "visibilty:hidden;border-color:black;border-style:solid;border-thickness:2px;"

 @return wcEl the new element or zero if an error is detected or the element can't be created.

 @see wcElNew, wcElSetStyle
 */
/*****************************************************************************/

/*****************************************************************************/
wcEL wcNewStyledTable(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int left, int height, int width, int zindex, char *style)
{
wcEL element;
// Not done..
	element = wcElNew(Ctx,"input type=button");
	if (element)
	{
		wcElSetValue(element,text);
		ApplyCommonStyles(Ctx, ParentEl, insert, element, top, left, height, width, zindex, color, bgcolor, 0, style);
	}
	return element;
}

/*****************************************************************************/
// wcNewStyledTableCell
/**

 @memo Constructor function for an "table cell" element with common styles applied to it.
 @doc Creates an table cell element and customize it with CSS based on the the values original in the function arguments. If requested the new element is also inserted into the parent table.
 If you can't conveniently completely style the element your needs with this constructor function you can modify the returned element handle by calling  wcElSetStyle or one of its derivatives.


 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL TableEl		If TabletEl is non zero the cell is inserted into the tabe.
 @param  ..					Still needs design work
 @param  char *style		Additional styles to apply or zero for none. Any style string may be provided for example" "visibilty:hidden;border-color:black;border-style:solid;border-thickness:2px;"

 @return wcEl the new element or zero if an error is detected or the element can't be created.

 @see wcElNew, wcElSetStyle
 */
/*****************************************************************************/

/*****************************************************************************/
wcEL wcNewStyledTableCell(wcCtx* Ctx, wcEL TableEl,..)
{
wcEL element;
// Not done..
	element = wcElNew(Ctx,"input type=button");
	if (element)
	{
		wcElSetValue(element,text);
		ApplyCommonStyles(Ctx, ParentEl, insert, element, top, left, height, width, zindex, color, bgcolor, 0, style);
	}
	return element;
}

/*****************************************************************************/
// wcNewStyledTableRow
/**

 @memo Constructor function for an "table cell" element with common styles applied to it.
 @doc Creates an table cell element and customize it with CSS based on the the values original in the function arguments. If requested the new element is also inserted into the parent table.
 If you can't conveniently completely style the element your needs with this constructor function you can modify the returned element handle by calling  wcElSetStyle or one of its derivatives.


 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL TableEl		If TabletEl is non zero the row is inserted into the table.
 @param  ..					Still needs design work
 @param  char *style		Additional styles to apply or zero for none. Any style string may be provided for example" "visibilty:hidden;border-color:black;border-style:solid;border-thickness:2px;"

 @return wcEl the new element or zero if an error is detected or the element can't be created.

 @see wcElNew, wcElSetStyle
 */
/*****************************************************************************/

/*****************************************************************************/
wcEL wcNewStyledTableRow(wcCtx* Ctx, wcEL TableEl,..)
{
wcEL element;
// Not done..
	element = wcElNew(Ctx,"input type=button");
	if (element)
	{
		wcElSetValue(element,text);
		ApplyCommonStyles(Ctx, ParentEl, insert, element, top, left, height, width, zindex, color, bgcolor, 0, style);
	}
	return element;
}

/*****************************************************************************/
// wcWinSetDisplayBuffer
/**
 @memo  Set a buffer to draw to for the window. Graphics will not be written to the framebuffer hardware but will instead be directed this buffer.
 @doc   Set a buffer to draw to for the window. Graphics will not be written to the framebuffer hardware but will instead be directed this buffer

 @param  wcCtx *			Ctx 		- 	Window to change the display buffer on.
 @param  unsigned char *buffer			-   New buffer to write to, if zero a new buffer is allocated internally.

 @return        wcTRUE if the operation was succesful wcFALSE otherwise.

 @see wcWinUnSetDisplayBuffer
 */
/*****************************************************************************/
wcBOOL wcWinSetDisplayBuffer(wcCtx *Ctx, unsigned char *buffer)
{
	return(wcFALSE);
}

/*****************************************************************************/
// wcWinUnSetDisplayBuffer
/**
 @memo  Restore a window to draw to the frame buffer and optionally blit the offscreen buffer to the framebuffer memory.
 @doc   Restore a window to draw to the frame buffer and optionally blit the offscreen buffer to the framebuffer memory.

 @param  wcCtx *			Ctx 		- 	Window to restore the display buffer on.
 @param  wcBOOL 			refreh		-   If wcTRUE  blit the offscreen buffer to the framebuffer memory.

 @return        wcTRUE if the operation was succesful wcFALSE otherwise.

 @see wcWinSetDisplayBuffer
 */
/*****************************************************************************/
wcBOOL wcWinUnSetDisplayBuffer(wcCtx *Ctx, wcBOOL refresh)
{
	return(wcFALSE);
}

/*****************************************************************************/
// wcDocAddStyle
/**
 @memo  Add stylesheet contents to a document.
  @doc  Current work around is to load HTML content with embedded style commands or to load a stylesheet from a url.
  Under development development, adds directly to the document's style sheet.
  <br><center></i> Genreal idea is to use styles more effectively with the API </i></center>

 @param  wcCtx*  Ctx        The current wc context.
 @param  char *CSSSource    CSS siurce to add to the document.

 @return        nothing.

 @see wcElSetStyleHtmlColor
 */
/*****************************************************************************/

void wcDocAddStyle(wcCtx*  Ctx, char *CSSSource)
{
}

/*****************************************************************************/
// wcElSetClass
/**
 @memo  Add stylesheet contents to a document.
  @doc  Current work around is to load HTML content with embedded style commands or to load a stylesheet from a url.
  Under development development, adds directly to the document's style sheet.
  <br><center></i> Genreal idea is to use styles more effectively with the API </i></center>

 @param  wcEL          		Element     -   The element.
 @param  char *ClassName    CSS class name

 @return        nothing.

 @see wcElSetStyleHtmlColor
 */
/*****************************************************************************/

void wcElSetClass( wcEL Element,char *ClassName)
{
}


/*****************************************************************************/
// wcSetSliderStyle
/**
 @memo  Sets the default slider artwork.
  @doc  Under development, currently methods and artwork are to draw sliders at the application layer,
        this API will allow the slider style to be set from the API.
		<br> To be determined if setting sliders individually by document, image, element type etc is desirable.
		<br> Can it be a css style extension.

 @param  char *				SliderStyleName

 @return        nothing.

 @see wcElSetStyleHtmlColor
 */
/*****************************************************************************/

void wcSetSliderStyle(char *SliderStyleName)
{
}


/*****************************************************************************/
// wcWinDisplayBufferToImage
/**
 @memo  Create an image element containing content extracted from the screen buffer.
 @doc   Create an image element containing content extracted from the screen buffer.

 @param  wcCtx *			Ctx 		- 	Window to restore the display buffer on.
 @param  int          		top      	-   coordinbates to extract from the display buffer.
 @param  int          		left      	-
 @param  int          		height     	-
 @param  int          		width     	-

 @return An image element containing the content extracted from the screen buffer.

 @see wcWinSetDisplayBuffer
 */
/*****************************************************************************/
wcEL wcWinDisplayBufferToImage(wcCtx *Ctx, int top ,int left,int height,int width)
{
	return(wcFALSE);
}

/*****************************************************************************/
// wcSysDbHeapReport
/**
 @memo  Populate a heap usage report structureCreate an image element containing content extracted from the screen buffer.
 @doc   Under development, makes memory usage statistics in debug mode that are currently output to the console by WEBC_DEBUG_LOG_MEM_USAGE visible through and API, to aid in burn in testing and leak testing.

 @param  HeapReportSummary 			*pReportSummary,
 @param  HeapReportDetailArray 		*pReport,
 @param  int 						HeapReportDetailArraySize,
 @param  NepReportType 				ReportType

 @return The number of records required for a full detail report summary..

 @see wcWinSetDisplayBuffer
 */
/*****************************************************************************/
int wcSysDbHeapReport(HeapReportSummary *pReportSummary, HeapReportDetailArray *pReport, int HeapReportDetailArraySize, NepReportType ReportType)
{
	return(wcFALSE);
}



