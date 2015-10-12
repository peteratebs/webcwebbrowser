//
// wcdomtest.CPP -
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
/**@name <name for the group>
* <documentation for the group>
*/

//#include "../include/NewApi.h"
#include "rtpmem.h"
#include "wcapi.h"
#include "rtpprint.h"
#include "rtpstr.h"

#define VERBOSE 1
static wcBOOL EnumTestCallback(wcCtx* Ctx, wcEL El, int UserIntParm, void *UserVoidParm);
static char *elTypeToString(wcEL element);
static HTMLEventStatus wcTestEventCallback(wcCtx* Ctx,	wcEL element,HTMLEvent* event,	char* param);
static HTMLEventStatus ClickMeCallback(wcCtx* Ctx,	wcEL element,HTMLEvent* event,	char* param);
static void OpenModalDialog(char *message);

static HTMLEventStatus wcTestBodyCallback(wcCtx* Ctx,	wcEL element,HTMLEvent* event,	char* param);
static void SimpleModalDialog(char *message);
static void OpenTest10Window(wcCtx *newCtxt, int top, int left, char *message);
static HTMLEventStatus Test10Callback(wcCtx* Ctx,	wcEL element,HTMLEvent* event,	char* param);
static wcEL Test10Elements[3][3];
static int Test10Focus;
static int Test10CycleCount;
static wcCtx Test10WindowZero;
static wcCtx Test10WindowOne;
static void CycleTest10(void);
static void TestWindowBorders(void);
static void StartFlyingWindowDemo( int top, int left,WEBC_UINT32 WindowFlags);



wcCtx _Ctx;


