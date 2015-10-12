#include "webc_ctrl_demo.h"

/*
webc_ctrl_demo_color_blender.cpp

    void ctrl_demo_color_blender(void)

        This demo is not complete
*/

/* Generic Demo setup step 1: Define several integer handles we will use to access controls */
#define CONTROL_COLOR_ONE 0
#define CONTROL_BUTTON 1
#define DEMO_CONTROL_COUNT    2
static int bgred=1, bggreen=1, bgblue=1,fgred = 1,fggreen = 1,fgblue=1;

/* Generic Demo setup step 2: Declare an array to hold the handles */
static WEBC_CONTROL_DEMO_HANDLE demo_control_handles[DEMO_CONTROL_COUNT];

/* Generic Demo setup step 3: Declare individual callback functions we will use to over-ride some default
   characteristics of individual controls */
static HTMLEventStatus cb_CONTROL_BGFGUPDATE (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);

/* Generic Demo setup step 4: Initialize our control handle array html with values to match
  against the "id=" properties in the html and the control type
  Documentation moment: List the types
*/
static void init_demo_control_handles(void)
{
    webc_control_demo_init_control_array(&demo_control_handles[0],DEMO_CONTROL_COUNT);
    demo_control_handles[CONTROL_COLOR_ONE].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_COLOR_ONE].element_id = "CONTROL_COLOR_ONE";
    demo_control_handles[CONTROL_COLOR_ONE].element_handle = 0;
    demo_control_handles[CONTROL_COLOR_ONE].cb = 0;

    demo_control_handles[CONTROL_BUTTON].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[CONTROL_BUTTON].element_id = "CONTROL_BUTTON";
    demo_control_handles[CONTROL_BUTTON].element_handle = 0;
    demo_control_handles[CONTROL_BUTTON].cb = cb_CONTROL_BGFGUPDATE;

}

/* Generic Demo setup step 5: Provide an array of html source code that
   will be loaded into the document view of the demo browser when the demo
   is started. */

static char * demo_control_html = {
"<html>\n\
     <body>\n\
     <input type=text value=XoXoXOXoXoX id=CONTROL_COLOR_ONE >\n\
     <input type=button id=CONTROL_BUTTON value=CHANGE_COLOR>\n\
     <br>Click on the button and then use the keyboard (keypress with repeat) to change the colors<br>\n\
    </body>\n\
</html>\n"};

/* Generic Demo setup step 6: Provide a demo entry point. This will be called
   from the dispatcher in webc_ctrl_demo_main.c
    The argument may be
        START_DEMO - To start a demo
        STOP_DEMO  - To stop a demo if necessary

    Most likey only the name of the demo entry point will need to be changed */

void webc_control_demo_color_blender(int command)
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

static HTMLEventStatus cb_CONTROL_BGFGUPDATE (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{

    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);

    /* The switch contains all event types currently sent to the callback */
    switch (event->type)
    { /* Print the Id on a click */
        case HTML_EVENT_CLICK:
            break;
        case HTML_EVENT_KEYDOWN:
        case HTML_EVENT_MOUSEDOWN:
        {
      	char ascii_colorStr[128];
            if (fgblue++ > 255)  { fgblue = 0;}
            if (fggreen++ > 255) { fggreen = 0;}
            if (fgred++ > 255)   { fgred = 0; }
              if (bgred++ > 255)   { bgred = 0; }
            if (bggreen++ > 255) { bggreen = 0;}
            if (bgblue++ > 255)  { bgblue = 0;}
      /* rtp_sprintf .. not tested, not good solution */
	        rtp_sprintf(ascii_colorStr, "background-color:#%02x%02x%02x;color:#%02x%02x%02x;",
			bgred, bggreen,	bgblue, fgred, fggreen,	fgblue);
            webc_ElementSetStyleASCII(demo_control_handles[CONTROL_COLOR_ONE].element_handle,ascii_colorStr,WEBC_TRUE);
            webc_control_demo_update_info_action(ascii_colorStr);
            break;
        }
        case HTML_EVENT_MOUSEUP:
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
