/*
|  JHCOLL.CPP -
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
#include "jhcoll.h"
#if (WEBC_SUPPORT_JSCRIPT)
#include "jhimg.h"
#include "jhform.h"
#include "jhutils.h"
#include "hselect.hpp"
#include "hoption.hpp"
#include "helement.hpp"
#include "hform.hpp"
#include "hradio.hpp"
#include "htmldoc.hpp"
#include "hframe.hpp"
#include "htmlfind.hpp"
#include "wjscript.hpp"
#include "webcstr.h"
#include "objref.cpp"
#include "webcmem.h"
#include "jhoption.h"
#include "wjsdebug.h"

//forward declarations
static int _matchAll(HTMLElement *p_elem, WEBC_PFBYTE b1);
static int _matchByName(HTMLElement *p_elem, WEBC_PFBYTE p_name);
static int _matchByNameOrId(HTMLElement *p_elem, WEBC_PFBYTE p_name);

static int _matchInputByNameOrId(HTMLElement *p_elem, WEBC_PFBYTE p_name);
static int _matchInput(HTMLElement *p_elem, WEBC_PFBYTE b1);

JSBool jhtml_collection_namedItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jhtml_collection_item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
#define INCLUDE_SELF_NODES_DEFAULT WEBC_FALSE	  // When FALSE NODES search does not include self if self matched
#define INCLUDE_SELF_ELEMENT_DEFAULT WEBC_FALSE	  // When FALSE other searches do not include self if self matched

/**
 * This is the JS implementation of the HTMLCollection defined in the DOM
 *
 * WebC has no HTMLCollection class associated with it, therefore we
 * must implement this class in a different manner.  We have defined a
 * struct called jhtml_collection that holds information regarding the
 * type of collection that is associated with it.  Whenever someone
 * uses the collection, the type pf collection it is defines how
 * information is used.  Usually this is done by HTMLElementFinders.
*/
//set the unique ids for the properties of the collection class
enum jhtml_collection_tinyid
{
	HTMLCOLLECTION_LENGTH = -1
};


static JSBool jhtml_collection_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!jsmgr)
	{
		return JS_FALSE;
	}

	HTMLDocument *pDoc = jsmgr->GetDocument();
	jhtml_collection *pColl = (jhtml_collection *)jhutil_GetPrivate(cx, obj);

	if (!pColl)
	{
		return JS_TRUE;
	}

	JSObject *jvalobj = JSVAL_IS_OBJECT(*vp)? JSVAL_TO_OBJECT(*vp) : 0;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLCOLLECTION_LENGTH:
			switch(pColl->finderType)
			{
				case SELECT_OPTIONS:
				{
					HTMLSelect *pSelect = (HTMLSelect *) pColl->pTop;
					if (JSVAL_IS_INT(*vp))
					{
						pSelect->SetNumOptions(UTIL_SAFE_LONG_TO_SHORT(JSVAL_TO_INT(*vp)));
					}
				}
				break;

				default:
					break;
			}
			break;

		default:
			switch(pColl->finderType)
			{
				case SELECT_OPTIONS:
				{
					HTMLOption *pOption = 0;
					HTMLSelect *pSelect = (HTMLSelect *) pColl->pTop;

					if (jvalobj)
					{
						// check to make sure this object is the right class
						if (JS_GetClass(jvalobj) == getHtmlOptionElement())
						{
							HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, jvalobj);
							pOption = (pElem)? (HTMLOption *) pElem : 0;
						}
					}

					if (pOption)
					{
						if (pColl->pTop && pColl->pTop->Type() == HTML_SELECT_ELEMENT)
						{
							HTMLOption *pNextOption = pSelect->GetOptionIndex(JSVAL_TO_INT(id));

							if (pNextOption != pOption)
							{
								if (pOption->mpParent)
								{
									pOption->mpParent->Remove(pOption);
								}
								if (pNextOption)
								{
									pNextOption->mpParent->InsertBefore(pOption, pNextOption);
									pNextOption->mpParent->Remove(pNextOption);
									WEBC_DELETE(pNextOption);
								}
								else
								{
									pSelect->InsertFirst(pOption);
								}
							}
						}
						break;
					}
					else // this means we're setting the object to null
					{
						HTMLOption *pNullOption = pSelect->GetOptionIndex(JSVAL_TO_INT(id));
						if (pNullOption)
						{
							pNullOption->mpParent->Remove(pNullOption);
							if (!jsmgr->AddNewElement(pNullOption))
							{
								WEBC_DELETE(pNullOption);
							}
						}
					}
				}
				break;

				default:
					break;
			}
			break;
	}

	return JS_TRUE;
}


