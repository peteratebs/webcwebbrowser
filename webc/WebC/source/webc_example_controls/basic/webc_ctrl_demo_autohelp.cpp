#include "webc_ctrl_demo.h"
/*
webc_ctrl_demo_autohelp.cpp

    void ctrl_demo_autohelp(void)
        Demonstrate using the webc API to implement rollover help
*/

/* Generic Demo setup step 1: Define several integer handles we will use to access controls */
#define CONTROL_INPUT_ONE        0
#define CONTROL_INPUT_TWO        1
#define CONTROL_INPUT_THREE      2
#define CONTROL_FLOATING_HELPBOX 3
#define CONTROL_FLOATING_DIV     4
#define DEMO_CONTROL_COUNT       5

/* Generic Demo setup step 2: Declare an array to hold the handles */
static WEBC_CONTROL_DEMO_HANDLE demo_control_handles[DEMO_CONTROL_COUNT];

/* Generic Demo setup step 3: Declare individual callback functions we will use to over-ride some default
   characteristics of individual controls */
static HTMLEventStatus cb_CONTROL_INPUT (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);

/* Generic Demo setup step 4: Initialize our control handle array html with values to match
  against the "id=" properties in the html and the control type
  Documentation moment: List the types
*/
static void init_demo_control_handles(void)
{
    webc_control_demo_init_control_array(&demo_control_handles[0],DEMO_CONTROL_COUNT);

    demo_control_handles[CONTROL_INPUT_ONE].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_INPUT_ONE].element_id = "CONTROL_INPUT_ONE";
    demo_control_handles[CONTROL_INPUT_ONE].element_handle = 0;
    demo_control_handles[CONTROL_INPUT_ONE].cb = cb_CONTROL_INPUT;

    demo_control_handles[CONTROL_INPUT_TWO].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_INPUT_TWO].element_id = "CONTROL_INPUT_TWO";
    demo_control_handles[CONTROL_INPUT_TWO].element_handle = 0;
    demo_control_handles[CONTROL_INPUT_TWO].cb = cb_CONTROL_INPUT;

    demo_control_handles[CONTROL_INPUT_THREE].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_INPUT_THREE].element_id = "CONTROL_INPUT_THREE";
    demo_control_handles[CONTROL_INPUT_THREE].element_handle = 0;
    demo_control_handles[CONTROL_INPUT_THREE].cb = cb_CONTROL_INPUT;

    demo_control_handles[CONTROL_FLOATING_HELPBOX].element_type = HTML_EDITBOX_ELEMENT;
    demo_control_handles[CONTROL_FLOATING_HELPBOX].element_id = "CONTROL_FLOATING_HELPBOX";
    demo_control_handles[CONTROL_FLOATING_HELPBOX].element_handle = 0;
    demo_control_handles[CONTROL_FLOATING_HELPBOX].cb = 0;

    demo_control_handles[CONTROL_FLOATING_DIV].element_type = HTML_DIV_ELEMENT;
    demo_control_handles[CONTROL_FLOATING_DIV].element_id = "CONTROL_FLOATING_DIV";
    demo_control_handles[CONTROL_FLOATING_DIV].element_handle = 0;
    demo_control_handles[CONTROL_FLOATING_DIV].cb = 0;

}

/* Generic Demo setup step 5: Provide an array of html source code that
   will be loaded into the document view of the demo browser when the demo
   is started. */

static char * demo_control_html = {
"<html>\n\
  <body>\n\
  First  Field:  <input type=text id=CONTROL_INPUT_ONE>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n\
  Second Field:  <input type=text id=CONTROL_INPUT_TWO><br><br><br>\n\
  &nbsp;&nbsp;&nbsp;Third  Field:  <input type=text id=CONTROL_INPUT_THREE><br><br>\n\
  <center> Roll over the input fields to see balloon help.</center>\n\
  <div id=CONTROL_FLOATING_DIV>\n\
  <textarea style=width:100px; style=height:100px; style=visibility:hidden; id=CONTROL_FLOATING_HELPBOX name=NAME_CONTROL_EDITBOX cols=10 rows=6>\n\
  </textarea>\n\
  </div>\n\
 </body>\n\
</html>\n"};

/* Generic Demo setup step 6: Provide a demo entry point. This will be called
   from the dispatcher in webc_ctrl_demo_main.c
    The argument may be
        START_DEMO - To start a demo
        STOP_DEMO  - To stop a demo if necessary

    Most likey only the name of the demo entry point will need to be changed */

void webc_control_demo_autohelp(int command)
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
         return;
    } else if (command == STOP_DEMO)
    {
        /* Put any necesseray stop code here */
    }
}
/* Generic Demo setup step 7: Provide callback routines to customize individual elemnts in
   the html provided in
    static char * demo_control_html = {

    cb_CONTROL_OPTION
    Note these functions are mapped to the above html by name and type by webc_control_demo_extract_controls
    using the keys provided provided by init_demo_control_handles().
*/

static HTMLEventStatus cb_CONTROL_INPUT (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    webc_demo_event_log_event(event->type);

    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);

    /* The switch contains all event types currently sent to the callback */
    switch (event->type)
    {
        char *the_string;
        case HTML_EVENT_MOUSEOVER:
            /* Set Field specific help context */
            if (hElement == demo_control_handles[CONTROL_INPUT_ONE].element_handle)
                the_string = "Help \n For \n Field \n One\n";
            else if (hElement == demo_control_handles[CONTROL_INPUT_TWO].element_handle)
                the_string = "Help \n For \n Field \n Two\n";
            else if (hElement == demo_control_handles[CONTROL_INPUT_THREE].element_handle)
                the_string = "Help \n For \n Field \n Three\n";
            else
                the_string = "Help \n System \n Is \n Lost\n";
            webc_ElementSetValueASCII(demo_control_handles[CONTROL_FLOATING_HELPBOX].element_handle, the_string, WEBC_TRUE);

            /* Position the help box next to the field */
            {
            char pos_buffer[100];
            long my_posx,my_posy,my_width;
            webc_ElementGetXYPosition(hElement, &my_posx, &my_posy);
            my_width = webc_ElementGetWidth(hElement);
            rtp_sprintf(pos_buffer, "position:absolute;left:%d;top:%d;", my_posx + my_width, my_posy);
            webc_ElementSetStyleASCII(demo_control_handles[CONTROL_FLOATING_DIV].element_handle,
                pos_buffer,WEBC_TRUE);
            }

            /* Make the help box visible */
            webc_ElementSetStyleASCII(demo_control_handles[CONTROL_FLOATING_HELPBOX].element_handle,
                "visibility:show;",WEBC_TRUE);
            break;
        case HTML_EVENT_MOUSEOUT:
            /* Make the help box invisible */
            webc_ElementSetStyleASCII(demo_control_handles[CONTROL_FLOATING_HELPBOX].element_handle,
                "visibility:hidden;",WEBC_TRUE);
            break;
        default:
        break;
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}
