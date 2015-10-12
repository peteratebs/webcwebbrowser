#ifndef __EBS_CSS_INCLUDED__
#define __EBS_CSS_INCLUDED__

#ifdef WEBC_BUILDING_WIGITS_ONLY

#include "webctypes.h" // for WEBC_UINT8, WEBC_UINT16, etc.
#include "webstr.hpp"
#include "html.h"
#include "vector.h"
#include "ref.hpp"
#include "cssborder.h"

#else

typedef struct s_CSSBorderThickness       CSSBorderThickness;
typedef struct s_CSSFontSize              CSSFontSize;
typedef struct s_CSSLength                CSSLength;
typedef struct s_CSSLineHeight            CSSLineHeight;

typedef union u_CSSPropertyValue          CSSPropertyValue;

#include "webctypes.h" // for WEBC_UINT8, WEBC_UINT16, etc.
#include "webstr.hpp"
#include "html.h"
#include "vector.h"
#include "ref.hpp"
#include "cssborder.h"

#if (WEBC_SUPPORT_JSCRIPT)
 struct JSObject;
 #ifdef __cplusplus
  #include "jsobjref.hpp"
 #endif
#endif //WEBC_SUPPORT_JSCRIPT

#define CSS_PARSE_BUFFER_SIZE WEBC_LOAD_BUFFER_SIZE

/* Stuff asking for a CSS Property shouldn't ask for a shorthand property.
   They're all enumerated only for the purposes of parsing

   Almost all of the enumerations depend on order for parsing				*/

enum e_CSSPropertyType
{
	CSS_PROPERTY_UNKNOWN = -1,
	CSS_PROPERTY_ALINK_COLOR = 0,
	CSS_PROPERTY_VLINK_COLOR,           // 1
	CSS_PROPERTY_BACKGROUND,
	CSS_PROPERTY_BACKGROUND_ATTACHMENT,
	CSS_PROPERTY_BACKGROUND_COLOR,
	CSS_PROPERTY_BACKGROUND_IMAGE,
	CSS_PROPERTY_BACKGROUND_POSITION,
	CSS_PROPERTY_BACKGROUND_X_POSITION,
	CSS_PROPERTY_BACKGROUND_Y_POSITION,
	CSS_PROPERTY_BACKGROUND_REPEAT,
	CSS_PROPERTY_BORDER,				//10
	CSS_PROPERTY_BORDER_BOTTOM,
	CSS_PROPERTY_BORDER_BOTTOM_COLOR,
	CSS_PROPERTY_BORDER_BOTTOM_STYLE,
	CSS_PROPERTY_BORDER_BOTTOM_WIDTH,
	CSS_PROPERTY_BORDER_COLOR,
	CSS_PROPERTY_BORDER_LEFT,
	CSS_PROPERTY_BORDER_LEFT_COLOR,
	CSS_PROPERTY_BORDER_LEFT_STYLE,
	CSS_PROPERTY_BORDER_LEFT_WIDTH,
	CSS_PROPERTY_BORDER_RIGHT,			// 20
	CSS_PROPERTY_BORDER_RIGHT_COLOR,
	CSS_PROPERTY_BORDER_RIGHT_STYLE,
	CSS_PROPERTY_BORDER_RIGHT_WIDTH,
	CSS_PROPERTY_BORDER_STYLE,
	CSS_PROPERTY_BORDER_TOP,
	CSS_PROPERTY_BORDER_TOP_COLOR,
	CSS_PROPERTY_BORDER_TOP_STYLE,
	CSS_PROPERTY_BORDER_TOP_WIDTH,
	CSS_PROPERTY_BORDER_WIDTH,
	CSS_PROPERTY_BOTTOM,				// 30
	CSS_PROPERTY_CLEAR,
	CSS_PROPERTY_COLOR,
	CSS_PROPERTY_DISPLAY,
	CSS_PROPERTY_FLOAT,
	CSS_PROPERTY_FONT,
	CSS_PROPERTY_FONT_FAMILY,
	CSS_PROPERTY_FONT_SIZE,
	CSS_PROPERTY_FONT_STYLE,
	CSS_PROPERTY_FONT_VARIANT,
	CSS_PROPERTY_FONT_WEIGHT,			// 40
	CSS_PROPERTY_HEIGHT,
	CSS_PROPERTY_LEFT,
	CSS_PROPERTY_LETTER_SPACING,
	CSS_PROPERTY_LINE_HEIGHT,
	CSS_PROPERTY_LINK_COLOR,
	CSS_PROPERTY_LIST_STYLE,
	CSS_PROPERTY_LIST_STYLE_IMAGE,
	CSS_PROPERTY_LIST_STYLE_POSITION,
	CSS_PROPERTY_LIST_STYLE_TYPE,
	CSS_PROPERTY_MARGIN,				// 50
	CSS_PROPERTY_MARGIN_BOTTOM,
	CSS_PROPERTY_MARGIN_LEFT,
	CSS_PROPERTY_MARGIN_RIGHT,
	CSS_PROPERTY_MARGIN_TOP,
	CSS_PROPERTY_OPACITY,
	CSS_PROPERTY_OUTLINE,
	CSS_PROPERTY_OUTLINE_COLOR,
	CSS_PROPERTY_OUTLINE_STYLE,
	CSS_PROPERTY_OUTLINE_WIDTH,
	CSS_PROPERTY_OVERFLOW,		// 60
	CSS_PROPERTY_OVERFLOW_X,
	CSS_PROPERTY_OVERFLOW_Y,
	CSS_PROPERTY_PADDING,
	CSS_PROPERTY_PADDING_BOTTOM,
	CSS_PROPERTY_PADDING_LEFT,
	CSS_PROPERTY_PADDING_RIGHT,
	CSS_PROPERTY_PADDING_TOP,
	CSS_PROPERTY_POSITION,
	CSS_PROPERTY_POSITION_X,
	CSS_PROPERTY_POSITION_Y,		// 70
	CSS_PROPERTY_RIGHT,
	CSS_PROPERTY_TEXT_ALIGN,
	CSS_PROPERTY_TEXT_DECORATION,
	CSS_PROPERTY_TEXT_INDENT,
	CSS_PROPERTY_TEXT_TRANSFORM,
	CSS_PROPERTY_TOP,
	CSS_PROPERTY_VERTICAL_ALIGN,
	CSS_PROPERTY_VISIBILITY,
	CSS_PROPERTY_WHITE_SPACE,
	CSS_PROPERTY_WIDTH,		// 80
	CSS_PROPERTY_WORD_SPACING,
	CSS_PROPERTY_ZINDEX,
	CSS_NUM_PROPERTIES
};
typedef enum e_CSSPropertyType            CSSPropertyType;

