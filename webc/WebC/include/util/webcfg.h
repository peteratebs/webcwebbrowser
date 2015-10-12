
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

#ifdef __TIARM__
#define __TSDPROJECT__	  // defined if __TIARM__
#endif

// Include file we use to override WEBC_SUPPORT_JSCRIPT command line argument. (so we are not dependant on command line)
#include "webcjhconfig.h"

// define this to build the web/G viewer
//#define WEBG_VIEWER 1


// define this to build the small version of the web/G viewer
#define SMALL_VIEWER 1

#define WEBC_VERSION                        2.6  	// do not change
#define WEBC_VERSION_STR                    "2.6.0" 	// do not change
#define WEBC_OPERATING_SYSTEM               "Windows XP"
//#define WEBC_USER_AGENT_ID                  "EBS WebC MicroBrowser"  <-- pretty well behaved
//#define WEBC_USER_AGENT_ID                  "Mozilla/4.0 (compatible; EBSWebC 2.6; Windows NT 5.1)" <-- Displays inline-block , not pretty 
// #define WEBC_USER_AGENT_ID                  "Mozilla/4.0 (compatible; MSIE 5.0; Windows NT 5.1)" 
#define WEBC_USER_AGENT_ID                  "Mozilla/4.0 (compatible; MSIE 5.0; Windows NT 5.1)  Mobile" 

/*****************************************************************************/
// Major Feature Set (WEBC_SUPPORT_xxx)
/*****************************************************************************/
/****************************************************************************/

/** @name Compile time configuration
   		@memo Modify constants in the file include/util/webcfg.h to change the feature set of webC.<br>
		The preprocessor constants listed below may be changed to chagne the feature set and behavior of webC.
*/
#define DUMMY

/****************************************************************************/



/** @name  WEBC_SUPPORT_UNICODE
        @memo Set to 1 to enable Unicode support, set to zero to disable.
   		@doc Unicode adds to the code footprint and to memory consumption but it is required if you need to display non European content. You may cut down on resources by disabling Unicode if your application requires only the ASCII character set.
<pre>
          Note: For technical reasons WEBC_SUPPORT_UNICODE is not in include/util/webcfg.h instead you must pass it on the compiler command line:
          This is typically done by passing -DWEBC_SUPPORT_UNICODE=0 or -DWEBC_SUPPORT_UNICODE=1. Most compiler will default to a value of zero if the value is not specified.
</pre>

@see WEBC_SUPPORT_BIG5 WEBC_SUPPORT_SHIFTJIS WEBC_SUPPORT_UTF8 WEBC_SUPPORT_EUC_KR WEBC_SUPPORT_EUC_JP WEBC_SUPPORT_GB2312

*/

#define DUMMY1

/** @name WEBC_SUPPORT_JSCRIPT
        @memo Set to 1 to enable Javascript support, set to zero to disable.
   		@doc Javascript adds to the code footprint, ram footprint and memory consumption and document load time. Embedded applications that use the webC API exclusively and do not
   		rely on Javascript may disable it to reduce resource consumption.
<pre>
          Note: For technical reasons WEBC_SUPPORT_JSCRIPT is not in include/util/webcfg.h instead you must configure change the setting in include/util/webcjhconfig.h
</pre>
*/
#define DUMMY2

/** @name WEBC_DEBUG
        @memo Define WEBC_DEBUG to enable extra debug code and asserts when errant behavior is suspect.
   		@doc Enables extra debug checks and asserts when errant behavior is suspected.
<pre>
          Note: For technical reasons WEBC_DEBUG is not in include/util/webcfg.h instead you must pass it on the compiler command line:
          This is typically done by passing -DWEBC_DEBUG. The test is #ifdef so -DWEBC_DEBUG=0 will still produce code with debug enabled.
</pre>
*/
#define DUMMY3


/** @name WEBC_SUPPORT_SMALL_SCREEN
   		@memo Set to 1 to for a reduced screen device.
   		@doc Set to 1 to build for a reduced screen device. Defaults to smaller fonts. Use PDA chromebrowser by default, clip certain display elements instead of displaying horizontal scrollbar.
<pre>
         Note: use caution when enabling this option, it is an old option with some useful features but it has not been tested on recent releases of webC.
</pre>
*/
#define WEBC_SUPPORT_SMALL_SCREEN 0



