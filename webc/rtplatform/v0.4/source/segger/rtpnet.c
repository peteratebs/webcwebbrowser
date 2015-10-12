 /*
 | RTPNET.C - Runtime Platform Network Services
 |
 |   PORTED TO THE SEGGER embOS Platform
 |
 | EBSnet - RT-Platform 
 |
 |
 | Copyright EBSnet Inc. , 2003-2005
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
#include "rtpdebug.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Global.h>
#include <IP.h>
#include <IP_socket.h>

/************************************************************************
* Defines
************************************************************************/
#ifdef RTP_FD_SET_MAX
#undef RTP_FD_SET_MAX
#endif
#define RTP_FD_SET_MAX      12

/************************************************************************
* Compile Time Possible Porting Errors
************************************************************************/
#if (RTP_FD_SET_MAX > FD_SETSIZE)
#error RTP_FD_SET_MAX SHOULD NEVER BE LARGER THAN THE NATIVE FD_SETSIZE
#error         Adjustments should be made to RTP_FD_SET_MAX in rtpnet.h
#endif

#define fd_set IP_fd_set

// always wait for the interface to become ready
#define DO_IFACE_CHECK 1

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/

int rtpnetOpenSockets = 0;

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/
void _fd_set_to_rtp (RTP_FD_SET *rtp, fd_set *set);
void _rtp_to_fd_set (fd_set *set, RTP_FD_SET *rtp);

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                              rtp_net_init
 *----------------------------------------------------------------------*/
int rtp_net_init (void)
{
	return (0);
}


/*----------------------------------------------------------------------*
                              rtp_net_exit
 *----------------------------------------------------------------------*/
void rtp_net_exit (void)
{
}


/*----------------------------------------------------------------------*
                         rtp_net_socket_stream
 *----------------------------------------------------------------------*/
