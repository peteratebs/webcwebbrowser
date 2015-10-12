#ifndef _DISPLAY_DRV_H_
#define _DISPLAY_DRV_H_

/* status message types  (status message 16 bit value) */
#define EINK_STAT_INIT			1
#define EINK_STAT_SLEEP			2
#define EINK_STAT_RUNNING		3
#define EINK_STAT_LOAD_DONE		4
#define EINK_STAT_COPY_DONE		5
#define EINK_STAT_UPDATE_DONE	6

/* error return codes */
#define EINK_ERR_NULL_PTR		-1
#define EINK_ERR_BAD_XOFF_WID	-2
#define EINK_ERR_BAD_YOFF_HT	-3
#define EINK_ERR_BAD_IMG_IND	-4
#define EINK_ERR_BAD_ROTATION	-5
#define EINK_ERR_LOAD_BUSY		-6
#define EINK_ERR_COPY_BUSY		-7
#define EINK_ERR_NOT_RUNNING	-8
#define EINK_ERR_UPDATE_BUSY	-9
#define EINK_ERR_NOT_INIT		-10
#define EINK_ERR_IMG_FORMAT		-11
#define EINK_ERR_X_ALIGN		-12
#define EINK_ERR_DATA_ALIGN		-13

/* status bits for status return value and
 * status message 32 bit value
 */
#define EIS_INIT		1
#define EIS_RUNNING		2
#define EIS_LOAD_BUSY	4
#define EIS_COPY_BUSY	8
#define EIS_UPDATE_BUSY	0x10

/* EINK_IMAGE_TYPE filed defines */
#define EIT_BITS_MASK	0xf
#define		EIT_BITS_1	0
#define		EIT_BITS_2	1
#define		EIT_BITS_4	2
#define		EIT_BITS_8	3
#define		EIT_BITS_16	4
#define		EIT_BITS_24	5
#define		EIT_BITS_32	6

#define EIT_FORMAT_MASK	0xff0
	/* native formats, copied directly to EInk	
	 * pImageData must be correct format for direct loading
	 * of the requested bitsPerPixel
	 * Intended for use with internally stored dialog pieces
	 */
#define		EIT_FORMAT_WORD_DIRECT	0
	/* 8 bits per pixel converted to native format	
	 * pImageData must be in "standard" format
	 * one byte per pixel, value in most significant bits
	 * data is converted to properload  format by driver
	 * so pImageData can be reused as soon as the function returns
	 * Intended for use with browser
	 */
#define		EIT_FORMAT_BYTE_CONVERT	(1 << 4)

typedef enum
{
	EIT_8BITS_DIRECT = (EIT_BITS_8 | EIT_FORMAT_WORD_DIRECT),
	EIT_4BITS_DIRECT = (EIT_BITS_4 | EIT_FORMAT_WORD_DIRECT),
	EIT_1BIT_DIRECT = (EIT_BITS_1 | EIT_FORMAT_WORD_DIRECT),
	EIT_CONVERT_TO_4BITS = (EIT_BITS_4 | EIT_FORMAT_BYTE_CONVERT),
	EIT_CONVERT_TO_1BIT = (EIT_BITS_1 | EIT_FORMAT_BYTE_CONVERT),
} EINK_IMAGE_TYPE;

/* bits in the imageBitcountMask */
#define DI_BITS_1_MASK		0x1
#define DI_BITS_2_MASK		0x2
#define DI_BITS_4_MASK		0x4
#define DI_BITS_8_MASK		0x8
#define DI_BITS_16_MASK		0x10
#define DI_BITS_24_MASK		0x20
#define DI_BITS_32_MASK		0x30

/* bits in the imageFormatMask */
#define DI_FORMAT_WORD_DIRECT_MASK		0x1
#define DI_FORMAT_BYTE_CONVERT_MASK		0x2

typedef struct _display_info_
{
	int rows;
	int columns;
	int maxBitsPerPixel;
	DWORD imageFormatMask;
	DWORD imageBitcountMask;
} DISPLAY_INFO;

