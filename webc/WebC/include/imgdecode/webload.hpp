//
// WEBLOAD.HPP - Prototypes and defines for WEBLOAD.CPP
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __WEBLOAD_HPP__
#define __WEBLOAD_HPP__

#include "webc.h"
#include "httpc.h"
#include "webcfile.h"
#include "vector.h"
#include "css.h"
#if (WEBC_SUPPORT_OFFLINE_BROWSING)
 #include "wcache.h"
#endif
#ifdef __cplusplus
 #include "himage.hpp"
 #include "hobject.hpp"
 #include "hflow.hpp"
 #include "htable.hpp"
 #include "hscript.hpp"
 #include "webcbmp.hpp"
 #include "strbld.hpp"
 #include "webimage.hpp"
#endif // __cplusplus

#include "urldesc.hpp"


typedef struct s_WebcSocketStream
{
	HTTPStreamClass* base;
	WEBC_SOCKET_t    socket;
	webcIdleFn       idle_func;
	WEBC_PFBYTE      idle_data;
}
WebcSocketStream;

typedef struct s_WebcSocketStreamFactory
{
	HTTPStreamFactoryClass* base;
	webcIdleFn              idle_func;
	WEBC_PFBYTE             idle_data;
}
WebcSocketStreamFactory;

#if (WEBC_SUPPORT_HTTPS)
typedef struct s_WebcSSLStream
{
	HTTPStreamClass* base;
	WEBC_SOCKET_t    socket;
	RTP_SSL_SESSION  ssl;
	WEBC_BOOL        created;
	webcIdleFn       idle_func;
	WEBC_PFBYTE      idle_data;
}
WebcSSLStream;

typedef struct s_WebcSSLStreamFactory
{
	HTTPStreamFactoryClass* base;
	webcIdleFn              idle_func;
	WEBC_PFBYTE             idle_data;
}
WebcSSLStreamFactory;
#endif // WEBC_SUPPORT_HTTPS

#if (WEBC_SUPPORT_OFFLINE_BROWSING)
class CacheHandle;
#endif

#define WLOAD_REPORT_STATUS       0

#define URLSTREAM_CLOSED          0
#define URLSTREAM_OPEN_RDONLY     1
#define URLSTREAM_OPEN_WRONLY     2
#define URLSTREAM_OPEN_RDWR       3
#define URLSTREAM_ERROR           4

typedef struct
{
	WEBC_UINT8 mode;
	WebcFileContentType data_type;
	URLDescriptor *p_desc;

	webcIdleFn   idle_func;

	WEBC_PFBYTE  idle_data;

	webcReportFn report_func;
	WEBC_PFBYTE  report_ctx;

  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
	CacheHandle* onlineCache;
	struct wcache_stream wcs;
	WEBC_BOOL readFromCache;
	WEBC_BOOL saveToCache;
  #endif // WEBC_SUPPORT_OFFLINE_BROWSING

	union
	{
	  #if (WEBC_SUPPORT_HTTP)
		struct {
			struct httpc_ctx ctx;
			WebcSocketStreamFactory factory;
			char stop;
		} http;
	  #endif	// WEBC_SUPPORT_HTTP

	  #if (WEBC_SUPPORT_HTTPS)
		struct {
			struct httpc_ctx ctx;
			WebcSSLStreamFactory factory;
			char stop;
		} https;
	  #endif	// WEBC_SUPPORT_HTTP

	  #if (WEBC_SUPPORT_FILE)
		WEBC_FILE file;
	  #endif	// WEBC_SUPPORT_FILE

	 #if (WEBC_SUPPORT_LOCAL)
		void	*local;
	  #endif	// WEBC_SUPPORT_LOCAL

	  #if (WEBC_SUPPORT_FTP)
		WEBC_PFBYTE buffer;
	  #endif  // WEBC_SUPPORT_FTP

	  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
		struct {
			char* buffer;
			char done;
			int unread_start;
			int unread_size;
			int mime_filter;
			int count;
			struct wcache_stream dirFile;
		} offline;
	  #endif // WEBC_SUPPORT_OFFLINE_BROWSING

		struct {
			void *vfileTable;
			int  vfile;
			long pos;
		} webc;

	} stream;

	RTP_TIMESTAMP* p_date;
	RTP_TIMESTAMP date;
}
UrlStreamCtx;


