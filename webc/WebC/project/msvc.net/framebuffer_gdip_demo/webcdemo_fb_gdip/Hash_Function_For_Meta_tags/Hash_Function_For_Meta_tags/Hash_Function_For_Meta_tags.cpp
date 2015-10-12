// Hash_Function_For_Meta_tags.cpp : Defines the entry point for the console application.

//typedef unsigned long HashCode_t;
//typedef    WEBC_INT8;
#include "stdafx.h"
#include "Data_strings.h"
#define RANDOM_SEED            5297
//#define RANDOM_SEED           24539
#define LEFT_BIT_SHIFT        5
#define RIGHT_BIT_SHIFT       2
#define UPPERCASE(X) ((((X) >= 'a') && ((X) <= 'z'))? ((X) - ('a' - 'A')) : (X))
#define LOWERCASE(X) ((((X) >= 'A') && ((X) <= 'Z'))? ((X) - ('A' - 'a')) : (X))
#define miTableSize (172)

;
/*-----------Prototypes--------------*/
unsigned int METATAG_hash(const char *s);
//unsigned char METATAG_hash (const char *s);

int _tmain(int argc, _TCHAR* argv[])
{
	int i;
	//short int k;	
	unsigned int h;
	//HashCode_t h;
	FILE *fp1;
	fp1=fopen("META_TAG_HASH_VALUES.txt","w");
	
	//signed char h;
	//k^=k;
	for(i=0;i<172;i++)
	{
		h = METATAG_hash(MetaTags[i]);
		//k=(short int) h;
		//k=k & 0xFF;
		printf("%s, \t \t hash value is \t\t, %lu \n", MetaTags[i],h);
		fprintf(fp1,"%s,\t\t hash value is \t\t, %lu \n", MetaTags[i],h);
		//k^=k;
	}

	fflush(fp1);
	fclose(fp1);
	printf("the sizeof(unsigned short) is = %d \n",sizeof(unsigned short));
	printf("while the sizeof(char) is = %d \n",sizeof(char));
	typedef unsigned short XMLchar;
	getchar();
	
	return 0;
}

unsigned int METATAG_hash (const char *s)
{
	unsigned int h = RANDOM_SEED;
	for (; *s; s++)
	{
		unsigned char S;
		
		S = LOWERCASE(* s);

		h = h ^ ( (h << LEFT_BIT_SHIFT) + (h >> RIGHT_BIT_SHIFT) + S);
	}

	return (h % miTableSize);
	//return ( h / miTableSize );
}


/*
1)	h= (1/h) ^ (S * RANDOM_SEED)
2)	p=random number, q=randome number, i=0
	h=h+ (S<< pi)*(S>>qi); i++;
	
	*/

/* we may yet have a use for these...
use pi and qi from here... qi???
static short primes[65] = 
     { 173, 281, 349, 463, 541, 659, 733, 863, 941,1069,1151,1291,1373,
      1511,1583,1733,1811,1987,2053,2213,2287,2423,2531,2687,2741,2903,
      2999,3181,3257,3413,3511,3643,3727,3907,3989,4139,4231,4409,4493,
      4657,4751,4937,5003,5179,5279,5443,5521,5693,5791,5939,6053,6221,
      6301,6473,6571,6761,6833,6997,7103,7297,7411,7561,7643,7829,7919};
*/