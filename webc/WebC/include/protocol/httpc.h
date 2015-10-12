#ifndef __HTTPC_H__
#define __HTTPC_H__

#include "webcfg.h"
#include "wcache.h"
#include "util.h"
#include "webcsock.h"
#include "webc.h"
#include "webcutf.h"

#define USE_NON_BLOCKING_IO     1
#define HTTP_REPORT_STATUS      1

#define MAX_MIME_HEADER_LEN 1024

typedef enum e_HTTPAuthenticationScheme
{
	HTTP_AUTHENTICATION_UNKNOWN = 0,
	HTTP_AUTHENTICATION_BASIC,
	HTTP_AUTHENTICATION_DIGEST

} HTTPAuthenticationScheme;

typedef struct s_HTTPStreamClass
{
	int        (*getHostAddr) (struct s_HTTPStreamClass** instance,
	                           WEBC_UINT8* ipaddr,
	                           WEBC_CHAR8* name);

	int        (*connectTo)   (struct s_HTTPStreamClass** instance,
	                           WEBC_UINT8* remoteAddr,
	                           WEBC_UINT16 port);

	WEBC_BOOL  (*isConnected) (struct s_HTTPStreamClass** instance);

	WEBC_INT32 (*sendData)    (struct s_HTTPStreamClass** instance,
	                           const WEBC_UINT8* data,
	                           WEBC_INT32 length);

	WEBC_INT32 (*recvData)    (struct s_HTTPStreamClass** instance,
	                           WEBC_UINT8* data,
	                           WEBC_INT32 minBytes,
	                           WEBC_INT32 maxBytes);

	WEBC_BOOL  (*cacheStream) (struct s_HTTPStreamClass** instance);

	void       (*closeStream) (struct s_HTTPStreamClass** instance);
}
HTTPStreamClass;

typedef struct s_HTTPStreamFactoryClass
{
	HTTPStreamClass** (*newStream) (struct s_HTTPStreamFactoryClass** factory);
	HTTPStreamClass** (*activate)  (struct s_HTTPStreamFactoryClass** factory,
	                                HTTPStreamClass** cachedStream);
}
HTTPStreamFactoryClass;

struct httpc_ctx
{
	HTTPStreamClass**        stream;
	HTTPStreamFactoryClass** factory;

	WEBC_UINT16              port;
  #if(USE_NON_BLOCKING_IO)
	WEBC_UINT32              start_time;
  #endif
	char   *redirected_page;
	int     http_code;

	WebcFileContentType mime_data_type;			// HTML_MIME_TYPE_PLAIN_TEXT etc
	WEBC_UINT32         mime_size;
	RTP_TIMESTAMP       mime_date[1];
	RTP_TIMESTAMP       mime_expires[1];
	RTP_TIMESTAMP       mime_lastmod[1];
	enum WebcCharset    mime_charset;

	long                     bytes_received;
  	char*                    host;
  	char*                    fileName;
  	char*                    access_scheme;

  	/* authentication request parameters */
  	char*                    realm;
  	HTTPAuthenticationScheme scheme;

  	WEBC_UINT16              flags;

#define HTTPC_FLAG_REDIRECTED          0x0001
#define HTTPC_FLAG_CHUNKED             0x0002
#define HTTPC_FLAG_LAST_CHUNK_READ     0x0004
#define HTTPC_FLAG_HAS_MIME_SIZE       0x0008
#define HTTPC_FLAG_MIME_HDR_READ       0x0010
#define HTTPC_FLAG_USE_CACHED_FILE     0x0020
#define HTTPC_FLAG_FILE_IN_CACHE       0x0040
#define HTTPC_FLAG_CACHE_INCOMING      0x0080
#define HTTPC_FLAG_USE_KEEPALIVE       0x0100
#define HTTPC_FLAG_AUTHORIZATION_SENT  0x0200
#define HTTPC_FLAG_SECURE              0x0400

	WEBC_UINT8*              in_buffer;
	long                     in_buffer_size;
	long                     in_buffer_begin_ptr;
	long                     in_buffer_end_ptr;

  	// required for HTTP 1.1
  	WEBC_UINT32              chunk_size;
  	WEBC_UINT32              bytes_this_chunk;

  	WEBC_UINT8               ip_addr[4];
  	WEBC_UINT32              bytes_this_file;

  #if (HTTP_REPORT_STATUS)
	webcReportFn             report_func;
	WEBC_PFBYTE              report_ctx;
  #endif

  #if (WEBC_SUPPORT_HTTP_CACHE)
	struct wcache_stream     cfile;
	RTP_TIMESTAMP            cache_date;
	RTP_TIMESTAMP*           p_user_date;
	RTP_TIMESTAMP            user_date[1];
	char                     cname[WEBC_CACHE_MAX_KEY_LEN + 1];
  #endif // WEBC_SUPPORT_HTTP_CACHE
};


