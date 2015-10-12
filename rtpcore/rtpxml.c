/*
|  RTPXML.C -
|
|  EBS -
|
|   $Author: vmalaiya $
|   $Date: 2006/07/17 15:29:00 $
|   $Name:  $
|   $Revision: 1.3 $
|
|  Copyright EBS Inc. , 2006
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/
#include "rtpxml.h"

/* Uncomment the following line to build with functions supplied by the ixml library stubbed */

//#define STUB_INTEL_XML
#ifndef STUB_INTEL_XML

#include "ixml.h"

#else
/* Rtupnp can build in the absense if the ixml library with these functions stubbed */
#define	ixmlDocument_free(A)
#define	ixmlElement_free(A)
#define	ixmlFreeDOMString(A)
#define	ixmlNodeList_free(A)

#define ixmlElement_getElementsByTagNameNS(A,B,C) 0
#define ixmlDocument_getElementsByTagNameNS(A,B,C) 0

#define ixmlDocument_createElement(A,B) 0
#define ixmlDocument_createTextNode(A,B) 0
#define ixmlDocument_getElementsByTagName(A,B) 0
#define ixmlElement_getAttribute(A,B) 0
#define ixmlNode_appendChild(A,B) 0
#define ixmlParseBufferEx(A,B) 0
#define ixmlNode_setNodeValue(A,B) 0
#define ixmlNodeList_item(A,B) 0
#define ixmlElement_getElementsByTagName(A,B) 0

#define ixmlElement_getTagName(A) 0
#define ixmlLoadDocument(A) 0
#define ixmlNode_getChildNodes(A) 0
#define ixmlNode_getFirstChild(A) 0
#define ixmlNode_getNextSibling(A) 0
#define ixmlNode_getNodeType(A) 0
#define ixmlNode_getNodeValue(A) 0
#define ixmlNode_getParentNode(A) 0
#define ixmlNodeList_length(A) 0
#define ixmlNodetoString(A) 0
#define ixmlParseBuffer(A) 0
#define ixmlPrintDocument(A) 0

#define eELEMENT_NODE 0
#endif
/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

RTPXML_Element *rtpxmlDocument_createElement(RTPXML_Document *doc, DOMString tagName)
{
	return ixmlDocument_createElement((IXML_Document *)doc, tagName);
}
RTPXML_Node *rtpxmlDocument_createTextNode(RTPXML_Document *doc, DOMString data)
{
	return (RTPXML_Node *)ixmlDocument_createTextNode((IXML_Document *)doc, data);
}
void rtpxmlDocument_free(RTPXML_Document* doc)
{
	ixmlDocument_free((IXML_Document *)doc);
}
RTPXML_NodeList *rtpxmlDocument_getElementsByTagName(RTPXML_Document *doc, DOMString tagName)
{
	return (RTPXML_NodeList *)ixmlDocument_getElementsByTagName((IXML_Document *)doc, tagName);
}
RTPXML_NodeList *rtpxmlDocument_getElementsByTagNameNS(RTPXML_Document* doc,DOMString namespaceURI,DOMString localName)
{
	return (RTPXML_NodeList *)ixmlDocument_getElementsByTagNameNS((IXML_Document *)doc, namespaceURI,localName);
}
void rtpxmlElement_free(RTPXML_Element* element)
{
	ixmlElement_free((IXML_Element *) element);
}
DOMString rtpxmlElement_getAttribute(RTPXML_Element* element, DOMString name)
{
	return ixmlElement_getAttribute((IXML_Element *)element, name);
}
RTPXML_NodeList *rtpxmlElement_getElementsByTagName(RTPXML_Element* element,DOMString tagName)
{
	return (RTPXML_NodeList *)ixmlElement_getElementsByTagName((IXML_Element *)element,tagName);
}
RTPXML_NodeList *rtpxmlElement_getElementsByTagNameNS(RTPXML_Element* element,DOMString namespaceURI,DOMString localName)
{
	return (RTPXML_NodeList *)ixmlElement_getElementsByTagNameNS((IXML_Element *)element, namespaceURI, localName);
}
const DOMString rtpxmlElement_getTagName(RTPXML_Element* element)
{
	return ixmlElement_getTagName((IXML_Element *)element);
}
void rtpxmlFreeDOMString(DOMString buf)
{
	ixmlFreeDOMString(buf);
}
RTPXML_Document *rtpxmlLoadDocument(char* xmlFile)
{
	return ixmlLoadDocument(xmlFile);
}
RTPXML_NodeList *rtpxmlNode_getChildNodes(RTPXML_Node *nodeptr)
{
	return (RTPXML_NodeList *)ixmlNode_getChildNodes((IXML_Node *)nodeptr);
}
RTPXML_Node *rtpxmlNode_getFirstChild(RTPXML_Node *nodeptr)
{
	return (RTPXML_Node *)ixmlNode_getFirstChild((IXML_Node *)nodeptr);
}
RTPXML_Node* rtpxmlNode_getNextSibling(RTPXML_Node *nodeptr)
{
	return (RTPXML_Node *)ixmlNode_getNextSibling((IXML_Node *)nodeptr);
}
const unsigned short rtpxmlNode_getNodeType(RTPXML_Node *nodeptr)
{
	/* We only test for type == eELEMENT_NODE in the source code so to retain separation of
	   datastructures between the application and the xml implementation test for that value only */
	if (ixmlNode_getNodeType((IXML_Node *)nodeptr) == eELEMENT_NODE)
		return (rtpxeELEMENT_NODE);
	else
		return(0);
}
DOMString rtpxmlNode_getNodeValue(RTPXML_Node *nodeptr)
{
	return ixmlNode_getNodeValue((IXML_Node *)nodeptr);
}
RTPXML_Node* rtpxmlNode_getParentNode(RTPXML_Node *nodeptr)
{
	return ixmlNode_getParentNode((IXML_Node *)nodeptr);
}
int rtpxmlNode_setNodeValue(RTPXML_Node *nodeptr, char *newNodeValue)
{
	return ixmlNode_setNodeValue((IXML_Node *)nodeptr, newNodeValue);
}
RTPXML_Node *rtpxmlNodeList_item(RTPXML_NodeList *nList, unsigned long index)
{
	return (RTPXML_Node *)ixmlNodeList_item(nList, index);
}
unsigned long rtpxmlNodeList_length(RTPXML_NodeList *nList)
{
	return ixmlNodeList_length(nList);
}
void rtpxmlNodeList_free(RTPXML_NodeList *nList)
{
	ixmlNodeList_free(nList);
}
DOMString rtpxmlNodetoString(RTPXML_Node *doc)
{
	return ixmlNodetoString((IXML_Node *)doc);
}
RTPXML_Document *rtpxmlParseBuffer(char *buffer)
{
	return ixmlParseBuffer(buffer);
}
int rtpxmlParseBufferEx(char *buffer, RTPXML_Document** doc)
{
	return ixmlParseBufferEx(buffer, (IXML_Document **) doc);
}
DOMString rtpxmlPrintDocument(RTPXML_Document *doc)
{
	return (ixmlPrintDocument((IXML_Document *)doc));
}
int rtpxmlNode_appendChild(RTPXML_Node *nodeptr,RTPXML_Node* newChild)
{
	return ixmlNode_appendChild((IXML_Node *)nodeptr,newChild);
}

