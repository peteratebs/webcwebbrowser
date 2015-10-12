//
// HTMLBLD.CPP - Methods for HTML DOM helper class HtmlDocumentBuilder
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
// Header Files
/*****************************************************************************/
#include "webc.h"
#include "webcjhconfig.h"
#include "htmldom.hpp"
#include "htmlbld.hpp"
#include "htmlfind.hpp"
#include "haddress.hpp"
#include "harea.hpp"
#include "hanchor.hpp"
#include "hbase.hpp"
#include "hbig.hpp"
#include "hblquote.hpp"
#include "hbold.hpp"
#include "hbody.hpp"
#include "hbutton.hpp"
#include "hbr.hpp"
#include "hcenter.hpp"
#include "hchkbox.hpp"
#include "hcite.hpp"
#include "hcode.hpp"
#include "hdd.hpp"
#include "hdel.hpp"
#include "hdl.hpp"
#include "hdt.hpp"
#include "hdiv.hpp"
#include "hedit.hpp"
#include "hemphsz.hpp"
#include "hfont.hpp"
#include "hform.hpp"
#include "hframe.hpp"
#include "hframset.hpp"
#include "hheading.hpp"
#include "hhidden.hpp"
#include "hhr.hpp"
#include "hiframe.hpp"
#include "hins.hpp"
#include "hitalic.hpp"
#include "hkbd.hpp"
#include "hlabel.hpp"
#include "hli.hpp"
#include "hlist.hpp"
#include "hmap.hpp"
#include "hnobr.hpp"
#include "hoption.hpp"
#include "hpasswd.hpp"
#include "hpre.hpp"
#include "hradio.hpp"
#include "hreset.hpp"
#include "hselect.hpp"
#include "hsmall.hpp"
#include "hspacer.hpp"
#include "hspan.hpp"
#include "hstrike.hpp"
#include "hstring.hpp"
#include "hstrong.hpp"
#include "hsubmit.hpp"
#include "htable.hpp"
#include "htd.hpp"
#include "htr.hpp"
#include "htxtarea.hpp"
#include "hundline.hpp"
#include "webload.hpp"
#if (WEBC_SUPPORT_JSCRIPT)
 #include "wjsdoc.h"
 #include "wjscript.hpp"
#endif
#include "htmlbrow.hpp"
#include "cookie.h"
#include "httpc.h"
#include "webcstr.h"
#include "webcmem.h"
#include "objref.cpp"
#include "webcassert.h"

#if (WEBC_SUPPORT_EMBED)
#include "hembed.hpp"
#endif

/*****************************************************************************/
// Macros
/*****************************************************************************/

#define META_RESULT_CONTINUE        0
#define META_RESULT_REDIRECT       -1
#define META_RESULT_CHARSET_CHANGE -2
#define META_RESULT_HALT           -3
#define TEXT_CAPTURE_GRANULARITY    512


/*****************************************************************************/
// Local Constants
/*****************************************************************************/


/*****************************************************************************/
// Local Functions
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// HtmlDocumentBuilder Methods
/*****************************************************************************/

HtmlDocumentBuilder::HtmlDocumentBuilder(HTMLElement *pRoot)
{
	miDepth = 0;
	mwFlags = 0;
	mElements[miDepth].Set(pRoot);
	mpDocument = pRoot->GetDocument();
	mpScript = 0;
	pTagNesting = 0;

	pRoot->SetFlag(HELEM_FLAG_PARSING_CONTENT);

	// Calculate the current form
	HTMLElementFormFinder formFinder;
	pRoot->FindParent(&formFinder);
	mpCurrentForm = formFinder.Found();

	mbSkipLeadingWhitespace = 1;

	if (!pRoot)
	{
		// error !!!
		return;
	}

	// Set all flags according to root node
	HTMLNthOfTypeFinder preFinder(HTML_PRE_ELEMENT, 0);
	if (pRoot->FindParent(&preFinder))
	{
		SetFlag(HDBLD_FLAG_IN_PRE);
	}

	switch (pRoot->Type())
	{
		case HTML_EDITBOX_ELEMENT:
			StartTextCapture();
			break;

		case HTML_OPTION_ELEMENT:
			SetFlag(HDBLD_FLAG_IN_OPTION);
			StartTextCapture();
			break;

		case HTML_SCRIPT_ELEMENT:
			SetFlag(HDBLD_FLAG_IN_SCRIPT);
			break;

		case HTML_STYLE_ELEMENT:
			SetFlag(HDBLD_FLAG_IN_STYLE);
			break;

		default:
			break;
	}
}

HtmlDocumentBuilder::~HtmlDocumentBuilder(void)
{
	SetDepth(-1);
}

void HtmlDocumentBuilder::SetDepth (int newDepth)
{
	if (newDepth < miDepth)
	{
		while (miDepth > newDepth)
		{
			if (mElements[miDepth].Get())
			{
				ImplicitClose();
				mElements[miDepth].Get()->ClearFlag(HELEM_FLAG_PARSING_CONTENT);
				mElements[miDepth].Set(0);
			}
			miDepth--;
		}
	}
}

