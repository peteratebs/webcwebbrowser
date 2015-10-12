
//
// HTML.H - Structs, prototypes, and defines for HTML.C
//
// EBS - WebC
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __HTML_H__
#define __HTML_H__

#include "webcfg.h"
#include "xml.h"

typedef long                      HTML_INT32;
typedef unsigned long             HTML_UINT32;
typedef XMLchar                   HTML_CHAR;
typedef long                      HTML_INT;
typedef int                       HTML_BOOL;
typedef unsigned long             HTMLColor;
typedef char                      URI_CHAR;
typedef unsigned long             HTMLLength;


#define HTML_NBSP_CHAR            ((HTML_CHAR) 160)

enum e_HTMLLengthUnit
{
	HTML_LENGTH_UNIT_NONE = 0,
	HTML_LENGTH_UNIT_PIXELS,
	HTML_LENGTH_UNIT_PERCENT,
	HTML_LENGTH_UNIT_STAR,
	HTML_NUM_LENGTH_UNITS
};

#define HTML_LENGTH_UNIT_MASK     0xf0000000
#define HTML_LENGTH_UNIT_OFFSET   28
#define HTML_LENGTH_VALUE_MASK    0x0fffffff

#define HTML_LengthGetUnit(L)     ((HTMLLengthUnit)(((L) & HTML_LENGTH_UNIT_MASK) >> HTML_LENGTH_UNIT_OFFSET))
#define HTML_LengthSetUnit(L,U)   {(L) = ((L) & ~HTML_LENGTH_UNIT_MASK) | (((U) << HTML_LENGTH_UNIT_OFFSET) & HTML_LENGTH_UNIT_MASK) ;}
#define HTML_LengthGetValue(L)    ((long)((L) & HTML_LENGTH_VALUE_MASK))
#define HTML_LengthSetValue(L,V)  {(L) = ((L) & ~HTML_LENGTH_VALUE_MASK) | ((V) & HTML_LENGTH_VALUE_MASK);}
#define HTML_LengthSet(L,V,U)     {(L) = (((U) << HTML_LENGTH_UNIT_OFFSET) & HTML_LENGTH_UNIT_MASK) | ((V) & HTML_LENGTH_VALUE_MASK);}
#define HTML_ToLength(V,U)        (((U) << HTML_LENGTH_UNIT_OFFSET) & HTML_LENGTH_UNIT_MASK) | ((V) & HTML_LENGTH_VALUE_MASK)

#define HTML_COLOR_RED_MASK       0x00ff0000
#define HTML_COLOR_GREEN_MASK     0x0000ff00
#define HTML_COLOR_BLUE_MASK      0x000000ff
#define HTML_COLOR_ALPHA_MASK     0xff000000

#define HTML_COLOR_RED_OFFSET     16
#define HTML_COLOR_GREEN_OFFSET   8
#define HTML_COLOR_BLUE_OFFSET    0
#define HTML_COLOR_ALPHA_OFFSET   24

#define HTML_ColorGetRed(C)       ((unsigned char)(((C) & HTML_COLOR_RED_MASK) >> HTML_COLOR_RED_OFFSET))
#define HTML_ColorGetGreen(C)     ((unsigned char)(((C) & HTML_COLOR_GREEN_MASK) >> HTML_COLOR_GREEN_OFFSET))
#define HTML_ColorGetBlue(C)      ((unsigned char)(((C) & HTML_COLOR_BLUE_MASK) >> HTML_COLOR_BLUE_OFFSET))
#define HTML_ColorGetAlpha(C)     ((unsigned char)(((C) & HTML_COLOR_ALPHA_MASK) >> HTML_COLOR_ALPHA_OFFSET))

#define HTML_ColorSetRed(C,R)     {(C) = ((C) & ~HTML_COLOR_RED_MASK) | ((R) & 0xff) << HTML_COLOR_RED_OFFSET);}
#define HTML_ColorSetGreen(C,G)   {(C) = ((C) & ~HTML_COLOR_GREEN_MASK) | ((R) & 0xff) << HTML_COLOR_GREEN_OFFSET);}
#define HTML_ColorSetBlue(C,B)    {(C) = ((C) & ~HTML_COLOR_BLUE_MASK) | ((R) & 0xff) << HTML_COLOR_BLUE_OFFSET);}
#define HTML_ColorSetAlpha(C,A)   {(C) = ((C) & ~HTML_COLOR_ALPHA_MASK) | ((R) & 0xff) << HTML_COLOR_ALPHA_OFFSET);}

