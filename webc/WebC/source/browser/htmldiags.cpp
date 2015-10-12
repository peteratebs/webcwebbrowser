//
// HTMLDIAGS.CPP -
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

#include "htmlsdk.h"
#include "helement.hpp"
#include "delement.hpp"
#include "webcLog.h"

#if (!WEBC_MESSAGEFLOW_DEBUG)
#define NO_DONT_DO_IT 0
void PrintRecievedHTMLNotifyEvent(HTMLElement *TheReciever, Notifier *who, NotifyEvent *what)
{   if (NO_DONT_DO_IT)  /* eliminate arg warnings */   PrintRecievedHTMLNotifyEvent(TheReciever, who, what);}
void PrintRecievedHTMLEvent(HTMLElement *TheReciever, HTMLEvent *TheEvent)
{ if (NO_DONT_DO_IT)  /* eliminate arg warnings */ PrintRecievedHTMLEvent(TheReciever, TheEvent); }
void PrintRecievedDisplayElementHTMLEvent(DisplayElement *TheReciever, HTMLEvent *TheEvent)
{ if (NO_DONT_DO_IT)  /* eliminate arg warnings */  PrintRecievedDisplayElementHTMLEvent(TheReciever, TheEvent);}
void PrintRecievedDisplayElementNotifyEvent(DisplayElement *TheReciever, Notifier *who, NotifyEvent *what)
{if (NO_DONT_DO_IT)  /* eliminate arg warnings */ PrintRecievedDisplayElementNotifyEvent(TheReciever, who, what);}
char *ElementTypeToString(HTMLElementType TheElementType)
{if (NO_DONT_DO_IT)  /* eliminate arg warnings */ return(ElementTypeToString(TheElementType)); else return ("Enable WEBC_MESSAGEFLOW_DEBUG for element names");}
char * GetEventName(HTMLEvent * TheEvent)
{if (NO_DONT_DO_IT)  /* eliminate arg warnings */return (GetEventName(TheEvent)); else return ("Enable WEBC_MESSAGEFLOW_DEBUG for event names");}
#else
/* Select html events, either select non or all or select specific events using show_which_html_events[] */
static int ShowAllHtmlEevents = 1;
static int ShowNoHtmlEevents = 0;

/* Select html elements, either select non or all or select specific events using show_which_html_events[] */
static int ShowAllHtmlElements = 0;
static int ShowNoHtmlElements = 1;

/* Select display elements, either select non or all or select specific events using show_which_html_events[] */
static int ShowAllDisplayElements = 0;
static int ShowNoDisplayElements = 1;

/* Select display events, either select non or all or select specific events using show_which_html_events[] */
static int ShowAllNotifyEvent = 0;
static int ShowNoNotifyEvent = 1;

static int show_which_html_events[] = {
    0, /* HTML_EVENT_NONE */
	0, /* HTML_EVENT_CLICK */
	0, /* HTML_EVENT_DBLCLICK */
	0, /* HTML_EVENT_KEYDOWN */
	0, /* HTML_EVENT_KEYPRESS */
	0, /* HTML_EVENT_KEYUP */
	0, /* HTML_EVENT_MOUSEDOWN */
	0, /* HTML_EVENT_MOUSEMOVE */
	0, /* HTML_EVENT_MOUSEOUT */
	0, /* HTML_EVENT_MOUSEOVER */
	0, /* HTML_EVENT_MOUSEUP */
	0, /* HTML_EVENT_FOCUS */
	0, /* HTML_EVENT_UNFOCUS */
	0, /* HTML_EVENT_LOAD */
	0, /* HTML_EVENT_UNLOAD */
	0, /* HTML_EVENT_SUBMIT */
	0, /* HTML_EVENT_CHANGE */
	0, /* HTML_EVENT_EDIT */
	0, /* HTML_EVENT_RESET */
	0, /* HTML_EVENT_OBJECT_CREATED */
	0, /* HTML_EVENT_OBJECT_DESTROYED */
	0, /* HTML_EVENT_ATTRIBUTE_PARSED */
};

