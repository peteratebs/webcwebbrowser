//
// HTMLDOC.CPP - Methods for class HTMLDocument
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "webcjhconfig.h"
#include "html.h"
#include "htmldom.hpp"
#include "css.h"
#include "webfonts.hpp"
#include "webload.hpp"
#include "hdstream.hpp"
#include "hform.hpp"
#include "hframe.hpp"
#include "hframset.hpp"
#include "hbase.hpp"
#include "hanchor.hpp"
#include "htmlfind.hpp"
#include "htmlbrow.hpp"
#include "webctmo.h"
#if (WEBC_SUPPORT_JSCRIPT)
 #include "wjsdoc.h"
#endif
#include "webcstr.h"
#include "objref.cpp"
#include "webcmem.h"
#include "rtpprint.h"



/*****************************************************************************/
// Local data types
/*****************************************************************************/

struct MetaRefreshInfo
{
	WEBC_CHAR*    url;
	HTMLDocument* doc;
};

struct PendingImage
{
	WEBC_CHAR* key;
	vector     listeners;
};

#if (WEBC_SUPPORT_JSCRIPT)

#define WEBC_JS_CUSTOM_FUNCTION_NAME_LEN  64

struct functionInfo
{
	JSNative   func;
	WEBC_INT32 numArgs;
	char       name[WEBC_JS_CUSTOM_FUNCTION_NAME_LEN];
};

#endif

/*****************************************************************************/
// Data
/*****************************************************************************/
#if (WEBC_SUPPORT_JSCRIPT)
extern "C" WebcJSBrowserContext* gJSBrowserContext;
#endif
/*****************************************************************************/
// Static functions
/*****************************************************************************/

static int _DoMetaRefresh(void* data, long err);
static int _FreeMetaRefresh(WEBC_PFBYTE data);
int _GetBitmapKey(WEBC_CHAR * pKey, URLDescriptor *pUrl);


#if (WEBC_SUPPORT_EMBED)
int _Get_embed_elementKey(WEBC_CHAR *pKey, URLDescriptor *pUrl);
#endif


extern "C" {
static int _MatchListener(WEBC_PFBYTE data, WEBC_PFBYTE p);
static int _MatchPendingImageKey(WEBC_PFBYTE data, WEBC_PFBYTE param);
static int _RemoveListenerFromPendingImage(WEBC_PFBYTE data, WEBC_PFBYTE p);
static void _PendingImageDestructor(WEBC_PFBYTE data);
}

/*****************************************************************************/
// HTMLDocument methods
/*****************************************************************************/

HTMLDocument::HTMLDocument(void)
{
	mpBrowser = 0;
	mpDocStream = 0;
	mpName = 0;
	miCurrentJobs = 0;
	mpNewUrl = 0;
	miNewX = 0;
	miNewY = 0;
	mpTitle = 0;
	mpScrollToAnchor = 0;
	mCharset = WEBC_CHARSET_ASCII;
	mwFlags = 0;
	mpManager = 0;
	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)

	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"HTMLDocument::HTMLDocument"," ")

  #if (WEBC_SUPPORT_JSCRIPT)
	mpJSMgr = 0;
	vector_init(&mJSCustomFunctions, sizeof(functionInfo));
  #endif

  #if (WEBC_SUPPORT_FRAMES)
	mpFrame = 0;
  #endif // WEBC_SUPPORT_FRAMES

  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
	mpOnlineCacheHandle = 0;
  #endif

	vector_init(&mPendingImages, sizeof(PendingImage));

  #if (WEBC_SUPPORT_STYLE_SHEETS)
	WEBC_NEW_VERBOSE(mpCSSContext, CSSDocumentContext(this),"CSSDocumentContext");
  #endif

	WEBC_NEW_VERBOSE(mpImages, WebcImageCollection(),"WebcImageCollection");
	WEBC_NEW_VERBOSE(mHead, HTMLElementContainer(0, this),"HTMLElementContainer");
	WEBC_NEW_VERBOSE(mBody, HTMLBody(this),"HTMLBody");
	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"HTMLDocument::HTMLDocument"," ")
}

HTMLDocument::~HTMLDocument(void)
{
	Close();

	// We do the clear before disassociating the HtmlWindow from
	//  this document because the UNLOAD event may cause the DOM
	//  to need to access the HtmlWindow's HTMLDocument.
	Clear();

	if (mpBrowser)
	{
		mpBrowser->AbortDocumentJobs(this);
	}

	WEBC_DELETE(mHead);
	WEBC_DELETE(mBody);

#if (WEBC_SUPPORT_JSCRIPT)

	// Do this AFTER clearing the document, so that any JScript
	//  based onunload event handlers will operate correctly.
	if (mpJSMgr)
	{
		mpJSMgr->SetDocument(0);
		mpJSMgr->Destroy();
		mpJSMgr = 0;
	}

	vector_delete(&mJSCustomFunctions);

#endif // WEBC_SUPPORT_JSCRIPT

	WEBC_DELETE(mpImages);

#if (WEBC_SUPPORT_STYLE_SHEETS)
	if (mpCSSContext)
	{
		mpCSSContext->Destroy();
		mpCSSContext = 0;
	}
#endif

	webc_free_string_copy(mpNewUrl, __FILE__, __LINE__);

	if (mpBrowser)
	{
		mpBrowser->PurgeDocumentFromHistory(this);
	}

	webc_free_string_copy(mpTitle, __FILE__, __LINE__);
	webc_free_string_copy(mpScrollToAnchor, __FILE__, __LINE__);

	vector_delete_cb(&mPendingImages, _PendingImageDestructor);

  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
  	if (mpOnlineCacheHandle)
  	{
  		mpOnlineCacheHandle->release();
  	}
  #endif // WEBC_SUPPORT_OFFLINE_BROWSING
}

HTMLElement *HTMLDocument::Head()
{
	return mHead;
}

HTMLBody *HTMLDocument::Body()
{
	return mBody;
}

HTMLElement *HTMLDocument::FindElement(HTMLElementFinder *pFinder, int iSearchFrames)
{
	return (mBody? mBody->FindElement(pFinder, iSearchFrames) : 0);
}

void HTMLDocument::Clear(void)
{
	ClearDocument();
#if (WEBC_SUPPORT_JSCRIPT)
	ClearJavaScript();
#endif

	// All our use counters should be down to zero, so this will free up all
	//  the memory used by bitmaps in this document.
	if (mpImages)
	{
		mpImages->GarbageCollect();
	}
}

