//
// 32to24.CPP - 
//
// EBS - WebC
//
// Copyright EBS Inc. , 2009
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
#include "32to24.hpp"
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
// RGB32ToRGB24 methods
/*****************************************************************************/

RGB32AToRGB24::RGB32AToRGB24()
{
}

RGB32AToRGB24::~RGB32AToRGB24()
{
}

void RGB32AToRGB24::noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	long length = width * height;
	for (;length > 0; length--, dest+=3, source+=4)
	{
		dest[0] = source[0]; // red
		dest[1] = source[1]; // green
		dest[2] = source[2]; // blue
	}
}

void RGB32AToRGB24::fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
}

void RGB32AToRGB24::orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
}

/*****************************************************************************/
// RGBA32ToBGR24 methods
/*****************************************************************************/


RGBA32ToBGR24::RGBA32ToBGR24()
{
}

RGBA32ToBGR24::~RGBA32ToBGR24()
{
}

void RGBA32ToBGR24::noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	long length = width * height;
	for (;length > 0; length--, dest+=3, source+=4)
	{
		dest[0] = source[2]; // blue
		dest[1] = source[1]; // green
		dest[2] = source[0]; // red
	}
}

void RGBA32ToBGR24::fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
}

void RGBA32ToBGR24::orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
}
