//
// WEBCSDKMISC.CPP -
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

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "webcjhconfig.h"
#include "htmlsdk.h"
#include "httpc.h"
#if (WEBC_SUPPORT_JSCRIPT)
 #include "wjsdoc.h"
 #include "wjsbrowser.h"
 #include "wjscript.hpp"
#endif
#if (WEBC_SUPPORT_COOKIES)
 #include "cookie.h"
#endif
#include "hchkbox.hpp"
#include "helement.hpp"
#include "hframe.hpp"
#include "himage.hpp"
#include "hobject.hpp"
#include "hinput.hpp"
#include "hhidden.hpp"
#include "hedit.hpp"
#include "hpasswd.hpp"
#include "hselect.hpp"
#include "htmldoc.hpp"
#include "htmlfind.hpp"
#include "format.hpp"
#include "webctmo.h"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "prothttp.h"
#include "wgkeys.hpp"
#include "jhutils.h"
#include "tuenum.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "rtpprint.h"
#include "webcdefault.h"
#ifdef WEBC_DEBUG
#include "webcLog.h"
#endif
#if (WEBC_DEBUG_JSCRIPT)
#include "wjsdebug.h"
#endif


/*****************************************************************************/
// Local types
/*****************************************************************************/
#include "webcsdk.hpp"


/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// External Declarations
/*****************************************************************************/


/*****************************************************************************/
// Macros
/*****************************************************************************/

/*****************************************************************************/
// HTML C API Functions
/*****************************************************************************/


// HEREHERE Begin websdkmisc.cpp

#if(1)

#if (WEBC_SUPPORT_COOKIES)


/*****************************************************************************/
/**
 @memo Create a cookie with the specified parameters

 @doc Given a cookie name, its value, host name and path, this function
 	  will return a handle to the newly created cookie.

 @param name The name of the cookie
 @param value Value the cookie will store
 @param host Domain the cookie was received from
 @param path Path of the cookie
 @return COOKIE_HANDLE: Handle to the new cookie
 @see webc_DestroyCookie
*/
/*****************************************************************************/

COOKIE_HANDLE webc_CreateCookie(char *name, char *value, char *host, char *path)
{
    return (COOKIE_HANDLE)webc_create_cookie(name, value, host, path, 0);

}

/*****************************************************************************/
/**
 @memo Destroys a cookie previously created with webc_CreateCookie

 @doc This function removes a cookie that was instantiated with webc_CreateCookie

 @param name The name of the cookie
 @param value Value the cookie will store
 @param host Domain the cookie was received from
 @param path Path of the cookie
 @return 0 if the cookie was successfully destroyed, -1 otherwise
 @see webc_CreateCookie
*/
/*****************************************************************************/

int webc_DestroyCookie(COOKIE_HANDLE cookie)
{

    return webc_delete_cookie((CookieInfo*)cookie);

}


/*****************************************************************************/
/**
 @memo Find a cookie that has the specified attributes

 @doc Find a cookie that has the specified attributes.  If more than one cookie matches,
      then the NUMth cookie will be returned.

 @param name The name of the cookie
 @param value Value the cookie stores
 @param host Domain the cookie was received from
 @param path Path of the cookie
 @param num Sequence number of the cookie (beginning from 0)
 @return 0 if the cookie was not found, or a handle to the cookie otherwise
 */
/*****************************************************************************/

COOKIE_HANDLE webc_FindCookie(const char *name, const char *host, const char *path, int num)
{
    return (COOKIE_HANDLE) webc_find_cookie(name, host, path, num);
}


/*****************************************************************************/
/**
 @memo Retrieve the name of a cookie

 @doc Retrieve the name of a cookie

 @param cookie Handle of the cookie
 @return The name of the cookie
 @see webc_CookieGetValue, webc_CookieGetHost, webc_CookieGetPath
 */
/*****************************************************************************/

const char *webc_CookieGetName(COOKIE_HANDLE cookie)
{
    return webc_cookie_get_name((CookieInfo*)cookie);
}


/*****************************************************************************/
/**
 @memo Retrieve a cookie's value

 @doc Retrieve a cookie's value

 @param cookie Handle of the cookie
 @return The cookie's value
 @see webc_CookieGetName, webc_CookieGetHost, webc_CookieGetPath
 */
/*****************************************************************************/

const char *webc_CookieGetValue(COOKIE_HANDLE cookie)
{
    return webc_cookie_get_value((CookieInfo*)cookie);
}


/*****************************************************************************/
/**
 @memo Get the host that a cookie was received from

 @doc Retrieve a cookie's value

 @param cookie Handle of the cookie
 @return The cookie's host
 @see webc_CookieGetName, webc_CookieGetValue, webc_CookieGetPath
 */
/*****************************************************************************/

