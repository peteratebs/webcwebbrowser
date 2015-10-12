/*
|  WEBCDEFAULT.CPP -
|
|  EBS - WebC
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/** @name Run time configuration
   		@memo webcdefault.cpp contains a table of default values that determine how webC graphical elements will be displayed. The table is initialized in bulk at startup but
   		individual values may be changed later by calling an API.
*/
#define DUMMY

/** @name Changeable Parameters
   		@memo Click on this link to view display parameters that may be configured.
		@doc  webC consults the default table for formatting instructions when it needs to draw internal elements like scroll bars. It also consult the default table when drawing HTML elements and
		a needed formatting instruction can is not available from styles or html attributes. The default values in the table may be changed at compile time and at run time to change the default motif.

		<pre>
     The following default values are used by webC when it is drawing elements and the required property is not provided by a style declaration or html property setting.

     Note: Most defaults may be overridden using CSS styles or the html style attribute, but some properties can only be changed by changing the default values in the default table.

	 In particulr scrollbar display properties are always retrieved from the default and can ony be changed by changing the default values in the table at compile time or by calling webc_SetDefault at run time.

	 * = Not supported.

     WEBC_DEFAULT_ACTIVE_BACKGROUND_COLOR   WEBC_DEFAULT_SLIDER_BACKGROUND_COLOR
     WEBC_DEFAULT_ACTIVE_COLOR              WEBC_DEFAULT_SLIDER_COLOR
     WEBC_DEFAULT_ACTIVE_LINK_COLOR         WEBC_DEFAULT_SLIDER_FRAME_WIDTH
     WEBC_DEFAULT_BACKGROUND_COLOR          WEBC_DEFAULT_SLIDER_BACKGROUND_FRAME_WIDTH
     WEBC_DEFAULT_BODY_FONT_FAMILY          WEBC_DEFAULT_SLIDER_HILITE_COLOR
     WEBC_DEFAULT_BODY_FONT_SIZE            WEBC_DEFAULT_SLIDER_PADDING
     WEBC_DEFAULT_BODY_FONT_STYLE           WEBC_DEFAULT_SLIDER_SHADOW_COLOR
     WEBC_DEFAULT_BODY_FONT_WEIGHT          WEBC_DEFAULT_SLIDER_WIDTH
     WEBC_DEFAULT_BODY_PADDING              WEBC_DEFAULT_SLIDER_FRAME_SHADOW_COLOR
     WEBC_DEFAULT_BUTTON_BACKGROUND_COLOR   WEBC_DEFAULT_SLIDER_FRAME_HILITE_COLOR
     WEBC_DEFAULT_BUTTON_FONT_FAMILY        WEBC_DEFAULT_TEXT_AREA_FONT_FAMILY
     WEBC_DEFAULT_BUTTON_FONT_SIZE          WEBC_DEFAULT_TEXT_AREA_FONT_SIZE
     WEBC_DEFAULT_BUTTON_FONT_STYLE         WEBC_DEFAULT_TEXT_AREA_FONT_STYLE
     WEBC_DEFAULT_BUTTON_FONT_WEIGHT        WEBC_DEFAULT_TEXT_AREA_FONT_WEIGHT
     WEBC_DEFAULT_BUTTON_FRAME_WIDTH        WEBC_DEFAULT_TEXT_COLOR
    * WEBC_DEFAULT_BUTTON_HILITE_COLOR      WEBC_DEFAULT_TEXT_INPUT_FONT_FAMILY
    * WEBC_DEFAULT_BUTTON_SHADOW_COLOR      WEBC_DEFAULT_TEXT_INPUT_FONT_SIZE
    * WEBC_DEFAULT_BUTTON_PADDING           WEBC_DEFAULT_TEXT_INPUT_FONT_STYLE
     WEBC_DEFAULT_HILITE_COLOR              WEBC_DEFAULT_TEXT_INPUT_FONT_WEIGHT
     WEBC_DEFAULT_LINK_COLOR                WEBC_DEFAULT_MONOSPACE_FONT_FAMILY
     WEBC_DEFAULT_LIST_INDENT               WEBC_DEFAULT_MONOSPACE_FONT_SIZE
     WEBC_DEFAULT_SCROLL_BAR_BUTTON_LENGTH  WEBC_DEFAULT_MONOSPACE_FONT_STYLE
     WEBC_DEFAULT_SELECT_FONT_FAMILY        WEBC_DEFAULT_MONOSPACE_FONT_WEIGHT
     WEBC_DEFAULT_SELECT_FONT_SIZE          WEBC_DEFAULT_VISITED_LINK_COLOR
     WEBC_DEFAULT_SELECT_FONT_STYLE         WEBC_DEFAULT_SCROLL_CORNER_COLOR
     WEBC_DEFAULT_SELECT_FONT_WEIGHT        WEBC_DEFAULT_DRAW_SCROLL_CORNER
    * WEBC_DEFAULT_SHADOW_COLOR
    </pre>
*/
#define DUMMY1

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "webcdefault.h"
#include "htmldom.hpp"
/*****************************************************************************/
/* Macros
 *****************************************************************************/

