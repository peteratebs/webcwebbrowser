/*
	Dialogues.

*/
#include "../include/NewApi.h"
#include "../include/AppBuilder.h"

HELEMENT_HANDLE currenthtmlStoryBoardElement;
IXML_Element  *currentxmlStoryBoardElement;

static int LoadEventsFromDialogTemplate(IXML_Element **ChildArray,int max);
static HTMLEventStatus EventDialogButtonHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param);
static HTMLEventStatus EventDialogToStyleButtonHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param);
int SynchronizeEventDialogToAppObElement(HDOC_HANDLE hdoc,IXML_Element *xmlAppObElement);

int ConstructEventDialogue(HDOC_HANDLE hdoc)
{
HELEMENT_HANDLE DialogElement,DialogTable;
char *p;
char html_buf[32000];

	DialogElement = webc_DocFindElement (hdoc, "EventDialogueBox", 0, HTML_ELEMENT_ANY, 0);

	p = html_buf;
	if (!DialogElement)
		goto error;
	else
    {
    IXML_Element *ChildArray[100];
    int N;
    	DialogTable = webc_DocFindElement (hdoc, "EventDialogueTable", 0, HTML_ELEMENT_ANY, 0);
		if (!DialogTable)
			goto error;
    	{
			sprintf(p,"<th colspan=\"4\" ><center>Select Event Handlers For This Element</center></th>");	p = p+rtp_strlen(p);
			N = LoadEventsFromDialogTemplate(&ChildArray[0],100);
			/* Format the inner table.
			   Fill with checkboxes. Each checkbox has the id of the xml record it represents */
    		if (N)
    		{
    			for (int row =0; row < 8; row++)
				{
					sprintf(p,"<tr>");	p = p+rtp_strlen(p);
					for (int col =0; col < 4; col++)
					{
						int i;
						i = row*4+col;
						if (i <= N && ixmlElement_getTagName(ChildArray[i]))
						{
							sprintf(p,
								"<td style=\"font-size:8;border-style:none;width:74px;\"><input type=\"checkbox\" id=\"%s\" >%s</td>",
								ixmlElement_getTagName(ChildArray[i]),
								rtpxmlGetElemTextInElem(ChildArray[i], "CHANDLER"));
						}
						else
							sprintf(p," ");
						p = p+rtp_strlen(p);
    				}
    				sprintf(p,"</tr>");	p = p+rtp_strlen(p);
    			}
    		}
    		webc_ElementAppendInnerHtmlASCII(DialogTable, html_buf);
    	}
    }
	HELEMENT_HANDLE Element = webc_DocFindElement (hdoc, "EventDialogApply", 0, HTML_ELEMENT_ANY, 0);
	if (!Element)goto error;
	webc_ElementSetTagEventHandler (Element, EventDialogButtonHandler);
	Element = webc_DocFindElement (hdoc, "EventDialogCancel", 0, HTML_ELEMENT_ANY, 0);
	if (!Element)goto error;
	webc_ElementSetTagEventHandler (Element, EventDialogButtonHandler);
	Element = webc_DocFindElement (hdoc, "EventDialogToStyle", 0, HTML_ELEMENT_ANY, 0);
	if (!Element)goto error;
	webc_ElementSetTagEventHandler (Element, EventDialogToStyleButtonHandler);
	return 0;
error:
	return -1;
}
int ShowEventDialog(HDOC_HANDLE hdoc, HELEMENT_HANDLE htmlStoryBoardElement,IXML_Element *xmlStoryBoardElement)
{
int X, Y, W, H;
	if (SynchronizeEventDialogToAppObElement(hdoc,xmlStoryBoardElement) != 0)
		goto error;
	HELEMENT_HANDLE DialogElement = webc_DocFindElement (hdoc, "EventDialogueBox", 0, HTML_ELEMENT_ANY, 0);
	/* Get XY of the palette element */
	webc_ElementGetXYWHPosition(webc_DocFindElement (hdoc, "Pallete", 0, HTML_ELEMENT_ANY, 0), &X, &Y, &W, &H);
	currenthtmlStoryBoardElement = htmlStoryBoardElement;
	currentxmlStoryBoardElement  = xmlStoryBoardElement;
	/* Move the Dialog there */
	webc_ElementSetXYPosition(DialogElement, X, Y);
	if (DialogElement)
   		webc_ElementHide(DialogElement , WEBC_FALSE);
	return 0;
error:
	return -1;

}

static int _SynchronizeAppObElementWithEventDialog(HDOC_HANDLE hdoc,IXML_Element *xmlAppObElement,int isEpilog);
int SynchronizeEventDialogToAppObElement(HDOC_HANDLE hdoc,IXML_Element *xmlAppObElement)
{	return _SynchronizeAppObElementWithEventDialog(hdoc, xmlAppObElement,WEBC_FALSE);}
int SynchronizeAppObElementToEventDialog(HDOC_HANDLE hdoc,IXML_Element *xmlAppObElement)
{	return _SynchronizeAppObElementWithEventDialog(hdoc, xmlAppObElement,WEBC_TRUE); }

