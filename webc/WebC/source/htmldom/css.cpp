//
// CSS.CPP - Functions and classes related to the CSS property database
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "webcjhconfig.h"
#include "htmlsdk.h"
#include "util.h"
#include "html.h"
#include "webfonts.hpp"
#include "css.h"
#include "webload.hpp"
#include "htmldoc.hpp"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "webcassert.h"
#include "dllist.h"
#include "rtpchar.h"
#include "webcmem.h"
#include "objref.cpp"



WEBC_DEBUG_LOG_TIME_ACCUMULATOR(TimeInDeclarations)
WEBC_DEBUG_LOG_TIME_ACCUMULATOR(TimeInSelectors)
WEBC_DEBUG_LOG_TIME_ACCUMULATOR(TimeInAddRule)



/*****************************************************************************/
// Macros
/*****************************************************************************/

// this is a prime number on purpose
#define CSS_CFG_SELECTOR_TABLE_SIZE  601

// modify these at your own risk !!
#define IS_CLASS_CHAR(C)        (IS_DIGIT(C) || IS_ALPHA(C) || ((C) == '-') || ((C) == '_'))
#define IS_ID_CHAR(C)           (IS_DIGIT(C) || IS_ALPHA(C) || ((C) == '-') || ((C) == '_'))
#define IS_PSEUDO_CLASS_CHAR(C) (IS_DIGIT(C) || IS_ALPHA(C) || ((C) == '-') || ((C) == '_'))
#define IS_SELECTOR_CHAR(C)     (IS_ID_CHAR(C) || ((C) == '#') || ((C) == '.') || ((C) == ':'))
#define IS_VALUE_CHAR(C)        (IS_DIGIT((C)) || IS_ALPHA((C)) || IS_WHITESPACE((C)) || \
                                 ((C) == ',')  || ((C) == '.')  || ((C) == '#')  || ((C) == '(') || \
                                 ((C) == ')')  || ((C) == '%')  || ((C) == ':')  || ((C) == '/') || \
                                 ((C) == '\\') || ((C) == '\"') || ((C) == '\'') || ((C) == '_') || \
                                 ((C) == '!')  || ((C) == '-') )


// #define CSS_DEBUG

// 24.8 Fixed point
#define PX_PER_EM                256       // 1
#define PX_PER_EX                256       // 1
#define PX_PER_PX                256       // 1
#define PX_PER_IN                23040     // 90
#define PX_PER_CM                9062      // 35.4
#define PX_PER_MM                906       // 3.54
#define PX_PER_PT                340       // 1.33
#define PX_PER_PC                3840      // 15

#define PT_PER_EM                256       // 1
#define PT_PER_EX                256       // 1
#define PT_PER_PX                192       // 0.75
#define PT_PER_IN                18432     // 72
#define PT_PER_CM                7250      // 28.32
#define PT_PER_MM                725       // 2.832
#define PT_PER_PT                256       // 1
#define PT_PER_PC                3072      // 12

/*****************************************************************************/
// Local Constants
/*****************************************************************************/

const int CSS_NUM_FONT_SIZE_VALUES = 7;
const int CSS_NUM_BORDER_WIDTH_VALUES = 3;
const int CSS_NUM_TEXT_ALIGNMENT_VALUES = 4;
const int CSS_NUM_POSITION_VALUES = 4;
const int CSS_NUM_VISIBILITY_VALUES = 4;
const int CSS_NUM_TEXT_DECORATION_VALUES = 5;
const int CSS_NUM_FONT_WEIGHT_VALUES = 14;
const int CSS_NUM_FONT_VARIANT_VALUES = 2;
const int CSS_NUM_FONT_STYLE_VALUES = 3;
const int CSS_NUM_WHITE_SPACE_VALUES = 3;
const int CSS_NUM_BACKGROUND_REPEAT_VALUES = 4;
const int CSS_NUM_BACKGROUND_ATTACHMENT_VALUES = 2;
const int CSS_NUM_TEXT_TRANSFORM_VALUES = 4;
const int CSS_NUM_BORDER_STYLE_VALUES = 9;
const int CSS_NUM_FLOAT_VALUES = 3;
const int CSS_NUM_CLEAR_VALUES = 4;
const int CSS_NUM_DISPLAY_VALUES = 5;
const int CSS_NUM_OVERFLOW_VALUES = 4;
const int CSS_NUM_LIST_STYLE_TYPE_VALUES = 9;
const int CSS_NUM_VERTICAL_ALIGN_VALUES = 8;


/*****************************************************************************/
// Local Data
/*****************************************************************************/

/*****************************************************************************************/
// mapping from HTML font sizes to point values
/*****************************************************************************************/
#if (0)
WEBC_READONLY int riFontSizeAbsToPt_IE6_0_Medium[7]   = {7,9,12,14,18,24,36};
WEBC_READONLY int riFontSizePtToAbs_IE6_0_Medium[CSS_FONT_SIZE_PT_RANGE] =
{
	0,0,0,       // 6-8
	1,1,1,       // 9-11
	2,2,         // 12-13
	3,3,3,3,     // 14-17
	4,4,4,4,4,4, // 18-23
	5,5,5,5,5,5, // 24-35
	5,5,5,5,5,5,
	6            // 36
};

WEBC_READONLY int riFontSizeAbsToPt_IE6_0_Smaller[7]  = {7,9,10,13,16,20,30};
WEBC_READONLY int riFontSizePtToAbs_IE6_0_Smaller[CSS_FONT_SIZE_PT_RANGE] =
{
	0,0,0,       // 6-8
	1,           // 9
	2,2,2,       // 10-12
	3,3,3,       // 13-15
	4,4,4,4,     // 16-19
	5,5,5,5,5,   // 20-29
	5,5,5,5,5,
	6,6,6,6,6,   // 30-36
	6,6
};

WEBC_READONLY int riFontSizeAbsToPt_IE6_0_Smallest[7] = {6,8,10,11,13,16,24};
WEBC_READONLY int riFontSizePtToAbs_IE6_0_Smallest[CSS_FONT_SIZE_PT_RANGE] =
{
	0,0,         // 6-7
	1,1,         // 8-9
	2,           // 10
	3,3,         // 11-12
	4,4,4,       // 13-15
	5,5,5,5,5,   // 16-23
	5,5,5,
	6,6,6,6,6,   // 24-36
	6,6,6,6,6,
	6,6,6
};

WEBC_READONLY int riFontSizeAbsToPt_Mozilla1_6[7]     = {7,7,10,12,13,18,24};
WEBC_READONLY int riFontSizePtToAbs_Mozilla1_6[CSS_FONT_SIZE_PT_RANGE] =
{
	0,0,         // 6-7
	1,1,         // 8-9
	2,2,         // 10-11
	3,3,         // 12-13
	4,4,4,4,     // 14-17
	5,5,5,5,5,5, // 18-23
	6,6,6,6,6,   // 24-36
	6,6,6,6,6,
	6,6,6
};
#endif

WEBC_READONLY int riFontSizeAbsToPt_WebC[7]     = {7,8,9,10,11,14,18};
WEBC_READONLY int riFontSizePtToAbs_WebC[CSS_FONT_SIZE_PT_RANGE] =
{
	0,0,         // 6-7
	1,           // 8
	2,           // 9
	3,           // 10
	4,4,4,       // 11-13
	5,5,5,5,     // 14-17
	6,6,6,6,6,   // 18-36
	6,6,6,6,6,
	6,6,6,6,6,
	6,6,6,6
};

#if (WEBC_SUPPORT_SMALL_SCREEN)
WEBC_READONLY int riFontSizeAbsToPt_SmallScreen[7] = {6,6,7,8,9,12,14};
WEBC_READONLY int riFontSizePtToAbs_SmallScreen[CSS_FONT_SIZE_PT_RANGE] =
{
	0,           // 6
	2,           // 7
	3,           // 8
	4,4,4,       // 9-11
	5,5,         // 12-13
	6,6,6,6,6,   // 14-18
	6,6,6,6,6,   // 19-23
	6,6,6,6,6,   // 24-28
	6,6,6,6,6,   // 29-33
	6,6,6        // 34-36
};

WEBC_READONLY int* giFontSizePointsToAbsolute = riFontSizePtToAbs_SmallScreen;
WEBC_READONLY int* giFontSizeAbsoluteToPoints = riFontSizeAbsToPt_SmallScreen;
#else
WEBC_READONLY int* giFontSizePointsToAbsolute = riFontSizePtToAbs_WebC;
WEBC_READONLY int* giFontSizeAbsoluteToPoints = riFontSizeAbsToPt_WebC;
#endif

WEBC_READONLY DISPLAY_INT unitToPixelTable[] =
{
	PX_PER_EM,
	PX_PER_EX,
	PX_PER_PX,
	PX_PER_IN,
	PX_PER_CM,
	PX_PER_MM,
	PX_PER_PT,
	PX_PER_PC
};

WEBC_READONLY DISPLAY_INT unitToPointsTable[] =
{
	PT_PER_EM,
	PT_PER_EX,
	PT_PER_PX,
	PT_PER_IN,
	PT_PER_CM,
	PT_PER_MM,
	PT_PER_PT,
	PT_PER_PC
};

WEBC_READONLY char* gpFontSizeValues[] =
{
	"xx-small",
	"x-small",
	"small",
	"medium",
	"large",
	"x-large",
	"xx-large",
	WEBC_NULL
};	// CSS_FONT_SIZE_XXSMALL to XXLARGE must be 0 to 6

#if (WEBC_SUPPORT_STYLE_SHEETS)

// WEBC_READONLY char _INHERIT_STR[] = "inherit";

WEBC_READONLY char * gpBorderWidthValues[] = { "thin", "medium", "thick" };
WEBC_READONLY char * gpTextAlignmentValues[] = { "center", "justify", "left", "right" };
WEBC_READONLY char * gpPositionValues[] = { "absolute", "fixed", "relative", "static" };
WEBC_READONLY char * gpVisibilityValues[] = { "hidden", "hide", "show", "visible" };
WEBC_READONLY char * gpTextDecorationValues[] = { "blink", "line-through", "none", "overline", "underline" };

// font weights 100-900 must be labelled 1-9
WEBC_READONLY char * gpFontWeightValues[] =
{
	"000", 	"100", 	"200", 	"300", 	"400", 	"500", 	"600", 	"700",
	"800", "900", "bold", "bolder", "lighter", "normal"
};

WEBC_READONLY char * gpFontVariantValues[] = { "normal", "small-caps" };
WEBC_READONLY char * gpFontStyleValues[] = { "italic", "normal", "oblique" };
WEBC_READONLY char * gpWhiteSpaceValues[] = { "normal", "nowrap",  "pre" };
WEBC_READONLY char * gpBackgroundRepeatValues[] = { "no-repeat", "repeat", "repeat-x", "repeat-y" };
WEBC_READONLY char * gpBackgroundAttachmentValues[] = { "fixed", "scroll" };
WEBC_READONLY char * gpTextTransformValues[] = { "capitalize", "lowercase", "none", "uppercase" };
WEBC_READONLY char * gpBorderStyleValues[] = { "dashed", "dotted", "double", "groove", "inset", "none", "outset", "ridge", "solid" };
WEBC_READONLY char * gpFloatValues[] = { "left", "none", "right" };
WEBC_READONLY char * gpClearValues[] = { "both", "left", "none", "right" };
WEBC_READONLY char * gpDisplayValues[] = { "block", "inline", "inline-block", "list-item", "none" };
WEBC_READONLY char * gpOverflowValues[] = { "visible", "hidden", "scroll", "auto" };

WEBC_READONLY char * gpListStyleTypeValues[] =
{
	"circle",
	"decimal",
	"disc",
	"lower-alpha",
	"lower-roman",
	"none",
	"square",
	"upper-alpha",
	"upper-roman"
};

WEBC_READONLY char * gpVerticalAlignValues[] = { "baseline", "bottom", "middle", "sub", "super", "text-bottom", "text-top", "top" };


WEBC_READONLY char * gpCSSPropertyNames[] =
{
	"A:LINK",
	"A:VISITED",
	"BACKGROUND",
	"BACKGROUND-ATTACHMENT",
	"BACKGROUND-COLOR",
	"BACKGROUND-IMAGE",
	"BACKGROUND-POSITION",
	"BACKGROUND-POSITION-X",		// doesn't acutally exist, added to sync with enums in css.h
	"BACKGROUND-POSITION-Y",		// doesn't acutally exist, added to sync with enums in css.h
	"BACKGROUND-REPEAT",
	"BORDER",						// 10
	"BORDER-BOTTOM",
	"BORDER-BOTTOM-COLOR",
	"BORDER-BOTTOM-STYLE",
	"BORDER-BOTTOM-WIDTH",
	"BORDER-COLOR",
	"BORDER-LEFT",
	"BORDER-LEFT-COLOR",
	"BORDER-LEFT-STYLE",
	"BORDER-LEFT-WIDTH",
	"BORDER-RIGHT", 				// 20
	"BORDER-RIGHT-COLOR",
	"BORDER-RIGHT-STYLE",
	"BORDER-RIGHT-WIDTH",
	"BORDER-STYLE",
	"BORDER-TOP",
	"BORDER-TOP-COLOR",
	"BORDER-TOP-STYLE",
	"BORDER-TOP-WIDTH",
	"BORDER-WIDTH",
	"BOTTOM",						// 30
	"CLEAR",
	"COLOR",
	"DISPLAY",
	"FLOAT",
	"FONT",
	"FONT-FAMILY",
	"FONT-SIZE",
	"FONT-STYLE",
	"FONT-VARIANT",
	"FONT-WEIGHT", 				// 40
	"HEIGHT",
	"LEFT",
	"LETTER-SPACING",
	"LINE-HEIGHT",
	"LINK-COLOR",
	"LIST-STYLE",
	"LIST-STYLE-IMAGE",
	"LIST-STYLE-POSITION",
	"LIST-STYLE-TYPE",
	"MARGIN", 					// 50
	"MARGIN-BOTTOM",
	"MARGIN-LEFT",
	"MARGIN-RIGHT",
	"MARGIN-TOP",
    "OPACITY",
    "OUTLINE",
    "OUTLINE-COLOR",
    "OUTLINE-STYLE",
    "OUTLINE-WIDTH",
    "OVERFLOW",      			// 60  added pmb
    "OVERFLOW_X", 				// added pmb
    "OVERFLOW_Y", 				// added pmb
    "PADDING",
    "PADDING-BOTTOM",           // previous 60 pmb
    "PADDING-LEFT",
    "PADDING-RIGHT",
    "PADDING-TOP",
    "POSITION",
    "POSITION-X",
    "POSITION-Y",    			// 70 pmb
    "RIGHT",
    "TEXT-ALIGN",
    "TEXT-DECORATION",
    "TEXT-INDENT",              // previous 70 pmb
    "TEXT-TRANSFORM",
    "TOP",
    "VERTICAL-ALIGN",
    "VISIBILITY",
    "WHITE-SPACE",
    "WIDTH",  					// 80 pmb
    "WORD-SPACING",
    "Z-INDEX"
};

