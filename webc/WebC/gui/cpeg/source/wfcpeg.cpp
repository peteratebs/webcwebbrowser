//
// WFCPEG.CPP - 
//
// Copyright EBS Inc. , 2008
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

//#ifdef WEBC_USE_PEG

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "wfcpeg.h"
#include "wcconst.hpp"
extern "C" {
#include "peg.h"
}
#include "webfonts.hpp"

/*****************************************************************************/
// Macros
/*****************************************************************************/

#define INCLUDE_ARIAL_SANS_SERIF     1 // Arial, Sans-serif
#define INCLUDE_COURIER_MONOSPACE    1 // Courier, Monospace
#define INCLUDE_TIMES_SERIF          1 // Times, Serif
#define INCLUDE_VERDANA_SANS_SERIF   1 // Verdana, Sans-serif

#if (WEBC_SUPPORT_UNICODE)
#define INCLUDE_ARIALUNICODE         1 // ArialUnicode
#else
#define INCLUDE_ARIALUNICODE         0 // ArialUnicode
#endif

#define WEBC_SMALLEST_FONT_SIZE_PT   6
#define WEBC_LARGEST_FONT_SIZE_PT    36

/*****************************************************************************/
// Types
/*****************************************************************************/

/*****************************************************************************/
// Function Prototypes
/*****************************************************************************/

/*****************************************************************************/
// Data
/*****************************************************************************/

static WEBC_READONLY WEBC_CHAR* gpFontFamilyName[WEBC_CFG_MAX_FONT_FAMILIES] = {
		WEBC_STR_ARIAL,
		WEBC_STR_COURIER,
		WEBC_STR_TIMES,
		WEBC_STR_VERDANA
	};

static WEBC_READONLY WEBC_CHAR* gpFontFamilyGenericName[WEBC_CFG_MAX_FONT_FAMILIES] = {
		WEBC_STR_SANS_SERIF,
		WEBC_STR_MONOSPACE,
		WEBC_STR_SERIF,
		WEBC_STR_SANS_SERIF
	};

//extern PegFont MenuFont;

#if (INCLUDE_ARIALUNICODE)
extern PegFont ArialUnicode;
#endif

/*---------------------------------------------------------------------------*/
#if (INCLUDE_TIMES_SERIF)

extern PegFont
	Times6,Times6b,Times6i,Times6bi,
	Times7,Times7b,Times7i,Times7bi,
	Times8,Times8b,Times8i,Times8bi,
	Times9,Times9b,Times9i,Times9bi,
	Times10,Times10b,Times10i,Times10bi,
	Times11,Times11b,Times11i,Times11bi,
	Times12,Times12b,Times12i,Times12bi,
	Times13,Times13b,Times13i,Times13bi,
	Times14,Times14b,Times14i,Times14bi,
	Times16,Times16b,Times16i,Times16bi,
	Times18,Times18b,Times18i,Times18bi,
	Times20,Times20b,Times20i,Times20bi,
	Times24,Times24b,Times24i,Times24bi,
	Times30,Times30b,Times30i,Times30bi,
	Times36,Times36b,Times36i,Times36bi;

