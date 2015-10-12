//
// HTTPC.CPP - HTTP Client
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//    This module contains HTTP client routines.
//

/*****************************************************************************/
// Include files
/*****************************************************************************/

#include "webcjhconfig.h"

#include "httpc.h"
#include "wcache.h"
#include "util.h"
#include "webcsock.h"
#include "html.h"
#include "webc.h"
#include "cookie.h"
#include "webctmo.h"
#include "webcstr.h"
#include "rtpprint.h"
#include "rtptime.h"
#include "rtpscnv.h"
#include "webcmem.h"
#include "webcfg.h"

/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define HTTPC_DEFAULT_PORT            80

/* this controls the maximum HTTP connections active at any one time */
#define HTTPC_CFG_MAX_CONNECTIONS     20
#define HTTPC_SUPPORT_PIPELINE        0     // DO NOT CHANGE
#define HTTPC_SUPPORT_KEEPALIVE       0     // Set to use persistant connections
#define HTTPC_SUPPORT_AUTHENTICATE    1     // Set to enable HTTP authentication

#define WEBC_HTTP_KEEPALIVE_TIMEOUT   10000 // in milliseconds

#if (HTTPC_SUPPORT_AUTHENTICATE)
 #define HTTPC_CFG_MAX_HOSTNAME_LEN   80
 #define HTTPC_CFG_MAX_PASSWORD_LEN   16
 #define HTTPC_CFG_MAX_PATH_LEN       256
 #define HTTPC_CFG_MAX_REALMNAME_LEN  36
 #define HTTPC_CFG_MAX_USERNAME_LEN   32
 #define HTTPC_CFG_AUTH_TTL_SEC       (60*3)
#endif

#define HTTP_IO_NEW(F)               (*(F))->newStream((F))
#define HTTP_IO_ACTIVATE(F,S)        (*(F))->activate((F),(S))
#define HTTP_IO_CONNECT(S,A,P)       (*(S))->connectTo((S),(A),(P))
#define HTTP_IO_GETHOSTBYNAME(S,A,N) (*(S))->getHostAddr((S),(A),(P))
#define HTTP_IO_ISCONNECTED(S)       (*(S))->isConnected((S))
#define HTTP_IO_SEND(S,D,L)          (*(S))->sendData((S),(D),(L))
#define HTTP_IO_RECV(S,D,N,X)        (*(S))->recvData((S),(D),(N),(X))
#define HTTP_IO_CLOSE(S)             (*(S))->closeStream((S))
#define HTTP_IO_CACHE(S)             (*(S))->cacheStream((S))

#if (WEBC_SUPPORT_HTTP || WEBC_SUPPORT_HTTPS)

/*****************************************************************************/
// Local types
/*****************************************************************************/

typedef int (*HttpParseFn) (struct httpc_ctx *ctx, char *value);

struct HttpHeaderEntry
{
	char       *pName;
	HttpParseFn Parse;
};


#if (HTTPC_SUPPORT_KEEPALIVE)
struct HttpSocketHolder
{
	WEBC_BOOL         valid;
	HTTPStreamClass** stream;
	WEBC_UINT8        ip_addr[4];
	WEBC_UINT16       port;
	WEBC_UINT32       closeTime;
};
#endif // HTTPC_SUPPORT_KEEPALIVE

#if (HTTPC_SUPPORT_AUTHENTICATE)

// if a realm is used to authenticate and the authentication fails, it is
//  deleted from the client-side
typedef struct s_HTTPAuthenticationRealm
{
	char realmName[HTTPC_CFG_MAX_REALMNAME_LEN];
	char path[HTTPC_CFG_MAX_PATH_LEN];
	RTP_TIMESTAMP expires;
	long ttlSec;
	HTTPAuthenticationScheme scheme; // "Basic", "Digest", etc
	union
	{
		struct
		{
			char userName[HTTPC_CFG_MAX_USERNAME_LEN];
			char password[HTTPC_CFG_MAX_PASSWORD_LEN];
		} basic;

		struct
		{
			char userName[HTTPC_CFG_MAX_USERNAME_LEN];
			char password[HTTPC_CFG_MAX_PASSWORD_LEN];
		} digest;

	} param;

	struct s_HTTPAuthenticationHost *host;
	struct s_HTTPAuthenticationRealm *pPrev;
	struct s_HTTPAuthenticationRealm *pNext;

} HTTPAuthenticationRealm;

typedef struct s_HTTPAuthenticationHost
{
	char hostName[HTTPC_CFG_MAX_HOSTNAME_LEN];
	unsigned short port;
	HTTPAuthenticationRealm *pFirstRealm;
	HTTPAuthenticationRealm *pLastRealm;
	struct s_HTTPAuthenticationHost *pPrev;
	struct s_HTTPAuthenticationHost *pNext;

} HTTPAuthenticationHost;

#endif // HTTPC_SUPPORT_AUTHENTICATE


/*****************************************************************************/
// Local functions
/*****************************************************************************/

int httpc_done(struct httpc_ctx *ctx);
int http_escape_path_string(char *dest, const char *src);
long _httpc_sock_read(struct httpc_ctx *ctx, char *buffer, long size);
long _httpc_sock_readline(struct httpc_ctx *ctx, char *buffer, long size);
int _httpc_process_one_header(struct httpc_ctx *ctx, char *buffer);
int _httpc_parse_connection(struct httpc_ctx *ctx, char *value);
int _httpc_parse_content_length(struct httpc_ctx *ctx, char *value);
int _httpc_parse_content_type(struct httpc_ctx *ctx, char *value);
int _httpc_parse_date(struct httpc_ctx *ctx, char *value);
int _httpc_parse_expires(struct httpc_ctx *ctx, char *value);
int _httpc_parse_location(struct httpc_ctx *ctx, char *value);
int _httpc_parse_set_cookie(struct httpc_ctx *ctx, char *value);
int _httpc_parse_transfer_encoding(struct httpc_ctx *ctx, char *value);
int _httpc_parse_last_modified(struct httpc_ctx *ctx, char *value);
int _httpc_parse_name_value_pair (char *str, char *name, char *value, long nameSize, long valueSize);

#if (WEBC_SUPPORT_HTTP_CACHE)
static int _http_init_cache (void);
#endif

#if (HTTPC_SUPPORT_AUTHENTICATE)

int _httpc_parse_www_authenticate(struct httpc_ctx *ctx, char *value);

/* Authentication Realm lookup functions */
HTTPAuthenticationRealm * _http_create_authentication_realm      (char *hostName, unsigned short port, char *realmName,
                                                                  char *path, long ttlSec, HTTPAuthenticationScheme scheme);
HTTPAuthenticationRealm * _http_get_authentication_realm         (char *hostName, unsigned short port, char *realmName);
HTTPAuthenticationRealm * _http_get_authentication_realm_by_path (char *hostName, unsigned short port, char *path);
HTTPAuthenticationHost  * _http_get_authentication_host          (char *hostName, unsigned short port);
void                      _http_add_authentication_host          (HTTPAuthenticationHost *host);
void                      _http_remove_authentication_host       (HTTPAuthenticationHost *host);

/* Authentication Host management functions */
HTTPAuthenticationHost  * _http_new_host (char *hostName, unsigned short port);

void _http_delete_host       (HTTPAuthenticationHost *host);
void _http_host_add_realm    (HTTPAuthenticationHost *host, HTTPAuthenticationRealm *realm);
void _http_host_remove_realm (HTTPAuthenticationHost *host, HTTPAuthenticationRealm *realm);

/* Authentication Realm management functions */
HTTPAuthenticationRealm * _http_new_realm (char *realmName, char *path, long ttlSec,
                                           HTTPAuthenticationScheme scheme);

void _http_delete_realm              (HTTPAuthenticationRealm *realm);
void _http_realm_set_user            (HTTPAuthenticationRealm *realm, char *user);
void _http_realm_set_password        (HTTPAuthenticationRealm *realm, char *password);
void _http_realm_set_ttl             (HTTPAuthenticationRealm *realm, long ttlSec);
long _http_realm_print_authorization (HTTPAuthenticationRealm *realm, char *buffer);
void _http_realm_update_path         (HTTPAuthenticationRealm *realm, char *path);
int  _http_realm_expired             (HTTPAuthenticationRealm *realm);
void _http_check_realm_expirations   (void);

/* Authentication formatting functions */
long _http_print_authorization (struct httpc_ctx *ctx, char *buffer);
long _http_base64_encode (char *dst, unsigned char *src, long size);

#endif // HTTPC_SUPPORT_AUTHENTICATE


/*****************************************************************************/
// Local data
/*****************************************************************************/

#if (HTTPC_SUPPORT_KEEPALIVE)
 struct HttpSocketHolder gHttpSocketPool[HTTPC_CFG_MAX_CONNECTIONS] = {0};
#endif // HTTPC_SUPPORT_KEEPALIVE

static const struct HttpHeaderEntry gHttpHeaderTable [] =
{
	{"Connection",        _httpc_parse_connection},
	{"Content-Length",    _httpc_parse_content_length},
	{"Content-Type",      _httpc_parse_content_type},
	{"Date",              _httpc_parse_date},
	{"Expires",           _httpc_parse_expires},
	{"Last-Modified",     _httpc_parse_last_modified},
	{"Location",          _httpc_parse_location},
	{"Set-Cookie",        _httpc_parse_set_cookie},
	{"Transfer-Encoding", _httpc_parse_transfer_encoding},
  #if (HTTPC_SUPPORT_AUTHENTICATE)
	{"WWW-Authenticate",  _httpc_parse_www_authenticate},
  #endif
	{0,0} // array is null-terminated
};

static const int mime_types[] =
{
	HTML_MIME_TYPE_JSCRIPT,
	HTML_MIME_TYPE_GIF,
	HTML_MIME_TYPE_JPEG,
	HTML_MIME_TYPE_PNG,
	HTML_MIME_TYPE_CSS,
	HTML_MIME_TYPE_HTML,
	HTML_MIME_TYPE_PLAIN_TEXT,
	HTML_MIME_TYPE_MNG
};

#if (WEBC_SUPPORT_HTTP_CACHE)
static int giHttpCacheInitialized = 0;
static struct wcache_ctx gHttpCache;
#endif  // WEBC_SUPPORT_HTTP_CACHE

#if WEBC_SUPPORT_HTTP_PROXY
int  WebcHttpProxyEnable = WEBC_HTTP_PROXY_ENABLE;
WEBC_UINT8 WebcHttpProxyIp[4]  = WEBC_HTTP_PROXY_IP;
int  WebcHttpProxyPort   = WEBC_HTTP_PROXY_PORT;
#endif // WEBC_SUPPORT_HTTP_PROXY

#if (HTTPC_SUPPORT_AUTHENTICATE)

static const char authorization_prefix[] =
		"\r\nAuthorization: ";

static const char basic_string[] =
		"Basic ";

HTTPAuthenticationHost * gpHttpFirstHost = 0;
HTTPAuthenticationHost * gpHttpLastHost = 0;

