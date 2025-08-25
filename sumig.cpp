#include "stdafx.h"
#include "sumig.h"
#include "math.h"
#include "util.h"
#include "pfafft.h"

//#define NTAB 240
//static struct {
//	int n;  
//	float c;
//} nctab[NTAB] = {
//{       1, 0.000052f},
//{       2, 0.000061f},
//{       3, 0.000030f},
//{       4, 0.000053f},
//{       5, 0.000066f},
//{       6, 0.000067f},
//{       7, 0.000071f},
//{       8, 0.000062f},
//{       9, 0.000079f},
//{      10, 0.000080f},
//{      11, 0.000052f},
//{      12, 0.000069f},
//{      13, 0.000103f},
//{      14, 0.000123f},
//{      15, 0.000050f},
//{      16, 0.000086f},
//{      18, 0.000108f},
//{      20, 0.000101f},
//{      21, 0.000098f},
//{      22, 0.000135f},
//{      24, 0.000090f},
//{      26, 0.000165f},
//{      28, 0.000084f},
//{      30, 0.000132f},
//{      33, 0.000158f},
//{      35, 0.000138f},
//{      36, 0.000147f},
//{      39, 0.000207f},
//{      40, 0.000156f},
//{      42, 0.000158f},
//{      44, 0.000176f},
//{      45, 0.000171f},
//{      48, 0.000185f},
//{      52, 0.000227f},
//{      55, 0.000242f},
//{      56, 0.000194f},
//{      60, 0.000215f},
//{      63, 0.000233f},
//{      65, 0.000288f},
//{      66, 0.000271f},
//{      70, 0.000248f},
//{      72, 0.000247f},
//{      77, 0.000285f},
//{      78, 0.000395f},
//{      80, 0.000285f},
//{      84, 0.000209f},
//{      88, 0.000332f},
//{      90, 0.000321f},
//{      91, 0.000372f},
//{      99, 0.000400f},
//{     104, 0.000391f},
//{     105, 0.000358f},
//{     110, 0.000440f},
//{     112, 0.000367f},
//{     117, 0.000494f},
//{     120, 0.000413f},
//{     126, 0.000424f},
//{     130, 0.000549f},
//{     132, 0.000480f},
//{     140, 0.000450f},
//{     143, 0.000637f},
//{     144, 0.000497f},
//{     154, 0.000590f},
//{     156, 0.000626f},
//{     165, 0.000654f},
//{     168, 0.000536f},
//{     176, 0.000656f},
//{     180, 0.000611f},
//{     182, 0.000730f},
//{     195, 0.000839f},
//{     198, 0.000786f},
//{     208, 0.000835f},
//{     210, 0.000751f},
//{     220, 0.000826f},
//{     231, 0.000926f},
//{     234, 0.000991f},
//{     240, 0.000852f},
//{     252, 0.000820f},
//{     260, 0.001053f},
//{     264, 0.000987f},
//{     273, 0.001152f},
//{     280, 0.000952f},
//{     286, 0.001299f},
//{     308, 0.001155f},
//{     312, 0.001270f},
//{     315, 0.001156f},
//{     330, 0.001397f},
//{     336, 0.001173f},
//{     360, 0.001259f},
//{     364, 0.001471f},
//{     385, 0.001569f},
//{     390, 0.001767f},
//{     396, 0.001552f},
//{     420, 0.001516f},
//{     429, 0.002015f},
//{     440, 0.001748f},
//{     455, 0.001988f},
//{     462, 0.001921f},
//{     468, 0.001956f},
//{     495, 0.002106f},
//{     504, 0.001769f},
//{     520, 0.002196f},
//{     528, 0.002127f},
//{     546, 0.002454f},
//{     560, 0.002099f},
//{     572, 0.002632f},
//{     585, 0.002665f},
//{     616, 0.002397f},
//{     624, 0.002711f},
//{     630, 0.002496f},
//{     660, 0.002812f},
//{     693, 0.002949f},
//{     715, 0.003571f},
//{     720, 0.002783f},
//{     728, 0.003060f},
//{     770, 0.003392f},
//{     780, 0.003553f},
//{     792, 0.003198f},
//{     819, 0.003726f},
//{     840, 0.003234f},
//{     858, 0.004354f},
//{     880, 0.003800f},
//{     910, 0.004304f},
//{     924, 0.003975f},
//{     936, 0.004123f},
//{     990, 0.004517f},
//{    1001, 0.005066f},
//{    1008, 0.003902f},
//{    1040, 0.004785f},
//{    1092, 0.005017f},
//{    1144, 0.005599f},
//{    1155, 0.005380f},
//{    1170, 0.005730f},
//{    1232, 0.005323f},
//{    1260, 0.005112f},
//{    1287, 0.006658f},
//{    1320, 0.005974f},
//{    1365, 0.006781f},
//{    1386, 0.006413f},
//{    1430, 0.007622f},
//{    1456, 0.006679f},
//{    1540, 0.007032f},
//{    1560, 0.007538f},
//{    1584, 0.007126f},
//{    1638, 0.007979f},
//{    1680, 0.007225f},
//{    1716, 0.008961f},
//{    1820, 0.008818f},
//{    1848, 0.008427f},
//{    1872, 0.009004f},
//{    1980, 0.009398f},
//{    2002, 0.010830f},
//{    2145, 0.012010f},
//{    2184, 0.010586f},
//{    2288, 0.012058f},
//{    2310, 0.011673f},
//{    2340, 0.011700f},
//{    2520, 0.011062f},
//{    2574, 0.014313f},
//{    2640, 0.013021f},
//{    2730, 0.014606f},
//{    2772, 0.013216f},
//{    2860, 0.015789f},
//{    3003, 0.016988f},
//{    3080, 0.014911f},
//{    3120, 0.016393f},
//{    3276, 0.016741f},
//{    3432, 0.018821f},
//{    3465, 0.018138f},
//{    3640, 0.018892f},
//{    3696, 0.018634f},
//{    3960, 0.020216f},
//{    4004, 0.022455f},
//{    4095, 0.022523f},
//{    4290, 0.026087f},
//{    4368, 0.023474f},
//{    4620, 0.024590f},
//{    4680, 0.025641f},
//{    5005, 0.030303f},
//{    5040, 0.025253f},
//{    5148, 0.030364f},
//{    5460, 0.031250f},
//{    5544, 0.029412f},
//{    5720, 0.034404f},
//{    6006, 0.037500f},
//{    6160, 0.034091f},
//{    6435, 0.040214f},
//{    6552, 0.037221f},
//{    6864, 0.042735f},
//{    6930, 0.040214f},
//{    7280, 0.042980f},
//{    7920, 0.045872f},
//{    8008, 0.049505f},
//{    8190, 0.049834f},
//{    8580, 0.055762f},
//{    9009, 0.057034f},
//{    9240, 0.054945f},
//{    9360, 0.056818f},
//{   10010, 0.066667f},
//{   10296, 0.065502f},
//{   10920, 0.068182f},
//{   11088, 0.065217f},
//{   11440, 0.075000f},
//{   12012, 0.078534f},
//{   12870, 0.087719f},
//{   13104, 0.081081f},
//{   13860, 0.084270f},
//{   15015, 0.102740f},
//{   16016, 0.106383f},
//{   16380, 0.105634f},
//{   17160, 0.119048f},
//{   18018, 0.123967f},
//{   18480, 0.119048f},
//{   20020, 0.137615f},
//{   20592, 0.140187f},
//{   21840, 0.154639f},
//{   24024, 0.168539f},
//{   25740, 0.180723f},
//{   27720, 0.180723f},
//{   30030, 0.220588f},
//{   32760, 0.241935f},
//{   34320, 0.254237f},
//{   36036, 0.254237f},
//{   40040, 0.288462f},
//{   45045, 0.357143f},
//{   48048, 0.357143f},
//{   51480, 0.384615f},
//{   55440, 0.384615f},
//{   60060, 0.454545f},
//{   65520, 0.517241f},
//{   72072, 0.576923f},
//{   80080, 0.625000f},
//{   90090, 0.833333f},
//{  102960, 0.789474f},
//{  120120, 1.153846f},
//{  144144, 1.153846f},
//{  180180, 1.875000f},
//{  240240, 2.500000f},
//{  360360, 3.750000f},
//{  720720, 7.500000f},
//};


