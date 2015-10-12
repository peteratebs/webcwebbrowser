//
// WCACHE.CPP - Flexible data cache implementation
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

#include "wcache.h"
#include "webc.h"
#include "webcstr.h"
#include "webcassert.h"
#include "webcmem.h"


#if(WEBC_SUPPORT_CACHE)
#include "rtpprint.h"
#define CACHE_STAT_EVENT_HIT 0
#define CACHE_STAT_EVENT_CREATE 1
#define CACHE_STAT_EVENT_DELETE 2

#if (WEBC_DEBUG_CACHING)
static void wcache_stats_update_entry (struct wcache_ctx *cc, char *key, int what_event);


#define CACHE_STATISTICS_UPDATE(C, K,E)   wcache_stats_update_entry (C, K, E);
static void dump_write_cache_info(char *prompt, unsigned char *buffer, long len)
{
 	rtp_printf("[%s]: len == [%d] \r\n", prompt, len);
}

static void dump_read_cache_info(char *prompt, unsigned char *buffer, long len)
{
int i,np,is_binary;
	is_binary = np = 0;
// rtp_printf("%s: == len == %d [%40.40s]\n", prompt, len, buffer);
rtp_printf("[%s]: len == [%d] \r\n", prompt, len);
 	for (i = 0; i < len; i++)
	{
		if (buffer[i] > 127)
		{
			is_binary =1 ;
			break;
		}
	}
	if (is_binary)
	{
		rtp_printf("[%s]: len == [%d] \r\n", prompt, len);
		return;
	}
	else
		rtp_printf("ascii: %s: == len == %d \r\n", prompt, len);
 	for (i = 0; i < len; i++)
	{
		if (buffer[i] < 127 && buffer[i] >= 32)
		{
			rtp_printf("%c", buffer[i]);
			np += 1;
			if( np > 60)
			{
				rtp_printf("\r\n");
				np = 0;
			}
		}
	}
	rtp_printf("\r\n");
}



#else
#define CACHE_STATISTICS_UPDATE(C, K ,E)
#endif


// Internal routines

// #define rtp_printf
int _wcache_open(struct wcache_ctx *cc, struct wcache_entry *entry, const char * key, struct wcache_stream *wcs, int mode);
struct wcache_entry *_wcache_new_entry(struct wcache_ctx *cc, const char * key);
struct wcache_entry *_wcache_find_entry(struct wcache_ctx *cc, const char * key);
int  _wcache_free_bytes(struct wcache_ctx *cc, long target);
int  _wcache_delete_entry(struct wcache_ctx *cc, struct wcache_entry *entry);
int  _wcache_get_filename(char *fileName, struct wcache_ctx *cc, struct wcache_entry *entry);
int  _wcache_lock_entry(struct wcache_entry *entry, int mode);
void _wcache_unlock_entry(struct wcache_entry *entry);
int _wcache_free_oldest_file(struct wcache_ctx *cc);

long giCacheFileIndexHint = 0;

#ifndef EBSMIN
#define EBSMIN(X,Y) (((X)>(Y))?(Y):(X))
#endif
#ifndef EBSMAX
#define EBSMAX(X,Y) (((X)>(Y))?(X):(Y))
#endif

#define UNLINK_CACHE(X,Y) \
{\
	if ((Y)->next) \
		(Y)->next->prev = (Y)->prev; \
	if ((Y)->prev) \
		(Y)->prev->next = (Y)->next; \
	if ((Y) == (X)->index) \
		(X)->index = (Y)->next; \
	if ((Y) == (X)->lru) \
		(X)->lru = (Y)->prev; \
	(Y)->next = 0; \
	(Y)->prev = 0; \
}

#define INSERT_FRONT_CACHE(X,Y) \
{\
	(Y)->next = (X)->index; \
	(Y)->prev = 0; \
	if ((X)->index) \
		(X)->index->prev = (Y); \
	(X)->index = (Y); \
	if ((X)->lru == 0) \
		(X)->lru = (Y); \
}





/*************************************************************************
 wcache_init() - Initialize a cache context

  cc -

 Returns: 0 on success, -1 otherwise.
*************************************************************************/

