/*
|  WEBCSOCK.C - WebC sockets porting layer
|
|  EBS -
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "webcsock.h"
#include "webcstr.h"
#include "webc.h"
#include "rtptime.h"
#include "rtpprint.h"

#if (WEBC_SUPPORT_DEBUG_PROFILE)
extern unsigned long total_bytes_received;
#endif
/*****************************************************************************/
/* Macros
 *****************************************************************************/
#ifndef WEBC_SUPPORT_SOCKETS
#define WEBC_SUPPORT_SOCKETS 1
#endif

#if (!defined(VXWORKS))

#if (defined(WEBC_MQX))
#define WEBC_SOCKET_NONBLOCK 0
#else
#define WEBC_SOCKET_NONBLOCK 1 // 1
#endif

#else
#define WEBC_SOCKET_NONBLOCK 1
#endif

#define SELECT_MSEC             20     // number of msec to block in
#define DO_IDLE_FUNCTION        1      // 0 to disable, 1 to enable

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

static WEBC_BOOL gWebcNetworkInitialized = WEBC_FALSE;

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*************************************************************************
 webc_network_init() - Initialize TCP/IP Networking

 Returns: 0 on success, -1 otherwise.
*************************************************************************/

int webc_network_init (void)
{
	if (!gWebcNetworkInitialized)
	{
		if (rtp_net_init() < 0)
		{
			return (-1);
		}
		gWebcNetworkInitialized = WEBC_TRUE;
	}

	return (0);
}


/*************************************************************************
 webc_network_close() - Do any TCP/IP related cleanup

 Returns: 0 on success, -1 otherwise.
*************************************************************************/

int webc_network_close (void)
{
	if (gWebcNetworkInitialized)
	{
		gWebcNetworkInitialized = WEBC_FALSE;
		rtp_net_exit();
	}
	return (0);
}



/*************************************************************************
 webc_socket() - Allocate a socket, set to non-blocking

 psd - pointer to WEBC_SOCKET_t to be initialized

 Returns: 0 on success, -1 otherwise.
*************************************************************************/

int webc_socket (WEBC_SOCKET_t *psd)
{
	if (rtp_net_socket_stream(psd) >= 0)
	{
		/* set the socket to hard-close (linger on, timeout 0) */
		if (rtp_net_setlinger(*psd, 1, 0) >= 0)
		{
			/* turn off nagle's delayed-send algorithm; this is sometimes
			   called TCP_NODELAY */
			rtp_net_setnagle(*psd, 0);

#if (WEBC_SOCKET_NONBLOCK)
			/* set the socket to use non-blocking I/O */
			if (rtp_net_setblocking(*psd, 0) >= 0)
#endif
			{
				return (0);
			}
		}

		rtp_net_closesocket(*psd);
	}
	return (-1);
}


/*************************************************************************
 webc_closesocket() - Terminate a socket connection

 sd - socket to close

 Returns: 0 on success, -1 otherwise.
*************************************************************************/

int webc_closesocket (WEBC_SOCKET_t sd)
{
	return (rtp_net_closesocket(sd));
}


/*************************************************************************
 webc_connect() - Establish a TCP connection to a particular ip/port

 sd - socket to use for this connection
 ip_addr - 4 byte_ array containing IP address to connect to
 port - port number to connect to

 Returns: 0 on success, -1 otherwise.
*************************************************************************/

int webc_connect (WEBC_SOCKET_t sd, WEBC_UINT8* ip_addr, WEBC_UINT16 port)
{
int result;
	result = rtp_net_connect(sd, ip_addr, port, 4);
	if (result < 0)
	{
#if (WEBC_SOCKET_NONBLOCK)
		if (result == -2)
		{
			return (0);
		}
#endif
		return (-1);
	}

	return (0);
}


