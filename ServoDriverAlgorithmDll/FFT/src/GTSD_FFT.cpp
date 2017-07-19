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


//////////////////////////////////////////////////////////////////////////
//add wang.bin new function can speed up the fft caculate


//////////////////////////////////////////////////////////////////////////
// complex input type 
//////////////////////////////////////////////////////////////////////////
//*********************************************************************************************************
//*											FFT TRANSFORM MODULE
//*
//* Description: Computes the discrete Fourier transform (DFT) of the given complex vector, storing the result back into the vector.
//*				 The vector can have any length. This is a wrapper function.
//*              
//* Arguments  : x			is a array to storage data
//*				 n			length of array
//*
//* Returns    : 0(false)/out of memory, 1(true)/successful
//*********************************************************************************************************
int transform_x(COMPLEX* x, size_t n)
{
	if (n == 0)
		return 1;
	else if ((n & (n - 1)) == 0)															// Is power of 2
		return transform_radix2_x(x, n);
	else																					// More complicated algorithm for aribtrary sizes
		return transform_bluestein_x(x, n);
}

//*********************************************************************************************************
//*											INVERSE  DFT MODULE
//*
//* Description: Computes the inverse discrete Fourier transform (IDFT) of the given complex vector, storing the result back into the vector.
//*				 The vector can have any length. This is a wrapper function. This transform does not perform scaling,
//*				 so the inverse is not a true inverse.
//*              
//* Arguments  : x			is a array to storage data
//*				 n			length of array
//*
//* Returns    : 0(false)/out of memory, 1(true)/successful
//*********************************************************************************************************
int inverse_transform_x(COMPLEX* x, size_t n)
{
	double tmp;
	int32 i;
	int32 rtn;
	// exchange the real part and imag part
	for (i = 0; i < n; ++i)
	{
		tmp = x[i].real;
		x[i].real = x[i].imag;
		x[i].imag = tmp;
	}
	//do the dft
	rtn = transform_x(x, n);

	// exchange the real part and imag part
	for (i = 0; i < n; ++i)
	{
		tmp = x[i].real;
		x[i].real = x[i].imag;
		x[i].imag = tmp;
	}

	return rtn;

}

