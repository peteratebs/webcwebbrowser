//
// WEBC.C - Main web client module
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//    This module contains code to download, process, and display web pages,
//		as well as process browser messages, etc.
//

#include "webload.hpp"
#include "webc.h"
#include "webcfile.h"
#include "webcsock.h"
#include "html.h"
#include "webcfg.h"
#include "wcache.h"
#include "httpc.h"
#include "htmlbrow.hpp"
#include "strtable.hpp"
#include "webcstr.h"
#include "webcmem.h"

#if (WEBC_SUPPORT_FILE && WEBC_SUPPORT_CACHE)
 #include "stdio.h"
#endif

// TURN ON DEBUG INFORMATION PRINTED IN DEBUG WINDOW OR TO FILE
// NOTE: WEBC_SUPPORT_DEBUG_ERROR must also be set
#define DEBUG_GET_NEXT_UNIT 1
#define DEBUG_IMPLICIT_TAG  0
#define DEBUG_PUSH_POP_TAG  0
#define DEBUG_TAG_STATE     0

#define WEBC_DO_ESCAPE_STRINGS (!WEBC_SUPPORT_FORMAT)

#define WEBC_PAGE_INVALIDATED 255


// ***********************************************************************

StringTable *gpWebcStringTable = 0;
int giAllowTimerTick = 0;

#if (WEBC_SUPPORT_PIXEL_SCALING)
WEBC_UINT16 gwPixelMultiplier = WEBC_CFG_PIXEL_MUL;
WEBC_UINT16 gwPixelDivisor    = WEBC_CFG_PIXEL_DIV;
#endif

extern "C" int giEscapeDown;
int giEscapeDown = 0;

#if (WEBC_SUPPORT_OFFLINE_BROWSING)
int giWebcOnline = 1;
#endif // WEBC_SUPPORT_OFFLINE_BROWSING

#if (WEBC_SUPPORT_CACHE)

int  webcfile_cache_open   (void *ctx, const char *fileName, void **fileStream, int mode);
int  webcfile_cache_close  (void *ctx, void *fileStream);
int  webcfile_cache_flush  (void *ctx, void *fileStream);
long webcfile_cache_read   (void *ctx, void *fileStream, WEBC_PFBYTE buffer, long length);
long webcfile_cache_write  (void *ctx, void *fileStream, WEBC_PFBYTE buffer, long length);
int  webcfile_cache_remove (void *ctx, const char *fileName);

WEBC_READONLY struct wcache_system_spec gWebcFileCacheSpec =
{
	webcfile_cache_open,
	webcfile_cache_close,
	webcfile_cache_flush,
	webcfile_cache_read,
	webcfile_cache_write,
	webcfile_cache_remove
};

#if (WEBC_SUPPORT_OFFLINE_BROWSING)

int  webcram_cache_open   (void *ctx, const char *fileName, void **fileStream, int mode);
int  webcram_cache_close  (void *ctx, void *fileStream);
int  webcram_cache_flush  (void *ctx, void *fileStream);
long webcram_cache_read   (void *ctx, void *fileStream, WEBC_PFBYTE buffer, long length);
long webcram_cache_write  (void *ctx, void *fileStream, WEBC_PFBYTE buffer, long length);
int  webcram_cache_remove (void *ctx, const char *fileName);

WEBC_READONLY struct wcache_system_spec gWebcRamCacheSpec =
{
	webcram_cache_open,
	webcram_cache_close,
	webcram_cache_flush,
	webcram_cache_read,
	webcram_cache_write,
	webcram_cache_remove
};

#endif // WEBC_SUPPORT_OFFLINE_BROWSING

#endif // WEBC_SUPPORT_CACHE

/******************** FUNCTION BODIES ********************/

// utility routines

/*************************************************************************
 webc_escape_form_fields() - Escape out characters from form string.

 fields - source form string
 str - destination string, or 0 if none

 This routine converts a form string to escaped form as per RFC 1945.

 Returns: Length of escaped string.
*************************************************************************/
static char specialChars[] = {'@', /* <-- reserved */
	'$', '-', '_', '.', '!', '*', '(', ')', ',', '\0' /* <-- special */};

