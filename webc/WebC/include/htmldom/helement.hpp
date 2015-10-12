#ifndef __HELEMENT_HPP__
#define __HELEMENT_HPP__

#include "css.h"
#include "textunit.hpp"
#include "htmldom.hpp"
#include "ref.hpp"
#include "webcinline.h"
#include "webfonts.hpp"

/*************************************************************************
* class HTMLElement
*
*  The base class for all nodes in the HTMLDocument tree.
*
* Public Methods:
*
*   DeleteChildren - detach all children of this element from the tree and
*     free the memory they are using.
*   InsertFirst - attach an HTMLElement to this one as the first child.
*   InsertAfter - attach an HTMLElement to this one immediately after the
*     specified child (pRef).
*   InsertLast - attach an HTMLElement to this one as the last child.
*   Remove - detach a child element from this one.
*   Root - returns the root element of this document tree.
*   GetDocument - returns the HTMLDocument object for this document tree.
*   FindParentByType - returns the most immediate parent of this object
*     with a particular type.
*   FindElement - do a depth-first search and return the first element
*     on which the given HTMLElementFinder::Check function returns non-zero.
*   Event - process an event which has occurred to this element.  See
*     the WEBC HTML DOM Event Model description above.
*   Refresh - redraw this element only.
*
*************************************************************************/

#if (WEBC_SUPPORT_JSCRIPT)
class WebcJSEventHandler;
class WebcJSDocumentContext;
#endif

class HTMLElementFinder;
class HTMLElementContainer;
class ITextDisplayStyle;

#define INVALID_TABINDEX   (-1)
#define MAX_TABINDEX       32767L
#define TABINDEX_TO_INT(X) (((X)==0)? (long) (MAX_TABINDEX+1L) : (long) (X))
#define TABINDEX_CMP(X,Y)  ((long) (TABINDEX_TO_INT(X) - TABINDEX_TO_INT(Y)))


// HTMLElement flags - bits 0-7 are reserved for HTMLElement use
//  but bits 8-15 (UNUSED_x) can be redefined by a particular class
//  and used for any purpose

typedef WEBC_UINT32 HELEM_FLAGS;

#define HELEM_INLINE WEBC_INLINE

#define HELEM_FLAG_FONT_CACHED          0x00000001
#define HELEM_FLAG_FONT_FAMILY_CACHED   0x00000002
#define HELEM_FLAG_FONT_SIZE_CACHED     0x00000004
#define HELEM_FLAG_FONT_WEIGHT_CACHED   0x00000008
#define HELEM_FLAG_FONT_STYLE_CACHED    0x00000010
#define HELEM_FLAG_HIDDEN               0x00000020
#define HELEM_FLAG_USERCOLOR            0x00000040
#define HELEM_FLAG_USERBGCOLOR          0x00000080
#define HELEM_FLAG_USERPOSITION         0x00000100
#define HELEM_FLAG_USERVISIBILITY       0x00000200
#define HELEM_FLAG_UNIT_TYPE_CACHED     0x00000400
#define HELEM_FLAG_VISIBILITY_CACHED    0x00000800
#define HELEM_FLAG_LOADING_IMAGE        0x00001000
#define HELEM_FLAG_HAS_FOCUS            0x00002000
#define HELEM_FLAG_PARSING_CONTENT      0x00004000
#define HELEM_FLAG_HAS_INLINE_STYLE     0x00008000
#define HELEM_FLAG_UNUSED_0             0x00010000
#define HELEM_FLAG_UNUSED_1             0x00020000
#define HELEM_FLAG_UNUSED_2             0x00040000
#define HELEM_FLAG_UNUSED_3             0x00080000
#define HELEM_FLAG_UNUSED_4             0x00100000
#define HELEM_FLAG_UNUSED_5             0x00200000
#define HELEM_FLAG_UNUSED_6             0x00400000
#define HELEM_FLAG_UNUSED_7             0x00800000
#define HELEM_FLAG_UNUSED_8             0x01000000
#define HELEM_FLAG_UNUSED_9             0x02000000
#define HELEM_FLAG_UNUSED_10            0x04000000
#define HELEM_FLAG_UNUSED_11            0x08000000
#define HELEM_FLAG_UNUSED_12            0x10000000
#define HELEM_FLAG_UNUSED_13            0x20000000
#define HELEM_FLAG_UNUSED_14            0x40000000
#define HELEM_FLAG_UNUSED_15            0x80000000

