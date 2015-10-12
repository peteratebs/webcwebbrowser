//
// SysInfo.cpp - WebC GUI Example
//
//
//  C code needed for the SysInfo page of the Spectrometer Demo.
//
// 	webC provides a url extension named local:. This is implemented as a callback mechanism that
//  allows underlying C callback code to serve web pages by inspecting the url name and returning formatted
//  HTML based on the url name.
//
//  	The local url is one of several simple name based interfaces for interfacing C code to html content.
//      These are somewhat simpler than the framework model used in the rest of this.
// 		 Other simple links between C and HTML are.
//  		Metatag Handlers - A C function may be registered to be called when a specific HTML Metatag is encountered.
//  		Named HTML Event Handlers - A C function may be registered as a named event handler. In an HTML tag.
//
// In this example the local url callback interface is configured to return a table of formatted
// fictitious configuration values when local:\\sysinfo.html is requested.
//
// Public functions:
//		webc_localGet(),webc_localRead()webc_localWrite(),webc_localClose()
//
//

#include "htmlsdk.h"
#include "rtpstr.h"
#include "rtpprint.h"
#include "rtpmem.h"

extern "C" int webc_localGet(char *path, void **phandle);
extern "C" int webc_localRead(void *phandle, char *buffer, long length);
extern "C" int webc_localWrite(void *phandle, char *buffer, long length);
extern "C" int webc_localClose(void *phandle);

// Formatting strings to create our table
static char *SysinfoProLog = "\
<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\
<html>\
<head>\
<meta http-equiv=\"pragma\" content=\"no-cache\"/>\
<link rel=\"stylesheet\" type=\"text/css\" href=\"file://\\demo_pages\\demo_spectrometer_style.css\" />\
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">\
<title>Help Window</title>\
</head>\
<body>\
<table style=\"position:absolute;left:0px;top:0px;width:280px;\" border=\"1\" cellspacing=\"0\" cellpadding=\"0\" >";
static char *SysinfoRowStr = "<tr><td class=\"SysinfoPrompt\">%s </td><td class=\"SysinfoValue\">%s</td></tr>";
static char *SysinfoEpilog = "</body></html>";
static int AppendSysinfoRecord(char *str, int recordnumber);



// webc_localGet()   Formats an html document containing a table with system information, stores it a way for later retrieval.
// webc_localRead()  Returns content each time it is called. When all content is reed, return zero.
// webc_localClose() Release the content.


struct localfile {
	char *buffer;
	int  bufferlength;
	int bytesreturned;
};
int webc_localGet(char *path, void **phandle)
{
	if (rtp_strcmp(path,"SysInfo.html") == 0)
	{
		char *str = 0;
		struct localfile *plocalfile;
		plocalfile = (struct localfile *)rtp_malloc(sizeof(*plocalfile));
		if (!plocalfile)
			return -1;
		rtp_memset(plocalfile, 0, sizeof(*plocalfile));
		plocalfile->buffer = (char *) rtp_malloc(0xffff); // Allocate 64K but we'll use a fraction of it and
		str = plocalfile->buffer;

		rtp_strcpy(str,SysinfoProLog);
		str += rtp_strlen(str);
		for (int inforec  =0; ;inforec++)
		{
			int reclen = AppendSysinfoRecord(str,inforec);
			if (!reclen) break;
			str += reclen;
		}
		rtp_strcpy(str,SysinfoEpilog);
		plocalfile->bufferlength = rtp_strlen(plocalfile->buffer);
		*phandle = (void *) plocalfile;
		return(0);
	}
	return(-1);
}


int webc_localRead(void *phandle, char *buffer, long length)
{
	struct localfile *plocalfile = (struct localfile *) phandle;
	if (plocalfile->bytesreturned >= plocalfile->bufferlength)
		return 0;
	if (length > (plocalfile->bufferlength - plocalfile->bytesreturned))
		length = plocalfile->bufferlength - plocalfile->bytesreturned;
	rtp_strncpy(buffer, plocalfile->buffer+plocalfile->bytesreturned, length);
	plocalfile->bytesreturned += length;
	return(length);
}

int webc_localWrite(void *phandle, char *buffer, long length)
{
	return(-1);
}

int webc_localClose(void *phandle)
{
	struct localfile *plocalfile = (struct localfile *) phandle;
	if (plocalfile)
	{
		if (plocalfile->buffer)	rtp_free(plocalfile->buffer);
		rtp_free(plocalfile);
	}
	return(0);
}


char *FakeInorecordPairs[][2] = {
		{ "Serial Number", "214321" },
		{ "Manufacture Date", "Jan 1, 2010" },
		{ "Installed PMTs", "Hg,Mn,Na,Ca,Md" },
		{ "Absorbtion Sources", "Carbon Tube,Flame" },
		{ "Emission Sources", "Arc, Spark, Plasma" },
		{ "Sample Sources", "Nebulizer, Laser Vaporization" },
		{0,0} };


static int AppendSysinfoRecord(char *str, int recordnumber)
{
	if (FakeInorecordPairs[recordnumber][0])
	{
		rtp_sprintf(str, SysinfoRowStr, FakeInorecordPairs[recordnumber][0], FakeInorecordPairs[recordnumber][1]);
		return(rtp_strlen(str));
	}
	return(0);
}
