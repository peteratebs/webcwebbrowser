#ifndef __WCAPI_INCLUDED__
#define __WCAPI_INCLUDED__
//
// wcapi.h -
//
// EBS - WebC
//
// Copyright EBS Inc. , 2011
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//
#include "webctypes.h"
#include "htmlsdk.h"   /* For element types, remove later */
#include "listener.hpp"   /* For litener events */
#include "webc.h"


typedef unsigned char * wcEL;
typedef unsigned char * wcDOC;
typedef unsigned char * wcBROW;
#define HELEMENT_OBJ(X)           ((HTMLElement *)(X))
#define DELEMENT_HANDLE void *
#define HBROWSER_OBJ(X)           ((HTMLBrowser *)(X))
#define HDOC_OBJ(X)               ((HTMLDocument *)(X))


#define MAXIDSIZE 256
#define HTML_TEMP_STR_LEN         256

enum e_wcBOOL
{
	wcFALSE,
	wcTRUE,
};
typedef enum e_wcBOOL wcBOOL;

#define wcMODAL		1
/* Not completed yet */
#define wcRESIZE	2
#define wcSYSTEM	4
#define wcBORDER	8
typedef unsigned long wcWINFLAGS;

enum DomTraverseResult
{
    DomError=-1,
    DomFind,
    DomSelf,
    DomParent,
    DomFirstChild,
    DomLastChild,
    DomPrevSibling,
    DomNextSibling,
};


#define MAXGARBAGEPOINTERS 256
struct s_wcCtx
{
	wcDOC hdoc;
	wcBROW hbrowser;
	char *GarbageList[MAXGARBAGEPOINTERS];
};
typedef struct s_wcCtx wcCtx;

char * wcCtxtAppendGarbage(wcCtx* Ctx, char *p);

typedef wcBOOL (*wcDocEnumFn)(wcCtx* Ctx, wcEL El, int UserIntParm, void *UserVoidParm);
typedef HTMLEventStatus (*wcEventCallback) (wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param);
typedef void (*wcTimedCallbackfn)(int UserIntParm, void *UserVoidParm);


/* Dom functions */
wcEL wcElNew(wcCtx* Ctx, char *Html);
wcEL wcELFind (wcCtx* Ctx, wcEL El, char *Domstr, wcBOOL *pError);
wcBOOL wcELIdMatches(wcEL element, char *string);
wcBOOL wcELRemoveChild (wcEL parent,wcEL child);
wcBOOL wcELDeleteChildren (wcEL element);
wcBOOL wcELDelete(wcEL element);
wcBOOL wcELInsertFirst(wcEL element,wcEL child);
wcBOOL wcELInsertLast(wcEL element,wcEL child);
wcBOOL wcELInsertAfter(wcEL Parent,wcEL CurChild, wcEL NewChild);
wcBOOL wcELInsertBefore(wcEL Parent,wcEL CurChild,wcEL NewChild);
void wcElSetInnerHtml(wcEL El,	char * newhtml);
void wcElSetEventHandler(HELEMENT_HANDLE element, wcEventCallback cb);
void wcElPopEventHandler(HELEMENT_HANDLE element);
HTMLEventStatus wcTriggerEvent(wcEL El, HTMLEvent *pEvent, int delay);
void wcTimedCallback(wcTimedCallbackfn pfn,int delay, int IntParm, void *VoidParm);
wcEL wcElNew(wcCtx* Ctx, char *_name);
wcEL wcElClone(wcEL element, wcBOOL bCloneChildren);
void wcDocEnumerate(wcCtx* Ctx, wcDocEnumFn pfn, int Intparm, void *Voidparm);
wcEL wcDocFindEl(wcCtx* Ctx, char *id);
void wcDocRefresh(wcCtx* Ctx);
void wcCtxtInit(wcCtx* Ctx,  wcBROW hbrowser, wcDOC doc);
void wcCtxtRelease(wcCtx* Ctx);
void wcDocInsertFirst (wcCtx* Ctx,wcEL eL);
void wcDocInsertLast (wcCtx* Ctx,wcEL eL);
wcBOOL wcElSetStyle(wcEL element, char *_style,wcBOOL doUpdate);
wcBOOL wcElSetId(wcEL element, char *_Id);
wcBOOL wcElSetChecked(wcEL element,wcBOOL checked);
wcBOOL wcElSetValue(wcEL element, char *_Val);
char *wcELGetId(wcCtx* Ctx,wcEL element);
char *wcELGetValue(wcCtx* Ctx,wcEL element);
wcBOOL wcElGetChecked(wcEL element);
wcBOOL wcElGetPosWin(wcEL element, int *top, int *left, int *width, int *height);
wcBOOL wcElGetPosStyle(wcEL element, int *top, int *left, int *width, int *height);
HTMLElementType wcElGetType(wcEL element);
int wcWinCreateEx(wcCtx* Ctx,char *_windowname,	wcEventCallback BodyCallback,char *_defaultUrl,	char *_defaultContent,struct HTMLBrowserWindowConfig *pconf);
int wcWinExecute(wcCtx *Ctx);
void wcWinExecuteStop(wcCtx *Ctx, int ReturnValue);
void wcWinClose(wcCtx *Ctx);
void wcPushData(wcCtx *Ctx, wcEL  Element, void* data);
void * wcGetData(wcCtx *Ctx, wcEL  Element);
void wcPopData(wcCtx *Ctx, wcEL  Element);
void wcWinClaimFocus(wcCtx *Ctx);
void wcWinReleaseFocus(wcCtx *Ctx);
wcBOOL wcWinSetFocus(wcCtx *Ctx,wcEL  Element);
wcEL wcWinGetFocus(wcCtx *Ctx);
void wcWinMoveTo(wcCtx *Ctx,int left, int top);
void wcWinBringToTop(wcCtx *Ctx);
wcBOOL wcWinGetPos(wcCtx *Ctx, int *left, int *top, int *width, int *height);
wcBOOL wcWinSetPos(wcCtx *Ctx, int left, int top, int width, int height);
#endif // _WCAPI_INCLUDED__
