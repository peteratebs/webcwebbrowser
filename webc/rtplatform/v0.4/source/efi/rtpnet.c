 /*
 | RTPNET.C - Runtime Platform Network Services
 |
 |   PORTED TO INTEL EFI PLATFORM
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/14 18:33:37 $
 |  $Name:  $
 |  $Revision: 1.6 $
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
#include "rtpdebug.h"

#include "./libsocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>


/************************************************************************
* Defines
************************************************************************/
#define RTP_NET_TYPE_IPV4   4   /* DO NOT CHANGE */
#define RTP_NET_TYPE_IPV6   6   /* DO NOT CHANGE */

/************************************************************************
* Externs
************************************************************************/

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------
 rtp_net_init - Initialize network API
----------------------------------------------------------------------*/

int rtp_net_init (void)
{
	rtp_not_yet_implemented();
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_exit - Close network API
----------------------------------------------------------------------*/

void rtp_net_exit (void)
{
    rtp_not_yet_implemented();
}


/*----------------------------------------------------------------------
 rtp_net_socket_stream - Create stream type (TCP) socket
 
    sockHandle  - pointer to an RTP_HANDLE to store the allocated socket
    
 Description

    Allocates, if possible, a stream (TCP, connection-based) type socket.  If 
    return value is negative, the value of *sockHandle is undefined.

 Returns
 
    0 if successful, -1 otherwise
----------------------------------------------------------------------*/

int rtp_net_socket_stream (RTP_HANDLE  *sockHandle)
{
    int sock;
    
#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif
    
    sock = socket(PF_INET, SOCK_STREAM, 0);
    
    if (sock < 0)
    {
        *sockHandle = ((RTP_HANDLE)-1);
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_socket_stream: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *sockHandle = (RTP_HANDLE) sock;
    
    return (0);   
}

/*----------------------------------------------------------------------
 rtp_net_socket_datagram - Create datagram type (UDP) socket
 
    sockHandle  - pointer to an RTP_HANDLE to store the allocated socket
    
 Description

    Allocates, if possible, a datagram (UDP, connectionless) type socket.  If 
    return value is negative, the value of *socketId is undefined.

 Returns
 
    0 if successful, -1 otherwise
----------------------------------------------------------------------*/

int rtp_net_socket_datagram (RTP_HANDLE  *sockHandle)
{
    int sock;
    
#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif
    
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    
    if (sock < 0)
    {
        *sockHandle = ((RTP_HANDLE)-1);
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_socket_datagram: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *sockHandle = (RTP_HANDLE) sock;
    
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_bind - Bind a socket to a port/address
 
    sockHandle - contains the port to bind to
    ipAddr     - (optional) the ip address to associate with this socket
    port       - port in host byte order
    type       - type of ip address
                 4 indicates an IPv4 address
                 6 indicates an IPv6 address

 Description
 
    This function should be called on a socket before calling 
    rtp_net_listen/rtp_net_accept (if the socket is stream-type), 
    or rtp_net_recvfrom (if the socket is datagram-type).  The behavior 
    of this function is undefined if sockHandle is not a valid socket handle.
    
    If the port specified is already in use by another socket, this function
    must return RTP_EBINDFAILED.

 See Also

    rtp_net_accept, rtp_net_listen, rtp_net_socket_stream
        
 Returns
 
    0 on success, -1 on failure
----------------------------------------------------------------------*/

int rtp_net_bind (RTP_HANDLE sockHandle, unsigned char *ipAddr, int port, int type)
{
    int sinLen;
    struct sockaddr_in sin;
    unsigned long in_addr = 0;
    
    sinLen = sizeof (sin);
    memset(&sin, 0, sinLen);
    
    if (ipAddr)
    {
        unsigned char *ptr = (unsigned char *) &in_addr;

        if (type == RTP_NET_TYPE_IPV4)
        {
            ptr[0] = ipAddr[0];
            ptr[1] = ipAddr[1];
            ptr[2] = ipAddr[2];
            ptr[3] = ipAddr[3];
        }
        else
        {
            /* ----------------------------------- */
            /* RTP_NET_TYPE_IPV6 not yet supported */
            /* ----------------------------------- */
            return (-1);
        }
    }
    else
    {
        in_addr = INADDR_ANY;
    }
    
    sin.sin_family = PF_INET;
    sin.sin_addr.s_addr = in_addr;
    sin.sin_port = htons((unsigned short)port);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if ((bind ((int) sockHandle, (const struct sockaddr *) &sin, sinLen)) != 0)
    {
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_bind: error returned ");
		RTP_DEBUG_OUTPUT_INT(errno);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_listen - Set up a socket for accepting connections
 
    sockHandle - the socket handle
    queueSize  - the max number of requested connections to queue for acceptance
    
 Description
 
    This function is called on a stream-type socket after rtp_net_bind
    but before rtp_net_accept to get a socket ready to accept connections
    from remote hosts.
    
 See Also

    rtp_net_accept, rtp_net_bind, rtp_net_socket_stream
        
 Returns
 
    0 on success, -1 on failure
----------------------------------------------------------------------*/

int rtp_net_listen (RTP_HANDLE sockHandle, int queueSize)
{
#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if ((listen ((int) sockHandle, queueSize)) != 0)
    {
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_listen: error returned ");
		RTP_DEBUG_OUTPUT_INT(errno);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_getpeername - Get port and IP address of the remote host
 
    sockHandle - socket connected to the remote host that is to be determined
    ipAddr     - to store the ip returned address
    port       - to store the returned port in host byte order
    type       - to store the returned type of ip address
                 4 indicates an IPv4 address
                 6 indicates an IPv6 address
    
 Description
 
    This function extracts the IP address and port number of the host 
    connected to the socket and stores it in addr.  The socket must 
    be connected.
        
 Returns
 
    0 on success, -1 on failure
----------------------------------------------------------------------*/

int rtp_net_getpeername (RTP_HANDLE sockHandle, unsigned char *ipAddr, int *port, int *type)
{
    struct sockaddr_in peerAddr;
    int peerLen;
    
    peerLen = sizeof (peerAddr);
    memset(&peerAddr, 0, peerLen);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (getpeername ((int) sockHandle, (struct sockaddr *) &peerAddr, &peerLen) != 0)
    {
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_getpeername: error returned ");
		RTP_DEBUG_OUTPUT_INT(errno);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    if (ipAddr)
    {
        unsigned long d = peerAddr.sin_addr.s_addr;
        unsigned char *ptr = (unsigned char *) &d;
        
        *type = RTP_NET_TYPE_IPV4;

        ipAddr[0] = ptr[0];
        ipAddr[1] = ptr[1];
        ipAddr[2] = ptr[2];
        ipAddr[3] = ptr[3];
    }

    if (port)
    {
        *port = ntohs (peerAddr.sin_port);       
    }
    
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_getsockname - Get port and IP address of the local host
 
    sockHandle - socket being used by the local host
    ipAddr     - to store the ip returned address
    port       - to store the returned port in host byte order
    type       - to store the returned type of ip address
                 4 indicates an IPv4 address
                 6 indicates an IPv6 address
    
 Description
 
    This function extracts the IP address and port number of the local
    host of the socket and stores it in addr.
        
 Returns
 
    0 on success, -1 on failure
----------------------------------------------------------------------*/

int rtp_net_getsockname (RTP_HANDLE sockHandle, unsigned char *ipAddr, int *port, int *type)
{
    struct sockaddr_in localAddr;
    int localLen;
    
    localLen = sizeof (localAddr);
    memset(&localAddr, 0, localLen);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (getsockname ((int) sockHandle, (struct sockaddr *) &localAddr, &localLen) != 0)
    {
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_getsockname: error returned ");
		RTP_DEBUG_OUTPUT_INT(errno);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    if (ipAddr)
    {
        unsigned long d = localAddr.sin_addr.s_addr;
        unsigned char *ptr = (unsigned char *) &d;
        
        *type = RTP_NET_TYPE_IPV4;
        
        ipAddr[0] = ptr[0];
        ipAddr[1] = ptr[1];
        ipAddr[2] = ptr[2];
        ipAddr[3] = ptr[3];
    }

    if (port)
    {
        *port = ntohs (localAddr.sin_port);       
    }
    
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_gethostbyname - Get the network address for a host name
 
    ipAddr     - to store the ip returned address
    type       - to store the returned type of ip address
                 4 indicates an IPv4 address
                 6 indicates an IPv6 address
    name       - string representation of the ip address
    
 Description
 
    This function is called to obtain an ip address from a
    character string.
        
 Returns
 
    0 on success, -1 on failure
----------------------------------------------------------------------*/

int rtp_net_gethostbyname (unsigned char *ipAddr, int *type, char *name)
{
    struct hostent *hp = NULL;
    
#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (!(hp = gethostbyname(name)))
    {
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_gethostbyname: error returned ");
		RTP_DEBUG_OUTPUT_INT(errno);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    if (hp->h_addrtype != PF_INET)
    {
        return (-1);
    }
    *type = RTP_NET_TYPE_IPV4;

    ipAddr[0] = (unsigned char) hp->h_addr_list[0][0];
    ipAddr[1] = (unsigned char) hp->h_addr_list[0][1];
    ipAddr[2] = (unsigned char) hp->h_addr_list[0][2];
    ipAddr[3] = (unsigned char) hp->h_addr_list[0][3];

    
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_accept - Accept a connection on a listening socket
 
    connectSock - pointer to a socket handle to set to the socket id of the 
                  new connection
    serverSock  - the socket to accept the connection on
    ipAddr      - to store the ip returned address
    port        - to store the returned port in host byte order
    type        - to store the returned type of ip address
                  4 indicates an IPv4 address
                  6 indicates an IPv6 address

 Description
 
    This function should block waiting for a remote host to connect to the 
    port/ip address to which serverSock is bound.  If a connection is 
    successfully established, it must set *connectSock to the socket for the 
    new connection (serverSock continues to listen on its port/ip), and return
    0. In the event of a successful connection, addr should 
    also be set to the ip/port of the remote host that connected.  If no 
    connection can be established or an error occurs, the return value is 
    negative, and the values of *connectSock, and *addr are undefined.
    
    The behavior of this function is undefined in the following cases:
    
        - serverSock is not a valid, stream-type socket
        - rtp_net_bind was never called on serverSock  
        - rtp_net_listen was never called on serverSock
        
 See Also
 
    rtp_net_bind, rtp_net_listen, rtp_net_socket_stream
    
 Returns
 
    0 on success, -1 on failure

----------------------------------------------------------------------*/

int rtp_net_accept (RTP_HANDLE *connectSock, RTP_HANDLE serverSock, 
                    unsigned char *ipAddr, int *port, int *type)
{
    struct sockaddr_in clientAddr;
    int clientLen;
    int conSocket;
    
    clientLen = sizeof (clientAddr);
    memset(&clientAddr, 0, clientLen);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    conSocket = accept ((int) serverSock, (struct sockaddr *) &clientAddr, &clientLen);
    
    if (conSocket < 0)
    {
        *connectSock = ((RTP_HANDLE)-1);
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_accept: error returned ");
		RTP_DEBUG_OUTPUT_INT(errno);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *connectSock = (RTP_HANDLE)conSocket;
    
    if (ipAddr)
    {
        unsigned long d = clientAddr.sin_addr.s_addr;
        unsigned char *ptr = (unsigned char *) &d;
        
        *type = RTP_NET_TYPE_IPV4;
        
        ipAddr[0] = ptr[0];
        ipAddr[1] = ptr[1];
        ipAddr[2] = ptr[2];
        ipAddr[3] = ptr[3];
    }

    if (port)
    {
        *port = ntohs (clientAddr.sin_port);       
    }
    
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_connect - Connect to a remote host

    sockHandle - the socket to connect
    ipAddr     - the ip address to connect to
    port       - port in host byte order
    type       - type of ip address
                 4 indicates an IPv4 address
                 6 indicates an IPv6 address

 Description
 
    This function is called on a stream-type socket to initiate a connection
    to a specific ip address and port.  This function can block until the 
    connection is established.  Behavior is undefined if sockHandle is not
    a valid, stream-type socket handle.

 See Also

    rtp_net_accept, rtp_net_socket_stream, rtp_net_closesocket
        
 Returns
 
    0 on success
    1 if connect is in progress or would block
    -1 on failure
    
----------------------------------------------------------------------*/

int rtp_net_connect (RTP_HANDLE sockHandle, 
                     unsigned char *ipAddr, 
                     int port, int type)
{
    int sinLen;
    struct sockaddr_in sin;
    unsigned long in_addr = 0;
    
    sinLen = sizeof (sin);
    memset(&sin, 0, sinLen);
    
    if (ipAddr)
    {
        unsigned char *ptr = (unsigned char *) &in_addr;
        
        if (type != RTP_NET_TYPE_IPV4)
        {
            return (-1);
        }
        
        ptr[0] = ipAddr[0];
        ptr[1] = ipAddr[1];
        ptr[2] = ipAddr[2];
        ptr[3] = ipAddr[3];
    }
    else
    {
        /* invalid address */
        return (-1);
    }
    
    sin.sin_family = PF_INET;
    sin.sin_addr.s_addr = in_addr;
    sin.sin_port = htons((unsigned short)port);

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;

    if (connect ((int) sockHandle, (const struct sockaddr *) &sin, sinLen) != 0)
    {
        if ((errno == EINPROGRESS) || (errno == EWOULDBLOCK))
        {
            return (1);
        }
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_connect: error returned ");
		RTP_DEBUG_OUTPUT_INT(errno);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_write_select - Check if socket is ready for send or 
                        if it is connected.
 
    sockHandle  - the socket to check
    msecTimeout - time period to wait in msec for the desired action
                   0 to poll
                  -1 to indicate use of infinite

 Description
 
    This function is called to determine if a socket is ready for
    sends or if the socket has completed a connect operation before 
    the timeout period is up.

 Returns
 
    0 on success, -1 on failure
    
----------------------------------------------------------------------*/

int rtp_net_write_select (RTP_HANDLE sockHandle, long msecTimeout)
{
struct timeval selectTime;
fd_set write_set;
int result;
    
    /* ----------------------------------- */
    /*              write list             */
    /* ----------------------------------- */
    FD_ZERO(&write_set);
    FD_SET(sockHandle, &write_set);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (msecTimeout >= 0)
    {
        selectTime.tv_sec = msecTimeout / 1000;
        selectTime.tv_usec = (msecTimeout % 1000) * 1000;
        result = select(1, 0, &write_set, 0, &selectTime);
    }
    else
    {
        result = select(1, 0, &write_set, 0, NULL);
    }

    /* if an error or if it timed out */
    if (result <= 0)
    {
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_write_select: error returned ");
		RTP_DEBUG_OUTPUT_INT(errno);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_read_select - Check if socket is ready for recv or 
                       ready for an accept.
 
    sockHandle  - the socket to check
    msecTimeout - time period to wait in msec for the desired action
                   0 to poll
                  -1 to indicate use of infinite

 Description
 
    This function is called to determine if a socket is ready for
    recv or if the socket is ready for an accept operation before 
    the timeout period is up.

 Returns
 
    0 on success, -1 on failure
    
----------------------------------------------------------------------*/

int rtp_net_read_select (RTP_HANDLE sockHandle, long msecTimeout)
{
struct timeval selectTime;
fd_set read_set;
int result;
    
    /* ----------------------------------- */
    /*              read list              */
    /* ----------------------------------- */
    FD_ZERO(&read_set);
    FD_SET(sockHandle, &read_set);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (msecTimeout >= 0)
    {
        selectTime.tv_sec = msecTimeout / 1000;
        selectTime.tv_usec = (msecTimeout % 1000) * 1000;
        result = select(1, &read_set, 0, 0, &selectTime);
    }
    else
    {
        result = select(1, &read_set, 0, 0, NULL);
    }

    /* if an error or if it timed out */
    if (result <= 0)
    {
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_read_select: error returned ");
		RTP_DEBUG_OUTPUT_INT(errno);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_send - Send data over a connection
 
    sockHandle  - the socket to send over
    buffer      - pointer to data to send
    size        - the number of unsigned chars to send
    
 Description

    This function is used to send data over a connected socket.  The socket
    must be a stream-type; behavior is undefined if sockHandle is a 
    datagram-type (connectionless) socket.

 See Also

    rtp_net_sendto, rtp_net_recv, rtp_net_recvfrom
        
 Returns
 
    the number of unsigned chars sent if successful, -1 if an error occurred
----------------------------------------------------------------------*/

long rtp_net_send (RTP_HANDLE sockHandle, unsigned char * buffer, long size)
{
    long result;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    result = (long) send((int) sockHandle, (const void *) buffer, (size_t) size, 0);
    
    if (result < 0)
    {
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_send: error returned ");
		RTP_DEBUG_OUTPUT_INT(errno);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (result);
}


/*----------------------------------------------------------------------
 rtp_net_recv - Receive data over a connection
 
    sockHandle - the socket to read from
    buffer     - pointer to a buffer to place received data in
    size       - the maximum number of unsigned chars to read
    
 Description
 
    This function is called on a connected socket to read data off that socket.
    It can block until up to size unsigned chars are read or until the connection is
    closed, signifying that no more unsigned chars are coming.  If the connection has
    been terminated, it can return either 0 or a negative value.
    
    If socketId is not a valid handle to a stream-type socket, behavior
    is undefined.
    
 See Also

    rtp_net_recvfrom, rtp_net_send, rtp_net_sendto
        
 Returns
 
    number of unsigned chars read on success, -1 on failure
----------------------------------------------------------------------*/

long rtp_net_recv (RTP_HANDLE sockHandle, unsigned char * buffer, long size)
{
    long result;
    
#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif
    
    result = (long) recv((int) sockHandle, (void *) buffer, (size_t) size, 0);
    
    if (result < 0)
    {
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_recv: error returned ");
		RTP_DEBUG_OUTPUT_INT(errno);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (result);
}


/*----------------------------------------------------------------------
 rtp_net_sendto - Send a datagram
 
    sockHandle  - the socket to send over
    buffer      - pointer to data to send
    size        - the number of unsigned chars to send
    ipAddr      - the ip address to send to
    port        - port in host byte order
    type        - type of ip address
                  4 indicates an IPv4 address
                  6 indicates an IPv6 address
    
 Description

    Sends size unsigned chars from buffer to the specified ip address/port.  The given
    socket must be a connectionless (datagram-type); otherwise, behavior is 
    undefined.

 See Also

    rtp_net_send, rtp_net_recv, rtp_net_recvfrom
        
 Returns
 
    the number of unsigned chars sent if successful, -1 if an error occurred
----------------------------------------------------------------------*/

long rtp_net_sendto (RTP_HANDLE sockHandle, 
                     unsigned char * buffer, long size,
                     unsigned char * ipAddr, int port, int type)
{
    long result;
    int  sinLen;
    struct sockaddr_in sin;
    unsigned long in_addr = 0;
        
    sinLen = sizeof (sin);
    memset(&sin, 0, sinLen);
    
    if (ipAddr)
    {
        unsigned char *ptr = (unsigned char *) &in_addr;
        
        if (type != RTP_NET_TYPE_IPV4)
        {
            return (-1);
        }
        
        ptr[0] = ipAddr[0];
        ptr[1] = ipAddr[1];
        ptr[2] = ipAddr[2];
        ptr[3] = ipAddr[3];
    }
    else
    {
        /* invalid address */
        return (-1);
    }
    
    sin.sin_family = PF_INET;
    sin.sin_addr.s_addr = in_addr;
    sin.sin_port = htons((unsigned short)port);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    result = (long) sendto((int) sockHandle, (const void *) buffer, (size_t) size, 0, (const struct sockaddr *)&sin, sinLen);
    
    if (result < 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_sendto: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (result);
}


/*----------------------------------------------------------------------
 rtp_net_recvfrom - Receive a datagram
 
    sockHandle - the socket to read from
    buffer     - pointer to a buffer to place received data in
    size       - the maximum number of unsigned chars to read
    ipAddr     - (optional) to store the ip returned address
    port       - (optional) to store the returned port in host byte order
    type       - (optional) to store the returned type of ip address
                 4 indicates an IPv4 address
                 6 indicates an IPv6 address
    
 Description
 
    This function is called on a connectionless socket to read data.  This 
    function should block until data is ready, then read at most size unsigned chars
    into buffer.  If the datagram is smaller than size unsigned chars, then only
    the number of unsigned chars in the datagram must be read into the buffer, and
    the function must return.  
    
 See Also

    rtp_net_recv, rtp_net_send, rtp_net_sendto
        
 Returns
 
    number of unsigned chars read on success, -1 on failure
----------------------------------------------------------------------*/

long rtp_net_recvfrom (RTP_HANDLE sockHandle, 
                       unsigned char *buffer, long size,
                       unsigned char *ipAddr, int *port, int *type)
{
    long result;
    struct sockaddr_in remote;
    int remoteLen;
    
    remoteLen = sizeof (remote);
    memset (&remote, 0, remoteLen);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    result = (long) recvfrom ((int) sockHandle, (void *) buffer, (size_t) size, 0, (struct sockaddr *) &remote, &remoteLen);

    if (result < 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_recvfrom: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    if (ipAddr)
    {
        unsigned long d = remote.sin_addr.s_addr;
        unsigned char *ptr = (unsigned char *) &d;
                
        ipAddr[0] = ptr[0];
        ipAddr[1] = ptr[1];
        ipAddr[2] = ptr[2];
        ipAddr[3] = ptr[3];
        
        *type = RTP_NET_TYPE_IPV4;
        *port = ntohs (remote.sin_port);
    }
    
    return (result);
}


/*----------------------------------------------------------------------
 rtp_net_closesocket - Close connection/free socket
 
    sockHandle - the socket to close

 Description
 
    This function should be called to release a socket and shut down any
    open connection it has.

 See Also

    rtp_net_accept, rtp_net_connect
        
 Returns
 
    0 on success, -1 on failure
----------------------------------------------------------------------*/

int rtp_net_closesocket (RTP_HANDLE sockHandle)
{
#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (shutdown((int) sockHandle, SHUT_RDWR) != 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_closesocket: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_shutdown - Disable receives and/or sends on a socket.
 
    sockHandle - the socket to shutdown
    how        - specifies which operations to shutdown
                 0 = disable recv
                 1 = disable send
                 2 = disable both recv and send

 Description
 
    This function disables recv or send depending upon the how parameter. 
    
    For TCP, if receives are disabled, incoming data will be accepted until
    the input window is full but the data will not be acknowledged. If 
    sends are disabled, a FIN will be snet.
    
    For UDP and RAW, if receives are disabled, incoming packets are queued
    on the UDP exchange.

    
    Resources will not be freed until a closesocket is done (TCP).
        
 Returns
 
    0 on success, -1 on failure
----------------------------------------------------------------------*/

int rtp_net_shutdown (RTP_HANDLE sockHandle, int how)
{
#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (shutdown((int) sockHandle, how) != 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_shutdown: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_setblocking - Set a socket to (non-)blocking mode
 
    sockHandle - the socket to set (non-)blocking
    onBool     - 1 to set blocking, 0 to set non-blocking

 Description
 
    This function sets a socket to blocking or non-blocking mode.
        
 Returns
 
    0 on success, -1 on failure
----------------------------------------------------------------------*/

int rtp_net_setblocking (RTP_HANDLE sockHandle, unsigned int onBool)
{
    unsigned long arg;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    arg = (unsigned long)(!onBool);
    if (ioctlsocket((int) sockHandle, FIONBIO, (u_long *)&arg) != 0)
    {
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_setblocking: error returned ");
		RTP_DEBUG_OUTPUT_INT(errno);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_setnagle - Enable/disable NAGLE flow control algorithm
 
    sockHandle - the socket to use or not use nagle
    onBool     - 1 to turn on nagle, 0 to turn of nagle

 Description
 
    This function sets a socket to blocking or non-blocking mode.
        
 Returns
 
    0 on success, -1 on failure
----------------------------------------------------------------------*/

int rtp_net_setnagle (RTP_HANDLE sockHandle, unsigned int onBool)
{
    rtp_not_yet_implemented();
    return (-1);
}


/*----------------------------------------------------------------------
 rtp_net_setlinger - Set linger time on a closed socket
 
    sockHandle  - the socket to set its linger period
    msecTimeout - timeout period in msec

 Description
 
    This function sets the length of time the socket lingers 
    before closing.  A timeout of 0 causes a hard close (immediatly
    closing the socket).  After closing the socket if the output
    window is able to become empty, a FIN is sent.  If the timeout
    is reached before this is possible, a RESET is sent and data 
    still in the output window may be lost.
        
 Returns
 
    0 on success, -1 on failure
----------------------------------------------------------------------*/

int rtp_net_setlinger (RTP_HANDLE sockHandle, long msecTimeout)
{
    struct linger arg = { 0, 0 };
     
    arg->l_onoff = 1;
    if( msecTimeout )
    {
        arg.l_linger = (int) (msecTimeout / 1000);
    }
    else
    {
        arg.l_linger = 0;
    }

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (setsockopt((int) sockHandle, SOL_SOCKET,
                   SO_LINGER, (const void *) &arg, 
                   (int) sizeof (struct linger)) < 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_setlinger: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_setreuse - Set reuse socket option
 
    sockHandle  - the socket to be reused
    onBool      - 1 reuse socket, 0 don't reuse socket

 Description
 
    This function lets the socket be reused if it is found in the 
    TWAIT state and run out of sockets.
        
 Returns
 
    0 on success, -1 on failure
----------------------------------------------------------------------*/

int rtp_net_setreuse (RTP_HANDLE sockHandle, unsigned int onBool)
{
    
#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (setsockopt((int) sockHandle, SOL_SOCKET, 
                    SO_REUSEPORT, (const void *) &onBool, 
                    (int) sizeof (int)) != 0)
    {
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_setreuse: error returned ");
		RTP_DEBUG_OUTPUT_INT(errno);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_htons - Convert a short int from host to network format
 
    i - the short integer to be converted

 Description
 
    On a little endian system the short will be flipped.
        
 Returns
 
    The converted short int.
----------------------------------------------------------------------*/

short rtp_net_htons (short i)
{
    return (htons(i));
}


/*----------------------------------------------------------------------
 rtp_net_ntohs - Convert a short int from network to host format
 
    i - the short integer to be converted

 Description
 
    On a little endian system the short will be flipped.
        
 Returns
 
    The converted short int.
----------------------------------------------------------------------*/

short rtp_net_ntohs (short i)
{
    return (ntohs(i));
}


/*----------------------------------------------------------------------
 rtp_net_htonl - Convert a long int from host to network format
 
    i - the long integer to be converted

 Description
 
    On a little endian system the long will be flipped.
        
 Returns
 
    The converted long int.
----------------------------------------------------------------------*/

long rtp_net_htonl (long i)
{
    return (htonl(i));
}


/*----------------------------------------------------------------------
 rtp_net_ntohl - Convert a long int from network to host format
 
    i - the long integer to be converted

 Description
 
    On a little endian system the long will be flipped.
        
 Returns
 
    The converted long int.
----------------------------------------------------------------------*/

long rtp_net_ntohl (long i)
{
    return (ntohl(i));
}


/*----------------------------------------------------------------------
 rtp_net_ip_to_str - Print address to a string

    str    - the storage location for the dotted string representation
             must be long enough to contain the string
    ipAddr - the ip address to be extracted
    type   - type of ip address
             4 indicates an IPv4 address
             6 indicates an IPv6 address

 Description
    
    Extracts an ip address and creates a dotted string representation.

 Notes
    {111,111,111,111} converts to "111.111.111.111"
    {111,111,111,0}   converts to "111.111.111.0"
    {111,111,0,0}     converts to "111.111.0.0"
    {111,0,0,0}       converts to "111.0.0.0"
 
 Returns
 
    The string length of the ip address, -1 on failure.
----------------------------------------------------------------------*/

int rtp_net_ip_to_str (char *str, unsigned char *ipAddr, int type)
{
    int n;

    str[0] = '\0';
    if (type != RTP_NET_TYPE_IPV4)
    {
        return (-1);
    }
    
    for (n=0; n<4; n++)
    {
        rtp_itoa(ipAddr[n], (char *) &(str[strlen(str)]), 10);
        if (n<3)
        {
            strcat((char *) str, ".");
        }
    }
    
    return (strlen((const char *) str));
}


/*----------------------------------------------------------------------
 rtp_net_str_to_ip - Parse address from a string

    ipAddr - to store the ip address
             for an IPv4 address this must be at least a four byte array
             for an IPv6 address this must be at least a six byte array
    str    - the dotted string to convert
    type   - type of ip address
             4 indicates an IPv4 address
             6 indicates an IPv6 address

 Description
    
    Parses a dotted string and stores the extracted ip address in addr.

 Notes
    "111.111.111.111" converts to {111,111,111,111}
    "111.111.111"     converts to {111,111,111,0}
    "111.111"         converts to {111,111,0,0}
    "111"             converts to {111,0,0,0}
 
 Returns
 
    0 on success
----------------------------------------------------------------------*/

int rtp_net_str_to_ip (unsigned char *ipAddr, char *str, int type)
{
    char * ptr;
    char savech;
    int n;

    if ((type != RTP_NET_TYPE_IPV4) && (type != RTP_NET_TYPE_IPV6))
    {
        return (-1);
    }

    for (n=0; n<type; n++)
    {
        ptr = str;
        while (*ptr != '.' && *ptr != '\0')
        {
            ptr++;
        }
        
        savech = *ptr;
        *ptr = '\0';
        ipAddr[n] =  (unsigned char)rtp_atoi((const char *) str);   
        if (savech == '\0')
        {
            break;
        }
        *ptr = savech;
    
        str = ptr + 1;
    }
    
    return (0);
}


/*----------------------------------------------------------------------
 rtp_fd_zero - Reset socket list
----------------------------------------------------------------------*/

void rtp_fd_zero (RTP_FD_SET  *list)
{
    list->fdCount = 0;
}


/*----------------------------------------------------------------------
 rtp_fd_set - Add socket to list
----------------------------------------------------------------------*/

void rtp_fd_set (RTP_FD_SET  *list, RTP_HANDLE fd)
{
    if (list->fdCount < RTP_FD_SET_MAX)
    {
        if (!rtp_fd_isset(list, fd))
        {
            list->fdArray[list->fdCount] = fd;
            list->fdCount++;
        }
    }
}


/*----------------------------------------------------------------------
 rtp_fd_clr - Remove socket from list
----------------------------------------------------------------------*/

void rtp_fd_clr (RTP_FD_SET  *list, RTP_HANDLE fd)
{
    int n;
    
    for (n = 0; n < list->fdCount; n++)
    {
        if (list->fdArray[n] == fd)
        {
            int i;
            
            for (i = n; i < list->fdCount - 1; i++)
            {
                list->fdArray[i] = list->fdArray[i+1];
            }
            
            list->fdCount--;
            break;
        }
    }
}


/*----------------------------------------------------------------------
 rtp_fd_isset - Check if socket is in list
----------------------------------------------------------------------*/

int rtp_fd_isset (RTP_FD_SET  *list, RTP_HANDLE fd)
{
    int n;
    
    for (n = 0; n < list->fdCount; n++)
    {
        if (list->fdArray[n] == fd)
        {
            return (1);
        }
    }

    return (0);
}


/*----------------------------------------------------------------------
 rtp_net_select - Select ready sockets
 
    readList    - (optional) a list of sockets to check for read access
    writeList   - (optional) a list of sockets to check for write access
    errorList   - (optional) a list of sockets to check for errors
    msecTimeout - time period to wait in msec for the desired action
                   0 to poll
                  -1 to indicate use of infinite

 Description
 
    This function is called to obtain the number of sockets that are 
    contained in the RTP_FD_SET lists that meet the condition before
    the timeout period is up.
        
 Returns
 
    0 if timed out, number of sockets on success, -1 on failure
----------------------------------------------------------------------*/

int rtp_net_select (RTP_FD_SET  *readList, 
                    RTP_FD_SET  *writeList,
                    RTP_FD_SET  *errorList, 
                    long msecTimeout)
{
    struct timeval selectTime;
    
    int index;
    int result;
    int listSize;
    
    fd_set write;
    fd_set read;
    fd_set error;
    
    FD_ZERO(&write);
    FD_ZERO(&read);
    FD_ZERO(&error);
    
    
    /* ----------------------------------- */
    /*              write list             */
    /* ----------------------------------- */
    if (writeList)
    {
        if (writeList->fdCount > RTP_FD_SET_MAX)
        {
            write.count = RTP_FD_SET_MAX;
        }
        else
        {
            write.count = writeList->fdCount;
        }
        
        for (index = 0; index < write.count; index++)
        {
            write.array[index] = (int) writeList->fdArray[index];
        }
    }

    /* ----------------------------------- */
    /*               read list             */
    /* ----------------------------------- */
    if (readList)
    {
        if (readList->fdCount > RTP_FD_SET_MAX)
        {
            read.count = RTP_FD_SET_MAX;
        }
        else
        {
            read.count = readList->fdCount;
        }
        
        for (index = 0; index < read.count; index++)
        {
            read.array[index] = (int) readList->fdArray[index];
        }
    }
    
    /* ----------------------------------- */
    /*              error list             */
    /* ----------------------------------- */
    if (errorList)
    {
        if (errorList->fdCount > RTP_FD_SET_MAX)
        {
            error.count = RTP_FD_SET_MAX;
        }
        else
        {
            error.count = errorList->fdCount;
        }
        
        for (index = 0; index < error.count; index++)
        {
            error.array[index] = (int) errorList->fdArray[index];
        }
    }

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    errno = 0;
#endif

    if (msecTimeout >= 0)
    {
        selectTime.tv_sec = msecTimeout / 1000;
        selectTime.tv_usec = (msecTimeout % 1000) * 1000;
        result = select(1, &read, &write, &error, &selectTime);
    }
    else
    {
        result = select(1, &read, &write, &error, NULL);
    }

    if (result < 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_select: error returned ");
        RTP_DEBUG_OUTPUT_INT(errno);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    return (result);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
