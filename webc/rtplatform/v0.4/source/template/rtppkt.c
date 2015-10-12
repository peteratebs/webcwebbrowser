 /*
 | RTPPKT.C - Runtime Platform Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:31 $
 |  $Name:  $
 |  $Revision: 1.2 $
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

/************************************************************************
* Defines
************************************************************************/

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
		 RTP_PKT_CALLBACK recvCallback,
		 RTP_PKT_CALLBACK filterCallback,
		 void* callbackData)
{
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
}

/*----------------------------------------------------------------------*
                              rtp_pkt_send
 *----------------------------------------------------------------------*/
/** @memo   

    @doc	This function should return as quickly as possible.  If delayFree
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
		(RTP_PKT_DEVICE handle,
		 RTP_PKT_HEAD* packet)
{
	return (-1);
}
                                             
/*----------------------------------------------------------------------*
                      rtp_pkt_calculate_ip_checksum
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
long rtp_pkt_calculate_ip_checksum 
		(const unsigned char* buffer,
		 long length)
{
	return (-1);
}
                                             
/*----------------------------------------------------------------------*
                       rtp_pkt_validate_ip_checksum
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
unsigned rtp_pkt_validate_ip_checksum
		(const unsigned char* buffer,
		 long length)
{
	return (0);
}
                                             
/*----------------------------------------------------------------------*
                         rtp_pkt_get_address_length
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
long rtp_pkt_get_address_length
		(RTP_PKT_DEVICE handle)
{
	return (-1);
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
	return (-1);
}
                                             
/*----------------------------------------------------------------------*
                       rtp_pkt_get_multicast_address
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
int rtp_pkt_get_multicast_address 
		(RTP_PKT_DEVICE handle,
		 unsigned char* buffer)
{
	return (-1);
}
                                             
/*----------------------------------------------------------------------*
                              rtp_pkt_alloc
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
RTP_PKT_HEAD* rtp_pkt_alloc
		(long minPayloadSize)
{
	return (0);
}

/*----------------------------------------------------------------------*
                            rtp_pkt_alloc_part
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
RTP_PKT_PART* rtp_pkt_alloc_part
		(unsigned char* data,
		 long size,
		 RTP_PKT_PART_FREE freeCallback)
{
	return (0);
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
}

/*----------------------------------------------------------------------*
                           rtp_pkt_free_on_sent
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    

    @return 
 */
int rtp_pkt_free_on_sent
		(void* callbackData, 
		 RTP_PKT_HEAD* packet)
{
	return (-1);
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