/* Copyright (c) Colorado School of Mines, 1996.*/
/* All rights reserved.                       */

/*********************** self documentation **********************/
/*****************************************************************************
STOEP - Functions to solve a symmetric Toeplitz linear system of equations
	 Rf=g for f

stoepd		solve a symmetric Toeplitz system - doubles
stoepf		solve a symmetric Toeplitz system - floats

******************************************************************************
Function Prototypes:
void stoepd (int n, double r[], double g[], double f[], double a[]);
void stoepf (int n, float r[], float g[], float f[], float a[]);

******************************************************************************
Input:
n		dimension of system
r		array[n] of top row of Toeplitz matrix
g		array[n] of right-hand-side column vector

Output:
f		array[n] of solution (left-hand-side) column vector
a		array[n] of solution to Ra=v (Claerbout, FGDP, p. 57)

******************************************************************************
Notes:
These routines do NOT solve the case when the main diagonal is zero, it
just silently returns.

The left column of the Toeplitz matrix is assumed to be equal to the top
row (as specified in r); i.e., the Toeplitz matrix is assumed symmetric.

******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
/**************** end self doc ********************************/

void stoepd (int n, double r[], double g[], double f[], double a[])
/*****************************************************************************
Solve a symmetric Toeplitz linear system of equations Rf=g for f
(double version)
******************************************************************************
Input:
n		dimension of system
r		array[n] of top row of Toeplitz matrix
g		array[n] of right-hand-side column vector

Output:
f		array[n] of solution (left-hand-side) column vector
a		array[n] of solution to Ra=v (Claerbout, FGDP, p. 57)
******************************************************************************
Notes:
This routine does NOT solve the case when the main diagonal is zero, it
just silently returns.

The left column of the Toeplitz matrix is assumed to be equal to the top
row (as specified in r); i.e., the Toeplitz matrix is assumed symmetric.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
{
	int i,j;
	double v,e,c,w,bot;

	if (r[0] == 0.0) return;

	a[0] = 1.0;
	v = r[0];
	f[0] = g[0]/r[0];

	for (j=1; j<n; j++) {
		
		/* solve Ra=v as in Claerbout, FGDP, p. 57 */
		a[j] = 0.0;
		f[j] = 0.0;
		for (i=0,e=0.0; i<j; i++)
			e += a[i]*r[j-i];
		c = e/v;
		v -= c*e;
		for (i=0; i<=j/2; i++) {
			bot = a[j-i]-c*a[i];
			a[i] -= c*a[j-i];
			a[j-i] = bot;
		}

		/* use a and v above to get f[i], i = 0,1,2,...,j */
		for (i=0,w=0.0; i<j; i++)
			w += f[i]*r[j-i];
		c = (w-g[j])/v;
		for (i=0; i<=j; i++)
			f[i] -= c*a[j-i];
	}
}

void stoepf (int n, float r[], float g[], float f[], float a[])
/*****************************************************************************
Solve a symmetric Toeplitz linear system of equations Rf=g for f
(float version) 
******************************************************************************
Input:
n		dimension of system
r		array[n] of top row of Toeplitz matrix
g		array[n] of right-hand-side column vector

Output:
f		array[n] of solution (left-hand-side) column vector
a		array[n] of solution to Ra=v (Claerbout, FGDP, p. 57)
******************************************************************************
Notes:
This routine does NOT solve the case when the main diagonal is zero, it
just silently returns.

The left column of the Toeplitz matrix is assumed to be equal to the top
row (as specified in r); i.e., the Toeplitz matrix is assumed symmetric.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
{
	int i,j;
	float v,e,c,w,bot;

	if (r[0] == 0.0) return;

	a[0] = 1.0;
	v = r[0];
	f[0] = g[0]/r[0];

//	for (j=1; j<n; j++) {
	for (j=1; j<n-1; j++) {	
		/* solve Ra=v as in Claerbout, FGDP, p. 57 */
		a[j] = 0.0;
		f[j] = 0.0;
		for (i=0,e=0.0; i<j; i++)
			e += a[i]*r[j-i];
		c = e/v;
		v -= c*e;
		for (i=0; i<=j/2; i++) {
			bot = a[j-i]-c*a[i];
			a[i] -= c*a[j-i];
			a[j-i] = bot;
		}

		/* use a and v above to get f[i], i = 0,1,2,...,j */
		for (i=0,w=0.0; i<j; i++)
			w += f[i]*r[j-i];
		c = (w-g[j])/v;
		for (i=0; i<=j; i++)
			f[i] -= c*a[j-i];
	}
}


/* Copyright (c) Colorado School of Mines, 1996.*/
/* All rights reserved.                       */

/*********************** self documentation **********************/
/*****************************************************************************
SINC - Return SINC(x) for as floats or as doubles

fsinc		return float value of sinc(x) for x input as a float
dsinc		return double precision sinc(x) for double precision x

******************************************************************************
Function Prototype:
float fsinc (float x);
double dsinc (double x);

******************************************************************************
Input:
x		value at which to evaluate sinc(x)

Returned: 	sinc(x)

******************************************************************************
Notes:
    sinc(x) = sin(PI*x)/(PI*x) 

******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
/**************** end self doc ********************************/

float fsinc (float x)
/*****************************************************************************
Return sinc(x) = sin(PI*x)/(PI*x) (float version)
******************************************************************************
Input:
x		value at which to evaluate sinc(x)

Returned: 	sinc(x)
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
{
	float pix;

	if (x==0.0) {
		return 1.0;
	} else {
		pix = (float)(PI*x);
		return (float)(sin(pix)/pix);
	}
}

double dsinc (double x)
/*****************************************************************************
Return sinc(x) = sin(PI*x)/(PI*x) (double version)
******************************************************************************
Input:
x		value at which to evaluate sinc(x)

Returned:	sinc(x)
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
{
	double pix;

	if (x==0.0) {
		return 1.0;
	} else {
		pix = PI*x;
		return sin(pix)/pix;
	}
}


/* Copyright (c) Colorado School of Mines, 1996.*/
/* All rights reserved.                       */