enum e_CSSFontStyle
{
	CSS_FONT_STYLE_ITALIC = 0,
	CSS_FONT_STYLE_NORMAL,
	CSS_FONT_STYLE_OBLIQUE
};
typedef enum e_CSSFontStyle               CSSFontStyle;

enum e_CSSFontVariant
{
	CSS_FONT_VARIANT_NORMAL = 0,
	CSS_FONT_VARIANT_SMALL_CAPS
};
typedef enum e_CSSFontVariant             CSSFontVariant;

enum e_CSSFontWeight
{
	CSS_FONT_WEIGHT_100 = 1,
	CSS_FONT_WEIGHT_200,
	CSS_FONT_WEIGHT_300,
	CSS_FONT_WEIGHT_400,
	CSS_FONT_WEIGHT_500,
	CSS_FONT_WEIGHT_600,
	CSS_FONT_WEIGHT_700,
	CSS_FONT_WEIGHT_800,
	CSS_FONT_WEIGHT_900,
	CSS_FONT_WEIGHT_BOLDER,
	CSS_FONT_WEIGHT_LIGHTER,
	CSS_FONT_WEIGHT_SAME
};
typedef enum e_CSSFontWeight              CSSFontWeight;

#define CSS_FONT_WEIGHT_NORMAL	CSS_FONT_WEIGHT_400
#define CSS_FONT_WEIGHT_BOLD	CSS_FONT_WEIGHT_700

