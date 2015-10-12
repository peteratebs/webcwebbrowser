//
// wceventapi.cpp -
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
#include "webcsdk.hpp"
#include "rtpprint.h"

/*****************************************************************************/
// Global Data
/*****************************************************************************/
#if (WEBC_SUPPORT_USER_EVENTS)
CFunctionHandler *gpCHandlerTable[WEBC_CFG_MAX_EVENT_HANDLERS];
#endif // WEBC_SUPPORT_USER_EVENTS


/*****************************************************************************/
// wcElTriggerEvent
/**
 @memo  Send an event to an element immediately or with delay.
 @doc           Pass an event to an element's event handler routine.

 @param  wcEL          		Element     -   The element.
 @param  HTMLEvent *		pEvent    	-   Initialized event structure to pass to the handler.
 @param  int 				delay   	-   Delay in miliseconds before triggering the event. Zero for no delay.


 @return        HTMLEventStatus. If Delay is non-zero HTML_EVENT_STATUS_CONTINUE otherwise the return value from the element's event handler.

 */
/*****************************************************************************/

HTMLEventStatus wcElTriggerEvent(wcEL El, HTMLEvent *pEvent, int delay)
{
	if (delay == 0)
	{
		// New code..
		// If the element has a display element with an event handler call the Display element
		// This lets us pass event like mouse presses to the display element.
		// Events are passed through to the element.
		// Verify the Eventss are passed through for all diisplay elemtns

		DisplayElement *pDisplay = ((HTMLElement *)El)->GetDisplayElement();
		if (pDisplay)
			return pDisplay->Event(pEvent);
		else
			return webc_TriggerEvent((HELEMENT_HANDLE) El, pEvent);
	}
	else if (wcElGetDocument(El))
	{
		webc_BrowserQueueTimedEvent(webc_DocGetBrowser(wcElGetDocument(El)),(HELEMENT_HANDLE) El,pEvent,(WEBC_INT32) delay);
		return HTML_EVENT_STATUS_CONTINUE;
	}
	else
		return HTML_EVENT_STATUS_CONTINUE;

}

#ifdef HEREHERE



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


#ifdef NOTDEF // Conditionally excluded to satisfy docpp
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
	param is typically unused.

	See below for a description of event types and the event structure.

	Example: see \Ref{ExElEventHandling}


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
}
#endif

