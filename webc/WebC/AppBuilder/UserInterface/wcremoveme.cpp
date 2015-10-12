//
// wcremoveme.CPP -
//
// EBS - WebC
//
// Copyright EBS Inc. , 2011
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

//#include "../include/NewApi.h"
#include "../wcapi/wcapi.h"
//#include "dflow.hpp"
//#include "helement.hpp"
//#include "htmlbrow.hpp"
#include "webcassert.h"

#if (1)

int webc_ElementRemoveChild (wcEL element,wcEL child)
{
	if (wcELRemoveChild(element,child))
		return 0;
	else
		return -1;
}

int webc_ElementDeleteChildren (wcEL element)
{
	if (wcELDeleteChildren(element))
		return 0;
	else
		return -1;
}

int webc_ElementDelete(wcEL element)
{
	if (wcELDelete(element))
		return 0;
	else
		return -1;
}

int webc_ElementInsertFirst(wcEL element,wcEL child)
{
	if (wcELInsertFirst(element,child))
		return 0;
	else
		return -1;
}
int webc_ElementInsertLast(wcEL element,wcEL child)
{
	if (wcELInsertLast(element,child))
		return 0;
	else
		return -1;
}
int webc_ElementInsertAfter(wcEL element,wcEL insertthischild,wcEL afterthischild)
{
	if (wcELInsertAfter(element,afterthischild,insertthischild))
		return 0;
	else
		return -1;
}
int webc_ElementInsertBefore(wcEL element,wcEL insertthischild,wcEL beforethischild)
{
	if (wcELInsertBefore(element,beforethischild,insertthischild))
		return 0;
	else
		return -1;
}


wcEL webc_CreateElementFromHtml(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, char *Html)
{
wcEL NewElement;
wcEL FactoryContainer = webc_DocFindElement(hdoc, "FactoryContainer", 0, HTML_ELEMENT_ANY, 0);
	if (!FactoryContainer)
		goto error;
	webc_ElementSetInnerHtmlASCII (FactoryContainer,Html);
	NewElement =  webc_ElementGetFirstChild(FactoryContainer);
	if (!NewElement)
		goto error;
	webc_ElementRemoveChild (FactoryContainer, NewElement);
	return NewElement;
error:
	assert(0);
	return(0);
}

#endif