#define HTML_ColorSet(C,R,G,B,A)  {(C) = (((R) & 0xff) << HTML_COLOR_RED_OFFSET) | \
                                         (((G) & 0xff) << HTML_COLOR_GREEN_OFFSET) | \
                                         (((B) & 0xff) << HTML_COLOR_BLUE_OFFSET) | \
                                         (((A) & 0xff) << HTML_COLOR_ALPHA_OFFSET); }

#define HTML_RGBAToColor(R,G,B,A) ((((R) & 0xff) << HTML_COLOR_RED_OFFSET) | \
                                   (((G) & 0xff) << HTML_COLOR_GREEN_OFFSET) | \
                                   (((B) & 0xff) << HTML_COLOR_BLUE_OFFSET) | \
                                   (((A) & 0xff) << HTML_COLOR_ALPHA_OFFSET))

enum e_HTMLAttributeType
{
	HTML_UNKNOWN_ATTRIB = -1,
	HTML_ATTRIB_ABBR = 0,
	HTML_ATTRIB_ACCEPT,
	HTML_ATTRIB_ACCEPT_CHARSET,
	HTML_ATTRIB_ACCESSKEY,
	HTML_ATTRIB_ACTION,
	HTML_ATTRIB_ALIGN,
	HTML_ATTRIB_ALINK,
	HTML_ATTRIB_ALT,
	HTML_ATTRIB_ARCHIVE,
	HTML_ATTRIB_AXIS,
	HTML_ATTRIB_BACKGROUND,
	HTML_ATTRIB_BGCOLOR,
	HTML_ATTRIB_BORDER,
	HTML_ATTRIB_CELLPADDING,
	HTML_ATTRIB_CELLSPACING,
	HTML_ATTRIB_CHAR,
	HTML_ATTRIB_CHAROFF,
	HTML_ATTRIB_CHARSET,
	HTML_ATTRIB_CHECKED,
	HTML_ATTRIB_CITE,
	HTML_ATTRIB_CLASS,
	HTML_ATTRIB_CLASSID,
	HTML_ATTRIB_CLEAR,
	HTML_ATTRIB_CODE,
	HTML_ATTRIB_CODEBASE,
	HTML_ATTRIB_CODETYPE,
	HTML_ATTRIB_COLOR,
	HTML_ATTRIB_COLS,
	HTML_ATTRIB_COLSPAN,
	HTML_ATTRIB_COMPACT,
	HTML_ATTRIB_CONTENT,
	HTML_ATTRIB_COORDS,
	HTML_ATTRIB_DATA,
	HTML_ATTRIB_DATETIME,
	HTML_ATTRIB_DECLARE,
	HTML_ATTRIB_DEFER,
	HTML_ATTRIB_DIR,
	HTML_ATTRIB_DISABLED,
	HTML_ATTRIB_ENCTYPE,
	HTML_ATTRIB_EVENTHANDLER,
	HTML_ATTRIB_FACE,
	HTML_ATTRIB_FOR,
	HTML_ATTRIB_FRAME,
	HTML_ATTRIB_FRAMEBORDER,
	HTML_ATTRIB_HEADERS,
	HTML_ATTRIB_HEIGHT,
	HTML_ATTRIB_HREF,
	HTML_ATTRIB_HREFLANG,
	HTML_ATTRIB_HSPACE,
	HTML_ATTRIB_HTTP_EQUIV,
	HTML_ATTRIB_ID,
	HTML_ATTRIB_ISMAP,
	HTML_ATTRIB_LABEL,
	HTML_ATTRIB_LANG,
	HTML_ATTRIB_LANGUAGE,
	HTML_ATTRIB_LINK,
	HTML_ATTRIB_LONGDESC,
	HTML_ATTRIB_MARGINHEIGHT,
	HTML_ATTRIB_MARGINWIDTH,
	HTML_ATTRIB_MAXLENGTH,
	HTML_ATTRIB_MEDIA,
	HTML_ATTRIB_METHOD,
	HTML_ATTRIB_MULTIPLE,
	HTML_ATTRIB_NAME,
	HTML_ATTRIB_NOBACKGROUND,
	HTML_ATTRIB_NOHREF,
	HTML_ATTRIB_NORESIZE,
	HTML_ATTRIB_NOSHADE,
	HTML_ATTRIB_NOTAB,
	HTML_ATTRIB_NOWRAP,
	HTML_ATTRIB_OBJECT,
	HTML_ATTRIB_ONBLUR,
	HTML_ATTRIB_ONCHANGE,
	HTML_ATTRIB_ONCLICK,
	HTML_ATTRIB_ONDBLCLICK,
	HTML_ATTRIB_ONFOCUS,
	HTML_ATTRIB_ONKEYDOWN,
	HTML_ATTRIB_ONKEYPRESS,
	HTML_ATTRIB_ONKEYUP,
	HTML_ATTRIB_ONLOAD,
	HTML_ATTRIB_ONLOSECAPTURE,
	HTML_ATTRIB_ONMOUSEDOWN,
	HTML_ATTRIB_ONMOUSEMOVE,
	HTML_ATTRIB_ONMOUSEOUT,
	HTML_ATTRIB_ONMOUSEOVER,
	HTML_ATTRIB_ONMOUSEUP,
	HTML_ATTRIB_ONRESET,
	HTML_ATTRIB_ONSELECT,
	HTML_ATTRIB_ONSUBMIT,
	HTML_ATTRIB_ONUNLOAD,
	HTML_ATTRIB_PROFILE,
	HTML_ATTRIB_PROMPT,
	HTML_ATTRIB_QUARANTINE,
	HTML_ATTRIB_READONLY,
	HTML_ATTRIB_REL,
	HTML_ATTRIB_REV,
	HTML_ATTRIB_ROWS,
	HTML_ATTRIB_ROWSPAN,
	HTML_ATTRIB_RULES,
	HTML_ATTRIB_SCHEME,
	HTML_ATTRIB_SCOPE,
	HTML_ATTRIB_SCROLLING,
	HTML_ATTRIB_SELECTED,
	HTML_ATTRIB_SHAPE,
	HTML_ATTRIB_SIZE,
	HTML_ATTRIB_SPAN,
	HTML_ATTRIB_SRC,
	HTML_ATTRIB_STANDBY,
	HTML_ATTRIB_START,
	HTML_ATTRIB_STYLE,
	HTML_ATTRIB_SUMMARY,
	HTML_ATTRIB_TABINDEX,
	HTML_ATTRIB_TARGET,
	HTML_ATTRIB_TEXT,
	HTML_ATTRIB_TITLE,
	HTML_ATTRIB_TYPE,
	HTML_ATTRIB_USEMAP,
	HTML_ATTRIB_VALIGN,
	HTML_ATTRIB_VALUE,
	HTML_ATTRIB_VALUETYPE,
	HTML_ATTRIB_VERSION,
	HTML_ATTRIB_VLINK,
	HTML_ATTRIB_VSPACE,
	HTML_ATTRIB_WIDTH,
	HTML_ATTRIB_WRAP,
	HTML_NUM_ATTRIBUTES
};

