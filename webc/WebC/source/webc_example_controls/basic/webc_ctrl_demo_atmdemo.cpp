#include "webc_ctrl_demo.h"
/*
webc_ctrl_demo_atmdemo.cpp

    void webc_ctrl_demo_atmdemo(void)
        Demonstrate using multiple webc API calls to implement a simulated Automatic Teller Machine

*/

#define DIV_CUSTINFO             0
#define TINPUT_CUSTNAME          1
#define TINPUT_CUSTPIN           2
#define TINPUT_CUSTACCOUNT       3
#define TINPUT_CUSTBALANCE       4

#define DIV_FORMCONTROL          5
#define BUTTON_INSERT            6
#define BUTTON_BIOID             7
#define IMAGE_BIOFACE            8
#define BUTTON_FINISH            9
#define BUTTON_CANCEL           10

#define DIV_FORMHELPBOXL        11
#define TEXTBOX_HELP            12

#define DIV_TRANSACTIONTYPE     13
#define RADIOBUTTON_DEPOSIT     14
#define RADIOBUTTON_WITHDRAWAL  15

#define DIV_TRANSACTIONAMOUNT   16
#define TINPUT_DEPOSIT          17
#define TINPUT_WITHDRAW         18

#define DIV_KEYPAD              19
#define BUTTON_7                20
#define BUTTON_8                21
#define BUTTON_9                22
#define BUTTON_LEFT             23
#define BUTTON_4                24
#define BUTTON_5                25
#define BUTTON_6                26
#define BUTTON_DEL              27
#define BUTTON_1                28
#define BUTTON_2                29
#define BUTTON_3                30
#define BUTTON_OK               31
#define BUTTON_0                32
#define BUTTON_DOT              33

#define DEMO_CONTROL_COUNT       34
/* Generic Demo setup step 2: Declare an array to hold the handles */
static WEBC_CONTROL_DEMO_HANDLE demo_control_handles[DEMO_CONTROL_COUNT];
static void init_demo_control_handles(void);
void demo_atm_initialize_help_strings(void);
static void demo_atm_display_help_set(char *help_value);
static void demo_atm_display_help_restore_default(void);

static void atm_demo_mouseover_help(HELEMENT_HANDLE hElement,HTMLEvent* event);
static int demo_atm_element_to_index(HELEMENT_HANDLE hElement);
static void atm_demo_simulate_new_customer(char *custname, char *custaccout, int custbalance);
static void atm_demo_simulate_take_cash(void);
static void atm_demo_simulate_finish(void);
static void atm_demo_simulate_cancel(void);
static void atm_demo_simulate_deposit(void);
static void atm_demo_simulate_withdrawal(void);
static void atm_demo_simulate_deposit_complete(void);
static void atm_demo_simulate_withdrawal_complete(void);
static void atm_demo_simulate_keypress(WebKey theKey);
static void atm_demo_simulate_check_pin(char *pin_value);
static void atm_demo_simulate_show_bioface(WEBC_BOOL show);

/* Set the PIN field to the only legal field */
static void demo_atm_clear_focus_choice(void);
static void demo_atm_add_focus_choice(int which_choice);

static char *_atm_help_default_string = "To Start\nScan Your\nCard\nOr Select\nBioMetric ID\nNo other\nChoices";
static char *atm_help_default_string  = _atm_help_default_string;
static char *atm_keypad_help = "Click Digits\nClick Ok\nTo Continue";
static char *atm_help_pin_string = "Enter A\n4 digit Pin\nBefore\nContinuing\nOr Press\nCancel\nNo other\nChoices";
static char *atm_help_select_transaction_string = "Select\nDeposit or..\nWithdrawl\nOr Press\nCancel\nNo other\nChoices";
static char *atm_help_deposit_string = "Type\nAmount to Deposit\nOr use keypad\nOr Press\nCancel\nNo other\nChoices";
static char *atm_help_withdrawal_string = "Type\nAmount to Withdraw\nOr use keypad\nOr Press\nCancel\nNo other\nChoices";
static char *atm_help_done_deposit_string = "Thank You\nFor Depositing\nPlease Press\nFinish now\n";
static char *atm_help_done_withdrawal_string = "Thank You\nFor Withdrawing\nPlease Press\nFinish now\n";

/* Keypad entries go here */
static int current_keypad_target = -1;  /* can be: -1, TINPUT_DEPOSIT, TINPUT_WITHDRAW, TINPUT_PIN */

/* Manage which controls are active, depending on the state */
static int num_legal_controls = -1;
static int which_legal_controls[DEMO_CONTROL_COUNT];

/* Generic Demo setup step 5: Provide an array of html source code that
   will be loaded into the document view of the demo browser when the demo
   is started. */
static char **get_demo_control_html_strings();

