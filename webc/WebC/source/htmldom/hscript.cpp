//
// HSCRIPT.CPP - Methods for HTML DOM class HTMLScript
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
#include "hscript.hpp"
#include "hstring.hpp"
#include "webload.hpp"
#include "wjscript.hpp"
#include "webcstr.h"
#include "webcmem.h"
#include "htmlbrow.hpp"

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
// HTMLScript Constructors
/*****************************************************************************/

HTMLScript::HTMLScript(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElementContainer(td, pDocument)
{
	mpName = 0;
	mpSrc = 0;
#if (WEBC_SUPPORT_JSCRIPT)
	mpScript = 0;
#endif // WEBC_SUPPORT_JSCRIPT
	mCharset = WEBC_CHARSET_UNKNOWN;
	mTag = HTML_TAG_SCRIPT;

}


/*****************************************************************************/
// HTMLScript Destructor
/*****************************************************************************/

HTMLScript::~HTMLScript(void)
{
#if (WEBC_SUPPORT_JSCRIPT)
	if (mpScript)
	{
		mpScript->Destroy();
	}
#endif // WEBC_SUPPORT_JSCRIPT

	webc_free_string_copy(mpName, __FILE__, __LINE__);
	webc_free_string_copy(mpSrc, __FILE__, __LINE__);
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


void HTMLScript::Update(WEBC_PFBYTE o, WEBC_PFBYTE data)
{
#if (WEBC_SUPPORT_JSCRIPT)
HTMLDocument *pDoc = GetDocument();
WebcJSDocumentContext *jsMgr = (pDoc)? pDoc->GetJSMgr() : 0;

	if (mpScript || !jsMgr)
	{
		return;
	}

	WEBC_NEW_VERBOSE(mpScript, WebcJSScript(jsMgr, jsMgr->GetGlobalObject()),"WebcJSDocumentContext");

	if (mpScript)
	{
		if (Src())
		{
			LoadScript *pJob;
			if (pDoc)
			{
				HTMLBrowser *pBrowser = pDoc->GetBrowser();
				if (pBrowser)
				{
					WebcCharset charset = (mCharset == WEBC_CHARSET_UNKNOWN)? pDoc->GetCharset() : mCharset;
					WEBC_NEW_VERBOSE(pJob, LoadScript(this, pDoc, charset),"LoadScript");
					if (pJob)
					{
						pBrowser->ProcessLoadJob(pJob, pJob->GetURL(), pDoc->BaseURL(), pDoc, WEBC_TRUE /* urgent */);
					}
					webc_free_string_copy(mpSrc, __FILE__, __LINE__);
					mpSrc = 0;
				}
			}
		}
		else // compile the contents of this tag as the script to execute
		{
			HTMLElement *pChild = mpFirstChild;
			WEBC_CHAR* pStr;
			long iLen;
			for (; pChild != WEBC_NULL; pChild = pChild->mpNext)
			{
				if (pChild->Type() == HTML_STRING_ELEMENT)
				{
					pStr = ((HTMLString*)(pChild))->GetString();
					iLen = webc_strlen(pStr);
					if (mpScript->Write(pStr, iLen) < iLen)
					{
						mpScript->Destroy();
						mpScript = 0;
						break;
					}
				}
			}

			if (mpScript)
			{
				mpScript->Compile();
			}

			DeleteChildren();
		}
	}

#endif
}

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

int HTMLScript::SetProperty(HTMLAttributeType property, WEBC_CHAR* value, HTMLDocument* document, int call_base)
{
	if (document == 0)
	{
		document = GetDocument();
	}

	switch (property)
	{
		case HTML_ATTRIB_CHARSET:
			mCharset = webc_charset_lookup(value);
			return (1);

		case HTML_ATTRIB_NAME:
			SetName(value);
			return (1);

		case HTML_ATTRIB_SRC:
			SetSrc(value);
			return (1);

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLElement::SetProperty(property, value, document, call_base));
	}

	return (0);
}


void HTMLScript::SetName(const WEBC_CHAR *pName)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(pName, __FILE__, __LINE__);
	webc_free_string_copy(mpName, __FILE__, __LINE__);
	mpName = pStr;
}


void HTMLScript::SetSrc(const WEBC_CHAR *pSrc)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(pSrc, __FILE__, __LINE__);
	webc_free_string_copy(mpSrc, __FILE__, __LINE__);
	mpSrc = pStr;
}


/*****************************************************************************/
// HTMLScript-Specific Methods
/*****************************************************************************/

int HTMLScript::Run(void)
{
#if (WEBC_SUPPORT_JSCRIPT)
jsval result;

	if (mpScript)
	{
		int retval = mpScript->ExecuteScript(&result);
		if (retval != WEBC_JS_SCRIPT_DELETED)
		{
			mpScript->Destroy();
			mpScript = 0;
		}

		return (retval);
	}

#endif // WEBC_SUPPORT_JSCRIPT

	return (-1);
}

WebcJSScript *HTMLScript::GetJSScript(void)
{
#if (WEBC_SUPPORT_JSCRIPT)
	return (mpScript);
#else
	return 0;
#endif // WEBC_SUPPORT_JSCRIPT
}