enum e_HTMLTagType
{
	HTML_ANY_TAG = -2,
	HTML_UNKNOWN_TAG = -1,
	HTML_TAG_A=0,
	HTML_TAG_ABBR,
	HTML_TAG_ACRONYM,
	HTML_TAG_ADDRESS,
	HTML_TAG_APPLET,
	HTML_TAG_AREA,
 	HTML_TAG_B,
	HTML_TAG_BASE,
	HTML_TAG_BASEFONT,
	HTML_TAG_BDO,
	HTML_TAG_BIG,
	HTML_TAG_BLOCKQUOTE,
	HTML_TAG_BODY,
	HTML_TAG_BR,
	HTML_TAG_BUTTON,
	HTML_TAG_CAPTION,
	HTML_TAG_CENTER,
	HTML_TAG_CITE,
	HTML_TAG_CODE,
	HTML_TAG_COL,
	HTML_TAG_COLGROUP,
	HTML_TAG_DD,
	HTML_TAG_DEL,
	HTML_TAG_DFN,
	HTML_TAG_DIR,
	HTML_TAG_DIV,
	HTML_TAG_DL,
	HTML_TAG_DT,
	HTML_TAG_EM,
#if (WEBC_SUPPORT_EMBED)
	HTML_TAG_EMBED,
#endif
	HTML_TAG_FIELDSET,
	HTML_TAG_FONT,
	HTML_TAG_FORM,
	HTML_TAG_FRAME,
	HTML_TAG_FRAMESET,
	HTML_TAG_H1,
	HTML_TAG_H2,
	HTML_TAG_H3,
	HTML_TAG_H4,
	HTML_TAG_H5,
	HTML_TAG_H6,
	HTML_TAG_HR,
	HTML_TAG_HTML,
	HTML_TAG_I,
	HTML_TAG_IFRAME,
	HTML_TAG_IMG,
	HTML_TAG_INPUT,
	HTML_TAG_INS,
	HTML_TAG_ISINDEX,
	HTML_TAG_KBD,
	HTML_TAG_LABEL,
	HTML_TAG_LEGEND,
	HTML_TAG_LI,
	HTML_TAG_LINK,
	HTML_TAG_MAP,
	HTML_TAG_MENU,
	HTML_TAG_META,
	HTML_TAG_NOBR,
	HTML_TAG_NOFRAMES,
	HTML_TAG_NOSCRIPT,
	HTML_TAG_OBJECT,
	HTML_TAG_OL,
	HTML_TAG_OPTGROUP,
	HTML_TAG_OPTION,
	HTML_TAG_P,
	HTML_TAG_PARAM,
	HTML_TAG_PRE,
	HTML_TAG_Q,
	HTML_TAG_S,
	HTML_TAG_SAMP,
	HTML_TAG_SCRIPT,
	HTML_TAG_SELECT,
	HTML_TAG_SMALL,
	HTML_TAG_SPACER,
	HTML_TAG_SPAN,
	HTML_TAG_STRIKE,
	HTML_TAG_STRONG,
	HTML_TAG_STYLE,
	HTML_TAG_SUB,
	HTML_TAG_SUP,
	HTML_TAG_TABLE,
	HTML_TAG_TBODY,
	HTML_TAG_TD,
	HTML_TAG_TEXTAREA,
	HTML_TAG_TFOOT,
	HTML_TAG_TH,
	HTML_TAG_THEAD,
	HTML_TAG_TITLE,
	HTML_TAG_TR,
	HTML_TAG_TT,
	HTML_TAG_U,
	HTML_TAG_UL,
	HTML_TAG_VAR,
	HTML_TAG_STRING, // not a real tag
	HTML_NUM_TAGS
};

