#include <math.h>
#include <windows.h>
#include "Procedural.h"

#define Interpolate CosineInterpolate

float LinearInterpolate(float a,float b, float alpha)
{
	return  a*(1-alpha) + b*alpha;
}

float CosineInterpolate(float a,float b, float alpha)
{
	float ft = alpha * 3.1415927f;
	float f = (1 - (float)cos(ft)) * .5f;
	return  a*(1-f) + b*f;
}


float CubicInterpolate(float v0,float v1,float v2,float v3,float alpha)
{
	float P = (v3 - v2) - (v0 - v1);
	float Q = (v0 - v1) - P;
	float R = v2 - v0;
	float S = v1;
	return P*alpha*alpha*alpha + Q*alpha*alpha + R*alpha + S;
}


float Noise1(LONG x)
{
    x = (x<<13) ^ x;
    return ( 1.0f - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float SmoothedNoise1(LONG x)
{
    return Noise1(x)/2.0f  +  Noise1(x-1)/4.0f  +  Noise1(x+1)/4.0f;
}

float InterpolatedNoise_1(float x)
{
	LONG integer_X    = ((int)(x));
	float fractional_X = x - integer_X;
	float v1 = SmoothedNoise1(integer_X);
	float v2 = SmoothedNoise1(integer_X + 1);
	return Interpolate(v1 , v2 , fractional_X);
}

float PerlinNoise_1D(float x,float persistence,DWORD Number_Of_Octaves)
{
	DWORD i;
	float total = 0;
	float p = persistence;
	DWORD n = Number_Of_Octaves - 1;
	for( i=0; i<n; i++)
	{
		float frequency = (float)2*i;
		float amplitude = (float)p*i;
		total = total + InterpolatedNoise_1(x * frequency) * amplitude;
	}
	return total;
}

//똑같은 코드를 2차원이상의 펄린노이즈 함수에 적용하는것은 쉽다. 

float Noise(LONG x,LONG y)
{
	LONG n = x + y * 57;
	n = (n<<13) ^ n;
	return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float SmoothedNoise1(LONG x, LONG y)
{
	float corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16;
	float sides   = ( Noise(x-1, y)  +Noise(x+1, y)  +Noise(x, y-1)  +Noise(x, y+1) ) /  8;
	float center  =  Noise(x, y) / 4;
	return corners + sides + center;
}

float InterpolatedNoise_1(float x, float y)
{
	LONG integer_X    = int(x);
	float fractional_X = x - integer_X;
	LONG integer_Y    = int(y);
	float fractional_Y = y - integer_Y;
	float v1 = SmoothedNoise1(integer_X,     integer_Y);
	float v2 = SmoothedNoise1(integer_X + 1, integer_Y);
	float v3 = SmoothedNoise1(integer_X,     integer_Y + 1);
	float v4 = SmoothedNoise1(integer_X + 1, integer_Y + 1);
	float i1 = Interpolate(v1 , v2 , fractional_X);
	float i2 = Interpolate(v3 , v4 , fractional_X);
	return Interpolate(i1 , i2 , fractional_Y);
}

float PerlinNoise_2D(float x, float y,float persistence,DWORD Number_Of_Octaves)
{
	float total = 0;
	DWORD i;
	float p = persistence;
	DWORD n = Number_Of_Octaves - 1;
	for(i=0; i<n; i++)
	{
		float frequency = (float)2*i;
		float amplitude = (float)p*i;
		total = total + InterpolatedNoise_1(x * frequency, y * frequency) * amplitude;
	}
	return total;
}
