//
// wcapiexample.cpp -
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
#include "rtpmem.h"
#include "wcapi.h"
#include "rtpprint.h"
#include "rtpstr.h"
int webc_GuiInitConfig(HTMLBrowserConfig* config);
#include <math.h>
#if (WEBC_SUPPORT_JSCRIPT)
 #include "wjsdoc.h"
 #include "wjsbrowser.h"
 #include "wjscript.hpp"
#endif

static HTMLEventStatus webcApiExamplecb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
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
    			wcElTriggerEvent(element, &Event, 100);
    			return (HTML_EVENT_STATUS_CONTINUE);
    		}
    		break;
	        case HTML_EVENT_TIMER:
				rtp_printf("Fired off\n");
	        	break;
	        case HTML_EVENT_KEYDOWN:
				rtp_printf("Keydown in Body\n");
         	default:
         	break;
		}
    }
    return (HTML_EVENT_STATUS_CONTINUE);
}




#define EXAMPLEBORDERWIDTH 2
#define MAIN_CANVAS_HEIGHT (WEBC_SCREEN_HEIGHT - (2*EXAMPLEBORDERWIDTH))
#define MAIN_CANVAS_WIDTH (WEBC_SCREEN_WIDTH - (2*EXAMPLEBORDERWIDTH))
#define PROMPT_CONSOLE_HEIGHT 60
#define PROMPT_BORDERWIDTH 2
#define BUTTON_CONSOLE_HEIGHT 30
#define BUTTON_CONSOLE_WIDTH  60
#define CONSOLE_HEIGHT (BUTTON_CONSOLE_HEIGHT+(PROMPT_BORDERWIDTH*2)+PROMPT_CONSOLE_HEIGHT)
#define CONSOLE_WIDTH MAIN_CANVAS_WIDTH
#define CONSOLE_POS (MAIN_CANVAS_HEIGHT-CONSOLE_HEIGHT)
#define CHILD_WINDOW_HEIGHT (WEBC_SCREEN_HEIGHT - CONSOLE_HEIGHT - EXAMPLEBORDERWIDTH)
#define CHILD_CANVAS_HEIGHT (CHILD_WINDOW_HEIGHT - (2*EXAMPLEBORDERWIDTH))
#define BUTTON_ARRAY_YPOS (PROMPT_CONSOLE_HEIGHT+(PROMPT_BORDERWIDTH*2))							// In the div
#define BUTTON_ARRAY_HEIGHT BUTTON_CONSOLE_HEIGHT
#define BUTTON_STYLE "background-color:lime;color:maroon;"
#define CONSOLE_BACKGROUND_STYLE "background-color:aqua;border-style:solid;border-width:2px;border-color:olive;"
#define CONSOLE_TEXT_STYLE "background-color:black;color:aqua;border-style:solid;border-width:2px;border-color:olive;"
#define SPLASH_BACKGROUND_STYLE "background-color:gray;border-style:none;"
#define SPLASH_TEXT_STYLE "background-color:gray;color:black;"

#define EXBKIMAGE0 "webc:ebslogo.png"
#define EXTARGETIMAGE "webc:/Example/Images/Target.png"
#define BODYBKIMAGE0 "webc:/Example/Images/TestBackGround.jpg" // ExampleBackground.png"
#define FADEINIMAGE "webc:/Example/Images/MoonRover.jpg"
#define FLAMEIMAGE "webc:/Example/Images/Flame.jpg"
typedef struct s_B {char *name; int xpos; int command;} B;


enum MainWindowCommand {EXHOME,EXLEFT,EXRIGHT,EXSOURCE,EXHELP};

//B HomebuttonArray[]= { {"Home", 0, (int)EXHOME}, {"Left", 60, (int)EXLEFT},{"Right", 120, (int)EXRIGHT},{"Source", 180, (int)EXSOURCE},{"Help", 240, (int)EXHELP},{0, 240, 0}};
B HomebuttonArray[]= { {"-Next-", 120, (int)EXRIGHT},{0, 240, 0}};

wcCtx mainwindowCtx;
wcCtx childwindowCtx;
wcEL gConsoleTextObject;
wcEL gSplashDivObject;

#define SPLASH_HTML "<a href=\"file://index.html\">Click here to link to index.html in the current directory</a><br>\
<center>Press Next to cycle through the examples.</center><ul><li>Graphics - Resize Box</li><li>SlideShow - Drag and Drop</li>\
<li>Calculator - Event Handling</li><li>Soft Keypad - Using Rommed Data</li></ul></small>"
#define UPDATECONSOLETEXT(T) wcElSetValue(gConsoleTextObject,T)
static void ExDemonstrateWebcVirtualFiles(void);
static void ExDragButtons(void);
static void ExPresentationSlideShow(void);
static void ExCalculator(void);
static void ExCalculatorDynamic(void);
static void ExResizeBox(void);
static void ExSoftKeyPad(void);
static void ExSliders(void);
static void ExGraphics(void);
static void ExDocEventHandling(void);
static void ExElEventHandling(void);
static void ExControls(void);
#ifdef RTP_TRACK_LOCAL_MEMORY
static void ExMemDiags(void);
#endif
#if (WEBC_SUPPORT_JSCRIPT)
 static void ExJScript(void);
#endif
typedef void (*ExampleFn)(void);
typedef struct s_Example {
ExampleFn ExampleFunction;
char *Description;
} Example;

static Example Examples[] =
{
#if 0
#if (WEBC_SUPPORT_JSCRIPT)
	{ ExJScript, "Demonstrate Javascript and C working together"},
#endif

#ifdef RTP_TRACK_LOCAL_MEMORY
	{ ExMemDiags,"Demonstrate Memory Diagnostics."},
#endif
	{ ExControls,"Messing with controls."},
#endif
//	{ ExGraphics,"Line Graphics slideshow running."},
	{ ExSoftKeyPad,"HowTo use the built in soft keypad. Click in the textbox to pop open a keypad."},
	{ ExResizeBox,"Click on the frame to resize. Click and hold anywhere to move."},
	{ ExCalculator,"Simple functional calculator program in 40 lines of CSS and HTML and < 100 lines of C."},
	{ ExCalculatorDynamic,"Calculator program using fully dynamic html."},
	{ ExDragButtons,"Drag and drop - Drag buttons from the right side box and drop them on the target."},
	{ ExSoftKeyPad,"HowTo use the built in soft keypad. Click in the textbox to pop open a keypad."},
//	{ ExPresentationSlideShow,"Slideshow running. Some of the ways webC can display content."},
	{ ExElEventHandling,"HowTo use events with elements - look in the editbox to verify results. These tests demonstrate using the event API with elements. Other examples use events to do work."},
	// Note: There is a leak in this test, createwindow creates an event handler that is never freed. THat is because Document::RestoreDefaults stomps the user pointer.
	// Another pointer is probably needed, requires more investegation.
//	{ ExDocEventHandling,"HowTo use events with documents - look in the editbox to verify results. These tests demonstrate using the event API with elements. Other examples use events to do work."},
	{ ExSliders,"HowTo change default scrollbar appearance."},
	{ ExDemonstrateWebcVirtualFiles,"HowTo install precompiled files into webC's rom file system and load content."},

};
#define NUMEXAMPLES ( sizeof(Examples)/sizeof(Examples[0]))
static int currentExample;


static HTMLEventStatus MainButtonCb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
int initialExample = currentExample;
	if (element != 0 && event->type == HTML_EVENT_CLICK)
	{
		enum MainWindowCommand theCommand;
		theCommand =  (enum MainWindowCommand)  ((int)wcGetData(Ctx,  element));

		switch (theCommand)
		{
		case EXLEFT:
			if (--currentExample < 0)
				currentExample = NUMEXAMPLES -1;
			break;
		case EXRIGHT:
			if (++currentExample >= NUMEXAMPLES)
				currentExample =0;
			break;
		case EXSOURCE:
		case EXHELP:
		case EXHOME:
			return HTML_EVENT_STATUS_CONTINUE;
		break;
		}
	}
	if (initialExample != currentExample)
	{ //
		wcElSetValue(gConsoleTextObject,Examples[currentExample].Description);
		Examples[currentExample].ExampleFunction();
		return HTML_EVENT_STATUS_HALT; //CONTINUE;
	}
//	return HTML_EVENT_STATUS_DONE;
	return HTML_EVENT_STATUS_CONTINUE;
//	return HTML_EVENT_STATUS_HALT;
}
static void FormatMainWindow(void);
static int OpenExampleChildWindow(void);

void doFileTest(void);
// Include compiled in content we will register with the webc: file system
#include "ExampleContentfiletable.h"
// =======
#if (INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS)
static void FormatDivWindow(void)
{
	char styletext[256];
	wcEL DivObject;

	DivObject = wcElNew(&mainwindowCtx, "div");
	rtp_sprintf(styletext, "position:absolute;top:%dpx;left:%dpx;height:%dpx;width:%dpx;%s",20,40,100,200,"border-style:solid;border-color:black;border-width:4;background-color:lightblue;overflow:auto;");
//	rtp_sprintf(styletext, "position:absolute;left:%dpx;top:%dpx;width:%dpx;%s",0,20,200,"border-style:solid;background-color:lightblue;overflow:auto;");
	wcElSetStyle(DivObject,styletext,wcFALSE);
	wcDocInsertLast(&mainwindowCtx, DivObject);
	wcElSetInnerHtml(DivObject,
		"1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ\
	<br>1<br>2<br>3<br>4<br>5<br>6<br>1<br>2<br>3<br>4<br>5<br><br>1<br>2<br>3<br>4<br>5<br>");
	 wcDocRefresh(&mainwindowCtx);

}
#endif
// =======
void webcApiExampleRun(void)
{
	struct HTMLBrowserWindowConfig WindowConfig;

	currentExample = -1; // Initialize to illegal current example

	// Create a main Window in the foreground that we'll use for selecting tests to run.
	rtp_memset(&WindowConfig, 0, sizeof(WindowConfig));
	WindowConfig.top   = 0;
	WindowConfig.left  = 0;
	WindowConfig.width = WEBC_SCREEN_WIDTH;
	WindowConfig.height= WEBC_SCREEN_HEIGHT;
	WindowConfig.BorderThickness = EXAMPLEBORDERWIDTH;
	WindowConfig.BorderColor = webc_GetDefaultColor(WEBC_DEFAULT_TEXT_COLOR);
	WindowConfig.SetBodyBackGroundColor = 1;
	WindowConfig.BackGroundColor = webc_GetDefaultColor(WEBC_DEFAULT_BACKGROUND_COLOR);
	WindowConfig.BorderStyle = 0;  // or one of BS_RAISED,BS_LOWERED,BS_RIDGE,BS_GROOVE
	WindowConfig.WindowFlags = WF_DISABLESCROLLBARS;   // WF_RESIZABLE,WF_DISABLESCROLLBARS,WF_MODAL,WF_SAVEUNDER,WF_SPRITE,WF_STARTINBG

	// Add the Example subtree to the wec: file system
	wcAddFsTree(ExampleContent);

	// Open the window no initial content
	if (wcWinCreateEx(	&mainwindowCtx,
		"Example",		// char *_windowname
		webcApiExamplecb, // wcEventCallback BodyCallback
		0, // char *_defaultUrl
		0, // char *_defaultContent,
		&WindowConfig // struct HTMLBrowserWindowConfig *pWindowConfig
	) < 0)
		return;
#if(INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS)
FormatDivWindow();
return;
#endif

	FormatMainWindow();
	// Create a child window in background, leave BUTTON_CONSOLE_HEIGHT pixels exposed of the parent window.
	if (OpenExampleChildWindow() < 0)
		return;
	wcDocRefresh(&mainwindowCtx);
}
/** @name OpenExampleChildWindow
   	@memo Responsible for opening the window context used by example below.
 	@doc This source code provided in webcapiexample.c implements a simple drag and drop interface that drags test buttons from a pallet that when dropped into a target div are cloned and inserted into the target div.
 	<pre>

//
// Source code for OpenExampleChildWindow
//

static int OpenExampleChildWindow(void)
{
	struct HTMLBrowserWindowConfig WindowConfig;
	// Create a child window in background, leave BUTTON_CONSOLE_HEIGHT pixels exposed of the parent window.
	rtp_memset(&childwindowCtx, 0, sizeof(WindowConfig));
	WindowConfig.top   = 0;
	WindowConfig.left  = 0;
	WindowConfig.width = WEBC_SCREEN_WIDTH;
	WindowConfig.height= CHILD_WINDOW_HEIGHT;
	WindowConfig.BorderThickness = EXAMPLEBORDERWIDTH;
	WindowConfig.BorderColor = webc_GetDefaultColor(WEBC_DEFAULT_TEXT_COLOR);
	WindowConfig.SetBodyBackGroundColor = 1;
	WindowConfig.BackGroundColor = webc_GetDefaultColor(WEBC_DEFAULT_BACKGROUND_COLOR);
	WindowConfig.BorderStyle = 0;  // or one of BS_RAISED,BS_LOWERED,BS_RIDGE,BS_GROOVE
	WindowConfig.WindowFlags = WF_DISABLESCROLLBARS|WF_STARTINBG;   // WF_RESIZABLE,WF_DISABLESCROLLBARS,WF_MODAL,WF_SAVEUNDER,WF_SPRITE,WF_STARTINBG

	return (\Ref{wcWinCreateEx}(	&childwindowCtx,
		"Example",		// char *_windowname
		0, // wcEventCallback BodyCallback
		0, // char *_defaultUrl
		0, // char *_defaultContent,
		&WindowConfig // struct HTMLBrowserWindowConfig *pWindowConfig
	) < 0);
}

	</pre>
 */
static int OpenExampleChildWindow(void)
{
	struct HTMLBrowserWindowConfig WindowConfig;
	// Create a child window in background, leave BUTTON_CONSOLE_HEIGHT pixels exposed of the parent window.
	rtp_memset(&WindowConfig, 0, sizeof(WindowConfig));
	WindowConfig.top   = 0;
	WindowConfig.left  = 0;
	WindowConfig.width = WEBC_SCREEN_WIDTH;
	WindowConfig.height= CHILD_WINDOW_HEIGHT;
	WindowConfig.BorderThickness = EXAMPLEBORDERWIDTH;
	WindowConfig.BorderColor = webc_GetDefaultColor(WEBC_DEFAULT_TEXT_COLOR);
	WindowConfig.SetBodyBackGroundColor = 1;
	WindowConfig.BackGroundColor = webc_GetDefaultColor(WEBC_DEFAULT_BACKGROUND_COLOR);
	WindowConfig.BorderStyle = 0;  // or one of BS_RAISED,BS_LOWERED,BS_RIDGE,BS_GROOVE
	WindowConfig.WindowFlags = WF_DISABLESCROLLBARS|WF_STARTINBG;   // WF_RESIZABLE,WF_DISABLESCROLLBARS,WF_MODAL,WF_SAVEUNDER,WF_SPRITE,WF_STARTINBG

	return (wcWinCreateEx(	&childwindowCtx,
		"Example",		// char *_windowname
		0, // wcEventCallback BodyCallback
		0, // char *_defaultUrl
		"Hello", // char *_defaultContent,
		&WindowConfig // struct HTMLBrowserWindowConfig *pWindowConfig
	) < 0);
}

static HTMLEventStatus MainButtonCb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param);



static void FormatMainWindow(void)
{
	char styletext[256];
	wcEL ConsoleDivObject;

	gSplashDivObject = wcElNew(&mainwindowCtx, "div");
	rtp_sprintf(styletext, "position:absolute;left:%dpx;top:%dpx;height:%dpx;width:%dpx;%s",0,0,CHILD_WINDOW_HEIGHT,MAIN_CANVAS_WIDTH,SPLASH_TEXT_STYLE);
	wcElSetStyle(gSplashDivObject,styletext,wcFALSE);
	wcDocInsertLast(&mainwindowCtx, gSplashDivObject);
	wcElSetInnerHtml(gSplashDivObject, SPLASH_HTML);

	ConsoleDivObject = wcElNew(&mainwindowCtx, "div");
	rtp_sprintf(styletext, "position:absolute;left:%dpx;top:%dpx;height:%dpx;width:%dpx;%s",0,CONSOLE_POS,CONSOLE_HEIGHT,CONSOLE_WIDTH,CONSOLE_BACKGROUND_STYLE);
	wcElSetStyle(ConsoleDivObject,styletext,wcFALSE);
	wcDocInsertLast(&mainwindowCtx, ConsoleDivObject);

	gConsoleTextObject = wcElNew(&mainwindowCtx, "textarea");
	rtp_sprintf(styletext, "position:absolute;left:%dpx;top:%dpx;height:%dpx;width:%dpx;%s",0,0,PROMPT_CONSOLE_HEIGHT,CONSOLE_WIDTH,CONSOLE_TEXT_STYLE);
	wcElSetStyle(gConsoleTextObject,styletext,wcFALSE);
	wcElSetValue(gConsoleTextObject,"Main Window");
	wcElInsertLast(ConsoleDivObject, gConsoleTextObject,wcFALSE);
	for (int i = 0; HomebuttonArray[i].name; i++)
	{

	wcEL ButtonObject;
		ButtonObject = wcElNew(&mainwindowCtx, "input type=button");
		wcElSetValue(ButtonObject,HomebuttonArray[i].name);
		//rtp_sprintf(styletext, "position:absolute;left:%dpx;top:%dpx;height:%dpx;width:%dpx;%s",HomebuttonArray[i].xpos,BUTTON_ARRAY_YPOS,BUTTON_ARRAY_HEIGHT,BUTTON_CONSOLE_WIDTH,BUTTON_STYLE);
		//wcElSetStyle(ButtonObject,styletext,wcFALSE);
		//wcElSetStyle(ButtonObject,BUTTON_STYLE,wcFALSE);

		wcElSetStyleColorRgb(ButtonObject,CSS_PROPERTY_COLOR, 255, 0, 255, wcFALSE);
		wcElSetStyleColorNamed(ButtonObject,CSS_PROPERTY_BACKGROUND_COLOR, "SteelBlue", wcFALSE);

		wcElSetStylePos(ButtonObject, BUTTON_ARRAY_YPOS, HomebuttonArray[i].xpos,wcFALSE);
		wcElSetStyleDim(ButtonObject, BUTTON_ARRAY_HEIGHT, BUTTON_CONSOLE_WIDTH, wcFALSE);
		wcElSetEventHandler(ButtonObject, MainButtonCb);
		 wcPushData(&mainwindowCtx, ButtonObject,(void *)HomebuttonArray[i].command);
		wcElInsertLast(ConsoleDivObject, ButtonObject,wcFALSE);
	}

}


// ====

static char *ExEventToAscii(HTMLEventType EventType);
static char *ExEventPrintBuffer,*ExEventPrintBufferBase;

// Event handler used when we open a new window
static HTMLEventStatus ExDocWincb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExDocWincb:->%s\n", ExEventToAscii(event->type));
    return (HTML_EVENT_STATUS_CONTINUE);
}
// Event handler used when we call wcDocSetEventHandler
static HTMLEventStatus ExDocWincbTwo(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExDocWincbTwo:->%s\n", ExEventToAscii(event->type));
    return (HTML_EVENT_STATUS_CONTINUE);
}

/** @name ExDocEventHandling
   	@memo This example demonstrates using document event handlers.
 	@doc Document event handlers are actually just element event handlers acting on the body element of a document. The body element is the containing element for all other elements
 	so its event handlers provide some additional utility, such as on application screen onload and on unload processing, that other element handlers do not.<br>
	<i>Note: This example demonstrates event behavior and API usage, other examples show real world application of event handlers. </i>

	<ul>
	<li>Can be used to configure allocate and initialize application screen specific data prior to loading content into the page.</li>
	<li>Can be used to release application screen specific data prior to loading new content or destroying the window.</li>
	<li>Can be used to catch events like clicks that land on the document background.</li>
	<li>Can be used to send events to the application itself and not necessarilly any of its elements.</li>
	<li>Can be used to catch events that were processed by elements in the document but not canceled (bubbled).</li>
	</ul>
	<b>This example demonstrates the following:</b>
	<ul>
	<li>Opening a new window and assigning it a document body event handler.</li>
	<li>Verify using the push/pop stack on a document body event handler.</li>
	<li>Demonstrate the behavior of document event handlers with HTML_EVENT_OBJECT_CREATED, HTML_EVENT_LOAD, and HTML_EVENT_OBJECT_DESTROYED events.</li>
	<li>Demonstrate assigning a document body event handler to an existing document.</li>
	<li>Demonstrate the behavior of document body event handlers with HTML_EVENT_UNLOAD events.</li>
	<li>Verify sending maessage to a document body event handler.</li>
	</ul>
 	<pre>


static char *ExEventToAscii(HTMLEventType EventType);
static char *ExEventPrintBuffer,*ExEventPrintBufferBase;

// Event handler used when we open a new window
static HTMLEventStatus ExDocWincb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExDocWincb:->%s\n", ExEventToAscii(event->type));
    return (HTML_EVENT_STATUS_CONTINUE);
}
// Event handler used when we call wcDocSetEventHandler
static HTMLEventStatus ExDocWincbTwo(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExDocWincbTwo:->%s\n", ExEventToAscii(event->type));
    return (HTML_EVENT_STATUS_CONTINUE);
}


static void ExDocEventHandling(void)
{
wcCtx Ctx;

	struct HTMLBrowserWindowConfig WindowConfig;
	// Create window in the foreground with.
	rtp_memset(&Ctx, 0, sizeof(WindowConfig));
	WindowConfig.top   = 0;
	WindowConfig.left  = 0;
	WindowConfig.width = WEBC_SCREEN_WIDTH;
	WindowConfig.height= CHILD_WINDOW_HEIGHT;
	WindowConfig.BorderThickness = EXAMPLEBORDERWIDTH;
	WindowConfig.BorderColor = webc_GetDefaultColor(WEBC_DEFAULT_TEXT_COLOR);
	WindowConfig.SetBodyBackGroundColor = 1;
	WindowConfig.BackGroundColor = webc_GetDefaultColor(WEBC_DEFAULT_BACKGROUND_COLOR);
	WindowConfig.BorderStyle = 0;  // or one of BS_RAISED,BS_LOWERED,BS_RIDGE,BS_GROOVE
	WindowConfig.WindowFlags = WF_DISABLESCROLLBARS;   // WF_RESIZABLE,WF_DISABLESCROLLBARS,WF_MODAL,WF_SAVEUNDER,WF_SPRITE,WF_STARTINBG

	// Allocate a buffer for logging results in formated html.
	ExEventPrintBuffer=ExEventPrintBufferBase = (char *) WEBC_MALLOC(0x8000);
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "&lt;small&gt;&lt;textarea style=\"font-size:small;color:black;height:%dpx;width:%dpx;white-space:no-wrap;\"&gt;", CHILD_CANVAS_HEIGHT, MAIN_CANVAS_WIDTH);

	// print what we events we are expecting, then open the window with some initial content in a url, the event hendler will print what events it processed.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "%s","ExDocWincb:Expecting:HTML_EVENT_OBJECT_CREATED,HTML_EVENT_LOAD,HTML_EVENT_UNLOAD\n");
	\Ref{wcWinCreateEx}(&Ctx,
		"ExDocExample",		// char *_windowname
		ExDocWincb, // wcEventCallback BodyCallback
		"webc:///Example/Index.html",  // char *_defaultUrl
		0, // char *_defaultContent,
		&WindowConfig // struct HTMLBrowserWindowConfig *pWindowConfig
	);
	\Ref{wcWinClose}(&Ctx);

	// Open the window with some initial content in a string
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExDocWincb:Expecting:HTML_EVENT_OBJECT_CREATED,HTML_EVENT_LOAD\n");
	\Ref{wcWinCreateEx}(&Ctx,
		"ExDocExample",		// char *_windowname
		ExDocWincb, // wcEventCallback BodyCallback
		0, // char *_defaultUrl
		"Look at the printf console window for results", // char *_defaultContent,
		&WindowConfig // struct HTMLBrowserWindowConfig *pWindowConfig
	);

	// Set a new event handler. An HTML_EVENT_OBJECT_CREATED event is automatically sent to the hander.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer,  "ExDocWincbTwo:Expecting:HTML_EVENT_OBJECT_CREATED\n");
	\Ref{wcDocSetEventHandler}(&Ctx,ExDocWincbTwo);

	// Send a timer event to the new event handler to show that it is attached.
	HTMLEvent Event;
	rtp_memset(&Event, 0 ,sizeof(Event));
	Event.type = HTML_EVENT_TIMER;
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExDocWincbTwo:Expecting:HTML_EVENT_TIMER\n");
	\Ref{wcDocTriggerEvent}(&Ctx, &Event, 0);
	// Pop the old event handler and send a timer event to be sure that pop worked.
	\Ref{wcDocPopEventHandler}(&Ctx);
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer,"ExDocWincb:Expecting:HTML_EVENT_TIMER\n");

	\Ref{wcDocTriggerEvent}(&Ctx, &Event, 0);
	// Close the document should generate an HTML_EVENT_UNLOAD event to the original handler
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExDocWincb:Expecting:HTML_EVENT_UNLOAD\n");

	\Ref{wcWinClose}(&Ctx);

	// Bring the previous window to top.
	\Ref{wcWinBringToTop}(&childwindowCtx);
	// Set inner html on the body to report our results.
	wcEL hBody = \Ref{wcDocGetBody}(&childwindowCtx);
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "&lt;/textarea&gt;&lt;/small&gt;");
	\Ref{wcElSetInnerHtml}(hBody, ExEventPrintBufferBase);
	\Ref{wcDocRefresh}(&childwindowCtx); // Make sure it is visible
	WEBC_FREE(ExEventPrintBufferBase);

}


	</pre>
 */

