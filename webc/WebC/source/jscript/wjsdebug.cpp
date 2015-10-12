/*
|  WJSDEBUG.C -
|
|  EBS - WebC
|
|
|  Copyright EBS Inc. , 2008
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#include "webcjhconfig.h"
#include "webc.h"
#if (WEBC_SUPPORT_JSCRIPT)
#include "wjsdebug.h"
#include "wjsdoc.h"
#include "wjsbrowser.h"
#include "vector.h"
#include "jhanchor.h"
#include "jhbody.h"
#include "jhcoll.h"
#include "jhdoc.h"
#include "jhelem.h"
#include "jhform.h"
#include "jhflow.h"
#include "jhframe.h"
#include "jhfrset.h"
#include "jhiframe.h"
#include "jhimg.h"
#include "jhinput.h"
#include "jhistory.h"
#include "jhoption.h"
#include "jhselect.h"
#include "jhtable.h"
#include "jhtarea.h"
#include "jhtblrow.h"
#include "jhtitle.h"
#include "jhtcell.h"
#include "jloc.h"
#include "jnav.h"
#include "jrule.h"
#include "jssheet.h"
#include "jstyle.h"
#include "jwindow.h"
#include "jevent.h"
#include "jsobjref.hpp"
#include "webctmo.h"
#include "wjscript.hpp"
#include "webcmem.h"
#include "hbody.hpp"
#include "jhutils.h"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "htmlbrow.hpp"
#include "webcassert.h"
#include "jscntxt.h"
#include "jsdbgapi.h"
#include "jsscript.h"
#include "webcLog.h"


/*****************************************************************************/
/* Header files
 *****************************************************************************/

#if (WEBC_DEBUG_JSCRIPT)

#define JSDEBUG_SHOW_SOURCE 0                   /* So far show source is all we can do, and not that well */
#define JSDEBUG_SHOW_OBJECT_ELEMENT_NAMES 0     /* Print webc object from JScript objects names. disabled because buggy */

/* Logfile for now at least */
FILE *gpJSLogFile = 0;

/* Debugger */

/* WEBC functions that Javascript calls as it executes */
static void JWEBCDebugSourceHandler(const char *filename, uintN lineno, jschar *str, size_t length, void **listenerTSData, void *closure);
static JSTrapStatus JWEBCDebugInterruptHook (JSContext *cx, JSScript *script, jsbytecode *pc, jsval *rval, void *closure);
static void JWEBCDebugNewScriptHook (JSContext  *cx, const char *filename, uintN lineno, JSScript   *script, JSFunction *fun, void *callerdata);
static void * JWEBCDebugInterpreterHook(JSContext *cx, JSStackFrame *fp, JSBool before, JSBool *ok, void *closure);

/* Initiate webC javascript debugging for a context called by WebcJSBrowserContext constructor */
JWEBCDebug::JWEBCDebug(JSRuntime* rt, JSContext*cx)
{
	mRuntime = rt;
	mDefaultContext = cx;
	mNlines = 0;
	mLastlineDisplayed = -1;
    rtp_memset(&strLines[0], 0, sizeof(strLines));
    LOG_JAVASCRIPT("Debug constructor called");
    /* Install debug callbacks */
    JS_SetSourceHandler(rt, JWEBCDebugSourceHandler, (void *) this);
    JS_SetInterrupt(rt, JWEBCDebugInterruptHook, (void *) this);
    JS_SetNewScriptHook(rt, JWEBCDebugNewScriptHook, (void *) this);
    JS_SetExecuteHook(rt, JWEBCDebugInterpreterHook, (void *) this);
//    JS_SetDebuggerHandler(JSRuntime *rt, JSTrapHandler handler, void *closure)
}


JWEBCDebug::~JWEBCDebug(void)
{
    LOG_JAVASCRIPT("Debug destructor called");
    /* Remove debug callbacks */
    JS_ClearInterrupt(mRuntime, 0, 0);
#if (JSDEBUG_SHOW_SOURCE)
    for (int i = 0; i < MAXLINES; i++)
    {
        if (strLines[i])
        {
            WEBC_DELETE((WebString *)strLines[i]);
            strLines[i] = 0;
        }
    }
#endif
}


static void * JWEBCDebugInterpreterHook(JSContext *cx, JSStackFrame *fp, JSBool before, JSBool *ok, void *closure)
{
    if (ok)
    {
        LOG_JAVASCRIPT("JWEBCDebugInterpreterHook: before = %d, ok == %d\n", (int) before, (int) *ok)
    }
    else
    {
        LOG_JAVASCRIPT("JWEBCDebugInterpreterHook: before = %d\n", (int) before)
    }
    return closure;
}

static void JWEBCDebugSourceHandler(const char *filename, uintN lineno,
                                    jschar *str, size_t length,
                                    void **listenerTSData, void *closure)
{
#if (JSDEBUG_SHOW_SOURCE)
jschar *tstr = str;
jschar c;
int linesize = 0;
JWEBCDebug *pJWEBCDebug = (JWEBCDebug *) closure;

    c = *tstr;
    while (*tstr)
    {
        if (*tstr == '\r' || *tstr == '\n')
        {
            break;
        }
        linesize++;
        tstr++;
    }
    c = *(str+linesize);
    *(str+linesize) = 0;
    WebString *pWs;
    WEBC_NEW(pWs, WebString(linesize+1));
    pJWEBCDebug->strLines[pJWEBCDebug->mNlines] = (void *) pWs;
    if (pJWEBCDebug->strLines[pJWEBCDebug->mNlines])
        ((WebString *)pJWEBCDebug->strLines[pJWEBCDebug->mNlines])->copy(str, linesize+1);
    pJWEBCDebug->mNlines += 1;
    *(str + linesize) = c;
#endif
}

static JSTrapStatus JWEBCDebugInterruptHook (JSContext *cx, JSScript *script, jsbytecode *pc, jsval *rval, void *closure)
{
#if (JSDEBUG_SHOW_SOURCE)
JWEBCDebug *pJWEBCDebug;
int line_number;
    pJWEBCDebug = (JWEBCDebug *) closure;
    line_number = JS_PCToLineNumber(cx, script, pc);
	if (line_number != pJWEBCDebug->mLastlineDisplayed)
    {
        if (pJWEBCDebug->strLines[line_number])
        {
            LOG_JAVASCRIPT("Executing: %S\n", ((WebString *)pJWEBCDebug->strLines[line_number])->getChars());
        }
        pJWEBCDebug->mLastlineDisplayed = line_number;
    }
#endif
    return(JSTRAP_CONTINUE);
}

static void JWEBCDebugNewScriptHook (JSContext  *cx,
                                    const char *filename,  /* URL of script */
                                    uintN      lineno,     /* first line */
                                    JSScript   *script,
                                    JSFunction *fun,
                                    void       *callerdata)
{
JWEBCDebug *pJWEBCDebug;

    pJWEBCDebug = (JWEBCDebug *) callerdata;
    LOG_JAVASCRIPT("New script JWEBCDebugNewScriptHook called (fun, script) %X %X \n", fun, script);
}


/* Initialize debugging support - called onece by wec_Init */
void wjsdebug_wrapper_init(void);
void wjsdebug_init(void)
{
    wjsdebug_wrapper_init();
}

// ========== End of debugger

/* Function provides a buffer for formatting log messages */
static char parm_buffer[512];
static char *debug_get_parm_buffer(void){    return(&parm_buffer[0]); }

