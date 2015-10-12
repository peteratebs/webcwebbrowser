#include "webc_ctrl_demo.h"
/*
webc_ctrl_demo_password.cpp

    void webc_control_demo_password(void)
        Demonstrate using the webc API to access and control HTML elements of type: <input type=password>

    This demo:
       . loads html text edit demonstration source code from a character array into the demo frame
            . Extracts HTML Element handles from the document
            . Assigns an event handler to that demonstrates application subclassing of control functions.

*/

/* Generic Demo setup step 1: Define several integer handles we will use to access controls */
#define CONTROL_PASSWORD 0
#define DEMO_CONTROL_COUNT    1
#define DEMOING_WHICH_HTML_ELEMENT HTML_INPUT_PASSWORD_ELEMENT

/* Generic Demo setup step 2: Declare an array to hold the handles */
static WEBC_CONTROL_DEMO_HANDLE demo_control_handles[DEMO_CONTROL_COUNT];

/* Generic Demo setup step 3: Declare individual callback functions we will use to over-ride some default
   characteristics of individual controls */
static HTMLEventStatus cb_CONTROL_PASSWORD (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);

/* Generic Demo setup step 4: Initialize our control handle array html with values to match
  against the "id=" properties in the html and the control type
  Documentation moment: List the types
*/
static void init_demo_control_handles(void)
{
    webc_control_demo_init_control_array(&demo_control_handles[0],DEMO_CONTROL_COUNT);
    demo_control_handles[CONTROL_PASSWORD].element_type = DEMOING_WHICH_HTML_ELEMENT;
    demo_control_handles[CONTROL_PASSWORD].element_id = "CONTROL_PASSWORD";
    demo_control_handles[CONTROL_PASSWORD].element_handle = 0;
    demo_control_handles[CONTROL_PASSWORD].cb = cb_CONTROL_PASSWORD;
}

/* Generic Demo setup step 5: Provide an array of html source code that
   will be loaded into the document view of the demo browser when the demo
   is started. */

static char * demo_control_html = {
"<html>\n\
  <body>\n\
  (id:CONTROL_PASSWORD)\n\
  <input type=password id=CONTROL_PASSWORD><br>\n\
 </body>\n\
</html>\n"};

/* Generic Demo setup step 6: Provide a demo entry point. This will be called
   from the dispatcher in webc_ctrl_demo_main.c
    The argument may be
        START_DEMO - To start a demo
        STOP_DEMO  - To stop a demo if necessary

    Most likey only the name of the demo entry point will need to be changed */

void webc_control_demo_password(int command)
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
   the html provided in   static char * demo_control_html = {
*/

static HTMLEventStatus cb_CONTROL_PASSWORD (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    webc_demo_event_log_event(event->type);

    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);

    /* The switch contains all event types currently sent to the callback */
    switch (event->type)
    {  /* The display is masked but we'll update the status field each time */
        case HTML_EVENT_KEYUP:
        case HTML_EVENT_CHANGE:
        { /* Display the passworded option's value */
            char *passwordContent;
            passwordContent = webc_ElementGetValueASCII(hElement);
            if(passwordContent)
            {
                webc_control_demo_update_info_action(passwordContent);
                webc_FreeASCIIString(passwordContent);
            }
        }
        default:
        /* Other events that are sent */
        case HTML_EVENT_KEYDOWN:
        case HTML_EVENT_KEYPRESS:
        case HTML_EVENT_MOUSEDOWN:
        case HTML_EVENT_CLICK:
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
