

 /*
 | RTPNET.C - Runtime Platform Network Services
 |
 |   PORTED TO THE RTIP network stack for all PLATFORMs
 |
 | EBSnet - EBS_RT-Platform
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
#include "rtpprint.h"

#include "rtip.h"
#include "socket.h"
#include "udpapi.h"
#include "tcpapi.h"

#if (INCLUDE_BGET)
#include "bget.h"
#endif

#define MEMORY_POOL_SIZE       6*128*1024
static unsigned char    MemoryPool[MEMORY_POOL_SIZE];


/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Compile Time Possible Porting Errors
************************************************************************/
#if (RTP_FD_SET_MAX > FD_SETSIZE)
#error RTP_FD_SET_MAX SHOULD NEVER BE LARGER THAN THE NATIVE FD_SETSIZE
#error     Adjustments should be made to RTP_FD_SET_MAX in rtpnet.h
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
#if (TAHI_TEST)
extern void start_device_setup();
#endif

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                  rtp_net_init
 *----------------------------------------------------------------------*/
int rtp_net_init (void)
{
    int retVal;
    /*  These functions are usually registered by the stack in the
    normal build (INCLUDE_RUN_TIME_CONFIG = 0).  If INCLUDE_RUN_TIME_CONFIG is
    turned on, each add-on needs to be registered by the application
     */
#if (INCLUDE_RUN_TIME_CONFIG)
#if (INCLUDE_LOOP)
    XN_REGISTER_LOOPBACK()
#endif

#if (INCLUDE_DHCP_CLI)
    XN_REGISTER_DHCP_CLI()
#endif
#endif   /* INCLUDE_RUN_TIME_CONFIG */
      
#if (INCLUDE_BGET)
    bget_init();
    bpool(&MemoryPool[0], MEMORY_POOL_SIZE);   
#endif
    
    rtp_kern_init();
    
    rtp_threads_init();
    // TODO: Add NAT parameters when EBSNet makes them available
     
    retVal = xn_rtip_init();
    
    xn_udp_initialize();
    xn_tcp_initialize();
    return (retVal);
}


/*----------------------------------------------------------------------*
                  rtp_net_exit
 *----------------------------------------------------------------------*/
void rtp_net_exit (void)
{
    xn_rtip_exit();
	
#if (RTKERNEL)
   rtp_threads_shutdown();

    rtp_kern_exit( 0 );
#endif
}

/*----------------------------------------------------------------------*
                  rtp_net_restart
 *----------------------------------------------------------------------*/
void rtp_net_restart (void)
{
#if (TAHI_TEST)

    xn_rtip_exit();
    xn_rtip_restart();
    start_device_setup();
#else
#if (0)
    tc_interface_close(ifaceNo);

    if (tc_wait_pkts_output(RTP_TRUE, ks_ticks_p_sec()))
    {
         rtp_term_puts("ifrtip.c: xn_wait_pkts_output returned");
    }
    else
    {
        rtp_term_puts("ifrtip.c: xn_wait_pkts_output returned: but not all pkts xmitted");
    }
#endif

    xn_rtip_exit();
    xn_rtip_restart();
#endif
}

/*----------------------------------------------------------------------*
                        rtp_net_socket
  INPUTS: 
      family = AF_INET for IPv4 or AF_INET6 for IPv6
 *----------------------------------------------------------------------*/