const char *webc_CookieGetHost(COOKIE_HANDLE cookie)
{
    return webc_cookie_get_host((CookieInfo*)cookie);
}


/*****************************************************************************/
/**
 @memo Get the path of a cookie

 @doc Get the path of a cookie

 @param cookie Handle of the cookie
 @return The cookie's path
 @see webc_CookieGetName, webc_CookieGetValue, webc_CookieGetHost
 */
/*****************************************************************************/

const char *webc_CookieGetPath(COOKIE_HANDLE cookie)
{
    return webc_cookie_get_path((CookieInfo*)cookie);
}


/*****************************************************************************/
/**
 @memo Set the name of a cookie

 @doc Set the name of a cookie

 @param cookie Handle of the cookie
 @param name Pointer to the string containing the cookie's new name
 @see webc_CookieSetValue, webc_CookieSetHost, webc_CookieSetPath
 */
/*****************************************************************************/

void webc_CookieSetName(COOKIE_HANDLE cookie, const char *name)
{
    webc_cookie_set_name((CookieInfo*)cookie, name);
}


/*****************************************************************************/
/**
 @memo Set the value of a cookie

 @doc Set the value of a cookie

 @param cookie Handle of the cookie
 @param value Pointer to the string containing the cookie's new value
 @see webc_CookieSetName, webc_CookieSetHost, webc_CookieSetPath
 */
/*****************************************************************************/

void webc_CookieSetValue(COOKIE_HANDLE cookie, const char *value)
{
    webc_cookie_set_value((CookieInfo*)cookie, value);
}


/*****************************************************************************/
/**
 @memo Set a cookie's host

 @doc Set a cookie's host

 @param cookie Handle of the cookie
 @param host Pointer to the string containing the cookie's new host
 @see webc_CookieSetName, webc_CookieSetValue, webc_CookieSetPath
 */
/*****************************************************************************/

void webc_CookieSetHost(COOKIE_HANDLE cookie, const char *host)
{
    webc_cookie_set_host((CookieInfo*)cookie, host);
}


/*****************************************************************************/
/**
 @memo Set the path of a cookie

 @doc Set the path of a cookie

 @param cookie Handle of the cookie
 @param path Pointer to the string containing the cookie's new path
 @see webc_CookieSetName, webc_CookieSetValue, webc_CookieSetHost
 */
/*****************************************************************************/

void webc_CookieSetPath(COOKIE_HANDLE cookie, const char *path)
{
    webc_cookie_set_path((CookieInfo*)cookie, path);
}


#endif // (WEBC_SUPPORT_COOKIES)

#if (WEBC_SUPPORT_HTTPS)

class SetAuthHandler : public HTMLEventHandler
{

public:
	virtual HTMLEventStatus Event(HTMLElement *pElement, HTMLEvent *pEvent, WEBC_CHAR *pParam = 0)
	{
		if ((pEvent->type == HTML_EVENT_CLICK) ||
		    (pEvent->type == HTML_EVENT_KEYDOWN && pEvent->data.key == WGK_CR))
		{
			HTMLDocument *pDoc;
			HTMLBrowser *pBrowser;
			HTMLInputHidden *pUrlInput;
			HTMLInputHidden *pRealmInput;
			HTMLEditString *pUserInput;
			HTMLInputPassword *pPassInput;

			pDoc = pElement->GetDocument();
			if (pDoc)
			{
				HTMLNameTypeFinder f1((WEBC_CHAR *) WEBC_STR_URL,  HTML_HIDDEN_INPUT_ELEMENT);
				HTMLNameTypeFinder f2((WEBC_CHAR *) WEBC_STR_USER, HTML_EDIT_STR_ELEMENT);
				HTMLNameTypeFinder f3((WEBC_CHAR *) WEBC_STR_PASS, HTML_INPUT_PASSWORD_ELEMENT);
				HTMLNameTypeFinder f4((WEBC_CHAR *) WEBC_STR_REALM,  HTML_HIDDEN_INPUT_ELEMENT);

				pUrlInput   = (HTMLInputHidden *)   pDoc->FindElement(&f1);
				pUserInput  = (HTMLEditString *)    pDoc->FindElement(&f2);
				pPassInput  = (HTMLInputPassword *) pDoc->FindElement(&f3);
				pRealmInput = (HTMLInputHidden *)   pDoc->FindElement(&f4);

				if (pUrlInput && pUserInput && pPassInput && pRealmInput)
				{
					pBrowser = pDoc->GetBrowser();
					if (pBrowser)
					{
						WEBC_CHAR * url = pUrlInput->Value();
						URLDescriptor urlDesc;
						if (urlDesc.parseURL(url) >= 0)
						{
							long ttl = 0;

							WEBC_CHAR ttlEnable[] = {'t','t','l','E','n','a','b','l','e',0};
							HTMLNameTypeFinder f5(ttlEnable,  HTML_CHECKBOX_ELEMENT);
							HTMLCheckbox *pCheck = (HTMLCheckbox *) pDoc->FindElement(&f5);
							if (pCheck)
							{
								WEBC_CHAR ttlMinutes[] = {'t','t','l','M','i','n','u','t','e','s',0};
								HTMLNameTypeFinder f6(ttlMinutes,  HTML_EDIT_STR_ELEMENT);
								HTMLEditString *pMin = (HTMLEditString *) pDoc->FindElement(&f6);
								if (pMin && pMin->Value())
								{
									long minutes;
									HTML_ParseDecNumber(pMin->Value(), 12, &minutes);
									ttl = minutes * 60;
								}
							}

							char *host      = webc_arg_to_char(urlDesc.getHost());
							char *path      = webc_arg_to_char(urlDesc.getPath());
							char *realmName = webc_arg_to_char(pRealmInput->Value());
							char *userName  = webc_arg_to_char(pUserInput->Value());
							char *password  = webc_arg_to_char(pPassInput->Value());

							http_set_authorization(host, urlDesc.getPort(), path, realmName, HTTP_AUTHENTICATION_BASIC,
							                       userName, password, ttl);

							webc_arg_done(password);
							webc_arg_done(userName);
							webc_arg_done(realmName);
							webc_arg_done(path);
							webc_arg_done(host);

							pBrowser->LoadUrl(url);
						}
					}
				}
			}

			return (HTML_EVENT_STATUS_DONE);
		}

		return (HTML_EVENT_STATUS_CONTINUE);
	}

