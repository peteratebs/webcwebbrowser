//
// HTML.C - HTML Parser
//
// EBS - Common Library
//
// Copyright EBS Inc. , 2003
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "html.h"
#include "util.h"
#include "webcstr.h"
#include "webcmem.h"


/*****************************************************************************/
// Macros/Constants
/*****************************************************************************/

#define HTML_INCLUDE_EXTENDED_COLORS

#ifdef HTML_INCLUDE_EXTENDED_COLORS
#define HTML_NUM_COLORS        144
#else
#define HTML_NUM_COLORS        17
#endif

#define HTML_NUM_ESCAPE_CHARS  108
#define HTML_MIN_CHAR_NAME_LEN 2
#define HTML_MAX_CHAR_NAME_LEN 6

/*****************************************************************************/
// Types
/*****************************************************************************/

/*****************************************************************************/
// Function Prototypes
/*****************************************************************************/

static long _HTML_BSearchStringCaseInsensitive (
		const HTML_CHAR* forStr,
		const char** inStrList,
		long forStrLen,
		long arraySize
	);

static long _HTML_BSearchString (
		const HTML_CHAR* forStr,
		const char** inStrList,
		long forStrLen,
		long arraySize
	);

long _HTML_GetCharEntity (
		const HTML_CHAR* srcStr,
		HTML_CHAR* dstChar
	);

/*****************************************************************************/
// Data
/*****************************************************************************/

const char* rHTMLAttributeNames [HTML_NUM_ATTRIBUTES] =
{
	"abbr",
	"accept",
	"accept-charset",
	"accesskey",
	"action",
	"align",
	"alink",
	"alt",
	"archive",
	"axis",
	"background",
	"bgcolor",
	"border",
	"cellpadding",
	"cellspacing",
	"char",
	"charoff",
	"charset",
	"checked",
	"cite",
	"class",
	"classid",
	"clear",
	"code",
	"codebase",
	"codetype",
	"color",
	"cols",
	"colspan",
	"compact",
	"content",
	"coords",
	"data",
	"datetime",
	"declare",
	"defer",
	"dir",
	"disabled",
	"enctype",
	"eventhandler",
	"face",
	"for",
	"frame",
	"frameborder",
	"headers",
	"height",
	"href",
	"hreflang",
	"hspace",
	"http-equiv",
	"id",
	"ismap",
	"label",
	"lang",
	"language",
	"link",
	"longdesc",
	"marginheight",
	"marginwidth",
	"maxlength",
	"media",
	"method",
	"multiple",
	"name",
	"nobackground",
	"nohref",
	"noresize",
	"noshade",
	"notab",
	"nowrap",
	"object",
	"onblur",
	"onchange",
	"onclick",
	"ondblclick",
	"onfocus",
	"onkeydown",
	"onkeypress",
	"onkeyup",
	"onload",
	"onlosecapture",
	"onmousedown",
	"onmousemove",
	"onmouseout",
	"onmouseover",
	"onmouseup",
	"onreset",
	"onselect",
	"onsubmit",
	"onunload",
	"profile",
	"prompt",
	"quarantine",
	"readonly",
	"rel",
	"rev",
	"rows",
	"rowspan",
	"rules",
	"scheme",
	"scope",
	"scrolling",
	"selected",
	"shape",
	"size",
	"span",
	"src",
	"standby",
	"start",
	"style",
	"summary",
	"tabindex",
	"target",
	"text",
	"title",
	"type",
	"usemap",
	"valign",
	"value",
	"valuetype",
	"version",
	"vlink",
	"vspace",
	"width",
	"wrap"
};

const char* rHTMLTagNames [HTML_NUM_ATTRIBUTES] =
{
	"a",
	"abbr",
	"acronym",
	"address",
	"applet",
	"area",
 	"b",
	"base",
	"basefont",
	"bdo",
	"big",
	"blockquote",
	"body",
	"br",
	"button",
	"caption",
	"center",
	"cite",
	"code",
	"col",
	"colgroup",
	"dd",
	"del",
	"dfn",
	"dir",
	"div",
	"dl",
	"dt",
	"em",
#if (WEBC_SUPPORT_EMBED)
	"embed",
#endif
	"fieldset",
	"font",
	"form",
	"frame",
	"frameset",
	"h1",
	"h2",
	"h3",
	"h4",
	"h5",
	"h6",
	"hr",
	"html",
	"i",
	"iframe",
	"img",
	"input",
	"ins",
	"isindex",
	"kbd",
	"label",
	"legend",
	"li",
	"link",
	"map",
	"menu",
	"meta",
	"nobr",
	"noframes",
	"noscript",
	"object",
	"ol",
	"optgroup",
	"option",
	"p",
	"param",
	"pre",
	"q",
	"s",
	"samp",
	"script",
	"select",
	"small",
	"spacer",
	"span",
	"strike",
	"strong",
	"style",
	"sub",
	"sup",
	"table",
	"tbody",
	"td",
	"textarea",
	"tfoot",
	"th",
	"thead",
	"title",
	"tr",
	"tt",
	"u",
	"ul",
	"var",
	"string", // Not a real tag just text
};

const char* rHTMLInputTypeNames [HTML_NUM_INPUT_TYPES] =
{
	"button",
	"checkbox",
	"file",
	"hidden",
	"image",
	"password",
	"radio",
	"reset",
	"submit",
	"text",
	"textarea",
	"option",
	"select",
	"undefined",
};

const char* rHTMLAlignTypeNames [HTML_NUM_ALIGN_TYPES] =
{
	"absbottom",
	"absmiddle",
	"baseline",
	"bottom",
	"center",
	"justify",
	"left",
	"middle",
	"none",
	"right",
	"textbottom",
	"texttop",
	"top"
};

const char* rHTMLVAlignTypeNames [HTML_NUM_VALIGN_TYPES] =
{
	"bottom",
	"center",
	"middle",
	"none",
	"top"
};

const char* rHTMLShapeTypeNames [HTML_NUM_SHAPE_TYPES] =
{
	"circle",
	"poly",
	"rect"
};

const char* rHTMLTableFrameTypeNames [HTML_NUM_TABLEFRAME_TYPES] =
{
"above",
"below",
"border",
"box",
"hsides",
"lhs",
"rhs",
"void",
"vsides"
};

