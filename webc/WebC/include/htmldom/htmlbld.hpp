#ifndef __HTML_DOCUMENT_BUILDER__
#define __HTML_DOCUMENT_BUILDER__

#include "html.h"
#include "htmldom.hpp"
#include "htmldoc.hpp"
#include "hscript.hpp"
#include "webstr.hpp"
#include "helement.hpp"
#include "ref.hpp"

#define HDBLD_FLAG_IN_PRE            0x0001
#define HDBLD_FLAG_IN_SCRIPT         0x0002
#define HDBLD_FLAG_IN_STYLE          0x0004
#define HDBLD_FLAG_TEXT_CAPTURE_ON   0x0008
#define HDBLD_FLAG_IN_OPTION         0x0010
#define HDBLD_FLAG_IN_NOFRAMES       0x0020
#define HDBLD_FLAG_IN_NOSCRIPT       0x0040
#define HDBLD_FLAG_IN_IFRAME         0x0080
#define HDBLD_FLAG_IMPLICIT_CLOSE    0x0100
#define HDBLD_FLAG_STOPPED           0x0200

#define HDBLD_SUCCESS                0
#define HDBLD_RUN_SCRIPT             1
#define HDBLD_DONE                   2
#define HDBLD_STOPPED                3
#define HDBLD_ERR_CHARSET_CHANGE    -1
#define HDBLD_ERR_ABORTED           -2
#define HDBLD_ERR_REDIRECT          -3
#define HDBLD_ERR_UNSPECIFIED       -4

class HtmlDocumentBuilder
{
private:
	void StartTextCapture(void);
	void StopTextCapture(void);
	int  CloseType(HTMLElementType Type, MatchElementFn implicitClose = 0);
	void SetDepth (int newDepth);

	WEBC_UINT16 CheckFlag(WEBC_UINT16 mask);
	void SetFlag(WEBC_UINT16 mask);
	void ClearFlag(WEBC_UINT16 mask);

	int            miDepth;
	ObjectReference<HTMLElement> mElements[WEBC_CFG_MAX_ELEMENT_DEPTH];
	HTMLDocument*  mpDocument;
	HTMLForm*      mpCurrentForm;
	HTMLScript*    mpScript;
	WEBC_UINT8     mbSkipLeadingWhitespace;
	WebString      mTextCapture;
	WebString      mTextBuffer;
	WebString      mAttribBuffer;
	WEBC_UINT16    mwFlags;
	int            mState;
	int				pTagNesting; // keep track of P tags

public:
	HtmlDocumentBuilder(HTMLElement* pRoot);
	~HtmlDocumentBuilder(void);

	// Parse HTML source into document, return number of bytes successfully parsed
	long ParseHtml(WEBC_CHAR* pHtmlSource, long lHtmlLen, HTMLParseContext* parser);

	int GetState (void)
	{
		return (mState);
	}

    HTMLScript *GetScript(void)
	{
		return (mpScript);
	}

    WebString  *GetTextCapture(void)
    {
        return(&mTextCapture);
    }

    WebString   *GetTextBuffer(void)
    {
        return(&mTextBuffer);
    }

    WebString   *GetAttribBuffer(void)
    {
        return(&mAttribBuffer);
    }


	void SetState (int state)
	{
		mState = state;
	}

	void Stop (void)
	{
		SetFlag(HDBLD_FLAG_STOPPED);
	}

	int RunScript(void);
	void FlushText (void);

	HTMLElement* CurElement(void);
	void AddElement(HTMLElement* pChild);
	void InsertElement(HTMLElement* pChild);
	void InsertElementBefore(HTMLElement* pChild);
	void InsertElementAfter(HTMLElement* pChild);
	void ImplicitClose (void);
	int  CurrentHasParent(HTMLElementType type);
	void DoImpliedClose(HTMLTagType htmlTag);

	HTMLElement* Area(HTMLTagParse *td);
	HTMLElement* Base(HTMLTagParse *td);
	HTMLElement* Body(HTMLTagParse *td);
    HTMLElement* Br(HTMLTagParse *td);
	HTMLElement* Checkbox(HTMLTagParse *td);
	HTMLElement* EditStr(HTMLTagParse *td);
	HTMLElement* InputHidden(HTMLTagParse *td);
	HTMLElement* HorizontalRule(HTMLTagParse *td);
	HTMLElement* Image(HTMLTagParse *td);
 #if (WEBC_SUPPORT_EMBED)
	HTMLElement* Embed(HTMLTagParse *td);
 #endif
	HTMLElement* Object(HTMLTagParse *td);
	HTMLElement* Input(HTMLTagParse *td);
	HTMLElement* InputButton(HTMLTagParse *td);
	HTMLElement* InputReset(HTMLTagParse *td);
	HTMLElement* InputSubmit(HTMLTagParse *td);
	void		 Link(HTMLTagParse *td);
	HTMLElement* Marker(void);
	int          Meta(HTMLTagParse *td);
	HTMLElement* Password(HTMLTagParse *td);
	HTMLElement* RadioButton(HTMLTagParse *td);
	HTMLElement* Spacer(HTMLTagParse *td);
	HTMLElement* Text(WEBC_CHAR *Str);

