/*
|  WFGUI.H - 
| 
|  EBS - 
| 
|  Copyright EBS Inc. , 2004
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

void* GDI_WCE_CreateFont (
		HTMLGraphicsContext** ctx,
		const WEBC_CHAR* familyName,
		const WEBC_CHAR* genericName,
		WEBC_UINT16 pointSize,
		WEBC_UINT16 attrib,
		int familyIndex
	);

void* GDI_WCE_GetDefaultFont (
		HTMLGraphicsContext** ctx
	);

void GDI_WCE_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* font
	);

int GDI_WCE_NumFontFamilies (
		HTMLGraphicsContext** ctx
	);
	
const WEBC_CHAR** GDI_WCE_GetFontFamilyNames (
		HTMLGraphicsContext** ctx
	);

const WEBC_CHAR** GDI_WCE_GetFontFamilyGenericNames (
		HTMLGraphicsContext** ctx
	);

#ifdef __cplusplus
}
#endif

#endif /* __WFGUI_H__ */
