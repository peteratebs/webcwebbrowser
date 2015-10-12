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
// Header files
/*****************************************************************************/

//#include "../include/NewApi.h"
#include "wcapi.h"

#include "dflow.hpp"
#include "helement.hpp"
#include "htmlbrow.hpp"
#include "webcassert.h"
#include "htmlfind.hpp"
#include "htmldoc.hpp"
#include "webcmem.h"
#include "rtptime.h"

/*****************************************************************************/
// wcElSetInnerHtml
/**

 @memo Display html source inside a container element.
 @doc Set an HTML element's inner html. Parses the provided Html source code and insert it into the the specified container element.
 The existing contents are freed and replaced with the elements created from the html text representation.

 <pre>
 Examples:
     wcElSetInnerHtml(Ctx, "<table border=\"1\">\<tr><td>(1,1)</td><td>(1,2)</td></tr></table>");
 </pre>

 @param  wcEL 		Elw    		-   The containing element.
 @param  char 		*NewHtml    -   Html source code.

 @return The new element or zero on failure.

 @see wcElGetInnerHtml
 */
/*****************************************************************************/

/*****************************************************************************/
void wcElSetInnerHtml(wcEL El,	char * newhtml)
{
  #if (WEBC_SUPPORT_UNICODE)
   	WEBC_CHAR *pustring;
	pustring = (WEBC_CHAR *) rtp_malloc((rtp_strlen(newhtml)+1)*2);
	if (pustring)
	{
		webc_c_strcpy(pustring, newhtml);
		HELEMENT_OBJ(El)->SetInnerHTML(pustring);
		rtp_free(pustring);
	}
  #else
	HELEMENT_OBJ(El)->SetInnerHTML(newhtml);
  #endif
}

HTMLElement *jhutil_CreateElement(HTMLElementType eType, HTMLTagParse *tag, HTMLDocument *pDoc);

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

 @see
 */
/*****************************************************************************/
#include "hedit.hpp"
#include "hchkbox.hpp"
#include "hradio.hpp"
#include "himage.hpp"
#include "hbutton.hpp"

wcEL wcElNew(wcCtx* Ctx, char *_name)
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
				WEBC_NEW(pElement, HTMLEditString(tag, (HTMLDocument *)Ctx->hdoc));
			    break;
		    case HTML_INPUT_TYPE_PASSWORD:
//			input_elem = Password(td);
			    break;
		    case HTML_INPUT_TYPE_CHECKBOX:
				WEBC_NEW(pElement,HTMLCheckbox(tag, (HTMLDocument *)Ctx->hdoc));
			    break;
		    case HTML_INPUT_TYPE_RADIO:
				WEBC_NEW(pElement,HTMLRadioButton(tag, (HTMLDocument *)Ctx->hdoc));
			    break;
		    case HTML_INPUT_TYPE_SUBMIT:
//			input_elem = InputSubmit(td);
			    break;
		    case HTML_INPUT_TYPE_HIDDEN:
//			input_elem = InputHidden(td);
			    break;
		    case HTML_INPUT_TYPE_IMAGE:
				WEBC_NEW(pElement,HTMLImage(tag, (HTMLDocument *)Ctx->hdoc));
//			input_elem = Image(td);
			    break;
		    case HTML_INPUT_TYPE_RESET:
//			input_elem = InputReset(td);
			    break;
		    case HTML_INPUT_TYPE_BUTTON:
				WEBC_NEW(pElement,HTMLButton(tag, (HTMLDocument *)Ctx->hdoc));
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
#if (WEBC_SUPPORT_JSCRIPT)
	HTMLElement *pElement = jhutil_CreateElement(eType, tag, pDoc);
#else
	HTMLElement *pElement = 0;
#endif
	return ((wcEL)pElement);
}

/*****************************************************************************/
// wcELRemoveChild
/**

 @memo Remove a child element from its parent.
 @doc Remove a child element from its parent. The child element must be contained by the parent.
 The child and parent are unlinked. The child is not deleted, it may be deleted or inserted into another
 container.

 @param  wcEL          Parent        -   The parent "container" element.
 @param  wcEL          Child        -   The child element to remove.

 @return wcTRUE    if the arguments non NULL. It is the callers responsibilty to ensure Child is a direct child of parent.

 @see
 */
/*****************************************************************************/

wcBOOL wcELRemoveChild (wcEL parent,wcEL child)
{
    if (!parent || !child)
        return wcFALSE;
    HELEMENT_OBJ(parent)->Remove(HELEMENT_OBJ(child));
    HELEMENT_OBJ(parent)->Update(0,(WEBC_PFBYTE)1);
    HELEMENT_OBJ(parent)->Refresh();
    return wcTRUE;
}