// Uncoment this and call this function to verify css registrion
// #define SHOWCSSREGISTRATION
#ifdef SHOWCSSREGISTRATION
#define PRINTCSSREG printf
void showCSSregitration(void)
{
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_ALINK_COLOR", gpCSSPropertyNames[CSS_PROPERTY_ALINK_COLOR]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_VLINK_COLOR", gpCSSPropertyNames[CSS_PROPERTY_VLINK_COLOR]);           // 1
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BACKGROUND", gpCSSPropertyNames[CSS_PROPERTY_BACKGROUND]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BACKGROUND_ATTACHMENT", gpCSSPropertyNames[CSS_PROPERTY_BACKGROUND_ATTACHMENT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BACKGROUND_COLOR", gpCSSPropertyNames[CSS_PROPERTY_BACKGROUND_COLOR]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BACKGROUND_IMAGE", gpCSSPropertyNames[CSS_PROPERTY_BACKGROUND_IMAGE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BACKGROUND_POSITION", gpCSSPropertyNames[CSS_PROPERTY_BACKGROUND_POSITION]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BACKGROUND_X_POSITION", gpCSSPropertyNames[CSS_PROPERTY_BACKGROUND_X_POSITION]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BACKGROUND_Y_POSITION", gpCSSPropertyNames[CSS_PROPERTY_BACKGROUND_Y_POSITION]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BACKGROUND_REPEAT", gpCSSPropertyNames[CSS_PROPERTY_BACKGROUND_REPEAT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER", gpCSSPropertyNames[CSS_PROPERTY_BORDER]);				//10
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_BOTTOM", gpCSSPropertyNames[CSS_PROPERTY_BORDER_BOTTOM]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_BOTTOM_COLOR", gpCSSPropertyNames[CSS_PROPERTY_BORDER_BOTTOM_COLOR]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_BOTTOM_STYLE", gpCSSPropertyNames[CSS_PROPERTY_BORDER_BOTTOM_STYLE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_BOTTOM_WIDTH", gpCSSPropertyNames[CSS_PROPERTY_BORDER_BOTTOM_WIDTH]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_COLOR", gpCSSPropertyNames[CSS_PROPERTY_BORDER_COLOR]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_LEFT", gpCSSPropertyNames[CSS_PROPERTY_BORDER_LEFT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_LEFT_COLOR", gpCSSPropertyNames[CSS_PROPERTY_BORDER_LEFT_COLOR]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_LEFT_STYLE", gpCSSPropertyNames[CSS_PROPERTY_BORDER_LEFT_STYLE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_LEFT_WIDTH", gpCSSPropertyNames[CSS_PROPERTY_BORDER_LEFT_WIDTH]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_RIGHT", gpCSSPropertyNames[CSS_PROPERTY_BORDER_RIGHT]);			// 20
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_RIGHT_COLOR", gpCSSPropertyNames[CSS_PROPERTY_BORDER_RIGHT_COLOR]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_RIGHT_STYLE", gpCSSPropertyNames[CSS_PROPERTY_BORDER_RIGHT_STYLE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_RIGHT_WIDTH", gpCSSPropertyNames[CSS_PROPERTY_BORDER_RIGHT_WIDTH]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_STYLE", gpCSSPropertyNames[CSS_PROPERTY_BORDER_STYLE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_TOP", gpCSSPropertyNames[CSS_PROPERTY_BORDER_TOP]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_TOP_COLOR", gpCSSPropertyNames[CSS_PROPERTY_BORDER_TOP_COLOR]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_TOP_STYLE", gpCSSPropertyNames[CSS_PROPERTY_BORDER_TOP_STYLE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_TOP_WIDTH", gpCSSPropertyNames[CSS_PROPERTY_BORDER_TOP_WIDTH]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BORDER_WIDTH", gpCSSPropertyNames[CSS_PROPERTY_BORDER_WIDTH]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_BOTTOM", gpCSSPropertyNames[CSS_PROPERTY_BOTTOM]);				// 30
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_CLEAR", gpCSSPropertyNames[CSS_PROPERTY_CLEAR]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_COLOR", gpCSSPropertyNames[CSS_PROPERTY_COLOR]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_DISPLAY", gpCSSPropertyNames[CSS_PROPERTY_DISPLAY]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_FLOAT", gpCSSPropertyNames[CSS_PROPERTY_FLOAT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_FONT", gpCSSPropertyNames[CSS_PROPERTY_FONT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_FONT_FAMILY", gpCSSPropertyNames[CSS_PROPERTY_FONT_FAMILY]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_FONT_SIZE", gpCSSPropertyNames[CSS_PROPERTY_FONT_SIZE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_FONT_STYLE", gpCSSPropertyNames[CSS_PROPERTY_FONT_STYLE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_FONT_VARIANT", gpCSSPropertyNames[CSS_PROPERTY_FONT_VARIANT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_FONT_WEIGHT", gpCSSPropertyNames[CSS_PROPERTY_FONT_WEIGHT]);			// 40
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_HEIGHT", gpCSSPropertyNames[CSS_PROPERTY_HEIGHT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_LEFT", gpCSSPropertyNames[CSS_PROPERTY_LEFT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_LETTER_SPACING", gpCSSPropertyNames[CSS_PROPERTY_LETTER_SPACING]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_LINE_HEIGHT", gpCSSPropertyNames[CSS_PROPERTY_LINE_HEIGHT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_LINK_COLOR", gpCSSPropertyNames[CSS_PROPERTY_LINK_COLOR]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_LIST_STYLE", gpCSSPropertyNames[CSS_PROPERTY_LIST_STYLE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_LIST_STYLE_IMAGE", gpCSSPropertyNames[CSS_PROPERTY_LIST_STYLE_IMAGE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_LIST_STYLE_POSITION", gpCSSPropertyNames[CSS_PROPERTY_LIST_STYLE_POSITION]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_LIST_STYLE_TYPE", gpCSSPropertyNames[CSS_PROPERTY_LIST_STYLE_TYPE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_MARGIN", gpCSSPropertyNames[CSS_PROPERTY_MARGIN]);				// 50
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_MARGIN_BOTTOM", gpCSSPropertyNames[CSS_PROPERTY_MARGIN_BOTTOM]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_MARGIN_LEFT", gpCSSPropertyNames[CSS_PROPERTY_MARGIN_LEFT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_MARGIN_RIGHT", gpCSSPropertyNames[CSS_PROPERTY_MARGIN_RIGHT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_MARGIN_TOP", gpCSSPropertyNames[CSS_PROPERTY_MARGIN_TOP]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_OPACITY", gpCSSPropertyNames[CSS_PROPERTY_OPACITY]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_OUTLINE", gpCSSPropertyNames[CSS_PROPERTY_OUTLINE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_OUTLINE_COLOR", gpCSSPropertyNames[CSS_PROPERTY_OUTLINE_COLOR]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_OUTLINE_STYLE", gpCSSPropertyNames[CSS_PROPERTY_OUTLINE_STYLE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_OUTLINE_WIDTH", gpCSSPropertyNames[CSS_PROPERTY_OUTLINE_WIDTH]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_OVERFLOW", gpCSSPropertyNames[CSS_PROPERTY_OVERFLOW]);      			// 60  added pmb
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_OVERFLOW_X", gpCSSPropertyNames[CSS_PROPERTY_OVERFLOW_X]);      			// added pmb
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_OVERFLOW_Y", gpCSSPropertyNames[CSS_PROPERTY_OVERFLOW_Y]);      			// added pmb
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_PADDING", gpCSSPropertyNames[CSS_PROPERTY_PADDING]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_PADDING_BOTTOM", gpCSSPropertyNames[CSS_PROPERTY_PADDING_BOTTOM]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_PADDING_LEFT", gpCSSPropertyNames[CSS_PROPERTY_PADDING_LEFT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_PADDING_RIGHT", gpCSSPropertyNames[CSS_PROPERTY_PADDING_RIGHT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_PADDING_TOP", gpCSSPropertyNames[CSS_PROPERTY_PADDING_TOP]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_POSITION", gpCSSPropertyNames[CSS_PROPERTY_POSITION]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_POSITION_X", gpCSSPropertyNames[CSS_PROPERTY_POSITION_X]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_POSITION_Y", gpCSSPropertyNames[CSS_PROPERTY_POSITION_Y]);				// 70
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_RIGHT", gpCSSPropertyNames[CSS_PROPERTY_RIGHT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_TEXT_ALIGN", gpCSSPropertyNames[CSS_PROPERTY_TEXT_ALIGN]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_TEXT_DECORATION", gpCSSPropertyNames[CSS_PROPERTY_TEXT_DECORATION]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_TEXT_INDENT", gpCSSPropertyNames[CSS_PROPERTY_TEXT_INDENT]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_TEXT_TRANSFORM", gpCSSPropertyNames[CSS_PROPERTY_TEXT_TRANSFORM]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_TOP", gpCSSPropertyNames[CSS_PROPERTY_TOP]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_VERTICAL_ALIGN", gpCSSPropertyNames[CSS_PROPERTY_VERTICAL_ALIGN]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_VISIBILITY", gpCSSPropertyNames[CSS_PROPERTY_VISIBILITY]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_WHITE_SPACE", gpCSSPropertyNames[CSS_PROPERTY_WHITE_SPACE]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_WIDTH", gpCSSPropertyNames[CSS_PROPERTY_WIDTH]);					// 80
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_WORD_SPACING", gpCSSPropertyNames[CSS_PROPERTY_WORD_SPACING]);
	PRINTCSSREG("%s == %s \n", "CSS_PROPERTY_ZINDEX", gpCSSPropertyNames[CSS_PROPERTY_ZINDEX]);
}
#endif // SHOWCSSREGISTRATION
#endif // WEBC_SUPPORT_STYLE_SHEETS

/*****************************************************************************/
// Local Functions
/*****************************************************************************/

static long _bucket_scan(WEBC_CHAR *string, WEBC_CHAR *bucket, int numBuckets, long bucketSize);
extern "C" int _StringListItemCompare(void *item, void *list, int index);

#if (WEBC_SUPPORT_STYLE_SHEETS)

/*****************************************************************************/
// Local Classes
/*****************************************************************************/

/* This is experimental code that adopts a hash table scheme to speed up
   CSS property lookups.  Use at your own risk */

#define SELECTOR_VALUE_FLAG_INHERIT     0x80000000L

#define CSS_CASCADE_ORDER(I,O,S,T)      ( ((I)?0x40000000:0) | (((O)==CSS_ORIGIN_INTERNAL)?0x20000000:0) | (((S)&0xfff)<<17) | ((T)&0x1ffff) )
class CSSSelectorValuePair
{
protected:
	unsigned long          mOrder;
	CSSSelector          * mpSelector;
	CSSPropertyValue       mValue;
	CSSSelectorValuePair * mpNext;

public:
	CSSSelectorValuePair (CSSSelector *selector, CSSPropertyValue *value, long cascadeOrder)
	{
		mOrder = 0;
		mpSelector = selector;
		mpNext = 0;


		setOrder(cascadeOrder);
		setValue(value);

		mpSelector->addReference();
	}

	~CSSSelectorValuePair (void)
	{
		mpSelector->removeReference();
	}

	CSSSelector * getSelector (void)
	{
		return mpSelector;
	}

	CSSSelectorValuePair * getNext (void)
	{
		return mpNext;
	}

	void setNext(CSSSelectorValuePair *next)
	{
		mpNext = next;
	}

	CSSPropertyValue * getValue (void)
	{
		return (mOrder & SELECTOR_VALUE_FLAG_INHERIT)? 0 : &mValue;
	}

	void setValue (CSSPropertyValue *value)
	{
		if (value)
		{
			mValue = *value;
			mOrder &= ~SELECTOR_VALUE_FLAG_INHERIT;
		}
		else
		{
			mOrder |= SELECTOR_VALUE_FLAG_INHERIT;
		}
	}

	long getOrder(void)
	{
		return ((long) (mOrder & ~SELECTOR_VALUE_FLAG_INHERIT));
	}

	void setOrder(long order)
	{
		mOrder &= SELECTOR_VALUE_FLAG_INHERIT;
		mOrder |= order & ~SELECTOR_VALUE_FLAG_INHERIT;
	}

};

#define SELECTOR_TYPE_TAG             0
#define SELECTOR_TYPE_CLASS           1
#define SELECTOR_TYPE_ID              2
#define SELECTOR_TYPE_ELEMENT         3
#define SELECTOR_TYPE_STYLE_ELEMENT   4

struct CSSPropertySearch
{
	CSSStyleElement  *element;
	CSSPropertyValue *bestValue;
	long              bestOrder;
};

class CSSSelectorPropertyHash : public HashKey
{
public:
	CSSSelectorPropertyHash (void);
	virtual ~CSSSelectorPropertyHash (void);

	// HashKey methods (subclasses must implement Equals and HashCode)
	virtual void       Dispose  (void);

	// CSSPartialSelectorTypePair methods
	virtual int             getSelectorType         (void) = 0;
	virtual CSSPropertyType getType                 (void) = 0;
	virtual int             addSelectorValuePair    (CSSSelector *selector, CSSPropertyValue *value, long order) = 0;
	virtual int             removeSelectorValuePair (CSSSelector *selector, long order) = 0;
	virtual void            getMostSpecificValue    (CSSPropertySearch *search) = 0;
};

class CSSSelectorValuePairListHash : public CSSSelectorPropertyHash
{
protected:
	CSSSelectorValuePair * mpSelectorValueList;
	CSSPropertyType        mType;
	HashCode_t             miHashCode;

public:
	CSSSelectorValuePairListHash (CSSPropertyType type);
	virtual ~CSSSelectorValuePairListHash (void);

	// HashKey methods (subclasses must implement Equals and HashCode)
	virtual HashCode_t HashCode (void);

	// CSSPartialSelectorTypePair methods
	virtual CSSPropertyType getType                 (void);
	virtual int             addSelectorValuePair    (CSSSelector *selector, CSSPropertyValue *value, long order);
	virtual int             removeSelectorValuePair (CSSSelector *selector, long order);
	virtual void            getMostSpecificValue    (CSSPropertySearch *search);
};


class CSSElementSelectorHash;

class CSSStyleElementHash : public CSSSelectorPropertyHash
{
protected:
	CSSStyleElement*  mpElement;
	HashCode_t        miHashCode;

public:
	CSSStyleElementHash (CSSStyleElement* element, CSSPropertyType type = CSS_PROPERTY_UNKNOWN);
	virtual ~CSSStyleElementHash (void);

	// HashKey methods
	virtual WEBC_BOOL       Equals   (HashKey* key);
	virtual HashCode_t HashCode (void);

	// CSSSelectorValuePairListHash methods
	virtual CSSPropertyType getType              (void);
	virtual int             getSelectorType      (void);
	virtual void            getMostSpecificValue (CSSPropertySearch* search);
	virtual int             addSelectorValuePair (CSSSelector* selector, CSSPropertyValue* value, long order);
	virtual int             removeSelectorValuePair (CSSSelector* selector, long order);

	// CSSStyleElementHash methods
	CSSStyleElement*     getElement (void);
	void                 addElementProperty (CSSElementSelectorHash* p);

	CSSStyleElementHash* next;
	CSSStyleElementHash* prev;
};


class CSSElementSelectorHash : public CSSStyleElementHash
{
protected:
	CSSPropertyValue  mValue;
	CSSPropertyType   mType;
	char              mValueValid;

public:
	CSSElementSelectorHash (CSSStyleElement* element, CSSPropertyType type);
	virtual ~CSSElementSelectorHash (void);

	// HashKey methods
	virtual WEBC_BOOL       Equals   (HashKey* key);

	// CSSSelectorValuePairListHash methods
	virtual CSSPropertyType getType              (void);
	virtual int             getSelectorType      (void);
	virtual void            getMostSpecificValue (CSSPropertySearch* search);
	virtual int             addSelectorValuePair (CSSSelector* selector, CSSPropertyValue* value, long order);
	virtual int             removeSelectorValuePair (CSSSelector* selector, long order);
};


class CSSIdSelectorHashLite : public CSSSelectorValuePairListHash
{
protected:
	const WEBC_CHAR *mpId;

public:
	CSSIdSelectorHashLite (const WEBC_CHAR *id, CSSPropertyType type);
	virtual ~CSSIdSelectorHashLite (void);

	// HashKey methods
	virtual WEBC_BOOL       Equals   (HashKey *key);

	// CSSSelectorValuePairListHash methods
	virtual int        getSelectorType (void);
	virtual void       getMostSpecificValue (CSSPropertySearch *search);

	// CSSIdSelectorHash methods
	const WEBC_CHAR   * getId (void);
};

class CSSIdSelectorHash : public CSSIdSelectorHashLite
{
public:
	CSSIdSelectorHash (const WEBC_CHAR *id, CSSPropertyType type);
	virtual ~CSSIdSelectorHash (void);
};

class CSSClassSelectorHashLite : public CSSSelectorValuePairListHash
{
protected:
	const WEBC_CHAR *mpClass;

public:
	CSSClassSelectorHashLite (const WEBC_CHAR *className, CSSPropertyType type);
	virtual ~CSSClassSelectorHashLite (void);

	// HashKey methods
	virtual WEBC_BOOL       Equals   (HashKey *key);

	// CSSSelectorValuePairListHash methods
	virtual int        getSelectorType (void);
	virtual void       getMostSpecificValue (CSSPropertySearch *search);

	// CSSIdSelectorHash methods
	const WEBC_CHAR   * getClass (void);
};

class CSSClassSelectorHash : public CSSClassSelectorHashLite
{
public:
	CSSClassSelectorHash (const WEBC_CHAR *className, CSSPropertyType type);
	virtual ~CSSClassSelectorHash (void);
};


class CSSTagSelectorHash : public CSSSelectorValuePairListHash
{
protected:
	HTMLTagType mTag;

public:
	CSSTagSelectorHash (HTMLTagType tag, CSSPropertyType type);
	virtual ~CSSTagSelectorHash (void);

	// HashKey methods
	virtual WEBC_BOOL       Equals   (HashKey *key);

	// CSSSelectorValuePairListHash methods
	virtual int        getSelectorType (void);
	virtual void       getMostSpecificValue (CSSPropertySearch *search);

	// CSSIdSelectorHash methods
	HTMLTagType        getTag (void);
};


#define LEFT_BIT_SHIFT        5
#define RIGHT_BIT_SHIFT       2
#define RANDOM_SEED           5297


/*****************************************************************************/
// CSSSelectorPropertyHash methods
/*****************************************************************************/

CSSSelectorPropertyHash::CSSSelectorPropertyHash (void)
{
}

CSSSelectorPropertyHash::~CSSSelectorPropertyHash (void)
{
}

void CSSSelectorPropertyHash::Dispose (void)
{
	WEBC_DELETE(this);
}


/*****************************************************************************/
// CSSSelectorValuePairListHash methods
/*****************************************************************************/

CSSSelectorValuePairListHash::CSSSelectorValuePairListHash (CSSPropertyType type)
{
	mpSelectorValueList = 0;
	mType = type;
}

CSSSelectorValuePairListHash::~CSSSelectorValuePairListHash (void)
{
	CSSSelectorValuePair *pNext;

	while (mpSelectorValueList)
	{
		pNext = mpSelectorValueList->getNext();
		WEBC_DELETE(mpSelectorValueList);
		mpSelectorValueList = pNext;
	}
}

HashCode_t CSSSelectorValuePairListHash::HashCode (void)
{
	return (miHashCode);
}

CSSPropertyType CSSSelectorValuePairListHash::getType (void)
{
	return (mType);
}

int CSSSelectorValuePairListHash::addSelectorValuePair (CSSSelector *selector, CSSPropertyValue *value, long order)
{
	CSSSelectorValuePair *prev = 0;
	CSSSelectorValuePair *insertAfter = 0;
	CSSSelectorValuePair *insertBefore = 0;
	CSSSelectorValuePair *pair = mpSelectorValueList;

	// first check to make sure this selector isn't already in the list
	while (pair)
	{
		if (!insertBefore && (order >= pair->getOrder()))
		{
			insertAfter = prev;
			insertBefore = pair;
		}

		if (pair->getSelector() == selector)
		{
			if (order >= pair->getOrder())
			{
				// move it up in the list if necessary
				if (prev != insertAfter)
				{
					WEBC_ASSERT(prev);

					// remove it from the list
					prev->setNext(pair->getNext());

					// insert in the correct order
					if (insertAfter)
					{
						pair->setNext(insertAfter->getNext());
						insertAfter->setNext(pair);
					}
					else
					{
						// move to the front of the list
						pair->setNext(mpSelectorValueList);
						mpSelectorValueList = pair;
					}

					WEBC_ASSERT(!insertAfter || (insertAfter->getOrder() > order));
					WEBC_ASSERT(!pair->getNext() || (order >= pair->getNext()->getOrder()));
				}

				pair->setValue(value);
				pair->setOrder(order);
			}

			return (0);
		}
		prev = pair;
		pair = pair->getNext();
	}

	if (!insertBefore)
	{
		insertAfter = prev;
	}

	// not in the list; create a new node
	WEBC_NEW_VERBOSE(pair, CSSSelectorValuePair(selector, value, order),"SelectorValuePair");
	if (pair)
	{
		// insert in the correct order
		if (insertAfter)
		{
			pair->setNext(insertAfter->getNext());
			insertAfter->setNext(pair);
		}
		else
		{
			// move to the front of the list
			pair->setNext(mpSelectorValueList);
			mpSelectorValueList = pair;
		}

		WEBC_ASSERT(!insertAfter || (insertAfter->getOrder() > pair->getOrder()));
		WEBC_ASSERT(!pair->getNext() || (pair->getOrder() >= pair->getNext()->getOrder()));

		return (0);
	}

	// failure
	return (-1);
}

int CSSSelectorValuePairListHash::removeSelectorValuePair (CSSSelector *selector, long order)
{
	CSSSelectorValuePair *prev = 0;
	CSSSelectorValuePair *pair = mpSelectorValueList;

	// first check to make sure this selector isn't already in the list
	while (pair)
	{
		if ((pair->getSelector() == selector) && (order == pair->getOrder()))
		{
			// remove it from the list
			if (prev)
			{
				prev->setNext(pair->getNext());
			}
			else
			{
				mpSelectorValueList = pair->getNext();
			}

			// delete the object
			WEBC_DELETE(pair);

			// success
			return (0);
		}
		prev = pair;
		pair = pair->getNext();
	}

	// failure
	return (-1);
}

void CSSSelectorValuePairListHash::getMostSpecificValue (CSSPropertySearch *search)
{
	CSSSelectorValuePair *pair = mpSelectorValueList;

	while (pair && (pair->getOrder() > search->bestOrder))
	{
		if (pair->getSelector()->elementMatches(search->element))
		{
			search->bestValue = pair->getValue();
			search->bestOrder = pair->getOrder();
		}
		pair = pair->getNext();
	}
}


/*****************************************************************************/
// CSSStyleElementHash methods
/*****************************************************************************/

CSSStyleElementHash::CSSStyleElementHash (CSSStyleElement *element, CSSPropertyType type)
	: CSSSelectorPropertyHash ()
{
	mpElement = element;

	HashCode_t h = (HashCode_t) mpElement;
	h = ((h >> 13) | (h << 19)) ^ (type | (type << 8) | (type << 16) | (type << 24));

	miHashCode = h;

	DLLIST_INIT(this);
}

CSSStyleElementHash::~CSSStyleElementHash (void)
{
	DLLIST_REMOVE(this);
}

CSSPropertyType CSSStyleElementHash::getType (void)
{
	return (CSS_PROPERTY_UNKNOWN);
}

int CSSStyleElementHash::getSelectorType (void)
{
	return (SELECTOR_TYPE_STYLE_ELEMENT);
}

WEBC_BOOL CSSStyleElementHash::Equals (HashKey *key)
{
	CSSSelectorPropertyHash* other = (CSSSelectorPropertyHash*) key;

	if (other->getSelectorType() == SELECTOR_TYPE_STYLE_ELEMENT)
	{
		CSSElementSelectorHash* obj = (CSSElementSelectorHash*) other;
		return (mpElement == obj->getElement());
	}

	return (0);
}

HashCode_t CSSStyleElementHash::HashCode (void)
{
	return (miHashCode);
}

CSSStyleElement * CSSStyleElementHash::getElement (void)
{
	return (mpElement);
}

void CSSStyleElementHash::addElementProperty (CSSElementSelectorHash* p)
{
	DLLIST_INSERT_BEFORE(this, p);
}

void CSSStyleElementHash::getMostSpecificValue (CSSPropertySearch *search)
{
}

int CSSStyleElementHash::addSelectorValuePair (CSSSelector *selector, CSSPropertyValue *value, long order)
{
	return (0);
}

int CSSStyleElementHash::removeSelectorValuePair (CSSSelector *selector, long order)
{
	return (0);
}


/*****************************************************************************/
// CSSElementSelectorHash methods
/*****************************************************************************/

CSSElementSelectorHash::CSSElementSelectorHash (
		CSSStyleElement* element,
		CSSPropertyType type
	)
	: CSSStyleElementHash (element, type)
{
	mType = type;
	mValueValid = 0;
}

CSSElementSelectorHash::~CSSElementSelectorHash (void)
{
}

CSSPropertyType CSSElementSelectorHash::getType (void)
{
	return (mType);
}

int CSSElementSelectorHash::getSelectorType (void)
{
	return (SELECTOR_TYPE_ELEMENT);
}

WEBC_BOOL CSSElementSelectorHash::Equals (HashKey *key)
{
	CSSSelectorPropertyHash* other = (CSSSelectorPropertyHash*) key;

	if ((other->getType() == mType) && (other->getSelectorType() == SELECTOR_TYPE_ELEMENT))
	{
		CSSElementSelectorHash* obj = (CSSElementSelectorHash*) other;
		return (mpElement == obj->getElement());
	}

	return (0);
}

void CSSElementSelectorHash::getMostSpecificValue (CSSPropertySearch *search)
{
	if (mValueValid)
	{
		search->bestValue = (mValueValid == 1)? &mValue : 0;
		search->bestOrder = 0x7fffffff;
	}
}

int CSSElementSelectorHash::addSelectorValuePair (CSSSelector *selector, CSSPropertyValue *value, long order)
{
	if (value)
	{
		mValueValid = 1;
		mValue = *value;
	}
	else
	{
		mValueValid = 2;
	}
	return (0);
}

int CSSElementSelectorHash::removeSelectorValuePair (CSSSelector *selector, long order)
{
	mValueValid = 0;
	return (0);
}


/*****************************************************************************/
// CSSIdSelectorHash methods
/*****************************************************************************/

CSSIdSelectorHash::CSSIdSelectorHash (const WEBC_CHAR *id, CSSPropertyType type)
	: CSSIdSelectorHashLite(webc_string_table_add(id, __FILE__, __LINE__), type)
{
}

CSSIdSelectorHash::~CSSIdSelectorHash (void)
{
	webc_string_table_remove(mpId, __FILE__, __LINE__);
}


/*****************************************************************************/
// CSSIdSelectorHashLite methods
/*****************************************************************************/

CSSIdSelectorHashLite::CSSIdSelectorHashLite (const WEBC_CHAR *id, CSSPropertyType type)
	: CSSSelectorValuePairListHash(type)
{
	mpId = id;

	unsigned long h = 9293;

	h = h ^ ( (h << LEFT_BIT_SHIFT) + (h >> RIGHT_BIT_SHIFT) + mType);

	for (const WEBC_CHAR *s = mpId; s && *s; s++)
	{
		h = h ^ ( (h << LEFT_BIT_SHIFT) + (h >> RIGHT_BIT_SHIFT) + *s);
	}

	miHashCode = h;
}

CSSIdSelectorHashLite::~CSSIdSelectorHashLite (void)
{
}

int CSSIdSelectorHashLite::getSelectorType (void)
{
	return (SELECTOR_TYPE_ID);
}

WEBC_BOOL CSSIdSelectorHashLite::Equals (HashKey *key)
{
	CSSSelectorPropertyHash *other = (CSSSelectorPropertyHash *) key;

	if ((other->getType() == mType) && (other->getSelectorType() == SELECTOR_TYPE_ID))
	{
		CSSIdSelectorHash *obj = (CSSIdSelectorHash *) other;
		return (obj->getId() == mpId);
	}

	return (0);
}

const WEBC_CHAR * CSSIdSelectorHashLite::getId (void)
{
	return (mpId);
}

void CSSIdSelectorHashLite::getMostSpecificValue (CSSPropertySearch *search)
{
	CSSSelectorValuePair *pair = mpSelectorValueList;

	while (pair && (pair->getOrder() > search->bestOrder))
	{
		if (pair->getSelector()->matchesClassTagParent(search->element))
		{
			search->bestValue = pair->getValue();
			search->bestOrder = pair->getOrder();
		}
		pair = pair->getNext();
	}
}


/*****************************************************************************/
// CSSClassSelectorHash methods
/*****************************************************************************/

CSSClassSelectorHash::CSSClassSelectorHash (const WEBC_CHAR *className, CSSPropertyType type)
	: CSSClassSelectorHashLite(webc_string_table_add(className, __FILE__, __LINE__), type)
{
}

CSSClassSelectorHash::~CSSClassSelectorHash (void)
{
	webc_string_table_remove(mpClass, __FILE__, __LINE__);
}


/*****************************************************************************/
// CSSClassSelectorHashLite methods
/*****************************************************************************/

CSSClassSelectorHashLite::CSSClassSelectorHashLite (const WEBC_CHAR *className, CSSPropertyType type)
	: CSSSelectorValuePairListHash(type)
{
	mpClass = className;

	unsigned long h = 398214537;

	h = h ^ ( (h << LEFT_BIT_SHIFT) + (h >> RIGHT_BIT_SHIFT) + mType);

	for (const WEBC_CHAR *s = mpClass; s && *s; s++)
	{
		h = h ^ ( (h << LEFT_BIT_SHIFT) + (h >> RIGHT_BIT_SHIFT) + *s);
	}

	miHashCode = h;
}

CSSClassSelectorHashLite::~CSSClassSelectorHashLite (void)
{
}


int CSSClassSelectorHashLite::getSelectorType (void)
{
	return (SELECTOR_TYPE_CLASS);
}

WEBC_BOOL CSSClassSelectorHashLite::Equals (HashKey *key)
{
	CSSSelectorPropertyHash *other = (CSSSelectorPropertyHash *) key;

	if ((other->getType() == mType) && (other->getSelectorType() == SELECTOR_TYPE_CLASS))
	{
		CSSClassSelectorHash *obj = (CSSClassSelectorHash *) other;
		return (obj->getClass() == mpClass);
	}

	return (0);
}

const WEBC_CHAR * CSSClassSelectorHashLite::getClass (void)
{
	return (mpClass);
}

void CSSClassSelectorHashLite::getMostSpecificValue (CSSPropertySearch *search)
{
	CSSSelectorValuePair *pair = mpSelectorValueList;

	while (pair && (pair->getOrder() > search->bestOrder))
	{
		if (pair->getSelector()->matchesTagParent(search->element))
		{
			search->bestValue = pair->getValue();
			search->bestOrder = pair->getOrder();
		}
		pair = pair->getNext();
	}
}


/*****************************************************************************/
// CSSTagSelectorHash methods
/*****************************************************************************/

CSSTagSelectorHash::CSSTagSelectorHash (HTMLTagType tag, CSSPropertyType type)
	: CSSSelectorValuePairListHash(type)
{
	mTag = tag;
	miHashCode = ((mTag >> 1) | (mType << 9) | (mTag << 17) | (mType << 25)) ^
	              (mType | (mTag << 8) | (mType << 16) | (mTag << 24));
}

CSSTagSelectorHash::~CSSTagSelectorHash (void)
{
}

int CSSTagSelectorHash::getSelectorType (void)
{
	return (SELECTOR_TYPE_TAG);
}

WEBC_BOOL CSSTagSelectorHash::Equals (HashKey *key)
{
	CSSSelectorPropertyHash *other = (CSSSelectorPropertyHash *) key;

	if ((other->getType() == mType) && (other->getSelectorType() == SELECTOR_TYPE_TAG))
	{
		CSSTagSelectorHash *obj = (CSSTagSelectorHash *) other;
		return (mTag == obj->getTag());
	}

	return (0);
}

HTMLTagType CSSTagSelectorHash::getTag (void)
{
	return (mTag);
}

void CSSTagSelectorHash::getMostSpecificValue (CSSPropertySearch *search)
{
	CSSSelectorValuePair *pair = mpSelectorValueList;

	while (pair && (pair->getOrder() > search->bestOrder))
	{
		if (pair->getSelector()->matchesParent(search->element))
		{
			search->bestValue = pair->getValue();
			search->bestOrder = pair->getOrder();
		}
		pair = pair->getNext();
	}
}


#ifndef CSS_DEBUG
#define PRINTF1(A)
#define PRINTF2(A,B)
#define PRINTF3(A,B,C)
  // remove warning "Expression has no side-effects"
  #if defined(__MET__) || defined(WEBC_METAWARE_HCARM)
    #pragma Offwarn(88)
  #endif
#else
#define PRINTF printf
#endif


CSSDocumentContext::CSSDocumentContext(HTMLDocument *pDocument)
	: mPropertyTable(1987), mSelectorTable(CSS_CFG_SELECTOR_TABLE_SIZE)
{
	miDeleteLock = 0;
	mbDeleted = 0;
	mDocument.Set(pDocument);

 	miHighestIdOrder = -1;
	miHighestClassOrder = -1;
	miHighestTagOrder = -1;
	miParseTime = 0;

	mParser.SetCSSDocumentContext(this);

	vector_init(&mImportedStyleSheets, sizeof(const WEBC_CHAR *));
	mStringTable.Init(349);
}

CSSDocumentContext::~CSSDocumentContext()
{
	Clear();

#if (WEBC_SUPPORT_JSCRIPT)
	SetJSObject(0);
#endif // WEBC_SUPPORT_JSCRIPT
}

void CSSDocumentContext::Destroy(void)
{
	if (miDeleteLock > 0)
	{
		mbDeleted = 1;
	}
	else
	{
		WEBC_DELETE(this);
	}
}

void CSSDocumentContext::Lock(void)
{
	miDeleteLock++;
}

int CSSDocumentContext::UnLock(void)
{
	if (miDeleteLock > 0)
	{
		miDeleteLock--;

		if ((miDeleteLock == 0) && mbDeleted)
		{
			Destroy();
			return (1);
		}
	}

	return (mbDeleted);
}

int CSSDocumentContext::Deleted(void)
{
	return (mbDeleted);
}

void CSSDocumentContext::Clear(void)
{
	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"CSS::mPropertyTable.Clear", " ")
	mPropertyTable.Clear();
	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"CSS::mPropertyTable.Clear", " ")

	miHighestIdOrder = -1;
	miHighestClassOrder = -1;
	miHighestTagOrder = -1;
	miParseTime = 0;

	mStringTable.Clear();
	CleanUp();

	vector_delete(&mImportedStyleSheets);
}

void CSSDocumentContext::CleanUp(void)
{
	mParser.CleanUp();
}

CSSValueType CSSDocumentContext::GetProperty(CSSStyleElement * element, CSSPropertyType type, CSSPropertyValue * value)
{

	CSSPropertySearch search;
	CSSSelectorPropertyHash *list;

	PRINTF3("Element %s asking for %s ", element->Id(), gpCSSPropertyNames[type])

	search.element = element;
	search.bestValue = 0;
	search.bestOrder = -1;

	// look for inline style
	if (element)
	{
		CSSElementSelectorHash key(element, type);
		list = (CSSSelectorPropertyHash *) mPropertyTable.Find(&key);
		if (list)
		{
			list->getMostSpecificValue(&search);
			if (search.bestOrder >= 0)
			{
				PRINTF1("(success)\n")
				if (search.bestValue)
				{
					*value = *search.bestValue;
					return (CSS_VALUE_SPECIFIED);
				}
				else
				{
					return (CSS_VALUE_INHERIT);
				}
			}
		}
	}
	else
	{
		PRINTF1("(failure)\n")
		return (CSS_VALUE_UNKNOWN);
	}

	// Try to look up property by id
	if (element->Id() && (miHighestIdOrder > search.bestOrder))
	{
		CSSIdSelectorHashLite key(element->IdLowercase(), type);
		list = (CSSSelectorPropertyHash *) mPropertyTable.Find(&key);
		if (list)
		{
			list->getMostSpecificValue(&search);
		}
	}

	// Try to look up property by class
	if (miHighestClassOrder > search.bestOrder)
	{
		WEBC_CHAR * elemClass = 0;
		Enumerator *e = element->GetClasses();
		if (e)
		{
			elemClass = (WEBC_CHAR *) e->Next();
			while (elemClass && (miHighestClassOrder > search.bestOrder))
			{
				CSSClassSelectorHashLite key(elemClass, type);
				list = (CSSSelectorPropertyHash *) mPropertyTable.Find(&key);
				if (list)
				{
					list->getMostSpecificValue(&search);
				}

				elemClass = (WEBC_CHAR *) e->Next();
			}
			e->Dispose();
		}
	}

	// Try to look up property by tag
	if (miHighestTagOrder > search.bestOrder)
	{
		CSSTagSelectorHash key(element->GetTag(), type);
		list = (CSSSelectorPropertyHash *) mPropertyTable.Find(&key);
		if (list)
		{
			list->getMostSpecificValue(&search);
		}
	}

	if (search.bestOrder >= 0)
	{
		PRINTF1("(success)\n")
		if (search.bestValue)
		{
			*value = *search.bestValue;
			return (CSS_VALUE_SPECIFIED);
		}
		else
		{
			return (CSS_VALUE_INHERIT);
		}
	}
	else
	{
		PRINTF1("(failure)\n")
		return (CSS_VALUE_UNKNOWN);
	}
}

int CSSDocumentContext::ParseElementProperty(CSSStyleElement* element, CSSPropertyType type, WEBC_CHAR* value, URLDescriptor* sheetUrl)
{
	CSSRule rule;
	mParser.ParsePropertyValue(type, value, &rule, CSS_ORIGIN_INTERNAL, sheetUrl, 0 /* important */);
	return (AddElementStyle(element, &rule, sheetUrl));
}

int CSSDocumentContext::ParseElementStyle(CSSStyleElement* element, WEBC_CHAR* string, URLDescriptor* sheetUrl)
{
	CSSRule rule;
	mParser.ParseDeclarations(string, &rule, CSS_ORIGIN_INTERNAL, sheetUrl);
	return (AddElementStyle(element, &rule, sheetUrl));
}

int CSSDocumentContext::AddElementStyle(CSSStyleElement* element, CSSRule* rule, URLDescriptor* sheetUrl)
{
	vector_iterator propertyIterator;

	if (rule->enumerateFirstProperty(&propertyIterator) != 0)
	{
		CSSProperty * p = rule->enumerateFirstProperty(&propertyIterator);
		CSSSelectorPropertyHash *list;
		long cascadeOrder = 0x7fffffff;
		CSSStyleElementHash masterKey(element);
		CSSStyleElementHash* masterList;

		// the master element list provides an easy way to find all style properties
		//  in the hash table related to a specific style element
		masterList = (CSSStyleElementHash*) mPropertyTable.Find(&masterKey);
		if (!masterList)
		{
			WEBC_NEW_VERBOSE(masterList, CSSStyleElementHash(element),"cssMasterList");
			if (masterList)
			{
				if (mPropertyTable.Insert(masterList) != HASH_SUCCESS)
				{
					WEBC_DELETE(masterList);
					masterList = 0;
					// it is nice to have the master list, but it is not necessary.
				}
			}
		}

		while (p)
		{
			CSSElementSelectorHash key(element, p->type);

			list = (CSSSelectorPropertyHash *) mPropertyTable.Find(&key);
			if (!list && !(p->flags & CSS_PROPERTY_FLAG_NO_VALUE))
			{
				WEBC_NEW_VERBOSE(list, CSSElementSelectorHash(element, p->type),"CSSHashEntry");
				if (list)
				{
					if (mPropertyTable.Insert(list) != HASH_SUCCESS)
					{
						WEBC_DELETE(list);
						list = 0;
					}
					else
					{
						if (masterList)
						{
							masterList->addElementProperty((CSSElementSelectorHash*) list);
						}
					}
				}
			}

			if (list)
			{
				if (p->flags & CSS_PROPERTY_FLAG_NO_VALUE)
				{
					list->removeSelectorValuePair(0, cascadeOrder);
					/*
					if (list->isEmpty())
					{
						// remove this list from the hash table?
					}
					*/
				}
				else
				{
					if (p->flags & CSS_PROPERTY_FLAG_INHERIT)
					{
						list->addSelectorValuePair(0, 0, cascadeOrder);
					}
					else
					{
						list->addSelectorValuePair(0, &p->value, cascadeOrder);
					}
				}
			}

			p = rule->enumerateNextProperty(&propertyIterator);
		}
	}

	// return failure
	return (-1);
}

void CSSDocumentContext::RemoveElementStyle(CSSStyleElement* element)
{
	CSSStyleElementHash masterKey(element);
	CSSStyleElementHash* masterList;

	masterList = (CSSStyleElementHash*) mPropertyTable.Find(&masterKey);
	if (masterList)
	{
		CSSStyleElementHash* property = masterList->next;
		CSSStyleElementHash* next;

		while (property != masterList)
		{
			next = property->next;

			mPropertyTable.Remove(property);
			WEBC_DELETE(property);

			property = next;
		}

		mPropertyTable.Remove(masterList);
		WEBC_DELETE(masterList);
	}
}


int CSSDocumentContext::AddRule(CSSRule * rule, CSSOrigin origin)
{
	vector_iterator propertyIterator, selectorIterator;
	CSSProperty* p = rule->enumerateFirstProperty(&propertyIterator);
	CSSSelector* sel = rule->enumerateFirstSelector(&selectorIterator);
	CSSSelectorPropertyHash* list;
	long cascadeOrder;


	if (!p || !sel)
	{
		return (-1);
	}
	while (sel)
	{
    // new - save a local copy of id and class name
	const WEBC_CHAR * id = sel->getID();
	const WEBC_CHAR * className = sel->getClass();
	unsigned long Specificity;

		if (p)
			Specificity = sel->getSpecificity();
		while (p)
		{
			cascadeOrder = CSS_CASCADE_ORDER((p->flags & CSS_PROPERTY_FLAG_IMPORTANT) != 0, origin, Specificity, miParseTime);
			miParseTime++;

			if (id)
			{
				CSSIdSelectorHash key(id, p->type);
				list = (CSSSelectorPropertyHash *) mPropertyTable.Find(&key);
				if (!list && !(p->flags & CSS_PROPERTY_FLAG_NO_VALUE))
				{
					WEBC_NEW_VERBOSE(list, CSSIdSelectorHash(id, p->type),"CSSSelectorPropertyHash");
					if (list)
					{
						if (mPropertyTable.Insert(list) != HASH_SUCCESS)
						{
							WEBC_DELETE(list);
							list = 0;
						}
					}
				}

				miHighestIdOrder = EBSMAX(miHighestIdOrder, cascadeOrder);
			}
			else if (className)
			{
				CSSClassSelectorHash key(className, p->type);
				list = (CSSSelectorPropertyHash *) mPropertyTable.Find(&key);
				if (!list && !(p->flags & CSS_PROPERTY_FLAG_NO_VALUE))
				{
					WEBC_NEW_VERBOSE(list, CSSClassSelectorHash(className, p->type),"CSSSelectorPropertyHash");
					if (list)
					{
						if (mPropertyTable.Insert(list) != HASH_SUCCESS)
						{
							WEBC_DELETE(list);
							list = 0;
						}
					}
				}

				miHighestClassOrder = EBSMAX(miHighestClassOrder, cascadeOrder);
			}
			else if (sel->getTag() != HTML_UNKNOWN_TAG) // tag
			{
				CSSTagSelectorHash key(sel->getTag(), p->type);
				list = (CSSSelectorPropertyHash *) mPropertyTable.Find(&key);

				if (!list && !(p->flags & CSS_PROPERTY_FLAG_NO_VALUE))
				{
					WEBC_NEW_VERBOSE(list, CSSTagSelectorHash(sel->getTag(), p->type),"CSSSelectorPropertyHash");
					if (list)
					{
						if (mPropertyTable.Insert(list) != HASH_SUCCESS)
						{
							WEBC_DELETE(list);
							list = 0;
						}
					}
				}

				miHighestTagOrder = EBSMAX(miHighestTagOrder, cascadeOrder);
			}
			else
			{
				list = 0;
			}

			if (list)
			{
				if (p->flags & CSS_PROPERTY_FLAG_NO_VALUE)
				{
					list->removeSelectorValuePair(sel, cascadeOrder);
					/*
					if (list->isEmpty())
					{
						// remove this list from the hash table?
					}
					*/
				}
				else
				{
					if (p->flags & CSS_PROPERTY_FLAG_INHERIT)
					{
						list->addSelectorValuePair(sel, 0, cascadeOrder);
					}
					else
					{
						list->addSelectorValuePair(sel, &p->value, cascadeOrder);
					}
				}
			}

			p = rule->enumerateNextProperty(&propertyIterator);
		}

		p = rule->enumerateFirstProperty(&propertyIterator);
		sel = rule->enumerateNextSelector(&selectorIterator);
	}
	return (0);
}

int CSSDocumentContext::ParseStyleSheet (
		WEBC_CHAR* string,
		CSSOrigin origin,
		URLDescriptor* sheetUrl,
		unsigned lastChunk
	)
{
	HTMLDocument* doc = mDocument.Get();
	if (doc)
	{
		mParser.SetCSSDocumentContext(this);
		mParser.ParseStyleSheet(string, origin, sheetUrl, lastChunk);

		doc->StyleSheetUpdate();
	}
	return 0;
}

#if (WEBC_SUPPORT_JSCRIPT)
JSObject *CSSDocumentContext::GetJSObject()
{
	return mJSObject.Get();
}

void CSSDocumentContext::SetJSObject(JSObject* obj)
{
	mJSObject.SetPrivate(0);
	mJSObject.Set(obj);
	if (obj)
	{
		mJSObject.SetPrivate(this);
	}
}
#endif

void CSSDocumentContext::Import(const WEBC_CHAR *url, URLDescriptor *baseUrl)
{
	WEBC_CHAR *urlCopy = mStringTable.GetString(url);

	if (!urlCopy)
	{
		return;
	}

	vector_iterator vi;
	const WEBC_CHAR **importedUrl = (const WEBC_CHAR **) vector_get_first(&mImportedStyleSheets, &vi);
	while (importedUrl)
	{
		if (*importedUrl == urlCopy)
		{
			// we've already imported this sheet; don't do it again
			mStringTable.ReleaseString(url);
			return;
		}

		importedUrl = (const WEBC_CHAR **) vector_get_next(&vi);
	}

	if (vector_add_node_to_front(&mImportedStyleSheets, (WEBC_PFBYTE) &urlCopy) < 0)
	{
		mStringTable.ReleaseString(urlCopy);
		return;
	}

	if (!baseUrl)
	{
		baseUrl = GetHTMLDocument()->BaseURL();
	}

	URLDescriptor urlDesc(urlCopy, baseUrl);

	CSSLoadExternalSheet * loadJob;
	WEBC_NEW_VERBOSE(loadJob, CSSLoadExternalSheet(this, &urlDesc, GetHTMLDocument()->GetCharset()),"loadJob");
	if (!loadJob)
	{
		return;
	}

	HTMLBrowser* browser = GetHTMLDocument()->GetBrowser();
	WEBC_ASSERT(browser);
	browser->ProcessLoadJob(loadJob, &urlDesc, baseUrl, GetHTMLDocument());

  #ifdef WEBC_OLD_LOAD_METHOD
	webc_thread_load_browser(GetHTMLDocument()->GetBrowser());
  #endif // WEBC_OLD_LOAD_METHOD
}

CSSSelector *CSSDocumentContext::newSelector (const WEBC_CHAR *str)
{
	CSSSelector *sel;

	WEBC_NEW_VERBOSE(sel, CSSSelector(this, str),"NewSelector");

	if (sel)
	{
		while (sel->getChild())
		{
			sel = sel->getChild();
		}

		CSSSelector *old = (CSSSelector *) mSelectorTable.Find(sel);
		if (old)
		{
			WEBC_DELETE(sel);
			sel = old;
		}
		else
		{
			if (mSelectorTable.Insert(sel) != HASH_SUCCESS)
			{
				WEBC_DELETE(sel);
				sel = 0;
			}
		}
	}

	return (sel);
}

void CSSDocumentContext::deleteSelector (CSSSelector *sel)
{
	sel = (CSSSelector *) mSelectorTable.Remove(sel);
	WEBC_DELETE(sel);
}


/*
  CSSDocumentParser:
  used by CSSDocumentContext to parse in a CSS style sheet

*/


CSSDocumentParser::CSSDocumentParser()
{
	mDocumentContext = WEBC_NULL;
	mInComment = 0;
	mStarSeen = 0;
}

CSSDocumentParser::~CSSDocumentParser()
{
	CleanUp();
}

void CSSDocumentParser::CleanUp(void)
{
	mParseBuffer.freeBuffer();
	mInComment = 0;
	mStarSeen = 0;
}

int CSSDocumentParser::ParseStyleSheet (
		WEBC_CHAR * string,
		CSSOrigin origin,
		URLDescriptor *sheetUrl,
		unsigned lastChunk
	)
	{
	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
	WEBC_DEBUG_LOG_TIME_CLEAR_ACCUMULATOR(TimeInDeclarations)
	WEBC_DEBUG_LOG_TIME_CLEAR_ACCUMULATOR(TimeInSelectors)
	WEBC_DEBUG_LOG_TIME_CLEAR_ACCUMULATOR(TimeInAddRule)


	if (!mDocumentContext || mDocumentContext->Deleted())
	{
		return (-1);
	}

	// handle the case where "*/" ending a comment stradles two buffers
	if (mInComment && mStarSeen && string[0] == '/')
	{
		mInComment = 0;
		mStarSeen = 0;
		string++;
	}

	long stringLength = webc_strlen(string);

	if (!lastChunk && stringLength == 0)
	{
		// nothing to parse!
		return (0);
	}

	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"ParseStyleSheet", "Phase one")
	if (stringLength > 0)
	{
		if (mParseBuffer.append(string, stringLength) < 0)
		{
			// probably a malloc failure
			return (-1);
		}
	}

	/*---------------------------------------------------------------*/
	// BEGIN - Preprocessing
	long length = 0;
	long parseEnd = 0;
	long singleQuoteCount = 0;
	long doubleQuoteCount = 0;
	long braceNesting = 0;
	unsigned escapeChar = 0;
	WEBC_CHAR* strIn = mParseBuffer.getChars();
	WEBC_CHAR* strOut = strIn;

	while (*strIn)
	{
		if (mInComment)
		{
			if (mStarSeen && *strIn == '/')
			{
				mInComment = 0;
				mStarSeen = 0;
			}
			else if (*strIn == '*')
			{
				mStarSeen = 1;
			}
			else
			{
				mStarSeen = 0;
			}
		}
		else
		{
			if (webc_c_strncmp(strIn, "/*",2) == 0)
			{
				mInComment = 1;
				mStarSeen = 0;
				strIn += 2;
				continue;
			}

			if (webc_c_strncmp(strIn, "<!--", 4) == 0)
			{
				strIn += 4;
				continue;
			}
			if (webc_c_strncmp(strIn, "-->", 3) == 0)
			{
				strIn += 3;
				continue;
			}

			if (lastChunk)
			{
				// if this is the last chunk of the style sheet, don't be so picky about
				//  how far to parse
				if (escapeChar)
				{
					escapeChar = 0;
				}
				else
				{
					if (*strIn == '\\')
					{
						escapeChar = 1;
					}
					else if (*strIn == '\'')
					{
						singleQuoteCount++;
					}
					else if (*strIn == '\"')
					{
						doubleQuoteCount++;
					}
					// this seems to be what Mozilla does
					else if ((*strIn == '}' || *strIn == ';') && !(singleQuoteCount & 1) && !(doubleQuoteCount & 1))
					{
						parseEnd = length + 1;
					}
				}
			}
			else
			{
				if (*strIn == '{')
				{
					braceNesting++;
				}
				else if (*strIn == '}')
				{
					if (braceNesting > 0)
					{
						braceNesting--;
					}

					if (braceNesting == 0)
					{
						parseEnd = length + 1;
					}
				}
				else if (braceNesting == 0 && *strIn == ';')
				{
					parseEnd = length + 1;
				}
			}

			strOut[length++] = *strIn;
		}

		strIn++;
	}
	/* If the chunk ended in the middle of a comment, add a start comment tag
       to strOut so that it properly closes the comment when the next half
       of the comment arrives */

    if (mInComment == 1)
    {
        strOut[length++] = '/';
        strOut[length++] = '*';
        mInComment = 0;
    }
	mParseBuffer.truncate(length);

	WEBC_CHAR* parseUpToHere = strOut + parseEnd;
	WEBC_CHAR save = *parseUpToHere;
	*parseUpToHere = '\0';

	// END - Preprocessing
	/*---------------------------------------------------------------*/

	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"ParseStyleSheet", "Phase one")

	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"ParseStyleSheet", "Phase Two")
	/*---------------------------------------------------------------*/
	// BEGIN - Parsing
	WEBC_CHAR* cpos = strOut;
	CSSRule rule;
	while (*cpos && cpos < parseUpToHere)
	{
		while (IS_WHITESPACE(cpos[0]))
		{
			cpos++;
		}

		// parse '@' directive
		if (cpos[0] == '@')
		{
			if (!webc_c_strnicmp(cpos + 1, "import", 6))
			{
				long result;
				cpos += 8;

				result = parseImport(cpos, sheetUrl);
				if (result < 0)
				{
					return (-1);
				}
				cpos += result;
			}
			else if (!webc_c_strnicmp(cpos + 1, "media", 5) && validateMediaType(cpos + 7))
			{
				cpos += 7;
				while (*cpos && *cpos != '{')
				{
					cpos++;
				}

				if (*cpos == '{')
				{
					cpos++;
				}
			}
			else
			{
				WEBC_CHAR* beginCpos = cpos;

				// step past '@'
				cpos++;

				// some unrecognized '@' directive; scan forward to the next ';' or '{...}'
				while (*cpos && *cpos != '{' && *cpos != ';')
				{
					cpos++;
				}

				if (*cpos == '{')
				{
					int unknownBraceNesting = 1;

					cpos++;
					while (*cpos)
					{
						if (*cpos == '}')
						{
							unknownBraceNesting--;
							if (unknownBraceNesting == 0)
							{
								break;
							}
						}
						else if (*cpos == '{')
						{
							unknownBraceNesting++;
						}
						cpos++;
					}

					if (unknownBraceNesting > 0 || !(*cpos))
					{
						cpos = beginCpos;
						break; // out of the while loop
					}
					else
					{
						if (*cpos == '}')
						{
							cpos++;
						}
					}
				}
				else if (*cpos == ';')
				{
					cpos++;
				}
			}
		}
		// step past anything else that is not an '@' directive or a selector (i.e. a rule)
		else if (!IS_SELECTOR_CHAR(cpos[0]))
		{
			cpos++;
			// skip until the next bit of whitespace
			while (*cpos && !IS_WHITESPACE(*cpos))
			{
				cpos++;
			}
		}
		else
		{
			WEBC_DEBUG_LOG_TIME_HANDLE(TimeIn)

			WEBC_CHAR* beginCpos = cpos;
			cpos = ParseChunk(cpos, &rule, origin, sheetUrl);
			WEBC_DEBUG_LOG_TIME_ACCUMULATE_BEGIN(TimeIn)
	  		mDocumentContext->AddRule(&rule, origin);
			rule.clear();
			WEBC_DEBUG_LOG_TIME_ACCUMULATE(TimeIn,TimeInAddRule)

			if (cpos == beginCpos)
			{
				// make sure we are moving forward
				break;
			}
		}
	}

	WEBC_DEBUG_LOG_TIME_PRINT_ACCUMULATOR("In Declarations",TimeInDeclarations)
	WEBC_DEBUG_LOG_TIME_PRINT_ACCUMULATOR("In Selectors",TimeInSelectors)
	WEBC_DEBUG_LOG_TIME_PRINT_ACCUMULATOR("In Addrule",TimeInAddRule)
	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"ParseStyleSheet", "Phase Two")

	// END - Parsing
	/*---------------------------------------------------------------*/

	*parseUpToHere = save;

	long numCharsParsed = cpos - strOut;
	mParseBuffer.discardPrefix(numCharsParsed);

	return 0;
}

long CSSDocumentParser::parseImport(WEBC_CHAR* import, URLDescriptor* sheetUrl)
{
	int n;
	WEBC_CHAR* beginPos = import;
	WEBC_CHAR endChar;
	WEBC_CHAR url[256];

	while (IS_WHITESPACE(*import))
	{
		import++;
	}

	endChar = ';';

	if (UPPERCASE(import[0]) == 'U' && UPPERCASE(import[1]) == 'R' &&
	    UPPERCASE(import[2]) == 'L' && UPPERCASE(import[3]) == '(')
	{
		endChar = ')';

		import += 4;                    // skip over the URL(
		while (IS_WHITESPACE(*import))  // ...and the whitespace after it
		{
			import++;
		}
	}

	if (*import == '\'' || *import == '"')
	{
		endChar = *import;
		import++;
	}

	n = 0;
	while (*import != endChar && *import)
	{
		// Parentheses, commas, whitespace characters, single quotes (') and double quotes (")
		//  appearing in a URL must be escaped with a backslash: '\(', '\)', '\,'.
		if ((import[0] == '\\') &&
		    ((import[1] == '(')  || (import[1] == ')') || (import[1] == ',') ||
		     (import[1] == '\'') || (import[1] == '"')))
		{
			url[n] = import[1];
			import += 2;
		}
		else
		{
			url[n] = import[0];
			import++;
		}

		n++;
	}
	url[n] = 0;

	mDocumentContext->Lock();
	mDocumentContext->Import(url, sheetUrl);
	if (mDocumentContext->UnLock()) // non-zero means we've been deleted
	{
		return (-1);
	}

	while (*import && *import != ';')
	{
		import++;
	}

	return (import - beginPos);
}

unsigned CSSDocumentParser::validateMediaType(WEBC_CHAR* cpos)
{
	unsigned mediaTypeValidated = 0;

	while (!mediaTypeValidated && cpos[0])
	{
		while (IS_WHITESPACE(cpos[0]))
		{
			cpos++;
		}

		if (cpos[0] == '{')
		{
			break;
		}

		if (!webc_c_strnicmp(cpos, "all", 3))
		{
			mediaTypeValidated = 1;
		}
		else if (!webc_c_strnicmp(cpos, "screen", 3))
		{
			mediaTypeValidated = 1;
		}
		else if (!webc_c_strnicmp(cpos, "handheld", 3))
		{
			mediaTypeValidated = 1;
		}

		while (cpos[0] && cpos[0] != ',' && cpos[0] != '{')
		{
			cpos++;
		}

		if (cpos[0] == ',')
		{
			cpos++;
		}
	}

	return (mediaTypeValidated);
}


WEBC_CHAR * CSSDocumentParser::ParseChunk(WEBC_CHAR * string, CSSRule * rule, CSSOrigin origin, URLDescriptor *sheetUrl)
{
	if (string && webc_strlen(string) > 0)
	{
		WEBC_DEBUG_LOG_TIME_HANDLE(TimeIn)
		WEBC_CHAR * pos = string;

		WEBC_DEBUG_LOG_TIME_ACCUMULATE_BEGIN(TimeIn)
		pos = ParseSelectors(pos, rule);
		WEBC_DEBUG_LOG_TIME_ACCUMULATE(TimeIn,TimeInSelectors)
		WEBC_DEBUG_LOG_TIME_ACCUMULATE_BEGIN(TimeIn)
		pos = ParseDeclarations(pos, rule, origin, sheetUrl);
		WEBC_DEBUG_LOG_TIME_ACCUMULATE(TimeIn,TimeInDeclarations)
		return pos;
	}
	else
	{
		return WEBC_NULL;
	}
}


WEBC_CHAR * CSSDocumentParser::ParseDeclarations(WEBC_CHAR * string, CSSRule * rule, CSSOrigin origin, URLDescriptor *sheetUrl)
{
	WEBC_CHAR * pos = string;

	while (*pos && (!rtp_isalnum(*pos)) && (*pos != '{'))					// eat the { and leading white space
	{
		pos++;
	}

	if (*pos == '{')
	{
		pos++;
	}

	if (!(*pos))
	{
		PRINTF2("ParseDeclarations:  Passed bad string? (%s)\n", string)
		return pos;
	}

	do
	{
		pos = ParseSingleDeclaration(pos, rule, origin, sheetUrl);
		if (*pos == ';')
		{
			pos++;
		}
	} while (*pos && *pos != '}' && *pos != '{');

	if (*pos == '}' || *pos == '{')
	{
		pos++;
	}

	return pos;
}


WEBC_CHAR *CSSDocumentParser::ParseSingleDeclaration(WEBC_CHAR * string, CSSRule * rule, CSSOrigin origin, URLDescriptor *sheetUrl)
{
	int n;
	WEBC_CHAR propertyName[32];
	WEBC_CHAR _propertyValue[128];
	WEBC_CHAR* propertyValue = _propertyValue;

	propertyName[0] = propertyValue[0] = '\0';
	WEBC_CHAR * p = string;
	while (IS_WHITESPACE(*p) && *p)
	{
		p++;				// eat white space
	}

	if (*p == '}' || *p == '{')
	{
		return (p);
	}
	// read the property name
	for (n=0; n<31 && (IS_DIGIT(p[n]) || IS_ALPHA(p[n]) || (p[n] == '!') || (p[n] == '-')); n++)
	{
		propertyName[n] = p[n];
	}
	propertyName[n] = 0;

	while (*p && *p != ':')
	{
		p++;				// eat stuff until the :
	}

	if (!*p)
	{
		return (p);
	}

	p++;
	while (IS_WHITESPACE(*p))
	{
		p++; // eat more white space
	}

	WEBC_CHAR insideQuote = 0;

	// read the property value
	for (n=0; n<127; n++)
	{
		if (*p == '\\' && p[1])
		{
			p++;
			propertyValue[n] = *p;
		}
		else if (IS_VALUE_CHAR(*p) || insideQuote)
		{
			propertyValue[n] = *p;
			if (insideQuote)
			{
				if (*p == insideQuote)
				{
					insideQuote = 0;
				}
			}
			else
			{
				if (*p == '\'' || *p == '\"')
				{
					insideQuote = *p;
				}
			}
		}
		else
		{
			break;
		}
		p++;
	}

	while (n > 0 && IS_WHITESPACE(propertyValue[n-1]))
	{
		n--;
	}

	propertyValue[n] = 0;

	if ((propertyValue[0] == '\'' && propertyValue[n-1] == '\'') ||
	    (propertyValue[0] == '\"' && propertyValue[n-1] == '\"'))
	{
		propertyValue[n-1] = 0;
		propertyValue++;
	}

	PRINTF3("Property \"%s\" has value \"%s\"\n", propertyName, propertyValue)

	ParseDeclaration(propertyName, propertyValue, rule, origin, sheetUrl);
	return (p);
}


int CSSDocumentParser::ParseBorderColor(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important)
{
	WEBC_CHAR bucket[4][32];
	CSSPropertyValue propVal[4];

	int numRead = _bucket_scan(string, (WEBC_CHAR *) bucket, 4, 32);
	if (numRead < 1)
		return -1;
	switch (numRead)
	{
		case 1:
			HTML_ParseColor(bucket[0], 0, &propVal[0].color);
			propVal[1] = propVal[2] = propVal[3] = propVal[0];
			break;

		case 2:
			HTML_ParseColor(bucket[0], 0, &propVal[0].color);
			propVal[2] = propVal[0];
			HTML_ParseColor(bucket[1], 0, &propVal[1].color);
			propVal[3] = propVal[1];
			break;

		case 3:
			HTML_ParseColor(bucket[0], 0, &propVal[0].color);
			HTML_ParseColor(bucket[1], 0, &propVal[1].color);
			propVal[3] = propVal[1];
			HTML_ParseColor(bucket[2], 0, &propVal[2].color);
			break;

		case 4:
			HTML_ParseColor(bucket[0], 0, &propVal[0].color);
			HTML_ParseColor(bucket[1], 0, &propVal[1].color);
			HTML_ParseColor(bucket[2], 0, &propVal[2].color);
			HTML_ParseColor(bucket[3], 0, &propVal[3].color);
			break;
	}
	CSSProperty p;
	p.flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);

	p.type = CSS_PROPERTY_BORDER_TOP_COLOR;
	p.value = propVal[0];
	rule->addProperty(p);

	p.type = CSS_PROPERTY_BORDER_RIGHT_COLOR;
	p.value = propVal[1];
	rule->addProperty(p);

	p.type = CSS_PROPERTY_BORDER_BOTTOM_COLOR;
	p.value = propVal[2];
	rule->addProperty(p);

	p.type = CSS_PROPERTY_BORDER_LEFT_COLOR;
	p.value = propVal[3];
	rule->addProperty(p);

	return 0;
}

int CSSDocumentParser::ParseMargin(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important)
{

	WEBC_CHAR bucket[4][16];
	CSSPropertyValue val[4];

	int numRead = _bucket_scan(string, (WEBC_CHAR *) bucket, 4, 16);
	if (numRead == 0)
	{
		return -1;
	}
	if (numRead == 4)
	{
		if (GetLength(bucket[3], &val[3].length) == -1)
		{
			numRead--;
		}
	}
	if (numRead >= 3)
	{
		if (GetLength(bucket[2], &val[2].length) == -1)
		{
			numRead--;
		}
	}
	if (numRead >= 2)
	{
		if (GetLength(bucket[1], &val[1].length) == -1)
		{
			numRead--;
		}
	}
	if (numRead >= 1)
	{
		if (GetLength(bucket[0], &val[0].length) == -1)
		{
			numRead--;
		}
	}
	CSSProperty p;
	p.flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);

	switch (numRead)
	{
		case 1:
			p.type = CSS_PROPERTY_MARGIN_TOP; p.value = val[0]; rule->addProperty(p);
			p.type = CSS_PROPERTY_MARGIN_LEFT; p.value = val[0]; rule->addProperty(p);
			p.type = CSS_PROPERTY_MARGIN_BOTTOM; p.value = val[0]; rule->addProperty(p);
			p.type = CSS_PROPERTY_MARGIN_RIGHT; p.value = val[0]; rule->addProperty(p);
			break;

		case 2:
			p.type = CSS_PROPERTY_MARGIN_TOP; p.value = val[0]; rule->addProperty(p);
			p.type = CSS_PROPERTY_MARGIN_BOTTOM; p.value = val[0]; rule->addProperty(p);
			p.type = CSS_PROPERTY_MARGIN_LEFT; p.value = val[1]; rule->addProperty(p);
			p.type = CSS_PROPERTY_MARGIN_RIGHT; p.value = val[1]; rule->addProperty(p);
			break;

		case 3:
			p.type = CSS_PROPERTY_MARGIN_TOP; p.value = val[0]; rule->addProperty(p);
			p.type = CSS_PROPERTY_MARGIN_RIGHT; p.value = val[1]; rule->addProperty(p);
			p.type = CSS_PROPERTY_MARGIN_BOTTOM; p.value = val[2]; rule->addProperty(p);
			p.type = CSS_PROPERTY_MARGIN_LEFT; p.value = val[1]; rule->addProperty(p);
			break;

		case 4:
			p.type = CSS_PROPERTY_MARGIN_TOP; p.value = val[0]; rule->addProperty(p);
			p.type = CSS_PROPERTY_MARGIN_RIGHT; p.value = val[1]; rule->addProperty(p);
			p.type = CSS_PROPERTY_MARGIN_BOTTOM; p.value = val[2]; rule->addProperty(p);
			p.type = CSS_PROPERTY_MARGIN_LEFT; p.value = val[3]; rule->addProperty(p);
			break;

		default:
			return -1;
	}

	return 0;

}