static void ExDocEventHandling(void)
{
wcCtx Ctx;

	struct HTMLBrowserWindowConfig WindowConfig;
	// Create window in the foreground with.
	rtp_memset(&Ctx, 0, sizeof(WindowConfig));
	WindowConfig.top   = 0;
	WindowConfig.left  = 0;
	WindowConfig.width = WEBC_SCREEN_WIDTH;
	WindowConfig.height= CHILD_WINDOW_HEIGHT;
	WindowConfig.BorderThickness = EXAMPLEBORDERWIDTH;
	WindowConfig.BorderColor = webc_GetDefaultColor(WEBC_DEFAULT_TEXT_COLOR);
	WindowConfig.SetBodyBackGroundColor = 1;
	WindowConfig.BackGroundColor = webc_GetDefaultColor(WEBC_DEFAULT_BACKGROUND_COLOR);
	WindowConfig.BorderStyle = 0;  // or one of BS_RAISED,BS_LOWERED,BS_RIDGE,BS_GROOVE
	WindowConfig.WindowFlags = WF_DISABLESCROLLBARS;   // WF_RESIZABLE,WF_DISABLESCROLLBARS,WF_MODAL,WF_SAVEUNDER,WF_SPRITE,WF_STARTINBG

	// Allocate a buffer for logging results in formated html.
	ExEventPrintBuffer=ExEventPrintBufferBase = (char *) WEBC_MALLOC(0x8000);
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "<small><textarea style=\"font-size:small;color:black;height:%dpx;width:%dpx;white-space:no-wrap;\">", CHILD_CANVAS_HEIGHT, MAIN_CANVAS_WIDTH);

	// print what we events we are expecting, then open the window with some initial content in a url, the event hendler will print what events it processed.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "%s","ExDocWincb:Expecting:HTML_EVENT_OBJECT_CREATED,HTML_EVENT_LOAD,HTML_EVENT_UNLOAD\n");
	wcWinCreateEx(&Ctx,
		"ExDocExample",		// char *_windowname
		ExDocWincb, // wcEventCallback BodyCallback
		"webc:///Example/Index.html",  // char *_defaultUrl
		0, // char *_defaultContent,
		&WindowConfig // struct HTMLBrowserWindowConfig *pWindowConfig
	);
	wcWinClose(&Ctx);

	// Open the window with some initial content in a string
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExDocWincb:Expecting:HTML_EVENT_OBJECT_CREATED,HTML_EVENT_LOAD\n");
	wcWinCreateEx(&Ctx,
		"ExDocExample",		// char *_windowname
		ExDocWincb, // wcEventCallback BodyCallback
		0, // char *_defaultUrl
		"Look at the printf console window for results", // char *_defaultContent,
		&WindowConfig // struct HTMLBrowserWindowConfig *pWindowConfig
	);

	// Set a new event handler. An HTML_EVENT_OBJECT_CREATED event is automatically sent to the hander.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer,  "ExDocWincbTwo:Expecting:HTML_EVENT_OBJECT_CREATED\n");
	wcDocSetEventHandler(&Ctx,ExDocWincbTwo);

	// Send a timer event to the new event handler to show that it is attached.
	HTMLEvent Event;
	rtp_memset(&Event, 0 ,sizeof(Event));
	Event.type = HTML_EVENT_TIMER;
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExDocWincbTwo:Expecting:HTML_EVENT_TIMER\n");
	wcDocTriggerEvent(&Ctx, &Event, 0);
	// Pop the old event handler and send a timer event to be sure that pop worked.
	wcDocPopEventHandler(&Ctx);
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer,"ExDocWincb:Expecting:HTML_EVENT_TIMER\n");

	wcDocTriggerEvent(&Ctx, &Event, 0);
	// Close the document should generate an HTML_EVENT_UNLOAD event to the original handler
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExDocWincb:Expecting:HTML_EVENT_UNLOAD\n");

	wcWinClose(&Ctx);

	// Bring the previous window to top.
	wcWinBringToTop(&childwindowCtx);
	// Set inner html on the body to report our results.
	wcEL hBody = wcDocGetBody(&childwindowCtx);
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "</textarea></small>");
	wcElSetInnerHtml(hBody, ExEventPrintBufferBase);
	wcDocRefresh(&childwindowCtx); // Make sure it is visible
	wcCtxtRelease(&childwindowCtx); // Release any accumulated garbage.
	WEBC_FREE(ExEventPrintBufferBase);

}

// Event handler used when we open a new window
static HTMLEventStatus ExElEventcb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExElEventcb:->%s\n", ExEventToAscii(event->type));
    return (HTML_EVENT_STATUS_CONTINUE);
}
// Event handler used when we call wcDocSetEventHandler
static HTMLEventStatus ExElEventcbTwo(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExElEventcbTwo:->%s\n", ExEventToAscii(event->type));
    return (HTML_EVENT_STATUS_CONTINUE);
}

/** @name ExElEventHandling
   	@memo This example demonstrates using element event handlers.
 	@doc This source code provided in webcapiexample.c demonstrates how to use element event handlers. Element event handlers are attached to individual elements and called when events are directed to them. These handlers perform
 	most of the work in a typical application.<br>
	<i>Note: This example demonstrates event behavior and API usage, other examples show real world application of event handlers. </i>
	<br> Note: childwindowCtx has already been initialized to a valid window context by \Ref{OpenExampleChildWindow}.
 	<pre>
<ul>
<li>Demonstrate assigning an event handler to an element.</li>
<li>Verify using the push/pop stack on an event handler.</li>
<li>Demonstrate the behavior of element event handlers with HTML_EVENT_OBJECT_CREATED, HTML_EVENT_OBJECT_DESTROYED events.</li>
<li>Demonstrate sending events to element event handlers.</li>
</ul>

// Event handler used when we open a new window
static HTMLEventStatus ExElEventcb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExElEventcb:->%s\n", ExEventToAscii(event->type));
    return (HTML_EVENT_STATUS_CONTINUE);
}
// Event handler used when we call wcDocSetEventHandler
static HTMLEventStatus ExElEventcbTwo(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExElEventcbTwo:->%s\n", ExEventToAscii(event->type));
    return (HTML_EVENT_STATUS_CONTINUE);
}
static void ExElEventHandling(void)
{
	wcEL Element;
	HTMLEvent Event;

	// Allocate a buffer for logging results in formated html.
	ExEventPrintBuffer=ExEventPrintBufferBase = (char *) WEBC_MALLOC(0x8000);
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "&lt;small&gt;&lt;textarea style=\"font-size:small;color:black;height:%dpx;width:%dpx;white-space:no-wrap;\"&gt;", CHILD_CANVAS_HEIGHT, MAIN_CANVAS_WIDTH);
	// Clear the child window content and make sure it is the top window
	\Ref{wcDocClear}(&childwindowCtx, wcFALSE);
	\Ref{wcWinBringToTop}(&childwindowCtx);
	// Set inner html on the body to create some context on the fly.
	wcEL hBody = \Ref{wcDocGetBody}(&childwindowCtx);
	\Ref{wcElSetInnerHtml}(hBody, "Look at the printf console window for results&lt;br&gt;Input control: &lt;input type=text value=\"XXX\"&gt;");
	\Ref{wcDocRefresh}(&childwindowCtx); // Make sure it is visible
	// Get the input element from the document
	Element = \Ref{wcDocFindElement }(&childwindowCtx, 0, 0, HTML_EDIT_STR_ELEMENT, 0);


	// print what events we are expecting, then set event handler, it will print what events it processed.
	// Set an event handler. An HTML_EVENT_OBJECT_CREATED event is automatically sent to the hander.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExElEventcb:Expecting:HTML_EVENT_OBJECT_CREATED\n");

	\Ref{wcElSetEventHandler}(Element,ExElEventcb);
	// Set another event handler. An HTML_EVENT_OBJECT_CREATED event is automatically sent to the hander.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExElEventcbTwo:Expecting:HTML_EVENT_OBJECT_CREATED\n");
	\Ref{wcElSetEventHandler}(Element,ExElEventcbTwo);

	rtp_memset(&Event, 0 ,sizeof(Event));
	Event.type = HTML_EVENT_TIMER;
	// Send a timer event to the second event handler to show that it is attached.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer,"ExElEventcbTwo:Expecting:HTML_EVENT_TIMER\n");
	\Ref{wcElTriggerEvent}(Element, &Event, 0);
	// Pop the old event handler
	\Ref{wcElPopEventHandler}(Element);
	// send it a timer event to be sure that pop worked.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExElEventcb:Expecting:HTML_EVENT_TIMER\n");
	\Ref{wcElTriggerEvent}(Element, &Event, 0);
	// Pop the original event handler (zero unless we inhereted the object through a chain)
	\Ref{wcElPopEventHandler}(Element);
	// Make sure we don't see it.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "NotExpecting:HTML_EVENT_TIMER\n");
	\Ref{wcElTriggerEvent}(Element, &Event, 0);
	// Set inner html on the body to report our results.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "&lt;/textarea&gt;&lt;/small&gt;");
	hBody = \Ref{wcDocGetBody}(&childwindowCtx);
	\Ref{wcElSetInnerHtml}(hBody, ExEventPrintBufferBase);
	\Ref{wcDocRefresh}(&childwindowCtx); // Make sure it is visible
	WEBC_FREE(ExEventPrintBufferBase);

}


static char *ExEventToAscii(HTMLEventType EventType)
{
	switch(EventType)
	{
	/// Single mouse click
	case HTML_EVENT_CLICK:
	    return "HTML_EVENT_CLICK";
	/// Double mouse click
	case HTML_EVENT_DBLCLICK:
	    return "HTML_EVENT_DBLCLICK";
	/// Key pressed
	case HTML_EVENT_KEYDOWN:
	    return "HTML_EVENT_KEYDOWN";
	/// Key pressed and released
	case HTML_EVENT_KEYPRESS:
	    return "HTML_EVENT_KEYPRESS";
	/// Key released
	case HTML_EVENT_KEYUP:
	    return "HTML_EVENT_KEYUP";
	/// Mouse button pressed
	case HTML_EVENT_MOUSEDOWN:
	    return "HTML_EVENT_MOUSEDOWN";
	/// Mouse movement over this object
	case HTML_EVENT_MOUSEMOVE:
	    return "HTML_EVENT_MOUSEMOVE";
	/// Mouse pointer exits the client area of this object
	case HTML_EVENT_MOUSEOUT:
	    return "HTML_EVENT_MOUSEOUT";
	/// Mouse pointer enters the client area
	case HTML_EVENT_MOUSEOVER:
	    return "HTML_EVENT_MOUSEOVER";
	/// Mouse button released
	case HTML_EVENT_MOUSEUP:
	    return "HTML_EVENT_MOUSEUP";
	/// Object has received input focus
	case HTML_EVENT_FOCUS:
	    return "HTML_EVENT_FOCUS";
	/// Object has lost input focus
	case HTML_EVENT_UNFOCUS:
	    return "HTML_EVENT_UNFOCUS";
	/// Load completes
	case HTML_EVENT_LOAD:
	    return "HTML_EVENT_LOAD";
	/// Object is discarded
	case HTML_EVENT_UNLOAD:
	    return "HTML_EVENT_UNLOAD";
	/// For forms only; submit is pressed
	case HTML_EVENT_SUBMIT:
	    return "HTML_EVENT_SUBMIT";
	/// Input text was edited
	case HTML_EVENT_CHANGE:
	    return "HTML_EVENT_CHANGE";
	/// Finished Editing on a Textbox
	case HTML_EVENT_EDIT:
	    return "HTML_EVENT_EDIT";
	/// For forms only; reset is pressed
	case HTML_EVENT_RESET:
	    return "HTML_EVENT_RESET";
	/// HTMLElement object created
	case HTML_EVENT_OBJECT_CREATED:
	    return "HTML_EVENT_OBJECT_CREATED";
	/// HTMLElement object destroyed
	case HTML_EVENT_OBJECT_DESTROYED:
	    return "HTML_EVENT_OBJECT_DESTROYED";
	/// Attribute parsed
	case HTML_EVENT_ATTRIBUTE_PARSED:
	    return "HTML_EVENT_ATTRIBUTE_PARSED";
	/// mouse capture released
	case HTML_EVENT_LOSECAPTURE:
	    return "HTML_EVENT_LOSECAPTURE";

	case HTML_EVENT_TIMER:
	    return "HTML_EVENT_TIMER";

	case HTML_EVENT_RENDER:
	    return "HTML_EVENT_RENDER";

	case HTML_EVENT_WINDOW_REFRESHED:
	    return "HTML_EVENT_WINDOW_REFRESHED";
	default:
	    return "HTML_UNKNOWN_EVENT";
	}
}


	</pre>
 */

static void ExElEventHandling(void)
{
	wcEL Element;
	HTMLEvent Event;

	// Allocate a buffer for logging results in formated html.
	ExEventPrintBuffer=ExEventPrintBufferBase = (char *) WEBC_MALLOC(0x8000);
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "<small><textarea style=\"font-size:small;color:black;height:%dpx;width:%dpx;white-space:no-wrap;\">", CHILD_CANVAS_HEIGHT, MAIN_CANVAS_WIDTH);
	// Clear the child window content and make sure it is the top window
	wcDocClear(&childwindowCtx, wcFALSE);
	wcWinBringToTop(&childwindowCtx);
	// Set inner html on the body to create some context on the fly.
	wcEL hBody = wcDocGetBody(&childwindowCtx);
	wcElSetInnerHtml(hBody, "Look at the printf console window for results<br>Input control: <input type=text value=\"XXX\">");
	wcDocRefresh(&childwindowCtx); // Make sure it is visible
	// Get the input element from the document
	Element = wcDocFindElement (&childwindowCtx, 0, 0, HTML_EDIT_STR_ELEMENT, 0);


	// print what events we are expecting, then set event handler, it will print what events it processed.
	// Set an event handler. An HTML_EVENT_OBJECT_CREATED event is automatically sent to the hander.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExElEventcb:Expecting:HTML_EVENT_OBJECT_CREATED\n");

	wcElSetEventHandler(Element,ExElEventcb);
	// Set another event handler. An HTML_EVENT_OBJECT_CREATED event is automatically sent to the hander.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExElEventcbTwo:Expecting:HTML_EVENT_OBJECT_CREATED\n");
	wcElSetEventHandler(Element,ExElEventcbTwo);

	rtp_memset(&Event, 0 ,sizeof(Event));
	Event.type = HTML_EVENT_TIMER;
	// Send a timer event to the second event handler to show that it is attached.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer,"ExElEventcbTwo:Expecting:HTML_EVENT_TIMER\n");
	wcElTriggerEvent(Element, &Event, 0);
	// Pop the old event handler
	wcElPopEventHandler(Element);
	// send it a timer event to be sure that pop worked.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "ExElEventcb:Expecting:HTML_EVENT_TIMER\n");
	wcElTriggerEvent(Element, &Event, 0);
	// Pop the original event handler (zero unless we inhereted the object through a chain)
	wcElPopEventHandler(Element);
	// Make sure we don't see it.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "NotExpecting:HTML_EVENT_TIMER\n");
	wcElTriggerEvent(Element, &Event, 0);
	// Allocate a buffer for logging printfs.

	// Set inner html on the body to report our results.
	ExEventPrintBuffer+=rtp_sprintf(ExEventPrintBuffer, "</textarea></small>");
	hBody = wcDocGetBody(&childwindowCtx);
	wcElSetInnerHtml(hBody, ExEventPrintBufferBase);
	wcDocRefresh(&childwindowCtx); // Make sure it is visible
	wcCtxtRelease(&childwindowCtx); // Release any accumulated garbage.
	WEBC_FREE(ExEventPrintBufferBase);

}


static char *ExEventToAscii(HTMLEventType EventType)
{
	switch(EventType)
	{
	/// Single mouse click
	case HTML_EVENT_CLICK:
	    return "HTML_EVENT_CLICK";
	/// Double mouse click
	case HTML_EVENT_DBLCLICK:
	    return "HTML_EVENT_DBLCLICK";
	/// Key pressed
	case HTML_EVENT_KEYDOWN:
	    return "HTML_EVENT_KEYDOWN";
	/// Key pressed and released
	case HTML_EVENT_KEYPRESS:
	    return "HTML_EVENT_KEYPRESS";
	/// Key released
	case HTML_EVENT_KEYUP:
	    return "HTML_EVENT_KEYUP";
	/// Mouse button pressed
	case HTML_EVENT_MOUSEDOWN:
	    return "HTML_EVENT_MOUSEDOWN";
	/// Mouse movement over this object
	case HTML_EVENT_MOUSEMOVE:
	    return "HTML_EVENT_MOUSEMOVE";
	/// Mouse pointer exits the client area of this object
	case HTML_EVENT_MOUSEOUT:
	    return "HTML_EVENT_MOUSEOUT";
	/// Mouse pointer enters the client area
	case HTML_EVENT_MOUSEOVER:
	    return "HTML_EVENT_MOUSEOVER";
	/// Mouse button released
	case HTML_EVENT_MOUSEUP:
	    return "HTML_EVENT_MOUSEUP";
	/// Object has received input focus
	case HTML_EVENT_FOCUS:
	    return "HTML_EVENT_FOCUS";
	/// Object has lost input focus
	case HTML_EVENT_UNFOCUS:
	    return "HTML_EVENT_UNFOCUS";
	/// Load completes
	case HTML_EVENT_LOAD:
	    return "HTML_EVENT_LOAD";
	/// Object is discarded
	case HTML_EVENT_UNLOAD:
	    return "HTML_EVENT_UNLOAD";
	/// For forms only; submit is pressed
	case HTML_EVENT_SUBMIT:
	    return "HTML_EVENT_SUBMIT";
	/// Input text was edited
	case HTML_EVENT_CHANGE:
	    return "HTML_EVENT_CHANGE";
	/// Finished Editing on a Textbox
	case HTML_EVENT_EDIT:
	    return "HTML_EVENT_EDIT";
	/// For forms only; reset is pressed
	case HTML_EVENT_RESET:
	    return "HTML_EVENT_RESET";
	/// HTMLElement object created
	case HTML_EVENT_OBJECT_CREATED:
	    return "HTML_EVENT_OBJECT_CREATED";
	/// HTMLElement object destroyed
	case HTML_EVENT_OBJECT_DESTROYED:
	    return "HTML_EVENT_OBJECT_DESTROYED";
	/// Attribute parsed
	case HTML_EVENT_ATTRIBUTE_PARSED:
	    return "HTML_EVENT_ATTRIBUTE_PARSED";
	/// mouse capture released
	case HTML_EVENT_LOSECAPTURE:
	    return "HTML_EVENT_LOSECAPTURE";

	case HTML_EVENT_TIMER:
	    return "HTML_EVENT_TIMER";

	case HTML_EVENT_RENDER:
	    return "HTML_EVENT_RENDER";

	case HTML_EVENT_WINDOW_REFRESHED:
	    return "HTML_EVENT_WINDOW_REFRESHED";
	default:
	    return "HTML_UNKNOWN_EVENT";
	}
}

// Definitions for ExDragButtons. Out of the way of doc++
static HTMLEventStatus FlyButtonCb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param);
static wcEL DragPallete, DragTarget;
#define PALLETTOP  20
#define PALLETLEFT 200
#define PALLETHEIGHT 120
#define PALLETWIDTH  120
#define TARGETTOP  10
#define TARGETLEFT 10
#define TARGETHEIGHT 160
#define TARGETWIDTH  160
#define EXDBUTTONHEIGHT 20
#define EXDBUTTONWIDTH  20
#define PALLETBUTTONZ 6
#define PALLETZ	5
#define FLYBUTTONZ 7

