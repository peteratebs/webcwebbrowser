#ifndef __HTMLSDK_INCLUDED__
#define __HTMLSDK_INCLUDED__
//
// HTMLSDK.H -
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/
#include "webcfg.h"
#include "webcutf.h"
#include "html.h"
#include "wgkeys.hpp"
#include "filetype.h"
//#include "listener.hpp"

/*****************************************************************************/
// struct/enum/union Definitions
/*****************************************************************************/

// Predefinitions
struct s_HTMLEvent;
struct HTMLBrowserConfig;

// Fix me comments
#define URL_ERROR_IO             -1
#define URL_ERROR_MEMORY         -2
#define URL_ERROR_NOT_MODIFIED   -3
#define URL_ERROR_REDIRECT       -4
#define URL_ERROR_NOT_AVAILABLE  -5
#define URL_ERROR_SERVER_NOT_FOUND -6
#define URL_ERROR_PAYMENT_REQ      -402
#define URL_ERROR_FORBIDDEN        -403
#define URL_ERROR_PAGE_NOT_FOUND   -404
#define URL_ERROR_PAGE_NOT_SET	   -500

/** HTML event type
 @see HTMLEvent
 */
enum e_HTMLEventType
{
	/// Matches any event type
	HTML_EVENT_ANY=-1,
	HTML_EVENT_NONE=0,
	/// Single mouse click
	HTML_EVENT_CLICK,
	/// Double mouse click
	HTML_EVENT_DBLCLICK,
	/// Key pressed
	HTML_EVENT_KEYDOWN,
	/// Key pressed and released
	HTML_EVENT_KEYPRESS,
	/// Key released
	HTML_EVENT_KEYUP,
	/// Mouse button pressed
	HTML_EVENT_MOUSEDOWN,
	/// Mouse movement over this object
	HTML_EVENT_MOUSEMOVE,
	/// Mouse pointer exits the client area of this object
	HTML_EVENT_MOUSEOUT,
	/// Mouse pointer enters the client area
	HTML_EVENT_MOUSEOVER,
	/// Mouse button released
	HTML_EVENT_MOUSEUP,
	/// Object has received input focus
	HTML_EVENT_FOCUS,
	/// Object has lost input focus
	HTML_EVENT_UNFOCUS,
	/// Load completes
	HTML_EVENT_LOAD,
	/// Object is discarded
	HTML_EVENT_UNLOAD,
	/// For forms only; submit is pressed
	HTML_EVENT_SUBMIT,
	/// Input text was edited
	HTML_EVENT_CHANGE,
	/// Finished Editing on a Textbox
	HTML_EVENT_EDIT,
	/// For forms only; reset is pressed
	HTML_EVENT_RESET,
	/// HTMLElement object created
	HTML_EVENT_OBJECT_CREATED,
	/// HTMLElement object destroyed
	HTML_EVENT_OBJECT_DESTROYED,
	/// Attribute parsed
	HTML_EVENT_ATTRIBUTE_PARSED,
	/// mouse capture released
	HTML_EVENT_LOSECAPTURE,
#if (WEBC_SUPPORT_AJAX)
	/// XMLHttpRequest.onreadystatechange event
	HTML_EVENT_ONREADYSTATECHANGE,
#endif
	HTML_EVENT_TIMER,
	HTML_EVENT_RENDER,
	HTML_EVENT_WINDOW_REFRESHED,
	NUM_HTML_EVENTS,
	HTML_FIRST_USER_EVENT
};
typedef enum e_HTMLEventType HTMLEventType;


enum e_HTMLEventStatus
{
	HTML_EVENT_STATUS_DONE,
	HTML_EVENT_STATUS_CONTINUE,
	HTML_EVENT_STATUS_HALT,
	HTML_EVENT_STATUS_ERROR
};
typedef enum e_HTMLEventStatus HTMLEventStatus;


enum e_HTMLBrowserEventType
{
	HTML_BROWSER_EVENT_BLANK_DOCUMENT_REQUESTED = 0,
	HTML_BROWSER_EVENT_DOCUMENT_TITLE_CHANGE,
	HTML_BROWSER_EVENT_DOCUMENT_URL_CHANGE,
	HTML_BROWSER_EVENT_HTTP_NOT_AUTHORIZED,
	HTML_BROWSER_EVENT_HTTP_AUTHORIZATION_REJECTED,
	HTML_BROWSER_EVENT_LOAD_DONE,
	HTML_BROWSER_EVENT_LOAD_START,
	HTML_BROWSER_EVENT_STATUS_CHANGE,
	HTML_BROWSER_EVENT_UNKNOWN_CONTENT_TYPE,
	HTML_BROWSER_EVENT_URL_REQUEST,
	HTML_BROWSER_EVENT_HTML_PAGE_PREFIX_REQUEST,
	HTML_BROWSER_NUM_EVENTS
};
typedef enum e_HTMLBrowserEventType HTMLBrowserEventType;

/** HTML element type
 @see HELEMENT_HANDLE, webc_ElementGetType
 */
