/*
	templateinst.cpp

	This file provides bodies for virual methods that are never accessed but are still required.

	This overcomes a problem with version 2.96 of the GNU compiler used for vxworks from the compiler's
	template handling

	For certain versions of the GNU compiler this file must be included in the library and linked with the
	application.

*/

#include "dhelem.hpp"
#include "dhelem.cpp"
#include "delement.hpp"
#include "delement.cpp"

template class HTMLElementDisplay<DisplayElement>;
