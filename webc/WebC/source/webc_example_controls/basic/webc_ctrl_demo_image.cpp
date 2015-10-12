#include "webc_ctrl_demo.h"
/*
webc_ctrl_demo_image.cpp

    void webc_control_demo_image(void)
        Demonstrate using the webc API to access and control HTML elements of type: <input type=image>

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
#define CONTROL_IMAGE 0
#define CONTROL_IMAGE_TWO 1
#define DEMO_CONTROL_COUNT    2
#define DEMOING_WHICH_HTML_ELEMENT HTML_IMAGE_ELEMENT

/* Generic Demo setup step 2: Declare an array to hold the handles */
static WEBC_CONTROL_DEMO_HANDLE demo_control_handles[DEMO_CONTROL_COUNT];

/* Generic Demo setup step 3: Declare individual callback functions we will use to over-ride some default
   characteristics of individual controls */
static HTMLEventStatus cb_CONTROL_IMAGE (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);

/* Generic Demo setup step 4: Initialize our control handle array html with values to match
  against the "id=" properties in the html and the control type
  Documentation moment: List the types
*/
static void init_demo_control_handles(void)
{
    webc_control_demo_init_control_array(&demo_control_handles[0],DEMO_CONTROL_COUNT);
    demo_control_handles[CONTROL_IMAGE].element_type = DEMOING_WHICH_HTML_ELEMENT;
    demo_control_handles[CONTROL_IMAGE].element_id = "CONTROL_IMAGE";
    demo_control_handles[CONTROL_IMAGE].element_handle = 0;
    demo_control_handles[CONTROL_IMAGE].cb = cb_CONTROL_IMAGE;

    demo_control_handles[CONTROL_IMAGE_TWO].element_type = DEMOING_WHICH_HTML_ELEMENT;
    demo_control_handles[CONTROL_IMAGE_TWO].element_id = "CONTROL_IMAGE_TWO";
    demo_control_handles[CONTROL_IMAGE_TWO].element_handle = 0;
    demo_control_handles[CONTROL_IMAGE_TWO].cb = cb_CONTROL_IMAGE;
}

/* Generic Demo setup step 5: Provide an array of html source code that
   will be loaded into the document view of the demo browser when the demo
   is started. */
static char * demo_control_html = {
"<html>\n\
  <body bgcolor=blue>\n\
  (id:CONTROL_IMAGE)\n\
  <img src=webc://ebslogo.png id=CONTROL_IMAGE border=2>\n\
  <br>(id:CONTROL_IMAGE_TWO)\n\
  <img src=\'webc://ebslogo.png\' id=CONTROL_IMAGE_TWO border=8><br>\n\
  <br>\n\
  <b>Two IMAGE controls<br>\n\
  <b>&nbsp;&nbsp;To Drag: Mouse Down; Drag; Mouse Up<br>\n\
  <b>&nbsp;&nbsp;To Select: Mouse Double Click<br>\n\
 </body>\n\
</html>\n"};

/* Generic Demo setup step 6: Provide a demo entry point. This will be called
   from the dispatcher in webc_ctrl_demo_main.c
    The argument may be
        START_DEMO - To start a demo
        STOP_DEMO  - To stop a demo if necessary

    Most likey only the name of the demo entry point will need to be changed */

void webc_control_demo_image(int command)
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
/* Callback for both images, just prints the image id when click. can implement rollovers, image maps etc */
static HTMLEventStatus cb_CONTROL_IMAGE (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
static HTMLEvent lastMouseEvent;
static int dragging = 0;
long x_displacement, y_displacement;

    x_displacement = y_displacement = 0;
    webc_demo_event_log_event(event->type);

    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);

    /* The switch contains all event types currently sent to the callback */
    switch (event->type)
    {
        case HTML_EVENT_DBLCLICK:
        {
            char *myId;
            myId = webc_ElementGetIdASCII(hElement);
            if (myId)
            {
                webc_control_demo_update_info_action(myId);
                webc_FreeASCIIString(myId);
            }
        }
        break;
        /* Other events that are sent */
        case HTML_EVENT_MOUSEDOWN:
            lastMouseEvent = *event;
            dragging = 1;
            webc_ElementClaimPointer(hElement);
            break;
        case HTML_EVENT_MOUSEOUT:
            break;
        case HTML_EVENT_MOUSEUP:
            webc_ElementReleasePointer(hElement);
            dragging = 0;
            break;
        case HTML_EVENT_MOUSEMOVE:
            if (dragging)
            {
                x_displacement = event->data.position.x - lastMouseEvent.data.position.x;
                y_displacement = event->data.position.y - lastMouseEvent.data.position.y;
                lastMouseEvent = *event;
            }
            break;
        case HTML_EVENT_CLICK:
        case HTML_EVENT_MOUSEOVER:
        default:
        break;
    }
/* If HTML_EVENT_KEYPRESS or HTML_EVENT_MOUSEMOVE initialized x_displacement or
   y_displacement then move the element */

    if (x_displacement || y_displacement)
    {
    HELEMENT_HANDLE parent;
    long my_parent_y;
    long my_parent_x;
    long my_parent_width;
    long my_parent_height;
    long my_width;
    long my_height;
    long new_xpos;
    long new_ypos;
    long current_xpos;
    long current_ypos;

        parent = webc_ElementGetParent (hElement);
        my_parent_width  = webc_ElementGetWidth(parent);
        my_parent_height  = webc_ElementGetHeight(parent);
        webc_ElementGetXYPosition(parent, &my_parent_x, &my_parent_y);
        webc_ElementGetXYPosition(wcdContext.hDemoFrame, &my_parent_x, &my_parent_y);

        /* Get the new position based on current position and deltax and deltay */
        webc_ElementGetXYPosition(hElement, &current_xpos, &current_ypos);
        my_width  = webc_ElementGetWidth(hElement);
        my_height  = webc_ElementGetHeight(hElement);
        new_xpos = current_xpos + x_displacement;
        new_ypos = current_ypos + y_displacement;
        if ( (new_ypos >= 0) &&
             ((new_ypos+my_height) < my_parent_height) &&
             (new_xpos >= 0) &&
             (new_xpos+my_width < my_parent_width) )
        {
            webc_ElementMove(hElement, x_displacement, y_displacement);
        }
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}
