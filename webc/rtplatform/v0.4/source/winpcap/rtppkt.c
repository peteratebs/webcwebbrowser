 /*
 | RTPPKT.C - Runtime Platform Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: tony $
 |  $Date: 2005/03/17 19:49:48 $
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
#include "rtppkt.h"
#include "rtpthrd.h"
#include "rtpsignl.h"
#include "pcap.h"

#include <packet32.h>
#include <windows.h>
#include <windowsx.h>
#include <ntddndis.h>
#include <malloc.h>
#include <string.h>

/************************************************************************
* Defines
************************************************************************/

#define ETHERNET_HEADER_LEN    14
#define ETHERNET_ADDR_LEN      6
#define ETHERNET_PROTOCOL_LEN  2

/************************************************************************
* Types
************************************************************************/


typedef struct s_RTP_PKT_DEVICE_INFO RTP_PKT_DEVICE_INFO;

struct s_RTP_PKT_DEVICE_INFO
{
	int              running;
	unsigned         promiscuous;
	pcap_t*          packetCapture;
	unsigned char    macAddress[ETHERNET_ADDR_LEN];
	RTP_PKT_CALLBACK recvCallback;
	void*            recvData;
	RTP_PKT_CALLBACK filterCallback;
	void*            filterData;
};

/************************************************************************
* Data
************************************************************************/

unsigned      rtpPacketInitialized = 0;
RTP_PKT_HEAD  recvQueue;
RTP_MUTEX     recvQueueLock;
RTP_SEMAPHORE recvQueueSignal;

/************************************************************************
* Macros
************************************************************************/

#define RTP_PKT_ENTER_CRITICAL  rtp_sig_mutex_claim(recvQueueLock)

#define RTP_PKT_EXIT_CRITICAL   rtp_sig_mutex_release(recvQueueLock)

#define LIST_INIT(X)            { (X)->next = (X); (X)->prev = (X); }

/* insert Y after X */
#define LIST_INSERT_AFTER(X,Y)  { (X)->next->prev = (Y); \
                                  (Y)->next = (X)->next; \
                                  (Y)->prev = (X); \
                                  (X)->next = (Y); \
                                }

/* insert Y before X */                                  
#define LIST_INSERT_BEFORE(X,Y) { (X)->prev->next = (Y); \
                                  (Y)->prev = (X)->prev; \
                                  (Y)->next = (X); \
                                  (X)->prev = (Y); \
                                }
                                  
#define LIST_REMOVE(X)          { (X)->prev->next = (X)->next; \
                                  (X)->next->prev = (X)->prev; \
                                }

/* replace X with R */                                  
#define LIST_REPLACE(X,R)       { (R)->next = (X)->next; \
                                  (R)->prev = (X)->prev; \
                                  (X)->next->prev = (R); \
                                  (X)->prev->next = (R); \
                                }

/************************************************************************
* Function Prototypes
************************************************************************/
void _rtp_winpcap_thread (void* pcapStream);

RTP_PKT_HEAD* _rtp_pkt_alloc
		(RTP_PKT_DEVICE device,
		 long minPayloadSize);

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                              rtp_pkt_init
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
int rtp_pkt_init (void)
{
	if (rtpPacketInitialized == 0)
	{
		if (rtp_sig_mutex_alloc(&recvQueueLock, 0) >= 0)
		{
			if (rtp_sig_semaphore_alloc(&recvQueueSignal, 0) >= 0)
			{
				rtpPacketInitialized++;
				LIST_INIT(&recvQueue);
				return (0);
			}
		
			rtp_sig_mutex_free(recvQueueLock);
		}
		return (-1);
	}
	
	rtpPacketInitialized++;
	
	return (0);
}

