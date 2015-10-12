//
// wcapphelper.CPP -
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

#ifndef RTP_TRACK_LOCAL_MEMORY // If memory diagnostics are enabled the callers line and file are passed in
#define file 0
#define line 0
#endif


void ApplyCommonStyles(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, wcEL element, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style)
{
	// Optimize this later, plus if (width) and if(height) should be done seperately
	wcElSetStylePos(element, top, left,wcFALSE);
	if (height && width)
		wcElSetStyleDim(element, height, width, wcFALSE);
	wcElSetStyleZindex(element, zindex, wcFALSE);
	if (color)
		wcElSetStyleColorNamed(element,CSS_PROPERTY_COLOR, color, wcFALSE);
	if (bgcolor)
		wcElSetStyleColorNamed(element,CSS_PROPERTY_BACKGROUND_COLOR, bgcolor, wcFALSE);
	if (bgimage)
		wcElSetStyleBgImage(element, bgimage, wcFALSE);
	if (style)
		wcElSetStyle(element, style, wcFALSE);
	if (insert)
	{
		if (ParentEl)
			wcElInsertLast(ParentEl, element, wcTRUE);
		else
			wcDocInsertLast(Ctx, element);
	}
}

#include "hedit.hpp"
#include "hchkbox.hpp"
#include "hradio.hpp"
#include "himage.hpp"
#include "hibutton.hpp"

#ifdef NOTDEF  // Conditionally excluded to satisfy docpp
/*****************************************************************************/
// wcElNew
/**

 @memo Dynamically create an element.
 @doc Dynamically create an element from its tag name.
 The element must be inserted into the document before much can be done with it.

 <pre>
 Examples:
     wcElNew(Ctx, "div");
     wcElNew(Ctx, "img");

     Example : Make a table with one cell.
     wcEL table = wcElNew(Ctx, "table");
     wcEL td 	= wcElNew(Ctx, "td");
     wcEL tr 	= wcElNew(Ctx, "tr");
     	wcELInsertFirst(table,tr);
     	wcELInsertFirst(tr,td);
     	wcElSetInnerHtml(td,"Hello");
	    wcDocInsertLast(Ctx, table);

	    Example : Create, position and assign an event handler to an input button.
	    Note that input element creation is different because the type must be specified.
	    The button element is actually a container of content to displays with a specified value,
	    so both the inner html and the value have to be set.

     	wcEL button = wcElNew(&_Ctx, "input type=button");
     	wcElSetInnerHtml(button, "Clickme");
     	wcElSetValue(button, "Clickme");
     	wcElSetStyle(button, "position:absolute;left:20px;top:150px;",wcTRUE);
     	wcDocInsertLast(&_Ctx, button);
     	wcElSetEventHandler(button,  wcTestEventCallback);
 </pre>

 @param  wcCtx*     Ctx     -   The current wc context.
 @param  char *     Name    -   Html tag name.

 @return The new element or zero on failure.

 @see wcDocInsertFirst wcDocInsertLast wcElInsertFirst wcElInsertLast wcElInsertAfter wcElSetEventHandler
 */
