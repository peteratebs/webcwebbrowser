/*
|  WFCPEG.H - 
| 
|  EBS - 
| 
|  Copyright EBS Inc. , 2008
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __WFPEG_H__
#define __WFPEG_H__

#include "webc.h"

#ifdef __cplusplus
extern "C" {
#endif

void* CPEG_CreateFont (
		HTMLGraphicsContext** ctx,
		const WEBC_CHAR* familyName,
		const WEBC_CHAR* genericName,
		WEBC_UINT16 pointSize,
		WEBC_UINT16 attrib,
		int familyIndex
	);

void* CPEG_GetDefaultFont (
		HTMLGraphicsContext** ctx
	);

void CPEG_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* font
	);

int CPEG_NumFontFamilies (
		HTMLGraphicsContext** ctx
	);
	
const WEBC_CHAR** CPEG_GetFontFamilyNames (
		HTMLGraphicsContext** ctx
	);

const WEBC_CHAR** CPEG_GetFontFamilyGenericNames (
		HTMLGraphicsContext** ctx
	);

#ifdef __cplusplus
}
#endif

#endif /* __WFPEG_H__ */
