//
// WJSCRIPT.CPP - Methods for WebcJSScript-derived classes
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "webcjhconfig.h"
#include "wjscript.hpp"
#if (WEBC_SUPPORT_JSCRIPT)
#ifdef __cplusplus
extern "C" {
#endif
#include "jsapi.h"
#ifdef __cplusplus
}
#endif
#include "util.h"
#include "htmldoc.hpp"
#include "dmanager.hpp"
#include "webcstr.h"
#include "jhutils.h"
#include "webcassert.h"
#include "rtptime.h"
#include "objref.cpp"
#include "webcmem.h"
#include "jsobjref.hpp"

/*****************************************************************************/
// Macros
/*****************************************************************************/

#define WEBC_CFG_SCRIPT_MAX_BRANCHES            0xfffffL  // ~1 million
#define WEBC_CFG_SCRIPT_MAX_RUNNING_TIME_MSEC   (10000L)  // 10 sec
//#define WEBC_CFG_SCRIPT_MAX_RUNNING_TIME_MSEC   (1000000L)  // 1000 sec

/*****************************************************************************/
// Function bodies
/*****************************************************************************/

JSObject *webcjs_NewEventObject(JSContext *pCx, HTMLEvent *pEvent)
{
	// tbd - not yet implemented
	return WEBC_NULL;
}


/*****************************************************************************/
// WebcJSScript methods
/*****************************************************************************/

WebcJSScript::WebcJSScript(WebcJSDocumentContext *pJSMgr)
{
	mpJSMgr = pJSMgr;
	mbFlags = JS_SCRIPT_FLAG_DEFAULT;
	mCompiled.pScript = 0;
	mBranchCount = 0;
	mStartTimeMsec = 0;
	// IMPORTANT: this constructor does not and SHOULD not call WebcJSDocumentContext::Claim
}

WebcJSScript::WebcJSScript(WebcJSDocumentContext *pJSMgr, JSObject *pScope, const WEBC_CHAR *pSource, long lSize)
{
	mpJSMgr = pJSMgr;
	mScopeObject.Set(pScope);
	mbFlags = 0;
	mCompiled.pScript = 0;
	mBranchCount = 0;
	mStartTimeMsec = 0;

	if (mpJSMgr)
	{
		mpJSMgr->claim();
	}

	if (pSource && lSize)
	{
		Compile(pSource, lSize);
	}
}

// destructor
WebcJSScript::~WebcJSScript(void)
{
	WEBC_ASSERT((mbFlags & JS_SCRIPT_FLAG_DELETED) || (mbFlags & JS_SCRIPT_FLAG_DEFAULT));

	// deletes the script
	Clear();

	if (mpJSMgr && !CheckFlag(JS_SCRIPT_FLAG_DEFAULT))
	{
		mpJSMgr->release();
	}
}

void WebcJSScript::Clear(void)
{
	ClearScript();
	mSourceBuffer.discardAll();
}

void WebcJSScript::SetSource(const WEBC_CHAR* source, long size)
{
	SetFlag(JS_SCRIPT_FLAG_NEW_SOURCE);
	mSourceBuffer.discardAll();
	Write(source, size);
}

// clear the contents of this script
void WebcJSScript::ClearScript(void)
{
	if (mCompiled.pScript)
	{
		mMyObject.Set(0);
		mScopeObject.Set(0);

		// Don't know why this would happen, but if we are running, we should stop
		Stop();
		mCompiled.pScript = 0;
	}
}

// write source code into this script's "to-compile" buffer
long WebcJSScript::Write(const WEBC_CHAR *pSource, long iSize)
{
	if (mSourceBuffer.append(pSource, iSize) >= 0)
	{
		return (iSize);
	}

	return (-1);
}

// compile all pending source; this will erase the old compiled script
int WebcJSScript::Compile(void)
{
	int result;

	result = Compile(mSourceBuffer.getChars(), mSourceBuffer.getLength());

	return (result);
}