const char* rHTMLEscapeCharNames [HTML_NUM_ESCAPE_CHARS] =
{
	"AElig",   // 198: capital AE diphthong (ligature)
	"Aacute",  // 193: capital A, acute accent
	"Acirc",   // 194: capital A, circumflex accent
	"Agrave",  // 192: capital A, grave accent
	"Aring",   // 197: capital A, ring
	"Atilde",  // 195: capital A, tilde
	"Auml",    // 196: capital A, dieresis or umlaut mark
	"Ccedil",  // 199: capital C, cedilla
	"ETH",     // 208: capital Eth, Icelandic
	"Eacute",  // 201: capital E, acute accent
	"Ecirc",   // 202: capital E, circumflex accent
	"Egrave",  // 200: capital E, grave accent
	"Euml",    // 203: capital E, dieresis or umlaut mark
	"Iacute",  // 205: capital I, acute accent
	"Icirc",   // 206: capital I, circumflex accent
	"Igrave",  // 204: capital I, grave accent
	"Iuml",    // 207: capital I, dieresis or umlaut mark
	"Ntilde",  // 209: capital N, tilde
	"Oacute",  // 211: capital O, acute accent
	"Ocirc",   // 212: capital O, circumflex accent
	"Ograve",  // 210: capital O, grave accent
	"Oslash",  // 216: capital O, slash
	"Otilde",  // 213: capital O, tilde
	"Ouml",    // 214: capital O, dieresis or umlaut mark
	"THORN",   // 222: capital THORN, Icelandic
	"Uacute",  // 218: capital U, acute accent
	"Ucirc",   // 219: capital U, circumflex accent
	"Ugrave",  // 217: capital U, grave accent
	"Uuml",    // 220: capital U, dieresis or umlaut mark
	"Yacute",  // 221: capital Y, acute accent
	"aacute",  // 225: small a, acute accent
	"acirc",   // 226: small a, circumflex accent
	"acute",   // 180: acute accent
	"aelig",   // 230: small ae diphthong (ligature)
	"agrave",  // 224: small a, grave accent
	"amp",     // 038: ampersand
	"aring",   // 229: small a, ring
	"atilde",  // 227: small a, tilde
	"auml",    // 228: small a, dieresis or umlaut mark
	"bdquo",   // trademark
	"brvbar",  // 166: broken (vertical) bar
	"ccedil",  // 231: small c, cedilla
	"cedil",   // 184: cedilla
	"cent",    // 162: cent sign
	"copy",    // 169: copyright sign
	"curren",  // 164: general currency sign
	"dbquo",   // double bottom quote?
	"deg",     // 176: degree sign
	"divide",  // 247: divide sign
	"eacute",  // 233: small e, acute accent
	"ecirc",   // 234: small e, circumflex accent
	"egrave",  // 232: small e, grave accent
	"eth",     // 240: small eth, Icelandic
	"euml",    // 235: small e, dieresis or umlaut mark
	"frac12",  // 189: fraction one-half
	"frac14",  // 188: fraction one-quarter
	"frac34",  // 190: fraction three-quarters
	"gt",      // 062: greater than
	"iacute",  // 237: small i, acute accent
	"icirc",   // 238: small i, circumflex accent
	"iexcl",   // 161: inverted exclamation mark
	"igrave",  // 236: small i, grave accent
	"iquest",  // 191: inverted question mark
	"iuml",    // 239: small i, dieresis or umlaut mark
	"laquo",   // 171: angle quotation mark, left
	"lsquo",   // left single quote
	"lt",      // 060: less than
	"macr",    // 175: macron
	"mdash",   // 151: Em dash
	"micro",   // 181: micro sign
	"middot",  // 183: middle dot
	"nbsp",    // 160: non-breaking space
	"ndash",   // 150: En dash
	"not",     // 172: not sign
	"ntilde",  // 241: small n, tilde
	"oacute",  // 243: small o, acute accent
	"ocirc",   // 244: small o, circumflex accent
	"ograve",  // 242: small o, grave accent
	"ordf",    // 170: ordinal indicator, feminine
	"ordm",    // 186: ordinal indicator, masculine
	"oslash",  // 248: small o, slash
	"otilde",  // 245: small o, tilde
	"ouml",    // 246: small o, dieresis or umlaut mark
	"para",    // 182: pilcrow (paragraph sign)
	"plusmn",  // 177: plus-or-minus sign
	"pound",   // 163: pound sterling sign
	"quot",    // 034: quotation mark
	"raquo",   // 187: angle quotation mark, right
	"reg",     // 174: registered sign
	"rsquo",   // right single quote
	"sbquo",   // single bottom quote?
	"sect",    // 167: section sign
	"shy",     // 173: soft hyphen
	"sup1",    // 185: superscript one
	"sup2",    // 178: superscript two
	"sup3",    // 179: superscript three
	"szlig",   // 223: small sharp s, German (sz ligature)
	"thorn",   // 254: small thorn, Icelandic
	"times",   // 215: multiply sign
	"trade",   // trademark
	"uacute",  // 250: small u, acute accent
	"ucirc",   // 251: small u, circumflex accent
	"ugrave",  // 249: small u, grave accent
	"uml",     // 168: umlaut (dieresis)
	"uuml",    // 252: small u, dieresis or umlaut mark
	"yacute",  // 253: small y, acute accent
	"yen",     // 165: yen sign
	"yuml"     // 255: small y, dieresis or umlaut mark
};

const HTML_CHAR rHTMLEscapeChars [HTML_NUM_ESCAPE_CHARS] =
{
	'\xc6',          // 198: capital AE diphthong (ligature)
	'\xc1',          // 193: capital A, acute accent
	'\xc2',          // 194: capital A, circumflex accent
	'\xc0',          // 192: capital A, grave accent
	'\xc5',          // 197: capital A, ring
	'\xc3',          // 195: capital A, tilde
	'\xc4',          // 196: capital A, dieresis or umlaut mark
	'\xc7',          // 199: capital C, cedilla
	'\xd0',          // 208: capital Eth, Icelandic
	'\xc9',          // 201: capital E, acute accent
	'\xca',          // 202: capital E, circumflex accent
	'\xc8',          // 200: capital E, grave accent
	'\xcb',          // 203: capital E, dieresis or umlaut mark
	'\xcd',          // 205: capital I, acute accent
	'\xce',          // 206: capital I, circumflex accent
	'\xcc',          // 204: capital I, grave accent
	'\xcf',          // 207: capital I, dieresis or umlaut mark
	'\xd1',          // 209: capital N, tilde
	'\xd3',          // 211: capital O, acute accent
	'\xd4',          // 212: capital O, circumflex accent
	'\xd2',          // 210: capital O, grave accent
	'\xd8',          // 216: capital O, slash
	'\xd5',          // 213: capital O, tilde
	'\xd6',          // 214: capital O, dieresis or umlaut mark
	'\xde',          // 222: capital THORN, Icelandic
	'\xda',          // 218: capital U, acute accent
	'\xdb',          // 219: capital U, circumflex accent
	'\xd9',          // 217: capital U, grave accent
	'\xdc',          // 220: capital U, dieresis or umlaut mark
	'\xdd',          // 221: capital Y, acute accent
	'\xe1',          // 225: small a, acute accent
	'\xe2',          // 226: small a, circumflex accent
	'\xb4',          // 180: acute accent
	'\xe6',          // 230: small ae diphthong (ligature)
	'\xe0',          // 224: small a, grave accent
	'&',             // 038: ampersand
	'\xe5',          // 229: small a, ring
	'\xe3',          // 227: small a, tilde
	'\xe4',          // 228: small a, dieresis or umlaut mark
	'\x99',          // trademark
	'\xa6',          // 166: broken (vertical) bar
	'\xe7',          // 231: small c, cedilla
	'\xb8',          // 184: cedilla
	'\xa2',          // 162: cent sign
	'\xa9',          // 169: copyright sign
	'\xa4',          // 164: general currency sign
	'\"',            // double bottom quote?
	'\xb0',          // 176: degree sign
	'\xf7',          // 247: divide sign
	'\xe9',          // 233: small e, acute accent
	'\xea',          // 234: small e, circumflex accent
	'\xe8',          // 232: small e, grave accent
	'\xf0',          // 240: small eth, Icelandic
	'\xeb',          // 235: small e, dieresis or umlaut mark
	'\xbd',          // 189: fraction one-half
	'\xbc',          // 188: fraction one-quarter
	'\xbe',          // 190: fraction three-quarters
	'>',             // 062: greater than
	'\xed',          // 237: small i, acute accent
	'\xee',          // 238: small i, circumflex accent
	'\xa1',          // 161: inverted exclamation mark
	'\xec',          // 236: small i, grave accent
	'\xbf',          // 191: inverted question mark
	'\xef',          // 239: small i, dieresis or umlaut mark
	'\xab',          // 171: angle quotation mark, left
	'\'',            // left single quote
	'<',             // 060: less than
	'\xaf',          // 175: macron
	'-',             // 151: Em dash
	'\xb5',          // 181: micro sign
	'\xb7',          // 183: middle dot
	HTML_NBSP_CHAR,  // 160: non-breaking
	'-',             // 150: En dash
	'\xac',          // 172: not sign
	'\xf1',          // 241: small n, tilde
	'\xf3',          // 243: small o, acute accent
	'\xf4',          // 244: small o, circumflex accent
	'\xf2',          // 242: small o, grave accent
	'\xaa',          // 170: ordinal indicator, feminine
	'\xba',          // 186: ordinal indicator, masculine
	'\xf8',          // 248: small o, slash
	'\xf5',          // 245: small o, tilde
	'\xf6',          // 246: small o, dieresis or umlaut mark
	'\xb6',          // 182: pilcrow (paragraph sign)
	'\xb1',          // 177: plus-or-minus sign
	'\xa3',          // 163: pound sterling sign
	'\"',            // 034: quotation mark
	'\xbb',          // 187: angle quotation mark, right
	'\xae',          // 174: registered sign
	'\'',            // right single quote
	'\'',            // single bottom quote?
	'\xa7',          // 167: section sign
	'\xad',          // 173: soft hyphen
	'\xb9',          // 185: superscript one
	'\xb2',          // 178: superscript two
	'\xb3',          // 179: superscript three
	'\xdf',          // 223: small sharp s, German (sz ligature)
	'\xfe',          // 254: small thorn, Icelandic
	'\xd7',          // 215: multiply sign
	'\x99',          // trademark
	'\xfa',          // 250: small u, acute accent
	'\xfb',          // 251: small u, circumflex accent
	'\xf9',          // 249: small u, grave accent
	'\xa8',          // 168: umlaut (dieresis)
	'\xfc',          // 252: small u, dieresis or umlaut mark
	'\xfd',          // 253: small y, acute accent
	'\xa5',          // 165: yen sign
	'\xff'           // 255: small y, dieresis or umlaut mark
};

