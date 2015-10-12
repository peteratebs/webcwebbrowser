#include "webc_ctrl_demo.h"
/*
webc_ctrl_demo_checkbox.cpp

    void webc_control_demo_checkbox(void)
        Demonstrate using the webc API to access and control HTML elements of type: <input type=checkbox>

        The demo uses webc API functions to access and control these elements using
        handles of type HTML_CHECKBOX_ELEMENT.


        HTML_CHECKBOX_ELEMENT controls can be manipulated by the webC API and Events to them can be monitored
        and intercepted by installing application specific event handler for individual elements.


    This demo:
       . loads html text edit demonstration source code from a character array into the demo frame
            . Extracts HTML Element handles from the document
            . Assigns an event handler to that demonstrates application subclassing of control functions.

*/

/* Generic Demo setup step 1: Define several integer handles we will use to access controls */
#define CONTROL_ONLYONE 0
#define DEMO_CONTROL_COUNT    1
#define DEMOING_WHICH_HTML_ELEMENT HTML_CHECKBOX_ELEMENT

/* Generic Demo setup step 2: Declare an array to hold the handles */
static WEBC_CONTROL_DEMO_HANDLE demo_control_handles[DEMO_CONTROL_COUNT];

/* Generic Demo setup step 3: Declare individual callback functions we will use to over-ride some default
   characteristics of individual controls */
static HTMLEventStatus cb_CONTROL_ONLYONE (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);

/* Generic Demo setup step 4: Initialize our control handle array html with values to match
  against the "id=" properties in the html and the control type
  Documentation moment: List the types
*/
static void init_demo_control_handles(void)
{
    webc_control_demo_init_control_array(&demo_control_handles[0],DEMO_CONTROL_COUNT);
    demo_control_handles[CONTROL_ONLYONE].element_type = DEMOING_WHICH_HTML_ELEMENT;
    demo_control_handles[CONTROL_ONLYONE].element_id = "CONTROL_ONLYONE";
    demo_control_handles[CONTROL_ONLYONE].element_handle = 0;
    demo_control_handles[CONTROL_ONLYONE].cb = cb_CONTROL_ONLYONE;


}

/* Generic Demo setup step 5: Provide an array of html source code that
   will be loaded into the document view of the demo browser when the demo
   is started. */

static char * demo_control_html = {
"<html>\n\
  <body>\n\
  (id=CONTROL_ONLYONE) <input type=checkbox id=CONTROL_ONLYONE<br>\n\
  <b>Generic for experimenteing <br>\n\
 </body>\n\
</html>\n"};

/* Generic Demo setup step 6: Provide a demo entry point. This will be called
   from the dispatcher in webc_ctrl_demo_main.c
    The argument may be
        START_DEMO - To start a demo
        STOP_DEMO  - To stop a demo if necessary

    Most likey only the name of the demo entry point will need to be changed */

void webc_control_demo_checkbox(int command)
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

    cb_CONTROL_ONLYONE
    Note these functions are mapped to the above html by name and type by webc_control_demo_extract_controls
    using the keys provided provided by init_demo_control_handles().
*/

/*    cb_CONTROL_AUTOINCREMENT - Text control with with arrow key conroled increment and decrement */
static HTMLEventStatus cb_CONTROL_ONLYONE (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    webc_demo_event_log_event(event->type);

    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);

    /* The switch contains all event types currently sent to the callback */
    /* Intercept HTML_EVENT_KEYDOWN on the keys WGK_LNUP and  WGK_LNDNImplement.
       increment or decrement the contents */
    switch (event->type)
    {
        case HTML_EVENT_CLICK:
        case HTML_EVENT_DBLCLICK:
            if (webc_ElementGetChecked(hElement))
                webc_control_demo_update_info_action("click Down.. IS CHECKED");
            else
                webc_control_demo_update_info_action("click Down.. Not CHECKED");
            break;
        case HTML_EVENT_KEYDOWN:
        case HTML_EVENT_KEYPRESS:
            break;
        case HTML_EVENT_KEYUP:
            if (webc_ElementGetChecked(hElement))
                webc_control_demo_update_info_action("Keydown Down.. IS CHECKED");
            else
                webc_control_demo_update_info_action("Keydown Down.. Not CHECKED");
            break;
        case HTML_EVENT_OBJECT_CREATED:
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
