/*
|  WEBCTYPES.H - WebC Basic Types
|
|  EBS -
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#ifndef __WEBCTYPES_H__
#define __WEBCTYPES_H__

#ifdef __GNUC__
#include <sys/types.h> /* for size_t */
#endif
typedef unsigned       WEBC_BOOL;
typedef char           WEBC_CHAR8;
typedef unsigned char  WEBC_UCHAR8;
typedef unsigned short WEBC_CHAR16;
typedef signed char    WEBC_INT8;
#define WEBC_PFBYTE unsigned char *
#define WEBC_UINT8 unsigned char
typedef signed short   WEBC_INT16;
typedef unsigned short WEBC_UINT16;
typedef signed long    WEBC_INT32;
typedef unsigned long  WEBC_UINT32;
typedef long           WEBC_SIZE;
typedef long           WEBC_SSIZE;
typedef unsigned long  WEBC_USIZE;
typedef int            WEBC_INT;


#if (WEBC_SUPPORT_UNICODE)
typedef WEBC_CHAR16    WEBC_CHAR;
#else
typedef WEBC_CHAR8     WEBC_CHAR;
#endif

#define WEBC_TRUE      1
#define WEBC_FALSE     0
#define WEBC_READONLY  const
#define WEBC_FAR

#ifdef __cplusplus
#define WEBC_NULL 0
#else
#define WEBC_NULL ((void*)0)
#endif /* c++ */

/* type used for all coordinate values */
typedef WEBC_INT32 DISPLAY_INT;

#define DISPLAY_INT_MIN (-0x7fffffffL)
#define DISPLAY_INT_MAX (0x7fffffffL)

#define WEBC_MAX_SIGNED_VALUE(T)    (((1 << ((sizeof(T) << 3) - 1)) - 1)
#define WEBC_MIN_SIGNED_VALUE(T)    (-((1 << ((sizeof(T) << 3) - 1)) - 1)
#define WEBC_MAX_UNSIGNED_VALUE(T)  ((1 << (sizeof(T) << 3)) - 1)

// max length of a file path name
#define WEBC_MAX_PATH 1024

#endif /* __WEBCTYPES_H__ */
