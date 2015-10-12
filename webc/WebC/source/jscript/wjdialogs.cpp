//
// WJDIALOGS.CPP -
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//    This module contains routines that implement the window alert, confirm and prompt methods.
//    Code in this module may also be used as a useful reference for creating integrated C and javascript code
//
// The working code resides in the function jwindow_dialogue()
//    Provides alert, confirm and prompt methods.
//      Demonstrates:
//          Running a new browser in a modal window
//          Creating 'C' functions that will be called from Javascript
//          Compiling and executing ascii Javascript source code in an existing document
//          Passing parameters from Javascript to C
//          Passing parameters from C to Javascript
//
//  A new browser window is created and placed at a fixed location on the screen
//    The browser window is created with the is_modal attribute.
//      This means that it will capture all mouse and keyboard input until it is closed.
//  A link is made between a locally defined 'C' function named PromptDismiss() and a Javascript function of the same name
//    When the PromptDismiss() is called from javascript the interpreter will invoke the native C function.
//  The HTML and Javascript code are executed in the browser
//     The onclick handlers for the dialog buttons are all PromtDismiss()
//     What arguments are passed to PromtDismiss() depend on which button was pressed
//
//  After the code is loaded jwindow_dialog() begind processing the message queue in a loop
//
//  When a button is pushed the 'C' function is invoked
//     The C function stores the parameters passed from Javascript in the Browser's private data slot
//
//  jwindow_dialogue() stops message processing when is sees that the private data slot is non-empty.
//     jwindow_dialogue() retrieves the data that was placed in the browser private area
//
//  jwindow_dialogue() executes some ascii javascript source code to close the modal browser (dialog box)
//     jwindow_dialogue() uses the webC API call named webc_JScriptExecuteSource() to executes the Javascript source
//     The source code contains "window.close()"
//     executing window.close(), destroys the modal window
//
// jwindow_dialogue() converts the retrieved data to a Javascript object and returns it to the Javascript interpreter.
//     For a prompt dialog the string entered by the user or null on cancel is returned
//     For a confirm dialog a JSCRIPT boolean is returned for OK/CANCEL
//

/*****************************************************************************/
// Headers
/*****************************************************************************/


#include "webcjhconfig.h"
#include "jwindow.h"
#if (WEBC_SUPPORT_JSCRIPT)
#include "jhutils.h"
#include "htmlbrow.hpp"
#include "htmldoc.hpp"
#include "webcmem.h"
#include "objref.cpp"
#include "webclog.h"
#include "rtpprint.h"
#include "wcapi.h"

/* Fixed configuration for dialog boxes. Change and recompile if needed */
#define WEBC_SKIN_CANVAS_WIDTH  640
#define WEBC_SKIN_CANVAS_HEIGHT 480

#define WEBC_SKIN_ALERT_WIDTH   200
#define WEBC_SKIN_ALERT_HEIGHT  100
#define WEBC_SKIN_ALERT_LEFT    (WEBC_SKIN_CANVAS_WIDTH-WEBC_SKIN_ALERT_WIDTH)/2
#define WEBC_SKIN_ALERT_TOP     (WEBC_SKIN_CANVAS_HEIGHT-WEBC_SKIN_ALERT_HEIGHT)/2

#define WEBC_SKIN_PROMPT_WIDTH   400
#define WEBC_SKIN_PROMPT_HEIGHT  100
#define WEBC_SKIN_PROMPT_LEFT    (WEBC_SKIN_CANVAS_WIDTH-WEBC_SKIN_PROMPT_WIDTH)/2
#define WEBC_SKIN_PROMPT_TOP     (WEBC_SKIN_CANVAS_HEIGHT-WEBC_SKIN_PROMPT_HEIGHT)/2

#define WEBC_SKIN_CONFIRM_WIDTH   200
#define WEBC_SKIN_CONFIRM_HEIGHT  100
#define WEBC_SKIN_CONFIRM_LEFT    (WEBC_SKIN_CANVAS_WIDTH-WEBC_SKIN_CONFIRM_WIDTH)/2
#define WEBC_SKIN_CONFIRM_TOP     (WEBC_SKIN_CANVAS_HEIGHT-WEBC_SKIN_CONFIRM_HEIGHT)/2

#define WEBC_SKIN_ALERT_SOURCE_1 "<BODY BGCOLOR=red><CENTER><BOLD> webC Alert !! </BOLD><BR>"
#define WEBC_SKIN_ALERT_SOURCE_2 "<BR><BR><INPUT TYPE=\"button\" VALUE=\"  OK  \"  onClick=\"PromptDismiss('OK')\">"
#define WEBC_SKIN_ALERT_SOURCE_3 "</CENTER></BODY>"

#define WEBC_SKIN_CONFIRM_SOURCE_1 "<BODY BGCOLOR=green><CENTER>"
#define WEBC_SKIN_CONFIRM_SOURCE_2 "<BR><BR><INPUT TYPE=\"button\" VALUE=\"  OK  \"  onClick=\"PromptDismiss('OK')\">"
#define WEBC_SKIN_CONFIRM_SOURCE_3 "&nbsp &nbsp <INPUT TYPE=\"button\" VALUE=\"CANCEL\"  onClick=\"PromptDismiss('')\">"
#define WEBC_SKIN_CONFIRM_SOURCE_4 "</CENTER></BODY>"