/*----------------------------------------------------------------------*
                          rtp_pkt_open_device
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
int rtp_pkt_open_device 
		(RTP_PKT_DEVICE* handle,
		 const char* type,
		 int number,
		 unsigned promiscuous)
{
pcap_if_t* alldevs;
char errbuf[PCAP_ERRBUF_SIZE+1];

	/* Retrieve the interfaces list */
	if (pcap_findalldevs(&alldevs, errbuf) >= 0)
	{
		if (alldevs)
		{
			RTP_PKT_DEVICE_INFO* devInfo = (RTP_PKT_DEVICE_INFO*) malloc(sizeof(RTP_PKT_DEVICE_INFO));
			if (devInfo)
			{
				LPADAPTER adapter = PacketOpenAdapter(alldevs->name);
				
				if (adapter)
				{
					unsigned char buf[100];
					PPACKET_OID_DATA oidData = (PPACKET_OID_DATA) buf;
					
					oidData->Oid = OID_802_3_PERMANENT_ADDRESS;
					oidData->Length = 40;

					if (PacketRequest (adapter, 0 /* set, boolean */, oidData))
					{
						memcpy(devInfo->macAddress, oidData->Data, ETHERNET_ADDR_LEN);
					}

					PacketCloseAdapter(adapter);
				}
				
				devInfo->packetCapture = pcap_open_live (
						alldevs->name,  /* adapter name                 */
						1514,           /* packet data size             */
						0,              /* flags                        */
						1,              /* read timeout in milliseconds */
						errbuf          /* error message buffer         */
						);
						
				if (devInfo->packetCapture)
				{
					RTP_THREAD threadHandle;

					devInfo->promiscuous = promiscuous;
					devInfo->running = 1;
									
					if (rtp_thread_spawn(
								&threadHandle, 
								_rtp_winpcap_thread, 
								0,      /* name       */
								0,      /* stack size */
								0,      /* priority   */
								devInfo /* user data  */
							) >= 0)
					{
						*handle = (RTP_PKT_DEVICE) devInfo;
						return (0);
					}
				}

				free(devInfo);
			}
		}	
	}
	
	return (-1);
}

/*----------------------------------------------------------------------*
                          rtp_pkt_close_device
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
void rtp_pkt_close_device
		(RTP_PKT_DEVICE handle)
{
	RTP_PKT_DEVICE_INFO* devInfo = (RTP_PKT_DEVICE_INFO*) handle;

	RTP_PKT_ENTER_CRITICAL;
	devInfo->running = 0;
	RTP_PKT_EXIT_CRITICAL;	
}

/*----------------------------------------------------------------------*
                              rtp_pkt_send
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
   		This function should return as quickly as possible.  If delayFree
   		is false, then the caller of this function should assume that
   		the packet is free once the function returns (note that this may
   		not actually be the case).  If it is true, then the packet is
   		assumed to persist (i.e. it is not freed) until rtp_pkt_free_if_sent
   		when called on the given packet returns a value of 1, indicating
   		that the packet and all its part headers have been freed.  (Note:
   		any data pointed to by a part header in the packet is not freed
   		automatically by any rtp_pkt_ call; such data is "owned" by the
   		user of the RTPPKT module.  rtp_pkt_free_if_sent returning 1
   		just lets the caller know that the data pointed to by such 
   		part headers is no longer needed by the packet transmit layer,
   		and therefore MAY be freed, if the upper layer so desires).

    @return 
 */