int wcache_init(struct wcache_ctx *cc, const char *cachePrefix, long size,
                long maxFiles, const struct wcache_system_spec *spec,
                void *sysPtr)
{
void *fp;
char indexfile[WEBC_CACHE_MAX_PATH_LEN];
int index;
struct wcache_entry *entry;
int resetCache = 1;
char keyTemp[WEBC_CACHE_MAX_KEY_LEN+1];

	tc_strcpy(indexfile, cachePrefix);
	tc_strcat(indexfile, "index.dat");

	if (spec->sys_open(sysPtr, indexfile, &fp, WEBC_CO_READ) >= 0)
	{
		if (spec->sys_read(sysPtr, fp, (WEBC_PFBYTE) cc, sizeof(struct wcache_ctx)) < sizeof(struct wcache_ctx))
		{
			spec->sys_close(sysPtr, fp);
			spec->sys_remove(sysPtr, indexfile);
		}
		else
		{
			long entrySize = sizeof(struct wcache_entry) - sizeof(char *);

			cc->index = 0;
			cc->lru = 0;
			for (index=0; index < cc->numFiles; index++)
			{
				entry = (struct wcache_entry *) WEBC_MALLOC(sizeof(struct wcache_entry));
				if (!entry)
				{
					wcache_free(cc);
					resetCache = 1;
					break;
				}

				if (spec->sys_read(sysPtr, fp, (WEBC_PFBYTE) entry, entrySize) < entrySize)
				{
					wcache_free(cc);
					resetCache = 1;
					break;
				}

				entry->keyLen = EBSMIN(entry->keyLen, WEBC_CACHE_MAX_KEY_LEN - 1);
				spec->sys_read(sysPtr, fp, (WEBC_PFBYTE) keyTemp, entry->keyLen*sizeof(char));
				keyTemp[entry->keyLen] = '\0';
				entry->key = webc_malloc_string_copy_8(keyTemp, __FILE__, __LINE__);
				if (!entry->key)
				{
					WEBC_FREE(entry);
					entry = 0;
					wcache_free(cc);
					resetCache = 1;
					break;
				}
				entry->next = 0;
				entry->prev = 0;
				INSERT_FRONT_CACHE(cc,entry);
			}

			if (index == cc->numFiles)
			{
				resetCache = 0;
			}

			spec->sys_close(sysPtr, fp);
		}
	}

	// just copy the system spec directly
	cc->spec = *spec;
	cc->sysPtr = sysPtr;

	if (resetCache)
	{
		// no index exists; create one
		tc_strncpy(cc->cachePrefix, cachePrefix, WEBC_CACHE_MAX_PREFIX_LEN-1);
		cc->cachePrefix[WEBC_CACHE_MAX_PREFIX_LEN-1] = 0;
		cc->bytesMax = size;
		cc->bytesUsed = sizeof(wcache_ctx);
		cc->numFiles = 0;
		cc->maxFiles = maxFiles;
		cc->initialized = 1;
		cc->index = 0;
		cc->lru = 0;
		wcache_flush(cc);
	}

	return (0);
}

#define WEBC_CACHE_SAVE_DIRECTORY      0

int wcache_flush(struct wcache_ctx *cc)
{
struct wcache_entry *entry;
WEBC_UINT16 saveFlags;
void *fp;
char filename[WEBC_CACHE_MAX_PATH_LEN];
#if (WEBC_CACHE_SAVE_DIRECTORY)
void *fpDir;
char lineBuffer[512];
#endif

	tc_strcpy(filename, cc->cachePrefix);
	tc_strcat(filename, "index.dat");

	if (cc->spec.sys_open(cc->sysPtr, filename, &fp, WEBC_CO_CREATE) < 0)
	{
		return (-1);
	}

#if (WEBC_CACHE_SAVE_DIRECTORY)
	tc_strcpy(filename, cc->cachePrefix);
	tc_strcat(filename, "index.txt");

	if (cc->spec.sys_open(cc->sysPtr, filename, &fpDir, WEBC_CO_CREATE) < 0)
	{
		cc->spec.sys_close(cc->sysPtr, fp);
		return (-1);
	}
#endif

	cc->spec.sys_write(cc->sysPtr, fp, (WEBC_PFBYTE) cc, sizeof(struct wcache_ctx));

#if (WEBC_CACHE_SAVE_DIRECTORY)
	rtp_sprintf(lineBuffer,
			"   index   mime-type          size   flags   date                            expires                         key\r\n"
			"   =====   =========          ====   =====   ====                            =======                         ===\r\n"
			"\r\n");
	cc->spec.sys_write(cc->sysPtr, fpDir, (WEBC_PFBYTE) lineBuffer, tc_strlen(lineBuffer));
#endif


	// reverse the order of all entries so we'll load it back in mru order
	entry = cc->lru;
	while (entry)
	{
		// if we dump the index in the middle of a cache op, then we don't want to have
		//  the entry marked as "open" when we re-load the index
		saveFlags = entry->flags;
		entry->flags &= ~(WEBC_CACHE_ENTRY_LOCKED | WEBC_CACHE_ENTRY_INUSE | WEBC_CACHE_ENTRY_LOCK_MASK);

		cc->spec.sys_write(cc->sysPtr, fp, (WEBC_PFBYTE) entry, sizeof(struct wcache_entry) - sizeof(char *));
		cc->spec.sys_write(cc->sysPtr, fp, (WEBC_PFBYTE) entry->key, entry->keyLen*sizeof(char));

#if (WEBC_CACHE_SAVE_DIRECTORY)
		rtp_sprintf(lineBuffer, "%8x   %9d  %12ld    %04hx   ",
		             entry->fileIndex,
		             entry->dataType,
		             entry->size,
		             entry->flags);

		ebs_print_time(lineBuffer + tc_strlen(lineBuffer), entry->date, 0);
		tc_strcat(lineBuffer, "   ");

		ebs_print_time(lineBuffer + tc_strlen(lineBuffer), entry->expires, 0);
		tc_strcat(lineBuffer, "   ");

		tc_strcat(lineBuffer, entry->key);
		tc_strcat(lineBuffer, "\r\n");

		cc->spec.sys_write(cc->sysPtr, fpDir, (WEBC_PFBYTE) lineBuffer, tc_strlen(lineBuffer));
#endif // WEBC_CACHE_SAVE_DIRECTORY

		entry->flags = saveFlags;
		entry = entry->prev;
	}

	cc->spec.sys_close(cc->sysPtr, fp);

#if (WEBC_CACHE_SAVE_DIRECTORY)
	cc->spec.sys_close(cc->sysPtr, fpDir);
#endif

	return (0);
}

