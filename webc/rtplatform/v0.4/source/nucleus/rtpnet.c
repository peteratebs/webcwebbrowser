 /*
 | RTPNET.C - Runtime Platform Network Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: tony $
 |  $Date: 2005/04/01 15:40:15 $
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
#include "rtpnet.h"
#include "externs.h"
#include "tcp.h"

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
                              rtp_net_init
 *----------------------------------------------------------------------*/
/** @memo   Initialize network API.

    @doc    Initialize network API.

    @return 0 on success, -1 otherwise. For debugging purposes; if 
    the cause of the error is obtainable at the native Socket layer, 
    turn on RTP_DEBUG in rtpnet.c to display the native 
    error value.
 */
int rtp_net_init (void)
{
#ifdef RTP_DEBUG
    assert (sizeof(RTP_FD_SET) >= sizeof(FD_SET));
#endif
    
	return (0);
}


/*----------------------------------------------------------------------*
                              rtp_net_exit
 *----------------------------------------------------------------------*/
/** @memo   Close network API.

    @doc    Close network API.

    @return void
 */
void rtp_net_exit (void)
{
}


/*----------------------------------------------------------------------*
                         rtp_net_socket_stream
 *----------------------------------------------------------------------*/
/** @memo   Allocate stream type (TCP) socket.

    @doc    Allocates, if possible, a stream (TCP, connection-based) 
    type socket.  The handle then may later be used as an argument to 
    the other RTPlatform Socket API functions.  The number of sockets 
    available at any one time is dependant on the native TCP/IP 
    stack.  When use of the socket handle is completed, 
    rtp_net_closesocket() should be called to make it available for 
    other calls to rtp_net_socket_stream().  Like the UNIX sockets 
    environment, the TCP socket handle is not useful until 
    rtp_net_connect(), or rtp_net_listen(), rtp_net_bind() and 
    rtp_net_accept() has been called.

    @return 0 if successful, -1 otherwise.  If a -1 is returned the 
    value of *sockeHandle is undefined.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_socket_stream (
  RTP_HANDLE * sockHandle               /** Address of a socket handle to store the allocated socket. */
  )
{
	*sockHandle = NU_Socket(NU_FAMILY_IP, NU_TYPE_STREAM, 0);
	
	if (*sockHandle < 0)
	{
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_socket_stream: failed to obtain IP socket.\n");
#endif
		return (-1);
	}
	NU_Fcntl(*sockHandle, NU_SETFLAG, NU_BLOCK);

	return (0);	
}


/*----------------------------------------------------------------------*
                        rtp_net_socket_datagram
 *----------------------------------------------------------------------*/
/** @memo   Allocate datagram type (UDP) socket.

    @doc    Allocates, if possible, a datagram (UDP, connectionless) 
    type socket.  The handle then may later be used as an argument 
    to the other RTPlatform Socket API functions.  The number of 
    sockets available at any one time is dependant on the native 
    TCP/IP stack.  When use of the socket handle is completed, 
    rtp_net_closesocket() should be called to make it available 
    for other calls to rtp_net_socket_datagram().

    @return 0 if successful, -1 otherwise.  If a -1 is returned the 
    value of *sockeHandle is undefined.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_socket_datagram (
  RTP_HANDLE * sockHandle               /** Address of a socket handle to store the allocated socket. */
  )
{
    *sockHandle = NU_Socket(NU_FAMILY_IP, NU_TYPE_DGRAM, 0);
	
	if (*sockHandle < 0)
	{
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_socket_datagram: failed to obtain UDP socket.\n");
#endif
		return (-1);
	}
	
	NU_Fcntl(*sockHandle, NU_SETFLAG, NU_BLOCK);
	return (0);
}


/*----------------------------------------------------------------------*
                            rtp_net_bind
 *----------------------------------------------------------------------*/
/** @memo   Bind a socket handle to a local port number and optionally 
    an IP address.

    @doc    This function should be called on a socket before 
    calling rtp_net_listen/rtp_net_accept (if the socket is 
    stream-type), or rtp_net_recvfrom (if the socket is 
    datagram-type).  The behavior of this function is undefined 
    if sockHandle is not a valid socket handle.  If the IP 
    address is not passed in, the default (typically INADDR_ANY) 
    is used.  If the default IP address is used, packets sent 
    to any of the devices interfaces are accepted.  If 
    rtp_net_bind is called with 0 for its port number, the 
    native TCP/IP stack should assign a unique port number.  If 
    the port specified is already in use by another socket 
    handle, this function must return -1.<br><br>
    
    Server applications will call rtp_net_bind() to establish a 
    “well known port” for clients to call on.  rtp_net_bind 
    does not establish any connections itself.<br><br>
    
    For datagram-type sockets, in order to receive broadcast and 
    multicast packets, the socket handle should be bound to the 
    defualt (INADDR_ANY) IP address or to the broadcast or the 
    specific multicast address.<br><br>
    
    It is not necessary to call rtp_net_bind if 
    rtp_net_connect or rtp_net_sendto is to be called.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_bind (
  RTP_HANDLE sockHandle,                /** Socket handle to bind to. */
  unsigned char *ipAddr,                /** The optional IP address to be attached to. */
  int port,                             /** Port (in host byte order) to be attached to. */
  int type                              /** Type of IP address:<br>
	<pre>
|		4   Indicates an IPv4 address.
|		6   Indicates an IPv6 address.
	</pre> */
  )
{
    
    struct addr_struct sin;
	unsigned long 		in_addr = 0;
	
	memset(&sin, 0, sizeof (sin));

#ifdef RTP_DEBUG
	RTP_DEBUG_OUTPUT_STR("PSMBNET: bind socket ");
	RTP_DEBUG_OUTPUT_INT(sockHandle);
	RTP_DEBUG_OUTPUT_STR(".\n");
#endif
	
	if (ipAddr)
	{
		unsigned char *ptr = (unsigned char *) &in_addr;
		ptr[0] = ipAddr[0];
		ptr[1] = ipAddr[1];
		ptr[2] = ipAddr[2];
		ptr[3] = ipAddr[3];
	}
	else
		in_addr = IP_ADDR_ANY;

	sin.family = NU_FAMILY_IP;
	memcpy(sin.id.is_ip_addrs, &in_addr, 4);
	sin.port = port;
	
	if (NU_Bind (sockHandle, &sin, 0) < 0)
		return (-1);

	return (0);
   
}


/*----------------------------------------------------------------------*
                            rtp_net_listen
 *----------------------------------------------------------------------*/
