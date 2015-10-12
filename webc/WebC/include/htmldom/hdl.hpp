#ifndef __HDL_HPP__
#define __HDL_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLDefinitionList
* 
*************************************************************************/

class HTMLDefinitionList : public HTMLFlow
{
public:
	HTMLDefinitionList(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLDefinitionList();

	virtual HTMLElementType Type() { return HTML_DEFINITION_LIST_ELEMENT; }
};

#endif // __HDL_HPP__
