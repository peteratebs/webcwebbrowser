/*
|  WEBCGDIP.H - WebC/GDI+ integration layer
| 
|  EBS - 
| 
|   $Author: vmalaiya $
|   $Date: 2006/08/31 22:49:21 $
|   $Name:  $
|   $Revision: 1.1 $
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __WEBCGDIP_H__
#define __WEBCGDIP_H__

#ifdef __cplusplus

#include "webc.h"
#include "windows.h"

extern "C" {

int    webc_InitGDIPlusBrowserConfig    (HTMLBrowserConfig* config, HWND windowHandle);
void   webc_DestroyGDIPlusBrowserConfig (HTMLBrowserConfig* config);
void   webc_DestroyGDIPlusBrowser       (HBROWSER_HANDLE hbrowser);
WebKey webc_TranslateGDIPlusKey         (WEBC_UINT32 rawKey);

}

#endif /* __cplusplus */

#endif /* __WEBCGDIP_H__ */