/** @memo   Set up a socket handle for accepting connections.

    @doc    This function is called on a stream-type socket after 
    rtp_net_bind() but before rtp_net_accept() to get a 
    socket ready to accept connections from remote hosts.  
    The behavior of this function is undefined if sockHandle 
    is not a valid socket handle.<br><br>
    
    After calling rtp_net_listen(), server applications call 
    rtp_net_accept() to establish a session with a client and 
    then “fork” a new task for the session.  The rtp_net_listen() 
    call allows a backlog of client connection requests to be 
    queued up for processing by the server.  Without 
    rtp_net_listen(), the client connection requests will be 
    rejected.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_listen (
  RTP_HANDLE sockHandle,                /** Pointer to a socket handle to listen on. */
  int queueSize                         /** The max number of requested connections 
                                            to queue for acceptance. */
  )
{
   return (NU_Listen (sockHandle, queueSize));
}


/*----------------------------------------------------------------------*
                          rtp_net_getpeername
 *----------------------------------------------------------------------*/
/** @memo   Get port and IP address of the remote host that the 
    socket handle is connected to.

    @doc    This function extracts the IP address and port number 
    of the host connected to the socket and stores them in the 
    arguments passed to the function.  The socket handle must be 
    connected. The ipAddr must have enough room to store the worst 
    case scenario (If IPv6 is enbled, a 16 byte or unsigned char 
    array containing 16 elements must be used).

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_getpeername (
  RTP_HANDLE sockHandle,                /** Socket handle connected to the remote host. */
  unsigned char *ipAddr,                /** Location to store the IP address in host 
	                                        byte order. */
  int *port,                            /** Location to store the port in host byte order. */
  int *type                             /** Location to store the type of IP address:<br>
	<pre>
|		4   Indicates an IPv4 address.
|		6   Indicates an IPv6 address.
	</pre> */
  )
{
    rtp_not_yet_implemented();
	return (-1);
}


/*----------------------------------------------------------------------*
                          rtp_net_getsockname
 *----------------------------------------------------------------------*/
/** @memo   Get port and IP address of the local host.

    @doc    This function extracts the IP address and port number 
    of the local host of the socket handle and stores them in the 
    arguments passed to the function. The ipAddr must have enough 
    room to store the worst case scenario (If IPv6 is enbled, a 
    16 byte or unsigned char array containing 16 elements must be 
    used).

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_getsockname (
  RTP_HANDLE sockHandle,                /** Socket handle connected to the remote host. */
  unsigned char *ipAddr,                /** Location to store the IP address in host 
	                                        byte order. */
  int *port,                            /** Location to store the port in host byte order. */
  int *type                             /** Location to store the type of IP address:<br>
	<pre>
|		4   Indicates an IPv4 address.
|		6   Indicates an IPv6 address.
	</pre> */
  )
{
    rtp_not_yet_implemented();
	return (-1);
}


/*----------------------------------------------------------------------*
                          rtp_net_gethostbyname
 *----------------------------------------------------------------------*/
/** @memo   Get the network address for a host name.

    @doc    This function is called to do a DNS lookup on the 
    string “name” value (domain name) to obtain the IP address.  
    The ipAddr must have enough room to store the worst case 
    scenario (If IPv6 is enbled, a 16 byte or unsigned char 
    array containing 16 elements must be used).  Note that DNS 
    uses the UDP protocol to retrieve the information from the 
    DNS server.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_gethostbyname (
  unsigned char *ipAddr,                /** Location to store the IP address in host byte order. */
  int *type,                            /** Location to store the type of IP address:<br>
	<pre>
|		4   Indicates an IPv4 address.
|		6   Indicates an IPv6 address.
	</pre> */
  char *name                            /** Wtring representation of the IP address. */
  )
{
	NU_HOSTENT hentry;
	STATUS status = NU_Get_Host_By_Name(name, &hentry);

	if (status == NU_SUCCESS)
	{
		memcpy(ipAddr, hentry.h_addr, 4);
		return (0);
	}
	else
	{
		return (-1);
	}
}


/*----------------------------------------------------------------------*
                           rtp_net_accept
 *----------------------------------------------------------------------*/
/** @memo   Accept a stream-type (TCP) connection on a bound and 
    listening socket.

    @doc    This function can block waiting for a remote host to 
    connect to the port/IP address to which serverSock is bound.  
    If a connection is successfully established, it must set 
    *connectSock to the socket for the new connection (serverSock 
    continues to listen on its port/IP address), and returns 0.  
    In the event of a successful connection, ipAddr, port, and type 
    should also be set to the port and IP address of the remote 
    host that is connected.  If no connection can be established or 
    an error occurs, the values of the arguments are undefined.  
    The ipAddr must have enough room to store the worst case 
    scenario (If IPv6 is enbled, a 16 byte or unsigned char 
    array containing 16 elements must be used).<br><br>

    rtp_net_accept returns a new socket handle to be used for 
    the connection but the original socket handle is still allocated 
    and needs to be closed when no more rtp_net_accept on the 
    original socket will be done.  The socket handle returned by 
    rtp_net_accept also needs to be closed when it is no longer 
    needed.<br><br>

    The behavior of this function is undefined in the following cases:    
    <pre>
|		- serverSock is not a valid, stream-type socket.
|		- rtp_net_bind was never called on serverSock.
|		- rtp_net_listen was never called on serverSock.
	</pre>

    @return 0 if successful, -1 on failure, -2 if a non-fatal 
    error occurred.  A non-fatal error includes a socket that is 
    set for non-blocking mode and the native accept call returns 
    immediately with an error indicating that it would block or 
    that the operation is in progress.  If the application making 
    use of rtp_net_accept is using non-blocking sockets, it is 
    advised that the return value is checked for -2 and continues, 
    using rtp_net_read/write_select to determine if the connection 
    has been established before making other RTPlatform Socket API 
    calls on this socket.  If the sockets are blocking, the 
    rtp_net_accept will wait infinitely for a connection.  For 
    debugging purposes; if the cause of the error is obtainable at 
    the native TCP/IP layer, turn on RTP_DEBUG in rtpnet.c 
    to display the native error value.
 */
