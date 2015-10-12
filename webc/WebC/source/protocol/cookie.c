//
// COOKIE.C - HTTP cookie management code
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//
#include "webc.h"
#include "cookie.h"
#include "vector.h"
#include "util.h"
#include "webcfg.h"
#include "webcstr.h"
#include "webcmem.h"

#if (WEBC_SUPPORT_COOKIES)

struct vector gpCookieVector[1];

static int cookies_initialized = 0;
static void initialize_cookies(void);
long giCookieSpace = WEBC_CFG_MAX_COOKIE_BYTES;

long webc_sizeof_cookie (CookieInfo * pCookie);
long webc_cookie_specificity (CookieInfo *pCookie);

#endif

void webc_check_all_cookie_expirations(void);
int webc_check_cookie_expiration(CookieInfo * pCookie);

int webc_set_cookie_uc(unsigned short *cookie, const unsigned short *defaultDomain, const unsigned short *defaultPath)
{
	char asciiCookie[384];
	char asciiDomain[128];
	char asciiPath[256];
	int n;
	
	for (n=0; n<383; n++)
	{
		asciiCookie[n] = (char) cookie[n];
		if (!cookie[n])
		{
			break;
		}
	}
	asciiCookie[383] = '\0';

	for (n=0; n<127; n++)
	{
		asciiDomain[n] = (char) defaultDomain[n];
		if (!defaultDomain[n])
		{
			break;
		}
	}
	asciiDomain[127] = '\0';

	for (n=0; n<255; n++)
	{
		asciiPath[n] = (char) defaultPath[n];
		if (!defaultPath[n])
		{
			break;
		}
	}
	asciiPath[255] = '\0';
	
	return webc_set_cookie(asciiCookie, asciiDomain, asciiPath);
}