/** @name WEBC_SCREEN_WIDTH
   		@memo Set to your target's screen width in pixels. For example for a QVGA display the value is 320.
*/
//#define WEBC_SCREEN_WIDTH		  320
#define WEBC_SCREEN_WIDTH		  320
/** @name WEBC_SCREEN_HEIGHT
   		@memo Set to your target's screen height in pixels. For example for a QVGA display the value is 240.
*/
//#define WEBC_SCREEN_HEIGHT		  240

#define WEBC_SCREEN_HEIGHT		  240
/** @name WEBC_SUPPORT_TOUCHSCREEN
   		@memo Set to 1 to enable a softkeyboard popup when a text box or text area is touched
*/
#define WEBC_SUPPORT_TOUCHSCREEN 			1 // Includes softkeyboard

/** @name WEBC_CFG_STR_FOCUS_SELECT
        @memo Set to 1 to select all when a text edit gets focus.
	@doc
	<pre>
1 to select all when a text edit gets the focus and to clear select when it loses focus.
0 Change of focus does not affect the selected region
If set to 1 this is behavior is traditional
If set to 0 focus can change without losing selection.. (useful for soft keypads)
	</pre>
*/
#if (WEBC_SUPPORT_TOUCHSCREEN)
#define WEBC_CFG_STR_FOCUS_SELECT           0 /* Set to 1 to select all when a text edit gets the */
#else
#define WEBC_CFG_STR_FOCUS_SELECT           1 /* Set to 1 to select all when a text edit gets the */
#endif

/** @name WEBC_CFG_STR_CURSOR_WIDTH
	@memo Set the width in pixels of the textbox and editstring cursor.
*/
#define WEBC_CFG_STR_CURSOR_WIDTH           2 /* Width in pixels of the textbox and editstring cursor */

/** @name WEBC_CFG_ENABLE_CHROME
        @memo Set to 1 to include code needed to support the Chrome C and Javascript based web browser console.
*/
#ifdef __TSDPROJECT__
#define WEBC_CFG_ENABLE_CHROME              0 /* Set to 1 to enable HTML-based browser interface */
#else
#define WEBC_CFG_ENABLE_CHROME              1 /* Set to 1 to enable HTML-based browser interface */
#endif

/** @name WEBC_SUPPORT_HTTP
   		@memo Set to 1 to include support for the http: url. If enabled rtpnet.c in the rtplatform layer must be ported.
*/
#define WEBC_SUPPORT_HTTP                   1  /* Set to 1 to enable support for HTTP: protocol */

/** @name WEBC_SUPPORT_HTTPS
   		@memo Set to 1 to include support for the https: url. A working reference is available for windows.
   		@doc Set to 1 to include support for the https: url. If WEBC_SUPPORT_HTTPS is 1 you must provided SSL services to webC through the rtp_ssl_XX() interface. A reference implementation is is included for using OpenSSL on Windows.
*/
#define DUMMY4                   0  /* Set to 1 to enable support for HTTPS: protocol */

/** @name WEBC_SUPPORT_FILE
        @memo Set to 1 to enable support for FILE: protocol. If enebled you must port rtpfile.c in the rtplatform layer to your file system.
*/
#ifdef __TSDPROJECT__
#define WEBC_SUPPORT_FILE                   1  /* Set to 1 to enable support for FILE:// protocol */
#else
#define WEBC_SUPPORT_FILE                   1  /* Set to 1 to enable support for FILE:// protocol */
#endif
/** @name WEBC_SUPPORT_INTERNAL
        @memo Set to 1 to enable support for WEBC: built in "rom" file protocol.
   		@doc Set to 1 to enable the WEBC:// internal precompiled file lookup. The precompiled files must be built using the bintoc utility to create 'C' arrays from content source code
   		which must then be manually inluded in webvfile.cpp. See appendix for instructions.
*/
#define WEBC_SUPPORT_INTERNAL               1 /* Set to 1 to enable the WEBC:// internal precompiled file lookup */

