//
// WFGRAFX.CPP - 
//
// EBS - 
//
//  $Author: vmalaiya $
//  $Date: 2006/08/31 22:34:35 $
//  $Name:  $
//  $Revision: 1.1 $
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "wfgrafx.h"
#include "wcconst.hpp"
#include "webfonts.hpp"

/*****************************************************************************/
// Macros
/*****************************************************************************/

#define INCLUDE_ARIAL_SANS_SERIF     1 // Arial, Sans-serif
#define INCLUDE_COURIER_MONOSPACE    1 // Courier, Monospace

#define WEBC_SMALLEST_FONT_SIZE_PT   6
#define WEBC_LARGEST_FONT_SIZE_PT    36

/*****************************************************************************/
// Types
/*****************************************************************************/

/*****************************************************************************/
// Function Prototypes
/*****************************************************************************/

static void _webc_nucleus_font_init (void);

/*****************************************************************************/
// Data
/*****************************************************************************/

WEBC_READONLY WEBC_CHAR* gpFontFamilyName[WEBC_CFG_MAX_FONT_FAMILIES] = {
		WEBC_STR_ARIAL,
		WEBC_STR_COURIER
	};

WEBC_READONLY WEBC_CHAR* gpFontFamilyGenericName[WEBC_CFG_MAX_FONT_FAMILIES] = {
		WEBC_STR_SANS_SERIF,
		WEBC_STR_MONOSPACE
	};

static unsigned webcNucleusFontsInitialized = 0;

/*---------------------------------------------------------------------------*/
#if (INCLUDE_COURIER_MONOSPACE)

extern "C"
{
	extern fontRcd * gpCourier8;
}

WebcNucleusFont 
	Fixed,Fixedb,Fixedi,Fixedbi;

WebcNucleusFont* gpFixedFontTable [31*4] = {
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 6
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 7
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 8
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 9
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 10
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 11
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 12
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 13
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 14
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 15
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 16
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 17
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 18
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 19
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 20
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 21
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 22
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 23
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 24
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 25
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 26
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 27
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 28
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 29
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 30
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 31
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 32
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 33
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 34
	&Fixed,&Fixedb,&Fixedi,&Fixedbi, // 35
	&Fixed,&Fixedb,&Fixedi,&Fixedbi  // 36
};

#endif // INCLUDE_COURIER_MONOSPACE

/*---------------------------------------------------------------------------*/
#if (INCLUDE_ARIAL_SANS_SERIF)

extern "C"
{
	extern fontRcd * gpArial6;
	extern fontRcd * gpArial7;
	extern fontRcd * gpArial8;
	extern fontRcd * gpArial10;
	extern fontRcd * gpArial12;
	extern fontRcd * gpArial14;
	extern fontRcd * gpArial18;
}

WebcNucleusFont 
	Sans6,  Sans6b,  Sans6i,  Sans6bi,
	Sans7,  Sans7b,  Sans7i,  Sans7bi,
	Sans8,  Sans8b,  Sans8i,  Sans8bi,
	Sans10, Sans10b, Sans10i, Sans10bi,
	Sans12, Sans12b, Sans12i, Sans12bi,
	Sans14, Sans14b, Sans14i, Sans14bi,
	Sans18, Sans18b, Sans18i, Sans18bi;

WebcNucleusFont* gpSansFontTable [31*4] = {
	&Sans6,&Sans6b,&Sans6i,&Sans6bi,     // 6
	&Sans7,&Sans7b,&Sans7i,&Sans7bi,     // 7
	&Sans8,&Sans8b,&Sans8i,&Sans8bi,     // 8
	&Sans8,&Sans8b,&Sans8i,&Sans8bi,     // 9
	&Sans10,&Sans10b,&Sans10i,&Sans10bi, // 10
	&Sans10,&Sans10b,&Sans10i,&Sans10bi, // 11
	&Sans12,&Sans12b,&Sans12i,&Sans12bi, // 12
	&Sans12,&Sans12b,&Sans12i,&Sans12bi, // 13
	&Sans14,&Sans14b,&Sans14i,&Sans14bi, // 14
	&Sans14,&Sans14b,&Sans14i,&Sans14bi, // 15
	&Sans14,&Sans14b,&Sans14i,&Sans14bi, // 14
	&Sans14,&Sans14b,&Sans14i,&Sans14bi, // 15
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 18
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 19
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 20
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 21
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 22
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 23
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 24
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 25
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 26
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 27
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 28
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 29
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 30
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 31
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 32
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 33
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 34
	&Sans18,&Sans18b,&Sans18i,&Sans18bi, // 35
	&Sans18,&Sans18b,&Sans18i,&Sans18bi  // 36
};

#endif // INCLUDE_ARIAL_SANS_SERIF

/*---------------------------------------------------------------------------*/
static WebcNucleusFont** gpFontFamilyTable [WEBC_CFG_MAX_FONT_FAMILIES+1] = {
  
  #if (INCLUDE_ARIAL_SANS_SERIF)
	gpSansFontTable,
  #else
	0,
  #endif
  
  #if (INCLUDE_COURIER_MONOSPACE)
	gpFixedFontTable,
  #else
	0,
  #endif
  
	0
};

