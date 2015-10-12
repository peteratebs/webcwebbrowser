//
// WEBLOAD.CPP - Routines to load URL from network or file system.
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

#include "webload.hpp"
#include "webc.h"
#include "webcsock.h"
#include "htmlbld.hpp"
#include "webvfile.hpp"
#if(WEBC_SUPPORT_UNICODE)
#include "webcutf.h"
#endif
#if(WEBC_SUPPORT_HTTP)
#include "prothttp.h"
#endif
#if(WEBC_SUPPORT_HTTPS)
#include "prothssl.h"
#endif
#if(WEBC_SUPPORT_FILE)
#include "protfile.h"
#endif
#if(WEBC_SUPPORT_LOCAL)
#include "protlocal.h"
#endif
#if(WEBC_SUPPORT_FTP)
#include "webftp.h"
#endif
#if(WEBC_SUPPORT_OFFLINE_BROWSING)
#include "protoffl.h"
#endif
#if(WEBC_SUPPORT_JSCRIPT)
#include "wjscript.hpp"
#endif
#include "htmlfind.hpp"
#include "strbld.hpp"
#include "util.h"

#include "webimage.hpp"
#include "webgif.hpp"
#include "webjpeg.hpp"
#include "webpng.hpp"
#include "webmng.hpp"
#include "webbmp.hpp"
#include "webc.h"
#include "hframe.hpp"
#include "htmlbrow.hpp"
#include "webcstr.h"

#if(_EFI_)
#include "mwinidle.hpp"
#include <stdio.h>
#endif

#include "webcassert.h"
#include "objref.cpp"
#include "webcmem.h"
#include "rtptime.h"
#include "wcstream.hpp"


/*****************************************************************************/
// Local Type Definitions
/*****************************************************************************/
#if(WEBC_SUPPORT_ERROR_PAGE_REDIRECT)
static int webc_urlerror2Index(int url_error);
static void getHtmlPageWithRedirectUrl(char *tobuffer, char *RedirectUrl);
extern "C" {	void webc_ClearErrorRedirects(void);}
static char * webc_getErrorRedirect(int url_error);
#endif

#if (WEBC_SUPPORT_URL_MACROS)

#define WEBC_CFG_MAX_URL_MACRO_NAME_LEN   8

struct UrlMacroEntry
{
	WEBC_CHAR name[WEBC_CFG_MAX_URL_MACRO_NAME_LEN+1];
	WebcUrlMacro macro;
};
#endif

struct UrlWait
{
	UrlWaitCallBack callback;
	WEBC_PFBYTE          context;
};

struct ReadCallbackStruct
{
	UrlStreamCtx * stream;
	WEBC_UINT8 * initialBuffer;
	int initialBufferSize;
};

/*****************************************************************************/
// Local Functions
/*****************************************************************************/

int WebcLoadImageReadCallback(char * buffer, long length, void * context);
long webc_SetImageStream(UrlStreamCtx *pStream, WEBC_PFBYTE context);
long webc_SetDocumentStream(UrlStreamCtx *pStream, WEBC_PFBYTE context);
long webc_SetScriptStream(UrlStreamCtx *pStream, WEBC_PFBYTE context);
#if (WEBC_SUPPORT_INTERNAL)
int  wload_WebsGet(UrlStreamCtx *pStream);
int  wload_WebsPost(UrlStreamCtx *pStream, WEBC_CHAR * pContentType, long lContentLength);
long wload_WebsRead(UrlStreamCtx *pStream, char * buffer, long length);
long wload_WebsWrite(UrlStreamCtx *pStream, char * buffer, long length);
WebcFileContentType  wload_WebsDataType(UrlStreamCtx *pStream);
int  wload_WebsClose(UrlStreamCtx *pStream);
#endif // WEBC_SUPPORT_INTERNAL


#if (WEBC_CACHE_IMAGES)
	int _GetBitmapKey(WEBC_CHAR * pKey, URLDescriptor *pUrl); // in htmldoc.cpp
#endif
#include "rtpprint.h"


int _LoadJobPollCycle (WEBC_PFBYTE data)
{
	LoadJob* job = (LoadJob*) data;
	HTMLBrowser* browser = job->GetBrowser();

	if (browser && !job->Aborted())
	{
		int i = 5;
		WEBC_UINT32 startTime = rtp_get_system_msec();
		while ((i > 0) && (rtp_get_system_msec() - startTime < 10))
		{
			i--;
			if (job->Aborted())
			{
				return (-1);
			}

			int result = browser->ProcessInputQueue(WEBC_FALSE);
			if (result == 0)
			{
				break;
			}

			if (result < 0)
			{
				return (-1);
			}
		}

		// MUST be after the above message dispatch code
		if (!job->Aborted())
		{
			return (0);
		}
	}

	return (-1);
}


/*****************************************************************************/
// Static Local Functions
/*****************************************************************************/

static vector     *_GetUrlWaitQueue(void);
#if (WEBC_CACHE_IMAGES)
static long        _wload_cache_read (void *handle, WEBC_UINT8 *buffer, long len);
static long        _wload_cache_write (void *handle, WEBC_UINT8 *buffer, long len);
#endif
extern "C"{
static int         _MatchLoadJobInfo(WEBC_PFBYTE node, WEBC_PFBYTE job);
}

/*****************************************************************************/
// Global Data
/*****************************************************************************/

#ifdef WEBC_DEBUG
FILE *gpUrlLogFile = 0;
#endif

#if (WEBC_CACHE_IMAGES)
int WebcLoadImage::miBitmapCacheInitialized = 0;
wcache_ctx WebcLoadImage::mpBitmapCache[1];
#endif

/*	If you would like to support any protocols that have not been
 *	included, you will want to add it to the following table.
 *	Thereby enabling the url descriptor to aquire the correct port.
 *	If you are to add to this table, remember to add to the values
 *	for webc_loader_ctx.protocol.  You will find a list of the
 *	already implemented protocols in WEBC.H.  If you don't add to
 *	this, an error (overrun) will occur in WEBC.C's webc_get_url_descriptor	*/
