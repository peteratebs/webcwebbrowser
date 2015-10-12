/*
|  WCSTREAM.CPP -
|
|  EBS - WebC
|
|
|  Copyright EBS Inc. , 2005
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "wcstream.hpp"
#include "webcmem.h"
#include "webcassert.h"
#include "buffer.h"
#include "htmldoc.hpp"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*===========================================================================*/
// class WebcBufferedInputStream : public WebcInputStream

/*---------------------------------------------------------------------------*/
WebcBufferedInputStream::WebcBufferedInputStream (WebcInputStream* source, long bufferSize)
{
 	inputSource = source;
  	inputBuffer = (unsigned char*) ((bufferSize > 0) ? WEBC_MALLOC (bufferSize) : 0);
 	inputBufferSize = inputBuffer ? bufferSize : 0;
 	inputBufferUsed = 0;
}

/*---------------------------------------------------------------------------*/
WebcBufferedInputStream::~WebcBufferedInputStream ()
{
	if (inputBuffer)
	{
		WEBC_FREE(inputBuffer);
	}
}

/*---------------------------------------------------------------------------*/
int WebcBufferedInputStream::resizeBuffer (long newSize)
{
	int result = buffer_alloc_at_least (
			(void**) &inputBuffer,
			&inputBufferSize,
			newSize,
			WEBC_TRUE /* preserve content */,
			WEBC_FALSE /* zero new space */);

	if (result >= 0)
	{
		inputBufferUsed = EBSMIN(inputBufferUsed, inputBufferSize);
		return SUCCESS;
	}

	return ERROR_BUFFER_ALLOC;
}

/*---------------------------------------------------------------------------*/
long WebcBufferedInputStream::read (unsigned char* buffer, long maxBytes)
{
	if (!inputBuffer)
	{
		return ERROR_BUFFER_ALLOC;
	}

	long numRead = 0;

	while (maxBytes > 0)
	{
		if (inputBufferUsed > 0)
		{
			long copySize = EBSMIN(maxBytes, inputBufferUsed);
			rtp_memcpy(buffer, inputBuffer, copySize);
			compactBytes(copySize);
			buffer += copySize;
			maxBytes -= copySize;
			numRead += copySize;
		}

		if ((maxBytes == 0) || (fillBuffer() < 0) || (inputBufferUsed == 0))
		{
			break;
		}
	}

	return numRead;
}

/*---------------------------------------------------------------------------*/
// reads as much data from the input stream as will fit
int WebcBufferedInputStream::fillBuffer (void)
{
	if (!inputBuffer)
	{
		return ERROR_BUFFER_ALLOC;
	}

	WEBC_ASSERT(inputBufferUsed <= inputBufferSize);

	if (inputBufferUsed < inputBufferSize)
	{
		long readStatus = inputSource->read(inputBuffer + inputBufferUsed, inputBufferSize - inputBufferUsed);
		if (readStatus < 0)
		{
			return readStatus;
		}
		inputBufferUsed += readStatus;
	}

	return SUCCESS;
}

/*---------------------------------------------------------------------------*/
// throws out the given number of bytes at the beginning of
//  the buffer and moves remaining data to the beginning.
int WebcBufferedInputStream::compactBytes (long byteCount)
{
	if (byteCount <= 0)
	{
		return ERROR_BAD_ARGS;
	}

	if (inputBuffer)
	{
		if (inputBufferUsed <= byteCount)
		{
			inputBufferUsed = 0;
		}
		else
		{
			inputBufferUsed -= byteCount;
			rtp_memmove(inputBuffer, inputBuffer + byteCount, inputBufferUsed);
		}

		return SUCCESS;
	}

	return ERROR_BUFFER_ALLOC;
}

/*===========================================================================*/
// class WebcSeededInputStream : public WebcInputStream

/*---------------------------------------------------------------------------*/
WebcSeededInputStream::WebcSeededInputStream (WebcInputStream* fallback, const unsigned char* data, long length)
{
 	fallbackInputStream = fallback;
 	seedData = data;
	seedLength = length;
}

/*---------------------------------------------------------------------------*/
//  reads the data from the seed buffer first, then reads directly off the
//   fallback stream
long WebcSeededInputStream::read (unsigned char* buffer, long maxBytes)
{
	long seedRead = 0;

	if (maxBytes <= 0)
	{
		return ERROR_BAD_ARGS;
	}

	if (seedLength > 0)
	{
		long copySize = EBSMIN(maxBytes, seedLength);
		rtp_memcpy(buffer, seedData, copySize);
		seedData += copySize;
		seedLength -= copySize;
		buffer += copySize;
		maxBytes -= copySize;
		seedRead += copySize;

		if (maxBytes == 0)
		{
			return seedRead;
		}
	}

	return fallbackInputStream->read(buffer, maxBytes) + seedRead;
}

/*===========================================================================*/
// class WebcURLInputStream : public WebcInputStream

/*---------------------------------------------------------------------------*/
WebcURLInputStream::WebcURLInputStream (UrlStreamCtx* source)
{
	urlContext = source;
}

/*---------------------------------------------------------------------------*/
// this class is just an adapter from the old url stream context interface to
//  the new WebcInputStream design
long WebcURLInputStream::read (unsigned char* buffer, long maxBytes)
{
	return wload_Read(urlContext, (char*) buffer, maxBytes);
}

/*===========================================================================*/
// class WebcFilteredInputStream : public WebcBufferedInputStream

/*---------------------------------------------------------------------------*/
WebcFilteredInputStream::WebcFilteredInputStream (WebcDataFilter* filter, WebcInputStream* source, long bufferSize)
	: WebcBufferedInputStream (source, bufferSize)
{
	inputFilter = filter;
}