typedef int  (*wload_GetFn)(UrlStreamCtx *pStream);
typedef int  (*wload_PostFn)(UrlStreamCtx *pStream, WEBC_CHAR *pContentType, long lContentLength);
typedef long (*wload_ReadFn)(UrlStreamCtx *pStream, char *buffer, long length);
typedef long (*wload_WriteFn)(UrlStreamCtx *pStream, char *buffer, long length);
typedef WebcFileContentType  (*wload_DataTypeFn)(UrlStreamCtx *pStream);
typedef int  (*wload_CloseFn)(UrlStreamCtx *pStream);
typedef enum WebcCharset (*wload_CharsetFn)(UrlStreamCtx *pStream);

struct WebcProtocolEntry
{
	char *name;
	WEBC_UINT16 port;
	char online;

	wload_GetFn       prot_Get;
	wload_PostFn      prot_Post;
	wload_ReadFn      prot_Read;
	wload_WriteFn     prot_Write;
	wload_DataTypeFn  prot_DataType;
	wload_CloseFn     prot_Close;
	wload_CharsetFn   prot_Charset;
};

extern const struct WebcProtocolEntry gProtocolTable[];

#define NUM_PROTOCOLS  11

typedef long (*UrlWaitCallBack)(UrlStreamCtx *pStream, WEBC_PFBYTE pContext);

#ifdef __cplusplus

class LoadJob;

typedef enum
{
	LOAD_JOB_STATE_CREATED = 0,
	LOAD_JOB_STATE_WAITING_FOR_STREAM,
	LOAD_JOB_STATE_WAITING_TO_EXECUTE,
	LOAD_JOB_STATE_PROCESSING_STREAM_STATE,
	LOAD_JOB_STATE_EXECUTING,
	LOAD_JOB_STATE_WAITING_FOR_DATA,
	LOAD_JOB_STATE_PROCESSING_DATA,
	LOAD_JOB_STATE_ABORTED,
	LOAD_JOB_STATE_ERROR,
	LOAD_JOB_STATE_DONE,
	NUM_LOAD_JOB_STATES
}
LoadJobState;

// if a LoadJob is in the LOAD_JOB_STATE_ERROR state, then
//  LoadJob::GetLastError() should return one of the following:
#define LOAD_JOB_OK                     0
#define LOAD_JOB_ERR_NETWORK_INIT      -1
#define LOAD_JOB_ERR_HOST_NOT_FOUND    -2
#define LOAD_JOB_ERR_CONNECT_FAIL      -3
#define LOAD_JOB_ERR_POST_FAIL         -4
#define LOAD_JOB_ERR_READ_FAIL         -5
#define LOAD_JOB_ERR_FILE_NOT_FOUND    -6

int  wload_StopWaitingForStream(LoadJob *pJob);

/******************************************************************************
* interface LoadJob
*
*  Purpose - this interface is used as an abstraction to queue URL load jobs
*    it can be subclassed to implement a specific load procedure
*
*  Methods -
*    virtual long Execute (void) - Perform the load job
*    void Abort (void) - Cancel this job
*    int Aborted (void) - returns boolean; whether or not the job has been
*                         cancelled
*
*  Data -
*    miAborted - boolean, whether the job has been aborted
*
******************************************************************************/

class LoadJob
{
public:
	LoadJob (void) { miAborted = 0; }
	virtual ~LoadJob(void) { wload_StopWaitingForStream(this); }

	virtual long Execute (void) = 0;
	virtual int SetStream(UrlStreamCtx *pStream) = 0;
	virtual int WaitForData (void) { return -1; }
	virtual WEBC_SSIZE ProcessData (WEBC_UINT8* data, WEBC_SSIZE len,
		                            WebcFileContentType contentType,
									enum WebcCharset charEncoding) { return -1; }