int webc_set_cookie(char *cookie, const char *defaultDomain, const char *defaultPath)
{
#if (WEBC_SUPPORT_COOKIES)	
CookieInfo pCInfo[1];
CookieInfo *pCookie;
char *pos, *end;
long iCookieSize = 0;

	pCInfo->domain = WEBC_NULL;
	pCInfo->path   = WEBC_NULL;
	pCInfo->name   = WEBC_NULL;
	pCInfo->value  = WEBC_NULL;
	pCInfo->secure = 0;
	pCInfo->expires.year = 0;
	pCInfo->expires.second = 0;
	initialize_cookies();

	// eat beginning whitespace
	while (cookie && (*cookie) && IS_WHITESPACE(*cookie))
	{
		cookie++;
	}
    
	end = tc_strstr(cookie,"=");
	if (!end)
	{
		return (-1);
	}
	
	// Parse cookie name
	pos = cookie;
	*end = '\0';
	pCInfo->name = webc_malloc_string_copy_8(pos, __FILE__, __LINE__);
	*end = '=';
	end++;
	pos = end;
	
	// Parse cookie value
	end = tc_strstr(pos,";");
	if (end)
	{
		*end = '\0';
	}
	pCInfo->value = webc_malloc_string_copy_8(pos, __FILE__, __LINE__);
	if (end)
	{
		*end = ';';
		end++;
	}
	pos = end;

	if ((pCInfo->name == WEBC_NULL) || (pCInfo->value == WEBC_NULL))
	{
		if (pCInfo->name)
		{
			webc_free_string_copy_8(pCInfo->name, __FILE__, __LINE__);
		}
		if (pCInfo->value)
		{
			webc_free_string_copy_8(pCInfo->value, __FILE__, __LINE__);
		}
		return (-1);
	}
		
	// Parse other cookie parameters... domain, path, expires
	while (pos)
	{
		end = tc_strstr(pos, ";");
		if (end)
		{
			*end = '\0';
		}
		
		while (IS_WHITESPACE(*pos) && (*pos != '\0')) {pos++;}
			
		if (!tc_strnicmp(pos,"path",4))
		{
			while ((*pos != '=') && (*pos != '\0')) {pos++;}
			pos++;
			while (IS_WHITESPACE(*pos) && (*pos != '\0')) {pos++;}
			if (pCInfo->path)
			{
				webc_free_string_copy_8(pCInfo->path, __FILE__, __LINE__);
			}
			pCInfo->path = webc_malloc_string_copy_8(pos, __FILE__, __LINE__);
		}
		else if (!tc_strnicmp(pos,"domain",5))
		{
			while ((*pos != '=') && (*pos != '\0')) {pos++;}
			pos++;
			while (IS_WHITESPACE(*pos) && (*pos != '\0')) {pos++;}
			if (pCInfo->domain)
			{
				webc_free_string_copy_8(pCInfo->domain, __FILE__, __LINE__);
			}
			pCInfo->domain = webc_malloc_string_copy_8(pos, __FILE__, __LINE__);
		}
		else if (!tc_strnicmp(pos,"expires",7))
		{
			// remove the equals and any leading white-space
			while ((pos && *pos) && (*pos != '=')) {pos++;}
			if (pos && *pos){pos++;}
			while ((pos && *pos) && (IS_WHITESPACE(*pos))) {pos++;}
			
			// parse the cookie's expiration time
			rtp_date_parse_time(&pCInfo->expires, pos);
		}
		else if (!tc_strnicmp(pos,"secure",6))
		{
			pCInfo->secure = 1;
		}
			
		if (end)
		{
			*end = ';';
			end++;
		}
		pos = end;
	}
	
	if (pCInfo->path == WEBC_NULL)
	{
		pCInfo->path = webc_malloc_string_copy_8(defaultPath, __FILE__, __LINE__);
		
		if (pCInfo->path)
		{
			// remove any trailing GET parameters (anything after a '?') 
			// in the copied path
			for (pos = pCInfo->path; *pos; pos++)
			{
				if (*pos == '?')
				{
					*pos = 0; 
					break;
				}
			}
		}
	}
	
	if (pCInfo->domain == WEBC_NULL)
	{
		pCInfo->domain = webc_malloc_string_copy_8(defaultDomain, __FILE__, __LINE__);
	}

	if (!pCInfo->domain || !pCInfo->path || !pCInfo->name || !pCInfo->value)
	{
		webc_free_cookie_data(pCInfo);
		return (-1);
	}

	// this cookie always replaces the old version
	pCookie = webc_find_exact_cookie(pCInfo->name, pCInfo->domain, pCInfo->path);
	if (pCookie)
	{
		webc_delete_cookie(pCookie);
	}
			
	// don't add this cookie if it's already expired
	if (webc_check_cookie_expiration(pCInfo))
	{
		webc_free_cookie_data(pCInfo);
		return (-1);
	}
	
	iCookieSize = webc_sizeof_cookie(pCInfo);
	if (iCookieSize > WEBC_CFG_MAX_COOKIE_BYTES)
	{
		// this cookie is too big for the current configuration
		webc_free_cookie_data(pCInfo);
		return (-1);		
	}
	
	if (pCInfo->domain)
	{
		CookieIterator ci[1];

		// First free up some cookie memory to store this cookie
		while (giCookieSpace < iCookieSize)
		{
			pCookie = webc_get_first_cookie(ci);
			if (!pCookie)
			{
				// we should never get here, but just in case...
				webc_free_cookie_data(pCInfo);
				return (-1);
			}
			
			giCookieSpace += webc_sizeof_cookie(pCookie);

			webc_free_cookie_data(pCookie);
			vector_delete_node(gpCookieVector, 0);
		}
				
		// This is a new cookie; add it to the list
		//  tbd - add in order of path specificity
		if (vector_add_node_to_rear(gpCookieVector, (WEBC_PFBYTE) pCInfo) < 0)
		{
			// malloc failure
			webc_free_cookie_data(pCInfo);
			return (-1);
		}

		giCookieSpace -= iCookieSize;		
	}
	else
	{
		webc_free_cookie_data(pCInfo);
	}
#endif

	return (0);
}

