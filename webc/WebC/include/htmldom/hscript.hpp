#ifndef __HSCRIPT_HPP__
#define __HSCRIPT_HPP__

#include "helement.hpp"

/*************************************************************************
* class HTMLScript
* 
*************************************************************************/

class WebcJSScript;

class HTMLScript : public HTMLElementContainer
{
protected:
	WEBC_CHAR*     mpName;
	WEBC_CHAR*     mpSrc;
	WebcCharset   mCharset;	
	WebcJSScript* mpScript;

public:
	HTMLScript(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLScript() ;

	virtual HTMLElementType Type() { return HTML_SCRIPT_ELEMENT; }
	virtual WEBC_UINT16 GetUnitType(void) { return TU_TYPE_NODISPLAY; }
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	
	// Compile and run this script
	int Run(void);
	WebcJSScript *GetJSScript(void);
	
	virtual WEBC_CHAR *Name() { return (mpName);}
	virtual void   SetName(const WEBC_CHAR *pName);
	
	virtual void Update(WEBC_PFBYTE o, WEBC_PFBYTE data);

	void SetSrc(const WEBC_CHAR *pSrc);
	WEBC_CHAR *Src() { return (mpSrc);}
};

#endif // __HSCRIPT_HPP__