	virtual void Abort (void) { miAborted = 1; }
	int Aborted (void) { return (miAborted); }

	virtual HTMLBrowser*  GetBrowser(void) = 0;
	virtual HTMLDocument* GetDocument(void) = 0;
	virtual HTMLElement*  GetElement(void) = 0;

protected:
	int miAborted;
};



/******************************************************************************
* class LoadHTMLDocument (implements LoadJob)
*
*  Purpose - used to load data into an HTMLDocument (using GET method)
*
*  Methods - (see LoadJob)
*
*  Data -
*
******************************************************************************/

class LoadHTMLDocument : public LoadJob
{
public:
	LoadHTMLDocument(void);
	LoadHTMLDocument(HTMLDocument *pDocument, URLDescriptor *pUrl, WebcCharset charset,
	                 DISPLAY_INT x=0, DISPLAY_INT y=0, WEBC_BOOL scrollPosSet = WEBC_FALSE);
	virtual ~LoadHTMLDocument();

	// LoadJob interface
	virtual long Execute(void);
	virtual int SetStream(UrlStreamCtx *pStream);

	virtual HTMLBrowser*  GetBrowser(void)  { return mpBrowser; }
	virtual HTMLDocument* GetDocument(void) { return mDocRef.Get(); }
	virtual HTMLElement*  GetElement(void)  { return 0; }

	virtual void Abort (void);

protected:
	// Job specific data
	ObjectReference<HTMLDocument> mDocRef;
	URLDescriptor   mUrl;
	UrlStreamCtx*   mpStream;
	WEBC_BOOL       mbStreamOpen;
	WEBC_BOOL		mbStreamOpenFailed;
	WEBC_BOOL       mResumeRefresh;
	HTMLBrowser*    mpBrowser;
	DISPLAY_INT     miScrollX;
	DISPLAY_INT     miScrollY;
	WebcCharset     mCharset;
	WEBC_BOOL       mGeneratedLoadEvent;
	WEBC_BOOL       mScrollPositionSet;

	WEBC_BOOL ClearDocumentAndOpen (void);
};


/******************************************************************************
* class PostHTMLDocument (implements LoadJob)
*
*  Purpose - used to load data into an HTMLDocument using POST method
*
*  Methods - (see LoadJob)
*
*  Data -
*
******************************************************************************/

class PostHTMLDocument : public LoadHTMLDocument
{
public:
	PostHTMLDocument(HTMLDocument *pDocument, URLDescriptor *pUrl, WEBC_CHAR *pType,
	                 char *pPost, long lLen, WebcCharset charset);
	virtual ~PostHTMLDocument();

	virtual int SetStream(UrlStreamCtx *pStream);

protected:
	WEBC_CHAR* mpContentType;
	char*      mpPostData;
	long       mlContentLength;
};

#if (WEBC_SUPPORT_POSTFILE)
/******************************************************************************
* class PostUploadRawFile (implements LoadJob)
*
*  Purpose - used to load data from a file to an HTMLDocument using POST method.
*            Does not perform mime encapsulation
*
*  Methods - (see LoadJob)
*
*  Data -
*
******************************************************************************/

class PostUploadRawFile : public LoadHTMLDocument
{
public:
	PostUploadRawFile(HTMLDocument *pDocument, URLDescriptor *pUrl, WEBC_CHAR *pContentType,
								   char *pPostApp, long queryLen, char* path, WebcCharset charset);
	virtual ~PostUploadRawFile();

	virtual int SetStream(UrlStreamCtx *pStream);
	virtual long Execute(void) {return 0;};
//MA fix upload issue, where we write to the uploaded file,thus deleting int, need to call a refresh after succesufuly uploading the file
	// LoadJob interface
	

protected:
	WEBC_CHAR * 		mpContentType;
	WEBC_FILE			mFileHandle;
	WEBC_BOOL       	mFileIsOpen;
//Experemintal
	WEBC_BOOL       mResumeRefresh;
//Experemintal

