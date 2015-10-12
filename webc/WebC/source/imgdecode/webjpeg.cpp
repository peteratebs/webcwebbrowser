//
// WEBJPEG.CPP -
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

#if (WEBC_SUPPORT_JPGS)

#include "webgraph.hpp"
#include "webjpeg.hpp"
#include "setjmp.h"
#include "webcbmp.hpp"
#include "webcmem.h"

/*****************************************************************************/
// Local functions
/*****************************************************************************/
extern "C"{
void WebJPEGInitSource(j_decompress_ptr info);  // callback for libjpeg
JPEG_BOOL WebJPEGFillInputBuffer(j_decompress_ptr info);  // callback for libjpeg
void WebJPEGSkipInputData(j_decompress_ptr info, long num); // callback for libjpeg
void WebJPEGTerminateSource(j_decompress_ptr info);     // callback for libjpeg
void WebJPEGFatalError(j_common_ptr c);
JPEG_BOOL WebJPEGResyncToRestart (j_decompress_ptr cinfo, int desired);
}

/*****************************************************************************/
// WebcJPEGDecoder methods
/*****************************************************************************/

WebcJPEGDecoder::WebcJPEGDecoder()
{
	mpImageData = WEBC_NULL;
	meDitherMode = WEBIMAGE_ERROR_DIFFUSION;
}

WebcJPEGDecoder::~WebcJPEGDecoder()
{
	if (mpImageData)
	{
		WEBC_DELETE(mpImageData);
	}
}

WebcImage * WebcJPEGDecoder::go(WEBC_CHAR *bitmapKey)
{
	if (!mpDataBuffer)
	{
		meState = WEBIMAGE_MALLOC_ERROR;
		return (0);
	}

	WEBC_UCHAR8 ** buffer = WEBC_NULL;
	WebcImage * ret = WEBC_NULL;
	WebBitmap bmp = WEBC_NULL;

	jpeg_decompress_struct cinfo;

	WebJPEGErrorHandlerStruct jerr;
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = WebJPEGFatalError;

	jpeg_create_decompress(&cinfo);

	if (InitSource(&cinfo) < 0)
	{
		jpeg_destroy_decompress(&cinfo);
		meState = WEBIMAGE_MALLOC_ERROR;
		return WEBC_NULL;
	}

	if (setjmp(jerr.jumpBuffer) || setjmp(mLongJump))
	{
		jpeg_destroy_decompress(&cinfo);
		FreeSource(&cinfo);
		meState = WEBIMAGE_FORMAT_ERROR;
		if (mpImageData)
		{
			WEBC_DELETE(mpImageData);
			mpImageData = WEBC_NULL;
		}
		if (buffer)
		{
			WEBC_DELETE(/*[]*/ buffer);  // fix the WEBC_DELETE macro to take []
			buffer = WEBC_NULL;
		}
		return ret;
	}
	mcLongJumpValid = 1;

	jpeg_read_header(&cinfo, TRUE);

	cinfo.dct_method = JDCT_IFAST;
	cinfo.do_fancy_upsampling = FALSE;
	//cinfo.out_color_space = WEBIMAGE_IS_GREYSCALE(meColorDepth) ? JCS_GRAYSCALE : JCS_RGB;
	cinfo.out_color_space = JCS_RGB;


	jpeg_start_decompress(&cinfo);

	muWidth = cinfo.output_width;
	muHeight = cinfo.output_height;

	WEBC_NEW_SIZED(buffer, WEBC_UCHAR8*[cinfo.output_height],(sizeof(WEBC_UCHAR8)*cinfo.output_height));
	WEBC_NEW_SIZED(mpImageData, WEBC_UCHAR8[muWidth * muHeight * cinfo.output_components],(muWidth * muHeight * cinfo.output_components));
	if (!buffer || !mpImageData)
	{
		error(WEBIMAGE_MALLOC_ERROR);
	}

	for (unsigned int y = 0; y < cinfo.output_height; y++)
	{
		buffer[y] = mpImageData + y * muWidth * cinfo.output_components;
	}
	while (cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, buffer + cinfo.output_scanline, cinfo.output_height - cinfo.output_scanline);
	}

	WEBC_DELETE(/*[]*/ buffer);  // fix the WEBC_DELETE macro to take []
	buffer = WEBC_NULL;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);


	WebGraphBitmapInfo info = {
			muWidth,                     // iWidth
			muHeight,                    // iHeight;
			muWidth * 3,                 // iBytesPerRow;
			WB_PIXEL_FORMAT_RGB_24,      // pixelFormat;
			0,                           // bHasTransparentColor;
			0,                           // iTransparentIndex;
			0,                           // uTransparentRed;
			0,                           // uTransparentGreen;
			0,                           // uTransparentBlue;
			0,                           // iPaletteSize;
			0,                           // pPalette;
			mpImageData                  // pData;
		};

	bmp = mpGraphicsContext->CreateWebBitmap(&info);

	if (!bmp)
	{
		error(WEBIMAGE_MALLOC_ERROR);
	}

	WEBC_DELETE(mpImageData);
	mpImageData = WEBC_NULL;

	FreeSource(&cinfo);

	WEBC_NEW_VERBOSE(ret, WebcBitmapImage(mpGraphicsContext, bmp, bitmapKey),"png:WebcBitmapImage");
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