// differs from wcache_clear in that this one doesn't care if entries are locked;
//  it will delete them all.
void wcache_free(struct wcache_ctx *cc)
{
struct wcache_entry *entry;

	while (cc->index)
	{
		entry = cc->index;

		// Unlink entry
		UNLINK_CACHE(cc,entry);

		// release the key string
		if (entry->key)
		{
			webc_free_string_copy_8(entry->key, __FILE__, __LINE__);
		}

		// Free Entry
		WEBC_FREE(entry);
	}
}

int wcache_open(struct wcache_ctx *cc, const char * key, struct wcache_stream *wcs, int mode)
{
struct wcache_entry *entry;

#if 1//FIX ME... SKIPPING HTTP:// CACHING
	if(tc_strncmp(key, "http://", 7)== 0)
		return (-1);
#endif

	// First get the entry we'll be using
	entry = _wcache_find_entry(cc,key);

#if (WEBC_DEBUG_CACHING)
	if (entry)
	{
		CACHE_STATISTICS_UPDATE(cc, (char *)key, CACHE_STAT_EVENT_HIT);
	}
#endif

	return (_wcache_open(cc, entry, key, wcs, mode));
}

int _wcache_open(struct wcache_ctx *cc, struct wcache_entry *entry, const char * key, struct wcache_stream *wcs, int mode)
{
int result;
char filename[WEBC_CACHE_MAX_PATH_LEN];

	result = 0;

	if (mode == WEBC_CO_CREATE)
	{
		if (entry)
		{
			wcache_delete_entry(cc, entry->key);
		}
		entry = _wcache_new_entry(cc, key);
		if (!entry)
		{
			return (-1);
		}
	}

	if (!entry)
	{
		return (-1);
	}

	// Lock this entry so we won't delete it in an attempt to free up blocks for data
	if (!_wcache_lock_entry(entry, mode))
	{
		// this means the lock failed, either because someone else is writing to the
		//  entry (WEBC_CACHE_ENTRY_INUSE flag set), mode is a write mode and someone else
		//  is reading from the entry (WEBC_CACHE_ENTRY_LOCKED is set), or we've reached
		//  the max read count lock limit (255).
		return (-1);
	}

	// Now initialize the stream parameters
	wcs->cc = cc;
	wcs->entry = entry;
	_wcache_get_filename(filename, wcs->cc, wcs->entry);

	if (mode == WEBC_CO_CREATE)
	{
		// flush the cache index now to avoid orphans
		wcache_flush(cc);
	}


	mode = EBSCLIP(mode, 1, 3);
	result = cc->spec.sys_open(cc->sysPtr, filename, &wcs->fp, mode);


	if (result < 0)
	{
		// flush the cache index now to avoid orphans
		_wcache_unlock_entry(entry);
		_wcache_delete_entry(cc, entry);
		wcache_flush(cc);
		return (result);
	}

	wcs->mode = mode;

#if (WEBC_SUPPORT_CACHE_COMPRESSION)
	if (wcs->entry->flags & WEBC_CACHE_ENTRY_COMPRESSED)
	{
		wcs->pZlibStream = (z_streamp ) WEBC_MALLOC(sizeof(z_stream));
		if (wcs->pZlibStream)
		{
			z_streamp pz = wcs->pZlibStream;
			pz->zalloc = Z_NULL;
			pz->zfree = Z_NULL;
			pz->next_in = Z_NULL;
			pz->next_out = Z_NULL;
			pz->avail_in = 0;
			pz->avail_out = 0;
			if (mode == WEBC_CO_READ)
			{
				/* initialize the decompresszion context */
				inflateInit(pz);
			}
			else
			{
				deflateInit(pz, Z_DEFAULT_COMPRESSION);
			}

			wcs->inBufferSize = 2048;
			wcs->outBufferSize = 2048;
			wcs->outBufferPos = 0;
			wcs->outBufferAvail = 0;
			wcs->pInBuffer = (WEBC_UINT8*) WEBC_MALLOC(wcs->inBufferSize);
			wcs->pOutBuffer = (WEBC_UINT8*) WEBC_MALLOC(wcs->outBufferSize);
			if (wcs->pInBuffer == 0)   wcs->inBufferSize  = 0;
			if (wcs->pOutBuffer == 0)  wcs->outBufferSize = 0;
			pz->next_in = wcs->pInBuffer;
			pz->next_out = wcs->pOutBuffer;
			pz->avail_in = 0;
			pz->avail_out = wcs->outBufferSize;
		}
	}
#endif

	return (result);
}

void wcache_get_date(struct wcache_stream *wcs, RTP_TIMESTAMP* date)
{
	*date = *wcs->entry->date;
}

void wcache_get_expires(struct wcache_stream *wcs, RTP_TIMESTAMP* expires)
{
	*expires = *wcs->entry->expires;
}

