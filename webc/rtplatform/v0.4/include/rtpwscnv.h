/*
|  RTPWSCNV.H - 
| 
|  EBSnet - 
| 
|   $Author: tony $
|   $Date: 2004/09/01 21:42:33 $
|   $Name:  $
|   $Revision: 1.2 $
| 
|  Copyright EBSnet Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __RTPWSCNV_H__
#define __RTPWSCNV_H__

#define rtp_wtoi(S)   ((int)rtp_wcstol((S), 0, 10))
#define rtp_wtol(S)   rtp_wcstol((S), 0, 10)
/*#define rtp_wcstol    */
/*#define rtp_wcstoul   */
/*#define rtp_itow      */
/*#define rtp_ltow      */
/*#define rtp_ultow     */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef rtp_itow
unsigned short *     rtp_itow     (int, unsigned short *, int);
#endif
#ifndef rtp_ltow
unsigned short *     rtp_ltow     (long, unsigned short *, int);
#endif
#ifndef rtp_ultow
unsigned short *     rtp_ultow    (unsigned long, unsigned short *, int);
#endif

#ifndef rtp_wtoi
int                  rtp_wtoi     (const unsigned short * s);
#endif
#ifndef rtp_wtol
long                 rtp_wtol     (const unsigned short * s);
#endif

#ifndef rtp_wcstol
long                 rtp_wcstol   (const unsigned short * str, unsigned short ** delimiter, int base);
#endif
#ifndef rtp_wcstoul
unsigned long        rtp_wcstoul  (const unsigned short * str, unsigned short ** delimiter, int base);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RTPWSCNV_H__ */