#define WEBC_SKIN_PROMPT_SOURCE_1 "<BODY BGCOLOR=blue>:&nbsp <INPUT NAME=\"resp\" TYPE=\"text\" VALUE=\"%s\"><BR>"
#define WEBC_SKIN_PROMPT_SOURCE_2 "<BR><CENTER><INPUT TYPE=\"button\" VALUE=\"  OK  \"  onClick=\"PromptDismiss(resp.value)\">"
#define WEBC_SKIN_PROMPT_SOURCE_3 "&nbsp &nbsp <INPUT TYPE=\"button\" VALUE=\"CANCEL\"  onClick=\"PromptDismiss('')\"></CENTER></BODY>"


#define WHICH_DIALOGUE_ALERT 1
#define WHICH_DIALOGUE_CONFIRM 2
#define WHICH_DIALOGUE_PROMPT 3
static JSBool jwindow_dialog(JSContext *cx, JSObject *obj,uintN argc, jsval *argv, jsval *rval, int which_dialogue);


// jwindow_alert(),jwindow_confirm(),and jwindow_prompt() are methods of the window calss (see jwindows.cpp)

JSBool jwindow_alert(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
#define WEBC_JSCRIPT_USER_ALERT 1
#if (WEBC_JSCRIPT_USER_ALERT)
	if (argc >= 1)
	{
		char *str = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		webc_user_alert_box((char *)str);

	}
	return JS_TRUE;
#else
    return jwindow_dialog(cx, obj, argc, argv, rval, WHICH_DIALOGUE_ALERT);
#endif
}

JSBool jwindow_dbprint(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc >= 1)
	{
		char *str = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		webc_user_debug_window((char *)str);
	}
	return JS_TRUE;
}

JSBool jwindow_dblog(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc >= 1)
	{
		char *str = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		LOG_JAVASCRIPT(str);
	}
	return JS_TRUE;
}


JSBool jwindow_confirm(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    return jwindow_dialog(cx, obj, argc, argv, rval, WHICH_DIALOGUE_CONFIRM);
}
JSBool jwindow_prompt(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    return jwindow_dialog(cx, obj, argc, argv, rval, WHICH_DIALOGUE_PROMPT);
}

/* 'C' function that we will register as a Javascript native function
    we will name it 'PromptDismiss()' but we could name it anything */