void HTMLDocument::ClearDocument(void)
{
	// Since we'll be clearing out the content of this document,
	//  we don't want any queued load jobs to subsequently try to
	//  access one of the HTMLElements (or this HTMLDocument)
	//  that we'll have just deleted. So abort all jobs associated
	//  with this document.
	if (mpBrowser)
	{
		mpBrowser->AbortDocumentChildJobs(this);
	}

	// First send an unload event to all elements in this document.
	if (Body())
	{
		HTMLEvent e;
		e.type = HTML_EVENT_UNLOAD;
		e.data.position.x = 0;
		e.data.position.y = 0;
		ObjectReference<HTMLDocument> self(this);
		Body()->Event(&e);
		if (!self.Get())
		{
			return;
		}
	}

  #if (WEBC_SUPPORT_FRAMES)
	// also send a load event to the root frameset, if there is one
	HTMLNthOfTypeFinder f(HTML_FRAMESET_ELEMENT, 0);
	HTMLFrameSet *frameset = Body()? (HTMLFrameSet *) Body()->FindElement(&f) : 0;
	if (frameset)
	{
		HTMLEvent e;
		e.type = HTML_EVENT_UNLOAD;
		frameset->Event(&e);
	}
  #endif // WEBC_SUPPORT_FRAMES

	if (mHead)
	{
		mHead->DeleteChildren();
	}

	if (mBody)
	{
		mBody->DeleteChildren();
	}
  #if (WEBC_SUPPORT_STYLE_SHEETS)
	if (mpCSSContext)
	{
		mpCSSContext->Clear();
	}
  #endif // WEBC_SUPPORT_STYLE_SHEETS
	if (mBody)
	{
		mBody->RestoreDefaults();
	}

	ClearRefreshTimers();

	SetTitle(WEBC_STR_NULL);

	if (mpName)
	{
		webc_free_string_copy(mpName, __FILE__, __LINE__);
		mpName = 0;
	}
}

int _DoMetaRefresh(void* data, long err)
{
	MetaRefreshInfo* info = (MetaRefreshInfo*) data;
	HTMLDocument* doc = info->doc;

	if (info->url)
	{
		URLDescriptor urlParse(info->url, doc->BaseURL());
		// don't save the current url to the history if protocol is JavaScript,
		//  because javascript:// url's are not really page locations
		if (urlParse.getProtocol() != WEBC_PROTOCOL_JSCRIPT)
		{
			doc->SaveLocation();
		}

		doc->SetSrc(info->url);
		doc->Update(1,0);
	}
	else
	{
		doc->QueueLoad(doc->CurrentURL(), 1);
	}

	return (0);
}

int _FreeMetaRefresh(WEBC_PFBYTE data)
{
	MetaRefreshInfo *info = (MetaRefreshInfo *) data;
	if (info->url)
	{
		webc_free_string_copy(info->url, __FILE__, __LINE__);
	}
	WEBC_FREE(info);
	return (0);
}

int _MatchMetaRefreshInfo(TimeoutInfo* timeout, void* data)
{
	HTMLDocument*    doc  = (HTMLDocument*) data;
	MetaRefreshInfo* info = (MetaRefreshInfo*) timeout->pData;

	return (timeout->Func == (webcTimeoutFn) _DoMetaRefresh && info->doc == doc);
}

void HTMLDocument::ClearRefreshTimers (void)
{
	GetBrowser()->TimeoutManager()->CancelTimeout(_MatchMetaRefreshInfo, this, WEBC_TRUE);
}

void HTMLDocument::SetRefreshTimer (const WEBC_CHAR *url, WEBC_UINT32 msec)
{
	ClearRefreshTimers();

	if (msec == 0)
	{
		if (url)
		{
			SetSrc(url);
			Update(1,0);
		}
	}
	else
	{
		HTMLBrowser* browser = GetBrowser();

		if (browser)
		{
			MetaRefreshInfo *info = (MetaRefreshInfo *) WEBC_MALLOC_VERBOSE(sizeof(MetaRefreshInfo),"MetaRefreshInfo");

			if (info)
			{
				info->url = url? webc_malloc_string_copy(url, __FILE__, __LINE__) : 0;
				info->doc = this;

				WEBC_UINT16 id = browser->TimeoutManager()->SetTimeout (
						(webcTimeoutFn) _DoMetaRefresh,
						_FreeMetaRefresh,
						info,
						msec,
						0 // loop
					);

				if (id == WEBC_TIMEOUT_INVALID)
				{
					if (info->url)
					{
						webc_free_string_copy(info->url, __FILE__, __LINE__);
					}
					WEBC_FREE(info);
				}
			}
		}
	}
}

#if (WEBC_SUPPORT_JSCRIPT)
void HTMLDocument::ClearJavaScript(void)
{
	SetJSObject(0);

	if (mpJSMgr)
	{
		// Won't actually delete until its safe
		mpJSMgr->Destroy();
		mpJSMgr = 0;
	}
}
#endif

WebcImageCollection *HTMLDocument::ImageBitmaps(void)
{
	return (mpImages);
}

void HTMLDocument::SetStatus(WEBC_CHAR *pStr)
{
#if (WEBCFG_ENABLE_STATUS_BAR)
	if (!pStr)
		return;

	if (mpBrowser)
	{
		mpBrowser->SetStatus(pStr);
	}
#endif //WEBCFG_ENABLE_STATUS_BAR
}

WEBC_CHAR * HTMLDocument::Status()
{
	if (mpBrowser)
	{
		return (mpBrowser->GetStatus());
	}

	return 0;
}

void HTMLDocument::SetBrowser(HTMLBrowser *wc)
{
	mpBrowser = wc;
}

HTMLDocument *HTMLDocument::Parent(void)
{
  #if (WEBC_SUPPORT_FRAMES)
	return (mpFrame? mpFrame->GetDocument() : 0);
  #else
	return (0);
  #endif // WEBC_SUPPORT_FRAMES
}

HTMLDocument *HTMLDocument::RootParent(void)
{
	HTMLDocument *pParent = Parent();
	HTMLDocument *pDoc = this;
	while (pParent)
	{
		pDoc = pParent;
		pParent = pParent->Parent();
	}
	return pDoc;
}

int HTMLDocument::FrameDepth (void)
{
	int depth = 0;

	HTMLDocument *pParent = Parent();
	while (pParent)
	{
		depth++;
		pParent = pParent->Parent();
	}

	return depth;
}


void HTMLDocument::SetURL(URLDescriptor* desc)
{
	if (desc)
	{
		mCurrentUrl.set(desc);
		mBaseUrl.set(desc);

		UrlStringBuilder ubld(desc);
		WEBC_CHAR *urlStr = ubld.MallocString(__FILE__, __LINE__);
		if (urlStr)
		{
			HTMLBrowserEvent e;
			e.type = HTML_BROWSER_EVENT_DOCUMENT_URL_CHANGE;
			e.data.string = urlStr;
			e.hdoc = (HDOC_HANDLE) this;

			if (GetBrowser())
			{
				GetBrowser()->TriggerBrowserEvent(&e);
			}

			ubld.FreeString(urlStr, __FILE__, __LINE__);
		}
	}
}

URLDescriptor *HTMLDocument::CurrentURL(void)
{
	return (&mCurrentUrl);
}

