 /*
 | RTPNET.C - Runtime Platform Network Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - RT-Platform
 |
 |  $Author: vmalaiya $
 |  $Date: 2006/03/24 00:03:19 $
 |  $Name:  $
 |  $Revision: 1.12 $
 |
 | Copyright EBS Inc. , 2006
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
#include <QTcpSocket.h>
#include <QUdpSocket.h>

/************************************************************************
* Defines
************************************************************************/
#define DEFAULT_FAMILY     PF_UNSPEC // Accept either IPv4 or IPv6
#define DEFAULT_SOCKTYPE   SOCK_STREAM // TCP

/************************************************************************
* Compile Time Possible Porting Errors
************************************************************************/
#if (RTP_FD_SET_MAX > FD_SETSIZE)
#error RTP_FD_SET_MAX SHOULD NEVER BE LARGER THAN THE NATIVE FD_SETSIZE
#error         Adjustments should be made to RTP_FD_SET_MAX in rtpnet.h
#endif

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/

int rtpnetOpenSockets = 0;
int rtpnetInitialized = 0;

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
    QTcpSocket *tcpSocket = new QTcpSocket();
    *sockHandle = (RTP_HANDLE) tcpSocket;
	rtpnetOpenSockets++;
    return (0);
}

/*----------------------------------------------------------------------*
                        rtp_net_socket_datagram
 *----------------------------------------------------------------------*/
int rtp_net_socket_datagram (RTP_HANDLE  *sockHandle)
{
    QUdpSocket *udpSocket = new QUdpSocket();
    *sockHandle = (RTP_HANDLE) udpSocket;
	rtpnetOpenSockets++;
    return (0);
}

/*----------------------------------------------------------------------*
                            rtp_net_bind
 *----------------------------------------------------------------------*/