/*---------------------------------------------------------------------------*/
// runs data from the source stream through the filter first
long WebcFilteredInputStream::read (unsigned char* buffer, long maxBytes)
{
	long bytesRead = 0;
	long rv;

	WEBC_ASSERT(inputFilter);

	if (maxBytes <= 0)
	{
		return ERROR_BAD_ARGS;
	}

	while (bytesRead < maxBytes)
	{
		if ((rv = fillBuffer()) < 0)
		{
			return (bytesRead == 0) ? rv : bytesRead;
		}

		if (inputBufferUsed == 0)
		{
			return (bytesRead == 0) ? ERROR_END_OF_FILE : bytesRead;
		}

		WebcDataFilterResult r = inputFilter->go (buffer + bytesRead, maxBytes - bytesRead,
		                                          inputBuffer, inputBufferUsed);
		if (r.bytesRead == 0)
		{
			break;
		}

		compactBytes(r.bytesRead);
		bytesRead += r.bytesWritten;
	}

	return bytesRead;
}

/*===========================================================================*/
// class WebcHTMLDocumentOutputStream : public WebcOutputStream

long WebcHTMLDocumentOutputStream::write (const unsigned char* data, long numBytes)
{
	return document->Write((WEBC_CHAR*) data, numBytes / sizeof(WEBC_CHAR));
}

/*===========================================================================*/
// class WebcBufferedOutputStream : public WebcOutputStream

WebcBufferedOutputStream::WebcBufferedOutputStream (WebcOutputStream* sink, long bufferSize)
{
	outputStream = sink;
	outputBuffer = (bufferSize > 0) ? (unsigned char*) WEBC_MALLOC(bufferSize) : 0;
	outputBufferSize = outputBuffer ? bufferSize : 0;
	outputBufferUsed = 0;
}

WebcBufferedOutputStream::~WebcBufferedOutputStream ()
{
	if (outputBuffer)
	{
		WEBC_FREE(outputBuffer);
	}
}

long WebcBufferedOutputStream::write (const unsigned char* data, long numBytes)
{
	long bytesWritten = 0;

	if (!outputBuffer)
	{
		return ERROR_BUFFER_ALLOC;
	}

	if (numBytes <= 0)
	{
		return ERROR_BAD_ARGS;
	}

	while (bytesWritten < numBytes)
	{
		if (outputBufferUsed == outputBufferSize)
		{
			int err;
			if ((err = flush()) < 0)
			{
				return err;
			}
		}

		if (outputBufferUsed == 0 && numBytes - bytesWritten > outputBufferSize)
		{
			// we're just going to copy and flush n times anyhow, so save the trouble
			//  by just copying the last remaining MOD outputBufferSize bytes and
			//  writing the rest directly
			long bytesRemaining = numBytes - bytesWritten;
			long writeSize = bytesRemaining - bytesRemaining % outputBufferSize;
			long result = outputStream->write(data + bytesWritten, writeSize);
			if (result < 0)
			{
				break;
			}
			bytesWritten += result;
			if (result < writeSize)
			{
				break;
			}
		}
		else
		{
			long toCopy = EBSMIN(outputBufferSize - outputBufferUsed, numBytes - bytesWritten);
			rtp_memcpy(outputBuffer + outputBufferUsed, data + bytesWritten, toCopy);
			outputBufferUsed += toCopy;
			bytesWritten += toCopy;
		}
	}

	return bytesWritten;
}

int WebcBufferedOutputStream::flush (void)
{
	if (!outputBuffer)
	{
		return ERROR_BUFFER_ALLOC;
	}

	long rv;
	if ((rv = outputStream->write(outputBuffer, outputBufferUsed)) != outputBufferUsed)
	{
		return (int) rv;
	}

	outputBufferUsed  = 0;

	return SUCCESS;
}

/*===========================================================================*/
// class WebcFilteredOutputStream : public WebcBufferedOutputStream

WebcFilteredOutputStream::WebcFilteredOutputStream (WebcDataFilter* filter, WebcOutputStream* sink, long bufferSize)
	: WebcBufferedOutputStream (sink, bufferSize)
{
 	outputFilter = filter;
}

long WebcFilteredOutputStream::write (const unsigned char* data, long numBytes)
{
	long bytesProcessed = 0;

	if (!outputBuffer)
	{
		return ERROR_BUFFER_ALLOC;
	}

	if (numBytes <= 0)
	{
		return ERROR_BAD_ARGS;
	}

	while (bytesProcessed < numBytes)
	{
		if (outputBufferSize == outputBufferUsed)
		{
			int err;
			if ((err = flush()) < 0)
			{
				return err;
			}
		}

		WebcDataFilterResult r = outputFilter->go (outputBuffer + outputBufferUsed,
		                                           outputBufferSize - outputBufferUsed,
		                                           data + bytesProcessed, numBytes - bytesProcessed);

		if (r.bytesRead == 0 && r.bytesWritten == 0)
		{
			// the filter didn't make any progress
			if (outputBufferUsed > 0)
			{
				// maybe we can get it unstuck by flushing the buffer
				int err;
				if ((err = flush()) < 0)
				{
					return err;
				}
			}
			else
			{
				// we are simply stuck - buffer is too small
				return ERROR_BUFFER_TOO_SMALL;
			}
		}

		bytesProcessed += r.bytesRead;
		outputBufferUsed += r.bytesWritten;

		// if this assertion fails, the filter implementation is bad
		WEBC_ASSERT(outputBufferUsed < outputBufferSize);
	}

	return bytesProcessed;
}
/* Return the socket number associated with a stream used when fetching web pages using the loopback interface in polled mode. */
extern "C" int StreamToSocket(void *pStream)
{
    WebcSocketStream* s = (WebcSocketStream*) pStream;
    return s->socket;
 }