#if (WEBC_SUPPORT_COOKIES)
CookieInfo *webc_insert_cookie(CookieInfo * cookie)
{
	vector_iterator iter;
	initialize_cookies();
	if (vector_add_node_to_front(gpCookieVector, (WEBC_PFBYTE) cookie) < 0)
	{
		// malloc failure
		webc_free_cookie_data(cookie);
		return (0);
	}

	giCookieSpace -= webc_sizeof_cookie(cookie);

	return (CookieInfo*)vector_get_first(gpCookieVector, &iter);

}
#endif // WEBC_SUPPORT_COOKIES

#if (WEBC_SUPPORT_COOKIES)
void initialize_cookies(void)
{
	if (!cookies_initialized)
	{
		vector_init(gpCookieVector, sizeof(CookieInfo));
		cookies_initialized = 1;
	}
}
#endif

void webc_delete_cookies(void)
{
#if (WEBC_SUPPORT_COOKIES)
CookieIterator pCI[1];
CookieInfo *pCookie = webc_get_first_cookie(pCI);

	while (pCookie)
	{
		giCookieSpace += webc_sizeof_cookie(pCookie); // before free
		webc_free_cookie_data(pCookie);		
		pCookie = webc_get_next_cookie(pCI);
	}
	
	vector_delete(gpCookieVector);
#endif
}

void webc_free_cookie_data(CookieInfo * cookie)
{
#if (WEBC_SUPPORT_COOKIES)
	if (cookie)
	{
		webc_free_string_copy_8(cookie->name, __FILE__, __LINE__);
		webc_free_string_copy_8(cookie->value, __FILE__, __LINE__);
		webc_free_string_copy_8(cookie->domain, __FILE__, __LINE__);
		webc_free_string_copy_8(cookie->path, __FILE__, __LINE__);
		
		cookie->name = 0;
		cookie->value = 0;
		cookie->domain = 0;
		cookie->path = 0;
		cookie->secure = 0;
		cookie->expires.year = 0;
		cookie->expires.second = 0;
	}		
#endif
}

CookieInfo *webc_get_first_cookie(CookieIterator *pCI)
{
#if (WEBC_SUPPORT_COOKIES)
	initialize_cookies();
	
	return (CookieInfo *) vector_get_first(gpCookieVector, pCI->vi);
#else
	return (0);
#endif
}

CookieInfo *webc_get_next_cookie(CookieIterator *pCI)
{
#if (WEBC_SUPPORT_COOKIES)
	initialize_cookies();

	return (CookieInfo *) vector_get_next(pCI->vi);
#else
	return (0);
#endif
}

CookieInfo *webc_get_first_matching_cookie(const char *host, const char *path, CookieIterator *pCI)
{
#if (WEBC_SUPPORT_COOKIES)
CookieInfo *pCookie = webc_get_first_cookie(pCI);

	while (pCookie && !webc_cookie_match(pCookie, host, path))
	{
		pCookie = webc_get_next_cookie(pCI);
	}
	
	return pCookie;
#else
	return (0);
#endif
}

CookieInfo *webc_get_next_matching_cookie(const char *host, const char *path, CookieIterator *pCI)
{
#if (WEBC_SUPPORT_COOKIES)
CookieInfo *pCookie = webc_get_next_cookie(pCI);

	while (pCookie && !webc_cookie_match(pCookie, host, path))
	{
		pCookie = webc_get_next_cookie(pCI);
	}
	
	return pCookie;
#else
	return (0);
#endif
}

int webc_cookie_match(CookieInfo *pCookie, const char *host, const char *path)
{
#if (WEBC_SUPPORT_COOKIES)
int iDomainLen, iHostLen;
char *domain;

	if (!pCookie->domain)
	{
		return 0;
	}

	if (webc_check_cookie_expiration(pCookie) == 1)
	{
		return 0;
	}

	domain = pCookie->domain;
	
	iDomainLen = tc_strlen(domain);
	iHostLen = tc_strlen(host);
	
	if (iHostLen > iDomainLen)
	{
		host += (iHostLen - iDomainLen);
	}
	else
	{
		domain += (iDomainLen - iHostLen);
	}
	
	if (!tc_stricmp(host, domain))
	{
		if (!pCookie->path)
		{
			return 1;
		}
		
		if (!tc_strnicmp(path, pCookie->path, tc_strlen(pCookie->path)))
		{
			return (1);
		}
	}
#endif	
	return (0);
}

