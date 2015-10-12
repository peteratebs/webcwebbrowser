#ifndef WINDOWSGDI		/* So we can build GDI and Framebuffer from the same project */

#include "webcmem.h"
#include "webc.h"
#include "wcconst.hpp"
#include "webfonts.hpp"
#include "FBGraphicsContext.h"

#include "FBAbstraction.h"
#include "FBAbstractionFonts.h"

#define CHAR_MAP_BPP 1 // bits per pixel of font bitmap (set when FontGen is run)

//#define INCLUDE_CJK_FONTS 1
#define TAHOMAQVGAMOTIF 0
#define ARIELONLY 1
#if (ARIELONLY)
#undef WEBC_CFG_MAX_FONT_FAMILIES
#define WEBC_CFG_MAX_FONT_FAMILIES 1
#endif
#ifdef __TIARM__
#undef  TAHOMAQVGAMOTIF
#define TAHOMAQVGAMOTIF 1
#endif

#if (TAHOMAQVGAMOTIF)
static WEBC_READONLY WEBC_CHAR* gpFontFamilyName[WEBC_CFG_MAX_FONT_FAMILIES] = {
		WEBC_STR_TAHOMA,
		WEBC_STR_ARIAL,
		WEBC_STR_COURIER,
		WEBC_STR_TAHOMA
	};
static WEBC_READONLY WEBC_CHAR* gpFontFamilyGenericName[WEBC_CFG_MAX_FONT_FAMILIES] = {
		WEBC_STR_SANS_SERIF,
		WEBC_STR_MONOSPACE,
		WEBC_STR_SANS_SERIF,
		WEBC_STR_SANS_SERIF
	};
#else
static WEBC_READONLY WEBC_CHAR* gpFontFamilyName[WEBC_CFG_MAX_FONT_FAMILIES] = {
		WEBC_STR_ARIAL,
#if (!ARIELONLY)
		WEBC_STR_COURIER,
		WEBC_STR_TIMES,
		WEBC_STR_VERDANA
#endif
	};
static WEBC_READONLY WEBC_CHAR* gpFontFamilyGenericName[WEBC_CFG_MAX_FONT_FAMILIES] = {
		WEBC_STR_SANS_SERIF,
#if (!ARIELONLY)
		WEBC_STR_MONOSPACE,
		WEBC_STR_SERIF,
		WEBC_STR_SANS_SERIF
#endif
	};
#endif


static FB_Font* gFBDefaultFont = 0;

//  The fontMapping_t structure defines the list of available fonts. Each entry
//  points to either a file or an in-memory (ROM) image.

typedef struct {
	const WEBC_CHAR* familyName;
	const WEBC_CHAR* genericName;
	WEBC_UINT16 pointSize;
	WEBC_UINT16 attrib;
	WEBC_CHAR startChar;
	WEBC_CHAR endChar;
//#ifdef FB_FONTS_STORED_IN_FILES
#if (FB_FONTS_STORED_IN_FILES)
	const char fileName[WEBC_MAX_PATH]; // points to a file containing binary font data (made with FontGen)
	WEBC_UINT32 createdFontSize, lastCacheRequest;
#else
	FB_Font *binData; // points to an area of memory (usually ROM) containing binary font data
#endif
	FB_Font *pCreatedFont;
} fontMapping_t;


//#ifdef FB_FONTS_STORED_IN_FILES
#if (FB_FONTS_STORED_IN_FILES)

#include "rtpfile.h"

// File-Based Font Mapping (ultimately defined by the user)

static fontMapping_t fontMap[] = {
#if (TAHOMAQVGAMOTIF)
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 0, 0x0020, 0x007e, "TahomaRegular10B0020E007e",  9173,  1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 14, 0, 0x0020, 0x007e, "TahomaRegular14B0020E007e",  16381,  1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 20, 0, 0x0020, 0x007e, "TahomaRegular20B0020E007e",  31733,  1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 0, 0x0020, 0x007e, "TahomaBold10B0020E007e",     9173,     1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 14, 0, 0x0020, 0x007e, "TahomaBold14B0020E007e",     16381,     1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 20, 0, 0x0020, 0x007e, "TahomaBold20B0020E007e",     31733,     1, 0},

  { gpFontFamilyName[1], gpFontFamilyGenericName[1], 10, 0, 0x0020, 0x007e, "ArialRegular10B0020E007e",   9173,   1, 0},
  { gpFontFamilyName[1], gpFontFamilyGenericName[1], 14, 0, 0x0020, 0x007e, "ArialRegular14B0020E007e",   16381,   1, 0},
  { gpFontFamilyName[1], gpFontFamilyGenericName[1], 20, 0, 0x0020, 0x007e, "ArialRegular20B0020E007e",   31733,   1, 0},
  { gpFontFamilyName[1], gpFontFamilyGenericName[1], 10, 0, 0x0020, 0x007e, "ArialBold10B0020E007e",      9173,      1, 0},
  { gpFontFamilyName[1], gpFontFamilyGenericName[1], 14, 0, 0x0020, 0x007e, "ArialBold14B0020E007e",      16381,      1, 0},
  { gpFontFamilyName[1], gpFontFamilyGenericName[1], 20, 0, 0x0020, 0x007e, "ArialBold20B0020E007e",      31733,      1, 0},

  { gpFontFamilyName[2], gpFontFamilyGenericName[2], 10, 0, 0x0020, 0x007e, "CourierRegular10B0020E007e", 9173, 1, 0},
  { gpFontFamilyName[2], gpFontFamilyGenericName[2], 14, 0, 0x0020, 0x007e, "CourierRegular14B0020E007e", 16381, 1, 0},
  { gpFontFamilyName[2], gpFontFamilyGenericName[2], 20, 0, 0x0020, 0x007e, "CourierRegular20B0020E007e", 31733, 1, 0},
  { gpFontFamilyName[2], gpFontFamilyGenericName[2], 10, 0, 0x0020, 0x007e, "CourierBold10B0020E007e",    9173,    1, 0},
  { gpFontFamilyName[2], gpFontFamilyGenericName[2], 14, 0, 0x0020, 0x007e, "CourierBold14B0020E007e",    16381,    1, 0},
  { gpFontFamilyName[2], gpFontFamilyGenericName[2], 20, 0, 0x0020, 0x007e, "CourierBold20B0020E007e",    31733,    1, 0},

  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 0, 0x0020, 0x007e, "TahomaRegular10B0020E007e",  9173,  1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 14, 0, 0x0020, 0x007e, "TahomaRegular14B0020E007e",  16381,  1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 20, 0, 0x0020, 0x007e, "TahomaRegular20B0020E007e",  31733,  1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 0, 0x0020, 0x007e, "TahomaBold10B0020E007e",     9173,     1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 14, 0, 0x0020, 0x007e, "TahomaBold14B0020E007e",     16381,     1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 20, 0, 0x0020, 0x007e, "TahomaBold20B0020E007e",     31733,     1, 0},

