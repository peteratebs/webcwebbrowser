
//
// WEBC.H - Structs, prototypes, and defines for WEBC.C
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __WEBC_H__
#define __WEBC_H__


/******************** INCLUDES ********************/

#include "webcfg.h"
#include "filetype.h"
#include "htmlsdk.h"
#include "util.h"
#include "webcdefault.h"
#include "css.h"
#include "dbgprofile.h"

/******************** STRUCTURES ********************/

typedef int  (*WebcUrlMacro) (WEBC_CHAR *param, WEBC_CHAR *replace, int maxLen);
typedef long (*webcStreamFn) (void *handle, WEBC_UINT8 *buffer, long len);
#ifdef __cplusplus
extern "C" {
#endif
typedef int  (*webcIdleFnC)   (WEBC_PFBYTE data);
#ifdef __cplusplus
}
#endif
typedef int  (*webcIdleFn)   (WEBC_PFBYTE data);
typedef int  (*webcReportFn) (WEBC_PFBYTE ctx, WEBC_CHAR *string);

typedef struct s_HTMLRect
{
	DISPLAY_INT left;
	DISPLAY_INT top;
	DISPLAY_INT right;
	DISPLAY_INT bottom;
}
HTMLRect;

// <EDIT 20080120 new screen infor structure
// used by void WebGraphics::GetScreenInfo() queries driver for screen size and resolution.
typedef struct s_HTMLScreenInfo
{
    int availHeight;
    int availWidth ;
    int height;
    int width;
    int colorDepth;
}
HTMLScreenInfo;

typedef struct s_HTMLGraphicsContext HTMLGraphicsContext;

// <EDIT 20080120 move struct s_HTMLGraphicsContext down in the file

typedef int (*ProcessInputQueueFn) (
			void* fn,
			WEBC_BOOL block,
			WEBC_INT32* exitCode);

#ifdef __cplusplus
extern "C" {
#endif
struct HTMLBrowserWindowConfig
{
    DISPLAY_INT top;
    DISPLAY_INT left;
    DISPLAY_INT height;     // 0 is use default
    DISPLAY_INT width;      // 0 is use default
	DISPLAY_INT BorderThickness;	
	WEBC_UINT32	BorderColor;
	WEBC_BOOL   SetBodyBackGroundColor;
	WEBC_UINT32 BackGroundColor;
#define BS_RAISED 				1  /* These 4 styles are exclusive of each other */
#define BS_LOWERED				2
#define BS_RIDGE				4
#define BS_GROOVE				8
	WEBC_UINT32 BorderStyle;
#define WF_RESIZABLE 			0x01
#define WF_DISABLESCROLLBARS 	0x02
#define WF_MODAL 				0x04
#define WF_SAVEUNDER			0x08
#define WF_SPRITE			    0x10
#define WF_STARTINBG			0x20
	WEBC_UINT32 WindowFlags;
	/* Legacy values. Not used by framebuffer window manager */
#if (1)
    WEBC_BOOL is_resizable; // note: should also support "resizeable"
    WEBC_BOOL has_status;
    WEBC_BOOL has_menubar;
    WEBC_BOOL has_location;
    WEBC_BOOL has_toolbar;
#endif
};

/* Added back in to compile */
typedef struct s_HTMLGraphicsContext HTMLGraphicsContext;

typedef int (*ProcessInputQueueFn) (
			void* fn,
			WEBC_BOOL block,
			WEBC_INT32* exitCode);


struct HTMLBrowserConfig
{
	HTMLGraphicsContext** graphics;
	HBROWSER_HANDLE BrowserHandle;
	/* return 1 if message processed, 0 if not, -1 if destroyed */
	ProcessInputQueueFn* processInputQueue;

	HTMLEventStatus (*browserEventHandler) (
			HBROWSER_HANDLE hbrowser,
			HTMLBrowserEvent* event);
	void* privateData;
// <EDIT 20080120 add window configuration options that are passed to all layers of the window.open process
    WEBC_CHAR *WindowName;
    WEBC_CHAR *WindowTitle;
    void *WindowHandle;
    struct HTMLBrowserWindowConfig WindowConfig;
};
//#ifdef __cplusplus
#ifdef __cplusplus
}
#endif
struct s_HTMLGraphicsContext
{
	/*---------------------------------------------------------------*/
	// <EDIT 20080120 Add window open and close
	// Windowing services
    int (*WindowOpen) (
		HTMLGraphicsContext** ctx,
		struct HTMLBrowserConfig *pconfig,
        const WebChar *title
        );

