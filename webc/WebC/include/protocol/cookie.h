//
// COOKIE.H - Structs, prototypes, and defines for COOKIE.C
//
// EBS - WebC
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
#ifndef __WEB_COOKIE_INCLUDED__
#define __WEB_COOKIE_INCLUDED__

#include "vector.h"
#include "webc.h"
#include "rtpdutil.h"

typedef struct
{
	char*         domain;
	char*         path;
	char*         name;
	char*         value;
	int	          secure;
	RTP_TIMESTAMP expires;
} CookieInfo;

typedef struct 
{
	vector_iterator vi[1];
} CookieIterator;

#ifdef __cplusplus
extern "C" {
#endif

extern struct vector gpCookieVector[1];

int         webc_set_cookie(char *cookie, const char *defaultDomain, const char *defaultPath);
int         webc_set_cookie_uc(unsigned short *cookie, const unsigned short *defaultDomain, const unsigned short *defaultPath);
CookieInfo *webc_insert_cookie(CookieInfo * cookie);
CookieInfo *webc_get_first_cookie(CookieIterator *pCI);
CookieInfo *webc_get_next_cookie(CookieIterator *pCI);
CookieInfo *webc_get_first_matching_cookie(const char *host, const char *path, CookieIterator *pCI);
CookieInfo *webc_get_next_matching_cookie(const char *host, const char *path, CookieIterator *pCI);
void        webc_free_cookie_data(CookieInfo * cookie);
int         webc_cookie_match(CookieInfo *pCookie, const char *host, const char *path);
void        webc_delete_cookies(void);
long        webc_print_matching_cookies(char *buffer, const char *host, const char *path, int secure_connection);
long        webc_print_matching_cookies_uc(unsigned short *buffer, const unsigned short *host, const unsigned short *path, int secure_connection);

CookieInfo *webc_create_cookie(const char *name, const char *value, const char *host, const char *path, int secure);
int         webc_delete_cookie(CookieInfo *cookie);
CookieInfo *webc_find_cookie (const char *name, const char *host, const char *path, int num);
CookieInfo *webc_find_exact_cookie (const char *name, const char *host, const char *path);
const char *webc_cookie_get_name  (CookieInfo *cookie);
const char *webc_cookie_get_value (CookieInfo *cookie);
const char *webc_cookie_get_host  (CookieInfo *cookie);
const char *webc_cookie_get_path  (CookieInfo *cookie);
int		    webc_cookie_get_secure(CookieInfo *cookie);
void        webc_cookie_set_name  (CookieInfo *cookie, const char *name);
void        webc_cookie_set_value (CookieInfo *cookie, const char *value);
void        webc_cookie_set_host  (CookieInfo *cookie, const char *host);
void        webc_cookie_set_path  (CookieInfo *cookie, const char *path);
void		webc_cookie_set_secure(CookieInfo *cookie, int secure);

/* tbd
int webc_store_cookies(webcStreamFn _write_fn, void *handle);
int webc_retrieve_cookies(webcStreamFn _read_fn, void *handle);

int webc_store_cookie(CookieInfo *pCookie, webcStreamFn _write_fn, void *handle);
int webc_retrieve_cookie(CookieInfo *pCookie, webcStreamFn _read_fn, void *handle);

int webc_add_cookie(CookieInfo *pCookie);
CookieInfo *webc_get_cookie(char *name, char *host, char *path, int num);
int webc_delete_matching_cookies(char *name, char *host, char *path);
*/

#ifdef __cplusplus
}
#endif

#endif