const struct WebcProtocolEntry gProtocolTable[NUM_PROTOCOLS] =
{
#if (WEBC_SUPPORT_HTTP)
	{
		"http", 80, 1,
		(wload_GetFn) wload_HttpGet,
		(wload_PostFn) wload_HttpPost,
		(wload_ReadFn) wload_HttpRead,
		(wload_WriteFn) wload_HttpWrite,
		(wload_DataTypeFn) wload_HttpDataType,
		(wload_CloseFn) wload_HttpClose,
		(wload_CharsetFn) wload_HttpCharset
	},
#else
	{"http", 80, 1, 0, 0, 0, 0, 0, 0, 0},
#endif
	{"ftp", 21, 1, 0, 0, 0, 0, 0, 0, 0},
	{"telnet", 20, 0, 0, 0, 0, 0, 0, 0, 0},
#if (WEBC_SUPPORT_FILE)
	{
		"file", 0, 0,
		(wload_GetFn)wload_FileGet,
		(wload_PostFn)wload_FilePost,
		(wload_ReadFn)wload_FileRead,
		(wload_WriteFn)wload_FileWrite,
		(wload_DataTypeFn)wload_FileDataType,
		(wload_CloseFn)wload_FileClose,
		(wload_CharsetFn)wload_FileCharset
	},
#else
	{"file", 0, 0, 0, 0, 0, 0, 0, 0, 0},
#endif
#if (WEBC_SUPPORT_HTTPS)
	{
		"https", 443, 1,
		wload_HttpsGet,
		wload_HttpsPost,
		wload_HttpRead,
		wload_HttpWrite,
		wload_HttpDataType,
		wload_HttpClose,
		wload_HttpCharset
	},
#else
	{"https", 443, 1, 0, 0, 0, 0, 0, 0, 0},
#endif
#if (WEBC_SUPPORT_INTERNAL)
	{
		"webc", 0, 0,
		(wload_GetFn) wload_WebcGet,
		(wload_PostFn)wload_WebcPost,
		(wload_ReadFn)wload_WebcRead,
		(wload_WriteFn)wload_WebcWrite,
		(wload_DataTypeFn)wload_WebcDataType,
		(wload_CloseFn)wload_WebcClose,
		(wload_CharsetFn)wload_WebcCharset
	},
	{
		"webs", 80, 0,
		wload_WebsGet,
		wload_WebsPost,
		wload_WebsRead,
		wload_WebsWrite,
		wload_WebsDataType,
		wload_WebsClose,
		0
	},
#else
	{"webc", 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{"webs", 80, 0, 0, 0, 0, 0, 0, 0, 0},
#endif
	{"javascript", 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{"cache", 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if (WEBC_SUPPORT_OFFLINE_BROWSING)
	{
		"offline", 0, 0,
		(wload_GetFn)wload_OfflineGet,
		(wload_PostFn)wload_OfflinePost,
		(wload_ReadFn)wload_OfflineRead,
		(wload_WriteFn)wload_OfflineWrite,
		(wload_DataTypeFn)wload_OfflineDataType,
		(wload_CloseFn)wload_OfflineClose ,
		(wload_CharsetFn)wload_OfflineCharset
	},
#else
	{"offline", 0, 0, 0, 0, 0, 0, 0, 0, 0},
#endif
#if (WEBC_SUPPORT_LOCAL)
	{
		"local", 0, 0,
		(wload_GetFn)wload_LocalGet,
		(wload_PostFn)wload_LocalPost,
		(wload_ReadFn)wload_LocalRead,
		(wload_WriteFn)wload_LocalWrite,
		(wload_DataTypeFn)wload_LocalDataType,
		(wload_CloseFn)wload_LocalClose,
		(wload_CharsetFn)wload_LocalCharset
	},
#else
	{"local", 0, 0, 0, 0, 0, 0, 0, 0, 0},
#endif
};

#if (WEBC_SUPPORT_OFFLINE_BROWSING)
int giOfflineContentCacheInitialized = 0;
wcache_ctx gOfflineContentCache;
#endif


/*****************************************************************************/
// Static Global Data
/*****************************************************************************/

static int giNumStreams = WEBC_CFG_MAX_STREAMS;
static vector url_wait_queue;
static WEBC_UINT8 url_wait_queue_initialized = 0;
#if (WEBC_SUPPORT_URL_MACROS)
static vector _gUrlMacroVector;
static int _giUrlMacrosInitialized = 0;
#endif


/*****************************************************************************/
// API Function Bodies
/*****************************************************************************/

/******************************************************************************
 * webc_SetDocumentStream - Set the stream for a LoadHTMLDocument object
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   long webc_SetDocumentStream(UrlStreamCtx *pStream, WEBC_PFBYTE pContext);
 *
 * Parameters
 *
 *   UrlStreamCtx *pStream - the stream context to open and read data from
 *   WEBC_PFBYTE pContext - pointer to a LoadHTMLDocument instance
 *
 * Description
 **
 * Returns (long)
 *
 *   0 on success, negative on error
 *
 ******************************************************************************/

long webc_SetDocumentStream(UrlStreamCtx *pStream, WEBC_PFBYTE context)
{
	LoadHTMLDocument *pLoader = (LoadHTMLDocument *) context;

	return pLoader->SetStream(pStream);
}


/******************************************************************************
 * webc_SetImageStream - Set the stream for a WebcLoadImage object
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   long webc_SetImageStream(UrlStreamCtx *pStream, WEBC_PFBYTE pContext);
 *
 * Parameters
 *
 *   UrlStreamCtx *pStream - the stream context to open and read data from
 *   WEBC_PFBYTE pContext - pointer to a WebcLoadImage instance
 *
 * Description
 **
 * Returns (long)
 *
 *   0 on success, negative on error
 *
 ******************************************************************************/

long webc_SetImageStream(UrlStreamCtx *pStream, WEBC_PFBYTE context)
{
	WebcLoadImage *pLoader = (WebcLoadImage *) context;

	return pLoader->SetStream(pStream);

}

#if (WEBC_SUPPORT_JSCRIPT)
long webc_SetScriptStream(UrlStreamCtx* pStream, WEBC_PFBYTE context)
{
	LoadScript *pScript = (LoadScript *)context;
	return pScript->SetStream(pStream);
}
#endif //(WEBC_SUPPORT_JSCRIPT)

/******************************************************************************
 * wload_StreamAlloc() - allocate a stream context
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   UrlStreamCtx *wload_StreamAlloc(void);
 *
 * Parameters
 *
 *   none.
 *
 * Description
 *
 *   This function returns a pointer to a UrlStreamCtx structure, which can
 *   be used to open a url stream.  The total number of these streams in the
 *   system is limited, so we don't open too many files/sockets at once.
 *
 * Returns (UrlStreamCtx *)
 *
 *   The allocated UrlStreamCtx, or WEBC_NULL if no memory/too many streams already
 *   open.
 *
 ******************************************************************************/

UrlStreamCtx *wload_StreamAlloc(WEBC_UINT8 bForceAlloc)
{
	if (giNumStreams <= 0 && !bForceAlloc)
	{
		return (0);
	}

	UrlStreamCtx *pStream = (UrlStreamCtx *) WEBC_MALLOC_VERBOSE(sizeof(UrlStreamCtx),"wload_StreamAlloc");

	if (!pStream)
	{
		return (0);
	}

	if (!bForceAlloc)
	{
		giNumStreams--;
	}

	return (pStream);
}


/******************************************************************************
 * wload_WaitForStream() - Set a function to be called when a UrlStreamCtx is
 *                         freed up.
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   int wload_WaitForStream(UrlWaitCallBack callback, WEBC_PFBYTE context);
 *
 * Parameters
 *
 *   UrlWaitCallBack callback - Pointer to the function to be called when a
 *     stream context is available:
 *
 *       typedef long (*UrlWaitCallBack)(UrlStreamCtx *pStream, WEBC_PFBYTE pContext);
 *
 *   WEBC_PFBYTE context - Context information to pass to the given callback function
 *
 * Description
 *
 * Returns (int)
 *
 *   0 if a stream was immediately available
 *   1 if the callback was put in the stream wait queue
 *
 ******************************************************************************/

int wload_WaitForStream(LoadJob *pJob)
{
	LoadJobInfo jinfo;

	if (giNumStreams > 0)
	{
		pJob->SetStream(wload_StreamAlloc(0));
		return (0);
	}

	jinfo.pJob = pJob;

	if (vector_add_node_to_rear(_GetUrlWaitQueue(), (WEBC_PFBYTE) &jinfo) < 0)
	{
		return (-1);
	}

	return (1);
}
int wload_StopWaitingForStream(LoadJob *pJob)
{
	return (vector_delete_cond(_GetUrlWaitQueue(),  _MatchLoadJobInfo, (WEBC_PFBYTE) pJob));
}

int _MatchLoadJobInfo(WEBC_PFBYTE node, WEBC_PFBYTE job)
{
	return ( ((LoadJobInfo *) node)->pJob == ((LoadJob *) job) );
}


/******************************************************************************
 * wload_StreamFree() - Free a UrlStreamCtx structure
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   void wload_StreamFree(UrlStreamCtx *ctx);
 *
 * Parameters
 *
 *   UrlStreamCtx *ctx - the stream context to free
 *
 * Description
 *
 *   This function first checks to see if there are any functions waiting for
 *   a stream context (see wload_WaitForStream).  If there are, then the
 *   stream context is passed onto the first function in the wait queue.  If
 *   not, then the memory used by this structure is freed and the number of
 *   global streams available is incremented.
 *
 * Returns (void)
 *
 *   nothing.
 *
 ******************************************************************************/

void wload_StreamFree(UrlStreamCtx *ctx, WEBC_UINT8 bForceAlloc)
{
	if (ctx)
	{
		vector_iterator vi;
		struct LoadJobInfo *p_job = (struct LoadJobInfo *) vector_get_first(_GetUrlWaitQueue(), &vi);

	  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
		if (ctx->onlineCache)
		{
			ctx->onlineCache->release();
			ctx->onlineCache = 0;
		}
	  #endif // WEBC_SUPPORT_OFFLINE_BROWSING

		if (bForceAlloc)
		{
			// this stream does not participate in the wait queue
			WEBC_FREE(ctx);
			return;
		}

		// if there are callbacks waiting on a stream context, call them with the one just freed
		if (p_job)
		{
			struct LoadJobInfo job = *p_job;

			// We must delete the waiting job BEFORE the callback because the callback may call
			// wload_StreamFree, which would cause a stack overflow unless we hand it to the next guy.
			vector_delete_node(_GetUrlWaitQueue(), 0);
			job.pJob->SetStream(ctx);
		}
		else
		{
			// otherwise we just add it to the free pool
			giNumStreams++;
			WEBC_FREE(ctx);
		}
	}
}


/******************************************************************************
 * wload_Init() - Initialize a context stream.
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   void wload_Init(UrlStreamCtx *p_ctx);
 *
 * Parameters
 *
 *   UrlStreamCtx *p_ctx - the stream to initialize
 *
 * Description
 *
 *   This function MUST be called AFTER allocating a UrlStreamCtx via
 *   wload_StreamAlloc and BEFORE any subsequent operations are performed
 *   on the stream.
 *
 * Returns (void)
 *
 *   nothing.
 *
 ******************************************************************************/

void wload_Init(UrlStreamCtx *p_ctx, webcIdleFn idle_func, WEBC_PFBYTE idle_data)
{
	tc_memset(p_ctx, 0, sizeof(UrlStreamCtx));
	p_ctx->idle_func = idle_func;
	p_ctx->idle_data = idle_data;

  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
	p_ctx->readFromCache = WEBC_FALSE;
	p_ctx->saveToCache = WEBC_FALSE;
  #endif
}


/******************************************************************************
 * wload_Get() - Open a stream through GET method.
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   int wload_Get(UrlStreamCtx *p_ctx, url_descriptor *pUrlDesc, PEBSTIME pModified);
 *
 * Parameters
 *
 *   UrlStreamCtx *p_ctx - the stream context to open
 *   url_descriptor *pUrlDesc - the url to GET
 *   PEBSTIME pModified - set to return failure if the given url has not been
 *      modified since this date/time.
 *
 * Description
 *
 *   This functions opens a URL for reading data via the GET method.  If the
 *   URL is an HTTP/HTTPS file, then this issues a GET command to the server.  If
 *   the protocol is FILE, then it just opens the file in read-only mode.
 *
 * Returns (int)
 *
 *   0 on success, negative on failure
 *
 ******************************************************************************/



#if(WEBC_SUPPORT_DEBUG_PROFILE)
static char rbuff[128];
char *URLTOSTR(URLDescriptor *pUrldesc)
{
	WEBC_CHAR *b = pUrldesc->getPath();
	WEBC_CHAR *b2, *bLast;
	bLast = b2 = b;
	while (*b2){if (*b2 == '\\' || *b2 == '/')	{ if (*b2+1) bLast = b2+1;	}b2++;}
	webc_to_c_strcpy (rbuff, bLast);
	return(&rbuff[0]);
}
#endif


int wload_Get(UrlStreamCtx *p_ctx, URLDescriptor *pUrlDesc, RTP_TIMESTAMP* pModified)
{
	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
	int result = 0;


	p_ctx->mode = URLSTREAM_CLOSED;
	p_ctx->p_desc = pUrlDesc;
	p_ctx->p_date = 0;
	if (pModified)
	{
		p_ctx->date = *pModified;
		p_ctx->p_date = &p_ctx->date;
	}
	// WEBC_CHAR *b = p_ctx->p_desc->getPath();
	//printf("Path == %S\n", b);

	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"wload_Get",URLTOSTR(pUrlDesc))

	if ((p_ctx->p_desc->getProtocol() >= 0) && (p_ctx->p_desc->getProtocol() < NUM_PROTOCOLS))
	{
	  #if (WEBC_SUPPORT_OFFLINE_BROWSING)

		if (gProtocolTable[p_ctx->p_desc->getProtocol()].online)
		{
		  	// if we're offline and this url is an online type, try to access it from the offline cache
			if (!webc_online())
			{
				wcache_ctx *cc = wload_GetOfflineContentCache();

				if (cc)
				{
					UrlStringBuilder ubld(pUrlDesc);
					WEBC_CHAR *key = ubld.MallocString(__FILE__, __LINE__);
					if (key)
					{
						char *_key = webc_arg_to_char(key);
						if (_key)
						{
							if (wcache_open(cc, _key, &p_ctx->wcs, WEBC_CO_READ) >= 0)
							{
								p_ctx->readFromCache = WEBC_TRUE;
								p_ctx->mode = URLSTREAM_OPEN_RDONLY;
								webc_arg_done(_key);
								ubld.FreeString(key, __FILE__, __LINE__);
								return (0);
							}
							webc_arg_done(_key);
						}
						ubld.FreeString(key, __FILE__, __LINE__);
					}
				}

				return (URL_ERROR_NOT_AVAILABLE);
			}

		  	// if we're online and this url is an online type
			if (p_ctx->onlineCache)
			{
				UrlStringBuilder ubld(pUrlDesc);
				WEBC_CHAR *key = ubld.MallocString(__FILE__, __LINE__);
				if (key)
				{
					char *_key = webc_arg_to_char(key);
					if (_key)
					{
						if (p_ctx->onlineCache && p_ctx->onlineCache->getCache())
						{
							if (wcache_open(p_ctx->onlineCache->getCache(), _key, &p_ctx->wcs, WEBC_CO_CREATE) >= 0)
							{
								p_ctx->saveToCache = WEBC_TRUE;
							}
						}
						webc_arg_done(_key);
					}
					ubld.FreeString(key, __FILE__, __LINE__);
				}
			}
		}

	  #endif // WEBC_SUPPORT_OFFLINE_BROWSING

		if (gProtocolTable[p_ctx->p_desc->getProtocol()].prot_Get)
		{
			result = gProtocolTable[p_ctx->p_desc->getProtocol()].prot_Get(p_ctx);
			if (result >= 0)
			{
				p_ctx->mode = URLSTREAM_OPEN_RDONLY;
			}
		  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
			else
			{
				if (p_ctx->readFromCache || p_ctx->saveToCache)
				{
					p_ctx->readFromCache = 0;
					p_ctx->saveToCache = 0;
					wcache_close(&p_ctx->wcs);
				}
			}
		  #endif // WEBC_SUPPORT_OFFLINE_BROWSING
  		}
	}

	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"wload_Get",URLTOSTR(pUrlDesc))
	return (result);
}


/******************************************************************************
 * wload_Post() - Open a stream through POST method.
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   int wload_Post(UrlStreamCtx *p_ctx, url_descriptor *pUrlDesc,
 *                  PFCHAR pContentType, long lContentLength);
 *
 * Parameters
 *
 *   UrlStreamCtx *p_ctx -
 *   url_descriptor *pUrlDesc -
 *   PFCHAR pContentType -
 *   long lContentLength -
 *
 * Description
 *
 * Returns (int)
 *
 ******************************************************************************/

int wload_Post(UrlStreamCtx *p_ctx, URLDescriptor *pUrlDesc, WEBC_CHAR * pContentType, long lContentLength)
{
	int result = -1;

	if (p_ctx->mode != URLSTREAM_CLOSED)
	{
		return (-1);
	}

	p_ctx->p_desc = pUrlDesc;

	int protocol = p_ctx->p_desc->getProtocol();

	/*	added https to the following as this is 'legal' aswell	*/
	if ( (protocol != WEBC_PROTOCOL_HTTP) &&
	     (protocol != WEBC_PROTOCOL_HTTPS) )
	{
		return (-1);
	}

	if ((protocol >= 0) && (protocol < NUM_PROTOCOLS))
	{
		if (gProtocolTable[protocol].prot_Post)
		{
			result = gProtocolTable[protocol].prot_Post(p_ctx, pContentType, lContentLength);
			if (result >= 0)
			{
				p_ctx->mode = URLSTREAM_OPEN_RDWR;
			}
		}
	}

	return (result);
}


/******************************************************************************
 * wload_Read() - Read data from an open stream.
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   long wload_Read(UrlStreamCtx *p_ctx, PFCHAR buffer, long length);
 *
 * Parameters
 *
 *   UrlStreamCtx *p_ctx - the open stream to read from
 *   PFCHAR buffer - buffer to read data into
 *   long length - size of buffer (maximum bytes to read)
 *
 * Description
 *
 * Returns (long)
 *
 *   number of bytes read if successful, negative error code otherwise
 *
 ******************************************************************************/

long wload_Read(UrlStreamCtx *p_ctx, char * buffer, long length)
{
  #if (WEBC_SUPPORT_OFFLINE_BROWSING)

	if (p_ctx->readFromCache)
	{
		return (wcache_read(&p_ctx->wcs, (WEBC_PFBYTE) buffer, length));
	}

  #endif // WEBC_SUPPORT_OFFLINE_BROWSING

	long bytes_read = 0;
	short redirect_count = 10; // The upper limit on redirections... tbd - put this in webcfg.h

	while (redirect_count > 0)
	{

		if (!(p_ctx->mode & URLSTREAM_OPEN_RDONLY))
		{
			return (-1);
		}

		// Read data (protocol-specific)
		int protocol = p_ctx->p_desc->getProtocol();
		if ((protocol >= 0) && (protocol < NUM_PROTOCOLS))
		{
			if (gProtocolTable[protocol].prot_Read)
			{
				bytes_read = gProtocolTable[protocol].prot_Read(p_ctx, buffer, length);
				if (bytes_read == URL_ERROR_REDIRECT)
				{
					redirect_count--;
					continue;
				}

			  #if (WEBC_SUPPORT_OFFLINE_BROWSING)

				if (p_ctx->saveToCache && (bytes_read > 0))
				{
					struct httpc_ctx *ctx = &p_ctx->stream.http.ctx;
					if (ctx ->flags & HTTPC_FLAG_USE_CACHED_FILE)
						rtp_printf("Writing again, not needed\n");

					wcache_set_data_type(&p_ctx->wcs, wload_DataType(p_ctx));
					wcache_set_charset(&p_ctx->wcs, (char) wload_Charset(p_ctx));
					wcache_write(&p_ctx->wcs, (WEBC_PFBYTE) buffer, bytes_read);
				}

			  #endif // WEBC_SUPPORT_OFFLINE_BROWSING

				return (bytes_read);
			}
		}
		else
		{
			return (-1);
		}
	}

	return (-1);
}

void wload_ResolveCharset (UrlStreamCtx* p_ctx, WebcCharset charset)
{
  #if (WEBC_SUPPORT_OFFLINE_BROWSING)

	if (p_ctx->saveToCache)
	{
		wcache_set_charset(&p_ctx->wcs, (char) charset);
	}

  #endif // WEBC_SUPPORT_OFFLINE_BROWSING
}


/******************************************************************************
 * wload_Write() - Write data to an open stream.
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   long wload_Write(UrlStreamCtx *p_ctx, PFCHAR buffer, long length);
 *
 * Parameters
 *
 *   UrlStreamCtx *p_ctx - the open stream to write to
 *   PFCHAR buffer - buffer of characters to write to stream
 *   long length - number of bytes in buffer
 *
 * Description
 *
 * Returns (long)
 *
 *   number of bytes written if successful, negative error code otherwise
 *
 ******************************************************************************/

long wload_Write(UrlStreamCtx *p_ctx, char * buffer, long length)
{
	long bytes_written = -1;

	if (!(p_ctx->mode & URLSTREAM_OPEN_WRONLY))
	{
		return (-1);
	}

	int protocol = p_ctx->p_desc->getProtocol();
	if ((protocol >= 0) && (protocol < NUM_PROTOCOLS))
	{
		if (gProtocolTable[protocol].prot_Write)
		{
			bytes_written = gProtocolTable[protocol].prot_Write(p_ctx, buffer, length);
		}
	}

	return (bytes_written);
}

#if (WEBC_SUPPORT_POSTFILE)
/******************************************************************************
 * wload_WriteFile() - Write data from a file to an open stream.
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 * static long wload_WriteFile(UrlStreamCtx *p_ctx, WEBC_FILE file_handle, long filelength)
 *
 * Parameters
 *
 *   UrlStreamCtx *p_ctx - the open stream to write to
 *   WEBC_FILE file_handle - Handle to an open file file pointer is at zero.
 *   long length - number of bytes in the file
 *
 * Description
 *
 * Returns (long)
 *
 *   number of bytes written if successful, negative error code otherwise
 *
 ******************************************************************************/

static long wload_WriteFile(UrlStreamCtx *p_ctx, WEBC_FILE file_handle, long filelength)
{
	long total_bytes_written = -1;
	if (!(p_ctx->mode & URLSTREAM_OPEN_WRONLY))
	{
		return (-1);
	}

	int protocol = p_ctx->p_desc->getProtocol();
	if ((protocol >= 0) && (protocol < NUM_PROTOCOLS))
	{
		if (gProtocolTable[protocol].prot_Write)
		{
		long bytes_read,bytes_left,bytes_left_inbuffer;
		unsigned char *DataBuffer,*pData;
			DataBuffer	= (unsigned char *) WEBC_MALLOC(WEBC_CFG_FILE_POST_BUFFER_SIZE);
			if (!DataBuffer)
				return (-1);
			bytes_left = filelength;
			bytes_left_inbuffer = 0;
			pData = DataBuffer;
			while (bytes_left > 0)
			{
				if (bytes_left_inbuffer == 0)
				{
				long max_read_size;
					max_read_size = EBSMIN(bytes_left, WEBC_CFG_FILE_POST_BUFFER_SIZE);
					bytes_left_inbuffer = bytes_read = webc_fread (file_handle, (char *) DataBuffer, max_read_size, 0, 0);
					if (bytes_read <= 0)
						break;
				}
				int bytes_sent = gProtocolTable[protocol].prot_Write(p_ctx, (char *) pData, bytes_left_inbuffer);
				if (bytes_sent > 0)
				{
					bytes_left_inbuffer -= bytes_sent;
					bytes_left -= bytes_sent;
					if (bytes_left_inbuffer == 0)
						pData = DataBuffer;
					else
						pData += bytes_sent;
					if (total_bytes_written == -1)
						total_bytes_written = bytes_sent;
					else
						total_bytes_written += bytes_sent;
				}
				else
					break;
			}
			WEBC_FREE(DataBuffer);
		}
	}

	return (total_bytes_written);
}

#endif


/******************************************************************************
 * wload_DataType() - Get the data type of an open stream
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   int wload_DataType(UrlStreamCtx *p_ctx);
 *
 * Parameters
 *
 *   UrlStreamCtx *p_ctx - the stream whose data type to return
 *
 * Description
 *
 * Returns (int)
 *
 *   One of the following:
 *
 *     HTML_MIME_TYPE_UNKNOWN
 *     HTML_MIME_TYPE_HTML
 *     HTML_MIME_TYPE_GIF
 *     HTML_MIME_TYPE_JPEG
 *     HTML_MIME_TYPE_PNG
 *     HTML_MIME_TYPE_MNG
 *     HTML_MIME_TYPE_PLAIN_TEXT
 *
 ******************************************************************************/

WebcFileContentType wload_DataType(UrlStreamCtx *p_ctx)
{
  #if (WEBC_SUPPORT_OFFLINE_BROWSING)

	if (p_ctx->readFromCache)
	{
		return ((WebcFileContentType) wcache_get_data_type(&p_ctx->wcs));
	}

  #endif // WEBC_SUPPORT_OFFLINE_BROWSING

	int protocol = p_ctx->p_desc->getProtocol();
	if ((protocol >= 0) && (protocol < NUM_PROTOCOLS))
	{
		if (gProtocolTable[protocol].prot_DataType)
		{
			return (gProtocolTable[protocol].prot_DataType(p_ctx));
		}
	}

	return (HTML_MIME_TYPE_UNKNOWN);
}

/******************************************************************************
 * wload_Charset() - Return the character encoding scheme used
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 * Parameters
 *
 *   UrlStreamCtx *p_ctx - the stream
 *
 * Description
 *
 * Returns ()
 *
 ******************************************************************************/

enum WebcCharset wload_Charset(UrlStreamCtx *p_ctx)
{
  #if (WEBC_SUPPORT_OFFLINE_BROWSING)

	if (p_ctx->readFromCache)
	{
		return ((enum WebcCharset) wcache_get_charset(&p_ctx->wcs));
	}

  #endif // WEBC_SUPPORT_OFFLINE_BROWSING

	int protocol = p_ctx->p_desc->getProtocol();
	if ((protocol >= 0) && (protocol < NUM_PROTOCOLS))
	{
		if (gProtocolTable[protocol].prot_Charset)
		{
			return (gProtocolTable[protocol].prot_Charset(p_ctx));
		}
	}

	return (WEBC_CHARSET_UNKNOWN);
}


/******************************************************************************
 * wload_Mode() - Return the current state of a stream context
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   int wload_Mode(UrlStreamCtx *p_ctx);
 *
 * Parameters
 *
 *   UrlStreamCtx *p_ctx - the stream
 *
 * Description
 *
 * Returns (int)
 *
 *   One of the following:
 *
 *     URLSTREAM_CLOSED       - the stream is closed
 *     URLSTREAM_OPEN_RDONLY  - the stream is open for reading only
 *     URLSTREAM_OPEN_WRONLY  - the stream is open for writing only
 *     URLSTREAM_OPEN_RDWR    - the stream is open for reading and writing
 *     URLSTREAM_ERROR        - the stream is in a error state
 *
 ******************************************************************************/

int wload_Mode(UrlStreamCtx *p_ctx)
{
	return (p_ctx->mode);
}


/******************************************************************************
 * wload_Close() - Close an open URL stream.
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   int wload_Close(UrlStreamCtx *p_ctx);
 *
 * Parameters
 *
 *   UrlStreamCtx *p_ctx - the stream to close
 *
 * Description
 *
 *   This function closes an open URL stream.  If the URL protocol is HTTP, it
 *   frees the socket used; if the URL protocol is HTTPS, it frees the SSL
 *	connections and the SSL context; if it is FILE, wload_Close() will close
 *	the file.
 *
 * Returns (int)
 *
 *   0 on success, negative otherwise
 *
 ******************************************************************************/

int wload_Close(UrlStreamCtx *p_ctx)
{
	int result;

  #if (WEBC_SUPPORT_OFFLINE_BROWSING)

	if (p_ctx->readFromCache)
	{
		p_ctx->readFromCache = WEBC_FALSE;
		result = wcache_close(&p_ctx->wcs);
		if (result >= 0)
		{
			p_ctx->mode = URLSTREAM_CLOSED;
		}
		return (result);
	}

	if (p_ctx->saveToCache)
	{
		p_ctx->saveToCache = WEBC_FALSE;
		wcache_close(&p_ctx->wcs);
	}

  #endif // WEBC_SUPPORT_OFFLINE_BROWSING

	if (p_ctx->mode == URLSTREAM_CLOSED)
	{
		return (0);
	}

	int protocol = p_ctx->p_desc->getProtocol();
	if ((protocol >= 0) && (protocol < NUM_PROTOCOLS))
	{
		if (gProtocolTable[protocol].prot_Close)
		{
			result = gProtocolTable[protocol].prot_Close(p_ctx);
			if (result >= 0)
			{
				p_ctx->mode = URLSTREAM_CLOSED;
			}

			return (result);
		}
	}

	return (-1);
}

/**
   @memo Set a function for a stream to report progress
   @doc  This function will allow the module using wload_ to track the
   progress of a stream.

   @param pStream    The stream to report progress for
   @param report_fn  A pointer to a function to be called whenever staus is updated
   @param report_ctx A context pointer to be passed into report_fn when it is called
   @see
*/
void wload_SetReportStatusFn(UrlStreamCtx *pStream, webcReportFn report_fn, WEBC_PFBYTE report_ctx)
{
	if (pStream)
	{
		pStream->report_func = report_fn;
		pStream->report_ctx = report_ctx;
	}
}

#if (WEBC_SUPPORT_OFFLINE_BROWSING)

void wload_SetOnlineContentCache(UrlStreamCtx *pStream, CacheHandle* pCache)
{
	if (pStream)
	{
		if (pStream->onlineCache)
		{
			pStream->onlineCache->release();
		}
		pStream->onlineCache = pCache;
		return;
	}

	if (pCache)
	{
		pCache->release();
	}
}

struct wcache_ctx *wload_GetOfflineContentCache(void)
{
	if (!giOfflineContentCacheInitialized)
	{
		if (wcache_init(&gOfflineContentCache, WEBC_CFG_OFFLINE_CACHE_PREFIX,
		                WEBC_CFG_OFFLINE_CACHE_SIZE,
		                WEBC_CFG_OFFLINE_CACHE_MAX_FILES,
		                &gWebcFileCacheSpec, 0) == 0)
		{
			giOfflineContentCacheInitialized = 1;
		}
	}

	if (giOfflineContentCacheInitialized)
	{
		return (&gOfflineContentCache);
	}

	return (0);
}

int webc_OfflineInitDirEntry(WebcOfflineCacheDirEntry *entry, URLDescriptor* urlDesc, const WEBC_CHAR *title)
{
	if (entry && urlDesc)
	{
		UrlStringBuilder ubld(urlDesc);
		WEBC_CHAR *_key = ubld.MallocString(__FILE__, __LINE__);
		if (_key)
		{
			char *key = webc_arg_to_char(_key);
			if (key)
			{
				entry->title[0] = 0;
				if (title)
				{
					long size = webc_encode_string(0, (WEBC_CHAR *) title, 0, WEBC_CHARSET_UTF8);
					WEBC_CHAR *titleUtf8 = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR) * (size + 1));
					if (titleUtf8)
					{
						webc_encode_string(titleUtf8, (WEBC_CHAR *) title, 0, WEBC_CHARSET_UTF8);
						titleUtf8[size] = 0;

						char *titleAscii = webc_arg_to_char(titleUtf8);
						if (titleAscii)
						{
							tc_strncpy(entry->title, titleAscii, WEBC_OFFLINE_DIR_TITLE_SIZE-1);
							webc_arg_done(titleAscii);
						}
						WEBC_FREE(titleUtf8);
					}
				}

				tc_strncpy(entry->key, key, WEBC_OFFLINE_DIR_KEY_SIZE-1);

				webc_arg_done(key);
				ubld.FreeString(_key, __FILE__, __LINE__);
				return (0);
			}

			ubld.FreeString(_key, __FILE__, __LINE__);
		}
	}

	return (-1);
}

