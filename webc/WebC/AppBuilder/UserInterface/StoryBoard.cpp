/*
	StoryBoard class.

		Initialized at stratup.

		Drag drop handler drop calls DropObjectOnStoryBoard() to create an object.
		Objects can be dragged.
		Properties .

		When dragging stops it call CheckStoryBoardPalleteObjectCollision() and if there is a collision
		calls DropObjectOnStoryBoard(). Passing the Id of the object to the storyboard input.

	References.
		StoryBoard div in the main document.

	Note: A table is tracked via the Private data pointer but all that is reaaly needed is the element Id for Identification.
	Should enumerate and get all Ids instead.

	Clean up later.

	Public Functions.
		int ConstructwebCStoryBoard(HDOC_HANDLE hdoc)
		int CheckStoryBoardPalleteObjectCollision(int X, int Y)
		int DropObjectOnStoryBoard(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, int X, int Y, char *ObjectId)

	Called Functions:
		DropObjectOnStoryBoard(hbrowser, hdoc, DragInfo->DraggingElementCurrentX, DragInfo->DraggingElementCurrentY,pwebcPalleteObject->objHTMLId);

*/


#include "../include/NewApi.h"
#include "../include/AppBuilder.h"



struct webcStoryBoardObject	{
	HELEMENT_HANDLE h;
	struct webcStoryBoardObject *pnext;
//	char   *objHTMLId;
//	int X;
//	int Y;
//	char *src;
};

struct _webcStoryBoard	{
	HELEMENT_HANDLE h;					/* Pallet div */
 	struct webcStoryBoardObject *rootwebcStoryBoardObject;
	int Y;
	int Y2;
	int X;
	int X2;
	int H;
	int W;
//	char *src;
};

static 	struct _webcStoryBoard webcStoryBoard;

static void BindPalleteHtmlToC(HDOC_HANDLE hdoc);
static void InitializeTestPallete(HDOC_HANDLE hdoc);

static HTMLEventStatus StoryBoardAppObjectEventHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param);
static HELEMENT_HANDLE	StoryBoard_ElementAppendInnerHtmlASCII(char *newhtml);

int ConstructwebCStoryBoard(HDOC_HANDLE hdoc)
{
	webcStoryBoard.h = webc_DocFindElement (hdoc, "StoryBoard", 0, HTML_ELEMENT_ANY, 0);
	if (!webcStoryBoard.h)
		return -1;
	/* Get the goordinates of the Pallete */
	webc_ElementGetXYWHPosition(webcStoryBoard.h, &webcStoryBoard.X, &webcStoryBoard.Y, &webcStoryBoard.W, &webcStoryBoard.H);
	webcStoryBoard.X2 = webcStoryBoard.X+webcStoryBoard.W-1;
	webcStoryBoard.Y2 = webcStoryBoard.Y+webcStoryBoard.H-1;
	return 0;
}

int CheckStoryBoardPalleteObjectCollision(int X, int Y)
{
	if (X >= webcStoryBoard.X && X <= webcStoryBoard.X2 &&
		Y >= webcStoryBoard.Y && Y <= webcStoryBoard.Y2)
	{
		return 1;
	}
	return 0;
}
static int SetElementDefaultsOnDrop(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, int X, int Y, char *ObjectId,IXML_Element *newxmlElement)
{
char xmlbuff[256];
	if (newxmlElement)
	{
		rtp_sprintf(xmlbuff,"%d",X);
			if (rtpxmlSetElemTextInElem (rtpxmlGetElemInElemFromDomStr(newxmlElement, "HTMLMetadata"), "x", xmlbuff)!=0) goto error;
		rtp_sprintf(xmlbuff,"%d",Y);
			if (rtpxmlSetElemTextInElem (rtpxmlGetElemInElemFromDomStr(newxmlElement, "HTMLMetadata"), "y", xmlbuff)!=0) goto error;
	}
	return 0;
error:
	return -1;
}

