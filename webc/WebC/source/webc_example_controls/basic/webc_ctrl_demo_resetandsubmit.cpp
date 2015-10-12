#include "webc_ctrl_demo.h"
/*
webc_ctrl_demo_radio_button.cpp

    void webc_control_demo_radio_button(void)
        Demonstrate using the webc API to access and control HTML elements of type: <input type=reset> <input type=submit>

    This demo:
       . loads html text edit demonstration source code from a character array into the demo frame
            . Extracts HTML Element handles from the document
            . Assigns an event handler to that demonstrates application subclassing of control functions.

*/

/* Generic Demo setup step 1: Define several integer handles we will use to access controls */
#define CONTROL_RESET   0
#define CONTROL_SUBMIT  1
#define CONTROL_THEFORM 2
#define DEMO_CONTROL_COUNT    3
//#define DEMOING_WHICH_HTML_ELEMENT HTML_RESETANDSUBMIT_ELEMENT

/* Generic Demo setup step 2: Declare an array to hold the handles */
static WEBC_CONTROL_DEMO_HANDLE demo_control_handles[DEMO_CONTROL_COUNT];

/* Generic Demo setup step 3: Declare individual callback functions we will use to over-ride some default
   characteristics of individual controls */
static HTMLEventStatus cb_CONTROL_RESETANDSUBMIT (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_CONTROL_THEFORM (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);

/* Generic Demo setup step 4: Initialize our control handle array html with values to match
  against the "id=" properties in the html and the control type
  Documentation moment: List the types
*/
static void init_demo_control_handles(void)
{
    webc_control_demo_init_control_array(&demo_control_handles[0],DEMO_CONTROL_COUNT);
    demo_control_handles[CONTROL_RESET].element_type = HTML_INPUT_RESET_ELEMENT;
    demo_control_handles[CONTROL_RESET].element_id = "CONTROL_RESET";
    demo_control_handles[CONTROL_RESET].element_handle = 0;
    demo_control_handles[CONTROL_RESET].cb = cb_CONTROL_RESETANDSUBMIT;

    demo_control_handles[CONTROL_SUBMIT].element_type = HTML_BUTTON_ELEMENT;
    demo_control_handles[CONTROL_SUBMIT].element_id = "CONTROL_SUBMIT";
    demo_control_handles[CONTROL_SUBMIT].element_handle = 0;
    demo_control_handles[CONTROL_SUBMIT].cb = cb_CONTROL_RESETANDSUBMIT;

    demo_control_handles[CONTROL_THEFORM].element_type = HTML_FORM_ELEMENT;
    demo_control_handles[CONTROL_THEFORM].element_id = "CONTROL_THEFORM";
    demo_control_handles[CONTROL_THEFORM].element_handle = 0;
    demo_control_handles[CONTROL_THEFORM].cb = cb_CONTROL_THEFORM;
}

/* Generic Demo setup step 5: Provide an array of html source code that
   will be loaded into the document view of the demo browser when the demo
   is started. */

static char * demo_control_html = {
"<html>\n\
  <body>\n\
  <form id=CONTROL_THEFORM>\n\
  Text Input Field 1:<input type=text value=defaulttext id=TEXT_INPUT_FIELD><br>\n\
  Text Input Field 2:<input type=text id=TEXT_INPUT_FIELD_2><br>\n\
  <input type=reset value=RESET_UP id=CONTROL_RESET>\n\
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n\
  <input type=submit value=SUBMIT_UP id=CONTROL_SUBMIT>\n\
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n\
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n\
  <b>Generic for experimenteing <br>\n\
  </form>\n\
 </body>\n\
</html>\n"};

/* Generic Demo setup step 6: Provide a demo entry point. This will be called
   from the dispatcher in webc_ctrl_demo_main.c
    The argument may be
        START_DEMO - To start a demo
        STOP_DEMO  - To stop a demo if necessary

    Most likey only the name of the demo entry point will need to be changed */

void webc_control_demo_resetandsubmit(int command)
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
static HTMLEventStatus cb_CONTROL_THEFORM (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);

    /* The switch contains all event types currently sent to the callback */
    /* Intercept HTML_EVENT_KEYDOWN on the keys WGK_LNUP and  WGK_LNDNImplement.
       increment or decrement the contents */
    switch (event->type)
    {
        case HTML_EVENT_SUBMIT:
            break;
        case HTML_EVENT_CLICK:
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
        case HTML_EVENT_CHANGE:
        case HTML_EVENT_OBJECT_DESTROYED:
        break;
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

/* Generic Demo setup step 7: Provide callback routines to customize individual elemnts in
   the html provided in
    cb_CONTROL_RESET
*/
static HTMLEventStatus cb_CONTROL_RESETANDSUBMIT (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);

    webc_demo_event_log_event(event->type);

    switch (event->type)
    {
    char buffer[256];
        case HTML_EVENT_CLICK:
        {
        char *myId;
           myId = webc_ElementGetIdASCII(hElement);
           if (myId)
           {
               rtp_sprintf(buffer, "CLICK (%s)",myId);
               webc_control_demo_update_info_action(buffer);
               webc_FreeASCIIString(myId);
           }
            /* Enable the next lines if you don't want to submit */
            if (0 || hElement == demo_control_handles[CONTROL_SUBMIT].element_handle)
                return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
            /* Do the next lines if you don't want to reset */
            else if (0 || hElement == demo_control_handles[CONTROL_RESET].element_handle)
            {
                return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
            }
        }
        break;
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