URLDescriptor *HTMLDocument::BaseURL(void)
{
	URLDescriptor *baseUrl = &mBaseUrl;
	HTMLElement *pChild;

	if (mHead)
	{
		pChild = mHead->FirstChild();
		while (pChild)
		{
			if (pChild->Type() == HTML_BASE_ELEMENT)
			{
				WEBC_CHAR* url = ((HTMLBase *) pChild)->HRef();
				if (url)
				{
					mBaseUrl.parseURL(url, &mCurrentUrl);
					baseUrl = &mBaseUrl;
					break;
				}
			}
			pChild = pChild->mpNext;
		}
	}

	if ((baseUrl->getProtocol() == WEBC_PROTOCOL_NONE) && Parent())
	{
		return (Parent()->BaseURL());
	}

	return (baseUrl);
}

WEBC_CHAR *HTMLDocument::BaseTarget(void)
{
HTMLElement *pChild;

	if (mHead)
	{
		pChild = mHead->FirstChild();
		while (pChild)
		{
			if (pChild->Type() == HTML_BASE_ELEMENT)
			{
				return (((HTMLBase *) pChild)->Target());
			}
			pChild = pChild->mpNext;
		}
	}

	return (0);
}

void HTMLDocument::ResetCharset(void)
{
	ClearFlag(HDOC_FLAG_STREAM_CHARSET);
	mCharset = WEBC_CHARSET_ASCII; // assume ascii unless otherwise specified
}

void HTMLDocument::SetCharset(WebcCharset charset, HtmlCharsetSource source)
{
	if (charset == WEBC_CHARSET_UNKNOWN)
        return;
    mCharset = charset;
   	if (source == HTML_CHARSET_SRC_STREAM)
   	{
   		SetFlag(HDOC_FLAG_STREAM_CHARSET);
   	}
}

WebcCharset HTMLDocument::GetCharset (void)
{
	return (mCharset);
}

/*****************************************************************************/
// HTMLDocument::Open - open a stream into the document to write HTML source
//
//  returns -1 on failure, 0 otherwise
/*****************************************************************************/

int HTMLDocument::Open(void)
{
	if (!mpDocStream)
	{
		if (this->Body())
		{
			WEBC_NEW_VERBOSE(mpDocStream, HTMLDocumentStream(this->Body()),"HTMLDocumentStream");
		}
		if (!mpDocStream)
		{
			return (-1);
		}
	}

	return 0;
}


/*****************************************************************************/
// HTMLDocument::Refresh -
//
//  returns nothing.
/*****************************************************************************/

void HTMLDocument::Refresh(void)
{
	if (mpManager)
	{
		mpManager->Invalidate();
		mpManager->Refresh();
	}
}


/*****************************************************************************/
// HTMLDocument::Close -
//
//  returns nothing.
/*****************************************************************************/

void HTMLDocument::Close(void)
{

	if (mpDocStream)
	{

		mpDocStream->Close();
		mpDocStream = 0;
		 // Send a load event to the body element because we're closing after feeding it content.
		if (Body())
		{
			HTMLEvent e;
			e.type = HTML_EVENT_LOAD;
			e.data.position.x = 0;
			e.data.position.y = 0;
			Body()->Event(&e);
		}
	}
}


/*****************************************************************************/
// HTMLDocument::IsOpen -
//
//  returns nothing.
/*****************************************************************************/

int HTMLDocument::IsOpen()
{
	if (mpDocStream)
	{
		return 1;
	}
	return 0;
}


/*****************************************************************************/
// HTMLDocument::Write -
//
//  returns nothing.
/*****************************************************************************/

long HTMLDocument::Write(const WEBC_CHAR *text, HTMLParseContext* parser)
{
	if (mpDocStream)
	{
		return (mpDocStream->Write(text));
	}

	return (-1);
}


/*****************************************************************************/
// HTMLDocument::WriteLine -
//
//  returns nothing.
/*****************************************************************************/

long HTMLDocument::WriteLine(const WEBC_CHAR *text, HTMLParseContext* parser)
{
	if (mpDocStream)
	{
		return (mpDocStream->WriteLine(text));
	}

	return (-1);
}

/*****************************************************************************/
// HTMLDocument::Write -
//
//  returns nothing.
/*****************************************************************************/

long HTMLDocument::Write(const WEBC_CHAR *text, long len)
{
	if (mpDocStream)
	{
		return (mpDocStream->Write(text, len));
	}

	return (-1);
}


/*****************************************************************************/
// HTMLDocument::WriteLine -
//
//  returns nothing.
/*****************************************************************************/

long HTMLDocument::WriteLine(const WEBC_CHAR *text, long len)
{
	if (mpDocStream)
	{
		return (mpDocStream->WriteLine(text, len));
	}

	return (-1);
}

void HTMLDocument::LoadUrl (const WEBC_CHAR* urlStr, WEBC_BOOL saveInHistory)
{
#if (WEBC_SUPPORT_JSCRIPT)
// < EDIT 20081020 - HTMLDocument::LoadUrl - Added test for and execute javascript: url
// If URL type "javascript:" write directly to the document
    {
        WEBC_CHAR* ScriptStart = (WEBC_CHAR *) webc_c_strstr(urlStr, "javascript:");
        if (ScriptStart)
        {
            ScriptStart += 11; /* rtp_strlen("javascript:"); */
            if (*ScriptStart)
            {
                Clear();
                if (Open() == 0)
                {
                    int result = Write(ScriptStart, webc_strlen(ScriptStart));
                    Close();
                    if (result >= 0)
                        Refresh();
                }
            }
            return;
        }
    }
#endif
	if (saveInHistory)
	{
		this->SaveLocation();
	}
	if (!webc_c_strstr(urlStr, "://") && (webc_strlen(urlStr) > 0))
	{
		if (!webc_c_strstr(urlStr, "."))
		{
			WEBC_CHAR* temp = (WEBC_CHAR*) WEBC_MALLOC((webc_strlen(urlStr) + 16)*sizeof(WEBC_CHAR));
			if (temp)
			{
				webc_c_strcpy(temp, "http://www.");
				webc_strcat(temp, urlStr);
				webc_c_strcat(temp, ".com");
				this->SetSrc(temp);
				WEBC_FREE(temp);
			}
			else
			{
				this->SetSrc(urlStr);
			}
		}
		else
		{
			// default to HTTP: protocol
			WEBC_CHAR* temp = (WEBC_CHAR*) WEBC_MALLOC((webc_strlen(urlStr) + 8)*sizeof(WEBC_CHAR));
			if (temp)
			{
				webc_c_strcpy(temp, "http://");
				webc_strcat(temp, urlStr);
				this->SetSrc(temp);
				WEBC_FREE(temp);
			}
			else
			{
				this->SetSrc(urlStr);
			}
		}
	}
	else
	{
		this->SetSrc(urlStr);
	}
	this->Update(1,0);
}

WEBC_CHAR* HTMLDocument::GetTitle()
{
	return (mpTitle);
}

