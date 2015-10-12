//
// WFGDIP.CPP -
//
// EBS -
//
//  $Author: vmalaiya $
//  $Date: 2006/08/31 22:49:21 $
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
#ifndef GDIFRAMEBUFFER

#include "wfgdip.h"
#include "wcconst.hpp"
#include "webfonts.hpp"
#define UNICODE
#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

#if (!WEBC_SUPPORT_UNICODE)
// #error GDI plus integration may only be run with WEBC_SUPPORT_UNICODE=1
#endif

/*****************************************************************************/
// Macros
/*****************************************************************************/

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

static FontFamily* gGDIPlusFontFamilyTable[WEBC_CFG_MAX_FONT_FAMILIES] = {
		0
	};

static Font* gGDIPlusDefaultFont = 0;

/*****************************************************************************/
// Function Definitions
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
void* GDIPlus_GetDefaultFont (
		HTMLGraphicsContext** ctx
	)
{
	if (!gGDIPlusDefaultFont)
	{
		gGDIPlusDefaultFont = new Font(L"Arial", 10);
	}

	return gGDIPlusDefaultFont;
}

/*---------------------------------------------------------------------------*/
void* GDIPlus_CreateFont (
		HTMLGraphicsContext** ctx,
		const WEBC_CHAR* familyName,
		const WEBC_CHAR* genericName,
		WEBC_UINT16 pointSize,
		WEBC_UINT16 attrib,
		int familyIndex
	)
{
	if (familyIndex < 0)
	{
		familyIndex = webc_font_family_index (familyName, genericName, gpFontFamilyName, gpFontFamilyGenericName, WEBC_CFG_MAX_FONT_FAMILIES);
		if (familyIndex < 0)
		{
			familyIndex = 0;
		}
	}

	if (!gGDIPlusFontFamilyTable[familyIndex])
	{
#if (WEBC_SUPPORT_UNICODE)
		if (!webc_c_stricmp(familyName, "courier"))
		{
			familyName = (WEBC_CHAR *)L"Courier New";
		}
		gGDIPlusFontFamilyTable[familyIndex] = new FontFamily((const WCHAR *)familyName);
		if (!gGDIPlusFontFamilyTable[familyIndex])
		{
			return 0;
		}
#else
		/* Hacking font for now, non unicode  */
		gGDIPlusFontFamilyTable[familyIndex] = new FontFamily(L"Courier New");
		if (!gGDIPlusFontFamilyTable[familyIndex])
		{
			return 0;
		}
#endif

	}

	static INT fontStyleMap[4] =
	{
		FontStyleRegular,
		FontStyleBold,
		FontStyleItalic,
		FontStyleBoldItalic
	};

	Font* f = new Font(gGDIPlusFontFamilyTable[familyIndex], pointSize, fontStyleMap[attrib & 0x3], UnitPoint);

	if (f->IsAvailable())
	{
		return f;
	}
	else
	{
		return GDIPlus_GetDefaultFont(ctx);
	}
}

/*---------------------------------------------------------------------------*/
void GDIPlus_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* font
	)
{
	delete (Font*) font;
}

/*---------------------------------------------------------------------------*/
int GDIPlus_NumFontFamilies (
		HTMLGraphicsContext** ctx
	)
{
	return WEBC_CFG_MAX_FONT_FAMILIES;
}

/*---------------------------------------------------------------------------*/
const WEBC_CHAR** GDIPlus_GetFontFamilyNames (
		HTMLGraphicsContext** ctx
	)
{
	return gpFontFamilyName;
}

/*---------------------------------------------------------------------------*/
const WEBC_CHAR** GDIPlus_GetFontFamilyGenericNames (
		HTMLGraphicsContext** ctx
	)
{
	return gpFontFamilyGenericName;
}
#endif // GDIFRAMEBUFFER