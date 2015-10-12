 /*
 | RTPUTIL.C - Runtime Platform Utility Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: sarah $
 |  $Date: 2005/06/14 01:33:31 $
 |  $Name:  $
 |  $Revision: 1.2 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  These utilities are dependent on the endian of the hardware.
*/

/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"
#include "rtputil.h"

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
                            rtp_net_htons
 *----------------------------------------------------------------------*/
short rtp_net_htons (short i)
{
#if (RTP_LITTLE_ENDIAN)
	i = ((i>>8) | (i<<8));
#endif
	return(i);
}


/*----------------------------------------------------------------------*
                            rtp_net_ntohs
 *----------------------------------------------------------------------*/
short rtp_net_ntohs (short i)
{
#if (RTP_LITTLE_ENDIAN)
	i = ((i>>8) | (i<<8));
#endif
	return(i);
}


/*----------------------------------------------------------------------*
                            rtp_net_htonl
 *----------------------------------------------------------------------*/
long rtp_net_htonl (long i)
{
#if (RTP_LITTLE_ENDIAN)
		/* ----------------------------------- */
		/*  return((byte 3 to 0) OR            */
		/*         (byte 0 to 3) OR            */
		/*         (byte 2 to 1) OR            */
		/*         (byte 1 to 2)               */
		/* ----------------------------------- */
		i = (i >> 24) | (i << 24) | ((i & 0x00ff0000ul) >> 8) | 
		                            ((i & 0x0000ff00ul) << 8);
#endif
	return(i);
}


/*----------------------------------------------------------------------*
                            rtp_net_ntohl
 *----------------------------------------------------------------------*/
long rtp_net_ntohl (long i)
{
#if (RTP_LITTLE_ENDIAN)
		/* ----------------------------------- */
		/*  return((byte 3 to 0) OR            */
		/*         (byte 0 to 3) OR            */
		/*         (byte 2 to 1) OR            */
		/*         (byte 1 to 2)               */
		/* ----------------------------------- */
		i = (i >> 24) | (i << 24) | ((i & 0x00ff0000ul) >> 8) | 
		                            ((i & 0x0000ff00ul) << 8);
#endif
	return(i);
}


/*----------------------------------------------------------------------*
                            rtp_net_wtol
 *----------------------------------------------------------------------*/
unsigned long rtp_net_wtol (unsigned short* w)
{
unsigned long i;

#ifdef RTP_LONGWORD_ALLIGNED
	i = ((unsigned char*)w)[0]
	  | (((unsigned char*)w)[1]<<8)
	  | (((unsigned char*)w)[2]<<16)
	  | (((unsigned char*)w)[3]<<24);
#else
unsigned long* pi;
int n = 4;
	
	pi = &i;
	while (n--)
	{
	    *((unsigned char *)pi)++ = *((unsigned char *)w)++;
	}
#endif

	return(i);
}


/*----------------------------------------------------------------------*
                            rtp_net_ltow
 *----------------------------------------------------------------------*/
void rtp_net_ltow (unsigned short* w, unsigned long i)
{
	unsigned char *pi, *_w;
	int n = 4;
	
	pi = &i;
	_w = (unsigned char *)w;
	while (n--)
	{
	    *((unsigned char *)_w++) = *((unsigned char *)pi++);
   	}
}

// (INCLUDE_TCP_TIMESTAMP || INCLUDE_SNMP || INCLUDE_NFS_CLI || INCLUDE_NFS_SRV || INCLUDE_RIP) 
/* ********************************************************************   */
/* DWORD ALIGNMENT ROUTINES                                               */
/* ********************************************************************   */
union lbu 
{
    RTP_UINT8  as_bytes[4];
    RTP_UINT32 as_long;
};

/* return RTP_UINT32 which is stored in memory at pd   */
/* byte array to long                                  */
RTP_UINT32 byte_to_long(RTP_PFUINT32 pd)
{
union lbu l;
RTP_PFUINT8 p;
    p = (RTP_PFUINT8) pd;
    l.as_bytes[0] = *(p+0);
    l.as_bytes[1] = *(p+1);
    l.as_bytes[2] = *(p+2);
    l.as_bytes[3] = *(p+3);
    return(l.as_long);
}

/* store d at pd in memory     */
/* long to byte array          */
void long_to_bytes(RTP_PFUINT32 pd, RTP_UINT32 d)
{
union lbu l;
RTP_PFUINT8 p;
    p = (RTP_PFUINT8) pd;
    l.as_long = d;
    *(p+0) = l.as_bytes[0];
    *(p+1) = l.as_bytes[1];
    *(p+2) = l.as_bytes[2];
    *(p+3) = l.as_bytes[3];
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
