#ifndef __WCAPI_INCLUDED__
#define __WCAPI_INCLUDED__
//
// wcapi.h -
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

#include "webctypes.h"
#include "htmlsdk.h"   /* For element types, remove later */
#include "listener.hpp"   /* For litener events */
#include "webc.h"
#include "webcmem.h"
#include "webcjhconfig.h"

#if (WEBC_SUPPORT_JSCRIPT)
 #include "wjsdoc.h"
 #include "wjsbrowser.h"
 #include "wjscript.hpp"
#endif

typedef unsigned char * wcEL;
typedef unsigned char * wcDOC;
typedef unsigned char * wcBROW;
#define HELEMENT_OBJ(X)           ((HTMLElement *)(X))
#define DELEMENT_HANDLE void *
#define HBROWSER_OBJ(X)           ((HTMLBrowser *)(X))
#define HDOC_OBJ(X)               ((HTMLDocument *)(X))


#define MAXIDSIZE 256
#define HTML_TEMP_STR_LEN         256

enum e_wcBOOL
{
	wcFALSE,
	wcTRUE,
};
typedef enum e_wcBOOL wcBOOL;

#define wcMODAL		1
/* Not completed yet */
#define wcRESIZE	2
#define wcSYSTEM	4
#define wcBORDER	8
typedef unsigned long wcWINFLAGS;

enum DomTraverseResult
{
    DomError=-1,
    DomFind,
    DomSelf,
    DomParent,
    DomFirstChild,
    DomLastChild,
    DomPrevSibling,
    DomNextSibling,
};


#define MAXGARBAGEPOINTERS 256
struct s_wcCtx
{
	wcDOC hdoc;
	wcBROW hbrowser;
	char *GarbageList[MAXGARBAGEPOINTERS];
};
typedef struct s_wcCtx wcCtx;

char * wcCtxtAppendGarbage(wcCtx* Ctx, char *p);

typedef wcBOOL (*wcDocEnumFn)(wcCtx* Ctx, wcEL El, int UserIntParm, void *UserVoidParm);
typedef HTMLEventStatus (*wcEventCallback) (wcCtx* Ctx,wcEL element,HTMLEvent* event,char* param);
typedef void (*wcTimedCallbackfn)(int UserIntParm, void *UserVoidParm);
typedef HTMLMetaStatus (*HTMLMetaCallbackFn)(const char* pContent, wcCtx *Ctx);
/* Sys functions */
void wcCtxtInit(wcCtx* Ctx, wcBROW browser, wcDOC doc);
void wcCtxtRelease(wcCtx* Ctx);
char * wcCtxtAppendGarbage(wcCtx* Ctx, char *p);
int wcAddFsTree(const struct WebcVFileEntry *TableName);
void wcTimedCallback(wcTimedCallbackfn pfn,int delay, int IntParm, void *VoidParm);
/* wcEL Functions */

void wcElSetInnerHtml(wcEL El,	char * newhtml);
char * wcElGetInnerHtml (wcCtx* Ctx, wcEL El);
char * wcElGetOuterHtml (wcCtx* Ctx, wcEL El);
wcDOC wcElGetFrameDocument(wcEL element);