// helement.cpp
class HTMLElement :
	  public TextUnit
	, public Referable
#if (WEBC_SUPPORT_STYLE_SHEETS)
	, public CSSStyleElement
#endif
{
protected:
	static WEBC_UINT8 mbDepth;

	// HTMLElement Data for TextUnit methods
	WEBC_CHAR *mpId; // ID attribute
	WEBC_CHAR *mpTitle; // TITLE attribute
	WEBC_CHAR *mpTextDirection; // DIR attribute
	WEBC_CHAR *mpLanguage; // LANG attribute
	vector mClassList;			// vector of class pointers
	ObjectReference<HTMLDocument> mDocRef;
	HTMLTagType mTag;
	WebcFontDescriptor mFontDesc;
	WebFontRef mFont;
	WEBC_UINT8 mTextUnitType;
  #if (WEBC_SUPPORT_STYLE_SHEETS)
	const WEBC_CHAR *mpIdLowercase;
  #endif
	vector mPrivateDataStack;
	void* mPrivateData;

  #if (WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION)
	HTMLElement *mpFocusBoxElement;
	int miFocusBoxIndex;
  #endif

  #if (WEBC_SUPPORT_USER_EVENTS)
	// tbd - join these together in a single struct and point to that to save some space
	HTMLEventHandler* mpUserEventHandler;
	WEBC_CHAR*        mpUserParam;
    HTMLEventHandler* mpWebCElementEventHandler;
	vector mHandlerVectorStack;
  #endif

  #if (WEBC_SUPPORT_JSCRIPT)
	vector mJSHandlerVector;
	JSObjectRef mJSObject;
  #endif

	ObjectReference<HTMLForm> mForm;
  #if (WEBC_SUPPORT_INNER_HTML)
	WEBC_CHAR* mpTagAttributes;
  #endif // WEBC_SUPPORT_INNER_HTML

	WEBC_UINT16       FindVisibility (void);

	WebcFontDescriptor GetFontDescriptor (
			CSSPropertyValue* family,
			CSSPropertyValue* size,
			CSSPropertyValue* style,
			CSSPropertyValue* weight,
			CSSPropertyValue* variant,
			WebcFontDescriptor baseDesc
		);

	int GetFontProperty (
			WebcFontDescriptor desc,
			CSSPropertyType property,
			CSSPropertyValue* value
		);

	WebGraphFont* GetWebGraphFont (WebcFontDescriptor desc);
	WebGraphFont* GetDefaultFont (void);

	HTMLEventStatus JSEvent_Internal(HTMLEvent *pEvent, int n);
	HTMLBrowser * MakeNewTargetWindow(HTMLDocument *pDoc, char *pName, char *pURL=NULL);

public:
	HELEM_FLAGS  mFlags;

#ifdef WEBC_DEBUG
	void WEBC_DEBUG_ReceiveEvent(HTMLElement *TheReciever, HTMLEvent *TheEvent);
	void WEBC_DEBUG_ReceiveNotify(HTMLElement *TheReciever, Notifier *who, NotifyEvent *what);
#else
#undef WEBC_DEBUG_ReceiveEvent
#define WEBC_DEBUG_ReceiveEvent(x, y)
#undef WEBC_DEBUG_ReceiveNotify
#define WEBC_DEBUG_ReceiveNotify(x, y, z)
#endif

  #if (WEBC_SUPPORT_JSCRIPT)
	HTMLElement(HTMLTagParse *td = 0, HTMLDocument *pDocument = 0, JSObject *pJSObj = 0);
  #else
	HTMLElement(HTMLTagParse *td = 0, HTMLDocument *pDocument = 0);
  #endif // WEBC_SUPPORT_JSCRIPT

	// copy constructor
	HTMLElement(HTMLElement &copy);

	virtual ~HTMLElement(void);

	// Methods for adding/removing children
	virtual void DeleteChildren(void) {}
	virtual void InsertFirst(HTMLElement *pNode) {}
	virtual void InsertAfter(HTMLElement *pNode, HTMLElement *pRef) {}
	virtual void InsertBefore(HTMLElement *pNode, HTMLElement *pRef) {}
	virtual void InsertLast(HTMLElement *pNode) {}
	virtual WEBC_BOOL IsInserted() {	return (!( (0 == mpNext) && (0 == mpPrev) && (0 == mpParent) )); }
	virtual void Remove(HTMLElement *pNode) {}
	virtual void NotifyChildAdded(void) {}
	virtual void NotifyChildRemoved(void) {}

	virtual HTMLElement *FirstChild (void) { return 0; }
	virtual HTMLElement *LastChild (void) { return 0; }

	virtual HTMLElement *DocumentNext(void);

  #if (WEBC_SUPPORT_INNER_HTML)
	virtual void SetInnerHTML (WEBC_CHAR* innerHTML);
	virtual void SetOuterHTML (WEBC_CHAR* outerHTML);
	virtual void AppendInnerHTML (WebString* toString);
	virtual void AppendOuterHTML (WebString* toString);
	virtual void SetRenderedText(WEBC_CHAR* textVal);
	virtual void AppendRenderedText (WebString* toString);
#endif // WEBC_SUPPORT_INNER_HTML

	HTMLElement *Root(void);
	HTMLElement *Parent(void) { return mpParent; };
	HTMLDocument *GetFrameDocument(WEBC_CHAR *name);
	HTMLElement *FindParent(HTMLElementFinder* finder);
	HTMLEventStatus ProcessEvent(HTMLEvent *pEvent);
	HTMLEventStatus Event(HTMLEvent *pEvent);

	void  SetPrivateData (void* data) { mPrivateData = data; }
	void* GetPrivateData (void) { return mPrivateData; }
	void  PushPrivateData (void* data);
	void  PopPrivateData (void);
	void* GetFromPrivateDataStack(void);

  #if (WEBC_SUPPORT_USER_EVENTS)
	void SetEventHandler(HTMLEventHandler *pUserEventHandler);
	void SetEventHandlerName(WEBC_CHAR *pName);
	void SetElementEventHandler(HTMLEventHandler *pUserEventHandler);
	void PushElementEventHandler(HTMLEventHandler *pUserEventHandler);
	void PopElementEventHandler(void);
  #endif

  #if (WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION)
	void Focus (HTMLElement *focusBoxElement = 0, int focusBoxIndex = 0);
  #else
	void Focus (void);
  #endif
	void Unfocus (void);

	// Tab Navigation related methods
	HTMLElement *GetNextInTabOrder(WEBC_BOOL traverseFrames=WEBC_TRUE, WEBC_BOOL wrap=WEBC_TRUE);
	HTMLElement *GetPrevInTabOrder(WEBC_BOOL traverseFrames=WEBC_TRUE, WEBC_BOOL wrap=WEBC_TRUE);

  #if (WEBC_SUPPORT_SPATIAL_TAB_NAVIGATION)
	void FocusTabLeft(void);
	void FocusTabRight(void);
	void FocusTabUp(void);
	void FocusTabDown(void);
  #endif

	HTMLEventStatus DefaultInputKeyHandler(HTMLEvent *pEvent);

	//event methods
  #if (WEBC_SUPPORT_JSCRIPT)
	WebcJSEventHandler *AddJSEventHandler(HTMLEventType eventType, const char *handlerName, const WEBC_CHAR *source, long sourceLen, HTMLDocument *pDoc=0);
	WebcJSEventHandler *AddJSEventHandler(HTMLEventType eventType, const char *handlerName, JSFunction *pFunc, const WEBC_CHAR *functionText, HTMLDocument *pDoc=0);
	WebcJSEventHandler *FindJSEventHandler(HTMLEventType eventType, const WEBC_CHAR *functionText, int n=0);
	void RemoveJSEventHandler(HTMLEventType eventType, const WEBC_CHAR *functionText);
	void ClearJSEventHandlers(void);

	JSObject *GetJSObject() { return (mJSObject.Get()); }
	JSObject *CreateJSObject(void);
	void SetJSObject(JSObject *jsobj);
	WebcJSDocumentContext *GetJSMgr(void);
  #endif //WEBC_SUPPORT_JSCRIPT

	HTMLDocument *GetDocument(void);

	// Over-rideable HTMLElement methods
	virtual HTMLBrowser *GetBrowser(void);
	virtual HTMLElement *FindElement(HTMLElementFinder *pFinder, int iSearchFrames = 1, WEBC_BOOL bIncludeSelf=WEBC_TRUE);
	virtual HTMLEventStatus UserEvent(HTMLEvent *pEvent);
	virtual HTMLEventStatus JSEvent(HTMLEvent *pEvent);
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	virtual HTMLElement *EventParent (void);
	virtual void Reset(void);
	virtual void Select(void);

	// html attribute accessors
	virtual WEBC_UINT16 AccessKey (void) { return (0); }
	virtual WEBC_CHAR*  Alt       (void) { return (WEBC_NULL); }
	virtual WEBC_BOOL   Checked   (void) { return (WEBC_FALSE); }
	virtual WEBC_CHAR*  DefValue  (void) { return (WEBC_NULL); }
	virtual WEBC_BOOL   Disabled  (void) { return (WEBC_FALSE); }
	virtual HTMLLength  Height    (void) { return (HTML_ToLength(0,HTML_LENGTH_UNIT_NONE)); }
	virtual WEBC_CHAR*  Id        (void);
	virtual WEBC_SIZE   MaxLength (void) { return (-1); }
	virtual WEBC_CHAR*  Name      (void) { return (WEBC_NULL);}
	virtual WEBC_BOOL   ReadOnly  (void) { return (WEBC_TRUE); }
	virtual WEBC_SIZE   Size      (void) { return (0); }
	virtual WEBC_CHAR*  Src       (void) { return (WEBC_NULL); }
	virtual WEBC_INT16  TabIndex  (void) { return (-1); }
	virtual WEBC_CHAR*  Value     (void) { return (WEBC_NULL); }
	virtual HTMLLength  Width     (void) { return (HTML_ToLength(0,HTML_LENGTH_UNIT_NONE)); }
	virtual WEBC_CHAR*  Title        (void);
	virtual WEBC_CHAR*  TextDirection(void);
	virtual WEBC_CHAR*  Language     (void);

	// html attribute modifiers
	virtual void SetAccessKey (WEBC_UINT16 key)        {}
    virtual void SetAlt       (const WEBC_CHAR* alt)   {}
	virtual void SetChecked   (WEBC_BOOL checked)      {}
	virtual void SetDefValue  (const WEBC_CHAR* value) {}
	virtual void SetDisabled  (WEBC_BOOL disabled)     {}
	virtual void SetId        (const WEBC_CHAR* id);
	virtual void SetMaxLength (WEBC_SIZE maxLength)    {}
	virtual void SetName      (const WEBC_CHAR* name)  {}
	virtual void SetReadOnly  (WEBC_BOOL readonly)     {}
	virtual void SetSize      (WEBC_SIZE size)         {}
	virtual void SetSrc       (const WEBC_CHAR* src)   {}
	virtual void SetTabIndex  (WEBC_INT16 tabIndex)    {}
	virtual void SetValue     (const WEBC_CHAR* value) {}
	virtual void SetTitle     (const WEBC_CHAR* pTitle);
	virtual void SetTextDirection(const WEBC_CHAR* pTextDirection);
	virtual void SetLanguage  (const WEBC_CHAR* pLanguage);

	// Methods for getting/setting properties of elements
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);

	virtual HTMLForm *Form(void);
	virtual void SetForm(HTMLForm *pForm);

	virtual WEBC_BOOL IsNameOrId(WEBC_CHAR *);
	virtual int IsInput();

	// HTMLElement Data
	HTMLElement *mpParent;
	HTMLElement *mpNext;
	HTMLElement *mpPrev;

	virtual HTMLElementType Type (void) { return HTML_ELEMENT; }
	virtual WEBC_BOOL IsBasedOnType (HTMLElementType type) { return (type == HTML_ELEMENT); }

	// TextUnit interface
	virtual WEBC_UINT16 GetUnitType(void);
	virtual WEBC_UINT16 GetBreak(void);
	virtual WEBC_UINT16 GetClear(void);
	virtual WEBC_UINT16 GetDir(void);
	virtual WEBC_UINT16 GetWrap(void);
	virtual WEBC_UINT16 GetChildWrap(void);
	virtual WEBC_UINT16 GetVisibility(void);
	virtual WEBC_UINT16 GetPosition(void);
	virtual WEBC_UINT8  GetFloat(void);
	virtual WEBC_UINT8  GetTextAlign(void);
	virtual WEBC_UINT8  GetChildTextAlign(void);
	virtual WEBC_UINT8  GetVAlign(void);
	virtual WEBC_INT16  GetZIndex(void);
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);
	virtual DISPLAY_INT GetLeft(void);
	virtual DISPLAY_INT GetTop(void);
	virtual DISPLAY_INT GetTopMargin(void);
	virtual DISPLAY_INT GetLeftMargin(void);
	virtual DISPLAY_INT GetBottomMargin(void);
	virtual DISPLAY_INT GetRightMargin(void);
	virtual DISPLAY_INT GetBaseline(void);
	virtual DISPLAY_INT GetParagraphSpace(void) {return 0;}
	virtual void SetXYPosition(DISPLAY_INT iXPos, DISPLAY_INT iYPos);
	virtual void SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail);
	virtual void SetParentHeight(DISPLAY_INT iHeight);
	virtual void SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore);
	virtual void SetDisplayContext(DisplayElement *parent);
	virtual void Release(void);

	// Observer interface
	virtual void Update(WEBC_PFBYTE o, WEBC_PFBYTE data); // tbd - DEPRECATED

	virtual void SetClass(WEBC_CHAR *pClass, HTMLDocument *pDoc = 0);
	virtual void ResetClass(void);

	HELEM_INLINE HELEM_FLAGS GetFlags  (void)          { return mFlags; }
	HELEM_INLINE void        SetFlag   (HELEM_FLAGS f) { mFlags |= f; }
	HELEM_INLINE void        ClearFlag (HELEM_FLAGS f) { mFlags &= ~f; }