/** @name ExDragButtons
   	@memo This example demonstrates using dynamic HTML from 'C' create a pallet of buttons that may be dragged and inserted into a target area.
 	@doc This source code provided in webcapiexample.c implements a simple drag and drop interface that drags test buttons from a pallet that when dropped into a target div are cloned and inserted into the target div.
	<br> Note: childwindowCtx has already been initialized to a valid window context by \Ref{OpenExampleChildWindow}.
 	<pre>

//
// Source code for ExDragButtons
//

static HTMLEventStatus FlyButtonCb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param);
static wcEL DragPallete, DragTarget;
#define PALLETTOP  20
#define PALLETLEFT 200
#define PALLETHEIGHT 120
#define PALLETWIDTH  120
#define TARGETTOP  10
#define TARGETLEFT 10
#define TARGETHEIGHT 160
#define TARGETWIDTH  160
#define EXDBUTTONHEIGHT 20
#define EXDBUTTONWIDTH  20
#define PALLETBUTTONZ 6
#define PALLETZ	5
#define FLYBUTTONZ 7
#define EXTARGETIMAGE "webc:/Example/Images/Target.png"

static void ExDragButtons(void)
{
char buttonfacetext[4];
	// Clear the window content
	\Ref{wcDocClear}(&childwindowCtx, wcFALSE);
	// Insert a div into the document to act as our pallet no foreground color no extra style directive
	DragPallete=\Ref{wcNewStyledDiv}(&childwindowCtx, 0, wcTRUE, PALLETTOP, PALLETLEFT, PALLETHEIGHT, PALLETWIDTH,PALLETZ, 0, "DarkTurquoise",0, 0);
	// Create two 3 by 3 array of buttons in deifferent z-index planes. The top layer of buttons will be dragged around
	buttonfacetext[0] = 'a';buttonfacetext[1]=0;
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
		wcEL ButtonObject,FlyButtonObject;
		int left, top;
			buttonfacetext[0]=(char) 'a' + (row*3) + col;
			left = PALLETLEFT+(EXDBUTTONWIDTH/2) + col*(EXDBUTTONWIDTH*2);
			top  = PALLETTOP+(EXDBUTTONHEIGHT/2) + row*(EXDBUTTONHEIGHT*2);
			// The button that remains behind when we drag.
			ButtonObject=\Ref{wcNewStyledButtonText(&childwindowCtx, 0, wcTRUE, buttonfacetext, top, left, EXDBUTTONHEIGHT, EXDBUTTONWIDTH, PALLETBUTTONZ, "Black", "SteelGrey", 0);
			// The button that gets dragged.
			FlyButtonObject=wcNewStyledButtonText(&childwindowCtx, 0, wcTRUE, buttonfacetext, top, left, EXDBUTTONHEIGHT, EXDBUTTONWIDTH, FLYBUTTONZ, "Black", "SteelBlue", 0);
			// Set an event handler for the button so we can drag it.
			\Ref{wcElSetEventHandler}(FlyButtonObject, FlyButtonCb);
			\Ref{wcPushData}(&childwindowCtx, FlyButtonObject,(void *) 0);// Use wcPushData to set the state to zero, no dragging
		}
	}
	// Insert a div into the document to act as our target.
	DragTarget=\Ref{wcNewStyledDiv}(&childwindowCtx, 0, wcTRUE, TARGETTOP, TARGETLEFT, TARGETWIDTH, TARGETHEIGHT,PALLETZ, 0, "DarkSlateBlue", EXTARGETIMAGE, "overflow:hidden;");
	// Bring the child windo to the top and go
	\Ref{wcWinBringToTop}(&childwindowCtx);
}
// Event handler for dragging.
static HTMLEventStatus FlyButtonCb(wcCtx* Ctx,wcEL element,HTMLEvent* e,char* param)
{
	switch (e->type)
	{
		case HTML_EVENT_MOUSEDOWN:
			\Ref{wcPopData}(Ctx, element);
			\Ref{wcPushData}(Ctx, element,(void *)1);	// Set the state to one, dragging
			\Ref{wcElClaimPointer}(element);			// Claim the pointer until the button is released.
			return HTML_EVENT_STATUS_HALT;		// returning HALT so normal button processing is not performed
			break;
		case HTML_EVENT_MOUSEMOVE:
			if (\Ref{wcGetData}(Ctx, element))
			{ // Mouse the button if we are dragging
				\Ref{wcElSetStylePos}(element, e->data.position.y, e->data.position.x, wcTRUE);
				return HTML_EVENT_STATUS_HALT;
			}
			break;
		case HTML_EVENT_MOUSEUP:
		{
			if (\Ref{wcGetData}(Ctx, element))
			{ // If we are dragging.
			int row, col, left, top,width, height;
			char *p;
				\Ref{wcPopData}(Ctx, element); // Pop the value, will set private data to null
				// Move the tracking button back to the pallet.
				p = \Ref{wcELGetValue}(Ctx,element);
				row = (int)(*p - 'a')/3;
				col = (int)(*p - 'a')%3;
				left = PALLETLEFT+(EXDBUTTONWIDTH/2) + col*(EXDBUTTONWIDTH*2);
				top  = PALLETTOP+(EXDBUTTONHEIGHT/2) + row*(EXDBUTTONHEIGHT*2);
				\Ref{wcElSetStylePos}(element,top, left, wcTRUE);

				// Check if we are in the target div, if so, create a new button and put it into the target div at the mouse coordinates
				\Ref{wcElGetPosStyle}(DragTarget, &top, &left, &height,  &width);
				if (e->data.position.y >= top && e->data.position.y < top + height && e->data.position.x >= left && e->data.position.x < left + width)
					\Ref{wcNewStyledButtonText}(Ctx, DragTarget, wcTRUE, p, e->data.position.y , e->data.position.x, EXDBUTTONHEIGHT, EXDBUTTONWIDTH, PALLETBUTTONZ, "Red", "BlanchedAlmond", 0);
				// Release the mouse pointer now that we are not dragging
				\Ref{wcElReleasePointer}(element);
				return HTML_EVENT_STATUS_HALT;
			}
			break;
		}
	}
	return HTML_EVENT_STATUS_CONTINUE;
//	return HTML_EVENT_STATUS_HALT;
}
	</pre>
 */
static void ExDragButtons(void)
{
char buttonfacetext[4];
	// Clear the window content
	wcDocClear(&childwindowCtx, wcFALSE);
	// Insert a div into the document to act as our pallet no foreground color no extra style directive
	DragPallete=wcNewStyledDiv(&childwindowCtx, 0, wcTRUE, PALLETTOP, PALLETLEFT, PALLETHEIGHT, PALLETWIDTH,PALLETZ, 0, "DarkTurquoise",0, 0);
	// Create two 3 by 3 array of buttons in deifferent z-index planes. The top layer of buttons will be dragged around
	buttonfacetext[0] = 'a';buttonfacetext[1]=0;
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
		wcEL ButtonObject,FlyButtonObject;
		int left, top;
			buttonfacetext[0]=(char) 'a' + (row*3) + col;
			left = PALLETLEFT+(EXDBUTTONWIDTH/2) + col*(EXDBUTTONWIDTH*2);
			top  = PALLETTOP+(EXDBUTTONHEIGHT/2) + row*(EXDBUTTONHEIGHT*2);
			// The button that remains behind when we drag.
			ButtonObject=wcNewStyledButtonText(&childwindowCtx, 0, wcTRUE, buttonfacetext, top, left, EXDBUTTONHEIGHT, EXDBUTTONWIDTH, PALLETBUTTONZ, "Black", "SteelGrey", 0);
			// The button that gets dragged.
			FlyButtonObject=wcNewStyledButtonText(&childwindowCtx, 0, wcTRUE, buttonfacetext, top, left, EXDBUTTONHEIGHT, EXDBUTTONWIDTH, FLYBUTTONZ, "White", "Coral", 0);
			// Set an event handler for the button so we can drag it.
			wcElSetEventHandler(FlyButtonObject, FlyButtonCb);
			wcPushData(&childwindowCtx, FlyButtonObject,(void *) 0);// Use wcPushData to set the state to zero, no dragging
		}
	}
	// Insert a div into the document to act as our target.
	DragTarget=wcNewStyledDiv(&childwindowCtx, 0, wcTRUE, TARGETTOP, TARGETLEFT, TARGETWIDTH, TARGETHEIGHT,PALLETZ, 0, "DarkSlateBlue", EXTARGETIMAGE, "overflow:hidden;");
	// Bring the child windo to the top and go
	wcWinBringToTop(&childwindowCtx);
}
// Event handler for dragging.
static HTMLEventStatus FlyButtonCb(wcCtx* Ctx,wcEL element,HTMLEvent* e,char* param)
{
	switch (e->type)
	{
		case HTML_EVENT_MOUSEDOWN:
			wcPopData(Ctx, element);
			wcPushData(Ctx, element,(void *)1);	// Set the state to one, dragging
			wcElClaimPointer(element);			// Claim the pointer until the button is released.
			return HTML_EVENT_STATUS_HALT;		// returning HALT so normal button processing is not performed
			break;
		case HTML_EVENT_MOUSEMOVE:
			if (wcGetData(Ctx, element))
			{ // Mouse the button if we are dragging
				wcElSetStylePos(element, e->data.position.y, e->data.position.x, wcTRUE);
				return HTML_EVENT_STATUS_HALT;
			}
			break;
		case HTML_EVENT_MOUSEUP:
		{
			if (wcGetData(Ctx, element))
			{ // If we are dragging.
			int row, col, left, top,width, height;
			char *p;
				wcPopData(Ctx, element); // Pop the value, will set private data to null
				// Move the tracking button back to the pallet.
				p = wcELGetValue(Ctx,element);
				row = (int)(*p - 'a')/3;
				col = (int)(*p - 'a')%3;
				left = PALLETLEFT+(EXDBUTTONWIDTH/2) + col*(EXDBUTTONWIDTH*2);
				top  = PALLETTOP+(EXDBUTTONHEIGHT/2) + row*(EXDBUTTONHEIGHT*2);
				wcElSetStylePos(element,top, left, wcTRUE);

				// Check if we are in the target div, if so, create a new button and put it into the target div at the mouse coordinates
				wcElGetPosStyle(DragTarget, &top, &left, &height, &width);
				if (e->data.position.y >= top && e->data.position.y < top + height && e->data.position.x >= left && e->data.position.x < left + width)
					wcNewStyledButtonText(Ctx, DragTarget, wcTRUE, p, e->data.position.y , e->data.position.x, EXDBUTTONHEIGHT, EXDBUTTONWIDTH, PALLETBUTTONZ, "HotPink", "DarkTurquoise", 0);
				// Release the mouse pointer now that we are not dragging
				wcElReleasePointer(element);
				return HTML_EVENT_STATUS_HALT;
			}
			break;
		}
	}
	return HTML_EVENT_STATUS_CONTINUE;
//	return HTML_EVENT_STATUS_HALT;
}


// Static for the next test
static HTMLEventStatus CalcButtonCb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param); // Button handler that implements the calculator
// For brevity use static variable to hold a few values but we could allocate them and use private dtaa pointers.
static wcEL CalcWindow; // Window where the digits are displayed
static char CalcStringVal[32];
static int CalcStringCount;
static int LastOperand;
static char LastOperator;

static wcBOOL CalcEnumeratorFunction(wcCtx* Ctx, wcEL El, int UserIntParm, void *UserVoidParm);
static char *CalcHtmlSource = "\
<html>\
<head>\
<style>\
.table\
{\
font-family: \"arial bold\", arial;\
font-size:8px;\
color:grey;\
left:20px;\
top:0px;\
width:300px;\
height:120px;\
position:absolute;\
border:1px solid black;\
}\
td\
{\
background-color:orange;\
width:40px;\
text-align:center;\
border:2px solid white;\
padding:2px;\
}\
\
th\
{\
background-color:lightblue;\
color:steelgrey;\
border:1px solid grey;\
text-align:right;\
font-size:14px;\
}\
   </style>\
\
</head>\
<body id=\"ComposeWindow\"  style=\"overflow:hidden\">\
<table class=\"CalculatorTable\">\
<tr><th colspan=4>|</th></tr>\
<tr><td>7</td><td>8</td><td>9</td><td>+</td></tr>\
<tr><td>4</td><td>5</td><td>6</td><td>-</td></tr>\
<tr><td>1</td><td>2</td><td>3</td><td>=</td></tr>\
<tr><td colspan=2>0</td><td colspan=2>Clr</td></tr>\
</table></body></html>";

/** @name ExCalculator
   	@memo This example demonstrates mixing html and css source with 'C' source code to implement a simple functional calculator in 40 lines of CSS and HTML and < 100 lines of C.
<ul>
<li>Demonstrate using \Ref{wcDocEnumerate} to access elements in an html document.
</ul>
 	@doc This source code provided in webcapiexample.c provides examples for creating create elements and display them with customized styles.
	<br> Note: childwindowCtx has already been initialized to a valid window context by \Ref{OpenExampleChildWindow}.
<pre>
// ===================================================================
// Declare an array that holds the html source code for our calculator
    static char *CalcHtmlSource = "\
    &lt;html&gt;\
    &lt;head&gt;\
    &lt;style&gt;\
    .table\
    {\
    font-family: \"arial bold\", arial;\
    font-size:8px;\
    color:grey;\
    left:20px;\
    top:0px;\
    width:300px;\
    height:120px;\
    position:absolute;\
    border:1px solid black;\
    }\
    td\
    {\
    background-color:orange;\
    width:40px;\
    text-align:center;\
    border:2px solid white;\
    padding:2px;\
    }\
    \
    th\
    {\
    background-color:lightblue;\
    color:steelgrey;\
    border:1px solid grey;\
    text-align:right;\
    font-size:14px;\
    }\
     &lt;/style&gt;\
    \
    &lt;/head&gt;\
    &lt;body id=\"ComposeWindow\"  style=\"overflow:hidden\"&gt;\
    &lt;table class=\"CalculatorTable\"&gt;\
    &lt;tr&gt;&lt;th colspan=4&gt;|&lt;/th&gt;&lt;/tr&gt;\
    &lt;tr&gt;&lt;td&gt;7&lt;/td&gt;&lt;td&gt;8&lt;/td&gt;&lt;td&gt;9&lt;/td&gt;&lt;td&gt;+&lt;/td&gt;&lt;/tr&gt;\
    &lt;tr&gt;&lt;td&gt;4&lt;/td&gt;&lt;td&gt;5&lt;/td&gt;&lt;td&gt;6&lt;/td&gt;&lt;td&gt;-&lt;/td&gt;&lt;/tr&gt;\
    &lt;tr&gt;&lt;td&gt;1&lt;/td&gt;&lt;td&gt;2&lt;/td&gt;&lt;td&gt;3&lt;/td&gt;&lt;td&gt;=&lt;/td&gt;&lt;/tr&gt;\
    &lt;tr&gt;&lt;td colspan=2&gt;0&lt;/td&gt;&lt;td colspan=2&gt;Clr&lt;/td&gt;&lt;/tr&gt;\
    &lt;/table&gt;&lt;/body&gt;&lt;/html&gt;";
    </body></html>


static void ExCalculator(void)
{
	// Clear the window content and load the HTML and CSS calculator source in
	wcDocClear(&childwindowCtx, wcFALSE);
	if (wcDocOpen(&childwindowCtx)==0)
	{
		wcDocWriteHtml(&childwindowCtx, CalcHtmlSource, rtp_strlen(CalcHtmlSource),  wcTRUE);
		wcDocClose(&childwindowCtx);
	}
	// Start the app by enumerating through all the elements and setting event handlers.
	wcDocEnumerate(&childwindowCtx,CalcEnumeratorFunction, 0, 0);
	wcWinBringToTop(&childwindowCtx);
}

static HTMLEventStatus CalcButtonCb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param); // Button handler that implements the calculator
// For brevity use static variable to hold a few values but we could allocate them and use private dtaa pointers.
static wcEL CalcWindow; // Window where the digits are displayed
static char CalcStringVal[32];
static int CalcStringCount;
static int LastOperand;
static char LastOperator;

// This callback is called for ever element in the document
static wcBOOL CalcEnumeratorFunction(wcCtx* Ctx, wcEL El, int UserIntParm, void *UserVoidParm)
{
	if (wcElGetType(El) == HTML_TABLE_CELL_ELEMENT)
	{ // Read the inner HTML, '|' means it is our text window all other table cell element get the same event handler.
	 	char *p = \Ref{wcElGetInnerHtml}(Ctx, El);
		if (p)
		{
			if (rtp_strcmp(p, "|")==0) // '|' means it is our text window where we should places results.
				CalcWindow = El;
			else
			{
				\Ref{wcElSetEventHandler}(El, CalcButtonCb); // Set an event handler
				\Ref{wcPushData}(Ctx, El, (void *) *p);		// Store the keyvalue in private data so we don't have to get html source each time
			}
		}
	}
	return wcFALSE; // Keep enumerating
}


static HTMLEventStatus CalcButtonCb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
int initialExample = currentExample;
static int ClearTextBox = 0;
	if (!element)
		return HTML_EVENT_STATUS_CONTINUE;
	if (event->type == HTML_EVENT_MOUSEDOWN){ // Animate keypresses with a simple color change
		  \Ref{wcElSetStyleColorNamed}(element, CSS_PROPERTY_BACKGROUND_COLOR, "Maroon", wcTRUE);
	}
	else if (event->type == HTML_EVENT_MOUSEUP){
		  \Ref{wcElSetStyleColorNamed}(element, CSS_PROPERTY_BACKGROUND_COLOR, "orange", wcTRUE);
	}
	else if (event->type == HTML_EVENT_CLICK)
	{
		char KeyVal = (char) \Ref{wcGetData}( Ctx, element);
		if (KeyVal == 'C')	{ // Clear
			LastOperator = 0;LastOperand = 0;ClearTextBox = 1;
		}
		if (ClearTextBox) { // Clear the text. Workaround ! putting nothing in the cells causes a resize, so display an invisible character in the background color.
			char Buffer[64];
			CalcStringCount=0;
			CalcStringVal[CalcStringCount]=0;
			rtp_sprintf(Buffer, "&lt;div style=\"color:lightblue;\"&gt;x&lt;div&gt;");
			\Ref{wcElSetInnerHtml}(CalcWindow, Buffer);
			ClearTextBox=0;
		}
		if (KeyVal == '+' || KeyVal == '-')	{ // Save the current operand and the operator, clear to start a new operand
			LastOperand = rtp_atol(CalcStringVal);
			ClearTextBox = 1; // Clear
			LastOperator = KeyVal;
		}
		else if (KeyVal == '=')	{ // Calculate and update.
			long Answer;
			if (LastOperator == '+') Answer = LastOperand +  rtp_atol(CalcStringVal);
			else if  (LastOperator == '-') Answer = LastOperand -  rtp_atol(CalcStringVal);
			else Answer = 0;
			LastOperand = Answer;
			rtp_sprintf(CalcStringVal, "%d", Answer);
			CalcStringCount = rtp_strlen(CalcStringVal);
			\Ref{wcElSetInnerHtml}(CalcWindow, CalcStringVal);
		}	else if (KeyVal == 'C')	;
		else{ // A digit was pressed. update the string which acts as our key accumulator
			CalcStringVal[CalcStringCount++] = KeyVal;
			CalcStringVal[CalcStringCount]=0;
			\Ref{wcElSetInnerHtml}(CalcWindow, CalcStringVal);
		}
	}
	return HTML_EVENT_STATUS_CONTINUE;
}

	</pre>
 */
static void ExCalculator(void)
{
	// Clear the window content and load the HTML and CSS calculator source in
	wcDocClear(&childwindowCtx, wcFALSE);
	if (wcDocOpen(&childwindowCtx)==0)
	{
		wcDocWriteHtml(&childwindowCtx, CalcHtmlSource, rtp_strlen(CalcHtmlSource),  wcTRUE);
		wcDocClose(&childwindowCtx);
	}
	// Start the app by enumerating through all the elements and setting event handlers.
	wcDocEnumerate(&childwindowCtx,CalcEnumeratorFunction, 0, 0);
	wcWinBringToTop(&childwindowCtx);
	wcCtxtRelease(&childwindowCtx); // Release any accumulated garbage.
}


// ======
static char *ButtonFaces[] = {"7","8","9","+", "4","5","6","-", "1","2","3","=","0","Clr",0};

/** @name ExCalculatorDynamic
   	@memo This example demonstrates mixing html and css source with 'C' source code to implement a simple functional calculator in 40 lines of CSS and HTML and < 100 lines of C.
<ul>
<li>Demonstrate using \Ref{wcDocEnumerate} to access elements in an html document.
</ul>

   	@memo This example demonstrates wcTableApi functions to implement a calculator function similar to \Ref{ExCalculator} using dynamically generated content instead of CSS and HTML.
<ul>
<li>Demonstrate using \Ref{wcNewStyledTable} \Ref{wcTableInsertRow} and \Ref{wcTableInsertCell} to dynamically create a table.
</ul>
 	@doc This source code provided in webcapiexample.c provides examples for creating create elements and display them with customized styles.
	<br> Note: childwindowCtx has already been initialized to a valid window context by \Ref{OpenExampleChildWindow}.
	\Ref{ExCalculator}
<pre>

static char *ButtonFaces[] = {"7","8","9","+", "4","5","6","-", "1","2","3","=","0","Clr",0};

static void ExCalculatorDynamic(void)
{
	wcEL r,c;
	wcEL CalcWindowRow;
	wcEL CalcTable;
	// Clear the window content.
	\Ref{wcDocClear}(&childwindowCtx, wcFALSE);
	// Create a table to contain the calculator
	CalcTable = \Ref{wcNewStyledTable}(&childwindowCtx, 0, wcTRUE,10, 20, 120, 200, 1,0,0,0,"steelgrey","lightblue",0,"font-size:14px;border-color:grey;");
	CalcWindowRow = \Ref{wcTableInsertRow}(&childwindowCtx, CalcTable, CSS_TEXT_ALIGN_CENTER, CSS_VERTICAL_ALIGN_MIDDLE, 0, 0, 0, 0);

	// Create a row spanning 4 collumns for holding calculator answers.
	CalcWindow = \Ref{wcTableInsertCell}(&childwindowCtx,CalcWindowRow,0,0,0, 0);
	\Ref{wcElSetInnerHtml}(CalcWindow,"0");
	\Ref{wcTableCellSetSpans}(&childwindowCtx,CalcWindow, 4, 1);

	// Create a 4 by 4 array of keys with rows spanning 1 collumn followed by a row with 2 keys each spanning 2 rows.
	for(int row=0;row<4;row++)
	{
		r = \Ref{wcTableInsertRow}(&childwindowCtx, CalcTable, CSS_TEXT_ALIGN_CENTER, CSS_VERTICAL_ALIGN_MIDDLE, 0, 0, 0, 0);
		for(int col=0;col<4;col++)
		{
			if (!ButtonFaces[row*4+col])
				break;
			c = \Ref{wcTableInsertCell}(&childwindowCtx,r,0, 0,0, "background-color:orange;width:40px;text-align:center;border:2px solid white;padding:2px;");
			\Ref{wcElSetInnerHtml}(c,ButtonFaces[row*4+col]);
			\Ref{wcElSetEventHandler}(c, CalcButtonCb); // Set an event handler
			\Ref{wcPushData}(&childwindowCtx, c, (void *) *ButtonFaces[row*4+col]);		// Store the keyvalue in private data so we don't have to get html source each time
			if (row==3)
				\Ref{wcTableCellSetSpans}(&childwindowCtx,c, 2, 1);
		}
	}

	// Demonstrate wcTableGetCell by printing the contents of the entries
	for(int row=0;row<5;row++)
	{
		for(int col=0;col<4;col++)
		{
			if (!ButtonFaces[row*4+col])
				break;
			c=\Ref{wcTableGetCell}(&childwindowCtx, CalcTable, row+1, col);
			if (c)
				rtp_printf("(%d,%d) --> %s\n", row, col, wcElGetInnerHtml(&childwindowCtx, c);
		}
	}


   	\Ref{wcDocRefresh}(&childwindowCtx);
	\Ref{wcWinBringToTop}(&childwindowCtx);
}

</pre>
 */