enum e_CSSFontSizeType
{
	CSS_FONT_SIZE_NONE = 0,
	CSS_FONT_SIZE_ABSOLUTE,
	CSS_FONT_SIZE_RELATIVE,
	CSS_FONT_SIZE_LENGTH
};
typedef enum e_CSSFontSizeType            CSSFontSizeType;

enum e_CSSLengthUnit
{
	CSS_LENGTH_UNIT_EM = 0,
	CSS_LENGTH_UNIT_EX,
	CSS_LENGTH_UNIT_PX,
	CSS_LENGTH_UNIT_IN,
	CSS_LENGTH_UNIT_CM,
	CSS_LENGTH_UNIT_MM,
	CSS_LENGTH_UNIT_PT,
	CSS_LENGTH_UNIT_PC,
	CSS_LENGTH_UNIT_AUTO,
	CSS_LENGTH_UNIT_PERCENT
};
typedef enum e_CSSLengthUnit              CSSLengthUnit;

enum e_CSSFontSizes
{
	CSS_FONT_SIZE_XXSMALL = 0,
	CSS_FONT_SIZE_XSMALL,
	CSS_FONT_SIZE_SMALL,
	CSS_FONT_SIZE_MEDIUM,
	CSS_FONT_SIZE_LARGE,
	CSS_FONT_SIZE_XLARGE,
	CSS_FONT_SIZE_XXLARGE,
	CSS_FONT_SIZE_SMALLER,
	CSS_FONT_SIZE_LARGER,
	CSS_FONT_SIZE_SAME
};

enum e_CSSBackgroundAttachment
{
	CSS_BACKGROUND_ATTACHMENT_FIXED = 0,
	CSS_BACKGROUND_ATTACHMENT_SCROLL
};
typedef enum e_CSSBackgroundAttachment    CSSBackgroundAttachment;

enum e_CSSBackgroundRepeat
{
	CSS_BACKGROUND_REPEAT_NO_REPEAT = 0,
	CSS_BACKGROUND_REPEAT_REPEAT,
	CSS_BACKGROUND_REPEAT_REPEAT_X,
	CSS_BACKGROUND_REPEAT_REPEAT_Y
};
typedef enum e_CSSBackgroundRepeat        CSSBackgroundRepeat;

enum e_CSSSide
{
	CSS_TOP = 0,
	CSS_RIGHT,
	CSS_BOTTOM,
	CSS_LEFT
};
typedef enum e_CSSSide                    CSSSide;

enum e_CSSDisplay
{
	CSS_DISPLAY_BLOCK = 0,
	CSS_DISPLAY_INLINE,
	CSS_DISPLAY_INLINE_BLOCK,
	CSS_DISPLAY_LIST_ITEM,
	CSS_DISPLAY_NONE
};
typedef enum e_CSSDisplay                 CSSDisplay;

enum e_CSSOverflow
{
	CSS_OVERFLOW_VISIBLE = 0,
	CSS_OVERFLOW_HIDDEN,
	CSS_OVERFLOW_SCROLL,
	CSS_OVERFLOW_AUTO
};
typedef enum e_CSSOverflow                 CSSOverflow;

enum e_CSSVerticalAlign
{
	CSS_VERTICAL_ALIGN_BASELINE = 0,
	CSS_VERTICAL_ALIGN_BOTTOM,
	CSS_VERTICAL_ALIGN_MIDDLE,
	CSS_VERTICAL_ALIGN_SUB,
	CSS_VERTICAL_ALIGN_SUPER,
	CSS_VERTICAL_ALIGN_TEXT_BOTTOM,
	CSS_VERTICAL_ALIGN_TEXT_TOP,
	CSS_VERTICAL_ALIGN_TOP

};
typedef enum e_CSSVerticalAlign           CSSVerticalAlign;

enum e_CSSVisibility
{
	CSS_VISIBILITY_HIDDEN = 0,
	CSS_VISIBILITY_HIDE,
	CSS_VISIBILITY_SHOW,
	CSS_VISIBILITY_VISIBLE
};
typedef enum e_CSSVisibility              CSSVisibility;

// - replace struct CSSLength with a 32-bit signed int (18bits (value, dec) + 10bits (value, frac) + 4bits (type))
struct s_CSSLength
{
	CSSLengthUnit type;
	struct
	{
		short dec; // must be signed
		WEBC_UINT16  frac;	// fractional value of any of the above
	} val;
};