long HtmlDocumentBuilder::ParseHtml(WEBC_CHAR *pHtmlSource, long lHtmlLen, HTMLParseContext* parseContext)
{
	HTML_INT32 charsParsed;
	HTML_INT32 totalParsed = 0;
	HTMLTagParse tagParse;
	int done = 0;

	pTagNesting = 0;

	SetState(HDBLD_SUCCESS);

	if (!mpDocument || !parseContext || !CurElement())
	{
		SetState(HDBLD_ERR_UNSPECIFIED);
		return (0);
	}

	while (!done)
	{
		if (CheckFlag(HDBLD_FLAG_STOPPED))
		{
			SetState(HDBLD_STOPPED);
			break;
		}

		charsParsed = HTML_ParseUnit (
				parseContext,
				&tagParse,
				pHtmlSource + totalParsed,
				lHtmlLen - totalParsed
			);

		if (charsParsed == 0)
		{
			SetState(HDBLD_DONE);
			break;
		}

		totalParsed += charsParsed;

		unsigned closeTag = 0;

		if (tagParse.xmlParse.type == XML_UNIT_TAG)
		{
			if ((tagParse.xmlParse.parse.tag.type == XML_TAG_MALFORMED ||
			     tagParse.xmlParse.parse.tag.type == XML_TAG_PARTIAL) &&
			    lHtmlLen - totalParsed == 0)
			{
				totalParsed -= charsParsed;
				SetState(HDBLD_DONE);
				break;
			}

		  #if (WEBC_SUPPORT_JSCRIPT)
			//if we support javascript and we are in a noscript tag then go to the next iteration
			//without parsing anything.
			if (CheckFlag(HDBLD_FLAG_IN_NOSCRIPT) && tagParse.htmlTag != HTML_TAG_NOSCRIPT)
			{
				continue;
			}
		  #endif

		  #if (WEBC_SUPPORT_FRAMES)
			//if we support frames and we are in a noframes tag then go to the next iteration without
			//parsing anything.
			if (CheckFlag(HDBLD_FLAG_IN_NOFRAMES) && tagParse.htmlTag != HTML_TAG_NOFRAMES)
			{
				continue;
			}
		  #endif

			closeTag = (tagParse.xmlParse.parse.tag.type == XML_TAG_END);
		}

	  #if (WEBC_SUPPORT_IFRAMES)
		// if we support IFRAME's then ignore the contents of IFRAME tags
		if (CheckFlag(HDBLD_FLAG_IN_IFRAME) &&
			(tagParse.xmlParse.type != XML_UNIT_TAG || (tagParse.htmlTag != HTML_TAG_IFRAME) || !closeTag))
		{
			continue;
		}
	  #endif

		switch (tagParse.xmlParse.type)
		{
			case XML_UNIT_TAG:
				FlushText();

				if (tagParse.xmlParse.parse.tag.type == XML_TAG_PARTIAL)
				{
					SetState(HDBLD_DONE);
					done = 1;
					break;
				}

				if (tagParse.xmlParse.parse.tag.type == XML_TAG_MALFORMED)
				{
					break;
				}

				if (!closeTag)
				{
					XML_TokenizeStrings(&tagParse.xmlParse);

					// close any tags that have "implied" close tags
					DoImpliedClose(tagParse.htmlTag);
				}

				switch (tagParse.htmlTag)
				{
					case HTML_TAG_A:
						if (!closeTag)
						{
							OpenA(&tagParse);
						}
						else
						{
							CloseA();
						}
						break;
					case HTML_TAG_BUTTON:
						if (!closeTag)
						{
							OpenButton (&tagParse);
						}
						else
						{
							CloseButton();
						}
						break;

					case HTML_TAG_ADDRESS:
						if (!closeTag)
						{
							OpenAddress(&tagParse);
						}
						else
						{
							CloseAddress();
						}
						break;

					case HTML_TAG_AREA:
						if (!closeTag)
						{
							Area(&tagParse);
						}
						break;

					case HTML_TAG_B:
						if (!closeTag)
						{
							OpenB(&tagParse);
						}
						else
						{
							CloseB();
						}
						break;

					case HTML_TAG_BASE:
						if (!closeTag)
						{
							Base(&tagParse);
						}
						break;

					case HTML_TAG_BIG:
						if (!closeTag)
						{
							OpenBig(&tagParse);
						}
						else
						{
							CloseBig();
						}
						break;

					case HTML_TAG_BLOCKQUOTE:
						if (!closeTag)
						{
							OpenBlockquote(&tagParse);
						}
						else
						{
							CloseBlockquote();
						}
						break;

					case HTML_TAG_BODY:
						if (!closeTag)
						{
							Body(&tagParse);
						}
						break;

					case HTML_TAG_BR:
						if (!closeTag)
						{
							Br(&tagParse);
						}
						break;

					case HTML_TAG_CAPTION:
						break;

					case HTML_TAG_CENTER:
						if (!closeTag)
						{
							OpenCenter(&tagParse);
						}
						else
						{
							CloseCenter();
						}
						break;

					case HTML_TAG_CITE:
						if (!closeTag)
						{
							OpenCite(&tagParse);
						}
						else
						{
							CloseCite();
						}
						break;

					case HTML_TAG_CODE:
					case HTML_TAG_SAMP:
					case HTML_TAG_TT:
						if (!closeTag)
						{
							OpenCode(&tagParse);
						}
						else
						{
							CloseCode();
						}
						break;

					case HTML_TAG_DD:
						if (!closeTag)
						{
							OpenDD(&tagParse);
						}
						else
						{
							CloseDD();
						}
						break;

					case HTML_TAG_DEL:
						if (!closeTag)
						{
							OpenDel(&tagParse);
						}
						else
						{
							CloseDel();
						}
						break;

					//case HTML_TAG_DIR:
						//break;

					case HTML_TAG_DIV:
						if (!closeTag)
						{
							OpenDiv(&tagParse);
						}
						else
						{
							CloseDiv();
						}
						break;

					case HTML_TAG_DL:
						if (!closeTag)
						{
							OpenDL(&tagParse);
						}
						else
						{
							CloseDL();
						}
						break;

					case HTML_TAG_DT:
						if (!closeTag)
						{
							OpenDT(&tagParse);
						}
						else
						{
							CloseDT();
						}
						break;

					case HTML_TAG_EM:
						if (!closeTag)
						{
							OpenEm(&tagParse);
						}
						else
						{
							CloseEm();
						}
						break;

					case HTML_TAG_FONT:
						if (!closeTag)
						{
							OpenFont(&tagParse);
						}
						else
						{
							CloseFont();
						}
						break;

					case HTML_TAG_FORM:
						if (!closeTag)
						{
							OpenForm(&tagParse);
						}
						else
						{
							CloseForm();
						}
						break;

#if (WEBC_SUPPORT_FRAMES)
					case HTML_TAG_FRAME:
						if (!closeTag)
						{
							Frame(&tagParse);
						}
						break;
#endif // WEBC_SUPPORT_FRAMES

#if (WEBC_SUPPORT_FRAMES)
					case HTML_TAG_FRAMESET:
						if (!closeTag)
						{
							OpenFrameset(&tagParse);
						}
						else
						{
							CloseFrameset();
						}
						break;
#endif // WEBC_SUPPORT_FRAMES

					case HTML_TAG_H1:
					case HTML_TAG_H2:
					case HTML_TAG_H3:
					case HTML_TAG_H4:
					case HTML_TAG_H5:
					case HTML_TAG_H6:
						if (!closeTag)
						{
							OpenHeading(&tagParse);
						}
						else
						{
							CloseHeading();
						}
						break;

					case HTML_TAG_HR:
						if (!closeTag)
						{
							HorizontalRule(&tagParse);
						}
						break;

					case HTML_TAG_HTML:
						break;

					case HTML_TAG_I:
					case HTML_TAG_VAR:
						if (!closeTag)
						{
							OpenI(&tagParse);
						}
						else
						{
							CloseI();
						}
						break;

#if (WEBC_SUPPORT_IFRAMES && WEBC_SUPPORT_FRAMES)
					case HTML_TAG_IFRAME:
						if (!closeTag)
						{
							OpenIFrame(&tagParse);
						}
						else
						{
							CloseIFrame();
						}
						break;
#endif // WEBC_SUPPORT_IFRAMES

					// We can get away with treating applet and object tags like images because
					// the overlap in attributes is enough to provide "placeholder" behavior
					case HTML_TAG_APPLET:
					case HTML_TAG_OBJECT:
						if (!closeTag)
						{
							Object(&tagParse);
						}
						break;
					case HTML_TAG_IMG:
						if (!closeTag)
						{
							Image(&tagParse);
						}
						break;
#if (WEBC_SUPPORT_EMBED)
					case HTML_TAG_EMBED:
						if (!closeTag)
						{
							Embed(&tagParse);
						}
						break;
#endif
					case HTML_TAG_INPUT:
						if (!closeTag)
						{
							Input(&tagParse);
						}
						break;

					case HTML_TAG_INS:
						if (!closeTag)
						{
							OpenIns(&tagParse);
						}
						else
						{
							CloseIns();
						}
						break;

					case HTML_TAG_KBD:
						if (!closeTag)
						{
							OpenKbd(&tagParse);
						}
						else
						{
							CloseKbd();
						}
						break;

					case HTML_TAG_LABEL:
						if (!closeTag)
						{
							OpenLabel(&tagParse);
						}
						else
						{
							CloseLabel();
						}
						break;

					case HTML_TAG_LI:
						if (!closeTag)
						{
							OpenLI(&tagParse);
						}
						else
						{
							CloseLI();
						}
						break;

					case HTML_TAG_LINK:
						if (!closeTag)
						{
							Link(&tagParse);
						}
						break;

					case HTML_TAG_MAP:
						if (!closeTag)
						{
							OpenMap(&tagParse);
						}
						else
						{
							CloseMap();
						}
						break;

					case HTML_TAG_META:
						if (!closeTag)
						{
							int metaResult = Meta(&tagParse);
							switch (metaResult)
							{
								case META_RESULT_HALT:
									done = 1;
									SetState(HDBLD_ERR_UNSPECIFIED);
									break;

								case META_RESULT_CHARSET_CHANGE:
									done = 1;
									SetState(HDBLD_ERR_CHARSET_CHANGE);
									break;

								default:
									break;
							}
						}
						break;

					case HTML_TAG_NOBR:
						if (!closeTag)
						{
							OpenNoBr(&tagParse);
						}
						else
						{
							CloseNoBr();
						}
						break;

					case HTML_TAG_NOFRAMES:
						if (!closeTag)
						{
							OpenNoFrames(&tagParse);
						}
						else
						{
							CloseNoFrames();
						}
						break;

					case HTML_TAG_NOSCRIPT:
						if (!closeTag)
						{
							OpenNoScript(&tagParse);
						}
						else
						{
							CloseNoScript();
						}
						break;

					case HTML_TAG_OL:
						if (!closeTag)
						{
							OpenOL(&tagParse);
						}
						else
						{
							CloseOL();
						}
						break;

					case HTML_TAG_OPTION:
						if (!closeTag)
						{
							OpenOption(&tagParse);
						}
						else
						{
							CloseOption();
						}
						break;

					case HTML_TAG_P:
						if (!closeTag)
						{
							pTagNesting++;
							OpenP(&tagParse);
						}
						else
						{
							if(pTagNesting)
								pTagNesting--;
							CloseP();
						}
						break;

					case HTML_TAG_PRE:
						if (!closeTag)
						{
							OpenPre(&tagParse);
						}
						else
						{
							ClosePre();
						}
						break;

					case HTML_TAG_SCRIPT:
						if (!closeTag)
						{
							OpenScript(&tagParse);
						}
						else
						{
							CloseScript();
							if (GetState() == HDBLD_RUN_SCRIPT)
							{
								done = 1;
							}
						}
						break;

					case HTML_TAG_SELECT:
						if (!closeTag)
						{
							OpenSelect(&tagParse);
						}
						else
						{
							CloseSelect();
						}
						break;

					case HTML_TAG_SMALL:
						if (!closeTag)
						{
							OpenSmall(&tagParse);
						}
						else
						{
							CloseSmall();
						}
						break;

					case HTML_TAG_SPACER:
						if (!closeTag)
						{
							Spacer(&tagParse);
						}
						break;

					case HTML_TAG_SPAN:
						if (!closeTag)
						{
							OpenSpan(&tagParse);
						}
						else
						{
							CloseSpan();
						}
						break;

					case HTML_TAG_S:
					case HTML_TAG_STRIKE:
						if (!closeTag)
						{
							OpenStrike(&tagParse);
						}
						else
						{
							CloseStrike();
						}
						break;

					case HTML_TAG_STRONG:
						if (!closeTag)
						{
							OpenStrong(&tagParse);
						}
						else
						{
							CloseStrong();
						}
						break;

					case HTML_TAG_STYLE:
						if (!closeTag)
						{
							OpenStyle();
						}
						else
						{
							CloseStyle();
						}
						break;

					case HTML_TAG_TABLE:
#if (WEBC_SUPPORT_TABLES)
						if (!closeTag)
						{
							OpenTable(&tagParse);
						}
						else
						{
							CloseTable();
						}
#endif // WEBC_SUPPORT_TABLES
						break;

					case HTML_TAG_TD:
#if (WEBC_SUPPORT_TABLES)
						if (!closeTag)
						{
							OpenTd(&tagParse);
						}
						else
						{
							CloseTd();
						}
#endif // WEBC_SUPPORT_TABLES
						break;

					case HTML_TAG_TEXTAREA:
						if (!closeTag)
						{
							OpenTextArea(&tagParse);
						}
						else
						{
							CloseTextArea();
						}
						break;

					case HTML_TAG_TH:
#if (WEBC_SUPPORT_TABLES)
						if (!closeTag)
						{
							OpenTd(&tagParse);
						}
						else
						{
							CloseTd();
						}
#endif // WEBC_SUPPORT_TABLES
						break;

					case HTML_TAG_TITLE:
						if (!closeTag)
						{
							OpenTitle(&tagParse);
						}
						else
						{
							CloseTitle();
						}
						break;

					case HTML_TAG_TR:
#if (WEBC_SUPPORT_TABLES)
						if (!closeTag)
						{
							OpenTr(&tagParse);
						}
						else
						{
							CloseTr();
						}
#endif // WEBC_SUPPORT_TABLES
						break;

					case HTML_TAG_U:
						if (!closeTag)
						{
							OpenU(&tagParse);
						}
						else
						{
							CloseU();
						}
						break;

					case HTML_TAG_DIR:
					case HTML_TAG_MENU:
					case HTML_TAG_UL:
						if (!closeTag)
						{
							OpenUL(&tagParse);
						}
						else
						{
							CloseUL();
						}
						break;

					default:
						// ignore other HTML tags
						break;
				}

				if (!closeTag)
				{
					XML_RestoreStrings(&tagParse.xmlParse);
				}

				break;

			case XML_UNIT_TEXT:
			{
              #if (!WEBC_SUPPORT_JSCRIPT)
				if (CheckFlag(HDBLD_FLAG_IN_SCRIPT))
			  #else
				if (CheckFlag(HDBLD_FLAG_IN_NOSCRIPT))
			  #endif
				{
					break;
				}

			  #if (!WEBC_SUPPORT_STYLE_SHEETS)
				if (CheckFlag(HDBLD_FLAG_IN_STYLE))
				{
					break;
				}
			  #endif

			  #if (WEBC_SUPPORT_FRAMES)
				if (CheckFlag(HDBLD_FLAG_IN_NOFRAMES))
				{
					break;
				}
			  #endif

				XML_TokenizeStrings(&tagParse.xmlParse);

				WEBC_CHAR* parsedText = tagParse.xmlParse.parse.text.str;
				long parsedTextLen = tagParse.xmlParse.parse.text.len;

				if (CheckFlag(HDBLD_FLAG_TEXT_CAPTURE_ON))
				{
					mTextCapture.append(parsedText, parsedTextLen);
				}
				else
				{
					mTextBuffer.append(parsedText, parsedTextLen);
				}

				XML_RestoreStrings(&tagParse.xmlParse);
				break;
			}

			case XML_UNIT_UNKNOWN:
				// tbd - log this?
				break;

			case XML_UNIT_COMMENT:
				break;
		}
	}

	if (CheckFlag(HDBLD_FLAG_STOPPED))
	{
		SetState(HDBLD_STOPPED);
	}

	return (totalParsed);
}