	char*      			mpPostApp;
	long        		mpPostAppLen;  
	
	long       			mlContentLength;
};

//MA
//Separating the PostUploadRawFile and PostUploadDataBuffer  
class PostUploadDataBuffer : public LoadHTMLDocument
{
public:
	//PostUploadRawFile(HTMLDocument *pDocument, URLDescriptor *pUrl, WEBC_CHAR *pContentType,char *pPostApp, long queryLen, char* path, char *pPostData, long pPostDataSize, WebcCharset charset);
	PostUploadDataBuffer(HTMLDocument *pDocument, URLDescriptor *pUrl, WEBC_CHAR *pContentType,
		char *pPostApp, long queryLen, char *pPostData, long pPostBufferSize, WebcCharset charset);
	virtual ~PostUploadDataBuffer();

	virtual int SetStream(UrlStreamCtx *pStream);

protected:
	WEBC_CHAR * 		mpContentType;
//	WEBC_FILE			mFileHandle;
//	WEBC_BOOL       	mFileIsOpen;
	char*      			mpPostData;

	char*      			mpPostApp;
	long        		mpPostAppLen;  
	
	long       			mlContentLength;
};

#endif






/******************************************************************************
* class WebcLoadImage (implements LoadJob)
*
*  Purpose - loads a WebcBitmap from GIF or JPEG data; serves as base class
*            for LoadHTMLImage and LoadBackgroundImage
*
*  Methods - (see LoadJob)
*
*  Data -
*
******************************************************************************/

class WebcLoadImage : public LoadJob
{
public:
#if (WEBC_SUPPORT_DITHERING)
	WebcLoadImage(URLDescriptor *pUrl, WEBC_CHAR *key, HTMLBrowser *pBrowser, WebcImageDitherMode mode = WEBIMAGE_ERROR_DIFFUSION);
#else
	WebcLoadImage(URLDescriptor *pUrl, WEBC_CHAR *key, HTMLBrowser *pBrowser, WebcImageDitherMode mode = WEBIMAGE_NO_DITHERING);
#endif
	virtual ~WebcLoadImage();

	virtual int SetStream(UrlStreamCtx *pStream);

	// LoadJob interface
	virtual long Execute(void);

	virtual HTMLBrowser  *GetBrowser(void)  { return mpBrowser; }
	virtual HTMLDocument *GetDocument(void) { return (0); }
	virtual HTMLElement  *GetElement(void)  { return (0); }
	//virtual void Abort (void);

	virtual void SetBrowser(HTMLBrowser * b) { mpBrowser = b; }

	WebcImage *Image (void);

protected:
	// Job specific data
	WebcImageDitherMode mConvertMode;
	URLDescriptor       mUrl;
	HTMLBrowser*        mpBrowser;
	WebcImage*          mpImage;
	WEBC_CHAR*          mpKey;
	UrlStreamCtx*       mpStream;
	WEBC_BOOL           mbStreamOpen;
	WEBC_BOOL			mbStreamOpenFailed;
#if (WEBC_CACHE_IMAGES)
	static int WebcLoadImage::miBitmapCacheInitialized;
	static wcache_ctx WebcLoadImage::mpBitmapCache[1];
#endif
};

#if (WEBC_SUPPORT_EMBED)
/******************************************************************************
* class WebcLoadembed (implements LoadJob)
*
*
*
*  Methods - (see LoadJob)
*
*  Data -
*
******************************************************************************/

class WebcLoadEmbed : public LoadJob
{
public:
#if (WEBC_SUPPORT_DITHERING)
	WebcLoadEmbed(URLDescriptor *pUrl, WEBC_CHAR *key, HTMLBrowser *pBrowser);
#else
	WebcLoadImage(URLDescriptor *pUrl, WEBC_CHAR *key, HTMLBrowser *pBrowser,);
#endif
	virtual ~WebcLoadEmbed();

	virtual int SetStream(UrlStreamCtx *pStream);

