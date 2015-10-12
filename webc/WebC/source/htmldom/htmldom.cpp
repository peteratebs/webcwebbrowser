//
// HTMLDOM.CPP - Misc DOM related data/classes/functions
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "webc.h"
#include "webcstr.h"
#include "html.h"
#include "htmldom.hpp"
#include "htmldoc.hpp"
#include "css.h"
#include "webfonts.hpp"
#include "webload.hpp"
#include "wgkeys.hpp"
#include "htmlbrow.hpp"
#include "hedit.hpp"
#include "strbld.hpp"
#include "htmlfind.hpp"
#include "wcconst.hpp"
#include "hhidden.hpp"
#include "hpasswd.hpp"
#include "hchkbox.hpp"
#include "webcmem.h"

#include "webcassert.h"
#include "rtpprint.h"


/*****************************************************************************/
// Type definitions
/*****************************************************************************/


/*****************************************************************************/
// Global data
/*****************************************************************************/

WEBC_READONLY WEBC_BOOL TextUnitIsContainer[NUM_HTML_ELEMENT_TYPES] =
{
	1,//    HTML_ADDRESS_ELEMENT,
	1,//	HTML_ANCHOR_ELEMENT,
	0,// 	HTML_AREA_ELEMENT,
	0,// 	HTML_BASE_ELEMENT
	1,//    HTML_BIG_ELEMENT,
	1,//    HTML_BLOCKQUOTE_ELEMENT,
	1,//	HTML_BODY_ELEMENT,
	1,//	HTML_BOLD_ELEMENT,
	0,//	HTML_BR_ELEMENT,
	1,//	HTML_BUTTON_ELEMENT,
	1,//	HTML_CENTER_ELEMENT,
	0,//	HTML_CHECKBOX_ELEMENT,
	1,//    HTML_CITE_ELEMENT;
	1,//    HTML_CODE_ELEMENT;
	1,//    HTML_DEFINITION_ELEMENT,
	1,//    HTML_DEFINITION_LIST_ELEMENT,
	1,//    HTML_DEFINITION_TERM_ELEMENT,
	1,//    HTML_DEL_ELEMENT,
	1,//	HTML_DIV_ELEMENT,
	0,//	HTML_EDIT_STR_ELEMENT,
	0,//	HTML_EDITBOX_ELEMENT,
	0,//	HTML_ELEMENT=0,
#if (WEBC_SUPPORT_EMBED	)
	1,//	HTML_EMBED_ELEMENT //61 It is numbered from 0 or 1
#endif
	1,//	HTML_EMPHASIZE_ELEMENT,
	1,//	HTML_FLOW_ELEMENT,
	1,//	HTML_FONT_ELEMENT,
	1,//    HTML_FORM_ELEMENT
	0,//	HTML_FRAME_ELEMENT,
	1,//	HTML_FRAMESET_ELEMENT,
	1,//	HTML_HEADING_ELEMENT,
	0,//	HTML_HIDDEN_INPUT_ELEMENT,
	0,//	HTML_HORIZONTAL_RULE_ELEMENT,
	0,//    HTML_IFRAME_ELEMENT,
	0,//	HTML_IMAGE_ELEMENT,
	0,//    HTML_INPUT_BUTTON_ELEMENT,
	0,//    HTML_INPUT_PASSWORD_ELEMENT,
	0,//    HTML_INPUT_RESET_ELEMENT,
	1,//    HTML_INS_ELEMENT,
	1,//	HTML_ITALIC_ELEMENT,
	1,//	HTML_KBD_ELEMENT,
	1,//    HTML_LABEL_ELEMENT,
	1,//	HTML_LIST_ELEMENT,
	1,//	HTML_LIST_ITEM_ELEMENT,
	1,//	HTML_MAP_ELEMENT,
	1,//	HTML_NOBR_ELEMENT,
	1,//	HTML_OPTION_ELEMENT,
	1,//	HTML_PARAGRAPH_ELEMENT,
	1,//	HTML_PRE_ELEMENT,
	0,//	HTML_RADIO_BUTTON_ELEMENT,
	1,//	HTML_SCRIPT_ELEMENT,
	1,//	HTML_SELECT_ELEMENT,
	1,//    HTML_SMALL_ELEMENT,
	0,//    HTML_SPACER_ELEMENT,
	1,//    HTML_SPAN_ELEMENT,
	1,//	HTML_STRIKE_ELEMENT,
	0,//	HTML_STRING_ELEMENT,
	1,//	HTML_STRONG_ELEMENT,
	1,//	HTML_STYLE_ELEMENT,
	1,//	HTML_TABLE_ELEMENT,
	1,//	HTML_TABLE_CELL_ELEMENT,
	1,//	HTML_TABLE_ROW_ELEMENT,
	1,//	HTML_UNDERLINE_ELEMENT,

};