static void ExCalculatorDynamic(void)
{
	wcEL r,c;
	wcEL CalcWindowRow;
	wcEL CalcTable;
	// Clear the window content and load the HTML and CSS calculator source in
	wcDocClear(&childwindowCtx, wcFALSE);
	// Create a table to contain the calculator
	CalcTable = wcNewStyledTable(&childwindowCtx, 0, wcTRUE,10, 20, 120, 200, 1,0,0,0,"steelgrey","lightblue",0,"font-size:14px;border-color:grey;");
	CalcWindowRow = wcTableInsertRow(&childwindowCtx, CalcTable, CSS_TEXT_ALIGN_CENTER, CSS_VERTICAL_ALIGN_MIDDLE, 0, 0, 0, 0);
	// Create a row spanning 4 collumns for holding calculator answers.
	CalcWindow = wcTableInsertCell(&childwindowCtx,CalcWindowRow,0,0,0, 0);
	wcElSetInnerHtml(CalcWindow,"0");
	wcTableCellSetSpans(&childwindowCtx,CalcWindow, 4, 1);
	// Create a 4 by 4 array of keys with rows spanning 1 collumn followed by a row with 2 keys each spanning 2 rows.
	for(int row=0;row<4;row++)
	{
		r = wcTableInsertRow(&childwindowCtx, CalcTable, CSS_TEXT_ALIGN_CENTER, CSS_VERTICAL_ALIGN_MIDDLE, 0, 0, 0, 0);
		for(int col=0;col<4;col++)
		{
			if (!ButtonFaces[row*4+col])
				break;
			c = wcTableInsertCell(&childwindowCtx,r,0, 0,0, "background-color:orange;width:40px;text-align:center;border:2px solid white;padding:2px;");
			wcElSetInnerHtml(c,ButtonFaces[row*4+col]);
			wcPushData(&childwindowCtx, c, (void *) *ButtonFaces[row*4+col]);		// Store the keyvalue in private data so we don't have to get html source each time
			wcElSetEventHandler(c, CalcButtonCb); // Set an event handler
			if (row==3)
				wcTableCellSetSpans(&childwindowCtx,c, 2, 1);
		}
	}
	// Demonstrate wcTableGetCell by printing the contents of the entries
	rtp_printf("Table contents by row and column.\n");
	for(int row=0;row<5;row++)
	{
		for(int col=0;col<4;col++)
		{
			c=wcTableGetCell(&childwindowCtx, CalcTable, row+1, col);
			if (c)
				rtp_printf("(%d,%d) --> %s\n", row, col, wcElGetInnerHtml(&childwindowCtx, c));
		}
	}
   	wcDocRefresh(&childwindowCtx);
	wcWinBringToTop(&childwindowCtx);
	wcCtxtRelease(&childwindowCtx); // Release any accumulated garbage.
}




// This callback is called for ever element in the document
static wcBOOL CalcEnumeratorFunction(wcCtx* Ctx, wcEL El, int UserIntParm, void *UserVoidParm)
{
	if (wcElGetType(El) == HTML_TABLE_CELL_ELEMENT)
	{ // Read the inner HTML,
	 	char *p = wcElGetInnerHtml(Ctx, El);
		if (p)
		{
			if (rtp_strcmp(p, "|")==0) // '|' means it is our text window where we should places results.
				CalcWindow = El;
			else
			{
				wcElSetEventHandler(El, CalcButtonCb); // Set an event handler
				wcPushData(Ctx, El, (void *) *p);		// Store the keyvalue in private data so we don't have to get html source each time
			}
		}
	}
	return wcFALSE; // Keep enumerating
}


static HTMLEventStatus CalcButtonCb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
int initialExample = currentExample;
static int ClearTextBox = 0;
	if (!element)
		return HTML_EVENT_STATUS_CONTINUE;
	if (event->type == HTML_EVENT_MOUSEDOWN){ // Animate keypresses with a simple color change
		  //wcElClaimPointer(element);
		  wcElSetStyleColorNamed(element, CSS_PROPERTY_BACKGROUND_COLOR, "Maroon", wcTRUE);
	}
	else if (event->type == HTML_EVENT_MOUSEUP){
		  //w/cElReleasePointer(element);
		  wcElSetStyleColorNamed(element, CSS_PROPERTY_BACKGROUND_COLOR, "orange", wcTRUE);
	}
	else if (event->type == HTML_EVENT_CLICK)
	{
		char KeyVal = (char) ((unsigned long) wcGetData( Ctx, element));
		if (KeyVal == 'C')	{ // Clear
			LastOperator = 0;LastOperand = 0;ClearTextBox = 1;
		}
		if (ClearTextBox) { // Clear the text. Workaround ! putting nothing in the cells causes a resize, so display an invisible character in the background color.
			char Buffer[64];
			CalcStringCount=0;
			CalcStringVal[CalcStringCount]=0;
			rtp_sprintf(Buffer, "<div style=\"color:lightblue;\">x<div>");
			wcElSetInnerHtml(CalcWindow, Buffer);
			ClearTextBox=0;
		}
		if (KeyVal == '+' || KeyVal == '-')	{ // Save the current operand and the operator, clear to start a new operand
			LastOperand = rtp_atol(CalcStringVal);
			ClearTextBox = 1; // Clear
			LastOperator = KeyVal;
		}
		else if (KeyVal == '=')	{ // Calculate and update.
			long Answer;
			if (LastOperator == '+') Answer = LastOperand +  rtp_atol(CalcStringVal);
			else if  (LastOperator == '-') Answer = LastOperand -  rtp_atol(CalcStringVal);
			else Answer = 0;
			LastOperand = Answer;
			rtp_sprintf(CalcStringVal, "%d", Answer);
			CalcStringCount = rtp_strlen(CalcStringVal);
			wcElSetInnerHtml(CalcWindow, CalcStringVal);
		}	else if (KeyVal == 'C')	;
		else{ // A digit was pressed. update the string which acts as our key accumulator
			CalcStringVal[CalcStringCount++] = KeyVal;
			CalcStringVal[CalcStringCount]=0;
			wcElSetInnerHtml(CalcWindow, CalcStringVal);
		}
	}
	return HTML_EVENT_STATUS_CONTINUE;
}
// ====

#define TEXTAREATOP		4
#define TEXTAREALEFT	4
#define TEXTAREAHEIGHT  80
#define TEXTAREAWIDTH	200
#define TEXTAREAZ		0

#define TEXTEDITTOP		100
#define TEXTEDITLEFT	20
#define TEXTEDITHEIGHT  24
#define TEXTEDITWIDTH	200
#define TEXTEDITZ		0
#define SLIDESHOWDELAY 2000
static void ExPresentationSlideShowCB(int Sequence, void *UserVoidParm);
static void *LastAllocateElementArray;	// Use this to guard against executing a timer for a stale session
/** @name ExPresentationSlideShow
   	@memo Using pure dynamic HTML from 'C' create elements and display them using varying style changes.
 	@doc This source code provided in webcapiexample.c provides examples for creating create elements and display them with customized styles.
	<br> Note: childwindowCtx has already been initialized to a valid window context by \Ref{OpenExampleChildWindow}.
 	<pre>

#define TEXTAREATOP		4
#define TEXTAREALEFT	4
#define TEXTAREAHEIGHT  80
#define TEXTAREAWIDTH	200
#define TEXTAREAZ		0

#define TEXTEDITTOP		100
#define TEXTEDITLEFT	20
#define TEXTEDITHEIGHT  24
#define TEXTEDITWIDTH	200
#define TEXTEDITZ		0
#define SLIDESHOWDELAY 2000
static void ExPresentationSlideShowCB(int Sequence, void *UserVoidParm);
static void *LastAllocateElementArray;	// Use this to guard against executing a timer for a stale session


static void ExPresentationSlideShow(void)
{
int width, height;
	// Clear the window content
	\Ref{wcDocClear}(&childwindowCtx, wcFALSE);
	// Create some local storage so we can pass argument between timeout callback handling sequences 100 should be way more than enough
	wcEL *ElementArray = (wcEL  *) WEBC_MALLOC(sizeof(wcEL) * 100);
	rtp_memset(ElementArray, 0, sizeof(wcEL) * 10);
	LastAllocateElementArray=(void *) ElementArray; // Use this as a gaurd against processing stale timers.
	// Get the Canvas size and insert a transparent div that we will insert and remove elements from we work. Use ElementArray[0] as the handle for it.
	\Ref{wcWinGetPos}(&childwindowCtx,0, 0, &width, &height);
	ElementArray[0] = \Ref{wcNewStyledDiv(&childwindowCtx, 0, wcTRUE,0, TEXTAREALEFT, height, width,-1, 0, 0, 0, "overflow:hidden;");


	\Ref{wcWinBringToTop(&childwindowCtx);
	// Start the show imediately at sequence 0
	\Ref{wcTimedCallback}(ExPresentationSlideShowCB, 0, 0, (void *)ElementArray);

}

// Tools to remove elements from the outer div and delete them.
static void DeleteElementFromSlideShow(wcEL *ElementArray, int ElementNumber)
{
	\Ref{wcElRemoveChild}(ElementArray[0], ElementArray[ElementNumber]);
	\Ref{wcElDelete}(ElementArray[ElementNumber]);
	ElementArray[ElementNumber] = 0;
}
static void DeleteAllElementsFromSlideShow(wcEL *ElementArray)
{
int ElementNumber;
	for (ElementNumber=1;ElementArray[ElementNumber]; ElementNumber++)
		DeleteElementFromSlideShow(ElementArray, ElementNumber);
}
static int SubStateNumber; // For executing substates to keep the number of conditions down
static void ExPresentationSlideShowCB(int Sequence, void *UserVoidParm)
{
wcEL *ElementArray = (wcEL  *) UserVoidParm;
int nextShowDelay = SLIDESHOWDELAY; // Change nextShowDelay to speed up or slow down a particular slide

	// We are called by a system timer that doesn't know about our window so check if the user press left or right an reloaded since the timer was armed.
	// If we proceeded we would crash because many the elements were cleared from the document.
	if (Examples[currentExample].ExampleFunction != ExPresentationSlideShow || LastAllocateElementArray!=UserVoidParm)
	{
		WEBC_FREE(ElementArray);
		return;
	}
	// Cycle through the sequence numbers showing display features
	//
	//
	switch (Sequence) {
	default: // Out of tests. just exit
		WEBC_FREE(ElementArray);
		return;
	case 0:  // Dynamically create controls
		UPDATECONSOLETEXT("Create simple controls");
		ElementArray[1]=\Ref{wcNewStyledTextArea}(&childwindowCtx, ElementArray[0], wcTRUE, TEXTAREATOP, TEXTAREALEFT, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ, 0, 0,0, 0);
		ElementArray[2]=\Ref{wcNewStyledTextEdit}(&childwindowCtx, ElementArray[0], wcTRUE, TEXTEDITTOP, TEXTEDITLEFT, TEXTEDITHEIGHT, TEXTEDITWIDTH,TEXTEDITZ, 0, 0,0, 0);
		\Ref{wcElSetValue}(ElementArray[1], "Text area content\n goes here. \n  A. Test a.\n  B.Test b.");
		\Ref{wcElSetValue}(ElementArray[2], "Text edit content goes here");
		break;
	case 1: // Show colors
		DeleteAllElementsFromSlideShow(ElementArray);
		UPDATECONSOLETEXT("Show text elements using colors");
		ElementArray[1]=\Ref{wcNewStyledTextArea}(&childwindowCtx, ElementArray[0], wcTRUE, TEXTAREATOP, TEXTAREALEFT, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ, "Green", "DarkTurquoise", 0, "border-style:none;");
		ElementArray[2]=\Ref{wcNewStyledTextEdit}(&childwindowCtx, ElementArray[0], wcTRUE, TEXTEDITTOP, TEXTEDITLEFT, TEXTEDITHEIGHT, TEXTEDITWIDTH,TEXTEDITZ, "Black", "LightSalmon",0, "border-style:none;");
		\Ref{wcElSetValue}(ElementArray[1], "Text area content\n goes here. \n  A. Test a.\n  B.Test b.");
		\Ref{wcElSetValue}(ElementArray[2], "Text edit content goes here");
		break;
	case 2: // Show Borders
		DeleteAllElementsFromSlideShow(ElementArray);
		UPDATECONSOLETEXT("Show text elements using borders");
		ElementArray[1]=\Ref{wcNewStyledTextArea}(&childwindowCtx, ElementArray[0], wcTRUE, TEXTAREATOP, TEXTAREALEFT, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ, "Green", "DarkTurquoise", 0, "border-style:solid;border-width:4px;border-color:Aqua;");
		ElementArray[2]=\Ref{wcNewStyledTextEdit}(&childwindowCtx, ElementArray[0], wcTRUE, TEXTEDITTOP, TEXTEDITLEFT, TEXTEDITHEIGHT, TEXTEDITWIDTH,TEXTEDITZ, "Black", "LightSalmon",0, "border-style:solid;border-width:2px;border-color:Black;");
		\Ref{wcElSetValue}(ElementArray[1], "Text area content\n goes here. \n  A. Test a.\n  B.Test b.");
		\Ref{wcElSetValue}(ElementArray[2], "Text edit content goes here");
		break;
	case 3: // Delete controls
		UPDATECONSOLETEXT("Remove a control");
		DeleteElementFromSlideShow(ElementArray, 2);
		break;
	case 4:
		UPDATECONSOLETEXT("Remove another control");
		DeleteElementFromSlideShow(ElementArray, 1);
		break;
	case 5: // Show background images
		DeleteAllElementsFromSlideShow(ElementArray);
		UPDATECONSOLETEXT("Divs and table support background images.");
		ElementArray[1]=\Ref{wcNewStyledDiv}(&childwindowCtx, ElementArray[0], wcTRUE,TEXTAREATOP, TEXTAREALEFT, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ-1, 0, "DarkSlateBlue", EXBKIMAGE0, "overflow:hidden;");
		break;
	case 6: // Draw text with a transparent background over a background image
		UPDATECONSOLETEXT("Text area backgound image style not supported but use a workaround drawing with a transparent background over a div with a background image.");
		ElementArray[2]=\Ref{wcNewStyledTextArea}(&childwindowCtx, ElementArray[0], wcTRUE, TEXTAREATOP, TEXTAREALEFT, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ, "Maroon", "Transparent", 0, 0);
		\Ref{wcElSetValue}(ElementArray[2], "Text area backgound image style is not supported but there is a workaround using a transparent background over a div with a background image.");
		break;
	case 7: // Set up elements to show alpha blending effects on foreground and background
		DeleteAllElementsFromSlideShow(ElementArray);
		UPDATECONSOLETEXT("Alpha channel support for foreground colors.");
		ElementArray[1]=\Ref{wcNewStyledDiv}(&childwindowCtx, ElementArray[0], wcTRUE,TEXTAREATOP, TEXTAREALEFT, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ-1, "black", "rgba(128,128,128,0)", 0, "overflow:hidden;");
		\Ref{wcElSetInnerHtml}(ElementArray[1], "Text text &lt;br&gt; text text ..");
		ElementArray[2]=\Ref{wcNewStyledDiv}(&childwindowCtx, ElementArray[0], wcTRUE,TEXTAREATOP+TEXTAREAHEIGHT/2, TEXTAREALEFT+TEXTAREAWIDTH/2, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ-1, "rgba(0,0,0,.1)", "rgba(128,128,128,1)", 0, "overflow:hidden;");
		\Ref{wcElSetInnerHtml}(ElementArray[2], "&lt;big&gt;&lt;b&gt;Text Fading &lt;br&gt;in now !!&lt;/b&gt;"); // Show some context, we'll fade in the next sequence
		SubStateNumber=0;
		break;
	case 8: // Show alpha blending effects on foreground
		if (SubStateNumber==0)	{ // Vary substate number from 10 to 100 and use it as the alpha value for the Text color
			SubStateNumber = 10;
			UPDATECONSOLETEXT("Alpha blending is supported for foreground colors.");
		}
		SubStateNumber += 5;
		if (SubStateNumber&gt;100)	{
			SubStateNumber=0;
			\Ref{wcElSetInnerHtml}(ElementArray[2], "&lt;big&gt;&lt;b&gt;Backround&lt;br&gt;fade now !!&lt;/b&gt;"); // Prepare for background fad
			\Ref{wcElSetStyleColorRgba}(ElementArray[2],CSS_PROPERTY_COLOR, 0, 0, 0, 0, wcFALSE); // Black text, opaque
			\Ref{wcElSetStyleColorRgba}(ElementArray[2],CSS_PROPERTY_BACKGROUND_COLOR, 128, 128, 128, 90, wcTRUE); // Fully opaque background
		}
		else {
			\Ref{wcElSetStyleColorRgba}(ElementArray[2],CSS_PROPERTY_COLOR, 0, 0, 0, SubStateNumber, wcTRUE);
			\Ref{wcElSetInnerHtml}(ElementArray[2], "&lt;big&gt;&lt;b&gt;Tex Fading &lt;br&gt;in now !!&lt;/b&gt;"); // Workaround, must change content for new alpha on text to take affect
			\Ref{wcTimedCallback}(ExPresentationSlideShowCB, 200, Sequence, (void *)ElementArray);
			return;
		}
		break;
	case 9: // Show alpha blending effects on background
		if (SubStateNumber==0)	{ // Vary substate number from 10 to 100 and use it as the alpha value for the background color
			SubStateNumber = 10;
			UPDATECONSOLETEXT("Alpha blending is supported for background colors.");
		}
		else
			SubStateNumber += 10;
		if (SubStateNumber==100)
		{
			SubStateNumber=0;
			break;
		}

		\Ref{wcElSetStyleColorRgba}(ElementArray[2],CSS_PROPERTY_BACKGROUND_COLOR, 128, 128, 128, 100-SubStateNumber, wcTRUE);
		\Ref{wcTimedCallback}(ExPresentationSlideShowCB, 200, Sequence, (void *)ElementArray);
		return;
	case 10: // Show fade in special effects using the opacity feature of divs
		SubStateNumber=0;
		DeleteAllElementsFromSlideShow(ElementArray);
		UPDATECONSOLETEXT("Create fade in special effects in a few lines of code");
		// Create a set of simple messages and images inside divs that we will fade in and out.
		// Any content that can go in a div (that's anything) can be faded in and out.
		ElementArray[1]=\Ref{wcNewStyledDiv}(&childwindowCtx, ElementArray[0], wcTRUE,40, 80, 180, 200,TEXTAREAZ-1, 0, 0, EXBKIMAGE0, "overflow:hidden;");
		ElementArray[2]=\Ref{wcNewStyledDiv}(&childwindowCtx, ElementArray[0], wcTRUE,10, 10,  TEXTAREAHEIGHT*2, TEXTAREAWIDTH,TEXTAREAZ, "RoyalBlue", "Transparent", 0, "overflow:hidden;opacity:0;");
		\Ref{wcElSetInnerHtml}(ElementArray[2], "&lt;big&gt;&lt;b&gt;Make a &lt;br&gt;Dramatic entrance&lt;br&gt;With little code !!&lt;/b&gt;"); // Show some context, we'll fade in the next sequence
		ElementArray[3]=\Ref{wcNewStyledDiv}(&childwindowCtx, ElementArray[0], wcTRUE, 0, 120,  100, 100,TEXTAREAZ, "blue", "Feldspar", 0, "overflow:hidden;opacity:0;");
		\Ref{wcElSetInnerHtml}(ElementArray[3], "&lt;big&gt;&lt;b&gt;Create fade in effects."); // Show some context, we'll fade in the next sequence
		ElementArray[4]=\Ref{wcNewStyledDiv}(&childwindowCtx, ElementArray[0], wcTRUE, 80, 20,  80, 200,TEXTAREAZ, "Red", "SeaShell", 0, "overflow:hidden;opacity:0;");
		\Ref{wcElSetInnerHtml}(ElementArray[4], "Fade any element type. &lt;input type=text value=\"Like Buttons\"&gt;"); // Show some context, we'll fade in the next sequence
		ElementArray[5]=\Ref{wcNewStyledDiv}(&childwindowCtx, ElementArray[0], wcTRUE, 0, 0,  TEXTAREAHEIGHT, 200,TEXTAREAWIDTH, "Blue", "SeaShell", 0, "overflow:hidden;opacity:0;");
		\Ref{wcElSetInnerHtml}(ElementArray[5], "And Images.&lt;br&gt;&lt;img src=\"webc:ebslogo.png\"&gt;"); // Show some context, we'll fade in the next sequence
		ElementArray[6]=\Ref{wcNewStyledDiv}(&childwindowCtx, ElementArray[0], wcTRUE, 30, 60,  160, 200,TEXTAREAWIDTH, "Blue", "SeaShell", 0, "overflow:hidden;opacity:0;");
		\Ref{wcElSetInnerHtml}(ElementArray[6], "&lt;big&gt;&lt;b&gt;Click Left or&lt;br&gt;Right buttons&lt;br&gt;To run other&lt;br&gt;examples.&lt;/b&gt;");
		break;
	case 11: // Animate the messages created in the previous state. contains multiple substates.
		{
		SubStateNumber += 10;
		if (SubStateNumber&lt;=100)
		{ // Fade the first message in from the top left corner
			\Ref{wcElSetStyleDim}(ElementArray[2], SubStateNumber*2, SubStateNumber*2, wcTRUE); // Change height and width
			\Ref{wcElSetStyleOpacity}(ElementArray[2], SubStateNumber, wcFALSE);	// Change opacity
		}
		else if (SubStateNumber&lt;=200) // Animate the top left
			\Ref{wcElSetStyleOpacity}(ElementArray[3], SubStateNumber-100, wcFALSE);
		else if (SubStateNumber&lt;=300) // Fade the message out
			\Ref{wcElSetStyleOpacity}(ElementArray[3], 100 - (SubStateNumber-200), wcFALSE);
		else if (SubStateNumber&lt;=400) // Animate the next message
			\Ref{wcElSetStyleOpacity}(ElementArray[4], SubStateNumber-300, wcFALSE);
		else if (SubStateNumber&lt;=500) // Fade the message out
			\Ref{wcElSetStyleOpacity}(ElementArray[4], 100 - (SubStateNumber-400), wcFALSE);
		else if (SubStateNumber&lt;=600) // Animate the next message
			\Ref{wcElSetStyleOpacity}(ElementArray[5], SubStateNumber-500, wcFALSE);
		else if (SubStateNumber&lt;=700) // Fade the message out
			\Ref{wcElSetStyleOpacity}(ElementArray[5], 100 - (SubStateNumber-600), wcFALSE);
		else if (SubStateNumber&lt;=800) // Fade the first message out
		{
			\Ref{wcElSetStyleOpacity}(ElementArray[2], 100 - (SubStateNumber-700), wcFALSE);
		}
		else if (SubStateNumber&lt;=900)
		{ // We are done, go to a mode fading a prompt message in and out
			\Ref{wcElSetStyleOpacity}(ElementArray[6], 100 - (SubStateNumber-800), wcFALSE);
		}
		else if (SubStateNumber&lt;=1000)
		{  // Fade the message in and loop back to fading out
			\Ref{wcElSetStyleOpacity}(ElementArray[6], SubStateNumber-900, wcFALSE);
			if (SubStateNumber==1000)
			{
				SubStateNumber=810;
				\Ref{wcTimedCallback}(ExPresentationSlideShowCB, 1400, Sequence, (void *)ElementArray);
				return;
			}
		}
		else
			break; // done with this sequence
		if (SubStateNumber&gt;100 && (SubStateNumber%100) == 0) // Pause longer on text fadein finishes for affect
			\Ref{wcTimedCallback}(ExPresentationSlideShowCB, 1400, Sequence, (void *)ElementArray);
		else
			\Ref{wcTimedCallback}(ExPresentationSlideShowCB, 200, Sequence, (void *)ElementArray);
		return;	  // Stay in this case and cycle the substate after our timeer expires
		}
	}
	\Ref{wcTimedCallback}(ExPresentationSlideShowCB, SLIDESHOWDELAY, ++Sequence, (void *)ElementArray);
}
	</pre>
 */
