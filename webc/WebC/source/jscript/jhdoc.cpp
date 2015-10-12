/*
|  JHDOC.CPP -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|  Module description:
|   [tbd]
*/

#include "webcjhconfig.h"
#include "jhdoc.h"
#if (WEBC_SUPPORT_JSCRIPT)
#ifdef __cplusplus
extern "C" {
#endif
#include "jsapi.h"
#ifdef __cplusplus
}
#endif
#include "jhutils.h"
#include "webload.hpp"
#include "webc.h"
#include "cookie.h"
#include "htmldoc.hpp"
#include "htmlfind.hpp"
#include "hform.hpp"
#include "htmlbrow.hpp"
#include "wjscript.hpp"
#include "webcstr.h"
#include "objref.cpp"
#include "webcmem.h"
#include "jhelem.h"
#include "hstring.hpp"
#include "wjsdebug.h"
#include "webcLog.h"

//This file is our javascript implementation of the HTMLDocument class defined
//by the DOM.  It uses the SpiderMonkey JSApi defined at www.mozilla.org to
//allow us to create JS classes by specifying methods and attributes for it.

/**
 * @memo set the unique ids for the properties of the document class
 *
 * @doc the jsapi uses these values to determine which attribute is being used in
 * the get/set property methods
 */
enum jhtml_doc_tinyid
{
	HTMLDOCUMENT_TITLE=            -1,
  	HTMLDOCUMENT_REFERRER=         -2,
  	HTMLDOCUMENT_DOMAIN=           -3,
  	HTMLDOCUMENT_URL=              -4,
	HTMLDOCUMENT_BODY=             -5,
	HTMLDOCUMENT_COOKIE=           -6,
	HTMLDOCUMENT_PARENTWINDOW=     -7,
	HTMLDOCUMENT_PROTOCOL=         -8,
	HTMLDOCUMENT_UNIQUEID=         -9,
	HTMLDOCUMENT_DOCUMENTELEMENT= -10,
	HTMLDOCUMENT_ALINK=           -11,
	HTMLDOCUMENT_BACKGROUND=      -12,
	HTMLDOCUMENT_BGCOLOR=         -13,
	HTMLDOCUMENT_LINK=            -14,
	HTMLDOCUMENT_TEXT=            -15,
	HTMLDOCUMENT_VLINK=           -16,
	HTMLDOCUMENT_CLIENTWIDTH=     -17,
	HTMLDOCUMENT_CLIENTHEIGHT=    -18,
	HTMLDOCUMENT_LINKCOLOR=       -19,
	HTMLDOCUMENT_VLINKCOLOR=      -20,
	HTMLDOCUMENT_ALINKCOLOR=      -21,
	HTMLDOCUMENT_ONCLICK=         -22,
	HTMLDOCUMENT_ONDBLCLICK=      -23,
	HTMLDOCUMENT_ONKEYDOWN=       -24,
	HTMLDOCUMENT_ONKEYPRESS=      -25,
	HTMLDOCUMENT_ONKEYUP=         -26,
	HTMLDOCUMENT_ONMOUSEDOWN=     -27,
	HTMLDOCUMENT_ONMOUSEMOVE=     -28,
	HTMLDOCUMENT_ONMOUSEOUT=      -29,
	HTMLDOCUMENT_ONMOUSEOVER=     -30,
	HTMLDOCUMENT_ONMOUSEUP=       -31,
	HTMLDOCUMENT_ONFOCUS=         -32,
	HTMLDOCUMENT_ONUNFOCUS=       -33,
	HTMLDOCUMENT_ONLOAD=          -34,
	HTMLDOCUMENT_ONUNLOAD=        -35,
	HTMLDOCUMENT_ONSUBMIT=        -36,
	HTMLDOCUMENT_ONCHANGE=        -37,
	HTMLDOCUMENT_FGCOLOR=         -38,
	HTMLDOCUMENT_LOCATION=        -39,
	HTMLDOCUMENT_URL_LC=          -40,
	HTMLDOCUMENT_WINDOW=          -41,
	HTMLDOCUMENT_NODETYPE=        -42,
	HTMLDOCUMENT_NODENAME=        -43
};

/**
 * @memo This struct associates strings with unique ids, which represent attributes of
 * this class.
 *
 * @doc The fields of this struct are the names of all the attributes of HTMLDocument
 * WARNING: the order if the array below must be kept in lock-step of the enumeration
 * above (jhtml_doc_tinyid)
 *
 */