PegFont* gpTimesFontTable [31*4] = {
	&Times6,&Times6b,&Times6i,&Times6bi,     // 6
	&Times7,&Times7b,&Times7i,&Times7bi,     // 7
	&Times8,&Times8b,&Times8i,&Times8bi,     // 8
	&Times9,&Times9b,&Times9i,&Times9bi,     // 9
	&Times10,&Times10b,&Times10i,&Times10bi, // 10
	&Times11,&Times11b,&Times11i,&Times11bi, // 11
	&Times12,&Times12b,&Times12i,&Times12bi, // 12
	&Times13,&Times13b,&Times13i,&Times13bi, // 13
	&Times14,&Times14b,&Times14i,&Times14bi, // 14
	&Times14,&Times14b,&Times14i,&Times14bi, // 15
	&Times16,&Times16b,&Times16i,&Times16bi, // 16
	&Times16,&Times16b,&Times16i,&Times16bi, // 17
	&Times18,&Times18b,&Times18i,&Times18bi, // 18
	&Times18,&Times18b,&Times18i,&Times18bi, // 19
	&Times20,&Times20b,&Times20i,&Times20bi, // 20
	&Times20,&Times20b,&Times20i,&Times20bi, // 21
	&Times20,&Times20b,&Times20i,&Times20bi, // 22
	&Times20,&Times20b,&Times20i,&Times20bi, // 23
	&Times24,&Times24b,&Times24i,&Times24bi, // 24
	&Times24,&Times24b,&Times24i,&Times24bi, // 25
	&Times24,&Times24b,&Times24i,&Times24bi, // 26
	&Times24,&Times24b,&Times24i,&Times24bi, // 27
	&Times30,&Times30b,&Times30i,&Times30bi, // 28
	&Times30,&Times30b,&Times30i,&Times30bi, // 29
	&Times30,&Times30b,&Times30i,&Times30bi, // 30
	&Times30,&Times30b,&Times30i,&Times30bi, // 31
	&Times30,&Times30b,&Times30i,&Times30bi, // 32
	&Times30,&Times30b,&Times30i,&Times30bi, // 33
	&Times36,&Times36b,&Times36i,&Times36bi, // 34
	&Times36,&Times36b,&Times36i,&Times36bi, // 35
	&Times36,&Times36b,&Times36i,&Times36bi  // 36
};

#endif // INCLUDE_TIMES_SERIF

/*---------------------------------------------------------------------------*/
#if (INCLUDE_ARIAL_SANS_SERIF)

extern PegFont
	Arial6,Arial6b,Arial6i,Arial6bi,
	Arial7,Arial7b,Arial7i,Arial7bi,
	Arial8,Arial8b,Arial8i,Arial8bi,
	Arial9,Arial9b,Arial9i,Arial9bi,
	Arial10,Arial10b,Arial10i,Arial10bi,
	Arial11,Arial11b,Arial11i,Arial11bi,
	Arial12,Arial12b,Arial12i,Arial12bi,
	Arial13,Arial13b,Arial13i,Arial13bi,
	Arial14,Arial14b,Arial14i,Arial14bi,
	Arial16,Arial16b,Arial16i,Arial16bi,
	Arial18,Arial18b,Arial18i,Arial18bi,
	Arial20,Arial20b,Arial20i,Arial20bi,
	Arial24,Arial24b,Arial24i,Arial24bi,
	Arial30,Arial30b,Arial30i,Arial30bi,
	Arial36,Arial36b,Arial36i,Arial36bi;

PegFont* gpArialFontTable [31*4] = {
	&Arial6,&Arial6b,&Arial6i,&Arial6bi,     // 6
	&Arial7,&Arial7b,&Arial7i,&Arial7bi,     // 7
	&Arial8,&Arial8b,&Arial8i,&Arial8bi,     // 8
	&Arial9,&Arial9b,&Arial9i,&Arial9bi,     // 9
	&Arial10,&Arial10b,&Arial10i,&Arial10bi, // 10
	&Arial11,&Arial11b,&Arial11i,&Arial11bi, // 11
	&Arial12,&Arial12b,&Arial12i,&Arial12bi, // 12
	&Arial13,&Arial13b,&Arial13i,&Arial13bi, // 13
	&Arial14,&Arial14b,&Arial14i,&Arial14bi, // 14
	&Arial14,&Arial14b,&Arial14i,&Arial14bi, // 15
	&Arial16,&Arial16b,&Arial16i,&Arial16bi, // 16
	&Arial16,&Arial16b,&Arial16i,&Arial16bi, // 17
	&Arial18,&Arial18b,&Arial18i,&Arial18bi, // 18
	&Arial18,&Arial18b,&Arial18i,&Arial18bi, // 19
	&Arial20,&Arial20b,&Arial20i,&Arial20bi, // 20
	&Arial20,&Arial20b,&Arial20i,&Arial20bi, // 21
	&Arial20,&Arial20b,&Arial20i,&Arial20bi, // 22
	&Arial20,&Arial20b,&Arial20i,&Arial20bi, // 23
	&Arial24,&Arial24b,&Arial24i,&Arial24bi, // 24
	&Arial24,&Arial24b,&Arial24i,&Arial24bi, // 25
	&Arial24,&Arial24b,&Arial24i,&Arial24bi, // 26
	&Arial24,&Arial24b,&Arial24i,&Arial24bi, // 27
	&Arial30,&Arial30b,&Arial30i,&Arial30bi, // 28
	&Arial30,&Arial30b,&Arial30i,&Arial30bi, // 29
	&Arial30,&Arial30b,&Arial30i,&Arial30bi, // 30
	&Arial30,&Arial30b,&Arial30i,&Arial30bi, // 31
	&Arial30,&Arial30b,&Arial30i,&Arial30bi, // 32
	&Arial30,&Arial30b,&Arial30i,&Arial30bi, // 33
	&Arial36,&Arial36b,&Arial36i,&Arial36bi, // 34
	&Arial36,&Arial36b,&Arial36i,&Arial36bi, // 35
	&Arial36,&Arial36b,&Arial36i,&Arial36bi  // 36
};