#else

  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 0, 0x0020, 0x007e, "ArialRegular10B0020E007e", 9173, 1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 6, 0, 0x0020, 0x007e, "ArialRegular6B0020E007e", 4341, 1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 9, 0, 0x0020, 0x007e, "ArialRegular9B0020E007e", 7737, 1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 20, 0, 0x0020, 0x007e, "ArialRegular20B0020E007e", 31733, 1, 0},

  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 6, 1, 0x0020, 0x007e, "ArialBold6B0020E007e", 4341, 1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 9, 1, 0x0020, 0x007e, "ArialBold9B0020E007e", 7737, 1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 1, 0x0020, 0x007e, "ArialBold10B0020E007e", 9173, 1, 0},

  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 6, 2, 0x0020, 0x007e, "ArialItalic6B0020E007e", 4341, 1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 9, 2, 0x0020, 0x007e, "ArialItalic9B0020E007e", 7737, 1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 2, 0x0020, 0x007e, "ArialItalic10B0020E007e", 9173, 1, 0},

#ifdef INCLUDE_CJK_FONTS
// CJK
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 0, 0x4e00, 0x9fcb, "ArialRegular10B4e00E9fcb", 2010293, 1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 20, 0, 0x4e00, 0x9fcb, "ArialRegular20B4e00E9fcb", 7035653, 1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 30, 0, 0x4e00, 0x9fcb, "ArialRegular30B4e00E9fcb", 15411253, 1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 6, 0, 0x4e00, 0x9fcb, "ArialRegular6B4e00E9fcb", 938197, 1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 9, 0, 0x4e00, 0x9fcb, "ArialRegular9B4e00E9fcb", 1692013, 1, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 40, 0, 0x4e00, 0x9fcb, "ArialRegular40B4e00E9fcb", 27137093, 1, 0},
#endif
#endif
};

const char g_fontFilePath[] = "..\\..\\..\\..\\gui\\framebuffer\\FB_Fonts\\";

#ifdef INCLUDE_CJK_FONTS
#define NUM_FONT_MAP_ITEMS 16
#else
#define NUM_FONT_MAP_ITEMS 10
#endif

#else

extern "C" {

#if (TAHOMAQVGAMOTIF)
#include "TahomaRegular10B0020E007e.h"
#include "TahomaRegular11B0020E007e.h"
#include "TahomaRegular12B0020E007e.h"
#include "TahomaRegular13B0020E007e.h"
#include "TahomaRegular14B0020E007e.h"
#include "TahomaRegular15B0020E007e.h"
#include "TahomaRegular16B0020E007e.h"
#include "TahomaRegular17B0020E007e.h"
#include "TahomaRegular18B0020E007e.h"

#include "TahomaBold10B0020E007e.h"
#include "TahomaBold11B0020E007e.h"
#include "TahomaBold12B0020E007e.h"
#include "TahomaBold13B0020E007e.h"
#include "TahomaBold14B0020E007e.h"
#include "TahomaBold15B0020E007e.h"
#include "TahomaBold16B0020E007e.h"
#include "TahomaBold17B0020E007e.h"
#include "TahomaBold18B0020E007e.h"


#include "ArialRegular10B0020E007e.h"
#include "ArialRegular14B0020E007e.h"
#include "ArialRegular20B0020E007e.h"
#include "ArialBold10B0020E007e.h"
#include "ArialBold14B0020E007e.h"
#include "ArialBold20B0020E007e.h"
#include "CourierRegular10B0020E007e.h"
#include "CourierRegular14B0020E007e.h"
#include "CourierRegular20B0020E007e.h"
#include "CourierBold10B0020E007e.h"
#include "CourierBold14B0020E007e.h"
#include "CourierBold20B0020E007e.h"

#else


#include "ArialRegular6B0020E007e.h"
#include "ArialRegular9B0020E007e.h"
#include "ArialRegular10B0020E007e.h"
#include "ArialRegular20B0020E007e.h"

#if(!ARIELONLY)
#include "ArialItalic6B0020E007e.h"
#include "ArialItalic9B0020E007e.h"
#include "ArialItalic10B0020E007e.h"

#include "ArialBold6B0020E007e.h"
#include "ArialBold9B0020E007e.h"
#include "ArialBold10B0020E007e.h"
#endif
 #ifdef INCLUDE_CJK_FONTS

// CJK
#include "ArialRegular6B4e00E9fcb.h"
#include "ArialRegular9B4e00E9fcb.h"
#include "ArialRegular10B4e00E9fcb.h"
#include "ArialRegular20B4e00E9fcb.h"
#include "ArialRegular30B4e00E9fcb.h"
#include "ArialRegular40B4e00E9fcb.h"

#endif
#endif

}

