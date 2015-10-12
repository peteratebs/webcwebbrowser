//
// SlideShowViewer.cpp - WebC GUI Example
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
#include <rtpstr.h>


void ScreenSaverHome(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);

static void StartSlideShowViewer(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);
static void UpdateSlideShowViewer(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);
static void UpdateSlideShowViewer(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE h);
static void SlideShowViewerRearmTimer(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, int period);
static int FindNextImage(char *name, char *fullpath);
static void CloseImageScan(void);
static void ToggleSlideShow(void);

static int SlideShowViewerRunning;
static int SlideShowPaused;
extern int DemoModeEnabled;
static int DemoCountdown;

#define SLIDESHOWROOTPATH "\\color_demo_pages\\SlideShowImages"
enum ViewerStates
{
	LOADING = 0,
	ANIMATEFOREWARD,
	ANIMATEBACKWARDS
};
static enum ViewerStates CurrentState = LOADING;

#define WIDTHINCREMENT	 32
#define HEIGHTINCREMENT	 24
#define OPACITYINCREMENT 1
#define OPACITY_ONLY 1
#define SLIDESHOWUPDATEPERIOD 100
#define SLIDESHOWSTILLPERIOD 1000


HTMLEventStatus SlideShowViewerBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
HELEMENT_HANDLE h;

    switch (event->type)
    {
    case HTML_EVENT_LOAD:
		SlideShowViewerRunning = 1;
		SlideShowPaused = 0;
		StartSlideShowViewer(hbrowser, hdoc, helem);
        break;
    case HTML_EVENT_UNLOAD:
        break;
    case HTML_EVENT_TIMER:
		if (!SlideShowViewerRunning)
		{
			ScreenSaverHome(hbrowser, hdoc);
		    return (HTML_EVENT_STATUS_HALT);
		}
		UpdateSlideShowViewer(hbrowser, hdoc, helem);
        break;
    case HTML_EVENT_CLICK:
		SlideShowViewerRunning = 0;
    	//ToggleSlideShow();
    default:
            break;
    }
    return (HTML_EVENT_STATUS_CONTINUE);
}


