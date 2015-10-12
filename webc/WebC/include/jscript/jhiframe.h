#ifndef __JHIFRAME_H__
#define __JHIFRAME_H__
//****************************************************************************
//**
//**    JHIFRAME.H
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
//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================
#if (WEBC_SUPPORT_FRAMES && WEBC_SUPPORT_IFRAMES)
JSObject *jhtml_iframe_Init(JSContext *cx, JSObject* proto);
JSClass *getHtmlIFrameElement();
#endif // (WEBC_SUPPORT_FRAMES && WEBC_SUPPORT_IFRAMES)
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================

//****************************************************************************
//**
//**    END HEADER JHIFRAME.H
//**
//****************************************************************************
#endif // WEBC_SUPPORT_JSCRIPT
#endif // __JHIFRAME_H__