JSPropertySpec jhtml_doc_props[] = {
	{"title",           HTMLDOCUMENT_TITLE,           JSPROP_ENUMERATE},
  	{"referrer",        HTMLDOCUMENT_REFERRER,        JSPROP_ENUMERATE | JSPROP_READONLY},
 	{"domain",          HTMLDOCUMENT_DOMAIN,          JSPROP_ENUMERATE},
  	{"URL",             HTMLDOCUMENT_URL,             JSPROP_ENUMERATE | JSPROP_READONLY},
  	{"body",            HTMLDOCUMENT_BODY,            JSPROP_ENUMERATE | JSPROP_READONLY},
  	{"cookie",          HTMLDOCUMENT_COOKIE,          JSPROP_ENUMERATE},
	{"parentWindow",    HTMLDOCUMENT_PARENTWINDOW,    JSPROP_ENUMERATE | JSPROP_READONLY},
	{"protocol",        HTMLDOCUMENT_PROTOCOL,        JSPROP_ENUMERATE | JSPROP_READONLY},
	{"uniqueId",        HTMLDOCUMENT_UNIQUEID,        JSPROP_ENUMERATE | JSPROP_READONLY},
	{"documentElement", HTMLDOCUMENT_DOCUMENTELEMENT, JSPROP_ENUMERATE | JSPROP_READONLY},
	{"aLink",           HTMLDOCUMENT_ALINK,           JSPROP_ENUMERATE},
	{"background",      HTMLDOCUMENT_BACKGROUND,      JSPROP_ENUMERATE},
	{"bgColor",         HTMLDOCUMENT_BGCOLOR,         JSPROP_ENUMERATE},
	{"link",            HTMLDOCUMENT_LINK,            JSPROP_ENUMERATE},
	{"text",            HTMLDOCUMENT_TEXT,            JSPROP_ENUMERATE},
	{"vLink",           HTMLDOCUMENT_VLINK,           JSPROP_ENUMERATE},
	{"clientWidth",     HTMLDOCUMENT_CLIENTWIDTH,     JSPROP_ENUMERATE},
	{"clientHeight",    HTMLDOCUMENT_CLIENTHEIGHT,    JSPROP_ENUMERATE},
	{"linkColor",       HTMLDOCUMENT_LINKCOLOR,       JSPROP_ENUMERATE},
	{"vlinkColor",      HTMLDOCUMENT_VLINKCOLOR,      JSPROP_ENUMERATE},
	{"alinkColor",      HTMLDOCUMENT_ALINKCOLOR,      JSPROP_ENUMERATE},
	{"onclick",         HTMLDOCUMENT_ONCLICK,         JSPROP_ENUMERATE},
	{"ondblclick",      HTMLDOCUMENT_ONDBLCLICK,      JSPROP_ENUMERATE},
	{"onkeydown",       HTMLDOCUMENT_ONKEYDOWN,       JSPROP_ENUMERATE},
	{"onkeypress",      HTMLDOCUMENT_ONKEYPRESS,      JSPROP_ENUMERATE},
	{"onkeyup",         HTMLDOCUMENT_ONKEYUP,	      JSPROP_ENUMERATE},
	{"onmousedown",     HTMLDOCUMENT_ONMOUSEDOWN,     JSPROP_ENUMERATE},
	{"onmousemove",     HTMLDOCUMENT_ONMOUSEMOVE,     JSPROP_ENUMERATE},
	{"onmouseout",      HTMLDOCUMENT_ONMOUSEOUT,      JSPROP_ENUMERATE},
	{"onmouseover",     HTMLDOCUMENT_ONMOUSEOVER,     JSPROP_ENUMERATE},
	{"onmouseup",       HTMLDOCUMENT_ONMOUSEUP,       JSPROP_ENUMERATE},
	{"onfocus",         HTMLDOCUMENT_ONFOCUS,	      JSPROP_ENUMERATE},
	{"onunfocus",       HTMLDOCUMENT_ONUNFOCUS,       JSPROP_ENUMERATE},
	{"onload",          HTMLDOCUMENT_ONLOAD,	      JSPROP_ENUMERATE},
	{"onunload",        HTMLDOCUMENT_ONUNLOAD,	      JSPROP_ENUMERATE},
	{"onsubmit",        HTMLDOCUMENT_ONSUBMIT,	      JSPROP_ENUMERATE},
	{"onchange",        HTMLDOCUMENT_ONCHANGE,        JSPROP_ENUMERATE},
	{"fgColor",         HTMLDOCUMENT_FGCOLOR,         JSPROP_ENUMERATE},
	{"location",        HTMLDOCUMENT_LOCATION,        JSPROP_ENUMERATE},
  	{"url",             HTMLDOCUMENT_URL_LC,          JSPROP_ENUMERATE | JSPROP_READONLY},
	{"window",          HTMLDOCUMENT_PARENTWINDOW,    JSPROP_ENUMERATE | JSPROP_READONLY},
	{"nodeType",        HTMLDOCUMENT_NODETYPE,        JSPROP_ENUMERATE | JSPROP_READONLY},
	{"nodeName",        HTMLDOCUMENT_NODENAME,        JSPROP_ENUMERATE | JSPROP_READONLY},
	{0}
};

JSBool jhtml_doc_resolve(JSContext *cx, JSObject *obj, jsval id)
{
	HTMLDocument *pDoc  = (HTMLDocument *) jhutil_GetPrivate(cx, obj);
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;

	if (!pDoc || !jsmgr)
		return JS_TRUE;

	if (JSVAL_IS_STRING(id))
	{
		WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(id));

		if (!webc_c_strcmp(name, "images"))
		{
			LOG_HTMLDOM("jhtml_doc_resolve jhtml_collection_New(images)\n");

			jhtml_collection_New(jsmgr, obj, "images", pDoc->Body(), DOCUMENT_IMAGES);
		}
		else if (!webc_c_strcmp(name, "forms"))
		{
			LOG_HTMLDOM("jhtml_doc_resolve jhtml_collection_New(forms)\n");

			jhtml_collection_New(jsmgr, obj, "forms", pDoc->Body(), DOCUMENT_FORMS);
		}
		else if (!webc_c_strcmp(name, "anchors"))
		{
			LOG_HTMLDOM("jhtml_doc_resolve jhtml_collection_New(anchors)\n");

			jhtml_collection_New(jsmgr, obj, "anchors", pDoc->Body(), DOCUMENT_ANCHORS);
		}
		else if (!webc_c_strcmp(name, "links"))
		{
			LOG_HTMLDOM("jhtml_doc_resolve jhtml_collection_New(links)\n");

			jhtml_collection_New(jsmgr, obj, "links", pDoc->Body(), DOCUMENT_ANCHORS);
		}
		else if (!webc_c_strcmp(name, "ids"))
		{
			LOG_HTMLDOM("jhtml_doc_resolve jhtml_collection_New(ids)\n");

			jhtml_collection_New(jsmgr, obj, "ids", pDoc->Body(), DOCUMENT_IDS);
		}
		else if (!webc_c_strcmp(name, "all"))
		{
			LOG_HTMLDOM("jhtml_doc_resolve jhtml_collection_New(all)\n");

			jhtml_collection_New(jsmgr, obj, "all", pDoc->Body(), DOCUMENT_ALL);
		}
		else if (!webc_c_strcmp(name, "styleSheets"))
		{
			LOG_HTMLDOM("jhtml_doc_resolve jhtml_collection_New(styleSheets)\n");

			jhtml_collection_New(jsmgr, obj, "styleSheets", 0, DOCUMENT_STYLESHEETS);
		}
		else if (!webc_c_strcmp(name, "childNodes"))
		{
			LOG_HTMLDOM("jhtml_doc_resolve jhtml_collection_New(childNodes)\n");

			jhtml_collection_New(jsmgr, obj, "childNodes", pDoc->Body(), ELEMENT_NODES);
		}
		else
		{
			// try to find an element whose name is <name> in this document
			HTMLElementByNameFinder f(name);
			HTMLElement *pElem = pDoc->FindElement(&f, 0 /* don't search subframes */);
			if (pElem)
			{
				LOG_HTMLDOM("jhtml_doc_resolve  %S\n", name);

				// found it; define a property for this object
			  #if (WEBC_SUPPORT_UNICODE)
				JS_DefineUCProperty(cx, obj, name, webc_strlen(name),
				        OBJECT_TO_JSVAL(pElem->CreateJSObject()),
				        0 /*getter*/, 0 /*setter*/, JSPROP_READONLY
				    );
			  #else
				JS_DefineProperty(cx, obj, name, OBJECT_TO_JSVAL(pElem->CreateJSObject()),
				        0 /*getter*/, 0 /*setter*/, JSPROP_READONLY
				    );
			  #endif
			}
		}
	}

    return JS_TRUE;
}