    void (*WindowClose) (
		HTMLGraphicsContext** ctx
        );

    void (*MoveTo) (
		HTMLGraphicsContext** ctx,
		DISPLAY_INT x,
		DISPLAY_INT y,
        WEBC_BOOL is_absolute
        );

    void (*ScreenInfo) (
		HTMLGraphicsContext** ctx,
		HTMLScreenInfo *pinfo
        );

	// Canvas services

	void (*getCanvasRect) (
			HTMLGraphicsContext** ctx,
			HTMLRect* rect);

	/*---------------------------------------------------------------*/
	// Pointer services

	void (*claimPointer) (
			HTMLGraphicsContext** ctx);

	void (*releasePointer) (
			HTMLGraphicsContext** ctx);

	/*---------------------------------------------------------------*/
	// Drawing control

	// Invalidate adds a rectangular region to the portion of the graphics context already marked dirty.
	//  The entire dirty region is what will be updated when endDraw is called.
	void (*invalidateRect) (
			HTMLGraphicsContext** ctx,
			HTMLRect* rect);

	void (*beginDrawing) (
			HTMLGraphicsContext** ctx);

	void (*getClipRect) (
			HTMLGraphicsContext** ctx,
			HTMLRect* rect);

	void (*setClipRect) (
			HTMLGraphicsContext** ctx,
			HTMLRect* rect);

	void (*endDrawing) (
			HTMLGraphicsContext** ctx);

	/*---------------------------------------------------------------*/
	// Color services

	HTMLColor (*colorToRGBA) (
			HTMLGraphicsContext** ctx,
			WebColor color);

	WebColor (*RGBAToColor) (
			HTMLGraphicsContext** ctx,
			HTMLColor rgba,
			HTMLColor* error);

	WebColor (*getColorByIndex) (
			HTMLGraphicsContext** ctx,
			int index);

	/*---------------------------------------------------------------*/
	// Drawing services

	void (*drawText) (
			HTMLGraphicsContext** ctx,
			DISPLAY_INT x,
			DISPLAY_INT y,
			const WebChar* text,
			WebColor textColor,
			WebColor fillColor,
			WEBC_BOOL fill,
			void* font);

	void (*drawTextLen) (
			HTMLGraphicsContext** ctx,
			DISPLAY_INT x,
			DISPLAY_INT y,
			const WebChar* text,
			WebColor textColor,
			WebColor fillColor,
			WEBC_BOOL fill,
			void* font,
			long textLen);

	void (*drawBitmap) (
			HTMLGraphicsContext** ctx,
			DISPLAY_INT x,
			DISPLAY_INT y,
			void* bmp);

	void (*drawBitmapStretchedToRect) (
			HTMLGraphicsContext** ctx,
			HTMLRect* rect,
			void* bmp);

	void (*drawRectangle) (
			HTMLGraphicsContext** ctx,
			HTMLRect* rect,
			WebColor outlineColor,
			WebColor fillColor,
			WEBC_BOOL fill);

	/*---------------------------------------------------------------*/
	// Font services

	void* (*createFont) (
			HTMLGraphicsContext** ctx,
			const WEBC_CHAR* familyName,
			const WEBC_CHAR* genericName,
			WEBC_UINT16 pointSize,
			WEBC_UINT16 attrib,
			int familyIndex);

	void* (*getDefaultFont) (
			HTMLGraphicsContext** ctx);

	void (*destroyFont) (
			HTMLGraphicsContext** ctx,
			void* font);

	int (*getNumFontFamilies) (
			HTMLGraphicsContext** ctx);

	const WEBC_CHAR** (*getFontFamilyNames) (
			HTMLGraphicsContext** ctx);

	const WEBC_CHAR** (*getFontFamilyGenericNames) (
			HTMLGraphicsContext** ctx);

