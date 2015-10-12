#ifndef __JHELEM_H__
#define __JHELEM_H__
//****************************************************************************
//**
//**    JHELEM.H
//**    Header - Description
//**
//**
//****************************************************************************
//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================
#if (WEBC_SUPPORT_JSCRIPT)
#include "htmldom.hpp"
#include "wjsdoc.h"
//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
class HTMLElement;
//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================
JSObject *jhtml_element_Init(JSContext *cx, JSObject* proto);

JSBool jhtml_element_attachEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_element_detachEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_element_fireEvent  (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_element_resolve(JSContext *cx, JSObject *obj, jsval id);
JSBool jhtml_element_setAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

JSClass *getHtmlElement();
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================

//****************************************************************************
//**
//**    END HEADER JHELEM.H
//**
//****************************************************************************
#endif // WEBC_SUPPORT_JSCRIPT
#endif // __JHELEM_H__
