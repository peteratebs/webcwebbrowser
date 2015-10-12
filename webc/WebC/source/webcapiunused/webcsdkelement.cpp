//
// WEBCSDKEXIT.CPP -
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
// Header files
/*****************************************************************************/

#include "webcjhconfig.h"
#include "htmlsdk.h"
#include "httpc.h"
#if (WEBC_SUPPORT_JSCRIPT)
 #include "wjsdoc.h"
 #include "wjsbrowser.h"
 #include "wjscript.hpp"
#endif
#if (WEBC_SUPPORT_COOKIES)
 #include "cookie.h"
#endif
#include "hchkbox.hpp"
#include "helement.hpp"
#include "hframe.hpp"
#include "himage.hpp"
#include "hobject.hpp"
#include "hinput.hpp"
#include "hhidden.hpp"
#include "hedit.hpp"
#include "hpasswd.hpp"
#include "hselect.hpp"
#include "htmldoc.hpp"
#include "htmlfind.hpp"
#include "format.hpp"
#include "webctmo.h"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "prothttp.h"
#include "wgkeys.hpp"
#include "jhutils.h"
#include "tuenum.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "rtpprint.h"
#include "webcdefault.h"
#ifdef WEBC_DEBUG
#include "webcLog.h"
#endif
#if (WEBC_DEBUG_JSCRIPT)
#include "wjsdebug.h"
#endif


/*****************************************************************************/
// Local types
/*****************************************************************************/
#include "webcsdk.hpp"
#include "wcapi.h"

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// External Declarations
/*****************************************************************************/

/*****************************************************************************/
// Macros
/*****************************************************************************/

/*****************************************************************************/
// HTML C API Functions
/*****************************************************************************/



/*****************************************************************************/
// HTML Element API Functions
/*****************************************************************************/
/*****************************************************************************/
/*X Deprecated from published API
 @memo Get an HTML element attribute

 @doc

 @see webc_ElementSetAttribute
 */
/*****************************************************************************/

/* not tested and what about refresh */

