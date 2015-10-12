#include "webc_ctrl_demo.h"
/*
webc_ctrl_demo_button.cpp

    void webc_control_demo_button(void)
        Demonstrate using the webc API to access and control HTML elements of type: <input type=button>

        The demo uses webc API functions to access and control these elements using
        handles of type HTML_INPUT_BUTTON_ELEMENT.


    This demo:
       . loads html text edit demonstration source code from a character array into the demo frame
            . Extracts HTML Element handles from the document
            . Assigns an event handler to that demonstrates application subclassing of control functions.
*/

/* Generic Demo setup step 1: Define several integer handles we will use to access controls */
#define CONTROL_FIRSTONE 0
#define CONTROL_SECONDONE 1
#define DEMO_CONTROL_COUNT    2
#define DEMOING_WHICH_HTML_ELEMENT HTML_INPUT_BUTTON_ELEMENT

/* Generic Demo setup step 2: Declare an array to hold the handles */
static WEBC_CONTROL_DEMO_HANDLE demo_control_handles[DEMO_CONTROL_COUNT];

/* Generic Demo setup step 3: Declare individual callback functions we will use to over-ride some default
   characteristics of individual controls */
static HTMLEventStatus cb_CONTROL_FIRSTORSECONDONE (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);

/* Generic Demo setup step 4: Initialize our control handle array html with values to match
  against the "id=" properties in the html and the control type
  Documentation moment: List the types
*/
static void init_demo_control_handles(void)
{
    webc_control_demo_init_control_array(&demo_control_handles[0],DEMO_CONTROL_COUNT);
    demo_control_handles[CONTROL_FIRSTONE].element_type = DEMOING_WHICH_HTML_ELEMENT;
    demo_control_handles[CONTROL_FIRSTONE].element_id = "CONTROL_FIRSTONE";
    demo_control_handles[CONTROL_FIRSTONE].element_handle = 0;
    demo_control_handles[CONTROL_FIRSTONE].cb = cb_CONTROL_FIRSTORSECONDONE;

    demo_control_handles[CONTROL_SECONDONE].element_type = DEMOING_WHICH_HTML_ELEMENT;
    demo_control_handles[CONTROL_SECONDONE].element_id = "CONTROL_SECONDONE";
    demo_control_handles[CONTROL_SECONDONE].element_handle = 0;
    demo_control_handles[CONTROL_SECONDONE].cb = cb_CONTROL_FIRSTORSECONDONE;

}

/* Generic Demo setup step 5: Provide an array of html source code that
   will be loaded into the document view of the demo browser when the demo
   is started. */

static char * demo_control_html = {
"<html>\n\
  <body>\n\
  <input type=button value=FIRSTONE_UP id=CONTROL_FIRSTONE>\n\
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n\
  <input type=button value=SECONDONE_UP id=CONTROL_SECONDONE>\n\
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n\
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n\
  <b>Generic for experimenteing <br>\n\
 </body>\n\
</html>\n"};

/* Generic Demo setup step 6: Provide a demo entry point. This will be called
   from the dispatcher in webc_ctrl_demo_main.c
    The argument may be
        START_DEMO - To start a demo
        STOP_DEMO  - To stop a demo if necessary

    Most likey only the name of the demo entry point will need to be changed */

void webc_control_demo_button(int command)
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
    cb_CONTROL_FIRSTONE
*/

static HTMLEventStatus cb_CONTROL_FIRSTORSECONDONE (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
 char *upButtonName,*downButtonName;
 //   webc_demo_event_log_event(event->type);

    if (hElement == demo_control_handles[CONTROL_FIRSTONE].element_handle)
    {
      upButtonName = "FIRSTONE_UP";
      downButtonName = "FIRSTONE_DOWN";
    }
    else if (hElement == demo_control_handles[CONTROL_FIRSTONE].element_handle)
    {
      upButtonName = "SECONDONE_UP";
      downButtonName = "SECONDONE_DOWN";
    }
    else
    {
      upButtonName = "BROKEN_UP";
      downButtonName = "BROKEN_DOWN";
    }

    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);

    /* The switch contains all event types currently sent to the callback */
    switch (event->type)
    { /* Print the Id on a click */
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
        }
        break;
        /* On mouseUp, MouseDonwn, change the valu in the button so it appears active */
        case HTML_EVENT_MOUSEDOWN:
            webc_ElementSetValueASCII (hElement, downButtonName, WEBC_TRUE);
        break;
        case HTML_EVENT_MOUSEUP:
            webc_ElementSetValueASCII (hElement, upButtonName, WEBC_TRUE);
        break;
        case HTML_EVENT_OBJECT_CREATED:
        case HTML_EVENT_KEYDOWN:
        case HTML_EVENT_KEYPRESS:
        case HTML_EVENT_KEYUP:
        case HTML_EVENT_MOUSEMOVE:
        case HTML_EVENT_MOUSEOUT:
        case HTML_EVENT_MOUSEOVER:
        case HTML_EVENT_FOCUS:
        case HTML_EVENT_UNFOCUS:
        break;
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}
