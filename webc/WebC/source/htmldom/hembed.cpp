//
// HEMBED.CPP - Methods for HTML DOM class HTMLImage
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
#include "htmldoc.hpp"
#include "hembed.hpp"
#include "hanchor.hpp"
#include "hform.hpp"
#include "webload.hpp"
#include "css.h"
#include "wgkeys.hpp"
#include "htmlfind.hpp"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "webcmem.h"

#if(WEBC_SUPPORT_EMBED)
/*****************************************************************************/
// Local Constants
/*****************************************************************************/

/*****************************************************************************/
// Local type definitions
/*****************************************************************************/

/*****************************************************************************/
// Local Functions
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// HTMLEmbed Constructors
/*****************************************************************************/

#if (WEBC_SUPPORT_JSCRIPT)
HTMLEmbed::HTMLEmbed(HTMLTagParse *td, HTMLDocument *pDocument, JSObject *pJSObj)
	: HTMLElement(td, pDocument, pJSObj)
#else
HTMLEmbed::HTMLEmbed(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElement(td, pDocument)
#endif // WEBC_SUPPORT_JSCRIPT
{
	mAlt = 0;
	mLongDesc = 0;
	//mMap = 0;
	mUseMap = 0;
	mName = 0;
	mSrc = 0;
	mpType = 0; //added type for Embed
	mTabIndex = 0;
	HTML_LengthSet(mEmbedWidth, 0, HTML_LENGTH_UNIT_NONE);
	HTML_LengthSet(mEmbedHeight, 0, HTML_LENGTH_UNIT_NONE);
	mEmbedAlign = HTML_ALIGN_NONE;
	//mImage = 0;
	mHSpace = 0;
	mVSpace = 0;

	//mImageDisplay = 0;

	mBorderStyle = CSS_BORDER_STYLE_NONE;
	mBorderWidth = 0;
}

/*****************************************************************************/
// HTMLEmbed Destructor
/*****************************************************************************/

HTMLEmbed::~HTMLEmbed(void)
{
	SetForm(WEBC_NULL);

	webc_free_string_copy(mAlt, __FILE__, __LINE__);
	webc_free_string_copy(mLongDesc, __FILE__, __LINE__);
	webc_free_string_copy(mUseMap, __FILE__, __LINE__);
	webc_free_string_copy(mName, __FILE__, __LINE__);
	webc_free_string_copy(mSrc, __FILE__, __LINE__);
	webc_free_string_copy(mpType, __FILE__, __LINE__);

	/*if (mImage)
	{
		mImage->Release();
	}

	if (mImageDisplay)
	{
		WEBC_DELETE(mImageDisplay);
	}*/
}

/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/


/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/


/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

int HTMLEmbed::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_SRC:
			SetSrc(value);
			return (1);

		case HTML_ATTRIB_NAME:
			SetName(value);
			return (1);

		case HTML_ATTRIB_TYPE:
			SetType(value);
			return (1);

		case HTML_ATTRIB_WIDTH:
			HTML_ParseLength(value, 0, &mEmbedWidth);
			return (1);

		case HTML_ATTRIB_HEIGHT:
			HTML_ParseLength(value, 0, &mEmbedHeight);
			return (1);

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLElement::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}

#if(0)
WEBC_CHAR *HTMLEmbed::Name(void)
{
	return (mName);
}

WEBC_CHAR *HTMLEmbed::Src(void)
{
	return (mSrc);
}
#endif

void HTMLEmbed::SetName(const WEBC_CHAR* name)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(name, __FILE__, __LINE__);
	webc_free_string_copy(mName, __FILE__, __LINE__);
	mName = pStr;
}

void HTMLEmbed::SetSrc(const WEBC_CHAR* src)
{
	if (!src || !mSrc || (webc_stricmp(src, mSrc)))
	{
		SetFlag(HIMG_FLAG_SRC_DIRTY);
	}

	WEBC_CHAR *pStr = webc_malloc_string_copy(src, __FILE__, __LINE__);
	webc_free_string_copy(mSrc, __FILE__, __LINE__);
	mSrc = pStr;
}

