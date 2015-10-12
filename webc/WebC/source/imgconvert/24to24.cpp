//
// 24to24.CPP - 
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
#include "24to24.hpp"
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
// RGB24ToRGB24 methods
/*****************************************************************************/

RGB24ToRGB24::RGB24ToRGB24()
{
}

RGB24ToRGB24::~RGB24ToRGB24()
{
}

void RGB24ToRGB24::noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	long length = width * height * 3;
	tc_movebytes(dest, source, length);
}

void RGB24ToRGB24::fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
}

void RGB24ToRGB24::orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
}


/*****************************************************************************/
// RGB24ToBGR24 methods
/*****************************************************************************/


RGB24ToBGR24::RGB24ToBGR24()
{
}

RGB24ToBGR24::~RGB24ToBGR24()
{
}

void RGB24ToBGR24::noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	long length = width * height;
	for (;length > 0; length--, dest+=3, source+=3)
	{
		dest[0] = source[2];
		dest[1] = source[1];
		dest[2] = source[0];
	}
}

void RGB24ToBGR24::fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
}

void RGB24ToBGR24::orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
}
