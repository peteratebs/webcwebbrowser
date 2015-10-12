 /*
 | RTPNET.C - Runtime Platform Network Services
 |
 |   PORTED TO THE EBSnet Inc RTIP TCP/IP PLATFORM
 |
 | EBSnet - RT-Platform
 |
 |  $Author: vmalaiya $
 |  $Date: 2005/02/23 16:20:00 $
 |  $Name:  $
 |  $Revision: 1.14 $
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
#include "rtpstr.h"
#include "rtpdebug.h"
#include "rtpscnv.h"

#include "rtip.h"
#include "socket.h"


/************************************************************************
* ONLY HERE FOR TESTING - MUST REMOVE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
************************************************************************/
#ifdef RTP_DEBUG
void cb_wr_dbstring(int a, int b, int c, int d, char *e)
{
    RTP_DEBUG_OUTPUT_STR(e);
    RTP_DEBUG_OUTPUT_STR(".\n");
}
#endif

/************************************************************************
* Defines
************************************************************************/

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

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        *sockHandle = ((RTP_HANDLE)-1);
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_net_socket_stream: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *sockHandle = (RTP_HANDLE) sock;

    return (0);
}

/*----------------------------------------------------------------------*
                        rtp_net_socket_datagram
 *----------------------------------------------------------------------*/