void TestDomApi(wcBROW browser, wcDOC doc)
{
wcCtx _Ctx;

	wcCtxtInit(&_Ctx,  browser, doc);
	// Test1: - Use dynamic element creation and dom insertion to create a page.
	// Containing a table.
	rtp_printf("Test1: - Use dynamic element creation and dom insertion to create a page. Containing a table.\n");

     wcEL table = wcElNew(&_Ctx, "table");
     wcEL td;
     wcEL tr 	= wcElNew(&_Ctx, "tr");

     	wcELInsertFirst(table,tr);
     	wcElSetId(table, "table0");
		wcElSetStyle(table, "border-style:solid;border-thickness:2px;", wcTRUE);

		td 	= wcElNew(&_Ctx, "td");
    	wcELInsertFirst(tr,td);
     	wcElSetId(tr, "row0");
		// This crashes when we clone if it is a checkboxk
     	wcElSetInnerHtml(td,"Hello from 0,0");
     	wcElSetId(td, "col0");

      	//wcElSetStyle(td, "border-style:solid;border-thickness:2px;");
		td 	= wcElNew(&_Ctx, "td");
     	wcElSetId(td, "col1");
    	wcELInsertFirst(tr,td);
     	wcElSetInnerHtml(td,"Hello from 0,1");
      	//wcElSetStyle(td, "border-style:solid;border-thickness:2px;");
     	wcDocInsertFirst(&_Ctx, table);
     
		wcEL checkboxdiv;
		checkboxdiv	= wcElNew(&_Ctx, "div");
     	wcElSetInnerHtml(checkboxdiv,"<input id=\"check\" type=\"checkbox\" name=\"checkme\" value=\"Car\" checked=\"checked\" />");

     	wcDocRefresh(&_Ctx);

	rtp_printf("Test2: - Find the table cell by walking the dom.\n");
	{
	wcBOOL Error = wcFALSE;
		wcEL El = wcELFind (&_Ctx, 0, "/table0/row0/col1", &Error);
		if (El)
     		wcElSetInnerHtml(El,"0,0 again");
	}
	rtp_printf("Test3: - Use the enumeration function to walk the tree and print Id values.\n");
	// Garbage collect ascii return buffers when done.
	// See EnumTestCallback below.
	{
	int nfound = 0;
		wcDocEnumerate(&_Ctx, EnumTestCallback, 0, (void *) &nfound);
		rtp_printf("Enumeration stest fond this many objects == %d\n", nfound); 	// print the count we tabulated from the callback.
		wcCtxtRelease(&_Ctx);					// free buffers that ascii strings were returned in.
	}


	rtp_printf("Test4: - Use the clone function to clone the table we just made.\n");
	// Test4: - Use the clone function to clone the table we just made.
	// Insert it into our document, give it a new name and move it.
	// Re enumerate to see what we have
	// See EnumTestCallback below.
	wcEL tableClone = wcElClone(table, wcTRUE);
	if (tableClone)
	{
	wcBOOL Error = wcFALSE;
	int nfound = 0;
	wcEL El;
     	wcDocInsertLast(&_Ctx, tableClone);
		wcElSetStyle(tableClone, "position:absolute;left:20px;top:100px;",wcTRUE);
		wcElSetId(tableClone, "clonetable0");
		El = wcELFind (&_Ctx, 0, "/clonetable0/row0/col0", &Error);
		if (El)
     		wcElSetInnerHtml(El,"0,0 cloned");
		wcDocEnumerate(&_Ctx, EnumTestCallback, 0, (void *) &nfound);
		rtp_printf("Enumeration after clone found this many objects == %d\n", nfound); 	// print the count we tabulated from the callback.
		wcCtxtRelease(&_Ctx);					// free buffers that ascii strings were returned in.
     	wcDocRefresh(&_Ctx);
	}

	rtp_printf("Test5: - Create a button and attach an event handler to it.\n");
	// Test5: - Create a button and attach an event handler to it.
	// The event handler will print out the events presented to it.
	// To demonstrate use of push and pop events we will over write the the event hendler that must
	// be clicked to pop the first handler into place.
    wcEL button = wcElNew(&_Ctx, "input type=button");
	{
	wcBOOL Error = wcFALSE;
	int nfound = 0;
	static int whichhandler = 0;

	    wcDocInsertLast(&_Ctx, button);
     	wcElSetInnerHtml(button, "Clickme to toggle event handlers");
		wcElSetValue(button, "Clickme");
		wcElSetStyle(button, "position:absolute;left:20px;top:150px;",wcTRUE);

		// cloning of input tag crashes, needs debugging.
		// wcEL buttonClone = wcElClone(button, wcTRUE);
		wcEL buttonClone = wcElNew(&_Ctx, "input type=button");
	    wcDocInsertLast(&_Ctx, buttonClone);
     	wcElSetInnerHtml(buttonClone, "Another butoon, no handler attached");
		wcElSetValue(buttonClone, "Clickme");
		wcElSetStyle(buttonClone, "position:absolute;left:20px;top:200px;",wcTRUE);

		wcElSetEventHandler(button,  wcTestEventCallback);
		wcElSetEventHandler(button,  ClickMeCallback);
		// enumerate to show it and its value
		wcDocEnumerate(&_Ctx, EnumTestCallback, 0, (void *) &nfound);
		wcCtxtRelease(&_Ctx);					// free buffers that ascii strings were returned in.
     	wcDocRefresh(&_Ctx);
//		wcElSetStyle(button, "position:absolute;left:120px;top:150px;width:100px;height:40px;");
     	wcDocRefresh(&_Ctx);

	}
	// Test6: - Verify push/pop data functioons.
	rtp_printf("Test6: - Verify element push/pop private data functions. \n");
	{
	int v1 = 1;
	int v2 = 2;
	int *p;
	wcPushData(&_Ctx, button, (void *) &v1);
	wcPushData(&_Ctx, button, (void *) &v2);
	p = (int *) wcGetData(&_Ctx, button);
	if (p)
		rtp_printf(" wcGetdata test value should be two: %d\n", *p);
	else
		rtp_printf(" wcGetdata test failed");
	wcPopData(&_Ctx, button);
	p = (int *) wcGetData(&_Ctx, button);
	if (p)
		rtp_printf(" wcGetdata test value should be one: %d\n", *p);
	else
		rtp_printf(" wcGetdata test failed");
	v1 = 3;
	v2 = 4;
	wcPushData(&_Ctx, 0, (void *) &v1);
	wcPushData(&_Ctx, 0, (void *) &v2);
	p = (int *) wcGetData(&_Ctx, 0);
	if (p)
		rtp_printf(" wcGetdata test value should be four: %d\n", *p);
	else
		rtp_printf(" wcGetdata test failed");
	wcPopData(&_Ctx, 0);
	p = (int *) wcGetData(&_Ctx, 0);
	if (p)
		rtp_printf(" wcGetdata test value should be three: %d\n", *p);
	else
		rtp_printf(" wcGetdata test failed");
	}

	rtp_printf("Test7 and 8: - Verify modal window support. \n");

	// Test7: - Create a modal window. Use the body callback event handler to create a button element.
	// Use the button to trigger a close of the window.
	// Note: This test also verifies the behavior of wcElGetPosWin, wcElGetPosStyle and wcElSetPosStyle.
	// The tests center a button on a screen without setting the left and top style attributes.
	// These test sequences demonstrate the code sequence that handles the HTML_EVENT_WINDOW_REFRESHED event
	// and calls wcElGetPosWin() to retrieve the coordinates where the elements is displayed and wcElSetPosStyle() to
	// set the top, left, height, width style attributes to the rendered position.
	OpenModalDialog("Dismiss the modal dialog to proceed");

	// Test8: - Create a window. Combine a message and a dismiss button in the html text.
	// Give the find button an ID then attach a dismiss button handler to the button using
	// wcElFind and wcElSetEventHandler.
	// Call wcWinExecute instead of using the Modal argument to wcWinCreateEX
	SimpleModalDialog("I am a modal dialog box. Click to dismiss.");


	rtp_printf("Test9: - Verify window border setting support. \n");
	// Test9: - Test window borders.
	TestWindowBorders();
dotest10:
	// Test10: - Create two windows each containing two edit controls.
	// First cycle the focus among the edit controls for 20 seconds.
	// Then move the windows around the screen
	wcBOOL Error = wcFALSE;

	OpenTest10Window(&Test10WindowZero, 100, 100, "Focus test: Window  zero. Type. The focus changes every second.");
	OpenTest10Window(&Test10WindowOne,  325, 175, "Focus test: Window One.Type. The focus changes every second.");


	Test10Elements[0][0] =  wcELFind (&Test10WindowZero, 0, "/0", &Error);
	Test10Elements[0][1] =  wcELFind (&Test10WindowZero, 0, "/1", &Error);
	Test10Elements[0][2] =  wcELFind (&Test10WindowZero, 0, "/2", &Error);
	Test10Elements[1][0] =  wcELFind (&Test10WindowOne,  0, "/0", &Error);
	Test10Elements[1][1] =  wcELFind (&Test10WindowOne,  0, "/1", &Error);
	Test10Elements[1][2] =  wcELFind (&Test10WindowOne,  0, "/2", &Error);
	wcElSetEventHandler(Test10Elements[0][0], Test10Callback);
	CycleTest10();
	// Note: CycleTest10() calls StartFlyingWindowDemo() when it is finished

}

#define FLYINGWINDOWPERIOD 10
#define FLYINGWINDOWITERATIONS 1000
struct flyingwindowcontrol {
	int iteration;
	int xstep;
	int ystep;
};

int FlyWithNoSpriteDone;
static void FlyingWindowCloseCallback(int UserIntParm, void *UserVoidParm)
{
	wcWinClose((wcCtx*)UserVoidParm);
	rtp_free(UserVoidParm);
	if (!FlyWithNoSpriteDone)
		StartFlyingWindowDemo(0,10, 0);
	FlyWithNoSpriteDone = 1;
}

