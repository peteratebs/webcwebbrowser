//
// FILETYPE.H - File type utilities
//
// EBS - HTTP
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
#ifndef __FILETYPE_H__
#define __FILETYPE_H__

// array indicies for gpFileContentTypeNames
typedef enum e_WebcFileContentType
{
	WEBC_FILE_TYPE_UNKNOWN = -1,
	WEBC_FILE_TYPE_APPLICATION_ODA = 0,
	WEBC_FILE_TYPE_APPLICATION_OCTET_STREAM,
	WEBC_FILE_TYPE_APPLICATION_PDF,
	WEBC_FILE_TYPE_APPLICATION_POSTSCRIPT,
	WEBC_FILE_TYPE_APPLICATION_X_COMPRESS,
	WEBC_FILE_TYPE_APPLICATION_X_TAR,
	WEBC_FILE_TYPE_APPLICATION_X_GTAR,
	WEBC_FILE_TYPE_APPLICATION_X_GZIP,
	WEBC_FILE_TYPE_APPLICATION_X_JAVASCRIPT,
	WEBC_FILE_TYPE_APPLICATION_XHTML,
	WEBC_FILE_TYPE_APPLICATION_ZIP,
	WEBC_FILE_TYPE_AUDIO_BASIC,
	WEBC_FILE_TYPE_AUDIO_MID,
	WEBC_FILE_TYPE_AUDIO_MPEG,
	WEBC_FILE_TYPE_AUDIO_X_AIFF,
	WEBC_FILE_TYPE_AUDIO_X_MPEGURL,
	WEBC_FILE_TYPE_AUDIO_X_PN_REALAUDIO,
	WEBC_FILE_TYPE_AUDIO_X_WAV,
	WEBC_FILE_TYPE_IMAGE_BMP,
	WEBC_FILE_TYPE_IMAGE_CIS_COD,
	WEBC_FILE_TYPE_IMAGE_GIF,
	WEBC_FILE_TYPE_IMAGE_IEF,
	WEBC_FILE_TYPE_IMAGE_JPEG,
	WEBC_FILE_TYPE_IMAGE_PIPEG,
	WEBC_FILE_TYPE_IMAGE_PNG,
	WEBC_FILE_TYPE_IMAGE_TIFF,
	WEBC_FILE_TYPE_IMAGE_X_CMU_RASTER,
	WEBC_FILE_TYPE_IMAGE_X_ICON,
	WEBC_FILE_TYPE_IMAGE_X_PORTABLE_ANYMAP,
	WEBC_FILE_TYPE_IMAGE_X_PORTABLE_BITMAP,
	WEBC_FILE_TYPE_IMAGE_X_PORTABLE_GRAYMAP,
	WEBC_FILE_TYPE_IMAGE_X_PORTABLE_PIXMAP,
	WEBC_FILE_TYPE_IMAGE_X_RGB,
	WEBC_FILE_TYPE_IMAGE_X_XBITMAP,
	WEBC_FILE_TYPE_IMAGE_X_XPIXMAP,
	WEBC_FILE_TYPE_IMAGE_X_XWINDOWDUMP,
	WEBC_FILE_TYPE_MESSAGE_RFC822,
	WEBC_FILE_TYPE_TEXT_CSS,
	WEBC_FILE_TYPE_TEXT_H323,
	WEBC_FILE_TYPE_TEXT_HTML,
	WEBC_FILE_TYPE_TEXT_IULS,
	WEBC_FILE_TYPE_TEXT_PLAIN,
	WEBC_FILE_TYPE_TEXT_RICHTEXT,
	WEBC_FILE_TYPE_TEXT_SCRIPTLET,
	WEBC_FILE_TYPE_TEXT_TAB_SEPARATED_VALUES,
	WEBC_FILE_TYPE_TEXT_VCARD,
	WEBC_FILE_TYPE_TEXT_VND_WAP_WML,
	WEBC_FILE_TYPE_TEXT_WEBVIEWHTML,
	WEBC_FILE_TYPE_TEXT_X_COMPONENT,
	WEBC_FILE_TYPE_TEXT_X_SETEXT,
	WEBC_FILE_TYPE_TEXT_XML,
	WEBC_FILE_TYPE_VIDEO_MPEG,
	WEBC_FILE_TYPE_VIDEO_QUICKTIME,
	WEBC_FILE_TYPE_VIDEO_X_LA_ASF,
	WEBC_FILE_TYPE_VIDEO_X_MNG,
	WEBC_FILE_TYPE_VIDEO_X_MS_ASF,
	WEBC_FILE_TYPE_VIDEO_X_MSVIDEO,
	WEBC_FILE_TYPE_VIDEO_X_SGI_MOVIE,
	WEBC_FILE_TYPE_X_WORLD_X_VRML,
	WEBC_NUM_FILE_TYPES
}
WebcFileContentType;

#ifdef __cplusplus
extern "C" {
#endif

WebcFileContentType webc_StrToFileContentType (const char *str);
const char*         webc_FileContentTypeToStr (WebcFileContentType type);

#ifdef __cplusplus
}
#endif


#endif /*__FILETYPE_H__*/