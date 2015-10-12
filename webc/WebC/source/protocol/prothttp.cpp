//
// PROTHTTP.C - Routines to load URL using HTTP protocol
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

#include "htmlsdk.h"
#include "webload.hpp"
#include "httpc.h"
#include "webcsock.h"
#include "prothttp.h"
#include "webcstr.h"
#include "strbld.hpp"
#include "htmlbrow.hpp"
#include "htmldoc.hpp"
#include "webcassert.h"
#include "webcmem.h"
#include "sramdisk.hpp"

/*****************************************************************************/
// Local types
/*****************************************************************************/

/*****************************************************************************/
// Local functions
/*****************************************************************************/


HTTPStreamClass**   webc_SocketStreamNew         (HTTPStreamFactoryClass** factory);

HTTPStreamClass**   webc_SocketStreamActivate    (HTTPStreamFactoryClass** factory,
												  HTTPStreamClass** cachedStream);

int                 webc_SocketStreamGetHostAddr (HTTPStreamClass** instance,
                                                  WEBC_UINT8* ipaddr,
                                                  WEBC_CHAR8* name);

int                 webc_SocketStreamConnect     (HTTPStreamClass** instance,
                                                  WEBC_UINT8* remoteAddr,
                                                  WEBC_UINT16 port);

WEBC_BOOL           webc_SocketStreamIsConnected (HTTPStreamClass** instance);

WEBC_INT32          webc_SocketStreamSend        (HTTPStreamClass** instance,
                                                  const WEBC_UINT8* data,
                                                  WEBC_INT32 length);

WEBC_INT32          webc_SocketStreamRecv        (HTTPStreamClass** instance,
                                                  WEBC_UINT8* data,
                                                  WEBC_INT32 minBytes,
                                                  WEBC_INT32 maxBytes);

WEBC_BOOL           webc_SocketStreamCache       (HTTPStreamClass** instance);

void                webc_SocketStreamClose       (HTTPStreamClass** instance);

/*****************************************************************************/
// Data
/*****************************************************************************/

unsigned char* gpHttpAuthCallbackParam = 0;

WEBC_READONLY HTTPStreamClass gWebcSocketStreamClass =
{
	webc_SocketStreamGetHostAddr,
	webc_SocketStreamConnect,
	webc_SocketStreamIsConnected,
	webc_SocketStreamSend,
	webc_SocketStreamRecv,
	webc_SocketStreamCache,
	webc_SocketStreamClose
};

WEBC_READONLY HTTPStreamFactoryClass gWebcSocketStreamFactoryClass =
{
	webc_SocketStreamNew,
	webc_SocketStreamActivate
};

#if (WEBC_SUPPORT_HTTP && WEBC_SUPPORT_HTTP_CACHE)
#ifdef WEBC_HTTP_USE_RAM_CACHE
SimpleRamDisk* gHttpRamDisk = 0;
#endif
#endif


/*****************************************************************************/
// Function Bodies
/*****************************************************************************/

#if (WEBC_SUPPORT_HTTP)

int wload_HttpGet(UrlStreamCtx *pStream)
{
int result = -1;
char *path = webc_arg_to_char(pStream->p_desc->getPath());
char *host = webc_arg_to_char(pStream->p_desc->getHost());
struct httpc_ctx *ctx = &pStream->stream.http.ctx;

	webc_network_init();

	pStream->stream.http.stop = 0;
	pStream->stream.http.factory.base = (HTTPStreamFactoryClass*) &gWebcSocketStreamFactoryClass;
	pStream->stream.http.factory.idle_func = pStream->idle_func;
	pStream->stream.http.factory.idle_data = pStream->idle_data;

	webc_http_init(ctx, &pStream->stream.http.factory.base);
	http_set_report_fn(ctx, pStream->report_func, pStream->report_ctx);


	if (path && host)
	{
		result = http_get(ctx, path, host, pStream->p_desc->getPort(), pStream->p_date,0);
	}

	webc_arg_done(host);
	webc_arg_done(path);

	return (result);
}

