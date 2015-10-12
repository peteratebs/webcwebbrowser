//
// PROTFILE.CPP - Routines to load URL using FILE protocol
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
#include "webcfile.h"
#include "protfile.h"
#include "webcstr.h"
#include "fileext.h"


/*****************************************************************************/
// Function Bodies
/*****************************************************************************/

#if (WEBC_SUPPORT_FILE)

int wload_FileGet(UrlStreamCtx *pStream)
{
	int result = -1;
	char *path = webc_arg_to_char(pStream->p_desc->getPath());

	if (path)
	{
		result = webc_fopen(path, &(pStream->stream.file), WEBC_FILE_RDONLY);
		if (result >= 0)
		{
			pStream->mode = URLSTREAM_OPEN_RDONLY;
			pStream->data_type = webc_GetFileTypeByExtension(path);
		}
        else
        {
           // printf("wload_FileGet() failed opening %s\n", path);
        }
		webc_arg_done(path);
	}

	return (result);
}

int wload_FilePost(UrlStreamCtx *pStream, WEBC_CHAR * pContentType, long lContentLength)
{
	return (-1);
}

long wload_FileRead(UrlStreamCtx *pStream, char * buffer, long length)
{
long bytes_read;

	bytes_read = webc_fread(pStream->stream.file, buffer, length,
	                           (webcIdleFnC)pStream->idle_func, pStream->idle_data);
//bytes_read = webc_fread(pStream->stream.file, buffer, length,pStream->stream.https.factory.idle_func, pStream->idle_data);
//pStream->stream.https.factory.idle_func
	return (bytes_read);
}

long wload_FileWrite(UrlStreamCtx *pStream, char * buffer, long length)
{
	return (-1);
}

WebcFileContentType wload_FileDataType(UrlStreamCtx *pStream)
{
	return (pStream->data_type);
}

int wload_FileClose(UrlStreamCtx *pStream)
{
	webc_fclose(pStream->stream.file);
	return (0);
}

enum WebcCharset wload_FileCharset(UrlStreamCtx *pStream)
{
	return (WEBC_CHARSET_UNKNOWN);
}

#endif // WEBC_SUPPORT_FILE