struct s_CSSFontSize
{
	CSSFontSizeType type;
	union
	{
		WEBC_UINT16 absolute;
		WEBC_UINT16 relative;
		CSSLength length;
	} val;
};

enum e_CSSTextDecoration
{
	CSS_TEXT_DECORATION_BLINK = 0,
	CSS_TEXT_DECORATION_LINETHRU,
	CSS_TEXT_DECORATION_NONE,
	CSS_TEXT_DECORATION_OVERLINE,
	CSS_TEXT_DECORATION_UNDERLINE

};
typedef enum e_CSSTextDecoration          CSSTextDecoration;

enum e_CSSFloat
{
	CSS_FLOAT_LEFT = 0,
	CSS_FLOAT_NONE,
	CSS_FLOAT_RIGHT

};
typedef enum e_CSSFloat                   CSSFloat;

enum e_CSSClear
{
	CSS_CLEAR_BOTH = 0,
	CSS_CLEAR_LEFT,
	CSS_CLEAR_NONE,
	CSS_CLEAR_RIGHT
};
typedef enum e_CSSClear                   CSSClear;

enum e_CSSBorderThicknessAbsolute
{
	CSS_BORDER_THICKNESS_THIN = 0,
	CSS_BORDER_THICKNESS_MEDIUM,
	CSS_BORDER_THICKNESS_THICK
};
typedef enum e_CSSBorderThicknessAbsolute CSSBorderThicknessAbsolute;

enum e_CSSPosition
{
	CSS_POSITION_ABSOLUTE = 0,
	CSS_POSITION_FIXED,
	CSS_POSITION_RELATIVE,
	CSS_POSITION_STATIC
};
typedef enum e_CSSPosition                CSSPosition;

enum e_CSSBorderThicknessType
{
	CSS_BORDER_THICKNESS_LENGTH = 0,
	CSS_BORDER_THICKNESS_ABSOLUTE
};
typedef enum e_CSSBorderThicknessType     CSSBorderThicknessType;

struct s_CSSBorderThickness
{
	CSSBorderThicknessType type;
	union
	{
		CSSLength length;
		CSSBorderThicknessAbsolute absolute;
	} val;
};

enum e_CSSListStyleType
{
	CSS_LIST_STYLE_CIRCLE = 0,
	CSS_LIST_STYLE_DECIMAL,
	CSS_LIST_STYLE_DISC,
	CSS_LIST_STYLE_LOWER_ALPHA,
	CSS_LIST_STYLE_LOWER_ROMAN,
	CSS_LIST_STYLE_NONE,
	CSS_LIST_STYLE_SQUARE,
	CSS_LIST_STYLE_UPPER_ALPHA,
	CSS_LIST_STYLE_UPPER_ROMAN
};
typedef enum e_CSSListStyleType           CSSListStyleType;

enum e_CSSTextAlign
{
	CSS_TEXT_ALIGN_CENTER = 0,
	CSS_TEXT_ALIGN_JUSTIFY,
	CSS_TEXT_ALIGN_LEFT,
	CSS_TEXT_ALIGN_RIGHT

};
typedef enum e_CSSTextAlign               CSSTextAlign;

enum e_CSSTextTransform
{
	CSS_TEXT_TRANSFORM_CAPITALIZE = 0,
	CSS_TEXT_TRANSFORM_LOWERCASE,
	CSS_TEXT_TRANSFORM_NONE,
	CSS_TEXT_TRANSFORM_UPPERCASE

};
typedef enum e_CSSTextTransform           CSSTextTransform;

enum e_CSSWhiteSpace
{
	CSS_WHITE_SPACE_NORMAL = 0,
	CSS_WHITE_SPACE_NOWRAP,
	CSS_WHITE_SPACE_PRE

};
typedef enum e_CSSWhiteSpace              CSSWhiteSpace;