#define C_WHITE      HTML_RGBAToColor(255,255,255,0)
#define LIGHTGRAY    HTML_RGBAToColor(238,238,238,0)
#define MEDLIGHTGRAY HTML_RGBAToColor(204,204,204,0)
#define MEDDARKGRAY  HTML_RGBAToColor(152,152,152,0)
#define DARKGRAY     HTML_RGBAToColor(128,128,128,0)

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/
WEBC_READONLY WEBC_CHAR gpTimesSerifName[]       = {'T','i','m','e','s',',',' ','s','e','r','i','f',0};
WEBC_READONLY WEBC_CHAR gpArialSansSerifName[]   = {'A','r','i','a','l',',',' ','s','a','n','s','-','s','e','r','i','f',0};
WEBC_READONLY WEBC_CHAR gpCourierMonospaceName[] = {'C','o','u','r','i','e','r',',',' ','m','o','n','o','s','p','a','c','e',0};

static void* gWebcDefault [WEBC_NUM_DEFAULTS];

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
/** @name webc_RestoreDefaults
   		@memo Called by webC as part of its initialization sequence. This function may be editted directly to change default settings when the application is compiled. Alternatively, at run time, the function webc_SetDefault() may be called to change individual values.
*/
void webc_RestoreDefaults (void)
{
    gWebcDefault[WEBC_DEFAULT_ACTIVE_BACKGROUND_COLOR]       = (void*) HTML_RGBAToColor(0xAA,0,0,0);
    gWebcDefault[WEBC_DEFAULT_ACTIVE_COLOR]                  = (void*) HTML_RGBAToColor(255,255,255,0);
    gWebcDefault[WEBC_DEFAULT_ACTIVE_LINK_COLOR]             = (void*) HTML_RGBAToColor(255,0,0,0);
    gWebcDefault[WEBC_DEFAULT_BACKGROUND_COLOR]              = (void*) HTML_RGBAToColor(255,255,255,0);
    gWebcDefault[WEBC_DEFAULT_BODY_FONT_FAMILY]              = (void*) gpTimesSerifName;
    gWebcDefault[WEBC_DEFAULT_BODY_FONT_SIZE]                = (void*) CSS_FONT_SIZE_MEDIUM;
    gWebcDefault[WEBC_DEFAULT_BODY_FONT_STYLE]               = (void*) CSS_FONT_STYLE_NORMAL;
    gWebcDefault[WEBC_DEFAULT_BODY_FONT_WEIGHT]              = (void*) CSS_FONT_WEIGHT_NORMAL;
    gWebcDefault[WEBC_DEFAULT_BODY_PADDING]                  = (void*) 0;
    gWebcDefault[WEBC_DEFAULT_BUTTON_BACKGROUND_COLOR]       = (void*) MEDLIGHTGRAY;
    gWebcDefault[WEBC_DEFAULT_BUTTON_FONT_FAMILY]            = (void*) gpArialSansSerifName;
    gWebcDefault[WEBC_DEFAULT_BUTTON_FONT_SIZE]              = (void*) CSS_FONT_SIZE_SMALL;
    gWebcDefault[WEBC_DEFAULT_BUTTON_FONT_STYLE]             = (void*) CSS_FONT_STYLE_NORMAL;
    gWebcDefault[WEBC_DEFAULT_BUTTON_FONT_WEIGHT]            = (void*) CSS_FONT_WEIGHT_NORMAL;
    gWebcDefault[WEBC_DEFAULT_BUTTON_FRAME_WIDTH]            = (void*) 1;
    gWebcDefault[WEBC_DEFAULT_BUTTON_HILITE_COLOR]           = (void*) LIGHTGRAY;
    gWebcDefault[WEBC_DEFAULT_BUTTON_SHADOW_COLOR]           = (void*) MEDDARKGRAY;
    gWebcDefault[WEBC_DEFAULT_BUTTON_PADDING]                = (void*) 3;
    gWebcDefault[WEBC_DEFAULT_HILITE_COLOR]                  = (void*) 0;
    gWebcDefault[WEBC_DEFAULT_LINK_COLOR]                    = (void*) HTML_RGBAToColor(0,0,128,0);
    gWebcDefault[WEBC_DEFAULT_LIST_INDENT]                   = (void*) 42;
    gWebcDefault[WEBC_DEFAULT_SCROLL_BAR_BUTTON_LENGTH]      = (void*) 15;
//    gWebcDefault[WEBC_DEFAULT_SCROLL_BAR_FRAME_WIDTH]        = (void*) 15;
    gWebcDefault[WEBC_DEFAULT_SCROLL_BAR_HILITE_COLOR]       = (void*) LIGHTGRAY;
    gWebcDefault[WEBC_DEFAULT_SCROLL_BAR_SHADOW_COLOR]       = (void*) MEDDARKGRAY;
    gWebcDefault[WEBC_DEFAULT_SELECT_FONT_FAMILY]            = (void*) gpArialSansSerifName;
    gWebcDefault[WEBC_DEFAULT_SELECT_FONT_SIZE]              = (void*) CSS_FONT_SIZE_SMALL;
    gWebcDefault[WEBC_DEFAULT_SELECT_FONT_STYLE]             = (void*) CSS_FONT_STYLE_NORMAL;
    gWebcDefault[WEBC_DEFAULT_SELECT_FONT_WEIGHT]            = (void*) CSS_FONT_WEIGHT_NORMAL;
    gWebcDefault[WEBC_DEFAULT_SHADOW_COLOR]                  = (void*) 0;
    gWebcDefault[WEBC_DEFAULT_SLIDER_BACKGROUND_COLOR]       = (void*) MEDDARKGRAY;
    gWebcDefault[WEBC_DEFAULT_SLIDER_COLOR]                  = (void*) MEDLIGHTGRAY;
    gWebcDefault[WEBC_DEFAULT_SLIDER_FRAME_WIDTH]            = (void*) 1;
    gWebcDefault[WEBC_DEFAULT_SLIDER_BACKGROUND_FRAME_WIDTH] = (void*) 1;
    gWebcDefault[WEBC_DEFAULT_SLIDER_HILITE_COLOR]           = (void*) LIGHTGRAY;
    gWebcDefault[WEBC_DEFAULT_SLIDER_PADDING]                = (void*) 0;
    gWebcDefault[WEBC_DEFAULT_SLIDER_SHADOW_COLOR]           = (void*) MEDDARKGRAY;
    gWebcDefault[WEBC_DEFAULT_SLIDER_WIDTH]                  = (void*) 15;
    gWebcDefault[WEBC_DEFAULT_SLIDER_FRAME_SHADOW_COLOR]     = (void*) MEDLIGHTGRAY;
    gWebcDefault[WEBC_DEFAULT_SLIDER_FRAME_HILITE_COLOR]     = (void*) DARKGRAY;
    gWebcDefault[WEBC_DEFAULT_TEXT_AREA_FONT_FAMILY]         = (void*) gpArialSansSerifName;
    gWebcDefault[WEBC_DEFAULT_TEXT_AREA_FONT_SIZE]           = (void*) CSS_FONT_SIZE_SMALL;
    gWebcDefault[WEBC_DEFAULT_TEXT_AREA_FONT_STYLE]          = (void*) CSS_FONT_STYLE_NORMAL;
    gWebcDefault[WEBC_DEFAULT_TEXT_AREA_FONT_WEIGHT]         = (void*) CSS_FONT_WEIGHT_NORMAL;
    gWebcDefault[WEBC_DEFAULT_TEXT_COLOR]                    = (void*) HTML_RGBAToColor(0,0,0,0);
    gWebcDefault[WEBC_DEFAULT_TEXT_INPUT_FONT_FAMILY]        = (void*) gpArialSansSerifName;
    gWebcDefault[WEBC_DEFAULT_TEXT_INPUT_FONT_SIZE]          = (void*) CSS_FONT_SIZE_SMALL;
    gWebcDefault[WEBC_DEFAULT_TEXT_INPUT_FONT_STYLE]         = (void*) CSS_FONT_STYLE_NORMAL;
    gWebcDefault[WEBC_DEFAULT_TEXT_INPUT_FONT_WEIGHT]        = (void*) CSS_FONT_WEIGHT_NORMAL;
    gWebcDefault[WEBC_DEFAULT_MONOSPACE_FONT_FAMILY]        = (void*) gpCourierMonospaceName;
    gWebcDefault[WEBC_DEFAULT_MONOSPACE_FONT_SIZE]          = (void*) CSS_FONT_SIZE_MEDIUM;
    gWebcDefault[WEBC_DEFAULT_MONOSPACE_FONT_STYLE]         = (void*) CSS_FONT_STYLE_NORMAL;
    gWebcDefault[WEBC_DEFAULT_MONOSPACE_FONT_WEIGHT]        = (void*) CSS_FONT_WEIGHT_NORMAL;
	gWebcDefault[WEBC_DEFAULT_VISITED_LINK_COLOR]            = (void*) HTML_RGBAToColor(128,0,128,0);
	gWebcDefault[WEBC_DEFAULT_SCROLL_CORNER_COLOR]           = gWebcDefault[WEBC_DEFAULT_BUTTON_BACKGROUND_COLOR];
	gWebcDefault[WEBC_DEFAULT_DRAW_SCROLL_CORNER]            = (void*) WEBC_TRUE;
}

