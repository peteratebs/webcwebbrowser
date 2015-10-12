#include "webc_ctrl_demo.h"
/*
webc_ctrl_demo_main.cpp

    webc_control_demo_main() is the entry point to a series of subroutines that demonstrate
    using html controls in webC.

    Each demo is in a seperate file and each file contains the following:
        . html source code in string arrays for the html control being demonstrated.
        . "C" code that.
            . loads the html from a character array into the browser contained within a demo frame
            . Extracts necessary HTML Element handles from the document
            . Assigns Event handlers to the correct element
            . Updates status and message flow information into text boxes in an information frame.
            . Implements algorithms that demonstrate how use webC event capture and API calls to perform
              common UI application development tasks.

              The following demos are incuded in other files:

              void webc_control_demo_text(int command)
              void webc_control_demo_checkbox(int command)
              void webc_control_demo_radio_button(int command)
              void webc_control_demo_button(int command)
              void webc_control_demo_resetandsubmit(int command)
              void webc_control_demo_select(int command)
              void webc_control_demo_textarea(int command)
              void webc_control_demo_password(int command)
              void webc_control_demo_menu(int command)
              void webc_control_demo_autohelp(int command)
              void webc_control_demo_atmdemo(int command)
              void webc_control_demo_viewer(int command)
              void webc_control_demo_color_blender(int command)
              void webc_control_demo_image(int command)
              void webc_control_demo_sliders(int command)
              void webc_control_demo_autohelp(int command)

    webc_control_demo_main() in this file is the entry point to the control demo. Functions in
    this file perform the following tasks.

        . Loads the html source from an array that subdivides the browser window into
          frames.
          . one for displaying status information
          . one for executing the control demos in

        . Extracts necessary HTML Element handles from main document view
            . The demo and info HTML Frame element handles.
            . The seperate HTML Document handles of the two frames.

        . Loads the html source from an array for the information frame. This HTML supplies:
            . several text boxes that the demos will use for displaying status information
            . an HTML select control (dropbox) for selecting which demo routine to execute.

        . Extracts necessary HTML Element handles from information frame document
            . Handles to readonly edit boxes to use for displaying status information.
            . A handle to the HTML "select" control with id=which_control

        . Installs an event handler on the select control named cb_demo_TestSelector ()
            This handler intercepts HTML_CHANGED events sent to the select control when a selection
            is made. It calls:
                webc_ElementGetValueASCII();
            to retrieve the new selection and based on that value it calls routines in other files
            that demonstrate programming individual webC controls.

        . This file also contains several utility function that are used by the demos.
           . webc_control_demo_load_string() - This function loads a character array containing
             html source code into an HTML document view and refreshes the frame.
               Uses the following webC API calls:
                webc_DocClear(),webc_DocOpen(),DocWriteHtml(),DocClose(),DocRefresh()

           . control_demo_update_info_element() - This function extracts information about
             an element handle passed to it and updates text boxes in the information view
             based on it's findings:
               Uses the following webC API calls:
                webc_ElementGetType(), webc_ElementGetIdASCII(), webc_ElementGetNameASCII(),
                webc_ElementSetValueASCII(),webc_FreeASCIIString()

           . webc_control_demo_update_info_event()  - This function extracts information about
             an event structure passed to it and updates text boxes in the information view
             based on it's findings. It dispalys the event ID certain event parameters, like key values.
           . webc_control_demo_update_info_action()
           . webc_control_demo_update_info_demoname()
               . These functions use webc_ElementSetValueASCII() to update their respective text boxes
                 in the information frame.
*/

static int webc_control_demo_main(HBROWSER_HANDLE Browser);
static HTMLEventStatus webcDemoEventHandler (HBROWSER_HANDLE hbrowser,	HTMLBrowserEvent* event);
void webc_control_demo_hide_viewframe(void);

HBROWSER_HANDLE webc_start_webc_demo(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow)
{
HBROWSER_HANDLE Browser;


//    These styleseare done in the browser layer
//    if (pconfig->has_status)
//        dwStyle |= WS_XXXX;
//    if (pconfig->has_location)
//        dwStyle |= WS_XXXX;
//    if (pconfig->has_toolbar)
//        dwStyle |= WS_XXXX;
//    if (pconfig->has_scrollbars_enabled)
//        dwStyle |= WS_XXXX;

  //  pconfig->WindowTitle = L"WebC 2.6 C api Demo";
  //  pconfig->is_resizable = WEBC_TRUE;
    /* User supplied parameters */
    pconfig->WindowConfig.top = 0;
    pconfig->WindowConfig.left = 0;
    pconfig->WindowConfig.width = 640;
    pconfig->WindowConfig.height = 480;
	pconfig->browserEventHandler = webcDemoEventHandler;
	pconfig->privateData = 0;

	Browser = webc_CreateBrowser(pconfig, CreateNewWindow,"Demo", "");
    if (Browser)
    {
	    webc_control_demo_main(Browser);
    }

    return(Browser);
}

