/*
|  JSTYLE.CPP -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|  Module description:
|   [tbd]
*/

#include "webcjhconfig.h"
#include "jstyle.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)
#include "jhcoll.h"
#include "helement.hpp"
#include "htmldoc.hpp"
#include "wjscript.hpp"
#include "webcstr.h"
#include "webcmem.h"
#include "wjsdebug.h"
#include "webcLog.h"

#ifdef WEBC_DEBUG
extern const char * gpCSSPropertyNames[];
#endif

enum jstyle_tinyid
{
	JSTYLE_ALINK=0                   ,
	JSTYLE_VLINK                     ,
	JSTYLE_BACKGROUNDATTACHMENT      ,
	JSTYLE_BACKGROUNDCOLOR           ,
	JSTYLE_BACKGROUNDIMAGE           ,
	JSTYLE_BACKGROUNDPOSITION        ,
	JSTYLE_BACKGROUNDPOSITIONX       ,
	JSTYLE_BACKGROUNDPOSITIONY       ,
	JSTYLE_BACKGROUNDREPEAT          ,
	JSTYLE_BORDER                    ,
	JSTYLE_BORDERBOTTOM              ,
	JSTYLE_BORDERBOTTOMCOLOR         ,
	JSTYLE_BORDERBOTTOMSTYLE         ,
	JSTYLE_BORDERBOTTOMWIDTH         ,
	JSTYLE_BORDERCOLOR               ,
	JSTYLE_BORDERLEFT                ,
	JSTYLE_BORDERLEFTCOLOR           ,
	JSTYLE_BORDERLEFTSTYLE           ,
	JSTYLE_BORDERLEFTWIDTH           ,
	JSTYLE_BORDERRIGHT               ,
	JSTYLE_BORDERRIGHTCOLOR          ,
	JSTYLE_BORDERRIGHTSTYLE          ,
	JSTYLE_BORDERRIGHTWIDTH          ,
	JSTYLE_BORDERSTYLE               ,
	JSTYLE_BORDERTOP                 ,
	JSTYLE_BORDERTOPCOLOR            ,
	JSTYLE_BORDERTOPSTYLE            ,
	JSTYLE_BORDERTOPWIDTH            ,
	JSTYLE_BORDERWIDTH               ,
	JSTYLE_BOTTOM                    ,
	JSTYLE_CLEAR                     ,
	JSTYLE_COLOR                     ,
	JSTYLE_DISPLAY                   ,
	JSTYLE_FLOAT                     ,
	JSTYLE_FONT                      ,
	JSTYLE_FONTFAMILY                ,
	JSTYLE_FONTSIZE                  ,
	JSTYLE_FONTSTYLE                 ,
	JSTYLE_FONTVARIANT               ,
	JSTYLE_FONTWEIGHT                ,
	JSTYLE_HEIGHT                    ,
	JSTYLE_LEFT                      ,
	JSTYLE_LETTERSPACING             ,
	JSTYLE_LINEHEIGHT                ,
	JSTYLE_LISTSTYLE                 ,
	JSTYLE_LISTSTYLEIMAGE            ,
	JSTYLE_LISTSTYLEPOSITION         ,
	JSTYLE_LISTSTYLETYPE             ,
	JSTYLE_MARGIN                    ,
	JSTYLE_MARGINBOTTOM              ,
	JSTYLE_MARGINLEFT                ,
	JSTYLE_MARGINRIGHT               ,
	JSTYLE_MARGINTOP                 ,
	JSTYLE_OPACITY                   ,
	JSTYLE_OVERFLOW                  ,
	JSTYLE_OVERFLOWX                 ,
	JSTYLE_OVERFLOWY                 ,
	JSTYLE_PADDING                   ,
	JSTYLE_PADDINGBOTTOM             ,
	JSTYLE_PADDINGLEFT               ,
	JSTYLE_PADDINGRIGHT              ,
	JSTYLE_PADDINGTOP                ,
	JSTYLE_POSITION                  ,
	JSTYLE_RIGHT                     ,
	JSTYLE_TEXTALIGN                 ,
	JSTYLE_TEXTDECORATION            ,
	JSTYLE_TEXTINDENT                ,
	JSTYLE_TEXTTRANSFORM             ,
	JSTYLE_TOP                       ,
	JSTYLE_VERTICALALIGN             ,
	JSTYLE_VISIBILITY                ,
	JSTYLE_WHITESPACE                ,
	JSTYLE_WIDTH                     ,
	JSTYLE_WORDSPACING               ,
	JSTYLE_ZINDEX                    ,
	JSTYLE_ONOFFBEHAVIOR             ,
	JSTYLE_BEHAVIOR                  ,
	JSTYLE_BORDERCOLLAPSE            ,
	JSTYLE_SETS                      ,
	JSTYLE_CURSOR                    ,
	JSTYLE_IMEMODE                   ,
	JSTYLE_LAYOUTFLOW                ,
	JSTYLE_LAYOUTGRID                ,
	JSTYLE_LAYOUTGRIDCHAR            ,
	JSTYLE_LAYOUTGRIDLINE            ,
	JSTYLE_LAYOUTGRIDMODE            ,
	JSTYLE_LAYOUTGRIDTYPE            ,
	JSTYLE_TEXTDECORATIONNONE        ,
	JSTYLE_TEXTDECORATIONOVERLINE    ,
	JSTYLE_TEXTDECORATIONUNDERLINE   ,
	JSTYLE_RUBYALIGN                 ,
	JSTYLE_RUBYOVERHANG              ,
	JSTYLE_RUBYPOSITION              ,
	JSTYLE_SCROLLBARBASECOLOR        ,
	JSTYLE_SCROLLBARHIGHLIGHTCOLOR   ,
	JSTYLE_STYLEFLOAT                ,
	JSTYLE_TABLELAYOUT               ,
	JSTYLE_CLIP                      ,
	JSTYLE_DIRECTION                 ,
	JSTYLE_LINEBREAK                 ,
	JSTYLE_MINHEIGHT                 ,
	JSTYLE_PAGEBREAKAFTER            ,
	JSTYLE_PAGEBREAKBEFORE           ,
	JSTYLE_PIXELBOTTOM               ,
	JSTYLE_PIXELHEIGHT               ,
	JSTYLE_PIXELLEFT                 ,
	JSTYLE_PIXELRIGHT                ,
	JSTYLE_PIXELTOP                  ,
	JSTYLE_PIXELWIDTH                ,
	JSTYLE_POSBOTTOM                 ,
	JSTYLE_POSHEIGHT                 ,
	JSTYLE_POSLEFT                   ,
	JSTYLE_POSRIGHT                  ,
	JSTYLE_POSTOP                    ,
	JSTYLE_POSWIDTH                  ,
	JSTYLE_TEXTALIGNLAST             ,
	JSTYLE_TEXTAUTOSPACE             ,
	JSTYLE_TEXTDECORATIONBLINK       ,
	JSTYLE_TEXTDECORATIONLINETHROUGH ,
	JSTYLE_TEXTJUSTIFY               ,
	JSTYLE_TEXTKASHIDASPACE          ,
	JSTYLE_TEXTOVERFLOW              ,
	JSTYLE_TEXTUNDERLINEPOSITION     ,
	JSTYLE_WORDBREAK                 ,
	JSTYLE_WORDWRAP                  ,
	JSTYLE_WRITINGMODE               ,
	JSTYLE_ZOOM                      ,
	JSTYLE_NUM_PROPERTIES
};