enum e_CSSPseudoClass
{
	CSS_PSEUDO_CLASS_UNKNOWN = -1,
	CSS_PSEUDO_CLASS_ACTIVE = 0,
	CSS_PSEUDO_CLASS_HOVER,
	CSS_PSEUDO_CLASS_LINK,
	CSS_PSEUDO_CLASS_FIRST_LETTER,
	CSS_PSEUDO_CLASS_FIRST_LINE,
	CSS_PSEUDO_CLASS_FOCUS,
	CSS_PSEUDO_CLASS_VISITED,
	CSS_PSEUDO_CLASS_NONE
};
typedef enum e_CSSPseudoClass             CSSPseudoClass;

struct s_CSSLineHeight
{
	char type;
	CSSLength length;
};

#define CSS_LINE_HEIGHT_NUMBER	0
#define CSS_LINE_HEIGHT_LENGTH  1

enum e_CSSListStylePosition
{
	CSS_LIST_STYLE_POSITION_INSIDE = 0,
	CSS_LIST_STYLE_POSITION_OUTSIDE
};
typedef enum e_CSSListStylePosition       CSSListStylePosition;

enum e_CSSValueType
{
	CSS_VALUE_UNKNOWN = -1,
	CSS_VALUE_SPECIFIED = 0,
	CSS_VALUE_INHERIT
};
typedef enum e_CSSValueType               CSSValueType;

union u_CSSPropertyValue
{
	const WEBC_CHAR*        url;
	const WEBC_CHAR*        name;
	long                    number;
	unsigned long           count;
	HTMLColor               color;
	CSSFontSize             fontSize;
	CSSLength               length;
	CSSBorderThickness      borderThickness;
	CSSLineHeight           lineHeight;
	CSSListStylePosition    listStylePosition;
	CSSListStyleType        listStyleType;
	CSSDisplay              display;
	CSSOverflow             overflow;
	CSSClear                clear;
	CSSFloat                floatType;
	CSSBorderStyle          borderStyle;
	CSSBackgroundAttachment backgroundAttachment;
	CSSBackgroundRepeat     backgroundRepeat;
	CSSFontVariant          fontVariant;
	CSSFontStyle            fontStyle;
	CSSFontWeight           fontWeight;
	CSSTextDecoration 	    textDecoration;
	CSSTextAlign            textAlign;
	CSSTextTransform        textTransform;
	CSSWhiteSpace           whitespace;
	CSSVerticalAlign        verticalAlign;
	CSSVisibility           visibility;
	CSSPosition             position;
};

#define CSS_PROPERTY_FLAG_IMPORTANT   0x01
#define CSS_PROPERTY_FLAG_INHERIT     0x02
#define CSS_PROPERTY_FLAG_NO_VALUE    0x04

struct CSSProperty
{
	CSSPropertyType  type;
	CSSPropertyValue value;
	WEBC_UINT8 flags;
};


enum CSSOrigin
{
	CSS_ORIGIN_INTERNAL = 0,
	CSS_ORIGIN_EXTERNAL
};

