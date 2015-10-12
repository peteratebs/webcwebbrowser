#include "htmlsdk.h"
#include "webc.h"
#include "rtpprint.h"
#include "rtpmem.h"
//const char g_testsuiteURL[] = "webc://welcome.html"; // file://M:\\myfile.html"
// const char g_testsuiteURL[] = "file://m:\\sampleweb.html";
const char g_testsuiteURL[] = "webc://welcome.html";
#if (HTTP_HEADER_TEST) //MA TESTING BEGIN
static void webc_set_macaddr(void);
#endif

HBROWSER_HANDLE webc_start_webc_tests(HTMLBrowserConfig *config, WEBC_BOOL CreateNewWindow, char * pTestsuiteURL)
{
    HBROWSER_HANDLE browser;
    HDOC_HANDLE document;
    //config->browserEventHandler = browserHandler;

    config->browserEventHandler = 0;
    config->privateData = 0;
	/* 11/23/09 - Make the window larger and resizable. Had been using defaults. */
	config->WindowConfig.top = 0;
    config->WindowConfig.left = 0;
    config->WindowConfig.height = 240; //   768;
    config->WindowConfig.width =  320; // 1024;
     browser = webc_CreateBrowser(config,  CreateNewWindow, "Explore" , "");
    document = webc_BrowserGetDocument(browser);
	//CONIO_Output_String_Time( (char*)g_testsuiteURL);
#if (HTTP_HEADER_TEST)
	webc_set_macaddr();
#endif
    if (document)
    {
		WEBC_DEBUG_LOG_TIME_RESET("STARTTEST")
		webc_BrowserLoadUrl(browser, pTestsuiteURL ? pTestsuiteURL : (char *) g_testsuiteURL);
    }
	return browser;
}


#if (HTTP_HEADER_TEST) //MA TESTING BEGIN

static void webc_set_macaddr(void)
{
WEBC_CHAR MAC_ADDR[18]="10:20:4E:55:65:AA";
WEBC_CHAR SERIAL_NO[18]= "123456789ABCDEF";

	mac_len=rtp_strlen(MAC_ADDR)+1;
	serial_len=rtp_strlen(SERIAL_NO)+1;
	set_webc_tsd_token(WEBC_TOKEN_MACADDR,MAC_ADDR,mac_len);
	set_webc_tsd_token(WEBC_TOKEN_SERILANO,SERIAL_NO,serial_len);
	//set for the "WEBC_TOKEN_ADDITIONAL_HEADER" token
}
#endif