// compile the given source; this will erase the old compiled script
int WebcJSScript::Compile(const WEBC_CHAR *pSource, long lSize)
{

#if(WEBC_DEBUG_PARSER)
    WriteParseLog("\n<!-- >>>           =====Jscript Compiling Source ============                -->\n", pSource);
#endif
	if (mScopeObject.Get() && mpJSMgr && pSource)
	{

		JSContext *pCx = mpJSMgr->AllocContext();
		if (!pCx)
		{
#if(WEBC_DEBUG_PARSER)
            WriteParseLog("\n<!-- >>>           =====Jscript Compiling Failure 1 ============                -->\n",0);
#endif
			// out of memory/JSContexts
			return (-1);
		}
		ClearScript();

	  #if (WEBC_SUPPORT_UNICODE)
//		mCompiled.pScript = JS_CompileUCScript(pCx, mScopeObject.Get(), (jschar*) pSource, lSize, __FILE__, __LINE__);
		mCompiled.pScript = JS_CompileUCScript(pCx, mScopeObject.Get(), (jschar*) pSource, lSize, __FILE__, 0);
	  #else
//		mCompiled.pScript = JS_CompileScript(pCx, mScopeObject.Get(), pSource, lSize, __FILE__, __LINE__);
		mCompiled.pScript = JS_CompileScript(pCx, mScopeObject.Get(), pSource, lSize, __FILE__, 0);
	  #endif

		// Create a JSObject for the script and root it to prevent script-local
		//  constants from being garbage collected.
		if (mCompiled.pScript)
		{
			mMyObject.Set(JS_NewScriptObject(pCx, mCompiled.pScript));
		}
		else
		{
			mMyObject.Set(0);
		}

		mpJSMgr->FreeContext(pCx);
	}
	else
	{
#if(WEBC_DEBUG_PARSER)
        WriteParseLog("\n<!-- >>>           =====Jscript Compiling Failure No source ============                -->\n",0);
#endif
		return (-1);
	}

	if (mCompiled.pScript == 0 || mMyObject.Get() == 0)
	{
#if(WEBC_DEBUG_PARSER)
        WriteParseLog("\n<!-- >>>           ===== (mCompiled.pScript == 0 || mMyObject.Get() == 0) ============                -->\n",0);
#endif
		return (-1);
	}

	mSourceBuffer.freeBuffer();

	return (0);
}

// run the script
WebcJSScriptResult WebcJSScript::ExecuteScript(jsval *pResult)
{
	if (CheckFlag(JS_SCRIPT_FLAG_NEW_SOURCE))
	{
		ClearFlag(JS_SCRIPT_FLAG_NEW_SOURCE);
		Compile();
	}

	return (ExecuteScript(mCompiled.pScript, pResult));
}

