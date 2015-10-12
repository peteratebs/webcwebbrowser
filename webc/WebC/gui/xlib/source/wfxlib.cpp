//
// WFXLIB.CPP -
//
// EBS -
//
//  $Author: vmalaiya $
//  $Date: 2006/11/20 19:02:27 $
//  $Name:  $
//  $Revision: 1.4 $
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "webcxlib.h"
#include "wfxlib.h"
#include "wcconst.hpp"
#include "webfonts.hpp"
#include "webcmem.h"


/*****************************************************************************/
// Macros
/*****************************************************************************/

#define WEBC_SMALLEST_FONT_SIZE_PT   6
#define WEBC_LARGEST_FONT_SIZE_PT    36


#define XFONT_WEIGHT_BOLD 			"bold"
#define XFONT_WEIGHT_MEDIUM			"medium"
#define XFONT_SLANT_ITALIC			"i"
#define XFONT_SLANT_OBLIQUE			"o"
#define XFONT_SLANT_ROMAN			"r"


/*****************************************************************************/
// Types
/*****************************************************************************/

/*****************************************************************************/
// Function Prototypes
/*****************************************************************************/
char *_XLIB_GetFontSlant(Display *display, char *fontName);
int _XLIB_GetFontSize (Display *display, char *fontName, int fontSize);

/*****************************************************************************/
// Data
/*****************************************************************************/
static WEBC_READONLY WEBC_CHAR* xFontFamilyName[WEBC_CFG_MAX_FONT_FAMILIES] = {
#if (WEBC_SUPPORT_UNICODE)
		(WEBC_CHAR*) L"Helvetica",
		(WEBC_CHAR*) L"Lucida",
#else
		"Helvetica",
		"Lucida",
#endif
		WEBC_STR_COURIER,
		WEBC_STR_TIMES,
	};

static WEBC_READONLY WEBC_CHAR* xFontFamilyGenericName[WEBC_CFG_MAX_FONT_FAMILIES] = {
		WEBC_STR_SANS_SERIF,
		WEBC_STR_SANS_SERIF,
		WEBC_STR_MONOSPACE,
		WEBC_STR_SERIF,
	};

XFontStruct *xlib_default_font = 0;

/*****************************************************************************/
// Function Definitions
/*****************************************************************************/


/*---------------------------------------------------------------------------*/
void* XLIB_CreateFont (
		HTMLGraphicsContext** ctx,
		const WEBC_CHAR* familyName,
		const WEBC_CHAR* genericName,
		WEBC_UINT16 pointSize,
		WEBC_UINT16 attrib,
		int familyIndex
	)
{
	WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
	char **fontName;
	int  numFonts;
	char *weight = XFONT_WEIGHT_MEDIUM;
	char *slant = XFONT_SLANT_ROMAN;
	char fontTarget[100];
	int supportedFontSize;
	int fontID;
	if (familyIndex < 0)
	{
		familyIndex = webc_font_family_index (familyName, genericName, xFontFamilyName, xFontFamilyGenericName, WEBC_CFG_MAX_FONT_FAMILIES);
		if (familyIndex < 0)
		{
			familyIndex = 0;
		}
	}
	/* Get a font id based on family name */
	if(attrib == 1) /* bold */
	{
		weight = XFONT_WEIGHT_BOLD;
	}
	/* If the font has a slant, in X Windows there is no way to determine if a particular font supports
	   oblique or italic field for slant */
	if(attrib == 2 || attrib == 3)
	{
		slant = _XLIB_GetFontSlant(pxctx->display, (char*)xFontFamilyName[familyIndex]);
	}

	/* XLib may not contain a font with exact font size, in such case get best match for available font
	   size */
	supportedFontSize = _XLIB_GetFontSize(pxctx->display, (char*)xFontFamilyName[familyIndex], pointSize);
	if(supportedFontSize < 0)
	{
		supportedFontSize = pointSize; /* no matching pixel size found, take a chance with supplied size and
											let it pass to default case */
	}
	sprintf(fontTarget, "-*-%s-%s-%s-*-*-%d-*-*-*-*-*-*-*", xFontFamilyName[familyIndex], weight, slant, supportedFontSize);
	fontName = XListFonts (pxctx->display, fontTarget, 1, &numFonts);
	if (fontName == NULL) /* no match for family name */
	{
		/* Get a known font */
		sprintf(fontTarget, "-*-courier-%s-*-*-*-*-*-*-*-*-*-*-*", weight);
		fontName = XListFonts (pxctx->display, fontTarget, 1, &numFonts);
		if (fontName == NULL) /* no match for generic name */
		{
			return (0);
		}
	}
	pxctx->xlib_font_info = XLoadQueryFont(pxctx->display, fontName[0]);

	XFreeFontNames(fontName);
 	return ((void*)pxctx->xlib_font_info);
}

