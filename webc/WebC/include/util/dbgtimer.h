#ifndef __DBGTIMER_H__
#define __DBGTIMER_H__

#include "webc.h"


/* ID's for use with the functions below */

#define WEBC_DEBUG_MAIN                                   0   
#define WEBC_DEBUG_DISPLAYMANAGER_REFRESH                 1   
#define WEBC_DEBUG_WEBGRAPHICS_BITMAP                     2
#define WEBC_DEBUG_WEBGRAPHICS_COLORTORGB                 3
#define WEBC_DEBUG_WEBGRAPHICS_CREATESTRETCHEDBITMAP      4
#define WEBC_DEBUG_WEBGRAPHICS_DARKCOLOR                  5
#define WEBC_DEBUG_WEBGRAPHICS_DESTROYSTRETCHEDBITMAP     6
#define WEBC_DEBUG_WEBGRAPHICS_ENDBUFFER                  7
#define WEBC_DEBUG_WEBGRAPHICS_FONTHEIGHT                 8
#define WEBC_DEBUG_WEBGRAPHICS_FRAME3D                    9
#define WEBC_DEBUG_WEBGRAPHICS_GETCHAROFFSET              10 
#define WEBC_DEBUG_WEBGRAPHICS_GETCLIP                    11
#define WEBC_DEBUG_WEBGRAPHICS_GETRECT                    12
#define WEBC_DEBUG_WEBGRAPHICS_HLINE                      13
#define WEBC_DEBUG_WEBGRAPHICS_INVALIDATE                 14
#define WEBC_DEBUG_WEBGRAPHICS_LIGHTCOLOR                 15
#define WEBC_DEBUG_WEBGRAPHICS_RECTANGLE                  16
#define WEBC_DEBUG_WEBGRAPHICS_RGBTOCOLOR                 17
#define WEBC_DEBUG_WEBGRAPHICS_RGBTODARKCOLOR             18
#define WEBC_DEBUG_WEBGRAPHICS_RGBTOLIGHTCOLOR            19
#define WEBC_DEBUG_WEBGRAPHICS_SETCLIP                    20
#define WEBC_DEBUG_WEBGRAPHICS_STARTBUFFER                21
#define WEBC_DEBUG_WEBGRAPHICS_STRETCHBITMAP              22
#define WEBC_DEBUG_WEBGRAPHICS_TEXT                       23
#define WEBC_DEBUG_WEBGRAPHICS_TEXTHEIGHT                 24
#define WEBC_DEBUG_WEBGRAPHICS_TEXTWIDTH                  25
#define WEBC_DEBUG_WEBGRAPHICS_TILEBITMAP                 26
#define WEBC_DEBUG_WEBGRAPHICS_VLINE                      27
#define WEBC_DEBUG_HTMLFLOWDISPLAY_DRAWTHISONLY           28
#define WEBC_DEBUG_HTMLELEMENTDISPLAY_DRAWTHISONLY        29
#define WEBC_DEBUG_HTMLINPUTBUTTONDISPLAY_DRAWTHISONLY    30
#define WEBC_DEBUG_HTMLTABLEDISPLAY_DRAWTHISONLY          31
#define WEBC_DEBUG_HTMLSTRINGDISPLAY_DRAW                 32
#define WEBC_DEBUG_HTMLELEMENT_GETSTYLEFROMCSS            33
#define WEBC_DEBUG_HTMLELEMENT_GETFONT                    34
#define WEBC_DEBUG_HTMLELEMENT_GETCOLOR                   35
#define WEBC_DEBUG_HTMLELEMENT_GETVISIBILITY              36
#define WEBC_DEBUG_HTMLELEMENT_GETTEXTDECORATION          37
#define WEBC_DEBUG_HTMLELEMENT_GETSTYLEFROMPARENT         38
#define WEBC_DEBUG_HTMLFLOWDISPLAY_DRAWBACKGROUND         39
#define WEBC_DEBUG_HTMLFLOWDISPLAY_DRAWBORDER             40
#define WEBC_DEBUG_HTMLCONTENTBOX_DRAWBORDER              41
#define WEBC_DEBUG_HTMLCONTENTBOX_GETBORDERCOLOR          42
#define WEBC_DEBUG_HTMLCONTENTBOX_GETBORDERSTYLE          43
#define WEBC_DEBUG_HTMLCONTENTBOX_GETMARGINWIDTH          44
#define WEBC_DEBUG_HTMLCONTENTBOX_GETPADDINGWIDTH         45
#define WEBC_DEBUG_HTMLCONTENTBOX_GETBORDERWIDTH          46
#define WEBC_DEBUG_TIMER_CALIBRATE                        47
#define WEBC_DEBUG_HTMLELEMENT_GETBACKGROUNDCOLOR         48
#define WEBC_DEBUG_HTMLFLOWDISPLAY_FORMAT                 49
#define WEBC_DEBUG_HTMLFLOWDISPLAY_ADDUNITTOLINE          50
#define WEBC_DEBUG_HTMLTABLEDISPLAY_FORMAT                51
#define WEBC_DEBUG_DISPLAYTUENUMERATOR_GETNEXTUNIT        52
#define WEBC_DEBUG_HTMLELEMENT_GETLEFT                    53
#define WEBC_DEBUG_HTMLELEMENT_GETTOP                     54
#define WEBC_DEBUG_HTMLELEMENT_GETWIDTH                   55
#define WEBC_DEBUG_HTMLELEMENT_GETHEIGHT                  56
#define WEBC_DEBUG_HTMLELEMENT_GETCHILDWRAP               57
#define WEBC_DEBUG_HTMLELEMENT_GETUNITTYPE                58
#define WEBC_DEBUG_HTMLELEMENT_GETCLEAR                   59
#define WEBC_DEBUG_HTMLELEMENT_GETFLOAT                   60
#define WEBC_DEBUG_HTMLELEMENT_GETPOSITION                61
#define WEBC_DEBUG_HTMLELEMENT_GETCHILDTEXTALIGN          62
#define WEBC_DEBUG_HTMLELEMENT_GETVALIGN                  63
#define WEBC_DEBUG_HTMLELEMENT_GETZINDEX                  64
#define WEBC_DEBUG_HTMLSTRINGDISPLAY_SETELEMENTSTYLE      65


/* Functions */

#if (WEBC_SUPPORT_DEBUG_TIME)

#define WEBC_DEBUG_TIMER_START(X)     webc_DebugTimerStart(X)
#define WEBC_DEBUG_TIMER_STOP(X)      webc_DebugTimerStop(X)

#ifdef __cplusplus
extern "C" {
#endif

void webc_DebugTimerStart  (long id);
void webc_DebugTimerStop   (long id);
int  webc_DebugTimerReport (char *fileName);
void webc_DebugTimerReset  (void);

#ifdef __cplusplus
};
#endif

#else

#define WEBC_DEBUG_TIMER_START(X)
#define WEBC_DEBUG_TIMER_STOP(X)

#endif

#endif // __DBGTIMER_H__