WebcJSScriptResult WebcJSScript::ExecuteScript(JSScript *pScript, jsval *pResult)
{
WebcJSDocumentContext *pMgr = mpJSMgr;
JSContext *pCx;
JSBool result;

#if(WEBC_DEBUG_PARSER)
    WriteParseLog("\n<!-- !!!! (====Script ExecuteScript) -->\n", 0);
#endif

	if (CheckFlag(JS_SCRIPT_FLAG_EXECUTING))
	{
#if(WEBC_DEBUG_PARSER)
        WriteParseLog("\n<!-- !!!! (===Script Error Executing) -->\n", 0);
#endif
		return (WEBC_JS_SCRIPT_ERROR);
	}

	if (!pScript)
	{
#if(WEBC_DEBUG_PARSER)
        WriteParseLog("\n<!-- !!!! (===Script Error None) -->\n", 0);
#endif
		// error: no script to run
		return (WEBC_JS_SCRIPT_NO_SCRIPT);
	}

	pCx = pMgr->AllocContext();

	if (!pCx)
	{
		// out of memory/JSContexts
#if(WEBC_DEBUG_PARSER)
        WriteParseLog("\n<!-- !!!! (====Script Error Mem) -->\n", 0);
#endif
		return (WEBC_JS_SCRIPT_MEMORY_ERROR);
	}

	SetFlag(JS_SCRIPT_FLAG_EXECUTING);
	ClearFlag(JS_SCRIPT_FLAG_STOPPED);

	mBranchCount = 0;
	mStartTimeMsec = rtp_get_system_msec();

	// when a script/function/event handler runs the private context pointer is set
	//  to the executing object
	jhutil_SetContextPrivate(pCx, (WebcJSScript *) this);
	result = JS_ExecuteScript(pCx, mScopeObject.Get(), pScript, pResult);

	ClearFlag(JS_SCRIPT_FLAG_EXECUTING);

	if (CheckFlag(JS_SCRIPT_FLAG_DELETED))
	{
		Destroy();
		pMgr->FreeContext(pCx);
#if(WEBC_DEBUG_PARSER)
        WriteParseLog("\n<!-- !!!! (====Script Deleete) -->\n", 0);
#endif
		return (WEBC_JS_SCRIPT_DELETED);
	}

	pMgr->FreeContext(pCx);
	if (CheckFlag(JS_SCRIPT_FLAG_STOPPED))
	{
#if(WEBC_DEBUG_PARSER)
        WriteParseLog("\n<!-- !!!! (====Script Error Stopped) -->\n", 0);
#endif
		return (WEBC_JS_SCRIPT_STOPPED);
	}
#if(WEBC_DEBUG_PARSER)
        WriteParseLog("\n<!-- !!!! (====Script OK) -->\n", 0);
#endif
	return (WEBC_JS_SCRIPT_OK);

}

void WebcJSScript::IncBranchCount (void)
{
	if (mBranchCount < WEBC_CFG_SCRIPT_MAX_BRANCHES)
	{
		mBranchCount++;
	}
	else
	{
		Stop();
	}
}

void WebcJSScript::CheckRunningTime (void)
{
	long runningTimeMsec = rtp_get_system_msec() - mStartTimeMsec;

	if (runningTimeMsec > WEBC_CFG_SCRIPT_MAX_RUNNING_TIME_MSEC)
	{
		Stop();
	}
}

// stop the script mid-execution
int WebcJSScript::Stop(void)
{
//    webc_user_alert_box("JavaScript Stoped");
    return SetFlag(JS_SCRIPT_FLAG_STOPPED);

}

// call this to delete a script... it is safe even if the script is running
int WebcJSScript::Destroy(void)
{
	SetFlag(JS_SCRIPT_FLAG_DELETED);

	if (CheckFlag(JS_SCRIPT_FLAG_EXECUTING))
	{
		SetFlag(JS_SCRIPT_FLAG_STOPPED);
	}
	else
	{
		WEBC_DELETE(this);
	}

	return (0);
}

// utility methods to manage the flags for this script
int WebcJSScript::SetFlag(WEBC_UINT16 wFlag)
{
int iRetVal = CheckFlag(wFlag);
	mbFlags |= wFlag;
	return (iRetVal);
}

int WebcJSScript::CheckFlag(WEBC_UINT16 wMask)
{
	return (mbFlags & wMask);
}

int WebcJSScript::ClearFlag(WEBC_UINT16 wFlag)
{
int iRetVal = CheckFlag(wFlag);
	mbFlags &= ~wFlag;
	return (iRetVal);
}

HTMLBrowser* WebcJSScript::GetBrowser(void)
{
	if (mpJSMgr)
	{
		HTMLDocument* doc = mpJSMgr->GetDocument();
		if (doc)
		{
			return (doc->GetBrowser());
		}
	}
	return (0);
}

DisplayManager* WebcJSScript::GetDisplayManager(void)
{
	if (mpJSMgr)
	{
		HTMLDocument* doc = mpJSMgr->GetDocument();
		if (doc)
		{
			return (doc->GetDisplayManager());
		}
	}
	return (0);
}

/*****************************************************************************/
// WebcJSFunction methods
/*****************************************************************************/