/*************************************************************************
 webc_recv() - receive data over a socket
 webc_s_recv() - receive data over a secure socket

 sd - socket to receive data over
 buffer - buffer to place data into
 size - the size of the buffer (max bytes to read)

 Returns: number of bytes received on success, < 0 on error
*************************************************************************/
#if(WEBC_CALL_SERVER_ON_LOOPBACK)
int RtipCheckIfLoopBack(int sd);
#endif
int  WebCReadData(char *p, int buffersize);

#define DIAGNOSE_SOCKETS 0
#define USE_EXPERIMENTAL_RECEIVE 1
#if (USE_EXPERIMENTAL_RECEIVE)
int webc_recv_at_least (WEBC_SOCKET_t sd, char * buffer, long min_bytes, long max_bytes, webcIdleFn idle_func, WEBC_PFBYTE idle_data)
{

  #if (!WEBC_SOCKET_NONBLOCK)
	return (rtp_net_recv(sd, (unsigned char *)buffer, max_bytes));
  #else

	RTP_FD_SET f_read;
	RTP_FD_SET f_error;
	WEBC_UINT8* pkt_data;
	WEBC_INT32  pkt_len;
	WEBC_INT32  bytes_received;
	WEBC_UINT32 start_time, elap_time_msec;
	int select_val;
    /* Bypass the network stack when fetching web pages using the loopback interface in polled mode. */
#if(WEBC_CALL_SERVER_ON_LOOPBACK)
    if (RtipCheckIfLoopBack(sd))
    {
        return  WebCReadData((char *)buffer, min_bytes);
    }
#endif

	bytes_received = 0;
	start_time = rtp_get_system_msec();

#if (DIAGNOSE_SOCKETS)
	rtp_printf("(%-6.6d) - max bytes == %d\n", start_time, max_bytes);
#endif

	while (bytes_received < min_bytes)
	{
		while (1)
		{

			rtp_fd_zero(&f_read);
			rtp_fd_set(&f_read, sd);

			rtp_fd_zero(&f_error);
			rtp_fd_set(&f_error, sd);

			select_val = rtp_net_select(&f_read, 0, &f_error, SELECT_MSEC);

			if (rtp_fd_isset(&f_error, sd))
			{
				return (-1);
			}

			/* return value of 0 indicates no sockets selected this time */
			if (select_val > 0)
			{
				break;
			}

			elap_time_msec = rtp_get_system_msec() - start_time;
			if (elap_time_msec > WEBC_TIMEOUT_SEC * 1000)
			{
				return (-1);
			}
#if (DO_IDLE_FUNCTION)
			if (idle_func)
			{
#if (DIAGNOSE_SOCKETS)
			unsigned long intoidle =  rtp_get_system_msec();
#endif
				if (idle_func(idle_data) < 0)
				{
					return (-1);
				}
#if (DIAGNOSE_SOCKETS)
				rtp_printf("(%-6.6d) Idle (1) for == %d\n", intoidle, rtp_get_system_msec()-intoidle);
#endif
			}
#endif
		} /* while (1) */

#if (DIAGNOSE_SOCKETS)
		{
		unsigned long intoread =  rtp_get_system_msec();
#endif
		pkt_data = (unsigned char *) &(buffer[bytes_received]);
 		pkt_len = rtp_net_recv(sd, pkt_data, max_bytes - bytes_received);
#if (DIAGNOSE_SOCKETS)
		rtp_printf("(%-6.6d) elapsed == (%-4.4d) received == (%-6.6d)\n", intoread, rtp_get_system_msec()-intoread, pkt_len);
		}
#endif

	 	if (pkt_len == 0 || pkt_len == -2)
	 	{
	 		break;
	 	}

	 	if (pkt_len < 0)
	 	{
			return (pkt_len);
		}
#if (WEBC_SUPPORT_DEBUG_PROFILE)
		total_bytes_received += pkt_len;
#endif
 		bytes_received += pkt_len;
	}
	return (bytes_received);

  #endif // WEBC_SOCKET_NONBLOCK
}
#else
int webc_recv_at_least (WEBC_SOCKET_t sd, char * buffer, long min_bytes, long max_bytes, webcIdleFn idle_func, WEBC_PFBYTE idle_data)
{
  #if (!WEBC_SOCKET_NONBLOCK)
	return (rtp_net_recv(sd, (unsigned char *)buffer, max_bytes));
  #else

	RTP_FD_SET f_read;
	RTP_FD_SET f_error;
	WEBC_UINT8* pkt_data;
	WEBC_INT32  pkt_len;
	WEBC_INT32  bytes_received;
	WEBC_UINT32 start_time, elap_time_msec;
	int select_val;

	bytes_received = 0;
	start_time = rtp_get_system_msec();

#if (DIAGNOSE_SOCKETS)
	rtp_printf("(%-6.6d) - max bytes == %d\n", start_time, max_bytes);
#endif

	while (bytes_received < min_bytes)
	{
		while (1)
		{
		  #if (DO_IDLE_FUNCTION)
			if (idle_func)
			{
#if (DIAGNOSE_SOCKETS)
			unsigned long intoidle =  rtp_get_system_msec();
#endif
				if (idle_func(idle_data) < 0)
				{
					return (-1);
				}
#if (DIAGNOSE_SOCKETS)
				rtp_printf("(%-6.6d) Idle (1) for == %d\n", intoidle, rtp_get_system_msec()-intoidle);
#endif
			}
		  #endif

			rtp_fd_zero(&f_read);
			rtp_fd_set(&f_read, sd);

			rtp_fd_zero(&f_error);
			rtp_fd_set(&f_error, sd);

			select_val = rtp_net_select(&f_read, 0, &f_error, SELECT_MSEC);

			if (rtp_fd_isset(&f_error, sd))
			{
				return (-1);
			}

			/* return value of 0 indicates no sockets selected this time */
			if (select_val > 0)
			{
				break;
			}

			elap_time_msec = rtp_get_system_msec() - start_time;
			if (elap_time_msec > WEBC_TIMEOUT_SEC * 1000)
			{
				return (-1);
			}

		} /* while (1) */

#if (DIAGNOSE_SOCKETS)
		{
		unsigned long intoread =  rtp_get_system_msec();
#endif
		pkt_data = (unsigned char *) &(buffer[bytes_received]);
 		pkt_len = rtp_net_recv(sd, pkt_data, max_bytes - bytes_received);
#if (DIAGNOSE_SOCKETS)
		rtp_printf("(%-6.6d) elapsed == (%-4.4d) received == (%-6.6d)\n", intoread, rtp_get_system_msec()-intoread, pkt_len);
		}
#endif

	 	if (pkt_len == 0 || pkt_len == -2)
	 	{
	 		break;
	 	}

	 	if (pkt_len < 0)
	 	{
			return (pkt_len);
		}
#if (WEBC_SUPPORT_DEBUG_PROFILE)
		total_bytes_received += pkt_len;
#endif
 		bytes_received += pkt_len;
	}

#if (DO_IDLE_FUNCTION)
	if (idle_func)
	{
#if (DIAGNOSE_SOCKETS)
		unsigned long intoidle =  rtp_get_system_msec();
#endif
		if (idle_func(idle_data) < 0)
		{
			return (-1);
		}
#if (DIAGNOSE_SOCKETS)
		rtp_printf("(%-6.6d) Idle (2) for == %d\n", intoidle, rtp_get_system_msec()-intoidle);
#endif
	}
#endif

	return (bytes_received);

  #endif // WEBC_SOCKET_NONBLOCK
}
#endif // USE_EXPERIMENTAL_RECEIVE

