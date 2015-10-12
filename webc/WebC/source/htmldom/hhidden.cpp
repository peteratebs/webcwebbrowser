//
// HHIDDEN.CPP - Methods for HTML DOM class HTMLInputHidden
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

#include "htmldoc.hpp"
#include "hhidden.hpp"
#include "webcstr.h"


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
// HTMLInputHidden Constructors
/*****************************************************************************/

HTMLInputHidden::HTMLInputHidden(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLInput(td, pDocument)
{
	mValue = 0;
	mInputType = HTML_INPUT_TYPE_HIDDEN;
}


/*****************************************************************************/
// HTMLInputHidden Destructor
/*****************************************************************************/

HTMLInputHidden::~HTMLInputHidden(void)
{
	webc_free_string_copy(mValue, __FILE__, __LINE__);
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/

WEBC_UINT16 HTMLInputHidden::GetUnitType(void)
{
	return (TU_TYPE_NODISPLAY);
}

	
/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/

/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

WEBC_CHAR *HTMLInputHidden::Value(void)
{
	return (mValue);
}


void HTMLInputHidden::SetValue(const WEBC_CHAR *pValue)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(pValue, __FILE__, __LINE__);
	webc_free_string_copy(mValue, __FILE__, __LINE__);
	mValue = pStr;
}


/*****************************************************************************/
// HTMLInputHidden-Specific Methods
/*****************************************************************************/
