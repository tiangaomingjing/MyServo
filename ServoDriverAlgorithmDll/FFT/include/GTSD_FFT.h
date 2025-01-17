﻿//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	FFT Aanlysis 												//
//	file				:	GTSD_FFT.h													//
//	Description			:	change the file write by liu.g to lib						//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/9/27	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef		__GTSD_FFT_H__
#define		__GTSD_FFT_H__

#include "Basetype_def.h"
#ifndef M_PI
#define  M_PI				3.14159265358979323846
#endif


typedef struct complex
{
	double real;
	double imag;
}COMPLEX;

//////////////////////////////////////////////////////////////////////////
//function  declaration

int transform(double real[], double imag[], size_t n);

int inverse_transform(double real[], double imag[], size_t n);

int transform_radix2(double real[], double imag[], size_t n);

int transform_bluestein(double real[], double imag[], size_t n);

int convolve_real(double x[], double y[], double out[], size_t n);

int convolve_complex(double xreal[], double ximag[], double yreal[], double yimag[], double outreal[], double outimag[], size_t n);

double	IFatanCal(double xq, double xd);

// Private function prototypes
static int32	reverse_bits(int32 x, Uint32 n);
static void		*memdup(void *src, size_t n);
static int32	reverse_bits1(int32 x, Uint32 levels);

//////////////////////////////////////////////////////////////////////////
//new function 

int transform_x(COMPLEX* x, size_t n);

int inverse_transform_x(COMPLEX* x, size_t n);

int transform_radix2_x(COMPLEX* x, size_t n);

int transform_bluestein_x(COMPLEX* x, size_t n);

int transform2D(COMPLEX* X1, COMPLEX* X2, size_t N);

int inverse_transform2D(COMPLEX* X1, COMPLEX* X2, size_t N);

int transform1(COMPLEX* X, size_t N);

int inverse_transform1(COMPLEX* X, size_t N);

#endif
