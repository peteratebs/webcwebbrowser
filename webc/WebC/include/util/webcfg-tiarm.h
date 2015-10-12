
//
// WEBCFG.H - Configuration defines for web client
//
// EBS - WebC
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __WEBCFG_H__
#define __WEBCFG_H__

// define this to build the web/G viewer
//#define WEBG_VIEWER 1

// define this to build the small version of the web/G viewer
#define SMALL_VIEWER 1

#define WEBC_VERSION                        2.6  	// do not change
#define WEBC_VERSION_STR                    "2.6.0" 	// do not change
#define WEBC_OPERATING_SYSTEM               "Windows XP"
//#define WEBC_USER_AGENT_ID                  "EBS WebC MicroBrowser"
#define WEBC_USER_AGENT_ID                  "Mozilla/4.0 (compatible; EBSWebC 2.6; Windows NT 5.1)"
//#define WEBC_USER_AGENT_ID                  "Mozilla/4.0 (compatible; MSIE 5.0; Windows NT 5.1)"

/*****************************************************************************/
// Major Feature Set (WEBC_SUPPORT_xxx)
/*****************************************************************************/
#define WEBC_IMAGE_DRAW_EXTENSIONS 1
/****************************************************************************/

/* Set to 1 to build for a reduced screen device */
/* Default to smaller fonts. 					 */
/* Use PDA chromebrowser by default 			 */
/* Clip certian display elements instead of      */
/* Displaying horizontal scrollbar 				 */
#define WEBC_SUPPORT_SMALL_SCREEN 0
#define WEBC_SCREEN_WIDTH		  320

/* Set WEBC_CFG_STR_FOCUS_SELECT to
.. 1 to select all when a text edit gets the focus and to clear select when it loses focus.
.. 0 Change of focus does not affect the selected region
.. If set to 1 this is behavior is traditional
.. If set to 0 focus can change without losing selection.. (useful for soft keypads
*/

#define WEBC_CFG_STR_FOCUS_SELECT           1 /* Set to 1 to select all when a text edit gets the */
#define WEBC_CFG_ENABLE_CHROME              0 /* Set to 1 to enable HTML-based browser interface */


#define WEBC_SUPPORT_HTTP                   0  /* Set to 1 to enable support for HTTP: protocol */
#define WEBC_SUPPORT_FILE                   0 /* Set to 1 to enable support for FILE:// protocol */
#define WEBC_SUPPORT_INTERNAL               1 /* Set to 1 to enable the WEBC:// internal precompiled file lookup */


#if (WEBC_CFG_ENABLE_CHROME && !WEBC_SUPPORT_INTERNAL)
#error WEBC_SUPPORT_INTERNAL must be enabled to support HTML chrome
#endif



#if (WEBC_SUPPORT_UNICODE)
#define WEBC_SUPPORT_BIG5                   0  // Chinese Traditional encoding
#define WEBC_SUPPORT_SHIFTJIS               0  // Japanese encoding
#define WEBC_SUPPORT_UTF8                   1  // Unicode 8-bit transitional format
#define WEBC_SUPPORT_EUC_KR                 0  // Korean encoding
#define WEBC_SUPPORT_EUC_JP                 0  // Japanese encoding
#define WEBC_SUPPORT_GB2312                 0  // Simplified Chinese (GB) encoding
#endif

#define WEBC_CFG_MAX_FONT_FAMILIES          7
#define WEBC_CFG_HTTP_BUFFER_SIZE           1024
#define WEBC_CFG_BITMAP_BUFFER_SIZE         1024
#define WEBC_CFG_MAX_STREAMS                4
#define WEBC_CFG_META_TABLE_SIZE            1
#define WEBC_CFG_MAX_ELEMENT_DEPTH          30 // How deep you allow HTML tags to be nested
#define WEBC_CFG_MAX_FRAME_DEPTH            10 // How deep frames are allowed to be nested

// Max msec time to recognize as a double click event
#define WEBC_DBLCLICK_TIME                  300

#define WEBC_DRAW_IMAGE_BOXES               1
#define WEBC_DRAW_ALT_TEXT                  1
#define WEBC_MINIMIZE_SCREEN_REFRESH        0
#define WEBC_USE_3D_FRAMES                  1