int CSSDocumentParser::ParseSingleBorder(WEBC_CHAR * string, CSSRule * rule, CSSPropertyType border, WEBC_UINT8 important)
{

	WEBC_CHAR bucket[3][32];
	CSSPropertyValue value;

	int numRead = _bucket_scan(string, (WEBC_CHAR *) bucket, 3, 32);
	CSSPropertyType widthEnum, colorEnum, styleEnum;
	switch (border)
	{
		case CSS_PROPERTY_BORDER_TOP:
			widthEnum = CSS_PROPERTY_BORDER_TOP_WIDTH;
			colorEnum = CSS_PROPERTY_BORDER_TOP_COLOR;
			styleEnum = CSS_PROPERTY_BORDER_TOP_STYLE;
			break;

		case CSS_PROPERTY_BORDER_LEFT:
			widthEnum = CSS_PROPERTY_BORDER_LEFT_WIDTH;
			colorEnum = CSS_PROPERTY_BORDER_LEFT_COLOR;
			styleEnum = CSS_PROPERTY_BORDER_LEFT_STYLE;
			break;

		case CSS_PROPERTY_BORDER_BOTTOM:
			widthEnum = CSS_PROPERTY_BORDER_BOTTOM_WIDTH;
			colorEnum = CSS_PROPERTY_BORDER_BOTTOM_COLOR;
			styleEnum = CSS_PROPERTY_BORDER_BOTTOM_STYLE;
			break;

		case CSS_PROPERTY_BORDER_RIGHT:
			widthEnum = CSS_PROPERTY_BORDER_RIGHT_WIDTH;
			colorEnum = CSS_PROPERTY_BORDER_RIGHT_COLOR;
			styleEnum = CSS_PROPERTY_BORDER_RIGHT_STYLE;
			break;

		default:
			PRINTF2("CSSDocumentParser::ParseSingleBorder(...): Weird border (%d)\n", (int) border)
			break;
	}
	CSSProperty p;
	p.flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);
	for (int t = 0; t < numRead; t++)
	{
		if (ParseSingleBorderWidth(bucket[t], &value) == 0)
		{
			p.type = widthEnum;
			p.value = value;
			rule->addProperty(p);
		}
		else if (ParseColor(bucket[t], &value) == 0)
		{
			p.type = colorEnum;
			p.value = value;
			rule->addProperty(p);
		}
		else if (ParseSingleBorderStyle(bucket[t], &value) == 0)
		{
			p.type = styleEnum;
			p.value = value;
			rule->addProperty(p);
		}

	}

	return 0;
}

