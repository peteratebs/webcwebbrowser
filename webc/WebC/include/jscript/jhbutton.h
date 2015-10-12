#ifndef __JHBUTTON_H__
#define __JHBUTTON_H__
//****************************************************************************
//**
//**    JHBUTTON.H
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
JSObject *jhtml_button_Init(JSContext *cx, JSObject* proto);
JSClass *getHtmlButtonElement();
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================

//****************************************************************************
//**
//**    END HEADER JHTBUTTON.H
//**
//****************************************************************************
#endif // WEBC_SUPPORT_JSCRIPT
#endif // __JHBUTTON_H__
