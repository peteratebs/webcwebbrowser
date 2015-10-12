 /*
 | RTP.H - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform
 |
 |  $Author: tony $
 |  $Date: 2005/07/12 15:41:15 $
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

#ifndef __RTPARCH_H__
#define __RTPARCH_H__

#ifdef __IAR_SYSTEMS_ICC__
#define EXRAM _Pragma("location=\"FarMemory\"")
#else
#define EXRAM
#endif

#endif /*__RTPARCH_H__*/



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
