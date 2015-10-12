#include "webc_ctrl_demo.h"
/*
webc_ctrl_demo_viewer.cpp

    void webc_control_demo_viewer(void)
        Demonstrate using multiple webc API calls to implement a file system browser

    Note: This demo currently only rns on windows platforms

*/

#define EMULATE_FILESYSTEM 1
#define RTPLATFORM_FILESYSTEM 1
#include "rtpchar.h"

#if (EMULATE_FILESYSTEM && WEBC_SUPPORT_INTERNAL)
#include "webvfile.hpp"
#endif

WEBC_BOOL use_real_filesytem = WEBC_FALSE;
WEBC_BOOL use_rollover_mode = WEBC_FALSE;
static void select_fs_to_view(void);

#define DEFAULT_WIDTH  8
#define DEFAULT_HEIGHT 20
#define DEFAULT_XORIGIN 0
#define DEFAULT_YORIGIN 0
#define DEFAULT_VIEWER_XORIGIN 140
#define DEFAULT_VIEWER_YORIGIN 20


#define SUBDIR_CONTROL_STR      "SUBDIR"
#define FILE_CONTROL_STR        "FILE"
#define FILEDROP_CONTROL_STR    "FILEDROP"

#define ISDOT                0
#define ISDOTDOT             1
#define ISSUBDIRECTORY       2
#define ISFILEGROUP          3
#define ISFILE               4
#define MAX_DIR_ENTRY_TYPES  5
#define FTYPE_HTML           0
#define FTYPE_JPEG           1
#define FTYPE_PNG            2
#define FTYPE_GIF            3
#define MAX_FILE_ENTRY_TYPES 4

/* For identifying file groups to expand */
char *file_type_tabs[MAX_FILE_ENTRY_TYPES] = {"HTM", "JPG", "PNG", "GIF"};
/* Suffixes for file groups to expand */
char *file_type_ident[MAX_FILE_ENTRY_TYPES] = {"htm", "jpg", "png", "gif"};
char *file_type_alt_ident[MAX_FILE_ENTRY_TYPES] = {"html", "jpeg", "png", "gif"};

char *expanded_direntry_color  = "color:White;background-color:Black";
char *collapsed_direntry_color = "color:Black;background-color:White";
char *highlighted_direntry_color = "color:White;background-color:Black";
char *expanded_filegroup_color  = "color:White;background-color:Black";
char *collapsed_filegroup_color = "color:Black;background-color:White";
char *highlighted_filegroup_color = "color:White;background-color:Black";
char *expanded_file_color   = "color:White;background-color:Black";
char *collapsed_file_color  = "color:Black;background-color:White";
char *highlighted_file_color  = "color:White;background-color:Black";

char *html_open_tag = "<html>\n";
char *html_close_tag = "</html>\n";
char *body_open_tag = "<body>\n";
char *body_close_tag = "</body>\n";
static int current_emit_control_id;

typedef struct dir_entry {
    struct dir_entry *parent;
    struct dir_entry *pprev;
    struct dir_entry *pnext;
    int    id_int;
    HELEMENT_HANDLE element;
    int    expand_me;
    int    dirent_type;
    /* If dirent_type == ISSUBDIRECTORY */
    struct dir_entry *psubdirectories;
    struct dir_entry *pfilesbytype[MAX_FILE_ENTRY_TYPES];
    /* If dirent_type == ISFILE */
    int    file_type;
    /* Display components */
    int is_expanded;
    int is_visible;
    int column;
    int row;
    char *pcolor_style;
    /* fname must be last field */
    char direntry_fname[4]; /* must be alloced and freed */
} DIR_ENTRY;

static DIR_ENTRY *load_root_directory(char *rootpath);
static DIR_ENTRY * load_sub_directory(DIR_ENTRY *pparent,DIR_ENTRY *prevsib,char *path, char *fname);
static int set_subdirectory_display_element_styles(int start_column, int start_row, int show_self, DIR_ENTRY *pentry);
static void emit_directory_display_elements(DIR_ENTRY *pentry);
static void view_file_contents(DIR_ENTRY *pentry);
static void view_internal_file_contents(DIR_ENTRY *pentry);

long first_visible_column;

static DIR_ENTRY *current_display_root;
DIR_ENTRY *current_load_root;
/* Generic Demo setup step 3: Declare individual callback functions we will use to over-ride some default
   characteristics of individual controls */

/* Generic Demo setup step 4: Initialize our control handle array html with values to match
  against the "id=" properties in the html and the control type
  Documentation moment: List the types
*/
static HTMLEventStatus cb_file_browser (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);


/* Generic Demo setup step 6: Provide a demo entry point. This will be called
   from the dispatcher in webc_ctrl_demo_main.c
    The argument may be
        START_DEMO - To start a demo
        STOP_DEMO  - To stop a demo if necessary

    Most likey only the name of the demo entry point will need to be changed */