WebcJSFunction::WebcJSFunction (
		WebcJSDocumentContext *pJSMgr,
		JSObject *pScope,
		const char *name,
		int nargs,
		char **argnames,
		const WEBC_CHAR *pSource,
		long lSize
	)
	: WebcJSScript(pJSMgr, pScope, 0, 0)
{
	mpName = 0;
	mppArgNames = 0;
	miNumArgs = 0;

	SetName(name);

	if (nargs && argnames)
	{
		SetArgs(nargs, argnames);
	}

	if (pSource && lSize)
	{
		Compile(pSource, lSize);
	}
}

WebcJSFunction::WebcJSFunction(WebcJSDocumentContext *pJSMgr, JSObject *pScope, const char *name, JSFunction *pFunc)
	: WebcJSScript(pJSMgr, pScope, 0, 0)
{
	mpName = 0;
	mppArgNames = 0;
	miNumArgs = 0;

	SetName(name);

	if (pFunc)
	{
		SetFunction(pFunc);
	}
}

WebcJSFunction::~WebcJSFunction(void)
{
	Clear();

	webc_free_string_copy_8(mpName, __FILE__, __LINE__);
	FreeArgs();
}

void WebcJSFunction::ClearScript(void)
{
// JSBool found;

	if (mCompiled.pFunction)
	{
		JSContext *pCx;

		pCx = mpJSMgr->AllocContext();
		if (pCx)
		{
			// Clear the permanent and read-only attributes of the function
//			JS_SetPropertyAttributes(pCx, mScopeObject.Get(), JS_GetFunctionName(mCompiled.pFunction), 0, &found);

			// Remove it from the scope object; it will now be garbage collected
//			if (found == JS_TRUE)
			{
//				JS_DeleteProperty(pCx, mScopeObject.Get(), JS_GetFunctionName(mCompiled.pFunction));
			}
			mpJSMgr->FreeContext(pCx);
		}

		// Don't know why this would happen, but if we are running, we should stop
		Stop();

		mCompiled.pScript = 0;
		mCompiled.pFunction = 0;
		mMyObject.Set(0);
	}
}

// compile all pending source; this will erase the old compiled script
int WebcJSFunction::Compile(void)
{
	int result;

	result = Compile(mSourceBuffer.getChars(), mSourceBuffer.getLength());

	return (result);
}

int WebcJSFunction::Compile(const WEBC_CHAR *pSource, long lSize)
{
	if (!mScopeObject.Get() || !mpName || !pSource || !mpJSMgr)
	{
		return (-1);
	}

	JSContext *pCx;
	JSBool found;

	// Grab a context to use for compiling
	pCx = mpJSMgr->AllocContext();
	if (!pCx)
	{
		// out of memory/JSContexts
		return (-1);
	}
	ClearScript();

#if (WEBC_SUPPORT_UNICODE)
	mCompiled.pFunction = JS_CompileUCFunction (
			pCx,
			mScopeObject.Get(),
			mpName,
			miNumArgs,
			(const char **) mppArgNames,
			pSource,
			lSize,
			__FILE__,
			__LINE__
		);
#else
	mCompiled.pFunction = JS_CompileFunction (
			pCx,
			mScopeObject.Get(),
			mpName,
			miNumArgs,
			(const char **) mppArgNames,
			pSource,
			lSize,
			__FILE__,
			__LINE__
		);
#endif

	// if this is NULL, it means compile failed somehow
	if (mCompiled.pFunction)
	{
		mMyObject.Set(JS_GetFunctionObject(mCompiled.pFunction));
		JSObject *parent = JS_GetParent(pCx, mMyObject.Get());
		// protect this object so it won't be deleted
		JS_SetPropertyAttributes(pCx, mScopeObject.Get(), mpName, JSPROP_READONLY|JSPROP_PERMANENT, &found);
	}
	else
	{
		mMyObject.Set(0);
	}

	// we are done with the context... release it
	mpJSMgr->FreeContext(pCx);

	mSourceBuffer.freeBuffer();

	return (0);
}