	DISPLAY_INT (*getTextHeight) (
			HTMLGraphicsContext** ctx,
			const WebChar* text,
			void* font);

	DISPLAY_INT (*getTextWidth) (
			HTMLGraphicsContext** ctx,
			const WebChar* text,
			void* font);

	DISPLAY_INT (*getTextHeightLen) (
			HTMLGraphicsContext** ctx,
			const WebChar *text,
			void* font,
			long textLen);

	DISPLAY_INT (*getTextWidthLen) (
			HTMLGraphicsContext** ctx,
			const WebChar *text,
			void* font,
			long textLen);

	DISPLAY_INT (*getFontHeight) (
			HTMLGraphicsContext** ctx,
			void* font);

	DISPLAY_INT (*getFontBaseline) (
			HTMLGraphicsContext** ctx,
			void* font);

	/*---------------------------------------------------------------*/
	// Bitmap services

	void* (*createWebBitmap) (
			HTMLGraphicsContext** ctx,
			const WebGraphBitmapInfo* info);

	void (*destroyWebBitmap) (
			HTMLGraphicsContext** ctx,
			void* bmp);

	void* (*createStretchedBitmap) (
			HTMLGraphicsContext** ctx,
			void* src,
			DISPLAY_INT w,
			DISPLAY_INT h);

	void (*destroyStretchedBitmap) (
			HTMLGraphicsContext** ctx,
			void* bmp);

	DISPLAY_INT (*getBitmapWidth) (
			HTMLGraphicsContext** ctx,
			void* bmp);

	DISPLAY_INT (*getBitmapHeight) (
			HTMLGraphicsContext** ctx,
			void* bmp);

	/*---------------------------------------------------------------*/
	// Optional services (defining these methods will speed things up)
	void (*optionalDrawHorizontalLine) (
			HTMLGraphicsContext** ctx,
			DISPLAY_INT y,
			DISPLAY_INT left,
			DISPLAY_INT right,
			WebColor color,
			DISPLAY_INT thickness);

	void (*optionalDrawVerticalLine) (
			HTMLGraphicsContext** ctx,
			DISPLAY_INT x,
			DISPLAY_INT top,
			DISPLAY_INT bottom,
			WebColor color,
			DISPLAY_INT thickness);

	void (*optionalDrawStyledFrame) (
			HTMLGraphicsContext** ctx,
			HTMLRect* rect,
			WebColor color,
			DISPLAY_INT thickness,
			CSSBorderStyle style);

	void (*optionalDrawBitmapTiled) (
			HTMLGraphicsContext** ctx,
			HTMLRect* rect,
			DISPLAY_INT patternXOffset,
			DISPLAY_INT patternYOffset,
			void* bmp);

	// used to re-size the browser window if this feature is supported
	void (*optionalSetCanvasRect) (
			HTMLGraphicsContext** ctx,
			HTMLRect* rect);

	// draw a vertical scroll bar
	WEBC_BOOL (*optionalDrawVScroll) (
			HTMLGraphicsContext** ctx,
			HTMLRect* rect,
			WEBC_UINT32 buttonLength,
			WEBC_UINT32 sliderBegin,
			WEBC_UINT32 sliderLength,
			WEBC_UINT32 scrollFlags /* bit-wise combination of the following: */ );

	  #define HTML_SCROLL_FLAG_DECREASE_ACTIVE      0x00000001  // the decrease scroll position button is active
	  #define HTML_SCROLL_FLAG_INCREASE_ACTIVE      0x00000002  // the increase scroll position button is active
	  #define HTML_SCROLL_FLAG_SLIDER_ACTIVE        0x00000004  // the slider is active
	  #define HTML_SCROLL_FLAG_BEFORE_SLIDER_ACTIVE 0x00000008  // the region representing the range below the visible region is active
	  #define HTML_SCROLL_FLAG_AFTER_SLIDER_ACTIVE  0x00000010  // the region representing the range above the visible region is active
	  #define HTML_SCROLL_FLAG_HAS_FOCUS            0x00000020  // the scroll bar has input focus
	  #define HTML_SCROLL_FLAG_LEFT_FLUSH           0x00000040  // the scroll bar is flush against the left side of the parent rect
	  #define HTML_SCROLL_FLAG_TOP_FLUSH            0x00000080  // the scroll bar is flush against the top side of the parent rect
	  #define HTML_SCROLL_FLAG_RIGHT_FLUSH          0x00000100  // the scroll bar is flush against the right side of the parent rect
	  #define HTML_SCROLL_FLAG_BOTTOM_FLUSH         0x00000200  // the scroll bar is flush against the bottom side of the parent rect
	  #define HTML_SCROLL_FLAG_DISABLED             0x00000400  // the scroll bar is disabled
	  #define HTML_SCROLL_FLAG_READONLY             0x00000800  // the scroll bar is in a read-only mode