WEBC_READONLY HTMLElementType TagToHTMLElementType[HTML_NUM_TAGS] =
{
	HTML_ANCHOR_ELEMENT,          //HTML_TAG_A=0,
	HTML_ELEMENT,                 //HTML_TAG_ABBR
	HTML_ELEMENT,                 //HTML_TAG_ACRONYM
	HTML_ADDRESS_ELEMENT,         //HTML_TAG_ADDRESS
	HTML_ELEMENT,                 //HTML_TAG_APPLET
	HTML_AREA_ELEMENT,            //HTML_TAG_AREA,
 	HTML_BOLD_ELEMENT,            //HTML_TAG_B,
	HTML_BASE_ELEMENT,            //HTML_TAG_BASE,
	HTML_ELEMENT,                 //HTML_TAG_BASEFONT
	HTML_ELEMENT,                 //HTML_TAG_BDO
	HTML_BIG_ELEMENT,             //HTML_TAG_BIG,
	HTML_BLOCKQUOTE_ELEMENT,      //HTML_TAG_BLOCKQUOTE,
	HTML_BODY_ELEMENT,            //HTML_TAG_BODY,
	HTML_BR_ELEMENT,              //HTML_TAG_BR,
	HTML_BUTTON_ELEMENT,          //HTML_TAG_BUTTON
	HTML_ELEMENT,                 //HTML_TAG_CAPTION,
	HTML_CENTER_ELEMENT,          //HTML_TAG_CENTER,
	HTML_CITE_ELEMENT,            //HTML_TAG_CITE
	HTML_CODE_ELEMENT,            //HTML_TAG_CODE
	HTML_ELEMENT,                 //HTML_TAG_COL
	HTML_ELEMENT,                 //HTML_TAG_COLGROUP
	HTML_DEFINITION_ELEMENT,      //HTML_TAG_DD,
	HTML_DEL_ELEMENT,             //HTML_TAG_DEL
	HTML_ELEMENT,                 //HTML_TAG_DFN
	HTML_ELEMENT,                 //HTML_TAG_DIR,
	HTML_DIV_ELEMENT,             //HTML_TAG_DIV,
	HTML_DEFINITION_LIST_ELEMENT, //HTML_TAG_DL,
	HTML_DEFINITION_TERM_ELEMENT, //HTML_TAG_DT,
#if (WEBC_SUPPORT_EMBED	)
	HTML_EMBED_ELEMENT,          //HTML_TAG_EMBED
#endif
	HTML_EMPHASIZE_ELEMENT,       //HTML_TAG_EM,
	HTML_ELEMENT,                 //HTML_TAG_FIELDSET
	HTML_FONT_ELEMENT,            //HTML_TAG_FONT,
	HTML_FORM_ELEMENT,            //HTML_TAG_FORM,
	HTML_FRAME_ELEMENT,           //HTML_TAG_FRAME,
	HTML_FRAMESET_ELEMENT,        //HTML_TAG_FRAMESET,
	HTML_HEADING_ELEMENT,         //HTML_TAG_H1,
	HTML_HEADING_ELEMENT,         //HTML_TAG_H2,
	HTML_HEADING_ELEMENT,         //HTML_TAG_H3,
	HTML_HEADING_ELEMENT,         //HTML_TAG_H4,
	HTML_HEADING_ELEMENT,         //HTML_TAG_H5,
	HTML_HEADING_ELEMENT,         //HTML_TAG_H6,
	HTML_HORIZONTAL_RULE_ELEMENT, //HTML_TAG_HR,
	HTML_ELEMENT,                 //HTML_TAG_HTML
	HTML_ITALIC_ELEMENT,          //HTML_TAG_I,
	HTML_IFRAME_ELEMENT,          //HTML_TAG_IFRAME,
	HTML_IMAGE_ELEMENT,           //HTML_TAG_IMG,
	HTML_INPUT_BUTTON_ELEMENT,    //HTML_TAG_INPUT,
	HTML_INS_ELEMENT,             //HTML_TAG_INS
	HTML_ELEMENT,                 //HTML_TAG_ISINDEX
	HTML_KBD_ELEMENT,             //HTML_TAG_KBD
	HTML_LABEL_ELEMENT,           //HTML_TAG_LABEL
	HTML_ELEMENT,                 //HTML_TAG_LEGEND
	HTML_LIST_ITEM_ELEMENT,       //HTML_TAG_LI,
	HTML_ELEMENT,                 //HTML_TAG_LINK,
	HTML_MAP_ELEMENT,             //HTML_TAG_MAP,
	HTML_ELEMENT,                 //HTML_TAG_MENU,
	HTML_ELEMENT,                 //HTML_TAG_META,
	HTML_NOBR_ELEMENT,            //HTML_TAG_NOBR,
	HTML_ELEMENT,                 //HTML_TAG_NOFRAMES,
	HTML_ELEMENT,                 //HTML_TAG_NOSCRIPT,
	HTML_ELEMENT,                 //HTML_TAG_OBJECT
	HTML_LIST_ELEMENT,            //HTML_TAG_OL,
	HTML_ELEMENT,                 //HTML_TAG_OPTGROUP
	HTML_OPTION_ELEMENT,          //HTML_TAG_OPTION,
	HTML_PARAGRAPH_ELEMENT,       //HTML_TAG_P,
	HTML_ELEMENT,                 //HTML_TAG_PARAM
	HTML_PRE_ELEMENT,             //HTML_TAG_PRE,
	HTML_ELEMENT,                 //HTML_TAG_Q
	HTML_STRIKE_ELEMENT,          //HTML_TAG_S
	HTML_CODE_ELEMENT,            //HTML_TAG_SAMP
	HTML_SCRIPT_ELEMENT,          //HTML_TAG_SCRIPT,
	HTML_SELECT_ELEMENT,          //HTML_TAG_SELECT,
	HTML_SMALL_ELEMENT,           //HTML_TAG_SMALL,
	HTML_SPACER_ELEMENT,          //HTML_TAG_SPACER,
	HTML_SPAN_ELEMENT,            //HTML_TAG_SPAN,
	HTML_STRIKE_ELEMENT,          //HTML_TAG_STRIKE,
	HTML_STRONG_ELEMENT,          //HTML_TAG_STRONG,
	HTML_STYLE_ELEMENT,           //HTML_TAG_STYLE,
	HTML_ELEMENT,                 //HTML_TAG_SUB
	HTML_ELEMENT,                 //HTML_TAG_SUP
	HTML_TABLE_ELEMENT,           //HTML_TAG_TABLE,
	HTML_ELEMENT,                 //HTML_TAG_TBODY
	HTML_TABLE_CELL_ELEMENT,      //HTML_TAG_TD,
	HTML_EDITBOX_ELEMENT,         //HTML_TAG_TEXTAREA,
	HTML_ELEMENT,                 //HTML_TAG_TFOOT
	HTML_HEADING_ELEMENT,         //HTML_TAG_TH,
	HTML_ELEMENT,                 //HTML_TAG_THEAD
	HTML_ELEMENT,                 //HTML_TAG_TITLE,
	HTML_TABLE_ROW_ELEMENT,       //HTML_TAG_TR,
	HTML_CODE_ELEMENT,            //HTML_TAG_TT
	HTML_UNDERLINE_ELEMENT,       //HTML_TAG_U,
	HTML_LIST_ELEMENT,            //HTML_TAG_UL,
	HTML_ITALIC_ELEMENT,          //HTML_TAG_VAR
	HTML_STRING_ELEMENT,          //HTML_TAG_STRING not a real tag just text
};