int CSSDocumentParser::ParseOutline(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important)
{
	WEBC_CHAR bucket[3][32];
	CSSPropertyValue value;
	CSSProperty p;

	int numRead = _bucket_scan(string, (WEBC_CHAR *) bucket, 3, 32);

	p.flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);
	for (int t = 0; t < numRead; t++)
	{
		if (ParseSingleBorderWidth(bucket[t], &value) == 0)
		{
			p.type = CSS_PROPERTY_OUTLINE_WIDTH;
			p.value = value;
			rule->addProperty(p);
		}
		else if (ParseColor(bucket[t], &value) == 0)
		{
			p.type = CSS_PROPERTY_OUTLINE_COLOR;
			p.value = value;
			rule->addProperty(p);
		}
		else if (ParseSingleBorderStyle(bucket[t], &value) == 0)
		{
			p.type = CSS_PROPERTY_OUTLINE_STYLE;
			p.value = value;
			rule->addProperty(p);
		}
	}
	return 0;
}

int CSSDocumentParser::ParseListStyle(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important, CSSOrigin origin, URLDescriptor *sheetUrl)
{
	// doesn't set initial values for things that aren't parsed...
	WEBC_CHAR * temp;
	WEBC_CHAR * s[6];
	temp = (WEBC_CHAR *) WEBC_MALLOC((webc_strlen(string) + 1) * sizeof(WEBC_CHAR));
	if (!temp)
	{
		return (-1);
	}

	webc_strcpy(temp, string);
	int numRead = 0;
	WEBC_CHAR * po = temp;

	s[0] = po;
	while (*po)
	{
		if (IS_WHITESPACE(*po))
		{
			if (numRead >= 5)
			{
				break;
			}
			*po = '\0';
			po++;
			while (IS_WHITESPACE(*po))
			{
				po++;
			}
			s[++numRead] = po;
		}
		po++;
	}

	numRead++;

	CSSProperty p;

	p.flags = (important)? CSS_PROPERTY_FLAG_IMPORTANT : 0;

	for (int t = 0; t < numRead; t++)
	{
		if (ParseListStyleType(s[t], &p.value) == 0)
		{
			p.type = CSS_PROPERTY_LIST_STYLE_TYPE;
			rule->addProperty(p);
			PRINTF2("list-style-type is %s\n", s[t])
			continue;
		}

		if (ParseListStylePosition(s[t], &p.value) == 0)
		{
			p.type = CSS_PROPERTY_LIST_STYLE_POSITION;
			rule->addProperty(p);
			PRINTF2("list-style-position is %s\n", s[t])
			continue;
		}

		if (ParseURL(s[t], &p.value, origin, sheetUrl) == 0)
		{
			p.type = CSS_PROPERTY_LIST_STYLE_IMAGE;
			if (rule->addProperty(p) < 0)
			{
				mDocumentContext->GetStringTable()->ReleaseString(p.value.url);
			}
			PRINTF2("list-style-image is %s\n", p.value.url)
			continue;
		}
	}

	WEBC_FREE(temp);

	return 0;
}


