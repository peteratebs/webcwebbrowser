//
// wcdom.CPP -
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


struct EnumParms
{
	wcCtx* Ctx;
	wcDocEnumFn pfn;
	int Intparm;
	void *Voidparm;
};

static WEBC_BOOL wcDocEnumerateFinderCallback(HTMLElement *pElem, int chore, void **ppContext)
{
struct EnumParms *Pe;
	Pe = (struct EnumParms *)ppContext;
	return (WEBC_BOOL) Pe->pfn(Pe->Ctx,(wcEL) pElem, Pe->Intparm, Pe->Voidparm);
	return(0);
}



/*****************************************************************************/
// wcDocEnumerate
/**

 @memo Traverse all elements in a document with user callback.
 @doc Traverse all elements in the current document and pass each element to a user supplied
 callback function. The callback instructs the enumerator to continue scanning or stop.

 @param  wcCtx*     	Ctx     	-   The current wc context.
 @param  wcDocEnumFn 	pfn    		-   User supplied callback function.
 @param  int 			Intparm 	-   Integer (command perhaps) passed to the callback fucntion.
 @param  void*			VoidParm 	-   Void pointer (pass by reference or return by reference.)

 <pre>
  The user callback function is of type:
  	typedef wcBOOL (*wcDocEnumFn)(wcCtx* Ctx, wcEL El, int UserIntParm, void *UserVoidParm);
	When the callback fuction returns wcFALSE the enumeration continues, when it returns wcTRUE, enumeration stops.

 Examples:
 	Scan the document for an element with id "findme" and return the element.

	Callback function
  	wcBOOL MyEnumFn(wcCtx* Ctx, wcEL El, int UserIntParm, void *UserVoidParm)
	{
        if (wcELIdMatches(El, "findme"))
		{
            *((wcEL *) UserVoidParm) = El; << return the element and stop
			return wcTRUE;
		}
		else
		return wcFALSE;
	}
	wcEL MyFindFunction(wcCtx* Ctx)
	{
		wcEl answer = 0;
		wcDocEnumerate( Ctx, MyEnumFn, 0, (void *) &answer);
		return answer;
	}
 </pre>


 @return Nothing.

 @see
 */
/*****************************************************************************/

void wcDocEnumerate(wcCtx* Ctx, wcDocEnumFn pfn, int Intparm, void *Voidparm)
{
struct EnumParms E;
	E.Ctx = Ctx;
	E.pfn = pfn;
	E.Intparm = Intparm;
	E.Voidparm = Voidparm;
	HTMLGenericFinder finder(wcDocEnumerateFinderCallback,0,(void **) &E);
	((HTMLDocument *) Ctx->hdoc)->FindElement(&finder);
}


/*****************************************************************************/
// wcDocFindEl
/**

 @memo Search the document for a specific id.
 @doc			Search of the document tree for the first element with the specified id.

 @param  wcCtx*     Ctx    -   The current wc context.
 @param  char*     Id	   -   The id to search for.

 @return		The handle of the element with the specified id, or zero.

 @see
 */
/*****************************************************************************/


wcEL wcDocFindEl (wcCtx* Ctx, char *id)
{
HTMLIdNameTypeIndexFinder8 f(id, 0, HTML_ELEMENT_ANY, 0);
	return (wcEL) (((HTMLDocument *) Ctx->hdoc)->FindElement(&f));
}

/*****************************************************************************/
// wcDocInsertFirst
/**

 @memo  Insert an element into the first position of a document.
 @doc   Insert an element into the first position of a document.

 @param  wcCtx*    		Ctx    -   The current wc context.
 @param  wcEL         	Child  -   The element to insert.


 @return		nothing

 @see
 */
/*****************************************************************************/
void wcDocInsertFirst (wcCtx* Ctx,wcEL eL)
{

	((HTMLDocument *) Ctx->hdoc)->Body()->InsertFirst((HTMLElement *) eL);
}
/*****************************************************************************/
// wcDocInsertLast
/**

 @memo  Insert an element into the last position of a document.
 @doc  Insert an element into the last position of a document.

 @param  wcCtx*    		Ctx    -   The current wc context.
 @param  wcEL         	Child  -   The element to insert.


 @return		nothing

 @see
 */
/*****************************************************************************/
void wcDocInsertLast (wcCtx* Ctx,wcEL eL)
{
	((HTMLDocument *) Ctx->hdoc)->Body()->InsertLast((HTMLElement *) eL);
}


/*****************************************************************************/
// wcDocRefresh
/**

 @memo  Redraw the current document.
 @doc   Redraw the current document.

 @param  wcCtx*    		Ctx    -   The current wc context.

 @return		nothing

 @see
 */
/*****************************************************************************/
void wcDocRefresh (wcCtx* Ctx)
{
	((HTMLDocument *) Ctx->hdoc)->Refresh();
}