/*************************************************************************
 webc_send() - send data over a socket
 webc_s_send() - send data over a secure socket

 sd - socket to send data over
 buffer - data to send
 size - the size of the buffer (max bytes to send)

 Returns: number of bytes sent on success, < 0 on error
*************************************************************************/
int webc_send (WEBC_SOCKET_t sd, char * buffer, long size, webcIdleFn idle_func, WEBC_PFBYTE idle_data)
{
  #if (!WEBC_SOCKET_NONBLOCK)
	return (rtp_net_send(sd, (const unsigned char *)buffer, size));
  #else

	RTP_FD_SET f_write;
	RTP_FD_SET f_error;
	WEBC_UINT8* pkt_data;
	WEBC_INT32  pkt_len;
	WEBC_INT32  bytes_sent;
	WEBC_UINT32 start_time, elap_time_msec;
	int select_val;

	bytes_sent = 0;
	start_time = rtp_get_system_msec();

	while (bytes_sent < size)
	{
		while (1)
		{
		  #if (DO_IDLE_FUNCTION)
			if (idle_func)
			{
				if (idle_func(idle_data) < 0)
				{
					return (-1);
				}
			}
		  #endif

			rtp_fd_zero(&f_write);
			rtp_fd_set(&f_write, sd);

			rtp_fd_zero(&f_error);
			rtp_fd_set(&f_error, sd);

			select_val = rtp_net_select(0, &f_write, &f_error, SELECT_MSEC);

			if (rtp_fd_isset(&f_error, sd))
			{
				return (-1);
			}

			/* return value of 0 indicates no sockets selected this time */
			if (select_val > 0)
			{
				break;
			}

			elap_time_msec = rtp_get_system_msec() - start_time;
			if (elap_time_msec > WEBC_TIMEOUT_SEC * 1000)
			{
				return (-1);
			}
		} /* while (1) */

		pkt_data = (unsigned char *) &(buffer[bytes_sent]);


 		pkt_len = rtp_net_send(sd, pkt_data, size - bytes_sent);
#if (DIAGNOSE_SOCKETS)
		rtp_printf("(%6.6d) sent %d bytes\n", rtp_get_system_msec() ,pkt_len);
#endif
	 	if (pkt_len == 0 || pkt_len == -2)
	 	{
	 		break;
	 	}

	 	if (pkt_len < 0)
	 	{
			return (pkt_len);
		}

 		bytes_sent += pkt_len;
	}

  #if (DO_IDLE_FUNCTION)
	if (idle_func)
	{
		if (idle_func(idle_data) < 0)
		{
			return (-1);
		}
	}
  #endif

	return (bytes_sent);

  #endif // WEBC_SOCKET_NONBLOCK
}


