//
// IMGCONV.CPP - 
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

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "webimage.hpp"
#include "webc.h"
#include "webgraph.hpp"
#include "webcmem.h"


/*****************************************************************************/
// Data
/*****************************************************************************/

const int giDitherMatrix[4][4] =  
{
	1 * 15, 9 * 15, 3 * 15, 11 * 15,
	13 *15, 5 * 15, 15 *15,  7 * 15,
	4 * 15, 12* 15, 2 * 15, 10 * 15,
	16 *15, 8 * 15, 14 *15,  6 * 15 
};


/*****************************************************************************/
// WebcBitmapConverter methods
/*****************************************************************************/

WebcBitmapConverter::WebcBitmapConverter()
{
}

WebcBitmapConverter::~WebcBitmapConverter()
{
}

void WebcBitmapConverter::go(WEBC_UCHAR8 * dest, WEBC_UCHAR8 * source, unsigned int width, unsigned int height, WebcImageDitherMode dm)
{
	switch (dm)
	{
		case WEBIMAGE_NO_DITHERING:
			noDither(dest,source,width ,height);
			break;
		case WEBIMAGE_ORDERED:
			orderedDither(dest,source,width,height);
			break;
		case WEBIMAGE_ERROR_DIFFUSION:
			fsDither(dest, source, width, height);
			break;
	}

}


/*****************************************************************************/
// FSDitherChannel methods
/*****************************************************************************/

FSDitherChannel::FSDitherChannel()
{
	error = WEBC_NULL;
	nextLine = WEBC_NULL;
	width = 0;
	pos = 0;
}

FSDitherChannel::~FSDitherChannel()
{
	WEBC_DELETE(error);
	WEBC_DELETE(nextLine);
}

void FSDitherChannel::init(int w)
{
	WEBC_DELETE(error);
	WEBC_DELETE(nextLine);
	
	WEBC_NEW(error, int[w]);
	WEBC_NEW(nextLine, int[w]);
	
	if (error)
	{
		rtp_memset(error, 0, sizeof(int) * w);
	}
	
	if (nextLine)
	{
		rtp_memset(nextLine, 0, sizeof(int) * w);
	}

	width = w;
	pos = 0;

}

void FSDitherChannel::addError(char e)
{
	int er = e;
	int erTimesTwo  = er << 1;
	int erTimesFour = er << 2;
	
	//if (pos < width - 1)
	//{
		//error[pos+1] += (er * 7) >> 4;
		//nextLine[pos+1] = er >> 4;
	//}

	if (pos == 0)
	{
		//nextLine[pos] = (er * 5) >> 4;
		nextLine[pos] = (er + erTimesFour);
	}
	else
	{
		//nextLine[pos-1] += (er * 3) >> 4;
		nextLine[pos-1] += (er + erTimesTwo);
		//nextLine[pos] += (er * 5) >> 4;
		nextLine[pos]   += (er + erTimesFour);
	}

	pos++;
	if (pos >= width)
	{
		pos = 0;
		int* swap = nextLine;
		nextLine = error;
		error = swap;
	}
	else
	{
		error[pos] += (er + erTimesTwo + erTimesFour);
		nextLine[pos] = er;
	}
}

char FSDitherChannel::getError(void)
{
	//return error[pos];
	// just divide by 16 once, when we need the value
	return error[pos] >> 4;
}