#ifdef __cplusplus
extern "C" {
#endif

int CSS_PropertyToString (
		WEBC_CHAR *str,
		CSSPropertyType type,
		CSSPropertyValue *value,
		unsigned int maxlen
	);

int CSS_LengthToString (
		WEBC_CHAR *str,
		CSSLength *length,
		int maxlen
	);

int CSS_PropEnumToString (
		WEBC_CHAR *str,
		int enumValue,
		const char **enumStrings,
		unsigned enumSize,
		unsigned int maxlen
	);

HTMLLength CSS_LengthToHTML (
		CSSLength* length,
		DISPLAY_INT emHeight,
		DISPLAY_INT exHeight
	);

int CSS_HTMLLengthToCSS (
		CSSLength* cssLen,
		HTMLLength htmlLen
	);

DISPLAY_INT CSS_LengthToPixels (
		CSSLength* length,
		DISPLAY_INT iParentLength,
		DISPLAY_INT iFontHeight,
		DISPLAY_INT iSmallXHeight
	);

DISPLAY_INT CSS_LengthToPoints (
		CSSLength* length,
		DISPLAY_INT iParentLength,
		DISPLAY_INT iFontHeight,
		DISPLAY_INT iSmallXHeight
	);

int CSS_LengthIsRelative (
		CSSLength *pLength
	);

#define CSS_MIN_FONT_SIZE_PT    6
#define CSS_MAX_FONT_SIZE_PT    36
#define CSS_FONT_SIZE_PT_RANGE  31

extern WEBC_READONLY int* giFontSizePointsToAbsolute;
extern WEBC_READONLY int* giFontSizeAbsoluteToPoints;

// the fastest way to multiply by 4/3... I'll never get that half hour back :(
#define TIMES_4_THIRDS_1BIT(B)  (((B)<<1) - (B))
#define TIMES_4_THIRDS_2BIT(B)  (TIMES_4_THIRDS_1BIT(B)  + ((B)>>1))
#define TIMES_4_THIRDS_3BIT(B)  (TIMES_4_THIRDS_2BIT(B)  - ((B)>>2))
#define TIMES_4_THIRDS_4BIT(B)  (TIMES_4_THIRDS_3BIT(B)  + ((B)>>3))
#define TIMES_4_THIRDS_5BIT(B)  (TIMES_4_THIRDS_4BIT(B)  - ((B)>>4))
#define TIMES_4_THIRDS_6BIT(B)  (TIMES_4_THIRDS_5BIT(B)  + ((B)>>5))
#define TIMES_4_THIRDS_7BIT(B)  (TIMES_4_THIRDS_6BIT(B)  - ((B)>>6))
#define TIMES_4_THIRDS_8BIT(B)  (TIMES_4_THIRDS_7BIT(B)  + ((B)>>7))

#define CSS_FontSizePointsToAbsolute(P) (giFontSizePointsToAbsolute[EBSCLIP((P), CSS_MIN_FONT_SIZE_PT, CSS_MAX_FONT_SIZE_PT) - CSS_MIN_FONT_SIZE_PT])
#define CSS_FontSizeAbsoluteToPoints(A) (giFontSizeAbsoluteToPoints[EBSCLIP((A), 0, 6)])
#define CSS_LengthPointsToPixels(P)     TIMES_4_THIRDS_8BIT(P) // P * (4/3)
#define CSS_LengthPixelsToPoints(P)     (((P) + ((P)<<1) + 3) >> 2) // P * (3/4)

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

extern WEBC_READONLY char * gpFontSizeValues[];

extern const int CSS_NUM_FONT_SIZE_VALUES;

#include "strtable.hpp"
#include "enum.hpp"
#include "urldesc.hpp"

#include "hashtbl.hpp"

class CSSStyleElement;
class CSSRule;
class HTMLElement;
class CSSDocumentContext;
class HTMLDocument;
class CSSSelector;
class CSSDocumentParser;

/* set this the max number of pseudo-classes a selector can simultaneously
   specify */
#define CSS_CFG_MAX_PSEUDO_CLASSES     3

class CSSSelector : public HashKey
{
 protected:
	const WEBC_CHAR * id;
	const WEBC_CHAR * className;
	CSSPseudoClass pseudoClass[CSS_CFG_MAX_PSEUDO_CLASSES];
	HTMLTagType tag;
	int referenceCount;

	CSSSelector * child;
	CSSSelector * parent;
	CSSDocumentContext * document;

	void setID(const WEBC_CHAR * i);
	void setClass(const WEBC_CHAR * c);
	void setPseudoClass(CSSPseudoClass pc, int n=0);
	void setTag(HTMLTagType t);
	void setParent(CSSSelector * p);
	void setChild(CSSSelector * p);
	void parseSelector(const WEBC_CHAR * s);

 public:
	CSSSelector(CSSDocumentContext * d, const WEBC_CHAR * p);
	~CSSSelector();

	// HashKey interface
	virtual void Dispose(void);
	virtual WEBC_BOOL Equals(HashKey *key);
	virtual HashCode_t HashCode(void);

	const WEBC_CHAR * getClass(void) ;
	const WEBC_CHAR * getID(void);
	CSSPseudoClass getPseudoClass(int n=0);
	HTMLTagType getTag(void);

	void addReference(void);
	void incReference(void);
	int removeReference(void);

	int elementMatches(CSSStyleElement * element);
	int matchesClassTagParent(CSSStyleElement * element);
	int matchesTagParent(CSSStyleElement * element);
	int matchesParent(CSSStyleElement * element);
	long getSpecificity(void);
	CSSSelector * getParent(void);
	CSSSelector * getChild(void);
};

#if (WEBC_SUPPORT_STYLE_SHEETS)

class CSSRule;

class CSSStyleElement
{
public:

	virtual WEBC_CHAR       * Id(void) = 0;
	virtual const WEBC_CHAR * IdLowercase(void) = 0;
	virtual Enumerator      * GetClasses(void) = 0;
	virtual CSSStyleElement * GetStyleParent(void) = 0;
	virtual int               InPseudoClass(CSSPseudoClass pseudoClass) = 0;
	virtual HTMLTagType       GetTag(void) = 0;

};

class CSSObserver
{
 public:
	virtual void Notify(CSSStyleElement * element) = 0;
 protected:
};

class CSSDocumentParser
{
 protected:
	int GetLength(WEBC_CHAR* string, CSSLength* length);
	int GetBorderThickness(WEBC_CHAR* string, CSSPropertyValue* value);
	unsigned validateMediaType(WEBC_CHAR* mediaType);
	long parseImport(WEBC_CHAR* str, URLDescriptor* sheetUrl);

	CSSDocumentContext* mDocumentContext;
	unsigned            mInComment;
	unsigned            mStarSeen;
	WebString           mParseBuffer;

 public:
	CSSDocumentParser(void);
	~CSSDocumentParser(void);

	int ParseStyleSheet (
			WEBC_CHAR* string,
			CSSOrigin origin,
			URLDescriptor* sheetUrl,
			unsigned lastChunk
		);

	WEBC_CHAR * ParseChunk(WEBC_CHAR * string, CSSRule * rule, CSSOrigin origin, URLDescriptor *sheetUrl);
	WEBC_CHAR * ParseSelectors(WEBC_CHAR * string, CSSRule * rule);
	WEBC_CHAR * ParseDeclarations(WEBC_CHAR * string, CSSRule * rule, CSSOrigin origin, URLDescriptor *sheetUrl);
	WEBC_CHAR * ParseSingleDeclaration(WEBC_CHAR * string, CSSRule * rule, CSSOrigin origin, URLDescriptor *sheetUrl);
	void ParseDeclaration(WEBC_CHAR * name, WEBC_CHAR * value, CSSRule * rule, CSSOrigin origin, URLDescriptor *sheetUrl);
	void ParsePropertyValue(CSSPropertyType propNum, WEBC_CHAR* value, CSSRule* rule, CSSOrigin origin, URLDescriptor* sheetUrl, WEBC_UINT8 important);

	int ParseSingleBorder(WEBC_CHAR * string, CSSRule * rule, CSSPropertyType border, WEBC_UINT8 important = 0);
	int ParseBorderStyle(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important = 0);
	int ParseBorderWidth(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important = 0);
	int ParseBorderColor(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important = 0);
	int ParseBorder(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important = 0);
	int ParseOutline(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important = 0);
	int ParseFont(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important = 0);
	int ParseMargin(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important = 0);
	int ParsePadding(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important = 0);
	int ParseBackground(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important, CSSOrigin origin, URLDescriptor *sheetUrl);
	int ParseBackgroundPosition(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important = 0);
	int ParseListStyle(WEBC_CHAR * string, CSSRule * rule, WEBC_UINT8 important, CSSOrigin origin, URLDescriptor *sheetUrl);

	int ParseColor(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseURL(WEBC_CHAR * string, CSSPropertyValue * value, CSSOrigin origin, URLDescriptor *sheetUrl);
	int ParseSingleBorderStyle(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseSingleBorderWidth(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseTextTransform(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseTextAlignment(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseTextDecoration(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseFontSize(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseFontFamily(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseFontWeight(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseFontVariant(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseFontStyle(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseBackgroundAttachment(WEBC_CHAR * string, CSSPropertyValue * prop);
	int ParseBackgroundRepeat(WEBC_CHAR * string, CSSPropertyValue * prop);
	int ParseBackgroundPositionX(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseBackgroundPositionY(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParsePosition(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseLength(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseWhiteSpace(WEBC_CHAR * string, CSSPropertyValue * prop);
	int ParseListStyleType(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseListStylePosition(WEBC_CHAR * sttring, CSSPropertyValue * value);
	int ParseVerticalAlign(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseLineHeight(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseClear(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseDisplay(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseOverflow(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseFloat(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseNumber(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseOpacity(WEBC_CHAR * string, CSSPropertyValue * value);
	int ParseStringIntoEnum(char ** string, WEBC_CHAR * value, int size, void * enumPointer);
	int ParseVisibility(WEBC_CHAR * string, CSSPropertyValue * value);

	void SetCSSDocumentContext(CSSDocumentContext * p) { mDocumentContext = p; };

	void CleanUp(void);														// call after parsing
};

class CSSRule                  // used by the parser
{
 protected:
	vector mSelectors;
	vector mProperties;

 public:
	CSSRule();
	~CSSRule();
	int addSelector(CSSSelector *s);
	int addProperty(CSSProperty p);
	int addProperty(CSSPropertyType type, CSSPropertyValue value, WEBC_UINT8 important = 0);

	CSSSelector * enumerateFirstSelector(vector_iterator * iter);
	CSSSelector * enumerateNextSelector(vector_iterator * iter);

	CSSProperty * enumerateFirstProperty(vector_iterator * iter);
	CSSProperty * enumerateNextProperty(vector_iterator * iter);

	void clear(void);
	void print(void);
};

class CSSDocumentContext
{
protected:
	enum CSSOrigin mOrigin;
	int miDeleteLock;
	int mbDeleted;
	CSSDocumentParser mParser;

	HashTable mPropertyTable;
	HashTable mSelectorTable;
	StringTable mStringTable;

	long miHighestIdOrder;
	long miHighestClassOrder;
	long miHighestTagOrder;
	long miParseTime;

	ObjectReference<HTMLDocument> mDocument;
#if (WEBC_SUPPORT_JSCRIPT)
	JSObjectRef mJSObject;
#endif	//WEBC_SUPPORT_JSCRIPT

	vector mImportedStyleSheets;

public:
	CSSDocumentContext(HTMLDocument *pDocument);
	virtual ~CSSDocumentContext();

	int ParseStyleSheet (
			WEBC_CHAR* string,
			CSSOrigin origin,
			URLDescriptor* sheetUrl,
			unsigned lastChunk
		);

	CSSValueType GetProperty(CSSStyleElement * element, CSSPropertyType type, CSSPropertyValue * value);
	int ParseElementStyle(CSSStyleElement* element, WEBC_CHAR* string, URLDescriptor* sheetUrl);
	int ParseElementProperty(CSSStyleElement* element, CSSPropertyType type, WEBC_CHAR* value, URLDescriptor* sheetUrl);
	int AddElementStyle(CSSStyleElement* element, CSSRule* rule, URLDescriptor* sheetUrl);
	void RemoveElementStyle(CSSStyleElement* element);
	void CleanUp(void);			// call after parsing to free intermediate buffers
	void Clear(void);
	void AddObserver(CSSObserver * observer);
	void RemoveObserver(CSSObserver * observer);
	int  AddRule(CSSRule *rule, CSSOrigin origin);
	HTMLDocument* GetHTMLDocument() {return mDocument.Get();}
	StringTable * GetStringTable(void) { return &mStringTable; }
	CSSDocumentParser * GetParser(void) { return &mParser; }

#if (WEBC_SUPPORT_JSCRIPT)
	JSObject* GetJSObject(void);
	void SetJSObject(JSObject* pObj);
#endif //(WEBC_SUPPORT_JSCRIPT

	void Import(const WEBC_CHAR* url, URLDescriptor* baseUrl = 0);

	void Destroy (void);
	void Lock    (void);
	int  UnLock  (void);
	int  Deleted (void);

	CSSSelector* newSelector (const WEBC_CHAR* str);
	void deleteSelector (CSSSelector* sel);
};


#endif  // WEBC_SUPPORT_STYLE_SHEETS

#endif 	// __cplusplus
#endif  // (BUILDING_WIGITS_ONLY)
#endif  // __CSS_INCLUDED__
