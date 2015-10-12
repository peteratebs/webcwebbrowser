#ifndef __PWEBWIN_HPP__
#define __PWEBWIN_HPP__

#include "webc.h"
#include "peg.hpp"

class PegWebWindow : public PegDecoratedWindow
{
 public:
	 PegWebWindow (PegRect &Rect);

	PegWebWindow (
			PegRect &Rect,
			void* privateData, 
			char *pURL=NULL
		);
	~PegWebWindow();

    virtual PEGINT Message(const PegMessage &Mesg);
    virtual void Draw(void);

	HBROWSER_HANDLE mBrowser;
};

#endif // __PWEBWIN_HPP__