	// LoadJob interface
	virtual long Execute(void);

	virtual HTMLBrowser  *GetBrowser(void)  { return mpBrowser; }
	virtual HTMLDocument *GetDocument(void) { return (0); }
	virtual HTMLElement  *GetElement(void)  { return (0); }
	//virtual void Abort (void);

	virtual void SetBrowser(HTMLBrowser * b) { mpBrowser = b; }

	/*WebcImage *Image (void); *///Image specific

protected:
	// Job specific data
	//WebcImageDitherMode mConvertMode;//Image specific
	URLDescriptor       mUrl;
	HTMLBrowser*        mpBrowser;
	//WebcImage*          mpImage;//Image specific
	WEBC_CHAR*          mpKey;
	UrlStreamCtx*       mpStream;
	WEBC_BOOL           mbStreamOpen;
	WEBC_BOOL			mbStreamOpenFailed;
#if (WEBC_CACHE_IMAGE)
	//static int WebcLoadImage::miBitmapCacheInitialized; //Image specific
	//static wcache_ctx WebcLoadImage::mpBitmapCache[1];  //Image specific
#endif
};

#endif
class LoadHTMLDocumentImage : public WebcLoadImage
{
protected:
	ObjectReference<HTMLDocument> mDocRef;

public:
  #if (WEBC_SUPPORT_DITHERING)
	LoadHTMLDocumentImage(URLDescriptor *pUrl, WEBC_CHAR *key, HTMLDocument *pDoc,
	    HTMLBrowser *pBrowser, WebcImageDitherMode mode = WEBIMAGE_ERROR_DIFFUSION);
  #else
	LoadHTMLDocumentImage(URLDescriptor *pUrl, WEBC_CHAR *key, HTMLDocument *pDoc,
	    HTMLBrowser *pBrowser, WebcImageDitherMode mode = WEBIMAGE_NO_DITHERING);
  #endif
	virtual ~LoadHTMLDocumentImage();

	// LoadJob interface
	virtual long Execute(void);
	virtual int SetStream(UrlStreamCtx *pStream);

	virtual HTMLDocument *GetDocument(void) { return (mDocRef.Get()); }
};


#if(WEBC_SUPPORT_JSCRIPT)
/******************************************************************************
* class LoadScript (implements LoadJob)
*
*  Purpose - Load JavaScript source from an external URL
*
*  Methods - (see LoadJob)
*
*  Data -
*
******************************************************************************/

class LoadScript : public LoadJob
{
 protected:
	ObjectReference<HTMLElement> mElemRef;
	HTMLDocument*   mpDocument;
	UrlStreamCtx*   mpStream;
	HTMLBrowser*    mpBrowser;
	URLDescriptor   mUrl;
	WEBC_BOOL       mbExecuteScript;
	WebcCharset     mCharset;

 public:
	LoadScript(HTMLScript *pScript, HTMLDocument *pDoc, WebcCharset charset,
	           WEBC_BOOL bExecute = WEBC_FALSE);
	LoadScript(WEBC_CHAR *pHRef, HTMLDocument *pDoc);
	virtual ~LoadScript();

	virtual int SetStream(UrlStreamCtx *pStream);
	virtual long Execute (void);

	virtual HTMLBrowser*  GetBrowser(void)  { return mpBrowser; }
	virtual HTMLDocument* GetDocument(void) { return mpDocument; }
	virtual HTMLElement*  GetElement(void)  { return mElemRef.Get(); }
	virtual void Abort (void);

	URLDescriptor* GetURL (void) { return &mUrl; }
};

#endif //(WEBC_SUPPORT_JSCRIPT)

#if (WEBC_SUPPORT_STYLE_SHEETS)
class CSSLoadExternalSheet : public LoadJob
{
 public:
	CSSLoadExternalSheet();
	CSSLoadExternalSheet(CSSDocumentContext * pCSSContext, URLDescriptor * url, WebcCharset charset);
	~CSSLoadExternalSheet();

