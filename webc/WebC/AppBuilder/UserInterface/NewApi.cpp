/*
	To do -
		webc_ElementPushPrivateData();
		webc_ElementPopPrivateData();
   		webc_ElementPushTagEventHandler();
   		webc_ElementPopTagEventHandler();

	Fix up DragDrop interface.
	Dynamically create control sprite.

	Scope start and garbage collect
		webc_QueueGc();
		webc_Gc();
*/

#include "../include/NewApi.h"
#include "rtpmem.h"
#include "dflow.hpp"
#include "helement.hpp"
#include "htmlbrow.hpp"

#include <assert.h>

#if (0)
char *webc_Strmalloc(char *str)
{
	char *rstr = 0 ;
	if (str)
	{
		rstr = (char *)rtp_malloc(rtp_strlen(str)+1);
		rtp_strcpy(rstr, str);
	}
	return rstr;
}
#endif

/* Get display element. (not used.)
   ** Currently must be an HtmlElement, doesn't work for FlowDisplay. Can we tell if it's an Element or flow display ? So we can do both ?.*/
DELEMENT_HANDLE webc_GetDisplayElement(HELEMENT_HANDLE hElem)
{
	DisplayElement *pDisplay=0;
	if (hElem)
		pDisplay = ((HTMLElement *)hElem)->GetDisplayElement();
	return (DELEMENT_HANDLE) pDisplay;
}

/* Attaching listener callbacks to the object so we get HTML_EVENT_RENDER
   ** Currently must be a FlowDisplay. Can we tell if it's an Element or flow display ? So we can do both ?.*/
void webc_ElementEnableDisplayEvents(HELEMENT_HANDLE hElem)
{
	HTMLFlowDisplay *D = (HTMLFlowDisplay *)((HTMLElementContainer *) hElem)->GetDisplayElement();
	if (D)
		D->SetListener(HELEMENT_OBJ(hElem));
}

int webc_SetControlSpritePosition(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, int X, int Y)
{
HELEMENT_HANDLE ControlSprite;
char ascii_buff[80];
	ControlSprite = webc_GetControlSprite(hbrowser, hdoc);
	if (!ControlSprite)
		return -1;
	rtp_sprintf(ascii_buff, "position:absolute;left:%dpx;top:%dpx;", X, Y);
    webc_ElementSetStyleASCII(ControlSprite, ascii_buff,WEBC_TRUE);
	return 0;
}

int webc_ElementSetXYPosition(HELEMENT_HANDLE element, int X, int Y)
{
char ascii_buff[80];
	if (!element)
		return -1;
	rtp_sprintf(ascii_buff, "position:absolute;left:%dpx;top:%dpx;", X, Y);
    webc_ElementSetStyleASCII(element, ascii_buff,WEBC_TRUE);
	HDOC_HANDLE hdoc = webc_ElementGetDocument(element);
    return webc_SetControlSpritePosition(webc_DocGetBrowser(hdoc),hdoc,X,Y);/* Move the control sprite so the element actually moves */
}
int webc_ElementSetXYWHPosition(HELEMENT_HANDLE element, int X, int Y,int W, int H)
{
char ascii_buff[80];
	if (!element)
		return -1;
	rtp_sprintf(ascii_buff, "position:absolute;left:%dpx;top:%dpx;width:%dpx;height:%dpx;", X, Y, W, H);
    webc_ElementSetStyleASCII(element, ascii_buff,WEBC_TRUE);
	HDOC_HANDLE hdoc = webc_ElementGetDocument(element);
    return webc_SetControlSpritePosition(webc_DocGetBrowser(hdoc),hdoc,X,Y);/* Move the control sprite so the element actually moves */
}

int webc_ElementGetXYWHPosition(HELEMENT_HANDLE element, int *X, int *Y, int *W, int *H)
{
 long y,x;
 	if (!element)
		return -1;
 	x=y=0;
 	webc_ElementGetXYPosition(element, &x, &y);
	*Y=y;*X=x;
	*W = webc_ElementGetWidth(element);
	*H = webc_ElementGetHeight(element);
	return 0;
}
int webc_ElementHide(HELEMENT_HANDLE helem, WEBC_BOOL doHide)
{
	if (doHide)
    	return webc_ElementSetStyleASCII(helem, "visibility:hidden;",WEBC_TRUE);
	else
    	return webc_ElementSetStyleASCII(helem, "visibility:visible;",WEBC_TRUE);
}

int webc_ElementSetZindex(HELEMENT_HANDLE helem, int z)
{
char ascii_buff[80];
	rtp_sprintf(ascii_buff, "z-index:%d;", z);
    return webc_ElementSetStyleASCII(helem, ascii_buff,WEBC_TRUE);
}