wcBOOL wcElRemoveChild (wcEL parent,wcEL child,wcBOOL refresh);
wcBOOL wcElDeleteChildren (wcEL element,wcBOOL refresh);
wcBOOL wcElDelete(wcEL element,wcBOOL refresh);
wcBOOL wcElInsertFirst(wcEL element,wcEL child,wcBOOL refresh);
wcBOOL wcElInsertLast(wcEL element,wcEL child,wcBOOL refresh);
wcBOOL wcElInsertAfter(wcEL Parent,wcEL CurChild, wcEL NewChild,wcBOOL refresh);
wcBOOL wcElInsertBefore(wcEL Parent,wcEL CurChild,wcEL NewChild,wcBOOL refresh);
wcBOOL wcElIdMatches(wcEL element, char *string);
wcEL wcElGetParent(wcEL element);
wcEL wcElGetChild(wcEL element);
wcEL wcElGetPrev(wcEL element);
wcEL wcElGetNext(wcEL element);
wcEL wcElFindNext(wcEL element, char *id, char *name, HTMLElementType type, long index);
wcEL wcElFindPrev(wcEL element, char *id, char *name, HTMLElementType type, long index);
wcEL wcElFindChild (wcEL element, char *id, char *name, HTMLElementType type, long index);
wcEL wcElClone(wcEL element, wcBOOL bCloneChildren);
void wcElRefresh(wcEL element);
wcBOOL wcElSetStyle(wcEL element, char *_style,wcBOOL doUpdate);
wcBOOL wcElSetId(wcEL element, char *_Id);
int wcElSetName(wcEL element, char *_name);
wcBOOL wcElSetChecked(wcEL element,wcBOOL checked,wcBOOL refresh);
wcBOOL wcElSetText(wcEL element, char *_Val,wcBOOL refresh);
wcBOOL wcElSetValue(wcEL element, char *_Val);
int wcElSetSrc(wcEL element, char *_src, wcBOOL refresh);
void wcElSetBgImage(wcEL element, char *_bgimg, wcBOOL refresh);
int wcElEnable(wcEL element, wcBOOL refresh);
int wcElDisable(wcEL element, wcBOOL refresh);
int wcElSetSelected(HELEMENT_HANDLE element, int selected, wcBOOL refresh);
int wcGetSelected(HELEMENT_HANDLE element);
wcBOOL wcElGetChecked(wcEL element);
char *wcELGetId(wcCtx* Ctx,wcEL element);
wcBOOL wcElGetIdMatch(wcEL element, char *string);
char *wcElGetName(wcCtx* Ctx,wcEL element);
char *wcELGetValue(wcCtx* Ctx,wcEL element);
HTMLElementType wcElGetType(wcEL element);
const char* wcGetTypeString(HTMLElementType  TypeVal);
wcDOC wcElGetDocument(wcEL element);
char* wcElGetSrc(wcCtx* Ctx,wcEL element);
int wcElGetColorValue(wcEL element, CSSPropertyType Prop, HTMLColor* color);
int  wcElGetLengthValue(HELEMENT_HANDLE element,  CSSPropertyType Prop, HTMLLength *Length);
wcBOOL wcElGetPosStyle(wcEL element, int *top, int *left, int *height, int *width);
void wcElSetStylePos(wcEL element, int top, int left,wcBOOL refresh);
void wcElSetStyleDim(wcEL element, int height, int width, wcBOOL refresh);
void wcElSetStyleHeight(wcEL element, int height, wcBOOL refresh);
void wcElSetStyleWidth(wcEL element, int width, wcBOOL refresh);
void wcElSetStyleZindex(wcEL element, int zorder, wcBOOL refresh);
void wcElSetStyleColorRgb(wcEL element,CSSPropertyType Prop, int r, int g, int b, wcBOOL refresh);
void wcElSetStyleColorRgba(wcEL element,CSSPropertyType Prop, int r, int g, int b, int a, wcBOOL refresh);
void wcElSetStyleColorNamed(wcEL element,CSSPropertyType Prop, char *ColorName, wcBOOL refresh);
void wcElSetStyleOpacity(wcEL element, int Opacity, wcBOOL refresh);
void wcElSetStyleColorHtml(wcEL element,CSSPropertyType Prop, HTMLColor color, wcBOOL refresh);
void wcElSetStyleBgImage(wcEL element, char *bgImage, wcBOOL refresh);
void wcElSetStyleBorder(wcEL element, int Thickness, char *Color, wcBOOL refresh);
void wcElSetStyleHidden(wcEL element, wcBOOL refresh);
void wcElSetStyleVisible(wcEL element, wcBOOL refresh);
int wcElSetFocus(wcEL element, wcBOOL refresh);
void wcElClaimPointer(wcEL element);
void wcElReleasePointer(wcEL element);
#ifdef RTP_TRACK_LOCAL_MEMORY
void _wcElSetEventHandler(HELEMENT_HANDLE element, wcEventCallback cb,const char *file, const long line);
#define wcElSetEventHandler(E,F) _wcElSetEventHandler(E,F,__FILE__,__LINE__)
void _wcDocSetEventHandler(wcCtx* Ctx, wcEventCallback cb, const char *file, const long line);
#define wcDocSetEventHandler(C,F) _wcDocSetEventHandler(C, F, __FILE__, __LINE__)
#else
void wcElSetEventHandler(HELEMENT_HANDLE element, wcEventCallback cb);
void wcDocSetEventHandler(wcCtx* Ctx, wcEventCallback cb);
#endif
void wcElPopEventHandler(wcEL  element);
HTMLEventStatus wcElTriggerEvent(wcEL El, HTMLEvent *pEvent, int delay);

void wcDocPopEventHandler(wcCtx *Ctx);
HTMLEventStatus wcDocTriggerEvent(wcCtx *Ctx, HTMLEvent *pEvent, int delay);


wcBOOL wcElGetPosWin(wcEL element, int *top, int *left, int *width, int *height);
void wcPushData(wcCtx *Ctx, wcEL  Element, void* data);
void * wcGetData(wcCtx *Ctx, wcEL  Element);
void wcPopData(wcCtx *Ctx, wcEL  Element);

