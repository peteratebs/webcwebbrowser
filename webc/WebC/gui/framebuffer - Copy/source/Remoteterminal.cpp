#include "webc.h"
// #include "FB_HWInterface.h"
#include "webcmem.h"
//#include "assert.h"
#include "FBConfig.h"
#include "rtpprint.h"
#include "Remoteterminal.h"

#include "rtpnet.h"
#include "rtpprint.h"
#define SUPPORT_PARTIAL_NETDRAW 1

#if (INCLUDE_REMOTE_TERMINAL)


static int remote_teminal_role; /* 0 = disabled, 1 == display, 2 == headless device */
static WEBC_UINT8 *TermData;
//static WEBC_UINT8 *TermInputData;
static int TermBytesPerRow;
static int TermBytesPerPix;
//static int TermHeight;
static int term_initialized = 0;
int network_display_enabled = 1;
// static unsigned long display_bytes_buffered = 0;
#define TERM_DUMB_DISPLAY    	1
#define TERM_HEADLESS_DEVICE    2
#define TERM_BITBUCKET 0 /* Disable display so you can run without connecting to the net */
#define TERM_VERBOSE 1   /* 0 == no output, 1 == errors only 2 == chatty 3 == very chatty */
#define DISPLAY_OVER_NETWORK 1 /* Set to zero to not send/recv over network but do display */

int timeout_called = 0;
int send_called = 0;

extern "C" {
void webC_SignalAccept(void);
}




static int headless_terminal_make_connection(void);

WEBC_UINT8 *targIpaddress;
WEBC_UINT8 *myIpaddress;

#ifndef WIN32
	extern unsigned long fakeWindowHandle;
#endif

#define SELECT_MSEC_WRITE 0
#define SELECT_MSEC   0
#define SELECTTIMEVAL 0

#define USE_NON_BLOCKING 1
WEBC_UINT8 termIpaddress[4] = {192,168,0,17};
WEBC_UINT8 headlessIpaddress[4] = {192,168,0,118};

static long ComsockHandle, tcpsrvsockhandle;

static void call_TerminalInit(int whoami);
static void rterm_send(unsigned long sockHandle, WEBC_UINT8 *b, unsigned long length);
static unsigned long rterm_recv(unsigned long sockHandle, WEBC_UINT8 *b, unsigned long maxlength);
#ifdef WIN32
static void ReceiveDisplayData(unsigned long sockHandle);
static int terminal_accept_connection(void);
#endif

extern "C" {void CONIO_Output_String_Time(char *);}

static void ReceiveMessageData(unsigned long sockHandle, HBROWSER_HANDLE hbrowser);
// static WEBC_UINT8 *get_SendTerminalDisplay(DISPLAY_INT x, DISPLAY_INT y);
static int headless_terminal_make_connection(void);

void start_HeadlessTerminal(void)
{
	if (network_display_enabled)
	{
		call_TerminalInit(TERM_HEADLESS_DEVICE);
		if (headless_terminal_make_connection() == 0)
			remote_teminal_role = TERM_HEADLESS_DEVICE;
	}
	else
		remote_teminal_role = TERM_HEADLESS_DEVICE;
}


void set_TerminalCanvas(DISPLAY_INT height, int BytesPerRow, int BytesPerPix)
{
	TermData =  (WEBC_UINT8*)WEBC_MALLOC(BytesPerRow * height + 32);
//	TermInputData = TermData;
//	TermHeight = height;
	TermBytesPerRow = BytesPerRow;
	TermBytesPerPix = BytesPerPix;
}


WEBC_UINT8 *get_TerminalDisplayBuffer(void)
{
	if (TermData && remote_teminal_role == TERM_HEADLESS_DEVICE)
	{
		return(TermData);
	}
	else
		return(0);
}

