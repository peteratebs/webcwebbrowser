#include "webc.h"
// #include "FB_HWInterface.h"
#include "webcmem.h"
//#include "assert.h"
//#include "FBConfig.h"
#include "rtpprint.h"
#include "Remoteterminal.h"


#if (INCLUDE_REMOTE_TERMINAL)
#include "rtpnet.h"
#include "rtpprint.h"

static int remote_teminal_role; /* 0 = disabled, 1 == display, 2 == headless device */
static WEBC_UINT8 *TermData;
static WEBC_UINT8 *TermInputData;
static int TermBytesPerRow;
static int TermBytesPerPix;
static int TermHeight;
static int term_initialized = 0;
static unsigned long display_bytes_buffered = 0;
#define TERM_DISABLED			0
#define TERM_DUMB_DISPLAY    	1
#define TERM_HEADLESS_DEVICE    2

static int headless_terminal_make_connection(void);

WEBC_UINT8 *targIpaddress;
WEBC_UINT8 *myIpaddress;

#define USE_LOOP 0
#if (USE_LOOP)

WEBC_UINT8 termIpaddress[4] = {127,0,0,1};
WEBC_UINT8 headlessIpaddress[4] = {127,0,0,1};
#else
#define PETERTERM 0
#if (PETERTERM)

WEBC_UINT8 termIpaddress[4] = {192,168,0,3};
WEBC_UINT8 headlessIpaddress[4] = {192,168,0,5};
#else
WEBC_UINT8 termIpaddress[4] = {192,168,0,5};
WEBC_UINT8 headlessIpaddress[4] = {192,168,0,3};
#endif

#endif


static long ComsockHandle, tcpsrvsockhandle;

static void call_TerminalInit(int whoami);
static void rterm_send(unsigned long sockHandle, WEBC_UINT8 *b, unsigned long length);
static unsigned long rterm_recv(unsigned long sockHandle, WEBC_UINT8 *b, unsigned long maxlength, long blocktime);
#ifdef WIN32
static void ReceiveDisplayData(unsigned long sockHandle);
static int terminal_accept_connection(void);
#endif

static void ReceiveMessageData(unsigned long sockHandle, HBROWSER_HANDLE hbrowser);
// static WEBC_UINT8 *get_SendTerminalDisplay(DISPLAY_INT x, DISPLAY_INT y);
static int headless_terminal_make_connection(void);

#ifdef WIN32

int eventcount = 0;

void send_TerminalEvent(HBROWSER_HANDLE hbrowser,	HTMLEvent* event)
{
	if (remote_teminal_role == TERM_DUMB_DISPLAY)
	{
		if (eventcount++ == 0)
		{
			printf("killing event 0 \n");
			return;
		}
		if (event->type != HTML_EVENT_MOUSEMOVE)
		{

			rterm_send(ComsockHandle, (WEBC_UINT8 *) event, (unsigned long) sizeof(*event));

		}
	}
//	else if (remote_teminal_role == TERM_HEADLESS_DEVICE)
//		;
}

static void start_RemoteTerminal(void)
{
		call_TerminalInit(TERM_DUMB_DISPLAY);
		if (terminal_accept_connection() == 0)
			remote_teminal_role = TERM_DUMB_DISPLAY;
}
void start_HeadlessTerminal(void)
{
	call_TerminalInit(TERM_HEADLESS_DEVICE);
	if (headless_terminal_make_connection() == 0)
	{
		remote_teminal_role = TERM_HEADLESS_DEVICE;
	}

}

void call_TerminalCheckKey(WebKey keyPressed)
{
	if(keyPressed == WGK_F7)
	{
	 	if (remote_teminal_role == TERM_DISABLED)
			start_RemoteTerminal();
		else
			remote_teminal_role = TERM_DISABLED;
	}
	else if(keyPressed == WGK_F8)
	{
	 	if (remote_teminal_role == TERM_DISABLED)
		{
			start_HeadlessTerminal();
		}
		else
			remote_teminal_role = TERM_DISABLED;
	}
}

#endif


void set_TerminalCanvas(DISPLAY_INT height, int BytesPerRow, int BytesPerPix)
{
	TermData =  (WEBC_UINT8*)WEBC_MALLOC(BytesPerRow * height);

	TermInputData = TermData;
	TermHeight = height;
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
	return 0;
}