void wcDocEnumerate(wcCtx* Ctx, wcDocEnumFn pfn, int Intparm, void *Voidparm);
wcEL wcDocGetBody (wcCtx* Ctx);
wcEL wcDocFindEl (wcCtx* Ctx, char *id);
wcEL wcELFind (wcCtx* Ctx, wcEL El, char *Domstr, wcBOOL *pError);
wcEL wcDocGetFocus(wcDOC Doc);
void wcDocInsertFirst (wcCtx* Ctx,wcEL eL);
void wcDocInsertLast (wcCtx* Ctx,wcEL eL);
wcEL wcDocFindElement (wcCtx* Ctx, char *id, char *name, HTMLElementType type, long index);
void wcDocSetUrl(wcCtx* Ctx, char *_url, wcBOOL refresh);
void wcDocRefresh(wcCtx* Ctx);
void wcDocClear(wcCtx* Ctx, wcBOOL refresh);
long wcDocWriteHtml(wcCtx* Ctx, char *html_src, long length, wcBOOL refresh);
long wcDocWriteString(wcCtx* Ctx, char *str, long length, wcBOOL refresh);
int wcDocOpen(wcCtx* Ctx);
void wcDocClose(wcCtx* Ctx);
wcBROW wcDocGetBrowser(wcDOC doc);
HELEMENT_HANDLE webc_DocFindElement (HDOC_HANDLE doc, char *id, char *name, HTMLElementType type, long index);
HBROWSER_HANDLE webc_DocGetBrowser(HDOC_HANDLE doc);
void webc_DocSaveForOffline(HDOC_HANDLE hdoc);
void webc_SetOnlineStatus (	WEBC_BOOL isOnline);

int wcWinCreateEx(wcCtx* Ctx,char *_windowname,	wcEventCallback BodyCallback,char *_defaultUrl,	char *_defaultContent,struct HTMLBrowserWindowConfig *pconf);
int wcWinExecute(wcCtx *Ctx);
int wcWinLoadUrl(wcCtx *Ctx, char *_url_str);
void wcWinExecuteStop(wcCtx *Ctx, int ReturnValue);
void wcWinClose(wcCtx *Ctx);
void wcPushData(wcCtx *Ctx, wcEL  Element, void* data);
void * wcGetData(wcCtx *Ctx, wcEL  Element);
void wcPopData(wcCtx *Ctx, wcEL  Element);
void wcWinClaimFocus(wcCtx *Ctx);
void wcWinReleaseFocus(wcCtx *Ctx);
wcBOOL wcWinSetFocus(wcCtx *Ctx,wcEL  Element);
int wcWinGetFocusCtx(wcCtx *Ctx);
wcEL wcWinGetFocus(wcCtx *Ctx);
void wcWinMoveTo(wcCtx *Ctx,int left, int top);
void wcWinBringToTop(wcCtx *Ctx);
wcBOOL wcWinGetPos(wcCtx *Ctx, int *left, int *top, int *width, int *height);
wcBOOL wcWinSetPos(wcCtx *Ctx, int left, int top, int width, int height);
#ifdef RTP_TRACK_LOCAL_MEMORY
#define wcCreateElement(TY,TA,DOC) _wcCreateElement(TY,TA,DOC,__FILE__,__LINE__);
HTMLElement *_wcCreateElement(HTMLElementType eType, HTMLTagParse *tag, HTMLDocument *pDoc, const char *file, const long line);
#else
HTMLElement *wcCreateElement(HTMLElementType eType, HTMLTagParse *tag, HTMLDocument *pDoc);
#endif

#ifdef RTP_TRACK_LOCAL_MEMORY
wcEL _wcElNew(wcCtx* Ctx, char *_name, const char *file, const long line);
wcEL _wcNewStyledDiv(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style, const char *file, const long line);
wcEL _wcNewStyledButtonText(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, char *text, int top, int left, int height, int width, int zindex, char *color, char *bgcolor, char *style, const char *file, const long line);
wcEL _wcNewStyledFont(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor, char *style, const char *file, const long line);
wcEL _wcNewStyledTextEdit(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style, const char *file, const long line);
wcEL _wcNewStyledTextArea(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style, const char *file, const long line);
wcEL _wcNewStyledImage(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, char *url, char *alttext,int top, int left, int height, int width, int zindex, char *style, const char *file, const long line);
#define wcElNew(C,N) _wcElNew(C,N,__FILE__, __LINE__);
#define wcNewStyledDiv(A,B,C,D,E,F,G,K,L,M,N,O)	_wcNewStyledDiv(A,B,C,D,E,F,G,K,L,M,N,O,__FILE__,__LINE__)
#define wcNewStyledButtonText(A,B,C,D,E,F,G,K,L,M,N,O) _wcNewStyledButtonText(A,B,C,D,E,F,G,K,L,M,N,O,__FILE__,__LINE__)
#define wcNewStyledTextEdit(A,B,C,D,E,F,G,K,L,M,N,O) _wcNewStyledTextEdit(A,B,C,D,E,F,G,K,L,M,N,O,__FILE__,__LINE__)
#define wcNewStyledTextArea(A,B,C,D,E,F,G,K,L,M,N,O) _wcNewStyledTextArea(A,B,C,D,E,F,G,K,L,M,N,O,__FILE__,__LINE__)
#define wcNewStyledImage(A,B,C,D,E,F,G,K,L,M,N) _wcNewStyledImage(A,B,C,D,E,F,G,K,L,M,N,__FILE__,__LINE__)
#define wcNewStyledFont(A,B,C,D,E,F,G,K,L,M,N)	_wcNewStyledFont(A,B,C,D,E,F,G,K,L,M,N,__FILE__,__LINE__)
#else
wcEL wcElNew(wcCtx* Ctx, char *_name);
wcEL wcNewStyledDiv(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style);
wcEL wcNewStyledButtonText(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, char *text, int top, int left, int height, int width, int zindex, char *color, char *bgcolor, char *style);
wcEL wcNewStyledFont(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor, char *style);
wcEL wcNewStyledTextEdit(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style);
wcEL wcNewStyledTextArea(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int zindex, char *color, char *bgcolor,char *bgimage, char *style);
wcEL wcNewStyledImage(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, char *url, char *alttext,int top, int left, int height, int width, int zindex, char *style);
#endif