static void ExPresentationSlideShow(void)
{
int width, height;
	// Clear the window content
	wcDocClear(&childwindowCtx, wcFALSE);
	wcNewStyledDiv(&childwindowCtx, 0, wcTRUE,0, TEXTAREALEFT, MAIN_CANVAS_WIDTH, CHILD_WINDOW_HEIGHT,-2, 0, 0, BODYBKIMAGE0, "overflow:hidden;");

	// Create some local storage so we can pass argument between timeout callback handling sequences 100 should be way more than enough
	wcEL *ElementArray = (wcEL  *) WEBC_MALLOC(sizeof(wcEL) * 100);
	rtp_memset(ElementArray, 0, sizeof(wcEL) * 10);
	LastAllocateElementArray=(void *) ElementArray; // Use this as a gaurd against processing stale timers.
	// Get the Canvas size and insert a transparent div that we will insert and remove elements from we work. Use ElementArray[0] as the handle for it.
	wcWinGetPos(&childwindowCtx,0, 0, &width, &height);
	ElementArray[0] = wcNewStyledDiv(&childwindowCtx, 0, wcTRUE,0, TEXTAREALEFT, height, width,-1, 0, 0, 0, "overflow:hidden;");


	wcWinBringToTop(&childwindowCtx);
	wcCtxtRelease(&childwindowCtx); // Release any accumulated garbage.
	// Start the show imediately at sequence 0
	wcTimedCallback(ExPresentationSlideShowCB, 0, 0, (void *)ElementArray);

}

// Tools to remove elements from the outer div and delete them.
static void DeleteElementFromSlideShow(wcEL *ElementArray, int ElementNumber)
{
	wcElRemoveChild(ElementArray[0], ElementArray[ElementNumber],wcFALSE);
	wcElDelete(ElementArray[ElementNumber], wcTRUE);
	ElementArray[ElementNumber] = 0;
}
static void DeleteAllElementsFromSlideShow(wcEL *ElementArray)
{
int ElementNumber;
	for (ElementNumber=1;ElementArray[ElementNumber]; ElementNumber++)
		DeleteElementFromSlideShow(ElementArray, ElementNumber);
}
static int SubStateNumber; // For executing substates to keep the number of conditions down
static void ExPresentationSlideShowCB(int Sequence, void *UserVoidParm)
{
wcEL *ElementArray = (wcEL  *) UserVoidParm;
int nextShowDelay = SLIDESHOWDELAY; // Change nextShowDelay to speed up or slow down a particular slide

	// We are called by a system timer that doesn't know about our window so check if the user press left or right an reloaded since the timer was armed.
	// If we proceeded we would crash because many the elements were cleared from the document.
	if (Examples[currentExample].ExampleFunction != ExPresentationSlideShow || LastAllocateElementArray!=UserVoidParm)
	{
		WEBC_FREE(ElementArray);
		return;
	}
	// Cycle through the sequence numbers showing display features
	//
	//
	switch (Sequence) {
	default: // Out of tests. just exit
		UPDATECONSOLETEXT("Slideshow complete - Click NEXT to move to next example.");
		WEBC_FREE(ElementArray);
		return;
	case 0:  // Dynamically create controls
		UPDATECONSOLETEXT("Slideshow - Create simple controls");
		ElementArray[1]=wcNewStyledTextArea(&childwindowCtx, ElementArray[0], wcTRUE, TEXTAREATOP, TEXTAREALEFT, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ, 0, 0,0, 0);
		ElementArray[2]=wcNewStyledTextEdit(&childwindowCtx, ElementArray[0], wcTRUE, TEXTEDITTOP, TEXTEDITLEFT, TEXTEDITHEIGHT, TEXTEDITWIDTH,TEXTEDITZ, 0, 0,0, 0);
		wcElSetValue(ElementArray[1], "Text area content\n goes here. \n  A. Test a.\n  B.Test b.");
		wcElSetValue(ElementArray[2], "Text edit content goes here");
		break;
	case 1: // Show colors
		DeleteAllElementsFromSlideShow(ElementArray);
		UPDATECONSOLETEXT("Slideshow - Show text elements using colors");
		ElementArray[1]=wcNewStyledTextArea(&childwindowCtx, ElementArray[0], wcTRUE, TEXTAREATOP, TEXTAREALEFT, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ, "Green", "DarkTurquoise", 0, "border-style:none;");
		ElementArray[2]=wcNewStyledTextEdit(&childwindowCtx, ElementArray[0], wcTRUE, TEXTEDITTOP, TEXTEDITLEFT, TEXTEDITHEIGHT, TEXTEDITWIDTH,TEXTEDITZ, "Black", "LightSalmon",0, "border-style:none;");
		wcElSetValue(ElementArray[1], "Text area content\n goes here. \n  A. Test a.\n  B.Test b.");
		wcElSetValue(ElementArray[2], "Text edit content goes here");
		break;
	case 2: // Show Borders
		DeleteAllElementsFromSlideShow(ElementArray);
		UPDATECONSOLETEXT("Slideshow - Show text elements using borders");
		ElementArray[1]=wcNewStyledTextArea(&childwindowCtx, ElementArray[0], wcTRUE, TEXTAREATOP, TEXTAREALEFT, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ, "Green", "DarkTurquoise", 0, "border-style:solid;border-width:4px;border-color:Aqua;");
		ElementArray[2]=wcNewStyledTextEdit(&childwindowCtx, ElementArray[0], wcTRUE, TEXTEDITTOP, TEXTEDITLEFT, TEXTEDITHEIGHT, TEXTEDITWIDTH,TEXTEDITZ, "Black", "LightSalmon",0, "border-style:solid;border-width:2px;border-color:Black;");
		wcElSetValue(ElementArray[1], "Text area content\n goes here. \n  A. Test a.\n  B.Test b.");
		wcElSetValue(ElementArray[2], "Text edit content goes here");
		break;
	case 3: // Delete controls
		UPDATECONSOLETEXT("Slideshow - Remove a control");
		DeleteElementFromSlideShow(ElementArray, 2);
		break;
	case 4:
		UPDATECONSOLETEXT("Slideshow - Remove another control");
		DeleteElementFromSlideShow(ElementArray, 1);
		break;
	case 5: // Show background images
		DeleteAllElementsFromSlideShow(ElementArray);
		UPDATECONSOLETEXT("Slideshow - Divs and table support background images.");
		ElementArray[1]=wcNewStyledDiv(&childwindowCtx, ElementArray[0], wcTRUE,TEXTAREATOP, TEXTAREALEFT, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ-1, 0, "DarkSlateBlue", FADEINIMAGE, "overflow:hidden;");
		break;
	case 6: // Draw text with a transparent background over a background image
		UPDATECONSOLETEXT("Slideshow - Text area backgound image style not supported but use a workaround drawing with a transparent background over a div with a background image.");
		ElementArray[2]=wcNewStyledTextArea(&childwindowCtx, ElementArray[0], wcTRUE, TEXTAREATOP, TEXTAREALEFT, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ, "Maroon", "Transparent", 0, 0);
		wcElSetValue(ElementArray[2], "Text area backgound image style is not supported but there is a workaround using a transparent background over a div with a background image.");
		break;
	case 7: // Set up elements to show alpha blending effects on foreground and background
		DeleteAllElementsFromSlideShow(ElementArray);
		UPDATECONSOLETEXT("Slideshow - Alpha channel support for foreground colors.");
		ElementArray[1]=wcNewStyledDiv(&childwindowCtx, ElementArray[0], wcTRUE,TEXTAREATOP, TEXTAREALEFT, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ-1, "black", "rgba(128,128,128,0)", 0, "overflow:hidden;");
		wcElSetInnerHtml(ElementArray[1], "Text text <br> text text ..");
		ElementArray[2]=wcNewStyledDiv(&childwindowCtx, ElementArray[0], wcTRUE,TEXTAREATOP+TEXTAREAHEIGHT/2, TEXTAREALEFT+TEXTAREAWIDTH/2, TEXTAREAHEIGHT, TEXTAREAWIDTH,TEXTAREAZ-1, "rgba(0,0,0,.1)", "rgba(128,128,128,1)", 0, "overflow:hidden;");
		wcElSetInnerHtml(ElementArray[2], "<big><b>Text Fading <br>in now !!</b>"); // Show some context, we'll fade in the next sequence
		SubStateNumber=0;
		break;
	case 8: // Show alpha blending effects on foreground
		if (SubStateNumber==0)	{ // Vary substate number from 10 to 100 and use it as the alpha value for the Text color
			SubStateNumber = 10;
			UPDATECONSOLETEXT("Slideshow - Alpha blending is supported for foreground colors.");
		}
		SubStateNumber += 5;
		if (SubStateNumber>100)	{
			SubStateNumber=0;
			wcElSetInnerHtml(ElementArray[2], "<big><b>Backround<br>fade now !!</b>"); // Prepare for background fad
			wcElSetStyleColorRgba(ElementArray[2],CSS_PROPERTY_COLOR, 0, 0, 0, 0, wcFALSE); // Black text, opaque
			wcElSetStyleColorRgba(ElementArray[2],CSS_PROPERTY_BACKGROUND_COLOR, 128, 128, 128, 90, wcTRUE); // Fully opaque background
		}
		else {
			wcElSetStyleColorRgba(ElementArray[2],CSS_PROPERTY_COLOR, 0, 0, 0, SubStateNumber, wcTRUE);
			wcElSetInnerHtml(ElementArray[2], "<big><b>Tex Fading <br>in now !!</b>"); // Workaround, must change content for new alpha on text to take affect
			wcTimedCallback(ExPresentationSlideShowCB, 200, Sequence, (void *)ElementArray);
			return;
		}
		break;
	case 9: // Show alpha blending effects on background
		if (SubStateNumber==0)	{ // Vary substate number from 10 to 100 and use it as the alpha value for the background color
			SubStateNumber = 10;
			UPDATECONSOLETEXT("Slideshow - Alpha blending is supported for background colors.");
		}
		else
			SubStateNumber += 10;
		if (SubStateNumber==100)
		{
			SubStateNumber=0;
			break;
		}

		wcElSetStyleColorRgba(ElementArray[2],CSS_PROPERTY_BACKGROUND_COLOR, 128, 128, 128, 100-SubStateNumber, wcTRUE);
		wcTimedCallback(ExPresentationSlideShowCB, 200, Sequence, (void *)ElementArray);
		return;
	case 10: // Show fade in special effects using the opacity feature of divs
		SubStateNumber=0;
		DeleteAllElementsFromSlideShow(ElementArray);
		UPDATECONSOLETEXT("Slideshow - Create fade in special effects in a few lines of code");
		// Create a set of simple messages and images inside divs that we will fade in and out.
		// Any content that can go in a div (that's anything) can be faded in and out.
		ElementArray[1]=wcNewStyledDiv(&childwindowCtx, ElementArray[0], wcTRUE,10, 80, 180, 200,TEXTAREAZ-1, 0, 0, FLAMEIMAGE, "overflow:hidden;");
		ElementArray[2]=wcNewStyledDiv(&childwindowCtx, ElementArray[0], wcTRUE,10, 10,  TEXTAREAHEIGHT*2, TEXTAREAWIDTH,TEXTAREAZ, "RoyalBlue", "Transparent", 0, "overflow:hidden;opacity:0;");
		wcElSetInnerHtml(ElementArray[2], "<big><b>Make a <br>Dramatic entrance<br>With little code !!</b>"); // Show some context, we'll fade in the next sequence
		ElementArray[3]=wcNewStyledDiv(&childwindowCtx, ElementArray[0], wcTRUE, 0, 120,  100, 100,TEXTAREAZ, "blue", "Transparent", 0, "overflow:hidden;opacity:0;");
		wcElSetInnerHtml(ElementArray[3], "<big><b>Create fade in effects."); // Show some context, we'll fade in the next sequence
		ElementArray[4]=wcNewStyledDiv(&childwindowCtx, ElementArray[0], wcTRUE, 80, 20,  80, 200,TEXTAREAZ, "Red", "SeaShell", 0, "overflow:hidden;opacity:0;");
		wcElSetInnerHtml(ElementArray[4], "Fade any element type. <input type=text value=\"Like Buttons\">"); // Show some context, we'll fade in the next sequence
		ElementArray[5]=wcNewStyledDiv(&childwindowCtx, ElementArray[0], wcTRUE, 0, 0,  TEXTAREAHEIGHT, 200,TEXTAREAWIDTH, "Blue", "SeaShell", 0, "overflow:hidden;opacity:0;");
		wcElSetInnerHtml(ElementArray[5], "And Images.<br><img src=\"webc:/Example/Images/MoonRover.jpg\">"); // Show some context, we'll fade in the next sequence
		ElementArray[6]=wcNewStyledDiv(&childwindowCtx, ElementArray[0], wcTRUE, 30, 70,  160, 200,TEXTAREAWIDTH, "White", "Transparent", 0, "overflow:hidden;opacity:0;");
		wcElSetInnerHtml(ElementArray[6], "<big><b>Click Left or<br>Right buttons<br>To run other<br>examples.</b>");
		break;
	case 11: // Animate the messages created in the previous state. contains multiple substates.
		{
		SubStateNumber += 10;
		if (SubStateNumber<=100)
		{ // Fade the first message in from the top left corner
			wcElSetStyleDim(ElementArray[2], SubStateNumber*2, SubStateNumber*2, wcTRUE); // Change height and width
			wcElSetStyleOpacity(ElementArray[2], SubStateNumber, wcFALSE);	// Change opacity
		}
		else if (SubStateNumber<=200) // Fade the first message out
		{
			wcElSetStyleOpacity(ElementArray[2], 100 - (SubStateNumber-100), wcFALSE);
		}
		else if (SubStateNumber<=300) // Animate the top left
			wcElSetStyleOpacity(ElementArray[3], SubStateNumber-200, wcFALSE);
		else if (SubStateNumber<=400) // Fade the message out
			wcElSetStyleOpacity(ElementArray[3], 100 - (SubStateNumber-300), wcFALSE);
		else if (SubStateNumber<=500) // Animate the next message
			wcElSetStyleOpacity(ElementArray[4], SubStateNumber-400, wcFALSE);
		else if (SubStateNumber<=600) // Fade the message out
			wcElSetStyleOpacity(ElementArray[4], 100 - (SubStateNumber-500), wcFALSE);
		else if (SubStateNumber<=700) // Animate the next message
			wcElSetStyleOpacity(ElementArray[5], SubStateNumber-600, wcFALSE);
		else if (SubStateNumber<=800) // Fade the message out
			wcElSetStyleOpacity(ElementArray[5], 100 - (SubStateNumber-700), wcFALSE);
		else if (SubStateNumber<=900)
		{ // We are done, go to a mode fading a prompt message in and out
			wcElSetStyleOpacity(ElementArray[6], 100 - (SubStateNumber-800), wcFALSE);
		}
		else if (SubStateNumber<=1000)
		{  // Fade the message in and loop back to fading out
			wcElSetStyleOpacity(ElementArray[6], SubStateNumber-900, wcFALSE);
			if (SubStateNumber==1000)
			{
				SubStateNumber=810;
				wcTimedCallback(ExPresentationSlideShowCB, 1400, Sequence, (void *)ElementArray);
				return;
			}
		}
		else
			break; // done with this sequence
		if (SubStateNumber>100 && (SubStateNumber%100) == 0) // Pause longer on text fadein finishes for affect
			wcTimedCallback(ExPresentationSlideShowCB, 1400, Sequence, (void *)ElementArray);
		else
			wcTimedCallback(ExPresentationSlideShowCB, 200, Sequence, (void *)ElementArray);
		return;	  // Stay in this case and cycle the substate after our timeer expires
		}
	}
	wcTimedCallback(ExPresentationSlideShowCB, SLIDESHOWDELAY, ++Sequence, (void *)ElementArray);
}


//#include "ExampleContentfiletable.h"

/** @name ExDemonstrateWebcVirtualFiles
   	@memo This example registers precompiled content with the read only file system layer and loads content from it.
 	@doc This source code provided in webcapiexample.c provides examples for registering precompiled content with the read read only file system layer and loading content from it.
	The content was compiled from the webc\ExampleContent subdirectory using \Ref{makewebcfs} which is driven by the script webc\ExampleContent\CompileContent.bat.
	<br> Note: childwindowCtx has already been initialized to a valid window context by \Ref{OpenExampleChildWindow}.
 	<pre>

// Include the files we generated with \Ref{makewebcfs}
#include "ExampleContentfiletable.h"

static void ExDemonstrateWebcVirtualFiles(void)
{

	UPDATECONSOLETEXT("Installing precompiled sample content into the webC rom file system. The content should be displayed above. Click to navigate the compiled in content.");
	// ExampleContent was generated with the command ..\bin\makewebcfs -o=..\source\webcapiexamples -n=ExampleContent html
	// The statement above "#include "ExampleContentfiletable.h" declared the symbol named ExampleContent which we now register with webc:
	\Ref{wcAddFsTree}(ExampleContent);
	// Now we should be able to access the data
	\Ref{wcDocSetUrl}(&childwindowCtx, "webc:///Example/Index.html", wcTRUE);
	\Ref{wcWinBringToTop}(&childwindowCtx);
}
	</pre>
 */
static void ExDemonstrateWebcVirtualFiles(void)
{

	UPDATECONSOLETEXT("Installing precompiled sample content into the webC rom file system. The content should be displayed above. Click to navigate the compiled in content.");
	// ExampleContent was generated with the command ..\bin\makewebcfs -o=..\source\webcapiexamples -n=ExampleContent html
	// The stateent above "#include "ExampleContentfiletable.h" declared the symbol ExampleContent which we now register with webc:
	wcAddFsTree(ExampleContent);
	wcDocSetUrl(&childwindowCtx, "webc:///Example/Index.html", wcTRUE);
	wcWinBringToTop(&childwindowCtx);
	// wcDocClear(&childwindowCtx, wcFALSE);
}


// Declarations for resize box demo. A lot of statics but it's only a demo.
static wcEL gResizeFrame, gResizeBox;
static int gLastx, gLasty, gActivestation, gActiveIcon, gResizing,gDragIfTimeExpires,gDragingWindow;
#define RESIZEFRAMEZ	5
#define RESIZECURSORZ   7
#define RESIZEBOXZ		6
#define RESIZEFRAMEMARGIN 10
#define RESIZEBOXWIDTH MAIN_CANVAS_WIDTH/2
#define RESIZEBOXHEIGHT CHILD_CANVAS_HEIGHT/2
#define RESIZEFRAMEWIDTH  (RESIZEBOXWIDTH+RESIZEFRAMEMARGIN*2)
#define RESIZEFRAMEHEIGHT (RESIZEBOXHEIGHT+RESIZEFRAMEMARGIN*2)
#define RESIZEFRAMEINITIALLEFT (MAIN_CANVAS_WIDTH-RESIZEFRAMEWIDTH)/2
#define RESIZEFRAMEINITIALTOP  (CHILD_CANVAS_HEIGHT-RESIZEFRAMEHEIGHT)/2


static wcEL MovingIconHandle;
static wcEL ResizeIconHandle[8];
#define RESIZEICONHEIGHT 32
#define RESIZEICONWIDTH  32

static void ResizeClearIcon(void);						// Clear resize icons
static void ResizeClearMovingIcon(void);				// Clear dragging Icon
static void ResizeMoveIcon(int deltax, int deltay);		// Moves currently active icons by deltax deltay
static void ResizeSetMovingIcon(void);					// Puts dragging icon in the middle of the window.
static int  GetResizeFrameStation(HTMLEvent* e);		// Return what clockwise "station" that the mouse is on the resize from, 0-7 where 0 is the top left
static void GetResizeFrameStationPos(int station, int *x, int *y ); 	// Return the x,y coordinates of the station location realtive to gResizeFrame.
static int ResizeDoMoveFrame(wcCtx* Ctx,int deltax, int deltay);	// Dragging - Move the frame and everything with it.
static void ResizeDoResizeFrame(wcCtx* Ctx,int deltax, int deltay);	// Resizing - Resize the frame and the inner box, then redraw the active cursor
static void ResizeDrawIcon(int currentstation); // Draw the current active resizing icon or the dragging icon if that is active. Clear icons that should be hidden
static void ResizeCreateIcons(wcCtx* Ctx); // Load the icons we need from the webc: internal rom filesystem.Maker them hidden initially, we'll make them visible as needed
static HTMLEventStatus ResizeFrameCb(wcCtx* Ctx,wcEL element,HTMLEvent* e,char* param); // Event handler for dragging and resizing