/*****************************************************************************/
// wcELDeleteChildren
/**

 @memo Delete all child elements from a container element.
 @doc   Delete all child elements from element.

 @param  wcEL          Parent        -   The parent "container" element.

 @return wcTRUE    if the argument is non NULL.

 */
/*****************************************************************************/

wcBOOL wcELDeleteChildren (wcEL element)
{
    if (!element)
        return wcFALSE;
    HELEMENT_OBJ(element)->DeleteChildren();
    HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
    HELEMENT_OBJ(element)->Refresh();
    return wcTRUE;
}

/*****************************************************************************/
// wcELDelete
/**

 @memo Delete an element and all of its children.
 @doc   Delete element and all of its children.

 @param  wcEL          Element        -   The element to delete.

 @return wcTRUE    if the argument is non NULL.

 */
/*****************************************************************************/
wcBOOL wcELDelete(wcEL element)
{
    if (!element)
        return wcFALSE;
    wcELDeleteChildren (element);
    delete HELEMENT_OBJ(element);
    return wcTRUE;
}

/*****************************************************************************/
// wcELInsertFirst
/**

 @memo Insert an element into the first position of a containing element.
 @doc   Insert an element into the first position of a containing element.

 @param  wcEL          Parent        -   The containing element.
 @param  wcEL          Child        -   The element to insert.

 @return wcTRUE    if the arguments are non NULL.

 */
/*****************************************************************************/
wcBOOL wcELInsertFirst(wcEL element,wcEL child)
{
    if (!element || !child)
        return wcFALSE;
    HELEMENT_OBJ(element)->InsertFirst(HELEMENT_OBJ(child));
    HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
    HELEMENT_OBJ(element)->Refresh();
    return wcTRUE;
}
/*****************************************************************************/
// wcELInsertLast
/**

 @memo Insert an element into the last position of a containing element.
 @doc   Insert an element into the last position of a containing element.

 @param  wcEL          Parent        -   The containing element.
 @param  wcEL          Child        -   The element to insert.

 @return wcTRUE    if the arguments are non NULL.

 */
/*****************************************************************************/
wcBOOL wcELInsertLast(wcEL element,wcEL child)
{
    if (!element || !child)
        return wcFALSE;
    HELEMENT_OBJ(element)->InsertLast(HELEMENT_OBJ(child));
    HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
    HELEMENT_OBJ(element)->Refresh();
    return wcTRUE;
}

/*****************************************************************************/
// wcELInsertAfter
/**

 @memo Insert an element after a specific element.
 @doc   Insert an element after a specific element in a container.

 @param  wcEL          Parent            -   The containing element.
 @param  wcEL          CurChild        -   The element to insert after.
 @param  wcEL          NewChild        -   The element to insert.

 @return wcTRUE    if the arguments are non NULL.

 */
/*****************************************************************************/

wcBOOL wcELInsertAfter(wcEL Parent,wcEL CurChild, wcEL NewChild)
{
    if (!Parent || !NewChild || !CurChild)
        return wcFALSE;
    HELEMENT_OBJ(Parent)->InsertAfter(HELEMENT_OBJ(NewChild),HELEMENT_OBJ(CurChild));
    HELEMENT_OBJ(Parent)->Update(0,(WEBC_PFBYTE)1);
    HELEMENT_OBJ(Parent)->Refresh();
    return wcTRUE;
}

/*****************************************************************************/
// wcELInsertBefore
/**

 @memo Insert an element before a specific element.
 @doc   Insert an element before a specific element in a container.

 @param  wcEL          Parent            -   The containing element.
 @param  wcEL          CurChild        -   The element to insert before.
 @param  wcEL          NewChild        -   The element to insert.

 @return wcTRUE    if the arguments are non NULL.

 */
/*****************************************************************************/
wcBOOL wcELInsertBefore(wcEL Parent,wcEL CurChild,wcEL NewChild)
{
    if (!Parent || !NewChild || !CurChild)
        return wcFALSE;
    HELEMENT_OBJ(Parent)->InsertBefore(HELEMENT_OBJ(NewChild),HELEMENT_OBJ(CurChild));
    HELEMENT_OBJ(Parent)->Update(0,(WEBC_PFBYTE)1);
    HELEMENT_OBJ(Parent)->Refresh();
    return wcTRUE;
}


/*****************************************************************************/
// wcELIdMatches
/**

 @memo  Compare an element's id to a string.
 @doc   Compare an element's id to a string, return true if they match.


 @param  wcEL          El        -   The element to compare.
 @param  char*         IdStr   -   The string to match.

 @return wcTRUE    if the element has an assigned id and they match wcFALSE otherwise.

 @see
 */
/*****************************************************************************/


wcBOOL wcELIdMatches(wcEL element, char *string)
{
    if (HELEMENT_OBJ(element)->Id() && string && !webc_c_stricmp(HELEMENT_OBJ(element)->Id(), string))
        return wcTRUE;
    return wcFALSE;
}