enum e_HTMLInputType
{
	HTML_INPUT_TYPE_UNKNOWN = -1,
	HTML_INPUT_TYPE_BUTTON = 0,
	HTML_INPUT_TYPE_CHECKBOX,
	HTML_INPUT_TYPE_FILE,
	HTML_INPUT_TYPE_HIDDEN,
	HTML_INPUT_TYPE_IMAGE,
	HTML_INPUT_TYPE_PASSWORD,
	HTML_INPUT_TYPE_RADIO,
	HTML_INPUT_TYPE_RESET,
	HTML_INPUT_TYPE_SUBMIT,
	HTML_INPUT_TYPE_TEXT,
	HTML_INPUT_TYPE_TEXTAREA,
	HTML_INPUT_TYPE_OPTION,
	HTML_INPUT_TYPE_SELECT,
	HTML_INPUT_TYPE_UNDEFINED,
	HTML_NUM_INPUT_TYPES
};

enum e_HTMLAlignType
{
	HTML_ALIGN_UNKNOWN = -1,
	HTML_ALIGN_ABSBOTTOM = 0,
	HTML_ALIGN_ABSMIDDLE,
	HTML_ALIGN_BASELINE,
	HTML_ALIGN_BOTTOM,
	HTML_ALIGN_CENTER,
	HTML_ALIGN_JUSTIFY,
	HTML_ALIGN_LEFT,
	HTML_ALIGN_MIDDLE,
	HTML_ALIGN_NONE,
	HTML_ALIGN_RIGHT,
	HTML_ALIGN_TEXTBOTTOM,
	HTML_ALIGN_TEXTTOP,
	HTML_ALIGN_TOP,
	HTML_NUM_ALIGN_TYPES
};

enum e_HTMLVAlignType
{
	HTML_VALIGN_UNKNOWN = -1,
	HTML_VALIGN_BOTTOM = 0,
	HTML_VALIGN_CENTER,
	HTML_VALIGN_MIDDLE, // tbd - always set this to center
	HTML_VALIGN_NONE,
	HTML_VALIGN_TOP,
	HTML_NUM_VALIGN_TYPES
};

enum e_HTMLShapeType
{
	HTML_SHAPE_UNKNOWN = -1,
	HTML_SHAPE_CIRCLE = 0,
	HTML_SHAPE_POLY,
	HTML_SHAPE_RECT,
	HTML_NUM_SHAPE_TYPES
};

enum e_HTMLTableFrameType
{
	HTML_TABLEFRAME_UNKNOWN = -1,
	HTML_TABLEFRAME_ABOVE = 0,
	HTML_TABLEFRAME_BELOW,
	HTML_TABLEFRAME_BORDER,
	HTML_TABLEFRAME_BOX,
	HTML_TABLEFRAME_HSIDES,
	HTML_TABLEFRAME_LHS,
	HTML_TABLEFRAME_RHS,
	HTML_TABLEFRAME_VOID,
	HTML_TABLEFRAME_VSIDES,
	HTML_NUM_TABLEFRAME_TYPES
};