void WebcJSFunction::SetFunction(JSFunction *pFunc)
{
JSContext *pCx;
JSBool found;

	if (mpName)
	{
		// Grab a context to use for compiling
		pCx = mpJSMgr->AllocContext();
		if (!pCx)
		{
			// out of memory/JSContexts
			return;
		}
		Clear();
		mCompiled.pFunction = pFunc;
		mMyObject.Set(JS_GetFunctionObject(mCompiled.pFunction));

		JS_DefineProperty(pCx, mScopeObject.Get(), mpName, OBJECT_TO_JSVAL(mMyObject.Get()), 0,0,0);

		// protect this object so it won't be deleted
		JS_SetPropertyAttributes(pCx, mScopeObject.Get(), mpName, JSPROP_READONLY|JSPROP_PERMANENT, &found);

		// we are done with the context... release it
		mpJSMgr->FreeContext(pCx);
	}
}

WebcJSScriptResult WebcJSFunction::ExecuteFunction(jsval *pResult, int argc, jsval *argv)
{
	if (CheckFlag(JS_SCRIPT_FLAG_NEW_SOURCE))
	{
		ClearFlag(JS_SCRIPT_FLAG_NEW_SOURCE);
		Compile();
	}

	return (ExecuteFunction(mCompiled.pFunction, pResult, argc, argv));
}

WebcJSScriptResult WebcJSFunction::ExecuteFunction(JSFunction *pFunc, jsval *pResult, int argc, jsval *argv)
{
WebcJSDocumentContext *pMgr = mpJSMgr;
JSContext *pCx;
JSBool result;

	if (CheckFlag(JS_SCRIPT_FLAG_EXECUTING))
	{
		return (WEBC_JS_SCRIPT_ERROR);
	}

	if (!pFunc)
	{
		// error: no script to run
		return (WEBC_JS_SCRIPT_NO_SCRIPT);
	}

	pCx = pMgr->AllocContext();
	if (!pCx)
	{
		// out of memory/JSContexts
		return (WEBC_JS_SCRIPT_MEMORY_ERROR);
	}

	SetFlag(JS_SCRIPT_FLAG_EXECUTING);
	ClearFlag(JS_SCRIPT_FLAG_STOPPED);

	mBranchCount = 0;
	mStartTimeMsec = rtp_get_system_msec();

	// when a script/function/event handler runs the private context pointer is set
	//  to the executing object
	jhutil_SetContextPrivate(pCx, (WebcJSScript*) this);

	// Stop screen refreshes from occurring while we're inside the script
	if (GetDisplayManager())
	{
		DISPLAY_MANAGER_STOP_REFRESH(GetDisplayManager());
	}

	result = JS_CallFunction(pCx, mScopeObject.Get(), pFunc, argc, argv, pResult);

	// Resume screen refreshes
	if (GetDisplayManager())
	{
		DISPLAY_MANAGER_RESUME_REFRESH(GetDisplayManager());
	}

	ClearFlag(JS_SCRIPT_FLAG_EXECUTING);

	if (CheckFlag(JS_SCRIPT_FLAG_DELETED))
	{
		Destroy();
		pMgr->FreeContext(pCx);
		return (WEBC_JS_SCRIPT_DELETED);
	}

	pMgr->FreeContext(pCx);

	if (CheckFlag(JS_SCRIPT_FLAG_STOPPED))
	{
		return (WEBC_JS_SCRIPT_STOPPED);
	}

	if (result == JS_TRUE)
	{
		return (WEBC_JS_SCRIPT_OK);
	}

	return (WEBC_JS_SCRIPT_ERROR);
}

void WebcJSFunction::SetName(const char *pName)
{
	char *pStr = webc_malloc_string_copy_8(pName, __FILE__, __LINE__);
	webc_free_string_copy_8(mpName, __FILE__, __LINE__);
	mpName = pStr;
}

void WebcJSFunction::SetArgs(int iNumArgs, char **ppArgNames)
{
int n;

	FreeArgs();

	miNumArgs = iNumArgs;
	if (miNumArgs)
	{
		if (ppArgNames)
		{
			mppArgNames = (char **) WEBC_MALLOC(sizeof(char *) * miNumArgs);
			if (!mppArgNames)
			{
				// malloc failure
				miNumArgs = 0;
				return;
			}

			for (n=0; n<miNumArgs; n++)
			{
				mppArgNames[n] = webc_malloc_string_copy_8(ppArgNames[n], __FILE__, __LINE__);
			}
		}
	}
}

