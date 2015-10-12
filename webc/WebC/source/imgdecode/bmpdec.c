/*
|  BMPDEC.C -
|
|  EBS - WebC
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/
#include "webcfg.h"
#include "bmpdec.h"
#include "rtpstr.h"

/*****************************************************************************/
/* Macros
 *****************************************************************************/
// #define TEST_BIG_ENDIAN
#ifdef TEST_BIG_ENDIAN

#define READ_INT32(V,B,P)   V = (((unsigned long)(((unsigned char*)B)[P+3])))         | \
                                (((unsigned long)(((unsigned char*)B)[P+2])) << 8)  | \
                                (((unsigned long)(((unsigned char*)B)[P+1])) << 16) | \
                                (((long)         (((unsigned char*)B)[P])) << 24);  \
                            P += 4;

#define READ_UINT32(V,B,P)  V = (((unsigned long)(((unsigned char*)B)[P+3])))         | \
                                (((unsigned long)(((unsigned char*)B)[P+2])) << 8)  | \
                                (((unsigned long)(((unsigned char*)B)[P+1])) << 16) | \
                                (((unsigned long)(((unsigned char*)B)[P])) << 24);  \
                            P += 4;

#define READ_INT16(V,B,P)   V = (((unsigned short)(((unsigned char*)B)[P+1])))      |  \
                                (((short)         (((unsigned char*)B)[P])) << 8); \
                            P += 2;

#define READ_UINT16(V,B,P)  V = (((unsigned short)(((unsigned char*)B)[P+1])))      |  \
                                (((unsigned short)(((unsigned char*)B)[P])) << 8); \
                            P += 2;

#else

#define READ_INT32(V,B,P)   V = (((unsigned long)(((unsigned char*)B)[P])))         | \
                                (((unsigned long)(((unsigned char*)B)[P+1])) << 8)  | \
                                (((unsigned long)(((unsigned char*)B)[P+2])) << 16) | \
                                (((long)         (((unsigned char*)B)[P+3])) << 24);  \
                            P += 4;

#define READ_UINT32(V,B,P)  V = (((unsigned long)(((unsigned char*)B)[P])))         | \
                                (((unsigned long)(((unsigned char*)B)[P+1])) << 8)  | \
                                (((unsigned long)(((unsigned char*)B)[P+2])) << 16) | \
                                (((unsigned long)(((unsigned char*)B)[P+3])) << 24);  \
                            P += 4;

#define READ_INT16(V,B,P)   V = (((unsigned short)(((unsigned char*)B)[P])))      |  \
                                (((short)         (((unsigned char*)B)[P+1])) << 8); \
                            P += 2;

#define READ_UINT16(V,B,P)  V = (((unsigned short)(((unsigned char*)B)[P])))      |  \
                                (((unsigned short)(((unsigned char*)B)[P+1])) << 8); \
                            P += 2;

#endif

#define READ_INT8(V,B,P)    V = ((char*)B)[P]; \
                            P++;

#define READ_UINT8(V,B,P)   V = ((unsigned char*)B)[P]; \
                            P++;

#define READ_UINT4_2X(V1,V2,B,P)  V1 = (((unsigned char*)B)[P] & 0xf0) >> 4; \
                                  V2 = (((unsigned char*)B)[P] & 0x0f); \
                                  P++;

#define READ_8BITS(V,B,P)         V[0] = (((unsigned char*)B)[P] & 0x80) >> 7; \
                                  V[1] = (((unsigned char*)B)[P] & 0x40) >> 6; \
                                  V[2] = (((unsigned char*)B)[P] & 0x20) >> 5; \
                                  V[3] = (((unsigned char*)B)[P] & 0x10) >> 4; \
                                  V[4] = (((unsigned char*)B)[P] & 0x08) >> 3; \
                                  V[5] = (((unsigned char*)B)[P] & 0x04) >> 2; \
                                  V[6] = (((unsigned char*)B)[P] & 0x02) >> 1; \
                                  V[7] = (((unsigned char*)B)[P] & 0x01); \
                                  P++;

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
int BMP_DecodeInit (BMPImageDecoder* decoder)
{
	rtp_memset(decoder, 0, sizeof(BMPImageDecoder));
	decoder->state = BMP_READING_HEADER;
	return BMP_SUCCESS;
}