/* Set to 1 to enable support for animated images, 0 otherwise. */
#define WEBC_ANIMATE_IMAGES                 1

/* Set to 1 to enable custom event handlers, 0 otherwise. */
#define WEBC_SUPPORT_USER_EVENTS            1
#if (WEBC_SUPPORT_USER_EVENTS)
 // The maximum number of custom user C++ event handlers
 #define WEBC_CFG_MAX_EVENT_HANDLERS        20
#endif // WEBC_SUPPORT_USER_EVENTS

/* Set to 1 to enable CSS, 0 otherwise. */
#define WEBC_SUPPORT_STYLE_SHEETS           1

/* Set to 1 to enable support for the <TABLE> tag, 0 otherwise */
#define WEBC_SUPPORT_TABLES                 1

/* Set to 1 to enable support for HTML frames, 0 otherwise. */
#define WEBC_SUPPORT_FRAMES                 1

/* Set to 1 to enable support for inline frames (<IFRAME>), 0 otherwise. */
#define WEBC_SUPPORT_IFRAMES                1

/* Set to 1 to enable support for the <IMG> tag, 0 otherwise. */
#define WEBC_SUPPORT_IMAGES                 1

/* Set to 1 to dither incoming bitmaps
PVOPVO - Need to explain and estimate.. also if SUPPORT_DITHERING is turned off it looks
like case WEBIMAGE_ORDERED: and case WEBIMAGE_ERROR_DIFFUSION: should be disabled along with
image coverter inclusion
other
*/
#define WEBC_SUPPORT_DITHERING              1

/* Set to 1 to enable support for .GIF image conversion, 0 otherwise.
   Note: WEBC_SUPPORT_IMAGES must be set to 1 for this option to have any
   effect. */
#define WEBC_SUPPORT_GIFS                   1

/* Set to 1 to enable support for .JPG image conversion, 0 otherwise.
   Note: WEBC_SUPPORT_IMAGES must be set to 1 for this option to have any
   effect. */
#define WEBC_SUPPORT_JPGS                   1

/* Set to 1 to enable support for .PNG image conversion, 0 otherwise.
   Note: WEBC_SUPPORT_IMAGES must be set to 1 for this option to have any
   effect. */
#define WEBC_SUPPORT_PNG                    1

/* Set to 1 to enable support for .MNG image conversion, 0 otherwise.
   Note: WEBC_SUPPORT_IMAGES must be set to 1 for this option to have any
   effect. */
#define WEBC_SUPPORT_MNG                    0

/* Set to 1 to enable support for .BMP image conversion, 0 otherwise.
   Note: WEBC_SUPPORT_IMAGES must be set to 1 for this option to have any
   effect. */
#define WEBC_SUPPORT_BMP                    1

/* Set to 1 to enable support for background images, 0 otherwise.
   Note: Only has effect if WEBC_SUPPORT_IMAGES=1. */
#define WEBC_SUPPORT_BACKGROUND_IMAGES      1

/* Set to 1 to enable handling for z-positioned elements, 0 to save a little space */
#define WEBC_SUPPORT_ZINDEX                 1

/* Set to 1 to enable support for relative-positioned elements */
#define WEBC_SUPPORT_RELATIVE_POS           1

#define WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION 1

#define WEBC_SUPPORT_INNER_HTML             1

#define WEBC_SUPPORT_OUTER_HTML             1
// if outerHTML is enabled, innerHTML must be as well
#if (WEBC_SUPPORT_OUTER_HTML)
#undef WEBC_SUPPORT_INNER_HTML
#define WEBC_SUPPORT_INNER_HTML             1
#endif

/* Set to 1 to support caching, 0 otherwise. 			*/
/* If 1 then we enable a caching layer that uses a file */
/* based caching mechanism for text and images. 		*/
/* The file system calls are abstracted through calls to*/
/* An abstract portable file access API. 				*/
/* Please see: webcfile.c								*/
/* See also : WEBC_CACHE_PAGE_SOURCE and WEBC_CACHE_IMAGES */
#define WEBC_SUPPORT_CACHE                  0   /* Set to 1 to support caching, 0 otherwise. */

/* Set to 1 to enable cookies, 0 otherwise. */
#define WEBC_SUPPORT_COOKIES                0

/* Set to 1 to enable the offline content cache */
#define WEBC_SUPPORT_OFFLINE_BROWSING       0