	HTMLElement* OpenA (HTMLTagParse *td);
	int          CloseA (void);
	HTMLElement* OpenAddress (HTMLTagParse *td);
	int          CloseAddress (void);
	HTMLElement* OpenB (HTMLTagParse *td);
	int          CloseB (void);
	HTMLElement* OpenBig (HTMLTagParse *td);
	int          CloseBig (void);
	HTMLElement* OpenBlockquote (HTMLTagParse *td);
	int          CloseBlockquote (void);
	HTMLElement* OpenButton (HTMLTagParse *td);
	int          CloseButton (void);
	HTMLElement* OpenCenter (HTMLTagParse *td);
	int          CloseCenter (void);
	HTMLElement* OpenCite (HTMLTagParse *td);
	int          CloseCite (void);
	HTMLElement* OpenCode (HTMLTagParse *td);
	int          CloseCode (void);
	HTMLElement* OpenDD (HTMLTagParse *td);
	int          CloseDD (void);
	HTMLElement* OpenDel (HTMLTagParse *td);
	int          CloseDel (void);
	HTMLElement* OpenDiv (HTMLTagParse *td);
	int          CloseDiv (void);
	HTMLElement* OpenDL (HTMLTagParse *td);
	int          CloseDL (void);
	HTMLElement* OpenDT (HTMLTagParse *td);
	int          CloseDT (void);
	HTMLElement* OpenEm (HTMLTagParse *td);
	int          CloseEm (void);
	HTMLElement* OpenFont (HTMLTagParse *td);
	int          CloseFont (void);
	HTMLForm    *OpenForm (HTMLTagParse *td);
	int          CloseForm(void);
	HTMLElement* OpenHeading (HTMLTagParse *td);
	int          CloseHeading (void);
	HTMLElement* OpenI (HTMLTagParse *td);
	int          CloseI (void);
	HTMLElement* OpenIFrame (HTMLTagParse *td);
	int          CloseIFrame (void);
	HTMLElement* OpenIns (HTMLTagParse *td);
	int          CloseIns (void);
	HTMLElement* OpenKbd (HTMLTagParse *td);
	int          CloseKbd (void);
	HTMLElement* OpenLabel (HTMLTagParse *td);
	int          CloseLabel (void);
	HTMLElement* OpenLI (HTMLTagParse *td);
	int          CloseLI (void);
	HTMLElement* OpenMap (HTMLTagParse *td);
	int          CloseMap (void);
	HTMLElement* OpenNoBr (HTMLTagParse *td);
	int          CloseNoBr (void);
	HTMLElement* OpenNoFrames (HTMLTagParse *td);
	int          CloseNoFrames (void);
	HTMLElement* OpenNoScript (HTMLTagParse *td);
	int          CloseNoScript (void);
	HTMLElement* OpenOL (HTMLTagParse *td);
	int          CloseOL (void);
	HTMLElement* OpenOption (HTMLTagParse *td);
	int          CloseOption (void);
	HTMLElement* OpenP (HTMLTagParse *td);
	int          CloseP (void);
	HTMLElement* OpenPre (HTMLTagParse *td);
	int          ClosePre (void);
	HTMLElement* OpenSelect (HTMLTagParse *td);
	int          CloseSelect (void);
	HTMLElement* OpenSmall (HTMLTagParse *td);
	int          CloseSmall (void);
	HTMLElement* OpenSpan (HTMLTagParse *td);
	int          CloseSpan (void);
	HTMLElement* OpenStrike (HTMLTagParse *td);
	int          CloseStrike (void);
	HTMLElement* OpenStrong (HTMLTagParse *td);
	int          CloseStrong (void);
	HTMLElement* OpenTextArea (HTMLTagParse *td);
	int          CloseTextArea (void);
	HTMLElement* OpenTitle (HTMLTagParse *td);
	int          CloseTitle (void);
	HTMLElement* OpenU (HTMLTagParse *td);
	int          CloseU (void);
	HTMLElement* OpenUL (HTMLTagParse *td);
	int          CloseUL (void);
	HTMLElement* OpenScript (HTMLTagParse *td);
	int 		 CloseScript (void);

  #if (WEBC_SUPPORT_FRAMES)
	HTMLElement* Frame(HTMLTagParse *td);
	HTMLElement* OpenFrameset (HTMLTagParse *td);
	int          CloseFrameset (void);
  #endif // WEBC_SUPPORT_FRAMES

  #if (WEBC_SUPPORT_TABLES)
	HTMLElement* OpenTable (HTMLTagParse *td);
	int          CloseTable (void);
	HTMLElement* OpenTd (HTMLTagParse *td);
	int          CloseTd (void);
	HTMLElement* OpenTr (HTMLTagParse *td);
	int          CloseTr (void);
  #endif // WEBC_SUPPORT_TABLES

	void 		 OpenStyle(void);
	int			 CloseStyle(void);

};

#endif