// In-Memory Font Mapping

static fontMapping_t fontMap[] = {
#if (TAHOMAQVGAMOTIF)
// tahoma regular
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 0, 0x0020, 0x007e, (FB_Font *)TahomaRegular10B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 12, 0, 0x0020, 0x007e, (FB_Font *)TahomaRegular12B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 14, 0, 0x0020, 0x007e, (FB_Font *)TahomaRegular14B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 15, 0, 0x0020, 0x007e, (FB_Font *)TahomaRegular15B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 16, 0, 0x0020, 0x007e, (FB_Font *)TahomaRegular16B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 18, 0, 0x0020, 0x007e, (FB_Font *)TahomaRegular18B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 17, 0, 0x0020, 0x007e, (FB_Font *)TahomaRegular17B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 13, 0, 0x0020, 0x007e, (FB_Font *)TahomaRegular13B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 11, 0, 0x0020, 0x007e, (FB_Font *)TahomaRegular11B0020E007e_file,0},
// tahoma bold 
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 1, 0x0020, 0x007e, (FB_Font *)TahomaBold10B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 12, 1, 0x0020, 0x007e, (FB_Font *)TahomaBold12B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 14, 1, 0x0020, 0x007e, (FB_Font *)TahomaBold14B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 15, 1, 0x0020, 0x007e, (FB_Font *)TahomaBold15B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 16, 1, 0x0020, 0x007e, (FB_Font *)TahomaBold16B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 18, 1, 0x0020, 0x007e, (FB_Font *)TahomaBold18B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 17, 1, 0x0020, 0x007e, (FB_Font *)TahomaBold17B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 13, 1, 0x0020, 0x007e, (FB_Font *)TahomaBold13B0020E007e_file,0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 11, 1, 0x0020, 0x007e, (FB_Font *)TahomaBold11B0020E007e_file,0},



  { gpFontFamilyName[1], gpFontFamilyGenericName[1], 10, 0, 0x0020, 0x007e, (FB_Font *)ArialRegular10B0020E007e_file,0},
  { gpFontFamilyName[1], gpFontFamilyGenericName[1], 14, 0, 0x0020, 0x007e, (FB_Font *)ArialRegular14B0020E007e_file,0},
  { gpFontFamilyName[1], gpFontFamilyGenericName[1], 20, 0, 0x0020, 0x007e, (FB_Font *)ArialRegular20B0020E007e_file,0},
  { gpFontFamilyName[1], gpFontFamilyGenericName[1], 10, 1, 0x0020, 0x007e, (FB_Font *)ArialBold10B0020E007e_file,0},
  { gpFontFamilyName[1], gpFontFamilyGenericName[1], 14, 1, 0x0020, 0x007e, (FB_Font *)ArialBold14B0020E007e_file,0},
  { gpFontFamilyName[1], gpFontFamilyGenericName[1], 20, 1, 0x0020, 0x007e, (FB_Font *)ArialBold20B0020E007e_file,0},

  { gpFontFamilyName[2], gpFontFamilyGenericName[2], 10, 0, 0x0020, 0x007e, (FB_Font *)CourierRegular10B0020E007e_file,0},
  { gpFontFamilyName[2], gpFontFamilyGenericName[2], 14, 0, 0x0020, 0x007e, (FB_Font *)CourierRegular14B0020E007e_file,0},
  { gpFontFamilyName[2], gpFontFamilyGenericName[2], 20, 0, 0x0020, 0x007e, (FB_Font *)CourierRegular20B0020E007e_file,0},
  { gpFontFamilyName[2], gpFontFamilyGenericName[2], 10, 1, 0x0020, 0x007e, (FB_Font *)CourierBold10B0020E007e_file,0},
  { gpFontFamilyName[2], gpFontFamilyGenericName[2], 14, 1, 0x0020, 0x007e, (FB_Font *)CourierBold14B0020E007e_file,0},
  { gpFontFamilyName[2], gpFontFamilyGenericName[2], 20, 1, 0x0020, 0x007e, (FB_Font *)CourierBold20B0020E007e_file,0},

