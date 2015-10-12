/*
|  WEBCPEG.H - WebC/PEG integration layer
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

#ifndef __WEBCPEG_H__
#define __WEBCPEG_H__

#ifdef __cplusplus

#include "webc.h"
 
class PegThing;

typedef struct
{
	HTMLGraphicsContext* ctx;
	PegThing*            parentThing;
	WEBC_UINT8*          palette;
	WEBC_UINT32          paletteSize;
}
WebcPegGraphics;

extern "C" {

int    webc_InitPegBrowserConfig    (HTMLBrowserConfig* config, PegThing* parentThing);
void   webc_DestroyPegBrowserConfig (HTMLBrowserConfig* config);
WebKey webc_TranslatePegKey         (WEBC_UINT32 pegKey);

}

#endif /* __cplusplus */

#endif /* __WEBCPEG_H__ */