static HTMLEventStatus FlyingWindowBodyCallback(wcCtx* Ctx,	wcEL element,HTMLEvent* event,	char* param)
{ /* Close the Browser window when the dismiss button is clicked. */
int CycleTimer = 0;
int MaxX,MaxY;
struct flyingwindowcontrol *pcontrol;

	MaxX = 600;	// Should call get canvas
	MaxY = 600;

    if (event->type == HTML_EVENT_OBJECT_CREATED)
	{
		struct flyingwindowcontrol *pcontrol;
		pcontrol = (struct flyingwindowcontrol *)rtp_malloc(sizeof(struct flyingwindowcontrol));
		pcontrol->iteration = 0;
		pcontrol->xstep = 4;
		pcontrol->ystep = 2;
		wcPushData(Ctx, element, (void*) pcontrol);
    	CycleTimer = 1;
	}
    else if (event->type == HTML_EVENT_MOUSEDOWN)
	{ // experimental
		pcontrol = (struct flyingwindowcontrol *) wcGetData(Ctx, element);
		wcPopData(Ctx, element);
		rtp_free(pcontrol);
		wcWinClose(Ctx);
		StartFlyingWindowDemo(0, 10, WF_SPRITE);
		return (HTML_EVENT_STATUS_HALT);
	}
    else if (event->type == HTML_EVENT_TIMER)
	{
	int left,top,width,height;
		pcontrol = (struct flyingwindowcontrol *) wcGetData(Ctx, element);
		pcontrol->iteration += 1;
		if (pcontrol->iteration >= FLYINGWINDOWITERATIONS)
		{
			// free private data
			wcPopData(Ctx, element);
			rtp_free(pcontrol);
			// Make a private copy of the stack based context to pass to the Global timer handler
			wcCtx* KillCtx;
			KillCtx = (wcCtx*)rtp_malloc(sizeof(*Ctx));
			*KillCtx = *Ctx;
			// Tell the global timer to call our close callback routine
			wcTimedCallback(FlyingWindowCloseCallback,0, 0, (void *) KillCtx);
			return (HTML_EVENT_STATUS_CONTINUE);
		}
		wcWinGetPos(Ctx, &left, &top, &width, &height);
		left += pcontrol->xstep;
		if (left < 0)
		{
			left = 0;
			pcontrol->xstep *= -1;
		}
		else if (left+width >= MaxX)
		{
			left -= pcontrol->xstep;
			pcontrol->xstep *= -1;
		}
		top += pcontrol->ystep;
		if (top < 0)
		{
			top = 0;
			pcontrol->ystep *= -1;
		}
		else if (top+height >= MaxY)
		{
			top -= pcontrol->ystep;
			pcontrol->ystep *= -1;
		}
		wcWinSetPos(Ctx, left, top, width, height);
    	CycleTimer = 1;
	}

	if (CycleTimer)
	{
		HTMLEvent NewEvent;
		rtp_memset(&NewEvent, 0 ,sizeof(NewEvent));
		NewEvent.type = HTML_EVENT_TIMER;
		wcTriggerEvent(element, &NewEvent, FLYINGWINDOWPERIOD);
    }
	return (HTML_EVENT_STATUS_CONTINUE);
}
static void StartFlyingWindowDemo( int top, int left,WEBC_UINT32 WindowFlags)
{
	wcCtx newCtxt;
	struct HTMLBrowserWindowConfig conf;
	rtp_memset(&conf, 0, sizeof(conf));
	conf.top=top;
	conf.left=left;
	conf.width = 50;
	conf.height= 60;
	conf.BorderThickness = 4;
	conf.BorderColor = 0xff0000;
	conf.SetBodyBackGroundColor    = WEBC_TRUE;
	conf.BackGroundColor =0x0000ff; ;
	conf.BorderStyle = BS_LOWERED;
	conf.WindowFlags = WindowFlags;
	// v =
	wcWinCreateEx(&newCtxt, "Testwindow",FlyingWindowBodyCallback,0,
	"<html><body><div><center>I<br>can<br>fly!!<br></center></div></body></html>",
	&conf);
}


static void CycleTest10WindowMove(void);

#define FOCUSTESTCOUNT 6
#define MOVETESTCOUNT 10
static void CycleTest10(void)
{
	HTMLEvent Event;

	if (Test10Focus == 0)
	{
		printf("Set Test10WindowZero, element 0 \n");
		wcWinSetFocus(&Test10WindowZero, Test10Elements[0][0]);
	}
	else if (Test10Focus == 1)
	{
		printf("Set Test10WindowZero, element 1 \n");
		wcWinSetFocus(&Test10WindowZero, Test10Elements[0][1]);
	}
	else if (Test10Focus == 2)
	{
		printf("Set Test10WindowOne, element 0 \n");
		wcWinSetFocus(&Test10WindowOne,  Test10Elements[1][0]);
	}
	else if (Test10Focus == 3)
	{
		printf("Set Test10WindowOne element 1 \n");
		wcWinSetFocus(&Test10WindowOne,  Test10Elements[1][1]);
	}
	Test10Focus += 1;
	Test10CycleCount++;

	if (Test10Focus > 3)
	{
		if (Test10CycleCount < FOCUSTESTCOUNT)
			Test10Focus = 0;
	}
	if (Test10Focus > 3)
		CycleTest10WindowMove();
	rtp_memset(&Event, 0 ,sizeof(Event));
	Event.type = HTML_EVENT_TIMER;

	if (Test10CycleCount < FOCUSTESTCOUNT)
		wcTriggerEvent(Test10Elements[0][0], &Event, 500);
	else if (Test10CycleCount < 300)
		wcTriggerEvent(Test10Elements[0][0], &Event, 500);
	else
		StartFlyingWindowDemo(0,10,WF_SPRITE);
}

