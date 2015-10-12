//
// WFGUI.CPP - 
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

#include "wfgui.h"
#include "wcconst.hpp"
#include "webfonts.hpp"

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

/*****************************************************************************/
// Function Definitions
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
void* GUINAME_CreateFont (
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

	return 0;
}

/*---------------------------------------------------------------------------*/
void* GUINAME_GetDefaultFont (
		HTMLGraphicsContext** ctx
	)
{
	return 0;
}

/*---------------------------------------------------------------------------*/
void GUINAME_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* font
	)
{
}

/*---------------------------------------------------------------------------*/
int GUINAME_NumFontFamilies (
		HTMLGraphicsContext** ctx
	)
{
	return WEBC_CFG_MAX_FONT_FAMILIES;
}
	
/*---------------------------------------------------------------------------*/
const WEBC_CHAR** GUINAME_GetFontFamilyNames (
		HTMLGraphicsContext** ctx
	)
{
	return gpFontFamilyName;
}

/*---------------------------------------------------------------------------*/
const WEBC_CHAR** GUINAME_GetFontFamilyGenericNames (
		HTMLGraphicsContext** ctx
	)
{
	return gpFontFamilyGenericName;
}