#if (WEBC_SUPPORT_STYLE_SHEETS)
	virtual const WEBC_CHAR * IdLowercase(void);
	virtual CSSStyleElement * GetStyleParent(void);
	virtual int InPseudoClass(CSSPseudoClass c) { return (c == CSS_PSEUDO_CLASS_NONE); }
#endif // (WEBC_SUPPORT_STYLE_SHEETS)

	virtual Enumerator * GetClasses(void);
	virtual HTMLTagType GetTag(void);									// ...

	void StyleUpdate         (NotifyEventType eventType = NOTIFY_STYLE_CHANGE);
	void StyleUpdateChildren (NotifyEventType eventType = NOTIFY_STYLE_CHANGE,
	                          NotifyEventType childEvent = NOTIFY_PARENT_STYLE_CHANGE);
	void ParentStyleUpdate   (NotifyEventType eventType = NOTIFY_PARENT_STYLE_CHANGE);

	virtual void LocalStyleUpdate (void);
	virtual void InheritedStyleUpdate (void);
	virtual void NotifyDisplay (NotifyEventType event);

	void SetTagProperties(HTMLTagParse *td, HTMLDocument *pDocument, WebString* valueBuffer, int callBase = 1);
	void SetTagProperties(HTMLTagParse *td, HTMLDocument *pDocument, int callBase = 1);

	virtual WebFont              GetWebFont (void);
	virtual ITextDisplayStyle*   GetTextDisplayStyle (WEBC_BOOL create = WEBC_TRUE) { return 0; }
	virtual WebGraphFont*        GetFont (void);
	virtual const WEBC_CHAR*     GetFontFamily (void);
	virtual void                 GetFontSize (CSSPropertyValue *value);
	virtual CSSFontWeight        GetFontWeight (void);
	virtual CSSFontStyle         GetFontStyle (void);
	virtual CSSTextDecoration    GetTextDecoration (void);
	virtual HTMLColor            GetColor (void);
	virtual HTMLColor            GetBackgroundColor (void);
	virtual void                 GetWidth (CSSLength *width);
	virtual void                 GetHeight (CSSLength *height);
	virtual void                 GetLengthProperty (CSSPropertyType type, CSSLength *len);
	virtual HTMLColor            GetBorderColor(CSSSide side);
	virtual CSSBorderStyle       GetBorderStyle(CSSSide side);
	virtual void                 GetBorderWidth(CSSSide side, CSSBorderThickness *width);
	virtual void                 GetMarginWidth(CSSSide side, CSSLength *width);
	virtual void                 GetPaddingWidth(CSSSide side, CSSLength *width);
	virtual HTMLColor            GetOutlineColor(void);
	virtual CSSBorderStyle       GetOutlineStyle(void);
	virtual void                 GetOutlineWidth(CSSBorderThickness *width);
	virtual CSSListStyleType     GetListStyleType(void);
	virtual CSSListStylePosition GetListStylePosition(void);

	virtual WebcFontDescriptor   GetFontFamilyDesc (void);
	virtual WebcFontDescriptor   GetFontSizeDesc   (void);
	virtual WebcFontDescriptor   GetFontWeightDesc (void);
	virtual WebcFontDescriptor   GetFontStyleDesc  (void);
	virtual DISPLAY_INT          GetFontHeight     (void);

	virtual CSSValueType GetStyleFromCSS(CSSPropertyType Property, CSSPropertyValue *pValue);
	virtual CSSValueType GetStyleFromParent(CSSPropertyType Property, CSSPropertyValue *pValue);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);

	virtual DisplayElement* GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual DisplayElement* GetLastDisplayElement (void) { return GetDisplayElement(WEBC_FALSE); }
	virtual DisplayElement* GetDisplayParent (void);
	virtual TextUnit*       GetTextUnit (void);
	virtual WEBC_BOOL 		GetVisibleFromDisplay (void);
	virtual DISPLAY_INT     GetClientHeight (void);
	virtual DISPLAY_INT     GetClientWidth (void);
	virtual DISPLAY_INT     GetClientLeft (void);
	virtual DISPLAY_INT     GetClientTop (void);
	virtual int             GetDocumentPosition (DISPLAY_INT *px, DISPLAY_INT *py);
	virtual int             GetDocumentDisplayRect (WebRect *rect, int n);
	virtual int             SetDocumentDisplayRect (WebRect *rect);
	virtual int             IsVisible (void);

	virtual void Refresh (WEBC_BOOL draw = WEBC_TRUE);

	DisplayElement* GetFirstDisplayChild (void);