#define WEBC_SUPPORT_URL_MACROS             1  // Set to 1 to enable URL Macros

#define WEBC_SUPPORT_STORE_BITMAP           0  // Set to 1 to enable code for store and retrieving images (required for image caching)


#ifdef WEBC_DEBUG
#define WEBC_SUPPORT_URL_LOG                0 /* Set to one to write all urls visited to a log file */
#define WEBC_DEBUG_PARSER                   1 /* Does a lot, turn off if not needed */
#define WEBC_DEBUG_PARSER_BREAK_LINES       0 /* Add line breaks to captured data */
#define WEBC_DEBUG_LOG_JSCRIPT              0 /* (very verbose) Enable and JSCRIPT sends diagnostics to jscriptlog.txt */
#define WEBC_DEBUG_JSCRIPT                  0 /* JAVASCRIPT Debug environment, incomplete */
#else
#define WEBC_SUPPORT_URL_LOG                0
#define WEBC_DEBUG_PARSER                   0
#define WEBC_DEBUG_JSCRIPT                  0
#endif

/* The log file name used when WEBC_SUPPORT_URL_LOG is enabled */
#define WEBC_CFG_URL_LOG_FILE               "urllog.txt"


/*****************************************************************************/
// Configured Values (WEBC_CFG_xxx)
/*****************************************************************************/

#define WEBC_CFG_HISTORY_SIZE               20  // Number of URLs stored in the history buffer
#define WEBC_CFG_EVENT_QUEUE_SIZE           15  // Size of HTMLEvent queue
#define WEBC_CFG_EVENT_MAX_RECURSE_DEPTH    4   // Max recursion depth for HTMLElement::Event

#if (WEBC_SUPPORT_COOKIES)
 #define WEBC_CFG_MAX_COOKIE_BYTES			(1024*32)
#endif // WEBC_SUPPORT_COOKIES


// The size of the global string table (DO NOT CHANGE)
#define WEBC_CFG_STRING_TABLE_SIZE          1973 // a prime number


/* Automatically convert Unicode fullwidth ascii characters to normal ascii
   (only relevant when WEBC_SUPPORT_UNICODE is enabled) */
#define WEBC_CONVERT_FW_ASCII_TO_ASCII      1

/*****************************************************************************/
// Optional Features (WEBC_INCLUDE_xxx)
/*****************************************************************************/

// ********************************************************************
// SUPPORT OF WEB PROTOCOLS
// ********************************************************************

#if (!WEBC_SUPPORT_HTTP)
#undef WEBC_SUPPORT_HTTPS
#define WEBC_SUPPORT_HTTPS					0
#endif

/* This define controls whether the certs received from the server are
 verified. Set to 0 to use only encryption; 1 for authentication aswell.
 Should only have the choice to set to 1 if HTTPS is enabled. */
#if (WEBC_SUPPORT_HTTPS)
#define WEBC_SUPPORT_VERIFY					0 // 1
#else
#define WEBC_SUPPORT_VERIFY					0
#endif

#if(WEBC_SUPPORT_VERIFY==1)

#if (defined(INCLUDE_ERTFS))
#define HTTPS_CERT_FILE						"a:\\client.pem"
//#define HTTPS_KEY_FILE
#else
#define HTTPS_CERT_FILE						"client.pem"
//#define HTTPS_KEY_FILE
#endif

#endif

// ********************************************************************
// SUPPORT OF VARIOUS OPTIONS
// ********************************************************************


#if (WEBC_SUPPORT_CACHE && !WEBC_SUPPORT_FILE)
#error WEBC_SUPPORT_FILE must be set to 1 if WEBC_SUPPORT_CACHE is set to 1.
#endif

#define WEBC_CACHE_MAX_PATH_LEN             128
#define WEBC_CFG_CACHE_PREFIX               "cache\\"
#define WEBC_CFG_OFFLINE_CACHE_PREFIX       "archive\\"
#define WEBC_CFG_BMP_CACHE_PREFIX           WEBC_CFG_CACHE_PREFIX "bmp\\"

#if (WEBC_SUPPORT_OFFLINE_BROWSING)
 #define WEBC_CFG_ONLINE_CACHE_SIZE         (1024L*1024L*100L)
 #define WEBC_CFG_ONLINE_CACHE_MAX_FILES    1000
 #define WEBC_CFG_OFFLINE_CACHE_SIZE        (1024L*1024L*100L)
 #define WEBC_CFG_OFFLINE_CACHE_MAX_FILES   10000
