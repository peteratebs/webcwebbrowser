#include "webc_ctrl_demo.h"
/*
webc_ctrl_demo_text.cpp

    void webc_control_demo_text(void)
        Demonstrate using the webc API to access and control HTML elements of type: <input type=text>,
        these types of elements are called edit boxes in certain frameworks.

        The demo uses webc API functions to access and control these elements using
        handles of type HTML_EDIT_STR_ELEMENT.

        HTML_EDIT_STR_ELEMENT controls can be manipulated by the webC API and Events to them can be monitored
        and intercepted by installing application specific event handler for individual elements.


    This demo:
       . loads html text edit demonstration source code from a character array into the demo frame
            . Extracts HTML Element handles from the document
            . Assigns an event handler to that demonstrates application subclassing of control functions.

*/

/* Generic Demo setup step 1: Define several integer handles we will use to access controls */
#define CONTROL_AUTOINCREMENT 0
#define CONTROL_AUTOCORRECT   1
#define CONTROL_DATE_DAY      2
#define CONTROL_DATE_MONTH    3
#define CONTROL_DATE_YEAR     4
#define CONTROL_DRAG          5
#define CONTROL_ROLLOVER      6
#define ROLL_WRAPPER          7
#define DEMO_CONTROL_COUNT    8

/* Generic Demo setup step 2: Declare an array to hold the handles */
static WEBC_CONTROL_DEMO_HANDLE demo_control_handles[DEMO_CONTROL_COUNT];

/* Generic Demo setup step 3: Declare individual callback functions we will use to over-ride some default
   characteristics of individual controls */
static HTMLEventStatus cb_CONTROL_AUTOINCREMENT (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_CONTROL_AUTOCORRECT (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_CONTROL_DATE (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_CONTROL_DRAG (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_CONTROL_ROLLOVER (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);

/* Generic Demo setup step 4: Initialize our control handle array html with values to match
  against the "id=" properties in the html and the control type
  Documentation moment: List the types
*/
static void init_demo_control_handles(void)
{
    webc_control_demo_init_control_array(&demo_control_handles[0],DEMO_CONTROL_COUNT);
    demo_control_handles[CONTROL_AUTOINCREMENT].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_AUTOINCREMENT].element_id = "CONTROL_AUTOINCREMENT";
    demo_control_handles[CONTROL_AUTOINCREMENT].element_handle = 0;
    demo_control_handles[CONTROL_AUTOINCREMENT].cb = cb_CONTROL_AUTOINCREMENT;

    demo_control_handles[CONTROL_AUTOCORRECT].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_AUTOCORRECT].element_id = "CONTROL_AUTOCORRECT";
    demo_control_handles[CONTROL_AUTOCORRECT].element_handle = 0;
    demo_control_handles[CONTROL_AUTOCORRECT].cb = cb_CONTROL_AUTOCORRECT;

    demo_control_handles[CONTROL_DATE_DAY].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_DATE_DAY].element_id = "CONTROL_DATE_DAY";
    demo_control_handles[CONTROL_DATE_DAY].element_handle = 0;
    demo_control_handles[CONTROL_DATE_DAY].cb = cb_CONTROL_DATE;

    demo_control_handles[CONTROL_DATE_MONTH].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_DATE_MONTH].element_id = "CONTROL_DATE_MONTH";
    demo_control_handles[CONTROL_DATE_MONTH].element_handle = 0;
    demo_control_handles[CONTROL_DATE_MONTH].cb = cb_CONTROL_DATE;

    demo_control_handles[CONTROL_DATE_YEAR].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_DATE_YEAR].element_id = "CONTROL_DATE_YEAR";
    demo_control_handles[CONTROL_DATE_YEAR].element_handle = 0;
    demo_control_handles[CONTROL_DATE_YEAR].cb = cb_CONTROL_DATE;

    demo_control_handles[CONTROL_DRAG].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_DRAG].element_id = "CONTROL_DRAG";
    demo_control_handles[CONTROL_DRAG].element_handle = 0;
    demo_control_handles[CONTROL_DRAG].cb = cb_CONTROL_DRAG;

    demo_control_handles[ROLL_WRAPPER].element_type = HTML_TABLE_CELL_ELEMENT;
    demo_control_handles[ROLL_WRAPPER].element_id = "ROLL_WRAPPER";
    demo_control_handles[ROLL_WRAPPER].element_handle = 0;
    demo_control_handles[ROLL_WRAPPER].cb = 0;

    demo_control_handles[CONTROL_ROLLOVER].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[CONTROL_ROLLOVER].element_id = "CONTROL_ROLLOVER";
    demo_control_handles[CONTROL_ROLLOVER].element_handle = 0;
    demo_control_handles[CONTROL_ROLLOVER].cb = cb_CONTROL_ROLLOVER;

}