/*************************************************************************
 webc_gethostipaddr() - Convert a host name into an IP address

 ipaddr - 4 byte_ array to fill with the IP address
 name - host name to look up

 Returns: 0 on success, WEBC_EDNSFAILED (<0) on error
*************************************************************************/

int webc_gethostipaddr (WEBC_PFBYTE ipaddr, char * name)
{
	int type;

  #if (WEBC_SUPPORT_SOCKETS)
	// first try the name as a numbered IP address
	if (IS_DIGIT(name[0]))
	{
		char testStr[16];

		webc_str_to_ip(ipaddr, name);

		// do a check to make sure we translated an IP address
		webc_ip_to_str(testStr, ipaddr);
		if (!tc_strcmp(name, testStr))
		{
			// if we go from string to ip addr and back, without
			//  changing the host name, this is success.
			return (0);
		}
	}
  #endif // WEBC_SUPPORT_SOCKETS

	return (rtp_net_gethostbyname(ipaddr, &type, name));
}


/*************************************************************************
 webc_ip_to_str() - Convert 4 WEBC_UINT8 ip address to dotted string

 ipstr - 13 char array : the buffer to fill with dotted string
 ipaddr - 4 byte_ array : the ip address to convert

 Returns: ipstr
*************************************************************************/

char * webc_ip_to_str(char * ipstr, WEBC_PFBYTE ipaddr)
{
int n;

	ipstr[0] = '\0';

	for (n=0; n<4; n++)
	{
		tc_itoa(ipaddr[n], &(ipstr[tc_strlen(ipstr)]), 10);
		if (n<3)
		{
			tc_strcat(ipstr, ".");
		}
	}

	return (ipstr);
}