#endif

#if (WEBC_SUPPORT_CACHE && WEBC_SUPPORT_HTTP)
#define WEBC_SUPPORT_HTTP_CACHE             1
#else
#define WEBC_SUPPORT_HTTP_CACHE             0
#endif

#if (WEBC_SUPPORT_HTTP_CACHE && !WEBC_SUPPORT_CACHE)
#error WEBC_SUPPORT_CACHE must be set to 1 if WEBC_SUPPORT_HTTP_CACHE is set to 1.
#endif

/* Set to 1 to use PEG's ZIP and UNZIP functions to compress cache content, 0 to disable
   (only supported by PEG version 1.78 and later) */
#define WEBC_SUPPORT_CACHE_COMPRESSION      0
#if (WEBC_SUPPORT_CACHE_COMPRESSION)
#error Cache compression not yet supported in WebC version 2.4 and above
#endif

/* Set to 1 to cache HTML page source.  This #define is meaningful only when
  WEBC_SUPPORT_CACHE is set to 1. */
#define WEBC_CACHE_PAGE_SOURCE              0	// Not currently supported

/* Set to 1 to cache binary image data for images, 0 otherwise.  The caching
  is done after the image has been converted, for best performance.  This
  #define is meaningful only when WEBC_SUPPORT_CACHE=1. */
#define WEBC_CACHE_IMAGES                   (WEBC_SUPPORT_CACHE && 0)

/* where to store cached files */
#define WEBC_CFG_HTTP_CACHE_LOCATION        "cache\\"

/* The maximum bytes to use for caching HTTP data */
#define WEBC_CFG_HTTP_CACHE_SIZE            (1024L*1024L*2L)

/* The maximum number of files in the HTTP cache */
#define WEBC_CFG_HTTP_CACHE_MAX_FILES       500

/* define this symbol to use a RAM-based cache for HTTP */
//#define WEBC_HTTP_USE_RAM_CACHE

/* The default image expire time = 3 min */
#define WEBC_CFG_CACHE_IMG_EXPIRE_TIME      180


/* Set to 1 to be compatible with Mozilla & IE; set to 0 for better formatting
   on narrow displays */
#define WEBC_NO_TABLE_AUTO_CLEAR            1


// ********************************************************************
// JAVASCRIPT OPTIONS/CONFIGURATION
// ********************************************************************

#if (WEBC_SUPPORT_JSCRIPT)

//Turn this on if you want to body to receive onClick, onMouseover etc... events
//
//NOTE:  This only works if JScript is enabled and it will hurt performance.
#define WEBC_SUPPORT_BODY_EVENTS            1
#define WEBC_CFG_JS_CX_POOL_SIZE            50  // lets be generous

#define WEBC_SUPPORT_JS_ERROR_REPORT        0   // adds a function to pop up a dialog box when
                                                //  there is a JavaScript error

//use these defines to be recognized as MSIE 5.0
#define WEBC_APP_CODE_NAME                  "Mozilla"
#define WEBC_APP_NAME                       "Microsoft Internet Explorer"
#define WEBC_APP_MINOR_VERSION              "0"
#define WEBC_APP_VERSION                    "4.0 (compatible; MSIE 5.0; Windows 98; COM+ 1.0.2204)"

// Use these defines to be recognized as WebC
//#define WEBC_APP_CODE_NAME                  "WebC"
//#define WEBC_APP_NAME                       "EBS WebC"
//#define WEBC_APP_MINOR_VERSION              "0"
//#define WEBC_APP_VERSION                    WEBC_VERSION_STR " (compatible; WebC " WEBC_VERSION_STR "; Windows 98;)"

//these defines should be set to reflect the platform and environment that WebC is running on
#define WEBC_CPU_CLASS                      "x86"
#define WEBC_PLATFORM                       "Win32"
#define WEBC_SYSTEM_LANGUAGE                "en-us"
#define WEBC_USER_LANGUAGE                  "en-us"

#endif


// ********************************************************************
// DEBUG/TESTING AIDS
// ********************************************************************