static int show_which_html_elements[] = {
	0, /* HTML_ADDRESS_ELEMENT */
	0, /* HTML_ANCHOR_ELEMENT */
	0, /* HTML_AREA_ELEMENT */
	0, /* HTML_BASE_ELEMENT */
	0, /* HTML_BIG_ELEMENT */
	0, /* HTML_BLOCKQUOTE_ELEMENT */
	0, /* HTML_BODY_ELEMENT */
	0, /* HTML_BOLD_ELEMENT */
	0, /* HTML_BR_ELEMENT */
	0, /* HTML_BUTTON_ELEMENT */
	0, /* HTML_CENTER_ELEMENT */
	0, /* HTML_CHECKBOX_ELEMENT */
	0, /* HTML_CITE_ELEMENT */
	0, /* HTML_CODE_ELEMENT */
	0, /* HTML_DEFINITION_ELEMENT */
	0, /* HTML_DEFINITION_LIST_ELEMENT */
	0, /* HTML_DEFINITION_TERM_ELEMENT */
	0, /* HTML_DEL_ELEMENT */
	0, /* HTML_DIV_ELEMENT */
	0, /* HTML_EDIT_STR_ELEMENT */
	0, /* HTML_EDITBOX_ELEMENT */
	0, /* HTML_ELEMENT */
	0, /* HTML_EMPHASIZE_ELEMENT */
	0, /* HTML_FLOW_ELEMENT */
	0, /* HTML_FONT_ELEMENT */
	0, /* HTML_FORM_ELEMENT */
	0, /* HTML_FRAME_ELEMENT */
	0, /* HTML_FRAMESET_ELEMENT */
	0, /* HTML_HEADING_ELEMENT */
	0, /* HTML_HIDDEN_INPUT_ELEMENT */
	0, /* HTML_HORIZONTAL_RULE_ELEMENT */
	0, /* HTML_IFRAME_ELEMENT */
	0, /* HTML_IMAGE_ELEMENT */
	0, /* HTML_INPUT_BUTTON_ELEMENT */
	0, /* HTML_INPUT_PASSWORD_ELEMENT */
	0, /* HTML_INPUT_RESET_ELEMENT */
	0, /* HTML_INS_ELEMENT */
	0, /* HTML_ITALIC_ELEMENT */
	0, /* HTML_KBD_ELEMENT */
	0, /* HTML_LABEL_ELEMENT */
	0, /* HTML_LIST_ELEMENT */
	0, /* HTML_LIST_ITEM_ELEMENT */
	0, /* HTML_MAP_ELEMENT */
	0, /* HTML_NOBR_ELEMENT */
	0, /* HTML_OPTION_ELEMENT */
	0, /* HTML_PARAGRAPH_ELEMENT */
	0, /* HTML_PRE_ELEMENT */
	0, /* HTML_RADIO_BUTTON_ELEMENT */
	0, /* HTML_SCRIPT_ELEMENT */
	0, /* HTML_SELECT_ELEMENT */
	0, /* HTML_SMALL_ELEMENT */
	0, /* HTML_SPACER_ELEMENT */
	0, /* HTML_SPAN_ELEMENT */
	0, /* HTML_STRIKE_ELEMENT */
	0, /* HTML_STRING_ELEMENT */
	0, /* HTML_STRONG_ELEMENT */
	0, /* HTML_STYLE_ELEMENT */
	0, /* HTML_TABLE_ELEMENT */
	0, /* HTML_TABLE_CELL_ELEMENT */
	0, /* HTML_TABLE_ROW_ELEMENT */
	0, /* HTML_UNDERLINE_ELEMENT */
};