const CSSPropertyType gpTinyIdToPropType[JSTYLE_NUM_PROPERTIES] =
{
	CSS_PROPERTY_ALINK_COLOR,
	CSS_PROPERTY_VLINK_COLOR,
	CSS_PROPERTY_BACKGROUND_ATTACHMENT,
	CSS_PROPERTY_BACKGROUND_COLOR,
	CSS_PROPERTY_BACKGROUND_IMAGE,
	CSS_PROPERTY_BACKGROUND_POSITION,
	CSS_PROPERTY_BACKGROUND_X_POSITION,
	CSS_PROPERTY_BACKGROUND_Y_POSITION,
	CSS_PROPERTY_BACKGROUND_REPEAT,
	CSS_PROPERTY_BORDER,
	CSS_PROPERTY_BORDER_BOTTOM,
	CSS_PROPERTY_BORDER_BOTTOM_COLOR,
	CSS_PROPERTY_BORDER_BOTTOM_STYLE,
	CSS_PROPERTY_BORDER_BOTTOM_WIDTH,
	CSS_PROPERTY_BORDER_COLOR,
	CSS_PROPERTY_BORDER_LEFT,
	CSS_PROPERTY_BORDER_LEFT_COLOR,
	CSS_PROPERTY_BORDER_LEFT_STYLE,
	CSS_PROPERTY_BORDER_LEFT_WIDTH,
	CSS_PROPERTY_BORDER_RIGHT,
	CSS_PROPERTY_BORDER_RIGHT_COLOR,
	CSS_PROPERTY_BORDER_RIGHT_STYLE,
	CSS_PROPERTY_BORDER_RIGHT_WIDTH,
	CSS_PROPERTY_BORDER_STYLE,
	CSS_PROPERTY_BORDER_TOP,
	CSS_PROPERTY_BORDER_TOP_COLOR,
	CSS_PROPERTY_BORDER_TOP_STYLE,
	CSS_PROPERTY_BORDER_TOP_WIDTH,
	CSS_PROPERTY_BORDER_WIDTH,
	CSS_PROPERTY_BOTTOM,
	CSS_PROPERTY_CLEAR,
	CSS_PROPERTY_COLOR,
	CSS_PROPERTY_DISPLAY,
	CSS_PROPERTY_FLOAT,
	CSS_PROPERTY_FONT,
	CSS_PROPERTY_FONT_FAMILY,
	CSS_PROPERTY_FONT_SIZE,
	CSS_PROPERTY_FONT_STYLE,
	CSS_PROPERTY_FONT_VARIANT,
	CSS_PROPERTY_FONT_WEIGHT,
	CSS_PROPERTY_HEIGHT,
	CSS_PROPERTY_LEFT,
	CSS_PROPERTY_LETTER_SPACING,
	CSS_PROPERTY_LINE_HEIGHT,
	CSS_PROPERTY_LIST_STYLE,
	CSS_PROPERTY_LIST_STYLE_IMAGE,
	CSS_PROPERTY_LIST_STYLE_POSITION,
	CSS_PROPERTY_LIST_STYLE_TYPE,
	CSS_PROPERTY_MARGIN,
	CSS_PROPERTY_MARGIN_BOTTOM,
	CSS_PROPERTY_MARGIN_LEFT,
	CSS_PROPERTY_MARGIN_RIGHT,
	CSS_PROPERTY_MARGIN_TOP,
	CSS_PROPERTY_OPACITY,
	CSS_PROPERTY_OVERFLOW,
	CSS_PROPERTY_OVERFLOW_X,
	CSS_PROPERTY_OVERFLOW_Y,
	CSS_PROPERTY_PADDING,
	CSS_PROPERTY_PADDING_BOTTOM,
	CSS_PROPERTY_PADDING_LEFT,
	CSS_PROPERTY_PADDING_RIGHT,
	CSS_PROPERTY_PADDING_TOP,
	CSS_PROPERTY_POSITION,
	CSS_PROPERTY_RIGHT,
	CSS_PROPERTY_TEXT_ALIGN,
	CSS_PROPERTY_TEXT_DECORATION,
	CSS_PROPERTY_TEXT_INDENT,
	CSS_PROPERTY_TEXT_TRANSFORM,
	CSS_PROPERTY_TOP,
	CSS_PROPERTY_VERTICAL_ALIGN,
	CSS_PROPERTY_VISIBILITY,
	CSS_PROPERTY_WHITE_SPACE,
	CSS_PROPERTY_WIDTH,
	CSS_PROPERTY_WORD_SPACING,
	CSS_PROPERTY_ZINDEX,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
	CSS_PROPERTY_UNKNOWN,
};




