#ifndef __WEBVFILE_HPP_INCLUDED__
#define __WEBVFILE_HPP_INCLUDED__

#include "webload.hpp"
#include "webc.h"


#if (WEBC_SUPPORT_INTERNAL)
extern struct WebcVFileEntry gWebcVFileTable[];
#endif

#ifdef __cplusplus
extern "C" {
#endif

int  wload_WebcGet(UrlStreamCtx *pStream);
int  wload_WebcPost(UrlStreamCtx *pStream, WEBC_CHAR * pContentType, long lContentLength);
long wload_WebcRead(UrlStreamCtx *pStream, char * buffer, long length);
long wload_WebcWrite(UrlStreamCtx *pStream, char * buffer, long length);
WebcFileContentType  wload_WebcDataType(UrlStreamCtx *pStream);
int  wload_WebcClose(UrlStreamCtx *pStream);
enum WebcCharset wload_WebcCharset(UrlStreamCtx *pStream);

char *wload_WebcGetFileName(int n, long *pdata_length, WEBC_PFBYTE **pdata);
void wload_WebcVirtualFileSystemInitialize(void);
int wload_WebcVirtualFileSystemTableInstall(const struct WebcVFileEntry *NewTable);
#ifdef __cplusplus
}
#endif

#endif// __WEBVFILE_HPP_INCLUDED__
