//
// AppBuilder.h - WebC Screen Builder
//
// EBS - WebC
//
// Copyright EBS Inc.
//
// Module description:
//

#include "htmlsdk.h"
#include "rtpstr.h"
#include "rtpprint.h"
#include "rtpmem.h"
#include "rtpscnv.h"
#include "ixml.h"
#include <assert.h>

// Application content.
#define MAINAPPLICATIONURL (char *)"file://\\AppBuilder\\MainWindow.html"
#define COMPOSEWINDOWID "ComposeWindow"


/* FixME Used to position element on prototype pallete, must change to xml */
#define PALLETEROWS		3
#define PALLETECOLUMNS	3
#define PALLETEXPADDING	10
#define PALLETEYPADDING	10
#define PALLETEELHEIGHT	50
#define PALLETEELWIDTH	50
#define ROW0		30
#define ROW1		100
#define ROW2	   	150
#define COLUMN0		10
#define COLUMN1		70
#define COLUMN2	   	130

#define PALETTEXPOS(COLUMNNUMBER) (COLUMN0 + ((PALLETEELWIDTH+PALLETEXPADDING)*COLUMNNUMBER))
#define PALETTEYPOS(ROWNUMBER) (ROW0 + ((PALLETEELHEIGHT+PALLETEYPADDING)*ROWNUMBER))

//#define IDOBJ0X0 "Obj0x0"

#define IDOBJ0X1 "Obj0x1"
#define IDOBJ0X2 "Obj0x2"
#define IDOBJ1X0 "Obj1x0"
#define IDOBJ1X1 "Obj1x1"
#define IDOBJ1X2 "Obj1x2"
#define IDOBJ2X0 "Obj2x0"
#define IDOBJ2X1 "Obj2x1"
#define IDOBJ2X2 "Obj2x2"

#define IDTEXTBOX "TextBox"
#define SRCTEXTBOX "file:\\AppBuilder\\Myimage0X0.PNG"
#define SRCTEXTBOXSHADOW "file:\\AppBuilder\\Myimage0X0Shadow.PNG"

#define SRCTESTXML "\\AppBuilder\\XML\\Test.xml"
#define DESTTESTXML "\\AppBuilder\\XML\\TestOutput.xml"



#define SRCOBJ0X1 "file:\\AppBuilder\\Myimage0X1.PNG"
#define SRCOBJ0X2 "file:\\AppBuilder\\Myimage0X2.PNG"

#define SRCOBJ1X0 "file:\\AppBuilder\\Myimage.PNG"
#define SRCOBJ1X1 "file:\\AppBuilder\\Myimage.PNG"
#define SRCOBJ1X2 "file:\\AppBuilder\\Myimage.PNG"

#define SRCOBJ2X0 "file:\\AppBuilder\\Myimage.PNG"
#define SRCOBJ2X1 "file:\\AppBuilder\\Myimage.PNG"
#define SRCOBJ2X2 "file:\\AppBuilder\\Myimage.PNG"




#define PALETTEZINDEX		20
#define SHADOWZINDEX		30
#define SPRITEZINDEX		 5

// Start up
int ConstructwebCPalleteObject(HBROWSER_HANDLE hbrowser,HDOC_HANDLE hdoc);
int ConstructwebCStoryBoard(HDOC_HANDLE hdoc);
int CheckStoryBoardPalleteObjectCollision(int X, int Y);
int DropObjectOnStoryBoard(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, int X, int Y, char *ObjectId);
int LoadStoryBoardFromSreenDocument(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, IXML_Document *Doc);

//
#define INSTALLDIRECTORY "\\AppBuilder"
#define	TEMPLATEDIRECTORY INSTALLDIRECTORY"\\XML\\Templates"
#define	COMPONENTTEMPLATEFILE TEMPLATEDIRECTORY"\\WebcComponents.xml"
#define	DIALOGUETEMPLATEFILE TEMPLATEDIRECTORY"\\AppbuilderDialogues.xml"
#define	PALLETTEMPLATEFILE TEMPLATEDIRECTORY"\\AppbuilderPallet.xml"
#define TESTPROJECT "\\AppBuilder\\XML\\TestProject\\TestProject.xml"
#define STORYBOARDABSX 400
#define STORYBOARDABSY 10

extern IXML_Document *ComponentTemplate;
extern IXML_Document *DialogueTemplate;
extern IXML_Document *TestProject;
extern IXML_Document *PalletTemplate;

#define STORYBOARDDOMSTR "root.WebCAppobjects"
#define PALLETDOMSTR "root.PalletImages"

IXML_Element* rtpxmlGetElemInElemFromDomStr(IXML_Element* Elem, char *inDomStr);
IXML_Element* rtpxmlGetElemInDocFromDomStr(IXML_Document *Doc, char *inDomStr);
int rtpxmlSaveDocument(IXML_Document *p, char *xmlFileName);
char *rtpxmlGetElemTextInElem (IXML_Element *parent, char *tagName);
static IXML_Element *rtpxmlGetElemInElem (IXML_Element *parent, char *tagName, int num);
int WebcAppGenXmlLLoadTemplates(void);
int WebcAppGenXmlLoadChildArray(IXML_Element *ParentElement, IXML_Element **ChildArray, int ArraySize);

IXML_Element *newxmlAppElement(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, char *ObjectId);
int xmlAppElementToHtml(IXML_Element *xmlElement, char *tohtml);
int xmlPalletElementToHtml(IXML_Element *xmlElement, char *tohtml, int isShadow);
int rtpxmlSetElemTextInElem (IXML_Element *parent, char *tagName,char *Value);


int ConstructEventDialogue(HDOC_HANDLE hdoc);
int ShowEventDialog(HDOC_HANDLE hdoc, HELEMENT_HANDLE htmlStoryBoardElement,IXML_Element *xmlStoryBoardElement);
int ShowStyleDialog(HDOC_HANDLE hdoc, HELEMENT_HANDLE htmlStoryBoardElement,IXML_Element *xmlStoryBoardElement);
