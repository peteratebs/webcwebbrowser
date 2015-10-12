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



struct EnumParms
{
	wcCtx* Ctx;
	wcDocEnumFn pfn;
	int Intparm;
	void *Voidparm;
};

static WEBC_BOOL wcDocEnumerateFinderCallback(HTMLElement *pElem, int chore, void **ppContext)
{
struct EnumParms *Pe;
	Pe = (struct EnumParms *)ppContext;
	return (WEBC_BOOL) Pe->pfn(Pe->Ctx,(wcEL) pElem, Pe->Intparm, Pe->Voidparm);
	return(0);
}



/*****************************************************************************/
// wcDocEnumerate
/**

 @memo Traverse all elements in a document with user callback.
 @doc Traverse all elements in the current document and pass each element to a user supplied
 callback function. The callback instructs the enumerator to continue scanning or stop.

 @param  wcCtx*     	Ctx     	-   The current wc context.
 @param  wcDocEnumFn 	pfn    		-   User supplied callback function.
 @param  int 			Intparm 	-   Integer (command perhaps) passed to the callback fucntion.
 @param  void*			VoidParm 	-   Void pointer (pass by reference or return by reference.)

 <pre>
  The user callback function is of type:
  	typedef wcBOOL (*wcDocEnumFn)(wcCtx* Ctx, wcEL El, int UserIntParm, void *UserVoidParm);
	When the callback fuction returns wcFALSE the enumeration continues, when it returns wcTRUE, enumeration stops.

 Examples:
 	Scan the document for an element with id "findme" and return the element.

	Callback function
  	wcBOOL MyEnumFn(wcCtx* Ctx, wcEL El, int UserIntParm, void *UserVoidParm)
	{
        if (wcELIdMatches(El, "findme"))
		{
            *((wcEL *) UserVoidParm) = El; << return the element and stop
			return wcTRUE;
		}
		else
		return wcFALSE;
	}
	wcEL MyFindFunction(wcCtx* Ctx)
	{
		wcEl answer = 0;
		wcDocEnumerate( Ctx, MyEnumFn, 0, (void *) &answer);
		return answer;
	}
 </pre>


 @return Nothing.

 @see
 */
/*****************************************************************************/

void wcDocEnumerate(wcCtx* Ctx, wcDocEnumFn pfn, int Intparm, void *Voidparm)
{
struct EnumParms E;
	E.Ctx = Ctx;
	E.pfn = pfn;
	E.Intparm = Intparm;
	E.Voidparm = Voidparm;
	HTMLGenericFinder finder(wcDocEnumerateFinderCallback,0,(void **) &E);
	((HTMLDocument *) Ctx->hdoc)->FindElement(&finder);
}

/*****************************************************************************/
// wcDocGetBody
/**

 @memo Return the body element of a document.
 @doc  Return the body element of a document, which can then be used to set the body's event handler, or inner html property.

 @param  wcCtx*     Ctx    -   The current wc context.

 @return		The handle of the body element with the specified id, or zero.

 @see
 */
/*****************************************************************************/


wcEL wcDocGetBody (wcCtx* Ctx)
{
	if (Ctx && Ctx->hdoc)
		return (wcEL) (HELEMENT_HANDLE) HDOC_OBJ(Ctx->hdoc)->Body();
	return 0;
}


/*****************************************************************************/
// wcDocOpen
/**

 @memo		Open an HTML Document for writing
 @doc			This function must preceed any calls to wcDocWriteHtml and
            wcDocWriteString

 @param Ctx		The current wc context.
 @return    0 if the operation was successful, -1 otherwise.
 @see           HDOC_HANDLE, wcDocClear, wcDocClose, wcDocWriteHtml,
                wcDocWriteString
 */
/*****************************************************************************/

int wcDocOpen(wcCtx* Ctx)
{
	if (!Ctx || !Ctx->hdoc)
		return -1;
	return HDOC_OBJ(Ctx->hdoc)->Open();
}


/*****************************************************************************/
// wcDocClose
/**

 @memo		Close an HTML Document after writing content
 @doc			This function should be called on an HTML Document as soon as
            the user is finnished writing HTML and text content to it
            (\Ref{wcDocWriteHtml}, \Ref{wcDocWriteString}).

 @param Ctx		The current wc context.
 @return    0 if the operation was successful, -1 otherwise.
 @see       HDOC_HANDLE, wcDocClear, wcDocOpen, wcDocWriteHtml,
            wcDocWriteString
 */
/*****************************************************************************/

void wcDocClose(wcCtx* Ctx)
{
	if (!Ctx || !Ctx->hdoc)
		return;
	HDOC_OBJ(Ctx->hdoc)->Close();
}