int webc_escape_form_fields(WEBC_CHAR * str, const WEBC_CHAR * fields)
{
int fpos,spos;

	if (!fields)
	{
		return(-1);
	}

	for (fpos=0,spos=0;fields[fpos];fpos++)
	{
		if (fields[fpos]==' ')
		{
			if(str)
			{
				str[spos]='+';
			}
			spos++;
		}
		else if((fields[fpos]>='0' && fields[fpos]<='9')||
				(fields[fpos]>='A' && fields[fpos]<='Z')||
				(fields[fpos]>='a' && fields[fpos]<='z')||
				 tc_strchr(specialChars, (char) fields[fpos]))
		{
			if (str)
			{
				str[spos]=fields[fpos];
			}
			spos++;
		}
		//if this is a new line or a carriage return then convert it to %0D%0A
		else if (fields[fpos]=='\r' || fields[fpos]=='\n')
		{
			if (str)
			{
				webc_c_strcpy(&(str[spos]),"%0D%0A");
			}
			spos+=6;
		}
		else
		{
			if (str)
			{
				str[spos] = '%';
				webc_print_hex(&(str[spos+1]), (long) fields[fpos], 2, 2, 1);
			}
			spos+=3;
		}
	}

	if (str)
	{
		str[spos]='\0';
	}

	return(spos);
}


/*************************************************************************
 webc_unescape_string() - Converts escaped characters in HTML document.

 str - source string from HTML document
 unstr - un-escaped string

 This routine converts a string from an HTML document into a string in which
 the escaped characters are removed.  For example, this routine would
 convert "&Ouml" to "™".

 Returns: -1 on error, 0 otherwise.
*************************************************************************/

long webc_escape_string(WEBC_CHAR * escstr, const char * str)
{
	long pos, epos, i;
	char * rep;

	for (pos = 0, epos= 0; str[pos]; pos++)
	{
		switch (str[pos])
		{
			case '\"':
				rep = "&quot;";
				break;

			case '&':
				rep = "&amp;";
				break;

			case '<':
				rep = "&lt;";
				break;

			case '>':
				rep = "&gt;";
				break;

			case (char) HTML_NBSP_CHAR:
				rep = "&nbsp;";
				break;

			case '\n':
				rep = "<br>";
				break;

			default:
				rep = 0;
				break;
		}

		if (rep)
		{
			if (escstr)
			{
				for (i=0; rep[i]; i++)
				{
					escstr[epos++] = (WEBC_CHAR) rep[i];
				}
				continue;
			}
			else
			{
				//Incase the target is NULL, add the length of the rep string
				//but - 1 for the char its replacing
				epos += tc_strlen(rep) - 1;
			}
		}
		else
		{
			if (escstr)
			{
				escstr[epos] = str[pos];
			}
		}

		epos++;
	}//end for

	if (escstr)
	{
		//don't forget to NULL terminate
		escstr[epos] = '\0';
	}

	return epos;
}

/*************************************************************************
 webc_get_form_parameter() - Search a query string for a given field

 value - buffer to write the value for the specified parameter
 field - the case-sensitive name of the parameter to retrieve
 path  - the complete query string (given as path of the path in a URL)

 This function will search path for the string "<field>=", then writes
 what follows (terminated either by '&' or '\0') to the value buffer,
 converting all URL escape characters.

 Returns: -1 on error, 0 otherwise.
*************************************************************************/

int webc_get_form_parameter(WEBC_CHAR * value, WEBC_CHAR * field, WEBC_CHAR * path)
{
int len;
WEBC_CHAR ch;
WEBC_CHAR * ptr;

	path = webc_strchr(path, '?');
	if (!path)
		return (-1);

	path++;
	len = webc_strlen(field);

	while (path)
	{
		path = webc_strstr(path, field);
		if (path)
		{
			path += len;
			if (*path == '=')
			{
				path++;
				ptr = webc_c_strchrs(path, "&\0");
				ch = *ptr;
				*ptr = 0;
				HTML_UnescapeString(path, value);
				*ptr = ch;
				return (0);
			}
		}
	}

	return (-1);
}


/*****************************************************************************/
// Thread Related functions
/*****************************************************************************/

long webc_strip_eol(WEBC_CHAR * dest, const WEBC_CHAR * src)
{
	long len = 0;

	while (*src)
	{
		if ((*src != '\r') && (*src != '\n'))
		{
			*dest = *src;
			dest++;
			len++;
		}
		src++;
	}

	*dest = '\0';
	return (len);
}

HTMLBrowser *_webc_NewBrowser(HTMLBrowserConfig *pConfig, WEBC_BOOL CreateWindow, WEBC_CHAR *pWindowName, WEBC_CHAR *pHomePage)
{
	HTMLBrowser *pBrowser = 0;
	WEBC_NEW_VERBOSE(pBrowser, HTMLBrowser(pConfig, CreateWindow, pWindowName, pHomePage),"HTMLBrowser");
	return (pBrowser);
}

#if (WEBC_SUPPORT_CACHE)

