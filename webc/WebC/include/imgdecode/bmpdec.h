/*
|  BMPDEC.H -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#ifndef __BMPDEC_H__
#define __BMPDEC_H__

#include "webctypes.h"

#define BMP_MAX_COLORS 256

typedef enum
{
	BMP_ERROR = -1,
	BMP_READING_HEADER = 0,
	BMP_READING_IMAGE_INFO,
	BMP_READING_COLORS,
	BMP_READING_PIXELS,
	BMP_RLE_DEFAULT,
	BMP_RLE_ESCAPED,
	BMP_RLE_READING_RUN,
	BMP_RLE_READING_LITERAL,
	BMP_RLE_READING_DELTA,
	BMP_DONE,
	BMP_NUM_STATES
}
BMPDecoderState;

#define BMP_SUCCESS             0
#define BMP_NEED_DATA          -1
#define BMP_NEED_IMAGE_BUFFER  -2
#define BMP_MALFORMED          -3
#define BMP_SIZE_UNKNOWN       -4

#define BMP_COLOR_DEPTH_UNKNOWN 0
#define BMP_COLOR_DEPTH_1_BIT   1
#define BMP_COLOR_DEPTH_4_BIT   2
#define BMP_COLOR_DEPTH_8_BIT   3
#define BMP_COLOR_DEPTH_24_BIT  4

typedef struct
{
	WEBC_UINT16 bmpFormatId;
	WEBC_INT32  fileSizeBytes;
	WEBC_UINT16 reserved1;
	WEBC_UINT16 reserved2;
	WEBC_UINT32 imageDataOffsetBytes;
}
BMPFileHeader;

#define BMP_FILE_HEADER_SIZE 14

typedef struct
{
   WEBC_UINT32 sizeBytes;          /* Header size in bytes      */
   WEBC_INT32  width;
   WEBC_INT32  height;             /* Width and height of image */
   WEBC_UINT16 planes;             /* Number of colour planes   */
   WEBC_UINT16 bits;               /* Bits per pixel (1,4,8,24) */
   WEBC_UINT32 compression;        /* Compression type          */
   WEBC_UINT32 imagesize;          /* Image size in bytes       */
   WEBC_INT32  xresolution;        /* Pixels per meter          */
   WEBC_INT32  yresolution;        /* Pixels per meter          */
   WEBC_UINT32 ncolors;            /* Number of colours         */
   WEBC_UINT32 importantcolors;    /* Important colours         */
}
BMPImageInfoHeader;

#define BMP_IMAGE_INFO_HEADER_SIZE 40

typedef struct
{
	WEBC_UINT8 red;
	WEBC_UINT8 green;
	WEBC_UINT8 blue;
	WEBC_UINT8 unused;
}
BMPColorInfo;

#define BMP_COLOR_INFO_SIZE 4

typedef struct
{
	BMPDecoderState     state;
	BMPFileHeader       header;
	BMPImageInfoHeader  info;
	BMPColorInfo        palette[BMP_MAX_COLORS];
	WEBC_UINT8*         bitmapData;
	WEBC_INT32          xPos;
	WEBC_INT32          yPos;
	int                 colorDepth; /* 0 = known, 1 = 1bpp, 2 = 4bpp, 3 = 8bpp, 4 = 24bpp */
	int                 numColorsRead;
}
BMPImageDecoder;

#ifdef __cplusplus
extern "C" {
#endif

int        BMP_DecodeInit           (BMPImageDecoder* decoder);

WEBC_INT32 BMP_DecodeData           (BMPImageDecoder* decoder,
                                     WEBC_UINT8* data,
                                     WEBC_INT32 size,
                                     int* status);

WEBC_INT32 BMP_DecodeGetBufferSize  (BMPImageDecoder* decoder);

WEBC_INT32 BMP_DecodeGetBytesPerRow (BMPImageDecoder* decoder);

WEBC_INT32 BMP_DecodeGetDestBytesPerRow (BMPImageDecoder* decoder);
void       BMP_DecodeSetImageBuffer (BMPImageDecoder* decoder,
                                     WEBC_UINT8* buffer);

void       BMP_DecodeDestroy        (BMPImageDecoder* decoder);

#ifdef __cplusplus
}
#endif

#endif /* __BMPDEC_H__ */