void call_TerminalTimout(HBROWSER_HANDLE which_Browser)
{
#ifdef WIN32
	if (TermData && remote_teminal_role == TERM_DUMB_DISPLAY)
	{
		ReceiveDisplayData(ComsockHandle);
	}
	else
#endif
	if (TermData && remote_teminal_role == TERM_HEADLESS_DEVICE)
		ReceiveMessageData(ComsockHandle,which_Browser);
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
			rtp_printf("TCP socket failed\n");
		}
		term_initialized = 1;
	}
}

static void rterm_send(unsigned long sockHandle, WEBC_UINT8 *b, unsigned long length)
{
unsigned long nsent = length;

	while (length)
	{
	unsigned long ntosend;
	int err;
	ntosend = length > MAXTCPSENDSIZE ? MAXTCPSENDSIZE : length;
//	rtp_printf("Sending %d bytes\n", ntosend);
		err = rtp_net_send (sockHandle, b, ntosend);
//	rtp_printf("Returned %dd\n", err);

		if(err < 0)//send to other sid
		{
			rtp_printf("\n error: send returned %d\n", err);
			return;
		}

		b += ntosend;
		length -= ntosend;
	}
}


#define SUPPORT_PARTIAL_NETDRAW 1
#if (SUPPORT_PARTIAL_NETDRAW)


static unsigned long rterm_recv_partial(unsigned long sockHandle, WEBC_UINT8 *b, unsigned long maxlength, long blocktime)
{
unsigned long myrect[4];
unsigned long totalrecieved = 0;
int doitall, rowscompleted;
unsigned long partial_width,partial_height, bytes_this_row;
int err,i;


	for(;;)
	{
		err = rtp_net_read_select (sockHandle, blocktime);
		if (err == 0)
		{
			if (rtp_net_recv (sockHandle, (WEBC_UINT8 *)myrect, 16) != 16)
			{
				rtp_printf("error: recieving coordinates\n");
				return 0;
			}
			else
				break;
		}
	}
	partial_width = myrect[3] - myrect[1] + 1; // right  - left
	partial_height = myrect[2] - myrect[0] + 1;// bottom - top
	maxlength = partial_width * partial_height;
	if (partial_width == 320 && partial_height == 240)
		doitall = 1;
	else
		doitall = 0;
	rowscompleted = 0;
	bytes_this_row = 0;

	b = get_TerminalDisplayLine(myrect[1], myrect[0]); // x, y
//	rtp_printf("Recving top, left, bottom right %d, %d, %d, %d\n", myrect[0], myrect[1], myrect[2], myrect[3]);

	while (maxlength)
	{
	unsigned long ntorecv,nrecieved;

		nrecieved = 0;
		err = rtp_net_read_select (sockHandle, blocktime);
		if (err == 0)
		{
			if (doitall)
				ntorecv = maxlength > MAXTCPRECVSIZE ? MAXTCPRECVSIZE : maxlength;
			else
			{
				ntorecv = partial_width - bytes_this_row;
			}

			i = rtp_net_recv (sockHandle, b, ntorecv);
			if (i > 0)
				nrecieved = (unsigned long) i;
			if(i < 0)
			{
				rtp_printf("\n error: breaking recv returned %d\n", i);
				break;
			}
		}
		if (nrecieved)
		{
			if (!doitall)
			{
				bytes_this_row += nrecieved;
				if (bytes_this_row >= partial_width)
				{
					rowscompleted += 1;
					bytes_this_row = 0;
					b = get_TerminalDisplayLine(myrect[1], myrect[0]+rowscompleted); // x, y
				}
				else
				{
					b = get_TerminalDisplayLine(myrect[1]+bytes_this_row, myrect[0]+rowscompleted); // x, y
				}
			}
			else
				b += nrecieved;
			maxlength -= nrecieved;
			totalrecieved += nrecieved;
		}
		else
		{
			rtp_printf("\n error: recv retrned 0 breaking \n");
			break;
	    }
	}
//	rtp_printf("Finished recving (%d) bytes\n", totalrecieved);
	return (totalrecieved);
}
#endif

