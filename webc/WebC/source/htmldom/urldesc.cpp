//
// URLDESC.CPP - Routines for parsing urls
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

#include "urldesc.hpp"
#include "webload.hpp"
#include "webcassert.h"
#include "webcmem.h"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define WEBC_DEFAULT_PROTOCOL    WEBC_PROTOCOL_HTTP

/*****************************************************************************/
// URLDescriptor static data
/*****************************************************************************/

WEBC_CHAR URLDescriptor::mNullStr[1] = {0};
const WEBC_CHAR WEBC_DEFAULT_USERNAME[] = {'a','n','o','n','y','m','o','u','s',0};
const WEBC_CHAR WEBC_DEFAULT_HOSTNAME[] = {'l','o','c','a','l','h','o','s','t',0};


/*****************************************************************************/
// URLDescriptor methods
/*****************************************************************************/

URLDescriptor::URLDescriptor (URLDescriptor *proto)
{
	miProtocol = WEBC_PROTOCOL_NONE;
	miPort = 0;
	mpUser = mNullStr;
	mpPassword = mNullStr;
	mpHost = mNullStr;
	mpPath = mNullStr;
	mpAnchor = mNullStr;

#if(WEBC_SUPPORT_REFERER_HEADER)
	referer_url=mNullStr;
#endif

	set(proto);	
}

#if (WEBC_SUPPORT_UNICODE)
URLDescriptor::URLDescriptor (const WEBC_CHAR8* _urlStr)
{
	miProtocol = WEBC_PROTOCOL_NONE;
	miPort = 0;
	mpUser = mNullStr;
	mpPassword = mNullStr;
	mpHost = mNullStr;
	mpPath = mNullStr;
	mpAnchor = mNullStr;

	if (_urlStr)
	{
		WEBC_CHAR str[384];	
		webc_c_strncpy(str, _urlStr, 383);
		str[383] = 0;
		parseURL(str);	
	}
#if(WEBC_SUPPORT_REFERER_HEADER)
	referer_url=mNullStr;
#endif

}
#endif 	

URLDescriptor::URLDescriptor (const WEBC_CHAR *urlStr, URLDescriptor *baseUrl)
{
	miProtocol = WEBC_PROTOCOL_NONE;
	miPort = 0;
	mpUser = mNullStr;
	mpPassword = mNullStr;
	mpHost = mNullStr;
	mpPath = mNullStr;
	mpAnchor = mNullStr;

	if (urlStr)
	{
		parseURL(urlStr, baseUrl);
	}
#if(WEBC_SUPPORT_REFERER_HEADER)
	referer_url=mNullStr;
#endif

}

URLDescriptor::~URLDescriptor (void)
{
	setUser(0);
	setPassword(0);
	setHost(0);
	setPath(0);
	setScript(0);
	setAnchor(0);
#if(WEBC_SUPPORT_REFERER_HEADER)
	setReferer(0);
#endif
}

int URLDescriptor::getProtocol (void)
{
	return (miProtocol);
}

int URLDescriptor::getPort (void)
{
	return (miPort);
}

WEBC_CHAR *URLDescriptor::getUser (void)
{
	return (mpUser);
}

WEBC_CHAR *URLDescriptor::getPassword (void)
{
	return (mNullStr);
}

WEBC_CHAR *URLDescriptor::getHost (void)
{
	return (mpHost);
}

WEBC_CHAR *URLDescriptor::getPath (void)
{
	return (mpPath);
}

WEBC_CHAR *URLDescriptor::getScript (void)
{
	return (mpPath);
}

WEBC_CHAR *URLDescriptor::getAnchor (void)
{
	return (mpAnchor);
}

int URLDescriptor::parseURL (const WEBC_CHAR *urlStr, URLDescriptor *baseUrl)
{
  #if (WEBC_SUPPORT_URL_MACROS)
	WEBC_CHAR urlBuf[1024];
  #endif // WEBC_SUPPORT_URL_MACROS

	WEBC_ASSERT(urlStr);

	if (!urlStr || !urlStr[0])
	{
		return (-1);
	}

	setUser(0);
	setPassword(0);
	setHost(0);
	setPath(0);
	setScript(0);
	setAnchor(0);

  #if (WEBC_SUPPORT_URL_MACROS)
	webc_replace_macros(urlBuf, urlStr, 1023);
	urlBuf[1023] = 0;
	urlStr = urlBuf;
  #endif // WEBC_SUPPORT_URL_MACROS
	
	return (parseProtocol(urlStr, baseUrl));
}