int CSSDocumentParser::ParseBackground(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important, CSSOrigin origin, URLDescriptor *sheetUrl)
{
	// doesn't set initial values for things that aren't parsed...
	WEBC_CHAR * temp;
	WEBC_CHAR * s[6];
	temp = (WEBC_CHAR *) WEBC_MALLOC((webc_strlen(string) + 1) * sizeof(WEBC_CHAR));
	if (!temp)
	{
		return (-1);
	}

	webc_strcpy(temp, string);
	int numRead = 0;
	WEBC_CHAR * po = temp;
	CSSPropertyValue val;

	s[0] = po;
	int squotedepth = 0;
	int quotedepth = 0;
	int parendepth = 0;
	// Break arguments into pieces by whitespace unless it should be terminated by a closing quote or parentheses.
	while (*po)
	{
		if (*po == '\'')
 			squotedepth = ~squotedepth;
 		else if (*po == '\"')
 			quotedepth = ~quotedepth;
 		else if (*po == '(')
 			parendepth = 1;
 		else if (*po == ')' && parendepth)
 			parendepth = 0;
		else if (!quotedepth && !squotedepth && !parendepth)
		{
		if (IS_WHITESPACE(*po))
		{
			if (numRead >= 5)
			{
				break;
			}
			*po = '\0';
			po++;
			while (IS_WHITESPACE(*po))
			{
				po++;
			}
			s[++numRead] = po;
		}
		}
		po++;
	}

	WEBC_BOOL urlParsed = WEBC_FALSE;
	WEBC_BOOL colorParsed = WEBC_FALSE;
	WEBC_BOOL positionParsed = WEBC_FALSE;
	WEBC_BOOL attachmentParsed = WEBC_FALSE;
	WEBC_BOOL repeatParsed = WEBC_FALSE;

	WEBC_CHAR* xPosStr = 0;
	WEBC_CHAR* yPosStr = 0;
	CSSPropertyValue xPosVal;
	CSSPropertyValue yPosVal;

	numRead++;
	CSSProperty p;
	p.flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);
	for (int t = 0; t < numRead; t++)
	{
		if (!urlParsed && ParseURL(s[t], &val, origin, sheetUrl) == 0)
		{
			urlParsed = WEBC_TRUE;
			p.type = CSS_PROPERTY_BACKGROUND_IMAGE;
			p.value = val;
			if (rule->addProperty(p) < 0)
			{
				mDocumentContext->GetStringTable()->ReleaseString(val.url);
			}
			PRINTF2("Background URL is %s\n", val.url)
			continue;
		}

		if (!repeatParsed && ParseBackgroundRepeat(s[t], &val) == 0)
		{
			repeatParsed = WEBC_TRUE;
			p.type = CSS_PROPERTY_BACKGROUND_REPEAT;
			p.value = val;
			rule->addProperty(p);
			PRINTF2("Background repeat is %s\n", s[t])
			continue;
		}

		if (!attachmentParsed && ParseBackgroundAttachment(s[t], &val) == 0)
		{
			attachmentParsed = WEBC_TRUE;
			p.type = CSS_PROPERTY_BACKGROUND_ATTACHMENT;
			p.value = val;
			rule->addProperty(p);
			PRINTF2("Background attachment is %s\n", s[t])
			continue;
		}

		if (!colorParsed && ParseColor(s[t], &val) == 0)
		{
			colorParsed = WEBC_TRUE;
			p.type = CSS_PROPERTY_BACKGROUND_COLOR;
			p.value = val;
			rule->addProperty(p);
			PRINTF2("Background color is %s\n", s[t])
			continue;
		}

		if (!positionParsed)
		{
			if (ParseBackgroundPositionX(s[t], &xPosVal) == 0)
			{
				positionParsed = WEBC_TRUE;
				xPosStr = s[t];
			}

			if (ParseBackgroundPositionY(s[t], &yPosVal) == 0)
			{
				positionParsed = WEBC_TRUE;
				yPosStr = s[t];
			}
		}
		else
		{
			if (xPosStr && ParseBackgroundPositionY(s[t], &yPosVal) == 0)
			{
				yPosStr = s[t];
			}
			else
			{
				if (ParseBackgroundPositionX(s[t], &xPosVal) == 0)
				{
					xPosStr = s[t];
				}
				else if (ParseBackgroundPositionY(s[t], &yPosVal) == 0)
				{
					yPosStr = s[t];
				}
			}
		}
	}

	if (xPosStr)
	{
		p.type = CSS_PROPERTY_BACKGROUND_X_POSITION;
		p.value = xPosVal;
		rule->addProperty(p);
	}

	if (yPosStr)
	{
		p.type = CSS_PROPERTY_BACKGROUND_Y_POSITION;
		p.value = yPosVal;
		rule->addProperty(p);
	}

	WEBC_FREE(temp);
	return 0;
}

int CSSDocumentParser::ParseFont(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important)
{
	WEBC_CHAR * s;
	WEBC_CHAR * work;
	work = (WEBC_CHAR *) WEBC_MALLOC((webc_strlen(string) + 1) * sizeof(WEBC_CHAR));
	if (!work)
	{
		return -1;
	}
	webc_strcpy(work, string);
	s = work;
	while (*s != '\0')
	{
		if (IS_WHITESPACE(*s) || *s == '/')
		{
			*s = '\0';
		}
		s++;
	}

	CSSPropertyValue value;

	s = work;
	char styleParsed, variantParsed, weightParsed, sizeParsed, lineHeightParsed;
	styleParsed = variantParsed = weightParsed = sizeParsed = lineHeightParsed = 0;
	CSSProperty p;
	p.flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);
	while ((s - work) < (int)webc_strlen(string))
	{
		if (ParseFontStyle(s, &value) == 0 && !styleParsed)
		{
			p.value = value;
			p.type = CSS_PROPERTY_FONT_STYLE;
			rule->addProperty(p);
			styleParsed = 1;
		}
		else if (ParseFontWeight(s, &value) == 0 && !weightParsed)
		{
			p.value = value;
			p.type = CSS_PROPERTY_FONT_WEIGHT;
			rule->addProperty(p);
			weightParsed = 1;
		}
		else if (ParseFontVariant(s, &value) == 0 && !variantParsed)
		{
			p.value = value;
			p.type = CSS_PROPERTY_FONT_VARIANT;
			rule->addProperty(p);
			variantParsed = 1;
		}
		else if (ParseFontSize(s, &value) == 0 && !sizeParsed)
		{
			p.value = value;
			p.type = CSS_PROPERTY_FONT_SIZE;
			rule->addProperty(p);
			sizeParsed = 1;
		}
		else if (ParseLineHeight(s, &value) == 0 && sizeParsed) 		// only parse a line height if the size was already parsed
		{
			p.value = value;
			p.type = CSS_PROPERTY_LINE_HEIGHT;
			rule->addProperty(p);
			lineHeightParsed = 1;
		}
		else										// everything left must be the family
		{
			int charIn = (s - work);
			s = string + charIn;
			ParseFontFamily(s, &value);
			p.type = CSS_PROPERTY_FONT_FAMILY;
			p.value = value;
			rule->addProperty(p);
			break;															// done
		}
		while (*s != '\0')
		{
			s++;
		}
		while (*s == '\0')
		{
			s++;
		}
	}
	WEBC_FREE(work);

	WEBC_CHAR _normal[8];
	webc_strcpy(_normal, WEBC_STR_NORMAL);

	if (!styleParsed) 								// set initial values for things not parsed
	{
		ParseFontStyle(_normal, &value);
		rule->addProperty(CSS_PROPERTY_FONT_STYLE, value, important);
	}
	if (!weightParsed)
	{
		ParseFontWeight(_normal, &value);
		rule->addProperty(CSS_PROPERTY_FONT_WEIGHT, value, important);
	}
	if (!variantParsed)
	{
		ParseFontWeight(_normal, &value);
		rule->addProperty(CSS_PROPERTY_FONT_VARIANT, value, important);
	}
	return 0;
}

int CSSDocumentParser::ParseBorderStyle(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important)
{
	char * s[] = { "dashed", "dotted", "double", "groove", "inset", "none", "outset", "ridge", "solid" };
	WEBC_CHAR bucket[4][32];
	int style[4];
	// top right bottom left
	int numRead = _bucket_scan(string, (WEBC_CHAR *) bucket, 4, 32);
	style[0] = style[1] = style[2] = style[3] = -1;
	switch (numRead)
	{
		case 0:
			return -1;

		case 1:
			ParseStringIntoEnum((char**)s, bucket[0], 9, (void*)&style[0]);
			style[1] = style[2] = style[3] = style[0];
			break;

		case 2:
			ParseStringIntoEnum((char**)s, bucket[0], 9, (void*)&style[0]);
			style[2] = style[0];
			ParseStringIntoEnum((char**)s, bucket[1], 9, (void*)&style[1]);
			style[3] = style[1];
			break;

		case 3:
			ParseStringIntoEnum((char**)s, bucket[0], 9, (void*)&style[0]);
			ParseStringIntoEnum((char**)s, bucket[1], 9, (void*)&style[1]);
			style[3] = style[1];
			ParseStringIntoEnum((char**)s, bucket[2], 9, (void*)&style[2]);
			break;

		case 4:
			ParseStringIntoEnum((char**)s, bucket[0], 9, (void*)&style[0]);
			ParseStringIntoEnum((char**)s, bucket[1], 9, (void*)&style[1]);
			ParseStringIntoEnum((char**)s, bucket[2], 9, (void*)&style[2]);
			ParseStringIntoEnum((char**)s, bucket[3], 9, (void*)&style[3]);
			break;
	}

	if (style[0] == -1 || style[1] == -1 || style[2] == -1 || style[3] == -1)
	{
		return -1;
	}

	CSSPropertyValue val;

	val.borderStyle = (CSSBorderStyle) style[0];
	rule->addProperty(CSS_PROPERTY_BORDER_TOP_STYLE, val, important);

	val.borderStyle = (CSSBorderStyle) style[1];
	rule->addProperty(CSS_PROPERTY_BORDER_RIGHT_STYLE, val, important);

	val.borderStyle = (CSSBorderStyle) style[2];
	rule->addProperty(CSS_PROPERTY_BORDER_BOTTOM_STYLE, val, important);

	val.borderStyle = (CSSBorderStyle) style[3];
	rule->addProperty(CSS_PROPERTY_BORDER_LEFT_STYLE, val, important);


	return 0;
}


int CSSDocumentParser::ParseSingleBorderWidth(WEBC_CHAR * string, CSSPropertyValue * value)
{
	int n = ebs_LinearSearch(string, gpBorderWidthValues, CSS_NUM_BORDER_WIDTH_VALUES, _StringListItemCompare);
	if (n == -1)
	{
		if (GetLength(string, &value->borderThickness.val.length) == -1)
		{
			return -1;
		}
		value->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;

	}
	else
	{
		value->borderThickness.type = CSS_BORDER_THICKNESS_ABSOLUTE;
		value->borderThickness.val.absolute = (CSSBorderThicknessAbsolute) n;
	}
	return 0;
}

int CSSDocumentParser::ParseBorderWidth(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important)
{
	WEBC_CHAR bucket[4][16];
	int numRead = _bucket_scan(string, (WEBC_CHAR *) bucket, 4, 16);

	CSSProperty prop[4];

	prop[0].flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);
	prop[1].flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);
	prop[2].flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);
	prop[3].flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);

	prop[0].type = CSS_PROPERTY_BORDER_TOP_WIDTH; prop[0].flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);
	prop[1].type = CSS_PROPERTY_BORDER_RIGHT_WIDTH; prop[1].flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);
	prop[2].type = CSS_PROPERTY_BORDER_BOTTOM_WIDTH; prop[2].flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);
	prop[3].type = CSS_PROPERTY_BORDER_LEFT_WIDTH; prop[2].flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);

	switch (numRead)
	{
		case 0:
			return -1;

		case 1:
			if (!ParseSingleBorderWidth(bucket[0], &prop[0].value))
			{
				prop[1].value = prop[2].value = prop[3].value = prop[0].value;
				break;
			}
			return -1;

		case 2:
			if (!ParseSingleBorderWidth(bucket[0], &prop[0].value) && !ParseSingleBorderWidth(bucket[1], &prop[1].value))
			{
				prop[3].value = prop[1].value;
				prop[2].value = prop[0].value;
				break;
			}
		case 3:
			if (!ParseSingleBorderWidth(bucket[0], &prop[0].value) &&
			    !ParseSingleBorderWidth(bucket[1], &prop[1].value) &&
			    !ParseSingleBorderWidth(bucket[2], &prop[2].value))
			{
				prop[3].value = prop[1].value;
				break;
			}
			return -1;

		case 4:
			if (!ParseSingleBorderWidth(bucket[0], &prop[0].value) &&
			    !ParseSingleBorderWidth(bucket[1], &prop[1].value) &&
			    !ParseSingleBorderWidth(bucket[2], &prop[2].value) &&
			    !ParseSingleBorderWidth(bucket[3], &prop[3].value))
			{
				break;
			}
			return -1;
	}

	rule->addProperty(prop[0]);
	rule->addProperty(prop[1]);
	rule->addProperty(prop[2]);
	rule->addProperty(prop[3]);
	return 0;
}

int CSSDocumentParser::ParsePadding(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important)
{

	WEBC_CHAR bucket[4][16];
	CSSPropertyValue val[4];
	int numRead = _bucket_scan(string, (WEBC_CHAR *) bucket, 4, 16);
	if (numRead == 0)
	{
		return -1;
	}
	if (numRead == 4)
	{
		if (GetLength(bucket[3], &val[3].length) == -1)
		{
			numRead--;
		}
	}
	if (numRead >= 3)
	{
		if (GetLength(bucket[2], &val[2].length) == -1)
		{
			numRead--;
		}
	}
	if (numRead >= 2)
	{
		if (GetLength(bucket[1], &val[1].length) == -1)
		{
			numRead--;
		}
	}
	if (numRead >= 1)
	{
		if (GetLength(bucket[0], &val[0].length) == -1)
		{
			numRead--;
		}
	}
	switch (numRead)
	{
		case 1:
			rule->addProperty(CSS_PROPERTY_PADDING_TOP, val[0], important);
			rule->addProperty(CSS_PROPERTY_PADDING_LEFT, val[0], important);
			rule->addProperty(CSS_PROPERTY_PADDING_BOTTOM, val[0], important);
			rule->addProperty(CSS_PROPERTY_PADDING_RIGHT, val[0], important);
			break;

		case 2:
			rule->addProperty(CSS_PROPERTY_PADDING_TOP, val[0], important);
			rule->addProperty(CSS_PROPERTY_PADDING_BOTTOM, val[0], important);
			rule->addProperty(CSS_PROPERTY_PADDING_RIGHT, val[1], important);
			rule->addProperty(CSS_PROPERTY_PADDING_LEFT, val[1], important);
			break;

		case 3:
			rule->addProperty(CSS_PROPERTY_PADDING_TOP, val[0], important);
			rule->addProperty(CSS_PROPERTY_PADDING_RIGHT, val[1], important);
			rule->addProperty(CSS_PROPERTY_PADDING_BOTTOM, val[2], important);
			rule->addProperty(CSS_PROPERTY_PADDING_LEFT, val[1], important);
			break;

		case 4:
			rule->addProperty(CSS_PROPERTY_PADDING_TOP, val[0], important);
			rule->addProperty(CSS_PROPERTY_PADDING_RIGHT, val[1], important);
			rule->addProperty(CSS_PROPERTY_PADDING_BOTTOM, val[2], important);
			rule->addProperty(CSS_PROPERTY_PADDING_LEFT, val[3], important);
			break;

		default:
			return -1;
	}
	return 0;


}