void HtmlDocumentBuilder::DoImpliedClose(HTMLTagType htmlTag)
{
	// An HTML paragraph (P tag) may contain any element allowed in a text flow, including
	// conventional words and punctuation, links (<a>), images (<img>), line breaks
	// (<br>), font changes (<b>, <i>, <tt>, <u>, <strike>, <big>, <small>, <sup>,
	// <sub>, and <font>), and content-based style changes (<cite>, <code>, <dfn>,
	// <em>, <kbd>, <samp>, <strong>, and <var>). If any other element occurs within
	//  the paragraph, it implies the paragraph has ended, and the browser assumes the
	// closing </p> tag was not specified.

	// TODO add logic for any other tags that have "implied" close tags

	if(pTagNesting)
	{
		switch(htmlTag)
		{
		// list of "allowed" tags within a <P> (that won't cause an automatic close)
		case HTML_TAG_A:
		case HTML_TAG_B:
		case HTML_TAG_IMG:
		case HTML_TAG_BR:
		case HTML_TAG_I:
		case HTML_TAG_TT:
		case HTML_TAG_U:
		case HTML_TAG_BIG:
		case HTML_TAG_STRIKE:
		case HTML_TAG_STYLE:
		case HTML_TAG_SMALL:
		case HTML_TAG_SUB:
		case HTML_TAG_SUP:
		case HTML_TAG_FONT:
		case HTML_TAG_CITE:
		case HTML_TAG_CODE:
		case HTML_TAG_DFN:
		case HTML_TAG_EM:
		case HTML_TAG_KBD:
		case HTML_TAG_SAMP:
		case HTML_TAG_STRONG:
		case HTML_TAG_VAR:
#if (WEBC_SUPPORT_EMBED)
		case HTML_TAG_EMBED:
#endif
			break;

		default:
			do
			{
				WEBC_ASSERT(pTagNesting > 0);
				CloseP();
			}
			while(--pTagNesting);
			break;
		}
	}
}


void HtmlDocumentBuilder::FlushText (void)
{
	if (mTextBuffer.getLength() > 0)
	{
		WEBC_CHAR* str;

		if (!CheckFlag(HDBLD_FLAG_IN_SCRIPT))
		{
			if (!CheckFlag(HDBLD_FLAG_IN_PRE))
			{
				mTextBuffer.collapseWhitespace();
				mTextBuffer.unescapeHtmlChars();
			}
			else
			{
				mTextBuffer.unescapeHtmlChars();
				mTextBuffer.crLfToLf();
			}

			str = mTextBuffer.getChars();
			if (mbSkipLeadingWhitespace && !CheckFlag(HDBLD_FLAG_IN_PRE))
			{
				while (IS_WHITESPACE(*str))
				{
					str++;
				}
			}
		}
		else
		{
			str = mTextBuffer.getChars();
		}

		Text(str);

		mTextBuffer.discardAll();
	}
}

void HtmlDocumentBuilder::ImplicitClose (void)
{
	if (CheckFlag(HDBLD_FLAG_IMPLICIT_CLOSE))
	{
		return;
	}
	SetFlag(HDBLD_FLAG_IMPLICIT_CLOSE);

	switch (mElements[miDepth].Get()->GetTag())
	{
		case HTML_TAG_A:
			break;

		case HTML_TAG_ADDRESS:
			break;

		case HTML_TAG_B:
			break;

		case HTML_TAG_BIG:
			break;

		case HTML_TAG_BLOCKQUOTE:
			mbSkipLeadingWhitespace = 1;
			break;

		case HTML_TAG_CAPTION:
			break;

		case HTML_TAG_CENTER:
			break;

		case HTML_TAG_CITE:
			break;

		case HTML_TAG_CODE:
		case HTML_TAG_SAMP:
		case HTML_TAG_TT:
			break;

		case HTML_TAG_DD:
			mbSkipLeadingWhitespace = 1;
			break;

		case HTML_TAG_DEL:
			break;

		case HTML_TAG_DIV:
			break;

		case HTML_TAG_DL:
			mbSkipLeadingWhitespace = 1;
			break;

		case HTML_TAG_DT:
			mbSkipLeadingWhitespace = 1;
			break;

		case HTML_TAG_EM:
			break;

		case HTML_TAG_FONT:
			break;

		case HTML_TAG_FORM:
			break;

		case HTML_TAG_FRAMESET:
			break;

		case HTML_TAG_H1:
		case HTML_TAG_H2:
		case HTML_TAG_H3:
		case HTML_TAG_H4:
		case HTML_TAG_H5:
		case HTML_TAG_H6:
			mbSkipLeadingWhitespace = 1;
			break;

		case HTML_TAG_HTML:
			break;

		case HTML_TAG_I:
			break;

		case HTML_TAG_INS:
			break;

		case HTML_TAG_KBD:
			break;

		case HTML_TAG_LI:
			break;

		case HTML_TAG_MAP:
			break;

		case HTML_TAG_NOBR:
			break;

		case HTML_TAG_OL:
			mbSkipLeadingWhitespace = 1;
			break;

		case HTML_TAG_OPTION:
			if (CheckFlag(HDBLD_FLAG_IN_OPTION))
			{
				StopTextCapture();
				if (mTextCapture.getLength() > 0)
				{
					mTextCapture.collapseWhitespace();
					mTextCapture.unescapeHtmlChars();

					int iNewDepth = miDepth;
					if (mElements[iNewDepth].Get())
					{
						HTMLElementType t = mElements[iNewDepth].Get()->Type();
						while (iNewDepth > 0)
						{
							if (t == HTML_OPTION_ELEMENT)
							{
								((HTMLOption *) mElements[iNewDepth].Get())->SetText(mTextCapture.getChars());
								break;
							}
							iNewDepth--;
							if (!mElements[iNewDepth].Get())
							{
								break;
							}
							t = mElements[iNewDepth].Get()->Type();
						}
					}
				}
				ClearFlag(HDBLD_FLAG_IN_OPTION);
			}
			break;

		case HTML_TAG_P:
			mbSkipLeadingWhitespace = 1;
			break;

		case HTML_TAG_PRE:
			ClearFlag(HDBLD_FLAG_IN_PRE);
			break;

		case HTML_TAG_SCRIPT:
			// NEVER IMPLICITLY CLOSE A SCRIPT !
			break;

		case HTML_TAG_SELECT:
			break;

		case HTML_TAG_SMALL:
			break;

		case HTML_TAG_SPAN:
			break;

		case HTML_TAG_S:
		case HTML_TAG_STRIKE:
			break;

		case HTML_TAG_STRONG:
			break;

		case HTML_TAG_STYLE:
			// NEVER IMPLICITLY CLOSE A STYLE !
			break;

		case HTML_TAG_TABLE:
			break;

		case HTML_TAG_TD:
		case HTML_TAG_TH:
			break;

		case HTML_TAG_TEXTAREA:
			break;

		case HTML_TAG_TITLE:
			break;

		case HTML_TAG_TR:
			break;

		case HTML_TAG_U:
			break;

		case HTML_TAG_DIR:
		case HTML_TAG_MENU:
		case HTML_TAG_UL:
			mbSkipLeadingWhitespace = 1;
			break;

		default:
			// ignore other HTML tags
			break;
	}

	ClearFlag(HDBLD_FLAG_IMPLICIT_CLOSE);
}

HTMLElement *HtmlDocumentBuilder::CurElement(void)
{
	return (mElements[miDepth].Get());
}

