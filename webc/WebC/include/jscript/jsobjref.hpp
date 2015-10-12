/*
|  JSOBJREF.H - 
| 
|  EBS - WebC
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef _JSOBJREF_H_
#define _JSOBJREF_H_

#if (WEBC_SUPPORT_JSCRIPT)

struct JSContext;
struct JSObject;

class JSObjectRef 
{
public:
	JSObjectRef ();
	~JSObjectRef ();
	
	JSObject* Get(void);
	void Set(JSObject* jsobj);
	void SetPrivate(void* p);
	void* GetPrivate(void);
	
private:
	JSObject *mpJSObject;
};

#endif //WEBC_SUPPORT_JSCRIPT

#endif //_JSOBJREF_H_
