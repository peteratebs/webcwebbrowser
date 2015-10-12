//
// wccreateelement.CPP -
//
// EBS - WebC
//
// Copyright EBS Inc. , 2011
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

//#include "../include/NewApi.h"
#include "wcapi.h"
//#include "dflow.hpp"
#include "helement.hpp"
//#include "htmlbrow.hpp"
//#include "webcassert.h"
//#include "htmlfind.hpp"
//#include "htmldoc.hpp"
#include "webcmem.h"
//#include "rtptime.h"

#include "haddress.hpp"
#include "hbase.hpp"
#include "hbig.hpp"
#include "hblquote.hpp"
#include "hcite.hpp"
#include "hcode.hpp"
#include "hdel.hpp"
#include "hemphsz.hpp"
#include "hfont.hpp"
#include "hheading.hpp"
#include "hhr.hpp"
#include "hins.hpp"
#include "hitalic.hpp"
#include "hkbd.hpp"
#include "hlabel.hpp"
#include "hnobr.hpp"
#include "hpre.hpp"
#include "hscript.hpp"
#include "hsmall.hpp"
#include "hstrike.hpp"
#include "hstrong.hpp"
#include "htr.hpp"
#include "hstring.hpp"
#include "hundline.hpp"
#include "htd.hpp"
#include "hedit.hpp"
#include "hli.hpp"
#include "hlist.hpp"
#include "hradio.hpp"
#include "hform.hpp"
#include "helement.hpp"
#include "rtpprint.h"
#include "htmldoc.hpp"
#include "hanchor.hpp"
#include "hbody.hpp"
//#include "hcoll.hpp"
//#include "hdoc.hpp"
#include "helement.hpp"
#include "hform.hpp"
#include "hflow.hpp"
#include "hframe.hpp"
#include "hframset.hpp"
#include "hiframe.hpp"
#include "himage.hpp"
#include "hinput.hpp"
#include "hoption.hpp"
#include "hselect.hpp"
#include "htable.hpp"
#include "htxtarea.hpp"
#include "htmlbrow.hpp"
//#include "htitle.hpp"
//#include "htcell.hpp"
#include "hmap.hpp"
#include "harea.hpp"
#include "hbutton.hpp"