int webc_OfflineWriteDirEntry(wcache_stream *wcs, struct WebcOfflineCacheDirEntry *entry)
{
	short titleLen = tc_strlen(entry->title);
	short keyLen = tc_strlen(entry->key);
	unsigned char len[4];

	len[0] = (titleLen >> 8);
	len[1] = (titleLen & 0xff);
	len[2] = (keyLen >> 8);
	len[3] = (keyLen & 0xff);

	if (wcache_write(wcs, len, 4) < 4)
	{
		return (-1);
	}

	if (wcache_write(wcs, (unsigned char *) entry->title, titleLen) < titleLen)
	{
		return (-1);
	}

	if (wcache_write(wcs, (unsigned char *) entry->key, keyLen) < keyLen)
	{
		return (-1);
	}

	return (0);
}

int webc_OfflineReadDirEntry(wcache_stream *wcs, struct WebcOfflineCacheDirEntry *entry)
{
	unsigned char len[4];

	if (wcache_read(wcs, len, 4) < 4)
	{
		return (-1);
	}

	short titleLen = (len[0] << 8) | len[1];
	short keyLen   = (len[2] << 8) | len[3];

	if (titleLen > WEBC_OFFLINE_DIR_TITLE_SIZE-1)
	{
		titleLen = WEBC_OFFLINE_DIR_TITLE_SIZE-1;
	}

	if (keyLen > WEBC_OFFLINE_DIR_KEY_SIZE-1)
	{
		keyLen = WEBC_OFFLINE_DIR_KEY_SIZE-1;
	}

	if (wcache_read(wcs, (unsigned char *) entry->title, titleLen) < titleLen)
	{
		return (-1);
	}

	if (wcache_read(wcs, (unsigned char *) entry->key, keyLen) < keyLen)
	{
		return (-1);
	}

	entry->title[titleLen] = 0;
	entry->key[keyLen] = 0;

	return (0);
}

#endif


/******************************************************************************
 * LoadHTMLDocument::LoadHTMLDocument() - Default constructor
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   LoadHTMLDocument(void);
 *
 * Parameters
 *   none.
 *
 * Description
 *   Default constructor for class LoadHTMLDocument.
 *
 * Returns ()
 *
 ******************************************************************************/

LoadHTMLDocument::LoadHTMLDocument(void)
{
	mpBrowser = 0;
	mpStream = 0;
	mbStreamOpen = 0;
	mbStreamOpenFailed = 0;
	miScrollX = 0;
	miScrollY = 0;
	mResumeRefresh = WEBC_FALSE;
	mCharset = WEBC_CHARSET_UNKNOWN;
	mGeneratedLoadEvent = WEBC_FALSE;
	mScrollPositionSet = WEBC_FALSE;
}


/******************************************************************************
 * LoadHTMLDocument::LoadHTMLDocument() - Constructor
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   LoadHTMLDocument(HTMLDocument *pDocument, url_descriptor *pUrl);
 *
 * Parameters
 *
 *   HTMLDocument *pDocument - the document to write data to
 *   url_descriptor *pUrl - the URL to read data from
 *
 * Description
 *
 *   Initialize a LoadHTMLDocument LoadJob to load a particular URL into an
 *   HTMLDocument instance.  This function calls wload_WaitForStream to obtain
 *   a stream to open and read from.
 *
 * Returns ()
 *
 ******************************************************************************/


LoadHTMLDocument::LoadHTMLDocument(HTMLDocument *pDocument, URLDescriptor *pUrl,
		WebcCharset charset, DISPLAY_INT x, DISPLAY_INT y, WEBC_BOOL scrollPositionSet)
{
	mDocRef.Set(pDocument);
	mpStream = 0;
	mbStreamOpen = 0;
	mbStreamOpenFailed = 0;
	mpBrowser = 0;
	miScrollX = x;
	miScrollY = y;
	mResumeRefresh = WEBC_FALSE;
	mCharset = charset;
	mGeneratedLoadEvent = WEBC_FALSE;
	mScrollPositionSet = scrollPositionSet;

	if (pDocument)
	{
		mpBrowser = pDocument->GetBrowser();

		// Notify our parent document that we are loading
#if (WEBC_SUPPORT_FRAMES)
		HTMLFrame *pFrame = pDocument->GetFrame();
		if (pFrame && pDocument->Parent())
		{
			pDocument->Parent()->NotifyLoading(pFrame);
		}
#endif // WEBC_SUPPORT_FRAMES

		// Call once for this document
		pDocument->NotifyLoading(pDocument->Body());
	}

	if (pUrl)
	{
		// protect here against NULL pointer.  if URL is passed, then get on the
		//  waiting list for a UrlStreamCtx.
		mUrl.set(pUrl);
	}
}


/******************************************************************************
 * LoadHTMLDocument::~LoadHTMLDocument() - Destructor
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   ~LoadHTMLDocument(void);
 *
 * Parameters
 *
 * Description
 *
 *   Closes the URL stream if open and frees the stream context.
 *
 * Returns ()
 *
 ******************************************************************************/

LoadHTMLDocument::~LoadHTMLDocument()
{
	if (mpStream)
	{
		// Do protocol-specific shutdown
		wload_Close(mpStream);
		wload_StreamFree(mpStream, 0);
		mbStreamOpen = 0;
		mbStreamOpenFailed = 0;
		mpStream = 0;
	}

	if (!mGeneratedLoadEvent)
	{
		mGeneratedLoadEvent = WEBC_TRUE;

		HTMLDocument *pDoc = mDocRef.Get();
		if (pDoc)
		{
			// Notify ourselves that we are done
			pDoc->NotifyDone(pDoc->Body());
			pDoc = mDocRef.Get();

			// Notify our parent document we are done loading
	#if (WEBC_SUPPORT_FRAMES)
			if (pDoc && pDoc->Parent() && pDoc->GetFrame())
			{
				pDoc->Parent()->NotifyDone(pDoc->GetFrame());
			}
	#endif // WEBC_SUPPORT_FRAMES
		}
	}
}

void LoadHTMLDocument::Abort()
{
	HTMLDocument *pDoc = mDocRef.Get();

	if (!mGeneratedLoadEvent)
	{
		mGeneratedLoadEvent = WEBC_TRUE;

		if (pDoc)
		{
			// Notify ourselves that we are done
			pDoc->NotifyDone(pDoc->Body());
			pDoc = mDocRef.Get();

			// Notify our parent document we are done loading
	#if (WEBC_SUPPORT_FRAMES)
			if (pDoc && pDoc->Parent() && pDoc->GetFrame())
			{
				pDoc->Parent()->NotifyDone(pDoc->GetFrame());
			}
	#endif // WEBC_SUPPORT_FRAMES
		}
	}

	if (pDoc)
	{
		DisplayManager* pManager = pDoc->GetDisplayManager();

		if (pManager && mResumeRefresh)
		{
			DISPLAY_MANAGER_RESUME_REFRESH(pManager);
			mResumeRefresh = WEBC_FALSE;
		}
	}

	mDocRef.Set(0);

	LoadJob::Abort();
}


/******************************************************************************
 * LoadHTMLDocument::SetStream() - Initialize stream and do GET
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   int SetStream(UrlStreamCtx *pStream);
 *
 * Parameters
 *
 *   UrlStreamCtx *pStream - a preallocated UrlStreamCtx to open the URL with
 *
 * Description
 *
 *   Calls wload_Init and wload_Get to open a stream for reading HTML data.
 *
 * Returns (int)
 *
 *   0 on success, -1 on failure
 *
 ******************************************************************************/

int LoadHTMLDocument::SetStream(UrlStreamCtx *pStream)
{
	if (pStream)
	{
		mpStream = pStream;


		wload_Init(mpStream, _LoadJobPollCycle, (WEBC_PFBYTE) this);

	  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
		if (mDocRef.Get())
		{
			wload_SetOnlineContentCache(mpStream, mDocRef.Get()->GetOnlineCache());
		}
	  #endif
		wload_SetReportStatusFn(mpStream,(webcReportFn) _DocumentSetStatus, (WEBC_PFBYTE) mDocRef.Get());
		if (!Aborted())
		{
			if (wload_Get(mpStream, &mUrl, 0) >= 0)
			{
				mbStreamOpen = 1;
				return (0);
			}
			else
				mbStreamOpenFailed = 1;
		}

	  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
		wload_SetOnlineContentCache(mpStream, 0);
	  #endif
	}

	return (-1);
}

WEBC_BOOL LoadHTMLDocument::ClearDocumentAndOpen (void)
{
	HTMLDocument* pDoc = mDocRef.Get();

	if (!pDoc)
	{
		return WEBC_FALSE;
	}

	// clear the document - deletes all HTMLElements
	pDoc->ScrollTo(0,0);
	pDoc->Clear();
	pDoc = mDocRef.Get();

	if (!pDoc)
	{
		return WEBC_FALSE;
	}

	// set input focus to the body of the document
	if (pDoc->Body())
	{
		if (pDoc->OkayToResetFocusToBody())
		{
			pDoc->Body()->Focus();
			pDoc = mDocRef.Get();
		}

		if (!pDoc)
		{
			return WEBC_FALSE;
		}
	}

	// open up the document for writing; this creates an HtmlDocumentBuilder object
	//  associated with this HTMLDocument.
	if (pDoc->Open() < 0)
	{
		return WEBC_FALSE;
	}

	// Need to set this here since the first call wload_Read may change it
	//  (on account of an HTTP re-direction)
	pDoc->SetURL(&mUrl);

	return WEBC_TRUE;
}


/******************************************************************************
 * LoadHTMLDocument::Execute() - Load an HTMLDocument
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   long Execute(void);
 *
 * Description
 *
 *   See class LoadJob.
 *
 * Returns (int)
 *
 *   0 on success, -1 on failure
 *
 ******************************************************************************/