/*************************************************************************
 parseProtocol() - parse protocol

 Returns: 0 upon success or -1 upon failure
*************************************************************************/

int URLDescriptor::parseProtocol (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl)
{		
	WEBC_CHAR * firstColon;

	miProtocol = WEBC_PROTOCOL_NONE;

	while (*urlStr == ' ')
	{
		urlStr++;
	}
	
	firstColon = (WEBC_CHAR *) webc_strchr(urlStr,':');	

	if (firstColon)
	{
		int protLen, n, l;

		protLen = firstColon - urlStr;		
		for(n = 0; n < NUM_PROTOCOLS; n++)
		{
			if (gProtocolTable[n].name)
			{
				l = tc_strlen(gProtocolTable[n].name);
				if (l == protLen)
				{
					if (webc_c_strnicmp(urlStr, gProtocolTable[n].name, protLen) == 0)
					{
						setProtocol(n);
						urlStr = firstColon + 1;
						break;
					}
				}
			}
		}
	}

	if (getProtocol() == WEBC_PROTOCOL_NONE)
	{
		if (!baseUrl || baseUrl->getProtocol() == WEBC_PROTOCOL_NONE)
		{
			setProtocol(WEBC_DEFAULT_PROTOCOL);
		}
		else
		{
			setProtocol(baseUrl->getProtocol());
		}
	}
	
	if (baseUrl && getProtocol() != baseUrl->getProtocol())
	{
		baseUrl = 0;
	}

	switch (getProtocol())
	{
		case WEBC_PROTOCOL_HTTP:
		case WEBC_PROTOCOL_HTTPS:
			return (parseHost(urlStr, baseUrl));
			
		case WEBC_PROTOCOL_OFFLINE:
		case WEBC_PROTOCOL_WEBC:
		case WEBC_PROTOCOL_FILE:
		case WEBC_PROTOCOL_LOCAL:
			if ((urlStr[0] == '/') && (urlStr[1] == '/'))
			{
				urlStr += 2;
			}
			setHost(0);
			setUser(0);
			setPassword(0);
			return (parsePath(urlStr, baseUrl));
			
		case WEBC_PROTOCOL_FTP:
		case WEBC_PROTOCOL_TELNET:
			return (parseName(urlStr, baseUrl));

		case WEBC_PROTOCOL_JSCRIPT:
			return (parseScript(urlStr, baseUrl));
	}

	return (-1);
}

/*************************************************************************
 parseName() - parse username

 Returns: 0 upon success or -1 upon failure
*************************************************************************/

int URLDescriptor::parseName (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl)
{
	if ((urlStr[0] == '/') && (urlStr[1] == '/'))
	{
		size_t nameLen;
		
		urlStr += 2;
		nameLen = webc_c_strcspn(urlStr, "@/");
		setUser(urlStr, nameLen);
		urlStr += nameLen;
	}
	else
	{
		if (baseUrl && *(baseUrl->getUser()))
		{
			setUser(baseUrl->getUser());
		}
		else
		{
			setUser(WEBC_DEFAULT_USERNAME);
		}
	}
	
	return (parseHost(urlStr, baseUrl));
}

/*************************************************************************
 parseHost() - parse hostname

 Returns: 0 upon success or -1 upon failure
*************************************************************************/

int URLDescriptor::parseHost (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl)
{
	if (urlStr[0] == '\0')
	{
		return (0);
	}

	if (((urlStr[0] == '/') && (urlStr[1] == '/')) || (urlStr[0] == '@'))
	{
		size_t hostLen;
		
		urlStr += (urlStr[0] == '@')? 1 : 2;
		hostLen = webc_c_strcspn(urlStr, "/:?#");
		if (hostLen > 0)
		{
			setHost(urlStr, hostLen);
			urlStr += hostLen;

			if (baseUrl && webc_strcmp(getHost(), baseUrl->getHost()))
			{	
				baseUrl = 0;
			}
		}
	}
	else
	{
		if (baseUrl && *(baseUrl->getHost()))
		{
			setHost(baseUrl->getHost());
			setPort(baseUrl->getPort());
		}
		else
		{
			setHost(WEBC_DEFAULT_HOSTNAME);
			if (getProtocol() != WEBC_PROTOCOL_NONE)
			{
				setPort(gProtocolTable[getProtocol()].port);
			}
		}		
		
		return (parsePath(urlStr, baseUrl));
	}

	return (parsePort(urlStr, baseUrl));
}


/*************************************************************************
 parsePort() - parse port for network connection

 Returns: 0 upon success or -1 upon failure
*************************************************************************/