/* Generic Demo setup step 5: Provide an array of html source code that
   will be loaded into the document view of the demo browser when the demo
   is started. */

static char * demo_control_html = {
"<html>\n\
  <body>\n\
  (id=CONTROL_AUTOINCREMENT) <input type=text id=CONTROL_AUTOINCREMENT><br>\n\
  <b>Press UP and DOWN arrow to Increment and Decrement by ones<br>\n\
  <b>Press + and  - to Increment and Decrement by tens<br>\n\
  (id=CONTROL_AUTOCORRECT) <input type=text id=CONTROL_AUTOCORRECT><br>\n\
  <b>Type lowercase and it will be auto-corrected to upper case<br>\n\
  <input type=text id=CONTROL_DATE_DAY><br>\n\
  (Enter a day. values < 1 or > 31 will be ignored)<br>\n\
  <input type=text id=CONTROL_DATE_MONTH><br>\n\
  (Enter a month. values < 1 or > 12 will be ignored)<br>\n\
  <input type=text id=CONTROL_DATE_YEAR><br>\n\
  (Enter a year. values  < 2000 or > 2020 will be ignored)<br>\n\
  <input type=text value=\"Drag Me\" id=CONTROL_DRAG><br>\n\
  <b>Click and drag to move, or use up arrow and down arrow<br>\n\
  <table><tr><td id=ROLL_WRAPPER bgcolor=blue>\n\
  <input type=text value=\"Roll Over Me\" size=4 id=CONTROL_ROLLOVER><br>\n\
  </td></tr></table>\n\
  <b>Roll over with mouse to change width, click to fix width<br>\n\
 </body>\n\
</html>\n"};

/* Generic Demo setup step 6: Provide a demo entry point. This will be called
   from the dispatcher in webc_ctrl_demo_main.c
    The argument may be
        START_DEMO - To start a demo
        STOP_DEMO  - To stop a demo if necessary
*/

