/*
|  JSOBJREF.CPP -
|
|  EBS - WebC
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "webcjhconfig.h"
#include "jsobjref.hpp"
#include "webcassert.h"

#if (WEBC_SUPPORT_JSCRIPT)

#ifdef __cplusplus
extern "C" {
#endif
#include "jsapi.h"
#ifdef __cplusplus
}
#endif
#include "jhutils.h"
#include "wjsbrowser.h"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

extern "C" WebcJSBrowserContext* gJSBrowserContext;

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
JSObjectRef::JSObjectRef (void)
{
	mpJSObject = 0;
}

/*---------------------------------------------------------------------------*/
JSObjectRef::~JSObjectRef (void)
{
	Set(0);
}

/*---------------------------------------------------------------------------*/
JSObject* JSObjectRef::Get (void)
{
	return (mpJSObject);
}

/*---------------------------------------------------------------------------*/
void JSObjectRef::Set (JSObject* obj)
{
	WEBC_ASSERT(gJSBrowserContext);

	if (mpJSObject)
	{
		if (!obj)
		{
			JS_RemoveRoot(gJSBrowserContext->DefaultContext(), &mpJSObject);
		}
		mpJSObject = obj;
	}
	else
	{
		if (obj)
		{
			mpJSObject = obj;
			JS_AddRoot(gJSBrowserContext->DefaultContext(), &mpJSObject);
		}
	}
}

/*---------------------------------------------------------------------------*/
void JSObjectRef::SetPrivate(void* p)
{
	if (mpJSObject)
	{
		jhutil_SetPrivate(gJSBrowserContext->DefaultContext(), mpJSObject, p);
	}
}

/*---------------------------------------------------------------------------*/
void* JSObjectRef::GetPrivate(void)
{
	if (mpJSObject)
	{
		return jhutil_GetPrivate(gJSBrowserContext->DefaultContext(), mpJSObject);
	}
	return (0);
}


#endif // WEBC_SUPPORT_JSCRIPT
