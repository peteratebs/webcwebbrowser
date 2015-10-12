/*
* EBS - R
graph.c - 
Routines in this file are 
*/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define screenHight		100
#define screenLength	300
#define decayAcel		40 //prevents the sin noise from being to large at the beginning 

void signal(char*);
void signal(char* c);
void sinNoise(char* c, int decay, int frequency , int amp);
void triple_sin_noise(char * trip);
void rand_noise(char* c, char min, char max, char num);
void noisy_signal(char* output);
void ran_noisy_signal(char* output);
//int rand(void);

void signal(char* c)
{
	double a, b;
	int i;
	a = 0.879160251 * screenHight;
    b = 0.169055274 * screenLength;
	for (i = 0; i < screenLength; i++){
		c[i] = (char)floor((a) * (1- exp(-i/b)));
    }
}
void sinNoise(char* c, int decay, int frequency , int amp)
{
	int i;
	for (i = 0; i < screenLength; i++){
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

void triple_sin_noise(char * trip){
	int i;
	char temp[screenLength];
	for (i = 0; i< screenLength; i++)
	{
		trip[i]=0;
		temp[i]=0;
	}
	sinNoise(temp, 350, 13 , 20);
	for (i = 0; i< screenLength; i++)
	{
		trip[i] = (char) temp[i];
	}
	sinNoise(temp, 300, 7 , 40);
	for (i = 0; i< screenLength; i++)
	{
		trip[i] = (char)(temp[i] + trip[i]);
	}
	sinNoise(temp, 250, 29 , 10);
	for (i = 0; i< screenLength; i++)
	{
		trip[i] = (char) (temp[i] + trip[i]);
		trip[i] = trip[i]/3;
	}
}

void rand_noise(char* c, char min, char max, char num)
{
	int i;
	srand ((unsigned int) time(NULL) );
	for (i = 0; i<num; i++)
	{
		c[i] = (char) (rand() % (max-min)) + min;
	}
}

void noisy_signal(char* output)
{
	int i;
	char noise[screenLength];
	char sig[screenLength];
	for (i = 0; i< screenLength; i++)
	{
		sig[i]=0;
		noise[i]=0;
		output[i]=0;
	}
	signal(sig);
	triple_sin_noise(noise);
	for (i = 0; i< screenLength; i++)
	{
		output[i]=(char) noise[i]+sig[i];
	}
}


/*
void ran_noisy_signal(char* output)
{
	int i;
	char noise[screenLength];
	char sig[screenLength];
	for (i = 0; i< screenLength; i++)
	{
		sig[i]=0;
		noise[i]=0;
		output[i]=0;
	}
	signal(sig);
	rand_noise(noise, 90, 110, 300/5));
	for (i = 0; i< screenLength; i++)
	{
		output[i]=(char) noise[i]+sig[i];
	}
}
*/


void main(void){
	int i;
	
	char sig[screenLength];
	char nsig[screenLength];
	char noise[screenLength];
	char noise1[screenLength];
	char noise2[screenLength];
	char noise3[screenLength];
	for (i = 0; i< screenLength; i++)
	{
		sig[i]=0;
		nsig[i]=0;
		noise[i]=0;
		noise1[i]=0;
		noise2[i]=0;
		noise3[i]=0;
	}
	noisy_signal(nsig);
	signal(sig);
	triple_sin_noise(noise);

	
	
	sinNoise(noise1, 400, 13 , 10);
	sinNoise(noise2, 300, 7 , 10);
	sinNoise(noise3, 350, 29 , 10);
	for (i = 0; i< screenLength; i++)
	{
		//printf("sig %d, noise1 %d, noise2 %d, noise3 %d, noiseS %d, \n",  sig[i], noise1[i], noise2[i], noise3[i], noise[i]);
		//printf(" %d, %d,  %d,  %d,  %d,  %d, \n",  sig[i], nsig[i], noise1[i], noise2[i], noise3[i], noise[i]);
		printf(" %d \n",  nsig[i]);
	}

}



