//
// PROTFILE.H - Prototypes and defines for PROTFILE.C
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __PROTFILE_H_INCLUDED__
#define __PROTFILE_H_INCLUDED__

#include "webload.hpp"

#ifdef __cplusplus
extern "C" {
#endif

int  wload_FileGet(UrlStreamCtx *pStream);
int  wload_FilePost(UrlStreamCtx *pStream, WEBC_CHAR * pContentType, long lContentLength);
long wload_FileRead(UrlStreamCtx *pStream, char * buffer, long length);
long wload_FileWrite(UrlStreamCtx *pStream, char * buffer, long length);
WebcFileContentType  wload_FileDataType(UrlStreamCtx *pStream);
int  wload_FileClose(UrlStreamCtx *pStream);
enum WebcCharset wload_FileCharset(UrlStreamCtx *pStream);

#ifdef __cplusplus
}
#endif

#endif
