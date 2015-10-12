/*
|  WEBVFILE.CPP -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|  Module description:
|   [tbd]
*/


//New

#include "util.h"
#include "webvfile.hpp"
#include "webload.hpp"
#include "webcstr.h"
#include "webcmem.h"


#if (WEBC_SUPPORT_INTERNAL)
// Virtual Files

#ifdef __cplusplus
extern "C" {
#endif

#include "welcome.h"
#include "ebslogo.h"
#include "browsericon.h"
//#include "ebslogo_bmp.h"
//#include "welcome_bmp.h"
//#include "ebslogo_bmp_html.h"
//#include "browsericon_bmp.h"

#define WEBC_CFG_INCLUDE_SOFTKEYPAD 1

#if (WEBC_CFG_INCLUDE_SOFTKEYPAD)
#include "EBS_SOFTKEY_CAPITALS.h"
#include "EBS_SOFTKEY_NUMBERS.h"
#include "EBS_SOFTKEY_SMALLSS.h"
#include "EBS_SOFTKEY_SYMBOLS.h"
#endif

#if (WEBC_CFG_ENABLE_CHROME)
#include "alert_small.h"
#include "archive.h"
#include "archive_large.h"
#include "archive_selected.h"
#include "archives.h"
#include "back_default.h"
#include "back_disable.h"
#include "back_over.h"
#include "back_halo.h"
#include "back_over_halo.h"
#include "bookmarks.h"
#include "bookmark_default.h"
#include "bookmarks_large.h"
#include "bookmark_selected.h"
#include "bottom_border.h"
#include "brushedmetal.h"
#include "brushedmetal_side.h"
#include "chrome.h"
#include "document.h"
#include "forward_default.h"
#include "forward_disable.h"
#include "forward_over.h"
#include "forward_halo.h"
#include "forward_over_halo.h"
#include "home_default.h"
#include "home_over.h"
#include "home_halo.h"
#include "home_over_halo.h"
#include "loadChrome.h"
#include "offline.h"
#include "offline_disabled.h"
#include "offline_toggle.h"
#include "online.h"
#include "online_disabled.h"
#include "online_toggle.h"
#include "prompt_small.h"
#include "refresh_default.h"
#include "refresh_over.h"
#include "refresh_halo.h"
#include "refresh_over_halo.h"
#include "savetoarchive.h"
#include "savetoarchive_disabled.h"
#include "search_bottom.h"
#include "search_left.h"
#include "search_left_menu.h"
#include "search_right.h"
#include "search_top.h"
#include "small_back.h"
#include "small_forward.h"
#include "small_refresh.h"
#include "smallGlobe.h"
#include "smallGlobe_gray.h"
#include "stop_default.h"
#include "stop_disable.h"
#include "stop_over.h"
#include "stop_halo.h"
#include "stop_over_halo.h"
#include "stripes.h"
#include "top_border.h"
#include "url_left.h"
#include "url_right.h"
#include "close_default.h"
#include "close_over.h"

#include "chrome_pda.h"
#include "loadChrome_pda.h"
#include "chrome_pda_small.h"
#include "loadChrome_pda_small.h"
#include "bookmarks_pda.h"
#include "archives_pda.h"

#endif // WEBC_CFG_ENABLE_CHROME

#ifdef __TSDPROJECT__

#if (WEBC_SUPPORT_FILE == 0)
#define INCLUDE_MOTO_HOME
#endif
#endif
#ifdef INCLUDE_MOTO_HOME

/* 12 */
#include "home.h"
#include "applicationDock1.h"
#include "nrf_style.h"
#include "Button_3wide.h"
#include "Help_Button.h"
#include "Home_Header.h"
#include "Kill_App_Toggle_Off.h"
#include "Launcher_Button.h"
#include "Left_Button.h"
#include "Right_Button.h"
#include "SelectButton.h"
#include "Settings_Button.h"

/* 27 more */

#include "badge.h"
#include "communicating.h"
//#include "initializing.h"
#include "login.h"
#include "MWFM_Header.h"
#include "Arrow_Button_1x2.h"
#include "AlphaButton_Special_Hyphen.h"
#include "AlphaButton_Special_Space.h"
#include "AlphaButton_Special_Backspace.h"
#include "AlphaButton_ABC.h"
#include "AlphaButton_DEF.h"
#include "AlphaButton_GHI.h"
#include "AlphaButton_JKL.h"
#include "AlphaButton_MNO.h"
#include "AlphaButton_PQRS.h"
#include "AlphaButton_TUV.h"
#include "AlphaButton_WXYZ.h"
#include "Numeric_Button_0.h"
#include "Numeric_Button_9.h"
#include "Numeric_Button_1.h"
#include "Numeric_Button_2.h"
#include "Numeric_Button_3.h"
#include "Numeric_Button_4.h"
#include "Numeric_Button_5.h"
#include "Numeric_Button_6.h"
#include "Numeric_Button_7.h"
#include "Numeric_Button_8.h"

//Added by MA
#include "Button_6wide.h"
#include "Empty_Header.h"
#include "NumericButton_123.h"
#include "Scan_Button.h"
#include "Prompt_4Wide.h"
#include "Arrow_Button_1x2.h"




//#define HOMEEXTRA (12+26) // 28 when all 9 /* 12 */
// #define HOMEEXTRA (20+26) // 28 when all 9 /* 12 */ edited by MA
#define HOMEEXTRA (20+24) // Take out badge and loading
#else
#define HOMEEXTRA 0
#endif

#ifdef __TSDPROJECT__
#include "mock.h"
#include "mock1.h"
#include "mock2.h"
#include "mock3.h"
#include "jscriptcounter.h"
#include "regulartexttest.h"
#include "boldtexttest.h"



#define EXTRA (7)+HOMEEXTRA
#else
#define EXTRA HOMEEXTRA
#endif


#ifdef __cplusplus
}
#endif

