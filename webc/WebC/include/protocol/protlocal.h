//
// PROTLOCAL.H - Prototypes and defines for PROTFILE.C
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __PROTLOCAL_H_INCLUDED__
#define __PROTLOCAL_H_INCLUDED__

#include "webload.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#if(WEBC_SUPPORT_LOCAL)
int  wload_LocalGet(UrlStreamCtx *pStream);
int  wload_LocalPost(UrlStreamCtx *pStream, WEBC_CHAR * pContentType, long lContentLength);
long wload_LocalRead(UrlStreamCtx *pStream, char * buffer, long length);
long wload_LocalWrite(UrlStreamCtx *pStream, char * buffer, long length);
WebcFileContentType  wload_LocalDataType(UrlStreamCtx *pStream);
int  wload_LocalClose(UrlStreamCtx *pStream);
enum WebcCharset wload_LocalCharset(UrlStreamCtx *pStream);
#endif

#ifdef __cplusplus
}
#endif

#endif
