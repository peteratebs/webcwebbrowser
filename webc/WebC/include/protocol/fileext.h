//
// FILEEXT.H - File extension identifier
//
// EBS - HTTP
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
#ifndef __FILEEXT_H__
#define __FILEEXT_H__

#include "filetype.h"

#ifdef __cplusplus
extern "C" {
#endif

WebcFileContentType webc_GetFileTypeByExtension (const char *fileName);

#ifdef __cplusplus
}
#endif

#endif /*__FILEEXT_H__*/
