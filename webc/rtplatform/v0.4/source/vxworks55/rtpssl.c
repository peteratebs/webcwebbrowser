 /*
 | RTPSSL.C - Runtime Platform secure socket layer
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: manish $
 |  $Date: 2006/10/17 06:08:51 $
 |  $Name:  $
 |  $Revision: 1.1 $
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

/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                           rtp_ssl_init_context
 *----------------------------------------------------------------------*/
/** @memo   Initialization for a secure client or server.

    @doc    Initializes a secure client or server context and
    returns it via the RTP_SSL_CONTEXT* sslContext. This returned 
    value should remain untouched outside of RTSSL and passed 
    on to the other rtp_ssl calls that need it. When the server
    or client context is no longer needed by the application,
    a call to rtp_ssl_close_context should be made to free any
    memory that was allocated by this call.

    @return 0 if successful, -1 otherwise.
 */
int rtp_ssl_init_context (
		RTP_SSL_CONTEXT* sslContext,    /** Pointer to the transparent SSL library context. */
		unsigned int sslMode,           /** MUST be an or'ed combination of the following:<br>
	<pre>
|		RTP_SSL_MODE_SERVER    to initialize a server context
|		RTP_SSL_MODE_CLIENT    to initialize a client context
|		RTP_SSL_MODE_SSL2      to enable SSLv2
|		RTP_SSL_MODE_SSL3      to enable SSLv3
|		RTP_SSL_MODE_TLS1      to enable TLSv1
	</pre> */
		unsigned int verifyMode         /** MUST be an or'ed combination of the following:<br>
	<pre>
|		RTP_SSL_VERIFY_NONE                   don't verify peer, even if a certificate is.
|		                                      passed and fails verification, the session
|		                                      continues.
|		RTP_SSL_VERIFY_PEER                   (Client) if verifies the peer, and fails if 
|		                                      no certificate is sent.
|		                                      (Server) verifies the peer only if a 
|		                                      certificate is sent.
|		RTP_SSL_VERIFY_FAIL_IF_NO_PEER_CERT   demands a certificate from the peer.
|		RTP_SSL_VERIFY_CLIENT_ONCE            (Server ONLY) validate a clients certificate
|		                                      once; doesn't bother checking it again on a 
|		                                      renegotiation.
	</pre> */
	)
{
    rtp_not_yet_implemented ();
	return (-1);
}

/*----------------------------------------------------------------------*
                            rtp_ssl_connect
 *----------------------------------------------------------------------*/
/** @memo   Establish a secure connection.

    @doc    Establishes a connection on the pre-established
    socket and forms an SSL session object based on this
    connection. This SSL session is passed back using the 
    RTP_SSL_SESSION* sslStream, and MUST remain untouched by code 
    outside the RTSSL.
    
    @precondition A 'clear-text' connect should be successfully
    done before calling this function.

    @return 0 if successful, -1 otherwise.
 */

#if 0 && defined(BAS)
int rtp_ssl_connect (
		RTP_SSL_SESSION *sslStream,     /** Pointer to the transparent SSL session object. */
		RTP_HANDLE sockHandle,          /** The socket to communicate over. */
		RTP_SSL_CONTEXT sslContext      /** The transparent SSL context returned by rtp_ssl_init_context. */
	)
#endif

int  rtp_ssl_connect (RTP_SSL_SESSION sslStream)
{
    rtp_not_yet_implemented ();
	return (-1);
}

/*----------------------------------------------------------------------*
                              rtp_ssl_accept
 *----------------------------------------------------------------------*/
/** @memo   Accepts a secure connection.

    @doc    Accepts a connection on the pre-established
    socket and forms an SSL session object based on this
    connection. This SSL session is passed back using the 
    RTP_SSL_SESSION* sslStream, and MUST remain untouched by code 
    outside the RTSSL.
    
    @precondition A 'clear-text' accept should be successfully
    done before calling this function.

    @return 0 if successful, -1 otherwise.
 */
