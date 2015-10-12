 /*
 | RTPNET.H - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform
 |
 |  $Author: vmalaiya $
 |  $Date: 2006/03/23 21:49:19 $
 |  $Name:  $
 |  $Revision: 1.5 $
 |
 | Copyright EBS Inc. , 2006
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/

#ifndef __RTPNET_H__
#define __RTPNET_H__

#include "rtp.h"

typedef RTP_HANDLE RTP_SOCKET;

#define RTP_IPV6_IPV4_CALLS  0

/************************************************************************
 * Defines
 ************************************************************************/
#ifdef __TIARM__
#define RTP_FD_SET_MAX      10
#else
#define RTP_FD_SET_MAX      20
#endif

#define RTP_NET_TYPE_IPV4   4
#define RTP_NET_TYPE_IPV6   6
#define RTP_NET_IP_ALEN     4
#define RTP_NET_DATAGRAM    0
#define RTP_NET_STREAM      1
#define RTP_NET_NI_MAXHOST  1025

/************************************************************************
 * Typedefs
 ************************************************************************/
/************************************************************************
 * RTP_FD_SET - socket handle list for rtp_net_select                   *
 ************************************************************************/
typedef struct s_RTP_FD_SET
{
    int                fdCount;
    RTP_SOCKET         fdArray[RTP_FD_SET_MAX];
}
RTP_FD_SET;

/************************************************************************
 * RTP_NET_ADDR - network address structure                             *
 ************************************************************************/
typedef struct s_RTP_NET_ADDR
{
	unsigned char      ipAddr[RTP_NET_NI_MAXHOST];
	int                port;
	int                type;
}
RTP_NET_ADDR;

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 * Initialize/Shutdown the API                                          *
 ************************************************************************/

int  rtp_net_init            (void);
void rtp_net_exit            (void);


/************************************************************************
 * Sockets API                                                          *
 ************************************************************************/

/* general sockets calls */

int      rtp_net_socket_stream   (RTP_SOCKET *sockHandle);
int      rtp_net_socket_datagram (RTP_SOCKET *sockHandle);
int      rtp_net_bind            (RTP_SOCKET sockHandle, unsigned char *ipAddr,
                                  int port, int type);
int      rtp_net_connect         (RTP_SOCKET sockHandle, unsigned char *ipAddr,
                                  int port, int type);
int      rtp_net_listen          (RTP_SOCKET sockHandle, int queueSize);
int      rtp_net_getsockname     (RTP_SOCKET sockHandle, unsigned char *ipAddr,
                                  int *port, int *type);
int      rtp_net_gethostbyname   (unsigned char *ipAddr, int *type, char *name);
int      rtp_net_accept          (RTP_SOCKET *connectSock,
                                  RTP_SOCKET serverSock,unsigned char *ipAddr,
                                  int *port, int *type);
unsigned rtp_net_is_connected    (RTP_SOCKET sockHandle);
int      rtp_net_write_select    (RTP_SOCKET sockHandle, long msecTimeout);
int      rtp_net_read_select     (RTP_SOCKET sockHandle, long msecTimeout);
long     rtp_net_send            (RTP_SOCKET sockHandle,
                                  const unsigned char *buffer, long size);
long     rtp_net_recv            (RTP_SOCKET sockHandle, unsigned char *buffer,
                                  long size);
long     rtp_net_sendto          (RTP_SOCKET sockHandle,
                                  const unsigned char *buffer, long size,
                                  unsigned char *ipAddr, int port, int type);
long     rtp_net_recvfrom        (RTP_SOCKET sockHandle, unsigned char *buffer,
                                  long size, unsigned char *ipAddr, int *port,
                                  int *type);
int      rtp_net_closesocket     (RTP_SOCKET sockHandle);
int      rtp_net_shutdown        (RTP_SOCKET sockHandle, int how);
int      rtp_net_getifaceaddr    (unsigned char *localAddr,
                                  unsigned char *remoteAddr, int remotePort,
                                  int remoteType);

/* Calls to support IPv4, IPv6, dual stack */
int      rtp_net_socket_stream_dual   (RTP_SOCKET *sockHandle, int type);
int      rtp_net_socket_datagram_dual (RTP_SOCKET *sockHandle, int type);
int      rtp_net_bind_dual            (RTP_SOCKET sockHandle, int sockType,
                                       unsigned char *ipAddr, int port,
                                       int type);
int      rtp_net_connect_dual         (RTP_SOCKET sockHandle, int sockType,
                                       unsigned char *ipAddr, int port,
                                       int type);
/* socket options */
int      rtp_net_getntoread      (RTP_SOCKET sockHandle,
                                  unsigned long * nToRead);
int      rtp_net_setblocking     (RTP_SOCKET sockHandle, unsigned int onBool);
int      rtp_net_setnagle        (RTP_SOCKET sockHandle, unsigned int onBool);
int      rtp_net_setlinger       (RTP_SOCKET sockHandle, unsigned int onBool,
                                  long msecTimeout);
int      rtp_net_setreusesock    (RTP_SOCKET sockHandle, unsigned int onBool);
int      rtp_net_setreuseaddr    (RTP_SOCKET sockHandle, unsigned int onBool);
int      rtp_net_settcpnocopy    (RTP_SOCKET sockHandle, unsigned int onBool);
int      rtp_net_setkeepalive    (RTP_SOCKET sockHandle, unsigned int onBool);
int      rtp_net_setmembership   (RTP_SOCKET sockHandle, unsigned char * ipAddr,
                                  int type, unsigned int onBool);
int      rtp_net_setmcastttl     (RTP_SOCKET sockHandle, int ttl);
int      rtp_net_setbroadcast    (RTP_SOCKET sockHandle, unsigned int onBool);

/* utility functions */
short    rtp_net_htons           (short i);
short    rtp_net_ntohs           (short i);
long     rtp_net_htonl           (long i);
long     rtp_net_ntohl           (long i);
int      rtp_net_ip_to_str       (char *str, unsigned char *ipAddr, int type);
int      rtp_net_str_to_ip       (unsigned char *ipAddr, char *str, int *type);

/* FD_SET functions */
void     rtp_fd_zero             (RTP_FD_SET  *list);
void     rtp_fd_set              (RTP_FD_SET  *list, RTP_SOCKET fd);
void     rtp_fd_clr              (RTP_FD_SET  *list, RTP_SOCKET fd);
int      rtp_fd_isset            (RTP_FD_SET  *list, RTP_SOCKET fd);

int      rtp_net_select          (RTP_FD_SET  *readList, RTP_FD_SET  *writeList,
                                  RTP_FD_SET  *errorList, long  msecTimeout);

#ifdef __cplusplus
}
#endif

#endif /* __RTPNET_H__ */

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