static void StartSlideShowViewer(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{
	 CurrentState = LOADING;
     SlideShowViewerRearmTimer(hbrowser, helem, SLIDESHOWUPDATEPERIOD);
}

void SetSlideShowImage(HELEMENT_HANDLE ImageElement, char *imagepath)
{
	webc_ElementSetSrcASCII(ImageElement, imagepath, true);
}
void UpdateSlideShowImage(HELEMENT_HANDLE ImageElement,HELEMENT_HANDLE ImageDiv, int left, int top, int width, int height, int opacity)
{
char stylebuffer[255];
	if ( OPACITY_ONLY)
	{
		width = 320;
		height = 240;
	}
	if (opacity == 10)
		rtp_sprintf(stylebuffer, "position:absolute;left:%dpx;top:%dpx;width:%dpx;height:%dpx;visibility:visible;",left, top, width, height);
	else if (opacity == 0)
		rtp_sprintf(stylebuffer, "position:absolute;left:%dpx;top:%dpx;width:%dpx;height:%dpx;visibility:hidden;",left, top, width, height);
	else
		rtp_sprintf(stylebuffer, "position:absolute;left:%dpx;top:%dpx;width:%dpx;height:%dpx;visibility:visible;opacity:.%d;",left, top, width, height, opacity);
	webc_ElementSetStyleASCII(ImageDiv, stylebuffer, true);
	webc_ElementSetStyleASCII(ImageElement, stylebuffer, true);
}


static void UpdateSlideShowViewer(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE h)
{
HELEMENT_HANDLE	ImageElement,ImageDiv;
int x,y,w,ht;
static int prevx,prevy,prevw,prevht,prevop;
char imagepath[255];
int rearmtime = SLIDESHOWUPDATEPERIOD;

	if (SlideShowPaused)
	{
		SlideShowViewerRearmTimer(hbrowser, h, rearmtime);
		return;
	}

	ImageDiv = webc_DocFindElement (hdoc, "SlideShowImageDiv", 0, HTML_ELEMENT_ANY, 0);
	ImageElement = webc_DocFindElement (hdoc, "SlideShowImage", 0, HTML_ELEMENT_ANY, 0);
	if (CurrentState == LOADING)
	{
		UpdateSlideShowImage(ImageElement,ImageDiv, 0, 0, 0, 0, 0);
		FindNextImage(SLIDESHOWROOTPATH, imagepath);
		SetSlideShowImage(ImageElement, imagepath);
		prevw  = WIDTHINCREMENT;
		prevht = HEIGHTINCREMENT;
		prevop = 0;
		UpdateSlideShowImage(ImageElement,ImageDiv, 0, 0, WIDTHINCREMENT, HEIGHTINCREMENT, 0);
		CurrentState = ANIMATEFOREWARD;
	}
	if (CurrentState == ANIMATEFOREWARD)
	{
		prevop += OPACITYINCREMENT;
		prevw  += WIDTHINCREMENT;
		prevht += HEIGHTINCREMENT;
		UpdateSlideShowImage(ImageElement,ImageDiv, 0, 0, prevw, prevht, prevop);
		if (prevop == 10)
		{
			rearmtime = SLIDESHOWSTILLPERIOD;
			CurrentState = ANIMATEBACKWARDS;
		}
	}
	if (CurrentState == ANIMATEBACKWARDS)
	{
		prevop -= OPACITYINCREMENT;
		if (prevw > WIDTHINCREMENT)
			prevw  -= WIDTHINCREMENT;
		if (prevht > HEIGHTINCREMENT)
			prevht -= HEIGHTINCREMENT;
		UpdateSlideShowImage(ImageElement,ImageDiv, 0, 0, prevw, prevht, prevop);
		if (prevop == 0)
		{
			CurrentState = LOADING;
			if (DemoModeEnabled)
				SlideShowViewerRunning = 0;
		}
	}

	SlideShowViewerRearmTimer(hbrowser, h, rearmtime);
}



static void SlideShowViewerRearmTimer(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, int period)
{
HTMLEvent event;
	rtp_memset(&event, 0, sizeof(event));
	event.type = HTML_EVENT_TIMER;
	event.elem = event.target = event.srcElement = helem;
	webc_BrowserQueueTimedEvent (hbrowser,helem,&event,period);
}



//=====

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <conio.h>
#include <dos.h>
#include <io.h>
#include <direct.h>

static intptr_t dd = -1;
static void CloseImageScan(void)
{
	if (dd != -1)
		_findclose(dd);
	dd = -1;
}

/* Return the next image in the patj, -1 on finished */
static int FindNextImage(char *name, char *fullpath)
{
struct _finddata_t statobj;


	for(;;)
	{
        if (dd < 0)
	    {
	    char path[256];
        	strcpy((char *)path, (char *)name);
        	strcat((char *)path, "\\*.*");
        	if ((dd = _findfirst((char *)path, &statobj)) < 0)
        	{
            	return(-1);
        	}
	    }
	    else if (_findnext(dd, &statobj) != 0)
	    {
         	_findclose(dd);
	    	dd = -1;
           	return(-1);
	    }
	    if (!(statobj.attrib & _A_SUBDIR ))
        {
           	if (	rtp_stristr(statobj.name, ".png")||rtp_stristr(statobj.name, ".gif")||
           			rtp_stristr(statobj.name, ".bmp")||rtp_stristr(statobj.name, ".jpg"))
	    	{
           		sprintf(fullpath, "%s\\%s",(char *)name,statobj.name);
	    		return(0);
	    	}
	    }
	}
}

void PauseSlideShow(void)
{
	SlideShowPaused = 1;
}
void ResumeSlideShow(void)
{
	SlideShowPaused = 0;
}

static void ToggleSlideShow(void)
{
	SlideShowPaused = SlideShowPaused?0:1;
}
