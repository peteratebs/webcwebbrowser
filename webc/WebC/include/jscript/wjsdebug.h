/*
|  WJSDDEBUG.H -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#ifndef __WJSDEBUG_H__
#define __WJSDEBUG_H__

#include "webcfg.h"

#if (WEBC_SUPPORT_JSCRIPT)
#ifdef __cplusplus
extern "C" {
#endif
#include "jsapi.h"
#ifdef __cplusplus
}
#endif

void wjsdebug_init(void);

#if (!WEBC_DEBUG_JSCRIPT)
#define DEBUGJS_InitClass(A,B,C,D,E,F,G,H,I,J) JS_InitClass((A),(B),(C),(D),(E),(F),(G),(H),(I),(J))

#else

#include <stdio.h>

#define MAX_DEBUGJCLASSES 30
#define MAX_DEBUGJMETHODS 130


#ifdef __cplusplus


class JWEBCDebug
{
 private:
	JSRuntime* mRuntime;
	JSContext* mDefaultContext;
 public:
    int mNlines;
	int mLastlineDisplayed;
#define MAXLINES 1000
	void *strLines[MAXLINES];   /* Caste to webcstr */

 	JWEBCDebug (JSRuntime* rt, JSContext*cx);
 	~JWEBCDebug (void);
};


extern "C" {
#endif
#ifdef __cplusplus
}
#endif

extern JS_PUBLIC_API(JSObject *)
DEBUGJS_InitClass(JSContext *cx, JSObject *obj, JSObject *parent_proto,
             JSClass *clasp, JSNative constructor, uintN nargs,
             JSPropertySpec *ps, JSFunctionSpec *fs,
             JSPropertySpec *static_ps, JSFunctionSpec *static_fs);


#endif /* WEBC_DEBUG_JSCRIPT */

#endif /* WEBC_SUPPORT_JSCRIPT */

#endif /* ____WJSDEBUG_H__ */