#else
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 0, 0x0020, 0x007e,(FB_Font *)ArialRegular10B0020E007e_file, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 6, 0, 0x0020, 0x007e, (FB_Font *)ArialRegular6B0020E007e_file, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 9, 0, 0x0020, 0x007e, (FB_Font *)ArialRegular9B0020E007e_file, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 20, 0, 0x0020, 0x007e, (FB_Font *)ArialRegular20B0020E007e_file, 0},
#if(!ARIELONLY)
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 6, 1, 0x0020, 0x007e, (FB_Font *)ArialBold6B0020E007e_file, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 9, 1, 0x0020, 0x007e, (FB_Font *)ArialBold9B0020E007e_file, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 1, 0x0020, 0x007e, (FB_Font *)ArialBold10B0020E007e_file, 0},

  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 6, 2, 0x0020, 0x007e, (FB_Font *)ArialItalic6B0020E007e_file, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 9, 2, 0x0020, 0x007e, (FB_Font *)ArialItalic9B0020E007e_file, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 2, 0x0020, 0x007e, (FB_Font *)ArialItalic10B0020E007e_file, 0},
#endif
#ifdef INCLUDE_CJK_FONTS
// CJK
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 10, 0, 0x4e00, 0x9fcb, (FB_Font *)ArialRegular10B4e00E9fcb_file, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 20, 0, 0x4e00, 0x9fcb, (FB_Font *)ArialRegular20B4e00E9fcb_file, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 30, 0, 0x4e00, 0x9fcb, (FB_Font *)ArialRegular30B4e00E9fcb_file, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 6, 0, 0x4e00, 0x9fcb, (FB_Font *)ArialRegular6B4e00E9fcb_file, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 9, 0, 0x4e00, 0x9fcb, (FB_Font *)ArialRegular9B4e00E9fcb_file, 0},
  { gpFontFamilyName[0], gpFontFamilyGenericName[0], 40, 0, 0x4e00, 0x9fcb, (FB_Font *)ArialRegular40B4e00E9fcb_file, 0},
#endif
#endif
};


#define NUM_FONT_MAP_ITEMS (sizeof(fontMap)/sizeof(fontMap[0]))

#endif
/**
*  This function is called by WebC to retrieve a handle or pointer to a font that it
*  will use to render html content. For more information see the GUI abstraction layer
*  document.
*/
void* FB_CreateFont (
		HTMLGraphicsContext** ctx,
		const WEBC_CHAR* familyName,
		const WEBC_CHAR* genericName,
		WEBC_UINT16 pointSize,
		WEBC_UINT16 attrib,
		int familyIndex)
{
	if (familyIndex < 0)
	{
		familyIndex = webc_font_family_index (familyName, genericName, gpFontFamilyName, gpFontFamilyGenericName, WEBC_CFG_MAX_FONT_FAMILIES);
		if (familyIndex < 0)
		{
			familyIndex = 0;
		}
	}

#if (WEBC_SUPPORT_UNICODE)
//		if (!webc_c_stricmp(familyName, "courier"))
//		{
//			familyName = (WEBC_CHAR *)L"Courier New";
//		}
#endif

	FB_Font *new_font = FB_LoadFont(ctx, familyName, genericName, pointSize, attrib, 0);

	return new_font;
}


void FB_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* font
)
{
	FB_Font* f = (FB_Font*)font;
	if(f && (--(f->FontReferenceCount) == 0))

	{
		if(f->charMap)
		{
//#ifdef FB_FONTS_STORED_IN_FILES
#if (FB_FONTS_STORED_IN_FILES)
// pData is a ROM pointer if we're not using file based fonts
			if(f->charMap->pData)
			{
				WEBC_FREE((void*)f->charMap->pData);
				f->charMap->pData = 0;
			}
#endif
			WEBC_FREE((void*)f->charMap);
			f->charMap = 0;
		}

		// remove our saved pointer to the created font from the font map table
		for(int fontMapIndex=0; fontMapIndex < NUM_FONT_MAP_ITEMS; fontMapIndex++)
		{
			if(fontMap[fontMapIndex].pCreatedFont == f)
			{
				fontMap[fontMapIndex].pCreatedFont = 0;
				break;
			}
		}
		WEBC_FREE((void*)f);
	}
}


/**
*  This function is called by WebC to retrieve a handle or pointer to a default
*  font that it will use to draw text in list boxes and drop boxes. The returned
*  value will later be passed to other graphics abstraction layer functions that
*  take a font handle/pointer as one of their arguments.
*/
void* FB_GetDefaultFont (
	HTMLGraphicsContext** ctx
){
	if (!gFBDefaultFont)
	{
#if (TAHOMAQVGAMOTIF)
		gFBDefaultFont = (FB_Font*)FB_CreateFont(ctx,WEBC_STR_TAHOMA,WEBC_STR_SANS_SERIF,10,0,0);
#else
		gFBDefaultFont = (FB_Font*)FB_CreateFont(ctx,WEBC_STR_ARIAL,WEBC_STR_SANS_SERIF,10,0,0);
#endif
	}
	return (void*)gFBDefaultFont;
}

/**
*	Used to get the number of font families available.
*/
int FB_NumFontFamilies(
	HTMLGraphicsContext** ctx
){
	return WEBC_CFG_MAX_FONT_FAMILIES;
}

/**
*	This function is called by WebC to retrieve a table of the font families
*	that are available by name. The table must contain WEBC_CFG_MAX_FONT_FAMILIES entries.
*/
const WEBC_CHAR** FB_GetFontFamilyNames (
	HTMLGraphicsContext** ctx
){
	return gpFontFamilyName;
}

/**
*	This function is called by WebC to retrieve a table of the generic font
*	families that are available by name. The table must contain
*	WEBC_CFG_MAX_FONT_FAMILIES entries.
*/
const WEBC_CHAR** FB_GetFontFamilyGenericNames (
	HTMLGraphicsContext** ctx
){
	return gpFontFamilyGenericName;
}


