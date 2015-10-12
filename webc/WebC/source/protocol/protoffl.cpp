//
// PROTOFFL.CPP - Routines to load URL using OFFLINE protocol
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

#include "webload.hpp"
#include "wcache.h"
#include "protoffl.h"
#include "webcstr.h"
#include "webcmem.h"


/*****************************************************************************/
// Function Bodies
/*****************************************************************************/

#if (WEBC_SUPPORT_OFFLINE_BROWSING)

int wload_OfflineGet(UrlStreamCtx *pStream)
{
	struct wcache_ctx *cc = wload_GetOfflineContentCache();

	if (wcache_open(cc, WEBC_OFFLINE_DIR_KEY, &pStream->stream.offline.dirFile, WEBC_CO_READ) < 0)
	{
		return (-1);
	}

	pStream->stream.offline.count = 0;
	
	pStream->stream.offline.buffer = (char *) WEBC_MALLOC(512);
	if (pStream->stream.offline.buffer)
	{
		tc_strcpy(pStream->stream.offline.buffer, "<html><body><table width=100% border=0 cellpadding=2 cellspacing=0>");

		if (webc_c_stristr(pStream->p_desc->getPath(), "html"))
		{
			pStream->stream.offline.mime_filter = HTML_MIME_TYPE_HTML;
		}
		else if (webc_c_stristr(pStream->p_desc->getPath(), "gif"))
		{
			pStream->stream.offline.mime_filter = HTML_MIME_TYPE_GIF;
		}
		else if (webc_c_stristr(pStream->p_desc->getPath(), "png"))
		{
			pStream->stream.offline.mime_filter = HTML_MIME_TYPE_PNG;
		}
		else if (webc_c_stristr(pStream->p_desc->getPath(), "jpeg"))
		{
			pStream->stream.offline.mime_filter = HTML_MIME_TYPE_JPEG;
		}
		else if (webc_c_stristr(pStream->p_desc->getPath(), "text"))
		{
			pStream->stream.offline.mime_filter = HTML_MIME_TYPE_PLAIN_TEXT;
		}
		else if (webc_c_stristr(pStream->p_desc->getPath(), "javascript"))
		{
			pStream->stream.offline.mime_filter = HTML_MIME_TYPE_JSCRIPT;
		}
		else if (webc_c_stristr(pStream->p_desc->getPath(), "css"))
		{
			pStream->stream.offline.mime_filter = HTML_MIME_TYPE_CSS;
		}
 		else
		{
			pStream->stream.offline.mime_filter = HTML_MIME_TYPE_UNKNOWN;
		}
	
		pStream->stream.offline.done = 0;
		pStream->stream.offline.unread_start = 0;
		pStream->stream.offline.unread_size = tc_strlen(pStream->stream.offline.buffer);
		pStream->data_type = HTML_MIME_TYPE_HTML;
		
		return (0);
	}
		
	return (-1);
}

int wload_OfflinePost(UrlStreamCtx *pStream, WEBC_CHAR * pContentType, long lContentLength)
{
	return (-1);
}

long wload_OfflineRead(UrlStreamCtx *pStream, char * buffer, long length)
{
long bytes_read = 0;
char *src = pStream->stream.offline.buffer;

	if (pStream->stream.offline.buffer)
	{
		long to_copy;
		struct wcache_ctx *cc = wload_GetOfflineContentCache();

		while (bytes_read + 768 < length)
		{
			/* first copy what we have in the buffer */
			if (pStream->stream.offline.unread_size > 0)
			{
				to_copy = EBSMIN(pStream->stream.offline.unread_size, length);
				tc_movebytes(&buffer[bytes_read], &src[pStream->stream.offline.unread_start], to_copy);				             
				bytes_read += to_copy;
				pStream->stream.offline.unread_size -= to_copy;
			}

			/* put more stuff in the buffer to be read */
			if (bytes_read + 768 < length)
			{
				int foundEntry = 0;
				struct WebcOfflineCacheDirEntry entry;
				
				if (pStream->stream.offline.done)
				{
					break;
				}
				
				src[0] = '\0';

				// Enumerate through the offline cache entries until we find one of the MIME type
				// we're looking for
				if (webc_OfflineReadDirEntry(&pStream->stream.offline.dirFile, &entry) >= 0)
				{					
					pStream->stream.offline.count++;
					foundEntry = 1;
				}
				
				/*
				while (wcache_next_entry(cc, &pStream->stream.offline.entry) >= 0)
				{
					if ((pStream->stream.offline.mime_filter == HTML_MIME_TYPE_UNKNOWN) || 
					    (pStream->stream.offline.mime_filter == pStream->stream.offline.entry.dataType))
					{
						foundEntry = 1;
						break;
					}
				}
				*/

				if (foundEntry)
				{
					if (pStream->stream.offline.count & 1)
					{
						tc_strcpy(src, "<tr valign=top>");
					}
					else
					{
						tc_strcpy(src, "<tr valign=top bgcolor=#eee>");
					}
					
					tc_strcat(src, "<td width=1><img width=20 height=20 src=\'webc://document.png\'></td>");

					if (entry.title[0])
					{
						tc_strcat(src, "<td><a href=\'");
						tc_strcat(src, entry.key);
						tc_strcat(src, "\'>");
						tc_strcat(src, entry.title);
						tc_strcat(src, "</a></td></tr>");
						/*
						tc_strcat(src, "</a></td><td>");
						tc_strcat(src, entry.key);
						tc_strcat(src, "</td></tr>");
						*/
					}
					else
					{
						tc_strcat(src, "<td><a href=\'");
						tc_strcat(src, entry.key);
						tc_strcat(src, "\'>");
						tc_strcat(src, entry.key);
						tc_strcat(src, "</a></td></tr>");
					}
				}
				else
				{
					wcache_close(&pStream->stream.offline.dirFile);
					tc_strcpy(src, "</table></body></html>");
					pStream->stream.offline.done = 1;
				}

				pStream->stream.offline.unread_start = 0;
				pStream->stream.offline.unread_size = tc_strlen(src);										
			}
		}

		return (bytes_read);
	}

	return (-1);
}

long wload_OfflineWrite(UrlStreamCtx *pStream, char * buffer, long length)
{
	return (-1);
}

WebcFileContentType wload_OfflineDataType(UrlStreamCtx *pStream)
{
	return (pStream->data_type);
}

int wload_OfflineClose(UrlStreamCtx *pStream)
{
	if (pStream->stream.offline.buffer)
	{
		WEBC_FREE(pStream->stream.offline.buffer);
	}

	return (0);
}

enum WebcCharset wload_OfflineCharset(UrlStreamCtx *pStream)
{
	return (WEBC_CHARSET_UTF8);
}

#endif // WEBC_SUPPORT_OFFLINE_BROWSING
