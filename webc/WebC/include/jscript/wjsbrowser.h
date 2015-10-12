/*
|  WJSBROWSER.H -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#ifndef __WJSBROWSER_H__
#define __WJSBROWSER_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "jsapi.h"
#ifdef __cplusplus
}
#endif
#include "webctypes.h"
#include "refcount.hpp"
#include "webcfg.h"
#if (WEBC_DEBUG_JSCRIPT)
#include "wjsdebug.h"
#endif
#define COLLECT_AT 2L*1024L*1024L

#define JSCRIPT_EXECUTING 1
#define JSCRIPT_NOT_EXECUTING 0

#define JSCRIPT_STOPPED 1
#define JSCRIPT_NOT_STOPPED 0

extern "C" JSClass null_class;

#ifdef __cplusplus

class WebcJSBrowserContext
{
 private:
	JSRuntime* mRuntime;
	JSContext* mDefaultContext;
	JSContext* mContextPool[WEBC_CFG_JS_CX_POOL_SIZE];
#if (WEBC_DEBUG_JSCRIPT)
	JWEBCDebug *mJSDebug;
#endif
	int        mNumFreeContexts;
	int        mGCCounter;

 public:
 	WebcJSBrowserContext (void);
 	~WebcJSBrowserContext (void);

	JSContext* AllocContext   (void);
	JSContext* DefaultContext (void) { return mDefaultContext; }
#if(WEBC_DEBUG_PARSER)
    void WebcJSBrowserContext::SetJscriptTrace(FILE *fp);
#endif
	void       FreeContext    (JSContext* cx);
	void       FreeTempStore  (void);
};


extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __WJSBROWSER_H__ */