WEBC_CHAR* webc_ElementGetAttribute (
		HELEMENT_HANDLE element,
		WEBC_CHAR* attrName
	)
{
	WEBC_CHAR* attrValueCopy = 0;
	HTMLAttributeType attrType = HTML_ParseAttributeType(attrName, 0);
	if (attrType > HTML_UNKNOWN_ATTRIB)
	{
		switch (attrType)
		{
			case HTML_ATTRIB_ABBR:
			case HTML_ATTRIB_ACCEPT_CHARSET:
			case HTML_ATTRIB_ACCEPT:
			case HTML_ATTRIB_ACCESSKEY:
			case HTML_ATTRIB_ACTION:
			case HTML_ATTRIB_ALIGN:
			case HTML_ATTRIB_ALINK:
			case HTML_ATTRIB_ALT:
			case HTML_ATTRIB_ARCHIVE:
			case HTML_ATTRIB_AXIS:
			case HTML_ATTRIB_BACKGROUND:
			case HTML_ATTRIB_BGCOLOR:
			case HTML_ATTRIB_BORDER:
			case HTML_ATTRIB_CELLPADDING:
			case HTML_ATTRIB_CELLSPACING:
			case HTML_ATTRIB_CHAR:
			case HTML_ATTRIB_CHAROFF:
			case HTML_ATTRIB_CHARSET:
			case HTML_ATTRIB_CHECKED:
			case HTML_ATTRIB_CITE:
			case HTML_ATTRIB_CLASS:
			case HTML_ATTRIB_CLASSID:
			case HTML_ATTRIB_CLEAR:
			case HTML_ATTRIB_CODE:
			case HTML_ATTRIB_CODEBASE:
			case HTML_ATTRIB_CODETYPE:
			case HTML_ATTRIB_COLOR:
			case HTML_ATTRIB_COLS:
			case HTML_ATTRIB_COLSPAN:
			case HTML_ATTRIB_COMPACT:
			case HTML_ATTRIB_CONTENT:
			case HTML_ATTRIB_COORDS:
			case HTML_ATTRIB_DATA:
			case HTML_ATTRIB_DATETIME:
			case HTML_ATTRIB_DECLARE:
			case HTML_ATTRIB_DEFER:
			case HTML_ATTRIB_DIR:
			case HTML_ATTRIB_DISABLED:
			case HTML_ATTRIB_ENCTYPE:
			case HTML_ATTRIB_EVENTHANDLER:
			case HTML_ATTRIB_FACE:
			case HTML_ATTRIB_FOR:
			case HTML_ATTRIB_FRAME:
			case HTML_ATTRIB_FRAMEBORDER:
			case HTML_ATTRIB_HEADERS:
			case HTML_ATTRIB_HEIGHT:
			case HTML_ATTRIB_HREF:
			case HTML_ATTRIB_HREFLANG:
			case HTML_ATTRIB_HSPACE:
			case HTML_ATTRIB_HTTP_EQUIV:
			case HTML_ATTRIB_ID:
			case HTML_ATTRIB_ISMAP:
			case HTML_ATTRIB_LABEL:
			case HTML_ATTRIB_LANG:
			case HTML_ATTRIB_LANGUAGE:
			case HTML_ATTRIB_LINK:
			case HTML_ATTRIB_LONGDESC:
			case HTML_ATTRIB_MARGINHEIGHT:
			case HTML_ATTRIB_MARGINWIDTH:
				break;

			case HTML_ATTRIB_MAXLENGTH:
				if (HELEMENT_OBJ(element)->MaxLength() >= 0)
				{
					attrValueCopy = webc_MallocStringFromInt(HELEMENT_OBJ(element)->MaxLength());
				}
				else
				{
					attrValueCopy = webc_MallocString((WEBC_CHAR*) _WEBC_STR(""));
				}
				break;

			case HTML_ATTRIB_MEDIA:
			case HTML_ATTRIB_METHOD:
			case HTML_ATTRIB_MULTIPLE:
			case HTML_ATTRIB_NAME:
			case HTML_ATTRIB_NOBACKGROUND:
			case HTML_ATTRIB_NOHREF:
			case HTML_ATTRIB_NORESIZE:
			case HTML_ATTRIB_NOSHADE:
			case HTML_ATTRIB_NOTAB:
			case HTML_ATTRIB_NOWRAP:
			case HTML_ATTRIB_OBJECT:
			case HTML_ATTRIB_ONBLUR:
			case HTML_ATTRIB_ONCHANGE:
			case HTML_ATTRIB_ONCLICK:
			case HTML_ATTRIB_ONDBLCLICK:
			case HTML_ATTRIB_ONFOCUS:
			case HTML_ATTRIB_ONKEYDOWN:
			case HTML_ATTRIB_ONKEYPRESS:
			case HTML_ATTRIB_ONKEYUP:
			case HTML_ATTRIB_ONLOAD:
			case HTML_ATTRIB_ONLOSECAPTURE:
			case HTML_ATTRIB_ONMOUSEDOWN:
			case HTML_ATTRIB_ONMOUSEMOVE:
			case HTML_ATTRIB_ONMOUSEOUT:
			case HTML_ATTRIB_ONMOUSEOVER:
			case HTML_ATTRIB_ONMOUSEUP:
			case HTML_ATTRIB_ONRESET:
			case HTML_ATTRIB_ONSELECT:
			case HTML_ATTRIB_ONSUBMIT:
			case HTML_ATTRIB_ONUNLOAD:
			case HTML_ATTRIB_PROFILE:
			case HTML_ATTRIB_PROMPT:
			case HTML_ATTRIB_QUARANTINE:
				break;

			case HTML_ATTRIB_READONLY:
				if (HELEMENT_OBJ(element)->ReadOnly() == WEBC_TRUE)
				{
					attrValueCopy = webc_MallocString((WEBC_CHAR*)_WEBC_STR("true"));
				}
				else
				{
					attrValueCopy = webc_MallocString((WEBC_CHAR*)_WEBC_STR("false"));
				}
				break;

			case HTML_ATTRIB_REL:
			case HTML_ATTRIB_REV:
			case HTML_ATTRIB_ROWS:
			case HTML_ATTRIB_ROWSPAN:
			case HTML_ATTRIB_RULES:
			case HTML_ATTRIB_SCHEME:
			case HTML_ATTRIB_SCOPE:
			case HTML_ATTRIB_SCROLLING:
			case HTML_ATTRIB_SELECTED:
			case HTML_ATTRIB_SHAPE:
			case HTML_ATTRIB_SIZE:
			case HTML_ATTRIB_SPAN:
			case HTML_ATTRIB_SRC:
			case HTML_ATTRIB_STANDBY:
			case HTML_ATTRIB_START:
			case HTML_ATTRIB_STYLE:
			case HTML_ATTRIB_SUMMARY:
			case HTML_ATTRIB_TABINDEX:
			case HTML_ATTRIB_TARGET:
			case HTML_ATTRIB_TEXT:
			case HTML_ATTRIB_TITLE:
			case HTML_ATTRIB_TYPE:
			case HTML_ATTRIB_USEMAP:
			case HTML_ATTRIB_VALIGN:
			case HTML_ATTRIB_VALUE:
			case HTML_ATTRIB_VALUETYPE:
			case HTML_ATTRIB_VERSION:
			case HTML_ATTRIB_VLINK:
			case HTML_ATTRIB_VSPACE:
			case HTML_ATTRIB_WIDTH:
			case HTML_ATTRIB_WRAP:
				break;
		}
	}

	return attrValueCopy;
}

#ifdef HEREHERE


