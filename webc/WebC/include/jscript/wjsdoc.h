/*
|  WJSDOC.H -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#ifndef __WJSDOC_H__
#define __WJSDOC_H__

#include "webcfg.h"

#if (WEBC_SUPPORT_JSCRIPT)

#ifdef __cplusplus
extern "C" {
#endif
#include "jsapi.h"
#include "vector.h"
#ifdef __cplusplus
}
#endif

#include "webctypes.h"
#include "vector.h"
#include "refcount.hpp"
#include "ref.hpp"
#include "htmlsdk.h"
#include "jsobjref.hpp"

#ifdef __cplusplus

class HTMLDocument;
class WebcJSBrowserContext;
class WebcJSScript;
class HTMLBody;
class HTMLElement;

class WebcJSDocumentContext : public ReferenceCounted
{

protected:
	void Init (HTMLDocument* doc);
	void Clear (void);
	void CancelTimeouts (void);

	vector        mpCollections[1];
	JSContext*    mpCx;
	WebcJSScript* mpDefaultScript;
	HTMLBody*     mpFakeBody;
	HTMLEvent     mHtmlEvent;
	JSObjectRef   mEventObj;
	JSObjectRef   mLocation;
	JSObjectRef   mNavigator;
	JSObjectRef   mHistory;
	JSObjectRef   mWindow;

	ObjectReference<HTMLDocument> mDocument;
	WebcJSBrowserContext* mBrowserContext;

public:
	WebcJSDocumentContext  (WebcJSBrowserContext* browserCtx, HTMLDocument* doc);
	~WebcJSDocumentContext (void);
    // < 20080120 - added ErrorFunction entry
	JSFunction *mErrorFunction;

//	using ReferenceCounted::recycle;

 	virtual void recycle (void);


	int           AddNewElement   (HTMLElement* elem);
	JSContext*    AllocContext    (void);
	JSObject*     CreateElement   (HTMLElementType type);
	void          Destroy         (void);
	void          FreeContext     (JSContext* cx);
	void          GarbageCollect  (void);
	vector*       GetCollections  (void) { return mpCollections; }
	JSContext*    GetContext      (void) { return mpCx; }
	HTMLDocument* GetDocument     (void) { return mDocument.Get(); }
	HTMLBody*     GetFakeBody     (void) { return mpFakeBody; }
	JSObject*     GetGlobalObject (void) { return mWindow.Get(); }
	void          SetDocument     (HTMLDocument* doc) { mDocument.Set(doc);}
	void          SetWindowEvent  (HTMLEvent* event);
	WEBC_UINT8    GetEventFlags  () { return mHtmlEvent.flags; }

	JSObject*     Event           (void) { return mEventObj.Get(); }
	JSObject*     Location        (void) { return mLocation.Get(); }
	JSObject*     Navigator       (void) { return mNavigator.Get(); }
	JSObject*     History         (void) { return mHistory.Get(); }
	JSObject*     Window          (void) { return mWindow.Get(); }
};

extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* WEBC_SUPPORT_JSCRIPT */

#endif /* __WJSDOC_H__ */