	// same as optionalDrawVScroll above, but for horizontal scroll bars
	WEBC_BOOL (*optionalDrawHScroll) (
			HTMLGraphicsContext** ctx,
			HTMLRect* rect,
			WEBC_UINT32 buttonLength,
			WEBC_UINT32 sliderBegin,
			WEBC_UINT32 sliderLength,
			WEBC_UINT32 scrollFlags);
	// optional support for CSS opacity
	void * (*pushCanvas)(HTMLGraphicsContext** ctx, HTMLRect* rect);
	void (*popCanvas)(HTMLGraphicsContext** ctx, void *pvretPrevCtx, int opacity);

#ifdef WEBC_BUFFER_SCROLLING

	HTMLGraphicsContext** (*createBuffer) (
			HTMLGraphicsContext** ctx,
			DISPLAY_INT w,
			DISPLAY_INT h);

	HTMLGraphicsContext** (*resizeBuffer) (
			HTMLGraphicsContext** ctx,
			HTMLGraphicsContext** buffer,
			DISPLAY_INT w,
			DISPLAY_INT h);

	void (*destroyBuffer) (
			HTMLGraphicsContext** ctx,
			HTMLGraphicsContext** buffer);

	void (*copyBufferRegion) (
			HTMLGraphicsContext** dstCtx,
			HTMLRect* dstRect,
			HTMLGraphicsContext** srcBuffer,
			DISPLAY_INT srcX,
			DISPLAY_INT srcY);

#endif // WEBC_BUFFER_SCROLLING

#if(WEBC_IMAGE_DRAW_EXTENSIONS)
	int (*createGenericBitmap) (
		void* webCBitmapSrc,
		HTMLGraphicsContext** ctx,
		WebGraphBitmapInfo** genericBitMapDst);
#endif
};


/* Types related to META tag processing */
typedef enum
{
	HTML_META_STATUS_CONTINUE,
	HTML_META_STATUS_STOP,
	HTML_META_STATUS_HALT
} HTMLMetaStatus;

// webC vfile declarations
struct WebcVFileEntry;
typedef void (*VFileDeleteFn)(struct WebcVFileEntry *pFile);
struct WebcVFileEntry
{
	char*               pName;
	WebcFileContentType iMimeType;
	long                lSize;
	WEBC_PFBYTE         pData;
	WEBC_UINT16         wFlags;

#define WEBC_VFILE_COPY        0x01  // Create a copy of the data
#define WEBC_VFILE_MORTAL      0x02  // Can be deleted; if not set, webc_DeleteVFile will fail.
#define WEBC_VFILE_CREATED     0x04
#define WEBC_VFILE_FOLDER      0x08  // Is a subdirectory record. If so then the number of entries in the folder is lSize
#define WEBC_VFILE_INROOT	   0x10  // Resides in the root. We need this to determine the number of files in the root since there is no directory entry with a count
	VFileDeleteFn       Destroy; // Function to call when/if this entry is deleted.
	enum WebcCharset    charset;
};


// error codes for URL loading routines; passed to write_error_box()
#define WEBC_EBADFILE              -2
#define WEBC_EBADPROTOCOL          -3
#define WEBC_EFILEREADFAILED       -4


