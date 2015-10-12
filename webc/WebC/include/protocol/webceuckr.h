#ifndef __WEBCEUCKR_H_INCLUDED__
#define __WEBCEUCKR_H_INCLUDED__

#define EUC_KR_CHAR_LEN(A) (((A)>=0xa1 && (A)<=0xfe)? 2 : 1)

#ifdef __cplusplus
extern "C" {
#endif

int webc_euc_kr_to_unicode(unsigned short *uc, const unsigned char *eucKr);
int webc_unicode_to_euc_kr(unsigned char *eucKr, unsigned short uc);

#ifdef __cplusplus
}
#endif

#endif // __WEBCEUCKR_H_INCLUDED__
