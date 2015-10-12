/*
	Note: Send Mitel a note:

		Added Cancel timer call to element destructor, cancels any pending timers for the event.

	Notes: Sliding window API
		List elements in a background window (div) that slides under a peek through window.

	The background window is draggable if it is larger than the peek through window.

	Uses new webC features (Feb 2011):
		Uses New event HTML_EVENT_RENDER submessage event->data.NotifyMessage.NotifyEvent = NOTIFY_RENDER_DRAW_DONE
			Note:
				webc_ElementEnableDisplayEvents() has to be called to attach listener.
				This function will crashed if called on element that are not Flowdisplays (Div (maybe anchor, check)
				Note: Need to leak chack the new event type.

			Note: The HTML_EVENT_RENDER event passes notify status from the display engine to the Element message handler.
				NOTIFY_RENDER_DRAW_START
				NOTIFY_RENDER_DRAW_DONE
				Others to be added.

			We don't set the sizes on lists we create so FixUpStyleSizeFromDisplayandParentSize() is called by
			the NOTIFY_RENDER_DRAW_DONE handler to set the height and width in the element's style to the value
			that was rendered.


	What is still needed:

		Need an API - Currently only have TestSlidingTextList()
		Need additional api's for other type other than text.
		Need api to enumerate a div and turn all elements into selection.
			Need push user data, pop user data.
			Need push user handler, pop handler.
			Need change to webc_assert.
*/


#include "../include/NewApi.h"
#include "rtpmem.h"
#include "helement.hpp"
#include "delement.hpp"
#include "dmanager.hpp"
#include <assert.h>



struct _webcSlidingWindowView	{
	int WindowTop;							/* Dimensions of peek window */
	int WindowLeft;
	int WindowHeight;
	int WindowWidth;
	HELEMENT_HANDLE SlidingElement;			/* Element we are sliding */
	int startSlidingNextTick;
	int Sliding;
	int SlidingElementStartTop;
	int SlidingElementStartLeft;
	int SlidingElementCurrentTop;
	int SlidingElementCurrentLeft;
	unsigned long Controlflags;			   /* SLIDINGWINDOW_VERTICAL, SLIDINGWINDOW_HORIZONTAL */
	int StartPointerX;
	int StartPointerY;
	SlidingCallbackFunction cb;			  /* Function to call when an item is selected */
};


static HELEMENT_HANDLE webc_GetSlidingWindow(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);
static HTMLEventStatus SlidingWindowEventHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param);
static int DestroySlidingWindow(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, struct _webcSlidingWindowView *slideObject);

/* Start dragging helem. */
int webc_SlidingWindowOpen(
	HBROWSER_HANDLE hbrowser,
	HDOC_HANDLE hdoc,
	int WindowTop,
	int WindowLeft,
	int WindowHeight,
	int WindowWidth,
	HELEMENT_HANDLE SlidingElement,
	int SlidingElementStartTop,
	int SlidingElementStartLeft,
	unsigned long Controlflags,
	SlidingCallbackFunction cb)
{
struct _webcSlidingWindowView *SlideObject;
HELEMENT_HANDLE SlideWindow;
char style_buff[512];

	SlideWindow = webc_GetSlidingWindow(hbrowser, hdoc);
	if (!SlideWindow)
		goto error;

	SlideObject = (struct _webcSlidingWindowView *) rtp_malloc(sizeof(* SlideObject));
	if (!SlideObject)
		goto error;
	rtp_memset(SlideObject,0,sizeof(*SlideObject));
//	SlideObject->WindowRect.Set(WindowLeft, WindowTop, WindowLeft+WindowWidth-1, WindowTop+WindowHeight-1);

	SlideObject->WindowTop     = WindowTop;
	SlideObject->WindowLeft    = WindowLeft;
	SlideObject->WindowHeight  = WindowHeight;
	SlideObject->WindowWidth   = WindowWidth;
	SlideObject->Controlflags  = Controlflags;
	SlideObject->SlidingElement = SlidingElement;

	SlideObject->SlidingElementStartTop    =  SlidingElementStartTop;
	SlideObject->SlidingElementStartLeft   =  SlidingElementStartLeft;
	SlideObject->SlidingElementCurrentTop  =  SlidingElementStartTop;
	SlideObject->SlidingElementCurrentLeft =  SlidingElementStartLeft;
	SlideObject->cb = cb;

	/* Install a listener for display events */
	webc_ElementEnableDisplayEvents(SlidingElement);
	/* Move the start position of the element underneath us and show it */
	/* Done below too, may not be needed. */
	webc_ElementSetXYPosition(SlidingElement, SlidingElementStartLeft, SlidingElementStartTop);
	/* Hide the sliding window */
  	webc_ElementHide(SlidingElement, WEBC_TRUE);
	/* Put it inside the SlideWindow */
	if (webc_ElementGetParent(SlidingElement))
		webc_ElementRemoveChild(webc_ElementGetParent(SlidingElement),SlidingElement);
	webc_ElementInsertFirst(SlideWindow,SlidingElement);
	/* Set an event handler */
	webc_ElementSetPrivateData(SlidingElement,SlideObject);
	webc_ElementSetTagEventHandler(SlidingElement, SlidingWindowEventHandler);

	/* Show the viewport with the sliding window inside it, clipped. */
	rtp_sprintf(style_buff,"height:%dpx;width:%dpx;\"", WindowHeight, WindowWidth);
    webc_ElementSetStyleASCII(SlideWindow, style_buff, WEBC_TRUE);
	webc_ElementSetXYPosition(SlideWindow, WindowLeft, WindowTop);
   	webc_ElementHide(SlideWindow, WEBC_FALSE);

	/* Move the start position of the element underneath us and show it */
	webc_ElementSetXYPosition(SlidingElement, SlidingElementStartLeft, SlidingElementStartTop);
	webc_ElementHide(SlidingElement, WEBC_FALSE);

	return 0;
error:
	assert(0);
	return -1;
}