const NotifyEventType gTinyIdToEventType[JSTYLE_NUM_PROPERTIES] =
{
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_ALINK_COLOR,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_VLINK_COLOR,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_BACKGROUND_ATTACHMENT,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_BACKGROUND_COLOR,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_BACKGROUND_IMAGE,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_BACKGROUND_POSITION,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_BACKGROUND_X_POSITION,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_BACKGROUND_Y_POSITION,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BACKGROUND_REPEAT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_BOTTOM,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_BORDER_BOTTOM_COLOR,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_BOTTOM_STYLE,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_BOTTOM_WIDTH,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_BORDER_COLOR,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_LEFT,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_BORDER_LEFT_COLOR,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_LEFT_STYLE,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_LEFT_WIDTH,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_RIGHT,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_BORDER_RIGHT_COLOR,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_RIGHT_STYLE,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_RIGHT_WIDTH,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_STYLE,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_TOP,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_BORDER_TOP_COLOR,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_TOP_STYLE,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_TOP_WIDTH,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BORDER_WIDTH,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_BOTTOM,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_CLEAR,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_COLOR,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_DISPLAY,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_FLOAT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_FONT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_FONT_FAMILY,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_FONT_SIZE,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_FONT_STYLE,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_FONT_VARIANT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_FONT_WEIGHT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_HEIGHT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_LEFT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_LETTER_SPACING,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_LINE_HEIGHT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_LIST_STYLE,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_LIST_STYLE_IMAGE,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_LIST_STYLE_POSITION,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_LIST_STYLE_TYPE,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_MARGIN,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_MARGIN_BOTTOM,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_MARGIN_LEFT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_MARGIN_RIGHT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_MARGIN_TOP,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_OPACITY,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_OVERFLOW,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_OVERFLOW_X,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_OVERFLOW_Y,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_PADDING,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_PADDING_BOTTOM,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_PADDING_LEFT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_PADDING_RIGHT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_PADDING_TOP,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_POSITION,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_RIGHT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_TEXT_ALIGN,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_TEXT_DECORATION,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_TEXT_INDENT,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_TEXT_TRANSFORM,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_TOP,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_VERTICAL_ALIGN,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_VISIBILITY,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_WHITE_SPACE,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_WIDTH,
	NOTIFY_STYLE_CHANGE,          // CSS_PROPERTY_WORD_SPACING,
	NOTIFY_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_ZINDEX,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,         // CSS_PROPERTY_UNKNOWN,
};