enum e_HTMLElementType
{
	HTML_ELEMENT_HAS_TAB_INDEX=-3,
	/// Matches any element type when searching
	HTML_ELEMENT_ANY=-2,
	HTML_ELEMENT_NONE=-1,
	/// <ADDRESS>
	HTML_ADDRESS_ELEMENT=0,
	/// <A>
	HTML_ANCHOR_ELEMENT,
	/// <AREA>
	HTML_AREA_ELEMENT,
	/// <BASE>
	HTML_BASE_ELEMENT,
	/// <BIG>
	HTML_BIG_ELEMENT,
	/// <BLOCKQUOTE>
	HTML_BLOCKQUOTE_ELEMENT,
	/// <BODY>
	HTML_BODY_ELEMENT,
	/// <B>
	HTML_BOLD_ELEMENT,
	/// <BR>
	HTML_BR_ELEMENT,
	/// <INPUT type=submit>
	HTML_BUTTON_ELEMENT,
	/// <CENTER>
	HTML_CENTER_ELEMENT,
	/// <INPUT type=checkbox>
	HTML_CHECKBOX_ELEMENT,
	/// <CITE>
	HTML_CITE_ELEMENT,
	/// <CODE>
	HTML_CODE_ELEMENT,
	/// <DD>
	HTML_DEFINITION_ELEMENT,
	/// <DL>
	HTML_DEFINITION_LIST_ELEMENT,
	/// <DT>
	HTML_DEFINITION_TERM_ELEMENT,
	/// <DEL>
	HTML_DEL_ELEMENT,
	/// <DIV>
	HTML_DIV_ELEMENT,
	/// <INPUT type=text>
	HTML_EDIT_STR_ELEMENT,
	/// <TEXTAREA>
	HTML_EDITBOX_ELEMENT,
	HTML_ELEMENT,
#if (WEBC_SUPPORT_EMBED)
	///<EMBED>
	HTML_EMBED_ELEMENT,
#endif
	/// <EM>
	HTML_EMPHASIZE_ELEMENT,
	/// Abstract class for all block-level elements
	HTML_FLOW_ELEMENT,
	/// <FONT>
	HTML_FONT_ELEMENT,
	/// <FORM>
	HTML_FORM_ELEMENT,
	/// <FRAME>
	HTML_FRAME_ELEMENT,
	/// <FRAMESET>
	HTML_FRAMESET_ELEMENT,
	/// <H1> ... <H6>
	HTML_HEADING_ELEMENT,
	/// <INPUT type=hidden>
	HTML_HIDDEN_INPUT_ELEMENT,
	/// <HR>
	HTML_HORIZONTAL_RULE_ELEMENT,
	/// <IFRAME>
	HTML_IFRAME_ELEMENT,
	/// <IMG>
	HTML_IMAGE_ELEMENT,
	/// <INPUT type=button>
	HTML_INPUT_BUTTON_ELEMENT,
	/// <INPUT type=pass>
	HTML_INPUT_PASSWORD_ELEMENT,
	/// <INPUT type=reset>
	HTML_INPUT_RESET_ELEMENT,
	/// <INS>
	HTML_INS_ELEMENT,
	/// <I>
	HTML_ITALIC_ELEMENT,
	/// <KBD>
	HTML_KBD_ELEMENT,
	/// <LABEL>
	HTML_LABEL_ELEMENT,
	/// <UL>, <OL>
	HTML_LIST_ELEMENT,
	/// <LI>
	HTML_LIST_ITEM_ELEMENT,
	/// <MAP>
	HTML_MAP_ELEMENT,
	/// <NOBR>
	HTML_NOBR_ELEMENT,
	/// <OPTION>
	HTML_OPTION_ELEMENT,
	/// <P>
	HTML_PARAGRAPH_ELEMENT,
	/// <PRE>
	HTML_PRE_ELEMENT,
	/// <INPUT type=radio>
	HTML_RADIO_BUTTON_ELEMENT,
	/// <SCRIPT>
	HTML_SCRIPT_ELEMENT,
	/// <SELECT>
	HTML_SELECT_ELEMENT,
	/// <SMALL>
	HTML_SMALL_ELEMENT,
	/// <SPACER>
	HTML_SPACER_ELEMENT,
	/// <SPAN>
	HTML_SPAN_ELEMENT,
	/// <STRIKE>
	HTML_STRIKE_ELEMENT,
	/// HTML text data
	HTML_STRING_ELEMENT,
	/// <STRONG>
	HTML_STRONG_ELEMENT,
	/// <STYLE>
	HTML_STYLE_ELEMENT,
	/// <TABLE>
	HTML_TABLE_ELEMENT,
	/// <TD>
	HTML_TABLE_CELL_ELEMENT,
	/// <TR>
	HTML_TABLE_ROW_ELEMENT,
	/// <U>
	HTML_UNDERLINE_ELEMENT,
	NUM_HTML_ELEMENT_TYPES
};
typedef enum e_HTMLElementType HTMLElementType;


enum e_DISPLAYElementType
{
	DISPLAY_ELEMENT_NONE=-1,
    DISPLAY_ELEMENT,
    DISPLAY_ELEMENT_CONTENTHOLDER,
    DISPLAY_ELEMENT_HTMLFLOWDISPLAY,
    DISPLAY_ELEMENT_HTMLAREADISPLAY,
    DISPLAY_ELEMENT_FRAMESATETDISPLAY,
    DISPLAY_ELEMENT_HRDISPLAY,
    DISPLAY_ELEMENT_IMAGEDISPLAY,
    DISPLAY_ELEMENT_ITEMBULLETDISPLAY,
    DISPLAY_ELEMENT_STRINGDISPLAY,
    DISPLAY_ELEMENT_TABLEDISPLAY,
    DISPLAY_ELEMENT_DISPLAYMANAGER,
    DISPLAY_ELEMENT_KEYCATCHER,
    DISPLAY_ELEMENT_WEBBUTTON,
    DISPLAY_ELEMENT_WEBCHECKBOX,
    DISPLAY_ELEMENT_WEBDROPBOX,
    DISPLAY_ELEMENT_WEBEDITSTRING,
    DISPLAY_ELEMENT_WEBLISTBOX,
    DISPLAY_ELEMENT_WEBRADIOBUTTON,
    DISPLAY_ELEMENT_WEBSCROLL,
    DISPLAY_ELEMENT_WEBSLIDER,
    DISPLAY_ELEMENT_ZORDERED
};
typedef enum e_DISPLAYElementType DISPLAYElementType;