wcEL wcNewStyledTable(wcCtx* Ctx, wcEL ParentEl, wcBOOL insert, int top, int left, int height, int width, int borderwidth, int cellspacing,int cellpadding,int zindex, char *color, char *bgcolor,char *bgimage, char *style);
wcEL wcTableInsertRow(wcCtx* Ctx, wcEL TableEl, CSSTextAlign align, CSSVerticalAlign valign, char *color, char *bgcolor,char *bgimage, char *style);
wcEL wcTableInsertCell(wcCtx* Ctx, wcEL TableRow, char *color, char *bgcolor,char *bgimage, char *style);
wcEL wcTableInsertHeaderCell(wcCtx* Ctx, wcEL TableRow, char *color, char *bgcolor,char *bgimage, char *style);
void wcTableCellSetSpans(wcCtx* Ctx, wcEL TableCell, int colspan, int rowspan);
void wcTableCellSetWidth(wcCtx* Ctx, wcEL TableCell, int width);
void wcTableCellSetHeight(wcCtx* Ctx, wcEL TableCell, int height);
wcEL wcTableGetCell(wcCtx* Ctx, wcEL TableEl, int row, int collumn);
wcEL wcTableGetRow(wcCtx* Ctx, wcEL TableEl, int row);

void wcImageGraphBeginDraw(wcEL ImageElement);
void wcImageGraphEndDraw(wcEL ImageElement);
void wcImageGraphRefreshSrc(wcEL ImageElement);
void wcImageGraphSetBrushWidth(wcEL ImageElement, int width);
void wcImageGraphSetPenColor(wcEL ImageElement, HTMLColor color);
void wcImageGraphDrawLine(wcEL ImageElement,int x0, int y0, int x1, int y1);
void wcImageGraphDrawPolygon(wcEL ImageElement, int ArrayLength, int *coordArray, wcBOOL fill);
void wcImageGraphDrawRect(wcEL ImageElement, int x, int y, int w, int h, wcBOOL fill);
void wcImageGraphDrawEllipse(wcEL ImageElement, int x, int y, int w, int h, wcBOOL fill);
void wcImageGraphArc(wcEL ImageElement, int ix, int iy, int ir1, int ir2, int ia1, int ia2, wcBOOL fill);


#define WEBC_BROWSER_LOAD_ALL(B)  HBROWSER_OBJ(B)->LoadAll()

