/*                                                                                    */
/* CONFIG.H  - RTIP header file                                                       */
/*                                                                                    */
/*   EBS - RTIP                                                                       */
/*                                                                                    */
/*   Copyright EBSNet Inc. , 1993                                                     */
/*   All rights reserved.                                                             */
/*   This code may not be redistributed in source or linkable object form             */
/*   without the consent of its author.                                               */
/*                                                                                    */
/*    Module description:                                                             */
/*                                                                                    */


/* ********************************************************************             */
/* CONFIGURATION SECTION */
/* ********************************************************************             */
/* select which interface to use         */
#define USE_ETHER   1  /* ethernet or loopback */
#define USE_PPP     0
#define USE_SLIP    0
#define USE_CSLIP   0

/* ********************************************************************             */
/* length of string to hold acct info (user name, password, pap info etc)           */
#define ACCT_LEN 30

/* number of gateways per interface           */
#define NUM_GW 3

/* number of DNS servers         */
#define NUM_DNS_SRV 3

/* ********************************************************************             */
/* END OF CONFIGURATION SECTION */
/* ********************************************************************             */

/* THE REST OF THIS FILE MATCHES CONFIG.C but has externs */

extern int  RTP_FAR device_id;
extern IOADDRESS RTP_FAR ed_io_add;
extern int  RTP_FAR ed_irq;
extern RTP_UINT16 RTP_FAR ed_mem_add;

/* ********************************************************************              */
/* THE FOLLOWING SECTION CONTAINS ALL THE DEFAULT CONFIGURATION INFORMATION          */
/* AND SPECIFIES WHICH TEST TO RUN.                                                  */
/* YOU NEED TO REVIEW THIS INFORMATION FOR YOUR ENVIRONMENT.                         */
/* YOU MIGHT NEED TO CHANGE SOME OF THE FOLLOWING VALUES IN THIS SECTION.            */
/* ********************************************************************              */

/* ********************************************************************            */
/* LOCAL and REMOTE IPV4 addresses                                                 */

extern RTP_UINT8 my_ip[IP_ALEN];
extern RTP_UINT8 his_ip[IP_ALEN];

/* ********************************************************************         */
extern RTP_UINT8 RTP_FAR ip_gw_address[IP_ALEN];

/* ********************************************************************         */
extern RTP_UINT8 RTP_FAR dns_ip_address[IP_LEN];

#if (INCLUDE_DNS)
extern char RTP_FAR dns_name[CFG_DNS_NAME_LEN];
#endif

extern RTP_UINT8 RTP_FAR ip_mask_address[IP_ALEN];

extern  int RTP_FAR device_id;
extern  int RTP_FAR minor_number;

/* ********************************************************************            */
/* END OF CONFIGURATION SECTION                                                    */
/* ********************************************************************            */