const NotifyEventType gTinyIdToChildEventType[JSTYLE_NUM_PROPERTIES] =
{
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_ALINK_COLOR,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_VLINK_COLOR,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BACKGROUND_ATTACHMENT,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BACKGROUND_COLOR,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BACKGROUND_IMAGE,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BACKGROUND_POSITION,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BACKGROUND_X_POSITION,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BACKGROUND_Y_POSITION,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BACKGROUND_REPEAT,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_BOTTOM,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_BOTTOM_COLOR,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_BOTTOM_STYLE,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_BOTTOM_WIDTH,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_COLOR,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_LEFT,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_LEFT_COLOR,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_LEFT_STYLE,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_LEFT_WIDTH,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_RIGHT,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_RIGHT_COLOR,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_RIGHT_STYLE,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_RIGHT_WIDTH,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_STYLE,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_TOP,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_TOP_COLOR,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_TOP_STYLE,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_TOP_WIDTH,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BORDER_WIDTH,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_BOTTOM,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_CLEAR,
	NOTIFY_PARENT_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_COLOR,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_DISPLAY,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_FLOAT,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_FONT,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_FONT_FAMILY,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_FONT_SIZE,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_FONT_STYLE,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_FONT_VARIANT,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_FONT_WEIGHT,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_HEIGHT,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_LEFT,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_LETTER_SPACING,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_LINE_HEIGHT,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_LIST_STYLE,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_LIST_STYLE_IMAGE,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_LIST_STYLE_POSITION,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_LIST_STYLE_TYPE,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_MARGIN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_MARGIN_BOTTOM,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_MARGIN_LEFT,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_MARGIN_RIGHT,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_MARGIN_TOP,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_OPACITY,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_OVERFLOW,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_OVERFLOW_X,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_OVERFLOW_Y,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_PADDING,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_PADDING_BOTTOM,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_PADDING_LEFT,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_PADDING_RIGHT,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_PADDING_TOP,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_POSITION,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_RIGHT,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_TEXT_ALIGN,
	NOTIFY_PARENT_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_TEXT_DECORATION,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_TEXT_INDENT,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_TEXT_TRANSFORM,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_TOP,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_VERTICAL_ALIGN,
	NOTIFY_PARENT_DISPLAY_STYLE_CHANGE,  // CSS_PROPERTY_VISIBILITY,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_WHITE_SPACE,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_WIDTH,
	NOTIFY_PARENT_STYLE_CHANGE,          // CSS_PROPERTY_WORD_SPACING,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_ZINDEX,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
	NOTIFY_UNKNOWN_EVENT,                // CSS_PROPERTY_UNKNOWN,
};
/**
 * Call this method when getting a Property from style
 */
static JSBool jstyle_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
#if (WEBC_SUPPORT_STYLE_SHEETS)
	HTMLElement *pElem = (HTMLElement*) jhutil_GetPrivate(cx, obj);

	if (!pElem)
	{
		return JS_TRUE;
	}

	if (JSVAL_IS_INT(id))
	{
		int index = JSVAL_TO_INT(id);

		if (index >= 0 && index < JSTYLE_NUM_PROPERTIES)
		{
			CSSPropertyValue cssValue;
			CSSPropertyType cssProp = gpTinyIdToPropType[index];

			if (cssProp != CSS_PROPERTY_UNKNOWN)
			{
#ifdef WEBC_DEBUG
				LOG_HTMLDOM("jstyle_getProperty CSSPropertyType= %s\n", gpCSSPropertyNames[(int)cssProp]);
#endif
				CSSValueType cssResult = pElem->GetStyleFromCSS(cssProp, &cssValue);

				if (cssResult == CSS_VALUE_INHERIT)
				{
					JSString *pNewString = JS_NewStringCopyZ(cx, "inherit");
					if (pNewString)
					{
						*vp = STRING_TO_JSVAL(pNewString);
						return JS_TRUE;
					}
				}

				if (index == JSTYLE_ZINDEX)
				{
					// these properties are integer values
					if (cssResult != CSS_VALUE_SPECIFIED)
					{
						cssValue.number = 0;
					}

					*vp = INT_TO_JSVAL((int) cssValue.number);
				}
				else
				{
					if (cssResult == CSS_VALUE_SPECIFIED)
					{
						WEBC_CHAR cssStr[64];

						if (CSS_PropertyToString(cssStr, cssProp, &cssValue, 64) >= 0)
						{
							LOG_HTMLDOM("    %S\n", cssStr);

							JSString *pNewString = WEBC_JS_NEW_STRING_COPY_Z(cx, cssStr);
							if (pNewString)
							{
								*vp = STRING_TO_JSVAL(pNewString);
								return JS_TRUE;
							}
						}
					}
				}
			}
		}
	}

