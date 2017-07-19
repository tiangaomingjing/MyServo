//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	FFT Aanlysis 												//
//	file				:	GTSD_FFT.cpp												//
//	Description			:	change the file write by liu.g to lib						//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/9/27	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <math.h>
#include <stdlib.h>
#include "Basetype_def.h"
#include "GTSD_FFT.h"


//*********************************************************************************************************
//*											FFT TRANSFORM MODULE
//*
//* Description: Computes the discrete Fourier transform (DFT) of the given complex vector, storing the result back into the vector.
//*				 The vector can have any length. This is a wrapper function.
//*              
//* Arguments  : real		is a array to storage real part of data
//*				 imag		is a array to storage imag part of data
//*				 n			length of array
//*
//* Returns    : 0(false)/out of memory, 1(true)/successful
//*********************************************************************************************************
int transform(double real[], double imag[], size_t n)
{
	if (n == 0)
		return 1;
	else if ((n & (n - 1)) == 0)															// Is power of 2
		return transform_radix2(real, imag, n);
	else																					// More complicated algorithm for aribtrary sizes
		return transform_bluestein(real, imag, n);
}

//*********************************************************************************************************
//*											INVERSE  DFT MODULE
//*
//* Description: Computes the inverse discrete Fourier transform (IDFT) of the given complex vector, storing the result back into the vector.
//*				 The vector can have any length. This is a wrapper function. This transform does not perform scaling,
//*				 so the inverse is not a true inverse.
//*              
//* Arguments  : real		is a array to storage real part of data
//*				 imag		is a array to storage imag part of data
//*				 n			length of array
//*
//* Returns    : 0(false)/out of memory, 1(true)/successful
//*********************************************************************************************************
int inverse_transform(double real[], double imag[], size_t n)
{
	return transform(imag, real, n);
}

//*********************************************************************************************************
//*                                  Cooley-Tukey decimation-in-time algorithm
//*
//* Description: Computes the discrete Fourier transform (DFT) of the given complex vector, storing the result back into the vector.
//*				 The vector's length must be a power of 2. Uses the Cooley-Tukey decimation-in-time radix-2 algorithm.
//*              
//* Arguments  : real		is a array to storage real part of data
//*				 imag		is a array to storage imag part of data
//*				 n			length of array
//*
//* Returns    : 0(false)/ otherwise (n is not a power of 2, or out of memory), 1(true)/successful
//*********************************************************************************************************
int transform_radix2(double real[], double imag[], size_t n)
{
	// Variables
	unsigned int	levels;
	double			*cos_table, *sin_table;
	size_t			size;
	size_t			i;

	size_t temp = n;
	levels = 0;
	while (temp > 1)
	{
		levels++;
		temp >>= 1;
	}
	if (1 << levels != n)
		return 0;																			// n is not a power of 2
	// Trignometric tables
	if (SIZE_MAX / sizeof(double) < n / 2)
		return 0;
	size = (n / 2) * sizeof(double);
	cos_table = (double *)(malloc(size));

	if (cos_table == NULL)
		return 0;

	sin_table = (double *)(malloc(size));
	if (sin_table == NULL)
	{
		free(cos_table);
		return 0;
	}
	for (i = 0; i < n / 2; i++)
	{
		cos_table[i] = cos(2 * M_PI * i / n);
		sin_table[i] = sin(2 * M_PI * i / n);
	}

	// Bit-reversed addressing permutation
	for (i = 0; i < n; i++)
	{
		size_t j = reverse_bits(i, levels);
		if (j > i)
		{
			double temp = real[i];
			real[i] = real[j];
			real[j] = temp;
			temp = imag[i];
			imag[i] = imag[j];
			imag[j] = temp;
		}
	}

	// Cooley-Tukey decimation-in-time radix-2 FFT
	for (size = 2; size <= n; size *= 2)
	{
		size_t halfsize = size / 2;
		size_t tablestep = n / size;
		for (i = 0; i < n; i += size)
		{
			size_t j;
			size_t k;
			for (j = i, k = 0; j < i + halfsize; j++, k += tablestep)
			{
				double tpre = real[j + halfsize] * cos_table[k] + imag[j + halfsize] * sin_table[k];
				double tpim = -real[j + halfsize] * sin_table[k] + imag[j + halfsize] * cos_table[k];
				real[j + halfsize] = real[j] - tpre;
				imag[j + halfsize] = imag[j] - tpim;
				real[j] += tpre;
				imag[j] += tpim;
			}
		}
		if (size == n)																		// Prevent overflow in 'size *= 2'
			break;
	}
	free(cos_table);
	free(sin_table);
	return 1;
}