static void OpenTest10Window(wcCtx *newCtxt, int left, int top,char *message)
{
	int v;
	struct HTMLBrowserWindowConfig conf;
	rtp_memset(&conf, 0, sizeof(conf));
	conf.top=top;
	conf.left=left;
	conf.width = 200;
	conf.height= 200;
	conf.BorderThickness = 4;
	conf.BorderColor = 0xff0000;
	conf.SetBodyBackGroundColor    = WEBC_TRUE;
	conf.BackGroundColor =0x0000ff; ;
//#define BS_RAISED 				1  /* These 4 styles are exclusive of each other */
//#define BS_LOWERED				2
//#define BS_RIDGE				4
//#define BS_GROOVE				8
//#define BS_ROUNDED				16 /* This can be applied to the other styles */
	conf.BorderStyle = BS_LOWERED;
//#define WF_RESIZABLE 			1
//#define WF_DISABLESCROLLBARS 	2
//#define WF_MODAL 				4
//	conf.WindowFlags = WF_MODAL;
	char page[1024];
	rtp_sprintf(page,"<html><body>%s<br>\
	<input type=\"text\" id=\"0\" value=\"0\"><br>\
	<input type=\"text\" id=\"1\" value=\"1\"><br>\
	<input type=\"text\" id=\"2\" value=\"Field 0\">\
	</body></html>", message);
	v = wcWinCreateEx(newCtxt, "Testwindow",0,0,page, &conf);
}
static int MoveCycle;
static int ZeroX, OneX, ZeroY, OneY;
#define MOVEXPOINT(P) P+=40;if(P>600) P=0;
#define MOVEYPOINT(P) P+=40;if(P>400) P=0;
#define RESIZEWIDTH(P) P+=40;if(P>800) P=100;
#define RESIZEHEIGHT(P) P+=40;if(P>600) P=100;

static void CycleTest10WindowMove(void)
{
	if (MoveCycle == 0)
	{
		ZeroX = 100; OneX = 150; ZeroY = 10; OneY = 60;
		wcWinMoveTo(&Test10WindowZero,ZeroX, ZeroY);
		wcWinMoveTo(&Test10WindowOne,OneX, OneY);
		wcWinBringToTop(&Test10WindowZero);
	}
	else if (MoveCycle < MOVETESTCOUNT)
	{
		int Move = (MoveCycle-1) % 4;
		switch (Move) {
			case 0:
				MOVEXPOINT(ZeroX)
				MOVEYPOINT(ZeroY)
				wcWinMoveTo(&Test10WindowZero,ZeroX, ZeroY);
				break;
			case 1:
				wcWinBringToTop(&Test10WindowOne);
				break;
			case 2:
				MOVEXPOINT(OneX)
				MOVEYPOINT(OneY)
				wcWinMoveTo(&Test10WindowOne,OneX, OneY);
				break;
			case 3:
				wcWinBringToTop(&Test10WindowZero);
				break;
			default :
				break;
		}
	}
	else
	{
	int left,top,width,height;
		int Move = (MoveCycle-MOVETESTCOUNT) % 2;
		switch (Move) {
			case 0:
				wcWinGetPos(&Test10WindowZero, &left, &top, &width, &height);
				RESIZEWIDTH(width)
				RESIZEHEIGHT(height)
				wcWinSetPos(&Test10WindowZero, 0, 0, width, height);
				break;
			case 1:
				wcWinGetPos(&Test10WindowOne, &left, &top, &width, &height);
				RESIZEWIDTH(width)
				RESIZEHEIGHT(height)
				wcWinSetPos(&Test10WindowOne, 0, 0, width, height);
				break;
			default :
				break;
		}
	}
	MoveCycle++;
}
static HTMLEventStatus Test10Callback(wcCtx* Ctx,	wcEL element,HTMLEvent* event,	char* param)
{ /* Close the Browser window when the dismiss button is clicked. */
    if (event->type == HTML_EVENT_TIMER)
    	CycleTest10();
	return (HTML_EVENT_STATUS_CONTINUE);
}
static void OpenModalDialog(char *message)
{
	wcCtx newCtxt;
	int v;
	char modalmessage[1024];
	struct HTMLBrowserWindowConfig conf;
	rtp_memset(&conf, 0, sizeof(conf));
	conf.top=200;
	conf.left=100;
	conf.width = 200;
	conf.height= 200;
	conf.BorderThickness = 4;
	conf.BorderColor = 0xff0000;
	conf.SetBodyBackGroundColor    = WEBC_TRUE;
	conf.BackGroundColor =0x0000ff; ;
//#define BS_RAISED 				1  /* These 4 styles are exclusive of each other */
//#define BS_LOWERED				2
//#define BS_RIDGE				4
//#define BS_GROOVE				8
//#define BS_ROUNDED				16 /* This can be applied to the other styles */
	conf.BorderStyle = BS_LOWERED;
//#define WF_RESIZABLE 			1
//#define WF_DISABLESCROLLBARS 	2
//#define WF_MODAL 				4
	conf.WindowFlags = WF_MODAL;
	rtp_sprintf(modalmessage,"<html><body>%s<br><br></body></html>", message);
	v = wcWinCreateEx(&newCtxt, "Testwindow",wcTestBodyCallback,0,modalmessage, &conf);
	wcWinExecute(&newCtxt);
	wcWinClose(&newCtxt);
}

// Dump information about an element
static void showElInfo(wcEL El ,char *message)
{
#if (VERBOSE)
int top,left,width,height;
	/* Display the coordinates reported by the display driver */
	if (!wcElGetPosWin(El, &top,&left,&width,&height))
		rtp_printf("%s: wcElGetPosWin() could not get coordinated \n", message);
	else
		rtp_printf("%s: wcElGetPosWin() (%d,%d,%d,%d) \n", message,top,left,width,height);
	/* Display the coordinates reported by the style manager */
	if (!wcElGetPosStyle(El, &top,&left,&width,&height))
		rtp_printf("%s: wcElGetPosStyle() could not get coordinated \n", message);
	else
		rtp_printf("%s: wcElGetPosStyle() (%d,%d,%d,%d) \n", message,top,left,width,height);
#endif
}

static void _SimpleModalDialog(struct HTMLBrowserWindowConfig *pconf, char *message,int left_justified);