/*****************************************************************************/
// Typedefs
/*****************************************************************************/

/**

 @memo Cookie handle type
 @doc

 @see
 */
typedef WEBC_UINT8* COOKIE_HANDLE;

/**

 @memo script handle type
 @doc

 @see
 */
typedef WEBC_UINT8* HSCRIPT_HANDLE;

/**

 @memo HTML Browser handle type
 @doc

 @see
 */
typedef WEBC_UINT8* HBROWSER_HANDLE;

/**
 @memo HTML Element handle type

 @doc (see also \Ref{Introduction}, \Ref{Working with HTML Elements})

      HTML Elements are the internal objects used to represent HTML tags and
      intervening text.  The children of an HTML element are the HTML elements
      that represent the tags and text fragments that are contained withing
      that tag in the original source document.  (The various HTML element
      types are given in the \Ref{HTMLElementType} enumeration.)  All HTML elements
      share a common set of core attributes.  In addition, certain element
      types also have attributes that are special to their type; for
      example, all instances of \Ref{HTML_IMAGE_ELEMENT} have a source image
      URL associated with them.

      The list of properties accessible through
      this API, the element types that pertain to each property, and the
      functions used to access and/or modify them are listed below:

 
 */
typedef WEBC_UINT8* HELEMENT_HANDLE;
//@{
/** Document-unique identifier string.
 @name ID
 @doc  Pertains to: All elements

 @see  webc_ElementGetId
 */

/** Element type.
 @name TYPE
 @doc  Pertains to: All elements

 @see  webc_ElementGetType, HTMLElementType
 */

 /** Element name; not necessarily unique.
 @name NAME
 @doc  Pertains to: All elements

 @see  webc_ElementGetName, webc_ElementSetName
 */

 /** The current value of an input field.
 @name VALUE
 @doc  Pertains to:
       \begin{itemize}
       \item \Ref{HTML_EDIT_STR_ELEMENT}
       \item \Ref{HTML_EDITBOX_ELEMENT}
       \item \Ref{HTML_BUTTON_ELEMENT}
       \item \Ref{HTML_CHECKBOX_ELEMENT}
       \item \Ref{HTML_HIDDEN_INPUT_ELEMENT}
       \item \Ref{HTML_OPTION_ELEMENT}
       \item \Ref{HTML_SELECT_ELEMENT}
       \item \Ref{HTML_RADIO_BUTTON_ELEMENT}
       \end{itemize}

 @see  webc_ElementGetValue, webc_ElementSetValue
 */

 /** The element's \Ref{VALUE} if selected, NULL otherwise.
 @name CHECKED
 @doc  Pertains to:
        \begin{itemize}
		\item \Ref{HTML_CHECKBOX_ELEMENT}
		\item \Ref{HTML_RADIO_BUTTON_ELEMENT}
		\end{itemize}

 @see  webc_ElementGetChecked, webc_ElementSetChecked
 */

 /** The URL of the element's image or document source file.
 @name SRC
 @doc  Pertains to:
        \begin{itemize}
		\item \Ref{HTML_IMAGE_ELEMENT}
		\item \Ref{HTML_FRAME_ELEMENT}
		\end{itemize}

 @see  webc_ElementGetSrc, webc_ElementSetSrc
 */

 /** Foreground color of the element.
 @name COLOR
 @doc  Pertains to: All elements

 @see  webc_ElementSetColor, html_color
 */

 /** Background color of the element.
 @name BGCOLOR
 @doc  Pertains to: All elements

 @see  webc_ElementGetBgColor, html_color
 */

/** Url of the element's background bitmap.
 @name BGIMAGE
 @doc  Pertains to:
        \begin{itemize}
		\item \Ref{HTML_BODY_ELEMENT}
		\item \Ref{HTML_TABLE_ELEMENT}
		\item \Ref{HTML_TABLE_CELL_ELEMENT}
		\end{itemize}

 @see  webc_ElementGetBgImage, webc_ElementSetBgImage
 */

/** (x,y) The position in pixels of an element relative to its parent.
 @name POSITION
 @doc  Pertains to: All elements

 @see  webc_ElementGetPosition, webc_ElementSetPosition
 */

/** Width of the element in pixels.
 @name WIDTH
 @doc  Pertains to: All elements

  */

/** Height of the element in pixels.
 @name HEIGHT
 @doc  Pertains to: All elements

 @see  webc_ElementSetHeight
 */

/** Whether the element is visible.
 @name VISIBILITY
 @doc  Pertains to: All elements

 @see  webc_ElementSetHidden, webc_ElementSetVisible
 */

/** Style sheet information for this element.
 @name STYLE
 @doc  Pertains to: All elements

 @see  webc_ElementSetStyle
 */

//@}