/**
 * @memo This method is called by the jsapi when the document has a property that is being accessed.
 *
 * @doc This method is called by the jsapi when the document has a property that is being accessed.
 *      It always returns WEBC_TRUE;
 *
 * @param cx  - The JSContext that contains all the js information for this webpage
 * @param obj - The JSObject that is an instance of the JS HTMLDocument class
 * @param id  - The unique id of the attribute (jhtml_doc_tinyid)
 * @param vp  - The value pointer that should be set, representing the value of the attribute
 */
static JSBool jhtml_doc_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	//The GetPrivate function gets a pointer associated with this JSObject
	//that allows data to be associated with a JSObject.  This private data was
	//set by the jhutil_SetPrivate function in JscriptManager
	HTMLDocument* pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
	//GetContextPrivate gets data to be associated with a context.
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	int index = JSVAL_TO_INT(id);

#ifdef LOG_HTMLDOM_DOC_GETPROPRTY
	if(index < 0)
		LOG_HTMLDOM("jhtml_doc_getProperty %s\n", jhtml_doc_props[-(JSVAL_TO_INT(id)+1)].name);
	else
		LOG_HTMLDOM("jhtml_doc_getProperty %d\n", index);
#endif

	if (!pDoc || !jsmgr)
	{
		return JS_TRUE;
	}

	if (index)
	{
		switch (JSVAL_TO_INT(id))
		{
			case HTMLDOCUMENT_TITLE:
			{
				//get the title
				//the JS_NewStringCopyZ function takes a char* and copies it into
				//a newly allocated char* and gives it to a new JSString.  The
				//memory allocated by this call will be garbage collected.
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pDoc->GetTitle()));
				return JS_TRUE;
			}

			case HTMLDOCUMENT_LOCATION:
			{
				*vp = OBJECT_TO_JSVAL(jsmgr->Location());
				return (JS_TRUE);
			}

			case HTMLDOCUMENT_REFERRER:
			{
				//get the last page in the document history
				HTMLBrowser *pBrowser = pDoc->GetBrowser();
				if (pBrowser)
				{
					WEBC_CHAR *str = pBrowser->Referer(pDoc);
					*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, str));
				}
				return JS_TRUE;
			}

			case HTMLDOCUMENT_DOMAIN:
			{
				break;
			}

			case HTMLDOCUMENT_URL_LC:
			case HTMLDOCUMENT_URL:
			{
				if (pDoc && pDoc->CurrentURL())
				{
					UrlStringBuilder ubld(pDoc->CurrentURL());
					WEBC_CHAR *str = ubld.MallocString(__FILE__, __LINE__);
					if (str)
					{
						*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, str));
						ubld.FreeString(str, __FILE__, __LINE__);
					}
				}
				return JS_TRUE;
			}

			case HTMLDOCUMENT_BODY:
			{
				//this case sets the value pointer to the JSObject associated with
				//the HTMLBody element for this document.
				*vp = OBJECT_TO_JSVAL(pDoc->Body()->GetJSObject());
				return JS_TRUE;
			}

			case HTMLDOCUMENT_COOKIE:
			{
			  #if (WEBC_SUPPORT_COOKIES)
				char *buffer;
				JSString *host = WEBC_JS_NEW_STRING_COPY_Z(cx, pDoc->CurrentURL()->getHost());
				JSString *path = WEBC_JS_NEW_STRING_COPY_Z(cx, pDoc->CurrentURL()->getPath());
				int request_len = 0;
				int secure = (pDoc->CurrentURL()->getProtocol() == WEBC_PROTOCOL_HTTPS);

				request_len = webc_print_matching_cookies(0, JS_GetStringBytes(host), JS_GetStringBytes(path), secure);

				buffer = (char *) WEBC_MALLOC((request_len + 1) * sizeof(char));
				if (!buffer)
				{
					return JS_TRUE;
				}

				webc_print_matching_cookies(buffer, JS_GetStringBytes(host), JS_GetStringBytes(path), secure);

				buffer[request_len] = '\0';

				*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, buffer));
				WEBC_FREE(buffer);
			  #endif //(WEBC_SUPPORT_COOKIES)
				return JS_TRUE;
			}

			case HTMLDOCUMENT_PARENTWINDOW:
			{
				*vp = OBJECT_TO_JSVAL(pDoc->GetJSMgr()->GetGlobalObject());
				return JS_TRUE;
			}

			case HTMLDOCUMENT_PROTOCOL:
			{
				JSString *str;
				switch (pDoc->CurrentURL()->getProtocol())
				{
					case WEBC_PROTOCOL_NONE:
						str = JS_InternString(cx, "none");
						break;
					case WEBC_PROTOCOL_HTTP:
						str = JS_InternString(cx, "http:");
						break;
					case WEBC_PROTOCOL_FTP:
						str = JS_InternString(cx, "ftp:");
						break;
					case WEBC_PROTOCOL_TELNET:
						str = JS_InternString(cx, "telnet:");
						break;
					case WEBC_PROTOCOL_FILE:
						str = JS_InternString(cx, "file:");
						break;
					case WEBC_PROTOCOL_LOCAL:
						str = JS_InternString(cx, "local:");
						break;
					case WEBC_PROTOCOL_HTTPS:
						str = JS_InternString(cx, "https:");
						break;
					case WEBC_PROTOCOL_WEBC:
						str = JS_InternString(cx, "webc:");
						break;
					case WEBC_PROTOCOL_WEBS:
						str = JS_InternString(cx, "webs:");
						break;
					case WEBC_PROTOCOL_CACHE:
						str = JS_InternString(cx, "cache:");
						break;
					default:
						str = JS_InternString(cx, "none");
				}
				*vp = STRING_TO_JSVAL(str);
				return JS_TRUE;
			}

			case HTMLDOCUMENT_UNIQUEID:
			{
				char str[52];
				nameGenerator(cx, str);
				*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, str));
				return JS_TRUE;
			}

			case HTMLDOCUMENT_DOCUMENTELEMENT:
			{
			  #if (WEBC_SUPPORT_FRAMES)
				if (pDoc->IsQuarantined())
				{
					*vp = OBJECT_TO_JSVAL(pDoc->GetJSObject());
				}
			  #endif // WEBC_SUPPORT_FRAMES

				HTMLDocument *pParent = pDoc->RootParent();
				if (pParent)
				{
					*vp = OBJECT_TO_JSVAL(pParent->GetJSObject());
				}
				else
				{
					*vp = OBJECT_TO_JSVAL(pDoc->GetJSObject());
				}

				break;
			}
		case HTMLDOCUMENT_NODETYPE:
		{
			*vp = INT_TO_JSVAL(9);
			return JS_TRUE;
		}

		case HTMLDOCUMENT_NODENAME:
		{
			static WEBC_CHAR tmpStr[] = {'H','T','M','L',0};
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, tmpStr));
			return JS_TRUE;
		}

			case HTMLDOCUMENT_FGCOLOR:
			{
				HTMLBody *body = (HTMLBody *)pDoc->Body();
				if (body)
				{
					WEBC_CHAR str[8];
					HTML_PrintColor(str, body->TextColor()); // is this the right color?
					*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, str));
					return JS_TRUE;
				}
			}

			case HTMLDOCUMENT_ALINK:
			case HTMLDOCUMENT_BACKGROUND:
			case HTMLDOCUMENT_BGCOLOR:
			case HTMLDOCUMENT_LINK:
			case HTMLDOCUMENT_TEXT:
			case HTMLDOCUMENT_VLINK:
			case HTMLDOCUMENT_LINKCOLOR:
			case HTMLDOCUMENT_VLINKCOLOR:
			case HTMLDOCUMENT_ALINKCOLOR:
			case HTMLDOCUMENT_CLIENTWIDTH:
			case HTMLDOCUMENT_CLIENTHEIGHT:
			case HTMLDOCUMENT_ONCLICK:
			case HTMLDOCUMENT_ONDBLCLICK:
			case HTMLDOCUMENT_ONKEYDOWN:
			case HTMLDOCUMENT_ONKEYPRESS:
			case HTMLDOCUMENT_ONKEYUP:
			case HTMLDOCUMENT_ONMOUSEDOWN:
			case HTMLDOCUMENT_ONMOUSEMOVE:
			case HTMLDOCUMENT_ONMOUSEOUT:
			case HTMLDOCUMENT_ONMOUSEOVER:
			case HTMLDOCUMENT_ONMOUSEUP:
			case HTMLDOCUMENT_ONFOCUS:
			case HTMLDOCUMENT_ONUNFOCUS:
			case HTMLDOCUMENT_ONLOAD:
			case HTMLDOCUMENT_ONUNLOAD:
			case HTMLDOCUMENT_ONSUBMIT:
			case HTMLDOCUMENT_ONCHANGE:
			{
				//these attributes are handled by the body
				int32 index = 0;
				if (JS_ValueToInt32(cx, id, &index) == JS_TRUE)
				{
					const char *name = jhtml_doc_props[0-(index+1)].name;
					JS_GetProperty(cx, pDoc->Body()->GetJSObject(), name, vp);
					return JS_TRUE;
				}
			}

			default:
				break;
		}
	}

	return JS_TRUE;
}