static void TestWindowBorders(void)
{
	struct HTMLBrowserWindowConfig conf;

	rtp_memset(&conf, 0, sizeof(conf));
	conf.top=100;
	conf.left=100;
	conf.width = 200;
	conf.height= 200;
	conf.BorderThickness = 8;
	conf.BorderColor = 0xffff0000;
	conf.SetBodyBackGroundColor    = WEBC_TRUE;
	conf.BackGroundColor =0x000000ff;
//#define BS_RAISED 				1  /* These 4 styles are exclusive of each other */
//#define BS_LOWERED				2
//#define BS_RIDGE				4
//#define BS_GROOVE				8
//#define BS_ROUNDED				16 /* This can be applied to the other styles */
	conf.BorderStyle = BS_RAISED;
//#define WF_RESIZABLE 			1
//#define WF_DISABLESCROLLBARS 	2
//#define WF_MODAL 				4
	conf.WindowFlags = WF_MODAL|WF_SAVEUNDER;

	/* Format a small html page with our message and a dimiss button
	   The button is contained inside <center>. Center is a container so it requires an id
	   for us to be able to use dom traversal search from the root of the window to find it */
	_SimpleModalDialog(&conf, "Style Raise Borderthickness 4",0);
//	pW->Config.WindowConfig.BorderThickness = 2;
//	pW->Config.WindowConfig.BorderColor = 0xff808080;
//	pW->Config.WindowConfig.BackGroundColor = 0xffffffff;
//	pW->Config.WindowConfig.BorderStyle = 0;
	conf.left=300;
	conf.BorderThickness = 12;
	conf.BorderColor =0x0000ff; ;
	conf.SetBodyBackGroundColor    = WEBC_TRUE;
	conf.BackGroundColor = 0xff0000;
	_SimpleModalDialog(&conf, "Style Raise Borderthickness 12 color 0000ff,ff0000",0);

	conf.left=300;
	conf.BorderThickness = 2;
	conf.BorderColor =0x0000ff; ;
	conf.SetBodyBackGroundColor    = WEBC_TRUE;
	conf.BackGroundColor = 0x0000ff;
	_SimpleModalDialog(&conf, "Style Raise Borderthickness 2 color 0000ff,000000ff",0);
	conf.BorderThickness = 8;
	conf.BorderColor =0x0000ff; ;
	conf.SetBodyBackGroundColor    = WEBC_TRUE;
	conf.BackGroundColor = 0xff0000;
	_SimpleModalDialog(&conf, "Style Raise Borderthickness 8 color 0000ff,ff0000",0);
	conf.BorderThickness = 16;
	conf.BorderColor =0x0000ff; ;
	conf.SetBodyBackGroundColor    = WEBC_TRUE;
	conf.BackGroundColor = 0xff0000;
	_SimpleModalDialog(&conf, "Style Raise Borderthickness 16 color 0000ff,ff0000",0);

 	conf.BorderColor =0x00ff00; ;
	conf.SetBodyBackGroundColor    = WEBC_TRUE;
	conf.BackGroundColor = 0xff0000;
	_SimpleModalDialog(&conf, "Style Raise Borderthickness 16 color 00ff00,ff0000",0);
 	conf.BorderColor =0xffff00; ;
	conf.SetBodyBackGroundColor    = WEBC_TRUE;
	conf.BackGroundColor = 0xff0000;
	_SimpleModalDialog(&conf, "Style Raise Borderthickness 16 color ffff00,ff0000",0);


	conf.BorderThickness = 8;
 	conf.BorderColor =0xffff00; ;
	conf.SetBodyBackGroundColor    = WEBC_TRUE;
	conf.BackGroundColor = 0xff0000;
	conf.BorderStyle = BS_LOWERED;
	_SimpleModalDialog(&conf, "Style Lowered Borderthickness 8 color ffff00,ff0000",0);
	conf.BorderStyle = BS_RIDGE;
	_SimpleModalDialog(&conf, "Style ridge Borderthickness 8 color ffff00,ff0000",0);
	conf.BorderStyle = BS_GROOVE;
	_SimpleModalDialog(&conf, "Style groove Borderthickness 8 color ffff00,ff0000",0);
test:
	_SimpleModalDialog(&conf, "Withscrollbarsenabledxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",0);
	conf.BorderStyle = BS_GROOVE;
	conf.WindowFlags |= WF_DISABLESCROLLBARS;
	_SimpleModalDialog(&conf, "Withoutscrollbarsenabledxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",1);


}

static HTMLEventStatus ModalDismissCallback(wcCtx* Ctx,	wcEL element,HTMLEvent* event,	char* param);



static void _SimpleModalDialog(struct HTMLBrowserWindowConfig *pconf, char *message,int left_justified)
{
	wcCtx newCtxt;
	int v;
	wcBOOL Error = wcFALSE;
	wcEL El;
	char modalmessage[512];

	pconf->WindowFlags |= WF_SAVEUNDER;
	if (left_justified)
	{
		rtp_sprintf(
			modalmessage,
			"<html><body><b>Alert!</b><br>%s<br><br><div id=\"center0\">\
			<input type=\"button\" id=\"dismiss\" value=\"Dismiss\"></div></body></html>"
			, message);

	}
	else
	{
		rtp_sprintf(
			modalmessage,
			"<html><body><center id=\"center0\"><b>Alert!</b><br>%s<br><br>\
			<input type=\"button\" id=\"dismiss\" value=\"Dismiss\"></center></body></html>"
			, message);
	}
	/* Call wcWinExecute instead of using the Modal argument to wcWinCreateEX */
	v = wcWinCreateEx(&newCtxt, "Alertwindow",0,0,modalmessage,pconf);
	El = wcELFind (&newCtxt, 0, "/center0/dismiss", &Error);
	if (El)
	{
		wcElSetEventHandler(El,  ModalDismissCallback);
		showElInfo(El, "Button element after create modal: ");
	}
	wcWinExecute(&newCtxt);
	wcWinClose(&newCtxt);

}


