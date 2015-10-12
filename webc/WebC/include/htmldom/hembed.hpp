#ifndef __HEMBED_HPP__
#define __HEMBED_HPP__

#include "helement.hpp"
#if(WEBC_SUPPORT_EMBED)
/*#define HIMG_FLAG_SRC_DIRTY          HINPUT_FLAG_UNUSED_0
#define HIMG_FLAG_HAS_FOCUS          HINPUT_FLAG_UNUSED_1
#define HIMG_FLAG_WIDTH_CACHED       HINPUT_FLAG_UNUSED_2
#define HIMG_FLAG_HEIGHT_CACHED      HINPUT_FLAG_UNUSED_3
#define HIMG_FLAG_HSPACE_SPECIFIED   HINPUT_FLAG_UNUSED_4
#define HIMG_FLAG_VSPACE_SPECIFIED   HINPUT_FLAG_UNUSED_5
#define HIMG_FLAG_IS_MAP             HINPUT_FLAG_UNUSED_6
#define HIMG_FLAG_UNUSED_0           HINPUT_FLAG_UNUSED_7

#define HTML_IMAGE_MAP_NONE   0
#define HTML_IMAGE_MAP_CLIENT 1
#define HTML_IMAGE_MAP_SERVER 2*/

//class HTMLembed; // we have to intialize it

class HTMLEmbed : public HTMLElement
{
protected:
	WEBC_CHAR* InsertXYCoords(WEBC_CHAR* base_str, DISPLAY_INT x, DISPLAY_INT y);

	HTMLLength     mEmbedWidth;
	HTMLLength     mEmbedHeight;
	WEBC_INT16     mHSpace;
	WEBC_INT16     mVSpace;
	WEBC_CHAR*     mAlt;
	WEBC_CHAR*     mLongDesc;
	WEBC_CHAR*     mUseMap;
	//HTMLMap*       mMap;
	WEBC_INT16     mTabIndex;
    //WEBC_UINT8     mImgAlign;
	WEBC_UINT8     mEmbedAlign;
	WebcImage*     mImage;

	// For <INPUT TYPE=IMAGE>
	WEBC_CHAR*     mName;
	WEBC_CHAR*     mSrc;
	WEBC_CHAR*	   mpType;

	// border support (as specified in the html attributes)
	CSSBorderStyle mBorderStyle;
	short mBorderWidth;


	//HTMLEmbedDisplay* GetEmbedDisplay (WEBC_BOOL create = WEBC_TRUE);
	//HTMLEmbedDisplay* mEmbedDisplay;

public:
#if (WEBC_SUPPORT_JSCRIPT)
	HTMLEmbed(HTMLTagParse *td = 0, HTMLDocument *pDocument = 0, JSObject *pJSObj = 0);
#else
	HTMLEmbed(HTMLTagParse *td = 0, HTMLDocument *pDocument = 0);
#endif // WEBC_SUPPORT_JSCRIPT
	virtual ~HTMLEmbed();

	virtual HTMLElementType Type() { return HTML_EMBED_ELEMENT; }
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
//	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	//virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	//virtual WEBC_BOOL   Disabled  (void) { return (mFlags & HINPUT_FLAG_DISABLED)? WEBC_TRUE : WEBC_FALSE; }
	//virtual HTMLEventStatus JSEvent(HTMLEvent *pEvent);

	// Listener interface
	virtual void Notify (Notifier *who, NotifyEvent *what);

	// Observer interface
	virtual void Update(WEBC_PFBYTE o, WEBC_PFBYTE data);

	       /* WEBC_UINT8 GetMapType(void)
	        {
	        	if (mFlags & HIMG_FLAG_IS_MAP)
	        	{
	        		if (mUseMap)
	        		{
	        			return (HTML_IMAGE_MAP_CLIENT);
	        		}
	        		else
	        		{
	        			return (HTML_IMAGE_MAP_SERVER);
	        		}
	        	}
	        	else
	        	{
	        		return HTML_IMAGE_MAP_NONE;
	        	}
	        }

	        HTMLMap*   Map(void) { return mMap; }
	        WEBC_BOOL  IsMap(void) { return ((mFlags & HIMG_FLAG_IS_MAP) != 0); }
	        HTMLMap*   GetMap(void);

	        void       SetMap(HTMLMap *pMap) { mMap = pMap; }
	        void       SetImage(WebcImage *image);
	        WebcImage* GetImage(void);
	        void       SetIsMap(WEBC_BOOL b) { if (b) SetFlag(HIMG_FLAG_IS_MAP); else ClearFlag(HIMG_FLAG_IS_MAP); }*/

	// For <INPUT TYPE=Embed>
	virtual WEBC_CHAR*  Name     (void);
	virtual WEBC_CHAR*  Src      (void);
	virtual void SetName     (const WEBC_CHAR *name);
	virtual void SetSrc      (const WEBC_CHAR* src);
	virtual void SetType	 (const WEBC_CHAR *pType);
	/*virtual WEBC_CHAR*  Alt      (void) { return mAlt; }
	virtual WEBC_CHAR*  LongDesc      (void) { return mLongDesc; }
	virtual WEBC_CHAR*  Name     (void);
	virtual WEBC_CHAR*  Src      (void);
	virtual WEBC_CHAR*  UseMap   (void) { return mUseMap;}
	virtual WEBC_INT16  TabIndex (void);

    virtual void SetAlt      (const WEBC_CHAR *alt);
    virtual void SetLongDesc      (const WEBC_CHAR *alt);
    virtual void SetName     (const WEBC_CHAR *name);
	virtual void SetSrc      (const WEBC_CHAR* src);
	virtual void SetTabIndex (WEBC_INT16 index);
	virtual void SetUseMap   (const WEBC_CHAR* useMap);*/

	WEBC_INLINE DISPLAY_INT GetHSpace (void) { return mHSpace; }
	WEBC_INLINE DISPLAY_INT GetVSpace (void) { return mVSpace; }
	void SetHSpace (DISPLAY_INT h);
	void SetVSpace (DISPLAY_INT v);
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);

	//virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	//virtual TextUnit       *GetTextUnit (void);

	virtual void SetForm(HTMLForm *pForm);

	void ClearLoadJob (LoadJob *pJob);
	LoadJob *GetLoadJob (void);

	void RefreshFromSrc(); // re-reads the original src file (in support of webC img extensions (jet))

};

#endif
#endif // __HEMBED_HPP__

