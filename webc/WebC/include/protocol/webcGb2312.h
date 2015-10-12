#ifndef __WEBCGB2312_H_INCLUDED__
#define __WEBCGB2312_H_INCLUDED__



//#define GB2312_CHAR_LEN(A) ( (((A) >= 0x21 && (A) <= 0x29) || ((A) >= 0x30 && (A) <= 0x77)) ? 2 : 1)
#define GB2312_CHAR_LEN(A) ( (((A) & 0x80) && ( ((A & 0x7f) >= 0x21 && (A & 0x7f) <= 0x29) || ((A & 0x7f) >= 0x30 && (A & 0x7f) <= 0x77))) ? 2 : 1)

#ifdef __cplusplus
extern "C" {
#endif

int webc_gb2312_to_unicode(unsigned short *uc, const unsigned char *gb2312);
int webc_unicode_to_gb2312(unsigned char *gb2312, unsigned short uc);

#ifdef __cplusplus
}
#endif

#endif // __WEBCGB2312_H_INCLUDED__
