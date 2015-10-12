//
// PROTHTTP.H - Prototypes and defines for PROTHTTP.C
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __PROTHTTP_H_INCLUDED__
#define __PROTHTTP_H_INCLUDED__

#include "webload.hpp"

#ifdef __cplusplus
extern "C" {
#endif

int  wload_HttpGet(UrlStreamCtx *pStream);
int  wload_HttpPost(UrlStreamCtx *pStream, WEBC_CHAR * pContentType, long lContentLength);
long wload_HttpRead(UrlStreamCtx *pStream, char * buffer, long length);
long wload_HttpWrite(UrlStreamCtx *pStream, char * buffer, long length);
WebcFileContentType  wload_HttpDataType(UrlStreamCtx *pStream);
int  wload_HttpClose(UrlStreamCtx *pStream);
enum WebcCharset wload_HttpCharset(UrlStreamCtx *pStream);

#ifdef __cplusplus
}
#endif

#endif