#define QUOTERGBA(x) #x
#define rgbaAliceBlue(ALPHA) QUOTERGBA(rgba(240,248,255,ALPHA))
#define rgbaAntiqueWhite(ALPHA) QUOTERGBA(rgba(250,235,215,ALPHA))
#define rgbaAqua(ALPHA) QUOTERGBA(rgba(0,255,255,ALPHA))
#define rgbaAquamarine(ALPHA) QUOTERGBA(rgba(127,255,212,ALPHA))
#define rgbaAzure(ALPHA) QUOTERGBA(rgba(240,255,255,ALPHA))
#define rgbaBeige(ALPHA) QUOTERGBA(rgba(245,245,220,ALPHA))
#define rgbaBisque(ALPHA) QUOTERGBA(rgba(255,228,196,ALPHA))
#define rgbaBlack(ALPHA) QUOTERGBA(rgba(0,0,0,ALPHA))
#define rgbaBlanchedAlmond(ALPHA) QUOTERGBA(rgba(255,235,205,ALPHA))
#define rgbaBlue(ALPHA) QUOTERGBA(rgba(0,0,255,ALPHA))
#define rgbaBlueViolet(ALPHA) QUOTERGBA(rgba(138,43,226,ALPHA))
#define rgbaBrown(ALPHA) QUOTERGBA(rgba(165,42,42,ALPHA))
#define rgbaBurlyWood(ALPHA) QUOTERGBA(rgba(222,184,135,ALPHA))
#define rgbaCadetBlue(ALPHA) QUOTERGBA(rgba(95,158,160,ALPHA))
#define rgbaChartreuse(ALPHA) QUOTERGBA(rgba(127,255,0,ALPHA))
#define rgbaChocolate(ALPHA) QUOTERGBA(rgba(210,105,30,ALPHA))
#define rgbaCoral(ALPHA) QUOTERGBA(rgba(255,127,80,ALPHA))
#define rgbaCornflowerBlue(ALPHA) QUOTERGBA(rgba(100,149,237,ALPHA))
#define rgbaCornsilk(ALPHA) QUOTERGBA(rgba(255,248,220,ALPHA))
#define rgbaCrimson(ALPHA) QUOTERGBA(rgba(220,20,60,ALPHA))
#define rgbaCyan(ALPHA) QUOTERGBA(rgba(0,255,255,ALPHA))
#define rgbaDarkBlue(ALPHA) QUOTERGBA(rgba(0,0,139,ALPHA))
#define rgbaDarkCyan(ALPHA) QUOTERGBA(rgba(0,139,139,ALPHA))
#define rgbaDarkGoldenRod(ALPHA) QUOTERGBA(rgba(184,134,11,ALPHA))
#define rgbaDarkGray(ALPHA) QUOTERGBA(rgba(169,169,169,ALPHA))
#define rgbaDarkGreen(ALPHA) QUOTERGBA(rgba(0,100,0,ALPHA))
#define rgbaDarkKhaki(ALPHA) QUOTERGBA(rgba(189,183,107,ALPHA))
#define rgbaDarkMagenta(ALPHA) QUOTERGBA(rgba(139,0,139,ALPHA))
#define rgbaDarkOliveGreen(ALPHA) QUOTERGBA(rgba(85,107,47,ALPHA))
#define rgbaDarkorange(ALPHA) QUOTERGBA(rgba(255,140,0,ALPHA))
#define rgbaDarkOrchid(ALPHA) QUOTERGBA(rgba(153,50,204,ALPHA))
#define rgbaDarkRed(ALPHA) QUOTERGBA(rgba(139,0,0,ALPHA))
#define rgbaDarkSalmon(ALPHA) QUOTERGBA(rgba(233,150,122,ALPHA))
#define rgbaDarkSeaGreen(ALPHA) QUOTERGBA(rgba(143,188,143,ALPHA))
#define rgbaDarkSlateBlue(ALPHA) QUOTERGBA(rgba(72,61,139,ALPHA))
#define rgbaDarkSlateGray(ALPHA) QUOTERGBA(rgba(47,79,79,ALPHA))
#define rgbaDarkTurquoise(ALPHA) QUOTERGBA(rgba(0,206,209,ALPHA))
#define rgbaDarkViolet(ALPHA) QUOTERGBA(rgba(148,0,211,ALPHA))
#define rgbaDeepPink(ALPHA) QUOTERGBA(rgba(255,20,147,ALPHA))
#define rgbaDeepSkyBlue(ALPHA) QUOTERGBA(rgba(0,191,255,ALPHA))
#define rgbaDimGray(ALPHA) QUOTERGBA(rgba(105,105,105,ALPHA))
#define rgbaDodgerBlue(ALPHA) QUOTERGBA(rgba(30,144,255,ALPHA))
#define rgbaFeldspar(ALPHA) QUOTERGBA(rgba(209,146,117,ALPHA))
#define rgbaFireBrick(ALPHA) QUOTERGBA(rgba(178,34,34,ALPHA))
#define rgbaFloralWhite(ALPHA) QUOTERGBA(rgba(255,250,240,ALPHA))
#define rgbaForestGreen(ALPHA) QUOTERGBA(rgba(34,139,34,ALPHA))
#define rgbaFuchsia(ALPHA) QUOTERGBA(rgba(255,0,255,ALPHA))
#define rgbaGainsboro(ALPHA) QUOTERGBA(rgba(220,220,220,ALPHA))
#define rgbaGhostWhite(ALPHA) QUOTERGBA(rgba(248,248,255,ALPHA))
#define rgbaGold(ALPHA) QUOTERGBA(rgba(255,215,0,ALPHA))
#define rgbaGoldenRod(ALPHA) QUOTERGBA(rgba(218,165,32,ALPHA))
#define rgbaGray(ALPHA) QUOTERGBA(rgba(128,128,128,ALPHA))
#define rgbaGreen(ALPHA) QUOTERGBA(rgba(0,128,0,ALPHA))
#define rgbaGreenYellow(ALPHA) QUOTERGBA(rgba(173,255,47,ALPHA))
#define rgbaHoneyDew(ALPHA) QUOTERGBA(rgba(240,255,240,ALPHA))
#define rgbaHotPink(ALPHA) QUOTERGBA(rgba(255,105,180,ALPHA))
#define rgbaIndianRed(ALPHA) QUOTERGBA(rgba(205,92,92,ALPHA))
#define rgbaIndigo(ALPHA) QUOTERGBA(rgba(75,0,130,ALPHA))
#define rgbaIvory(ALPHA) QUOTERGBA(rgba(255,255,240,ALPHA))
#define rgbaKhaki(ALPHA) QUOTERGBA(rgba(240,230,140,ALPHA))
#define rgbaLavender(ALPHA) QUOTERGBA(rgba(230,230,250,ALPHA))
#define rgbaLavenderBlush(ALPHA) QUOTERGBA(rgba(255,240,245,ALPHA))
#define rgbaLawnGreen(ALPHA) QUOTERGBA(rgba(124,252,0,ALPHA))
#define rgbaLemonChiffon(ALPHA) QUOTERGBA(rgba(255,250,205,ALPHA))
#define rgbaLightBlue(ALPHA) QUOTERGBA(rgba(173,216,230,ALPHA))
#define rgbaLightCoral(ALPHA) QUOTERGBA(rgba(240,128,128,ALPHA))
#define rgbaLightCyan(ALPHA) QUOTERGBA(rgba(224,255,255,ALPHA))
#define rgbaLightGoldenRodYellow(ALPHA) QUOTERGBA(rgba(250,250,210,ALPHA))
#define rgbaLightGrey(ALPHA) QUOTERGBA(rgba(211,211,211,ALPHA))
#define rgbaLightGreen(ALPHA) QUOTERGBA(rgba(144,238,144,ALPHA))
#define rgbaLightPink(ALPHA) QUOTERGBA(rgba(255,182,193,ALPHA))
#define rgbaLightSalmon(ALPHA) QUOTERGBA(rgba(255,160,122,ALPHA))
#define rgbaLightSeaGreen(ALPHA) QUOTERGBA(rgba(32,178,170,ALPHA))
#define rgbaLightSkyBlue(ALPHA) QUOTERGBA(rgba(135,206,250,ALPHA))
#define rgbaLightSlateBlue(ALPHA) QUOTERGBA(rgba(132,112,255,ALPHA))
#define rgbaLightSlateGray(ALPHA) QUOTERGBA(rgba(119,136,153,ALPHA))
#define rgbaLightSteelBlue(ALPHA) QUOTERGBA(rgba(176,196,222,ALPHA))
#define rgbaLightYellow(ALPHA) QUOTERGBA(rgba(255,255,224,ALPHA))
#define rgbaLime(ALPHA) QUOTERGBA(rgba(0,255,0,ALPHA))
#define rgbaLimeGreen(ALPHA) QUOTERGBA(rgba(50,205,50,ALPHA))
#define rgbaLinen(ALPHA) QUOTERGBA(rgba(250,240,230,ALPHA))
#define rgbaMagenta(ALPHA) QUOTERGBA(rgba(255,0,255,ALPHA))
#define rgbaMaroon(ALPHA) QUOTERGBA(rgba(128,0,0,ALPHA))
#define rgbaMediumAquaMarine(ALPHA) QUOTERGBA(rgba(102,205,170,ALPHA))
#define rgbaMediumBlue(ALPHA) QUOTERGBA(rgba(0,0,205,ALPHA))
#define rgbaMediumOrchid(ALPHA) QUOTERGBA(rgba(186,85,211,ALPHA))
#define rgbaMediumPurple(ALPHA) QUOTERGBA(rgba(147,112,216,ALPHA))
#define rgbaMediumSeaGreen(ALPHA) QUOTERGBA(rgba(60,179,113,ALPHA))
#define rgbaMediumSlateBlue(ALPHA) QUOTERGBA(rgba(123,104,238,ALPHA))
#define rgbaMediumSpringGreen(ALPHA) QUOTERGBA(rgba(0,250,154,ALPHA))
#define rgbaMediumTurquoise(ALPHA) QUOTERGBA(rgba(72,209,204,ALPHA))
#define rgbaMediumVioletRed(ALPHA) QUOTERGBA(rgba(199,21,133,ALPHA))
#define rgbaMidnightBlue(ALPHA) QUOTERGBA(rgba(25,25,112,ALPHA))
#define rgbaMintCream(ALPHA) QUOTERGBA(rgba(245,255,250,ALPHA))
#define rgbaMistyRose(ALPHA) QUOTERGBA(rgba(255,228,225,ALPHA))
#define rgbaMoccasin(ALPHA) QUOTERGBA(rgba(255,228,181,ALPHA))
#define rgbaNavajoWhite(ALPHA) QUOTERGBA(rgba(255,222,173,ALPHA))
#define rgbaNavy(ALPHA) QUOTERGBA(rgba(0,0,128,ALPHA))
#define rgbaOldLace(ALPHA) QUOTERGBA(rgba(253,245,230,ALPHA))
#define rgbaOlive(ALPHA) QUOTERGBA(rgba(128,128,0,ALPHA))
#define rgbaOliveDrab(ALPHA) QUOTERGBA(rgba(107,142,35,ALPHA))
#define rgbaOrange(ALPHA) QUOTERGBA(rgba(255,165,0,ALPHA))
#define rgbaOrangeRed(ALPHA) QUOTERGBA(rgba(255,69,0,ALPHA))
#define rgbaOrchid(ALPHA) QUOTERGBA(rgba(218,112,214,ALPHA))
#define rgbaPaleGoldenRod(ALPHA) QUOTERGBA(rgba(238,232,170,ALPHA))
#define rgbaPaleGreen(ALPHA) QUOTERGBA(rgba(152,251,152,ALPHA))
#define rgbaPaleTurquoise(ALPHA) QUOTERGBA(rgba(175,238,238,ALPHA))
#define rgbaPaleVioletRed(ALPHA) QUOTERGBA(rgba(216,112,147,ALPHA))
#define rgbaPapayaWhip(ALPHA) QUOTERGBA(rgba(255,239,213,ALPHA))
#define rgbaPeachPuff(ALPHA) QUOTERGBA(rgba(255,218,185,ALPHA))
#define rgbaPeru(ALPHA) QUOTERGBA(rgba(205,133,63,ALPHA))
#define rgbaPink(ALPHA) QUOTERGBA(rgba(255,192,203,ALPHA))
#define rgbaPlum(ALPHA) QUOTERGBA(rgba(221,160,221,ALPHA))
#define rgbaPowderBlue(ALPHA) QUOTERGBA(rgba(176,224,230,ALPHA))
#define rgbaPurple(ALPHA) QUOTERGBA(rgba(128,0,128,ALPHA))
#define rgbaRed(ALPHA) QUOTERGBA(rgba(255,0,0,ALPHA))
#define rgbaRosyBrown(ALPHA) QUOTERGBA(rgba(188,143,143,ALPHA))
#define rgbaRoyalBlue(ALPHA) QUOTERGBA(rgba(65,105,225,ALPHA))
#define rgbaSaddleBrown(ALPHA) QUOTERGBA(rgba(139,69,19,ALPHA))
#define rgbaSalmon(ALPHA) QUOTERGBA(rgba(250,128,114,ALPHA))
#define rgbaSandyBrown(ALPHA) QUOTERGBA(rgba(244,164,96,ALPHA))
#define rgbaSeaGreen(ALPHA) QUOTERGBA(rgba(46,139,87,ALPHA))
#define rgbaSeaShell(ALPHA) QUOTERGBA(rgba(255,245,238,ALPHA))
#define rgbaSienna(ALPHA) QUOTERGBA(rgba(160,82,45,ALPHA))
#define rgbaSilver(ALPHA) QUOTERGBA(rgba(192,192,192,ALPHA))
#define rgbaSkyBlue(ALPHA) QUOTERGBA(rgba(135,206,235,ALPHA))
#define rgbaSlateBlue(ALPHA) QUOTERGBA(rgba(106,90,205,ALPHA))
#define rgbaSlateGray(ALPHA) QUOTERGBA(rgba(112,128,144,ALPHA))
#define rgbaSnow(ALPHA) QUOTERGBA(rgba(255,250,250,ALPHA))
#define rgbaSpringGreen(ALPHA) QUOTERGBA(rgba(0,255,127,ALPHA))
#define rgbaSteelBlue(ALPHA) QUOTERGBA(rgba(70,130,180,ALPHA))
#define rgbaTan(ALPHA) QUOTERGBA(rgba(210,180,140,ALPHA))
#define rgbaTeal(ALPHA) QUOTERGBA(rgba(0,128,128,ALPHA))
#define rgbaThistle(ALPHA) QUOTERGBA(rgba(216,191,216,ALPHA))
#define rgbaTomato(ALPHA) QUOTERGBA(rgba(255,99,71,ALPHA))
#define rgbaTransparent(ALPHA) QUOTERGBA(rgba(0,0,0,ALPHA))
#define rgbaTurquoise(ALPHA) QUOTERGBA(rgba(64,224,208,ALPHA))
#define rgbaViolet(ALPHA) QUOTERGBA(rgba(238,130,238,ALPHA))
#define rgbaVioletRed(ALPHA) QUOTERGBA(rgba(208,32,144,ALPHA))
#define rgbaWheat(ALPHA) QUOTERGBA(rgba(245,222,179,ALPHA))
#define rgbaWhite(ALPHA) QUOTERGBA(rgba(255,255,255,ALPHA))
#define rgbaWhiteSmoke(ALPHA) QUOTERGBA(rgba(245,245,245,ALPHA))
#define rgbaYellow(ALPHA) QUOTERGBA(rgba(255,255,0,ALPHA))
#define rgbaYellowGreen(ALPHA) QUOTERGBA(rgba(154,205,50,ALPHA))