HTMLElement *jhutil_CloneElement(HTMLElement *clone, WEBC_BOOL bCloneChildren);
/*****************************************************************************/
// wcElClone
/**

 @memo  Create an element by cloning.
 @doc Dynamically create a new element by cloning an existing element. If bCloneChildren is true, clone the element's children too.
 The element must be inserted into the document before much can be done with it.
  Note:

 @param  wcEL 	El			    -   The element to clone.
 @param  wcBOOL	bCloneChildren	-	wcTRUE or wcFALSE

 @return The new element or zero on failure.

 @see
 */
/*****************************************************************************/


wcEL wcElClone(wcEL element, wcBOOL bCloneChildren)
{
#if (WEBC_SUPPORT_JSCRIPT)
	return (wcEL) jhutil_CloneElement((HTMLElement *) element, (WEBC_BOOL) bCloneChildren);
#else
	return 0;
#endif
}


/*****************************************************************************/
// wcElSetStyle
/**
 @memo Set element style attributes.
 @doc  Set style value(s) for an element and redraw the element.

 @param wcEL element 	- The element to set style info for.
 @param char* style   	- Cascading style sheet source data (e.g. "top:20px;left:10px;")
 @param wcBOOL doUpdate	- Pass wcTRUE to queue the element for redraw after changing the style.

 @return wcTRUE    if the arguments are non NULL.
 @see
 */
/*****************************************************************************/

