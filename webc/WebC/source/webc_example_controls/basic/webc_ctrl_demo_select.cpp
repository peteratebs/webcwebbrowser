#include "webc_ctrl_demo.h"
/*
webc_ctrl_demo_select.cpp

    void webc_control_demo_select(void)
        Demonstrate using the webc API to access and control HTML elements of type: <input type=select>

        The demo uses webc API functions to access and control these elements using
        handles of type HTML_SELECT_ELEMENT.


        HTML_SELECT_ELEMENT controls can be manipulated by the webC API and Events to them can be monitored
        and intercepted by installing application specific event handler for individual elements.


    This demo:
       . loads html text edit demonstration source code from a character array into the demo frame
            . Extracts HTML Element handles from the document
            . Assigns an event handler to that demonstrates application subclassing of control functions.

*/

/* Generic Demo setup step 1: Define several integer handles we will use to access controls */
#define CONTROL_SELECT 0
#define CONTROL_OPTION 1
#define DEMO_CONTROL_COUNT    2
#define DEMOING_WHICH_HTML_ELEMENT HTML_SELECT_ELEMENT

/* Generic Demo setup step 2: Declare an array to hold the handles */
static WEBC_CONTROL_DEMO_HANDLE demo_control_handles[DEMO_CONTROL_COUNT];

/* Generic Demo setup step 3: Declare individual callback functions we will use to over-ride some default
   characteristics of individual controls */
static HTMLEventStatus cb_CONTROL_SELECT (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);

/* Generic Demo setup step 4: Initialize our control handle array html with values to match
  against the "id=" properties in the html and the control type
  Documentation moment: List the types
*/
static void init_demo_control_handles(void)
{
    webc_control_demo_init_control_array(&demo_control_handles[0],DEMO_CONTROL_COUNT);
    demo_control_handles[CONTROL_SELECT].element_type = DEMOING_WHICH_HTML_ELEMENT;
    demo_control_handles[CONTROL_SELECT].element_id = "CONTROL_SELECT";
    demo_control_handles[CONTROL_SELECT].element_handle = 0;
    demo_control_handles[CONTROL_SELECT].cb = cb_CONTROL_SELECT;

    demo_control_handles[CONTROL_OPTION].element_type = HTML_OPTION_ELEMENT;
    demo_control_handles[CONTROL_OPTION].element_id = "CONTROL_OPTION_ONE";
    demo_control_handles[CONTROL_OPTION].element_handle = 0;
    demo_control_handles[CONTROL_OPTION].cb = 0;

}

/* Generic Demo setup step 5: Provide an array of html source code that
   will be loaded into the document view of the demo browser when the demo
   is started. */
#if (0)
static char * demo_control_html = {
"<html>\n\
  <body>\n\
  <select style=\"POSITION:absolute;left:100px;top:50px\">\n\
   <option value=CONTROL_OPTION_ZERO>POSITION\n\
   <option value=CONTROL_OPTION_ONE>ID:OPTION_ONE\n\
   <option value=CONTROL_OPTION_TWO>ID:OPTION_TWO\n\
   <option value=CONTROL_OPTION_THREE>ID:OPTION_THREE\n\
  </select>\n\
  (id:CONTROL_SELECT)\n\
  <select id=CONTROL_SELECT>\n\
   <option id=CONTROL_OPTION_ZERO value=CONTROL_OPTION_ZERO>ID:OPTION_ZERO\n\
   <option id=CONTROL_OPTION_ONE value=CONTROL_OPTION_ONE>ID:OPTION_ONE\n\
   <option id=CONTROL_OPTION_TWO value=CONTROL_OPTION_TWO>ID:OPTION_TWO\n\
   <option id=CONTROL_OPTION_THREE value=CONTROL_OPTION_THREE>ID:OPTION_THREE\n\
  </select>\n\
  <select style=\"POSITION:absolute;left:300px;top:150px\">\n\
   <option value=CONTROL_OPTION_ZERO>POSITION300X150\n\
   <option value=CONTROL_OPTION_ONE>ID:OPTION_ONE\n\
   <option value=CONTROL_OPTION_TWO>ID:OPTION_TWO\n\
   <option value=CONTROL_OPTION_THREE>ID:OPTION_THREE\n\
  </select>\n\
  </body>\n\
</html>\n"};
#endif