/* Return the html name=XXX attribute of an object */
char *debug_get_object_element_name(JSContext *cx, JSObject *obj)
{
#if (!JSDEBUG_SHOW_OBJECT_ELEMENT_NAMES)
    return("NAME");
#else
    /* Look at it as a window and see if the stored Javascript manager matches the one in context */
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);    /* True for both window and element */
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;          /* True for both window and element */
	WebcJSDocumentContext *jsmgr_if_windows = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj); /* True if a window */
	HTMLElement *pElem = 0;
    HTMLDocument *pDoc = jsmgr? jsmgr->GetDocument() : 0;
    char *text_buffer = debug_get_parm_buffer();

    sprintf(text_buffer, "%s", "UNKNOWN TYPE");

    if (pDoc && jsscr->GetJSMgr() == jsmgr_if_windows)
    {  /* It's a window */
	    HTMLDocument *pDoc = jsmgr? jsmgr->GetDocument() : 0;
	    pDoc = jsmgr? jsmgr->GetDocument() : 0;
        if (pDoc)
        {
            if (pDoc->Name())
            {
#if (WEBC_SUPPORT_UNICODE)
                sprintf(text_buffer, "documentname=%S", pElem->Name());
#else
                sprintf(text_buffer, "documentname=%s", pElem->Name());
#endif
            }
            else
            {
                sprintf(text_buffer, "documentname=NULL", pElem->Name());
            }
            sprintf(text_buffer, "documentname=%s", "NULL");
        }
        else
            sprintf(text_buffer, "documentname=%s", "NODOCUMENTFOUND");
    }
    else
    {
        /* Its an element */
	    HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	    HTMLDocument *pDoc = jsmgr? jsmgr->GetDocument() : 0;
        if (pElem)
        {
            if (pElem->Name())
            {
#if (WEBC_SUPPORT_UNICODE)
                sprintf(text_buffer, "name=%S", pElem->Name());
#else
                sprintf(text_buffer, "name=%s", pElem->Name());
#endif
            }
            else
            {
                sprintf(text_buffer, "name=%s", "NULL");
            }
        }
        else
        {
            sprintf(text_buffer, "name=%s", "NOELEMENT FOUND");
        }
    }
    return(text_buffer);
#endif
}
// ========== End of utils

/*

 Provides a set of wrapper functions that encapsulate all javascript calls to webC.

    Contains wjsdebug_wrapper_init() - Called by wjsdebug_init()
    Contains DEBUGJS_InitClass()     - Called by webc Javascript class registration code.


 Theory -


    The function DEBUGJS_InitClass() is called instead of JS_InitClass() -  DEBUGJS_InitClass() calls JS_InitClass() but
    it replaces the callback functions with callbacks that get control before calling the original code entry points for the classes.

    The wrapper code is passed to JS_InitClass() instead of the original values. When the wrapper code is
    called from jsinterp it calls the original code. The wrapper code can display the calls, arguments and return
    values of every call to every webc javascript class.

    The wrapper functions are named
          debug_constructor()
          debug_addProperty()
          debug_delProperty()
          debug_getProperty()
          debug_setProperty()
          debug_enumerate()
          debug_resolve()
          debug_convert()
          bug_finalize()

    The routine named wjsdebug_init(void) is called from webc_Init(), it sets up the arrays of pointers that are
    needed to take over all classes.

    A set of macros, for example, DECLARE_WRAPPER_ADDPROPERTY(0).. declare all the necessary dispatchers that are required
    to get a unique entry point per cals.

*/

/* Structure needed to  place wrappers around native methods */
struct DEBUGJmethod {
    int                 class_index;/* Which webc JS class by order of registration */
    JSFunctionSpec      *pFs;       /* Original request */
    JSNative            wrap_call;  /* Unigue wrapper for this fn */
};

/* Structure needed to  place wrappers around native classes */
struct DEBUGJclass {
    JSFunctionSpec      *mFs;           /* Pointer to original class local methods */
    JSPropertySpec      *mPs;           /* Pointer to original properties */
    JSNative            mConstructor;   /* Pointer to original constructor function */
    JSClass             mModified;      /* Modified class definition containing with wrappers to (get set, enumerate) etc.*/
    JSClass             *pDeclared;     /* Pointer to original class definition */
    JSFunctionSpec      *pModifiedfs;   /* Pointer to modified JSFunctionSpec records which put wrappers around the class methods */
};

/* Arrays to contain addresses of unique function generated by the preprocessor */
JSPropertyOp        local_addProperty[MAX_DEBUGJCLASSES];
JSPropertyOp        local_delProperty[MAX_DEBUGJCLASSES];
JSPropertyOp        local_getProperty[MAX_DEBUGJCLASSES];
JSPropertyOp        local_setProperty[MAX_DEBUGJCLASSES];
JSEnumerateOp       local_enumerate[MAX_DEBUGJCLASSES];
JSResolveOp         local_resolve[MAX_DEBUGJCLASSES];
JSConvertOp         local_convert[MAX_DEBUGJCLASSES];
JSFinalizeOp        local_finalize[MAX_DEBUGJCLASSES];
JSNative            local_constructor[MAX_DEBUGJCLASSES];
struct DEBUGJmethod local_method[MAX_DEBUGJMETHODS];
struct DEBUGJclass classwrappers[MAX_DEBUGJCLASSES];



/* Use counter for the arrays, when a method is registered it consumes one index in one of the
   arrays. When javascript calls us, the array offset contains the original class and method
   that are monitored and called .*/
int num_wrapped_classes = 0;
int num_wrapped_methods = 0;
static char *unknown_property_string(jsval id); /* Format an unknown property message (tiny ID has no match in class propert table) */
static struct JSPropertySpec *debug_IdToPropertySpec(int class_index, jsval id); /* Given class and tiny ID, retrieve property name */

static void init_classwrappers(void);
void wjsdebug_wrapper_init(void)
{
    num_wrapped_classes = 0;
    num_wrapped_methods = 0;
    init_classwrappers();
}


/*
    Wrapper functions for interface between javascript and a webC object
    These wrappers are called from unique stub functions that call the common
    function providing the unique index used to store information when
    the method was registered.

    When enabled message flow between layers is logged to "jslog.txt".

    The message format is:

    For property accesses the log has the format:

        <Set|Get..>.. <Classname name=elementname>.propertyname=value
        So the log value:
            Set <Image name=animation >.src="images/1.gif"
        Is the same as:
            doument.animation.src="images/1.gif"
        And the log value:
            Get <Image name=animation >.src="images/1.gif"
        Is the same as:
            doument.animation.src == "images/1.gif"

    For native method calls, the log has the format:
            <Exe Classname name=elementname>.methodname()
        So the log value:
            <Exe Input name=myselectelement>.select()
        Is the same as:
            document.myselectelement.select();

    Logging of calls to javascript from webC is not implemeted yet....

    For calls to javascript from webC (onXXXXX events mostly) the log has the format: ...


*/

/* Callback made every time a constructor is invoked */
static JSBool debug_constructor(int class_index, JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
JSBool ret_val;
    LOG_JAVASCRIPT("Calling New %s\n", classwrappers[class_index].pDeclared->name)
    ret_val = classwrappers[class_index].mConstructor(cx, obj,argc, argv, rval);
    return(ret_val);
}
static JSBool debug_addProperty(int class_index, JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
JSBool ret_val;
struct JSPropertySpec *ps = debug_IdToPropertySpec(class_index, id);
char *property_name = ps ? (char *) ps->name : unknown_property_string(id);

    LOG_JAVASCRIPT("Add %s.%s\n", classwrappers[class_index].pDeclared->name, property_name)
    ret_val = classwrappers[class_index].pDeclared->addProperty(cx, obj, id, vp);
    return(ret_val);
}
static JSBool debug_delProperty(int class_index, JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
JSBool ret_val;
struct JSPropertySpec *ps = debug_IdToPropertySpec(class_index, id);
char *property_name = ps ? (char *) ps->name : unknown_property_string(id);

    LOG_JAVASCRIPT("Del %s.%s\n", classwrappers[class_index].pDeclared->name, property_name)
    ret_val = classwrappers[class_index].pDeclared->delProperty(cx, obj, id, vp);
    return(ret_val);
}


