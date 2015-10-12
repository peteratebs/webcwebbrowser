/*
|  WEBBMP.CPP -
|
|  EBS - WebC
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "webgraph.hpp"
#include "webbmp.hpp"
#include "webcmem.h"
#include "webcbmp.hpp"

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

/*---------------------------------------------------------------------------*/
WebcBMPDecoder::WebcBMPDecoder ()
{
	mImageData = 0;
}

/*---------------------------------------------------------------------------*/
WebcBMPDecoder::~WebcBMPDecoder ()
{
}

#define BUFFER_SIZE 4096

/*---------------------------------------------------------------------------*/
WebcImage* WebcBMPDecoder::go (WEBC_CHAR* key)
{
	WebcImage* image = 0;
	WEBC_UINT8* data = (WEBC_UINT8*) WEBC_MALLOC_VERBOSE(BUFFER_SIZE,"BmpBuffer");
	WEBC_INT32 bytesBuffered = 0;
	WEBC_INT32 bytesRead = 0;
	WEBC_INT32 bytesReceived;
	int status = BMP_NEED_DATA;

	if (!data)
	{
		return 0;
	}

	BMP_DecodeInit(&mDecoder);

	do
	{
		if (bytesBuffered > 0)
		{
			// move unread data to the front of the buffer
			rtp_memmove(data, data + bytesRead, bytesBuffered);
		}

		// now read more data into the buffer (from a socket)
		bytesReceived = mfCallback((char*)data + bytesBuffered, BUFFER_SIZE - bytesBuffered, mpCallbackContext);
		if (bytesReceived <= 0)
		{
			break;
		}

		bytesBuffered += bytesReceived;

		// decode new data
		bytesRead = BMP_DecodeData (&mDecoder, data, bytesBuffered, &status);

		if (bytesRead < 0)
		{
			// unknown condition
			break;
		}

		bytesBuffered -= bytesRead;

		if (status == BMP_NEED_IMAGE_BUFFER)
		{
			WEBC_INT32 size = BMP_DecodeGetBufferSize(&mDecoder);
			if (size <= 0)
			{
				break;
			}
			mImageData = (WEBC_UINT8*) WEBC_MALLOC_VERBOSE(size,"BmpImageData");
			if (!mImageData)
			{
				break;
			}
			BMP_DecodeSetImageBuffer(&mDecoder, mImageData);

			// decode new data
			bytesRead = BMP_DecodeData (&mDecoder, data + bytesRead, bytesBuffered, &status);

			if (bytesRead < 0)
			{
				// unknown condition
				break;
			}

			bytesBuffered -= bytesRead;
		}
	}
	while (status == BMP_NEED_DATA);

	if (mDecoder.state == BMP_DONE)
	{
		WebBitmapPixelFormat pixelFormat;
		WEBC_INT32 bytesPerRow = BMP_DecodeGetBytesPerRow(&mDecoder);
		WEBC_UINT8 palette[256*3];
		unsigned int c;

		for (c = 0; c < mDecoder.info.ncolors; c++)
		{
			palette[c*3 + 0] = mDecoder.palette[c].red;
			palette[c*3 + 1] = mDecoder.palette[c].green;
			palette[c*3 + 2] = mDecoder.palette[c].blue;
		}

		switch (mDecoder.info.bits)
		{
			case 24:
				pixelFormat = WB_PIXEL_FORMAT_RGB_24;
				break;
#if (WEBC_SUPPORT_BMP_16BITS)
			case 16:
				pixelFormat = WB_PIXEL_FORMAT_RGB_16;
				break;
#endif
			default:
				pixelFormat = WB_PIXEL_FORMAT_INDEX_8;
				break;
		}

		WebGraphBitmapInfo info = {
				mDecoder.info.width,         // iWidth
				mDecoder.info.height,        // iHeight;
				bytesPerRow,                 // iBytesPerRow;
				pixelFormat,                 // pixelFormat;
				WEBC_FALSE,                  // bHasTransparentColor;
				0,                           // iTransparentIndex;
				0,                           // uTransparentRed;
				0,                           // uTransparentGreen;
				0,                           // uTransparentBlue;
				mDecoder.info.ncolors,       // iPaletteSize;
				palette,                     // pPalette;
				mImageData                   // pData;
			};

		WebBitmap bmp = mpGraphicsContext->CreateWebBitmap(&info);

		if (bmp)
		{
			WEBC_NEW_VERBOSE(image, WebcBitmapImage(mpGraphicsContext, bmp, key),"WebcBMPDecoder::WebcBitmapImage");
			if (!image)
			{
				mpGraphicsContext->DestroyWebBitmap(bmp);
				bmp = 0;
			}
			else
			{
				image->SetState(getState());
			}
		}
	}

	BMP_DecodeDestroy(&mDecoder);

	WEBC_FREE(mImageData);
	mImageData = WEBC_NULL;

	WEBC_FREE(data);

	return image;
}