long rtp_pkt_send
		(RTP_PKT_HEAD* packet)
{
	RTP_PKT_DEVICE_INFO* devInfo = (RTP_PKT_DEVICE_INFO*) packet->device;
	long result = -1;

	RTP_PKT_ENTER_CRITICAL;
	
	if (packet->status == RTP_PKT_STATUS_ABORTED)
	{
		RTP_PKT_EXIT_CRITICAL;
		return (-1);
	}
	
	packet->status = RTP_PKT_STATUS_SENDING;

	RTP_PKT_EXIT_CRITICAL;
		
	if (rtp_pkt_get_local_address(packet->device, packet->srcAddr) >= 0)
	{
		if (!packet->nextPart)
		{
			RTP_PKT_ENTER_CRITICAL;
			
			if (pcap_sendpacket(
						devInfo->packetCapture, 
						packet->buffer, 
						packet->payloadSize + ETHERNET_HEADER_LEN) 
					>= 0)
			{
				result = 0;
			}
			
			RTP_PKT_EXIT_CRITICAL;
		}
		else
		{
			long totalLength = packet->payloadSize + ETHERNET_HEADER_LEN;
			RTP_PKT_PART* part;
			unsigned char* wholePacket;

			part = packet->nextPart;
			while (part)
			{
				totalLength += part->size;
				part = part->next;
			}

			wholePacket = (unsigned char*) malloc(totalLength);
			if (wholePacket)
			{
				unsigned char* ptr = wholePacket;

				rtp_memcpy(ptr, packet->buffer, packet->payloadSize + ETHERNET_HEADER_LEN);
				ptr += packet->payloadSize + ETHERNET_HEADER_LEN;

				part = packet->nextPart;
				while (part)
				{
					memcpy(ptr, part->data, part->size);
					ptr += part->size;
					part = part->next;
				}

				RTP_PKT_ENTER_CRITICAL;

				if (pcap_sendpacket(
							devInfo->packetCapture, 
							wholePacket, 
							totalLength) 
						>= 0)
				{
					result = 0;
				}
	
				RTP_PKT_EXIT_CRITICAL;

				free(wholePacket);
			}
			else
			{
				result = -1;
			}
		}
	}
	
	RTP_PKT_ENTER_CRITICAL;

	if (result == 0)
	{
		packet->status = RTP_PKT_STATUS_SENT;
	}
	else
	{
		packet->status = RTP_PKT_STATUS_SEND_ERROR;
	}

	RTP_PKT_EXIT_CRITICAL;
	
	if (packet->sentCallbackFn)
	{
		packet->sentCallbackFn(packet->sentCallbackData, packet);
	}

	return (result);
}
 
/*----------------------------------------------------------------------*
                           rtp_pkt_abort_send
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */ 
int rtp_pkt_abort_send
		(RTP_PKT_HEAD* packet)
{
	unsigned sending = 0;
	
	RTP_PKT_ENTER_CRITICAL;
	
	if (packet->status == RTP_PKT_STATUS_SENDING || 
	    packet->status == RTP_PKT_STATUS_QUEUED_FOR_SEND ||
	    packet->status == RTP_PKT_STATUS_SENT ||
	    packet->status == RTP_PKT_STATUS_SEND_ERROR)
	{
		RTP_PKT_EXIT_CRITICAL;
		return (-1);
	}	
		
	packet->status = RTP_PKT_STATUS_ABORTED;

	if (packet->sentCallbackFn)
	{
		RTP_PKT_EXIT_CRITICAL;
		packet->sentCallbackFn(packet->sentCallbackData, packet);
	}
	else
	{
		RTP_PKT_EXIT_CRITICAL;
	}

	return (0);
}