typedef enum e_HTMLAttributeType  HTMLAttributeType;
typedef enum e_HTMLTagType        HTMLTagType;
typedef enum e_HTMLInputType      HTMLInputType;
typedef enum e_HTMLLengthUnit     HTMLLengthUnit;
typedef enum e_HTMLAlignType      HTMLAlignType;
typedef enum e_HTMLVAlignType     HTMLVAlignType;
typedef enum e_HTMLShapeType      HTMLShapeType;
typedef enum e_HTMLTableFrameType HTMLTableFrameType;

struct s_HTMLParseContext
{
	XMLParseContext xmlContext;
	HTML_BOOL       inScript;
	HTML_BOOL       inStyle;
};

struct s_HTMLTagParse
{
	XMLUnitParse    xmlParse;
	HTMLTagType     htmlTag;
};

typedef struct s_HTMLParseContext HTMLParseContext;
typedef struct s_HTMLTagParse     HTMLTagParse;
typedef struct s_HTMLUnitParse    HTMLUnitParse;

typedef XMLAttribValueParse       HTMLAttribValueParse;

#ifdef __cplusplus
extern "C" {
#endif

int HTML_InitParse (
		HTMLParseContext* ctx
	);

void HTML_DestroyParse (
		HTMLParseContext* ctx
	);

HTML_INT32 HTML_ParseUnit (
		HTMLParseContext* ctx,
		HTMLTagParse* parse,
		const HTML_CHAR* buffer,
		HTML_INT32 bufferLen
	);

HTMLAttribValueParse* HTML_GetTagAttribute (
		HTMLTagParse* parse,
		HTMLAttributeType attrib
	);

HTMLAttributeType HTML_ParseAttributeType (
		HTML_CHAR* str,
		long len
	);

HTMLTagType HTML_ParseTagType (
		HTML_CHAR* str,
		long len
	);

HTMLInputType HTML_ParseInputType (
		HTML_CHAR* str,
		long len
	);

const char* HTML_InputTypeToString (
		HTMLInputType eType
	);

HTMLAlignType HTML_ParseAlignType (
		HTML_CHAR* str,
		long len
	);

const char* HTML_AlignTypeToString (
		HTMLAlignType align
	);

const char* HTML_TagTypeToString (
		HTMLTagType tagType
	);

const char* HTML_AttributeTypeToString (
		HTMLAttributeType attrib
	);

HTMLVAlignType HTML_ParseVAlignType (
		HTML_CHAR* str,
		long len
	);

HTMLShapeType HTML_ParseShapeType (
		HTML_CHAR* str,
		long len
	);

const char* HTML_ShapeTypeToString (
		HTMLShapeType shape);

HTMLTableFrameType HTML_ParseTableFrameType (
		HTML_CHAR* str,
		long len
	);

HTML_INT32 HTML_ParseLength (
		HTML_CHAR* str,
		HTML_INT32 strLen,
		HTMLLength* length
	);

void HTML_PrintLength (
		HTML_CHAR* toStr,
		HTML_INT32 strLen,
		HTMLLength length
	);

HTML_INT32 HTML_ParseColor (
		HTML_CHAR* str,
		long len,
		HTMLColor* color
	);

HTMLColor HTML_NameToColor (
		const HTML_CHAR* str
	);

HTML_INT32 HTML_PrintColor (
		HTML_CHAR* toStr,
		HTMLColor color
	);

HTML_INT32 HTML_CountLengthList (
		HTML_CHAR* str
	);

HTML_INT32 HTML_ParseLengthList (
		HTML_CHAR* str,
		HTMLLength* lengthArray,
		HTML_INT32 arraySize
	);

HTML_CHAR* HTML_PrintLengthList (
		HTMLLength* pLengthArray,
		HTML_INT32 lArraySize
	);

void HTML_InitTagParse (
		HTMLTagParse* parse,
		HTMLTagType tagType
	);

long HTML_UnescapeString (
		const HTML_CHAR * srcStr,
		HTML_CHAR * dstStr
	);

HTML_INT32 HTML_ParseHexNumber (
		const HTML_CHAR *str,
		HTML_INT32 maxDigits,
		long* outNum,
		HTML_INT32 *consumed
	);

HTML_INT32 HTML_ParseDecNumber (
		const HTML_CHAR *str,
		HTML_INT32 maxDigits,
		long* outNum
	);
HTML_INT32 HTML_AlphaFraction (
		const HTML_CHAR *str,
		HTML_INT32 maxDigits,
		long* outNum
	);
HTML_INT32 HTML_LengthToPixels (
		HTMLLength length,
		HTML_INT32 i100Percent
	);

#ifdef __cplusplus
}
#endif

#endif