/*****************************************************************************/
// Function Definitions
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
void* GRAFX_CreateFont (
		HTMLGraphicsContext** ctx,
		const WEBC_CHAR* familyName,
		const WEBC_CHAR* genericName,
		WEBC_UINT16 pointSize,
		WEBC_UINT16 attrib,
		int familyIndex
	)
{
	_webc_nucleus_font_init();

	if (familyIndex < 0)
	{
		familyIndex = webc_font_family_index (familyName, genericName, gpFontFamilyName, gpFontFamilyGenericName, WEBC_CFG_MAX_FONT_FAMILIES);
		if (familyIndex < 0)
		{
			familyIndex = 0;
		}
	}

	WebcNucleusFont** fontFamily = gpFontFamilyTable[familyIndex];

	if (!fontFamily)
	{
		fontFamily = gpSansFontTable;
	}

	pointSize = EBSCLIP(pointSize, WEBC_SMALLEST_FONT_SIZE_PT+1, WEBC_LARGEST_FONT_SIZE_PT);
	pointSize -= WEBC_SMALLEST_FONT_SIZE_PT;
	
	WebcNucleusFont* font = fontFamily[(pointSize << 2) | attrib];
	
	return (font);
}

/*---------------------------------------------------------------------------*/
void* GRAFX_GetDefaultFont (
		HTMLGraphicsContext** ctx
	)
{
	_webc_nucleus_font_init();
	return (&Sans10);
}

/*---------------------------------------------------------------------------*/
void GRAFX_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* font
	)
{
}

/*---------------------------------------------------------------------------*/
int GRAFX_NumFontFamilies (
		HTMLGraphicsContext** ctx
	)
{
	return WEBC_CFG_MAX_FONT_FAMILIES;
}
	
/*---------------------------------------------------------------------------*/
const WEBC_CHAR** GRAFX_GetFontFamilyNames (
		HTMLGraphicsContext** ctx
	)
{
	return gpFontFamilyName;
}

/*---------------------------------------------------------------------------*/
const WEBC_CHAR** GRAFX_GetFontFamilyGenericNames (
		HTMLGraphicsContext** ctx
	)
{
	return gpFontFamilyGenericName;
}

/*---------------------------------------------------------------------------*/
void _webc_nucleus_font_init (void)
{
	if (!webcNucleusFontsInitialized)
	{
		webcNucleusFontsInitialized = 1;
		
	  #if (INCLUDE_ARIAL_SANS_SERIF)
	  
		Sans6.fnt    = gpArial6;
		Sans6i.fnt   = gpArial6;
		Sans6b.fnt   = gpArial6;
		Sans6bi.fnt  = gpArial6;
		Sans7.fnt    = gpArial7;
		Sans7i.fnt   = gpArial7;
		Sans7b.fnt   = gpArial7;
		Sans7bi.fnt  = gpArial7;
		Sans8.fnt    = gpArial8;
		Sans8i.fnt   = gpArial8;
		Sans8b.fnt   = gpArial8;
		Sans8bi.fnt  = gpArial8;
		Sans10.fnt   = gpArial10;
		Sans10i.fnt  = gpArial10;
		Sans10b.fnt  = gpArial10;
		Sans10bi.fnt = gpArial10;
		Sans12.fnt   = gpArial12;
		Sans12i.fnt  = gpArial12;
		Sans12b.fnt  = gpArial12;
		Sans12bi.fnt = gpArial12;
		Sans14.fnt   = gpArial14;
		Sans14i.fnt  = gpArial14;
		Sans14b.fnt  = gpArial14;
		Sans14bi.fnt = gpArial14;
		Sans18.fnt   = gpArial18;
		Sans18i.fnt  = gpArial18;
		Sans18b.fnt  = gpArial18;
		Sans18bi.fnt = gpArial18;
		
		Sans6.face   = cNormal + cProportional;
		Sans6i.face  = cItalic + cProportional;
		Sans6b.face  = cBold + cProportional;
		Sans6bi.face = cBold + cItalic + cProportional;
		Sans7.face   = cNormal + cProportional;
		Sans7i.face  = cItalic + cProportional;
		Sans7b.face  = cBold + cProportional;
		Sans7bi.face = cBold + cItalic + cProportional;
		Sans8.face   = cNormal + cProportional;
		Sans8i.face  = cItalic + cProportional;
		Sans8b.face  = cBold + cProportional;
		Sans8bi.face = cBold + cItalic + cProportional;
		Sans10.face   = cNormal + cProportional;
		Sans10i.face  = cItalic + cProportional;
		Sans10b.face  = cBold + cProportional;
		Sans10bi.face = cBold + cItalic + cProportional;
		Sans12.face   = cNormal + cProportional;
		Sans12i.face  = cItalic + cProportional;
		Sans12b.face  = cBold + cProportional;
		Sans12bi.face = cBold + cItalic + cProportional;
		Sans14.face   = cNormal + cProportional;
		Sans14i.face  = cItalic + cProportional;
		Sans14b.face  = cBold + cProportional;
		Sans14bi.face = cBold + cItalic + cProportional;
		Sans18.face   = cNormal + cProportional;
		Sans18i.face  = cItalic + cProportional;
		Sans18b.face  = cBold + cProportional;
		Sans18bi.face = cBold + cItalic + cProportional;
		
	  #endif // INCLUDE_ARIAL_SANS_SERIF
	 
	  #if (INCLUDE_COURIER_MONOSPACE)
	  
		Fixed.fnt    = gpCourier8;
		Fixedi.fnt   = gpCourier8;
		Fixedb.fnt   = gpCourier8;
		Fixedi.fnt  = gpCourier8;
	
		Fixed.face   = cNormal + cProportional;
		Fixedi.face  = cItalic + cProportional;
		Fixedb.face  = cBold + cProportional;
		Fixedbi.face = cBold + cItalic + cProportional;	
			
	  #endif // INCLUDE_COURIER_MONOSPACE
	}
}
