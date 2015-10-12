 /*
 | RTPSSL.C - Runtime Platform secure socket layer
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:31 $
 |  $Name:  $
 |  $Revision: 1.2 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/

/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"
#include "rtpssl.h"
//#include "apirtssl.h"

/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/
static int sslContextCount = 0;

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/

int _rtp_ssl_to_rtssl_mode (unsigned int sslMode);
int _rtp_ssl_to_rtssl_verify (unsigned int verifyMode);

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                           rtp_ssl_init_context
 *----------------------------------------------------------------------*/
int rtp_ssl_init_context (
		RTP_SSL_CONTEXT* sslContext, 
		unsigned int sslMode, 
		unsigned int verifyMode
	)
{
	int rtsslmode = _rtp_ssl_to_rtssl_mode(sslMode);
	int rtsslverify = _rtp_ssl_to_rtssl_verify(verifyMode);

	if (sslMode & RTP_SSL_MODE_SERVER)
	{
		*sslContext = (RTP_HANDLE) secure_server_init(rtsslmode, rtsslverify);
	}
	else if (sslMode & RTP_SSL_MODE_CLIENT)
	{
		*sslContext = (RTP_HANDLE) secure_client_init(rtsslmode, rtsslverify);
	}

	if (*sslContext)
	{
		sslContextCount++;
		return (0);
	}
	
	return (-1);
}

/*----------------------------------------------------------------------*
                            rtp_ssl_connect
 *----------------------------------------------------------------------*/
int rtp_ssl_connect (
		RTP_SSL_SESSION *sslStream, 
		RTP_HANDLE sockHandle, 
		RTP_SSL_CONTEXT sslContext
	)
{
	return (secure_connect (sockHandle, (void *) sslContext, (void**) sslStream));
}

/*----------------------------------------------------------------------*
                              rtp_ssl_accept
 *----------------------------------------------------------------------*/
int rtp_ssl_accept (
		RTP_SSL_SESSION *sslStream, 
		RTP_HANDLE sockHandle, 
		RTP_SSL_CONTEXT sslContext
	)
{
	return (secure_accept (sockHandle, (void*) sslContext, (void**) sslStream));
}

/*----------------------------------------------------------------------*
                             rtp_ssl_send
 *----------------------------------------------------------------------*/
long rtp_ssl_send (
		RTP_SSL_SESSION sslStream, 
		const unsigned char * buffer, 
		long len, 
		unsigned int boolBlocking
	)
{
	return (secure_write ((void*) sslStream, (unsigned char*) buffer, len, boolBlocking));
}

/*----------------------------------------------------------------------*
                             rtp_ssl_recv
 *----------------------------------------------------------------------*/
long rtp_ssl_recv (
		RTP_SSL_SESSION sslStream, 
		unsigned char * buffer, 
		long len, 
		unsigned int boolBlocking
	)
{
	return (secure_read ((void*) sslStream, buffer, len, boolBlocking));
}

/*----------------------------------------------------------------------*
                         rtp_ssl_close_stream
 *----------------------------------------------------------------------*/
int rtp_ssl_close_stream (
		RTP_SSL_SESSION sslStream
	)
{
	if (sslStream)
    {
    	secure_sslshutdown ((void*)sslStream);
		secure_sslfree ((void*)sslStream);
		return (0);
    }
	return (-1);
}

/*----------------------------------------------------------------------*
                          rtp_ssl_close_context
 *----------------------------------------------------------------------*/
void rtp_ssl_close_context (
		RTP_SSL_CONTEXT sslContext
	)
{
	sslContextCount--;
	if (sslContextCount > 0)
	{
		secure_sslctxfree ((void*)sslContext);
	}
	else
	{
		secure_sslexit ((void*)sslContext);
	}
}

/*----------------------------------------------------------------------*/
int _rtp_ssl_to_rtssl_mode (unsigned int sslMode)
{
unsigned int rtsslMode = 0;
	
	rtsslMode |= (sslMode & RTP_SSL_MODE_SERVER) ? RTSSL_SECURE_SERVER : 0;
	rtsslMode |= (sslMode & RTP_SSL_MODE_CLIENT) ? RTSSL_SECURE_CLIENT : 0;
	rtsslMode |= (sslMode & RTP_SSL_MODE_SSL2)   ? ENABLE_RTSSLv2      : 0;
	rtsslMode |= (sslMode & RTP_SSL_MODE_SSL3)   ? ENABLE_RTSSLv3      : 0;
	rtsslMode |= (sslMode & RTP_SSL_MODE_TLS1)   ? ENABLE_RTTLSv1      : 0;
	
	return (rtsslMode);
}

/*----------------------------------------------------------------------*/
int _rtp_ssl_to_rtssl_verify (unsigned int verifyMode)
{
unsigned int rtsslVerifyMode = 0;

	rtsslVerifyMode |= (verifyMode & RTP_SSL_VERIFY_NONE)                 ? RTSSL_VERIFY_NONE                 : 0;
	rtsslVerifyMode |= (verifyMode & RTP_SSL_VERIFY_PEER)                 ? RTSSL_VERIFY_PEER                 : 0;
	rtsslVerifyMode |= (verifyMode & RTP_SSL_VERIFY_FAIL_IF_NO_PEER_CERT) ? RTSSL_VERIFY_FAIL_IF_NO_PEER_CERT : 0;
	rtsslVerifyMode |= (verifyMode & RTP_SSL_VERIFY_CLIENT_ONCE)          ? RTSSL_VERIFY_CLIENT_ONCE          : 0;
	
	return (rtsslVerifyMode);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */

