//
// AppBuilder.cpp - WebC Screen Builder
//
// EBS - WebC
//
// Copyright EBS Inc.
//
// Module description:
//
//  Implements 'C' code needed for the startup and utility routines of the Appbuilder.
//
// Startup code in this module attaches one application specific handler to to the HTML body tag.
// With this application handler in place the C application can control all aspect of the UI.
//
// AppBuilderStart()  -
//  Start the application. Called after the GUI runtime is initialized.
//    Establishes a universal body tag callback handler
//    Load the start page for the application.
//
//  AppBuilderStartBodycb() -
//	  Registered by the start code so we can intercept all page load messages.
//	  Retrieves the HTML id property if a page was just loaded.
//	  Calls other 'C' based onLoad handlers based on the id.
//
// Calls webC api functions:
// webc_BrowserSetTagEventHandler,webc_BrowserLoadUrl,webc_ElementIdMatches,webc_ElementIdMatches

#include "include/AppBuilder.h"
#include "rtpprint.h"

static HTMLEventStatus AppBuilderBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);




int AppBuilderStart(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{
	// Preload important Xml documents
	WebcAppGenXmlLLoadTemplates();
	// Attach a custom handler to the <BODY> tag
	webc_BrowserSetTagEventHandler (hbrowser,HTML_TAG_BODY, AppBuilderBodycb);
	// Load the main application Window
	webc_BrowserLoadUrl(hbrowser, MAINAPPLICATIONURL);
	return 0;
}

#include "wcApi/wcapi.h"
static HTMLEventStatus newAppBuilderBodycb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param);

int AppBuilderStartOriginal(HTMLBrowserConfig *config)
{
	wcCtx Ctx;

	config->WindowConfig.width = 800;
	config->WindowConfig.height= 700;
	config->WindowConfig.BackGroundColor = webc_GetDefaultColor(WEBC_DEFAULT_BACKGROUND_COLOR);

	// Preload important Xml documents
	WebcAppGenXmlLLoadTemplates();

	if (wcWinCreateEx(
	&Ctx,
	"Appbuilder", // char *_windowname
	newAppBuilderBodycb, // wcEventCallback BodyCallback
	MAINAPPLICATIONURL,
	0, // char *_defaultContent,
	&config->WindowConfig // struct HTMLBrowserWindowConfig *pWindowConfig
   ) < 0)
	return -1;
	// Attach a custom handler to the <BODY> tag
//	webc_BrowserSetTagEventHandler (hbrowser,HTML_TAG_BODY, AppBuilderBodycb);
	// Load the main application Window
//	webc_BrowserLoadUrl(hbrowser, MAINAPPLICATIONURL);
	return 0;
}

static HTMLEventStatus newAppBuilderBodycbTwo(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
	if (element)
	{
    	switch (event->type)
    	{
    		case HTML_EVENT_LOAD:
    		{
				/* Running thests now, queu up the test to run in 100 msec (after background was drawn */
    			HTMLEvent Event;
    			rtp_memset(&Event, 0 ,sizeof(Event));
    			Event.type = HTML_EVENT_TIMER;
    			wcTriggerEvent(element, &Event, 100);
    			return (HTML_EVENT_STATUS_CONTINUE);
    		}
    		break;
	        case HTML_EVENT_TIMER:
				printf("Fired off\n");
	        	break;
	        case HTML_EVENT_KEYDOWN:
				printf("Keydown in Body\n");
				rtp_debug_print_mem_usage (0,0,"Keydown");
         	default:
         	break;
		}
    }
    return (HTML_EVENT_STATUS_CONTINUE);
}
int AppBuilderStart(HTMLBrowserConfig *config)
{
	wcCtx Ctx;

	config->WindowConfig.width = WEBC_SCREEN_WIDTH;
	config->WindowConfig.height= WEBC_SCREEN_HEIGHT;
	config->WindowConfig.BackGroundColor = webc_GetDefaultColor(WEBC_DEFAULT_BACKGROUND_COLOR);


rtp_debug_print_mem_usage (0,0,"Starting");

	if (wcWinCreateEx(
	&Ctx,
	"Appbuilder", // char *_windowname
	newAppBuilderBodycbTwo, // wcEventCallback BodyCallback
	(char *)"file://\\AppBuilder\\SizeMe.html", // char *_defaultUrl, // char *_defaultUrl,
	0, // char *_defaultContent,
	&config->WindowConfig // struct HTMLBrowserWindowConfig *pWindowConfig
   ) < 0)
	return -1;
	// Attach a custom handler to the <BODY> tag
//	webc_BrowserSetTagEventHandler (hbrowser,HTML_TAG_BODY, AppBuilderBodycb);
	// Load the main application Window
//	webc_BrowserLoadUrl(hbrowser, MAINAPPLICATIONURL);
	config->BrowserHandle=webc_GetBrowserWithFocus();
	return 0;
}

void TestDomApi(wcBROW browser, wcDOC doc);

int TestSlidingWindow(HBROWSER_HANDLE hbrowser,	HDOC_HANDLE hdoc);



static HTMLEventStatus newAppBuilderBodycb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
	if (element && wcELIdMatches(element, COMPOSEWINDOWID))
	{
    	switch (event->type)
    	{
    		case HTML_EVENT_LOAD:
    		{
				/* Running thests now, queu up the test to run in 100 msec (after background was drawn */
    			HTMLEvent Event;
    			rtp_memset(&Event, 0 ,sizeof(Event));
    			Event.type = HTML_EVENT_TIMER;
    			wcTriggerEvent(element, &Event, 100);
    			return (HTML_EVENT_STATUS_CONTINUE);
    		}
    		break;
	        case HTML_EVENT_TIMER:
				TestDomApi(Ctx->hbrowser, Ctx->hdoc);
	        	break;
	        case HTML_EVENT_KEYDOWN:
				printf("Keydown in Body\n");
         	default:
         	break;
		}
    }
    return (HTML_EVENT_STATUS_CONTINUE);
}
static HTMLEventStatus AppBuilderBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
	if (webc_ElementIdMatches(helem, COMPOSEWINDOWID))
	{
    	switch (event->type)
    	{
    		case HTML_EVENT_LOAD:
    		{
				/* Running thests now, queu up the test to run in 100 msec (after background was drawn */
    			HTMLEvent Event;
    			rtp_memset(&Event, 0 ,sizeof(Event));
    			Event.type = HTML_EVENT_TIMER;
    			wcTriggerEvent(helem, &Event, 100);
    			return (HTML_EVENT_STATUS_CONTINUE);
    			ConstructwebCPalleteObject(hbrowser, hdoc);
    			ConstructwebCStoryBoard(hdoc);
    			LoadStoryBoardFromSreenDocument(hbrowser, hdoc, TestProject);
				ConstructEventDialogue(hdoc);
    		}
    		break;
	        case HTML_EVENT_TIMER:
	        	TestDomApi((wcBROW) hbrowser, (wcDOC) hdoc);
	        	break;
	        case HTML_EVENT_KEYDOWN:
				printf("Keydown in Body\n");
         	default:
         	break;
		}
    }
    return (HTML_EVENT_STATUS_CONTINUE);
}



int flowcallcount;
int callcount;
