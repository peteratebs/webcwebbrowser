#ifndef __WEBC_FILE_INCLUDED
#define __WEBC_FILE_INCLUDED

#include "webcfg.h"
#include "webctypes.h"
#include "rtpfile.h"

typedef RTP_FILE WEBC_FILE;

#define WEBC_FILE_RDONLY 0
#define WEBC_FILE_WRONLY 1
#define WEBC_FILE_APPEND 2
#define WEBC_FILE_UPDATE 3

#ifdef __cplusplus
extern "C" {
#endif

#if (WEBC_SUPPORT_FILE)
// File system porting routines
int  webc_fclose (WEBC_FILE file_handle);
int  webc_fopen  (const char * path, WEBC_FILE *pfile_handle, int mode);
unsigned long webc_flength (WEBC_FILE file_handle);
long webc_fread  (WEBC_FILE file_handle, char * buf, long size, int (*idle_func)(WEBC_UINT8*), WEBC_UINT8* idle_data);
long webc_fwrite (WEBC_FILE file_handle, char * buf, long size, int (*idle_func)(WEBC_UINT8*), WEBC_UINT8* idle_data);
#endif

#ifdef __cplusplus
}
#endif

#endif
