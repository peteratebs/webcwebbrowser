//
// WCCONST.CPP - Global data - platform independent string constants
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

#include "wcconst.hpp"
#include "wgtypes.hpp"

/*****************************************************************************/
// Global Data
/*****************************************************************************/

const WebChar WEBC_STR_NULL[]           = {'\0'}; // null
const WebChar WEBC_STR_STATUS[]         = {'S','t','a','t','u','s','\0'}; // "Status"
const WebChar WEBC_STR_NOURL[]          = {'[','N','o',' ','U','R','L',']','\0'}; // "[No URL]"							   
const WebChar WEBC_STR_ABOUT[]          = {'A','b','o','u','t','\0'}; // "About"
const WebChar WEBC_STR_PABOUT[]         = {'E','B','S','N','e','t',' ','M','i','c','r','o','W',
									     'e','b',' ','v','e','r','s','i','o','n',' ','\0'};  // "EBSNet MicroWeb version "
const WebChar WEBC_STR_X[]              = {'X','\0'}; // "X"
const WebChar WEBC_STR_HOME[]           = {'H','o','m','e','\0'}; // "Home"
const WebChar WEBC_STR_BACK[]           = {'B','a','c','k','\0'}; // "Back"
const WebChar WEBC_STR_FORWARD[]        = {'F','o','r','w','a','r','d','\0'}; // "Forward"
const WebChar WEBC_STR_REFRESH[]        = {'R','e','f','r','e','s','h','\0'}; // "Refresh"
const WebChar WEBC_STR_VIEWSOURCE[]     = {'V','i','e','w',' ','S','o','u','r','c','e','\0'}; // "View Source" 
const WebChar WEBC_STR_ZOOMIN[]         = {'Z','o','o','m',' ','I','n','\0'}; // "Zoom In"
const WebChar WEBC_STR_MEMORY[]         = {'M','e','m','o','r','y','\0'}; // "Memory"
const WebChar WEBC_STR_SAVE[]           = {'S','a','v','e','\0'}; // "Save"
const WebChar WEBC_STR_FORMAT[]         = {'F','o','r','m','a','t','\0'}; // "Format"
const WebChar WEBC_STR_DEBUG[]          = {'D','e','b','u','g','\0'}; // "Debug"
const WebChar WEBC_STR_EXIT[]           = {'E','x','i','t','\0'}; // "Exit"
const WebChar WEBC_STR_ERROR[]          = {'E','r','r','o','r','!','\0'};	// "Error!"
const WebChar WEBC_STR_ANIMATION[]      = {'-','\\','|','/','\0'}; // "-\\|/"
const WebChar WEBC_STR_ASTERISK[]       = {'*','\0'}; // "*"
const WebChar WEBC_STR_E[]              = {'E','\0'}; //"E"
const WebChar WEBC_STR_A[]              = {'A','\0'}; // "A"
const WebChar WEBC_STR_RESET[]          = {'R','e','s','e','t','\0'}; // "Reset"
const WebChar WEBC_STR_OK[]             = {'O','K','\0'}; // "OK"
const WebChar WEBC_STR_TNR[]            = {'\t','-','\n','\r',' ','\0'}; // "\t-\n\r "
const WebChar WEBC_STR_TABDASH[]        = {'\t','-',' ','\0'}; // "\t- "
const WebChar WEBC_STR_DOT[]            = {'.', '\0'};// ". "