/*************************************************************************
 webc_str_to_ip() - Convert dotted string to 4 WEBC_UINT8 ip address

 ipaddr -  a 4-byte_ buffer to fill with the ip address
 ipstr -   the dotted string to convert

 Notes:
 	"111.111.111.111" converts to {111,111,111,111}
 	"111.111.111"     converts to {111,111,111,0}
 	"111.111"         converts to {111,111,0,0}
 	"111"             converts to {111,0,0,0}

 Returns: ipaddr
*************************************************************************/

WEBC_PFBYTE webc_str_to_ip(WEBC_PFBYTE ipaddr, char * ipstr)
{
char * ptr;
char savech;
int n;

	tc_memset(ipaddr, 0, 4);

	for (n=0; n<4; n++)
	{
		ptr = ipstr;
		while (*ptr != '.' && *ptr != '\0')
		{
			ptr++;
		}

		savech = *ptr;
		*ptr = '\0';
		ipaddr[n] =  tc_atoi(ipstr);
		if (savech == '\0')
		{
			break;
		}
		*ptr = savech;

		ipstr = ptr + 1;
	}

	return (ipaddr);
}


/*************************************************************************
 webc_socket_is_connected() - Check whether a connection is still active

 sd - the socket to check

 Returns: non zero for true, 0 for false
*************************************************************************/

int webc_socket_is_connected (WEBC_SOCKET_t sd)
{
	return (rtp_net_is_connected(sd));
}


/*************************************************************************
 webc_socket_has_data_to_read() - Check whether a connection has data ready

 sd - the socket to check
 usec_tmo - microsecond timeout

 Returns: non zero for true, 0 for false
*************************************************************************/

int webc_socket_has_data_to_read (WEBC_SOCKET_t sd, long usec_tmo)
{
	return (rtp_net_read_select(sd, usec_tmo / 1000) >= 0);
}

/*
	 1 - ready to read
	 0 - timed out
	-1 - error on socket
	-2 - abort command received
*/

int webc_poll_socket (
		WEBC_SOCKET_t sd,
		webcIdleFn idle_func,
		WEBC_PFBYTE idle_data,
		WEBC_UINT32 untilMsec,
		int pollMode)
{
	RTP_FD_SET f_test;
	RTP_FD_SET* f_read;
	RTP_FD_SET* f_write;
	RTP_FD_SET f_error;
	int select_val;
	long timeToWait;
	long timeRemaining;

	while (1)
	{
		timeRemaining = (long) ((long) untilMsec - (long) rtp_get_system_msec());

		if (timeRemaining <= 0)
		{
			return WEBC_POLL_RESULT_TIMEOUT;
		}

	  #if (DO_IDLE_FUNCTION)
		if (idle_func)
		{
			if (idle_func(idle_data) < 0)
			{
				return WEBC_POLL_RESULT_ABORTED;
			}
		}
	  #endif

		timeToWait = EBSMIN(SELECT_MSEC, timeRemaining);

		rtp_fd_zero(&f_test);
		rtp_fd_set(&f_test, sd);

		if (pollMode == WEBC_POLL_READ)
		{
			f_read = &f_test;
			f_write = 0;
		}
		else
		{
			f_write = &f_test;
			f_read = 0;
		}

		rtp_fd_zero(&f_error);
		rtp_fd_set(&f_error, sd);

		select_val = rtp_net_select(f_read, f_write, &f_error, timeToWait);

		if (rtp_fd_isset(&f_error, sd))
		{
			return WEBC_POLL_RESULT_SOCKERR;
		}

		/* return value of 0 indicates no sockets selected this time */
		if (select_val > 0)
		{
			break;
		}
	} /* while (1) */

	return WEBC_POLL_RESULT_READY;
}

#if (WEBC_SUPPORT_HTTPS)

