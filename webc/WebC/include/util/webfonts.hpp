#ifndef __WEBFONTS_INCLUDED__
#define __WEBFONTS_INCLUDED__

#include "webcfg.h"
#include "css.h"
#include "webc.h"
#include "wcconst.hpp"

typedef WEBC_UINT16 WebcFontDescriptor;

#define WEBC_FONT_WEIGHT_MASK    0x0001
#define WEBC_FONT_WEIGHT_NORMAL  0x0000
#define WEBC_FONT_WEIGHT_BOLD    0x0001

#define WEBC_FONT_STYLE_MASK     0x0002
#define WEBC_FONT_STYLE_NORMAL   0x0000
#define WEBC_FONT_STYLE_ITALIC   0x0002

#define WEBC_FONT_SIZE_MASK      0x07fc

#define WEBC_FONT_FAMILY_MASK    0xf800
#define WEBC_FONT_VARIANT_MASK   0x0000

#define WEBC_FONT_FAMILY(X)      (((X) & WEBC_FONT_FAMILY_MASK) >> 11)
#define WEBC_FONT_WEIGHT(X)       ((X) & WEBC_FONT_WEIGHT_MASK)
#define WEBC_FONT_STYLE(X)        ((X) & WEBC_FONT_STYLE_MASK)
#define WEBC_FONT_SIZE(X)        (((X) & WEBC_FONT_SIZE_MASK) >> 2)
#define WEBC_FONT_VARIANT(X)      ((X) & WEBC_FONT_VARIANT_MASK)

#define WEBC_SET_FONT_FAMILY(D,X) {(D) = ((D) & ~WEBC_FONT_FAMILY_MASK) | (WebcFontDescriptor)(((X) << 11) & WEBC_FONT_FAMILY_MASK);}
#define WEBC_SET_FONT_SIZE(D,X)   {(D) = ((D) & ~WEBC_FONT_SIZE_MASK)   | (WebcFontDescriptor)(((X) << 2)  & WEBC_FONT_SIZE_MASK);}

#define WEBC_GET_ABS_FONT_SIZE(X) (CSS_FontSizePointsToAbsolute(WEBC_FONT_SIZE((X))))

#define WEBC_COPY_FONT_WEIGHT(X,Y) {(X) = ((X) & ~WEBC_FONT_WEIGHT_MASK) | ((Y) & WEBC_FONT_WEIGHT_MASK);}
#define WEBC_COPY_FONT_STYLE(X,Y)  {(X) = ((X) & ~WEBC_FONT_STYLE_MASK)  | ((Y) & WEBC_FONT_STYLE_MASK);}
#define WEBC_COPY_FONT_SIZE(X,Y)   {(X) = ((X) & ~WEBC_FONT_SIZE_MASK)   | ((Y) & WEBC_FONT_SIZE_MASK);}
#define WEBC_COPY_FONT_FAMILY(X,Y) {(X) = ((X) & ~WEBC_FONT_FAMILY_MASK) | ((Y) & WEBC_FONT_FAMILY_MASK);}

#define WEBC_SMALLEST_FONT_SIZE_PT   6
#define WEBC_LARGEST_FONT_SIZE_PT    36
#define WEBC_FONT_CACHE_SIZE         (WEBC_CFG_MAX_FONT_FAMILIES * \
                                      (WEBC_LARGEST_FONT_SIZE_PT - WEBC_SMALLEST_FONT_SIZE_PT + 1) * 4)

class WebGraphFont
{
protected:
	int miUseCount;

public:
	WebGraphFont (WebFont font);
	virtual ~WebGraphFont (void);

	virtual void Claim    (void);
	virtual void Release  (void);
	virtual int  UseCount (void);

	WebFont mFont;
};

class WebFontRef
{
protected:
	WebGraphFont *mpFontInstance;

public:
	WebFontRef (WebGraphFont *pInstance = 0);
	~WebFontRef (void);

	WebGraphFont *Get (void);
	void Set (WebGraphFont *pInstance);
	WebFont GetFont (void);
};


#ifdef __cplusplus
extern "C" {
#endif

int webc_font_family_index (
		const WEBC_CHAR* name,
		const WEBC_CHAR* genericName,
		const WEBC_CHAR** nameTable,
		const WEBC_CHAR** genericNameTable,
		int tableSize
	);

#ifdef __cplusplus
}
#endif

#endif // __WEBFONTS_INCLUDED__