int URLDescriptor::parsePort (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl)
{
	if (urlStr[0] == ':')
	{
		size_t portLen;
		
		urlStr++;
		portLen = webc_c_strcspn(urlStr, "/");
		if (portLen > 0)
		{
			setPort(webc_atoi(urlStr));
			urlStr += portLen;
		}
	}
	else
	{
		if (getProtocol() != WEBC_PROTOCOL_NONE)
		{
			setPort(gProtocolTable[getProtocol()].port);
		}
	}

	return (parsePath(urlStr, baseUrl));
}


/*************************************************************************
 parsePassword() - parse password

 Returns: 0 upon success or -1 upon failure
*************************************************************************/

int URLDescriptor::parsePassword (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl)
{
	// if starts with ':', find first '@' or '/'
	//    desc->password = char after ':' to char before '@' or '/'
	//    urlStr = first '@' or '/'
	// else
	//    set password to default
	// return parse_host

	return (0);	
}


/*************************************************************************
 parsePath() - parse path

 Returns: 0 upon success or -1 upon failure
*************************************************************************/

int URLDescriptor::parsePath (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl)
{
	static WEBC_CHAR rootPath[2] = {'/',0};
	size_t pathLen;
	
	if (urlStr[0] == '\0')
	{
		setPath(rootPath, 1);
		return (0);
	}

	if ((getProtocol() == WEBC_PROTOCOL_TELNET) || 
	    (getProtocol() == WEBC_PROTOCOL_FTP))
	{
		pathLen = webc_strlen(urlStr);
	}
	else
	{
		pathLen = webc_c_strcspn(urlStr, "#");
	}

	// detect absolute path; begins with '/', '\' or a drive letter
	if ((urlStr[0] == '.')||(urlStr[0] == '/') || (urlStr[0] == '\\') ||
	    (IS_ALPHA(urlStr[0]) && (urlStr[1] == ':')) // check for drive letter
	   )
	{		              
		// absolute path
		if (pathLen > 0)
		{
			setPath(urlStr, pathLen);
			urlStr += pathLen;
		}	
	}
	else if (urlStr[0] == '#')
	{
		if (baseUrl)
		{
			setPath(baseUrl->getPath());
		}
	}
	else
	{
		// relative path
		if (baseUrl)
		{
			if (pathLen > 0)
			{
				setPathRel(baseUrl->getPath(), urlStr, pathLen);
				urlStr += pathLen;
			}
			else
			{
				setPath(baseUrl->getPath());
			}
		}
		else
		{
			if (getProtocol() == WEBC_PROTOCOL_HTTP)
			{
				if (pathLen > 0)
				{
					setPathRel(rootPath, urlStr, pathLen);
					urlStr += pathLen;
				}
			}
			else
			{
				if (pathLen > 0)
				{
					setPath(urlStr, pathLen);
					urlStr += pathLen;
				}
			}
		}
	}

	return (parseAnchor(urlStr, baseUrl));
}


/*************************************************************************
 parseAnchor() - parse anchor

 Returns: 0 upon success or -1 upon failure
*************************************************************************/

int URLDescriptor::parseAnchor (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl)
{
	if (urlStr[0] == '#')
	{
		urlStr++;
		setAnchor(urlStr);
	}
	
	return (0);
}


/*************************************************************************
 parseScript() - parse script source

 Returns: 0 upon success or -1 upon failure
*************************************************************************/

int URLDescriptor::parseScript (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl)
{
	if ((urlStr[0] == '/') && (urlStr[1] == '/'))
	{
		urlStr += 2;
	}
	
	if (urlStr[0] != 0)
	{
		setScript(urlStr);
	}

	return (0);
}


void URLDescriptor::setProtocol (int protocol)
{
	miProtocol = protocol;
}

void URLDescriptor::setPort (int port)
{
	miPort = port;
}

void URLDescriptor::setUser (const WEBC_CHAR *user)
{
	setUser(user, user? webc_strlen(user) : 0);
}

void URLDescriptor::setPassword (const WEBC_CHAR *password)
{
	setPassword(password, password? webc_strlen(password) : 0);
}

void URLDescriptor::setHost (const WEBC_CHAR *host)
{
	setHost(host, host? webc_strlen(host) : 0);
}

void URLDescriptor::setPath (const WEBC_CHAR *path)
{
	setPath(path, path? webc_strlen(path) : 0);
}

void URLDescriptor::setScript (const WEBC_CHAR *script)
{
	setScript(script, script? webc_strlen(script) : 0);
}