/*****************************************************************************/
// wcDocGetBrowser
/**

 @memo		Retrieve an HTML Document's parent HTML Browser
 @doc

 @param doc	The handle of the document
 <pre>
	Example - Initialize a context from an element handle.
	 wcCtxtInit(wcDocGetBrowser(wcElGetDocument(element)), wcElGetDocument(element));
 </pre>
 @return    The handle of the HTML Browser containing the given document.
 @see       HDOC_HANDLE
 */
/*****************************************************************************/

wcBROW wcDocGetBrowser(wcDOC doc)
{
	return ((wcBROW)HDOC_OBJ(doc)->GetBrowser());
}

/*****************************************************************************/
// wcDocRefresh
/**

 @memo			Update the document and refresh the display
 @doc			Loads any outstanding URLs (including the source for the document)
                and redraws the document.

 @param Ctx		The current wc context.
 @see           HDOC_HANDLE
 */
/*****************************************************************************/

void wcDocRefresh(wcCtx* Ctx)
{
	if (!Ctx || !Ctx->hdoc)
		return;
	HDOC_OBJ(Ctx->hdoc)->Update(1,0);
	WEBC_BROWSER_LOAD_ALL(HDOC_OBJ(Ctx->hdoc)->GetBrowser());
	HDOC_OBJ(Ctx->hdoc)->Refresh();
}



/*****************************************************************************/
// wcDocClear
/**

 @memo			Clear all document content
 @doc			Will discard the content of this document and free all associated
                memory.

 @param Ctx		The current wc context.
 @param refresh Boolean - whether to redraw the document window
 @see           HDOC_HANDLE
 */
/*****************************************************************************/

void wcDocClear(wcCtx* Ctx, wcBOOL refresh)
{
	if (!Ctx || !Ctx->hdoc)
		return;
	HDOC_OBJ(Ctx->hdoc)->Clear();
	if (refresh)
	{
		HDOC_OBJ(Ctx->hdoc)->Refresh();
	}
}


/*****************************************************************************/
// wcDocWriteHtml
/**

 @memo			Write html code directly to a document
 @doc			This function will append html code onto the end of the
                existing document.  The string passed to this function is
                treated exactly like html code arriving via HTTP or FILE.

 @param Ctx		The current wc context.
 @param html_src Pointer to a null-terminated buffer of html source.
 @param length   The length of html_src.
 @param refresh  Whether to redraw the document window
 @return The number of characters successfully written to the document.
 @see           HDOC_HANDLE, wcDocClear
 */
/*****************************************************************************/

long wcDocWriteHtml(wcCtx* Ctx, char *html_src, long length, wcBOOL refresh)
{
int closedoc = 0;
long result;
WEBC_CHAR *to_write = 0;

	if (!Ctx || !Ctx->hdoc)
		return -1;
	if (!HDOC_OBJ(Ctx->hdoc)->IsOpen())
	{
		HDOC_OBJ(Ctx->hdoc)->Open();
		closedoc = 1;
	}

	//allocate the right amount of memory
	to_write = (WEBC_CHAR *) WEBC_MALLOC((length + 1) * sizeof(WEBC_CHAR));
	if (!to_write)
	{
		return (-1);
	}

	webc_c_strncpy(to_write, html_src, length);
	to_write[length] = 0;

	result = HDOC_OBJ(Ctx->hdoc)->Write(to_write, length);

	WEBC_FREE(to_write);

	if (closedoc)
	{
		HDOC_OBJ(Ctx->hdoc)->Close();
	}

	if (refresh)
	{
		WEBC_BROWSER_LOAD_ALL(HDOC_OBJ(Ctx->hdoc)->GetBrowser());
		HDOC_OBJ(Ctx->hdoc)->Refresh();
	}

	return result;
}



/*****************************************************************************/
// webc_DocGetBrowser
/* X Deprecated from published API

 @memo		Retrieve an HTML Document's parent HTML Browser
 @doc

 @param doc	The handle of the document
 @return    The \Ref{HBROWSER_HANDLE} of the HTML Browser containing the given document.
 @see       HDOC_HANDLE
 */
/*****************************************************************************/

HBROWSER_HANDLE webc_DocGetBrowser(HDOC_HANDLE doc)
{
	return ((HBROWSER_HANDLE)HDOC_OBJ(doc)->GetBrowser());
}



#if(1)

/*****************************************************************************/
// wcDocFindEl
/**

 @memo Search the document for a specific id.
 @doc			Search of the document tree for the first element with the specified id.

 @param  wcCtx*     Ctx    -   The current wc context.
 @param  char*     Id	   -   The id to search for.

 @return		The handle of the element with the specified id, or zero.

 @see
 */