/**

 @memo HTML Document handle type
 @doc HTML Documents are the internal representation of an HTML document.
      They consist of an HTML_BODY_ELEMENT which represents the <BODY>
      tag (implicit or explicit), and acts as the root of a tree structure
      of HTML elements (see \Ref{HELEMENT_HANDLE}).  The WebC HTML
      SDK API provides a call (\Ref{webc_DocFindElement}) for searching
      the document tree for a specific element.  Also provided are
      calls for setting the document URL (\Ref{webc_DocSetUrl}) and refreshing
      the display (\Ref{webc_DocRefresh}).

 @see webc_DocFindElement, webc_DocSetUrl, webc_DocRefresh, HELEMENT_HANDLE
 */
typedef WEBC_UINT8* HDOC_HANDLE;
///

/**
 @memo  HTML event data type
 @doc This structure is passed to all event handler routines when a user event occurs.
\begin{verbatim}
    struct HTMLEvent
    {
        HTMLEventType type;
        union
        {
            struct
            {
                short x;
                short y;
            } position;

			struct
			{
				const WEBC_CHAR* name;
				const WEBC_CHAR* value;
			} attribute;
			struct
			{
				enum NotifierType Notifier;
				enum NotifyEventType NotifyEvent;
			} NotifyMessage;
            unsigned short key;
            WEBC_CHAR      *string;
            unsigned char *buffer;
		} data;
    };
\end{verbatim}

 @field type The type of the event that occurred
 @field data Event type-specific data
 @see   HTMLEventType
 */
struct s_HTMLEvent
{
#ifdef __cplusplus
	s_HTMLEvent () { flags = 0; }
#endif

	HTMLEventType type;
	union
	{
		struct
		{
			long x;
			long y;
		} position;

		struct
		{
			const WEBC_CHAR* name;
			const WEBC_CHAR* value;
		} attribute;
		struct
		{
			int /*enum NotifierType*/ Notifier;
			int /*enum NotifyEventType*/ NotifyEvent;
		} NotifyMessage;

		WebKey         key;
		WEBC_CHAR*     string;
		unsigned char* buffer;
	} data;

	HELEMENT_HANDLE elem;
	HELEMENT_HANDLE target;
	HELEMENT_HANDLE srcElement;

	WEBC_UINT8 flags; // see flag definitions below
	int returnValue;
};
typedef struct s_HTMLEvent HTMLEvent;

typedef enum e_HTMLEventFlags
{
	HTML_EVENT_FLAG_CANCEL_BUBBLE    = 0x01,
	HTML_EVENT_FLAG_CANCEL_DEFAULT   = 0x02,
	HTML_EVENT_FLAG_CANCEL_SET_FOCUS = 0x04,
	HTML_EVENT_FLAG_SHIFT_DOWN       = 0x08,
	HTML_EVENT_FLAG_CTRL_DOWN        = 0x10,
	HTML_EVENT_FLAG_ALT_DOWN         = 0x20
}
HTMLEventFlags;

#define WEBC_MAX_AUTH_URL_LEN    256
#define WEBC_MAX_AUTH_REALM_LEN  64
#define WEBC_MAX_AUTH_SCHEME_LEN 16

/* Types related to HTTP authentication */
typedef struct s_HTTPAuthDomain
{
	char url    [WEBC_MAX_AUTH_URL_LEN];
	char realm  [WEBC_MAX_AUTH_REALM_LEN];
	char scheme [WEBC_MAX_AUTH_SCHEME_LEN];
}
HTTPAuthDomain;

typedef struct s_WebcWriteStream
{
	WEBC_INT32 (*writeFn) (struct s_WebcWriteStream** s, const WEBC_UINT8* data, WEBC_INT32 len);
	void       (*closeFn) (struct s_WebcWriteStream** s, int status);
}
WebcWriteStream;

typedef struct s_WebcReadStream
{
	WEBC_INT32 (*readFn)  (struct s_WebcReadStream** s, WEBC_UINT8* data, WEBC_INT32 len);
	void       (*closeFn) (struct s_WebcReadStream** s);
}
WebcReadStream;

struct s_HTMLBrowserEvent
{
	HTMLBrowserEventType  type;

	union
	{
		const WEBC_CHAR*  string;

		struct
		{
			const WEBC_CHAR*    url;
			WebcFileContentType contentType;
			enum WebcCharset    charEncoding;
		}
		file;

		struct
		{
			const WEBC_CHAR* url;
			const WEBC_CHAR* base;
			WEBC_UINT32      id;
		}
		request;

		HTTPAuthDomain    authDomain;
	}
	data;

	union
	{
		HDOC_HANDLE       blankDocument;
		WebcWriteStream** redirectTo;

		struct
		{
			WebcReadStream**  readFrom;
			WebcWriteStream** postTo;
		}
		urlStream;
	}
	out;

	HDOC_HANDLE hdoc;
};
typedef struct s_HTMLBrowserEvent HTMLBrowserEvent;

///
typedef HTMLEventStatus (*HTMLEventCallback) (
		HBROWSER_HANDLE hbrowser,
		HDOC_HANDLE hdoc,
		HELEMENT_HANDLE helem,
		HTMLEvent* event,
		char* param
	);



typedef WEBC_UINT32 WebcKeyMap[WG_NUM_KEYS*2+1]; // first element is how many keys

/*****************************************************************************/
// Macros
/*****************************************************************************/

// Passed as index argument to FindElement functions: matches the last element
#define HTML_INDEX_LAST 0xffffffffL