static void HideEventDialogue(HDOC_HANDLE hdoc)
{
HELEMENT_HANDLE DialogElement;
	DialogElement = webc_DocFindElement (hdoc, "EventDialogueBox", 0, HTML_ELEMENT_ANY, 0);
	if (DialogElement)	webc_ElementHide(DialogElement , WEBC_TRUE);
}

static HTMLEventStatus EventDialogButtonHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param)
{
	switch (event->type)
	{
		case HTML_EVENT_CLICK:
		{
			char *Id =webc_ElementGetIdASCII(hElem);
			if (Id)
			{
				if (rtp_strcmp(Id, "EventDialogApply") == 0)
				{
					SynchronizeAppObElementToEventDialog(hdoc,currentxmlStoryBoardElement);
					printf("Applying change and saving the project.\n");
					rtpxmlSaveDocument(TestProject, TESTPROJECT);
				}
				currenthtmlStoryBoardElement = 0;
				currentxmlStoryBoardElement  = 0;
				HideEventDialogue(hdoc);
				webc_FreeASCIIString(Id);
			}
			return (HTML_EVENT_STATUS_HALT);
		}
		default:
			break;
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}

static HTMLEventStatus EventDialogToStyleButtonHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param)
{
	if (event->type == HTML_EVENT_CLICK)
	{
   		HideEventDialogue(hdoc);
		ShowStyleDialog(hdoc, currenthtmlStoryBoardElement,currentxmlStoryBoardElement);
		return (HTML_EVENT_STATUS_CONTINUE);
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}

static int _SynchronizeAppObElementWithEventDialog(HDOC_HANDLE hdoc,IXML_Element *xmlAppObElement,int isEpilog)
{
IXML_Element *ChildArray[100];
char xmlbuff[256];
	int N = LoadEventsFromDialogTemplate(&ChildArray[0],100);
	for (int i=0;i<N;i++)
	{
		char *Id;
		HELEMENT_HANDLE CheckElement;
		CheckElement = webc_DocFindElement(hdoc, (char *)ixmlElement_getTagName(ChildArray[i]), 0, HTML_ELEMENT_ANY, 0);
		if (!CheckElement)
			goto error;
		Id =webc_ElementGetIdASCII(CheckElement);
		if (!Id)
			goto error;
		rtp_sprintf(xmlbuff,"EventMetadata.%s",Id);
		IXML_Element *xmlEventElement = rtpxmlGetElemInElemFromDomStr(xmlAppObElement, xmlbuff);
		/* Id contains the Tag name of the Event. */
		if (!isEpilog)
		{	/* Prolog, check if the event is enabled in the project file and check in the dialog box if it is */
			if (xmlEventElement)
				webc_ElementSetChecked(CheckElement,WEBC_TRUE,WEBC_TRUE);
			else
				webc_ElementSetChecked(CheckElement,WEBC_FALSE,WEBC_TRUE);
		}
		else
		{ /* Epilog, if checked in the dialog put it in the project, if not checked remove from the project */
			if (webc_ElementGetChecked(CheckElement))
			{	/* Make sure it is included in the project */
				/* Look for it in the <EventMetadata> section of the current <WebCAppobjects> */
				if (!xmlEventElement)
				{
					/* Clone it from the WebCComponentClasses.WebCCEvents of the WebCComponents template file */
					rtp_sprintf(xmlbuff,"WebCComponentClasses.WebCCEvents.%s",Id);
					IXML_Element *xmltemplate = rtpxmlGetElemInDocFromDomStr(ComponentTemplate, xmlbuff);
					if (!xmltemplate)  goto error;
					IXML_Element *newxmlElement = (IXML_Element *) ixmlNode_cloneNode((IXML_Node *) xmltemplate, 1);
					if (!newxmlElement)	goto error;
					/* Put the new node into the element */
					IXML_Element *EventMetadata = rtpxmlGetElemInElemFromDomStr(xmlAppObElement, "EventMetadata");
					if (!EventMetadata)	goto error;
					if (ixmlNode_appendChild((IXML_Node *) EventMetadata, (IXML_Node *) newxmlElement) != IXML_SUCCESS)
						goto error;
				}
			}
			else if (xmlEventElement)
			{	/* It is not check, remove it prom the project file if it is there */
				IXML_Element *EventMetadata = rtpxmlGetElemInElemFromDomStr(xmlAppObElement, "EventMetadata");
				IXML_Node *RemovedEvent = 0;
				if (!EventMetadata)	goto error;
				if (ixmlNode_removeChild((IXML_Node*) EventMetadata, (IXML_Node*) xmlEventElement, &RemovedEvent) != IXML_SUCCESS)
					goto error;
				ixmlNode_free((IXML_Node *) RemovedEvent);
			}
		}
		webc_FreeASCIIString(Id);
	}
	return 0;
error:
	return -1;
}

static int LoadEventsFromDialogTemplate(IXML_Element **ChildArray,int max)
{
int N = -1;
    IXML_Element *EventDialogueElement;
   	EventDialogueElement = rtpxmlGetElemInDocFromDomStr(DialogueTemplate, "WebCDialogueClasses.WebCCEvents");
   	if (!EventDialogueElement)
		goto error;
	N = WebcAppGenXmlLoadChildArray(EventDialogueElement, ChildArray, max);
	return N;
error:
	return -1;

}