/*****************************************************************************/


wcEL wcDocFindEl (wcCtx* Ctx, char *id)
{
HTMLIdNameTypeIndexFinder8 f(id, 0, HTML_ELEMENT_ANY, 0);
	return (wcEL) (((HTMLDocument *) Ctx->hdoc)->FindElement(&f));
}




/*****************************************************************************/
// wcDocGetFocus
/**
 @memo          Get the current input focus element for this document.
 @doc           Get the handle to element that is the input focus for keyboard and
                mouse events for this document.
 @param Doc		Handle of the document.

 @see          wcElSetFocus,wcWinSetFocus,wcWinGetFocus
 @return       Handle to the element, or 0.
 */
/*****************************************************************************/

wcEL wcDocGetFocus(wcDOC Doc)
{
	if (Doc)
	{
	HTMLFocusElementFinder f;
	HTMLElement *pElement;
			// pElement = (HDOC_OBJ(Ctx->hdoc))->FindElement(&f);
		pElement =(HDOC_OBJ(Doc)->FindElement(&f));
		return (wcEL) pElement;
	}
	return (wcEL) (0);
}

/*****************************************************************************/
// wcDocInsertFirst
/**

 @memo  Insert an element into the first position of a document.
 @doc   Insert an element into the first position of a document.

 @param  wcCtx*    		Ctx    -   The current wc context.
 @param  wcEL         	Child  -   The element to insert.


 @return		nothing

 @see
 */
/*****************************************************************************/
void wcDocInsertFirst (wcCtx* Ctx,wcEL eL)
{

	((HTMLDocument *) Ctx->hdoc)->Body()->InsertFirst((HTMLElement *) eL);
}
/*****************************************************************************/
// wcDocInsertLast
/**

 @memo  Insert an element into the last position of a document.
 @doc  Insert an element into the last position of a document.

 @param  wcCtx*    		Ctx    -   The current wc context.
 @param  wcEL         	Child  -   The element to insert.


 @return		nothing

 @see
 */
/*****************************************************************************/
void wcDocInsertLast (wcCtx* Ctx,wcEL eL)
{
	((HTMLDocument *) Ctx->hdoc)->Body()->InsertLast((HTMLElement *) eL);
}



/*****************************************************************************/
// wcDocFindElement
/**

 @memo			Search for an element by id, name, type, and/or index
 @doc			Does a depth-first search of the document tree for an element
                matching all of the search criteria

 @param Ctx		The current wc context.
 @param id      The \Ref{ID} of the element to search for (WEBC_NULL to match any id)
 @param name	The \Ref{NAME} of the element to search for (WEBC_NULL to match any name)
 @param type    The \Ref{TYPE} of element to search for (HTML_ELEMENT_ANY to match any type)
 @param index   The index of the element to find (HTML_INDEX_LAST for the last
                element in the document)
 @return		The handle of the Nth element with the specified id, name, and/or type,
                where N is the above specified index; WEBC_NULL if not found.
 @see           HDOC_HANDLE, wcEL
 */
/*****************************************************************************/

wcEL wcDocFindElement (wcCtx* Ctx, char *id, char *name, HTMLElementType type, long index)
{
HTMLIdNameTypeIndexFinder8 f(id, name, type, index);
	if (!Ctx || !Ctx->hdoc)
		return (0);
	return (wcEL) (HDOC_OBJ(Ctx->hdoc)->FindElement(&f));
}


/*****************************************************************************/
// wcDocSetUrl
/**

 @memo			Set the URL of a document
 @doc			Sets the URL of the specified document.  The new page will not be loaded
                (unless 'refresh' is set to WEBC_TRUE) until \Ref{wcDocRefresh} is called
                on this document.

 @param Ctx		The current wc context.
 @param url     The URL of the document to load
 @param refresh If TRUE, then load the given URL immediately and refresh the display;
                otherwise wait until \Ref{wcDocRefresh} is called.
 @see           HDOC_HANDLE, wcDocRefresh
 */
/*****************************************************************************/

void wcDocSetUrl(wcCtx* Ctx, char *_url, wcBOOL refresh)
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR url[HTML_TEMP_STR_LEN];
	webc_c_strncpy(url, _url, HTML_TEMP_STR_LEN-1);
	url[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *url = _url;
  #endif
	if (!Ctx || !Ctx->hdoc)
		return;

	HDOC_OBJ(Ctx->hdoc)->SetSrc(url);
	HDOC_OBJ(Ctx->hdoc)->Update(1,0);
	if (refresh)
	{

		WEBC_BROWSER_LOAD_ALL(HDOC_OBJ(Ctx->hdoc)->GetBrowser());
	}
}