enum e_HtmlColorIndex
{
	HtmlColorUnknown= -1,
    AliceBlue,    	// 0
    AntiqueWhite,   // 1
    Aqua,    		// 2
    Aquamarine,    	// 3
    Azure,    		// 4
    Beige,    		// 5
    Bisque,    		// 6
    Black,    		// 7
    BlanchedAlmond, // 8
    Blue,    		// 9
    BlueViolet,    	// 10
    Brown,    		// 11
    BurlyWood,    	// 12
    CadetBlue,    	// 13
    Chartreuse,    	// 14
    Chocolate,    	// 15
    Coral,    		// 16
    CornflowerBlue, // 17
    Cornsilk,    	// 18
    Crimson,    	// 19
    Cyan,    		// 20
    DarkBlue,    	// 21
    DarkCyan,    	// 22
    DarkGoldenRod,  // 23
    DarkGray,    	// 24
    DarkGreen,    	// 25
    DarkKhaki,    	// 26
    DarkMagenta,    // 27
    DarkOliveGreen, // 28
    Darkorange,    	// 29
    DarkOrchid,    	// 30
    DarkRed,    	// 31
    DarkSalmon,    	// 32
    DarkSeaGreen,   // 33
    DarkSlateBlue,  // 34
    DarkSlateGray,  // 35
    DarkTurquoise,  // 36
    DarkViolet,    	// 37
    DeepPink,    	// 38
    DeepSkyBlue,    // 39
    DimGray,    	// 40
    DodgerBlue,    	// 41
    Feldspar,    	// 42
    FireBrick,    	// 43
    FloralWhite,    // 44
    ForestGreen,    // 45
    Fuchsia,    	// 46
    Gainsboro,    	// 47
    GhostWhite,    	// 48
    Gold,    		// 49
    GoldenRod,    	// 40
    Gray,    		// 41
    Green,    		// 42
    GreenYellow,    // 43
    HoneyDew,    	// 44
    HotPink,    	// 45
    IndianRed,
    Indigo,
    Ivory,
    Khaki,
    Lavender,    	// 50
    LavenderBlush,
    LawnGreen,
    LemonChiffon,
    LightBlue,
    LightCoral,
    LightCyan,
    LightGoldenRodYeLlow,
    LightGrey,
    LightGreen,    		// 60
    LightPink,
    LightSalmon,
    LightSeaGreen,
    LightSkyBlue,
    LightSlateBlue,
    LightSlateGray,
    LightSteelBlue,
    LightYellow,
    Lime,
    LimeGreen,    		// 70
    Linen,
    Magenta,
    Maroon,
    MediumAquaMarine,
    MediumBlue,
    MediumOrchid,
    MediumPurple,
    MediumSeaGreen,
    MediumSlateBlue,
    MediumSpringGreen,   // 80
    MediumTurquoise,
    MediumVioletRed,
    MidnightBlue,
    MintCream,
    MistyRose,
    Moccasin,
    NavajoWhite,
    Navy,
    OldLace,
    Olive,    			// 90
    OliveDrab,
    Orange,
    OrangeRed,
    Orchid,
    PaleGoldenRod,
    PaleGreen,
    PaleTurquoise,
    PaleVioletRed,
    PapayaWhip,
    PeachPuff,    		// 100
    Peru,
    Pink,
    Plum,
    PowderBlue,
    Purple,
    Red,
    RosyBrown,
    RoyalBlue,
    SaddleBrown,
    Salmon,    			// 110
    SandyBrown,
    SeaGreen,
    SeaShell,
    Sienna,
    Silver,
    SkyBlue,
    SlateBlue,
    SlateGray,
    Snow,
    SpringGreen,    		// 120
    SteelBlue,
    Tan,
    Teal,
    Thistle,
    Tomato,
    Transparent,
    Turquoise,
    Violet,
    VioletRed,
    Wheat,    			// 130
    White,
    WhiteSmoke,
    Yellow,
    YellowGreen,
};
typedef enum e_HtmlColorIndex            HtmlColorIndex;