/**
 * Call this method when getting a Property from HTMLCollection
 */
static JSBool jhtml_collection_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!jsmgr)
	{
		return JS_FALSE;
	}

	HTMLDocument *pDoc = jsmgr->GetDocument();
	jhtml_collection *pColl = (jhtml_collection *)jhutil_GetPrivate(cx, obj);

	if (!pColl)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		//if we are looking for the element length
		case HTMLCOLLECTION_LENGTH:
		{
			if (pColl->finderType)
			{
				HTMLElementType ele_type = HTML_ELEMENT_NONE;
				//switch on the type of collection this is
				switch(pColl->finderType)
				{
					//these cases can use the HTMLCountByType Finder, and therefore
					//just set the ele_type and break
					case DOCUMENT_IMAGES:
						ele_type = HTML_IMAGE_ELEMENT;
						break;
					case DOCUMENT_ANCHORS:
						ele_type = HTML_ANCHOR_ELEMENT;
						break;
					case TABLE_ROWS:
						ele_type = HTML_TABLE_ROW_ELEMENT;
						break;
					case TABLEROW_CELLS:
						ele_type = HTML_TABLE_CELL_ELEMENT;
						break;
					case MAP_AREAS:
						ele_type = HTML_AREA_ELEMENT;
						break;
					case SELECT_OPTIONS:
						ele_type = HTML_OPTION_ELEMENT;
						break;
					case WIN_FRAMES:
						ele_type = HTML_FRAME_ELEMENT;
						break;
					case ELEMENT_NODES:
					{
						HTMLCountAllFinder finder;
						pColl->pTop->FindElement(&finder, 1, INCLUDE_SELF_NODES_DEFAULT);
						int count = finder.Length();
						*vp = INT_TO_JSVAL(count);
						return JS_TRUE;
					}
					case DOCUMENT_FORMS:
					{
						WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
						WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
						//forms do not live in the document tree, therefore we must use
						//the form vector
						if (jsmgr)
						{
							HTMLElementTypeFinder f(HTML_FORM_ELEMENT);
							HTMLElementCounter fl(&f);
							jsmgr->GetDocument()->FindElement(&fl);
							int len = fl.Count();
							*vp = INT_TO_JSVAL(len);
							return JS_TRUE;
						}
						return JS_FALSE;
					}
					case DOCUMENT_IDS:
					{
						HTMLCountAllFinder finder;
						pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_ELEMENT_DEFAULT);
						int count = finder.Length();
						*vp = INT_TO_JSVAL(count);
						return JS_TRUE;
					}
					case DOCUMENT_ALL:
					{
						HTMLCountAllFinder finder;
						pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_ELEMENT_DEFAULT);
						int count = finder.Length();
						*vp = INT_TO_JSVAL(count);
						return JS_TRUE;
					}
					case ALL_BY_NAME:
					{
						if (!pColl->nameOfAll)
						{
							*vp = INT_TO_JSVAL(0);
							return JS_TRUE;
						}
						//this finder counts the total number of elements with the
						//name given by nameOfAll
						HTMLCountByNameFinder finder(pColl->nameOfAll);
						pColl->pTop->FindElement(&finder, 1, INCLUDE_SELF_ELEMENT_DEFAULT);
						int count = finder.Length();
						*vp = INT_TO_JSVAL(count);
						return JS_TRUE;
					}
					case ALL_TAGS_BY_TAGNAME:
					{
						if (!pColl->nameOfAll)
						{
							*vp = INT_TO_JSVAL(0);
							return JS_TRUE;
						}
						// use this finder to count the total number of elements with the
						// type given by nameOfAll (misnomer)
						HTMLTagType hType = HTML_ParseTagType(pColl->nameOfAll, webc_strlen(pColl->nameOfAll));
						HTMLElementType eType = TagToHTMLElementType[hType];
						HTMLCountByTypeFinder finder(eType);
						pColl->pTop->FindElement(&finder, 1, WEBC_FALSE); // don't include myself in the search results
						int count = finder.Length();
						*vp = INT_TO_JSVAL(count);
						return JS_TRUE;
					}
					case FORM_INPUTS:
					{
						//for this type of collection, the top will always be of type form
						HTMLForm *form = (HTMLForm *)pColl->pTop;
						int len = vector_get_size(form->GetFieldVector());
						*vp = INT_TO_JSVAL(len);
						return JS_TRUE;
					}
					case DOCUMENT_STYLESHEETS:
					{
						//GMP this needs to be revisited if CSS is changed to have more than 1 style sheet
						*vp = INT_TO_JSVAL(1);
						return JS_TRUE;
					}
					case SSHEET_RULES:
					{
#if (WEBC_SUPPORT_STYLE_SHEETS)
						CSSDocumentContext *pCSSCx= pDoc->GetCSSContext();
	/*					int i = 0;
						if (pCSSCx)
						{
							vector_iterator pvi[1];
							CSSPropertyDescriptor * pCSSPD = pCSSCx->EnumFirstProperty(pvi);
							while (pCSSPD)
							{
								i++;
								pCSSPD = pCSSCx->EnumNextProperty(pvi);
							}
						}
						*vp = INT_TO_JSVAL(i);*/
#endif //(WEBC_SUPPORT_STYLE_SHEETS)
						return JS_TRUE;
					}

					case RADIO_BUTTONS:
					{
						int numRadioElems = 0;
						HTMLRadioButton *pRadio = (HTMLRadioButton*)pColl->pTop;
						while (pRadio)
						{
							numRadioElems++;
							pRadio = (pRadio->mpGroupNext != pColl->pTop)
								? pRadio->mpGroupNext : 0;
						}
						*vp = INT_TO_JSVAL(numRadioElems);
						return JS_TRUE;
					}

				}//end inner switch
				if (ele_type != HTML_ELEMENT_NONE)
				{
					HTMLCountByTypeFinder finder(ele_type);
					pColl->pTop->FindElement(&finder, 1, INCLUDE_SELF_ELEMENT_DEFAULT);
					*vp = INT_TO_JSVAL(finder.Length());
				}
			}//end if
			return JS_TRUE;
		}//end case length
		default:
		{
			//MSIE allows collections to be index like 'document.all.nameofelement' where
			//nameofelement is an element name in the document tree.  It also allows
			//'document.all[4]' To account for that we call item on the element passed in.
			//which will call namedItem if the jsval passed is not an int.
			jhtml_collection_item(cx, obj, 1, &id, vp);
			break;
		}
	}//end switch
	return JS_TRUE;
}