void WebcJPEGDecoder::FreeSource(j_decompress_ptr decom)
{
	WEBC_DELETE(decom->src);
}

int WebcJPEGDecoder::InitSource(j_decompress_ptr decom)
{
	WebJPEGSourceStruct *s = (WebJPEGSourceStruct*)decom->src;
	if (decom->src == WEBC_NULL)
	{
		WEBC_NEW_VERBOSE(s, WebJPEGSourceStruct,"WebJPEGSourceStruct");
		if (!s)
		{
			return (-1);
		}
		decom->src = (jpeg_source_mgr *) s;
	}
	s->jpeg = this;
	decom->src->init_source = WebJPEGInitSource;
	decom->src->fill_input_buffer = WebJPEGFillInputBuffer;
	decom->src->skip_input_data = WebJPEGSkipInputData;
	decom->src->term_source = WebJPEGTerminateSource;
	decom->src->resync_to_restart = WebJPEGResyncToRestart;

	return (0);
}


void WebJPEGInitSource(j_decompress_ptr info)  // callback for libjpeg
{
	WebJPEGSourceStruct * ss = (WebJPEGSourceStruct*)info->src;
	ss->pub.bytes_in_buffer = 0;
}

JPEG_BOOL WebJPEGFillInputBuffer(j_decompress_ptr info)  // callback for libjpeg
{
	WebJPEGSourceStruct * ss = (WebJPEGSourceStruct*)info->src;
	WebcJPEGDecoder * jpeg = ss->jpeg;
	jpeg->movePointer(jpeg->bytesAvailable());
	jpeg->fillBuffer(WEBIMAGE_BUFFER_SIZE);
	ss->pub.next_input_byte = jpeg->getPointer();
	ss->pub.bytes_in_buffer = jpeg->bytesAvailable();
	if (jpeg->getState() != WEBIMAGE_OKAY)
	{
		WebJPEGFatalError((j_common_ptr)&info);
	}
	return TRUE;
}

void WebJPEGSkipInputData(j_decompress_ptr info, long num) // callback for libjpeg
{
	WebJPEGSourceStruct * ss = (WebJPEGSourceStruct*)info->src;
	WebcJPEGDecoder * jpeg = ss->jpeg;
	jpeg->movePointer(num + (unsigned char*)ss->pub.next_input_byte - (unsigned char *)jpeg->getPointer());
	ss->pub.next_input_byte = jpeg->getPointer();
	ss->pub.bytes_in_buffer = jpeg->bytesAvailable();
}

void WebJPEGTerminateSource(j_decompress_ptr info)     // callback for libjpeg
{
}


void WebJPEGFatalError(j_common_ptr c)
{
	WebJPEGErrorHandlerStruct * err = (WebJPEGErrorHandlerStruct*)c->err;
	longjmp(err->jumpBuffer, 1);
}

JPEG_BOOL WebJPEGResyncToRestart (j_decompress_ptr cinfo, int desired)
{
	WebJPEGFatalError((j_common_ptr)cinfo);
	return 0;
}

#endif // WEBC_SUPPORT_JPGS
