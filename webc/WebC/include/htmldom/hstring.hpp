#ifndef __HSTRING_HPP__
#define __HSTRING_HPP__

#include "helement.hpp"

/*************************************************************************
* class HTMLString
* 
*************************************************************************/

class HTMLStringDisplay;

#define HTMLSTRING_FLAG_UPPERCASE		0x01			// CSS Text Transform property
#define HTMLSTRING_FLAG_CAPITALIZE		0x02			//  "
#define HTMLSTRING_FLAG_LOWERCASE		0x03			//  "
#define HTMLSTRING_MASK_TRANSFORM		0x03			//  "
#define HTMLSTRING_FLAG_SMALLCAPS		0x04			// CSS Font variant property

class HTMLString : public HTMLElement
{
public:
	HTMLString(HTMLDocument* document, WEBC_CHAR* str);
	HTMLString(HTMLTagParse *td, HTMLDocument* document);
	HTMLString(HTMLString &copy);
	virtual ~HTMLString(void);

	virtual HTMLElementType Type() { return HTML_STRING_ELEMENT; }
	virtual void NotifyDisplay (NotifyEventType event);
	virtual CSSValueType GetStyleFromCSS(CSSPropertyType Property, CSSPropertyValue *pValue);

  #if (WEBC_SUPPORT_INNER_HTML)
	virtual void SetInnerHTML (WEBC_CHAR* innerHTML);
	virtual void SetOuterHTML (WEBC_CHAR* outerHTML);
	virtual void AppendInnerHTML (WebString* toString);
	virtual void AppendOuterHTML (WebString* toString);
	virtual void AppendRenderedText (WebString* toString);
	virtual void SetRenderedText(WEBC_CHAR*  textVal);
  #endif // WEBC_SUPPORT_INNER_HTML

	// Methods overridden from class HTMLElement
	virtual WEBC_UINT16 GetUnitType(void);
	virtual WEBC_UINT16 GetClear(void);
	virtual WEBC_UINT16 GetVisibility(void);
	virtual WEBC_UINT16 GetPosition(void);
	virtual DISPLAY_INT GetLeft(void);
	virtual DISPLAY_INT GetTop(void);
	virtual WEBC_UINT8  GetFloat(void);
	virtual WEBC_UINT8  GetVAlign(void);
	virtual WEBC_INT16  GetZIndex(void);
	virtual void Update(WEBC_PFBYTE o, WEBC_PFBYTE data);

	virtual DisplayElement* GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual DisplayElement* GetLastDisplayElement (void);
	virtual int             GetDocumentDisplayRect (WebRect *rect, int n);

	virtual WebGraphFont*     GetFont(void);
	virtual CSSTextDecoration GetTextDecoration (void);
	virtual HTMLColor         GetColor (void);
	virtual HTMLColor         GetBackgroundColor (void);	
	virtual HTMLColor         GetOutlineColor(void);
	virtual CSSBorderStyle    GetOutlineStyle(void);
	virtual void              GetOutlineWidth(CSSBorderThickness *width);
	
	// tbd - is this needed?
	virtual void Refresh (WEBC_BOOL draw = WEBC_TRUE);
	virtual TextUnit *GetTextUnit (void);

	void SetString(WEBC_CHAR *);
	WEBC_CHAR *GetString(void);
	WEBC_CHAR *GetString(WEBC_SIZE iCharOffset);
	WEBC_SIZE GetStringLength(void);
	HTMLStringDisplay *GetStringDisplay (HTMLStringDisplay *pPrev);
	
protected:
	WEBC_SIZE miLength;
	WEBC_CHAR *mpString;	
	HTMLStringDisplay *mpFirstDisplay;
};

#endif // __HSTRING_HPP__
