#include "webc_ctrl_demo.h"
#include "rtpthrd.h"
/*
webc_ctrl_demo_menu.cpp

    void webc_control_demo_menu(void)
        Demonstrate using multiple webc API calls to implement a drop down menu bar

    This demo:
       . loads html text edit demonstration source code from a character array into the demo frame
            . Extracts HTML Element handles from the document
            . Assigns an event handler to that demonstrates application subclassing of control functions.

*/
/* Menu abstraction
    pparent points to the parent, or 0 if the root
    pchild  points to sub menus if there are any
    pprevsibling points to previous element in current submenu or nul (then go to parent).
    pnextsibling points to next element in current submenu or null if end
    webc_demo_handle: index into webC handle to element mapping. The mapping
    structure contains the html id, and control type and the webC element handle for access and control
    of the element.
    webc_xpos, ypos et al -  these are where the element belongs when shown
    is_shown - if 1 means the element is shown, otherwise it is hidden behind it's parent.
*/

#define MENU_ITEM_WIDTH  80
#define MENU_ITEM_HEIGHT 20
#define MENU_ORIGIN_X 200
#define MENU_ORIGIN_Y 80
int submenu_z_order = 0;
typedef struct sample_menu_element {
    struct sample_menu_element *pparent;
    struct sample_menu_element *pchild;
    struct sample_menu_element *pnextsibling;
    struct sample_menu_element *pprevsibling;
    WEBC_CONTROL_DEMO_HANDLE   *pdemo_handle;
    WEBC_CONTROL_DEMO_HANDLE   *pdiv_handle;
    char   *pvalue;
    char   id[32];
    long   webc_xpos;
    long   webc_ypos;
    long   webc_height;
    long   webc_width;
    int    is_shown;
    int    my_z_order;
} SAMPLE_MENU_ELEMENT;

/* Functions */
static void show_menu(SAMPLE_MENU_ELEMENT *pmenu, WEBC_BOOL display_children);
static void hide_menu(SAMPLE_MENU_ELEMENT *pmenu, WEBC_BOOL hide_children);
static void setup_menu_items(SAMPLE_MENU_ELEMENT *pmenu);
static SAMPLE_MENU_ELEMENT *sample_menu_add_item(SAMPLE_MENU_ELEMENT *parent, SAMPLE_MENU_ELEMENT *prevsibling, char *value);
static void sample_menu_init(void);

/* The type of HTML control we are using for menu items */
#define MENU_HTML_TYPE HTML_EDIT_STR_ELEMENT

SAMPLE_MENU_ELEMENT *top_menus[10];
SAMPLE_MENU_ELEMENT *sub_menus[10][10];
#define NUM_MENU_ELEMENTS 30
/* Set Constant used for gemeric demo code handler */
#define DEMO_CONTROL_COUNT NUM_MENU_ELEMENTS
/* Create a pool of sample webC handle mappig structures */
static SAMPLE_MENU_ELEMENT sample_menu_array[NUM_MENU_ELEMENTS];
/* Generic Demo setup step 2: Declare an array to hold the handles */
static WEBC_CONTROL_DEMO_HANDLE demo_control_handles[NUM_MENU_ELEMENTS*2];

static int control_handles_used;
char * style_show_element    =    "visibility:show;";
char * style_hide_element    =    "visibility:hidden;";
char * style_hilight_element =    "background-color:DeepSkyBlue;color:Black;border:4;";
char * style_select_element  =    "background-color:LightBlue;color:Red;border:none;";
char * style_normal_submenu  =    "background-color:Green;color:Black;border:none;";
char * style_first_child     =    "background-color:LightBlue;color:White;border:none;";
char * style_normal_child    =    "background-color:LightBlue;color:Black;border:none;";
char * style_normal_parent   =    "background-color:DarkGray;color:Black;border:none;";


/* Generic Demo setup step 3: Declare individual callback functions we will use to over-ride some default
   characteristics of individual controls */