#define PLAIN_TEXT_HTML_PREFIX  "<HTML><BODY><PRE>"
#define PLAIN_TEXT_HTML_SUFFIX  "</PRE></BODY></HTML>"

#define MAX_DEFAULT_HTML_ERROR_MSG (500)

/*
#define DEFAULT_HTML_ERROR "<head><META HTTP-EQUIV=\"WEBC_FILE_NOT_FOUND\" content=\"WEBC_FILE_NOT_FOUND\"</head><html><title>Error</title><h1>Page Error</h1>"\
*/
#define DEFAULT_HTML_ERROR      "<html><title>Error</title><h1>Page Error</h1>"\
	"<p>An error occurred while trying to load this document."\
                                "<p>Please press the refresh button or enter a new URL."\
                                "</html>"

#if(WEBC_SUPPORT_ERROR_PAGE_REDIRECT)
#define DEFAULT_LOCAL_PAGE      "<html><head><META HTTP-EQUIV=refresh CONTENT=5;home.html></head>"\
								"</html>"
#define REDIRECT_URLS_NOT_SET_ERROR		"<html><title>Error</title><h1>Redirect Urls not set.</h1>"\
                                "<p>Use webc_setErrorRedirect or turn off WEBC_SUPPORT_ERROR_PAGE_REDIRECT"\
                                "</html>"
#else
#define PAGE_NOT_FOUND_ERROR    "<html><title>Error</title><h1>Page Not Found.</h1>"\
                                "<p>The server can not find the requested page."\
                                "<p>Please press the refresh button or enter a new URL."\
                                "</html>"
#define SERVER_NOT_FOUND_ERROR  "<html><title>Error</title><h1>Server Not Found.</h1>"\
                                "<p>The Requested server was not found."\
                                "</html>"
#define PAYMENT_REQ_ERROR       "<html><title>Error</title><h1>Payment Required Error</h1>"\
                                "<p>Please press the refresh button or enter a new URL."\
                                "</html>"
#define FORBIDDEN_ERROR         "<html><title>Error</title><h1>Forbidden Error</h1>"\
                                "<p>Access is forbidden to the requested page ."\
	"<p>Please press the refresh button or enter a new URL."\
	"</html>"
#endif

long LoadHTMLDocument::Execute(void)
{
	char *buffer = 0;
	int result = 0;
	long bytes_read;
	HTMLDocument *pDoc = mDocRef.Get();
	DisplayManager *pManager = 0;
	int isPlainText = 0;
	WEBC_CHAR *htmlBuffer = 0;
	WEBC_INT32 fileSize = 0;
	if (!pDoc)
	{
		result = -1;
		goto clean_up_and_return;
	}
	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"HTMLDocument::Execute", URLTOSTR(&mUrl))

#if (WEBC_SUPPORT_JSCRIPT)
	// support "javascript:..." URLs
	if (mUrl.getProtocol() == WEBC_PROTOCOL_JSCRIPT)
	{
		WebcJSDocumentContext *jsmgr = pDoc->GetJSMgr();
		if (jsmgr)
		{
			WebcJSScript *pScript;

			WEBC_NEW_VERBOSE(pScript, WebcJSScript(jsmgr, jsmgr->GetGlobalObject(), mUrl.getScript(), webc_strlen(mUrl.getScript())),"WebcJSScript");
			if (pScript)
			{
				jsval rval;
				pScript->ExecuteScript(&rval);
				pScript->Destroy();
			}
			return 0;
		}
		return (-1);
	}
#endif // WEBC_SUPPORT_JSCRIPT

	pManager = pDoc->GetDisplayManager();

	if (pManager)
	{
		DISPLAY_MANAGER_STOP_REFRESH(pManager);
		mResumeRefresh = WEBC_TRUE;
	}

	if (pDoc->IsOpen())
	{
		pDoc->Close();
	}

	// allocate our receive buffer
	buffer = (char *) WEBC_MALLOC(WEBC_LOAD_BUFFER_SIZE+1);
	if (!buffer)
	{
		result = -1; // WEBC_EMALLOC
		goto clean_up_and_return;
	}


	// if we don't yet have a stream to read from, allocate one and initialize it
	//  (send GET request or open file)
	if (mbStreamOpenFailed || !mpStream || !mbStreamOpen)
	{
		if (!mbStreamOpenFailed && !mpStream)
		{
			mpStream = wload_StreamAlloc(0);
		}

		if (mbStreamOpenFailed || (SetStream(mpStream) < 0))
		{
			// clear the document - deletes all HTMLElements
			pDoc->Clear();
			pDoc = mDocRef.Get();

			if (pDoc)
			{
				// open up the document for writing; this creates an HtmlDocumentBuilder object
				//  associated with this HTMLDocument.
				if (pDoc && pDoc->Open() >= 0)
				{
					WEBC_CHAR errorMessage[MAX_DEFAULT_HTML_ERROR_MSG];
#if(WEBC_SUPPORT_ERROR_PAGE_REDIRECT)
					char * redirectUrl;
					char tempbuffer[MAX_DEFAULT_HTML_ERROR_MSG];
					redirectUrl = webc_getErrorRedirect(URL_ERROR_SERVER_NOT_FOUND);
					if(!redirectUrl)
						webc_c_strncpy (&errorMessage[0], REDIRECT_URLS_NOT_SET_ERROR, MAX_DEFAULT_HTML_ERROR_MSG-1);
					else
					{
						getHtmlPageWithRedirectUrl(tempbuffer,redirectUrl);
						webc_c_strncpy (&errorMessage[0], (const char*)tempbuffer, MAX_DEFAULT_HTML_ERROR_MSG-1);
					}
#else
					webc_c_strncpy(errorMessage, SERVER_NOT_FOUND_ERROR, MAX_DEFAULT_HTML_ERROR_MSG-1);
					errorMessage[MAX_DEFAULT_HTML_ERROR_MSG-1] = 0;
#endif
					pDoc->Write(errorMessage);
				}
			}

			result = -1; // WEBC_EMALLOC
			goto clean_up_and_return;
		}
	}

	// Main Load Loop - read the ENTIRE HTML Document from the stream
#if (WLOAD_REPORT_STATUS)
	webc_ReportBrowserStatus(mpBrowser, "Reading HTML data...");
#endif

	// Read data
	bytes_read = wload_Read(mpStream, buffer, WEBC_LOAD_BUFFER_SIZE);

	// if on first read, error is returned, then the current content is kept?
	if (bytes_read < 0)
	{
		pDoc->Clear();
		pDoc = mDocRef.Get();
		if (pDoc)
		{
				if (pDoc && pDoc->Open() >= 0)
				{
					WEBC_CHAR errorMessage[MAX_DEFAULT_HTML_ERROR_MSG];
#if (WEBC_SUPPORT_ERROR_PAGE_REDIRECT)
					char * redirectUrl = 0;
					char tempbuffer[MAX_DEFAULT_HTML_ERROR_MSG];
					switch(bytes_read)
					{
					case HTTPC_PAYMENTREQ:
						redirectUrl = webc_getErrorRedirect(URL_ERROR_PAYMENT_REQ);
						break;
					case HTTPC_FORBIDDEN:
						redirectUrl = webc_getErrorRedirect(URL_ERROR_FORBIDDEN);
						break;
					case HTTPC_PAGENOTFOUND:
						redirectUrl = webc_getErrorRedirect(URL_ERROR_PAGE_NOT_FOUND);
						break;
					default:
						redirectUrl = webc_getErrorRedirect(URL_ERROR_PAGE_NOT_SET);
						break;
					}

					if(redirectUrl)
					{
						getHtmlPageWithRedirectUrl(tempbuffer,redirectUrl);
						webc_c_strncpy (&errorMessage[0], (const char*)tempbuffer, MAX_DEFAULT_HTML_ERROR_MSG-1);
					}
					else
						webc_c_strncpy (&errorMessage[0], REDIRECT_URLS_NOT_SET_ERROR, MAX_DEFAULT_HTML_ERROR_MSG-1);
					errorMessage[MAX_DEFAULT_HTML_ERROR_MSG-1] = 0;
#else
					switch(bytes_read)
					{
					case HTTPC_PAYMENTREQ:
						webc_c_strncpy(errorMessage, PAYMENT_REQ_ERROR, MAX_DEFAULT_HTML_ERROR_MSG-1);
						errorMessage[MAX_DEFAULT_HTML_ERROR_MSG-1] = 0;
						break;
					case HTTPC_FORBIDDEN:
						webc_c_strncpy(errorMessage, FORBIDDEN_ERROR, MAX_DEFAULT_HTML_ERROR_MSG-1);
						errorMessage[MAX_DEFAULT_HTML_ERROR_MSG-1] = 0;
						break;
					case HTTPC_PAGENOTFOUND:
						webc_c_strncpy(errorMessage, PAGE_NOT_FOUND_ERROR, MAX_DEFAULT_HTML_ERROR_MSG-1);
						errorMessage[MAX_DEFAULT_HTML_ERROR_MSG-1] = 0;
						break;
					default:
						webc_c_strncpy(errorMessage, DEFAULT_HTML_ERROR, MAX_DEFAULT_HTML_ERROR_MSG-1);
						errorMessage[MAX_DEFAULT_HTML_ERROR_MSG-1] = 0;
						break;
					}
#endif
					pDoc->Write(errorMessage);
				}
			}
			result = bytes_read;
			goto clean_up_and_return;
	}
	fileSize = bytes_read;

	pDoc = mDocRef.Get();

	// If we successfully read some data, send it to the HTMLDocument
	// Bug fix Dec 2012. check pDoc
	if (pDoc && bytes_read > 0)
	{
		WebcFileContentType contentType = wload_DataType(mpStream);
		WebcCharset charEncoding = wload_Charset(mpStream);

		if (charEncoding == WEBC_CHARSET_UNKNOWN)
		{
			wload_ResolveCharset(mpStream, pDoc->GetCharset());
			charEncoding = pDoc->GetCharset();
		}

		switch (contentType)
		{
			case HTML_MIME_TYPE_MNG:
			case HTML_MIME_TYPE_PNG:
			case HTML_MIME_TYPE_JPEG:
			case HTML_MIME_TYPE_GIF:
			case HTML_MIME_TYPE_BMP:
			{
				UrlStringBuilder urlb(&mUrl);
				WEBC_CHAR *url_str = urlb.MallocString(__FILE__, __LINE__);
				if (url_str)
				{
					if ((urlb.GetStringLength() + 42) < WEBC_LOAD_BUFFER_SIZE)
					{
						webc_c_strcpy((WEBC_CHAR *) buffer, "<html><body><img src=\"");
						webc_strcat((WEBC_CHAR *) buffer, url_str);
						webc_c_strcat((WEBC_CHAR *) buffer, "\"></body></html>");

						// close the stream here so we don't destroy a cache entry
						//  only to try to access it later
						if (mpStream)
						{
							// Do protocol-specific shutdown
							wload_Close(mpStream);
							wload_StreamFree(mpStream, 0);
							mbStreamOpen = 0;
							mbStreamOpenFailed = 0;
							mpStream = 0;
						}

						if (!ClearDocumentAndOpen())
						{
							result = -1;
							goto clean_up_and_return;
						}

						pDoc = mDocRef.Get();
						pDoc->Write((WEBC_CHAR *) buffer);
					}
					urlb.FreeString(url_str, __FILE__, __LINE__);
				}

				bytes_read = 0;
			}
			break;

			case HTML_MIME_TYPE_PLAIN_TEXT:
			{
				if (!ClearDocumentAndOpen())
				{
					result = -1;
					goto clean_up_and_return;
				}

				pDoc = mDocRef.Get();
				if (!pDoc)
				{
					result = -1;
					goto clean_up_and_return;
				}

				pDoc->Write((WEBC_CHAR *)_WEBC_STR("<html><body><pre>"), 17);

				WebcURLInputStream urlStream(mpStream);
				WebcSeededInputStream rawStream(&urlStream, (unsigned char*) buffer, bytes_read);
				WebcCharsetDecodeFilter charsetDecoder(charEncoding);
				WebcFilteredInputStream charStream(&charsetDecoder, &rawStream);
				WebcTextToHTMLFilter textToHtml;
				WebcFilteredInputStream htmlFromTextStream(&textToHtml, &charStream);
				WebcInputStream* htmlStream = &htmlFromTextStream;

				const long htmlBufferSize = 1024;
				WEBC_CHAR* htmlBuffer = (WEBC_CHAR*) WEBC_MALLOC(sizeof(WEBC_CHAR) * htmlBufferSize);
				if (htmlBuffer)
				{
					long chars_parsed;
					WEBC_BOOL charsetChanged = WEBC_FALSE;

					do
					{
						pDoc = mDocRef.Get();
						if (!pDoc)
						{
							result = -1;
							break;
						}

						// charset has changed!
						if (charEncoding != pDoc->GetCharset() && !charsetChanged)
						{
							// set the new charset
							charEncoding = pDoc->GetCharset();

							// dump the old converted characters
							if (!rawStream.hasMoreSeedData())
							{
								// this means we're already too far into it; must abort
								break;
							}

							// we're just going to reset all our streams and restart from the beginning
							pDoc->Close();
							if (!ClearDocumentAndOpen())
							{
								result = -1;
								break;
							}

							{
								static WEBC_CHAR tmpStr[] = {'<','h','t','m','l','>','<','b','o','d','y','>','<','p','r','e','>',0};
								pDoc->Write(tmpStr, 17);
							}

							rawStream.setSeedData((unsigned char*) buffer, bytes_read);
							charsetDecoder.reset();
							charsetDecoder.setInputEncoding(charEncoding);
							textToHtml.reset();
							charStream.discardBufferedData();
							htmlFromTextStream.discardBufferedData();

							charsetChanged = WEBC_TRUE;
							chars_parsed = 1; // to make our loop check happy
							continue;
						}

						long bytesRead = htmlStream->read((unsigned char*) htmlBuffer, sizeof(WEBC_CHAR) * htmlBufferSize);
						if (bytesRead <= 0)
						{
							break;
						}
						chars_parsed = pDoc->Write(htmlBuffer, bytesRead / sizeof(WEBC_CHAR));

						if (this->Aborted())
						{
							result = -1;
							break;
						}
					}
					while (chars_parsed > 0);

					WEBC_FREE(htmlBuffer);
					htmlBuffer = 0;
				}

				if (pDoc)
				{
					static WEBC_CHAR tmpStr[] = {'<','/','p','r','e','>','<','/','b','o','d','y','>','<','/','h','t','m','l','>',0};
					pDoc->Write(tmpStr, 20);
				}

				break;
			}

			case HTML_MIME_TYPE_HTML:
			case HTML_MIME_TYPE_MOBILE_HTML:
			case HTML_MIME_TYPE_XHTML:
			{
				WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
				WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"ClearDocument", URLTOSTR(&mUrl))
				if (!ClearDocumentAndOpen())
				{
					result = -1;
					goto clean_up_and_return;
				}
				WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"ClearDocument", URLTOSTR(&mUrl))

				pDoc = mDocRef.Get();

				// generate an event so a prefix can be added from the user code
				HTMLBrowserEvent e;
				memset((void *)&e,0,sizeof(HTMLBrowserEvent));

				e.type = HTML_BROWSER_EVENT_HTML_PAGE_PREFIX_REQUEST;
				e.hdoc = (HDOC_HANDLE) pDoc;

				(void)mpBrowser->TriggerBrowserEvent(&e);

				if (contentType == HTML_MIME_TYPE_PLAIN_TEXT)
				{
					// treat text files as PRE-formatted HTML
					// format the doc so that we don't interpret
					// '<..>' as HTML tags
					if (!isPlainText)
					{
						WEBC_CHAR prefix[25];
						webc_c_strncpy(prefix, PLAIN_TEXT_HTML_PREFIX, 24);
						prefix[24] = 0;
						pDoc->Write(prefix);
						isPlainText = 1;
					}
				}

				WEBC_CHAR *textToParse;
				long sizeToParse;
				long bufferSize = WEBC_LOAD_BUFFER_SIZE;
				long bufferUsed = bytes_read;
				long chars_parsed;
				int charsetChanged = 0;
				WebcCharDecodeStream decodeStream;

				pDoc->SetCharset(charEncoding, HTML_CHARSET_SRC_STREAM);
				WebcCharset streamCharset = pDoc->GetCharset();

				decodeStream.pInBuffer     = 0;
				decodeStream.iInBufferSize = 0;
				decodeStream.iBytesRead    = 0;
				decodeStream.pOutBuffer    = 0;
				decodeStream.iOutBufferLen = 0;
				decodeStream.iCharsWritten = 0;
				decodeStream.charset       = streamCharset;

				do
				{
					if (this->Aborted())
					{
						result = -1;
						goto clean_up_and_return;
					}

					// fill the input buffer
					if ((bytes_read != 0) && (bufferUsed < bufferSize))
					{
						bytes_read = wload_Read(mpStream, buffer + bufferUsed, bufferSize - bufferUsed);

						if (bytes_read < 0)
						{
							result = -1; // io read error
							goto clean_up_and_return;
						}

						if (isPlainText && (bytes_read == 0))
						{
							// the last time thru, set the default type HTML suffix
							WEBC_CHAR suffix[25];
							webc_c_strncpy(suffix, PLAIN_TEXT_HTML_SUFFIX, 24);
							suffix[24] = 0;
							pDoc->Write(suffix);
						}

						fileSize += bytes_read;

						pDoc = mDocRef.Get();
						if (!pDoc)
						{
							result = -1;
							goto clean_up_and_return;
						}

						bufferUsed += bytes_read;
					}

					// do conversion (if necessary)
					if (streamCharset == WEBC_DEFAULT_CHARSET)
					{
						// no conversion necessary
						textToParse = (WEBC_CHAR *) buffer;
						sizeToParse = bufferUsed / sizeof(WEBC_CHAR);
						textToParse[sizeToParse] = 0; // null terminate
					}
					else
					{
						// incoming data must be converted to WEBC_CHAR's
						if (!decodeStream.pOutBuffer)
						{
							if (!htmlBuffer)
							{
								htmlBuffer = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR)*WEBC_LOAD_BUFFER_SIZE);
								if (!htmlBuffer)
								{
									result = -1; // malloc failure
									goto clean_up_and_return;
								}
							}

							decodeStream.pOutBuffer    = htmlBuffer;
							decodeStream.iOutBufferLen = WEBC_LOAD_BUFFER_SIZE;
							decodeStream.iCharsWritten = 0;
						}

						decodeStream.pInBuffer     = (unsigned char *) buffer;
						decodeStream.iInBufferSize = bufferUsed;
						decodeStream.iBytesRead    = 0;
						decodeStream.charset       = streamCharset;

						webc_decode_string(&decodeStream);

						textToParse = decodeStream.pOutBuffer;
						sizeToParse = decodeStream.iCharsWritten;
					}

					// parse the data
					if (sizeToParse > 0)
					{
						WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)

						WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"HTMLDocument::DocWrite", URLTOSTR(&mUrl))
						chars_parsed = pDoc->Write(textToParse, sizeToParse);
						WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"HTMLDocument::DocWrite", URLTOSTR(&mUrl))

						pDoc = mDocRef.Get();
						if (!pDoc)
						{
							result = -1;
							goto clean_up_and_return;
						}
					}
					else
					{
						break;
					}

					if (chars_parsed < 0)
					{
						// error?
						break;
					}

					// charset has changed!
					if (streamCharset != pDoc->GetCharset() && !charsetChanged)
					{
						// set the new charset
						streamCharset = pDoc->GetCharset();

						// dump the old converted characters
						decodeStream.iCharsWritten = 0;
						charsetChanged = 1;
						continue;
					}

					if (streamCharset == WEBC_DEFAULT_CHARSET)
					{
						// dump parsed chars
						if (chars_parsed < sizeToParse)
						{
							rtp_memmove(buffer, buffer + chars_parsed, (sizeToParse - chars_parsed) * sizeof(WEBC_CHAR));
						}
						bufferUsed -= chars_parsed * sizeof(WEBC_CHAR);
					}
					else
					{
						// dump parsed chars and converted input data
						if (chars_parsed < decodeStream.iCharsWritten)
						{
							rtp_memmove(htmlBuffer, htmlBuffer + chars_parsed, (decodeStream.iCharsWritten - chars_parsed) * sizeof(WEBC_CHAR));
						}
						decodeStream.iCharsWritten -= chars_parsed;

						if (decodeStream.iBytesRead < bufferUsed)
						{
							rtp_memmove(buffer, buffer + decodeStream.iBytesRead, bufferUsed - decodeStream.iBytesRead);
						}
						bufferUsed -= decodeStream.iBytesRead;
					}
				}
				while (chars_parsed > 0);
				break;
			}

			default:
			{
				// generate an event so this file can be handled in user code
				HTMLBrowserEvent e;

				e.type = HTML_BROWSER_EVENT_UNKNOWN_CONTENT_TYPE;
				e.data.file.url = 0;
				e.data.file.contentType = contentType;
				e.data.file.charEncoding = charEncoding;
				e.hdoc = (HDOC_HANDLE) pDoc;
				e.out.redirectTo = 0;

				UrlStringBuilder builder(&mUrl);
				WEBC_CHAR *url_str = builder.MallocString(__FILE__, __LINE__);
				if (url_str)
				{
					e.data.file.url = url_str;
					HTMLEventStatus status = mpBrowser->TriggerBrowserEvent(&e);
					builder.FreeString(url_str, __FILE__, __LINE__);

					pDoc = mDocRef.Get();
					if (status != HTML_EVENT_STATUS_HALT && e.out.redirectTo)
					{
						while (bytes_read > 0)
						{
							(*e.out.redirectTo)->writeFn(e.out.redirectTo, (WEBC_UINT8*) buffer, bytes_read);
							bytes_read = wload_Read(mpStream, buffer, WEBC_LOAD_BUFFER_SIZE);
						}
						(*e.out.redirectTo)->closeFn(e.out.redirectTo, bytes_read);
					}
				}
				break;
			}
		}
	}

	// At this point we are DONE reading the ENTIRE HTML document or SSL cert.

 clean_up_and_return:
			WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"HTMLDocument::Execute", URLTOSTR(&mUrl))
	if (pDoc)
	{
		pManager = pDoc->GetDisplayManager();

		if (pManager && mResumeRefresh)
		{
			DISPLAY_MANAGER_RESUME_REFRESH(pManager);
			mResumeRefresh = WEBC_FALSE;
		}

		// Refresh the display
	#if (!WEBC_MINIMIZE_SCREEN_REFRESH)
		pDoc->Refresh();

		// Scroll to the anchor in the url
		//  - it is pointless to do this unless the above line, pDoc->Refresh,
		//    executes, since the position of anchors is determined at format
		//    time (which is triggered by the refresh), and the viewport rect
		//    is only allowed to move over the formatted content region.
		//   In the case where WEBC_MINIMIZE_SCREEN_REFRESH is enabled,
		//    these functions (scrolling to anchor or default position) are
		//    handled inside HTMLDocument::ScrollToDefaultAnchor, which is
		//    called from HTMLBodyDisplay::Format.
		// TBD - can't we just get rid of this code since it will be handled
		//  in any case by HTMLBodyDisplay::Format?
		if (mUrl.getAnchor()[0] != '\0')
		{
			pDoc->ScrollToAnchor(mUrl.getAnchor());
		}
		else if (mScrollPositionSet)
		{
			if ((pDoc->GetScrollTop() == 0) && (pDoc->GetScrollLeft() == 0))
			{
				pDoc->ScrollTo(miScrollX, miScrollY);
			}
		}
	#else
		if (mScrollPositionSet)
		{
			pDoc->SetDefaultScrollPosition(miScrollX, miScrollY);
		}
	#endif // WEBC_MINIMIZE_SCREEN_REFRESH

	  #if (WEBC_SUPPORT_URL_LOG)
		if (pDoc->GetBrowser())
		{
			// If we're the root document in a browser window, set the url bar
			UrlStringBuilder ubld(&mUrl);
			WEBC_CHAR *url_str = ubld.MallocString(__FILE__, __LINE__);
			if (url_str)
			{
#ifdef WEBC_DEBUG
                           gpUrlLogFile = fopen(WEBC_CFG_URL_LOG_FILE, "a+");
				if (gpUrlLogFile)
				{
					char *str = webc_arg_to_char(url_str);
					fprintf(gpUrlLogFile, "%s\n", str);
					fclose(gpUrlLogFile);
					webc_arg_done(str);
				}
#endif
				ubld.FreeString(url_str, __FILE__, __LINE__);
			}
		}
	  #endif // WEBC_SUPPORT_URL_LOG

		pDoc->Close();
		pDoc->SetURL(&mUrl);
	}

	if (mpStream )
	{
		// Do protocol-specific shutdown
		if (mbStreamOpen)
			wload_Close(mpStream);
		wload_StreamFree(mpStream, 0);
		mbStreamOpen = 0;
		mbStreamOpenFailed = 0;
		mpStream = 0;
	}

	if (buffer)
	{
		WEBC_FREE(buffer);
	}

	if (htmlBuffer)
	{
		WEBC_FREE(htmlBuffer);
	}
	return (result);
}