/** @name WEBC_SUPPORT_LOCAL
        @memo Set to 1 to enable support for LOCAL: protocol
   		@doc Set to 1 to enable the LOCAL:// callback interface for content. If WEBC_SUPPORT_LOCAL is one when a URL starting with local: is requested webC calls user based
   		callback routines to satisfy the requests.
		<pre>
		These routines must be populated if WEBC_SUPPORT_LOCAL is enabled. (examples are provided)
		webc_localGet  - Give a the URL, prepare a named pipe of some sort based on the contents of the url string ande return a handle.
		webc_localRead - Return data from a pipe opened in webc_localGet
		localWrite     - Write data to a pipe opened in webc_localGet. Only needed if posting to local:
		webc_localClose- Close a pipe opened in webc_localGet.
		</pre>
*/
#define WEBC_SUPPORT_LOCAL                  0 /* Set to 1 to enable the local:// protocol */

/** @name WEBC_SUPPORT_ERROR_PAGE_REDIRECT
        @memo Set to 1 to enable support for redirecting the URL when a URL load error occurs.
   		@doc Set to 1 to enable loading of alternate URL when a load error occurs. Application specific content may be placed in url at the redirected page
   		to provide application specific look and feel to the error messages and to invoke applications specific error handling code.
		<pre>
		If WEBC_SUPPORT_ERROR_PAGE_REDIRECT is enabled you must call webc_setErrorRedirect() to assign redirect on error pages for
		URL_ERROR_SERVER_NOT_FOUND, URL_ERROR_PAGE_NOT_FOUND, URL_ERROR_PAYMENT_REQ and	URL_ERROR_FORBIDDEN.(examples are provided)
		</pre>
*/
#define WEBC_SUPPORT_ERROR_PAGE_REDIRECT    0 /* See webload.cpp */
#if(WEBC_SUPPORT_ERROR_PAGE_REDIRECT)
#define MAX_REDIRECT_URLS 4
#endif
#if (WEBC_CFG_ENABLE_CHROME && !WEBC_SUPPORT_INTERNAL)
#error WEBC_SUPPORT_INTERNAL must be enabled to support HTML chrome
#endif


#if (WEBC_SUPPORT_UNICODE)
#ifdef __TSDPROJECT__
#define WEBC_SUPPORT_BIG5                   0  // Chinese Traditional encoding
#define WEBC_SUPPORT_SHIFTJIS               0  // Japanese encoding
#define WEBC_SUPPORT_UTF8                   0  // Unicode 8-bit transitional format
#define WEBC_SUPPORT_EUC_KR                 0  // Korean encoding
#define WEBC_SUPPORT_EUC_JP                 0  // Japanese encoding
#define WEBC_SUPPORT_GB2312                 0  // Simplified Chinese (GB) encoding
#else
/** @name WEBC_SUPPORT_BIG5
        @memo Set to 1 to support Chinese Traditional encoding. WEBC_SUPPORT_UNICODE must be enabled, translation table adds to the rom footprint.
*/
#define WEBC_SUPPORT_BIG5                   1  // Chinese Traditional encoding

/** @name WEBC_SUPPORT_SHIFTJIS
        @memo Set to 1 to support Japanese encoding. WEBC_SUPPORT_UNICODE must be enabled, translation table adds to the rom footprint.
*/
#define WEBC_SUPPORT_SHIFTJIS               1  // Japanese encoding

/** @name WEBC_SUPPORT_UTF8
        @memo Set to 1 to support Unicode 8-bit transitional format. WEBC_SUPPORT_UNICODE must be enabled, translation table adds to the rom footprint.
*/
#define WEBC_SUPPORT_UTF8                   1  // Unicode 8-bit transitional format

/** @name WEBC_SUPPORT_EUC_KR
        @memo Set to 1 to support Korean encoding. WEBC_SUPPORT_UNICODE must be enabled, translation table adds to the rom footprint.
*/
#define WEBC_SUPPORT_EUC_KR                 1  // Korean encoding

