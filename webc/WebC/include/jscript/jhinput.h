#ifndef __JHINPUT_H__
#define __JHINPUT_H__
//****************************************************************************
//**
//**    JHINPUT.H
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
#define JHTML_ALIGN_NONE "none"
#define JHTML_ALIGN_LEFT "left"
#define JHTML_ALIGN_RIGHT "right"
#define JHTML_ALIGN_CENTER "center"
#define JHTML_ALIGN_BOTTOM "bottom"
#define JHTML_ALIGN_TOP "top"
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
struct jhtml_element_struct;
//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================
JSBool jhtml_input_blur(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_input_focus(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_input_select(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_input_click(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_input_setAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);


JSObject *jhtml_input_Init(JSContext *cx, JSObject *proto);
JSClass  *getHtmlInputElement();
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================

//****************************************************************************
//**
//**    END HEADER JHINPUT.H
//**
//****************************************************************************
#endif // WEBC_SUPPORT_JSCRIPT
#endif // __JHINPUT_H__