JSBool jhtml_collection_resolve(JSContext *cx, JSObject *obj, jsval id)
{
    return(jutil_resolve(cx, obj, id, "Collection"));
}

/**
 * This JSClass is the javascript wrapper for the HTMLCollection class defined in the DOM Level 1
 */
JSClass JSHtmlCollection =
{
	"Collection", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_collection_getProperty,
	(JSPropertyOp) jhtml_collection_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_collection_resolve , (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};




JSClass *getHtmlCollection()
{
	return &JSHtmlCollection;
}

/**
 * HTMLCollection namedItem(long) method
 *
 * param - name - the name of the element to get
 * return - the first htmlelement in the tree with the given name
 */
JSBool jhtml_collection_namedItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!jsmgr)
	{
		return JS_FALSE;
	}
	HTMLDocument *pDoc = jsmgr->GetDocument();
	if (!pDoc)
	{
		return JS_FALSE;
	}
	HTMLElementType ele_type = HTML_ELEMENT_NONE;
	//if we have atleast one argument and it is a string
	if (argc > 0 && JSVAL_IS_STRINGABLE(argv[0]))
	{
		//conver the JSString to char *
		WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));
		if (!name || !(*name))
		{
			*rval = JSVAL_NULL;
			return JS_TRUE;;
		}

		if (webc_c_stricmp(name, "toString")==0)
		{
				return JS_TRUE;
		}

		jhtml_collection *pColl = (jhtml_collection*)jhutil_GetPrivate(cx, obj);
		if (!pColl)
			return JS_TRUE;

		HTMLElement *pElem = 0;
		switch(pColl->finderType)
		{
			case DOCUMENT_IMAGES:
				ele_type = HTML_IMAGE_ELEMENT;
				break;

			case DOCUMENT_ANCHORS:
				ele_type = HTML_ANCHOR_ELEMENT;
				break;

			case TABLE_ROWS:
				ele_type = HTML_TABLE_ROW_ELEMENT;
				break;

			case TABLEROW_CELLS:
				ele_type = HTML_TABLE_CELL_ELEMENT;
				break;

			case MAP_AREAS:
				ele_type = HTML_AREA_ELEMENT;
				break;

			case SELECT_OPTIONS:
				ele_type = HTML_OPTION_ELEMENT;
				break;

			case WIN_FRAMES:
			{
			  #if (WEBC_SUPPORT_FRAMES)
				HTMLTypeAndNameOrIdFinder finder(name, HTML_FRAME_ELEMENT);
				pElem = pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_ELEMENT_DEFAULT);
				if (pElem)
				{
					HTMLFrame *pFrame = (HTMLFrame *) pElem;
					if (pFrame->FrameDocument() && pFrame->FrameDocument()->GetJSMgr())
					{
						*rval = OBJECT_TO_JSVAL(pFrame->FrameDocument()->GetJSMgr()->GetGlobalObject());
						return JS_TRUE;
					}
				}
				*rval = JSVAL_NULL;
			  #endif // WEBC_SUPPORT_FRAMES

				return JS_TRUE;
			}

			case DOCUMENT_FORMS:
			{
				WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
				WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
				if (jsmgr)
				{
					HTMLNameOrIdFinder nameFinder(name);
					HTMLElementTypeFinder formFinder(HTML_FORM_ELEMENT);
					HTMLElementAndFinder formNameFinder(&nameFinder, &formFinder);

					HTMLDocument *pDoc = jsmgr->GetDocument();
					pElem = pDoc->FindElement(&formNameFinder);
				}
				break;
			}

			case DOCUMENT_ALL:
			{
				HTMLNameOrIdFinder  finder(name);
				pElem = pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_ELEMENT_DEFAULT);
				break;
			}
			case DOCUMENT_IDS:
			{
				HTMLNameOrIdFinder  finder(name);
				pElem = pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_ELEMENT_DEFAULT);
				break;
			}

			case ALL_BY_NAME:
			{
				HTMLElementByNameFinder finder(name);
				pElem = pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_ELEMENT_DEFAULT);
				break;
			}

			case ALL_TAGS_BY_TAGNAME:
			{
				if(pColl && pColl->nameOfAll)
				{
					HTMLTagType hType = HTML_ParseTagType(pColl->nameOfAll, webc_strlen(pColl->nameOfAll));
					HTMLElementType eType = TagToHTMLElementType[hType];

					HTMLNameTypeFinder finder(name, eType);
					// TODO this should probably restrict to a specific tag name
					pElem = pColl->pTop->FindElement(&finder, 1, WEBC_FALSE);
				}
				break;
			}

			case ELEMENT_NODES:
			{
				HTMLNameOrIdFinder finder(name);
				pElem = pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_NODES_DEFAULT);
				break;
			}

			case FORM_INPUTS:
			{
				//for this type of collection, the top will always be of type form
				HTMLForm *form = (HTMLForm *)pColl->pTop;
				if (form && name && *name)
				{
					pElem = jutils_CheckObjectList(form->GetFieldVector(), _matchInputByNameOrId, (WEBC_PFBYTE)name);
				}//end if
				break;
			}

			case DOCUMENT_STYLESHEETS:
			{
#if (WEBC_SUPPORT_STYLE_SHEETS)
				//GMP this needs to be revisited if CSS is changed to have more than 1 style sheet
#endif
				break;
			}

			case SSHEET_RULES:
			{
#if (WEBC_SUPPORT_STYLE_SHEETS)
				CSSDocumentContext *pCSSCx= pDoc->GetCSSContext();
	/*			if (pCSSCx)
				{
					int i = 0;
					vector_iterator pvi[1];
					CSSPropertyDescriptor * pCSSPD = pCSSCx->EnumFirstProperty(pvi);
					while (pCSSPD)
					{
						char *cName = pCSSPD->EnumFirstClass();
						while (cName)
						{
							if (tc_stricmp(cName, name) == 0)
							{
								*rval = OBJECT_TO_JSVAL(pCSSPD->GetJSObject());
								return JS_TRUE;
							}
							pCSSPD->EnumNextClass();
						}
						pCSSPD = pCSSCx->EnumNextProperty(pvi);
					}
				}*/
#endif //(WEBC_SUPPORT_STYLE_SHEETS)
				return JS_TRUE;
			}

			case RADIO_BUTTONS:
			{
				HTMLRadioButton *pRadio = (HTMLRadioButton *) pColl->pTop;

				if (name)
				{
					while (pRadio && pRadio->Value() && webc_stricmp(name, pRadio->Value()))
					{
						pRadio = (pRadio->mpGroupNext != pColl->pTop)? pRadio->mpGroupNext : 0;
					}
				}

				if (pRadio)
				{
					*rval = OBJECT_TO_JSVAL(pRadio->CreateJSObject());
					return JS_TRUE;
				}

				break;
			}

			default:
				break;
		}//end switch

		if (ele_type != HTML_ELEMENT_NONE)
		{
			HTMLTypeAndNameOrIdFinder finder(name, ele_type);
			pElem = pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_ELEMENT_DEFAULT);
		}

		if (pElem)
		{
			*rval = OBJECT_TO_JSVAL(pElem->CreateJSObject());
		}

		return JS_TRUE;
	}//end if

	*rval = JSVAL_NULL;

	return JS_TRUE;
}