static int show_which_display_element[] = {
    0, /* DISPLAY_ELEMENT */
    0, /* DISPLAY_ELEMENT_CONTENTHOLDER */
    0, /* DISPLAY_ELEMENT_HTMLFLOWDISPLAY */
    0, /* DISPLAY_ELEMENT_HTMLAREADISPLAY */
    0, /* DISPLAY_ELEMENT_FRAMESATETDISPLAY */
    0, /* DISPLAY_ELEMENT_HRDISPLAY */
    0, /* DISPLAY_ELEMENT_IMAGEDISPLAY */
    0, /* DISPLAY_ELEMENT_ITEMBULLETDISPLAY */
    0, /* DISPLAY_ELEMENT_STRINGDISPLAY */
    0, /* DISPLAY_ELEMENT_TABLEDISPLAY */
    0, /* DISPLAY_ELEMENT_DISPLAYMANAGER */
    0, /* DISPLAY_ELEMENT_KEYCATCHER */
    0, /* DISPLAY_ELEMENT_WEBBUTTON */
    0, /* DISPLAY_ELEMENT_WEBCHECKBOX */
    0, /* DISPLAY_ELEMENT_WEBDROPBOX */
    0, /* DISPLAY_ELEMENT_WEBEDITSTRING */
    0, /* DISPLAY_ELEMENT_WEBLISTBOX */
    0, /* DISPLAY_ELEMENT_WEBRADIOBUTTON */
    0, /* DISPLAY_ELEMENT_WEBSCROLL */
    0, /* DISPLAY_ELEMENT_WEBSLIDER */
    0, /* DISPLAY_ELEMENT_ZORDERED */
};

/* ==== */

static char *html_event_names[] = {
	"HTML_EVENT_NONE", /* HTML_EVENT_NONE */
	"HTML_EVENT_CLICK", /* HTML_EVENT_CLICK */
	"HTML_EVENT_DBLCLICK", /* HTML_EVENT_DBLCLICK */
	"HTML_EVENT_KEYDOWN", /* HTML_EVENT_KEYDOWN */
	"HTML_EVENT_KEYPRESS", /* HTML_EVENT_KEYPRESS */
	"HTML_EVENT_KEYUP", /* HTML_EVENT_KEYUP */
	"HTML_EVENT_MOUSEDOWN", /* HTML_EVENT_MOUSEDOWN */
	"HTML_EVENT_MOUSEMOVE", /* HTML_EVENT_MOUSEMOVE */
	"HTML_EVENT_MOUSEOUT", /* HTML_EVENT_MOUSEOUT */
	"HTML_EVENT_MOUSEOVER", /* HTML_EVENT_MOUSEOVER */
	"HTML_EVENT_MOUSEUP", /* HTML_EVENT_MOUSEUP */
	"HTML_EVENT_FOCUS", /* HTML_EVENT_FOCUS */
	"HTML_EVENT_UNFOCUS", /* HTML_EVENT_UNFOCUS */
	"HTML_EVENT_LOAD", /* HTML_EVENT_LOAD */
	"HTML_EVENT_UNLOAD", /* HTML_EVENT_UNLOAD */
	"HTML_EVENT_SUBMIT", /* HTML_EVENT_SUBMIT */
	"HTML_EVENT_CHANGE", /* HTML_EVENT_CHANGE */
	"HTML_EVENT_EDIT", /* HTML_EVENT_EDIT */
	"HTML_EVENT_RESET", /* HTML_EVENT_RESET */
	"HTML_EVENT_OBJECT_CREATED", /* HTML_EVENT_OBJECT_CREATED */
	"HTML_EVENT_OBJECT_DESTROYED", /* HTML_EVENT_OBJECT_DESTROYED */
	"HTML_EVENT_ATTRIBUTE_PARSED", /* HTML_EVENT_ATTRIBUTE_PARSED */
};


static int ShowHTMLEvent(HTMLEvent * TheEvent)
{
    if (ShowAllHtmlEevents)
        return(1);
    if (ShowNoHtmlEevents)
        return(0);
    return (show_which_html_events[TheEvent->type]);
}




static int ShowHTMLElementEvent(HTMLElement *TheElement)
{
    if (ShowAllHtmlElements)
        return(1);
    if (ShowNoHtmlElements)
        return(0);
    return (show_which_html_elements[TheElement->Type()]);
};
char *ElementTypeToString(HTMLElementType TheElementType);

static char *GetElementType(HTMLElement *TheElement)
{
    return(ElementTypeToString(TheElement->Type()));
}