/******************************************************************************
 * PostHTMLDocument::PostHTMLDocument() - Constructor
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   PostHTMLDocument(HTMLDocument *pDocument, url_descriptor *pUrl, PFCHAR pContentType,
 *       PFCHAR pPostData, long lContentLen);
 *
 * Parameters
 *
 *   HTMLDocument *pDocument - the document to write data to
 *   url_descriptor *pUrl - the URL to POST to
 *   PFCHAR pContentType - MIME type of POST data
 *   PFCHAR pPostData - buffer containing the POST data to be sent to the server
 *   long lContentLen - the length in bytes of the POST data
 *
 * Description
 *
 *   Initialize a POST operation to load an HTML Document.  This function calls
 *   wload_WaitForStream to obtain a URL stream to perform the POST and read
 *   operations on.  PostHTMLDocument allocates its own copies of pPostData and
 *   pContentType, so these buffers may be volatile.
 *
 * Returns ()
 *
 ******************************************************************************/

PostHTMLDocument::PostHTMLDocument(HTMLDocument *pDocument, URLDescriptor *pUrl, WEBC_CHAR *pContentType,
								   char *pPostData, long lContentLen, WebcCharset charset) :
	LoadHTMLDocument(pDocument, pUrl, charset)
{
	// obtain our own private copies of the content type and post data
	mpContentType = webc_malloc_string_copy(pContentType, __FILE__, __LINE__);
	mpPostData = (char *) WEBC_MALLOC(lContentLen);
	mlContentLength = lContentLen;
	mCharset = charset;

	tc_movebytes(mpPostData, pPostData, lContentLen*sizeof(char));

	mDocRef.Set(pDocument);
	if (pDocument)
	{
		mpBrowser = pDocument->GetBrowser();
	}

	// protect against null pointers
	if (pUrl)
	{
		mUrl.set(pUrl);
	}
}


/******************************************************************************
 * PostHTMLDocument::~PostHTMLDocument() - Destructor
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   ~PostHTMLDocument(void);
 *
 * Parameters
 *
 * Description
 *
 *   Closes the URL stream if open and frees the stream context.  Also frees the
 *   copies of the ContentType string and POST data.
 *
 * Returns ()
 *
 ******************************************************************************/

PostHTMLDocument::~PostHTMLDocument()
{
	webc_free_string_copy(mpContentType, __FILE__, __LINE__);
	WEBC_FREE(mpPostData);
}

int PostHTMLDocument::SetStream(UrlStreamCtx *pStream)
{
	if (pStream)
	{
		mpStream = pStream;
		wload_Init(mpStream, _LoadJobPollCycle, (WEBC_PFBYTE) this);
		wload_SetReportStatusFn(mpStream, (webcReportFn)_DocumentSetStatus, (WEBC_PFBYTE) mDocRef.Get());
		if (!Aborted())
		{
			mbStreamOpen = 0;
			mbStreamOpenFailed = 0;

			if (wload_Post(mpStream, &mUrl, mpContentType, mlContentLength) < 0)
			{
				return (-1);
			}
			if (wload_Write(mpStream, mpPostData, mlContentLength) < mlContentLength)
			{
				return (-1);
			}

			mbStreamOpen = 1;
			mbStreamOpenFailed = 0;
			return (0);
		}
	}

	return (-1);
}


#if (WEBC_SUPPORT_POSTFILE)

/******************************************************************************
 * PostUploadRawFile::PostUploadRawFile() - Constructor
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   PostUploadRawFile(HTMLDocument *pDocument, url_descriptor *pUrl, PFCHAR pContentType,
 *       PFCHAR pPostData, long lContentLen);
 *
 * Parameters
 *
 *   HTMLDocument *pDocument - the document to write data to
 *   url_descriptor *pUrl - the URL to POST to
 *   PFCHAR pContentType - MIME type of POST data
 *   PFCHAR path - Filename containing the POST data to be sent to the server
 *
 * Description
 *
 *   Initialize a POST operation to load an HTML Document.  This function calls
 *   wload_WaitForStream to obtain a URL stream to perform the POST and read
 *   operations on.  PostUploadRawFile allocates its own copies of path and
 *   pContentType, so these buffers may be volatile.
 *
 * Returns ()
 *
 ******************************************************************************/


PostUploadRawFile::PostUploadRawFile(HTMLDocument *pDocument, URLDescriptor *pUrl, WEBC_CHAR *pContentType,
								   char *pPostApp, long queryLen, char* path, WebcCharset charset) :
	LoadHTMLDocument(pDocument, pUrl, charset)
{
	// obtain our own private copies of the content type and post data
	mpContentType = webc_malloc_string_copy(pContentType, __FILE__, __LINE__);

	mlContentLength			= 0;
	mFileIsOpen				= false;
	/* What do we do if we can't open ? */
	if (webc_fopen (path, &mFileHandle, WEBC_FILE_RDONLY) >= 0)
		{
			mFileIsOpen				= true;
			mpPostApp = (char *) WEBC_MALLOC(queryLen);
			tc_movebytes(mpPostApp, pPostApp, queryLen*sizeof(char));
			mlContentLength			= webc_flength (mFileHandle);
		}
		else
		{
			mFileIsOpen				= false;
			mlContentLength			= 0;
			mlContentLength		= 0;
		}
	mDocRef.Set(pDocument);
	if (pDocument)
	{
		mpBrowser = pDocument->GetBrowser();
	}

	// protect against null pointers
	if (pUrl)
	{
		mUrl.set(pUrl);
	}
}


/******************************************************************************
 * PostUploadRawFile::~PostUploadRawFile() - Destructor
 *
 * Summary
 *
 *   #include "webload.hpp"
 *
 *   ~PostUploadRawFile(void);
 *
 * Parameters
 *
 * Description
 *
 *   Closes the URL stream if open and frees the stream context.  Also frees the
 *   copies of the ContentType string and POST data.
 *
 * Returns ()
 *
 ******************************************************************************/

PostUploadRawFile::~PostUploadRawFile()
{
	if (mFileIsOpen)
		webc_fclose(mFileHandle) ;
	if (mpPostApp)
		WEBC_FREE(mpPostApp);
	webc_free_string_copy(mpContentType, __FILE__, __LINE__);
}