void HTMLDocument::SetTitle(const WEBC_CHAR *str)
{
	if (GetBrowser() && GetBrowser()->GetDocument() == this)
	{
		GetBrowser()->SetTitle(str);
	}

	HTMLBrowserEvent e;
	e.type = HTML_BROWSER_EVENT_DOCUMENT_TITLE_CHANGE;
	e.data.string = str;
	e.hdoc = (HDOC_HANDLE) this;

	if (GetBrowser())
	{
		GetBrowser()->TriggerBrowserEvent(&e);
	}

	WEBC_CHAR *s = webc_malloc_string_copy(str, __FILE__, __LINE__);
	webc_free_string_copy(mpTitle, __FILE__, __LINE__);
	mpTitle = s;
}


#if (WEBC_SUPPORT_JSCRIPT)
WebcJSDocumentContext *HTMLDocument::GetJSMgr()
{
	if (!mpJSMgr && !(GetFlags() & HDOC_FLAG_CREATING_JMANAGER))
	{
		SetFlag(HDOC_FLAG_CREATING_JMANAGER);
		WEBC_NEW_VERBOSE(mpJSMgr, WebcJSDocumentContext(gJSBrowserContext, this),"WebcJSDocumentContext");
		ClearFlag(HDOC_FLAG_CREATING_JMANAGER);
	}
	return (mpJSMgr);
}

JSObject *HTMLDocument::GetJSObject()
{
	if (!mJSObject.Get())
	{
		GetJSMgr();
	}
	return (mJSObject.Get());
}

#endif //(WEBC_SUPPORT_JSCRIPT)

void HTMLDocument::SetName(const WEBC_CHAR *pName)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(pName, __FILE__, __LINE__);
	webc_free_string_copy(mpName, __FILE__, __LINE__);
	mpName = pStr;
}


#if (WEBC_SUPPORT_STYLE_SHEETS)
CSSDocumentContext * HTMLDocument::GetCSSContext(void)
{

	return mpCSSContext;

}
#endif // (WEBC_SUPPORT_STYLE_SHEETS)

#if (WEBC_SUPPORT_STYLE_SHEETS)
CSSValueType HTMLDocument::GetCSSProperty(CSSStyleElement * element, CSSPropertyType type, CSSPropertyValue * value)
{
	if (!mpCSSContext)
	{
		return (CSS_VALUE_UNKNOWN);
	}

	return (mpCSSContext->GetProperty(element, type, value));
}
#endif // (WEBC_SUPPORT_STYLE_SHEETS)


void HTMLDocument::SetSrc(const WEBC_CHAR *pNewUrl, URLDescriptor *pBaseUrl)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(pNewUrl, __FILE__, __LINE__);
	webc_free_string_copy(mpNewUrl, __FILE__, __LINE__);
	mpNewUrl = pStr;
	if (pBaseUrl)
	{
		mBaseUrl.set(pBaseUrl);
	}
	ResetCharset();
	ClearFlag(HDOC_FLAG_SCROLL_POSITION_SET);
}


void HTMLDocument::SetSrc(const WEBC_CHAR *pNewUrl, URLDescriptor *pBaseUrl, DISPLAY_INT iNewX, DISPLAY_INT iNewY)
{
	SetSrc (pNewUrl, pBaseUrl);
	SetDefaultScrollPosition (iNewX, iNewY);
}


void HTMLDocument::SetDefaultScrollPosition(DISPLAY_INT iNewX, DISPLAY_INT iNewY)
{
	miNewX = iNewX;
	miNewY = iNewY;
	SetFlag(HDOC_FLAG_SCROLL_POSITION_SET);
}



void HTMLDocument::Update(int iAbortCurrent, WEBC_CHAR *referer)
{
	if (mpNewUrl)
	{
		if (FrameDepth() >= WEBC_CFG_MAX_FRAME_DEPTH)
		{
			webc_free_string_copy(mpNewUrl, __FILE__, __LINE__);
			mpNewUrl = 0;
			return;
		}

		// Parse the new url and free the space used by it
		URLDescriptor desc(mpNewUrl, BaseURL());
#if (WEBC_SUPPORT_REFERER_HEADER)
		desc.setReferer(referer);
#endif
		webc_free_string_copy(mpNewUrl, __FILE__, __LINE__);
		mpNewUrl = 0;

		QueueLoad(&desc, iAbortCurrent);
	}
}

int HTMLDocument::QueueLoad (URLDescriptor* urlDesc, int iAbortCurrent)
{
	if (urlDesc->getProtocol() == WEBC_PROTOCOL_NONE)
	{
		return -1;
	}

	if (urlDesc->samePage(CurrentURL()) && webc_strcmp(urlDesc->getAnchor(), CurrentURL()->getAnchor()))
	{
		// Set the current URL & update URL bar
		SetURL(urlDesc);

		// Scroll to the anchor in the url
		if (urlDesc->getAnchor()[0])
		{
			ScrollToAnchor(urlDesc->getAnchor());
		}
		else
		{
			ScrollTo(miNewX, miNewY);
		}

		return 0;
	}

	// NOTE: here is where you do any deciding about whether or not
	//  to really load the new URL.
	// tbd
  #if (WEBC_SUPPORT_HTTPS && 0)

	// if newurl is HTTPS and oldhost != newhost
	//    display "info: entering secure site"
	if ((urlDesc->protocol == WEBC_PROTOCOL_HTTPS) && tc_stricmp(urlDesc->host, CurrentURL()->host))
	{
	}

	// if newurl is non-HTTPS and oldurl is HTTPS
	//    display "warning: leaving secure site"
	if ((urlDesc->protocol != WEBC_PROTOCOL_HTTPS) && (CurrentURL()->protocol == WEBC_PROTOCOL_HTTPS))
	{
	}

  #endif // WEBC_SUPPORT_HTTPS

	// Abort any loading that is currently happening
	if (iAbortCurrent)
	{
		mpBrowser->Stop(this);
	}

	HTMLBrowser *pBrowser = mpBrowser;
	pBrowser->Lock();

	ObjectReference<HTMLDocument> self(this);

  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
	ClearOnlineCache();
  #endif // WEBC_SUPPORT_OFFLINE_BROWSING

	// Now create the load job and queue it up
	WEBC_BOOL doRefresh = WEBC_FALSE;
	LoadJob* pJob;
	WEBC_NEW_VERBOSE(pJob,LoadHTMLDocument(this, urlDesc, mCharset, miNewX, miNewY, CheckFlag(HDOC_FLAG_SCROLL_POSITION_SET)),"LoadHTMLDocument");

	miNewX = 0;
	miNewY = 0;

	if (pJob)
	{
		pBrowser->ProcessLoadJob(pJob, urlDesc, BaseURL(), this);
		doRefresh = WEBC_TRUE;
	}

	pBrowser->UnLock();

	if (self.Get() == 0)
	{
		return -1;
	}

	if (doRefresh)
	{
		// Update the BODY element
		if (Body())
		{
			Body()->Update(0,0);
		}
// rtp_printf("HTMLDocument::QueueLoad: Not calling refresh PMB testing\r\n");
//		Refresh();
	}

	return 0;
}

void HTMLDocument::NotifyLoading(HTMLElement *pElement)
{
	miCurrentJobs++;
}