#endif // (WEBC_SUPPORT_STYLE_SHEETS)

	return JS_TRUE;
}

static JSBool jstyle_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  #if (WEBC_SUPPORT_STYLE_SHEETS)
	HTMLElement *pElem = (HTMLElement *)jhutil_GetPrivate(cx, obj);
	HTMLDocument *pDoc = (pElem)? pElem->GetDocument() : 0;
	CSSDocumentContext *pCSSCx = (pDoc)? pDoc->GetCSSContext() : 0;

	if (!pCSSCx)
	{
		return JS_TRUE;
	}

	if (JSVAL_IS_INT(id))
	{
		int index = JSVAL_TO_INT(id);

		if (index >= 0 && index < JSTYLE_NUM_PROPERTIES)
		{
			CSSPropertyType cssProp = gpTinyIdToPropType[index];

			if (cssProp != CSS_PROPERTY_UNKNOWN)
			{
#ifdef WEBC_DEBUG
				LOG_HTMLDOM("jstyle_setProperty CSSPropertyType= %s\n", gpCSSPropertyNames[(int)cssProp]);
#endif
				HTMLDocument* doc = pElem->GetDocument();
				WEBC_CHAR* value = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));

				LOG_HTMLDOM("    %S\n", value);

				pCSSCx->ParseElementProperty(pElem, cssProp, value, (doc)? doc->CurrentURL() : 0);

				if (gTinyIdToChildEventType[index] != NOTIFY_UNKNOWN_EVENT)
				{
					pElem->StyleUpdateChildren(gTinyIdToEventType[index], gTinyIdToChildEventType[index]);
				}
				else
				{
					pElem->StyleUpdate(gTinyIdToEventType[index]);
				}
			}
		}
	}
  #endif// (WEBC_SUPPORT_STYLE_SHEETS)

	return JS_TRUE;
}

JSBool jstyle_resolve(JSContext *cx, JSObject *obj, jsval id)
{
#ifdef WEBC_DEBUG
	if (JSVAL_IS_STRING(id))
	{
		WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(id));
		LOG_HTMLDOM("jstyle_resolve %S\n", name);
	}
#endif
    return(jutil_resolve(cx, obj, id, "Style"));
}

/**
 * This JSClass is the javascript wrapper for the style class defined by MS
 */
JSClass JSStyle =
{
	"Style", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jstyle_getProperty,
	(JSPropertyOp) jstyle_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jstyle_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};