static char *html_element_names[] = {
	"HTML_ADDRESS_ELEMENT", /* HTML_ADDRESS_ELEMENT */
	"HTML_ANCHOR_ELEMENT", /* HTML_ANCHOR_ELEMENT */
	"HTML_AREA_ELEMENT", /* HTML_AREA_ELEMENT */
	"HTML_BASE_ELEMENT", /* HTML_BASE_ELEMENT */
	"HTML_BIG_ELEMENT", /* HTML_BIG_ELEMENT */
	"HTML_BLOCKQUOTE_ELEMENT", /* HTML_BLOCKQUOTE_ELEMENT */
	"HTML_BODY_ELEMENT", /* HTML_BODY_ELEMENT */
	"HTML_BOLD_ELEMENT", /* HTML_BOLD_ELEMENT */
	"HTML_BR_ELEMENT", /* HTML_BR_ELEMENT */
	"HTML_BUTTON_ELEMENT", /* HTML_BUTTON_ELEMENT */
	"HTML_CENTER_ELEMENT", /* HTML_CENTER_ELEMENT */
	"HTML_CHECKBOX_ELEMENT", /* HTML_CHECKBOX_ELEMENT */
	"HTML_CITE_ELEMENT", /* HTML_CITE_ELEMENT */
	"HTML_CODE_ELEMENT", /* HTML_CODE_ELEMENT */
	"HTML_DEFINITION_ELEMENT", /* HTML_DEFINITION_ELEMENT */
	"HTML_DEFINITION_LIST_ELEMENT", /* HTML_DEFINITION_LIST_ELEMENT */
	"HTML_DEFINITION_TERM_ELEMENT", /* HTML_DEFINITION_TERM_ELEMENT */
	"HTML_DEL_ELEMENT", /* HTML_DEL_ELEMENT */
	"HTML_DIV_ELEMENT", /* HTML_DIV_ELEMENT */
	"HTML_EDIT_STR_ELEMENT", /* HTML_EDIT_STR_ELEMENT */
	"HTML_EDITBOX_ELEMENT", /* HTML_EDITBOX_ELEMENT */
	"HTML_ELEMENT", /* HTML_ELEMENT */
	"HTML_EMPHASIZE_ELEMENT", /* HTML_EMPHASIZE_ELEMENT */
	"HTML_FLOW_ELEMENT", /* HTML_FLOW_ELEMENT */
	"HTML_FONT_ELEMENT", /* HTML_FONT_ELEMENT */
	"HTML_FORM_ELEMENT", /* HTML_FORM_ELEMENT */
	"HTML_FRAME_ELEMENT", /* HTML_FRAME_ELEMENT */
	"HTML_FRAMESET_ELEMENT", /* HTML_FRAMESET_ELEMENT */
	"HTML_HEADING_ELEMENT", /* HTML_HEADING_ELEMENT */
	"HTML_HIDDEN_INPUT_ELEMENT", /* HTML_HIDDEN_INPUT_ELEMENT */
	"HTML_HORIZONTAL_RULE_ELEMENT", /* HTML_HORIZONTAL_RULE_ELEMENT */
	"HTML_IFRAME_ELEMENT", /* HTML_IFRAME_ELEMENT */
	"HTML_IMAGE_ELEMENT", /* HTML_IMAGE_ELEMENT */
	"HTML_INPUT_BUTTON_ELEMENT", /* HTML_INPUT_BUTTON_ELEMENT */
	"HTML_INPUT_PASSWORD_ELEMENT", /* HTML_INPUT_PASSWORD_ELEMENT */
	"HTML_INPUT_RESET_ELEMENT", /* HTML_INPUT_RESET_ELEMENT */
	"HTML_INS_ELEMENT", /* HTML_INS_ELEMENT */
	"HTML_ITALIC_ELEMENT", /* HTML_ITALIC_ELEMENT */
	"HTML_KBD_ELEMENT", /* HTML_KBD_ELEMENT */
	"HTML_LABEL_ELEMENT", /* HTML_LABEL_ELEMENT */
	"HTML_LIST_ELEMENT", /* HTML_LIST_ELEMENT */
	"HTML_LIST_ITEM_ELEMENT", /* HTML_LIST_ITEM_ELEMENT */
	"HTML_MAP_ELEMENT", /* HTML_MAP_ELEMENT */
	"HTML_NOBR_ELEMENT", /* HTML_NOBR_ELEMENT */
	"HTML_OPTION_ELEMENT", /* HTML_OPTION_ELEMENT */
	"HTML_PARAGRAPH_ELEMENT", /* HTML_PARAGRAPH_ELEMENT */
	"HTML_PRE_ELEMENT", /* HTML_PRE_ELEMENT */
	"HTML_RADIO_BUTTON_ELEMENT", /* HTML_RADIO_BUTTON_ELEMENT */
	"HTML_SCRIPT_ELEMENT", /* HTML_SCRIPT_ELEMENT */
	"HTML_SELECT_ELEMENT", /* HTML_SELECT_ELEMENT */
	"HTML_SMALL_ELEMENT", /* HTML_SMALL_ELEMENT */
	"HTML_SPACER_ELEMENT", /* HTML_SPACER_ELEMENT */
	"HTML_SPAN_ELEMENT", /* HTML_SPAN_ELEMENT */
	"HTML_STRIKE_ELEMENT", /* HTML_STRIKE_ELEMENT */
	"HTML_STRING_ELEMENT", /* HTML_STRING_ELEMENT */
	"HTML_STRONG_ELEMENT", /* HTML_STRONG_ELEMENT */
	"HTML_STYLE_ELEMENT", /* HTML_STYLE_ELEMENT */
	"HTML_TABLE_ELEMENT", /* HTML_TABLE_ELEMENT */
	"HTML_TABLE_CELL_ELEMENT", /* HTML_TABLE_CELL_ELEMENT */
	"HTML_TABLE_ROW_ELEMENT", /* HTML_TABLE_ROW_ELEMENT */
	"HTML_UNDERLINE_ELEMENT", /* HTML_UNDERLINE_ELEMENT */
};




