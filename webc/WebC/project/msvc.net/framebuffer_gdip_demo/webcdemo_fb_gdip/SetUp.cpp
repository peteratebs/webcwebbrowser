//
// SetUp.cpp - WebC GUI Example
//
//
//  C code needed for the SetUp page of the Spectrometer Demo.
//
//  Implements a fictitious setup dialogue box.
//		Selects between two separate modes of operation and pops up dialogues specific
//		to the mode selects.
//
//		Demonstrates using html checkbox,radio button and button controls with C to create dynamic
//		screen.
//
// 		Also demonstrates changing sections of the screen view dynamically from 'C' code using the
//		API function webc_ElementSetInnerHtmlASCII.
//
// Public functions:
//
// SetUpBodycb()
//  Called by SpectrometerDemoBodycb() when the document is loaded
//  	Assigns event handlers to several controls in the source code.
//  	Uses webc_ElementSetInnerHtmlASCII to initialize a status area on the screen.
//
// Internal functions:
//
//  SetUpMapper() -  C event handler assigned to the image.
//     	SelectAbsorptionHandler	-  Switch visible controls to "absorbtion" dialog.
//     	BackToMethodHandler		-  Switch visible controls to front page.
//     	SelectEmissionHandler	-  Switch visible controls to "emission" dialog.
//     	SetUpUpdateStatusField	-  Update a table cell's inner html from 'C'.
//  	SetUpAcceptHandler()	-  C event handler assigned to the accept button.
//  	SetUpCancelHandler()	-  C event handler assigned to the cancel button.
//
// Calls webC api functions:
// 	webc_DocFindElement,webc_ElementSetTagEventHandler,webc_ElementSetStyleASCII, webc_ElementSetInnerHtmlASCII

#include "htmlsdk.h"
#include "rtpstr.h"

void MainWindowHome(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);

static void SetSetUpHandlers(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);
static HTMLEventStatus SetUpAcceptHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
static HTMLEventStatus SetUpCancelHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
static HTMLEventStatus SelectAbsorptionHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
static HTMLEventStatus SelectEmissionHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
static HTMLEventStatus BackToMethodHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
static void SetUpUpdateStatusField(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, char *status);

HTMLEventStatus SetUpBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
    switch (event->type)
    {
    case HTML_EVENT_LOAD:
         SetSetUpHandlers(hbrowser, hdoc, helem);
 		 SetUpUpdateStatusField(hbrowser,hdoc,"New Setup - Select Operating Mode.");
         break;
    case HTML_EVENT_UNLOAD:
            break;
    default:
            break;
    }
    return (HTML_EVENT_STATUS_CONTINUE);
}

static void SetSetUpHandlers(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{
	HELEMENT_HANDLE h;
	h = webc_DocFindElement (hdoc, "SelectAbsorption", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetTagEventHandler (h, SelectAbsorptionHandler);
	h = webc_DocFindElement (hdoc, "SelectEmission", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetTagEventHandler (h, SelectEmissionHandler);
	h = webc_DocFindElement (hdoc, "BackToMethodFromAbsorption", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetTagEventHandler (h, BackToMethodHandler);
	h = webc_DocFindElement (hdoc, "BackToMethodFromEmission", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetTagEventHandler (h, BackToMethodHandler);
	h = webc_DocFindElement (hdoc, "AcceptNewSetUp", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetTagEventHandler (h, SetUpAcceptHandler);
	h = webc_DocFindElement (hdoc, "CancelNewSetup", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetTagEventHandler (h, SetUpCancelHandler);

}

static HTMLEventStatus SelectAbsorptionHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
	HELEMENT_HANDLE h;
    if (event->type == HTML_EVENT_CLICK)
	{
		h = webc_DocFindElement (hdoc, "SelectAbsorbtionOrEmission", 0, HTML_ELEMENT_ANY, 0);
		if (h)	webc_ElementSetStyleASCII(h, "visibility:hidden;", true);
		h = webc_DocFindElement (hdoc, "SetUpAbsorbtion", 0, HTML_ELEMENT_ANY, 0);
		if (h)	webc_ElementSetStyleASCII(h, "visibility:visible;", true);
		SetUpUpdateStatusField(hbrowser,hdoc,"Absorption Mode Selected - Set absortion settings.");
	}
	return (HTML_EVENT_STATUS_CONTINUE);

}

static HTMLEventStatus BackToMethodHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
	HELEMENT_HANDLE h;
    if (event->type == HTML_EVENT_CLICK)
	{
		h = webc_DocFindElement (hdoc, "SelectAbsorbtionOrEmission", 0, HTML_ELEMENT_ANY, 0);
		if (h)	webc_ElementSetStyleASCII(h, "visibility:visible;", true);
		h = webc_DocFindElement (hdoc, "SetUpAbsorbtion", 0, HTML_ELEMENT_ANY, 0);
		if (h)	webc_ElementSetStyleASCII(h, "visibility:hidden;", true);
		h = webc_DocFindElement (hdoc, "SetUpEmission", 0, HTML_ELEMENT_ANY, 0);
		if (h)	webc_ElementSetStyleASCII(h, "visibility:hidden;", true);
		SetUpUpdateStatusField(hbrowser,hdoc,"Select Emission or Absorbtion Mode.");
	}
	return (HTML_EVENT_STATUS_CONTINUE);

}
static HTMLEventStatus SelectEmissionHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
HELEMENT_HANDLE h;
    if (event->type == HTML_EVENT_CLICK)
	{
		h = webc_DocFindElement (hdoc, "SelectAbsorbtionOrEmission", 0, HTML_ELEMENT_ANY, 0);
		if (h)	webc_ElementSetStyleASCII(h, "visibility:hidden;", true);
		h = webc_DocFindElement (hdoc, "SetUpEmission", 0, HTML_ELEMENT_ANY, 0);
		if (h)	webc_ElementSetStyleASCII(h, "visibility:visible;", true);
		SetUpUpdateStatusField(hbrowser,hdoc,"Emission Mode Selected - Set emission settings.");
	}
	return (HTML_EVENT_STATUS_CONTINUE);

}

static void SetUpUpdateStatusField(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, char *status)
{
	HELEMENT_HANDLE h;
	h = webc_DocFindElement (hdoc, "CurrentSetUpField", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetInnerHtmlASCII (h,status);
}

static HTMLEventStatus SetUpAcceptHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
    if (event->type == HTML_EVENT_CLICK)
	{
		MainWindowHome(hbrowser, hdoc);
		return (HTML_EVENT_STATUS_DONE);
	 }
	return (HTML_EVENT_STATUS_CONTINUE);

}
static HTMLEventStatus SetUpCancelHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
    if (event->type == HTML_EVENT_CLICK)
	{
		MainWindowHome(hbrowser, hdoc);
		return (HTML_EVENT_STATUS_DONE);
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}


