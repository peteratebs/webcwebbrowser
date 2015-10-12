//
// SRAMDISK.CPP - Methods for class SimpleRamDisk
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "webcfg.h"

#if (WEBC_SUPPORT_OFFLINE_BROWSING)

#include "sramdisk.hpp"
#include "webc.h"
#include "webcstr.h"    // for webc_malloc_string_copy
#include "webcmem.h"


/*****************************************************************************/
// Local Constants 
/*****************************************************************************/

/*****************************************************************************/
// Local type definitions
/*****************************************************************************/

/*****************************************************************************/
// Local Functions
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// class SimpleRamDisk
/*****************************************************************************/

SimpleRamDisk::SimpleRamDisk ()
{
	mpFirstFile = 0;
	mpLastFile = 0;
}

SimpleRamDisk::~SimpleRamDisk ()
{
	Clear();
}

void SimpleRamDisk::InsertFirst (SimpleRamDiskFile *pFile)
{
	if (mpFirstFile)
	{
		mpFirstFile->mpPrev = pFile;
	}
	else
	{
		mpLastFile = pFile;
	}
	
	pFile->mpNext = mpFirstFile;
	pFile->mpPrev = 0;
	mpFirstFile = pFile;
}

void SimpleRamDisk::InsertLast (SimpleRamDiskFile *pFile)
{
	if (mpLastFile)
	{
		mpLastFile->mpNext = pFile;
	}
	else
	{
		mpFirstFile = pFile;
	}
	
	pFile->mpPrev = mpLastFile;
	pFile->mpNext = 0;
	mpLastFile = pFile;
}

void SimpleRamDisk::Unlink (SimpleRamDiskFile *pFile)
{
	if (pFile->mpPrev)
	{
		pFile->mpPrev->mpNext = pFile->mpNext;
	}
	else
	{
		mpFirstFile = pFile->mpNext;
	}
	
	if (pFile->mpNext)
	{
		pFile->mpNext->mpPrev = pFile->mpPrev;
	}
	else
	{
		mpLastFile = pFile->mpPrev;
	}
	
	pFile->mpNext = 0;
	pFile->mpPrev = 0;
}

SimpleRamDiskFile * SimpleRamDisk::Find (const char *fileName)
{
	SimpleRamDiskFile *file = mpFirstFile;
	
	while (file)
	{
		if (file->Name() && !tc_strcmp(file->Name(), fileName))
		{
			return (file);
		}
		file = file->mpNext;
	}
	
	return (0);
}

SimpleRamDiskStream * SimpleRamDisk::Open (const char *fileName, SimpleRamDiskStreamMode mode)
{
	SimpleRamDiskFile *file = Find(fileName);
	
	if (file)
	{
		return (file->Open(mode));
	}

	if (mode == SIMPLE_RAMDISK_STREAM_CREATE)
	{
		WEBC_NEW(file, SimpleRamDiskFile(fileName, this));
		if (file)
		{
			InsertFirst(file);
			return (file->Open(mode));
		}
	}
	
	return (0);
}

WEBC_BOOL SimpleRamDisk::Remove (const char *fileName)
{
	SimpleRamDiskFile *file = Find(fileName);
	
	if (file && (file->State() == SIMPLE_RAMDISK_FILE_CLOSED))
	{
		Unlink(file);
		file->Delete();
		return (WEBC_TRUE);
	}

	return (WEBC_FALSE);	
}

WEBC_BOOL SimpleRamDisk::Move (const char *oldName, const char *newName)
{
	SimpleRamDiskFile *file = Find(oldName);
	
	if (file)
	{
		// tbd - is it OK to do this if the file has open streams?
		file->SetName(newName);
		return (WEBC_TRUE);
	}

	return (WEBC_FALSE);	
}

void SimpleRamDisk::Clear (void)
{
	SimpleRamDiskFile *file;
	
	while (mpFirstFile)
	{
		file = mpFirstFile;
		Unlink(file);
		file->Delete();
	}
}


/*****************************************************************************/
// class SimpleRamDiskFile
/*****************************************************************************/

