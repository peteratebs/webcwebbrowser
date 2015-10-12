#include "webc.h"


enum STYLIZED {REGULAR = 0,BOLD, ITALIC, BOLDITALIC};

typedef struct dispchar{
	int width;
	int height;
	int x;
	int y;
}DisplayChar;

/*
* This structure is the structure to contain the neccesary information about a font.
*/
typedef struct fb_font{
	WEBC_CHAR startChar;
	WEBC_CHAR endChar;
	int FontReferenceCount;
	STYLIZED style;
	WEBC_CHAR font_family[16];
	WEBC_CHAR generic_family[16];
	int pointSize;
	int baseline;
	int height;
	WebGraphBitmapInfo* charMap;
	DisplayChar charData[128];
} FB_Font;

/**
*  This function is called by WebC to retrieve a handle or pointer to a default
*  font that it will use to draw text in list boxes and drop boxes. The returned
*  value will later be passed to other graphics abstraction layer functions that
*  take a font handle/pointer as one of their arguments.
*/
void* FB_GetDefaultFont (
	HTMLGraphicsContext** ctx
);

/**
*	Used to get the number of font families available.
*/
int FB_NumFontFamilies(
	HTMLGraphicsContext** ctx
);

/**
*	This function is called by WebC to retrieve a table of the font families
*	that are available by name. The table must contain WEBC_CFG_MAX_FONT_FAMILIES entries.
*/
const WEBC_CHAR** FB_GetFontFamilyNames (
	HTMLGraphicsContext** ctx
);

/**
*	This function is called by WebC to retrieve a table of the generic font
*	families that are available by name. The table must contain
*	WEBC_CFG_MAX_FONT_FAMILIES entries.
*/
const WEBC_CHAR** FB_GetFontFamilyGenericNames (
	HTMLGraphicsContext** ctx
);


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
		int familyIndex
);

void FB_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* font
);


FB_Font *FB_SubstituteFont(HTMLGraphicsContext** ctx, FB_Font *pCurrentFont, WEBC_CHAR charCode);
FB_Font *FB_LoadFont(HTMLGraphicsContext** ctx, const WEBC_CHAR* familyName, const WEBC_CHAR* genericName, WEBC_UINT16 pointSize, WEBC_UINT16 attrib, WEBC_CHAR charCode);





