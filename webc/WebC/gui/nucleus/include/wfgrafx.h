/*
|  WFGRAFX.H - 
| 
|  EBS - 
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __WFGRAFX_H__
#define __WFGRAFX_H__

#include "webc.h"
#include "winlib.h"

typedef struct 
{
	fontRcd  * fnt;
	int        face;
} 
WebcNucleusFont;

#ifdef __cplusplus
extern "C" {
#endif

void* GRAFX_CreateFont (
		HTMLGraphicsContext** ctx,
		const WEBC_CHAR* familyName,
		const WEBC_CHAR* genericName,
		WEBC_UINT16 pointSize,
		WEBC_UINT16 attrib,
		int familyIndex
	);

void* GRAFX_GetDefaultFont (
		HTMLGraphicsContext** ctx
	);

void GRAFX_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* font
	);

int GRAFX_NumFontFamilies (
		HTMLGraphicsContext** ctx
	);
	
const WEBC_CHAR** GRAFX_GetFontFamilyNames (
		HTMLGraphicsContext** ctx
	);

const WEBC_CHAR** GRAFX_GetFontFamilyGenericNames (
		HTMLGraphicsContext** ctx
	);

#ifdef __cplusplus
}
#endif

#endif /* __WFGRAFX_H__ */
