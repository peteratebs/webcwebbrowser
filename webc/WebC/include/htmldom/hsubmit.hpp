#ifndef __HSUBMIT_HPP__
#define __HSUBMIT_HPP__

#include "hibutton.hpp"

/*************************************************************************
* class HTMLInputSubmit
* 
*************************************************************************/

class HTMLInputSubmit : public HTMLInputButton
{
public:
	HTMLInputSubmit(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLInputSubmit();
	
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
};

#endif // __HSUBMIT_HPP__