int rtp_net_socket_datagram (RTP_HANDLE  *sockHandle)
{
    int sock;

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0)
    {
        *sockHandle = ((RTP_HANDLE)-1);
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_net_socket_datagram: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    *sockHandle = (RTP_HANDLE) sock;

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

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = in_addr;
    sin.sin_port = htons((unsigned short)port);

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);

    if (bind ((int) sockHandle, (PSOCKADDR) &sin, sizeof (sin)) != 0)
    {
    	result = xn_getlasterror();
        if (result == EADDRINUSE)
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
    set_errno (0);
#endif

    if (listen ((int) sockHandle, queueSize) != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
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
    struct sockaddr_in peerAddr;
    int peerLen;
#ifdef RTP_DEBUG
    int result;
#endif

    peerLen = sizeof (peerAddr);
    memset(&peerAddr, 0, peerLen);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    if (getpeername ((int) sockHandle, (PSOCKADDR) &peerAddr, (PFINT) &peerLen) != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_net_getpeername: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
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


/*----------------------------------------------------------------------*
                          rtp_net_getsockname
 *----------------------------------------------------------------------*/
int rtp_net_getsockname (RTP_HANDLE sockHandle, unsigned char *ipAddr, int *port, int *type)
{
    struct sockaddr_in localAddr;
    int localLen;
#ifdef RTP_DEBUG
    int result;
#endif

    localLen = sizeof (localAddr);
    memset(&localAddr, 0, localLen);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    if (getsockname ((int) sockHandle, (PSOCKADDR) &localAddr, (PFINT) &localLen) != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_net_getsockname: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
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


/*----------------------------------------------------------------------*
                          rtp_net_gethostbyname
 *----------------------------------------------------------------------*/
int rtp_net_gethostbyname (unsigned char *ipAddr, int *type, char *name)
{
#if (INCLUDE_DNS)
#ifdef RTP_DEBUG
    int result;
#endif
    PFHOSTENT hp = NULL;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    if (!(hp = gethostbyname(name)))
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_net_gethostbyname: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    if (hp->h_addrtype != AF_INET)
    {
#ifdef RTP_DEBUG
        set_errno (EAFNOSUPPORT);
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_net_gethostbyname: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    *type = RTP_NET_TYPE_IPV4;

    ipAddr[0] = (unsigned char) hp->h_addr_list[0][0];
    ipAddr[1] = (unsigned char) hp->h_addr_list[0][1];
    ipAddr[2] = (unsigned char) hp->h_addr_list[0][2];
    ipAddr[3] = (unsigned char) hp->h_addr_list[0][3];


    return (0);
#else

    rtp_not_yet_implemented();
    return (-1);
#endif /* INCLUDE_DNS */
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
    int result;

    clientLen = sizeof (clientAddr);
    memset(&clientAddr, 0, clientLen);

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);

    conSocket = accept ((int) serverSock, (PSOCKADDR) &clientAddr, (PFINT) &clientLen);

    if (conSocket < 0)
    {
        *connectSock = ((RTP_HANDLE)-1);
        result = xn_getlasterror();
        if ((result == EINPROGRESS) || (result == EWOULDBLOCK))
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

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = in_addr;
    sin.sin_port = htons((unsigned short)port);

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);

    if (connect ((int) sockHandle, (PSOCKADDR) &sin, sinLen) != 0)
    {
        result = xn_getlasterror();
        if ((result == EINPROGRESS) ||
            (result == EWOULDBLOCK))
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
unsigned rtp_net_is_connected (RTP_SOCKET sockHandle)
{
    struct sockaddr_in peerAddr;
    int peerLen;

    peerLen = sizeof (peerAddr);
    rtp_memset(&peerAddr, 0, peerLen);

    return (getpeername ((int)sockHandle, (PSOCKADDR)&peerAddr, (PFINT)&peerLen) == 0);
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
    FD_SET(sockHandle, &write_set);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    if (msecTimeout >= 0)
    {
        selectTime.tv_sec = msecTimeout / 1000;
        selectTime.tv_usec = (msecTimeout % 1000) * 1000;
        result = select(1, (PFDSET) 0, (PFDSET) &write_set, (PFDSET) 0, (PCTIMEVAL) &selectTime);
    }
    else
    {
        result = select(1, (PFDSET) 0, (PFDSET) &write_set, (PFDSET) 0, (PCTIMEVAL) NULL);
    }

 /*   if (result == 1)
 |    {
 |        if (!xn_tcp_is_write(sockHandle))
 |        {
 |            result = (-1);
 |        }
 |    }
*/
    /* if an error or if it timed out */
    if (result <= 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
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
    /*              read list              */
    /* ----------------------------------- */
    FD_ZERO(&read_set);
    FD_SET(sockHandle, &read_set);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    if (msecTimeout >= 0)
    {
        selectTime.tv_sec = msecTimeout / 1000;
        selectTime.tv_usec = (msecTimeout % 1000) * 1000;
        result = select(1, (PFDSET) &read_set, (PFDSET) 0, (PFDSET) 0, (PCTIMEVAL) &selectTime);
    }
    else
    {
        result = select(1, (PFDSET) &read_set, (PFDSET) 0, (PFDSET) 0, (PCTIMEVAL) NULL);
    }

 /*   if (result == 1)
 |    {
 |        if (!xn_tcp_is_read(sockHandle))
 |        {
 |            result = (-1);
 |        }
 |    }
*/
    /* if an error or if it timed out */
    if (result <= 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
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
    set_errno (0);

    result = (long) send((int) sockHandle, (const char *) buffer, (int) size, 0);

    if (result < 0)
    {
        result = (long) xn_getlasterror();
        if ((result == EINPROGRESS) || (result == EWOULDBLOCK))
        {
			RTP_DEBUG_OUTPUT_STR("rtp_net_send: non-fatal error returned ");
			RTP_DEBUG_OUTPUT_INT(result);
			RTP_DEBUG_OUTPUT_STR(".\n");
            return (-2);
        }
		RTP_DEBUG_OUTPUT_STR("rtp_net_send: error returned ");
		RTP_DEBUG_OUTPUT_INT(result);
		RTP_DEBUG_OUTPUT_STR(".\n");
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
    set_errno (0);

    result = (long) recv((int) sockHandle, (char *) buffer, (int) size, 0);

    if (result < 0)
    {
        result = (long) xn_getlasterror();
        if ((result == EINPROGRESS) || (result == EWOULDBLOCK))
        {
        	RTP_DEBUG_OUTPUT_STR("rtp_net_recv: non-fatal error returned ");
        	RTP_DEBUG_OUTPUT_INT(result);
        	RTP_DEBUG_OUTPUT_STR(".\n");
            return (-2);
        }
        RTP_DEBUG_OUTPUT_STR("rtp_net_recv: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
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

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = in_addr;
    sin.sin_port = htons((unsigned short)port);

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);

    result = (long) sendto((int) sockHandle, (PFCCHAR) buffer, (int) size, 0, (PCSOCKADDR)&sin, sinLen);

    if (result < 0)
    {
        result = (long) xn_getlasterror();
        if ((result == EINPROGRESS) || (result == EWOULDBLOCK))
        {
        	RTP_DEBUG_OUTPUT_STR("rtp_net_sendto: non-fatal error returned ");
        	RTP_DEBUG_OUTPUT_INT(result);
        	RTP_DEBUG_OUTPUT_STR(".\n");
            return (-2);
        }
        RTP_DEBUG_OUTPUT_STR("rtp_net_sendto: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
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
    struct sockaddr_in remote;

    remoteLen = sizeof (remote);
    memset (&remote, 0, remoteLen);

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);

    result = (long) recvfrom ((int) sockHandle, (PFCHAR) buffer, (int) size, 0, (PSOCKADDR) &remote, (PFINT) &remoteLen);

    if (result < 0)
    {
        result = (long) xn_getlasterror();
        if ((result == EINPROGRESS) || (result == EWOULDBLOCK))
        {
        	RTP_DEBUG_OUTPUT_STR("rtp_net_recvfrom: non-fatal error returned ");
        	RTP_DEBUG_OUTPUT_INT(result);
        	RTP_DEBUG_OUTPUT_STR(".\n");
            return (-2);
        }
        RTP_DEBUG_OUTPUT_STR("rtp_net_recvfrom: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
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
    set_errno (0);
#endif

    if (closesocket((int) sockHandle) != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_net_closesocket: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    return (0);
}


/*----------------------------------------------------------------------*
                            rtp_net_shutdown
 *----------------------------------------------------------------------*/
int rtp_net_shutdown (RTP_HANDLE sockHandle, int how)
{
#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    if (shutdown((int) sockHandle, how) != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
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
    unsigned long arg;

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    if (ioctlsocket((int) sockHandle, FIONREAD, &arg) != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
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
    unsigned long arg;

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    arg = (unsigned long)(!onBool);
    if (ioctlsocket((int) sockHandle, FIONBIO, &arg) != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
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
    set_errno (0);
#endif

    option = (int) onBool;
    if (setsockopt((int) sockHandle, SOL_SOCKET,
                    SO_NAGLE, (PFCCHAR) &option,
                    sizeof (int)) != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
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

    arg.l_onoff = onBool;
    arg.l_linger = 0;
    if (arg.l_onoff)
    {
	    if (msecTimeout > 0)
	    {
	        arg.l_linger = (int) (msecTimeout / 1000);
	    }
    }

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    if (setsockopt((int) sockHandle, SOL_SOCKET,
                    SO_LINGER, (PFCCHAR) &arg,
                    sizeof (struct linger)) != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
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

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    if ( setsockopt((int) sockHandle,
                    SOL_SOCKET,
                    SO_REUSESOCK,
                    (PFCCHAR) &onBool,
                    sizeof (int)) != 0 )
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_net_setreusesock: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
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
    set_errno (0);
#endif

    if ( setsockopt((int) sockHandle,
                    SOL_SOCKET,
                    SO_REUSEADDR,
                    (PFCCHAR) &onBool,
                    sizeof (int)) != 0 )
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
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

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    if ( setsockopt((int) sockHandle,
                    SOL_SOCKET,
                    SO_TCP_NO_COPY,
                    (PFCCHAR) &onBool,
                    sizeof (int)) != 0 )
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_net_settcpnocopy: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
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
    set_errno (0);
#endif

    if (setsockopt((int) sockHandle, SOL_SOCKET,
                    SO_KEEPALIVE, (PFCCHAR)&onBool,
                    sizeof (int)) != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
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
    struct sockaddr_in sin;
    struct sockaddr_in localAddr;
    int localLen;
    int result;
    struct ip_mreq mcreq;
    unsigned long in_addr = 0;

    localLen = sizeof (localAddr);
    memset(&localAddr, 0, localLen);
    memset(&sin, 0, sizeof (sin));

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
    set_errno (0);
#endif

    if (getsockname ((int) sockHandle, (PSOCKADDR) &localAddr, (PFINT) &localLen) != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_net_setmembership: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    rtp_memmove(&mcreq.imr_multiaddr.s_addr, &in_addr, IP_ALEN);
    rtp_memmove(&mcreq.imr_interface.s_addr, &localAddr.sin_addr.s_addr, IP_ALEN);

    if (onBool)
    {
        result = setsockopt((int) sockHandle, IPPROTO_IP,
                            IP_ADD_MEMBERSHIP, (PFCCHAR)&mcreq,
                            sizeof (mcreq));
    }
    else
    {
        result = setsockopt((int) sockHandle, IPPROTO_IP,
                            IP_DROP_MEMBERSHIP, (PFCCHAR)&mcreq,
                            sizeof (mcreq));
    }

    if (result != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
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
    if (!onBool)
    {
    	RTP_DEBUG_OUTPUT_STR("rtp_net_setbroadcast: broadcasts cannot be turned off.\n");
    	return (-2);
    }
    return (0);
}


/*----------------------------------------------------------------------*
                         rtp_net_setmcastttl
 *----------------------------------------------------------------------*/
int  rtp_net_setmcastttl(RTP_HANDLE sockHandle, int ttl)
{
    int result;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    result = setsockopt( sockHandle, IPPROTO_IP,
                IP_MULTICAST_TTL, (char *) &ttl, sizeof (int));
    if (result != 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_net_setmcastttl: error returned ");
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
        return (-1);
    }

    for (n=0; n<4; n++)
    {
        rtp_itoa(ipAddr[n], (char *) &(str[rtp_strlen(str)]), 10);
        if (n<3)
        {
            rtp_strcat((char *) str, ".");
        }
    }

    return (rtp_strlen((const char *) str));
}


/*----------------------------------------------------------------------*
                           rtp_net_str_to_ip
 *----------------------------------------------------------------------*/
int rtp_net_str_to_ip (unsigned char *ipAddr, char *str, int *type)
{
    char * ptr;
    char savech;
    int nbytes;
    int n;

    *type = RTP_NET_TYPE_IPV4;
    nbytes = 4;

    for (n=0; n<nbytes; n++)
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
    int index;
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
        if (writeList->fdCount > limit)
        {
            write_set.num_sockets = limit;
        }
        else
        {
            write_set.num_sockets = writeList->fdCount;
        }

        for (index = 0; index < write_set.num_sockets; index++)
        {
            write_set.sockets[index] = (int) writeList->fdArray[index];
        }
    }

    /* ----------------------------------- */
    /*               read list             */
    /* ----------------------------------- */
    if (readList)
    {
        if (readList->fdCount > limit)
        {
            read_set.num_sockets = limit;
        }
        else
        {
            read_set.num_sockets = readList->fdCount;
        }

        for (index = 0; index < read_set.num_sockets; index++)
        {
            read_set.sockets[index] = (int) readList->fdArray[index];
        }
    }

    /* ----------------------------------- */
    /*              error list             */
    /* ----------------------------------- */
    if (errorList)
    {
        if (errorList->fdCount > limit)
        {
            error_set.num_sockets = limit;
        }
        else
        {
            error_set.num_sockets = errorList->fdCount;
        }

        for (index = 0; index < error_set.num_sockets; index++)
        {
            error_set.sockets[index] = (int) errorList->fdArray[index];
        }
    }

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    if (msecTimeout >= 0)
    {
        selectTime.tv_sec = msecTimeout / 1000;
        selectTime.tv_usec = (msecTimeout % 1000) * 1000;
        result = select(1, (PFDSET) &read_set, (PFDSET) &write_set, (PFDSET) &error_set, (PCTIMEVAL) &selectTime);
    }
    else
    {
        result = select(1, (PFDSET) &read_set, (PFDSET) &write_set, (PFDSET) &error_set, (PCTIMEVAL) NULL);
    }

    if (result < 0)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_net_select: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }

    if (writeList)
    {
        if (write_set.num_sockets > limit)
        {
            writeList->fdCount = limit;
        }
        else
        {
            writeList->fdCount = write_set.num_sockets;
        }

        for (index = 0; index < writeList->fdCount; index++)
        {
            writeList->fdArray[index] = write_set.sockets[index];
        }
    }

    if (readList)
    {
        if (read_set.num_sockets > limit)
        {
            readList->fdCount = limit;
        }
        else
        {
            readList->fdCount = read_set.num_sockets;
        }

        for (index = 0; index < readList->fdCount; index++)
        {
            readList->fdArray[index] = read_set.sockets[index];
        }
    }

    if (errorList)
    {
        if (error_set.num_sockets > limit)
        {
            errorList->fdCount = limit;
        }
        else
        {
            errorList->fdCount = error_set.num_sockets;
        }

        for (index = 0; index < errorList->fdCount; index++)
        {
            errorList->fdArray[index] = error_set.sockets[index];
        }
    }

    return (result);
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

	if (rtp_net_socket_datagram(&tempSock) >= 0)
	{
		/* determine the local IP address that is receiving this request by
		   creating a temporary UDP socket and connecting it back to the
		   sender; we then query the IP address of the temp socket using
		   getsockname. */
		if (rtp_net_connect(tempSock, remoteAddr, remotePort, remoteType) >= 0)		
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