#if 0 && defined(BAS)
int rtp_ssl_accept (
		RTP_SSL_SESSION *sslStream,     /** Pointer to the transparent SSL session object. */
		RTP_HANDLE sockHandle,          /** The socket to communicate over. */
		RTP_SSL_CONTEXT sslContext      /** The transparent SSL context returned by rtp_ssl_init_context. */
	)
#endif

int  rtp_ssl_accept(RTP_SSL_SESSION sslStream)
{
    rtp_not_yet_implemented ();
	return (-1);
}

/*----------------------------------------------------------------------*
                             rtp_ssl_send
 *----------------------------------------------------------------------*/
/** @memo   Send data over a secured socket.

    @doc    Uses the pre-established socket now stored in the 
    SSL session object returned from the successful secure 
    accept or successful secure connect to send data while
    transparently encrypting the data so that it can be sent 
    securely. This call takes place of the 'clear-text' send.

    @return Number of bytes sent, -1 otherwise.
 */
#if 0 && defined(BAS)
long rtp_ssl_send (
		RTP_SSL_SESSION sslStream,      /** The transparent SSL session object returned by 
		                                    rtp_ssl_connect or rtp_ssl_accept. */
		unsigned char * buffer,         /** The buffer containing data to be sent securely. */
		long len,                       /** Length of the data buffer. */
		unsigned int boolBlocking       /** 0 if the socket associated with the SSL session
		                                    object is nonblocking, 1 if it is in blocking mode. */
	)
#endif

long rtp_ssl_send(RTP_SSL_SESSION sslStream, const unsigned char* buffer, long len, unsigned int boolBlocking)
{
    rtp_not_yet_implemented ();
	return (-1);
}

/*----------------------------------------------------------------------*
                             rtp_ssl_recv
 *----------------------------------------------------------------------*/
/** @memo   Receive data over a secured socket.

    @doc    Uses the pre-established socket now stored in the 
    SSL session object returned from the successful secure 
    accept or successful secure connect to receive data while
    transparently decrypting the data so that it can be 
    received. This call takes place of the 'clear-text' recv.

    @return Number of bytes received, -1 otherwise.
 */
long rtp_ssl_recv (
		RTP_SSL_SESSION sslStream,      /** The transparent SSL session object returned by 
		                                    rtp_ssl_connect or rtp_ssl_accept. */
		unsigned char * buffer,         /** The buffer to store the securely received data. */
		long len,                       /** Length of the data buffer. */
		unsigned int boolBlocking       /** 0 if the socket associated with the SSL session
		                                    object is nonblocking, 1 if it is in blocking mode. */
	)
{
    rtp_not_yet_implemented ();
	return (-1);
}

/*----------------------------------------------------------------------*
                         rtp_ssl_close_stream
 *----------------------------------------------------------------------*/
/** @memo   Close a secure session and free its associated object.

    @doc    Shuts down an SSL session, then frees the SSL
    session object. MUST be called before the socket that the 
    SSL session is being ran over is closed. MUST be called
    once for every successful rtp_ssl_connect or rtp_ssl_accept
    call.

    @return 0 if successful, -1 otherwise.
 */
int rtp_ssl_close_stream (
		RTP_SSL_SESSION sslStream       /** The transparent SSL session object returned by 
		                                    rtp_ssl_connect or rtp_ssl_accept. */
	)
{
    rtp_not_yet_implemented ();
	return (-1);
}

/*----------------------------------------------------------------------*
                          rtp_ssl_close_context
 *----------------------------------------------------------------------*/
/** @memo   Close/Free an SSL context.

    @doc    Frees the current SSL context passed in, and
    if it is the last SSL context owned by the application
    this function will also free up any SSL library data 
    setup by the rtp_ssl_init_context. MUST be called once
    for every SSL context initialized.
 */
void rtp_ssl_close_context (
		RTP_SSL_CONTEXT sslContext      /** The transparent SSL context returned by rtp_ssl_init_context. */
	)
{
    rtp_not_yet_implemented ();
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
