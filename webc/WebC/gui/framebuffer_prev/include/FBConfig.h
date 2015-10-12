#ifndef __FBCONFIG_H__
#define __FBCONFIG_H__

#include "webc.h"

#ifndef WIN32
#ifndef __TIARM__
/* Declare your target name here */
#define OLIMEX
#endif
#endif

#ifdef WIN32
#define CONFIG_DEMO_MODE 1
#else
#define CONFIG_DEMO_MODE 0
#endif

// Test conditions windows
// Choose RENDER8BITINDEX, RENDERRGB24 or RENDERRGBA
#ifdef WIN32
#define RENDER8BITINDEX 0  // Set to one to display in 8 bit indexed.
#define RENDERRGB24     1  // Set to one to display in RGB24.
#define RENDERRGBA      0  // Set to one to display in RGB24 Plus alpha (not done).
#define CONFIG_INITIALIZED
#endif

#ifdef OLIMEX
#define RENDER8BITINDEX 0  // Set to one to display in 8 bit indexed.
#define RENDERRGB24     1  // Set to one to display in RGB24.
#define RENDERRGBA      0  // Set to one to display in RGB24 Plus alpha (not done).
#define HORIZONTAL_RESOLUTION    320
#define VERTICAL_RESOLUTION      240
#define CONFIG_INITIALIZED
#endif

#ifdef __TIARM__
#define RENDER8BITINDEX 1  // Set to one to display in 8 bit indexed.
#define RENDERRGB24     0  // Set to one to display in RGB24.
#define RENDERRGBA      0  // Set to one to display in RGB24 Plus alpha (not done).
#define HORIZONTAL_RESOLUTION    320
#define VERTICAL_RESOLUTION      240
#define CONFIG_INITIALIZED
#endif

#ifndef CONFIG_INITIALIZED
#error Bit map driver must be configured
#endif

#define MAPRGBTORGB  0  							// Windows FB only. Set to one to display in RGB with no conversion
#define MAPRGBTOGRAY (0&&RENDERRGB24)  			// Windows FB only. Set to one to display in RGB but convert to GRAY when displaying
#define MAPINDEXED8TOINDEXED4 (1&&RENDER8BITINDEX)  // Windows FB only. Set to display in 8 bit index but convert to 4 bit GRAY when displaying

#if (MAPINDEXED8TOINDEXED4)
#define BYTES_PER_PIXEL	 1
#else
#define BYTES_PER_PIXEL	 4
#endif

#define WEBC_BYTES_PER_ROW (BYTES_PER_PIXEL*WEBC_SCREEN_WIDTH) 

#define USE_DOUBLE_BUFFERING 1
#define BUFFER_INVALID_REGIONS_ONLY 1



// Prototyped here because config is included in ever cpp file.
WEBC_UINT8 Rgbto4BitGray(WEBC_UINT8 r, WEBC_UINT8 g, WEBC_UINT8 b);


#ifdef WIN32
//#include "WindowsBitMapFrameBuffer.h"
#else
#ifdef __TIARM__
#include "LocostoFrameBuffer.h"
#else
#include "EmbeddedBitMapFrameBuffer.h"
#endif
#endif

#endif
