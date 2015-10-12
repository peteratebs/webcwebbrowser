/*
|  WEBCINLINE.H - 
| 
|  EBS - 
| 
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __WEBCINLINE_H__
#define __WEBCINLINE_H__

#ifdef _MSC_VER /* Microsoft C */
#define WEBC_INLINE   __inline

#elif defined(__MET__) || defined(WEBC_METAWARE_HCARM) /* Metaware */
#define WEBC_INLINE   _Inline

#else
#define WEBC_INLINE
#endif

#endif /* __WEBCINLINE_H__ */
