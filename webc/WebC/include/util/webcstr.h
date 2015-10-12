#ifndef __WEBCSTR_H_INCLUDED__
#define __WEBCSTR_H_INCLUDED__

#include "rtpstr.h"
#include "rtptotc.h"
#include "rtpscnv.h"
#include "rtpwcs.h"
#if (WEBC_SUPPORT_UNICODE)
#include "rtpwscnv.h"
#endif
#include "webctypes.h"

// WEBC_CHAR type routines

#if (WEBC_SUPPORT_UNICODE)

/*---------------------------------------------------------------------------*/
/* 16-bit Unicode String Mode */

#define webc_atoi                   rtp_wtol
#define webc_strcpy                 rtp_wcscpy
#define webc_strncpy                rtp_wcsncpy
#define webc_strcmp                 rtp_wcscmp
#define webc_strncmp                rtp_wcsncmp
#define webc_strstr                 rtp_wcsstr
#define webc_stristr                rtp_wcsistr
#define webc_strlen                 rtp_wcslen
#define webc_strcat                 rtp_wcscat
#define webc_strncat                rtp_wcsncat
#define webc_strchr                 rtp_wcschr
#define webc_strspn                 rtp_wcsspn
#define webc_stricmp                rtp_wcsicmp
#define webc_strnicmp               rtp_wcsnicmp

#define _WEBC_STR(X)                L##X

#ifdef __TSDPROJECT__		/* Generic solution provided below */
typedef unsigned int		size_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

void       webc_c_strcpy   (WEBC_CHAR *webcStr, const char *str);
void       webc_c_strncpy  (WEBC_CHAR *webcStr, const char *str, long n);
void       webc_to_c_strcpy (char *dstCStr, const WEBC_CHAR *webcStr);
void       webc_c_strcat   (WEBC_CHAR *webcStr, const char *str);
void       webc_c_strncat  (WEBC_CHAR *webcStr, const char *str, long n);
int        webc_c_strcmp   (const WEBC_CHAR *webcStr, const char *str);
int        webc_c_strncmp  (const WEBC_CHAR *webcStr, const char *str, long n);
int        webc_c_stricmp  (const WEBC_CHAR *webcStr, const char *str);
int        webc_c_strnicmp (const WEBC_CHAR *webcStr, const char *str, long n);
WEBC_CHAR* webc_c_strstr   (const WEBC_CHAR *str, const char *subStr);
WEBC_CHAR* webc_c_stristr  (const WEBC_CHAR *str, const char *subStr);
WEBC_CHAR* webc_c_strchrs  (const WEBC_CHAR *str, const char *chrs);
size_t     webc_c_strcspn  (const WEBC_CHAR *str, const char *chrs);
size_t     webc_c_strlen  (const char *chrs);

char     * webc_malloc_string_copy_8 (const char* str, const char* file, unsigned int line);
void       webc_free_string_copy_8   (char* str, const char* file, unsigned int line);
char     * webc_arg_to_char          (const WEBC_CHAR* str);
void       webc_arg_done             (char* arg);

#ifdef __cplusplus
}
#endif

#else /* !(WEBC_SUPPORT_UNICODE) */

/*---------------------------------------------------------------------------*/
/* 8-bit String Mode */

#define webc_atoi                   rtp_atol
#define webc_strcpy                 rtp_strcpy
#define webc_strncpy                rtp_strncpy
#define webc_strcmp                 rtp_strcmp
#define webc_strncmp                rtp_strncmp
#define webc_strstr                 rtp_strstr
#define webc_stristr                rtp_stristr
#define webc_strlen                 rtp_strlen
#define webc_strcat                 rtp_strcat
#define webc_strncat                rtp_strncat
#define webc_strchr                 rtp_strchr
#define webc_strspn                 rtp_strspn
#define webc_stricmp                rtp_stricmp
#define webc_strnicmp               rtp_strnicmp

#define webc_c_strcpy               webc_strcpy
#define webc_c_strncpy              webc_strncpy
#define webc_to_c_strcpy            webc_strcpy
#define webc_c_strcat               webc_strcat
#define webc_c_strncat              webc_strncat
#define webc_c_strcmp               webc_strcmp
#define webc_c_strncmp              webc_strncmp
#define webc_c_stricmp              webc_stricmp
#define webc_c_strnicmp             webc_strnicmp
#define webc_c_strstr               webc_strstr
#define webc_c_stristr              webc_stristr
#define webc_c_strchrs              rtp_strpbrk
#define webc_c_strcspn              rtp_strcspn
#define webc_c_strlen               rtp_strlen

#define webc_malloc_string_copy_8   webc_malloc_string_copy
#define webc_free_string_copy_8     webc_free_string_copy
#define webc_arg_to_char(X)         (char*)(X)
#define webc_arg_done(X)

#define _WEBC_STR(X)                X

#endif /* WEBC_SUPPORT_UNICODE */

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

WEBC_CHAR* webc_malloc_string_copy (const WEBC_CHAR* str, const char* file, unsigned int line);
void       webc_free_string_copy   (WEBC_CHAR* str, const char* file, unsigned int line);
long       webc_scan_string        (WEBC_CHAR* dest, WEBC_CHAR* src, long maxLen);
WEBC_CHAR* webc_scan_whitespace    (WEBC_CHAR* src);
void       webc_print_dec          (WEBC_CHAR* str, long num, int min, int max);
void       webc_print_hex          (WEBC_CHAR* str, long num, int min, int max, int upperCase);

#ifdef __cplusplus
}
#endif


#endif // __WEBCSTR_H_INCLUDED__