long webc_print_matching_cookies(char *buffer, const char *host, const char* path, int secure_connection)
{
CookieInfo *pCookie = 0;
CookieIterator pCI[1];
long len = 0;
int lastCookie = 0;

	// is there somewhere better to garbage collect expired cookies?
	webc_check_all_cookie_expirations();
	pCookie = webc_get_first_matching_cookie(host, path, pCI);

	while (pCookie)
	{
		if (pCookie->name && (secure_connection || !pCookie->secure))
		{
			if (lastCookie)
			{
				len = tc_memcat(buffer, len, "; ", tc_strlen("; "));
			}

			len = tc_memcat(buffer, len, pCookie->name, tc_strlen(pCookie->name));

			if (pCookie->value)
			{
				len = tc_memcat(buffer, len, "=", tc_strlen("="));
				len = tc_memcat(buffer, len, pCookie->value, tc_strlen(pCookie->value));
			}
	
			lastCookie = 1;
		}
		pCookie = webc_get_next_matching_cookie(host, path, pCI);
	}
	
	return (len);
}

void webc_check_all_cookie_expirations(void)
{
#if (WEBC_SUPPORT_COOKIES)
	CookieIterator ci[1];
	CookieInfo *pCookie;
	int index; 
	
	pCookie = webc_get_first_cookie(ci);
	index = 0; 
	while (pCookie)
	{
		if (webc_check_cookie_expiration(pCookie) != 0)
		{
			// remove expired cookie
			giCookieSpace += webc_sizeof_cookie(pCookie); // before free
			webc_free_cookie_data(pCookie);
			pCookie = webc_get_next_cookie(ci);
			// vector_delete_node must be *after* webc_get_next_cookie
			// or the CookieIterator will get messed up because of 
			// the deleted vector node
			vector_delete_node(gpCookieVector, index);
		}
		else
		{
			index++; 
			pCookie = webc_get_next_cookie(ci);
		}
	}
#endif // WEBC_SUPPORT_COOKIES
    return;
}

int webc_check_cookie_expiration(CookieInfo * pCookie)
{
#if (WEBC_SUPPORT_COOKIES)
	// is this a valid cookie with a valid expiration
	if (   (pCookie)
	    && (pCookie->expires.year != 0)
	    && (pCookie->expires.second != 0)
	   )
	{
		RTP_TIMESTAMP now;
		if (rtp_date_get_timestamp(&now) == 0)
		{
			if (rtp_date_compare_time(&pCookie->expires, &now) < 0)
			{
				// expired cookie
				return(1);
			}
		}
	}
#endif // WEBC_SUPPORT_COOKIES

    return(0);
}
// moved cookie details into cookie.c from htmlsdk.c

int webc_delete_cookie(CookieInfo *pCookie)
{
#if (WEBC_SUPPORT_COOKIES)
	CookieIterator ci[1];
	CookieInfo *pCurCookie;
	int index; 

    if (pCookie)
    {
		pCurCookie = webc_get_first_cookie(ci);
		index = 0; 
		while (pCurCookie)
		{
			if (pCurCookie == pCookie)
			{
				giCookieSpace += webc_sizeof_cookie(pCookie);
				webc_free_cookie_data(pCookie);
				vector_delete_node(gpCookieVector, index);
				return(0); 
			}
			index++; 
			pCurCookie = webc_get_next_cookie(ci);
		}
    }
#endif
    return(-1);
}