	virtual long Execute (void);
	virtual int  SetStream (UrlStreamCtx* stream);
	virtual void Abort (void);

	virtual HTMLBrowser  *GetBrowser(void) { return mpBrowser; }
	virtual HTMLDocument *GetDocument(void) { return (mpCSSContext)? mpCSSContext->GetHTMLDocument() : 0; }
	virtual HTMLElement  *GetElement(void) { return 0; }

 protected:
	WEBC_BOOL            mbStreamOpen;
	WEBC_BOOL			mbStreamOpenFailed;
	URLDescriptor        mUrl;
	CSSDocumentContext * mpCSSContext;
	UrlStreamCtx       * mpURLStream;
	HTMLBrowser        * mpBrowser;
	WebcCharset          mCharset;
	WEBC_BOOL            mResumeRefresh;
};

#endif // (WEBC_SUPPORT_STYLE_SHEETS)



/*****************************************************************************/
// class UrlStringBuilder
/*****************************************************************************/

class UrlStringBuilder : public AggregateStringBuilder
{
public:
	UrlStringBuilder(URLDescriptor *desc);
	virtual ~UrlStringBuilder();

	void SetUrlDesc(URLDescriptor *desc);
};


int  wload_WaitForStream(LoadJob *pJob);

struct LoadJobInfo
{
	LoadJob*      pJob;
	HTMLDocument* ownerDocument;
};
#endif // __cplusplus

#if (WEBC_SUPPORT_OFFLINE_BROWSING)
#define WEBC_OFFLINE_DIR_KEY          "offlineDir"
#define WEBC_OFFLINE_DIR_TITLE_SIZE   128
#define WEBC_OFFLINE_DIR_KEY_SIZE     256

struct WebcOfflineCacheDirEntry
{
	char title[WEBC_OFFLINE_DIR_TITLE_SIZE];
	char key[WEBC_OFFLINE_DIR_KEY_SIZE];
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

// URL Stream API
UrlStreamCtx *wload_StreamAlloc(WEBC_UINT8 bForceAlloc);
void wload_StreamFree(UrlStreamCtx *, WEBC_UINT8 bForceAlloc);
void wload_Init(UrlStreamCtx *p_ctx, webcIdleFn idle_func, WEBC_PFBYTE idle_data);
int  wload_Get(UrlStreamCtx *pStream, URLDescriptor *pUrlDesc, RTP_TIMESTAMP* pModified);
int  wload_Post(UrlStreamCtx *pStream, URLDescriptor *pUrlDesc, WEBC_CHAR *pContentType, long lContentLength);
long wload_Read(UrlStreamCtx *pStream, char *buffer, long length);
long wload_Write(UrlStreamCtx *pStream, char *buffer, long length);
WebcFileContentType wload_DataType(UrlStreamCtx *pStream);
int  wload_Close(UrlStreamCtx *pStream);
int  wload_Mode(UrlStreamCtx *pStream);
void wload_SetReportStatusFn(UrlStreamCtx *pStream, webcReportFn report_fn, WEBC_PFBYTE report_ctx);
enum WebcCharset wload_Charset(UrlStreamCtx *pStream);

// Internal Functions
int _DocumentSetStatus(WEBC_PFBYTE ctx, WEBC_CHAR *string);

#ifdef __cplusplus

#if (WEBC_SUPPORT_OFFLINE_BROWSING)
void wload_SetOnlineContentCache(UrlStreamCtx *pStream, CacheHandle* pCache);
struct wcache_ctx *wload_GetOfflineContentCache(void);

int webc_OfflineInitDirEntry(struct WebcOfflineCacheDirEntry *entry, URLDescriptor *urlDesc, const WEBC_CHAR *title);
int webc_OfflineWriteDirEntry(struct wcache_stream *wcs, struct WebcOfflineCacheDirEntry *entry);
int webc_OfflineReadDirEntry(struct wcache_stream *wcs, struct WebcOfflineCacheDirEntry *entry);
#endif

}
#endif

#endif // __WEBLOAD_HPP__

// END WEBLOAD.H