// values for mime_data_type field in httpc_ctx
// NOTE: used for files also but set based upon file extension instead
//       of mime header
#define HTML_MIME_TYPE_UNKNOWN      WEBC_FILE_TYPE_UNKNOWN
#define HTML_MIME_TYPE_HTML         WEBC_FILE_TYPE_TEXT_HTML
#define HTML_MIME_TYPE_GIF          WEBC_FILE_TYPE_IMAGE_GIF
#define HTML_MIME_TYPE_JPEG         WEBC_FILE_TYPE_IMAGE_JPEG
#define HTML_MIME_TYPE_PLAIN_TEXT   WEBC_FILE_TYPE_TEXT_PLAIN
#define HTML_MIME_TYPE_CSS          WEBC_FILE_TYPE_TEXT_CSS
#define HTML_MIME_TYPE_JSCRIPT      WEBC_FILE_TYPE_APPLICATION_X_JAVASCRIPT
#define HTML_MIME_TYPE_PNG          WEBC_FILE_TYPE_IMAGE_PNG
#define HTML_MIME_TYPE_MNG          WEBC_FILE_TYPE_VIDEO_X_MNG
#define HTML_MIME_TYPE_BMP          WEBC_FILE_TYPE_IMAGE_BMP
#define HTML_MIME_TYPE_MOBILE_HTML	WEBC_FILE_TYPE_TEXT_VND_WAP_WML
#define	HTML_MIME_TYPE_XHTML	WEBC_FILE_TYPE_APPLICATION_XHTML

#ifdef __cplusplus
extern "C" {
#endif

extern WEBC_READONLY struct wcache_system_spec gWebcFileCacheSpec;
#if (WEBC_SUPPORT_OFFLINE_BROWSING)
extern WEBC_READONLY struct wcache_system_spec gWebcRamCacheSpec;
#endif


/******************** FUNCTION PROTOTYPES ********************/

#if (WEBC_DEBUG_PARSER)
void WriteParseLog(char *comment, const WEBC_CHAR* pString);
#endif

#if (WEBC_SUPPORT_INTERNAL)
char *webc_CreateVFile (const char *pName, int iMimeType, long lSize, WEBC_PFBYTE pData, WEBC_UINT16 wFlags, VFileDeleteFn Destructor);
int   webc_DeleteVFile (const char *pName);
#endif


// API for managing URL Macros
int webc_define_url_macro     (char *name, WebcUrlMacro macro);
int webc_undef_url_macro      (char *name);
int webc_undef_all_url_macros (void);
#if (WEBC_SUPPORT_URL_MACROS)
int webc_replace_macros       (WEBC_CHAR *dest, const WEBC_CHAR *src, long maxLen);
#endif

// string pointers returned by the string table routines are unique
//  per actual string
void             webc_string_table_init (unsigned int size);
void             webc_string_table_exit (void);
const WEBC_CHAR* webc_string_table_add (const WEBC_CHAR *str, const char *file, unsigned int line);
const WEBC_CHAR* webc_string_table_add_lowercase (const WEBC_CHAR *str, const char *file, unsigned int line);
void             webc_string_table_remove (const WEBC_CHAR *str, const char *file, unsigned int line);

// utility routines
int  webc_escape_form_fields (WEBC_CHAR *dest, const WEBC_CHAR *src);
long webc_escape_string      (WEBC_CHAR *escstr, const char *str);

// user supplied function

// Alert box provided by graphics porting layer
void webc_user_alert_box(char *str);
// Output box provided by graphics porting layer
void webc_user_debug_window(char *str);

#if (WEBC_SUPPORT_OFFLINE_BROWSING)
extern int giWebcOnline;
int webc_online(void);
#endif

#ifdef __cplusplus
 class HTMLBrowser;

#ifdef WEBC_OLD_LOAD_METHOD
 int webc_thread_set_loading(void);
 int webc_thread_load_all(void);
 int webc_thread_queue_browser_for_load(HTMLBrowser *pBrowser);
 int webc_thread_load_browser(HTMLBrowser *pBrowser);
#endif // WEBC_OLD_LOAD_METHOD

HTMLBrowser *_webc_NewBrowser(HTMLBrowserConfig *pConfig, WEBC_BOOL CreateWindow, WEBC_CHAR *pWindowName, WEBC_CHAR *pHomePage);

// *********************************************************************

#endif // __cplusplus

#ifdef __cplusplus
}
#endif

#endif

// END WEBC.H