static unsigned long rterm_recv(unsigned long sockHandle, WEBC_UINT8 *b, unsigned long maxlength, long blocktime)
{
unsigned long totalrecieved = 0;

	while (maxlength)
	{
	unsigned long ntorecv,nrecieved;
	int err;
	ntorecv = maxlength > MAXTCPRECVSIZE ? MAXTCPRECVSIZE : maxlength;
		nrecieved = 0;
		err = rtp_net_read_select (sockHandle, blocktime);
		if (err == 0)
		{
		int i;
			i = rtp_net_recv (sockHandle, b, ntorecv);

			if(i < 0)
			{
				rtp_printf("\n error: breaking recv retrned %d\n", i);
				break;
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
			rtp_printf("\n error: recv retrned 0 breaking \n");
			break;
	    }
	}
	return (totalrecieved);
}


static void ReceiveMessageData(unsigned long sockHandle, HBROWSER_HANDLE hbrowser)
{
	if (rtp_net_read_select (sockHandle, 1) == 0)
	{
	unsigned long totalrecieved;
		HTMLEvent* e = (HTMLEvent*)WEBC_MALLOC(sizeof(HTMLEvent));

		totalrecieved = rterm_recv(sockHandle, (WEBC_UINT8 *) e, sizeof(HTMLEvent), RTP_TIMEOUT_INFINITE);
		if (totalrecieved >= sizeof(HTMLEvent))
			webc_BrowserDispatchEvent(hbrowser, e);
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




#ifdef WIN32
#include "WindowsBitMapFrameBuffer.h"

extern WindowsBitMapFrameBuffer *pself;
static void ReceiveDisplayData(unsigned long sockHandle)
{
unsigned long nrecieved, maxlength,bytes_in_buffer;

	bytes_in_buffer = TermBytesPerRow * TermHeight;

	if (rtp_net_read_select (sockHandle, 1) == 0)
	{
#if (SUPPORT_PARTIAL_NETDRAW)
		maxlength =	bytes_in_buffer;
		nrecieved = rterm_recv_partial(ComsockHandle, TermData, maxlength,RTP_TIMEOUT_INFINITE);
		display_bytes_buffered = bytes_in_buffer;
#else
		maxlength =	bytes_in_buffer - display_bytes_buffered;
		nrecieved = rterm_recv(ComsockHandle, TermInputData, maxlength,RTP_TIMEOUT_INFINITE);
		TermInputData += nrecieved;
		display_bytes_buffered += nrecieved;
#endif
//		if (nrecieved)
//			printf("Recved %d bytes total == %d need == %d\n", nrecieved, display_bytes_buffered, bytes_in_buffer);
		if (display_bytes_buffered == bytes_in_buffer)
		{
			TermInputData = TermData;
			display_bytes_buffered = 0;
			if (pself)
			{
				rtp_memcpy(pself->GetBaseAddress(0),get_TerminalDisplayLine(0, 0),bytes_in_buffer);
				pself->DoDisplay();
			}
		}
	}
}

static int terminal_accept_connection(void)
{
unsigned char ripAddr[4];
int rport;
int rtype;

	rtp_printf("Waiting for a connection \n");
	if (rtp_net_bind_dual (tcpsrvsockhandle, 0, myIpaddress, webcport, webctype) < 0)
	{
		rtp_printf("TCP bind failed\n");
		return -1;
	}
	if(rtp_net_listen (tcpsrvsockhandle, 1) < 0)//open a tcp listener
	{
		rtp_printf("TCP listen failed\n");
		return -1;
	}

	if (rtp_net_accept (&ComsockHandle, tcpsrvsockhandle, ripAddr, &rport, &rtype) < 0)
	{
		rtp_printf("TCP accept failed\n");
		return -1;
	}
	rtp_printf("connected \n");
	return 0;
}
#endif


static int headless_terminal_make_connection(void)
{
	if (rtp_net_connect (tcpsrvsockhandle, targIpaddress, webcport, 4) < 0)
	{
		rtp_printf("TCP connect failed\n");
		return -1;
	}
	rtp_printf("TCP connection established\n");
	ComsockHandle = tcpsrvsockhandle;
	return 0;
}


#endif
