#ifndef WINDOWSGDI		/* So we can build GDI and Framebuffer from the same project */
#include "webc.h"
#include "webcmem.h"
#include "FBConfig.h"
#include "FBAbstraction.h"
#include "rtptime.h"
#include "rtpprint.h"

void wcProcessTimedCallbacks(void);

extern void * grabbedFocusedWindow;//current window to which events are sent
void * currentActivatedWindow;//current window which is active


static HBROWSER_HANDLE _FindBrowserByTarget(void *target);


/**
*  int webc_MessageReceive(HTMLEvent* e) 
*	This method's job is to receive messages from a target specific driver and pass them to webC.
*	
*   return 0 if the event was handled.
*	returns -1 if the event was not handled. If the caller has a default handler call it.
*/

/* Browser timout handller, called from an external timer handle */
extern "C" { void doFbBrowserTimeOut(unsigned long lBrowser); }


int webc_MessageReceive(HTMLEvent* e) 
{
	void *target;
	HTMLEventStatus r;
	HBROWSER_HANDLE which_Browser = 0;
	if (e->type == HTML_EVENT_TIMER)
	{
		wcProcessTimedCallbacks();
		doFbBrowserTimeOut((unsigned long )which_Browser);
		return 0;
	}
	target = (void *)e->target;
	which_Browser = _FindBrowserByTarget(target);
	if (!which_Browser)
		return -1;
  	webc_BrowserSuspendDraw(which_Browser);

	r = webc_BrowserDispatchEvent(which_Browser, e);
	/* Search for the browser again in case it was destroyed by the event handler */
	which_Browser = _FindBrowserByTarget(target);
	if (!which_Browser)
       	return 0;
	if (webc_BrowserGetDrawState (which_Browser) > 0)
    		webc_BrowserResumeDraw(which_Browser);
	webc_BrowserDraw(which_Browser);

	which_Browser = _FindBrowserByTarget(target);
	if (!which_Browser)
       	return 0;
 
	webc_BrowserSuspendDraw(which_Browser); // new
    webc_BrowserProcessLoadQueueAll(which_Browser);
    if (webc_BrowserGetDrawState (which_Browser) > 0) // new
   		webc_BrowserResumeDraw(which_Browser);
	which_Browser = _FindBrowserByTarget(target);
	if (!which_Browser)
		return 0;
	webc_BrowserDraw(which_Browser);
    return 0;
}


/**
*	This method must be called when the 100 millisecond periodic timer expires to format a timer event
*
*   Params: type - the type of event, x - x coor of the mouse, y - y coor of the mouse, e - the event to be
*           populated.
*/
int webc_timer_event(HTMLEvent* e)
{
	e->type = HTML_EVENT_TIMER;
	return 1;
}

/**
*	This method must be called to format an event structure when the driver receives a mouse button event
*
*   Params: type - the type of event, x - x coor of the mouse, y - y coor of the mouse, e - the event to be
*           populated.
*/
int mouse_button_event(int type, DISPLAY_INT x, DISPLAY_INT y, HTMLEvent* e)
{
	void* temporaryWindow;
	e->data.position.x = x;
	e->data.position.y = y;

	if(grabbedFocusedWindow){
		e->target = (WEBC_UINT8*)grabbedFocusedWindow;
		FBWM_setPointerCoordsToFocusWindow(grabbedFocusedWindow, e);
	}
    else{
		e->target = (WEBC_UINT8*)FBWM_getReceivingWindow(e);
	}

	switch(type){

		case RBUTTON_DOWN:
		case LBUTTON_DOWN:
			e->type = HTML_EVENT_MOUSEDOWN;
			//It may be a good idea to move the following code to move a window to the top to the
			//messageProcess method under Mouse_Down. Moving this block may facilitate better separation of
			//concerns.
			temporaryWindow = currentActivatedWindow;
			currentActivatedWindow = (void*)e->target;
			if(currentActivatedWindow != NULL && temporaryWindow != currentActivatedWindow){//determine if a new window is now the focus
				//recalculation of window z order
				; //FBWM_moveWindowToTop(currentActivatedWindow, true);
			}
			break;
		case RBUTTON_UP:
		case LBUTTON_UP:
			e->type = HTML_EVENT_MOUSEUP;

			break;
		default:
			break;
	}
	return(1);
}

/**
*	This method must be called to format an event structure when the driver receives a mouse move event
*	Params: x - new x coordinate of mouse, y - new y coordinate of mouse, e - HTMLEvent to populate with information
*/
int mouse_move_event(DISPLAY_INT x, DISPLAY_INT y, HTMLEvent* e){

	e->data.position.x = x;
	e->data.position.y = y;
	e->type = HTML_EVENT_MOUSEMOVE;

	if(grabbedFocusedWindow){
		e->target = (WEBC_UINT8*)grabbedFocusedWindow;
	}
	else {
		e->target = (WEBC_UINT8*)currentActivatedWindow;
	}
	return(1);
}

/**
*	This method must be called to format an event structure when a key board event is received.
*
*	Params: type - the type of key press(up or down), keyPressed - value of the key which can be accepted by
*		    webc_TranslateFBKey, e - HTMLEvent to be populated
*/
int keyboard_event(int type, WebKey keyPressed, HTMLEvent* e){

	if (type==KEY_UP)
		e->type= HTML_EVENT_KEYUP;
	else 
		e->type= HTML_EVENT_KEYDOWN;
	e->data.key = keyPressed;

	if(grabbedFocusedWindow){
		e->target = (WEBC_UINT8*)grabbedFocusedWindow;
	}
	else{
		e->target = (WEBC_UINT8*)currentActivatedWindow;
	}
	return(1);
}



static HBROWSER_HANDLE _FindBrowserByTarget(void *target)
{
	return FBWM_FindBrowserByWindow(target);
}


#endif // WINDOWSGDI
