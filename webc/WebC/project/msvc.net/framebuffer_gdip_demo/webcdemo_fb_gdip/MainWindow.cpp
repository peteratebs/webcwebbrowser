//
// MainWindow.cpp - WebC GUI Example
//
// EBS - WebC
//
// Copyright EBS Inc.
//
// Module description:
//
//  Implements 'C' code for the MainWindow page of the Spectrometer Demo.
//
//  All menu capability is provided by HTML so this file does very little, but an
//  empty onLoad handler for the main page is is included for future enhancement.
//
// Public functions:
//  	MainWindowHome() - Called by C code in other modules to reload the main page.
//
// Internal functions:
//		None
//
// Calls webC api functions:
// 	webc_DocSetUrl

#include "htmlsdk.h"
#include "rtpstr.h"
#include "rtpprint.h"

static int UpdateDemoMode(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);
static void MainRearmTimer(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, char *data);
#define LOADHELPONDEMO 0

HTMLEventStatus MainWindowBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
    switch (event->type)
    {
    case HTML_EVENT_LOAD:
		MainRearmTimer(hbrowser, helem, 0);
    	break;
    case HTML_EVENT_UNLOAD:
            break;
    case HTML_EVENT_TIMER:
		if (UpdateDemoMode(hbrowser, hdoc, helem)>=0)
			MainRearmTimer(hbrowser, helem, 0);
        break;
    default:
        break;
    }
    return (HTML_EVENT_STATUS_CONTINUE);
}
void ScreenSaverHome(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{
HDOC_HANDLE rootDoc = webc_BrowserGetDocument(hbrowser);

	if (0 && rootDoc)
	{
		HELEMENT_HANDLE h = webc_DocFindElement (rootDoc, "Frame1", 0, HTML_ELEMENT_ANY, 0);

		if (h)
		{
		HDOC_HANDLE d = webc_ElementGetFrameDocument(h);

			if (d)
			{
				webc_DocSetUrl(d, "file://\\demo_pages\\SlideShowPlaceHolder.html", true);
			}
		}
    }
	webc_DocSetUrl(hdoc, "file://\\demo_pages\\MainWindow.html", true);
}

void MainWindowHome(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{
HDOC_HANDLE rootDoc = webc_BrowserGetDocument(hbrowser);

	if (rootDoc)
	{
		HELEMENT_HANDLE h = webc_DocFindElement (rootDoc, "Frame0", 0, HTML_ELEMENT_ANY, 0);

		if (h)
		{
		HDOC_HANDLE d = webc_ElementGetFrameDocument(h);

			if (d)
			{
				webc_DocSetUrl(d, "file://\\demo_pages\\MainWindow.html", true);
			}
		}
		h = webc_DocFindElement (rootDoc, "Frame2", 0, HTML_ELEMENT_ANY, 0);
		if (h)
		{
#if (LOADHELPONDEMO)
		HDOC_HANDLE d = webc_ElementGetFrameDocument(h);

			if (d)
			{
				webc_DocSetUrl(d, "file://\\demo_pages\\Narrative\\MainWindow.html", true);
			}
#endif
		}
    }
	webc_DocSetUrl(hdoc, "file://\\demo_pages\\MainWindow.html", true);
}


static void MainWindowChangeUrl(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, char *newurl)
{
HDOC_HANDLE rootDoc = webc_BrowserGetDocument(hbrowser);

	if (rootDoc)
	{
		HELEMENT_HANDLE h = webc_DocFindElement (rootDoc, "Frame0", 0, HTML_ELEMENT_ANY, 0);

		if (h)
		{
		HDOC_HANDLE d = webc_ElementGetFrameDocument(h);

			if (d)
			{
			char name[255];
				rtp_sprintf(name, "file://\\demo_pages\\%s",newurl);
				webc_DocSetUrl(d, name, true);
			}
		}
#if (LOADHELPONDEMO)
		h = webc_DocFindElement (rootDoc, "Frame2", 0, HTML_ELEMENT_ANY, 0);
		if (h)
		{
		HDOC_HANDLE d = webc_ElementGetFrameDocument(h);

			if (d)
			{
				char name[255];
				rtp_sprintf(name, "file://\\demo_pages\\Narrative\\%s",newurl);
				webc_DocSetUrl(d, name, true);

			}
		}
#endif
    }
}

#define DEMOPERIOD 200

void AnimateImageEvent(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEventType eventType);

int DemoModeEnabled = 0;
static int DemoModeCountDown = 0;
static int DemoStepCountDown = 0;
// static char *ButtonNames[] = {	"NewClient","Calibrate","NewElement","SysInfo","SetUp","Screensaver","Help","Quit",0};
#define NUMBUTTONS 8
static char *ButtonNames[] = {	"NewClient","Screensaver", "Help", "Calibrate","Quit","SysInfo","NewElement","SetUp",0};
int CurrentButton = 0;
int CurrentButtonOver = 0;
#define INITPAUSEDEMO 150 /* 30 second */
#define SUBSEQUANTPAUSEDEMO 40 /* 8 second */

int  DEMOSTART = INITPAUSEDEMO;

#define DEMOCHANGETOOVER 1
#define DEMOCHANGEFROMOVER 2


#define DEMOCHANGEFROMMAIN 32
// static char *UrlNames[] = {	"Calibrate.html","NewClient.html", "NewElement.html", "SlideShow.html",0};
static char *UrlNames[] = {	"Calibrate.html", "SlideShow.html",0};

int CurrentUrl = 0;
static int DemoUrlCountDown = 0;

#include "rtprand.h"

void ResetDemoMode(void)
{
	DemoModeEnabled   = 0;
	CurrentButtonOver = 0;
	DemoModeCountDown = 0;
	DemoStepCountDown = 0;
	CurrentUrl = 0;
	DemoUrlCountDown = 0;
	DEMOSTART = SUBSEQUANTPAUSEDEMO;

}
static void UpdateBatteryDisplay(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);

static int UpdateDemoMode(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{
	if (DemoModeEnabled == 0)
	{
		DemoModeCountDown++;
		if (DemoModeCountDown >= DEMOSTART)
		{
			ResetDemoMode();
			DemoModeEnabled = 1;
		}
	} 
	else
	{
		UpdateBatteryDisplay(hbrowser, hdoc, helem);

		DemoUrlCountDown++;
		if (DemoUrlCountDown >= DEMOCHANGEFROMMAIN)
		{
			MainWindowChangeUrl(hbrowser, hdoc, UrlNames[CurrentUrl++]);
			if (!UrlNames[CurrentUrl])
				CurrentUrl = 0;
			DemoUrlCountDown = 0;
			return -1;
		}
		DemoStepCountDown++;
		if (CurrentButtonOver==0)
		{
			if (DemoStepCountDown < DEMOCHANGETOOVER)
				return 0;
		}
		else if (DemoStepCountDown < DEMOCHANGEFROMOVER)
			return 0;
		DemoStepCountDown = 0;
		HELEMENT_HANDLE h = webc_DocFindElement (hdoc, 0,ButtonNames[CurrentButton],HTML_ELEMENT_ANY, 0);
		if (h)
		{
			if (CurrentButtonOver)
			{
				CurrentButtonOver = 0;
				AnimateImageEvent(hbrowser, hdoc, h, HTML_EVENT_MOUSEOUT);
				CurrentButton = rtp_rand() % NUMBUTTONS;
			}
			else
			{
				CurrentButtonOver = 1;
				AnimateImageEvent(hbrowser, hdoc, h, HTML_EVENT_MOUSEOVER);
			}
		}
	}
	return 0;
}
#define XDOBATTERY 1
#define MAXCHARGE 40
static void UpdateBatteryDisplay(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{
HELEMENT_HANDLE ImageElement;
static int countdown = 0;
static int charge = 2;
#define BatteryColor 0x00ffffff


	if (countdown++ < 8)
		return;
	countdown = 0;
#if (XDOBATTERY)
	ImageElement = webc_DocFindElement (hdoc, "Battery", 0, HTML_ELEMENT_ANY, 0);
	if (ImageElement)
	{
		webc_ImageGraphBeginDraw(ImageElement);
		webc_ImageGraphSetPenColor(ImageElement, BatteryColor);
		webc_ImageGraphFillRect(ImageElement, 5, 6, 5+charge, 18);
	// webc_ImageGraphDrawLine(ImageElement, x0, y0, x1, y1);
	// webc_ImageGraphDrawLine(ImageElement, x0+1, y0, x1+2, y1);
		webc_ImageGraphEndDraw(ImageElement);
		if (charge > MAXCHARGE)
		{
			charge = 2;
			webc_ElementSetAttributeASCII(ImageElement, "src", "images/BatteryBG.png");
			webc_ElementSetAttributeASCII(ImageElement, "src", "images/Battery.png");
		}
		else
			charge += 2;
	}
#endif
}

static void MainRearmTimer(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, char *data)
{
HTMLEvent event;

//return;
	rtp_memset(&event, 0, sizeof(event));
	event.type = HTML_EVENT_TIMER;
//	event.data = data;
	event.elem = event.target = event.srcElement = helem;
	webc_BrowserQueueTimedEvent (hbrowser,helem,&event,DEMOPERIOD);
}