#ifdef RTP_TRACK_LOCAL_MEMORY
HTMLElement *_wcCreateElement(HTMLElementType eType, HTMLTagParse *tag, HTMLDocument *pDoc, const char *file, const long line)
#else
#define file 0
#define line 0
HTMLElement *wcCreateElement(HTMLElementType eType, HTMLTagParse *tag, HTMLDocument *pDoc)
#endif
{
    HTMLElement *pElement = 0; /* Set by NEWELEMENT(OFTYPE) */
	switch (eType)
	{
        case HTML_ADDRESS_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLAddress(tag, pDoc),file,line,"HTMLAddress");
			break;
        case HTML_ANCHOR_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLAnchor(tag, pDoc),file,line,"HTMLAnchor");
			break;
        case HTML_AREA_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLArea(tag, pDoc),file,line,"HTMLArea");
			break;
        case HTML_BASE_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLBase(tag, pDoc),file,line,"HTMLBase");
			break;
        case HTML_BIG_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLBig(tag, pDoc),file,line,"HTMLBig");
			break;
        case HTML_BLOCKQUOTE_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLBlockquote(tag, pDoc),file,line,"HTMLBlockquote");
			break;
        case HTML_CITE_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLCite(tag, pDoc),file,line,"HTMLCite");
			break;
        case HTML_CODE_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLCode(tag, pDoc),file,line,"HTMLCode");
			break;
        case HTML_DEL_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLDel(tag, pDoc),file,line,"HTMLDel");
			break;
        case HTML_DIV_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLDiv(tag, pDoc),file,line,"HTMLDiv");
			break;
        case HTML_EDIT_STR_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLEditString(tag, pDoc),file,line,"HTMLEditString");
			break;
        case HTML_EDITBOX_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLTextArea(tag, pDoc),file,line,"HTMLTextArea");
			break;
        case HTML_EMPHASIZE_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLEmphasize(tag, pDoc),file,line,"HTMLEmphasize");
			break;
        case HTML_FLOW_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLFlow(tag, pDoc),file,line,"HTMLFlow");
			break;
        case HTML_FONT_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLFont(tag, pDoc),file,line,"HTMLFont");
			break;
        case HTML_FORM_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLForm(tag, pDoc),file,line,"HTMLForm");
			break;
	  #if (WEBC_SUPPORT_FRAMES)
        case HTML_FRAME_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLFrame(tag, pDoc),file,line,"HTMLFrame");
			break;
        case HTML_FRAMESET_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLFrameSet(tag, pDoc),file,line,"HTMLFrameSet");
			break;
      #endif
        case HTML_HEADING_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLHeading(tag, pDoc),file,line,"HTMLHeading");
			break;
        case HTML_HORIZONTAL_RULE_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLHorizontalRule(tag, pDoc),file,line,"HTMLHorizontalRule");
			break;
        case HTML_IMAGE_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLImage(tag, pDoc),file,line,"HTMLImage");
			break;
        case HTML_INPUT_BUTTON_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLEditString(tag, pDoc),file,line,"HTMLEditString");
			break;
        case HTML_INS_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLIns(tag, pDoc),file,line,"HTMLIns");
			break;
        case HTML_ITALIC_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLItalic(tag, pDoc),file,line,"HTMLItalic");
			break;
        case HTML_KBD_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLKbd(tag, pDoc),file,line,"HTMLKbd");
			break;
        case HTML_LIST_ITEM_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLListItem(tag, pDoc),file,line,"HTMLListItem");
			break;
        case HTML_LIST_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLList(tag, pDoc),file,line,"HTMLList");
			break;
        case HTML_LABEL_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLLabel(tag, pDoc),file,line,"HTMLLabel");
			break;
        case HTML_MAP_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLMap(tag, pDoc),file,line,"HTMLMap");
			break;
        case HTML_NOBR_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLNoBr(tag, pDoc),file,line,"HTMLNoBr");
			break;
        case HTML_OPTION_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLOption(tag, pDoc),file,line,"HTMLOption");
			break;
        case HTML_PARAGRAPH_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLParagraph(tag, pDoc),file,line,"HTMLParagraph");
			break;
        case HTML_PRE_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLPreformatted(tag, pDoc),file,line,"HTMLPreformatted");
			break;
        case HTML_RADIO_BUTTON_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLRadioButton(tag, pDoc),file,line,"HTMLRadioButton");
			break;
        case HTML_SCRIPT_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLScript(tag, pDoc),file,line,"HTMLScript");
			break;
        case HTML_SELECT_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLSelect(tag, pDoc),file,line,"HTMLSelect");
			break;
        case HTML_SMALL_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLSmall(tag, pDoc),file,line,"HTMLSmall");
			break;
        case HTML_SPAN_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLSpan(tag, pDoc),file,line,"HTMLSpan");
			break;
        case HTML_STRIKE_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLStrike(tag, pDoc),file,line,"HTMLStrike");
			break;
        case HTML_STRONG_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLStrong(tag, pDoc),file,line,"HTMLStrong");
			break;
        case HTML_TABLE_CELL_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLTableCell(tag, pDoc),file,line,"HTMLTableCell");
			break;
        case HTML_TABLE_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLTable(tag, pDoc),file,line,"HTMLTable");
			break;
        case HTML_TABLE_ROW_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLTableRow(tag, pDoc),file,line,"HTMLTableRow");
			break;
        case HTML_UNDERLINE_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLUnderline(tag, pDoc),file,line,"HTMLUnderline");
			break;
	  #if (WEBC_SUPPORT_FRAMES)
	  #if (WEBC_SUPPORT_IFRAMES)
		case HTML_IFRAME_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLInlineFrame(tag, pDoc),file,line,"HTMLInlineFrame");
			break;
	  #endif // WEBC_SUPPORT_IFRAMES
	  #endif // WEBC_SUPPORT_FRAMES
        case HTML_STRING_ELEMENT:
			WEBC_DEBUG_NEW(pElement, HTMLString( tag, pDoc),file,line,"HTMLString");
			break;
		case HTML_STYLE_ELEMENT:
		case HTML_BUTTON_ELEMENT:
		case HTML_HIDDEN_INPUT_ELEMENT:
		case HTML_ELEMENT_NONE:
		case HTML_ELEMENT:
		default:
            break;
	}
    return (pElement);
}