#ifdef HTML_INCLUDE_EXTENDED_COLORS
const char* rHTMLColorNames [HTML_NUM_COLORS] =
{
	"AliceBlue",
	"AntiqueWhite",
	"Aqua",
	"Aquamarine",
	"Azure",
	"Beige",
	"Bisque",
	"Black",
	"BlanchedAlmond",
	"Blue",
	"BlueViolet",
	"Brown",
	"BurlyWood",
	"CadetBlue",
	"Chartreuse",
	"Chocolate",
	"Coral",
	"CornflowerBlue",
	"Cornsilk",
	"Crimson",
	"Cyan",
	"DarkBlue",
	"DarkCyan",
	"DarkGoldenRod",
	"DarkGray",
	"DarkGreen",
	"DarkKhaki",
	"DarkMagenta",
	"DarkOliveGreen",
	"Darkorange",
	"DarkOrchid",
	"DarkRed",
	"DarkSalmon",
	"DarkSeaGreen",
	"DarkSlateBlue",
	"DarkSlateGray",
	"DarkTurquoise",
	"DarkViolet",
	"DeepPink",
	"DeepSkyBlue",
	"DimGray",
	"DodgerBlue",
	"Feldspar",
	"FireBrick",
	"FloralWhite",
	"ForestGreen",
	"Fuchsia",
	"Gainsboro",
	"GhostWhite",
	"Gold",
	"GoldenRod",
	"Gray",
	"Green",
	"GreenYellow",
	"HoneyDew",
	"HotPink",
	"IndianRed",
	"Indigo",
	"Ivory",
	"Khaki",
	"Lavender",
	"LavenderBlush",
	"LawnGreen",
	"LemonChiffon",
	"LightBlue",
	"LightCoral",
	"LightCyan",
	"LightGoldenRodYellow",
	"LightGrey",
	"LightGreen",
	"LightPink",
	"LightSalmon",
	"LightSeaGreen",
	"LightSkyBlue",
	"LightSlateBlue",
	"LightSlateGray",
	"LightSteelBlue",
	"LightYellow",
	"Lime",
	"LimeGreen",
	"Linen",
	"Magenta",
	"Maroon",
	"MediumAquaMarine",
	"MediumBlue",
	"MediumOrchid",
	"MediumPurple",
	"MediumSeaGreen",
	"MediumSlateBlue",
	"MediumSpringGreen",
	"MediumTurquoise",
	"MediumVioletRed",
	"MidnightBlue",
	"MintCream",
	"MistyRose",
	"Moccasin",
	"NavajoWhite",
	"Navy",
	"OldLace",
	"Olive",
	"OliveDrab",
	"Orange",
	"OrangeRed",
	"Orchid",
	"PaleGoldenRod",
	"PaleGreen",
	"PaleTurquoise",
	"PaleVioletRed",
	"PapayaWhip",
	"PeachPuff",
	"Peru",
	"Pink",
	"Plum",
	"PowderBlue",
	"Purple",
	"Red",
	"RosyBrown",
	"RoyalBlue",
	"SaddleBrown",
	"Salmon",
	"SandyBrown",
	"SeaGreen",
	"SeaShell",
	"Sienna",
	"Silver",
	"SkyBlue",
	"SlateBlue",
	"SlateGray",
	"Snow",
	"SpringGreen",
	"SteelBlue",
	"Tan",
	"Teal",
	"Thistle",
	"Tomato",
	"Transparent",
	"Turquoise",
	"Violet",
	"VioletRed",
	"Wheat",
	"White",
	"WhiteSmoke",
	"Yellow",
	"YellowGreen"
};