int rtp_net_accept (
  RTP_HANDLE * connectSock,             /** Location to store the socket handle 
	                                        of the new connection. */
  RTP_HANDLE serverSock,                /** The socket handle to accept the 
	                                        connection on. */
  unsigned char * ipAddr,               /** Location to store the IP address in 
	                                        host byte order. */
  int * port,                           /** Location to store the port in host 
	                                        byte order. */
  int * type                            /** Location to store the type of IP address:<br>
	<pre>
|		4   Indicates an IPv4 address.
|		6   Indicates an IPv6 address.
	</pre> */
  )
{
 	struct addr_struct clientAddr;
	int clientLen;
	
	clientLen = sizeof (clientAddr);
	
	/**
	 * Move connection to a shiny new port and socket.
	 */
	*connectSock = NU_Accept(serverSock, &clientAddr, 0);

	if (*connectSock < 0)
	{
		return (-1);
	}

	if (ipAddr)
	{
		unsigned char *ptr = clientAddr.id.is_ip_addrs;
		ipAddr[0] = ptr[0];
		ipAddr[1] = ptr[1];
		ipAddr[2] = ptr[2];
		ipAddr[3] = ptr[3];

#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_accept: accepted connection from ");
		RTP_DEBUG_OUTPUT_INT(ptr[0])
		RTP_DEBUG_OUTPUT_STR(".");
		RTP_DEBUG_OUTPUT_INT(ptr[1]);
		RTP_DEBUG_OUTPUT_STR(".");
		RTP_DEBUG_OUTPUT_INT(ptr[2]);
		RTP_DEBUG_OUTPUT_STR(".");
		RTP_DEBUG_OUTPUT_INT(ptr[3]);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
	}
	
	if (ipAddr)
	{
		*ipAddr = clientAddr.port;		
	}
	
	return (0);
}


/*----------------------------------------------------------------------*
                          rtp_net_connect
 *----------------------------------------------------------------------*/
/** @memo   Connect a socket to a remote host.

    @doc    rtp_net_connect establishes a connection between 
    a socket and the service provider at the IP address and port 
    number provided in the arguments.  If the socket is unbound, 
    it will be bound with a unique port number and the local 
    connecting IP address for the interface.  Behavior is 
    undefined if sockHandle is not a valid, socket handle.  For 
    stream-type (TCP) sockets, rtp_net_connect initiates the 
    handshake.  For connectionless (UDP) sockets, the IP address 
    and port number are bound to the socket and the function 
    returns immediately.

    @return 0 if successful, -1 on failure, -2 if a non-fatal 
    error occurred.  A non-fatal error includes a socket that is 
    set for non-blocking mode and the native connect call 
    returns immediately with an error indicating that it would 
    block or that the operation is in progress.  If the application 
    making use of rtp_net_connect is using non-blocking sockets, 
    it is advised that the return value is checked for -2 and 
    continues, using rtp_net_read/write_select to determine if 
    the connection has been established before making other 
    RTPlatform Socket API calls on this socket.  If the sockets are 
    blocking, the rtp_net_connect will wait infinitely for a 
    connection.  For debugging purposes; if the cause of the error 
    is obtainable at the native TCP/IP layer, turn on 
    RTP_DEBUG in rtpnet.c to display the native error value.
 */
