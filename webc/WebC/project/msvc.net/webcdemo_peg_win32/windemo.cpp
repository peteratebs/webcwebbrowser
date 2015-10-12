/*
|  windemo.cpp
|
|  EBS -
|
|
|  Copyright EBS Inc. , 2009
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#include "peg.hpp"
#include "pwebwin.hpp"
#include "htmlsdk.h"


void PegAppInitialize(PegPresentationManager *pPresent)
{
	PegRect Rect1;
	Rect1.Set(0, 0, 480, 440);

	webc_Init();

	PegWebWindow *pWin1 = new PegWebWindow(Rect1, WEBC_NULL, WEBC_NULL);
	pPresent->Center(pWin1);
	pPresent->Add(pWin1);

}

#include "webc.h"

#if WEBC_SUPPORT_LOCAL
#include "rtpprint.h"
extern "C" int webc_localGet(char *path, void **phandle);
extern "C" int webc_localRead(void *phandle, char *buffer, long length);
extern "C" int webc_localWrite(void *phandle, char *buffer, long length);
extern "C" int webc_localClose(void *phandle);

char *errors[] = {
	"<html><body>local:error Unknown</body></html>",
	"<html><body>local:servernotfound url server not found error occurred</body></html>",
	"<html><body>local:error404 page not found error occurred</body></html>",
	"<html><body>local:error403 url 402 error occurred</body></html>",
	"<html><body>local:error403 url 403 error occurred</body></html>",
	};

int  bytes_returned[5];

int webc_localGet(char *path, void **phandle)
{
	int which_error = 0;
	if (rtp_strcmp(path,"servernotfound.html") == 0)
	{
		which_error = 1;
	}
	if (rtp_strcmp(path,"pagenotfound.html") == 0)
	{
		which_error = 2;
	}
	if (rtp_strcmp(path,"paymentreq.html") == 0)
	{
		which_error = 3;
	}
	if (rtp_strcmp(path,"forbidden.html") == 0)
	{
		which_error = 4;
	}

	*phandle = (void *) which_error;
	bytes_returned[which_error] = 0;
	return(1);
}

int webc_localRead(void *phandle, char *buffer, long length)
{
	int which_error = (int) phandle;
    /* Return len on the first and zero on the next so it terminates */
		if (bytes_returned[which_error]== 0)
		{
			rtp_strncpy(buffer, errors[which_error], length);
			bytes_returned[which_error] = rtp_strlen((char *) buffer);
			return(bytes_returned[which_error]);
		}
	return(0);
}

int webc_localWrite(void *phandle, char *buffer, long length)
{
	return(-1);
}

int webc_localClose(void *phandle)
{
	return(0);
	//return (rtp_file_close((RTP_FILE)phandle));
}
#endif//WEBC_SUPPORT_LOCAL

void wcDisplayManagerDrawCallback(class DisplayManager *p)
{

}
void guiWmBringToTop(struct s_HTMLGraphicsContext **p){ HEREHERE }
void guiWmGetPos(struct s_HTMLGraphicsContext **p,int *a,int *b,int *c,int *d){}
void guiWmSetPos(struct s_HTMLGraphicsContext **p,int a,int b,int c,int d){}
// Alert box for Javascript.. (badly in need of rework)
extern "C" void webc_user_alert_box(char *str)
{
}

void webc_user_debug_window(char *str)
{
}