/*****************************************************************************/
wcEL wcElNew(wcCtx* Ctx, char *_name)
{ // Conditionally excluded to satisfy docpp
}
#endif
#ifdef RTP_TRACK_LOCAL_MEMORY
wcEL _wcElNew(wcCtx* Ctx, char *_name, const char *file, const long line)
#else
wcEL wcElNew(wcCtx* Ctx, char *_name)
#endif
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR name[HTML_TEMP_STR_LEN];
	webc_c_strncpy(name, _name, HTML_TEMP_STR_LEN-1);
	name[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	 WEBC_CHAR* name = _name;
  #endif

	if (webc_c_strnicmp (name,"input type=", 11)==0)
	{
		WEBC_CHAR* na = name;
		na+=11;
		HTMLInputType inputType = HTML_ParseInputType (na, webc_strlen(na));
		HTMLElement *pElement = 0;
		HTMLTagParse tag[1];

#if (0)
	/// <INPUT type=submit>
	HTML_BUTTON_ELEMENT,
	/// <INPUT type=checkbox>
	HTML_CHECKBOX_ELEMENT,
	/// <INPUT type=text>
	HTML_EDIT_STR_ELEMENT,
	/// <TEXTAREA>
	HTML_EDITBOX_ELEMENT,
	/// <INPUT type=hidden>
	HTML_HIDDEN_INPUT_ELEMENT,
	/// <IMG>
	HTML_IMAGE_ELEMENT,
	/// <INPUT type=button>
	HTML_INPUT_BUTTON_ELEMENT,
	/// <INPUT type=pass>
	HTML_INPUT_PASSWORD_ELEMENT,
	/// <INPUT type=reset>
	HTML_INPUT_RESET_ELEMENT,
	/// <INPUT type=radio>
	HTML_RADIO_BUTTON_ELEMENT,
#endif
    	HTML_InitTagParse(tag, HTML_TAG_INPUT);

        switch (inputType)
        {
		    case HTML_INPUT_TYPE_FILE:
		    case HTML_INPUT_TYPE_TEXT: //AK_TODO treat the file upload object like a text item for now
				WEBC_DEBUG_NEW(pElement, HTMLEditString(tag, (HTMLDocument *)Ctx->hdoc),file,line,"HTMLEditString");
			    break;
		    case HTML_INPUT_TYPE_PASSWORD:
//			input_elem = Password(td);
			    break;
		    case HTML_INPUT_TYPE_CHECKBOX:
				WEBC_DEBUG_NEW(pElement,HTMLCheckbox(tag, (HTMLDocument *)Ctx->hdoc),file,line,"HTMLCheckbox");
			    break;
		    case HTML_INPUT_TYPE_RADIO:
				WEBC_DEBUG_NEW(pElement,HTMLRadioButton(tag, (HTMLDocument *)Ctx->hdoc),file,line,"HTMLRadioButton");
			    break;
		    case HTML_INPUT_TYPE_SUBMIT:
//			input_elem = InputSubmit(td);
			    break;
		    case HTML_INPUT_TYPE_HIDDEN:
//			input_elem = InputHidden(td);
			    break;
		    case HTML_INPUT_TYPE_IMAGE:
				WEBC_DEBUG_NEW(pElement,HTMLImage(tag, (HTMLDocument *)Ctx->hdoc),file,line,"HTMLImage");
//			input_elem = Image(td);
			    break;
		    case HTML_INPUT_TYPE_RESET:
//			input_elem = InputReset(td);
			    break;
		    case HTML_INPUT_TYPE_BUTTON:
				WEBC_DEBUG_NEW(pElement,HTMLInputButton(tag, (HTMLDocument *)Ctx->hdoc),file,line,"HTMLInputButton");
			    break;
		    default:
			    // ignore other input types
			    return (0);
		    }
			return (wcEL) pElement;
	}

	HTMLTagType hType = HTML_ParseTagType(name, webc_strlen(name));
	if (hType < 0)
		return 0;

	HTMLElementType eType = TagToHTMLElementType[hType];

	HTMLTagParse tag[1];
	HTMLDocument *pDoc = (HTMLDocument *)Ctx->hdoc;
	HTML_InitTagParse(tag, hType);
#ifdef RTP_TRACK_LOCAL_MEMORY
	HTMLElement *pElement = _wcCreateElement(eType, tag, pDoc,file,line);
#else
	HTMLElement *pElement = wcCreateElement(eType, tag, pDoc);
#endif

	return ((wcEL)pElement);
}