/*---------------------------------------------------------------------------*/
void* webc_GetDefault (WebcDefaultIndex varIndex)
{
	if (varIndex >= 0 && varIndex < WEBC_NUM_DEFAULTS)
	{
		return (gWebcDefault[varIndex]);
	}

	return WEBC_NULL;
}
unsigned long  webc_GetDefaultUint (WebcDefaultIndex varIndex)
{
	return (unsigned long) webc_GetDefault(varIndex);
}
/*---------------------------------------------------------------------------*/
/** @name _webc_SetDefault
   		@memo _webc_SetDefault(INDEX, VALUE) may be called to change any of the default values at run time. You should not call _webc_SetDefault directly, instead call the macro webc_SetDefault(), which castes the second argument to void *.
		@doc Call this function to change default values. Affected elements will be drawn using these defaults if no style or other HTML attribute overrides the setting.
		<pre>
		Note: Before using webc_SetDefault() to modify a default value you should consult the source code for webc_RestoreDefaults to verify that you are passing the correct data type.

	 	Examples:
		WEBC_CHAR TahomaName[] = {'T','a','h','o','m','a',0}; // Works for unicode or non unicode strings
		  webc_SetDefault(WEBC_DEFAULT_BODY_FONT_FAMILY, TahomaName);
		  webc_SetDefault(WEBC_DEFAULT_BODY_FONT_SIZE, CSS_FONT_SIZE_LARGE);

		  webc_SetDefault(WEBC_DEFAULT_SLIDER_WIDTH, 10);
		  webc_SetDefault(WEBC_DEFAULT_SLIDER_HILITE_COLOR, HTML_RGBAToColor(255,0,0,0));
		  webc_SetDefault(WEBC_DEFAULT_SLIDER_SHADOW_COLOR, HTML_RGBAToColor(0,255,0,0));
		  webc_SetDefault(WEBC_DEFAULT_SLIDER_FRAME_SHADOW_COLOR, HTML_RGBAToColor(128,0,0,0));
		  webc_SetDefault(WEBC_DEFAULT_SLIDER_FRAME_HILITE_COLOR, HTML_RGBAToColor(0,128,0,0));

		</pre>
*/
void _webc_SetDefault (WebcDefaultIndex varIndex, void* value)
{
	if (varIndex >= 0 && varIndex < WEBC_NUM_DEFAULTS)
	{
		gWebcDefault[varIndex] = value;
	}
}