static JSBool debug_getProperty(int class_index, JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
JSBool ret_val;
struct JSPropertySpec *ps = debug_IdToPropertySpec(class_index, id);
    LOG_JAVASCRIPT("Get");
    LOG_JAVASCRIPT(" <%s ", classwrappers[class_index].pDeclared->name);
    LOG_JAVASCRIPT("%s ", debug_get_object_element_name(cx, obj));
char *property_name = ps ? (char *) ps->name : unknown_property_string(id);
    LOG_JAVASCRIPT(">.%s", property_name)

    ret_val = classwrappers[class_index].pDeclared->getProperty(cx, obj, id, vp);
    if (!*vp)
    {
        LOG_JAVASCRIPT("=(%s)\n","null");
    }
    else
    {
        if (JSVAL_IS_STRINGABLE(*vp))
        {
            LOG_JAVASCRIPT("=\"%s\"\n", JS_GetStringBytes(JS_ValueToString(cx, *vp)));
        }
// Get <Input name=callcount >.name=callcount="object"

        else if (JSVAL_IS_OBJECT(*vp))
        {
            LOG_JAVASCRIPT("=\"%s\"\n", "object");
        }
        else
        {
            LOG_JAVASCRIPT("=\"%s\"\n", "????");
        }
    }
    return(ret_val);
}

static JSBool debug_setProperty(int class_index, JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
JSBool ret_val;
struct JSPropertySpec *ps = debug_IdToPropertySpec(class_index, id);
char *property_name = ps ? (char *) ps->name : unknown_property_string(id);
char *pvalue = JS_ValueToString(cx, *vp) ? JS_GetStringBytes(JS_ValueToString(cx, *vp)) : 0;

    LOG_JAVASCRIPT("Set");
    LOG_JAVASCRIPT(" <%s ", classwrappers[class_index].pDeclared->name);
    LOG_JAVASCRIPT("%s ", debug_get_object_element_name(cx, obj));
    LOG_JAVASCRIPT(">.%s", property_name)
    ret_val = classwrappers[class_index].pDeclared->setProperty(cx, obj, id, vp);
    if (pvalue)
    {
        LOG_JAVASCRIPT("=\"%s\"\n", pvalue)
    }
    else
    {
        LOG_JAVASCRIPT("=(%s)\n", "NULL" )
    }
    return(ret_val);
}
static JSBool debug_enumerate(int class_index, JSContext *cx, JSObject *obj)
{
JSBool ret_val;
    LOG_JAVASCRIPT("Enum %s\n", classwrappers[class_index].pDeclared->name)
    ret_val = classwrappers[class_index].pDeclared->enumerate(cx, obj);
    return(ret_val);
}
static JSBool debug_resolve(int class_index, JSContext *cx, JSObject *obj, jsval id)
{
JSBool ret_val;
char *property_name = JS_GetStringBytes(JSVAL_TO_STRING(id));

    if (!property_name)
        property_name = unknown_property_string(id);
    LOG_JAVASCRIPT("Resolv %s.%s\n", classwrappers[class_index].pDeclared->name, property_name)
    ret_val = classwrappers[class_index].pDeclared->resolve(cx, obj, id);
    return(ret_val);
}
static JSBool debug_convert(int class_index, JSContext *cx, JSObject *obj, JSType type, jsval *vp)
{
JSBool ret_val;
    ret_val = classwrappers[class_index].pDeclared->convert(cx, obj, type, vp);
    return(ret_val);
}
static void debug_finalize(int class_index, JSContext *cx, JSObject *obj)
{
    classwrappers[class_index].pDeclared->finalize(cx, obj);
}


/*
    Wrapper for when a native method call is requested from javascript to a webC object
    local_method[index] contains the information when the method was registered */

static JSBool debug_method(int method_index, JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
JSBool ret_val;
    LOG_JAVASCRIPT("Exe");
    LOG_JAVASCRIPT(" <%s ", classwrappers[local_method[method_index].class_index].pDeclared->name);
    LOG_JAVASCRIPT("%s ", debug_get_object_element_name(cx, obj));
    LOG_JAVASCRIPT(">.%s()\n", local_method[method_index].pFs->name)
    ret_val = local_method[method_index].pFs->call(cx, obj, argc, argv, rval);
    return(ret_val);
}
/* Format an unknown property message (tiny ID has no match in class propert table) */
static char *unknown_property_string(jsval id)
{
int8 idnumber;
    idnumber = (int8) JSVAL_TO_INT(id);
    sprintf(debug_get_parm_buffer(), "??UNKOWN?? (%d)" , idnumber);
    return(debug_get_parm_buffer());
}

/* Given class and tiny ID, retrieve property name */
static struct JSPropertySpec *debug_IdToPropertySpec(int class_index, jsval id)
{
int8 idnumber;
struct JSPropertySpec *ps;
    idnumber = (int8) JSVAL_TO_INT(id);
    ps = classwrappers[class_index].mPs;
    while (ps->name)
    {
        if (idnumber == ps->tinyid)
            return(ps);
        ps++;
    }
    return(0);
}

/*
    DEBUGJS_InitClass()

    When WEBC_DEBUG_JSCRIPT is enabled the constructors for the webC base JS classes
    call this function instead of JS_InitClass().
        When debugging is disabled DEBUGJS_InitClass() is defined as JS_InitClass().
        When debugging is enabled, this version DEBUGJS_InitClass() is include.

    This debug version calls JS_InitClass() after installing a method to redirect all message
    flow between webC and javascript through a few diagnostic routines that monitor the message
    flow and then pass it along.

    The approach is to generate a unique function name for each possible interface between Javascript
    and webC. These call common routine that monitor message flow.

*/

extern JS_PUBLIC_API(JSObject *)
DEBUGJS_InitClass(JSContext *cx, JSObject *obj, JSObject *parent_proto,
             JSClass *clasp, JSNative constructor, uintN nargs,
             JSPropertySpec *ps, JSFunctionSpec *fs,
             JSPropertySpec *static_ps, JSFunctionSpec *static_fs)
{
    WEBC_ASSERT(num_wrapped_classes < MAX_DEBUGJCLASSES);

    int class_number = num_wrapped_classes;
    num_wrapped_classes += 1;

    /* Take over addproperty et al */
    classwrappers[class_number].pDeclared = clasp;
    classwrappers[class_number].mModified = *clasp;
    classwrappers[class_number].mModified.addProperty = local_addProperty[class_number];
    classwrappers[class_number].mModified.delProperty = local_delProperty[class_number];
    classwrappers[class_number].mModified.getProperty = local_getProperty[class_number];
    classwrappers[class_number].mModified.setProperty = local_setProperty[class_number];
    classwrappers[class_number].mModified.enumerate   = local_enumerate[class_number];
    classwrappers[class_number].mModified.resolve     = local_resolve[class_number];
    classwrappers[class_number].mModified.convert     = local_convert[class_number];
    classwrappers[class_number].mModified.finalize    = local_finalize[class_number];
    classwrappers[class_number].mPs = ps;
    classwrappers[class_number].mFs = fs;
    /* Now take over constructor too */
    classwrappers[class_number].mConstructor         = constructor;
    /* Now take over methods */
    {
        JSFunctionSpec      *fromFs, *toFs;
        /* Count all the methods in this class */
        int n_methods = 0;
        {
            JSFunctionSpec      *lFs;
            lFs = fs;
            while (lFs->name)
            {
                n_methods++;
                lFs++;
            }
        }

        /* Allocate a new methods list to pass to JS_ClassInit */
        classwrappers[class_number].pModifiedfs = (JSFunctionSpec *) WEBC_MALLOC(sizeof(JSFunctionSpec) * (n_methods+1));
        rtp_memset(classwrappers[class_number].pModifiedfs,0,(sizeof(JSFunctionSpec)*(n_methods+1)));

        toFs = classwrappers[class_number].pModifiedfs;
        fromFs = fs;
        /* Now copy method function specs to private copy, but replace the call with the wrapper */
        while (fromFs->name)
        {
            WEBC_ASSERT(num_wrapped_methods < MAX_DEBUGJMETHODS);
            /* Copy the original method */
            *toFs = *fromFs;
            /* Save a pointer to the original method so we can call it */
            local_method[num_wrapped_methods].pFs = fromFs;
            /* Set the wrap_call entry as the entry point for the method in the copy of the method table */
            toFs->call = local_method[num_wrapped_methods].wrap_call;
            /* Save the class number */
            local_method[num_wrapped_methods].class_index = class_number;
            num_wrapped_methods++;
            fromFs++;
            toFs++;
        }
    }
    /* Now initialze with wrapper */
    return(JS_InitClass(cx, obj, parent_proto,
            &classwrappers[class_number].mModified, local_constructor[class_number], nargs,
             ps, classwrappers[class_number].pModifiedfs, static_ps, static_fs));
}