/******************************************************************************
* void HTMLEmbed::SetType(char *pvalue) - Set type attribute
*
* Parameters
*
* Description
*    Creates a local copy of the given string.
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLEmbed::SetType(const WEBC_CHAR *pType)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pType, __FILE__, __LINE__);

	webc_free_string_copy(mpType, __FILE__, __LINE__);
	mpType = pStr;
}

/*****************************************************************************/
// HTMLImage-Specific Methods
/*****************************************************************************/

WEBC_CHAR* HTMLEmbed::InsertXYCoords (
		WEBC_CHAR *base_str,
		DISPLAY_INT x,
		DISPLAY_INT y
	)
{
WEBC_CHAR xystr[36];
int sl;
WEBC_CHAR *str;

	xystr[0] = '?';
	xystr[35] = 0;
	webc_print_dec(&(xystr[1]), x, 1, 16);
	webc_c_strcat(xystr, ",");
	webc_print_dec(&(xystr[webc_strlen(xystr)]), y, 1, 16);

	sl = webc_strlen(base_str) + webc_strlen(xystr);

	str = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR) * (sl+1));
	if(!str)
	{
		return(0);
	}

	webc_strcpy(str, base_str);
	webc_strcat(str, xystr);

	return (str);
}

void HTMLEmbed::SetHSpace (DISPLAY_INT h)
{
	SetFlag(HIMG_FLAG_HSPACE_SPECIFIED);
	mHSpace = UTIL_SAFE_LONG_TO_SHORT(EBSMAX(0, h));
}

void HTMLEmbed::SetVSpace (DISPLAY_INT v)
{
	SetFlag(HIMG_FLAG_VSPACE_SPECIFIED);
	mVSpace = UTIL_SAFE_LONG_TO_SHORT(EBSMAX(0, v));
}

DISPLAY_INT HTMLEmbed::GetPixelWidth(void)
{
	//return (GetImageDisplay(WEBC_TRUE)->GetPixelWidth());
	return 0;
}

DISPLAY_INT HTMLEmbed::GetPixelHeight(void)
{
	//return (GetImageDisplay(WEBC_TRUE)->GetPixelHeight());
	return 0;
}


void HTMLEmbed::Notify (Notifier *who, NotifyEvent *what)
{
    WEBC_DEBUG_ReceiveNotify(this, who, what);
	/*switch (what->type)
	{
		//case NOTIFIER_TYPE_IMAGE: //NOTIFIER_TYPE_Embed in listener.hpp file (enum NotifierType)
			switch (what->event)
			{
				case NOTIFY_DONE:
					ClearFlag(HELEM_FLAG_LOADING_IMAGE);
					SetImage((WebcImage *) who);
// <EDIT> 20080120 _Added Call to Event(HTML_EVENT_LOAD) implements onload event for images
// <EDIT> 20080120 _
                    {
                		HTMLEvent e;
                		e.type = HTML_EVENT_LOAD;
                		e.data.position.x = 0;
                		e.data.position.y = 0;
                        Event(&e);
                    }
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}*/
}




/******************************************************************************
* HTMLImage::SetForm() -
*
* Parameters
*
* Description
*
* Returns (void)
*   nothing
******************************************************************************/

void HTMLEmbed::SetForm(HTMLForm* pForm)
{
	if (Form())
	{
		Form()->RemoveField(this);
	}
	HTMLElement::SetForm(pForm);
	if (Form() && GetTag() == HTML_TAG_INPUT)
	{
		Form()->AddField(this);
	}
}

WEBC_CHAR *HTMLEmbed::Src(void)
{
	return (mSrc);
}

WEBC_CHAR *HTMLEmbed::Name(void)
{
	return (mName);
}


void HTMLEmbed::Update(WEBC_PFBYTE o, WEBC_PFBYTE data)
{
	// if the src dirty bit is set, then reload our bitmap
		if (Src())
		{
			HTMLDocument *pDoc = GetDocument();
			if (pDoc)
			{
				pDoc->LoadEmbed(Src(), this);
			}
		}

	HTMLElement::Update(o,data);
}

#endif //WEBC_SUPPORT_EMBED