#ifdef NOTDEF	// Conditionally excluded to satisfy docpp
/*****************************************************************************/
// wcNewStyledDiv
/**

 @memo Constructor function for a div element with common styles applied to it.
 @doc Creates a div element and customize it with CSS based on the the values original in the function arguments. If requested the new element is also inserted into the document.
 If you can't conveniently completely style the element your needs with this constructor function you can modify the returned element handle by calling  wcElSetStyle or one of its derivatives.

 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL ParentEl		If insert is wcTRUE and this value is non-zero, insert the new element into this container element.
 @param  wcBOOL insert
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is non-zero insert the new element into ParentEl.
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is zero then insert the new element into the document.
 <br> &nbsp;&nbsp;If insert is wcFALSE, the element is not inserted and must be inserted later by calling one of webC explicit element insert methods, the most common being \Ref{wcDocInsertLast} and \Ref{wcElInsertLast}.
 @param  int top            top position in pixels, absolute.
 @param  int left			left position in pixels, absolute.
 @param  int height			height in pixels or zero to not apply the height style and rely on defaults.
 @param  int width          width in pixels or zero to not apply the width style and rely on defaults.
 @param  int zindex			zindex value.
 @param  char *color		Named foreground (text) color or zero to use default, see \Ref{wcElSetStyleColorNamed} for available colors.
 @param  char *bgcolor		named background color or zero to use default.
 @param  char *bgimage      URL of background image (example: "file://images/mainpagebackground.gif") or , or zero for none.
 @param  char *style		Additional styles to apply or zero for none. Any style string may be provided for example" "visibilty:hidden;border-color:black;border-style:solid;border-thickness:2px;"

 @return wcEl the new element or zero if an error is detected or the element can't be created.

 @see wcElNew, wcElSetStyle wcDocInsertFirst wcDocInsertLast wcElInsertFirst wcElInsertLast wcElInsertAfter wcElSetEventHandler
 */
/*****************************************************************************/
wcEL wcNewStyledDiv(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style)
{ // Conditionally excluded to satisfy docpp
}
#endif
/*****************************************************************************/
#ifdef RTP_TRACK_LOCAL_MEMORY
wcEL _wcNewStyledDiv(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style, const char *file, const long line)
#else
wcEL wcNewStyledDiv(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style)
#endif
{
wcEL element;
#ifdef RTP_TRACK_LOCAL_MEMORY
	element = _wcElNew(Ctx,"div",file,line);
#else
	element = wcElNew(Ctx,"div");
#endif
	if (element)
		ApplyCommonStyles(Ctx, ParentEl, insert, element, top, left, height, width, zindex, color, bgcolor, bgimage, style);

	return element;
}

#ifdef NOTDEF	// Conditionally excluded to satisfy docpp
/*****************************************************************************/
// wcNewStyledFont
/**

 @memo Constructor function for a font element with common styles applied to it.
 @doc Creates a font element and customize it with CSS based on the the values original in the function arguments. If requested the new element is also inserted into the document.
 If you can't conveniently completely style the element your needs with this constructor function you can modify the returned element handle by calling  wcElSetStyle or one of its derivatives.

 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL ParentEl		If insert is wcTRUE and this value is non-zero, insert the new element into this container element.
 @param  wcBOOL insert
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is non-zero insert the new element into ParentEl.
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is zero then insert the new element into the document.
 <br> &nbsp;&nbsp;If insert is wcFALSE, the element is not inserted and must be inserted later by calling one of webC explicit element insert methods, the most common being \Ref{wcDocInsertLast} and \Ref{wcElInsertLast}.
 @param  int top            top position in pixels, absolute.
 @param  int left			left position in pixels, absolute.
 @param  int height			height in pixels or zero to not apply the height style and rely on defaults.
 @param  int width          width in pixels or zero to not apply the width style and rely on defaults.
 @param  int zindex			zindex value.
 @param  char *color		Named foreground (text) color or zero to use default, see \Ref{wcElSetStyleColorNamed} for available colors.
 @param  char *bgcolor		named background color or zero to use default.
 @param  char *style		Additional styles to apply or zero for none. Any style string may be provided for example" "visibilty:hidden;border-color:black;border-style:solid;border-thickness:2px;"

 @return wcEl the new element or zero if an error is detected or the element can't be created.

 @see wcElNew, wcElSetStyle wcDocInsertFirst wcDocInsertLast wcElInsertFirst wcElInsertLast wcElInsertAfter
 */
