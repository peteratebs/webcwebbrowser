/*
|  WEBCGRAFX.H - WebC/GUI integration layer template
| 
|  EBS - 
| 
|   $Author: vmalaiya $
|   $Date: 2006/08/31 22:34:35 $
|   $Name:  $
|   $Revision: 1.1 $
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __WEBCGRAFX_H__
#define __WEBCGRAFX_H__

#ifdef __cplusplus

#include "webc.h"
#include "winlib.h"
 
extern "C" {

int    webc_InitNucleusBrowserConfig    (HTMLBrowserConfig* config, WINDOW nucleusWindow);
void   webc_DestroyNucleusBrowserConfig (HTMLBrowserConfig* config);
WebKey webc_TranslateNucleusKey         (WEBC_UINT32 rawKey);

}

#endif /* __cplusplus */

#endif /* __WEBCGRAFX_H__ */
