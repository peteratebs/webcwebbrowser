#include "webc.h"
#include "webcxlib.h"
#include "webc_ctrl_demo.h"

#include <X11/Xlib.h>

#include <stdio.h>
#include <stdlib.h>		/* getenv(), etc. */
#include <unistd.h>		/* sleep(), etc.  */

#define SPAWN_EXAMPLE 0
#define SPAWN_CHROME_BROWSER 1
#define SPAWN_TEST_SUITE 0


HBROWSER_HANDLE webc_start_webc_demo(HTMLBrowserConfig* cfg, WEBC_BOOL bCreateNewWindow);
HBROWSER_HANDLE webc_start_webc_tests(HTMLBrowserConfig* cfg, WEBC_BOOL bCreateNewWindow);
HBROWSER_HANDLE webc_start_chrome_browser(HTMLBrowserConfig *pconfig, WEBC_BOOL bCreateNewWindow);
void xlib_bind_window_to_browser(HTMLBrowserConfig *config, HBROWSER_HANDLE browser);
int main(int argc, char* argv[])
{
	webc_Init();
	WebcXLIBGraphics xlibCtx;
	HTMLBrowserConfig config[3];
	HBROWSER_HANDLE	gbrowser;

	rtp_memset(&config[0], 0, sizeof(HTMLBrowserConfig));
	rtp_memset(&config[1], 0, sizeof(HTMLBrowserConfig));
	rtp_memset(&config[2], 0, sizeof(HTMLBrowserConfig));

#if (SPAWN_CHROME_BROWSER)
	webc_InitXLIBBrowserConfig (&config[0], WEBC_FALSE);
    gbrowser = webc_start_chrome_browser(&config[0], WEBC_TRUE);
	xlib_bind_window_to_browser(&config[0], gbrowser);
	xlib_execute_webc(config[0].graphics, gbrowser);
#endif
	
#if (SPAWN_TEST_SUITE)
	webc_InitXLIBBrowserConfig (&config[1], WEBC_FALSE);
    gbrowser = webc_start_webc_tests(&config[1], WEBC_TRUE);
	xlib_bind_window_to_browser(&config[1], gbrowser);
	xlib_execute_webc(config[1].graphics, gbrowser);
#endif
	
#if (SPAWN_EXAMPLE)
	webc_InitXLIBBrowserConfig (&config[2], WEBC_FALSE);
    gbrowser = webc_start_webc_demo(&config[2], WEBC_TRUE);
	xlib_bind_window_to_browser(&config[2], gbrowser);
	xlib_execute_webc(config[2].graphics, gbrowser);
#endif
	return(0);
}
