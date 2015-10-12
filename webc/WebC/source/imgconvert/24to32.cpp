//
// 24to32.CPP - 
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

#include "webgraph.hpp"
#include "24to32.hpp"
#include "webimage.hpp"
#include "util.h"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

/*****************************************************************************/
// Local Types
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// RGB24ToRGBA32 methods
/*****************************************************************************/

RGB24ToRGBA32::RGB24ToRGBA32()
{
}

RGB24ToRGBA32::~RGB24ToRGBA32()
{
}

void RGB24ToRGBA32::noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	long length = width * height;
	for (;length > 0; length--, dest+=4, source+=3)
	{
		dest[0] = source[0]; // red
		dest[1] = source[1]; // green
		dest[2] = source[2]; // blue
		dest[3] = 0;         // alpha
	}
}

void RGB24ToRGBA32::fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
}

void RGB24ToRGBA32::orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
}

/*****************************************************************************/
// RGB24ToBGRA32 methods
/*****************************************************************************/


RGB24ToBGRA32::RGB24ToBGRA32()
{
}

RGB24ToBGRA32::~RGB24ToBGRA32()
{
}

void RGB24ToBGRA32::noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	long length = width * height;
	for (;length > 0; length--, dest+=4, source+=3)
	{
		dest[0] = source[2]; // blue
		dest[1] = source[1]; // green
		dest[2] = source[0]; // red
		dest[3] = 0;         // alpha
	}
}

void RGB24ToBGRA32::fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
}

void RGB24ToBGRA32::orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
}
