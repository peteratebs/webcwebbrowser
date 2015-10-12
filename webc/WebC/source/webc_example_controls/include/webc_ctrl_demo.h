#include "webc.h"
#include "webcmem.h"
#include "htmlsdk.h"
#include "rtpprint.h"

/* Set this to 1 to log events that are sent to controls
   When LOG_DEMO_CONTROL_EVENTS is enabled the demo application callback
   routines call webc_demo_event_log_event(). This routine accumulates
   a list of all events that are sent to the callback function and uses
   printf to print out a set of cases for a switch statement for every
   event that the control recieves.

   This basic version merges all messages it recieves from all callback handlers
   so the output is only reliable when an experiment is performed on a single
   control.

   To use this feature printf must be available or an alternative must be provided
   The source code is in webc_ctrl_demo_main.cpp.
*/

#define LOG_DEMO_CONTROL_EVENTS 0

void webc_demo_event_log_clear(void);
void webc_demo_event_log_event(HTMLEventType the_type);

#define webc_control_demo_show_info()


typedef struct webc_control_demo_globals
{
    HBROWSER_HANDLE hBrowser;
    HDOC_HANDLE hRootDocument;
    HELEMENT_HANDLE hOuterFrame;
    HDOC_HANDLE hOuterDoc;
    HELEMENT_HANDLE hInfoFrame;
    HDOC_HANDLE hInfoDoc;
    HELEMENT_HANDLE hDemoFrame;
    HDOC_HANDLE hDemoDoc;
    HELEMENT_HANDLE hViewerFrame;
    HDOC_HANDLE hViewerDoc;
    int  viewer_is_visible;
    long demo_topY;
    long demo_leftX;
    long demo_heightY;
    long demo_widthX;
    /* Info field prompts */
    HELEMENT_HANDLE hInfoDemoName;
    HELEMENT_HANDLE hInfoType;
    HELEMENT_HANDLE hInfoId;
    HELEMENT_HANDLE hInfoName;
    HELEMENT_HANDLE hInfoAction;
    HELEMENT_HANDLE hInfoEventName;
    HELEMENT_HANDLE hInfoEventValue;
    HELEMENT_HANDLE hBasicControlSelector;  /* SELECT control for choosing tests */
    HELEMENT_HANDLE hCustomControlSelector;  /* SELECT control for choosing tests */
} WEBC_CONTROL_DEMO_GLOBALS;

extern WEBC_CONTROL_DEMO_GLOBALS wcdContext;

typedef struct webc_control_demo_handle {
    HTMLElementType element_type;
    char *element_id;
    HELEMENT_HANDLE element_handle;
    HTMLEventCallback cb;
} WEBC_CONTROL_DEMO_HANDLE;

char *ElementTypeToString(HTMLElementType theType);
char * GetEventName(HTMLEvent * TheEvent);
char * GetEventTypeName(HTMLEventType the_type);

int webc_control_demo_load_string(HDOC_HANDLE hDoc, char *html_src);
int webc_control_demo_load_array(HDOC_HANDLE hDoc, char **html_src);
void webc_control_demo_init_control_array(WEBC_CONTROL_DEMO_HANDLE *phandle, int max_handles);
int webc_control_demo_extract_controls(HDOC_HANDLE hDoc, WEBC_CONTROL_DEMO_HANDLE *phandle, int max_handles);
void webc_control_demo_set_tag_handlers(HDOC_HANDLE hDoc, WEBC_CONTROL_DEMO_HANDLE *phandle, int max_handles);

void webc_control_demo_update_info_element(HELEMENT_HANDLE theElement);
void webc_control_demo_update_info_action(char *theText);
void webc_control_demo_update_info_demoname(char *theText);
void webc_control_demo_update_info_event(HTMLEvent* event);
void webc_control_demo_animate_fullscreen(char which_direction);
void webc_control_demo_draw_viewframe(long Xpos, long Ypos);
void webc_control_demo_hide_viewframe(void);


void webc_control_demo_int_toascii(char *toascii, int fromint);
int webc_control_demo_ascii_to_int(char *fromascii);

#define START_DEMO    1
#define RESTART_DEMO  2
#define STOP_DEMO     3
#define SHOW_INFO     4