JSBool jhtml_body_setPropertyHtml(JSContext *cx, JSObject *obj, jsval *vp, HTMLAttributeType htmlAttribute);

/**
 * @memo This method is called when setting a Property in HTMLDocument
 *
 * @doc This method's parameters are very similar to /Ref{jhtml_doc_getProperty} except
 * the in this case 'vp' is the value the property is being set to.
 */
static JSBool jhtml_doc_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	//Get the HTMLDocument associated wih this JSObject
	HTMLDocument* pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
	//Get the JavascriptMangager associated with this context
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	int index = JSVAL_TO_INT(id);

	if(index < 0)
		LOG_HTMLDOM("jhtml_doc_setProperty %s\n", jhtml_doc_props[-(JSVAL_TO_INT(id)+1)].name);
	else
		LOG_HTMLDOM("jhtml_doc_setProperty %d\n", index);

	if (!pDoc || !jsmgr)
	{
		return JS_TRUE;
	}
	switch (index)
	{
		case HTMLDOCUMENT_TITLE:
		{
			//set the REAL HTMLDocument's title
			pDoc->SetTitle(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}
		case HTMLDOCUMENT_BODY:
		{
			//GMP this is not supported by our browser
			//
			//It is defined in the DOM as being read/write but we are keeping
			//it readonly for now
			break;
		}
		case HTMLDOCUMENT_COOKIE:
		{
#if (WEBC_SUPPORT_COOKIES)
			URLDescriptor *desc = pDoc->CurrentURL();
		  #if (WEBC_SUPPORT_UNICODE)
			webc_set_cookie_uc(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)), desc->getHost(), desc->getPath());
		  #else
			webc_set_cookie(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)), desc->getHost(), desc->getPath());
		  #endif
#endif //(WEBC_SUPPORT_COOKIES)
			return JS_TRUE;
		}
		case HTMLDOCUMENT_FGCOLOR:
// < EDIT 20080120 - Added support for HTMLDOCUMENT_BGCOLOR
//		case HTMLDOCUMENT_BGCOLOR:
		{
            int t = JSVAL_TO_INT(id);
            HTMLAttributeType Property = HTML_ATTRIB_TEXT;
			WEBC_CHAR *str = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));

//            if (t == HTMLDOCUMENT_FGCOLOR)
                Property = HTML_ATTRIB_TEXT;