/*----------------------------------------------------------------------*
                              rtp_pkt_recv
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
int rtp_pkt_recv
		(RTP_PKT_HEAD** packet,
		 long msecTimeout)
{	
	*packet = 0;

	RTP_PKT_ENTER_CRITICAL;

	if (recvQueue.next == &recvQueue)
	{
		RTP_PKT_EXIT_CRITICAL;
		rtp_sig_semaphore_wait_timed(recvQueueSignal, msecTimeout);
		RTP_PKT_ENTER_CRITICAL;	
	}

	if (recvQueue.next != &recvQueue)
	{
		*packet = recvQueue.next;
		LIST_REMOVE(*packet);
		LIST_INIT(*packet);
	}

	RTP_PKT_EXIT_CRITICAL;

	if (*packet)
	{
		return (0);
	}

	return (-1);
}

/*----------------------------------------------------------------------*
                           rtp_pkt_get_status
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
int rtp_pkt_get_status (RTP_PKT_HEAD* packet)
{
	return (packet->status);
}
                                            
/*----------------------------------------------------------------------*
                      rtp_pkt_calculate_ip_checksum
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
unsigned short rtp_pkt_calculate_ip_checksum 
		(const unsigned char* buffer,
		 unsigned short length,
		 unsigned long partialChecksum)
{
	unsigned short* s = (unsigned short*) buffer;
	unsigned long checksum = partialChecksum;
	
	while (length > 1)
	{
		checksum += *(s++);
		length -= 2;
	}
	
	if (length)
	{
		checksum += *((unsigned char*) s);
	}

	while (checksum & 0xffff0000)
	{
		checksum = (checksum & 0xffff) + (checksum >> 16);
	}
	
	return ((unsigned short) checksum);
}                                           
                                             
/*----------------------------------------------------------------------*
                         rtp_pkt_get_address_length
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
unsigned char rtp_pkt_get_address_length
		(RTP_PKT_DEVICE handle)
{
	return (ETHERNET_ADDR_LEN);
}

/*----------------------------------------------------------------------*
                        rtp_pkt_get_local_address
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
int rtp_pkt_get_local_address
		(RTP_PKT_DEVICE handle,
		 unsigned char* buffer)
{
	RTP_PKT_DEVICE_INFO* devInfo = (RTP_PKT_DEVICE_INFO*) handle;

	memcpy(buffer, devInfo->macAddress, ETHERNET_ADDR_LEN);
	
	return (0);
}
       
/*----------------------------------------------------------------------*
                        rtp_pkt_get_protocol_length
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
unsigned char rtp_pkt_get_protocol_length
		(RTP_PKT_DEVICE handle)
{
	return (ETHERNET_PROTOCOL_LEN);
}
       
/*----------------------------------------------------------------------*
                        rtp_pkt_get_protocol_by_name
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
int rtp_pkt_get_protocol_by_name
		(RTP_PKT_DEVICE handle,
		 unsigned char* outBuffer,
		 const char* protocolName)
{
	switch (protocolName[0])
	{
		case 'i':
			switch (protocolName[1])
			{
				case 'p':
					switch (protocolName[2])
					{
						case 0:
							/* "ip" */
							outBuffer[0] = 0x08;
							outBuffer[1] = 0x00;
							return (0);
					}
			}
			break;
		
		case 'a':
			switch (protocolName[1])
			{
				case 'r':
					switch (protocolName[2])
					{
						case 'p':
							switch (protocolName[3])
							{
								case 0:
									/* "arp" */
									outBuffer[0] = 0x08;
									outBuffer[1] = 0x06;
									return (0);
							}
					}
					break;
			}
			break;
	}
	
	return (-1);
}

/*----------------------------------------------------------------------*
                    rtp_pkt_compare_protocol_by_name
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
unsigned rtp_pkt_compare_protocol_by_name 
		(RTP_PKT_DEVICE handle,
		 unsigned char* inBuffer,
		 const char* protocolName)
{
	switch (protocolName[0])
	{
		case 'i':
			switch (protocolName[1])
			{
				case 'p':
					switch (protocolName[2])
					{
						case 0:
							/* "ip" */
							return (inBuffer[0] == 0x08 && inBuffer[1] == 0x00);
					}
			}
			break;
		
		case 'a':
			switch (protocolName[1])
			{
				case 'r':
					switch (protocolName[2])
					{
						case 'p':
							switch (protocolName[3])
							{
								case 0:
									/* "arp" */
									return (inBuffer[0] == 0x08 && inBuffer[1] == 0x06);
							}
					}
					break;
			}
			break;
	}

	return (0);
}
                                             
