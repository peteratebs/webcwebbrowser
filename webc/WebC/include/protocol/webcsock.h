/*
|  WEBCSOCK.H - 
| 
|  EBS - 
| 
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __WEBCSOCK_H__
#define __WEBCSOCK_H__

#include "webc.h"
#include "rtpnet.h"
#if (WEBC_SUPPORT_HTTPS)
#include "rtpssl.h"
#endif

#if (INCLUDE_RTIP)
/* This needs further refinement if multitasking */
#define WEBC_CALL_SERVER_ON_LOOPBACK 1 
#else
#define WEBC_CALL_SERVER_ON_LOOPBACK 0
#endif

typedef RTP_SOCKET WEBC_SOCKET_t;

#define WEBC_POLL_READ  0
#define WEBC_POLL_WRITE 1

#define WEBC_POLL_RESULT_READY     1
#define WEBC_POLL_RESULT_TIMEOUT   0
#define WEBC_POLL_RESULT_SOCKERR  -1
#define WEBC_POLL_RESULT_ABORTED  -2

#ifdef __cplusplus
extern "C" {
#endif

int         webc_network_init            (void);

int         webc_network_close           (void);

int         webc_socket                  (WEBC_SOCKET_t *psd);

int         webc_closesocket             (WEBC_SOCKET_t sd);

int	        webc_connect                 (WEBC_SOCKET_t sd, 
                                          WEBC_UINT8* ip_addr, 
                                          WEBC_UINT16 port);

int         webc_send                    (WEBC_SOCKET_t sd, 
                                          char* buffer, 
                                          long size, 
                                          webcIdleFn idle_func, 
                                          WEBC_UINT8* idle_data);

int         webc_gethostipaddr           (unsigned char* ipaddr, 
                                          char* name);

char*       webc_ip_to_str               (char* ipstr, 
                                          unsigned char* ipaddr);

WEBC_UINT8* webc_str_to_ip               (unsigned char* ipaddr, 
                                          char* ipstr);

int         webc_socket_is_connected     (WEBC_SOCKET_t sd);

int         webc_recv_at_least           (WEBC_SOCKET_t sd, 
                                          char * buffer, 
                                          long min_bytes, 
                                          long max_bytes, 
                                          webcIdleFn idle_func, 
                                          WEBC_UINT8* idle_data);

int         webc_socket_has_data_to_read (WEBC_SOCKET_t sd, 
                                          long usec_tmo);

int         webc_poll_socket             (WEBC_SOCKET_t sd, 
                                          webcIdleFn idle_func, 
                                          WEBC_PFBYTE idle_data, 
                                          WEBC_UINT32 untilMsec,
                                          int pollMode);

#if (WEBC_SUPPORT_HTTPS)

int         webc_s_send                  (WEBC_SOCKET_t sd, 
                                          char *buffer, 
                                          long size, 
                                          webcIdleFn idle_func,
                                          WEBC_UINT8* idle_data,
                                          RTP_SSL_SESSION ssl);

int         webc_s_recv_at_least         (WEBC_SOCKET_t sd, 
                                          char *buffer, 
                                          long min, 
                                          long max, 
                                          webcIdleFn idle_func, 
                                          WEBC_UINT8* idle_data,
                                          RTP_SSL_SESSION ssl);

#endif // WEBC_SUPPORT_HTTPS

#ifdef __cplusplus
}
#endif

#endif /* __WEBCSOCK_H__ */