int webcfile_cache_open (void *ctx, const char *fileName, void **fileStream, int mode)
{
	switch (mode)
	{
	  #if (WEBC_SUPPORT_FILE)
		case WEBC_CO_READ:
			return (webc_fopen(fileName, (WEBC_FILE *) fileStream, WEBC_FILE_RDONLY));

		case WEBC_CO_CREATE:
			return (webc_fopen(fileName, (WEBC_FILE *) fileStream, WEBC_FILE_WRONLY));

		case WEBC_CO_APPEND:
			return (webc_fopen(fileName, (WEBC_FILE *) fileStream, WEBC_FILE_APPEND));
	  #endif

		default:
			return (-1);
	}
}

int webcfile_cache_close (void *ctx, void *fileStream)
{
  #if (WEBC_SUPPORT_FILE)
	return (webc_fclose((WEBC_FILE) fileStream));
  #else
	return (-1);
  #endif
}

int webcfile_cache_flush (void *ctx, void *fileStream)
{
	return (0);
}

long webcfile_cache_read (void *ctx, void *fileStream, WEBC_PFBYTE buffer, long length)
{
  #if (WEBC_SUPPORT_FILE)
	return (webc_fread((WEBC_FILE) fileStream, (char *) buffer, length, 0, 0));
  #else
	return (-1);
  #endif
}

long webcfile_cache_write (void *ctx, void *fileStream, WEBC_PFBYTE buffer, long length)
{
  #if (WEBC_SUPPORT_FILE)
	return (webc_fwrite((WEBC_FILE) fileStream, (char *) buffer, length, 0, 0));
  #else
	return (-1);
  #endif
}

int webcfile_cache_remove (void *ctx, const char *fileName)
{
  #if (WEBC_SUPPORT_FILE)
	rtp_file_delete(fileName);
  #endif
	return (0);
}

#endif

#if (WEBC_SUPPORT_OFFLINE_BROWSING)

int webcram_cache_open (void *ctx, const char *fileName, void **fileStream, int mode)
{
	SimpleRamDisk *disk = (SimpleRamDisk *) ctx;

	switch (mode)
	{
		case WEBC_CO_READ:
			*fileStream = disk->Open(fileName, SIMPLE_RAMDISK_STREAM_READONLY);
			break;

		case WEBC_CO_CREATE:
			*fileStream = disk->Open(fileName, SIMPLE_RAMDISK_STREAM_CREATE);
			break;

		case WEBC_CO_APPEND:
			*fileStream = disk->Open(fileName, SIMPLE_RAMDISK_STREAM_APPEND);
			break;

		default:
			*fileStream = 0;
	}

	if (*fileStream)
	{
		return (0);
	}

	return (-1);
}

int webcram_cache_close (void *ctx, void *fileStream)
{
	SimpleRamDiskStream *stream = (SimpleRamDiskStream *) fileStream;
	stream->Close();
	return (0);
}

int webcram_cache_flush (void *ctx, void *fileStream)
{
	return (0);
}

long webcram_cache_read (void *ctx, void *fileStream, WEBC_PFBYTE buffer, long length)
{
	SimpleRamDiskStream *stream = (SimpleRamDiskStream *) fileStream;
	long ret = stream->Read((char*) buffer, length);
	return (ret == ERR_RAMDISK_EOF)? 0 : ret;
}

long webcram_cache_write (void *ctx, void *fileStream, WEBC_PFBYTE buffer, long length)
{
	SimpleRamDiskStream *stream = (SimpleRamDiskStream *) fileStream;
	return (stream->Write((char *) buffer, length));
}

int webcram_cache_remove (void *ctx, const char *fileName)
{
	SimpleRamDisk *disk = (SimpleRamDisk *) ctx;
	if (disk->Remove(fileName))
	{
		return (0);
	}
	return (-1);
}

int webc_online(void)
{
	return (giWebcOnline);
}

#endif // WEBC_SUPPORT_OFFLINE_BROWSING

void webc_string_table_init(unsigned int size)
{
	if (!gpWebcStringTable)
	{
		WEBC_NEW_VERBOSE(gpWebcStringTable, StringTable(size),"gpWebcStringTable");
	}
}

void webc_string_table_exit(void)
{
	if (gpWebcStringTable)
	{
		WEBC_DELETE(gpWebcStringTable);
		gpWebcStringTable = 0;
	}
}

const WEBC_CHAR *webc_string_table_add(const WEBC_CHAR *str, const char *file, unsigned int line)
{
	return (gpWebcStringTable->GetString(str));
}

const WEBC_CHAR *webc_string_table_add_lowercase(const WEBC_CHAR *str, const char *file, unsigned int line)
{
	return (gpWebcStringTable->GetLowercaseString(str));
}

void webc_string_table_remove(const WEBC_CHAR *str, const char *file, unsigned int line)
{
	gpWebcStringTable->ReleaseString(str);
}

// END WEBC.C
