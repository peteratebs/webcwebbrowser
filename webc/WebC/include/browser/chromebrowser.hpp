#ifndef __WEBCDEMO_HPP_INCLUDED__
#define __WEBCDEMO_HPP_INCLUDED__

#include "webc.h"

#ifdef __cplusplus
extern "C" {
#endif


HTMLEventStatus chromeLoadHandler (
        HBROWSER_HANDLE hbrowser,
        HDOC_HANDLE hdoc,
        HELEMENT_HANDLE helem,
        HTMLEvent* event,
        char* param
    );

HTMLEventStatus chromeBrowserEventHandler (
        HBROWSER_HANDLE hbrowser,
		HTMLBrowserEvent* event
	);

HTMLEventStatus bodyHandler (
        HBROWSER_HANDLE hbrowser,
        HDOC_HANDLE hdoc,
        HELEMENT_HANDLE helem,
        HTMLEvent *pEvent,
        char *pParam
    );

HTMLEventStatus inputHandler (
        HBROWSER_HANDLE hbrowser,
        HDOC_HANDLE hdoc,
        HELEMENT_HANDLE helem,
        HTMLEvent *pEvent,
        char *pParam
    );

int GoogleUrlMacro(WEBC_CHAR *param, WEBC_CHAR *replace, int maxLen);

#if (WEBC_SUPPORT_JSCRIPT)
JSBool AppendFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool GetDefaultUrl(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
#endif

#ifdef __cplusplus
}
#endif

#endif //__WEBCDEMO_HPP_INCLUDED__
