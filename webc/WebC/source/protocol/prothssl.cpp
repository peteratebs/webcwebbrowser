//
// PROTHSSL.C - Routines to load URL using Secure HTTP (HTTPS) protocol
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
#include "httpc.h"
#include "webcsock.h"
#include "prothssl.h"
#include "webcstr.h"
#include "webcmem.h"
#include "webcassert.h"
#include "rtptime.h"
#include "rtpssl.h"

/*****************************************************************************/
// Local types
/*****************************************************************************/

/*****************************************************************************/
// Local functions
/*****************************************************************************/

HTTPStreamClass**   webc_SSLStreamNew         (HTTPStreamFactoryClass** factory);

HTTPStreamClass**   webc_SSLStreamActivate    (HTTPStreamFactoryClass** factory,
											   HTTPStreamClass** cachedStream);

int                 webc_SSLStreamGetHostAddr (HTTPStreamClass** instance,
                                               WEBC_UINT8* ipaddr,
                                               WEBC_CHAR8* name);

int                 webc_SSLStreamConnect     (HTTPStreamClass** instance,
                                               WEBC_UINT8* remoteAddr,
                                               WEBC_UINT16 port);

WEBC_BOOL           webc_SSLStreamIsConnected (HTTPStreamClass** instance);

WEBC_INT32          webc_SSLStreamSend        (HTTPStreamClass** instance,
                                               const WEBC_UINT8* data,
                                               WEBC_INT32 length);

WEBC_INT32          webc_SSLStreamRecv        (HTTPStreamClass** instance,
                                               WEBC_UINT8* data,
                                               WEBC_INT32 minBytes,
                                               WEBC_INT32 maxBytes);

WEBC_BOOL           webc_SSLStreamCache       (HTTPStreamClass** instance);

void                webc_SSLStreamClose       (HTTPStreamClass** instance);

/*****************************************************************************/
// Data
/*****************************************************************************/
#if (WEBC_SUPPORT_HTTPS)

WEBC_BOOL gWebcSSLInitialized = WEBC_FALSE;
RTP_SSL_CONTEXT gWebcSSLContext;

WEBC_READONLY HTTPStreamClass gWebcSSLStreamClass =
{
	webc_SSLStreamGetHostAddr,
	webc_SSLStreamConnect,
	webc_SSLStreamIsConnected,
	webc_SSLStreamSend,
	webc_SSLStreamRecv,
	webc_SSLStreamCache,
	webc_SSLStreamClose
};

WEBC_READONLY HTTPStreamFactoryClass gWebcSSLStreamFactoryClass =
{
	webc_SSLStreamNew,
	webc_SSLStreamActivate
};
#endif
/*****************************************************************************/
// Function Bodies
/*****************************************************************************/

int wload_HttpsGet(UrlStreamCtx *pStream)
{
#if (WEBC_SUPPORT_HTTPS)
int result = -1;
char *path = webc_arg_to_char(pStream->p_desc->getPath());
char *host = webc_arg_to_char(pStream->p_desc->getHost());
struct httpc_ctx *ctx = &pStream->stream.https.ctx;
	
	webc_network_init();
	
	if (!gWebcSSLInitialized)
	{
		if (rtp_ssl_init_context(&gWebcSSLContext, RTP_SSL_MODE_CLIENT, RTP_SSL_VERIFY_NONE) < 0)
		{
			return -1;
		}
		gWebcSSLInitialized = WEBC_TRUE;
	}

	pStream->stream.https.stop = 0;
	pStream->stream.https.factory.base = (HTTPStreamFactoryClass*) &gWebcSSLStreamFactoryClass;
	pStream->stream.https.factory.idle_func = pStream->idle_func;
	pStream->stream.https.factory.idle_data = pStream->idle_data;

	webc_http_init(ctx, &pStream->stream.https.factory.base);
	http_set_report_fn(ctx, pStream->report_func, pStream->report_ctx);
	http_set_secure(ctx, 1);

	if (path && host)
	{
		result = http_get(ctx, path, host, pStream->p_desc->getPort(), pStream->p_date,0);
	}
			
	webc_arg_done(host);
	webc_arg_done(path);

	return (result);
#else
	return (-1);
#endif	
}

int wload_HttpsPost(UrlStreamCtx *pStream, WEBC_CHAR * pContentType, long lContentLength)
{
#if (WEBC_SUPPORT_HTTPS)
int result = -1;
char *path = webc_arg_to_char(pStream->p_desc->getPath());
char *host = webc_arg_to_char(pStream->p_desc->getHost());
char *type = webc_arg_to_char(pContentType);
struct httpc_ctx *ctx = &pStream->stream.http.ctx;

	webc_network_init();

	if (!gWebcSSLInitialized)
	{
		if (rtp_ssl_init_context(&gWebcSSLContext, RTP_SSL_MODE_CLIENT, RTP_SSL_VERIFY_NONE) < 0)
		{
			return -1;
		}
		gWebcSSLInitialized = WEBC_TRUE;
	}

	pStream->stream.https.stop = 0;
	pStream->stream.https.factory.base = (HTTPStreamFactoryClass*) &gWebcSSLStreamFactoryClass;
	pStream->stream.https.factory.idle_func = pStream->idle_func;
	pStream->stream.https.factory.idle_data = pStream->idle_data;

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
#else
	return (-1);
#endif	
}

