/*
	Dialogues.

*/
#include "../include/NewApi.h"
#include "../include/AppBuilder.h"

extern HELEMENT_HANDLE currenthtmlStoryBoardElement;
extern IXML_Element  *currentxmlStoryBoardElement;

int SynchronizeEventDialogToAppObElement(HDOC_HANDLE hdoc,IXML_Element *xmlAppObElement);
static HTMLEventStatus StyleDialogButtonHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param);
static HTMLEventStatus StyleToEventDialogButtonHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param);
static int LoadChildListFromDialogTemplate(char *DomStr,IXML_Element **ChildArray,int max);
int LoadStyleDialogTemplates(void);
int SynchronizeAppObElementToEventDialog(HDOC_HANDLE hdoc,IXML_Element *xmlAppObElement);
static int FormatStyleDialogBox(HDOC_HANDLE hdoc);;
static int EnumerateStyleListFromHtml(HDOC_HANDLE hdoc, int isProlog);
static HTMLEventStatus StyleSelectionScrollHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param);
static int UpdateProjectElementStyle(char *AttributeName,char *AttributeValue);

static void HideStyleDialogue(HDOC_HANDLE hdoc)
{
HELEMENT_HANDLE DialogElement;
	DialogElement = webc_DocFindElement (hdoc, "StyleDialogueBox", 0, HTML_ELEMENT_ANY, 0);
	if (DialogElement)
   		webc_ElementHide(DialogElement , WEBC_TRUE);
}

