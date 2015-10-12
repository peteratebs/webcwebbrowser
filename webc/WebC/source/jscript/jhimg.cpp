/*
|  JHIMG.CPP -
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
#include "jhimg.h"
#if (WEBC_SUPPORT_JSCRIPT)
#include "jhutils.h"
#include "html.h"
#include "htmldoc.hpp"
#include "webload.hpp"
#include "wjscript.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhelem.h"
#include "wjsdebug.h"

#if(WEBC_IMAGE_DRAW_EXTENSIONS)
#include "jhimg_ext.h"
#endif

/**
 * This is the JS HTMLImage class definition
 */

//set the unique ids for the properties of the image element class
enum jhtml_image_tinyid
{
	HTMLIMAGEELEMENT_LOWSRC=-1,
	HTMLIMAGEELEMENT_NAME=-2,
	HTMLIMAGEELEMENT_ALIGN=-3,
	HTMLIMAGEELEMENT_ALT=-4,
	HTMLIMAGEELEMENT_BORDER=-5,
	HTMLIMAGEELEMENT_HSPACE=-6,
	HTMLIMAGEELEMENT_ISMAP=-7,
	HTMLIMAGEELEMENT_LONGDESC=-8,
	HTMLIMAGEELEMENT_SRC=-9,
	HTMLIMAGEELEMENT_USEMAP=-10,
	HTMLIMAGEELEMENT_VSPACE=-11,
	HTMLIMAGEELEMENT_WIDTH=-12,
	HTMLIMAGEELEMENT_HEIGHT=-13
};

static WEBC_READONLY HTMLAttributeType jimg_id_to_param [] =
{
	HTML_ATTRIB_SRC,
	HTML_ATTRIB_NAME,
	HTML_ATTRIB_ALIGN,
	HTML_ATTRIB_ALT,
	HTML_ATTRIB_BORDER,
	HTML_ATTRIB_HSPACE,
	HTML_ATTRIB_ISMAP,
	HTML_ATTRIB_LONGDESC,
	HTML_ATTRIB_SRC,
	HTML_ATTRIB_USEMAP,
	HTML_ATTRIB_VSPACE,
	HTML_ATTRIB_WIDTH,
	HTML_ATTRIB_HEIGHT
};


/**
 *  This method is called when getting a Property from HTMLImageElement
 */
static JSBool jhtml_image_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLImage *img = (pElem)? (HTMLImage *) pElem : 0;

	if (!img)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
#if (WEBC_SUPPORT_IMAGES)
		case HTMLIMAGEELEMENT_LOWSRC:
			//lowsrc is not implemented in WebC
		case HTMLIMAGEELEMENT_SRC:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, img->Src()));
			return JS_TRUE;
		}