//*********************************************************************************************************
//*									Bluestein's chirp z-transform algorithm 
//*
//* Description: Computes the discrete Fourier transform (DFT) of the given complex vector, storing the result back into the vector.
//*				 The vector can have any length. This requires the convolution function, which in turn requires the radix-2 FFT function.
//*              
//* Arguments  : real		is a array to storage real part of data
//*				 imag		is a array to storage imag part of data
//*				 n			length of array
//*
//* Returns    : 0(false)/ otherwise (out of memory), 1(true)/successful
//*********************************************************************************************************
int transform_bluestein(double real[], double imag[], size_t n)
{
	// Variables
	int status = 0;
	double *cos_table, *sin_table;
	double *areal, *aimag;
	double *breal, *bimag;
	double *creal, *cimag;
	size_t m;
	size_t size_n, size_m;
	size_t i;

	// Find a power-of-2 convolution length m such that m >= n * 2 + 1
	size_t target;
	if (n > (SIZE_MAX - 1) / 2)
		return 0;
	target = n * 2 + 1;
	for (m = 1; m < target; m *= 2)
	{
		if (SIZE_MAX / 2 < m)
			return 0;
	}
	// Allocate memory
	if (SIZE_MAX / sizeof(double) < n || SIZE_MAX / sizeof(double) < m)
		return 0;
	size_n = n * sizeof(double);
	size_m = m * sizeof(double);
	cos_table = (double *)(malloc(size_n));         if (cos_table == NULL) goto cleanup0;
	sin_table = (double *)(malloc(size_n));         if (sin_table == NULL) goto cleanup1;
	areal = (double *)(calloc(m, sizeof(double)));  if (areal == NULL) goto cleanup2;
	aimag = (double *)(calloc(m, sizeof(double)));  if (aimag == NULL) goto cleanup3;
	breal = (double *)(calloc(m, sizeof(double)));  if (breal == NULL) goto cleanup4;
	bimag = (double *)(calloc(m, sizeof(double)));  if (bimag == NULL) goto cleanup5;
	creal = (double *)(malloc(size_m));             if (creal == NULL) goto cleanup6;
	cimag = (double *)(malloc(size_m));             if (cimag == NULL) goto cleanup7;

	// Trignometric tables
	for (i = 0; i < n; i++)
	{
		double temp = M_PI * (size_t)((unsigned long long)i * i % ((unsigned long long)n * 2)) / n;
		// Less accurate version if long long is unavailable: double temp = M_PI * i * i / n;
		cos_table[i] = cos(temp);
		sin_table[i] = sin(temp);
	}

	// Temporary vectors and preprocessing
	for (i = 0; i < n; i++)
	{
		areal[i] = real[i] * cos_table[i] + imag[i] * sin_table[i];
		aimag[i] = -real[i] * sin_table[i] + imag[i] * cos_table[i];
	}
	breal[0] = cos_table[0];
	bimag[0] = sin_table[0];
	for (i = 1; i < n; i++)
	{
		breal[i] = breal[m - i] = cos_table[i];
		bimag[i] = bimag[m - i] = sin_table[i];
	}

	// Convolution
	status = convolve_complex(areal, aimag, breal, bimag, creal, cimag, m);

	// Postprocessing
	for (i = 0; i < n; i++)
	{
		real[i] = creal[i] * cos_table[i] + cimag[i] * sin_table[i];
		imag[i] = -creal[i] * sin_table[i] + cimag[i] * cos_table[i];
	}

	// Clean-up
	free(cimag);
cleanup7:
	free(creal);
cleanup6:
	free(bimag);
cleanup5:
	free(breal);
cleanup4:
	free(aimag);
cleanup3:
	free(areal);
cleanup2:
	free(sin_table);
cleanup1:
	free(cos_table);
cleanup0:
	return status;
}

//*********************************************************************************************************
//*											circular convolution	
//*
//* Description: Computes the circular convolution of the given real vectors. Each vector's length must be the same.
//*              
//* Arguments  : x			data x vector input for real convolution computes
//*				 y			data y vector input for real convolution computes
//*				 n			length of array
//*
//* Returns    : 0(false)/ otherwise (out of memory), 1(true)/successful
//*********************************************************************************************************
int convolve_real(double x[], double y[], double out[], size_t n)
{
	double *ximag, *yimag, *zimag;
	int status = 0;
	ximag = (double *)(calloc(n, sizeof(double)));  if (ximag == NULL) goto cleanup0;
	yimag = (double *)(calloc(n, sizeof(double)));  if (yimag == NULL) goto cleanup1;
	zimag = (double *)(calloc(n, sizeof(double)));  if (zimag == NULL) goto cleanup2;

	status = convolve_complex(x, ximag, y, yimag, out, zimag, n);
	free(zimag);
cleanup2:
	free(yimag);
cleanup1:
	free(ximag);
cleanup0:
	return status;
}

