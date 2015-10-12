#ifndef __JHTABLE_H__
#define __JHTABLE_H__
//****************************************************************************
//**
//**    JHTABLE.H
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
JSObject *jhtml_table_Init(JSContext *cx, JSObject* proto);
JSClass *getHtmlTableElement();
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================

//****************************************************************************
//**
//**    END HEADER JHTABLE.H
//**
//****************************************************************************
#endif // WEBC_SUPPORT_JSCRIPT
#endif // __JHTABLE_H__