const HTMLColor rHTMLColorValues [HTML_NUM_COLORS] =
{
	HTML_RGBAToColor(0xF0, 0xF8, 0xFF, 0x00), // AliceBlue
	HTML_RGBAToColor(0xFA, 0xEB, 0xD7, 0x00), // AntiqueWhite
	HTML_RGBAToColor(0x00, 0xFF, 0xFF, 0x00), // Aqua
	HTML_RGBAToColor(0x7F, 0xFF, 0xD4, 0x00), // Aquamarine
	HTML_RGBAToColor(0xF0, 0xFF, 0xFF, 0x00), // Azure
	HTML_RGBAToColor(0xF5, 0xF5, 0xDC, 0x00), // Beige
	HTML_RGBAToColor(0xFF, 0xE4, 0xC4, 0x00), // Bisque
	HTML_RGBAToColor(0x00, 0x00, 0x00, 0x00), // Black
	HTML_RGBAToColor(0xFF, 0xEB, 0xCD, 0x00), // BlanchedAlmond
	HTML_RGBAToColor(0x00, 0x00, 0xFF, 0x00), // Blue
	HTML_RGBAToColor(0x8A, 0x2B, 0xE2, 0x00), // BlueViolet
	HTML_RGBAToColor(0xA5, 0x2A, 0x2A, 0x00), // Brown
	HTML_RGBAToColor(0xDE, 0xB8, 0x87, 0x00), // BurlyWood
	HTML_RGBAToColor(0x5F, 0x9E, 0xA0, 0x00), // CadetBlue
	HTML_RGBAToColor(0x7F, 0xFF, 0x00, 0x00), // Chartreuse
	HTML_RGBAToColor(0xD2, 0x69, 0x1E, 0x00), // Chocolate
	HTML_RGBAToColor(0xFF, 0x7F, 0x50, 0x00), // Coral
	HTML_RGBAToColor(0x64, 0x95, 0xED, 0x00), // CornflowerBlue
	HTML_RGBAToColor(0xFF, 0xF8, 0xDC, 0x00), // Cornsilk
	HTML_RGBAToColor(0xDC, 0x14, 0x3C, 0x00), // Crimson
	HTML_RGBAToColor(0x00, 0xFF, 0xFF, 0x00), // Cyan
	HTML_RGBAToColor(0x00, 0x00, 0x8B, 0x00), // DarkBlue
	HTML_RGBAToColor(0x00, 0x8B, 0x8B, 0x00), // DarkCyan
	HTML_RGBAToColor(0xB8, 0x86, 0x0B, 0x00), // DarkGoldenRod
	HTML_RGBAToColor(0xA9, 0xA9, 0xA9, 0x00), // DarkGray
	HTML_RGBAToColor(0x00, 0x64, 0x00, 0x00), // DarkGreen
	HTML_RGBAToColor(0xBD, 0xB7, 0x6B, 0x00), // DarkKhaki
	HTML_RGBAToColor(0x8B, 0x00, 0x8B, 0x00), // DarkMagenta
	HTML_RGBAToColor(0x55, 0x6B, 0x2F, 0x00), // DarkOliveGreen
	HTML_RGBAToColor(0xFF, 0x8C, 0x00, 0x00), // Darkorange
	HTML_RGBAToColor(0x99, 0x32, 0xCC, 0x00), // DarkOrchid
	HTML_RGBAToColor(0x8B, 0x00, 0x00, 0x00), // DarkRed
	HTML_RGBAToColor(0xE9, 0x96, 0x7A, 0x00), // DarkSalmon
	HTML_RGBAToColor(0x8F, 0xBC, 0x8F, 0x00), // DarkSeaGreen
	HTML_RGBAToColor(0x48, 0x3D, 0x8B, 0x00), // DarkSlateBlue
	HTML_RGBAToColor(0x2F, 0x4F, 0x4F, 0x00), // DarkSlateGray
	HTML_RGBAToColor(0x00, 0xCE, 0xD1, 0x00), // DarkTurquoise
	HTML_RGBAToColor(0x94, 0x00, 0xD3, 0x00), // DarkViolet
	HTML_RGBAToColor(0xFF, 0x14, 0x93, 0x00), // DeepPink
	HTML_RGBAToColor(0x00, 0xBF, 0xFF, 0x00), // DeepSkyBlue
	HTML_RGBAToColor(0x69, 0x69, 0x69, 0x00), // DimGray
	HTML_RGBAToColor(0x1E, 0x90, 0xFF, 0x00), // DodgerBlue
	HTML_RGBAToColor(0xD1, 0x92, 0x75, 0x00), // Feldspar
	HTML_RGBAToColor(0xB2, 0x22, 0x22, 0x00), // FireBrick
	HTML_RGBAToColor(0xFF, 0xFA, 0xF0, 0x00), // FloralWhite
	HTML_RGBAToColor(0x22, 0x8B, 0x22, 0x00), // ForestGreen
	HTML_RGBAToColor(0xFF, 0x00, 0xFF, 0x00), // Fuchsia
	HTML_RGBAToColor(0xDC, 0xDC, 0xDC, 0x00), // Gainsboro
	HTML_RGBAToColor(0xF8, 0xF8, 0xFF, 0x00), // GhostWhite
	HTML_RGBAToColor(0xFF, 0xD7, 0x00, 0x00), // Gold
	HTML_RGBAToColor(0xDA, 0xA5, 0x20, 0x00), // GoldenRod
	HTML_RGBAToColor(0x80, 0x80, 0x80, 0x00), // Gray
	HTML_RGBAToColor(0x00, 0x80, 0x00, 0x00), // Green
	HTML_RGBAToColor(0xAD, 0xFF, 0x2F, 0x00), // GreenYellow
	HTML_RGBAToColor(0xF0, 0xFF, 0xF0, 0x00), // HoneyDew
	HTML_RGBAToColor(0xFF, 0x69, 0xB4, 0x00), // HotPink
	HTML_RGBAToColor(0xCD, 0x5C, 0x5C, 0x00), // IndianRed
	HTML_RGBAToColor(0x4B, 0x00, 0x82, 0x00), // Indigo
	HTML_RGBAToColor(0xFF, 0xFF, 0xF0, 0x00), // Ivory
	HTML_RGBAToColor(0xF0, 0xE6, 0x8C, 0x00), // Khaki
	HTML_RGBAToColor(0xE6, 0xE6, 0xFA, 0x00), // Lavender
	HTML_RGBAToColor(0xFF, 0xF0, 0xF5, 0x00), // LavenderBlush
	HTML_RGBAToColor(0x7C, 0xFC, 0x00, 0x00), // LawnGreen
	HTML_RGBAToColor(0xFF, 0xFA, 0xCD, 0x00), // LemonChiffon
	HTML_RGBAToColor(0xAD, 0xD8, 0xE6, 0x00), // LightBlue
	HTML_RGBAToColor(0xF0, 0x80, 0x80, 0x00), // LightCoral
	HTML_RGBAToColor(0xE0, 0xFF, 0xFF, 0x00), // LightCyan
	HTML_RGBAToColor(0xFA, 0xFA, 0xD2, 0x00), // LightGoldenRodYellow
	HTML_RGBAToColor(0xD3, 0xD3, 0xD3, 0x00), // LightGrey
	HTML_RGBAToColor(0x90, 0xEE, 0x90, 0x00), // LightGreen
	HTML_RGBAToColor(0xFF, 0xB6, 0xC1, 0x00), // LightPink
	HTML_RGBAToColor(0xFF, 0xA0, 0x7A, 0x00), // LightSalmon
	HTML_RGBAToColor(0x20, 0xB2, 0xAA, 0x00), // LightSeaGreen
	HTML_RGBAToColor(0x87, 0xCE, 0xFA, 0x00), // LightSkyBlue
	HTML_RGBAToColor(0x84, 0x70, 0xFF, 0x00), // LightSlateBlue
	HTML_RGBAToColor(0x77, 0x88, 0x99, 0x00), // LightSlateGray
	HTML_RGBAToColor(0xB0, 0xC4, 0xDE, 0x00), // LightSteelBlue
	HTML_RGBAToColor(0xFF, 0xFF, 0xE0, 0x00), // LightYellow
	HTML_RGBAToColor(0x00, 0xFF, 0x00, 0x00), // Lime
	HTML_RGBAToColor(0x32, 0xCD, 0x32, 0x00), // LimeGreen
	HTML_RGBAToColor(0xFA, 0xF0, 0xE6, 0x00), // Linen
	HTML_RGBAToColor(0xFF, 0x00, 0xFF, 0x00), // Magenta
	HTML_RGBAToColor(0x80, 0x00, 0x00, 0x00), // Maroon
	HTML_RGBAToColor(0x66, 0xCD, 0xAA, 0x00), // MediumAquaMarine
	HTML_RGBAToColor(0x00, 0x00, 0xCD, 0x00), // MediumBlue
	HTML_RGBAToColor(0xBA, 0x55, 0xD3, 0x00), // MediumOrchid
	HTML_RGBAToColor(0x93, 0x70, 0xD8, 0x00), // MediumPurple
	HTML_RGBAToColor(0x3C, 0xB3, 0x71, 0x00), // MediumSeaGreen
	HTML_RGBAToColor(0x7B, 0x68, 0xEE, 0x00), // MediumSlateBlue
	HTML_RGBAToColor(0x00, 0xFA, 0x9A, 0x00), // MediumSpringGreen
	HTML_RGBAToColor(0x48, 0xD1, 0xCC, 0x00), // MediumTurquoise
	HTML_RGBAToColor(0xC7, 0x15, 0x85, 0x00), // MediumVioletRed
	HTML_RGBAToColor(0x19, 0x19, 0x70, 0x00), // MidnightBlue
	HTML_RGBAToColor(0xF5, 0xFF, 0xFA, 0x00), // MintCream
	HTML_RGBAToColor(0xFF, 0xE4, 0xE1, 0x00), // MistyRose
	HTML_RGBAToColor(0xFF, 0xE4, 0xB5, 0x00), // Moccasin
	HTML_RGBAToColor(0xFF, 0xDE, 0xAD, 0x00), // NavajoWhite
	HTML_RGBAToColor(0x00, 0x00, 0x80, 0x00), // Navy
	HTML_RGBAToColor(0xFD, 0xF5, 0xE6, 0x00), // OldLace
	HTML_RGBAToColor(0x80, 0x80, 0x00, 0x00), // Olive
	HTML_RGBAToColor(0x6B, 0x8E, 0x23, 0x00), // OliveDrab
	HTML_RGBAToColor(0xFF, 0xA5, 0x00, 0x00), // Orange
	HTML_RGBAToColor(0xFF, 0x45, 0x00, 0x00), // OrangeRed
	HTML_RGBAToColor(0xDA, 0x70, 0xD6, 0x00), // Orchid
	HTML_RGBAToColor(0xEE, 0xE8, 0xAA, 0x00), // PaleGoldenRod
	HTML_RGBAToColor(0x98, 0xFB, 0x98, 0x00), // PaleGreen
	HTML_RGBAToColor(0xAF, 0xEE, 0xEE, 0x00), // PaleTurquoise
	HTML_RGBAToColor(0xD8, 0x70, 0x93, 0x00), // PaleVioletRed
	HTML_RGBAToColor(0xFF, 0xEF, 0xD5, 0x00), // PapayaWhip
	HTML_RGBAToColor(0xFF, 0xDA, 0xB9, 0x00), // PeachPuff
	HTML_RGBAToColor(0xCD, 0x85, 0x3F, 0x00), // Peru
	HTML_RGBAToColor(0xFF, 0xC0, 0xCB, 0x00), // Pink
	HTML_RGBAToColor(0xDD, 0xA0, 0xDD, 0x00), // Plum
	HTML_RGBAToColor(0xB0, 0xE0, 0xE6, 0x00), // PowderBlue
	HTML_RGBAToColor(0x80, 0x00, 0x80, 0x00), // Purple
	HTML_RGBAToColor(0xFF, 0x00, 0x00, 0x00), // Red
	HTML_RGBAToColor(0xBC, 0x8F, 0x8F, 0x00), // RosyBrown
	HTML_RGBAToColor(0x41, 0x69, 0xE1, 0x00), // RoyalBlue
	HTML_RGBAToColor(0x8B, 0x45, 0x13, 0x00), // SaddleBrown
	HTML_RGBAToColor(0xFA, 0x80, 0x72, 0x00), // Salmon
	HTML_RGBAToColor(0xF4, 0xA4, 0x60, 0x00), // SandyBrown
	HTML_RGBAToColor(0x2E, 0x8B, 0x57, 0x00), // SeaGreen
	HTML_RGBAToColor(0xFF, 0xF5, 0xEE, 0x00), // SeaShell
	HTML_RGBAToColor(0xA0, 0x52, 0x2D, 0x00), // Sienna
	HTML_RGBAToColor(0xC0, 0xC0, 0xC0, 0x00), // Silver
	HTML_RGBAToColor(0x87, 0xCE, 0xEB, 0x00), // SkyBlue
	HTML_RGBAToColor(0x6A, 0x5A, 0xCD, 0x00), // SlateBlue
	HTML_RGBAToColor(0x70, 0x80, 0x90, 0x00), // SlateGray
	HTML_RGBAToColor(0xFF, 0xFA, 0xFA, 0x00), // Snow
	HTML_RGBAToColor(0x00, 0xFF, 0x7F, 0x00), // SpringGreen
	HTML_RGBAToColor(0x46, 0x82, 0xB4, 0x00), // SteelBlue
	HTML_RGBAToColor(0xD2, 0xB4, 0x8C, 0x00), // Tan
	HTML_RGBAToColor(0x00, 0x80, 0x80, 0x00), // Teal
	HTML_RGBAToColor(0xD8, 0xBF, 0xD8, 0x00), // Thistle
	HTML_RGBAToColor(0xFF, 0x63, 0x47, 0x00), // Tomato
	HTML_RGBAToColor(0x00, 0x00, 0x00, 0xFF), // Transparent
	HTML_RGBAToColor(0x40, 0xE0, 0xD0, 0x00), // Turquoise
	HTML_RGBAToColor(0xEE, 0x82, 0xEE, 0x00), // Violet
	HTML_RGBAToColor(0xD0, 0x20, 0x90, 0x00), // VioletRed
	HTML_RGBAToColor(0xF5, 0xDE, 0xB3, 0x00), // Wheat
	HTML_RGBAToColor(0xFF, 0xFF, 0xFF, 0x00), // White
	HTML_RGBAToColor(0xF5, 0xF5, 0xF5, 0x00), // WhiteSmoke
	HTML_RGBAToColor(0xFF, 0xFF, 0x00, 0x00), // Yellow
	HTML_RGBAToColor(0x9A, 0xCD, 0x32, 0x00)  // YellowGreen
};

