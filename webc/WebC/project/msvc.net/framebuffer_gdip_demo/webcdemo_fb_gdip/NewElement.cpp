//
// NewElement.cpp - WebC GUI Example
//
//
//  C code needed for the NewElement page of the Spectrometer Demo.
//
// Public functions:
//
// NewElementcb()
//  Called by SpectrometerDemoBodycb() when the document is loaded
//  	Assigns event handlers to buttons in the html source with ids equal to AcceptElement and CancelElement.
//  	Assigns event handler to the img with id "PeriodicTableImage".
//
// Internal functions:
//
//  NewElementMapper() -  C event handler assigned to the image.
//		Implements a "mouseover" event handler that maps chemical elements
//      by the position of the pointer over the periodic table.
//
//  NewElementAcceptHandler()	 -  C event handler assigned to the accept button.
//  NewElementCancelHandler()	 -  C event handler assigned to the cancel button.
//
// Calls webC api functions:
// 	webc_DocFindElement,webc_ElementSetTagEventHandler,webc_ElementSetValueASCII, webc_ElementGetValueASCII
//  webc_FreeASCIIString


#include "htmlsdk.h"
#include "rtpstr.h"

static void SetNewElementHandlers(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);
static HTMLEventStatus NewElementAcceptHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
static HTMLEventStatus NewElementCancelHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
void MainWindowHome(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);
static void NewElementSaveName(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);
static void NewElementLoadName(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);

static int UpdateDemoMode(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);
static void RearmTimer(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, char *data);
static void StartDemo(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);

extern int DemoModeEnabled;
void AnimateImageEvent(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEventType eventType);

// Set up event handlers and load the name into the text control (see below)
// Load the current element name into the text control (see below)
HTMLEventStatus NewElementBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
    switch (event->type)
    {
    case HTML_EVENT_LOAD:
         SetNewElementHandlers(hbrowser, hdoc, helem);
         NewElementLoadName(hbrowser, hdoc);
         if (DemoModeEnabled)
			StartDemo(hbrowser, hdoc, helem);
    break;
    case HTML_EVENT_TIMER:
	 	if (DemoModeEnabled)
			if (UpdateDemoMode(hbrowser, hdoc, helem)>=0)
				RearmTimer(hbrowser, helem, 0);
		break;
    case HTML_EVENT_UNLOAD:
            break;
    default:
            break;
    }
    return (HTML_EVENT_STATUS_CONTINUE);
}


static HTMLEventStatus NewElementMapper(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);

// Assign event handlers to the periodic table image and the cacnel and accept button.
static void SetNewElementHandlers(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{
	HELEMENT_HANDLE h = webc_DocFindElement (hdoc, "PeriodicTableImage", 0, HTML_ELEMENT_ANY, 0);
	webc_ElementSetTagEventHandler (h, NewElementMapper);
	h = webc_DocFindElement (hdoc, "AcceptNewElement", 0, HTML_ELEMENT_ANY, 0);
	webc_ElementSetTagEventHandler (h, NewElementAcceptHandler);
	h = webc_DocFindElement (hdoc, "CancelNewElement", 0, HTML_ELEMENT_ANY, 0);
	webc_ElementSetTagEventHandler (h, NewElementCancelHandler);

}

// Element "chooser" image map rollover control.
static char *NewElementPtrToElem(HTMLEvent* event);
static HTMLEventStatus NewElementMapper(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
	HELEMENT_HANDLE h;
	char *s;
    if (event->type == HTML_EVENT_MOUSEMOVE)
	{	// Map the pointer location to an element
		s = NewElementPtrToElem(event);
		if (s)
		{	// Update the status element
			webc_ElementSetValueASCII (webc_DocFindElement (hdoc, "CurrElementValue", 0, HTML_ELEMENT_ANY, 0), s, true);
		}
    }
    else if (event->type == HTML_EVENT_CLICK)
	{
		s = NewElementPtrToElem(event);
		if (s)
		{	// Update the selected element
			webc_ElementSetValueASCII (webc_DocFindElement (hdoc, "NewElementValue", 0, HTML_ELEMENT_ANY, 0), s, true);
		}
	}
	return (HTML_EVENT_STATUS_DONE);
}

// Simple mapping table to map the pointer location to a value in the periodic table.
static char *ElementNames[7][19] = {
{"H" ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,"He",},
{"Li","Be",0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,"B" ,"C" ,"N" ,"O" ,"F" ,"Ne",},
{"Na","Mg",0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0,"Al","Si","P","S","Cl", "Ar"},
{"K" ,"Ca",0   ,"Sc","Tl","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br", "Kr"},
{"Rb","Sr",0   ,"Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I", "Xe"},
{"Cs","Ba",0   ,"Lu","Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Ti","Pb","Bi","Po","Al", "Rn"},
{"Fr","Ra",0   ,"Lr","Rf","Ob","Sg","Bh","Hs","Mt","Uum","Uuu","Uub",0,"Uuq",0,0,0, 0},
};
static char *NewElementPtrToElem(HTMLEvent* event)
{
int x, y;
char *pelement = 0;
	// Map mouse offset to grid position.
	// 19,14,16 and 78 are hardcoded from the map.
	x = (event->data.position.x-19)/14;
	y = (event->data.position.y-78)/16;
	if (x >= 0 && x <= 18 && y >= 0 && y <= 6)
		pelement = ElementNames[y][x];
	return (pelement);
}

// Accept and cancel handlers.
static HTMLEventStatus NewElementAcceptHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
    if (event->type == HTML_EVENT_CLICK)
	{	// Get the value from the text control and keep a local copy.
		NewElementSaveName(hbrowser, hdoc);
		// Reload the main page.
		MainWindowHome(hbrowser, hdoc);
		return (HTML_EVENT_STATUS_DONE);
	 }
	return (HTML_EVENT_STATUS_CONTINUE);

}
static HTMLEventStatus NewElementCancelHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
    if (event->type == HTML_EVENT_CLICK)
	{
		MainWindowHome(hbrowser, hdoc);
		return (HTML_EVENT_STATUS_DONE);
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}