#define INCLUDE_FORM_DATA_IN_RAW_FILE_POST 0
int PostUploadRawFile::SetStream(UrlStreamCtx *pStream)
{
	if (mFileIsOpen && mlContentLength && pStream)
	{
		mpStream = pStream;
		wload_Init(mpStream, _LoadJobPollCycle, (WEBC_PFBYTE) this);
		wload_SetReportStatusFn(mpStream, (webcReportFn)_DocumentSetStatus, (WEBC_PFBYTE) mDocRef.Get());
		if (!Aborted())
		{
			long ContentLength;
			mbStreamOpen = 0;
			mbStreamOpenFailed = 0;

			ContentLength = mlContentLength;

#if (INCLUDE_FORM_DATA_IN_RAW_FILE_POST)
			ContentLength += mlContentLengt
#endif


			if (wload_Post(mpStream, &mUrl, mpContentType, ContentLength) < 0)
			{
				return (-1);
			}
#if (INCLUDE_FORM_DATA_IN_RAW_FILE_POST)
			if (wload_Write(mpStream, mpPostData, mlContentLength) < mlContentLength)
			{
				return (-1);
			}
#endif
			if (wload_WriteFile(mpStream, mFileHandle, mlContentLength) < mlContentLength)
			{
				return (-1);
			}

			mbStreamOpen = 1;
			mbStreamOpenFailed = 0;
			return (0);
		}
	}

	return (-1);
}


#endif

#if (WEBC_SUPPORT_POST_DATABUFFER)

//MA
//Data Buffer POST SetStream Method
//Separating the File and Data buffer Post
PostUploadDataBuffer::PostUploadDataBuffer(HTMLDocument *pDocument, URLDescriptor *pUrl, WEBC_CHAR *pContentType,
								   char *pPostApp, long queryLen, char *pPostData, long pPostDataSize, WebcCharset charset) :
	LoadHTMLDocument(pDocument, pUrl, charset)
{
	mpContentType = webc_malloc_string_copy(pContentType, __FILE__, __LINE__);
	mlContentLength			= 0;
	if(pPostData && pPostDataSize )
		{
		mpPostData				= (char *) WEBC_MALLOC(pPostDataSize*sizeof(char) );
		tc_movebytes(mpPostData, pPostData, pPostDataSize*sizeof(char));
		mlContentLength			= pPostDataSize;  //to be determined, passed to the constructor
		mpPostApp				= (char *) WEBC_MALLOC(queryLen);
		tc_movebytes(mpPostApp, pPostApp, queryLen*sizeof(char));
		}
	else
		{
			mlContentLength			= 0;
			mpPostData				= 0;
			mlContentLength		= 0;
			//WEBC_ASSERT(-1);
			rtp_printf("No Data To POST?? /n/r");
		}
	mDocRef.Set(pDocument);
	if (pDocument)
	{
		mpBrowser = pDocument->GetBrowser();
	}

	// protect against null pointers
	if (pUrl)
	{
		mUrl.set(pUrl);
	}
}

int PostUploadDataBuffer::SetStream(UrlStreamCtx *pStream)
{
	if( mpPostData && mlContentLength )
	{
		mpStream = pStream;
		wload_Init(mpStream, _LoadJobPollCycle, (WEBC_PFBYTE) this);
		wload_SetReportStatusFn(mpStream, (webcReportFn)_DocumentSetStatus, (WEBC_PFBYTE) mDocRef.Get());
		if (!Aborted())
		{
			long ContentLength = mlContentLength;
			mbStreamOpen = 0;
			mbStreamOpenFailed = 0;
			if (wload_Post(mpStream, &mUrl, mpContentType, ContentLength) < 0)
			{
				return (-1);
			}
// Here I need to provide wload_WriteDataBuffer(mpStream,mpPostData, mlContentLength
 			if (wload_Write(mpStream, mpPostData, mlContentLength) < mlContentLength)
			{
				return (-1);
			}

			mbStreamOpen = 1;
			mbStreamOpenFailed = 0;
			return (0);
		}
	}
	return (-1); // missing return pmb
		
}

//Destructor for Data Buffer POST
PostUploadDataBuffer::~PostUploadDataBuffer()
{
//	if (mFileIsOpen)
//		webc_fclose(mFileHandle) ;
	if (mpPostData)
		WEBC_FREE(mpPostData);
	if (mpPostApp)
		WEBC_FREE(mpPostApp);
	webc_free_string_copy(mpContentType, __FILE__, __LINE__);
		
}

#endif


#if (WEBC_SUPPORT_EMBED)
WebcLoadEmbed :: WebcLoadEmbed (URLDescriptor *pUrl, WEBC_CHAR *key, HTMLBrowser *wc)
{
	mUrl.set(pUrl);
	mpStream = 0;
	mbStreamOpen = 0;
	mbStreamOpenFailed = 0;
	//mpImage = 0;
	mpBrowser = wc;
	mpKey = webc_malloc_string_copy(key, __FILE__, __LINE__);

}

WebcLoadEmbed::~WebcLoadEmbed()
{
	if (mpStream)
	{
		// Do protocol-specific shutdown
		if (mbStreamOpen)
			wload_Close(mpStream);
		wload_StreamFree(mpStream, 0);
		mpStream = 0;
		mbStreamOpen = 0;
		mbStreamOpenFailed = 0;
	}
//Need to provide similar functionality release the embed element (being Audio or Video)
	/*if (mpImage)
	{
		mpImage->Release();
	}*/

	webc_free_string_copy(mpKey, __FILE__, __LINE__);
}

int WebcLoadEmbed::SetStream(UrlStreamCtx *pStream)
{
	if (pStream)
	{

		mpStream = pStream;
		wload_Init(mpStream, _LoadJobPollCycle, (WEBC_PFBYTE) this);

		if (!Aborted())
		{
			if (wload_Get(mpStream, &mUrl, 0) >= 0)
			{
				mbStreamOpen = 1;
				mbStreamOpenFailed = 0;
				return (0);
			}
		}
	}

	return (-1);
}
#define TEST_POST_API 0
long WebcLoadEmbed::Execute(void)
{
	long result = -1;
#if (WEBC_SUPPORT_FILE)
	int Bytes_Read = 0;
	long  bytes_in_files;
	WEBC_UINT8 * Embed_buffer = WEBC_NULL;
	WEBC_FILE fp1 = -1;
	bytes_in_files=0;


#if __TIARM__
	result = webc_fopen("m:\\Buffer.mp3", &fp1,WEBC_FILE_WRONLY);
#else
	result = webc_fopen("Buffer.mp3", &fp1,WEBC_FILE_WRONLY);
#endif
	if(result < 0)
	{
		goto clean_up_and_return;
	}

	Embed_buffer = (WEBC_UINT8*) WEBC_MALLOC(WEBC_CFG_BITMAP_BUFFER_SIZE);
	if (!Embed_buffer )
		goto clean_up_and_return;

	if (!mpStream || !mbStreamOpen)
	{
		if (!mpStream)
		{
			mpStream = wload_StreamAlloc(0);
		}
		if (!mpStream || (SetStream(mpStream) < 0))
		{
			result = -1; // WEBC_EMALLOC
			goto clean_up_and_return;
		}
	}


#if (WLOAD_REPORT_STATUS)
	webc_ReportBrowserStatus(mpBrowser, "Reading Embed Data...");
#endif

	do
	{
		Bytes_Read = wload_Read(mpStream, (char*) Embed_buffer, WEBC_CFG_BITMAP_BUFFER_SIZE);
		if (Bytes_Read < 0)
			goto clean_up_and_return;
		else if (Bytes_Read > 0)
		{
		long bytes_written_to_file;
			bytes_written_to_file = webc_fwrite(fp1,(char *)Embed_buffer,Bytes_Read,0,0);
			if(bytes_written_to_file != Bytes_Read)
				goto clean_up_and_return;
		}
	}while(Bytes_Read > 0);

	result = 0;
#if(TEST_POST_API)
#if(0)
	if (webc_fopen ("Buffer.mp3", &fp3, WEBC_FILE_RDONLY) >= 0)
		{
		long Buffer_Size;
		char *POST_BUFFER;
		Buffer_Size		= webc_flength (fp3);
		POST_BUFFER = (char *) WEBC_MALLOC( Buffer_Size * sizeof(char) );
		webc_fread (fp3,POST_BUFFER,Buffer_Size,0,0);
		result = webc_post_UploadDataBuffer((HBROWSER_HANDLE) mpBrowser, POST_BUFFER, Buffer_Size, "10.1.10.6", "/cgi-bin/simplecgi.sh", "../html/uploads/2.pdf");
		WEBC_FREE(POST_BUFFER);
		POST_BUFFER=NULL;
		webc_fclose (fp3);
		}
		result = 0;
#endif
result = webc_post_UploadFileASCII((HBROWSER_HANDLE) mpBrowser,"Buffer.mp3", "10.1.10.6","/cgi-bin/simplecgi.sh", "../html/uploads/1.pdf");

#endif  //TEST_POST_API

clean_up_and_return:
	if (mpStream)
	{
		// Do protocol-specific shutdown
		if (mbStreamOpen)
			wload_Close(mpStream);
		wload_StreamFree(mpStream, 0);
		mpStream = 0;
		mbStreamOpen = 0;
		mbStreamOpenFailed = 0;
	}
	if(Embed_buffer)
	{
		WEBC_FREE(Embed_buffer);
		Embed_buffer = WEBC_NULL;
	}

	if(fp1 != -1) 
		webc_fclose (fp1);
#endif
	return(result);
}

/*************************************************************************************************************************************/
/* Experimental to support Embed, goal is to download the file in the Embed tage													 */
/*************************************************************************************************************************************/

#endif
WebcLoadImage::WebcLoadImage(URLDescriptor *pUrl, WEBC_CHAR *key, HTMLBrowser *wc, WebcImageDitherMode mode)
{
	mUrl.set(pUrl);
	mpStream = 0;
	mbStreamOpen = 0;
	mbStreamOpenFailed = 0;
	mpImage = 0;
	mpBrowser = wc;
	mConvertMode = mode;
	mpKey = webc_malloc_string_copy(key, __FILE__, __LINE__);
}

WebcLoadImage::~WebcLoadImage()
{
	if (mpStream)
	{
		// Do protocol-specific shutdown
		wload_Close(mpStream);
		wload_StreamFree(mpStream, 0);
		mpStream = 0;
		mbStreamOpen = 0;
		mbStreamOpenFailed = 0;
	}

	if (mpImage)
	{
		mpImage->Release();
	}

	webc_free_string_copy(mpKey, __FILE__, __LINE__);
}

int WebcLoadImage::SetStream(UrlStreamCtx *pStream)
{
	if (pStream)
	{

		mpStream = pStream;
		wload_Init(mpStream, _LoadJobPollCycle, (WEBC_PFBYTE) this);

		if (!Aborted())
		{
			if (wload_Get(mpStream, &mUrl, 0) >= 0)
			{
				mbStreamOpen = 1;
				mbStreamOpenFailed = 0;
				return (0);
			}
		}
	}

	return (-1);
}

WebcImage *WebcLoadImage::Image(void)
{
	return (mpImage);
}

long WebcLoadImage::Execute(void)
{
#if (!WEBC_SUPPORT_PNG && !WEBC_SUPPORT_GIFS && !WEBC_SUPPORT_JPGS && !WEBC_SUPPORT_MNG)
	return (-1);
#else

	int result = 0;

	WebcImageDecoder * webImage = WEBC_NULL;

	WebGraphics *gc = GetBrowser()? GetBrowser()->GetDocument()? GetBrowser()->GetDocument()->GetDisplayManager()?
		GetBrowser()->GetDocument()->GetDisplayManager()->GetGraphics() : 0 : 0 : 0;

	if (!gc)
	{
		return (-1);
	}
	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"LoadImage::Execute", URLTOSTR(&mUrl))


	WebcFileContentType bImageType = HTML_MIME_TYPE_UNKNOWN;
	WEBC_UINT8 * buffer;
	buffer = (WEBC_UINT8*) WEBC_MALLOC(WEBC_CFG_BITMAP_BUFFER_SIZE);
	if (!buffer)
	{
		result = -1; // WEBC_EMALLOC
		goto clean_up_and_return;
	}

	ReadCallbackStruct context;
	context.initialBuffer = buffer;
	context.initialBufferSize = WEBC_CFG_BITMAP_BUFFER_SIZE;

	// If we don't already have an open stream to read from, then allocate one and open our URL
	// 	Note: We should be processing mbStreamOpenFailed; for images too ?

	if (!mpStream || !mbStreamOpen)
	{
		if (!mpStream)
		{
			mpStream = wload_StreamAlloc(0);
		}

		if (SetStream(mpStream) < 0)
		{
			result = -1; // WEBC_EMALLOC
			goto clean_up_and_return;
		}
	}

#if (WLOAD_REPORT_STATUS)
	webc_ReportBrowserStatus(mpBrowser, "Reading Image Data...");
#endif

	// Read some data so MIME type is available
	context.initialBufferSize = wload_Read(mpStream, (char*)buffer, WEBC_CFG_BITMAP_BUFFER_SIZE);
	if (context.initialBufferSize <= 0)
	{
		goto clean_up_and_return;
	}

	context.stream = mpStream;

	bImageType = wload_DataType(mpStream);

	// Sometimes the stream layer incorrectly reports the mime type of the
	//  image, so we do crude testing to try to verify the image type.
	if ((context.initialBufferSize >= 3) &&
	    (buffer[0] == 'G') && (buffer[1] == 'I') && (buffer[2] == 'F'))
	{
		bImageType = HTML_MIME_TYPE_GIF;
	}
	else if ((context.initialBufferSize >= 10) &&
	    (buffer[0] == 0xff) && (buffer[2] == 0xff) && (buffer[6] == 'J') &&
	    (buffer[7] == 'F')  && (buffer[8] == 'I')  && (buffer[9] == 'F'))
	{
		bImageType = HTML_MIME_TYPE_JPEG;
	}
	else if ((context.initialBufferSize >= 8) &&
	    (buffer[0] == 137) && (buffer[1] ==  80) && (buffer[2] ==  78) &&
	    (buffer[3] ==  71) && (buffer[4] ==  13) && (buffer[5] ==  10) &&
	    (buffer[6] ==  26) && (buffer[7] ==  10))
	{
		bImageType = HTML_MIME_TYPE_PNG;
	}
	else if ((context.initialBufferSize >= 8) &&
	    (buffer[0] == 0x8a) && (buffer[1] == 'M') && (buffer[2] ==  'N') &&
	    (buffer[3] ==  'G') && (buffer[4] == 0xd) && (buffer[5] ==  0xa) &&
	    (buffer[6] == 0x1a) && (buffer[7] == 0xa))
	{
		bImageType = HTML_MIME_TYPE_MNG;
	}
	else if ((context.initialBufferSize >= 2) &&
	         (buffer[0] == 'B') && (buffer[1] == 'M'))
	{
		bImageType = HTML_MIME_TYPE_BMP;
	}

	// determine image type and allocate the appropriate PegImageConvert object
	switch (bImageType)
	{
		case HTML_MIME_TYPE_MNG:
#if (WEBC_SUPPORT_MNG)
			WEBC_NEW_VERBOSE(webImage,WebcMNGDecoder(),"WebcMNGDecoder");
#endif // WEBC_SUPPORT_MNG
			break;

		case HTML_MIME_TYPE_BMP:
#if (WEBC_SUPPORT_BMP)
			WEBC_NEW_VERBOSE(webImage,WebcBMPDecoder(),"WebcBMPDecoder");
#endif // WEBC_SUPPORT_BMP
			break;


		case HTML_MIME_TYPE_PNG:
#if (WEBC_SUPPORT_PNG)
			WEBC_NEW_VERBOSE(webImage,WebcPNGDecoder(),"WebcPNGDecoder");
#endif // WEBC_SUPPORT_PNG
			break;

		case HTML_MIME_TYPE_JPEG:
#if (WEBC_SUPPORT_JPGS)
		WEBC_NEW_VERBOSE(webImage,WebcJPEGDecoder(),"WebcJPEGDecoder");
#endif // WEBC_SUPPORT_JPGS
			break;

		case HTML_MIME_TYPE_GIF:
#if (WEBC_SUPPORT_GIFS)
			WEBC_NEW_VERBOSE(webImage,WebcGIFDecoder(),"WebcGIFDecoder");
			break;
#endif //	WEBC_SUPPORT_GIFS

		default:
			break;
	}

	if (!webImage)
	{
		result = -1;
		goto clean_up_and_return;
	}

	webImage->setReadCallback(WebcLoadImageReadCallback, (void*)&context);
	webImage->setGraphics(gc);
	mpImage = webImage->go(mpKey);

	if (!mpImage)
	{
		result = -1;
		goto clean_up_and_return;
	}

	switch (webImage->getState())
	{
	case WEBIMAGE_DONE:
	case WEBIMAGE_OKAY:
		break;

	case WEBIMAGE_READ_ERROR:
	case WEBIMAGE_FORMAT_ERROR:
	case WEBIMAGE_MALLOC_ERROR:
	case WEBIMAGE_PARTIAL_ERROR:
		mpImage->Release();
		mpImage = 0;
		result = -1;
		goto clean_up_and_return;
	}