static int ShowDisplayElementEvent(DisplayElement *TheDisplayElement)
{
    if (ShowAllDisplayElements)
        return(1);
    if (ShowNoDisplayElements)
        return(0);
    return(show_which_display_element[TheDisplayElement->Type()]);

}

static char *display_element_names[] = {
    "DISPLAY_ELEMENT", /* DISPLAY_ELEMENT */
    "DISPLAY_ELEMENT_CONTENTHOLDER", /* DISPLAY_ELEMENT_CONTENTHOLDER */
    "DISPLAY_ELEMENT_HTMLFLOWDISPLAY", /* DISPLAY_ELEMENT_HTMLFLOWDISPLAY */
    "DISPLAY_ELEMENT_HTMLAREADISPLAY", /* DISPLAY_ELEMENT_HTMLAREADISPLAY */
    "DISPLAY_ELEMENT_FRAMESATETDISPLAY", /* DISPLAY_ELEMENT_FRAMESATETDISPLAY */
    "DISPLAY_ELEMENT_HRDISPLAY", /* DISPLAY_ELEMENT_HRDISPLAY */
    "DISPLAY_ELEMENT_IMAGEDISPLAY", /* DISPLAY_ELEMENT_IMAGEDISPLAY */
    "DISPLAY_ELEMENT_ITEMBULLETDISPLAY", /* DISPLAY_ELEMENT_ITEMBULLETDISPLAY */
    "DISPLAY_ELEMENT_STRINGDISPLAY", /* DISPLAY_ELEMENT_STRINGDISPLAY */
    "DISPLAY_ELEMENT_TABLEDISPLAY", /* DISPLAY_ELEMENT_TABLEDISPLAY */
    "DISPLAY_ELEMENT_DISPLAYMANAGER", /* DISPLAY_ELEMENT_DISPLAYMANAGER */
    "DISPLAY_ELEMENT_KEYCATCHER", /* DISPLAY_ELEMENT_KEYCATCHER */
    "DISPLAY_ELEMENT_WEBBUTTON", /* DISPLAY_ELEMENT_WEBBUTTON */
    "DISPLAY_ELEMENT_WEBCHECKBOX", /* DISPLAY_ELEMENT_WEBCHECKBOX */
    "DISPLAY_ELEMENT_WEBDROPBOX", /* DISPLAY_ELEMENT_WEBDROPBOX */
    "DISPLAY_ELEMENT_WEBEDITSTRING", /* DISPLAY_ELEMENT_WEBEDITSTRING */
    "DISPLAY_ELEMENT_WEBLISTBOX", /* DISPLAY_ELEMENT_WEBLISTBOX */
    "DISPLAY_ELEMENT_WEBRADIOBUTTON", /* DISPLAY_ELEMENT_WEBRADIOBUTTON */
    "DISPLAY_ELEMENT_WEBSCROLL", /* DISPLAY_ELEMENT_WEBSCROLL */
    "DISPLAY_ELEMENT_WEBSLIDER", /* DISPLAY_ELEMENT_WEBSLIDER */
    "DISPLAY_ELEMENT_ZORDERED", /* DISPLAY_ELEMENT_ZORDERED */
};

