/*
	Pallete class.
		Called once at stratup.
		Attaches drag drop handlers to all objects inside the Pallete div in the main document.
		When dragging stops it call CheckStoryBoardPalleteObjectCollision() and if there is a collision
		calls DropObjectOnStoryBoard(). Passing the Id of the object to the storyboard input.

	References.
		Pallete div in the main document.

	Note: A table is tracked via the Private data pointer but all that is reaaly needed is the element Id for Identification.
	Should enumerate and get all Ids instead.

	Clean up later.

	Public Functions.
		int ConstructwebCPalleteObject(HDOC_HANDLE hdoc)
	Called Functions:
		DropObjectOnStoryBoard(hbrowser, hdoc, DragInfo->DraggingElementCurrentX, DragInfo->DraggingElementCurrentY,pwebcPalleteObject->objHTMLId);

*/

#include "../include/NewApi.h"
#include "../include/AppBuilder.h"
int TestSlidingWindowOpen(HBROWSER_HANDLE hbrowser,	HDOC_HANDLE hdoc);


static void BindPalleteHtmlToC(HDOC_HANDLE hdoc);
static int LoadPalleteFromPalletTemplate(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);

int ConstructwebCPalleteObject(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{
	LoadPalleteFromPalletTemplate(hbrowser, hdoc);
	BindPalleteHtmlToC(hdoc);

	return 0;
}

static DraggingCallbackResponse DragCb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc,	DraggingCallbackType CallType, struct _webcDraghandle *DragInfo)
{
#ifdef NOTDEF
struct webcPalleteObject *pwebcPalleteObject=0;
#endif
HELEMENT_HANDLE webcPalleteElement = 0;
   switch (CallType) {
   	case DRAGGING:
   		//return STOPDRAGGING;
   		return KEEPDRAGGING;
   		break;
   	case DONEDRAGGING:
		/* Hide the dragged element */
		webc_ElementHide(DragInfo->DraggingElement, WEBC_TRUE);
		/* Drop onto the storyboard ? */
		if (CheckStoryBoardPalleteObjectCollision(DragInfo->DraggingElementCurrentX, DragInfo->DraggingElementCurrentY))
		{
			char *p = webc_ElementGetIdASCII(DragInfo->DraggingElement);
			if (p)
			{
				char *ptoo = p+strlen("Shadow");
				int X,Y,W,H;
				X=Y=W=H=0;
	 			webc_ElementGetXYWHPosition(DragInfo->DraggingElement, &X, &Y, &W, &H);
				DropObjectOnStoryBoard(hbrowser, hdoc, X, Y, ptoo);
				webc_FreeASCIIString(p);
			}
		}
		return RESETDRAGGING;	/* Either one will work but reset puts everything back. */
//		return STOPDRAGGING;
		break;
   	default:
   		return KEEPDRAGGING;
   }
}
static void DragStart(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event)
{
	char ShadowId[80];
	char xmlDomstr[512];
	char *p;
	ShadowId[0]=0;
	rtp_strcat(ShadowId, "Shadow");
	p = webc_ElementGetIdASCII(helem);
	if (p)
	{
		rtp_sprintf(xmlDomstr,"%s.%s",PALLETDOMSTR,p);
		rtp_strcat(ShadowId, p);
		webc_FreeASCIIString(p);
	}
	else
		goto error;
	int X;
	int Y;
	IXML_Element *xmlElement;
	xmlElement = rtpxmlGetElemInDocFromDomStr(PalletTemplate,xmlDomstr);
	if (rtpxmlGetElemTextInElem (xmlElement, "left"))
		X = rtp_atoi(rtpxmlGetElemTextInElem (xmlElement, "left"));
	else
		X = 0;
	if (rtpxmlGetElemTextInElem (xmlElement, "top"))
		Y = rtp_atoi(rtpxmlGetElemTextInElem (xmlElement, "top"));
	else
		Y = 0;
	HELEMENT_HANDLE h = webc_DocFindElement (hdoc, ShadowId, 0, HTML_ELEMENT_ANY, 0);
	if (h)
	{
		webc_ElementSetXYPosition(h, X, Y);
		webc_ElementSetZindex(h, 2000);
		webc_ElementHide(h, WEBC_FALSE);
		webc_DragStart(hbrowser, hdoc, h, X, Y, DragCb, event->data.position.x,event->data.position.y);
	}
	return;
error:
	assert(0);
	;
}

static HTMLEventStatus PalleteElementEventHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
	if (event->type == HTML_EVENT_SUBMIT) // HEREHER
		printf("xxxxx\n");
	if (event->type == HTML_EVENT_MOUSEDOWN)
	{
		DragStart(hbrowser, hdoc, helem, event);
		return HTML_EVENT_STATUS_HALT;
	}
	if (event->type == HTML_EVENT_KEYDOWN)
			printf("Keydown in Sliding");
	return (HTML_EVENT_STATUS_CONTINUE);
}

static void BindPalleteHtmlToC(HDOC_HANDLE hdoc)
{
HELEMENT_HANDLE Elem, parent;
	parent = webc_DocFindElement (hdoc, "Pallete", 0, HTML_ELEMENT_ANY, 0);
	Elem = webc_ElementGetFirstChild (parent);
	while (Elem)
	{
    	webc_ElementSetTagEventHandler (Elem, PalleteElementEventHandler);
		Elem = webc_ElementGetNextSibling (Elem);
	}
}
static int _LoadPalleteFromPalletTemplate(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, int isShadow);

static int LoadPalleteFromPalletTemplate(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{
	if (_LoadPalleteFromPalletTemplate(hbrowser, hdoc, 0) == 0)
		return _LoadPalleteFromPalletTemplate(hbrowser, hdoc, 1);
	return -1;
}

static int _LoadPalleteFromPalletTemplate(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, int isShadow)
{
char newhtml[256];
IXML_Element *ParentElement;
IXML_Element *xmlElement;
IXML_Document *Doc = PalletTemplate;
char *lbuffer,*p;
HELEMENT_HANDLE h;

	p = (char *) rtp_malloc(100000);
	lbuffer = p;
	*p = 0;
	newhtml[0] = 0;
	if (isShadow)
		h = webc_DocFindElement (hdoc, "ShadowPallete", 0, HTML_ELEMENT_ANY, 0);
	else
		h = webc_DocFindElement (hdoc, "Pallete", 0, HTML_ELEMENT_ANY, 0);
	if (!h)
		goto error;
	xmlElement = 0;
	ParentElement = rtpxmlGetElemInDocFromDomStr(Doc, PALLETDOMSTR); /* "PalletImages" */
	if (ParentElement)
		xmlElement = (IXML_Element *)ixmlNode_getFirstChild((IXML_Node*) ParentElement);
	while (xmlElement)
	{
		if (xmlPalletElementToHtml(xmlElement, newhtml, isShadow) != 0)
			goto error;
		rtp_strcat(p,newhtml); p += rtp_strlen(p);
		xmlElement = (IXML_Element *) ixmlNode_getNextSibling((IXML_Node*) xmlElement);
	}
	webc_ElementSetInnerHtmlASCII (h, lbuffer);
	if (lbuffer)
		rtp_free(lbuffer);
	return 0;
error:
	if (lbuffer)
		rtp_free(lbuffer);
	printf("Error CreatePalleteFromPalletTemplate()\n");
	assert(0);
  	return -1;
}