// For building WebC as a Windows DLL - WEBC_BUILD_DLL should be defined on the command line
//  at compile-time
#if (defined(WEBC_BUILD_DLL) && MSP3)
 #define DECL_WEBCDLL __declspec( dllexport )
#else
 #define DECL_WEBCDLL
#endif


#if (WEBC_SUPPORT_JSCRIPT)
#ifdef __cplusplus
extern "C" {
#endif
#include "jsapi.h"
#ifdef __cplusplus
}
#endif
#endif

#define WEBC_BROWSER_DISABLE_DRAW  1
#define WEBC_BROWSER_ALLOW_DRAW    0

/*****************************************************************************/
// Function Prototypes
/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

// API level routines
#if (WEBC_SUPPORT_POSTFILE)
DECL_WEBCDLL int			 webc_post_UploadFileASCII(HBROWSER_HANDLE postBrowser, char *SourceFileName,  char *pUrl,char* DestApplication, char *DestFileName);
DECL_WEBCDLL int			 webc_post_UploadDataBuffer(HBROWSER_HANDLE postBrowser, char *Post_Buffer, long Post_Buffer_Size, char *pUrl, char* DestApplication, char *DestFileName);
#endif

DECL_WEBCDLL int             webc_Init (void);
DECL_WEBCDLL int             webc_Exit (void);
DECL_WEBCDLL void            webc_FreeTempStore (void);
DECL_WEBCDLL void			 webc_clear_http_cache(void);

DECL_WEBCDLL HBROWSER_HANDLE webc_GetBrowserWithFocus(void);

DECL_WEBCDLL HBROWSER_HANDLE webc_CreateBrowser (
                                     struct HTMLBrowserConfig* config,
                                     WEBC_BOOL CreateWindow,
                                     char* name,
                                     char* home
                                 );

DECL_WEBCDLL HBROWSER_HANDLE webc_FindBrowserByNameAscii (
                                     char* name
                                 );
DECL_WEBCDLL HBROWSER_HANDLE webc_FindBrowserByDisplayManager(void *manager);

DECL_WEBCDLL HBROWSER_HANDLE webc_FindBrowserByName (
                                     WEBC_CHAR* name
                                 );
DECL_WEBCDLL HBROWSER_HANDLE webc_FindBrowserByWindow (
                                     void *WindowHandle
                                 );
DECL_WEBCDLL HBROWSER_HANDLE webc_FindBrowserByIndex (
                                     int index
                                 );
DECL_WEBCDLL void webc_SetBrowserWindow (
                                    HBROWSER_HANDLE Browser,
                                    void *WindowHandle
                                    );

DECL_WEBCDLL void webc_UnSetBrowserWindow (
                                    void *WindowHandle
                                    );

DECL_WEBCDLL void            webc_DestroyBrowser (
                                     HBROWSER_HANDLE wc,
                                     int status
                                 );

DECL_WEBCDLL int             webc_BrowserBack (
                                     HBROWSER_HANDLE wc
                                 );

DECL_WEBCDLL int             webc_BrowserForward (
                                     HBROWSER_HANDLE wc
                                 );

DECL_WEBCDLL int             webc_BrowserHome (
                                     HBROWSER_HANDLE wc
                                 );

DECL_WEBCDLL int             webc_BrowserLoadUrl (
                                     HBROWSER_HANDLE wc,
                                     char* url_str
                                 );

DECL_WEBCDLL int             webc_BrowserStop (
                                     HBROWSER_HANDLE wc
                                 );

DECL_WEBCDLL int             webc_BrowserRefresh (
                                     HBROWSER_HANDLE wc
                                 );

DECL_WEBCDLL int             webc_BrowserGetConfig (
                                     HBROWSER_HANDLE wc,
                                     struct HTMLBrowserConfig* config
                                 );

DECL_WEBCDLL void            webc_BrowserSetPrivateData (
                                     HBROWSER_HANDLE browser,
                                     void* data
                                 );

DECL_WEBCDLL void*           webc_BrowserGetPrivateData (
                                     HBROWSER_HANDLE browser
                                 );

DECL_WEBCDLL HDOC_HANDLE     webc_BrowserGetDocument (
                                     HBROWSER_HANDLE browser
                                 );

DECL_WEBCDLL int             webc_BrowserSetTagEventHandler (
                                     HBROWSER_HANDLE wc,
                                     HTMLTagType tagType,
                                     HTMLEventCallback cb
                                 );

DECL_WEBCDLL void            webc_BrowserSuspendDraw (
                                     HBROWSER_HANDLE hbrowser
                                 );

DECL_WEBCDLL void            webc_BrowserResumeDraw (
                                     HBROWSER_HANDLE hbrowser
                                 );

DECL_WEBCDLL int             webc_BrowserGetDrawState (
                                     HBROWSER_HANDLE hbrowser
                                 );

DECL_WEBCDLL void            webc_BrowserInvalidate (
                                     HBROWSER_HANDLE hbrowser
                                 );

DECL_WEBCDLL void            webc_BrowserDraw (
                                     HBROWSER_HANDLE hbrowser
                                 );

DECL_WEBCDLL HTMLEventStatus webc_BrowserDispatchEvent (
                                     HBROWSER_HANDLE hbrowser,
                                     HTMLEvent* event
                                 );

DECL_WEBCDLL void            webc_BrowserProcessLoadQueueAll (
                                     HBROWSER_HANDLE hbrowser
                                 );

DECL_WEBCDLL void            webc_BrowserProcessTimers (
                                     HBROWSER_HANDLE hbrowser
                                 );