int CSSDocumentParser::ParseBackgroundPosition(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important)
{
	WEBC_CHAR s[2][32];

	WEBC_BOOL positionParsed = WEBC_FALSE;
	WEBC_CHAR* xPosStr = 0;
	WEBC_CHAR* yPosStr = 0;
	CSSPropertyValue xPosVal;
	CSSPropertyValue yPosVal;

	int numRead = _bucket_scan(string, (WEBC_CHAR*) s, 2, 32);

	CSSProperty p;
	p.flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);
	for (int t = 0; t < numRead; t++)
	{
		if (!positionParsed)
		{
			if (ParseBackgroundPositionX(s[t], &xPosVal) == 0)
			{
				positionParsed = WEBC_TRUE;
				xPosStr = s[t];
			}

			if (ParseBackgroundPositionY(s[t], &yPosVal) == 0)
			{
				positionParsed = WEBC_TRUE;
				yPosStr = s[t];
			}
		}
		else
		{
			if (xPosStr && ParseBackgroundPositionY(s[t], &yPosVal) == 0)
			{
				yPosStr = s[t];
			}
			else
			{
				if (ParseBackgroundPositionX(s[t], &xPosVal) == 0)
				{
					xPosStr = s[t];
				}
				else if (ParseBackgroundPositionY(s[t], &yPosVal) == 0)
				{
					yPosStr = s[t];
				}
			}
		}
	}

	if (xPosStr)
	{
		p.type = CSS_PROPERTY_BACKGROUND_X_POSITION;
		p.value = xPosVal;
		rule->addProperty(p);
	}

	if (yPosStr)
	{
		p.type = CSS_PROPERTY_BACKGROUND_Y_POSITION;
		p.value = yPosVal;
		rule->addProperty(p);
	}

	if (xPosStr || yPosStr)
	{
		return 0;
	}

	return -1;
}

int CSSDocumentParser::ParseBorder(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important)
{
	if (ParseSingleBorder(string, rule, CSS_PROPERTY_BORDER_TOP, important) == -1)
	{
		return -1;
	}
	if (ParseSingleBorder(string, rule, CSS_PROPERTY_BORDER_LEFT, important) == -1)
	{
		return -1;
	}
	if (ParseSingleBorder(string, rule, CSS_PROPERTY_BORDER_BOTTOM, important) == -1)
	{
		return -1;
	}
	if (ParseSingleBorder(string, rule, CSS_PROPERTY_BORDER_RIGHT, important) == -1)
	{
		return -1;
	}
	return 0;
}

int CSSDocumentParser::ParseColor(WEBC_CHAR * string, CSSPropertyValue *value)
{
	return (HTML_ParseColor(string, 0, &value->color) > 0)? 0 : -1;
}

int CSSDocumentParser::ParseURL(WEBC_CHAR * string, CSSPropertyValue *value, CSSOrigin origin, URLDescriptor *sheetUrl)
{
	if (webc_c_stricmp(string, "none") == 0)
	{
		value->url = WEBC_NULL;
		return 0;
	}

	if (webc_strlen(string) <= 4)
	{
		return -1;
	}

	if (UPPERCASE(string[0]) != 'U' || UPPERCASE(string[1]) != 'R' || UPPERCASE(string[2]) != 'L' || UPPERCASE(string[3]) != '(')
	{
		return -1;
	}

	WEBC_CHAR endChar = ')';
	WEBC_CHAR * s = string + 4;
	while (IS_WHITESPACE(*s))
	{
		s++;
	}
	if (*s=='"' || *s=='\'')
	{
		endChar = *s;
		s++;
	}
	WEBC_CHAR * url = (WEBC_CHAR*) WEBC_MALLOC((webc_strlen(s)+1)*sizeof(WEBC_CHAR));
	if (!url)
	{
		return -1;
	}

	int n = 0;
	while ((*s != endChar) && *s)
	{
		// Parentheses, commas, whitespace characters, single quotes (') and double quotes (")
		//  appearing in a URL must be escaped with a backslash: '\(', '\)', '\,'.
		if ((s[0] == '\\') && ((s[1] == '(') || (s[1] == ')') || (s[1] == ',') || (s[1] == '\'') || (s[1] == '"')))
		{
			url[n] = s[1];
			s += 2;
		}
		else
		{
			url[n] = s[0];
			s++;
		}

		n++;
	}
	url[n] = 0;

	// convert relative url to absolute
	URLDescriptor absUrlDesc (url, sheetUrl);
	WEBC_CHAR *absUrl = absUrlDesc.toString();
	if (absUrl)
	{
		value->url = mDocumentContext->GetStringTable()->GetString(absUrl);
		absUrlDesc.freeString(absUrl);
	}
	else
	{
		value->url = 0;
	}
	WEBC_FREE(url);

	return 0;
}

int CSSDocumentParser::GetLength(WEBC_CHAR * string, CSSLength * length)
{
	if (webc_c_stricmp(string, "auto") == 0)
	{
		length->type = CSS_LENGTH_UNIT_AUTO;
		return 0;
	}

	if (webc_c_strstr(string, "in"))
	{
		length->type = CSS_LENGTH_UNIT_IN;
	}
	else if (webc_c_strstr(string, "mm"))
	{
		length->type = CSS_LENGTH_UNIT_MM;
	}
	else if (webc_c_strstr(string, "cm"))
	{
		length->type = CSS_LENGTH_UNIT_CM;
	}
	else if (webc_c_strstr(string, "ex"))
	{
		length->type = CSS_LENGTH_UNIT_EX;
	}
	else if (webc_c_strstr(string, "em"))
	{
		length->type = CSS_LENGTH_UNIT_EM;
	}
	else if (webc_c_strstr(string, "pt"))
	{
		length->type = CSS_LENGTH_UNIT_PT;
	}
	else if (webc_c_strstr(string, "pc"))
	{
		length->type = CSS_LENGTH_UNIT_PC;
	}
	else if (webc_c_strstr(string, "px"))
	{
		length->type = CSS_LENGTH_UNIT_PX;
	}
	else if (webc_c_strstr(string, "%"))
	{
		length->type = CSS_LENGTH_UNIT_PERCENT;
	}
	else
	{
		length->type = CSS_LENGTH_UNIT_PX;
	}

	WEBC_CHAR * p = string;
	while (IS_WHITESPACE(*p))
	{
		p++;				// eat white space
	}

	if (*p == '.')
	{
		length->val.dec = 0;
	}
	else
	{
		if (!(IS_DIGIT(*p) || (*p == '-') || (*p == '+')))
		{
			return -1;
		}
		length->val.dec = (WEBC_UINT16) webc_atoi(p);
	}
	length->val.frac = 0;

	while (IS_DIGIT(*p) || (*p == '-') || (*p == '+'))
	{
		p++;
	}
	if (*p != '.')
	{
		return (0);
	}
	p++;

	long divisor = 10;
	long fraction = 0;
	while (IS_DIGIT(*p) && (divisor <= 100000))
	{
		                 // (1 * D[n]) / (10^n)
		fraction += (10000 * (*p - '0')) / divisor;
		divisor *= 10;
		p++;
	}

	length->val.frac = (WEBC_UINT16) EBSMIN(0xffff, fraction);

	return 0;
}

int CSSDocumentParser::ParseLength(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return GetLength(string, &value->length);
}

int CSSDocumentParser::ParseFontSize(WEBC_CHAR * string, CSSPropertyValue * value)
{
	int n;

	if (webc_c_stricmp(string, "larger") == 0)
	{
		value->fontSize.type = CSS_FONT_SIZE_RELATIVE;
		value->fontSize.val.relative = CSS_FONT_SIZE_LARGER;
		PRINTF1("Font is LARGER\n")
		return 0;
	}
	else if (webc_c_stricmp(string, "smaller") == 0)
	{
		value->fontSize.type = CSS_FONT_SIZE_RELATIVE;
		value->fontSize.val.relative = CSS_FONT_SIZE_SMALLER;
		PRINTF1("Font is SMALLER\n")
		return 0;
	}
	else if (webc_c_stricmp(string, "same") == 0)
	{
		value->fontSize.type = CSS_FONT_SIZE_RELATIVE;
		value->fontSize.val.relative = CSS_FONT_SIZE_SAME;
		PRINTF1("Font is SAME\n")
		return 0;
	}
	else if ( (n = ebs_LinearSearch(string, gpFontSizeValues, CSS_NUM_FONT_SIZE_VALUES, _StringListItemCompare)) != -1)
	{
		value->fontSize.type = CSS_FONT_SIZE_ABSOLUTE;
		value->fontSize.val.absolute = n;
		PRINTF1("Font is an absolute\n")
		return 0;
	}
	else
	{
		CSSLength fontSize;												// only want to change value's CSSFontSize if it's parsed correctly
		if (GetLength(string, &fontSize) == -1)
		{
			return (-1);
		}
		if (fontSize.val.dec < 0)								// ignore negative values
		{
			return (-1);
		}
		value->fontSize.val.length = fontSize;
		value->fontSize.type = CSS_FONT_SIZE_LENGTH;
		PRINTF1("Font is some length\n")
		return (0);
	}
	return (-1);
}

int CSSDocumentParser::ParseFontFamily(WEBC_CHAR * string, CSSPropertyValue * value)
{
	if (webc_strlen(string) < 1)
	{
		return (-1);
	}
	value->name = mDocumentContext->GetStringTable()->GetString(string);
	return (0);
}

int CSSDocumentParser::ParseTextAlignment(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum((char**)gpTextAlignmentValues, string, CSS_NUM_TEXT_ALIGNMENT_VALUES, (void*)&value->textAlign);
}

int CSSDocumentParser::ParsePosition(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum((char**)gpPositionValues, string, CSS_NUM_POSITION_VALUES, (void*)&value->position);
}

int CSSDocumentParser::ParseVisibility(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum((char**)gpVisibilityValues, string, CSS_NUM_VISIBILITY_VALUES, (void*)&value->visibility);
}

int CSSDocumentParser::ParseTextDecoration(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum((char**)gpTextDecorationValues, string, CSS_NUM_TEXT_DECORATION_VALUES, (void*)&value->textDecoration);
}

int CSSDocumentParser::ParseFontWeight(WEBC_CHAR * string, CSSPropertyValue * value)
{
	int n = ebs_BinarySearch(string, gpFontWeightValues, CSS_NUM_FONT_WEIGHT_VALUES, _StringListItemCompare);
	if (n == -1)
		return -1;
	if (n > 9)
	{
		switch(n)
		{
			case 10:
				n = 7;
				break;
			case 11:
				n = CSS_FONT_WEIGHT_BOLDER;
				break;
			case 12:
				n = CSS_FONT_WEIGHT_LIGHTER;
				break;
			case 13:
				n = CSS_FONT_WEIGHT_SAME;
				break;
		}
	}
	value->fontWeight = (CSSFontWeight) n;
	return 0;
}

int CSSDocumentParser::ParseFontVariant(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum((char**)gpFontVariantValues, string, CSS_NUM_FONT_VARIANT_VALUES,(void*)&value->fontVariant);
}

int CSSDocumentParser::ParseFontStyle(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum((char**)gpFontStyleValues, string, CSS_NUM_FONT_STYLE_VALUES,(void*)&value->fontStyle);
}

int CSSDocumentParser::ParseWhiteSpace(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum((char**)gpWhiteSpaceValues, string, CSS_NUM_WHITE_SPACE_VALUES, (void*)&value->whitespace);
}

int CSSDocumentParser::ParseStringIntoEnum(char ** strings, WEBC_CHAR * value, int size, void * enumPointer)
{
	int n = ebs_BinarySearch(value, strings, size, _StringListItemCompare);
	if (n == -1)
	{
		return -1;
	}
	int * thing = (int*)enumPointer;
	*thing = n;
	return 0;
}

int CSSDocumentParser::ParseBackgroundRepeat(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum((char**)gpBackgroundRepeatValues, string, CSS_NUM_BACKGROUND_REPEAT_VALUES, (void*)&value->backgroundRepeat);

}

int CSSDocumentParser::ParseBackgroundAttachment(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum((char**)gpBackgroundAttachmentValues, string, CSS_NUM_BACKGROUND_ATTACHMENT_VALUES, (void*)&value->backgroundAttachment);
}

int CSSDocumentParser::ParseBackgroundPositionX(WEBC_CHAR * string, CSSPropertyValue * value)
{
	CSSLength length;
	if (webc_c_stricmp(string, "center") == 0)
	{
		value->length.type = CSS_LENGTH_UNIT_PERCENT;
		value->length.val.dec = 50;
		value->length.val.frac = 0;
		return 0;
	}
	else if (webc_c_stricmp(string, "left") == 0)
	{
		value->length.type = CSS_LENGTH_UNIT_PERCENT;
		value->length.val.dec = 0;
		value->length.val.frac = 0;
		return 0;
	}
	else if (webc_c_stricmp(string, "right") == 0)
	{
		value->length.type = CSS_LENGTH_UNIT_PERCENT;
		value->length.val.dec = 100;
		value->length.val.frac = 0;
		return 0;
	}
	else if (GetLength(string, &length) == 0)
	{
		value->length = length;
		return 0;
	}
	return -1;
}

int CSSDocumentParser::ParseBackgroundPositionY(WEBC_CHAR * string, CSSPropertyValue * value)
{
	CSSLength length;
	if (webc_c_stricmp(string, "center") == 0)
	{
		value->length.type = CSS_LENGTH_UNIT_PERCENT;
		value->length.val.dec = 50;
		value->length.val.frac = 0;
		return 0;
	}
	else if (webc_c_stricmp(string, "top") == 0)
	{
		value->length.type = CSS_LENGTH_UNIT_PERCENT;
		value->length.val.dec = 0;
		value->length.val.frac = 0;
		return 0;
	}
	else if (webc_c_stricmp(string, "bottom") == 0)
	{
		value->length.type = CSS_LENGTH_UNIT_PERCENT;
		value->length.val.dec = 100;
		value->length.val.frac = 0;
		return 0;
	}
	if (GetLength(string, &length) == 0)
	{
		value->length = length;
		return 0;
	}
	return -1;
}

int CSSDocumentParser::ParseTextTransform(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum((char**)gpTextTransformValues, string, CSS_NUM_TEXT_TRANSFORM_VALUES, (void*)&value->textTransform);
}

int CSSDocumentParser::ParseLineHeight(WEBC_CHAR * string, CSSPropertyValue * value)
{

	if (webc_c_stricmp(string, "normal") == 0) 							// set to 1.0
	{
		value->lineHeight.type = CSS_LINE_HEIGHT_NUMBER;
		value->lineHeight.length.val.dec = 1;
		value->lineHeight.length.val.frac = 0;
		return 0;
	}
	else
	{
		value->lineHeight.type = CSS_LINE_HEIGHT_LENGTH;
        /* Changed January 2008, was previously:
		    return ParseLength(string, value);
            Note: This value is not used anyway (webC does not support line height */
        return GetLength(string, &value->lineHeight.length);
	}
	return -1;
}

int CSSDocumentParser::ParseSingleBorderStyle(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum (
			(char**) gpBorderStyleValues,
			string,
			CSS_NUM_BORDER_STYLE_VALUES,
			(void*) &value->borderStyle
		);
}

int CSSDocumentParser::ParseFloat(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum((char**)gpFloatValues, string, CSS_NUM_FLOAT_VALUES, (void*)&value->floatType);
}

int CSSDocumentParser::ParseNumber(WEBC_CHAR * string, CSSPropertyValue * value)
{
	value->number = webc_atoi(string);
	return (0);
}

// Parse 0.X to .X up to 1. IE uses 0 to 100
int CSSDocumentParser::ParseOpacity(WEBC_CHAR * string, CSSPropertyValue * value)
{
	if (GetLength(string, &value->length) == -1)
	{
		return -1;
	}
	if (value->length.val.dec >= 1 && value->length.val.frac == 0)
	{
		// frac is converted to a value from 0 to 255.. used for the alpha channel value
		// Frac is scaled to 0 to 10000 is 0 to 1. so (frac*255)/10000; is scaled to 255
		value->length.val.dec = 0;
		value->length.val.frac = 10000;
	}

	return (0);
}

int CSSDocumentParser::ParseClear(WEBC_CHAR * string, CSSPropertyValue * value)
{
	WEBC_CHAR _both[] = {'b','o','t','h',0};

	if (!webc_c_stricmp(string, "all"))
	{
		string = _both;
	}

	return ParseStringIntoEnum((char**)gpClearValues, string, CSS_NUM_CLEAR_VALUES, (void*)&value->clear);
}

int CSSDocumentParser::ParseDisplay(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum((char**)gpDisplayValues, string, CSS_NUM_DISPLAY_VALUES, (void*)&value->display);
}
int CSSDocumentParser::ParseOverflow(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum((char**)gpOverflowValues, string, CSS_NUM_OVERFLOW_VALUES, (void*)&value->overflow);

}

int CSSDocumentParser::ParseListStyleType(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return (ParseStringIntoEnum((char **) gpListStyleTypeValues,
	                            string,
								CSS_NUM_LIST_STYLE_TYPE_VALUES,
								(void *) &value->listStyleType));
}

int CSSDocumentParser::ParseListStylePosition(WEBC_CHAR * string, CSSPropertyValue * value)
{
	if (webc_c_stricmp(string, "inside")==0)
	{
		value->listStylePosition = CSS_LIST_STYLE_POSITION_INSIDE;
		return (0);
	}
	else if (webc_c_stricmp(string, "outside") == 0)
	{
		value->listStylePosition = CSS_LIST_STYLE_POSITION_OUTSIDE;
		return (0);
	}

	return (-1);
}

int CSSDocumentParser::ParseVerticalAlign(WEBC_CHAR * string, CSSPropertyValue * value)
{
	return ParseStringIntoEnum (
			(char**) gpVerticalAlignValues,
			string,
			CSS_NUM_VERTICAL_ALIGN_VALUES,
			(void*) &value->verticalAlign
		);
}

void CSSDocumentParser::ParseDeclaration (
		WEBC_CHAR* name,
		WEBC_CHAR* value,
		CSSRule* rule,
		CSSOrigin origin,
		URLDescriptor* sheetUrl
	)
{
	WEBC_UINT8 important = 0;
	int propNum = ebs_BinarySearch(name, gpCSSPropertyNames, CSS_NUM_PROPERTIES, _StringListItemCompare);
	WEBC_CHAR * i = webc_c_strstr(value, "!");
	if (i)
	{
		important = 1;
		*i = '\0';
		i--;
		while (IS_WHITESPACE(*i))
		{
			i--;
		}
		i++;
		*i = '\0';
	}

	PRINTF2("Parsed property: (%s)\n", name)

	ParsePropertyValue((CSSPropertyType) propNum, value, rule, origin, sheetUrl, important);
}