void HtmlDocumentBuilder::AddElement(HTMLElement* pChild)
{
	if (pChild->Type() != HTML_FORM_ELEMENT && mpCurrentForm)
	{
		pChild->SetForm(mpCurrentForm);
	}

	switch (pChild->Type())
	{
		// the following types can appear anywhere in a document
		case HTML_HIDDEN_INPUT_ELEMENT: // intentional fall-through
		case HTML_MAP_ELEMENT:          // intentional fall-through
		case HTML_SCRIPT_ELEMENT:       // intentional fall-through
		case HTML_FORM_ELEMENT:
			InsertElement(pChild);
			return;

		default:
			break;
	}

	switch (CurElement()->Type())
	{
#if (WEBC_SUPPORT_TABLES)
		case HTML_TABLE_ELEMENT:
			switch (pChild->Type())
			{
				case HTML_TABLE_ELEMENT:
					CloseTable();
					AddElement(pChild);
					break;

				case HTML_TABLE_ROW_ELEMENT:
					InsertElement(pChild);
					break;

				case HTML_TABLE_CELL_ELEMENT:
					OpenTr(0);
					InsertElement(pChild);
					break;

				default:

					HTMLElement *pElement;
					HTMLElement *pParent;
					HTMLElementType ParentType;
					HTMLElementType ElementType;

					pParent = CurElement()->mpParent;
					pElement = CurElement()->mpPrev;
					while (pElement && pElement->Type() == HTML_ELEMENT)
					{
						pElement = pElement->mpPrev;
					}

					ParentType = (pParent)? (HTMLElementType)pParent->Type() : HTML_ELEMENT_NONE;
					ElementType = (pElement)? (HTMLElementType)pElement->Type() : HTML_ELEMENT_NONE;

					while (pElement && TextUnitIsContainer[ElementType])
					{
						pParent = pElement;
						pElement = pElement->LastChild();

						ParentType = (pParent)? (HTMLElementType)pParent->Type() : HTML_ELEMENT_NONE;
						ElementType = (pElement)? (HTMLElementType)pElement->Type() : HTML_ELEMENT_NONE;
					}

					if (pParent)
					{
						pParent->InsertAfter(pChild, pElement);
					}

					break;
			}
			break;

		case HTML_TABLE_ROW_ELEMENT:
			switch (pChild->Type())
			{
				case HTML_TABLE_CELL_ELEMENT:
					InsertElement(pChild);
					break;

				case HTML_TABLE_ELEMENT:
					CloseTr();
					CloseTable();
					InsertElement(pChild);
					break;

				default:
				{
					HTMLElement *parent = CurElement();

					if (parent)
					{
						while (parent->mpParent &&
							   parent->Type() != HTML_TABLE_ELEMENT)
						{
							parent = parent->mpParent;
						}

						if (parent->mpParent && parent->Type() == HTML_TABLE_ELEMENT)
						{
							parent = parent->mpParent;
						}

						HTMLElement *after = parent->LastChild();
						if (after)
						{
							parent->InsertAfter(pChild, after);
						}
						else
						{
							parent->InsertLast(pChild);
						}
					}
					break;
				}
			}
			break;
#endif // WEBC_SUPPORT_TABLES

		default:
			InsertElement(pChild);
			break;
	}
}

void HtmlDocumentBuilder::InsertElement(HTMLElement *pChild)
{
	if ((pChild->GetBreak() & TU_BREAK_BEFORE) && (pChild->GetUnitType() != TU_TYPE_INLINE) &&  (pChild->GetUnitType() != TU_TYPE_INLINE_BLOCK))
	{
		mbSkipLeadingWhitespace = 1;
	}
	CurElement()->InsertLast(pChild);
	if (TextUnitIsContainer[pChild->Type()])
	{
		if (miDepth < WEBC_CFG_MAX_ELEMENT_DEPTH - 1)
		{
			miDepth++;
			mElements[miDepth].Set(pChild);
			if (!mElements[miDepth].Get())
			{
				miDepth--;
			}
			else
			{
				pChild->SetFlag(HELEM_FLAG_PARSING_CONTENT);
			}
		}
	}
}

WEBC_BOOL DivImplicitClose(HTMLElement *pElem, void *ctx)
{
	switch (pElem->Type())
	{
		case HTML_SELECT_ELEMENT:
		case HTML_TABLE_ELEMENT:
		case HTML_TABLE_ROW_ELEMENT:
		case HTML_TABLE_CELL_ELEMENT:
			return (0);

		default:
			return (1);
	}
}

WEBC_BOOL DefaultImplicitClose(HTMLElement *pElem, void *ctx)
{
	switch (pElem->Type())
	{
		case HTML_PARAGRAPH_ELEMENT:       // intentional fall-through
		case HTML_DEFINITION_TERM_ELEMENT: // intentional fall-through
		case HTML_DEFINITION_ELEMENT:      // intentional fall-through
		case HTML_LIST_ITEM_ELEMENT:       // intentional fall-through
		case HTML_SELECT_ELEMENT:          // intentional fall-through
		case HTML_OPTION_ELEMENT:          // intentional fall-through
		case HTML_FORM_ELEMENT:
			return (1);

		case HTML_CENTER_ELEMENT:
			return (0);

		default:
			return (pElem->GetUnitType() == TU_TYPE_INLINE || pElem->GetUnitType() == TU_TYPE_INLINE_BLOCK);
	}
}

WEBC_BOOL MatchAll(HTMLElement *pElem, void *ctx)
{
	return (1);
}

WEBC_BOOL MatchNone(HTMLElement *pElem, void *ctx)
{
	return (0);
}

WEBC_BOOL DTImplicitClose(HTMLElement *pElem, void *ctx)
{
	return (pElem->Type() != HTML_DEFINITION_LIST_ELEMENT);
}

WEBC_BOOL MatchTable(HTMLElement *pElem, void *ctx)
{
	return (pElem->Type() == HTML_TABLE_ELEMENT);
}

WEBC_BOOL MatchList(HTMLElement *pElem, void *ctx)
{
	return (pElem->Type() == HTML_LIST_ELEMENT);
}

int HtmlDocumentBuilder::CloseType(HTMLElementType Type, MatchElementFn allowImplicitClose)
{
int iNewDepth;
int iSkipWs = 0;

	if (CheckFlag(HDBLD_FLAG_IMPLICIT_CLOSE))
	{
		allowImplicitClose = MatchNone;
	}
	else
	{
		if (allowImplicitClose == 0)
		{
			{
				allowImplicitClose = DefaultImplicitClose;
			}
		}
	}

	iNewDepth = miDepth;
	while (iNewDepth > 0 && mElements[iNewDepth].Get())
	{
		if ((mElements[iNewDepth].Get()->GetBreak() & TU_BREAK_AFTER) &&
			(mElements[iNewDepth].Get()->GetUnitType() != TU_TYPE_INLINE)&&
			(mElements[iNewDepth].Get()->GetUnitType() != TU_TYPE_INLINE_BLOCK))
		{
			iSkipWs = 1;
		}

		if (mElements[iNewDepth].Get()->Type() == Type)
		{
			mbSkipLeadingWhitespace |= iSkipWs;
			if (mElements[miDepth].Get()->GetTextAlign() != mElements[iNewDepth - 1].Get()->GetTextAlign())
			{
				mbSkipLeadingWhitespace |= 1;
			}
			SetDepth(iNewDepth - 1);
			return (0);
		}
		else if (!allowImplicitClose(mElements[iNewDepth].Get(), 0))
		{
			break;
		}

		iNewDepth--;
	}

	return (-1);
}

void HtmlDocumentBuilder::StartTextCapture(void)
{
	mTextCapture.discardAll();
	SetFlag(HDBLD_FLAG_TEXT_CAPTURE_ON);
}

void HtmlDocumentBuilder::StopTextCapture(void)
{
	ClearFlag(HDBLD_FLAG_TEXT_CAPTURE_ON);
}

int HtmlDocumentBuilder::CurrentHasParent(HTMLElementType type)
{
	for (int index = miDepth; index >= 0; index--)
	{
		if (mElements[index].Get() &&
		    mElements[index].Get()->Type() == type)
		{
			return 1;
		}
	}
	return 0;
}

HTMLElement *HtmlDocumentBuilder::Text(WEBC_CHAR *str)
{
HTMLString *Unit;

	if(*str == '\0')
	{
		return(0);
	}

	if (CheckFlag(HDBLD_FLAG_IN_SCRIPT))
	{
		// if we're in a script, then don't break the string into lines
		//  (to save on the overhead)
		WEBC_NEW_VERBOSE(Unit, HTMLString(this->mpDocument, str),"HTMLString");

		if (!Unit)
		{
			return (0);
		}

		if (Unit->GetStringLength() == 0)
		{
			WEBC_DELETE(Unit);
			return (0);
		}

		AddElement(Unit);
		Unit->Update(0,0);
	}
	else
	{
		// if our last character is whitespace, then skip leading whitespace at
		//  the beginning of the next text fragment
		if (IS_WHITESPACE(str[webc_strlen(str) - 1]))
		{
			mbSkipLeadingWhitespace = 1;
		}
		else
		{
			mbSkipLeadingWhitespace = 0;
		}

		size_t lfPos, crPos, lineLen;
		WEBC_CHAR ch;
		while (str[0] != '\0')
		{
			for (lfPos = 0; str[lfPos] && str[lfPos] != '\n'; lfPos++) {;}
			for (crPos = 1; str[crPos] && str[crPos] != '\r'; crPos++) {;}
			if (str[lfPos] == '\n')
			{
				lfPos++;
			}

			lineLen = EBSMIN(lfPos, crPos);
			ch = str[lineLen];
			str[lineLen] = '\0';
			WEBC_NEW_VERBOSE(Unit, HTMLString(this->mpDocument, str),"HTMLString");
			str[lineLen] = ch;
			str += lineLen;

			if (!Unit)
			{
				return (0);
			}

			if (Unit->GetStringLength() == 0)
			{
				WEBC_DELETE(Unit);
				return (0);
			}

			AddElement(Unit);
			Unit->Update(0,0);
		}
	}

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::Image(HTMLTagParse *td)
{
HTMLImage *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLImage(td, mpDocument),"HTMLImage");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

#if (WEBC_SUPPORT_EMBED)
HTMLElement *HtmlDocumentBuilder::Embed(HTMLTagParse *td)
{
	//created an object of type HTMLembed *Unit;
	HTMLEmbed *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLEmbed(td, mpDocument),"HTMLEmbed");

	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);
	return (Unit);
}
#endif

HTMLElement *HtmlDocumentBuilder::Object(HTMLTagParse *td)
{
HTMLObject *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLObject(td, mpDocument),"HTMLObject");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

