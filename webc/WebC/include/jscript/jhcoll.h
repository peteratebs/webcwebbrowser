#ifndef __JHCOLL_H__
#define __JHCOLL_H__
//****************************************************************************
//**
//**    JHCOLL.H
//**    Header - Description
//**
//**
//****************************************************************************
//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================
#if (WEBC_SUPPORT_JSCRIPT)
#include "wjsdoc.h"
//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
enum CollectionType
{
	COLLECTION_TYPE_NONE,
	DOCUMENT_IMAGES,
	DOCUMENT_FORMS,
	DOCUMENT_ANCHORS,
	DOCUMENT_LINKS,
	DOCUMENT_ALL,
	DOCUMENT_STYLESHEETS,
	FORM_INPUTS,
	ALL_BY_NAME,
	TABLE_ROWS,
	TABLEROW_CELLS,
	SELECT_OPTIONS,
	WIN_FRAMES,
	ELEMENT_NODES,
	SSHEET_RULES,
	SSHEET_IMPORTS,
	RADIO_BUTTONS,
	DOCUMENT_IDS,
	MAP_AREAS,
	ALL_TAGS_BY_TAGNAME
};

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
//forward declarations
class HTMLElement;
class WebcJSDocumentContext;
struct jhtml_collection
{
	//a pointer to the our JSObject
	JSObjectRef jsObj;
	//the first HTMLElement to check on the tree;
	HTMLElement *pTop;
	//the type of finder to use
	int finderType;
	//if this is a ALL_BY_NAME or ALL_TAGS_BY_TAGNAME type then
	//this is the name of all the elements in
	//the collection.
	WEBC_CHAR *nameOfAll;
};
//==========================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================
JSObject *jhtml_collection_Init(JSContext *cx, JSObject *proto);
JSClass *getHtmlCollection();
jhtml_collection *jhtml_collection_New(WebcJSDocumentContext *jsmgr, JSObject *parent,
					const char *name, HTMLElement *top, CollectionType type);
//****************************************************************************
//**
//**    END HEADER JHCOLL.H
//**
//****************************************************************************
#endif // WEBC_SUPPORT_JSCRIPT
#endif // __JHCOLL_H__