JSPropertySpec jstyle_props[] = {
	{"aLink"                       , JSTYLE_ALINK                     , JSPROP_ENUMERATE},
	{"vLink"                       , JSTYLE_VLINK                     , JSPROP_ENUMERATE},
	{"backgroundAttachment"        , JSTYLE_BACKGROUNDATTACHMENT      , JSPROP_ENUMERATE},
	{"backgroundColor"             , JSTYLE_BACKGROUNDCOLOR           , JSPROP_ENUMERATE},
	{"backgroundImage"             , JSTYLE_BACKGROUNDIMAGE           , JSPROP_ENUMERATE},
	{"backgroundPosition"          , JSTYLE_BACKGROUNDPOSITION        , JSPROP_ENUMERATE},
	{"backgroundPositionx"         , JSTYLE_BACKGROUNDPOSITIONX       , JSPROP_ENUMERATE},
	{"backgroundPositiony"         , JSTYLE_BACKGROUNDPOSITIONY       , JSPROP_ENUMERATE},
	{"backgroundRepeat"            , JSTYLE_BACKGROUNDREPEAT          , JSPROP_ENUMERATE},
	{"border"                      , JSTYLE_BORDER                    , JSPROP_ENUMERATE},
	{"borderBottom"                , JSTYLE_BORDERBOTTOM              , JSPROP_ENUMERATE},
	{"borderBottomColor"           , JSTYLE_BORDERBOTTOMCOLOR         , JSPROP_ENUMERATE},
	{"borderBottomStyle"           , JSTYLE_BORDERBOTTOMSTYLE         , JSPROP_ENUMERATE},
	{"borderBottomWidth"           , JSTYLE_BORDERBOTTOMWIDTH         , JSPROP_ENUMERATE},
	{"borderColor"                 , JSTYLE_BORDERCOLOR               , JSPROP_ENUMERATE},
	{"borderLeft"                  , JSTYLE_BORDERLEFT                , JSPROP_ENUMERATE},
	{"borderLeftColor"             , JSTYLE_BORDERLEFTCOLOR           , JSPROP_ENUMERATE},
	{"borderLeftStyle"             , JSTYLE_BORDERLEFTSTYLE           , JSPROP_ENUMERATE},
	{"borderLeftWidth"             , JSTYLE_BORDERLEFTWIDTH           , JSPROP_ENUMERATE},
	{"borderRight"                 , JSTYLE_BORDERRIGHT               , JSPROP_ENUMERATE},
	{"borderRightColor"            , JSTYLE_BORDERRIGHTCOLOR          , JSPROP_ENUMERATE},
	{"borderRightStyle"            , JSTYLE_BORDERRIGHTSTYLE          , JSPROP_ENUMERATE},
	{"borderRightWidth"            , JSTYLE_BORDERRIGHTWIDTH          , JSPROP_ENUMERATE},
	{"borderStyle"                 , JSTYLE_BORDERSTYLE               , JSPROP_ENUMERATE},
	{"borderTop"                   , JSTYLE_BORDERTOP                 , JSPROP_ENUMERATE},
	{"borderTopColor"              , JSTYLE_BORDERTOPCOLOR            , JSPROP_ENUMERATE},
	{"borderTopStyle"              , JSTYLE_BORDERTOPSTYLE            , JSPROP_ENUMERATE},
	{"borderTopWidth"              , JSTYLE_BORDERTOPWIDTH            , JSPROP_ENUMERATE},
	{"borderWidth"                 , JSTYLE_BORDERWIDTH               , JSPROP_ENUMERATE},
	{"bottom"                      , JSTYLE_BOTTOM                    , JSPROP_ENUMERATE},
	{"clear"                       , JSTYLE_CLEAR                     , JSPROP_ENUMERATE},
	{"color"                       , JSTYLE_COLOR                     , JSPROP_ENUMERATE},
	{"display"                     , JSTYLE_DISPLAY                   , JSPROP_ENUMERATE},
	{"float"                       , JSTYLE_FLOAT                     , JSPROP_ENUMERATE},
	{"font"                        , JSTYLE_FONT                      , JSPROP_ENUMERATE},
	{"fontFamily"                  , JSTYLE_FONTFAMILY                , JSPROP_ENUMERATE},
	{"fontSize"                    , JSTYLE_FONTSIZE                  , JSPROP_ENUMERATE},
	{"fontStyle"                   , JSTYLE_FONTSTYLE                 , JSPROP_ENUMERATE},
	{"fontVariant"                 , JSTYLE_FONTVARIANT               , JSPROP_ENUMERATE},
	{"fontWeight"                  , JSTYLE_FONTWEIGHT                , JSPROP_ENUMERATE},
	{"height"                      , JSTYLE_HEIGHT                    , JSPROP_ENUMERATE},
	{"left"                        , JSTYLE_LEFT                      , JSPROP_ENUMERATE},
	{"letterSpacing"               , JSTYLE_LETTERSPACING             , JSPROP_ENUMERATE},
	{"lineHeight"                  , JSTYLE_LINEHEIGHT                , JSPROP_ENUMERATE},
	{"listStyle"                   , JSTYLE_LISTSTYLE                 , JSPROP_ENUMERATE},
	{"listStyleImage"              , JSTYLE_LISTSTYLEIMAGE            , JSPROP_ENUMERATE},
	{"listStylePosition"           , JSTYLE_LISTSTYLEPOSITION         , JSPROP_ENUMERATE},
	{"listStyleType"               , JSTYLE_LISTSTYLETYPE             , JSPROP_ENUMERATE},
	{"margin"                      , JSTYLE_MARGIN                    , JSPROP_ENUMERATE},
	{"marginBottom"                , JSTYLE_MARGINBOTTOM              , JSPROP_ENUMERATE},
	{"marginLeft"                  , JSTYLE_MARGINLEFT                , JSPROP_ENUMERATE},
	{"marginRight"                 , JSTYLE_MARGINRIGHT               , JSPROP_ENUMERATE},
	{"marginTop"                   , JSTYLE_MARGINTOP                 , JSPROP_ENUMERATE},
	{"opacity"                     , JSTYLE_OPACITY                   , JSPROP_ENUMERATE},
	{"overflow"                    , JSTYLE_OVERFLOW                  , JSPROP_ENUMERATE},
	{"overflowX"                   , JSTYLE_OVERFLOWX                 , JSPROP_ENUMERATE},
	{"overflowY"                   , JSTYLE_OVERFLOWY                 , JSPROP_ENUMERATE},
	{"padding"                     , JSTYLE_PADDING                   , JSPROP_ENUMERATE},
	{"paddingBottom"               , JSTYLE_PADDINGBOTTOM             , JSPROP_ENUMERATE},
	{"paddingLeft"                 , JSTYLE_PADDINGLEFT               , JSPROP_ENUMERATE},
	{"paddingRight"                , JSTYLE_PADDINGRIGHT              , JSPROP_ENUMERATE},
	{"paddingTop"                  , JSTYLE_PADDINGTOP                , JSPROP_ENUMERATE},
	{"position"                    , JSTYLE_POSITION                  , JSPROP_ENUMERATE},
	{"right"                       , JSTYLE_RIGHT                     , JSPROP_ENUMERATE},
	{"textAlign"                   , JSTYLE_TEXTALIGN                 , JSPROP_ENUMERATE},
	{"textDecoration"              , JSTYLE_TEXTDECORATION            , JSPROP_ENUMERATE},
	{"textIndent"                  , JSTYLE_TEXTINDENT                , JSPROP_ENUMERATE},
	{"textTransform"               , JSTYLE_TEXTTRANSFORM             , JSPROP_ENUMERATE},
	{"top"                         , JSTYLE_TOP                       , JSPROP_ENUMERATE},
	{"verticalAlign"               , JSTYLE_VERTICALALIGN             , JSPROP_ENUMERATE},
	{"visibility"                  , JSTYLE_VISIBILITY                , JSPROP_ENUMERATE},
	{"whiteSpace"                  , JSTYLE_WHITESPACE                , JSPROP_ENUMERATE},
	{"width"                       , JSTYLE_WIDTH                     , JSPROP_ENUMERATE},
	{"wordSpacing"                 , JSTYLE_WORDSPACING               , JSPROP_ENUMERATE},
	{"zIndex"                      , JSTYLE_ZINDEX                    , JSPROP_ENUMERATE},
	// unsupported
	{"onOffBehavior"               , JSTYLE_ONOFFBEHAVIOR             , JSPROP_ENUMERATE},
	{"behavior"                    , JSTYLE_BEHAVIOR                  , JSPROP_ENUMERATE},
	{"borderCollapse"              , JSTYLE_BORDERCOLLAPSE            , JSPROP_ENUMERATE},
 	{"sets"                        , JSTYLE_SETS                      , JSPROP_ENUMERATE},
	{"cursor"                      , JSTYLE_CURSOR                    , JSPROP_ENUMERATE},
	{"imeMode"                     , JSTYLE_IMEMODE                   , JSPROP_ENUMERATE},
	{"layoutFlow"                  , JSTYLE_LAYOUTFLOW                , JSPROP_ENUMERATE},
	{"layoutGrid"                  , JSTYLE_LAYOUTGRID                , JSPROP_ENUMERATE},
	{"layoutGridChar"              , JSTYLE_LAYOUTGRIDCHAR            , JSPROP_ENUMERATE},
	{"layoutGridLine"              , JSTYLE_LAYOUTGRIDLINE            , JSPROP_ENUMERATE},
	{"layoutGridMode"              , JSTYLE_LAYOUTGRIDMODE            , JSPROP_ENUMERATE},
	{"layoutGridType"              , JSTYLE_LAYOUTGRIDTYPE            , JSPROP_ENUMERATE},
 	{"textDecorationnone"          , JSTYLE_TEXTDECORATIONNONE        , JSPROP_ENUMERATE},
 	{"textDecorationoverLine"      , JSTYLE_TEXTDECORATIONOVERLINE    , JSPROP_ENUMERATE},
 	{"textDecorationunderLine"     , JSTYLE_TEXTDECORATIONUNDERLINE   , JSPROP_ENUMERATE},
	{"rubyAlign"                   , JSTYLE_RUBYALIGN                 , JSPROP_ENUMERATE},
	{"rubyOverhang"                , JSTYLE_RUBYOVERHANG              , JSPROP_ENUMERATE},
	{"rubyPosition"                , JSTYLE_RUBYPOSITION              , JSPROP_ENUMERATE},
	{"scrollbarBaseColor"          , JSTYLE_SCROLLBARBASECOLOR        , JSPROP_ENUMERATE},
	{"scrollbarHighlightColor"     , JSTYLE_SCROLLBARHIGHLIGHTCOLOR   , JSPROP_ENUMERATE},
	{"styleFloat"                  , JSTYLE_STYLEFLOAT                , JSPROP_ENUMERATE},
	{"tableLayout"                 , JSTYLE_TABLELAYOUT               , JSPROP_ENUMERATE},
	{"clip"                        , JSTYLE_CLIP                      , JSPROP_ENUMERATE},
	{"direction"                   , JSTYLE_DIRECTION                 , JSPROP_ENUMERATE},
	{"lineBreak"                   , JSTYLE_LINEBREAK                 , JSPROP_ENUMERATE},
	{"minHeight"                   , JSTYLE_MINHEIGHT                 , JSPROP_ENUMERATE},
	{"pageBreakAfter"              , JSTYLE_PAGEBREAKAFTER            , JSPROP_ENUMERATE},
	{"pageBreakBefore"             , JSTYLE_PAGEBREAKBEFORE           , JSPROP_ENUMERATE},
 	{"pixelBottom"                 , JSTYLE_PIXELBOTTOM               , JSPROP_ENUMERATE},
 	{"pixelHeight"                 , JSTYLE_PIXELHEIGHT               , JSPROP_ENUMERATE},
 	{"pixelLeft"                   , JSTYLE_PIXELLEFT                 , JSPROP_ENUMERATE},
 	{"pixelRight"                  , JSTYLE_PIXELRIGHT                , JSPROP_ENUMERATE},
 	{"pixelTop"                    , JSTYLE_PIXELTOP                  , JSPROP_ENUMERATE},
 	{"pixelWidth"                  , JSTYLE_PIXELWIDTH                , JSPROP_ENUMERATE},
 	{"posBottom"                   , JSTYLE_POSBOTTOM                 , JSPROP_ENUMERATE},
 	{"posHeight"                   , JSTYLE_POSHEIGHT                 , JSPROP_ENUMERATE},
 	{"posLeft"                     , JSTYLE_POSLEFT                   , JSPROP_ENUMERATE},
 	{"posRight"                    , JSTYLE_POSRIGHT                  , JSPROP_ENUMERATE},
 	{"posTop"                      , JSTYLE_POSTOP                    , JSPROP_ENUMERATE},
 	{"posWidth"                    , JSTYLE_POSWIDTH                  , JSPROP_ENUMERATE},
	{"textAlignLast"               , JSTYLE_TEXTALIGNLAST             , JSPROP_ENUMERATE},
	{"textAutoSpace"               , JSTYLE_TEXTAUTOSPACE             , JSPROP_ENUMERATE},
	{"textDecorationBlink"         , JSTYLE_TEXTDECORATIONBLINK       , JSPROP_ENUMERATE},
 	{"textDecorationLineThrough"   , JSTYLE_TEXTDECORATIONLINETHROUGH , JSPROP_ENUMERATE},
	{"textJustify"                 , JSTYLE_TEXTJUSTIFY               , JSPROP_ENUMERATE},
	{"textKashidaSpace"            , JSTYLE_TEXTKASHIDASPACE          , JSPROP_ENUMERATE},
	{"textOverflow"                , JSTYLE_TEXTOVERFLOW              , JSPROP_ENUMERATE},
	{"textUnderlinePosition"       , JSTYLE_TEXTUNDERLINEPOSITION     , JSPROP_ENUMERATE},
	{"wordBreak"                   , JSTYLE_WORDBREAK                 , JSPROP_ENUMERATE},
	{"wordWrap"                    , JSTYLE_WORDWRAP                  , JSPROP_ENUMERATE},
	{"writingMode"                 , JSTYLE_WRITINGMODE               , JSPROP_ENUMERATE},
	{"zoom"                        , JSTYLE_ZOOM                      , JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jstyle_methods[] =
{
/*	{"getAttribute",        jstyle_getAttribute,        2},
	{"getAttributeNode",    jstyle_getAttributeNode,    1},
	{"getExpression",       jstyle_getExpression,       1},
	{"normalize",           jstyle_normalize,           0}
	{"removeAttribute",     jstyle_removeAttribute,     2},
	{"removeAttributeNode", jstyle_removeAttributeNode, 1},
	{"removeExpression"     jstyle_removeExpression,    1},
	{"setAttribute",        jstyle_setAttribute,        3},
	{"setAttributeNode",    jstyle_setAttributeNode,    1},
	{"setExpression",       jstyle_setExpression,       3},*/
	{0}
};

static JSBool JStyle_Const(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return JS_TRUE;
}

JSClass *getJStyle()
{
	return &JSStyle;
}

JSObject *jstyle_Init(JSContext *cx, JSObject* proto)
{
	//initialize the style Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSStyle,
    				(JSNative) JStyle_Const, 0,
    				jstyle_props, jstyle_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