int rtp_net_socket_stream (RTP_HANDLE  *sockHandle)
{
    int sock;

    sock = socket(PF_INET, SOCK_STREAM, 0);

    if (sock == -1)
    {
		int errVal = _rtp_get_last_socket_error(sockHandle);
	  
        *sockHandle = ((RTP_HANDLE)-1);
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_socket_stream: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *sockHandle = (RTP_HANDLE) sock;

	rtpnetOpenSockets++;
    
    return (0);   
}

/*----------------------------------------------------------------------*
                        rtp_net_socket_datagram
 *----------------------------------------------------------------------*/
int rtp_net_socket_datagram (RTP_HANDLE  *sockHandle)
{
    int sock;
    
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    
    if (sock == -1)
    {
		int errVal = _rtp_get_last_socket_error(sockHandle);
	  
        *sockHandle = ((RTP_HANDLE)-1);
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_socket_datagram: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *sockHandle = (RTP_HANDLE) sock;
 
 	rtpnetOpenSockets++;
   
    return (0);
}


/*----------------------------------------------------------------------*
                            rtp_net_bind
 *----------------------------------------------------------------------*/
int rtp_net_bind (RTP_HANDLE sockHandle, unsigned char *ipAddr, int port, int type)
{
    struct sockaddr_in sin;
    unsigned long in_addr = 0;

    memset(&sin, 0, sizeof (sin));

    if (ipAddr)
    {
        unsigned char *ptr = (unsigned char *) &in_addr;

        ptr[0] = ipAddr[0];
        ptr[1] = ipAddr[1];
        ptr[2] = ipAddr[2];
        ptr[3] = ipAddr[3];

        /* ----------------------------------- */
        /* RTP_NET_TYPE_IPV6 not yet supported */
        /* ----------------------------------- */
    }
    else
    {
        in_addr = INADDR_ANY;
    }

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = in_addr;
    sin.sin_port = htons((unsigned short)port);

    if (bind ((int) sockHandle, (struct sockaddr *) &sin, sizeof (sin)) != 0)
    {
		int errVal = _rtp_get_last_socket_error(sockHandle);
	  
        if (errVal == EINVAL)
        {
#ifdef RTP_DEBUG
            RTP_DEBUG_OUTPUT_STR("rtp_net_bind: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(errVal);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
        else
        {
#ifdef RTP_DEBUG
        	RTP_DEBUG_OUTPUT_STR("rtp_net_bind: error returned ");
        	RTP_DEBUG_OUTPUT_INT(errVal);
        	RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        	return (-1);
        }
    }

    return (0);
}


/*----------------------------------------------------------------------*
                            rtp_net_listen
 *----------------------------------------------------------------------*/
int rtp_net_listen (RTP_HANDLE sockHandle, int queueSize)
{
    if (listen((int) sockHandle, queueSize) != 0)
    {
		int errVal = _rtp_get_last_socket_error(sockHandle);

#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_listen: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    return (0);
}


/*----------------------------------------------------------------------*
                          rtp_net_getpeername
 *----------------------------------------------------------------------*/
int rtp_net_getpeername (RTP_HANDLE sockHandle, unsigned char *ipAddr, int *port, int *type)
{
    struct sockaddr_in peerAddr;
    int peerLen;

    peerLen = sizeof(peerAddr);
    memset(&peerAddr, 0, peerLen);

    if (getpeername((int) sockHandle, (struct sockaddr *) &peerAddr, &peerLen) != 0)
    {
		int errVal = _rtp_get_last_socket_error(sockHandle);
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_getpeername: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
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
        *port = ntohs(peerAddr.sin_port);
    }

    return (0);
}


/*----------------------------------------------------------------------*
                          rtp_net_getsockname
 *----------------------------------------------------------------------*/
int rtp_net_getsockname (RTP_HANDLE sockHandle, unsigned char *ipAddr, int *port, int *type)
{
    struct sockaddr_in localAddr;
    int localLen;

    localLen = sizeof(localAddr);
    memset(&localAddr, 0, localLen);

    if (getsockname ((int) sockHandle, (struct sockaddr *) &localAddr, &localLen) != 0)
    {
		int errVal = _rtp_get_last_socket_error(sockHandle);
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_getsockname: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
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
        *port = ntohs(localAddr.sin_port);
    }

    return (0);
}


/*----------------------------------------------------------------------*
                          rtp_net_gethostbyname
 *----------------------------------------------------------------------*/
int rtp_net_gethostbyname (unsigned char *ipAddr, int *type, char *name)
{
    struct hostent* hp = NULL;

#ifdef DO_IFACE_CHECK
	while (IP_IFaceIsReady() == 0) {
	OS_Delay(100);
	}
#endif
	
    if (!(hp = gethostbyname((char *)name)))
    {
		//int errVal = _rtp_get_last_socket_error(sockHandle);
	  
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_gethostbyname: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    if (hp->h_addrtype != AF_INET)
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


/*----------------------------------------------------------------------*
                           rtp_net_accept
 *----------------------------------------------------------------------*/
int rtp_net_accept (RTP_HANDLE *connectSock, RTP_HANDLE serverSock, 
                    unsigned char *ipAddr, int *port, int *type)
{
    struct sockaddr_in clientAddr;
    int clientLen;
    int conSocket;

    clientLen = sizeof(clientAddr);
    memset(&clientAddr, 0, clientLen);

#ifdef DO_IFACE_CHECK
	while (IP_IFaceIsReady() == 0) {
	OS_Delay(100);
	}
#endif
	
    conSocket = accept((int) serverSock, (struct sockaddr *) &clientAddr, &clientLen);

    if (conSocket == -1)
    {
		int errVal = _rtp_get_last_socket_error(serverSock);
	  
        *connectSock = ((RTP_HANDLE)-1);
        /* The man page for accept(2) indicates that due to Linux
           passing already-pending errors through accept, the following
           TCP/IP errors should be treated as EAGAIN:
           ENETDOWN, EPROTO, ENOPROTOOPT, EHOSTDOWN, ENONET,
           EHOSTUNREACH, EOPNOTSUPP, and ENETUNREACH. */
        if (//(errVal == EAGAIN) || 
            (errVal == EWOULDBLOCK) ||
            //(errVal == EINTR) ||
            //(errVal == ENETDOWN) ||
            //(errVal == EPROTO) ||
            (errVal == ENOPROTOOPT) ||
            //(errVal == EHOSTDOWN) ||
            //(errVal == ENONET) ||
            //(errVal == EHOSTUNREACH) ||
            (errVal == EOPNOTSUPP) ||
            (errVal == ENETUNREACH))
        {
#ifdef RTP_DEBUG
            RTP_DEBUG_OUTPUT_STR("rtp_net_accept: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(errVal);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_accept: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
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
        *port = ntohs(clientAddr.sin_port);       
    }
    
    return (0);
}


/*----------------------------------------------------------------------*
                          rtp_net_connect
 *----------------------------------------------------------------------*/
int rtp_net_connect (RTP_HANDLE sockHandle, 
                     unsigned char *ipAddr, 
                     int port, int type)
{
    int sinLen;
    struct sockaddr_in sin;
    unsigned long in_addr = 0;
	int connectStatus;

    sinLen = sizeof(sin);
    memset(&sin, 0, sinLen);

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

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = in_addr;
    sin.sin_port = htons((unsigned short)port);

#ifdef DO_IFACE_CHECK
	while (IP_IFaceIsReady() == 0) {
	OS_Delay(100);
	}
#endif
	
	connectStatus = connect((int) sockHandle, ( struct sockaddr *) &sin, sinLen);
    if (connectStatus != 0)
    {
	  int errVal = _rtp_get_last_socket_error(sockHandle);
	  
        if ((errVal == EINPROGRESS) || 
            (errVal == EWOULDBLOCK) ||
            (errVal == EALREADY) ||
            (errVal == EISCONN))
        {
#ifdef RTP_DEBUG
            RTP_DEBUG_OUTPUT_STR("rtp_net_connect: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(errVal);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_connect: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    return (0);
}

/*----------------------------------------------------------------------*
                         rtp_net_is_connected
 *----------------------------------------------------------------------*/
unsigned  rtp_net_is_connected    (RTP_SOCKET sockHandle)
{
    struct sockaddr_in peerAddr;
    int peerLen;

#ifdef DO_IFACE_CHECK
	while (IP_IFaceIsReady() == 0) {
	OS_Delay(100);
	}
#endif
    
    peerLen = sizeof(peerAddr);
    memset(&peerAddr, 0, peerLen);

    if (getpeername ((int) sockHandle, (struct sockaddr *) &peerAddr, &peerLen) == 0)
	{
		/* this is necessary but not sufficient; now check to make sure the other 
		   side hasn't shutdown sending data to us */
		fd_set tempSet;
		long selectTime=0;
		int result;

		IP_FD_ZERO(&tempSet);
		IP_FD_SET((int) sockHandle, &tempSet);
		
		// check the socket for ready-to-read
		result = select(&tempSet, 0, 0, selectTime);
		if (result != -1)
		{
			unsigned char tempBuffer[1];

			if (!IP_FD_ISSET((int) sockHandle, &tempSet))
			{
				// if we would block, then there is no problem; if the other
				//  side has shut down its end of the connection, we would
				//  return immediately with an error code

				return (1);
			}
			
			// find out whether this means that:
			//  1. there is data in the buffer from the other side
			//  2. the connection has been closed

			result = recv ((int) sockHandle, tempBuffer, 1, MSG_PEEK);
			if (result == 1)
			{
				// there is data
				return (1);
			}

			if (result == -1)
			{
				return (1);
			}
		}
	}	

	return (0);
}

/*----------------------------------------------------------------------*
                        rtp_net_write_select
 *----------------------------------------------------------------------*/
int rtp_net_write_select (RTP_HANDLE sockHandle, long msecTimeout)
{
#ifdef LINUXTOBEIMPLEMENTED
long selectTime;
fd_set write_set;
int result;
    
    /* ----------------------------------- */
    /*              write list             */
    /* ----------------------------------- */
	IP_FD_ZERO(&write_set);
    IP_FD_SET((SOCKET) sockHandle, &write_set);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError(0);
#endif

	if (msecTimeout >= 0)
    {
        selectTime = msecTimeout / 1000;
		result = select(1, (fd_set *) 0, (fd_set *) &write_set, (fd_set *) 0, selectTime);
    }
	else
	{
		result = select(1, (fd_set *) 0, (fd_set *) &write_set, (fd_set *) 0, 0);
	}

    /* if an error or if it timed out */
    if ((result == SOCKET_ERROR) || (result == 0))
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_write_select: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    return (0);
#else
	return (0);
#endif
}


/*----------------------------------------------------------------------*
                        rtp_net_read_select
 *----------------------------------------------------------------------*/
int rtp_net_read_select (RTP_HANDLE sockHandle, long msecTimeout)
{
#ifdef LINUXTOBEIMPLEMENTED
long selectTime;
fd_set read_set;
int result;
    
    /* ----------------------------------- */
    /*              write list             */
    /* ----------------------------------- */
	IP_FD_ZERO(&read_set);
    IP_FD_SET((SOCKET) sockHandle, &read_set);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError(0);
#endif

	if (msecTimeout >= 0)
    {
        selectTime = msecTimeout / 1000;
		result = select(1, (fd_set *) &read_set, (fd_set *) 0, (fd_set *) 0,  selectTime);
    }
	else
	{
		result = select(1, (fd_set *) &read_set, (fd_set *) 0, (fd_set *) 0,  0);
	}

    /* if an error or if it timed out */
    if ((result == SOCKET_ERROR) || (result == 0))
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_read_select: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    return (0);
#else
	return (0);
#endif
}


/*----------------------------------------------------------------------*
                           rtp_net_send
 *----------------------------------------------------------------------*/
long rtp_net_send (RTP_HANDLE sockHandle, const unsigned char * buffer, long size)
{
    long result;
 
#ifdef DO_IFACE_CHECK
	while (IP_IFaceIsReady() == 0) {
	OS_Delay(100);
	}
#endif

    result = send((int) sockHandle, (const char *) buffer, (int) size, 0);
    
    if (result == -1)
    {
	  int errVal = _rtp_get_last_socket_error(sockHandle);

	  if (errVal == EWOULDBLOCK)
        {
#ifdef RTP_DEBUG
            RTP_DEBUG_OUTPUT_STR("rtp_net_send: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(errVal);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_send: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return ((long) result);
}


/*----------------------------------------------------------------------*
                           rtp_net_recv
 *----------------------------------------------------------------------*/
long rtp_net_recv (RTP_HANDLE sockHandle, unsigned char * buffer, long size)
{
    long result;

#ifdef DO_IFACE_CHECK
	while (IP_IFaceIsReady() == 0) {
	OS_Delay(100);
	}
#endif
	
    result = recv((int) sockHandle, (char *) buffer, (int) size, 0);
    
    if (result == -1)
    {
	  int errVal = _rtp_get_last_socket_error(sockHandle);
	  
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_recv: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return ((long) result);
}


/*----------------------------------------------------------------------*
                           rtp_net_sendto
 *----------------------------------------------------------------------*/
long rtp_net_sendto (RTP_HANDLE sockHandle, 
                     const unsigned char * buffer, long size,
                     unsigned char * ipAddr, int port, int type)
{
    long result;
    int  sinLen;
    struct sockaddr_in sin;
    unsigned long in_addr = 0;
    
    sinLen = sizeof(sin);
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
    
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = in_addr;
    sin.sin_port = htons((unsigned short)port);

    result = sendto((int) sockHandle, (const void *) buffer, (size_t) size, 0, (struct sockaddr *)&sin, sinLen);
    
    if (result == -1)
    {
	  int errVal = _rtp_get_last_socket_error(sockHandle);
	  
        if (errVal == EWOULDBLOCK)
        {
#ifdef RTP_DEBUG
            RTP_DEBUG_OUTPUT_STR("rtp_net_sendto: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(errVal);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_sendto: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return ((long) result);
}


/*----------------------------------------------------------------------*
                           rtp_net_recvfrom
 *----------------------------------------------------------------------*/
long rtp_net_recvfrom (RTP_HANDLE sockHandle, 
                       unsigned char *buffer, long size,
                       unsigned char *ipAddr, int *port, int *type)
{
    long result;
    int remoteLen;
    struct sockaddr_in remote;
    
    remoteLen = sizeof(remote);
    memset(&remote, 0, remoteLen);

    result = recvfrom((int) sockHandle, (char *) buffer, (int) size, 0, (struct sockaddr *) &remote, (int *) &remoteLen);

    if (result == -1)
    {
	  int errVal = _rtp_get_last_socket_error(sockHandle);
	  
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_recvfrom: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
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
        *port = ntohs(remote.sin_port);
    }
    
    return ((long) result);
}


/*----------------------------------------------------------------------*
                          rtp_net_closesocket
 *----------------------------------------------------------------------*/
int rtp_net_closesocket (RTP_HANDLE sockHandle)
{
    if (socketclose((int) sockHandle) == -1)
    {
	  	  int errVal = _rtp_get_last_socket_error(sockHandle);

#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_closesocket: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

	rtpnetOpenSockets--;
    
    return (0);
}


/*----------------------------------------------------------------------*
                            rtp_net_shutdown
 *----------------------------------------------------------------------*/
int rtp_net_shutdown (RTP_HANDLE sockHandle, int how)
{
#ifdef LINUXTOBEIMPLEMENTED
    int result;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

    if (how == 0)
    {
        how = SD_RECEIVE;
    }
    else if (how == 1)
    {
        how = SD_SEND;
    }
    else
    {
        how = SD_BOTH;
    }

    result = shutdown((SOCKET) sockHandle, how);
    if (result == SOCKET_ERROR)
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_shutdown: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
#else
	return (0);
#endif
}


/*----------------------------------------------------------------------*
                           rtp_net_getntoread
 *----------------------------------------------------------------------*/
int rtp_net_getntoread (RTP_HANDLE sockHandle, unsigned long * nToRead)
{
#ifdef LINUXTOBEIMPLEMENTED
    u_long arg;

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

    if (ioctlsocket((SOCKET) sockHandle, FIONREAD, (u_long *) &arg) == SOCKET_ERROR)
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_getntoread: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    *nToRead = arg;
    return (0);
#else
	return (0);
#endif
}


/*----------------------------------------------------------------------*
                           rtp_net_setblocking
 *----------------------------------------------------------------------*/
int rtp_net_setblocking (RTP_HANDLE sockHandle, unsigned int onBool)
{
  int arg;

  arg = (int)(!onBool);
  // if (ioctl((int) sockHandle, FIONBIO, &arg) < 0)
  if (setsockopt((int) sockHandle, SOL_SOCKET, SO_NONBLOCK, &arg, sizeof(arg)) < 0)

  
  {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_setblocking: error returned ");
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------*
                            rtp_net_setnagle
 *----------------------------------------------------------------------*/
int rtp_net_setnagle (RTP_HANDLE sockHandle, unsigned int onBool)
{
#ifdef LINUXTOBEIMPLEMENTED
    int option;

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

    option = (int)(!onBool);
    if ( setsockopt((SOCKET) sockHandle, IPPROTO_TCP, TCP_NODELAY, (char *) &option, sizeof (int)) == SOCKET_ERROR )
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_setnagle: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
#else
	return (0);
#endif
}


/*----------------------------------------------------------------------*
                          rtp_net_setlinger
 *----------------------------------------------------------------------*/
int rtp_net_setlinger (RTP_HANDLE sockHandle, unsigned int onBool, long msecTimeout)
{
#ifdef LINUXTOBEIMPLEMENTED
    LINGER arg;
#ifdef RTP_DEBUG
    int result;
#endif

    arg.l_onoff = onBool;
	arg.l_linger = 0;
    if (arg.l_onoff)
    {
	    if (msecTimeout > 0)
	    {
	        arg.l_linger = (u_short) (msecTimeout / 1000);
	    }
    }

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

    if (setsockopt((SOCKET) sockHandle, SOL_SOCKET,
                   SO_LINGER, (const char *) &arg, 
                   sizeof (struct linger)) < 0)
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_setlinger: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
#else
	return (0);
#endif
}


/*----------------------------------------------------------------------*
                           rtp_net_setreusesock
 *----------------------------------------------------------------------*/
int rtp_net_setreusesock (RTP_HANDLE sockHandle, unsigned int onBool)
{
#ifdef LINUXTOBEIMPLEMENTED
    /* ----------------------------------- */
    /*  Not supported in windows, but also */
    /*  not required in windows.  Return   */
    /*  success.                           */
    /* ----------------------------------- */
    return (0);
#else
	return (0);
#endif
}


/*----------------------------------------------------------------------*
                           rtp_net_setreuseaddr
 *----------------------------------------------------------------------*/
int rtp_net_setreuseaddr (RTP_HANDLE sockHandle, unsigned int onBool)
{
#ifdef LINUXTOBEIMPLEMENTED
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

    if ( setsockopt((SOCKET) sockHandle, SOL_SOCKET, SO_REUSEADDR, (char *) &onBool, sizeof (int)) == SOCKET_ERROR )
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_setreuseaddr: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
#else
	return (0);
#endif
}

/*----------------------------------------------------------------------*
                           rtp_net_settcpnocopy
 *----------------------------------------------------------------------*/
int rtp_net_settcpnocopy (RTP_HANDLE sockHandle, unsigned int onBool)
{
#ifdef LINUXTOBEIMPLEMENTED
    /* ----------------------------------- */
    /*  Not supported in windows, but also */
    /*  not required in windows.  Return   */
    /*  success.                           */
    /* ----------------------------------- */
    return (0);
#else
	return (0);
#endif
}


/*----------------------------------------------------------------------*
                         rtp_net_setkeepalive
 *----------------------------------------------------------------------*/
int rtp_net_setkeepalive (RTP_HANDLE sockHandle, unsigned int onBool)
{
#ifdef LINUXTOBEIMPLEMENTED
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

    if ( setsockopt((SOCKET) sockHandle, SOL_SOCKET, SO_KEEPALIVE, (char *) &onBool, sizeof (int)) == SOCKET_ERROR )
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_setkeepalive: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
#else
	return (0);
#endif
}


/*----------------------------------------------------------------------*
                         rtp_net_setmembership
 *----------------------------------------------------------------------*/
int rtp_net_setmembership (RTP_HANDLE sockHandle, unsigned char * ipAddr, int type, unsigned int onBool)
{
#ifdef LINUXTOBEIMPLEMENTED
    struct sockaddr_in localAddr;
    int result;
    int localLen;
    struct ip_mreq mcreq;
    unsigned long in_addr = 0;
    
    localLen = sizeof(localAddr);
    memset(&localAddr, 0, localLen);
    memset( ( void * )&mcreq, 0, sizeof( struct ip_mreq ) );
    
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

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

    if (getsockname ((SOCKET) sockHandle, (struct sockaddr *) &localAddr, (int *) &localLen) != 0)
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_setmembership: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    memmove(&mcreq.imr_multiaddr.s_addr, &in_addr, 4);
    memmove(&mcreq.imr_interface.s_addr, &localAddr.sin_addr.s_addr, 4); 

    if (onBool)
    {
        result = setsockopt((int) sockHandle, IPPROTO_IP, 
                            IP_ADD_MEMBERSHIP, (const char *)&mcreq, 
                            sizeof( struct ip_mreq ));
    }
    else
    {
        result = setsockopt((int) sockHandle, IPPROTO_IP, 
                            IP_DROP_MEMBERSHIP, (const char *)&mcreq, 
                            sizeof( struct ip_mreq ));
    }
        
    if (result != 0)
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_setmembership: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
#else
	return (0);
#endif
}


/*----------------------------------------------------------------------*
                         rtp_net_setmcastttl
 *----------------------------------------------------------------------*/
int  rtp_net_setmcastttl(RTP_HANDLE sockHandle, int ttl)
{
#ifdef LINUXTOBEIMPLEMENTED
    int result;
 
#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

    result = setsockopt( sockHandle, IPPROTO_IP,
                IP_MULTICAST_TTL, (char *) &ttl, sizeof (int));
    if (result != 0)
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_setmembership: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
#else
	return (0);
#endif
}    

/*----------------------------------------------------------------------*
                           rtp_net_setbroadcast
 *----------------------------------------------------------------------*/
int rtp_net_setbroadcast (RTP_HANDLE sockHandle, unsigned int onBool)
{

    if (setsockopt((int) sockHandle, SOL_SOCKET, SO_BROADCAST, (char *) &onBool, sizeof (int)) == -1)
    {
		int errVal = _rtp_get_last_socket_error(sockHandle);

#if (RTP_DEBUG)
        RTP_DEBUG_OUTPUT_STR("rtp_net_setbroadcast: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}



/*----------------------------------------------------------------------*
                            rtp_net_htons
 *----------------------------------------------------------------------*/
short rtp_net_htons (short i)
{
    return (htons(i));
}


/*----------------------------------------------------------------------*
                            rtp_net_ntohs
 *----------------------------------------------------------------------*/
short rtp_net_ntohs (short i)
{
    return (ntohs(i));
}


/*----------------------------------------------------------------------*
                            rtp_net_htonl
 *----------------------------------------------------------------------*/
long rtp_net_htonl (long i)
{
    return (htonl(i));
}


/*----------------------------------------------------------------------*
                            rtp_net_ntohl
 *----------------------------------------------------------------------*/
long rtp_net_ntohl (long i)
{
    return (ntohl(i));
}


/*----------------------------------------------------------------------*
                           rtp_net_ip_to_str
 *----------------------------------------------------------------------*/
int rtp_net_ip_to_str (char *str, unsigned char *ipAddr, int type)
{
    str[0] = '\0';
    if (type != RTP_NET_TYPE_IPV4)
    {
        return (-1);
    }

    sprintf(str, "%i.%i.%i.%i", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);

    return (strlen((const char *) str));
}


/*----------------------------------------------------------------------*
                           rtp_net_str_to_ip
 *----------------------------------------------------------------------*/
int rtp_net_str_to_ip (unsigned char *ipAddr, char *str, int *type)
{
    int nbytes;
	int n, i;

    *type = RTP_NET_TYPE_IPV4;
    nbytes = 4;

	memset(ipAddr, 0, nbytes);

	for (n=0, i=0; str[n] && i<nbytes; n++)
	{
		if (str[n] >= '0' && str[n] <= '9')
		{
			ipAddr[i] = (ipAddr[i] * 10) + (str[n] - '0');
		}
		else if (str[n] == '.')
		{
			if (n == 0)
			{
				return (-1);
			}
			i++;
		}
		else
		{
			if (i == 0)
			{
				return (-1);
			}
			break;
		}
    }
    
    return (0);
}


/*----------------------------------------------------------------------*
                                rtp_fd_zero
 *----------------------------------------------------------------------*/
void rtp_fd_zero (RTP_FD_SET  *list)
{
    list->fdCount = 0;
}


/*----------------------------------------------------------------------*
                                rtp_fd_set
 *----------------------------------------------------------------------*/
void rtp_fd_set (RTP_FD_SET  *list, RTP_HANDLE fd)
{
int limit;


    limit = (int) (RTP_FD_SET_MAX > FD_SETSIZE ? FD_SETSIZE : RTP_FD_SET_MAX);
    
    if (list->fdCount < limit)
    {
        if (!rtp_fd_isset(list, fd))
        {
            list->fdArray[list->fdCount] = fd;
            list->fdCount++;
        }
    }
}


/*----------------------------------------------------------------------*
                                rtp_fd_clr
 *----------------------------------------------------------------------*/
void rtp_fd_clr (RTP_FD_SET  *list, RTP_HANDLE fd)
{
int n;
int limit;


    limit = (int) (RTP_FD_SET_MAX > FD_SETSIZE ? FD_SETSIZE : RTP_FD_SET_MAX);
    
    if (list->fdCount > limit)
    {
        list->fdCount = limit;
    }

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


/*----------------------------------------------------------------------*
                               rtp_fd_isset
 *----------------------------------------------------------------------*/
int rtp_fd_isset (RTP_FD_SET  *list, RTP_HANDLE fd)
{
int n;
int limit;


    limit = (int) (RTP_FD_SET_MAX > FD_SETSIZE ? FD_SETSIZE : RTP_FD_SET_MAX);
    
    if (list->fdCount > limit)
    {
        list->fdCount = limit;
    }

    for (n = 0; n < list->fdCount; n++)
    {
        if (list->fdArray[n] == fd)
        {
            return (1);
        }
    }

    return (0);
}


/*----------------------------------------------------------------------*
                              rtp_net_select
 *----------------------------------------------------------------------*/
int rtp_net_select (RTP_FD_SET  *readList, 
                    RTP_FD_SET  *writeList,
                    RTP_FD_SET  *errorList, 
                    long msecTimeout)
{
    long selectTime;
    int result;
	int index;
    
    fd_set write_set;
    fd_set read_set;
    fd_set error_set;
    
	IP_FD_ZERO(&write_set);
	IP_FD_ZERO(&read_set);
	IP_FD_ZERO(&error_set);
	
    /* ----------------------------------- */
    /*              write list             */
    /* ----------------------------------- */
    if (writeList)
    {
		_rtp_to_fd_set(&write_set, writeList);
    }

    /* ----------------------------------- */
    /*               read list             */
    /* ----------------------------------- */
    if (readList)
    {
		_rtp_to_fd_set(&read_set, readList);
    }
    
    /* ----------------------------------- */
    /*              error list             */
    /* ----------------------------------- */
    if (errorList)
    {
		_rtp_to_fd_set(&error_set, errorList);
    }

	if (msecTimeout >= 0)
    {
        selectTime = msecTimeout / 1000; // seconds
		result = select((fd_set *) &read_set, (fd_set *) &write_set, (fd_set *) &error_set, selectTime);
    }
	else
	{
		result = select((fd_set *) &read_set, (fd_set *) &write_set, (fd_set *) &error_set,  0);
	}

    if (result >=0)
    {
        if (writeList)
        {
    		_fd_set_to_rtp(writeList, &write_set);
        }
    
        if (readList)
        {
    		_fd_set_to_rtp(readList, &read_set);
        }
        
        if (errorList)
        {
    		_fd_set_to_rtp(errorList, &error_set);
        }
    }
    
    if (result == -1)
    {
		//int errVal = _rtp_get_last_socket_error(sockHandle);

#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_select: error returned ");
        RTP_DEBUG_OUTPUT_INT(errVal);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    return (result);
}


void _fd_set_to_rtp (RTP_FD_SET *rtp, fd_set *set)
{
int index;

    rtp->fdCount = 0;

    /* This could definitely be optimized by making it less portable and using
       fd_set->__fds_bits. */
    for (index = 0; index < FD_SETSIZE && rtp->fdCount < RTP_FD_SET_MAX; index++)
    {
        if (IP_FD_ISSET (index, set))
        {
    		rtp->fdArray[rtp->fdCount] = index;
            rtp->fdCount++;
        }
    }
}

void _rtp_to_fd_set (fd_set *set, RTP_FD_SET *rtp)
{
int index;

    IP_FD_ZERO(set);

    for (index = 0; index < (signed) rtp->fdCount && index < FD_SETSIZE; index++)
    {
		IP_FD_SET((int) rtp->fdArray[index], set);
    }
}

int _rtp_get_last_socket_error(RTP_HANDLE sockHandle)
{
  int errorVal=0, status;
  status = getsockopt ( (long)sockHandle, SOL_SOCKET, SO_ERROR, &errorVal, sizeof(errorVal));
  return errorVal;
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
