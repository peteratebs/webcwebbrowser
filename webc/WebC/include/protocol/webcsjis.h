#ifndef __WEBCSJIS_H_INCLUDED__
#define __WEBCSJIS_H_INCLUDED__

#include "webctypes.h"

/* Return the length of a JIS character, 1 or 2 */

#define JIS_CHAR_LEN(C)    ((((C) >= 0x81 && (C) <= 0x9f) || ((C) >= 0xe0 && (C) <= 0xfc))? 2 : 1)
#define EUC_JP_CHAR_LEN(C) (((C) & 0x80)? 2 : 1)

#ifdef __cplusplus
extern "C" {
#endif

int webc_jis_to_unicode (WEBC_UINT16 *u, const WEBC_UINT8 *sjisChar);
int webc_unicode_to_jis (WEBC_UINT8 *pjis, WEBC_UINT16 u);

int webc_euc_jp_to_unicode (unsigned short *uc, const unsigned char *eucJp);
int webc_unicode_to_euc_jp (unsigned char *eucJp, unsigned short uc);

#ifdef __cplusplus
}
#endif

#endif // __WEBCSJIS_H_INCLUDED__