/** @name WEBC_SUPPORT_EUC_JP
        @memo Set to 1 to support Japanese encoding. WEBC_SUPPORT_UNICODE must be enabled, translation table adds to the rom footprint.
*/
#define WEBC_SUPPORT_EUC_JP                 1  // Japanese encoding

/** @name WEBC_SUPPORT_GB2312
        @memo Set to 1 to support simplified Chinese (GB) encoding. WEBC_SUPPORT_UNICODE must be enabled, translation table adds to the rom footprint.BUGBUG
*/
#define WEBC_SUPPORT_GB2312                 1  // Simplified Chinese (GB) encoding

#endif
#endif

/** @name WEBC_CFG_MAX_FONT_FAMILIES
        @memo Set to the number of font families included in the build.
    @doc Must be set to the number of font families provided by the graphics abstraction layer. For example if the build includes Time, Courier and Arial fonts then WEBC_CFG_MAX_FONT_FAMILIES should be 3.
	Consult white papers that describe including fonts in webC.
*/
#define WEBC_CFG_MAX_FONT_FAMILIES          7
/** @name WEBC_CFG_HTTP_BUFFER_SIZE
    @memo Set the buffer size for HTTP (TCP-IP) GET data tranfers. Each open HTTP connection allocates a buffer of this size and recieves data in this size chunks and passes the data to the parser. The default value is small, 1024 bytes and if it is increased it cmay improve performance.
*/
#define WEBC_CFG_HTTP_BUFFER_SIZE           1024     // 1024

/** @name WEBC_CFG_FILE_POST_BUFFER_SIZE
    @memo Set the buffer size for data being posted from a file. File post operations allocate a buffer of this size to retrieve data from a file and post to an HTTP server. A post operation allocates a buffer of this
	size and sends data in this size chunks. The default value is small, 1024 bytes and if it is increased it may improve performance.
*/
#define WEBC_CFG_FILE_POST_BUFFER_SIZE      1024     // 1024   Used to buffer data being posted from a file.

/** @name WEBC_CFG_BITMAP_BUFFER_SIZE
    @memo Set the buffer size for image loads. Image loads allocate a buffer of this size to retrieve data from the content source, FILE: HTTP: WEC: etc. Each concurrently loading image allocates a buffer of this
	size and recieves data in this size chunks and passes it to the image decoder. The default value is small, 1024 bytes and if it is increased it may improve performance.
*/
#define WEBC_CFG_BITMAP_BUFFER_SIZE         1024     // 1024

/** @name WEBC_CFG_MAX_STREAMS
    @memo Set the maximum number of simultaneous http gets to process. webC will perform WEBC_CFG_MAX_STREAMS simultaneous get operations when retrieving content over http. When WEBC_CFG_MAX_STREAMS are currently active and a new URL is detected in the data stream
	then webC waits for another strem to complete before processing the url. The default value is small, 4, if it is increased it may improve performance for http based applications.
*/
#define WEBC_CFG_MAX_STREAMS                8

/* Meta tag callback handler constants */
/** @name WEBC_CFG_META_TABLE_SIZE
        @memo Set the maximum number user metatag callbacks. If set to zero excludes metatag user callbacks, otherwise determines the maximum number of metatag callback handlers that may be assigned.
*/
#define WEBC_CFG_META_TABLE_SIZE            20
#define WEBC_CFG_META_DIPATCH_EQUIV			1		// If true (default) custom meta tag handler dispatched by HTTP_EQUIV instead of NAME property.
/** @name WEBC_CFG_MAX_META_FUNCTION_NAMELEN
    @memo Set the maximum length of a metatag to store in the dispatch table. Defines the maximum string length that will be store for assigned metatag handlers. Small names are slightly faster and consume less memory but larger names are more
	self documenting, especially when the application provides many metatag handlers, perhaps with sensible "namespaces".
*/
#define WEBC_CFG_MAX_META_FUNCTION_NAMELEN  128		// Deault was 16

/** @name WEBC_SUPPORT_USER_EVENTS
    @memo Set to 1 to enable custom named event handlers, 0 otherwise. Named event handlers are a webC legacy HTML extension that allows assigning a named event handler to one or more elements. There are newer ways to assign 'C' event handlers to elements but
	this method has advantages of simplicty and ease of interoperability between Javascript and 'C' with minimal complexity.
*/
#define WEBC_SUPPORT_USER_EVENTS            1 /* Set to 1 to enable custom event handlers, 0 otherwise. */

