#ifndef __HTMLDOC_HPP__
#define __HTMLDOC_HPP__

#include "helement.hpp"
#include "hbody.hpp"
#include "ref.hpp"
#include "webcutf.h"
#include "hframe.hpp"
#include "wjsdoc.h"


/*************************************************************************
* class HTMLDocument
*
*************************************************************************/

class HTMLDocumentStream;
class URLDescriptor;

#if (WEBC_SUPPORT_FRAMES)
class HTMLFrame;
#endif // WEBC_SUPPORT_FRAMES

#define HDOC_FLAG_CREATING_JMANAGER    0x0001
#define HDOC_FLAG_STREAM_CHARSET       0x0002
#define HDOC_FLAG_STOPPING             0x0004
#define HDOC_FLAG_SCROLL_POSITION_SET  0x0008

#include "dmanager.hpp"

enum HtmlCharsetSource
{
	HTML_CHARSET_SRC_NONE = 0,
	HTML_CHARSET_SRC_REFERER,
	HTML_CHARSET_SRC_META,
	HTML_CHARSET_SRC_STREAM
};

#if (WEBC_SUPPORT_OFFLINE_BROWSING)

#include "wcache.h"
#include "sramdisk.hpp"
#include "webcmem.h"
#include "refcount.hpp"

class CacheHandle : public ReferenceCounted
{
 protected:
 	wcache_ctx* mCacheContext;

 public:
 	CacheHandle (wcache_ctx* ctx = 0) { mCacheContext = ctx; }
 	virtual ~CacheHandle (void) {}

 	wcache_ctx*  getCache (void)            { return mCacheContext; }
 	void         setCache (wcache_ctx* ctx) { mCacheContext = ctx; }
 	virtual void recycle  (void)            { WEBC_DELETE(this); }
};

class DocumentContentCacheHandle : public CacheHandle
{
 private:
 	SimpleRamDisk* mRamDisk;

 public:
 	DocumentContentCacheHandle (void)
 	{
		WEBC_NEW(mRamDisk, SimpleRamDisk());
		if (mRamDisk)
		{
			mCacheContext = (wcache_ctx*) WEBC_MALLOC(sizeof(wcache_ctx));
			if (mCacheContext)
			{
				if (wcache_init(mCacheContext, "",
				                WEBC_CFG_ONLINE_CACHE_SIZE,
				                WEBC_CFG_ONLINE_CACHE_MAX_FILES,
				                &gWebcRamCacheSpec, mRamDisk) < 0)
				{
					WEBC_FREE(mCacheContext);
					mCacheContext = 0;
				}
			}

			if (!mCacheContext)
			{
				WEBC_DELETE(mRamDisk);
				mRamDisk = 0;
			}
		}
 	}

 	virtual ~DocumentContentCacheHandle (void)
 	{
		if (mCacheContext)
		{
			wcache_free(mCacheContext);
			WEBC_FREE(mCacheContext);
		}

		if (mRamDisk)
		{
			WEBC_DELETE(mRamDisk);
		}
 	}

 	WEBC_BOOL createdOkay (void) { return mRamDisk && mCacheContext; }
};

#endif // WEBC_SUPPORT_OFFLINE_BROWSING

class HTMLDocument : public Referable, public Listener, public Notifier
{
 protected:
	HTMLElementContainer*        mHead;
	HTMLBody*                    mBody;
	HTMLBrowser*                 mpBrowser;
	URLDescriptor                mCurrentUrl;
	URLDescriptor                mBaseUrl;
	HTMLDocumentStream*          mpDocStream;
	WebcImageCollection*         mpImages;
	WEBC_CHAR*                   mpName;
	WEBC_CHAR*                   mpNewUrl;
	int                          miCurrentJobs;
	WebcCharset                  mCharset;
	WEBC_UINT16                  mwFlags;
	DisplayManager*              mpManager;
	DISPLAY_INT                  miNewX;
	DISPLAY_INT                  miNewY;
	WEBC_CHAR*                   mpTitle;
	vector                       mPendingImages;
	ObjectReference<HTMLElement> mFocusElement;
	WEBC_CHAR*                   mpScrollToAnchor;
#if (WEBC_SUPPORT_REFERAL_HEADER)
	WEBC_CHAR*                   referal_url;
#endif
  #if (WEBC_SUPPORT_JSCRIPT)
	WebcJSDocumentContext*       mpJSMgr;
	JSObjectRef                  mJSObject;
	vector                       mJSCustomFunctions;
  #endif //(WEBC_SUPPORT_JSCRIPT)

