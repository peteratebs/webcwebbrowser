//
// PROTHSSL.H - Prototypes and defines for PROTHSSL.C
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __PROTHSSL_H_INCLUDED__
#define __PROTHSSL_H_INCLUDED__

#include "webload.hpp"

#ifdef __cplusplus
extern "C" {
#endif

int  wload_HttpsGet(UrlStreamCtx *pStream);
int  wload_HttpsPost(UrlStreamCtx *pStream, WEBC_CHAR * pContentType, long lContentLength);
long wload_HttpsRead(UrlStreamCtx *pStream, char * buffer, long length);
long wload_HttpsWrite(UrlStreamCtx *pStream, char * buffer, long length);
WebcFileContentType  wload_HttpsDataType(UrlStreamCtx *pStream);
int  wload_HttpsClose(UrlStreamCtx *pStream);
enum WebcCharset wload_HttpsCharset(UrlStreamCtx *pStream);

#ifdef __cplusplus
}
#endif

#endif