#endif // INCLUDE_ARIAL_SANS_SERIF

/*---------------------------------------------------------------------------*/
#if (INCLUDE_COURIER_MONOSPACE)

extern PegFont
	Courier6,Courier6b,Courier6i,Courier6bi,
	Courier7,Courier7b,Courier7i,Courier7bi,
	Courier8,Courier8b,Courier8i,Courier8bi,
	Courier9,Courier9b,Courier9i,Courier9bi,
	Courier10,Courier10b,Courier10i,Courier10bi,
	Courier11,Courier11b,Courier11i,Courier11bi,
	Courier12,Courier12b,Courier12i,Courier12bi,
	Courier13,Courier13b,Courier13i,Courier13bi,
	Courier14,Courier14b,Courier14i,Courier14bi,
	Courier16,Courier16b,Courier16i,Courier16bi,
	Courier18,Courier18b,Courier18i,Courier18bi,
	Courier20,Courier20b,Courier20i,Courier20bi,
	Courier24,Courier24b,Courier24i,Courier24bi,
	Courier30,Courier30b,Courier30i,Courier30bi,
	Courier36,Courier36b,Courier36i,Courier36bi;

PegFont* gpCourierFontTable [31*4] = {
	&Courier6,&Courier6b,&Courier6i,&Courier6bi,     // 6
	&Courier7,&Courier7b,&Courier7i,&Courier7bi,     // 7
	&Courier8,&Courier8b,&Courier8i,&Courier8bi,     // 8
	&Courier9,&Courier9b,&Courier9i,&Courier9bi,     // 9
	&Courier10,&Courier10b,&Courier10i,&Courier10bi, // 10
	&Courier11,&Courier11b,&Courier11i,&Courier11bi, // 11
	&Courier12,&Courier12b,&Courier12i,&Courier12bi, // 12
	&Courier13,&Courier13b,&Courier13i,&Courier13bi, // 13
	&Courier14,&Courier14b,&Courier14i,&Courier14bi, // 14
	&Courier14,&Courier14b,&Courier14i,&Courier14bi, // 15
	&Courier16,&Courier16b,&Courier16i,&Courier16bi, // 16
	&Courier16,&Courier16b,&Courier16i,&Courier16bi, // 17
	&Courier18,&Courier18b,&Courier18i,&Courier18bi, // 18
	&Courier18,&Courier18b,&Courier18i,&Courier18bi, // 19
	&Courier20,&Courier20b,&Courier20i,&Courier20bi, // 20
	&Courier20,&Courier20b,&Courier20i,&Courier20bi, // 21
	&Courier20,&Courier20b,&Courier20i,&Courier20bi, // 22
	&Courier20,&Courier20b,&Courier20i,&Courier20bi, // 23
	&Courier24,&Courier24b,&Courier24i,&Courier24bi, // 24
	&Courier24,&Courier24b,&Courier24i,&Courier24bi, // 25
	&Courier24,&Courier24b,&Courier24i,&Courier24bi, // 26
	&Courier24,&Courier24b,&Courier24i,&Courier24bi, // 27
	&Courier30,&Courier30b,&Courier30i,&Courier30bi, // 28
	&Courier30,&Courier30b,&Courier30i,&Courier30bi, // 29
	&Courier30,&Courier30b,&Courier30i,&Courier30bi, // 30
	&Courier30,&Courier30b,&Courier30i,&Courier30bi, // 31
	&Courier30,&Courier30b,&Courier30i,&Courier30bi, // 32
	&Courier30,&Courier30b,&Courier30i,&Courier30bi, // 33
	&Courier36,&Courier36b,&Courier36i,&Courier36bi, // 34
	&Courier36,&Courier36b,&Courier36i,&Courier36bi, // 35
	&Courier36,&Courier36b,&Courier36i,&Courier36bi  // 36
};