#endif
		case HTMLIMAGEELEMENT_NAME:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, img->Name()));
			return JS_TRUE;
		}
		case HTMLIMAGEELEMENT_ALIGN:
		{
			switch(img->GetFloat())
			{
			case TU_FLOAT_LEFT:
				*vp = STRING_TO_JSVAL(JS_InternString(cx, "left"));
				break;
			case TU_FLOAT_RIGHT:
				*vp = STRING_TO_JSVAL(JS_InternString(cx, "right"));
				break;
			case TU_FLOAT_NONE:
				*vp = STRING_TO_JSVAL(JS_InternString(cx, "undefined"));
				break;
			}
			return JS_TRUE;
		}
		case HTMLIMAGEELEMENT_ALT:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, img->Alt()));
			return JS_TRUE;
		}
		case HTMLIMAGEELEMENT_BORDER:
		{
			CSSBorderThickness thickness;
			short iThicknessVal=0;
			CSSBorderStyle style = img->GetBorderStyle(CSS_TOP);
			if(style == CSS_BORDER_STYLE_SOLID)
			{
				img->GetBorderWidth(CSS_TOP, &thickness);
				if((thickness.type == CSS_BORDER_THICKNESS_LENGTH) && (thickness.val.length.type == CSS_LENGTH_UNIT_PX))
					iThicknessVal = thickness.val.length.val.dec;
			}
			*vp = INT_TO_JSVAL(iThicknessVal);
			return JS_TRUE;
		}
		case HTMLIMAGEELEMENT_HEIGHT:
		{
			*vp = INT_TO_JSVAL(img->GetPixelHeight());
			return JS_TRUE;
		}
		case HTMLIMAGEELEMENT_HSPACE:
		{
			*vp = INT_TO_JSVAL(img->GetHSpace());
			break;
		}
		case HTMLIMAGEELEMENT_ISMAP:
		{
			if (img->IsMap())
			{
				*vp = BOOLEAN_TO_JSVAL(JS_TRUE);
				return JS_TRUE;
			}
			*vp = BOOLEAN_TO_JSVAL(JS_FALSE);
			return JS_TRUE;
		}
		case HTMLIMAGEELEMENT_LONGDESC:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, img->LongDesc()));
			return JS_TRUE;
		}
		case HTMLIMAGEELEMENT_USEMAP:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, img->UseMap()));
			return JS_TRUE;
		}
		case HTMLIMAGEELEMENT_VSPACE:
		{
			*vp = INT_TO_JSVAL(img->GetVSpace());
			break;
		}
		case HTMLIMAGEELEMENT_WIDTH:
		{
			*vp = INT_TO_JSVAL(img->GetPixelWidth());
			return JS_TRUE;
		}
	}//end switch
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLImageElement
 */
static JSBool jhtml_image_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLImage *img = (pElem)? (HTMLImage *) pElem : 0;

	if (!img)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLIMAGEELEMENT_LOWSRC:
		case HTMLIMAGEELEMENT_SRC:
		case HTMLIMAGEELEMENT_ALIGN:
		case HTMLIMAGEELEMENT_ALT:
		case HTMLIMAGEELEMENT_BORDER:
		case HTMLIMAGEELEMENT_HEIGHT:
		case HTMLIMAGEELEMENT_HSPACE:
		case HTMLIMAGEELEMENT_ISMAP:
		case HTMLIMAGEELEMENT_LONGDESC:
		case HTMLIMAGEELEMENT_USEMAP:
		case HTMLIMAGEELEMENT_VSPACE:
		case HTMLIMAGEELEMENT_WIDTH:
		{
			JSString *jsStr;
			if (JSVAL_IS_STRING(*vp))
			{
				jsStr = JSVAL_TO_STRING(*vp);
			}
			else
			{
				jsStr = JS_ValueToString(cx, *vp);
			}

			JS_AddRoot(cx, &jsStr);

			HTMLDocument *doc = img->GetDocument();
			ObjectReference<DisplayManager> manager(doc? doc->GetDisplayManager() : 0);

			WEBC_CHAR *str = WEBC_JS_STRING_TO_WEBC_STRING(jsStr);
			img->SetProperty(jimg_id_to_param[-1 - JSVAL_TO_INT(id)], str);
			img->Update(0,(WEBC_PFBYTE)1);

			JS_RemoveRoot(cx, &jsStr);

			break;
		}
	}//end switch

	return jutils_CheckContextState(cx);
}


JSBool jhtml_image_resolve(JSContext *cx, JSObject *obj, jsval id)
{
	return(jhtml_element_resolve(cx, obj, id));
}

/**
 * This JSClass is the javascript wrapper for the HTMLImageElement class defined in the DOM Level 1
 */
JSClass JSHtmlImageElement =
{
	"Image", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_image_getProperty,
	(JSPropertyOp) jhtml_image_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_image_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};

/**
 * This function returns the HTMLImageElement class
 */
JSClass *getHtmlImageElement()
{
	return &JSHtmlImageElement;
}

/**
 * These are the attributes of HTMLImageElement
 */
