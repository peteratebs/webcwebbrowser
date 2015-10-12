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

void wcUnSetBrowserFocus(wcBROW Ctx)
{
	;// wcUnSetBrowserFocus(Ctx->hbrowser);
}
void wcClaimBrowserFocus(wcBROW Ctx)
{
	;// wcUnSetBrowserFocus(Ctx->hbrowser);
}
int webc_GuiInitConfig(HTMLBrowserConfig *pconfig);
static HTMLEventStatus WinCreatecb (HBROWSER_HANDLE hbrowser,HTMLBrowserEvent* event);

/*****************************************************************************/
// wcWinCreateEx
/**

 @memo   Create a browser window.
 @doc    Opens a new browser view in a window with specified border style, color, position and mode.
 		 Load optional initial content and sets a user supplied handler function to be called when the view is created.
		 The Window config structure can be used to set the border color, background color, and border style.
		 The Window config structure can be used to control window properties including if it is modal and if it is resizable.


 @param  	wcCtx *Ctx 						Context to initialize with document information. This context block may be used as a handle to other wcAPI functions.
 @param  	char *_windowname 				The DOM name to use for retrieve the window from the list of all windows.
 @param  	wcEventCallback BodyCallback	If non-zero provides a callback function that is activated when the window is created. May be used initialize application fragments and to dynamically generate content and insert into the page.
 @param  	char *_defaultUrl				If non-zero initialize the window from the content at the URL.
 @param  	char *_defaultContent           If non-zero initialize the window from content stored in this string.
 @param		struct HTMLBrowserWindowConfig *pWindowConfig;
<pre>
			pWindowConfig must be zeroed and then initialized with proper values.
        struct HTMLBrowserWindowConfig
		{
            DISPLAY_INT top;                top, left, height and width of the Window.
            DISPLAY_INT left;
            DISPLAY_INT height;
            DISPLAY_INT width;
        	DISPLAY_INT BorderThickness;    Thickness of the border in pixels
        	WEBC_UINT32	BorderColor;        Border Color in RGBA
        	WEBC_UINT32 BackGroundColor;    Background Color in RGBA
        	WEBC_UINT32 BorderStyle;        BS_RAISED,BS_LOWERED,BS_RIDGE or BS_GROOVE
			WEBC_UINT32 Radius;             In non-zero create rounded corners, with raius in pixels.
        	WEBC_UINT32 WindowFlags;        Or together the folowing options

        	WF_DISABLESCROLLBARS  - If set content is clipped and no scrollbars appear on overflow.
        	WF_MODAL			  - If set the focus is owned by the window until released.
			                        Note: You should call wcWinExecute(); to process a modal window
        	WF_SAVEUNDER          - If set the screen contents are saved when the window is created and restored when
        	                        the window is destroyed.
        	WF_SPRITE             - Like WF_SAVEUNDER but if the window is moved it manipulates the frame buffer
                                    without repainting.
        };

</pre>
 <pre>
Example:
</pre>


 @return wcWinCreateEx returns 0 if it was able to create the window, -1 otherwise.

 @see wcWinCreate, wcWinClose
 */
/*****************************************************************************/