#if (WEBC_SUPPORT_USER_EVENTS)
 // The maximum number of custom user C++ event handlers
/** @name WEBC_CFG_MAX_EVENT_HANDLERS
        @memo Sets the maximum number of seperate named user event handlers that may be installed.
*/
#define WEBC_CFG_MAX_EVENT_HANDLERS        20
#endif // WEBC_SUPPORT_USER_EVENTS

/** @name WEBC_SUPPORT_URL_MACROS
    @memo Set to 1 to enable support for url macros. URL macros are a webC legacy extension allowing calls to 'C' to perform macro replacement on a section of a url string.
*/
#define WEBC_SUPPORT_URL_MACROS             1  // Set to 1 to enable URL Macros

/** @name WEBC_CFG_MAX_ELEMENT_DEPTH
    @memo Sets the maximum depth that HTML tags may be nested.
*/
#define WEBC_CFG_MAX_ELEMENT_DEPTH          30 // How deep you allow HTML tags to be nested

/** @name WEBC_CFG_MAX_FRAME_DEPTH
    @memo Sets the maximum depth that frames may be nested.
*/
#define WEBC_CFG_MAX_FRAME_DEPTH            10 // How deep frames are allowed to be nested

/** @name WEBC_DBLCLICK_TIME
    @memo Sets the maximum timespan in msecs, within which, if two clicks occur in this time a double click event is generated. Double click events are avaiable for the javascript ondblclick handler and for applications that wish to implement 'C' double click handlers to process HTML_EVENT_DBLCLICK events.
*/
#define WEBC_DBLCLICK_TIME                  300

// Do not change without re-testing. Changing values from the following block of configuration values is not supported or recommended.
#define WEBC_DRAW_IMAGE_BOXES               1
#define WEBC_DRAW_ALT_TEXT                  1
#define WEBC_MINIMIZE_SCREEN_REFRESH        0
#define WEBC_USE_3D_FRAMES                  1
#define WEBC_ANIMATE_IMAGES                 1
#define WEBC_SUPPORT_RADIUS           		0 /* Enable drawing rounded corners. (Implementation is incomplete, enable to continue development) */
#define WEBC_SUPPORT_STYLE_SHEETS           1
#define WEBC_SUPPORT_TABLES                 1
#define WEBC_SUPPORT_FRAMES                 1
#define WEBC_SUPPORT_IFRAMES                1
#define WEBC_SUPPORT_AJAX                	1	// Implementation is inclomplete
#define WEBC_SUPPORT_IMAGES                 1




/* Set to 1 to dither incoming bitmaps not supported for most targets, see the peg implementation for more information. */
#define WEBC_SUPPORT_DITHERING              1

/** @name  WEBC_SUPPORT_GIFS
        @memo Set to 1 to enable support for .GIF image decode and display, 0 otherwise.*/
#define WEBC_SUPPORT_GIFS                   1

/** @name WEBC_SUPPORT_JPGS
        @memo Set to 1 to enable support for .JPG image decode and display, 0 otherwise. */
#define WEBC_SUPPORT_JPGS                   1

/** @name  WEBC_SUPPORT_PNG
        @memo Set to 1 to enable support for .PNG image decode and display, 0 otherwise. */
#define WEBC_SUPPORT_PNG                    1

/** @name  WEBC_SUPPORT_MNG
        @memo Set to 1 to enable support for .MNG image decode and display, 0 otherwise. */
#ifdef __TSDPROJECT__
#define WEBC_SUPPORT_MNG                    0
#else
#define WEBC_SUPPORT_MNG                    0
#endif
/** @name  WEBC_SUPPORT_BMP
        @memo Set to 1 to enable support for .BMP image decode and display, 0 otherwise. */
#define WEBC_SUPPORT_BMP                    1