int rtp_net_socket (RTP_HANDLE  *sockHandle, int family, int type, int protocol)
{
    SOCKET sock;
    
#ifdef RTP_DEBUG
    int result;
    /* -----------------------------------    */
    /*  Clear the error state by setting      */
    /*  to 0.                                 */
    /* -----------------------------------    */
    
#endif

    /* -----------------------------------   */
    if (!family)
    {   
        *sockHandle = ((RTP_HANDLE)INVALID_SOCKET);
        tc_set_errno(RTP_NET_EAFNOSUPPORT);
        return (-1);
    }
    
    if (!type)
    {
        *sockHandle = ((RTP_HANDLE)INVALID_SOCKET);
        tc_set_errno(RTP_NET_ETNOSUPPORT);
        return (-1);
    }
    
    if (! (  (type == RTP_NET_SOCK_RAW) 
            || (type == RTP_NET_SOCK_STREAM && protocol == 6)
            || (type == RTP_NET_SOCK_DGRAM && protocol == 17)
           ) 
        )   
    {
        *sockHandle = ((RTP_HANDLE)INVALID_SOCKET);
        tc_set_errno(RTP_NET_ETNOSUPPORT);
        return (-1);
    }
    
    sock = socket(family, type, protocol);

    if (sock == INVALID_SOCKET)
    {
        *sockHandle = ((RTP_HANDLE)INVALID_SOCKET);
        
        
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
             rtp_net_socket_stream_dual
 *----------------------------------------------------------------------*/
int rtp_net_socket_stream_dual (RTP_HANDLE  *sockHandle, int type)
{
    SOCKET sock;
#ifdef RTP_DEBUG
    int result;
#endif


    if (type == RTP_NET_TYPE_IPV4)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
    }
    else if (type == RTP_NET_TYPE_IPV6)
    {
        sock = socket(AF_INET6, SOCK_STREAM, 0);
    }
    else
    {
        return (-1);
    }
    if (sock == INVALID_SOCKET)
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
    tc_set_errno (0);
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
    SOCKET sock;

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    tc_set_errno (0);
#endif

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == INVALID_SOCKET)
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
                        rtp_net_socket_datagram_dual
 *----------------------------------------------------------------------*/
int rtp_net_socket_datagram_dual (RTP_HANDLE  *sockHandle, int type)
{
    SOCKET sock;

#ifdef RTP_DEBUG
    int result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    tc_set_errno(0);
#endif

    if (type == RTP_NET_TYPE_IPV4)
    {
        sock = socket(AF_INET, SOCK_DGRAM, 0);
    }
    else if (type == RTP_NET_TYPE_IPV6)
    {
        sock = socket(AF_INET6, SOCK_DGRAM, 0);
    }
    else
    {
        return (-1);
    }

    if (sock == INVALID_SOCKET)
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

    rtp_memset(&sin, 0, sizeof (sin));

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
    tc_set_errno (0);

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
            return (-1);
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
                            rtp_net_bind_dual
 *----------------------------------------------------------------------*/
int rtp_net_bind_dual(RTP_HANDLE sockHandle, int sockType,
                      unsigned char *ipAddr, int port, int type)
{
    int result;
    struct addrinfo hints, *res;
    char portStr[32];
    char addrStr[NI_MAXHOST];

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    tc_set_errno(0);
#endif
    rtp_memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    if(type == RTP_NET_TYPE_IPV6)
    {
        hints.ai_family = AF_INET6;
    }
    else
    {
        hints.ai_family = AF_INET;
    }

    if (sockType == RTP_NET_STREAM)
    {
        hints.ai_socktype = SOCK_STREAM;
    }
    else
    {
        hints.ai_socktype = SOCK_DGRAM;
    }

    if(ipAddr)
    {
        rtp_net_ip_to_str (addrStr, ipAddr, type);
    }
    else
    {
        addrStr[0] = '\0';
    }

    if(port == 0)
    {
        rtp_sprintf(portStr, "%d", 0);
    }
    else
    {
        rtp_sprintf(portStr, "%d", port);
    }

    result = getaddrinfo(0, portStr, &hints, &res);
    if( result != 0)
    {
        return (-1);
    }

    if (bind((SOCKET) sockHandle, res->ai_addr, res->ai_addrlen) != 0)
    {
        freeaddrinfo(res);
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
    freeaddrinfo(res);
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
    tc_set_errno (0);
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
    rtp_memset(&peerAddr, 0, peerLen);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    tc_set_errno (0);
#endif

    if (getpeername ((int) sockHandle, (PSOCKADDR) &peerAddr, (RTP_PFINT) &peerLen) != 0)
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
    rtp_memset(&localAddr, 0, localLen);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    tc_set_errno (0);
#endif

    if (getsockname ((int) sockHandle, (PSOCKADDR) &localAddr, (RTP_PFINT) &localLen) != 0)
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
    tc_set_errno (0);
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
        tc_set_errno (EAFNOSUPPORT);
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
    rtp_memset(&clientAddr, 0, clientLen);

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    tc_set_errno (0);

    conSocket = accept ((int) serverSock, (PSOCKADDR) &clientAddr, (RTP_PFINT) &clientLen);

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
            return (-1);
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
    rtp_memset(&sin, 0, sinLen);

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
    tc_set_errno (0);

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
            return (-1);
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
                          rtp_net_connect_dual
 *----------------------------------------------------------------------*/
int rtp_net_connect_dual (RTP_HANDLE sockHandle, int sockType,
                          unsigned char *ipAddr,
                          int port, int type)
{
    int result;
    struct addrinfo hints, *res;
    char portStr[32];
    char addrStr[NI_MAXHOST];

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    tc_set_errno(0);
#endif

    rtp_memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;
    hints.ai_family   = AF_UNSPEC;
    if (sockType == RTP_NET_STREAM)
    {
        hints.ai_socktype = SOCK_STREAM;
    }
    else
    {
        hints.ai_socktype = SOCK_DGRAM;
    }

    if(ipAddr)
    {
        rtp_net_ip_to_str (addrStr, ipAddr, type);
    }
    else
    {
        addrStr[0] = '\0';
    }

    rtp_sprintf(portStr, "%d", port);

    if (getaddrinfo(addrStr, portStr ,&hints, &res) != 0)
    {
        return (-1);
    };

    if (connect ((SOCKET) sockHandle, res->ai_addr, res->ai_addrlen) != 0)
    {
        freeaddrinfo (res);
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

    freeaddrinfo(res);
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

    return (getpeername ((int)sockHandle, (PSOCKADDR)&peerAddr, (RTP_PFINT)&peerLen) == 0);
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
    FD_SET((SOCKET)sockHandle, &write_set);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    tc_set_errno (0);
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
    FD_SET((SOCKET)sockHandle, &read_set);

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    tc_set_errno (0);
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

#if (0) /* SPRZ */
/*----------------------------------------------------------------------*
                           rtp_net_send_ssl
 *----------------------------------------------------------------------*/
long rtp_net_send_ssl (RTP_SOCKET sockHandle, const unsigned char *buffer, long size, int flags)
{
    long ret;

    ret = (int)rtp_net_send(sockHandle, buffer, size, flags);
    
    if (ret < 0)
    { 
        int lastError = xn_getlasterror();
        if (lastError == EINPROGRESS || lastError == EWOULDBLOCK) ret = -1;
    }
}
#endif

/*----------------------------------------------------------------------*
                           rtp_net_send
 *----------------------------------------------------------------------*/
long rtp_net_send (RTP_SOCKET sockHandle, const unsigned char *buffer, long size)
{
    long result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    tc_set_errno (0);

    result = (long) send((int) sockHandle, (const char *) buffer, (int) size, 0);

    if (result < 0)
    {
        result = (long) xn_getlasterror();
        if ((result == EINPROGRESS) || (result == EWOULDBLOCK))
        {
            RTP_DEBUG_OUTPUT_STR("rtp_net_send: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(result);
            RTP_DEBUG_OUTPUT_STR(".\n");
            return (-1);
        }
        RTP_DEBUG_OUTPUT_STR("rtp_net_send: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
        return (-1);
    }
    return (result);
}

/*----------------------------------------------------------------------*
                           rtp_net_recv_ssl
 *----------------------------------------------------------------------*/
long rtp_net_recv_ssl(RTP_HANDLE sockHandle, unsigned char * buffer, long size)
{
    long ret;

    ret = (int)rtp_net_recv(sockHandle, buffer, size);  /* SPRC */
        
    if (ret < 0)
    { 
        int lastError = xn_getlasterror();
        if (lastError == EINPROGRESS || lastError == EWOULDBLOCK ) ret = -1;
    }

    return (ret);
}

/*----------------------------------------------------------------------*
                           rtp_net_recv
 *----------------------------------------------------------------------*/
long rtp_net_recv(RTP_SOCKET sockHandle, unsigned char *buffer, long size)
{
    long result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    tc_set_errno (0);

    result = (long) recv((int) sockHandle, (char *) buffer, (int) size, 0);

    if (result < 0)
    {
        result = (long) xn_getlasterror();
        if ((result == EINPROGRESS) || (result == EWOULDBLOCK))
        {
            RTP_DEBUG_OUTPUT_STR("rtp_net_recv: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(result);
            RTP_DEBUG_OUTPUT_STR(".\n");
            return (-1);
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
long     rtp_net_sendto          (RTP_SOCKET sockHandle, const unsigned char *buffer, long size,
                                  unsigned char *ipAddr, int port, int type)
{
    return(rtp_net_sendto_flags(sockHandle, buffer, size,
                                  ipAddr, port, type, 0));
}

long     rtp_net_sendto_flags    (RTP_SOCKET sockHandle, const unsigned char *buffer, long size,
                                  unsigned char *ipAddr, int port, int type, int flags)
{
    long result;
    int  sinLen;
    struct sockaddr_in sin;
    unsigned long in_addr = 0;

    sinLen = sizeof (sin);
    rtp_memset(&sin, 0, sinLen);

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
    tc_set_errno (0);

    result = (long) sendto((int) sockHandle, (RTP_PFCCHAR) buffer, (int) size, flags, (PCSOCKADDR)&sin, sinLen);

    if (result < 0)
    {
        result = (long) xn_getlasterror();
        if ((result == EINPROGRESS) || (result == EWOULDBLOCK))
        {
            RTP_DEBUG_OUTPUT_STR("rtp_net_sendto: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(result);
            RTP_DEBUG_OUTPUT_STR(".\n");
            return (-1);
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
    return(rtp_net_recvfrom_flags (sockHandle,
                       buffer, size,
                       ipAddr, port, type, 0));
}

long rtp_net_recvfrom_flags (RTP_HANDLE sockHandle,
                       unsigned char *buffer, long size,
                       unsigned char *ipAddr, int *port, int *type, int flags)
{
    long result;
    int remoteLen;
    struct sockaddr_in remote;

    remoteLen = sizeof (remote);
    rtp_memset (&remote, 0, remoteLen);

    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    tc_set_errno (0);

    result = (long) recvfrom ((int) sockHandle, (RTP_PFCHAR) buffer, (int) size, flags, (PSOCKADDR) &remote, (RTP_PFINT) &remoteLen);

    if (result < 0)
    {
        result = (long) xn_getlasterror();
        if ((result == EINPROGRESS) || (result == EWOULDBLOCK))
        {
            RTP_DEBUG_OUTPUT_STR("rtp_net_recvfrom: non-fatal error returned ");
            RTP_DEBUG_OUTPUT_INT(result);
            RTP_DEBUG_OUTPUT_STR(".\n");
            return (-1);
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
    tc_set_errno (0);
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
    tc_set_errno (0);
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
                           rtp_net_getlasterror
 *----------------------------------------------------------------------*/
int      rtp_net_getlasterror    (void)
{
    int ret;
    ret = xn_getlasterror();
    
#ifdef RTP_DEBUG
    
#endif
    return(ret);
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
    tc_set_errno (0);
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
                           rtp_net_setsockoopt
 *----------------------------------------------------------------------*/
int      rtp_net_setsockoopt (RTP_HANDLE sockHandle, int level, int optionName, 
                                    RTP_PFCCHAR optionVal, int length)
{
    int     ret;
    LINGER  linger;
    RTP_UINT32 win_size32;
    RTP_UINT16 win_size16;
    struct timeval timeout;
    int millisecs;
    PANYSOCK pport;
    int value;

    if (    (level == RTP_NET_SOL_SOCKET) &&
            (optionName == RTP_NET_IP_OPTS_BROADCAST)  )
    {
        pport = tc_api_sock_to_port((SOCKET)sockHandle);
        if (pport && pport->port_type == TCPSOCKTYPE)
        {
            tc_set_errno(RTP_NET_ENOPROTOOPT);
            return -1;
        }
    }
    
    if (    (level == RTP_NET_SOL_SOCKET) &&
            (optionName == RTP_NET_SO_LINGER) )
    {
        if(length != sizeof(int)) 
        {
            tc_set_errno(EFAULT);
            return -1;
        }
        
        value = *((int*)optionVal);

        if(value == -1) 
        {
            // graceful block
            linger.l_onoff  = 1;
            linger.l_linger = 1;
        }
        else if(value == -2)
        {
            // Linger OFF
            // graceful no block
            linger.l_onoff  = 0;
            linger.l_linger = 1;
        }
        else
        {
            // graceful block OR
            // hard block (with value 0)
            // Linger ON with timeout
            linger.l_onoff = 1;
            linger.l_linger = value;            
        }

        optionVal = (RTP_PFCCHAR)&linger;
        length = sizeof(LINGER);
    }
    /* JRT */
    else if ( (optionName == RTP_NET_SO_RECEIVE_BUFFER) ||
              (optionName == RTP_NET_SO_SEND_BUFFER) )
    {
        if (length != 4)
        {
            tc_set_errno(EFAULT);
            return (-1);
        }

        pport = tc_api_sock_to_port((SOCKET)sockHandle);
        if (!pport)
              return(tc_set_errno(ENOTSOCK));
              
        rtp_memcpy(&win_size32, optionVal, 4);
    
        if (pport->port_type == TCPSOCKTYPE)
        {
            if (win_size32 > 0xFFFF)
            {
                tc_set_errno(EFAULT);
                return (-1);
            }
        
            win_size16 = (RTP_UINT16) win_size32;
            length = sizeof(RTP_UINT16);
            optionVal = (RTP_PFCCHAR)&win_size16;
            level = RTP_NET_SOL_SOCKET;
        }
        else if (pport->port_type == UDPSOCKTYPE)
        {
            length = sizeof(RTP_UINT32);
            optionVal = (RTP_PFCCHAR)&win_size32;
            level = RTP_NET_SOL_SOCKET;
        }
    }
    /* JRT */
    else if ( (optionName == RTP_NET_SO_RCV_TIMEO) ||
              (optionName == RTP_NET_SO_SEND_TIMEO) )
    {
        length = sizeof(struct timeval);
        
        millisecs = *optionVal % 1000;
        timeout.tv_usec = millisecs * 1000;
        timeout.tv_sec = *optionVal / 1000;

        optionVal = (RTP_PFCCHAR)&timeout;
    } 
    else if ((level == RTP_NET_IPROTO_TCP) &&
            (optionName == RTP_NET_SO_NAGLE) )
    {
        int set_nagle = 0;
        
        if (!(*optionVal))
        {
            set_nagle = 1;        
        }

        optionVal = (RTP_PFCCHAR)&set_nagle;
    }
    
    ret = setsockopt((SOCKET)sockHandle, level, optionName, optionVal, length);
    
    return ret;   
    
}

/*----------------------------------------------------------------------*
                           rtp_net_getsockopt
 *----------------------------------------------------------------------*/
int      rtp_net_getsockopt  (RTP_HANDLE sockHandle, int level, int optionName, 
                              RTP_PFCHAR optionVal, int* optlen )
{    
    int ret = -1;
    int optionLength;
    PANYSOCK pport;
    
    /* JRT - linger not a pointer */
    LINGER      linger;
    
    /* JRT - SO_RCV_TIMEO, SO_SEND_TIMEO take timeval struct */
    struct      timeval timeout;
    RTP_UINT16  window_size;
    
    if (    (level == RTP_NET_SOL_SOCKET) &&
            (optionName == RTP_NET_IP_OPTS_BROADCAST)  )
    {
        pport = tc_api_sock_to_port((SOCKET)sockHandle);
        if (pport && pport->port_type == TCPSOCKTYPE)
        {
            tc_set_errno(RTP_NET_ENOPROTOOPT);
            return -1;
        }
    }
    
    if ( (level      == RTP_NET_SOL_SOCKET) &&
         (optionName == RTP_NET_SO_LINGER))
    {
        optionLength = sizeof(LINGER);
        ret = getsockopt((SOCKET)sockHandle, level, optionName, (RTP_PFCHAR) &linger, &optionLength);
    }         
    else if ( (optionName == RTP_NET_SO_RCV_TIMEO) ||
              (optionName == RTP_NET_SO_SEND_TIMEO) )
    {
        optionLength = sizeof(struct timeval);
        ret = getsockopt((SOCKET)sockHandle, level, optionName, (RTP_PFCHAR) &timeout, &optionLength);
    } 

    else if ( (optionName == RTP_NET_SO_RECEIVE_BUFFER) ||
              (optionName == RTP_NET_SO_SEND_BUFFER) )
    {        
        if (*optlen != 4)
        {
            tc_set_errno(EFAULT);
            return (-1);
        }
 
        pport = tc_api_sock_to_port((SOCKET)sockHandle);
        if (!pport)
        {
              return(tc_set_errno(ENOTSOCK));       
        }
        
        if (pport->port_type == TCPSOCKTYPE)
        {
            optionLength = sizeof(RTP_UINT16);
        }
        else if (pport->port_type == UDPSOCKTYPE)
        {
            optionLength = sizeof(RTP_UINT32);
        }

        level = RTP_NET_SOL_SOCKET;
            
        ret = getsockopt((SOCKET)sockHandle, level, optionName, (RTP_PFCHAR) &window_size, &optionLength);
    } 
    else if ((level == RTP_NET_IPROTO_TCP) &&
             (optionName == RTP_NET_SO_NAGLE) )
    {
        ret = getsockopt((SOCKET)sockHandle, level, optionName, optionVal, &optionLength);
  
        *optionVal = !(*optionVal);
    }             
    else
    {
        optionLength = *optlen;
        ret = getsockopt((SOCKET)sockHandle, level, optionName, optionVal, &optionLength);
    }
     
    /* JRT */
    /* ret = getsockopt(sockHandle, level, optionName, optionVal, &optionLength); */
    
    if ( (level == RTP_NET_SOL_SOCKET) &&
         (optionName == RTP_NET_SO_LINGER) )
    {
        /* JRT */
        *((int*)optionVal) = linger.l_linger;
        *optlen = sizeof(int);
    }
    /* JRT */
    if ( (optionName == RTP_NET_SO_RCV_TIMEO) ||
         (optionName == RTP_NET_SO_SEND_TIMEO) )
    {
        /* Convert the timeval values to milliseconds */
        *optionVal =  (int)((timeout.tv_sec * 1000) + (timeout.tv_usec / 1000));
    } 
    
    /* JRT */
    if ( (optionName == RTP_NET_SO_RECEIVE_BUFFER) ||
         (optionName == RTP_NET_SO_SEND_BUFFER) )
    {
        RTP_UINT32 temp = window_size;
        rtp_memcpy(optionVal, &temp, sizeof(RTP_UINT32));
    }
    
    return ret;
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
    tc_set_errno (0);
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
    tc_set_errno (0);
#endif

    option = (int) onBool;
    if (setsockopt((int) sockHandle, SOL_SOCKET,
                    SO_NAGLE, (RTP_PFCCHAR) &option,
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
    tc_set_errno (0);
#endif

    if (setsockopt((int) sockHandle, SOL_SOCKET,
                    SO_LINGER, (RTP_PFCCHAR) &arg,
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
    tc_set_errno (0);
#endif

    if ( setsockopt((int) sockHandle,
                    SOL_SOCKET,
                    SO_REUSESOCK,
                    (RTP_PFCCHAR) &onBool,
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
    tc_set_errno (0);
#endif

    if ( setsockopt((int) sockHandle,
                    SOL_SOCKET,
                    SO_REUSEADDR,
                    (RTP_PFCCHAR) &onBool,
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
    tc_set_errno (0);
#endif

    if ( setsockopt((int) sockHandle,
                    SOL_SOCKET,
                    SO_TCP_NO_COPY,
                    (RTP_PFCCHAR) &onBool,
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
    tc_set_errno (0);
#endif

    if (setsockopt((int) sockHandle, SOL_SOCKET,
                    SO_KEEPALIVE, (RTP_PFCCHAR)&onBool,
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
    rtp_memset(&localAddr, 0, localLen);
    rtp_memset(&sin, 0, sizeof (sin));

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
    tc_set_errno (0);
#endif

    if (getsockname ((int) sockHandle, (PSOCKADDR) &localAddr, (RTP_PFINT) &localLen) != 0)
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
                            IP_ADD_MEMBERSHIP, (RTP_PFCCHAR)&mcreq,
                            sizeof (mcreq));
    }
    else
    {
        result = setsockopt((int) sockHandle, IPPROTO_IP,
                            IP_DROP_MEMBERSHIP, (RTP_PFCCHAR)&mcreq,
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
        return (-1);
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
    tc_set_errno (0);
#endif

    result = setsockopt( (SOCKET)sockHandle, IPPROTO_IP,
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

#if (!INCLUDE_RTIP)
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

#endif //#if (!INCLUDE_RTIP) /* _YI_ 9/7/2005 */

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
    tc_set_errno (0);
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
            writeList->fdArray[index] = (RTP_SOCKET)write_set.sockets[index];
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
            readList->fdArray[index] = (RTP_SOCKET)read_set.sockets[index];
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
            errorList->fdArray[index] = (RTP_SOCKET)error_set.sockets[index];
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

/*----------------------------------------------------------------------*
                           rtp_net_set_ip
 *----------------------------------------------------------------------*/
int rtp_net_set_ip(int ifaceNumber, RTP_PFUINT8 myIpAddress, RTP_PFUINT8 myBroadCastAddress)
{
    return (xn_set_ip(ifaceNumber,
                   myIpAddress,
                   myBroadCastAddress));
}
/*----------------------------------------------------------------------*
                           rtp_net_invoke_input
 *----------------------------------------------------------------------*/
void rtp_net_invoke_input(PIFACE pi, DCU msg, int length)
{
    ks_invoke_input(pi, msg, length);
}

/*----------------------------------------------------------------------*
                           rtp_net_invoke_output
 *----------------------------------------------------------------------*/
void rtp_net_invoke_output(PIFACE pi, int xmit_complete_cnt)
{
    ks_invoke_output(pi, xmit_complete_cnt);
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