#if (WEBC_SUPPORT_USER_EVENTS)

/*****************************************************************************/
// Event Handler table
/*****************************************************************************/

HTMLEventHandler *gEventHandlerTable[WEBC_CFG_MAX_EVENT_HANDLERS] =
{
	0
};

#endif // WEBC_SUPPORT_USER_EVENTS


/*****************************************************************************/
// HTMLEventQueue methods
/*****************************************************************************/

HTMLEventQueue::HTMLEventQueue()
{
	mbHead = 0;
	mbTail = 0;
}

HTMLEventQueue::~HTMLEventQueue()
{
}

void HTMLEventQueue::Fold(HTMLEvent *pEvent, HTMLElement *pElem)
{
	if (pElem)
	{
		int iInsert;

		iInsert = mbHead;
		while (iInsert != mbTail)
		{
			// If we find a message of the same type bound for this element, just replace it
			if ((mpQueue[iInsert].event.type == pEvent->type) && (mpQueue[iInsert].element == pElem))
			{
				mpQueue[iInsert].event = *pEvent;
				return;
			}

			iInsert++;
			if (iInsert >= WEBC_CFG_EVENT_QUEUE_SIZE)
			{
				iInsert -= WEBC_CFG_EVENT_QUEUE_SIZE;
			}
		}

		// If no match is found, then just enqueue this event
		Enqueue(pEvent, pElem);
	}
}