void HTMLDocument::SetQueuedFocus(void)
{
	if (mFocusElement.Get())
	{
		mFocusElement.Get()->Focus();
	}
	mFocusElement.Set(0);
}

void HTMLDocument::NotifyDone(HTMLElement *pElement)
{
	if (miCurrentJobs > 0)
	{
		if (miCurrentJobs == 1)
		{
			miCurrentJobs = 0;

			SetQueuedFocus();
			ScrollToDefaultAnchor();

			miCurrentJobs = 1;

			if (Body())
			{
				if (!Body()->Loaded())
				{
					HTMLEvent e;
					e.type = HTML_EVENT_LOAD;
					ObjectReference<HTMLDocument> self(this);
					Body()->Event(&e);
					if (!self.Get())
					{
						return;
					}
					Body()->SetLoaded();
				}

				// also send a load event to the root frameset, if there is one
			  #if (WEBC_SUPPORT_FRAMES)
				HTMLNthOfTypeFinder f(HTML_FRAMESET_ELEMENT, 0);
				HTMLFrameSet *frameset = (HTMLFrameSet *) Body()->FindElement(&f);
				if (frameset)
				{
					HTMLEvent e;
					e.type = HTML_EVENT_LOAD;
					frameset->Event(&e);
				}
			  #endif // WEBC_SUPPORT_FRAMES
			}
			// tbd - move the focus to the body if there is no focus element
		}

		miCurrentJobs--;
	}
}

#if (WEBC_SUPPORT_FRAMES)
HTMLFrame *HTMLDocument::GetFrame(void)
{
	return (mpFrame);
}

void HTMLDocument::SetFrame(HTMLFrame *pFrame)
{
	mpFrame = pFrame;
}

#endif // WEBC_SUPPORT_FRAMES

void HTMLDocument::SetFocusElement(HTMLElement *elem)
{
	if (elem->GetDocument() == this)
	{
		//mpFocusElement = elem;
	}
}


#if (WEBC_SUPPORT_JSCRIPT)

void HTMLDocument::SetJSObject(JSObject *obj)
{
	mJSObject.SetPrivate(0);
	mJSObject.Set(obj);
	if (obj)
	{
		mJSObject.SetPrivate(this);
	}
}

WEBC_BOOL HTMLDocument::DefineJSFunction (JSNative func, WEBC_INT32 numArgs, const char* name)
{
	functionInfo i;

	if (ResolveJSFunction(&i.func, &i.numArgs, name, WEBC_FALSE))
	{
		// already defined!
		return (WEBC_FALSE);
	}

	i.func = func;
	i.numArgs = numArgs;
	tc_strncpy(i.name, name, WEBC_JS_CUSTOM_FUNCTION_NAME_LEN-1);
	i.name[WEBC_JS_CUSTOM_FUNCTION_NAME_LEN-1] = 0;

	return (vector_add_node_to_rear(&mJSCustomFunctions, (unsigned char*) &i) >= 0);
}

#if (WEBC_SUPPORT_UNICODE)
WEBC_BOOL HTMLDocument::ResolveJSFunction (
		JSNative* func,
		WEBC_INT32* numArgs,
		const char* name,
		WEBC_BOOL inheritParentFunctions
	)
{
	vector_iterator it;
	functionInfo *holder = (functionInfo*) vector_get_first(&mJSCustomFunctions, &it);

	while (holder)
	{
		if (!tc_strcmp(name, holder->name))
		{
			*func = holder->func;
			*numArgs = holder->numArgs;
			return (WEBC_TRUE);
		}
		holder = (functionInfo*) vector_get_next(&it);
	}

  #if WEBC_SUPPORT_FRAMES
	if (this->IsQuarantined())
	{
		return (WEBC_FALSE);
	}
  #endif // WEBC_SUPPORT_FRAMES

	if (inheritParentFunctions && Parent())
	{
		return (Parent()->ResolveJSFunction(func, numArgs, name, WEBC_TRUE));
	}

	return (WEBC_FALSE);
}
#endif // WEBC_SUPPORT_UNICODE

WEBC_BOOL HTMLDocument::ResolveJSFunction (
		JSNative* func,
		WEBC_INT32* numArgs,
		const WEBC_CHAR* name,
		WEBC_BOOL inheritParentFunctions
	)
{
	vector_iterator it;
	functionInfo *holder = (functionInfo*) vector_get_first(&mJSCustomFunctions, &it);

	while (holder)
	{
		if (!webc_c_strcmp(name, holder->name))
		{
			*func = holder->func;
			*numArgs = holder->numArgs;
			return (WEBC_TRUE);
		}
		holder = (functionInfo*) vector_get_next(&it);
	}

  #if WEBC_SUPPORT_FRAMES
	if (this->IsQuarantined())
	{
		return (WEBC_FALSE);
	}
  #endif // WEBC_SUPPORT_FRAMES

	if (inheritParentFunctions && Parent())
	{
		return (Parent()->ResolveJSFunction(func, numArgs, name, WEBC_TRUE));
	}

	return (WEBC_FALSE);
}

#endif // (WEBC_SUPPORT_JSCRIPT)

void HTMLDocument::SaveLocation(void)
{
DISPLAY_INT x=0,y=0;

	if (GetBrowser())
	{
		if (mpManager)
		{
			x = mpManager->mViewRect.left;
			y = mpManager->mViewRect.top;
		}

		// Add the current location to the history
		UrlStringBuilder ubld(CurrentURL());
		WEBC_CHAR *url_str = ubld.MallocString(__FILE__, __LINE__);
		if (url_str)
		{
			if (url_str[0])
			{
				GetBrowser()->AddToHistory(url_str, x, y, this); // tbd - save frame info
			}
			ubld.FreeString(url_str, __FILE__, __LINE__);
		}
	}
}

WEBC_UINT16 HTMLDocument::GetFlags(void)
{
	return mwFlags;
}

void HTMLDocument::SetFlag(WEBC_UINT16 mask)
{
	mwFlags |= mask;
}

void HTMLDocument::ClearFlag(WEBC_UINT16 mask)
{
	mwFlags &= ~mask;
}

void HTMLDocument::SetDisplayManager (DisplayManager *pManager)
{
	if (mpManager)
	{
		mpManager->SetRoot(0);
	}
	mpManager = pManager;
	if (mpManager)
	{
		mpManager->SetRoot(mBody? mBody->GetDisplayElement() : 0);
	}
}

DisplayManager *HTMLDocument::GetDisplayManager (void)
{
	return (mpManager);
}

