//
// ZDELEM.CPP - Methods for class ZOrderedDisplayElement
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "zdelem.hpp"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// ZOrderedDisplayElement methods
/*****************************************************************************/

ZOrderedDisplayElement::ZOrderedDisplayElement (WEBC_INT32 zIndex)
{
	SetZIndex(zIndex);
}

WEBC_INT32 ZOrderedDisplayElement::GetZIndex (void)
{
	return (miZIndexValue);
}

void ZOrderedDisplayElement::SetZIndex(WEBC_INT32 zIndex)
{
	miZIndexValue = zIndex;
}