// <EDIT 20080201 - Add button tag support
HTMLElement *HtmlDocumentBuilder::OpenButton(HTMLTagParse *td)
{
HTMLButton *Unit;

	// Don't allow button tag nesting
	CloseButton();

	WEBC_NEW_VERBOSE(Unit, HTMLButton(td, mpDocument),"HTMLButton");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseButton(void)
{
	return (CloseType(HTML_BUTTON_ELEMENT));
}


HTMLElement *HtmlDocumentBuilder::Input(HTMLTagParse *td)
{
	HTMLElement *input_elem = 0;
	HTMLAttribValueParse* valueParse;
	HTMLInputType inputType = HTML_INPUT_TYPE_TEXT;

	valueParse = HTML_GetTagAttribute(td, HTML_ATTRIB_TYPE);
	if (valueParse)
	{
		inputType = HTML_ParseInputType(valueParse->str, valueParse->len);
	}

	switch (inputType)
	{
		case HTML_INPUT_TYPE_FILE:
		case HTML_INPUT_TYPE_TEXT: //AK_TODO treat the file upload object like a text item for now
			input_elem = EditStr(td);
			break;

		case HTML_INPUT_TYPE_PASSWORD:
			input_elem = Password(td);
			break;

		case HTML_INPUT_TYPE_CHECKBOX:
			input_elem = Checkbox(td);
			break;

		case HTML_INPUT_TYPE_RADIO:
			input_elem = RadioButton(td);
			break;

		case HTML_INPUT_TYPE_SUBMIT:
			input_elem = InputSubmit(td);
			break;

		case HTML_INPUT_TYPE_HIDDEN:
			input_elem = InputHidden(td);
			break;

		case HTML_INPUT_TYPE_IMAGE:
			input_elem = Image(td);
			break;

		case HTML_INPUT_TYPE_RESET:
			input_elem = InputReset(td);
			break;

		case HTML_INPUT_TYPE_BUTTON:
			input_elem = InputButton(td);
			break;

		default:
			// ignore other input types
			return (0);
	}

	return (input_elem);
}


HTMLElement *HtmlDocumentBuilder::Area(HTMLTagParse *td)
{
HTMLArea *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLArea(td, mpDocument),"HTMLArea");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);
	AddElement(Unit);
	Unit->Update(0,0);
	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::Base(HTMLTagParse *td)
{
	HTMLBase* Unit = WEBC_NULL;

	if (mpDocument->Head())
	{
		WEBC_NEW_VERBOSE(Unit,HTMLBase(td, mpDocument),"HTMLBase");
		if (Unit)
		{
			Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);
			mpDocument->Head()->InsertLast(Unit);
		}
	}

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::Body(HTMLTagParse *td)
{
HTMLBody *Unit = (HTMLBody *) mpDocument->Body();

	if (Unit)
	{
		Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);
		Unit->StyleUpdateChildren();
	}

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::Checkbox(HTMLTagParse *td)
{
HTMLCheckbox *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLCheckbox(td, mpDocument),"HTMLCheckbox");
	if (!Unit)
	{
		return 0;
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);
	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::Meta(HTMLTagParse *td)
{
	static WEBC_CHAR nullStr[1] = {0};
	static HTMLAttribValueParse defaultValue = {nullStr, 0, 0};

	URLDescriptor *url;
	HTMLAttribValueParse* name;
	HTMLAttribValueParse* content;
	HTMLMetaStatus status;

#if (WEBC_CFG_META_DIPATCH_EQUIV)
	name = HTML_GetTagAttribute(td, HTML_ATTRIB_HTTP_EQUIV);
	if (!name)
	{
		name = &defaultValue;
	}
#else
	name = HTML_GetTagAttribute(td, HTML_ATTRIB_NAME);
	if (!name)
	{
		name = &defaultValue;
	}
#endif

	content = HTML_GetTagAttribute(td, HTML_ATTRIB_CONTENT);
	if (!content)
	{
		content = &defaultValue;
	}

	status = HTML_ProcessMetaTag(name->str, content->str, mpDocument);
	switch (status)
	{
		case HTML_META_STATUS_CONTINUE:
			break;

		case HTML_META_STATUS_STOP:
			return (META_RESULT_CONTINUE);

		case HTML_META_STATUS_HALT:
			return (META_RESULT_HALT);

		default:
			break;
	}

	url = mpDocument->CurrentURL();

    HTMLAttribValueParse* httpEquiv = HTML_GetTagAttribute(td, HTML_ATTRIB_HTTP_EQUIV);
    if (httpEquiv && httpEquiv->str)
    {
		if (!webc_c_stricmp(httpEquiv->str, "Refresh"))
		{
			int delay;
			WEBC_CHAR *savep;
			WEBC_CHAR *url_ptr;

			delay = webc_atoi(content->str);

			savep = content->str;
			while (*savep && *savep != ',' && *savep != ';')
			{
				savep++;
			}

			if (*savep)
			{
				savep++;

				while (IS_WHITESPACE(*savep))
				{
					savep++;
				}

				if (webc_c_strnicmp(savep, "URL=", 4) == 0)
				{
					url_ptr = savep+4;
				}
				else
				{
					url_ptr = savep;
				}

				while (IS_WHITESPACE(*url_ptr))
				{
					url_ptr++;
				}

				mpDocument->SetRefreshTimer(url_ptr, delay*1000);
			}
			else
			{
				mpDocument->SetRefreshTimer(0, delay*1000);
			}
		}
		else if (!webc_c_stricmp(httpEquiv->str, "Set-Cookie"))
		{
			// process HTTP-EQUIV=Set-Cookie
		  #if (WEBC_SUPPORT_UNICODE)
			webc_set_cookie_uc(content->str, mpDocument->CurrentURL()->getHost(), mpDocument->CurrentURL()->getPath());
		  #else
			webc_set_cookie(content->str, mpDocument->CurrentURL()->getHost(), mpDocument->CurrentURL()->getPath());
		  #endif
		}

	  #if (WEBC_SUPPORT_HTTP_CACHE)
		else if (!webc_c_stricmp(httpEquiv->str, "Expires"))
		{
			// process HTTP-EQUIV=Expires -
			//  reset the cache expiration time for this page
			RTP_TIMESTAMP expires;
		  #if (WEBC_SUPPORT_UNICODE)
			if (rtp_date_parse_time_uc(&expires, content->str) != 0)
		  #else
			if (rtp_date_parse_time(&expires, content->str) != 0)
		  #endif
			{
				// if not a valid time, set the expiration time
				// to now so that we render the cached file obsolete immediately
				rtp_date_get_timestamp(&expires);
			}

			URLDescriptor *url = mpDocument->CurrentURL();
		  #if (WEBC_SUPPORT_UNICODE)
			http_update_cache_expiration_uc(url->getPath(), url->getHost(), url->getPort(), &expires);
		  #else
			http_update_cache_expiration(url->getPath(), url->getHost(), url->getPort(), &expires);
		  #endif
		}

		else if (!webc_c_stricmp(httpEquiv->str, "Cache-Control"))
		{
			// process HTTP-EQUIV=Cache-Control
			if (!webc_c_stricmp(content->str, "no-cache"))
			{
				// process HTTP-EQUIV=Cache-Control/no-cache -
				//  delete this page from the cache
				URLDescriptor *url = mpDocument->CurrentURL();
			  #if (WEBC_SUPPORT_UNICODE)
				http_update_cache_expiration_uc(url->getPath(), url->getHost(), url->getPort(), WEBC_NULL);
			  #else
				http_update_cache_expiration(url->getPath(), url->getHost(), url->getPort(), WEBC_NULL);
			  #endif
			}
			else
			{
				// all other content values indicate that we can cache
				// this page, so don't reset any cache expiration time on it
			}
		}

		else if (!webc_c_stricmp(httpEquiv->str, "Pragma"))
		{
			// process HTTP-EQUIV=Pragma
			if (!webc_c_stricmp(content->str, "no-cache"))
			{
				// process HTTP-EQUIV=Pragma/no-cache -
				//  delete this page from the cache
				URLDescriptor *url = mpDocument->CurrentURL();
			  #if (WEBC_SUPPORT_UNICODE)
				http_update_cache_expiration_uc(url->getPath(), url->getHost(), url->getPort(), WEBC_NULL);
			  #else
				http_update_cache_expiration(url->getPath(), url->getHost(), url->getPort(), WEBC_NULL);
			  #endif
			}
			else
			{
				// ignore all other HTTP-EQUIV=Pragma's
			}
		}
	  #endif // WEBC_SUPPORT_HTTP_CACHE

	  #if (WEBC_SUPPORT_UNICODE)
		else if (!webc_c_stricmp(httpEquiv->str, "Content-Type"))
		{
			WEBC_CHAR *charset;

			charset = webc_c_stristr(content->str, "charset");
			if (charset)
			{
				charset += 7;
				while (IS_WHITESPACE(*charset))
				{
					charset++;
				}
				while (*charset)
				{
					if (*charset == '=')
					{
						charset++;
						while (IS_WHITESPACE(*charset))
						{
							charset++;
						}
						WebcCharset oldCharset = mpDocument->GetCharset();
						mpDocument->SetCharset(webc_charset_lookup(charset), HTML_CHARSET_SRC_META);
						if (oldCharset != mpDocument->GetCharset())
						{
							return (META_RESULT_CHARSET_CHANGE);
						}
						break;
					}
					charset++;
				}
			}
		}
	  #endif
	}

	return (0);
}

HTMLElement *HtmlDocumentBuilder::RadioButton(HTMLTagParse *td)
{
HTMLRadioButton *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLRadioButton(td, mpDocument),"HTMLRadioButton");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::InputSubmit(HTMLTagParse *td)
{
HTMLInputSubmit *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLInputSubmit(td, mpDocument),"HTMLInputSubmit");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::InputReset(HTMLTagParse *td)
{
HTMLInputReset *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLInputReset(td, mpDocument),"HTMLInputReset");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::InputButton(HTMLTagParse *td)
{
HTMLInputButton *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLInputButton(td, mpDocument),"HTMLInputButton");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::EditStr(HTMLTagParse *td)
{
HTMLEditString *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLEditString(td, mpDocument),"HTMLEditString");
	if (!Unit)
	{
		return (0);
	}

	// seed the default value (will get overwritten by the call to SetTagProperties)
	Unit->SetDefValue(WEBC_STR_NULL);

	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::Password(HTMLTagParse *td)
{
HTMLInputPassword *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLInputPassword(td, mpDocument),"HTMLInputPassword");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenTextArea(HTMLTagParse *td)
{
HTMLTextArea *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLTextArea(td, mpDocument),"HTMLTextArea");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	if (!CheckFlag(HDBLD_FLAG_STOPPED))
	{
		StartTextCapture();
	}

	return (Unit);
}