/*********************** self documentation **********************/
/*****************************************************************************
INTTABLE8 -  Interpolation of a uniformly-sampled complex function y(x)
		via a table of 8-coefficient interpolators

intt8c	interpolation of a uniformly-sampled complex function y(x)
		via a table of 8-coefficient interpolators
intt8r	interpolation of a uniformly-sampled real function y(x) via a
		table of 8-coefficient interpolators

******************************************************************************
Function Prototype:
void intt8c (int ntable, float table[][8],
	int nxin, float dxin, float fxin, complex yin[], 
	complex yinl, complex yinr, int nxout, float xout[], complex yout[]);
void intt8r (int ntable, float table[][8],
	int nxin, float dxin, float fxin, float yin[], 
	float yinl, float yinr, int nxout, float xout[], float yout[]);

******************************************************************************
Input:
ntable		number of tabulated interpolation operators; ntable>=2
table		array of tabulated 8-point interpolation operators
nxin		number of x values at which y(x) is input
dxin		x sampling interval for input y(x)
fxin		x value of first sample input
yin		array of input y(x) values:  yin[0] = y(fxin), etc.
yinl		value used to extrapolate yin values to left of yin[0]
yinr		value used to extrapolate yin values to right of yin[nxin-1]
nxout		number of x values a which y(x) is output
xout		array of x values at which y(x) is output

Output:
yout		array of output y(x) values:  yout[0] = y(xout[0]), etc.

******************************************************************************
NOTES:
ntable must not be less than 2.

The table of interpolation operators must be as follows:

Let d be the distance, expressed as a fraction of dxin, from a particular
xout value to the sampled location xin just to the left of xout.  Then,
for d = 0.0,

table[0][0:7] = 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0

are the weights applied to the 8 input samples nearest xout.
Likewise, for d = 1.0,

table[ntable-1][0:7] = 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0

are the weights applied to the 8 input samples nearest xout.  In general,
for d = (float)itable/(float)(ntable-1), table[itable][0:7] are the
weights applied to the 8 input samples nearest xout.  If the actual sample
distance d does not exactly equal one of the values for which interpolators
are tabulated, then the interpolator corresponding to the nearest value of
d is used.

Because extrapolation of the input function y(x) is defined by the left
and right values yinl and yinr, the xout values are not restricted to lie
within the range of sample locations defined by nxin, dxin, and fxin.

******************************************************************************
AUTHOR:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
/**************** end self doc ********************************/

void intt8c (int ntable, float table[][8],
	int nxin, float dxin, float fxin, complex yin[],
	complex yinl, complex yinr, int nxout, float xout[], complex yout[])
{
	int ioutb,ixout,ixoutn,kyin,ktable,itable,nxinm8;
	float xoutb,xoutf,xouts,xoutn,fntablem1,frac,
		yinlr,yinli,yinrr,yinri,yinir,yinii,sumr,sumi,
		*table00,*pyin,*ptable;
	complex *yin0;

	/* compute constants */
	ioutb = -3-8;
	xoutf = fxin;
	xouts = (float)(1.0/dxin);
	xoutb = (float)(8.0-xoutf*xouts);
	fntablem1 = (float)(ntable-1);
	nxinm8 = nxin-8;
	yin0 = &yin[0];
	table00 = &table[0][0];
	yinlr = (float)yinl.r;  yinli = (float)yinl.i;
	yinrr = (float)yinr.r;  yinri = (float)yinr.i;

	/* loop over output samples */
	for (ixout=0; ixout<nxout; ixout++) {

		/* determine pointers into table and yin */
		xoutn = xoutb+xout[ixout]*xouts;
		ixoutn = (int)xoutn;
		kyin = ioutb+ixoutn;
		pyin = (float*)(yin0+kyin);
		frac = xoutn-(float)ixoutn;
		ktable = (int)(frac>=0.0?frac*fntablem1+0.5:(frac+1.0)*fntablem1-0.5);
		ptable = table00+ktable*8;
		
		/* if totally within input array, use fast method */
		if (kyin>=0 && kyin<=nxinm8) {
			yout[ixout].r = 
				pyin[0]*ptable[0]+
				pyin[2]*ptable[1]+
				pyin[4]*ptable[2]+
				pyin[6]*ptable[3]+
				pyin[8]*ptable[4]+
				pyin[10]*ptable[5]+
				pyin[12]*ptable[6]+
				pyin[14]*ptable[7];
			yout[ixout].i = 
				pyin[1]*ptable[0]+
				pyin[3]*ptable[1]+
				pyin[5]*ptable[2]+
				pyin[7]*ptable[3]+
				pyin[9]*ptable[4]+
				pyin[11]*ptable[5]+
				pyin[13]*ptable[6]+
				pyin[15]*ptable[7];
		
		/* else handle end effects with care */
		} else {

			sumr = sumi = 0.0;
			for (itable=0; itable<8; itable++,kyin++) {
				if (kyin<0) {
					yinir = yinlr;
					yinii = yinli;
				} else if (kyin>=nxin) {
					yinir = yinrr;
					yinii = yinri;
				} else {
					yinir = (float)yin[kyin].r;
					yinii = (float)yin[kyin].i;
				}
				sumr += yinir*(*ptable);
				sumi += yinii*(*ptable++);
			}
			yout[ixout].r = sumr;
			yout[ixout].i = sumi;
		}
	}
}


void intt8r (int ntable, float table[][8],
	int nxin, float dxin, float fxin, float yin[], float yinl, float yinr,
	int nxout, float xout[], float yout[])
{
	int ioutb,nxinm8,ixout,ixoutn,kyin,ktable,itable;
	float xoutb,xoutf,xouts,xoutn,frac,fntablem1,yini,sum,
		*yin0,*table00,*pyin,*ptable;

	/* compute constants */
	ioutb = -3-8;
	xoutf = fxin;
	xouts = (float)(1.0/dxin);
	xoutb = (float)(8.0-xoutf*xouts);
	fntablem1 = (float)(ntable-1);
	nxinm8 = nxin-8;
	yin0 = &yin[0];
	table00 = &table[0][0];

	/* loop over output samples */
	for (ixout=0; ixout<nxout; ixout++) {

		if (ixout == 270) {
			int itierrt = 1;
		}

		/* determine pointers into table and yin */
		xoutn = xoutb+xout[ixout]*xouts;
		ixoutn = (int)xoutn;
		kyin = ioutb+ixoutn;
		pyin = yin0+kyin;
		frac = xoutn-(float)ixoutn;
		ktable = (int)(frac>=0.0?frac*fntablem1+0.5:(frac+1.0)*fntablem1-0.5);
		ptable = table00+ktable*8;
		
		/* if totally within input array, use fast method */
		if (kyin>=0 && kyin<=nxinm8) {
			yout[ixout] = 
				pyin[0]*ptable[0]+
				pyin[1]*ptable[1]+
				pyin[2]*ptable[2]+
				pyin[3]*ptable[3]+
				pyin[4]*ptable[4]+
				pyin[5]*ptable[5]+
				pyin[6]*ptable[6]+
				pyin[7]*ptable[7];
		
		/* else handle end effects with care */
		} else {
	
			/* sum over 8 tabulated coefficients */
			for (itable=0,sum=0.0; itable<8; itable++,kyin++) {
				if (kyin<0)
					yini = yinl;
				else if (kyin>=nxin)
					yini = yinr;
				else
					yini = yin[kyin];
				sum += yini*(*ptable++);
			}
			yout[ixout] = sum;
		}
	}
}