////////////////////////// FUNCTIONS PRIVATE TO FB IMPLEMENTATION //////////////////////////////////////
#if (1) /* Fixed */

#define FATTRMATCHED		0x08
#define FGENERICMATCHED		0x04
#define FNAMEMATCHED		0x02
#define FSIZEMATCHED		0x01
#define ALLMATCH			(FNAMEMATCHED|FGENERICMATCHED|FSIZEMATCHED|FATTRMATCHED)

//#define NUMPMATCHCRITERIA 5
//const unsigned char match_precedence[5] = {ALLMATCH, AAAMATCH, BBBMATCH, CCCMATCH, DDDMATCH};

WEBC_BOOL FB_ChooseFontFromMap(const WEBC_CHAR* familyName, const WEBC_CHAR* genericName, WEBC_UINT16 pointSize, WEBC_UINT16 attrib, WEBC_CHAR charCode, int &mapIndex)
{
	WEBC_BOOL bFound = WEBC_FALSE;
	unsigned char match_map[NUM_FONT_MAP_ITEMS];


	mapIndex = 0;

	// search for a matching font
	// TODO the algorithm for font searching and substitution could be much more robust
	for(int i=0; i < NUM_FONT_MAP_ITEMS; i++)
	{
		match_map[i] = 0;
		if(charCode)
		{
			// if the character is not contained in this font bitmap, then we can't use it
			if((charCode < fontMap[i].startChar) || (charCode > fontMap[i].endChar))
				continue;
		}
		if (!webc_stricmp(familyName, fontMap[i].familyName))
			match_map[i] |= FNAMEMATCHED;
		if (!webc_stricmp(genericName, fontMap[i].genericName))
			match_map[i] |= FGENERICMATCHED;
		if (pointSize == fontMap[i].pointSize)
			match_map[i] |= FSIZEMATCHED;
		if (attrib == fontMap[i].attrib)
			match_map[i] |= FATTRMATCHED;
		if (match_map[i] == ALLMATCH)
		{
			mapIndex = i;
			bFound = WEBC_TRUE;
			break;
		}
	}


	if(!bFound)
	{
	// we didn't get an exact match so try again
	// Size is highest precedence, then generic, then name, then style
	int best_size_error = 0;
	int best_size_error_index = -1;
	unsigned char best_match = 0;

		for(int i=0; i < NUM_FONT_MAP_ITEMS; i++)
		{
			if(charCode)
			{
				// if the character is not contained in this font bitmap, then we can't use it
				if((charCode < fontMap[i].startChar) || (charCode > fontMap[i].endChar))
					continue;
			}
			else
			{
				int match_size_error,match_size_error_abs;
				if (pointSize >= fontMap[i].pointSize)
					match_size_error = (int)(pointSize - fontMap[i].pointSize);
				else
					match_size_error = (int)(fontMap[i].pointSize - pointSize);
				match_size_error_abs = EBSABS(match_size_error);
				if (best_size_error_index < 0 || match_size_error_abs < best_size_error)
				{
					best_size_error = match_size_error_abs;
					best_match = match_map[i];
					best_size_error_index = i;
				}
				else if (match_size_error_abs == best_size_error)
				{
					if (match_map[i] > best_match)
					{
						best_size_error_index = i;
						best_match = match_map[i];
					}
				}
			}
		}
		if (best_size_error_index >= 0)
		{
			mapIndex = best_size_error_index;
			bFound = WEBC_TRUE;
		}
	}

	return bFound;
}
#else

WEBC_BOOL FB_ChooseFontFromMap(const WEBC_CHAR* familyName, const WEBC_CHAR* genericName, WEBC_UINT16 pointSize, WEBC_UINT16 attrib, WEBC_CHAR charCode, int &mapIndex)
{
	WEBC_BOOL bFound = WEBC_FALSE;

	mapIndex = 0;

	// search for a matching font
	// TODO the algorithm for font searching and substitution could be much more robust
	for(int i=0; i < NUM_FONT_MAP_ITEMS; i++)
	{
		if (!webc_stricmp(familyName, fontMap[i].familyName) &&
		!webc_stricmp(genericName, fontMap[i].genericName) &&
		(pointSize == fontMap[i].pointSize) &&
		(attrib == fontMap[i].attrib) )
		{
			if(charCode)
			{
				// if the character is not contained in this font bitmap, then we can't use it
				if((charCode < fontMap[i].startChar) || (charCode > fontMap[i].endChar))
					continue;
			}
			mapIndex = i;
			bFound = WEBC_TRUE;
			break;
		}
	}

	// we didn't get an exact match so try again
	if(!bFound)
	{
		for(int i=0; i < NUM_FONT_MAP_ITEMS; i++)
		{
			if(charCode)
			{
				// if the character is not contained in this font bitmap, then we can't use it
				if((charCode < fontMap[i].startChar) || (charCode > fontMap[i].endChar))
					continue;
			}
			printf("Use default search for %s %s %d %x\n",familyName, genericName, pointSize, attrib);
			mapIndex = i;
			bFound = WEBC_TRUE;
			break;
		}
	}

	return bFound;
}
#endif
//#ifdef FB_FONTS_STORED_IN_FILES
#if (FB_FONTS_STORED_IN_FILES)

