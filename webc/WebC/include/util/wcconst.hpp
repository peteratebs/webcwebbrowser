#ifndef __WCCONST_H_INCLUDED__
#define __WCCONST_H_INCLUDED__

#include "webctypes.h"

// wcconst.h
// contains externs for both unicode and ASCII versions of string constants

#ifdef __cplusplus
extern "C" {
#endif

extern const WEBC_CHAR WEBC_STR__BLANK[];                    // "_blank"
extern const WEBC_CHAR WEBC_STR__PARENT[];                   // "_parent"
extern const WEBC_CHAR WEBC_STR__SELF[];                     // "_self"
extern const WEBC_CHAR WEBC_STR__TOP[];                      // "_top"
extern const WEBC_CHAR WEBC_STR_A[];                         // "A"
extern const WEBC_CHAR WEBC_STR_ABOUT[];                     // "About"
extern const WEBC_CHAR WEBC_STR_ANIMATION[];                 // "-\\|/"
extern const WEBC_CHAR WEBC_STR_ARIAL[];                     // "Arial"
extern const WEBC_CHAR WEBC_STR_ARIALUNICODE[];              // "ArialUnicode"
extern const WEBC_CHAR WEBC_STR_ASTERISK[];                  // "*"
extern const WEBC_CHAR WEBC_STR_AT[];                        // "@"
extern const WEBC_CHAR WEBC_STR_BACK[];                      // "Back"
extern const WEBC_CHAR WEBC_STR_BACKBUTTON[];                // "backButton"
extern const WEBC_CHAR WEBC_STR_CIRCLE[];                    // "circle"
extern const WEBC_CHAR WEBC_STR_CLOSE_BRACE[];               // "}"
extern const WEBC_CHAR WEBC_STR_CLOSE_CSS_COMMENT[];         // "*/"
extern const WEBC_CHAR WEBC_STR_CLOSE_HTML_COMMENT[];        // "-->"
extern const WEBC_CHAR WEBC_STR_CLOSEWINDOW[];               // "closeWindow"
extern const WEBC_CHAR WEBC_STR_COLON[];                     // ":"
extern const WEBC_CHAR WEBC_STR_COLON_SLASH_SLASH[];         // "://"
extern const WEBC_CHAR WEBC_STR_COMMA[];                     // ","
extern const WEBC_CHAR WEBC_STR_COURIER[];                   // "Courier"
extern const WEBC_CHAR WEBC_STR_DEBUG[];                     // "Debug"
extern const WEBC_CHAR WEBC_STR_DOCUMENTICON[];              // "documentIcon"
extern const WEBC_CHAR WEBC_STR_DOMAIN[];                    // "domain"
extern const WEBC_CHAR WEBC_STR_DONE[];                      // "Done."
extern const WEBC_CHAR WEBC_STR_DOT[];                       // "."
extern const WEBC_CHAR WEBC_STR_E[];                         // "E"
extern const WEBC_CHAR WEBC_STR_EQUALS[];                    // "="
extern const WEBC_CHAR WEBC_STR_ERROR[];                     // "Error!"
extern const WEBC_CHAR WEBC_STR_EXIT[];                      // "Exit"
extern const WEBC_CHAR WEBC_STR_FORMAT[];                    // "Format"
extern const WEBC_CHAR WEBC_STR_FORWARD[];                   // "Forward"
extern const WEBC_CHAR WEBC_STR_FORWARDBUTTON[];             // "forwardButton"
extern const WEBC_CHAR WEBC_STR_GET[];                       // "GET"
extern const WEBC_CHAR WEBC_STR_GOOGLESEARCH[];              // "googleSearch"
extern const WEBC_CHAR WEBC_STR_HIDDEN[];                    // "hidden"
extern const WEBC_CHAR WEBC_STR_HOME[];                      // "Home"
extern const WEBC_CHAR WEBC_STR_HOMEBUTTON[];                // "homeButton"
extern const WEBC_CHAR WEBC_STR_LF[];                        // "\n"
extern const WEBC_CHAR WEBC_STR_LOADING[];                   // "Loading..."
extern const WEBC_CHAR WEBC_STR_LOADMOVIE[];                 // "loadMovie"
extern const WEBC_CHAR WEBC_STR_MEMORY[];                    // "Memory"
extern const WEBC_CHAR WEBC_STR_MONOSPACE[];                 // "monospace"
extern const WEBC_CHAR WEBC_STR_NO[];                        // "no"
extern const WEBC_CHAR WEBC_STR_NORMAL[];                    // "normal"
extern const WEBC_CHAR WEBC_STR_NULL[];                      // ""
extern const WEBC_CHAR WEBC_STR_NOURL[];                     // "[NO URL]"
extern const WEBC_CHAR WEBC_STR_OK[];                        // "OK"
extern const WEBC_CHAR WEBC_STR_ON[];                        // "on"
extern const WEBC_CHAR WEBC_STR_OPEN_BRACE[];                // "{"
extern const WEBC_CHAR WEBC_STR_OPEN_CSS_COMMENT[];          // "/*"
extern const WEBC_CHAR WEBC_STR_OPEN_HTML_COMMENT[];         // "<!--"
extern const WEBC_CHAR WEBC_STR_PABOUT[];                    // "EBSNet MicroWeb version "
extern const WEBC_CHAR WEBC_STR_PASS[];                      // "pass"
extern const WEBC_CHAR WEBC_STR_PATH[];                      // "path"
extern const WEBC_CHAR WEBC_STR_POLY[];                      // "poly"
extern const WEBC_CHAR WEBC_STR_POST[];                      // "POST"
extern const WEBC_CHAR WEBC_STR_READY[];                     // "Ready."
extern const WEBC_CHAR WEBC_STR_REALM[];                     // "realm"
extern const WEBC_CHAR WEBC_STR_REFRESH[];                   // "Refresh"
extern const WEBC_CHAR WEBC_STR_REFRESHBUTTON[];             // "refreshButton"
extern const WEBC_CHAR WEBC_STR_RESET[];                     // "Reset"
extern const WEBC_CHAR WEBC_STR_SANS_SERIF[];                // "sans-serif"
extern const WEBC_CHAR WEBC_STR_SAVE[];                      // "Save"
extern const WEBC_CHAR WEBC_STR_SEARCHBUTTON[];              // "searchButton"
extern const WEBC_CHAR WEBC_STR_SEARCHFIELD[];               // "searchField"
extern const WEBC_CHAR WEBC_STR_SEMICOLON[];                 // ";"
extern const WEBC_CHAR WEBC_STR_SEMI_SPACE[];                // "; "
extern const WEBC_CHAR WEBC_STR_SERIF[];                     // "Serif"
extern const WEBC_CHAR WEBC_STR_SETAUTH[];                   // "SetAuth"
extern const WEBC_CHAR WEBC_STR_SHARP[];                     // "#"
extern const WEBC_CHAR WEBC_STR_STATUSICON[];                // "statusIcon"
extern const WEBC_CHAR WEBC_STR_STATUS[];                    // "Status"
extern const WEBC_CHAR WEBC_STR_STOP[];                      // "Stop"
extern const WEBC_CHAR WEBC_STR_STOPBUTTON[];                // "stopButton"
extern const WEBC_CHAR WEBC_STR_STOPPED[];                   // "Stopped."
extern const WEBC_CHAR WEBC_STR_SUBMIT[];                    // "Submit"
extern const WEBC_CHAR WEBC_STR_TABDASH[];                   // "\t- "
extern const WEBC_CHAR WEBC_STR_TAHOMA[];                    // "Tahoma"
extern const WEBC_CHAR WEBC_STR_TIMES[];                     // "Times"
extern const WEBC_CHAR WEBC_STR_TIMES_COMMA_SERIF[];         // "Times, serif"
extern const WEBC_CHAR WEBC_STR_TNR[];                       // "\t-\n\r "
extern const WEBC_CHAR WEBC_STR_URL[];                       // "url"
extern const WEBC_CHAR WEBC_STR_URLBAR[];                    // "urlBar"
extern const WEBC_CHAR WEBC_STR_URLFIELD[];                  // "urlField"
extern const WEBC_CHAR WEBC_STR_USER[];                      // "user"
extern const WEBC_CHAR WEBC_STR_VERDANA[];                   // "Verdana" 
extern const WEBC_CHAR WEBC_STR_VIEWSOURCE[];                // "View Source" 
extern const WEBC_CHAR WEBC_STR_X[];                         // "X"
extern const WEBC_CHAR WEBC_STR_YES[];                       // "yes"
extern const WEBC_CHAR WEBC_STR_ZOOMIN[];                    // "Zoom In"

#define WEBC_CHAR_NULL                                      '\0'
#define WEBC_CHAR_CR                                        '\r'
#define WEBC_CHAR_TAB                                       '\t'
#define WEBC_CHAR_LF                                        '\n'
#define WEBC_CHAR_SPACE                                     ' '
#define WEBC_CHAR_DASH                                      '-'

#ifdef __cplusplus
}
#endif

#endif //__WCCONST_H_INCLUDED__
