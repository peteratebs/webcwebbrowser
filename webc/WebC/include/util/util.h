#ifndef __WEBC_UTIL_INCLUDED__
#define __WEBC_UTIL_INCLUDED__

#include "webctypes.h"
#include "wgtypes.hpp"

#define IS_WHITESPACE(C) ((C)==' ' ||  (C)=='\r' || (C)=='\n' || (C)=='\t' || (C)=='\f' || (C)=='\v')
#define IS_ALPHA(C)      (((C)>='A' && (C)<='Z') || ((C)>='a' && (C)<='z')) 
#define IS_DIGIT(X)      ((X)>='0' && (X)<='9')
#define IS_HEX_DIGIT(X)  (((X)>='0' && (X)<='9') || ((X)>='A' && (X)<='F') || ((X)>='a' && (X)<='f'))
#define IS_BREAK_CHAR(C) (((C) == (unsigned short) ' ') || ((C) == (unsigned short) '\t') || ((C) == (unsigned short) '-') || ((unsigned short)C & 0xff00))

#ifdef EBSMIN
#undef EBSMIN
#endif

#ifdef EBSMAX
#undef EBSMAX
#endif

#ifdef EBSCLIP
#undef EBSCLIP
#endif

#define EBSMIN(X,Y)    ((X)<(Y)?(X):(Y))
#define EBSMAX(X,Y)    ((X)>(Y)?(X):(Y))
#define EBSCLIP(S,A,B) (EBSMAX(A,EBSMIN(S,B)))
#define EBSABS(X)      ((X)<0?(-(X)):(X))

#define UTIL_SAFE_LONG_TO_SHORT(L)   ((short)EBSCLIP(L, -0x7fff, 0x7fff))
#define UTIL_SAFE_ULONG_TO_SHORT(L)  ((short)EBSMIN(L, 0x7fff))
#define UTIL_SAFE_ULONG_TO_USHORT(L) ((unsigned short)EBSMIN(L, 0xffff))
#define UTIL_SAFE_LONG_TO_USHORT(L)  ((unsigned short)EBSCLIP(L, 0, 0xffff))

#define UPPERCASE(X) ((((X) >= 'a') && ((X) <= 'z'))? ((X) - ('a' - 'A')) : (X))
#define LOWERCASE(X) ((((X) >= 'A') && ((X) <= 'Z'))? ((X) - ('A' - 'a')) : (X))

#ifdef __cplusplus
extern "C" {
#endif

int  GetSum(DISPLAY_INT *array, int start, int end);
int  GetMax(DISPLAY_INT *array, int start, int end);
int  GetMin(DISPLAY_INT *array, int start, int end);
int  GetMinIndex(DISPLAY_INT *array, int start, int end);
void ClipCeiling(DISPLAY_INT *array, int start, int end, int ceiling);
void ClipFloor(DISPLAY_INT *array, int start, int end, int floor);
void DistributeEven(DISPLAY_INT *array, int start, int end, int pool);
void DistributeWeighted(DISPLAY_INT *array, int start, int end, int pool);
void DistributeWeightedStrict(DISPLAY_INT *array, int start, int end, int pool);
void VectorSum(DISPLAY_INT *v1, int start, int end, DISPLAY_INT *v2, int v2index, DISPLAY_INT *v3, int v3index);
void VectorDiff(DISPLAY_INT *v1, int start, int end, DISPLAY_INT *v2, int v2index, DISPLAY_INT *v3, int v3index);
void VectorMax(DISPLAY_INT *v1, int start, int end, DISPLAY_INT *v2, int v2index, DISPLAY_INT *v3, int v3index);
void VectorMin(DISPLAY_INT *v1, int start, int end, DISPLAY_INT *v2, int v2index, DISPLAY_INT *v3, int v3index);

// tbd
//DISPLAY_INT DisplayIntMulDiv(DISPLAY_INT x, DISPLAY_INT m, DISPLAY_INT d);

int ebs_BinarySearch(void *item, void *list, int size, int (*cmp)(void *, void *, int));
int ebs_LinearSearch(void *item, void *list, int size, int (*cmp)(void *, void *, int));

// tbd - get rid of these four
long tc_strlistcmp(char *str, const char **list, long size);

#ifdef __cplusplus
}
#endif

#define EBS_MAX_DATE_STR_LEN	34

#endif
