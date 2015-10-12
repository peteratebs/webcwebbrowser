//
// NewClient.cpp - WebC GUI Example
//
//
//  C code needed for the NewClient page of the Spectrometer Demo.
//
// Public functions:
//
// NewClientBodycb()
//  Called by SpectrometerDemoBodycb() when the document is loaded
//  	Assigns event handlers to buttons in the html source with ids equal to AcceptClient and CancelClient.
//  	Assigns event handlers to the text control with id "NewClientName".
//  	Calls SetSoftKeyPadElementHandler to initialize the soft keyboard.
//  	Sets the focus to the text control.
//
// Internal functions:
//
//  NewClientTextControlHandler() -  C event handler assigned to the text control.
//		Initializes the control's value when the object is created.
//		Pops up a soft keyboard when it recieves focus.
//		Closes the soft keyboard when enter is pressed.
//
//  NewClientAcceptHandler()	 -  C event handler assigned to the accept button.
//  NewClientCancelHandler()	 -  C event handler assigned to the cancel button.
//
// Calls webC api functions:
// 	webc_ElementFocus, webc_DocFindElement,webc_ElementSetTagEventHandler,webc_ElementSetValueASCII,
//  webc_ElementGetValueASCII,webc_ElementGetParent,webc_FreeASCIIString


#include "htmlsdk.h"
#include "rtpstr.h"

int SetSoftKeyPadElementHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);
int SetSoftKeyPadVisible(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc,int xorigin, int yorigin);
void SetSoftKeyPadInvisible(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);
void SetSoftKeyTargetElement(HELEMENT_HANDLE h);
void MainWindowHome(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);
static void NewClientSaveName(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);
static void NewClientLoadName(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);
static void SetNewClientHandlers(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);
void SetSoftKeyPadTypeFace(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, int KeyPadNumber);

static char ClientName[80];
static int UpdateDemoMode(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);
static void RearmTimer(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, char *data);
static void StartDemo(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);

extern int DemoModeEnabled;
void AnimateImageEvent(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEventType eventType);

HTMLEventStatus NewClientBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
    switch (event->type)
    {
    case HTML_EVENT_LOAD:
	{	 // See below
		 SetNewClientHandlers(hbrowser, hdoc, helem);
		 // Set the focus to NewClientName. The onFocus handler for NewClientName will pop up a keyboard.
	 	webc_ElementFocus(webc_DocFindElement (hdoc, "NewClientName", 0, HTML_ELEMENT_ANY, 0), true);
	 	if (DemoModeEnabled)
			StartDemo(hbrowser, hdoc, helem);
	}
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

static HTMLEventStatus NewClientTextControlHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
static HTMLEventStatus NewClientAcceptHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
static HTMLEventStatus NewClientCancelHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
static void SetNewClientHandlers(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{
	HELEMENT_HANDLE h;
	// Set up the keypad.
	SetSoftKeyPadElementHandler(hbrowser, hdoc);
	// Attach event handler to elements by ID.
	h = webc_DocFindElement (hdoc, "NewClientName", 0, HTML_ELEMENT_ANY, 0);
	if (h)	webc_ElementSetTagEventHandler (h, NewClientTextControlHandler);
	h = webc_DocFindElement (hdoc, "AcceptClient", 0, HTML_ELEMENT_ANY, 0);
	if (h)	webc_ElementSetTagEventHandler (h, NewClientAcceptHandler);
	h = webc_DocFindElement (hdoc, "CancelClient", 0, HTML_ELEMENT_ANY, 0);
	if (h)	webc_ElementSetTagEventHandler (h, NewClientCancelHandler);
}


static HTMLEventStatus NewClientTextControlHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
	HELEMENT_HANDLE h = webc_DocFindElement (hdoc, "NewClientName", 0, HTML_ELEMENT_ANY, 0);

    if (event->type == HTML_EVENT_OBJECT_CREATED)
	{ // Set the field value when it is created so it displays correctly.
		HELEMENT_HANDLE h = webc_DocFindElement (hdoc, "NewClientName", 0, HTML_ELEMENT_ANY, 0);
		if (!ClientName[0])
		{
			rtp_strcpy(ClientName,"John Doe");
		}
		webc_ElementSetValueASCII(h,ClientName,true);
	}
    else if (event->type == HTML_EVENT_FOCUS)
	{ // Pop up the keyboard
		SetSoftKeyPadVisible(hbrowser, hdoc, 4, 80);
		SetSoftKeyTargetElement(h);
	 }
	 else if (event->type == HTML_EVENT_KEYDOWN)
	 {
		if (event->data.key == WGK_CR)
		{
			// CR pressed, give up the focus and hide the keyboard
			if (webc_ElementGetParent(h))
            	webc_ElementFocus(webc_ElementGetParent(h), WEBC_TRUE);
			SetSoftKeyPadInvisible(hbrowser, hdoc);
    		SetSoftKeyTargetElement(0);
    		return (HTML_EVENT_STATUS_DONE);
		}
	 }
	return (HTML_EVENT_STATUS_CONTINUE);
}


static HTMLEventStatus NewClientAcceptHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
    if (event->type == HTML_EVENT_CLICK)
	{
		HELEMENT_HANDLE h = webc_DocFindElement (hdoc, "NewClientName", 0, HTML_ELEMENT_ANY, 0);
		char *s = webc_ElementGetValueASCII(h);
		if(s)
		{
			rtp_strcpy(ClientName,s);
			webc_FreeASCIIString(s);
		}
		MainWindowHome(hbrowser, hdoc);
		return (HTML_EVENT_STATUS_DONE);
	 }
	return (HTML_EVENT_STATUS_CONTINUE);

}
static HTMLEventStatus NewClientCancelHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
    if (event->type == HTML_EVENT_CLICK)
	{
		MainWindowHome(hbrowser, hdoc);
		return (HTML_EVENT_STATUS_DONE);
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}




static int DemoStepCountDown = 0;
static char *ButtonNames[] = {	"Accept","Cancel",0};
static int CurrentButton = 0;
static int CurrentButtonOver = 0;
#define DEMOCHANGETOOVER 2
#define DEMOCHANGEFROMOVER 8
#define DEMOCHANGETOMAIN 100
#define SWIYCHKB1 1
#define SWIYCHKB2 80
static int DemoUrlCountDown = 0;
static int DemoStepKeyPad = 0;
static int CurrentKb = 0;


static void StartDemo(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{
	DemoStepCountDown = 0;
	CurrentButton = 0;
	CurrentButtonOver = 0;
	DemoUrlCountDown = 0;
	DemoStepKeyPad = 0;
	CurrentKb = 0;
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
	DemoStepKeyPad++;
	if (DemoStepKeyPad > SWIYCHKB2 && CurrentKb != 2)
	{
		CurrentKb = 2;
		SetSoftKeyPadTypeFace(hbrowser, hdoc, 2);
		DemoStepKeyPad = 0;
	}
	else if (DemoStepKeyPad > SWIYCHKB1 && CurrentKb != 1)
	{
		CurrentKb = 1;
		SetSoftKeyPadTypeFace(hbrowser, hdoc, 1);
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
