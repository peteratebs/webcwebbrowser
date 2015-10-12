#ifndef __WGTYPES_HPP__
#define __WGTYPES_HPP__

#include "webctypes.h"
#include "cssborder.h"

typedef WEBC_UINT32 WebColor;
typedef WEBC_CHAR   WebChar;

/* value of WebBitmap that indicates this is a NULL */
#define WEB_BITMAP_NULL       0

/* value of WebFont that indicates this is a NULL */
#define WEB_FONT_NULL         0

// conversion from webchar to unicode
#if (WEBC_SUPPORT_UNICODE)
#define WEBCHAR_TO_UNICHAR(C) (C)
#else
#define WEBCHAR_TO_UNICHAR(C) ((unsigned short)((unsigned char)C))
#endif

/* This patch is needed for GNUC because the frame buffer font package
 * does sizeof() which equals 4 with GCC and 1 with visual C
 * that code needs to be fixed.
 */ 
#ifdef __GNUC__
//unsigned long    e_WebBitmapPixelFormat;
//modified to fix an issue with Code Sourcery, Code Sourcery see enum (enumerated types) as 1 byte, while Visual Studio (as well as Code Composer) see it as 4 bytes, resulting in shift in the font bitmap pointer causing the font not to be renedered correctly. Might need to check the FongGen tool as well?? later
typedef unsigned long   e_WebBitmapPixelFormat;
#define	WB_PIXEL_FORMAT_UNKNOWN		 -1 // unknown format
#define	WB_PIXEL_FORMAT_RGB_24		  0  // 24bit color, stored red-green-blue
#define	WB_PIXEL_FORMAT_INDEX_8       1  // palette indexed, 8 bits per pixel
#define	WB_PIXEL_FORMAT_MONOCHROME_8  2   // monochrome, 8 bits per pixel
#define	WB_PIXEL_FORMAT_RGBA_32       3  // 32bit color with alpha, stored red-green-blue-alpha
#define	WB_PIXEL_FORMAT_RGB_16        4  // 16bit color , stored 5red-6green-5blue
typedef unsigned long  WebBitmapPixelFormat;

#else

typedef enum e_WebBitmapPixelFormat
{
	WB_PIXEL_FORMAT_UNKNOWN      = -1, // unknown format
	WB_PIXEL_FORMAT_RGB_24       = 0,  // 24bit color, stored red-green-blue
	WB_PIXEL_FORMAT_INDEX_8      = 1,  // palette indexed, 8 bits per pixel
	WB_PIXEL_FORMAT_MONOCHROME_8 = 2,   // monochrome, 8 bits per pixel
	WB_PIXEL_FORMAT_RGBA_32      = 3,  // 32bit color with alpha, stored red-green-blue-alpha
	WB_PIXEL_FORMAT_RGB_16       = 4  // 16bit color , stored 5red-6green-5blue
}
WebBitmapPixelFormat;
#endif
typedef struct s_WebGraphBitmapInfo WebGraphBitmapInfo;

struct s_WebGraphBitmapInfo
{
	DISPLAY_INT          iWidth;
	DISPLAY_INT          iHeight;
	WEBC_SIZE            iBytesPerRow;
	WebBitmapPixelFormat pixelFormat;
	WEBC_BOOL            bHasTransparentColor;
	WEBC_INT16           iTransparentIndex;
	WEBC_UINT8           uTransparentRed;
	WEBC_UINT8           uTransparentGreen;
	WEBC_UINT8           uTransparentBlue;
	WEBC_UINT32          iPaletteSize;
	const WEBC_UINT8*    pPalette;
	const WEBC_UINT8*    pData;
};

struct s_WebBitmap
{
	DISPLAY_INT          width;
	DISPLAY_INT          height;
	void*                bitmap;
};

typedef struct s_WebBitmap* WebBitmap;

#define WEB_BITMAP_WIDTH(X)  ((X)->width)
#define WEB_BITMAP_HEIGHT(X) ((X)->height)

struct s_WebFont
{
	DISPLAY_INT          height;
	DISPLAY_INT          baseline;
	void*                font;
};

typedef struct s_WebFont* WebFont;

#define WEB_FONT_HEIGHT(X)   ((X)->height)
#define WEB_FONT_BASELINE(X) ((X)->baseline)

#endif // __WGTYPES_HPP__