void WebcJSFunction::FreeArgs(void)
{
int n;

	if (miNumArgs)
	{
		if (mppArgNames)
		{
			for (n=0; n<miNumArgs; n++)
			{
				webc_free_string_copy_8(mppArgNames[n], __FILE__, __LINE__);
				mppArgNames[n] = 0;
			}
			WEBC_FREE(mppArgNames[n]);
			mppArgNames = 0;
		}
		miNumArgs = 0;
	}
}


/*****************************************************************************/
// WebcJSEventHandler methods
/*****************************************************************************/

WebcJSEventHandler::WebcJSEventHandler(WebcJSDocumentContext *pJSMgr, JSObject *pScope, const char *name, const WEBC_CHAR *pSource, long lSize)
	: WebcJSFunction(pJSMgr, pScope, name, 0, 0, 0, 0)
{
	Write(pSource, lSize);
}

WebcJSEventHandler::WebcJSEventHandler(WebcJSDocumentContext *pJSMgr, JSObject *pScope, const char *name, JSFunction *pFunc)
	: WebcJSFunction(pJSMgr, pScope, name, pFunc)
{
}

WebcJSEventHandler::~WebcJSEventHandler(void)
{
}

HTMLEventStatus WebcJSEventHandler::Execute(HTMLEvent *pEvent)
{
	if (CheckFlag(JS_SCRIPT_FLAG_NEW_SOURCE))
	{
		ClearFlag(JS_SCRIPT_FLAG_NEW_SOURCE);
		Compile();
	}

	if (!mCompiled.pFunction)
	{
		Compile();
	}

	return (ExecuteEventHandler(mCompiled.pFunction, pEvent));
}

HTMLEventStatus WebcJSEventHandler::ExecuteEventHandler(JSFunction* pFunc, HTMLEvent* pEvent)
{
jsval rval;
JSBool result = JS_TRUE;
jsval argv[1];

	// tbd - which event model to use: IE-style (dumb but ubiquitous) or
	//  Netscape (smart but no one uses Netscape) *sigh*
	mpJSMgr->SetWindowEvent(pEvent);

	argv[0] = OBJECT_TO_JSVAL(GetJSMgr()->Event());
	switch (ExecuteFunction(pFunc, &rval, 1, argv))
	{
		case WEBC_JS_SCRIPT_OK:
			// AHHH!!!  there are no consistent conventions for the meaning of
			//  the return value of a Javascript event handler, so we handle
			//  each case uniquely based on the the type of event
			if (JSVAL_IS_BOOLEAN(rval))
			{
				result = JSVAL_TO_BOOLEAN(rval);
				if (pEvent->type == HTML_EVENT_MOUSEOVER)
				{
					if (result == JS_FALSE)
					{
						pEvent->flags &= ~HTML_EVENT_FLAG_CANCEL_DEFAULT;
					}
					else
					{
						pEvent->flags |= HTML_EVENT_FLAG_CANCEL_DEFAULT;
					}
				}
				else
				{
					if (result == JS_TRUE)
					{
						pEvent->flags &= ~HTML_EVENT_FLAG_CANCEL_DEFAULT;
					}
					else
					{
						pEvent->flags |= HTML_EVENT_FLAG_CANCEL_DEFAULT;
					}
				}
			}

			// now check the window's event object
			if(GetJSMgr()->GetEventFlags() & HTML_EVENT_FLAG_CANCEL_DEFAULT)
			{
				pEvent->flags |= HTML_EVENT_FLAG_CANCEL_DEFAULT;
			}

			break;

		case WEBC_JS_SCRIPT_STOPPED:
		case WEBC_JS_SCRIPT_DELETED:
			return (HTML_EVENT_STATUS_HALT);

		default:
			break;
	}

	return (HTML_EVENT_STATUS_DONE);
}

#endif // WEBC_SUPPORT_JSCRIPT