int wload_HttpPost(UrlStreamCtx *pStream, WEBC_CHAR * pContentType, long lContentLength)
{
int result = -1;
char *path = webc_arg_to_char(pStream->p_desc->getPath());
char *host = webc_arg_to_char(pStream->p_desc->getHost());
char *type = webc_arg_to_char(pContentType);
struct httpc_ctx *ctx = &pStream->stream.http.ctx;

	webc_network_init();

	pStream->stream.http.stop = 0;
	pStream->stream.http.factory.base = (HTTPStreamFactoryClass*) &gWebcSocketStreamFactoryClass;
	pStream->stream.http.factory.idle_func = pStream->idle_func;
	pStream->stream.http.factory.idle_data = pStream->idle_data;

	webc_http_init(ctx, &pStream->stream.http.factory.base);
	http_set_report_fn(ctx, pStream->report_func, pStream->report_ctx);

	if (path && host)
	{
		static const char defaultContentType [] = "application/x-www-form-urlencoded";

		if (!type)
		{
			type = (char*) defaultContentType;
		}

		result = http_post(ctx, path, host, pStream->p_desc->getPort(),
						   type, lContentLength);

		if (type == defaultContentType)
		{
			type = 0;
		}
	}

	webc_arg_done(type);
	webc_arg_done(host);
	webc_arg_done(path);

	return (result);
}
extern "C" int http_server_process_one(void);

long wload_HttpRead(UrlStreamCtx *pStream, char * buffer, long length)
{
long bytes_read;
struct httpc_ctx *ctx = &pStream->stream.http.ctx;

	if (pStream->stream.http.stop)
	{
		return (0);
	}



	bytes_read = http_recv(ctx, buffer, length);

	switch (bytes_read)
	{
		case HTTPC_AUTHORIZATION_REQUIRED:
		{
			HTMLDocument* doc = (HTMLDocument*) pStream->report_ctx;
			if (doc)
			{
				HTMLBrowser* browser = doc->GetBrowser();
				HTMLBrowserEvent e;

				e.type = HTML_BROWSER_EVENT_HTTP_NOT_AUTHORIZED;
				if (ctx->realm)
				{
					rtp_strncpy(e.data.authDomain.realm, ctx->realm, WEBC_MAX_AUTH_REALM_LEN-1);
					e.data.authDomain.realm[WEBC_MAX_AUTH_REALM_LEN-1] = 0;
				}
				else
				{
					e.data.authDomain.realm[0] = 0;
				}
				e.data.authDomain.url[0] = 0;

				UrlStringBuilder ubld(pStream->p_desc);
				WEBC_CHAR *urlStr = ubld.MallocString(__FILE__, __LINE__);
				if (urlStr)
				{
					char *url = webc_arg_to_char(urlStr);
					if (url)
					{
						rtp_strncpy(e.data.authDomain.url, url, WEBC_MAX_AUTH_URL_LEN-1);
						e.data.authDomain.url[WEBC_MAX_AUTH_URL_LEN-1] = 0;
						webc_arg_done(url);
					}
					ubld.FreeString(urlStr, __FILE__, __LINE__);
				}

				const char* schemeName = http_auth_scheme_to_str(ctx->scheme);

				if (schemeName)
				{
					rtp_strncpy(e.data.authDomain.scheme, schemeName, WEBC_MAX_AUTH_SCHEME_LEN-1);
					e.data.authDomain.scheme[WEBC_MAX_AUTH_SCHEME_LEN-1] = 0;
				}
				else
				{
					e.data.authDomain.scheme[0] = 0;
				}
				e.hdoc = (HDOC_HANDLE) doc;

				browser->TriggerBrowserEvent(&e);
			}

			return (URL_ERROR_IO);
		}

		case HTTPC_PAGE_MOVED:
		{
			URLDescriptor desc;

			// If we receive an HTTP redirect, parse the new URL and send a new GET request
			if (ctx->redirected_page)
			{
			  #if (WEBC_SUPPORT_UNICODE)
				WEBC_CHAR redirectedUrl[256];
				webc_c_strncpy(redirectedUrl, ctx->redirected_page, 255);
				redirectedUrl[255] = 0;
				desc.parseURL(redirectedUrl, pStream->p_desc);
			  #else
				desc.parseURL(ctx->redirected_page, pStream->p_desc);
			  #endif

				wload_Close(pStream);

				pStream->p_desc->set(&desc);
				if (wload_Get(pStream, pStream->p_desc, pStream->p_date) < 0)
				{
					//return (bytes_read);
					return (URL_ERROR_IO);
				}
			}
			else
			{
				wload_Close(pStream);
				return (URL_ERROR_IO);
			}

			return (URL_ERROR_REDIRECT);
		}

		case HTTPC_ENOTMODIFIED:
			wload_Close(pStream);
			return (URL_ERROR_NOT_MODIFIED);
		case HTTPC_PAGENOTFOUND:
			return (HTTPC_PAGENOTFOUND);
		case HTTPC_PAYMENTREQ:
			return (HTTPC_PAYMENTREQ);
		case HTTPC_FORBIDDEN:
			return (HTTPC_FORBIDDEN);
		default:
			if (bytes_read < 0)
			{
				return (URL_ERROR_IO);
			}
	}

	pStream->data_type = ctx->mime_data_type;
	return (bytes_read);
}

