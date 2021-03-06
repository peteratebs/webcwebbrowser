#ifndef __JHTITLE_H__
#define __JHTITLE_H__
//****************************************************************************
//**
//**    JHTITLE.H
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
JSObject *jhtml_title_Init(JSContext *cx, JSObject* proto);
JSClass *getHtmlTitleElement();
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================

//****************************************************************************
//**
//**    END HEADER JHTITLE.H
//**
//****************************************************************************
#endif // WEBC_SUPPORT_JSCRIPT
#endif // __JHTITLE_H__