/* Test pages */
#define TEST_SOURCE_FIXED_WIDTH 0
#define TEST_SOURCE_IMPLICIT_WIDTH 0
#define TEST_SOURCE_BOTH 1

/*
MARGIN_INTRINSIC MARGIN_FIXED PADDING_ADD_MARGIN_HTML  RESULT Fixed      RESULT Implicit
               0            0          0           0   Frame 2 big by 2  Frame 2 small by 1
               0            0          0           1   Frame 2 big by 2  Frame 2 small by 1
               0            0          1           0   Frame 2 big by 4  Frame 2 small by 1 (maybe 2)
               0            0          1           1   Frame 2 big by 4  Frame 2 small by 1
               0            1          0           0   Frame good        Frame 2 small by 1
               0            1          0           1   Frame good        Frame 2 small by 1
               0            1          1           0   Frame 2 big by 2  Frame 2 small by 1
               0            1          1           1   Frame 2 big by 2  Frame 2 small by 1
               1            0          0           0   Frame 2 big by 2  Frame Good
               1            0          0           1   Frame 2 big by 2  Frame Good
               1            0          1           0   Frame 2 big by 4  Frame Good
               1            0          1           1   Frame 2 big by 4  Frame Good
               1            1          0           0   Frame Good        Frame Good
               1            1          0           1   Frame Good        Frame Good    ** Plus positioning is good
               1            1          1           0   Frame 2 big by 2  Frame Good
               1            1          1           1   Frame 2 big by 2  Frame Good
Experiment 1.. Add padding intrinsic dhselect.cpp line 140
               1            1          1           1   Frame 2 big by 2  Frame 2 big by 1
Experiment 1.. Also Add padding fixed dhselect.cpp line 398
               1            1          1           1   Frame 2 big by 1  Frame 2 big by 1
*/


#if (TEST_SOURCE_BOTH)
static char * demo_control_html = {
"<html>\n\
  <body>\n\
   <select name=\"cs\" style=\"POSITION:relative;left:10px; top:10px; font-size:30pt;width:300\">\n\
   <option value=1>FIX%\n\
   <option value=1>XXXXXX\n\
   <option value=2>50%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
   <option value=3>25%\n\
  </select>\n\
   <select name=\"cs\" style=\"POSITION:relative;left:20px; top:10px; font-size:30pt\">\n\
   <option value=1>IMP%\n\
   <option value=1>XXXXXX\n\
   <option value=2>50%\n\
   <option value=3>25%\n\
  </select>\n\
  <select name=\"presets\"style=\"POSITION:absolute; font-size:30pt; left:10px; top: 123px;width:300\">\n\
   <option value=1>FIX%\n\
   <option value=1>XXXXXX\n\
   <option value=2>50%\n\
   <option value=3>25%\n\
  </select>\n\
  <select name=\"presets\"style=\"POSITION:absolute; font-size:30pt; left:330px;top:123px\">\n\
   <option value=1>IMP%\n\
   <option value=1>XXXXXX\n\
   <option value=2>50%\n\
   <option value=3>25%\n\
  </select>\n\
  </body>\n\
</html>\n"};

#endif


