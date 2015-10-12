/*
|  WEBCDEFAULT.H - 
| 
|  EBS - WebC
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __WEBCDEFAULT_H__
#define __WEBCDEFAULT_H__

#include "webctypes.h"
#include "html.h"
#include "css.h"

typedef enum e_WebcDefaultIndex
{
	WEBC_DEFAULT_ACTIVE_BACKGROUND_COLOR = 0,
	WEBC_DEFAULT_ACTIVE_COLOR,
	WEBC_DEFAULT_ACTIVE_LINK_COLOR,
	WEBC_DEFAULT_BACKGROUND_COLOR,
	WEBC_DEFAULT_BODY_FONT_FAMILY,
	WEBC_DEFAULT_BODY_FONT_SIZE,
	WEBC_DEFAULT_BODY_FONT_STYLE,
	WEBC_DEFAULT_BODY_FONT_WEIGHT,
	WEBC_DEFAULT_BODY_PADDING,
	WEBC_DEFAULT_BUTTON_BACKGROUND_COLOR,
	WEBC_DEFAULT_BUTTON_FONT_FAMILY,
	WEBC_DEFAULT_BUTTON_FONT_SIZE,
	WEBC_DEFAULT_BUTTON_FONT_STYLE,
	WEBC_DEFAULT_BUTTON_FONT_WEIGHT,
	WEBC_DEFAULT_BUTTON_FRAME_WIDTH,
	WEBC_DEFAULT_BUTTON_HILITE_COLOR,
	WEBC_DEFAULT_BUTTON_PADDING,
	WEBC_DEFAULT_BUTTON_SHADOW_COLOR,
	WEBC_DEFAULT_DRAW_SCROLL_CORNER,
	WEBC_DEFAULT_HILITE_COLOR,
	WEBC_DEFAULT_LINK_COLOR,
	WEBC_DEFAULT_LIST_INDENT,
	WEBC_DEFAULT_SCROLL_BAR_BUTTON_LENGTH,	
	WEBC_DEFAULT_SCROLL_BAR_BUTTON_OVERLAP,
	WEBC_DEFAULT_SCROLL_BAR_FRAME_WIDTH,
	WEBC_DEFAULT_SCROLL_BAR_HILITE_COLOR,
	WEBC_DEFAULT_SCROLL_BAR_PADDING,
	WEBC_DEFAULT_SCROLL_BAR_SHADOW_COLOR,	
	WEBC_DEFAULT_SCROLL_CORNER_COLOR,
	WEBC_DEFAULT_SELECT_FONT_FAMILY,
	WEBC_DEFAULT_SELECT_FONT_SIZE,
	WEBC_DEFAULT_SELECT_FONT_STYLE,
	WEBC_DEFAULT_SELECT_FONT_WEIGHT,
	WEBC_DEFAULT_SHADOW_COLOR,
	WEBC_DEFAULT_SLIDER_BACKGROUND_COLOR,
	WEBC_DEFAULT_SLIDER_BACKGROUND_FRAME_WIDTH,
	WEBC_DEFAULT_SLIDER_COLOR,
	WEBC_DEFAULT_SLIDER_FRAME_HILITE_COLOR,
	WEBC_DEFAULT_SLIDER_FRAME_SHADOW_COLOR,
	WEBC_DEFAULT_SLIDER_FRAME_WIDTH,
	WEBC_DEFAULT_SLIDER_HILITE_COLOR,
	WEBC_DEFAULT_SLIDER_PADDING,
	WEBC_DEFAULT_SLIDER_SHADOW_COLOR,
	WEBC_DEFAULT_SLIDER_WIDTH,
	WEBC_DEFAULT_TEXT_AREA_FONT_FAMILY,
	WEBC_DEFAULT_TEXT_AREA_FONT_SIZE,
	WEBC_DEFAULT_TEXT_AREA_FONT_STYLE,
	WEBC_DEFAULT_TEXT_AREA_FONT_WEIGHT,
	WEBC_DEFAULT_TEXT_COLOR,
	WEBC_DEFAULT_TEXT_INPUT_FONT_FAMILY,
	WEBC_DEFAULT_TEXT_INPUT_FONT_SIZE,
	WEBC_DEFAULT_TEXT_INPUT_FONT_STYLE,
	WEBC_DEFAULT_TEXT_INPUT_FONT_WEIGHT,
	WEBC_DEFAULT_MONOSPACE_FONT_FAMILY,
	WEBC_DEFAULT_MONOSPACE_FONT_SIZE,  
	WEBC_DEFAULT_MONOSPACE_FONT_STYLE, 
	WEBC_DEFAULT_MONOSPACE_FONT_WEIGHT,
	WEBC_DEFAULT_VISITED_LINK_COLOR,
	WEBC_NUM_DEFAULTS
}
WebcDefaultIndex;

#ifdef __cplusplus
extern "C" {
#endif

void*   webc_GetDefault                (WebcDefaultIndex varIndex);
unsigned long  webc_GetDefaultUint            (WebcDefaultIndex varIndex);
void    webc_RestoreDefaults           (void);
void    _webc_SetDefault               (WebcDefaultIndex varIndex, void* value);

#define webc_SetDefault(I,V)            _webc_SetDefault(I,(void*)(V))
#define webc_GetDefaultBoolean(I)      ((WEBC_BOOL) webc_GetDefault(I))
#define webc_GetDefaultColor(I)        ((HTMLColor) webc_GetDefault(I))
#define webc_GetDefaultDisplayInt(I)   ((DISPLAY_INT) webc_GetDefault(I))
#define webc_GetDefaultFontSize(I)     ((CSSFontSize *) webc_GetDefault(I))
#define webc_GetDefaultFontStyle(I)    ((CSSFontStyle) webc_GetDefaultUint(I))
#define webc_GetDefaultFontWeight(I)   ((CSSFontWeight) webc_GetDefaultUint(I))
#define webc_GetDefaultInt16(I)        ((WEBC_INT16) webc_GetDefaultUint(I))
#define webc_GetDefaultUint16(I)       ((WEBC_UINT16) webc_GetDefaultUint(I))
#define webc_GetDefaultString(I)       ((const WEBC_CHAR*) webc_GetDefault(I))
#define webc_GetDefaultStringASCII(I)  ((const WEBC_CHAR8*) webc_GetDefault(I))
#define webc_GetDefaultStringUC(I)     ((const WEBC_CHAR16*) webc_GetDefault(I))

#ifdef __cplusplus
}
#endif

#endif /* __WEBCDEFAULT_H__ */
