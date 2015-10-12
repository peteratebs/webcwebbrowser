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

#include "windows.h"
#include "webcgdi_wce.h"
#include "wfgdi_wce.h"
#include "wcconst.hpp"
#include "webfonts.hpp"
#include "wgdifont.h"
#include "webcmem.h"

#ifndef MulDiv
#define MulDiv(a,b,c)       (((a)*(b))/(c))
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

/*****************************************************************************/
// Function Definitions
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
void* GDI_WCE_CreateFont (
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

	if (!familyName)
	{
		return (0);
	}

	WebcGDIFont* font = (WebcGDIFont*) WEBC_MALLOC(sizeof(WebcGDIFont));
	if (font)
	{
		LOGFONT fontInfo;
		
		memset(&fontInfo, 0, sizeof(LOGFONT));
		
		fontInfo.lfHeight = -MulDiv(pointSize, GetDeviceCaps(GET_OUTPUT_DC(ctx), LOGPIXELSY), 72);
		fontInfo.lfWeight = (attrib & WEBC_FONT_WEIGHT_BOLD)? FW_BOLD : FW_NORMAL;
		fontInfo.lfItalic = (attrib & WEBC_FONT_STYLE_ITALIC)? TRUE : FALSE; 
		fontInfo.lfCharSet = DEFAULT_CHARSET; 
	  #ifdef WEBC_FAST_SYSTEM		
		fontInfo.lfQuality = ANTIALIASED_QUALITY; 
	  #else
		fontInfo.lfQuality = NONANTIALIASED_QUALITY; 
	  #endif		
		fontInfo.lfPitchAndFamily; 
		
		wcsncpy(fontInfo.lfFaceName, familyName, LF_FACESIZE-1);
		fontInfo.lfFaceName[LF_FACESIZE-1] = 0;

		font->hFont = CreateFontIndirect (&fontInfo);

		if (font->hFont == NULL && genericName)
		{
			wcsncpy(fontInfo.lfFaceName, genericName, LF_FACESIZE-1);
			fontInfo.lfFaceName[LF_FACESIZE-1] = 0;
	
			font->hFont = CreateFontIndirect (&fontInfo);
		}
		
		if (font->hFont != NULL)
		{
			font->fontHeight = CSS_LengthPointsToPixels(pointSize) + 2;		
			return font;
		}
		
		WEBC_FREE(font);
	}
	
	return 0;
}

/*---------------------------------------------------------------------------*/
void* GDI_WCE_GetDefaultFont (
		HTMLGraphicsContext** ctx
	)
{
	return (GDI_WCE_CreateFont(ctx, L"Arial", L"sans-serif", 8, 0, -1));
}

/*---------------------------------------------------------------------------*/
void GDI_WCE_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* f
	)
{
	WebcGDIFont* font = (WebcGDIFont*) f;
	DeleteObject(font->hFont);
	WEBC_FREE(font);
}

/*---------------------------------------------------------------------------*/
int GDI_WCE_NumFontFamilies (
		HTMLGraphicsContext** ctx
	)
{
	return WEBC_CFG_MAX_FONT_FAMILIES;
}
	
/*---------------------------------------------------------------------------*/
const WEBC_CHAR** GDI_WCE_GetFontFamilyNames (
		HTMLGraphicsContext** ctx
	)
{
	return gpFontFamilyName;
}

/*---------------------------------------------------------------------------*/
const WEBC_CHAR** GDI_WCE_GetFontFamilyGenericNames (
		HTMLGraphicsContext** ctx
	)
{
	return gpFontFamilyGenericName;
}