int DropObjectOnStoryBoard(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, int X, int Y, char *ObjectId)
{
char newhtml[256];
IXML_Element *newxmlElement;
HELEMENT_HANDLE newhtmlElement=0;
	newhtml[0] = 0;
	Y = Y-webcStoryBoard.Y;
	X = X-webcStoryBoard.X;

	/* Create an xml record of the object */
	newxmlElement = newxmlAppElement(hbrowser, hdoc, ObjectId);
	if (!newxmlElement)
		goto error;
	if (SetElementDefaultsOnDrop(hbrowser, hdoc, X, Y, ObjectId,newxmlElement) != 0)
		goto error;
	if (xmlAppElementToHtml(newxmlElement, newhtml) != 0)
		goto error;
	if (newhtml[0])
		newhtmlElement=StoryBoard_ElementAppendInnerHtmlASCII(newhtml);
	IXML_Element *WebCAppobjects = rtpxmlGetElemInDocFromDomStr(TestProject, STORYBOARDDOMSTR);
	if (!WebCAppobjects)
		goto error;
	if (ixmlNode_appendChild((IXML_Node *) WebCAppobjects, (IXML_Node *) newxmlElement) != IXML_SUCCESS)
		goto error;
	webc_ElementSetPrivateData(newhtmlElement,(void *)newxmlElement);
	webc_ElementSetTagEventHandler (newhtmlElement, StoryBoardAppObjectEventHandler);

	return 0;
error:
	printf("Error dropping object on storyboard %s\n", ObjectId);
  	return -1;
}

int LoadStoryBoardFromSreenDocument(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, IXML_Document *Doc)
{
char newhtml[256];
IXML_Element *ParentElement;
IXML_Element *xmlElement;
	newhtml[0] = 0;

	xmlElement = 0;
	ParentElement = rtpxmlGetElemInDocFromDomStr(Doc, STORYBOARDDOMSTR); /* "WebCAppobjects" */

	if (ParentElement)
		xmlElement = (IXML_Element *)ixmlNode_getFirstChild((IXML_Node*) ParentElement);
	while (xmlElement)
	{
		if (xmlAppElementToHtml(xmlElement, newhtml) != 0)
			goto error;
		if (newhtml[0])
		{
			HELEMENT_HANDLE Elem = StoryBoard_ElementAppendInnerHtmlASCII(newhtml);
			if (!Elem)
				goto error;
			webc_ElementSetPrivateData(Elem,(void *)xmlElement);
			webc_ElementSetTagEventHandler (Elem, StoryBoardAppObjectEventHandler);

		}
		xmlElement = (IXML_Element *) ixmlNode_getNextSibling((IXML_Node*) xmlElement);
	}
	return 0;
error:
	printf("Error LoadStoryBoardFromSreenDocument()\n");
  	return -1;
}

static HTMLEventStatus StoryBoardAppObjectEventHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param)
{
	IXML_Element *xmlElement;
	switch (event->type)
	{
		case HTML_EVENT_CLICK:
		{
			xmlElement = (IXML_Element *) webc_ElementGetPrivateData(hElem);
			if (xmlElement)
				ShowEventDialog(hdoc, hElem,xmlElement);
			return (HTML_EVENT_STATUS_HALT);
		}
		default:
			break;
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}


/* These are needed because appendInnerHTML does not preserve user data and Handlers */
static int nElems;
static void *saved[32000];
static void StoryBoardFixPushUserData(void)
{
HELEMENT_HANDLE Elem;
	nElems = 0;
	Elem = webc_ElementGetFirstChild (webcStoryBoard.h);
	while (Elem)	{
		saved[nElems++] = webc_ElementGetPrivateData(Elem);
		Elem = webc_ElementGetNextSibling (Elem);
	}
}
static void StoryBoardFixPopUserData(void)
{
HELEMENT_HANDLE Elem;
	int i = 0;
	Elem = webc_ElementGetFirstChild (webcStoryBoard.h);
	while (Elem && i < nElems)	{
		if (i < nElems && saved[i])
		{
			webc_ElementSetTagEventHandler (Elem, StoryBoardAppObjectEventHandler);
			webc_ElementSetPrivateData(Elem,saved[i]);
		}
		Elem = webc_ElementGetNextSibling (Elem);
		i++;
	}
}

static HELEMENT_HANDLE	StoryBoard_ElementAppendInnerHtmlASCII(char *newhtml)
{
HELEMENT_HANDLE	newhtmlElement;
	StoryBoardFixPushUserData();
	newhtmlElement=webc_ElementAppendInnerHtmlASCII(webcStoryBoard.h, newhtml);
	StoryBoardFixPopUserData();
	return newhtmlElement;
}