int rtp_net_connect (
  RTP_HANDLE sockHandle,                /** The socket handle to make the 
	                                        connection on. */
  unsigned char *ipAddr,                /** The IP address in host byte 
	                                        order to connect to. */
  int port,                             /** The port in host byte order to 
	                                        connect to. */
  int type                              /** The type of IP address to 
	                                        connect to:<br>
	<pre>
|		4   Indicates an IPv4 address.
|		6   Indicates an IPv6 address.
	</pre> */
  )
{
    STATUS 			status;
	struct addr_struct 	sin;
	unsigned long 		in_addr = 0;
    struct sock_struct 	*sockptr;
#ifdef RTP_DEBUG
	unsigned char 		*ptr;
#endif
	
	memset(&sin, 0, sizeof (sin));

	if (ipAddr)
	{
		unsigned char *ptr = (unsigned char *) &in_addr;
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

    sockptr = SCK_Sockets[sockHandle];

    if (sockptr->s_protocol == NU_PROTO_UDP)
    {
#ifdef RTP_DEBUG
    	ptr = sockptr->s_local_addr.ip_num.is_ip_addrs;
    	RTP_DEBUG_OUTPUT_STR("rtp_net_connect: local ");
		RTP_DEBUG_OUTPUT_INT(ptr[0])
		RTP_DEBUG_OUTPUT_STR(".");
		RTP_DEBUG_OUTPUT_INT(ptr[1]);
		RTP_DEBUG_OUTPUT_STR(".");
		RTP_DEBUG_OUTPUT_INT(ptr[2]);
		RTP_DEBUG_OUTPUT_STR(".");
		RTP_DEBUG_OUTPUT_INT(ptr[3]);
		RTP_DEBUG_OUTPUT_STR(":");
		RTP_DEBUG_OUTPUT_INT(sockptr->s_local_addr.port_num);
		RTP_DEBUG_OUTPUT_STR(".\n");
		
    	ptr = sockptr->s_foreign_addr.ip_num.is_ip_addrs;
    	RTP_DEBUG_OUTPUT_STR("rtp_net_connect: foreign ");
		RTP_DEBUG_OUTPUT_INT(ptr[0])
		RTP_DEBUG_OUTPUT_STR(".");
		RTP_DEBUG_OUTPUT_INT(ptr[1]);
		RTP_DEBUG_OUTPUT_STR(".");
		RTP_DEBUG_OUTPUT_INT(ptr[2]);
		RTP_DEBUG_OUTPUT_STR(".");
		RTP_DEBUG_OUTPUT_INT(ptr[3]);
		RTP_DEBUG_OUTPUT_STR(":");
		RTP_DEBUG_OUTPUT_INT(sockptr->s_foreign_addr.port_num);
		RTP_DEBUG_OUTPUT_STR(".\n");
		RTP_DEBUG_OUTPUT_STR("rtp_net_connect: port index ");
		
		RTP_DEBUG_OUTPUT_INT(sockptr->s_port_index);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
    }
    else
    {
		sin.family = NU_FAMILY_IP;
		memcpy(sin.id.is_ip_addrs, &in_addr, 4);
		sin.port = (unsigned short)port;
	
		if ( (status = NU_Connect (sockHandle, &sin, 0)) < 0)
		{
			return (-1);
		}
    }

	return (0);
}


/*----------------------------------------------------------------------*
                          rtp_net_is_connected
 *----------------------------------------------------------------------*/
/** @memo   Determine if a socket is connected to a peer.

    @doc    rtp_net_is_connected determines if a connection 
    has been made over the indicated socket.

    @return 1 if successful, 0 on failure. For debugging 
    purposes; if the cause of the error is obtainable at the 
    native TCP/IP layer, turn on RTP_DEBUG in 
    rtpnet.c to display the native error value.
 */
unsigned rtp_net_is_connected (
  RTP_SOCKET sockHandle                 /** The socket handle to determine connected status. */
  )
{
	return (NU_Is_Connected(sockHandle) == NU_TRUE);
}


/*----------------------------------------------------------------------*
                        rtp_net_write_select
 *----------------------------------------------------------------------*/
/** @memo   Check if socket handle is ready for sending over 
    or if it is connected.

    @doc    This function is called to determine if a socket handle 
    is ready for sends or if the socket has completed a connect 
    operation all before the timeout period has expired.  The 
    associated socket should be set to non-blocking before calling 
    rtp_net_connect, rtp_net_accept, rtp_net_recv, and rtp_net_send
     to ensure the function will not block.

    @return 0 if successful, -1 on timeout and failure.  For 
    debugging purposes; if the cause of the error is obtainable 
    at the native TCP/IP layer, turn on RTP_DEBUG in 
    rtpnet.c to display the native error value.
 */
int rtp_net_write_select (
  RTP_HANDLE sockHandle,                /** The socket handle to check its state. */
  long msecTimeout                      /** Time period to wait in msec for the 
	                                        desired action:<br>
	<pre>
|		 0   To poll.
|		-1   To indicate use of infinite.
	</pre> */
  )
{
int selectTime;
FD_SET write_set;
int result;
    
    /* ----------------------------------- */
    /*              read list              */
    /* ----------------------------------- */
    NU_FD_Init(&write_set);
    NU_FD_Set(sockHandle, &write_set);

	if (msecTimeout >= 0)
    {
    	selectTime = msecTimeout / NU_Milliseconds_Per_Tick();
    	if (selectTime ==0)
    	{
		selectTime++;
	}
		result = NU_Select(((int)sockHandle) + 1, (FD_SET *) NULL, (FD_SET *) &write_set, (FD_SET *) NULL, selectTime);
    }
	else
	{
		result = NU_Select(((int)sockHandle) + 1, (FD_SET *) NULL, (FD_SET *) &write_set, (FD_SET *) NULL, 0);
	}

    /* if an error or if it timed out */
   if (result != NU_SUCCESS)
    {
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_write_select: error returned ");
		RTP_DEBUG_OUTPUT_INT(result);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    return (0);
}


/*----------------------------------------------------------------------*
                        rtp_net_read_select
 *----------------------------------------------------------------------*/
/** @memo   Check if socket handle is ready for receiving data 
    or if it is ready to attempt an accept operation.

    @doc    This function is called to determine if a socket handle 
    is ready for receives or if it is ready for an accept operation 
    all before the timeout period has expired.  The associated 
    socket should be set to non-blocking before calling 
    rtp_net_connect, rtp_net_accept, rtp_net_recv, and 
    rtp_net_send to ensure the function will not block.

    @return 0 if successful, -1 on timeout and failure.  For 
    debugging purposes; if the cause of the error is obtainable 
    at the native TCP/IP layer, turn on RTP_DEBUG in 
    rtpnet.c to display the native error value.
 */
int rtp_net_read_select (
  RTP_HANDLE sockHandle,                /** The socket handle to check its state. */
  long msecTimeout                      /** Time period to wait in msec for the 
	                                        desired action:<br>
	<pre>
|		 0   To poll.
|		-1   To indicate use of infinite.
	</pre> */
  )
{
int selectTime;
FD_SET read_set;
int result;
    
    /* ----------------------------------- */
    /*              read list              */
    /* ----------------------------------- */
    NU_FD_Init(&read_set);
    NU_FD_Set(sockHandle, &read_set);

	if (msecTimeout >= 0)
    {
    	selectTime = msecTimeout / NU_Milliseconds_Per_Tick();
    	if (selectTime ==0)
    	{
		selectTime++;
	}
		result = NU_Select(((int)sockHandle) + 1, (FD_SET *) &read_set, (FD_SET *) NULL, (FD_SET *) NULL, selectTime);
    }
	else
	{
		result = NU_Select(((int)sockHandle) + 1, (FD_SET *) &read_set, (FD_SET *) NULL, (FD_SET *) NULL, 0);
	}

    /* if an error or if it timed out */
   if (result != NU_SUCCESS)
    {
    
#ifdef RTP_DEBUG
		RTP_DEBUG_OUTPUT_STR("rtp_net_read_select: error returned ");
		RTP_DEBUG_OUTPUT_INT(result);
		RTP_DEBUG_OUTPUT_STR(".\n");
#endif

        return (-1);
    }
    
    return (0);
}


/*----------------------------------------------------------------------*
                           rtp_net_send
 *----------------------------------------------------------------------*/
/** @memo   Send data over a connection.

    @doc    This function is used to send data over a connected 
    socket. The socket must be a stream-type (TCP) socket; behavior 
    is undefined if sockHandle is not a valid stream-type socket.<br><br>

    If the socket is in blocking mode, it returns after all the 
    data has been sent and acknowledged by the remote host.  If the 
    socket is in non-blocking mode, as much data as possible is 
    queued in the output window and as much data as possible is 
    sent according to the remote hosts window size and the mss.  
    The function then returns.  If there is no room in the output 
    window, the function returns an error that may be non-fatal.  
    If the socket is in non-blocking mode, select may be called 
    to wait for any room in the output window.<br><br>

    In non-blocking mode, successful completion of the 
    rtp_net_send() indicates that all data is queued in the 
    output window.  Call rtp_net_write_select to receive 
    notification when the data was sent and acknowledged.<br><br>

    In blocking mode, successful completion of the rtp_net_send 
    indicates that all data has been received by the remote host.

    @return The number of unsigned chars (bytes) sent if successful, 
    -1 on error, -2 if a non-fatal error occurred.  A non-fatal error 
    includes a socket that is set for non-blocking mode and the 
    native send operation returns immediately with an error 
    indicating that it would block or that the operation is in 
    progress.  If the application making use of rtp_net_send is 
    using non-blocking sockets, it is advised that the return value 
    is checked for -2 and continues, using rtp_net_write_select to 
    determine if the send operation has been completed before making 
    other RTPlatform Socket API calls on this socket.  For debugging 
    purposes; if the cause of the error is obtainable at the native 
    TCP/IP layer, turn on RTP_DEBUG in rtpnet.c to display 
    the native error value.
 */
long rtp_net_send (
  RTP_HANDLE sockHandle,                /** The socket handle to send over. */
  const unsigned char * buffer,         /** Pointer to the data to be sent. */
  long size                             /** The number of unsigned chars (bytes) to send. */
  )
{
	long ret = NU_Send(sockHandle, (char*) buffer, size, 0);
	if (ret < 0)
	{
		if ((ret == NU_WOULD_BLOCK) || (ret == NU_NO_DATA))
		{
			return -2;
		}
		else
		{
			return -1;
		}
	}
	return ret;
}


/*----------------------------------------------------------------------*
                           rtp_net_recv
 *----------------------------------------------------------------------*/
/** @memo   Receive data over a connection.

    @doc    This function is used to receive data over a connected 
    socket. The socket must be a stream-type (TCP) socket; behavior is 
    undefined if sockHandle is not a valid stream-type socket.<br><br>

    If the socket is in blocking mode, it returns after “size“ 
    unsigned chars (bytes) have been received or a FIN has been 
    received.  If no data is available and the socket is in 
    blocking mode, rtp_net_recv blocks on the read signal until 
    any data is available.  If no data is available and the socket 
    is in non-blocking mode, rtp_net_recv returns immediately 
    with a -2.  rtp_net_read_select may be called prior to calling 
    rtp_net_recv to determine when data is available.

    @return The number of unsigned chars (bytes) received if 
    successful, -1 on error, -2 if a non-fatal error occurred.  
    A non-fatal error includes a socket that is set for 
    non-blocking mode and the native receive operation returns 
    immediately with an error indicating that it would block or 
    that the operation is in progress.  If the application making 
    use of rtp_net_recv is using non-blocking sockets, it is 
    advised that the return value is checked for -2 and continues, 
    using rtp_net_read_select to determine if the rtp_net_recv 
    should be tried again before making other RTPlatform Socket API 
    calls on this socket.  For debugging purposes; if the cause of 
    the error is obtainable at the native TCP/IP layer, turn on 
    RTP_DEBUG in rtpnet.c to display the native error value.
 */
long rtp_net_recv (
  RTP_HANDLE sockHandle,                /** The socket handle to receive over. */
  unsigned char * buffer,               /** Pointer to the storage location for the data received. */
  long size                             /** The maximum number of unsigned chars (bytes) to receive. */
  )
{
	long ret = NU_Recv(sockHandle, (char*) buffer, size, 0);
	if (ret < 0)
	{
		if ((ret == NU_WOULD_BLOCK) || (ret == NU_NO_DATA))
		{
			return -2;
		}
		else
		{
			return -1;
		}
	}
	return ret;
}


/*----------------------------------------------------------------------*
                           rtp_net_sendto
 *----------------------------------------------------------------------*/
/** @memo   Send a datagram.

    @doc    This function is used to send data over a connectionless 
    socket (UDP).  The socket must be a connectionless (UDP) 
    socket; behavior is undefined if sockHandle is not a valid 
    connectionless (UDP) socket.

    @return The number of unsigned chars (bytes) sent if successful, 
    -1 on error, -2 if a non-fatal error occurred.  A non-fatal 
    error includes a socket that is set for non-blocking mode and 
    the native sendto operation returns immediately with an error 
    indicating that it would block or that the operation is in 
    progress.  If the application making use of rtp_net_sendto is 
    using non-blocking sockets, it is advised that the return value 
    is checked for -2 and continues, using rtp_net_write_select to 
    determine if the sendto operation has been completed before 
    making other RTPlatform Socket API calls on this socket.  For 
    debugging purposes; if the cause of the error is obtainable at 
    the native TCP/IP layer, turn on RTP_DEBUG in rtpnet.c 
    to display the native error value.
 */
long rtp_net_sendto (
  RTP_HANDLE sockHandle,                /** The socket handle to send over. */
  const unsigned char * buffer,         /** Pointer to the data to be sent. */
  long size,                            /** The number of unsigned chars (bytes) to send. */
  unsigned char * ipAddr,               /** The IP address in host byte order to send to. */
  int port,                             /** The port in host byte order to send to. */
  int type                              /** The type of IP address to send to:<br>
	<pre>
|		4   Indicates an IPv4 address.
|		6   Indicates an IPv6 address.
	</pre> */
  )
{
	struct addr_struct sin;
	unsigned long in_addr = 0;
	STATUS status;
	
	memset(&sin, 0, sizeof (sin));
	
	if (ipAddr)
	{
		unsigned char *ptr = (unsigned char *) &in_addr;
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
	
	sin.family = NU_FAMILY_IP;
	memcpy(sin.id.is_ip_addrs,&in_addr,4);
	sin.port = port;
	
	status = NU_Send_To(sockHandle, (char*) buffer, size, 0,&sin, 0);

#ifdef RTP_DEBUG
	if (status < 0)
	{
		RTP_DEBUG_OUTPUT_STR("rtp_net_sendto: returned error ");
		RTP_DEBUG_OUTPUT_INT(status);
		RTP_DEBUG_OUTPUT_STR(".\n");
	}
#endif

	return (status);
}


/*----------------------------------------------------------------------*
                           rtp_net_recvfrom
 *----------------------------------------------------------------------*/
/** @memo   Receive a datagram.

    @doc    This function is used to receive data over a connectionless 
    socket (UDP).  The socket must be a connectionless (UDP) 
    socket; behavior is undefined if sockHandle is not a valid 
    connectionless (UDP) socket.<br><br>

    If the socket is in blocking mode, it returns after “size“ 
    unsigned chars (bytes) have been received or a graceful close 
    causing a return of 0.  If no data is available and the socket 
    is in blocking mode, rtp_net_recvfrom blocks on the read 
    signal until any data is available.  If no data is available 
    and the socket is in non-blocking mode, rtp_net_recvfrom 
    returns immediately with a -2.  rtp_net_read_select may be 
    called prior to calling rtp_net_recvfrom to determine when 
    data is available.  If the datagram is smaller than the “size” 
    of unsigned chars (bytes), then only the number of unsigned 
    chars in the datagram must be read into the buffer, and the 
    function must return.

    @return The number of unsigned chars (bytes) received if 
    successful, -1 on error, -2 if a non-fatal error occurred.  
    A non-fatal error includes a socket that is set for 
    non-blocking mode and the native datagram receive operation 
    returns immediately with an error indicating that it would 
    block or that the operation is in progress.  If the 
    application making use of rtp_net_recvfrom is using 
    non-blocking sockets, it is advised that the return value 
    is checked for -2 and continues, using rtp_net_read_select 
    to determine if the rtp_net_recv should be tried again 
    before making other RTPlatform Socket API calls on this socket.  
    For debugging purposes; if the cause of the error is obtainable 
    at the native TCP/IP layer, turn on RTP_DEBUG in 
    rtpnet.c to display the native error value.
 */
long rtp_net_recvfrom (
  RTP_HANDLE sockHandle,                /** The socket handle to receive over. */
  unsigned char *buffer,                /** Location to store the data received. */
  long size,                            /** The number of unsigned chars (bytes) to receive. */
  unsigned char *ipAddr,                /** (optional) Location to store the returned IP address. */
  int *port,                            /** (optional) Location to store the returned port in host byte order. */
  int *type                             /** (optional) Location to store the returned type of IP address:<br>
	<pre>
|		4   Indicates an IPv4 address.
|		6   Indicates an IPv6 address.
	</pre> */
  )
{
	struct addr_struct remote;
	long bytesRead;
	int remoteLen;
	
	RTP_DEBUG_OUTPUT_STR("rtp_net_recvfrom: recv from socket ");
	RTP_DEBUG_OUTPUT_INT(sockHandle);
	RTP_DEBUG_OUTPUT_STR(".\n");

	remoteLen = sizeof (remote);
	
	bytesRead = NU_Recv_From (sockHandle, (char*) buffer, size, 0, &remote, 0);

	if (ipAddr)
	{
		unsigned char *ptr = remote.id.is_ip_addrs;
		ipAddr[0] = ptr[0];
		ipAddr[1] = ptr[1];
		ipAddr[2] = ptr[2];
		ipAddr[3] = ptr[3];
	}
	
	if (port)
	{
		*port = (remote.port);		
	}
	
	return (bytesRead);
}


/*----------------------------------------------------------------------*
                          rtp_net_closesocket
 *----------------------------------------------------------------------*/
/** @memo   Close the connection and free the socket.

    @doc    This function is called to release a socket and 
    shutdown any open connections it has.  For stream-type (TCP) 
    sockets, this routine starts a shutdown of a connection.  
    Once handshaking is complete and all the input data queued 
    on the socket has been read, all resources associated with 
    the connection are freed.  For connectionless (UDP) sockets, 
    the connection is dropped immediately.<br><br>

    This may be used with rtp_net_setlinger() to obtain the 
    following affects:<br>
    <pre>
|		   ________________________________________________________
|		                  TCP CLOSE SOCKET SUMMARY                 
|		   ________________________________________________________
|		   TYPE    | LINGER | LINGER BLOCK | WILL BLOCK | DATA LOST
|		   ________|________|______________|____________|__________
|		   hard    |   on   |  zero        |   no       | possibly 
|		   graceful|   on   |  non-zero    |   yes      | possibly 
|		   graceful|   off  |  zero        |   no       | no       
|		   graceful|   off  |  non-zero    |   no       | possibly*
|		   ________|________|______________|____________|__________
|		   *Note: Not BSD or Winsock standard.
	</pre>

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_closesocket (
  RTP_HANDLE sockHandle                 /** Pointer to a socket handle of the socket to close. */
  )
{
	return (NU_Close_Socket (sockHandle));
}


/*----------------------------------------------------------------------*
                            rtp_net_shutdown
 *----------------------------------------------------------------------*/
/** @memo   Disable receives and/or sends on a socket.

    @doc    This function is called to disable receive and/or send 
    operations depending upon the “how” parameter.  For 
    stream-type (TCP) sockets, if receives are disabled, 
    incoming data will be accepted until the input window 
    is full but the data will not be acknowledged.  If sends 
    are disabled, a FIN will be sent.  For connectionless 
    (UDP) sockets, if receives are disabled, incoming packets 
    are queued.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_shutdown (
  RTP_HANDLE sockHandle,                /** Socket handle of the socket to manipulate. */
  int how                               /** Specifies which operations to shutdown:<br>
	<pre>
|		0   Disable recv.
|		1   Disable send.
|		2   Disable both recv and send.
	</pre> */
  )
{
    rtp_not_yet_implemented();
	return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_net_getntoread
 *----------------------------------------------------------------------*/
/** @memo   Get the number of bytes available to read.

    @doc    For stream-type sockets (TCP), the total number of bytes 
    in the input window is returned.  For connectionless sockets 
    (UDP), the total number of bytes in the first packet queued on 
    the socket is returned.  The number of bytes available is 
    stored in nToRead.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_getntoread (
    RTP_HANDLE sockHandle,              /** Socket handle of the socket to access. */
    unsigned long * nToRead             /** Storage location of the returned number to read. */
    )
{
    rtp_not_yet_implemented();
	return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_net_setblocking
 *----------------------------------------------------------------------*/
/** @memo   Set a socket to (non-)blocking mode.

    @doc    This function sets a socket to blocking or non-blocking 
    mode depending on the “onBool” value.  sockHandle must be a 
    valid socket handle.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_setblocking (
  RTP_HANDLE sockHandle,                /** Socket handle of the socket to manipulate. */
  unsigned int onBool                   /** 1 to set blocking, 0 to set non-blocking. */
  )
{
	NU_Fcntl(sockHandle, NU_SETFLAG, onBool? NU_BLOCK : NU_NO_BLOCK);
	return (0);
}


/*----------------------------------------------------------------------*
                            rtp_net_setnagle
 *----------------------------------------------------------------------*/
/** @memo   Enable/disable the NAGLE flow control algorithm.

    @doc    This function sets a socket to use the Nagle algorithm 
    or disable the Nagle algorithm depending on the “onBool” 
    value.  sockHandle must be a valid socket handle.  The Nagle 
    algorithm prohibits sending of small stream-type (TCP) 
    packets (less than the MSS) while there is any outstanding 
    output data which has not been acknowledged.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_setnagle (
  RTP_HANDLE sockHandle,                /** Socket handle of the socket to manipulate. */
  unsigned int onBool                   /** 1 to turn on nagle, 0 to turn of nagle. */
  )
{
	INT option = (onBool)? 1 : 0;
	if (NU_Setsockopt (sockHandle, IPPROTO_TCP, TCP_NODELAY, &option, sizeof(INT)) != NU_SUCCESS)
	{
		return (-1);
	}
	return 0;
}