/*****************************************************************************/
wcEL wcNewStyledFont(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor, char *style)
{ // Conditionally excluded to satisfy docpp
}

#endif

/*****************************************************************************/
#ifdef RTP_TRACK_LOCAL_MEMORY
wcEL _wcNewStyledFont(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor, char *style, const char *file, const long line)
#else
wcEL wcNewStyledFont(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor, char *style)
#endif
{
wcEL element;

#ifdef RTP_TRACK_LOCAL_MEMORY
	element = _wcElNew(Ctx,"font",file,line);
#else
	element = wcElNew(Ctx,"font");
#endif
	if (element)
		ApplyCommonStyles(Ctx, ParentEl, insert, element, top, left, height, width, zindex, color, bgcolor, 0, style);

	return element;
}

#ifdef NOTDEF	// Conditionally excluded to satisfy docpp
/*****************************************************************************/
// wcNewStyledButtonText
/**

 @memo Constructor function for an "input type=button" element with common styles applied to it.
 @doc Creates an input element and customize it with CSS based on the the values original in the function arguments. If requested the new element is also inserted into the document.
 If you can't conveniently completely style the element your needs with this constructor function you can modify the returned element handle by calling  wcElSetStyle or one of its derivatives.

 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL ParentEl		If insert is wcTRUE and this value is non-zero, insert the new element into this container element.
 @param  wcBOOL insert
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is non-zero insert the new element into ParentEl.
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is zero then insert the new element into the document.
 <br> &nbsp;&nbsp;If insert is wcFALSE, the element is not inserted and must be inserted later by calling one of webC explicit element insert methods, the most common being \Ref{wcDocInsertLast} and \Ref{wcElInsertLast}.
 @param  char *text         Text to display in the button.
 @param  int top            top position in pixels, absolute.
 @param  int left			left position in pixels, absolute.
 @param  int height			height in pixels or zero to not apply the height style and rely on defaults.
 @param  int width          width in pixels or zero to not apply the width style and rely on defaults.
 @param  int zindex			zindex value.
 @param  char *color		Named foreground (text) color or zero to use default, see \Ref{wcElSetStyleColorNamed} for available colors.
 @param  char *bgcolor		amed background color or zero to use default.
 @param  char *style		Additional styles to apply or zero for none. Any style string may be provided for example" "visibilty:hidden;border-color:black;border-style:solid;border-thickness:2px;"

 @return wcEl the new element or zero if an error is detected or the element can't be created.

 @see wcElNew, wcElSetStyle wcDocInsertFirst wcDocInsertLast wcElInsertFirst wcElInsertLast wcElInsertAfter wcElSetEventHandler
 */
/*****************************************************************************/
wcEL wcNewStyledButtonText(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, char *text, int top, int left, int height, int width, int zindex, char *color, char *bgcolor, char *style)
{ // Conditionally excluded to satisfy docpp
}
#endif

/*****************************************************************************/
#ifdef RTP_TRACK_LOCAL_MEMORY
wcEL _wcNewStyledButtonText(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, char *text, int top, int left, int height, int width, int zindex, char *color, char *bgcolor, char *style, const char *file, const long line)
#else
wcEL wcNewStyledButtonText(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, char *text, int top, int left, int height, int width, int zindex, char *color, char *bgcolor, char *style)
#endif
{
wcEL element;

#ifdef RTP_TRACK_LOCAL_MEMORY
	element = _wcElNew(Ctx,"input type=button",file,line);
#else
	element = wcElNew(Ctx,"input type=button");
#endif
	if (element)
	{
		wcElSetValue(element,text);
		ApplyCommonStyles(Ctx, ParentEl, insert, element, top, left, height, width, zindex, color, bgcolor, 0, style);
	}
	return element;
}