/* Set to 1 to enable support for performance timing */
// temporarily moved to dbgtimer.h to speed things up a bit
//#define WEBC_SUPPORT_DEBUG_TIME             0

/* Set to 1 to enable support for viewing internal message flow
   If enabled htmldiag.cpp contains code to print the message
   flow to HTML elements and underlying Html Display Elements
   Tables contained in the file allow you to select which
   messages to which elements should be displayed.
*/
#define WEBC_MESSAGEFLOW_DEBUG              0

/* If set to 1, internal counters will be maintained for each call to malloc
  and free, and a "Memory" button will be added to the toolbar, allowing you
  to monitor memory usage at run-time. */
#ifdef WEBC_DEBUG
#define WEBC_MEMORY_DEBUG					0
#else
#define WEBC_MEMORY_DEBUG                   0
#undef WEBC_MESSAGEFLOW_DEBUG
#define WEBC_MESSAGEFLOW_DEBUG              0
#endif



/* Set to the URL of the random link generator for the regression test to
  use. */
#define WEBC_RANDOM_LINK_GENERATOR          "http://rd.yahoo.com/more/?http://random.yahoo.com/bin/ryl"

#define WEBC_LOAD_BUFFER_SIZE               8192

// ********************************************************************
// ********************************************************************

/* Number of seconds to wait for response from server either when
   connecting or downloading data */
#define WEBC_TIMEOUT_SEC                    60

/****************************************************************************
 The following configuration defines correspond to array sizes.  Almost all
 of them will soon be made obsolete by the use of dynamic memory allocation.
****************************************************************************/

/* The default <SIZE> value for <INPUT> tags, i.e. the maximum number of
 visible characters */
#define WEBC_DEFAULT_INPUT_SIZE             12

/* The default <MaxLenght> value for <INPUT> tags, i.e. the maximum number
of characters that can be entered into the text box. */
#define WEBC_DEFAULT_INPUT_MAX              100

/* This string appears on the title bar of the browser window. */
#define WEBC_TITLE                          ""

/* This determines the tab size, in spaces. */
#define WEBC_TAB_SIZE                       5

/* The height, in pixels, of a horizontal rule if not specified
   as a parameter in the HTML page(<HR> tag). */
#define WEBC_HR_HEIGHT                      3

/* The space, in pixels, to put above and below a horizontal rule */
#define WEBC_HR_SPACING                     8

/* This controls the number of blank pixels in between lines. */
#define WEBC_LINE_SPACING                   0

/* The next two #defines control the width and height, respectively, for
   <IMG> tags with no WIDTH or HEIGHT attributes whose SRC files cannot be
   opened, i.e. "broken links". */
#define WEBC_BROKEN_IMG_LINK_WIDTH          1
#define WEBC_BROKEN_IMG_LINK_HEIGHT         1

/* The following two #defines determine the default dimensions of a
  <TEXTAREA> element. */
#define WEBC_DEFAULT_TEXTAREA_COLS          20
#define WEBC_DEFAULT_TEXTAREA_ROWS          2

/* Default values for input button style */

#define WEBC_CFG_DEFAULT_TEXT_HEIGHT        19
#define WEBC_CFG_DEFAULT_VSCROLL_STEP       32
#define WEBC_CFG_DEFAULT_HSCROLL_STEP       32

#define WEBC_CFG_BULLET_WIDTH               12
#define WEBC_CFG_BULLET_HEIGHT              16

// ********************************************************************
// ********** Check for invalid configurations **********
// ********************************************************************

/********** Make sure configuration is consistent **********/

#if(!WEBC_SUPPORT_IMAGES)

#undef  WEBC_SUPPORT_GIFS
#define WEBC_SUPPORT_GIFS                   0

#undef  WEBC_SUPPORT_JPGS
#define WEBC_SUPPORT_JPGS                   0

#undef  WEBC_SUPPORT_PNG
#define WEBC_SUPPORT_PNG                    0

#undef  WEBC_SUPPORT_MNG
#define WEBC_SUPPORT_MNG                    0

#undef  WEBC_SUPPORT_BMP
#define WEBC_SUPPORT_BMP                    0

#undef  WEBC_SUPPORT_BACKGROUND_IMAGES
#define WEBC_SUPPORT_BACKGROUND_IMAGES      0

#endif