int HTMLEventQueue::Enqueue(HTMLEvent *pEvent, HTMLElement *pElem)
{
	if ( (mbTail == WEBC_CFG_EVENT_QUEUE_SIZE-1 && mbHead == 0) ||
		 (mbTail + 1 == mbHead) ||
		 !pElem)
	{
		//if the queue is full drop this event
		WEBC_ASSERT(0);
		return -1;
	}

	mpQueue[mbTail].event = *pEvent;
	mpQueue[mbTail].element = pElem;
	mbTail++;
	if (mbTail == WEBC_CFG_EVENT_QUEUE_SIZE)
	{
		mbTail = 0;
	}

	return 0;
}

int HTMLEventQueue::Dequeue(HTMLEvent **pEvent, HTMLElement **pElem)
{
	if (mbHead != mbTail)
	{
		*pEvent = &(mpQueue[mbHead].event);
		*pElem = mpQueue[mbHead].element;
		mbHead++;
		if (mbHead == WEBC_CFG_EVENT_QUEUE_SIZE)
			mbHead = 0;
		return 0;
	}
	else
	{
		//queue is empty
		*pEvent = 0;
		*pElem = 0;
		return -1;
	}
}

void HTMLEventQueue::Purge(HTMLElement *pElem)
{
int iTestNode,iCopyNode,iNextNode;

	iTestNode = mbHead;

	while (iTestNode != mbTail)
	{
		if (mpQueue[iTestNode].element == pElem)
		{
			iCopyNode = iTestNode;
			while (iCopyNode != mbTail)
			{
				iNextNode = iCopyNode + 1;
				if (iNextNode >= WEBC_CFG_EVENT_QUEUE_SIZE)
				{
					iNextNode -= WEBC_CFG_EVENT_QUEUE_SIZE;
				}
				mpQueue[iCopyNode] = mpQueue[iNextNode];
				iCopyNode = iNextNode;
			}

			mbTail--;
			if (mbTail == 255)
			{
				mbTail += WEBC_CFG_EVENT_QUEUE_SIZE;
			}
		}
		else
		{
			iTestNode++;
			if (iTestNode >= WEBC_CFG_EVENT_QUEUE_SIZE)
			{
				iTestNode -= WEBC_CFG_EVENT_QUEUE_SIZE;
			}
		}
	}
}

void HTMLEventQueue::CopyNode(int start, int finish)
{
	for (; start < finish; start++)
	{
		mpQueue[start] = mpQueue[start + 1];
	}
}


/*****************************************************************************/
// Misc. Functions
/*****************************************************************************/

void HTML_ResetEventHandlers(void)
{
  #if (WEBC_SUPPORT_USER_EVENTS)
	int i;

	for (i=0; i<WEBC_CFG_MAX_EVENT_HANDLERS; i++)
	{
		gEventHandlerTable[i] = 0;
	}
  #endif // WEBC_SUPPORT_USER_EVENTS
}

void HTML_UnregisterEventHandler(HTMLEventHandler *pHandler)
{
  #if (WEBC_SUPPORT_USER_EVENTS)
	int i;

	for (i=0; i<WEBC_CFG_MAX_EVENT_HANDLERS; i++)
	{
		if (gEventHandlerTable[i] == pHandler)
			gEventHandlerTable[i] = 0;
	}
  #endif // WEBC_SUPPORT_USER_EVENTS
}

