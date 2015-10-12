#ifndef _JUTILS_H_
#define _JUTILS_H_

#include "webc.h"
#include "htmldom.hpp"

#if (WEBC_SUPPORT_JSCRIPT)

#if (WEBC_SUPPORT_UNICODE)
#define WEBC_JS_NEW_STRING_COPY_Z      JS_NewUCStringCopyZ
#define WEBC_JS_STRING_TO_WEBC_STRING  JS_GetStringChars
#else
#define WEBC_JS_NEW_STRING_COPY_Z      JS_NewStringCopyZ
#define WEBC_JS_STRING_TO_WEBC_STRING  JS_GetStringBytes
#endif

JSClass *getJSClassByElementType(HTMLElementType type);
void nameGenerator(JSContext *cx, char *str);

/**
 * Parsing functions
 */
char *jutils_ParseNameValuePair (char *pStr,   char *pName, char *pValue);
int   jutils_ParseBoolVal       (char *pValue, int *value);
char *jutils_FirstNonWS         (char *pFind);
void  jutils_NullOutTrailingWS  (char *pStr);

/**
 * These functions are used by jhtml_collection in order to traverse Forms and Inputs easily
 */
class HTMLElement;

HTMLElement *jutils_CheckObjectList(vector *pVec, int (*match)(HTMLElement *data, WEBC_PFBYTE p), WEBC_PFBYTE param);
int jutils_CountObjectList(vector *pVec, int (*match)(HTMLElement *data, WEBC_PFBYTE p), WEBC_PFBYTE param);

HTMLElement *jutils_CheckAllFormsAndInputs(vector *pVec, int (*match)(HTMLElement *data, WEBC_PFBYTE p), WEBC_PFBYTE param);
int jutils_CountAllFormsAndInputs(vector *pVec, int (*match)(HTMLElement *data, WEBC_PFBYTE p), WEBC_PFBYTE param);

HTMLElement *jutils_GetNthOfObjectList(vector *pVec, int index, int (*match)(HTMLElement *data, WEBC_PFBYTE p), WEBC_PFBYTE param);
HTMLElement *jutils_GetNthOfFormsAndInputs(vector *pVec, int index, int (*match)(HTMLElement *data, WEBC_PFBYTE p), WEBC_PFBYTE param);

HTMLEventType jutils_StringToEventType(char *pEventName);
const char *jutils_EventTypeToString(HTMLEventType type);

JSBool jutils_CheckContextState(JSContext *cx);
void jutils_LoadDocumentUrl(JSContext* cx, HTMLDocument* doc, const WEBC_CHAR* url);
HTMLElement *jhutil_CloneElement(HTMLElement *clone, WEBC_BOOL bCloneChildren);

/**
 * These methods are default methods that return WEBC_TRUE no matter what
 */
JSBool jutil_addProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool jutil_delProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool jutil_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool jutil_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool jutil_enumerate  (JSContext *cx, JSObject *obj);
JSBool jutil_resolve    (JSContext *cx, JSObject *obj, jsval id, char *who_called);
JSBool jutil_convert    (JSContext *cx, JSObject *obj, JSType type, jsval *vp);
void   jutil_finalize   (JSContext *cx, JSObject *obj);

void  jhutil_SetPrivate (JSContext *cx, JSObject *obj, void *ptr);
void *jhutil_GetPrivate (JSContext *cx, JSObject *obj);
void  jhutil_SetContextPrivate (JSContext *cx, void *ptr);
void *jhutil_GetContextPrivate (JSContext *cx);

int jutils_ResumeRefresh (JSContext* cx);
void jutils_RestoreRefresh (JSContext* cx, int refreshState);

#define JSVAL_IS_STRINGABLE(v) (JSVAL_IS_NUMBER(v) || JSVAL_IS_STRING(v) || JSVAL_IS_BOOLEAN(v))


#endif //_JUTILS_H_
#endif //WEBC_SUPPORT_JSCRIPT