int wcWinCreateEx(
	wcCtx *Ctx,
	char *_windowname,
	wcEventCallback BodyCallback,
	char *_defaultUrl,
	char *_defaultContent,
	struct HTMLBrowserWindowConfig *pWindowConfig
  )
{
	HTMLBrowserConfig config, *pconfig;

    rtp_memset(Ctx, 0, sizeof(*Ctx));
	pconfig = (HTMLBrowserConfig *)&config;
    rtp_memset(pconfig, 0, sizeof(*pconfig));
	if (webc_GuiInitConfig(pconfig) < 0)
		return -1;
    pconfig->browserEventHandler = WinCreatecb;
    pconfig->privateData = (void*) BodyCallback;

	pconfig->WindowConfig = *pWindowConfig;
	Ctx->hbrowser = webc_CreateBrowser(pconfig,  WEBC_TRUE, _windowname , "");
	if (!Ctx->hbrowser)
		return -1;
	// Pass the user's callback structure to ourselves through browser private data.
	if (BodyCallback)
	{
		webc_BrowserSetPrivateData((HBROWSER_HANDLE) Ctx->hbrowser, (void*) BodyCallback);
	}

	if (pWindowConfig->WindowFlags & WF_DISABLESCROLLBARS)
		((HTMLBrowser *)Ctx->hbrowser)->SetScrollMode(SCROLL_MODE_NONE);
	else
		((HTMLBrowser *)Ctx->hbrowser)->SetScrollMode(SCROLL_MODE_AUTO);

	Ctx->hdoc = webc_BrowserGetDocument(Ctx->hbrowser);
	if (_defaultUrl)
		webc_BrowserLoadUrl(Ctx->hbrowser, _defaultUrl);
	else if (_defaultContent)
	{
		if (wcDocWriteHtml(Ctx, _defaultContent, rtp_strlen(_defaultContent), (wcBOOL)WEBC_TRUE) < 0)
			return -1;
	}


	if (pWindowConfig->WindowFlags & WF_MODAL)
	{
		// Lock the focus, may be released
		wcClaimBrowserFocus(Ctx->hbrowser);
	}
	return 0;
}

