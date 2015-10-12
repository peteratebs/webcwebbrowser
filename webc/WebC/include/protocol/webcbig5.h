#ifndef __WEBCBIG5_H_INCLUDED__
#define __WEBCBIG5_H_INCLUDED__

#define BIG5_CHAR_LEN(A) (((A)>=0xa1 && (A)<=0xf9)? 2 : 1)

#ifdef __cplusplus
extern "C" {
#endif

int webc_big5_to_unicode(unsigned short *uc, const unsigned char *big5);
int webc_unicode_to_big5(unsigned char *big5, unsigned short uc);

#ifdef __cplusplus
}
#endif

#endif // __WEBCBIG5_H_INCLUDED__
