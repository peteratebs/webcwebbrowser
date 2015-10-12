#ifndef __HRESET_HPP__
#define __HRESET_HPP__

#include "hibutton.hpp"

/*************************************************************************
* class HTMLInputReset
* 
*************************************************************************/

class HTMLInputReset : public HTMLInputButton
{
public:
	HTMLInputReset(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLInputReset();
	
	virtual HTMLElementType Type() { return HTML_INPUT_RESET_ELEMENT; }
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
};

#endif // __HRESET_HPP__