void URLDescriptor::setAnchor (const WEBC_CHAR *anchor)
{
	setAnchor(anchor, anchor? webc_strlen(anchor) : 0);
}

void URLDescriptor::setStr (WEBC_CHAR **strPtr, const WEBC_CHAR *value, long len)
{
	if (*strPtr == value)
	{
		return;
	}

	if (*strPtr != mNullStr)
	{
		WEBC_FREE(*strPtr);
	}

	if (value)
	{
		*strPtr = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR) * (len + 1));
		if (*strPtr)
		{
			tc_movebytes((WEBC_PFBYTE) (*strPtr), (WEBC_PFBYTE) value, sizeof(WEBC_CHAR) * len);
			(*strPtr)[len] = 0;
		}
		else
		{
			*strPtr = mNullStr;
		}
	}
	else
	{
		*strPtr = mNullStr;
	}
}

void URLDescriptor::setUser (const WEBC_CHAR *user, long len)
{
	setStr(&mpUser, user, len);
}

void URLDescriptor::setPassword (const WEBC_CHAR *password, long len)
{
	setStr(&mpPassword, password, len);
}

void URLDescriptor::setHost (const WEBC_CHAR *host, long len)
{
	setStr(&mpHost, host, len);
}

void URLDescriptor::setPath (const WEBC_CHAR *path, long len)
{
	setStr(&mpPath, path, len);
	
	if (mpPath != mNullStr)
	{
		collapsePath(mpPath);
	}
}

void URLDescriptor::setScript (const WEBC_CHAR *script, long len)
{
	setStr(&mpPath, script, len);
}

void URLDescriptor::setAnchor (const WEBC_CHAR *anchor, long len)
{
	setStr(&mpAnchor, anchor, len);
}


void URLDescriptor::setPathRel(const WEBC_CHAR *basePath, const WEBC_CHAR *relPath, long relLen)
{
	long n;
	long basePathLen = 0;

	setPath(0);
	
	// find the last occurrance of '/' (before first occurrance of '?')
	for (n=0; basePath[n]; n++)
	{
		if ((basePath[n] == '/') || (basePath[n] == '\\'))
		{
			basePathLen = n+1;
		}
		else if (((getProtocol() == WEBC_PROTOCOL_HTTP) || 
		          (getProtocol() == WEBC_PROTOCOL_HTTPS)) && 
		         (basePath[n] == '?'))
		{
			if (relPath[0] == '?')
			{
				basePathLen = n;
			}
			break;
		}
	}
	
	mpPath = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR) * (basePathLen + relLen + 1));
	if (mpPath)
	{
		if (basePathLen > 0)
		{
			webc_strncpy(mpPath, basePath, basePathLen);
		}
		webc_strncpy(mpPath + basePathLen, relPath, relLen);
		mpPath[basePathLen + relLen] = 0;
	}
	else
	{
		mpPath = mNullStr;
	}

	if (mpPath != mNullStr)
	{
		collapsePath(mpPath);
	}	
}

void URLDescriptor::setQuery (const WEBC_CHAR *queryStr)
{
long queryLen = webc_strlen(queryStr);
	
	if (mpPath == mNullStr)
	{
		mpPath = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR) * (queryLen + 2));
		if (!mpPath)
		{
			mpPath = mNullStr;
			return;
		}
		mpPath[0] = '?';
		webc_strcpy(&mpPath[1], queryStr);
	}
	else
	{
		long pathLen = 0;
		
		while (mpPath[pathLen] && mpPath[pathLen] != '?')
		{
			pathLen++;
		}
		
		WEBC_CHAR *newPath = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR) * (queryLen + pathLen + 2));
		if (!newPath)
		{
			return;
		}
		tc_movebytes((WEBC_PFBYTE) newPath, (WEBC_PFBYTE) mpPath, sizeof(WEBC_CHAR) * pathLen);
		newPath[pathLen] = '?';
		webc_strcpy(&newPath[pathLen + 1], queryStr);
		WEBC_FREE(mpPath);
		mpPath = newPath;
	}
}


