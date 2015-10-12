/*
|  WEBCC-PEG.H - WebC/PEG integration layer
| 
|  EBS - 
| 
|  Copyright EBS Inc. , 2008
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __WEBCPEG_H__
#define __WEBCPEG_H__

#ifdef __cplusplus

#include "webc.h"
 
extern "C" {
#include "peg.h"
}

typedef struct
{
	HTMLGraphicsContext* ctx;
	PegThing*            parentThing;
	WEBC_UINT8*          palette;
	WEBC_UINT32          paletteSize;
	PegRect clipRect;
}
WebcCPegGraphics;

extern "C" {

int    webc_InitCPegBrowserConfig    (HTMLBrowserConfig* config, PegThing* parentThing);
void   webc_DestroyCPegBrowserConfig (HTMLBrowserConfig* config);
WebKey webc_TranslateCPegKey         (WEBC_UINT32 pegKey);

}

#endif /* __cplusplus */

#endif /* __WEBCPEG_H__ */