void wcache_set_date(struct wcache_stream *wcs, RTP_TIMESTAMP* pDate)
{
	tc_movebytes(wcs->entry->date, pDate, sizeof(RTP_TIMESTAMP));
}

void wcache_set_expires(struct wcache_stream *wcs, RTP_TIMESTAMP* pExpDate)
{
	tc_movebytes(wcs->entry->expires, pExpDate, sizeof(RTP_TIMESTAMP));
}

int wcache_get_data_type(struct wcache_stream *wcs)
{
	return (wcs->entry->dataType);
}

void wcache_set_data_type(struct wcache_stream *wcs, int dataType)
{
	if (wcs && wcs->entry)
	{
		wcs->entry->dataType = dataType;
	}
}

char wcache_get_charset (struct wcache_stream *wcs)
{
	return (wcs->entry->charset);
}

void wcache_set_charset (struct wcache_stream *wcs, char charset)
{
	if (wcs && wcs->entry)
	{
		wcs->entry->charset = charset;
	}
}

long wcache_get_size(struct wcache_stream *wcs)
{
	if (wcs && wcs->entry)
	{
		return wcs->entry->size;
	}

	return (0);
}

int wcache_read(struct wcache_stream *wcs, WEBC_PFBYTE buf, long len)
{
#if (WEBC_SUPPORT_CACHE_COMPRESSION)

	if (wcs->entry->flags & WEBC_CACHE_ENTRY_COMPRESSED)
	{
		z_streamp pz = wcs->pZlibStream;
		long bytes_to_read = len, to_copy, bytes_read = 0;
		int result;

		while (bytes_to_read > 0)
		{
			// fill up our output buffer with inflated data (note - this will probably slow
			//  down some operations that grab small amounts of data, but will likely speed up the
			//  inflation process on the whole)
			if (wcs->outBufferPos == wcs->outBufferAvail)
			{
				wcs->outBufferPos = 0;
				wcs->outBufferAvail = 0;
				pz->next_out = wcs->pOutBuffer;
				pz->avail_out = wcs->outBufferSize;
				while (pz->avail_out > 0)
				{
					if (pz->avail_in == 0)
					{
						pz->avail_in = webc_fread (wcs->fp, wcs->pInBuffer, wcs->inBufferSize);
						pz->next_in = wcs->pInBuffer;
					}

					if (pz->avail_in == 0)
					{
						break;
					}

					result = inflate(pz, Z_SYNC_FLUSH);
					if (result < 0)
					{
						// error!
						return (-1);
					}
					wcs->outBufferAvail = (wcs->outBufferSize - pz->avail_out);
				}
			}

			// any inflated data waiting in the buffer?
			if (wcs->outBufferAvail > wcs->outBufferPos)
			{
				to_copy = EBSMIN(bytes_to_read, wcs->outBufferAvail - wcs->outBufferPos);
				tc_movebytes(buf, &wcs->pOutBuffer[wcs->outBufferPos], to_copy);
				buf += to_copy;
				wcs->outBufferPos += to_copy;
				bytes_to_read -= to_copy;
				bytes_read += to_copy;
			}
			else
			{
				break;
			}
		}

		return (bytes_read);
	}
	else
#endif // WEBC_SUPPORT_CACHE_COMPRESSION
	{
		int l;


		l = wcs->cc->spec.sys_read(wcs->cc->sysPtr, wcs->fp, buf, len);
dump_read_cache_info("read", buf, l);
		return(l);
	}
}

int wcache_write(struct wcache_stream *wcs, WEBC_PFBYTE buf, long len)
{
#if (WEBC_SUPPORT_CACHE_COMPRESSION)

	if (wcs->entry->flags & WEBC_CACHE_ENTRY_COMPRESSED)
	{
		z_streamp pz = (z_streamp) wcs->pZlibStream;
		long bytes_to_zip = len;
		long bytes_written = 0;
		int result, val;

		while (bytes_to_zip > 0)
		{
			pz->avail_in = EBSMIN(bytes_to_zip, wcs->inBufferSize);
			pz->next_in = wcs->pInBuffer;
			bytes_to_zip -= pz->avail_in;
			tc_movebytes(wcs->pInBuffer, buf, pz->avail_in);
			buf += pz->avail_in;

			do
			{
				result = deflate(pz, Z_SYNC_FLUSH);
				if (result < 0)
				{
					// error!
					return (-1);
				}

				if (pz->avail_out == 0)
				{
					if (wcs->outBufferSize > (wcs->cc->bytesMax - wcs->cc->bytesUsed))
					{
						// won't fit; try to free up some space
						if (_wcache_free_bytes(wcs->cc, wcs->outBufferSize) == 0)
						{
							return (-1);
						}
					}

					val = wcs->cc->spec.sys_write(wcs->cc->sysPtr, wcs->fp, wcs->pOutBuffer, wcs->outBufferSize);
					if (val < 0)
					{
						return (val);
					}
					wcs->cc->bytesUsed += val;
					bytes_written += val;

					pz->avail_out = wcs->outBufferSize;
					pz->next_out = wcs->pOutBuffer;
				}
				else
				{
					break;
				}
			} while (1);
		}

		wcs->entry->size += bytes_written;
		return (len);
	}
	else
#endif // WEBC_SUPPORT_CACHE_COMPRESSION
	{
	  #if (WEBC_CACHE_MAX_FILE_SIZE)
		len = EBSMIN(len, WEBC_CACHE_MAX_FILE_SIZE - wcs->entry->size);
		if (len <= 0)
		{
			return (0);
		}
	  #endif

		if (len > (wcs->cc->bytesMax - wcs->cc->bytesUsed))
		{
			// won't fit; try to free up some space
			if (_wcache_free_bytes(wcs->cc, len) == 0)
			{
				return (-1);
			}
		}

		len = wcs->cc->spec.sys_write(wcs->cc->sysPtr, wcs->fp, buf, len);

		dump_write_cache_info("write", buf, len);

		if (len >= 0)
		{
			wcs->entry->size += len;
			wcs->cc->bytesUsed += len;
		}
		return (len);
	}
}