#else
const char* rHTMLColorNames [HTML_NUM_COLORS] =
{
	"aqua",
	"black",
	"blue",
	"fuchsia",
	"gray",
	"green",
	"lime",
	"maroon",
	"navy",
	"olive",
	"purple",
	"red",
	"silver",
	"teal",
	"transparent",
	"white",
	"yellow"
};

const HTMLColor rHTMLColorValues [HTML_NUM_COLORS] =
{
	HTML_RGBAToColor(0x00, 0xff, 0xff, 0x00), // aqua
	HTML_RGBAToColor(0x00, 0x00, 0x00, 0x00), // black
	HTML_RGBAToColor(0x00, 0x00, 0xFF, 0x00), // blue
	HTML_RGBAToColor(0xFF, 0x00, 0xFF, 0x00), // fuchsia
	HTML_RGBAToColor(0x80, 0x80, 0x80, 0x00), // gray
	HTML_RGBAToColor(0x00, 0x80, 0x00, 0x00), // green
	HTML_RGBAToColor(0x40, 0xFF, 0x40, 0x00), // lime
	HTML_RGBAToColor(0x80, 0x00, 0x00, 0x00), // maroon
	HTML_RGBAToColor(0x00, 0x00, 0x60, 0x00), // navy
	HTML_RGBAToColor(0x55, 0x80, 0x00, 0x00), // olive
	HTML_RGBAToColor(0x80, 0x00, 0x80, 0x00), // purple
	HTML_RGBAToColor(0xFF, 0x00, 0x00, 0x00), // red
	HTML_RGBAToColor(0xC0, 0xC0, 0xC0, 0x00), // silver
	HTML_RGBAToColor(0x00, 0x99, 0x99, 0x00), // teal
	HTML_RGBAToColor(0x00, 0x00, 0x00, 0xFF), // transparent
	HTML_RGBAToColor(0xFF, 0xFF, 0xFF, 0x00), // white
	HTML_RGBAToColor(0xFF, 0xFF, 0x00, 0x00)  // yellow
};
#endif // HTML_INCLUDE_EXTENDED_COLORS

#if (WEBC_SUPPORT_UNICODE)
WEBC_READONLY WEBC_CHAR16 rHTMLExtendedCharEntityToUC [32] =
{
	0x20AC,  // CHAR INDEX = 80, EURO SIGN
	0xFEFF,  // CHAR INDEX = 81,
	0x201A,  // CHAR INDEX = 82, SINGLE LOW-9 QUOTATION MARK
	0x0192,  // CHAR INDEX = 83, LATIN SMALL LETTER F WITH HOOK
	0x201E,  // CHAR INDEX = 84, DOUBLE LOW-9 QUOTATION MARK
	0x2026,  // CHAR INDEX = 85, HORIZONTAL ELLIPSIS
	0x2020,  // CHAR INDEX = 86, DAGGER
	0x2021,  // CHAR INDEX = 87, DOUBLE DAGGER
	0x02C6,  // CHAR INDEX = 88, MODIFIER LETTER CIRCUMFLEX ACCENT
	0x2030,  // CHAR INDEX = 89, PER MILLE SIGN
	0x0160,  // CHAR INDEX = 8A, LATIN CAPITAL LETTER S WITH CARON
	0x2039,  // CHAR INDEX = 8B, SINGLE LEFT-POINTING ANGLE QUOTATION MARK
	0x0152,  // CHAR INDEX = 8C, LATIN CAPITAL LIGATURE OE
	0xFEFF,  // CHAR INDEX = 8D,
	0x017D,  // CHAR INDEX = 8E, LATIN CAPITAL LETTER Z WITH CARON
	0xFEFF,  // CHAR INDEX = 8F,
	0xFEFF,  // CHAR INDEX = 90,
	0x2018,  // CHAR INDEX = 91, LEFT SINGLE QUOTATION MARK
	0x2019,  // CHAR INDEX = 92, RIGHT SINGLE QUOTATION MARK
	0x201C,  // CHAR INDEX = 93, LEFT DOUBLE QUOTATION MARK
	0x201D,  // CHAR INDEX = 94, RIGHT DOUBLE QUOTATION MARK
	0x2022,  // CHAR INDEX = 95, BULLET
	0x2013,  // CHAR INDEX = 96, EN DASH
	0x2014,  // CHAR INDEX = 97, EM DASH
	0x02DC,  // CHAR INDEX = 98, SMALL TILDE
	0x2122,  // CHAR INDEX = 99, TRADE MARK SIGN
	0x0161,  // CHAR INDEX = 9A, LATIN SMALL LETTER S WITH CARON
	0x203A,  // CHAR INDEX = 9B, SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
	0x0153,  // CHAR INDEX = 9C, LATIN SMALL LIGATURE OE
	0xFEFF,  // CHAR INDEX = 9D,
	0x017E,  // CHAR INDEX = 9E, LATIN SMALL LETTER Z WITH CARON
	0x0178   // CHAR INDEX = 9F, LATIN CAPITAL LETTER Y WITH DIAERESIS
};
#endif // WEBC_SUPPORT_UNICODE

/*****************************************************************************/
// Function Definitions
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
int HTML_InitParse (
		HTMLParseContext* ctx
	)
{
	XML_InitParse(&ctx->xmlContext);

	/* over-ride some default settings so we will handle real-world situations
	   gracefully */
	ctx->xmlContext.allowAnyNameStartChar = 1;
	ctx->xmlContext.allowCloseTagAttribLists = 1;
	ctx->xmlContext.allowMissingTagGreaterThan = 1;
	ctx->xmlContext.caseSensitive = 0;
	ctx->xmlContext.ignoreBadAttribListChars = 1;
	ctx->xmlContext.ignoreMalformedAttribs = 1;

	ctx->inScript = 0;
	ctx->inStyle = 0;

	return (0);
}

/*---------------------------------------------------------------------------*/
void HTML_DestroyParse (
		HTMLParseContext* ctx
	)
{
}