/* Stop dragging and reset. */
static void _webc_DragReset(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, struct _webcDraghandle *dragObject)
{
HELEMENT_HANDLE ControlSprite;
	webc_ElementSetXYPosition(dragObject->DraggingElement, dragObject->DraggingElementStartX, dragObject->DraggingElementStartY);
	ControlSprite = webc_GetControlSprite(hbrowser, hdoc);
	if (ControlSprite)
	{
		webc_ElementSetPrivateData(ControlSprite,0);
		rtp_free(dragObject);
		webc_ElementHide(ControlSprite, WEBC_TRUE);
		webc_ElementReleasePointer(ControlSprite);
	}
}

static int clipYDrag(struct _webcDraghandle *dragObject,int Y)
{
int inY = Y;
	if ((dragObject->DraggingFlags & HASYMIN) && Y < dragObject->DraggingElementminY)
		Y = dragObject->DraggingElementminY;
	if ((dragObject->DraggingFlags & HASYMAX) && Y > dragObject->DraggingElementmaxY)
		Y = dragObject->DraggingElementmaxY;
	return Y;
}
static int clipXDrag(struct _webcDraghandle *dragObject, int X)
{
	if ((dragObject->DraggingFlags & HASXMIN) && X < dragObject->DraggingElementminX)
		X = dragObject->DraggingElementminX;
	if ((dragObject->DraggingFlags & HASXMAX) && X > dragObject->DraggingElementmaxX)
		X = dragObject->DraggingElementmaxX;
	return X;
}