int wcache_close(struct wcache_stream *wcs)
{
	// parameter checking
	if ( (wcs == 0) || (wcs->entry == 0) )
	{
		return(0);
	}

#if (WEBC_SUPPORT_CACHE_COMPRESSION)

	if (wcs->entry->flags & WEBC_CACHE_ENTRY_COMPRESSED)
	{
		z_streamp pz = (z_streamp) wcs->pZlibStream;

		if (wcs->mode != WEBC_CO_READ)
		{
			long bytes_written = 0;
			int result, val;

			do
			{
				result = deflate(pz, Z_FINISH);
				if ((result == Z_STREAM_ERROR) || (result == Z_BUF_ERROR))
				{
					break;
				}
				if ((wcs->outBufferSize - pz->avail_out) > (wcs->cc->bytesMax - wcs->cc->bytesUsed))
				{
					// won't fit; try to free up some space
					if (_wcache_free_bytes(wcs->cc, (wcs->outBufferSize - pz->avail_out)) == 0)
					{
						break;
					}
				}

				val = webc_fwrite (wcs->fp, (char *) wcs->pOutBuffer, (wcs->outBufferSize - pz->avail_out));
				if (val < 0)
				{
					break;
				}
				bytes_written += val;
				wcs->cc->bytesUsed += val;

				pz->avail_out = wcs->outBufferSize;
				pz->next_out = wcs->pOutBuffer;

			} while (result != Z_STREAM_END);

			wcs->entry->size += bytes_written;

			deflateEnd(pz);
		}
		else
		{
			inflateEnd(pz);
		}
		WEBC_FREE(wcs->pInBuffer);
		WEBC_FREE(wcs->pOutBuffer);
		WEBC_FREE(pz);
	}

#endif // WEBC_SUPPORT_CACHE_COMPRESSION

	_wcache_unlock_entry(wcs->entry);
	wcs->cc->spec.sys_close(wcs->cc->sysPtr, wcs->fp);
	return (0);
}

/*************************************************************************
 webc_add_cache_entry() - Add new entry to the cache.

 wc - pointer to web client context.
 key - descriptor of URL being cached (can be image or page source).
 data - pointer to data to be cached.
 len - number of bytes to be cached.
 append - 1 to append to existing cache entry, 0 otherwise.
 dataType - WEBC_HTML_DATA, WEBC_GIF_DATA, or WEBC_JPG_DATA.

 This function adds a new entry to the web client's cache.  A cache entry
 consists of a URL descriptor, a pointer to raw data, and a length count.
 The total size of the cache is never allowed to exceed WEBC_MAX_CACHE_SIZE.
 If adding this new entry would make the cache too big, existing entries
 will be deleted before this entry is added.

 There can only be one cache entry for a given URL descriptor.  Thus, if
 'key' is identical to any given cache entry, and 'append' is 0,
 nothing will happen.  If 'append' is 1, the new data will be appended to
 the existing cache entry.

 If it is desired to overwrite an existing cache entry with a new, updated
 one, wcache_delete_entry() must be explicitly called to delete the
 existing cache entry for 'key' before this function is called.

 The URL can be an HTML page or an image (.GIF or .JPG).  Note that for
 page source to be cached, WEBC_CACHE_PAGE_SOURCE must be 1, and for images
 to be cached, WEBC_CACHE_IMAGES must be 1.  WEBC_SUPPORT_CACHE must be 1
 for any type of caching to occur.

 Returns: 0 on success, -1 otherwise.
*************************************************************************/


int wcache_add_entry (
		struct wcache_ctx *cc,
		const char * key,
		void* vdata,
		long len,
		WEBC_UINT8 append,
		int dataType,
		RTP_TIMESTAMP* date,
		RTP_TIMESTAMP* expires
	)
{
struct wcache_stream wcs[1];
int result;

	if (append)
	{
		result = wcache_open(cc, key, wcs, WEBC_CO_APPEND);
	}
	else
	{
		result = wcache_open(cc, key, wcs, WEBC_CO_CREATE);
		if (result >= 0)
		{
			wcs->entry->dataType = dataType;

			if (!date)
			{
				rtp_date_get_timestamp(wcs->entry->date);
			}

			if (!expires)
			{
				rtp_date_get_timestamp(wcs->entry->expires);
			}
		}
	}

	if (result >= 0)
	{
		if (date)
		{
			tc_movebytes((WEBC_PFBYTE)wcs->entry->date, (WEBC_PFBYTE)date, sizeof(RTP_TIMESTAMP));
		}

		if (expires)
		{
			tc_movebytes((WEBC_PFBYTE)wcs->entry->expires, (WEBC_PFBYTE)expires, sizeof(RTP_TIMESTAMP));
		}

		result = (wcache_write(wcs, (WEBC_PFBYTE) vdata, len) == len)? 0 : -1;
		wcache_close(wcs);
	}

	return (result);

}