/*****************************************************************************/
// wcDocWriteString
/**

 @memo			Write string directly to a document as is
 @doc			This function will copy and change this string so that it will
				appear as is in the document.

 @param Ctx		The current wc context.
 @param str      Pointer to a null-terminated string.
 @param length   The length of html_src.
 @param refresh  Whether to redraw the document window
 @return The number of characters successfully written to the document.
 @see           HDOC_HANDLE, wcDocClear
 */
/*****************************************************************************/

long wcDocWriteString(wcCtx* Ctx, char *str, long length, wcBOOL refresh)
{
int closedoc = 0;
long result = 0;
long new_str_size = 0;
WEBC_CHAR *to_write = 0;

	if (!Ctx || !Ctx->hdoc)
		return -1;
	//find the size of the new string
	new_str_size = webc_escape_string(0, str);

	//allocate the right amount of memory
	to_write = (WEBC_CHAR *) WEBC_MALLOC((new_str_size + 1) * sizeof(WEBC_CHAR));
	if (!to_write)
	{
		return (-1); // malloc error
	}

	webc_escape_string(to_write, str);

	if (!HDOC_OBJ(Ctx->hdoc)->IsOpen())
	{
		HDOC_OBJ(Ctx->hdoc)->Open();
		closedoc = 1;
	}

	result = HDOC_OBJ(Ctx->hdoc)->Write(to_write);

	if (closedoc)
	{
		HDOC_OBJ(Ctx->hdoc)->Close();
	}

	if (refresh)
	{
		WEBC_BROWSER_LOAD_ALL(HDOC_OBJ(Ctx->hdoc)->GetBrowser());
		HDOC_OBJ(Ctx->hdoc)->Refresh();
	}

	WEBC_FREE(to_write);
	return result;
}

// ******************* Unpublished APIs below this point


/*****************************************************************************/
// webc_DocFindElement
/*X Deprecated from published API

 @memo			Search for an element by id, name, type, and/or index
 @doc			Does a depth-first search of the document tree for an element
                matching all of the search criteria

 @param doc		The handle of the document to search
 @param id      The \Ref{ID} of the element to search for (WEBC_NULL to match any id)
 @param name	The \Ref{NAME} of the element to search for (WEBC_NULL to match any name)
 @param type    The \Ref{TYPE} of element to search for (HTML_ELEMENT_ANY to match any type)
 @param index   The index of the element to find (HTML_INDEX_LAST for the last
                element in the document)
 @return		The handle of the Nth element with the specified id, name, and/or type,
                where N is the above specified index; WEBC_NULL if not found.
 @see           HDOC_HANDLE, HELEMENT_HANDLE
 */
/*****************************************************************************/

HELEMENT_HANDLE webc_DocFindElement (HDOC_HANDLE doc, char *id, char *name, HTMLElementType type, long index)
{
HTMLIdNameTypeIndexFinder8 f(id, name, type, index);
	if (!doc)
		return (0);
	return (HELEMENT_HANDLE) (HDOC_OBJ(doc)->FindElement(&f));
}

// Deprecated - please remove
void webc_DocSetUrl(HDOC_HANDLE doc, char *_url, HTML_BOOL refresh)
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR url[HTML_TEMP_STR_LEN];
	webc_c_strncpy(url, _url, HTML_TEMP_STR_LEN-1);
	url[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *url = _url;
  #endif

	HDOC_OBJ(doc)->SetSrc(url);
	HDOC_OBJ(doc)->Update(1,0);
	if (refresh)
	{
		WEBC_BROWSER_LOAD_ALL(HDOC_OBJ(doc)->GetBrowser());
	}
}





/*****************************************************************************/
// webc_DocSaveForOffline
/*X Deprecated from published API

 @memo		Retrieve an HTML Document's parent HTML Browser
 @doc

 @param doc	The handle of the document
 @return    The \Ref{HBROWSER_HANDLE} of the HTML Browser containing the given document.
 @see       HDOC_HANDLE
 */
/*****************************************************************************/

void webc_DocSaveForOffline(HDOC_HANDLE hdoc)
{
  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
	HDOC_OBJ(hdoc)->SaveContentForOffline();
  #endif // WEBC_SUPPOR_TOFFLINE_BROWSING
}

void webc_SetOnlineStatus (
		WEBC_BOOL isOnline
	)
{
  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
	giWebcOnline = isOnline;
  #endif // WEBC_SUPPOR_TOFFLINE_BROWSING
}

WEBC_BOOL webc_GetOnlineStatus (void)
{
  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
	return (webc_online());
  #else
	return WEBC_TRUE;
  #endif // WEBC_SUPPOR_TOFFLINE_BROWSING
}

#endif // #if(1)