void CSSDocumentParser::ParsePropertyValue (
		CSSPropertyType propNum,
		WEBC_CHAR* value,
		CSSRule* rule,
		CSSOrigin origin,
		URLDescriptor* sheetUrl,
		WEBC_UINT8 important
	)
{
	WEBC_UINT8 inherit = 0, noValue = 0;
	CSSPropertyValue propertyValue;

	if (value[0] == 0)
	{
		noValue = CSS_PROPERTY_FLAG_NO_VALUE;
		propertyValue.number = 0;   /* Not used set it so debugger doesn;t catch uninitialized data */
	}
	else if (!webc_c_stricmp(value, "inherit"))
	{
		inherit = CSS_PROPERTY_FLAG_INHERIT;
		propertyValue.number = 0;   /* Not used set it so debugger doesn;t catch uninitialized data */
	}
	else
	{
		int ret = -1;
		switch (propNum) 							// ugh...
		{
			case CSS_PROPERTY_ALINK_COLOR:
			case CSS_PROPERTY_VLINK_COLOR:
			case CSS_PROPERTY_BACKGROUND_COLOR:
			case CSS_PROPERTY_COLOR:
			case CSS_PROPERTY_LINK_COLOR:
			case CSS_PROPERTY_BORDER_TOP_COLOR:
			case CSS_PROPERTY_BORDER_LEFT_COLOR:
			case CSS_PROPERTY_BORDER_BOTTOM_COLOR:
			case CSS_PROPERTY_BORDER_RIGHT_COLOR:
			case CSS_PROPERTY_OUTLINE_COLOR:
				ret = ParseColor(value, &propertyValue);
				break;

			case CSS_PROPERTY_BACKGROUND_IMAGE:
			case CSS_PROPERTY_LIST_STYLE_IMAGE:
				ret = ParseURL(value, &propertyValue, origin, sheetUrl);
				break;

			case CSS_PROPERTY_BORDER_TOP:
			case CSS_PROPERTY_BORDER_LEFT:
			case CSS_PROPERTY_BORDER_RIGHT:
			case CSS_PROPERTY_BORDER_BOTTOM:
				ret = ParseSingleBorder(value, rule, (CSSPropertyType) propNum, important);
				return;

			case CSS_PROPERTY_BORDER_COLOR:
				ret = ParseBorderColor(value, rule, important);
				return;

			case CSS_PROPERTY_OUTLINE:
				ret = ParseOutline(value, rule, important);
				break;

			case CSS_PROPERTY_BORDER_BOTTOM_WIDTH:
			case CSS_PROPERTY_BORDER_LEFT_WIDTH:
			case CSS_PROPERTY_BORDER_TOP_WIDTH:
			case CSS_PROPERTY_BORDER_RIGHT_WIDTH:
			case CSS_PROPERTY_OUTLINE_WIDTH:
				ret = ParseSingleBorderWidth(value, &propertyValue);
				break;

			case CSS_PROPERTY_BORDER_TOP_STYLE:
			case CSS_PROPERTY_BORDER_BOTTOM_STYLE:
			case CSS_PROPERTY_BORDER_LEFT_STYLE:
			case CSS_PROPERTY_BORDER_RIGHT_STYLE:
			case CSS_PROPERTY_OUTLINE_STYLE:
				ret = ParseSingleBorderStyle(value, &propertyValue);
				break;

			case CSS_PROPERTY_BORDER_WIDTH:
				// ParseBorderWidth adds the property, because there might be more than
				//  one property specified in the declaration
				ret = ParseBorderWidth(value, rule, important);
                // Change Jan 08. Return because ParseBorderWidth() added properties of individual edges.
                return;

			case CSS_PROPERTY_MARGIN_BOTTOM:
			case CSS_PROPERTY_MARGIN_LEFT:
			case CSS_PROPERTY_MARGIN_RIGHT:
			case CSS_PROPERTY_MARGIN_TOP:
			case CSS_PROPERTY_PADDING_BOTTOM:
			case CSS_PROPERTY_PADDING_LEFT:
			case CSS_PROPERTY_PADDING_TOP:
			case CSS_PROPERTY_PADDING_RIGHT:
			case CSS_PROPERTY_TEXT_INDENT:
			case CSS_PROPERTY_HEIGHT:
			case CSS_PROPERTY_WIDTH:
			case CSS_PROPERTY_WORD_SPACING:
				ret = ParseLength(value, &propertyValue);
				break;

			case CSS_PROPERTY_FONT_SIZE:
				ret = ParseFontSize(value, &propertyValue);
				break;

			case CSS_PROPERTY_MARGIN:
				ret = ParseMargin(value, rule, important);
				return;

			case CSS_PROPERTY_PADDING:
				ret = ParsePadding(value, rule, important);
				return;

			case CSS_PROPERTY_TEXT_ALIGN:
				ret = ParseTextAlignment(value, &propertyValue);
				break;

			case CSS_PROPERTY_TEXT_DECORATION:
				ret = ParseTextDecoration(value, &propertyValue);
				break;

			case CSS_PROPERTY_FONT_WEIGHT:
				ret = ParseFontWeight(value, &propertyValue);
				break;

			case CSS_PROPERTY_FONT_VARIANT:
				ret = ParseFontVariant(value, &propertyValue);
				break;

			case CSS_PROPERTY_FONT_STYLE:
				ret = ParseFontStyle(value, &propertyValue);
				break;

			case CSS_PROPERTY_WHITE_SPACE:
				ret = ParseWhiteSpace(value, &propertyValue);
				break;

			case CSS_PROPERTY_BORDER:
				ret = ParseBorder(value, rule, important);
				return;

			case CSS_PROPERTY_BACKGROUND_ATTACHMENT:
				ret = ParseBackgroundAttachment(value, &propertyValue);
				break;

			case CSS_PROPERTY_BACKGROUND_REPEAT:
				ret = ParseBackgroundRepeat(value, &propertyValue);
				break;

			case CSS_PROPERTY_BACKGROUND:
				ret = ParseBackground(value, rule, important, origin, sheetUrl);
				return;

			case CSS_PROPERTY_BORDER_STYLE:
				ret = ParseBorderStyle(value, rule, important);
				return;

			case CSS_PROPERTY_BACKGROUND_POSITION:
				ret = ParseBackgroundPosition(value, rule, important);
				return;

			case CSS_PROPERTY_FLOAT:
				ret = ParseFloat(value, &propertyValue);
				break;

			case CSS_PROPERTY_FONT_FAMILY:
				ret = ParseFontFamily(value, &propertyValue);
				break;

			case CSS_PROPERTY_TEXT_TRANSFORM:
				ret = ParseTextTransform(value, &propertyValue);
				break;

			case CSS_PROPERTY_CLEAR:
				ret = ParseClear(value, &propertyValue);
				break;

			case CSS_PROPERTY_DISPLAY:
				ret = ParseDisplay(value, &propertyValue);
				{
					if (ret >= 0 && propertyValue.display==CSS_DISPLAY_INLINE_BLOCK)
					{
//						printf("Am Not Changing INLINE_BLOCK to default \n");
//						ret=-1;
//						printf("Change INLINE_BLOCK to inline\n");
						propertyValue.display=CSS_DISPLAY_INLINE;
						ret=0;

					}
				}
				break;

			case CSS_PROPERTY_OVERFLOW:
			case CSS_PROPERTY_OVERFLOW_X:
			case CSS_PROPERTY_OVERFLOW_Y:
				ret = ParseOverflow(value, &propertyValue);
				break;
			case CSS_PROPERTY_LIST_STYLE:
				ret = ParseListStyle(value, rule, important, origin, sheetUrl);
				return;

			case CSS_PROPERTY_LIST_STYLE_TYPE:
				ret = ParseListStyleType(value, &propertyValue);
				break;

			case CSS_PROPERTY_VERTICAL_ALIGN:
				ret = ParseVerticalAlign(value, &propertyValue);
				break;

			case CSS_PROPERTY_FONT:
				ret = ParseFont(value, rule, important);
				return;

			case CSS_PROPERTY_LINE_HEIGHT:
				ret = ParseLineHeight(value, &propertyValue);
				break;

			case CSS_PROPERTY_LIST_STYLE_POSITION:
				ret = ParseListStylePosition(value, &propertyValue);
				break;

			case CSS_PROPERTY_VISIBILITY:
				ret = ParseVisibility(value, &propertyValue);
				break;

			case CSS_PROPERTY_POSITION_X:
			case CSS_PROPERTY_POSITION_Y:
				ret = ParseLength(value, &propertyValue);
				break;

			case CSS_PROPERTY_POSITION:
				ret = ParsePosition(value, &propertyValue);
				break;

			case CSS_PROPERTY_TOP:
			case CSS_PROPERTY_RIGHT:
			case CSS_PROPERTY_BOTTOM:
			case CSS_PROPERTY_LEFT:
				ret = ParseLength(value, &propertyValue);
				break;

			case CSS_PROPERTY_ZINDEX:
				ret = ParseNumber(value, &propertyValue);
				break;

			case CSS_PROPERTY_OPACITY:
				ret = ParseOpacity(value, &propertyValue);
				break;

			default:
				PRINTF2("Unknown property: (%d)\n", propNum)
				return;
		}

		if (ret)  // we failed parsing some value
		{
			return;
		}
	}

	CSSProperty p;
	p.type = (CSSPropertyType) propNum;
	p.value = propertyValue;
	p.flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0) | inherit | noValue;
	rule->addProperty(p);
}

WEBC_CHAR * CSSDocumentParser::ParseSelectors(WEBC_CHAR * string, CSSRule * rule)
{
	WEBC_CHAR bucket[64];
	int len = 0;
	while (1)
	{
		while (IS_WHITESPACE(*string))
		{
			string++;				// eat leading white space
		}
		for (len = 0; (len < 63) && (IS_SELECTOR_CHAR(string[len]) || IS_WHITESPACE(string[len])); len++)
		{
			bucket[len] = string[len];
		}
		if (len == 0)
		{
			break;
		}
		bucket[len] = 0;
		WEBC_CHAR * c = bucket + len - 1;
		while (IS_WHITESPACE(*c)) 				// eat trailing white space
		{
			*c = '\0';
			c--;
		}
		PRINTF2("\"%s\" selector read\n", bucket)

		CSSSelector * sel;
		sel = mDocumentContext->newSelector(bucket);
		if (sel)
		{
			rule->addSelector(sel);
		}

		string += len;
		while (*string && *string != ',' && *string != '{')
		{
			string++;
		}
		if (*string == '{')
		{
			return string;
		}
		if (*string == ',')
		{
			string++;
		}
		if (!*string)
		{
			return string;
		}
	}

	return string;

}


CSSSelector::CSSSelector(CSSDocumentContext * d, const WEBC_CHAR * s)
{
	id = WEBC_NULL;
	className = WEBC_NULL;
	for (int n=0; n<CSS_CFG_MAX_PSEUDO_CLASSES; n++)
	{
		pseudoClass[n] = CSS_PSEUDO_CLASS_NONE;
	}
	tag = HTML_ANY_TAG;
	parent = WEBC_NULL;
	child = WEBC_NULL;
	document = d;
	referenceCount = 0;
	parseSelector(s);
}

CSSSelector::~CSSSelector()
{
	WEBC_ASSERT(referenceCount == 0);
	WEBC_ASSERT(document);
	webc_string_table_remove(id, __FILE__, __LINE__);
	webc_string_table_remove(className, __FILE__, __LINE__);
	WEBC_DELETE(parent);
}

void CSSSelector::setID(const WEBC_CHAR * i)
{
	const WEBC_CHAR *newStr = webc_string_table_add(i, __FILE__, __LINE__);
	if (id)
	{
		webc_string_table_remove(id, __FILE__, __LINE__);
	}
	id = newStr;
}

void CSSSelector::setClass(const WEBC_CHAR * c)
{
	const WEBC_CHAR *newStr = webc_string_table_add(c, __FILE__, __LINE__);
	if (className)
	{
		webc_string_table_remove(className, __FILE__, __LINE__);
	}
	className = newStr;
}

void CSSSelector::setPseudoClass(CSSPseudoClass pc, int n)
{
	if ((n >= 0) && (n < CSS_CFG_MAX_PSEUDO_CLASSES))
	{
		pseudoClass[n] = pc;
	}
}

void CSSSelector::setTag(HTMLTagType t)
{
	tag = t;
}

void CSSSelector::setParent(CSSSelector * p)
{
	parent = p;
}

CSSSelector * CSSSelector::getParent(void)
{
	return parent;
}

void CSSSelector::setChild(CSSSelector * p)
{
	child = p;
}

CSSSelector * CSSSelector::getChild(void)
{
	return child;
}

const WEBC_CHAR * CSSSelector::getClass(void)
{
	return className;
}

const WEBC_CHAR * CSSSelector::getID(void)
{
	return id;
}

HTMLTagType CSSSelector::getTag(void)
{
	return tag;
}

CSSPseudoClass CSSSelector::getPseudoClass(int n)
{
	return pseudoClass[EBSCLIP(n, 0, CSS_CFG_MAX_PSEUDO_CLASSES)];
}

void CSSSelector::addReference(void)
{
	WEBC_ASSERT(referenceCount < 0x7fffffff);
	referenceCount++;
}

int CSSSelector::removeReference(void)
{
	WEBC_ASSERT(referenceCount > 0);
	referenceCount--;

  	if (referenceCount == 0)
  	{
  		document->deleteSelector(this);
		return (0);
  	}

	return referenceCount;
}


long CSSSelector::getSpecificity(void)
{
	int numIds = 0;
	int numClasses = 0;
	int numTags = 0;

	CSSSelector *sel = this;
	while (sel)
	{
		if (sel->getID())
		{
			numIds++;
		}

		if (sel->getClass())
		{
			numClasses++;
		}

		for (int n=0; n<CSS_CFG_MAX_PSEUDO_CLASSES; n++)
		{
			if (sel->getPseudoClass(n) != CSS_PSEUDO_CLASS_NONE)
			{
				numClasses++;
			}
			else
			{
				break;
			}
		}

		if (sel->getTag() != HTML_ANY_TAG)
		{
			numTags++;
		}

		sel = sel->getParent();
	}

	if (numIds > 7)
	{
		numIds = 7;
	}

	if (numClasses > 15)
	{
		numClasses = 15;
	}

	if (numTags > 31)
	{
		numTags = 31;
	}

	return ((numIds << 9) | (numClasses << 5) | numTags);
}

int CSSSelector::elementMatches(CSSStyleElement * element)
{
	if (!element)
	{
		return 0;
	}

	char match = 0;

	if (className)         // if the selector has a class, the element must match it
	{
		WEBC_CHAR * elemClass = WEBC_NULL;
		Enumerator *e = element->GetClasses();
		if (e)
		{
			elemClass = (WEBC_CHAR *) e->Next();
			while (elemClass && (elemClass != className))
			{
				elemClass = (WEBC_CHAR *) e->Next();
			}
			e->Dispose();
		}
		if (elemClass == WEBC_NULL)		// the classes don't match
		{
			return (0);
		}
		match = 1;
	}

	if (id)                // the element must match the selector's ID
	{
		if (element->IdLowercase() != id)
		{
			return (0);
		}
		match = 1;
	}

	if (tag != HTML_ANY_TAG)  // tag must match
	{
		if (element->GetTag() != tag)
		{
			return (0);
		}
		match = 1;
	}

	for (int n=0; n<CSS_CFG_MAX_PSEUDO_CLASSES; n++)
	{
		if (pseudoClass[n] == CSS_PSEUDO_CLASS_UNKNOWN)
		{
			return (0);
		}
		else if (pseudoClass[n] != CSS_PSEUDO_CLASS_NONE)   // pseudo-class must match
		{
			if (!element->InPseudoClass(pseudoClass[n]))
			{
				return (0);
			}
			match = 1;
		}
		else
		{
			break;
		}
	}

	if (match && parent)
	{
		// some ancestor of element must match parent
		CSSStyleElement *ancestor = element->GetStyleParent();

		while (ancestor)
		{
			if (parent->elementMatches(ancestor))
			{
				break;
			}
			ancestor = ancestor->GetStyleParent();
		}

		if (!ancestor)
		{
			return (0);
		}
	}

	return (match);
}

int CSSSelector::matchesClassTagParent(CSSStyleElement * element)
{
	if (className)         // if the selector has a class, the element must match it
	{
		const WEBC_CHAR * elemClass = WEBC_NULL;
		Enumerator *e = element->GetClasses();
		if (e)
		{
			elemClass = (const WEBC_CHAR *) e->Next();
			while (elemClass && (elemClass != className))
			{
				elemClass = (const WEBC_CHAR *) e->Next();
			}
			e->Dispose();
		}
		if (elemClass == WEBC_NULL)		// the classes don't match
		{
			return (0);
		}
	}

	return (matchesTagParent(element));
}

int CSSSelector::matchesTagParent(CSSStyleElement * element)
{
	if (tag != HTML_ANY_TAG)  // tag must match
	{
		if (element->GetTag() != tag)
		{
			return (0);
		}
	}

	return (matchesParent(element));
}

int CSSSelector::matchesParent(CSSStyleElement * element)
{
	for (int n=0; n<CSS_CFG_MAX_PSEUDO_CLASSES; n++)
	{
		if (pseudoClass[n] == CSS_PSEUDO_CLASS_NONE)   // pseudo-class must match
		{
			break;
		}

		if (!element->InPseudoClass(pseudoClass[n]))
		{
			return (0);
		}
	}

	if (parent)
	{
		// some ancestor of element must match parent
		CSSStyleElement *ancestor = element->GetStyleParent();

		while (ancestor)
		{
			if (parent->elementMatches(ancestor))
			{
				break;
			}
			ancestor = ancestor->GetStyleParent();
		}

		if (!ancestor)
		{
			return (0);
		}
	}

	return (1);
}

void CSSSelector::parseSelector(const WEBC_CHAR * s)
{
	if (!document)
	{
		PRINTF1("CSS Selector has no document context")
		return;
	}

	WEBC_CHAR ch;
	WEBC_CHAR *copy = webc_malloc_string_copy(s, __FILE__, __LINE__);
	WEBC_CHAR *p = copy;
	WEBC_CHAR *begin;

	if (!copy)
	{
		return;
	}

	while (IS_WHITESPACE(*p))
	{
		p++;
	}

	int numPseudo = 0;
	while (*p && !IS_WHITESPACE(*p))
	{
		switch (*p)
		{
			case ':':
				// read pseudo-class
				p++;
				if (numPseudo < CSS_CFG_MAX_PSEUDO_CLASSES)
				{
					if (webc_c_strnicmp(p, "active", 6) == 0)
					{
						p += 6;
						pseudoClass[numPseudo++] = CSS_PSEUDO_CLASS_ACTIVE;
					}
					else if (webc_c_strnicmp(p, "link", 4) == 0)
					{
						p += 4;
						pseudoClass[numPseudo++] = CSS_PSEUDO_CLASS_LINK;
					}
					else if (webc_c_strnicmp(p, "visited", 7) == 0)
					{
						p += 7;
						pseudoClass[numPseudo++] = CSS_PSEUDO_CLASS_VISITED;
					}
					else if (webc_c_strnicmp(p, "hover", 5) == 0)
					{
						p += 5;
						pseudoClass[numPseudo++] = CSS_PSEUDO_CLASS_HOVER;
					}
					else if (webc_c_strnicmp(p, "firstline", 9) == 0)
					{
						p += 9;
						pseudoClass[numPseudo++] = CSS_PSEUDO_CLASS_FIRST_LINE;
					}
					else if (webc_c_strnicmp(p, "firstletter", 11) == 0)
					{
						p += 11;
						pseudoClass[numPseudo++] = CSS_PSEUDO_CLASS_FIRST_LETTER;
					}
					else if (webc_c_strnicmp(p, "focus", 5) == 0)
					{
						p += 5;
						pseudoClass[numPseudo++] = CSS_PSEUDO_CLASS_FOCUS;
					}
					else
					{
						if (webc_strlen(p) > 0)
						{
							// unrecognized pseudo-class
							while (IS_PSEUDO_CLASS_CHAR(*p))
							{
								p++;
							}
							pseudoClass[numPseudo++] = CSS_PSEUDO_CLASS_UNKNOWN;
						}
					}
				}
				else
				{
					// too many pseudo-classes
					while (IS_PSEUDO_CLASS_CHAR(*p))
					{
						p++;
					}
				}
				break;

			case '#':
				// read id
				p++;
				begin = p;
				while (IS_ID_CHAR(*p))
				{
					p++;
				}
				ch = *p;
				*p = 0;
				setID(begin);
				*p = ch;
				break;

			case '.':
				// read class
				p++;
				begin = p;
				while (IS_CLASS_CHAR(*p))
				{
					p++;
				}
				ch = *p;
				*p = 0;
				setClass(begin);
				*p = ch;
				break;

			default:
				// read tag
				begin = p;
				while (IS_ALPHA(*p) || IS_DIGIT(*p))
				{
					p++;
				}
				ch = *p;
				*p = 0;
				setTag((HTMLTagType) HTML_ParseTagType(begin, 0));
				*p = ch;
				if (p == begin)
				{
					p++;
				}
				break;
		}
	}

	if ((tag == HTML_ANY_TAG) && (pseudoClass[0] != CSS_PSEUDO_CLASS_NONE))
	{
		switch (pseudoClass[0])
		{
			case CSS_PSEUDO_CLASS_ACTIVE:
			case CSS_PSEUDO_CLASS_LINK:
			case CSS_PSEUDO_CLASS_VISITED:
			case CSS_PSEUDO_CLASS_HOVER:
				setTag(HTML_TAG_A);
				break;

			case CSS_PSEUDO_CLASS_FIRST_LETTER:
			case CSS_PSEUDO_CLASS_FIRST_LINE:
				setTag(HTML_TAG_P);
				break;
		}
	}

	while (IS_WHITESPACE(*p))
	{
		p++;
	}

	if (*p)
	{
		WEBC_NEW_VERBOSE(child, CSSSelector(document, p),"NewChildSelector");
		if (child)
		{
			child->setParent(this);
		}
	}

	webc_free_string_copy(copy, __FILE__, __LINE__);
}

void CSSSelector::Dispose(void)
{
	WEBC_DELETE(this);
}

WEBC_BOOL CSSSelector::Equals(HashKey *key)
{
	CSSSelector *other = (CSSSelector *) key;

	int initialTest = (id == other->getID()) && (className == other->getClass()) && (tag == other->getTag());

	if (!initialTest)
	{
		return (0);
	}

	int psFound;
	for (int mine=0; mine<CSS_CFG_MAX_PSEUDO_CLASSES; mine++)
	{
		psFound = 0;

		for (int hers=0; hers<CSS_CFG_MAX_PSEUDO_CLASSES; hers++)
		{
			if (other->getPseudoClass(hers) == pseudoClass[mine])
			{
				psFound = 1;
				break;
			}
		}

		if (!psFound)
		{
			return (0);
		}
	}

	return (!getParent() || (other->getParent() && getParent()->Equals(other->getParent())));
}

HashCode_t CSSSelector::HashCode(void)
{
	HashCode_t h;

	h = (HashCode_t) id;
	h ^= ( (h << 6) + (h >> 2) + (HashCode_t) className);
	h ^= ( (h << 5) + (h >> 2) + (HashCode_t) tag);

	h ^= (h << 5) + (h >> 3);
	for (int n=0; n<CSS_CFG_MAX_PSEUDO_CLASSES; n++)
	{
		if (pseudoClass[n] != CSS_PSEUDO_CLASS_NONE)
		{
			h ^= (1 << ((int) pseudoClass[n]));
		}
	}

	if (getParent())
	{
		h ^= ( (h << 6) + (h >> 3) + getParent()->HashCode());
	}

	return h;
}

/*****************************************************************************/
// CSSRule Methods
/*****************************************************************************/
CSSRule::CSSRule()
{
	vector_init(&mSelectors, sizeof(CSSSelector *));
	vector_init(&mProperties, sizeof(CSSProperty));
}

CSSRule::~CSSRule()
{
	clear();
}

void CSSRule::clear(void)
{
	vector_iterator sel;
	CSSSelector * s = enumerateFirstSelector(&sel);
	while (s)
	{
		s->removeReference();
		s = enumerateNextSelector(&sel);
	}

	vector_delete(&mSelectors);
	vector_delete(&mProperties);
}

