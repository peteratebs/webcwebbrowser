//
// WGBMP.CPP - Precompiled bitmap data
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
// Header Files
/*****************************************************************************/

#include "webcfg.h"
#include "wgbmp.hpp"
#include "webctypes.h"
#include "wgtypes.hpp"

/*****************************************************************************/
// Constants
/*****************************************************************************/

#ifdef C_XX
#undef C_XX
#endif

#ifdef C_OO
#undef C_OO
#endif

#ifdef C___
#undef C___
#endif

#ifdef C_gg
#undef C_gg
#endif

#ifdef C_GG
#undef C_GG
#endif

#define C_XX    0
#define C_OO    1
#define C___    2
#define C_gg    3
#define C_GG    4


/*****************************************************************************/
// Palette shared by all bitmaps
/*****************************************************************************/

WEBC_READONLY WEBC_UINT8 gBitmapPalette[] =
{
	0x00,0x00,0x00,
	0xff,0xff,0xff,
	0xff,0x00,0x00, // transparent: make it red so its obvious if something is wrong
	0xdf,0xdf,0xdf,
	0x90,0x90,0x90
};

/*
Using these in the declaration in GNU C (vxworks) causes the structures to not be initialized
so we hard wire
#define UTransparentRed 	gBitmapPalette[C___*3+0]
#define UTransparentGreen 	gBitmapPalette[C___*3+1]
#define UTransparentBlue 	gBitmapPalette[C___*3+2]
*/

#define UTransparentRed 	0xff
#define UTransparentGreen 	0x00
#define UTransparentBlue 	0x00


/*****************************************************************************
 * Bitmap data
 *****************************************************************************/

