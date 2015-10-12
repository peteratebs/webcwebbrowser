//
// ScreenSaver.cpp - WebC GUI Example
//
//
//  C code needed for the ScreenSaver page of the Spectrometer Demo.
//
// Public functions:
//
// ScreenSaverBodycb()
//  Called by SpectrometerDemoBodycb() when the document is loaded
//  	Starts a timer chain that redraws the screen on a periodic interval.
//		Attaches an event handler to cancel the screen saver and wake up
//		when the user touches the console.
//		The current screen saver algorithm is weak. A more interest pattern would bwe nicer.
//
// Internal functions:
//
//	ScreenSaverBodycb 	 			- Initialize the screen saver Set up graphics and set up a timer to redraw screen periodically.
//	UpdateScreenSaver				- Redraws the screen.
//	ScreenSaverRearmTimer			- Period timer management.
//
//
// Calls webC api functions:
//  webc_DocFindElement,webc_ElementSetStyleASCII,webc_ImageGraphBeginDraw,
//	webc_ImageGraphSetPenColor,webc_ImageGraphFillRect,webc_ImageGraphEndDraw
//	webc_BrowserQueueTimedEvent

#include "htmlsdk.h"
#include "rtpstr.h"
#include "rtpprint.h"
#include "rtprand.h"

void MainWindowHome(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);
static void StartScreenSaver(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);
static void UpdateScreenSaver(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);

static int ScreenSaverPaused;

HTMLEventStatus ScreenSaverBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
	 HELEMENT_HANDLE h;
    switch (event->type)
    {
    case HTML_EVENT_LOAD:
		ScreenSaverPaused = 0;
		StartScreenSaver(hbrowser, hdoc, helem);
        break;
    case HTML_EVENT_UNLOAD:
        break;
    case HTML_EVENT_TIMER:
		UpdateScreenSaver(hbrowser, hdoc, helem);
        break;
    case HTML_EVENT_CLICK:
    	MainWindowHome(hbrowser, hdoc);
	    return (HTML_EVENT_STATUS_HALT);
    default:
            break;
    }
    return (HTML_EVENT_STATUS_CONTINUE);
}



static void ScreenSaverRearmTimer(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, char *data);
static void StartScreenSaver(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{
HELEMENT_HANDLE h;
	h = webc_DocFindElement (hdoc, "ScreenSaverCanvas", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetStyleASCII(h, "visibility:visible;", true);
     ScreenSaverRearmTimer(hbrowser, helem, 0);
}

static void UpdateScreenSaver(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE h)
{
HELEMENT_HANDLE	ImageElement;
HTMLColor WhiteColor = 0x000000ff;
HTMLColor BlackColor = 0x00000000;
int x,y,w,ht;
static int prevx,prevy,prevw,prevht;


	if (ScreenSaverPaused)
	{
		ScreenSaverRearmTimer(hbrowser, h, 0);
		return;
	}
	ImageElement = webc_DocFindElement (hdoc, "ScreenSaverCanvas", 0, HTML_ELEMENT_ANY, 0);
	webc_ImageGraphBeginDraw(ImageElement);
    if (prevw && prevht)
	{  // Undraw the previous recatngle
		webc_ImageGraphSetPenColor(ImageElement, BlackColor);
		webc_ImageGraphFillRect(ImageElement, prevx, prevy, prevw, prevht);
	}
	do
	{  // Get a new rectagle
		x = rtp_rand() % 310;
		y = rtp_rand() % 230;
		ht = rtp_rand() % (240-y);
		w = rtp_rand() % (320-y);
	} while (ht <= 1 || w <= 1);
	webc_ImageGraphSetPenColor(ImageElement, WhiteColor);
	webc_ImageGraphFillRect(ImageElement, x, y, w, ht);
    prevx = x; prevy = y; prevw = w; prevht = ht;
	webc_ImageGraphEndDraw(ImageElement);
	ScreenSaverRearmTimer(hbrowser, h, 0);
}


#define SCREENSAVERDATEPERIOD 800

static void ScreenSaverRearmTimer(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, char *data)
{
HTMLEvent event;
	rtp_memset(&event, 0, sizeof(event));
	event.type = HTML_EVENT_TIMER;
//	event.data = data;
	event.elem = event.target = event.srcElement = helem;
	webc_BrowserQueueTimedEvent (hbrowser,helem,&event,SCREENSAVERDATEPERIOD);
}