#endif // INCLUDE_COURIER_MONOSPACE

/*---------------------------------------------------------------------------*/
#if (INCLUDE_VERDANA_SANS_SERIF)

extern PegFont
	Verdana6,Verdana6b,Verdana6i,Verdana6bi,
	Verdana7,Verdana7b,Verdana7i,Verdana7bi,
	Verdana8,Verdana8b,Verdana8i,Verdana8bi,
	Verdana9,Verdana9b,Verdana9i,Verdana9bi,
	Verdana10,Verdana10b,Verdana10i,Verdana10bi,
	Verdana11,Verdana11b,Verdana11i,Verdana11bi,
	Verdana12,Verdana12b,Verdana12i,Verdana12bi,
	Verdana13,Verdana13b,Verdana13i,Verdana13bi,
	Verdana14,Verdana14b,Verdana14i,Verdana14bi,
	Verdana16,Verdana16b,Verdana16i,Verdana16bi,
	Verdana18,Verdana18b,Verdana18i,Verdana18bi,
	Verdana20,Verdana20b,Verdana20i,Verdana20bi,
	Verdana24,Verdana24b,Verdana24i,Verdana24bi,
	Verdana30,Verdana30b,Verdana30i,Verdana30bi,
	Verdana36,Verdana36b,Verdana36i,Verdana36bi;

PegFont* gpVerdanaFontTable [31*4] = {
	&Verdana6,&Verdana6b,&Verdana6i,&Verdana6bi,     // 6
	&Verdana7,&Verdana7b,&Verdana7i,&Verdana7bi,     // 7
	&Verdana8,&Verdana8b,&Verdana8i,&Verdana8bi,     // 8
	&Verdana9,&Verdana9b,&Verdana9i,&Verdana9bi,     // 9
	&Verdana10,&Verdana10b,&Verdana10i,&Verdana10bi, // 10
	&Verdana11,&Verdana11b,&Verdana11i,&Verdana11bi, // 11
	&Verdana12,&Verdana12b,&Verdana12i,&Verdana12bi, // 12
	&Verdana13,&Verdana13b,&Verdana13i,&Verdana13bi, // 13
	&Verdana14,&Verdana14b,&Verdana14i,&Verdana14bi, // 14
	&Verdana14,&Verdana14b,&Verdana14i,&Verdana14bi, // 15
	&Verdana16,&Verdana16b,&Verdana16i,&Verdana16bi, // 16
	&Verdana16,&Verdana16b,&Verdana16i,&Verdana16bi, // 17
	&Verdana18,&Verdana18b,&Verdana18i,&Verdana18bi, // 18
	&Verdana18,&Verdana18b,&Verdana18i,&Verdana18bi, // 19
	&Verdana20,&Verdana20b,&Verdana20i,&Verdana20bi, // 20
	&Verdana20,&Verdana20b,&Verdana20i,&Verdana20bi, // 21
	&Verdana20,&Verdana20b,&Verdana20i,&Verdana20bi, // 22
	&Verdana20,&Verdana20b,&Verdana20i,&Verdana20bi, // 23
	&Verdana24,&Verdana24b,&Verdana24i,&Verdana24bi, // 24
	&Verdana24,&Verdana24b,&Verdana24i,&Verdana24bi, // 25
	&Verdana24,&Verdana24b,&Verdana24i,&Verdana24bi, // 26
	&Verdana24,&Verdana24b,&Verdana24i,&Verdana24bi, // 27
	&Verdana30,&Verdana30b,&Verdana30i,&Verdana30bi, // 28
	&Verdana30,&Verdana30b,&Verdana30i,&Verdana30bi, // 29
	&Verdana30,&Verdana30b,&Verdana30i,&Verdana30bi, // 30
	&Verdana30,&Verdana30b,&Verdana30i,&Verdana30bi, // 31
	&Verdana30,&Verdana30b,&Verdana30i,&Verdana30bi, // 32
	&Verdana30,&Verdana30b,&Verdana30i,&Verdana30bi, // 33
	&Verdana36,&Verdana36b,&Verdana36i,&Verdana36bi, // 34
	&Verdana36,&Verdana36b,&Verdana36i,&Verdana36bi, // 35
	&Verdana36,&Verdana36b,&Verdana36i,&Verdana36bi  // 36
};

