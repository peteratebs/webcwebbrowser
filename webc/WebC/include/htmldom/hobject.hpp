#ifndef __HOBJECT_HPP__
#define __HOBJECT_HPP__


/*************************************************************************
* class HTMLObject
* 
*************************************************************************/


class HTMLObject : public HTMLElement
{

public:
#if (WEBC_SUPPORT_JSCRIPT)
	HTMLObject(HTMLTagParse *td = 0, HTMLDocument *pDocument = 0, JSObject *pJSObj = 0);
#else
	HTMLObject(HTMLTagParse *td = 0, HTMLDocument *pDocument = 0);
#endif // WEBC_SUPPORT_JSCRIPT	
	virtual ~HTMLObject();

};


#endif // __HOBJECT_HPP__