#if (!WEBC_SUPPORT_FRAMES && WEBC_SUPPORT_IFRAMES)
#undef WEBC_SUPPORT_IFRAMES
#define WEBC_SUPPORT_IFRAMES 0
#endif

#if (!WEBC_SUPPORT_CACHE && WEBC_SUPPORT_OFFLINE_BROWSING)
#undef WEBC_SUPPORT_OFFLINE_BROWSING
#define WEBC_SUPPORT_OFFLINE_BROWSING 0
#endif

/* The preferred language for HTML content */
#define WEBC_CFG_DEFAULT_LANGUAGE           "en-us,en;q=0.5"

// WEBC_BUFFER_SCROLLING: experimental option (define symbol to enable)
// #define WEBC_BUFFER_SCROLLING
/* These constants control the HTTP proxy features of webc */
#define WEBC_SUPPORT_HTTP_PROXY             0
/* PVOPVO - Experimental */
#if (WEBC_SUPPORT_HTTP_PROXY)
#error HTTP proxy not supported by EBS! (use at your own risk)
#define WEBC_HTTP_PROXY_ENABLE              0
#define WEBC_HTTP_PROXY_IP                  {172,20,4,9}
#define WEBC_HTTP_PROXY_PORT                6588
#endif // WEBC_SUPPORT_HTTP_PROXY
/* Controls fast scrolling algorithm that uses pixel blitting to move regions
   that are already drawn in the event of a scroll (set to 1 to enable) */
/* PVOPVO - Not used */
#define WEBC_FAST_SCROLL                    0   // DO NOT CHANGE


/* Set to 1 to enable a fast look-up table for RGB color matches (this is
   just a waste of space if you are using a true color - 15, 16 or 24 bit
   color screen device)
   PVOPVO PEG only, not used
*/
#define WEBC_SUPPORT_RGB_TABLE              1
#if (WEBC_SUPPORT_RGB_TABLE)
 #define WEBC_CFG_RGB_TABLE_SIZE            1024 // DO NOT CHANGE
#endif


/* Set to 1 to enable sockets, 0 otherwise.
   Note: REQUIRED for all network operations
   PVOPVO - Must be one, should eliminate
   */
#define WEBC_SUPPORT_SOCKETS                1

/* PVOPVO - Not used */
#define WEBC_NOT_IMPLEMENTED                0  // Not yet implemented; leave as 0
#define WEBC_SUPPORT_FTP                    WEBC_NOT_IMPLEMENTED  // Not yet implemented; leave as 0
#define WEBC_SUPPORT_MAILTO                 WEBC_NOT_IMPLEMENTED  // Not yet implemented; leave as 0
#define WEBC_SUPPORT_IMAP                   WEBC_NOT_IMPLEMENTED  // Not yet implemented; leave as 0
#define WEBC_SUPPORT_POP3                   WEBC_NOT_IMPLEMENTED  // Not yet implemented; leave as 0
#define WEBC_SUPPORT_WEBS                   WEBC_NOT_IMPLEMENTED  // Not yet implemented; leave as 0

/* DO NOT CHANGE */
/* PVOPVO - Not used */
#define WEBC_SUPPORT_THREADS                WEBC_NOT_IMPLEMENTED
/* Set to 1 to scale all graphics output for small displays, 0 otherwise. */
/* PVOPVO - Not used */
#define WEBC_SUPPORT_PIXEL_SCALING          WEBC_NOT_IMPLEMENTED
/* PVOPVO - Not used */
#if (WEBC_SUPPORT_PIXEL_SCALING)
#error Turn off WEBC_SUPPORT_PIXEL_SCALING: this feature is not supported in this release
#endif
#if (WEBC_SUPPORT_PIXEL_SCALING)
 #define WEBC_CFG_PIXEL_MUL                 2
 #define WEBC_CFG_PIXEL_DIV                 3
#endif // WEBC_SUPPORT_PIXEL_SCALING

/*  Set to 1 to turn on support for 8-bit ascii fonts, 0 for 7-bit
PVOPVO - Not used
*/
#define WEBC_SUPPORT_8_BIT_FONTS            WEBC_NOT_IMPLEMENTED
// PVOPVO - Not used
#define WEBC_CFG_MAX_LOAD_NESTING           3 // how deep HTMLBrowser loads can be nested


#endif // __WEBCFG_H
