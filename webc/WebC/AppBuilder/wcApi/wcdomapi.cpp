//
// wcdomapi.CPP -
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
#include "htmlfind.hpp"
#include "htmldoc.hpp"
#include "webcmem.h"
#include "rtptime.h"

static enum DomTraverseResult _ParseNextExpression(char *p, int *CharsConsumed);
static wcEL _FindElementById(wcCtx* Ctx, wcEL El, char *findId);
static wcEL _FindParentElement(wcCtx* Ctx,wcEL El);
static wcEL _FindFirstChild(wcCtx* Ctx,wcEL El);
static wcEL _FindFirstChildInRoot(wcCtx* Ctx);

static wcEL _FindElementInRoot(wcCtx* Ctx, char *findId);
static wcEL _FindLastChild(wcCtx* Ctx,wcEL El);
static wcEL _FindPrevSibling(wcCtx* Ctx,wcEL El);
static wcEL _FindNextSibling(wcCtx* Ctx,wcEL El);

/*****************************************************************************/
// wcElFind
/**

 @memo  Traverse the dom following an XPATH specifier.
 @doc   Traverse the dom following the provided path and return the element.

 @param  wcCtx*     Ctx     -   The current wc context.
 @param  wcEL       El      -   The root element for the search, may be zero if the search path begins at the root of the document.
 @param  char*     Domstr   -   A string containing the path from the current element.
 @param  wcBOOL*   pError    -   Set to wcTRUE if some kind of error.
 <pre>
 The string is a subset of the XPATH standard having the form "id/id/id/id".
 The following subset of XPATH expressions are supported.
 </pre>
 <ul>
 <li> '/'             - The root of the current document (only valid for the first character) </li>
 <li> '.'             - The current element </li>
 <li> '..'             - Parent of the current element </li>
 <li> '*'             - The first child of the element. </li>
 <li> '*[last()]'     - The last child of the  current element. </li>
 </ul>
 <pre>
 The following webC specific non-XPATH expressions are also supported.
 </pre>
 <ul>
 <li> '<'             - Previous sibling to the current element. </li>
 <li> '>'             - Next sibling to the current element. </li><li>
 </ul>
 <pre>
 Examples:
     ItemFromRoot          = wcELFind (Ctx, 0, "/section/subsection/paragraph/item");
     FirstChild            = wcELFind (Ctx, Parent, "*");
     LastChild             = wcELFind (Ctx, Parent, "*[last()]");
     LastGrandChild        = wcELFind (Ctx, Parent, "*[last()]/*[last()]");
     FirstGrandChild       = wcELFind (Ctx, Parent, "* /*");  (no space after * please)
     Child                 = wcELFind (Ctx, Parent, "myChild");
     Child                 = wcELFind (Ctx, Parent, "./myChild");
     Cousin                = wcELFind (Ctx, Parent, "../myUncle/HisChild");
     Cousin                = wcELFind (Ctx, Parent, "../myUncle/*");
     GrandParent           = wcELFind (Ctx, Parent, "../..");
     PrevSibling           = wcELFind (Ctx, Parent, "<");
     Nephew                = wcELFind (Ctx, Parent, ">/HisChild");
     Nephew                = wcELFind (Ctx, Parent, "</HisChild");
     Example: traverse all children of element X.
         el=wcELFind (Ctx, X, "*");
         while (el)
             el=wcELFind (Ctx, el, ">");
 </pre>

 @return wcEL        The element if successful, 0 if not found, ELERROR on an error.

 */
/*****************************************************************************/