/*---------------------------------------------------------------------------*
 * Disc bullet
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gDiscBulletBitmapData[] =
{
	C___, C_XX, C_XX, C_XX, C___,
	C_XX, C_XX, C_XX, C_XX, C_XX,
	C_XX, C_XX, C_XX, C_XX, C_XX,
	C_XX, C_XX, C_XX, C_XX, C_XX,
	C___, C_XX, C_XX, C_XX, C___
};

WEBC_READONLY WebGraphBitmapInfo gDiscBulletBitmapInfo =
{
	WGB_DISC_BULLET_WIDTH,        // iWidth
	WGB_DISC_BULLET_HEIGHT,       // iHeight
	WGB_DISC_BULLET_WIDTH,        // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,      // pixelFormat
	WEBC_TRUE,                    // bHasTransparentColor
	C___,                         // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                            // iPaletteSize
	gBitmapPalette,               // pPalette
	gDiscBulletBitmapData         // pData
};


/*---------------------------------------------------------------------------*
 * Circle bullet
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gCircleBulletBitmapData[] =
{
	C___, C___, C_XX, C_XX, C_XX, C___, C___,
	C___, C_XX, C___, C___, C___, C_XX, C___,
	C_XX, C___, C___, C___, C___, C___, C_XX,
	C_XX, C___, C___, C___, C___, C___, C_XX,
	C_XX, C___, C___, C___, C___, C___, C_XX,
	C___, C_XX, C___, C___, C___, C_XX, C___,
	C___, C___, C_XX, C_XX, C_XX, C___, C___
};

WEBC_READONLY WebGraphBitmapInfo gCircleBulletBitmapInfo =
{
	WGB_CIRCLE_BULLET_WIDTH,      // iWidth
	WGB_CIRCLE_BULLET_HEIGHT,     // iHeight
	WGB_CIRCLE_BULLET_WIDTH,      // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,      // pixelFormat
	WEBC_TRUE,                    // bHasTransparentColor
	C___,                         // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                            // iPaletteSize
	gBitmapPalette,               // pPalette
	gCircleBulletBitmapData       // pData
};


/*---------------------------------------------------------------------------*
 * Square bullet
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gSquareBulletBitmapData[] =
{
	C_XX, C_XX, C_XX, C_XX, C_XX,
	C_XX, C_XX, C_XX, C_XX, C_XX,
	C_XX, C_XX, C_XX, C_XX, C_XX,
	C_XX, C_XX, C_XX, C_XX, C_XX,
	C_XX, C_XX, C_XX, C_XX, C_XX
};

WEBC_READONLY WebGraphBitmapInfo gSquareBulletBitmapInfo =
{
	WGB_SQUARE_BULLET_WIDTH,      // iWidth
	WGB_SQUARE_BULLET_HEIGHT,     // iHeight
	WGB_SQUARE_BULLET_WIDTH,      // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,      // pixelFormat
	WEBC_TRUE,                    // bHasTransparentColor
	C___,                         // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                            // iPaletteSize
	gBitmapPalette,               // pPalette
	gSquareBulletBitmapData       // pData
};


/*---------------------------------------------------------------------------*
 * Up Arrow
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gUpArrowBitmapData[] =
{
	C___, C___, C___, C_XX, C___, C___, C___,
	C___, C___, C_XX, C_XX, C_XX, C___, C___,
	C___, C_XX, C_XX, C_XX, C_XX, C_XX, C___,
	C_XX, C_XX, C_XX, C_XX, C_XX, C_XX, C_XX,
	C___, C___, C___, C___, C___, C___, C___
};

WEBC_READONLY WebGraphBitmapInfo gUpArrowBitmapInfo =
{
	WGB_UP_ARROW_WIDTH,           // iWidth
	WGB_UP_ARROW_HEIGHT,          // iHeight
	WGB_UP_ARROW_WIDTH,           // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,      // pixelFormat
	WEBC_TRUE,                    // bHasTransparentColor
	C___,                         // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                            // iPaletteSize
	gBitmapPalette,               // pPalette
	gUpArrowBitmapData            // pData
};


/*---------------------------------------------------------------------------*
 * Down Arrow
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gDownArrowBitmapData[] =
{
	C___, C___, C___, C___, C___, C___, C___,
	C_XX, C_XX, C_XX, C_XX, C_XX, C_XX, C_XX,
	C___, C_XX, C_XX, C_XX, C_XX, C_XX, C___,
	C___, C___, C_XX, C_XX, C_XX, C___, C___,
	C___, C___, C___, C_XX, C___, C___, C___
};
WEBC_READONLY WebGraphBitmapInfo gDownArrowBitmapInfo =
{
	WGB_DOWN_ARROW_WIDTH,         // iWidth
	WGB_DOWN_ARROW_HEIGHT,        // iHeight
	WGB_DOWN_ARROW_WIDTH,         // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,      // pixelFormat
	WEBC_TRUE,                    // bHasTransparentColor
	C___,                         // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                            // iPaletteSize
	gBitmapPalette,               // pPalette
	gDownArrowBitmapData          // pData
};

/*---------------------------------------------------------------------------*
 * Left Arrow
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gLeftArrowBitmapData[] =
{
	C___, C___, C___, C_XX, C___,
	C___, C___, C_XX, C_XX, C___,
	C___, C_XX, C_XX, C_XX, C___,
	C_XX, C_XX, C_XX, C_XX, C___,
	C___, C_XX, C_XX, C_XX, C___,
	C___, C___, C_XX, C_XX, C___,
	C___, C___, C___, C_XX, C___
};

WEBC_READONLY WebGraphBitmapInfo gLeftArrowBitmapInfo =
{
	WGB_LEFT_ARROW_WIDTH,         // iWidth
	WGB_LEFT_ARROW_HEIGHT,        // iHeight
	WGB_LEFT_ARROW_WIDTH,         // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,      // pixelFormat
	WEBC_TRUE,                    // bHasTransparentColor
	C___,                         // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                            // iPaletteSize
	gBitmapPalette,               // pPalette
	gLeftArrowBitmapData          // pData
};


/*---------------------------------------------------------------------------*
 * Right Arrow
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gRightArrowBitmapData[] =
{
	C___, C_XX, C___, C___, C___,
	C___, C_XX, C_XX, C___, C___,
	C___, C_XX, C_XX, C_XX, C___,
	C___, C_XX, C_XX, C_XX, C_XX,
	C___, C_XX, C_XX, C_XX, C___,
	C___, C_XX, C_XX, C___, C___,
	C___, C_XX, C___, C___, C___
};

WEBC_READONLY WebGraphBitmapInfo gRightArrowBitmapInfo =
{
	WGB_RIGHT_ARROW_WIDTH,        // iWidth
	WGB_RIGHT_ARROW_HEIGHT,       // iHeight
	WGB_RIGHT_ARROW_WIDTH,        // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,      // pixelFormat
	WEBC_TRUE,                    // bHasTransparentColor
	C___,                         // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                            // iPaletteSize
	gBitmapPalette,               // pPalette
	gRightArrowBitmapData         // pData
};


/*---------------------------------------------------------------------------*
 * Radio On
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gRadioOnBitmapData[] =
{
	C___,C___,C___,C___,C_GG,C_GG,C_GG,C_GG,C___,C___,C___,C___,
	C___,C___,C_GG,C_GG,C_XX,C_XX,C_XX,C_XX,C_GG,C_GG,C___,C___,
	C___,C_GG,C_XX,C_XX,C_OO,C_OO,C_OO,C_OO,C_XX,C_gg,C_OO,C___,
	C___,C_GG,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_gg,C_OO,C___,
	C_GG,C_XX,C_OO,C_OO,C_OO,C_XX,C_XX,C_OO,C_OO,C_OO,C_gg,C_OO,
	C_GG,C_XX,C_OO,C_OO,C_XX,C_XX,C_XX,C_XX,C_OO,C_OO,C_gg,C_OO,
	C_GG,C_XX,C_OO,C_OO,C_XX,C_XX,C_XX,C_XX,C_OO,C_OO,C_gg,C_OO,
	C_GG,C_XX,C_OO,C_OO,C_OO,C_XX,C_XX,C_OO,C_OO,C_OO,C_gg,C_OO,
	C___,C_GG,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_gg,C_OO,C___,
	C___,C_GG,C_XX,C_gg,C_OO,C_OO,C_OO,C_OO,C_gg,C_gg,C_OO,C___,
	C___,C___,C_OO,C_OO,C_gg,C_gg,C_gg,C_gg,C_OO,C_OO,C___,C___,
	C___,C___,C___,C___,C_OO,C_OO,C_OO,C_OO,C___,C___,C___,C___
};

WEBC_READONLY WebGraphBitmapInfo gRadioOnBitmapInfo =
{
	WGB_RADIO_ON_WIDTH,           // iWidth
	WGB_RADIO_ON_HEIGHT,          // iHeight
	WGB_RADIO_ON_WIDTH,           // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,      // pixelFormat
	WEBC_TRUE,                    // bHasTransparentColor
	C___,                         // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                            // iPaletteSize
	gBitmapPalette,               // pPalette
	gRadioOnBitmapData            // pData
};


/*---------------------------------------------------------------------------*
 * Radio On Disabled
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gRadioOnDisabledBitmapData[] =
{
	C___,C___,C___,C___,C_GG,C_GG,C_GG,C_GG,C___,C___,C___,C___,
	C___,C___,C_GG,C_GG,C_XX,C_XX,C_XX,C_XX,C_GG,C_GG,C___,C___,
	C___,C_GG,C_XX,C_XX,C_gg,C_gg,C_gg,C_gg,C_XX,C_gg,C_OO,C___,
	C___,C_GG,C_XX,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_OO,C___,
	C_GG,C_XX,C_gg,C_gg,C_gg,C_XX,C_XX,C_gg,C_gg,C_gg,C_gg,C_OO,
	C_GG,C_XX,C_gg,C_gg,C_XX,C_XX,C_XX,C_XX,C_gg,C_gg,C_gg,C_OO,
	C_GG,C_XX,C_gg,C_gg,C_XX,C_XX,C_XX,C_XX,C_gg,C_gg,C_gg,C_OO,
	C_GG,C_XX,C_gg,C_gg,C_gg,C_XX,C_XX,C_gg,C_gg,C_gg,C_gg,C_OO,
	C___,C_GG,C_XX,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_OO,C___,
	C___,C_GG,C_XX,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_OO,C___,
	C___,C___,C_OO,C_OO,C_gg,C_gg,C_gg,C_gg,C_OO,C_OO,C___,C___,
	C___,C___,C___,C___,C_OO,C_OO,C_OO,C_OO,C___,C___,C___,C___
};

WEBC_READONLY WebGraphBitmapInfo gRadioOnDisabledBitmapInfo =
{
	WGB_RADIO_ON_DISABLED_WIDTH,  // iWidth
	WGB_RADIO_ON_DISABLED_HEIGHT, // iHeight
	WGB_RADIO_ON_DISABLED_WIDTH,  // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,      // pixelFormat
	WEBC_TRUE,                    // bHasTransparentColor
	C___,                         // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                            // iPaletteSize
	gBitmapPalette,               // pPalette
	gRadioOnDisabledBitmapData    // pData
};


/*---------------------------------------------------------------------------*
 * Radio Off
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gRadioOffBitmapData[] =
{
	C___,C___,C___,C___,C_GG,C_GG,C_GG,C_GG,C___,C___,C___,C___,
	C___,C___,C_GG,C_GG,C_XX,C_XX,C_XX,C_XX,C_GG,C_GG,C___,C___,
	C___,C_GG,C_XX,C_XX,C_OO,C_OO,C_OO,C_OO,C_XX,C_gg,C_OO,C___,
	C___,C_GG,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_gg,C_OO,C___,
	C_GG,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_gg,C_OO,
	C_GG,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_gg,C_OO,
	C_GG,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_gg,C_OO,
	C_GG,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_gg,C_OO,
	C___,C_GG,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_gg,C_OO,C___,
	C___,C_GG,C_XX,C_gg,C_OO,C_OO,C_OO,C_OO,C_gg,C_gg,C_OO,C___,
	C___,C___,C_OO,C_OO,C_gg,C_gg,C_gg,C_gg,C_OO,C_OO,C___,C___,
	C___,C___,C___,C___,C_OO,C_OO,C_OO,C_OO,C___,C___,C___,C___
};

WEBC_READONLY WebGraphBitmapInfo gRadioOffBitmapInfo =
{
	WGB_RADIO_OFF_WIDTH,          // iWidth
	WGB_RADIO_OFF_HEIGHT,         // iHeight
	WGB_RADIO_OFF_WIDTH,          // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,      // pixelFormat
	WEBC_TRUE,                    // bHasTransparentColor
	C___,                         // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                            // iPaletteSize
	gBitmapPalette,               // pPalette
	gRadioOffBitmapData           // pData
};


/*---------------------------------------------------------------------------*
 * Radio Off Disabled
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gRadioOffDisabledBitmapData[] =
{
	C___,C___,C___,C___,C_GG,C_GG,C_GG,C_GG,C___,C___,C___,C___,
	C___,C___,C_GG,C_GG,C_XX,C_XX,C_XX,C_XX,C_GG,C_GG,C___,C___,
	C___,C_GG,C_XX,C_XX,C_gg,C_gg,C_gg,C_gg,C_XX,C_gg,C_OO,C___,
	C___,C_GG,C_XX,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_OO,C___,
	C_GG,C_XX,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_OO,
	C_GG,C_XX,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_OO,
	C_GG,C_XX,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_OO,
	C_GG,C_XX,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_OO,
	C___,C_GG,C_XX,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_OO,C___,
	C___,C_GG,C_XX,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_gg,C_OO,C___,
	C___,C___,C_OO,C_OO,C_gg,C_gg,C_gg,C_gg,C_OO,C_OO,C___,C___,
	C___,C___,C___,C___,C_OO,C_OO,C_OO,C_OO,C___,C___,C___,C___
};

WEBC_READONLY WebGraphBitmapInfo gRadioOffDisabledBitmapInfo =
{
	WGB_RADIO_OFF_DISABLED_WIDTH, // iWidth
	WGB_RADIO_OFF_DISABLED_HEIGHT,// iHeight
	WGB_RADIO_OFF_DISABLED_WIDTH, // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,      // pixelFormat
	WEBC_TRUE,                    // bHasTransparentColor
	C___,                         // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                            // iPaletteSize
	gBitmapPalette,               // pPalette
	gRadioOffDisabledBitmapData   // pData
};


/*---------------------------------------------------------------------------*
 * Checkbox Off
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gCheckboxOffBitmapData[] =
{
	C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO
};

WEBC_READONLY WebGraphBitmapInfo gCheckboxOffBitmapInfo =
{
	WGB_CHECKBOX_OFF_WIDTH,       // iWidth
	WGB_CHECKBOX_OFF_HEIGHT,      // iHeight
	WGB_CHECKBOX_OFF_WIDTH,       // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,      // pixelFormat
	WEBC_TRUE,                    // bHasTransparentColor
	C___,                         // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                            // iPaletteSize
	gBitmapPalette,               // pPalette
	gCheckboxOffBitmapData        // pData
};


/*---------------------------------------------------------------------------*
 * Checkbox Off Disabled
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gCheckboxOffDisabledBitmapData[] =
{
	C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO
};

WEBC_READONLY WebGraphBitmapInfo gCheckboxOffDisabledBitmapInfo =
{
	WGB_CHECKBOX_OFF_DISABLED_WIDTH,  // iWidth
	WGB_CHECKBOX_OFF_DISABLED_HEIGHT, // iHeight
	WGB_CHECKBOX_OFF_DISABLED_WIDTH,  // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,          // pixelFormat
	WEBC_TRUE,                        // bHasTransparentColor
	C___,                             // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                                // iPaletteSize
	gBitmapPalette,                   // pPalette
	gCheckboxOffDisabledBitmapData    // pData
};


/*---------------------------------------------------------------------------*
 * Checkbox On
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gCheckboxOnBitmapData[] =
{
	C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_XX,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_XX,C_XX,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_XX,C_OO,C_OO,C_OO,C_XX,C_XX,C_XX,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_XX,C_XX,C_OO,C_XX,C_XX,C_XX,C_OO,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_XX,C_XX,C_XX,C_XX,C_XX,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_XX,C_XX,C_XX,C_OO,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C_XX,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C___,C_OO,
	C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO
};

WEBC_READONLY WebGraphBitmapInfo gCheckboxOnBitmapInfo =
{
	WGB_CHECKBOX_ON_WIDTH,            // iWidth
	WGB_CHECKBOX_ON_HEIGHT,           // iHeight
	WGB_CHECKBOX_ON_WIDTH,            // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,          // pixelFormat
	WEBC_TRUE,                        // bHasTransparentColor
	C___,                             // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                                // iPaletteSize
	gBitmapPalette,                   // pPalette
	gCheckboxOnBitmapData             // pData
};


/*---------------------------------------------------------------------------*
 * Checkbox On Disabled
 *---------------------------------------------------------------------------*/

