#include "../include/AppBuilder.h"


static IXML_Element *rtpxmlGetElemInElem (IXML_Element *parent, char *tagName, int num);

IXML_Element* rtpxmlGetElemInElemFromDomStr(IXML_Element* Elem, char *inDomStr)
{
char Current[80];
char *p,*pnext,*DomStr;
char _DomStr[2048];

	rtp_strcpy(_DomStr,inDomStr);
	DomStr = _DomStr;
	p = 0;
	pnext = rtp_strstr(DomStr,".");
	if (pnext)
	{
		*pnext=0;
		pnext++;
	}
	p = DomStr;
	while (Elem && p)
	{
		rtp_strcpy(Current, p);
		Elem = rtpxmlGetElemInElem (Elem, Current, 0);
		p = pnext;
		if (pnext)
		{
			pnext = rtp_strstr(pnext,".");
			if (pnext)
				*pnext++=0;
		}
	}
	return(Elem);
}

IXML_Element* rtpxmlGetElemInDocFromDomStr(IXML_Document *Doc, char *inDomStr)
{
IXML_Element *Elem;
char Current[80];
char *p,*DomStr;
char _DomStr[2048];

	rtp_strcpy(_DomStr,inDomStr);
	DomStr = _DomStr;
	p = 0;
	p = rtp_strstr(DomStr,".");
	if (p)
	{
		*p=0;
		p++;
	}
	rtp_strcpy(Current, DomStr);

	Elem = ixmlDocument_getElementById(Doc,  Current);
	while (Elem && p)
	{
		char *pnext;
		pnext = rtp_strstr(p,".");
		if (pnext)
			*pnext++=0;
		rtp_strcpy(Current, p);
		p = pnext;
		Elem = rtpxmlGetElemInElem (Elem, Current, 0);
	}
	return(Elem);
}

int rtpxmlSaveDocument(IXML_Document *p, char *xmlFileName)
{
    int fileSize = 0;
    int bytesWriten = 0;
    RTP_FILE xmlFilePtr;
	DOMString d;
	char *encscheme = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r";


    if (rtp_file_open(&xmlFilePtr, xmlFileName, RTP_FILE_O_CREAT|RTP_FILE_O_BINARY|RTP_FILE_O_WRONLY|RTP_FILE_O_TRUNC, RTP_FILE_S_IWRITE|RTP_FILE_S_IREAD) < 0)
       	goto error;
    rtp_file_write( xmlFilePtr, (unsigned char*)encscheme, rtp_strlen(encscheme));

	d = ixmlPrintDocument(p);
	fileSize = rtp_strlen(d);
    bytesWriten = rtp_file_write( xmlFilePtr, (unsigned char*)d, fileSize );
    if (bytesWriten != fileSize)
       	goto error;
    rtp_file_close( xmlFilePtr );
	ixmlFreeDOMString(d);
  	return 0;
error:
	printf("Error writing file %s\n", xmlFileName);
	assert(0);
  	return -1;
}

int rtpxmlSetElemTextInElem (IXML_Element *parent, char *tagName,char *Value)
{
IXML_Node *elementNode;
IXML_Node *textNode;
IXML_NodeList *nodeList;

    nodeList = ixmlElement_getElementsByTagName(parent, tagName);
    if (!nodeList)
    	goto error;

    elementNode = ixmlNodeList_item(nodeList, 0);
    ixmlNodeList_free(nodeList);

    if (!elementNode)
    	goto error;

    textNode = ixmlNode_getFirstChild(elementNode);
    if (!textNode)
    	goto error;

    if (IXML_SUCCESS == ixmlNode_setNodeValue(textNode, Value))
    	return 0;
error:
	printf("Error setting %s to %s \n", tagName, Value);
	assert(0);
  	return -1;
}

char *rtpxmlGetElemTextInElem (IXML_Element *parent, char *tagName)
{
IXML_Node *elementNode;
IXML_Node *textNode;
IXML_NodeList *nodeList;
int num = 0;

    nodeList = ixmlElement_getElementsByTagName(parent, tagName);
    if (!nodeList)
    {
    	return (0);
	}

    elementNode = ixmlNodeList_item(nodeList, num);
    ixmlNodeList_free(nodeList);

    if (!elementNode)
    {
    	return (0);
    }

    textNode = ixmlNode_getFirstChild(elementNode);
    if (!textNode)
    {
    	return (0);
    }

    char *p = ixmlNode_getNodeValue(textNode);
	if (p && rtp_strcmp(p,"NILL") == 0)
		p = 0;
    return (p);
}

static IXML_Element *rtpxmlGetElemInElem (IXML_Element *parent, char *tagName, int num)
{
IXML_Node *elementNode;
IXML_NodeList *nodeList;

    nodeList = ixmlElement_getElementsByTagName(parent, tagName);
    if (!nodeList)
    {
    	return (0);
	}

    elementNode = ixmlNodeList_item(nodeList, num);
    ixmlNodeList_free(nodeList);

    return (IXML_Element *) elementNode;
}

IXML_Document *ComponentTemplate;
IXML_Document *DialogueTemplate;
IXML_Document *PalletTemplate;
IXML_Document *TestProject;

int WebcAppGenXmlLLoadTemplates(void)
{
char *p;
	p = COMPONENTTEMPLATEFILE;
	ComponentTemplate = ixmlLoadDocument(p);
	if (!ComponentTemplate)
		goto error;
	p = DIALOGUETEMPLATEFILE;
	DialogueTemplate = ixmlLoadDocument(p);
	if (!DialogueTemplate)
		goto error;
	p = PALLETTEMPLATEFILE;
	PalletTemplate = ixmlLoadDocument(p);
	if (!PalletTemplate)
		goto error;
	p = TESTPROJECT;
	TestProject = ixmlLoadDocument(p);
	if (!TestProject)
		goto error;
	return 0;
error:
	printf("WebcAppGenXmlLLoadTemplates failed loading %s\n", p);
	assert(0);
	return -1;
}

/* Load all children into an array, easier to use than than lists */
int WebcAppGenXmlLoadChildArray(IXML_Element *ParentElement, IXML_Element **ChildArray, int ArraySize)
{
int returnCount = -1;

	if (ParentElement)
	{
		returnCount = 0;
		IXML_NodeList* children = ixmlNode_getChildNodes((IXML_Node*) ParentElement);
		if (children)
		{
			for (int index = 0; index < (int)ixmlNodeList_length(children); index++)
			{
				IXML_Node* node = ixmlNodeList_item(children, index);

				if (node && ixmlNode_getNodeType(node) == eELEMENT_NODE)
				{
					*ChildArray++ = (IXML_Element *) node;
					returnCount++;
				}
			}
			ixmlNodeList_free(children);
		}
	}
	return returnCount;
}