/**
 * HTMLCollection item(long) method
 *
 * param  - index - the index of the element to get
 * return - the HTMLElement at index
 */
JSBool jhtml_collection_item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int index;
	int32 dIndex = 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!jsmgr)
	{
		return JS_FALSE;
	}
	HTMLDocument *pDoc = jsmgr->GetDocument();
	if (!pDoc)
	{
		return JS_FALSE;
	}

	//if we have at least 1 argument and it is a number
	if (argc > 0 && JSVAL_IS_INT(argv[0]))
	{
		//convert the jsval to a number
		JS_ValueToInt32(cx, argv[0], &dIndex);
		index = (int)dIndex;
		HTMLElementType ele_type = HTML_ELEMENT_NONE;
		jhtml_collection *pColl = (jhtml_collection*)jhutil_GetPrivate(cx, obj);
		if (!pColl)
		{
			return JS_FALSE;
		}
		HTMLElement *pElem = 0;
		switch(pColl->finderType)
		{
			case DOCUMENT_IMAGES:
				ele_type = HTML_IMAGE_ELEMENT;
				break;

			case DOCUMENT_ANCHORS:
				ele_type = HTML_ANCHOR_ELEMENT;
				break;

			case TABLE_ROWS:
				ele_type = HTML_TABLE_ROW_ELEMENT;
				break;

			case TABLEROW_CELLS:
				ele_type = HTML_TABLE_CELL_ELEMENT;
				break;

			case MAP_AREAS:
				ele_type = HTML_AREA_ELEMENT;
				break;

			case SELECT_OPTIONS:
				ele_type = HTML_OPTION_ELEMENT;
				break;

			case WIN_FRAMES:
			{
				*rval = JSVAL_NULL;
			  #if (WEBC_SUPPORT_FRAMES)
				HTMLNthOfTypeFinder finder(HTML_FRAME_ELEMENT, index);
				pElem = pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_ELEMENT_DEFAULT);
				if (pElem)
				{
					HTMLFrame *pFrame = (HTMLFrame *) pElem;
					if (pFrame->FrameDocument() && pFrame->FrameDocument()->GetJSMgr())
					{
						*rval = OBJECT_TO_JSVAL(pFrame->FrameDocument()->GetJSMgr()->GetGlobalObject());
					}
				}
			  #endif // WEBC_SUPPORT_FRAMES
				return JS_TRUE;
			}

			case ELEMENT_NODES:
			{
				HTMLNthOfAnyTypeFinder finder(index);
				pElem = pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_NODES_DEFAULT);
				if (pElem)
				{
					*rval = OBJECT_TO_JSVAL(pElem->CreateJSObject());
				}
				else
				{
					*rval = JSVAL_NULL;
				}
				return JS_TRUE;
			}

			case DOCUMENT_FORMS:
			{
				HTMLElementTypeFinder formFinder(HTML_FORM_ELEMENT);
				HTMLNthElementFinder nthFormFinder(&formFinder, index);

				pElem = pDoc->FindElement(&nthFormFinder);

				if (pElem)
				{
					*rval = OBJECT_TO_JSVAL(pElem->CreateJSObject());
				}
				else
				{
					*rval = JSVAL_NULL;
				}
				return JS_TRUE;
			}

            case DOCUMENT_IDS:
			{
				HTMLNthOfAnyTypeFinder finder(index);
				pElem = pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_ELEMENT_DEFAULT);
				if (pElem)
				{
					*rval = OBJECT_TO_JSVAL(pElem->CreateJSObject());
				}
				else
				{
					*rval = JSVAL_NULL;
				}
				return JS_TRUE;
			}

			case DOCUMENT_ALL:
			{
				HTMLNthOfAnyTypeFinder finder(index);
				pElem = pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_ELEMENT_DEFAULT);
				if (pElem)
				{
					*rval = OBJECT_TO_JSVAL(pElem->CreateJSObject());
				}
				else
				{
					*rval = JSVAL_NULL;
				}
				return JS_TRUE;
			}

			case ALL_BY_NAME:
			{
				//get the nth item with name...
				HTMLNthByNameFinder finder(pColl->nameOfAll, index);
				pElem = pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_ELEMENT_DEFAULT);

				if (pElem)
				{
					*rval = OBJECT_TO_JSVAL(pElem->CreateJSObject());
				}
				else
				{
					*rval = JSVAL_NULL;
				}

				return JS_TRUE;
			}

			case ALL_TAGS_BY_TAGNAME:
			{
				//get the nth item with the specified tag name...
				HTMLTagType hType = HTML_ParseTagType(pColl->nameOfAll, webc_strlen(pColl->nameOfAll));
				HTMLElementType eType = TagToHTMLElementType[hType];
				HTMLNthOfTypeFinder finder(eType, index);
				pElem = pColl->pTop->FindElement(&finder, 1, WEBC_FALSE);

				if (pElem)
				{
					*rval = OBJECT_TO_JSVAL(pElem->CreateJSObject());
				}
				else
				{
					*rval = JSVAL_NULL;
				}

				return JS_TRUE;
			}

			case FORM_INPUTS:
			{
				//for this type of collection, the top will always be of type form
				HTMLForm *form = (HTMLForm *)pColl->pTop;
				if (form)
				{
					pElem = jutils_GetNthOfObjectList(form->GetFieldVector(),
					                                  index,
					                                  _matchInput,
					                                  0);

					if (pElem)
					{
						*rval = OBJECT_TO_JSVAL(pElem->CreateJSObject());
					}
					else
					{
						*rval = JSVAL_NULL;
					}

					return JS_TRUE;
				}
				break;
			}

			case DOCUMENT_STYLESHEETS:
			{
#if (WEBC_SUPPORT_STYLE_SHEETS)
				//GMP this needs to be revisited if CSS is changed to have more than 1 style sheet
				*rval = OBJECT_TO_JSVAL(pDoc->GetCSSContext()->GetJSObject());
#else
				*rval = JSVAL_NULL;
#endif
				return JS_TRUE;
			}

			case SSHEET_RULES:
			{
#if (WEBC_SUPPORT_STYLE_SHEETS)
				/*int i = 0;
				CSSDocumentContext *pCSSCx = pDoc->GetCSSContext();
				if (pCSSCx)
				{
					vector_iterator pvi[1];
					CSSPropertyDescriptor *pCSSPD = pCSSCx->EnumFirstProperty(pvi);
					while(pCSSPD)
					{
						if ( i == index)
						{
							*rval = OBJECT_TO_JSVAL(pCSSPD->GetJSObject());
							return JS_TRUE;
						}
						i++;
						pCSSPD = pCSSCx->EnumNextProperty(pvi);
					}
				}*/
#endif // (WEBC_SUPPORT_STYLE_SHEETS)
				return JS_TRUE;
			}

			case RADIO_BUTTONS:
			{
				HTMLRadioButton *pRadio = (HTMLRadioButton*)pColl->pTop;
				while (index > 0)
				{
					index--;
					pRadio = (pRadio->mpGroupNext != pColl->pTop)
						? pRadio->mpGroupNext : 0;
				}

				if (pRadio)
				{
					*rval = OBJECT_TO_JSVAL(pRadio->CreateJSObject());
					return JS_TRUE;
				}
			}
			break;

		}//end switch

		if (ele_type != HTML_ELEMENT_NONE)
		{
			HTMLNthOfTypeFinder finder(ele_type, index);
			pElem = pColl->pTop->FindElement(&finder,1, INCLUDE_SELF_ELEMENT_DEFAULT);
		}

		if (pElem)
		{
			*rval = OBJECT_TO_JSVAL(pElem->CreateJSObject());
		}
		else
		{
			*rval = JSVAL_NULL;
		}

		return JS_TRUE;
	}//end if
	else if (argc > 0 && JSVAL_IS_STRING(argv[0]))
	{
		//MSIE allows for the item method to take a srting that corresponds to the name
		//therefore we call namedItem instead
		return jhtml_collection_namedItem(cx, obj, argc, argv, rval);
	}
	*rval = JSVAL_NULL;
	return JS_TRUE;
}

