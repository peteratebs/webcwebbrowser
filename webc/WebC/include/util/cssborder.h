/*
|  CSSBORDER.H - 
| 
|  EBS - WebC
| 
|  Copyright EBS Inc. , 2005
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __CSSBORDER_H__
#define __CSSBORDER_H__

enum e_CSSBorderStyle
{
	CSS_BORDER_STYLE_DASHED = 0,
	CSS_BORDER_STYLE_DOTTED,
	CSS_BORDER_STYLE_DOUBLE,
	CSS_BORDER_STYLE_GROOVE,
	CSS_BORDER_STYLE_INSET,
	CSS_BORDER_STYLE_NONE,
	CSS_BORDER_STYLE_OUTSET,
	CSS_BORDER_STYLE_RIDGE,
	CSS_BORDER_STYLE_SOLID

};
typedef enum e_CSSBorderStyle CSSBorderStyle;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CSSBORDER_H__ */