/*----------------------------------------------------------------------*
                          rtp_net_setlinger
 *----------------------------------------------------------------------*/
/** @memo   Set time to linger while closing a socket.

    @doc    This function sets the length of time the socket lingers 
    before closing.  A timeout of 0 causes a hard close (immediately 
    closing the socket).  After closing the socket, if the output 
    window is able to become empty, a FIN is sent.  If the timeout is 
    reached before this is possible, a RESET is sent and the data 
    still in the output window may be lost.<br><br>

    <pre>
|		   ________________________________________________________
|		                  TCP CLOSE SOCKET SUMMARY                 
|		   ________________________________________________________
|		   TYPE    | LINGER | LINGER BLOCK | WILL BLOCK | DATA LOST
|		   ________|________|______________|____________|__________
|		   hard    |   on   |  zero        |   no       | possibly 
|		   graceful|   on   |  non-zero    |   yes      | possibly 
|		   graceful|   off  |  zero        |   no       | no       
|		   graceful|   off  |  non-zero    |   no       | possibly*
|		   ________|________|______________|____________|__________
|		   *Note: Not BSD or Winsock standard.
	</pre>

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_setlinger (
  RTP_HANDLE sockHandle,                /** Socket handle of the socket to manipulate. */
  unsigned int onBool,                  /** Boolean to turn on/off linger. */
  long msecTimeout                      /** Timeout period in msec to linger before hard closing. */
  )
{
	return (0);
}