/* Atm Demo Start Point */
void webc_control_demo_atmdemo(int command)
{
    if (command == START_DEMO)
    {
        /* Call a helper function to load the root page, which breaks the browser into frames */
        if (webc_control_demo_load_array(wcdContext.hDemoDoc, get_demo_control_html_strings()) < 0)
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
        demo_atm_initialize_help_strings();
        command = RESTART_DEMO; /* Fall through */
    }
    if (command == RESTART_DEMO)
    {
        current_keypad_target = -1;
        /* At first allow only Insert card and BIO ID */
        demo_atm_clear_focus_choice();
        demo_atm_add_focus_choice(BUTTON_INSERT);
        demo_atm_add_focus_choice(BUTTON_BIOID);
        webc_ElementSetValueASCII (demo_control_handles[TINPUT_CUSTNAME].element_handle, "", WEBC_TRUE);
        webc_ElementSetValueASCII (demo_control_handles[TINPUT_CUSTACCOUNT].element_handle, "", WEBC_TRUE);
        webc_ElementSetValueASCII (demo_control_handles[TINPUT_CUSTBALANCE].element_handle, "", WEBC_TRUE);
        webc_ElementSetValueASCII (demo_control_handles[TINPUT_CUSTPIN].element_handle, "", WEBC_TRUE);
        webc_ElementSetValueASCII (demo_control_handles[TINPUT_DEPOSIT].element_handle, "", WEBC_TRUE);
        webc_ElementSetValueASCII (demo_control_handles[TINPUT_WITHDRAW].element_handle,"", WEBC_TRUE);
        atm_demo_simulate_show_bioface(WEBC_FALSE);
        atm_help_default_string  = _atm_help_default_string;
        demo_atm_display_help_restore_default();
    } else if (command == STOP_DEMO)
    {
        /* Put any necesseray stop code here */
    }
}
/* The event handlers we will employ */
static HTMLEventStatus cb_ATM_RDONLY_EDITSTR (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_ATM_PIN (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_ATM_CONTROL_BUTTON (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_ATM_RADIO_BUTTON (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_ATM_INPUT_EDITSTR (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_ATM_KEYPAD_BUTTON (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_ATM_KEYPAD_CONTROLKEY (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_ATM_HELP_BOX  (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static void atm_demo_mouseover_help(HELEMENT_HANDLE hElement,HTMLEvent* event);
static int atm_app_check_active(HELEMENT_HANDLE hElement,HTMLEvent* event);


static HTMLEventStatus cb_ATM_HELP_BOX  (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    return(HTML_EVENT_STATUS_DONE);
}
static HTMLEventStatus cb_ATM_RDONLY_EDITSTR (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    if (!atm_app_check_active(hElement,event)) /* Don't process any events if not allowed for this control */
        return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
    atm_demo_mouseover_help(hElement, event); /* Keep help box up to date */
    return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

static HTMLEventStatus cb_ATM_PIN (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
char *pin_value;

    if (!atm_app_check_active(hElement,event)) /* Don't process any events if not allowed for this control */
        return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
    atm_demo_mouseover_help(hElement, event); /* Keep help box up to date */
    switch (event->type)
    {
        case HTML_EVENT_KEYDOWN:
             /* Allow control codes */
		    if (event->data.key >= WGK_PGUP)
                return(HTML_EVENT_STATUS_CONTINUE);
            /* Only allow digits */
		    else if (event->data.key < '0' || event->data.key > '9')
                return(HTML_EVENT_STATUS_DONE);

        default:
        break;
    }
    pin_value = webc_ElementGetValueASCII(hElement);    /* Get the contents value */
    if (pin_value)
    {
        atm_demo_simulate_check_pin(pin_value);
        webc_FreeASCIIString(pin_value);
   }
   return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

static HTMLEventStatus cb_ATM_CONTROL_BUTTON (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
int is_clicked = 0;
    if (!atm_app_check_active(hElement,event)) /* Don't process any events if not allowed for this control */
    {
        /* But allow CANCEL requests */
        if (demo_atm_element_to_index(hElement) == BUTTON_CANCEL)
        {
            if (event->type == HTML_EVENT_CLICK)
                atm_demo_simulate_cancel();
        }
        return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
    }
    atm_demo_mouseover_help(hElement, event); /* Keep help box up to date */
    switch (event->type)
    {
        case HTML_EVENT_CLICK:
            is_clicked = 1;
            break;
        case HTML_EVENT_KEYDOWN:
		    if (event->data.key == WGK_CR)
		        is_clicked = 1;
		    break;
        default:
        break;
    }
    if (is_clicked)
    {
    int which_button;
        which_button = demo_atm_element_to_index(hElement);
        switch (which_button)
        {
          /* Simulate card isertion or bio id */
          case BUTTON_INSERT            :
            atm_demo_simulate_new_customer("John CardHolder", "ACT#3470", 179800);
          break;
          case BUTTON_BIOID             :
            atm_demo_simulate_new_customer("Jane BioIdCustomer", "ACT#1234", 384000);
            atm_demo_simulate_show_bioface(WEBC_TRUE);
          break;
          case BUTTON_FINISH            :
            atm_demo_simulate_finish();
            return(HTML_EVENT_STATUS_DONE);
          break;
          case BUTTON_CANCEL            :
            atm_demo_simulate_cancel();
            return(HTML_EVENT_STATUS_DONE);
          break;
        }
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

static HTMLEventStatus cb_ATM_RADIO_BUTTON (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
int which_button;
    if (!atm_app_check_active(hElement,event)) /* Don't process any events if not allowed for this control */
        return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
    atm_demo_mouseover_help(hElement, event); /* Keep help box up to date */
    switch (event->type)
    {
        default:
            return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
        case HTML_EVENT_CLICK:
        case HTML_EVENT_KEYDOWN:
            break;
    }
    which_button = demo_atm_element_to_index(hElement);
    if (which_button == RADIOBUTTON_DEPOSIT)
        atm_demo_simulate_deposit();
    else if (which_button == RADIOBUTTON_WITHDRAWAL)
        atm_demo_simulate_withdrawal();
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

static HTMLEventStatus cb_ATM_INPUT_EDITSTR (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    if (!atm_app_check_active(hElement,event)) /* Don't process any events if not allowed for this control */
        return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
    atm_demo_mouseover_help(hElement, event); /* Keep help box up to date */
    switch (event->type)
    {
        case HTML_EVENT_CLICK:
        case HTML_EVENT_KEYDOWN:
            /* Allow control codes */
		    if (event->data.key >= WGK_PGUP)
                return(HTML_EVENT_STATUS_CONTINUE);
		    else if (event->data.key == WGK_CR)
            {
                if (demo_atm_element_to_index(hElement) == TINPUT_DEPOSIT)
		            atm_demo_simulate_deposit_complete();
                else if (demo_atm_element_to_index(hElement) == TINPUT_WITHDRAW)
		            atm_demo_simulate_withdrawal_complete();
                return(HTML_EVENT_STATUS_DONE);
            }
            /* Do not allow non digits */
		    else if (event->data.key != '.' && (event->data.key < '0' || event->data.key > '9') )
                return(HTML_EVENT_STATUS_DONE);
            else
                break;
        default:
        break;
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */

}
static HTMLEventStatus cb_ATM_KEYPAD_BUTTON (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
int is_clicked = 0;
    if (!atm_app_check_active(hElement,event)) /* Don't process any events if not allowed for this control */
    {/* Allow processing but don't update help */
        atm_demo_mouseover_help(hElement, event); /* Keep help box up to date */
    }
    switch (event->type)
    {
        case HTML_EVENT_CLICK:
            is_clicked = 1;
            break;
        case HTML_EVENT_KEYDOWN:
		    if (event->data.key == WGK_CR)
		        is_clicked = 1;
		    break;
        default:
        break;
    }
    if (is_clicked)
    {
        char *this_value;
        this_value = webc_ElementGetValueASCII(hElement);    /* Get the contents value */
        if (this_value)
        {
            WebKey theKey;
                theKey = (WebKey) *this_value;
                webc_FreeASCIIString(this_value);
                atm_demo_simulate_keypress(theKey);
        }
        return(HTML_EVENT_STATUS_DONE);
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

static HTMLEventStatus cb_ATM_KEYPAD_CONTROLKEY (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
int which_button, is_clicked = 0;

    if (!atm_app_check_active(hElement,event)) /* Don't process any events if not allowed for this control */
    {/* Allow processing but don't update help */
        atm_demo_mouseover_help(hElement, event); /* Keep help box up to date */
    }

    switch (event->type)
    {
        case HTML_EVENT_CLICK:
            is_clicked = 1;
            break;
        case HTML_EVENT_KEYDOWN:
		    if (event->data.key == WGK_CR)
		        is_clicked = 1;
		    break;
        default:
        break;
    }
    if (is_clicked)
    {
        which_button = demo_atm_element_to_index(hElement);
        if (which_button == BUTTON_LEFT)
            atm_demo_simulate_keypress(WGK_LEFT);
        if (which_button == BUTTON_DEL)
            atm_demo_simulate_keypress(WGK_BACKSPACE);
        if (which_button == BUTTON_OK)
            atm_demo_simulate_keypress(WGK_CR);
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

static void atm_demo_simulate_check_pin(char *pin_value)
{
    if (pin_value[3] >= '0' && pin_value[3] <= '9')
    {
        demo_atm_clear_focus_choice();
        demo_atm_add_focus_choice(RADIOBUTTON_DEPOSIT);
        demo_atm_add_focus_choice(RADIOBUTTON_WITHDRAWAL);
        demo_atm_display_help_set(atm_help_select_transaction_string);
        atm_help_default_string = atm_help_select_transaction_string;
        /* Route keypad input */
        current_keypad_target = -1;
    }
}
static void atm_demo_simulate_new_customer(char *custname, char *custaccout, int custbalance)
{
char ascii_custbalance[32];
    webc_control_demo_animate_fullscreen('+');
    webc_ElementSetValueASCII (demo_control_handles[TINPUT_CUSTNAME].element_handle, custname, WEBC_TRUE);
    webc_ElementSetValueASCII (demo_control_handles[TINPUT_CUSTACCOUNT].element_handle, custaccout, WEBC_TRUE);
    rtp_sprintf(ascii_custbalance, "$ %d.00", custbalance);
    webc_ElementSetValueASCII (demo_control_handles[TINPUT_CUSTBALANCE].element_handle, ascii_custbalance, WEBC_TRUE);
    /* Set the PIN filed to the only leagel field */
    demo_atm_clear_focus_choice();
    demo_atm_add_focus_choice(TINPUT_CUSTPIN);
    demo_atm_display_help_set(atm_help_pin_string);
    atm_help_default_string = atm_help_pin_string;
    /* Route keypad input */
    current_keypad_target = TINPUT_CUSTPIN;

}

static void atm_demo_simulate_show_bioface(WEBC_BOOL show)
{
char *style_show_element;
    if (show)
        style_show_element    =    "visibility:show;";
    else
        style_show_element    =   "visibility:hidden;";
    webc_ElementSetStyleASCII(demo_control_handles[IMAGE_BIOFACE].element_handle,
        style_show_element,WEBC_TRUE);
}
static void atm_demo_simulate_finish(void)
{
    webc_control_demo_animate_fullscreen('-');
    webc_control_demo_atmdemo(RESTART_DEMO);
}
static void atm_demo_simulate_cancel(void)
{
    webc_control_demo_animate_fullscreen('-');
    webc_control_demo_atmdemo(RESTART_DEMO);
}


static void atm_demo_simulate_deposit_complete(void)
{
    demo_atm_clear_focus_choice();
    demo_atm_add_focus_choice(BUTTON_FINISH);
    demo_atm_display_help_set(atm_help_done_deposit_string);
    atm_help_default_string = atm_help_done_deposit_string;
    /* Route keypad input */
    current_keypad_target = -1;
}
static void atm_demo_simulate_deposit(void)
{
    demo_atm_clear_focus_choice();
    demo_atm_add_focus_choice(TINPUT_DEPOSIT);
    demo_atm_display_help_set(atm_help_deposit_string);
    atm_help_default_string = atm_help_deposit_string;
    /* Route keypad input */
    current_keypad_target = TINPUT_DEPOSIT;
}

static void atm_demo_simulate_withdrawal_complete(void)
{
    demo_atm_clear_focus_choice();
    demo_atm_add_focus_choice(BUTTON_FINISH);
    demo_atm_display_help_set(atm_help_done_withdrawal_string);
    atm_help_default_string = atm_help_done_withdrawal_string;
    /* Route keypad input */
    current_keypad_target = -1;
}

static void atm_demo_simulate_withdrawal(void)
{
    demo_atm_clear_focus_choice();
    demo_atm_add_focus_choice(TINPUT_WITHDRAW);
    demo_atm_display_help_set(atm_help_withdrawal_string);
    atm_help_default_string = atm_help_withdrawal_string;
    /* Route keypad input */
    current_keypad_target = TINPUT_WITHDRAW;
}

static void demo_atm_clear_focus_choice(void)
{
    if (num_legal_controls == -1)
        return;
    for (int i = 0; i <= num_legal_controls; i++)
    {
        webc_ElementSetStyleASCII(demo_control_handles[which_legal_controls[i]].element_handle,
            "background-color:DarkGray",WEBC_TRUE);
    }
    num_legal_controls = -1;
}
static void demo_atm_add_focus_choice(int which_choice)
{
    num_legal_controls += 1;
    which_legal_controls[num_legal_controls] = which_choice;
    webc_ElementSetStyleASCII(demo_control_handles[which_choice].element_handle,
        "background-color:Blue",WEBC_TRUE);
    /* If this is the first element in our focus group set the focus */
    if (num_legal_controls == 0)
        webc_ElementFocus(demo_control_handles[which_choice].element_handle, WEBC_TRUE);

}

static int atm_app_check_active(HELEMENT_HANDLE hElement,HTMLEvent* event)
{
int my_index = demo_atm_element_to_index(hElement);
    if (num_legal_controls == -1)
        return(1);
    for (int i = 0; i <= num_legal_controls; i++)
    {
        if (which_legal_controls[i] == my_index)
            return(1);
    }
    return(0);
}

static void atm_demo_simulate_keypress(WebKey theKey)
{
HTMLEvent my_event;
    if (current_keypad_target != -1)
    {
            /* Set focus to the control we want */
            webc_ElementFocus(demo_control_handles[current_keypad_target].element_handle, WEBC_TRUE);
            /* Kludge, right shift to the end */
            for (int i = 0; i < 30; i++)
            {
                my_event.type = HTML_EVENT_KEYDOWN;
                my_event.data.key = WGK_RIGHT;
                webc_BrowserDispatchEvent(wcdContext.hBrowser, &my_event);
            }

            /* And send the key to it */
            my_event.type = HTML_EVENT_KEYDOWN;
            my_event.data.key = theKey;
            webc_BrowserDispatchEvent(wcdContext.hBrowser, &my_event);
    }
}

static char *atm_help_strings[DEMO_CONTROL_COUNT];
void demo_atm_initialize_help_strings(void)
{
    int i;
    for (i = 0; i < DEMO_CONTROL_COUNT; i++)
        atm_help_strings[i] = 0;

    atm_help_strings[TINPUT_CUSTPIN] = "Enter\nYour\n4 digit\nPin\n";
    atm_help_strings[BUTTON_INSERT ] = "Click\nTo Simulate\nCard Swipe";
    atm_help_strings[BUTTON_BIOID  ] = "Click\nTo Simulate\nRetina Scan";
    atm_help_strings[BUTTON_FINISH ] = "Click\nTo Finish";
    atm_help_strings[BUTTON_CANCEL ] = "Click\nTo Cancel";
    atm_help_strings[RADIOBUTTON_DEPOSIT    ] = "Click\nTo Deposit";
    atm_help_strings[RADIOBUTTON_WITHDRAWAL ] = "Click\nTo Witdraw";
    atm_help_strings[TINPUT_DEPOSIT] = "Enter\nDeposit\nAmount";
    atm_help_strings[TINPUT_WITHDRAW] = "Enter\nWithdrawal\nAmount";

    atm_help_strings[BUTTON_7    ] = atm_keypad_help;
    atm_help_strings[BUTTON_8    ] = atm_keypad_help;
    atm_help_strings[BUTTON_9    ] = atm_keypad_help;
    atm_help_strings[BUTTON_LEFT ] = atm_keypad_help;
    atm_help_strings[BUTTON_4    ] = atm_keypad_help;
    atm_help_strings[BUTTON_5    ] = atm_keypad_help;
    atm_help_strings[BUTTON_6    ] = atm_keypad_help;
    atm_help_strings[BUTTON_DEL  ] = atm_keypad_help;
    atm_help_strings[BUTTON_1    ] = atm_keypad_help;
    atm_help_strings[BUTTON_2    ] = atm_keypad_help;
    atm_help_strings[BUTTON_3    ] = atm_keypad_help;
    atm_help_strings[BUTTON_OK   ] = atm_keypad_help;
    atm_help_strings[BUTTON_0    ] = atm_keypad_help;
    atm_help_strings[BUTTON_DOT  ] = atm_keypad_help;
}

static int demo_atm_element_to_index(HELEMENT_HANDLE hElement)
{
    int i;
    for (i = 0; i < DEMO_CONTROL_COUNT; i++)
    {
        if (hElement == demo_control_handles[i].element_handle)
            return(i);
    }
    return(-1);
}

static void demo_atm_display_help_set(char *help_value)
{
    webc_ElementSetValueASCII(demo_control_handles[TEXTBOX_HELP].element_handle, help_value, WEBC_TRUE);
}
static void demo_atm_display_element_help_set(HELEMENT_HANDLE hElement)
{
int which_element;
    which_element = demo_atm_element_to_index(hElement);
    if (which_element >= 0 && atm_help_strings[which_element])
        demo_atm_display_help_set(atm_help_strings[which_element]);
}

static void demo_atm_display_help_restore_default(void)
{
    demo_atm_display_help_set(atm_help_default_string);
}
/* Spies on message flow and displays helps boxes as needed */
static void atm_demo_mouseover_help(HELEMENT_HANDLE hElement,HTMLEvent* event)
{
    switch (event->type)
    {
        case HTML_EVENT_MOUSEOVER:
            demo_atm_display_element_help_set(hElement);
            break;
        case HTML_EVENT_MOUSEOUT:
            demo_atm_display_help_restore_default();
            break;
        default:
        break;
    }
}

static void init_demo_control_handles(void)
{
    webc_control_demo_init_control_array(&demo_control_handles[0],DEMO_CONTROL_COUNT);

/* Customer information Div */
    demo_control_handles[DIV_CUSTINFO].element_type = HTML_DIV_ELEMENT;
    demo_control_handles[DIV_CUSTINFO].element_id = "DIV_CUSTINFO";
    demo_control_handles[DIV_CUSTINFO].element_handle = 0;
    demo_control_handles[DIV_CUSTINFO].cb = 0;
/*        Customer Name */
    demo_control_handles[TINPUT_CUSTNAME].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[TINPUT_CUSTNAME].element_id = "TINPUT_CUSTNAME";
    demo_control_handles[TINPUT_CUSTNAME].element_handle = 0;
    demo_control_handles[TINPUT_CUSTNAME].cb = cb_ATM_RDONLY_EDITSTR;
/*        Customer Pin */
    demo_control_handles[TINPUT_CUSTPIN].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[TINPUT_CUSTPIN].element_id = "TINPUT_CUSTPIN";
    demo_control_handles[TINPUT_CUSTPIN].element_handle = 0;
    demo_control_handles[TINPUT_CUSTPIN].cb = cb_ATM_PIN;
/*        Customer Account */
    demo_control_handles[TINPUT_CUSTACCOUNT].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[TINPUT_CUSTACCOUNT].element_id = "TINPUT_CUSTACCOUNT";
    demo_control_handles[TINPUT_CUSTACCOUNT].element_handle = 0;
    demo_control_handles[TINPUT_CUSTACCOUNT].cb = cb_ATM_RDONLY_EDITSTR;
/*        Customer Balance */
    demo_control_handles[TINPUT_CUSTBALANCE].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[TINPUT_CUSTBALANCE].element_id = "TINPUT_CUSTBALANCE";
    demo_control_handles[TINPUT_CUSTBALANCE].element_handle = 0;
    demo_control_handles[TINPUT_CUSTBALANCE].cb = cb_ATM_RDONLY_EDITSTR;
/* ATM Control Console  */
    demo_control_handles[DIV_FORMCONTROL].element_type = HTML_DIV_ELEMENT;
    demo_control_handles[DIV_FORMCONTROL].element_id = "DIV_FORMCONTROL";
    demo_control_handles[DIV_FORMCONTROL].element_handle = 0;
    demo_control_handles[DIV_FORMCONTROL].cb = 0;
/*          ATM Control Console:Card Insert  */
    demo_control_handles[BUTTON_INSERT].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_INSERT].element_id = "BUTTON_INSERT";
    demo_control_handles[BUTTON_INSERT].element_handle = 0;
    demo_control_handles[BUTTON_INSERT].cb = cb_ATM_CONTROL_BUTTON;
/*          ATM Control Console:Bio ID Scan  */
    demo_control_handles[BUTTON_BIOID].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_BIOID].element_id = "BUTTON_BIOID";
    demo_control_handles[BUTTON_BIOID].element_handle = 0;
    demo_control_handles[BUTTON_BIOID].cb = cb_ATM_CONTROL_BUTTON;
/*          ATM Control Console:Access Cash  */
    demo_control_handles[IMAGE_BIOFACE].element_type = HTML_IMAGE_ELEMENT;
    demo_control_handles[IMAGE_BIOFACE].element_id = "IMAGE_BIOFACE";
    demo_control_handles[IMAGE_BIOFACE].element_handle = 0;
    demo_control_handles[IMAGE_BIOFACE].cb = 0;
/*          ATM Control Console:Finish  */
    demo_control_handles[BUTTON_FINISH].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_FINISH].element_id = "BUTTON_FINISH";
    demo_control_handles[BUTTON_FINISH].element_handle = 0;
    demo_control_handles[BUTTON_FINISH].cb = cb_ATM_CONTROL_BUTTON;
/*          ATM Control Console:Cancel */
    demo_control_handles[BUTTON_CANCEL].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_CANCEL].element_id = "BUTTON_CANCEL";
    demo_control_handles[BUTTON_CANCEL].element_handle = 0;
    demo_control_handles[BUTTON_CANCEL].cb = cb_ATM_CONTROL_BUTTON;
/* ATM Help Window */
    demo_control_handles[DIV_FORMHELPBOXL].element_type = HTML_DIV_ELEMENT;
    demo_control_handles[DIV_FORMHELPBOXL].element_id = "DIV_FORMHELPBOXL";
    demo_control_handles[DIV_FORMHELPBOXL].element_handle = 0;
    demo_control_handles[DIV_FORMHELPBOXL].cb = 0;
          /* == */
    demo_control_handles[TEXTBOX_HELP].element_type = HTML_EDITBOX_ELEMENT;
    demo_control_handles[TEXTBOX_HELP].element_id = "TEXTBOX_HELP";
    demo_control_handles[TEXTBOX_HELP].element_handle = 0;
    demo_control_handles[TEXTBOX_HELP].cb = cb_ATM_HELP_BOX;
/* ATM Transaction Selector */
    demo_control_handles[DIV_TRANSACTIONTYPE].element_type = HTML_DIV_ELEMENT;
    demo_control_handles[DIV_TRANSACTIONTYPE].element_id = "DIV_TRANSACTIONTYPE";
    demo_control_handles[DIV_TRANSACTIONTYPE].element_handle = 0;
    demo_control_handles[DIV_TRANSACTIONTYPE].cb = 0;
/*      ATM Transaction Selector: Radio deposit */
    demo_control_handles[RADIOBUTTON_DEPOSIT].element_type = HTML_RADIO_BUTTON_ELEMENT;
    demo_control_handles[RADIOBUTTON_DEPOSIT].element_id = "RADIOBUTTON_DEPOSIT";
    demo_control_handles[RADIOBUTTON_DEPOSIT].element_handle = 0;
    demo_control_handles[RADIOBUTTON_DEPOSIT].cb = cb_ATM_RADIO_BUTTON;
/*      ATM Transaction Selector: Radio WithDraw */
    demo_control_handles[RADIOBUTTON_WITHDRAWAL].element_type = HTML_RADIO_BUTTON_ELEMENT;
    demo_control_handles[RADIOBUTTON_WITHDRAWAL].element_id = "RADIOBUTTON_WITHDRAWAL";
    demo_control_handles[RADIOBUTTON_WITHDRAWAL].element_handle = 0;
    demo_control_handles[RADIOBUTTON_WITHDRAWAL].cb = cb_ATM_RADIO_BUTTON;
    demo_control_handles[DIV_TRANSACTIONAMOUNT].element_type = HTML_DIV_ELEMENT;
/* ATM Transaction Detail: */
    demo_control_handles[DIV_TRANSACTIONAMOUNT].element_id = "DIV_TRANSACTIONAMOUNT";
    demo_control_handles[DIV_TRANSACTIONAMOUNT].element_handle = 0;
    demo_control_handles[DIV_TRANSACTIONAMOUNT].cb = 0;
/*      ATM Transaction Detail: Deposit Input Edit Control */
    demo_control_handles[TINPUT_DEPOSIT].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[TINPUT_DEPOSIT].element_id = "TINPUT_DEPOSIT";
    demo_control_handles[TINPUT_DEPOSIT].element_handle = 0;
    demo_control_handles[TINPUT_DEPOSIT].cb = cb_ATM_INPUT_EDITSTR;
/*      ATM Transaction Detail: Withdrawal Input Edit Control */
    demo_control_handles[TINPUT_WITHDRAW].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[TINPUT_WITHDRAW].element_id = "TINPUT_WITHDRAW";
    demo_control_handles[TINPUT_WITHDRAW].element_handle = 0;
    demo_control_handles[TINPUT_WITHDRAW].cb = cb_ATM_INPUT_EDITSTR;
/* ATM KeyPad Control */
    demo_control_handles[DIV_KEYPAD].element_type = HTML_DIV_ELEMENT;
    demo_control_handles[DIV_KEYPAD].element_id = "DIV_KEYPAD";
    demo_control_handles[DIV_KEYPAD].element_handle = 0;
    demo_control_handles[DIV_KEYPAD].cb = 0;
/*      ATM KeyPad Control: Row 1 */
    demo_control_handles[BUTTON_7].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_7].element_id = "BUTTON_7";
    demo_control_handles[BUTTON_7].element_handle = 0;
    demo_control_handles[BUTTON_7].cb = cb_ATM_KEYPAD_BUTTON;
          /* == */
    demo_control_handles[BUTTON_8].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_8].element_id = "BUTTON_8";
    demo_control_handles[BUTTON_8].element_handle = 0;
    demo_control_handles[BUTTON_8].cb = cb_ATM_KEYPAD_BUTTON;
          /* == */
    demo_control_handles[BUTTON_9].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_9].element_id = "BUTTON_9";
    demo_control_handles[BUTTON_9].element_handle = 0;
    demo_control_handles[BUTTON_9].cb = cb_ATM_KEYPAD_BUTTON;
          /* == */
    demo_control_handles[BUTTON_LEFT].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_LEFT].element_id = "BUTTON_LEFT";
    demo_control_handles[BUTTON_LEFT].element_handle = 0;
    demo_control_handles[BUTTON_LEFT].cb = cb_ATM_KEYPAD_CONTROLKEY;
          /* == */
/*      ATM KeyPad Control: Row 2 */
    demo_control_handles[BUTTON_4].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_4].element_id = "BUTTON_4";
    demo_control_handles[BUTTON_4].element_handle = 0;
    demo_control_handles[BUTTON_4].cb = cb_ATM_KEYPAD_BUTTON;
          /* == */
    demo_control_handles[BUTTON_5].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_5].element_id = "BUTTON_5";
    demo_control_handles[BUTTON_5].element_handle = 0;
    demo_control_handles[BUTTON_5].cb = cb_ATM_KEYPAD_BUTTON;
          /* == */
    demo_control_handles[BUTTON_6].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_6].element_id = "BUTTON_6";
    demo_control_handles[BUTTON_6].element_handle = 0;
    demo_control_handles[BUTTON_6].cb = cb_ATM_KEYPAD_BUTTON;
          /* == */
    demo_control_handles[BUTTON_DEL].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_DEL].element_id = "BUTTON_DEL";
    demo_control_handles[BUTTON_DEL].element_handle = 0;
    demo_control_handles[BUTTON_DEL].cb = cb_ATM_KEYPAD_CONTROLKEY;
/*      ATM KeyPad Control: Row 3 */
    demo_control_handles[BUTTON_1].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_1].element_id = "BUTTON_1";
    demo_control_handles[BUTTON_1].element_handle = 0;
    demo_control_handles[BUTTON_1].cb = cb_ATM_KEYPAD_BUTTON;
          /* == */
    demo_control_handles[BUTTON_2].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_2].element_id = "BUTTON_2";
    demo_control_handles[BUTTON_2].element_handle = 0;
    demo_control_handles[BUTTON_2].cb = cb_ATM_KEYPAD_BUTTON;
          /* == */
    demo_control_handles[BUTTON_3].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_3].element_id = "BUTTON_3";
    demo_control_handles[BUTTON_3].element_handle = 0;
    demo_control_handles[BUTTON_3].cb = cb_ATM_KEYPAD_BUTTON;
          /* == */
    demo_control_handles[BUTTON_OK].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_OK].element_id = "BUTTON_OK";
    demo_control_handles[BUTTON_OK].element_handle = 0;
    demo_control_handles[BUTTON_OK].cb = cb_ATM_KEYPAD_CONTROLKEY;
/*      ATM KeyPad Control: Row 4 */
    demo_control_handles[BUTTON_0].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_0].element_id = "BUTTON_0";
    demo_control_handles[BUTTON_0].element_handle = 0;
    demo_control_handles[BUTTON_0].cb = cb_ATM_KEYPAD_BUTTON;
          /* == */
    demo_control_handles[BUTTON_DOT].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[BUTTON_DOT].element_id = "BUTTON_DOT";
    demo_control_handles[BUTTON_DOT].element_handle = 0;
    demo_control_handles[BUTTON_DOT].cb = cb_ATM_KEYPAD_BUTTON;

}

static char * demo_control_html_strings[] = {
"<html>",
"<body>",
"<div id=DIV_CUSTINFO style=position:absolute;left:10;top:20>",
"<table>",
"<tr><td>Name:</td><td><input type=text style=background-color:DarkGray ID=TINPUT_CUSTNAME></td></tr>",
"<tr><td>Pin:</td><td><input type=text style=background-color:DarkGray ID=TINPUT_CUSTPIN></td></tr>",
"<tr><td>Account:</td><td><input type=text style=background-color:DarkGray ID=TINPUT_CUSTACCOUNT></td></tr>",
"<tr><td>Balance:</td><td><input type=text style=background-color:DarkGray ID=TINPUT_CUSTBALANCE></td></tr>",
"</table>",
"</div>",
"<div id=DIV_FORMCONTROL style=position:absolute;left:220;top:0>",
"<input type=button ID=BUTTON_INSERT value=\"INSERT CARD\"><br>",
"<input type=button ID=BUTTON_BIOID  value=\"BIOMETRIC ID\"><br>",
"<input type=button ID=BUTTON_FINISH value=\"FINISH\"><br>",
"<input type=button ID=BUTTON_CANCEL value=\"CANCEL\"><br>",
"</div>",
"<div id=DIV_FORMHELPBOXL style=position:absolute;left:360;top:0>",
"<textarea cols=10 rows=10 ID=TEXTBOX_HELP>",
"Help Goes here",
"</textarea>",
"</div>",
"<div style=position:absolute;left:460;top:0>",
"<img src=webc://ebslogo.png style=visibility:hidden ID=IMAGE_BIOFACE>",
"</div>",
"<div id=DIV_TRANSACTIONTYPE style=position:absolute;left:10;top:160>",
"<input type=radio name=transactiontype style=background-color:DarkGray ID=RADIOBUTTON_DEPOSIT    value=DEPOSIT>Deposit<br>",
"<input type=radio name=transactiontype style=background-color:DarkGray ID=RADIOBUTTON_WITHDRAWAL value=WITHDRAW>Withdraw<br>",
"</div>",
"<div id=DIV_TRANSACTIONAMOUNT style=position:absolute;left:10;top:220>",
"<table>",
"<tr><td>Amount For Deposit:</td><td><input type=text style=background-color:DarkGray ID=TINPUT_DEPOSIT></td></tr>",
"<tr><td>Amount To Withdraw:</td><td><input type=text style=background-color:DarkGray ID=TINPUT_WITHDRAW></td></tr>",
"</table>",
"</div>",
"<div id=DIV_KEYPAD style=font-family:times;font-size:80%;color:DarkBlue style=position:absolute;left:360;top:200>",
"<table>",
"<tr>",
" <td><input type=button ID=BUTTON_7    value=7></td>",
" <td><input type=button ID=BUTTON_8    value=8></td>",
" <td><input type=button ID=BUTTON_9    value=9></td>",
" <td><input type=button ID=BUTTON_LEFT value=\" < \"></td></tr>",
"<tr>",
" <td><input type=button ID=BUTTON_4    value=4></td>",
" <td><input type=button ID=BUTTON_5    value=5></td>",
" <td><input type=button ID=BUTTON_6    value=6></td>",
" <td><input type=button ID=BUTTON_DEL  value=DEL></td></tr>",
"<tr>",
" <td><input type=button ID=BUTTON_1    value=1></td>",
" <td><input type=button ID=BUTTON_2    value=2></td>",
" <td><input type=button ID=BUTTON_3    value=3></td>",
" <td><input type=button ID=BUTTON_OK   value=\"OK \"></td></tr>",
"<tr>",
" <td><input type=button ID=BUTTON_0    value=0></td>",
" <td><input type=button ID=BUTTON_DOT  value=\".\"></td></tr>",
"</table>",
"</div>",
"</body>",
"</html>",
0
};

static char **get_demo_control_html_strings()
{
    return(demo_control_html_strings);
}
