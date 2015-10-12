//
// WEBPNG.CPP -
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

#if (WEBC_SUPPORT_PNG)

#include "webgraph.hpp"
#include "webc.h"
#include "webpng.hpp"
#include "webcbmp.hpp"
#include "webcassert.h"
#include "webcmem.h"


/*****************************************************************************/
// Local functions
/*****************************************************************************/

void WebPNGRead(png_structp png, png_bytep data, png_size_t size);


/*****************************************************************************/
// WebcPNGDecoder methods
/*****************************************************************************/

WebcPNGDecoder::WebcPNGDecoder()
{
	mpPngInfo = 0;
	mpImageData = 0;
	mpPngData = png_create_read_struct(PNG_LIBPNG_VER_STRING,
					 WEBC_NULL, WEBC_NULL, WEBC_NULL);

	if (mpPngData)
	{
		mpPngInfo = png_create_info_struct(mpPngData);
		png_set_read_fn(mpPngData, (void*)this, (png_rw_ptr) &WebPNGRead);
	}
}

WebcPNGDecoder::~WebcPNGDecoder()
{
	if (mpPngData || mpPngInfo)
	{
		png_destroy_read_struct(&mpPngData, &mpPngInfo, WEBC_NULL);
	}

}

WebcImage * WebcPNGDecoder::go(WEBC_CHAR *bitmapKey)
{
	if (!mpDataBuffer || !mpPngInfo || !mpPngData)
	{
		meState = WEBIMAGE_MALLOC_ERROR;
		return (0);
	}
	
	png_color_struct *pal = 0;
	int palSize = 0;
	long bytesPerRow = 0;
	int transparentColor = -1;
	WEBC_UINT8 uTransparentRed(0), uTransparentGreen(0), uTransparentBlue(0);
	WEBC_BOOL bHasTransparentColor(0);
	WebcImage * ret = WEBC_NULL;
	WebBitmap bmp = WEBC_NULL;
	WebBitmapPixelFormat pixelFormat;
	WEBC_UCHAR8 ** row = WEBC_NULL;

	if (setjmp(png_jmpbuf(mpPngData)) || setjmp(mLongJump))
	{
		png_destroy_read_struct(&mpPngData, &mpPngInfo, WEBC_NULL);

		if (row)
		{
			WEBC_DELETE(row);
			row = WEBC_NULL;
		}

		if (mpImageData)
		{
			WEBC_DELETE(mpImageData);
			mpImageData = WEBC_NULL;
		}

		return ret;
	}
	mcLongJumpValid = 1;
	
	unsigned long w,h;
	
	png_read_info(mpPngData, mpPngInfo);
	
	png_get_IHDR(mpPngData, mpPngInfo, &w, &h, &miDepth,
		     &miColorType, WEBC_NULL, WEBC_NULL, WEBC_NULL);
	
	miWidth = w; 
	miHeight = h;
	
	miNumChannel = png_get_channels(mpPngData, mpPngInfo);
	png_set_interlace_handling(mpPngData);
	if (miColorType & PNG_COLOR_MASK_ALPHA)
	{
//		png_set_strip_alpha(mpPngData);
	}
	
	if (miDepth == 16)
	{
		png_set_strip_16(mpPngData);
	}
	if (miDepth < 8)
	{
		png_set_packing(mpPngData);
	}
	if (miColorType == PNG_COLOR_TYPE_GRAY || 
	    miColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		png_set_gray_to_rgb(mpPngData);
	}
	if (miColorType == PNG_COLOR_TYPE_PALETTE)
	{
		png_read_update_info(mpPngData, mpPngInfo);
		WEBC_NEW_SIZED(row, WEBC_UCHAR8*[miHeight],(sizeof(WEBC_UCHAR8)*miHeight));
		WEBC_NEW_SIZED(mpImageData, WEBC_UCHAR8[miWidth*miHeight*3],(miWidth*miHeight*3));

		if (!row || !mpImageData)
		{
			error(WEBIMAGE_MALLOC_ERROR);
		}
		
		for (int t = 0; t < miHeight; t++)
		{
			row[t] = &mpImageData[t*miWidth];
		}
		
		png_read_image(mpPngData, row);
		png_get_PLTE(mpPngData, mpPngInfo, &pal, &palSize);
		WEBC_ASSERT(palSize <= 256);
		WEBC_ASSERT(sizeof(png_color_struct) == 3);
		
		if (mpPngInfo->num_trans > 0)
		{
			WEBC_UCHAR8* pImageData2;
			WEBC_NEW_SIZED(pImageData2, WEBC_UCHAR8[miWidth*miHeight*4],(miWidth*miHeight*4));
			for (int t = 0; t < miHeight; t++)
			{
				WEBC_UCHAR8* rowSrc = row[t];
				WEBC_UCHAR8* rowDest = &pImageData2[t*miWidth*4];
				for (int u = 0; u<miWidth; ++u )
				{
					WEBC_UCHAR8 palIndex = *rowSrc;
					if (palIndex<palSize)
				{
						*rowDest++ = pal[palIndex].red;
						*rowDest++ = pal[palIndex].green;
						*rowDest++ = pal[palIndex].blue;
						*rowDest++ = mpPngInfo->trans[palIndex];
					}
					else
					{
						*rowDest++ = 0;
						*rowDest++ = 0;
						*rowDest++ = 0;
						*rowDest++ = 0;
					}
					rowSrc++;
				}
			}
			WEBC_DELETE(mpImageData);
			mpImageData = pImageData2;
			pixelFormat = WB_PIXEL_FORMAT_RGBA_32;
			pal = NULL;
			palSize = 0;
		}
		else
		{
		pixelFormat = WB_PIXEL_FORMAT_INDEX_8;
		}

		WEBC_DELETE(row);
		row = WEBC_NULL;
	}
	else // some sort of hicolor
	{
		int bytesPerPixel = (miColorType & PNG_COLOR_MASK_ALPHA) ? 4 : 3;
		png_read_update_info(mpPngData, mpPngInfo);
		WEBC_NEW_SIZED(row, WEBC_UCHAR8*[miHeight],(sizeof(WEBC_UCHAR8)*miHeight));
		WEBC_NEW_SIZED(mpImageData, WEBC_UCHAR8[miWidth*miHeight*bytesPerPixel],(miWidth*miHeight*bytesPerPixel));

		if (!row || !mpImageData)
		{
			error(WEBIMAGE_MALLOC_ERROR);
		}

		for (int t = 0; t < miHeight; t++)
		{
			row[t] = &mpImageData[t*miWidth*bytesPerPixel];
		}
		
		png_read_image(mpPngData, row);

	// Can't easily support binary colour transparency for 16 bit channel depth as png_set_strip_16 has reduced the
	// image to 8 bit depth and it can't be done properly after that.
		if (mpPngInfo->num_trans == 1 && miDepth!=16)
		{
			if (miColorType == PNG_COLOR_TYPE_GRAY || 
				miColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			{
			    uTransparentRed = uTransparentGreen = uTransparentBlue = (WEBC_UINT8)mpPngInfo->trans_values.gray;
			}
			else
			{
				uTransparentRed = (WEBC_UINT8)mpPngInfo->trans_values.red;
				uTransparentGreen =  (WEBC_UINT8)mpPngInfo->trans_values.green;
				uTransparentBlue =  (WEBC_UINT8)mpPngInfo->trans_values.blue;
			}
			bHasTransparentColor = 1;
		}
		pixelFormat = (miColorType & PNG_COLOR_MASK_ALPHA) ? WB_PIXEL_FORMAT_RGBA_32 : WB_PIXEL_FORMAT_RGB_24;
  
		WEBC_DELETE(row);
		row = WEBC_NULL;
	}
	
	if(pixelFormat == WB_PIXEL_FORMAT_RGB_24)
		bytesPerRow = (miWidth * 3);
	else if(pixelFormat == WB_PIXEL_FORMAT_RGBA_32)
		bytesPerRow = (miWidth * 4);
	else
		bytesPerRow = miWidth;

	WebGraphBitmapInfo info = {
			miWidth,                     // iWidth
			miHeight,                    // iHeight;
			bytesPerRow,                 // iBytesPerRow;
			pixelFormat,                 // pixelFormat;
			bHasTransparentColor,        // bHasTransparentColor;
			transparentColor,            // iTransparentIndex;
			uTransparentRed,             // uTransparentRed;
			uTransparentGreen,           // uTransparentGreen;
			uTransparentBlue,            // uTransparentBlue;
			palSize,                     // iPaletteSize;
			(WEBC_UCHAR8 *) pal,         // pPalette;
			mpImageData                  // pData;
		};			
	
	bmp = mpGraphicsContext->CreateWebBitmap(&info);

	WEBC_DELETE(mpImageData);
	mpImageData = WEBC_NULL;	

	if (!bmp)
	{
		error(WEBIMAGE_MALLOC_ERROR);
	}

	WEBC_NEW_VERBOSE(ret, WebcBitmapImage(mpGraphicsContext, bmp, bitmapKey),"PngWebcBitmapImage");
	if (!ret)
	{
		mpGraphicsContext->DestroyWebBitmap(bmp);
		bmp = 0;
	}
	else
	{
		ret->SetState(getState());
	}
	
	return ret;
}

void WebPNGRead(png_structp png, png_bytep data, png_size_t size)
{
	WebcPNGDecoder * p = (WebcPNGDecoder*) png_get_io_ptr(png);
	
	while (size > 0)
	{
		p->fillBuffer(WEBIMAGE_BUFFER_SIZE);
		if (p->getState() != WEBIMAGE_OKAY)
		{
			p->error(WEBIMAGE_READ_ERROR);
		}
		unsigned int numToCopy = EBSMIN(size, (unsigned int)(p->bytesAvailable()));
		if (numToCopy == 0)
		{
			break;
		}
		memcpy(data, p->getPointer(), numToCopy);
		p->movePointer(numToCopy);
		data += numToCopy;
		size -= numToCopy;	
	}
}

#endif // WEBC_SUPPORT_PNG