wcEL wcELFind (wcCtx* Ctx, wcEL El, char *Domstr, wcBOOL *pError)
{
wcEL current;
char *p;
int isRoot = 0;
	*pError = wcFALSE;
    p = Domstr;
    current = El;
    if (*p == '/')
    {
        current = 0;
    	isRoot = 1;
        p++;
    }
    else
	{
        current = El;
		if (!current)
			*pError = wcTRUE;
	}

    while (*p && (current || isRoot))
    {
        enum DomTraverseResult opcode;
        int nChars;
        opcode = _ParseNextExpression(p, &nChars);
		// Find first and find is the only legal operation on the root
     	if (isRoot && (opcode!=DomFind && opcode!=DomFirstChild))
		{
     		*pError = wcTRUE;
     		return current;
		}
        switch (opcode) {
            case DomFind:
            {
                char findBuffer[MAXIDSIZE+1];
                if (nChars > MAXIDSIZE)
                    nChars = MAXIDSIZE;
                rtp_memcpy(findBuffer,p,nChars);
				findBuffer[nChars] = 0;
            	if (isRoot)
            		current = _FindElementInRoot(Ctx,findBuffer);
				else
					current = _FindElementById(Ctx,current,findBuffer);
            }
            break;
            case DomSelf:
                current = current;
            break;
            case DomParent:
                current = _FindParentElement(Ctx,current);
            break;
            case DomFirstChild:
            	if (isRoot)
            		current = _FindFirstChildInRoot(Ctx);
				else
               		current = _FindFirstChild(Ctx,current);
            break;
            case DomLastChild:
                current = _FindLastChild(Ctx,current);
            break;
            case DomPrevSibling:
                current = _FindPrevSibling(Ctx,current);
            break;
            case DomNextSibling:
                current = _FindNextSibling(Ctx,current);
            break;
            case DomError:
            default:
            	*pError = wcTRUE;
                current = 0;
            break;
        }
        p+= nChars;
        if (*(p) == '/')
            p++;
        else if (*(p) != '\0')
		{
           	*pError = wcTRUE;
            current = 0;
		}
		isRoot = 0;
    }
    return current;
}

static enum DomTraverseResult _ParseNextExpression(char *p, int *CharsConsumed)
{
enum DomTraverseResult result = DomFind;
int nchars = 0;
    switch (*p) {
        case '.':
            if (*(p+1) == '.')
            {    result = DomParent; nchars = 2;}
            else if (*(p+1) == '/' || *(p+1) == '\0')
            {    result = DomSelf; nchars = 1;}
            break;
        case '*':
            if (*(p+1) == '/' || *(p+1) == '\0')
            {
                nchars = 1;
                result = DomFirstChild;
            }
            else if (*(p+1) == '[' && rtp_strstr((p+1), "[last()]/") == (p+1) )
            {
                nchars = 8;
                result = DomLastChild;
            }
            break;
        case '<':
            nchars = 1;
            result = DomPrevSibling;
            break;
        case '>':
            nchars = 1;
            result = DomNextSibling;
            break;
        default:
            nchars = 0;
            result = DomFind;
            break;
    }
    if (result == DomFind)
    {
        char *separator;
        separator = rtp_strstr(p, "/");
        if (separator)
        {
            nchars = (int) (separator - p);
        }
        else
            nchars = rtp_strlen(p);
        if (nchars == 0)
            result = DomError;
    }
    *CharsConsumed = nchars;
    return (result);
}
static wcEL _FindFirstChildInRoot(wcCtx* Ctx)
{
	return (wcEL) ((HTMLDocument *) Ctx->hdoc)->Body()->FirstChild();
}
static wcEL _FindElementInRoot(wcCtx* Ctx, char *findId)
{
wcEL ElResult, r;
    ElResult = 0;
    r = (wcEL) ((HTMLDocument *) Ctx->hdoc)->Body()->FirstChild();
    while (r)
    {
        if (wcELIdMatches(r, findId))
        {
            ElResult = r;
            break;
        }
        r = _FindNextSibling(Ctx,r);
    }
    return ElResult;

}
static wcEL _FindElementById(wcCtx* Ctx,wcEL El, char *findId)
{
wcEL ElResult, r;
    ElResult = 0;
    r = _FindFirstChild(Ctx, El);
    while (r)
    {
        if (wcELIdMatches(r, findId))
        {
            ElResult = r;
            break;
        }
        r = _FindNextSibling(Ctx,r);
    }
    return ElResult;
}
static wcEL _FindParentElement(wcCtx* Ctx,wcEL El)
{
    return (HELEMENT_HANDLE) (HELEMENT_OBJ(El)->mpParent);
}
static wcEL _FindFirstChild(wcCtx* Ctx,wcEL El)
{
    return (wcEL) (HELEMENT_OBJ(El)->FirstChild());
}
static wcEL _FindLastChild(wcCtx* Ctx,wcEL El)
{
wcEL Elem, prevElem, result;
    result = Elem = prevElem = 0;
    Elem = _FindFirstChild(Ctx, El);
    while (Elem)
    {
        result = Elem;
        Elem = _FindPrevSibling(Ctx, Elem);
    }
    return result;
}
static wcEL _FindPrevSibling(wcCtx* Ctx,wcEL El)
{
    return (HELEMENT_HANDLE) (HELEMENT_OBJ(El)->mpPrev);
}
static wcEL _FindNextSibling(wcCtx* Ctx,wcEL El)
{
    return (HELEMENT_HANDLE) (HELEMENT_OBJ(El)->mpNext);
}

