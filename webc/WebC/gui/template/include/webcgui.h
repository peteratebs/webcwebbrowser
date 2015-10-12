/*
|  WEBCGUI.H - WebC/GUI integration layer template
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

#ifndef __WEBCGUI_H__
#define __WEBCGUI_H__

#ifdef __cplusplus

#include "webc.h"
 
extern "C" {

int    webc_InitGuinameBrowserConfig    (HTMLBrowserConfig* config);
void   webc_DestroyGuinameBrowserConfig (HTMLBrowserConfig* config);
WebKey webc_TranslateGuinameKey         (WEBC_UINT32 rawKey);

}

#endif /* __cplusplus */

#endif /* __WEBCGUI_H__ */
