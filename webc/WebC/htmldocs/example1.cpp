/*****************************************************************************/
// wcElxClone
/**

 @memo  Create an element by cloning.
 @doc Dynamically create a new element by cloning an existing element. If bCloneChildren is true, clone the element's children too.
 The element must be inserted into the document before much can be done with it.
  Note:

 @param  wcEL 	El			    -   The element to clone.
 @param  wcBOOL	bCloneChildren	-	wcTRUE or wcFALSE

 @return The new element or zero on failure.

 @see
 */
/*****************************************************************************/


wcEL wcElxClone(wcEL element, wcBOOL bCloneChildren)
{
#if (WEBC_SUPPORT_JSCRIPT)
	return (wcEL) jhutil_CloneElement((HTMLElement *) element, (WEBC_BOOL) bCloneChildren);
#else
	return 0;
#endif
}

// @memo Hello from examples.xxx
