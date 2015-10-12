
#ifndef __JHIMG_EXT_H__
#define __JHIMG_EXT_H__
//****************************************************************************
//**
//**    JHIMG_EXT.H
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


// Here's the extended interface for image objects:
// webCBeginDraw();
// webCEndDraw();
// webCRefreshSrc(); re-read the image src
// webCBrushWidth( width );
// webCColor( color );
// webCLine(x0, y0, x1, x2);
// webCFillPolygon(
// webCFillRect(x, y, w, h)
// webCEllipse(
// webCFillEllipse(
// webCArc(x, y, radius1, radius2, angle1, angle2)

// TODO need to add webCFillArc(iL, iT, iW, iH, fAngA, fAngZ)


JSBool jhtml_img_ext_webCBeginDraw(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_img_ext_webCEndDraw(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_img_ext_webCRefreshSrc(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_img_ext_webCBrushWidth(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_img_ext_webCColor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_img_ext_webCLine(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_img_ext_webCFillPolygon(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_img_ext_webCFillRect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_img_ext_webCEllipse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_img_ext_webCFillEllipse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_img_ext_webCArc(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);



#endif // WEBC_SUPPORT_JSCRIPT

#endif //__JHIMG_EXT_H__