/*---------------------------------------------------------------------------*/
/*  BMP_DecodeData

	This is the main decode loop; it is called repeatedly with new data to
	decode the image.  The int pointed to by status is set to one of:

		BMP_SUCCESS            - everything went fine
		BMP_NEED_DATA          - the decoder needs more data
		BMP_NEED_IMAGE_BUFFER  - the size of the image buffer is now known;
		                          the user must now ask the decoder what the
		                          minimum image buffer size is using
		                          BMP_DecodeGetBufferSize, and then hand the
		                          decoder a buffer at least that big using
		                          BMP_DecodeSetImageBuffer.  Image data will
		                          be decoded directly into this buffer.
		BMP_MALFORMED          - the BMP data is malformed; BMP_DecodeDestroy
		                          must be called and the whole process aborted.

	Returns the number of bytes read.  The user is responsible to prefix any
	 unread data from the end of this buffer to any future buffers to ensure
	 no data is lost.

	Example:

		bytesBuffered = 0;

		while (status == BMP_NEED_DATA)
		{
			if (bytesBuffered > 0)
			{
				// move unread data to the front of the buffer
				memmove(data, data + bytesRead, bytesBuffered);
			}

			// now read more data into the buffer (from a socket)
			bytesReceived = recv(sd, data + bytesBuffered, BUFFER_SIZE - bytesBuffered);
			if (bytesReceived <= 0)
			{
				break;
			}

			bytesBuffered += bytesReceived;

			// decode new data
			bytesRead = BMP_DecodeData (decoder, data, bytesBuffered, &status);

			if (bytesRead < 0)
			{
				// unknown condition
				break;
			}

			bytesBuffered -= bytesRead;
		}

 */