/*
      Generate lots of unique function entry points that all converge to the same one, passing in an index
      value along with the other parameters

      Generate code:
        static JSBool debug_addProperty0(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
        { return(debug_addProperty(0, cx, obj, id, vp)  );
        static JSBool debug_addProperty1(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
        { return(debug_addProperty(1, cx, obj, id, vp)  );
        etc..
*/

#define DECLARE_WRAPPER_ADDPROPERTY(A) static JSBool debug_addProperty##A(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {return(debug_addProperty(A, cx, obj, id, vp));}
DECLARE_WRAPPER_ADDPROPERTY(0)
DECLARE_WRAPPER_ADDPROPERTY(1)
DECLARE_WRAPPER_ADDPROPERTY(2)
DECLARE_WRAPPER_ADDPROPERTY(3)
DECLARE_WRAPPER_ADDPROPERTY(4)
DECLARE_WRAPPER_ADDPROPERTY(5)
DECLARE_WRAPPER_ADDPROPERTY(6)
DECLARE_WRAPPER_ADDPROPERTY(7)
DECLARE_WRAPPER_ADDPROPERTY(8)
DECLARE_WRAPPER_ADDPROPERTY(9)
DECLARE_WRAPPER_ADDPROPERTY(10)
DECLARE_WRAPPER_ADDPROPERTY(11)
DECLARE_WRAPPER_ADDPROPERTY(12)
DECLARE_WRAPPER_ADDPROPERTY(13)
DECLARE_WRAPPER_ADDPROPERTY(14)
DECLARE_WRAPPER_ADDPROPERTY(15)
DECLARE_WRAPPER_ADDPROPERTY(16)
DECLARE_WRAPPER_ADDPROPERTY(17)
DECLARE_WRAPPER_ADDPROPERTY(18)
DECLARE_WRAPPER_ADDPROPERTY(19)
DECLARE_WRAPPER_ADDPROPERTY(20)
DECLARE_WRAPPER_ADDPROPERTY(21)
DECLARE_WRAPPER_ADDPROPERTY(22)
DECLARE_WRAPPER_ADDPROPERTY(23)
DECLARE_WRAPPER_ADDPROPERTY(24)
DECLARE_WRAPPER_ADDPROPERTY(25)
DECLARE_WRAPPER_ADDPROPERTY(26)
DECLARE_WRAPPER_ADDPROPERTY(27)
DECLARE_WRAPPER_ADDPROPERTY(28)
DECLARE_WRAPPER_ADDPROPERTY(29)
#define DECLARE_WRAPPER_DELPROPERTY(A) static JSBool debug_delProperty##A(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {return(debug_delProperty(A, cx, obj, id, vp));}
DECLARE_WRAPPER_DELPROPERTY(0)
DECLARE_WRAPPER_DELPROPERTY(1)
DECLARE_WRAPPER_DELPROPERTY(2)
DECLARE_WRAPPER_DELPROPERTY(3)
DECLARE_WRAPPER_DELPROPERTY(4)
DECLARE_WRAPPER_DELPROPERTY(5)
DECLARE_WRAPPER_DELPROPERTY(6)
DECLARE_WRAPPER_DELPROPERTY(7)
DECLARE_WRAPPER_DELPROPERTY(8)
DECLARE_WRAPPER_DELPROPERTY(9)
DECLARE_WRAPPER_DELPROPERTY(10)
DECLARE_WRAPPER_DELPROPERTY(11)
DECLARE_WRAPPER_DELPROPERTY(12)
DECLARE_WRAPPER_DELPROPERTY(13)
DECLARE_WRAPPER_DELPROPERTY(14)
DECLARE_WRAPPER_DELPROPERTY(15)
DECLARE_WRAPPER_DELPROPERTY(16)
DECLARE_WRAPPER_DELPROPERTY(17)
DECLARE_WRAPPER_DELPROPERTY(18)
DECLARE_WRAPPER_DELPROPERTY(19)
DECLARE_WRAPPER_DELPROPERTY(20)
DECLARE_WRAPPER_DELPROPERTY(21)
DECLARE_WRAPPER_DELPROPERTY(22)
DECLARE_WRAPPER_DELPROPERTY(23)
DECLARE_WRAPPER_DELPROPERTY(24)
DECLARE_WRAPPER_DELPROPERTY(25)
DECLARE_WRAPPER_DELPROPERTY(26)
DECLARE_WRAPPER_DELPROPERTY(27)
DECLARE_WRAPPER_DELPROPERTY(28)
DECLARE_WRAPPER_DELPROPERTY(29)
#define DECLARE_WRAPPER_GETPROPERTY(A) static JSBool debug_getProperty##A(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {return(debug_getProperty(A, cx, obj, id, vp));}
DECLARE_WRAPPER_GETPROPERTY(0)
DECLARE_WRAPPER_GETPROPERTY(1)
DECLARE_WRAPPER_GETPROPERTY(2)
DECLARE_WRAPPER_GETPROPERTY(3)
DECLARE_WRAPPER_GETPROPERTY(4)
DECLARE_WRAPPER_GETPROPERTY(5)
DECLARE_WRAPPER_GETPROPERTY(6)
DECLARE_WRAPPER_GETPROPERTY(7)
DECLARE_WRAPPER_GETPROPERTY(8)
DECLARE_WRAPPER_GETPROPERTY(9)
DECLARE_WRAPPER_GETPROPERTY(10)
DECLARE_WRAPPER_GETPROPERTY(11)
DECLARE_WRAPPER_GETPROPERTY(12)
DECLARE_WRAPPER_GETPROPERTY(13)
DECLARE_WRAPPER_GETPROPERTY(14)
DECLARE_WRAPPER_GETPROPERTY(15)
DECLARE_WRAPPER_GETPROPERTY(16)
DECLARE_WRAPPER_GETPROPERTY(17)
DECLARE_WRAPPER_GETPROPERTY(18)
DECLARE_WRAPPER_GETPROPERTY(19)
DECLARE_WRAPPER_GETPROPERTY(20)
DECLARE_WRAPPER_GETPROPERTY(21)
DECLARE_WRAPPER_GETPROPERTY(22)
DECLARE_WRAPPER_GETPROPERTY(23)
DECLARE_WRAPPER_GETPROPERTY(24)
DECLARE_WRAPPER_GETPROPERTY(25)
DECLARE_WRAPPER_GETPROPERTY(26)
DECLARE_WRAPPER_GETPROPERTY(27)
DECLARE_WRAPPER_GETPROPERTY(28)
DECLARE_WRAPPER_GETPROPERTY(29)
#define DECLARE_WRAPPER_SETPROPERTY(A) static JSBool debug_setProperty##A(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {return(debug_setProperty(A, cx, obj, id, vp));}
DECLARE_WRAPPER_SETPROPERTY(0)
DECLARE_WRAPPER_SETPROPERTY(1)
DECLARE_WRAPPER_SETPROPERTY(2)
DECLARE_WRAPPER_SETPROPERTY(3)
DECLARE_WRAPPER_SETPROPERTY(4)
DECLARE_WRAPPER_SETPROPERTY(5)
DECLARE_WRAPPER_SETPROPERTY(6)
DECLARE_WRAPPER_SETPROPERTY(7)
DECLARE_WRAPPER_SETPROPERTY(8)
DECLARE_WRAPPER_SETPROPERTY(9)
DECLARE_WRAPPER_SETPROPERTY(10)
DECLARE_WRAPPER_SETPROPERTY(11)
DECLARE_WRAPPER_SETPROPERTY(12)
DECLARE_WRAPPER_SETPROPERTY(13)
DECLARE_WRAPPER_SETPROPERTY(14)
DECLARE_WRAPPER_SETPROPERTY(15)
DECLARE_WRAPPER_SETPROPERTY(16)
DECLARE_WRAPPER_SETPROPERTY(17)
DECLARE_WRAPPER_SETPROPERTY(18)
DECLARE_WRAPPER_SETPROPERTY(19)
DECLARE_WRAPPER_SETPROPERTY(20)
DECLARE_WRAPPER_SETPROPERTY(21)
DECLARE_WRAPPER_SETPROPERTY(22)
DECLARE_WRAPPER_SETPROPERTY(23)
DECLARE_WRAPPER_SETPROPERTY(24)
DECLARE_WRAPPER_SETPROPERTY(25)
DECLARE_WRAPPER_SETPROPERTY(26)
DECLARE_WRAPPER_SETPROPERTY(27)
DECLARE_WRAPPER_SETPROPERTY(28)
DECLARE_WRAPPER_SETPROPERTY(29)
#define DECLARE_WRAPPER_ENUMERATE(A) static JSBool debug_enumerate##A(JSContext *cx, JSObject *obj) {return(debug_enumerate(A, cx, obj));}
DECLARE_WRAPPER_ENUMERATE(0)
DECLARE_WRAPPER_ENUMERATE(1)
DECLARE_WRAPPER_ENUMERATE(2)
DECLARE_WRAPPER_ENUMERATE(3)
DECLARE_WRAPPER_ENUMERATE(4)
DECLARE_WRAPPER_ENUMERATE(5)
DECLARE_WRAPPER_ENUMERATE(6)
DECLARE_WRAPPER_ENUMERATE(7)
DECLARE_WRAPPER_ENUMERATE(8)
DECLARE_WRAPPER_ENUMERATE(9)
DECLARE_WRAPPER_ENUMERATE(10)
DECLARE_WRAPPER_ENUMERATE(11)
DECLARE_WRAPPER_ENUMERATE(12)
DECLARE_WRAPPER_ENUMERATE(13)
DECLARE_WRAPPER_ENUMERATE(14)
DECLARE_WRAPPER_ENUMERATE(15)
DECLARE_WRAPPER_ENUMERATE(16)
DECLARE_WRAPPER_ENUMERATE(17)
DECLARE_WRAPPER_ENUMERATE(18)
DECLARE_WRAPPER_ENUMERATE(19)
DECLARE_WRAPPER_ENUMERATE(20)
DECLARE_WRAPPER_ENUMERATE(21)
DECLARE_WRAPPER_ENUMERATE(22)
DECLARE_WRAPPER_ENUMERATE(23)
DECLARE_WRAPPER_ENUMERATE(24)
DECLARE_WRAPPER_ENUMERATE(25)
DECLARE_WRAPPER_ENUMERATE(26)
DECLARE_WRAPPER_ENUMERATE(27)
DECLARE_WRAPPER_ENUMERATE(28)
DECLARE_WRAPPER_ENUMERATE(29)
#define DECLARE_WRAPPER_RESOLVE(A) static JSBool debug_resolve##A(JSContext *cx, JSObject *obj, jsval id) {return(debug_resolve(A, cx, obj, id));}
DECLARE_WRAPPER_RESOLVE(0)
DECLARE_WRAPPER_RESOLVE(1)
DECLARE_WRAPPER_RESOLVE(2)
DECLARE_WRAPPER_RESOLVE(3)
DECLARE_WRAPPER_RESOLVE(4)
DECLARE_WRAPPER_RESOLVE(5)
DECLARE_WRAPPER_RESOLVE(6)
DECLARE_WRAPPER_RESOLVE(7)
DECLARE_WRAPPER_RESOLVE(8)
DECLARE_WRAPPER_RESOLVE(9)
DECLARE_WRAPPER_RESOLVE(10)
DECLARE_WRAPPER_RESOLVE(11)
DECLARE_WRAPPER_RESOLVE(12)
DECLARE_WRAPPER_RESOLVE(13)
DECLARE_WRAPPER_RESOLVE(14)
DECLARE_WRAPPER_RESOLVE(15)
DECLARE_WRAPPER_RESOLVE(16)
DECLARE_WRAPPER_RESOLVE(17)
DECLARE_WRAPPER_RESOLVE(18)
DECLARE_WRAPPER_RESOLVE(19)
DECLARE_WRAPPER_RESOLVE(20)
DECLARE_WRAPPER_RESOLVE(21)
DECLARE_WRAPPER_RESOLVE(22)
DECLARE_WRAPPER_RESOLVE(23)
DECLARE_WRAPPER_RESOLVE(24)
DECLARE_WRAPPER_RESOLVE(25)
DECLARE_WRAPPER_RESOLVE(26)
DECLARE_WRAPPER_RESOLVE(27)
DECLARE_WRAPPER_RESOLVE(28)
DECLARE_WRAPPER_RESOLVE(29)
#define DECLARE_WRAPPER_CONVERT(A) static JSBool debug_convert##A(JSContext *cx, JSObject *obj, JSType type, jsval *vp) {return(debug_convert(A, cx, obj, type, vp));}
DECLARE_WRAPPER_CONVERT(0)
DECLARE_WRAPPER_CONVERT(1)
DECLARE_WRAPPER_CONVERT(2)
DECLARE_WRAPPER_CONVERT(3)
DECLARE_WRAPPER_CONVERT(4)
DECLARE_WRAPPER_CONVERT(5)
DECLARE_WRAPPER_CONVERT(6)
DECLARE_WRAPPER_CONVERT(7)
DECLARE_WRAPPER_CONVERT(8)
DECLARE_WRAPPER_CONVERT(9)
DECLARE_WRAPPER_CONVERT(10)
DECLARE_WRAPPER_CONVERT(11)
DECLARE_WRAPPER_CONVERT(12)
DECLARE_WRAPPER_CONVERT(13)
DECLARE_WRAPPER_CONVERT(14)
DECLARE_WRAPPER_CONVERT(15)
DECLARE_WRAPPER_CONVERT(16)
DECLARE_WRAPPER_CONVERT(17)
DECLARE_WRAPPER_CONVERT(18)
DECLARE_WRAPPER_CONVERT(19)
DECLARE_WRAPPER_CONVERT(20)
DECLARE_WRAPPER_CONVERT(21)
DECLARE_WRAPPER_CONVERT(22)
DECLARE_WRAPPER_CONVERT(23)
DECLARE_WRAPPER_CONVERT(24)
DECLARE_WRAPPER_CONVERT(25)
DECLARE_WRAPPER_CONVERT(26)
DECLARE_WRAPPER_CONVERT(27)
DECLARE_WRAPPER_CONVERT(28)
DECLARE_WRAPPER_CONVERT(29)

