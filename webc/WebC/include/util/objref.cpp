//
// OBJREF.CPP - Methods for class ObjectAlias
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

#include "ref.hpp"
#ifndef BUILDING_WIGITS_ONLY
#include "htmldoc.hpp"
#include "hform.hpp"
#endif
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
// ObjectAlias functions
/*****************************************************************************/

template<class T> ObjectReference<T>::ObjectReference(void)
{
	mpAlias = 0;
}

template<class T> ObjectReference<T>::ObjectReference(T *obj)
{
	mpAlias = 0;
	Set(obj);
}

template<class T> ObjectReference<T>::~ObjectReference(void)
{
	if (mpAlias)
	{
		mpAlias->Release();
	}
}

template<class T> T *ObjectReference<T>::Get(void)
{
	return ((mpAlias)? ((T *) mpAlias->Get()) : 0);
}

template<class T> void ObjectReference<T>::Set(T *obj)
{
	if (mpAlias)
	{
		mpAlias->Release();
	}

	mpAlias = (obj)? obj->ClaimReference() : 0;
}
