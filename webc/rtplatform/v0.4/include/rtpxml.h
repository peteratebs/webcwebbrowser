/*
|  RTPROT.H -
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

#ifndef __RTPXML_H__
#define __RTPXML_H__

// Don't include intel headers but include the rtplatform header files that it includes, so the 
// source files that rely on that side effect don't have to be modified.
// #include "ixml.h"
#include "rtpmem.h"
#include "rtpstdup.h"
#include "rtp.h"
#include "rtpfile.h"
#include "rtpstr.h"
#include "rtpdebug.h"

#ifndef DOMString
#define DOMString   char *
#endif

// All dom types are void. (void *) handles. All dom modification and inspection is donw through handles.
// #define RTPXML_Document IXML_Document
#define RTPXML_Document void
// #define RTPXML_Element  IXML_Element
#define RTPXML_Element  void
// #define RTPXML_Node     IXML_Node
#define RTPXML_Node     void
//#define RTPXML_NodeList IXML_NodeList
#define RTPXML_NodeList void
// #define RTPXML_SUCCESS  IXML_SUCCESS
#define RTPXML_SUCCESS  0 // must be the same as IXML_SUCCESS

#define rtpxeELEMENT_NODE 2 // translated from eELEMENT_NODE in rtpxmlNode_getNodeType

#ifdef __cplusplus
extern "C" {
#endif

RTPXML_Element *rtpxmlDocument_createElement(RTPXML_Document *doc, DOMString tagName);
RTPXML_Node *rtpxmlDocument_createTextNode(RTPXML_Document *doc, DOMString data);
void rtpxmlDocument_free(RTPXML_Document* doc);
RTPXML_NodeList *rtpxmlDocument_getElementsByTagName(RTPXML_Document *doc, DOMString tagName);
RTPXML_NodeList *rtpxmlDocument_getElementsByTagNameNS(RTPXML_Document* doc,DOMString namespaceURI,DOMString localName);
void rtpxmlElement_free(RTPXML_Element* element);
DOMString rtpxmlElement_getAttribute(RTPXML_Element* element, DOMString name);
RTPXML_NodeList *rtpxmlElement_getElementsByTagName(RTPXML_Element* element,DOMString tagName);
RTPXML_NodeList *rtpxmlElement_getElementsByTagNameNS(RTPXML_Element* element,DOMString namespaceURI,DOMString localName);
const DOMString rtpxmlElement_getTagName(RTPXML_Element* element);
void rtpxmlFreeDOMString(DOMString buf);
RTPXML_Document *rtpxmlLoadDocument(char* xmlFile);
RTPXML_NodeList *rtpxmlNode_getChildNodes(RTPXML_Node *nodeptr);
RTPXML_Node *rtpxmlNode_getFirstChild(RTPXML_Node *nodeptr);
RTPXML_Node* rtpxmlNode_getNextSibling(RTPXML_Node *nodeptr);
const unsigned short rtpxmlNode_getNodeType(RTPXML_Node *nodeptr);
DOMString rtpxmlNode_getNodeValue(RTPXML_Node *nodeptr);
RTPXML_Node* rtpxmlNode_getParentNode(RTPXML_Node *nodeptr);
int rtpxmlNode_setNodeValue(RTPXML_Node *nodeptr, char *newNodeValue);
RTPXML_Node *rtpxmlNodeList_item(RTPXML_NodeList *nList, unsigned long index);
unsigned long rtpxmlNodeList_length(RTPXML_NodeList *nList);
void rtpxmlNodeList_free(RTPXML_NodeList *nList);
DOMString rtpxmlNodetoString(RTPXML_Node *doc);
RTPXML_Document *rtpxmlParseBuffer(char *buffer);
int rtpxmlParseBufferEx(char *buffer, RTPXML_Document** doc);
DOMString rtpxmlPrintDocument(RTPXML_Document *);
int rtpxmlNode_appendChild(RTPXML_Node *nodeptr,RTPXML_Node* newChild);

#ifdef __cplusplus
}
#endif


#endif /*  __RTPXML_H__ */