/**
 * HTMLCollection tags(string) method
 *
 * param  - sTag Required. Variant of type String that specifies an HTML tag. It can be any
 *          one of the objects exposed by the DHTML Object Model.
 * return - the HTMLCollection of all the elements with the specified tags
 */

JSBool jhtml_collection_tags(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int32 dIndex = 0;
	jhtml_collection *pColl = (jhtml_collection *)jhutil_GetPrivate(cx, obj);
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!jsmgr)
	{
		return JS_FALSE;
	}
	HTMLDocument *pDoc = jsmgr->GetDocument();
	if (!pDoc)
	{
		return JS_FALSE;
	}

	HTMLElement *pElem  = pColl->pTop;
	if (!pElem)
	{
		return JS_FALSE;
	}

	//if we have at least 1 argument and it is a string
	if (argc > 0 && JSVAL_IS_STRING(argv[0]))
	{
		WEBC_CHAR *tagName = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));

		vector_iterator pvi[1];
		jhtml_collection *coll = (jhtml_collection *)
			vector_get_first(jsmgr->GetCollections(), pvi);
		// first look at all the collections in the jsmgr and find out if we already created
		// a collection for this tag name
		for (; coll != 0; coll = (jhtml_collection *)vector_get_next(pvi))
		{
			if (coll->finderType == ALL_TAGS_BY_TAGNAME && coll->nameOfAll && webc_stricmp(coll->nameOfAll, tagName))
			{
				//if we have, just return that, no need to allocate new memory
				*rval = OBJECT_TO_JSVAL(coll->jsObj.Get());
				return JS_TRUE;
			}
		}//end for

		// otherwise, return a new collection
		coll = jhtml_collection_New(jsmgr, obj, "tags", pElem, ALL_TAGS_BY_TAGNAME);
		if (coll)
		{
			coll->nameOfAll = webc_malloc_string_copy(tagName, __FILE__, __LINE__);
			*rval = OBJECT_TO_JSVAL(coll->jsObj.Get());
			return JS_TRUE;
		}
	}

    return JS_TRUE;
}