static JSBool PromptDismiss(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

/* Helper function that strlens and calls  wcDocWriteHtml() */
static void WriteDlgLine(wcCtx* Ctx, char *b)
{
   wcDocWriteHtml(Ctx, b, rtp_strlen(b), wcFALSE);
}
/*
 Helper function that strlens and calls  wcDocWriteHtml()
*/

static JSBool jwindow_dialog(JSContext *cx, JSObject *obj,uintN argc, jsval *argv, jsval *rval, int which_dialogue)
{
	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
	HTMLDocument *pDoc = (jsmgr)? jsmgr->GetDocument() : 0;

	if (argc < 1)
        return(JS_FALSE);

    char *prompt_str = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
// << EDIT 20080120 - Needs work

	// Clone the graphics abstraction layer function pointers so we know who to call
	HTMLBrowserConfig config, docConfig;
    rtp_memset(&config, 0, sizeof(config));
    rtp_memset(&docConfig, 0, sizeof(docConfig));
	pDoc->GetBrowser()->GetConfig(&docConfig);
	config.graphics = docConfig.graphics;

    switch (which_dialogue) {
        case WHICH_DIALOGUE_ALERT:
            config.WindowConfig.left =   WEBC_SKIN_ALERT_LEFT;
            config.WindowConfig.top =    WEBC_SKIN_ALERT_TOP;
            config.WindowConfig.width =  WEBC_SKIN_ALERT_WIDTH;
            config.WindowConfig.height = WEBC_SKIN_ALERT_HEIGHT;
            break;
        case WHICH_DIALOGUE_PROMPT:
            config.WindowConfig.left =   WEBC_SKIN_PROMPT_LEFT;
            config.WindowConfig.top =    WEBC_SKIN_PROMPT_TOP;
            config.WindowConfig.width =  WEBC_SKIN_PROMPT_WIDTH;
            config.WindowConfig.height = WEBC_SKIN_PROMPT_HEIGHT;
            break;
        case WHICH_DIALOGUE_CONFIRM:
        default:
            config.WindowConfig.left =   WEBC_SKIN_CONFIRM_LEFT;
            config.WindowConfig.top =    WEBC_SKIN_CONFIRM_TOP;
            config.WindowConfig.width =  WEBC_SKIN_CONFIRM_WIDTH;
            config.WindowConfig.height = WEBC_SKIN_CONFIRM_HEIGHT;
            break;
    }
	config.WindowConfig.WindowFlags |= WF_MODAL;

    HBROWSER_HANDLE Browser = webc_CreateBrowser(&config,  WEBC_TRUE,"dialog", "");
    if (Browser)
    {
     HDOC_HANDLE hDoc = webc_BrowserGetDocument(Browser);
	 wcCtx Ctx;
     char buff[128];
	  wcCtxtInit(&Ctx, (wcBROW) Browser, (wcDOC) hDoc);
     /* Experimental */
     wcDocClear(&Ctx, wcFALSE);
     wcDocOpen(&Ctx);


     // Create a link between 'C' and Javascript the Native 'C' function PromptDismiss will
     // be called with the value of the input field.
     webc_JScriptDefineFunction(hDoc, "PromptDismiss", (JSNative) PromptDismiss, 0);
     if (which_dialogue == WHICH_DIALOGUE_ALERT)
     {
         WriteDlgLine(&Ctx, WEBC_SKIN_ALERT_SOURCE_1);
         WriteDlgLine(&Ctx, prompt_str);
         WriteDlgLine(&Ctx, WEBC_SKIN_ALERT_SOURCE_2);
         WriteDlgLine(&Ctx, WEBC_SKIN_ALERT_SOURCE_3);
     }
     else if (which_dialogue == WHICH_DIALOGUE_CONFIRM)
     {
         WriteDlgLine(&Ctx, WEBC_SKIN_CONFIRM_SOURCE_1);
         WriteDlgLine(&Ctx, prompt_str);
         WriteDlgLine(&Ctx, WEBC_SKIN_CONFIRM_SOURCE_2);
         WriteDlgLine(&Ctx, WEBC_SKIN_CONFIRM_SOURCE_3);
     }
     else if (which_dialogue == WHICH_DIALOGUE_PROMPT)
     {
         char *default_str = "";
         if (argc > 1)
             default_str = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
         WriteDlgLine(&Ctx, prompt_str);
         rtp_sprintf(&buff[0],WEBC_SKIN_PROMPT_SOURCE_1,default_str);
         WriteDlgLine(&Ctx, &buff[0]);
         WriteDlgLine(&Ctx, WEBC_SKIN_PROMPT_SOURCE_2);
         WriteDlgLine(&Ctx, WEBC_SKIN_PROMPT_SOURCE_3);
     }
     wcDocClose(&Ctx);
     wcDocRefresh(&Ctx);

     /* Go modal here */
  //   HBROWSER_HANDLE Browser = webc_CreateBrowser(&config,  WEBC_TRUE, "");
     HTMLBrowser *pBrowser = (HTMLBrowser *) Browser;
     while (pBrowser->ProcessInputQueue(WEBC_TRUE) >= 0)
     {
         WebString *user_WebString;
         void* privateData = webc_BrowserGetPrivateData(Browser);
         if (privateData)
         {
             user_WebString = (WebString *) privateData;
             if (which_dialogue == WHICH_DIALOGUE_PROMPT)
             {
                 if (user_WebString && user_WebString->getLength())
 			        *rval = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, user_WebString->getChars()));
                 else
                     *rval = JSVAL_VOID;
             }
             else if (which_dialogue == WHICH_DIALOGUE_ALERT)
                 *rval = JSVAL_VOID;
             else if (which_dialogue == WHICH_DIALOGUE_CONFIRM)
             {
                 if (user_WebString && webc_c_strcmp(user_WebString->getChars(), "OK")==0)
                     *rval = BOOLEAN_TO_JSVAL(JS_TRUE);
                 else
                     *rval = BOOLEAN_TO_JSVAL(JS_FALSE);
             }
             WEBC_DELETE(user_WebString);
             webc_JScriptExecuteSource(hDoc, "window.close();");
             break;
         }
     }
     }
     return jutils_CheckContextState(cx);
}

/* C function bound to a Javascript function of the same name. Called when a prompt
   dialogue is dissmissed */
static JSBool PromptDismiss(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{

	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
	if (argc > 0)
	{
		JSString *user_input;
		user_input = JS_ValueToString(cx, *argv);
		JS_AddRoot(cx, &user_input);
		WEBC_CHAR *user_string = WEBC_JS_STRING_TO_WEBC_STRING(user_input);
		WebString *user_WebString;
        int user_len = webc_strlen(user_string);
		WEBC_NEW_VERBOSE(user_WebString, WebString(user_len+1),"JsPrompDismissDialog");
		if (user_WebString)
            user_WebString->copy(user_string, user_len+1);
		JS_RemoveRoot(cx, &user_input);

        HTMLDocument *pDoc = (jsmgr)? jsmgr->GetDocument() : 0;
        HDOC_HANDLE doc = (HDOC_HANDLE) pDoc;
        if (doc)
        {
            HBROWSER_HANDLE hbrowser = webc_DocGetBrowser(doc);
            if (hbrowser)
            {
                void* privateData = webc_BrowserGetPrivateData(hbrowser);
                privateData = (void *) user_WebString;
                webc_BrowserSetPrivateData(hbrowser, privateData);
            }
        }
	}
	return (JS_TRUE);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