DECL_WEBCDLL int            webc_BrowserQueueTimedEvent (
		                            HBROWSER_HANDLE hbrowser,
		                            HELEMENT_HANDLE element,
		                            HTMLEvent *pEvent,
		                            WEBC_INT32 msec
                                 );

DECL_WEBCDLL void webc_ElementCancelTimers(
							HBROWSER_HANDLE hbrowser,
							HELEMENT_HANDLE el);

DECL_WEBCDLL HELEMENT_HANDLE webc_DocFindElement (
                                     HDOC_HANDLE doc,
                                     char* id,
                                     char* name,
                                     HTMLElementType type,
                                     long index
                                 );

DECL_WEBCDLL void            webc_DocSetUrl (
                                     HDOC_HANDLE doc,
                                     char *url,
                                     HTML_BOOL refresh
                                 );


DECL_WEBCDLL void            webc_DocSetPrivateData (
                                     HDOC_HANDLE doc,
                                     void* data
                                 );

DECL_WEBCDLL void*           webc_DocGetPrivateData (
                                     HDOC_HANDLE doc
                                 );



DECL_WEBCDLL HBROWSER_HANDLE webc_DocGetBrowser (
                                     HDOC_HANDLE doc
                                 );


DECL_WEBCDLL void           webc_ElementGetXYPosition (
                                    HELEMENT_HANDLE element,
                                    long *current_xpos,
                                    long *current_ypos
                                 );

DECL_WEBCDLL                int webc_ElementMove (
                                    HELEMENT_HANDLE element,
                                    long deltax,
                                    long deltay
                                 );

DECL_WEBCDLL                int webc_ElementMoveAbs (
                                    HELEMENT_HANDLE element,
                                    long deltax,
                                    long deltay
                                 );

DECL_WEBCDLL void           webc_ElementClaimPointer (
                                    HELEMENT_HANDLE element
                                 );

DECL_WEBCDLL void           webc_ElementReleasePointer (
                                    HELEMENT_HANDLE element
                                 );

DECL_WEBCDLL void           webc_ElementSetXYPosition(
                                    HELEMENT_HANDLE hElement,
                                    long current_xpos,
                                    long current_ypos,
                                    WEBC_BOOL refresh
                                 );

DECL_WEBCDLL int            webc_ElementSetPrivateData (
                                     HELEMENT_HANDLE helem,
                                     void* data
                                 );

DECL_WEBCDLL void*           webc_ElementGetPrivateData (
                                     HELEMENT_HANDLE helem
                                 );

DECL_WEBCDLL HTMLElementType webc_ElementGetType (
                                     HELEMENT_HANDLE element
                                 );

DECL_WEBCDLL int webc_ElementSetTagEventHandler (
                            HELEMENT_HANDLE element,
                            HTMLEventCallback cb);

DECL_WEBCDLL HDOC_HANDLE     webc_ElementGetDocument (
                                     HELEMENT_HANDLE element
                                 );

DECL_WEBCDLL void            webc_ElementRefresh (
                                     HELEMENT_HANDLE element
                                 );

DECL_WEBCDLL int            webc_ElementFocus (
                                     HELEMENT_HANDLE element,
                                     HTML_BOOL refresh
                                 );


DECL_WEBCDLL HELEMENT_HANDLE webc_ElementGetLastChild (
                                     HELEMENT_HANDLE element
                                 );

DECL_WEBCDLL char*           webc_ElementGetNameASCII (
                                     HELEMENT_HANDLE element
                                 );
DECL_WEBCDLL char*           webc_ElementGetIdASCII (
                                     HELEMENT_HANDLE element
                                 );
DECL_WEBCDLL char*           webc_ElementGetValueASCII (
                                     HELEMENT_HANDLE element
                                 );
DECL_WEBCDLL WEBC_CHAR16*    webc_ElementGetValueUC (
                                     HELEMENT_HANDLE element
                                 );
DECL_WEBCDLL WEBC_BOOL       webc_ElementGetChecked (
                                     HELEMENT_HANDLE element
                                 );
DECL_WEBCDLL char*           webc_ElementGetSrcASCII (
                                     HELEMENT_HANDLE element
                                 );

DECL_WEBCDLL char*           webc_ElementGetBgImage (
                                     HELEMENT_HANDLE element
                                 );

DECL_WEBCDLL HDOC_HANDLE     webc_ElementGetFrameDocument (
                                     HELEMENT_HANDLE element
                                 );
DECL_WEBCDLL WEBC_CHAR*           webc_ElementGetInnerHTML (
                                     HELEMENT_HANDLE element
                                 );
DECL_WEBCDLL char * 		 webc_ElementGetInnerHtmlASCII (
								HELEMENT_HANDLE element
								);
DECL_WEBCDLL WEBC_CHAR*           webc_ElementGetOuterHTML (
                                     HELEMENT_HANDLE element
                                 );
DECL_WEBCDLL WEBC_CHAR*      webc_ElementGetAttribute (
                                     HELEMENT_HANDLE element,
                                     WEBC_CHAR* attrName
                                 );

DECL_WEBCDLL int            webc_ElementSetValueASCII (
                                     HELEMENT_HANDLE element,
                                     const char* value,
                                     HTML_BOOL refresh
                                 );
DECL_WEBCDLL int            webc_ElementSetValueUC (
                                     HELEMENT_HANDLE element,
                                     const WEBC_CHAR16* value,
                                     HTML_BOOL refresh
                                 );