/* Copyright (c) Colorado School of Mines, 1996.*/
/* All rights reserved.                       */

/*********************** self documentation **********************/
/*****************************************************************************
MKSINC - Compute least-squares optimal sinc interpolation coefficients.

mksinc		Compute least-squares optimal sinc interpolation coefficients.

******************************************************************************
Function Prototype:
void mksinc (float d, int lsinc, float sinc[]);

******************************************************************************
Input:
d		fractional distance to interpolation point; 0.0<=d<=1.0
lsinc		length of sinc approximation; lsinc%2==0 and lsinc<=20

Output:
sinc		array[lsinc] containing interpolation coefficients

******************************************************************************
Notes:
The coefficients are a least-squares-best approximation to the ideal
sinc function for frequencies from zero up to a computed maximum
frequency.  For a given interpolator length, lsinc, mksinc computes
the maximum frequency, fmax (expressed as a fraction of the nyquist
frequency), using the following empirically derived relation (from
a Western Geophysical Technical Memorandum by Ken Larner):

	fmax = min(0.066+0.265*log(lsinc),1.0)

Note that fmax increases as lsinc increases, up to a maximum of 1.0.
Use the coefficients to interpolate a uniformly-sampled function y(i) 
as follows:

            lsinc-1
    y(i+d) =  sum  sinc[j]*y(i+j+1-lsinc/2)
              j=0

Interpolation error is greatest for d=0.5, but for frequencies less
than fmax, the error should be less than 1.0 percent.

******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
/**************** end self doc ********************************/

void mksinc (float d, int lsinc, float sinc[])
/*****************************************************************************
Compute least-squares optimal sinc interpolation coefficients.
******************************************************************************
Input:
d		fractional distance to interpolation point; 0.0<=d<=1.0
lsinc		length of sinc approximation; lsinc%2==0 and lsinc<=20

Output:
sinc		array[lsinc] containing interpolation coefficients
******************************************************************************
Notes:
The coefficients are a least-squares-best approximation to the ideal
sinc function for frequencies from zero up to a computed maximum
frequency.  For a given interpolator length, lsinc, mksinc computes
the maximum frequency, fmax (expressed as a fraction of the nyquist
frequency), using the following empirically derived relation (from
a Western Geophysical Technical Memorandum by Ken Larner):

	fmax = min(0.066+0.265*log(lsinc),1.0)

Note that fmax increases as lsinc increases, up to a maximum of 1.0.
Use the coefficients to interpolate a uniformly-sampled function y(i) 
as follows:

            lsinc-1
    y(i+d) =  sum  sinc[j]*y(i+j+1-lsinc/2)
              j=0

Interpolation error is greatest for d=0.5, but for frequencies less
than fmax, the error should be less than 1.0 percent.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
{
	int j;
	double s[20],a[20],c[20],work[20],fmax;

	/* compute auto-correlation and cross-correlation arrays */
	fmax = 0.066+0.265*log((double)lsinc);
	fmax = (fmax<1.0)?fmax:1.0;
	for (j=0; j<lsinc; j++) {
		a[j] = dsinc(fmax*j);
		c[j] = dsinc(fmax*(lsinc/2-j-1+d));
	}

	/* solve symmetric Toeplitz system for the sinc approximation */
	stoepd(lsinc,a,c,s,work);
	for (j=0; j<lsinc; j++)
		sinc[j] = (float)s[j];
}



/* Copyright (c) Colorado School of Mines, 1996.*/
/* All rights reserved.                       */

/*********************** self documentation **********************/
/*****************************************************************************
INTSINC8 - Functions to interpolate uniformly-sampled data via 8-coeff. sinc
		approximations:

ints8c	interpolation of a uniformly-sampled complex function y(x) via an
	 8-coefficient sinc approximation.
ints8r	Interpolation of a uniformly-sampled real function y(x) via a
		table of 8-coefficient sinc approximations

******************************************************************************
Function Prototypes:
void ints8c (int nxin, float dxin, float fxin, complex yin[], 
	complex yinl, complex yinr, int nxout, float xout[], complex yout[]);
void ints8r (int nxin, float dxin, float fxin, float yin[], 
	float yinl, float yinr, int nxout, float xout[], float yout[]);

******************************************************************************
Input:
nxin		number of x values at which y(x) is input
dxin		x sampling interval for input y(x)
fxin		x value of first sample input
yin		array[nxin] of input y(x) values:  yin[0] = y(fxin), etc.
yinl		value used to extrapolate yin values to left of yin[0]
yinr		value used to extrapolate yin values to right of yin[nxin-1]
nxout		number of x values a which y(x) is output
xout		array[nxout] of x values at which y(x) is output

Output:
yout		array[nxout] of output y(x):  yout[0] = y(xout[0]), etc.

******************************************************************************
Notes:
Because extrapolation of the input function y(x) is defined by the
left and right values yinl and yinr, the xout values are not restricted
to lie within the range of sample locations defined by nxin, dxin, and
fxin.

The maximum error for frequiencies less than 0.6 nyquist is less than
one percent.

******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
/**************** end self doc ********************************/

/* these are used by both ints8c and ints8r */
#define LTABLE 8
#define NTABLE 513

void ints8c (int nxin, float dxin, float fxin, complex yin[], 
	complex yinl, complex yinr, int nxout, float xout[], complex yout[])
/*****************************************************************************
Interpolation of a uniformly-sampled complex function y(x) via a
table of 8-coefficient sinc approximations; maximum error for frequiencies
less than 0.6 nyquist is less than one percent.
******************************************************************************
Input:
nxin		number of x values at which y(x) is input
dxin		x sampling interval for input y(x)
fxin		x value of first sample input
yin		array[nxin] of input y(x) values:  yin[0] = y(fxin), etc.
yinl		value used to extrapolate yin values to left of yin[0]
yinr		value used to extrapolate yin values to right of yin[nxin-1]
nxout		number of x values a which y(x) is output
xout		array[nxout] of x values at which y(x) is output

Output:
yout		array[nxout] of output y(x):  yout[0] = y(xout[0]), etc.
******************************************************************************
Notes:
Because extrapolation of the input function y(x) is defined by the
left and right values yinl and yinr, the xout values are not restricted
to lie within the range of sample locations defined by nxin, dxin, and
fxin.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
{
	static float table[NTABLE][LTABLE];
	static int tabled=0;
	int jtable;
	float frac;

	/* tabulate sinc interpolation coefficients if not already tabulated */
	if (!tabled) {
		for (jtable=1; jtable<NTABLE-1; jtable++) {
			frac = (float)jtable/(float)(NTABLE-1);
			mksinc(frac,LTABLE,&table[jtable][0]);
		}
		for (jtable=0; jtable<LTABLE; jtable++) {
			table[0][jtable] = 0.0;
			table[NTABLE-1][jtable] = 0.0;
		}
		table[0][LTABLE/2-1] = 1.0;
		table[NTABLE-1][LTABLE/2] = 1.0;
		tabled = 1;
	}

	/* interpolate using tabulated coefficients */
	intt8c(NTABLE,table,nxin,dxin,fxin,yin,yinl,yinr,nxout,xout,yout);
}