void webc_control_demo_text(int command)
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
   the html provided in static char * demo_control_html = { */

/*    cb_CONTROL_AUTOINCREMENT - Text control with with arrow key conroled increment and decrement */
static HTMLEventStatus cb_CONTROL_AUTOINCREMENT (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
static int control_value = 0;
char control_value_ascii[32];

    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);
    webc_control_demo_int_toascii(control_value_ascii, control_value);
    webc_control_demo_update_info_action(control_value_ascii);

    /* The switch contains all event types currently sent to the callback */
    /* Intercept HTML_EVENT_KEYDOWN on the keys WGK_LNUP and  WGK_LNDNImplement.
       increment or decrement the contents */
    switch (event->type)
    {
        case HTML_EVENT_KEYDOWN:
		    if (event->data.key == WGK_LNUP || event->data.key == WGK_LNDN)
            {
		        if (event->data.key == WGK_LNUP)
		            control_value++;
		        else if (event->data.key == WGK_LNDN)
		            control_value--;
                webc_control_demo_int_toascii(control_value_ascii, control_value);
                webc_ElementSetValueASCII (hElement, control_value_ascii, WEBC_TRUE);
                return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
            }
            /* Demonstrate timed messages by queing multiple up and down keys on + and - */
            else if (event->data.key == '+')
            {
                event->data.key = WGK_LNUP;
                for (int i = 0; i < 8; i++)
                    webc_BrowserQueueTimedEvent (hBrowser,hElement,event,100);
                return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
            }
            else if (event->data.key == '-')
            {
                event->data.key = WGK_LNDN;
                for (int i = 0; i < 8; i++)
                    webc_BrowserQueueTimedEvent (hBrowser,hElement,event,50);
                return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
            }
            break;
        case HTML_EVENT_OBJECT_CREATED:
        /* Initialize the control on create */
            webc_control_demo_int_toascii(control_value_ascii, control_value);
            webc_ElementSetValueASCII (hElement, control_value_ascii, WEBC_TRUE);
        break;
        case HTML_EVENT_FOCUS:
        case HTML_EVENT_CHANGE:
        case HTML_EVENT_KEYUP:
        case HTML_EVENT_KEYPRESS:
        case HTML_EVENT_MOUSEMOVE:
        case HTML_EVENT_MOUSEDOWN:
        case HTML_EVENT_MOUSEUP:
        case HTML_EVENT_CLICK:
        case HTML_EVENT_MOUSEOUT:
        case HTML_EVENT_MOUSEOVER:
        case HTML_EVENT_UNFOCUS:
         break;
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

/*    cb_CONTROL_AUTOCORRECT   - Normal Text control but automatically converts lower to upper case */
static HTMLEventStatus cb_CONTROL_AUTOCORRECT (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    char *current_value;

    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);
    current_value = webc_ElementGetValueASCII(hElement);    /* Get the contents value */
    if (current_value) {
                webc_control_demo_update_info_action(current_value);
                webc_FreeASCIIString(current_value);}
    /* Itercept keydowns with lowercase and replace with upper case */
    switch (event->type)    /* The switch contains all event types currently sent to the callback */
    {
        case HTML_EVENT_KEYDOWN:
		    if (event->data.key >= 'a' && event->data.key <= 'z')
            {
                HTMLEvent my_event;
                /* Clone the event and resend with character set to upper case */
                my_event = *event;
                my_event.data.key = my_event.data.key - 32;
                webc_BrowserDispatchEvent(wcdContext.hBrowser, &my_event);
                /* Stop processing this character */
                return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
            }
            break;
        case HTML_EVENT_CHANGE:
        case HTML_EVENT_KEYUP:
        case HTML_EVENT_KEYPRESS:
        case HTML_EVENT_MOUSEMOVE:
        case HTML_EVENT_MOUSEDOWN:
        case HTML_EVENT_MOUSEUP:
        case HTML_EVENT_CLICK:
        case HTML_EVENT_MOUSEOUT:
        case HTML_EVENT_MOUSEOVER:
        case HTML_EVENT_FOCUS:
        case HTML_EVENT_UNFOCUS:
        break;
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

void webc_ElementEditStr (HELEMENT_HANDLE element);

/*    cb_CONTROL_DATE          - Three field integer control with primitive month day year checking */
static HTMLEventStatus cb_CONTROL_DATE (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
/* day month year */
static int control_value[3] = {21,11,2006};
static int min_control_value[3] = {1,1,2000};
static int max_control_value[3] = {31,12,2020};
char control_value_ascii[32];
int which_date_component;

    /* cb_CONTROL_DATE is hooked to three fileds, figure out which one by our handle */
    if (hElement == demo_control_handles[CONTROL_DATE_DAY].element_handle)
        which_date_component = 0;
    else if (hElement == demo_control_handles[CONTROL_DATE_MONTH].element_handle)
        which_date_component = 1;
    else if (hElement == demo_control_handles[CONTROL_DATE_YEAR].element_handle)
        which_date_component = 2;

    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);
    webc_control_demo_int_toascii(control_value_ascii, control_value[which_date_component]);
    webc_control_demo_update_info_action(control_value_ascii);

    /* Intercept HTML_EVENT_CHANGE - events and restore to the previous
       valid value if the new values is not valid for min and max
       values assigned for that filed
       Intercept HTML_EVENT_KEYDOWN - Don't allow non contol codes that are not
        digits between 0 and 9 */
    switch (event->type)    /* The switch contains all event types currently sent to the callback */
    {
        case HTML_EVENT_CHANGE:
        {
            char *control_contents;
            control_contents = webc_ElementGetValueASCII(hElement);
            if (control_contents)
            {
                int new_value;
                new_value = webc_control_demo_ascii_to_int(control_contents);
                if (new_value >= min_control_value[which_date_component] && new_value <= max_control_value[which_date_component])
                {
                    control_value[which_date_component] = new_value;
                }
                webc_FreeASCIIString(control_contents);
                webc_control_demo_int_toascii(control_value_ascii, control_value[which_date_component]);
                webc_ElementSetValueASCII (hElement, control_value_ascii, WEBC_TRUE);
            }
        }
        break;
        case HTML_EVENT_KEYPRESS:
        case HTML_EVENT_KEYUP:
            break;
        case HTML_EVENT_KEYDOWN:
            /* Allow control codes */
		    if (event->data.key == WGK_CR || event->data.key >= WGK_PGUP)
                return(HTML_EVENT_STATUS_CONTINUE);
            /* Do not allow non digits */
		    else if (event->data.key < '0' || event->data.key > '9')
                return(HTML_EVENT_STATUS_DONE);
            else
                break;
        case HTML_EVENT_OBJECT_CREATED:
            webc_control_demo_int_toascii(control_value_ascii, control_value[which_date_component]);
            webc_ElementSetValueASCII (hElement, control_value_ascii, WEBC_TRUE);
        break;
        case HTML_EVENT_MOUSEMOVE:
            break;
        case HTML_EVENT_MOUSEDOWN:

        case HTML_EVENT_MOUSEUP:
        case HTML_EVENT_CLICK:
        case HTML_EVENT_MOUSEOUT:
        case HTML_EVENT_MOUSEOVER:
        case HTML_EVENT_FOCUS:
        case HTML_EVENT_UNFOCUS:
        break;
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}



/*    cb_CONTROL_DRAG          - Demonstrates moving controls, mouse focus capture and event handling
                               needed to implement drag and drop                                     */

static HTMLEventStatus cb_CONTROL_DRAG (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
static HTMLEvent lastMouseEvent;
static int dragging = 0;
long x_displacement,y_displacement;
    x_displacement = y_displacement = 0;

    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);



    /* Intercept HTML_EVENT_KEYPRESS  - Move the control based on arrow keys
       Intercept HTML_EVENT_MOUSEDOWN - Begin dragging and grab the mouse
       Intercept HTML_EVENT_MOUSEUP   - Drag the control
       Intercept HTML_EVENT_MOUSEMOVE - Stop dragging and release the mouse

       claim and release mouse aren't working
    */
    switch (event->type)    /* The switch contains all event types currently sent to the callback */
    {
        case HTML_EVENT_KEYDOWN:
        case HTML_EVENT_CHANGE:
        case HTML_EVENT_KEYUP:
            break;
        case HTML_EVENT_KEYPRESS:
#define INCREMENT 1
		    if (event->data.key == WGK_LNUP)
                y_displacement = -INCREMENT;
		    else if (event->data.key == WGK_LNDN)
                y_displacement = INCREMENT;
		    else if (event->data.key == WGK_LEFT)
                x_displacement = -INCREMENT;
		    else if (event->data.key == WGK_RIGHT)
                x_displacement = INCREMENT;
            break;
        case HTML_EVENT_CLICK:
            break;
        case HTML_EVENT_MOUSEDOWN:
            lastMouseEvent = *event;
            dragging = 1;
            webc_ElementClaimPointer(hElement);
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
        case HTML_EVENT_MOUSEOUT:
        case HTML_EVENT_MOUSEOVER:
        case HTML_EVENT_UNFOCUS:
        case HTML_EVENT_FOCUS:
        break;
    }

/* If HTML_EVENT_KEYPRESS or HTML_EVENT_MOUSEMOVE initialized x_displacement or
   y_displacement then move the element */
    if (dragging && x_displacement || y_displacement)
    {
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

         my_parent_width  = webc_ElementGetWidth(wcdContext.hDemoFrame);
        my_parent_height  = webc_ElementGetHeight(wcdContext.hDemoFrame);
        webc_ElementGetXYPosition(wcdContext.hDemoFrame, &my_parent_x, &my_parent_y);
#define BORDERWIDTH 4
        /* Get the new position based on current position and deltax and deltay */
        webc_ElementGetXYPosition(hElement, &current_xpos, &current_ypos);
        my_width  = webc_ElementGetWidth(hElement) + BORDERWIDTH;
        my_height  = webc_ElementGetHeight(hElement)+ BORDERWIDTH;
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

/*    cb_CONTROL_ROLLOVER      - Demonstrates the event handling needed to implement rollovers */
/*    Changes the content and size of a text control when it is rolled over */
static HTMLEventStatus cb_CONTROL_ROLLOVER (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
static int in_rollover = 0;
static int control_size = 4;
char control_size_ascii[32];
static HTMLEvent lastMouseEvent;
    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);

    switch (event->type)    /* The switch contains all event types currently sent to the callback */
    {
        case HTML_EVENT_KEYDOWN:
        case HTML_EVENT_CHANGE:
        case HTML_EVENT_KEYUP:
        case HTML_EVENT_KEYPRESS:
        case HTML_EVENT_MOUSEDOWN:
        case HTML_EVENT_MOUSEUP:
            break;
        case HTML_EVENT_MOUSEMOVE:
            if (in_rollover)
            {
                if ((event->data.position.x - lastMouseEvent.data.position.x) > 0)
                {
                    control_size += 1;
                    if (control_size > 40)
                        control_size = 40;
                }
                else if ((event->data.position.x - lastMouseEvent.data.position.x) < 0)
                {
                    control_size -= 1;
                    if (control_size < 4)
                        control_size = 4;
                }
            }
            /* Now change the size of the edit control, it will drag open and shut */
            webc_control_demo_int_toascii(control_size_ascii, control_size);
            webc_ElementSetAttributeASCII (hElement,"size", control_size_ascii);
            lastMouseEvent = *event;
        break;
        case HTML_EVENT_CLICK:
        case HTML_EVENT_MOUSEOUT:
            in_rollover = 0;
            break;
        case HTML_EVENT_MOUSEOVER:
            webc_ElementSetAttributeASCII (hElement,"size", "4");
            webc_ElementSetValueASCII (hElement, "over", WEBC_TRUE);
            lastMouseEvent = *event;
            in_rollover = 1;
        break;
        case HTML_EVENT_UNFOCUS:
         break;
        case HTML_EVENT_FOCUS:
        break;
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}