static HTMLEventStatus cb_CONTROL_MENU_ITEM (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_CONTROL_BUTTON    (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);

/* Generic Demo setup step 4: Initialize our control handle array html with values to match
  against the "id=" properties in the html and the control type
  Documentation moment: List the types
*/
static void init_demo_control_handles(void)
{
    webc_control_demo_init_control_array(&demo_control_handles[0],DEMO_CONTROL_COUNT);
    /* Call special init function for menu sample */
    sample_menu_init();
    /* Attach handlers for other controls like buttons */
    /* Create the button for display all */
    demo_control_handles[control_handles_used].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[control_handles_used].element_id = "0";
    demo_control_handles[control_handles_used].element_handle = 0;
    demo_control_handles[control_handles_used].cb = cb_CONTROL_BUTTON;
    control_handles_used++;
}

/* Generic Demo setup step 5: Provide an array of html source code that
   will be loaded into the document view of the demo browser when the demo
   is started. */
/* HTML provides a set of hidden text edit controls that we will customize */
static char * demo_control_html = {
"<html>\n\
    <body>\n\
    <br><br><center>Menu Demo move mouse to change hilight, click to drop, undrop or select.\n\
    <br>click show all to see all selections<br></center><br><br>\n\
    <input type=button value=show_all; id=0>\n\
    <div id=0><input type=text style=visibility:hidden; id=0></div>\n\
    <div id=1><input type=text style=visibility:hidden; id=1></div>\n\
    <div id=2><input type=text style=visibility:hidden; id=2></div>\n\
    <div id=3><input type=text style=visibility:hidden; id=3></div>\n\
    <div id=4><input type=text style=visibility:hidden; id=4></div>\n\
    <div id=5><input type=text style=visibility:hidden; id=5></div>\n\
    <div id=6><input type=text style=visibility:hidden; id=6></div>\n\
    <div id=7><input type=text style=visibility:hidden; id=7></div>\n\
    <div id=8><input type=text style=visibility:hidden; id=8></div>\n\
    <div id=9><input type=text style=visibility:hidden; id=9></div>\n\
    <div id=10><input type=text style=visibility:hidden; id=10></div>\n\
    <div id=11><input type=text style=visibility:hidden; id=11></div>\n\
    <div id=12><input type=text style=visibility:hidden; id=12></div>\n\
    <div id=13><input type=text style=visibility:hidden; id=13></div>\n\
    <div id=14><input type=text style=visibility:hidden; id=14></div>\n\
    <div id=15><input type=text style=visibility:hidden; id=15></div>\n\
    <div id=16><input type=text style=visibility:hidden; id=16></div>\n\
    <div id=17><input type=text style=visibility:hidden; id=17></div>\n\
    <div id=18><input type=text style=visibility:hidden; id=18></div>\n\
    <div id=19><input type=text style=visibility:hidden; id=19></div>\n\
 </body>\n\
</html>\n"};

/* Generic Demo setup step 6: Provide a demo entry point. This will be called
   from the dispatcher in webc_ctrl_demo_main.c
    The argument may be
        START_DEMO - To start a demo
        STOP_DEMO  - To stop a demo if necessary
    Most likey only the name of the demo entry point will need to be changed */


#define WEBC_HANDLE(MENU_HANDLE) demo_control_handles[sample_menu_array[(MENU_HANDLE)].webc_demo_handle].element_handle
#define MENUITEM_SETSTYLE(H, SET_STYLE) webc_ElementSetStyleASCII(H,SET_STYLE,WEBC_TRUE)

void webc_control_demo_menu(int command)
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
        if (webc_control_demo_extract_controls(wcdContext.hDemoDoc, &demo_control_handles[0],control_handles_used)
            != control_handles_used)
            return;
        /* Use a demo framework function to install our callback handlers */
        webc_control_demo_set_tag_handlers(wcdContext.hDemoDoc, &demo_control_handles[0], control_handles_used);
        /* We have our handles captured, no customize the generic controls */
        setup_menu_items(top_menus[0]);
        show_menu(top_menus[0], WEBC_FALSE);

    } else if (command == STOP_DEMO)
    {
        /* Put any necesseray stop code here */
    }

}