/** @name ExResizeBox
   	@memo This example demonstrates a rectangular area that can be resized and dragged.
 	@doc This source code provided in webcapiexample.c demonstrates using a &lt;div&gt; with a transparent backround to act as a window manager for another &lt;div&gt; filled<br>
	with content. Demonstratesusing semitransparent borders, hiding and unhiding element, using images from the webc: rom file system and other principals of application development with webC.

	<br> Note: childwindowCtx has already been initialized to a valid window context by \Ref{OpenExampleChildWindow}.
 	<pre>
// Declarations for resize box demo. A lot of statics but it's only a demo.
static wcEL gResizeFrame, gResizeBox;
static int gLastx, gLasty, gActivestation, gActiveIcon, gResizing,gDragIfTimeExpires,gDragingWindow;
#define RESIZEFRAMEZ	5
#define RESIZECURSORZ   7
#define RESIZEBOXZ		6
#define RESIZEFRAMEMARGIN 10
#define RESIZEBOXWIDTH MAIN_CANVAS_WIDTH/2
#define RESIZEBOXHEIGHT CHILD_CANVAS_HEIGHT/2
#define RESIZEFRAMEWIDTH  (RESIZEBOXWIDTH+RESIZEFRAMEMARGIN*2)
#define RESIZEFRAMEHEIGHT (RESIZEBOXHEIGHT+RESIZEFRAMEMARGIN*2)
#define RESIZEFRAMEINITIALLEFT (MAIN_CANVAS_WIDTH-RESIZEFRAMEWIDTH)/2
#define RESIZEFRAMEINITIALTOP  (CHILD_CANVAS_HEIGHT-RESIZEFRAMEHEIGHT)/2


static wcEL MovingIconHandle;
static wcEL ResizeIconHandle[8];
#define RESIZEICONHEIGHT 32
#define RESIZEICONWIDTH  32

static void ResizeClearIcon(void);						// Clear resize icons
static void ResizeClearMovingIcon(void);				// Clear dragging Icon
static void ResizeMoveIcon(int deltax, int deltay);		// Moves currently active icons by deltax deltay
static void ResizeSetMovingIcon(void);					// Puts dragging icon in the middle of the window.
static int  GetResizeFrameStation(HTMLEvent* e);		// Return what clockwise "station" that the mouse is on the resize from, 0-7 where 0 is the top left
static void GetResizeFrameStationPos(int station, int *x, int *y ); 	// Return the x,y coordinates of the station location realtive to gResizeFrame.
static int ResizeDoMoveFrame(wcCtx* Ctx,int deltax, int deltay);	// Dragging - Move the frame and everything with it.
static void ResizeDoResizeFrame(wcCtx* Ctx,int deltax, int deltay);	// Resizing - Resize the frame and the inner box, then redraw the active cursor
static void ResizeDrawIcon(int currentstation); // Draw the current active resizing icon or the dragging icon if that is active. Clear icons that should be hidden
static void ResizeCreateIcons(wcCtx* Ctx); // Load the icons we need from the webc: internal rom filesystem.Maker them hidden initially, we'll make them visible as needed
static HTMLEventStatus ResizeFrameCb(wcCtx* Ctx,wcEL element,HTMLEvent* e,char* param); // Event handler for dragging and resizing

static void ExResizeBox(void)
{
	UPDATECONSOLETEXT("Drag in the margin to resize the box, Click hold and drag anywhere to move the box. You must click once to activate cursor rollovers.");
	gLastx=gLasty=0;
	gActivestation=-1;
	gActiveIcon=-1;

	// Put something in the window so we can see our semi-transparent borders working
	wcEL BackGroundChatter =wcNewStyledDiv(&childwindowCtx, 0, wcTRUE,  0, 0, CHILD_CANVAS_HEIGHT, MAIN_CANVAS_WIDTH,RESIZEFRAMEZ-1, 0, "white",0, 0);
	wcElSetInnerHtml(BackGroundChatter, "&lt;small&gt;Click and drag on the borders to resize the box.&lt;br&gt;Click, hold and then drag to move the box. A tranparent div acts as a window manager but events are still passed to the Window under management, click on the text edit box.&lt;br&gt;Note semitransparent borders.&lt;br&gt;");
	// Create a div to act as a frame frame around the canvas with a transparent background that's slightly larger and abovbe the canvas in z-index
	gResizeFrame =wcNewStyledDiv(&childwindowCtx, 0, wcTRUE,  RESIZEFRAMEINITIALTOP, RESIZEFRAMEINITIALLEFT, RESIZEFRAMEHEIGHT, RESIZEFRAMEWIDTH,RESIZEFRAMEZ, 0, "rgba(21,21,21,.6)",0, 0); // Gold
	// Create a canvas to display content in.
	gResizeBox=wcNewStyledDiv(&childwindowCtx, gResizeFrame, wcTRUE,  0,0, RESIZEBOXHEIGHT, RESIZEBOXWIDTH,RESIZEBOXZ, 0, "SeaGreen",0, "overflow:hidden;");
	// Give it a semi-transparent border RESIZEFRAMEMARGIN pixels thick
	wcElSetStyleBorder(gResizeBox, RESIZEFRAMEMARGIN, "rgba(211,211,211,.6)", wcFALSE);
	//  put something in it.
	wcElSetInnerHtml(gResizeBox, "&lt;br&gt;&lt;input type=text value=\"XX\"&gt; Hello: Drag around the edges to resize the box");
	// Create icons we will need for when we are resizing and moving. They are children of the frame. Leave them hidden.
	ResizeCreateIcons(&childwindowCtx);
	// Attach an event handler to the outer frame, we'll catch mouse events and move, resize and  update icons based on them.
	wcElSetEventHandler(gResizeFrame, ResizeFrameCb);
	wcWinBringToTop(&childwindowCtx);
}

// Event handler for dragging and resizing.
static HTMLEventStatus ResizeFrameCb(wcCtx* Ctx,wcEL element,HTMLEvent* e,char* param)
{
int currentstation = 0;

	switch (e->type)
	{ // This first switch pushes us move mode if we hold he mouse down without moving for 200 milliseconds
		case HTML_EVENT_TIMER:
			if (gDragIfTimeExpires)
			{ // 200 mSecs passed and we didn't move the mouse, start dragging
				ResizeSetMovingIcon();
				gDragIfTimeExpires=0;
				gDragingWindow=1;
				gResizing=0;
			}
		break;
		case HTML_EVENT_MOUSEDOWN:
			{ // Mouse down, set a timer, test later, if the mouse state doesn't chahge start dragging the window around
				HTMLEvent T; T.type=HTML_EVENT_TIMER;
				wcElTriggerEvent(element, &T, 200);
			}
			gDragIfTimeExpires=1;
			break;
		case HTML_EVENT_MOUSEOVER: case HTML_EVENT_MOUSEOUT: case HTML_EVENT_MOUSEMOVE:	case HTML_EVENT_MOUSEUP:
			gDragIfTimeExpires=0; // A mouseevent, clear drag if no mouse activity flag.
	}

	switch (e->type)
	{ // For mouse events find if we are at a station on the boundary and make sure the cursor is drawn at the station.
		case HTML_EVENT_MOUSEDOWN:	case HTML_EVENT_MOUSEOVER:	case HTML_EVENT_MOUSEMOVE:
			currentstation=GetResizeFrameStation(e); // Will be -1 if we are not on the border.
			if (!gDragingWindow && !gResizing && gActiveIcon != currentstation)
					ResizeDrawIcon(currentstation);
		break;
	}
	switch (e->type)
	{
		case HTML_EVENT_MOUSEDOWN:
			wcElClaimPointer(element);			// Claim the pointer until the button is released.
			gLastx = e->data.position.x;
			gLasty = e->data.position.y;
			gActivestation = currentstation;
			ResizeDrawIcon(currentstation);
			gResizing=1;						// Start resizing state but if we don't move the mouse within 200 mSecs we'll change to dragging.
			wcElSetStyleColorNamed(gResizeBox,CSS_PROPERTY_BACKGROUND_COLOR,rgbaSeaGreen(.5), wcTRUE);  // Start dragging, make it semi transparent.
			break;
		case HTML_EVENT_MOUSEOVER:			   // Draw proper icons for a rollover
			ResizeDrawIcon(currentstation);
			break;
		case HTML_EVENT_MOUSEOUT:			   // UnDraw the Icon
			if (!gResizing)
				ResizeClearIcon();
			break;
		case HTML_EVENT_MOUSEMOVE:
			if (gResizing||gDragingWindow)	{ // Mouse move while resizing or dragging, perform the operation.
				int deltax = e->data.position.x-gLastx;
				int deltay = e->data.position.y-gLasty;
				gLastx  = e->data.position.x;
				gLasty = e->data.position.y;
				if (gResizing)
					ResizeDoResizeFrame(Ctx,deltax, deltay);
				else if (gDragingWindow)
					ResizeDoMoveFrame(Ctx,deltax, deltay);
			}
			break;
		case HTML_EVENT_MOUSEUP: {	// UnDraw the Icon and release the mouse pointer now that we are not dragging
			wcElReleasePointer(element);
			ResizeClearIcon();
			gResizing=gDragingWindow=0;
			ResizeClearMovingIcon();
			wcElSetStyleColorNamed(gResizeBox,CSS_PROPERTY_BACKGROUND_COLOR,"SeaGreen", wcTRUE);  // Stop dragging, make it opaque.
			break;
		}
	}
	return HTML_EVENT_STATUS_CONTINUE;
}



// Return what clockwise "station" that the mouse is on the resize from, 0-7 where 0 is the top left
static int  GetResizeFrameStation(HTMLEvent* e)
{
int xoff,yoff, horizontalbar, verticalbar;
int top, left, width, height;
int station=-1;

	wcElGetPosStyle(gResizeFrame, &top, &left, &height, &width);
	xoff= e->data.position.x-left;
	yoff=  e->data.position.y-top;
	verticalbar=height-RESIZEFRAMEMARGIN;
	horizontalbar=width-RESIZEFRAMEMARGIN;
	if (yoff <=  RESIZEFRAMEMARGIN)
	{
		if (xoff <= RESIZEFRAMEMARGIN) station = 0;  // Top left
		else if (xoff <= horizontalbar) station = 1; // Top
		else station = 2;                            // Top right
	}
	else if (yoff < verticalbar)
	{
		if (xoff > horizontalbar) station = 3;		// Right
		else if (xoff <= RESIZEFRAMEMARGIN) station = 7;						    // Left
	}
	else if (yoff >= verticalbar)
	{
		if (xoff <= RESIZEFRAMEMARGIN) station = 6; // Bottom left
		else if (xoff <= horizontalbar) station = 5;// Bottom
		else station = 4;							// Botom right
	}

	return station;
}

// Return the x,y coordinates of the station location realtive to gResizeFrame.
static void GetResizeFrameStationPos(int station, int *x, int *y )
{
int top, left, width,right,height,bottom,middley,middlex;

	wcElGetPosStyle(gResizeFrame, &top, &left, &height, &width);
	top = left = 0; // Reset these to zero size we want to report  relative to the parent.
	right=left+width-1;middlex=left+(right-left)/2;
	bottom=top+height-1;middley=top+(bottom-top)/2;
	if (station == 0) { *x = left; *y = top;}
	else if (station == 1) { *x = middlex; *y = top;}
	else if (station == 2) { *x = right; *y = top;}
	else if (station == 3) { *x = right; *y = middley;}
	else if (station == 4) { *x = right; *y = bottom;}
	else if (station == 5) { *x = middlex; *y = bottom;}
	else if (station == 6) { *x = left; *y = bottom;}
	else  { *x = left; *y = middley;} // Station == 7
}



// Dragging - Move the frame and everything with it.
static int ResizeDoMoveFrame(wcCtx* Ctx,int deltax, int deltay)
{
int frametop,frameleft,framewidth,frameheight;
	wcElGetPosStyle(gResizeFrame, &frametop, &frameleft, &frameheight, &framewidth);
	wcElSetStylePos(gResizeFrame, frametop+deltay, frameleft+deltax,  wcTRUE);
	ResizeMoveIcon(deltax, deltay);
	return 0;
}

// Resizing - Resize the frame and the inner box, then redraw the active cursor
static void ResizeDoResizeFrame(wcCtx* Ctx,int deltax, int deltay)
{
int boxtop, boxleft, boxwidth,boxheight;
int frametop,frameleft,framewidth,frameheight;
	wcElGetPosStyle(gResizeBox, &boxtop, &boxleft, &boxheight, &boxwidth);
	wcElGetPosStyle(gResizeFrame, &frametop, &frameleft, &frameheight,  &framewidth);
	// Note that boxtp and boxleft always stay zero because top,left is offset from the parent.
	if (gActivestation==0 || gActivestation==1 || gActivestation==2)  // Top
	{ boxheight-=deltay; frametop+=deltay; frameheight-=deltay; }
	else if (gActivestation==4 || gActivestation==5 || gActivestation==6) // Bottom
	{ boxheight+=deltay; frameheight+=deltay; }
	if (gActivestation==0 || gActivestation == 6 || gActivestation ==7) // left
	{ boxwidth-=deltax; frameleft+=deltax; framewidth-=deltax; }
	if (gActivestation == 2 || gActivestation ==3 || gActivestation ==4) // Right
	{ boxwidth+=deltax; framewidth+=deltax; }
	if (framewidth > (2 *RESIZEFRAMEMARGIN) && frameheight > (2 *RESIZEFRAMEMARGIN))
	{
		wcElSetStyleDim(gResizeFrame, frameheight, framewidth,  wcFALSE);
		wcElSetStylePos(gResizeFrame, frametop, frameleft,  wcTRUE);
		wcElSetStyleDim(gResizeBox, boxheight, boxwidth,  wcFALSE);
		wcElSetStylePos(gResizeBox, boxtop, boxleft,  wcTRUE);
		ResizeMoveIcon(deltax, deltay);
	}
}

// Move the icon at the current active position by deltax, deltay.
static void ResizeMoveIcon(int deltax, int deltay)
{
int top, left, width,height;
	if (gActiveIcon!=-1)
	{
		wcElGetPosStyle(ResizeIconHandle[gActiveIcon], &top, &left, &height,  &width);
		if ((gActiveIcon == 1) || (gActiveIcon == 5)) deltax=0; // Don't move x if we are on the top or bottom rail
		if ((gActiveIcon == 3) || (gActiveIcon == 7)) deltay=0; // Don't move y if we are on the left or right rail
		if (gActiveIcon > 2) top+=deltay;
		if (gActiveIcon > 0 && gActiveIcon < 6) left+=deltax;
		wcElSetStylePos(ResizeIconHandle[gActiveIcon], top, left, wcTRUE);
	}

}

// Draw the current active resizing icon or the dragging icon if that is active. Clear icons that should be hidden
static void ResizeDrawIcon(int currentstation)
{
	if (gDragingWindow) {
		ResizeClearIcon();	// Clear the current icon if showing
		return;
	}
	ResizeClearMovingIcon();
	if (gActiveIcon!=currentstation)
		ResizeClearIcon();	// Clear the current icon if showing
	if (currentstation < 0)
		return;
	gActiveIcon=currentstation; // Select a new icon and position it by station
	wcEL IconHandle = ResizeIconHandle[currentstation];
	int top, left;
	GetResizeFrameStationPos(currentstation, &left, &top );
	wcElSetStylePos(IconHandle, top-(RESIZEICONHEIGHT/2), left-(RESIZEICONHEIGHT/2), wcTRUE);
	wcElSetStyleVisible(IconHandle,wcTRUE);
	gActiveIcon = currentstation;
}
// Clear the resize icon if one is showing
static void ResizeClearIcon(void)
{
	if (gActiveIcon!=-1)
	{
		wcElSetStyleHidden(ResizeIconHandle[gActiveIcon],wcTRUE);
		gActiveIcon=-1;
	}
}
// Clear the moving (dragging) icon if it is showing
static void ResizeClearMovingIcon(void)
{
	  wcElSetStyleHidden(MovingIconHandle,wcTRUE);
}
// Draw the moving (dragging) icon, and clear the resizing icon if it is showing.
static void ResizeSetMovingIcon(void)
{
int top, left, height, width;
	ResizeClearIcon();
	wcElGetPosStyle(gResizeFrame, &top, &left,  &height, &width);
 	wcElSetStylePos(MovingIconHandle, (height-RESIZEICONHEIGHT)/2, (width-RESIZEICONHEIGHT)/2, wcTRUE);
	wcElSetStyleVisible(MovingIconHandle,wcTRUE);
}
// Load the icons we need from the webc: internal rom filesystem.Maker them hidden initially, we'll make them visible as needed
static void ResizeCreateIcons(wcCtx* Ctx) // Load the icons we need from the webc: internal rom filesystem.Maker them hidden initially, we'll make them visible as needed
{
#endif
	ResizeIconHandle[0]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/NorthWest.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[1]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/North.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[2]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/NorthEast.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[3]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/East.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[4]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/SouthEast.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[5]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/South.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[6]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/SouthWest.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[7]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/West.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	MovingIconHandle =  wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneDoubles/NorthSouthEastWest.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");

}

	</pre>
 */

static void ExResizeBox(void)
{
	UPDATECONSOLETEXT("Drag in the margin to resize the box, Click hold and drag anywhere to move the box. You must click once to activate cursor rolovers.");
	gLastx=gLasty=0;
	gActivestation=-1;
	gActiveIcon=-1;

	// Put something in the window so we can see our semi-transparent borders working
	wcEL BackGroundChatter =wcNewStyledDiv(&childwindowCtx, 0, wcTRUE,  0, 0, CHILD_CANVAS_HEIGHT, MAIN_CANVAS_WIDTH,RESIZEFRAMEZ-1, 0, "white",0, 0);
	wcElSetInnerHtml(BackGroundChatter, "<small>Click and drag on the borders to resize the box.<br>Click, hold and then drag to move the box. A tranparent div acts as a window manager but events are still passed to the Window under management, click on the text edit box.<br>Note semitransparent borders.<br>");
	// Create a div to act as a frame frame around the canvas with a transparent background that's slightly larger and abovbe the canvas in z-index
	gResizeFrame =wcNewStyledDiv(&childwindowCtx, 0, wcTRUE,  RESIZEFRAMEINITIALTOP, RESIZEFRAMEINITIALLEFT, RESIZEFRAMEHEIGHT, RESIZEFRAMEWIDTH,RESIZEFRAMEZ, 0, "rgba(21,21,21,.6)",0, 0); // Gold
	// Create a canvas to display content in.
	gResizeBox=wcNewStyledDiv(&childwindowCtx, gResizeFrame, wcTRUE,  0,0, RESIZEBOXHEIGHT, RESIZEBOXWIDTH,RESIZEBOXZ, 0, "SeaGreen",0, "overflow:hidden;");
	// Give it a semi-transparent border RESIZEFRAMEMARGIN pixels thick
	wcElSetStyleBorder(gResizeBox, RESIZEFRAMEMARGIN, "rgba(211,211,211,.6)", wcFALSE);
	//  put something in it.
	wcElSetInnerHtml(gResizeBox, "<br><input type=text value=\"XX\"> Hello: Drag around the edges to resize the box");
	// Create icons we will need for when we are resizing and moving. They are children of the frame. Leave them hidden.
	ResizeCreateIcons(&childwindowCtx);
	// Attach an event handler to the outer frame, we'll catch mouse events and move, resize and  update icons based on them.
	wcElSetEventHandler(gResizeFrame, ResizeFrameCb);
	wcWinBringToTop(&childwindowCtx);
	wcCtxtRelease(&childwindowCtx); // Release any accumulated garbage.
}

// Event handler for dragging and resizing.
static HTMLEventStatus ResizeFrameCb(wcCtx* Ctx,wcEL element,HTMLEvent* e,char* param)
{
int currentstation = 0;

	switch (e->type)
	{ // This first switch pushes us move mode if we hold he mouse down without moving for 200 milliseconds
		case HTML_EVENT_TIMER:
			if (gDragIfTimeExpires)
			{ // 200 mSecs passed and we didn't move the mouse, start dragging
				ResizeSetMovingIcon();
				gDragIfTimeExpires=0;
				gDragingWindow=1;
				gResizing=0;
			}
		break;
		case HTML_EVENT_MOUSEDOWN:
			{ // Mouse down, set a timer, test later, if the mouse state doesn't chahge start dragging the window around
				HTMLEvent T; T.type=HTML_EVENT_TIMER;
				wcElTriggerEvent(element, &T, 100);
			}
			gDragIfTimeExpires=1;
			break;
		case HTML_EVENT_MOUSEOVER: case HTML_EVENT_MOUSEOUT: case HTML_EVENT_MOUSEMOVE:	case HTML_EVENT_MOUSEUP:
			gDragIfTimeExpires=0; // A mouseevent, clear drag if no mouse activity flag.
	}

	switch (e->type)
	{ // For mouse events find if we are at a station on the boundary and make sure the cursor is drawn at the station.
		case HTML_EVENT_MOUSEDOWN:	case HTML_EVENT_MOUSEOVER:	case HTML_EVENT_MOUSEMOVE:
			currentstation=GetResizeFrameStation(e); // Will be -1 if we are not on the border.
			if (!gDragingWindow && !gResizing && gActiveIcon != currentstation)
					ResizeDrawIcon(currentstation);
		break;
	}
	switch (e->type)
	{
		case HTML_EVENT_MOUSEDOWN:
			wcElClaimPointer(element);			// Claim the pointer until the button is released.
			gLastx = e->data.position.x;
			gLasty = e->data.position.y;
			gActivestation = currentstation;
			ResizeDrawIcon(currentstation);
			gResizing=1;						// Start resizing state but if we don't move the mouse within 200 mSecs we'll change to dragging.
			wcElSetStyleColorNamed(gResizeBox,CSS_PROPERTY_BACKGROUND_COLOR,rgbaSeaGreen(.5), wcTRUE);  // Start dragging, make it semi transparent.
			break;
		case HTML_EVENT_MOUSEOVER:			   // Draw proper icons for a rollover
			ResizeDrawIcon(currentstation);
			break;
		case HTML_EVENT_MOUSEOUT:			   // UnDraw the Icon
			if (!gResizing)
				ResizeClearIcon();
			break;
		case HTML_EVENT_MOUSEMOVE:
			if (gResizing||gDragingWindow)	{ // Mouse move while resizing or dragging, perform the operation.
				int deltax = e->data.position.x-gLastx;
				int deltay = e->data.position.y-gLasty;
				gLastx  = e->data.position.x;
				gLasty = e->data.position.y;
				if (gResizing)
					ResizeDoResizeFrame(Ctx,deltax, deltay);
				else if (gDragingWindow)
					ResizeDoMoveFrame(Ctx,deltax, deltay);
			}
			break;
		case HTML_EVENT_MOUSEUP: {	// UnDraw the Icon and release the mouse pointer now that we are not dragging
			wcElReleasePointer(element);
			ResizeClearIcon();
			gResizing=gDragingWindow=0;
			ResizeClearMovingIcon();
			wcElSetStyleColorNamed(gResizeBox,CSS_PROPERTY_BACKGROUND_COLOR,"SeaGreen", wcTRUE);  // Stop dragging, make it opaque.
			break;
		}
	}
	return HTML_EVENT_STATUS_CONTINUE;
}



// Return what clockwise "station" that the mouse is on the resize from, 0-7 where 0 is the top left
static int  GetResizeFrameStation(HTMLEvent* e)
{
int xoff,yoff, horizontalbar, verticalbar;
int top, left, width, height;
int station=-1;

	wcElGetPosStyle(gResizeFrame, &top, &left, &height, &width);
	xoff= e->data.position.x-left;
	yoff=  e->data.position.y-top;
	verticalbar=height-RESIZEFRAMEMARGIN;
	horizontalbar=width-RESIZEFRAMEMARGIN;
	if (yoff <=  RESIZEFRAMEMARGIN)
	{
		if (xoff <= RESIZEFRAMEMARGIN) station = 0;  // Top left
		else if (xoff <= horizontalbar) station = 1; // Top
		else station = 2;                            // Top right
	}
	else if (yoff < verticalbar)
	{
		if (xoff > horizontalbar) station = 3;		// Right
		else if (xoff <= RESIZEFRAMEMARGIN) station = 7;						    // Left
	}
	else if (yoff >= verticalbar)
	{
		if (xoff <= RESIZEFRAMEMARGIN) station = 6; // Bottom left
		else if (xoff <= horizontalbar) station = 5;// Bottom
		else station = 4;							// Botom right
	}

	return station;
}

// Return the x,y coordinates of the station location realtive to gResizeFrame.
static void GetResizeFrameStationPos(int station, int *x, int *y )
{
int top, left, width,right,height,bottom,middley,middlex;

	wcElGetPosStyle(gResizeFrame, &top, &left, &height, &width);
	top = left = 0; // Reset these to zero size we want to report  relative to the parent.
	right=left+width-1;middlex=left+(right-left)/2;
	bottom=top+height-1;middley=top+(bottom-top)/2;
	if (station == 0) { *x = left; *y = top;}
	else if (station == 1) { *x = middlex; *y = top;}
	else if (station == 2) { *x = right; *y = top;}
	else if (station == 3) { *x = right; *y = middley;}
	else if (station == 4) { *x = right; *y = bottom;}
	else if (station == 5) { *x = middlex; *y = bottom;}
	else if (station == 6) { *x = left; *y = bottom;}
	else  { *x = left; *y = middley;} // Station == 7
}



// Dragging - Move the frame and everything with it.
static int ResizeDoMoveFrame(wcCtx* Ctx,int deltax, int deltay)
{
int frametop,frameleft,framewidth,frameheight;
	wcElGetPosStyle(gResizeFrame, &frametop, &frameleft, &frameheight, &framewidth);
	wcElSetStylePos(gResizeFrame, frametop+deltay, frameleft+deltax,  wcTRUE);
	ResizeMoveIcon(deltax, deltay);
	return 0;
}