#ifdef NOTDEF	// Conditionally excluded to satisfy docpp
/*****************************************************************************/
// wcNewStyledTextEdit
/**

 @memo Constructor function for an input type=text element with common styles applied to it.
 @doc Creates an input type=text element and customize it with CSS based on the the values original in the function arguments. If requested the new element is also inserted into the document.
 If you can't conveniently completely style the element your needs with this constructor function you can modify the returned element handle by calling  wcElSetStyle or one of its derivatives.

 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL ParentEl		If insert is wcTRUE and this value is non-zero, insert the new element into this container element.
 @param  wcBOOL insert
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is non-zero insert the new element into ParentEl.
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is zero then insert the new element into the document.
 <br> &nbsp;&nbsp;If insert is wcFALSE, the element is not inserted and must be inserted later by calling one of webC explicit element insert methods, the most common being \Ref{wcDocInsertLast} and \Ref{wcElInsertLast}.
 @param  int top            top position in pixels, absolute.
 @param  int left			left position in pixels, absolute.
 @param  int height			height in pixels or zero to not apply the height style and rely on defaults.
 @param  int width          width in pixels or zero to not apply the width style and rely on defaults.
 @param  int zindex			zindex value.
 @param  char *color		Named foreground (text) color or zero to use default, see \Ref{wcElSetStyleColorNamed} for available colors.
 @param  char *bgcolor		named background color or zero to use default.
 @param  char *bgimage      URL of background image (example: "file://images/mainpagebackground.gif") or , or zero for none.
 @param  char *style		Additional styles to apply or zero for none. Any style string may be provided for example" "visibilty:hidden;border-color:black;border-style:solid;border-thickness:2px;"

 @return wcEl the new element or zero if an error is detected or the element can't be created.

 @see wcElNew, wcElSetStyle wcDocInsertFirst wcDocInsertLast wcElInsertFirst wcElInsertLast wcElInsertAfter wcElSetEventHandler
 */
/*****************************************************************************/
wcEL wcNewStyledTextEdit(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style)
{ // Conditionally excluded to satisfy docpp
}
#endif

/*****************************************************************************/
#ifdef RTP_TRACK_LOCAL_MEMORY
wcEL _wcNewStyledTextEdit(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style, const char *file, const long line)
#else
wcEL wcNewStyledTextEdit(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style)
#endif
{
wcEL element;

#ifdef RTP_TRACK_LOCAL_MEMORY
	element = _wcElNew(Ctx,"input type=text",file,line);
#else
	element = wcElNew(Ctx,"input type=text");
#endif
	if (element)
		ApplyCommonStyles(Ctx, ParentEl, insert, element, top, left, height, width, zindex, color, bgcolor, bgimage, style);
	return element;
}

