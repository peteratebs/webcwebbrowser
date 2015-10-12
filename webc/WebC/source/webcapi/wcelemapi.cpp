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
#include "hoption.hpp"
#include "webcmem.h"
#include "rtptime.h"
#include "hselect.hpp"
#include "rtpprint.h"


/*****************************************************************************/
// wcElGetFrameDocument
/**
 @memo          Get a frame's child document
 @doc           Gets a frame's child document.
 @param element The frame element handle
 @return        The handle of the HTML Document or WEBC_NULL if this element is not a frame.
 @see           wcBrowGetDocument
 */
/*****************************************************************************/

wcDOC wcElGetFrameDocument(wcEL element)
{
	if (!element)
		return 0;
  #if (WEBC_SUPPORT_FRAMES)
	if (HELEMENT_OBJ(element)->Type() == HTML_FRAME_ELEMENT ||
	    HELEMENT_OBJ(element)->Type() == HTML_IFRAME_ELEMENT)
	{
		return (wcDOC) (((HTMLFrame*)(element))->FrameDocument());
	}
  #endif // WEBC_SUPPORT_FRAMES

	return WEBC_NULL;
}


/*****************************************************************************/
// wcElGetDocument
/**
 @memo          Get the handle of the document containing a given element
 @doc           Gets the handle of the document that contains the given
                element.
 @param element The element whose document handle to return
 @return        Handle of the parent document of this element.

 */
/*****************************************************************************/

wcDOC wcElGetDocument(wcEL element)
{
	if (!element)
		return (0);
	return (HDOC_HANDLE) (HELEMENT_OBJ(element)->GetDocument());
}

/*****************************************************************************/
// wcElSetSrc
/**
 @memo          Set the source url for an image or frame element
 @doc           Sets the src attribute for the specified element.  Only affects
                HTML_FRAME_ELEMENT and HTML_IMAGE_ELEMENT.  Doesn't load or redraw
                (unless 'refresh' is set to WEBC_TRUE) until \Ref{webc_ElementRefresh} is called.
 @param element The element to set
 @param src     The new src
 @param refresh If wcTRUE, then load the new url and redraw this element
 @see           wcElGetSrc, wcElRefresh
 */
/*****************************************************************************/

