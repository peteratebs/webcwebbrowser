#ifndef __HFORM_HPP__
#define __HFORM_HPP__

#include "hspan.hpp"

/*************************************************************************
* class HTMLForm
* 
*************************************************************************/

/*Encoding Types*/

typedef enum e_HTMLFormEncodingType
{
	HTML_ENCTYPE_URLENCODED = 0,
	HTML_ENCTYPE_MULTIPART_FORM_DATA,
	HTML_NUM_ENCTYPES
}
HTMLFormEncodingType;

typedef enum e_HTMLFormMethod
{
	HTML_FORM_METHOD_GET = 0,
	HTML_FORM_METHOD_POST,
	HTML_NUM_FORM_METHODS
}
HTMLFormMethod;

// hform.cpp
class HTMLForm : public HTMLSpan
{
private:
	int AssemblePostQuery   (WEBC_CHAR* query, WEBC_CHAR* dest);
	int AssembleQueryString (WEBC_CHAR* buffer, HTMLElement* submit, int x, int y);
	int PostQuery           (WEBC_CHAR* query);
	
	WebcCharset          mAcceptCharset;
	WEBC_CHAR*           mAction;
	HTMLFormEncodingType mEncType;
	vector               mFieldVector;
	HTMLFormMethod       mMethod;
	WEBC_CHAR*           mName;
	WEBC_CHAR*           mTarget;
	
public:
	HTMLForm (HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLForm (void);

	// Overridden from HTMLElement
	virtual HTMLElementType Type() { return HTML_FORM_ELEMENT; }
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);

	virtual WEBC_CHAR*       Action        (void) { return mAction; }
	virtual WebcCharset      AcceptCharset (void);
	virtual const WEBC_CHAR* Method        (void) { return ((mMethod == HTML_FORM_METHOD_GET)? WEBC_STR_GET:WEBC_STR_POST); }
	virtual WEBC_CHAR*       Name          (void) { return mName; }
	virtual WEBC_CHAR*       Target        (void) { return mTarget; }

	virtual void SetAcceptCharset (const WEBC_CHAR *value);
	virtual void SetAction        (const WEBC_CHAR* action);
	virtual void SetMethod        (const WEBC_CHAR* method);
	virtual void SetName          (const WEBC_CHAR* name);	
	virtual void SetTarget        (const WEBC_CHAR* target);	
	virtual void SetEncType (WEBC_CHAR *strType);
	virtual const char* EncType ();

	void         AddField      (HTMLElement* field);
	WEBC_BOOL    ContainsField (HTMLElement* field);
	HTMLElement* FindField     (HTMLElementFinder* finder);
	void         RemoveField   (HTMLElement* field);

	vector* GetFieldVector(void) { return &mFieldVector; }

	virtual int  Submit (HTMLElement* submit, int x, int y);
	virtual void Reset  (void);
};

#endif // __HFORM_HPP__