#ifdef NOTDEF	// Conditionally excluded to satisfy docpp
/*****************************************************************************/
// wcNewStyledTextArea
/**

 @memo Constructor function for a textarea element with common styles applied to it.
 @doc Creates a textarea element and customizes it with CSS based on the the values original in the function arguments. If requested the new element is also inserted into the document.
 If you can't conveniently completely style the element your needs with this constructor function you can modify the returned element handle by calling  wcElSetStyle or one of its derivatives.

 @param  wcCtx*  Ctx        The current wc context.
 @param  wcEL ParentEl		If insert is wcTRUE and this value is non-zero, insert the new element into this container element.
 @param  wcBOOL insert
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is non-zero insert the new element into ParentEl.
 <br>&nbsp;&nbsp;If insert is wcTRUE and ParentEl is zero then insert the new element into the document.
 <br> &nbsp;&nbsp;If insert is wcFALSE, the element is not inserted and must be inserted later by calling one of webC explicit element insert methods, the most common being \Ref{wcDocInsertLast} and \Ref{wcElInsertLast}.
 @param  int top            top position in pixels, absolute.
 @param  int left			left position in pixels, absolute.
 @param  int height			height in pixels or zero to not apply the height style and rely on defaults.
 @param  int width          width in pixels or zero to not apply the width style and rely on defaults.
 @param  int zindex			zindex value.
 @param  char *color		Named foreground (text) color or zero to use default, see \Ref{wcElSetStyleColorNamed} for available colors.
 @param  char *bgcolor		named background color or zero to use default.
 @param  char *bgimage      URL of background image (example: "file://images/mainpagebackground.gif") or , or zero for none.
 @param  char *style		Additional styles to apply or zero for none. Any style string may be provided for example" "visibilty:hidden;border-color:black;border-style:solid;border-thickness:2px;"

 @return wcEl the new element or zero if an error is detected or the element can't be created.

 @see wcElNew, wcElSetStyle wcDocInsertFirst wcDocInsertLast wcElInsertFirst wcElInsertLast wcElInsertAfter wcElSetEventHandler
 */
/*****************************************************************************/
wcEL wcNewStyledTextArea(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style)
{ // Conditionally excluded to satisfy docpp
}
#endif

/*****************************************************************************/
#ifdef RTP_TRACK_LOCAL_MEMORY
wcEL _wcNewStyledTextArea(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style, const char *file, const long line)
#else
wcEL wcNewStyledTextArea(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style)
#endif
{
wcEL element;

#ifdef RTP_TRACK_LOCAL_MEMORY
	element = _wcElNew(Ctx,"textarea",file,line);
#else
	element = wcElNew(Ctx,"textarea");
#endif
	if (element)
		ApplyCommonStyles(Ctx, ParentEl, insert, element, top, left, height, width, zindex, color, bgcolor, bgimage, style);
	return element;
}


#ifdef NOTDEF	// Conditionally excluded to satisfy docpp
/*****************************************************************************/
// wcNewStyledImage
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
 @param  char *url          URL of the image.
 @param  char *altext       Text to display if the image can't be loaded.
 @param  int top            top position in pixels, absolute.
 @param  int left			left position in pixels, absolute.
 @param  int height			height in pixels or zero to not apply the height style and rely on values retrieved from the image.
 @param  int width          width in pixels or zero to not apply the width style and rely on values retrieved from the image.
 @param  int zindex			zindex value.
 @param  char *style		Additional styles to apply or zero for none. Any style string may be provided for example" "visibilty:hidden;border-color:black;border-style:solid;border-thickness:2px;"

 @return wcEl the new element or zero if an error is detected or the element can't be created.

 @see wcElNew, wcElSetStyle wcDocInsertFirst wcDocInsertLast wcElInsertFirst wcElInsertLast wcElInsertAfter wcElSetEventHandler
 */
/*****************************************************************************/
wcEL wcNewStyledImage(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, char *url, char *alttext,int top, int left, int height, int width, int zindex, char *style)
{ // Conditionally excluded to satisfy docpp
}
#endif

/*****************************************************************************/
#ifdef RTP_TRACK_LOCAL_MEMORY
wcEL _wcNewStyledImage(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, char *url, char *alttext,int top, int left, int height, int width, int zindex, char *style, const char *file, const long line)
#else
wcEL wcNewStyledImage(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, char *url, char *alttext,int top, int left, int height, int width, int zindex, char *style)
#endif
{
wcEL element;
// Not done..

#ifdef RTP_TRACK_LOCAL_MEMORY
	element = _wcElNew(Ctx,"img",file,line);
#else
	element = wcElNew(Ctx,"img");
#endif
	if (element)
	{
		wcElSetSrc(element,url, wcTRUE);
		ApplyCommonStyles(Ctx, ParentEl, insert, element, top, left, height, width, zindex, 0, 0, 0, style);
		wcElRefresh(element);
	}
	return element;
}