/*----------------------------------------------------------------------*
                           rtp_net_setreusesock
 *----------------------------------------------------------------------*/
/** @memo   Set the reuse socket option.

    @doc    This function lets the socket be reused (“onBool” is 1) 
    if the socket is found in the TWAIT state and no other sockets 
    are available.  sockHandle must be a valid socket handle.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_setreusesock (
  RTP_HANDLE sockHandle,                /** Socket handle of the socket to manipulate. */
  unsigned int onBool                   /** 1 reuse socket, 0 don't reuse socket. */
  )
{
    rtp_not_yet_implemented();
    return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_net_setreuseaddr
 *----------------------------------------------------------------------*/
/** @memo   Set the reuse address option.

    @doc    This function lets the socket to use the same address
    (“onBool” is 1), i.e. IP address and port number, which is 
    already being used by another socket of the same protocol type 
    in the system.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_setreuseaddr (
  RTP_HANDLE sockHandle,                /** Socket handle of the socket to manipulate. */
  unsigned int onBool                   /** 1 reuse address, 0 don't reuse address. */
  )
{
    rtp_not_yet_implemented();
    return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_net_settcpnocopy
 *----------------------------------------------------------------------*/
/** @memo   Set the tcp no copy option.

    @doc    If no copy mode is set it causes the native TCP/IP 
    layer to not copy any data as it is placed into the input or 
    output windows.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_settcpnocopy (
  RTP_HANDLE sockHandle,                /** Socket handle of the socket to manipulate. */
  unsigned int onBool                   /** 1 no copy, 0 copy. */
  )
{
    rtp_not_yet_implemented();
    return (-1);
}