/*---------------------------------------------------------------------------*/
void* XLIB_GetDefaultFont (
		HTMLGraphicsContext** ctx
	)
{
	//WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
	//return ((void*)pxctx->xlib_font_info);
	return((void*)xlib_default_font);
}

/*---------------------------------------------------------------------------*/
void XLIB_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* f
	)
{
	XFontStruct *xlib_new_font = (XFontStruct *)f;
	WebcXLIBGraphics *pxctx = (WebcXLIBGraphics *)ctx;
	XFreeFont(pxctx->display, xlib_new_font);
}

/*---------------------------------------------------------------------------*/
int XLIB_NumFontFamilies (
		HTMLGraphicsContext** ctx
	)
{
	return WEBC_CFG_MAX_FONT_FAMILIES;
}

/*---------------------------------------------------------------------------*/
const WEBC_CHAR** XLIB_GetFontFamilyNames (
		HTMLGraphicsContext** ctx
	)
{
	return xFontFamilyName;
}

/*---------------------------------------------------------------------------*/
const WEBC_CHAR** XLIB_GetFontFamilyGenericNames (
		HTMLGraphicsContext** ctx
	)
{
	return xFontFamilyGenericName;
}

/*---------------------------------------------------------------------------*/
/* X Lib specific function to obtain the best fit for slant type */
char *_XLIB_GetFontSlant(Display *display, char *fontName)
{
	char **font;
	char fontTarget[50];
	int numFonts;

	sprintf(fontTarget, "-*-%s-*-%s-*-*-*-*-*-*-*-*-*-*", fontName, XFONT_SLANT_ITALIC);
	font = XListFonts (display, fontTarget, 1, &numFonts);
	if (font == NULL) /* no match for family name */
	{
		sprintf(fontTarget, "-*-%s-*-%s-*-*-*-*-*-*-*-*-*-*", fontName, XFONT_SLANT_OBLIQUE);
		font = XListFonts (display, fontTarget, 1, &numFonts);
		if (font == NULL) /* no match for family name */
		{
			return (0);
		}
		XFreeFontNames(font);
		return (XFONT_SLANT_OBLIQUE);
	}
	XFreeFontNames(font);
	return (XFONT_SLANT_ITALIC);
}

/*---------------------------------------------------------------------------*/
/* X Lib specific function to obtain the best fit for support font pixel size */
int _XLIB_GetFontSize (Display *display, char *fontName, int fontSize)
{
	char **font;
	char fontTarget[50];
	int newSize = fontSize;
	int loSize = fontSize;
	int hiSize = fontSize;
	int numFonts;
	sprintf(fontTarget, "-*-%s-*-*-*-*-%d-*-*-*-*-*-*-*", fontName, newSize);
	font = XListFonts (display, fontTarget, 1, &numFonts);
	if (font != NULL) /* no match for this family name with pixel width */
	{
		XFreeFontNames(font);
		return (newSize);
	}

	while(loSize >= WEBC_SMALLEST_FONT_SIZE_PT)
	{
		loSize--;
		/* check closest pixel size downward */
		sprintf(fontTarget, "-*-%s-*-*-*-*-%d-*-*-*-*-*-*-*", fontName, loSize);
		font = XListFonts (display, fontTarget, 1, &numFonts);
		if (font != NULL)
		{
			XFreeFontNames(font);
			break;
		}
	}

	while(hiSize <= WEBC_LARGEST_FONT_SIZE_PT)
	{
		hiSize++;
		/* check closest pixel size upward */
		sprintf(fontTarget, "-*-%s-*-*-*-*-%d-*-*-*-*-*-*-*", fontName, hiSize);
		font = XListFonts (display, fontTarget, 1, &numFonts);
		if (font != NULL)
		{
			XFreeFontNames(font);
			break;
		}
	}

	if(loSize < WEBC_SMALLEST_FONT_SIZE_PT) /* no match found on the down search */
	{
		if (hiSize <= WEBC_LARGEST_FONT_SIZE_PT)
		{
			return (hiSize);
		}
		else
		{
			return (-1);
		}
	}

	if(hiSize > WEBC_LARGEST_FONT_SIZE_PT) /* no match found on the up search */
	{
		if (loSize >= WEBC_SMALLEST_FONT_SIZE_PT)
		{
			return (loSize);
		}
		else
		{
			return (-1);
		}
	}

	if((fontSize - loSize) < (fontSize - hiSize)) /* get the pixel size closest to requested size */
	{
		return (loSize);
	}
	else
	{
		return (hiSize);
	}
}
