#include "../include/AppBuilder.h"

IXML_Element *newxmlAppElement(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, char *ObjectId)
{
IXML_Element *xmltemplate,*newnode;
char dom_str[1024];
	newnode = 0;
	rtp_sprintf(dom_str,"WebCComponentObjects.%s",ObjectId);
	xmltemplate = rtpxmlGetElemInDocFromDomStr(ComponentTemplate, dom_str);
	if (xmltemplate)
	{
		newnode = (IXML_Element *) ixmlNode_cloneNode((IXML_Node *) xmltemplate, 1);
		return newnode;
	}
	return 0;
}
static char * xmlAppElementToStyleDecl(IXML_Element *xmlElement, char *tostyle);

static char * xmlAppElementToStyleDecl(IXML_Element *xmlElement, char *tostyle);

int xmlAppElementToHtml(IXML_Element *xmlElement, char *tohtml)
{
char *p, *X,*Y,*TAG,*SRC,*Z,*H,*W,*V,*VALUE,*CONTENT,*ENDTAG;
IXML_Element *HTMLMetadata = rtpxmlGetElemInElemFromDomStr(xmlElement, "HTMLMetadata");

	if (!HTMLMetadata)
		goto error;

	TAG = rtpxmlGetElemTextInElem (HTMLMetadata, "tag");
	X   = rtpxmlGetElemTextInElem (HTMLMetadata, "x");
	Y   = rtpxmlGetElemTextInElem (HTMLMetadata, "y");
	Z   = rtpxmlGetElemTextInElem (HTMLMetadata, "z");
	W   = rtpxmlGetElemTextInElem (HTMLMetadata, "w");
	H   = rtpxmlGetElemTextInElem (HTMLMetadata, "h");
	H   = rtpxmlGetElemTextInElem (HTMLMetadata, "h");
	VALUE 	= rtpxmlGetElemTextInElem (HTMLMetadata, "value");
	ENDTAG 	= rtpxmlGetElemTextInElem (HTMLMetadata, "endtag");
	CONTENT = rtpxmlGetElemTextInElem (HTMLMetadata, "content");
	SRC 	= rtpxmlGetElemTextInElem (HTMLMetadata, "src");

	V = "visible";
	p = tohtml;
	if (!X ||!Y||!TAG||!Z||!H||!W)
		goto error;
	rtp_sprintf(p,"<%s ", TAG);
	p += rtp_strlen(p);
	if (VALUE)
	{		rtp_sprintf(p," value=\"%s\" ", VALUE);	p += rtp_strlen(p);	}
	if (SRC)
	{		rtp_sprintf(p," src=\"%s\" ", SRC);	p += rtp_strlen(p);	}

//	rtp_sprintf(p,"style=\"position:absolute;top:%spx;left:%spx;height:%spx;width:%spx;z-index:%s;visibilty:%s;\"", Y,X,H,W,Z,V);
	xmlAppElementToStyleDecl(xmlElement, p);

	p += rtp_strlen(p);

	if (ENDTAG)
		rtp_strcat(p,">");
	else
		rtp_strcat(p,"/>");
	p += rtp_strlen(p);
	if (CONTENT)
	{
		rtp_strcat(p,CONTENT);
		p += rtp_strlen(p);
	}
	if (ENDTAG)
		rtp_sprintf(p,"<%s>",ENDTAG);

	return 0;
error:
	return -1;
}

static char * xmlAppElementToStyleDecl(IXML_Element *xmlElement, char *tostyle)
{
char *p, *X,*Y,*Z,*H,*W,*V;
IXML_Element * StyleAttributes[1024];
int nAttributes;
IXML_Element *HTMLMetadata = rtpxmlGetElemInElemFromDomStr(xmlElement, "HTMLMetadata");
	if (!HTMLMetadata)
		goto error;
	/* These are legacy emove */
	X   = rtpxmlGetElemTextInElem (HTMLMetadata, "x");
	Y   = rtpxmlGetElemTextInElem (HTMLMetadata, "y");
	Z   = rtpxmlGetElemTextInElem (HTMLMetadata, "z");
	W   = rtpxmlGetElemTextInElem (HTMLMetadata, "w");
	H   = rtpxmlGetElemTextInElem (HTMLMetadata, "h");
	V = "visible";

	p = tostyle;
	rtp_sprintf(p,"style=\"position:absolute;top:%spx;left:%spx;height:%spx;width:%spx;z-index:%s;visibilty:%s;", Y,X,H,W,Z,V);
	p += rtp_strlen(p);

	IXML_Element* ElementStyleAttributes;
	ElementStyleAttributes = rtpxmlGetElemInElemFromDomStr(HTMLMetadata,"StyleAttributes");
	if (!ElementStyleAttributes)
		goto error;
	nAttributes = WebcAppGenXmlLoadChildArray(ElementStyleAttributes, StyleAttributes,1024);
	if (nAttributes < 0)
		goto error;
	for (int i=0;i<nAttributes;i++)
	{
		char *name;
		char *value;

		name = (char *) ixmlElement_getTagName(StyleAttributes[i]);
		value = (char *) rtpxmlGetElemTextInElem(ElementStyleAttributes,name);
		rtp_sprintf(p,"%s:%s;",name,value);
		p += rtp_strlen(p);
	}
	rtp_sprintf(p,"\"");
	printf("%s\n", tostyle);
	return p;
error:
	assert(0);
	return 0;
}

int xmlPalletElementToHtml(IXML_Element *xmlElement, char *tohtml, int isShadow)
{
char *X,*Y,*Z,*H,*W,*ID,*SRC;
IXML_Element *HTMLMetadata = xmlElement; // rtpxmlGetElemInElemFromDomStr(xmlElement, "HTMLMetadata");

	X   = rtpxmlGetElemTextInElem (HTMLMetadata, "left");
	Y   = rtpxmlGetElemTextInElem (HTMLMetadata, "top");
	Z   = rtpxmlGetElemTextInElem (HTMLMetadata, "z");
	H   = rtpxmlGetElemTextInElem (HTMLMetadata, "h");
	W   = rtpxmlGetElemTextInElem (HTMLMetadata, "w");
	ID  = rtpxmlGetElemTextInElem (HTMLMetadata, "id");
	SRC = rtpxmlGetElemTextInElem (HTMLMetadata, "src");

	if (!X ||!Y||!Z||!H||!W||!SRC||!ID)
		goto error;
	if (isShadow)
		rtp_sprintf(tohtml,"<img src=\"%s\" id=\"Shadow%s\" style=\"position:absolute;top:%spx;left:%spx;height:%spx;width:%spx;z-index:%s;visibility:hidden;\">",SRC, ID,Y,X,H,W,Z);
	else
		rtp_sprintf(tohtml,"<img src=\"%s\" id=\"%s\" style=\"position:absolute;top:%spx;left:%spx;height:%spx;width:%spx;z-index:%s;visibility:visible;\">",SRC, ID,Y,X,H,W,Z);
	return 0;
error:
	assert(0);
	return -1;
}