/*----------------------------------------------------------------------*
                       rtp_pkt_get_broadcast_address
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
int rtp_pkt_get_broadcast_address 
		(RTP_PKT_DEVICE handle,
		 unsigned char* buffer)
{
	buffer[0] = 0xFF;
	buffer[1] = 0xFF;
	buffer[2] = 0xFF;
	buffer[3] = 0xFF;
	buffer[4] = 0xFF;
	buffer[5] = 0xFF;
	
	return (0);
}


/*----------------------------------------------------------------------*
                          rtp_pkt_address_compare
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
long rtp_pkt_get_device_mtu
		(RTP_PKT_DEVICE handle)
{
	return (1500);
}

/*----------------------------------------------------------------------*
                          rtp_pkt_address_compare
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
unsigned rtp_pkt_address_compare
		(RTP_PKT_DEVICE handle,
		 unsigned char* addrOne,
		 unsigned char* addrTwo)
{	
	return (*((unsigned long*) addrOne)  == *((unsigned long*) addrTwo) && 
	        *((unsigned short*) (addrOne + 4)) == *((unsigned short*) (addrTwo + 4)));
}
                                             
/*----------------------------------------------------------------------*
                              rtp_pkt_alloc
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
int rtp_pkt_alloc_async
		(RTP_PKT_DEVICE device,
		 long minPayloadSize,
		 RTP_PKT_CALLBACK allocCallback,
		 void* allocData,
		 RTP_PKT_ALLOC_REQUEST* allocRequest,
		 int priority)
{
	RTP_PKT_HEAD* newPacket;
	
	newPacket = _rtp_pkt_alloc(device, minPayloadSize);
	if (newPacket)
	{
		if (allocRequest)
		{
			*allocRequest = 0;
		}
		
		allocCallback(allocData, newPacket);
		return (0);
	}
	
	return (-1);
}

/*----------------------------------------------------------------------*
                             rtp_pkt_alloc
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
RTP_PKT_HEAD*  rtp_pkt_alloc 
		(RTP_PKT_DEVICE device,
		 long minPayloadSize)
{
	return (_rtp_pkt_alloc(device, minPayloadSize));
}

/*----------------------------------------------------------------------*
                             _rtp_pkt_alloc
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
int rtp_pkt_abort_alloc
		(RTP_PKT_ALLOC_REQUEST* allocKey)
{
	return (0);
}

/*----------------------------------------------------------------------*
                             _rtp_pkt_alloc
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
RTP_PKT_HEAD* _rtp_pkt_alloc
		(RTP_PKT_DEVICE device,
		 long minPayloadSize)
{
	RTP_PKT_HEAD* newPacket;
	
	newPacket = (RTP_PKT_HEAD*) malloc(sizeof(RTP_PKT_HEAD) + minPayloadSize + ETHERNET_HEADER_LEN);
	if (newPacket)
	{
		newPacket->status = RTP_PKT_STATUS_ALLOC;
		newPacket->device = device;
		newPacket->buffer = ((unsigned char*) newPacket) + sizeof(RTP_PKT_HEAD);
		newPacket->bufferSize = minPayloadSize + ETHERNET_HEADER_LEN;
		newPacket->payloadData = newPacket->buffer + ETHERNET_HEADER_LEN;
		newPacket->payloadSize = minPayloadSize;
		newPacket->nextPart = 0;
		newPacket->dstAddr  = newPacket->buffer;
		newPacket->srcAddr  = newPacket->buffer + 6;
		newPacket->protocol = newPacket->buffer + 12;
		newPacket->sentCallbackFn = 0;
	}
	
	return (newPacket);
}
/*----------------------------------------------------------------------*
                            rtp_pkt_alloc_part
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
RTP_PKT_PART* rtp_pkt_alloc_part
		(const unsigned char* data,
		 long size)
{
	RTP_PKT_PART* newPart;
	
	newPart = (RTP_PKT_PART*) malloc(sizeof(RTP_PKT_PART));
	if (newPart)
	{
		newPart->data = data;
		newPart->size = size;
		newPart->next = 0;
	}

	return (newPart);
}
                                             
/*----------------------------------------------------------------------*
                              rtp_pkt_free
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
void rtp_pkt_free 
		(RTP_PKT_HEAD* packet)
{
	RTP_PKT_PART* part = packet->nextPart;
	RTP_PKT_PART* next;

	while (part)
	{
		next = part->next;
		rtp_pkt_free_part(part);
		part = next;
	}
	
	free(packet);
}

/*----------------------------------------------------------------------*
                            rtp_pkt_free_part
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
void rtp_pkt_free_part 
		(RTP_PKT_PART* part)
{
	free(part);
}

/*----------------------------------------------------------------------*
                      rtp_pkt_set_recv_callback
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
int rtp_pkt_set_recv_callback (
		RTP_PKT_DEVICE   handle,
		RTP_PKT_CALLBACK recvCallback,
		void*            recvData
	)
{
	RTP_PKT_DEVICE_INFO* devInfo = (RTP_PKT_DEVICE_INFO*) handle;
	
	RTP_PKT_ENTER_CRITICAL;
	
	devInfo->recvCallback = recvCallback;
	devInfo->recvData = recvData;
	
	RTP_PKT_EXIT_CRITICAL;
	
	return (0);
}

/*----------------------------------------------------------------------*
                           _rtp_winpcap_thread
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
void _rtp_winpcap_thread (void* data)
{
RTP_PKT_DEVICE_INFO* devInfo = (RTP_PKT_DEVICE_INFO*) data;
struct pcap_pkthdr* pktHeader;
struct pcap_pkthdr pktHeaderCopy;
u_char* pktData;
u_char* pktDataCopy = (u_char*) malloc(81920);
unsigned char myMacAddr[6];
unsigned char mcastAddr[6];

	if (pktDataCopy)
	{
		rtp_pkt_get_local_address((RTP_PKT_DEVICE) devInfo, myMacAddr);
		rtp_pkt_get_broadcast_address((RTP_PKT_DEVICE) devInfo, mcastAddr);
	
		RTP_PKT_ENTER_CRITICAL;
	
		while (devInfo->running)
		{
			if (pcap_next_ex (devInfo->packetCapture, &pktHeader, &pktData) == 1)
			{
				RTP_PKT_HEAD* newPacket;
				
				memcpy(pktDataCopy, pktData, pktHeader->len);
				memcpy(&pktHeaderCopy, pktHeader, sizeof(struct pcap_pkthdr));
	
				if ((devInfo->promiscuous || 
				     rtp_pkt_address_compare((RTP_PKT_DEVICE) devInfo, pktDataCopy, myMacAddr) ||
					 rtp_pkt_address_compare((RTP_PKT_DEVICE) devInfo, pktDataCopy, mcastAddr)) &&
					(devInfo->recvCallback != 0))
				{			
					newPacket = _rtp_pkt_alloc((RTP_PKT_DEVICE) devInfo, pktHeaderCopy.len);
					if (newPacket)
					{			
						RTP_PKT_CALLBACK handlePacket;
						void* ctx;
						
						newPacket->status = RTP_PKT_STATUS_RECV;
		
						memcpy(newPacket->buffer, pktDataCopy, pktHeaderCopy.len);
			
						handlePacket = devInfo->recvCallback;
						ctx = devInfo->recvData;
			
						RTP_PKT_EXIT_CRITICAL;
						handlePacket(ctx, newPacket);
						RTP_PKT_ENTER_CRITICAL;
					}
				}
			}
			
			RTP_PKT_EXIT_CRITICAL;	
			RTP_PKT_ENTER_CRITICAL;
		}
	
		RTP_PKT_EXIT_CRITICAL;
		
		free(pktDataCopy);
	}
	
	/* shut down the capture */
	pcap_close(devInfo->packetCapture);
	free(devInfo);
}

/*----------------------------------------------------------------------*
                           _rtp_winpcap_thread
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
int rtp_pkt_free_callback 
		(void* callbackData, 
		 RTP_PKT_HEAD* packet)
{
	rtp_pkt_free(packet);
	return (0);
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
