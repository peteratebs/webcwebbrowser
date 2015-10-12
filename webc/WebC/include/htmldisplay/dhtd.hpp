#ifndef __DHTD_HPP__
#define __DHTD_HPP__

#include "htd.hpp"
#include "dflow.hpp"

#if (WEBC_SUPPORT_TABLES)

class HTMLTableCellDisplay : public HTMLFlowDisplay
{
protected:
	virtual void UpdateHeight (
		DISPLAY_INT topPad,
		DISPLAY_INT bottomPad
	);
	
public:
	HTMLTableCellDisplay(HTMLTableCell * td);
		
	virtual ~HTMLTableCellDisplay();
	
	// methods overridden from DisplayElement
	
	// methods overriden from TextUnit
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	
	// class-specific methods
	void SetTdElement(HTMLTableCell * td);
	void FormatTableCell (
			DISPLAY_INT cellWidth			
		);
};

#endif // WEBC_SUPPORT_TABLES

#endif // __DHTD_HPP__
