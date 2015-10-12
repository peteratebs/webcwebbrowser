//
// SpectrometrDemo.cpp - WebC GUI Example
//
// EBS - WebC
//
// Copyright EBS Inc.
//
// Module description:
//
//  Implements 'C' code needed for the startup and utility routines of the Spectrometer Demo.
//
// Startup code in this module attaches one application specific handler to to the HTML body tag.
// With this application handler in place the C application can control all aspect of the UI.
//
// SpectrometerDemoStart()  -
//  Start the application. Called after the GUI runtime is initialized.
//    Establishes a universal body tag callback handler
//    Load the start page for the application.
//
//  SpectrometerDemoBodycb() -
//	  Registered by the start code so we can intercept all page load messages.
//	  Retrieves the HTML id property if a page was just loaded.
//	  Calls other 'C' based onLoad handlers based on the id.
//
// Calls webC api functions:
// webc_BrowserSetTagEventHandler,webc_BrowserLoadUrl,webc_ElementIdMatches,webc_ElementIdMatches

#include "htmlsdk.h"
#include "rtpstr.h"
#include "rtpprint.h"
#include "rtpmem.h"

// Application content.
// Using the file: url for prototyping
// #define MAINAPPLICATION (char *)"file://\\demo_pages\\MainWindow.html"
#define MAINAPPLICATION (char *)"file://\\demo_pages\\UberWindow.html"

// Using the webc: (rom) url for production
// #define MAINAPPLICATION (char *)"webc://\\demo_pages\\MainWindow.html" (rom based version)

static HTMLEventStatus SpectrometerDemoBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
static HTMLEventStatus SpectrometerDemoImagecb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);

int SpectrometerDemoStart(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{	// Attach a custom handler to the <BODY> tag
	webc_BrowserSetTagEventHandler (hbrowser,HTML_TAG_BODY,SpectrometerDemoBodycb);
	webc_BrowserSetTagEventHandler (hbrowser,HTML_TAG_IMG,SpectrometerDemoImagecb);
	// Load the main application Window
	webc_BrowserLoadUrl(hbrowser, MAINAPPLICATION);
	return 0;
}

HTMLEventStatus MainWindowBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
HTMLEventStatus NewElementBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
HTMLEventStatus CalibrateBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
HTMLEventStatus NewClientBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
HTMLEventStatus ScreenSaverBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
HTMLEventStatus SetUpBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
HTMLEventStatus SlideShowViewerBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);

void AnimateImageEvent(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEventType eventType);

// Html body event callback.
// This handler is called every time an event is passed to the <BODY> tag of any html document.
// In our application we check if the id attribute of the body matches one of the pages we assign
// special 'C' code to, and if so, we call the handler.

static HTMLEventStatus SpectrometerDemoBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
	if (webc_ElementIdMatches(helem, "MainWindow"))
		return (MainWindowBodycb(hbrowser, hdoc, helem, event, param));
	else if (webc_ElementIdMatches(helem, "NewElement"))
		return (NewElementBodycb(hbrowser, hdoc, helem, event, param));
	else if (webc_ElementIdMatches(helem, "Calibrate"))
		return (CalibrateBodycb(hbrowser, hdoc, helem, event, param));
	else if (webc_ElementIdMatches(helem, "NewClient"))
		return (NewClientBodycb(hbrowser, hdoc, helem, event, param));
	else if (webc_ElementIdMatches(helem, "SetUp"))
		return (SetUpBodycb(hbrowser, hdoc, helem, event, param));
	else if (webc_ElementIdMatches(helem, "ScreenSaver"))
		return(ScreenSaverBodycb(hbrowser, hdoc, helem, event, param));
	else if (webc_ElementIdMatches(helem, "SlideShow"))
		return(SlideShowViewerBodycb(hbrowser, hdoc, helem, event, param));
	else
	    return (HTML_EVENT_STATUS_CONTINUE);
}

static HTMLEventStatus SpectrometerDemoImagecb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
char NewImage[256];
	switch (event->type) {
	/// Mouse button pressed
	case HTML_EVENT_CLICK:
		break;
	case HTML_EVENT_MOUSEOUT:
		break;
	case HTML_EVENT_MOUSEOVER:
		break;
	case HTML_EVENT_MOUSEDOWN:
		break;
	case HTML_EVENT_MOUSEUP:
		break;
	default:
		return (HTML_EVENT_STATUS_CONTINUE);
	}
	// Animate rollover
	AnimateImageEvent(hbrowser, hdoc, helem, event->type);

	return (HTML_EVENT_STATUS_CONTINUE);
}

void ResetDemoMode(void);
void PauseSlideShow(void);

void AnimateImageEvent(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEventType eventType)
{
char NewImage[256];


	char *Name = webc_ElementGetNameASCII(helem);

	if (!Name)
		return;


	switch (eventType) {
	/// Mouse button pressed
	case HTML_EVENT_CLICK:
		ResetDemoMode();
		PauseSlideShow();
		rtp_sprintf(NewImage, "images\\%sClick.png", Name);
		break;
	case HTML_EVENT_MOUSEOUT:
		rtp_sprintf(NewImage, "images\\%s.png", Name);
		break;
	case HTML_EVENT_MOUSEOVER:
		rtp_sprintf(NewImage, "images\\%sHover.png", Name);
		break;
	case HTML_EVENT_MOUSEDOWN:
	case HTML_EVENT_MOUSEUP:
	default:
		webc_FreeASCIIString(Name);
		return ;
	}
	webc_ElementSetAttributeASCII(helem, "src", NewImage);
	webc_FreeASCIIString(Name);

}
