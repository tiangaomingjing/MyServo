// ServoDriverAlgorithmDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Basetype_def.h"
#include <math.h>
#include <stdlib.h>
#include "GTSD_FFT.h"
#include "ServoDriverAlgorithmDll.h"

//////////////////////////////////////////////////////////////////////////
//fft analysis
//////////////////////////////////////////////////////////////////////////

//*********************************************************************************************************
//*									Response analysis in frequency domain
//*
//* Description: Computes the frequency response from output to input.
//*              
//* Arguments  : r_in		input real data of system
//*				 r_out		output real data of system
//*				 dbb		amplitude frequency gain of system	
//*				 phh		phase frequency characteristic of system
//*
//* Returns    : 0(false), 1(true)/successful
//*********************************************************************************************************
SERVODRIVERALGORITHMDLL_API int	RespAnalysisInFreDomain(double * r_in, double * r_out, double * dbb, double * phh, unsigned int n)
{
	double *i_in, *i_out, *gin, *gout, *angin, *angout;

	double dtmp;

	i_in = (double *)(calloc(n, sizeof(double)));
	i_out = (double *)(calloc(n, sizeof(double)));
	gin = (double *)(calloc(n, sizeof(double)));
	gout = (double *)(calloc(n, sizeof(double)));
	angin = (double *)(calloc(n, sizeof(double)));
	angout = (double *)(calloc(n, sizeof(double)));

	for (int i = 0; i < n; i++)
	{
		i_in[i] = 0;
		i_out[i] = 0;
	}

	transform(r_in, i_in, n);
	transform(r_out, i_out, n);

	for (int i = 0; i < n; i++)
	{
		gin[i] = (sqrt(r_in[i] * r_in[i] + i_in[i] * i_in[i]) * 2.0) / n;
		gout[i] = (sqrt(r_out[i] * r_out[i] + i_out[i] * i_out[i]) * 2.0) / n;
		dbb[i] = 20 * log10(gout[i] / gin[i]);

		angin[i] = IFatanCal(i_in[i], r_in[i]);
		angout[i] = IFatanCal(i_out[i], r_out[i]);

		dtmp = angout[i] - angin[i];

		while ((dtmp < -M_PI) || (dtmp > M_PI))
		{
			if (dtmp > M_PI)
			{
				dtmp = dtmp - 2 * M_PI;
			}
			else
			{
				dtmp = dtmp + 2 * M_PI;
			}
		}

		phh[i] = dtmp;

	}

	free(i_in);
	free(i_out);
	free(gin);
	free(gout);
	free(angin);
	free(angout);

	return	1;
}

//*********************************************************************************************************
//*									signal analysis in frequency domain
//*
//* Description: Computes the fft for time domain  .
//*              
//* Arguments  : 
//*				 r_in		input real data of system
//*				 dbb		amplitude frequency gain of system	
//*				 phh		phase frequency characteristic of system hz
//*				 samplerate		sample rate
//*				 n			signal point number
//* Returns    : 0(false), 1(true)/successful
//*********************************************************************************************************
SERVODRIVERALGORITHMDLL_API int	SignalAnalysisFFT(double * r_in, double * dbb, double * phh, unsigned int n, unsigned int samplerate)
{
	double *i_in, *gin, *angin;

	double dtmp;

	i_in = (double *)(calloc(n, sizeof(double))); //虚部
	gin = (double *)(calloc(n, sizeof(double))); //模
	angin = (double *)(calloc(n, sizeof(double)));//相位


	for (int i = 0; i < n; i++)
	{
		i_in[i] = 0;
	}

	transform(r_in, i_in, n);

	for (int i = 0; i < n; i++)
	{
		gin[i] = (sqrt(r_in[i] * r_in[i] + i_in[i] * i_in[i]) * 2.0) / n;
		dbb[i] = 20 * log10(gin[i]);

		angin[i] = IFatanCal(i_in[i], r_in[i]);

		dtmp = angin[i];

		while ((dtmp < -M_PI) || (dtmp > M_PI))
		{
			if (dtmp > M_PI)
			{
				dtmp = dtmp - 2 * M_PI;
			}
			else
			{
				dtmp = dtmp + 2 * M_PI;
			}
		}

		phh[i] = dtmp; //单位rad

	}

	free(i_in);
	free(gin);
	free(angin);

	return	1;
}