/*************************************************************************
 wcache_delete_entry() - Delete an entry from the cache.

 wc - pointer to web client context.
 key - Entry with this URL descriptor will be deleted.

 This function deleted an entry from the web client's cache.  It is called
 when the "Refresh" button is clicked, to ensure that the page will be
 reloaded from the file:// or http:// stream, rather than from the cache.

 Returns: 0 on success, -1 if the entry is not found.
*************************************************************************/

int wcache_delete_entry(struct wcache_ctx *cc, const char * key)
{
struct wcache_entry *entry;

	entry = _wcache_find_entry(cc,key);
	if (!entry)
		return (-1);

	return (_wcache_delete_entry(cc, entry));
}

int _wcache_delete_entry(struct wcache_ctx *cc, struct wcache_entry *entry)
{
char filename[WEBC_CACHE_MAX_PATH_LEN];
long totalSize;

	if (entry->flags & WEBC_CACHE_ENTRY_LOCKED)
		return (-1);

	// Unlink entry
	UNLINK_CACHE(cc,entry);
	cc->numFiles--;

	_wcache_get_filename(filename, cc, entry);
	cc->spec.sys_remove(cc->sysPtr, filename);
	totalSize = entry->size + sizeof(wcache_entry);
	WEBC_ASSERT(totalSize <= cc->bytesUsed);
	if (totalSize > cc->bytesUsed)
	{
		// error !
		cc->bytesUsed = 0;
	}
	else
	{
		cc->bytesUsed -= totalSize;
	}

	if (entry->key)
	{
		CACHE_STATISTICS_UPDATE(cc, entry->key,CACHE_STAT_EVENT_DELETE)
		webc_free_string_copy_8(entry->key, __FILE__, __LINE__);
	}

	// Free Entry
	WEBC_FREE(entry);

	return (0);
}


/*************************************************************************
 wcache_clear() - Delete all cache entries.

 wc - pointer to web client context.

 This function deletes all entries from the web client's cache.

 Returns: 0 on success, -1 otherwise.
*************************************************************************/

int wcache_clear(struct wcache_ctx *cc)
{
	struct wcache_entry *next, *current;

	current = cc->index;
	while (current)
	{
		next = current->next;
		_wcache_delete_entry(cc, current);
		current = next;
	}

	return (0);
}

struct wcache_entry *_wcache_new_entry(struct wcache_ctx *cc, const char * key)
{
struct wcache_entry *entry;
char filename[WEBC_CACHE_MAX_PATH_LEN];
void *fp;

	CACHE_STATISTICS_UPDATE(cc, (char *)key,CACHE_STAT_EVENT_CREATE)

	if (sizeof(wcache_entry) > (cc->bytesMax - cc->bytesUsed))
	{
		// won't fit; try to free up some space
		if (!_wcache_free_bytes(cc, sizeof(wcache_entry)))
		{
			return (0);
		}
	}

	if (cc->numFiles >= cc->maxFiles)
	{
		if (!_wcache_free_oldest_file(cc))
		{
			return (0);
		}
	}

	entry = (struct wcache_entry *) WEBC_MALLOC(sizeof(struct wcache_entry));
	if (entry)
	{
		entry->dataType = 0;
		entry->key = webc_malloc_string_copy_8(key, __FILE__, __LINE__);
		if (entry->key)
		{
			entry->keyLen = EBSMIN(WEBC_CACHE_MAX_KEY_LEN-1, tc_strlen(key));
			entry->fileIndex = giCacheFileIndexHint;
			while (1)
			{
				_wcache_get_filename(filename, cc, entry);
				if (cc->spec.sys_open(cc->sysPtr, filename, &fp, WEBC_CO_READ) < 0)
				{
					break;
				}
				cc->spec.sys_close(cc->sysPtr, fp);
				entry->fileIndex++;
				// tbd - bound this loop
			}
			giCacheFileIndexHint = entry->fileIndex + 1;
			entry->size = 0;
			entry->flags = 0;
		  #if (WEBC_SUPPORT_CACHE_COMPRESSION)
			entry->flags |= WEBC_CACHE_ENTRY_COMPRESSED;
		  #endif
			INSERT_FRONT_CACHE(cc,entry);

			cc->bytesUsed += sizeof(wcache_entry);
			cc->numFiles++;
		}
		else
		{
			WEBC_FREE(entry);
			entry = 0;
		}
	}

	return (entry);
}

/*************************************************************************
 _wcache_find_entry() - Find matching cache entry for URL.

 wc - webc_ctx used for all web client API calls
 key - key descriptor to match

 This function searches the web client's cache for an entry whose key
 matches the key supplied as a parameter.

 Returns: Pointer to matching cache entry, or NULL if none found.
*************************************************************************/