JSBool jhtml_collection_toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = STRING_TO_JSVAL(JS_InternString(cx, "[object HTMLCollection]"));
	return JS_TRUE;
}
/**
 * The properties for the HTMLCollection
 */
JSPropertySpec jhtml_collection_props[] = {
    {"length",     HTMLCOLLECTION_LENGTH,    JSPROP_ENUMERATE},
    {0}
};

/**
 * The methods for the HTMLCollection
 */
JSFunctionSpec jhtml_collection_methods[] =
{
	{"item",       (JSNative) jhtml_collection_item,               1},
  	{"namedItem",  (JSNative) jhtml_collection_namedItem,          1},
	{"toString",   (JSNative) jhtml_collection_toString,           1},
	{"tags",       (JSNative) jhtml_collection_tags,           1},
  	{0}
};


/**
 * The constructor for an HTMLCollection
 */
static JSBool JHTMLCollection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return JS_TRUE;
}

/**
 * This method creates a new jhtml_collecion struct, and a new HTMLCollection
 * JSObject.  It adds the new struct to the Collections vector in the jscriptmanager
 * so that is can be deleted.
 */
jhtml_collection *jhtml_collection_New(WebcJSDocumentContext *jsmgr, JSObject *parent,
						const char *name, HTMLElement *top, CollectionType type)
{
	jhtml_collection *stored_coll = 0;
	JSContext *cx = jsmgr->GetContext();
	vector_iterator pvi[1];

	jhtml_collection dummy;
	if (vector_add_node_to_front(jsmgr->GetCollections(), (WEBC_PFBYTE) &dummy) < 0)
	{
		return (0);
	}
	stored_coll = (jhtml_collection*) vector_get_first(jsmgr->GetCollections(), pvi);

	if (stored_coll)
	{
		//call define object to create an instance of the HTMLCollection
		JSObject *new_obj = JS_DefineObject(cx, parent, name, getHtmlCollection(), 0, 0);

		//set the typr of finder this is
		stored_coll->finderType = type;

		//set the top Element to use when calling findElement
		stored_coll->pTop = top;

		//root the new property object so it won't get garbage collected
		stored_coll->jsObj.Set(new_obj);

		// tbd - is this ever used?
		stored_coll->nameOfAll = 0;

		//set the private pointer of the JSObject to the jhtml_collection struct
		jhutil_SetPrivate(cx, new_obj, stored_coll);
	}

	return (stored_coll);

}