#define DECLARE_WRAPPER_FINALIZE(A) static void debug_finalize##A(JSContext *cx, JSObject *obj) { debug_finalize(A, cx, obj);}
DECLARE_WRAPPER_FINALIZE(0)
DECLARE_WRAPPER_FINALIZE(1)
DECLARE_WRAPPER_FINALIZE(2)
DECLARE_WRAPPER_FINALIZE(3)
DECLARE_WRAPPER_FINALIZE(4)
DECLARE_WRAPPER_FINALIZE(5)
DECLARE_WRAPPER_FINALIZE(6)
DECLARE_WRAPPER_FINALIZE(7)
DECLARE_WRAPPER_FINALIZE(8)
DECLARE_WRAPPER_FINALIZE(9)
DECLARE_WRAPPER_FINALIZE(10)
DECLARE_WRAPPER_FINALIZE(11)
DECLARE_WRAPPER_FINALIZE(12)
DECLARE_WRAPPER_FINALIZE(13)
DECLARE_WRAPPER_FINALIZE(14)
DECLARE_WRAPPER_FINALIZE(15)
DECLARE_WRAPPER_FINALIZE(16)
DECLARE_WRAPPER_FINALIZE(17)
DECLARE_WRAPPER_FINALIZE(18)
DECLARE_WRAPPER_FINALIZE(19)
DECLARE_WRAPPER_FINALIZE(20)
DECLARE_WRAPPER_FINALIZE(21)
DECLARE_WRAPPER_FINALIZE(22)
DECLARE_WRAPPER_FINALIZE(23)
DECLARE_WRAPPER_FINALIZE(24)
DECLARE_WRAPPER_FINALIZE(25)
DECLARE_WRAPPER_FINALIZE(26)
DECLARE_WRAPPER_FINALIZE(27)
DECLARE_WRAPPER_FINALIZE(28)
DECLARE_WRAPPER_FINALIZE(29)