static HTMLEventStatus ControlSpriteEventHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE ControlSprite, HTMLEvent* event,char * param)
{
struct _webcDraghandle *dragObject;
	switch (event->type)
	{
		case HTML_EVENT_MOUSEMOVE:
		{
			int moveX, moveY;
			dragObject = (struct _webcDraghandle *) webc_ElementGetPrivateData(ControlSprite);
			if (!dragObject)
				break;
			moveX = event->data.position.x - dragObject->DraggingPointerStartX;
			moveY = event->data.position.y - dragObject->DraggingPointerStartY;
			dragObject->DraggingElementCurrentX = clipXDrag(dragObject,dragObject->DraggingElementStartX+moveX);
			dragObject->DraggingElementCurrentY = clipYDrag(dragObject,dragObject->DraggingElementStartY+moveY);
			webc_ElementSetXYPosition(dragObject->DraggingElement, dragObject->DraggingElementCurrentX, dragObject->DraggingElementCurrentY);
			if (dragObject->cb)
			{
				DraggingCallbackResponse r = dragObject->cb(hbrowser, hdoc, DRAGGING, dragObject);
				switch (r) {
					case RESETDRAGGING:
						_webc_DragReset(hbrowser,hdoc, dragObject);
						return (HTML_EVENT_STATUS_HALT);
					case STOPDRAGGING:
						break; 		  	/* Fall through to up handler */
					case KEEPDRAGGING:
					default:
						return (HTML_EVENT_STATUS_HALT);
					}
			}
			else
				break;
		}
		case HTML_EVENT_MOUSEUP:
		{
			dragObject = (struct _webcDraghandle *) webc_ElementGetPrivateData(ControlSprite);
			if (dragObject && dragObject->Signature == DRAGOBJECTMAGICNUMBER)
			{
				if (dragObject->cb)
				{
					DraggingCallbackResponse r = dragObject->cb(hbrowser, hdoc, DONEDRAGGING, dragObject);
					switch (r) {
						case RESETDRAGGING:
							_webc_DragReset(hbrowser,hdoc, dragObject);
							return (HTML_EVENT_STATUS_HALT);
						case STOPDRAGGING:
						case KEEPDRAGGING:
						default:
							break; 		  	/* Fall through to up handler */
						}
				}
				webc_ElementSetPrivateData(ControlSprite,0);
				rtp_free(dragObject);
				webc_ElementHide(ControlSprite, WEBC_TRUE);
				webc_ElementReleasePointer(ControlSprite);
				return (HTML_EVENT_STATUS_HALT);
			}
		}
		case HTML_EVENT_KEYDOWN:
			printf("Keydown in sprite");
		default:
			break;
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}

HELEMENT_HANDLE CreateControlSprite(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{
	printf("No control sprite. Fix this \n");
	return 0;
}
HELEMENT_HANDLE webc_GetControlSprite(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{
HELEMENT_HANDLE	h;

	h = webc_DocFindElement(hdoc, "ControlSprite", 0, HTML_ELEMENT_ANY, 0);
	if (!h)
		h = CreateControlSprite(hbrowser, hdoc);
	if (h)
   		webc_ElementSetTagEventHandler (h, ControlSpriteEventHandler);
	return h;
}

/* Start dragging helem. */
struct _webcDraghandle * webc_DragStart(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, int elemStartX, int elemStartY, DraggingCallbackFunction cb, int x, int y)
{
HELEMENT_HANDLE ControlSprite;
struct _webcDraghandle *dragObject;

	ControlSprite = webc_GetControlSprite(hbrowser, hdoc);
	if (!ControlSprite)
	{
		assert(0);
		return 0;
	}
	dragObject = (struct _webcDraghandle *) rtp_malloc(sizeof(* dragObject));
	if (!dragObject)
		return 0;
	rtp_memset(dragObject,0,sizeof(*dragObject));

	dragObject->DraggingElement = helem;
	dragObject->Signature = DRAGOBJECTMAGICNUMBER;
	dragObject->DraggingElementCurrentX = dragObject->DraggingElementStartX = elemStartX; 	/* Need to validate a way to get position info */
	dragObject->DraggingElementCurrentY = dragObject->DraggingElementStartY = elemStartY;
	dragObject->DraggingPointerStartX = x;
	dragObject->DraggingPointerStartY = y;
	dragObject->cb = cb;

   	webc_ElementSetPrivateData (ControlSprite, (void *)  dragObject);

	/* Unhide the sprite and move it to the drag position */
   	webc_ElementHide(ControlSprite, WEBC_FALSE);
   	webc_SetControlSpritePosition(hbrowser, hdoc,elemStartX, elemStartY);
	webc_ElementClaimPointer(ControlSprite);

	return dragObject;
}

/*****************************************************************************/
/**
 @memo Set an HTML element's inner html

 	void webc_ElementSetInnerHtmlASCII ( HELEMENT_HANDLE element,	char * newhtml)
 @doc

 	Returns the last elemnt in the container, for single object inserts will be
 	the the last element.
 */
/*****************************************************************************/

HELEMENT_HANDLE webc_ElementAppendInnerHtmlASCII(HELEMENT_HANDLE element, char * newhtml)
{
	int OriginalLength = 0;
	char *newstring;
	char *Original = webc_ElementGetInnerHtmlASCII (element);
	if (Original)
	{
		OriginalLength = rtp_strlen(Original);
	}
	int NewLength = OriginalLength+rtp_strlen(newhtml);
	newstring = (char *) rtp_malloc(NewLength+1);
	*newstring = 0;
	if (Original)
	{
		rtp_strcpy(newstring,Original);
		webc_FreeASCIIString(Original);
	}
	rtp_strcat(newstring,newhtml);
	webc_ElementSetInnerHtmlASCII (element,newstring);

	rtp_free(newstring);
	return webc_ElementFixedGetLastChild(element);
}


/* Stop dragging and reset. */
void webc_DragReset(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, struct _webcDraghandle *dragObject)
{
HELEMENT_HANDLE ControlSprite;
	webc_ElementSetXYPosition(dragObject->DraggingElement, dragObject->DraggingElementStartX, dragObject->DraggingElementStartY);
	ControlSprite = webc_GetControlSprite(hbrowser, hdoc);
	if (ControlSprite)
	{
		webc_ElementSetPrivateData(ControlSprite,0);
		rtp_free(dragObject);
		webc_ElementReleasePointer(ControlSprite);
	}
}

void webc_ElementRedraw(HELEMENT_HANDLE element)
{
	HTMLFlowDisplay *D = (HTMLFlowDisplay *)((HTMLElementContainer *) element)->GetDisplayElement();
	if (D)
	{
		D->Invalidate();
//		webc_BrowserInvalidate(hbrowser);
		webc_BrowserDraw (webc_DocGetBrowser(webc_ElementGetDocument(element)));
	}
}

HELEMENT_HANDLE webc_GetRootContainer(HDOC_HANDLE hdoc)
{
	HELEMENT_HANDLE ElResult;
	ElResult = webc_DocFindElement(hdoc, "root", 0, HTML_ELEMENT_ANY, 0);
	return ElResult;
}

HELEMENT_HANDLE webc_ElementFixedGetLastChild(HELEMENT_HANDLE element)
{
HELEMENT_HANDLE Elem, prevElem, result;
	result = Elem = prevElem = 0;
	Elem = (HELEMENT_HANDLE) (HELEMENT_OBJ(element)->FirstChild());
	while (Elem)
	{
		result = Elem;
		(HELEMENT_HANDLE) (HELEMENT_OBJ(Elem)->mpPrev);
		Elem = (HELEMENT_HANDLE) (HELEMENT_OBJ(Elem)->mpPrev);
	}
	return result;

}

/* New function was put in API, may need to document */
//void webc_ElementCancelTimers(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE el)
//{
//	HBROWSER_OBJ(hbrowser)->TimeoutManager()->CancelTimeout(_matchElement, &el, WEBC_TRUE);
//}