// Simulated application specifc database functions to store and retrieve current element.
static void MainSetElementName(char *Name);
static char *MainGetElementName(void);
static void NewElementSaveName(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{
	HELEMENT_HANDLE h = webc_DocFindElement (hdoc, "NewElementValue", 0, HTML_ELEMENT_ANY, 0);
	if (h)
	{
		char *s = MainGetElementName();
		s = webc_ElementGetValueASCII(h);
		if(s)
		{
			MainSetElementName(s);
			webc_FreeASCIIString(s);
		}
	}
}
static void NewElementLoadName(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{
	HELEMENT_HANDLE h = webc_DocFindElement (hdoc, "NewElementValue", 0, HTML_ELEMENT_ANY, 0);
	if (h)
	{
		char *s = MainGetElementName();
		webc_ElementSetValueASCII(h,s,true);
	}
}
static char ElementName[10];
static void MainSetElementName(char *Name)
{
	rtp_strcpy(ElementName, Name);
}
static char *MainGetElementName(void)
{
	if (!ElementName[0])
		MainSetElementName("H");
	return &ElementName[0];
}



static int DemoStepCountDown = 0;
static char *ButtonNames[] = {	"Accept","Cancel",0};
static int CurrentButton = 0;
static int CurrentButtonOver = 0;
#define DEMOCHANGETOOVER 2
#define DEMOCHANGEFROMOVER 8
#define DEMOCHANGETOMAIN 100

#define SWITCHELEMENT 10

static int DemoUrlCountDown = 0;
static int DemoStepElement = 0;
static int CurrentElementX = 0;
static int CurrentElementY = 0;


static void StartDemo(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{
	DemoStepCountDown = 0;
	CurrentButton = 0;
	CurrentButtonOver = 0;
	DemoUrlCountDown = 0;
	DemoStepElement = 0;
	CurrentElementX = 0;
	CurrentElementY = 0;
	RearmTimer(hbrowser, helem, 0);
}
#define DEMOPERIOD 200
static void RearmTimer(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, char *data)
{
HTMLEvent event;
	rtp_memset(&event, 0, sizeof(event));
	event.type = HTML_EVENT_TIMER;
//	event.data = data;
	event.elem = event.target = event.srcElement = helem;
	webc_BrowserQueueTimedEvent (hbrowser,helem,&event,DEMOPERIOD);
}




static int UpdateDemoMode(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{
	if (DemoModeEnabled == 0)
		return -1;
	DemoUrlCountDown++;
	if (DemoUrlCountDown >= DEMOCHANGETOMAIN)
	{
		DemoUrlCountDown = 0;
		MainWindowHome(hbrowser, hdoc);
		return -1;
	}
	DemoStepElement++;
	if (DemoStepElement > SWITCHELEMENT)
	{
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
			CurrentButton++;
			if (!ButtonNames[CurrentButton])
				CurrentButton = 0;
		}
		else
		{
			CurrentButtonOver = 1;
			AnimateImageEvent(hbrowser, hdoc, h, HTML_EVENT_MOUSEOVER);
		}
	}
	return 0;
}