/* startup EInk system (also error reset)
 * post message when complete
 */
extern int EInk_ResetInit();

/* turn off power to PMIC and EInk (SDRAM auto-refresh)
 * turn off PLL
 * must have done ResetInit and notbe in error state
 * Post message when complete
 */
extern int EInk_Sleep();

/* turn off power to PMIC and EInk (SDRAM auto-refresh)
 * PLL keep running
 * must have done ResetInit and notbe in error state
 * Post message when complete
 */
extern int EInk_Standby();

/* wake up from sleep
 * must have done ResetInit and not be in error state
 * Post message when complete
 */
extern int EInk_Run();

/* return OR of relevant EIS_xxx bits */
extern DWORD EInk_GetCurrentStatus();

/* set index of image in EInk internal ram to use (may be a NOP)
 * cannot do if LOAD_BUSY or COPY_BUSY
 * must be running 
 * (at startup imageIndex is 0)
 */
extern int EInk_SetImageIndex(int index);

/* set rotation type of image load
 * value must be 0 (0 degrees) or 2 (180 degrees)
 * cannot do if LOAD_BUSY
 * must be running 
 * (at startup rotation is 0)
 */
extern int EInk_SetImageRotation(int rotation);

/* load pImageData into specified area of
 * current internal image memory
 * pImageData better be appropriately aligned
 * will use appropriate command if area is all of display
 * must have xStart >= 0, width > 0, (xStart + width) <= 320
 * must have yStart >= 0, height > 0, (yStart + height) <= 240
 * for 1 bit per pixel xStart divisible by 16
 * for 4 bits per pixel xStart divisible by 4
 * for 8 bit per pixel xStart divisible by 2
 * Post message when complete
 * cannot do if LOAD_BUSY or COPY_BUSY
 * must be running 
 */
extern int EInk_LoadImage(EINK_IMAGE_TYPE imgType,
						  int xStart,
						  int yStart,
						  int width,
						  int height,
						  void *pImageData);

/* copy specified area of current image memory to update memory
 * and use the result to display the result
 * will use appropriate command if area is all of display
 * must have xStart >= 0, width > 0, (xStart + width) <= 320
 * must have yStart >= 0, height > 0, (yStart + height) <= 240
 * imgType bitsPerPixel detemines wave form for display
 * Post message when complete
 * cannot do if UPDATE_BUSY or COPY_BUSY
 * must be running 
 */
extern int EInk_UpdateDisplay(EINK_IMAGE_TYPE imgType,
						  int xStart,
						  int yStart,
						  int width,
						  int height);

/* load pImageData into specified area of
 * current internal image memory
 * copy to update memory and display
 * must have xStart >= 0, width > 0, (xStart + width) <= 320
 * must have yStart >= 0, height > 0, (yStart + height) <= 240
 * for 1 bit per pixel xStart divisible by 16
 * for 4 bits per pixel xStart divisible by 4
 * for 8 bit per pixel xStart divisible by 2
 * pImageData better be appropriately aligned
 * will use appropriate commands if area is all of display
 * Post message when complete
 * cannot do if LOAD_BUSY or COPY_BUSY or UPDATE_BUSY
 * must be running 
 */
extern int EInk_LoadAndDisplayImage(EINK_IMAGE_TYPE loadImgType,
							EINK_IMAGE_TYPE displayImgType,
							int xStart,
							int yStart,
							int width,
							int height,
							void *pImageData);

/* Fill in DISPLAY_INFO struct passed in
 * if imageFormatMask != 0 on input only 1 bit may be set
 * imageBitcountMask must be 0 on input
 * and will have bits set for bits per pixel for the specified format
 * similarly
 * if imageBitcountMask != 0 on input only 1 bit may be set
 * imageFormatMask must be 0 on input
 * and will have bits set for formats for the specifed bits per pixel
 * if both masks are 0 on input each will be set with all
 * values valis for any value of the other
 */
extern int EInk_GetDisplayInfo(DISPLAY_INFO *pInfo);

#endif _DISPLAY_DRV_H_