//*********************************************************************************************************
//*                                  Cooley-Tukey decimation-in-time algorithm
//*
//* Description: Computes the discrete Fourier transform (DFT) of the given complex vector, storing the result back into the vector.
//*				 The vector's length must be a power of 2. Uses the Cooley-Tukey decimation-in-time radix-2 algorithm.
//*              
//* Arguments  : x			is a array to storage  data
//*				 n			length of array
//*
//* Returns    : 0(false)/ otherwise (n is not a power of 2, or out of memory), 1(true)/successful
//*********************************************************************************************************
int transform_radix2_x(COMPLEX* x, size_t n)
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
			double temp = x[i].real;
			x[i].real = x[j].real;
			x[j].real = temp;
			temp = x[i].imag;
			x[i].imag = x[j].imag;
			x[j].imag = temp;
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
				double tpre = x[j + halfsize].real * cos_table[k] + x[j + halfsize].imag * sin_table[k];
				double tpim = -x[j + halfsize].real * sin_table[k] + x[j + halfsize].imag * cos_table[k];
				x[j + halfsize].real = x[j].real - tpre;
				x[j + halfsize].imag = x[j].imag - tpim;
				x[j].real += tpre;
				x[j].imag += tpim;
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
//* Arguments  : x			is a array to storage data
//*				 n			length of array
//*
//* Returns    : 0(false)/ otherwise (out of memory), 1(true)/successful
//*********************************************************************************************************
int transform_bluestein_x(COMPLEX* x, size_t n)
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
		areal[i] = x[i].real * cos_table[i] + x[i].imag * sin_table[i];
		aimag[i] = -x[i].real * sin_table[i] + x[i].imag * cos_table[i];
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
		x[i].real = creal[i] * cos_table[i] + cimag[i] * sin_table[i];
		x[i].imag = -creal[i] * sin_table[i] + cimag[i] * cos_table[i];
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
//*											FFT 2D TRANSFORM MODULE
//*
//* Description: Computes the discrete Fourier transform (DFT) of the two n number given complex vector, 
//*				 but just compute one n number FFT and some addtion option, and storing the result back into the two vectors.
//*				 this function just used for real input.
//*				 The vector can have any length. This is a wrapper function.
//*              
//* Arguments  : X1		is a array to storage first real part of data
//*				 X2		is a array to storage second real part of data
//*				 N		length of array
//*
//* Returns    : 0(false)/out of memory, 1(true)/successful
//*********************************************************************************************************
int transform2D(COMPLEX* X1, COMPLEX* X2, size_t N)
{
	int32 i;

	//define n lenth space for fft input and result
	COMPLEX* X = new COMPLEX[N];

	/* Forward DFT */
	/* From the two N-point real sequences, x1(n) and x2(n), form the N-point complex
	sequence, x(n) = x1(n) + jx2(n) */
	for (i = 0; i < N; ++i)
	{
		X[i].real = X1[i].real;
		X[i].imag = X2[i].real;
	}
	/* Compute the DFT of x(n), X(k) = DFT{x(n)} */
	int32 rtn = transform_x(X, N);
	if (rtn == 0)
	{
		// return 1 success
		return 0;
	}
	/* The split function performs the additional computations required to get X1(k) and X2(k) from X(k). */
	int32 k;
	X1[0].real = X[0].real;
	X1[0].imag = 0;
	X2[0].real = X[0].imag;
	X2[0].imag = 0;
	X1[N / 2].real = X[N / 2].real;
	X1[N / 2].imag = 0;
	X2[N / 2].real = X[N / 2].imag;
	X2[N / 2].imag = 0;
	for (k = 1; k < N / 2; k++)
	{
		X1[k].real = (X[k].real + X[N - k].real) / 2;
		X1[k].imag = (X[k].imag - X[N - k].imag) / 2;
		X2[k].real = (X[k].imag + X[N - k].imag) / 2;
		X2[k].imag = (X[N - k].real - X[k].real) / 2;
		X1[N - k].real = X1[k].real;
		X1[N - k].imag = -(X1[k].imag);
		X2[N - k].real = X2[k].real;
		X2[(N - k)].imag = -(X2[k].imag);
	}

	delete[] X;
	return 1;
}
//*********************************************************************************************************
//*											inverse FFT 2D TRANSFORM MODULE
//*
//* Description: Computes the discrete Fourier transform (IDFT) of the two n number given complex vector, 
//*				 but just compute one n number FFT and some addtion option, and storing the result back into the two vectors.
//*				 this function just used for real input.
//*				 The vector can have any length. This is a wrapper function.
//*              
//* Arguments  : X1		is a array to storage first fft data
//*				 X2		is a array to storage second fft data
//*				 N		length of array
//*
//* Returns    : 0(false)/out of memory, 1(true)/successful
//*********************************************************************************************************
int inverse_transform2D(COMPLEX* X1, COMPLEX* X2, size_t N)
{
	int32 k;
	//use complex to store the fft result
	COMPLEX* x = new COMPLEX[N];
	if (x == NULL)
	{
		return 0;
	}
	/* Inverse DFT – We now want to get back x1(n) and x2(n) from X1(k) and X2(k) using
	one complex DFT */
	/* Recall that x(n) = x1(n) + jx2(n). Since the DFT operator is linear,
	X(k) = X1(k) + jX2(k). Thus we can express X(k) in terms of X1(k) and X2(k). */
	for (k = 0; k < N; k++)
	{
		x[k].real = X1[k].real - X2[k].imag;
		x[k].imag = X1[k].imag + X2[k].real;
	}

	inverse_transform_x(x, N);

	/* x1(n) is the real part of x(n), and x2(n) is the imaginary part of x(n). */
	for (k = 0; k < N; k++)
	{
		X1[k].real = x[k].real;
		X2[k].real = x[k].imag;
	}

	delete[] x;
	return 1;
}