#if (WEBC_SUPPORT_IMAGES)
	WEBC_DELETE(webImage);
	webImage = WEBC_NULL;
#endif // WEBC_SUPPORT_IMAGES

#if (WEBC_CACHE_IMAGES)
	// Finally cache the bitmap
	if (miBitmapCacheInitialized)
	{
		if (_GetBitmapKey(0,&mUrl) < 200)
		{
			WEBC_CHAR pKey[200];
			char pKeyChar[200];
			wcache_stream p_cs[1];

			pKey[0] = 0; // empty string
			pKeyChar[0] = 0;

			_GetBitmapKey(pKey,&mUrl);
			webc_to_c_strcpy(pKeyChar, pKey);

			if (wcache_open(mpBitmapCache,pKeyChar,p_cs,WEBC_CO_CREATE) >= 0)
			{
				if (mpImage->Store(_wload_cache_write, (void *) p_cs) < 0)
				{
					wcache_close(p_cs);
					wcache_delete_entry(mpBitmapCache, pKey);
				}
				else
				{
					wcache_close(p_cs);
				}
				wcache_flush(mpBitmapCache);
			}
		}
	}
#endif

 clean_up_and_return:
	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"LoadImage::Execute", URLTOSTR(&mUrl))
	if (mpStream)
	{
		// Do protocol-specific shutdown
		wload_Close(mpStream);
		wload_StreamFree(mpStream, 0);
		mpStream = 0;
		mbStreamOpen = 0;
		mbStreamOpenFailed = 0;
	}
	if (webImage)
		WEBC_DELETE(webImage);
	if (buffer)
		WEBC_FREE(buffer);
	return (result);
#endif
}

int WebcLoadImageReadCallback(char * buffer, long length, void * context)
{
	ReadCallbackStruct * s = (ReadCallbackStruct *)context;

	size_t n = EBSMIN(length, s->initialBufferSize);
	if (n > 0)
	{
		tc_movebytes(buffer, s->initialBuffer, n);
		s->initialBufferSize -= n;
		s->initialBuffer += n;
		length -= n;
		buffer += n;

		if (length > 0)
		{
			long result = wload_Read(s->stream, buffer, length);
			if (result < 0)
			{
				return (result);
			}
			n += result;
		}
	}
	else
	{
		n = wload_Read(s->stream, buffer, length);
	}

	return ((int) n);
}

LoadHTMLDocumentImage::LoadHTMLDocumentImage(URLDescriptor *pUrl, WEBC_CHAR *key, HTMLDocument *pDoc, HTMLBrowser *pBrowser, WebcImageDitherMode mode)
	: WebcLoadImage(pUrl, key, pBrowser, mode),
	  mDocRef(pDoc)
{
	if (!mpKey)
	{
		if (mDocRef.Get())
		{
			mDocRef.Get()->RemovePendingImage(key);
			mDocRef.Set(0);
		}
	}
}

LoadHTMLDocumentImage::~LoadHTMLDocumentImage(void)
{
	if (mDocRef.Get())
	{
		mDocRef.Get()->RemovePendingImage(mpKey);
	}
}

int LoadHTMLDocumentImage::SetStream(UrlStreamCtx *pStream)
{
	if (pStream)
	{
		mpStream = pStream;
		wload_Init(mpStream, _LoadJobPollCycle, (WEBC_PFBYTE) this);

	  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
		if (mDocRef.Get())
		{
			wload_SetOnlineContentCache(mpStream, mDocRef.Get()->GetOnlineCache());
		}
	  #endif
	  	wload_SetReportStatusFn(mpStream, (webcReportFn)_DocumentSetStatus, (WEBC_PFBYTE) mDocRef.Get());

		if (!Aborted())
		{
			if (wload_Get(mpStream, &mUrl, 0) >= 0)
			{
				mbStreamOpen = 1;
				mbStreamOpenFailed = 0;
				return (0);
			}
		}

	  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
		wload_SetOnlineContentCache(mpStream, 0);
	  #endif
	}

	return (-1);
}

long LoadHTMLDocumentImage::Execute(void)
{
	long result = WebcLoadImage::Execute();

	if ((result >= 0) && Image() && mDocRef.Get())
	{
		NotifyEvent e(NOTIFIER_TYPE_IMAGE, NOTIFY_DONE);
		mDocRef.Get()->Notify(Image(), &e);
	}

	return (result);
}

#if (WEBC_SUPPORT_JSCRIPT)
LoadScript::LoadScript(HTMLScript *pScript, HTMLDocument *pDoc, WebcCharset charset, WEBC_BOOL bExecute)
	: LoadJob(),
	  mElemRef(pScript)
{
	mpDocument = pDoc;
	mpStream = 0;
	mpBrowser = mpDocument->GetBrowser();
	mbExecuteScript = bExecute;
	mCharset = charset;

	if (mpDocument)
	{
		mpDocument->NotifyLoading(pScript);
	}

	if (pScript->Src())
	{
		int result = mUrl.parseURL(pScript->Src(), mpDocument->BaseURL());
	}
}

LoadScript::~LoadScript()
{
	if (mpStream)
	{
		wload_Close(mpStream);
		wload_StreamFree(mpStream, 1);
	}

	if (mpDocument)
	{
		mpDocument->NotifyDone(mElemRef.Get());
	}
}

void LoadScript::Abort()
{
	if (mpDocument)
	{
		mpDocument->NotifyDone(mElemRef.Get());
		mpDocument = 0;
	}

	LoadJob::Abort();
}

int LoadScript::SetStream(UrlStreamCtx *pStream)
{
	if (pStream)
	{
		mpStream = pStream;
		wload_Init(mpStream, 0, (WEBC_PFBYTE) mpBrowser);

	  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
		if (GetDocument())
		{
			wload_SetOnlineContentCache(mpStream, GetDocument()->GetOnlineCache());
		}
	  #endif

		if (!Aborted())
		{
			return (wload_Get(mpStream, &mUrl, 0));
		}

	  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
		wload_SetOnlineContentCache(mpStream, 0);
	  #endif
	}
	return (-1);
}

long LoadScript::Execute()
{
	HTMLScript *pScript = (HTMLScript *) mElemRef.Get();

	if (pScript)
	{
		char *buffer;
		long bufferSize;
		long bufferUsed;
		long bytes_read = 0;
		long total_read = 0;
		long sizeToParse;
		WEBC_CHAR* textToParse;
		WEBC_CHAR* convertBuffer = 0;
		jsval rval;

		if (!mpDocument)
		{
			return -1;
		}

		WebcJSDocumentContext *jsmgr = mpDocument->GetJSMgr();
		WebcJSScript *jsscr = pScript->GetJSScript();
		if (!jsscr)
		{
			return -1;
		}

		if (mpStream)
		{
			buffer = (char *) WEBC_MALLOC(WEBC_LOAD_BUFFER_SIZE);
			if (!buffer)
			{
				return (-1);
			}
			// Read data
			bytes_read = wload_Read(mpStream, buffer, WEBC_LOAD_BUFFER_SIZE-1);

			if (bytes_read < 0)
			{
				WEBC_FREE(buffer);
				return (-1);
			}

			bufferUsed = bytes_read;
			bufferSize = WEBC_LOAD_BUFFER_SIZE - 1;

			WebcCharset streamCharset = wload_Charset(mpStream);
			if (streamCharset == WEBC_CHARSET_UNKNOWN)
			{
				streamCharset = (mCharset == WEBC_CHARSET_UNKNOWN)? WEBC_CHARSET_ASCII : mCharset;
			}

			WebcCharDecodeStream decodeStream;

			decodeStream.pInBuffer     = 0;
			decodeStream.iInBufferSize = 0;
			decodeStream.iBytesRead    = 0;
			decodeStream.pOutBuffer    = 0;
			decodeStream.iOutBufferLen = 0;
			decodeStream.iCharsWritten = 0;
			decodeStream.charset       = streamCharset;
			WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
			WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"LoadScript::Load", URLTOSTR(&mUrl))

			// Main Load Loop - read the ENTIRE Jscript from the stream
			do
			{
				// fill the input buffer
				if ((bytes_read != 0) && (bufferUsed < bufferSize))
				{
					bytes_read = wload_Read(mpStream, buffer + bufferUsed, bufferSize - bufferUsed);

					if (bytes_read < 0)
					{
						break;
					}

					bufferUsed += bytes_read;
				}

				// do conversion (if necessary)
				if (streamCharset == WEBC_DEFAULT_CHARSET)
				{
					// no conversion necessary
					textToParse = (WEBC_CHAR *) buffer;
					sizeToParse = bufferUsed / sizeof(WEBC_CHAR);
				}
				else
				{
					// incoming data must be converted to WEBC_CHAR's
					if (!decodeStream.pOutBuffer)
					{
						if (!convertBuffer)
						{
							convertBuffer = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR)*WEBC_LOAD_BUFFER_SIZE /* tbd config */);
							if (!convertBuffer)
							{
								break;
							}
						}

						decodeStream.pOutBuffer    = convertBuffer;
						decodeStream.iOutBufferLen = WEBC_LOAD_BUFFER_SIZE - 1; // tbd config
						decodeStream.iCharsWritten = 0;
					}

					decodeStream.pInBuffer     = (unsigned char *) buffer;
					decodeStream.iInBufferSize = bufferUsed;
					decodeStream.iBytesRead    = 0;

					webc_decode_string(&decodeStream);

					textToParse = decodeStream.pOutBuffer;
					sizeToParse = decodeStream.iCharsWritten;
				}

				// If we successfully read some data
				if (sizeToParse > 0)
				{
					jsscr->Write(textToParse, sizeToParse);
					total_read += sizeToParse;
				}
				else
				{
					break;
				}

				if (streamCharset == WEBC_DEFAULT_CHARSET)
				{
					// dump parsed chars
					bufferUsed = 0;
				}
				else
				{
					// dump parsed chars and converted input data
					decodeStream.iCharsWritten = 0;

					if (decodeStream.iBytesRead < bufferUsed)
					{
						rtp_memmove(buffer, buffer + decodeStream.iBytesRead, bufferUsed - decodeStream.iBytesRead);
					}
					bufferUsed -= decodeStream.iBytesRead;
					decodeStream.iBytesRead = 0;
				}

			} while (bytes_read > 0);
			WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"LoadScript::Load", URLTOSTR(&mUrl))
			WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"LoadScript::Execute", URLTOSTR(&mUrl))

			if (mbExecuteScript)
			{
				int wasOpen = mpDocument->IsOpen();
				jsscr->Compile();
				jsscr->ExecuteScript(&rval);
				if (!wasOpen)
				{
					mpDocument->Close();
				}
			}
			else
			{
				jsscr->Compile();
			}
			WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"LoadScript::Execute", URLTOSTR(&mUrl))

			if (buffer)
			{
				WEBC_FREE(buffer);
			}

			if (convertBuffer)
			{
				WEBC_FREE(convertBuffer);
			}

			return total_read;
		}
	}

	return -1;
}
#endif//(WEBC_SUPPORT_JSCRIPT)

vector *_GetUrlWaitQueue(void)
{
	if (!url_wait_queue_initialized)
	{
		vector_init(&url_wait_queue, sizeof(struct LoadJobInfo));
		url_wait_queue_initialized = 1;
	}

	return (&url_wait_queue);
}

// URL management routines

#define PEG_DOES_NOT_SUPPORT_TILDE 1


#ifdef __MET__
#pragma Onwarn(39)
#endif


/*****************************************************************************/
// class UrlStringBuilder
/*****************************************************************************/

UrlStringBuilder::UrlStringBuilder(URLDescriptor *desc)
	: AggregateStringBuilder(10)
{
	SetUrlDesc(desc);
}

/*****************************************************************************/

UrlStringBuilder::~UrlStringBuilder()
{
}

/*****************************************************************************/

void UrlStringBuilder::SetUrlDesc(URLDescriptor *desc)
{
	StringBuilder *pDummy;

	Reset();

	if (!desc)
	{
		return;
	}

	int protocol = desc->getProtocol();
	if ((protocol < 0) || (protocol >= NUM_PROTOCOLS))
	{
		return;
	}

	Add(WEBC_NEW(pDummy, StringCopyBuilder(gProtocolTable[protocol].name)), 1);
	Add(WEBC_NEW(pDummy, StringCopyBuilder(WEBC_STR_COLON_SLASH_SLASH)), 1);

	switch (protocol)
	{
	case WEBC_PROTOCOL_FTP:

		if (desc->getUser()[0] && webc_c_strcmp(desc->getUser(), "anonymous"))
		{
			Add(WEBC_NEW(pDummy, StringCopyBuilder(desc->getUser())), 1);
			Add(WEBC_NEW(pDummy, StringCopyBuilder(WEBC_STR_AT)), 1);
		}

	case WEBC_PROTOCOL_HTTP:
	case WEBC_PROTOCOL_HTTPS:

		if (desc->getHost()[0])
		{
			Add(WEBC_NEW(pDummy, StringCopyBuilder(desc->getHost())), 1);
		}

		if (desc->getPort() != gProtocolTable[protocol].port)
		{
			Add(WEBC_NEW(pDummy, StringCopyBuilder(WEBC_STR_COLON)), 1);
			Add(WEBC_NEW(pDummy, DecimalStringBuilder(desc->getPort())), 1);
		}

	case WEBC_PROTOCOL_WEBC:
	case WEBC_PROTOCOL_WEBS:
	case WEBC_PROTOCOL_FILE:
    case WEBC_PROTOCOL_LOCAL:
	case WEBC_PROTOCOL_OFFLINE:

		Add(WEBC_NEW(pDummy, StringCopyBuilder(desc->getPath())), 1);

		if (desc->getAnchor()[0])
		{
			Add(WEBC_NEW(pDummy, StringCopyBuilder(WEBC_STR_SHARP)), 1);
			Add(WEBC_NEW(pDummy, StringCopyBuilder(desc->getAnchor())), 1);
		}

		break;
	}
}

/*****************************************************************************/

int _DocumentSetStatus(WEBC_PFBYTE ctx, WEBC_CHAR *string)
{
	HTMLDocument* doc = (HTMLDocument*) ctx;
	if (doc)
	{
		HTMLBrowser *pBrowser = doc->GetBrowser();
		if (pBrowser)
		{
			pBrowser->SetDefaultStatus(string);
		}
	}

	return (0);
}

/*****************************************************************************/

#if (WEBC_SUPPORT_INTERNAL)

int wload_WebsGet(UrlStreamCtx *pStream)
{
	return (-1);
}

int  wload_WebsPost(UrlStreamCtx *pStream, WEBC_CHAR * pContentType, long lContentLength)
{
	return (-1);
}

long wload_WebsRead(UrlStreamCtx *pStream, char * buffer, long length)
{
	return (-1);
}

long wload_WebsWrite(UrlStreamCtx *pStream, char * buffer, long length)
{
	return (-1);
}

WebcFileContentType  wload_WebsDataType(UrlStreamCtx *pStream)
{
	return WEBC_FILE_TYPE_UNKNOWN;
}

int  wload_WebsClose(UrlStreamCtx *pStream)
{
	return (-1);
}

#endif // WEBC_SUPPORT_INTERNAL


#if (WEBC_SUPPORT_URL_MACROS)