  #if (WEBC_SUPPORT_STYLE_SHEETS)
	CSSDocumentContext*   mpCSSContext;
  #endif

  #if (WEBC_SUPPORT_FRAMES)
	HTMLFrame*            mpFrame;
  #endif

  #if (WEBC_CACHE_IMAGES && 0) // tbd
	static WEBC_BOOL      mBitmapCacheInitialized;
	static wcache_ctx     mBitmapCache;
  #endif

  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
	DocumentContentCacheHandle* mpOnlineCacheHandle;
  #endif

 public:
	HTMLDocument(void);
	~HTMLDocument(void);

	// Listener interface
	virtual void Notify (Notifier *who, NotifyEvent *what);

	HTMLElement *Head();
	HTMLBody *Body();
	HTMLElement *FindElement(HTMLElementFinder *pFinder, int iSearchFrames = 1);

	WEBC_UINT16 GetFlags (void);
	void SetFlag (WEBC_UINT16 mask);
	WEBC_UINT16 CheckFlag (WEBC_UINT16 mask) { return (GetFlags() & mask) != 0; }
	void ClearFlag (WEBC_UINT16 mask);

	void Clear();
	void ClearDocument();
	void SetStatus(WEBC_CHAR *pStr);
	WEBC_CHAR *Status(void);

	void SetBrowser(HTMLBrowser *pBrowser);
	HTMLBrowser *GetBrowser(void) { return mpBrowser; }
	HTMLDocument *Parent(void);
	HTMLDocument *RootParent(void);
	WEBC_BOOL IsDescendantOf (HTMLDocument* parentDoc);
	WEBC_BOOL IsAncestorOf (HTMLDocument* childDoc);
	int FrameDepth (void);

	void LoadUrl (const WEBC_CHAR *urlStr, WEBC_BOOL saveInHistory);
	void SetURL (URLDescriptor *pUrlDesc);
	URLDescriptor *CurrentURL(void);
	URLDescriptor *BaseURL(void);

	void ResetCharset(void);
	void SetCharset(WebcCharset charset, HtmlCharsetSource source);
	WebcCharset GetCharset (void);

	void SaveLocation(void);

	/**

	 @memo Set the source URL for a document
	 @doc  This method does not perform the actual URL load.  To
	       update the contents of the document after you've set the
	       source URL, call Update.

	 @param pNewUrl New URL to load this document from.
	 @see HTMLDocument::SetURL, HTMLDocument::Update
	 */
	void SetSrc(const WEBC_CHAR *pNewUrl, URLDescriptor *pBaseUrl = 0);
	void SetSrc(const WEBC_CHAR *pNewUrl, URLDescriptor *pBaseUrl, DISPLAY_INT iNewX, DISPLAY_INT iNewY);
	void SetDefaultScrollPosition(DISPLAY_INT iNewX, DISPLAY_INT iNewY);

	/**

	 @memo Load outstanding URL's in an HTMLDocument
	 @doc  This method will initiate a document load if the current
	       URL has changed.

	 @param iAbortCurrent Boolean; if non-zero, then all currently loading jobs on this
	                      Browser will be halted before queueing this one
	 @see HTMLDocument::SetURL
	 */
	void Update(int iAbortCurrent, WEBC_CHAR *referer);
	/**

	 @memo Reload a URL
	 @doc  This method will reload the current document and refresh the display

	 @see HTMLDocument::Update
	 */
	void Refresh(void);

	/**

	 @memo Notify an HTMLDocument that one of its children is loading a URL
	 @doc  This method is called when an HTMLElement (for example an IMG or
	       FRAME tag) initiates a URL load.

     @param pEl     A pointer to the element being loaded.
	 @see HTMLDocument::NotifyDone
	 */
	void NotifyLoading(HTMLElement *pEl);

	/**

	 @memo Notify an HTMLDocument that one of its children is done loading a URL
	 @doc  This method is called when an HTMLElement (for example an IMG or
	       FRAME tag) completes a URL load.  It is used to synchronize the
	       sending of HTML_EVENT_LOAD events to the appropriate HTMLElements.

     @param pEl     A pointer to the element whose load is complete.
	 @see HTMLDocument::NotifyLoading
	 */
	void NotifyDone(HTMLElement *pEl);


	int QueueLoad (URLDescriptor* urlDesc, int iAbortCurrent);