CookieInfo *webc_create_cookie(const char *name, const char *value, const char *host, const char *path, int secure)
{
    CookieInfo *newCookie = WEBC_NULL;

#if (WEBC_SUPPORT_COOKIES)
    int nameSz, valueSz, hostSz, pathSz, secureSz;
    char *buf;

    nameSz  = (name)  ? tc_strlen(name)  : 0;
    valueSz = (value) ? tc_strlen(value) : 0;
    hostSz  = (host)  ? tc_strlen(host)  : 0;
    pathSz  = (path)  ? tc_strlen(path)  : 0;
    secureSz = (secure) ? 7 : 0;

    buf = (char *) WEBC_MALLOC(nameSz+valueSz+hostSz+pathSz+secureSz+34);
    if (buf)
    {
        buf[0] = 0;

        if (name)
        {
            tc_strcat(buf,name);

            if (value)
            {
                tc_strcat(buf,"=");
                tc_strcat(buf,value);
            }

            tc_strcat(buf,"; ");

            if (host)
            {
                tc_strcat(buf,"domain=");
                tc_strcat(buf,host);
                tc_strcat(buf,"; ");
            }

            if (path)
            {
                tc_strcat(buf,"path=");
                tc_strcat(buf,path);
                tc_strcat(buf,"; ");
            }
            
            if (secure)
            {
                tc_strcat(buf,"secure;");
            }
        }

        webc_set_cookie(buf,host,path);

        WEBC_FREE(buf);

        newCookie = webc_find_cookie(name, host, path, 1);
    }
#endif // WEBC_SUPPORT_COOKIES

    return(newCookie);
}

CookieInfo *webc_find_cookie(const char *name, const char *host, const char *path, int num)
{
  #if (WEBC_SUPPORT_COOKIES)

	CookieInfo *pCookie = 0;
	CookieIterator pCI[1];
	long len = 0;
	
	pCookie = webc_get_first_matching_cookie(host, path, pCI);
	
	while (pCookie)
	{
		if (tc_stricmp(pCookie->name, name) == 0)
		{
			// found a matching cookie
			if (num > 0)
			{
				num--;
			}
			else
			{
				return (pCookie);
			}
		}
		
		pCookie = webc_get_next_matching_cookie(host, path, pCI);
	}

  #endif // WEBC_SUPPORT_COOKIES

    return(WEBC_NULL);
}

CookieInfo *webc_find_exact_cookie (const char *name, const char *host, const char *path)
{
  #if (WEBC_SUPPORT_COOKIES)
	
	CookieInfo *pCookie = 0;
	CookieIterator ci;
	
	pCookie = webc_get_first_cookie(&ci);
	while (pCookie)
	{
		if (!tc_stricmp(name, pCookie->name) &&
		    !tc_stricmp(host, pCookie->domain) &&
		    !tc_stricmp(path, pCookie->path))
	    {
	    	return (pCookie);
	    }
		pCookie = webc_get_next_cookie(&ci);
	}
	  
  #endif /* WEBC_SUPPORT_COOKIES */
	
	return (WEBC_NULL);
}

const char *webc_cookie_get_name(CookieInfo *cookie)
{
	char *cookieName = WEBC_NULL;
	
  #if (WEBC_SUPPORT_COOKIES)
  
	if (cookie)
	{
		cookieName = cookie->name;
	}
	
  #endif // WEBC_SUPPORT_COOKIES
	
	return(cookieName);
}

const char *webc_cookie_get_value(CookieInfo *cookie)
{
	char *cookieValue = WEBC_NULL;
	
  #if (WEBC_SUPPORT_COOKIES)
  
	if (cookie)
	{
		cookieValue = cookie->value;
	}
	
  #endif // WEBC_SUPPORT_COOKIES
	
	return(cookieValue);
}

const char *webc_cookie_get_host(CookieInfo *cookie)
{
	char *cookieName = WEBC_NULL;
	
  #if (WEBC_SUPPORT_COOKIES)
	
	if (cookie)
	{
		cookieName = cookie->domain;
	}
	
  #endif // WEBC_SUPPORT_COOKIES
	
	return(cookieName);
}