WEBC_BOOL HTMLDocument::ScrollToAnchor (const WEBC_CHAR* name)
{
	if (Body())
	{
		HTMLBodyDisplay* bodyDisplay = (HTMLBodyDisplay*) Body()->GetDisplayElement(WEBC_FALSE);

		if (bodyDisplay && !bodyDisplay->NeedsFormat() && mpManager)
		{
			HTMLAnchorByNameFinder f(name);
			HTMLAnchor* anchor = (HTMLAnchor*) Body()->FindElement(&f);

			if (anchor)
			{
				HTMLDisplayElementFinder f(anchor); // find the next display element at or after anchor
				HTMLElement *elem = Body()->FindElement(&f);
				DisplayElement *display = elem? elem->GetDisplayElement(WEBC_FALSE) : 0;

				if (display)
				{
					DISPLAY_INT x = display->mRect.left, y = display->mRect.top;

					if (display->mpParent)
					{
						display->mpParent->GetDisplayPosition(display, &x, &y);
					}

					ScrollTo(0,y);

					webc_free_string_copy(mpScrollToAnchor, __FILE__, __LINE__);
					mpScrollToAnchor = 0;

					return (WEBC_TRUE);
				}
			}

			return (WEBC_FALSE);
		}

		WEBC_CHAR* nameCopy = webc_malloc_string_copy(name, __FILE__, __LINE__);
		webc_free_string_copy(mpScrollToAnchor, __FILE__, __LINE__);
		mpScrollToAnchor = nameCopy;
	}

	return (WEBC_FALSE);
}

void HTMLDocument::ScrollToDefaultAnchor (void)
{
	if (mpScrollToAnchor)
	{
		ScrollToAnchor(mpScrollToAnchor);
		webc_free_string_copy(mpScrollToAnchor, __FILE__, __LINE__);
		mpScrollToAnchor = 0;
	}
	else if (CheckFlag(HDOC_FLAG_SCROLL_POSITION_SET))
	{
		ScrollTo(miNewX, miNewY);
		ClearFlag(HDOC_FLAG_SCROLL_POSITION_SET);
	}
}

void HTMLDocument::ScrollTo (DISPLAY_INT x, DISPLAY_INT y)
{
	if (mpManager)
	{
		WebRect rect(mpManager->mViewRect);
		rect.MoveTo(x,y);
		mpManager->SetViewport(&rect);
	}
}

void HTMLDocument::ScrollBy (DISPLAY_INT x, DISPLAY_INT y)
{
	if (mpManager)
	{
		WebRect rect(mpManager->mViewRect);
		rect.Shift(x,y);
		mpManager->SetViewport(&rect);
	}
}

void HTMLDocument::SetScrollLeft (DISPLAY_INT x)
{
	if (mpManager)
	{
		WebRect rect(mpManager->mViewRect);
		rect.MoveTo(x,rect.top);
		mpManager->SetViewport(&rect);
	}
}

void HTMLDocument::SetScrollTop (DISPLAY_INT y)
{
	if (mpManager)
	{
		WebRect rect(mpManager->mViewRect);
		rect.MoveTo(rect.left,y);
		mpManager->SetViewport(&rect);
	}
}

DISPLAY_INT HTMLDocument::GetScrollLeft (void)
{
	return (mpManager? mpManager->mViewRect.left : 0);
}

DISPLAY_INT HTMLDocument::GetScrollTop (void)
{
	return (mpManager? mpManager->mViewRect.top : 0);
}

#if (WEBC_SUPPORT_STYLE_SHEETS)

void HTMLDocument::StyleSheetUpdate(void)
{
HTMLStyleUpdateFinder f;

	Body()->FindElement(&f);
}

#endif // WEBC_SUPPORT_STYLE_SHEETS

void HTMLDocument::QueueFocus (HTMLElement *element)
{
	mFocusElement.Set(element);
}

void HTMLDocument::LoadImage (const WEBC_CHAR *url, Listener *forWhom, WEBC_BOOL bForceLoad)
{
  #if (WEBC_SUPPORT_IMAGES)
	HTMLBrowser *pBrowser = GetBrowser();

	if (pBrowser)
	{
		URLDescriptor urlDesc(url, BaseURL());

//MA
#if (WEBC_SUPPORT_REFERER_HEADER)
	WEBC_CHAR	* referer;
	UrlStringBuilder ubld(this->CurrentURL());
	referer= ubld.MallocString(__FILE__, __LINE__);
	urlDesc.setReferer(referer);
	WEBC_FREE(referer);
#endif //WEBC_SUPPORT_REFERER_HEADER

		WEBC_CHAR *pKey = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR) * (_GetBitmapKey(0, &urlDesc) + 1));
		if (pKey)
		{
			_GetBitmapKey(pKey, &urlDesc);

			WebcImage *image = (mpImages)? mpImages->FindAndClaim(pKey) : 0;

			RemovePendingImageListener(forWhom);

			if(bForceLoad && image)
			{
				mpImages->Release(image);
				mpImages->Remove(image);
				image = WEBC_NULL;
			}

			if (image)
			{
				NotifyEvent what(NOTIFIER_TYPE_IMAGE, NOTIFY_DONE);
				forWhom->Notify(image, &what);
				image->Release();
			}
			else
			{
				PendingImage *pi = (PendingImage *) vector_find_node(&mPendingImages, _MatchPendingImageKey, (WEBC_PFBYTE) pKey);
				if (pi)
				{
					vector_add_node_to_rear(&pi->listeners, (WEBC_PFBYTE) &forWhom);
				}
				else
				{
					// no such image yet exists; create a record for it
					PendingImage i;

					i.key = webc_malloc_string_copy(pKey, __FILE__, __LINE__);
					if (i.key)
					{
						vector_init(&i.listeners, sizeof(Listener*));
						if (vector_add_node_to_rear(&i.listeners, (WEBC_PFBYTE) &forWhom) == 0)
						{
						    if (vector_add_node_to_rear(&mPendingImages, (WEBC_PFBYTE) &i) == 0)
							{
								pBrowser->Lock();

								WebcLoadImage *pJob;
								WEBC_NEW_VERBOSE(pJob, LoadHTMLDocumentImage(&urlDesc, pKey, this, pBrowser),"LoadHTMLDocumentImage");
								if (pJob)
								{
									pBrowser->ProcessLoadJob(pJob, &urlDesc, BaseURL(), this, bForceLoad);
								  #ifdef WEBC_OLD_LOAD_METHOD
									webc_thread_load_browser(pBrowser);
								  #endif // WEBC_OLD_LOAD_METHOD
								}
								else
								{
									RemovePendingImage(pKey);
								}

								pBrowser->UnLock();
							}
							else
							{
								vector_delete(&i.listeners);
								webc_free_string_copy(i.key, __FILE__, __LINE__);
							}
						}
						else
						{
							webc_free_string_copy(i.key, __FILE__, __LINE__);
						}
					}
				}
			}

			WEBC_FREE(pKey);
		}
	}
  #endif // WEBC_SUPPORT_IMAGES
}