struct wcache_entry *_wcache_find_entry(struct wcache_ctx *cc, const char * key)
{
struct wcache_entry *entry;
	entry = cc->index;
	while (entry)
	{

		if (entry->key && tc_strncmp(entry->key, key, WEBC_CACHE_MAX_KEY_LEN) == 0)
		{
			// unlink entry and ...
			UNLINK_CACHE(cc,entry);

			// ... move to front of list
			INSERT_FRONT_CACHE(cc,entry);

			break;
		}
		entry = entry->next;
	}

	return (entry);
}

int _wcache_free_bytes(struct wcache_ctx *cc, long target)
{
	// make sure we have enough potential room in the cache
	// before we start deleting entries
	if (target > cc->bytesMax)
	{
		// cannot free up enough room even if we delete everything
		return (0);
	}

	struct wcache_entry *prev, *lru;

	lru = cc->lru;

	if (!lru)
	{
		return (0);
	}

	while (cc->bytesUsed + target > cc->bytesMax)
	{
		if (!lru)
		{
			return (0);
		}

		prev = lru->prev;
		_wcache_delete_entry(cc, lru);
		lru = prev;
	}

	return (1);
}

int _wcache_free_oldest_file(struct wcache_ctx *cc)
{
struct wcache_entry *prev, *lru;

	if (cc->numFiles <= 0)
	{
		// failure
		return (0);
	}

	lru = cc->lru;

	while (lru)
	{
		prev = lru->prev;
		if (_wcache_delete_entry(cc, lru) == 0)
		{
			return (1);
		}
		lru = prev;
	}

	return (0);
}

int _wcache_get_filename(char *fileName, struct wcache_ctx *cc, struct wcache_entry *entry)
{
char *name;

	tc_strcpy(fileName, cc->cachePrefix);
	name = &fileName[tc_strlen(fileName)];
	tc_itoa(entry->fileIndex, name, 16);
	tc_strcat(fileName, ".dat");

	return (0);
}

int _wcache_lock_entry(struct wcache_entry *entry, int mode)
{
	if ((entry->flags & WEBC_CACHE_ENTRY_INUSE) || ((entry->flags & WEBC_CACHE_ENTRY_LOCK_MASK) == WEBC_CACHE_ENTRY_LOCK_MASK))
	{
		return (0);
	}

	switch (mode)
	{
		case WEBC_CO_READ:
			entry->flags |= WEBC_CACHE_ENTRY_LOCKED;
			entry->flags = (entry->flags & ~WEBC_CACHE_ENTRY_LOCK_MASK) | ((entry->flags & WEBC_CACHE_ENTRY_LOCK_MASK) + 1);
			break;

		default:
			if (entry->flags & WEBC_CACHE_ENTRY_LOCKED)
			{
				return (0);
			}
			entry->flags |= WEBC_CACHE_ENTRY_INUSE | WEBC_CACHE_ENTRY_LOCKED;
			break;
	}

	return (1);
}

void _wcache_unlock_entry(struct wcache_entry *entry)
{
	if (entry->flags & WEBC_CACHE_ENTRY_INUSE)
	{
		entry->flags &= ~WEBC_CACHE_ENTRY_INUSE;
		entry->flags &= ~WEBC_CACHE_ENTRY_LOCKED;
	}
	else if (entry->flags & WEBC_CACHE_ENTRY_LOCK_MASK)
	{
		entry->flags = (entry->flags & ~WEBC_CACHE_ENTRY_LOCK_MASK) | ((entry->flags & WEBC_CACHE_ENTRY_LOCK_MASK) - 1);

		if (!(entry->flags & WEBC_CACHE_ENTRY_LOCK_MASK))
		{
			entry->flags &= ~WEBC_CACHE_ENTRY_LOCKED;
		}
	}
}

// wcache_merge - save the entire contents of one cache into another

void wcache_merge (struct wcache_ctx *dst, struct wcache_ctx *src)
{
	struct wcache_entry *entry;
	struct wcache_stream srcStream, dstStream;
	WEBC_PFBYTE buffer = (WEBC_PFBYTE) WEBC_MALLOC(1024);
	if (!buffer)
	{
		return;
	}

	long bytesCopied = 0;

	entry = src->index;
	while (entry)
	{
		if (_wcache_open(src, entry, entry->key, &srcStream, WEBC_CO_READ) >= 0)
		{
			if (wcache_open(dst, entry->key, &dstStream, WEBC_CO_CREATE) >= 0)
			{
				wcache_set_data_type(&dstStream, entry->dataType);
				wcache_set_charset(&dstStream, entry->charset);
				wcache_set_expires(&dstStream, entry->date);
				wcache_set_date(&dstStream, entry->expires);

				// now copy the contents of the file in 1024 byte_ chunks
				do
				{
					bytesCopied = wcache_read(&srcStream, buffer, 1024);
					if (bytesCopied > 0)
					{
						wcache_write(&dstStream, buffer, bytesCopied);
					}
				}
				while (bytesCopied > 0);

				wcache_close(&dstStream);
			}
			wcache_close(&srcStream);
		}

		entry = entry->next;
	}

	WEBC_FREE(buffer);
}

