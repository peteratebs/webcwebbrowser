//
// wcelement.CPP -
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
#include "wcapi.h"

#include "dflow.hpp"
#include "helement.hpp"
#include "htmlbrow.hpp"
#include "webcassert.h"

/*****************************************************************************/
// wcxELIdMatches
/**

 @doc   Compare an element's id to a string, return true if they match.


 @param  wcEL          El        -   The element to compare.
 @param  char*         IdStr   -   The string to match.

 @return wcTRUE    if the element has an assigned id and they match wcFALSE otherwise.

 @see
 */
/*****************************************************************************/


wcBOOL wcxELIdMatches(wcEL element, char *string)
{
    if (HELEMENT_OBJ(element)->Id() && string && !webc_c_stricmp(HELEMENT_OBJ(element)->Id(), string))
        return wcTRUE;
    return wcFALSE;
}


/*****************************************************************************/
//
/**
 @name	Element Function Introduction
 @doc   Element Function Introduction
 <pre>
 The following webC specific non-XPATH expressions are also supported.
 </pre>

 */
/*****************************************************************************/