SimpleRamDiskFile::SimpleRamDiskFile (const char *pName, SimpleRamDisk *pDisk)
{
	mpFirstBlock = 0;
	mState = SIMPLE_RAMDISK_FILE_CLOSED;
	miSize = 0;
	miOpenStreams = 0;
	mpName = webc_malloc_string_copy_8(pName, __FILE__, __LINE__);
	mpDisk = pDisk;
	mpNext = 0;
	mpPrev = 0;
}

SimpleRamDiskFile::~SimpleRamDiskFile ()
{
	FreeBlocks();
	webc_free_string_copy_8(mpName, __FILE__, __LINE__);
}

SimpleRamDiskStream * SimpleRamDiskFile::Open  (SimpleRamDiskStreamMode mode)
{
	SimpleRamDiskStream *stream = 0;
	
	switch (mState)
	{
		case SIMPLE_RAMDISK_FILE_CLOSED:
			WEBC_NEW(stream, SimpleRamDiskStream(this, mode));
			break;
						
		case SIMPLE_RAMDISK_FILE_READING:
			if (mode == SIMPLE_RAMDISK_STREAM_READONLY)
			{
				WEBC_NEW(stream, SimpleRamDiskStream(this, mode));
			}
			break;
        default:
            break;			
		// when we are writing, we never allow anyone to open us
	}

	if (stream)
	{
		miOpenStreams++;
		switch (mode)
		{
			case SIMPLE_RAMDISK_STREAM_READONLY:
				mState = SIMPLE_RAMDISK_FILE_READING;
				break;
				
			case SIMPLE_RAMDISK_STREAM_APPEND:
			case SIMPLE_RAMDISK_STREAM_CREATE:
				mState = SIMPLE_RAMDISK_FILE_WRITING;
				break;
		}
	}

	return (stream);
}

SimpleRamDiskFileState SimpleRamDiskFile::State (void)
{
	return (mState);
}

void SimpleRamDiskFile::Close (SimpleRamDiskStream *stream)
{	
	WEBC_DELETE(stream);

	if (miOpenStreams > 0)
	{
		miOpenStreams--;
	}
	
	if (miOpenStreams == 0)
	{
		if (mState == SIMPLE_RAMDISK_FILE_DELETED)
		{
			Delete();
		}
		else
		{
			mState = SIMPLE_RAMDISK_FILE_CLOSED;
		}
	}
}


SimpleRamDiskBlock * SimpleRamDiskFile::GetBlock (long iPosition, long *piOffset)
{
	SimpleRamDiskBlock *block = mpFirstBlock;
	
	while (block)
	{
		if (block->iSize > iPosition)
		{
			if (piOffset)
			{
				*piOffset = iPosition;
			}
			return (block);
		}
		
		iPosition -= block->iSize;
		block = block->pNext;
	}

	if (piOffset)
	{
		// set offset to the number of bytes we fell short
		*piOffset = iPosition;
	}
	
	return (0);
}

SimpleRamDiskBlock * SimpleRamDiskFile::LastInChain (SimpleRamDiskBlock *block)
{
	while (block && block->pNext)
	{
		block = block->pNext;
	}

	return (block);
}

SimpleRamDiskBlock * SimpleRamDiskFile::GetLastBlock (void)
{
	return (LastInChain(mpFirstBlock));
}

void SimpleRamDiskFile::FreeChain (SimpleRamDiskBlock *block)
{
	SimpleRamDiskBlock *next;
	
	while (block)
	{
		next = block->pNext;
		miSize -= block->iSize;
		WEBC_FREE(block);
		block = next;
	}
}

void SimpleRamDiskFile::FreeBlocks (void)
{
	FreeChain(mpFirstBlock);
	mpFirstBlock = 0;
}

const char * SimpleRamDiskFile::Name (void)
{
	return (mpName);
}

void SimpleRamDiskFile::SetName (const char *pName)
{
	char *newName = webc_malloc_string_copy_8(pName, __FILE__, __LINE__);
	webc_free_string_copy_8(mpName, __FILE__, __LINE__);
	mpName = newName;
}