int wcache_get_entries (struct wcache_ctx *cc, struct wcache_entry *entry)
{
	if (cc->index)
	{
		tc_memset(entry, 0, sizeof(struct wcache_entry));
		entry->next = cc->index;
		return (0);
	}

	tc_memset(entry, 0, sizeof(wcache_entry));

	return (-1);
}

int wcache_next_entry (struct wcache_ctx *cc, struct wcache_entry *entry)
{
	if (entry->next)
	{
		struct wcache_entry *next = entry->next;
		*entry = *next;
		return (0);
	}

	tc_memset(entry, 0, sizeof(wcache_entry));

	return (-1);
}

void wcache_release_entry (struct wcache_ctx *cc, struct wcache_entry *entry)
{
	tc_memset(entry, 0, sizeof(wcache_entry));
}



/******************CACHE STATS****************************************/
#if (WEBC_DEBUG_CACHING)

struct wcache_stats_entry
{
	char *key;
	unsigned long delete_count;
	unsigned long create_count;
	unsigned long hit_count;
	struct wcache_stats_entry *pnext;
};

static struct wcache_stats_entry *wcache_stats_data;

static struct wcache_stats_entry *wcache_stats_find_entry (char *key)
{
struct wcache_stats_entry *pentry;
	pentry = wcache_stats_data;
	while (pentry)
	{
		if (tc_strncmp(pentry->key, key, WEBC_CACHE_MAX_KEY_LEN) == 0)
			return(pentry);
		pentry = pentry->pnext;

	}
	return(0);
}

static struct wcache_stats_entry *wcache_stats_add_entry (char *key)
{
struct wcache_stats_entry *pstats_entry;


	pstats_entry = (struct wcache_stats_entry *) WEBC_MALLOC(sizeof(struct wcache_entry));
	WEBC_ASSERT(pstats_entry);
	if (pstats_entry)
	{
		tc_memset(pstats_entry, 0, sizeof(*pstats_entry));
		pstats_entry->key = webc_malloc_string_copy_8(key, __FILE__, __LINE__);
		if (!wcache_stats_data)
			wcache_stats_data = pstats_entry;
		else
		{
			struct wcache_stats_entry *pendentry;
			pendentry = wcache_stats_data;
			while (pendentry->pnext)
				pendentry = pendentry->pnext;
			pendentry->pnext = pstats_entry;
		}
	}
	return(pstats_entry);
}


struct wcache_ctx cc_accum;

static void wcache_stats_update_entry (struct wcache_ctx *cc, char *key, int what_event)
{
struct wcache_stats_entry *pstats_entry;


	pstats_entry = wcache_stats_find_entry (key);

	if (!pstats_entry)
	{
		pstats_entry = wcache_stats_add_entry (key);

		cc_accum.bytesMax  = cc->bytesMax;
		cc_accum.bytesUsed += cc->bytesUsed;
		cc_accum.maxFiles  = cc->maxFiles;
		cc_accum.numFiles  += cc->numFiles;
	}

	if (pstats_entry)
	{
		switch(what_event) {
			case CACHE_STAT_EVENT_HIT:
				pstats_entry->hit_count += 1;
			break;
			case CACHE_STAT_EVENT_CREATE:
				pstats_entry->create_count += 1;
			break;
			case CACHE_STAT_EVENT_DELETE:
				pstats_entry->delete_count += 1;
			break;
		}
	}
}

void wcache_stats_display(char *prompt)
{
struct wcache_stats_entry *pstats_entry,*prev;
int n_entries = 0;

	rtp_printf("Cache statistics (%s)\r\n", prompt);
	rtp_printf("%4.4s %4.4s %4.4s %60.60s \r\n", "Creat", "Dele", "Hit", "Url Value");
	rtp_printf("%4.4s %4.4s %4.4s %60.60s \r\n", "====",  "====", "===", "=========");

	pstats_entry = wcache_stats_data;
	while (pstats_entry)
	{
		rtp_printf("% 4.4d % 4.4d % 4.4d %60.60s \r\n", pstats_entry->create_count, pstats_entry->delete_count, pstats_entry->hit_count,pstats_entry->key);
		prev = pstats_entry;
		pstats_entry = pstats_entry->pnext;
		n_entries += 1;
	}

	rtp_printf("%15.15s %15.15s %15.15s %15.15s \r\n", "bytesMax", "TotalBytesUsed", "maxFiles", "TotalNumFiles");
	rtp_printf("%15.15s %15.15s %15.15s %15.15s \r\n", "========", "=========", "========", "========");
	rtp_printf("%15d %15d %15d %15d \r\n", cc_accum.bytesMax, cc_accum.bytesUsed, cc_accum.maxFiles, cc_accum.numFiles);
}

void wcache_stats_clear(void)
{
struct wcache_stats_entry *pstats_entry,*prev;

	pstats_entry = wcache_stats_data;
	while (pstats_entry)
	{
		prev = pstats_entry;
		pstats_entry = pstats_entry->pnext;
		WEBC_FREE(prev->key);
		WEBC_FREE(prev);
	}
	wcache_stats_data = 0;
}
#else
void wcache_stats_display(char *prompt)
{
	rtp_printf("Cache statistics are disabled\r\n");
}

void wcache_stats_clear(void)
{
}

#endif /* (WEBC_DEBUG_CACHING) */


#endif // WEBC_SUPPORT_CACHE