/* return codes for http_recv */
#define HTTPC_FATAL_ERROR             -1
#define HTTPC_PAGE_MOVED              -2
#define HTTPC_UNSUPPORTED_MIME_TYPE   -3
#define HTTPC_ENOTMODIFIED            -4
#define HTTPC_ENOTFOUND               -5
#define HTTPC_AUTHORIZATION_REQUIRED  -401
#define HTTPC_PAYMENTREQ              -402
#define HTTPC_FORBIDDEN               -403
#define HTTPC_PAGENOTFOUND            -404

/* values for http_code */
#define HTTPC_OK                     200
#define HTTPC_MOVED_PERMANENTLY      301
#define HTTPC_MOVED_TEMPORARILY      302
#define HTTPC_SEE_OTHER              303
#define HTTPC_NOT_MODIFIED           304
#define HTTPC_NOT_AUTHORIZED         401
#define HTTPC_NOT_PAID               402
#define HTTPC_NOT_ALLOWED            403
#define HTTPC_NOT_FOUND              404
#define HTTPC_NO_OBJECTS             406

#ifdef __cplusplus
extern "C" {
#endif

// API routines
#if (WEBC_SUPPORT_CACHE)
void  http_clear_cache(void);
#endif
int   webc_http_init  (struct httpc_ctx* ctx, HTTPStreamFactoryClass** factory);
int   http_get   (struct httpc_ctx* ctx, char* file, char *host, WEBC_UINT16 port, RTP_TIMESTAMP* if_modified_since, char * referer);
int   http_post  (struct httpc_ctx* ctx, char* action, char *host, WEBC_UINT16 port, char *content_type, long length);
long  http_recv  (struct httpc_ctx* ctx, char* buffer, long size);
long  http_send  (struct httpc_ctx* ctx, char* buffer, long size);
int   webc_http_close (struct httpc_ctx* ctx);
int   http_exit  (void);
//int   http_set_idle_fn   (struct httpc_ctx *ctx, webcIdleFn idle_func, WEBC_PFBYTE idle_data);
int   http_set_report_fn (struct httpc_ctx *ctx, webcReportFn report_func, WEBC_PFBYTE report_ctx);
void  http_set_secure    (struct httpc_ctx* ctx, int on);
void  http_set_access_scheme (struct httpc_ctx* ctx, char *accessScheme);  // Mitel Change


void  http_timeout_connections (void);
long  http_msecs_to_next_timeout (void);

// Internal routines
int   http_connect                (struct httpc_ctx *ctx, WEBC_PFBYTE ip_addr, WEBC_UINT16 port);
long  http_process_mime_header    (struct httpc_ctx *ctx, char *buffer, long size);
char* http_find_mime_field        (char *header, char *field);
long  http_format_get_header      (struct httpc_ctx *ctx, char *buffer, const char *file, const char *host, unsigned short port, RTP_TIMESTAMP* modified_date,const char* referer);

long  http_format_post_header     (struct httpc_ctx *ctx, char *dest, const char *host, unsigned short port, const char *action, const char *content_type, long length);
#if (HTTP_REPORT_STATUS)
void  http_report_status          (struct httpc_ctx *ctx, char *status);
#endif
#if (WEBC_SUPPORT_HTTP_CACHE)
int   httpc_init_cache_storage       (const struct wcache_system_spec** spec, void** instance);
void  httpc_destroy_cache_storage    (void);
void  http_get_cache_key             (char *pKey, const char *pFile, const char *pHost, WEBC_UINT16 port);
void  http_update_cache_timestamp    (struct httpc_ctx *ctx);
void  http_create_cache_entry        (struct httpc_ctx *ctx);
void  http_delete_cache_entry        (struct httpc_ctx *ctx);
void  http_update_cache_expiration   (const char *file, const char *host, WEBC_UINT16 port, RTP_TIMESTAMP* pExpires);
void  http_update_cache_expiration_uc(const unsigned short *file, const unsigned short *host, WEBC_UINT16 port, RTP_TIMESTAMP* pExpires);
#endif	// WEBC_SUPPORT_HTTP_CACHE

// if support_chunky?
int   http_read_chunk_header (struct httpc_ctx *ctx);
int   http_read_chunk_trailer (struct httpc_ctx *ctx);

// HTTP client authentication;
//   only works if HTTPC_SUPPORT_AUTHENTICATE (httpc.c) is set to 1
int http_set_authorization (char *host, unsigned short port, char *path, char *realmName,
		HTTPAuthenticationScheme scheme, char *userName, char *password, long ttlSec);
void http_reset_all_authorizations (void);
void http_reset_authorization_realm (char *host, unsigned short port, char *realmName);
const char *http_auth_scheme_to_str (HTTPAuthenticationScheme scheme);

#ifdef __cplusplus
}
#endif

#endif // WEBC_SUPPORT_HTTP


// END HTTPC.H