//*********************************************************************************************************
//*											circular convolution	
//*
//* Description: Computes the circular convolution of the given complex vectors. Each vector's length must be the same.
//*              
//* Arguments  : xreal			real data x vector input for complex convolution computes
//*				 ximag			imag data x vector input for complex convolution computes
//*				 yreal			real data y vector input for complex convolution computes
//*				 yimag			imag data y vector input for complex convolution computes
//*				 n				length of array
//*
//* Returns    : 0(false)/ otherwise (out of memory), 1(true)/successful
//*********************************************************************************************************
int convolve_complex(double xreal[], double ximag[], double yreal[], double yimag[], double outreal[], double outimag[], size_t n)
{
	int status = 0;
	size_t size;
	size_t i;
	if (SIZE_MAX / sizeof(double) < n)
		return 0;
	size = n * sizeof(double);
	xreal = (double *)(memdup(xreal, size));  if (xreal == NULL) goto cleanup0;
	ximag = (double *)(memdup(ximag, size));  if (ximag == NULL) goto cleanup1;
	yreal = (double *)(memdup(yreal, size));  if (yreal == NULL) goto cleanup2;
	yimag = (double *)(memdup(yimag, size));  if (yimag == NULL) goto cleanup3;

	if (!transform(xreal, ximag, n))
		goto cleanup4;
	if (!transform(yreal, yimag, n))
		goto cleanup4;
	for (i = 0; i < n; i++)
	{
		double temp = xreal[i] * yreal[i] - ximag[i] * yimag[i];
		ximag[i] = ximag[i] * yreal[i] + xreal[i] * yimag[i];
		xreal[i] = temp;
	}
	if (!inverse_transform(xreal, ximag, n))
		goto cleanup4;
	for (i = 0; i < n; i++)
	{																							// Scaling (because this FFT implementation omits it)
		outreal[i] = xreal[i] / n;
		outimag[i] = ximag[i] / n;
	}
	status = 1;

cleanup4:
	free(yimag);
cleanup3:
	free(yreal);
cleanup2:
	free(ximag);
cleanup1:
	free(xreal);
cleanup0:
	return status;
}

/************************************************************************/
/* 循环level次，一个个取出最低位，然后左移                              */
/************************************************************************/
static int32 reverse_bits(int32 x, Uint32 n)
{
	size_t result = 0;
	unsigned int i;
	for (i = 0; i < n; i++, x >>= 1)
		result = (result << 1) | (x & 1);
	return result;
}
/************************************************************************/
/* for example 001----100 反转位数，    循环change次。 较小一倍 ，交换对应位上的值                                                               */
/************************************************************************/
static int32 reverse_bits1(int32 x, Uint32 levels)
{
	int32	result = 0;
	Uint32	i, j;
	//交换的次数，根据阶数来确定，levels = 2--交换1次（10）， levels = 3交换一次（001-100），levels = 4交换两次（0001--1000），levels = 5也是交换两次
	//从上述规律可以得到如下交换次数
	Uint32  change = 0;
	change = levels >> 1;
	//假如为奇数时保持中间的值
	Uint32  mid = 0;

	for (i = 0; i < change; i++)
	{
		//假如level是奇数，取中间不需要交换的值
		if (levels % 2 != 0)
		{
			mid = (1 << (levels >> 1))&x;
		}
		else
		{
			mid = 0;
		}
		j = levels - 1 - i;
		result |= (mid | ((x&(1 << j)) >> j) | ((x & 1) << j));
	}
	return result;
}

static void *memdup(void *src, size_t n)
{
	void *dest = malloc(n);
	if (dest != NULL)
		memcpy(dest, src, n);
	return dest;
}

//********************************************************************************
//	Function:		IFatanCal
//	Description:	calculate the phase of synthetic vector
//	input:			xd/value of d axis, xq/value of q axis					
//	output:			none
//	return:			phase between 0 to 2pi(0----2pi)

//	Author:			liu.g
//	date:			2014/03/19
//********************************************************************************
double	IFatanCal(double xq, double xd)
{
	double	phs_temp, abs_xq, abs_xd;
	double	dtmp;

	if (xd != 0.0)
	{
		abs_xq = abs(xq);
		abs_xd = abs(xd);

		dtmp = abs_xq / abs_xd;
		phs_temp = atan(dtmp);

		if (xd < 0.0)
		{
			phs_temp = M_PI - phs_temp;
		}
	}
	else
	{
		phs_temp = M_PI / 2.0;
	}

	if (xq < 0.0)
	{
		phs_temp = M_PI*2.0 - phs_temp;
	}

	return	phs_temp;

}