const WEBC_CHAR WEBC_STR_ARIAL[]         = {'A','r','i','a','l',0}; // "Arial"
const WEBC_CHAR WEBC_STR_ARIALUNICODE[]  = {'A','r','i','a','l','U','n','i','c','o','d','e',0}; // "ArialUnicode"
const WEBC_CHAR WEBC_STR_AT[]            = {'@',0}; // "@"
const WEBC_CHAR WEBC_STR_BACKBUTTON[]    = {'b','a','c','k','B','u','t','t','o','n',0}; // "backButton"
const WEBC_CHAR WEBC_STR_CLOSEWINDOW[]   = {'c','l','o','s','e','W','i','n','d','o','w',0}; // "closeWindow"
const WEBC_CHAR WEBC_STR_COLON[]         = {':',0}; // ":"
const WEBC_CHAR WEBC_STR_COLON_SLASH_SLASH[] = {':','/','/',0}; // "://"
const WEBC_CHAR WEBC_STR_COURIER[]       = {'C','o','u','r','i','e','r',0}; // "Courier"
const WEBC_CHAR WEBC_STR_DOCUMENTICON[]  = {'d','o','c','u','m','e','n','t','I','c','o','n',0}; // "documentIcon"
const WEBC_CHAR WEBC_STR_DONE[]          = {'D','o','n','e','.',0}; // "Done."
const WEBC_CHAR WEBC_STR_FORWARDBUTTON[] = {'f','o','r','w','a','r','d','B','u','t','t','o','n',0}; // "forwardButton"
const WEBC_CHAR WEBC_STR_GET[]           = {'G','E','T',0}; // "GET"
const WEBC_CHAR WEBC_STR_GOOGLESEARCH[]  = {'g','o','o','g','l','e','S','e','a','r','c','h',0}; // "googleSearch"
const WEBC_CHAR WEBC_STR_HOMEBUTTON[]    = {'h','o','m','e','B','u','t','t','o','n',0}; // "homeButton"
const WEBC_CHAR WEBC_STR_ON[]            = {'o','n',0};                        // "ON"
const WEBC_CHAR WEBC_STR_LF[]            = {'\n',0}; // "\n"
const WEBC_CHAR WEBC_STR_LOADING[]       = {'L','o','a','d','i','n','g','.','.','.',0}; // "Loading..."
const WEBC_CHAR WEBC_STR_LOADMOVIE[]     = {'l','o','a','d','M','o','v','i','e',}; // "loadMovie"
const WEBC_CHAR WEBC_STR_MONOSPACE[]     = {'m','o','n','o','s','p','a','c','e',0}; // "monospace"
const WEBC_CHAR WEBC_STR_NORMAL[]        = {'n','o','r','m','a','l',0}; // "normal"
const WEBC_CHAR WEBC_STR_PASS[]          = {'p','a','s','s',0}; // "pass"
const WEBC_CHAR WEBC_STR_POST[]          = {'P','O','S','T',0}; // "POST"
const WEBC_CHAR WEBC_STR_READY[]         = {'R','e','a','d','y','.',0}; // "Ready."
const WEBC_CHAR WEBC_STR_REALM[]         = {'r','e','a','l','m',0}; // "realm"
const WEBC_CHAR WEBC_STR_REFRESHBUTTON[] = {'r','e','f','r','e','s','h','B','u','t','t','o','n',0}; // "refreshButton"
const WEBC_CHAR WEBC_STR_SANS_SERIF[]    = {'s','a','n','s','-','s','e','r','i','f',0}; // "sans-serif"
const WEBC_CHAR WEBC_STR_SEARCHBUTTON[]  = {'s','e','a','r','c','h','B','u','t','t','o','n',0}; // "searchButton"
const WEBC_CHAR WEBC_STR_SEARCHFIELD[]   = {'s','e','a','r','c','h','F','i','e','l','d',0}; // "searchField"
const WEBC_CHAR WEBC_STR_SERIF[]         = {'S','e','r','i','f',0}; // "Serif"
const WEBC_CHAR WEBC_STR_SETAUTH[]       = {'S','e','t','A','u','t','h',0}; // "SetAuth"
const WEBC_CHAR WEBC_STR_SHARP[]         = {'#',0}; // "#"
const WEBC_CHAR WEBC_STR_STATUSICON[]    = {'s','t','a','t','u','s','I','c','o','n',0}; // "statusIcon"
const WEBC_CHAR WEBC_STR_STOP[]          = {'S','t','o','p',0}; // "Stop"
const WEBC_CHAR WEBC_STR_STOPBUTTON[]    = {'s','t','o','p','B','u','t','t','o','n',0}; // "stopButton"
const WEBC_CHAR WEBC_STR_STOPPED[]       = {'S','t','o','p','p','e','d','.',0}; // "Stopped."
const WEBC_CHAR WEBC_STR_SUBMIT[]        = {'S','u','b','m','i','t',0}; // "Submit"
const WEBC_CHAR WEBC_STR_TIMES[]         = {'T','i','m','e','s',0}; // "Times"
const WEBC_CHAR WEBC_STR_URLBAR[]        = {'u','r','l','B','a','r',0}; // "urlBar"
const WEBC_CHAR WEBC_STR_URLFIELD[]      = {'u','r','l','F','i','e','l','d',0}; // "urlField"
const WEBC_CHAR WEBC_STR_URL[]           = {'u','r','l',0}; // "url"
const WEBC_CHAR WEBC_STR_USER[]          = {'u','s','e','r',0}; // "user"
const WEBC_CHAR WEBC_STR_VERDANA[]       = {'V','e','r','d','a','n','a',0}; // "Verdana"
const WEBC_CHAR WEBC_STR_TAHOMA[]       = {'T','a','h','o','m','a',0}; // "Tahoma"