WEBC_INT32 BMP_DecodeData (BMPImageDecoder* decoder,
                           WEBC_UINT8* data,
                           WEBC_INT32 bytesAvailable,
                           int* status)
{
	WEBC_INT32 bytesRead = 0;

	*status = BMP_SUCCESS;

	do
	{
		switch (decoder->state)
		{
			case BMP_READING_HEADER:
				if ((bytesAvailable - bytesRead) < BMP_FILE_HEADER_SIZE)
				{
					*status = BMP_NEED_DATA;
				}
				else
				{
					// read the header
					READ_UINT16(decoder->header.bmpFormatId, data, bytesRead);
					READ_INT32(decoder->header.fileSizeBytes, data, bytesRead);
					READ_UINT16(decoder->header.reserved1, data, bytesRead);
					READ_UINT16(decoder->header.reserved2, data, bytesRead);
					READ_UINT32(decoder->header.imageDataOffsetBytes, data, bytesRead);

#if (WEBC_INCLUDE_BMP_PALLETE_FIXES)

					if (decoder->header.imageDataOffsetBytes >= 54)
						decoder->header.imageDataOffsetBytes -= 54;

#endif

					if (decoder->header.bmpFormatId != 0x4D42)
					{
						*status = BMP_MALFORMED;
						decoder->state = BMP_ERROR;
					}
					else
					{
						decoder->state = BMP_READING_IMAGE_INFO;
					}
				}
				break;

			case BMP_READING_IMAGE_INFO:
				if ((bytesAvailable - bytesRead) < BMP_IMAGE_INFO_HEADER_SIZE)
				{
					*status = BMP_NEED_DATA;
				}
				else
				{
					// read the image info
					READ_UINT32(decoder->info.sizeBytes, data, bytesRead);
					READ_INT32(decoder->info.width, data, bytesRead);
					READ_INT32(decoder->info.height, data, bytesRead);
					READ_UINT16(decoder->info.planes, data, bytesRead);
					READ_UINT16(decoder->info.bits, data, bytesRead); //bits per pixle 0x18 is 24 decimal
					READ_UINT32(decoder->info.compression, data, bytesRead);  //compression type if any, consult wiki in bmp image format or header
					READ_UINT32(decoder->info.imagesize, data, bytesRead);		//image size in byes
					READ_INT32(decoder->info.xresolution, data, bytesRead);		//bixles per meter???
					READ_INT32(decoder->info.yresolution, data, bytesRead);
					READ_UINT32(decoder->info.ncolors, data, bytesRead);			//number of colors
					READ_UINT32(decoder->info.importantcolors, data, bytesRead);	//number of colors

					// tbd - do consistency checks here

#if (WEBC_INCLUDE_BMP_PALLETE_FIXES)
					/* If ncolors == 0, use default color table size based on bits */
					if (decoder->info.ncolors == 0 && decoder->info.bits <= 8)
					{
					WEBC_UINT32 forcedncolors;
						if (decoder->info.bits == 1)
							forcedncolors = 2;
						else if (decoder->info.bits == 4)
							forcedncolors = 16;
						else
							forcedncolors = 256;

						decoder->info.ncolors = forcedncolors;
					}

					if (	(decoder->info.bits != 24 &&
							decoder->info.bits != 8  &&
							decoder->info.bits != 4  &&
							decoder->info.bits != 1)  &&
							((decoder->info.bits == 24 && decoder->info.ncolors != 0) ||
							decoder->info.compression != 0)
						)
					{
						*status = BMP_MALFORMED;
						decoder->state = BMP_ERROR;
						break;
					}
#endif

					if (decoder->info.ncolors > 0)
					{
						decoder->state = BMP_READING_COLORS;
					}
					else
					{
#if (WEBC_INCLUDE_BMP_PALLETE_FIXES)
						decoder->state = BMP_READING_PIXELS;
						decoder->xPos = 0;
						decoder->yPos = decoder->info.height - 1;

#else
						if (decoder->info.bits != 24)
						{
							*status = BMP_MALFORMED;
							decoder->state = BMP_ERROR;
						}
						else
						{
							decoder->state = BMP_READING_PIXELS;
							decoder->xPos = 0;
							decoder->yPos = decoder->info.height - 1;
						}
#endif
					}
				}
				break;

			case BMP_READING_COLORS:
				while (decoder->numColorsRead < (int) decoder->info.ncolors)
				{
					if ((bytesAvailable - bytesRead) < BMP_COLOR_INFO_SIZE)
					{
						*status = BMP_NEED_DATA;
						break;
					}

					READ_UINT8(decoder->palette[decoder->numColorsRead].blue, data, bytesRead);
					READ_UINT8(decoder->palette[decoder->numColorsRead].green, data, bytesRead);
					READ_UINT8(decoder->palette[decoder->numColorsRead].red, data, bytesRead);
					READ_UINT8(decoder->palette[decoder->numColorsRead].unused, data, bytesRead);

#if (WEBC_INCLUDE_BMP_PALLETE_FIXES)
					/* We are reading the color table preceeding the data so reduce the image offset value
					   That we have to skip */
					if (decoder->header.imageDataOffsetBytes >= 4)
						decoder->header.imageDataOffsetBytes -= 4;
#endif
					decoder->numColorsRead++;
				}

				if (*status != BMP_NEED_DATA)
				{
					decoder->state = BMP_READING_PIXELS;
					decoder->xPos = 0;
					decoder->yPos = decoder->info.height - 1;
				}
				break;

			case BMP_READING_PIXELS:
				if (!decoder->bitmapData)
				{
					*status = BMP_NEED_IMAGE_BUFFER;
				}
				else
				{
#if (WEBC_INCLUDE_BMP_PALLETE_FIXES)
					WEBC_UINT32 bytesPerLineDest = BMP_DecodeGetDestBytesPerRow(decoder);
					WEBC_UINT32 paddingBytes = (4 - (((decoder->info.width * decoder->info.bits) + 7) >> 3)) & 3;
					WEBC_UINT8* linePtr = decoder->bitmapData + (decoder->yPos * bytesPerLineDest);

#else
					WEBC_UINT32 bytesPerLine = BMP_DecodeGetBytesPerRow(decoder);
					WEBC_UINT32 paddingBytes = (4 - (((decoder->info.width * decoder->info.bits) + 7) >> 3)) & 3;
					WEBC_UINT8* linePtr = decoder->bitmapData + (decoder->yPos * bytesPerLine);
#endif
					while (decoder->yPos >= 0)
					{
#if (WEBC_INCLUDE_BMP_PALLETE_FIXES)
						/* If we are reading data and decoder->header.imageDataOffsetBytes is non zero, discard byte
						   until we reach the end */
						if (decoder->header.imageDataOffsetBytes)
						{

							WEBC_UINT32 bytesToProcess = (WEBC_UINT32)(bytesAvailable - bytesRead);
							if (bytesToProcess >= decoder->header.imageDataOffsetBytes)
							{ /* Discard some of the bytes */
								bytesRead += decoder->header.imageDataOffsetBytes;
								decoder->header.imageDataOffsetBytes = 0;
							}
							else
							{ /* Discard all bytes */
								decoder->header.imageDataOffsetBytes -= bytesToProcess;
								bytesRead = bytesAvailable;
							}
						}
#endif
						switch (decoder->info.bits)
						{
							case 24:
							{
								WEBC_INT32 bytesToCopy = (decoder->info.width - decoder->xPos) * 3;
								WEBC_INT32 pixelsToCopy;

								if (bytesToCopy > (bytesAvailable - bytesRead))
								{
									bytesToCopy = (bytesAvailable - bytesRead);
								}

								pixelsToCopy = bytesToCopy / 3;
								if (pixelsToCopy > 0)
								{
									WEBC_UINT8* dst = linePtr + (decoder->xPos * 3) + 2;
									decoder->xPos += pixelsToCopy;

									do
									{
										READ_UINT8(*(dst--), data, bytesRead);
										READ_UINT8(*(dst--), data, bytesRead);
										READ_UINT8(*dst, data, bytesRead);
										dst += 5;
										pixelsToCopy--;
									}
									while (pixelsToCopy > 0);
								}
								break;
							}

							// MA To support 16 bit bitmaps???
#if (WEBC_SUPPORT_BMP_16BITS)
							case 16:
								{
								WEBC_INT32 bytesToCopy = (decoder->info.width - decoder->xPos) * 2;
								WEBC_INT32 pixelsToCopy;
								if (bytesToCopy > (bytesAvailable - bytesRead))
								{
									bytesToCopy = (bytesAvailable - bytesRead);
								}
								pixelsToCopy = bytesToCopy / 2;

								if (pixelsToCopy > 0)
								{
									WEBC_UINT8* dst = linePtr + (decoder->xPos * 2)+1;

									//WEBC_UINT8* dst = linePtr + (decoder->xPos )+1  ;  //can live with +1 ?? what is the point
									decoder->xPos += pixelsToCopy;

									do
									{
										READ_UINT8(dst[0], data, bytesRead);
										READ_UINT8(dst[-1], data, bytesRead);

										dst += 2;
										pixelsToCopy--;

									}
									while (pixelsToCopy > 0);

								}
								break;
								}
#endif
							case 8:
							{
								WEBC_INT32 pixelsToCopy = decoder->info.width - decoder->xPos;

								if (pixelsToCopy > (bytesAvailable - bytesRead))
								{
									pixelsToCopy = (bytesAvailable - bytesRead);
								}

								if (pixelsToCopy > 0)
								{
									rtp_memcpy(linePtr + decoder->xPos, data + bytesRead, pixelsToCopy);
									bytesRead += pixelsToCopy;
									decoder->xPos += pixelsToCopy;
								}
								break;
							}

							case 4:
							{

								WEBC_INT32 pixelsToCopy = decoder->info.width - decoder->xPos;

								if (pixelsToCopy > ((bytesAvailable - bytesRead) << 1))
								{
									pixelsToCopy = ((bytesAvailable - bytesRead) << 1);
								}

								if (pixelsToCopy > 0)
								{
									WEBC_UINT8* dst = linePtr + decoder->xPos;
									WEBC_UINT8 unused;

									decoder->xPos += pixelsToCopy;

									while (pixelsToCopy > 1)
									{
										READ_UINT4_2X(dst[0], dst[1], data, bytesRead);
										pixelsToCopy -= 2;
										dst += 2;
									}

									if (pixelsToCopy == 1)
									{
										READ_UINT4_2X(dst[0], unused, data, bytesRead);
									}
								}
								break;
							}

							case 1:
							{

								WEBC_INT32 pixelsToCopy = decoder->info.width - decoder->xPos;

								if (pixelsToCopy > ((bytesAvailable - bytesRead) << 3))
								{
									pixelsToCopy = ((bytesAvailable - bytesRead) << 3);
								}

								if (pixelsToCopy > 0)
								{
									WEBC_UINT8* dst = linePtr + decoder->xPos;

									decoder->xPos += pixelsToCopy;

									while (pixelsToCopy > 7)
									{
										READ_8BITS(dst, data, bytesRead);
										pixelsToCopy -= 8;
										dst += 8;
									}

									if (pixelsToCopy > 0)
									{
										WEBC_UINT8 scratch[8];
										READ_8BITS(scratch, data, bytesRead);
										rtp_memcpy(dst, scratch, pixelsToCopy);
									}
								}
								break;
							}

							default:
								decoder->yPos = -1;
								*status = BMP_MALFORMED;
								break;
						}
						if (*status == BMP_SUCCESS)
						{
						if (decoder->xPos >= decoder->info.width)
						{
							decoder->xPos = 0;
							bytesRead += paddingBytes;
							if (decoder->yPos == 0)
							{
								decoder->state = BMP_DONE;
								break;
							}
							decoder->yPos--;
#if (WEBC_INCLUDE_BMP_PALLETE_FIXES)
							linePtr -= bytesPerLineDest;
#else
							linePtr -= bytesPerLine;

#endif
						}
						else
						{
							*status = BMP_NEED_DATA;
							break;
						}
						}
					}
				}
				break;

			case BMP_RLE_DEFAULT:
				break;

			case BMP_RLE_ESCAPED:
				break;

			case BMP_RLE_READING_RUN:
				break;

			case BMP_RLE_READING_LITERAL:
				break;

			case BMP_RLE_READING_DELTA:
				break;

			case BMP_DONE:
				return bytesRead;
		}
	}
	while (*status == BMP_SUCCESS);

	return bytesRead;
}