// Do not change without re-testing. Changing values from the following block of configuration values is not supported or recommended.
#define WEBC_SUPPORT_BACKGROUND_IMAGES      1
#define WEBC_SUPPORT_ZINDEX                 1
#define WEBC_SUPPORT_RELATIVE_POS           1
#define WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION 1
#define WEBC_SUPPORT_INNER_HTML             1
#define WEBC_SUPPORT_OUTER_HTML             1
// if outerHTML is enabled, innerHTML must be as well
#if (WEBC_SUPPORT_OUTER_HTML)
#undef WEBC_SUPPORT_INNER_HTML
#define WEBC_SUPPORT_INNER_HTML             1
#endif

/** @name WEBC_SUPPORT_CACHE
        @memo Set to 1 to support caching content to the local file system, 0 otherwise.
	@doc Enable a caching layer for HTTP that uses a file based caching mechanism for text and images.
	<pre>
	Startup code must initialize caching and either WEBC_HTTP_USE_RAM_CACHE must be enabled and configurd (See source code) or the Rtplatform file system abstraction layer must be supported on your target.
	</pre>
*/
#define WEBC_SUPPORT_CACHE                  0



/* Set to 1 to enable cookies, 0 otherwise. */
/** @name WEBC_SUPPORT_COOKIES
    @memo Set to 1 to support cookies, 0 otherwise. Enable support for cookies, document.cookie under Javascript and the native webC cookie API functions with 'C'. Note that cookies are stored in memory only, there is mo mechanism currently to write them to disk.
*/
#ifdef __TSDPROJECT__
#define WEBC_SUPPORT_COOKIES                1
#else
#define WEBC_SUPPORT_COOKIES                1
#endif

#define WEBC_CFG_MAX_COOKIE_BYTES			(1024*32)


/** @name WEBC_SUPPORT_OFFLINE_BROWSING
    @memo Set to 1 to enable the offline content cache. Enable support for storing content along with the content's URL to be reviewable at a later time when the device is not connected.
*/
#ifdef __TSDPROJECT__
#define WEBC_SUPPORT_OFFLINE_BROWSING       1
#else
#define WEBC_SUPPORT_OFFLINE_BROWSING       1
#endif



// Note that some of these debugging these methods are old and not necessarilly clean and reliable. Use at your own risk.
#ifdef WEBC_DEBUG
#define WEBC_SUPPORT_URL_LOG                0 /* Set to one to write all urls visited to a log file */
/* The log file name used when WEBC_SUPPORT_URL_LOG is enabled */
#define WEBC_CFG_URL_LOG_FILE               "urllog.txt"
#define WEBC_DEBUG_PARSER                   0 /* Does a lot, turn off if not needed */
#define WEBC_DEBUG_PARSER_BREAK_LINES       0 /* Add line breaks to captured data */
#define WEBC_DEBUG_LOG_JSCRIPT              0 /* (very verbose) Enable and JSCRIPT sends diagnostics to jscriptlog.txt */
#define WEBC_DEBUG_JSCRIPT                  0 /* JAVASCRIPT Debug environment, incomplete */
#define WEBC_DEBUG_CACHING                  0 /* Log cache hits */
#else
#define WEBC_SUPPORT_URL_LOG                0
#define WEBC_DEBUG_PARSER                   0
#define WEBC_DEBUG_JSCRIPT                  0
#define WEBC_DEBUG_CACHING                  0
#endif



/*****************************************************************************/
// Configured Values (WEBC_CFG_xxx)
/*****************************************************************************/

/** @name WEBC_CFG_EVENT_QUEUE_SIZE
        @memo Configure the size of HTMLEvent queue.
	@doc This is the maximum number of Events that webC will queue. If the maximum is exceeded new events are dropped. The default value is low, raise it if you suspect
	events like mouse move or touchscreen tracking events are being dropped. In debug mode an assert is generated if the queue fills.

*/
#define WEBC_CFG_EVENT_QUEUE_SIZE           15  // Size of HTMLEvent queue