int CSSRule::addSelector(CSSSelector * s)
{
	int result = vector_add_node_to_rear(&mSelectors, (WEBC_PFBYTE)&s);
	if (result >= 0)
	{
		s->addReference();
	}
	return (result);
}

int CSSRule::addProperty(CSSProperty p)
{
	return (vector_add_node_to_rear(&mProperties, (WEBC_PFBYTE)&p));
}

int CSSRule::addProperty(CSSPropertyType p, CSSPropertyValue v, WEBC_UINT8 important)
{
	CSSProperty prop;
	prop.type = p;
	prop.value = v;
 	prop.flags = (important ? CSS_PROPERTY_FLAG_IMPORTANT : 0);
	return (addProperty(prop));
}

CSSSelector * CSSRule::enumerateFirstSelector(vector_iterator * iter)
{
	CSSSelector **p = (CSSSelector **) vector_get_first(&mSelectors, iter);
	if (p)
	{
		return *p;
	}
	return WEBC_NULL;

}

CSSSelector * CSSRule::enumerateNextSelector(vector_iterator * iter)
{
	CSSSelector **p = (CSSSelector**)vector_get_next(iter);
	if (p)
	{
		return *p;
	}
	return WEBC_NULL;
}


CSSProperty * CSSRule::enumerateFirstProperty(vector_iterator * iter)
{
	CSSProperty *p = (CSSProperty *) vector_get_first(&mProperties, iter);
	return p;
}

CSSProperty * CSSRule::enumerateNextProperty(vector_iterator * iter)
{
	CSSProperty *p = (CSSProperty*)vector_get_next(iter);
	return p;
}

#endif // WEBC_SUPPORT_STYLE_SHEETS


DISPLAY_INT CSS_LengthToPixels (
		CSSLength* pLength,
		DISPLAY_INT iParentLength,
		DISPLAY_INT iFontHeight,
		DISPLAY_INT iSmallXHeight
	)
{
	long length;
	if (pLength->val.dec < 0)
	{
		length = (pLength->val.dec << 8) - ((pLength->val.frac >> 8) & 0xff);
	}
	else
	{
		length = (pLength->val.dec << 8) + ((pLength->val.frac >> 8) & 0xff);
	}

	switch (pLength->type)
	{
		case CSS_LENGTH_UNIT_PX:
			return (length >> 8);

		case CSS_LENGTH_UNIT_PERCENT:
			length = ((length * iParentLength) + 0x3200) / 0x6400;
			return (length);

		case CSS_LENGTH_UNIT_EX:
			length = (length * iSmallXHeight) + 0x80;
			return (length >> 8);

		case CSS_LENGTH_UNIT_EM:
			length = (length * iFontHeight) + 0x80;
			return (length >> 8);

		case CSS_LENGTH_UNIT_PT:
			length = ((length + 0x80) >> 8);
			length = CSS_LengthPointsToPixels(length);
			return (length);

		default:
			break;
	}

	if (pLength->type < 0 || pLength->type > 7)
	{
		return 0;
	}

	length *= unitToPixelTable[pLength->type];
	return (length >> 16);
}

DISPLAY_INT CSS_LengthToPoints (
		CSSLength *pLength,
		DISPLAY_INT iParentLength,
		DISPLAY_INT iFontHeight,
		DISPLAY_INT iSmallXHeight
	)
{
	long length;
	if (pLength->val.dec < 0)
	{
		length = (pLength->val.dec << 8) - ((pLength->val.frac >> 8) & 0xff);
	}
	else
	{
		length = (pLength->val.dec << 8) + ((pLength->val.frac >> 8) & 0xff);
	}

	switch (pLength->type)
	{
		case CSS_LENGTH_UNIT_PT:
			return (length >> 8);

		case CSS_LENGTH_UNIT_PERCENT:
			length = ((length * iParentLength) + 0x3200) / 0x6400;
			return (length);

		case CSS_LENGTH_UNIT_EX:
			length = (length * iSmallXHeight) + 0x80;
			return (length >> 8);

		case CSS_LENGTH_UNIT_EM:
			length = (length * iFontHeight) + 0x80;
			return (length >> 8);

		case CSS_LENGTH_UNIT_PX:
#if (1)
			/*  map PX size the same as PT \n"); */
			return (length >> 8);
#else
			// multiply by 3/4; round fraction
			length = ((length << 1) + length + 0x200) >> 10;
#endif
			return (length);

		default:
			break;
	}

	if (pLength->type < 0 || pLength->type > 7)
	{
		return 0;
	}

	length *= unitToPointsTable[pLength->type];
	return (length >> 16);
}

int CSS_LengthToString (
		WEBC_CHAR *str,
		CSSLength *length,
		int maxlen
	)
{
	switch (length->type)
	{
		case CSS_LENGTH_UNIT_AUTO:
			if (maxlen >= 5)
			{
				webc_c_strcpy(str, "auto");
				return (0);
			}
			break;

		default:
			// just to be safe
			if (maxlen < 17)  // 7 + 1 + 6 + 2 + 1
			{
				break;
			}

			if (length->val.frac != 0)
			{
				webc_print_dec(str, length->val.dec, 1, 7);
				webc_c_strcat(str, ".");

				// convert to base 10
				long frac = ((long) length->val.frac * 3125) / 2048;

				// get rid of trailing zeros
				while (frac % 10 == 0)
				{
					frac /= 10;
				}

				webc_print_dec(str + webc_strlen(str), length->val.frac, 4, 6);
			}
			else
			{
				webc_print_dec(str, length->val.dec, 1, 7);
			}

			switch (length->type)
			{
				case CSS_LENGTH_UNIT_IN:
					webc_c_strcat(str, "in");
					return (0);

				case CSS_LENGTH_UNIT_MM:
					webc_c_strcat(str, "mm");
					return (0);

				case CSS_LENGTH_UNIT_CM:
					webc_c_strcat(str, "cm");
					return (0);

				case CSS_LENGTH_UNIT_EX:
					webc_c_strcat(str, "ex");
					return (0);

				case CSS_LENGTH_UNIT_EM:
					webc_c_strcat(str, "em");
					return (0);

				case CSS_LENGTH_UNIT_PT:
					webc_c_strcat(str, "pt");
					return (0);

				case CSS_LENGTH_UNIT_PC:
					webc_c_strcat(str, "pc");
					return (0);

				case CSS_LENGTH_UNIT_PERCENT:
					webc_c_strcat(str, "%");
					return (0);

				default:
					return (0);
			}
			break;
	}



	return (-1);
}

int CSS_PropEnumToString(WEBC_CHAR *str, int enumValue, const char **enumStrings, unsigned enumSize, unsigned int maxlen)
{
	if ((unsigned) enumValue < enumSize)
	{
		webc_c_strncpy(str, enumStrings[enumValue], maxlen-1);
		str[maxlen-1] = 0;
		return (0);
	}
	return (-1);
}

#if (WEBC_SUPPORT_STYLE_SHEETS)

/*****************************************************************************/
// CSS_PropertyToString - convert parsed CSS property back to parsable string
//
//   str -    buffer to write string to
//   type -   the property type
//   value -  the property value
//   maxlen - the maximum number of characters to write to str (includes null)
//
// Returns: 0 on success, -1 if value is invalid
/*****************************************************************************/

int CSS_PropertyToString (
		WEBC_CHAR *str,
		CSSPropertyType type,
		CSSPropertyValue *value,
		unsigned int maxlen
	)
{
	switch (type)
	{
		case CSS_PROPERTY_ALINK_COLOR:
		case CSS_PROPERTY_VLINK_COLOR:
		case CSS_PROPERTY_BACKGROUND_COLOR:
		case CSS_PROPERTY_COLOR:
		case CSS_PROPERTY_LINK_COLOR:
		case CSS_PROPERTY_BORDER_TOP_COLOR:
		case CSS_PROPERTY_BORDER_LEFT_COLOR:
		case CSS_PROPERTY_BORDER_BOTTOM_COLOR:
		case CSS_PROPERTY_BORDER_RIGHT_COLOR:
			if (maxlen >= 8)
			{
				HTML_PrintColor(str, value->color);
				return (0);
			}
			break;

		case CSS_PROPERTY_BACKGROUND_IMAGE:
		case CSS_PROPERTY_LIST_STYLE_IMAGE:
			if (webc_strlen(value->url) <= maxlen)
			{
				webc_strcpy(str, value->url);
				return (0);
			}
			break;

		case CSS_PROPERTY_BORDER_TOP:
		case CSS_PROPERTY_BORDER_LEFT:
		case CSS_PROPERTY_BORDER_RIGHT:
		case CSS_PROPERTY_BORDER_BOTTOM:
			// tbd - aggregate properties
			return (-1);

		case CSS_PROPERTY_BORDER_COLOR:
			// tbd - aggregate properties
			return (-1);

		case CSS_PROPERTY_BORDER_BOTTOM_WIDTH:
		case CSS_PROPERTY_BORDER_LEFT_WIDTH:
		case CSS_PROPERTY_BORDER_TOP_WIDTH:
		case CSS_PROPERTY_BORDER_RIGHT_WIDTH:
			switch (value->borderThickness.type)
			{
				case CSS_BORDER_THICKNESS_LENGTH:
					return (CSS_LengthToString(str, &value->borderThickness.val.length, maxlen));

				case CSS_BORDER_THICKNESS_ABSOLUTE:
					return (CSS_PropEnumToString(str, value->borderThickness.val.absolute, gpBorderWidthValues, CSS_NUM_BORDER_WIDTH_VALUES, maxlen));

				default:
					break;
			}
			break;

		case CSS_PROPERTY_BORDER_TOP_STYLE:
		case CSS_PROPERTY_BORDER_BOTTOM_STYLE:
		case CSS_PROPERTY_BORDER_LEFT_STYLE:
		case CSS_PROPERTY_BORDER_RIGHT_STYLE:
			return (CSS_PropEnumToString(str, value->borderStyle, gpBorderStyleValues, CSS_NUM_BORDER_STYLE_VALUES, maxlen));

		case CSS_PROPERTY_BORDER_WIDTH:
			// tbd - aggregate properties
			return (-1);

		case CSS_PROPERTY_MARGIN_BOTTOM:
		case CSS_PROPERTY_MARGIN_LEFT:
		case CSS_PROPERTY_MARGIN_RIGHT:
		case CSS_PROPERTY_MARGIN_TOP:
		case CSS_PROPERTY_PADDING_BOTTOM:
		case CSS_PROPERTY_PADDING_LEFT:
		case CSS_PROPERTY_PADDING_TOP:
		case CSS_PROPERTY_PADDING_RIGHT:
		case CSS_PROPERTY_TEXT_INDENT:
		case CSS_PROPERTY_HEIGHT:
		case CSS_PROPERTY_WIDTH:
		case CSS_PROPERTY_WORD_SPACING:
		case CSS_PROPERTY_TOP:
		case CSS_PROPERTY_RIGHT:
		case CSS_PROPERTY_BOTTOM:
		case CSS_PROPERTY_LEFT:
		case CSS_PROPERTY_POSITION_X:
		case CSS_PROPERTY_POSITION_Y:
		case CSS_PROPERTY_BACKGROUND_X_POSITION:
		case CSS_PROPERTY_BACKGROUND_Y_POSITION:
		case CSS_PROPERTY_OPACITY: // not really a length, but close enough
			return (CSS_LengthToString(str, &value->length, maxlen));

		case CSS_PROPERTY_FONT_SIZE:
			switch (value->fontSize.type)
			{
				case CSS_FONT_SIZE_RELATIVE:
					switch (value->fontSize.val.relative)
					{
						case CSS_FONT_SIZE_LARGER:
							if (maxlen < 7)
							{
								break;
							}
							webc_c_strcpy(str, "larger");
							return (0);

						case CSS_FONT_SIZE_SMALLER:
							if (maxlen < 8)
							{
								break;
							}
							webc_c_strcpy(str, "smaller");
							return (0);

						case CSS_FONT_SIZE_SAME:
							if (maxlen < 5)
							{
								break;
							}
							webc_c_strcpy(str, "same");
							return (0);

						default:
							break;
					}
					break;

				case CSS_FONT_SIZE_ABSOLUTE:
					return (CSS_PropEnumToString(str, value->fontSize.val.absolute, gpFontSizeValues, CSS_NUM_FONT_SIZE_VALUES, maxlen));

				case CSS_FONT_SIZE_LENGTH:
					return (CSS_LengthToString(str, &value->fontSize.val.length, maxlen));

				default:
					break;
			}
			break;

		case CSS_PROPERTY_MARGIN:
			// tbd - aggregate properties
			break;

		case CSS_PROPERTY_PADDING:
			// tbd - aggregate properties
			break;

		case CSS_PROPERTY_TEXT_ALIGN:
			return (CSS_PropEnumToString(str, value->textAlign, gpTextAlignmentValues, CSS_NUM_TEXT_ALIGNMENT_VALUES, maxlen));

		case CSS_PROPERTY_TEXT_DECORATION:
			return (CSS_PropEnumToString(str, value->textDecoration, gpTextDecorationValues, CSS_NUM_TEXT_DECORATION_VALUES, maxlen));

		case CSS_PROPERTY_FONT_WEIGHT:
			return (CSS_PropEnumToString(str, value->fontWeight, gpFontWeightValues, CSS_NUM_FONT_WEIGHT_VALUES, maxlen));

		case CSS_PROPERTY_FONT_VARIANT:
			return (CSS_PropEnumToString(str, value->fontVariant, gpFontVariantValues, CSS_NUM_FONT_VARIANT_VALUES, maxlen));

		case CSS_PROPERTY_FONT_STYLE:
			return (CSS_PropEnumToString(str, value->fontStyle, gpFontStyleValues, CSS_NUM_FONT_STYLE_VALUES, maxlen));

		case CSS_PROPERTY_WHITE_SPACE:
			return (CSS_PropEnumToString(str, value->whitespace, gpWhiteSpaceValues, CSS_NUM_WHITE_SPACE_VALUES, maxlen));

		case CSS_PROPERTY_BORDER:
			// tbd - aggregate properties
			break;

		case CSS_PROPERTY_BACKGROUND_ATTACHMENT:
			return (CSS_PropEnumToString(str, value->backgroundAttachment, gpBackgroundAttachmentValues, CSS_NUM_BACKGROUND_ATTACHMENT_VALUES, maxlen));

		case CSS_PROPERTY_BACKGROUND_REPEAT:
			return (CSS_PropEnumToString(str, value->backgroundRepeat, gpBackgroundRepeatValues, CSS_NUM_BACKGROUND_REPEAT_VALUES, maxlen));

		case CSS_PROPERTY_BACKGROUND:
			// tbd - aggregate properties
			return (-1);

		case CSS_PROPERTY_BORDER_STYLE:
			// tbd - aggregate properties
			return (-1);

		case CSS_PROPERTY_BACKGROUND_POSITION:
			// tbd - aggregate properties
			return (-1);

		case CSS_PROPERTY_FLOAT:
			return (CSS_PropEnumToString(str, value->floatType, gpFloatValues, CSS_NUM_FLOAT_VALUES, maxlen));

		case CSS_PROPERTY_FONT_FAMILY:
			if (webc_strlen(value->name) <= maxlen)
			{
				webc_strcpy(str, value->name);
				return (0);
			}
			break;

		case CSS_PROPERTY_TEXT_TRANSFORM:
			return (CSS_PropEnumToString(str, value->textTransform, gpTextTransformValues, CSS_NUM_TEXT_TRANSFORM_VALUES, maxlen));

		case CSS_PROPERTY_CLEAR:
			return (CSS_PropEnumToString(str, value->clear, gpClearValues, CSS_NUM_CLEAR_VALUES, maxlen));

		case CSS_PROPERTY_DISPLAY:
			return (CSS_PropEnumToString(str, value->display, gpDisplayValues, CSS_NUM_DISPLAY_VALUES, maxlen));

		case CSS_PROPERTY_OVERFLOW:
		case CSS_PROPERTY_OVERFLOW_X:
		case CSS_PROPERTY_OVERFLOW_Y:
			return (CSS_PropEnumToString(str, value->overflow, gpOverflowValues, CSS_NUM_OVERFLOW_VALUES, maxlen));
		case CSS_PROPERTY_LIST_STYLE_TYPE:
			return (CSS_PropEnumToString(str, value->listStyleType, gpListStyleTypeValues, CSS_NUM_LIST_STYLE_TYPE_VALUES, maxlen));

		case CSS_PROPERTY_VERTICAL_ALIGN:
			return (CSS_PropEnumToString(str, value->verticalAlign, gpVerticalAlignValues, CSS_NUM_VERTICAL_ALIGN_VALUES, maxlen));

		case CSS_PROPERTY_FONT:
			// tbd - aggregate properties
			return (-1);

		case CSS_PROPERTY_LINE_HEIGHT:
			switch (value->lineHeight.type)
			{
				case CSS_LINE_HEIGHT_NUMBER:
					if (maxlen < 7)
					{
						break;
					}
					webc_c_strcpy(str, "normal");
					return (0);

				case CSS_LINE_HEIGHT_LENGTH:
					return (CSS_LengthToString(str, &value->lineHeight.length, maxlen));

				default:
					break;
			}
			break;

		case CSS_PROPERTY_LIST_STYLE_POSITION:
			switch (value->listStylePosition)
			{
				case CSS_LIST_STYLE_POSITION_INSIDE:
					if (maxlen >= 7)
					{
						webc_c_strcpy(str, "inside");
						return (0);
					}
					break;

				case CSS_LIST_STYLE_POSITION_OUTSIDE:
					if (maxlen >= 8)
					{
						webc_c_strcpy(str, "outside");
						return (0);
					}
					break;

				default:
					break;
			}
			break;

		case CSS_PROPERTY_VISIBILITY:
			return (CSS_PropEnumToString(str, value->visibility, gpVisibilityValues, CSS_NUM_VISIBILITY_VALUES, maxlen));

		case CSS_PROPERTY_POSITION:
			return (CSS_PropEnumToString(str, value->position, gpPositionValues, CSS_NUM_POSITION_VALUES, maxlen));

		default:
			break;
	}

	return (-1);
}

#endif // WEBC_SUPPORT_STYLE_SHEETS


HTMLLength CSS_LengthToHTML(CSSLength* length, DISPLAY_INT emHeight, DISPLAY_INT exHeight)
{
	HTMLLength htmlLength;

	switch (length->type)
	{
		case CSS_LENGTH_UNIT_AUTO:
			HTML_LengthSet(htmlLength, 0, HTML_LENGTH_UNIT_NONE);
			break;

		case CSS_LENGTH_UNIT_PERCENT:
			HTML_LengthSet(htmlLength, length->val.dec, HTML_LENGTH_UNIT_PERCENT);
			break;

		default:
		{
			DISPLAY_INT len = CSS_LengthToPixels (length, 0, emHeight, exHeight);
			HTML_LengthSet(htmlLength, len, HTML_LENGTH_UNIT_PIXELS);
			break;
		}
	}

	return (htmlLength);
}

int CSS_HTMLLengthToCSS (
		CSSLength *length,
		HTMLLength htmlLen
	)
{
	switch (HTML_LengthGetUnit(htmlLen))
	{
		case HTML_LENGTH_UNIT_STAR:
		case HTML_LENGTH_UNIT_NONE:
			length->type = CSS_LENGTH_UNIT_AUTO;
			length->val.dec = 0;
			length->val.frac = 0;
			return (0);

		case HTML_LENGTH_UNIT_PIXELS:
			length->type = CSS_LENGTH_UNIT_PX;
			length->val.dec = UTIL_SAFE_ULONG_TO_SHORT(HTML_LengthGetValue(htmlLen));
			length->val.frac = 0;
			return (0);

		case HTML_LENGTH_UNIT_PERCENT:
			length->type = CSS_LENGTH_UNIT_PERCENT;
			length->val.dec = UTIL_SAFE_ULONG_TO_SHORT(HTML_LengthGetValue(htmlLen));
			length->val.frac = 0;
			return (0);

		//case HTML_LENGTH_UNIT_STAR:
		//	return (-1);

		default:
			return (-1);
	}
}

int CSS_LengthIsRelative(CSSLength *pLength)
{
	return ( (pLength->type == CSS_LENGTH_UNIT_AUTO) ||
	         (pLength->type == CSS_LENGTH_UNIT_EM) ||
	         (pLength->type == CSS_LENGTH_UNIT_EX) ||
	         (pLength->type == CSS_LENGTH_UNIT_PERCENT) );
}

long _bucket_scan(WEBC_CHAR *string, WEBC_CHAR *bucket, int numBuckets, long bucketSize)
{
	int n;
	long len;

	for (n=0; n<numBuckets; n++)
	{
		string = webc_scan_whitespace(string);
		len = webc_scan_string(bucket, string, bucketSize-1);
		if (len == 0)
		{
			break;
		}
		string += len;
		bucket += bucketSize;
	}

	return (n);
}

int _StringListItemCompare(void *item, void *list, int index)
{
	return (webc_c_stricmp((WEBC_CHAR *) item, ((char **) list)[index]));
}