/*----------------------------------------------------------------------*
                         rtp_net_setkeepalive
 *----------------------------------------------------------------------*/
/** @memo   Set keep alive socket option.

    @doc    If keep alive is enabled, stream-type (TCP) keep alive 
    packets are sent to the connected host socket if no packet has 
    been received from that host after a period of time.  The keep 
    alives are retried until a packet is received or a timeout 
    occurs.  If a tiemout occurs, the socket is set to the closed 
    state, i.e. no close handshake is attempted.  If the socket is 
    closed, the resources will not be freed until 
    rtp_net_closesocket is called.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_setkeepalive (
  RTP_HANDLE sockHandle,                /** Socket handle of the socket to manipulate. */
  unsigned int onBool                   /** 1 keep alive, 0 don't keep alive. */
  )
{
    return (0);
}


/*----------------------------------------------------------------------*
                         rtp_net_setmcastttl
 *----------------------------------------------------------------------*/
/** @memo   Set multicast time to live option.

    @doc    Sets the TTL value associated with IP multicast 
    traffic on the socket.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_setmcastttl (
  RTP_HANDLE sockHandle,                /** Socket handle of the socket to manipulate. */
  int ttl                               /** Time to live value. */
  )
{
    rtp_not_yet_implemented();
    return (-1);
}


/*----------------------------------------------------------------------*
                         rtp_net_setbroadcast
 *----------------------------------------------------------------------*/
/** @memo   Set socket permission to allow/disallow broadcasts.

    @doc    Set socket permission to allow/disallow broadcasts 
    over this socket.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native TCP/IP 
    layer, turn on RTP_DEBUG in rtpnet.c to display the 
    native error value.
 */
int rtp_net_setbroadcast (
  RTP_SOCKET sockHandle,                /** Socket handle of the socket to manipulate. */
  unsigned int onBool                   /** 1 to permit broadcasts, 0 to disallow */
  )
{
	int one = 1;
	return (NU_Setsockopt (sockHandle, SOL_SOCKET, SO_BROADCAST, (char *) &one, sizeof (int)));
}


/*----------------------------------------------------------------------*
                            rtp_net_htons
 *----------------------------------------------------------------------*/
/** @memo   Convert a short integer from host to network format.

    @doc    On a little endian system the short will be flipped.

    @return The converted short int.
 */
short rtp_net_htons (
  short i                               /** The short integer to be converted. */
  )
{
    rtp_not_yet_implemented();
	return (-1);
}


/*----------------------------------------------------------------------*
                            rtp_net_ntohs
 *----------------------------------------------------------------------*/
/** @memo   Convert a short integer from network to host format.

    @doc    On a little endian system the short will be flipped.

    @return The converted short int.
 */
short rtp_net_ntohs (
  short i                               /** The short integer to be converted. */
  )
{
    rtp_not_yet_implemented();
	return (-1);
}


/*----------------------------------------------------------------------*
                            rtp_net_htonl
 *----------------------------------------------------------------------*/
/** @memo   Convert a long integer from host to network format.

    @doc    On a little endian system the long will be flipped.

    @return The converted long int.
 */
long rtp_net_htonl (
  long i                                /** The long to be converted. */
  )
{
    rtp_not_yet_implemented();
	return (-1);
}


/*----------------------------------------------------------------------*
                            rtp_net_ntohl
 *----------------------------------------------------------------------*/
/** @memo   Convert a long integer from network to host format.

    @doc    On a little endian system the long will be flipped.

    @return The converted long int.
 */
