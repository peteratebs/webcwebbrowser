#ifndef __WEBC_CACHE_INCLUDED__
#define __WEBC_CACHE_INCLUDED__

#include "webcfile.h"
#include "webcfg.h"
#include "rtpdutil.h"

#if (WEBC_SUPPORT_CACHE_COMPRESSION)
#include "zlib.h"
#endif

/***** Configuration Constants ***********************************************/

#define WEBC_CACHE_MAX_PREFIX_LEN       20
#define WEBC_CACHE_MAX_KEY_LEN          256
#define WEBC_CACHE_MAX_FILE_SIZE        0  /* set to non-zero to limit file size */


/***** Other Constants *******************************************************/

#define WEBC_CACHE_INITIALIZED          0x01
#define WEBC_CACHE_ENFORCE_MAX_BYTES    0x02
#define WEBC_CACHE_ENFORCE_MAX_FILES    0x04
#define WEBC_CACHE_COMPRESS_DATA        0x08
#define WEBC_CACHE_FLUSH_INDEX          0x10
#define WEBC_CACHE_SAVE_TEXT_INDEX      0x20

#define WEBC_CACHE_ENTRY_DIRTY          0x0100	// Needs to be flushed to disk
#define WEBC_CACHE_ENTRY_VALID          0x0200	// Contains valid data
#define WEBC_CACHE_ENTRY_LOCKED         0x0400	// Can not be swapped out
#define WEBC_CACHE_ENTRY_INUSE          0x0800	// Some other process is writing data to this entry
#define WEBC_CACHE_ENTRY_DELETED        0x1000	// Deletion pending
#define WEBC_CACHE_ENTRY_COMPRESSED     0x2000  // Uses ZLIB compression
#define WEBC_CACHE_ENTRY_LOCK_MASK      0x00ff	// lock counter

// file open modes - apply to cache streams and system streams (files)
#define WEBC_CO_READ                    1
#define WEBC_CO_CREATE                  2
#define WEBC_CO_APPEND                  3

/***** Data Structures *******************************************************/

struct wcache_system_spec 
{
	int    (*sys_open)   (void *ctx, const char *fileName, void **fileStream, int mode);
	int    (*sys_close)  (void *ctx, void *fileStream);
	int    (*sys_flush)  (void *ctx, void *fileStream);
	long   (*sys_read)   (void *ctx, void *fileStream, WEBC_PFBYTE buffer, long length);
	long   (*sys_write)  (void *ctx, void *fileStream, WEBC_PFBYTE buffer, long length);
	int    (*sys_remove) (void *ctx, const char *fileName);
};

struct wcache_ctx
{
	char cachePrefix[WEBC_CACHE_MAX_PREFIX_LEN + 1];
	long bytesMax;
	long bytesUsed;
	long numFiles;
	long maxFiles;
	WEBC_UINT8 initialized;	
	struct wcache_entry *index;
	struct wcache_entry *lru;
	struct wcache_system_spec spec;
	void *sysPtr;
};

struct wcache_entry
{
	long fileIndex; // HEX of this number is used as filename; i.e. XXXXXXXX.DAT, where XXXXXXXX is fileindex in HEX
	WEBC_UINT8 dataType;
	char charset;
	long size;
	WEBC_UINT16 flags;	
	RTP_TIMESTAMP date[1];
	RTP_TIMESTAMP expires[1];
	struct wcache_entry *next;
	struct wcache_entry *prev;
	short keyLen;
	char *key;
};

struct wcache_stream
{
	char mode;
	struct wcache_ctx *cc;
	struct wcache_entry *entry;
	void *fp;
#if (WEBC_SUPPORT_CACHE_COMPRESSION)
	long inBufferSize;
	long outBufferSize;
	long outBufferPos;
	long outBufferAvail;
	WEBC_UINT8 *pInBuffer;
	WEBC_UINT8 *pOutBuffer;
	z_streamp pZlibStream;
#endif // WEBC_SUPPORT_CACHE_COMPRESSION
};


/***** API Function Prototypes ***********************************************/

#ifdef __cplusplus
extern "C" {
#endif

int      wcache_init          (struct wcache_ctx *cc, const char *cachePrefix, long maxBytes, long maxFiles, 
                               const struct wcache_system_spec *spec, void *sysPtr);
int      wcache_flush         (struct wcache_ctx *cc);
int      wcache_clear         (struct wcache_ctx *cc);
void     wcache_free          (struct wcache_ctx *cc);
int      wcache_add_entry     (struct wcache_ctx *cc, const char *key, void* vdata, long len, WEBC_UINT8 append, 
                               int data_type, RTP_TIMESTAMP* date, RTP_TIMESTAMP* expires);
void     wcache_set_option    (struct wcache_ctx *cc, int option);
int      wcache_get_option    (struct wcache_ctx *cc, int option);
void     wcache_clear_option  (struct wcache_ctx *cc, int option);
void     wcache_merge         (struct wcache_ctx *dst, struct wcache_ctx *src);
int      wcache_delete_entry  (struct wcache_ctx *cc, const char *key);
int      wcache_get_entries   (struct wcache_ctx *cc, struct wcache_entry *entry);
int      wcache_next_entry    (struct wcache_ctx *cc, struct wcache_entry *entry);
void     wcache_release_entry (struct wcache_ctx *cc, struct wcache_entry *entry);

int      wcache_open          (struct wcache_ctx *cc, const char *key, struct wcache_stream *wcs, int mode);

int      wcache_read          (struct wcache_stream *wcs, WEBC_PFBYTE buf, long len);
int      wcache_write         (struct wcache_stream *wcs, WEBC_PFBYTE buf, long len);
int      wcache_close         (struct wcache_stream *wcs);
char     wcache_get_charset   (struct wcache_stream *wcs);
int      wcache_get_data_type (struct wcache_stream *wcs);
void     wcache_get_date      (struct wcache_stream *wcs, RTP_TIMESTAMP* date);
void     wcache_get_expires   (struct wcache_stream *wcs, RTP_TIMESTAMP* expires);
long     wcache_get_size      (struct wcache_stream *wcs);

void     wcache_set_charset   (struct wcache_stream *wcs, char charset);
void     wcache_set_data_type (struct wcache_stream *wcs, int data_type);
void     wcache_set_date      (struct wcache_stream *wcs, RTP_TIMESTAMP* date);
void     wcache_set_expires   (struct wcache_stream *wcs, RTP_TIMESTAMP* expires);

#ifdef __cplusplus
}
#endif

#endif
