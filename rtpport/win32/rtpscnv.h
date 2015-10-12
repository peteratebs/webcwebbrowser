/*
|  RTPSCNV.H - Runtime Platform String Conversion utilites
| 
|  MICROSOFT C/C++ 32-bit compiler-specific file
|
|  EBSnet - 
| 
|   $Author: tony $
|   $Date: 2004/08/23 20:36:31 $
|   $Name:  $
|   $Revision: 1.1 $
| 
|  Copyright EBSnet Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __RTPSCNV_H__
#define __RTPSCNV_H__

#include <stdlib.h>

#define rtp_hatoi(S)  ((int)rtp_strtol((S), 0, 16))
#define rtp_atoi      atoi
#define rtp_atol      atol
#define rtp_strtol    strtol
#define rtp_strtoul   strtoul
#define rtp_itoa      itoa
#define rtp_ltoa      ltoa
#define rtp_ultoa     ultoa

#ifdef __cplusplus
extern "C" {
#endif

#ifndef rtp_hatoi
unsigned int  rtp_hatoi    (const char *);
#endif
#ifndef rtp_itoa
char *        rtp_itoa     (int, char *, int);
#endif
#ifndef rtp_ltoa
char *        rtp_ltoa     (long, char *, int);
#endif
#ifndef rtp_ultoa
char *        rtp_ultoa    (unsigned long, char *, int);
#endif
#ifndef rtp_atoi
int           rtp_atoi     (const char * s);
#endif
#ifndef rtp_atol
long          rtp_atol     (const char * s);
#endif

#ifndef rtp_strtol
long          rtp_strtol   (const char * str, char ** delimiter, int base);
#endif
#ifndef rtp_strtoul
unsigned long rtp_strtoul  (const char * str, char ** delimiter, int base);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RTPSCNV_H__ */