void ints8r (int nxin, float dxin, float fxin, float yin[], 
	float yinl, float yinr, int nxout, float xout[], float yout[])
/*****************************************************************************
				ints8r((int)numsamp, // nxin
					(float)(1./oneodt),   // dxin
					getTraceDelay(&tr),   // fxin
					&trArr[nxStore].data[0], // yin[x=nxin], with yin[0] = y(fxin)
					0.f,  // yinl    value used to extrapolate yin values to left of yin[0]
					0.f,  // yinr	 value used to extrapolate yin values to right of yin[nxin-1]
					nz,   // nxout
					(float *)&tTrcDepth.data[0],  // nxout
					(float *)&tr.dat.data[0]);    // xout

Interpolation of a uniformly-sampled real function y(x) via a
table of 8-coefficient sinc approximations; maximum error for frequiencies
less than 0.6 nyquist is less than one percent.
******************************************************************************
Input:
nxin		number of x values at which y(x) is input
dxin		x sampling interval for input y(x)
fxin		x value of first sample input
yin[]		array[nxin] of input y(x) values:  yin[0] = y(fxin), etc.
yinl		value used to extrapolate yin values to left of yin[0]
yinr		value used to extrapolate yin values to right of yin[nxin-1]
nxout		number of x values at which y(x) is output
xout[]		array[nxout] of x values at which y(x) is output

Output:
yout[]		array[nxout] of output y(x):  yout[0] = y(xout[0]), etc.
******************************************************************************
Notes:
Because extrapolation of the input function y(x) is defined by the
left and right values yinl and yinr, the xout values are not restricted
to lie within the range of sample locations defined by nxin, dxin, and
fxin.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/

{
	static float table[NTABLE][LTABLE];
	static int tabled=0;
	int jtable;
	float frac;

	/* tabulate sinc interpolation coefficients if not already tabulated */
	if (!tabled) {
		for (jtable=1; jtable<NTABLE-1; jtable++) {
			frac = (float)jtable/(float)(NTABLE-1);
			mksinc(frac,LTABLE,&table[jtable][0]);
		}
		for (jtable=0; jtable<LTABLE; jtable++) {
			table[0][jtable] = 0.0;
			table[NTABLE-1][jtable] = 0.0;
		}
		table[0][LTABLE/2-1] = 1.0;
		table[NTABLE-1][LTABLE/2] = 1.0;
		tabled = 1;
	}

	/* interpolate using tabulated coefficients */
	intt8r(NTABLE,table,nxin,dxin,fxin,yin,yinl,yinr,nxout,xout,yout);
}

/* Copyright (c) Colorado School of Mines, 1996.*/
/* All rights reserved.                       */

/*********************** self documentation **********************/
/*****************************************************************************
RESSINC8 - Functions to resample uniformly-sampled data  via 8-coefficient sinc
		 approximations:

ress8c	resample a uniformly-sampled complex function via 8-coeff. sinc approx.
ress8r	resample a uniformly-sampled real function via 8-coeff. sinc approx.

******************************************************************************
Function Prototypes:
void ress8r (int nxin, float dxin, float fxin, float yin[], 
	float yinl, float yinr, 
	int nxout, float dxout, float fxout, float yout[]);
void ress8c (int nxin, float dxin, float fxin, complex yin[], 
	complex yinl, complex yinr, 
	int nxout, float dxout, float fxout, complex yout[]);

******************************************************************************
Input:
nxin		number of x values at which y(x) is input
dxin		x sampling interval for input y(x)
fxin		x value of first sample input
yin		array[nxin] of input y(x) values:  yin[0] = y(fxin), etc.
yinl		value used to extrapolate yin values to left of yin[0]
yinr		value used to extrapolate yin values to right of yin[nxin-1]
nxout		number of x values at which y(x) is output
dxout		x sampling interval for output y(x)
fxout		x value of first sample output

Output:
yout		array[nxout] of output y(x) values:  yout[0] = y(fxout), etc.

******************************************************************************
Notes:
Because extrapolation of the input function y(x) is defined by the
left and right values yinl and yinr, the output x values defined
by nxout, dxout, and fxout are not restricted to lie within the range 
of input x values defined by nxin, dxin, and fxin.
 
The maximum error for frequiencies less than 0.6 nyquist is 
less than one percent.

******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/06/90
*****************************************************************************/
/**************** end self doc ********************************/
void ress8c (int nxin, float dxin, float fxin, complex yin[], 
	complex yinl, complex yinr, 
	int nxout, float dxout, float fxout, complex yout[])
/*****************************************************************************
Resample a uniformly-sampled complex function y(x) via 8-coefficient sinc
approximations; maximum error for frequiencies less than 0.6 nyquist is 
less than one percent.
******************************************************************************
Input:
nxin		number of x values at which y(x) is input
dxin		x sampling interval for input y(x)
fxin		x value of first sample input
yin		array[nxin] of input y(x) values:  yin[0] = y(fxin), etc.
yinl		value used to extrapolate yin values to left of yin[0]
yinr		value used to extrapolate yin values to right of yin[nxin-1]
nxout		number of x values at which y(x) is output
dxout		x sampling interval for output y(x)
fxout		x value of first sample output

Output:
yout		array[nxout] of output y(x) values:  yout[0] = y(fxout), etc.
******************************************************************************
Notes:
Because extrapolation of the input function y(x) is defined by the
left and right values yinl and yinr, the output x values defined
by nxout, dxout, and fxout are not restricted to lie within the range 
of input x values defined by nxin, dxin, and fxin.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/06/90
*****************************************************************************/
{
	int ixin,ixout;
	float odxin,tiny,xout,xini,error,*xoutt;
	
	/* initially, assume output samples fall on input samples */
	odxin = 1.0f/dxin;
	tiny = 0.01f;
	for (ixout=0,xout=fxout; ixout<nxout; ++ixout,xout+=dxout) {
		xini = (xout-fxin)*odxin;
		ixin = NINT(xini);
		error = xini-ixin;
		if (ABS(error)>tiny) break;
		if (ixin<0)
			yout[ixout] = yinl;
		else if (ixin>=nxin)
			yout[ixout] = yinr;
		else
			yout[ixout] = yin[ixin];
	}
	
	/* if all output samples fell on input samples, then done */
	if (ixout==nxout) return;
	
	/* otherwise, must interpolate */
	xoutt = (float*)ealloc1float(nxout);
	for (ixout=0; ixout<nxout; ++ixout)
		xoutt[ixout] = fxout+ixout*dxout;
	ints8c(nxin,dxin,fxin,yin,yinl,yinr,nxout,xoutt,yout);
	free1float(xoutt);
}