//            else if (t == HTMLDOCUMENT_BGCOLOR)
//            {
//                Property = HTML_ATTRIB_BGCOLOR;
//                printf("Setting HTML_ATTRIB_BACKGROUND to %S\n", str);
//                Property = HTML_ATTRIB_BACKGROUND;
//            }

			if (JSVAL_IS_STRING(*vp) || JSVAL_IS_INT(*vp))
			{
				HTMLBody *body = (HTMLBody *) pDoc->Body();
				if (str && body)
				{
					body->SetProperty(Property, str);
					body->Update(0,0);
                    pDoc->GetBrowser()->GetDisplayManager()->Invalidate();
                    // << EDIT 20080120 - added refresh
                    pDoc->Refresh();
				}
			}
			return JS_TRUE;
		}


		case HTMLDOCUMENT_ALINK:
		case HTMLDOCUMENT_BACKGROUND:
		case HTMLDOCUMENT_BGCOLOR:
		case HTMLDOCUMENT_LINK:
		case HTMLDOCUMENT_TEXT:
		case HTMLDOCUMENT_VLINK:
		case HTMLDOCUMENT_CLIENTWIDTH:
		case HTMLDOCUMENT_CLIENTHEIGHT:
		case HTMLDOCUMENT_LINKCOLOR:
		case HTMLDOCUMENT_VLINKCOLOR:
		case HTMLDOCUMENT_ALINKCOLOR:
		case HTMLDOCUMENT_ONCLICK:
		case HTMLDOCUMENT_ONDBLCLICK:
		case HTMLDOCUMENT_ONKEYDOWN:
		case HTMLDOCUMENT_ONKEYPRESS:
		case HTMLDOCUMENT_ONKEYUP:
		case HTMLDOCUMENT_ONMOUSEDOWN:
		case HTMLDOCUMENT_ONMOUSEMOVE:
		case HTMLDOCUMENT_ONMOUSEOUT:
		case HTMLDOCUMENT_ONMOUSEOVER:
		case HTMLDOCUMENT_ONMOUSEUP:
		case HTMLDOCUMENT_ONFOCUS:
		case HTMLDOCUMENT_ONUNFOCUS:
		case HTMLDOCUMENT_ONLOAD:
		case HTMLDOCUMENT_ONUNLOAD:
		case HTMLDOCUMENT_ONSUBMIT:
		case HTMLDOCUMENT_ONCHANGE:
		{
			//these attributes are handled by the body
			int32 index = 0;
			if (JS_ValueToInt32(cx, id, &index) == JS_TRUE && pDoc->Body())
   		    {
                // Set the JS property value for this document object
				const char *name = jhtml_doc_props[0-(index+1)].name;
				JS_SetProperty(cx, pDoc->Body()->GetJSObject(), name, vp);

                // Calculate the HTML Attribute value to pass to jhtml_body_setPropertyHtml
				HTMLAttributeType htmlAttribute = HTML_UNKNOWN_ATTRIB;

        		switch (JSVAL_TO_INT(id))
        		{
                    case HTMLDOCUMENT_ALINK:      htmlAttribute = HTML_ATTRIB_ALINK; break;
            		case HTMLDOCUMENT_BACKGROUND: htmlAttribute = HTML_ATTRIB_BACKGROUND; break;
            		case HTMLDOCUMENT_BGCOLOR:      htmlAttribute = HTML_ATTRIB_BGCOLOR; break;
            		case HTMLDOCUMENT_LINK:     htmlAttribute = HTML_ATTRIB_LINK; break;
            		case HTMLDOCUMENT_TEXT:     htmlAttribute = HTML_ATTRIB_TEXT; break;
            		case HTMLDOCUMENT_VLINK:        htmlAttribute = HTML_ATTRIB_VLINK; break;
            		case HTMLDOCUMENT_CLIENTWIDTH:      htmlAttribute = HTML_ATTRIB_WIDTH; break;
            		case HTMLDOCUMENT_CLIENTHEIGHT:     htmlAttribute = HTML_ATTRIB_HEIGHT; break;
            		case HTMLDOCUMENT_LINKCOLOR:        htmlAttribute = HTML_ATTRIB_LINK; break;
            		case HTMLDOCUMENT_VLINKCOLOR:       htmlAttribute = HTML_ATTRIB_VLINK; break;
            		case HTMLDOCUMENT_ALINKCOLOR:       htmlAttribute = HTML_ATTRIB_ALINK; break;
            		case HTMLDOCUMENT_ONCLICK:          htmlAttribute = HTML_ATTRIB_ONCLICK; break;
            		case HTMLDOCUMENT_ONDBLCLICK:       htmlAttribute = HTML_ATTRIB_ONDBLCLICK; break;
            		case HTMLDOCUMENT_ONKEYDOWN:        htmlAttribute = HTML_ATTRIB_ONKEYDOWN; break;
            		case HTMLDOCUMENT_ONKEYPRESS:       htmlAttribute = HTML_ATTRIB_ONKEYPRESS; break;
            		case HTMLDOCUMENT_ONKEYUP:          htmlAttribute = HTML_ATTRIB_ONKEYUP; break;
            		case HTMLDOCUMENT_ONMOUSEDOWN:      htmlAttribute = HTML_ATTRIB_ONMOUSEDOWN; break;
            		case HTMLDOCUMENT_ONMOUSEMOVE:      htmlAttribute = HTML_ATTRIB_ONMOUSEMOVE; break;
            		case HTMLDOCUMENT_ONMOUSEOUT:       htmlAttribute = HTML_ATTRIB_ONMOUSEOUT; break;
            		case HTMLDOCUMENT_ONMOUSEOVER:      htmlAttribute = HTML_ATTRIB_ONMOUSEOVER; break;
            		case HTMLDOCUMENT_ONMOUSEUP:        htmlAttribute = HTML_ATTRIB_ONMOUSEUP; break;
            		case HTMLDOCUMENT_ONFOCUS:          htmlAttribute = HTML_ATTRIB_ONFOCUS; break;
//            		case HTMLDOCUMENT_ONUNFOCUS:        htmlAttribute = HTML_ATTRIB_ONUNFOCUS; break;
            		case HTMLDOCUMENT_ONLOAD:           htmlAttribute = HTML_ATTRIB_ONLOAD; break;
            		case HTMLDOCUMENT_ONUNLOAD:         htmlAttribute = HTML_ATTRIB_ONUNLOAD; break;
            		case HTMLDOCUMENT_ONSUBMIT:         htmlAttribute = HTML_ATTRIB_ONSUBMIT; break;
            		case HTMLDOCUMENT_ONCHANGE:         htmlAttribute = HTML_ATTRIB_ONCHANGE; break;
            		default:         htmlAttribute = HTML_UNKNOWN_ATTRIB; break;
               }


				if (index == HTMLDOCUMENT_BGCOLOR)
                {
                    return(jhtml_body_setPropertyHtml(cx, pDoc->Body()->GetJSObject(), vp, htmlAttribute));
                }
				return JS_TRUE;
			}
		}

		case HTMLDOCUMENT_LOCATION:
		{
			WEBC_CHAR *pUrlStr = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
			if (pDoc)
			{
				jutils_LoadDocumentUrl(cx, pDoc, pUrlStr);
			}
		}

		default:
			break;
	}
    return JS_TRUE;
}