/* Show all menu items on mouse down, hide on mouse up */
static HTMLEventStatus cb_CONTROL_BUTTON (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
int i;
    switch (event->type)
    {
       case HTML_EVENT_MOUSEDOWN:
            for (i = 9; i >= 0; i--)
            {
                if (top_menus[i])
                    show_menu(top_menus[i], WEBC_TRUE);
            }
        break;
       case HTML_EVENT_MOUSEUP:
            for (i = 0; i < 10; i++)
            {
                if (top_menus[i])
                    hide_menu(top_menus[i], WEBC_TRUE);
            }
       break;
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

static SAMPLE_MENU_ELEMENT *webc_element_to_menu_element(SAMPLE_MENU_ELEMENT *pmenu, HELEMENT_HANDLE hElement);
static HTMLEventStatus cb_CONTROL_MENU_ITEM (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
SAMPLE_MENU_ELEMENT *pmenu,*ptopmenu;

    /* Update generic status fields */
    webc_control_demo_update_info_element(hElement);
    webc_control_demo_update_info_event(event);

    /* Get the menu item associated with this element */
    pmenu = webc_element_to_menu_element(top_menus[0], hElement);
    if (!pmenu)
        return(HTML_EVENT_STATUS_CONTINUE);  /* Should not happen */

    /* Get the menu top menu item that is always visible */
    ptopmenu = pmenu;
    for(;;)
    {
        if (!ptopmenu->pparent)
            break;
        ptopmenu = ptopmenu->pparent;
    }

    /* The switch contains all event types currently sent to the callback */
    switch (event->type)
    {
         case HTML_EVENT_MOUSEDOWN:
         {
            /* Hide children if they are already up */
            if (pmenu->pchild)
            {
                if (pmenu->pchild->is_shown)
                {
                    /* Hide all elements in the child menu under the top */
                    hide_menu(pmenu->pchild,WEBC_TRUE);
                }
                else
                {
                    submenu_z_order += 1;
                    show_menu(pmenu->pchild,WEBC_FALSE);
                }
              }
              else
              { /* It is not a submenu so set the style to selected */

                    MENUITEM_SETSTYLE(hElement, style_select_element);
              }
            }
            return(HTML_EVENT_STATUS_DONE);
            break;
         case HTML_EVENT_CLICK:
            return(HTML_EVENT_STATUS_DONE);
            break;
        case HTML_EVENT_MOUSEUP:
            break;
        case HTML_EVENT_FOCUS:
            return(HTML_EVENT_STATUS_DONE);
        case HTML_EVENT_KEYPRESS:
        case HTML_EVENT_MOUSEMOVE:
                break;
        case HTML_EVENT_MOUSEOVER:
            MENUITEM_SETSTYLE(hElement, style_hilight_element);
            break;
        case HTML_EVENT_KEYDOWN:
            break;
        case HTML_EVENT_KEYUP:
            break;
        case HTML_EVENT_MOUSEOUT:
        case HTML_EVENT_UNFOCUS:
        {
            if (!pmenu->pparent)
                MENUITEM_SETSTYLE(hElement, style_normal_parent);
            else
            {
                if (pmenu->pchild)
                    MENUITEM_SETSTYLE(hElement, style_normal_submenu);
                else
                {
                    if (!pmenu->pprevsibling)
                        MENUITEM_SETSTYLE(hElement, style_first_child);
                    else
                        MENUITEM_SETSTYLE(hElement, style_normal_child);
                }
            }
        }
        break;
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}



static SAMPLE_MENU_ELEMENT *sample_menu_add_item(SAMPLE_MENU_ELEMENT *parent, SAMPLE_MENU_ELEMENT *prevsibling, char *value)
{
    SAMPLE_MENU_ELEMENT *pelement;

	pelement = (SAMPLE_MENU_ELEMENT *) WEBC_MALLOC(sizeof(SAMPLE_MENU_ELEMENT));

    if (parent)
    { /* If there is a parent the previous sibling is the lest element in parent */
        SAMPLE_MENU_ELEMENT *prev = 0;
        if (parent->pchild)
        {
            prev = parent->pchild;
            while (prev->pnextsibling)
            {
                prev = prev->pnextsibling;
            }
            if (prev)
                prev->pnextsibling = pelement;
            pelement->pprevsibling  = prev;
        }
        else
        {
            pelement->pprevsibling  = 0;
            parent->pchild = pelement;
        }
    }
    else
    { /* No parent, creating another root */
        pelement->pprevsibling  = prevsibling;
        if (prevsibling)
            prevsibling->pnextsibling = pelement;
    }
    pelement->pparent       = parent;
    pelement->pchild        = 0;
    pelement->pnextsibling  = 0;
    pelement->is_shown = 0;

    rtp_sprintf(pelement->id,"%d", control_handles_used);
    pelement->pdiv_handle = &demo_control_handles[control_handles_used];
    pelement->pdiv_handle->element_type = HTML_DIV_ELEMENT;
    pelement->pdiv_handle->element_id = pelement->id;
    pelement->pdiv_handle->element_handle = 0;
    pelement->pdiv_handle->cb = 0;

    pelement->pdemo_handle = &demo_control_handles[control_handles_used];
    pelement->pdemo_handle->element_type = MENU_HTML_TYPE;
    pelement->pdemo_handle->element_id = pelement->id;
    pelement->pdemo_handle->element_handle = 0;
    pelement->pdemo_handle->cb = cb_CONTROL_MENU_ITEM;
    control_handles_used++;

    pelement->pvalue = value;
    return(pelement);
}

/* Manually initialize and connect together SAMPLE_MENU_ELEMENT to create a menu */
static void sample_menu_init(void)
{
int TOP_MENU;
    control_handles_used = 0;
    for (int i = 0; i < 10; i++) {top_menus[i]=0;};
    TOP_MENU = 0;
    /* Create the top left item, no parent, no previous siblings */
    top_menus[TOP_MENU] = sample_menu_add_item(0,0, "(0,0)");
    {   /* Create submenu of top left. the top left item, is parent, no previous siblings */
        sub_menus[TOP_MENU][0] = sample_menu_add_item(top_menus[TOP_MENU],0, "(0,1)");
        sub_menus[TOP_MENU][1] = sample_menu_add_item(top_menus[TOP_MENU],0, "(0,2)");
        sub_menus[TOP_MENU][2] = sample_menu_add_item(top_menus[TOP_MENU],0, "(0,3)");
        sub_menus[TOP_MENU][3] = sample_menu_add_item(top_menus[TOP_MENU],0, "(0,4)");
        /* Create submenu of fourth element in top left. fourth element, is parent, no previous siblings */
        sample_menu_add_item(sub_menus[TOP_MENU][3],0,"(0,4,1)");
        sample_menu_add_item(sub_menus[TOP_MENU][3],0, "(0,4,2)");
        sample_menu_add_item(sub_menus[TOP_MENU][3],0, "(0,4,3)");
        sample_menu_add_item(sub_menus[TOP_MENU][3],0, "(0,5)");
    }
    TOP_MENU = 1;
    /* Create second top item, no parent, top left is previous sibling */
    top_menus[TOP_MENU] = sample_menu_add_item(0,top_menus[0], "(1,0)");
    {   /* Create submenu second item. second item is parent, no previous siblings */
        sub_menus[TOP_MENU][0] = sample_menu_add_item(top_menus[TOP_MENU],0, "(1,1)");
        sub_menus[TOP_MENU][1] = sample_menu_add_item(top_menus[TOP_MENU],0, "(1,2)");
        sub_menus[TOP_MENU][2] = sample_menu_add_item(top_menus[TOP_MENU],0, "(1,3)");
        /* Create submenu of third element. third element, is parent, no previous siblings */
        sample_menu_add_item(sub_menus[TOP_MENU][2],0, "(1,3,1)");
        sample_menu_add_item(sub_menus[TOP_MENU][2],0, "(1,3,2)");
        sample_menu_add_item(sub_menus[TOP_MENU][2],0, "(1,3,3)");
        sample_menu_add_item(sub_menus[TOP_MENU][2],0, "(1,3,4)");
        sample_menu_add_item(sub_menus[TOP_MENU][2],0, "(1,3,4)");
        /* Now put an item after the submenu */
        sub_menus[TOP_MENU][3] = sample_menu_add_item(top_menus[TOP_MENU],0, "(1,4)");
    }
}

static void setup_menu_items(SAMPLE_MENU_ELEMENT *pmenu)
{
    submenu_z_order = 0;
    while (pmenu)
    {   /* Set up all ov our values.
                Id is already set, we need to set the contents and the position */
        /* Set the position */
        pmenu->my_z_order = 0;

        pmenu->webc_height = MENU_ITEM_HEIGHT;
        pmenu->webc_width = MENU_ITEM_WIDTH;
        if (!pmenu->pparent)
        {
            if (!pmenu->pprevsibling)
            {  /* The root of the menu */
                pmenu->webc_xpos = MENU_ORIGIN_X;
                pmenu->webc_ypos = MENU_ORIGIN_Y;
            }
            else
            {  /* On the top line of the menu */
                pmenu->webc_xpos = pmenu->pprevsibling->webc_xpos + MENU_ITEM_WIDTH;
                pmenu->webc_ypos = MENU_ORIGIN_Y;
            }
        }
        else
        { /* A child */
            if (pmenu->pprevsibling)
            { /* Put yourself below the previous element in the submenu */
                pmenu->webc_xpos = pmenu->pprevsibling->webc_xpos;
                pmenu->webc_ypos = pmenu->pprevsibling->webc_ypos + MENU_ITEM_HEIGHT;
            }
            else
            { /* Put yourself under or next to your parent */
                if (!pmenu->pparent->pparent)
                {/* Put yourself under your parent */
                    pmenu->webc_xpos = pmenu->pparent->webc_xpos;
                    pmenu->webc_ypos = pmenu->pparent->webc_ypos + MENU_ITEM_HEIGHT;
                }
                else
                {/* Put yourself next to your parent */
                    pmenu->webc_ypos = pmenu->pparent->webc_ypos;
                    pmenu->webc_xpos = pmenu->pparent->webc_xpos + MENU_ITEM_WIDTH;
                }
            }
        }
        /* Set the value */
        webc_ElementSetValueASCII(pmenu->pdemo_handle->element_handle,pmenu->pvalue,WEBC_FALSE);
        /* Set height and width */
// Is not working for some reason
//        webc_ElementSetWidthINT(pmenu->pdemo_handle->element_handle, pmenu->webc_width, WEBC_TRUE);
//        webc_ElementSetHeightINT(pmenu->pdemo_handle->element_handle, pmenu->webc_height, WEBC_TRUE);
        /* Set display attributes */
        if (!pmenu->pparent)
            MENUITEM_SETSTYLE(pmenu->pdemo_handle->element_handle, style_normal_parent);
        else
        {

            if (pmenu->pchild)
                MENUITEM_SETSTYLE(pmenu->pdemo_handle->element_handle, style_normal_submenu);
            else
            {
                if (!pmenu->pprevsibling)
                    MENUITEM_SETSTYLE(pmenu->pdemo_handle->element_handle, style_first_child);
                else
                    MENUITEM_SETSTYLE(pmenu->pdemo_handle->element_handle, style_normal_child);
            }
        }
        /* Set location */
        {
            static char pos_buffer[100];
            rtp_sprintf(pos_buffer, "position:absolute;left:%d;top:%d", pmenu->webc_xpos,pmenu->webc_ypos);
            MENUITEM_SETSTYLE(pmenu->pdiv_handle->element_handle, pos_buffer);
        }
        /* If it is a root menu set Visibility */
        if (!pmenu->pparent)
            MENUITEM_SETSTYLE(pmenu->pdemo_handle->element_handle, style_show_element);
        if (pmenu->pchild)
            setup_menu_items(pmenu->pchild);
        pmenu = pmenu->pnextsibling;
    }
}

/* Looks through the menu tree array for the menu item who's associated demo
   handle structure matches the html elemt we are processing */
static SAMPLE_MENU_ELEMENT *webc_element_to_menu_element(SAMPLE_MENU_ELEMENT *pmenu, HELEMENT_HANDLE hElement)
{
SAMPLE_MENU_ELEMENT *child_result;
    while (pmenu)
    {
        if (hElement == pmenu->pdemo_handle->element_handle)
            return(pmenu);
        else if (pmenu->pchild)
        {
            child_result = webc_element_to_menu_element(pmenu->pchild,hElement);
            if (child_result)
                return (child_result);
        }
        pmenu = pmenu->pnextsibling;
    }
    return(0);
}

static void menu_demo_set_z_order(HELEMENT_HANDLE hElement, int z_order)
{
static char zorder_buffer[100];
    rtp_sprintf(zorder_buffer, "zIndex: %d;", z_order);
    webc_control_demo_update_info_action(zorder_buffer);
    webc_ElementSetStyleASCII(hElement,zorder_buffer,WEBC_TRUE);
}

/* Show a menu, if display_children is set also display children */
static void show_menu(SAMPLE_MENU_ELEMENT *pmenu, WEBC_BOOL display_children)
{
    if (display_children && pmenu->pchild && !pmenu->pchild->is_shown)
    {
        submenu_z_order += 1;
        show_menu(pmenu->pchild,display_children);
    }
    if (!pmenu->is_shown)
    {
        /* Set Visibility */
        MENUITEM_SETSTYLE(pmenu->pdemo_handle->element_handle, style_show_element);
        pmenu->is_shown = 1;
        if (pmenu->pparent)
        { /* Set z order if it is a submenu */
            pmenu->my_z_order = submenu_z_order;
            menu_demo_set_z_order(pmenu->pdiv_handle->element_handle, submenu_z_order);
        }
    }
    if (pmenu->pnextsibling && !pmenu->pnextsibling->is_shown)
        show_menu(pmenu->pnextsibling,display_children);
}

/* Hide a menu, if display_children is set also display children */
static void hide_menu(SAMPLE_MENU_ELEMENT *pmenu, WEBC_BOOL hide_children)
{
    /* Hide children */
    if (hide_children && pmenu->pchild)
        hide_menu(pmenu->pchild, hide_children);
    /* Hide this menu item. If it does not have a parent it's a root so don't hide */
    if (pmenu->pparent && pmenu->is_shown)
    {
        /* If it is the highest zorder drop the zorder by one */
        if (pmenu->my_z_order && pmenu->my_z_order == submenu_z_order)
            submenu_z_order -= 1;
        menu_demo_set_z_order(pmenu->pdiv_handle->element_handle, 0);
        MENUITEM_SETSTYLE(pmenu->pdemo_handle->element_handle, style_hide_element);
        pmenu->is_shown = 0;
    }
    /* Hide siblings */
    if (pmenu->pnextsibling)
        hide_menu(pmenu->pnextsibling, hide_children);
}