static char *GetDisplayElementType(DisplayElement *TheDisplayElement)
{
    return (display_element_names[TheDisplayElement->Type()]);
}

static char *notifier_type_names[] = {
	"NOTIFIER_TYPE_NONE", /* NOTIFIER_TYPE_NONE */
	"NOTIFIER_TYPE_HTML_ELEMENT", /* NOTIFIER_TYPE_HTML_ELEMENT */
	"NOTIFIER_TYPE_DISPLAY_ELEMENT", /* NOTIFIER_TYPE_DISPLAY_ELEMENT */
	"NOTIFIER_TYPE_IMAGE", /* NOTIFIER_TYPE_IMAGE */
	"NOTIFIER_TYPE_BUTTON", /* NOTIFIER_TYPE_BUTTON */
	"NOTIFIER_TYPE_SELECT", /* NOTIFIER_TYPE_SELECT */
	"NOTIFIER_TYPE_EDIT", /* NOTIFIER_TYPE_EDIT */
	"NOTIFIER_TYPE_SLIDER", /* NOTIFIER_TYPE_SLIDER */
	"NOTIFIER_TYPE_TEXT_UNIT", /* NOTIFIER_TYPE_TEXT_UNIT */
	"NOTIFIER_TYPE_RADIO_BUTTON", /* NOTIFIER_TYPE_RADIO_BUTTON */
	"NOTIFIER_TYPE_LIST_BOX", /* NOTIFIER_TYPE_LIST_BOX */
	"NOTIFIER_TYPE_DISPLAY_MANAGER", /* NOTIFIER_TYPE_DISPLAY_MANAGER */
	"NOTIFIER_TYPE_NODISPLAY_HTML_CHILD", /* NOTIFIER_TYPE_NODISPLAY_HTML_CHILD */
	"NOTIFIER_TYPE_CHILD_TABLE_ROW" /* NOTIFIER_TYPE_CHILD_TABLE_ROW */
};

static char *GetNotifierType(NotifyEvent *what)
{
    return(notifier_type_names[what->type]);
}

static char *notifier_event_names[] = {
	"NOTIFY_BITMAP_CHANGE", /* NOTIFY_BITMAP_CHANGE */
	"NOTIFY_BUTTON_DOWN", /* NOTIFY_BUTTON_DOWN */
	"NOTIFY_BUTTON_PRESS", /* NOTIFY_BUTTON_PRESS */
	"NOTIFY_BUTTON_UP", /* NOTIFY_BUTTON_UP */
	"NOTIFY_CONTENT_ADDED", /* NOTIFY_CONTENT_ADDED */
	"NOTIFY_CONTENT_REMOVED", /* NOTIFY_CONTENT_REMOVED */
	"NOTIFY_DISPLAY_STYLE_CHANGE", /* NOTIFY_DISPLAY_STYLE_CHANGE */
	"NOTIFY_DONE", /* NOTIFY_DONE */
	"NOTIFY_FLOW_CHILD_STYLE_CHANGE", /* NOTIFY_FLOW_CHILD_STYLE_CHANGE */
	"NOTIFY_FOCUS_LOST", /* NOTIFY_FOCUS_LOST */
	"NOTIFY_FORMAT_STYLE_CHANGE", /* NOTIFY_FORMAT_STYLE_CHANGE */
	"NOTIFY_MAX_WIDTH_CHANGE", /* NOTIFY_MAX_WIDTH_CHANGE */
	"NOTIFY_MIN_WIDTH_CHANGE", /* NOTIFY_MIN_WIDTH_CHANGE */
	"NOTIFY_PARENT_DISPLAY_STYLE_CHANGE", /* NOTIFY_PARENT_DISPLAY_STYLE_CHANGE */
	"NOTIFY_PARENT_STYLE_CHANGE", /* NOTIFY_PARENT_STYLE_CHANGE */
	"NOTIFY_POSITIONED_CHILD_STYLE_CHANGE", /* NOTIFY_POSITIONED_CHILD_STYLE_CHANGE */
	"NOTIFY_SELECT_CHANGE", /* NOTIFY_SELECT_CHANGE */
	"NOTIFY_SIZE_CHANGE", /* NOTIFY_SIZE_CHANGE */
	"NOTIFY_SLIDER_MOVE", /* NOTIFY_SLIDER_MOVE */
	"NOTIFY_STYLE_CHANGE", /* NOTIFY_STYLE_CHANGE */
	"NOTIFY_TEXT_EDIT", /* NOTIFY_TEXT_EDIT */
	"NOTIFY_WEB_GRAPHICS_CHANGE", /* NOTIFY_WEB_GRAPHICS_CHANGE */
	"NOTIFY_ZINDEX_CHANGE", /* NOTIFY_ZINDEX_CHANGE */
	"NUM_NOTIFIY_EVENTS" /* NUM_NOTIFIY_EVENTS */
};
static char *GetNotifierEventType(NotifyEvent *what)
{
    return(notifier_event_names[what->event]);
};