#define DECLARE_WRAPPER_CONSTRUCTOR(A) static JSBool debug_constructor##A(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {return(debug_constructor((A), cx, obj, argc, argv, rval));}
DECLARE_WRAPPER_CONSTRUCTOR(0)
DECLARE_WRAPPER_CONSTRUCTOR(1)
DECLARE_WRAPPER_CONSTRUCTOR(2)
DECLARE_WRAPPER_CONSTRUCTOR(3)
DECLARE_WRAPPER_CONSTRUCTOR(4)
DECLARE_WRAPPER_CONSTRUCTOR(5)
DECLARE_WRAPPER_CONSTRUCTOR(6)
DECLARE_WRAPPER_CONSTRUCTOR(7)
DECLARE_WRAPPER_CONSTRUCTOR(8)
DECLARE_WRAPPER_CONSTRUCTOR(9)
DECLARE_WRAPPER_CONSTRUCTOR(10)
DECLARE_WRAPPER_CONSTRUCTOR(11)
DECLARE_WRAPPER_CONSTRUCTOR(12)
DECLARE_WRAPPER_CONSTRUCTOR(13)
DECLARE_WRAPPER_CONSTRUCTOR(14)
DECLARE_WRAPPER_CONSTRUCTOR(15)
DECLARE_WRAPPER_CONSTRUCTOR(16)
DECLARE_WRAPPER_CONSTRUCTOR(17)
DECLARE_WRAPPER_CONSTRUCTOR(18)
DECLARE_WRAPPER_CONSTRUCTOR(19)
DECLARE_WRAPPER_CONSTRUCTOR(20)
DECLARE_WRAPPER_CONSTRUCTOR(21)
DECLARE_WRAPPER_CONSTRUCTOR(22)
DECLARE_WRAPPER_CONSTRUCTOR(23)
DECLARE_WRAPPER_CONSTRUCTOR(24)
DECLARE_WRAPPER_CONSTRUCTOR(25)
DECLARE_WRAPPER_CONSTRUCTOR(26)
DECLARE_WRAPPER_CONSTRUCTOR(27)
DECLARE_WRAPPER_CONSTRUCTOR(28)
DECLARE_WRAPPER_CONSTRUCTOR(29)



#define DECLARE_WRAPPER_METHOD(A) static JSBool debug_method##A(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {return(debug_method((A), cx, obj, argc, argv, rval));}

DECLARE_WRAPPER_METHOD( 0)
DECLARE_WRAPPER_METHOD( 1)
DECLARE_WRAPPER_METHOD( 2)
DECLARE_WRAPPER_METHOD( 3)
DECLARE_WRAPPER_METHOD( 4)
DECLARE_WRAPPER_METHOD( 5)
DECLARE_WRAPPER_METHOD( 6)
DECLARE_WRAPPER_METHOD( 7)
DECLARE_WRAPPER_METHOD( 8)
DECLARE_WRAPPER_METHOD( 9)
DECLARE_WRAPPER_METHOD(10)
DECLARE_WRAPPER_METHOD(11)
DECLARE_WRAPPER_METHOD(12)
DECLARE_WRAPPER_METHOD(13)
DECLARE_WRAPPER_METHOD(14)
DECLARE_WRAPPER_METHOD(15)
DECLARE_WRAPPER_METHOD(16)
DECLARE_WRAPPER_METHOD(17)
DECLARE_WRAPPER_METHOD(18)
DECLARE_WRAPPER_METHOD(19)
DECLARE_WRAPPER_METHOD(20)
DECLARE_WRAPPER_METHOD(21)
DECLARE_WRAPPER_METHOD(22)
DECLARE_WRAPPER_METHOD(23)
DECLARE_WRAPPER_METHOD(24)
DECLARE_WRAPPER_METHOD(25)
DECLARE_WRAPPER_METHOD(26)
DECLARE_WRAPPER_METHOD(27)
DECLARE_WRAPPER_METHOD(28)
DECLARE_WRAPPER_METHOD(29)
DECLARE_WRAPPER_METHOD(30)
DECLARE_WRAPPER_METHOD(31)
DECLARE_WRAPPER_METHOD(32)
DECLARE_WRAPPER_METHOD(33)
DECLARE_WRAPPER_METHOD(34)
DECLARE_WRAPPER_METHOD(35)
DECLARE_WRAPPER_METHOD(36)
DECLARE_WRAPPER_METHOD(37)
DECLARE_WRAPPER_METHOD(38)
DECLARE_WRAPPER_METHOD(39)
DECLARE_WRAPPER_METHOD(40)
DECLARE_WRAPPER_METHOD(41)
DECLARE_WRAPPER_METHOD(42)
DECLARE_WRAPPER_METHOD(43)
DECLARE_WRAPPER_METHOD(44)
DECLARE_WRAPPER_METHOD(45)
DECLARE_WRAPPER_METHOD(46)
DECLARE_WRAPPER_METHOD(47)
DECLARE_WRAPPER_METHOD(48)
DECLARE_WRAPPER_METHOD(49)
DECLARE_WRAPPER_METHOD(50)
DECLARE_WRAPPER_METHOD(51)
DECLARE_WRAPPER_METHOD(52)
DECLARE_WRAPPER_METHOD(53)
DECLARE_WRAPPER_METHOD(54)
DECLARE_WRAPPER_METHOD(55)
DECLARE_WRAPPER_METHOD(56)
DECLARE_WRAPPER_METHOD(57)
DECLARE_WRAPPER_METHOD(58)
DECLARE_WRAPPER_METHOD(59)
DECLARE_WRAPPER_METHOD(60)
DECLARE_WRAPPER_METHOD(61)
DECLARE_WRAPPER_METHOD(62)
DECLARE_WRAPPER_METHOD(63)
DECLARE_WRAPPER_METHOD(64)
DECLARE_WRAPPER_METHOD(65)
DECLARE_WRAPPER_METHOD(66)
DECLARE_WRAPPER_METHOD(67)
DECLARE_WRAPPER_METHOD(68)
DECLARE_WRAPPER_METHOD(69)
DECLARE_WRAPPER_METHOD(70)
DECLARE_WRAPPER_METHOD(71)
DECLARE_WRAPPER_METHOD(72)
DECLARE_WRAPPER_METHOD(73)
DECLARE_WRAPPER_METHOD(74)
DECLARE_WRAPPER_METHOD(75)
DECLARE_WRAPPER_METHOD(76)
DECLARE_WRAPPER_METHOD(77)
DECLARE_WRAPPER_METHOD(78)
DECLARE_WRAPPER_METHOD(79)
DECLARE_WRAPPER_METHOD(80)
DECLARE_WRAPPER_METHOD(81)
DECLARE_WRAPPER_METHOD(82)
DECLARE_WRAPPER_METHOD(83)
DECLARE_WRAPPER_METHOD(84)
DECLARE_WRAPPER_METHOD(85)
DECLARE_WRAPPER_METHOD(86)
DECLARE_WRAPPER_METHOD(87)
DECLARE_WRAPPER_METHOD(88)
DECLARE_WRAPPER_METHOD(89)
DECLARE_WRAPPER_METHOD(90)
DECLARE_WRAPPER_METHOD(91)
DECLARE_WRAPPER_METHOD(92)
DECLARE_WRAPPER_METHOD(93)
DECLARE_WRAPPER_METHOD(94)
DECLARE_WRAPPER_METHOD(95)
DECLARE_WRAPPER_METHOD(96)
DECLARE_WRAPPER_METHOD(97)
DECLARE_WRAPPER_METHOD(98)
DECLARE_WRAPPER_METHOD(99)
DECLARE_WRAPPER_METHOD(100)
DECLARE_WRAPPER_METHOD(101)
DECLARE_WRAPPER_METHOD(102)
DECLARE_WRAPPER_METHOD(103)
DECLARE_WRAPPER_METHOD(104)
DECLARE_WRAPPER_METHOD(105)
DECLARE_WRAPPER_METHOD(106)
DECLARE_WRAPPER_METHOD(107)
DECLARE_WRAPPER_METHOD(108)
DECLARE_WRAPPER_METHOD(109)
DECLARE_WRAPPER_METHOD(110)
DECLARE_WRAPPER_METHOD(111)
DECLARE_WRAPPER_METHOD(112)
DECLARE_WRAPPER_METHOD(113)
DECLARE_WRAPPER_METHOD(114)
DECLARE_WRAPPER_METHOD(115)
DECLARE_WRAPPER_METHOD(116)
DECLARE_WRAPPER_METHOD(117)
DECLARE_WRAPPER_METHOD(118)
DECLARE_WRAPPER_METHOD(119)
DECLARE_WRAPPER_METHOD(120)