DECL_WEBCDLL int            webc_ElementSetChecked (
                                     HELEMENT_HANDLE element,
                                     HTML_BOOL checked,
                                     HTML_BOOL refresh
                                 );
DECL_WEBCDLL int            webc_ElementSetSelected (
                                     HELEMENT_HANDLE element,
                                     int selected,
                                     HTML_BOOL refresh
                                 );
DECL_WEBCDLL int           webc_ElementSetNameASCII (
                                     HELEMENT_HANDLE element,
                                     char* name,
                                     HTML_BOOL refresh
                                 );
DECL_WEBCDLL int            webc_ElementSetSrcASCII (
                                     HELEMENT_HANDLE element,
                                     char *src,
                                     HTML_BOOL refresh
                                 );

DECL_WEBCDLL void            webc_ElementSetWidthASCII (
                                     HELEMENT_HANDLE element,
                                     char* width,
                                     HTML_BOOL refresh
                                 );
DECL_WEBCDLL void            webc_ElementSetHeightASCII (
                                     HELEMENT_HANDLE element,
                                     char* height,
                                     HTML_BOOL refresh
                                 );

DECL_WEBCDLL void           webc_ElementSetWidthINT(
                                    HELEMENT_HANDLE element,
                                    int value,
                                    HTML_BOOL refresh);

DECL_WEBCDLL void           webc_ElementSetHeightINT(
                                    HELEMENT_HANDLE element,
                                    int value,
                                    HTML_BOOL refresh);


DECL_WEBCDLL int            webc_ElementSetStyleASCII (
                                     HELEMENT_HANDLE element,
                                     char* style,
                                     HTML_BOOL refresh
                                 );
DECL_WEBCDLL WEBC_BOOL       webc_ElementSetAttributeASCII (
                                     HELEMENT_HANDLE element,
                                     char * _attrName,
									 char * _attrValue
                                 );

DECL_WEBCDLL HTML_BOOL       webc_ElementIdMatchesASCII (
                                     HELEMENT_HANDLE element,
                                     char* string
                                 );
DECL_WEBCDLL int           webc_ElementSetInnerHTMLASCII (
                                     HELEMENT_HANDLE element,
                                     char* innerHTML
                                 );


DECL_WEBCDLL int            webc_ElementSetOuterHTMLASCII (
                                     HELEMENT_HANDLE element,
                                     char* outerHTML
                                 );

DECL_WEBCDLL int             webc_RegisterEventCallback (
                                     HTMLEventCallback cb,
                                     char* name
                                 );

DECL_WEBCDLL HTML_BOOL      webc_ElementIdMatches (
                                HELEMENT_HANDLE element,
                                char *string);

DECL_WEBCDLL COOKIE_HANDLE   webc_CreateCookie (
                                     char* name,
                                     char* value,
                                     char* host,
                                     char* path
                                 );
DECL_WEBCDLL int             webc_DestroyCookie (
                                     COOKIE_HANDLE cookie
                                 );
DECL_WEBCDLL COOKIE_HANDLE   webc_FindCookie (
                                     const char* name,
                                     char* value,
                                     char *host,
                                     char *path,
                                     int num
                                 );

DECL_WEBCDLL const char     *webc_CookieGetName (
                                     COOKIE_HANDLE cookie
                                 );
DECL_WEBCDLL const char     *webc_CookieGetValue (
                                     COOKIE_HANDLE cookie
                                 );
DECL_WEBCDLL const char     *webc_CookieGetHost (
                                     COOKIE_HANDLE cookie
                                 );
DECL_WEBCDLL const char     *webc_CookieGetPath (
                                     COOKIE_HANDLE cookie
                                 );
DECL_WEBCDLL void            webc_CookieSetName (
                                     COOKIE_HANDLE cookie,
                                     const char* name
                                 );
DECL_WEBCDLL void            webc_CookieSetValue (
                                     COOKIE_HANDLE cookie,
                                     const char* value
                                  );
DECL_WEBCDLL void            webc_CookieSetHost (
                                     COOKIE_HANDLE cookie,
                                     const char* host
                                  );
DECL_WEBCDLL void            webc_CookieSetPath (
                                     COOKIE_HANDLE cookie,
                                     const char* path
                                  );

//DECL_WEBCDLL int             webc_RegisterMetaCallback (
//                                     const char* metaName,
//                                     HTMLMetaCallbackFn callback
//                                 );

DECL_WEBCDLL int webc_set_rotation_angle(HBROWSER_HANDLE hBrowser,int iRotation);
#if (WEBC_SUPPORT_TSD_TOKEN_EXCHANGE)

typedef enum e_Webc_tsd_Token_Type
{
WEBC_TOKEN_UNKNOWN  =	-1,
WEBC_TOKEN_MACADDR	=	0,
WEBC_TOKEN_SERILANO,
WEBC_TOKEN_ADDITIONAL_HEADER,
WEBC_TOKEN_ROM_ADDR,
WEBC_NUM_TOKENS
}
Webc_tsd_Token_Type;

DECL_WEBCDLL int			set_webc_tsd_token(int token_name, void * token_value, int cLen);   // int cLen contain a string length (or array length) if it is a string, 0 otherwise

DECL_WEBCDLL void*			get_webc_tsd_token(int token_name, int *len);

#endif  //if WEBC_SUPPORT_TSD_TOKEN_EXCHANGE