int HTML_RegisterEventHandler(HTMLEventHandler *pHandler)
{
  #if (WEBC_SUPPORT_USER_EVENTS)
	int i;

	for (i=0; i<WEBC_CFG_MAX_EVENT_HANDLERS; i++)
	{
		if (gEventHandlerTable[i] == 0)
		{
			gEventHandlerTable[i] = pHandler;
			return (0);
		}
	}
  #endif // WEBC_SUPPORT_USER_EVENTS

	return (-1);
}

HTMLEventHandler *HTML_FindEventHandler(WEBC_CHAR *pName, HTMLElement *pElem)
{
  #if (WEBC_SUPPORT_USER_EVENTS)
	int i;

	if (!pName)
	{
		return (0);
	}

	for (i=0; i<WEBC_CFG_MAX_EVENT_HANDLERS; i++)
	{
		if (gEventHandlerTable[i] && !webc_stricmp(gEventHandlerTable[i]->Name(), pName))
		{
			return (gEventHandlerTable[i]);
		}
	}
  #endif // WEBC_SUPPORT_USER_EVENTS

	return (0);
}




void HTML_GetDisplayElementManagerPosition (DisplayElement* display, DISPLAY_INT* x, DISPLAY_INT* y)
{
	if (display->mpParent)
	{
		display->mpParent->GetDisplayPosition(display, x, y);
	}
	else
	{
		*x = display->mRect.left;
		*y = display->mRect.top;
	}

	DisplayManager* manager = display->GetManager();
	if (manager)
	{
		WebRect padding;

		manager->GetPaddingWidth (&padding);

		*x += padding.left - manager->mViewRect.left;
		*y += padding.top  - manager->mViewRect.top;
	}
}

void HTML_GetElementManagerPosition  (HTMLElement* elem, DISPLAY_INT* x, DISPLAY_INT* y)
{
	DisplayElement* display = elem->GetDisplayElement(WEBC_FALSE);
	if (display)
	{
		HTML_GetDisplayElementManagerPosition(display, x, y);
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}

void HTML_GetElementViewPortPosition (HTMLElement* elem, DISPLAY_INT* x, DISPLAY_INT* y)
{
	DisplayElement* display = elem->GetDisplayElement(WEBC_FALSE);
	if (display && display->mpParent)
	{
		DisplayManager* manager = display->GetManager();
		if (manager)
		{
			display->mpParent->GetDisplayPosition(display, x, y);
			x -= manager->mViewRect.left;
			y -= manager->mViewRect.top;
			return;
		}
	}

	*x = 0;
	*y = 0;
}

void HTML_GetElementDocumentPosition (HTMLElement* elem, DISPLAY_INT* x, DISPLAY_INT* y)
{
	DisplayElement* display = elem->GetDisplayElement(WEBC_FALSE);
	if (display && display->mpParent)
	{
		display->mpParent->GetDisplayPosition(display, x, y);
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}

void HTML_GetDisplayElementScreenPosition (DisplayElement* display, DISPLAY_INT* x, DISPLAY_INT* y)
{
	DISPLAY_INT elementManagerX;
	DISPLAY_INT elementManagerY;
	DISPLAY_INT managerScreenX = 0;
	DISPLAY_INT managerScreenY = 0;

	HTML_GetDisplayElementManagerPosition (display, &elementManagerX, &elementManagerY);

	DisplayManager* manager = display->GetManager();

	if (manager && (DisplayElement*) manager != display)
	{
		HTML_GetDisplayElementScreenPosition (manager, &managerScreenX, &managerScreenY);
		if (!manager->GetManager())
		{
			managerScreenX += manager->mRect.left;
			managerScreenY += manager->mRect.top;
		}
	}

	*x = managerScreenX + elementManagerX;
	*y = managerScreenY + elementManagerY;
}

void HTML_GetElementScreenPosition   (HTMLElement* elem, DISPLAY_INT* x, DISPLAY_INT* y)
{
	DisplayElement* display = elem->GetDisplayElement(WEBC_FALSE);
	if (display)
	{
		HTML_GetDisplayElementScreenPosition(display, x, y);
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}

int check_hash_entries()
{
return (0);

}