/*******************************************************************************************/
/* Added
/*******************************************************************************************/
#if (WEBC_SUPPORT_EMBED)
void HTMLDocument::LoadEmbed (const WEBC_CHAR *url, Listener *forWhom, WEBC_BOOL bForceLoad)
{
	HTMLBrowser *pBrowser = GetBrowser();

	if (pBrowser)
	{
		URLDescriptor urlDesc(url, BaseURL());

//MA
#if (WEBC_SUPPORT_REFERER_HEADER)
	WEBC_CHAR	* referer;
	UrlStringBuilder ubld(this->CurrentURL());
	referer= ubld.MallocString(__FILE__, __LINE__);
	urlDesc.setReferer(referer);
	WEBC_FREE(referer);
#endif //#if (WEBC_SUPPORT_REFERER_HEADER)


		//Implementation is not complete
		WEBC_CHAR *pKey = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR) * (_Get_embed_elementKey(0, &urlDesc) + 1));
		if (pKey)
		{
			WebcLoadEmbed *pJob;
			//WEBC_NEW(pJob, WebcLoadembed(&urlDesc, pKey, this, pBrowser));  //question regarding this, for  class LoadHTMLDocumentImage : public WebcLoadImage this is "HTMLDocument *pDoc" ?? do I need to pass this, ignoring it for the moment
			WEBC_NEW_VERBOSE(pJob, WebcLoadEmbed(&urlDesc, pKey, pBrowser),"WebcLoadEmbed");
			if (pJob)
			{
				pBrowser->ProcessLoadJob(pJob, &urlDesc, BaseURL(), this, bForceLoad);
				#ifdef WEBC_OLD_LOAD_METHOD
				webc_thread_load_browser(pBrowser);
				#endif // WEBC_OLD_LOAD_METHOD
			}
			else
			{
				rtp_printf("Error creating pJob for embed \n \r");
			}
		}
	}
}

#endif  //WEBC_SUPPORT_EMBED

int _GetBitmapKey(WEBC_CHAR *pKey, URLDescriptor *pUrl)
{
	int iLength = 1; // init to 1 for the null terminator

	if (pKey)
	{
		pKey[0] = '\0';
	}

	switch (pUrl->getProtocol())
	{

#if (WEBC_SUPPORT_INTERNAL)
	case WEBC_PROTOCOL_WEBC:
		iLength += 7;
		if (pKey)
		{
			webc_c_strcat(pKey, "WEBC://");
		}
		break;
#endif	// WEBC_SUPPORT_HTTP

#if (WEBC_SUPPORT_HTTP)
	case WEBC_PROTOCOL_HTTP:
		iLength += 7;
		if (pKey)
		{
			webc_c_strcat(pKey, "HTTP://");
		}
		break;
#endif	// WEBC_SUPPORT_HTTP

#if (WEBC_SUPPORT_HTTPS)
	case WEBC_PROTOCOL_HTTPS:
		iLength += 8;
		if (pKey)
		{
			webc_c_strcat(pKey, "HTTPS://");
		}
		break;
#endif	// WEBC_SUPPORT_HTTPS

#if (WEBC_SUPPORT_FILE)
	case WEBC_PROTOCOL_FILE:
		iLength += 7;
		if (pKey)
		{
			webc_c_strcat(pKey, "FILE://");
		}
		break;
#endif // WEBC_SUPPORT_FILE

#if (WEBC_SUPPORT_LOCAL)
	case WEBC_PROTOCOL_LOCAL:
		iLength += 7;
		if (pKey)
		{
			webc_c_strcat(pKey, "LOCAL://");
		}
		break;
#endif // WEBC_SUPPORT_LOCAL


	default:
		break;
	};

	iLength += webc_strlen(pUrl->getHost());
	if (pKey && (webc_strlen(pUrl->getHost()) != 0))
	{
		webc_strcat(pKey, pUrl->getHost());
	}

	iLength += webc_strlen(pUrl->getPath());
	if (pKey)
	{
		webc_strcat(pKey, pUrl->getPath());
	}

	return (iLength);
}

void HTMLDocument::RemovePendingImageListener (Listener *forWhom)
{
	vector_find_node(&mPendingImages, _RemoveListenerFromPendingImage, (WEBC_PFBYTE) forWhom);
}

void HTMLDocument::RemovePendingImage (const WEBC_CHAR *pKey)
{
	vector_delete_cond_cb(&mPendingImages, _MatchPendingImageKey, (WEBC_PFBYTE) pKey, _PendingImageDestructor);
}

void HTMLDocument::Notify (Notifier *who, NotifyEvent *what)
{
	switch (what->type)
	{
		case NOTIFIER_TYPE_IMAGE:
			switch (what->event)
			{
				case NOTIFY_DONE:
				{
					WebcImage *image = (WebcImage *) who;
					if (image->Key() && mpImages)
					{
						PendingImage *pi = (PendingImage *) vector_find_node(&mPendingImages, _MatchPendingImageKey, (WEBC_PFBYTE) image->Key());

						// add the image to our image collection
						mpImages->Add(image);

						// now notify all listeners who are waiting on this image
						if (pi)
						{
							vector_iterator vi;
							Listener **l = (Listener **) vector_get_first(&pi->listeners, &vi);
							while (l && *l)
							{
								(*l)->Notify(who, what);

								l = (Listener **) vector_get_next(&vi);
							}
						}
					}
					break;
				}

				default:
					break;
			}
			break;

		default:
			break;
	}
}

// ok, so this one is a bit convoluted; this is the most accurate name I
//  can think of - this code is used in many places, so it deserves its
//  own method.
int HTMLDocument::OkayToResetFocusToBody(void)
{
	return 0;
}

WEBC_BOOL HTMLDocument::IsDescendantOf (HTMLDocument* parentDoc)
{
	HTMLDocument* myParent = Parent();

	while (myParent)
	{
		if (parentDoc == myParent)
		{
			return (WEBC_TRUE);
		}
		myParent = myParent->Parent();
	}

	return (WEBC_FALSE);
}

WEBC_BOOL HTMLDocument::IsAncestorOf (HTMLDocument* childDoc)
{
	return childDoc->IsDescendantOf(this);
}

#if (WEBC_SUPPORT_OFFLINE_BROWSING)

CacheHandle* HTMLDocument::GetOnlineCache (void)
{
	if (!mpOnlineCacheHandle)
	{
		WEBC_NEW_VERBOSE(mpOnlineCacheHandle, DocumentContentCacheHandle(),"DocumentContentCacheHandle");

		if (mpOnlineCacheHandle && !mpOnlineCacheHandle->createdOkay())
		{
			WEBC_DELETE(mpOnlineCacheHandle);
			mpOnlineCacheHandle = 0;
		}
	}

	if (mpOnlineCacheHandle)
	{
		mpOnlineCacheHandle->claim();
	}

	return (mpOnlineCacheHandle);
}

void HTMLDocument::ClearOnlineCache (void)
{
	if (mpOnlineCacheHandle && mpOnlineCacheHandle->getCache())
	{
		wcache_clear(mpOnlineCacheHandle->getCache());
	}
}