// Resizing - Resize the frame and the inner box, then redraw the active cursor
static void ResizeDoResizeFrame(wcCtx* Ctx,int deltax, int deltay)
{
int boxtop, boxleft, boxwidth,boxheight;
int frametop,frameleft,framewidth,frameheight;
	wcElGetPosStyle(gResizeBox, &boxtop, &boxleft, &boxheight, &boxwidth);
	wcElGetPosStyle(gResizeFrame, &frametop, &frameleft, &frameheight,  &framewidth);
	// Note that boxtp and boxleft always stay zero because top,left is offset from the parent.
	if (gActivestation==0 || gActivestation==1 || gActivestation==2)  // Top
	{ boxheight-=deltay; frametop+=deltay; frameheight-=deltay; }
	else if (gActivestation==4 || gActivestation==5 || gActivestation==6) // Bottom
	{ boxheight+=deltay; frameheight+=deltay; }
	if (gActivestation==0 || gActivestation == 6 || gActivestation ==7) // left
	{ boxwidth-=deltax; frameleft+=deltax; framewidth-=deltax; }
	if (gActivestation == 2 || gActivestation ==3 || gActivestation ==4) // Right
	{ boxwidth+=deltax; framewidth+=deltax; }
	if (framewidth > (2 *RESIZEFRAMEMARGIN) && frameheight > (2 *RESIZEFRAMEMARGIN))
	{
		wcElSetStyleDim(gResizeFrame, frameheight, framewidth,  wcFALSE);
		wcElSetStylePos(gResizeFrame, frametop, frameleft,  wcTRUE);
		wcElSetStyleDim(gResizeBox, boxheight, boxwidth,  wcFALSE);
		wcElSetStylePos(gResizeBox, boxtop, boxleft,  wcTRUE);
		ResizeMoveIcon(deltax, deltay);
	}
}

// Move the icon at the current active position by deltax, deltay.
static void ResizeMoveIcon(int deltax, int deltay)
{
int top, left, width,height;
	if (gActiveIcon!=-1)
	{
		wcElGetPosStyle(ResizeIconHandle[gActiveIcon], &top, &left, &height,  &width);
		if ((gActiveIcon == 1) || (gActiveIcon == 5)) deltax=0; // Don't move x if we are on the top or bottom rail
		if ((gActiveIcon == 3) || (gActiveIcon == 7)) deltay=0; // Don't move y if we are on the left or right rail
		if (gActiveIcon > 2) top+=deltay;
		if (gActiveIcon > 0 && gActiveIcon < 6) left+=deltax;
		wcElSetStylePos(ResizeIconHandle[gActiveIcon], top, left, wcTRUE);
	}

}

// Draw the current active resizing icon or the dragging icon if that is active. Clear icons that should be hidden
static void ResizeDrawIcon(int currentstation)
{
	if (gDragingWindow) {
		ResizeClearIcon();	// Clear the current icon if showing
		return;
	}
	ResizeClearMovingIcon();
	if (gActiveIcon!=currentstation)
		ResizeClearIcon();	// Clear the current icon if showing
	if (currentstation < 0)
		return;
	gActiveIcon=currentstation; // Select a new icon and position it by station
	wcEL IconHandle = ResizeIconHandle[currentstation];
	int top, left;
	GetResizeFrameStationPos(currentstation, &left, &top );
	wcElSetStylePos(IconHandle, top-(RESIZEICONHEIGHT/2), left-(RESIZEICONHEIGHT/2), wcTRUE);
	wcElSetStyleVisible(IconHandle,wcTRUE);
	gActiveIcon = currentstation;
}
// Clear the resize icon if one is showing
static void ResizeClearIcon(void)
{
	if (gActiveIcon!=-1)
	{
		wcElSetStyleHidden(ResizeIconHandle[gActiveIcon],wcTRUE);
		gActiveIcon=-1;
	}
}
// Clear the moving (dragging) icon if it is showing
static void ResizeClearMovingIcon(void)
{
	  wcElSetStyleHidden(MovingIconHandle,wcTRUE);
}
// Draw the moving (dragging) icon, and clear the resizing icon if it is showing.
static void ResizeSetMovingIcon(void)
{
int top, left, height, width;
	ResizeClearIcon();
	wcElGetPosStyle(gResizeFrame, &top, &left,  &height, &width);
 	wcElSetStylePos(MovingIconHandle, (height-RESIZEICONHEIGHT)/2, (width-RESIZEICONHEIGHT)/2, wcTRUE);
	wcElSetStyleVisible(MovingIconHandle,wcTRUE);
}
// Load the icons we need from the webc: internal rom filesystem.Maker them hidden initially, we'll make them visible as needed
static void ResizeCreateIcons(wcCtx* Ctx) // Load the icons we need from the webc: internal rom filesystem.Maker them hidden initially, we'll make them visible as needed
{
#if (0)
	ResizeIconHandle[0]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneDoubles/NorthWest.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[1]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneDoubles/NorthSouth.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[2]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneDoubles/NorthEast.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[3]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneDoubles/EastWest.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[4]=ResizeIconHandle[0];
	ResizeIconHandle[5]=ResizeIconHandle[1];
	ResizeIconHandle[6]=ResizeIconHandle[2];
	ResizeIconHandle[7]=ResizeIconHandle[3];
	MovingIconHandle =  wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneDoubles/NorthSouthEastWest.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
#endif
	ResizeIconHandle[0]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/NorthWest.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[1]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/North.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[2]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/NorthEast.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[3]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/East.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[4]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/SouthEast.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[5]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/South.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[6]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/SouthWest.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	ResizeIconHandle[7]= wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneSingles/West.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");
	MovingIconHandle =  wcNewStyledImage(Ctx, gResizeFrame, wcTRUE, "webc:/SysImages/Cursors/StyleOneDoubles/NorthSouthEastWest.gif", 0, 0, 0,RESIZEICONHEIGHT, RESIZEICONWIDTH, RESIZECURSORZ, "visibility:hidden;");

}




extern "C" void wcSoftKeyEnable(HBROWSER_HANDLE hBrowser);
extern "C" void wcSoftKeyDisable(void);


// Document event handler to disable the keyboard when the applet is unloaded.
static HTMLEventStatus ExSoftKeyPadDocb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
	if (event->type == HTML_EVENT_UNLOAD)
	{
		wcSoftKeyDisable();						// Disable the soft keyboard
		wcDocPopEventHandler(&childwindowCtx);	// Disable the event handler
        wcWinBringToTop(&mainwindowCtx);  
		wcDocRefresh(&mainwindowCtx);
	}
	wcCtxtRelease(&childwindowCtx); // Release any accumulated garbage.
    return (HTML_EVENT_STATUS_CONTINUE);
}
/** @name ExSoftKeyPad
   	@memo This example demonstrates initializing and using the built in soft keypad.
 	@doc This source code provided in webcapiexample.c provides demonstrates enabling the soft keypad so a keyboard pops up when the user clicks into a control requiring keyboard input.
 	This example also demonstrates using a document event handler to perfrom application cleanup, in this case disabling the keyboard.
	<br> Note: childwindowCtx has already been initialized to a valid window context by \Ref{OpenExampleChildWindow}.
 	<pre>
// Document event handler to disable the keyboard when the applet is unloaded.
static HTMLEventStatus ExSoftKeyPadDocb(wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param)
{
	if (event->type == HTML_EVENT_UNLOAD)
	{
		wcSoftKeyDisable();						// Disable the soft keyboard
		wcDocPopEventHandler(&childwindowCtx);	// Disable the event handler
	}
    return (HTML_EVENT_STATUS_CONTINUE);
}

// Soft Keypad example.
static void ExSoftKeyPad(void)
{
	// Clear the window content
	wcDocClear(&childwindowCtx, wcFALSE);
	UPDATECONSOLETEXT("Soft keyboard installed click on an edit control to pop it up.");
	wcEL BackGroundChatter=wcNewStyledDiv(&childwindowCtx, 0, wcTRUE,  0, 0, CHILD_CANVAS_HEIGHT, MAIN_CANVAS_WIDTH,RESIZEFRAMEZ-1, 0, "white",0, 0);
	wcElSetInnerHtml(BackGroundChatter, "Soft keypad pops up when you click. Press enter or escape on the keypad to dismiss it. &lt;input type=text value=\"Click Here\"&gt;");
	// Enable the soft keypad driver
	wcSoftKeyEnable(childwindowCtx.hbrowser);
	// Set up a handler to disable the keyboard when the applet is unloaded.
	wcDocSetEventHandler(&childwindowCtx,ExSoftKeyPadDocb);
	wcWinBringToTop(&childwindowCtx);
}


	</pre>
 */

static void ExSoftKeyPad(void)
{
	// Clear the window content
	wcDocClear(&childwindowCtx, wcFALSE);
	UPDATECONSOLETEXT("Soft keyboard installed click on an edit control to pop it up.");
	wcEL BackGroundChatter=wcNewStyledDiv(&childwindowCtx, 0, wcTRUE,  0, 0, CHILD_CANVAS_HEIGHT, MAIN_CANVAS_WIDTH,RESIZEFRAMEZ-1, 0, "white",0, 0);
	wcElSetInnerHtml(BackGroundChatter, "Soft keypad pops up when you click. Press enter or escape on the keypad to dismiss it. <input type=text value=\"Click Here\">");
	// Enable the soft keypad driver
	wcSoftKeyEnable(childwindowCtx.hbrowser);
	// Set up a handler so we can disable the keyboard when the applet is unloaded.
	wcDocSetEventHandler(&childwindowCtx,ExSoftKeyPadDocb);

	wcWinBringToTop(&childwindowCtx);
	wcCtxtRelease(&childwindowCtx); // Release any accumulated garbage.
}


// Static for the next test
static char *SliderEWxampleHtmlSource = "\
<html><body>\
<textarea rows=6 cols=24>\
testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest\
test test test test test test test test test\
test test test test test test test test test\
test test test test test test test test test\
test test test test test test test test test\
test test test test test test test test test\
test test test test test test test test test\
</textarea></body></html>";

static char *SliderEWxampleHtmlFmt =  "\
<html><body>\
<textarea rows=6 cols=24>\
(%s)\
\n\n\n\n\n\
====================================================================\n\
\n\n\
</textarea></body></html>";

/** @name ExSliders
   	@memo Under construction.
 	@doc This source code provided in webcapiexample.c provides examples for modifying the appearance of scrollbars.
	<br> Note: childwindowCtx has already been initialized to a valid window context by \Ref{OpenExampleChildWindow}.
<pre>

</pre>
 */
static void ExSlidersCB(int Sequence, void *UserVoidParm);

static void ExSliderDisplay(char *message)
{
char HtmlBuff[1024];
	rtp_sprintf(HtmlBuff,SliderEWxampleHtmlFmt,message);
	wcDocClear(&childwindowCtx, wcFALSE);
	if (wcDocOpen(&childwindowCtx)==0)
	{
		wcDocWriteHtml(&childwindowCtx, HtmlBuff, rtp_strlen(HtmlBuff),  wcTRUE);
	//	wcDocWriteHtml(&childwindowCtx, SliderEWxampleHtmlSource, rtp_strlen(SliderEWxampleHtmlSource),  wcTRUE);
		wcDocClose(&childwindowCtx);
	}
	wcCtxtRelease(&childwindowCtx); // Release any accumulated garbage.
}
static void ExSliders(void)
{
	wcDocClear(&childwindowCtx, wcFALSE);
	wcWinBringToTop(&childwindowCtx);
	//gWebcDefault[WEBC_DEFAULT_SHADOW_COLOR]                  = (void*) 0;
    //gWebcDefault[WEBC_DEFAULT_SLIDER_BACKGROUND_COLOR]       = (void*) MEDDARKGRAY;
    //gWebcDefault[WEBC_DEFAULT_SLIDER_COLOR]                  = (void*) MEDLIGHTGRAY;
    //gWebcDefault[WEBC_DEFAULT_SLIDER_FRAME_WIDTH]            = (void*) 1;
    //gWebcDefault[WEBC_DEFAULT_SLIDER_BACKGROUND_FRAME_WIDTH] = (void*) 1;
    //gWebcDefault[WEBC_DEFAULT_SLIDER_HILITE_COLOR]           = (void*) LIGHTGRAY;
    //gWebcDefault[WEBC_DEFAULT_SLIDER_PADDING]                = (void*) 0;
    //gWebcDefault[WEBC_DEFAULT_SLIDER_SHADOW_COLOR]           = (void*) MEDDARKGRAY;
    //gWebcDefault[WEBC_DEFAULT_SLIDER_WIDTH]                  = (void*) 15;
    //gWebcDefault[WEBC_DEFAULT_SLIDER_FRAME_SHADOW_COLOR]     = (void*) MEDLIGHTGRAY;

	ExSliderDisplay("Scrollbar Default values");
	 //webc_SetDefault(WEBC_DEFAULT_SHADOW_COLOR, wcImageGraphNameToColor(Orange));
#if (0)
	 webc_SetDefault(WEBC_DEFAULT_SLIDER_BACKGROUND_COLOR, wcImageGraphNameToColor(Black));
 	 webc_SetDefault(WEBC_DEFAULT_SLIDER_COLOR, wcImageGraphNameToColor(Black));
	 webc_SetDefault(WEBC_DEFAULT_SLIDER_FRAME_WIDTH, 2);
 	 webc_SetDefault(WEBC_DEFAULT_SLIDER_BACKGROUND_FRAME_WIDTH, 2);
 	 webc_SetDefault(WEBC_DEFAULT_SLIDER_HILITE_COLOR, wcImageGraphNameToColor(DarkGray));
	 webc_SetDefault(WEBC_DEFAULT_SLIDER_PADDING, 0);
	 webc_SetDefault(WEBC_DEFAULT_SLIDER_SHADOW_COLOR, wcImageGraphNameToColor(DarkGray));
 	 webc_SetDefault(WEBC_DEFAULT_SLIDER_WIDTH, 8);
 	 webc_SetDefault(WEBC_DEFAULT_SLIDER_FRAME_SHADOW_COLOR, wcImageGraphNameToColor(Black));
#endif

 	// webc_SetDefault(WEBC_DEFAULT_SLIDER_WIDTH, 8);

	// webc_SetDefault(WEBC_DEFAULT_SLIDER_HILITE_COLOR, HTML_RGBAToColor(255,0,0,0));
	//webc_SetDefault(WEBC_DEFAULT_SLIDER_SHADOW_COLOR, HTML_RGBAToColor(0,255,0,0));
	// webc_SetDefault(WEBC_DEFAULT_SLIDER_FRAME_SHADOW_COLOR, HTML_RGBAToColor(128,0,0,0));
	// webc_SetDefault(WEBC_DEFAULT_SLIDER_FRAME_HILITE_COLOR, HTML_RGBAToColor(0,128,0,0));


	// Start the show after 1 second at sequence 0
	wcTimedCallback(ExSlidersCB, 1000, 0, (void *)0);

}

static void ExSlidersCB(int Sequence, void *UserVoidParm)
{
	if (Examples[currentExample].ExampleFunction != ExSliders)
		return;
	// Cycle through the sequence numbers showing line graphics features
	switch (Sequence) {
	default: // Out of tests. just exit
		UPDATECONSOLETEXT("Slider example complete - Click NEXT to move to next example.");
		return;
	case 0:
		UPDATECONSOLETEXT("Slider example slideshow running.");
	   	webc_SetDefault(WEBC_DEFAULT_SLIDER_WIDTH, 24);
		ExSliderDisplay("The default scrollbar style can be changed.");
		break;
    case 1:
	   	webc_SetDefault(WEBC_DEFAULT_SLIDER_WIDTH, 16);
  		webc_SetDefault(WEBC_DEFAULT_SLIDER_COLOR, wcImageGraphNameToColor(White));
  		webc_SetDefault(WEBC_DEFAULT_SLIDER_BACKGROUND_COLOR, wcImageGraphNameToColor(Black));
    	webc_SetDefault(WEBC_DEFAULT_SLIDER_HILITE_COLOR, wcImageGraphNameToColor(DarkGray));
    	webc_SetDefault(WEBC_DEFAULT_SLIDER_SHADOW_COLOR, wcImageGraphNameToColor(DarkGray));
 	 	webc_SetDefault(WEBC_DEFAULT_SLIDER_FRAME_SHADOW_COLOR, wcImageGraphNameToColor(Black));
 	 	webc_SetDefault(WEBC_DEFAULT_SLIDER_FRAME_WIDTH, 2);
    	webc_SetDefault(WEBC_DEFAULT_SLIDER_BACKGROUND_FRAME_WIDTH, 2);
    	webc_SetDefault(WEBC_DEFAULT_SLIDER_BACKGROUND_FRAME_WIDTH, 2);
    	ExSliderDisplay("Variations like this");
    	break;
    case 2:
    	webc_SetDefault(WEBC_DEFAULT_SLIDER_WIDTH, 8);
    	ExSliderDisplay("Variations like this");
    	break;
    case 3:
		webc_RestoreDefaults();
    	webc_SetDefault(WEBC_DEFAULT_SLIDER_WIDTH, 8);
    	ExSliderDisplay("Resuming the exmaples with new scrollbar thickness");
    	break;
	}
	wcTimedCallback(ExSlidersCB, 2000, ++Sequence, UserVoidParm);
}

#if (WEBC_SUPPORT_JSCRIPT)

