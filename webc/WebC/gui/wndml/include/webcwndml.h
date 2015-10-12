/*
|  WEBCGUI.H - WebC/GUI integration layer template
|
|  EBSnet -
|
|   $Author: manish $
|   $Date: 2006/10/17 06:09:21 $
|   $Name:  $
|   $Revision: 1.1 $
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#ifndef __WEBCGUI_H__
#define __WEBCGUI_H__

#ifdef __cplusplus

#include "webc.h"

#include <ugl/ugl.h>
#include <ugl/uglinput.h>
#include <ugl/uglfont.h>

extern "C" {

typedef struct
{
	UGL_DDB_ID m_DDBitmap;
	UGL_TDDB_ID mtrans_DDBitmap;
	UGL_INT32 m_iWidth;
	UGL_INT32 m_iHeight;
	int	 reference_count;
	void *punstretched;
} WebcWindMLBitmap;

void* WindML_CreateFont (
		HTMLGraphicsContext** ctx,
		const WEBC_CHAR* familyName,
		const WEBC_CHAR* genericName,
		WEBC_UINT16 pointSize,
		WEBC_UINT16 attrib,
		int familyIndex
	);

void* WindML_GetDefaultFont (
		HTMLGraphicsContext** ctx
	);

void WindML_DestroyFont (
		HTMLGraphicsContext** ctx,
		void* font
	);

int WindML_NumFontFamilies (
		HTMLGraphicsContext** ctx
	);

const WEBC_CHAR** WindML_GetFontFamilyNames (
		HTMLGraphicsContext** ctx
	);

const WEBC_CHAR** WindML_GetFontFamilyGenericNames (
		HTMLGraphicsContext** ctx
	);

int    webc_InitWindMLBrowserConfig    (HTMLBrowserConfig* config);
void   webc_DestroyWindMLBrowserConfig (HTMLBrowserConfig* config);
void   webc_DestroyWindMLBrowser       (HBROWSER_HANDLE hbrowser);
WebKey webc_TranslateWindMLKey         (WEBC_UINT32 rawKey);

UGL_MSG webc_GetWindMLMessages(HTMLBrowserConfig* config);

}

#endif /* __cplusplus */

#endif /* __WEBCGUI_H__ */