void ress8r (int nxin, float dxin, float fxin, float yin[], 
	float yinl, float yinr, 
	int nxout, float dxout, float fxout, float yout[])
/*****************************************************************************
Resample a uniformly-sampled real function y(x) via 8-coefficient sinc
approximations; maximum error for frequiencies less than 0.6 nyquist is 
less than one percent.
******************************************************************************
Input:
nxin		number of x values at which y(x) is input
dxin		x sampling interval for input y(x)
fxin		x value of first sample input
yin		array[nin] of input y(x) values:  yin[0] = y(fxin), etc.
yinl		value used to extrapolate yin values to left of yin[0]
yinr		value used to extrapolate yin values to right of yin[nxin-1]
nxout		number of x values at which y(x) is output
dxout		x sampling interval for output y(x)
fxout		x value of first sample output

Output:
yout		array[nxout] of output y(x) values:  yout[0] = y(fxout), etc.
******************************************************************************
Notes:
Because extrapolation of the input function y(x) is defined by the
left and right values yinl and yinr, the output x values defined
by nxout, dxout, and fxout are not restricted to lie within the range 
of input x values defined by nxin, dxin, and fxin.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/06/90
*****************************************************************************/
{
	int ixin,ixout;
	float odxin,tiny,xout,xini,error,*xoutt;
	
	/* initially, assume output samples fall on input samples */
	odxin = 1.0f/dxin;
	tiny = 0.01f;
	for (ixout=0,xout=fxout; ixout<nxout; ++ixout,xout+=dxout) {
		xini = (xout-fxin)*odxin;
		ixin = NINT(xini);
		error = xini-ixin;
		if (ABS(error)>tiny) break;
		if (ixin<0)
			yout[ixout] = yinl;
		else if (ixin>=nxin)
			yout[ixout] = yinr;
		else
			yout[ixout] = yin[ixin];
	}
	
	/* if all output samples fell on input samples, then done */
	if (ixout==nxout) return;
	
	/* otherwise, must interpolate */
	xoutt = (float*)ealloc1float(nxout);
	for (ixout=0; ixout<nxout; ++ixout)
		xoutt[ixout] = fxout+ixout*dxout;
	ints8r(nxin,dxin,fxin,yin,yinl,yinr,nxout,xoutt,yout);
	free1float(xoutt);
}


/* Copyright (c) Colorado School of Mines, 1996.*/
/* All rights reserved.                       */

/*********************** self documentation **********************/
/*****************************************************************************
XINDEX - determine index of x with respect to an array of x values

xindex		determine index of x with respect to an array of x values

******************************************************************************
Input:
nx		number of x values in array ax
ax		array[nx] of monotonically increasing or decreasing x values
x		the value for which index is to be determined
index		index determined previously (used to begin search)

Output:
index		for monotonically increasing ax values, the largest index
		for which ax[index]<=x, except index=0 if ax[0]>x;
		for monotonically decreasing ax values, the largest index
		for which ax[index]>=x, except index=0 if ax[0]<x

******************************************************************************
Notes:
This function is designed to be particularly efficient when called
repeatedly for slightly changing x values; in such cases, the index 
returned from one call should be used in the next.

******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 12/25/89
*****************************************************************************/
/**************** end self doc ********************************/

void
xindex (int nx, float ax[], float x, int *index)
/*****************************************************************************
determine index of x with respect to an array of x values
******************************************************************************
Input:
nx		number of x values in array ax
ax		array[nx] of monotonically increasing or decreasing x values
x		the value for which index is to be determined
index		index determined previously (used to begin search)

Output:
index		for monotonically increasing ax values, the largest index
		for which ax[index]<=x, except index=0 if ax[0]>x;
		for monotonically decreasing ax values, the largest index
		for which ax[index]>=x, except index=0 if ax[0]<x
******************************************************************************
Notes:
This function is designed to be particularly efficient when called
repeatedly for slightly changing x values; in such cases, the index 
returned from one call should be used in the next.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 12/25/89
*****************************************************************************/
{
	int lower,upper,middle,step;

	/* initialize lower and upper indices and step */
	lower = *index;
	if (lower<0) lower = 0;
	if (lower>=nx) lower = nx-1;
	upper = lower+1;
	step = 1;

	/* if x values increasing */
	if (ax[nx-1]>ax[0]) {

		/* find indices such that ax[lower] <= x < ax[upper] */
		while (lower>0 && ax[lower]>x) {
			upper = lower;
			lower -= step;
			step += step;
		}
		if (lower<0) lower = 0;
		while (upper<nx && ax[upper]<=x) {
			lower = upper;
			upper += step;
			step += step;
		}
		if (upper>nx) upper = nx;

		/* find index via bisection */
		while ((middle=(lower+upper)>>1)!=lower) {
			if (x>=ax[middle])
				lower = middle;
			else
				upper = middle;
		}

	/* else, if not increasing */
	} else {

		/* find indices such that ax[lower] >= x > ax[upper] */
		while (lower>0 && ax[lower]<x) {
			upper = lower;
			lower -= step;
			step += step;
		}
		if (lower<0) lower = 0;
		while (upper<nx && ax[upper]>=x) {
			lower = upper;
			upper += step;
			step += step;
		}
		if (upper>nx) upper = nx;

		/* find index via bisection */
		while ((middle=(lower+upper)>>1)!=lower) {
			if (x<=ax[middle])
				lower = middle;
			else
				upper = middle;
		}
	}

	/* return lower index */
	*index = lower;
}

void mig1k (float k, float fmax, float speed, int nt, float dt, 
	float *v, complex *p, complex *q)
