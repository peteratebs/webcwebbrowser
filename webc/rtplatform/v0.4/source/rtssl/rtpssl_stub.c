//
// RTPSSL.C - Runtime Platform secure socket layer
//
// EBSnet - RT-Platform
//
//  $Author: shane $
//  $Date: 2004/07/12 18:01:23 $
//  $Name:  $
//  $Revision: 1.5 $
//
// Copyright EBSnet Inc. , 2003
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"
#include "rtpssl.h"


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
		return (0);
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
	return (0);
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
	return (0);
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
	return (0);
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
	return (0);
}

/*----------------------------------------------------------------------*
                         rtp_ssl_close_stream
 *----------------------------------------------------------------------*/
int rtp_ssl_close_stream (
		RTP_SSL_SESSION sslStream
	)
{
	return (0);
}

/*----------------------------------------------------------------------*
                          rtp_ssl_close_context
 *----------------------------------------------------------------------*/
void rtp_ssl_close_context (
		RTP_SSL_CONTEXT sslContext
	)
{
	return (0);
}

/*----------------------------------------------------------------------*/
int _rtp_ssl_to_rtssl_mode (unsigned int sslMode)
{
return (0);
}

/*----------------------------------------------------------------------*/
int _rtp_ssl_to_rtssl_verify (unsigned int verifyMode)
{
return (0);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */

