//
// WEBCSDKMISC.CPP -
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
#include "htmlsdk.h"
#include "wcapi.h"
#include "httpc.h"
#if (WEBC_SUPPORT_JSCRIPT)
 #include "wjsdoc.h"
 #include "wjsbrowser.h"
 #include "wjscript.hpp"
#endif
#if (WEBC_SUPPORT_COOKIES)
 #include "cookie.h"
#endif
#include "hchkbox.hpp"
#include "helement.hpp"
#include "hframe.hpp"
#include "himage.hpp"
#include "hobject.hpp"
#include "hinput.hpp"
#include "hhidden.hpp"
#include "hedit.hpp"
#include "hpasswd.hpp"
#include "hselect.hpp"
#include "htmldoc.hpp"
#include "htmlfind.hpp"
#include "format.hpp"
#include "webctmo.h"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "prothttp.h"
#include "wgkeys.hpp"
#include "jhutils.h"
#include "tuenum.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "rtpprint.h"
#include "webcdefault.h"
#ifdef WEBC_DEBUG
#include "webcLog.h"
#endif
#if (WEBC_DEBUG_JSCRIPT)
#include "wjsdebug.h"
#endif


/*****************************************************************************/
// Local types
/*****************************************************************************/
#include "webcsdk.hpp"

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// External Declarations
/*****************************************************************************/


/*****************************************************************************/
// Macros
/*****************************************************************************/

/*****************************************************************************/
// HTML C API Functions
/*****************************************************************************/


#if (1)
/*****************************************************************************/
// wcJSIntroduction
/**
 @memo Introduction to interfacing with Javascript from 'C' and extending Javascript for your application.

 @doc webC's Javascript support is derived from the Mozilla SpiderMonkey Javascript engine.
 This section documents specific functions that you can use to develop webC applications that use both C and Javascript.

There are many reasons that you might wish to use this aproach:

 <ul>
 <li> There are many skilled Javascript programmers and an infrastructure of code, tools and other resources.</li>
 <li> Developing apps in interpreted languages with object oriented features, like Javascript, is arguably today's most common development paradigm.</li>
 <li> Use of JSON and other Javascript techniques make interfaces easy to personalize, customize and and update.</li>
 <li> C is easily accesable from Javascript for real time processing, communicating with an RTOS and other system activities.</li>
 <li> Custom methods, objects and classes may be implemented in C that extend Javascript to directly support your device and application.</li>
 <li> These custom resources may be accessed from Javascript code driving your application.</li>
 <li> Javascript source code can come from multiple sources</li>
 <ul>
 <li>From the rom image of your application (rom:\\)</li>
 <li>From a file system (file:\\)</li>
 <li>From the network (http:\\)</li>
 <li>From 'C' arrays that have been dynamically filled with Javascript source code from 'C'.</li>
 or retrieved from file, necode driving your application.</li>
 </ul>
 <li> Server based applications can stay in control and still access remote device resources.</li>
 <li> Standalone applications can use Javascript for some or all of their user interface while keeping performing sytem task in C.</li>
 </ul>
<pre>
For more information beyond what is in this section you should also consult the following resources.
<ul>
<li>Useful references are also available from mozilla.org.</li>
<ul>
<li><A HREF="https://developer.mozilla.org/en/SpiderMonkey">SpiderMonkey main page.</A></li>
<li><A HREF="https://developer.mozilla.org/En/SpiderMonkey/JSAPI_User_Guide">JSAPI_User_Guide.</A></li>
<li><A HREF="https://developer.mozilla.org/en/SpiderMonkey/JSAPI_Reference">JSAPI_Reference.</A></li>
<li><A HREF="https://developer.mozilla.org/En/SpiderMonkey/JSAPI_Phrasebook">JSAPI_Phrasebook - Howto for typical things you might want to do.</A></li>
<li><A HREF="https://developer.mozilla.org/En/SpiderMonkey/Internals">Internals - How it works.</A></li>
</ul>
<li>You may also wish to study webC source modules for examples of C and Javascript working in concert.</li>
<ul>
<li>C++ source for webC's optional Chrome browser web browser console in <b>webc/source/browser/Chromebrowser.cpp</b></li>
<li>Javascript source for webC's optional Chrome browser web browser console in <b>webc/html/chrome/chrome.html, et al.</li>
<li>webC's DOM interface for Javascript. Multiple files in the webC source <b>webc/source/jscript/*.cpp</b></li>
</ul>
</ul>
</pre>

*/
/*****************************************************************************/

void wcJSIntroduction(void){}
#endif

// HEREHERE Begin websdkmisc.cpp

#if (WEBC_SUPPORT_JSCRIPT)



/*****************************************************************************/
// wcJSExecuteSource
/**
 @memo Call Javascript from native 'C' and execute the provided javascript source code

 @doc This function allows a C function to called javascript to compile and execute source code.
		This function returns a handle to a script.
		wcJSDestroyScript() must be called to stop the script and free
        the script resources.
<pre>
<A HREF="https://developer.mozilla.org/En/SpiderMonkey/JSAPI_User_Guide">JSAPI_User_Guide</A>
</pre>
 @param Ctx wc context structure referencing any created document
 @param source the Javascript source code to compile and execute
 @return a handle to the script
*/
/*****************************************************************************/