void URLDescriptor::collapsePath (WEBC_CHAR *path)
{
	// compact ".." and "." directories in path
	if (path[0])
	{
		WEBC_CHAR * pos = path - 1;
		do
		{
			pos++;
			while ((pos[0] == '.') && ((pos[1] == '/') || (pos[1] == '\\')))
			{
				rtp_memmove((WEBC_PFBYTE)pos, (WEBC_PFBYTE)(pos + 2), (webc_strlen(pos + 2) + 1)*sizeof(WEBC_CHAR));
			}
		} while ((pos = webc_c_strchrs(pos,"/\\")) != 0);  

		pos = path - 1;
		do
		{
			pos++;
			while ((pos[0] == '.') && (pos[1] == '.') && ((pos[2] == '/') || (pos[2] == '\\')))
			{
				WEBC_CHAR * nextdir, * prevdir;

				if (pos == path)
				{
					break;
				}

				nextdir = pos + 2;
				prevdir = pos - 2;
				if (prevdir < path)
				{
					prevdir = path;
				}
				else
				{
					while ((prevdir > path) && (prevdir[0] != '/') && (prevdir[0] != '\\'))
					{
						prevdir--;
					}
				}
				rtp_memmove((WEBC_PFBYTE)prevdir, (WEBC_PFBYTE)nextdir, (webc_strlen(nextdir) + 1)*sizeof(WEBC_CHAR));
				pos = prevdir;
			}
		} while ((pos = webc_c_strchrs(pos,"/\\")) != 0);
	}
	
}

void URLDescriptor::set (URLDescriptor *proto)
{
	setProtocol(proto->getProtocol());
	
	switch (getProtocol())
	{
		case WEBC_PROTOCOL_HTTP:
		case WEBC_PROTOCOL_HTTPS:
			setHost(proto->getHost());
			setPort(proto->getPort());
			setPath(proto->getPath());
			setAnchor(proto->getAnchor());
#if (WEBC_SUPPORT_REFERER_HEADER)
			setReferer(proto->referer_url);
#endif
			break;
			
		case WEBC_PROTOCOL_OFFLINE:
		case WEBC_PROTOCOL_WEBC:
		case WEBC_PROTOCOL_FILE:
			setPath(proto->getPath());
			setAnchor(proto->getAnchor());
			break;
		case WEBC_PROTOCOL_LOCAL:
			setPath(proto->getPath());
			setAnchor(proto->getAnchor());
			break;
			
		case WEBC_PROTOCOL_FTP:
		case WEBC_PROTOCOL_TELNET:
			setUser(proto->getUser());
			setPassword(proto->getPassword());
			setHost(proto->getHost());
			setPort(proto->getPort());
			setPath(proto->getPath());
			setAnchor(proto->getAnchor());
			break;

		case WEBC_PROTOCOL_JSCRIPT:
			setScript(proto->getScript());
			break;
	}
}

int URLDescriptor::samePage (URLDescriptor *other)
{
	if (getProtocol() != other->getProtocol())
	{
		return(0);
	}
	if (webc_stricmp(getHost(), other->getHost()))
	{
		return(0);
	}
	if (getPort() != other->getPort())
	{
		return(0);
	}
	if (webc_strcmp(getPath(), other->getPath()))
	{
		return(0);
	}

	return(1);
}

/*************************************************************************
 webc_is_type() - Test if URL is a type of file (.GIF,.JPG/.JPEG, .HTM/.HTML)

 desc - pointer to URL descriptor to test for .GIF-iness. etc

 This function determines if the 'path' of a URL descriptor ends in st1
 or str2 (for example .JPG or .JPEG)

 Returns: 1 if URL is of specified type, 0 otherwise.
*************************************************************************/

int URLDescriptor::hasExtension(const char *ext)
{
	int len = 0;
	const WEBC_CHAR *pos;

	pos = webc_strchr(getPath(), '?');

	if (!pos)
	{
		pos = getPath() + webc_strlen(getPath());
	}
	
	while (pos >= getPath() && pos[0] != '.')
	{
		pos--;
		len++;
	}
	
	if (pos < getPath())
	{
		return (0);
	}

	pos++;
	len--;

	return (webc_c_strnicmp(pos, ext, len) == 0);
}

WEBC_CHAR * URLDescriptor::toString (void)
{
	UrlStringBuilder b(this);
	WEBC_CHAR *str;
	
	str = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR) * (b.GetStringLength() + 1));
	if (str)
	{
		b.BuildString(str);
	}
	
	return (str);
}

void URLDescriptor::freeString (WEBC_CHAR *str)
{
	WEBC_FREE(str);
}



#if(WEBC_SUPPORT_REFERER_HEADER)
void URLDescriptor::setReferer(const WEBC_CHAR *referer, long len)
{
	setStr(&referer_url, referer, len);
}

void URLDescriptor::setReferer(const WEBC_CHAR *referer)
{
	setReferer(referer, referer? webc_strlen(referer) : 0);
}


WEBC_CHAR *URLDescriptor::getReferer(void)
{
	return (referer_url);
}
#endif