static int load_directory(char *rootpath, char *name, DIR_ENTRY *parent);
static void display_directory(HDOC_HANDLE hDoc, DIR_ENTRY *pdirectory);
static void free_entry_list(DIR_ENTRY *pentry);

char windows_root_path[512];
void select_fs_to_view(void);

void webc_control_demo_viewer(int command)
{
    if (command == START_DEMO)
    {
        /* Select the file system to view and run */
        select_fs_to_view();
    } else if (command == STOP_DEMO)
    {
        /* Put any necesseray stop code here */
    }
}
static int populate_sub_directory(DIR_ENTRY *pentry, char *path, char *fname);
static void populate_internal_sub_directory(DIR_ENTRY *pentry, char *path, char *fname);

#define Y_INCREMENT 20;
int y_pos;
void do_it(char *rootpath)
{

    first_visible_column = 0;
        current_load_root = load_sub_directory(0,0, 0, rootpath);
    if (current_load_root)
    {
        current_load_root->is_expanded = 1;
        set_subdirectory_display_element_styles(0,0,1,current_load_root);
        emit_directory_display_elements(current_load_root);
//        webc_control_demo_animate_fullscreen('+');
    }
}

/* ==== Begincallback  section */
static char *path_to_root(DIR_ENTRY *pentry, char *pbuffer);
static void free_sub_directory_contents(DIR_ENTRY *pentry);
static void highlight_directory_entry(DIR_ENTRY *pentry, WEBC_BOOL do_highlight);

int find_deepest_entry(DIR_ENTRY *pentry, int deepest)
{
DIR_ENTRY *psubs;
int ldeepest, mydepth;

    mydepth = deepest;

    psubs = pentry->psubdirectories;
    while (psubs)
    {
        if (psubs->is_expanded)
        {
            ldeepest = find_deepest_entry(psubs, mydepth+1);
            if (ldeepest > deepest)
                deepest = ldeepest;
        }
        psubs = psubs->pnext;
    }
    return(deepest);
}


