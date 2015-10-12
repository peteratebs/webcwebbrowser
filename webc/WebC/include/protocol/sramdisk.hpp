#ifndef __SRAMDISK_HPP__
#define __SRAMDISK_HPP__

#define ERR_RAMDISK_ALLOC_FAIL   -1  // Write failed because MALLOC returns NULL
#define ERR_RAMDISK_EOF          -2  // Read failed because we're at the end of the file
#define ERR_RAMDISK_RDONLY       -3  // Write failed because stream is read-only
#define ERR_RAMDISK_WRONLY       -4  // Read failed because stream is write-only

#include "webctypes.h"

enum SimpleRamDiskFileState
{
	SIMPLE_RAMDISK_FILE_CLOSED = 0,
	SIMPLE_RAMDISK_FILE_READING,
	SIMPLE_RAMDISK_FILE_WRITING,
	SIMPLE_RAMDISK_FILE_DELETED      // this means we've been deleted but we are still open,
	                                 //   so the real deletion happens when all streams have
	                                 //   been closed
};

enum SimpleRamDiskStreamMode
{
	SIMPLE_RAMDISK_STREAM_READONLY,
	SIMPLE_RAMDISK_STREAM_CREATE,
	SIMPLE_RAMDISK_STREAM_APPEND
};

struct SimpleRamDiskBlock
{
	SimpleRamDiskBlock     * pNext;
	long                     iSize;
	char                     pData[1];
};

class SimpleRamDiskFile;
class SimpleRamDiskStream;

class SimpleRamDisk
{
protected:
	SimpleRamDiskFile      * mpFirstFile;
	SimpleRamDiskFile      * mpLastFile;

	void                     InsertFirst (SimpleRamDiskFile *pFile);
	void                     InsertLast  (SimpleRamDiskFile *pFile);
	void                     Unlink      (SimpleRamDiskFile *pFile);
	SimpleRamDiskFile      * Find        (const char *fileName);	

public:
	SimpleRamDisk ();
	~SimpleRamDisk ();

	SimpleRamDiskStream    * Open     (const char *fileName, SimpleRamDiskStreamMode mode);
	WEBC_BOOL                Remove   (const char *fileName);
	WEBC_BOOL                Move     (const char *oldName, const char *newName);	
	void                     Clear    (void);
};

class SimpleRamDiskFile
{
protected:
	SimpleRamDiskBlock     * mpFirstBlock;
	SimpleRamDiskFileState   mState;
	long                     miSize;
	char                   * mpName;
	int                      miOpenStreams;

	SimpleRamDiskBlock     * LastInChain (SimpleRamDiskBlock *first);

public:
	SimpleRamDisk          * mpDisk;
	SimpleRamDiskFile      * mpNext;
	SimpleRamDiskFile      * mpPrev;

	SimpleRamDiskFile (const char *pName, SimpleRamDisk *pDisk);
	~SimpleRamDiskFile ();

	SimpleRamDiskStream    * Open          (SimpleRamDiskStreamMode mode);
	SimpleRamDiskBlock     * GetBlock      (long iPosition, long *piOffset);
	SimpleRamDiskBlock     * GetLastBlock  (void);
	void                     FreeBlocks    (void);
	void                     FreeChain     (SimpleRamDiskBlock *first);
	const char             * Name          (void);
	void                     SetName       (const char *pName);
	long                     Size          (void);
	void                     SetFirstBlock (SimpleRamDiskBlock *block);
	void                     InsertBlock   (SimpleRamDiskBlock *after, SimpleRamDiskBlock *block);
	void                     Close         (SimpleRamDiskStream *stream);
	SimpleRamDiskFileState   State         (void);
	void                     Delete        (void);
};

class SimpleRamDiskStream
{
protected:
	SimpleRamDiskFile      * mpFile;
	long                     miPosition;
	long                     miOffset;
	SimpleRamDiskBlock     * mpBlock;
	SimpleRamDiskStreamMode  mMode;
	
public:
	SimpleRamDiskStream (SimpleRamDiskFile *pFile, SimpleRamDiskStreamMode mode);
	~SimpleRamDiskStream ();

	long                     Read  (char *buffer, long size);
	long                     Write (const char *buffer, long size);
	void                     Close (void);
};


#endif // __SRAMDISK_HPP__