/**
 * @memo This JSClass is the javascript wrapper for the HTMLDocument class defined in the DOM Level 1
 *
 * @doc This struct defines the name of the class as well as the functions to use to access/mutate the
 * attributes associated with it.
 *
 */
JSClass JSHtmlDocument =
{
	"HTMLDocument", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty, (JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_doc_getProperty,
	(JSPropertyOp) jhtml_doc_setProperty, (JSEnumerateOp) jutil_enumerate, (JSResolveOp) jhtml_doc_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};



/**
 * @memo This method return the associated JSClass that contains Class specific information,
 * not instance specific.
 */
JSClass *GetJHtmlDocument()
{
	return &JSHtmlDocument;
}

/**
 * @memo This function writes a string to the document followed by a <br>
 *
 * @doc This is a wrapper function that is called when HTMLDocument::writeln(char *) is called in
 * javascript.<br>
 * HTMLDocument writeln method
 *
 * This method writes a string to the document followed by a break <br>
 * Param - String - string to write
 *
 * @param cx JSContext associated with this window
 * @param obj the JS HTMLDocument object that this method was called on
 * @param argc the number of arguments this method took
 * @param argv an array of the agruments passed in
 * @param rval the return val this method will hand back.
 *
 * @returns status
 */

extern JSClass JSWindow;

static JSBool jhtml_doc_writeln(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	JSClass *jsclass = JS_GetClass(obj);

	LOG_HTMLDOM("jhtml_doc_writeln\n");

	jsval jv;
	if (jsclass == &JSWindow)
	{
		if ((JS_GetProperty(cx, obj, "document", &jv) == JS_TRUE) && JSVAL_IS_OBJECT(jv))
		{
			obj = JSVAL_TO_OBJECT(jv);
			jsclass = JS_GetClass(obj);
		}
	}

	if (jsclass != &JSHtmlDocument)
	{
		// this is not a Document object
		*rval = JSVAL_VOID;
		return JS_TRUE;
	}

	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);

	long bytes = 0;
	if (pDoc && jsmgr && argc > 0)
	{
		WEBC_CHAR *str;

		if (!pDoc->IsOpen())
		{
			pDoc->ClearDocument();
			pDoc->Open();
		}

		while (argc > 0)
		{
			str = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *argv));

			//FILE *fp = fopen("jswrite.txt", "a");
			//fprintf(fp, "%s\n", str);
			//fclose(fp);

			bytes += pDoc->WriteLine(str);
			argc--;
			argv++;
		}
	}
// << EDIT 20080120 - added refresh
//	pDoc->Refresh();
	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/**
 * @memo HTMLDocument write method
 *
 * @doc This method writes a string to the document.<br>
 * Param - String - string to write<br>
 * returns - void
 */
static JSBool jhtml_doc_write(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	JSClass *jsclass = JS_GetClass(obj);

	LOG_HTMLDOM("jhtml_doc_write\n");

	jsval jv;
	if (jsclass == &JSWindow)
	{
		if ((JS_GetProperty(cx, obj, "document", &jv) == JS_TRUE) && JSVAL_IS_OBJECT(jv))
		{
			obj = JSVAL_TO_OBJECT(jv);
			jsclass = JS_GetClass(obj);
		}
	}

	if (jsclass != &JSHtmlDocument)
	{
		// this is not a Document object
		*rval = JSVAL_VOID;
		return JS_TRUE;
	}

	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);

	long bytes = 0;
	if (pDoc && jsmgr && argc > 0)
	{
		WEBC_CHAR *str;

		if (!pDoc->IsOpen())
		{
			pDoc->ClearDocument();
			pDoc->Open();
		}

		while (argc > 0)
		{
			str = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *argv));

			//FILE *fp = fopen("jswrite.txt", "a");
			//fprintf(fp, "%s\n", str);
			//fclose(fp);

			bytes += pDoc->Write(str);
			argc--;
			argv++;
		}
	}
// << EDIT 20080120 - added refresh
//	pDoc->Refresh();
	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/**
 * @memo This function calls the HTMLDocument::Close() method.
 *
 * @doc This is a wrapper function for HTMLDocument::Close()<br>
 * Params  - none <br>
 * returns - void
 */
static JSBool jhtml_doc_close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
// << EDIT 20080120 - Re-enabled pDoc->Close(); inside jhtml_doc_close()
	LOG_HTMLDOM("jhtml_doc_close\n");

	pDoc->Close();
// << EDIT 20080120 - added refresh
	pDoc->Refresh();
	*rval = JSVAL_VOID;
	return JS_TRUE;
}


/**
 * @memo wrapper function for HTMLDocument::LoadUrl()
 *
 * @doc This method calls the HTMLDocument::LoadUrl() method.
 *
 * Params  - none<br>
 * returns - void
 */

static JSBool jhtml_doc_load(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument* doc = (HTMLDocument*) jhutil_GetPrivate(cx, obj);

	LOG_HTMLDOM("jhtml_doc_load\n");

	if (doc && JS_GetClass(obj) == &JSHtmlDocument && argc > 0 && JSVAL_IS_STRINGABLE(*argv))
	{
		doc->LoadUrl(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *argv)), WEBC_TRUE);
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/**
 * @memo wrapper function for HTMLDocument::Open()
 *
 * @doc This method calls the HTMLDocument::Open() method.  It allows things to be written to the
 * document after it has been closed.
 * <br>
 * Params  - none<br>
 * returns - void
 */
static JSBool jhtml_doc_open(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
	LOG_HTMLDOM("jhtml_doc_open\n");

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jhutil_CreateElementFromText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

JSBool jhtml_doc_CreateElement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);

	LOG_HTMLDOM("jhtml_doc_CreateElement\n");

	if (argc > 0 && JSVAL_IS_STRINGABLE(argv[0]))
	{

		WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
		WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
		WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));

		if (jsmgr)
        {
            return(jhutil_CreateElementFromText(cx, obj, argc, argv, rval));
        }
    }
	return JS_FALSE;
}

