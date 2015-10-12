//
// PROTLOCAL.CPP - Routines to load URL using FILE protocol
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
#include "protlocal.h"
#include "webcstr.h"
#include "fileext.h"


/*****************************************************************************/
// Function Bodies
/*****************************************************************************/
#if (WEBC_SUPPORT_LOCAL)
extern "C" int webc_localGet(char *path, void **phandle);
extern "C" int webc_localRead(void *phandle, char *buffer, long length);
extern "C" int webc_localWrite(void *phandle, char *buffer, long length);
extern "C" int webc_localClose(void *phandle);

int wload_LocalGet(UrlStreamCtx *pStream)
{
	int result = -1;
	char *path = webc_arg_to_char(pStream->p_desc->getPath());

	if (path)
	{
//		result = webc_fopen(path, &(pStream->stream.local), WEBC_FILE_RDONLY);
		result = webc_localGet(path, &(pStream->stream.local));

		if (result >= 0)
		{
			pStream->mode = URLSTREAM_OPEN_RDONLY;
			pStream->data_type = webc_GetFileTypeByExtension(path);
		}

		webc_arg_done(path);
	}

	return (result);
}

int wload_LocalPost(UrlStreamCtx *pStream, WEBC_CHAR * pContentType, long lContentLength)
{
	return (-1);
}

long wload_LocalRead(UrlStreamCtx *pStream, char * buffer, long length)
{
long bytes_read;

	//bytes_read = webc_fread(pStream->stream.local, buffer, length,
	//                           pStream->idle_func, pStream->idle_data);
	bytes_read = webc_localRead(pStream->stream.local, buffer, length);
//bytes_read = webc_fread(pStream->stream.file, buffer, length,pStream->stream.https.factory.idle_func, pStream->idle_data);
//pStream->stream.https.factory.idle_func
	return (bytes_read);
}

long wload_LocalWrite(UrlStreamCtx *pStream, char * buffer, long length)
{
	return(webc_localWrite(pStream->stream.local, buffer, length));
}

WebcFileContentType wload_LocalDataType(UrlStreamCtx *pStream)
{
	return (pStream->data_type);
}

int wload_LocalClose(UrlStreamCtx *pStream)
{
	webc_localClose(pStream->stream.local);
	return (0);
}

enum WebcCharset wload_LocalCharset(UrlStreamCtx *pStream)
{
	return (WEBC_CHARSET_UNKNOWN);
}

#endif // WEBC_SUPPORT_LOCAL