HTMLColor wcImageGraphNameToColor(HtmlColorIndex ColorName);



#if (WEBC_MEMORY_RTPDEBUG)

typedef enum e_MemDbReportSortType {
	SortNone,		// Do not sort the records, best method for DbSummarizeNone report type.
	SortBySize,		// Sort the descending order by size.
	SortByFile,		// Sort in ascending order by file name followed by line if DbSummarizeByLine is specified.
} MemDbReportSortType;

typedef enum e_MemDbReportSummaryType {
	DbSummarizeByFile, // Return a list of records containing file name and total memory allocations outstanding for the file.
	DbSummarizeByLine, // Return a list of records containing file name:line number and total memory allocations outstanding at that location the file.
	DbSummarizeNone,   // Return a record of every outstanding allocation in the range.
} MemDbReportSummaryType;

void wcMemDbGetStats(MemDbStats *pStats);
void wcMemDbInstallPeakUsageCallback(void (*fn)(const char *file, long line_num,void *ctx),void *ctx, unsigned long threshold);
RTPMemBlockHeader *wcMemDbGetReport(long startSequence, long  endSequence, MemDbReportSummaryType ReportType, MemDbReportSortType SortBy);
void wcMemDbReleaseReport(RTPMemBlockHeader *pReport);

RTPMemBlockHeader *wcMemDbGetWatchPoint(void);
RTPMemBlockHeader *wcMemDbGetChangesSinceWatchPoint(RTPMemBlockHeader *DbWatchPoint);

void wcPrintMemStats(void);
void wcPrintMemChangeReport(RTPMemBlockHeader *PrevWatchPoint, wcBOOL ExcludeNetZeroResults);
void wcPrintMemDetailReport(long  startSequence, long  endSequence, MemDbReportSummaryType ReportType, MemDbReportSortType SortBy);

#endif

#if (WEBC_SUPPORT_JSCRIPT)
HSCRIPT_HANDLE wcJSExecuteSource(wcCtx* Ctx, const char *_source);
void wcJSDestroyScript(HSCRIPT_HANDLE  hscript);
void wcJSDefineFunction(wcCtx* Ctx, const char *name, JSNative func, int argc);
HDOC_HANDLE webc_JScriptGetDoc(struct JSContext *cx);
wcBOOL wcJSInitializeDocCtxt(struct JSContext *cx, wcCtx *Ctx);
#endif

#endif // _WCAPI_INCLUDED__