JSPropertySpec jhtml_image_props[] = {
	{"lowSrc", 	 HTMLIMAGEELEMENT_LOWSRC,   JSPROP_ENUMERATE},
	{"name",     HTMLIMAGEELEMENT_NAME,     JSPROP_ENUMERATE | JSPROP_READONLY},
	{"align",    HTMLIMAGEELEMENT_ALIGN,    JSPROP_ENUMERATE},
	{"alt",      HTMLIMAGEELEMENT_ALT,      JSPROP_ENUMERATE},
	{"border",   HTMLIMAGEELEMENT_BORDER,   JSPROP_ENUMERATE},
	{"hspace",   HTMLIMAGEELEMENT_HSPACE,   JSPROP_ENUMERATE},
	{"isMap",    HTMLIMAGEELEMENT_ISMAP,    JSPROP_ENUMERATE},
	{"longDesc", HTMLIMAGEELEMENT_LONGDESC, JSPROP_ENUMERATE},
	{"src",      HTMLIMAGEELEMENT_SRC,      JSPROP_ENUMERATE},
	{"useMap",   HTMLIMAGEELEMENT_USEMAP,   JSPROP_ENUMERATE},
	{"vspace",   HTMLIMAGEELEMENT_VSPACE,   JSPROP_ENUMERATE},
	{"width",    HTMLIMAGEELEMENT_WIDTH,    JSPROP_ENUMERATE},
	{"height",   HTMLIMAGEELEMENT_HEIGHT,   JSPROP_ENUMERATE},
    {0}
};

/**
 * There are no functions for HTMLImageElement
 */
JSFunctionSpec jhtml_image_methods[] =
{
#if (WEBC_IMAGE_DRAW_EXTENSIONS)
	 {"webCBeginDraw",     (JSNative) jhtml_img_ext_webCBeginDraw,     0},
	 {"webCEndDraw",     (JSNative) jhtml_img_ext_webCEndDraw,     0},
	 {"webCRefreshSrc",     (JSNative) jhtml_img_ext_webCRefreshSrc,     0},
	 {"webCBrushWidth",     (JSNative) jhtml_img_ext_webCBrushWidth,     1},
	 {"webCColor",     (JSNative) jhtml_img_ext_webCColor,     1},
	 {"webCLine",     (JSNative) jhtml_img_ext_webCLine,     4},
	 {"webCFillPolygon",     (JSNative) jhtml_img_ext_webCFillPolygon,     1},
	 {"webCFillRect",     (JSNative) jhtml_img_ext_webCFillRect,     4},
	 {"webCEllipse",     (JSNative) jhtml_img_ext_webCEllipse,     4},
	 {"webCArc",     (JSNative) jhtml_img_ext_webCArc,     6},
	 {"webCFillEllipse",     (JSNative) jhtml_img_ext_webCFillEllipse,     1},
#endif
	{0}
};

/**
 * This is the constructor for HTMLImageElement
 */
static JSBool JHTMLImageElement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;

	HTMLImage *pImg;
// <EDIT> 20080120 JHTMLImageElement() image_constructor review, compare, note
// <EDIT> 20080120 Look at jutils, what about calling class constructors for element create.
// <EDIT> 20080120 Note use of: jsmgr->AddNewElement(pImg);
	if (jsmgr)
	{
		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_IMG);

		WEBC_NEW_VERBOSE(pImg, HTMLImage(tag, jsmgr->GetDocument(), obj),"JHTMLImageElement");
		if (pImg)
		{
#if (WEBC_INCLUDE_SEPT2010_BUGFIXES)
			pImg->SetJSObject(obj);  // 10/05/2010 add missing SetJSObject call so we can access the object from webC event handler dispatch.
#endif
			pImg->SetTagProperties(tag, jsmgr->GetDocument());
			jsmgr->AddNewElement(pImg);
		}
	}
	return JS_TRUE;
}

/**
 * This function is used to initialize the HTMLImageELement Class
 */
JSObject *jhtml_image_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLImageElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlImageElement,
    				(JSNative) JHTMLImageElement, 0,
    				jhtml_image_props, jhtml_image_methods,
    				WEBC_NULL, WEBC_NULL);
}
#endif //(WEBC_SUPPORT_JSCRIPT)
