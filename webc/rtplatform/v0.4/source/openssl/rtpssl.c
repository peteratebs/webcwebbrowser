 /*
 | RTPSSL.C - Runtime Platform secure socket layer
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: tony $
 |  $Date: 2005/07/12 15:41:15 $
 |  $Name:  $
 |  $Revision: 1.3 $
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
#include "rtptime.h"
#include "rtpmem.h"
#include <openssl/ssl.h>
#include <openssl/rand.h>

extern int rtp_net_get_last_error(void);
/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/
typedef struct s_OpenSSLSession
{
	BIO* bufferedIO;
	SSL* sslObject;
}
OpenSSLSession;

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

//int _rtp_ssl_to_rtssl_mode (unsigned int sslMode);
//int _rtp_ssl_to_rtssl_verify (unsigned int verifyMode);
int _rtp_ssl_translate_status(int ret);

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
	long systemMsec[2];
	SSL_METHOD *method;
	
	systemMsec[0] = rtp_get_system_msec();
	
	// first initialize the library
	if (!sslContextCount)
	{
		SSL_library_init();
	}
	
	systemMsec[1] = rtp_get_system_msec();
	
	// seed the random number generator - the user may can add more/better sources of entropy
	RAND_seed(systemMsec, sizeof(systemMsec));

	// select the mode in which SSL will operate (client/server & protcols)
	switch (sslMode)
	{
		case RTP_SSL_MODE_SERVER:
			method = SSLv23_server_method();
			break;

		case RTP_SSL_MODE_CLIENT:
			method = SSLv23_client_method();
			break;

		default:
			method = SSLv23_method();
			break;
	}

	if (method)
	{
		*sslContext = (RTP_SSL_CONTEXT) SSL_CTX_new(method);
		if (*sslContext)
		{
			return 0;
		}
	}

	return -1;
}

/*----------------------------------------------------------------------*
                   rtp_ssl_connect, rtp_ssl_connect_retry
                            
	return values
		 0 = success; secure connection complete
		-1 = non-recoverable failure; sslStream is not valid
		-2 = connect in progress; waiting to read  - call rtp_ssl_connect_retry
		-3 = connect in progress; waiting to write - call rtp_ssl_connect_retry
 *----------------------------------------------------------------------*/
int rtp_ssl_init_client (
		RTP_SSL_SESSION *sslStream, 
		RTP_HANDLE sockHandle, 
		RTP_SSL_CONTEXT sslContext
	)
{
	OpenSSLSession* s = (OpenSSLSession*) rtp_malloc(sizeof(OpenSSLSession));
	if (s)
	{
		/*      make an SSL structure   */
		s->sslObject = SSL_new((SSL_CTX*) sslContext);
		if (s->sslObject)
		{
			s->bufferedIO = BIO_new_socket((int) sockHandle, BIO_NOCLOSE);
			if (s->bufferedIO)
			{
				SSL_set_bio(s->sslObject, s->bufferedIO, s->bufferedIO);
				SSL_set_connect_state(s->sslObject);						
				
				*sslStream = (RTP_SSL_SESSION) s;
				
				return 0;
			}
			
			// failure; shutdown & free the ssl object
			SSL_shutdown(s->sslObject);  // sends the "close notify" and sets 
			                             //  SSL_SENT_SHUTDOWN flag regardless 
			                             //  of outcome
			SSL_free(s->sslObject);

		}
		rtp_free(s);
	}
	return -1;
}

#if (0)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif
int rtp_ssl_connect (
		RTP_SSL_SESSION sslStream	
	)
{
	int tryAgain;
	int result;

	OpenSSLSession* s = (OpenSSLSession*) sslStream;
	do
	{
		tryAgain = 0;
		result = SSL_connect(s->sslObject);             // initiates the handshake

		switch (SSL_get_error(s->sslObject, result))
		{
			case SSL_ERROR_WANT_READ:
				return RTP_SSL_ERR_WANT_READ;

			case SSL_ERROR_WANT_WRITE:
				return RTP_SSL_ERR_WANT_WRITE;

			case SSL_ERROR_SYSCALL:
				if (result == 0)
				{
					tryAgain = 1;
				}

				result = (long) WSAGetLastError(); // rtp_net_get_last_error(); //  WSAGetLastError();
				if ((result == WSAEINPROGRESS) || (result == WSAEWOULDBLOCK))
				{
					return (-2);
				}
				// intentional fall-through			
					
			case SSL_ERROR_SSL:
			default:
				break;
		}
	}
	while (tryAgain);						

	if (result == 1)
	{
		return RTP_SSL_ERR_NONE;
	}

	return RTP_SSL_ERR_FAILED;
}

/*----------------------------------------------------------------------*
                              rtp_ssl_accept
 *----------------------------------------------------------------------*/
int rtp_ssl_accept (
		RTP_SSL_SESSION sslStream
	)
{
	return -1;
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
	OpenSSLSession* s = (OpenSSLSession*) sslStream;
	int tryAgain;
	int result;
	
	do
	{
		tryAgain = 0;
		result = SSL_write(s->sslObject, buffer, len);             // initiates the handshake

		switch (SSL_get_error(s->sslObject, result))
		{
			case SSL_ERROR_NONE:
				break;
				
			case SSL_ERROR_WANT_READ:
				return RTP_SSL_ERR_WANT_READ;

			case SSL_ERROR_WANT_WRITE:
				return RTP_SSL_ERR_WANT_WRITE;

			case SSL_ERROR_ZERO_RETURN:
				return 0;

			case SSL_ERROR_SYSCALL:
				if (result == 0)
				{
					tryAgain = 1;
				}
				break;
					
			case SSL_ERROR_SSL:
			default:
				return RTP_SSL_ERR_FAILED;
		}
	}
	while (tryAgain);		
	
	return result;
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
	OpenSSLSession* s = (OpenSSLSession*) sslStream;
	int tryAgain;
	int result;
	int err;
	
	do
	{
		tryAgain = 0;
		result = SSL_read(s->sslObject, buffer, len);             // initiates the handshake

		switch ((err = SSL_get_error(s->sslObject, result)))
		{
			case SSL_ERROR_NONE:
				break;
				
			case SSL_ERROR_WANT_READ:
				return RTP_SSL_ERR_WANT_READ;

			case SSL_ERROR_WANT_WRITE:
				return RTP_SSL_ERR_WANT_WRITE;

			case SSL_ERROR_ZERO_RETURN:
				return 0;

			case SSL_ERROR_SYSCALL:
				if (result == 0)
				{
					tryAgain = 1;
				}
				// intentional fall-through			
					
			case SSL_ERROR_SSL:
			default:
				return RTP_SSL_ERR_FAILED;
		}
	}
	while (tryAgain);		
	
	return result;
}

/*----------------------------------------------------------------------*
                         rtp_ssl_close_stream
 *----------------------------------------------------------------------*/
int rtp_ssl_close_session (
		RTP_SSL_SESSION sslStream
	)
{
	OpenSSLSession* s = (OpenSSLSession*) sslStream;

	// failure; shutdown & free the ssl object
	SSL_shutdown(s->sslObject); // sends the "close notify" and sets 
								//  SSL_SENT_SHUTDOWN flag regardless 
								//  of outcome
	SSL_free(s->sslObject);

	rtp_free(s);	
	
	return 0;
}

/*----------------------------------------------------------------------*
                          rtp_ssl_close_context
 *----------------------------------------------------------------------*/
void rtp_ssl_close_context (
		RTP_SSL_CONTEXT sslContext
	)
{
}

#if 0
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
#endif

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */

