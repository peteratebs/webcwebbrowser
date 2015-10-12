#ifndef __URLDESC_HPP__
#define __URLDESC_HPP__

#include "webcfg.h"
#include "webcstr.h"


// values for protocol
#define WEBC_PROTOCOL_NONE       -1
#define WEBC_PROTOCOL_HTTP        0
#define WEBC_PROTOCOL_FTP         1 // not supported yet
#define WEBC_PROTOCOL_TELNET      2 // not supported yet
#define WEBC_PROTOCOL_FILE        3
#define WEBC_PROTOCOL_HTTPS       4
#define WEBC_PROTOCOL_WEBC        5 // internal client-side files
#define WEBC_PROTOCOL_WEBS        6 // direct access to server files... tbd - integrate server with client
#define WEBC_PROTOCOL_JSCRIPT     7
#define WEBC_PROTOCOL_CACHE       8 // not supported yet
#define WEBC_PROTOCOL_OFFLINE     9
#define WEBC_PROTOCOL_LOCAL       10 //local


class URLDescriptor
{
protected:
	int         miProtocol;
	int         miPort;

	WEBC_CHAR   *mpUser;
	WEBC_CHAR   *mpPassword;
	WEBC_CHAR   *mpHost;
	WEBC_CHAR   *mpPath;
	WEBC_CHAR   *mpAnchor;
#if(WEBC_SUPPORT_REFERER_HEADER)
	WEBC_CHAR	*referer_url;
#endif

	static WEBC_CHAR mNullStr[1];

	int parseProtocol (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl);
	int parseName     (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl);
	int parseHost     (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl);
	int parsePort     (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl);
	int parsePassword (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl);
	int parsePath     (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl);
	int parseAnchor   (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl);
	int parseScript   (const WEBC_CHAR * urlStr, URLDescriptor *baseUrl);

	void setStr (WEBC_CHAR **strPtr, const WEBC_CHAR *value, long len);

	void collapsePath (WEBC_CHAR *path);

public:

	URLDescriptor (URLDescriptor *proto);
  #if (WEBC_SUPPORT_UNICODE)
	URLDescriptor (const WEBC_CHAR8* urlStr);
  #endif
	URLDescriptor (const WEBC_CHAR *urlStr=0, URLDescriptor *baseUrl=0);
	~URLDescriptor (void);

	int       getProtocol (void);
	int       getPort     (void);
	WEBC_CHAR *getUser     (void);
	WEBC_CHAR *getPassword (void);
	WEBC_CHAR *getHost     (void);
	WEBC_CHAR *getPath     (void);
	WEBC_CHAR *getScript   (void);
	WEBC_CHAR *getAnchor   (void);

	void setProtocol (int protocol);
	void setPort     (int port);
	void setUser     (const WEBC_CHAR *user);
	void setPassword (const WEBC_CHAR *password);
	void setHost     (const WEBC_CHAR *host);
	void setPath     (const WEBC_CHAR *path);
	void setScript   (const WEBC_CHAR *script);
	void setAnchor   (const WEBC_CHAR *anchor);

	void setUser     (const WEBC_CHAR *user,     long len);
	void setPassword (const WEBC_CHAR *password, long len);
	void setHost     (const WEBC_CHAR *host,     long len);
	void setPath     (const WEBC_CHAR *path,     long len);
	void setScript   (const WEBC_CHAR *script,   long len);
	void setAnchor   (const WEBC_CHAR *anchor,   long len);

	void setPathRel  (const WEBC_CHAR *basePath, const WEBC_CHAR *relPath, long relLen);
	void setQuery    (const WEBC_CHAR *queryStr);

	int  parseURL (const WEBC_CHAR *urlStr, URLDescriptor *baseUrl = 0);

	void set (URLDescriptor *proto);

	int  samePage (URLDescriptor *other);
	int  hasExtension (const char *ext);

	WEBC_CHAR * toString   (void);
	void       freeString (WEBC_CHAR *str);
#if(WEBC_SUPPORT_REFERER_HEADER)
	void setReferer   (const WEBC_CHAR *referer,   long len);
	void setReferer   (const WEBC_CHAR *referer);
	WEBC_CHAR *getReferer   (void);
#endif
};

#endif // __URLDESC_HPP__