/*****************************************************************************
migration in t-k domain for one wavenumber
******************************************************************************
Input:
k		wavenumber
fmax		maximum frequency
speed		speed parameter - >>1.0 for lots of dispersion
nt		number of time samples
dt		time sampling interval
v		array[nt] containing interval velocities v(t)
p		array[nt] containing input p(t;k)

Output:
q		array[nt] containing migrated q(t;k)
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 11/05/90
*****************************************************************************/
{
	int nfac=5,ns,is,it,i1,i2,i1stop;
	float fac=0.75,ds,g00r,g00i,g01r,g01i,g10r,g10i,g11r,g11i,
		temp,vmin,vmax,ct,
		*vs,*cs,*cp,*grp,*gip;
	// complex czero=cmplx(0.0,0.0),*gs;
	complex czero={0.0,0.0}, *gs;
	
	/* determine time sampling to avoid excessive grid dispersion */
	for (it=1,vmin=vmax=v[0]; it<nt; ++it) {
		if (v[it]<vmin) vmin = v[it];
		if (v[it]>vmax) vmax = v[it];
	}
	if (k!=0.0) {
		ds = (float)( fac*MAX(0.4f*PI/ABS(vmax*k),0.1f*vmin/(vmax*fmax)) );
		ds *= speed;
		ds = MIN(ds,dt);
	} else {
		ds = dt;
	}
	ns = (int)(1+(nt-1)*dt/ds);
	ns = MIN(ns,nfac*nt);
	ds = (nt-1)*dt/(ns-1);
	// _ftprintf_s(stderr,"ns=%d\n",ns);
	
	/* allocate workspace */
	vs = ealloc1float(ns);
	cs = ealloc1float(ns*2);
	gs = (complex *) ealloc1complex(ns);
	
	/* resample v(t) and p(t) */
	ress8r(nt,dt,0.0,v,v[0],v[nt-1],ns,ds,0.0,vs);
	ress8c(nt,dt,0.0,p,czero,czero,ns,ds,0.0,gs);

        /* compute finite-difference coefficients */
	for (is=0; is<ns; is++) {
		temp = (float)(0.125*vs[is]*k*ds);
		temp = temp*temp;
		temp = (float)((1.0-temp)/(1.0+temp));
		cs[2*is] = cs[2*is+1] = temp;
	}

	/* loop over t2 = (tau-t)/sqrt(2) */
	for (i2=2-ns; i2<ns; i2++) {

		/* determine t1 stop index */
		i1stop = (i2<=0)?1-i2:i2;

		/* initialize finite-difference star and coefficient */
		g00r = g00i = g01r = g01i = 0.0f;
		grp = (float*)(&gs[ns-1]);
		gip = grp+1;
		cp = &cs[i2+ns-2];
		ct = *cp--;

		/* loop over t1 = (tau+t)/sqrt(2) */
		for (i1=ns-1; i1>=i1stop; i1--) {

			/* update real part */
			g10r = g00r;
			g11r = g01r;
			g00r = *grp;
			*grp = g01r = ct*(g11r+g00r)-g10r;

			/* update imaginary part */
			g10i = g00i;
			g11i = g01i;
			g00i = *gip;
			*gip = g01i = ct*(g11i+g00i)-g10i;

			/* update pointers and finite-difference coefficient */
			grp -= 2; gip -= 2;
			ct = *cp--;
		}
	}
	
	/* resample q(t) */
	ress8c(ns,ds,0.0,gs,czero,czero,nt,dt,0.0,q);
	
	/* free workspace */
	free1float(vs);
	free1float(cs);
	free1complex(gs);
}

void mig1kPS (TATable *table, float k, float ffil[4], int ntflag,
	int nt, float dt, complex *cp,
	int ntau, float dtau, complex *cq)
/*****************************************************************************
phase shift migration for one wavenumber k via tabulated times and amps
******************************************************************************
Input:
table	 	pointer to the TATable
k		wavenumber
ffil[4]		four frequencies (in Hz) defining trapezoidal filter
ntflag		=1 for normal, 2 for turned, 3 for normal+turned
nt		number of time samples
dt		time sampling interval
cp		array[nt] of data to be migrated
ntau		number of vertical two-way times tau
dtau		vertical time sampling interval

Output:
cq		array[ntau] of migrated data
******************************************************************************/
{
	int ntfft,nw,iwnyq,iwmin,iwmax,iw,itau,it,np,
		ip1,ip2,iwfil0,iwfil1,iwfil2,iwfil3,
		itaumax,itaumin,itab;
	float dw,wnyq,wmin,wmax,w,ampf,p1,p2,a1,a2,wa1,wa2,
		taumaxi,taumini,kow,phst,pwsr,pwsi,pwdr,pwdi,
		ampi,phsi,phsn,
		*p,*taumax,*taumin,*tturn,**t,**a;
	complex *pt,*pw,*qn,*qt;
	static int tabbed=0;
	float fntab,*ctab,*stab;
	static float pio2=(float)(PI/2.0),opi2=(float)(1.0/(PI*2.0));

	/* if not already built, build cosine/sine tables */
	if (!tabbed) {
		int itab,ntab=1025;
		float angle,dangle=(float)(2.0*PI/(ntab-1));
		ctab = alloc1float(ntab);
		stab = alloc1float(ntab);
		for (itab=0,angle=0.0; itab<ntab; ++itab,angle+=dangle) {
			ctab[itab] = (float)cos(angle);
			stab[itab] = (float)sin(angle);
		}
		tabbed = 1;
		fntab = (float)ntab;
	}
	
	/* parts of time/amplitude table */
	np = table->np;
	p = table->p;
	taumax = table->taumax;
	taumin = table->taumin;
	tturn = table->tturn;
	t = table->t;
	a = table->a;
	
	/* frequency sampling */
	ntfft = npfao(2*nt,4*nt);
	nw = ntfft;
	dw = (float)(2.0*PI/(ntfft*dt));
	
	/* allocate workspace */
	pt = pw = alloc1complex(ntfft);
	qn = alloc1complex(ntau);
	qt = alloc1complex(ntau);
	
	/* nyquist frequency */
	wnyq = PI/dt;
	
	/* index of smallest frequency >= nyquist */
	iwnyq = (ntfft%2)?ntfft/2:ntfft/2+1;
	
	/* determine frequency filter sample indices */
	iwfil0 = MAX(0,MIN(iwnyq,NINT(2.0*PI*ffil[0]/dw)));
	iwfil1 = MAX(0,MIN(iwnyq,NINT(2.0*PI*ffil[1]/dw)));
	iwfil2 = MAX(0,MIN(iwnyq,NINT(2.0*PI*ffil[2]/dw)));
	iwfil3 = MAX(0,MIN(iwnyq,NINT(2.0*PI*ffil[3]/dw)));
	
	/* pad p(t) with zeros */
	for (it=0; it<nt; ++it)
		pt[it] = cp[it];
	for (it=nt; it<ntfft; ++it)
		pt[it].r = pt[it].i = 0.0;
	
	/* Fourier transform p(t) to p(w) (include scaling) */
	pfacc(1,ntfft,pt);
	for (iw=0; iw<nw; ++iw) {
		pw[iw].r *= 1.0f/ntfft;
		pw[iw].i *= 1.0f/ntfft;
	}
	
	/* initially zero normal and turned images */
	for (itau=0; itau<ntau; ++itau)
		qn[itau].r = qn[itau].i = qt[itau].r = qt[itau].i = 0.0;
	
	/* minimum and maximum frequency indices */
	wmin = ABS(k)/p[np-1];
	iwmin = MAX(MAX(1,iwfil0),(int)(wmin/dw));
	if (p[0]<=ABS(k)/wnyq)
		wmax = wnyq;
	else
		wmax = ABS(k)/p[0];
	iwmax = MIN(MIN(iwnyq-1,iwfil3),(int)(wmax/dw));
	
	/* loop over frequencies */
	for (iw=iwmin,w=iwmin*dw; iw<=iwmax; ++iw,w+=dw) {
			
		/* if slope not within range of table, continue */
		kow = ABS(k)/w;
		if (kow>p[np-1]) continue;
		
		/* amplitude of frequency filter */
		if (iwfil0<=iw && iw<iwfil1)
			ampf = (float)(iw-iwfil0)/(float)(iwfil1-iwfil0);
		else if (iwfil2<iw && iw<=iwfil3)
			ampf = (float)(iwfil3-iw)/(float)(iwfil3-iwfil2);
		else
			ampf = 1.0;
		
		/* weights for interpolation in table */
		xindex(np,p,kow,&ip1);
		ip1 = MIN(ip1,np-2);
		ip2 = ip1+1;
		p1 = p[ip1];
		p2 = p[ip2];
		a1 = (p2*p2-kow*kow)/(p2*p2-p1*p1);
		a2 = (kow*kow-p1*p1)/(p2*p2-p1*p1);
		wa1 = w*a1;
		wa2 = w*a2;
		
		/* maximum tau index for normal image */
		taumaxi = a1*taumax[ip1]+a2*taumax[ip2];
		itaumax = MIN(ntau-1,NINT(taumaxi/dtau));
		
		/* minimum tau index for turned image */
		taumini = a1*taumin[ip1]+a2*taumin[ip2];
		itaumin = MAX(0,NINT(taumini/dtau));
		
		/* phase at turning point */
		phst = 2.0f*(wa1*tturn[ip1]+wa2*tturn[ip2]);
		
		/* filtered sum and differences for positive and negative w */
		pwsr = (float)(ampf*(pw[iw].r+pw[nw-iw].r));
		pwsi = (float)(ampf*(pw[iw].i+pw[nw-iw].i));
		pwdr = (float)(ampf*(pw[iw].r-pw[nw-iw].r));
		pwdi = (float)(ampf*(pw[iw].i-pw[nw-iw].i));
		
		/* accumulate normal image */
		if (ntflag&1) {
		for (itau=0; itau<itaumax; ++itau) {
			ampi = a1*a[ip1][itau]+a2*a[ip2][itau];
			phsi = wa1*t[ip1][itau]+wa2*t[ip2][itau];
			phsn = phsi*opi2;
			itab = (int)(fntab*(phsn-(int)phsn));
			qn[itau].r = qn[itau].r +
				ampi*(pwsr*ctab[itab]+pwdi*stab[itab]);
			qn[itau].i = qn[itau].i +
				ampi*(pwsi*ctab[itab]-pwdr*stab[itab]);
		}
		}
		
		/* accumulate turned image */
		if (ntflag&2) {
		for (itau=itaumin+1; itau<itaumax; ++itau) {
			ampi = a1*a[ip1][itau]+a2*a[ip2][itau];
			phsi = phst-(wa1*t[ip1][itau]+wa2*t[ip2][itau]);
			phsn = phsi*opi2;
			itab = (int)(fntab*(phsn-(int)phsn));
			qt[itau].r = qt[itau].r +
				ampi*(pwsr*stab[itab]-pwdi*ctab[itab]);
			qt[itau].i = qt[itau].i +
				ampi*(pwsi*stab[itab]+pwdr*ctab[itab]);
		}
		}
	}
	
	/* sum normal and turned images */
	for (itau=0; itau<ntau; ++itau) {
		cq[itau].r = qn[itau].r+qt[itau].r;
		cq[itau].i = qn[itau].i+qt[itau].i;
	}
	
	/* free workspace */
	free1complex(pt);
	free1complex(qn);
	free1complex(qt);
}	

