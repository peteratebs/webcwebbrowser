//
// REF.CPP - Methods for class ObjectAlias
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
#include "webc.h"
#include "webcmem.h"
#include "webcassert.h"


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

ObjectAlias::ObjectAlias(Referable *ptr, unsigned long count)
{
	mpObject = ptr;
	miUseCount = count;
}

void ObjectAlias::Set(Referable *ptr)
{
	mpObject = ptr;
}

Referable *ObjectAlias::Get(void)
{
	return (mpObject);
}

void ObjectAlias::Claim(void)
{
	// tbd - watch out for integer overflows
	WEBC_ASSERT(miUseCount + 1 > miUseCount);
	miUseCount++;
}

void ObjectAlias::Release(void)
{
	if (miUseCount == 1)
	{
		FreeObject(this);
	}
	else
	{
		miUseCount--;
		WEBC_ASSERT(miUseCount + 1 > miUseCount);
	}
}

ObjectAlias *ObjectAlias::AllocObject(Referable *ptr, unsigned long count)
{
	ObjectAlias *pNew;
	WEBC_NEW_VERBOSE(pNew, ObjectAlias(ptr,count),"ObjectAlias");
	return (pNew);
}

void ObjectAlias::FreeObject(ObjectAlias *alias)
{
	WEBC_DELETE(alias);
}


/*****************************************************************************/
// Referable functions
/*****************************************************************************/

Referable::Referable (void)
{
	mpAlias = 0;
}

Referable::~Referable (void)
{
	if (mpAlias)
	{
		mpAlias->Set(0);
		mpAlias->Release();
	}
}
	
ObjectAlias * Referable::ClaimReference (void)
{
	if (!mpAlias)
	{
		mpAlias = mpAlias->AllocObject((Referable *) this, 2);
	}
	else
	{
		mpAlias->Claim();
	}
	
	return (mpAlias);	
}