long SimpleRamDiskFile::Size (void)
{
	return (miSize);
}

void SimpleRamDiskFile::SetFirstBlock (SimpleRamDiskBlock *block)
{
	if (mpFirstBlock)
	{
		FreeBlocks();
	}
	
	mpFirstBlock = block;
	miSize = block->iSize;
}

void SimpleRamDiskFile::InsertBlock (SimpleRamDiskBlock *after, SimpleRamDiskBlock *block)
{
	SimpleRamDiskBlock *last = LastInChain(block);
	last->pNext = 0;
	FreeChain(after->pNext);
	after->pNext = block;
	miSize += block->iSize;
}

void SimpleRamDiskFile::Delete (void)
{
	if (miOpenStreams > 0)
	{
		mState = SIMPLE_RAMDISK_FILE_DELETED;
	}
	else
	{
		WEBC_DELETE(this);
	}
}


/*****************************************************************************/
// class SimpleRamDiskStream
/*****************************************************************************/

SimpleRamDiskStream::SimpleRamDiskStream (SimpleRamDiskFile *pFile, SimpleRamDiskStreamMode mode)
{
	mpFile = pFile;
	mpBlock = 0;
	miPosition = 0;
	miOffset = 0;
	
	switch (mode)
	{
		case SIMPLE_RAMDISK_STREAM_READONLY:
			mpBlock = pFile->GetBlock(0, &miOffset);
			break;

		// miPosition and miOffset are not used for write-only streams
			
		case SIMPLE_RAMDISK_STREAM_CREATE:
			pFile->FreeBlocks();
			break;
			
		case SIMPLE_RAMDISK_STREAM_APPEND:
			mpBlock = pFile->GetLastBlock();
			break;
	}
	
	mMode = mode;
}

SimpleRamDiskStream::~SimpleRamDiskStream ()
{
}

long SimpleRamDiskStream::Read (char *buffer, long size)
{
	long bytesToCopy = 0;
	long bytesRead = 0;
	
	if (mMode != SIMPLE_RAMDISK_STREAM_READONLY)
	{
		return ERR_RAMDISK_WRONLY;
	}
	
	if (!mpBlock)
	{
		return ERR_RAMDISK_EOF;
	}
	
	while (mpBlock && (size > 0))
	{	
		bytesToCopy = EBSMIN(size, mpBlock->iSize - miOffset);
		tc_movebytes(buffer, &mpBlock->pData[miOffset], bytesToCopy);
		buffer     += bytesToCopy;
		size       -= bytesToCopy;
		miOffset   += bytesToCopy;
		miPosition += bytesToCopy;
		bytesRead  += bytesToCopy;
		 
		if (miOffset >= mpBlock->iSize)
		{
			miOffset = 0;
			mpBlock = mpBlock->pNext;
		}
		else
		{
			// size should be 0
			break;
		}
	}
	
	return (bytesRead);
}

long SimpleRamDiskStream::Write (const char *buffer, long size)
{	
	if (mMode == SIMPLE_RAMDISK_STREAM_READONLY)
	{
		return ERR_RAMDISK_RDONLY;
	}
	
	SimpleRamDiskBlock * nextBlock = (SimpleRamDiskBlock *) WEBC_MALLOC(sizeof(SimpleRamDiskBlock) - 1 + size);
	
	if (!nextBlock)
	{
		return ERR_RAMDISK_ALLOC_FAIL;
	}
	
	nextBlock->pNext = 0;
	nextBlock->iSize = size;
	tc_movebytes(nextBlock->pData, buffer, size);
	
	if (mpBlock)
	{
		mpFile->InsertBlock(mpBlock, nextBlock);
	}
	else
	{
		mpFile->SetFirstBlock(nextBlock);
	}

	mpBlock = nextBlock;
	
	return (size);
}

void SimpleRamDiskStream::Close ()
{
	mpFile->Close(this);
}

#endif // WEBC_SUPPORT_OFFLINE_BROWSING