/*****************************************************************************/
// webc_ElementGetValueUC
/*X Deprecated from published API
 @memo          Get the value of an input widget
 @doc           If the given element handle is an HTML_EDIT_STR_ELEMENT (text field), HTML_EDITBOX_ELEMENT (text box),
                or HTML_SELECT_ELEMENT (combo box), then this function will
                return the currently entered value of the widget.  For element types that have
                no input value (everything that is not a widget), this function will return WEBC_NULL.
                To retrieve the status of boolean type input widgets (such as radio buttons and
                checkboxes), use \Ref{webc_ElementGetChecked}.
 @param element The element whose value to get
 @return        The element's value or WEBC_NULL if it doesn't have one.
 @see           HELEMENT_HANDLE, VALUE, webc_ElementSetValue, webc_ElementGetChecked
 */
/*****************************************************************************/

WEBC_CHAR16* webc_ElementGetValueUC(HELEMENT_HANDLE element)
{
  if (element && HELEMENT_OBJ(element)->Value())
  {
  #if (WEBC_SUPPORT_UNICODE)
   	return webc_MallocUCString(HELEMENT_OBJ(element)->Value());
  #else
	return WEBC_NULL;
  #endif
  }
  else
  	return WEBC_NULL;
}



/*****************************************************************************/
// webc_ElementGetFrameDocument
/*X Deprecated from published API
 @memo          Get a frame's child document
 @doc           Gets a frame's child document.
 @param element The frame element handle
 @return        The handle of the HTML Document or WEBC_NULL if this element is not a frame.
 @see           HELEMENT_HANDLE, HDOC_HANDLE, webc_BrowserGetDocument
 */
/*****************************************************************************/

HDOC_HANDLE webc_ElementGetFrameDocument(HELEMENT_HANDLE element)
{
	return (HDOC_HANDLE) wcElGetFrameDocument((wcEL)element);
}

/*****************************************************************************/
// webc_ElementSetValueUC
/*X Deprecated from published API
 @memo          Set the current value for an input widget
 @doc           For text entry widgets (HTML_EDIT_STR_ELEMENT, HTML_EDITBOX_ELEMENT),
                this function sets the current text in the widget.  For boolean-type
                widgets (HTML_CHECKBOX_ELEMENT, HTML_RADIO_BUTTON_ELEMENT), this
                function will set the value string that will be returned by
                \Ref{webc_ElementGetChecked} if the element's status is 'checked'.
                To set the currently selected item in combo boxes (HTML_SELECT_ELEMENT),
                use \Ref{webc_ElementSetSelected}.
 @param element The element whose value to get
 @param value   The string to set the element's value to
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, VALUE, webc_ElementGetValue, webc_ElementGetChecked,
                webc_ElementSetChecked, webc_ElementSetSelected, webc_ElementRefresh
 */
/*****************************************************************************/
int webc_ElementSetValueUC (
		HELEMENT_HANDLE element,
		const WEBC_CHAR16* value,
		HTML_BOOL refresh
	)
{
	if (!element)
		return -1;
  #if (WEBC_SUPPORT_UNICODE)
    HELEMENT_OBJ(element)->SetValue(value);
    if (refresh)
    {
		HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
		HELEMENT_OBJ(element)->Refresh();
    }
  #endif
    return 0;
}



/*****************************************************************************/
// webc_ElementSetSrc
/*X Deprecated from published API
 @memo          Set the source url for an image or frame element
 @doc           Sets the src attribute for the specified element.  Only affects
                HTML_FRAME_ELEMENT and HTML_IMAGE_ELEMENT.  Doesn't load or redraw
                (unless 'refresh' is set to WEBC_TRUE) until \Ref{webc_ElementRefresh} is called.
 @param element The element to set
 @param src     The new src
 @param refresh If WEBC_TRUE, then load the new url and redraw this element
 @see           HELEMENT_HANDLE, SRC, webc_ElementGetSrc, webc_ElementRefresh
 */
/*****************************************************************************/

int webc_ElementSetSrcASCII(HELEMENT_HANDLE element, char *_src, HTML_BOOL refresh)
{
	return wcElSetSrc((wcEL) element, _src, (wcBOOL) refresh);
}



/*****************************************************************************/
// webc_ElementSetPrivateData
/* X Deprecated from published API
 @memo
 @doc
 @param element The element to set
 @see           HELEMENT_HANDLE, webc_ElementGetPrivateData
 */
/*****************************************************************************/

int webc_ElementSetPrivateData (
		HELEMENT_HANDLE helem,
		void* data
	)
{
	if (!helem)
		return -1;
	HELEMENT_OBJ(helem)->SetPrivateData(data);
	return 0;
}

/*****************************************************************************/
// webc_ElementGetPrivateData
/*X Deprecated from published API
 @memo
 @doc
 @param element The element to set
 @param
 @see           HELEMENT_HANDLE, webc_ElementSetPrivateData
 */
/*****************************************************************************/

void* webc_ElementGetPrivateData (
		HELEMENT_HANDLE helem
	)
{
	return HELEMENT_OBJ(helem)->GetPrivateData();
}



#endif // HEREHERE