static void SimpleModalDialog(char *message)
{
	struct HTMLBrowserWindowConfig conf;

	rtp_memset(&conf, 0, sizeof(conf));

	conf.top=200;
	conf.left=200;
	conf.width = 200;
	conf.height= 200;
	conf.BorderThickness = 4;
	conf.BorderColor = 0xff0000;
	conf.SetBodyBackGroundColor    = WEBC_TRUE;
	conf.BackGroundColor =0x0000ff; ;
//#define BS_RAISED 				1  /* These 4 styles are exclusive of each other */
//#define BS_LOWERED				2
//#define BS_RIDGE				4
//#define BS_GROOVE				8
//#define BS_ROUNDED				16 /* This can be applied to the other styles */
	conf.BorderStyle = BS_RAISED;
//#define WF_RESIZABLE 			1
//#define WF_DISABLESCROLLBARS 	2
//#define WF_MODAL 				4
	conf.WindowFlags = WF_MODAL;
	/* Format a small html page with our message and a dimiss button
	   The button is contained inside <center>. Center is a container so it requires an id
	   for us to be able to use dom traversal search from the root of the window to find it */
	_SimpleModalDialog(&conf, message,0);
}

static HTMLEventStatus ModalDismissCallback(wcCtx* Ctx,	wcEL element,HTMLEvent* event,	char* param)
{ /* Close the Browser window when the dismiss button is clicked. */
    if (event->type == HTML_EVENT_WINDOW_REFRESHED)
	{
	char ascii_buff[80];
	int top,left,width,height;
//		showElInfo(element, "Button element after HTML_EVENT_WINDOW_REFRESHED: ");
//		wcElGetPosWin(element, &top,&left,&width,&height);
//		rtp_sprintf(ascii_buff, "position:absolute;left:%dpx;top:%dpx;", left, top);
//		wcElSetStyle(element, ascii_buff,wcFALSE);
		// On draw done we know that we are rendered, so showElInfo should show real coordinates
//		showElInfo(element, "Button element set style 1");
//		rtp_sprintf(ascii_buff, "height:%dpx;width:%dpx;", height, width);
//		wcElSetStyle(element, ascii_buff, wcFALSE);
//		showElInfo(element, "Button element set style 2");
	}
    if (event->type == HTML_EVENT_RENDER)
	{
		// On draw done we know that we are rendered, so showElInfo should show real coordinates
//		if (event->data.NotifyMessage.NotifyEvent == NOTIFY_RENDER_DRAW_DONE)
//			showElInfo(element, "Button element after NOTIFY_RENDER_DRAW_DONE");
	}
    if (event->type == HTML_EVENT_CLICK)
	{
		wcWinExecuteStop(Ctx, 0);
		return (HTML_EVENT_STATUS_HALT);
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}

static HTMLEventStatus wcTestBodyCallback(wcCtx* Ctx,	wcEL element,HTMLEvent* event,	char* param)
{
    switch (event->type)
    {
		case HTML_EVENT_LOAD              :
		{
		wcEL button = wcElNew(Ctx, "input type=button");

			rtp_printf("The new body is loaded");
			//====
			if (button)
			{
				wcDocInsertLast(Ctx, button);
				wcElSetInnerHtml(button, "Click me to release modal dialog window");
				wcElSetEventHandler(button,  ModalDismissCallback);
				wcElSetValue(button, "Clickme");
				showElInfo(button, "Button element after create in load :");
				wcDocRefresh(Ctx);
				showElInfo(button, "Button element after refresh in load :");
			}
			break;
		}
		case HTML_EVENT_UNLOAD              :
			break;
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}

static HTMLEventStatus ClickMeCallback(wcCtx* Ctx,	wcEL element,HTMLEvent* event,	char* param)
{
    if (event->type == HTML_EVENT_CLICK)
	{
		wcElPopEventHandler(element);
	}
	return (HTML_EVENT_STATUS_CONTINUE);

}
static char *EventToString(HTMLEventType type);
static HTMLEventStatus wcTestEventCallback(wcCtx* Ctx,	wcEL element,HTMLEvent* event,	char* param)
{

	// print the event name
#if (VERBOSE)
	rtp_printf("Event>>: %s\n", EventToString(event->type));
#endif
	// dispatch on event ids to appropriate responses.
    switch (event->type)
    {
		case HTML_EVENT_CLICK               :
			OpenModalDialog("Got me. dismiss the modal dialog to proceed");
			break;
		case HTML_EVENT_KEYDOWN             :
		case HTML_EVENT_KEYPRESS            :
		case HTML_EVENT_KEYUP               :
			break;
		case HTML_EVENT_DBLCLICK            :
		case HTML_EVENT_MOUSEDOWN           :
		case HTML_EVENT_MOUSEMOVE           :
		case HTML_EVENT_MOUSEOUT            :
		case HTML_EVENT_MOUSEOVER           :
		case HTML_EVENT_MOUSEUP             :
			break;
		case HTML_EVENT_FOCUS               :
		case HTML_EVENT_UNFOCUS             :
		case HTML_EVENT_LOAD                :
		case HTML_EVENT_UNLOAD              :
		case HTML_EVENT_SUBMIT              :
		case HTML_EVENT_CHANGE              :
		case HTML_EVENT_EDIT                :
		case HTML_EVENT_RESET               :
			break;
		case HTML_EVENT_OBJECT_CREATED       :
		case HTML_EVENT_OBJECT_DESTROYED     :
		case HTML_EVENT_ATTRIBUTE_PARSED     :
		case HTML_EVENT_LOSECAPTURE          :
		case HTML_EVENT_TIMER                :
		case HTML_EVENT_RENDER	            :
		case HTML_FIRST_USER_EVENT           :
		default:
		break;
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}

static char *elTypeToString(wcEL element);

static wcBOOL EnumTestCallback(wcCtx* Ctx, wcEL El, int UserIntParm, void *UserVoidParm)
{
int *count;
	count = (int *) UserVoidParm;
	*count += 1;
#if (VERBOSE)
	char *myId = wcELGetId(Ctx, El);
	if (myId)
		rtp_printf("Element Number %d: type: %s has id: == %s ", *count, elTypeToString(El), myId);
	else
		rtp_printf("Element Number %d: type: %s has id: == %s ", *count, elTypeToString(El), "no Id");
	char *myValue = wcELGetValue(Ctx,El);
	if (myValue)
		rtp_printf(" value ==:%s\n", myValue);
	else
		rtp_printf("\n");


	if (wcElGetChecked(El))	// Call getchecked, should not crash even on non radio /  check box elementsd
		rtp_printf("Element is Checked\n");
	if (wcElGetType(El)==HTML_CHECKBOX_ELEMENT)
	{
		if (wcElGetChecked(El))
			rtp_printf("Check box Element is Checked\n");
		else
			rtp_printf("Check box Element is not Checked\n");
	}
	if (wcElGetType(El)==HTML_RADIO_BUTTON_ELEMENT)
	{
		if (wcElGetChecked(El))
			rtp_printf("Radio Button Element is Checked\n");
		else
			rtp_printf("Radio Button Element is not Checked\n");
	}

#endif
	return wcFALSE;
}


static char *elTypeToString(wcEL element)
{
HTMLElementType elementType;
char *r = 0;
	elementType = wcElGetType(element);

	switch (elementType)
	{
	/// <ADDRESS>
	case HTML_ADDRESS_ELEMENT:
		r = "HTML_ADDRESS_ELEMENT";
		break;
	/// <A>
	case HTML_ANCHOR_ELEMENT:
		r = "HTML_ANCHOR_ELEMENT";
		break;
	/// <AREA>
	case HTML_AREA_ELEMENT:
		r = "HTML_AREA_ELEMENT";
		break;
	/// <BASE>
	case HTML_BASE_ELEMENT:
		r = "HTML_BASE_ELEMENT";
		break;
	/// <BIG>
	case HTML_BIG_ELEMENT:
		r = "HTML_BIG_ELEMENT";
		break;
	/// <BLOCKQUOTE>
	case HTML_BLOCKQUOTE_ELEMENT:
		r = "HTML_BLOCKQUOTE_ELEMENT";
		break;
	/// <BODY>
	case HTML_BODY_ELEMENT:
		r = "HTML_BODY_ELEMENT";
		break;
	/// <B>
	case HTML_BOLD_ELEMENT:
		r = "HTML_BOLD_ELEMENT";
		break;
	/// <BR>
	case HTML_BR_ELEMENT:
		r = "HTML_BR_ELEMENT";
		break;
	/// <INPUT type=submit>
	case HTML_BUTTON_ELEMENT:
		r = "HTML_BUTTON_ELEMENT";
		break;
	/// <CENTER>
	case HTML_CENTER_ELEMENT:
		r = "HTML_CENTER_ELEMENT";
		break;
	/// <INPUT type=checkbox>
	case HTML_CHECKBOX_ELEMENT:
		r = "HTML_CHECKBOX_ELEMENT";
		break;
	/// <CITE>
	case HTML_CITE_ELEMENT:
		r = "HTML_CITE_ELEMENT";
		break;
	/// <CODE>
	case HTML_CODE_ELEMENT:
		r = "HTML_CODE_ELEMENT";
		break;
	/// <DD>
	case HTML_DEFINITION_ELEMENT:
		r = "HTML_DEFINITION_ELEMENT";
		break;
	/// <DL>
	case HTML_DEFINITION_LIST_ELEMENT:
		r = "HTML_DEFINITION_LIST_ELEMENT";
		break;
	/// <DT>
	case HTML_DEFINITION_TERM_ELEMENT:
		r = "HTML_DEFINITION_TERM_ELEMENT";
		break;
	/// <DEL>
	case HTML_DEL_ELEMENT:
		r = "HTML_DEL_ELEMENT";
		break;
	/// <DIV>
	case HTML_DIV_ELEMENT:
		r = "HTML_DIV_ELEMENT";
		break;
	/// <INPUT type=text>
	case HTML_EDIT_STR_ELEMENT:
		r = "HTML_EDIT_STR_ELEMENT";
		break;
	/// <TEXTAREA>
	case HTML_EDITBOX_ELEMENT:
		r = "HTML_EDITBOX_ELEMENT";
		break;
	HTML_ELEMENT;
#if (WEBC_SUPPORT_EMBED)
	///<EMBED>
	case HTML_EMBED_ELEMENT:
		r = "HTML_EMBED_ELEMENT";
		break;
#endif
	/// <EM>
	case HTML_EMPHASIZE_ELEMENT:
		r = "HTML_EMPHASIZE_ELEMENT";
		break;
	/// Abstract class for all block-level elements
	case HTML_FLOW_ELEMENT:
		r = "HTML_FLOW_ELEMENT";
		break;
	/// <FONT>
	case HTML_FONT_ELEMENT:
		r = "HTML_FONT_ELEMENT";
		break;
	/// <FORM>
	case HTML_FORM_ELEMENT:
		r = "HTML_FORM_ELEMENT";
		break;
	/// <FRAME>
	case HTML_FRAME_ELEMENT:
		r = "HTML_FRAME_ELEMENT";
		break;
	/// <FRAMESET>
	case HTML_FRAMESET_ELEMENT:
		r = "HTML_FRAMESET_ELEMENT";
		break;
	/// <H1> ... <H6>
	case HTML_HEADING_ELEMENT:
		r = "HTML_HEADING_ELEMENT";
		break;
	/// <INPUT type=hidden>
	case HTML_HIDDEN_INPUT_ELEMENT:
		r = "HTML_HIDDEN_INPUT_ELEMENT";
		break;
	/// <HR>
	case HTML_HORIZONTAL_RULE_ELEMENT:
		r = "HTML_HORIZONTAL_RULE_ELEMENT";
		break;
	/// <IFRAME>
	case HTML_IFRAME_ELEMENT:
		r = "HTML_IFRAME_ELEMENT";
		break;
	/// <IMG>
	case HTML_IMAGE_ELEMENT:
		r = "HTML_IMAGE_ELEMENT";
		break;
	/// <INPUT type=button>
	case HTML_INPUT_BUTTON_ELEMENT:
		r = "HTML_INPUT_BUTTON_ELEMENT";
		break;
	/// <INPUT type=pass>
	case HTML_INPUT_PASSWORD_ELEMENT:
		r = "HTML_INPUT_PASSWORD_ELEMENT";
		break;
	/// <INPUT type=reset>
	case HTML_INPUT_RESET_ELEMENT:
		r = "HTML_INPUT_RESET_ELEMENT";
		break;
	/// <INS>
	case HTML_INS_ELEMENT:
		r = "HTML_INS_ELEMENT";
		break;
	/// <I>
	case HTML_ITALIC_ELEMENT:
		r = "HTML_ITALIC_ELEMENT";
		break;
	/// <KBD>
	case HTML_KBD_ELEMENT:
		r = "HTML_KBD_ELEMENT";
		break;
	/// <LABEL>
	case HTML_LABEL_ELEMENT:
		r = "HTML_LABEL_ELEMENT";
		break;
	/// <UL>; <OL>
	case HTML_LIST_ELEMENT:
		r = "HTML_LIST_ELEMENT";
		break;
	/// <LI>
	case HTML_LIST_ITEM_ELEMENT:
		r = "HTML_LIST_ITEM_ELEMENT";
		break;
	/// <MAP>
	case HTML_MAP_ELEMENT:
		r = "HTML_MAP_ELEMENT";
		break;
	/// <NOBR>
	case HTML_NOBR_ELEMENT:
		r = "HTML_NOBR_ELEMENT";
		break;
	/// <OPTION>
	case HTML_OPTION_ELEMENT:
		r = "HTML_OPTION_ELEMENT";
		break;
	/// <P>
	case HTML_PARAGRAPH_ELEMENT:
		r = "HTML_PARAGRAPH_ELEMENT";
		break;
	/// <PRE>
	case HTML_PRE_ELEMENT:
		r = "HTML_PRE_ELEMENT";
		break;
	/// <INPUT type=radio>
	case HTML_RADIO_BUTTON_ELEMENT:
		r = "HTML_RADIO_BUTTON_ELEMENT";
		break;
	/// <SCRIPT>
	case HTML_SCRIPT_ELEMENT:
		r = "HTML_SCRIPT_ELEMENT";
		break;
	/// <SELECT>
	case HTML_SELECT_ELEMENT:
		r = "HTML_SELECT_ELEMENT";
		break;
	/// <SMALL>
	case HTML_SMALL_ELEMENT:
		r = "HTML_SMALL_ELEMENT";
		break;
	/// <SPACER>
	case HTML_SPACER_ELEMENT:
		r = "HTML_SPACER_ELEMENT";
		break;
	/// <SPAN>
	case HTML_SPAN_ELEMENT:
		r = "HTML_SPAN_ELEMENT";
		break;
	/// <STRIKE>
	case HTML_STRIKE_ELEMENT:
		r = "HTML_STRIKE_ELEMENT";
		break;
	/// HTML text data
	case HTML_STRING_ELEMENT:
		r = "HTML_STRING_ELEMENT";
		break;
	/// <STRONG>
	case HTML_STRONG_ELEMENT:
		r = "HTML_STRONG_ELEMENT";
		break;
	/// <STYLE>
	case HTML_STYLE_ELEMENT:
		r = "HTML_STYLE_ELEMENT";
		break;
	/// <TABLE>
	case HTML_TABLE_ELEMENT:
		r = "HTML_TABLE_ELEMENT";
		break;
	/// <TD>
	case HTML_TABLE_CELL_ELEMENT:
		r = "HTML_TABLE_CELL_ELEMENT";
		break;
	/// <TR>
	case HTML_TABLE_ROW_ELEMENT:
		r = "HTML_TABLE_ROW_ELEMENT";
		break;
	/// <U>
	case HTML_UNDERLINE_ELEMENT:
		r = "HTML_UNDERLINE_ELEMENT";
		break;
	default:
		r = 0;
		break;
	};
	return r;
};
static char *EventToString(HTMLEventType type)
{
	char *r = 0;
	switch (type) {
	case HTML_EVENT_CLICK               : r = "Single mouse click"; break;
	case HTML_EVENT_DBLCLICK            : r = "Double mouse click"; break;
	case HTML_EVENT_KEYDOWN             : r = "Key pressed"; break;
	case HTML_EVENT_KEYPRESS            : r = "Key pressed and released"; break;
	case HTML_EVENT_KEYUP               : r = "Key released"; break;
	case HTML_EVENT_MOUSEDOWN           : r = "Mouse button pressed"; break;
	case HTML_EVENT_MOUSEMOVE           : r = "Mouse movement over this object"; break;
	case HTML_EVENT_MOUSEOUT            : r = "Mouse pointer exits the client area of this object"; break;
	case HTML_EVENT_MOUSEOVER           : r = "Mouse pointer enters the client area"; break;
	case HTML_EVENT_MOUSEUP             : r = "Mouse button released"; break;
	case HTML_EVENT_FOCUS               : r = "Object has received input focus"; break;
	case HTML_EVENT_UNFOCUS             : r = "Object will lose input focus"; break;
	case HTML_EVENT_LOAD                : r = "Load completes"; break;
	case HTML_EVENT_UNLOAD              : r = "Object is discarded"; break;
	case HTML_EVENT_SUBMIT              : r = "For forms only; submit is pressed"; break;
	case HTML_EVENT_CHANGE              : r = "Input text was edited"; break;
	case HTML_EVENT_EDIT                : r = "Finished Editing on a Textbox"; break;
	case HTML_EVENT_RESET               : r = "For forms only; reset is pressed"; break;
	case HTML_EVENT_OBJECT_CREATED       : r = "HTMLElement object created"; break;
	case HTML_EVENT_OBJECT_DESTROYED     : r = "HTMLElement object destroyed"; break;
	case HTML_EVENT_ATTRIBUTE_PARSED     : r = "Attribute parsed"; break;
	case HTML_EVENT_LOSECAPTURE          : r = "Mouse capture released"; break;
	case HTML_EVENT_TIMER                : r = "A timer message sent by user code via wcQueueEventTimed"; break;
	case HTML_EVENT_RENDER	            : r = "A notification message send by the display processing loop about the element. Useful for doing things like retrieveing an element's screen position after it was automatically placed in non fixed formatted content."; break;
	case HTML_FIRST_USER_EVENT           : r = "This and higher event numbers are available for application programming."; break;
	default :
		r = "Unknown event ?";
	}
	return r;
}