int HtmlDocumentBuilder::CloseTextArea(void)
{
HTMLTextArea *Unit;

	StopTextCapture();

	Unit = (HTMLTextArea *) CurElement()->LastChild();
	if (Unit && Unit->Type() == HTML_EDITBOX_ELEMENT)
	{
		if (mTextCapture.getLength() > 0)
		{
			mTextCapture.unescapeHtmlChars();
			mTextCapture.removeLeadingCrLfs();
			mTextCapture.crLfToLf();
			Unit->SetValue(mTextCapture.getChars());
			Unit->SetDefValue(mTextCapture.getChars());
		}
	}

	return (0);
}

HTMLElement *HtmlDocumentBuilder::OpenSelect (HTMLTagParse *td)
{
HTMLSelect *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLSelect(td, mpDocument),"HTMLSelect");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseSelect (void)
{
	if (CheckFlag(HDBLD_FLAG_IN_OPTION))
	{
		CloseOption();
	}
	return (CloseType(HTML_SELECT_ELEMENT));
}

HTMLElement *HtmlDocumentBuilder::OpenOption (HTMLTagParse *td)
{
HTMLOption *Unit;

	if (CheckFlag(HDBLD_FLAG_IN_OPTION))
	{
		CloseOption();
	}

	WEBC_NEW_VERBOSE(Unit,HTMLOption(td, mpDocument),"HTMLOption");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	if (!CheckFlag(HDBLD_FLAG_STOPPED))
	{
		StartTextCapture();
		SetFlag(HDBLD_FLAG_IN_OPTION);
	}

	return (0);
}

int HtmlDocumentBuilder::CloseOption (void)
{
	return (CloseType(HTML_OPTION_ELEMENT));
}

HTMLElement *HtmlDocumentBuilder::InputHidden(HTMLTagParse *td)
{
HTMLInputHidden *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLInputHidden(td, mpDocument),"HTMLInputHidden");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, 1);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::HorizontalRule(HTMLTagParse *td)
{
HTMLHorizontalRule *Unit;
HTMLElementType wParentType;

	/* tbd - what is this even doing here!?!?! */
	wParentType = CurElement()->Type();
	if (wParentType == HTML_PARAGRAPH_ELEMENT)
	{
		int d = miDepth;
		while (mElements[d].Get()->Type() == HTML_PARAGRAPH_ELEMENT)
		{
			if (d == 0)
			{
				return (0);
			}
			d--;
		}
		SetDepth(d);
	}

	WEBC_NEW_VERBOSE(Unit,HTMLHorizontalRule(td, mpDocument),"HTMLHorizontalRule");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	mbSkipLeadingWhitespace = 1;

	AddElement(Unit);
	Unit->Update(0,0);
	return (Unit);
}


HTMLElement *HtmlDocumentBuilder::Br(HTMLTagParse *td)
{
HTMLElement *Unit;
	mbSkipLeadingWhitespace = 1;

	WEBC_NEW_VERBOSE(Unit,HTMLBr(td, mpDocument),"HTMLBr");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);
	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::Spacer(HTMLTagParse *td)
{
HTMLSpacer *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLSpacer(td,mpDocument),"HTMLSpacer");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	return (Unit);
}


#if (WEBC_SUPPORT_FRAMES)
HTMLElement *HtmlDocumentBuilder::Frame(HTMLTagParse *td)
{
HTMLFrame *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLFrame(td, mpDocument),"HTMLFrame");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
#if (WEBC_SUPPORT_REFERER_HEADER)
	WEBC_CHAR *referer;
	UrlStringBuilder ubld(mpDocument->CurrentURL());
	referer= ubld.MallocString(__FILE__, __LINE__);
	Unit->Update(0,(WEBC_UINT8 *)referer);
#else
	Unit->Update(0,0);
#endif

	return (Unit);
}
#endif // WEBC_SUPPORT_FRAMES