long rtp_net_ntohl (
  long i                                /** The long to be converted. */
  )
{
    rtp_not_yet_implemented();
	return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_net_ip_to_str
 *----------------------------------------------------------------------*/
/** @memo   Print an IP address to a dotted decimal string.

    @doc    Extracts an IP address and creates a dotted decimal 
    string representation.  The “str” must be large enough to 
    contain the IP address.  If it is not, the resulting operation 
    is undefined.<br><br>

    <pre>
|		{111,111,111,111} converts to "111.111.111.111"
|		{111,111,111,0}   converts to "111.111.111.0"
|		{111,111,0,0}     converts to "111.111.0.0"
|		{111,0,0,0}       converts to "111.0.0.0"
	</pre>

    @return The length of the string representation, -1 on failure.
 */
int rtp_net_ip_to_str (
  char *str,                            /** The storage location for the dotted string representation. */
  unsigned char *ipAddr,                /** The IP address to be extracted. */
  int type                              /** Type of IP address:<br>
	<pre>
|		4   Indicates an IPv4 address.
|		6   Indicates an IPv6 address.
	</pre> */
  )
{
    rtp_not_yet_implemented();
	return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_net_str_to_ip
 *----------------------------------------------------------------------*/
/** @memo   Extract an IP address from a dotted decimal string.

    @doc    Extracts an IP address from a string and stores it in 
    ipAddr. The ipAddr must be large enough to contain the IP address. 
    If it is not, the resulting operation is undefined.  For an IPv4 
    address, this must be at least a four element unsigned char 
    array.  For an IPv6 address, this must be at least a six 
    element unsigned char array.<br><br>

    <pre>
|		"111.111.111.111" converts to {111,111,111,111}
|		"111.111.111"     converts to {111,111,111,0}
|		"111.111"         converts to {111,111,0,0}
|		"111"             converts to {111,0,0,0}
	</pre>

    @return 0 on success, -1 on error.
 */
int rtp_net_str_to_ip (
  unsigned char *ipAddr,                /** The storage location for the IP address to be extracted. */
  char *str,                            /** The dotted decimal string representation. */
  int *type                             /** Type of IP address:<br>
	<pre>
|		4   Indicates an IPv4 address.
|		6   Indicates an IPv6 address.
	</pre> */
  )
{
    rtp_not_yet_implemented();
	return (-1);
}


/*----------------------------------------------------------------------*
                                rtp_fd_zero
 *----------------------------------------------------------------------*/
/** @memo   Reset the socket handle list.

    @doc    Resets the socket handle list that is used by 
    rtp_net_select.

    @precondition <b>Support for this functionality is not needed 
    unless rtp_net_select is to be used.</b>

    @return void
 */
void rtp_fd_zero (
  RTP_FD_SET  *list                     /** The list to reset. */
  )
{
 	NU_FD_Init((FD_SET*) list);
}


/*----------------------------------------------------------------------*
                                rtp_fd_set
 *----------------------------------------------------------------------*/
/** @memo   Add a socket handle to a list.

    @doc    Adds a socket handle to the list that is used by 
    rtp_net_select.

    @precondition <b>Support for this functionality is not needed 
    unless rtp_net_select is to be used.</b>

    @return void
 */
void rtp_fd_set (
  RTP_FD_SET  *list,                    /** The list to add to. */
  RTP_HANDLE fd                         /** The handle to add to the list. */
  )
{
	NU_FD_Set(fd, (FD_SET*) list);
}


/*----------------------------------------------------------------------*
                                rtp_fd_clr
 *----------------------------------------------------------------------*/
/** @memo   Remove a socket handle from a list.

    @doc    Removes a socket handle from a list that is used by 
    rtp_net_select.

    @precondition <b>Support for this functionality is not needed 
    unless rtp_net_select is to be used.</b>

    @return void
 */
void rtp_fd_clr (
  RTP_FD_SET  *list,                    /** The list to remove from. */
  RTP_HANDLE fd                         /** The handle to remove from the list. */
  )
{
}


/*----------------------------------------------------------------------*
                               rtp_fd_isset
 *----------------------------------------------------------------------*/
/** @memo   Check if a socket handle is in a list.

    @doc    Checks if a socket handle is in a list that is used by 
    rtp_net_select.

    @precondition <b>Support for this functionality is not needed 
    unless rtp_net_select is to be used.</b>

    @return 1 if found, 0 on failure.
 */
int rtp_fd_isset (
  RTP_FD_SET  *list,                    /** The list to check. */
  RTP_HANDLE fd                         /** The handle to find. */
  )
{
	return NU_FD_Check(fd, (FD_SET*) list);
}


/*----------------------------------------------------------------------*
                              rtp_net_select
 *----------------------------------------------------------------------*/
/** @memo   Select ready sockets.

    @doc    This function is called to obtain the number of sockets that are 
    contained in the RTP_FD_SET lists that meet the condition before 
    the timeout period is up.  This function only needs to be 
    implemented if selecting on multiple sockets at a time and/or use 
    of the errorList is desired (See also: rtp_net_read_select, 
    rtp_net_write_select).

    rtp_net_select provides a timeout capability for 
    rtp_net_connect, rtp_net_accept, rtp_net_recv, 
    rtp_net_recvfrom, rtp_net_send, and rtp_net_sendto.  
    Each of these functions block infinitely if the associated 
    socket is in blocking mode or they return immediately if the 
    socket is in non-blocking mode (rtp_net_setblocking).  
    rtp_net_select takes three socket lists and a timeout value 
    as inputs.  It blocks the select signal until either the 
    timeout period expires or the select criteria, as specified 
    by the lists, is met for at least one socket.

    If readList is setup, this will return when any socket in the 
    readList has data.  For stream-type (TCP) sockets are listening 
    (rtp_net_listen has been called and -2 has returned on the 
    non-blocking socket handle) rtp_net_select will return when a 
    connection has been established and rtp_net_accept will 
    succeed without blocking.

    If the writeList is setup, this will return when any non-blocking 
    stream-type (TCP) socket in the writeList has room in the output 
    window.  It will also return when a blocking stream-type (TCP) 
    socket has an empty output window.  If there are any connectionless 
    (UDP) sockets on the list, it will return immediately.

    If an error occurs on the a socket associated with the errorList, 
    rtp_net_select returns.

    The lists will be updated to contain only those socket handles that 
    have passed the above criteria.

    @precondition <b>Support for this functionality is not needed 
    unless full functions of rtp_net_select is needed.</b>
    
    @see rtp_net_read_select
    @see rtp_net_write_select

    @return Number of sockets meeting the criteria on success, 0 if 
    timed out, and -1 on failure.  For debugging purposes; if the 
    cause of the error is obtainable at the native TCP/IP layer, turn 
    on RTP_DEBUG in rtpnet.c to display the native error 
    value.
 */
int rtp_net_select (
  RTP_FD_SET  *readList,                /** (optional) A list of sockets to check for read access. */
  RTP_FD_SET  *writeList,               /** (optional) A list of sockets to check for write access. */
  RTP_FD_SET  *errorList,               /** (optional) A list of sockets to check for errors. */
  long msecTimeout                      /** Time period to wait in msec for the desired action:<br>
	<pre>
|		 0   To poll.
|		-1   To indicate use of infinite.
	</pre> */
  )
{
    int selectTime;
    int result;

	// NET doesn't use this parameter; so we need to manually clear the 
	//  error set here.
	NU_FD_Init((FD_SET*) errorList);

	if (writeList && !readList)
	{
		// Nucleus NET doesn't support select on write; there is no non-blocking
		//  connect/send support
		return 1;
	}

    if (msecTimeout >= 0)
    {
    	selectTime = (msecTimeout+9) / (1000 / NU_PLUS_Ticks_Per_Second);
    	result = NU_Select(NSOCKETS, (FD_SET *) readList, (FD_SET *) writeList, (FD_SET *) errorList, selectTime);
	}
	else
	{
		result = NU_Select(NSOCKETS, (FD_SET *) readList, (FD_SET *) writeList, (FD_SET *) errorList, 0);
	}
	
	if ((result == NU_WOULD_BLOCK) || (result == NU_NO_DATA))
	{
		return 0;
	}

    if (result != NU_SUCCESS)
    {
        return -1;
    }    
    
    return 1;
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
