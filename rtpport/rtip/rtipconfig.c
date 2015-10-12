
#include "xnconf.h"
#include "rtipapi.h"

#if (RTPLATFORM)
#include "rtp.h"
#endif

#include "rtipconfig.h"

#define USE_IP_OFFICE 0  /* use 192.168.0.x */

/* ********************************************************************              */
/* CONFIGURATION SECTION                                                             */
/* ********************************************************************              */
/* THE FOLLOWING SECTION CONTAINS ALL THE DEFAULT CONFIGURATION INFORMATION          */
/* AND SPECIFIES WHICH TEST TO RUN.                                                  */
/* YOU NEED TO REVIEW THIS INFORMATION FOR YOUR ENVIRONMENT.                         */
/* YOU MIGHT NEED TO CHANGE SOME OF THE FOLLOWING VALUES IN THIS SECTION.            */


#if (USE_ETHER)
#if (0)
    int  RTP_FAR device_id    = LOOP_DEVICE;  /* see rtipapi.h for possible values */
#elif (0)
    int  RTP_FAR device_id    = R8139_DEVICE;  /* see rtipapi.h for possible values */

#elif (WINMSP3 || POLMSP3)
    int  RTP_FAR device_id    = WINPCAP_DEVICE;  /* see rtipapi.h for possible values */
#else
    int  RTP_FAR device_id    = NE2000_DEVICE;  /* see rtipapi.h for possible values */
#endif
    int  RTP_FAR minor_number = PARAM_MINOR_0;
#endif
    IOADDRESS RTP_FAR ed_io_add   = (IOADDRESS)0x300;   /* NOTE: not needed for PCI drivers */
    int  RTP_FAR ed_irq           = 0x0a;               /* NOTE: not needed for PCI drivers */
    RTP_UINT16 RTP_FAR ed_mem_add = 0xD000;             /* NOTE: not needed for PCI drivers */

/* ********************************************************************              */
/* THE FOLLOWING SECTION CONTAINS ALL THE DEFAULT CONFIGURATION INFORMATION          */
/* AND SPECIFIES WHICH TEST TO RUN.                                                  */
/* YOU NEED TO REVIEW THIS INFORMATION FOR YOUR ENVIRONMENT.                         */
/* YOU MIGHT NEED TO CHANGE SOME OF THE FOLLOWING VALUES IN THIS SECTION.            */
/* ********************************************************************              */

/* ********************************************************************            */
/* LOCAL and REMOTE IPV4 addresses                                                 */
#if (0)
RTP_UINT8 my_ip[IP_ALEN]     = {172, 20, 167, 10};
RTP_UINT8 his_ip[IP_ALEN]    = {172, 20, 167, 232};
#elif (USE_IP_OFFICE)
RTP_UINT8 my_ip[IP_ALEN]     = {192, 168, 0, 10};
RTP_UINT8 his_ip[IP_ALEN]    = {192, 168, 0, 11};
#elif (1)
RTP_UINT8 my_ip[IP_ALEN]     = {192, 168, 1, 201};
RTP_UINT8 his_ip[IP_ALEN]    = {192, 168, 1, 11};
#else
RTP_UINT8 my_ip[IP_ALEN]     = {169, 254, 137, 10};
RTP_UINT8 his_ip[IP_ALEN]    = {169, 254, 137, 11};
#endif

/* ********************************************************************         */
RTP_UINT8 RTP_FAR ip_gw_address[IP_ALEN]     =
#if (USE_IP_OFFICE)
    {192,168,0,1};                                   /* gateway's ip address */
#else
    {192,168,1,1};                                   /* gateway's ip address */
#endif

/* ********************************************************************         */
/* DNS's ip address - all 255's means no DNS server          */
RTP_UINT8 RTP_FAR dns_ip_address[IP_LEN]    = 
    {192, 168, 1, 1}; // {255,255,255,255};

#if (INCLUDE_DNS)
char RTP_FAR dns_name[CFG_DNS_NAME_LEN] = "";   /* remote host name; only */
                                                /* valid if DNS server            */
                                                /* exists                         */
#endif

#if (USE_ETHER)
    RTP_UINT8 RTP_FAR ip_mask_address[IP_ALEN] = {0xff, 0xff, 0xff, 0};
#else
    RTP_UINT8 RTP_FAR ip_mask_address[IP_ALEN] = {0xff, 0xff, 0xff, 0xff};
#endif

/* ********************************************************************            */
/* END OF CONFIGURATION SECTION                                                    */
/* ********************************************************************            */
