//
// FILEEXT.C - 
//
// EBS - 
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/************************************************************************
* Headers
************************************************************************/
#include "fileext.h"
#include "rtpstr.h"

/************************************************************************
* Macros
************************************************************************/

#define UPPERCASE(X) (((X) >= 'a' || (X) <= 'z')? ((X) + 'A' - 'a') : (X))


/************************************************************************
* Types
************************************************************************/

typedef struct s_FileExtensionEntry
{
	const char*         extension;
	WebcFileContentType contentType;
}
FileExtensionEntry;


/************************************************************************
* Data
************************************************************************/

FileExtensionEntry gpFileExtensionTable [] =
{
	{"323",   WEBC_FILE_TYPE_TEXT_H323},
	{"AI",    WEBC_FILE_TYPE_APPLICATION_POSTSCRIPT},
	{"AIF",   WEBC_FILE_TYPE_AUDIO_X_AIFF},
	{"AIFC",  WEBC_FILE_TYPE_AUDIO_X_AIFF},
	{"AIFF",  WEBC_FILE_TYPE_AUDIO_X_AIFF},
	{"ASF",   WEBC_FILE_TYPE_VIDEO_X_MS_ASF},
	{"ASP",   WEBC_FILE_TYPE_TEXT_HTML},
	{"ASR",   WEBC_FILE_TYPE_VIDEO_X_MS_ASF},
	{"ASX",   WEBC_FILE_TYPE_VIDEO_X_MS_ASF},
	{"AU",    WEBC_FILE_TYPE_AUDIO_BASIC},
	{"AVI",   WEBC_FILE_TYPE_VIDEO_X_MSVIDEO},
	{"BAS",   WEBC_FILE_TYPE_TEXT_PLAIN},
	{"BIN",   WEBC_FILE_TYPE_APPLICATION_OCTET_STREAM},
	{"BMP",   WEBC_FILE_TYPE_IMAGE_BMP},
	{"C",     WEBC_FILE_TYPE_TEXT_PLAIN},
	{"CPP",   WEBC_FILE_TYPE_TEXT_PLAIN},
	{"CHM",   WEBC_FILE_TYPE_TEXT_HTML},
	{"CLA",   WEBC_FILE_TYPE_APPLICATION_OCTET_STREAM},
	{"CLASS", WEBC_FILE_TYPE_APPLICATION_OCTET_STREAM},
	{"COD",   WEBC_FILE_TYPE_IMAGE_CIS_COD},
	{"CSS",   WEBC_FILE_TYPE_TEXT_CSS},
	{"DMS",   WEBC_FILE_TYPE_APPLICATION_OCTET_STREAM},
	{"EPS",   WEBC_FILE_TYPE_APPLICATION_POSTSCRIPT},
	{"ETX",   WEBC_FILE_TYPE_TEXT_X_SETEXT},
	{"EXE",   WEBC_FILE_TYPE_APPLICATION_OCTET_STREAM},
	{"FLR",   WEBC_FILE_TYPE_X_WORLD_X_VRML},
	{"GIF",   WEBC_FILE_TYPE_IMAGE_GIF},
	{"GTAR",  WEBC_FILE_TYPE_APPLICATION_X_GTAR},
	{"GZ",    WEBC_FILE_TYPE_APPLICATION_X_GZIP},
	{"GZIP",  WEBC_FILE_TYPE_APPLICATION_X_GZIP},
	{"H",     WEBC_FILE_TYPE_TEXT_PLAIN},
	{"HPP",   WEBC_FILE_TYPE_TEXT_PLAIN},
	{"HTM",   WEBC_FILE_TYPE_TEXT_HTML},
	{"HTML",  WEBC_FILE_TYPE_TEXT_HTML},
	{"HTC",   WEBC_FILE_TYPE_TEXT_X_COMPONENT},
	{"HTT",   WEBC_FILE_TYPE_TEXT_WEBVIEWHTML},
	{"ICO",   WEBC_FILE_TYPE_IMAGE_X_ICON},
	{"IEF",   WEBC_FILE_TYPE_IMAGE_IEF},
	{"JFIF",  WEBC_FILE_TYPE_IMAGE_PIPEG},
	{"JPE",   WEBC_FILE_TYPE_IMAGE_JPEG},
	{"JPEG",  WEBC_FILE_TYPE_IMAGE_JPEG},
	{"JPG",   WEBC_FILE_TYPE_IMAGE_JPEG},
	{"JS",    WEBC_FILE_TYPE_APPLICATION_X_JAVASCRIPT},
	{"JSP",   WEBC_FILE_TYPE_TEXT_HTML},
	{"LHA",   WEBC_FILE_TYPE_APPLICATION_OCTET_STREAM},
	{"LSF",   WEBC_FILE_TYPE_VIDEO_X_LA_ASF},
	{"LSX",   WEBC_FILE_TYPE_VIDEO_X_LA_ASF},
	{"LZH",   WEBC_FILE_TYPE_APPLICATION_OCTET_STREAM},
	{"M3U",   WEBC_FILE_TYPE_AUDIO_X_MPEGURL},
	{"MHT",   WEBC_FILE_TYPE_MESSAGE_RFC822},
	{"MHTML", WEBC_FILE_TYPE_MESSAGE_RFC822},
	{"MID",   WEBC_FILE_TYPE_AUDIO_MID},
	{"MOV",   WEBC_FILE_TYPE_VIDEO_QUICKTIME},
	{"MOVIE", WEBC_FILE_TYPE_VIDEO_X_SGI_MOVIE},
	{"MP2",   WEBC_FILE_TYPE_VIDEO_MPEG},
	{"MP3",   WEBC_FILE_TYPE_AUDIO_MPEG},
	{"MPA",   WEBC_FILE_TYPE_VIDEO_MPEG},
	{"MPE",   WEBC_FILE_TYPE_VIDEO_MPEG},
	{"MPEG",  WEBC_FILE_TYPE_VIDEO_MPEG},
	{"MPG",   WEBC_FILE_TYPE_VIDEO_MPEG},
	{"MPV2",  WEBC_FILE_TYPE_VIDEO_MPEG},
	{"NWS",   WEBC_FILE_TYPE_MESSAGE_RFC822},
	{"ODA",   WEBC_FILE_TYPE_APPLICATION_ODA},
	{"PBM",   WEBC_FILE_TYPE_IMAGE_X_PORTABLE_BITMAP},
	{"PDF",   WEBC_FILE_TYPE_APPLICATION_PDF},
	{"PGM",   WEBC_FILE_TYPE_IMAGE_X_PORTABLE_GRAYMAP},
	{"PHP",   WEBC_FILE_TYPE_TEXT_HTML},
	{"PNG",   WEBC_FILE_TYPE_IMAGE_PNG},
	{"PPM",   WEBC_FILE_TYPE_IMAGE_X_PORTABLE_PIXMAP},
	{"PNM",   WEBC_FILE_TYPE_IMAGE_X_PORTABLE_ANYMAP},
	{"PS",    WEBC_FILE_TYPE_APPLICATION_POSTSCRIPT},
	{"QT",    WEBC_FILE_TYPE_VIDEO_QUICKTIME},
	{"RA",    WEBC_FILE_TYPE_AUDIO_X_PN_REALAUDIO},
	{"RAM",   WEBC_FILE_TYPE_AUDIO_X_PN_REALAUDIO},
	{"RAS",   WEBC_FILE_TYPE_IMAGE_X_CMU_RASTER},
	{"RGB",   WEBC_FILE_TYPE_IMAGE_X_RGB},
	{"RMI",   WEBC_FILE_TYPE_AUDIO_MID},
	{"RTF",   WEBC_FILE_TYPE_TEXT_RICHTEXT},
	{"RTX",   WEBC_FILE_TYPE_TEXT_RICHTEXT},
	{"SCT",   WEBC_FILE_TYPE_TEXT_SCRIPTLET},
	{"SND",   WEBC_FILE_TYPE_AUDIO_BASIC},
	{"STM",   WEBC_FILE_TYPE_TEXT_HTML},
	{"TAR",   WEBC_FILE_TYPE_APPLICATION_X_TAR},
	{"TEXT",  WEBC_FILE_TYPE_TEXT_PLAIN},
	{"TIF",   WEBC_FILE_TYPE_IMAGE_TIFF},
	{"TIFF",  WEBC_FILE_TYPE_IMAGE_TIFF},
	{"TSV",   WEBC_FILE_TYPE_TEXT_TAB_SEPARATED_VALUES},
	{"TXT",   WEBC_FILE_TYPE_TEXT_PLAIN},
	{"ULS",   WEBC_FILE_TYPE_TEXT_IULS},
	{"VCF",   WEBC_FILE_TYPE_TEXT_VCARD},
	{"VRML",  WEBC_FILE_TYPE_X_WORLD_X_VRML},
	{"WAV",   WEBC_FILE_TYPE_AUDIO_X_WAV},
	{"WRL",   WEBC_FILE_TYPE_X_WORLD_X_VRML},
	{"WRZ",   WEBC_FILE_TYPE_X_WORLD_X_VRML},
	{"XAF",   WEBC_FILE_TYPE_X_WORLD_X_VRML},
	{"XBM",   WEBC_FILE_TYPE_IMAGE_X_XBITMAP},
	{"XOF",   WEBC_FILE_TYPE_X_WORLD_X_VRML},
	{"XPM",   WEBC_FILE_TYPE_IMAGE_X_XPIXMAP},
	{"XWD",   WEBC_FILE_TYPE_IMAGE_X_XWINDOWDUMP},
	{"XML",   WEBC_FILE_TYPE_TEXT_XML},
	{"Z",     WEBC_FILE_TYPE_APPLICATION_X_COMPRESS},
	{"ZIP",   WEBC_FILE_TYPE_APPLICATION_ZIP}
};