static char * GetEventTypeName(HTMLEventType the_type);
char * GetEventName(HTMLEvent * TheEvent)
{
    return(GetEventTypeName(TheEvent->type));
}
static char * GetEventTypeName(HTMLEventType the_type)
{
    return(html_event_names[the_type]);
}

void PrintRecievedHTMLNotifyEvent(HTMLElement *TheReciever, Notifier *who, NotifyEvent *what)
{
WEBC_CHAR *name,*id;

        if (ShowNoNotifyEvent)
            return;

        if (!ShowAllNotifyEvent)
            if (!ShowHTMLElementEvent(TheReciever))
                return;
        name = TheReciever->Name();
        id = TheReciever->Id();
        if (name || id)
        {
            if (!id)
                id = (WEBC_CHAR *)"";
            if (!name)
                name=(WEBC_CHAR *)"";
            LOG_PRINTF("%s(name==%S)(id==%S) <== %s %s\n",
                GetElementType(TheReciever),
                name,
                id,
                GetNotifierType(what),
                GetNotifierEventType(what) );
        }
        else
        {
            LOG_PRINTF("%s <== %s %s\n",
                GetElementType(TheReciever),
                GetNotifierType(what),
                GetNotifierEventType(what) );
        }
}



void PrintRecievedHTMLEvent(HTMLElement *TheReciever, HTMLEvent *TheEvent)
{
WEBC_CHAR *name,*id;
        if (!ShowHTMLElementEvent(TheReciever))
            return;
        if (!ShowHTMLEvent(TheEvent))
            return;
        name = TheReciever->Name();
        id = TheReciever->Id();
        if (name || id)
        {
            if (!id)
                id = (WEBC_CHAR *)"";
            if (!name)
                name=(WEBC_CHAR *)"";
            LOG_PRINTF("%s(name==%S)(id==%S) <== %s\n",
                GetElementType(TheReciever),
                name,
                id,
                GetEventName(TheEvent));
        }
        else
        {
            LOG_PRINTF("%s <== %s\n",
                GetElementType(TheReciever),
                GetEventName(TheEvent));
        }
}

void PrintRecievedDisplayElementHTMLEvent(DisplayElement *TheReciever, HTMLEvent *TheEvent)
{
        if (!ShowDisplayElementEvent(TheReciever))
            return;
        if (!ShowHTMLEvent(TheEvent))
            return;
        LOG_PRINTF("Notify:%s <== %s\n",
            GetDisplayElementType(TheReciever),
            GetEventName(TheEvent));
}


void PrintRecievedDisplayElementNotifyEvent(DisplayElement *TheReciever, Notifier *who, NotifyEvent *what)
{
    if (ShowNoNotifyEvent)
        return;
    if (!ShowAllNotifyEvent)
        if (!ShowDisplayElementEvent(TheReciever))
            return;

    LOG_PRINTF("Notify:%s <== %s %s\n",
        GetDisplayElementType(TheReciever),
        GetNotifierType(what),
        GetNotifierEventType(what) );
}

char *ElementTypeToString(HTMLElementType TheElementType)
{
    return(html_element_names[TheElementType]);
};


#endif /* WEBC_MESSAGEFLOW_DEBUG */