static HTMLEventStatus cb_file_browser (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
WEBC_BOOL repaint = WEBC_FALSE;
DIR_ENTRY *pentry;

    switch (event->type)
    {
        case HTML_EVENT_CLICK:
        {
            pentry = (DIR_ENTRY *) webc_ElementGetPrivateData (hElement);
            if (!pentry)
                break;
            webc_control_demo_hide_viewframe();
            if (pentry->dirent_type == ISDOT)
                break;
            else if (pentry->dirent_type == ISDOTDOT)
                break;
            else if (pentry->dirent_type == ISSUBDIRECTORY)
            {
                if (!pentry->is_expanded)
                {
                char path_buffer[512];
                    if (use_real_filesytem)
                    {
                        populate_sub_directory(pentry, path_to_root(pentry, path_buffer),
                                            pentry->direntry_fname);
                    }
                    else
                    {
                        populate_internal_sub_directory(pentry, path_to_root(pentry, path_buffer),
                                            pentry->direntry_fname);
                    }
                    pentry->is_expanded = 1;
                }
                else
                {
                    pentry->is_expanded = 0;
                    free_sub_directory_contents(pentry);
                }
                repaint = WEBC_TRUE;
                break;
            }
            else if (pentry->dirent_type == ISFILEGROUP)
            {
                if (pentry->is_expanded)
                {
                    pentry->is_expanded = 0;
                }
                else
                {
                    pentry->is_expanded = 1;
                }
                repaint = WEBC_TRUE;
                break;
            }
            else if (pentry->dirent_type == ISFILE)
            {
                if (use_real_filesytem)
                    view_file_contents(pentry);
                else
                    view_internal_file_contents(pentry);

                return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
            }
        }
        break;
        case HTML_EVENT_MOUSEOVER:
        {
            pentry = (DIR_ENTRY *) webc_ElementGetPrivateData (hElement);
            if (pentry)
            {
                highlight_directory_entry(pentry, WEBC_TRUE);
                if (use_rollover_mode && pentry->dirent_type == ISFILE)
                {
                    if (use_real_filesytem)
                        view_file_contents(pentry);
                    else
                        view_internal_file_contents(pentry);
                }
                return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
            }
            return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
            break;
        }
        case HTML_EVENT_MOUSEOUT:
        {
            pentry = (DIR_ENTRY *) webc_ElementGetPrivateData (hElement);
            if (pentry)
                highlight_directory_entry(pentry, WEBC_FALSE);
            return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
            break;
        }

        default:
        break;
        case HTML_EVENT_OBJECT_CREATED:
            break;
    }
    if (repaint)
    {
       int deepest;
       deepest = find_deepest_entry(current_load_root,0);
       if (deepest >= 3)
        first_visible_column = deepest - 3;
       else
        first_visible_column = 0;

       /* reload html.. return HALT so no more processing is done on the current */
       set_subdirectory_display_element_styles(0,0,1,current_load_root);
       emit_directory_display_elements(current_load_root);
       return(HTML_EVENT_STATUS_HALT); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

/* Each of the HTML_EDIT_STR_ELEMENT we created has a unique ID for 1 to
   current_emit_control_id,
   for each HTML_EDIT_STR_ELEMENT,
   See if matches one of our IDs and if so:
    Attach a common event handler
    Store the address of the directory_entry structure in the HTML elements private data slot
    Save the hadle to the HTML control in the directory_entry structure
*/

static WEBC_BOOL bind_directory_entry_by_id (HELEMENT_HANDLE hElement);
static void clear_dirent_ids(void);

static void bind_webc_to_html(void)
{
    HELEMENT_HANDLE hElement;
    /* Clear the html id bindings, we will reload them */
    for (int which_element = 0; ;which_element++)
    {
       hElement = webc_DocFindElement (wcdContext.hDemoDoc, 0, 0, HTML_EDIT_STR_ELEMENT, which_element);
       if (!hElement)
           break;
       /* If we find an entry associated with this control we exchange pointers
          and assign an event handler */
        if (bind_directory_entry_by_id(hElement))
            webc_ElementSetTagEventHandler (hElement, cb_file_browser);
    }
    return;
}

static DIR_ENTRY *find_dirent_by_id_in_subdir(DIR_ENTRY *psubdir,int myID_int);
static void clear_dirent_subdir_ids(DIR_ENTRY *psubdir);
static void clear_dirent_in_list(DIR_ENTRY *plist)
{
    while (plist)
    {
        plist->id_int = 0;
        if (plist->dirent_type == ISSUBDIRECTORY)
        {
            clear_dirent_subdir_ids(plist);
        }
        plist = plist->pnext;
    }
}
static void clear_dirent_subdir_ids(DIR_ENTRY *psubdir)
{
    if (!psubdir)
        return;
    psubdir->id_int = 0;
    clear_dirent_in_list(psubdir->psubdirectories);
   {
        for (int i = 0; i < MAX_FILE_ENTRY_TYPES; i++)
        {
            clear_dirent_in_list(psubdir->pfilesbytype[i]);
        }
    }

}
static void clear_dirent_ids(void)
{
    clear_dirent_subdir_ids(current_load_root);
}

static DIR_ENTRY *find_dirent_by_id_in_list(DIR_ENTRY *plist,int myID_int)
{
    while (plist)
    {
        if (plist->id_int == myID_int)
            return(plist);
        if (plist->dirent_type == ISSUBDIRECTORY)
        {
            DIR_ENTRY *presult;
            presult = find_dirent_by_id_in_subdir(plist,myID_int);
            if (presult)
                return(presult);
        }
        plist = plist->pnext;
    }
    return(0);
}

static DIR_ENTRY *find_dirent_by_id_in_subdir(DIR_ENTRY *psubdir,int myID_int)
{
static DIR_ENTRY *presult;
    if (!psubdir)
        return(0);
    /* Check self */
    if (psubdir->id_int == myID_int)
        return(psubdir);

    /* Scan the directory list, this will recurse until all subentries are scanned */
    presult = find_dirent_by_id_in_list(psubdir->psubdirectories, myID_int);
    if (!presult)
    { /* Not found, check file groups */
        for (int i = 0; i < MAX_FILE_ENTRY_TYPES; i++)
        {
            presult = find_dirent_by_id_in_list(psubdir->pfilesbytype[i], myID_int);
            if (presult)
                break;
        }
    }
    return(presult);
}
static WEBC_BOOL bind_directory_entry_by_id (HELEMENT_HANDLE hElement)
{
    char *myId;
    myId = webc_ElementGetIdASCII(hElement);
    if (myId)
    {
        int myID_int;
        DIR_ENTRY *matched_entry;
        myID_int = webc_control_demo_ascii_to_int(myId);
        webc_FreeASCIIString(myId);
        matched_entry =  find_dirent_by_id_in_subdir(current_load_root,myID_int);
        if (matched_entry)
        {
            matched_entry->element = hElement;
            webc_ElementSetPrivateData (hElement, (void *)matched_entry);
            return(WEBC_TRUE);
        }
    }
    return(WEBC_FALSE);
}

/* Generic Demo setup step 7: Provide callback routines to customize individual elemnts in
   the html provided in
    static char * demo_control_html = {
*/
/* Build a path string back to the root, not including the curent directory */
static char *path_to_root(DIR_ENTRY *pentry, char *pbuffer)
{
#define MAX_DEPTH 64 /* huge we hope */
DIR_ENTRY *parents[MAX_DEPTH];
int top_parent;
char *presult;

    presult = pbuffer;
    top_parent = 0;
    parents[top_parent] = pentry->parent;
    while(parents[top_parent]->parent)
    {
        parents[top_parent+1] = parents[top_parent]->parent;
        top_parent += 1;
    }
    *presult = 0;
    while(top_parent >= 0 && parents[top_parent])
    {
        rtp_strcat(presult, parents[top_parent]->direntry_fname);
        if (top_parent)
            rtp_strcat(presult, "\\");
        top_parent--;
    }
    return(pbuffer);
}

static void highlight_directory_entry(DIR_ENTRY *pentry, WEBC_BOOL do_highlight)
{
char *thestyle = 0;
    if (do_highlight)
    {
        if (pentry->dirent_type == ISSUBDIRECTORY)
            thestyle = highlighted_direntry_color;
        else if (pentry->dirent_type == ISFILEGROUP)
            thestyle = highlighted_filegroup_color;
        else if (pentry->dirent_type == ISFILE)
            thestyle = highlighted_file_color;
    }
    else
    {
        if (pentry->dirent_type == ISSUBDIRECTORY)
        {
            if (pentry->is_expanded)
                thestyle = expanded_direntry_color;
            else
                thestyle = collapsed_direntry_color;
        }
        else if (pentry->dirent_type == ISFILEGROUP)
        {
            if (pentry->is_expanded)
                thestyle = expanded_filegroup_color;
            else
                thestyle = collapsed_filegroup_color;
        }
        else if (pentry->dirent_type == ISFILE)
            thestyle = collapsed_file_color;
    }
    if (thestyle)
        webc_ElementSetStyleASCII(pentry->element, thestyle,WEBC_TRUE);
//    if (do_highlight)
//    {
//        HEREHERE - expand
//    }
//    else
//    {
//        HEREHERE - collapse
//
//   }

}

/* ==== End callback  section */


/* ==== Begin emit_ section */
static void set_entry_display_element_style(DIR_ENTRY *pentry);
static void set_display_element_color(DIR_ENTRY *pentry);

static int set_subdirectory_display_element_styles(int start_column, int start_row, int show_self, DIR_ENTRY *pentry)
{
DIR_ENTRY *ptempentry;
int current_row;

    if (!pentry->is_visible)
        return(0);

    current_row = start_row;
    if (show_self)
    {
            pentry->column = start_column;
            /* Don't increment row if not visible */
            if (start_column >= first_visible_column)
                pentry->row = current_row++;
            set_display_element_color(pentry);
    }
    /* Now do subdirectories and file groups */
    if (!pentry->is_expanded)
        return (current_row - start_row);
    /* Display subdirectories */
    ptempentry = pentry->psubdirectories;
    while (ptempentry)
    {
        ptempentry->column = start_column;
        ptempentry->row = current_row;
        set_entry_display_element_style(ptempentry);
        if (start_column >= first_visible_column)
            current_row++;
        if (ptempentry->is_expanded)
            current_row += set_subdirectory_display_element_styles(start_column+1, current_row, 0,ptempentry);
        ptempentry = ptempentry->pnext;
    }
    /* Display files by group */
    for (int i= 0; i <MAX_FILE_ENTRY_TYPES;i++)
    {
        ptempentry = pentry->pfilesbytype[i];
        if (ptempentry)
        {
            /* Display the tab */
            if (start_column >= first_visible_column)
                ptempentry->row = current_row++;
            ptempentry->column = start_column;
            set_entry_display_element_style(ptempentry);
            /* Display the files */
            if (ptempentry->is_expanded)
            {
                ptempentry = ptempentry->pnext;
                while (ptempentry)
                {
                    if (start_column >= first_visible_column)
                        ptempentry->row = current_row++;
                    ptempentry->column = start_column+1;
                    set_entry_display_element_style(ptempentry);
                    ptempentry = ptempentry->pnext;
                }
            }
        }
    }
    return (current_row - start_row);
}

static void set_entry_display_element_style(DIR_ENTRY *pentry)
{
    set_display_element_color(pentry);
}

static void set_display_element_color(DIR_ENTRY *pentry)
{
    if (pentry->dirent_type == ISSUBDIRECTORY)
    {
        if (pentry->is_expanded)
            pentry->pcolor_style =  expanded_direntry_color;
        else
            pentry->pcolor_style =  collapsed_direntry_color;
    }
    else if (pentry->dirent_type == ISFILEGROUP)
    {
        if (pentry->is_expanded)
            pentry->pcolor_style =  expanded_filegroup_color;
        else
            pentry->pcolor_style =  collapsed_filegroup_color;
    }
    else
        pentry->pcolor_style =  collapsed_file_color;
}



static void continue_emit_directory_display_elements(DIR_ENTRY *pentry);

void emit_tag(HDOC_HANDLE hDoc, char *tag)
{
    webc_DocWriteHtml(hDoc, tag, rtp_strlen(tag), WEBC_FALSE);
}


static void emit_entry(DIR_ENTRY *pentry);
static void emit_entry_contents(DIR_ENTRY *pentry);

static void emit_directory_display_elements(DIR_ENTRY *pentry)
{
    current_display_root = pentry;
    clear_dirent_ids();
    webc_DocClear(wcdContext.hDemoDoc, WEBC_FALSE);
    webc_DocOpen(wcdContext.hDemoDoc);
    emit_tag(wcdContext.hDemoDoc,html_open_tag);
    emit_tag(wcdContext.hDemoDoc,body_open_tag);

    /* We'll use a counter to give each control an individual id */
    current_emit_control_id = 1;

    if (!pentry->is_visible)
        return;
    emit_entry_contents(pentry);


    emit_tag(wcdContext.hDemoDoc,body_close_tag);
    emit_tag(wcdContext.hDemoDoc,html_close_tag);

    webc_DocClose(wcdContext.hDemoDoc);
    webc_DocRefresh(wcdContext.hDemoDoc);
    /* Now bind the html to C callback routines and data structures */
    bind_webc_to_html();

}

static void emit_entry_contents(DIR_ENTRY *pentry)
{
DIR_ENTRY *ptempentry;
    if (pentry->is_expanded)
    {
        /* Display subdirectories */
        ptempentry = pentry->psubdirectories;
        while (ptempentry)
        {
           emit_entry(ptempentry);
           if (ptempentry->is_expanded)
           {
               ptempentry->column = pentry->column + 1;
               emit_entry_contents(ptempentry);
           }
           ptempentry = ptempentry->pnext;
        }
    }
    /* Display files by group */
    for (int i= 0; i <MAX_FILE_ENTRY_TYPES;i++)
    {
        ptempentry = pentry->pfilesbytype[i];
        if (ptempentry)
        {
            /* Display the tab */
            emit_entry(ptempentry);
            /* Display the files */
            if (ptempentry->is_expanded)
            {
                ptempentry = ptempentry->pnext;
                while (ptempentry)
                {
                    emit_entry(ptempentry);
                    ptempentry = ptempentry->pnext;
                }
            }
        }
    }
}

static char *alloc_format_buff(void){ return( (char *) WEBC_MALLOC(1024));}
static void free_format_buff(char *format_buff){    WEBC_FREE(format_buff);}

static void emit_entry(DIR_ENTRY *pentry)
{
int  xpos_px, ypos_px;
char *format_buff;
int nwritten;

    if (pentry->column < first_visible_column)
        return;

    xpos_px = DEFAULT_XORIGIN + ((pentry->column-first_visible_column) * DEFAULT_WIDTH);
    ypos_px = DEFAULT_YORIGIN + (pentry->row * DEFAULT_HEIGHT);

    /* Assign a unique id to this html control so we can find it from a generic
       event handler that is shared by all controls */
    pentry->id_int = current_emit_control_id++;

    format_buff = alloc_format_buff();
    rtp_sprintf(format_buff,
        "<input type=text id=\"%d\" value=\"%s\" style=\"%s;position:absolute;left:%dpx;top:%dpx;\"><br>\n",
        pentry->id_int,
        pentry->direntry_fname,
        pentry->pcolor_style,
        xpos_px,
        ypos_px);
    nwritten = webc_DocWriteHtml(wcdContext.hDemoDoc, format_buff, rtp_strlen(format_buff), WEBC_FALSE);
    free_format_buff(format_buff);
}

static DIR_ENTRY * allocate_sub_directory(DIR_ENTRY *pparent,DIR_ENTRY *prevsib,char *fname, int dirent_type, int file_type, int visible);


static DIR_ENTRY * load_sub_directory(DIR_ENTRY *pparent,DIR_ENTRY *prevsib, char *path, char *fname)
{
DIR_ENTRY *pentry;
    pentry = allocate_sub_directory(0,0, fname, ISSUBDIRECTORY, 0, 1);
    if (!pentry)
        return(0);
    if (use_real_filesytem)
    {
        if (!populate_sub_directory(pentry, path, fname))
        {
            free_entry_list(pentry);
            return(0);
        }
    }
    else
        populate_internal_sub_directory(pentry, path, fname);
    return(pentry);
}




static DIR_ENTRY * allocate_sub_directory(DIR_ENTRY *pparent,DIR_ENTRY *prevsib,char *fname, int dirent_type, int file_type, int visible)
{
int size_of_entry;
DIR_ENTRY *pentry,*return_entry;

    return_entry = prevsib;

    size_of_entry = sizeof(*pentry)+rtp_strlen(fname);
    pentry = (DIR_ENTRY *) WEBC_MALLOC(size_of_entry);
    if (!pentry)
        return(0);
    rtp_memset(pentry, 0, size_of_entry);
    pentry->parent = pparent;
    if (prevsib)
    {
        while(prevsib->pnext)
            prevsib = prevsib->pnext;
        prevsib->pnext = pentry;
    }
    pentry->pprev   = prevsib;
    pentry->dirent_type = dirent_type;
    pentry->file_type   = file_type;
    rtp_strcpy(pentry->direntry_fname, fname);
    pentry->is_visible = visible;
    if (!return_entry)
        return_entry = pentry;
    return(return_entry);
}

static void free_entry_list(DIR_ENTRY *pentry)
{
DIR_ENTRY *pcurr, *pnext;

    pcurr = pentry;
    while(pcurr)
    {
        pnext = pcurr->pnext;
        WEBC_FREE(pcurr);
        pcurr = pnext;
    }
}

static void free_sub_directory_contents(DIR_ENTRY *pentry)
{
    if (!pentry)
        return;
    if (pentry->dirent_type == ISSUBDIRECTORY)
    {
        free_entry_list(pentry->psubdirectories);
        pentry->psubdirectories = 0;
        for (int i= 0; i <MAX_FILE_ENTRY_TYPES;i++)
        {
            free_entry_list(pentry->pfilesbytype[i]);
            pentry->pfilesbytype[i] = 0;
        }
    }
}

static int check_for_file_type(char *fname)
{
char *pto, *pfrom;
char lcfname[256];
int i;

    /* Lower case */
    pfrom = fname;
    pto = lcfname;
    do {
        *pto = rtp_tolower(*pfrom);
        pfrom++;
        pto++;
    } while (*pfrom);
    *pto = 0;
    for (i = 0; i < MAX_FILE_ENTRY_TYPES; i++)
    {
        /* Look for our file types */
        if (rtp_strstr(lcfname, file_type_ident[i]) || rtp_strstr(lcfname, file_type_alt_ident[i]))
            return(i);
    }
    return(-1);
}




static int check_for_file_type(char *fname);


#if (EMULATE_FILESYSTEM)
#if (WEBC_SUPPORT_INTERNAL)
static char *get_internal_file_info(int n, long *pdata_length, WEBC_PFBYTE **pdata)
{
    return(wload_WebcGetFileName(n, pdata_length, pdata));
}

static void view_internal_file_contents(DIR_ENTRY *pentry)
{
            char url_string[128];
            long  xpos_px = DEFAULT_VIEWER_XORIGIN;
            long  ypos_px = DEFAULT_VIEWER_YORIGIN;

   webc_control_demo_draw_viewframe(xpos_px, ypos_px);

  rtp_sprintf(url_string, "webc://%s", pentry->direntry_fname);
  webc_DocSetUrl(wcdContext.hViewerDoc, url_string, WEBC_TRUE);
  webc_DocSetUrl(wcdContext.hViewerDoc, url_string, WEBC_FALSE);
}
#else
char *filenames[] = {"testhtml.html","testgif.gif", "testpng.png", "testjpg.jpg", 0};
static char *get_internal_file_info(int n, long *pdata_length, WEBC_PFBYTE **pdata)
{
    *pdata_length = 0;
    *pdata = 0;
    return(filenames[n]);
}
static void view_internal_file_contents(DIR_ENTRY *pentry)
{
}
#endif

static void populate_internal_sub_directory(DIR_ENTRY *pentry, char *path, char *fname)
{

    rtp_strcpy(pentry->direntry_fname, fname);

    {
    int i, entry_type = -1;
    int entry_file_type = -1;

        pentry->psubdirectories = allocate_sub_directory(pentry, pentry->psubdirectories,"ASUBIRECTORY",ISSUBDIRECTORY,0,1);
        for (i = 0; ; i++)
        {
        long data_length;
        WEBC_PFBYTE *pdata;
        char *file_name;
        int file_type;
            file_name = get_internal_file_info(i, &data_length, &pdata);
            if (!file_name)
                break;
            file_type = check_for_file_type(file_name);
            if (file_type >= 0)
            {
                /* Insert a tab if we need one */
                if (!pentry->pfilesbytype[file_type])
                {
                    pentry->pfilesbytype[file_type] =
                        allocate_sub_directory(pentry, 0,
                            file_type_tabs[file_type], ISFILEGROUP, file_type,1);
                }
                /* Insert the file */
                pentry->pfilesbytype[file_type] =
                        allocate_sub_directory(pentry, pentry->pfilesbytype[file_type], file_name,ISFILE,file_type,0);
            }
        }
    }
}

#endif

static void view_file_contents(DIR_ENTRY *pentry)
{
   char path_buffer[256];
   char url_string[256];


   long  xpos_px = DEFAULT_VIEWER_XORIGIN;
   long  ypos_px = DEFAULT_VIEWER_YORIGIN;

   webc_control_demo_draw_viewframe(xpos_px, ypos_px);

  path_to_root(pentry, path_buffer);
  rtp_sprintf(url_string, "file://%s\\%s", path_buffer, pentry->direntry_fname);
  webc_DocSetUrl(wcdContext.hViewerDoc, url_string, WEBC_TRUE);
  webc_DocSetUrl(wcdContext.hViewerDoc, url_string, WEBC_FALSE);
}


#if (RTPLATFORM_FILESYSTEM)

#include "rtpdobj.h"

static int populate_sub_directory(DIR_ENTRY *pentry, char *path, char *fname)
{
void *prtp_statobj;
char pathbuff[512];
char statobj_fname[128];
unsigned char statobj_attrib;

    pathbuff[0] = 0;
    if (path)
    {
        rtp_strcpy(pathbuff, path);
        rtp_strcat(pathbuff,"\\");
    }
    rtp_strcat(pathbuff,fname);
    rtp_strcat(pathbuff,"\\*.*");

    rtp_strcpy(pentry->direntry_fname, fname);

    if (rtp_file_gfirst (&prtp_statobj, (char *)pathbuff) < 0)
    {
        return(0);
    }

    do
    {
    int entry_type = -1;
    int entry_file_type = -1;
        rtp_file_get_attrib (prtp_statobj, &statobj_attrib);
        rtp_file_get_name (prtp_statobj, statobj_fname, 128);

        if (rtp_strcmp(".", statobj_fname)==0)
            ; //pentry->psubdirectories = allocate_sub_directory(pentry, pentry->psubdirectories,statobj.name,ISDOT,0,1);
        else if (rtp_strcmp("..", statobj_fname)==0)
            ; //pentry->psubdirectories = allocate_sub_directory(pentry, pentry->psubdirectories,statobj.name,ISDOTDOT,0,1);
        else if( statobj_attrib & RTP_FILE_ATTRIB_ISDIR)
            pentry->psubdirectories = allocate_sub_directory(pentry, pentry->psubdirectories,statobj_fname,ISSUBDIRECTORY,0,1);
        else
        {
            int file_type;
            file_type = check_for_file_type(statobj_fname);
            if (file_type >= 0)
            {

                /* Insert a tab if we need one */
                if (!pentry->pfilesbytype[file_type])
                {
                    pentry->pfilesbytype[file_type] =
                        allocate_sub_directory(pentry, 0,
                            file_type_tabs[file_type], ISFILEGROUP, file_type,1);
                }
                /* Insert the file */
                pentry->pfilesbytype[file_type] =
                        allocate_sub_directory(pentry, pentry->pfilesbytype[file_type], statobj_fname,ISFILE,file_type,0);
            }
        }
    } while (rtp_file_gnext(prtp_statobj) == 0);
    rtp_file_gdone(prtp_statobj);
    return(1);
}
#endif (RTPLATFORM_FILESYSTEM)

static char * demo_control_html_strings[] = {
"<html>",
"<body>",
"<bold>Select files to view, webC internal or local disk files</bold><br>",
"<input type=radio checked name=fstype ID=RADIOBUTTON_INTERNAL    value=INTERNAL>View webC compiled in internal files<br>",
"<input type=radio name=fstype ID=RADIOBUTTON_NATIVE      value=NATIVE>View disk files<br>",
"<br><br><br>",
"<bold>If Disk files are selected, enter root path here </bold><br>",
"<bold>Type for example: C: or \\server\\content </bold><br>",
"Root Search Path:<input type=text value=\"C:\" ID=ROOT_PATH>",
"<br><br><bold>Click to continue</bold><br>",
"<input type=radio name=frollover ID=RADIOBUTTON_ROLLOVER    value=ROLLOVER>View content on rollover, otherwise view content on click<br>",
"<input type=button ID=BUTTON_PROCEED value=\"PROCEED\"><br>",
"</body>",
"</html>",
0
};
static char **get_demo_control_html_strings()
{
    return(demo_control_html_strings);
}

/* Select internal fs or windows */
#define RADIOBUTTON_INTERNAL    0
#define RADIOBUTTON_NATIVE      1
#define ROOT_PATH               2
#define PROCEED                 3
#define RADIOBUTTON_ROLLOVER    4
#define DEMO_CONTROL_COUNT      5
/* Generic Demo setup step 2: Declare an array to hold the handles */
static WEBC_CONTROL_DEMO_HANDLE demo_control_handles[DEMO_CONTROL_COUNT];
static HTMLEventStatus cb_FS_SELECT_ROLLOVER_MODE (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_FS_SELECT_RADIO_BUTTON (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_FS_SELECT_ROOT_PATH (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static HTMLEventStatus cb_FS_PROCEED (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param);
static void init_demo_control_handles(void)
{
    webc_control_demo_init_control_array(&demo_control_handles[0],DEMO_CONTROL_COUNT);

    demo_control_handles[RADIOBUTTON_INTERNAL].element_type = HTML_RADIO_BUTTON_ELEMENT;
    demo_control_handles[RADIOBUTTON_INTERNAL].element_id = "RADIOBUTTON_INTERNAL";
    demo_control_handles[RADIOBUTTON_INTERNAL].element_handle = 0;
    demo_control_handles[RADIOBUTTON_INTERNAL].cb = cb_FS_SELECT_RADIO_BUTTON;

    demo_control_handles[RADIOBUTTON_NATIVE].element_type = HTML_RADIO_BUTTON_ELEMENT;
    demo_control_handles[RADIOBUTTON_NATIVE].element_id = "RADIOBUTTON_NATIVE";
    demo_control_handles[RADIOBUTTON_NATIVE].element_handle = 0;
    demo_control_handles[RADIOBUTTON_NATIVE].cb = cb_FS_SELECT_RADIO_BUTTON;

    demo_control_handles[ROOT_PATH].element_type = HTML_EDIT_STR_ELEMENT;
    demo_control_handles[ROOT_PATH].element_id = "ROOT_PATH";
    demo_control_handles[ROOT_PATH].element_handle = 0;
    demo_control_handles[ROOT_PATH].cb = cb_FS_SELECT_ROOT_PATH;

    demo_control_handles[RADIOBUTTON_ROLLOVER].element_type = HTML_RADIO_BUTTON_ELEMENT;
    demo_control_handles[RADIOBUTTON_ROLLOVER].element_id = "RADIOBUTTON_ROLLOVER";
    demo_control_handles[RADIOBUTTON_ROLLOVER].element_handle = 0;
    demo_control_handles[RADIOBUTTON_ROLLOVER].cb = cb_FS_SELECT_ROLLOVER_MODE;


    demo_control_handles[PROCEED].element_type = HTML_INPUT_BUTTON_ELEMENT;
    demo_control_handles[PROCEED].element_id = "BUTTON_PROCEED";
    demo_control_handles[PROCEED].element_handle = 0;
    demo_control_handles[PROCEED].cb = cb_FS_PROCEED;

}

static HTMLEventStatus cb_FS_SELECT_ROLLOVER_MODE (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    switch (event->type)
    {
        default:
            return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
        case HTML_EVENT_CLICK:
        case HTML_EVENT_KEYDOWN:
            break;
    }
    use_rollover_mode = WEBC_TRUE;
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}


static HTMLEventStatus cb_FS_SELECT_RADIO_BUTTON (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    switch (event->type)
    {
        default:
            return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
        case HTML_EVENT_CLICK:
        case HTML_EVENT_KEYDOWN:
            break;
    }
    if (demo_control_handles[RADIOBUTTON_NATIVE].element_handle ==  hElement)
        use_real_filesytem = WEBC_TRUE;
    else
        use_real_filesytem = WEBC_FALSE;
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

static HTMLEventStatus cb_FS_SELECT_ROOT_PATH (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    switch (event->type)
    {
        case HTML_EVENT_CHANGE:
        {
            char *control_contents;
   control_contents = "C:\\dev\\ebs\\webc\\v2.6.5candidate\\webc\\v2.6.5\\webc_exploration";
   printf("Set root to %s\n", control_contents);
   rtp_strcpy(windows_root_path, control_contents);
   return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
            control_contents = webc_ElementGetValueASCII(hElement);
            if (control_contents)
            {
                rtp_strcpy(windows_root_path, control_contents);
                webc_FreeASCIIString(control_contents);
            }
        }
        break;
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

static HTMLEventStatus cb_FS_PROCEED (HBROWSER_HANDLE hBrowser,HDOC_HANDLE hDoc,HELEMENT_HANDLE hElement,HTMLEvent* event, char* param)
{
    switch (event->type)
    {
        case HTML_EVENT_CLICK:
            do_it(windows_root_path);
            return(HTML_EVENT_STATUS_DONE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
    }
    return(HTML_EVENT_STATUS_CONTINUE); /* or HTML_EVENT_STATUS_DONE or HTML_EVENT_STATUS_HALT */
}

static void select_fs_to_view(void)
{
        rtp_strcpy(windows_root_path, "C:\\");
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
        /* The callback will start the process */
}