TATable *tableta (int np, int ntau, float dtau, float vtau[],
	int nt, float dt)
/*****************************************************************************
tabulate time shifts and amplitudes for use in phase-shift migration
******************************************************************************
Input:
np		number of slopes p
ntau		number of vertical two-way times tau
dtau		vertical time sampling interval
vtau		array[ntau] of velocities as a function of tau
nt		number of time samples
dt		time sampling interval
verbose	 non-zero to print diagnostic info on stderr

Returned: 	pointer to the TATable
******************************************************************************/
{
	int jp,ktau,itau,jtau,ltau,it;
	float pj,taui,taul,tauj,ti,tl,ai,al,
		vel,dvel,angle,cosa,frac,
		*p,*taumax,*taumin,*tturn,**t,**a;
	TATable *table;
	
	/* allocate table */
	table = (TATable *) alloc1(1,sizeof(TATable));
	table->np = np;
	table->p = p = alloc1float(np);
	table->taumax = taumax = alloc1float(np);
	table->taumin = taumin =alloc1float(np);
	table->tturn = tturn = alloc1float(np);
	table->t = t = ealloc2float(ntau,np);
	table->a = a = ealloc2float(ntau,np);
		
	/* loop over slopes p */
	for (jp=0; jp<np; ++jp) {
	
		/* slope p */
		p[jp] = pj = (float)(2.0f/vtau[0]*sqrt((float)(jp)/(float)(np-1)));
			
		/* time and amplitude at tau = 0 */
		t[jp][0] = 0.0;
		a[jp][0] = 1.0;
		
		/* tau index */
		ktau = 0;
		
		/* initial ray tracing parameters */
		taui = 0.0;
		ti = 0.0;
		ai = 1.0;
		vel = 0.5f*vtau[0];
		dvel = 0.5f*(vtau[1]-vtau[0]);
		angle = (float)(asin(MIN(1.0f,pj*vel)));
		
		/* loop over times t */
		for (it=1; it<nt; ++it) {
			
			/* remember last tau, t, and a */
			taul = taui;
			tl = ti;
			al = ai;
			
			/* update cosine of propagation angle */
			cosa = (float)cos(angle);
			
			/* update tau, t, and a */
			taui += dt*cosa;
			ti += dt*cosa*cosa;
			ai = 1.0;
			
			/* if ray emerges at surface, break */
			if (taui<0.0) break;
			
			/* update turning time and max,min tau */
			if (taui>=taul) {
				tturn[jp] = ti;
				taumax[jp] = taui;
				taumin[jp] = taui;
			} else {
				taumin[jp] = taui;
			}
			
			/* compute tau sample indices */
			itau = (int)(taui/dtau);
			ltau = (int)(taul/dtau);
			
			/* loop over tau samples crossed by ray */
			for (jtau=ltau+1; jtau<=MIN(itau,ntau-1); ++jtau) {
				
				/* tau of sample crossed */
				tauj = jtau*dtau;
				
				/* time and amp via linear interpolation */
				frac = (tauj-taul)/(taui-taul);
				ktau++;
				t[jp][ktau] = (1.0f-frac)*tl+frac*ti;
				a[jp][ktau] = (1.0f-frac)*al+frac*ai;
			}
			
			/* update angle */
			angle += pj*dvel;
			
			/* update velocity and first difference */
			if (itau<ntau-1) {
				frac = (taui-(itau*dtau))/dtau;
				dvel = 0.5f*(vtau[itau+1]-vtau[itau]);
				vel = 0.5f*((1.0f-frac)*vtau[itau] +
					frac*vtau[itau+1]);
			} else {
				dvel = 0.5f*(vtau[ntau-1]-vtau[ntau-2]);
				vel = 0.5f*vtau[ntau-1] +
					(taui-(ntau-1)*dtau)*dvel/dtau;
			}
		}
		
		/* extrapolate times and amplitudes for interpolation */
		for (jtau=ktau+1; jtau<ntau; ++jtau) {
			t[jp][jtau] = t[jp][ktau];
			a[jp][jtau] = a[jp][ktau];
		}
		
		/* print turning time and max,min tau */
		/*
		if (verbose)
			_ftprintf_s(stderr,"p=%g tturn=%g taumax=%g taumin=%g\n",
				p[jp],tturn[jp],taumax[jp],taumin[jp]);
		*/
	}
	return table;
}