	virtual WEBC_CHAR *Name(void) { return (WEBC_CHAR *) WEBC_STR_SETAUTH; }
};

#endif // WEBC_SUPPORT_HTTPS


/*****************************************************************************/
/**
 @memo Set Authorization for HTTP site

 @doc This method is used to set a username and password to access a protected
      HTTP server

 @param domain     pointer to a struct decribing the authentication domain
 @param userName   user name to set for this domain (string will be copied)
 @param password   password to set for this domain (string will be copied)
 @param ttlSec     time to live in seconds (0 for infinite) for user/password
 @see HTTPAuthDomain, webc_HttpClearAuthorization, webc_HttpSetAuthCallback,
      webc_HttpClearAllAuthorizations
 */
/*****************************************************************************/

int webc_HttpSetAuthorization (
		HTTPAuthDomain* domain,
		const char* userName,
		const char* password,
		long ttlSec
	)
{
  #if (WEBC_SUPPORT_HTTP)
	HTTPAuthenticationScheme scheme = HTTP_AUTHENTICATION_UNKNOWN;

	if (!tc_stricmp(domain->scheme, "Basic"))
	{
		scheme = HTTP_AUTHENTICATION_BASIC;
	}
	else if (!tc_stricmp(domain->scheme, "Digest"))
	{
		scheme = HTTP_AUTHENTICATION_DIGEST;
	}

	URLDescriptor urlDesc(domain->url);

	char* path = webc_arg_to_char(urlDesc.getPath());
	char* host = webc_arg_to_char(urlDesc.getHost());

	int result = http_set_authorization (
			host,
			urlDesc.getPort(),
			path,
			domain->realm,
			scheme,
			(char*) userName,
			(char*) password,
			ttlSec
		);

	webc_arg_done(host);
	webc_arg_done(path);

	return result;
  #else
	return (-1);
  #endif // WEBC_SUPPORT_HTTP
}


/*****************************************************************************/
/**
 @memo Discard username/password for an HTTP host

 @doc This method is used to discard the current username and password for a
      certain HTTP authentication domain

 @param domain     pointer to a struct decribing the authentication domain
 @see HTTPAuthDomain, webc_HttpSetAuthorization, webc_HttpSetAuthCallback,
      webc_HttpClearAllAuthorizations
 */
/*****************************************************************************/

void webc_HttpClearAuthorization (HTTPAuthDomain *domain)
{
  #if (WEBC_SUPPORT_HTTP)
	//http_reset_authorization_realm ((char *) domain->host, domain->port,
	 //(char *) domain->realm);
  #endif // WEBC_SUPPORT_HTTP
}


/*****************************************************************************/
/**
 @memo Discard all HTTP username/passwords

 @doc This method is used to discard all usernames and passwords on all HTTP
      authentication domains

 @see webc_HttpSetAuthorization, webc_HttpSetAuthCallback,
      webc_HttpClearAuthorization
 */
/*****************************************************************************/

void webc_HttpClearAllAuthorizations (void)
{
  #if (WEBC_SUPPORT_HTTP)
	http_reset_all_authorizations();
  #endif // WEBC_SUPPORT_HTTP
}
// HEREHERE End websdkmisc.cpp
#endif // HEREHERE