#if(!defined(WEBC_SUPPORT_ERROR_PAGE_REDIRECT)) // Normally we define this
#define WEBC_SUPPORT_ERROR_PAGE_REDIRECT 1
#endif
#if(WEBC_SUPPORT_ERROR_PAGE_REDIRECT)
int webc_setErrorRedirect(int url_error, const char * url);
#endif


DECL_WEBCDLL HTMLEventStatus webc_TriggerEvent (
                                     HELEMENT_HANDLE element,
                                     HTMLEvent* event
                                 );

#if (WEBC_SUPPORT_JSCRIPT)

DECL_WEBCDLL HSCRIPT_HANDLE webc_JScriptExecuteSource(
                            HDOC_HANDLE doc,
                            const char *_source
                            );

DECL_WEBCDLL void webc_JScriptDestroyScript(
                            HSCRIPT_HANDLE  hscript
                            );

DECL_WEBCDLL HSCRIPT_HANDLE webc_JScriptGetDoc (
                                     struct JSContext* cx
                                 );

DECL_WEBCDLL void            webc_JScriptDefineFunction (
                                     HDOC_HANDLE doc,
                                     const char* name,
                                     JSNative func,
                                     int argc
                                 );

DECL_WEBCDLL HBROWSER_HANDLE webc_JScriptGetDoc (
                                     struct JSContext* cx
                                 );

DECL_WEBCDLL HDOC_HANDLE     webc_JSValToDoc (
                                     jsval val
                                 );

DECL_WEBCDLL struct JSContext *webc_JScriptGetJSContext(HDOC_HANDLE doc);

DECL_WEBCDLL HELEMENT_HANDLE webc_JSValToElement (
                                     jsval val
                                 );

DECL_WEBCDLL jsval           webc_DocToJSVal (
                                     HDOC_HANDLE hdoc
                                 );

DECL_WEBCDLL jsval           webc_ElementToJSVal (
                                     HELEMENT_HANDLE helem
                                 );

#endif // WEBC_SUPPORT_JSCRIPT

DECL_WEBCDLL void            webc_FreeASCIIString (
                                     char* str
                                 );

#ifdef RTP_TRACK_LOCAL_MEMORY
#define webc_MallocASCIIString(S) _webc_MallocASCIIString(S,__FILE__, __LINE__)
DECL_WEBCDLL char*           _webc_MallocASCIIString (
                                     const WEBC_CHAR* str,
	                                 const char * file,
									 const long line
                                 );
#else
DECL_WEBCDLL char*           webc_MallocASCIIString (
                                     const WEBC_CHAR* str
                                 );
#endif
#if (WEBC_SUPPORT_UNICODE)
DECL_WEBCDLL void            webc_FreeUCString (
                                     WEBC_CHAR16* str
                                 );
DECL_WEBCDLL WEBC_CHAR16*    webc_MallocUCString (
                                     const WEBC_CHAR* str
                                 );
#endif // WEBC_SUPPORT_UNICODE

DECL_WEBCDLL WEBC_CHAR *     webc_MallocStringFromInt (
                                     WEBC_INT32 i
                                 );


DECL_WEBCDLL int             webc_HttpSetAuthorization (
                                     HTTPAuthDomain* domain,
                                     const char* userName,
                                     const char* password,
                                     long ttlSec
                                 );
DECL_WEBCDLL void            webc_HttpRemoveAuthorization (
                                     HTTPAuthDomain* domain
                                 );
DECL_WEBCDLL void            webc_HttpRemoveAllAuthorizations (
                                     void
                                 );

DECL_WEBCDLL void            webc_SetOnlineStatus (
                                     WEBC_BOOL isOnline
                                 );
DECL_WEBCDLL WEBC_BOOL       webc_GetOnlineStatus (
                                     void
                                 );
DECL_WEBCDLL void            webc_DocSaveForOffline (
                                     HDOC_HANDLE hdoc
                                 );
DECL_WEBCDLL WEBC_BOOL       webc_UrlAvailableOffline (
                                     const char* url
                                 );

DECL_WEBCDLL void            webc_CreateKeyMap (
                                     WebcKeyMap* map,
                                     WEBC_UINT32* keyData,
                                     WEBC_SIZE numKeys
                                 );
DECL_WEBCDLL WebKey          webc_TranslateKey (
                                     WebcKeyMap* map,
                                     WEBC_UINT32 rawKey
                                 );
DECL_WEBCDLL void            webc_DestroyKeyMap (
                                     WebcKeyMap* map
                                 );

#if (WEBC_DEBUG_CACHING && WEBC_SUPPORT_CACHE)
void wcache_stats_display(char *prompt);
#define WEBC_DEBUG_CACHE_STATISTICS(P) wcache_stats_display(P);
#else
#define WEBC_DEBUG_CACHE_STATISTICS(P)
#endif




#ifdef __cplusplus
}
#endif

#if (WEBC_SUPPORT_UNICODE)

#define webc_ElementSetValue webc_ElementSetValueUC
#define webc_ElementGetValue webc_ElementGetValueUC
#define webc_MallocString    webc_MallocUCString
#define webc_FreeString      webc_FreeUCString

#else

#define webc_ElementSetValue webc_ElementSetValueASCII
#define webc_ElementGetValue webc_ElementGetValueASCII
#define webc_MallocString    webc_MallocASCIIString
#define webc_FreeString      webc_FreeASCIIString

#endif // WEBC_SUPPORT_UNICODE

#endif //__HTMLSDK_INCLUDED__