int wcElSetSrc(wcEL element, char *_src, wcBOOL refresh)
{
	if (!element)
		return -1;
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR src[HTML_TEMP_STR_LEN];
	webc_c_strncpy(src, _src, HTML_TEMP_STR_LEN-1);
	src[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *src = _src;
  #endif

  	HELEMENT_OBJ(element)->SetSrc(src);

	if (refresh)
	{
		HELEMENT_OBJ(element)->Update(0,0);
		WEBC_BROWSER_LOAD_ALL(HELEMENT_OBJ(element)->GetBrowser());
		HELEMENT_OBJ(element)->Refresh();
	}
	return 0;
}

#if(1)

/*****************************************************************************/
// wcElSetInnerHtml
/**

 @memo Set an HTML element's inner html.
 @doc Set an HTML element's inner html. Parses the provided Html source code and insert it into the the specified container element.
 The existing contents are freed and replaced with the elements created from the html text representation.

 <pre>
 Examples:
     wcElSetInnerHtml(Elw, "&lt;table border=\"1\"&gt;\&lt;tr&gt;&lt;td&gt;(1,1)&lt;/td&gt;&lt;td&gt;(1,2)&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;");
 </pre>

 @param  wcEL 		Elw    		-   The containing element.
 @param  char 		*NewHtml    -   Html source code.

 @return

 @see wcElGetInnerHtml
 */
/*****************************************************************************/

/*****************************************************************************/
void wcElSetInnerHtml(wcEL El,	char * newhtml)
{
  #if (WEBC_SUPPORT_UNICODE)
   	WEBC_CHAR *pustring;
	pustring = (WEBC_CHAR *) WEBC_MALLOC((rtp_strlen(newhtml)+1)*2);
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

/*****************************************************************************/
// wcElGetInnerHtml
/**

 @memo Retrieve html source from a container element.
 @doc Retrieve an HTML element's inner html.

 <pre>
 Examples:
     wcElSetInnerHtml(Elw, wcElGetInnerHtml(Ctx, OtherElw));
 </pre>

 @param  wcEL 		Elw    		-   The containing element.

 @return The ASCII HTML source code.

 @see wcElSetInnerHtml
 */
/*****************************************************************************/

char * wcElGetInnerHtml (wcCtx* Ctx, wcEL El)
{
	char *retVal = 0;
#if (WEBC_SUPPORT_INNER_HTML)
	 WebString tempString;
	((HTMLElement*)(El))->AppendInnerHTML(&tempString);
	//retVal = (char *)WEBC_MALLOC((tempString.getLength() + 1) * sizeof(char)); // caller must free
    return wcCtxtAppendGarbage(Ctx, webc_MallocASCIIString(tempString.getChars()));
#endif // WEBC_SUPPORT_INNER_HTML

	return retVal;
}
/*****************************************************************************/
// wcElGetOuterHtml
/**

 @memo Retrieve outer html source from a container element.
 @doc Retrieve an HTML element's inner html.

 <pre>

 </pre>

 @param  wcEL 		Elw    		-   The containing element.

 @return The ASCII HTML source code.

 @see wcElGetInnerHtml
 */
/*****************************************************************************/

char * wcElGetOuterHtml (wcCtx* Ctx, wcEL El)
{
	char *retVal = 0;
#if (WEBC_SUPPORT_INNER_HTML)
	 WebString tempString;
	((HTMLElement*)(El))->AppendOuterHTML(&tempString);
	//retVal = (char *)WEBC_MALLOC((tempString.getLength() + 1) * sizeof(char)); // caller must free
    return wcCtxtAppendGarbage(Ctx, webc_MallocASCIIString(tempString.getChars()));
#endif // WEBC_SUPPORT_INNER_HTML

	return retVal;
}


/*****************************************************************************/
// wcElRemoveChild
/**

 @memo Remove a child element from its parent.
 @doc Remove a child element from its parent. The child element must be contained by the parent.
 The child and parent are unlinked. The child is not deleted, it may be deleted or inserted into another
 container.

 @param  wcEL          Parent       -   The parent "container" element.
 @param  wcEL          Child        -   The child element to remove.
 @param wcBOOL 		   refresh		-   If wcTRUE redraw the element imediately.

 @return wcTRUE    if the arguments non NULL. It is the callers responsibilty to ensure Child is a direct child of parent.

 @see
 */
/*****************************************************************************/

wcBOOL wcElRemoveChild (wcEL parent,wcEL child,wcBOOL refresh)
{
    if (!parent || !child)
        return wcFALSE;
    HELEMENT_OBJ(parent)->Remove(HELEMENT_OBJ(child));
	if (refresh)
	{
    	HELEMENT_OBJ(parent)->Update(0,(WEBC_PFBYTE)1);
    	HELEMENT_OBJ(parent)->Refresh();
	}
    return wcTRUE;
}

/*****************************************************************************/
// wcElDeleteChildren
/**

 @memo Delete all child elements from a container element.
 @doc   Delete all child elements from element.

 @param  wcEL          Parent       -   The parent "container" element.
 @param wcBOOL 		   refresh		-   If wcTRUE redraw the element imediately.

 @return wcTRUE    if the argument is non NULL.

 */
/*****************************************************************************/

wcBOOL wcElDeleteChildren (wcEL element,wcBOOL refresh)
{
    if (!element)
        return wcFALSE;
    HELEMENT_OBJ(element)->DeleteChildren();
	if (refresh)
	{
    	HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
    	HELEMENT_OBJ(element)->Refresh();
	}
    return wcTRUE;
}

/*****************************************************************************/
// wcElDelete
/**

 @memo Delete an element and all of its children.
 @doc   Delete element and all of its children.

 @param  wcEL          Element      -   The element to delete.
 @param wcBOOL 		   refresh		-   If wcTRUE redraw the element imediately.

 @return wcTRUE    if the argument is non NULL.

 */
/*****************************************************************************/
wcBOOL wcElDelete(wcEL element,wcBOOL refresh)
{
    if (!element)
        return wcFALSE;
    wcElDeleteChildren (element, refresh);
    WEBC_DELETE(HELEMENT_OBJ(element));
    return wcTRUE;
}

/*****************************************************************************/
// wcElInsertFirst
/**

 @memo Insert an element into the first position of a containing element.
 @doc   Insert an element into the first position of a containing element.

 @param  wcEL          Parent        -   The containing element.
 @param  wcEL          Child        -   The element to insert.
 @param wcBOOL 		   refresh		-   If wcTRUE redraw the element imediately.

 @return wcTRUE    if the arguments are non NULL.

 */
/*****************************************************************************/
wcBOOL wcElInsertFirst(wcEL element,wcEL child,wcBOOL refresh)
{
    if (!element || !child)
        return wcFALSE;
    HELEMENT_OBJ(element)->InsertFirst(HELEMENT_OBJ(child));
	if (refresh)
	{
    	HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
    	HELEMENT_OBJ(element)->Refresh();
	}
    return wcTRUE;
}
/*****************************************************************************/
// wcElInsertLast
/**

 @memo Insert an element into the last position of a containing element.
 @doc   Insert an element into the last position of a containing element.

 @param  wcEL          Parent        -   The containing element.
 @param  wcEL          Child        -   The element to insert.
 @param wcBOOL 		   refresh		-   If wcTRUE redraw the element imediately.

 @return wcTRUE    if the arguments are non NULL.

 */
/*****************************************************************************/
wcBOOL wcElInsertLast(wcEL element,wcEL child,wcBOOL refresh)
{
    if (!element || !child)
        return wcFALSE;
    HELEMENT_OBJ(element)->InsertLast(HELEMENT_OBJ(child));
	if (refresh)
	{
    	HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
    	HELEMENT_OBJ(element)->Refresh();
	}
    return wcTRUE;
}

/*****************************************************************************/
// wcElInsertAfter
/**

 @memo Insert an element after a specific element.
 @doc   Insert an element after a specific element in a container.

 @param  wcEL          Parent           -   The containing element.
 @param  wcEL          CurChild        	-   The element to insert after.
 @param  wcEL          NewChild        	-   The element to insert.
 @param wcBOOL 		   refresh			-   If wcTRUE redraw the element imediately.

 @return wcTRUE    if the arguments are non NULL.

 */
/*****************************************************************************/

wcBOOL wcElInsertAfter(wcEL Parent,wcEL CurChild, wcEL NewChild,wcBOOL refresh)
{
    if (!Parent || !NewChild || !CurChild)
        return wcFALSE;
    HELEMENT_OBJ(Parent)->InsertAfter(HELEMENT_OBJ(NewChild),HELEMENT_OBJ(CurChild));
	if (refresh)
	{
    	HELEMENT_OBJ(Parent)->Update(0,(WEBC_PFBYTE)1);
    	HELEMENT_OBJ(Parent)->Refresh();
	}
    return wcTRUE;
}

/*****************************************************************************/
// wcElInsertBefore
/**

 @memo Insert an element before a specific element.
 @doc   Insert an element before a specific element in a container.

 @param  wcEL          Parent       -   The containing element.
 @param  wcEL          CurChild     -   The element to insert before.
 @param  wcEL          NewChild     -   The element to insert.
 @param wcBOOL 		   refresh		-   If wcTRUE redraw the element imediately.

 @return wcTRUE    if the arguments are non NULL.

 */
/*****************************************************************************/
wcBOOL wcElInsertBefore(wcEL Parent,wcEL CurChild,wcEL NewChild,wcBOOL refresh)
{
    if (!Parent || !NewChild || !CurChild)
        return wcFALSE;
    HELEMENT_OBJ(Parent)->InsertBefore(HELEMENT_OBJ(NewChild),HELEMENT_OBJ(CurChild));
	if (refresh)
	{
    	HELEMENT_OBJ(Parent)->Update(0,(WEBC_PFBYTE)1);
    	HELEMENT_OBJ(Parent)->Refresh();
	}
    return wcTRUE;
}


/*****************************************************************************/
// wcElIdMatches
/**

 @memo  Compare an element's id to a string.
 @doc   Compare an element's id to a string, return true if they match.


 @param  wcEL          El        -   The element to compare.
 @param  char*         IdStr   -   The string to match.

 @return wcTRUE    if the element has an assigned id and they match wcFALSE otherwise.

 @see
 */
/*****************************************************************************/


wcBOOL wcElIdMatches(wcEL element, char *string)
{
    if (HELEMENT_OBJ(element)->Id() && string && !webc_c_stricmp(HELEMENT_OBJ(element)->Id(), string))
        return wcTRUE;
    return wcFALSE;
}



HTMLElement *jhutil_CloneElement(HTMLElement *clone, WEBC_BOOL bCloneChildren);


/*****************************************************************************/
// wcElGetParent
/**

 @memo  Get the parent of an element.
 @doc Get the parent of an element.

 @param  wcEL 	El			    -   The element.

 @return The parent element or zero if the element is the root or it is not in the DOM.

 @see
 */
/*****************************************************************************/

wcEL wcElGetParent(wcEL element)
{
	if (!element)
		return (0);
	return (HELEMENT_HANDLE) (HELEMENT_OBJ(element)->mpParent);
}

/*****************************************************************************/
// wcElGetChild
/**

 @memo  Get the first child of an element.
 @doc Get the first child of an element.

 @param  wcEL 	El			    -   The element.

 @return The first child of an element or zero if the element has no children.

 @see
 */
/*****************************************************************************/

wcEL wcElGetChild(wcEL element)
{
	return (wcEL) wcElFindChild((HELEMENT_HANDLE) element, 0, 0, HTML_ELEMENT_ANY, 0);
}

/*****************************************************************************/
// wcElGetPrev
/**

 @memo  Get the previous sibling of an element.
 @doc Get the previous sibling of an element.

 @param  wcEL 	El			    -   The element.

 @return The previous sibling of an element or zero if the element has no previous sibling.

 @see
 */
/*****************************************************************************/

wcEL wcElGetPrev(wcEL element)
{
	return (wcEL) wcElFindPrev((HELEMENT_HANDLE) element, 0, 0, HTML_ELEMENT_ANY, 0);
}

/*****************************************************************************/
// wcElGetNext
/**

 @memo  Get the next sibling of an element.
 @doc Get the next sibling of an element.

 @param  wcEL 	El			    -   The element.

 @return The next sibling of an element or zero if the element has no next sibling.

 @see
 */
/*****************************************************************************/

wcEL wcElGetNext(wcEL element)
{
	return (wcEL) wcElFindNext((HELEMENT_HANDLE) element, 0, 0, HTML_ELEMENT_ANY, 0);

}


/*****************************************************************************/
// wcElFindNext
/**
 @memo          Find next element in document
 @doc           Searches the document this element is a part of, starting
                at the given element, for the next element that matches
                all the given criteria

 @param element The element to search from
 @param id      The \Ref{ID} of the element to search for (WEBC_NULL to match any id)
 @param name	The \Ref{NAME} of the element to search for (WEBC_NULL to match any name)
 @param type    The \Ref{TYPE} of element to search for (HTML_ELEMENT_ANY to match any type)
 @param index   The offset from the given element of the element to find (0 to match the
                first element after this one matching all the other criteria)
 @return        The handle of the matching element or WEBC_NULL if not found
 @see           wcElGetNext
 */
/*****************************************************************************/

wcEL wcElFindNext(wcEL element, char *id, char *name, HTMLElementType type, long index)
{
	if (!element)
		return (0);
HTMLNextByIdNameTypeIndexFinder f(HELEMENT_OBJ(element), id, name, type, index);
HTMLDocument *pDoc = HELEMENT_OBJ(element)->GetDocument();

	if (pDoc)
	{
		return (wcEL) (pDoc->FindElement(&f));
	}

	return (0);
}


/*****************************************************************************/
// wcElFindPrev
/**
 @memo          Find prev element in document
 @doc           Searches the document this element is a part of, starting
                at the given element, for the previous element that matches
                all the given criteria
 @param          element The element to search from
 @param id      The \Ref{ID} of the element to search for (WEBC_NULL to match any id)
 @param name	The \Ref{NAME} of the element to search for (WEBC_NULL to match any name)
 @param type    The \Ref{TYPE} of element to search for (HTML_ELEMENT_ANY to match any type)
 @param index   The offset from the given element of the element to find (0 to match the
                last element before this one matching all the other criteria)
 @return        The handle of the matching element or WEBC_NULL if not found
 @see           wcGetPrev
 */
/*****************************************************************************/

wcEL wcElFindPrev(wcEL element, char *id, char *name, HTMLElementType type, long index)
{
	if (!element)
		return (0);
HTMLPrevByIdNameTypeIndexFinder f(HELEMENT_OBJ(element), id, name, type, index);
HTMLDocument *pDoc = HELEMENT_OBJ(element)->GetDocument();

	if (pDoc)
	{
		pDoc->FindElement(&f);
		return (wcEL) (f.Found());
	}

	return (0);
}


/*****************************************************************************/
// wcElFindChild
/**
 @memo          Search for a child of an element by id, name, type, and/or index
 @doc           Searches the document subtree at the given element for a child
                element that matches the given search criteria.
				 @param element The parent element whose children to search
 @param id      The \Ref{ID} of the element to search for (WEBC_NULL to match any id)
 @param name	The \Ref{NAME} of the element to search for (WEBC_NULL to match any name)
 @param type    The \Ref{TYPE} of element to search for (HTML_ELEMENT_ANY to match any type)
 @param index   The index of the element to find (HTML_INDEX_LAST for the last
                element in the document)
 @return        The handle of the matching element or WEBC_NULL if not found
 @see           wcGetChild
 */
/*****************************************************************************/

wcEL wcElFindChild (wcEL element, char *id, char *name, HTMLElementType type, long index)
{
	if (!element)
		return (0);
HTMLIdNameTypeIndexFinder8 f(id, name, type, index);
	if (!element)
		return (0);
	return (wcEL) (HELEMENT_OBJ(element)->FindElement(&f));
}


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
// wcElRefresh
/**
 @memo Force a redraw of an element.
 @doc   Force a redraw of an element..

 @param wcEL element 	- The element to redraw.
  Pass wcTRUE to redraw the element after changing the style.

 @return Nothing.
 @see
 */
/*****************************************************************************/

void wcElRefresh(wcEL element)
{
	if (element)
	{
		HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
		HELEMENT_OBJ(element)->Refresh();
	}
}

/*****************************************************************************/
// wcElSetStyle
/**
 @memo Set element style attributes.
 @doc  Set style value(s) for an element and redraw the element.

 @param wcEL element 	- The element to set style info for.
 @param char* style   	- Cascading style sheet source data (e.g. "top:20px;left:10px;")
 @param wcBOOL doUpdate	- Pass wcTRUE to redraw the element after changing the style.

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
		HELEMENT_OBJ(element)->StyleUpdate();
		if (doUpdate)
		{
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
// wcElSetName
/**
 @memo          Set an element's name
 @doc           Sets the name attribute for the specified element
 @param element The element to set
 @param name    The new name

 @see           HELEMENT_HANDLE, NAME, wcElGetName, wcElRefresh
 */
/*****************************************************************************/

int wcElSetName(wcEL element, char *_name)
{
	if (!element)
		return -1;

  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR name[HTML_TEMP_STR_LEN];
	webc_c_strncpy(name, _name, HTML_TEMP_STR_LEN-1);
	name[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *name = _name;
  #endif

	HELEMENT_OBJ(element)->SetName(name);
	return 0;
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
 @param wcBOOL refresh	- If wcTRUE redraw the element imediately.

 @return        The wcTRUE if the element is radio button element or a checkbox element.
 @see           wcElGetChecked, wcElSetValue
 */
/*****************************************************************************/

wcBOOL wcElSetChecked(wcEL element,wcBOOL checked,wcBOOL refresh)
{
HTMLElementType theType;
	if (!element)
		return (wcFALSE);
	theType=wcElGetType(element);
	if (theType==HTML_CHECKBOX_ELEMENT || theType==HTML_RADIO_BUTTON_ELEMENT)
	{
		HELEMENT_OBJ(element)->SetChecked(checked);
		if (refresh)
		{
			HELEMENT_OBJ(element)->Update(0,0);
			HELEMENT_OBJ(element)->Refresh();
		}
		return (wcTRUE);
	}
	else
		return (wcFALSE);

}

/*****************************************************************************/
// wcElSetText
/**
 @memo Assign a text value to an element.
 @doc           Set the text propery of an option element.

 @param wcEL element 	- The element to set text for.
 @param char* Val   	- New Value for the element.
 @param wcBOOL refresh	- If wcTRUE redraw the element imediately.

 @return wcTRUE    if the arguments are non NULL.
 @see
 */
/*****************************************************************************/

wcBOOL wcElSetText(wcEL element, char *_Val,wcBOOL refresh)
{
HTMLElementType theType;
	if (!element)
		return wcFALSE;

	theType=wcElGetType(element);
	if (theType==HTML_OPTION_ELEMENT)
	{
#if (WEBC_SUPPORT_UNICODE)
		WEBC_CHAR Val[HTML_TEMP_STR_LEN];
		webc_c_strncpy(Val, _Val, HTML_TEMP_STR_LEN-1);
		Val[HTML_TEMP_STR_LEN-1] = 0;
#else
  		WEBC_CHAR *Val = _Val;
#endif
		((HTMLOption *)element)->SetText(Val);
		if (refresh)
		{
			HELEMENT_OBJ(element)->Update(0,0);
			HELEMENT_OBJ(element)->Refresh();
		}
		return (wcTRUE);
	}
	else
		return (wcFALSE);




	return wcTRUE;
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
// wcElSetBgImage
/**
 @memo          Set an element's background image url
 @doc           Sets the background image url for the specified element.  Only affects
                HTML_BODY_ELEMENT, HTML_TABLE_ELEMENT, HTML_TABLE_CELL_ELEMENT,
                and HTML_TABLE_ROW_ELEMENT.  Doesn't load the new bitmap or redraw
                (unless 'refresh' is set to WEBC_TRUE) until \Ref{webc_ElementRefresh} is called.
 @param element The element to set
 @param bgimg   The new background image url
 @param refresh If wcTRUE, then load the new url and redraw this element

 */
/*****************************************************************************/

void wcElSetBgImage(wcEL element, char *_bgimg, wcBOOL refresh)
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR bgimg[HTML_TEMP_STR_LEN];
	webc_c_strncpy(bgimg, _bgimg, HTML_TEMP_STR_LEN-1);
	bgimg[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *bgimg = _bgimg;
  #endif

	HELEMENT_OBJ(element)->SetProperty(HTML_ATTRIB_BACKGROUND, bgimg);
	if (refresh)
	{
		HELEMENT_OBJ(element)->Update(0,0);
		WEBC_BROWSER_LOAD_ALL(HELEMENT_OBJ(element)->GetBrowser());
		HELEMENT_OBJ(element)->Refresh();
	}
}


/*****************************************************************************/
// wcElEnable
/**
 @memo          Set an element to be selectable
 @doc           Will shade in an input widget and make it modifiable.

 @param element The element to set
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, VISIBILITY, webc_ElementSetHidden
 */
/*****************************************************************************/

int wcElEnable(wcEL element, wcBOOL refresh)
{
	if (!element)
		return -1;
	HELEMENT_OBJ(element)->SetDisabled(WEBC_FALSE);
	if (refresh)
	{
		HELEMENT_OBJ(element)->Update(0,0);
		HELEMENT_OBJ(element)->Refresh();
	}
	return 0;
}


/*****************************************************************************/
// wcElDisable
/**
 @memo          Set an element to not be selectable
 @doc           Will shade out an input widget and make it non-modifiable.

 @param element The element to set
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, VISIBILITY, webc_ElementSetHidden
 */
/*****************************************************************************/

int wcElDisable(wcEL element, wcBOOL refresh)
{
	if (!element)
		return -1;
	HELEMENT_OBJ(element)->SetDisabled(WEBC_TRUE);
	HELEMENT_OBJ(element)->Update(0,0);
	if (refresh)
	{
		HELEMENT_OBJ(element)->Refresh();
	}
	return 0;
}



/*****************************************************************************/
// wcElSetSelected
/**
 @memo           Set the selected option in a combo box
 @doc            Sets the index (beginning with 0) of the selected item in a combo
                 box.  The value (string) of the selected item of a combo box can
                 be retrieved via webc_ElementGetValue.
 @param element  The combo box element to set.
 @param selected Index of the option to select.
 @param refresh  If WEBC_TRUE, then redraw this element
 @return 		 0, -1 on failure.
 @see           wcElGetSelected
 */
/*****************************************************************************/

int wcElSetSelected(HELEMENT_HANDLE element, int selected, wcBOOL refresh)
{
	if (element && HELEMENT_OBJ(element)->Type() == HTML_SELECT_ELEMENT)
	{
		HTMLSelect *pSelect = (HTMLSelect *) HELEMENT_OBJ(element);

		pSelect->SetSelectedIndex(selected);
		if (refresh)
		{
			pSelect->Refresh();
		}
	return 0;
	}
	return -1;
}

/*****************************************************************************/
// wcElGetSelected
/**
 @memo           Get the selected option in a combo box
 @doc            Gets the index (beginning with 0) of the selected item in a combo
                 box.  The value (string) of the selected item of a combo box can
                 be retrieved via wcElGetValue.
 @param element  The combo box element to check.
 @return 		 The selected index (0 - N), -1 on failure.
 @see           wcElGetSelected
 */
/*****************************************************************************/

int wcGetSelected(HELEMENT_HANDLE element)
{
	if (element && HELEMENT_OBJ(element)->Type() == HTML_SELECT_ELEMENT)
	{
		HTMLSelect *pSelect = (HTMLSelect *) HELEMENT_OBJ(element);

		return pSelect->SelectedIndex();
	}
	return -1;
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
// wcElGetIdMatch
/**

 @memo   Test if an element's Id attribute matches an ascii string.
 @doc    Returns wcTRUE if the Id matches the string. Helpful when executing wcDocEnumerate to find elements with specific Id's.

 @param  wcEL          Element       -   The element.
 @param  wcEL          char *        -   The string.
 @return char *Id     wcTRUE if they match.

 */
/*****************************************************************************/

wcBOOL wcElGetIdMatch(wcEL element, char *string)
{
	if (HELEMENT_OBJ(element)->Id() && string && !webc_c_stricmp(HELEMENT_OBJ(element)->Id(), string))
	{
		return wcTRUE;
	}

	return wcFALSE;
}

/*****************************************************************************/
// wcELGetName
/**

 @memo   Get the name attribute.
 @doc           Returns the name string of this element.

 @param  wcEL          Element       -   The element.

 @return char *Name     The id if the element has one, or zero. (note: You must call wcCtxtRelease, to release the storage).

 */
/*****************************************************************************/

char *wcElGetName(wcCtx* Ctx,wcEL element)
{
    if (element && HELEMENT_OBJ(element)->Name())
	    return wcCtxtAppendGarbage(Ctx, webc_MallocASCIIString(HELEMENT_OBJ(element)->Name()));
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

// Version for chrome browser
char *webc_ElementGetValueASCII(wcEL element)
{
    if (element && HELEMENT_OBJ(element)->Value())
	    return webc_MallocASCIIString(HELEMENT_OBJ(element)->Value());
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


static char *html_element_names[] = {
	"HTML_ADDRESS_ELEMENT", /* HTML_ADDRESS_ELEMENT */
	"HTML_ANCHOR_ELEMENT", /* HTML_ANCHOR_ELEMENT */
	"HTML_AREA_ELEMENT", /* HTML_AREA_ELEMENT */
	"HTML_BASE_ELEMENT", /* HTML_BASE_ELEMENT */
	"HTML_BIG_ELEMENT", /* HTML_BIG_ELEMENT */
	"HTML_BLOCKQUOTE_ELEMENT", /* HTML_BLOCKQUOTE_ELEMENT */
	"HTML_BODY_ELEMENT", /* HTML_BODY_ELEMENT */
	"HTML_BOLD_ELEMENT", /* HTML_BOLD_ELEMENT */
	"HTML_BR_ELEMENT", /* HTML_BR_ELEMENT */
	"HTML_BUTTON_ELEMENT", /* HTML_BUTTON_ELEMENT */
	"HTML_CENTER_ELEMENT", /* HTML_CENTER_ELEMENT */
	"HTML_CHECKBOX_ELEMENT", /* HTML_CHECKBOX_ELEMENT */
	"HTML_CITE_ELEMENT", /* HTML_CITE_ELEMENT */
	"HTML_CODE_ELEMENT", /* HTML_CODE_ELEMENT */
	"HTML_DEFINITION_ELEMENT", /* HTML_DEFINITION_ELEMENT */
	"HTML_DEFINITION_LIST_ELEMENT", /* HTML_DEFINITION_LIST_ELEMENT */
	"HTML_DEFINITION_TERM_ELEMENT", /* HTML_DEFINITION_TERM_ELEMENT */
	"HTML_DEL_ELEMENT", /* HTML_DEL_ELEMENT */
	"HTML_DIV_ELEMENT", /* HTML_DIV_ELEMENT */
	"HTML_EDIT_STR_ELEMENT", /* HTML_EDIT_STR_ELEMENT */
	"HTML_EDITBOX_ELEMENT", /* HTML_EDITBOX_ELEMENT */
	"HTML_ELEMENT", /* HTML_ELEMENT */
	"HTML_EMPHASIZE_ELEMENT", /* HTML_EMPHASIZE_ELEMENT */
	"HTML_FLOW_ELEMENT", /* HTML_FLOW_ELEMENT */
	"HTML_FONT_ELEMENT", /* HTML_FONT_ELEMENT */
	"HTML_FORM_ELEMENT", /* HTML_FORM_ELEMENT */
	"HTML_FRAME_ELEMENT", /* HTML_FRAME_ELEMENT */
	"HTML_FRAMESET_ELEMENT", /* HTML_FRAMESET_ELEMENT */
	"HTML_HEADING_ELEMENT", /* HTML_HEADING_ELEMENT */
	"HTML_HIDDEN_INPUT_ELEMENT", /* HTML_HIDDEN_INPUT_ELEMENT */
	"HTML_HORIZONTAL_RULE_ELEMENT", /* HTML_HORIZONTAL_RULE_ELEMENT */
	"HTML_IFRAME_ELEMENT", /* HTML_IFRAME_ELEMENT */
	"HTML_IMAGE_ELEMENT", /* HTML_IMAGE_ELEMENT */
	"HTML_INPUT_BUTTON_ELEMENT", /* HTML_INPUT_BUTTON_ELEMENT */
	"HTML_INPUT_PASSWORD_ELEMENT", /* HTML_INPUT_PASSWORD_ELEMENT */
	"HTML_INPUT_RESET_ELEMENT", /* HTML_INPUT_RESET_ELEMENT */
	"HTML_INS_ELEMENT", /* HTML_INS_ELEMENT */
	"HTML_ITALIC_ELEMENT", /* HTML_ITALIC_ELEMENT */
	"HTML_KBD_ELEMENT", /* HTML_KBD_ELEMENT */
	"HTML_LABEL_ELEMENT", /* HTML_LABEL_ELEMENT */
	"HTML_LIST_ELEMENT", /* HTML_LIST_ELEMENT */
	"HTML_LIST_ITEM_ELEMENT", /* HTML_LIST_ITEM_ELEMENT */
	"HTML_MAP_ELEMENT", /* HTML_MAP_ELEMENT */
	"HTML_NOBR_ELEMENT", /* HTML_NOBR_ELEMENT */
	"HTML_OPTION_ELEMENT", /* HTML_OPTION_ELEMENT */
	"HTML_PARAGRAPH_ELEMENT", /* HTML_PARAGRAPH_ELEMENT */
	"HTML_PRE_ELEMENT", /* HTML_PRE_ELEMENT */
	"HTML_RADIO_BUTTON_ELEMENT", /* HTML_RADIO_BUTTON_ELEMENT */
	"HTML_SCRIPT_ELEMENT", /* HTML_SCRIPT_ELEMENT */
	"HTML_SELECT_ELEMENT", /* HTML_SELECT_ELEMENT */
	"HTML_SMALL_ELEMENT", /* HTML_SMALL_ELEMENT */
	"HTML_SPACER_ELEMENT", /* HTML_SPACER_ELEMENT */
	"HTML_SPAN_ELEMENT", /* HTML_SPAN_ELEMENT */
	"HTML_STRIKE_ELEMENT", /* HTML_STRIKE_ELEMENT */
	"HTML_STRING_ELEMENT", /* HTML_STRING_ELEMENT */
	"HTML_STRONG_ELEMENT", /* HTML_STRONG_ELEMENT */
	"HTML_STYLE_ELEMENT", /* HTML_STYLE_ELEMENT */
	"HTML_TABLE_ELEMENT", /* HTML_TABLE_ELEMENT */
	"HTML_TABLE_CELL_ELEMENT", /* HTML_TABLE_CELL_ELEMENT */
	"HTML_TABLE_ROW_ELEMENT", /* HTML_TABLE_ROW_ELEMENT */
	"HTML_UNDERLINE_ELEMENT", /* HTML_UNDERLINE_ELEMENT */
};
/*****************************************************************************/
// wcGetTypeString
/**
 @memo          (not implemented) Return a pointer to an element type's string representation.
 @doc           Returns an ascii string containing the element's type or 0 if no match.<br>
	Example: const char *Type =  wcElGetType(element) ?  wcGetTypeString(wcElGetType(wcEL element)) : "Unknown type";
 @param TypeVal Type to retrieve string representation.
 @return        An ascii string containing the element's type or 0 if no match.
 @see           wcElGetType, HTMLElementType
 */
/*****************************************************************************/

const char* wcGetTypeString(HTMLElementType  TypeVal)
{
	if (TypeVal <= (sizeof(html_element_names)/sizeof(html_element_names[0])))
		return html_element_names[TypeVal];
	else
		return 0;
}


/*****************************************************************************/
// wcElGetSrc
/**
 @memo          Get the source url for an image or frame element
 @doc           This function only returns meaningful values for frame and
                image elements.
 @param element The element whose src to get
 @return        The element's src or if it has none.
 @see           SRC, wcElSetSrc,
 */
/*****************************************************************************/

char* wcElGetSrc(wcCtx* Ctx, wcEL element)
{

    if (element && HELEMENT_OBJ(element)->Src())
	    return wcCtxtAppendGarbage(Ctx,webc_MallocASCIIString(HELEMENT_OBJ(element)->Src()));
	else
	    return(WEBC_NULL);
}

// Version for chrome browser
char * webc_ElementGetSrcASCII(wcEL element)
{

    if (element && HELEMENT_OBJ(element)->Src())
	    return webc_MallocASCIIString(HELEMENT_OBJ(element)->Src());
	else
	    return(WEBC_NULL);
}

/*****************************************************************************/
// wcElGetColorValue
/**
 @memo          Get a property of type color from Css or html.
 @doc           Get a property of type color from Css. First checks Css, if not found consults html for default values.
	Note: The property must ve one that is expressed in color otherwise the result will be meaningless. <br>
	<pre>
		These are the most common properties that applications code might query.

		CSS_PROPERTY_COLOR
		CSS_PROPERTY_BACKGROUND_COLOR

		These properties may also be queried.

		CSS_PROPERTY_ALINK_COLOR
		CSS_PROPERTY_VLINK_COLOR
		CSS_PROPERTY_LINK_COLOR
		CSS_PROPERTY_OUTLINE_COLOR

		CSS_PROPERTY_BORDER_BOTTOM_COLOR
		CSS_PROPERTY_BORDER_COLOR
		CSS_PROPERTY_BORDER_LEFT_COLOR
		CSS_PROPERTY_BORDER_RIGHT_COLOR
		CSS_PROPERTY_BORDER_TOP_COLOR

	</pre>
 @param element The element whose color value to get
 @param Prop    Which property to retrieve
  @param color   Pointer to an HTMLColor (unsigned long) variable to fill in with the
                color information for this element.
 @return        0 if the property was found and color was filled with a valid color,
                -1 otherwise
 @see           wcElGetLengthValue
 */
/*****************************************************************************/

int wcElGetColorValue(wcEL element, CSSPropertyType Prop, HTMLColor* color)
{
CSSPropertyValue value;

	if (!element)
		return (-1);
	if ((HELEMENT_OBJ(element)->GetStyleFromCSS(Prop, &value) >= 0)||
		(HELEMENT_OBJ(element)->GetStyleFromHTML(Prop, &value) >= 0))
	{
		*color = value.color;
		return (0);
	}

	return (-1);
}

/*****************************************************************************/
// wcElGetLengthValue
/**
 @memo          Get a property of type HTMLLength from Css or html.
 @doc           Get a property of type HTMLLength from Css or html. First checks Css, if not found consults html for default values.
	Note: The property must ve one that is expressed in color otherwise the result will be meaningless. <br>
	<pre>

	These are the most common properties that applications code might query (they can also be retrieved from wcElGetPosStyle)..

	CSS_PROPERTY_LEFT
	CSS_PROPERTY_TOP
	CSS_PROPERTY_WIDTH
	CSS_PROPERTY_HEIGHT

	These properties may also be queried.

	CSS_PROPERTY_BACKGROUND_X_POSITION,	CSS_PROPERTY_BACKGROUND_Y_POSITION,	CSS_PROPERTY_BORDER_BOTTOM_WIDTH,	CSS_PROPERTY_BORDER_LEFT_WIDTH,	CSS_PROPERTY_BORDER_RIGHT_WIDTH,
	CSS_PROPERTY_BORDER_TOP_WIDTH,	CSS_PROPERTY_BORDER_WIDTH,	CSS_PROPERTY_BOTTOM,	CSS_PROPERTY_LETTER_SPACING, CSS_PROPERTY_LINE_HEIGHT, CSS_PROPERTY_MARGIN,	CSS_PROPERTY_MARGIN_BOTTOM,
	CSS_PROPERTY_MARGIN_LEFT,	CSS_PROPERTY_MARGIN_RIGHT,	CSS_PROPERTY_MARGIN_TOP, CSS_PROPERTY_OUTLINE_WIDTH, CSS_PROPERTY_PADDING_BOTTOM, CSS_PROPERTY_PADDING_LEFT,
	CSS_PROPERTY_PADDING_RIGHT,	CSS_PROPERTY_PADDING_TOP, CSS_PROPERTY_POSITION, CSS_PROPERTY_POSITION_X, CSS_PROPERTY_POSITION_Y,
	</pre>
 @param element The element whose length value to get
 @param Prop    Which property to retrieve
  @param Length   Pointer to an HTMLLength (unsigned long) variable to fill in with the
                Length information for this element.
 @return        0 if the property was found and Lengh was filled with a valid color,
                -1 otherwise
 @see           wcElGetColorValue,wcElGetPosStyle
 */
/*****************************************************************************/

int  wcElGetLengthValue(HELEMENT_HANDLE element,  CSSPropertyType Prop, HTMLLength *Length)
{
CSSPropertyValue value;

	if (!element)
		return -1;
	if ((HELEMENT_OBJ(element)->GetStyleFromCSS(Prop, &value) >= 0) ||
		(HELEMENT_OBJ(element)->GetStyleFromHTML(Prop, &value) >= 0))
	{
		*Length = value.length.val.dec;
		return 0;
	}
	return  -1;
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

wcBOOL wcElGetPosStyle(wcEL element, int *top, int *left, int *height, int *width)
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
// wcElSetStylePos
/**
 @memo  Set the absolute top left corner of an element using absolute  positioning.
  @doc  Formats a string and passes it to wcElSetStyle() to set the top, left position of an element using CSS.

 @param  wcEL          		Element     -   The element.
 @param  int          		top      	-   top goes here.
 @param  int          		left      	-   left goes here.
 @param  refresh      		Redraw the element immediately if wcTRUE.

 @return        nothing.

 @see wcElSetStyleDim,wcElSetStyleRgb,,wcElSetStyleHtmlColor
 */
/*****************************************************************************/

void wcElSetStylePos(wcEL element, int top, int left,wcBOOL refresh)
{
char StyleBuffer[80];
	rtp_sprintf(StyleBuffer, "position:absolute;top:%dpx;left:%dpx;",top,left);
	wcElSetStyle(element,StyleBuffer, refresh);
}

/*****************************************************************************/
// wcElSetStyleDim
/**
 @memo  Set the height and width of an element using CSS.
 @doc  Formats a string and passes it to wcElSetStyle() to set the height and width of an element using CSS.

 @param  wcEL          		Element     -   The element.
 @param  int          		height      -	New height.
 @param  int          		width		-	New width.
 @param  refresh      		Redraw the element immediately if wcTRUE.

 @return        nothing.

 @see wcElSetStylePos,wcElSetStyleRgb,,wcElSetStyleHtmlColor
 */
/*****************************************************************************/
void wcElSetStyleDim(wcEL element, int height, int width, wcBOOL refresh)
{
char StyleBuffer[80];
	rtp_sprintf(StyleBuffer, "height:%dpx;width:%dpx;", height, width);
	wcElSetStyle(element,StyleBuffer, refresh);
}

/*****************************************************************************/
// wcElSetStyleHeight
/**
 @memo  Set the height of an element using CSS.
 @doc  Formats a string and passes it to wcElSetStyle() to set the height using CSS.

 @param  wcEL          		Element     -   The element.
 @param  int          		height      -	New height.
 @param  int          		width		-	New width.
 @param  refresh      		Redraw the element immediately if wcTRUE.

 @return        nothing.

 @see wcElSetStylePos,wcElSetStyleRgb,,wcElSetStyleHtmlColor
 */
/*****************************************************************************/
void wcElSetStyleHeight(wcEL element, int height, wcBOOL refresh)
{
char StyleBuffer[80];
	rtp_sprintf(StyleBuffer, "height:%dpx;", height);
	wcElSetStyle(element,StyleBuffer, refresh);
}


/*****************************************************************************/
// wcElSetStyleWidth
/**
 @memo  Set the width of an element using CSS.
 @doc  Formats a string and passes it to wcElSetStyle() to set the width using CSS.

 @param  wcEL          		Element     -   The element.
 @param  int          		width		-	New width.
 @param  refresh      		Redraw the element immediately if wcTRUE.

 @return        nothing.

 @see wcElSetStylePos,wcElSetStyleRgb,,wcElSetStyleHtmlColor
 */
/*****************************************************************************/
void wcElSetStyleWidth(wcEL element, int width, wcBOOL refresh)
{
char StyleBuffer[80];
	rtp_sprintf(StyleBuffer, "width:%dpx;", width);
	wcElSetStyle(element,StyleBuffer, refresh);
}


/*****************************************************************************/
// wcElSetStyleZindex
/**
 @memo  Set the zindex of an element using CSS.
 @doc  Formats a string and passes it to wcElSetStyle() to set the z index of an element using CSS.

 @param  wcEL          		Element     -   The element.
 @param  int          		zindex      -	New zindex value.
  @param  refresh      		Redraw the element immediately if wcTRUE.

 @return        nothing.

 @see wcElSetStylePos,wcElSetStyleRgb,,wcElSetStyleHtmlColor
 */
/*****************************************************************************/
void wcElSetStyleZindex(wcEL element, int zindex, wcBOOL refresh)
{
char StyleBuffer[80];
	rtp_sprintf(StyleBuffer, "z-index:%dpx;", zindex);
	wcElSetStyle(element,StyleBuffer, refresh);
}
extern const char * gpCSSPropertyNames[];
/*****************************************************************************/
// wcElSetStyleColorRgb
/**
 @memo  Set the color of an element using CSS.
 @doc  Formats a string and passes it to wcElSetStyle() to set the color a color of an element using CSS.

 @param  wcEL	Element     -   The element.
 @param Prop    Which color property to set.
 @param  int          		r (0-255).
 @param  int          		g (0-255).
 @param  int          		b (0-255).

 @param  refresh      		Redraw the element immediately if wcTRUE.

 	<pre>
		<br>These are the most common properties that you might set.<br>
		CSS_PROPERTY_COLOR - The text color
		CSS_PROPERTY_BACKGROUND_COLOR - The background

		<br>These properties may also be set.<br>

		CSS_PROPERTY_ALINK_COLOR,CSS_PROPERTY_VLINK_COLOR,CSS_PROPERTY_LINK_COLOR,CSS_PROPERTY_OUTLINE_COLOR,CSS_PROPERTY_BORDER_BOTTOM_COLOR,CSS_PROPERTY_BORDER_COLOR
		CSS_PROPERTY_BORDER_LEFT_COLOR,CSS_PROPERTY_BORDER_RIGHT_COLOR,CSS_PROPERTY_BORDER_TOP_COLOR


	</pre>
 @return        nothing.

 @see wcElSetStylePos,,wcElSetStyleColorHtml, wcElSetStyleColorNamed
 */
/*****************************************************************************/
void wcElSetStyleColorRgb(wcEL element,CSSPropertyType Prop, int r, int g, int b, wcBOOL refresh)
{
char StyleBuffer[80];

	rtp_sprintf(StyleBuffer, "%s:%2.2X%2.2X%2.2X;",gpCSSPropertyNames[Prop], r, g, b);
	wcElSetStyle(element,StyleBuffer, refresh);
}

/*****************************************************************************/
// wcElSetStyleColorRgba
/**
 @memo  Set the color and alpha blend value of an element using CSS.
 @doc  Formats a string and passes it to wcElSetStyle() to set the color a color of an element using CSS.

 @param  wcEL	Element     -   The element.
 @param Prop    Which color property to set.
 @param  int          		r (0-255).
 @param  int          		g (0-255).
 @param  int          		b (0-255).
 @param  int          		a (0-100) Alpha channel value. 0 is fully opaque, 100 is fully transparent
 @param  refresh      		Redraw the element immediately if wcTRUE.

 	<pre>
		<br>These are the most common properties that you might set.<br>
		CSS_PROPERTY_COLOR - The text color
		CSS_PROPERTY_BACKGROUND_COLOR - The background

		<br>These properties may also be set.<br>

		CSS_PROPERTY_ALINK_COLOR,CSS_PROPERTY_VLINK_COLOR,CSS_PROPERTY_LINK_COLOR,CSS_PROPERTY_OUTLINE_COLOR,CSS_PROPERTY_BORDER_BOTTOM_COLOR,CSS_PROPERTY_BORDER_COLOR
		CSS_PROPERTY_BORDER_LEFT_COLOR,CSS_PROPERTY_BORDER_RIGHT_COLOR,CSS_PROPERTY_BORDER_TOP_COLOR


	</pre>
 @return        nothing.

 @see wcElSetStylePos,,wcElSetStyleColorHtml, wcElSetStyleColorNamed
 */
/*****************************************************************************/
void wcElSetStyleColorRgba(wcEL element,CSSPropertyType Prop, int r, int g, int b, int a, wcBOOL refresh)
{
char StyleBuffer[80], astring[32];

	if (a == 0)
		rtp_sprintf(astring, "%d",0);
	else if (a == 100)
		rtp_sprintf(astring, "%d",1);
	else
		rtp_sprintf(astring, ".%d", a);

	rtp_sprintf(StyleBuffer, "%s:rgba(%d,%d,%d,%s);",gpCSSPropertyNames[Prop], r, g, b, astring);
	wcElSetStyle(element,StyleBuffer, refresh);
}

/*****************************************************************************/
// wcElSetStyleOpacity
/**
 @memo Set the opacity of a div element.
 @doc  Formats a string and passes it to wcElSetStyle() to set the opacity a div element using CSS.

 @param  wcEL	Element     -   The element, must be a div
 @param  int          		- opacity, must be between 0 and 100
 @param  refresh      		Redraw the element immediately if wcTRUE.


 @return        nothing.

 @see wcElSetStyle
 */
/*****************************************************************************/
void wcElSetStyleOpacity(wcEL element, int Opacity, wcBOOL refresh)
{
char StyleBuffer[80], astring[32];

	if (wcElGetType(element)!=HTML_DIV_ELEMENT)
		return;
	if (Opacity == 0)
		rtp_sprintf(astring, "%d",0);
	else if (Opacity == 100)
		rtp_sprintf(astring, "%d",1);
	else
		rtp_sprintf(astring, ".%d",Opacity);

	rtp_sprintf(StyleBuffer, "opacity:%s;", astring);
	wcElSetStyle(element,StyleBuffer, refresh);

}

/*****************************************************************************/
// wcElSetStyleColorName
/**
 @memo  Set the color of an element using CSS.
 @doc  Formats a string and passes it to wcElSetStyle() to set the a color property of an element using CSS.

 @param  wcEL	Element     -   The element.
 @param Prop    Which color property to set.
 @param  char *          	ColorName.
 @param  refresh      		Redraw the element immediately if wcTRUE.

 	<pre>
		<br>These are the most common properties that you might set.<br>
		CSS_PROPERTY_COLOR - The text color
		CSS_PROPERTY_BACKGROUND_COLOR - The background

		<br>These properties may also be set.<br>

		CSS_PROPERTY_ALINK_COLOR,CSS_PROPERTY_VLINK_COLOR,CSS_PROPERTY_LINK_COLOR,CSS_PROPERTY_OUTLINE_COLOR,CSS_PROPERTY_BORDER_BOTTOM_COLOR,CSS_PROPERTY_BORDER_COLOR
		CSS_PROPERTY_BORDER_LEFT_COLOR,CSS_PROPERTY_BORDER_RIGHT_COLOR,CSS_PROPERTY_BORDER_TOP_COLOR

		<br>These named color values are supported.<br>

	AliceBlue,AntiqueWhite,Aqua,Aquamarine,Azure,Beige,Bisque,Black,BlanchedAlmond,Blue,BlueViolet,Brown,BurlyWood,CadetBlue,Chartreuse,Chocolate,Coral,CornflowerBlue,Cornsilk,
	Crimson,Cyan,DarkBlue,DarkCyan,DarkGoldenRod,DarkGray,DarkGreen,DarkKhaki,DarkMagenta,DarkOliveGreen,Darkorange,DarkOrchid,DarkRed,DarkSalmon,DarkSeaGreen,DarkSlateBlue,
	DarkSlateGray,DarkTurquoise,DarkViolet,DeepPink,DeepSkyBlue,DimGray,DodgerBlue,Feldspar,FireBrick,FloralWhite,ForestGreen,Fuchsia,Gainsboro,GhostWhite,Gold,GoldenRod,Gray,
	Green,GreenYellow,HoneyDew,HotPink,IndianRed,Indigo,Ivory,Khaki,Lavender,LavenderBlush,LawnGreen,LemonChiffon,LightBlue,LightCoral,LightCyan,LightGoldenRodYellow,LightGrey,
	LightGreen,LightPink,LightSalmon,LightSeaGreen,LightSkyBlue,LightSlateBlue,LightSlateGray,LightSteelBlue,LightYellow,Lime,LimeGreen,Linen,Magenta,Maroon,MediumAquaMarine,
	MediumBlue,MediumOrchid,MediumPurple,MediumSeaGreen,MediumSlateBlue,MediumSpringGreen,MediumTurquoise,MediumVioletRed,MidnightBlue,MintCream,MistyRose,Moccasin,NavajoWhite,
	Navy,OldLace,Olive,OliveDrab,Orange,OrangeRed,Orchid,PaleGoldenRod,PaleGreen,PaleTurquoise,PaleVioletRed,PapayaWhip,PeachPuff,Peru,Pink,Plum,PowderBlue,Purple,Red,
	RosyBrown,RoyalBlue,SaddleBrown,Salmon,SandyBrown,SeaGreen,SeaShell,Sienna,Silver,SkyBlue,SlateBlue,SlateGray,Snow,SpringGreen,SteelBlue,Tan,Teal,Thistle,Tomato,
	Transparent,Turquoise,Violet,VioletRed,Wheat,White,WhiteSmoke,Yellow,YellowGreen<br>

	You may also use named colors with an alpha channel component. Wcapi.h contains set of macros that generate rbga() quoted strings that may be used whereever a named color value is expected.
	The macros have the same base name as the named colors. The colr is predeeded by "rgba" and the macros take an alpha channel value as an argument (a decimal value between 0 and 1).
	For example, the intrinsic HTML color "LightSkyBlue" can expressed as "rgba(135,206,250)". When the rgbaLightSkyBlue(A) macro is expanded it produces "rgba(135,206,250,A)" where A is expanded.
	So rgbaLightSkyBlue(.5)	produces the string "rgba(135,206,250,.5)" which will display sky blue with 50 per cent opacity.<br>


	rgbaAliceBlue(A),rgbaAntiqueWhite(A),rgbaAqua(A),rgbaAquamarine(A),rgbaAzure(A),rgbaBeige(A),rgbaBisque(A),rgbaBlack(A),rgbaBlanchedAlmond(A),rgbaBlue(A),rgbaBlueViolet(A),rgbaBrown(A),rgbaBurlyWood(A),rgbaCadetBlue(A),rgbaChartreuse(A),rgbaChocolate(A),rgbaCoral(A),rgbaCornflowerBlue(A),rgbaCornsilk(A),
	rgbaCrimson(A),rgbaCyan(A),rgbaDarkBlue(A),rgbaDarkCyan(A),rgbaDarkGoldenRod(A),rgbaDarkGray(A),rgbaDarkGreen(A),rgbaDarkKhaki(A),rgbaDarkMagenta(A),rgbaDarkOliveGreen(A),rgbaDarkorange(A),rgbaDarkOrchid(A),rgbaDarkRed(A),rgbaDarkSalmon(A),rgbaDarkSeaGreen(A),rgbaDarkSlateBlue(A),
	rgbaDarkSlateGray(A),rgbaDarkTurquoise(A),rgbaDarkViolet(A),rgbaDeepPink(A),rgbaDeepSkyBlue(A),rgbaDimGray(A),rgbaDodgerBlue(A),rgbaFeldspar(A),rgbaFireBrick(A),rgbaFloralWhite(A),rgbaForestGreen(A),rgbaFuchsia(A),rgbaGainsboro(A),rgbaGhostWhite(A),rgbaGold(A),rgbaGoldenRod(A),rgbaGray(A),
	rgbaGreen(A),rgbaGreenYellow(A),rgbaHoneyDew(A),rgbaHotPink(A),rgbaIndianRed(A),rgbaIndigo(A),rgbaIvory(A),rgbaKhaki(A),rgbaLavender(A),rgbaLavenderBlush(A),rgbaLawnGreen(A),rgbaLemonChiffon(A),rgbaLightBlue(A),rgbaLightCoral(A),rgbaLightCyan(A),rgbaLightGoldenRodYellow(A),rgbaLightGrey(A),
	rgbaLightGreen(A),rgbaLightPink(A),rgbaLightSalmon(A),rgbaLightSeaGreen(A),rgbaLightSkyBlue(A),rgbaLightSlateBlue(A),rgbaLightSlateGray(A),rgbaLightSteelBlue(A),rgbaLightYellow(A),rgbaLime(A),rgbaLimeGreen(A),rgbaLinen(A),rgbaMagenta(A),rgbaMaroon(A),rgbaMediumAquaMarine(A),
	rgbaMediumBlue(A),rgbaMediumOrchid(A),rgbaMediumPurple(A),rgbaMediumSeaGreen(A),rgbaMediumSlateBlue(A),rgbaMediumSpringGreen(A),rgbaMediumTurquoise(A),rgbaMediumVioletRed(A),rgbaMidnightBlue(A),rgbaMintCream(A),rgbaMistyRose(A),rgbaMoccasin(A),rgbaNavajoWhite(A),
	rgbaNavy(A),rgbaOldLace(A),rgbaOlive(A),rgbaOliveDrab(A),rgbaOrange(A),rgbaOrangeRed(A),rgbaOrchid(A),rgbaPaleGoldenRod(A),rgbaPaleGreen(A),rgbaPaleTurquoise(A),rgbaPaleVioletRed(A),rgbaPapayaWhip(A),rgbaPeachPuff(A),rgbaPeru(A),rgbaPink(A),rgbaPlum(A),rgbaPowderBlue(A),rgbaPurple(A),rgbaRed(A),
	rgbaRosyBrown(A),rgbaRoyalBlue(A),rgbaSaddleBrown(A),rgbaSalmon(A),rgbaSandyBrown(A),rgbaSeaGreen(A),rgbaSeaShell(A),rgbaSienna(A),rgbaSilver(A),rgbaSkyBlue(A),rgbaSlateBlue(A),rgbaSlateGray(A),rgbaSnow(A),rgbaSpringGreen(A),rgbaSteelBlue(A),rgbaTan(A),rgbaTeal(A),rgbaThistle(A),rgbaTomato(A),
	rgbaTransparent(A),rgbaTurquoise(A),rgbaViolet(A),rgbaVioletRed(A),rgbaWheat(A),rgbaWhite(A),rgbaWhiteSmoke(A),rgbaYellow(A),rgbaYellowGreen(A)

	</pre>
 @return        nothing.

 @see wcElSetStylePos,,wcElSetStyleColorHtml, wcElSetStyleColorNamed
 */
/*****************************************************************************/

void wcElSetStyleColorNamed(wcEL element,CSSPropertyType Prop, char *ColorName, wcBOOL refresh)
{
char StyleBuffer[80];
	rtp_sprintf(StyleBuffer, "%s:%s",gpCSSPropertyNames[Prop], ColorName);
	wcElSetStyle(element,StyleBuffer, refresh);
}


/*****************************************************************************/
// wcElSetStyleColorHtml
/**
 @memo  Set the color of an element using CSS.
 @doc  Formats a string and passes it to wcElSetStyle() to set the color an element using CSS. Uses the same color representation that is returned from wcElGetColorValue().

 @param  wcEL	Element     -   The element.
 @param  Prop    Which color property to set.
 @param  HTMLColor			Color - The type returned from wcElGetColorValue().
 @param  refresh      		Redraw the element immediately if wcTRUE.


 @return        nothing.

 @see wcElSetStylePos,,wcElSetStyleColorRgb, wcElSetStyleColorNamed
 */
/*****************************************************************************/
void wcElSetStyleColorHtml(wcEL element,CSSPropertyType Prop, HTMLColor color, wcBOOL refresh)
{
	 wcElSetStyleColorRgb(element,Prop, (int) HTML_ColorGetRed(color), (int)HTML_ColorGetGreen(color), (int) HTML_ColorGetBlue(color), refresh);
}

/*****************************************************************************/
// wcElSetStyleBgImage
/**
 @memo  Set the background image of an element using CSS.
 @doc  Formats a string and passes it to wcElSetStyle() to set the background image an element using CSS.
 <br> Note: The style background-repeat:repeat; is also applied.


 @param  wcEL				Element
 @param  char *          	bgImage
 @param  refresh      		Redraw the element immediately if wcTRUE.


 @return        nothing.

 @see wcElSetStyle
 */
/*****************************************************************************/
void wcElSetStyleBgImage(wcEL element, char *bgImage, wcBOOL refresh)
{
char StyleBuffer[512];
	rtp_sprintf(StyleBuffer, "background-repeat:repeat;background-image:url('%s');",bgImage);
	wcElSetStyle(element,StyleBuffer, refresh);
}
/*****************************************************************************/
// wcElSetStyleBorder
/**
 @memo  Set the border style for an element.
  @doc  Formats a string and passes it to wcElSetStyle() to set the border style for an element.<br>
		Note: Sets the border style to solid, the thickness to the passed in value and the color to the passed in value. If you need any othe combinations call \Ref{wcElSetStyle}.

 @param  wcEL          		Element     -   The element.
 @param  int          		Thickness  	-   Border thickness zero to disable.
 @param  char *        		Color     	-   Any named Color including rgb and rgba expressions.
 @param  char *        		Color     	-   Any named Color including rgb and rgba expressions.
 @param  refresh     		Redraw the element immediately if wcTRUE.


 @return        nothing.

 @see wcElSetStyle
 */
/*****************************************************************************/

void wcElSetStyleBorder(wcEL element, int Thickness, char *Color, wcBOOL refresh)
{
char StyleBuffer[80];
	rtp_sprintf(StyleBuffer, "border-style:solid;border-width:%dpx;border-color:%s;",Thickness,Color);
	wcElSetStyle(element,StyleBuffer, refresh);
}

/*****************************************************************************/
// wcElSetStyleHidden
/**
 @memo  Set style for an element to hidden.
  @doc  Sets the style visibility:hidden; for the element.

 @param  wcEL          		Element     -   The element.
 @param  refresh     		Redraw the element immediately if wcTRUE.


 @return        nothing.

 @see wcElSetStyleVisible
 */
/*****************************************************************************/

void wcElSetStyleHidden(wcEL element, wcBOOL refresh)
{
	wcElSetStyle(element,"visibility:hidden;", refresh);
}
/*****************************************************************************/
// wcElSetStyleVisible
/**
 @memo  Set style for an element to visible.
  @doc  Sets the style visibility:visible; for the element.

 @param  wcEL          		Element     -   The element.
 @param  refresh     		Redraw the element immediately if wcTRUE.


 @return        nothing.

 @see wcElSetStyleVisible
 */
/*****************************************************************************/

void wcElSetStyleVisible(wcEL element, wcBOOL refresh)
{
	wcElSetStyle(element,"visibility:visible;", refresh);
}


/*****************************************************************************/
// wcElSetFocus
/**
 @memo          Set the current input focus to an element
 @doc           Makes the given element the input focus for keyboard and
                mouse events.
 @param element The element to set
 @param refresh If wcTRUE, then redraw this element
 @see          wcDocGetFocus
 */
/*****************************************************************************/

int wcElSetFocus(wcEL element, wcBOOL refresh)
{
	if (element)
	{
		HELEMENT_OBJ(element)->Focus();
		if (refresh)
		{
			HELEMENT_OBJ(element)->Refresh();
			WEBC_BROWSER_LOAD_ALL(HELEMENT_OBJ(element)->GetBrowser());
		}
		return 0;
	}
	return -1;
}


static void _webc_ElementClaimPointer(HELEMENT_HANDLE hElement, WEBC_BOOL doclaim)
{
    HTMLElement    *phtmlelement;
    DisplayElement *pdispelement;
    HTMLDocument *pDoc;
	HTMLBrowser  *pBrowser;


    phtmlelement = HELEMENT_OBJ(hElement);
    if (phtmlelement)
        pdispelement = phtmlelement->GetDisplayElement();
    else
        pdispelement = 0;
    if (!pdispelement)
        return;

	if (!pdispelement->GetManager())
        return;


	if (doclaim)
	    pdispelement->GetManager()->ClaimPointer(pdispelement);
    else
	    pdispelement->GetManager()->ReleasePointer(pdispelement);
    return;

	pDoc = (phtmlelement)? phtmlelement->GetDocument() : 0;
	pBrowser = (pDoc)? pDoc->GetBrowser() : 0;
    if (pBrowser)
    {
	    if (doclaim)
            pBrowser->GetDisplayManager()->ClaimPointer(pdispelement);
        else
            pBrowser->GetDisplayManager()->ReleasePointer(pdispelement);
    }
}

/*****************************************************************************/
// wcElClaimPointer
/**
 @memo  Claim the all traffic to this window for the element.
 @doc   Claims traffic to this window for the selected element.

 @param  wcEL	Element     -   The element.

 @return        nothing.

 @see wcWinClaimFocus,wcElReleasePointer,wcWinReleaseFocus
 */
/*****************************************************************************/
void wcElClaimPointer(wcEL element)
{
    _webc_ElementClaimPointer(element, WEBC_TRUE);
}

/*****************************************************************************/
// wcElReleasePointer
/**
 @memo  Release claim on all traffic to this window for the element.
 @doc   Release claim on all traffic to this window for the element.

 @param  wcEL	Element     -   The element.

 @return        nothing.

 @see wcWinClaimFocus,wcElClaimPointer,wcWinReleaseFocus
 */
/*****************************************************************************/
void wcElReleasePointer(wcEL element)
{
    _webc_ElementClaimPointer(element, WEBC_FALSE);
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
#endif // HEREHERE