/* This routine must be called once,
    This functions loads the addresses of MAX_DEBUGJCLASSES wrapper functions into arrarys of function pointers
    The functions all have unique names
    They generate a unique index number (one for each class)
    For each class the same method is called but with the class number prepended to the parameter list
    The common function prints object information and then routes it to the original method
*/

static void init_classwrappers(void)
{
    /* Generate code:
        local_addProperty[0] = debug_addProperty0;
        local_addProperty[1] = debug_addProperty1;
        local_addProperty[2] = debug_addProperty2;
    etc */
#define LOAD_addProperty(INDEX) local_addProperty[##INDEX] = debug_addProperty##INDEX
    LOAD_addProperty(0);
    LOAD_addProperty(1);
    LOAD_addProperty(2);
    LOAD_addProperty(3);
    LOAD_addProperty(4);
    LOAD_addProperty(5);
    LOAD_addProperty(6);
    LOAD_addProperty(7);
    LOAD_addProperty(8);
    LOAD_addProperty(9);
    LOAD_addProperty(10);
    LOAD_addProperty(11);
    LOAD_addProperty(12);
    LOAD_addProperty(13);
    LOAD_addProperty(14);
    LOAD_addProperty(15);
    LOAD_addProperty(16);
    LOAD_addProperty(17);
    LOAD_addProperty(18);
    LOAD_addProperty(19);
    LOAD_addProperty(20);
    LOAD_addProperty(21);
    LOAD_addProperty(22);
    LOAD_addProperty(23);
    LOAD_addProperty(24);
    LOAD_addProperty(25);
    LOAD_addProperty(26);
    LOAD_addProperty(27);
    LOAD_addProperty(28);
    LOAD_addProperty(29);
#define LOAD_getProperty(INDEX) local_getProperty[##INDEX] = debug_getProperty##INDEX
    LOAD_getProperty(0);
    LOAD_getProperty(1);
    LOAD_getProperty(2);
    LOAD_getProperty(3);
    LOAD_getProperty(4);
    LOAD_getProperty(5);
    LOAD_getProperty(6);
    LOAD_getProperty(7);
    LOAD_getProperty(8);
    LOAD_getProperty(9);
    LOAD_getProperty(10);
    LOAD_getProperty(11);
    LOAD_getProperty(12);
    LOAD_getProperty(13);
    LOAD_getProperty(14);
    LOAD_getProperty(15);
    LOAD_getProperty(16);
    LOAD_getProperty(17);
    LOAD_getProperty(18);
    LOAD_getProperty(19);
    LOAD_getProperty(20);
    LOAD_getProperty(21);
    LOAD_getProperty(22);
    LOAD_getProperty(23);
    LOAD_getProperty(24);
    LOAD_getProperty(25);
    LOAD_getProperty(26);
    LOAD_getProperty(27);
    LOAD_getProperty(28);
    LOAD_getProperty(29);
#define LOAD_setProperty(INDEX) local_setProperty[##INDEX] = debug_setProperty##INDEX
    LOAD_setProperty(0);
    LOAD_setProperty(1);
    LOAD_setProperty(2);
    LOAD_setProperty(3);
    LOAD_setProperty(4);
    LOAD_setProperty(5);
    LOAD_setProperty(6);
    LOAD_setProperty(7);
    LOAD_setProperty(8);
    LOAD_setProperty(9);
    LOAD_setProperty(10);
    LOAD_setProperty(11);
    LOAD_setProperty(12);
    LOAD_setProperty(13);
    LOAD_setProperty(14);
    LOAD_setProperty(15);
    LOAD_setProperty(16);
    LOAD_setProperty(17);
    LOAD_setProperty(18);
    LOAD_setProperty(19);
    LOAD_setProperty(20);
    LOAD_setProperty(21);
    LOAD_setProperty(22);
    LOAD_setProperty(23);
    LOAD_setProperty(24);
    LOAD_setProperty(25);
    LOAD_setProperty(26);
    LOAD_setProperty(27);
    LOAD_setProperty(28);
    LOAD_setProperty(29);
#define LOAD_enumerate(INDEX) local_enumerate[##INDEX] = debug_enumerate##INDEX
    LOAD_enumerate(0);
    LOAD_enumerate(1);
    LOAD_enumerate(2);
    LOAD_enumerate(3);
    LOAD_enumerate(4);
    LOAD_enumerate(5);
    LOAD_enumerate(6);
    LOAD_enumerate(7);
    LOAD_enumerate(8);
    LOAD_enumerate(9);
    LOAD_enumerate(10);
    LOAD_enumerate(11);
    LOAD_enumerate(12);
    LOAD_enumerate(13);
    LOAD_enumerate(14);
    LOAD_enumerate(15);
    LOAD_enumerate(16);
    LOAD_enumerate(17);
    LOAD_enumerate(18);
    LOAD_enumerate(19);
    LOAD_enumerate(20);
    LOAD_enumerate(21);
    LOAD_enumerate(22);
    LOAD_enumerate(23);
    LOAD_enumerate(24);
    LOAD_enumerate(25);
    LOAD_enumerate(26);
    LOAD_enumerate(27);
    LOAD_enumerate(28);
    LOAD_enumerate(29);
#define LOAD_resolve(INDEX) local_resolve[##INDEX] = debug_resolve##INDEX
    LOAD_resolve(0);
    LOAD_resolve(1);
    LOAD_resolve(2);
    LOAD_resolve(3);
    LOAD_resolve(4);
    LOAD_resolve(5);
    LOAD_resolve(6);
    LOAD_resolve(7);
    LOAD_resolve(8);
    LOAD_resolve(9);
    LOAD_resolve(10);
    LOAD_resolve(11);
    LOAD_resolve(12);
    LOAD_resolve(13);
    LOAD_resolve(14);
    LOAD_resolve(15);
    LOAD_resolve(16);
    LOAD_resolve(17);
    LOAD_resolve(18);
    LOAD_resolve(19);
    LOAD_resolve(20);
    LOAD_resolve(21);
    LOAD_resolve(22);
    LOAD_resolve(23);
    LOAD_resolve(24);
    LOAD_resolve(25);
    LOAD_resolve(26);
    LOAD_resolve(27);
    LOAD_resolve(28);
    LOAD_resolve(29);
#define LOAD_convert(INDEX) local_convert[##INDEX] = debug_convert##INDEX
    LOAD_convert(0);
    LOAD_convert(1);
    LOAD_convert(2);
    LOAD_convert(3);
    LOAD_convert(4);
    LOAD_convert(5);
    LOAD_convert(6);
    LOAD_convert(7);
    LOAD_convert(8);
    LOAD_convert(9);
    LOAD_convert(10);
    LOAD_convert(11);
    LOAD_convert(12);
    LOAD_convert(13);
    LOAD_convert(14);
    LOAD_convert(15);
    LOAD_convert(16);
    LOAD_convert(17);
    LOAD_convert(18);
    LOAD_convert(19);
    LOAD_convert(20);
    LOAD_convert(21);
    LOAD_convert(22);
    LOAD_convert(23);
    LOAD_convert(24);
    LOAD_convert(25);
    LOAD_convert(26);
    LOAD_convert(27);
    LOAD_convert(28);
    LOAD_convert(29);
#define LOAD_finalize(INDEX) local_finalize[##INDEX] = debug_finalize##INDEX
    LOAD_finalize(0);
    LOAD_finalize(1);
    LOAD_finalize(2);
    LOAD_finalize(3);
    LOAD_finalize(4);
    LOAD_finalize(5);
    LOAD_finalize(6);
    LOAD_finalize(7);
    LOAD_finalize(8);
    LOAD_finalize(9);
    LOAD_finalize(10);
    LOAD_finalize(11);
    LOAD_finalize(12);
    LOAD_finalize(13);
    LOAD_finalize(14);
    LOAD_finalize(15);
    LOAD_finalize(16);
    LOAD_finalize(17);
    LOAD_finalize(18);
    LOAD_finalize(19);
    LOAD_finalize(20);
    LOAD_finalize(21);
    LOAD_finalize(22);
    LOAD_finalize(23);
    LOAD_finalize(24);
    LOAD_finalize(25);
    LOAD_finalize(26);
    LOAD_finalize(27);
    LOAD_finalize(28);
    LOAD_finalize(29);
#define LOAD_constructor(INDEX) local_constructor[##INDEX] = debug_constructor##INDEX
    LOAD_constructor(0);
    LOAD_constructor(1);
    LOAD_constructor(2);
    LOAD_constructor(3);
    LOAD_constructor(4);
    LOAD_constructor(5);
    LOAD_constructor(6);
    LOAD_constructor(7);
    LOAD_constructor(8);
    LOAD_constructor(9);
    LOAD_constructor(10);
    LOAD_constructor(11);
    LOAD_constructor(12);
    LOAD_constructor(13);
    LOAD_constructor(14);
    LOAD_constructor(15);
    LOAD_constructor(16);
    LOAD_constructor(17);
    LOAD_constructor(18);
    LOAD_constructor(19);
    LOAD_constructor(20);
    LOAD_constructor(21);
    LOAD_constructor(22);
    LOAD_constructor(23);
    LOAD_constructor(24);
    LOAD_constructor(25);
    LOAD_constructor(26);
    LOAD_constructor(27);
    LOAD_constructor(28);
    LOAD_constructor(29);
#define LOAD_METHOD(INDEX) local_method[##INDEX].wrap_call = debug_method##INDEX
    LOAD_METHOD( 0);
    LOAD_METHOD( 1);
    LOAD_METHOD( 2);
    LOAD_METHOD( 3);
    LOAD_METHOD( 4);
    LOAD_METHOD( 5);
    LOAD_METHOD( 6);
    LOAD_METHOD( 7);
    LOAD_METHOD( 8);
    LOAD_METHOD( 9);
    LOAD_METHOD(10);
    LOAD_METHOD(11);
    LOAD_METHOD(12);
    LOAD_METHOD(13);
    LOAD_METHOD(14);
    LOAD_METHOD(15);
    LOAD_METHOD(16);
    LOAD_METHOD(17);
    LOAD_METHOD(18);
    LOAD_METHOD(19);
    LOAD_METHOD(20);
    LOAD_METHOD(21);
    LOAD_METHOD(22);
    LOAD_METHOD(23);
    LOAD_METHOD(24);
    LOAD_METHOD(25);
    LOAD_METHOD(26);
    LOAD_METHOD(27);
    LOAD_METHOD(28);
    LOAD_METHOD(29);
    LOAD_METHOD(30);
    LOAD_METHOD(31);
    LOAD_METHOD(32);
    LOAD_METHOD(33);
    LOAD_METHOD(34);
    LOAD_METHOD(35);
    LOAD_METHOD(36);
    LOAD_METHOD(37);
    LOAD_METHOD(38);
    LOAD_METHOD(39);
    LOAD_METHOD(40);
    LOAD_METHOD(41);
    LOAD_METHOD(42);
    LOAD_METHOD(43);
    LOAD_METHOD(44);
    LOAD_METHOD(45);
    LOAD_METHOD(46);
    LOAD_METHOD(47);
    LOAD_METHOD(48);
    LOAD_METHOD(49);
    LOAD_METHOD(50);
    LOAD_METHOD(51);
    LOAD_METHOD(52);
    LOAD_METHOD(53);
    LOAD_METHOD(54);
    LOAD_METHOD(55);
    LOAD_METHOD(56);
    LOAD_METHOD(57);
    LOAD_METHOD(58);
    LOAD_METHOD(59);
    LOAD_METHOD(60);
    LOAD_METHOD(61);
    LOAD_METHOD(62);
    LOAD_METHOD(63);
    LOAD_METHOD(64);
    LOAD_METHOD(65);
    LOAD_METHOD(66);
    LOAD_METHOD(67);
    LOAD_METHOD(68);
    LOAD_METHOD(69);
    LOAD_METHOD(70);
    LOAD_METHOD(71);
    LOAD_METHOD(72);
    LOAD_METHOD(73);
    LOAD_METHOD(74);
    LOAD_METHOD(75);
    LOAD_METHOD(76);
    LOAD_METHOD(77);
    LOAD_METHOD(78);
    LOAD_METHOD(79);
    LOAD_METHOD(80);
    LOAD_METHOD(81);
    LOAD_METHOD(82);
    LOAD_METHOD(83);
    LOAD_METHOD(84);
    LOAD_METHOD(85);
    LOAD_METHOD(86);
    LOAD_METHOD(87);
    LOAD_METHOD(88);
    LOAD_METHOD(89);
    LOAD_METHOD(90);
    LOAD_METHOD(91);
    LOAD_METHOD(92);
    LOAD_METHOD(93);
    LOAD_METHOD(94);
    LOAD_METHOD(95);
    LOAD_METHOD(96);
    LOAD_METHOD(97);
    LOAD_METHOD(98);
    LOAD_METHOD(99);
    LOAD_METHOD(100);
    LOAD_METHOD(101);
    LOAD_METHOD(102);
    LOAD_METHOD(103);
    LOAD_METHOD(104);
    LOAD_METHOD(105);
    LOAD_METHOD(106);
    LOAD_METHOD(107);
    LOAD_METHOD(108);
    LOAD_METHOD(109);
    LOAD_METHOD(110);
    LOAD_METHOD(111);
    LOAD_METHOD(112);
    LOAD_METHOD(113);
    LOAD_METHOD(114);
    LOAD_METHOD(115);
    LOAD_METHOD(116);
    LOAD_METHOD(117);
    LOAD_METHOD(118);
    LOAD_METHOD(119);
    LOAD_METHOD(120);
}
// ========== End of wrappers
#endif // #if (WEBC_DEBUG_JSCRIPT)
#endif // #if (WEBC_SUPPORT_JSCRIPT)