extern "C"{
static int _matchUrlMacro(WEBC_PFBYTE p_entry, WEBC_PFBYTE p_name)
{
	return ( !webc_strncmp(((UrlMacroEntry *)p_entry)->name, (WEBC_CHAR *) p_name, WEBC_CFG_MAX_URL_MACRO_NAME_LEN) );
}

static int _matchUrlMacroAscii(WEBC_PFBYTE p_entry, WEBC_PFBYTE p_name)
{
	return ( !webc_c_strncmp(((UrlMacroEntry *)p_entry)->name, (char *) p_name, WEBC_CFG_MAX_URL_MACRO_NAME_LEN) );
}
} // extern "C"
// Replace all %<macro>%'s in a string; return the length of the resultant string
int webc_replace_macros(WEBC_CHAR * dest, const WEBC_CHAR * src, long maxLen)
{
	long charsCopied = 0, numCharsToCopy;
	const WEBC_CHAR *p_name, *p_end;
	UrlMacroEntry *p_entry;

	while (maxLen > charsCopied)
	{
		p_name = webc_c_strstr(src, "%");
		if (!p_name)
		{
			break;
		}

		// copy everything before the % verbatum (not more than maxLen)
		numCharsToCopy = EBSMIN((p_name - src), (maxLen - charsCopied));
		tc_movebytes(dest, src, numCharsToCopy * sizeof(WEBC_CHAR));
		dest += numCharsToCopy;
		charsCopied += numCharsToCopy;

		if (!(maxLen > charsCopied))
		{
			break;
		}

		// if the first or second char after the '%' is not a hex digit then we know this is
		//  not a url-encoded escape sequence; so we assume its one of our freaky macro thangs
		if (!IS_HEX_DIGIT(p_name[1]) || !IS_HEX_DIGIT(p_name[2]))
		{
			// increment past the '%' and lookup the macro
			p_end = webc_c_strstr(p_name + 1, "%");
			if (p_end)
			{
				int testLen = p_end - (p_name + 1);
				WEBC_CHAR testName[WEBC_CFG_MAX_URL_MACRO_NAME_LEN + 1];

				webc_strncpy(testName, p_name + 1, EBSMIN(testLen, WEBC_CFG_MAX_URL_MACRO_NAME_LEN));
				testName[EBSMIN(testLen, WEBC_CFG_MAX_URL_MACRO_NAME_LEN)] = 0;

				p_entry = (UrlMacroEntry *) vector_find_node(&_gUrlMacroVector, _matchUrlMacro, (WEBC_PFBYTE) testName);
				if (p_entry)
				{
					int macro_len = p_entry->macro(0, dest, maxLen - charsCopied);
					dest += macro_len;
					charsCopied += macro_len;
				}
				p_name = p_end;
			}
		}
		else
		{
			dest[0] = '%';
			dest++;
			charsCopied++;
		}

		src = p_name + 1;
	}

	WEBC_ASSERT(maxLen >= charsCopied);

	// copy the remainder of the string
	webc_strncpy(dest, src, EBSMAX(0, maxLen - charsCopied));

	return (charsCopied);
}

#endif // (WEBC_SUPPORT_URL_MACROS)


int webc_define_url_macro(char * name, WebcUrlMacro macro)
{
  #if (WEBC_SUPPORT_URL_MACROS)
	UrlMacroEntry umentry;

	webc_c_strncpy(umentry.name, name, WEBC_CFG_MAX_URL_MACRO_NAME_LEN);
	umentry.name[WEBC_CFG_MAX_URL_MACRO_NAME_LEN] = 0;
	umentry.macro = macro;

	if (!_giUrlMacrosInitialized)
	{
		vector_init(&_gUrlMacroVector, sizeof(UrlMacroEntry));
		_giUrlMacrosInitialized = 1;
	}

	if (vector_add_node_to_front(&_gUrlMacroVector, (WEBC_PFBYTE) &umentry) < 0)
	{
		return (-1);
	}

	return (0);
  #else
	return (-1);
  #endif
}

// find 'name' in the _gUrlMacroVector and delete that node
int webc_undef_url_macro(char * name)
{
  #if (WEBC_SUPPORT_URL_MACROS)
	return vector_delete_cond(&_gUrlMacroVector, _matchUrlMacroAscii, (WEBC_PFBYTE) name);
  #else
	return (-1);
  #endif
}

int webc_undef_all_url_macros(void)
{
  #if (WEBC_SUPPORT_URL_MACROS)
	vector_delete(&_gUrlMacroVector);
	return (0);
  #else
	return (-1);
  #endif
}


#if (WEBC_CACHE_IMAGES)

static long _wload_cache_read (void *handle, WEBC_UINT8 *buffer, long len)
{
	wcache_stream *wcs = (wcache_stream *) handle;

	return (wcache_read(wcs, buffer, len));
}

static long _wload_cache_write (void *handle, WEBC_UINT8 *buffer, long len)
{
	wcache_stream *wcs = (wcache_stream *) handle;

	return (wcache_write(wcs, buffer, len));
}

#endif


#if (WEBC_SUPPORT_STYLE_SHEETS)

CSSLoadExternalSheet::CSSLoadExternalSheet()
{
	mpCSSContext = WEBC_NULL;
	mpURLStream = WEBC_NULL;
	mCharset = WEBC_CHARSET_UNKNOWN;
}

CSSLoadExternalSheet::CSSLoadExternalSheet(
		CSSDocumentContext * pCSSContext,
		URLDescriptor * url,
		WebcCharset charset)
{
	mpCSSContext = pCSSContext;
	mUrl.set(url);
	mpBrowser = 0;
	mbStreamOpen = 0;
	mbStreamOpenFailed = 0;
	mpURLStream = WEBC_NULL;
	mCharset = charset;
	mResumeRefresh = WEBC_FALSE;

	if (mpCSSContext)
	{
		mpCSSContext->Lock();

		HTMLDocument *pDoc = mpCSSContext->GetHTMLDocument();
		if (pDoc)
		{
			DisplayManager *pManager = pDoc->GetDisplayManager();

			if (pManager)
			{
				mResumeRefresh = WEBC_TRUE;
				DISPLAY_MANAGER_STOP_REFRESH(pManager);
			}

			mpBrowser = mpCSSContext->GetHTMLDocument()->GetBrowser();
		}
	}
}

CSSLoadExternalSheet::~CSSLoadExternalSheet()
{
	if (mpURLStream)
	{
		// Do protocol-specific shutdown
		wload_Close(mpURLStream);
		wload_StreamFree(mpURLStream, 0);
		mbStreamOpen = 0;
		mbStreamOpenFailed = 0;
		mpURLStream = 0;
	}

	if (mpCSSContext)
	{
		CSSDocumentContext* ctx = mpCSSContext;
		mpCSSContext = 0;

		HTMLDocument *pDoc = ctx->GetHTMLDocument();
		if (pDoc)
		{
			DisplayManager *pManager = pDoc->GetDisplayManager();

			if (pManager && mResumeRefresh)
			{
				DISPLAY_MANAGER_RESUME_REFRESH(pManager);
			}

			pDoc->Refresh();

			if (pDoc->CurrentURL()->getAnchor()[0])
			{
				pDoc->ScrollToAnchor(pDoc->CurrentURL()->getAnchor());
			}
		}

		ctx->UnLock();
	}
}

void CSSLoadExternalSheet::Abort()
{
	if (mpCSSContext)
	{
		CSSDocumentContext* ctx = mpCSSContext;
		mpCSSContext = 0;

		HTMLDocument *pDoc = ctx->GetHTMLDocument();
		if (pDoc)
		{
			DisplayManager *pManager = pDoc->GetDisplayManager();

			if (pManager && mResumeRefresh)
			{
				DISPLAY_MANAGER_RESUME_REFRESH(pManager);
			}
		}

		ctx->UnLock();
	}

	LoadJob::Abort();
}

int CSSLoadExternalSheet::SetStream(UrlStreamCtx * stream)
{
	mpURLStream = stream;
	if (mpURLStream)
	{
		wload_Init(mpURLStream, _LoadJobPollCycle, (WEBC_PFBYTE) this);

	  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
		if (GetDocument())
		{
			wload_SetOnlineContentCache(mpURLStream, GetDocument()->GetOnlineCache());
		}
	  #endif
		wload_SetReportStatusFn(mpURLStream, (webcReportFn) _DocumentSetStatus, (WEBC_PFBYTE) GetDocument());
		if (!Aborted())
		{
			if (wload_Get(mpURLStream, &mUrl, 0) >= 0)
			{
				mbStreamOpen = 1;
				mbStreamOpenFailed = 0;
				return 0;
			}
		}

	  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
		wload_SetOnlineContentCache(mpURLStream, 0);
	  #endif
	}

	return -1;
}

long CSSLoadExternalSheet::Execute(void)
{

#define BUFFER_SIZE	4096

	long result = 0;
	long bytes_read;
	char * buffer;
	long bufferSize;
	long bufferUsed;
	WEBC_CHAR *textToParse = 0;
	WEBC_CHAR *convertBuffer = 0;
	long sizeToParse;

	if (!mpURLStream || !mpCSSContext || mpCSSContext->Deleted())
	{
		return -1;
	}

	buffer = (char *) WEBC_MALLOC(BUFFER_SIZE);
	if (!buffer)
	{
		return -1;
	}
	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"CSS::Execute", URLTOSTR(&mUrl))


	bytes_read = wload_Read(mpURLStream, buffer, BUFFER_SIZE - 1);

	if (bytes_read < 0 || Aborted())
	{
		WEBC_FREE(buffer);
		return (-1);
	}
	bufferUsed = bytes_read;
	bufferSize = BUFFER_SIZE - 1;

	WebcCharset streamCharset = wload_Charset(mpURLStream);
	if (streamCharset == WEBC_CHARSET_UNKNOWN)
	{
		streamCharset = (mCharset == WEBC_CHARSET_UNKNOWN)? WEBC_CHARSET_ASCII : mCharset;
	}

	WebcCharDecodeStream decodeStream;

	decodeStream.pInBuffer     = 0;
	decodeStream.iInBufferSize = 0;
	decodeStream.iBytesRead    = 0;
	decodeStream.pOutBuffer    = 0;
	decodeStream.iOutBufferLen = 0;
	decodeStream.iCharsWritten = 0;
	decodeStream.charset       = streamCharset;

	do
	{
		// fill the input buffer
		if ((bytes_read != 0) && (bufferUsed < bufferSize))
		{
			bytes_read = wload_Read(mpURLStream, buffer + bufferUsed, bufferSize - bufferUsed);

			if (bytes_read < 0 || Aborted())
			{
				result = -1; // io read error
				break;
			}

			bufferUsed += bytes_read;
		}

		// do conversion (if necessary)
		if (streamCharset == WEBC_DEFAULT_CHARSET)
		{
			// no conversion necessary
			textToParse = (WEBC_CHAR *) buffer;
			sizeToParse = bufferUsed / sizeof(WEBC_CHAR);
		}
		else
		{
			// incoming data must be converted to WEBC_CHAR's
			if (!decodeStream.pOutBuffer)
			{
				if (!convertBuffer)
				{
					convertBuffer = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR)*BUFFER_SIZE /* tbd config */);
					if (!convertBuffer)
					{
						result = -1; // malloc failure
						break;
					}
				}

				decodeStream.pOutBuffer    = convertBuffer;
				decodeStream.iOutBufferLen = BUFFER_SIZE - 1; // tbd config
				decodeStream.iCharsWritten = 0;
			}

			decodeStream.pInBuffer     = (unsigned char *) buffer;
			decodeStream.iInBufferSize = bufferUsed;
			decodeStream.iBytesRead    = 0;

			webc_decode_string(&decodeStream);

			textToParse = decodeStream.pOutBuffer;
			sizeToParse = decodeStream.iCharsWritten;
		}

		// parse the data
		if (sizeToParse > 0 && textToParse && mpCSSContext)
		{
		WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
			WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"ParseStyleSheet", URLTOSTR(&mUrl))

			textToParse[sizeToParse] = (WEBC_CHAR) 0;
			mpCSSContext->ParseStyleSheet(textToParse, CSS_ORIGIN_EXTERNAL, &mUrl, 0);
			WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"ParseStyleSheet", URLTOSTR(&mUrl))
			if (mpCSSContext->Deleted())
			{
				break;
			}
		}
		else
		{
			break;
		}

		if (streamCharset == WEBC_DEFAULT_CHARSET)
		{
			// dump parsed chars
			bufferUsed = 0;
		}
		else
		{
			// dump parsed chars and converted input data
			decodeStream.iCharsWritten = 0;

			if (decodeStream.iBytesRead < bufferUsed)
			{
				rtp_memmove(buffer, buffer + decodeStream.iBytesRead, bufferUsed - decodeStream.iBytesRead);
			}
			bufferUsed -= decodeStream.iBytesRead;
			decodeStream.iBytesRead = 0;
		}

	} while (bytes_read > 0);

	if (textToParse)
	{
		WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
		WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"ParseStyleSheet", URLTOSTR(&mUrl))
		textToParse[0] = 0;

		mpCSSContext->ParseStyleSheet(textToParse, CSS_ORIGIN_EXTERNAL, &mUrl, 1);
		WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"ParseStyleSheet", URLTOSTR(&mUrl))
	}
	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"CSS::Execute", URLTOSTR(&mUrl))
#if (1)
	// Doing this causes over painting the CSS stream being reset over and over, not sure why
	// Bug fix June 2010, was not closing the stream which resulted in persistent connection support to fail.
	if (mpURLStream)
    {
        // Do protocol-specific shutdown
        wload_Close(mpURLStream);
        wload_StreamFree(mpURLStream, 0);
        mbStreamOpen = 0;
		mbStreamOpenFailed = 0;
        mpURLStream = 0;
    }
#endif
	WEBC_FREE(buffer);
	if (convertBuffer)
	{
		WEBC_FREE(convertBuffer);
	}

	if (!mpCSSContext->Deleted())
	{
		mpCSSContext->CleanUp();
	}
	return result;

#undef BUFFER_SIZE

}

#endif // (WEBC_SUPPORT_STYLE_SHEETS)


#if(WEBC_SUPPORT_ERROR_PAGE_REDIRECT)

#define MAX_REDIRECT_URLS 4
static char *RedirectUrls[MAX_REDIRECT_URLS];
/* Called from webc_init to clear the redirect array */
void webc_ClearErrorRedirects(void)
{
	rtp_memset(&RedirectUrls[0],0, sizeof(RedirectUrls));
}

/* API call to set the url to redirect to when not found errors occur */
int webc_setErrorRedirect(int url_error, const char * url)
{
	int index;
	int retval = -1;
	if(url)
	{
		index = webc_urlerror2Index(url_error);
		if(index >= 0)
		{
			RedirectUrls[index] = (char *)url;
			retval = 1;
		}
	}
	return(retval);
}

/* Gets the index corresponding to the url_error */
static char * webc_getErrorRedirect(int url_error)
{
	int index;
	index = webc_urlerror2Index(url_error);
	if (index < 0)
		return(0);
	return(RedirectUrls[index]);
}

/* Finds the index based on the error passed to it*/
static int webc_urlerror2Index(int url_error)
{
	int RedirectPageIndex;
	switch(url_error)
	{
	case URL_ERROR_SERVER_NOT_FOUND:
			 RedirectPageIndex = 0;
		break;
	case URL_ERROR_PAGE_NOT_FOUND:
			 RedirectPageIndex = 1;
		break;
	case URL_ERROR_PAYMENT_REQ:
			 RedirectPageIndex = 2;
		break;
	case URL_ERROR_FORBIDDEN:
			 RedirectPageIndex = 3;
		break;
	case URL_ERROR_PAGE_NOT_SET:
			 RedirectPageIndex = 4;
		break;
	default:
		RedirectPageIndex = -1;
		break;
	}
	return(RedirectPageIndex);
}

/* Returns the html page with the RedirectUrl */
static void getHtmlPageWithRedirectUrl(char *tobuffer, char *RedirectUrl)
{
	const char *str1 = "<html><head><META HTTP-EQUIV=refresh CONTENT=0;%s></head></html>";
    rtp_sprintf(tobuffer, str1, RedirectUrl);
}
#endif //WEBC_SUPPORT_ERROR_PAGE_REDIRECT
