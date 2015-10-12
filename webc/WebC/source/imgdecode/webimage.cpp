//
// WEBIMAGE.CPP - Methods for class WebcImageDecoder
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

#include "webimage.hpp"
#include "webgraph.hpp"
#include "util.h"
#include "webcassert.h"
#include "webcmem.h"


/*****************************************************************************/
// Macros
/*****************************************************************************/

/*****************************************************************************/
// Global data
/*****************************************************************************/

/*****************************************************************************/
// WebImageDecompress methods
/*****************************************************************************/

WebcImageDecoder::WebcImageDecoder()
{
	WEBC_NEW_SIZED(mpDataBuffer, WEBC_UCHAR8[WEBIMAGE_BUFFER_SIZE],WEBIMAGE_BUFFER_SIZE);
	mpReadPtr = mpDataBuffer;
	muDataBytesAvailable = 0;
	meState = WEBIMAGE_OKAY;
	
	setDitherMode(WEBIMAGE_NO_DITHERING);
	setColorDepth(WEBIMAGE_INDEXED);
	setGraphics(WEBC_NULL);
	mcLongJumpValid = 0;
}

WebcImageDecoder::WebcImageDecoder(ImageStreamRead  cb, void * context)
{
	WEBC_NEW_SIZED(mpDataBuffer, WEBC_UCHAR8[WEBIMAGE_BUFFER_SIZE],WEBIMAGE_BUFFER_SIZE);
	setReadCallback(cb,context);
	mpReadPtr = mpDataBuffer;
	muDataBytesAvailable = 0;
	meState = WEBIMAGE_OKAY;

	setDitherMode(WEBIMAGE_NO_DITHERING);
	setColorDepth(WEBIMAGE_INDEXED);
	setGraphics(WEBC_NULL);
	mcLongJumpValid = 0;
}

WebcImageDecoder::~WebcImageDecoder()
{
	WEBC_DELETE(mpDataBuffer);
}

void WebcImageDecoder::setDitherMode(WebcImageDitherMode d)
{
	meDitherMode = d;
}

void WebcImageDecoder::setColorDepth(WebcImageColorDepth d)
{
	meColorDepth = d;
}

void WebcImageDecoder::setGraphics(WebGraphics * g)
{
	mpGraphicsContext = g;
}

void WebcImageDecoder::setReadCallback(ImageStreamRead cb, void * context)
{
	mfCallback = cb;
	mpCallbackContext = context;
}

long WebcImageDecoder::readAtLeast(WEBC_UCHAR8 * data, long maxSize, long num)
{
	long numRead = mfCallback((char*)data, maxSize, mpCallbackContext);
	if (numRead < 0)
	{
		return numRead;
	}
	while (numRead < num)
	{
		int n = mfCallback((char*)(data + numRead), maxSize - numRead, mpCallbackContext);
		if (n < 0)
		{
			return n;
		}
		if (n == 0)
		{
			break;
		}
		numRead += n;
	}
	return numRead;
}
	
void WebcImageDecoder::fillBuffer(int bytes)
{
	// at least this many bytes are already in the buffer; return
	if (bytes < 0)
	{
		return;
	}	

	if (muDataBytesAvailable >= (unsigned long) bytes)
	{
		return;
	}
	
	// move unread data to the beginning of the buffer and read new data
	rtp_memmove(mpDataBuffer, mpReadPtr, muDataBytesAvailable);
	mpReadPtr = mpDataBuffer;
	long n = readAtLeast(
					mpDataBuffer + muDataBytesAvailable, 
					WEBIMAGE_BUFFER_SIZE - muDataBytesAvailable, 
					WEBIMAGE_BUFFER_SIZE - muDataBytesAvailable
					);
					
	if (n < 0)
	{
		error(WEBIMAGE_READ_ERROR);
	}
	else
	{
		muDataBytesAvailable += n;
	}
}

void WebcImageDecoder::error(WebcImageState s)
{
	meState = s;
	if (mcLongJumpValid)
	{
		mcLongJumpValid = 0;
		longjmp(mLongJump,1);
	}
}

WEBC_UCHAR8 WebcImageDecoder::getNext(void)
{
	if (muDataBytesAvailable == 0)
	{
		fillBuffer(256);
		if (muDataBytesAvailable == 0)
		{
			meState = WEBIMAGE_READ_ERROR;
			return 0;
		}
	}
	muDataBytesAvailable--;
	return *mpReadPtr++;
}

WEBC_UCHAR8 WebcImageDecoder::peekNext(void)
{
	if (muDataBytesAvailable == 0)
	{
		fillBuffer(256);
		if (muDataBytesAvailable == 0)
		{
			meState = WEBIMAGE_READ_ERROR;
			return 0;
		}
	}
	return *mpReadPtr;
}

void WebcImageDecoder::movePointer(WEBC_UINT32 s)
{
	WEBC_ASSERT(!(s & 0x80000000));
	if (s > 0)
	{
		WEBC_UINT32 advanceBy = EBSMIN(s, muDataBytesAvailable);
		do
		{
			mpReadPtr += advanceBy;
			muDataBytesAvailable -= advanceBy;
			s -= advanceBy;
			fillBuffer(WEBIMAGE_BUFFER_SIZE);
			advanceBy = EBSMIN(s, muDataBytesAvailable);
		}
		while (advanceBy != 0);
	}
}