// implements doc.createTextNode()
JSBool jhtml_doc_CreateTextNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLString *Unit;
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
	JSObject *myObject;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;

	LOG_HTMLDOM("jhtml_doc_CreateTextNode\n");

	if (jsmgr && argc > 0 && JSVAL_IS_STRINGABLE(argv[0]))
	{
		WEBC_CHAR *str = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));
		WEBC_NEW_VERBOSE(Unit, HTMLString(pDoc, str),"HTMLString");
        // jsmgr->CreateElement() allocates a JSOBJ
	    myObject = jsmgr->CreateElement(HTML_ELEMENT); // AK_TODO potential leak?
        jhutil_SetPrivate(cx, myObject, Unit);
        *rval = OBJECT_TO_JSVAL(myObject);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool jhtml_doc_releaseCapture(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;

	LOG_HTMLDOM("jhtml_doc_releaseCapture\n");

	if (!pDoc || !jsmgr)
	{
		return JS_TRUE;
	}

	if (pDoc && (argc == 0))
	{
		DisplayManager *pDisplayMgr = jsscr->GetDisplayManager();
		pDisplayMgr->SetCapture(WEBC_NULL);

		// trigger the onlosecapture event handler
		HTMLEvent e;
		e.type = HTML_EVENT_LOSECAPTURE;
		pDoc->Body()->Event(&e);

		return JS_TRUE;
	}
	return JS_FALSE;
}

JSBool jhtml_doc_insertBefore(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);

	LOG_HTMLDOM("jhtml_doc_insertBefore\n");

	if (pDoc && (argc > 0))
	{
	    JSObject *jNewNodeObj = JSVAL_IS_OBJECT(argv[0])? JSVAL_TO_OBJECT(argv[0]) : 0;
	    JSObject *jChildNodeObj = JSVAL_IS_OBJECT(argv[1])? JSVAL_TO_OBJECT(argv[1]) : 0; // optional
        if (jNewNodeObj)
        {
            HTMLElement *pNewNode = (HTMLElement *)jhutil_GetPrivate(cx, jNewNodeObj);
			HTMLElement *pChildNode = jChildNodeObj ? ((HTMLElement *)jhutil_GetPrivate(cx, jNewNodeObj)) : 0;
            if (pNewNode)
            {
				// if the new node is a body, then we must have a fragment created with createDocumentFragment
				if(HTML_TAG_BODY == pNewNode->GetTag())
				{
					return JS_FALSE;
				}
				else
				{
					pDoc->Body()->InsertBefore(pNewNode, pChildNode);
					return JS_TRUE;
				}
            }
        }
	}
	return JS_FALSE;
}

JSBool jhtml_doc_removeChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);

	LOG_HTMLDOM("jhtml_doc_removeChild\n");

	if (pDoc && (argc > 0))
	{
	    JSObject *jvalobj = JSVAL_IS_OBJECT(argv[0])? JSVAL_TO_OBJECT(argv[0]) : 0;
        if (jvalobj)
        {
            HTMLElement *pChild = (HTMLElement *)jhutil_GetPrivate(cx, jvalobj);
            if (pChild)
            {
                pDoc->Body()->Remove(pChild);
                return JS_TRUE;
            }
        }
	}
	return JS_FALSE;
}

JSBool jhtml_doc_CreateComment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	jsval nameArg[1];
	static WEBC_CHAR scriptTagName[] = {'s','c','r','i','p','t',0};

	LOG_HTMLDOM("jhtml_doc_CreateComment\n");

	// since we don't store comments in the DOM, this call is a NOOP - BUT we
	// have to return an object, so create an empty script block
	nameArg[0] = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, scriptTagName));

	return jhtml_doc_CreateElement(cx, obj, 1, nameArg, rval);

	return JS_TRUE;
}

static JSBool jhtml_doc_getElementsByTagName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;

	if (pDoc && (argc > 0))
	{
		HTMLElement *pElem = pDoc->Body();
		if (pDoc && argc > 0 && JSVAL_IS_STRINGABLE(argv[0]))
		{
			WEBC_CHAR *tagName = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));
			jhtml_collection *coll;

			LOG_HTMLDOM("jhtml_doc_getElementsByTagName %S\n", tagName);

			if (!webc_c_strcmp(tagName, "*"))
			{
				coll = jhtml_collection_New(jsmgr, obj, "elementsByTagName", pElem, ELEMENT_NODES); // TODO is this the right collection?
				if (coll)
				{
					*rval = OBJECT_TO_JSVAL(coll->jsObj.Get());
					return JS_TRUE;
				}
			}
			else
			{
				coll = jhtml_collection_New(jsmgr, obj, "elementsByTagName", pElem, ALL_TAGS_BY_TAGNAME);
				if (coll)
				{
					coll->nameOfAll = webc_malloc_string_copy(tagName, __FILE__, __LINE__);
					*rval = OBJECT_TO_JSVAL(coll->jsObj.Get());
					return JS_TRUE;
				}
			}
		}
	}

	*rval = JSVAL_NULL;
	return JS_TRUE;
}

static JSBool jhtml_doc_createDocumentFragment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;

	LOG_HTMLDOM("jhtml_doc_createDocumentFragment\n");

	if (pDoc)
	{
		HTMLBody *pNewBody = 0;

		WEBC_NEW_VERBOSE(pNewBody, HTMLBody(pDoc, WEBC_TRUE),"jhtml_doc_createDocumentFragment"); // AK_TODO this likely leaks?

		if (pNewBody)
		{
			pNewBody->CreateJSObject();
			*rval = OBJECT_TO_JSVAL(pNewBody->GetJSObject());
			return JS_TRUE;
		}
	}
	*rval = JSVAL_NULL;
	return JS_TRUE;
}

/**
 * @memo This function returns a HTMLCollection of Elements with the specified name.
 *
 * @doc The HTMLCollection is implimented via HTMLElementFinders <br>
 * Param - name - name of elements to be in collection<br>
 * returns - HTMLCollection of HTMLElements
 *
 * @see jhtml_collection
 */