static int _matchByName(HTMLElement * p_elem, WEBC_PFBYTE p_name)
{
	WEBC_CHAR * pName = (WEBC_CHAR *) p_name;

	if (pName && p_elem->Name() && webc_stricmp(p_elem->Name(), pName) == 0)
		return 1;
	return 0;
}

static int _matchByNameOrId(HTMLElement *p_elem, WEBC_PFBYTE p_name)
{
	return (p_elem->IsNameOrId((WEBC_CHAR *) p_name));
}

static int _matchAll(HTMLElement *pElem, WEBC_PFBYTE b1)
{
	return 1;
}

static int _matchInputByNameOrId(HTMLElement *p_elem, WEBC_PFBYTE p_name)
{
	if (p_elem->IsInput())
	{
		return _matchByNameOrId(p_elem, p_name);
	}

	return 0;
}

static int _matchInput(HTMLElement *p_elem, WEBC_PFBYTE b1)
{
	if (p_elem->IsInput())
	{
		return 1;
	}

	return 0;
}

/**
 * Initialize the HTMLCollection class
 */
JSObject *jhtml_collection_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLCollection Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlCollection,
    				(JSNative) JHTMLCollection, 0,
    				jhtml_collection_props, jhtml_collection_methods,
    				WEBC_NULL, WEBC_NULL);
}
#endif //(WEBC_SUPPORT_JSCRIPT)