static const char _base64[64] =
{
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
	'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
	'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
	'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};
#endif // HTTPC_SUPPORT_AUTHENTICATE


/*****************************************************************************/
// API routines
/*****************************************************************************/


/*************************************************************************
 http_clear_cache.

*************************************************************************/
#if (WEBC_SUPPORT_CACHE)
void http_clear_cache(void)
{
	wcache_clear(&gHttpCache);
}
#endif

/*************************************************************************
 http_init() - Initialize httpc_ctx.

 ctx - httpc_ctx to initialize.

 Returns: 0 on success, -1 otherwise.
*************************************************************************/

int webc_http_init(struct httpc_ctx *ctx, HTTPStreamFactoryClass** factory)
{
#if (WEBC_SUPPORT_HTTP_CACHE)
	_http_init_cache();
#endif // WEBC_SUPPORT_HTTP_CACHE

	tc_memset(ctx, 0, sizeof(struct httpc_ctx));
	ctx->mime_data_type = HTML_MIME_TYPE_UNKNOWN;
	ctx->mime_charset = WEBC_CHARSET_UNKNOWN;
	ctx->factory = factory;

	return(0);
}


/*************************************************************************
 http_set_report_fn() - Set the status reporting function for an HTTP
 	context

 ctx - http context to use
 report_func - function to use when reporting status
 report_ctx - argument to pass to report_func

 Returns: 0 on success, -1 otherwise.
*************************************************************************/

int http_set_report_fn(struct httpc_ctx *ctx, webcReportFn report_func, WEBC_PFBYTE report_ctx)
{
  #if (HTTP_REPORT_STATUS)
	ctx->report_func = report_func;
	ctx->report_ctx  = report_ctx;
  #endif
	return (0);
}


/*************************************************************************
 http_report_status() - Report HTTP client status (internal use only)

 ctx - http context to use
 status - string to report

 Returns: 0 on success, -1 otherwise.
*************************************************************************/

#if (HTTP_REPORT_STATUS)
void http_report_status(struct httpc_ctx *ctx, char *status)
{
	if (ctx->report_func)
	{
	  #if (WEBC_SUPPORT_UNICODE)
		WEBC_CHAR temp[256];
		webc_c_strncpy(temp, status, 255);
		temp[255] = 0;
		ctx->report_func(ctx->report_ctx, temp);
	  #else
		ctx->report_func(ctx->report_ctx, status);
	  #endif
	}
}
#endif


/*************************************************************************
 http_exit() - Free all global resources used by http client

 To be called on program exit.

 Returns: 0 on success, -1 otherwise.
*************************************************************************/

int http_exit(void)
{
#if (WEBC_SUPPORT_HTTP_CACHE)
	if (giHttpCacheInitialized)
	{
		wcache_free(&gHttpCache);
		httpc_destroy_cache_storage();
		giHttpCacheInitialized = 0;
	}
#endif

#if (WEBC_SUPPORT_COOKIES)
	webc_delete_cookies();
#endif

#if (HTTPC_SUPPORT_KEEPALIVE)
	{
		int n;

		// check our socket pool to see if someone left a connection open
		for (n=0; n<HTTPC_CFG_MAX_CONNECTIONS; n++)
		{
			if (gHttpSocketPool[n].valid)
			{
				// close all open connections
				HTTP_IO_CLOSE(gHttpSocketPool[n].stream);
				gHttpSocketPool[n].valid = WEBC_FALSE;
			}
		}
	}
#endif // HTTPC_SUPPORT_KEEPALIVE

	return (0);
}


#define LOG_HTTP_PROGRESS 0 /* Simple progress logging for debugging new protocol uses */
#if (LOG_HTTP_PROGRESS==0)
#define http_log_message(M)
#else
static void _http_log_message(char *m)
{
	printf("HTTP:: t== %d %s\n", rtp_get_system_msec(), m);
}
#define http_log_message(M) _http_log_message(M);
#endif

/*************************************************************************
 http_connect() - Connect to a host

 Called by http_get and http_post

 Returns: 0 on success, -1 otherwise.
*************************************************************************/


int http_connect(struct httpc_ctx *ctx, WEBC_PFBYTE ip_addr, WEBC_UINT16 port)
{
	http_log_message("Connect called\n")
	if (ctx->factory == WEBC_NULL)
	{
		return (-1);
	}
#if (HTTPC_SUPPORT_KEEPALIVE)

	if (ctx->flags & HTTPC_FLAG_USE_KEEPALIVE)
	{
		int n;
		// first check our socket pool to see if someone left a connection open
		for (n=0; n<HTTPC_CFG_MAX_CONNECTIONS; n++)
		{
			if (gHttpSocketPool[n].valid)
			{
				if (HTTP_IO_ISCONNECTED(gHttpSocketPool[n].stream))
				{
					ctx->stream = HTTP_IO_ACTIVATE(ctx->factory, gHttpSocketPool[n].stream);
					if (!ctx->stream)
					{
						continue;
					}

					// we have a match!
					if (tc_comparen(gHttpSocketPool[n].ip_addr, ip_addr, 4) && (gHttpSocketPool[n].port == port))
					{
						// alloc a buffer for socket data
						ctx->in_buffer = (WEBC_UINT8 *) WEBC_MALLOC_VERBOSE(WEBC_CFG_HTTP_BUFFER_SIZE,"HttpInbuffer");
						ctx->in_buffer_size = WEBC_CFG_HTTP_BUFFER_SIZE;
						ctx->in_buffer_begin_ptr = 0;
						ctx->in_buffer_end_ptr = 0;

						if (!ctx->in_buffer)
						{
							ctx->in_buffer_size = 0;
							return (-1);
						}

						ctx->stream = gHttpSocketPool[n].stream;
						tc_movebytes(ctx->ip_addr, ip_addr, 4);
						ctx->port = port;
						gHttpSocketPool[n].valid = WEBC_FALSE;

						return (0);
					}
				}
				else // even if this one doesn't match, we want to free up the slot if the
				     //  connection has died
				{
					HTTP_IO_CLOSE(gHttpSocketPool[n].stream);
					gHttpSocketPool[n].valid = WEBC_FALSE;
				}
			}
		}
	}

#endif // HTTPC_SUPPORT_KEEPALIVE
	ctx->stream = HTTP_IO_NEW(ctx->factory);
	if (ctx->stream == WEBC_NULL)
	{
		return (-1);
	}

#if WEBC_SUPPORT_HTTP_PROXY
	if (WebcHttpProxyEnable)
	{
		// convert host ip/port address to use the proxy's
		ip_addr[0] = WebcHttpProxyIp[0];
		ip_addr[1] = WebcHttpProxyIp[1];
		ip_addr[2] = WebcHttpProxyIp[2];
		ip_addr[3] = WebcHttpProxyIp[3];
		port = WebcHttpProxyPort;
	}
#endif // WEBC_SUPPORT_HTTP_PROXY
	http_log_message("HTTP_IO_CONNECT Called\n")
	if (HTTP_IO_CONNECT(ctx->stream, ip_addr, port) < 0)
	{
		HTTP_IO_CLOSE(ctx->stream);
		http_log_message("HTTP_IO_CONNECT Fail\n")
		return (-1);
	}
	http_log_message("HTTP_IO_CONNECT Succes\n")
	tc_movebytes(ctx->ip_addr, ip_addr, 4);
	ctx->port = port;

	ctx->in_buffer = (WEBC_UINT8 *) WEBC_MALLOC_VERBOSE(WEBC_CFG_HTTP_BUFFER_SIZE,"HttpInbuffer");
	ctx->in_buffer_size = WEBC_CFG_HTTP_BUFFER_SIZE;
	ctx->in_buffer_begin_ptr = 0;
	ctx->in_buffer_end_ptr = 0;

	if (!ctx->in_buffer)
	{
		ctx->in_buffer_size = 0;
		HTTP_IO_CLOSE(ctx->stream);
		return (-1);
	}
	http_log_message("http_connect returning success\n");
	return (0);
}
extern int StreamToSocket(void *pStream);
extern int RtipCheckIfLoopBack(int sd);
extern int http_server_process_one(void);
extern int disable_server_processing;
extern int RtipCheckIfLoopBackIpAddr(unsigned char *pIpaddr);
static int _http_get(struct httpc_ctx *ctx, char *pFile, char *pHost, WEBC_UINT16 port, RTP_TIMESTAMP* if_modified_since, char *referer);
#if(WEBC_CALL_SERVER_ON_LOOPBACK)
extern int processing_http_get;
#endif
int http_get(struct httpc_ctx *ctx, char *pFile, char *pHost, WEBC_UINT16 port, RTP_TIMESTAMP* if_modified_since, char *referer)
{
int r;
unsigned char ipaddr[8];
    webc_gethostipaddr((WEBC_PFBYTE)ipaddr, pHost);
#if(WEBC_CALL_SERVER_ON_LOOPBACK)
    disable_server_processing=RtipCheckIfLoopBackIpAddr((unsigned char *)ipaddr);
#endif
    r=_http_get(ctx, pFile, pHost,  port, if_modified_since, referer);
#if(WEBC_CALL_SERVER_ON_LOOPBACK)
    disable_server_processing=0;
#endif
    return r;
}

static int _http_get(struct httpc_ctx *ctx, char *pFile, char *pHost, WEBC_UINT16 port, RTP_TIMESTAMP* if_modified_since, char *referer)
{
long lReqSize;
char *pRequest;
WEBC_UINT8 ipaddr[4];
#if (HTTP_REPORT_STATUS)
 char status_string[256];
#endif
	http_log_message("http_get called\n");

	if (!ctx)
	{
		return (-1);
	}

	if (ctx->host)
	{
		webc_free_string_copy_8(ctx->host, __FILE__, __LINE__);
		ctx->host = 0;
	}

	if (ctx->fileName)
	{
		webc_free_string_copy_8(ctx->fileName, __FILE__, __LINE__);
		ctx->fileName = 0;
	}

	ctx->host = webc_malloc_string_copy_8(pHost, __FILE__, __LINE__);
  	ctx->fileName = webc_malloc_string_copy_8(pFile, __FILE__, __LINE__);
	ctx->bytes_this_file = 0;

#if (HTTPC_SUPPORT_KEEPALIVE)
	ctx->flags |= HTTPC_FLAG_USE_KEEPALIVE;
#endif // HTTPC_SUPPORT_KEEPALIVE

#if (WEBC_SUPPORT_HTTP_CACHE)
	if (giHttpCacheInitialized)
	{
		if (if_modified_since)
		{
			ctx->p_user_date = ctx->user_date;
			*ctx->p_user_date = *if_modified_since;
		}
		else
		{
			ctx->p_user_date = 0;
		}

		ctx->flags &= ~(HTTPC_FLAG_FILE_IN_CACHE|HTTPC_FLAG_USE_CACHED_FILE|HTTPC_FLAG_CACHE_INCOMING);
		http_get_cache_key(ctx->cname, pFile, pHost, port);
		if (wcache_open(&gHttpCache, ctx->cname, &ctx->cfile, WEBC_CO_READ) >= 0)
		{
			RTP_TIMESTAMP current_time[1];
			RTP_TIMESTAMP expires;

			ctx->mime_data_type = (WebcFileContentType) wcache_get_data_type(&ctx->cfile);
			ctx->mime_charset = (enum WebcCharset) wcache_get_charset(&ctx->cfile);
			ctx->flags |= HTTPC_FLAG_FILE_IN_CACHE;
			wcache_get_date(&ctx->cfile, &ctx->cache_date);

			rtp_date_get_timestamp(current_time);
			wcache_get_expires(&ctx->cfile, &expires);

			if ((rtp_date_compare_time(current_time, &expires) < 0) &&
				(!if_modified_since || (rtp_date_compare_time(&ctx->cache_date, ctx->p_user_date) >= 0)))
			{
				// if cached file is still valid, then use it
				ctx->mime_size = wcache_get_size(&ctx->cfile);
				ctx->flags |= (HTTPC_FLAG_HAS_MIME_SIZE|HTTPC_FLAG_USE_CACHED_FILE);
				return (0);
			}

			// if our cached file is newer than the modifed date, then only get from server
			//  if the file has been modified since the cache date
			if (!if_modified_since || (rtp_date_compare_time(&ctx->cache_date, ctx->p_user_date) >= 0))
			{
				if_modified_since = &ctx->cache_date;
			}
		}
	}
#endif // WEBC_SUPPORT_HTTP_CACHE


#if WEBC_SUPPORT_HTTP_PROXY
	if (WebcHttpProxyEnable)
	{
		// convert host ip/port address to use the proxy's
		ipaddr[0] = WebcHttpProxyIp[0];
		ipaddr[1] = WebcHttpProxyIp[1];
		ipaddr[2] = WebcHttpProxyIp[2];
		ipaddr[3] = WebcHttpProxyIp[3];
		port = WebcHttpProxyPort;
	}
	else
#endif // WEBC_SUPPORT_HTTP_PROXY
	{
#if (HTTP_REPORT_STATUS)
		rtp_sprintf(status_string,"GET Looking up host name '%.200s'...",pHost);
		http_report_status(ctx,status_string);
#endif

		if (webc_gethostipaddr((WEBC_PFBYTE)ipaddr, pHost) < 0)
		{
			if (ctx->host)
			{
				webc_free_string_copy_8(ctx->host, __FILE__, __LINE__);
				ctx->host = 0;
			}

			if (ctx->fileName)
			{
				webc_free_string_copy_8(ctx->fileName, __FILE__, __LINE__);
				ctx->fileName = 0;
			}

			return (-1);
		}
	}

#if (HTTP_REPORT_STATUS)
	rtp_sprintf(status_string,"GET Connecting to host '%u.%u.%u.%u'...",
		ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
	http_report_status(ctx,status_string);
#endif


	if (http_connect(ctx, ipaddr, port) < 0)
	{
		if (ctx->host)
		{
			webc_free_string_copy_8(ctx->host, __FILE__, __LINE__);
			ctx->host = 0;
		}

		if (ctx->fileName)
		{
			webc_free_string_copy_8(ctx->fileName, __FILE__, __LINE__);
			ctx->fileName = 0;
		}
		return (-1);
	}

	lReqSize = http_format_get_header(ctx, 0, pFile, pHost, port, if_modified_since ,referer);
	pRequest = (char *) WEBC_MALLOC(lReqSize);
	if (!pRequest)
	{
		webc_http_close(ctx);
		return (-1);
	}
	http_format_get_header(ctx, pRequest, pFile, pHost, port, if_modified_since, referer);

#if (HTTP_REPORT_STATUS)
	rtp_sprintf(status_string,"GET Sending HTTP request for '%.200s'...",pFile);
	http_report_status(ctx, status_string);
#endif
	http_log_message("http_get call HTTP_IO_SEND\n");
	if (HTTP_IO_SEND(ctx->stream, (const unsigned char*) pRequest, lReqSize) < lReqSize)
	{
		webc_http_close(ctx);
		WEBC_FREE(pRequest);
		http_log_message("http_get HTTP_IO_SEND failed\n");
		return (-1);
	}

#if (HTTP_REPORT_STATUS)
	rtp_sprintf(status_string,"Loading %.200s...",pFile);
	http_report_status(ctx, status_string);
#endif

	WEBC_FREE(pRequest);
	http_log_message("http_get returning success\n");

#if(WEBC_CALL_SERVER_ON_LOOPBACK)
/*
   If we're fetching web pages using the loopback interface in polled mode call the server and instruct it to retrieve the
   http get command we just passed through RtipToWebC() visa vis tc_tcp_write.

   It will then dump all of the requested content via a chain the ultimately calls RtipToWebC().

   We will retrieve the content by calling WebCReadData() instead of the network stack inside webc_recv_at_least()
 */
    {
        if (RtipCheckIfLoopBack(StreamToSocket((void *)ctx->stream))==1)
        {
	        http_server_process_one();
        }
    }
#endif
	return (0);
}

int http_post(struct httpc_ctx *ctx, char *action, char *host, WEBC_UINT16 port, char *content_type, long length)
{
long lReqSize;
char *pRequest;
WEBC_UINT8 ipaddr[4];
#if (HTTP_REPORT_STATUS)
char status_string[256];
#endif

	if (!ctx)
	{
		return (-1);
	}

	if (ctx->host)
	{
		webc_free_string_copy_8(ctx->host, __FILE__, __LINE__);
		ctx->host = 0;
	}

	if (ctx->fileName)
	{
		webc_free_string_copy_8(ctx->fileName, __FILE__, __LINE__);
		ctx->fileName = 0;
	}

	ctx->host = webc_malloc_string_copy_8(host, __FILE__, __LINE__);
  	ctx->fileName = webc_malloc_string_copy_8(action, __FILE__, __LINE__);
	ctx->bytes_this_file = 0;

#if (HTTPC_SUPPORT_KEEPALIVE)
	ctx->flags |= HTTPC_FLAG_USE_KEEPALIVE;
#endif // HTTPC_SUPPORT_KEEPALIVE

#if (WEBC_SUPPORT_HTTP_CACHE)
	ctx->flags &= ~(HTTPC_FLAG_FILE_IN_CACHE|HTTPC_FLAG_USE_CACHED_FILE|HTTPC_FLAG_CACHE_INCOMING);
#endif // WEBC_SUPPORT_HTTP_CACHE

#if WEBC_SUPPORT_HTTP_PROXY
    if (WebcHttpProxyEnable)
    {
        // convert host ip/port address to use the proxy's
        ipaddr[0] = WebcHttpProxyIp[0];
        ipaddr[1] = WebcHttpProxyIp[1];
        ipaddr[2] = WebcHttpProxyIp[2];
        ipaddr[3] = WebcHttpProxyIp[3];
        port = WebcHttpProxyPort;
    }
    else
#endif // WEBC_SUPPORT_HTTP_PROXY
    {
#if (HTTP_REPORT_STATUS)
        rtp_sprintf(status_string,"POST Looking up host name '%.200s'...",host);
        http_report_status(ctx,status_string);
#endif

        if (webc_gethostipaddr((WEBC_PFBYTE)ipaddr, host) < 0)
        {
			if (ctx->host)
			{
				webc_free_string_copy_8(ctx->host, __FILE__, __LINE__);
				ctx->host = 0;
			}

			if (ctx->fileName)
			{
				webc_free_string_copy_8(ctx->fileName, __FILE__, __LINE__);
				ctx->fileName = 0;
			}

            return (-1);
        }
    }

#if (HTTP_REPORT_STATUS)
    rtp_sprintf(status_string,"POST Connecting to host '%u.%u.%u.%u'...",
        ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
    http_report_status(ctx,status_string);
#endif

	if (http_connect(ctx, ipaddr, port) < 0)
	{
		if (ctx->host)
		{
			webc_free_string_copy_8(ctx->host, __FILE__, __LINE__);
			ctx->host = 0;
		}

		if (ctx->fileName)
		{
			webc_free_string_copy_8(ctx->fileName, __FILE__, __LINE__);
			ctx->fileName = 0;
		}

		return (-1);
	}

	lReqSize = http_format_post_header(ctx, 0, host, port, action, content_type, length);
	pRequest = (char *) WEBC_MALLOC(lReqSize);
	if (!pRequest)
	{
		webc_http_close(ctx);
		return (-1);
	}
	http_format_post_header(ctx, pRequest, host, port, action, content_type, length);

#if (HTTP_REPORT_STATUS)
    rtp_sprintf(status_string,"POST Sending HTTP request for '%.150s'...",action);
    http_report_status(ctx,status_string);
#endif

	if (HTTP_IO_SEND(ctx->stream, (const unsigned char*) pRequest, lReqSize) < lReqSize)
	{
		webc_http_close(ctx);
		WEBC_FREE(pRequest);
		return (-1);
	}

#if (HTTP_REPORT_STATUS)
    rtp_sprintf(status_string,"POST Finished");
    http_report_status(ctx,status_string);
#endif

	WEBC_FREE(pRequest);
	return (0);
}


long http_recv(struct httpc_ctx *ctx, char *buffer, long size)
{
long bytes_left = size, bytes_read = 0, len, result;
	http_log_message("http_recv called\n");

#if (WEBC_SUPPORT_HTTP_CACHE)
	if (giHttpCacheInitialized)
	{
		// If we are reading from the cache, do so
		if (ctx->flags & HTTPC_FLAG_USE_CACHED_FILE)
		{
			bytes_read = wcache_read(&ctx->cfile, (unsigned char *) buffer, size);

			rtp_printf("ZZZ cache read 1 == %d\n",  bytes_read);
			if (bytes_read > 0)
			{
				ctx->bytes_this_file += bytes_read;
			rtp_printf("ZZZ totsl read 1 == %d\n",  ctx->bytes_this_file);
			}

			return (bytes_read);
		}
	}
#endif // WEBC_SUPPORT_HTTP_CACHE

	// First make sure we've read the mime header
	if (!(ctx->flags & HTTPC_FLAG_MIME_HDR_READ))
	{
		result = http_process_mime_header(ctx, buffer, size);
		if (result < 0)
		{
		  #if (HTTPC_SUPPORT_AUTHENTICATE)
			if (result == HTTPC_AUTHORIZATION_REQUIRED)
			{
				if (!(ctx->flags & HTTPC_FLAG_AUTHORIZATION_SENT) && ctx->realm)
				{
					HTTPAuthenticationRealm *realm = _http_get_authentication_realm(ctx->host, ctx->port, ctx->realm);
					if (realm)
					{
						char *host = ctx->host;
						char *file = ctx->fileName;
						unsigned short port = ctx->port;

						// HTTP_API_EXIT - tbd
						HTTPStreamFactoryClass** factory = ctx->factory;
						webc_http_close(ctx);
						webc_http_init(ctx, factory);
						http_get(ctx, file, host, port, 0, 0);
						ctx->flags |= HTTPC_FLAG_AUTHORIZATION_SENT;
						return (http_recv(ctx, buffer, size));
					}
				}
			}
		  #endif // HTTPC_SUPPORT_AUTHENTICATE
			return (result);
		}

	  #if (WEBC_SUPPORT_HTTP_CACHE)
		if (giHttpCacheInitialized)
		{
			// If we are reading from the cache, do so
			if (ctx->flags & HTTPC_FLAG_USE_CACHED_FILE)
			{
				long rval = wcache_read(&ctx->cfile, (unsigned char *) buffer, size);
				rtp_printf("ZZZ read 2 == %d\n",  rval);
				return (rval);
			}
		}
	  #endif // WEBC_SUPPORT_HTTP_CACHE
	}

	if (ctx->flags & HTTPC_FLAG_HAS_MIME_SIZE)
	{
		bytes_left = EBSMIN(ctx->mime_size - ctx->bytes_this_file, (unsigned long) bytes_left);
	}

	while (bytes_left > 0)
	{
#if (HTTPC_SUPPORT_KEEPALIVE)
		if (ctx->flags & HTTPC_FLAG_CHUNKED)
		{
			// if we're at the end of the current chunk, read the next chunk header
			if ((ctx->bytes_this_chunk >= ctx->chunk_size) && !(ctx->flags & HTTPC_FLAG_LAST_CHUNK_READ))
			{
				http_read_chunk_header(ctx);
				if (ctx->chunk_size == 0)
				{
					char buf[64];

					ctx->flags |= HTTPC_FLAG_LAST_CHUNK_READ;

					// last chunk; that's all folks...
					do
					{
						len = _httpc_sock_readline(ctx, buf, 64);
						if ((len > 0) && buf[len-1] == '\n')
						{
							len--;
							if ((len > 0) && buf[len-1] == '\r')
							{
								len--;
							}
						}
					} while (len > 0);
					break;
				}
			}

			if (ctx->flags & HTTPC_FLAG_LAST_CHUNK_READ)
			{
				break;
			}

			to_read = EBSMIN((unsigned long) bytes_left, ctx->chunk_size - ctx->bytes_this_chunk);
			len = _httpc_sock_read(ctx, &buffer[bytes_read], to_read);
			if (len == 0)
			{
				break;
			}
			if (len < 0)
			{
				return (HTTPC_FATAL_ERROR);
			}
			bytes_left -= len;
			bytes_read += len;
			ctx->bytes_this_chunk += len;
			ctx->bytes_this_file += len;

			// if we've reached the end of the chunk, then read the CR LF at the end
			if ((ctx->bytes_this_chunk == ctx->chunk_size) && !(ctx->flags & HTTPC_FLAG_LAST_CHUNK_READ))
			{
				char buf[10];
				_httpc_sock_readline(ctx, buf, 10);
			}
		}
		else
		{
#endif // HTTPC_SUPPORT_KEEPALIVE

			len = _httpc_sock_read(ctx, &buffer[bytes_read], bytes_left);

			if (len < 0)
			{
				return (HTTPC_FATAL_ERROR);
			}

			if (len == 0)
			{
				break;
			}

			bytes_left -= len;
			bytes_read += len;
			ctx->bytes_this_file += len;

#if (HTTPC_SUPPORT_KEEPALIVE)
		}
#endif // HTTPC_SUPPORT_KEEPALIVE
	}

#if (WEBC_SUPPORT_HTTP_CACHE)
	if (giHttpCacheInitialized)
	{
		if ((bytes_read > 0) && (ctx->flags & HTTPC_FLAG_CACHE_INCOMING))
		{
			wcache_write(&ctx->cfile, (unsigned char *) buffer, bytes_read);
		}
	}
#endif // WEBC_SUPPORT_HTTP_CACHE

	http_log_message("http_recv done\n");

	return (bytes_read);
}


long  http_send(struct httpc_ctx *ctx, char *buffer, long size)
{
	return (HTTP_IO_SEND(ctx->stream, (const unsigned char*) buffer, size));
}


/*************************************************************************
 http_close() - Closes TCP connection to HTTP server.

 ctx - httpc_ctx initialized via http_init().

 This routine closes the TCP connection opened via http_connect_init(),
 http_connect_cycle().

 Returns: Return code of closesocket(), i.e. 0 on success, < 0 otherwise.
 Upon error, use GETLASTERROR() to retrieve error code.
*************************************************************************/

int webc_http_close(struct httpc_ctx *ctx)
{
HTTPStreamClass** s = ctx->stream;
#if (HTTPC_SUPPORT_KEEPALIVE)
int n;
#endif // HTTPC_SUPPORT_KEEPALIVE

	if (ctx->in_buffer)
	{
		WEBC_FREE(ctx->in_buffer);
	}
	ctx->in_buffer_size = 0;
	ctx->in_buffer_begin_ptr = 0;
	ctx->in_buffer_end_ptr = 0;
	if (ctx->redirected_page)
	{
		webc_free_string_copy_8(ctx->redirected_page, __FILE__, __LINE__);
		ctx->redirected_page = 0;
	}

	if (ctx->realm)
	{
		webc_free_string_copy_8(ctx->realm, __FILE__, __LINE__);
		ctx->realm = 0;
	}

	if (ctx->host)
	{
		webc_free_string_copy_8(ctx->host, __FILE__, __LINE__);
		ctx->host = 0;
	}

	if (ctx->fileName)
	{
		webc_free_string_copy_8(ctx->fileName, __FILE__, __LINE__);
		ctx->fileName = 0;
	}


	// Fix June 2010. Close the socket if the get failed.
	// Do not close if the transfer failed because it is a redirect.
//	if ((!httpc_done(ctx)) && ( (ctx->http_code != 301) && (ctx->http_code != 302) && (ctx->http_code != 303)))
	if (!httpc_done(ctx))
	{
		// if the file download did not complete, then don't save the socket for reuse;
		//  close it now
#if (WEBC_SUPPORT_HTTP_CACHE)
		if (giHttpCacheInitialized)
		{
			// also delete the cache entry so we won't try to load an incomplete file from
			//  the cache.
			if (ctx->flags & HTTPC_FLAG_CACHE_INCOMING)
			{
				wcache_close(&ctx->cfile);
				wcache_delete_entry(&gHttpCache, ctx->cname);
				wcache_flush(&gHttpCache);
			}
			else if (ctx->flags & HTTPC_FLAG_USE_CACHED_FILE)
			{
				wcache_close(&ctx->cfile);
				wcache_flush(&gHttpCache);
			}
		}
#endif // WEBC_SUPPORT_HTTP_CACHE

		tc_memset(ctx, 0, sizeof(struct httpc_ctx));
		ctx->mime_data_type = HTML_MIME_TYPE_UNKNOWN;

		if (s)
		{
			HTTP_IO_CLOSE(s);
		}
		return 0;
	}

#if (WEBC_SUPPORT_HTTP_CACHE)
	if (giHttpCacheInitialized)
	{
		if (ctx->flags & (HTTPC_FLAG_USE_CACHED_FILE | HTTPC_FLAG_CACHE_INCOMING))
		{
			wcache_close(&ctx->cfile);
			wcache_flush(&gHttpCache);
		}
	}
#endif // WEBC_SUPPORT_HTTP_CACHE

#if (HTTPC_SUPPORT_KEEPALIVE)

	// if the connection is still open, save this socket for future use
	if ((ctx->flags & HTTPC_FLAG_USE_KEEPALIVE) &&
	    ctx->stream &&
	    HTTP_IO_ISCONNECTED(ctx->stream))
	{
		for (n=0; n<HTTPC_CFG_MAX_CONNECTIONS; n++)
		{
			if (!gHttpSocketPool[n].valid)
			{
				// we found an open slot; save the socket
				if (!HTTP_IO_CACHE(ctx->stream))
				{
					break;
				}

				tc_movebytes(gHttpSocketPool[n].ip_addr, ctx->ip_addr, 4);

				gHttpSocketPool[n].port      = ctx->port;
				gHttpSocketPool[n].stream    = ctx->stream;
				gHttpSocketPool[n].valid     = WEBC_TRUE;
				gHttpSocketPool[n].closeTime = rtp_get_system_msec() + WEBC_HTTP_KEEPALIVE_TIMEOUT;

				tc_memset(ctx, 0, sizeof(struct httpc_ctx));
				ctx->mime_data_type = HTML_MIME_TYPE_UNKNOWN;

				return (0);
			}
		}
	}

#endif // HTTPC_SUPPORT_KEEPALIVE

	tc_memset(ctx, 0, sizeof(struct httpc_ctx));
	ctx->mime_data_type = HTML_MIME_TYPE_UNKNOWN;

	if (s)
	{
		HTTP_IO_CLOSE(s);
	}

	return 0;
}

void  http_timeout_connections (void)
{
  #if (HTTPC_SUPPORT_KEEPALIVE)
	int n;
	unsigned long now = rtp_get_system_msec();

	// first check our socket pool to see if someone left a connection open
	for (n=0; n<HTTPC_CFG_MAX_CONNECTIONS; n++)
	{
		if (gHttpSocketPool[n].valid)
		{
			if (!HTTP_IO_ISCONNECTED(gHttpSocketPool[n].stream) ||
			    (long)(gHttpSocketPool[n].closeTime - now) <= 0)
			{
				HTTP_IO_CLOSE(gHttpSocketPool[n].stream);
				gHttpSocketPool[n].valid = WEBC_FALSE;
			}
		}
	}
  #endif // HTTPC_SUPPORT_KEEPALIVE
}

long  http_msecs_to_next_timeout (void)
{
	long msecTime = WEBC_TIMEOUT_INFINITE;

  #if (HTTPC_SUPPORT_KEEPALIVE)
	int n;
	unsigned long now = rtp_get_system_msec();
	long timeLeft;

	// first check our socket pool to see if someone left a connection open
	for (n=0; n<HTTPC_CFG_MAX_CONNECTIONS; n++)
	{
		if (gHttpSocketPool[n].valid)
		{
			timeLeft = (long)(gHttpSocketPool[n].closeTime - now);
			if (timeLeft < msecTime)
			{
				msecTime = timeLeft;
			}
		}
	}
  #endif // HTTPC_SUPPORT_KEEPALIVE

	return (msecTime);
}

int httpc_done(struct httpc_ctx *ctx)
{
	return ( ((ctx->flags & HTTPC_FLAG_CHUNKED) &&
	          (ctx->flags & HTTPC_FLAG_LAST_CHUNK_READ) &&
	          (ctx->bytes_this_chunk == ctx->chunk_size)) ||
	         ((ctx->flags & HTTPC_FLAG_HAS_MIME_SIZE) &&
	          (ctx->bytes_this_file == ctx->mime_size)) ||
	         ((ctx->http_code == 304 /* Not Modified */) &&
	          (!(ctx->flags & HTTPC_FLAG_HAS_MIME_SIZE) ||
	            (ctx->mime_size == 0))) );
}


#if (WEBC_SUPPORT_HTTP_CACHE)
void http_get_cache_key(char *pKey, const char *pFile, const char *pHost, WEBC_UINT16 port)
{
long bytes_left = WEBC_CACHE_MAX_KEY_LEN;
char portstr[8];

	if (!pKey)
		return;

	pKey[bytes_left-1] = 0;
	bytes_left--;

	tc_strncpy(pKey, pHost, bytes_left);
	bytes_left -= tc_strlen(pHost);
	if (bytes_left <= 0)
	{
		return;
	}

	tc_strncat(pKey, ":", bytes_left);
	bytes_left -= tc_strlen(":");
	if (bytes_left <= 0)
	{
		return;
	}

	tc_itoa(port, portstr, 10);
	tc_strncat(pKey, portstr, bytes_left);
	bytes_left -= tc_strlen(portstr);
	if (bytes_left <= 0)
	{
		return;
	}

	//Krishna: Commenting the below line for avoiding /// while getting the key
	//tc_strncat(pKey, "//", bytes_left);
	bytes_left -= tc_strlen("//");
	if (bytes_left <= 0)
	{
		return;
	}

	tc_strncat(pKey, pFile, bytes_left);
}
#endif // WEBC_SUPPORT_HTTP_CACHE


#if (WEBC_SUPPORT_HTTP || WEBC_SUPPORT_HTTPS)

// New helper functions

static const char httpget_prefix[] =
		"GET ";

static const char httppost_prefix[] =
		"POST ";

#if (HTTPC_SUPPORT_KEEPALIVE)
static const char http_1_1_postfix[] =
		" HTTP/1.1";

static const char keepalive_string[] =
		"\r\nConnection: Keep-Alive";
#endif // HTTPC_SUPPORT_KEEPALIVE

static const char http_1_0_postfix[]=
		" HTTP/1.0";
#undef WEBC_USER_AGENT_ID

#undef WEBC_USER_AGENT_ID 
#define WEBC_USER_AGENT_ID                  "Mozilla/4.0 (compatible; MSIE 5.0; Windows NT 5.1)  Mobile" 

static const char agent_string[] =
		"\r\nUser-Agent: "WEBC_USER_AGENT_ID;

static const char accept_string[] =
#if (WEBC_SUPPORT_JSCRIPT)
		"\r\nAccept: text/*,image/gif,image/jpeg,image/png,application/x-javascript";
#else
		"\r\nAccept: text/*,image/gif,image/jpeg,image/png";
#endif

static const char host_prefix[] =
		"\r\nHost: ";

static const char date_prefix[] =
		"\r\nIf-Modified-Since: ";

static const char cookie_prefix[] =
		"\r\nCookie: ";

static const char content_len_prefix[] =
		"\r\nContent-Length: ";

static const char content_type_prefix[] =
		"\r\nContent-Type: ";

static const char mime_postfix[] =
		"\r\n\r\n";

#define WEBC_SUPPORT_HTTP_MACADDR       0
#if (WEBC_SUPPORT_HTTP_MACADDR)
// static const char macaddr_prefix[]="\r\nMACAddress: ";
static const char macaddr_prefix[]="\r\nMAC: ";
#endif // (defined(WEBC_SUPPORT_HTTP_MACADDR) && WEBC_SUPPORT_HTTP_MACADDR)

#undef WEBC_SUPPORT_DEVICE_SERIAL_HEADER
#define WEBC_SUPPORT_DEVICE_SERIAL_HEADER 0

#if (WEBC_SUPPORT_REFERER_HEADER)
static const char referer_header_entry[]="\r\nReferer: ";
#endif  // #if (WEBC_SUPPORT_REFERER_HEADER)
long http_format_get_header (
		struct httpc_ctx* ctx,
		char* buffer,
		const char* file,
		const char* host,
		unsigned short port,
		RTP_TIMESTAMP* modified_date
		,const char* referer
	)
{
char datestr[EBS_MAX_DATE_STR_LEN];
long request_len = 0;
#if (WEBC_SUPPORT_COOKIES)
long cookie_len;
#endif

	request_len  = tc_memcat(buffer, request_len, httpget_prefix, sizeof(httpget_prefix) - 1);

#if WEBC_SUPPORT_HTTP_PROXY
	if (WebcHttpProxyEnable)
	{
    	request_len += http_escape_path_string(
						buffer ? (buffer + request_len) : 0, "http://");
    	request_len += http_escape_path_string(
						buffer ? (buffer + request_len) : 0, host);
	}
#endif // WEBC_SUPPORT_HTTP_PROXY
	request_len += http_escape_path_string(buffer ? (buffer + request_len) : 0, file);

#if (HTTPC_SUPPORT_KEEPALIVE)
	if (ctx->flags & HTTPC_FLAG_USE_KEEPALIVE)
	{
		request_len  = tc_memcat(buffer, request_len, http_1_1_postfix, sizeof(http_1_1_postfix) - 1);
		request_len  = tc_memcat(buffer, request_len, keepalive_string, sizeof(keepalive_string) - 1);
	}
	else
	{
#endif // HTTPC_SUPPORT_KEEPALIVE

		request_len  = tc_memcat(buffer, request_len, http_1_0_postfix, sizeof(http_1_0_postfix) - 1);

#if (HTTPC_SUPPORT_KEEPALIVE)
	}
#endif // HTTPC_SUPPORT_KEEPALIVE

	request_len  = tc_memcat(buffer, request_len, agent_string, sizeof(agent_string) - 1);
	request_len  = tc_memcat(buffer, request_len, accept_string, sizeof(accept_string) - 1);

/*	if (WEBC_CFG_DEFAULT_LANGUAGE) */
	{
		static char accept_lang_header[] = "\r\nAccept-Language: ";
		request_len = tc_memcat(buffer, request_len, accept_lang_header, sizeof(accept_lang_header) - 1);
		request_len = tc_memcat(buffer, request_len, WEBC_CFG_DEFAULT_LANGUAGE, sizeof(WEBC_CFG_DEFAULT_LANGUAGE) - 1);
	}

	if (host)
	{
		request_len = tc_memcat(buffer, request_len, host_prefix, sizeof(host_prefix) - 1);
		request_len = tc_memcat(buffer, request_len, host, tc_strlen(host));

		if (port != HTTPC_DEFAULT_PORT)
		{
			char portString[8];
			portString[0] = ':';
			tc_itoa(port, portString + 1, 10);
			portString[7] = 0;

			request_len = tc_memcat(buffer, request_len, portString, tc_strlen(portString));
		}
	}


	if (modified_date)
	{
		rtp_date_print_time(datestr,modified_date,0);
		request_len = tc_memcat(buffer, request_len,
								date_prefix, sizeof(date_prefix) - 1);
		request_len = tc_memcat(buffer, request_len,
								datestr, tc_strlen(datestr));
	}

#if (WEBC_SUPPORT_COOKIES)
	cookie_len = webc_print_matching_cookies(0, host, file, (ctx->flags & HTTPC_FLAG_SECURE)? 1 : 0);

	if (cookie_len > 0)
	{
		request_len =  tc_memcat(buffer, request_len,
								cookie_prefix, sizeof(cookie_prefix) - 1);
		request_len += webc_print_matching_cookies(
						buffer ? (buffer + request_len) : 0, host, file,
						(ctx->flags & HTTPC_FLAG_SECURE)? 1 : 0);
	}
#endif

#if (HTTPC_SUPPORT_AUTHENTICATE)
	request_len += _http_print_authorization(ctx, buffer? (buffer + request_len) : 0);
#endif // HTTPC_SUPPORT_AUTHENTICATE

	//request_len = tc_memcat(buffer, request_len,mime_postfix, sizeof(mime_postfix) - 1);
#if	(WEBC_SUPPORT_REFERER_HEADER)
	if(referer && tc_strlen(referer) != 0)
	{
		request_len = tc_memcat(buffer, request_len, referer_header_entry, sizeof(referer_header_entry) - 1);
		request_len = tc_memcat(buffer, request_len, referer, tc_strlen(referer));
	}
#endif //#if	(WEBC_SUPPORT_REFERER_HEADER)

#if (defined(WEBC_SUPPORT_HTTP_MACADDR) && WEBC_SUPPORT_HTTP_MACADDR)
        {
            extern char* get_mac(void);
        // There was room for the serial number, so the MAC should fit
            char* mac_string = get_mac();
            if (*mac_string != '\0')
            {
                request_len = tc_memcat(buffer, request_len,macaddr_prefix, sizeof(macaddr_prefix) - 1);
                request_len = tc_memcat(buffer, request_len,mac_string, strlen(mac_string));
            }
        }
#endif // (defined(WEBC_SUPPORT_HTTP_MACADDR) && WEBC_SUPPORT_HTTP_MACADDR)


#if	(WEBC_SUPPORT_DEVICE_SERIAL_HEADER)
		request_len = tc_memcat(buffer, request_len,DeviceSerialNo_Prefix, sizeof(DeviceSerialNo_Prefix) - 1);
		request_len = tc_memcat(buffer, request_len,DeviceSerialNo, sizeof(DeviceSerialNo) - 1);
#endif

request_len = tc_memcat(buffer, request_len,mime_postfix, sizeof(mime_postfix) - 1);


	return (request_len);
}

int http_escape_path_string(char *dest, const char *src)
{
int dest_length = 0;

	for (; *src; src++)
	{
		if (IS_WHITESPACE(*src))
		{
			dest_length += 3;
			if (dest)
			{
				dest[0] = '%';
				if ((unsigned)(*src) > 15)
				{
					tc_itoa(*src,dest+1,16);
				}
				else
				{
					dest[1] = '0';
					tc_itoa(*src,dest+2,16);
				}
				dest += 3;
			}
		}
		else
		{
			dest_length++;
			if (dest)
			{
				*(dest++) = *src;
			}
		}
	}

	return (dest_length);
}

long http_format_post_header (
		struct httpc_ctx* ctx,
		char* dest,
		const char* host,
		unsigned short port,
		const char* action,
		const char* content_type,
		long length
	)
{
int sl=0;
char lenstr[20];
#if (WEBC_SUPPORT_COOKIES)
long cookie_len;
#endif

	sl = tc_memcat(dest, sl, httppost_prefix, sizeof(httppost_prefix)-1);

#if WEBC_SUPPORT_HTTP_PROXY
    if (WebcHttpProxyEnable)
    {
		sl += http_escape_path_string(dest? (dest + sl) : 0, "http://");
		sl += http_escape_path_string(dest? (dest + sl) : 0, host);
	}
#endif // WEBC_SUPPORT_HTTP_PROXY

	sl += http_escape_path_string(dest? (dest + sl) : 0, action);
	sl = tc_memcat(dest, sl, http_1_0_postfix, sizeof(http_1_0_postfix)-1);

	sl = tc_memcat(dest, sl, accept_string, sizeof(accept_string)-1);

/*	if (WEBC_CFG_DEFAULT_LANGUAGE) */
	{
		static char accept_lang_header[] = "\r\nAccept-Language: ";
		sl = tc_memcat(dest, sl, accept_lang_header, sizeof(accept_lang_header) - 1);
		sl = tc_memcat(dest, sl, WEBC_CFG_DEFAULT_LANGUAGE, sizeof(WEBC_CFG_DEFAULT_LANGUAGE) - 1);
	}

	sl = tc_memcat(dest, sl, content_len_prefix, sizeof(content_len_prefix)-1);
	tc_itoa(length,lenstr,10);
	sl = tc_memcat(dest, sl, lenstr, tc_strlen(lenstr));

	sl = tc_memcat(dest, sl, content_type_prefix, sizeof(content_type_prefix)-1);
	sl = tc_memcat(dest, sl, content_type, tc_strlen(content_type));

	sl = tc_memcat(dest, sl, agent_string, sizeof(agent_string) - 1);

	if (host)
	{
		sl = tc_memcat(dest, sl, host_prefix, sizeof(host_prefix) - 1);
		sl = tc_memcat(dest, sl, host, tc_strlen(host));

		if (port != HTTPC_DEFAULT_PORT)
		{
			char portString[8];
			portString[0] = ':';
			tc_itoa(port, portString + 1, 10);
			portString[7] = 0;

			sl = tc_memcat(dest, sl, portString, tc_strlen(portString));
		}
	}

#if (WEBC_SUPPORT_COOKIES)
	cookie_len = webc_print_matching_cookies(0, host, action, (ctx->flags & HTTPC_FLAG_SECURE)? 1 : 0);

	if (cookie_len > 0)
	{
		sl =  tc_memcat(dest, sl, cookie_prefix, sizeof(cookie_prefix) - 1);
		sl += webc_print_matching_cookies(dest? (dest + sl) : 0, host, action, (ctx->flags & HTTPC_FLAG_SECURE)? 1 : 0);
	}
#endif

#if (HTTPC_SUPPORT_AUTHENTICATE)
	sl += _http_print_authorization(ctx, dest? (dest + sl) : 0);
#endif // HTTPC_SUPPORT_AUTHENTICATE


#if (defined(WEBC_SUPPORT_HTTP_MACADDR) && WEBC_SUPPORT_HTTP_MACADDR)

        {
            extern char* get_mac(void);
            char* mac_string = get_mac();

            if (*mac_string != '\0')
            {
                sl= tc_memcat(dest, sl, macaddr_prefix, sizeof(macaddr_prefix) - 1);
                sl= tc_memcat(dest, sl, mac_string, strlen(mac_string));
            }
        }

#endif // (defined(WEBC_SUPPORT_HTTP_MACADDR) && WEBC_SUPPORT_HTTP_MACADDR)


#if	(WEBC_SUPPORT_DEVICE_SERIAL_HEADER)
	sl= tc_memcat(dest, sl, DeviceSerialNo_Prefix, sizeof(DeviceSerialNo_Prefix) - 1);
	sl= tc_memcat(dest, sl, DeviceSerialNo, sizeof(DeviceSerialNo) - 1);
#endif


	sl = tc_memcat(dest, sl, mime_postfix, sizeof(mime_postfix) - 1);

	return (sl);
}


#if (WEBC_SUPPORT_HTTP_CACHE)

void http_update_cache_timestamp(struct httpc_ctx *ctx)
{
RTP_TIMESTAMP expire_time[1];
long seconds;

	// Update expiration date
	rtp_date_get_timestamp(expire_time);
	if (rtp_date_compare_time(ctx->mime_date, ctx->mime_expires) < 0)
	{
		seconds = rtp_date_time_difference(ctx->mime_date, ctx->mime_expires);
		if (seconds > 0)
		{
			rtp_date_add_seconds(expire_time, seconds);
		}
	}
	else
	{
		// if there is no explicit expiration time, compute an implicit
		// cache time-to-live based on the object's last modification time
		if ((ctx->mime_expires[0].year == 0) && (ctx->mime_lastmod[0].year != 0))
		{
		  #define CACHE_TTL_DIVISOR   10
		  #define CACHE_TTL_MAX       (60 * 60 * 24)  // in seconds (1 day)

			long ttl;

			ttl = (rtp_date_time_difference(ctx->mime_lastmod, ctx->mime_date) + CACHE_TTL_DIVISOR - 1)
			       / CACHE_TTL_DIVISOR;

			if (ttl > CACHE_TTL_MAX)
			{
				ttl = CACHE_TTL_MAX;
			}

			rtp_date_add_seconds(expire_time, ttl);
		}
	}
	wcache_set_expires(&ctx->cfile, expire_time);

	// Update file date and type
	wcache_set_date(&ctx->cfile, ctx->mime_date);
}

void http_create_cache_entry(struct httpc_ctx *ctx)
{
	// tbd - make table which stores boolean for whether to cache certain data types
	if (wcache_open(&gHttpCache, ctx->cname, &ctx->cfile, WEBC_CO_CREATE) >= 0)
	{
		wcache_set_data_type(&ctx->cfile, ctx->mime_data_type);
		wcache_set_charset(&ctx->cfile, (char) ctx->mime_charset);
		http_update_cache_timestamp(ctx);

		// Set to cache this file
		ctx->flags |= HTTPC_FLAG_CACHE_INCOMING;
	}
}

void http_delete_cache_entry(struct httpc_ctx *ctx)
{
	wcache_close(&ctx->cfile);
	wcache_delete_entry(&gHttpCache, ctx->cname);
	wcache_flush(&gHttpCache);
}


void  http_update_cache_expiration_uc(const unsigned short *file, const unsigned short *host, WEBC_UINT16 port, RTP_TIMESTAMP* pExpires)
{
	int n;
	char _file[256];
	char _host[128];

	for (n=0; *file && n<255; n++, file++)
	{
		_file[n] = (char) *file;
	}
	_file[n] = '\0';

	for (n=0; *host && n<127; n++, host++)
	{
		_host[n] = (char) *host;
	}
	_host[n] = '\0';

	http_update_cache_expiration(_file, _host, port, pExpires);
}

void http_update_cache_expiration(const char *file, const char *host, WEBC_UINT16 port, RTP_TIMESTAMP* pExpires)
{
	struct wcache_stream cacheStream;
	char cacheName[WEBC_CACHE_MAX_KEY_LEN + 1];

#if (WEBC_SUPPORT_HTTP_CACHE)
	if (_http_init_cache() < 0)
	{
		return;
	}
#endif // WEBC_SUPPORT_HTTP_CACHE

	http_get_cache_key(cacheName, file, host, port);

	if (pExpires)
	{
		// if pExpires is not NULL, then update the cache entry
		// (if it exists) with a new expiration time
		if (wcache_open(&gHttpCache, cacheName, &cacheStream, WEBC_CO_READ) >= 0)
		{
			wcache_set_expires(&cacheStream, pExpires);
			wcache_close(&cacheStream);
			wcache_flush(&gHttpCache);
		}
	}
	else
	{
		// if pExpires is NULL, then delete entry from cache
		wcache_delete_entry(&gHttpCache, cacheName);
		wcache_flush(&gHttpCache);
	}
}

#endif // WEBC_SUPPORT_HTTP_CACHE


/*****************************************************************************/
// _httpc_parse_* - parse a header field
/*****************************************************************************/

int _httpc_parse_connection(struct httpc_ctx *ctx, char *value)
{
	if (tc_stristr(value, "close"))
	{
		ctx->flags &= ~HTTPC_FLAG_USE_KEEPALIVE;
	}
	return (0);
}

int _httpc_parse_content_length(struct httpc_ctx *ctx, char *value)
{
	ctx->mime_size = tc_atoi(value);
	ctx->flags |= HTTPC_FLAG_HAS_MIME_SIZE;
	return (0);
}

int _httpc_parse_content_type(struct httpc_ctx *ctx, char *value)
{
char *charset;

	while (IS_WHITESPACE(*value))
	{
		value++;
	}

	// only match the part of the string before any ';'
	ctx->mime_data_type = webc_StrToFileContentType(value);

	// now look for charset specifier
	charset = tc_stristr(value, "charset");
	if (charset)
	{
		charset += 7;
		while (IS_WHITESPACE(*charset))
		{
			charset++;
		}
		while (*charset)
		{
			if (*charset == '=')
			{
				charset++;
				while (IS_WHITESPACE(*charset))
				{
					charset++;
				}
				ctx->mime_charset = webc_charset_lookup_8(charset);
				break;
			}
			charset++;
		}
	}
	else
	{
		ctx->mime_charset = WEBC_CHARSET_UNKNOWN;
	}

	return (0);
}

int _httpc_parse_date(struct httpc_ctx *ctx, char *value)
{
	rtp_date_parse_time(ctx->mime_date, value);
	return (0);
}

int _httpc_parse_expires(struct httpc_ctx *ctx, char *value)
{
	rtp_date_parse_time(ctx->mime_expires, value);
	return (0);
}

int _httpc_parse_set_cookie(struct httpc_ctx *ctx, char *value)
{
	webc_set_cookie(value, ctx->host, ctx->fileName);
	return (0);
}

int _httpc_parse_transfer_encoding(struct httpc_ctx *ctx, char *value)
{
	if (tc_stristr(value, "chunked"))
	{
		ctx->flags |= HTTPC_FLAG_CHUNKED;
	}
	return (0);
}

int _httpc_parse_last_modified(struct httpc_ctx *ctx, char *value)
{
    rtp_date_parse_time(ctx->mime_lastmod, value);
    return (0);
}

int _httpc_parse_location(struct httpc_ctx *ctx, char *value)
{
	while (IS_WHITESPACE(value[0]))
	{
		value++;
	}
	if (ctx->redirected_page)
	{
		webc_free_string_copy_8(ctx->redirected_page, __FILE__, __LINE__);
	}
	ctx->redirected_page = webc_malloc_string_copy_8(value, __FILE__, __LINE__);
	return (0);
}

int http_read_chunk_header (struct httpc_ctx *ctx)
{
char buffer[64];

	ctx->bytes_this_chunk = 0;
	_httpc_sock_readline(ctx, buffer, 64);
	ctx->chunk_size = rtp_strtol(buffer, 0, 16);

	return (0);
}


int http_read_chunk_trailer (struct httpc_ctx *ctx)
{
	return (0);
}

/*****************************************************************************/
// _httpc_sock_read - read size bytes from the socket stream
/*****************************************************************************/

long _httpc_sock_read(struct httpc_ctx *ctx, char *buffer, long size)
{
	long result;
	long bytes_read = 0;

	// if there is something in our buffer already, use that first
	if (ctx->in_buffer_begin_ptr < ctx->in_buffer_end_ptr)
	{
		long to_copy = EBSMIN(ctx->in_buffer_end_ptr - ctx->in_buffer_begin_ptr, size);
		tc_movebytes(buffer, ctx->in_buffer + ctx->in_buffer_begin_ptr, to_copy);
		ctx->in_buffer_begin_ptr += to_copy;
		buffer += to_copy;
		size -= to_copy;
		bytes_read += to_copy;
	}

	if (size > 0)
	{
		// read whatever is left directly off the socket
		result = HTTP_IO_RECV(ctx->stream, (unsigned char*) buffer, size, size);

		if (result < 0)
		{
			return (-1);
		}

		bytes_read += result;
	}

	return (bytes_read);
}


/*****************************************************************************/
// _httpc_sock_readline - read up to and including the next CR LF from the
//     socket stream (read at most size bytes)
/*****************************************************************************/

long _httpc_sock_readline(struct httpc_ctx *ctx, char *buffer, long size)
{
long n, to_copy, bytes_copied = 0, bytes_received, unread_data, limit;
unsigned char done = 0;
unsigned char *in_buffer;

	while (1)
	{
		in_buffer = ctx->in_buffer + ctx->in_buffer_begin_ptr;
		unread_data = ctx->in_buffer_end_ptr - ctx->in_buffer_begin_ptr;
		limit = EBSMIN(unread_data, size);

		// search what's in the buffer to see if we have a complete line
		for (n = 0; n < limit; n++)
		{
			if (in_buffer[n] == (unsigned char) '\n')
			{
				n++;
				done = 1;
				break;
			}
		}

		to_copy = n;

		if (to_copy > 0)
		{
			tc_movebytes(buffer, in_buffer, to_copy);

			size -= to_copy;
			buffer += to_copy;
			bytes_copied += to_copy;
			ctx->in_buffer_begin_ptr += to_copy;

			if (done || (size <= 0))
			{
				break;
			}
		}

		ctx->in_buffer_begin_ptr = 0;
		ctx->in_buffer_end_ptr = 0;

		// now receive more data off the socket
		bytes_received = HTTP_IO_RECV(ctx->stream, ctx->in_buffer, 1, ctx->in_buffer_size);

		if (bytes_received < 1)
		{
			break;
		}

		ctx->in_buffer_end_ptr += bytes_received;
	}

	return (bytes_copied);
}


long http_process_mime_header(struct httpc_ctx *ctx, char *buffer, long size)
{
long len;

	ctx->flags |= HTTPC_FLAG_MIME_HDR_READ;

	// Read the status line
	len = _httpc_sock_readline(ctx, buffer, size-1);
	if ((len > 0) && buffer[len-1] == '\n')
	{
		len--;
		if ((len > 0) && buffer[len-1] == '\r')
		{
			len--;
		}
	}
	buffer[len] = '\0';

	// set response code
	if (tc_strnicmp(buffer, "HTTP/", 5) == 0)
	{
		// this is the normal response header form
		ctx->http_code = tc_atoi(&(buffer[9]));

		// if the server responds with HTTP 1.0, then disable persistent connection
		if (buffer[5] == '1' && buffer[6] == '.' && buffer[7] == '0')
		{
			ctx->flags &= ~HTTPC_FLAG_USE_KEEPALIVE;
		}
	}
	else if (tc_strnicmp(buffer, "Status: ", 8) == 0)
	{
		// sometimes we get this type, too
		ctx->http_code = tc_atoi(&(buffer[8]));
	}
	else
	{
		int n;
		for (n=0; buffer[n] && !IS_DIGIT(buffer[n]); n++) {;}
		if (buffer[n])
		{
			ctx->http_code = tc_atoi(&(buffer[n]));
		}
	}

	// Now go through the headers one by one and parse each
	while (1)
	{
		len = _httpc_sock_readline(ctx, buffer, size-1);

		if ((len > 0) && buffer[len-1] == '\n')
		{
			len--;
			if ((len > 0) && buffer[len-1] == '\r')
			{
				len--;
			}
		}

		if (len <= 0)
		{
			break;
		}

		buffer[len] = '\0';

		_httpc_process_one_header(ctx, buffer);
	}

	switch (ctx->http_code)
	{
		case HTTPC_MOVED_PERMANENTLY:
		case HTTPC_MOVED_TEMPORARILY:
		case HTTPC_SEE_OTHER:
			return (HTTPC_PAGE_MOVED);

		case HTTPC_NOT_AUTHORIZED:
			return (HTTPC_AUTHORIZATION_REQUIRED);

		case HTTPC_NOT_PAID:
			return (HTTPC_PAYMENTREQ);
		case HTTPC_NOT_ALLOWED:
			return (HTTPC_FORBIDDEN);
		case HTTPC_NOT_FOUND:
			return (HTTPC_PAGENOTFOUND);
		case HTTPC_NO_OBJECTS:
			return (HTTPC_ENOTFOUND);

		case HTTPC_NOT_MODIFIED:
		  #if (WEBC_SUPPORT_HTTP_CACHE)
			// if the cached file is newer than the specified date (or if no date was
			//  specified), read from the the cached file
			if (giHttpCacheInitialized &&
			    (ctx->flags & HTTPC_FLAG_FILE_IN_CACHE) &&
				(!ctx->p_user_date || (rtp_date_compare_time(&ctx->cache_date, ctx->p_user_date) > 0)))
			{
				http_update_cache_timestamp(ctx);
				ctx->mime_data_type = (WebcFileContentType) wcache_get_data_type(&ctx->cfile);
				ctx->mime_charset = (enum WebcCharset) wcache_get_charset(&ctx->cfile);

				// Read from cached file
				ctx->flags |= HTTPC_FLAG_USE_CACHED_FILE;
			}
			else
		  #endif // WEBC_SUPPORT_HTTP_CACHE
			{
				return (HTTPC_ENOTMODIFIED);
			}
			break;

		default:
		  #if (WEBC_SUPPORT_HTTP_CACHE)
			if (giHttpCacheInitialized)
			{
				if (ctx->flags & HTTPC_FLAG_FILE_IN_CACHE)
				{
					// Discard the cached file and start caching the incoming data
					wcache_close(&ctx->cfile);
					wcache_delete_entry(&gHttpCache, ctx->cname);
				}

				http_create_cache_entry(ctx);
			}
		  #endif // WEBC_SUPPORT_HTTP_CACHE
			break;
	}

	return (0);
}

int _httpc_process_one_header(struct httpc_ctx *ctx, char *buffer)
{
int n;
char *value;

	for (n=0; gHttpHeaderTable[n].pName; n++)
	{
		// tbd - use ebs_BinarySearch here
		if (tc_stristr(buffer, gHttpHeaderTable[n].pName) == buffer)
		{
			// found a match
			value = tc_strchr(buffer, ':');
			if (value)
			{
				value++;
				gHttpHeaderTable[n].Parse(ctx, value);
				return (1);
			}
		}
	}

	return (0);
}

int _httpc_parse_name_value_pair (char *str, char *name, char *value, long nameSize, long valueSize)
{
	while (IS_WHITESPACE(str[0]))
	{
		str++;
	}

	nameSize--;
	while (str[0] && str[0] != '=' && nameSize > 0)
	{
		name[0] = str[0];
		nameSize--;
		name++;
		str++;
	}
	name[0] = 0;

	while (str[0] && str[0] != '=')
	{
		str++;
	}

	if (str[0] != '=')
	{
		return (-1);
	}

	str++;

	while (IS_WHITESPACE(str[0]))
	{
		str++;
	}

	if (str[0] == '\'' || str[0] == '"')
	{
		str++;
	}

	valueSize--;
	while (str[0] && str[0] != '\'' && str[0] != '"' && valueSize > 0)
	{
		value[0] = str[0];
		valueSize--;
		value++;
		str++;
	}
	value[0] = 0;

	return (0);
}


int http_set_authorization (char *host, unsigned short port, char *path, char *realmName,
		HTTPAuthenticationScheme scheme, char *userName, char *password, long ttlSec)
{
  #if (HTTPC_SUPPORT_AUTHENTICATE)

	HTTPAuthenticationRealm * realm = 0;

	/* first try to find an existing authentication realm that matches */
	if (realmName)
	{
		/* find by realm name */
		realm = _http_get_authentication_realm(host, port, realmName);
		if (realm)
		{
			_http_realm_update_path(realm, path);
		}
		else if (path)
		{
			/* find by common subtree */
			realm = _http_get_authentication_realm_by_path(host, port, path);
		}
	}

	/* no such realm is known; create a new one */
	if (!realm)
	{
		realm = _http_create_authentication_realm(host, port, path, realmName, ttlSec, scheme);
		if (!realm)
		{
			return (-1);
		}
	}

	realm->scheme = scheme;
	_http_realm_set_ttl(realm, ttlSec);
	_http_realm_set_user(realm, userName);
	_http_realm_set_password(realm, password);

	return (0);

  #else

	return (-1);

  #endif // HTTPC_SUPPORT_AUTHENTICATE
}

void http_reset_all_authorizations (void)
{
  #if (HTTPC_SUPPORT_AUTHENTICATE)

	HTTPAuthenticationRealm * realm, * nextRealm;
	HTTPAuthenticationHost * host = gpHttpFirstHost, * nextHost;

	while (host)
	{
		realm = host->pFirstRealm;
		while (realm)
		{
			nextRealm = realm->pNext;
			_http_host_remove_realm(host, realm);
			_http_delete_realm(realm);
			realm = nextRealm;
		}

		nextHost = host->pNext;
		_http_remove_authentication_host(host);
		_http_delete_host(host);
		host = nextHost;
	}

  #endif // HTTPC_SUPPORT_AUTHENTICATE
}

void http_reset_authorization_realm (char *host, unsigned short port, char *realmName)
{
  #if (HTTPC_SUPPORT_AUTHENTICATE)

	HTTPAuthenticationRealm * realm = _http_get_authentication_realm(host, port, realmName);
	if (realm)
	{
		HTTPAuthenticationHost * host = realm->host;

		_http_host_remove_realm(host, realm);
		_http_delete_realm(realm);

		if (host->pFirstRealm == 0)
		{
			_http_remove_authentication_host(host);
			_http_delete_host(host);
		}
	}

  #endif // HTTPC_SUPPORT_AUTHENTICATE
}

static const char *schemeNames[] = {"unknown", "Basic", "Digest"};

const char *http_auth_scheme_to_str (HTTPAuthenticationScheme scheme)
{
	int n = EBSCLIP((int) scheme, 0, 2);
	return (schemeNames[n]);
}

#if (HTTPC_SUPPORT_AUTHENTICATE)

/* The format of WWW-Authenticate is:
      WWW-Authenticate: scheme (param)*\r\n

   The schemes we recognize are "Basic"
*/

int _httpc_parse_www_authenticate(struct httpc_ctx *ctx, char *value)
{
	char name[10], param[64];

	// parse the scheme
	while (IS_WHITESPACE(value[0]))
	{
		value++;
	}

	if (!rtp_strnicmp(value, "Basic", 5))
	{
		// parse Basic authentication paramaters
		ctx->scheme = HTTP_AUTHENTICATION_BASIC;
		value += 5;
	}
	else
	{
		return (0);
	}

	if (_httpc_parse_name_value_pair (value, name, param, 10, 64) < 0)
	{
		return (0);
	}

	if (!rtp_stricmp(name, "realm"))
	{
		HTTPAuthenticationRealm* realm = _http_get_authentication_realm(ctx->host, ctx->port, param);
		if (realm)
		{
			_http_realm_update_path(realm, ctx->fileName);
			realm->scheme = ctx->scheme;
		}

		if (ctx->realm)
		{
			webc_free_string_copy_8(ctx->realm, __FILE__, __LINE__);
		}

		ctx->realm = webc_malloc_string_copy_8(param, __FILE__, __LINE__);
	}

	return (0);
}

HTTPAuthenticationRealm * _http_create_authentication_realm (char *hostName, unsigned short port,
		char *path,	char *realmName, long ttlSec, HTTPAuthenticationScheme scheme)
{
	HTTPAuthenticationRealm *realm = 0;
	HTTPAuthenticationHost *host = _http_get_authentication_host(hostName, port);

	if (!host)
	{
		host = _http_new_host(hostName, port);
		if (!host)
		{
			return (0);
		}
	}

	realm = _http_new_realm(realmName, path, ttlSec, scheme);
	if (!realm)
	{
		_http_delete_host(host);
		return (0);
	}

	_http_add_authentication_host(host);
	_http_host_add_realm(host, realm);

	return (realm);
}


HTTPAuthenticationRealm * _http_get_authentication_realm (char *hostName, unsigned short port, char *realmName)
{
	HTTPAuthenticationRealm * realm = 0;
	HTTPAuthenticationHost * host = _http_get_authentication_host(hostName, port);

	if (host)
	{
		realm = host->pFirstRealm;
		while (realm)
		{
			if (!tc_strcmp(realm->realmName, realmName))
			{
				break;
			}
			realm = realm->pNext;
		}
	}

	return (realm);
}

HTTPAuthenticationRealm * _http_get_authentication_realm_by_path (char *hostName, unsigned short port, char *path)
{
	HTTPAuthenticationRealm * realm = 0;
	HTTPAuthenticationHost * host = _http_get_authentication_host(hostName, port);
	long pathLen, realmPathLen;

	if (host)
	{
		pathLen = tc_strlen(path);
		realm = host->pFirstRealm;
		while (realm)
		{
			realmPathLen = tc_strlen(realm->path);
			if (realmPathLen > 0 && realmPathLen <= pathLen)
			{
				if (!tc_strncmp(realm->path, path, realmPathLen))
				{
					break;
				}
			}
			realm = realm->pNext;
		}
	}

	return (realm);
}

HTTPAuthenticationHost * _http_get_authentication_host (char *hostName, unsigned short port)
{
	HTTPAuthenticationHost * host = gpHttpFirstHost;

	while (host)
	{
		if (!tc_stricmp(hostName, host->hostName) && port == host->port)
		{
			return (host);
		}

		host = host->pNext;
	}

	return (0);
}

void _http_add_authentication_host (HTTPAuthenticationHost *host)
{
	host->pPrev = gpHttpLastHost;
	host->pNext = 0;
	if (gpHttpLastHost)
	{
		gpHttpLastHost->pNext = host;
	}
	else
	{
		gpHttpFirstHost = host;
	}
	gpHttpLastHost = host;
}

void _http_remove_authentication_host (HTTPAuthenticationHost *host)
{
	if (host->pPrev)
	{
		host->pPrev->pNext = host->pNext;
	}
	else
	{
		gpHttpFirstHost = host->pNext;
	}

	if (host->pNext)
	{
		host->pNext->pPrev = host->pPrev;
	}
	else
	{
		gpHttpLastHost = host->pPrev;
	}

	host->pPrev = 0;
	host->pNext = 0;
}

/* Authentication Host management functions */
HTTPAuthenticationHost * _http_new_host (char *hostName, unsigned short port)
{
	HTTPAuthenticationHost * host = (HTTPAuthenticationHost *) WEBC_MALLOC(sizeof(HTTPAuthenticationHost));

	if (host)
	{
		tc_memset(host, 0, sizeof(HTTPAuthenticationHost));
		tc_strncpy(host->hostName, hostName, HTTPC_CFG_MAX_HOSTNAME_LEN-1);
		host->hostName[HTTPC_CFG_MAX_HOSTNAME_LEN-1] = 0;
		host->port = port;
	}

	return (host);
}

void _http_delete_host (HTTPAuthenticationHost *host)
{
	WEBC_FREE(host);
}

void _http_host_add_realm (HTTPAuthenticationHost *host, HTTPAuthenticationRealm *realm)
{
	realm->pPrev = host->pLastRealm;
	realm->pNext = 0;
	if (host->pLastRealm)
	{
		host->pLastRealm->pNext = realm;
	}
	else
	{
		host->pFirstRealm = realm;
	}
	host->pLastRealm = realm;
	realm->host = host;
}

void _http_host_remove_realm (HTTPAuthenticationHost *host, HTTPAuthenticationRealm *realm)
{
	if (realm->pPrev)
	{
		realm->pPrev->pNext = realm->pNext;
	}
	else
	{
		host->pFirstRealm = realm->pNext;
	}

	if (realm->pNext)
	{
		realm->pNext->pPrev = realm->pPrev;
	}
	else
	{
		host->pLastRealm = realm->pPrev;
	}

	realm->pPrev = 0;
	realm->pNext = 0;
	realm->host = 0;
}

/* Authentication Realm management functions */
HTTPAuthenticationRealm * _http_new_realm (char *realmName, char *path, long ttlSec,
                                           HTTPAuthenticationScheme scheme)
{
	HTTPAuthenticationRealm * realm = (HTTPAuthenticationRealm *) WEBC_MALLOC(sizeof(HTTPAuthenticationRealm));

	if (realm)
	{
		tc_memset(realm, 0, sizeof(HTTPAuthenticationRealm));
		tc_strncpy(realm->realmName, realmName, HTTPC_CFG_MAX_REALMNAME_LEN-1);
		realm->realmName[HTTPC_CFG_MAX_REALMNAME_LEN-1] = 0;
		tc_strncpy(realm->path, path, HTTPC_CFG_MAX_PATH_LEN-1);
		realm->path[HTTPC_CFG_MAX_PATH_LEN-1] = 0;
		realm->scheme = scheme;
	}

	return (realm);
}

void _http_delete_realm (HTTPAuthenticationRealm *realm)
{
	WEBC_FREE(realm);
}

void _http_realm_set_ttl (HTTPAuthenticationRealm *realm, long ttlSec)
{
	if (!ttlSec)
	{
		rtp_date_set_time_forever(&realm->expires);
	}
	else
	{
		rtp_date_get_timestamp(&realm->expires);
		rtp_date_add_seconds(&realm->expires, ttlSec);
	}
	realm->ttlSec = ttlSec;
}

// Find the longest prefix that both paths share and set that as the new path
void _http_realm_update_path (HTTPAuthenticationRealm *realm, char *path)
{
	long n;

	if (!realm->path[0])
	{
		tc_strncpy(realm->path, path, HTTPC_CFG_MAX_PATH_LEN-1);
		realm->path[HTTPC_CFG_MAX_PATH_LEN-1] = 0;
		return;
	}

	for (n=0; realm->path[n] == path[n]; n++)
	{
		if (!path[n] || !realm->path[n])
		{
			break;
		}
	}

	realm->path[n] = 0;
}

int _http_realm_expired (HTTPAuthenticationRealm *realm)
{
	RTP_TIMESTAMP current;

	rtp_date_get_timestamp(&current);
	return (rtp_date_compare_time(&realm->expires, &current) < 0);
}

void _http_realm_set_user(HTTPAuthenticationRealm *realm, char *user)
{
	switch (realm->scheme)
	{
		case HTTP_AUTHENTICATION_BASIC:
			tc_strncpy(realm->param.basic.userName, user, HTTPC_CFG_MAX_USERNAME_LEN-1);
			realm->param.basic.userName[HTTPC_CFG_MAX_USERNAME_LEN-1] = 0;
			break;

		case HTTP_AUTHENTICATION_DIGEST:
			tc_strncpy(realm->param.digest.userName, user, HTTPC_CFG_MAX_USERNAME_LEN-1);
			realm->param.digest.userName[HTTPC_CFG_MAX_USERNAME_LEN-1] = 0;
			break;

		default:
			break;
	}
}

void _http_realm_set_password(HTTPAuthenticationRealm *realm, char *password)
{
	switch (realm->scheme)
	{
		case HTTP_AUTHENTICATION_BASIC:
			tc_strncpy(realm->param.basic.password, password, HTTPC_CFG_MAX_PASSWORD_LEN-1);
			realm->param.basic.password[HTTPC_CFG_MAX_PASSWORD_LEN-1] = 0;
			break;

		case HTTP_AUTHENTICATION_DIGEST:
			tc_strncpy(realm->param.digest.password, password, HTTPC_CFG_MAX_PASSWORD_LEN-1);
			realm->param.digest.password[HTTPC_CFG_MAX_PASSWORD_LEN-1] = 0;
			break;

		default:
			break;
	}
}

void _http_check_realm_expirations(void)
{
	HTTPAuthenticationRealm * realm, * nextRealm;
	HTTPAuthenticationHost * host = gpHttpFirstHost, * nextHost;

	while (host)
	{
		realm = host->pFirstRealm;
		while (realm)
		{
			nextRealm = realm->pNext;
			if (_http_realm_expired(realm))
			{
				_http_host_remove_realm(host, realm);
				_http_delete_realm(realm);
			}
			realm = nextRealm;
		}

		nextHost = host->pNext;
		if (host->pFirstRealm == 0)
		{
			_http_remove_authentication_host(host);
			_http_delete_host(host);
		}
		host = nextHost;
	}
}

long _http_print_authorization (struct httpc_ctx *ctx, char *buffer)
{
	HTTPAuthenticationRealm *realm;

	if (!buffer)
	{
		_http_check_realm_expirations();
	}

	realm = _http_get_authentication_realm_by_path(ctx->host, ctx->port, ctx->fileName);
	if (realm)
	{
		long len = _http_realm_print_authorization(realm, buffer);
		ctx->flags |= HTTPC_FLAG_AUTHORIZATION_SENT;
		//_http_realm_set_ttl(realm, realm->ttlSec); // reset time to live?

		return (len);
	}

	return (0);
}

long _http_realm_print_authorization (HTTPAuthenticationRealm *realm, char *buffer)
{
	switch (realm->scheme)
	{
		case HTTP_AUTHENTICATION_BASIC:
			if (realm->param.basic.userName[0])
			{
				long len = 0;
				char auth[HTTPC_CFG_MAX_USERNAME_LEN + HTTPC_CFG_MAX_PASSWORD_LEN];

				len = tc_memcat(buffer, len, authorization_prefix, sizeof(authorization_prefix) - 1);
				len = tc_memcat(buffer, len, basic_string, sizeof(basic_string) - 1);

				tc_strcpy(auth, realm->param.basic.userName);
				tc_strcat(auth, ":");
				tc_strcat(auth, realm->param.basic.password);

				len += _http_base64_encode(buffer? (buffer + len) : 0, (unsigned char *) auth, (long) tc_strlen(auth));

				return (len);
			}
			break;

		default:
			break;
	}

	return (0);
}

long _http_base64_encode (char *dst, unsigned char *src, long size)
{
	long len = 0;

	while (size > 0)
	{
		switch (size)
		{
			case 1:
				// encode 1 WEBC_UINT8
				if (dst)
				{
					dst[0] = _base64[((src[0] >> 2) & 0x3f)];
					dst[1] = _base64[((src[0] << 4) & 0x30)];
					dst[2] = '=';
					dst[3] = '=';
				}

				src++;
				size--;
				break;

			case 2:
				// encode 2 bytes
				if (dst)
				{
					dst[0] = _base64[((src[0] >> 2) & 0x3f)];
					dst[1] = _base64[((src[0] << 4) & 0x30) | ((src[1] >> 4) & 0x0f)];
					dst[2] = _base64[((src[1] << 2) & 0x3c)];
					dst[3] = '=';
				}

				src += 2;
				size -= 2;
				break;

			default:
				// encode 3 bytes
				if (dst)
				{
					dst[0] = _base64[((src[0] >> 2) & 0x3f)];
					dst[1] = _base64[((src[0] << 4) & 0x30) | ((src[1] >> 4) & 0x0f)];
					dst[2] = _base64[((src[1] << 2) & 0x3c) | ((src[2] >> 6) & 0x03)];
					dst[3] = _base64[(src[2] & 0x3f)];
				}

				src += 3;
				size -= 3;
				break;
		}

		len += 4;
		if (dst)
		{
			dst += 4;
		}
	}

	return (len);
}

#endif // HTTPC_SUPPORT_AUTHENTICATE

void http_set_secure (struct httpc_ctx* ctx, int on)
{
	if (on)
	{
		ctx->flags |= HTTPC_FLAG_SECURE;
	}
	else
	{
		ctx->flags &= ~HTTPC_FLAG_SECURE;
	}
}

void http_set_access_scheme (struct httpc_ctx* ctx, char *accessScheme)
{
	ctx->access_scheme = accessScheme;
}

#endif // HTTP or HTTPS

#if (WEBC_SUPPORT_HTTP_CACHE)
static int _http_init_cache (void)
{
	if (!giHttpCacheInitialized)
	{
		const struct wcache_system_spec* spec;
		void* sysPtr;

		if (httpc_init_cache_storage(&spec, &sysPtr) >= 0)
		{
			if (wcache_init(&gHttpCache, WEBC_CFG_HTTP_CACHE_LOCATION,
							WEBC_CFG_HTTP_CACHE_SIZE,
							WEBC_CFG_HTTP_CACHE_MAX_FILES,
							spec, sysPtr) == 0)
			{
				giHttpCacheInitialized = 1;
			}
		}
	}

	if (giHttpCacheInitialized)
	{
		return 0;
	}

	return -1;
}
#endif /* WEBC_SUPPORT_HTTP_CACHE */
/* Buffering system used to bypass the network stack when fetching web pages
   using the loopback interface in polled mode.

    This code that acts as a back door for webC and the web server to bypass the stack.
    The buffer must be large enough to hold all of the content sent by the server for a single HTTP get.
    This must be as large as largest image or page will be fetched.
    The default is set to 1 megabyte, this is larger enough to fetch a page as large as one meg.

    This method appears to provide good predicatble performance running in loopback mode.
    The only easilly attainable alternative is to switch to multitasking which would then run
    the server in a thread.

 */
#include <assert.h>
#define BUFFERSIZE 0x100000
static char tcpbuffer[BUFFERSIZE];
static int rtiptcpbuffercount;
static int webctcpbuffercount;
void RtipToWebC(char *p, int nbytes)
{
    if (rtiptcpbuffercount + nbytes >= BUFFERSIZE)
    {
        assert(rtiptcpbuffercount + nbytes >= BUFFERSIZE);
        return;
    }
     memcpy(&tcpbuffer[rtiptcpbuffercount], p, nbytes);
    rtiptcpbuffercount += nbytes;
}

 int  WebCReadData(char *p, int buffersize)
{
int n=EBSMIN(buffersize,(rtiptcpbuffercount-webctcpbuffercount));
    memcpy(p, &tcpbuffer[webctcpbuffercount], n);
    webctcpbuffercount += n;
    if ((rtiptcpbuffercount-webctcpbuffercount)==0)
    {
        rtiptcpbuffercount=0;
        webctcpbuffercount=0;
    }
    return n;
}
#endif // (WEBC_SUPPORT_HTTP || WEBC_SUPPORT_HTTPS)
// END HTTPC.C
