/*
|  jhXMLHttpRequestFactory.h -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2009
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|  Module description:
|   [tbd]
*/
#ifndef __JHXMLHTTPREQUESTFACTORY_H__
#define __JHXMLHTTPREQUESTFACTORY_H__
//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================
#include "webcfg.h"
#if (WEBC_SUPPORT_JSCRIPT)
#if (WEBC_SUPPORT_AJAX)
#ifdef __cplusplus
extern "C" {
#endif
#include "jsapi.h"
#ifdef __cplusplus
}
#endif
//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================
JSObject *jhtml_XMLHttpRequestFactory_Init(JSContext *cx, JSObject *proto);
JSClass *getHtmlXMLHttpRequestFactoryElement();
JSObject *jhtml_XMLHttpRequestFactory_MakeNewObject(JSContext *cx, JSObject *proto, JSObject *parent);
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================

//****************************************************************************
//**
//**    END HEADER JHXMLHTTPREQUESTFACTORY.H
//**
//****************************************************************************
#endif //WEBC_SUPPORT_AJAX
#endif // WEBC_SUPPORT_JSCRIPT
#endif // __JHXMLHTTPREQUESTFACTORY_H__