/*---------------------------------------------------------------------------*/
HTML_INT32 HTML_ParseUnit (
		HTMLParseContext* ctx,
		HTMLTagParse* parse,
		const HTML_CHAR* buffer,
		HTML_INT32 bufferLen
	)
{
	static HTML_CHAR _scriptTagName[] = {'s','c','r','i','p','t',0};
	static HTML_CHAR _styleTagName[] = {'s','t','y','l','e',0};
	HTML_INT32 charsParsed;

	if (ctx->inScript)
	{
		charsParsed = XML_ParseTagCDATA (
				&ctx->xmlContext,
				&parse->xmlParse,
				_scriptTagName,
				buffer,
				bufferLen,
				0 /* case sensitive */,
				1 /* parse comments */
			);
	}
	else if (ctx->inStyle)
	{
		charsParsed = XML_ParseTagCDATA (
				&ctx->xmlContext,
				&parse->xmlParse,
				_styleTagName,
				buffer,
				bufferLen,
				0 /* case sensitive */,
				0 /* parse comments */
			);
	}
	else
	{
		charsParsed = XML_ParseUnit (
				&ctx->xmlContext,
				&parse->xmlParse,
				buffer,
				bufferLen
			);
	}

	if (parse->xmlParse.type == XML_UNIT_TAG)
	{
		parse->htmlTag = HTML_UNKNOWN_TAG;

		if (parse->xmlParse.parse.tag.type != XML_TAG_MALFORMED)
		{
			parse->htmlTag = HTML_ParseTagType (
					parse->xmlParse.parse.tag.name.str,
					parse->xmlParse.parse.tag.name.len
				);

			switch (parse->htmlTag)
			{
				case HTML_TAG_SCRIPT:
					switch (parse->xmlParse.parse.tag.type)
					{
						case XML_TAG_START:
							if (!ctx->inScript)
							{
								ctx->inScript = 1;
							}
							break;

						case XML_TAG_END:
							if (ctx->inScript)
							{
								ctx->inScript = 0;
							}
							break;
						default:
							break;
					}
					break;

				case HTML_TAG_STYLE:
					switch (parse->xmlParse.parse.tag.type)
					{
						case XML_TAG_START:
							if (!ctx->inStyle)
							{
								ctx->inStyle = 1;
							}
							break;

						case XML_TAG_END:
							if (ctx->inStyle)
							{
								ctx->inStyle = 0;
							}
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}
	}

	return (charsParsed);
}

/*---------------------------------------------------------------------------*/
HTMLAttribValueParse* HTML_GetTagAttribute (
		HTMLTagParse* parse,
		HTMLAttributeType attrib
	)
{
	if (attrib >= 0 && attrib < HTML_NUM_ATTRIBUTES)
	{
		return (XML_GetTagAttributeASCII (
				&parse->xmlParse.parse.tag,
				rHTMLAttributeNames[attrib],
				0 /* case sensitive */
			));
	}

	return (0);
}

/*---------------------------------------------------------------------------*/
HTMLAttributeType HTML_ParseAttributeType (HTML_CHAR* str, long len)
{
	return ((HTMLAttributeType) _HTML_BSearchStringCaseInsensitive (
			str,
			rHTMLAttributeNames,
			len,
			HTML_NUM_ATTRIBUTES
		));
}

/*---------------------------------------------------------------------------*/
const char* HTML_AlignTypeToString (
		HTMLAlignType align
	)
{
	if (align < 0 || align >= HTML_NUM_ATTRIBUTES)
	{
		align = HTML_ALIGN_ABSBOTTOM;
	}

	return (rHTMLAlignTypeNames[align]);
}

/*---------------------------------------------------------------------------*/
const char* HTML_TagTypeToString (
		HTMLTagType tagType
	)
{
	if (tagType < 0 || tagType >= HTML_NUM_TAGS)
	{
		return (0);
	}

	return (rHTMLTagNames[tagType]);
}

/*---------------------------------------------------------------------------*/
const char* HTML_AttributeTypeToString (
		HTMLAttributeType attrib
	)
{
	if (attrib < 0 || attrib >= HTML_NUM_ATTRIBUTES)
	{
		return (0);
	}

	return (rHTMLAttributeNames[attrib]);
}

/*---------------------------------------------------------------------------*/
HTMLTagType HTML_ParseTagType (HTML_CHAR* str, long len)
{
	return ((HTMLTagType) _HTML_BSearchStringCaseInsensitive (
			str,
			rHTMLTagNames,
			len,
			HTML_NUM_TAGS
		));
}

/*---------------------------------------------------------------------------*/
HTMLInputType HTML_ParseInputType (HTML_CHAR* str, long len)
{
	return ((HTMLInputType) _HTML_BSearchStringCaseInsensitive (
			str,
			rHTMLInputTypeNames,
			len,
			HTML_NUM_INPUT_TYPES
		));
}

/*---------------------------------------------------------------------------*/
const char* HTML_InputTypeToString (
		HTMLInputType eType
	)
{
	if (eType < 0 || eType >= HTML_NUM_INPUT_TYPES)
	{
		return (0);
	}

	return (rHTMLInputTypeNames[eType]);
}

/*---------------------------------------------------------------------------*/
HTMLAlignType HTML_ParseAlignType (
		HTML_CHAR* str,
		long len
	)
{
	return ((HTMLAlignType) _HTML_BSearchStringCaseInsensitive (
			str,
			rHTMLAlignTypeNames,
			len,
			HTML_NUM_ALIGN_TYPES
		));
}

/*---------------------------------------------------------------------------*/
HTMLVAlignType HTML_ParseVAlignType (
		HTML_CHAR* str,
		long len
	)
{
	return ((HTMLVAlignType) _HTML_BSearchStringCaseInsensitive (
			str,
			rHTMLVAlignTypeNames,
			len,
			HTML_NUM_VALIGN_TYPES
		));
}

/*---------------------------------------------------------------------------*/
HTMLShapeType HTML_ParseShapeType (
		HTML_CHAR* str,
		long len
	)
{
	return ((HTMLShapeType) _HTML_BSearchStringCaseInsensitive (
			str,
			rHTMLShapeTypeNames,
			len,
			HTML_NUM_SHAPE_TYPES
		));
}

/*---------------------------------------------------------------------------*/
const char* HTML_ShapeTypeToString (
		HTMLShapeType shape
	)
{
	if (shape < 0 || shape >= HTML_NUM_SHAPE_TYPES)
	{
		return (0);
	}

	return (rHTMLShapeTypeNames[shape]);
}

/*---------------------------------------------------------------------------*/
HTMLTableFrameType HTML_ParseTableFrameType (
		HTML_CHAR* str,
		long len
	)
{
	return (/*(HTMLShapeType)*/ (HTMLTableFrameType) _HTML_BSearchStringCaseInsensitive (
			str,
			rHTMLTableFrameTypeNames,
			len,
			HTML_NUM_TABLEFRAME_TYPES
		));
}

/*---------------------------------------------------------------------------*/
HTML_INT32 HTML_ParseLength (
		HTML_CHAR* str,
		HTML_INT32 strLen,
		HTMLLength* length
	)
{
	long processed = 0;
	long count;
	long i = 1;
	HTMLLengthUnit defaultUnit = HTML_LENGTH_UNIT_PIXELS;

	if (!str)
	{
		HTML_LengthSet(*length, 0, HTML_LENGTH_UNIT_NONE);
		return 0;
	}

	if (strLen == 0)
	{
		strLen = webc_strlen(str);
	}

	count = XML_ParseSpace(str, strLen);
	processed += count;
	str += count;
	strLen -= count;

	switch (*str)
	{
		case '%':
			str++;
			defaultUnit = HTML_LENGTH_UNIT_PERCENT;
			break;

		case '*':
			str++;
			defaultUnit = HTML_LENGTH_UNIT_STAR;
			break;
	}

	if (IS_DIGIT(*str) || (*str == '-'))
	{
		count = HTML_ParseDecNumber(str, strLen, &i);
		processed += count;
		str += count;
		strLen -= count;

		count = XML_ParseSpace(str, strLen);
		processed += count;
		str += count;
		strLen -= count;
	}
	else if (*str != '*')
	{
		HTML_LengthSet(*length, 0, HTML_LENGTH_UNIT_NONE);
		return processed;
	}

	if (*str == '*')
	{
		HTML_LengthSet(*length, i, HTML_LENGTH_UNIT_STAR);
		processed++;
	}
	else if (*str == '%')
	{
		HTML_LengthSet(*length, i, HTML_LENGTH_UNIT_PERCENT);
		processed++;
	}
	else
	{
		HTML_LengthSet(*length, i, defaultUnit);
	}

	return (processed);
}

/*---------------------------------------------------------------------------*/
void HTML_PrintLength (
		HTML_CHAR* toStr,
		HTML_INT32 strLen,
		HTMLLength length
	)
{
	webc_print_dec(toStr, HTML_LengthGetValue(length), 1, strLen-1);
	switch(HTML_LengthGetUnit(length))
	{
	case HTML_LENGTH_UNIT_PERCENT:
		webc_strcat(toStr, (WEBC_CHAR*)L"%");
		break;
	case HTML_LENGTH_UNIT_STAR:
		webc_strcat(toStr, (WEBC_CHAR*)L"*");
		break;
	case HTML_LENGTH_UNIT_NONE:
	case HTML_LENGTH_UNIT_PIXELS:
	default:
		// do nothing
		break;
	}

	return;
}



//
/*---------------------------------------------------------------------------*/
HTML_INT32 HTML_ParseColor (
		HTML_CHAR* str,
		long len,
		HTMLColor* color
	)
{
HTML_INT32 parsedConsumed=0;
long charsParsed = 0;
int parsedLen;
long rgb_color;
long r,g,b,a;


	a = 0;
	if (len == 0)
	{
		len = webc_strlen(str);
	}

	if ((str[0] == '\'') || (str[0] == '\"'))
	{
		str++;
		charsParsed++;
		len--;

		if ((str[len] == '\'') || (str[len] == '\"'))
		{
			charsParsed++;
			len--;
		}
	}

	*color = HTML_RGBAToColor(0,0,0,0xff);

	if ((UPPERCASE(str[0]) == 'R') &&
	    (UPPERCASE(str[1]) == 'G') &&
	    (UPPERCASE(str[2]) == 'B'))
	{
	int  hasalpha = 0;
		if (UPPERCASE(str[3]) == 'A')
			hasalpha=1;
		str += 3+hasalpha;
		charsParsed += 3+hasalpha;

		while (IS_WHITESPACE(*str))
		{
			str++;
			charsParsed++;
		}

		if (*str != '(')
		{
			return (0);
		}

		str++;
		charsParsed++;

		while (IS_WHITESPACE(*str))
		{
			str++;
			charsParsed++;
		}

		parsedLen = HTML_ParseDecNumber(str, 3, &r);
		str += parsedLen;
		charsParsed += parsedLen;

		while (IS_WHITESPACE(*str))
		{
			str++;
			charsParsed++;
		}

		if (*str != ',')
		{
			return (0);
		}

		str++;
		charsParsed++;

		while (IS_WHITESPACE(*str))
		{
			str++;
			charsParsed++;
		}

		parsedLen = HTML_ParseDecNumber(str, 3, &g);
		str += parsedLen;
		charsParsed += parsedLen;

		while (IS_WHITESPACE(*str))
		{
			str++;
			charsParsed++;
		}

		if (*str != ',')
		{
			return (0);
		}

		str++;
		charsParsed++;

		while (IS_WHITESPACE(*str))
		{
			str++;
			charsParsed++;
		}

		parsedLen = HTML_ParseDecNumber(str, 3, &b);
		str += parsedLen;
		charsParsed += parsedLen;

		while (IS_WHITESPACE(*str))
		{
			str++;
			charsParsed++;
		}

		if (hasalpha)
		{
			if (*str != ',')
				return (0);
			str++;
			charsParsed++;
			while (IS_WHITESPACE(*str))
			{
				str++;
				charsParsed++;
			}
			parsedLen =  HTML_AlphaFraction(str, 5, &a);
			str += parsedLen;
			charsParsed += parsedLen;
			while (IS_WHITESPACE(*str))
			{
				str++;
				charsParsed++;
			}
		}
		if (*str != ')')
		{
			return (0);
		}

		str++;
		charsParsed++;
	}
	else
	{
		if (str[0] != '#')
		{
			int index;

			// is this a valid HTML color string?
			index = _HTML_BSearchStringCaseInsensitive (
					str,
					rHTMLColorNames,
					len,
					HTML_NUM_COLORS
				);

			if (index >= 0)
			{
				charsParsed += len;
				*color = rHTMLColorValues[index];
				return (charsParsed);
			}
		}
		else
		{
			str++;
			charsParsed++;
		}

// <EDIT 20080127 return number of character consumed to. This is because of
// Floating point 0xFF.1 hex passed by Jscript
		parsedLen = HTML_ParseHexNumber(str, 6, &rgb_color, &parsedConsumed);
// This disables		parsedConsumed = parsedLen;
		if (parsedLen > 0)
		{
			/* so we won't parse CSS lengths as colors (e.g. "100%") */
			switch (str[parsedConsumed])
			{
				case '%':
				case 'p':
				case 'm':
				case 'i':
				case 'x':
					return 0;
			}
		}

		switch (parsedLen)
		{
			case 3:
 				// Three digit colors have each digit doubled to form a 6 digit string
 				r = (rgb_color & 0xf00) >> 8;
				r |= (r << 4);

				g = (rgb_color & 0x0f0) >> 4;
				g |= (g << 4);

				b = (rgb_color & 0x00f);
				b |= (b << 4);
				break;

			case 6:
				r = (rgb_color & 0xff0000) >> 16;
				g = (rgb_color & 0x00ff00) >> 8;
				b = (rgb_color & 0x0000ff);
				break;

			default:
				return (0);
		}
// <EDIT Changed
//		charsParsed += parsedLen;
		charsParsed += parsedConsumed;
	}

	*color = HTML_RGBAToColor(r,g,b,a);

	return (charsParsed);
}

/*---------------------------------------------------------------------------*/
HTML_INT32 HTML_PrintColor (
		HTML_CHAR* toStr,
		HTMLColor color
	)
{
	WEBC_CHAR tempStr[16];
	webc_print_hex(tempStr, (long)color,6,15,0);
	webc_c_strcpy(toStr, "#");
	webc_strcat(toStr, tempStr);
	return (0);
}

/*---------------------------------------------------------------------------*/
HTML_INT32 HTML_CountLengthList (
		HTML_CHAR* str
	)
{
	long count = 1;

	while (*str)
	{
		if (*str == ',')
		{
			count++;
		}
		str++;
	}

	return (count);
}

/*---------------------------------------------------------------------------*/
HTML_INT32 HTML_ParseLengthList (
		HTML_CHAR* str,
		HTMLLength* lengthArray,
		HTML_INT32 arraySize
	)
{
	HTML_INT32 index = 0;
	HTML_INT32 valueLen;
	HTML_INT32 strLen = webc_strlen(str);

	while (index < arraySize)
	{
		for (valueLen = 0; str[valueLen] && str[valueLen] != ',' && valueLen < strLen; valueLen++) {;}

		if (valueLen > 0)
		{
			HTML_ParseLength(str, valueLen, &lengthArray[index]);
			index++;
			str += valueLen;
			strLen -= valueLen;
		}

		if (strLen > 0 && *str == ',')
		{
			str++;
			strLen--;
		}
		else
		{
			break;
		}
	}

	return (index);
}
/*---------------------------------------------------------------------------*/
HTML_CHAR* HTML_PrintLengthList (
		HTMLLength* pLengthArray,
		HTML_INT32 lArraySize
	)
{
	long lStringLength = (13 * lArraySize) + 1; // worst case signed 32 bit integer plus unit designator
	long i = 0;

	HTML_CHAR *retVal = (HTML_CHAR*) WEBC_MALLOC(sizeof(HTML_CHAR) * lStringLength);

	// run through the array and append the coordinate values
	if(retVal && pLengthArray)
	{
		retVal[0] = 0;
		for(i = 0; i < lArraySize; i++)
		{
			WEBC_CHAR strNum[14];
			HTML_PrintLength(strNum, 14, pLengthArray[i]);
			webc_strcat(retVal, strNum);
			if(i+1 < lArraySize)
				webc_strcat(retVal, (WEBC_CHAR *) L",");
		}
	}

	return retVal;
}

/*---------------------------------------------------------------------------*/
long HTML_UnescapeString (
		const HTML_CHAR * srcStr,
		HTML_CHAR * dstStr
	)
{
	HTML_CHAR escch;
	HTML_INT32 spos, upos, len;

	for (spos=0, upos=0; srcStr[spos];)
	{
		if (srcStr[spos] == '&')
		{
			spos++;

			if (srcStr[spos])
			{
				len = _HTML_GetCharEntity(&srcStr[spos], &escch);
			}
			else
			{
				len = 0;
			}

			if (len > 0)
			{
				spos += len;
			}
			else
			{
				escch = '&';
			}
		}
		else
		{
			escch = srcStr[spos++];
		}

	  #if (WEBC_SUPPORT_UNICODE && WEBC_CONVERT_FW_ASCII_TO_ASCII)
		if (escch > 0xff00 && escch < 0xff5f)
		{
			escch -= (0xff01 - '!');
		}
	  #endif

		dstStr[upos++] = escch;
	}

	dstStr[upos] = '\0';

	return (upos);
}

/*---------------------------------------------------------------------------*/
long _HTML_GetCharEntity (
		const HTML_CHAR* srcStr,
		HTML_CHAR* dstChar
	)
{
	HTML_INT32 parsedConsumed=0;
	long len = 0;

	if (*srcStr == '#')
	{
		long i;

		len++;

		if ((srcStr[1] == 'x') || (srcStr[1] == 'X'))
		{
			len++;

			// read a hex character code
			len += HTML_ParseHexNumber (srcStr + 2, 6, &i, &parsedConsumed);
		}
		else
		{
			// read a decimal character code
			len += HTML_ParseDecNumber (srcStr + 1, 8, &i);
		}

	  #if (WEBC_SUPPORT_UNICODE)
		*dstChar = (0x80 <= i && i <= 0x9f)? rHTMLExtendedCharEntityToUC[i-0x80] : (HTML_CHAR) i;
	  #else
		*dstChar = (i <= 0xff)? (HTML_CHAR) i : ' ';
	  #endif
	}
	else
	{
		long charIndex = -1;

		for (len = 0; srcStr[len] && len <= HTML_MAX_CHAR_NAME_LEN; len++)
		{
			if (srcStr[len] == ';')
			{
				break;
			}
		}

		if (len < HTML_MIN_CHAR_NAME_LEN)
		{
			return (0);
		}

		// we only know where the entity name ends if we encounter a ';'...
		if (srcStr[len] == ';')
		{
			charIndex = _HTML_BSearchString (
					srcStr,
					rHTMLEscapeCharNames,
					len,
					HTML_NUM_ESCAPE_CHARS
				);
		}
		else
		// ...otherwise we must search all possible entity name lengths,
		//  favoring the longest ones.
		{
			long temp;

			for (; len >= HTML_MIN_CHAR_NAME_LEN; len--)
			{
				temp = _HTML_BSearchString (
						srcStr,
						rHTMLEscapeCharNames,
						len,
						HTML_NUM_ESCAPE_CHARS
					);

				if (temp >= 0)
				{
					charIndex = temp;
					break;
				}
			}
		}

		if (charIndex < 0)
		{
			return (0);
		}
		else
		{
			*dstChar = rHTMLEscapeChars[charIndex];
		}
	}

	if (srcStr[len] == ';')
	{
		len++;
	}

	return (len);
}

/*---------------------------------------------------------------------------*/
HTML_INT32 HTML_ParseHexNumber (
		const HTML_CHAR *str,
		HTML_INT32 maxDigits,
		long* outNum,
		HTML_INT32 *consumed
	)
{
	HTML_INT32 n;
	long value = 0;

    *consumed = 0;
	// read a maximum of 8 hex digits and convert to a long
	for (n=0; str[n] && (n < maxDigits); n++)
	{
		if ((str[n] >= '0') && (str[n] <= '9'))
		{
			value <<= 4;
			value |= (str[n] - '0') & 0xf;
		}
		else if ((str[n] >= 'a') && (str[n] <= 'f'))
		{
			value <<= 4;
			value |= (str[n] - 'a' + 10) & 0xf;
		}
		else if ((str[n] >= 'A') && (str[n] <= 'F'))
		{
			value <<= 4;
			value |= (str[n] - 'A' + 10) & 0xf;
		}
// << EDIT - Hack to force at least something to be returned if '.' encountered
		else if (str[n] == '.')
        {
            if (n > 2)
                break;
        }
		else
		{
			break;
		}
	}

	*outNum = value;

	*consumed = n;
	return (n);
}
#if (0)
HTML_INT32 HTML_HexNumberCountWS (const HTML_CHAR *str)
{
    if (str[0] != '.')  /* Not JS floating point */
        return(0);
    int offset_to_seperator = 0;
    while (str[offset_to_seperator])  /* Not JS floating point */
    {
        if (str[offset_to_seperator] == '.')  /* Not JS floating point */
            break;
    }
    return(offset_to_seperator);
}
//
// #11.cbdef22.22abc33.abcd == #112233
// #4422abc33.abcd == #442233
//

fn(HTML_CHAR* str)
{
HTML_CHAR* str;
int v = 0;
int len = 0;
int inmantissa = 0;

    if str[0] == '.')
        inmantissa = 1;
    whlie str[len]
    {
        if (!inmantissa)
        {
            v =  = v[len]|v[1];
    if (v[2] == '.')
        inmantissa = 1;
    }
    else
    {

    }
#endif

/*---------------------------------------------------------------------------*/
HTML_INT32 HTML_AlphaFraction (
		const HTML_CHAR *str,
		HTML_INT32 maxDigits,
		long* outNum
	)
{
	int frac = 0;
	HTML_INT32 n = 0;
	long value = 0;
	long scalefactor=10;
	if (str[n] == '1') 
	{
		*outNum = 255;
		return n+1;
	}
	if (str[n] == '0') 
		n += 1;
	if (str[n] == '.')
	{
		frac = 1;
		n++;
		while (str[n] == '0') 
		{
			scalefactor *= 10;
			n += 1;
		}
	}
	if ((str[n] >= '0') && (str[n] <= '9'))
		n += HTML_ParseDecNumber (str+n, maxDigits, &value);
	if (frac)
	{ // Awful patch if they put in .600 or .60 instead of .6
		if (value > 100) scalefactor*=100;
		else if (value > 10) scalefactor*=10;
		*outNum = (value * 255)/scalefactor;
	}
	else
		*outNum = value;
	return n;
}

/*---------------------------------------------------------------------------*/
HTML_INT32 HTML_ParseDecNumber (
		const HTML_CHAR *str,
		HTML_INT32 maxDigits,
		long* outNum
	)
{
	unsigned negative = 0;
	HTML_INT32 n = 0;
	long value = 0;

	if (str[n] == '-')
	{
		negative = 1;
		n++;
	}

	// read a maximum of 13 decimal digits and convert to a long
	for (; str[n] && (n < maxDigits); n++)
	{
		if ((str[n] >= '0') && (str[n] <= '9'))
		{
  			value = (value << 1) + (value << 3) + (str[n] - '0');
		}
		else
		{
			break;
		}
	}

	*outNum = negative? -value : value;

	return (n);
}

/*---------------------------------------------------------------------------*/
long _HTML_BSearchString (
		const HTML_CHAR* forStr,
		const char** inStrList,
		long forStrLen,
		long arraySize
	)
{
	long order;
	long low = 0;
	long high = arraySize-1;
	long middle;

	if (forStrLen == 0)
	{
		forStrLen = webc_strlen(forStr);
	}

	while (low <= high)
	{
		middle = (low + high) >> 1;
		order = webc_c_strncmp(forStr, inStrList[middle], forStrLen);

		if (order == 0)
		{
			if (inStrList[middle][forStrLen] == 0)
			{
				return (middle);
			}
			order = -1;
		}

		if (order < 0)
		{
			high = middle - 1;
		}
		else
		{
			low = middle + 1;
		}
	}

	return (-1);
}

/*---------------------------------------------------------------------------*/
long _HTML_BSearchStringCaseInsensitive (
		const HTML_CHAR* forStr,
		const char** inStrList,
		long forStrLen,
		long arraySize
	)
{
	long order;
	long low = 0;
	long high = arraySize-1;
	long middle;

	if (forStrLen == 0)
	{
		forStrLen = webc_strlen(forStr);
	}

	while (low <= high)
	{
		middle = (low + high) >> 1;
		order = webc_c_strnicmp(forStr, inStrList[middle], forStrLen);

		if (order == 0)
		{
			if (inStrList[middle][forStrLen] == 0)
			{
				return (middle);
			}
			order = -1;
		}

		if (order < 0)
		{
			high = middle - 1;
		}
		else
		{
			low = middle + 1;
		}
	}

	return (-1);
}

/*---------------------------------------------------------------------------*/
void HTML_InitTagParse (
		HTMLTagParse* parse,
		HTMLTagType tagType
	)
{
	tc_memset(parse, 0, sizeof(HTMLTagParse));
	parse->htmlTag = tagType;
}

/*---------------------------------------------------------------------------*/
HTML_INT32 HTML_LengthToPixels (
		HTMLLength length,
		HTML_INT32 i100Percent
	)
{
	switch (HTML_LengthGetUnit(length))
	{
		case HTML_LENGTH_UNIT_PIXELS:
			return (HTML_LengthGetValue(length));

		case HTML_LENGTH_UNIT_PERCENT:
			if (i100Percent < 0)
			{
				break;
			}
			return ((HTML_INT32)(((long)HTML_LengthGetValue(length) * (long)i100Percent) / 100));

		default:
			break;
	}
	return (-1);
}