/*************************************************************************
 webc_recv() - receive data over a socket
 webc_s_recv() - receive data over a secure socket

 sd - socket to receive data over
 buffer - buffer to place data into
 size - the size of the buffer (max bytes to read)

 Returns: number of bytes received on success, < 0 on error
*************************************************************************/
int webc_s_recv_at_least (RTP_SOCKET sd, char * buffer, long min_bytes, long max_bytes, webcIdleFn idle_func, WEBC_PFBYTE idle_data, RTP_SSL_SESSION ssl)
{
  #if (!WEBC_SOCKET_NONBLOCK)
	return (rtp_net_recv(sd, (unsigned char *)buffer, max_bytes));
  #else
	WEBC_INT32  pkt_len;
	WEBC_INT32  bytes_received;
	WEBC_UINT32 end_time;
	int pollResult;

	bytes_received = 0;
	end_time = rtp_get_system_msec() + (WEBC_TIMEOUT_SEC * 1000);

	do
	{
		// pkt_len must be a signed int!
		pkt_len = rtp_ssl_recv(ssl, (unsigned char *) (buffer + bytes_received), max_bytes - bytes_received, WEBC_FALSE);
		if (pkt_len < 0)
		{
			if (pkt_len == RTP_SSL_ERR_WANT_READ)
			{
				pollResult = webc_poll_socket(sd, idle_func, idle_data, end_time, WEBC_POLL_READ);
			}
			else if (pkt_len == RTP_SSL_ERR_WANT_WRITE)
			{
				pollResult = webc_poll_socket(sd, idle_func, idle_data, end_time, WEBC_POLL_WRITE);
			}
			else
			{
				return -1;
			}

			if (pollResult == WEBC_POLL_RESULT_TIMEOUT && bytes_received > 0)
			{
				break;
			}

			if (pollResult != WEBC_POLL_RESULT_READY)
			{
				return -1;
			}
		}
		else if (pkt_len == 0)
		{
			break;
		}
		else // (pkt_len > 0)
		{
			bytes_received += pkt_len;
		}
	}
	while (bytes_received < min_bytes);

	return (bytes_received);

  #endif // WEBC_SOCKET_NONBLOCK
}

/*************************************************************************
 webc_send() - send data over a socket
 webc_s_send() - send data over a secure socket

 sd - socket to send data over
 buffer - data to send
 size - the size of the buffer (max bytes to send)

 Returns: number of bytes sent on success, < 0 on error
*************************************************************************/
int webc_s_send (WEBC_SOCKET_t sd, char * buffer, long size, webcIdleFn idle_func, WEBC_PFBYTE idle_data, RTP_SSL_SESSION ssl)
{
	WEBC_INT32  pkt_len;
	WEBC_INT32  bytes_sent;
	WEBC_UINT32 end_time;
	int pollResult;

	bytes_sent = 0;
	end_time = rtp_get_system_msec() + (WEBC_TIMEOUT_SEC * 1000);

	do
	{
		// pkt_len must be a signed int!
		pkt_len = rtp_ssl_send(ssl, (const unsigned char *) (buffer + bytes_sent), size - bytes_sent, WEBC_FALSE);
		if (pkt_len < 0)
		{
			if (pkt_len == RTP_SSL_ERR_WANT_READ)
			{
				pollResult = webc_poll_socket(sd, idle_func, idle_data, end_time, WEBC_POLL_READ);
			}
			else if (pkt_len == RTP_SSL_ERR_WANT_WRITE)
			{
				pollResult = webc_poll_socket(sd, idle_func, idle_data, end_time, WEBC_POLL_WRITE);
			}
			else
			{
				return -1;
			}

			if (pollResult == WEBC_POLL_RESULT_TIMEOUT && bytes_sent > 0)
			{
				break;
			}

			if (pollResult != WEBC_POLL_RESULT_READY)
			{
				return -1;
			}
		}
		else if (pkt_len == 0)
		{
			break;
		}
		else // (pkt_len > 0)
		{
			bytes_sent += pkt_len;
		}
	}
	while (bytes_sent < size);

	return (bytes_sent);
}

#endif // WEBC_SUPPORT_HTTPS