/** @name WEBC_CFG_EVENT_MAX_RECURSE_DEPTH
        @memo Max recursion depth for HTMLElement::Event. This is the maximum number of depth that the actions of an element's Event handler can recurse. Making API calls and sending messages to an Element from its own event handler
	can cause a certain amount of recursion to occur, this is a guard against excessive recursion. In debug mode an assert is generated if the limit is reached.

*/
#define WEBC_CFG_EVENT_MAX_RECURSE_DEPTH    4   // Max recursion depth for HTMLElement::Event


/** @name WEBC_CFG_HISTORY_SIZE
    @memo Number of URLs stored in browser history stack. This is the number of URLs that are stored in the history stack for PREV and NEXT support. This feature is
	included mainly to support hypertext browsing but the history feature can be usefull in applications with hierarchical page format as well. If history is not needed for your
	application you may reduce this	value to 1. Reducing it to zero is not supported.
*/
#define WEBC_CFG_HISTORY_SIZE               20  // Number of URLs stored in the history buffer

// The size of the global string table (DO NOT CHANGE)
#define WEBC_CFG_STRING_TABLE_SIZE          1973 // a prime number


/* Automatically convert Unicode fullwidth ascii characters to normal ascii	(only relevant when WEBC_SUPPORT_UNICODE is enabled) */
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


/** @name WEBC_SUPPORT_VERIFY
	@memo Set to 0 to use only encryption for https:; 1 for authentication as well. Controls if certs received from the server are verified. Set to 0 to use only encryption; 1 for authentication as well.WEBC_SUPPORT_VERIFY is ignored if HTTPS is not enabled.

*/
#if (WEBC_SUPPORT_HTTPS)
#define WEBC_SUPPORT_VERIFY					0 // 1
#else
#define WEBC_SUPPORT_VERIFY					0
#endif

#if(WEBC_SUPPORT_VERIFY==1)

/** @name HTTPS_CERT_FILE
    @memo Path to the certificate file if WEBC_SUPPORT_VERIFY if enabled and HTTPS is enable. Points webC at a file that contains valid certificates. WEBC_SUPPORT_FILE must be enabled to support certificate verification.
*/
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

/** @name  WEBC_CFG_CACHE_PREFIX
    @memo Path prefix to the subdirectory where content will be cached.
	@doc This define points webC at a subdirectory where it should place cache files.
	<pre>
	    Both WEBC_SUPPORT_CACHE and WEBC_SUPPORT_FILE must be enabled and the rtplatform file layer must be supported to support caching.
		Note: Consult webcfg.h for additional cache configuration parameters.
	</pre>
*/
#ifdef __TSDPROJECT__
#define WEBC_CFG_CACHE_PREFIX               "m:\\cache\\"
#define WEBC_CFG_OFFLINE_CACHE_PREFIX       "m:\\archive\\"
#define WEBC_CFG_BMP_CACHE_PREFIX           WEBC_CFG_CACHE_PREFIX "bmp\\"
#else
#define WEBC_CFG_CACHE_PREFIX               "cache\\"
#define WEBC_CFG_OFFLINE_CACHE_PREFIX       "archive\\"
#define WEBC_CFG_BMP_CACHE_PREFIX           WEBC_CFG_CACHE_PREFIX "bmp\\"
#endif

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

#define WEBC_SUPPORT_CACHE_COMPRESSION      0	// Must be zero.
#if (WEBC_SUPPORT_CACHE_COMPRESSION)
#error Cache compression not yet supported in WebC version 2.4 and above
#endif

/* Set to 1 to cache HTML page source.  This #define is meaningful only when  WEBC_SUPPORT_CACHE is set to 1. */
#define WEBC_CACHE_PAGE_SOURCE              0	// Not currently supported

/* Set to 1 to cache binary image data for images, 0 otherwise.  The caching
  is done after the image has been converted, for best performance.  This
  #define is meaningful only when WEBC_SUPPORT_CACHE=1. */
#define WEBC_CACHE_IMAGES                   (WEBC_SUPPORT_CACHE && 0)

/* where to store cached files */
#ifdef __TSDPROJECT__
#define WEBC_CFG_HTTP_CACHE_LOCATION        "m:\\cache\\"
/* The maximum bytes to use for caching HTTP data */
#define WEBC_CFG_HTTP_CACHE_SIZE            (1024L*1024L*2L)