static char *JsdemoHtmlSource = "\
<html>\
<head>\
<style>\
input{width:200px;}\
</style>\
<script>\
function OnloadHandler(){\
	b = true;\
	o=new Array();\
	alert(\"Click to execute C code from Javascript\");\
	r=ExampleStart(\"Hello from Jscript\",1,b,o);\
	document.getElementById(\"jsoutput\").value=r;\
}\
function CalledFromC()\
{\
return \"String from JS to C\";\
}\
</script>\
</head>\
<body onload=\"OnloadHandler();\">\
Javascript:<input type=\"text\" id=\"jsoutput\"><br>\
---------C:<input type=\"text\" id=\"coutput\"><br>\
</body></html>";
static void JScriptArgsToString(char *result, JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

JSBool NativeExampleStart(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
wcCtx Ctx;
jsval r;
char argstring[256];
char buf[512];
	wcJSInitializeDocCtxt(cx, &Ctx); // Get a webC api context
	// Update the HTML from C after being called by javascript
	wcElSetValue(wcDocFindEl(&Ctx, "coutput"),"C called from Jscript");
	// Get the arguments passed to C from Javascript
	JScriptArgsToString(argstring, cx, obj, argc, argv, rval);
	// Call a Javascript function, pass it some arguments and retrieve its return value
	{
	jsval args;
	if (!JS_CallFunctionName(cx, JS_GetGlobalObject(cx), "CalledFromC", 0, NULL, &r))
		return JS_FALSE;
	}
	// Display the pased arguments and the return value from inside C
	rtp_sprintf(buf,"Argments passed JScript to C where: %s.\nC code called javascript which returned %s",argstring,JS_GetStringBytes(JS_ValueToString(cx,r)));
	UPDATECONSOLETEXT(buf); // Send the result to the console

	wcCtxtRelease(&Ctx); // Release the webC context
	*rval = STRING_TO_JSVAL(JS_InternString(cx, "String returned from 'C'")); 	// Now return a C string to Javascript.
	return JS_TRUE;
}

// Retrieve all values passed from Javascript and put them in a string.
static void JScriptArgsToString(char *result, JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
int i;
char buf[256];

	result[0] = 0;
	for (i = 0; i < (int)argc; i++)
	{
		rtp_strcat(result,":");
		if (JSVAL_IS_STRING(argv[i]))  // Display string
		{
			char *Cstring;
			JSString* Jstring = JS_ValueToString(cx, argv[i]);
			JS_AddRoot(cx, &Jstring);
			Cstring=JS_GetStringBytes(Jstring);
			rtp_strcat(result,Cstring);
			JS_RemoveRoot(cx, &Jstring);
		}
		else if (JSVAL_IS_INT(argv[i])) // Display intyegers
		{
			int32 cval;
			JS_ValueToInt32(cx, argv[i], &cval);
			rtp_sprintf(buf,"%d",cval);
			rtp_strcat(result, buf);
		}
		else	// All others just display the type
		{
			rtp_sprintf(buf,"Type=%s",JS_GetTypeName(cx, JS_TypeOfValue(cx, argv[i])));
			rtp_strcat(result, buf);
		}
	}
	UPDATECONSOLETEXT(result); // Send the result to the console
}

/** @name ExJScript
   	@memo Under construction.
 	@doc This source code provided in webcapiexample.c provides examples for using Javascript and C together.
	<br> Note: childwindowCtx has already been initialized to a valid window context by \Ref{OpenExampleChildWindow}.
<pre>

</pre>
 */

 static void ExJScript(void)
 {
	 // Clear the window content and raise the window to top.
	wcDocClear(&childwindowCtx, wcFALSE);

	wcJSDefineFunction(&childwindowCtx, "ExampleStart", NativeExampleStart , 1);

	if (wcDocOpen(&childwindowCtx)==0)
	{
		wcDocWriteHtml(&childwindowCtx, JsdemoHtmlSource, rtp_strlen(JsdemoHtmlSource),  wcTRUE);
		wcDocClose(&childwindowCtx);
	}
	wcWinBringToTop(&childwindowCtx);

 }
#endif

// Static for the next test
static char *ControlsExampleHtmlSource[] = {"\
<html>\
<body style=\"overflow:scroll;\">\
<ol style=\"list-style-type:circle;width:140px;background-color:green;color:maroon;border-style:solid;border-color:blue;\">\
<li>One</li>\
<li style=\"list-style-type:square;background-color:lightblue;color:burgundy;\">Two</li>\
</ol>",
"\
<div style=\"background-color:lightgrey\">\
<label for=\"male\">Male</label>\
<input type=\"checkbox\" id=\"male\" name=\"vehicle\" value=\"Car\" checked=\"checked\" />\
</div>\
<div style=\"background-color:lightblue\">\
<input type=\"radio\" name=\"vehicle\" id=\"first\" value=\"Car\" checked=\"checked\" />\
<input type=\"radio\" name=\"vehicle\" id=\"middle\" value=\"Car\" checked=\"checked\"/>\
<input type=\"radio\" name=\"vehicle\" id=\"last\" value=\"Car\" checked=\"checked\" /><br>\
<label for=\"first\">First:</label><br>\
<label for=\"last\">Last:</label><br>\
<label for=\"middle\">Middle:</label><br>\
</div>\
",
"</body></html>",
0};

/** @name ExControls
   	@memo Under construction.
 	@doc This source code provided in webcapiexample.c provides examples for modifying the appearance of scrollbars.
	<br> Note: childwindowCtx has already been initialized to a valid window context by \Ref{OpenExampleChildWindow}.
<pre>

</pre>
 */
static void ExControls(void)
{
	// Clear the window content and load the HTML and CSS calculator source in
	wcDocClear(&childwindowCtx, wcFALSE);
	wcWinBringToTop(&childwindowCtx);
wcEL c;
wcEL r;
wcEL t = wcNewStyledTable(&childwindowCtx, 0, wcTRUE,10, 20, 120, 200, 1,0,0,0,"red","blue",0,"font-style:italic;");
	r = wcTableInsertRow(&childwindowCtx, t, CSS_TEXT_ALIGN_CENTER, CSS_VERTICAL_ALIGN_MIDDLE, "white", "orange", 0, 0);

	c =  wcTableInsertHeaderCell(&childwindowCtx,r,0, 0,0,0);
	wcTableCellSetWidth(&childwindowCtx, c, 80);
	wcElSetInnerHtml(c,"Bing");
	c =  wcTableInsertHeaderCell(&childwindowCtx,r,0, 0,0, 0);
	wcElSetInnerHtml(c,"Yo");
	c =  wcTableInsertHeaderCell(&childwindowCtx,r,0, 0,0, 0);
	wcElSetInnerHtml(c,"Y");

	r = wcTableInsertRow(&childwindowCtx, t, CSS_TEXT_ALIGN_CENTER, CSS_VERTICAL_ALIGN_MIDDLE, "white", "orange", 0, 0);
	c = wcTableInsertCell(&childwindowCtx,r,0, 0,0, 0);
	wcTableCellSetSpans(&childwindowCtx, c, 3, 1);
	wcElSetInnerHtml(c,"Boing");
//	c = wcTableInsertCell(&childwindowCtx,r,0, 0,0, 0);
//	wcElSetInnerHtml(c,"X");
//	c = wcTableInsertCell(&childwindowCtx,r,0, 0,0, 0);
//	wcElSetInnerHtml(c,"Y");

	r = wcTableInsertRow(&childwindowCtx, t, CSS_TEXT_ALIGN_CENTER, CSS_VERTICAL_ALIGN_MIDDLE, "white", "orange", 0, 0);
	c = wcTableInsertCell(&childwindowCtx,r,0, 0,0,0);
	wcElSetInnerHtml(c,"Bing2");
	c = wcTableInsertCell(&childwindowCtx,r,0, 0,0, 0);
	wcElSetInnerHtml(c,"Yo2");

	r = wcTableInsertRow(&childwindowCtx, t, CSS_TEXT_ALIGN_CENTER, CSS_VERTICAL_ALIGN_MIDDLE, "white", "orange", 0, 0);
	c = wcTableInsertCell(&childwindowCtx,r,0, 0,0,0);
	wcElSetInnerHtml(c,"Bing3");
	c = wcTableInsertCell(&childwindowCtx,r,0, 0,0, 0);
	wcElSetInnerHtml(c,"Yo3");


//	c = wcTableInsertCell(&childwindowCtx,r,1,1, 0, 0,0, 0);

//	if (wcDocOpen(&childwindowCtx)==0)
//	{
//		for (int i=0; ControlsExampleHtmlSource[i]; i++)
//		wcDocWriteHtml(&childwindowCtx, ControlsExampleHtmlSource[i], rtp_strlen(ControlsExampleHtmlSource[i]),  wcFALSE);
//		wcDocClose(&childwindowCtx);
//	}
	     	wcDocRefresh(&childwindowCtx);
	wcCtxtRelease(&childwindowCtx); // Release any accumulated garbage.
}


// ==========
// Statics for the next test

static void ExGraphicsSlideShowCB(int Sequence, void *UserVoidParm);
#define GRAPHPALLETWIDTH  300
#define GRAPHPALLETHEIGHT 120
#ifndef PI
#define PI 3.14159265358979323846
#endif
static void ExGraphicsSinWave(wcEL ImageElement,int frequency, int baseline, int amplitude, HTMLColor color, wcBOOL fill);
// Float a label over the template
static void ExGraphicsLabel(int top, int left,int z,char *message)
{
wcEL MessageDiv;
	// MessageDiv=wcNewStyledDiv(&childwindowCtx, 0, wcTRUE,  top, left, 20, 180,z, "white", "Transparent",0, "overflow:hidden;");
	MessageDiv=wcNewStyledFont(&childwindowCtx, 0, wcTRUE,  top, left, 20, 180,z, "white", "Transparent", "overflow:hidden;");

	//  put something in it.
	char Buffer[512];
	rtp_sprintf(Buffer, "<small>%s</small>", message);
	wcElSetInnerHtml(MessageDiv, Buffer);

}
// Clears the child window and returns a new image element to draw into
static wcEL ExGraphicsClear(void)
{
	wcDocClear(&childwindowCtx, wcFALSE);
	wcEL ImageElement =  wcNewStyledImage(&childwindowCtx, 0, wcTRUE, "webc:///Example/Images/ExampleCanvas.png", 0, 0, 0, GRAPHPALLETHEIGHT,GRAPHPALLETWIDTH, 0, 0);
	return ImageElement;
}


/** @name ExGraphics
   	@memo A slideshow applet demonstrating the line drawing capabilities of webC.
 	@doc Presents several slides showing line, ellipse, arc, polygon, and rectangles draw and fill functions.
	<br> Note: childwindowCtx has already been initialized to a valid window context by \Ref{OpenExampleChildWindow}.
<pre>
Source code:

static void ExGraphicsSlideShowCB(int Sequence, void *UserVoidParm);
static void ExGraphics(void)
{
	// Clear the window content and load an immage to draw into.
	\Ref{wcDocClear}(&childwindowCtx, wcFALSE);
	wcEL ImageElement =  \Ref{wcNewStyledImage}(&childwindowCtx, 0, wcTRUE, "webc:///Example/Images/ExampleCanvas.png", 0, 0, 0, GRAPHPALLETHEIGHT,GRAPHPALLETWIDTH, 0, 0);

	\Ref{wcWinBringToTop}(&childwindowCtx);
	// Start the show imediately at sequence 0
	\Ref{wcTimedCallback}(ExGraphicsSlideShowCB, 0, 0, (void *)ImageElement);

}

#define GRAPHPALLETWIDTH  300
#define GRAPHPALLETHEIGHT 120
#define PI 3.14159265358979323846
static void ExGraphicsSinWave(wcEL ImageElement,int frequency, int baseline, int amplitude, HTMLColor color, wcBOOL fill);
// Float a label over the template
static void ExGraphicsLabel(int top, int left,int z,char *message)
{
wcEL MessageDiv;
	MessageDiv=\Ref{wcNewStyledDiv}(&childwindowCtx, 0, wcTRUE,  top, left, 20, 180,z, "white", "Transparent",0, "overflow:hidden;");
	//  put something in it.
	char Buffer[512];
	rtp_sprintf(Buffer, "&lt;small&gt;%s&lt;/small&gt;", message);
	\Ref{wcElSetInnerHtml}(MessageDiv, Buffer);

}
// Clears the child window and returns a new image element to draw into
static wcEL ExGraphicsClear(void)
{
	\Ref{wcDocClear}(&childwindowCtx, wcFALSE);
	wcEL ImageElement =  \Ref{wcNewStyledImage}(&childwindowCtx, 0, wcTRUE, "webc:///Example/Images/ExampleCanvas.png", 0, 0, 0, GRAPHPALLETHEIGHT,GRAPHPALLETWIDTH, 0, 0);
	return ImageElement;
}

static void ExGraphicsSlideShowCB(int Sequence, void *UserVoidParm)
{
wcEL ImageElement = (wcEL) UserVoidParm;
int nextShowDelay = SLIDESHOWDELAY; // Change nextShowDelay to speed up or slow down a particular slide
wcBOOL 	FillPolygons = wcTRUE; // Cases that do the unfilled version clear this value
	// We are called by a system timer that doesn't know about our window so check if the user press left or right an reloaded since the timer was armed.
	// If we proceeded we would crash because the elements were cleared from the document.
	if (Examples[currentExample].ExampleFunction != ExGraphics)
		return;
	// Cycle through the sequence numbers showing line graphics features
	switch (Sequence) {
	default: // Out of tests. just exit
		UPDATECONSOLETEXT("Line Graphics slideshow complete - Click LEFT or RIGHT to move to next example.");
		return;
	case 0:  // Draw lines
		{
		UPDATECONSOLETEXT("Line Graphics  slideshow - Draw Lines and arcs");
		\Ref{wcImageGraphBeginDraw}(ImageElement);
		HtmlColorIndex c = Blue;
		double Delta=(2*PI)/16;	double theta =0.0;	double r = 50.0;
		int oy,ox;	oy=ox=50;
		\Ref{wcImageGraphSetBrushWidth}(ImageElement, 4);
		// Draw 16 lines in a circle, vary the color by indexing through the color table.
		for (int i = 0;i < 16; i += 2,theta+=Delta)
		{
		int x0,y0,x1,y1;
			y1 = oy+int(sin(theta)*r);y0=oy-int(sin(theta)*r);
			x1 = ox+int(cos(theta)*r);x0=oy-int(cos(theta)*r);
			\Ref{wcImageGraphSetPenColor}(ImageElement, \Ref{wcImageGraphNameToColor}(c));
			c = (HtmlColorIndex) ((int)c+1);
			\Ref{wcImageGraphDrawLine}(ImageElement, x0,  y0, x1, y1);
		}
		ExGraphicsLabel(oy+50, ox/2,1,"- Lines -");
		// Draw a series of connected arcs, vary the color by indexing through the color table.
		\Ref{wcImageGraphSetPenColor}(ImageElement, wcImageGraphNameToColor(Purple));
		ox=200;oy=50;
		for (int ir1 = 90; ir1 > 20; ir1 -= 10)
		{
			wcBOOL doFill = ir1==30 ? wcTRUE : wcFALSE; // Calculation so we fill the last arc we draw
			\Ref{wcImageGraphSetPenColor}(ImageElement, wcImageGraphNameToColor(c));	c = (HtmlColorIndex) ((int)c+1);
			\Ref{wcImageGraphArc}(ImageElement, ox, oy, ir1, 40, 0, 180,wcFALSE);
			\Ref{wcImageGraphArc}(ImageElement, ox, oy, ir1, 40, 180, 360,doFill);
		}
		ExGraphicsLabel(oy+50, ox-20,1,"- Arcs -");
		}
		break;
	case 1: // Draw Ellipses
		{
		int x,y,h,w;
		UPDATECONSOLETEXT("Line Graphics slideshow - Draw Ellipses");
		ImageElement = ExGraphicsClear();
		\Ref{wcImageGraphBeginDraw}(ImageElement);
		// Fill the background in SteelBlue
		\Ref{wcImageGraphSetPenColor}(ImageElement, wcImageGraphNameToColor(SteelBlue));
		\Ref{wcImageGraphDrawRect}(ImageElement, 0, 0, GRAPHPALLETWIDTH, GRAPHPALLETHEIGHT, wcTRUE);
		\Ref{wcImageGraphSetBrushWidth}(ImageElement, 4);
		// Draw some elipses in different colors.
		HtmlColorIndex c = Blue;
		x=20; w=10;h=10;y=50;
		for (;;)
		{
			\Ref{wcImageGraphSetPenColor}(ImageElement, wcImageGraphNameToColor(c));
			c = (HtmlColorIndex) ((int)c+1);
			\Ref{wcImageGraphDrawEllipse}(ImageElement, x, y, h, w,wcFALSE);
			x += 6; h+=6; w+=6;
			if (x+w > GRAPHPALLETWIDTH || y+h > GRAPHPALLETHEIGHT)
				break;
		}
		ExGraphicsLabel(GRAPHPALLETHEIGHT-20, (GRAPHPALLETWIDTH*1)/3-80,1,"- Ellipses -");
		// Draw some filled elipses.
		x = (GRAPHPALLETWIDTH*2)/3;
		h-=6; w-=6;
		for (;;)
		{
			wcImageGraphSetPenColor(ImageElement, wcImageGraphNameToColor(c));
			c = (HtmlColorIndex) ((int)c+1);
			\Ref{wcImageGraphDrawEllipse}(ImageElement, x, y, h, w,wcTRUE);
			x += 6; h-=6; w-=6;
			if (w < 0 || h < 0)
				break;
		}
		ExGraphicsLabel(GRAPHPALLETHEIGHT-20, (GRAPHPALLETWIDTH*2)/3-40,1,"- Filled Ellipses -");
		}
		break;
	case 2: // Polygons
		FillPolygons = wcFALSE;
	case 3: // Filled Polygons
		{
			ImageElement = ExGraphicsClear();
			HtmlColorIndex c = LightGreen;
			int baseline = 40;
			\Ref{wcImageGraphBeginDraw}(ImageElement);
			// Fill the background in SteelBlue
			\Ref{wcImageGraphSetPenColor}(ImageElement, wcImageGraphNameToColor(SteelBlue));
			\Ref{wcImageGraphDrawRect}(ImageElement, 0, 0, GRAPHPALLETWIDTH, GRAPHPALLETHEIGHT, wcTRUE);

			for (int frequency=20; frequency > 4; frequency -= 4,baseline+= 10)
			{
				c = (HtmlColorIndex) ((int)c+1);
				ExGraphicsSinWave(ImageElement, frequency, baseline, 30, wcImageGraphNameToColor(c), FillPolygons);
			}
			if (FillPolygons)
			{
				UPDATECONSOLETEXT("Line Graphics slideshow- Draw Filled PolyGons");
				ExGraphicsLabel(GRAPHPALLETHEIGHT-20, GRAPHPALLETWIDTH/2-80,1,"- Filled Polygons -");
			}
			else
			{
				UPDATECONSOLETEXT("Line Graphics slideshow - Draw PolyGons");
				ExGraphicsLabel(GRAPHPALLETHEIGHT-20, GRAPHPALLETWIDTH/2-80,1,"- Polygons -");
			}
		}
		break;

	}
	\Ref{wcImageGraphEndDraw}(ImageElement);
	\Ref{wcTimedCallback}(ExGraphicsSlideShowCB, SLIDESHOWDELAY, ++Sequence, UserVoidParm);
}

static void ExGraphicsSinWave(wcEL ImageElement,int frequency, int baseline, int amplitude, HTMLColor color, wcBOOL fill)
{
	double Delta=(frequency*3.414)/200;
	double theta =0;
	int SinArray[220];
	int npoints = 100;
	for (int i = 0;i < 200; i += 2,theta+=Delta)
	{
		SinArray[i]=i+20;
		SinArray[i+1]=baseline + int(sin(theta)*(double)amplitude);
	}
	SinArray[200]=SinArray[198];
	SinArray[201]=(GRAPHPALLETHEIGHT-20);
	SinArray[202]=SinArray[0];
	SinArray[203]=(GRAPHPALLETHEIGHT-20);
	SinArray[204]=SinArray[0];
	SinArray[205]=SinArray[1];
	if (fill)
		npoints += 3;
	\Ref{wcImageGraphSetPenColor}(ImageElement, color);
	\Ref{wcImageGraphDrawPolygon}(ImageElement, npoints, SinArray,fill);
}

</pre>

*/

static void ExGraphics(void)
{
	// Clear the window content and load an immage to draw into.
	wcDocClear(&childwindowCtx, wcFALSE);
	wcEL ImageElement =  wcNewStyledImage(&childwindowCtx, 0, wcTRUE, "webc:///Example/Images/ExampleCanvas.png", 0, 0, 0, GRAPHPALLETHEIGHT,GRAPHPALLETWIDTH, 0, 0);

	wcWinBringToTop(&childwindowCtx);
	// Start the show imediately at sequence 0
	wcCtxtRelease(&childwindowCtx); // Release any accumulated garbage.
	wcTimedCallback(ExGraphicsSlideShowCB, 0, 0, (void *)ImageElement);

}

static void ExGraphicsSlideShowCB(int Sequence, void *UserVoidParm)
{
wcEL ImageElement = (wcEL) UserVoidParm;
int nextShowDelay = SLIDESHOWDELAY; // Change nextShowDelay to speed up or slow down a particular slide
wcBOOL 	FillPolygons = wcTRUE; // Cases that do the unfilled version clear this value
	// We are called by a system timer that doesn't know about our window so check if the user press left or right an reloaded since the timer was armed.
	// If we proceeded we would crash because the elements were cleared from the document.
	if (Examples[currentExample].ExampleFunction != ExGraphics)
		return;
	// Cycle through the sequence numbers showing line graphics features
	switch (Sequence) {
	default: // Out of tests. just exit
		UPDATECONSOLETEXT("Line Graphics slideshow complete - Click NEXT to move to next example.");
		return;
	case 0:  // Draw lines
		{
		UPDATECONSOLETEXT("Line Graphics  slideshow - Draw Lines and arcs");
		wcImageGraphBeginDraw(ImageElement);
		HtmlColorIndex c = Blue;
		double Delta=(2*PI)/16;	double theta =0.0;	double r = 50.0;
		int oy,ox;	oy=ox=50;
		wcImageGraphSetBrushWidth(ImageElement, 4);
		// Draw 16 lines in a circle, vary the color by indexing through the color table.
		for (int i = 0;i < 16; i += 2,theta+=Delta)
		{
		int x0,y0,x1,y1;
			y1 = oy+int(sin(theta)*r);y0=oy-int(sin(theta)*r);
			x1 = ox+int(cos(theta)*r);x0=oy-int(cos(theta)*r);
			wcImageGraphSetPenColor(ImageElement, wcImageGraphNameToColor(c));
			c = (HtmlColorIndex) ((int)c+1);
			wcImageGraphDrawLine(ImageElement, x0,  y0, x1, y1);
		}
		ExGraphicsLabel(oy+50, ox/2,1,"- Lines -");
		// Draw a series of connected arcs, vary the color by indexing through the color table.
		wcImageGraphSetPenColor(ImageElement, wcImageGraphNameToColor(Purple));
		ox=200;oy=50;
		for (int ir1 = 90; ir1 > 20; ir1 -= 10)
		{
			wcBOOL doFill = ir1==30 ? wcTRUE : wcFALSE; // Calculation so we fill the last arc we draw
			wcImageGraphSetPenColor(ImageElement, wcImageGraphNameToColor(c));	c = (HtmlColorIndex) ((int)c+1);
			wcImageGraphArc(ImageElement, ox, oy, ir1, 40, 0, 180,wcFALSE);
			wcImageGraphArc(ImageElement, ox, oy, ir1, 40, 180, 360,doFill);
		}
		ExGraphicsLabel(oy+50, ox-20,1,"- Arcs -");
		}
		break;
	case 1: // Draw Ellipses
		{
		int x,y,h,w;
		UPDATECONSOLETEXT("Line Graphics slideshow - Draw Ellipses");
		ImageElement = ExGraphicsClear();
		wcImageGraphBeginDraw(ImageElement);
		// Fill the background in SteelBlue
		wcImageGraphSetPenColor(ImageElement, wcImageGraphNameToColor(SteelBlue));
		wcImageGraphDrawRect(ImageElement, 0, 0, GRAPHPALLETWIDTH, GRAPHPALLETHEIGHT, wcTRUE);
		wcImageGraphSetBrushWidth(ImageElement, 4);
		// Draw some elipses in different colors.
		HtmlColorIndex c = Blue;
		x=20; w=10;h=10;y=50;
		for (;;)
		{
			wcImageGraphSetPenColor(ImageElement, wcImageGraphNameToColor(c));
			c = (HtmlColorIndex) ((int)c+1);
			wcImageGraphDrawEllipse(ImageElement, x, y, h, w,wcFALSE);
			x += 6; h+=6; w+=6;
			if (x+w > GRAPHPALLETWIDTH || y+h > GRAPHPALLETHEIGHT)
				break;
		}
		ExGraphicsLabel(GRAPHPALLETHEIGHT-20, (GRAPHPALLETWIDTH*1)/3-80,1,"- Ellipses -");
		// Draw some filled elipses.
		x = (GRAPHPALLETWIDTH*2)/3;
		h-=6; w-=6;
		for (;;)
		{
			wcImageGraphSetPenColor(ImageElement, wcImageGraphNameToColor(c));
			c = (HtmlColorIndex) ((int)c+1);
			wcImageGraphDrawEllipse(ImageElement, x, y, h, w,wcTRUE);
			x += 6; h-=6; w-=6;
			if (w < 0 || h < 0)
				break;
		}
		ExGraphicsLabel(GRAPHPALLETHEIGHT-20, (GRAPHPALLETWIDTH*2)/3-40,1,"- Filled Ellipses -");
		}
		break;
	case 2: // Polygons
		FillPolygons = wcFALSE;
	case 3: // Filled Polygons
		{
			ImageElement = ExGraphicsClear();
			HtmlColorIndex c = LightGreen;
			int baseline = 40;
			wcImageGraphBeginDraw(ImageElement);
			// Fill the background in SteelBlue
			wcImageGraphSetPenColor(ImageElement, wcImageGraphNameToColor(SteelBlue));
			wcImageGraphDrawRect(ImageElement, 0, 0, GRAPHPALLETWIDTH, GRAPHPALLETHEIGHT, wcTRUE);

			for (int frequency=20; frequency > 4; frequency -= 4,baseline+= 10)
			{
				c = (HtmlColorIndex) ((int)c+1);
				ExGraphicsSinWave(ImageElement, frequency, baseline, 30, wcImageGraphNameToColor(c), FillPolygons);
			}
			if (FillPolygons)
			{
				UPDATECONSOLETEXT("Line Graphics slideshow- Draw Filled PolyGons");
				ExGraphicsLabel(GRAPHPALLETHEIGHT-20, GRAPHPALLETWIDTH/2-80,1,"- Filled Polygons -");
			}
			else
			{
				UPDATECONSOLETEXT("Line Graphics slideshow - Draw PolyGons");
				ExGraphicsLabel(GRAPHPALLETHEIGHT-20, GRAPHPALLETWIDTH/2-80,1,"- Polygons -");
			}
		}
		break;

	}
	wcImageGraphEndDraw(ImageElement);
	wcTimedCallback(ExGraphicsSlideShowCB, SLIDESHOWDELAY, ++Sequence, UserVoidParm);
}

static void ExGraphicsSinWave(wcEL ImageElement,int frequency, int baseline, int amplitude, HTMLColor color, wcBOOL fill)
{
	double Delta=(frequency*3.414)/200;
	double theta =0;
	int SinArray[220];
	int npoints = 100;
	for (int i = 0;i < 200; i += 2,theta+=Delta)
	{
		SinArray[i]=i+20;
		SinArray[i+1]=baseline + int(sin(theta)*(double)amplitude);
	}
	SinArray[200]=SinArray[198];
	SinArray[201]=(GRAPHPALLETHEIGHT-20);
	SinArray[202]=SinArray[0];
	SinArray[203]=(GRAPHPALLETHEIGHT-20);
	SinArray[204]=SinArray[0];
	SinArray[205]=SinArray[1];
	if (fill)
		npoints += 3;
	wcImageGraphSetPenColor(ImageElement, color);
	wcImageGraphDrawPolygon(ImageElement, npoints, SinArray,fill);
}

#ifdef RTP_TRACK_LOCAL_MEMORY
static char *exMemFileName(char *path);
RTPMemBlockHeader *PreviousWatchPoint;


static void PeakUsageCallback(const char *file, long line_num,void *ctx)
{
	wcPrintMemDetailReport(0,0,DbSummarizeByLine, SortBySize);
}



static void ExMemDiags(void)
{

	wcCtxtRelease(&childwindowCtx); // Release any accumulated garbage.
	wcDocClear(&childwindowCtx, wcTRUE);

	wcPrintMemStats();

	if (!PreviousWatchPoint)
	{
		wcMemDbInstallPeakUsageCallback(PeakUsageCallback,(void *)0, 2000000);
		PreviousWatchPoint = wcMemDbGetWatchPoint();
		//OpenExampleChildWindow();
		return;
	}
	else
	{

		wcPrintMemChangeReport(PreviousWatchPoint, wcTRUE);
		wcMemDbReleaseReport(PreviousWatchPoint);
		PreviousWatchPoint = wcMemDbGetWatchPoint();
	}
}




#endif