void FB_ProcessFontCache(HTMLGraphicsContext** ctx, WEBC_UINT32 allocationAmount)
{
	fontMapping_t* orderedCacheList[NUM_FONT_MAP_ITEMS];
	unsigned int runningAllocation=allocationAmount;
	int fontIndex, numActualFonts=0;
	WEBC_BOOL bSwapped;

	// make a list of the actual fonts in the cache (excluding NULLs)
	for(fontIndex = 0; fontIndex < NUM_FONT_MAP_ITEMS; fontIndex++)
	{
		if(fontMap[fontIndex].pCreatedFont)
			orderedCacheList[numActualFonts++] = &fontMap[fontIndex];
	}

	// create an ordered list based on how recently the font was used
	do
	{
		bSwapped = WEBC_FALSE;
		for(fontIndex = 0; fontIndex <= numActualFonts - 2; fontIndex++)
		{
			if(orderedCacheList[fontIndex]->lastCacheRequest < orderedCacheList[fontIndex + 1]->lastCacheRequest)
			{
				// swap
				fontMapping_t*pTemp = orderedCacheList[fontIndex];
				orderedCacheList[fontIndex] = orderedCacheList[fontIndex + 1];
				orderedCacheList[fontIndex + 1] = orderedCacheList[fontIndex];
				bSwapped = WEBC_TRUE;
			}
		}
	}
	while(WEBC_TRUE == bSwapped);

	// take this opportunity to deal with the case when GET_FBGRAPHICS_CTX(ctx)->fontCacheCurrentUsageIndex wraps
	// if GET_FBGRAPHICS_CTX(ctx)->fontCacheCurrentUsageIndex is about to loop around,
	// then re-index all of the cache items in use
	if(0xFFFFFFFF == GET_FBGRAPHICS_CTX(ctx)->fontCacheCurrentUsageIndex)
	{
		GET_FBGRAPHICS_CTX(ctx)->fontCacheCurrentUsageIndex = 1;

		for(fontIndex = 0; fontIndex < numActualFonts; fontIndex++)
		{
			if(orderedCacheList[fontIndex])
			{
				orderedCacheList[fontIndex]->lastCacheRequest = GET_FBGRAPHICS_CTX(ctx)->fontCacheCurrentUsageIndex++;
				for(int origIndex=0; origIndex < NUM_FONT_MAP_ITEMS; origIndex++)
					if(&fontMap[origIndex] == orderedCacheList[fontIndex])
						fontMap[origIndex].lastCacheRequest = orderedCacheList[fontIndex]->lastCacheRequest;
			}
		}
	}

	// now toss any of the least recently used fonts if we're exceeding the current font allocation

	// run through the ordered list and toss any fonts that exceed our allocation
		for(fontIndex = 0; fontIndex < numActualFonts; fontIndex++)
		{
			runningAllocation += orderedCacheList[fontIndex]->createdFontSize;
			if(runningAllocation > GET_FBGRAPHICS_CTX(ctx)->fontCacheMaxMemory)
			{
				// find the pointer in the original font cache list
				for(int origIndex=0; origIndex < NUM_FONT_MAP_ITEMS; origIndex++)
				{
					if(&fontMap[origIndex] == orderedCacheList[fontIndex])
					{
						fontMap[origIndex].pCreatedFont = WEBC_NULL;
						break;
					}
				}

				// toss this font
				FB_DestroyFont(ctx, orderedCacheList[fontIndex]->pCreatedFont);
				orderedCacheList[fontIndex] = WEBC_NULL;
			}
	}
}