long wload_HttpWrite(UrlStreamCtx *pStream, char * buffer, long length)
{
	struct httpc_ctx *ctx = &pStream->stream.http.ctx;
	long bytes_sent = http_send(ctx, buffer, length);

	if (bytes_sent < 0)
	{
		return (URL_ERROR_IO);
	}

	return (bytes_sent);
}

WebcFileContentType wload_HttpDataType(UrlStreamCtx *pStream)
{
	return (pStream->data_type);
}

int wload_HttpClose(UrlStreamCtx *pStream)
{
	webc_http_close(&pStream->stream.http.ctx);
	return (0);
}

enum WebcCharset wload_HttpCharset(UrlStreamCtx *pStream)
{
	return (pStream->stream.http.ctx.mime_charset);
}

/*
void wload_HttpSetAuthCallback (HTTPAuthCallback fn, unsigned char *param)
{
	if (fn)
	{
		gHttpAuthCallback = fn;
	}
	else
	{
		gHttpAuthCallback = DefaultHttpAuthCallback;
	}
	gpHttpAuthCallbackParam = param;
}
*/

/*****************************************************************************/
/*

WEBC_READONLY char rHttpAuthDialogPart1 [] =
		"<html><body onload=\"document.f.user.focus()\">"
		"<input type=hidden name=\"url\" value=\"";

WEBC_READONLY char rHttpAuthDialogPart2 [] =
		"\"><input type=hidden name=\"realm\" value=\"";

WEBC_READONLY char rHttpAuthDialogPart3 [] =
		"\"><table align=center border=0 cellspacing=10><tr><td>"
		"<h1><img align=middle src=webc://prompt.png>&nbsp&nbsp"
		"Authorization Required</h1><form name=f><p><small>"
		"Please enter your username and password.</small>"
		"<p><table border=0 cellpadding=0 cellspacing=2><tr>"
		"<td align=right>username</td><td width=5>&nbsp</td>"
		"<td><input name=user type=text maxlength=36 size=36></td>"
		"<tr><tr><td align=right>password</td><td width=5>&nbsp</td>"
		"<td><input name=pass type=password maxlength=16 size=36>"
		"</td></tr><tr><td colspan=3 align=center><small>"
		"<br>"
		"<script> function toggle(b) {"
			"if (b)"
			"{"
				"window.ttlLabel.style.color = \'#000\';"
				"window.ttlMinutes.disabled = false;"
			"}"
			"else"
			"{"
				"window.ttlLabel.style.color = \'#888\';"
				"window.ttlMinutes.disabled = true;"
			"}"
		"} </script>"
		"<input type=checkbox name=\"ttlEnable\" "
		"onclick=\"toggle(!this.checked)\" onkeypress=\"toggle(this.checked)\">"
		"<span id=ttlLabel style=\"color:#888\">"
		"Discard this password after <input size=3 disabled type=text name=\"ttlMinutes\"> "
		"&nbsp;minutes</span><br><br></small></td></tr><tr><td colspan=3 align=right>"
		"<input eventhandler=\"SetAuth\" type=button value=\"OK\" style=\"width:80px;\">"
		"<input eventhandler=\"BackButton\" type=button value=\"Cancel\" style=\"width:80px;\">"
		"</td></tr></table>"
		"</td></tr></table></form>"
		"</body></html>";

enum HTTPAuthStatus DefaultHttpAuthCallback (struct HTTPAuthCallbackParam *param)
{
	unsigned long charsNeeded =
			sizeof(rHttpAuthDialogPart1) +
			sizeof(rHttpAuthDialogPart3) +
			tc_strlen(param->url) + 20;

	if (param->domain.realm)
	{
		charsNeeded += tc_strlen(param->domain.realm) + sizeof(rHttpAuthDialogPart2);
	}

	if ((unsigned) param->size > charsNeeded*sizeof(WEBC_CHAR))
	{
		WEBC_CHAR *htmlBuffer = (WEBC_CHAR *) param->data;

		webc_c_strcpy(htmlBuffer, rHttpAuthDialogPart1);
		webc_c_strcat(htmlBuffer, param->url);
		if (param->domain.realm)
		{
			webc_c_strcat(htmlBuffer, rHttpAuthDialogPart2);
			webc_c_strcat(htmlBuffer, param->domain.realm);
		}
		webc_c_strcat(htmlBuffer, rHttpAuthDialogPart3);

		param->mimeType    = HTML_MIME_TYPE_HTML;
		param->mimeCharset = WEBC_DEFAULT_CHARSET;
		param->size        = webc_strlen(htmlBuffer) * sizeof(WEBC_CHAR);
		return (HTTP_AUTH_STOP);
	}
	else
	{
		param->size = 0;
	}

	return (HTTP_AUTH_CONTINUE);
}
*/