WEBC_READONLY WEBC_UINT8 gCheckboxOnDisabledBitmapData[] =
{
	C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C_XX,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C_XX,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C___,C_OO,
	C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO,C_OO
};

WEBC_READONLY WebGraphBitmapInfo gCheckboxOnDisabledBitmapInfo =
{
	WGB_CHECKBOX_ON_DISABLED_WIDTH,   // iWidth
	WGB_CHECKBOX_ON_DISABLED_HEIGHT,  // iHeight
	WGB_CHECKBOX_ON_DISABLED_WIDTH,   // iBytesPerRow
	WB_PIXEL_FORMAT_INDEX_8,          // pixelFormat
	WEBC_TRUE,                        // bHasTransparentColor
	C___,                             // iTransparentIndex
	UTransparentRed,              // uTransparentRed
	UTransparentGreen,            // uTransparentGreen
	UTransparentBlue,             // uTransparentBlue
	5,                                // iPaletteSize
	gBitmapPalette,                   // pPalette
	gCheckboxOnDisabledBitmapData     // pData
};

/*****************************************************************************/
// Table of all bitmap info
/*****************************************************************************/

const WebGraphBitmapInfo* gWebcDefaultBitmapInfo[] =
{
	&gUpArrowBitmapInfo,
	&gDownArrowBitmapInfo,
	&gLeftArrowBitmapInfo,
	&gRightArrowBitmapInfo,
	&gRadioOnBitmapInfo,
	&gRadioOnDisabledBitmapInfo,
	&gRadioOffBitmapInfo,
	&gRadioOffDisabledBitmapInfo,
	&gCheckboxOnBitmapInfo,
	&gCheckboxOnDisabledBitmapInfo,
	&gCheckboxOffBitmapInfo,
	&gCheckboxOffDisabledBitmapInfo,
	&gDiscBulletBitmapInfo,
	&gCircleBulletBitmapInfo,
	&gSquareBulletBitmapInfo
};