HTMLElement *HtmlDocumentBuilder::OpenA(HTMLTagParse *td)
{
HTMLAnchor *Unit;

	// Don't allow anchor tag nesting
	CloseA();

	WEBC_NEW_VERBOSE(Unit,HTMLAnchor(td, mpDocument),"HTMLAnchor");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseA(void)
{
	return (CloseType(HTML_ANCHOR_ELEMENT));
}

HTMLElement *HtmlDocumentBuilder::OpenAddress(HTMLTagParse *td)
{
HTMLAddress *Unit;

	mbSkipLeadingWhitespace = 1;

	WEBC_NEW_VERBOSE(Unit,HTMLAddress(td, mpDocument),"HTMLAddress");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseAddress(void)
{
	return (CloseType(HTML_ADDRESS_ELEMENT));
}

HTMLElement *HtmlDocumentBuilder::OpenB(HTMLTagParse *td)
{
HTMLBold *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLBold(td, mpDocument),"HTMLBold");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);
	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenBig(HTMLTagParse *td)
{
HTMLBig *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLBig(td, mpDocument),"HTMLBig");
	if (!Unit)
		return (0);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseBig(void)
{
	return (CloseType(HTML_BIG_ELEMENT));
}

HTMLElement *HtmlDocumentBuilder::OpenHeading(HTMLTagParse *td)
{
HTMLHeading *Unit;

	mbSkipLeadingWhitespace = 1;

	WEBC_NEW_VERBOSE(Unit,HTMLHeading(td, mpDocument),"HTMLHeading");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseHeading(void)
{
	return (CloseType(HTML_HEADING_ELEMENT));
}

HTMLElement *HtmlDocumentBuilder::OpenI(HTMLTagParse *td)
{
HTMLItalic *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLItalic(td, mpDocument),"HTMLItalic");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenSmall(HTMLTagParse *td)
{
HTMLSmall *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLSmall(td, mpDocument),"HTMLSmall");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseSmall(void)
{
	return (CloseType(HTML_SMALL_ELEMENT));
}

HTMLElement *HtmlDocumentBuilder::OpenScript(HTMLTagParse *td)
{
#if (WEBC_SUPPORT_JSCRIPT)
HTMLScript *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLScript(td, mpDocument),"HTMLScript");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	SetFlag(HDBLD_FLAG_IN_SCRIPT);

	return (Unit);
#else
	SetFlag(HDBLD_FLAG_IN_SCRIPT);
	return (0);
#endif
}

int HtmlDocumentBuilder::CloseScript(void)
{
int result = 0;

	ClearFlag(HDBLD_FLAG_IN_SCRIPT);
#if (WEBC_SUPPORT_JSCRIPT)
	HTMLScript *pScript = (HTMLScript *)CurElement();

	if (pScript->Type() == HTML_SCRIPT_ELEMENT)
	{
		result = CloseType(HTML_SCRIPT_ELEMENT);
		pScript->Update(0,0);
		if (!CheckFlag(HDBLD_FLAG_STOPPED))
		{
			SetState(HDBLD_RUN_SCRIPT);
			mpScript = pScript;
		}
	}
#endif

	return (result);
}

int HtmlDocumentBuilder::RunScript(void)
{
#if (WEBC_SUPPORT_JSCRIPT)
	if (mpScript->Run() == JS_SCRIPT_FLAG_DELETED)
	{
		return (-1);
	}
#endif

	return (0);
}

HTMLElement *HtmlDocumentBuilder::OpenU(HTMLTagParse *td)
{
HTMLUnderline *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLUnderline(td, mpDocument),"HTMLUnderline");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

/*****************************************************************************/
// Table support
/*****************************************************************************/

#if (WEBC_SUPPORT_TABLES)

HTMLElement *HtmlDocumentBuilder::OpenTable(HTMLTagParse *td)
{
HTMLTable *Unit;
HTMLElementType wParentType;

	mbSkipLeadingWhitespace = 1;

	wParentType = CurElement()->Type();
	switch (wParentType)
	{
		case HTML_TABLE_ELEMENT:
			CloseTable();
			break;

		default:
			// ignore other parent types
			break;
	}

	WEBC_NEW_VERBOSE(Unit,HTMLTable(td, mpDocument),"HTMLTable");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}


int HtmlDocumentBuilder::CloseTable(void)
{
int iNewDepth;
HTMLElementType t;

	mbSkipLeadingWhitespace = 1;

	iNewDepth = miDepth;
	if (mElements[iNewDepth].Get())
	{
		t = mElements[iNewDepth].Get()->Type();
		while (iNewDepth > 0)
		{
			if (t == HTML_TABLE_ELEMENT)
			{
				SetDepth(iNewDepth - 1);
				return (0);
			}

			iNewDepth--;

			if (!mElements[iNewDepth].Get())
			{
				break;
			}

			t = mElements[iNewDepth].Get()->Type();
		}
	}

	return (-1);
}

HTMLElement *HtmlDocumentBuilder::OpenTr(HTMLTagParse *td)
{
HTMLTableRow* Unit;

	mbSkipLeadingWhitespace = 1;

	HTMLElementType parentType = CurElement()->Type();
	switch (parentType)
	{
		case HTML_TABLE_ELEMENT:
			break;

		case HTML_TABLE_CELL_ELEMENT:
			CloseTd();
			// Fall through

		default:
			CloseTr();
			break;
	}

	WEBC_NEW_VERBOSE(Unit, HTMLTableRow(td, mpDocument),"HTMLTableRow");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}


int HtmlDocumentBuilder::CloseTr(void)
{
int iNewDepth;
HTMLElementType t;

	mbSkipLeadingWhitespace = 1;

	iNewDepth = miDepth;
	if (mElements[iNewDepth].Get())
	{
		t = mElements[iNewDepth].Get()->Type();
		while ((iNewDepth > 0) && (t != HTML_TABLE_ELEMENT))
		{
			if (t == HTML_TABLE_ROW_ELEMENT)
			{
				SetDepth(iNewDepth - 1);
				return (0);
			}

			iNewDepth--;

			if (!mElements[iNewDepth].Get())
			{
				break;
			}

			t = mElements[iNewDepth].Get()->Type();
		}
	}

	return (-1);
}

HTMLElement *HtmlDocumentBuilder::OpenTd(HTMLTagParse *td)
{
HTMLTableCell* Unit;

	mbSkipLeadingWhitespace = 1;

	HTMLElementType parentType = CurElement()->Type();
	switch (parentType)
	{
		case HTML_TABLE_ELEMENT:
		case HTML_TABLE_ROW_ELEMENT:
			break;

		default:
			CloseTd();
			break;
	}

	WEBC_NEW_VERBOSE(Unit,HTMLTableCell(td, mpDocument),"HTMLTableCell");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseTd(void)
{
int iNewDepth;
HTMLElementType t;

	mbSkipLeadingWhitespace = 1;

	iNewDepth = miDepth;
	if (mElements[iNewDepth].Get())
	{
		t = mElements[iNewDepth].Get()->Type();
		while ((iNewDepth > 0) && (t != HTML_TABLE_ELEMENT))
		{
			if (t == HTML_TABLE_CELL_ELEMENT)
			{
				SetDepth(iNewDepth - 1);
				return (0);
			}

			if (!mElements[iNewDepth].Get())
			{
				break;
			}

			iNewDepth--;
			t = mElements[iNewDepth].Get()->Type();
		}
	}

	return (-1);
}

#endif // WEBC_SUPPORT_TABLES

HTMLElement *HtmlDocumentBuilder::OpenEm(HTMLTagParse *td)
{
HTMLEmphasize *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLEmphasize(td, mpDocument),"HTMLEmphasize");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenCite(HTMLTagParse *td)
{
HTMLCite *Unit;

	WEBC_NEW_VERBOSE(Unit, HTMLCite(td, mpDocument),"HTMLCite");
	if (!Unit)
	{
		return (0);
	}

	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenCode(HTMLTagParse *td)
{
HTMLCode *Unit;

	WEBC_NEW_VERBOSE(Unit, HTMLCode(td, mpDocument),"HTMLCode");
	if (!Unit)
	{
		return (0);
	}

	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenDel(HTMLTagParse *td)
{
HTMLDel *Unit;

	WEBC_NEW_VERBOSE(Unit, HTMLDel(td, mpDocument),"HTMLDel");
	if (!Unit)
	{
		return (0);
	}

	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenIns(HTMLTagParse *td)
{
HTMLIns *Unit;

	WEBC_NEW_VERBOSE(Unit, HTMLIns(td, mpDocument),"HTMLIns");
	if (!Unit)
	{
		return (0);
	}

	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenKbd(HTMLTagParse *td)
{
HTMLKbd *Unit;

	WEBC_NEW_VERBOSE(Unit, HTMLKbd(td, mpDocument),"HTMLKbd");
	if (!Unit)
	{
		return (0);
	}

	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenStrike(HTMLTagParse *td)
{
HTMLStrike *Unit;

	WEBC_NEW_VERBOSE(Unit, HTMLStrike(td, mpDocument),"HTMLStrike");
	if (!Unit)
	{
		return (0);
	}

	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenStrong(HTMLTagParse *td)
{
HTMLStrong *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLStrong(td, mpDocument),"HTMLStrong");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenCenter(HTMLTagParse *td)
{
HTMLCenter *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLCenter(td, mpDocument),"HTMLCenter");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenP(HTMLTagParse *td)
{
HTMLParagraph *Unit;
HTMLElementType wParentType;

	mbSkipLeadingWhitespace = 1;

	wParentType = CurElement()->Type();
	if (wParentType == HTML_PARAGRAPH_ELEMENT)
	{
		int d = miDepth;
		while (mElements[d].Get()->Type() == HTML_PARAGRAPH_ELEMENT)
		{
			if (d == 0)
			{
				return (0);
			}
			d--;
		}
		SetDepth(d);
	}

	WEBC_NEW_VERBOSE(Unit,HTMLParagraph(td, mpDocument),"HTMLParagraph");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseP(void)
{
	return (CloseType(HTML_PARAGRAPH_ELEMENT));
}


HTMLElement *HtmlDocumentBuilder::OpenSpan (HTMLTagParse *td)
{
HTMLSpan *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLSpan(td, mpDocument),"HTMLSpan");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}


int HtmlDocumentBuilder::CloseSpan (void)
{
	return (CloseType(HTML_SPAN_ELEMENT));
}


HTMLElement *HtmlDocumentBuilder::OpenLabel (HTMLTagParse *td)
{
HTMLLabel *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLLabel(td, mpDocument),"HTMLLabel");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}


int HtmlDocumentBuilder::CloseLabel (void)
{
	return (CloseType(HTML_LABEL_ELEMENT));
}

HTMLElement *HtmlDocumentBuilder::OpenDiv(HTMLTagParse *td)
{
HTMLDiv *Unit;

	mbSkipLeadingWhitespace = 1;

	WEBC_NEW_VERBOSE(Unit,HTMLDiv(td, mpDocument),"HTMLDiv");
	if (!Unit)
		return (0);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseDiv(void)
{
	return (CloseType(HTML_DIV_ELEMENT, DivImplicitClose));
}

#if (WEBC_SUPPORT_FRAMES)
HTMLElement *HtmlDocumentBuilder::OpenFrameset(HTMLTagParse *td)
{
HTMLFrameSet *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLFrameSet(td, mpDocument),"HTMLFrameSet");
	if (!Unit)
		return (0);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);

	return (Unit);
}
#endif // WEBC_SUPPORT_FRAMES

HTMLElement *HtmlDocumentBuilder::OpenFont(HTMLTagParse *td)
{
HTMLFont *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLFont(td, mpDocument),"HTMLFont");
	if (!Unit)
		return (0);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenNoBr(HTMLTagParse *td)
{
HTMLNoBr *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLNoBr(td, mpDocument),"HTMLNoBr");
	if (!Unit)
		return (0);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenPre(HTMLTagParse *td)
{
HTMLPreformatted *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLPreformatted(td, mpDocument),"HTMLPreformatted");
	if (!Unit)
		return (0);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	SetFlag(HDBLD_FLAG_IN_PRE);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenMap(HTMLTagParse *td)
{
HTMLMap *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLMap(td, mpDocument),"HTMLMap");
	if (!Unit)
		return (0);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);

	return (Unit);
}

HTMLForm *HtmlDocumentBuilder::OpenForm(HTMLTagParse *td)
{
HTMLForm *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLForm(td, mpDocument),"HTMLForm");
	mpCurrentForm = Unit;

	if (!Unit)
	{
		return (0);
	}

	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);

	return (Unit);
}

HTMLElement *HtmlDocumentBuilder::OpenTitle(HTMLTagParse *td)
{
	StartTextCapture();
	return (0);
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int HtmlDocumentBuilder::CloseB(void)
{
	return (CloseType(HTML_BOLD_ELEMENT));
}

int HtmlDocumentBuilder::CloseI(void)
{
	return (CloseType(HTML_ITALIC_ELEMENT));
}

int HtmlDocumentBuilder::CloseU(void)
{
	return (CloseType(HTML_UNDERLINE_ELEMENT));
}

int HtmlDocumentBuilder::CloseStrong(void)
{
	return (CloseType(HTML_STRONG_ELEMENT));
}

int HtmlDocumentBuilder::CloseEm(void)
{
	return (CloseType(HTML_EMPHASIZE_ELEMENT));
}

int HtmlDocumentBuilder::CloseCite(void)
{
	return (CloseType(HTML_CITE_ELEMENT));
}

int HtmlDocumentBuilder::CloseCode(void)
{
	return (CloseType(HTML_CODE_ELEMENT));
}

int HtmlDocumentBuilder::CloseDel(void)
{
	return (CloseType(HTML_DEL_ELEMENT));
}

int HtmlDocumentBuilder::CloseKbd(void)
{
	return (CloseType(HTML_KBD_ELEMENT));
}

int HtmlDocumentBuilder::CloseIns(void)
{
	return (CloseType(HTML_INS_ELEMENT));
}

int HtmlDocumentBuilder::CloseStrike(void)
{
	return (CloseType(HTML_STRIKE_ELEMENT));
}

int HtmlDocumentBuilder::CloseCenter(void)
{
	return (CloseType(HTML_CENTER_ELEMENT));
}

#if (WEBC_SUPPORT_FRAMES)
int HtmlDocumentBuilder::CloseFrameset(void)
{
	return (CloseType(HTML_FRAMESET_ELEMENT));
}
#endif // WEBC_SUPPORT_FRAMES

int HtmlDocumentBuilder::CloseFont(void)
{
	return (CloseType(HTML_FONT_ELEMENT));
}

int HtmlDocumentBuilder::CloseNoBr(void)
{
	return (CloseType(HTML_NOBR_ELEMENT));
}

int HtmlDocumentBuilder::ClosePre(void)
{
	return (CloseType(HTML_PRE_ELEMENT));
}

int HtmlDocumentBuilder::CloseMap(void)
{
	return (CloseType(HTML_MAP_ELEMENT));
}

int HtmlDocumentBuilder::CloseForm(void)
{
	mpCurrentForm = 0;
	return (CloseType(HTML_FORM_ELEMENT));
}

int HtmlDocumentBuilder::CloseTitle(void)
{
	StopTextCapture();

	if (mTextCapture.getLength() > 0)
	{
		WEBC_CHAR stripChars[] = {'\r','\n','\t',0};
		mTextCapture.collapseWhitespace();
		mTextCapture.unescapeHtmlChars();
		mTextCapture.removeChars(stripChars);
		mTextCapture.replaceChars(HTML_NBSP_CHAR, ' ');
	}

	mpDocument->SetTitle(mTextCapture.getChars());

	return (0);
}

HTMLElement *HtmlDocumentBuilder::OpenLI (HTMLTagParse *td)
{
HTMLListItem *Unit;
HTMLList *pList;
int iDepth;

	CloseLI();

	mbSkipLeadingWhitespace = 1;

	WEBC_NEW_VERBOSE(Unit,HTMLListItem(td, mpDocument),"HTMLListItem");
	if (!Unit)
	{
		return (0);
	}

	HTMLParentListFinder listFinder;

	iDepth = miDepth;
	while (iDepth >= 0)
	{
		if (listFinder.Check(mElements[iDepth].Get()))
		{
			break;
		}

		iDepth--;
	}

	if (listFinder.Found())
	{
		pList = (HTMLList *) listFinder.Found();

		HTMLLastListItemFinder itemFinder(pList);
		pList->FindElement(&itemFinder);

		HTMLListItem *pLastItem = (HTMLListItem *) itemFinder.Found();

		if (pLastItem)
		{
			Unit->SetListOrder(pLastItem->ListOrder() + 1);
		}
		else
		{
			Unit->SetListOrder(pList->Start());
		}
	}

	AddElement(Unit);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseLI (void)
{
	return (CloseType(HTML_LIST_ITEM_ELEMENT));
}


HTMLElement *HtmlDocumentBuilder::OpenUL (HTMLTagParse *td)
{
HTMLList *Unit;
static CSSListStyleType _bulletStyles[3] = {
		CSS_LIST_STYLE_DISC,
		CSS_LIST_STYLE_CIRCLE,
		CSS_LIST_STYLE_SQUARE
	};

	mbSkipLeadingWhitespace = 1;

	WEBC_NEW_VERBOSE(Unit,HTMLUnorderedList(td, mpDocument),"HTMLUnorderedList");
	if (!Unit)
	{
		return (0);
	}

	AddElement(Unit);

	int nesting = Unit->Nesting();
	nesting = EBSCLIP(nesting, 0, 2);
	Unit->SetListStyleType(_bulletStyles[nesting]);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseUL (void)
{
	return (CloseType(HTML_LIST_ELEMENT));
}

HTMLElement *HtmlDocumentBuilder::OpenOL (HTMLTagParse *td)
{
HTMLList *Unit;

	mbSkipLeadingWhitespace = 1;

	WEBC_NEW_VERBOSE(Unit,HTMLOrderedList(td, mpDocument),"HTMLOrderedList");
	if (!Unit)
	{
		return (0);
	}

	AddElement(Unit);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseOL (void)
{
	return (CloseType(HTML_LIST_ELEMENT));
}


HTMLElement *HtmlDocumentBuilder::OpenBlockquote (HTMLTagParse *td)
{
HTMLBlockquote *Unit;

	mbSkipLeadingWhitespace = 1;

	WEBC_NEW_VERBOSE(Unit,HTMLBlockquote(td, mpDocument),"HTMLBlockquote");
	if (!Unit)
		return (0);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseBlockquote (void)
{
	return (CloseType(HTML_BLOCKQUOTE_ELEMENT));
}



HTMLElement *HtmlDocumentBuilder::OpenDL (HTMLTagParse *td)
{
HTMLDefinitionList *Unit;

	mbSkipLeadingWhitespace = 1;

	WEBC_NEW_VERBOSE(Unit,HTMLDefinitionList(td, mpDocument),"HTMLDefinitionList");
	if (!Unit)
		return (0);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseDL (void)
{
	return (CloseType(HTML_DEFINITION_LIST_ELEMENT, DivImplicitClose));
}


HTMLElement *HtmlDocumentBuilder::OpenDT (HTMLTagParse *td)
{
HTMLDefinitionTerm *Unit;

	CloseDD();
	CloseDT();

	mbSkipLeadingWhitespace = 1;

	WEBC_NEW_VERBOSE(Unit,HTMLDefinitionTerm(td, mpDocument),"HTMLDefinitionTerm");
	if (!Unit)
		return (0);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseDT (void)
{
	return (CloseType(HTML_DEFINITION_TERM_ELEMENT, DTImplicitClose));
}


HTMLElement *HtmlDocumentBuilder::OpenDD (HTMLTagParse *td)
{
HTMLDefinition *Unit;

	CloseDD();
	CloseDT();

	mbSkipLeadingWhitespace = 1;

	WEBC_NEW_VERBOSE(Unit,HTMLDefinition(td, mpDocument),"HTMLDefinition");
	if (!Unit)
		return (0);
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);

	return (Unit);
}

int HtmlDocumentBuilder::CloseDD (void)
{
	return (CloseType(HTML_DEFINITION_ELEMENT, DTImplicitClose));
}

void HtmlDocumentBuilder::OpenStyle(void)
{
#if (WEBC_SUPPORT_STYLE_SHEETS)
	StartTextCapture();
#endif //(WEBC_SUPPORT_STYLE_SHEETS)

	SetFlag(HDBLD_FLAG_IN_STYLE);
}


int HtmlDocumentBuilder::CloseStyle(void)
{
#if (WEBC_SUPPORT_STYLE_SHEETS)

	StopTextCapture();

	CSSDocumentContext * doc = mpDocument->GetCSSContext();

	if (doc)
	{
		if (CheckFlag(HDBLD_FLAG_IN_STYLE))
		{
			doc->ParseStyleSheet (
					mTextCapture.getChars(),
					CSS_ORIGIN_INTERNAL,
					mpDocument->CurrentURL(),
					1 /* lastChunk */
				);
		}

		doc->CleanUp();
	}

#endif //(WEBC_SUPPORT_STYLE_SHEETS)

	ClearFlag(HDBLD_FLAG_IN_STYLE);
	return 0;
}

void HtmlDocumentBuilder::Link(HTMLTagParse *td)
{
#if (WEBC_SUPPORT_STYLE_SHEETS)
	HTMLAttribValueParse* rel = HTML_GetTagAttribute(td, HTML_ATTRIB_REL);
	if (rel && (webc_c_stricmp(rel->str, "stylesheet") == 0))
	{
		CSSLoadExternalSheet * loadJob;
		HTMLAttribValueParse* media = HTML_GetTagAttribute(td, HTML_ATTRIB_MEDIA);
		if (!media || (webc_c_stristr(media->str, "all") != 0) || (webc_c_stristr(media->str, "screen") != 0))
		{
			HTMLAttribValueParse* href = HTML_GetTagAttribute(td, HTML_ATTRIB_HREF);
			if (href && mpDocument->GetCSSContext())
			{
				HTMLAttribValueParse* charsetName = HTML_GetTagAttribute(td, HTML_ATTRIB_CHARSET);
				WebcCharset charset;
				charset = (charsetName)? webc_charset_lookup(charsetName->str) : WEBC_CHARSET_UNKNOWN;

				URLDescriptor url_desc(href->str, mpDocument->BaseURL());

				HTMLBrowser *pBrowser = mpDocument->GetBrowser();
				pBrowser->Lock();

				WEBC_NEW_VERBOSE(loadJob, CSSLoadExternalSheet(mpDocument->GetCSSContext(), &url_desc, charset),"CSSLoadExternalSheet");
				if (loadJob)
				{
					pBrowser->ProcessLoadJob(loadJob, &url_desc, mpDocument->BaseURL(), mpDocument);
				  #ifdef WEBC_OLD_LOAD_METHOD
					webc_thread_load_browser(pBrowser);
				  #endif // WEBC_OLD_LOAD_METHOD
				}

				pBrowser->UnLock();
			}
		}
	}
#endif //(WEBC_SUPPORT_STYLE_SHEETS)
#if (REDIRECT_TO_MOBILE)
	else
	if (rel && (webc_c_stricmp(rel->str, "alternate") == 0))
	{
		HTMLAttribValueParse* media = HTML_GetTagAttribute(td, HTML_ATTRIB_MEDIA);
		if (media && media->str && (webc_c_stricmp(media->str, "handheld") == 0))
		{
			HTMLAttribValueParse* href = HTML_GetTagAttribute(td, HTML_ATTRIB_HREF);
			if (href && href->str)
			{
				URLDescriptor desc(href->str);
				Stop();
				mpDocument->LoadUrl(href->str, WEBC_TRUE);
			}
		}
	}
#endif //(WEBC_SUPPORT_STYLE_SHEETS)
}

WEBC_UINT16 HtmlDocumentBuilder::CheckFlag(WEBC_UINT16 mask)
{
	return mwFlags & mask;
}

void HtmlDocumentBuilder::SetFlag(WEBC_UINT16 mask)
{
	mwFlags |= mask;
}

void HtmlDocumentBuilder::ClearFlag(WEBC_UINT16 mask)
{
	mwFlags &= ~mask;
}

HTMLElement *HtmlDocumentBuilder::OpenIFrame (HTMLTagParse *td)
{
#if (WEBC_SUPPORT_IFRAMES && WEBC_SUPPORT_FRAMES)
	HTMLInlineFrame *Unit;

	WEBC_NEW_VERBOSE(Unit,HTMLInlineFrame(td, mpDocument),"HTMLInlineFrame");
	if (!Unit)
	{
		return (0);
	}
	Unit->SetTagProperties(td, mpDocument, &mAttribBuffer);

	AddElement(Unit);
	Unit->Update(0,0);
	SetFlag(HDBLD_FLAG_IN_IFRAME);

	return (Unit);
#else
	SetFlag(HDBLD_FLAG_IN_IFRAME);
	return (0);
#endif // WEBC_SUPPORT_IFRAMES
}

int HtmlDocumentBuilder::CloseIFrame (void)
{
	ClearFlag(HDBLD_FLAG_IN_IFRAME);
	return (0);
}

HTMLElement *HtmlDocumentBuilder::OpenNoFrames (HTMLTagParse *td)
{
	SetFlag(HDBLD_FLAG_IN_NOFRAMES);
	return (0);
}

int HtmlDocumentBuilder::CloseNoFrames (void)
{
	ClearFlag(HDBLD_FLAG_IN_NOFRAMES);
	return (0);
}

HTMLElement *HtmlDocumentBuilder::OpenNoScript (HTMLTagParse *td)
{
	SetFlag(HDBLD_FLAG_IN_NOSCRIPT);
	return (0);
}

int HtmlDocumentBuilder::CloseNoScript (void)
{
	ClearFlag(HDBLD_FLAG_IN_NOSCRIPT);
	return (0);
}