#ifdef RTP_TRACK_LOCAL_MEMORY
void _wcElSetEventHandler(HELEMENT_HANDLE element, wcEventCallback cb, const char *file, const long line)
#else
void wcElSetEventHandler(HELEMENT_HANDLE element, wcEventCallback cb)
#endif
{
    if (element)
    {
        wcCFunctionHandler* eventHandler;
		WEBC_DEBUG_NEW(eventHandler, wcCFunctionHandler(0,cb),file,line,"wcCFunctionHandler");
        if (eventHandler)
        { // eventHandler will be freed by HELEMENT_OBJ(element)->PopElementEventHandler();
            HTMLEvent the_event;
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


#ifdef NOTDEF // Conditionally excluded to satisfy docpp
/*****************************************************************************/
// wcDocSetEventHandler
/**
 @memo          Assign an event handler to the body element of this document.
 @doc           Push the previous event handler for the body element on an internal stack and then
 				sets a new handler for events targeted at the body.	The previous event handler may be restored later by calling wcDocPopEventHandler.
 <br>
		 Note: wcDocSetEventHandler pushes the previous handler onto an internal stack. You must call
		 wcDocPopEventHandler at a later time if your application requires you to restore the previous handler when the current algorithm is completed.

 @param  wcCtx*     Ctx     -   The current wc context.
 @param  wcEventCallback 	cb	 -   Function to call when an event occurs.	(see \Ref{wcElSetEventHandler})


	wcDocSetEventHandler calls \Ref{wcElSetEventHandler} to attach the event handler to the documents body element.<br>

 	Document event handlers are actually just element event handlers acting on the body element of a document but since the body element is the containing element for all other elements
 	its event handlers provide some additional utility over other handlers for other elements.<br>

	<ul>
	<li>Can be used to configure allocate and initialize application screen specific data prior to loading content into the page.</li>
	<li>Can be used to configure release application screen specific data prior to release content from the page (loading new content or destroying the window).</li>
	<li>Can be used to catch events like clicks that land on the document background.</li>
	<li>Can be used to send events to the application itself and not necessarilly any of its elements.</li>
	<li>Can be used to catch events that were processes by elements in the document but not canceled (bubbled).</li>
	</ul>
	The document body event handler is especially useful for these events -
	<ul>
		<li>HTML_EVENT_LOAD                - Called when the document has been loaded with content by a call to \Ref{wcDocSetUrl} or \Ref{wcDocClose} or \Ref{wcWinCreateEx} or a Javascript equivalent.
		<i>Note: Body elements of documents that were created using pure dynamic HTML are never sent an HTML_EVENT_LOAD event notification.</i></li>
		<li>HTML_EVENT_UNLOAD              - Called every time the current content is about to be cleared to prepare for loading another page. Application screen "OnExit" processing should be placed in HTML_EVENT_UNLOAD clauses.</li>
		<li>HTML_EVENT_OBJECT_CREATED      - Called every time the document is cleared . Application screen "OnEntry" processing can be placed in the HTML_EVENT_OBJECT_CREATED clause but remember
		that the document will be empty. If a url is being loaded or \Ref{wcDocWriteHtml} it is being used HTML_EVENT_LOAD will be triggered when the content is loaded.</li>
    </ul>


	Example: See \Ref{ExDocEventHandling}



 @return        Nothing
 @see wcElSetEventHandler, wcDocPopEventHandler, wcDocTriggerEvent, ExDocEventHandling
 */
/*****************************************************************************/
void wcDocSetEventHandler(wcCtx* Ctx, wcEventCallback cb)
{
}
#endif


#ifdef RTP_TRACK_LOCAL_MEMORY
void _wcDocSetEventHandler(wcCtx* Ctx, wcEventCallback cb, const char *file, const long line)
#else
void wcDocSetEventHandler(wcCtx* Ctx, wcEventCallback cb)
#endif
{
	wcEL hBody = wcDocGetBody (Ctx);
	if (hBody)
	{
		// Set the Body event handler, that will trigger a CREATE_OBJECT event.
#ifdef RTP_TRACK_LOCAL_MEMORY
		_wcElSetEventHandler(hBody, cb, file, line);
#else
		wcElSetEventHandler(hBody, cb);
#endif
	}
}



/*****************************************************************************/
// wcDocPopEventHandler
/**
 @memo          Restore the previous event handler for this document's body element.
 @doc           Pop the previous event handler for the body element from an internal stack and make it
 				the new event handler for the specified element. This function returns the event handler to
				the handler prior to the last call to wcDocSetEventHandler.

 @param  wcCtx*     Ctx     -   The current wc context.
  <pre>
	Example:   See the example for \Ref{wcDocSetEventHandler}.
  </pre>


 @return        Nothing
 @see wcDocSetEventHandler
 */
/*****************************************************************************/

void wcDocPopEventHandler(wcCtx *Ctx)
{
	wcEL hBody = wcDocGetBody (Ctx);
	if (hBody)
	{
   		wcElPopEventHandler(hBody);
 	}
}

/*****************************************************************************/
// wcDocTriggerEvent
/**
 @memo  Send an event to the document's body element either immediately or with a delay.
 @doc   Pass an event to the document's body element either immediately or with a delay.

 @param  wcCtx*				Ctx			-   The current wc context.
 @param  HTMLEvent *		pEvent    	-   Initialized event structure to pass to the handler.
 @param  int 				delay   	-   Delay in miliseconds before triggering the event. Zero for no delay.


 @return        HTMLEventStatus. If Delay is non-zero HTML_EVENT_STATUS_CONTINUE otherwise the return value from the element's event handler.

 */
/*****************************************************************************/

HTMLEventStatus wcDocTriggerEvent(wcCtx *Ctx, HTMLEvent *pEvent, int delay)
{
	wcEL hBody = wcDocGetBody (Ctx);
	if (hBody)
	{
		return wcElTriggerEvent(hBody, pEvent, delay);
	}
	return HTML_EVENT_STATUS_CONTINUE;
}


/*****************************************************************************/
// webc_RegisterEventCallback
/**
 @memo           webC extension to assign a 'C' custom named event handler accessable from html source code.
 @doc            After this function is called, whenever an HTML tag loaded into WebC specifies its 'eventhandler' attribute (a WebC
                 HTML extension) as the name passed in, all events that happen to that tag will be passed on to the C function specified as
                 cb.
<br><pre>
Example:

static HTMLEventStatus MyEvHandler (HBROWSER_HANDLE hbrowser,HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,	char* param);
// This line of code registers a function named MyEvHandler to  be called for all html content with the attribute eventhandler="MyNamedEvent",
webc_RegisterEventCallback( MyEvHandler, "MyNamedEvent");

// An example of the content that would create an object with this default handler:
&lt;div eventhandler="MyNamedEvent"&gt;Hello&lt;/div&gt;.

// This function will be called as the default event handler for the above object.
static HTMLEventStatus MyEvHandler (HBROWSER_HANDLE hbrowser,HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,	char* param)
{
wcCtx Ctx;
wcEL El;

	// Example: Set up a context for calling wcXX API calls from a named default handler.
	El = (wcEL) element;
	wcCtxtInit(&Ctx, (wcBROW) hbrowser, (wcDOC) hdoc);
	// Do some work

	// Free the context
	wcCtxtRelease(&Ctx);
	return HTML_EVENT_STATUS_CONTINUE; // or HTML_EVENT_STATUS_DONE,HTML_EVENT_STATUS_HALT,HTML_EVENT_STATUS_ERROR

</pre>

 @param cb       The event handler callback.
 @param name     The name of this event handler
 @return         0 if the callback was successfully registered; -1 otherwise
 @see           wcElSetEventHandler,  webc_BrowserSetTagEventHandler
 */
/*****************************************************************************/

int webc_RegisterEventCallback (HTMLEventCallback cb, char *argName)
{
  #if (WEBC_SUPPORT_USER_EVENTS)
	int n;
	WEBC_CHAR* name;

   #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR unicodeName[64];
	webc_c_strncpy(unicodeName, argName, 64);
	unicodeName[63] = 0;
	name = unicodeName;
   #else
	name = argName;
   #endif // WEBC_SUPPORT_UNICODE

	for (n=0; n<WEBC_CFG_MAX_EVENT_HANDLERS; n++)
	{
		if (gpCHandlerTable[n] && !webc_strcmp(gpCHandlerTable[n]->Name(), name))
		{
			gpCHandlerTable[n]->mFunction = cb;
			return (0);
		}

		if (!gpCHandlerTable[n])
		{
			WEBC_NEW_VERBOSE(gpCHandlerTable[n], CFunctionHandler(name, cb),"CFunctionHandler");
			if (gpCHandlerTable[n])
			{
				HTML_RegisterEventHandler(gpCHandlerTable[n]);
				return (0);
			}
			break;
		}
	}
  #endif // WEBC_SUPPORT_USER_EVENTS

	return (-1);
}


/*****************************************************************************/
// webc_BrowserSetTagEventHandler
/**

 @memo   Specify a default event handler for all elements with the specified HTML tag type
 @doc Specify a default event handler for this tag type for this browser/window. This is different from wcElSetEventHandler in two important ways.
 <br> 1. If a new document is created or loaded in the window the handler remains in place.
 <br> 2. The handler is called for every element of the specified type, not just for a spefic element, for which a handler was assigned.
 <br><pre>
 // This code fragment attaches a default handler for the Body tag.
 Each time a new document is loaded the callback function is called.
 In this example the callback function tests the ID of the document body and initiaates startup code for the page based on the ID.

 // Attach a custom handler to the <BODY> tag
 webc_BrowserSetTagEventHandler (hbrowser,HTML_TAG_BODY, AppBuilderBodycb);
 ..
 // Example callback handler for the BODY tag. If it is a load of our ID, "MainWindow", we let load processing complete and start a timer to call us
    again in 100 milliseconds to start the main application.
 static HTMLEventStatus newAppBuilderBodycb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
	if (element && wcELIdMatches(element, "MainWindow"))
	{
    	switch (event->type)
    	{
    		case HTML_EVENT_LOAD:
    		{
				// queu up the application to run in 100 msec (after background was drawn)
    			HTMLEvent Event;
    			rtp_memset(&Event, 0 ,sizeof(Event));
    			Event.type = HTML_EVENT_TIMER;
    			wcElTriggerEvent(element, &Event, 100);
    			return (HTML_EVENT_STATUS_CONTINUE);
    		}
    		break;
	        case HTML_EVENT_TIMER:
				StartMainApplication(Ctx->hbrowser, Ctx->hdoc);
	        	break;
	        case HTML_EVENT_KEYDOWN:
				printf("Keydown in Body\n");
         	default:
         	break;
		}
    }
    return (HTML_EVENT_STATUS_CONTINUE);
}


</pre>

 @param
 @return 0 on success -1 on error.
 */
/*****************************************************************************/
int webc_BrowserSetTagEventHandler (
		HBROWSER_HANDLE wc,
		HTMLTagType tagType,
		HTMLEventCallback cb
	)
{
  #if (WEBC_SUPPORT_USER_EVENTS)
	CFunctionHandler* eventHandler;

	WEBC_NEW_VERBOSE(eventHandler, CFunctionHandler(0, cb), "CFunctionHandler");
	if (eventHandler)
	{
		if (HBROWSER_OBJ(wc)->SetTagEventHandler(tagType, eventHandler) >= 0)
		{
			return (0);
		}

		WEBC_DELETE(eventHandler);
	}
  #endif // WEBC_SUPPORT_USER_EVENTS

	return (-1);
}
#endif // HEREHERE