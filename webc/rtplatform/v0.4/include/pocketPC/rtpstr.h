/*
|  RTPSTR.H - Runtime Platform String Library
|
|  MICROSOFT C/C++ 32-bit compiler-specific file
|
|  EBSnet -
|
|   $Author: tony $
|   $Date: 2004/12/21 22:13:39 $
|   $Name:  $
|   $Revision: 1.1 $
|
|  Copyright EBSnet Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#ifndef __RTPSTR_H__
#define __RTPSTR_H__

#include <string.h>

/* Define any of these to map directly to another implementation of
   the function, or leave undefined to use the generic implementation */

#define rtp_memchr   memchr
#define rtp_memcmp   memcmp
#define rtp_memcpy   memcpy
#define rtp_memmove  memmove
#define rtp_memset   memset
//#define rtp_stricmp  stricmp
#define rtp_strncmp  strncmp
//#define rtp_strnicmp strnicmp
#define rtp_strcat   strcat
#define rtp_strncat  strncat
#define rtp_strchr   strchr
#define rtp_strcmp   strcmp
#define rtp_strcpy   strcpy
#define rtp_strlen   strlen
#define rtp_strncpy  strncpy
#define rtp_strrchr  strrchr
#define rtp_strstr   strstr
#define rtp_strspn   strspn
#define rtp_strcspn  strcspn
#define rtp_strpbrk  strpbrk
/*#define rtp_memcat   */
/*#define rtp_stristr  */

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 * Non-ANSI C Standard library utility function declarations            *
 ************************************************************************/

#ifndef rtp_memcat
long          rtp_memcat   (char *dst, long offset, const char *src, long size);
#endif

#ifndef rtp_stricmp
int           rtp_stricmp  (const char *, const char *);
#endif
#ifndef rtp_stristr
char *        rtp_stristr  (const char *, const char *);
#endif
#ifndef rtp_strncmp
int           rtp_strncmp  (const char *, const char *, unsigned int);
#endif
#ifndef rtp_strnicmp
int           rtp_strnicmp (const char *, const char *, unsigned int);
#endif
#ifndef rtp_strchrs
char *        rtp_strchrs (char * str, const char * find_chrs);
#endif

/************************************************************************
 * ANSI C Standard library utility function declarations (if needed)    *
 ************************************************************************/

#ifndef rtp_memchr
void *        rtp_memchr   (const void * str, int chr, unsigned int n);
#endif
#ifndef rtp_memcmp
int           rtp_memcmp   (const void * a, const void * b, unsigned int n);
#endif
#ifndef rtp_memcpy
void *        rtp_memcpy   (void *a, const void *b, unsigned int n);
#endif
#ifndef rtp_memmove
void *        rtp_memmove  (void * a, const void * b, unsigned int n);
#endif
#ifndef rtp_memset
void *        rtp_memset   (void * p, int b, unsigned int n);
#endif

#ifndef rtp_strcat
char *        rtp_strcat   (char * a, const char * b);
#endif
#ifndef rtp_strncat
char *        rtp_strncat  (char * a, const char * b);
#endif
#ifndef rtp_strchr
char *        rtp_strchr   (const char * str, int find_chr);
#endif
#ifndef rtp_strcmp
int           rtp_strcmp   (const char * s1, const char * s2);
#endif
#ifndef rtp_strcpy
char *        rtp_strcpy   (char * a, const char * b);
#endif
#ifndef rtp_strlen
unsigned int  rtp_strlen   (const char * string);
#endif
#ifndef rtp_strncpy
char *        rtp_strncpy  (char * a, const char * b, unsigned int n);
#endif
#ifndef rtp_strrchr
char *        rtp_strrchr  (const char * str, int find_chr);
#endif
#ifndef rtp_strstr
char *        rtp_strstr   (const char * str, const char * find_str);
#endif
#ifndef rtp_strspn
unsigned long rtp_strspn (const char *str, const char *chrs)
#endif
#ifndef rtp_strcspn
unsigned long rtp_strcspn (const char *str, const char *chrs)
#endif


#ifdef __cplusplus
}
#endif

#endif /* __RTPSTR_H__ */