#if (WEBC_CFG_INCLUDE_SOFTKEYPAD)
#define SOFTKEYPADEXTRA 4
#else
#define SOFTKEYPADEXTRA 0
#endif
#if (WEBC_CFG_ENABLE_CHROME)
#define NUM_VFILE_ENTRIES       127+EXTRA+SOFTKEYPADEXTRA
#else
#define NUM_VFILE_ENTRIES       8+EXTRA+SOFTKEYPADEXTRA
#endif

// Virtual Table
struct WebcVFileEntry gWebcVFileTable[NUM_VFILE_ENTRIES] =

{
#ifdef __TSDPROJECT__
	{"mock.html",		       HTML_MIME_TYPE_HTML, MOCK_SIZE,            mock_file,               0, 0, WEBC_CHARSET_ASCII},
	{"mock1.html",		       HTML_MIME_TYPE_HTML, MOCK1_SIZE,            mock1_file,               0, 0, WEBC_CHARSET_ASCII},
	{"mock2.html",		       HTML_MIME_TYPE_HTML, MOCK2_SIZE,            mock2_file,               0, 0, WEBC_CHARSET_ASCII},
	{"mock3.html",		       HTML_MIME_TYPE_HTML, MOCK3_SIZE,            mock3_file,               0, 0, WEBC_CHARSET_ASCII},
	{"jscriptcounter.html",    HTML_MIME_TYPE_HTML,  JSCRIPTCOUNTER_SIZE,  jscriptcounter_file,     0, 0, WEBC_CHARSET_ASCII},
	{"regulartexttest.html",   HTML_MIME_TYPE_HTML, REGULARTEXTTEST_SIZE,  regulartexttest_file,     0, 0, WEBC_CHARSET_ASCII},
	{"boldtexttest.html",      HTML_MIME_TYPE_HTML, BOLDTEXTTEST_SIZE,     boldtexttest_file,        0, 0, WEBC_CHARSET_ASCII},
#endif

#ifdef INCLUDE_MOTO_HOME


#if (0)
{"badge.bmp" ,         HTML_MIME_TYPE_BMP, BADGE_SIZE,         badge_file        ,0,0,WEBC_CHARSET_UNKNOWN},
{"communicating.bmp" , HTML_MIME_TYPE_BMP, COMMUNICATING_SIZE, communicating_file,0,0,WEBC_CHARSET_UNKNOWN},
{"initializing.bmp" , HTML_MIME_TYPE_BMP, INITIALIZING_SIZE, initializing_file,0,0,WEBC_CHARSET_UNKNOWN},
#endif

{"home.html" , HTML_MIME_TYPE_HTML, HOME_SIZE, home_file,0,0,WEBC_CHARSET_ASCII},
{"applicationDock1.html" , HTML_MIME_TYPE_HTML, APPLICATIONDOCK1_SIZE, applicationdock1_file,0,0,WEBC_CHARSET_ASCII},
{"nrf_style.css" , HTML_MIME_TYPE_CSS, NRF_STYLE_SIZE, nrf_style_file,0,0,WEBC_CHARSET_ASCII},

#if (1)
{"Button_3wide.bmp" , HTML_MIME_TYPE_BMP, BUTTON_3WIDE_SIZE, button_3wide_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Help_Button.bmp" , HTML_MIME_TYPE_BMP, HELP_BUTTON_SIZE, help_button_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Home_Header.bmp" , HTML_MIME_TYPE_BMP, HOME_HEADER_SIZE, home_header_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Kill_App_Toggle_Off.bmp" , HTML_MIME_TYPE_BMP, KILL_APP_TOGGLE_OFF_SIZE, kill_app_toggle_off_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Launcher_Button.bmp" , HTML_MIME_TYPE_BMP, LAUNCHER_BUTTON_SIZE, launcher_button_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Left_Button.bmp" , HTML_MIME_TYPE_BMP, LEFT_BUTTON_SIZE, left_button_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Right_Button.bmp" , HTML_MIME_TYPE_BMP, RIGHT_BUTTON_SIZE, right_button_file,0,0,WEBC_CHARSET_UNKNOWN},
{"SelectButton.bmp" , HTML_MIME_TYPE_BMP, SELECTBUTTON_SIZE, selectbutton_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Settings_Button.bmp" , HTML_MIME_TYPE_BMP, SETTINGS_BUTTON_SIZE, settings_button_file,0,0,WEBC_CHARSET_UNKNOWN},
{"login.html" , HTML_MIME_TYPE_HTML, LOGIN_SIZE, login_file,0,0,WEBC_CHARSET_ASCII},
{"MWFM_Header.bmp", HTML_MIME_TYPE_BMP, MWFM_HEADER_SIZE, mwfm_header_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Arrow_Button_1x2.bmp", HTML_MIME_TYPE_BMP, ARROW_BUTTON_1X2_SIZE, arrow_button_1x2_file, 0,0, WEBC_CHARSET_UNKNOWN},
#else
{"Button_3wide.png" , HTML_MIME_TYPE_PNG, BUTTON_3WIDE_SIZE, button_3wide_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Help_Button.png" , HTML_MIME_TYPE_PNG, HELP_BUTTON_SIZE, help_button_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Home_Header.png" , HTML_MIME_TYPE_PNG, HOME_HEADER_SIZE, home_header_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Kill_App_Toggle_Off.png" , HTML_MIME_TYPE_PNG, KILL_APP_TOGGLE_OFF_SIZE, kill_app_toggle_off_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Launcher_Button.png" , HTML_MIME_TYPE_PNG, LAUNCHER_BUTTON_SIZE, launcher_button_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Left_Button.png" , HTML_MIME_TYPE_PNG, LEFT_BUTTON_SIZE, left_button_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Right_Button.png" , HTML_MIME_TYPE_PNG, RIGHT_BUTTON_SIZE, right_button_file,0,0,WEBC_CHARSET_UNKNOWN},
{"SelectButton.png" , HTML_MIME_TYPE_PNG, SELECTBUTTON_SIZE, selectbutton_file,0,0,WEBC_CHARSET_UNKNOWN},
{"Settings_Button.png" , HTML_MIME_TYPE_PNG, SETTINGS_BUTTON_SIZE, settings_button_file,0,0,WEBC_CHARSET_UNKNOWN},
{"login.html" , HTML_MIME_TYPE_HTML, LOGIN_SIZE, login_file,0,0,WEBC_CHARSET_ASCII},
{"MWFM_Header.png", HTML_MIME_TYPE_PNG, MWFM_HEADER_SIZE, mwfm_header_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Arrow_Button_1x2.png", HTML_MIME_TYPE_PNG, ARROW_BUTTON_1X2_SIZE, arrow_button_1x2_file, 0,0, WEBC_CHARSET_UNKNOWN},
#endif

#if (1)
{"AlphaButton_Special_Hyphen.bmp", HTML_MIME_TYPE_BMP, ALPHABUTTON_SPECIAL_HYPHEN_SIZE, alphabutton_special_hyphen_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_Special_Space.bmp", HTML_MIME_TYPE_BMP,  ALPHABUTTON_SPECIAL_SPACE_SIZE, alphabutton_special_space_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_Special_Backspace.bmp", HTML_MIME_TYPE_BMP,  ALPHABUTTON_SPECIAL_BACKSPACE_SIZE, alphabutton_special_backspace_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_ABC.bmp", HTML_MIME_TYPE_BMP,  ALPHABUTTON_ABC_SIZE,  alphabutton_abc_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_DEF.bmp", HTML_MIME_TYPE_BMP,  ALPHABUTTON_DEF_SIZE,  alphabutton_def_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_GHI.bmp", HTML_MIME_TYPE_BMP,  ALPHABUTTON_GHI_SIZE,  alphabutton_ghi_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_JKL.bmp", HTML_MIME_TYPE_BMP,  ALPHABUTTON_JKL_SIZE,  alphabutton_jkl_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_MNO.bmp", HTML_MIME_TYPE_BMP,  ALPHABUTTON_MNO_SIZE,  alphabutton_mno_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_PQRS.bmp", HTML_MIME_TYPE_BMP, ALPHABUTTON_PQRS_SIZE, alphabutton_pqrs_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_TUV.bmp", HTML_MIME_TYPE_BMP,  ALPHABUTTON_TUV_SIZE,  alphabutton_tuv_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_WXYZ.bmp", HTML_MIME_TYPE_BMP, ALPHABUTTON_WXYZ_SIZE, alphabutton_wxyz_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_0.bmp", HTML_MIME_TYPE_BMP, NUMERIC_BUTTON_0_SIZE, numeric_button_0_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_9.bmp", HTML_MIME_TYPE_BMP, NUMERIC_BUTTON_9_SIZE, numeric_button_9_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_1.bmp", HTML_MIME_TYPE_BMP, NUMERIC_BUTTON_1_SIZE, numeric_button_1_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_2.bmp", HTML_MIME_TYPE_BMP, NUMERIC_BUTTON_2_SIZE, numeric_button_2_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_3.bmp", HTML_MIME_TYPE_BMP, NUMERIC_BUTTON_3_SIZE, numeric_button_3_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_4.bmp", HTML_MIME_TYPE_BMP, NUMERIC_BUTTON_4_SIZE, numeric_button_4_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_5.bmp", HTML_MIME_TYPE_BMP, NUMERIC_BUTTON_5_SIZE, numeric_button_5_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_6.bmp", HTML_MIME_TYPE_BMP, NUMERIC_BUTTON_6_SIZE, numeric_button_6_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_7.bmp", HTML_MIME_TYPE_BMP, NUMERIC_BUTTON_7_SIZE, numeric_button_7_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_8.bmp", HTML_MIME_TYPE_BMP, NUMERIC_BUTTON_8_SIZE, numeric_button_8_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Button_6wide.bmp", HTML_MIME_TYPE_BMP, BUTTON_6WIDE_SIZE, button_6wide_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Empty_Header.bmp", HTML_MIME_TYPE_BMP, EMPTY_HEADER_SIZE, empty_header_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_ABC.bmp", HTML_MIME_TYPE_BMP, ALPHABUTTON_ABC_SIZE, alphabutton_abc_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"NumericButton_123.bmp", HTML_MIME_TYPE_BMP, NUMERICBUTTON_123_SIZE, numericbutton_123_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Scan_Button.bmp", HTML_MIME_TYPE_BMP, SCAN_BUTTON_SIZE, scan_button_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Prompt_4Wide.bmp", HTML_MIME_TYPE_BMP, PROMPT_4WIDE_SIZE, prompt_4wide_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Arrow_Button_1x2.bmp", HTML_MIME_TYPE_BMP, ARROW_BUTTON_1X2_SIZE, numeric_button_8_file, 0,0, WEBC_CHARSET_UNKNOWN},

#else
{"AlphaButton_Special_Hyphen.png", HTML_MIME_TYPE_PNG, ALPHABUTTON_SPECIAL_HYPHEN_SIZE, alphabutton_special_hyphen_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_Special_Space.png", HTML_MIME_TYPE_PNG,  ALPHABUTTON_SPECIAL_SPACE_SIZE, alphabutton_special_space_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_Special_Backspace.png", HTML_MIME_TYPE_PNG,  ALPHABUTTON_SPECIAL_BACKSPACE_SIZE, alphabutton_special_backspace_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_ABC.png", HTML_MIME_TYPE_PNG,  ALPHABUTTON_ABC_SIZE,  alphabutton_abc_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_DEF.png", HTML_MIME_TYPE_PNG,  ALPHABUTTON_DEF_SIZE,  alphabutton_def_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_GHI.png", HTML_MIME_TYPE_PNG,  ALPHABUTTON_GHI_SIZE,  alphabutton_ghi_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_JKL.png", HTML_MIME_TYPE_PNG,  ALPHABUTTON_JKL_SIZE,  alphabutton_jkl_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_MNO.png", HTML_MIME_TYPE_PNG,  ALPHABUTTON_MNO_SIZE,  alphabutton_mno_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_PQRS.png", HTML_MIME_TYPE_PNG, ALPHABUTTON_PQRS_SIZE, alphabutton_pqrs_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_TUV.png", HTML_MIME_TYPE_PNG,  ALPHABUTTON_TUV_SIZE,  alphabutton_tuv_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"AlphaButton_WXYZ.png", HTML_MIME_TYPE_PNG, ALPHABUTTON_WXYZ_SIZE, alphabutton_wxyz_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_0.png", HTML_MIME_TYPE_PNG, NUMERIC_BUTTON_0_SIZE, numeric_button_0_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_9.png", HTML_MIME_TYPE_PNG, NUMERIC_BUTTON_9_SIZE, numeric_button_9_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_1.png", HTML_MIME_TYPE_PNG, NUMERIC_BUTTON_1_SIZE, numeric_button_1_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_2.png", HTML_MIME_TYPE_PNG, NUMERIC_BUTTON_2_SIZE, numeric_button_2_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_3.png", HTML_MIME_TYPE_PNG, NUMERIC_BUTTON_3_SIZE, numeric_button_3_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_4.png", HTML_MIME_TYPE_PNG, NUMERIC_BUTTON_4_SIZE, numeric_button_4_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_5.png", HTML_MIME_TYPE_PNG, NUMERIC_BUTTON_5_SIZE, numeric_button_5_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_6.png", HTML_MIME_TYPE_PNG, NUMERIC_BUTTON_6_SIZE, numeric_button_6_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_7.png", HTML_MIME_TYPE_PNG, NUMERIC_BUTTON_7_SIZE, numeric_button_7_file, 0,0, WEBC_CHARSET_UNKNOWN},
{"Numeric_Button_8.png", HTML_MIME_TYPE_PNG, NUMERIC_BUTTON_8_SIZE, numeric_button_8_file, 0,0, WEBC_CHARSET_UNKNOWN},
#endif


#endif


	{"welcome.html",           HTML_MIME_TYPE_HTML, WELCOME_SIZE,            welcome_file,            0, 0, WEBC_CHARSET_ASCII},
	{"ebslogo.png",            HTML_MIME_TYPE_PNG,  EBSLOGO_SIZE,            ebslogo_file,            0, 0, WEBC_CHARSET_UNKNOWN},
	{"browsericon.png",        HTML_MIME_TYPE_PNG,  BROWSERICON_SIZE,        browsericon_file,        0, 0, WEBC_CHARSET_UNKNOWN},
#if (WEBC_CFG_INCLUDE_SOFTKEYPAD)
	{"EBS_SOFTKEY_CAPITALS.bmp",        HTML_MIME_TYPE_BMP,  EBS_SOFTKEY_CAPITALS_SIZE,        ebs_softkey_capitals_file,        0, 0, WEBC_CHARSET_UNKNOWN},
	{"EBS_SOFTKEY_SMALLSS.bmp",        HTML_MIME_TYPE_BMP,  EBS_SOFTKEY_SMALLSS_SIZE,        ebs_softkey_smallss_file,        0, 0, WEBC_CHARSET_UNKNOWN},
	{"EBS_SOFTKEY_NUMBERS.bmp",        HTML_MIME_TYPE_BMP,  EBS_SOFTKEY_NUMBERS_SIZE,        ebs_softkey_numbers_file,        0, 0, WEBC_CHARSET_UNKNOWN},
	{"EBS_SOFTKEY_SYMBOLS.bmp",        HTML_MIME_TYPE_BMP,  EBS_SOFTKEY_SYMBOLS_SIZE,        ebs_softkey_symbols_file,        0, 0, WEBC_CHARSET_UNKNOWN},
#endif


#if (WEBC_CFG_ENABLE_CHROME)
	// ui chrome
	{"alert_small.png",        HTML_MIME_TYPE_PNG,  ALERT_SMALL_SIZE,        alert_small_file,        0, 0, WEBC_CHARSET_UNKNOWN},
	{"archive.png",            HTML_MIME_TYPE_PNG,  ARCHIVE_SIZE,            archive_file,            0, 0, WEBC_CHARSET_UNKNOWN},
	{"archive_large.png",      HTML_MIME_TYPE_PNG,  ARCHIVE_LARGE_SIZE,      archive_large_file,      0, 0, WEBC_CHARSET_UNKNOWN},
	{"archive_selected.png",   HTML_MIME_TYPE_PNG,  ARCHIVE_SELECTED_SIZE,   archive_selected_file,   0, 0, WEBC_CHARSET_UNKNOWN},
	{"archives.html",          HTML_MIME_TYPE_HTML, ARCHIVES_SIZE,           archives_file,           0, 0, WEBC_CHARSET_ASCII},
	{"archives_pda.html",      HTML_MIME_TYPE_HTML, ARCHIVES_PDA_SIZE,       archives_pda_file,       0, 0, WEBC_CHARSET_ASCII},
	{"back_default.png",       HTML_MIME_TYPE_PNG,  BACK_DEFAULT_SIZE,       back_default_file,       0, 0, WEBC_CHARSET_UNKNOWN},
	{"back_disable.png",       HTML_MIME_TYPE_PNG,  BACK_DISABLE_SIZE,       back_disable_file,       0, 0, WEBC_CHARSET_UNKNOWN},
	{"back_over.png",          HTML_MIME_TYPE_PNG,  BACK_OVER_SIZE,          back_over_file,          0, 0, WEBC_CHARSET_UNKNOWN},
	{"back_halo.png",          HTML_MIME_TYPE_PNG,  BACK_HALO_SIZE,          back_halo_file,          0, 0, WEBC_CHARSET_UNKNOWN},
	{"back_over_halo.png",     HTML_MIME_TYPE_PNG,  BACK_OVER_HALO_SIZE,     back_over_halo_file,     0, 0, WEBC_CHARSET_UNKNOWN},
	{"bookmarks.html",         HTML_MIME_TYPE_HTML, BOOKMARKS_SIZE,          bookmarks_file,          0, 0, WEBC_CHARSET_ASCII},
	{"bookmarks_pda.html",     HTML_MIME_TYPE_HTML, BOOKMARKS_PDA_SIZE,      bookmarks_pda_file,      0, 0, WEBC_CHARSET_ASCII},
	{"bookmark_default.png",   HTML_MIME_TYPE_PNG,  BOOKMARK_DEFAULT_SIZE,   bookmark_default_file,   0, 0, WEBC_CHARSET_UNKNOWN},
	{"bookmarks_large.png",    HTML_MIME_TYPE_PNG,  BOOKMARKS_LARGE_SIZE,    bookmarks_large_file,    0, 0, WEBC_CHARSET_UNKNOWN},
	{"bookmark_selected.png",  HTML_MIME_TYPE_PNG,  BOOKMARK_SELECTED_SIZE,  bookmark_selected_file,  0, 0, WEBC_CHARSET_UNKNOWN},
	{"bottom_border.png",      HTML_MIME_TYPE_PNG,  BOTTOM_BORDER_SIZE,      bottom_border_file,      0, 0, WEBC_CHARSET_UNKNOWN},
	{"brushedmetal.png",       HTML_MIME_TYPE_PNG,  BRUSHEDMETAL_SIZE,       brushedmetal_file,       0, 0, WEBC_CHARSET_UNKNOWN},
	{"brushedmetal_side.png",  HTML_MIME_TYPE_PNG,  BRUSHEDMETAL_SIDE_SIZE,  brushedmetal_side_file,  0, 0, WEBC_CHARSET_UNKNOWN},
	{"close_default.png",      HTML_MIME_TYPE_PNG,  CLOSE_DEFAULT_SIZE,      close_default_file,      0, 0, WEBC_CHARSET_UNKNOWN},
	{"close_over.png",         HTML_MIME_TYPE_PNG,  CLOSE_OVER_SIZE,         close_over_file,         0, 0, WEBC_CHARSET_UNKNOWN},
	{"chrome.html",            HTML_MIME_TYPE_HTML, CHROME_SIZE,             chrome_file,             0, 0, WEBC_CHARSET_ASCII},
	{"chrome_pda.html",        HTML_MIME_TYPE_HTML, CHROME_PDA_SIZE,         chrome_pda_file,         0, 0, WEBC_CHARSET_ASCII},
	{"chrome_pda_small.html",  HTML_MIME_TYPE_HTML, CHROME_PDA_SMALL_SIZE,         chrome_pda_small_file,         0, 0, WEBC_CHARSET_ASCII},
	{"document.png",           HTML_MIME_TYPE_PNG,  DOCUMENT_SIZE,           document_file,           0, 0, WEBC_CHARSET_UNKNOWN},
	{"forward_default.png",    HTML_MIME_TYPE_PNG,  FORWARD_DEFAULT_SIZE,    forward_default_file,    0, 0, WEBC_CHARSET_UNKNOWN},
	{"forward_disable.png",    HTML_MIME_TYPE_PNG,  FORWARD_DISABLE_SIZE,    forward_disable_file,    0, 0, WEBC_CHARSET_UNKNOWN},
	{"forward_over.png",       HTML_MIME_TYPE_PNG,  FORWARD_OVER_SIZE,       forward_over_file,       0, 0, WEBC_CHARSET_UNKNOWN},
	{"forward_halo.png",       HTML_MIME_TYPE_PNG,  FORWARD_HALO_SIZE,       forward_halo_file,       0, 0, WEBC_CHARSET_UNKNOWN},
	{"forward_over_halo.png",  HTML_MIME_TYPE_PNG,  FORWARD_OVER_HALO_SIZE,  forward_over_halo_file,  0, 0, WEBC_CHARSET_UNKNOWN},
	{"home_default.png",       HTML_MIME_TYPE_PNG,  HOME_DEFAULT_SIZE,       home_default_file,       0, 0, WEBC_CHARSET_UNKNOWN},
	{"home_over.png",          HTML_MIME_TYPE_PNG,  HOME_OVER_SIZE,          home_over_file,          0, 0, WEBC_CHARSET_UNKNOWN},
	{"home_halo.png",          HTML_MIME_TYPE_PNG,  HOME_HALO_SIZE,          home_halo_file,          0, 0, WEBC_CHARSET_UNKNOWN},
	{"home_over_halo.png",     HTML_MIME_TYPE_PNG,  HOME_OVER_HALO_SIZE,     home_over_halo_file,     0, 0, WEBC_CHARSET_UNKNOWN},
	{"loadChrome.html",        HTML_MIME_TYPE_HTML, LOADCHROME_SIZE,         loadchrome_file,         0, 0, WEBC_CHARSET_ASCII},
	{"loadChrome_pda.html",    HTML_MIME_TYPE_HTML, LOADCHROME_PDA_SIZE,     loadchrome_pda_file,     0, 0, WEBC_CHARSET_ASCII},
{"loadChrome_pda_small.html",    HTML_MIME_TYPE_HTML, LOADCHROME_PDA_SMALL_SIZE,     loadchrome_pda_small_file,     0, 0, WEBC_CHARSET_ASCII},

	{"offline.png",            HTML_MIME_TYPE_PNG,  OFFLINE_SIZE,            offline_file,            0, 0, WEBC_CHARSET_ASCII},
	{"offline_disabled.png",   HTML_MIME_TYPE_PNG,  OFFLINE_DISABLED_SIZE,   offline_disabled_file,   0, 0, WEBC_CHARSET_ASCII},
	{"offline_toggle.png",     HTML_MIME_TYPE_PNG,  OFFLINE_TOGGLE_SIZE,     offline_toggle_file,     0, 0, WEBC_CHARSET_ASCII},
	{"online.png",             HTML_MIME_TYPE_PNG,  ONLINE_SIZE,             online_file,             0, 0, WEBC_CHARSET_ASCII},
	{"online_disabled.png",    HTML_MIME_TYPE_PNG,  ONLINE_DISABLED_SIZE,    online_disabled_file,    0, 0, WEBC_CHARSET_ASCII},
	{"online_toggle.png",      HTML_MIME_TYPE_PNG,  ONLINE_TOGGLE_SIZE,      online_toggle_file,      0, 0, WEBC_CHARSET_ASCII},
	{"prompt_small.png",       HTML_MIME_TYPE_PNG,  PROMPT_SMALL_SIZE,       prompt_small_file,       0, 0, WEBC_CHARSET_UNKNOWN},
	{"refresh_default.png",    HTML_MIME_TYPE_PNG,  REFRESH_DEFAULT_SIZE,    refresh_default_file,    0, 0, WEBC_CHARSET_UNKNOWN},
	{"refresh_over.png",       HTML_MIME_TYPE_PNG,  REFRESH_OVER_SIZE,       refresh_over_file,       0, 0, WEBC_CHARSET_UNKNOWN},
	{"refresh_halo.png",       HTML_MIME_TYPE_PNG,  REFRESH_HALO_SIZE,       refresh_halo_file,       0, 0, WEBC_CHARSET_UNKNOWN},
	{"refresh_over_halo.png",  HTML_MIME_TYPE_PNG,  REFRESH_OVER_HALO_SIZE,  refresh_over_halo_file,  0, 0, WEBC_CHARSET_UNKNOWN},
	{"savetoarchive.png",      HTML_MIME_TYPE_PNG,  SAVETOARCHIVE_SIZE,      savetoarchive_file,      0, 0, WEBC_CHARSET_UNKNOWN},
	{"savetoarchive_disabled.png", HTML_MIME_TYPE_PNG, SAVETOARCHIVE_DISABLED_SIZE, savetoarchive_disabled_file, 0, 0, WEBC_CHARSET_UNKNOWN},
	{"search_bottom.png",      HTML_MIME_TYPE_PNG,  SEARCH_BOTTOM_SIZE,      search_bottom_file,      0, 0, WEBC_CHARSET_UNKNOWN},
	{"search_left.png",        HTML_MIME_TYPE_PNG,  SEARCH_LEFT_SIZE,        search_left_file,        0, 0, WEBC_CHARSET_UNKNOWN},
	{"search_left_menu.png",   HTML_MIME_TYPE_PNG,  SEARCH_LEFT_MENU_SIZE,   search_left_menu_file,   0, 0, WEBC_CHARSET_UNKNOWN},
	{"search_right.png",       HTML_MIME_TYPE_PNG,  SEARCH_RIGHT_SIZE,       search_right_file,       0, 0, WEBC_CHARSET_UNKNOWN},
	{"search_top.png",         HTML_MIME_TYPE_PNG,  SEARCH_TOP_SIZE,         search_top_file,         0, 0, WEBC_CHARSET_UNKNOWN},
	{"small_back.png",         HTML_MIME_TYPE_PNG,  SMALL_BACK_SIZE,         small_back_file,         0, 0, WEBC_CHARSET_UNKNOWN},
	{"small_forward.png",      HTML_MIME_TYPE_PNG,  SMALL_FORWARD_SIZE,      small_forward_file,      0, 0, WEBC_CHARSET_UNKNOWN},
	{"small_refresh.png",      HTML_MIME_TYPE_PNG,  SMALL_REFRESH_SIZE,      small_refresh_file,      0, 0, WEBC_CHARSET_UNKNOWN},
	{"smallGlobe.gif",         HTML_MIME_TYPE_GIF,  SMALLGLOBE_SIZE,         smallglobe_file,         0, 0, WEBC_CHARSET_UNKNOWN},
	{"smallGlobe_gray.gif",    HTML_MIME_TYPE_GIF,  SMALLGLOBE_GRAY_SIZE,    smallglobe_gray_file,    0, 0, WEBC_CHARSET_UNKNOWN},
	{"stop_default.png",       HTML_MIME_TYPE_PNG,  STOP_DEFAULT_SIZE,       stop_default_file,       0, 0, WEBC_CHARSET_UNKNOWN},
	{"stop_disable.png",       HTML_MIME_TYPE_PNG,  STOP_DISABLE_SIZE,       stop_disable_file,       0, 0, WEBC_CHARSET_UNKNOWN},
	{"stop_over.png",          HTML_MIME_TYPE_PNG,  STOP_OVER_SIZE,          stop_over_file,          0, 0, WEBC_CHARSET_UNKNOWN},
	{"stop_halo.png",          HTML_MIME_TYPE_PNG,  STOP_HALO_SIZE,          stop_halo_file,          0, 0, WEBC_CHARSET_UNKNOWN},
	{"stop_over_halo.png",     HTML_MIME_TYPE_PNG,  STOP_OVER_HALO_SIZE,     stop_over_halo_file,     0, 0, WEBC_CHARSET_UNKNOWN},
	{"stripes.png",            HTML_MIME_TYPE_PNG,  STRIPES_SIZE,            stripes_file,            0, 0, WEBC_CHARSET_UNKNOWN},
	{"top_border.png",         HTML_MIME_TYPE_PNG,  TOP_BORDER_SIZE,         top_border_file,         0, 0, WEBC_CHARSET_UNKNOWN},
	{"url_left.png",           HTML_MIME_TYPE_PNG,  URL_LEFT_SIZE,           url_left_file,           0, 0, WEBC_CHARSET_UNKNOWN},
	{"url_right.png",          HTML_MIME_TYPE_PNG,  URL_RIGHT_SIZE,          url_right_file,          0, 0, WEBC_CHARSET_UNKNOWN},
   #endif

	{0,WEBC_FILE_TYPE_UNKNOWN,0,0,0,0}
};

//#define NUM_VFILE_ENTRIES (sizeof(gWebcVFileTable)/sizeof(WebcVFileEntry))
#endif


static int _nextUniqueNum = 0;

char *webc_CreateVFile (const char *pName, int iMimeType, long lSize, WEBC_PFBYTE pData, WEBC_UINT16 wFlags, VFileDeleteFn Destructor)
{
#if (WEBC_SUPPORT_INTERNAL)
int n;
char unique_name[20];

	if (!pName)
	{
		tc_strcpy(unique_name, "\xffvfile");
		tc_itoa(_nextUniqueNum, &unique_name[tc_strlen(unique_name)], 10);
		pName = unique_name;
		_nextUniqueNum++;
	}

	for (n=0; n<NUM_VFILE_ENTRIES; n++)
	{
		if (!gWebcVFileTable[n].pName)
		{
			gWebcVFileTable[n].pName = webc_malloc_string_copy_8(pName, __FILE__, __LINE__);
			gWebcVFileTable[n].iMimeType = (WebcFileContentType) iMimeType;
			gWebcVFileTable[n].lSize = lSize;
			if (wFlags & WEBC_VFILE_COPY)
			{
				gWebcVFileTable[n].pData = (WEBC_PFBYTE) WEBC_MALLOC(lSize);
				if (!gWebcVFileTable[n].pData)
				{
					gWebcVFileTable[n].lSize = 0;
				}
				else
				{
					tc_movebytes(gWebcVFileTable[n].pData, pData, lSize);
				}
			}
			else
			{
				gWebcVFileTable[n].pData = pData;
			}
			gWebcVFileTable[n].wFlags = wFlags | WEBC_VFILE_CREATED;
			gWebcVFileTable[n].Destroy = Destructor;
			return (gWebcVFileTable[n].pName);
		}
	}
#endif

	return 0;
}

int webc_DeleteVFile (const char *pName)
{
#if (WEBC_SUPPORT_INTERNAL)
int n;

	for (n=0; n<NUM_VFILE_ENTRIES; n++)
	{
		if (gWebcVFileTable[n].pName && !tc_stricmp(pName, gWebcVFileTable[n].pName))
		{
			if (gWebcVFileTable[n].wFlags & WEBC_VFILE_CREATED)
			{
				webc_free_string_copy_8(gWebcVFileTable[n].pName, __FILE__, __LINE__);
			}

			if (gWebcVFileTable[n].wFlags & WEBC_VFILE_COPY)
			{
				WEBC_FREE(gWebcVFileTable[n].pData);
				gWebcVFileTable[n].pData = 0;
			}

			if (gWebcVFileTable[n].Destroy)
			{
				gWebcVFileTable[n].Destroy(&gWebcVFileTable[n]);
			}

			gWebcVFileTable[n].pName = 0;
			break;
		}
	}
#endif

	return (0);
}

/* Easy access to contents of the virtual file system, pass in an
   index number and it returns the name, length and a pointer to the
   data. If the index is beyond of the end of the file table returns NULL */
char *wload_WebcGetFileName(int n, long *pdata_length, WEBC_PFBYTE **pdata)
{// Noone uses this but keep it here to fix later..
#if (WEBC_SUPPORT_INTERNAL)
	if (n<NUM_VFILE_ENTRIES)
    {
        *pdata_length = gWebcVFileTable[n].lSize;
        *pdata = &gWebcVFileTable[n].pData;
		return(gWebcVFileTable[n].pName);
    }
#endif
	return (0);

}
//static struct WebcVFileEntry * webc_TestOpenVFile(struct WebcVFileEntry *pWebcFsRoot,int RootFolderFileCount, WEBC_CHAR *Path);
static const struct WebcVFileEntry * webcVfFindInInstalledTables(WEBC_CHAR *Path, int *indexintable);

int wload_WebcGet(UrlStreamCtx *pStream)
{
#if (WEBC_SUPPORT_INTERNAL)
int n;
//webc_TestOpenVFile(gWebcVFileTable,NUM_VFILE_ENTRIES,pStream->p_desc->getPath());
	for (n=0; n<NUM_VFILE_ENTRIES; n++)
	{
		if (gWebcVFileTable[n].pName && !webc_c_stricmp(pStream->p_desc->getPath(), gWebcVFileTable[n].pName))
		{
			pStream->stream.webc.vfileTable = (void *)gWebcVFileTable;
			pStream->stream.webc.vfile = n;
			pStream->stream.webc.pos = 0;
			return (0);
		}
	}
	/* Didn't find it in the legacy table, try installed tables */
	int indexintable;
	pStream->stream.webc.vfileTable = (void *) webcVfFindInInstalledTables(pStream->p_desc->getPath(),&indexintable);
	if (pStream->stream.webc.vfileTable)
	{
		pStream->stream.webc.vfile = indexintable;
		pStream->stream.webc.pos = 0;
		return (0);
	}

#endif
	return (-1);
}

int wload_WebcPost(UrlStreamCtx *pStream, WEBC_CHAR *pContentType, long lContentLength)
{
	return (-1);
}

long wload_WebcRead(UrlStreamCtx *pStream, char * buffer, long length)
{
#if (WEBC_SUPPORT_INTERNAL)
long vfsize;
WEBC_PFBYTE vfbuf;
struct WebcVFileEntry *FileTable = (struct WebcVFileEntry  *) pStream->stream.webc.vfileTable;



	vfsize = FileTable[pStream->stream.webc.vfile].lSize;
	vfbuf = &FileTable[pStream->stream.webc.vfile].pData[pStream->stream.webc.pos];

	length = EBSMIN(vfsize - pStream->stream.webc.pos, length);
	tc_movebytes(buffer, vfbuf, length);
	pStream->stream.webc.pos += length;

	return (length);
#else
	return (-1);
#endif
}

long wload_WebcWrite(UrlStreamCtx *pStream, char * buffer, long length)
{
	return (-1);
}

WebcFileContentType wload_WebcDataType(UrlStreamCtx *pStream)
{
#if (WEBC_SUPPORT_INTERNAL)
	return (((struct WebcVFileEntry  *) pStream->stream.webc.vfileTable)[pStream->stream.webc.vfile].iMimeType);
#else
	return (WEBC_FILE_TYPE_UNKNOWN);
#endif
}

int wload_WebcClose(UrlStreamCtx *pStream)
{
	return (0);
}

enum WebcCharset wload_WebcCharset(UrlStreamCtx *pStream)
{
  #if (WEBC_SUPPORT_INTERNAL)
	return (((struct WebcVFileEntry  *) pStream->stream.webc.vfileTable)[pStream->stream.webc.vfile].charset);
  #else
	return (WEBC_DEFAULT_CHARSET);
  #endif
}


// *******************************

#define TESTNEWVFILE 1
#if (TESTNEWVFILE)


#define WEBCVFMAXDEPTH 64

struct WebcVTableContainer {
const struct WebcVFileEntry * AssignedTable;
int EntriesInTable;
};
struct vector gWebcVTableContainerVector;


// Called once at startup by webC_init();
void wload_WebcVirtualFileSystemInitialize(void)
{
static int webcVfInitialized=0;
	vector_init(&gWebcVTableContainerVector, sizeof(WebcVTableContainer));
}

#define MAXPOSSIBLETABLESIZE 1000000
// Install a file system, return 0 or -1 on failure
int wload_WebcVirtualFileSystemTableInstall(const struct WebcVFileEntry *NewTable)
{
	// Check the table to be sure it is properly formed and null terminated
	const struct WebcVFileEntry *Iterator = NewTable;
	int tablesize=0;
	while(Iterator->pName && tablesize < MAXPOSSIBLETABLESIZE)
	{
		tablesize++;
		Iterator++;
	}
	if (tablesize == MAXPOSSIBLETABLESIZE)
		return -1;
	// Copy the container into the vector.
	struct WebcVTableContainer c;
	c.AssignedTable = NewTable;
	c.EntriesInTable=tablesize;
	return vector_add_node_to_rear(&gWebcVTableContainerVector, (WEBC_PFBYTE) &c);
}


// Returns the table containing the path and the index in the table if found, otherwise returns 0.
static struct WebcVFileEntry * webc_OpenVFile(const struct WebcVFileEntry *pWebcFsRoot, WEBC_CHAR *Path);
static const struct WebcVFileEntry *webcVfFindInInstalledTables(WEBC_CHAR *Path, int *indexintable)
{
int tableNumber = 0;
struct WebcVTableContainer *pc;
const struct WebcVFileEntry *pRecord;

	// Get the
	pc = (struct WebcVTableContainer *) vector_get_node(&gWebcVTableContainerVector,tableNumber);
	if (pc)
	{
		pRecord = webc_OpenVFile(pc->AssignedTable, Path);
		if (pRecord)
		{
			*indexintable  =(int) (pRecord - pc->AssignedTable);
			return pc->AssignedTable;
		}
	}
	return 0;
}


static struct WebcVFileEntry * webc_TestOpenVFile(struct WebcVFileEntry *pWebcFsRoot, WEBC_CHAR *Path)
{
	 return (WebcVFileEntry *) webc_OpenVFile((struct WebcVFileEntry *)pWebcFsRoot, Path);
}
static int webc_CountFilesInRoot(struct WebcVFileEntry *pWebcFolder)
{
WebcVFileEntry *pEnumerator=pWebcFolder;
int rootcount = 0;
		for (int lFolderFileCount= 0; pEnumerator->pName!=0; lFolderFileCount++,pEnumerator++)
		{
			if (pEnumerator->wFlags & WEBC_VFILE_INROOT)
				rootcount++;
		}
		return rootcount;
}
static struct WebcVFileEntry * webc_FindFileInFolder(struct WebcVFileEntry *pWebcFolder, WEBC_CHAR *FileName,int isRoot, int FileCount)
{
WebcVFileEntry *pEnumerator=pWebcFolder;
		for (int lFolderFileCount= 0; pEnumerator->pName!=0 && lFolderFileCount<FileCount; lFolderFileCount++,pEnumerator++)
		{
			if (isRoot && ((pEnumerator->wFlags & WEBC_VFILE_INROOT) == 0))
				continue;
			if (!webc_c_stricmp(FileName, pEnumerator->pName))
				return pEnumerator;
		}
		return 0;
}

struct WebcVFileEntry * webc_OpenVFile(const struct WebcVFileEntry *pWebcFsRoot, WEBC_CHAR *Path)
{
#if (WEBC_SUPPORT_INTERNAL)
struct WebcVFileEntry *RetVal;
WEBC_CHAR *PathBuffer,*PathBufferCore,*p;
WEBC_CHAR *FileNames[WEBCVFMAXDEPTH];
int  NumFileNames=0;
int FileNamesInDirectory;
	RetVal = 0;
	PathBufferCore = PathBuffer = webc_malloc_string_copy(Path, __FILE__, __LINE__);
	if (!PathBufferCore)
		return 0;

	// Parse the "/" seperated input into an array of null terminated strings.
	p = webc_c_strstr(PathBuffer, "/"); // Skip leading "/"
	if (p == PathBuffer)
		PathBuffer++;
	while (PathBuffer && *PathBuffer)
	{
		FileNames[NumFileNames++]=PathBuffer;
		p = webc_c_strstr(PathBuffer, "/"); // Get next "/", terminate the file name and advance the buffer
		if (p)
		{
			*p++ = 0;
			if (*p)
				PathBuffer = p;
		}
		else
			PathBuffer = 0;
	}
	struct WebcVFileEntry *pFsFolder = (struct WebcVFileEntry *) pWebcFsRoot;
	int CurrentFileName;

	// When we first enter at the root we need to scan all so set the count way up.
	FileNamesInDirectory =  100000;
	
	for (CurrentFileName = 0; CurrentFileName < NumFileNames; CurrentFileName++)
	{
		struct WebcVFileEntry *pFsResult;
		int isRoot = (CurrentFileName==0);
		pFsResult = webc_FindFileInFolder(pFsFolder, FileNames[CurrentFileName],isRoot,FileNamesInDirectory);
		if (!pFsResult)
			break;
		else
		{
			if (CurrentFileName+1==NumFileNames) // Matched the whole path, we are done it is a match if it is a file.
			{
				if ((pFsResult->wFlags & WEBC_VFILE_FOLDER)==0)
					RetVal = pFsResult;
				break;
			}
			else
			{
				if (pFsResult->wFlags & WEBC_VFILE_FOLDER) //If it's a folder we'll get the next file and search this folder.
				{
					pFsFolder = (struct WebcVFileEntry *) &pWebcFsRoot[(int)pFsResult->pData];	// The data array points to the first table entry and they are contiguous
					FileNamesInDirectory = pFsResult->lSize; 
				}
				else
					break; // It's a file, we can't step into it.. Sorry professor.

			}
		}
	}
	if (PathBufferCore)
		webc_free_string_copy(PathBufferCore, __FILE__, __LINE__);
	return RetVal;
#endif
	return (0);
}


#endif
