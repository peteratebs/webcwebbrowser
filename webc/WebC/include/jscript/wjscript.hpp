//
// WJSCRIPT.HPP - Structs, prototypes, and defines for WJSCRIPT.CPP
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __JSCRIPT_HPP_INCLUDED__
#define __JSCRIPT_HPP_INCLUDED__


/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "webc.h"
#if (WEBC_SUPPORT_JSCRIPT)
#ifdef __cplusplus
extern "C" {
#endif
#include "jsapi.h"
#ifdef __cplusplus
}
#endif
#include "webstr.hpp"
#include "jsobjref.hpp"

/*****************************************************************************/
// Constants and Macros
/*****************************************************************************/

// Values for mbFlags below
#define JS_SCRIPT_FLAG_EXECUTING  0x01
#define JS_SCRIPT_FLAG_DELETED    0x02
#define JS_SCRIPT_FLAG_STOPPED    0x04 // different from just not executing;
                                       // this flag is set when a script is
                                       // explicitly halted
#define JS_SCRIPT_FLAG_DEFAULT    0x08
#define JS_SCRIPT_FLAG_NEW_SOURCE 0x10

enum WebcJSScriptType
{
	WEBC_JS_SCRIPT,
	WEBC_JS_FUNCTION,
	WEBC_JS_EVENT_HANDLER
};

enum WebcJSScriptResult
{
	WEBC_JS_SCRIPT_NO_SCRIPT = -4,
	WEBC_JS_SCRIPT_IO_ERROR = -3,
	WEBC_JS_SCRIPT_MEMORY_ERROR = -2,
	WEBC_JS_SCRIPT_ERROR = -1,
	WEBC_JS_SCRIPT_OK = 0,
	WEBC_JS_SCRIPT_STOPPED,
	WEBC_JS_SCRIPT_DELETED
};


/*****************************************************************************/
// Function prototypes
/*****************************************************************************/

// Creates a new event object to pass to event handlers - tbd is this the event
//  model we want to use (Netscape style, which is better but probably less
//  widely utilized) or IE style (brain-dead global event object)
JSObject *webcjs_NewEventObject(JSContext *pCx, HTMLEvent *pEvent);


/*****************************************************************************/
// Class definitions
/*****************************************************************************/

class WebcJSDocumentContext;
class DisplayManager;

class WebcJSScript
{
public:
	// constructor
	WebcJSScript(WebcJSDocumentContext *pJSMgr);

	// constructor
	WebcJSScript(WebcJSDocumentContext *pJSMgr, JSObject *pScope, const WEBC_CHAR *pSource = 0, long lSize = 0);

	// destructor
	virtual ~WebcJSScript(void);

	// clear the contents of this script
	virtual void Clear(void);
	virtual void ClearScript(void);

	// write source code into this script's "to-compile" buffer
	virtual long Write(const WEBC_CHAR *pSource, long lSize);

	// write source code; overwrite pre-existing script
	virtual void SetSource(const WEBC_CHAR* source, long size);

	// compile all pending source; this will erase the old compiled script
	virtual int Compile(void);

	// compile the given source; this will erase the old compiled script
	virtual int Compile(const WEBC_CHAR *pSource, long lSize);

	// run the script
	virtual WebcJSScriptResult ExecuteScript(jsval *pResult);

	// stop the script mid-execution
	virtual int Stop(void);

	// call this to delete a script... it is safe even if the script is running
	virtual int Destroy(void);

	// utility methods to manage the flags for this script; set and clear return
	//  the value of the flag before it is modified
	int SetFlag(WEBC_UINT16 wFlag);
	int CheckFlag(WEBC_UINT16 wMask);
	int ClearFlag(WEBC_UINT16 wFlag);

	// what kind of object is this
	virtual WebcJSScriptType Type(void) { return WEBC_JS_SCRIPT; }

	// retrieve the manager for this script
	WebcJSDocumentContext* GetJSMgr(void) { return mpJSMgr; }
	JSObject *GetMyObject(void) { return mMyObject.Get(); }
	JSObject *GetParentObject(void) { return mScopeObject.Get(); }

	DisplayManager* GetDisplayManager(void);
	HTMLBrowser* GetBrowser(void);

	// this function can be called for one-time execution of a script or function
	virtual WebcJSScriptResult ExecuteScript(JSScript *pScript, jsval *pResult);
    // pvo added to get access to script from javascript debugger
	JSScript   *GetScript(void)  { return (mCompiled.pScript); }
	JSFunction *GetFunction(void)  { return (mCompiled.pFunction); }

	void IncBranchCount (void);
	void CheckRunningTime (void);

protected:
	WebString       mSourceBuffer;
	WebcJSDocumentContext *mpJSMgr;
	JSObjectRef     mScopeObject; // the object passed into JS_ExecuteScript or JS_CallFunction
	JSObjectRef     mMyObject; // the object which represents the script or function... different from the Owner
	WEBC_UINT8      mbFlags; // see values above
	union {
		JSScript   *pScript;
		JSFunction *pFunction;
	} mCompiled;
	long            mBranchCount;
	unsigned long   mStartTimeMsec;
};

// Keeps the code buffering abilities of JSScript but replaces methods for most everything else
class WebcJSFunction : public WebcJSScript
{
public:
	// constructor
	WebcJSFunction(WebcJSDocumentContext *pJSMgr, JSObject *pScope, const char *name, JSFunction *pFunc);

	// constructor
	WebcJSFunction(WebcJSDocumentContext *pJSMgr, JSObject *pScope, const char *name, int nargs,
	               char **argnames, const WEBC_CHAR *pSource, long lSize = 0);

	// destructor
	virtual ~WebcJSFunction(void);

	virtual void ClearScript(void);
	virtual int Compile(void);
	virtual int Compile(const WEBC_CHAR *pSource, long lSize);

	virtual WebcJSScriptType Type(void) { return WEBC_JS_FUNCTION; }

	virtual void SetName(const char *pName);
	virtual void SetArgs(int iNumArgs, char **ppArgNames);

	void SetFunction(JSFunction *pFunc);

	virtual WebcJSScriptResult ExecuteFunction(jsval *pResult, int argc, jsval *argv);
	virtual WebcJSScriptResult ExecuteFunction(JSFunction *pFunc, jsval *pResult, int argc, jsval *argv);

protected:
	void FreeArgs(void);

	char  *mpName;
	char **mppArgNames;
	short  miNumArgs;
};

// Keeps all compile/clear/destroy code from JSFunction, but adds a slightly different execute method
class WebcJSEventHandler : public WebcJSFunction
{
public:
	// constructor
	WebcJSEventHandler(WebcJSDocumentContext *pJSMgr, JSObject *pScope, const char *name, JSFunction *pFunc);

	// constructor
	WebcJSEventHandler(WebcJSDocumentContext *pJSMgr, JSObject *pScope, const char *name,
	                   const WEBC_CHAR *pSource = 0, long lSize = 0);

	// destructor
	virtual ~WebcJSEventHandler(void);

	virtual HTMLEventStatus Execute(HTMLEvent *pEvent);
	virtual WebcJSScriptType Type(void) { return WEBC_JS_EVENT_HANDLER; }

	virtual HTMLEventStatus ExecuteEventHandler(JSFunction *pFunc, HTMLEvent *pEvent);
};

#endif // WEBC_SUPPORT_JSCRIPT

#endif // __JSCRIPT_HPP_INCLUDED__
