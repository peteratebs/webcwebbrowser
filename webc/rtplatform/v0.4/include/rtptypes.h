 /*
 | RTPTYPES.H - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:30 $
 |  $Name:  $
 |  $Revision: 1.5 $
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

#define RTP_FAR
#define RTP_CONST  const
#define RTP_EXTERN extern

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

typedef unsigned                      RTP_BOOL;

#define RTP_TRUE  1
#define RTP_FALSE 0



///// ITEMS PINCHED FROM OSTYPES.H
#define KS_CONSTANT const
#if (__cplusplus)
#define KS_GLOBAL_CONSTANT extern const
#else
#define KS_GLOBAL_CONSTANT const
#endif
#define KS_EXTERN_GLOBAL_CONSTANT extern const

typedef int RTP_FAR *                     RTP_PFINT;
typedef void  RTP_FAR *                   RTP_PFVOID;
typedef long                              RTP_INT64;
typedef long  RTP_FAR *                   RTP_PFLONG;

#define RTP_NUM_INTS 1 /* hopefully not used (number of interrupts) */

typedef	int PRTIP_CALLBACKS; // wrong

void RTP_DEBUG_ERROR(RTP_PFCCHAR string, int type, RTP_UINT32 val1, RTP_UINT32 val2);
//void  RTP_DEBUG_LOG(RTP_PFCCHAR string, int level, int type, RTP_UINT32 val1, RTP_UINT32 val2);


#endif /*__RTPTYPES_H__*/



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