int webc_cookie_get_secure(CookieInfo *cookie)
{
#if (WEBC_SUPPORT_COOKIES)
    if (cookie)
    {
        return(cookie->secure);
    }
#endif // WEBC_SUPPORT_COOKIES

    return(0);
}

const char *webc_cookie_get_path(CookieInfo *cookie)
{
	char *cookieName = WEBC_NULL;
	
  #if (WEBC_SUPPORT_COOKIES)
  
	if (cookie)
	{
		cookieName = cookie->path;
	}
	
  #endif // WEBC_SUPPORT_COOKIES
	
	return(cookieName);
}

void webc_cookie_set_name(CookieInfo *cookie, const char *name)
{
  #if (WEBC_SUPPORT_COOKIES)
	if (cookie)
	{
		char *newName = webc_malloc_string_copy_8(name, __FILE__, __LINE__);
		if (newName)
		{
			giCookieSpace += webc_sizeof_cookie(cookie);
			
			webc_free_string_copy_8(cookie->name, __FILE__, __LINE__);
			cookie->name = newName;
			
			giCookieSpace -= webc_sizeof_cookie(cookie);
		}
	}
  #endif // WEBC_SUPPORT_COOKIES
	
	return;
}

void webc_cookie_set_value(CookieInfo *cookie, const char *value)
{
  #if (WEBC_SUPPORT_COOKIES)
	if (cookie)
	{
		char *newValue = webc_malloc_string_copy_8(value, __FILE__, __LINE__);
		if (newValue)
		{
			giCookieSpace += webc_sizeof_cookie(cookie);
			
			webc_free_string_copy_8(cookie->value, __FILE__, __LINE__);
			cookie->value = newValue;
			
			giCookieSpace -= webc_sizeof_cookie(cookie);
		}
	}
  #endif // WEBC_SUPPORT_COOKIES
	
	return;
}

void webc_cookie_set_host(CookieInfo *cookie, const char *host)
{
  #if (WEBC_SUPPORT_COOKIES)
	if (cookie)
	{
		char *newHost = webc_malloc_string_copy_8(host, __FILE__, __LINE__);
		if (newHost)
		{
			giCookieSpace += webc_sizeof_cookie(cookie);
			
			webc_free_string_copy_8(cookie->domain, __FILE__, __LINE__);
			cookie->domain = newHost;
			
			giCookieSpace -= webc_sizeof_cookie(cookie);
		}
	}
  #endif // WEBC_SUPPORT_COOKIES
	
	return;
}

void webc_cookie_set_path(CookieInfo *cookie, const char *path)
{
  #if (WEBC_SUPPORT_COOKIES)
	if (cookie)
	{
		char *newPath = webc_malloc_string_copy_8(path, __FILE__, __LINE__);
		if (newPath)
		{
			giCookieSpace += webc_sizeof_cookie(cookie);
			
			webc_free_string_copy_8(cookie->path, __FILE__, __LINE__);
			cookie->path = newPath;
			
			giCookieSpace -= webc_sizeof_cookie(cookie);
		}
	}
  #endif // WEBC_SUPPORT_COOKIES
	
	return;
}

void webc_cookie_set_secure(CookieInfo *cookie, int secure)
{
#if (WEBC_SUPPORT_COOKIES)
    if (cookie)
    {
    	cookie->secure = secure;
    }
#endif // WEBC_SUPPORT_COOKIES

    return;
}

#if (WEBC_SUPPORT_COOKIES)

long webc_sizeof_cookie (CookieInfo * pCookie)
{
	return (tc_strlen(pCookie->name) + tc_strlen(pCookie->value) + 
	        tc_strlen(pCookie->domain) + tc_strlen(pCookie->path) + 
	        sizeof(CookieInfo) + sizeof(CookieInfo *) + 4*sizeof(char));
}

long webc_cookie_specificity (CookieInfo *pCookie)
{
	long specificity = (tc_strlen(pCookie->path) << 15) | tc_strlen(pCookie->domain);
	return (specificity);
}

#endif // WEBC_SUPPORT_COOKIES