#if (WEBC_SUPPORT_HTTPS)

HTTPStreamClass** webc_SSLStreamNew (HTTPStreamFactoryClass** factory)
{
	WEBC_ASSERT((*factory) == &gWebcSSLStreamFactoryClass);

	WebcSSLStreamFactory* f = (WebcSSLStreamFactory*) factory;
	WebcSSLStream* s = (WebcSSLStream*) WEBC_MALLOC(sizeof(WebcSSLStream));
	if (s)
	{
		s->created = WEBC_FALSE;
		if (webc_socket(&s->socket) >= 0)
		{
			if (rtp_ssl_init_client(&s->ssl, s->socket, gWebcSSLContext) >= 0)
			{
				s->base = (HTTPStreamClass*) &gWebcSSLStreamClass;
				s->created = WEBC_TRUE;
				s->idle_func = f->idle_func;
				s->idle_data = f->idle_data;
				return &s->base;
			}
			webc_closesocket(s->socket);
		}
		WEBC_FREE(s);
	}
	return 0;
}

HTTPStreamClass** webc_SSLStreamActivate (HTTPStreamFactoryClass** factory,
											 HTTPStreamClass** cachedStream)
{
	WEBC_ASSERT((*factory) == &gWebcSSLStreamFactoryClass);
	
	if (*cachedStream == &gWebcSSLStreamClass)
	{
		WebcSSLStreamFactory* f = (WebcSSLStreamFactory*) factory;
		WebcSSLStream* s = (WebcSSLStream*) cachedStream;
		
		s->idle_func = f->idle_func;
		s->idle_data = f->idle_data;

		return &s->base;
	}

	return 0;
}

int webc_SSLStreamGetHostAddr (HTTPStreamClass** instance,
                                  WEBC_UINT8* ipaddr,
                                  WEBC_CHAR8* name)
{
	return webc_gethostipaddr(ipaddr, name);
}

int webc_SSLStreamConnect (HTTPStreamClass** instance,
                           WEBC_UINT8* remoteAddr,
                           WEBC_UINT16 port)
{
	WebcSSLStream* s = (WebcSSLStream*) instance;

	if (webc_connect(s->socket, remoteAddr, port) >= 0)
	{	
		int ret;
		long timeoutInterval = rtp_get_system_msec() + (WEBC_TIMEOUT_SEC * 1000);

		if (webc_poll_socket(s->socket, s->idle_func, s->idle_data, timeoutInterval, WEBC_POLL_WRITE) != WEBC_POLL_RESULT_READY)
		{
			return -1;
		}

		do
		{
			ret = rtp_ssl_connect(s->ssl);

			switch (ret)
			{
				case RTP_SSL_ERR_FAILED:
					return -1;
				
				case RTP_SSL_ERR_WANT_READ:
					if (webc_poll_socket(s->socket, s->idle_func, s->idle_data, timeoutInterval, WEBC_POLL_READ) != WEBC_POLL_RESULT_READY)
					{
						return -1;
					}
					break;
					
				case RTP_SSL_ERR_WANT_WRITE:
					if (webc_poll_socket(s->socket, s->idle_func, s->idle_data, timeoutInterval, WEBC_POLL_WRITE) != WEBC_POLL_RESULT_READY)
					{
						return -1;
					}
					break;
			}
		}
		while (ret != RTP_SSL_ERR_NONE);
		
		return 0;
	}
	
	return -1;
}

WEBC_BOOL webc_SSLStreamIsConnected (HTTPStreamClass** instance)
{
	WebcSSLStream* s = (WebcSSLStream*) instance;
	return webc_socket_is_connected (s->socket);	
}

WEBC_INT32 webc_SSLStreamSend (HTTPStreamClass** instance,
                                  const WEBC_UINT8* data,
                                  WEBC_INT32 length)
{
	WebcSSLStream* s = (WebcSSLStream*) instance;
	return webc_s_send (s->socket, (char*) data, length, s->idle_func, s->idle_data, s->ssl);	
}

WEBC_INT32 webc_SSLStreamRecv (HTTPStreamClass** instance,
                                  WEBC_UINT8* data,
                                  WEBC_INT32 minBytes,
                                  WEBC_INT32 maxBytes)
{
	WebcSSLStream* s = (WebcSSLStream*) instance;
	return webc_s_recv_at_least (s->socket, (char*) data, minBytes, maxBytes, s->idle_func, s->idle_data, s->ssl);	
}

WEBC_BOOL webc_SSLStreamCache (HTTPStreamClass** instance)
{
	WebcSSLStream* s = (WebcSSLStream*) instance;
	
	s->idle_func = 0;
	s->idle_data = 0;

	return WEBC_TRUE;
}

void webc_SSLStreamClose (HTTPStreamClass** instance)
{
	WebcSSLStream* s = (WebcSSLStream*) instance;
	if (s->created)
	{
		rtp_ssl_close_session(s->ssl);
		webc_closesocket(s->socket);
	}
	WEBC_FREE(s);
}

#endif // WEBC_SUPPORT_HTTPS