  #if (WEBC_SUPPORT_FRAMES)
	/**

	 @memo Return the frame element a document belongs to, if any
	 @doc  Can be used to determine whether or not this document was
	       created as the content of an HTMLFrame element within
	       a FRAMESET tag.

	 @return        The parent frame if in one, NULL otherwise.
     @see
	 */
	HTMLFrame *GetFrame(void);
	void SetFrame(HTMLFrame *pFrame);
	WEBC_BOOL IsQuarantined (void) { return (mpFrame? ((mpFrame->mFlags & HFRAME_FLAG_QUARANTINED) != 0) : WEBC_FALSE); }
  #else
	WEBC_BOOL IsQuarantined (void) { return (WEBC_FALSE); }
  #endif // WEBC_SUPPORT_FRAMES

	int Open(void);
	void Close(void);
	int IsOpen();
	long Write(const WEBC_CHAR* input, HTMLParseContext* parser = WEBC_NULL);
	long WriteLine(const WEBC_CHAR* input, HTMLParseContext* parser = WEBC_NULL);
	long Write(const WEBC_CHAR* input, long len);
	long WriteLine(const WEBC_CHAR* input, long len);
	WEBC_CHAR *GetTitle();
	WEBC_CHAR *BaseTarget(void);
	void SetTitle(const WEBC_CHAR *str);
	WebcImageCollection *ImageBitmaps(void);

	WEBC_CHAR *Name() { return(mpName); }
	void SetName(const WEBC_CHAR *pName);

  #if (WEBC_SUPPORT_STYLE_SHEETS)
	CSSDocumentContext * GetCSSContext(void);
	CSSValueType GetCSSProperty(CSSStyleElement * element, CSSPropertyType type, CSSPropertyValue * value);
	void StyleSheetUpdate(void);
  #endif // (WEBC_SUPPORT_STYLE_SHEETS)

  #if (WEBC_SUPPORT_JSCRIPT)
	void ClearJavaScript();
	WebcJSDocumentContext* GetJSMgr();
	JSObject* GetJSObject();
	JSContext* GetJSContext() { return GetJSMgr()? GetJSMgr()->GetContext() : 0; }
	void SetJSObject(JSObject *jo);
	WEBC_BOOL DefineJSFunction (JSNative func, WEBC_INT32 numArgs, const char* name);
	WEBC_BOOL ResolveJSFunction (JSNative* func, WEBC_INT32* numArgs, const WEBC_CHAR* name,
	                             WEBC_BOOL inheritParentFunctions = WEBC_TRUE);
   #if (WEBC_SUPPORT_UNICODE)
	WEBC_BOOL ResolveJSFunction (JSNative* func, WEBC_INT32* numArgs, const char* name,
	                             WEBC_BOOL inheritParentFunctions = WEBC_TRUE);
   #endif // WEBC_SUPPORT_UNICODE
  #endif //(WEBC_SUPPORT_JSCRIPT)

	HTMLElement *GetFocusElement() { return (0); }
	void SetFocusElement (HTMLElement *pElem);
	void SetDisplayManager (DisplayManager *pManager);
	DisplayManager *GetDisplayManager (void);
	WEBC_BOOL ScrollToAnchor (const WEBC_CHAR *name);
	void ScrollToDefaultAnchor (void);
	void ScrollTo (DISPLAY_INT x, DISPLAY_INT y);
	void ScrollBy (DISPLAY_INT x, DISPLAY_INT y);
	DISPLAY_INT GetScrollLeft (void);
	DISPLAY_INT GetScrollTop (void);
	void SetScrollLeft (DISPLAY_INT i);
	void SetScrollTop (DISPLAY_INT i);
	void ClearRefreshTimers (void);
	void SetRefreshTimer (const WEBC_CHAR *url, WEBC_UINT32 msec);
	void QueueFocus (HTMLElement *element);
	void SetQueuedFocus (void);

	void LoadImage (const WEBC_CHAR *url, Listener *forWhom, WEBC_BOOL bForceLoad=WEBC_FALSE);
	void RemovePendingImageListener (Listener *who);
	void RemovePendingImage (const WEBC_CHAR *key);

	int OkayToResetFocusToBody (void);

#if (WEBC_SUPPORT_EMBED)
	void LoadEmbed (const WEBC_CHAR *url, Listener *forWhom, WEBC_BOOL bForceLoad=WEBC_FALSE);
#endif

  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
	CacheHandle* GetOnlineCache (void);
	void ClearOnlineCache (void);
	void SaveContentForOffline (WEBC_BOOL createDirEntry = WEBC_TRUE);
  #endif
};

#endif // __HTMLDOC_HPP__
