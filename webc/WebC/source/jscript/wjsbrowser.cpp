/*
|  WJSBROWSER.CPP -
|
|  EBS - WebC
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#include "webcjhconfig.h"
#if (WEBC_SUPPORT_JSCRIPT)

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "wjsbrowser.h"
#include "rtpstr.h"
#include "webcmem.h"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

#define WEBC_JS_CONTEXT_STACK_SIZE        4096
#define WEBC_JS_GARBAGE_COLLECT_INTERVAL  100

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
/* Called from webc_Init(). */
WebcJSBrowserContext::WebcJSBrowserContext (void)
{
	rtp_memset(mContextPool, 0, sizeof(mContextPool));
	mNumFreeContexts = 0;
	mDefaultContext = 0;
  #ifdef WEBC_JS_GARBAGE_COLLECT_INTERVAL
	mGCCounter = WEBC_JS_GARBAGE_COLLECT_INTERVAL;
  #endif

	mRuntime = JS_NewRuntime(COLLECT_AT);
	if (mRuntime)
	{
		mDefaultContext = JS_NewContext(mRuntime, WEBC_JS_CONTEXT_STACK_SIZE);
		if (!mDefaultContext)
		{
			JS_DestroyRuntime(mRuntime);
			mRuntime = WEBC_NULL;
		}
	}
#if (WEBC_DEBUG_JSCRIPT)
    WEBC_NEW_VERBOSE(mJSDebug, JWEBCDebug(mRuntime,mDefaultContext),"JWEBCDebug");
#endif
}


#if(WEBC_DEBUG_PARSER)
#include "jscntxt.h"
#include "jsapi.h"
// <EDIT> 20080120  Method to enable javascript logging if WEBC_DEBUG_PARSER is enabled.
extern "C" WebcJSBrowserContext* gJSBrowserContext;
void WebcJSBrowserContext::SetJscriptTrace(FILE *fp)
{
#if (WEBC_DEBUG_LOG_JSCRIPT)
JSContext* pDefaultContext;
    pDefaultContext = gJSBrowserContext->DefaultContext();
    if (pDefaultContext)
    {
        pDefaultContext->tracefp = fp;
    }
#endif
}
void SetContextTracefp(JSContext* pContext)
{
#if (WEBC_DEBUG_LOG_JSCRIPT)
JSContext* pDefaultContext;

    pDefaultContext = gJSBrowserContext->DefaultContext();
    if (pDefaultContext)
    {
        pContext->tracefp = pDefaultContext->tracefp;
    }
#endif
}
#endif

/*---------------------------------------------------------------------------*/
WebcJSBrowserContext::~WebcJSBrowserContext (void)
{
	JSContext* acx = 0;
	JSContext* iterp = WEBC_NULL;

#if (WEBC_DEBUG_JSCRIPT)
    if (mJSDebug)
	    WEBC_DELETE(mJSDebug);
#endif
	while ((acx = JS_ContextIterator(mRuntime, &iterp)) != WEBC_NULL)
	{
        JS_GC(acx);
		JS_DestroyContext(acx);
		iterp = WEBC_NULL;
	}

	JS_DestroyRuntime(mRuntime);
	JS_ShutDown();
}

/*---------------------------------------------------------------------------*/
JSContext* WebcJSBrowserContext::AllocContext (void)
{
    JSContext*newContext;
	if (mNumFreeContexts > 0)
	{
		mNumFreeContexts--;
#if(WEBC_DEBUG_PARSER)
		SetContextTracefp(mContextPool[mNumFreeContexts]);
#endif
		return (mContextPool[mNumFreeContexts]);
	}

	newContext = JS_NewContext(mRuntime, WEBC_JS_CONTEXT_STACK_SIZE);
#if(WEBC_DEBUG_PARSER)
    if (newContext)
	    SetContextTracefp(newContext);
#endif
	return (newContext);
}

/*---------------------------------------------------------------------------*/
void WebcJSBrowserContext::FreeContext (JSContext* cx)
{
	if (mNumFreeContexts < WEBC_CFG_JS_CX_POOL_SIZE)
	{
		mContextPool[mNumFreeContexts++] = cx;
	  #ifdef WEBC_JS_GARBAGE_COLLECT_INTERVAL
		if (!mGCCounter)
		{
			JS_GC(cx);
			mGCCounter = WEBC_JS_GARBAGE_COLLECT_INTERVAL;
		}
		else
		{
			mGCCounter--;
		}
	  #endif // WEBC_JS_GARBAGE_COLLECT_INTERVAL
		JS_SetGlobalObject(cx, WEBC_NULL);
	}
	else
	{
		JS_DestroyContext(cx);
	}
}

/*---------------------------------------------------------------------------*/
void WebcJSBrowserContext::FreeTempStore (void)
{
	int i;

	for (i = 0; i < mNumFreeContexts; i++)
	{
		JS_DestroyContext(mContextPool[i]);
	}

	mNumFreeContexts = 0;
}

#endif // WEBC_SUPPORT_JSCRIPT