FB_Font *FB_LoadFontFromFile(HTMLGraphicsContext** ctx, const WEBC_CHAR* familyName, const WEBC_CHAR* genericName, WEBC_UINT16 pointSize, WEBC_UINT16 attrib, WEBC_CHAR charCode)
{
   	//creation of font file name
	char filePathName[WEBC_MAX_PATH];
	int selectedFontIndex = 0;

	if(!FB_ChooseFontFromMap(familyName, genericName, pointSize, attrib, charCode, selectedFontIndex))
		return 0;

	// if we've created this font map entry before, just return that pointer to save time
	if(fontMap[selectedFontIndex].pCreatedFont)
	{
		fontMap[selectedFontIndex].FontReferenceCount += 1;
		return fontMap[selectedFontIndex].pCreatedFont;
	}

	// See if we need to flush any fonts from our cache
	FB_ProcessFontCache(ctx, fontMap[selectedFontIndex].createdFontSize);

	rtp_strcpy(filePathName, g_fontFilePath);
	rtp_strcat(filePathName, fontMap[selectedFontIndex].fileName);

	FB_Font* new_font = 0;

	//create a new font here

	RTP_FILE font_file;
	if(-1 != rtp_file_open(&font_file, filePathName,RTP_FILE_O_BINARY, RTP_FILE_S_IREAD))
	{
		int tempBaseline = 0;
		rtp_file_read(font_file,(unsigned char*)&tempBaseline,sizeof(int));

		int numChars=0;
		rtp_file_read(font_file,(unsigned char*)&numChars,sizeof(int));

		new_font = (FB_Font*)WEBC_MALLOC(sizeof(FB_Font) + (numChars * sizeof(DisplayChar)));
		if(!new_font)
			return 0;

		// asssociate the created font with the item in the map for quick retrieval in the future

		fontMap[selectedFontIndex].pCreatedFont = new_font; // TODO some reference counting here might be nice

		new_font->charMap = (WebGraphBitmapInfo*)WEBC_MALLOC(sizeof(WebGraphBitmapInfo));
		if(!new_font->charMap)
			return 0;
		new_font->charMap->pData = 0;

		new_font->baseline = tempBaseline;

		rtp_file_read(font_file,(unsigned char*)new_font->charData,sizeof(DisplayChar) * numChars);
		rtp_file_read(font_file,(unsigned char*)new_font->font_family,sizeof(WEBC_CHAR[16]));
		rtp_file_read(font_file,(unsigned char*)new_font->generic_family,sizeof(WEBC_CHAR[16]));
		rtp_file_read(font_file,(unsigned char*)&new_font->height,sizeof(int));
		rtp_file_read(font_file,(unsigned char*)&new_font->pointSize,sizeof(int));
		rtp_file_read(font_file,(unsigned char*)&new_font->style,sizeof(enum STYLIZED)); // CORRECT SIZE HERE?
		rtp_file_read(font_file,(unsigned char*)&new_font->startChar,sizeof(WEBC_CHAR));
		rtp_file_read(font_file,(unsigned char*)&new_font->endChar,sizeof(WEBC_CHAR));
		rtp_file_read(font_file,(unsigned char*)&new_font->charMap->bHasTransparentColor,sizeof(WEBC_BOOL));
		rtp_file_read(font_file,(unsigned char*)&new_font->charMap->iBytesPerRow,sizeof(WEBC_SIZE));
		rtp_file_read(font_file,(unsigned char*)&new_font->charMap->iHeight,sizeof(DISPLAY_INT));
		rtp_file_read(font_file,(unsigned char*)&new_font->charMap->iPaletteSize,sizeof(WEBC_UINT32));
		rtp_file_read(font_file,(unsigned char*)&new_font->charMap->iTransparentIndex,sizeof(WEBC_UINT16));
		rtp_file_read(font_file,(unsigned char*)&new_font->charMap->iWidth,sizeof(DISPLAY_INT));
		rtp_file_read(font_file,(unsigned char*)&new_font->charMap->pixelFormat,sizeof(WebBitmapPixelFormat));
		rtp_file_read(font_file,(unsigned char*)&new_font->charMap->uTransparentBlue,sizeof(WEBC_UINT8));
		rtp_file_read(font_file,(unsigned char*)&new_font->charMap->uTransparentGreen,sizeof(WEBC_UINT8));
		rtp_file_read(font_file,(unsigned char*)&new_font->charMap->uTransparentRed,sizeof(WEBC_UINT8));

		int dataSize = new_font->charMap->iHeight * new_font->charMap->iBytesPerRow;

		new_font->charMap->pData = (WEBC_UINT8*)WEBC_MALLOC(dataSize);

		rtp_file_read(font_file,(unsigned char*)new_font->charMap->pData,dataSize);

		fontMap[selectedFontIndex].lastCacheRequest = GET_FBGRAPHICS_CTX(ctx)->fontCacheCurrentUsageIndex++;
	}
	else{
		//This is an error condition a font needed is not found please ensure that the path to the
		//fonts is accurate, as well that the font exists.  If the path is correct and the font
		//exists, the font file may have become corrupted, please regenerate the font files.
	}
	rtp_file_close(font_file);
	font_file = WEBC_NULL;
	return new_font;
}


FB_Font *FB_SubstituteFileFont(HTMLGraphicsContext** ctx, FB_Font *pCurrentFont, WEBC_CHAR charCode)
{
	return FB_LoadFontFromFile(ctx, pCurrentFont->font_family, pCurrentFont->generic_family, pCurrentFont->pointSize, pCurrentFont->style, charCode);
}

#else