static DraggingCallbackResponse SlidingWindowDragCb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc,	DraggingCallbackType CallType, struct _webcDraghandle *DragInfo)
{
	struct _webcSlidingWindowView *slideObject = (struct _webcSlidingWindowView *) webc_ElementGetPrivateData(DragInfo->DraggingElement);
	if (slideObject && CallType==DONEDRAGGING)
	{
		slideObject->Sliding = 0; // Tell SlidingWindowEventHandler we're not dragging
		slideObject->SlidingElementCurrentTop = DragInfo->DraggingElementCurrentY;
		slideObject->SlidingElementCurrentLeft = DragInfo->DraggingElementCurrentX;
   }
   return KEEPDRAGGING;
}

static void FixUpStyleSizeFromDisplayandParentSize(HELEMENT_HANDLE hElem);

static HTMLEventStatus SlidingWindowEventHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param)
{
int scrollvalueX = 0;
int scrollvalueY = 0;

    switch (event->type)
    {
		case HTML_EVENT_RENDER:
			// On draw done we know that we are rendered, so set the style accordingly since
			// we did not use a set size and instead let the renderer calculate our size.
			if (event->data.NotifyMessage.NotifyEvent == NOTIFY_RENDER_DRAW_DONE)
				FixUpStyleSizeFromDisplayandParentSize(hElem);
		break;
        case HTML_EVENT_TIMER:
		{
			struct _webcSlidingWindowView *slideObject = (struct _webcSlidingWindowView *) webc_ElementGetPrivateData(hElem);
			if (slideObject && slideObject->startSlidingNextTick)
			{
				FixUpStyleSizeFromDisplayandParentSize(hElem);
				slideObject->startSlidingNextTick = 0;
 				if (!slideObject->Sliding)
				{
					if (slideObject)
					{
					struct _webcDraghandle *pDrag;
						slideObject->SlidingElementCurrentTop  =  slideObject->SlidingElementCurrentTop;
						slideObject->SlidingElementCurrentLeft =  slideObject->SlidingElementCurrentLeft;
						slideObject->Sliding = 1;
						pDrag = webc_DragStart(hbrowser, hdoc, hElem, slideObject->SlidingElementCurrentLeft, slideObject->SlidingElementCurrentTop, SlidingWindowDragCb,	slideObject->StartPointerX,slideObject->StartPointerY);
						if (pDrag)
						{
							int X, Y, W, H;
							webc_ElementGetXYWHPosition(hElem, &X, &Y, &W, &H);

							pDrag->DraggingFlags |= (HASXMIN|HASXMAX);
							pDrag->DraggingFlags |= (HASYMIN|HASYMAX);
							if (slideObject->Controlflags & SLIDINGWINDOW_VERTICAL)
								pDrag->DraggingElementminX = pDrag->DraggingElementmaxX = slideObject->SlidingElementStartLeft;
							else
							{
								if (slideObject->WindowWidth >= W)
									pDrag->DraggingElementminX = 0;
								else
									pDrag->DraggingElementminX = -1 * (W - slideObject->WindowWidth);
								pDrag->DraggingElementmaxX = 0; // slideObject->SlidingElementStartLeft + W;
							}
							if (slideObject->Controlflags & SLIDINGWINDOW_HORIZONTAL)
								pDrag->DraggingElementminY = pDrag->DraggingElementmaxY = slideObject->SlidingElementStartTop;
							else
							{
								if (slideObject->WindowHeight >= H)
									pDrag->DraggingElementminY = 0;
								else
									pDrag->DraggingElementminY = -1 * (H - slideObject->WindowHeight);
								pDrag->DraggingElementmaxY = 0; // slideObject->SlidingElementStartTop + H;
							}
						}
					}
				}
			}
			break;
		}
		case HTML_EVENT_MOUSEMOVE:
		case HTML_EVENT_MOUSEUP:
			break;
		case HTML_EVENT_MOUSEOUT:
			webc_ElementFocus(hElem, WEBC_TRUE);
			break;
		case HTML_EVENT_MOUSEDOWN:
		{
			struct _webcSlidingWindowView *slideObject = (struct _webcSlidingWindowView *) webc_ElementGetPrivateData(hElem);
			if (slideObject)
			{
				HTMLEvent newevent;
				slideObject->StartPointerX = event->data.position.x;
				slideObject->StartPointerY = event->data.position.y;
 				newevent.type = HTML_EVENT_TIMER;
				slideObject->startSlidingNextTick = 1;
				webc_BrowserQueueTimedEvent (hbrowser, hElem,&newevent,200);
			}
			webc_ElementFocus(hElem, WEBC_TRUE);
			break;
		}
        case HTML_EVENT_KEYDOWN:
		    if (event->data.key == WGK_ESC)
			{
				struct _webcSlidingWindowView *slideObject = (struct _webcSlidingWindowView *) webc_ElementGetPrivateData(hElem);
				DestroySlidingWindow(hbrowser, hdoc, slideObject);
				return HTML_EVENT_STATUS_HALT;
			}
            break;
        default:
        break;
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}
static int DestroySlidingWindow(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, struct _webcSlidingWindowView *slideObject)
{
HELEMENT_HANDLE SlideWindow;

	SlideWindow = webc_ElementGetParent(slideObject->SlidingElement);
	if (!SlideWindow)
		goto error;
	/* Hide the sliding window */
  	webc_ElementHide(slideObject->SlidingElement, WEBC_TRUE);
	/* Hide the viewport window */
  	webc_ElementHide(SlideWindow, WEBC_TRUE);
	/* Remove the slidewindow from the viewport */
	webc_ElementRemoveChild(SlideWindow,slideObject->SlidingElement);
	/* Delete the selections we */
	webc_ElementDeleteChildren(slideObject->SlidingElement);
	/* Delete the slider */
	webc_ElementDelete(slideObject->SlidingElement);
	/* Delete the viewport window */
	webc_ElementRemoveChild (webc_GetRootContainer(hdoc),SlideWindow);
	webc_ElementDelete(SlideWindow);

// webc_ElementRedraw(webc_GetRootContainer(hdoc));

	rtp_free(slideObject);
	return 0;
error:
	assert(0);
	return -1;
}
static HELEMENT_HANDLE webc_GetSlidingWindow(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{
HELEMENT_HANDLE hElem;
	hElem = webc_CreateElementFromHtml(hbrowser, hdoc, "<div style=\"z-index:2001;overflow:hidden;\"></div>");
	webc_ElementInsertLast(webc_GetRootContainer(hdoc), hElem);
	return hElem;
	//return (webc_DocFindElement(hdoc, "SlidingWindow", 0, HTML_ELEMENT_ANY, 0));
}



static HTMLEventStatus TextListEventHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param)
{
	char *p;
    switch (event->type)
    {
        case HTML_EVENT_CLICK:
			p = webc_ElementGetIdASCII(hElem);
			if (p)
			{
				printf("Selected %s\n", p);
				webc_FreeASCIIString(p);
				struct _webcSlidingWindowView *SlideObject = (struct _webcSlidingWindowView *)
					webc_ElementGetPrivateData(webc_ElementGetParent(hElem));
				if (SlideObject)
					DestroySlidingWindow(hbrowser, hdoc, SlideObject);
			}
			return (HTML_EVENT_STATUS_HALT);
			break;
        default:
        break;
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}


static void FixUpStyleSizeFromDisplayandParentSize(HELEMENT_HANDLE hElem)
{
HTMLElement *pContainer;
DisplayElement *pDisplay;
int X, Y, W, H;
int pX, pY, pW, pH;
HELEMENT_HANDLE ParentElement;
static int CallDepth = 0;
	if (CallDepth)
		return;
	CallDepth++;

	webc_ElementGetXYWHPosition(hElem, &X, &Y, &W, &H);
	if (H!=0||W!=0)
		return;
	pX=pY=pW=pH=0;
	ParentElement = webc_ElementGetParent(hElem);
	if (ParentElement)
		webc_ElementGetXYWHPosition(ParentElement, &pX, &pY, &pW, &pH);

	pContainer = (HTMLElement *)hElem;
	pDisplay = pContainer->GetDisplayElement();
	if (pDisplay)
	{
		W =pDisplay->mRect.Width(); H= pDisplay->mRect.Height();
		if (W!=1 && H!=1)
		{
			if (pW > W) W = pW;
			webc_ElementSetXYWHPosition(hElem, X, Y, W, H);
		}
	}
	CallDepth--;
}

static int FormatSlidingWindowTextList(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE ContainingDiv,char **pSelections, int nSelections)
{
	for (int i = 0; i < nSelections; i++,pSelections++)
	{
	char html_buff[512];
	HELEMENT_HANDLE SelectionDiv, Br;
		for (int i = 0; i < 4; i++)
		{
			Br = webc_CreateElementFromHtml(hbrowser, hdoc, "&nbsp;");if (!Br)	goto error;
			if (webc_ElementInsertLast(ContainingDiv,Br) != 0)goto error;
		}
		rtp_sprintf(html_buff, "<a href=\"anything\" id=\"%s\">%s</a>",*pSelections,*pSelections);
		SelectionDiv = webc_CreateElementFromHtml(hbrowser, hdoc, html_buff);
		if (!SelectionDiv)
			goto error;
		/* Assign a handler here */
		webc_ElementSetTagEventHandler(SelectionDiv, TextListEventHandler);
		/* Insert into the Containing Div */
		if (webc_ElementInsertLast(ContainingDiv,SelectionDiv) != 0)
				goto error;
		Br = webc_CreateElementFromHtml(hbrowser, hdoc, "<br>");
		if (!Br)
			goto error;
		if (webc_ElementInsertLast(ContainingDiv,Br) != 0)
				goto error;
	}
	return 0;
error:
	assert (0);
	return -1;
}


char *labels[] = {
"test01","test02","test03","test04","test05","test06","test07","test08","test09","test10",
"test space 11 I am really wide",
"test12","test13","test14","test15","test16","test17","test18","test19","test20","test21","test22","test23","test24",
"test25","test26","test27","test28","test29","test30","test31","test32","test33","test34","test35","test36",
"test37","test38","test39","test40",};

static int TestSlidingTextList(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);
int TestSlidingWindow(HBROWSER_HANDLE hbrowser,	HDOC_HANDLE hdoc)
{
	TestSlidingTextList(hbrowser, hdoc);
	return 0;
}

static int TestSlidingTextList(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{
HELEMENT_HANDLE Container;
	Container = webc_CreateElementFromHtml(hbrowser, hdoc, "<div style=\"background-color:green;\" ></div>");
	if (!Container)
		goto error;
	FormatSlidingWindowTextList(hbrowser, hdoc, Container, &labels[0], sizeof(labels)/sizeof(labels[0]) );

	int WindowTop = 100;
	int WindowLeft = 200;
	int WindowHeight = 200;
	int WindowWidth = 100;
	int SlidingElementStartTop  =  -100;
	int SlidingElementStartLeft =  0;
	webc_SlidingWindowOpen(
		hbrowser,hdoc,
		WindowTop,
		WindowLeft,
		WindowHeight,
		WindowWidth,
		Container,
		SlidingElementStartTop,
		SlidingElementStartLeft,
		SLIDINGWINDOW_VERTICAL,
		0);
	return 0;
error:
	return -1;
}