#ifdef WEBC_DEBUG
	virtual void Dump(int d=0);
#endif
};


class HTMLElementContainer :
	public HTMLElement
{
protected:
	HTMLElement *mpFirstChild;
	HTMLElement *mpLastChild;

public:
  #if (WEBC_SUPPORT_JSCRIPT)
	HTMLElementContainer(HTMLTagParse *td = 0, HTMLDocument *pDocument = 0, JSObject *pJSObj = 0);
  #else
	HTMLElementContainer(HTMLTagParse *td = 0, HTMLDocument *pDocument = 0);
  #endif // WEBC_SUPPORT_JSCRIPT
	HTMLElementContainer(HTMLElementContainer &copy);
	virtual ~HTMLElementContainer(void);

	// Methods for adding/removing children
	virtual void DeleteChildren(void);
	virtual void InsertFirst(HTMLElement *pNode);
	virtual void InsertAfter(HTMLElement *pNode, HTMLElement *pRef);
	virtual void InsertBefore(HTMLElement *pNode, HTMLElement *pRef);
	virtual void InsertLast(HTMLElement *pNode);
	virtual WEBC_BOOL IsInserted();
	virtual void Remove(HTMLElement *pNode);
	virtual void NotifyChildAdded(void);
	virtual void NotifyChildRemoved(void);

	virtual HTMLElement *FirstChild (void);
	virtual HTMLElement *LastChild (void);

#ifdef WEBC_DEBUG
	virtual void Dump(int d = 0);
#endif
};

struct HTMLElementPtr
{
	HTMLElement *pElement;
};

class HTMLElementTreeWalker
{
 private:
 	HTMLElement       *mpRoot;
 	HTMLElement       *mpPrev;
 	HTMLElement       *mpNext;
 	HTMLElementFinder *mpFilter;
 	HTMLElementFinder *mpStepIntoFilter;

 public:
 	HTMLElementTreeWalker (
			HTMLElement* root,
			HTMLElement* first,
			HTMLElementFinder* filter,
			HTMLElementFinder* stepInto
	 	);

 	~HTMLElementTreeWalker(void);

 	HTMLElement *getNext();
};

#endif // __HELEMENT_HPP__