static HTMLEventStatus webcDemoEventHandler (
        HBROWSER_HANDLE hbrowser,
		HTMLBrowserEvent* event
	)
{
	switch (event->type)
	{
		case HTML_BROWSER_EVENT_UNKNOWN_CONTENT_TYPE:
		case HTML_BROWSER_EVENT_STATUS_CHANGE:
			/* event->data.string contains new status */
			break;
		case HTML_BROWSER_EVENT_LOAD_START:
			break;
		case HTML_BROWSER_EVENT_LOAD_DONE:
			break;
		case HTML_BROWSER_EVENT_DOCUMENT_TITLE_CHANGE:
			/* event->data.string contains the new title */
			break;
		case HTML_BROWSER_EVENT_DOCUMENT_URL_CHANGE:
			/* event->data.string contains new url */
			break;
		case HTML_BROWSER_EVENT_HTTP_NOT_AUTHORIZED:
		case HTML_BROWSER_EVENT_HTTP_AUTHORIZATION_REJECTED:
			break;
		case HTML_BROWSER_EVENT_BLANK_DOCUMENT_REQUESTED:
			break;
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}


char * webc_control_demo_main_html =
"<html>\n\
<TITLE>WebC HTML Control Programming Examples</TITLE>\n\
<body style=background-color:DeepSkyBlue;>\n\
<center><h1> The Outer Frame </h1></center>\n\
  <iframe style=\"position:absolute;left:40;top:60\" id=\"info_frame\" title=\"Demo Progress\" width=\"640\" height=\"140\" frameborder=\"1\">\n\
  </iframe>\n\
  <iframe style=\"position:absolute;left:40;top:220\" id=\"demo_frame\" title=\"Demo Code\" width=\"640\" height=\"400\" frameborder=\"1\">\n\
  </iframe>\n\
  <iframe style=\"position:absolute;left:40;top:220\" id=\"viewer_frame\" title=\"Viewer Frame\" width=\"1\" height=\"1\" frameborder=\"1\">\n\
  </iframe>\n\
</body>\n\
</html>\n";

static long info_topY    = 60;
WEBC_CONTROL_DEMO_GLOBALS wcdContext;

/* html source to be loaded ito the info_frame
    Contains text elements for status display
      id=demo_name,  id=element_type, id=element_id, id=element_name,
      id=element_action, id=event_name, id=event_value.
     After loading handles to these controls are extracted and are saved
     in a structure that status update routines use to change the contents of
     the text elements.
    Contains an HTML select element (dropbox) with id="which_control"
        An event handler is estabished for this element. The handler
        traps the control's HTML_CHANGE event and call the approprite
        subroutine.
*/

char * webc_control_demo_info_html = {
"<html>\n\
<TITLE>WebC HTML Control Programming Info Frame</TITLE>\n\
<body>\n\
Demo Name :<input disabled=\"disabled\" type=\"text\" size=30 id=\"demo_name\">\n\
<br>Element Type: <input disabled=\"disabled\" type=\"text\" size=30 id=\"element_type\">\n\
ID: <input disabled=\"disabled\" type=\"text\" size=30 id=\"element_id\">\n\
Name: <input disabled=\"disabled\" type=\"text\" size=10 id=\"element_name\">\n\
<br>Action: <input disabled=\"disabled\" type=\"text\" size=80 id=\"element_action\">\n\
Event Name: <input disabled=\"disabled\" type=\"text\" size=30 id=\"event_name\">\n\
Value: <input disabled=\"disabled\" type=\"text\" size=10 id=\"event_value\">\n\
<br> Basic controls:\n\
<select id=\"which_control\"\n\
 <option value=\"webc_control_demo_text\"> webc_control_demo_text()\n\
 <option value=\"webc_control_demo_checkbox\"> webc_control_demo_checkbox()\n\
 <option value=\"webc_control_demo_radiobutton\"> webc_control_demo_radiobutton()\n\
 <option value=\"webc_control_demo_button\"> webc_control_demo_button()\n\
 <option value=\"webc_control_demo_reset\"> webc_control_demo_reset()\n\
 <option value=\"webc_control_demo_submit\"> webc_control_demo_submit()\n\
 <option value=\"webc_control_demo_image\"> webc_control_demo_image()\n\
 <option value=\"webc_control_demo_password\"> webc_control_demo_password()\n\
 <option value=\"webc_control_demo_select\"> webc_control_demo_select()\n\
 <option value=\"webc_control_demo_textarea\"> webc_control_demo_textarea()\n\
 </select>\n\
Custom Controls:\n\
<select id=\"which_custom\"\n\
<option value=\"webc_control_demo_menu\"> webc_control_demo_menu()\n\
<option value=\"webc_control_demo_color_blender\"> webc_control_demo_colorblnder()\n\
<option value=\"webc_control_demo_autohelp\"> webc_control_demo_autohelp()\n\
<option value=\"webc_control_demo_sliders\"> webc_control_demo_sliders()\n\
<option value=\"webc_control_demo_atmdemo\"> webc_control_demo_atmdemo()\n\
<option value=\"webc_control_demo_viewer\"> webc_control_demo_viewer()\n\
</select>\n\
</html>\n"};

static char * demo_control_html_strings[] = {
"<html>",
"<body>",
"<br><center><u>Using the webC API with HTML controls to create basic and advanced widgits.</u><br></center>",
"<br><b>Click on the other Window to use an Internet Browser Application created with webC.</b><br>",
"<br>&nbsp;<u>Select from either the basic examples on the left</u>",
"<br>&nbsp;&nbsp;&nbsp;webc_control_demo_checkbox , webc_control_demo_radiobutton webc_control_demo_button",
"<br>&nbsp;&nbsp;&nbsp;webc_control_demo_reset , webc_control_demo_submit, webc_control_demo_password",
"<br>&nbsp;&nbsp;&nbsp;webc_control_demo_select , webc_control_demo_textarea",
"<br>&nbsp;<u>Or from more eleborate examples on the left:</u>",
"<br>&nbsp;&nbsp;&nbsp;webc_control_demo_text   - Simple date controls and input filtering",
"<br>&nbsp;&nbsp;&nbsp;webc_control_demo_image  - Image drag and drop",
"<br>&nbsp;<u>Or from the more advanced group on the right:</u>",
"<br>&nbsp;&nbsp;&nbsp;webc_control_demo_menu     - Dropdown menus",
"<br>&nbsp;&nbsp;&nbsp;webc_control_demo_autohelp - Rollover help",
"<br>&nbsp;&nbsp;&nbsp;webc_control_demo_sliders  - A custom slider control",
"<br>&nbsp;&nbsp;&nbsp;webc_control_demo_atmdemo  - A simulated Automatic Teller Machine",
"<br>&nbsp;&nbsp;&nbsp;webbc_control_demo_viewer  - A File system explorer and content viewer",
"</body>",
"</html>",
0
};


/*  An event handler to trap HTML_CHANGE events to the HTML select element */
static HTMLEventStatus cb_demo_TestSelector (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
/* Main demo entry point */
static int webc_control_demo_main(HBROWSER_HANDLE Browser)
{
    /* Get the root document from the browser. HTML code is loaded into the
       document view and individual elements are accessed from the tree of
       elements contained withing the document view */
    wcdContext.hBrowser = Browser;

    wcdContext.hRootDocument = webc_BrowserGetDocument(wcdContext.hBrowser);
    if (!wcdContext.hRootDocument)
        return(-1);
    /* Call a helper function to load the root page, which breaks the browser into frames */
    if (webc_control_demo_load_string(wcdContext.hRootDocument, webc_control_demo_main_html) < 0)
    	{
        return(-1);
    	}
    /* Extract the handles of the sub frames, using the id attribute of the frames
       to match the HTML frame elements with internal C handles. */
    {
    int which_frame=0;
    HELEMENT_HANDLE hFrameElement;
        do
        {
            hFrameElement = webc_DocFindElement (wcdContext.hRootDocument, 0, 0, HTML_IFRAME_ELEMENT, which_frame);
            if (!hFrameElement)
                break;
            if (webc_ElementIdMatches(hFrameElement, "info_frame"))
                wcdContext.hInfoFrame = hFrameElement;
            if (webc_ElementIdMatches(hFrameElement, "demo_frame"))
                wcdContext.hDemoFrame = hFrameElement;
            if (webc_ElementIdMatches(hFrameElement, "viewer_frame"))
                wcdContext.hViewerFrame = hFrameElement;
            which_frame += 1;
        } while(hFrameElement);
    }
    if (!wcdContext.hDemoFrame || !wcdContext.hInfoFrame || !wcdContext.hViewerFrame)
        return(-1);
    wcdContext.demo_topY = 220;
    wcdContext.demo_leftX = 40;
    wcdContext.demo_heightY = 400;
    wcdContext.demo_widthX = 640;

    /* Extract the document handles of the frames. Each frame has it's
       own document view and acts as a sub browser. The Document
       handle is uase to load the frame's html and to access elements of the
       frame. */
    wcdContext.hInfoDoc = webc_ElementGetFrameDocument(wcdContext.hInfoFrame);
    wcdContext.hDemoDoc = webc_ElementGetFrameDocument(wcdContext.hDemoFrame);
    wcdContext.hViewerDoc = webc_ElementGetFrameDocument(wcdContext.hViewerFrame);
    if (!wcdContext.hInfoDoc || !wcdContext.hDemoDoc || !wcdContext.hViewerDoc)
        return(-1);
    /* Load the Info frame with controls to display status and control the demo */
    if (webc_control_demo_load_string(wcdContext.hInfoDoc, webc_control_demo_info_html) < 0)
        return(-1);

    /* Extract handles of text prompts in the info frame. Save them in a structure to be
       used when updating individual status fields in the info frame */
    {
    int which_string=0;
    HELEMENT_HANDLE hElement;
        do
        {
            hElement = webc_DocFindElement (wcdContext.hInfoDoc, 0, 0, HTML_EDIT_STR_ELEMENT, which_string);
            if (!hElement)
                break;
            if (webc_ElementIdMatches(hElement, "demo_name"))
                wcdContext.hInfoDemoName = hElement;
            else if (webc_ElementIdMatches(hElement, "element_type"))
                wcdContext.hInfoType = hElement;
            else if (webc_ElementIdMatches(hElement, "element_id"))
                wcdContext.hInfoId = hElement;
            else if (webc_ElementIdMatches(hElement, "element_name"))
                wcdContext.hInfoName = hElement;
            else if (webc_ElementIdMatches(hElement, "element_action"))
                wcdContext.hInfoAction = hElement;
            else if (webc_ElementIdMatches(hElement, "event_name"))
                wcdContext.hInfoEventName = hElement;
            else if (webc_ElementIdMatches(hElement, "event_value"))
                wcdContext.hInfoEventValue = hElement;
            which_string += 1;
        } while(hElement);
    }
    if (!wcdContext.hInfoType ||
        !wcdContext.hInfoId   ||
        !wcdContext.hInfoName ||
        !wcdContext.hInfoAction||
        !wcdContext.hInfoEventName ||
        !wcdContext.hInfoDemoName ||
        !wcdContext.hInfoEventValue )
        return(-1);

    /* Extract the handle of the HTML select control (DropBox) that will control our tests */
    wcdContext.hBasicControlSelector = webc_DocFindElement (wcdContext.hInfoDoc, 0, 0, HTML_SELECT_ELEMENT, 0);
    if (!wcdContext.hBasicControlSelector)
        return(-1);
    wcdContext.hCustomControlSelector = webc_DocFindElement (wcdContext.hInfoDoc, 0, 0, HTML_SELECT_ELEMENT, 1);
    if (!wcdContext.hCustomControlSelector)
        return(-1);

    /* Assing an event handler to the select control that intercepts HTML_CHANGE events and
       calls functions that control individual routines */
    webc_ElementSetTagEventHandler (wcdContext.hBasicControlSelector, cb_demo_TestSelector);
    webc_ElementSetTagEventHandler (wcdContext.hCustomControlSelector, cb_demo_TestSelector);
   /* Update status boxes in the infor fielde */

   webc_control_demo_update_info_demoname("Control Demo Initialized");
    //webc_control_demo_update_info_element(wcdContext.hDemoFrame);
   webc_control_demo_update_info_action("Please select a control demo");

   /* Load the welcome window */
   webc_control_demo_load_array(wcdContext.hDemoDoc, demo_control_html_strings);

    return(0);
}

void webc_control_not_implented () {webc_control_demo_update_info_action("Not implemented yet"); }
void webc_control_demo_text(int command);  /* See webc_ctrl_demo_text.cpp */
void webc_control_demo_checkbox(int command); /* See webc_ctrl_demo_checkbox.cpp */
void webc_control_demo_radio_button(int command); /* See webc_ctrl_demo_radiobuttoninfor fielde */
void webc_control_demo_button(int command); /* See webc_ctrl_demo_radiobutton.cpp */
void webc_control_demo_resetandsubmit(int command); /* See webc_ctrl_demo_resetsubmit.cpp */
void webc_control_demo_select(int command); /* See webc_ctrl_demo_select.cpp */
void webc_control_demo_textarea(int command);/* See webc_ctrl_demo_textarea.cpp */
void webc_control_demo_password(int command);/* See webc_ctrl_demo_password.cpp */
void webc_control_demo_menu(int command); /* See webc_ctrl_demo_menu.cpfielde *;
void webc_control_demo_autohelp(int command); /* See webc_ctrl_demo_autohelp.cpp */
void webc_control_demo_atmdemo(int command); /* See webc_ctrl_demo_atmdemo.cpp */
void webc_control_demo_viewer(int command); /* See webc_ctrl_demo_viewer.cpp */
void webc_control_demo_color_blender(int command);
void webc_control_demo_image(int command);
void webc_control_demo_sliders(int command);
void webc_control_demo_autohelp(int command);


/* Event handler to the wcdContext.hBasicControlSelector select control. intercepts HTML_CHANGE events and calls
   demo functions based on the content of the select control cotnrol */
static void webc_control_demo_tmo_animate_fullscreen(char which_direction);
void webc_control_demo_hide_viewframe(void);

static HTMLEventStatus cb_demo_TestSelector (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    /* Allow our exapander widgit to epand when it sends itself + and - keys */
    if (event->type == HTML_EVENT_KEYDOWN)
    {
        webc_control_demo_tmo_animate_fullscreen((char)event->data.key);
        return(HTML_EVENT_STATUS_DONE);
    }

    if (event->type == HTML_EVENT_CHANGE)
    {
    char *testName;
        webc_control_demo_hide_viewframe();
        testName = webc_ElementGetValueASCII(hElement);
        if (testName)
        {
            /* Update the demo name field */
            webc_control_demo_update_info_demoname(testName);
            /* Clear demo callback event tracer */
            webc_demo_event_log_clear();
           if (rtp_strcmp(testName, "webc_control_demo_button") == 0)
               webc_control_demo_button(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_checkbox") == 0)
               webc_control_demo_checkbox(START_DEMO);
           // else if (rtp_strcmp(testName, "webc_control_demo_hidden") == 0)
           //    webc_control_demo_hidden(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_image") == 0)
               webc_control_demo_image(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_password") == 0)
               webc_control_demo_password(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_radiobutton") == 0)
               webc_control_demo_radio_button(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_reset") == 0)
               webc_control_demo_resetandsubmit(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_submit") == 0)
               webc_control_demo_resetandsubmit(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_text") == 0)
               webc_control_demo_text(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_select") == 0)
               webc_control_demo_select(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_textarea") == 0)
               webc_control_demo_textarea(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_menu") == 0)
               webc_control_demo_menu(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_color_blender") == 0)
               webc_control_demo_color_blender(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_sliders") == 0)
               webc_control_demo_sliders(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_autohelp") == 0)
               webc_control_demo_autohelp(START_DEMO);
           else if (rtp_strcmp(testName, "webc_control_demo_atmdemo") == 0)
               webc_control_demo_atmdemo(START_DEMO);
            else if (rtp_strcmp(testName, "webc_control_demo_viewer") == 0)
               webc_control_demo_viewer(START_DEMO);
//           else if (rtp_strcmp(testName, "webc_control_demo_inputbutton") == 0)
//               webc_control_demo_inputbutton(START_DEMO);
            webc_FreeASCIIString(testName);
        }
        return(HTML_EVENT_STATUS_DONE);
    }
    return(HTML_EVENT_STATUS_CONTINUE);
}

/* Utility function that expands the demo frame */
void webc_control_demo_animate_fullscreen(char which_direction)
{
HTMLEvent event;
    /* Start the process by sending a plus to the control selector. We'll just use that callback because
       it's already there and it doesn't need the plus our minus */
    event.type = HTML_EVENT_KEYDOWN;
    event.data.key = which_direction;
    webc_BrowserQueueTimedEvent (wcdContext.hBrowser, wcdContext.hBasicControlSelector,&event,50);
}

static void webc_control_demo_tmo_animate_fullscreen(char which_direction)
{
#define INCREMENT 20
    if (which_direction == '+')
    {
        if ((wcdContext.demo_topY-INCREMENT) <info_topY)
            return;
        wcdContext.demo_topY -= INCREMENT;
        wcdContext.demo_heightY += INCREMENT;
    }
    else if (which_direction == '-')
    {
        if ((wcdContext.demo_heightY - INCREMENT) < 400)
            return;
        wcdContext.demo_topY += INCREMENT;
        wcdContext.demo_heightY -= INCREMENT;
    }
    else
        return;
    /* Redraw */
    {
    char ascii_buff[64];
    rtp_sprintf(ascii_buff, "position:absolute;left:%d;top:%d", wcdContext.demo_leftX,wcdContext.demo_topY);
    webc_ElementSetStyleASCII(wcdContext.hDemoFrame, ascii_buff,WEBC_FALSE);
    rtp_sprintf(ascii_buff, "width:%dpx;", wcdContext.demo_widthX);
    webc_ElementSetStyleASCII(wcdContext.hDemoFrame, ascii_buff,WEBC_FALSE);
    rtp_sprintf(ascii_buff, "height:%dpx;", wcdContext.demo_heightY);
    webc_ElementSetStyleASCII(wcdContext.hDemoFrame, ascii_buff,WEBC_TRUE);
    /* Queue up another one */
    webc_control_demo_animate_fullscreen(which_direction);
    }
}

void webc_control_demo_draw_viewframe(long Xpos, long Ypos)
{
char ascii_buff[64];


    rtp_sprintf(ascii_buff, "position:absolute;left:%d;top:%d", wcdContext.demo_leftX,wcdContext.demo_topY);
    webc_ElementSetStyleASCII(wcdContext.hDemoFrame, ascii_buff,WEBC_FALSE);
    rtp_sprintf(ascii_buff, "width:%dpx;", Xpos-4);
    webc_ElementSetStyleASCII(wcdContext.hDemoFrame, ascii_buff,WEBC_FALSE);
    rtp_sprintf(ascii_buff, "height:%dpx;", wcdContext.demo_heightY);
    webc_ElementSetStyleASCII(wcdContext.hDemoFrame, ascii_buff,WEBC_TRUE);


    rtp_sprintf(ascii_buff, "position:absolute;left:%d;top:%d", wcdContext.demo_leftX+Xpos,wcdContext.demo_topY);
    webc_ElementSetStyleASCII(wcdContext.hViewerFrame, ascii_buff,WEBC_FALSE);
    rtp_sprintf(ascii_buff, "width:%dpx;", wcdContext.demo_widthX-Xpos); /* Leave room for scrollbar */
    webc_ElementSetStyleASCII(wcdContext.hViewerFrame, ascii_buff,WEBC_FALSE);
    rtp_sprintf(ascii_buff, "height:%dpx;", wcdContext.demo_heightY);
    webc_ElementSetStyleASCII(wcdContext.hViewerFrame, ascii_buff,WEBC_TRUE);
    wcdContext.viewer_is_visible = 1;
}

void webc_control_demo_hide_viewframe(void)
{
char ascii_buff[64];
    if (!wcdContext.viewer_is_visible)
        return;

    rtp_sprintf(ascii_buff, "position:absolute;left:%d;top:%d", 40,220);
    webc_ElementSetStyleASCII(wcdContext.hViewerFrame, ascii_buff,WEBC_FALSE);
    rtp_sprintf(ascii_buff, "width:%dpx;", 1);
    webc_ElementSetStyleASCII(wcdContext.hViewerFrame, ascii_buff,WEBC_FALSE);
    rtp_sprintf(ascii_buff, "height:%dpx;", 1);
    webc_ElementSetStyleASCII(wcdContext.hViewerFrame, ascii_buff,WEBC_TRUE);
    wcdContext.viewer_is_visible = 0;

    rtp_sprintf(ascii_buff, "position:absolute;left:%d;top:%d", wcdContext.demo_leftX,wcdContext.demo_topY);
    webc_ElementSetStyleASCII(wcdContext.hDemoFrame, ascii_buff,WEBC_FALSE);
    rtp_sprintf(ascii_buff, "width:%dpx;", wcdContext.demo_widthX);
    webc_ElementSetStyleASCII(wcdContext.hDemoFrame, ascii_buff,WEBC_FALSE);
    rtp_sprintf(ascii_buff, "height:%dpx;", wcdContext.demo_heightY);
    webc_ElementSetStyleASCII(wcdContext.hDemoFrame, ascii_buff,WEBC_TRUE);
}

/* Utility function that displays the event type in the information frame */
void webc_control_demo_update_info_event(HTMLEvent* event)
{
    webc_ElementSetValueASCII (wcdContext.hInfoEventName,
                                GetEventName(event), WEBC_TRUE);
}


/* Utility function that determines the type and, if they are assigned, the name
   and id of an element handle and displays them in test edit control in the
   information frame */
void webc_control_demo_update_info_element(HELEMENT_HANDLE theElement)
{
char *myType, *myId, *myName;
    myType = ElementTypeToString(webc_ElementGetType(theElement));
    myId = webc_ElementGetIdASCII(theElement);
    myName = webc_ElementGetNameASCII(theElement);

    /* always has a Type */
    webc_ElementSetValueASCII (wcdContext.hInfoType, myType, WEBC_TRUE);

    if (myId)
        webc_ElementSetValueASCII (wcdContext.hInfoId, myId, WEBC_TRUE);
    if (myName)
        webc_ElementSetValueASCII (wcdContext.hInfoName, myName, WEBC_TRUE);
    if (myId)
        webc_FreeASCIIString(myId);
    if (myName)
        webc_FreeASCIIString(myName);
}
/* Utility function that displays a text string in the status edit control in the
   information frame */
void webc_control_demo_update_info_action(char *theText)
{
    webc_ElementSetValueASCII (wcdContext.hInfoAction, theText, WEBC_TRUE);
}
/* Utility function that displays a text string in the current control demo name
   in the demo name edit control in the information frame */
void webc_control_demo_update_info_demoname(char *theText)
{
    webc_ElementSetValueASCII (wcdContext.hInfoDemoName, theText, WEBC_TRUE);
}



/* Utility function that reload the HTML source code of a document from an
   array of chars. After loading the document the display is refreshed */
int webc_control_demo_load_string(HDOC_HANDLE hDoc, char *html_src)
{
    long written, html_src_length;
    html_src_length = (long) rtp_strlen(html_src) + 1;;
    /* First clear it , don't refresh */
    webc_DocClear(hDoc, WEBC_FALSE);
    webc_DocOpen(hDoc);
    written = webc_DocWriteHtml(hDoc, html_src, html_src_length, WEBC_FALSE);
    if (written < 0)
        return(-1);
    webc_DocClose(hDoc);
    webc_DocRefresh(hDoc);
    return(0);
}

/* Utility function that reload the HTML source code of a document from an
   array of strings. After loading the document the display is refreshed */
int webc_control_demo_load_array(HDOC_HANDLE hDoc, char **html_src)
{
    long written, html_src_length;
    /* First clear it , don't refresh */
    webc_DocClear(hDoc, WEBC_FALSE);
    webc_DocOpen(hDoc);
    while (*html_src)
    {
        html_src_length = (long) rtp_strlen(*html_src) + 1;;
        written = webc_DocWriteHtml(hDoc, *html_src, html_src_length, WEBC_FALSE);
        if (written < 0)
        {
            webc_DocClose(hDoc);
            return(-1);
        }
        html_src++;
    }
    webc_DocClose(hDoc);
    webc_DocRefresh(hDoc);
    return(0);
}


/*   void webc_control_demo_init_control_array(WEBC_CONTROL_DEMO_HANDLE *phandle, int max_handles)

    This helper function clears an array of WEBC_CONTROL_DEMO_HANDLE structures and should be
    called before calling webc_control_demo_extract_controls().
*/
void webc_control_demo_init_control_array(WEBC_CONTROL_DEMO_HANDLE *phandle, int max_handles)
{
int i;
    for (i = 0; i < max_handles; i++)
    {
       phandle->element_handle = 0;
       phandle->cb = 0;
       phandle->element_type = HTML_ELEMENT_NONE;
       phandle->element_id = 0;
       phandle++;
    }
}

/*  int webc_control_demo_extract_controls()

    This helper function takes a document handle and an array of WEBC_CONTROL_DEMO_HANDLE structures
    and finds the webC handle for each request in the list.

    Before calling this function you must zero the array and inititialize the element_type and element_id
    fields of each structure in the array. max_handles must contain the number of structures in the array */

/* Scan for every type of type , and then by id this is a little inefficient and easy to rewrite */
static int extract_elements_of_type_by_id(HDOC_HANDLE hDoc, HTMLElementType the_element_type, WEBC_CONTROL_DEMO_HANDLE *phandle, int max_handles);
int webc_control_demo_extract_controls(HDOC_HANDLE hDoc, WEBC_CONTROL_DEMO_HANDLE *phandle, int max_handles)
{
int num_handles;
WEBC_CONTROL_DEMO_HANDLE *ptemphandle;
int types_in_form[NUM_HTML_ELEMENT_TYPES];

    for (int i = 0; i < NUM_HTML_ELEMENT_TYPES;i++)
        types_in_form[i] = 0;
    ptemphandle = phandle;
    for (int i = 0; i < max_handles;i++,ptemphandle++)
        types_in_form[ptemphandle->element_type] = 1;

    num_handles = 0;
    for (int i=(int)HTML_ADDRESS_ELEMENT; i < (int)NUM_HTML_ELEMENT_TYPES; i++)
    {
    HTMLElementType this_type = (HTMLElementType) i;
        if (types_in_form[this_type])
            num_handles += extract_elements_of_type_by_id(hDoc,this_type,phandle, max_handles);
    }
    return(num_handles);
}

/*   webc_control_demo_int_toascii()
    This demo framework helper function uses spirntf to convert an int to a string
*/
void webc_control_demo_int_toascii(char *toascii, int fromint)
{
    rtp_sprintf(toascii, "%d", fromint);
}

/*  webc_control_demo_ascii_to_int()
    This demo framework helper function uses sscanf to convert a string to an int
*/
int webc_control_demo_ascii_to_int(char *fromascii)
{
int myint;
    sscanf(fromascii,"%d", &myint);
    return(myint);
}


/*   webc_control_demo_set_tag_handlers()

    This demo framework helper function takes an array of WEBC_CONTROL_DEMO_HANDLE structures and
    assigns the callback function addresses contained in the array to the thml element in the document

    Call webc_control_demo_extract_controls before calling this function
*/


void webc_control_demo_set_tag_handlers(HDOC_HANDLE hDoc, WEBC_CONTROL_DEMO_HANDLE *phandle, int max_handles)
{
    int i;
    for (i = 0; i < max_handles; i++)
    {
        if (phandle->element_handle && phandle->cb)
            webc_ElementSetTagEventHandler (phandle->element_handle, phandle->cb);
        phandle++;
    }
}

/* Attempt to map all element_type:element_id pairs in the array of mapping structures to handles of html
   elementsd in the supplied document handle.

   return the count of matches
*/
static int extract_elements_of_type_by_id(HDOC_HANDLE hDoc, HTMLElementType the_element_type, WEBC_CONTROL_DEMO_HANDLE *phandle, int max_handles)
{
int num_handles, which_element;
    num_handles = 0;
    /* Iterate through all elements of the the_element_type in the document */
    for (which_element = 0; ;which_element++)
    {
    int i;
    HELEMENT_HANDLE hElement;
       hElement = webc_DocFindElement (hDoc, 0, 0, the_element_type, which_element);
       if (!hElement)
           break;
       /* Look through the demo handle array and assign this handle to the array
          if the ID amtches and the control type matches */
       for (i = 0; i < max_handles; i++)
       {
           if ( (phandle+i)->element_type == the_element_type)
           if (webc_ElementIdMatches(hElement, (phandle+i)->element_id))
           {
               (phandle+i)->element_handle = hElement;
               num_handles += 1;
               break;
           }
        }
    }
    return(num_handles);
}

/* Utility diagnostic that maintains a list of the types of HtmlEvents that are passed to
   an installed user function. When a new event is added the whole list is printed
   and the last list printed will contain every event type recieved */
#if (DEMO_CONTROL_EVENTS)
static HTMLEventType  logged_events[NUM_HTML_EVENTS];
static void webc_demo_event_log_print(void);
#endif

void webc_demo_event_log_clear(void)
{
#if (DEMO_CONTROL_EVENTS)
int i;
    for (i = 0; i < NUM_HTML_EVENTS; i++)
    {
        logged_events[i] = HTML_EVENT_NONE;
    }
#endif
}

void webc_demo_event_log_event(HTMLEventType the_type)
{
#if (DEMO_CONTROL_EVENTS)
    if (logged_events[the_type] == HTML_EVENT_NONE)
    {
        logged_events[the_type] = the_type;
        webc_demo_event_log_print();
    }
#endif
}
#if (DEMO_CONTROL_EVENTS)
static void webc_demo_event_log_print(void)
{
    int i;
    printf("Start cases\n");
    for (i = 0; i < NUM_HTML_EVENTS; i++)
    {
        if (logged_events[i] != HTML_EVENT_NONE)
        {
            printf("        case %s:\n", GetEventTypeName(logged_events[i]));
        }
    }
    printf("end cases\n");
}
#endif