#define NUM_FILE_EXTENSIONS (sizeof(gpFileExtensionTable) / sizeof(FileExtensionEntry))


/************************************************************************
* Functions
************************************************************************/

/*---------------------------------------------------------------------------*/
WebcFileContentType webc_GetFileTypeByExtension (const char *fileName)
{
const char *extPos = 0;

	while (fileName[0] && fileName[0] != '?')
	{
		if (fileName[0] == '.')
		{
			extPos = fileName + 1;
		}
		else if (fileName[0] == '/' || fileName[0] == '\\')
		{
			extPos = 0;
		}
		fileName++;
	}
	
	if (extPos)
	{	
		char extStr[8];
		int n;
		int order;
		int low = 0;	
		int high = NUM_FILE_EXTENSIONS-1;
		int middle;

		for (n = 0; n < 7 && extPos[0] && extPos[0] != '?'; n++, extPos++)
		{
			extStr[n] = UPPERCASE(extPos[0]);
		}
		
		extStr[n] = 0;
	
		while (low <= high)
		{
			middle = (low + high) >> 1;
			order = rtp_strcmp(extStr, gpFileExtensionTable[middle].extension);
			
			if (order == 0)
			{
				return (gpFileExtensionTable[middle].contentType);
			}
			
			if (order < 0)
			{
				high = middle - 1;
			}
			else
			{
				low = middle + 1;
			}
		}
	}

	return (WEBC_FILE_TYPE_UNKNOWN);
}
