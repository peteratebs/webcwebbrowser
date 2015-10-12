#include "webc_ctrl_demo.h"
/*

webc_ctrl_demo_sliders.cpp

    void webc_control_demo_sliders(void)
        Demonstrate using multiple webc API calls to implement a slider control

    This demo:
       . loads html text edit demonstration source code from a character array into the demo frame
            . Extracts HTML Element handles from the document
            . Assigns an event handler to that demonstrates application subclassing of control functions.

*/

/* Generic Demo setup step 1: Define several integer handles we will use to access controls */
#define CONTROL_SLIDER_TC_ONE       0
#define CONTROL_SLIDER_TC_TWO       1
#define CONTROL_SLIDER_COUNTER_ONE  2
#define CONTROL_SLIDER_BUTTON_ONE   3
#define CONTROL_SLIDER_BUTTON_UP    4
#define CONTROL_SLIDER_BUTTON_DOWN  5

#define DEMO_CONTROL_COUNT          6

/* Generic Demo setup step 2: Declare an array to hold the handles */
static WEBC_CONTROL_DEMO_HANDLE demo_control_handles[DEMO_CONTROL_COUNT];

/* Generic Demo setup step 3: Declare individual callback functions we will use to over-ride some default
   characteristics of individual controls */
static HTMLEventStatus cb_CONTROL_SLIDER_BUTTON_ONE  (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_CONTROL_SLIDER_BUTTON_UP   (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_CONTROL_SLIDER_BUTTON_DOWN (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_CONTROL_SLIDER_TC_ONE_TWO  (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static int slider_one_is_dragging = 0;
#define SLIDER_WIDTH 200
#define FONTHEIGHT 40
static int slider_one_control_start_X = 100;
static int slider_one_control_start_Y = 40;
static int slider_two_control_start_X = 200;
static int slider_two_control_start_Y = 40;
static int slider_one_control_size  = 1;
static int slider_two_control_size  = SLIDER_WIDTH-1;
static int slider_one_counter_start_X = 200;
static int slider_one_counter_start_Y = 40;
static int slider_one_counter_size   = 40;
static int slider_one_button_up_start_X      = 340;
static int slider_one_button_down_start_X    = 340;
static int slider_one_button_down_size = 10;

static HTMLEvent slider_one_lastMouseEvent;

/* Generic Demo setup step 4: Initialize our control handle array html with values to match
  against the "id=" properties in the html and the control type
  Documentation moment: List the types
*/
static void init_demo_control_handles(void)
{
    webc_control_demo_init_control_array(&demo_control_handles[0],DEMO_CONTROL_COUNT);

    demo_control_handles[CONTROL_SLIDER_TC_ONE].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_SLIDER_TC_ONE].element_id = "CONTROL_SLIDER_TC_ONE";
    demo_control_handles[CONTROL_SLIDER_TC_ONE].element_handle = 0;
    demo_control_handles[CONTROL_SLIDER_TC_ONE].cb = cb_CONTROL_SLIDER_TC_ONE_TWO;

    demo_control_handles[CONTROL_SLIDER_TC_TWO].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_SLIDER_TC_TWO].element_id = "CONTROL_SLIDER_TC_TWO";
    demo_control_handles[CONTROL_SLIDER_TC_TWO].element_handle = 0;
    demo_control_handles[CONTROL_SLIDER_TC_TWO].cb = cb_CONTROL_SLIDER_TC_ONE_TWO;

    demo_control_handles[CONTROL_SLIDER_BUTTON_ONE].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[CONTROL_SLIDER_BUTTON_ONE].element_id = "CONTROL_SLIDER_BUTTON_ONE";
    demo_control_handles[CONTROL_SLIDER_BUTTON_ONE].element_handle = 0;
    demo_control_handles[CONTROL_SLIDER_BUTTON_ONE].cb = cb_CONTROL_SLIDER_BUTTON_ONE;

    /* Create up and down buttons */
    demo_control_handles[CONTROL_SLIDER_BUTTON_UP].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[CONTROL_SLIDER_BUTTON_UP].element_id = "CONTROL_SLIDER_BUTTON_UP";
    demo_control_handles[CONTROL_SLIDER_BUTTON_UP].element_handle = 0;
    demo_control_handles[CONTROL_SLIDER_BUTTON_UP].cb = cb_CONTROL_SLIDER_BUTTON_UP;

    demo_control_handles[CONTROL_SLIDER_BUTTON_DOWN].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[CONTROL_SLIDER_BUTTON_DOWN].element_id = "CONTROL_SLIDER_BUTTON_DOWN";
    demo_control_handles[CONTROL_SLIDER_BUTTON_DOWN].element_handle = 0;
    demo_control_handles[CONTROL_SLIDER_BUTTON_DOWN].cb = cb_CONTROL_SLIDER_BUTTON_DOWN;

    /* Create an edit control that we'll put the current value in */
    demo_control_handles[CONTROL_SLIDER_COUNTER_ONE].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_SLIDER_COUNTER_ONE].element_id = "CONTROL_SLIDER_COUNTER_ONE";
    demo_control_handles[CONTROL_SLIDER_COUNTER_ONE].element_handle = 0;
    demo_control_handles[CONTROL_SLIDER_COUNTER_ONE].cb = 0;
}

/* Generic Demo setup step 5: Provide an array of html source code that
   will be loaded into the document view of the demo browser when the demo
   is started. */
//     <input style=position:absolute;left:10;top:40 type=button src=webc://alert.png value=slider id=CONTROL_SLIDER_BUTTON_ONE>\n\

static char * demo_control_html = {
"<html>\n\
     <body>\n\
     <center>Click inside the slider or on ^ or v to move thumb<br>\n\
     Or click on the thumb and drag<br><br></center>\n\
     <input style=width:1px;background-color:Red;position:absolute;left:100;top:40 type=text id=CONTROL_SLIDER_TC_ONE\n\
     value=\"(empty)                                          (full)\">\n\
     <input style=width:200px;background-color:LightBlue;position:absolute;left:100;top:40 type=text id=CONTROL_SLIDER_TC_TWO >\n\
     <input style=position:absolute;left:100;top:40 type=button value=| id=CONTROL_SLIDER_BUTTON_ONE>\n\
     <input style=width:10px; style=position:absolute;left:340;top:40 type=button value=^ id=CONTROL_SLIDER_BUTTON_UP>\n\
     <input style=width:10px; style=position:absolute;left:340;top:40 type=button value=v id=CONTROL_SLIDER_BUTTON_DOWN>\n\
     <input style=width:40px;background-color:Green;position:absolute;left:340;top:40 value=0 type=text id=CONTROL_SLIDER_COUNTER_ONE>\n\
    </body>\n\
</html>\n"};

/* Generic Demo setup step 6: Provide a demo entry point. This will be called
   from the dispatcher in webc_ctrl_demo_main.c
    The argument may be
        START_DEMO - To start a demo
        STOP_DEMO  - To stop a demo if necessary

    Most likey only the name of the demo entry point will need to be changed */

static void draw_slider_one(void);
void webc_control_demo_sliders(int command)
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

        slider_one_control_start_X = 100;
        slider_one_control_start_Y = 40;
        slider_two_control_start_X = 100+SLIDER_WIDTH/2;
        slider_two_control_start_Y = 40;
        slider_one_control_size  = (SLIDER_WIDTH/2) - 1;
        slider_two_control_size  = (SLIDER_WIDTH/2) + 1;
        slider_one_counter_start_X = slider_two_control_start_X;
        slider_one_counter_start_Y = 40;
        slider_one_counter_size   = 40;
        slider_one_is_dragging = 0;
        slider_one_button_up_start_X      = 340;
        slider_one_button_down_start_X    = 340;
        slider_one_button_down_size = 10;
        draw_slider_one();

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
/* Event handler for the button, slides when dragged with mouse down */
static HTMLEventStatus cb_CONTROL_SLIDER_BUTTON_ONE (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{

    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);

    /* The switch contains all event types currently sent to the callback */
    switch (event->type)
    { /* Print the Id on a click */
        case HTML_EVENT_MOUSEDOWN:
            slider_one_is_dragging = 1;
            slider_one_lastMouseEvent = *event;
            webc_ElementClaimPointer(hElement);
            break;
        case HTML_EVENT_MOUSEUP:
            webc_ElementReleasePointer(hElement);
            slider_one_is_dragging = 0;
            break;
        case HTML_EVENT_MOUSEMOVE:
            if (slider_one_is_dragging)
            {
                long delta_x;

                delta_x = (event->data.position.x - slider_one_lastMouseEvent.data.position.x);

                if (  ((slider_one_control_size + delta_x) < SLIDER_WIDTH) &&
                      ((slider_one_control_size + delta_x) > 0) )
                { /* Change locations and draw slider */
                    slider_one_control_size += delta_x;
                    slider_two_control_size -= delta_x;
                    slider_two_control_start_X = slider_one_control_start_X+slider_one_control_size;
                    draw_slider_one();
                    slider_one_lastMouseEvent = *event;
                }
            }
            break;
        case HTML_EVENT_OBJECT_CREATED:
            slider_two_control_start_X = slider_one_control_start_X+slider_one_control_size;
            draw_slider_one();
            break;
        case HTML_EVENT_MOUSEOUT:
        case HTML_EVENT_CLICK:
        case HTML_EVENT_KEYDOWN:
        case HTML_EVENT_KEYPRESS:
        case HTML_EVENT_KEYUP:
        case HTML_EVENT_MOUSEOVER:
        case HTML_EVENT_FOCUS:
        case HTML_EVENT_UNFOCUS:
        break;
    }

    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

/* Event handler for the slider. Moves button to the location when clicked */
static HTMLEventStatus cb_CONTROL_SLIDER_TC_ONE_TWO (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    switch (event->type)
    {
        case HTML_EVENT_CLICK:
        {
            long mouse_x;
            mouse_x = (long) event->data.position.x;
            if (hElement == demo_control_handles[CONTROL_SLIDER_TC_ONE].element_handle)
            { /* In slider one, make the new size the click point */
                if ( (mouse_x - slider_one_control_start_X) > 0)
                    slider_one_control_size = (mouse_x - slider_one_control_start_X);
            }
            else
            { /* In slider two, add the offset into two to first control
                 (will increase it by the same amout we decrease ourselves */
                if ( ((mouse_x - slider_two_control_start_X)+slider_one_control_size) < SLIDER_WIDTH)
                    slider_one_control_size += (mouse_x - slider_two_control_start_X);
            }
            /* Change locations and draw slider */
            slider_two_control_size = SLIDER_WIDTH - slider_one_control_size;
            slider_two_control_start_X = slider_one_control_start_X + slider_one_control_size;
            draw_slider_one();
       }
       break;
        default:
       break;
    };
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

static HTMLEventStatus cb_CONTROL_SLIDER_BUTTON_UP   (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    switch (event->type)
    { /* Print the Id on a click */
        case HTML_EVENT_CLICK:
            {
                long delta_x;
                delta_x = 4;

                if (slider_one_control_size + delta_x < SLIDER_WIDTH)
                { /* Change locations and draw slider */
                    slider_one_control_size += delta_x;
                    slider_two_control_size -= delta_x;
                    slider_two_control_start_X = slider_one_control_start_X+slider_one_control_size;
                    draw_slider_one();
                }
            }
            break;
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */

}

static HTMLEventStatus cb_CONTROL_SLIDER_BUTTON_DOWN   (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    switch (event->type)
    { /* Print the Id on a click */
        case HTML_EVENT_CLICK:
            {
                long delta_x;
                delta_x = -4;

                if (slider_one_control_size + delta_x > 4)
                { /* Change locations and draw slider */
                    slider_one_control_size += delta_x;
                    slider_two_control_size -= delta_x;
                    slider_two_control_start_X = slider_one_control_start_X+slider_one_control_size;
                    draw_slider_one();
                }
            }
            break;
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */

}

/* Draw the slider control */
static void draw_slider_one(void)
{
    char ascii_buff[64];
#define GAP 4
    /* Set the origin of the up and down buttons, (they won't change but it's good practice */
    slider_one_button_down_start_X = slider_one_control_start_X - (slider_one_button_down_size + GAP);
    rtp_sprintf(ascii_buff, "position:absolute;left:%d;top:%d",slider_one_button_down_start_X,slider_one_control_start_Y);
    webc_ElementSetStyleASCII(demo_control_handles[CONTROL_SLIDER_BUTTON_DOWN].element_handle,ascii_buff,WEBC_TRUE);

    slider_one_button_up_start_X = slider_one_control_start_X + SLIDER_WIDTH + GAP;
    rtp_sprintf(ascii_buff, "position:absolute;left:%d;top:%d",slider_one_button_up_start_X,slider_one_control_start_Y);
    webc_ElementSetStyleASCII(demo_control_handles[CONTROL_SLIDER_BUTTON_UP].element_handle,ascii_buff,WEBC_TRUE);

    /* Set the origin of the first edit control, (it won't change but it's good practice */
    rtp_sprintf(ascii_buff, "position:absolute;left:%d;top:%d",
            slider_one_control_start_X,slider_one_control_start_Y);
    webc_ElementSetStyleASCII(demo_control_handles[CONTROL_SLIDER_TC_ONE].element_handle,ascii_buff,WEBC_TRUE);
    /* Set the size of the first edit control, it will drag open and shut */
    rtp_sprintf(ascii_buff, "width:%dpx;", slider_one_control_size);
    webc_ElementSetStyleASCII(demo_control_handles[CONTROL_SLIDER_TC_ONE].element_handle,ascii_buff,WEBC_TRUE);

    /* Set the origin of the second edit control, it will drag open and shut */
    rtp_sprintf(ascii_buff, "position:absolute;left:%d;top:%d",
            slider_two_control_start_X,slider_two_control_start_Y);
    webc_ElementSetStyleASCII(demo_control_handles[CONTROL_SLIDER_TC_TWO].element_handle,
                                ascii_buff,WEBC_TRUE);
    /* Set the origin of the button to be the same as the origin of the second control */
    webc_ElementSetStyleASCII(demo_control_handles[CONTROL_SLIDER_BUTTON_ONE].element_handle,
                              ascii_buff,WEBC_TRUE);
    /* Set the size of the second edit control, it will drag open and shut */
    rtp_sprintf(ascii_buff, "width:%dpx;", slider_two_control_size);
    webc_ElementSetStyleASCII(demo_control_handles[CONTROL_SLIDER_TC_TWO].element_handle,
                                ascii_buff,WEBC_TRUE);

    rtp_sprintf(ascii_buff, "position:absolute;left:%d;top:%d",
             slider_one_counter_start_X,slider_one_counter_start_Y);
    webc_ElementSetStyleASCII(demo_control_handles[CONTROL_SLIDER_COUNTER_ONE].element_handle,
                                ascii_buff,WEBC_TRUE);

    /* Set the value of counter to the size of first window, which is the control's value */
    rtp_sprintf(ascii_buff, "[%d]", slider_one_control_size);
    webc_ElementSetValueASCII(demo_control_handles[CONTROL_SLIDER_COUNTER_ONE].element_handle,
                                ascii_buff,WEBC_TRUE);
    /* Set the origin of the counter. Above if > midpoint else below */
    {
//        long font_adjustment = (long)webc_ElementGetHeight(demo_control_handles[CONTROL_SLIDER_COUNTER_ONE].element_handle);
        long font_adjustment = (long)webc_ElementGetHeight(demo_control_handles[CONTROL_SLIDER_TC_ONE].element_handle);
        if ((slider_two_control_start_X-slider_one_control_start_X) > SLIDER_WIDTH/2)
        { /* On top */
            slider_one_counter_start_X =  slider_two_control_start_X;
            slider_one_counter_start_Y =  slider_two_control_start_Y - font_adjustment;
        }
        else
        {
            if ((slider_one_control_size - slider_one_counter_size) >= 0)
                slider_one_counter_start_X =  slider_one_control_start_X + slider_one_control_size - slider_one_counter_size;
            else
                slider_one_counter_start_X =  slider_one_control_start_X;
            slider_one_counter_start_Y =  slider_one_control_start_Y + font_adjustment;
        }
    }
}
