//
// PROTOFFL.H - Prototypes and defines for PROTOFFL.C
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __PROTOFFL_H_INCLUDED__
#define __PROTOFFL_H_INCLUDED__

#include "webload.hpp"

#ifdef __cplusplus
extern "C" {
#endif

int  wload_OfflineGet(UrlStreamCtx *pStream);
int  wload_OfflinePost(UrlStreamCtx *pStream, WEBC_CHAR * pContentType, long lContentLength);
long wload_OfflineRead(UrlStreamCtx *pStream, char * buffer, long length);
long wload_OfflineWrite(UrlStreamCtx *pStream, char * buffer, long length);
WebcFileContentType  wload_OfflineDataType(UrlStreamCtx *pStream);
int  wload_OfflineClose(UrlStreamCtx *pStream);
enum WebcCharset wload_OfflineCharset(UrlStreamCtx *pStream);

#ifdef __cplusplus
}
#endif

#endif