void HTMLDocument::SaveContentForOffline (WEBC_BOOL createDirEntry)
{
	wcache_ctx* cc = wload_GetOfflineContentCache();
	wcache_ctx* docCache = mpOnlineCacheHandle? mpOnlineCacheHandle->getCache() : 0;
	if (cc && docCache)
	{
		wcache_merge(cc, docCache);

	  #if (WEBC_SUPPORT_FRAMES)
		if (Body())
		{
			HTMLElementTypeFinder frameFinder(HTML_FRAME_ELEMENT);
			HTMLElementTypeFinder iframeFinder(HTML_IFRAME_ELEMENT);
			HTMLElementOrFinder anyFrameFinder(&frameFinder, &iframeFinder);
			HTMLElementNotFinder notFrameFinder(&anyFrameFinder);
			HTMLElementTreeWalker walker(Body(), Body(), &anyFrameFinder, &notFrameFinder);

			HTMLElement* elem = walker.getNext();
			while (elem)
			{
				if (((HTMLFrame*)elem)->FrameDocument())
				{
					((HTMLFrame*)elem)->FrameDocument()->SaveContentForOffline(WEBC_FALSE);
				}
				elem = walker.getNext();
			}
		}
	  #endif /* WEBC_SUPPORT_FRAMES */

		if (createDirEntry)
		{
			// Update the offline cache directory file
			wcache_stream wcs, check;
			vector entryList;
			vector_iterator vi;
			WebcOfflineCacheDirEntry entry;
			WebcOfflineCacheDirEntry* entryPtr;

			// load in the old file, constructing in-memory list
			if (wcache_open(cc, WEBC_OFFLINE_DIR_KEY, &wcs, WEBC_CO_READ) < 0)
			{
				// maybe the directory just doesn't yet exist; try to create it
				if (wcache_open(cc, WEBC_OFFLINE_DIR_KEY, &wcs, WEBC_CO_CREATE) >= 0)
				{
					// creation successful; write the current URL to the file
					webc_OfflineInitDirEntry(&entry, CurrentURL(), GetTitle());
					webc_OfflineWriteDirEntry(&wcs, &entry);
					wcache_close(&wcs);
				}
				return;
			}

			vector_init(&entryList, sizeof(WebcOfflineCacheDirEntry));
			while (webc_OfflineReadDirEntry(&wcs, &entry) == 0)
			{
				// only add the entries that are still around
				if (wcache_open(cc, entry.key, &check, WEBC_CO_READ) >= 0)
				{
					vector_add_node_to_rear(&entryList, (WEBC_PFBYTE) &entry);
					wcache_close(&check);
				}
			}
			wcache_close(&wcs);

			// find the current URL in the list
			webc_OfflineInitDirEntry(&entry, CurrentURL(), GetTitle());
			entryPtr = (WebcOfflineCacheDirEntry*) vector_get_first(&entryList, &vi);
			while (entryPtr)
			{
				if (!tc_stricmp(entry.key, entryPtr->key))
				{
					break;
				}

				entryPtr = (WebcOfflineCacheDirEntry*) vector_get_next(&vi);
			}

			// if its not there, add it
			if (!entryPtr)
			{
				vector_add_node_to_front(&entryList, (WEBC_PFBYTE) &entry);
			}

			// now open the directory for writing and write
			//  each URL in the list to it
			if (wcache_open(cc, WEBC_OFFLINE_DIR_KEY, &wcs, WEBC_CO_CREATE) >= 0)
			{
				entryPtr = (WebcOfflineCacheDirEntry*) vector_get_first(&entryList, &vi);
				while (entryPtr)
				{
					if (webc_OfflineWriteDirEntry(&wcs, entryPtr) < 0)
					{
						break;
					}

					entryPtr = (WebcOfflineCacheDirEntry*) vector_get_next(&vi);
				}
				wcache_close(&wcs);
			}

			vector_delete(&entryList);
		}

		wcache_flush(cc);
	}
}

#endif // WEBC_SUPPORT_OFFLINE_BROWSING

extern "C" {
int _MatchPendingImageKey(WEBC_PFBYTE data, WEBC_PFBYTE param)
{
	return (webc_strcmp(((PendingImage *) data)->key, (WEBC_CHAR *) param) == 0);
}

int _MatchListener(WEBC_PFBYTE data, WEBC_PFBYTE p)
{
	Listener **a = (Listener **) data, *b = (Listener *) p;
	return (a && (a[0] == b));
}

int _RemoveListenerFromPendingImage(WEBC_PFBYTE data, WEBC_PFBYTE p)
{
	PendingImage *pi = (PendingImage *) data;
	return (vector_delete_cond(&pi->listeners, _MatchListener, p) == 0);
}

void _PendingImageDestructor (WEBC_PFBYTE data)
{
	PendingImage *pi = (PendingImage *) data;

	webc_free_string_copy(pi->key, __FILE__, __LINE__);
	vector_delete(&pi->listeners);
}
}


/*******************************************************************************************************/
/* Added to support embed tage, can use int _GetBitmapKey(WEBC_CHAR *pKey, URLDescriptor *pUrl)		   */
/*  except that I don't like the name							                             MA		   */
/*******************************************************************************************************/

#if (WEBC_SUPPORT_EMBED)
int _Get_embed_elementKey(WEBC_CHAR *pKey, URLDescriptor *pUrl)
{
	int iLength = 1; // init to 1 for the null terminator

	if (pKey)
	{
		pKey[0] = '\0';
	}

	switch (pUrl->getProtocol())
	{

#if (WEBC_SUPPORT_INTERNAL)
	case WEBC_PROTOCOL_WEBC:
		iLength += 7;
		if (pKey)
		{
			webc_c_strcat(pKey, "WEBC://");
		}
		break;
#endif	// WEBC_SUPPORT_HTTP

#if (WEBC_SUPPORT_HTTP)
	case WEBC_PROTOCOL_HTTP:
		iLength += 7;
		if (pKey)
		{
			webc_c_strcat(pKey, "HTTP://");
		}
		break;
#endif	// WEBC_SUPPORT_HTTP

#if (WEBC_SUPPORT_HTTPS)
	case WEBC_PROTOCOL_HTTPS:
		iLength += 8;
		if (pKey)
		{
			webc_c_strcat(pKey, "HTTPS://");
		}
		break;
#endif	// WEBC_SUPPORT_HTTPS

#if (WEBC_SUPPORT_FILE)
	case WEBC_PROTOCOL_FILE:
		iLength += 7;
		if (pKey)
		{
			webc_c_strcat(pKey, "FILE://");
		}
		break;
#endif // WEBC_SUPPORT_FILE

#if (WEBC_SUPPORT_LOCAL)
	case WEBC_PROTOCOL_LOCAL:
		iLength += 7;
		if (pKey)
		{
			webc_c_strcat(pKey, "LOCAL://");
		}
		break;
#endif // WEBC_SUPPORT_LOCAL


	default:
		break;
	};

	iLength += webc_strlen(pUrl->getHost());
	if (pKey && (webc_strlen(pUrl->getHost()) != 0))
	{
		webc_strcat(pKey, pUrl->getHost());
	}

	iLength += webc_strlen(pUrl->getPath());
	if (pKey)
	{
		webc_strcat(pKey, pUrl->getPath());
	}

	return (iLength);
}

#endif
/*******************************************************************************************************/
/* Added to support embed tage, can use								MA								   */
/*******************************************************************************************************/