int send_TerminalDisplayBuffer(WEBC_UINT8 *srcPtr,HTMLRect rect)
{
int width,height;
send_called += 1;
// rtp_printf("Number of calls to timeout == %d to send == %d\n", timeout_called, send_called);
// rtp_printf("Dirty == %d %d %d %d \n", rect.top,	rect.left,	rect.bottom,rect.right);

#if (SUPPORT_PARTIAL_NETDRAW)
	if (TermData && remote_teminal_role == TERM_HEADLESS_DEVICE)
	{
		WEBC_UINT8 *remotePtr = (WEBC_UINT8*) get_TerminalDisplayBuffer();
		if (remotePtr)
		{	// Send data over the network as it was rendered
			int y,x;
			WEBC_UINT8* destRowPtr = remotePtr;
			WEBC_UINT8* srcRowPtr = (WEBC_UINT8*)srcPtr;
			WEBC_UINT8* srcColPtr,*destColPtr;
			WEBC_UINT32 *pHeader;

			if (rect.right > (WEBC_SCREEN_WIDTH -1)) // 319
				rect.right = (WEBC_SCREEN_WIDTH -1); //319;
			if (rect.bottom > (WEBC_SCREEN_HEIGHT -1) ) //239)
				rect.bottom = (WEBC_SCREEN_HEIGHT -1);//239;

			width = (rect.right - rect.left + 1);

			rtp_printf("send_TerminalDisplayBuffer:Sending top, left, bottom right %d, %d, %d, %d\n", rect.top, rect.left, rect.bottom, rect.right);

			pHeader = (WEBC_UINT32*) remotePtr;
			*pHeader++ = rect.top;
			*pHeader++ = rect.left;
			*pHeader++ = rect.bottom;
			*pHeader++  = rect.right;


			destRowPtr = (WEBC_UINT8*) pHeader;
			srcRowPtr += ((rect.top * TermBytesPerRow) + rect.left);
			for (y = rect.top; y < rect.bottom; y++, destRowPtr += width, srcRowPtr += TermBytesPerRow)
			{
				srcColPtr = srcRowPtr;
				destColPtr = destRowPtr;
				for (x = 0; x < width; x++)
				{
					destColPtr[0] = srcColPtr[0];
					srcColPtr += 1;
					destColPtr += 1;
				}
			}

			rtp_printf("send_TerminalDisplayBuffer:Sending %d bytes\n", 16 + width*(rect.bottom-rect.top+1) );

		}
	    if (network_display_enabled)
			rterm_send(ComsockHandle, TermData, 16 + width*(rect.bottom-rect.top+1) );

		rtp_printf("send_TerminalDisplayBuffer:Returned from send\n");
#else
// this code has not been tested
	if (TermData && remote_teminal_role == TERM_HEADLESS_DEVICE)
	{

		WEBC_UINT8 *remotePtr = (WEBC_UINT8*) get_TerminalDisplayBuffer();
		if (remotePtr)
		{	// Send data over the network as it was rendered

			WEBC_UINT32 *pHeader;
		    int TermHeight;
			int y,x,width;
            TermHeight = WEBC_SCREEN_HEIGHT;

	   		 rect.top = 0;
	    	rect.left = 0;
	    	rect.right = (WEBC_SCREEN_WIDTH -1);//319;
	    	rect.bottom = (WEBC_SCREEN_HEIGHT -1);//239;

			pHeader = (WEBC_UINT32*) remotePtr;
			*pHeader++ = 0;//rect.top;
			*pHeader++ = 0;//rect.left;
			*pHeader++ = WEBC_SCREEN_HEIGHT;//320;//rect.bottom;
			*pHeader++  = WEBC_SCREEN_WIDTH;/240;//rect.right;

			WEBC_UINT8* destRowPtr = pHeader;
			WEBC_UINT8* srcRowPtr = (WEBC_UINT8*)srcPtr;
			WEBC_UINT8* srcColPtr,*destColPtr;

			destRowPtr = (WEBC_UINT8*) pHeader;

			width = TermBytesPerRow/TermBytesPerPix;

			for (y = 0; y < TermHeight; y++, destRowPtr += TermBytesPerRow, srcRowPtr += TermBytesPerRow)
			{
				srcColPtr = srcRowPtr;
				destColPtr = destRowPtr;
				for (x = 0; x < width; x++)
				{
					destColPtr[0] = srcColPtr[0];
					srcColPtr += 1;
					destColPtr += 1;
				}
			}
		}

	if (network_display_enabled)
		rterm_send(ComsockHandle, TermData, 16 + width*(rect.bottom-rect.top+1));

#endif

		return 1;
	}


	return 0;
}


void webc_timer_pulse(void);


void call_TerminalTimout(HBROWSER_HANDLE which_Browser)
{
	if (!network_display_enabled)
		return;
	timeout_called += 1;
	if (TermData && remote_teminal_role == TERM_HEADLESS_DEVICE)
		ReceiveMessageData(ComsockHandle,which_Browser);
//		webc_timer_pulse();	 do this later
/* See if we have any processing to do */
}


WEBC_UINT8 *get_TerminalDisplayLine(DISPLAY_INT x, DISPLAY_INT y)
{
	if (TermData && remote_teminal_role == TERM_DUMB_DISPLAY)
	{
		WEBC_UINT8 *ptr;
		ptr = (WEBC_UINT8*) TermData + (y * TermBytesPerRow) + (x * TermBytesPerPix);
		return(ptr);
	}
	return(0);
}



static void call_TerminalInit(int whoami)
{
	if (!network_display_enabled)
		return;
	rtp_net_init();
	if (whoami == TERM_HEADLESS_DEVICE)
	{
		targIpaddress = termIpaddress;
		myIpaddress =   headlessIpaddress;
	}
	else
	{
		targIpaddress = headlessIpaddress;
		myIpaddress = termIpaddress;
	}

	if (!term_initialized)
	{

		rtp_net_init();
		if (rtp_net_socket_stream (&tcpsrvsockhandle) < 0)
		{
#if (TERM_VERBOSE > 0)
			rtp_printf("TCP socket failed\n");
#endif
		}
		term_initialized = 1;
	}
}



static void rterm_send(unsigned long sd, WEBC_UINT8 *buffer, unsigned long size)
{
  #if (!USE_NON_BLOCKING)
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

//rtp_printf("Sending %d bytes llops == %d\n", size, num_loops);
#if (TERM_VERBOSE > 1)
CONIO_Output_String_Time("Sending");
#endif
	while (bytes_sent < size)
	{
		while (1)
		{
			rtp_fd_zero(&f_write);
			rtp_fd_set(&f_write, sd);

			rtp_fd_zero(&f_error);
			rtp_fd_set(&f_error, sd);

			select_val = rtp_net_select(0, &f_write, &f_error, SELECT_MSEC_WRITE);

			if (rtp_fd_isset(&f_error, sd))
			{
#if (TERM_VERBOSE > 0)
rtp_printf("Error return with sent == %d bytes\n", bytes_sent);
#endif
				return ;
			}

			/* return value of 0 indicates no sockets selected this time */
			if (select_val > 0)
			{
				break;
			}
			webC_SignalAccept();

//			elap_time_msec = rtp_get_system_msec() - start_time;
//			if (elap_time_msec > WEBC_TIMEOUT_SEC * 1000)
//			{
//				return (-1);
//			}
		} /* while (1) */
#define MYMAXTCPSENDSIZE (1024*8)
		pkt_data = (unsigned char *) &(buffer[bytes_sent]);
		int tosend = size - bytes_sent;
		tosend = tosend > MYMAXTCPSENDSIZE ? MYMAXTCPSENDSIZE : tosend;
 		pkt_len = rtp_net_send(sd, pkt_data, tosend);
#if (TERM_VERBOSE > 2)
			rtp_printf("Sent %d bytes\n", pkt_len);
#endif
		// HEREHERE hack
	 	if (0 && pkt_len == 0 || pkt_len == -2)
	 	{
	 		break;
	 	}

	 	if (0 && pkt_len < 0)
	 	{
			return;
		}
	    if (pkt_len > 0)
 			bytes_sent += pkt_len;
	}

#if (TERM_VERBOSE > 1)
CONIO_Output_String_Time("Done sent");
#endif
	return;

  #endif // WEBC_SOCKET_NONBLOCK
}


//#define MAXTCPRECVSIZE (32768/2)

static unsigned long rterm_recv(unsigned long sockHandle, WEBC_UINT8 *b, unsigned long maxlength)
{
unsigned long totalrecieved = 0;

	while (maxlength)
	{
	unsigned long ntorecv,nrecieved;
	int err;
	ntorecv = maxlength > MAXTCPRECVSIZE ? MAXTCPRECVSIZE : maxlength;
		nrecieved = 0;
		err = rtp_net_read_select (sockHandle, 1);
		if (err == 0)
		{
		int i;
			i = rtp_net_recv (sockHandle, b, ntorecv);

			if(i < 0)
			{
#if (TERM_VERBOSE > 0)
				rtp_printf("\n error: recv returned %d\n", i);
#endif
				return (0);
			}
			if (i > 0)
				nrecieved = (unsigned long) i;
		}
		if (nrecieved)
		{
			b += nrecieved;
			maxlength -= nrecieved;
			totalrecieved += nrecieved;
		}
		else
		{
			break;
	    }
	}
	return (totalrecieved);
}




static void ReceiveMessageData(unsigned long sockHandle, HBROWSER_HANDLE hbrowser)
{
	if (rtp_net_read_select (sockHandle, SELECTTIMEVAL) == 0)
	{
	unsigned long totalrecieved;
		HTMLEvent* e = (HTMLEvent*)WEBC_MALLOC(sizeof(HTMLEvent));

		totalrecieved = rterm_recv(sockHandle, (WEBC_UINT8 *) e, sizeof(HTMLEvent));
 		if (totalrecieved >= sizeof(HTMLEvent))
		{
#if (TERM_VERBOSE > 1)
			CONIO_Output_String_Time("Rcved event");
#endif
		    webc_BrowserSuspendDraw(hbrowser);
	 		webc_BrowserDispatchEvent(hbrowser, e);
	 		if (webc_BrowserGetDrawState (hbrowser) > 0)
		    	webc_BrowserResumeDraw(hbrowser);
	 		webc_BrowserDraw(hbrowser);
		}
		else
		{
#if (TERM_VERBOSE > 1)
			 rtp_printf("Rcved %d \n",totalrecieved);
#endif

		}
	}
	else
	{
		webC_SignalAccept();
	}
}





// static WEBC_UINT8 *get_SendTerminalDisplay(DISPLAY_INT x, DISPLAY_INT y)
// {
//
// 	if (TermData && remote_teminal_role == TERM_DUMB_DISPLAY)
// 	{
// 		WEBC_UINT8 *ptr;
// 		ptr = (WEBC_UINT8*) TermData + (y * TermBytesPerRow) + (x * TermBytesPerPix);
// 		return(ptr);
// 	}
// 	return(0);
// }






static int headless_terminal_make_connection(void)
{
#if (USE_NON_BLOCKING)
	if (rtp_net_setblocking(tcpsrvsockhandle, 0) < 0)
	{
#if (TERM_VERBOSE > 0)
		rtp_printf("set non block failed\n");
#endif
		return -1;
	}
#endif

	if (rtp_net_connect (tcpsrvsockhandle, targIpaddress, webcport, 4) < 0)
	{
		rtp_printf("TCP connect failed return annyway\n");
		// return -1;
	}
#if (TERM_VERBOSE > 1)
	rtp_printf("TCP connection established\n");
#endif
	ComsockHandle = tcpsrvsockhandle;
	return 0;
}


#else
void call_TerminalTimout(HBROWSER_HANDLE which_Browser);
// stubs for the remote not being included
int network_display_enabled = 0;
void call_TerminalTimout(HBROWSER_HANDLE which_Browser)
{

	if (!network_display_enabled)
		return;

}
#endif


bool IsPartialDraw()
{
#if SUPPORT_PARTIAL_NETDRAW
return true;
#else
return false;
#endif
}