HTTPStreamClass** webc_SocketStreamNew (HTTPStreamFactoryClass** factory)
{
	WEBC_ASSERT((*factory) == &gWebcSocketStreamFactoryClass);

	WebcSocketStreamFactory* f = (WebcSocketStreamFactory*) factory;
	WebcSocketStream* s = (WebcSocketStream*) WEBC_MALLOC(sizeof(WebcSocketStream));
	if (s)
	{
		if (webc_socket(&s->socket) >= 0)
		{
			s->base = (HTTPStreamClass*) &gWebcSocketStreamClass;
			s->idle_func = f->idle_func;
			s->idle_data = f->idle_data;
			return &s->base;
		}
		WEBC_FREE(s);
	}
	return 0;
}

HTTPStreamClass** webc_SocketStreamActivate (HTTPStreamFactoryClass** factory,
											 HTTPStreamClass** cachedStream)
{
	WEBC_ASSERT((*factory) == &gWebcSocketStreamFactoryClass);

	if (*cachedStream == &gWebcSocketStreamClass)
	{
		WebcSocketStreamFactory* f = (WebcSocketStreamFactory*) factory;
		WebcSocketStream* s = (WebcSocketStream*) cachedStream;

		s->idle_func = f->idle_func;
		s->idle_data = f->idle_data;

		return &s->base;
	}

	return 0;
}

int webc_SocketStreamGetHostAddr (HTTPStreamClass** instance,
                                  WEBC_UINT8* ipaddr,
                                  WEBC_CHAR8* name)
{
	return webc_gethostipaddr(ipaddr, name);
}

int webc_SocketStreamConnect (HTTPStreamClass** instance,
                              WEBC_UINT8* remoteAddr,
                              WEBC_UINT16 port)
{
	WebcSocketStream* s = (WebcSocketStream*) instance;
	return webc_connect(s->socket, remoteAddr, port);
}

WEBC_BOOL webc_SocketStreamIsConnected (HTTPStreamClass** instance)
{
	WebcSocketStream* s = (WebcSocketStream*) instance;
	return webc_socket_is_connected (s->socket);
}

WEBC_INT32 webc_SocketStreamSend (HTTPStreamClass** instance,
                                  const WEBC_UINT8* data,
                                  WEBC_INT32 length)
{
	WebcSocketStream* s = (WebcSocketStream*) instance;
	return webc_send (s->socket, (char*) data, length, s->idle_func, s->idle_data);
}

WEBC_INT32 webc_SocketStreamRecv (HTTPStreamClass** instance,
                                  WEBC_UINT8* data,
                                  WEBC_INT32 minBytes,
                                  WEBC_INT32 maxBytes)
{
	WebcSocketStream* s = (WebcSocketStream*) instance;
	return webc_recv_at_least (s->socket, (char*) data, minBytes, maxBytes, s->idle_func, s->idle_data);
}

WEBC_BOOL webc_SocketStreamCache (HTTPStreamClass** instance)
{
	WebcSocketStream* s = (WebcSocketStream*) instance;

	s->idle_func = 0;
	s->idle_data = 0;

	return WEBC_TRUE;
}

void webc_SocketStreamClose (HTTPStreamClass** instance)
{
	WebcSocketStream* s = (WebcSocketStream*) instance;
	webc_closesocket(s->socket);
	WEBC_FREE(s);
}

#if (WEBC_SUPPORT_HTTP_CACHE)

int httpc_init_cache_storage (const struct wcache_system_spec** spec, void** instance)
{
  #ifdef WEBC_HTTP_USE_RAM_CACHE
	if (!gHttpRamDisk)
	{
	  	WEBC_NEW(gHttpRamDisk, SimpleRamDisk());
	}

	if (gHttpRamDisk)
	{
		*spec = &gWebcRamCacheSpec;
		*instance = (void*) gHttpRamDisk;
		return 0;
	}

	return -1;

  #else

	*spec = &gWebcFileCacheSpec;
	*instance = 0;
	return 0;

  #endif
}

void httpc_destroy_cache_storage (void)
{
  #ifdef WEBC_HTTP_USE_RAM_CACHE
	if (gHttpRamDisk)
	{
		WEBC_DELETE(gHttpRamDisk);
		gHttpRamDisk = 0;
	}
  #endif
}

#endif // WEBC_SUPPORT_HTTP_CACHE

#endif // WEBC_SUPPORT_HTTP