#endif // INCLUDE_VERDANA_SANS_SERIF

/*---------------------------------------------------------------------------*/

static PegFont** gpFontFamilyTable [WEBC_CFG_MAX_FONT_FAMILIES+1] = {
  
  #if (INCLUDE_ARIAL_SANS_SERIF)
	gpArialFontTable,
  #else
	0,
  #endif
  
  #if (INCLUDE_COURIER_MONOSPACE)
	gpCourierFontTable,
  #else
	0,
  #endif
  
  #if (INCLUDE_TIMES_SERIF)
	gpTimesFontTable,
  #else
	0,
  #endif
  
  #if (INCLUDE_VERDANA_SANS_SERIF)
	gpVerdanaFontTable,
  #else
	0,
  #endif
  
	0
};

/*****************************************************************************/
// Function Definitions
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
void* CPEG_CreateFont (
		HTMLGraphicsContext** ctx,
		const WEBC_CHAR* familyName,
		const WEBC_CHAR* genericName,
		WEBC_UINT16 pointSize,
		WEBC_UINT16 attrib,
		int familyIndex
	)
{
	PegFont* font;
	
	if (familyIndex < 0)
	{
		familyIndex = webc_font_family_index (familyName, genericName, gpFontFamilyName, gpFontFamilyGenericName, WEBC_CFG_MAX_FONT_FAMILIES);
		if (familyIndex < 0)
		{
			familyIndex = 0;
		}
	}

	PegFont** fontFamily = gpFontFamilyTable[familyIndex];

	if (!fontFamily)
	{
		fontFamily = gpTimesFontTable;
	}

	pointSize = EBSCLIP(pointSize, WEBC_SMALLEST_FONT_SIZE_PT+1, WEBC_LARGEST_FONT_SIZE_PT);
	pointSize -= WEBC_SMALLEST_FONT_SIZE_PT;
	
	font = fontFamily[(pointSize << 2) | attrib];
	
  #if (WEBC_SUPPORT_UNICODE && INCLUDE_ARIALUNICODE)
	if (!font->pNext)
	{
		font->pNext = &ArialUnicode;
	}
  #endif // WEBC_SUPPORT_UNICODE 

	return (font);	
}

/*---------------------------------------------------------------------------*/
void* CPEG_GetDefaultFont (
		HTMLGraphicsContext** ctx
	)
{
	PegFont* font;
	
  #if (WEBC_SUPPORT_UNICODE && INCLUDE_ARIALUNICODE)
	font = &Arial9;
	
	if (!font->pNext)
	{
		font->pNext = &ArialUnicode;
	}
  #else
//	font = &MenuFont;  
	font = &Arial9;
	if (!font->pNext)
	{
		font->pNext = &Arial10;
	}
  #endif // WEBC_SUPPORT_UNICODE 

	return (font);
}

/*---------------------------------------------------------------------------*/
void CPEG_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* font
	)
{
}

/*---------------------------------------------------------------------------*/
int CPEG_NumFontFamilies (
		HTMLGraphicsContext** ctx
	)
{
	return WEBC_CFG_MAX_FONT_FAMILIES;
}
	
/*---------------------------------------------------------------------------*/
const WEBC_CHAR** CPEG_GetFontFamilyNames (
		HTMLGraphicsContext** ctx
	)
{
	return gpFontFamilyName;
}

/*---------------------------------------------------------------------------*/
const WEBC_CHAR** CPEG_GetFontFamilyGenericNames (
		HTMLGraphicsContext** ctx
	)
{
	return gpFontFamilyGenericName;
}

//#endif // WEBC_USE_PEG
