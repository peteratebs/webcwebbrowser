#ifndef __HDT_HPP__
#define __HDT_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLDefinitionTerm
* 
*************************************************************************/

class HTMLDefinitionTerm : public HTMLFlow
{
public:
	HTMLDefinitionTerm(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLDefinitionTerm();

	virtual HTMLElementType Type() { return HTML_DEFINITION_TERM_ELEMENT; }
};

#endif // __HDT_HPP__