static HTMLEventStatus WinCreatecb (HBROWSER_HANDLE hbrowser,HTMLBrowserEvent* event)
{
	switch (event->type)
	{
		case HTML_BROWSER_EVENT_DOCUMENT_TITLE_CHANGE:
		{
			wcEventCallback BodyCallback = (wcEventCallback) webc_BrowserGetPrivateData((HBROWSER_HANDLE) hbrowser);
			if (BodyCallback)
			{
				wcCtx _Ctx;
				wcCtxtInit(&_Ctx, (wcBROW) hbrowser, (wcDOC) event->hdoc);
				HTMLEvent e;
				// Pass an HTML_EVENT_LOAD event
				e.type = HTML_EVENT_LOAD;
				if (BodyCallback(&_Ctx, (wcEL) 0,&e,0) == HTML_EVENT_STATUS_CONTINUE)
				{
					HELEMENT_HANDLE el = webc_DocFindElement (event->hdoc, 0, 0, HTML_ELEMENT_ANY, 0);
					// Set the Body event handler, that will trigger a CREATE_OBJECT event.
					wcElSetEventHandler(el, BodyCallback);
				}
			}
		}
		break;
		case HTML_BROWSER_EVENT_UNKNOWN_CONTENT_TYPE:
		case HTML_BROWSER_EVENT_STATUS_CHANGE:
		case HTML_BROWSER_EVENT_LOAD_START:
		case HTML_BROWSER_EVENT_LOAD_DONE:
		case HTML_BROWSER_EVENT_DOCUMENT_URL_CHANGE:
		case HTML_BROWSER_EVENT_HTTP_NOT_AUTHORIZED:
		case HTML_BROWSER_EVENT_HTTP_AUTHORIZATION_REJECTED:
		case HTML_BROWSER_EVENT_BLANK_DOCUMENT_REQUESTED:
			break;
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

static wcBOOL wcWinExecuteRunning;
static int wcWinExecuteReturnValue = 0;

/*****************************************************************************/
// wcWinExecute
/**

 @memo   Perfrom modal cycling of webC message loop on the context until stopped by wcWinExecuteStop.
 @doc    Cycles the webC message loop on the context until stopped by a call to wcWinExecuteStop from inside an event handler.


 @param  	wcCtx *Ctx 						Context of the window to cycle on.

 @return The integer return code passed to wcWinExecuteStop.

 Example:


 @see wcWinCreateEx, wcWinClose,wcWinExecuteStop
 */
/*****************************************************************************/
int wcWinExecute(wcCtx *Ctx)
{
	wcWinExecuteReturnValue = 0;
	wcWinExecuteRunning = wcTRUE;
	HTMLBrowser *pBrowser = (HTMLBrowser *) Ctx->hbrowser;
    while (wcWinExecuteRunning && pBrowser->ProcessInputQueue(WEBC_TRUE) >= 0)
		;
	return wcWinExecuteReturnValue;
}
/*****************************************************************************/
// wcWinExecuteStop
/**

 @memo   Tell wcWinExecute to stop cycling and return a value to its caller.
 @doc    Tell wcWinExecute to stop cycling and return a value to its caller.


 @param  	wcCtx *Ctx 						Context of the window to cycle on.
 @param  	int   ReturnValue				Value to return to the caller of wcWinExecute.

 @return Nothing

 Example:


 @see wcWinCreateEx, wcWinClose,wcWinExecute
 */
/*****************************************************************************/

void wcWinExecuteStop(wcCtx *Ctx, int ReturnValue)
{
	wcWinExecuteReturnValue = ReturnValue;
	wcWinExecuteRunning = wcFALSE;
}

/*****************************************************************************/
// wcWinClaimFocus
/**

 @memo   Claim focus for a browser window.
 @doc    Instructs webC to disard all events not destined for the current window. The Claim may be relesed by calling wcWinReleaseFocus. The claim is automatically released if
 wcWinClose is called. wcWinClaimFocus is called by wcWinCreate and wcWinCreateEx when a Modal window is requested.

 @param  	wcCtx *Ctx 						Context to claim focus for.

 @return Nothing

 @see wcWinReleaseFocus
 */
/*****************************************************************************/

void wcWinClaimFocus(wcCtx *Ctx)
{
	wcClaimBrowserFocus(Ctx->hbrowser);
}

/*****************************************************************************/
// wcWinReleaseFocus
/**

 @memo   Release focus that was previously claimed for this browser view.
 @doc    Instructs webC to release the hold the browser view has on events dispatching and allow other browser views to
         recieve events.

 @param  	wcCtx *Ctx 						Context to claim focus for.

 @return Nothing

 @see wcWinClaimFocus
 */
/*****************************************************************************/

void wcWinReleaseFocus(wcCtx *Ctx)
{
	wcUnSetBrowserFocus(Ctx->hbrowser);
}

/*****************************************************************************/
// wcWinSetFocus
/**

 @memo   Set the focus to a browser window and element.
 @doc    Instructs webC to route keyboard events to the current window.

 @param  	wcCtx *Ctx 			- Window to set focus to. Current context if window already has focus.
 @param  	wcEL  Element		- Element in the window. If zero use the windows current focus element.

 @return wcTRUE on success.

 @see wcWinGetFocus
 */
/*****************************************************************************/

wcBOOL wcWinSetFocus(wcCtx *Ctx,wcEL  Element)
{

	if (gpFocusBrowser != (HTMLBrowser *) Ctx->hbrowser)
	{
		gpFocusBrowser = (HTMLBrowser *) Ctx->hbrowser;
		if (gpFocusBrowser->GetDisplayManager())
		{
			WebGraphics *gc = gpFocusBrowser->GetDisplayManager()->GetGraphics();
			if (gc)
				gc->BringToTop();
		}
	}

	if (Element)
	{
		if (webc_ElementFocus((HELEMENT_HANDLE) Element, WEBC_TRUE) >= 0)
			return wcTRUE;
	}
	return wcFALSE;
}


/*****************************************************************************/
// wcWinMoveTo
/**

 @memo   Move a browser window.
 @doc    Move a browser window.

 @param  	wcCtx *Ctx 			- Window to move.
 @param  	int		left		- Move to left, top (x,y)
 @param  	int		top			-

 @return wcTRUE on success.

 @see wcWinGetFocus
 */
/*****************************************************************************/
void wcWinMoveTo(wcCtx *Ctx,int left, int top)
{
int oldleft,oldtop,width,height;
	wcWinGetPos(Ctx, &oldleft, &oldtop, &width, &height);
	wcWinSetPos(Ctx, left, top, width, height);
}

// wcWinBringToTop
/**

 @memo   Raise window to z-order stack top.
 @doc    Raise browser to z-order stack top.

 @param  	wcCtx *Ctx 			- Window to raise

 @return wcTRUE on success.

 */
/*****************************************************************************/
void wcWinBringToTop(wcCtx *Ctx)
{
	if (((HTMLBrowser *) Ctx->hbrowser)->GetDisplayManager())
	{
		WebGraphics *gc = ((HTMLBrowser *) Ctx->hbrowser)->GetDisplayManager()->GetGraphics();
		if (gc)
			gc->BringToTop();
	}
}

/*****************************************************************************/
// wcWinGetPos
/**
 @memo  Get the displayed position of a window.
 @doc           Get the current position of a window.

 @param  wcCtx *			Ctx 		- 	Window to report on.
 @param  int*          		top      	-   top goes here. If NULL no result is returned for top.
 @param  int*          		left      	-   left goes here. If NULL no result is returned for leftl.
 @param  int*          		height     	-   height goes here. If NULL no result is returned for height.
 @param  int*          		width     	-   width goes here. If NULL no result is returned for width.

 @return        wcTRUE if the return arguments are updated.
 @return		wcFALSE otherwise.

 @see wcWinSetPos
 */
/*****************************************************************************/

wcBOOL wcWinGetPos(wcCtx *Ctx, int *left, int *top, int *width, int *height)
{
	if (((HTMLBrowser *) Ctx->hbrowser)->GetDisplayManager())
	{
		WebGraphics *gc = ((HTMLBrowser *) Ctx->hbrowser)->GetDisplayManager()->GetGraphics();
		if (gc)
			gc->GetPos(left, top, width, height);
		return(wcTRUE);
	}
	return(wcFALSE);
}


/*****************************************************************************/
// wcWinSetPos
/**
 @memo  Set the position, height and width of a window.
 @doc  Set the position, height and width of a window.

 @param  wcCtx *			Ctx 		- 	Window to report on.
 @param  int          		top      	-   New top position
 @param  int          		left      	-   New left position.
 @param  int          		height     	-   New height.
 @param  int          		width     	-   New width.

 @return        wcTRUE on succes.

 @see wcWinSetPos
 */
/*****************************************************************************/


wcBOOL wcWinSetPos(wcCtx *Ctx, int left, int top, int width, int height)
{

	if (((HTMLBrowser *) Ctx->hbrowser)->GetDisplayManager())
	{
		WebGraphics *gc = ((HTMLBrowser *) Ctx->hbrowser)->GetDisplayManager()->GetGraphics();
		if (gc)
			gc->SetPos(left, top, width, height);
		return(wcTRUE);
	}
	return(wcFALSE);
}


/*****************************************************************************/
// wcWinGetFocus
/**

 @memo			Search for the element and or window with focus.
 @doc			If the Context is uninitialized find the browser window with focus. Do a depth-first search of the document tree of the focus window
 for the element having focus.

 @return		wcEL - The handle of the element with focus.
 @see           wcWinSetFocus,wcWinClaimFocus,wcWinReleaseFocus
 */
/*****************************************************************************/

wcEL wcWinGetFocus(wcCtx *Ctx)
{	// Note - will superceed webc_FindFocusElement()
	if (!Ctx->hbrowser)
	{
		Ctx->hbrowser = webc_GetBrowserWithFocus();
		if (Ctx->hbrowser)
			Ctx->hdoc = (wcDOC) (HBROWSER_OBJ(Ctx->hbrowser)->GetDocument());
	}
	if (Ctx->hbrowser)
	{
		if (Ctx->hdoc)
		{
			HTMLFocusElementFinder f;
			HTMLElement *pElement;
			// pElement = (HDOC_OBJ(Ctx->hdoc))->FindElement(&f);
			pElement =(HDOC_OBJ(Ctx->hdoc)->FindElement(&f));
			return (wcEL) pElement;
		}
	}
	return (wcEL) (0);
}

/*****************************************************************************/
// wcWinClose
/**

 @memo   Close a browser window.
 @doc    Close the current browser view, call object destroy message handlers and release internal resources.


 @param  	wcCtx 	*Ctx 					Context of the window to close.

 @return Nothing.

 @see wcWinCreateEx, wcWinCreate
 */
/*****************************************************************************/

void wcWinClose(wcCtx *Ctx)
{
	webc_DestroyBrowser(Ctx->hbrowser,0);
}