/* The maximum number of files in the HTTP cache */
#define WEBC_CFG_HTTP_CACHE_MAX_FILES       500

/* The default image expire time = 3 min */
#define WEBC_CFG_CACHE_IMG_EXPIRE_TIME      180
#else
#define WEBC_CFG_HTTP_CACHE_LOCATION        "cache\\"
/* The maximum bytes to use for caching HTTP data */
#define WEBC_CFG_HTTP_CACHE_SIZE            (1024L*1024L*2L)

/* The maximum number of files in the HTTP cache */
#define WEBC_CFG_HTTP_CACHE_MAX_FILES       500

/* The default image expire time = 3 min */
#define WEBC_CFG_CACHE_IMG_EXPIRE_TIME      180
#endif

/* define this symbol to use a RAM-based cache for HTTP */
//#define WEBC_HTTP_USE_RAM_CACHE



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
// Note: WEBC_SUPPORT_DEBUG_TIME is deprecated use WEBC_SUPPORT_DEBUG_PROFILE
//#define WEBC_SUPPORT_DEBUG_TIME             0


// Added a new profiling method. in addition to WEBC_SUPPORT_DEBUG_TIME sources are in dbgprofile.h and dbgprofile.c
// Enable this feature to profile execution times.
#define WEBC_SUPPORT_DEBUG_PROFILE            0

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
// Note: WEBC_MEMORY_DEBUG is deprecated use RTP_TRACK_LOCAL_MEMORY in rtplatform\include\rtmem.h instead
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
#define WEBC_TIMEOUT_SEC                    10 // 60

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
#ifdef __TSDPROJECT__
#define FB_FONTS_STORED_IN_FILES 0
#define DEFAULT_FONT_CACHE_SIZE (4*1024*1024)  //4MB
#endif

/* Set to 1 for "normal" browser behavior, Scrollbars appear if a page exceeds the canvas
   if set to 0 no scrollbars appear and the view is clipped */
#define WEBC_SUPPORT_SCROLLBARS 1
#ifdef __TSDPROJECT__
#undef  WEBC_SUPPORT_SCROLLBARS
#define WEBC_SUPPORT_SCROLLBARS 0
#endif

/* Change set for October 2010 release.
   Changes are conditionally included and may be excluded to diagnose issues
   Conditional support will be removed once changes are determined to be stabile
*/
#define USE_EXPERIMENTAL_TIMER_EVENT 		0	// Review source, fixes race conditions caused by the time handler interrupting executing Jscript code.
#define WEBC_SUPPORT_BMP_16BITS 			1	// Added support for 16 bit bit map types (previouskly had only 8 and 24)
#define WEBC_INCLUDE_BMP_PALLETE_FIXES 		1	// Added .bmp files with default color palets (most common case)
#define WEBC_SUPPORT_EMBED					1	// Added limitted support for EMBED Tag (for audio files initially)
#define WEBC_SUPPORT_POSTFILE				0	// Added webc_post_UploadFileASCII() API call
#define WEBC_SUPPORT_METATAG_OPTIMIZATION 	0	// Added hash code to metatag callback dispatcher to speed dispatch.
#define WEBC_INPUT_BUTTON_ELEMENT_FIX		1	// Fixed form post capability to include name_value pair for submit button.
#define WEBC_INCLUDE_FOCUS_FINDER			1	// Added webc_webc_FindFocusElement() API call
#define WEBC_INCLUDE_SEPT2010_BUGFIXES		1	// Several bug fixes for leaks in Javascript, and update errors in FRAMES
#define WEBC_IMAGE_DRAW_EXTENSIONS 			0
//MA
#define WEBC_SUPPORT_HTTP_MACADDR_HEADER	1
#define WEBC_SUPPORT_DEVICE_SERIAL_HEADER	1
#define WEBC_SUPPORT_POST_DATABUFFER		0
//MA NEW_FEATURES_WORKING_ON
#define	WEBC_SUPPORT_TSD_TOKEN_EXCHANGE		1
#define WEBC_SUPPORT_REFERER_HEADER			1

#define INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS		0
#endif // __WEBCFG_H