/*---------------------------------------------------------------------------*/
WEBC_INT32 BMP_DecodeGetBufferSize  (BMPImageDecoder* decoder)
{
	if (decoder->state > BMP_READING_IMAGE_INFO)
	{
#if (WEBC_INCLUDE_BMP_PALLETE_FIXES)
		return BMP_DecodeGetDestBytesPerRow(decoder) * decoder->info.height;
#else
		return BMP_DecodeGetBytesPerRow(decoder) * decoder->info.height;

#endif
	}

	return BMP_SIZE_UNKNOWN;
}

/*---------------------------------------------------------------------------*/
WEBC_INT32 BMP_DecodeGetBytesPerRow (BMPImageDecoder* decoder)
{

#if (WEBC_INCLUDE_BMP_PALLETE_FIXES)
WEBC_INT32 r;

	if (decoder->info.bits < 8)
		{
		//r = decoder->info.width/(8/decoder->info.bits;
		//if (decoder->info.bits == 4 || decoder->info.bits == 1)
		r = decoder->info.width;
		}
	else
		r = decoder->info.width*(decoder->info.bits/8);
	return(r);
#else
	return (decoder->info.bits < 24)? decoder->info.width : (decoder->info.width * 3);
#endif
}
#if (WEBC_INCLUDE_BMP_PALLETE_FIXES)

/* Return the destination bytes per row. 1 byte per pixel or 3 */
/* 1 BYTE per Pixel for 1,4,8 bit bitmaps, 2 bytes for 16 bits, and 3 bytes for 24  bit bmp*/
WEBC_INT32 BMP_DecodeGetDestBytesPerRow (BMPImageDecoder* decoder)
{
WEBC_INT32 r;
if(decoder->info.bits < 24 )
{
	if(decoder->info.bits < 16)
	{
		r=decoder->info.width ;
		return(r);
	}
	else
	{
		r= (decoder->info.width *2);
		return(r);

	}
}
else
{
	if(decoder->info.bits ==24)
	{
		r=(decoder->info.width * 3);
		return r;
	}
	else
		return -1;
}

	//r = (decoder->info.bits < 24)? decoder->info.width : (decoder->info.width * 3);
	return(-1);
}

#endif


/*---------------------------------------------------------------------------*/
void BMP_DecodeSetImageBuffer (BMPImageDecoder* decoder,
                               WEBC_UINT8* buffer)
{
	decoder->bitmapData = buffer;
}

/*---------------------------------------------------------------------------*/
void BMP_DecodeDestroy (BMPImageDecoder* decoder)
{
	/* nothing really to do here */
}