int ShowStyleDialog(HDOC_HANDLE hdoc, HELEMENT_HANDLE htmlStoryBoardElement,IXML_Element *xmlStoryBoardElement)
{
int X, Y, W, H;

	if (LoadStyleDialogTemplates()!=0)
		goto error;

	if (SynchronizeEventDialogToAppObElement(hdoc,xmlStoryBoardElement) != 0)
		goto error;
	HELEMENT_HANDLE DialogElement = webc_DocFindElement (hdoc, "StyleDialogueBox", 0, HTML_ELEMENT_ANY, 0);
	if (!DialogElement)
		goto error;

	/* Get XY of the palette element */
	if (webc_ElementGetXYWHPosition(webc_DocFindElement (hdoc, "Pallete", 0, HTML_ELEMENT_ANY, 0), &X, &Y, &W, &H)!=0)
		goto error;
	/* Move the Dialog there */
	webc_ElementSetXYPosition(DialogElement, X, Y);
	/* Assign apply button handler */
	if (webc_ElementSetTagEventHandler (webc_DocFindElement (hdoc, "StyleDialogApply", 0, HTML_ELEMENT_ANY, 0), StyleDialogButtonHandler)!=0)
		goto error;
	/* Assign Cancel button handler */
	if (webc_ElementSetTagEventHandler (webc_DocFindElement (hdoc, "StyleDialogCancel", 0, HTML_ELEMENT_ANY, 0), StyleDialogButtonHandler)!=0)
		goto error;
	/* Assign Cancel button handler */
	if (webc_ElementSetTagEventHandler (webc_DocFindElement (hdoc, "StyleDialogToEvent", 0, HTML_ELEMENT_ANY, 0), StyleDialogButtonHandler)!=0)
		goto error;
	/* Assign switch to event dialog event handler */
	if (webc_ElementSetTagEventHandler (webc_DocFindElement (hdoc, "StyleDialogToEvent", 0, HTML_ELEMENT_ANY, 0), StyleToEventDialogButtonHandler)!=0)
		goto error;
	if (FormatStyleDialogBox(hdoc)!=0)
		goto error;
	if (EnumerateStyleListFromHtml(hdoc, 1)!=0)
		goto error;
	if (webc_ElementSetTagEventHandler(webc_DocFindElement (hdoc, "StyleDialogueBoxInner",0, HTML_ELEMENT_ANY, 0), StyleSelectionScrollHandler)!=0)
		goto error;
	webc_ElementHide(DialogElement , WEBC_FALSE);

	currenthtmlStoryBoardElement = htmlStoryBoardElement;
	currentxmlStoryBoardElement  = xmlStoryBoardElement;
	return 0;
error:
	assert(0);
	return -1;

}
static HTMLEventStatus StyleDialogButtonHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param)
{
	switch (event->type)
	{
		case HTML_EVENT_CLICK:
		{
			char *Id =webc_ElementGetIdASCII(hElem);
			if (Id)
			{
				if (rtp_strcmp(Id, "StyleDialogApply") == 0)
				{
				 	EnumerateStyleListFromHtml(hdoc, 0);

					SynchronizeAppObElementToEventDialog(hdoc,currentxmlStoryBoardElement);
					printf("Saving oiut of bounds\n");
					rtpxmlSaveDocument(TestProject, TESTPROJECT);
				}
				currenthtmlStoryBoardElement = 0;
				currentxmlStoryBoardElement  = 0;
				HideStyleDialogue(hdoc);
				webc_FreeASCIIString(Id);
			}
			return (HTML_EVENT_STATUS_HALT);
		}
		default:
			break;
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}
static HTMLEventStatus StyleToEventDialogButtonHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param)
{
	if (event->type == HTML_EVENT_CLICK)
	{
   		HideStyleDialogue(hdoc);
		ShowEventDialog(hdoc, currenthtmlStoryBoardElement,currentxmlStoryBoardElement);
		return (HTML_EVENT_STATUS_CONTINUE);
//		return (HTML_EVENT_STATUS_HALT);
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}



#define MAXPROPERTIES 256
int  NWebCStyleProperties;
IXML_Element *WebCStyleProperties[MAXPROPERTIES];
#define MAXPROPERTYTYPES 256
int NFieldPropertyTypes;
IXML_Element *FieldPropertyTypes[MAXPROPERTYTYPES];

static int LoadStyleDialogTemplates(void)
{
	if ((NWebCStyleProperties=LoadChildListFromDialogTemplate("root.WebCDialogueClasses.WebCStyleProperties",WebCStyleProperties,MAXPROPERTIES))<=0)
		goto error;
	if ((NFieldPropertyTypes=LoadChildListFromDialogTemplate("root.WebCDialogueClasses.FieldPropertyTypes",FieldPropertyTypes,MAXPROPERTYTYPES))<=0)
		goto error;
	return 0;
error:
	assert(0);
	return -1;
}
static int LoadChildListFromDialogTemplate(char *DomStr,IXML_Element **ChildArray,int max)
{
int N = -1;
    IXML_Element *Element;
   	Element = rtpxmlGetElemInDocFromDomStr(DialogueTemplate, DomStr);
   	if (!Element)
		goto error;
	N = WebcAppGenXmlLoadChildArray(Element, ChildArray, max);
	return N;
error:
	assert(0);
	return -1;

}

static int CharStringToColSpanValue(char *p);
static int StringIntToColSpanValue(char *p);

#define CHARSPERSPAN   4
#define PIXELSPERSPAN 24
#define PIXELSPERSPANH 16
#define NCOLS 16

static char *FormatOneStyleDialogEntry(char *p, int column, int row, int labelwidthspans, int valuewidthspans, char *label, char *value, char *id);
static int FormatStyleDialogBox(HDOC_HANDLE hdoc)
{
char *html_buffer,*p;
int currentRow;
	p = html_buffer = (char *) rtp_malloc(1000000);
	int colsused = 0;
	currentRow = 0;
	for (int j = 0; j < NWebCStyleProperties; j++)
	{
		char *Prompt,*InputType,*Width;
		IXML_Element* element = WebCStyleProperties[j];

		Prompt = (char *)ixmlElement_getTagName(WebCStyleProperties[j]);
		InputType = ixmlElement_getAttribute(element,"WebCStylePropertyType");
		Width = ixmlElement_getAttribute(element,"Width");

		int ncols = CharStringToColSpanValue(Prompt)+StringIntToColSpanValue(Width);
		ncols = 8;
		if (ncols > NCOLS-colsused)
		{
			currentRow += 1;
			colsused = 0;
		}
		int labelwidthspans = 5; // CharStringToColSpanValue(Prompt);
		int valuewidthspans = 3; //StringIntToColSpanValue(Width);
		char *label = Prompt;
		char *value	= "";
		char *id	= Prompt;
	 	p = FormatOneStyleDialogEntry(p, colsused, currentRow, labelwidthspans, valuewidthspans, label, value, id);
		colsused += ncols;
	}
//	StyleDialogueBoxInner
//	HELEMENT_HANDLE helem = webc_DocFindElement (hdoc, "StyleDialogueBox", 0, HTML_ELEMENT_ANY, 0);
	HELEMENT_HANDLE helem = webc_DocFindElement (hdoc, "StyleDialogueBoxInner", 0, HTML_ELEMENT_ANY, 0);
	if (!helem)
	{
		printf("Failure in FormatStyleDialogBox\n");
		assert(0);
		return -1;
	}
// printf("%s",html_buffer);
	webc_ElementSetInnerHtmlASCII (helem, html_buffer);
	rtp_free(html_buffer);
	return 0;
}
static int CharStringToColSpanValue(char *p)
{
	int n = 1 + rtp_strlen(p)/CHARSPERSPAN;
	return n;
}

static int StringIntToColSpanValue(char *p)
{
	int n = 1 + rtp_atoi(p)/PIXELSPERSPAN;
	return n;
}

static char *FormatOneStyleDialogEntry(char *p, int column, int row, int labelwidthspans, int valuewidthspans, char *label, char *value, char *id)
{
int left, top;

	left = column *	PIXELSPERSPAN;
	top  = row * PIXELSPERSPANH;
	rtp_sprintf(p,"<div style=\"position:absolute;left:%dpx;top:%dpx;width:%dpx;height:%dpx;font-size:6;\">", left, top, PIXELSPERSPAN*labelwidthspans,PIXELSPERSPANH);	p+=rtp_strlen(p);
	rtp_sprintf(p,"%s</div>",label);p+=rtp_strlen(p);
	left += PIXELSPERSPAN*labelwidthspans;
	rtp_sprintf(p,"<input style=\"position:absolute;left:%dpx;top:%dpx;font-size:6;width:%dpx;\" type=\"text\" value=\"%s\" id=\"%s\"/></div>",left, top, PIXELSPERSPAN*valuewidthspans,value,id);
	p+=rtp_strlen(p);

//	rtp_sprintf(p,"<div style=\"position:absolute;left:%dpx;top:%dpx;width:%dpx;height:%dpx;font-size:6;\">", left, top, PIXELSPERSPAN*valuewidthspans,PIXELSPERSPANH);	p+=rtp_strlen(p);
//	left += PIXELSPERSPAN;
//	rtp_sprintf(p,"<input style=\"font-size:6;width:%dpx;\" type=\"text\" value=\"%s\" id=\"%s\"/></div>",PIXELSPERSPAN*valuewidthspans,value,id);p+=rtp_strlen(p);
	return(p);
}

static HTMLEventStatus StyleSelectionHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param)
{
    switch (event->type)
    {
//        case HTML_EVENT_CLICK:
        case HTML_EVENT_KEYUP:
            /* Allow control codes */
		    if (event->data.key == WGK_CR)
            {
				char *v  = webc_ElementGetValueASCII (hElem);
            	char *id = webc_ElementGetIdASCII (hElem);
            	if (id)
				{
					printf("id %s changed new value == %s\n", id, v);
					webc_FreeASCIIString(id);
				}
            }
            break;
        default:
        break;
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}

static char *GetStyleValue(char *id)
{
	return 0; // id;
}
static int EnumerateStyleListFromHtml(HDOC_HANDLE hdoc, int isProlog)
{
HELEMENT_HANDLE Elem;
	HELEMENT_HANDLE inner = webc_DocFindElement (hdoc, "StyleDialogueBoxInner", 0, HTML_ELEMENT_ANY, 0);
	if (!inner)
		goto error;
	Elem = webc_ElementGetFirstChild (inner);
	while (Elem)	{
		char *id = webc_ElementGetIdASCII (Elem);
		if (id)
		{
			if (isProlog)
			{
				if (webc_ElementSetTagEventHandler(Elem, StyleSelectionHandler)!=0)
					goto error;
				if (GetStyleValue(id))
					if (webc_ElementSetValueASCII (Elem, GetStyleValue(id), WEBC_TRUE) != 0)
						goto error;

			}
			else
			{
				char *v = webc_ElementGetValueASCII (Elem);
				if (v && *v)
				{
					printf("Element %s has v == %s\n", id, v);
					UpdateProjectElementStyle(id,v);
					webc_FreeASCIIString(v);
				}

			}
			webc_FreeASCIIString(id);
		}
		Elem = webc_ElementGetNextSibling (Elem);
	}
	return 0;
error:
	assert(0);
	return -1;
}
static int startDraggingNextTick;
static int Dragging;
static int DraggingPointerStartX,DraggingPointerStartY;

static HTMLEventStatus StyleSelectionScrollHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE hElem, HTMLEvent* event,char * param)
{
int scrollvalue = 0;
    switch (event->type)
    {
        case HTML_EVENT_TIMER:
			if (startDraggingNextTick)
			{
	 			startDraggingNextTick = 0;
				Dragging = 1;
				webc_ElementClaimPointer(hElem);
			}
			break;
		case HTML_EVENT_MOUSEMOVE:
 			if (startDraggingNextTick)
			{
				DraggingPointerStartX = event->data.position.x;
				DraggingPointerStartY = event->data.position.y;
			}
 			else if (Dragging)
			{
				scrollvalue = event->data.position.y - DraggingPointerStartY;
				DraggingPointerStartX = event->data.position.x;
				DraggingPointerStartY = event->data.position.y;
			}
			break;
		case HTML_EVENT_MOUSEDOWN:
		{
			HTMLEvent newevent;
 			startDraggingNextTick = 1;
			newevent.type = HTML_EVENT_TIMER;
			webc_BrowserQueueTimedEvent (hbrowser, hElem,&newevent,200);
			DraggingPointerStartX = event->data.position.x;
			DraggingPointerStartY = event->data.position.y;
			break;
		}
//		case HTML_EVENT_MOUSEOUT:
		case HTML_EVENT_MOUSEUP:
		{
 			startDraggingNextTick = 0;
			if (Dragging)
			{
				webc_ElementReleasePointer(hElem);
				Dragging = 0;
			}
			break;
		}
        case HTML_EVENT_KEYDOWN:
            /* Allow control codes */
		    if (event->data.key == WGK_PGUP)
            	scrollvalue = -10;
		    if (event->data.key == WGK_PGDN)
            	scrollvalue = 10;

            break;
        default:
        break;
	}
	if (scrollvalue)
	{
	int X, Y, W, H;
		if (webc_ElementGetXYWHPosition(hElem, &X, &Y, &W, &H)==0)
		{
			if (Y+scrollvalue <= 0 && Y+scrollvalue > -600)
				webc_ElementSetXYPosition(hElem, X, Y+scrollvalue);
			return (HTML_EVENT_STATUS_CONTINUE);
		}
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}

#if (0)
for (int j = 0; j < NWebCStyleProperties; j++)
{
IXML_Element* element = 	WebCStyleProperties[j];
	IXML_NamedNodeMap *pnm =  ixmlNode_getAttributes((IXML_Node *) element);
	printf("Num attribs == %d for name == %s\n", ixmlNamedNodeMap_getLength(pnm),ixmlElement_getTagName(WebCStyleProperties[j]));
	for (long i = 0;i < ixmlNamedNodeMap_getLength(pnm);i++)
	{
		IXML_Node *attr = ixmlNamedNodeMap_item(pnm, i);
		if (attr)
		{
			printf("    %d name == %s value == %s\n", i, ixmlNode_getNodeName(attr), ixmlNode_getNodeValue(attr));
			printf("    %d value by name lookup is == %s\n", i, ixmlElement_getAttribute(element,(char *)ixmlNode_getNodeName(attr)));
		}
	}
}
for (int j = 0; j < NFieldPropertyTypes; j++)
{
IXML_Element* element = 	FieldPropertyTypes[j];
	IXML_NamedNodeMap *pnm =  ixmlNode_getAttributes((IXML_Node *) element);
	printf("Num attribs == %d for name == %s\n", ixmlNamedNodeMap_getLength(pnm),ixmlElement_getTagName(FieldPropertyTypes[j]));
	for (long i = 0;i < ixmlNamedNodeMap_getLength(pnm);i++)
	{
		IXML_Node *attr = ixmlNamedNodeMap_item(pnm, i);
		if (attr)
		{
			printf("    %d name == %s value == %s\n", i, ixmlNode_getNodeName(attr), ixmlNode_getNodeValue(attr));
			printf("    %d value by name lookup is == %s\n", i, ixmlElement_getAttribute(element,(char *)ixmlNode_getNodeName(attr)));
		}
	}
}
#endif
static int UpdateProjectElementStyle(char *AttributeName,char *AttributeValue)
{
	IXML_Element* ProjectElementStyleAttributes;
	IXML_Element* thisAttribute;
char xml_buff[1024];

	rtp_sprintf(xml_buff, "HTMLMetadata.StyleAttributes");
	ProjectElementStyleAttributes = rtpxmlGetElemInElemFromDomStr(currentxmlStoryBoardElement,xml_buff);
	if (!ProjectElementStyleAttributes)
		goto error;
	rtp_sprintf(xml_buff, "%s", AttributeName);
	thisAttribute = rtpxmlGetElemInElemFromDomStr(currentxmlStoryBoardElement,xml_buff);
	if (thisAttribute)
	{
		if (rtpxmlSetElemTextInElem (ProjectElementStyleAttributes, AttributeName,AttributeValue) != 0)
			goto error;
	}
	else
	{
		rtp_sprintf(xml_buff, "root.WebCComponentClasses.WebCHtmlComponent.StyleAttributes.%s", AttributeName);
		IXML_Element* TemplateAttribute = rtpxmlGetElemInDocFromDomStr(ComponentTemplate, xml_buff);
		if (!TemplateAttribute)
			goto error;
		IXML_Element *newnode;
		newnode = (IXML_Element *) ixmlNode_cloneNode((IXML_Node *) TemplateAttribute, 1);
		if (ixmlNode_appendChild((IXML_Node *) ProjectElementStyleAttributes, (IXML_Node *) newnode) != IXML_SUCCESS)
			goto error;
		if (rtpxmlSetElemTextInElem (ProjectElementStyleAttributes, AttributeName,AttributeValue) != 0)
			goto error;
	}
	return 0;
error:
	assert(0);
	return -1;
}