//*********************************************************************************************************
//*											FFT 2n TRANSFORM MODULE
//*
//* Description: Computes the discrete Fourier transform (DFT) of the N = 2n number given complex vector, 
//*				 but just compute once n number FFT and some addtion, and storing the result back into the vector.
//*				 this function just used for real input and N must be even.
//*				 This is a wrapper function.
//*              
//* Arguments  : X		is a array to storage real part of data
//*				 N		length of array
//*
//* Returns    : 0(false)/out of memory, 1(true)/successful
//*********************************************************************************************************
int transform1(COMPLEX* X, size_t N)
{
	//if ((N % 2) != 0)
	//{
	//	//return 0;
	//}
	int32 DFT_N = N / 2; // dft number
	int32 n, k;
	COMPLEX* x = new COMPLEX[DFT_N]; /* array of complex DFT data */
	COMPLEX* A = new COMPLEX[DFT_N]; /* array of complex A coefficients */
	COMPLEX* B = new COMPLEX[DFT_N]; /* array of complex B coefficients */

	for (k = 0; k < DFT_N; k++)
	{
		A[k].real = 0.5*(1.0 - sin(2 * M_PI / (double)(N)*(double)k));
		A[k].imag = 0.5*(-cos(2 * M_PI / (double)(N)*(double)k));
		B[k].real = 0.5*(1.0 + sin(2 * M_PI / (double)(N)*(double)k));
		B[k].imag = 0.5*(cos(2 * M_PI / (double)(N)*(double)k));
	}

	/* Forward DFT */
	/* From the 2N point real sequence, X(n), for the N-point complex sequence, x(n) */
	for (n = 0; n < DFT_N; n++)
	{
		x[n].real = X[2 * n].real; /* x1(n) = X(2n) */
		x[n].imag = X[(2 * n) + 1].real; /* x2(n) = X(2n + 1) */
	}
	/* Compute the DFT of x(n), X(k) = DFT{x(n)} */
	int32 rtn = transform_x(x, DFT_N);
	if (rtn == 0)
	{
		// return 1 success
		return 0;
	}

	for (k = 0; k < DFT_N; k++)
	{
		X[k].real = (x[k].real * A[k].real) - (x[k].imag * A[k].imag) + (x[DFT_N - k].real * B[k].real) + (x[DFT_N - k].imag * B[k].imag);
		X[k].imag = (x[k].imag * A[k].real) + (x[k].real * A[k].imag) + (x[DFT_N - k].real * B[k].imag) - (x[DFT_N - k].imag * B[k].real);
	}

	/* Use complex conjugate symmetry properties to get the rest of G(k) */
	X[DFT_N].real = (x[0].real - x[0].imag);
	X[DFT_N].imag = 0;

	for (k = 1; k < DFT_N; k++)
	{
		X[(2 * DFT_N) - k].real = X[k].real;
		X[(2 * DFT_N) - k].imag = (-X[k].imag);
	}

	delete[] x;
	delete[] A;
	delete[] B;

	return 1;
}
//*********************************************************************************************************
//*											Inverse FFT 2n TRANSFORM MODULE
//*
//* Description: Computes the discrete Fourier transform (IDFT) of the N = 2n number given complex vector, 
//*				 but just compute one n number IFFT and some addtion, and storing the result back into the vector.
//*				 this function just used for real input and N must be even.
//*				 This is a wrapper function.
//*              
//* Arguments  : X		is a array to storage real part of data
//*				 N		length of array
//*
//* Returns    : 0(false)/out of memory, 1(true)/successful
//*********************************************************************************************************
int inverse_transform1(COMPLEX* X, size_t N)
{
	//if ((N % 2) != 0)
	//{
	//	//return 0;
	//}
	int32 DFT_N = N / 2; // dft number
	int32 i, k;
	COMPLEX* x = new COMPLEX[DFT_N]; /* array of complex DFT data */
	COMPLEX* A = new COMPLEX[DFT_N]; /* array of complex A coefficients */
	COMPLEX* B = new COMPLEX[DFT_N]; /* array of complex B coefficients */
	COMPLEX* IA = new COMPLEX[DFT_N]; /* array of complex A* coefficients */
	COMPLEX* IB = new COMPLEX[DFT_N]; /* array of complex B* coefficients */

	for (k = 0; k < DFT_N; k++)
	{
		A[k].real = 0.5*(1.0 - sin(2 * M_PI / (double)(N)*(double)k));
		A[k].imag = 0.5*(-cos(2 * M_PI / (double)(N)*(double)k));
		B[k].real = 0.5*(1.0 + sin(2 * M_PI / (double)(N)*(double)k));
		B[k].imag = 0.5*(cos(2 * M_PI / (double)(N)*(double)k));
		IA[k].imag = -A[k].imag;
		IA[k].real = A[k].real;
		IB[k].imag = -B[k].imag;
		IB[k].real = B[k].real;
	}
	/* The split function performs the additional computations required to get
	x(k) from X(k). */
	for (k = 0; k < DFT_N; k++)
	{
		x[k].real = X[k].real * A[k].real - X[k].imag * A[k].imag +
			X[DFT_N - k].real * B[k].real + X[DFT_N - k].imag * B[k].imag;
		x[k].imag = X[k].imag * A[k].real + X[k].real * A[k].imag +
			X[DFT_N - k].real * B[k].imag - X[DFT_N - k].imag * B[k].real;
	}


	/* Compute the inverse DFT of x(n), X(k) = IDFT{x(n)} */
	int32 rtn = inverse_transform_x(x, DFT_N);
	if (rtn == 0)
	{
		// return 1 success
		return 0;
	}

	for (i = 0; i < DFT_N; ++i)
	{
		X[2 * i].real = x[i].real;
		X[2 * i + 1].real = x[i].imag;
	}
	delete[] x;
	delete[] A;
	delete[] B;
	delete[] IA;
	delete[] IB;

	return 1;
}

