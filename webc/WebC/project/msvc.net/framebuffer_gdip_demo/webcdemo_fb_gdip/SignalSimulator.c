/*
* EBS - R
graph.c -
Routines in this file are
*/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define decayAcel		40 //prevents the sin noise from being to large at the beginning
#define MAXCONCIEVABLEWIDTH 2048
void InstrumentSignal(unsigned char* c, int Height, int Width);
void sinNoise(unsigned char *c, int decay, int frequency , int amp, int Height, int Width);
void triple_sin_noise(char * trip, int Height, int Width);
void rand_noise(char* c, char min, char max, int num);
void noisy_signal(char* output, int Height, int Width);

void InstrumentSignal(unsigned char* c, int Height, int Width)
{
	double a, b;
	int i;
	a = 0.879160251 * Height;
    b = 0.169055274 * Width;
	for (i = 0; i < Width; i++){
		c[i] = (char)floor((a) * (1- exp(-i/b)));
    }
}
void sinNoise(unsigned char *c, int decay, int frequency , int amp, int Height, int Width)
{
	int i;
	for (i = 0; i < Width; i++){
		if(i < decayAcel)
		{
			c[i] = (char)floor(
				(amp *(
					sin(i * frequency)/pow((double)decayAcel,((double)decayAcel/decay))
					)+1)+ 0.5);
		}
		else
		{
			c[i] = (char)floor(
				(amp *(
					sin(i*frequency)/pow((double)(i),((double)i/decay))
					)+1)+ 0.5);
		}
    }


	//(10*(sin(x*15)/((x)^(x/500))+        sin(x*30)/((x)^(x/300)))
}

void triple_sin_noise(char * trip, int Height, int Width){
	int i;
	char temp[MAXCONCIEVABLEWIDTH];
	for (i = 0; i< Width; i++)
	{
		trip[i]=0;
		temp[i]=0;
	}
	sinNoise(temp, 350, 13 , 20, Height, Width);
	for (i = 0; i< Width; i++)
	{
		trip[i] = (char) temp[i];
	}
	sinNoise(temp, 300, 7 , 40, Height, Width);
	for (i = 0; i< Width; i++)
	{
		trip[i] = (char)(temp[i] + trip[i]);
	}
	sinNoise(temp, 250, 29 , 10, Height, Width);
	for (i = 0; i< Width; i++)
	{
		trip[i] = (char) (temp[i] + trip[i]);
		trip[i] = trip[i]/3;
	}
}

void rand_noise(char* c, char min, char max, int num)
{
	int i;
	static int seeded = 0;
	if (!seeded)
		srand ((unsigned int) time(NULL) );
	seeded = 1;
	for (i = 0; i<num; i++)
	{
		c[i] = (char) (rand() % (max-min)) + min;
	}
}

void noisy_signal(char* output, int Height, int Width)
{
	int i;
	char noise[MAXCONCIEVABLEWIDTH];
	char sig[MAXCONCIEVABLEWIDTH];
	for (i = 0; i< Width; i++)
	{
		sig[i]=0;
		noise[i]=0;
		output[i]=0;
	}
	InstrumentSignal(sig, Height, Width);
	triple_sin_noise(noise, Height, Width);
	for (i = 0; i< Width; i++)
	{
		output[i]=(char) noise[i]+sig[i];
	}
}


/*
void ran_noisy_signal(char* output)
{
	int i;
	char noise[Width];
	char sig[Width];
	for (i = 0; i< Width; i++)
	{
		sig[i]=0;
		noise[i]=0;
		output[i]=0;
	}
	signal(sig);
	rand_noise(noise, 90, 110, 300/5));
	for (i = 0; i< Width; i++)
	{
		output[i]=(char) noise[i]+sig[i];
	}
}
*/

#define MYHeight 100
#define MYWidth  300
void TestGraphmain(void){
	int i;

	char sig[MYWidth];
	char nsig[MYWidth];
	char noise[MYWidth];
	char noise1[MYWidth];
	char noise2[MYWidth];
	char noise3[MYWidth];
	for (i = 0; i< MYWidth; i++)
	{
		sig[i]=0;
		nsig[i]=0;
		noise[i]=0;
		noise1[i]=0;
		noise2[i]=0;
		noise3[i]=0;
	}
	noisy_signal(nsig, MYHeight, MYWidth);
	InstrumentSignal(sig, MYHeight, MYWidth);
	triple_sin_noise(noise, MYHeight, MYWidth);



	sinNoise(noise1, 400, 13 , 10, MYHeight, MYWidth);
	sinNoise(noise2, 300, 7 , 10, MYHeight, MYWidth);
	sinNoise(noise3, 350, 29 , 10, MYHeight, MYWidth);
	for (i = 0; i< MYWidth; i++)
	{
		//printf("sig %d, noise1 %d, noise2 %d, noise3 %d, noiseS %d, \n",  sig[i], noise1[i], noise2[i], noise3[i], noise[i]);
		//printf(" %d, %d,  %d,  %d,  %d,  %d, \n",  sig[i], nsig[i], noise1[i], noise2[i], noise3[i], noise[i]);
		printf(" %d \n",  nsig[i]);
	}

}