wcBOOL wcElSetStyle(wcEL element, char *_style,wcBOOL doUpdate)
{
#if (WEBC_SUPPORT_STYLE_SHEETS)
HTMLDocument *pDocument;
	if (!element)
		return wcFALSE;
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR style[HTML_TEMP_STR_LEN];
	webc_c_strncpy(style, _style, HTML_TEMP_STR_LEN-1);
	style[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *style = _style;
  #endif
	pDocument = HELEMENT_OBJ(element)->GetDocument();
	if (pDocument)
	{
		pDocument->GetCSSContext()->ParseElementStyle(HELEMENT_OBJ(element), style, pDocument->CurrentURL());
		if (doUpdate)
		{
			HELEMENT_OBJ(element)->StyleUpdate();
			HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
			HELEMENT_OBJ(element)->Refresh();
		}
	}
#endif
	return wcTRUE;
}

/*****************************************************************************/
// wcElSetId
/**
 @memo Assign an id to an element.
 @doc           Assign an id to an element.

 @param wcEL element 	- The element to set style info for.
 @param char* Id   		- Id for the element.

 @return wcTRUE    if the arguments are non NULL.
 @see
 */
/*****************************************************************************/

wcBOOL wcElSetId(wcEL element, char *_Id)
{
	if (!element)
		return wcFALSE;
#if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR Id[HTML_TEMP_STR_LEN];
	webc_c_strncpy(Id, _Id, HTML_TEMP_STR_LEN-1);
	Id[HTML_TEMP_STR_LEN-1] = 0;
#else
  	WEBC_CHAR *Id = _Id;
#endif
	HELEMENT_OBJ(element)->SetId(Id);
	return wcTRUE;
}



/*****************************************************************************/
// wcElSetChecked
/**
 @memo          Check or uncheck a checkbox or radio button element
 @doc           If the given element handle is a radio button element or a checkbox element and
  				If passed wcTRUE setchecked, if wcFALSE clear checked.
 it is checked.
 @param wcEL element 	- The element to set the checked attribute on
 @param wcBOOL checked 	- If wcTRUE set the checked state, if wcFALSE clear it.
 @return        The wcTRUE if the element is radio button element or a checkbox element.
 @see           wcElGetChecked, wcElSetValue
 */
/*****************************************************************************/

wcBOOL wcElSetChecked(wcEL element,wcBOOL checked)
{
HTMLElementType theType;
	if (!element)
		return (wcFALSE);
	theType=wcElGetType(element);
	if (theType==HTML_CHECKBOX_ELEMENT || theType==HTML_RADIO_BUTTON_ELEMENT)
	{
		HELEMENT_OBJ(element)->SetChecked(checked);
		HELEMENT_OBJ(element)->Update(0,0);
		HELEMENT_OBJ(element)->Refresh();
		return (wcTRUE);
	}
	else
		return (wcFALSE);

}

/*****************************************************************************/
// wcElSetValue
/**
 @memo Assign a value to an element.
 @doc           Set the value propery of an element.

 @param wcEL element 	- The element to set style info for.
 @param char* Val   	- New Value for the element.

 @return wcTRUE    if the arguments are non NULL.
 @see
 */
/*****************************************************************************/

wcBOOL wcElSetValue(wcEL element, char *_Val)
{
	if (!element)
		return wcFALSE;
#if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR Val[HTML_TEMP_STR_LEN];
	webc_c_strncpy(Val, _Val, HTML_TEMP_STR_LEN-1);
	Val[HTML_TEMP_STR_LEN-1] = 0;
#else
  	WEBC_CHAR *Val = _Val;
#endif
	HELEMENT_OBJ(element)->SetValue(Val);
	return wcTRUE;
}



/*****************************************************************************/
// wcElGetChecked
/**
 @memo          Get the status of a checkbox or radio button
 @doc           Return wcTRUE if the given element handle is a radio button element and selectyed or a
                checkbox element and it is checked.
 @param wcEL element 	- The element to test the checked attribute for.
 @return        The wcTRUE if the element is checked wcFALSE if it is not checked
 @see           wcElSetChecked, wcElSetValue
 */
/*****************************************************************************/

wcBOOL wcElGetChecked(wcEL element)
{
HTMLElementType theType;
	if (!element)
		return (wcFALSE);

	theType=wcElGetType(element);
	if (theType==HTML_CHECKBOX_ELEMENT || theType==HTML_RADIO_BUTTON_ELEMENT)
		return (wcBOOL ) HELEMENT_OBJ(element)->Checked();
	else
		return (wcFALSE);

}

/*****************************************************************************/
// wcELGetId
/**

 @memo   Get the id attribute.
 @doc           Returns the unique id string of this element.

 @param  wcEL          Element       -   The element.

 @return char *Id     The id if the element has one, or zero. (note: You must call wcCtxtRelease, to release the storage).

 */
/*****************************************************************************/

char *wcELGetId(wcCtx* Ctx,wcEL element)
{
    if (element && HELEMENT_OBJ(element)->Id())
	    return wcCtxtAppendGarbage(Ctx, webc_MallocASCIIString(HELEMENT_OBJ(element)->Id()));
	else
	    return(0);
}

/*****************************************************************************/
// wcELGetValue
/**

 @memo   Get the value attribute.
 @doc           Returns the value field of this element. TODO: Booleans check boxes ?

 @param  wcEL          Element       -   The element.

 @return char *Value     The value if the element has one, or zero. (note: You must call wcCtxtRelease, to release the storage).


 */
/*****************************************************************************/

char *wcELGetValue(wcCtx* Ctx,wcEL element)
{
    if (element && HELEMENT_OBJ(element)->Value())
	    return wcCtxtAppendGarbage(Ctx, webc_MallocASCIIString(HELEMENT_OBJ(element)->Value()));
	else
	    return(0);
}


/*****************************************************************************/
// wcELGetType
/**
 @memo   Get the tag type of this element.
 @doc    Returns the type identifier of this element.

 @param  wcEL          Element       -   The element.

 @return HTMLElementType Type     The type of element, HTML_ADDRESS_ELEMENT, HTML_BUTTON_ELEMENT etc (see htmlsdk.h)

 @param element The element whose type to return
 @return        The element type
 @see
 */
/*****************************************************************************/

HTMLElementType wcElGetType(wcEL element)
{
	if (!element)
		return (HTMLElementType)0;
	return (HELEMENT_OBJ(element)->Type());
}


/*****************************************************************************/
// Local types
/*****************************************************************************/
class wcCFunctionHandler : public HTMLEventHandler
{
public:

	wcCFunctionHandler(WEBC_CHAR *pName, wcEventCallback Function)
	{
		if (pName)
		{
			webc_strncpy(mpName, pName, 35);
			mpName[35] = '\0';
		}
		else
		{
			mpName[0] = '\0';
		}
		mFunction = Function;
	}
	~wcCFunctionHandler() {}


	virtual HTMLEventStatus Event(HTMLElement *pElement, HTMLEvent *pEvent, WEBC_CHAR *pParam = 0)
	{
		HTMLEventStatus result = HTML_EVENT_STATUS_CONTINUE;

		if (mFunction)
		{
			HTMLDocument *pDoc;
			HTMLBrowser  *pBrowser;
			wcCtx _Ctx;
			char nphoney[4];

			pDoc = (pElement)? pElement->GetDocument() : 0;
			pBrowser = (pDoc)? pDoc->GetBrowser() : 0;

			wcCtxtInit(&_Ctx,  (wcBROW) pBrowser, (wcDOC) pDoc);
			char *param = webc_arg_to_char(pParam);
			if (!param)
				param = &nphoney[0];

			result = mFunction(&_Ctx, (wcEL)pElement, pEvent, param);
			wcCtxtRelease(&_Ctx);
			if (param != &nphoney[0])
				webc_arg_done(param);
		}
		return (result);
	}

	virtual WEBC_CHAR *Name(void) { return mpName; }
	WEBC_CHAR mpName[36];
	wcEventCallback mFunction;
};



/*****************************************************************************/
// wcElSetEventHandler
/**
 @memo          Assign an event handler for this element.
 @doc           Push the previous event handler for this element on an internal stack and then
 				set a new handler for events targetted at the specified element.
				The previous event handler may be restored later by calling wcElpopEventHandler.

		 Note: wcElSetEventHandler pushes the previous handler onto an internal stack. You must call
		 wcElPopEventHandler at a later time if your application requires you to restore the previous handler when the current algorith is completed.
		 Most applications don't need a stack more than one level deep but more complex interfaces like drag and drop and slide under windows use this capability.

 @param  wcEL          		Element       -   The element.
 @param  wcEventCallback 	cb			  -   Function to call for when an event occur.
 <pre>
 The callback function is of type:
 	typedef HTMLEventStatus (*wcEventCallback) (wcCtx* Ctx,	wcEL element, HTMLEvent* event,	char* param);
	where Ctx and Element are as in other documented routines.
	param is typically unused..
	See below for a description of event types and the event structure.

	Example:

	Attach a callback event handler that displays all events to a table row.
	Demonstrate restoring the previous handler by Popping the current handler
	on click.

		static HTMLEventStatus wcTestEventCallback(wcCtx* Ctx,	wcEL element,HTMLEvent* event,	char* param);

		wcBOOL &Error;
		wcEL El = wcELFind (&_Ctx, 0, "/table0/row0/col0", &Error);
		wcElSetEventHandler(&_Ctx, wcTestEventCallback);


static char *EventToString(HTMLEventType type);
static HTMLEventStatus wcTestEventCallback(wcCtx* Ctx,	wcEL element,HTMLEvent* event,	char* param)
{

	// print the event name
	rtp_printf("Event>>: %s\n", EventToString(event->type));
	// dispatch on event ids to appropriate responses.

    switch (event->type)
    {
		case HTML_EVENT_CLICK            :
		// Revert to previous handler when clicked.
			wcElPopEventHandler(element);
			return (HTML_EVENT_STATUS_HALT);
			break;
		case HTML_EVENT_KEYDOWN             :
		case HTML_EVENT_KEYPRESS            :
		case HTML_EVENT_KEYUP               :
		case HTML_EVENT_DBLCLICK            :
		case HTML_EVENT_MOUSEDOWN           :
		case HTML_EVENT_MOUSEMOVE           :
		case HTML_EVENT_MOUSEOUT            :
		case HTML_EVENT_MOUSEOVER           :
		case HTML_EVENT_MOUSEUP             :
			break;
		case HTML_EVENT_FOCUS               :
		case HTML_EVENT_UNFOCUS             :
		case HTML_EVENT_LOAD                :
		case HTML_EVENT_UNLOAD              :
		case HTML_EVENT_SUBMIT              :
		case HTML_EVENT_CHANGE              :
		case HTML_EVENT_EDIT                :
		case HTML_EVENT_RESET               :
			break;
		case HTML_EVENT_OBJECT_CREATED       :
		case HTML_EVENT_OBJECT_DESTROYED     :
		case HTML_EVENT_ATTRIBUTE_PARSED     :
		case HTML_EVENT_LOSECAPTURE          :
		case HTML_EVENT_TIMER                :
		case HTML_EVENT_RENDER	            :
		case HTML_FIRST_USER_EVENT           :
		default:



		break;
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}


	=================================================================
	The following events are reported along with user assignable event.

	HTML_EVENT_CLICK               Single mouse click
	HTML_EVENT_DBLCLICK            Double mouse click
	HTML_EVENT_KEYDOWN             Key pressed
	HTML_EVENT_KEYPRESS            Key pressed and released
	HTML_EVENT_KEYUP               Key released
	HTML_EVENT_MOUSEDOWN           Mouse button pressed
	HTML_EVENT_MOUSEMOVE           Mouse movement over this object
	HTML_EVENT_MOUSEOUT            Mouse pointer exits the client area of this object
	HTML_EVENT_MOUSEOVER           Mouse pointer enters the client area
	HTML_EVENT_MOUSEUP             Mouse button released
	HTML_EVENT_FOCUS               Object has received input focus
	HTML_EVENT_UNFOCUS             Object will lose input focus
	HTML_EVENT_LOAD                Load completes
	HTML_EVENT_UNLOAD              Object is discarded
	HTML_EVENT_SUBMIT              For forms only; submit is pressed
	HTML_EVENT_CHANGE              Input text was edited
	HTML_EVENT_EDIT                Finished Editing on a Textbox
	HTML_EVENT_RESET               For forms only; reset is pressed
	HTML_EVENT_OBJECT_CREATED       HTMLElement object created
	HTML_EVENT_OBJECT_DESTROYED     HTMLElement object destroyed
	HTML_EVENT_ATTRIBUTE_PARSED     Attribute parsed
	HTML_EVENT_LOSECAPTURE          Mouse capture released
	HTML_EVENT_ONREADYSTATECHANGE   Ajax event (XMLHttpRequest.onreadystatechange event) not implemented
	HTML_EVENT_TIMER                A timer message sent by user code via wcQueueEventTimed
	HTML_EVENT_RENDER	            A notification message send by the display processing loop about the element. Useful for doing things like retrieveing an element's screen position after it was automatically placed in non fixed formatted
	content.
	HTML_FIRST_USER_EVENT           This and higher event numbers are available for application programming.

	Event handlers must return one of the following:
		HTML_EVENT_STATUS_DONE       Handling of this event is complete don't pass it to parent handlers.
		HTML_EVENT_STATUS_CONTINUE   Pass the event to parent handler listers and perform any default processing.
		HTML_EVENT_STATUS_HALT       Cancel processing operation. Examples include refusing to give up focus and cancel mouse processing.
		HTML_EVENT_STATUS_ERROR

	The event structure has the following fields.
        struct s_HTMLEvent
        {
            HTMLEventType type;
            union
            {
                struct
                {
                    long x;
                    long y;
                } position;
                struct
                {
                    const WEBC_CHAR* name;
                    const WEBC_CHAR* value;
                } attribute;
                struct
                {
                    enum NotifierType Notifier;
                    enum NotifyEventType NotifyEvent;
                } NotifyMessage;
                WebKey         key;
                WEBC_CHAR*     string;
                unsigned char* buffer;
            } data;
            HELEMENT_HANDLE elem;
            HELEMENT_HANDLE target;
            HELEMENT_HANDLE srcElement;
            WEBC_UINT8 flags;                 see flag definitions below
            int returnValue;
		};

        Event flags that may be set by handlers to control processing as with the return codes.
        The return codes are more often used by Javascript but they can be used for C event dispatchers as well.

        HTML_EVENT_FLAG_CANCEL_BUBBLE    - These flags values may be set by handlers to
        HTML_EVENT_FLAG_CANCEL_DEFAULT   - control processing as with the return codes.
        HTML_EVENT_FLAG_CANCEL_SET_FOCUS - Javascript events use these flags too.

        Keyboard metakey states are provided on mouse events an key events.
        HTML_EVENT_FLAG_SHIFT_DOWN       - Keyboard metakey states provided on mouse event or keypress.
        HTML_EVENT_FLAG_CTRL_DOWN        -
        HTML_EVENT_FLAG_ALT_DOWN         -
 </pre>

 @return        Nothing
 @see
 */
/*****************************************************************************/

void wcElSetEventHandler(HELEMENT_HANDLE element, wcEventCallback cb)
{
    if (element)
    {
        wcCFunctionHandler* eventHandler;
        WEBC_NEW(eventHandler, wcCFunctionHandler(0,cb));
        if (eventHandler)
        {
            HTMLEvent the_event;
//            HELEMENT_OBJ(element)->SetElementEventHandler(eventHandler);
            HELEMENT_OBJ(element)->PushElementEventHandler(eventHandler);
            the_event.type = HTML_EVENT_OBJECT_CREATED;
            webc_TriggerEvent(element, &the_event);
        }
   }
}

/*****************************************************************************/
// wcElPopEventHandler
/**
 @memo          Restore the previous event handler for this element.
 @doc           Pop the previous event handler for this element from an internal stack and make it
 				the new event handler for the specified element. This function returns the event handler to
				the handler prior to the last call to wcElSetEventHandler. This push pop capability simplifies implementing
				effects like drag and drop, where a new event handlers are assigned when dragging starts, and released when it stops.

		 Note: See wcElSetEventHandler.

 @param  wcEL          		Element       -   The element.

  <pre>
	Example:

	See the example for wcElSetEventHandler.

			case HTML_EVENT_CLICK            :
		// Revert to previous handler when clicked.
			wcElPopEventHandler(element);
			return (HTML_EVENT_STATUS_HALT);
 </pre>


 @return        Nothing
 */
/*****************************************************************************/

void wcElPopEventHandler(wcEL  element)
{
    if (element)
    {
   		HELEMENT_OBJ(element)->PopElementEventHandler();
   }
}

/*****************************************************************************/
// wcTriggerEvent
/**
 @memo  Send an event to an element immediately or with delay.
 @doc           Pass an event to an element's event handler routine.

 @param  wcEL          		Element     -   The element.
 @param  HTMLEvent *		pEvent    	-   Initialized event structure to pass to the handler.
 @param  int 				delay   	-   Delay in miliseconds before triggering the event. Zero for no delay.


 @return        HTMLEventStatus. If Delay is non-zero HTML_EVENT_STATUS_CONTINUE otherwise the return value from the element's event handler.

 */
/*****************************************************************************/

HTMLEventStatus wcTriggerEvent(wcEL El, HTMLEvent *pEvent, int delay)
{
	if (delay == 0)
		return webc_TriggerEvent((HELEMENT_HANDLE) El, pEvent);
	else if (webc_ElementGetDocument((HELEMENT_HANDLE) El))
	{
		webc_BrowserQueueTimedEvent(webc_DocGetBrowser(webc_ElementGetDocument((HELEMENT_HANDLE) El)),(HELEMENT_HANDLE) El,pEvent,(WEBC_INT32) delay);
		return HTML_EVENT_STATUS_CONTINUE;
	}
	else
		return HTML_EVENT_STATUS_CONTINUE;

}

/*****************************************************************************/
// wcElGetPosWin
/**
 @memo  Get the displayed position of an element.
 @doc           Get the current position of an element. The values returned make up the actual bounding rectangle surrounding
 the element the last time it was displayed.
 Note: wcElGetPosWin is useful for obtaining the coordinates of elements that were automatically sized or
 positioned by webC. It returns the coordinates the last time an element was drawn.
 The routine can not be used in screen constructors on elements that have not been drawn yet.
 If an element is manually positioned it's coordinates can be optained before it is drawn by calling wcElGetPosStyle.

 @param  wcEL          		Element     -   The element.
 @param  int*          		top      	-   top goes here. If NULL no result is returned for top.
 @param  int*          		left      	-   left goes here. If NULL no result is returned for leftl.
 @param  int*          		height     	-   height goes here. If NULL no result is returned for height.
 @param  int*          		width     	-   width goes here. If NULL no result is returned for width.

 <pre>
 	Example:
		The following event handler fragment handles the HTML_EVENT_WINDOW_REFRESHED event on an auto positioned element.
		It updates the element's style attributes from the auto positioned values of top, left, height, width.
		This operation is important because, to operate properly, operations like drag, resize, and window manager functions require
		style based positioning. This technique may be used to allow auto formatting (like center, left right etc) to
		initially position certain elements and still use algorithms that rely on absolute positioning.

    if (event->type == HTML_EVENT_WINDOW_REFRESHED)
	{
	char ascii_buff[256];
	int top,left,width,height;
		wcElGetPosWin(element, &top,&left,&width,&height);
		rtp_sprintf(ascii_buff, "position:absolute;left:%dpx;top:%dpx;height:%dpx;width:%dpx;", left, top, height, width);
		== Set the style but don't trigger a redraw
		wcElSetStyle(element, ascii_buff,wcFALSE);
	}
 </pre>


 @return        wcTRUE if the arguments are valid and the element has been drawn.
 @return		wcFALSE if the element has not yet been drawn or it does not have a display element associated with it.

 @see wcElGetPosStyle,wcElSetPosStyle
 */
/*****************************************************************************/

wcBOOL wcElGetPosWin(wcEL element, int *top, int *left, int *width, int *height)
{
HTMLElement *pContainer;
DisplayElement *pDisplay =0;
	if (!element)
		return(wcFALSE);
	pContainer = (HTMLElement *)element;
	pDisplay = pContainer->GetDisplayElement();

	if (pDisplay)
	{
		if (pDisplay->mRect.left==0 &&  pDisplay->mRect.top==0 && pDisplay->mRect.right==0 && pDisplay->mRect.bottom==0)
		{
			if (top) 	*top   =0;
			if (left)	*left  =0;
			if (width) 	*width =0;
			if (height) *height=0;
		}
		else
		{
			if (top) 	*top =pDisplay->mRect.top;
			if (left)	*left =pDisplay->mRect.left;
			if (width) 	*width =pDisplay->mRect.Width();
			if (height) *height=pDisplay->mRect.Height();
		}
		return(wcTRUE);
	}
	else
		return(wcFALSE);
}

/*****************************************************************************/
// wcElGetPosStyle
/**
 @memo  Get the style positioning values of an element.
 @doc           Get the current position of an element from its CSS or HTML positioning. Fails if the element has not
 been positioned.
 Note: wcElGetPosStyle can only obtain the coordinates of elements that were manually sized and positioned
 with CSS or HTML attributes. The routine can be used in screen constructors on elements that have been positioned but not yet drawn.
 After an is drawn, whether manually positioned or not, coordinates may be obtained by calling wcElGetPosWin.
 Note: top, left height and width are returned as zero if they have not been set.

 @param  wcEL          		Element     -   The element.
 @param  int*          		top      	-   top goes here. If NULL no result is returned for top.
 @param  int*          		left      	-   left goes here. If NULL no result is returned for left.
 @param  int*          		height     	-   height goes here. If NULL no result is returned for height.
 @param  int*          		width     	-   width goes here. If NULL no result is returned for width.

 @return        wcTRUE if the argument is valid.

 @see wcElGetPosStyle,wcElSetPosStyle
 */
/*****************************************************************************/

wcBOOL wcElGetPosStyle(wcEL element, int *top, int *left, int *width, int *height)
{
CSSPropertyValue value;
	if (!element)
		return(wcFALSE);

	if (top) 	*top   =0;
	if (left)	*left  =0;
	if (width) 	*width =0;
	if (height) *height=0;

	if ( top && (
		(HELEMENT_OBJ(element)->GetStyleFromCSS(CSS_PROPERTY_TOP, &value) >= 0) ||
		(HELEMENT_OBJ(element)->GetStyleFromHTML(CSS_PROPERTY_TOP, &value) >= 0)))
		{
		*top = (int) value.length.val.dec;
		}
	if (left && (
		(HELEMENT_OBJ(element)->GetStyleFromCSS(CSS_PROPERTY_LEFT, &value) >= 0) ||
		(HELEMENT_OBJ(element)->GetStyleFromHTML(CSS_PROPERTY_LEFT, &value) >= 0)))
		{
			*left = (int) value.length.val.dec;
		}
	if (width && (
		(HELEMENT_OBJ(element)->GetStyleFromCSS(CSS_PROPERTY_WIDTH, &value) >= 0) ||
		(HELEMENT_OBJ(element)->GetStyleFromHTML(CSS_PROPERTY_WIDTH, &value) >= 0)))
		{
			*width = (int) value.length.val.dec;
		}
	if (height && (
		(HELEMENT_OBJ(element)->GetStyleFromCSS(CSS_PROPERTY_HEIGHT, &value) >= 0) ||
		(HELEMENT_OBJ(element)->GetStyleFromHTML(CSS_PROPERTY_HEIGHT, &value) >= 0)))
		{
			*height = (int) value.length.val.dec;
		}
	return(wcTRUE);
}


/*****************************************************************************/
// wcPushData
/**

 @memo   Associate a data pointer with a window or element within a window.
 @doc    The data is not used by WebC. Typical use of private data is to associate
         application specific data with the window when it is created, perhaps at start-up
         and to attach element specific data to screen objects as screens are loaded and created.
         Event handlers can then access this data and perform application specific operations based on
		 their contents and perform updates on their contents.

		 Note: wcPushData places the argument onto an internal stack. You must call wcGetData to access the private data pointer that was
		 last pushed. When you are done using the data pointer stack you may pop your value and expose the previous private data pointer by
		 calling wcPopData. Most applications don't need a stack more than one level deep but more complex interfaces involving drag and drop, slide under windows use this capability.

 @param  wcCtx* Ctx 	- Context to associate private data with.
 @param  wcEL   Element - Element to associate private data with. If this value is zero the data is associated with the context and not a specific element within the conmtext.
 @param  void*  data    - Data pointer to push.

 @return Nothing
 */
/*****************************************************************************/

void wcPushData(wcCtx *Ctx, wcEL  Element, void* data)
{
	if (Element)
		HELEMENT_OBJ(Element)->PushPrivateData(data);
	else
		HBROWSER_OBJ(Ctx->hbrowser)->PushPrivateData(data);
}


/*****************************************************************************/
// wcGetData
/**

 @memo   Retrieve a data pointer from a window or element within a window.
 @doc    Retrieves a data pointer that was placed by wcPushData.

		 See wcPushData for an explanation about webC private data.

 @param  wcCtx* Ctx 	- Context to retrieve private data from.
 @param  wcEL   Element - Element to retrieve private data from. If this value is zero the data is retrieved from the context and not a specific element within the conmtext.

 @return void*  data    - Data pointer that was last pushed.
 */
/*****************************************************************************/

void * wcGetData(wcCtx *Ctx, wcEL  Element)
{
void *pdata=0;
	if (Element)
		pdata = (void *) HELEMENT_OBJ(Element)->GetFromPrivateDataStack();
	else
		pdata = (void *) HBROWSER_OBJ(Ctx->hbrowser)->GetFromPrivateDataStack();
	return pdata;
}
/*****************************************************************************/
// wcPopData
/**

 @memo   Pop the current data pointer from the a window or element within the window.

 @param  wcCtx* Ctx 	- Context to retrieve private data from.
 @param  wcEL   Element - Element whose data stack is to be popped. If this value is zero the context data stack is popped.

 @return nothing
 */
/*****************************************************************************/

void wcPopData(wcCtx *Ctx, wcEL  Element)
{
	if (Element)
		HELEMENT_OBJ(Element)->PopPrivateData();
	else
		HBROWSER_OBJ(Ctx->hbrowser)->PopPrivateData();
}