#if (TEST_SOURCE_FIXED_WIDTH)
static char * demo_control_html = {
"<html>\n\
  <body>\n\
   <select name=\"cs\" style=\"POSITION:relative;left:181px; top:73px; font-size:30pt;width:300\">\n\
   <option value=1>FIX%\n\
   <option value=1>XXXXXX\n\
   <option value=2>50%\n\
   <option value=3>25%\n\
  </select>\n\
  <select name=\"presets\"style=\"POSITION:absolute; font-size:30pt; left:181px; top: 123px;width:300\">\n\
   <option value=1>FIX%\n\
   <option value=1>XXXXXX\n\
   <option value=2>50%\n\
   <option value=3>25%\n\
  </select>\n\
  </body>\n\
</html>\n"};
#endif

#if (TEST_SOURCE_IMPLICIT_WIDTH)
static char * demo_control_html = {
"<html>\n\
  <body>\n\
   <select name=\"cs\" style=\"POSITION:relative;left:181px; top:73px; font-size:30pt\">\n\
   <option value=1>IMP%\n\
   <option value=1>XXXXXX\n\
   <option value=2>50%\n\
   <option value=3>25%\n\
  </select>\n\
  <select name=\"presets\"style=\"POSITION:absolute; font-size:30pt; left:181px;top:123px\">\n\
   <option value=1>IMP%\n\
   <option value=1>XXXXXX\n\
   <option value=2>50%\n\
   <option value=3>25%\n\
  </select>\n\
  </body>\n\
</html>\n"};
#endif


/* Generic Demo setup step 6: Provide a demo entry point. This will be called
   from the dispatcher in webc_ctrl_demo_main.c
    The argument may be
        START_DEMO - To start a demo
        STOP_DEMO  - To stop a demo if necessary

    Most likey only the name of the demo entry point will need to be changed */

void webc_control_demo_select(int command)
{
    if (command == START_DEMO)
    {
        /* Call a helper function to load the root page, which breaks the browser into frames */
        if (webc_control_demo_load_string(wcdContext.hDemoDoc, demo_control_html) < 0)
            return;
        /* Register the controls we need from the document with the demo framework */
        init_demo_control_handles();
        /* Use a demo framework function to map controls in the document to offsets into the
            demo_control_handles array. Matching on HTML type and ID */
        if (webc_control_demo_extract_controls(wcdContext.hDemoDoc, &demo_control_handles[0],DEMO_CONTROL_COUNT)
            != DEMO_CONTROL_COUNT)
            return;
        /* Use a demo framework function to install our callback handlers */
        webc_control_demo_set_tag_handlers(wcdContext.hDemoDoc, &demo_control_handles[0],DEMO_CONTROL_COUNT);
    }
    else if (command == SHOW_INFO)
    {
        webc_control_demo_show_info();
    }
    else if (command == STOP_DEMO)
    {
        /* Put any necesseray stop code here */
    }
}
/* Generic Demo setup step 7: Provide callback routines to customize individual elemnts in
   the html provided in
    static char * demo_control_html = {
*/

static HTMLEventStatus cb_CONTROL_SELECT (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    webc_demo_event_log_event(event->type);

    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);

    /* The switch contains all event types currently sent to the callback */
    switch (event->type)
    {
        case HTML_EVENT_CHANGE:
        { /* Display the selected option's value */
            char *selectedOption;
            selectedOption = webc_ElementGetValueASCII(hElement);
            if(selectedOption)
            {
                webc_control_demo_update_info_action(selectedOption);
                webc_FreeASCIIString(selectedOption);
            }
        }
        break;
        /* Other events that are sent */
        case HTML_EVENT_CLICK:
        case HTML_EVENT_DBLCLICK:
        case HTML_EVENT_KEYDOWN:
        case HTML_EVENT_KEYPRESS:
        case HTML_EVENT_KEYUP:
        case HTML_EVENT_MOUSEDOWN:
        case HTML_EVENT_MOUSEMOVE:
        case HTML_EVENT_MOUSEOUT:
        case HTML_EVENT_MOUSEOVER:
        case HTML_EVENT_MOUSEUP:
        case HTML_EVENT_FOCUS:
        case HTML_EVENT_UNFOCUS:
        break;
        }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}
