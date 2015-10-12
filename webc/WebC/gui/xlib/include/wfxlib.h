/*
|  WFXLIB.H - 
| 
|  EBS - 
| 
|  Copyright EBS Inc. , 2006
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __WFGUI_H__
#define __WFGUI_H__

#include "webc.h"

#ifdef __cplusplus
extern "C" {
#endif

void* XLIB_CreateFont (
		HTMLGraphicsContext** ctx,
		const WEBC_CHAR* familyName,
		const WEBC_CHAR* genericName,
		WEBC_UINT16 pointSize,
		WEBC_UINT16 attrib,
		int familyIndex
	);

void* XLIB_GetDefaultFont (
		HTMLGraphicsContext** ctx
	);

void XLIB_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* font
	);

int XLIB_NumFontFamilies (
		HTMLGraphicsContext** ctx
	);
	
const WEBC_CHAR** XLIB_GetFontFamilyNames (
		HTMLGraphicsContext** ctx
	);

const WEBC_CHAR** XLIB_GetFontFamilyGenericNames (
		HTMLGraphicsContext** ctx
	);

#ifdef __cplusplus
}
#endif

#endif /* __WFGUI_H__ */