HSCRIPT_HANDLE wcJSExecuteSource(wcCtx* Ctx, const char *_source)
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR source[HTML_TEMP_STR_LEN];
	webc_c_strncpy(source, _source, HTML_TEMP_STR_LEN-1);
	source[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	const WEBC_CHAR *source = _source;
  #endif
	HDOC_HANDLE doc = Ctx->hdoc;
  /* Okay.. here we go */
  WebcJSDocumentContext *jsmgr = HDOC_OBJ(doc)->GetJSMgr();
  JSObject *scopeobj = HDOC_OBJ(doc)->GetJSObject();
  JSContext *cx = HDOC_OBJ(doc)->GetJSContext();

  WebcJSScript *pScript;
  WEBC_NEW_VERBOSE(pScript, WebcJSScript(jsmgr, scopeobj, source, webc_strlen(source)),"WebcJSScript");
  if (pScript)
  {
    jsval Result;
    pScript->ExecuteScript(&Result);
  }
  return (HSCRIPT_HANDLE) pScript;
}

// Old calling convention.
HSCRIPT_HANDLE webc_JScriptExecuteSource(HDOC_HANDLE doc, const char *_source)
{
	wcCtx Ctx;
	wcCtxtInit(&Ctx,wcDocGetBrowser(doc) , doc);
	return wcJSExecuteSource(&Ctx, _source);
}
// Legacy version
void webc_JScriptDefineFunction(HDOC_HANDLE doc, const char *name, JSNative func, int argc)
{
	HDOC_OBJ(doc)->DefineJSFunction(func, argc, name);
}

#if(1)
void webc_JScriptDestroyScript(HSCRIPT_HANDLE  hscript)
{
	wcJSDestroyScript(hscript);
}

/*****************************************************************************/
// wcJSDestroyScript
/**
 @memo Destroy and free reources of a script that was created by wcJSExecuteSource

 @doc This function stops, destroys and frees resources of a script that was created by wcJSExecuteSource.

 @param hscript a script handle returned from wcJSExecuteSource
 @return void
*/
/*****************************************************************************/

void wcJSDestroyScript(HSCRIPT_HANDLE  hscript)
{
  WebcJSScript *pScript = (WebcJSScript *) hscript;
  if (pScript)
    pScript->Destroy();   /* Free it */
}



/*****************************************************************************/
// wcJSDefineFunction
/**
 @memo Call a native C function from Javascript

 @doc This function allows a native C function to be called by javascript.
		This function must have this signature:
		JSBool someName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
		When the function executes, cx will be the JSContext that is associated with the
		executing script.  obj is the global object that can access all other objects
		in running scripts.  argc is the number of arguments passed into the function.
		argv is an array of jsvalues that contain the parameters passed into the function.
		rval should be set by the user and will be the return value.  The function should
		return JS_TRUE.

 @param doc any created document
 @param name the name that the javascript interpretor will recognize.
 @param func the native function that will be called, where func is of the type:<br>
 &nbsp;&nbsp;&nbsp;&nbsp;JSBool Funcname(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
 @param argc the number parameter count
*/
/*****************************************************************************/
void wcJSDefineFunction(wcCtx* Ctx, const char *name, JSNative func, int argc)
{
	HDOC_HANDLE doc;
	doc = (HDOC_HANDLE) Ctx->hdoc;
	HDOC_OBJ(doc)->DefineJSFunction(func, argc, name);
}

// Legacy API
HDOC_HANDLE webc_JScriptGetDoc(struct JSContext *cx)
{
wcCtx Ctx;
	if (wcJSInitializeDocCtxt(cx,&Ctx))
		return (HDOC_HANDLE) Ctx.hdoc;
	else
		return 0;
}


/*****************************************************************************/
/**
 @memo Initialize a webC context for use from a javascript context.

 @doc  This function should be called when entering a C handler from Javascript. It initializes a context block
 that may be passed to other wcAPI functions. When the C handler completes it should call call wcCtxtRelease before it returns.
 This will ensure that if any garbage was accumullated in the context block it will be freed.

 @param cx  - The javascript context that was running
 @param Ctx - Pointer to a webC context structure to be initialized.
  @return wcTRUE if the javascript context could be mapped succesfully, otherwise wcFALSE.
 */
/*****************************************************************************/

wcBOOL wcJSInitializeDocCtxt(struct JSContext *cx, wcCtx *Ctx)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (jsmgr)
	{
		wcCtxtInit(Ctx,wcDocGetBrowser((wcDOC)jsmgr->GetDocument()), (wcDOC)jsmgr->GetDocument());
		return wcTRUE;
	}
	return wcFALSE;
}


#endif // HEREHERE
#endif // WEBC_SUPPORT_JSCRIPT