FB_Font *FB_LoadFontFromROM(HTMLGraphicsContext** ctx, const WEBC_CHAR* familyName, const WEBC_CHAR* genericName, WEBC_UINT16 pointSize, WEBC_UINT16 attrib, WEBC_CHAR charCode)
{
	FB_Font* new_font = 0;

	unsigned char *rawData =  0;
	int selectedFontIndex = 0;

	if(!FB_ChooseFontFromMap(familyName, genericName, pointSize, attrib, charCode, selectedFontIndex))
		return 0;

	// if we've created this font map entry before, just return that pointer to save time
	if(fontMap[selectedFontIndex].pCreatedFont)
	{
		fontMap[selectedFontIndex].pCreatedFont->FontReferenceCount += 1;
		return fontMap[selectedFontIndex].pCreatedFont;
	}

	rawData =  (unsigned char *) (fontMap[selectedFontIndex].binData);

	if(!rawData)
		return 0;

	int tempBaseline = *((int *)rawData);
	rawData += sizeof(int);

	int numChars=0;
	numChars = *((int *)rawData);
	rawData += sizeof(int);

	new_font = (FB_Font*)WEBC_MALLOC_VERBOSE(sizeof(FB_Font) + (numChars * sizeof(DisplayChar)),"FontMetaData");
	if(!new_font)
		return 0;
	new_font->charMap = (WebGraphBitmapInfo*)WEBC_MALLOC_VERBOSE(sizeof(WebGraphBitmapInfo),"FontGlyphMap");
	if(!new_font->charMap)
		return 0;
	// asssociate the created font with the item in the map for quick retrieval in the future
	fontMap[selectedFontIndex].pCreatedFont = new_font; // TODO some reference counting here might be nice
	fontMap[selectedFontIndex].pCreatedFont->FontReferenceCount = 1;  //Refernce count so we don't crash.
	new_font->charMap->pData = 0;

	new_font->baseline = tempBaseline;

	rtp_memcpy((void*)new_font->charData,rawData, sizeof(DisplayChar) * numChars);
	rawData += sizeof(DisplayChar) * numChars;

#if (WEBC_SUPPORT_UNICODE)
	rtp_memcpy((void*)new_font->font_family,rawData, sizeof(WEBC_CHAR16[16]));
#else // Stored in unicode so copy it in ASCII
{
	unsigned short *p = (unsigned short *) rawData;
	for (int i = 0; i < 16; i++, p++) new_font->font_family[i] = (WEBC_CHAR) *p;
}
#endif
	rawData += sizeof(WEBC_CHAR16[16]);

#if (WEBC_SUPPORT_UNICODE)
	rtp_memcpy((void*)new_font->generic_family,rawData, sizeof(WEBC_CHAR16[16]));
#else // Stored in unicode so copy it in ASCII
{
	WEBC_CHAR16 *p = (WEBC_CHAR16 *) rawData;
	for (int i = 0; i < 16; i++, p++) new_font->generic_family[i] = (WEBC_CHAR) *p;
}
#endif
	rawData += sizeof(WEBC_CHAR16[16]);

	new_font->height = *((int *)rawData);
	rawData += sizeof(int);

	new_font->pointSize = *((int *)rawData);
	rawData += sizeof(int);

	new_font->style = *((enum STYLIZED *)rawData); // CORRECT SIZE HERE?
	//rawData += sizeof(enum STYLIZED);
	rawData += sizeof(WEBC_UINT32);
#if (WEBC_SUPPORT_UNICODE)
	new_font->startChar = *((WEBC_CHAR *)rawData);
	rawData += sizeof(WEBC_CHAR);

	new_font->endChar = *((WEBC_CHAR *)rawData);
	rawData += sizeof(WEBC_CHAR);
#else
	/* Data is stored in unicode */
WEBC_CHAR16 w16;
	w16 =  *((WEBC_CHAR16 *)rawData);
	new_font->startChar = (WEBC_CHAR) w16;
	rawData += sizeof(WEBC_CHAR16);

	w16 =  *((WEBC_CHAR16 *)rawData);
	new_font->endChar = (WEBC_CHAR) w16;
	rawData += sizeof(WEBC_CHAR16);
#endif
	new_font->charMap->bHasTransparentColor = *((WEBC_BOOL *)rawData);
	rawData += sizeof(WEBC_BOOL);

	new_font->charMap->iBytesPerRow = *((WEBC_SIZE *)rawData);
	rawData += sizeof(WEBC_SIZE);

	new_font->charMap->iHeight = *((DISPLAY_INT *)rawData);
	rawData += sizeof(DISPLAY_INT);

	new_font->charMap->iPaletteSize = *((WEBC_UINT32 *)rawData);
	rawData += sizeof(WEBC_UINT32);

	new_font->charMap->iTransparentIndex = *((WEBC_UINT16 *)rawData);
	rawData += sizeof(WEBC_UINT16);

	new_font->charMap->iWidth = *((DISPLAY_INT *)rawData) & 0xFFFF;
	rawData += sizeof(DISPLAY_INT);

	new_font->charMap->pixelFormat = *((WebBitmapPixelFormat *)rawData);
	rawData += sizeof(WebBitmapPixelFormat);

	new_font->charMap->uTransparentBlue = *((WEBC_UINT8 *)rawData);
	rawData += sizeof(WEBC_UINT8);

	new_font->charMap->uTransparentGreen = *((WEBC_UINT8 *)rawData);
	rawData += sizeof(WEBC_UINT8);

	new_font->charMap->uTransparentRed = *((WEBC_UINT8 *)rawData);
	rawData += sizeof(WEBC_UINT8);

	new_font->charMap->pData = rawData; // no need to copy it

	return new_font;
}


FB_Font *FB_SubstituteROMFont(HTMLGraphicsContext** ctx, FB_Font *pCurrentFont, WEBC_CHAR charCode)
{
	return FB_LoadFontFromROM(ctx, pCurrentFont->font_family, pCurrentFont->generic_family, pCurrentFont->pointSize, pCurrentFont->style, charCode);
}


#endif


FB_Font *FB_LoadFont(HTMLGraphicsContext** ctx, const WEBC_CHAR* familyName, const WEBC_CHAR* genericName, WEBC_UINT16 pointSize, WEBC_UINT16 attrib, WEBC_CHAR charCode)
{
//#ifdef FB_FONTS_STORED_IN_FILES
#if (FB_FONTS_STORED_IN_FILES)
	return FB_LoadFontFromFile(ctx, familyName, genericName, pointSize, attrib, charCode);
#else
	return FB_LoadFontFromROM(ctx, familyName, genericName, pointSize, attrib, charCode);
#endif
}

FB_Font *FB_SubstituteFont(HTMLGraphicsContext** ctx, FB_Font *pCurrentFont, WEBC_CHAR charCode)
{
//#ifdef FB_FONTS_STORED_IN_FILES
#if (FB_FONTS_STORED_IN_FILES)
	return FB_SubstituteFileFont(ctx, pCurrentFont, charCode);
#else
	return FB_SubstituteROMFont(ctx, pCurrentFont, charCode);
#endif
}
#endif /* WINDOWSGDI	*/
