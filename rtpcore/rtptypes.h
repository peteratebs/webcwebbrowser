 /*
 | RTPTYPES.H - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform
 |
 |  $Author: sarah $
 |  $Date: 2005/05/14 17:35:21 $
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

#ifndef __RTPTYPES_H__
#define __RTPTYPES_H__

#if !defined(__RTIP_TYPES__)

#if  (defined(__BORLANDC__) || defined(_MSC_VER) || defined(__WATCOMC__))
// ********************************************************************
/* BORLAND, MICROSOFT, WATCOM, and HIGHC                                  */
// ********************************************************************

/* Simplify test of if we're using microsoft 8.0 + in protected mode */
#define IS_MS_PM   0
#ifdef _MSC_VER
#undef IS_MS_PM
#define IS_MS_PM   ( (_MSC_VER >= 800) && (_M_IX86 >= 300) )
#endif

/* Simplify test of if we're using Borland + in protected mode */
#define IS_BCC_PM 0
#if (defined(__BORLANDC__))
#ifdef __WIN32__
#undef IS_BCC_PM
#define IS_BCC_PM 1
#endif
#endif

/* Simplify test of if we're using Watcom in protected mode */
#define IS_WC_PM 0
#if (defined(__WATCOMC__))
#ifdef __386__
#undef IS_WC_PM
#define IS_WC_PM 1
#endif
#endif

/* Simplify test of if we're using High C/C++ in protected mode   */
#define IS_HC_PM 0

#endif

// ********************************************************************
// RTP_FAR
// ********************************************************************
#define RTP_FAR
#define RTP_FAR_DECL
/* Set to 1 to instruct code in main.c to clear external memory */
#ifdef __IAR_SYSTEMS_ICC__
#undef RTP_FAR_DECL
#define EXRAM _Pragma("location=\"FarMemory\"")
#define RTP_FAR_DECL _Pragma("location=\"FarMemory\"")
#else
#define EXRAM
#endif

#if ( defined(__BORLANDC__) || defined(_MSC_VER) ||  defined(__WATCOMC__) )	// real mode
#undef RTP_FAR
#define RTP_FAR far
#endif

#if (IS_MS_PM || IS_BCC_PM || IS_WC_PM || IS_HC_PM || defined(MC68K) || MCF52XX || defined(DJGPP) || defined(SEG_IAR) || defined(RTPXGSH2) || defined(RTPXGSH3))
#undef RTP_FAR
#define RTP_FAR
#define far
#endif

// ********************************************************************
#define RTP_INLINE

// ********************************************************************
#define RTP_CONST  const
#define RTP_EXTERN extern

#define RTIP_STATIC static
#define RTP_STATIC  static

typedef char                              RTP_INT8;
typedef RTP_CONST char                    RTP_CINT8;
typedef unsigned char                     RTP_UINT8;
typedef RTP_CONST unsigned char           RTP_CUINT8;

typedef short                             RTP_INT16;
typedef RTP_CONST short                   RTP_CINT16;
typedef unsigned short                    RTP_UINT16;
typedef RTP_CONST unsigned short          RTP_CUINT16;
typedef long                              RTP_INT32;
typedef RTP_CONST long                    RTP_CINT32;
typedef unsigned long                     RTP_UINT32;
typedef RTP_CONST unsigned long           RTP_CUINT32;

typedef char                              RTP_CHAR;
typedef RTP_CONST char                    RTP_CCHAR;
typedef unsigned char                     RTP_UCHAR;
typedef RTP_CONST unsigned char           RTP_CUCHAR;

typedef char RTP_FAR*                     RTP_PFINT8;
typedef unsigned char RTP_FAR*            RTP_PFUINT8;
typedef RTP_CONST unsigned char RTP_FAR*  RTP_PFCUINT8;
typedef short RTP_FAR*                    RTP_PFINT16;
typedef unsigned short RTP_FAR*           RTP_PFUINT16;
typedef RTP_CONST unsigned short RTP_FAR* RTP_PFCUINT16;
typedef long RTP_FAR*                     RTP_PFINT32;
typedef unsigned long RTP_FAR*            RTP_PFUINT32;
typedef RTP_CONST unsigned long RTP_FAR*  RTP_PFCUINT32;

typedef char RTP_FAR*                     RTP_PFCHAR;
typedef RTP_CONST char RTP_FAR*           RTP_PFCCHAR;
typedef unsigned char RTP_FAR*            RTP_PFUCHAR;
typedef RTP_CONST unsigned char RTP_FAR*  RTP_PFCUCHAR;

typedef int RTP_FAR *                     RTP_PFINT;
typedef long  RTP_FAR *                   RTP_PFLONG;
typedef void  RTP_FAR *                   RTP_PFVOID;
typedef RTP_CONST void  RTP_FAR *         RTP_PFCVOID;

typedef unsigned                          RTP_BOOL;
typedef RTP_BOOL RTP_FAR *                RTP_PFBOOL;

#define RTP_TRUE  1
#define RTP_FALSE 0

// ********************************************************************
// *******************************************************************
// 64 bit pointer
// *******************************************************************
/* set to 1 if addresses are 64-bits */
/* SPRZ - tbd - porting issue */
#define KS_64BIT_ADDRS 0
#define KS_64BIT_LONG  0

#if (KS_64BIT_ADDRS)  /* 64bitpointer */
#define RTP_UINT64     unsigned long long
#define RTP_INT64      long long
#define RTP_UADDR      RTP_UINT64
#define RTP_ADDR       RTP_INT64
#define RTP_VOID_ADDR  RTP_INT64

#else

#define RTP_UADDR      RTP_UINT32
#define RTP_ADDR       RTP_INT32
#define RTP_VOID_ADDR  RTP_INT32
#endif

#if (!KS_64BIT_ADDRS && !KS_64BIT_LONG)
typedef struct rtp_int64
{
    RTP_UINT32 msb;
    RTP_UINT32 lsb;
} RTP_INT64;

typedef struct rtp_uint64
{
    unsigned long msb;
    unsigned long lsb;
} RTP_UINT64;

#elif (!KS_64BIT_ADDRS)
#define RTP_INT64      long long
#define RTP_UINT64     unsigned long long
#endif

/* Address constants */
#define RTP_NULL ((void *)0)
#define RTP_NOT  ((void *)0xffffffff)

#if (RTKERNEL32 || ONTIME32)
#define DLL_FAR    RTTDDL2
#define DLL_EXTERN RTTDDL
#else
#define DLL_FAR    RTP_FAR
#define DLL_EXTERN extern
#endif

// ********************************************************************
#endif  /* !defined(__RTIP_TYPES__) */
#endif /*__RTPTYPES_H__*/



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