static JSBool jhtml_doc_getElementsByName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc > 0 && JSVAL_IS_STRINGABLE(argv[0]))
	{
		WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));
		if (!name)
		{
			*rval = JSVAL_NULL;
			return JS_TRUE;
		}
		WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
		WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
		if (jsmgr)
		{
			vector_iterator pvi[1];
			jhtml_collection *coll = (jhtml_collection *)
				vector_get_first(jsmgr->GetCollections(), pvi);
			//first look at all the collections in the jsmgr and find out if we already created
			//this one
			for (; coll != 0; coll = (jhtml_collection *)vector_get_next(pvi))
			{
				if (coll->finderType == ALL_BY_NAME && coll->nameOfAll && webc_stricmp(coll->nameOfAll, name))
				{
					//if we have, just return that, no need to allocate new memory
					*rval = OBJECT_TO_JSVAL(coll->jsObj.Get());
					return JS_TRUE;
				}
			}//end for
			HTMLDocument *pDoc = jsmgr->GetDocument();
			if (pDoc && pDoc->Body())
			{
				char new_name[42];
				//all JSObjects need a name so we will create a unique one with our nameGenerator
				nameGenerator(cx, new_name);
				//create a new collection
				coll = jhtml_collection_New(jsmgr, pDoc->GetJSObject(), new_name, pDoc->Body(), ALL_BY_NAME);
				if (coll)
				{
					coll->nameOfAll = webc_malloc_string_copy(name, __FILE__, __LINE__);
					*rval = OBJECT_TO_JSVAL(coll->jsObj.Get());
					return JS_TRUE;
				}
			}
		}//end if
	}
	*rval = JSVAL_NULL;
	return JS_TRUE;;
}

/**
 * @memo This function returns an HTMLElement with the specified ID
 *
 * @doc  @doc The HTMLCollection is implimented via HTMLElementFinders <br>
 * Param - id - the string id of the element to get.
 * returns - HTMLElement
 *
 * @see jhtml_collection
 */
static JSBool jhtml_doc_getElementByID(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (JS_GetClass(obj) == &JSHtmlDocument)
	{
		HTMLDocument *pDoc = (HTMLDocument*) jhutil_GetPrivate(cx, obj);
		if (pDoc && argc > 0 && JSVAL_IS_STRINGABLE(argv[0]))
		{
			WEBC_CHAR *id = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));
			HTMLElementByIdFinder finder(id);
			HTMLElement *pElem = pDoc->FindElement(&finder);

			if (pElem)
			{
				*rval = OBJECT_TO_JSVAL(pElem->CreateJSObject());
				return JS_TRUE;
			}
		}
	}

	*rval = JSVAL_NULL;
	return JS_TRUE;
}

static JSBool jhtml_doc_attachEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (JS_GetClass(obj) == &JSHtmlDocument)
	{
		HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
		if (pDoc && pDoc->Body())
		{
			obj = pDoc->Body()->GetJSObject();
			return jhtml_element_attachEvent(cx, obj, argc, argv, rval);
		}
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

static JSBool jhtml_doc_detachEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (JS_GetClass(obj) == &JSHtmlDocument)
	{
		HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
		if (pDoc && pDoc->Body())
		{
			obj = pDoc->Body()->GetJSObject();
			return jhtml_element_detachEvent(cx, obj, argc, argv, rval);
		}
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

static JSBool jhtml_doc_fireEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (JS_GetClass(obj) == &JSHtmlDocument)
	{
		HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
		if (pDoc && pDoc->Body())
		{
			obj = pDoc->Body()->GetJSObject();
			return jhtml_element_fireEvent(cx, obj, argc, argv, rval);
		}
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/**
 * @memo This struct associates strings with the wrapper methods defined above
 *
 * @doc The fields of this struct are the interface functions for HTMLDocument
 */
JSFunctionSpec jhtml_doc_methods[] =
{
	{"close",              (JSNative) jhtml_doc_close,             0},
  	{"getElementsByName",  (JSNative) jhtml_doc_getElementsByName, 1},
  	{"getElementById",     (JSNative) jhtml_doc_getElementByID,    1},
	{"getElementByID",     (JSNative) jhtml_doc_getElementByID,    1},
 	{"load",               (JSNative) jhtml_doc_load,              1},
 	{"open",               (JSNative) jhtml_doc_open,              0},
  	{"write",              (JSNative) jhtml_doc_write,             0},
  	{"writeln",            (JSNative) jhtml_doc_writeln,           1},
	{"attachEvent",        (JSNative) jhtml_doc_attachEvent,       2},
	{"detachEvent",        (JSNative) jhtml_doc_detachEvent,       2},
	{"fireEvent",          (JSNative) jhtml_doc_fireEvent,         2},
  	{"createElement",      (JSNative) jhtml_doc_CreateElement,     1},
  	{"createTextNode",     (JSNative) jhtml_doc_CreateTextNode,     1},
	{"releaseCapture",     (JSNative) jhtml_doc_releaseCapture, 0},
	{"insertBefore",       (JSNative) jhtml_doc_insertBefore, 2},
	{"removeChild",        (JSNative) jhtml_doc_removeChild, 1},
  	{"createComment",      (JSNative) jhtml_doc_CreateComment,     1},
  	{"getElementsByTagName",  (JSNative) jhtml_doc_getElementsByTagName, 1},
  	{"createDocumentFragment",  (JSNative) jhtml_doc_createDocumentFragment, 0},
	{0}
};



/**
 * @memo This is the constructor used for this class
 *
 * @doc A document cannot be created via constructor in MSIE so this will most likely
 * never be called.
 */
static JSBool JHTMLDocument(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return JS_TRUE;
}


/**
 * @memo This function is called by the jscriptmanager to create the HTMLDocument
 * class.
 *
 * @doc This function is called by \Ref{WebcJSDocumentContext}'s constructor.  It initializes the
 * JSClass associated with HTMLDocument.
 *
 * @param cx the context the class will live in.
 * @param proto the class this class will inherit from
 *
 * @return this class as a JSObject so that other classes can inherit from it.
 */
JSObject *jhtml_doc_Init(JSContext *cx, JSObject * proto)
{
	//initialize the HTMLDocument Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlDocument,
    				(JSNative) JHTMLDocument, 0,
    				jhtml_doc_props, jhtml_doc_methods,
    				WEBC_NULL, WEBC_NULL);
}
#endif //(WEBC_SUPPORT_JSCRIPT)

