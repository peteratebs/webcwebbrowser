#ifndef __JNAV_H__
#define __JNAV_H__
//****************************************************************************
//**
//**    JNAV.H
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
JSObject *jnavigator_Init(JSContext *cx, JSObject* proto);
JSClass *getJNavigator();
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================

//****************************************************************************
//**
//**    END HEADER JNav.H
//**
//****************************************************************************
#endif // WEBC_SUPPORT_JSCRIPT
#endif // __JNAV_H__