int rtp_net_bind (RTP_HANDLE sockHandle, unsigned char *ipAddr, int port, int type)
{
    int result;
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

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);

    if (bind ((SOCKET) sockHandle, (const struct sockaddr *) &sin, sizeof (sin)) != 0)
    {
        result = WSAGetLastError();
        if (result == WSAEADDRINUSE)
        {
#ifdef RTP_DEBUG
            RTP_DEBUG_OUTPUT_STR("rtp_net_bind: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(result);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
        else
        {
#ifdef RTP_DEBUG
        	RTP_DEBUG_OUTPUT_STR("rtp_net_bind: error returned ");
        	RTP_DEBUG_OUTPUT_INT(result);
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
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError(0);
#endif

    if (listen((SOCKET) sockHandle, queueSize) != 0)
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_listen: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
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
    printf("FUNCTION NOT PORTED\n");
	return (0);
}


/*----------------------------------------------------------------------*
                          rtp_net_getsockname
 *----------------------------------------------------------------------*/
int rtp_net_getsockname (RTP_HANDLE sockHandle, unsigned char *ipAddr, int *port, int *type)
{
    struct sockaddr_storage localAddr;
    char addrName[NI_MAXHOST];
    int localLen;
#ifdef RTP_DEBUG
    int result;
#endif

    localLen = sizeof(localAddr);
    memset(&localAddr, 0, localLen);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

	if (getsockname ((SOCKET) sockHandle, (LPSOCKADDR)&localAddr, (int *) &localLen) != 0)
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_getsockname: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    if (getnameinfo( (LPSOCKADDR)&localAddr, localLen, addrName, sizeof(addrName), NULL, 0, NI_NUMERICHOST) != 0)
    {
            strcpy(addrName, "");
	}

	if (ipAddr)
    {
        rtp_net_str_to_ip (ipAddr, addrName, type);
    }

    if (port)
    {
        *port = ntohs(SS_PORT(&localAddr));
    }

    return (0);
}

/*----------------------------------------------------------------------*
                          rtp_net_gethostbyname
 *----------------------------------------------------------------------*/
int rtp_net_gethostbyname (unsigned char *ipAddr, int *type, char *name)
{
#ifdef RTP_DEBUG
    int result;
#endif
    struct hostent* hp = NULL;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

    if (!(hp = gethostbyname((const char *)name)))
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_gethostbyname: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
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
    struct sockaddr_storage clientAddr;
    socklen_t clientLen;
    SOCKET conSocket;
    int result;
    char clientHost[NI_MAXHOST];

    clientLen = sizeof(clientAddr);
    memset(&clientAddr, 0, clientLen);

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError(0);

    conSocket = accept((SOCKET) serverSock, (struct sockaddr *) &clientAddr, (int *) &clientLen);

    if (conSocket == INVALID_SOCKET)
    {
        *connectSock = ((RTP_HANDLE)-1);
        result = WSAGetLastError();
        if ((result == WSAEINPROGRESS) ||
            (result == WSAEWOULDBLOCK) ||
            (result == WSAEALREADY))
        {
#ifdef RTP_DEBUG
            RTP_DEBUG_OUTPUT_STR("rtp_net_accept: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(result);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_accept: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *connectSock = (RTP_HANDLE)conSocket;

    getnameinfo((struct sockaddr *)&clientAddr, clientLen,
                    clientHost, sizeof(clientHost),
                    0, 0, NI_NUMERICHOST);
    if (ipAddr)
    {
        rtp_net_str_to_ip (ipAddr, clientHost, type);
    }

    if (port)
    {
        *port = ntohs(SS_PORT(&clientAddr));
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
    int result;
    int sinLen;
    struct sockaddr_in sin;
    unsigned long in_addr = 0;

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

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);

    if (connect((SOCKET) sockHandle, (const struct sockaddr *) &sin, sinLen) != 0)
    {
        result = WSAGetLastError();
        if ((result == WSAEINPROGRESS) ||
            (result == WSAEWOULDBLOCK) ||
            (result == WSAEALREADY))
        {
#ifdef RTP_DEBUG
            RTP_DEBUG_OUTPUT_STR("rtp_net_connect: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(result);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_connect: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
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
    struct sockaddr_storage peerAddr;
    socklen_t peerLen;

    peerLen = sizeof(peerAddr);
    memset(&peerAddr, 0, peerLen);

    if (getpeername ((SOCKET) sockHandle, (struct sockaddr *) &peerAddr, &peerLen) == 0)
	{
		/* this is necessary but not sufficient; now check to make sure the other
		   side hasn't shutdown sending data to us */
		fd_set tempSet;
		struct timeval selectTime;
		int result;

        selectTime.tv_sec = 0;
        selectTime.tv_usec = 0;

		FD_ZERO(&tempSet);
		FD_SET((SOCKET) sockHandle, &tempSet);

		// check the socket for ready-to-read
		result = select(1, &tempSet, 0, 0, &selectTime);
		if (result != SOCKET_ERROR)
		{
			unsigned char tempBuffer[1];

			if (!FD_ISSET((SOCKET) sockHandle, &tempSet))
			{
				// if we would block, then there is no problem; if the other
				//  side has shut down its end of the connection, we would
				//  return immediately with an error code

				return (1);
			}

			// find out whether this means that:
			//  1. there is data in the buffer from the other side
			//  2. the connection has been closed

			result = recv ((SOCKET) sockHandle, tempBuffer, 1, MSG_PEEK);
			if (result == 1)
			{
				// there is data
				return (1);
			}

			if (result == SOCKET_ERROR)
			{
				switch (WSAGetLastError())
				{
					case WSAEINPROGRESS:
					case WSAEWOULDBLOCK:
						return (1);
				}
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
struct timeval selectTime;
fd_set write_set;
int result;

    /* ----------------------------------- */
    /*              write list             */
    /* ----------------------------------- */
	FD_ZERO(&write_set);
    FD_SET((SOCKET) sockHandle, &write_set);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError(0);
#endif

	if (msecTimeout >= 0)
    {
        selectTime.tv_sec = msecTimeout / 1000;
        selectTime.tv_usec = (msecTimeout % 1000) * 1000;
		result = select(1, (fd_set *) 0, (fd_set *) &write_set, (fd_set *) 0, (const struct timeval *) &selectTime);
    }
	else
	{
		result = select(1, (fd_set *) 0, (fd_set *) &write_set, (fd_set *) 0, (const struct timeval *) NULL);
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
}


/*----------------------------------------------------------------------*
                        rtp_net_read_select
 *----------------------------------------------------------------------*/
int rtp_net_read_select (RTP_HANDLE sockHandle, long msecTimeout)
{
struct timeval selectTime;
fd_set read_set;
int result;

    /* ----------------------------------- */
    /*              write list             */
    /* ----------------------------------- */
	FD_ZERO(&read_set);
    FD_SET((SOCKET) sockHandle, &read_set);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError(0);
#endif

	if (msecTimeout >= 0)
    {
        selectTime.tv_sec = msecTimeout / 1000;
        selectTime.tv_usec = (msecTimeout % 1000) * 1000;
		result = select(1, (fd_set *) &read_set, (fd_set *) 0, (fd_set *) 0, (const struct timeval *) &selectTime);
    }
	else
	{
		result = select(1, (fd_set *) &read_set, (fd_set *) 0, (fd_set *) 0, (const struct timeval *) NULL);
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
}


/*----------------------------------------------------------------------*
                           rtp_net_send
 *----------------------------------------------------------------------*/
long rtp_net_send (RTP_HANDLE sockHandle, const unsigned char * buffer, long size)
{
    long result;

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);

    result = (long) send((SOCKET) sockHandle, (const char *) buffer, (int) size, 0);

    if (result == SOCKET_ERROR)
    {
        result = (long) WSAGetLastError();
        if ((result == WSAEINPROGRESS) || (result == WSAEWOULDBLOCK))
        {
#ifdef RTP_DEBUG
            RTP_DEBUG_OUTPUT_STR("rtp_net_send: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(result);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_send: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (result);
}


/*----------------------------------------------------------------------*
                           rtp_net_recv
 *----------------------------------------------------------------------*/
long rtp_net_recv (RTP_HANDLE sockHandle, unsigned char * buffer, long size)
{
    long result;

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);

    result = (long) recv((SOCKET) sockHandle, (char *) buffer, (int) size, 0);

    if (result == SOCKET_ERROR)
    {
        result = (long) WSAGetLastError();
        if ((result == WSAEINPROGRESS) || (result == WSAEWOULDBLOCK))
        {
#ifdef RTP_DEBUG
            RTP_DEBUG_OUTPUT_STR("rtp_net_recv: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(result);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_recv: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (result);
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
    struct sockaddr_storage sin;
    struct addrinfo hints, *res;
	char portStr[32];
	char addrStr[NI_MAXHOST];

    sinLen = sizeof(sin);
    memset(&sin, 0, sinLen);

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if(ipAddr)
    {
    	rtp_net_ip_to_str (addrStr, ipAddr, type);
	}
	else
	{
		addrStr[0] = '\0';
	}

	sprintf(portStr, "%d", port);

	if (getaddrinfo(addrStr, portStr ,&hints, &res) != 0)
    {
	    return (-1);
	};

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);

    result = (long) sendto((SOCKET) sockHandle, (const char *) buffer, (int) size, 0, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    if (result == SOCKET_ERROR)
    {
        result = (long) WSAGetLastError();
        if ((result == WSAEINPROGRESS) || (result == WSAEWOULDBLOCK))
        {
#ifdef RTP_DEBUG
            RTP_DEBUG_OUTPUT_STR("rtp_net_sendto: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(result);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_sendto: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (result);
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
    struct sockaddr_storage remote;
    char remotehost[NI_MAXHOST];

    remoteLen = sizeof(remote);
    memset(&remote, 0, remoteLen);

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);

    result = (long) recvfrom((SOCKET) sockHandle, (char *) buffer, (int) size, 0, (struct sockaddr *) &remote, (int *) &remoteLen);

    if (result == SOCKET_ERROR)
    {
        result = (long) WSAGetLastError();
        if ((result == WSAEINPROGRESS) ||
            (result == WSAEWOULDBLOCK) ||
            (result == WSAECONNRESET))
        {
#ifdef RTP_DEBUG
            RTP_DEBUG_OUTPUT_STR("rtp_net_recvfrom: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(result);
            RTP_DEBUG_OUTPUT_STR(".\n");
#endif
            return (-2);
        }
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_net_recvfrom: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    memset(remotehost, 0, sizeof(remotehost));
    getnameinfo((struct sockaddr *)&remote, remoteLen,
                remotehost, sizeof(remotehost),0,0,NI_NUMERICHOST);

    if (ipAddr)
    {
        rtp_net_str_to_ip (ipAddr, remotehost, type);
	}
    if (port)
    {
        *port = ntohs(SS_PORT(&remote));
    }

    return (result);
}


/*----------------------------------------------------------------------*
                          rtp_net_closesocket
 *----------------------------------------------------------------------*/
int rtp_net_closesocket (RTP_HANDLE sockHandle)
{
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

    if (closesocket((SOCKET) sockHandle) == SOCKET_ERROR)
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_closesocket: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
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
}


/*----------------------------------------------------------------------*
                           rtp_net_getntoread
 *----------------------------------------------------------------------*/
int rtp_net_getntoread (RTP_HANDLE sockHandle, unsigned long * nToRead)
{
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
}


/*----------------------------------------------------------------------*
                           rtp_net_setblocking
 *----------------------------------------------------------------------*/
int rtp_net_setblocking (RTP_HANDLE sockHandle, unsigned int onBool)
{
    u_long arg;

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

    arg = (u_long)(!onBool);
    if (ioctlsocket((SOCKET) sockHandle, FIONBIO, (u_long *) &arg) == SOCKET_ERROR)
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_setblocking: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
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
}


/*----------------------------------------------------------------------*
                          rtp_net_setlinger
 *----------------------------------------------------------------------*/
int rtp_net_setlinger (RTP_HANDLE sockHandle, unsigned int onBool, long msecTimeout)
{
    LINGER arg;
#ifdef RTP_DEBUG
    int result;
#endif

    arg.l_onoff = (u_short) onBool;
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
}


/*----------------------------------------------------------------------*
                           rtp_net_setreusesock
 *----------------------------------------------------------------------*/
int rtp_net_setreusesock (RTP_HANDLE sockHandle, unsigned int onBool)
{
    /* ----------------------------------- */
    /*  Not supported in windows, but also */
    /*  not required in windows.  Return   */
    /*  success.                           */
    /* ----------------------------------- */
    return (0);
}


/*----------------------------------------------------------------------*
                           rtp_net_setreuseaddr
 *----------------------------------------------------------------------*/
int rtp_net_setreuseaddr (RTP_HANDLE sockHandle, unsigned int onBool)
{

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
}

/*----------------------------------------------------------------------*
                           rtp_net_settcpnocopy
 *----------------------------------------------------------------------*/
int rtp_net_settcpnocopy (RTP_HANDLE sockHandle, unsigned int onBool)
{
    /* ----------------------------------- */
    /*  Not supported in windows, but also */
    /*  not required in windows.  Return   */
    /*  success.                           */
    /* ----------------------------------- */
    return (0);
}


/*----------------------------------------------------------------------*
                         rtp_net_setkeepalive
 *----------------------------------------------------------------------*/
int rtp_net_setkeepalive (RTP_HANDLE sockHandle, unsigned int onBool)
{

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
}


/*----------------------------------------------------------------------*
                         rtp_net_setmembership
 *----------------------------------------------------------------------*/
int rtp_net_setmembership (RTP_HANDLE sockHandle, unsigned char * ipAddr, int type, unsigned int onBool)
{

	long result = -1;
    struct addrinfo hints, *res;
    char addrStr[NI_MAXHOST];

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if(ipAddr)
    {
    	rtp_net_ip_to_str (addrStr, ipAddr, type);
	}
	else
	{
		addrStr[0] = '\0';
	}

	if (getaddrinfo(addrStr, 0 ,&hints, &res) != 0)
    {
	    return (-1);
	};

	switch (res->ai_family)
    {
        case PF_INET:
        {
            struct ip_mreq mcreq;

            mcreq.imr_multiaddr.s_addr=
                ((struct sockaddr_in *)res->ai_addr)->sin_addr.s_addr;
            mcreq.imr_interface.s_addr= INADDR_ANY;

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
        }
        break;

        case PF_INET6:
        {
            struct ipv6_mreq mcreq6;

           	memcpy(&mcreq6.ipv6mr_multiaddr,
                  &(((struct sockaddr_in6 *)res->ai_addr)->sin6_addr),
                  sizeof(struct in6_addr));

           	mcreq6.ipv6mr_interface= 0; // cualquier interfaz

            if (onBool)
    		{
        		result = setsockopt((int) sockHandle, IPPROTO_IPV6,
                            IPV6_ADD_MEMBERSHIP, (const char *)&mcreq6,
                            sizeof( struct ipv6_mreq ));
    		}
    		else
    		{
        		result = setsockopt((int) sockHandle, IPPROTO_IPV6,
                            IPV6_DROP_MEMBERSHIP, (const char *)&mcreq6,
                            sizeof( struct ipv6_mreq ));
    		}

        }
        break;

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
}


/*----------------------------------------------------------------------*
                         rtp_net_setmcastttl
 *----------------------------------------------------------------------*/
int  rtp_net_setmcastttl(RTP_HANDLE sockHandle, int ttl)
{

    struct sockaddr_storage localAddr;
    int localLen;
	int result;
#ifdef RTP_DEBUG

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);

#endif
    localLen = sizeof(localAddr);
    memset(&localAddr, 0, localLen);

	if (getsockname ((SOCKET) sockHandle, (LPSOCKADDR)&localAddr, (int *) &localLen) != 0)
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_getsockname: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

	if(localAddr.ss_family == AF_INET)
    {
    	result = setsockopt( sockHandle, IPPROTO_IP, IP_MULTICAST_TTL, (char *) &ttl, sizeof (int));
	}
	else if (localAddr.ss_family == AF_INET6)
	{
		result = setsockopt( sockHandle, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char *) &ttl, sizeof (int));
	}
	else
	{
		printf("Can not figure out the ip version type");
		return (-1);
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
}

/*----------------------------------------------------------------------*
                           rtp_net_setbroadcast
 *----------------------------------------------------------------------*/
int rtp_net_setbroadcast (RTP_HANDLE sockHandle, unsigned int onBool)
{

#if (RTP_DEBUG)
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

    if ( setsockopt((SOCKET) sockHandle, SOL_SOCKET, SO_BROADCAST, (char *) &onBool, sizeof (int)) == SOCKET_ERROR )
    {
#if (RTP_DEBUG)
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_setbroadcast: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
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
    int n;

    str[0] = '\0';
    if (type != RTP_NET_TYPE_IPV4)
    {
        strcpy(str, ipAddr);
        return (0);
    }

    for (n=0; n<4; n++)
    {
        itoa(ipAddr[n], (char *) &(str[strlen(str)]), 10);
        if (n<3)
        {
            strcat((char *) str, ".");
        }
    }

    return (strlen((const char *) str));
}


/*----------------------------------------------------------------------*
                           rtp_net_str_to_ip
 *----------------------------------------------------------------------*/
int rtp_net_str_to_ip (unsigned char *ipAddr, char *str, int *type)
{
    int nbytes;
	int n, i;
	int ipType = RTP_NET_TYPE_IPV4;

	n = strlen(str);
    for(i=0; i<n; i++)
    {
        if(str[i]==':')
        {
            ipType = RTP_NET_TYPE_IPV6;
            break;
        }
    }

    if(ipType == RTP_NET_TYPE_IPV4)
    {

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
	}
	else
	{
		*type = RTP_NET_TYPE_IPV6;
    	strcpy(ipAddr, str);
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
    struct timeval selectTime;
    int limit;
    int result;

    fd_set write_set;
    fd_set read_set;
    fd_set error_set;

	FD_ZERO(&write_set);
	FD_ZERO(&read_set);
	FD_ZERO(&error_set);

	limit = (int) (RTP_FD_SET_MAX > FD_SETSIZE ? FD_SETSIZE : RTP_FD_SET_MAX);

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

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    WSASetLastError (0);
#endif

	if (msecTimeout >= 0)
    {
        selectTime.tv_sec = msecTimeout / 1000;
        selectTime.tv_usec = (msecTimeout % 1000) * 1000;
		result = select(1, (fd_set *) &read_set, (fd_set *) &write_set, (fd_set *) &error_set, (const struct timeval *) &selectTime);
    }
	else
	{
		result = select(1, (fd_set *) &read_set, (fd_set *) &write_set, (fd_set *) &error_set, (const struct timeval *) NULL);
	}

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

    if (result == SOCKET_ERROR)
    {
#ifdef RTP_DEBUG
        result = WSAGetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_net_select: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    return (result);
}


void _fd_set_to_rtp (RTP_FD_SET *rtp, fd_set *set)
{
int index;
int limit = (int) (RTP_FD_SET_MAX > FD_SETSIZE ? FD_SETSIZE : RTP_FD_SET_MAX);

    if (set->fd_count > (unsigned) limit)
    {
		rtp->fdCount = limit;
    }
    else
    {
		rtp->fdCount = set->fd_count;
    }

    for (index = 0; index < rtp->fdCount; index++)
    {
		rtp->fdArray[index] = (int) set->fd_array[index];
    }
}

void _rtp_to_fd_set (fd_set *set, RTP_FD_SET *rtp)
{
int index;
int limit = (int) (RTP_FD_SET_MAX > FD_SETSIZE ? FD_SETSIZE : RTP_FD_SET_MAX);

    if (rtp->fdCount > limit)
    {
		set->fd_count = limit;
    }
    else
    {
        set->fd_count = rtp->fdCount;
    }

    for (index = 0; index < (signed) set->fd_count; index++)
    {
		set->fd_array[index] = (int) rtp->fdArray[index];
    }
}

/*----------------------------------------------------------------------*
                           rtp_net_getifaceaddr
 *----------------------------------------------------------------------*/
int rtp_net_getifaceaddr (unsigned char *localAddr, unsigned char *remoteAddr,
                          int remotePort, int remoteType)
{
	RTP_SOCKET tempSock;
	int localPort;
	int localType;

	if (rtp_net_socket_datagram_dual(&tempSock, remoteType) >= 0)
	{
		/* determine the local IP address that is receiving this request by
		   creating a temporary UDP socket and connecting it back to the
		   sender; we then query the IP address of the temp socket using
		   getsockname. */
		if (rtp_net_connect_dual(tempSock, RTP_NET_DATAGRAM, remoteAddr, remotePort, remoteType) >= 0)
		{
			if (rtp_net_getsockname(tempSock, localAddr, &localPort, &localType) >= 0)
			{
				rtp_net_closesocket(tempSock);
				return (0);
			}
		}
		rtp_net_closesocket(tempSock);
	}
	return (-1);
}
/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
