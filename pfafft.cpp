#include "stdafx.h"
/* Copyright (c) Colorado School of Mines, 2011.*/
/* All rights reserved.                       */

/*********************** self documentation **********************/
/*****************************************************************************
PFAFFT - Functions to perform Prime Factor (PFA) FFT's, in place

npfa		return valid n for complex-to-complex PFA
npfar		return valid n for real-to-complex/complex-to-real PFA
npfao		return optimal n for complex-to-complex PFA
npfaro		return optimal n for real-to-complex/complex-to-real PFA
pfacc		1D PFA complex to complex
pfacr		1D PFA complex to real
pfarc		1D PFA real to complex
pfamcc		multiple PFA complex to real
pfa2cc		2D PFA complex to complex
pfa2cr		2D PFA complex to real
pfa2rc		2D PFA real to complex

*****************************************************************************
Function Prototypes:
int npfa (int nmin);
int npfao (int nmin, int nmax);
int npfar (int nmin);
int npfaro (int nmin, int nmax);
void pfacc (int isign, int n, complex z[]);
void pfacr (int isign, int n, complex cz[], float rz[]);
void pfarc (int isign, int n, float rz[], complex cz[]);
void pfamcc (int isign, int n, int nt, int k, int kt, complex z[]);
void pfa2cc (int isign, int idim, int n1, int n2, complex z[]);
void pfa2cr (int isign, int idim, int n1, int n2, complex cz[], float rz[]);
void pfa2rc (int isign, int idim, int n1, int n2, float rz[], complex cz[]);

*****************************************************************************
npfa:
Input:
nmin		lower bound on returned value (see notes below)

Returned:	valid n for prime factor fft

******************************************************************************
npfao
Input:
nmin		lower bound on returned value (see notes below)
nmax		desired (but not guaranteed) upper bound on returned value

Returned:	valid n for prime factor fft

******************************************************************************
npfar
Input:
nmin		lower bound on returned value

Returned:	valid n for real-to-complex/complex-to-real prime factor fft

*****************************************************************************
npfaro:
Input:
nmin		lower bound on returned value
nmax		desired (but not guaranteed) upper bound on returned value

Returned:	valid n for real-to-complex/complex-to-real prime factor fft

******************************************************************************
pfacc:
Input:
isign		sign of isign is the sign of exponent in fourier kernel
n		length of transform (see notes below)
z		array[n] of complex numbers to be transformed in place

Output:
z		array[n] of complex numbers transformed

******************************************************************************
pfacr:
Input:
isign       sign of isign is the sign of exponent in fourier kernel
n           length of transform (see notes below)
cz          array[n/2+1] of complex values (may be equivalenced to rz)

Output:
rz          array[n] of real values (may be equivalenced to cz)

******************************************************************************
pfarc:
Input:
isign       sign of isign is the sign of exponent in fourier kernel
n           length of transform; must be even (see notes below)
rz          array[n] of real values (may be equivalenced to cz)

Output:
cz          array[n/2+1] of complex values (may be equivalenced to rz)

******************************************************************************
pfamcc:
Input:
isign       	sign of isign is the sign of exponent in fourier kernel
n           	number of complex elements per transform (see notes below)
nt          	number of transforms
k           	stride in complex elements within transforms
kt          	stride in complex elements between transforms
z           	array of complex elements to be transformed in place

Output:
z		array of complex elements transformed

******************************************************************************
pfa2cc:
Input:
isign       	sign of isign is the sign of exponent in fourier kernel
idim        	dimension to transform, either 1 or 2 (see notes)
n1          	1st (fast) dimension of array to be transformed (see notes)
n2          	2nd (slow) dimension of array to be transformed (see notes)
z           	array[n2][n1] of complex elements to be transformed in place

Output:
z		array[n2][n1] of complex elements transformed

******************************************************************************
pfa2cr:
Input:
isign       sign of isign is the sign of exponent in fourier kernel
idim        dimension to transform, which must be either 1 or 2 (see notes)
n1          1st (fast) dimension of array to be transformed (see notes)
n2          2nd (slow) dimension of array to be transformed (see notes)
cz          array of complex values (may be equivalenced to rz)

Output:
rz          array of real values (may be equivalenced to cz)

******************************************************************************
pfa2rc:
Input:
isign       sign of isign is the sign of exponent in fourier kernel
idim        dimension to transform, which must be either 1 or 2 (see notes)
n1          1st (fast) dimension of array to be transformed (see notes)
n2          2nd (slow) dimension of array to be transformed (see notes)
rz          array of real values (may be equivalenced to cz)

Output:
cz          array of complex values (may be equivalenced to rz)

******************************************************************************
Notes:
Table of valid n and cost for prime factor fft.  For each n, cost
was estimated to be the inverse of the number of ffts done in 1 sec
on an IBM RISC System/6000 Model 320H, by Dave Hale, 08/04/91.
(Redone by Jack Cohen for 15 sec to rebuild NTAB table on advice of
David and Gregory Chudnovsky, 05/03/94).
Cost estimates are least accurate for very small n.  An alternative method
for estimating cost would be to count multiplies and adds, but this
method fails to account for the overlapping of multiplies and adds
that is possible on some computers, such as the IBM RS/6000 family.

npfa:
The returned n will be composed of mutually prime factors from
the set {2,3,4,5,7,8,9,11,13,16}.  Because n cannot exceed
720720 = 5*7*9*11*13*16, 720720 is returned if nmin exceeds 720720.

npfao:
The returned n will be composed of mutually prime factors from
the set {2,3,4,5,7,8,9,11,13,16}.  Because n cannot exceed
720720 = 5*7*9*11*13*16, 720720 is returned if nmin exceeds 720720.
If nmin does not exceed 720720, then the returned n will not be 
less than nmin.  The optimal n is chosen to minimize the estimated
cost of performing the fft, while satisfying the constraint, if
possible, that n not exceed nmax.

npfar and npfaro:
Current implemenations of real-to-complex and complex-to-real prime 
factor ffts require that the transform length n be even and that n/2 
be a valid length for a complex-to-complex prime factor fft.  The 
value returned by npfar satisfies these conditions.  Also, see notes 
for npfa.

pfacc:
n must be factorable into mutually prime factors taken 
from the set {2,3,4,5,7,8,9,11,13,16}.  in other words,
	n = 2**p * 3**q * 5**r * 7**s * 11**t * 13**u
where
	0 <= p <= 4,  0 <= q <= 2,  0 <= r,s,t,u <= 1
is required for pfa to yield meaningful results.  this
restriction implies that n is restricted to the range
	1 <= n <= 720720 (= 5*7*9*11*13*16)

pfacr:
Because pfacr uses pfacc to do most of the work, n must be even 
and n/2 must be a valid length for pfacc.  The simplest way to
obtain a valid n is via n = npfar(nmin).  A more optimal n can be 
obtained with npfaro.

pfarc:
Because pfarc uses pfacc to do most of the work, n must be even 
and n/2 must be a valid length for pfacc.  The simplest way to
obtain a valid n is via n = npfar(nmin).  A more optimal n can be 
obtained with npfaro.

pfamcc:
To perform a two-dimensional transform of an n1 by n2 complex array 
(assuming that both n1 and n2 are valid "n"), stored with n1 fast 
and n2 slow:
    pfamcc(isign,n1,n2,1,n1,z); (to transform 1st dimension)
    pfamcc(isign,n2,n1,n1,1,z); (to transform 2nd dimension)

pfa2cc:
Only one (either the 1st or 2nd) dimension of the 2-D array is transformed.

If idim equals 1, then n2 transforms of n1 complex elements are performed; 
else, if idim equals 2, then n1 transforms of n2 complex elements are 
performed.

Although z appears in the argument list as a one-dimensional array,
z may be viewed as an n1 by n2 two-dimensional array:  z[n2][n1].

Valid n is computed via the "np" subroutines.

To perform a two-dimensional transform of an n1 by n2 complex array 
(assuming that both n1 and n2 are valid "n"), stored with n1 fast 
and n2 slow:  pfa2cc(isign,1,n1,n2,z);  pfa2cc(isign,2,n1,n2,z);

pfa2cr:
If idim equals 1, then n2 transforms of n1/2+1 complex elements to n1 real 
elements are performed; else, if idim equals 2, then n1 transforms of n2/2+1 
complex elements to n2 real elements are performed.

Although rz appears in the argument list as a one-dimensional array,
rz may be viewed as an n1 by n2 two-dimensional array:  rz[n2][n1].  
Likewise, depending on idim, cz may be viewed as either an n1/2+1 by 
n2 or an n1 by n2/2+1 two-dimensional array of complex elements.

Let n denote the transform length, either n1 or n2, depending on idim.
Because pfa2rc uses pfa2cc to do most of the work, n must be even 
and n/2 must be a valid length for pfa2cc.  The simplest way to
obtain a valid n is via n = npfar(nmin).  A more optimal n can be 
obtained with npfaro.

pfa2rc:
If idim equals 1, then n2 transforms of n1 real elements to n1/2+1 complex 
elements are performed; else, if idim equals 2, then n1 transforms of n2 
real elements to n2/2+1 complex elements are performed.

Although rz appears in the argument list as a one-dimensional array,
rz may be viewed as an n1 by n2 two-dimensional array:  rz[n2][n1].  
Likewise, depending on idim, cz may be viewed as either an n1/2+1 by 
n2 or an n1 by n2/2+1 two-dimensional array of complex elements.

Let n denote the transform length, either n1 or n2, depending on idim.
Because pfa2rc uses pfa2cc to do most of the work, n must be even 
and n/2 must be a valid length for pfa2cc.  The simplest way to
obtain a valid n is via n = npfar(nmin).  A more optimal n can be 
obtained with npfaro.

******************************************************************************
References:  
Temperton, C., 1985, Implementation of a self-sorting
in-place prime factor fft algorithm:  Journal of
Computational Physics, v. 58, p. 283-299.

Temperton, C., 1988, A new set of minimum-add rotated
rotated dft modules: Journal of Computational Physics,
v. 75, p. 190-198.

Differ significantly from Press et al, 1988, Numerical Recipes in C, p. 417.

******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 04/27/89
*****************************************************************************/
/**************** end self doc ********************************/

#include "util.h"
#include "ealloc_d.h"
#include "pfafft.h"
#include "sumig.h"



int npfa (int nmin)
/*****************************************************************************
Return smallest valid n not less than nmin for prime factor fft.
******************************************************************************
Input:
nmin		lower bound on returned value (see notes below)

Returned:	valid n for prime factor fft
******************************************************************************
Notes:
The returned n will be composed of mutually prime factors from
the set {2,3,4,5,7,8,9,11,13,16}.  Because n cannot exceed
720720 = 5*7*9*11*13*16, 720720 is returned if nmin exceeds 720720.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 04/28/89
Modified:  Dave Hale, Colorado School of Mines, 08/05/91
	For efficiency, use pre-computed table of valid n and costs.
*****************************************************************************/
{
	int i;
	for (i=0; i<NTAB-1 && nctab[i].n<nmin; ++i);
	return nctab[i].n;
}

int npfao (int nmin, int nmax)
/*****************************************************************************
Return optimal n between nmin and nmax for prime factor fft.
******************************************************************************
Input:
nmin		lower bound on returned value (see notes below)
nmax		desired (but not guaranteed) upper bound on returned value

Returned:	valid n for prime factor fft
******************************************************************************
Notes:
The returned n will be composed of mutually prime factors from
the set {2,3,4,5,7,8,9,11,13,16}.  Because n cannot exceed
720720 = 5*7*9*11*13*16, 720720 is returned if nmin exceeds 720720.
If nmin does not exceed 720720, then the returned n will not be 
less than nmin.  The optimal n is chosen to minimize the estimated
cost of performing the fft, while satisfying the constraint, if
possible, that n not exceed nmax.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/13/89
Modified:  Dave Hale, Colorado School of Mines, 08/05/91
	For efficiency, use pre-computed table of valid n and costs.
*****************************************************************************/
{
	int i,j;
	for (i=0; i<NTAB-1 && nctab[i].n<nmin; ++i);
	for (j=i+1; j<NTAB-1 && nctab[j].n<=nmax; ++j)
		if (nctab[j].c<nctab[i].c) i = j;
	return nctab[i].n;
}

int npfar (int nmin)
/*****************************************************************************
Return smallest valid n not less than nmin for real-to-complex or 
complex-to-real prime factor ffts.
******************************************************************************
Input:
nmin		lower bound on returned value

Returned:	valid n for real-to-complex/complex-to-real prime factor fft
******************************************************************************
Notes:
Current implemenations of real-to-complex and complex-to-real prime 
factor ffts require that the transform length n be even and that n/2 
be a valid length for a complex-to-complex prime factor fft.  The 
value returned by npfar satisfies these conditions.  Also, see notes 
for npfa.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/16/89
*****************************************************************************/
{
    return 2*npfa((nmin+1)/2);
}

int npfaro (int nmin, int nmax)
/*****************************************************************************
Return optimal n between nmin and nmax for real-to-complex or 
complex-to-real prime factor ffts
******************************************************************************
Input:
nmin		lower bound on returned value
nmax		desired (but not guaranteed) upper bound on returned value

Returned:	valid n for real-to-complex/complex-to-real prime factor fft
******************************************************************************
Notes:
Current implemenations of real-to-complex and complex-to-real prime 
factor ffts require that the transform length n be even and that n/2 
be a valid length for a complex-to-complex prime factor fft.  The 
value returned by npfaro satisfies these conditions.  Also, see notes 
for npfao.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/16/89
*****************************************************************************/
{
    return 2*npfao((nmin+1)/2,(nmax+1)/2);
}

#define P120 0.120536680f
#define P142 0.142314838f
#define P173 0.173648178f
#define P222 0.222520934f
#define P239 0.239315664f
#define P281 0.281732557f
#define P342 0.342020143f
#define P354 0.354604887f
#define P382 0.382683432f
#define P415 0.415415013f
#define P433 0.433883739f
#define P464 0.464723172f
#define P540 0.540640817f
#define P559 0.559016994f
#define P568 0.568064747f
#define P587 0.587785252f
#define P623 0.623489802f
#define P642 0.642787610f
#define P654 0.654860734f
#define P663 0.663122658f
#define P707 0.707106781f
#define P748 0.748510748f
#define P755 0.755749574f
#define P766 0.766044443f
#define P781 0.781831482f
#define P822 0.822983866f
#define P841 0.841253533f
#define P866 0.866025404f
#define P885 0.885456026f
#define P900 0.900968868f
#define P909 0.909631995f
#define P923 0.923879533f
#define P935 0.935016243f
#define P939 0.939692621f
#define P951 0.951056516f
#define P959 0.959492974f
#define P970 0.970941817f
#define P974 0.974927912f
#define P984 0.984807753f
#define P989 0.989821442f
#define P992 0.992708874f
#define NFAX 10

void pfacc (int isign, int n, complex cz[])
/*****************************************************************************
Prime factor fft:  complex to complex transform, in place
******************************************************************************
Input:
isign		sign of isign is the sign of exponent in fourier kernel
n		length of transform (see notes below)
z		array[n] of complex numbers to be transformed in place

Output:
z		array[n] of complex numbers transformed
******************************************************************************
Notes:
n must be factorable into mutually prime factors taken 
from the set {2,3,4,5,7,8,9,11,13,16}.  in other words,
	n = 2**p * 3**q * 5**r * 7**s * 11**t * 13**u
where
	0 <= p <= 4,  0 <= q <= 2,  0 <= r,s,t,u <= 1
is required for pfa to yield meaningful results.  this
restriction implies that n is restricted to the range
	1 <= n <= 720720 (= 5*7*9*11*13*16)
******************************************************************************
References:  
Temperton, C., 1985, Implementation of a self-sorting
in-place prime factor fft algorithm:  Journal of
Computational Physics, v. 58, p. 283-299.

Temperton, C., 1988, A new set of minimum-add rotated
rotated dft modules: Journal of Computational Physics,
v. 75, p. 190-198.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 04/27/89
*****************************************************************************/
{
	static int kfax[] = { 16,13,11,9,8,7,5,4,3,2 };
	register float *zr=(float*)cz;	/* Real */
	register float *zi= zr+1;	/* Imag */ 
	register int j00,j01,j2,j3,j4,j5,j6,j7,j8,j9,j10,j11,j12,j13,j14,j15,jt;
	int nleft,jfax,ifac,jfac,jinc,jmax,ndiv,m,mm=0,mu=0,l;
	float t1r,t1i,t2r,t2i,t3r,t3i,t4r,t4i,t5r,t5i,
		t6r,t6i,t7r,t7i,t8r,t8i,t9r,t9i,t10r,t10i,
		t11r,t11i,t12r,t12i,t13r,t13i,t14r,t14i,t15r,t15i,
		t16r,t16i,t17r,t17i,t18r,t18i,t19r,t19i,t20r,t20i,
		t21r,t21i,t22r,t22i,t23r,t23i,t24r,t24i,t25r,t25i,
		t26r,t26i,t27r,t27i,t28r,t28i,t29r,t29i,t30r,t30i,
		t31r,t31i,t32r,t32i,t33r,t33i,t34r,t34i,t35r,t35i,
		t36r,t36i,t37r,t37i,t38r,t38i,t39r,t39i,t40r,t40i,
		t41r,t41i,t42r,t42i,
		y1r,y1i,y2r,y2i,y3r,y3i,y4r,y4i,y5r,y5i,
		y6r,y6i,y7r,y7i,y8r,y8i,y9r,y9i,y10r,y10i,
		y11r,y11i,y12r,y12i,y13r,y13i,y14r,y14i,y15r,y15i,
		c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12;

	/* keep track of n left after dividing by factors */
	nleft = n;

	/* begin loop over possible factors (from biggest to smallest) */
	for (jfax=0; jfax<NFAX; jfax++) {

		/* skip if not a mutually prime factor of n */
        ifac = kfax[jfax];
        ndiv = nleft/ifac;
        if (ndiv*ifac!=nleft) continue;
 
		/* update n left and determine n divided by factor */
        nleft = ndiv;
        m = n/ifac;
 
		/* determine rotation factor mu and stride mm */
        for (jfac=1; jfac<=ifac; jfac++) {
			mu = jfac;
			mm = jfac*m;
			if (mm%ifac==1) break;
		}
 
		/* adjust rotation factor for sign of transform */
        if (isign<0) mu = ifac-mu;
 
		/* compute stride, limit, and pointers */
        jinc = 2*mm;
	jmax = 2*n;
        j00 = 0;
        j01 = j00+jinc;

		/* if factor is 2 */
        if (ifac==2) {
			for (l=0; l<m; l++) {
				t1r = zr[j00]-zr[j01];
				t1i = zi[j00]-zi[j01];
				zr[j00] = zr[j00]+zr[j01];
				zi[j00] = zi[j00]+zi[j01];
				zr[j01] = t1r;
				zi[j01] = t1i;
				jt = j01+2;
				j01 = j00+2;
				j00 = jt;
			}
			continue;
		}
        j2 = j01+jinc;
        if (j2>=jmax) j2 = j2-jmax;

		/* if factor is 3 */
        if (ifac==3) {
			if (mu==1)
				c1 = P866;
			else
				c1 = -P866;
			for (l=0; l<m; l++) {
				t1r = zr[j01]+zr[j2];
				t1i = zi[j01]+zi[j2];
				y1r = zr[j00]-0.5f*t1r;
				y1i = zi[j00]-0.5f*t1i;
				y2r = c1*(zr[j01]-zr[j2]);
				y2i = c1*(zi[j01]-zi[j2]);
				zr[j00] = zr[j00]+t1r;
				zi[j00] = zi[j00]+t1i;
				zr[j01] = y1r-y2i;
				zi[j01] = y1i+y2r;
				zr[j2] = y1r+y2i;
				zi[j2] = y1i-y2r;
				jt = j2+2;
				j2 = j01+2;
				j01 = j00+2;
				j00 = jt;
			}
			continue;
		}
		j3 = j2+jinc;
		if (j3>=jmax) j3 = j3-jmax;

		/* if factor is 4 */
		if (ifac==4) {
			if (mu==1)
				c1 = 1.0;
			else
				c1 = -1.0;
			for (l=0; l<m; l++) {
				t1r = zr[j00]+zr[j2];
				t1i = zi[j00]+zi[j2];
				t2r = zr[j01]+zr[j3];
				t2i = zi[j01]+zi[j3];
				y1r = zr[j00]-zr[j2];
				y1i = zi[j00]-zi[j2];
				y3r = c1*(zr[j01]-zr[j3]);
				y3i = c1*(zi[j01]-zi[j3]);
				zr[j00] = t1r+t2r;
				zi[j00] = t1i+t2i;
				zr[j01] = y1r-y3i;
				zi[j01] = y1i+y3r;
				zr[j2] = t1r-t2r;
				zi[j2] = t1i-t2i;
				zr[j3] = y1r+y3i;
				zi[j3] = y1i-y3r;
				jt = j3+2;
				j3 = j2+2;
				j2 = j01+2;
				j01 = j00+2;
				j00 = jt;
			}
			continue;
		}
		j4 = j3+jinc;
		if (j4>=jmax) j4 = j4-jmax;

		/* if factor is 5 */
		if (ifac==5) {
			if (mu==1) {
				c1 = P559;
				c2 = P951;
				c3 = P587;
			} else if (mu==2) {
				c1 = -P559;
				c2 = P587;
				c3 = -P951;
			} else if (mu==3) {
				c1 = -P559;
				c2 = -P587;
				c3 = P951;
			} else { 
				c1 = P559;
				c2 = -P951;
				c3 = -P587;
			}
			for (l=0; l<m; l++) {
				t1r = zr[j01]+zr[j4];
				t1i = zi[j01]+zi[j4];
				t2r = zr[j2]+zr[j3];
				t2i = zi[j2]+zi[j3];
				t3r = zr[j01]-zr[j4];
				t3i = zi[j01]-zi[j4];
				t4r = zr[j2]-zr[j3];
				t4i = zi[j2]-zi[j3];
				t5r = t1r+t2r;
				t5i = t1i+t2i;
				t6r = c1*(t1r-t2r);
				t6i = c1*(t1i-t2i);
				t7r = zr[j00]-0.25f*t5r;
				t7i = zi[j00]-0.25f*t5i;
				y1r = t7r+t6r;
				y1i = t7i+t6i;
				y2r = t7r-t6r;
				y2i = t7i-t6i;
				y3r = c3*t3r-c2*t4r;
				y3i = c3*t3i-c2*t4i;
				y4r = c2*t3r+c3*t4r;
				y4i = c2*t3i+c3*t4i;
				zr[j00] = zr[j00]+t5r;
				zi[j00] = zi[j00]+t5i;
				zr[j01] = y1r-y4i;
				zi[j01] = y1i+y4r;
				zr[j2] = y2r-y3i;
				zi[j2] = y2i+y3r;
				zr[j3] = y2r+y3i;
				zi[j3] = y2i-y3r;
				zr[j4] = y1r+y4i;
				zi[j4] = y1i-y4r;
				jt = j4+2;
				j4 = j3+2;
				j3 = j2+2;
				j2 = j01+2;
				j01 = j00+2;
				j00 = jt;
			}
			continue;
		}
		j5 = j4+jinc;
		if (j5>=jmax) j5 = j5-jmax;
		j6 = j5+jinc;
		if (j6>=jmax) j6 = j6-jmax;

		/* if factor is 7 */
		if (ifac==7) {
			if (mu==1) {
				c1 = P623;
				c2 = -P222;
				c3 = -P900;
				c4 = P781;
				c5 = P974;
				c6 = P433;
			} else if (mu==2) {
				c1 = -P222;
				c2 = -P900;
				c3 = P623;
				c4 = P974;
				c5 = -P433;
				c6 = -P781;
			} else if (mu==3) {
				c1 = -P900;
				c2 = P623;
				c3 = -P222;
				c4 = P433;
				c5 = -P781;
				c6 = P974;
			} else if (mu==4) {
				c1 = -P900;
				c2 = P623;
				c3 = -P222;
				c4 = -P433;
				c5 = P781;
				c6 = -P974;
			} else if (mu==5) {
				c1 = -P222;
				c2 = -P900;
				c3 = P623;
				c4 = -P974;
				c5 = P433;
				c6 = P781;
			} else {
				c1 = P623;
				c2 = -P222;
				c3 = -P900;
				c4 = -P781;
				c5 = -P974;
				c6 = -P433;
			}
			for (l=0; l<m; l++) {
				t1r = zr[j01]+zr[j6];
				t1i = zi[j01]+zi[j6];
				t2r = zr[j2]+zr[j5];
				t2i = zi[j2]+zi[j5];
				t3r = zr[j3]+zr[j4];
				t3i = zi[j3]+zi[j4];
				t4r = zr[j01]-zr[j6];
				t4i = zi[j01]-zi[j6];
				t5r = zr[j2]-zr[j5];
				t5i = zi[j2]-zi[j5];
				t6r = zr[j3]-zr[j4];
				t6i = zi[j3]-zi[j4];
				t7r = zr[j00]-0.5f*t3r;
				t7i = zi[j00]-0.5f*t3i;
				t8r = t1r-t3r;
				t8i = t1i-t3i;
				t9r = t2r-t3r;
				t9i = t2i-t3i;
				y1r = t7r+c1*t8r+c2*t9r;
				y1i = t7i+c1*t8i+c2*t9i;
				y2r = t7r+c2*t8r+c3*t9r;
				y2i = t7i+c2*t8i+c3*t9i;
				y3r = t7r+c3*t8r+c1*t9r;
				y3i = t7i+c3*t8i+c1*t9i;
				y4r = c6*t4r-c4*t5r+c5*t6r;
				y4i = c6*t4i-c4*t5i+c5*t6i;
				y5r = c5*t4r-c6*t5r-c4*t6r;
				y5i = c5*t4i-c6*t5i-c4*t6i;
				y6r = c4*t4r+c5*t5r+c6*t6r;
				y6i = c4*t4i+c5*t5i+c6*t6i;
				zr[j00] = zr[j00]+t1r+t2r+t3r;
				zi[j00] = zi[j00]+t1i+t2i+t3i;
				zr[j01] = y1r-y6i;
				zi[j01] = y1i+y6r;
				zr[j2] = y2r-y5i;
				zi[j2] = y2i+y5r;
				zr[j3] = y3r-y4i;
				zi[j3] = y3i+y4r;
				zr[j4] = y3r+y4i;
				zi[j4] = y3i-y4r;
				zr[j5] = y2r+y5i;
				zi[j5] = y2i-y5r;
				zr[j6] = y1r+y6i;
				zi[j6] = y1i-y6r;
				jt = j6+2;
				j6 = j5+2;
				j5 = j4+2;
				j4 = j3+2;
				j3 = j2+2;
				j2 = j01+2;
				j01 = j00+2;
				j00 = jt;
			}
			continue;
		}
		j7 = j6+jinc;
		if (j7>=jmax) j7 = j7-jmax;

		/* if factor is 8 */
		if (ifac==8) {
			if (mu==1) {
				c1 = 1.0;
				c2 = P707;
			} else if (mu==3) {
				c1 = -1.0;
				c2 = -P707;
			} else if (mu==5) {
				c1 = 1.0;
				c2 = -P707;
			} else {
				c1 = -1.0;
				c2 = P707;
			}
			c3 = c1*c2;
			for (l=0; l<m; l++) {
				t1r = zr[j00]+zr[j4];
				t1i = zi[j00]+zi[j4];
				t2r = zr[j00]-zr[j4];
				t2i = zi[j00]-zi[j4];
				t3r = zr[j01]+zr[j5];
				t3i = zi[j01]+zi[j5];
				t4r = zr[j01]-zr[j5];
				t4i = zi[j01]-zi[j5];
				t5r = zr[j2]+zr[j6];
				t5i = zi[j2]+zi[j6];
				t6r = c1*(zr[j2]-zr[j6]);
				t6i = c1*(zi[j2]-zi[j6]);
				t7r = zr[j3]+zr[j7];
				t7i = zi[j3]+zi[j7];
				t8r = zr[j3]-zr[j7];
				t8i = zi[j3]-zi[j7];
				t9r = t1r+t5r;
				t9i = t1i+t5i;
				t10r = t3r+t7r;
				t10i = t3i+t7i;
				t11r = c2*(t4r-t8r);
				t11i = c2*(t4i-t8i);
				t12r = c3*(t4r+t8r);
				t12i = c3*(t4i+t8i);
				y1r = t2r+t11r;
				y1i = t2i+t11i;
				y2r = t1r-t5r;
				y2i = t1i-t5i;
				y3r = t2r-t11r;
				y3i = t2i-t11i;
				y5r = t12r-t6r;
				y5i = t12i-t6i;
				y6r = c1*(t3r-t7r);
				y6i = c1*(t3i-t7i);
				y7r = t12r+t6r;
				y7i = t12i+t6i;
				zr[j00] = t9r+t10r;
				zi[j00] = t9i+t10i;
				zr[j01] = y1r-y7i;
				zi[j01] = y1i+y7r;
				zr[j2] = y2r-y6i;
				zi[j2] = y2i+y6r;
				zr[j3] = y3r-y5i;
				zi[j3] = y3i+y5r;
				zr[j4] = t9r-t10r;
				zi[j4] = t9i-t10i;
				zr[j5] = y3r+y5i;
				zi[j5] = y3i-y5r;
				zr[j6] = y2r+y6i;
				zi[j6] = y2i-y6r;
				zr[j7] = y1r+y7i;
				zi[j7] = y1i-y7r;
				jt = j7+2;
				j7 = j6+2;
				j6 = j5+2;
				j5 = j4+2;
				j4 = j3+2;
				j3 = j2+2;
				j2 = j01+2;
				j01 = j00+2;
				j00 = jt;
			}
			continue;
		}
		j8 = j7+jinc;
		if (j8>=jmax) j8 = j8-jmax;

		/* if factor is 9 */
		if (ifac==9) {
			if (mu==1) {
				c1 = P866;
				c2 = P766;
				c3 = P642;
				c4 = P173;
				c5 = P984;
			} else if (mu==2) {
				c1 = -P866;
				c2 = P173;
				c3 = P984;
				c4 = -P939;
				c5 = P342;
			} else if (mu==4) {
				c1 = P866;
				c2 = -P939;
				c3 = P342;
				c4 = P766;
				c5 = -P642;
			} else if (mu==5) {
				c1 = -P866;
				c2 = -P939;
				c3 = -P342;
				c4 = P766;
				c5 = P642;
			} else if (mu==7) {
				c1 = P866;
				c2 = P173;
				c3 = -P984;
				c4 = -P939;
				c5 = -P342;
			} else {
				c1 = -P866;
				c2 = P766;
				c3 = -P642;
				c4 = P173;
				c5 = -P984;
			}
			c6 = c1*c2;
			c7 = c1*c3;
			c8 = c1*c4;
			c9 = c1*c5;
			for (l=0; l<m; l++) {
				t1r = zr[j3]+zr[j6];
				t1i = zi[j3]+zi[j6];
				t2r = zr[j00]-0.5f*t1r;
				t2i = zi[j00]-0.5f*t1i;
				t3r = c1*(zr[j3]-zr[j6]);
				t3i = c1*(zi[j3]-zi[j6]);
				t4r = zr[j00]+t1r;
				t4i = zi[j00]+t1i;
				t5r = zr[j4]+zr[j7];
				t5i = zi[j4]+zi[j7];
				t6r = zr[j01]-0.5f*t5r;
				t6i = zi[j01]-0.5f*t5i;
				t7r = zr[j4]-zr[j7];
				t7i = zi[j4]-zi[j7];
				t8r = zr[j01]+t5r;
				t8i = zi[j01]+t5i;
				t9r = zr[j2]+zr[j5];
				t9i = zi[j2]+zi[j5];
				t10r = zr[j8]-0.5f*t9r;
				t10i = zi[j8]-0.5f*t9i;
				t11r = zr[j2]-zr[j5];
				t11i = zi[j2]-zi[j5];
				t12r = zr[j8]+t9r;
				t12i = zi[j8]+t9i;
				t13r = t8r+t12r;
				t13i = t8i+t12i;
				t14r = t6r+t10r;
				t14i = t6i+t10i;
				t15r = t6r-t10r;
				t15i = t6i-t10i;
				t16r = t7r+t11r;
				t16i = t7i+t11i;
				t17r = t7r-t11r;
				t17i = t7i-t11i;
				t18r = c2*t14r-c7*t17r;
				t18i = c2*t14i-c7*t17i;
				t19r = c4*t14r+c9*t17r;
				t19i = c4*t14i+c9*t17i;
				t20r = c3*t15r+c6*t16r;
				t20i = c3*t15i+c6*t16i;
				t21r = c5*t15r-c8*t16r;
				t21i = c5*t15i-c8*t16i;
				t22r = t18r+t19r;
				t22i = t18i+t19i;
				t23r = t20r-t21r;
				t23i = t20i-t21i;
				y1r = t2r+t18r;
				y1i = t2i+t18i;
				y2r = t2r+t19r;
				y2i = t2i+t19i;
				y3r = t4r-0.5f*t13r;
				y3i = t4i-0.5f*t13i;
				y4r = t2r-t22r;
				y4i = t2i-t22i;
				y5r = t3r-t23r;
				y5i = t3i-t23i;
				y6r = c1*(t8r-t12r);
				y6i = c1*(t8i-t12i);
				y7r = t21r-t3r;
				y7i = t21i-t3i;
				y8r = t3r+t20r;
				y8i = t3i+t20i;
				zr[j00] = t4r+t13r;
				zi[j00] = t4i+t13i;
				zr[j01] = y1r-y8i;
				zi[j01] = y1i+y8r;
				zr[j2] = y2r-y7i;
				zi[j2] = y2i+y7r;
				zr[j3] = y3r-y6i;
				zi[j3] = y3i+y6r;
				zr[j4] = y4r-y5i;
				zi[j4] = y4i+y5r;
				zr[j5] = y4r+y5i;
				zi[j5] = y4i-y5r;
				zr[j6] = y3r+y6i;
				zi[j6] = y3i-y6r;
				zr[j7] = y2r+y7i;
				zi[j7] = y2i-y7r;
				zr[j8] = y1r+y8i;
				zi[j8] = y1i-y8r;
				jt = j8+2;
				j8 = j7+2;
				j7 = j6+2;
				j6 = j5+2;
				j5 = j4+2;
				j4 = j3+2;
				j3 = j2+2;
				j2 = j01+2;
				j01 = j00+2;
				j00 = jt;
			}
			continue;
		}
		j9 = j8+jinc;
		if (j9>=jmax) j9 = j9-jmax;
		j10 = j9+jinc;
		if (j10>=jmax) j10 = j10-jmax;

		/* if factor is 11 */
		if (ifac==11) {
			if (mu==1) {
				c1 = P841;
				c2 = P415;
				c3 = -P142;
				c4 = -P654;
				c5 = -P959;
				c6 = P540;
				c7 = P909;
				c8 = P989;
				c9 = P755;
				c10 = P281;
			} else if (mu==2) {
				c1 = P415;
				c2 = -P654;
				c3 = -P959;
				c4 = -P142;
				c5 = P841;
				c6 = P909;
				c7 = P755;
				c8 = -P281;
				c9 = -P989;
				c10 = -P540;
			} else if (mu==3) {
				c1 = -P142;
				c2 = -P959;
				c3 = P415;
				c4 = P841;
				c5 = -P654;
				c6 = P989;
				c7 = -P281;
				c8 = -P909;
				c9 = P540;
				c10 = P755;
			} else if (mu==4) {
				c1 = -P654;
				c2 = -P142;
				c3 = P841;
				c4 = -P959;
				c5 = P415;
				c6 = P755;
				c7 = -P989;
				c8 = P540;
				c9 = P281;
				c10 = -P909;
			} else if (mu==5) {
				c1 = -P959;
				c2 = P841;
				c3 = -P654;
				c4 = P415;
				c5 = -P142;
				c6 = P281;
				c7 = -P540;
				c8 = P755;
				c9 = -P909;
				c10 = P989;
			} else if (mu==6) {
				c1 = -P959;
				c2 = P841;
				c3 = -P654;
				c4 = P415;
				c5 = -P142;
				c6 = -P281;
				c7 = P540;
				c8 = -P755;
				c9 = P909;
				c10 = -P989;
			} else if (mu==7) {
				c1 = -P654;
				c2 = -P142;
				c3 = P841;
				c4 = -P959;
				c5 = P415;
				c6 = -P755;
				c7 = P989;
				c8 = -P540;
				c9 = -P281;
				c10 = P909;
			} else if (mu==8) {
				c1 = -P142;
				c2 = -P959;
				c3 = P415;
				c4 = P841;
				c5 = -P654;
				c6 = -P989;
				c7 = P281;
				c8 = P909;
				c9 = -P540;
				c10 = -P755;
			} else if (mu==9) {
				c1 = P415;
				c2 = -P654;
				c3 = -P959;
				c4 = -P142;
				c5 = P841;
				c6 = -P909;
				c7 = -P755;
				c8 = P281;
				c9 = P989;
				c10 = P540;
			} else {
				c1 = P841;
				c2 = P415;
				c3 = -P142;
				c4 = -P654;
				c5 = -P959;
				c6 = -P540;
				c7 = -P909;
				c8 = -P989;
				c9 = -P755;
				c10 = -P281;
			}
			for (l=0; l<m; l++) {
				t1r = zr[j01]+zr[j10];
				t1i = zi[j01]+zi[j10];
				t2r = zr[j2]+zr[j9];
				t2i = zi[j2]+zi[j9];
				t3r = zr[j3]+zr[j8];
				t3i = zi[j3]+zi[j8];
				t4r = zr[j4]+zr[j7];
				t4i = zi[j4]+zi[j7];
				t5r = zr[j5]+zr[j6];
				t5i = zi[j5]+zi[j6];
				t6r = zr[j01]-zr[j10];
				t6i = zi[j01]-zi[j10];
				t7r = zr[j2]-zr[j9];
				t7i = zi[j2]-zi[j9];
				t8r = zr[j3]-zr[j8];
				t8i = zi[j3]-zi[j8];
				t9r = zr[j4]-zr[j7];
				t9i = zi[j4]-zi[j7];
				t10r = zr[j5]-zr[j6];
				t10i = zi[j5]-zi[j6];
				t11r = zr[j00]-0.5f*t5r;
				t11i = zi[j00]-0.5f*t5i;
				t12r = t1r-t5r;
				t12i = t1i-t5i;
				t13r = t2r-t5r;
				t13i = t2i-t5i;
				t14r = t3r-t5r;
				t14i = t3i-t5i;
				t15r = t4r-t5r;
				t15i = t4i-t5i;
				y1r = t11r+c1*t12r+c2*t13r+c3*t14r+c4*t15r;
				y1i = t11i+c1*t12i+c2*t13i+c3*t14i+c4*t15i;
				y2r = t11r+c2*t12r+c4*t13r+c5*t14r+c3*t15r;
				y2i = t11i+c2*t12i+c4*t13i+c5*t14i+c3*t15i;
				y3r = t11r+c3*t12r+c5*t13r+c2*t14r+c1*t15r;
				y3i = t11i+c3*t12i+c5*t13i+c2*t14i+c1*t15i;
				y4r = t11r+c4*t12r+c3*t13r+c1*t14r+c5*t15r;
				y4i = t11i+c4*t12i+c3*t13i+c1*t14i+c5*t15i;
				y5r = t11r+c5*t12r+c1*t13r+c4*t14r+c2*t15r;
				y5i = t11i+c5*t12i+c1*t13i+c4*t14i+c2*t15i;
				y6r = c10*t6r-c6*t7r+c9*t8r-c7*t9r+c8*t10r;
				y6i = c10*t6i-c6*t7i+c9*t8i-c7*t9i+c8*t10i;
				y7r = c9*t6r-c8*t7r+c6*t8r+c10*t9r-c7*t10r;
				y7i = c9*t6i-c8*t7i+c6*t8i+c10*t9i-c7*t10i;
				y8r = c8*t6r-c10*t7r-c7*t8r+c6*t9r+c9*t10r;
				y8i = c8*t6i-c10*t7i-c7*t8i+c6*t9i+c9*t10i;
				y9r = c7*t6r+c9*t7r-c10*t8r-c8*t9r-c6*t10r;
				y9i = c7*t6i+c9*t7i-c10*t8i-c8*t9i-c6*t10i;
				y10r = c6*t6r+c7*t7r+c8*t8r+c9*t9r+c10*t10r;
				y10i = c6*t6i+c7*t7i+c8*t8i+c9*t9i+c10*t10i;
				zr[j00] = zr[j00]+t1r+t2r+t3r+t4r+t5r;
				zi[j00] = zi[j00]+t1i+t2i+t3i+t4i+t5i;
				zr[j01] = y1r-y10i;
				zi[j01] = y1i+y10r;
				zr[j2] = y2r-y9i;
				zi[j2] = y2i+y9r;
				zr[j3] = y3r-y8i;
				zi[j3] = y3i+y8r;
				zr[j4] = y4r-y7i;
				zi[j4] = y4i+y7r;
				zr[j5] = y5r-y6i;
				zi[j5] = y5i+y6r;
				zr[j6] = y5r+y6i;
				zi[j6] = y5i-y6r;
				zr[j7] = y4r+y7i;
				zi[j7] = y4i-y7r;
				zr[j8] = y3r+y8i;
				zi[j8] = y3i-y8r;
				zr[j9] = y2r+y9i;
				zi[j9] = y2i-y9r;
				zr[j10] = y1r+y10i;
				zi[j10] = y1i-y10r;
				jt = j10+2;
				j10 = j9+2;
				j9 = j8+2;
				j8 = j7+2;
				j7 = j6+2;
				j6 = j5+2;
				j5 = j4+2;
				j4 = j3+2;
				j3 = j2+2;
				j2 = j01+2;
				j01 = j00+2;
				j00 = jt;
			}
			continue;
		}
		j11 = j10+jinc;
		if (j11>=jmax) j11 = j11-jmax;
		j12 = j11+jinc;
		if (j12>=jmax) j12 = j12-jmax;

		/* if factor is 13 */
		if (ifac==13) {
			if (mu==1) {
				c1 = P885;
				c2 = P568;
				c3 = P120;
				c4 = -P354;
				c5 = -P748;
				c6 = -P970;
				c7 = P464;
				c8 = P822;
				c9 = P992;
				c10 = P935;
				c11 = P663;
				c12 = P239;
			} else if (mu==2) {
				c1 = P568;
				c2 = -P354;
				c3 = -P970;
				c4 = -P748;
				c5 = P120;
				c6 = P885;
				c7 = P822;
				c8 = P935;
				c9 = P239;
				c10 = -P663;
				c11 = -P992;
				c12 = -P464;
			} else if (mu==3) {
				c1 = P120;
				c2 = -P970;
				c3 = -P354;
				c4 = P885;
				c5 = P568;
				c6 = -P748;
				c7 = P992;
				c8 = P239;
				c9 = -P935;
				c10 = -P464;
				c11 = P822;
				c12 = P663;
			} else if (mu==4) {
				c1 = -P354;
				c2 = -P748;
				c3 = P885;
				c4 = P120;
				c5 = -P970;
				c6 = P568;
				c7 = P935;
				c8 = -P663;
				c9 = -P464;
				c10 = P992;
				c11 = -P239;
				c12 = -P822;
			} else if (mu==5) {
				c1 = -P748;
				c2 = P120;
				c3 = P568;
				c4 = -P970;
				c5 = P885;
				c6 = -P354;
				c7 = P663;
				c8 = -P992;
				c9 = P822;
				c10 = -P239;
				c11 = -P464;
				c12 = P935;
			} else if (mu==6) {
				c1 = -P970;
				c2 = P885;
				c3 = -P748;
				c4 = P568;
				c5 = -P354;
				c6 = P120;
				c7 = P239;
				c8 = -P464;
				c9 = P663;
				c10 = -P822;
				c11 = P935;
				c12 = -P992;
			} else if (mu==7) {
				c1 = -P970;
				c2 = P885;
				c3 = -P748;
				c4 = P568;
				c5 = -P354;
				c6 = P120;
				c7 = -P239;
				c8 = P464;
				c9 = -P663;
				c10 = P822;
				c11 = -P935;
				c12 = P992;
			} else if (mu==8) {
				c1 = -P748;
				c2 = P120;
				c3 = P568;
				c4 = -P970;
				c5 = P885;
				c6 = -P354;
				c7 = -P663;
				c8 = P992;
				c9 = -P822;
				c10 = P239;
				c11 = P464;
				c12 = -P935;
			} else if (mu==9) {
				c1 = -P354;
				c2 = -P748;
				c3 = P885;
				c4 = P120;
				c5 = -P970;
				c6 = P568;
				c7 = -P935;
				c8 = P663;
				c9 = P464;
				c10 = -P992;
				c11 = P239;
				c12 = P822;
			} else if (mu==10) {
				c1 = P120;
				c2 = -P970;
				c3 = -P354;
				c4 = P885;
				c5 = P568;
				c6 = -P748;
				c7 = -P992;
				c8 = -P239;
				c9 = P935;
				c10 = P464;
				c11 = -P822;
				c12 = -P663;
			} else if (mu==11) {
				c1 = P568;
				c2 = -P354;
				c3 = -P970;
				c4 = -P748;
				c5 = P120;
				c6 = P885;
				c7 = -P822;
				c8 = -P935;
				c9 = -P239;
				c10 = P663;
				c11 = P992;
				c12 = P464;
			} else {
				c1 = P885;
				c2 = P568;
				c3 = P120;
				c4 = -P354;
				c5 = -P748;
				c6 = -P970;
				c7 = -P464;
				c8 = -P822;
				c9 = -P992;
				c10 = -P935;
				c11 = -P663;
				c12 = -P239;
			}
			for (l=0; l<m; l++) {
				t1r = zr[j01]+zr[j12];
				t1i = zi[j01]+zi[j12];
				t2r = zr[j2]+zr[j11];
				t2i = zi[j2]+zi[j11];
				t3r = zr[j3]+zr[j10];
				t3i = zi[j3]+zi[j10];
				t4r = zr[j4]+zr[j9];
				t4i = zi[j4]+zi[j9];
				t5r = zr[j5]+zr[j8];
				t5i = zi[j5]+zi[j8];
				t6r = zr[j6]+zr[j7];
				t6i = zi[j6]+zi[j7];
				t7r = zr[j01]-zr[j12];
				t7i = zi[j01]-zi[j12];
				t8r = zr[j2]-zr[j11];
				t8i = zi[j2]-zi[j11];
				t9r = zr[j3]-zr[j10];
				t9i = zi[j3]-zi[j10];
				t10r = zr[j4]-zr[j9];
				t10i = zi[j4]-zi[j9];
				t11r = zr[j5]-zr[j8];
				t11i = zi[j5]-zi[j8];
				t12r = zr[j6]-zr[j7];
				t12i = zi[j6]-zi[j7];
				t13r = zr[j00]-0.5f*t6r;
				t13i = zi[j00]-0.5f*t6i;
				t14r = t1r-t6r;
				t14i = t1i-t6i;
				t15r = t2r-t6r;
				t15i = t2i-t6i;
				t16r = t3r-t6r;
				t16i = t3i-t6i;
				t17r = t4r-t6r;
				t17i = t4i-t6i;
				t18r = t5r-t6r;
				t18i = t5i-t6i;
				y1r = t13r+c1*t14r+c2*t15r+c3*t16r+c4*t17r+c5*t18r;
				y1i = t13i+c1*t14i+c2*t15i+c3*t16i+c4*t17i+c5*t18i;
				y2r = t13r+c2*t14r+c4*t15r+c6*t16r+c5*t17r+c3*t18r;
				y2i = t13i+c2*t14i+c4*t15i+c6*t16i+c5*t17i+c3*t18i;
				y3r = t13r+c3*t14r+c6*t15r+c4*t16r+c1*t17r+c2*t18r;
				y3i = t13i+c3*t14i+c6*t15i+c4*t16i+c1*t17i+c2*t18i;
				y4r = t13r+c4*t14r+c5*t15r+c1*t16r+c3*t17r+c6*t18r;
				y4i = t13i+c4*t14i+c5*t15i+c1*t16i+c3*t17i+c6*t18i;
				y5r = t13r+c5*t14r+c3*t15r+c2*t16r+c6*t17r+c1*t18r;
				y5i = t13i+c5*t14i+c3*t15i+c2*t16i+c6*t17i+c1*t18i;
				y6r = t13r+c6*t14r+c1*t15r+c5*t16r+c2*t17r+c4*t18r;
				y6i = t13i+c6*t14i+c1*t15i+c5*t16i+c2*t17i+c4*t18i;
				y7r = c12*t7r-c7*t8r+c11*t9r-c8*t10r+c10*t11r-c9*t12r;
				y7i = c12*t7i-c7*t8i+c11*t9i-c8*t10i+c10*t11i-c9*t12i;
				y8r = c11*t7r-c9*t8r+c8*t9r-c12*t10r-c7*t11r+c10*t12r;
				y8i = c11*t7i-c9*t8i+c8*t9i-c12*t10i-c7*t11i+c10*t12i;
				y9r = c10*t7r-c11*t8r-c7*t9r+c9*t10r-c12*t11r-c8*t12r;
				y9i = c10*t7i-c11*t8i-c7*t9i+c9*t10i-c12*t11i-c8*t12i;
				y10r = c9*t7r+c12*t8r-c10*t9r-c7*t10r+c8*t11r+c11*t12r;
				y10i = c9*t7i+c12*t8i-c10*t9i-c7*t10i+c8*t11i+c11*t12i;
				y11r = c8*t7r+c10*t8r+c12*t9r-c11*t10r-c9*t11r-c7*t12r;
				y11i = c8*t7i+c10*t8i+c12*t9i-c11*t10i-c9*t11i-c7*t12i;
				y12r = c7*t7r+c8*t8r+c9*t9r+c10*t10r+c11*t11r+c12*t12r;
				y12i = c7*t7i+c8*t8i+c9*t9i+c10*t10i+c11*t11i+c12*t12i;
				zr[j00] = zr[j00]+t1r+t2r+t3r+t4r+t5r+t6r;
				zi[j00] = zi[j00]+t1i+t2i+t3i+t4i+t5i+t6i;
				zr[j01] = y1r-y12i;
				zi[j01] = y1i+y12r;
				zr[j2] = y2r-y11i;
				zi[j2] = y2i+y11r;
				zr[j3] = y3r-y10i;
				zi[j3] = y3i+y10r;
				zr[j4] = y4r-y9i;
				zi[j4] = y4i+y9r;
				zr[j5] = y5r-y8i;
				zi[j5] = y5i+y8r;
				zr[j6] = y6r-y7i;
				zi[j6] = y6i+y7r;
				zr[j7] = y6r+y7i;
				zi[j7] = y6i-y7r;
				zr[j8] = y5r+y8i;
				zi[j8] = y5i-y8r;
				zr[j9] = y4r+y9i;
				zi[j9] = y4i-y9r;
				zr[j10] = y3r+y10i;
				zi[j10] = y3i-y10r;
				zr[j11] = y2r+y11i;
				zi[j11] = y2i-y11r;
				zr[j12] = y1r+y12i;
				zi[j12] = y1i-y12r;
				jt = j12+2;
				j12 = j11+2;
				j11 = j10+2;
				j10 = j9+2;
				j9 = j8+2;
				j8 = j7+2;
				j7 = j6+2;
				j6 = j5+2;
				j5 = j4+2;
				j4 = j3+2;
				j3 = j2+2;
				j2 = j01+2;
				j01 = j00+2;
				j00 = jt;
			}
			continue;
		}
		j13 = j12+jinc;
		if (j13>=jmax) j13 = j13-jmax;
		j14 = j13+jinc;
		if (j14>=jmax) j14 = j14-jmax;
		j15 = j14+jinc;
		if (j15>=jmax) j15 = j15-jmax;

		/* if factor is 16 */
		if (ifac==16) {
			if (mu==1) {
				c1 = 1.0;
				c2 = P923;
				c3 = P382;
				c4 = P707;
			} else if (mu==3) {
				c1 = -1.0;
				c2 = P382;
				c3 = P923;
				c4 = -P707;
			} else if (mu==5) {
				c1 = 1.0;
				c2 = -P382;
				c3 = P923;
				c4 = -P707;
			} else if (mu==7) {
				c1 = -1.0;
				c2 = -P923;
				c3 = P382;
				c4 = P707;
			} else if (mu==9) {
				c1 = 1.0;
				c2 = -P923;
				c3 = -P382;
				c4 = P707;
			} else if (mu==11) {
				c1 = -1.0;
				c2 = -P382;
				c3 = -P923;
				c4 = -P707;
			} else if (mu==13) {
				c1 = 1.0;
				c2 = P382;
				c3 = -P923;
				c4 = -P707;
			} else {
				c1 = -1.0;
				c2 = P923;
				c3 = -P382;
				c4 = P707;
			}
			c5 = c1*c4;
			c6 = c1*c3;
			c7 = c1*c2;
			for (l=0; l<m; l++) {
				t1r = zr[j00]+zr[j8];
				t1i = zi[j00]+zi[j8];
				t2r = zr[j4]+zr[j12];
				t2i = zi[j4]+zi[j12];
				t3r = zr[j00]-zr[j8];
				t3i = zi[j00]-zi[j8];
				t4r = c1*(zr[j4]-zr[j12]);
				t4i = c1*(zi[j4]-zi[j12]);
				t5r = t1r+t2r;
				t5i = t1i+t2i;
				t6r = t1r-t2r;
				t6i = t1i-t2i;
				t7r = zr[j01]+zr[j9];
				t7i = zi[j01]+zi[j9];
				t8r = zr[j5]+zr[j13];
				t8i = zi[j5]+zi[j13];
				t9r = zr[j01]-zr[j9];
				t9i = zi[j01]-zi[j9];
				t10r = zr[j5]-zr[j13];
				t10i = zi[j5]-zi[j13];
				t11r = t7r+t8r;
				t11i = t7i+t8i;
				t12r = t7r-t8r;
				t12i = t7i-t8i;
				t13r = zr[j2]+zr[j10];
				t13i = zi[j2]+zi[j10];
				t14r = zr[j6]+zr[j14];
				t14i = zi[j6]+zi[j14];
				t15r = zr[j2]-zr[j10];
				t15i = zi[j2]-zi[j10];
				t16r = zr[j6]-zr[j14];
				t16i = zi[j6]-zi[j14];
				t17r = t13r+t14r;
				t17i = t13i+t14i;
				t18r = c4*(t15r-t16r);
				t18i = c4*(t15i-t16i);
				t19r = c5*(t15r+t16r);
				t19i = c5*(t15i+t16i);
				t20r = c1*(t13r-t14r);
				t20i = c1*(t13i-t14i);
				t21r = zr[j3]+zr[j11];
				t21i = zi[j3]+zi[j11];
				t22r = zr[j7]+zr[j15];
				t22i = zi[j7]+zi[j15];
				t23r = zr[j3]-zr[j11];
				t23i = zi[j3]-zi[j11];
				t24r = zr[j7]-zr[j15];
				t24i = zi[j7]-zi[j15];
				t25r = t21r+t22r;
				t25i = t21i+t22i;
				t26r = t21r-t22r;
				t26i = t21i-t22i;
				t27r = t9r+t24r;
				t27i = t9i+t24i;
				t28r = t10r+t23r;
				t28i = t10i+t23i;
				t29r = t9r-t24r;
				t29i = t9i-t24i;
				t30r = t10r-t23r;
				t30i = t10i-t23i;
				t31r = t5r+t17r;
				t31i = t5i+t17i;
				t32r = t11r+t25r;
				t32i = t11i+t25i;
				t33r = t3r+t18r;
				t33i = t3i+t18i;
				t34r = c2*t29r-c6*t30r;
				t34i = c2*t29i-c6*t30i;
				t35r = t3r-t18r;
				t35i = t3i-t18i;
				t36r = c7*t27r-c3*t28r;
				t36i = c7*t27i-c3*t28i;
				t37r = t4r+t19r;
				t37i = t4i+t19i;
				t38r = c3*t27r+c7*t28r;
				t38i = c3*t27i+c7*t28i;
				t39r = t4r-t19r;
				t39i = t4i-t19i;
				t40r = c6*t29r+c2*t30r;
				t40i = c6*t29i+c2*t30i;
				t41r = c4*(t12r-t26r);
				t41i = c4*(t12i-t26i);
				t42r = c5*(t12r+t26r);
				t42i = c5*(t12i+t26i);
				y1r = t33r+t34r;
				y1i = t33i+t34i;
				y2r = t6r+t41r;
				y2i = t6i+t41i;
				y3r = t35r+t40r;
				y3i = t35i+t40i;
				y4r = t5r-t17r;
				y4i = t5i-t17i;
				y5r = t35r-t40r;
				y5i = t35i-t40i;
				y6r = t6r-t41r;
				y6i = t6i-t41i;
				y7r = t33r-t34r;
				y7i = t33i-t34i;
				y9r = t38r-t37r;
				y9i = t38i-t37i;
				y10r = t42r-t20r;
				y10i = t42i-t20i;
				y11r = t36r+t39r;
				y11i = t36i+t39i;
				y12r = c1*(t11r-t25r);
				y12i = c1*(t11i-t25i);
				y13r = t36r-t39r;
				y13i = t36i-t39i;
				y14r = t42r+t20r;
				y14i = t42i+t20i;
				y15r = t38r+t37r;
				y15i = t38i+t37i;
				zr[j00] = t31r+t32r;
				zi[j00] = t31i+t32i;
				zr[j01] = y1r-y15i;
				zi[j01] = y1i+y15r;
				zr[j2] = y2r-y14i;
				zi[j2] = y2i+y14r;
				zr[j3] = y3r-y13i;
				zi[j3] = y3i+y13r;
				zr[j4] = y4r-y12i;
				zi[j4] = y4i+y12r;
				zr[j5] = y5r-y11i;
				zi[j5] = y5i+y11r;
				zr[j6] = y6r-y10i;
				zi[j6] = y6i+y10r;
				zr[j7] = y7r-y9i;
				zi[j7] = y7i+y9r;
				zr[j8] = t31r-t32r;
				zi[j8] = t31i-t32i;
				zr[j9] = y7r+y9i;
				zi[j9] = y7i-y9r;
				zr[j10] = y6r+y10i;
				zi[j10] = y6i-y10r;
				zr[j11] = y5r+y11i;
				zi[j11] = y5i-y11r;
				zr[j12] = y4r+y12i;
				zi[j12] = y4i-y12r;
				zr[j13] = y3r+y13i;
				zi[j13] = y3i-y13r;
				zr[j14] = y2r+y14i;
				zi[j14] = y2i-y14r;
				zr[j15] = y1r+y15i;
				zi[j15] = y1i-y15r;
				jt = j15+2;
				j15 = j14+2;
				j14 = j13+2;
				j13 = j12+2;
				j12 = j11+2;
				j11 = j10+2;
				j10 = j9+2;
				j9 = j8+2;
				j8 = j7+2;
				j7 = j6+2;
				j6 = j5+2;
				j5 = j4+2;
				j4 = j3+2;
				j3 = j2+2;
				j2 = j01+2;
				j01 = j00+2;
				j00 = jt;
			}
			continue;
		}
	}
}

void pfacr (int isign, int n, complex cz[], float rz[])
/*****************************************************************************
Prime factor fft:  complex to real transform
******************************************************************************
Input:
isign       sign of isign is the sign of exponent in fourier kernel
n           length of transform (see notes below)
cz          array[n/2+1] of complex values (may be equivalenced to rz)

Output:
rz          array[n] of real values (may be equivalenced to cz)
******************************************************************************
Notes:
Because pfacr uses pfacc to do most of the work, n must be even 
and n/2 must be a valid length for pfacc.  The simplest way to
obtain a valid n is via n = npfar(nmin).  A more optimal n can be 
obtained with npfaro.
******************************************************************************
References:  
Inspired by, but differ significantly from Press et al, 1988, NR in C, p. 417.

Also, see notes and references for function pfacc.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/13/89
*****************************************************************************/
{
    int i,ir,ii,jr,ji,no2;
    float *z,tempr,tempi,sumr,sumi,difr,difi;
    double wr,wi,wpr,wpi,wtemp,theta;

    /* copy input to output and fix dc and nyquist */
    z = (float*)cz;
    for (i=2; i<n; i++)
        rz[i] = z[i];
    rz[1] = z[0]-z[n];
    rz[0] = z[0]+z[n];
    z = rz;

    /* initialize cosine-sine recurrence */
    theta = 2.0*PI/(double)n;
    if (isign>0) theta = -theta;
    wtemp = sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi = sin(theta);
    wr = 1.0+wpr;
    wi = wpi;

    /* twiddle */
    no2 = n/2;
    for (ir=2,ii=3,jr=n-2,ji=n-1; ir<=no2; ir+=2,ii+=2,jr-=2,ji-=2) {
        sumr = z[ir]+z[jr];
        sumi = z[ii]+z[ji];
        difr = z[ir]-z[jr];
        difi = z[ii]-z[ji];
		tempr = (float)(wi*difr - wr*sumi);
		tempi = (float)(wi*sumi + wr*difr);
        z[ir] = sumr+tempr;
        z[ii] = difi+tempi;
        z[jr] = sumr-tempr;
        z[ji] = tempi-difi;
        wtemp = wr;
        wr += wr*wpr-wi*wpi;
        wi += wi*wpr+wtemp*wpi;
    }

    /* do complex to complex transform */
    pfacc(isign,n/2,(complex*)z);
}

void pfarc (int isign, int n, float rz[], complex cz[])
/*****************************************************************************
Prime factor fft:  real to complex transform
******************************************************************************
Input:
isign       sign of isign is the sign of exponent in fourier kernel
n           length of transform; must be even (see notes below)
rz          array[n] of real values (may be equivalenced to cz)

Output:
cz          array[n/2+1] of complex values (may be equivalenced to rz)
******************************************************************************
Notes:
Because pfarc uses pfacc to do most of the work, n must be even 
and n/2 must be a valid length for pfacc.  The simplest way to
obtain a valid n is via n = npfar(nmin).  A more optimal n can be 
obtained with npfaro.
******************************************************************************
References:  
Inspired by, but differ significantly from Press et al, 1988, NR in C, p. 417.

Also, see notes and references for function pfacc.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/13/89
*****************************************************************************/
{
    int i,ir,ii,jr,ji,no2;
    float *z,tempr,tempi,sumr,sumi,difr,difi;
    double wr,wi,wpr,wpi,wtemp,theta;

    /* copy input to output while scaling */
    z = (float*)cz;
    for (i=0; i<n; i++)
        z[i] = 0.5f*rz[i];

    /* do complex to complex transform */
    pfacc(isign,n/2,cz);

    /* fix dc and nyquist */
    z[n] = 2.0f*(z[0]-z[1]);
    z[0] = 2.0f*(z[0]+z[1]);
    z[n+1] = 0.0f;
    z[1] = 0.0f;

    /* initialize cosine-sine recurrence */
    theta = 2.0*PI/(double)n;
    if (isign<0) theta = -theta;
    wtemp = sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi = sin(theta);
    wr = 1.0+wpr;
    wi = wpi;

    /* twiddle */
    no2 = n/2;
    for (ir=2,ii=3,jr=n-2,ji=n-1; ir<=no2; ir+=2,ii+=2,jr-=2,ji-=2) {
        sumr = z[ir]+z[jr];
        sumi = z[ii]+z[ji];
        difr = z[ir]-z[jr];
        difi = z[ii]-z[ji];
		tempr = (float)(wi*difr + wr*sumi);
		tempi = (float)(wi*sumi - wr*difr);
        z[ir] = sumr+tempr;
        z[ii] = difi+tempi;
        z[jr] = sumr-tempr;
        z[ji] = tempi-difi;
        wtemp = wr;
        wr += wr*wpr-wi*wpi;
        wi += wi*wpr+wtemp*wpi;
    }
}

void pfamcc (int isign, int n, int nt, int k, int kt, complex cz[])
/*****************************************************************************
Prime factor fft:  multiple complex to complex transforms, in place
******************************************************************************
Input:
isign       	sign of isign is the sign of exponent in fourier kernel
n           	number of complex elements per transform (see notes below)
nt          	number of transforms
k           	stride in complex elements within transforms
kt          	stride in complex elements between transforms
z           	array of complex elements to be transformed in place

Output:
z		array of complex elements transformed
******************************************************************************
Notes:
n must be factorable into mutually prime factors taken 
from the set {2,3,4,5,7,8,9,11,13,16}.  in other words,
    n = 2**p * 3**q * 5**r * 7**s * 11**t * 13**u
where
    0 <= p <= 4,  0 <= q <= 2,  0 <= r,s,t,u <= 1
is required for pfamcc to yield meaningful results.  this
restriction implies that n is restricted to the range
    1 <= n <= 720720 (= 5*7*9*11*13*16)

To perform a two-dimensional transform of an n1 by n2 complex array 
(assuming that both n1 and n2 are valid "n"), stored with n1 fast 
and n2 slow:
    pfamcc(isign,n1,n2,1,n1,z); (to transform 1st dimension)
    pfamcc(isign,n2,n1,n1,1,z); (to transform 2nd dimension)
******************************************************************************
References:  
Temperton, C., 1985, Implementation of a self-sorting
in-place prime factor fft algorithm:  Journal of
Computational Physics, v. 58, p. 283-299.

Temperton, C., 1988, A new set of minimum-add rotated
rotated dft modules: Journal of Computational Physics,
v. 75, p. 190-198.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/15/89
*****************************************************************************/
{
    static int kfax[] = { 16,13,11,9,8,7,5,4,3,2 };
    register float *zr=(float*)cz;	/* Real */
    register float *zi=zr+1;		/* Imag */
    register int j00,j01,j2,j3,j4,j5,j6,j7,j8,j9,j10,j11,j12,j13,j14,j15;
    int nleft,jfax,ifac,jfac,iinc,imax,ndiv,m,mm=0,mu=0,l,istep,jstep,
        jt,i0,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,i15,it;
    float t1r,t1i,t2r,t2i,t3r,t3i,t4r,t4i,t5r,t5i,
        t6r,t6i,t7r,t7i,t8r,t8i,t9r,t9i,t10r,t10i,
        t11r,t11i,t12r,t12i,t13r,t13i,t14r,t14i,t15r,t15i,
        t16r,t16i,t17r,t17i,t18r,t18i,t19r,t19i,t20r,t20i,
        t21r,t21i,t22r,t22i,t23r,t23i,t24r,t24i,t25r,t25i,
        t26r,t26i,t27r,t27i,t28r,t28i,t29r,t29i,t30r,t30i,
        t31r,t31i,t32r,t32i,t33r,t33i,t34r,t34i,t35r,t35i,
        t36r,t36i,t37r,t37i,t38r,t38i,t39r,t39i,t40r,t40i,
        t41r,t41i,t42r,t42i,
        y1r,y1i,y2r,y2i,y3r,y3i,y4r,y4i,y5r,y5i,
        y6r,y6i,y7r,y7i,y8r,y8i,y9r,y9i,y10r,y10i,
        y11r,y11i,y12r,y12i,y13r,y13i,y14r,y14i,y15r,y15i,
        c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12;

    /* determine step within and between transforms */
    istep = 2*k;
    jstep = 2*kt;

    /* keep track of n left after dividing by factors */
    nleft = n;

    /* begin loop over possible factors (from biggest to smallest) */
    for (jfax=0; jfax<NFAX; jfax++) {

        /* skip if not a mutually prime factor of n */
        ifac = kfax[jfax];
        ndiv = nleft/ifac;
        if (ndiv*ifac!=nleft) continue;
 
        /* update n left and determine n divided by factor */
        nleft = ndiv;
        m = n/ifac;
 
        /* determine rotation factor mu and stride mm */
        for (jfac=1; jfac<=ifac; jfac++) {
            mu = jfac;
            mm = jfac*m;
            if (mm%ifac==1) break;
        }
 
        /* adjust rotation factor for sign of transform */
        if (isign<0) mu = ifac-mu;
 
        /* compute stride, limit, and pointers */
        iinc = istep*mm;
        imax = istep*n;
        i0 = 0;
        i1 = i0+iinc;

        /* if factor is 2 */
        if (ifac==2) {
            for (l=0; l<m; l++) {
                j00 = i0;
                j01 = i1;
                for (jt=0; jt<nt; jt++) {
                    t1r = zr[j00]-zr[j01];
                    t1i = zi[j00]-zi[j01];
                    zr[j00] = zr[j00]+zr[j01];
                    zi[j00] = zi[j00]+zi[j01];
                    zr[j01] = t1r;
                    zi[j01] = t1i;
                    j00 += jstep;
                    j01 += jstep;
                }
                it = i1+istep;
                i1 = i0+istep;
                i0 = it;
            }
            continue;
        }
        i2 = i1+iinc;
        if (i2>=imax) i2 = i2-imax;

        /* if factor is 3 */
        if (ifac==3) {
            if (mu==1)
                c1 = P866;
            else
                c1 = -P866;
            for (l=0; l<m; l++) {
                j00 = i0;
                j01 = i1;
                j2 = i2;
                for (jt=0; jt<nt; jt++) {
                    t1r = zr[j01]+zr[j2];
                    t1i = zi[j01]+zi[j2];
                    y1r = zr[j00]-0.5f*t1r;
                    y1i = zi[j00]-0.5f*t1i;
                    y2r = c1*(zr[j01]-zr[j2]);
                    y2i = c1*(zi[j01]-zi[j2]);
                    zr[j00] = zr[j00]+t1r;
                    zi[j00] = zi[j00]+t1i;
                    zr[j01] = y1r-y2i;
                    zi[j01] = y1i+y2r;
                    zr[j2] = y1r+y2i;
                    zi[j2] = y1i-y2r;
                    j00 += jstep;
                    j01 += jstep;
                    j2 += jstep;
                }
                it = i2+istep;
                i2 = i1+istep;
                i1 = i0+istep;
                i0 = it;
            }
            continue;
        }
        i3 = i2+iinc;
        if (i3>=imax) i3 = i3-imax;

        /* if factor is 4 */
        if (ifac==4) {
            if (mu==1)
                c1 = 1.0;
            else
                c1 = -1.0;
            for (l=0; l<m; l++) {
                j00 = i0;
                j01 = i1;
                j2 = i2;
                j3 = i3;
                for (jt=0; jt<nt; jt++) {
                    t1r = zr[j00]+zr[j2];
                    t1i = zi[j00]+zi[j2];
                    t2r = zr[j01]+zr[j3];
                    t2i = zi[j01]+zi[j3];
                    y1r = zr[j00]-zr[j2];
                    y1i = zi[j00]-zi[j2];
                    y3r = c1*(zr[j01]-zr[j3]);
                    y3i = c1*(zi[j01]-zi[j3]);
                    zr[j00] = t1r+t2r;
                    zi[j00] = t1i+t2i;
                    zr[j01] = y1r-y3i;
                    zi[j01] = y1i+y3r;
                    zr[j2] = t1r-t2r;
                    zi[j2] = t1i-t2i;
                    zr[j3] = y1r+y3i;
                    zi[j3] = y1i-y3r;
                    j00 += jstep;
                    j01 += jstep;
                    j2 += jstep;
                    j3 += jstep;
                }
                it = i3+istep;
                i3 = i2+istep;
                i2 = i1+istep;
                i1 = i0+istep;
                i0 = it;
            }
            continue;
        }
        i4 = i3+iinc;
        if (i4>=imax) i4 = i4-imax;

        /* if factor is 5 */
        if (ifac==5) {
            if (mu==1) {
                c1 = P559;
                c2 = P951;
                c3 = P587;
            } else if (mu==2) {
                c1 = -P559;
                c2 = P587;
                c3 = -P951;
            } else if (mu==3) {
                c1 = -P559;
                c2 = -P587;
                c3 = P951;
            } else { 
                c1 = P559;
                c2 = -P951;
                c3 = -P587;
            }
            for (l=0; l<m; l++) {
                j00 = i0;
                j01 = i1;
                j2 = i2;
                j3 = i3;
                j4 = i4;
                for (jt=0; jt<nt; jt++) {
                    t1r = zr[j01]+zr[j4];
                    t1i = zi[j01]+zi[j4];
                    t2r = zr[j2]+zr[j3];
                    t2i = zi[j2]+zi[j3];
                    t3r = zr[j01]-zr[j4];
                    t3i = zi[j01]-zi[j4];
                    t4r = zr[j2]-zr[j3];
                    t4i = zi[j2]-zi[j3];
                    t5r = t1r+t2r;
                    t5i = t1i+t2i;
                    t6r = c1*(t1r-t2r);
                    t6i = c1*(t1i-t2i);
                    t7r = zr[j00]-0.25f*t5r;
                    t7i = zi[j00]-0.25f*t5i;
                    y1r = t7r+t6r;
                    y1i = t7i+t6i;
                    y2r = t7r-t6r;
                    y2i = t7i-t6i;
                    y3r = c3*t3r-c2*t4r;
                    y3i = c3*t3i-c2*t4i;
                    y4r = c2*t3r+c3*t4r;
                    y4i = c2*t3i+c3*t4i;
                    zr[j00] = zr[j00]+t5r;
                    zi[j00] = zi[j00]+t5i;
                    zr[j01] = y1r-y4i;
                    zi[j01] = y1i+y4r;
                    zr[j2] = y2r-y3i;
                    zi[j2] = y2i+y3r;
                    zr[j3] = y2r+y3i;
                    zi[j3] = y2i-y3r;
                    zr[j4] = y1r+y4i;
                    zi[j4] = y1i-y4r;
                    j00 += jstep;
                    j01 += jstep;
                    j2 += jstep;
                    j3 += jstep;
                    j4 += jstep;
                }
                it = i4+istep;
                i4 = i3+istep;
                i3 = i2+istep;
                i2 = i1+istep;
                i1 = i0+istep;
                i0 = it;
            }
            continue;
        }
        i5 = i4+iinc;
        if (i5>=imax) i5 = i5-imax;
        i6 = i5+iinc;
        if (i6>=imax) i6 = i6-imax;

        /* if factor is 7 */
        if (ifac==7) {
            if (mu==1) {
                c1 = P623;
                c2 = -P222;
                c3 = -P900;
                c4 = P781;
                c5 = P974;
                c6 = P433;
            } else if (mu==2) {
                c1 = -P222;
                c2 = -P900;
                c3 = P623;
                c4 = P974;
                c5 = -P433;
                c6 = -P781;
            } else if (mu==3) {
                c1 = -P900;
                c2 = P623;
                c3 = -P222;
                c4 = P433;
                c5 = -P781;
                c6 = P974;
            } else if (mu==4) {
                c1 = -P900;
                c2 = P623;
                c3 = -P222;
                c4 = -P433;
                c5 = P781;
                c6 = -P974;
            } else if (mu==5) {
                c1 = -P222;
                c2 = -P900;
                c3 = P623;
                c4 = -P974;
                c5 = P433;
                c6 = P781;
            } else {
                c1 = P623;
                c2 = -P222;
                c3 = -P900;
                c4 = -P781;
                c5 = -P974;
                c6 = -P433;
            }
            for (l=0; l<m; l++) {
                j00 = i0;
                j01 = i1;
                j2 = i2;
                j3 = i3;
                j4 = i4;
                j5 = i5;
                j6 = i6;
                for (jt=0; jt<nt; jt++) {
                    t1r = zr[j01]+zr[j6];
                    t1i = zi[j01]+zi[j6];
                    t2r = zr[j2]+zr[j5];
                    t2i = zi[j2]+zi[j5];
                    t3r = zr[j3]+zr[j4];
                    t3i = zi[j3]+zi[j4];
                    t4r = zr[j01]-zr[j6];
                    t4i = zi[j01]-zi[j6];
                    t5r = zr[j2]-zr[j5];
                    t5i = zi[j2]-zi[j5];
                    t6r = zr[j3]-zr[j4];
                    t6i = zi[j3]-zi[j4];
                    t7r = zr[j00]-0.5f*t3r;
                    t7i = zi[j00]-0.5f*t3i;
                    t8r = t1r-t3r;
                    t8i = t1i-t3i;
                    t9r = t2r-t3r;
                    t9i = t2i-t3i;
                    y1r = t7r+c1*t8r+c2*t9r;
                    y1i = t7i+c1*t8i+c2*t9i;
                    y2r = t7r+c2*t8r+c3*t9r;
                    y2i = t7i+c2*t8i+c3*t9i;
                    y3r = t7r+c3*t8r+c1*t9r;
                    y3i = t7i+c3*t8i+c1*t9i;
                    y4r = c6*t4r-c4*t5r+c5*t6r;
                    y4i = c6*t4i-c4*t5i+c5*t6i;
                    y5r = c5*t4r-c6*t5r-c4*t6r;
                    y5i = c5*t4i-c6*t5i-c4*t6i;
                    y6r = c4*t4r+c5*t5r+c6*t6r;
                    y6i = c4*t4i+c5*t5i+c6*t6i;
                    zr[j00] = zr[j00]+t1r+t2r+t3r;
                    zi[j00] = zi[j00]+t1i+t2i+t3i;
                    zr[j01] = y1r-y6i;
                    zi[j01] = y1i+y6r;
                    zr[j2] = y2r-y5i;
                    zi[j2] = y2i+y5r;
                    zr[j3] = y3r-y4i;
                    zi[j3] = y3i+y4r;
                    zr[j4] = y3r+y4i;
                    zi[j4] = y3i-y4r;
                    zr[j5] = y2r+y5i;
                    zi[j5] = y2i-y5r;
                    zr[j6] = y1r+y6i;
                    zi[j6] = y1i-y6r;
                    j00 += jstep;
                    j01 += jstep;
                    j2 += jstep;
                    j3 += jstep;
                    j4 += jstep;
                    j5 += jstep;
                    j6 += jstep;
                }
                it = i6+istep;
                i6 = i5+istep;
                i5 = i4+istep;
                i4 = i3+istep;
                i3 = i2+istep;
                i2 = i1+istep;
                i1 = i0+istep;
                i0 = it;
            }
            continue;
        }
        i7 = i6+iinc;
        if (i7>=imax) i7 = i7-imax;

        /* if factor is 8 */
        if (ifac==8) {
            if (mu==1) {
                c1 = 1.0;
                c2 = P707;
            } else if (mu==3) {
                c1 = -1.0;
                c2 = -P707;
            } else if (mu==5) {
                c1 = 1.0;
                c2 = -P707;
            } else {
                c1 = -1.0;
                c2 = P707;
            }
            c3 = c1*c2;
            for (l=0; l<m; l++) {
                j00 = i0;
                j01 = i1;
                j2 = i2;
                j3 = i3;
                j4 = i4;
                j5 = i5;
                j6 = i6;
                j7 = i7;
                for (jt=0; jt<nt; jt++) {
                    t1r = zr[j00]+zr[j4];
                    t1i = zi[j00]+zi[j4];
                    t2r = zr[j00]-zr[j4];
                    t2i = zi[j00]-zi[j4];
                    t3r = zr[j01]+zr[j5];
                    t3i = zi[j01]+zi[j5];
                    t4r = zr[j01]-zr[j5];
                    t4i = zi[j01]-zi[j5];
                    t5r = zr[j2]+zr[j6];
                    t5i = zi[j2]+zi[j6];
                    t6r = c1*(zr[j2]-zr[j6]);
                    t6i = c1*(zi[j2]-zi[j6]);
                    t7r = zr[j3]+zr[j7];
                    t7i = zi[j3]+zi[j7];
                    t8r = zr[j3]-zr[j7];
                    t8i = zi[j3]-zi[j7];
                    t9r = t1r+t5r;
                    t9i = t1i+t5i;
                    t10r = t3r+t7r;
                    t10i = t3i+t7i;
                    t11r = c2*(t4r-t8r);
                    t11i = c2*(t4i-t8i);
                    t12r = c3*(t4r+t8r);
                    t12i = c3*(t4i+t8i);
                    y1r = t2r+t11r;
                    y1i = t2i+t11i;
                    y2r = t1r-t5r;
                    y2i = t1i-t5i;
                    y3r = t2r-t11r;
                    y3i = t2i-t11i;
                    y5r = t12r-t6r;
                    y5i = t12i-t6i;
                    y6r = c1*(t3r-t7r);
                    y6i = c1*(t3i-t7i);
                    y7r = t12r+t6r;
                    y7i = t12i+t6i;
                    zr[j00] = t9r+t10r;
                    zi[j00] = t9i+t10i;
                    zr[j01] = y1r-y7i;
                    zi[j01] = y1i+y7r;
                    zr[j2] = y2r-y6i;
                    zi[j2] = y2i+y6r;
                    zr[j3] = y3r-y5i;
                    zi[j3] = y3i+y5r;
                    zr[j4] = t9r-t10r;
                    zi[j4] = t9i-t10i;
                    zr[j5] = y3r+y5i;
                    zi[j5] = y3i-y5r;
                    zr[j6] = y2r+y6i;
                    zi[j6] = y2i-y6r;
                    zr[j7] = y1r+y7i;
                    zi[j7] = y1i-y7r;
                    j00 += jstep;
                    j01 += jstep;
                    j2 += jstep;
                    j3 += jstep;
                    j4 += jstep;
                    j5 += jstep;
                    j6 += jstep;
                    j7 += jstep;
                }
                it = i7+istep;
                i7 = i6+istep;
                i6 = i5+istep;
                i5 = i4+istep;
                i4 = i3+istep;
                i3 = i2+istep;
                i2 = i1+istep;
                i1 = i0+istep;
                i0 = it;
            }
            continue;
        }
        i8 = i7+iinc;
        if (i8>=imax) i8 = i8-imax;

        /* if factor is 9 */
        if (ifac==9) {
            if (mu==1) {
                c1 = P866;
                c2 = P766;
                c3 = P642;
                c4 = P173;
                c5 = P984;
            } else if (mu==2) {
                c1 = -P866;
                c2 = P173;
                c3 = P984;
                c4 = -P939;
                c5 = P342;
            } else if (mu==4) {
                c1 = P866;
                c2 = -P939;
                c3 = P342;
                c4 = P766;
                c5 = -P642;
            } else if (mu==5) {
                c1 = -P866;
                c2 = -P939;
                c3 = -P342;
                c4 = P766;
                c5 = P642;
            } else if (mu==7) {
                c1 = P866;
                c2 = P173;
                c3 = -P984;
                c4 = -P939;
                c5 = -P342;
            } else {
                c1 = -P866;
                c2 = P766;
                c3 = -P642;
                c4 = P173;
                c5 = -P984;
            }
            c6 = c1*c2;
            c7 = c1*c3;
            c8 = c1*c4;
            c9 = c1*c5;
            for (l=0; l<m; l++) {
                j00 = i0;
                j01 = i1;
                j2 = i2;
                j3 = i3;
                j4 = i4;
                j5 = i5;
                j6 = i6;
                j7 = i7;
                j8 = i8;
                for (jt=0; jt<nt; jt++) {
                    t1r = zr[j3]+zr[j6];
                    t1i = zi[j3]+zi[j6];
                    t2r = zr[j00]-0.5f*t1r;
                    t2i = zi[j00]-0.5f*t1i;
                    t3r = c1*(zr[j3]-zr[j6]);
                    t3i = c1*(zi[j3]-zi[j6]);
                    t4r = zr[j00]+t1r;
                    t4i = zi[j00]+t1i;
                    t5r = zr[j4]+zr[j7];
                    t5i = zi[j4]+zi[j7];
                    t6r = zr[j01]-0.5f*t5r;
                    t6i = zi[j01]-0.5f*t5i;
                    t7r = zr[j4]-zr[j7];
                    t7i = zi[j4]-zi[j7];
                    t8r = zr[j01]+t5r;
                    t8i = zi[j01]+t5i;
                    t9r = zr[j2]+zr[j5];
                    t9i = zi[j2]+zi[j5];
                    t10r = zr[j8]-0.5f*t9r;
                    t10i = zi[j8]-0.5f*t9i;
                    t11r = zr[j2]-zr[j5];
                    t11i = zi[j2]-zi[j5];
                    t12r = zr[j8]+t9r;
                    t12i = zi[j8]+t9i;
                    t13r = t8r+t12r;
                    t13i = t8i+t12i;
                    t14r = t6r+t10r;
                    t14i = t6i+t10i;
                    t15r = t6r-t10r;
                    t15i = t6i-t10i;
                    t16r = t7r+t11r;
                    t16i = t7i+t11i;
                    t17r = t7r-t11r;
                    t17i = t7i-t11i;
                    t18r = c2*t14r-c7*t17r;
                    t18i = c2*t14i-c7*t17i;
                    t19r = c4*t14r+c9*t17r;
                    t19i = c4*t14i+c9*t17i;
                    t20r = c3*t15r+c6*t16r;
                    t20i = c3*t15i+c6*t16i;
                    t21r = c5*t15r-c8*t16r;
                    t21i = c5*t15i-c8*t16i;
                    t22r = t18r+t19r;
                    t22i = t18i+t19i;
                    t23r = t20r-t21r;
                    t23i = t20i-t21i;
                    y1r = t2r+t18r;
                    y1i = t2i+t18i;
                    y2r = t2r+t19r;
                    y2i = t2i+t19i;
                    y3r = t4r-0.5f*t13r;
                    y3i = t4i-0.5f*t13i;
                    y4r = t2r-t22r;
                    y4i = t2i-t22i;
                    y5r = t3r-t23r;
                    y5i = t3i-t23i;
                    y6r = c1*(t8r-t12r);
                    y6i = c1*(t8i-t12i);
                    y7r = t21r-t3r;
                    y7i = t21i-t3i;
                    y8r = t3r+t20r;
                    y8i = t3i+t20i;
                    zr[j00] = t4r+t13r;
                    zi[j00] = t4i+t13i;
                    zr[j01] = y1r-y8i;
                    zi[j01] = y1i+y8r;
                    zr[j2] = y2r-y7i;
                    zi[j2] = y2i+y7r;
                    zr[j3] = y3r-y6i;
                    zi[j3] = y3i+y6r;
                    zr[j4] = y4r-y5i;
                    zi[j4] = y4i+y5r;
                    zr[j5] = y4r+y5i;
                    zi[j5] = y4i-y5r;
                    zr[j6] = y3r+y6i;
                    zi[j6] = y3i-y6r;
                    zr[j7] = y2r+y7i;
                    zi[j7] = y2i-y7r;
                    zr[j8] = y1r+y8i;
                    zi[j8] = y1i-y8r;
                    j00 += jstep;
                    j01 += jstep;
                    j2 += jstep;
                    j3 += jstep;
                    j4 += jstep;
                    j5 += jstep;
                    j6 += jstep;
                    j7 += jstep;
                    j8 += jstep;
                }
                it = i8+istep;
                i8 = i7+istep;
                i7 = i6+istep;
                i6 = i5+istep;
                i5 = i4+istep;
                i4 = i3+istep;
                i3 = i2+istep;
                i2 = i1+istep;
                i1 = i0+istep;
                i0 = it;
            }
            continue;
        }
        i9 = i8+iinc;
        if (i9>=imax) i9 = i9-imax;
        i10 = i9+iinc;
        if (i10>=imax) i10 = i10-imax;

        /* if factor is 11 */
        if (ifac==11) {
            if (mu==1) {
                c1 = P841;
                c2 = P415;
                c3 = -P142;
                c4 = -P654;
                c5 = -P959;
                c6 = P540;
                c7 = P909;
                c8 = P989;
                c9 = P755;
                c10 = P281;
            } else if (mu==2) {
                c1 = P415;
                c2 = -P654;
                c3 = -P959;
                c4 = -P142;
                c5 = P841;
                c6 = P909;
                c7 = P755;
                c8 = -P281;
                c9 = -P989;
                c10 = -P540;
            } else if (mu==3) {
                c1 = -P142;
                c2 = -P959;
                c3 = P415;
                c4 = P841;
                c5 = -P654;
                c6 = P989;
                c7 = -P281;
                c8 = -P909;
                c9 = P540;
                c10 = P755;
            } else if (mu==4) {
                c1 = -P654;
                c2 = -P142;
                c3 = P841;
                c4 = -P959;
                c5 = P415;
                c6 = P755;
                c7 = -P989;
                c8 = P540;
                c9 = P281;
                c10 = -P909;
            } else if (mu==5) {
                c1 = -P959;
                c2 = P841;
                c3 = -P654;
                c4 = P415;
                c5 = -P142;
                c6 = P281;
                c7 = -P540;
                c8 = P755;
                c9 = -P909;
                c10 = P989;
            } else if (mu==6) {
                c1 = -P959;
                c2 = P841;
                c3 = -P654;
                c4 = P415;
                c5 = -P142;
                c6 = -P281;
                c7 = P540;
                c8 = -P755;
                c9 = P909;
                c10 = -P989;
            } else if (mu==7) {
                c1 = -P654;
                c2 = -P142;
                c3 = P841;
                c4 = -P959;
                c5 = P415;
                c6 = -P755;
                c7 = P989;
                c8 = -P540;
                c9 = -P281;
                c10 = P909;
            } else if (mu==8) {
                c1 = -P142;
                c2 = -P959;
                c3 = P415;
                c4 = P841;
                c5 = -P654;
                c6 = -P989;
                c7 = P281;
                c8 = P909;
                c9 = -P540;
                c10 = -P755;
            } else if (mu==9) {
                c1 = P415;
                c2 = -P654;
                c3 = -P959;
                c4 = -P142;
                c5 = P841;
                c6 = -P909;
                c7 = -P755;
                c8 = P281;
                c9 = P989;
                c10 = P540;
            } else {
                c1 = P841;
                c2 = P415;
                c3 = -P142;
                c4 = -P654;
                c5 = -P959;
                c6 = -P540;
                c7 = -P909;
                c8 = -P989;
                c9 = -P755;
                c10 = -P281;
            }
            for (l=0; l<m; l++) {
                j00 = i0;
                j01 = i1;
                j2 = i2;
                j3 = i3;
                j4 = i4;
                j5 = i5;
                j6 = i6;
                j7 = i7;
                j8 = i8;
                j9 = i9;
                j10 = i10;
                for (jt=0; jt<nt; jt++) {
                    t1r = zr[j01]+zr[j10];
                    t1i = zi[j01]+zi[j10];
                    t2r = zr[j2]+zr[j9];
                    t2i = zi[j2]+zi[j9];
                    t3r = zr[j3]+zr[j8];
                    t3i = zi[j3]+zi[j8];
                    t4r = zr[j4]+zr[j7];
                    t4i = zi[j4]+zi[j7];
                    t5r = zr[j5]+zr[j6];
                    t5i = zi[j5]+zi[j6];
                    t6r = zr[j01]-zr[j10];
                    t6i = zi[j01]-zi[j10];
                    t7r = zr[j2]-zr[j9];
                    t7i = zi[j2]-zi[j9];
                    t8r = zr[j3]-zr[j8];
                    t8i = zi[j3]-zi[j8];
                    t9r = zr[j4]-zr[j7];
                    t9i = zi[j4]-zi[j7];
                    t10r = zr[j5]-zr[j6];
                    t10i = zi[j5]-zi[j6];
                    t11r = zr[j00]-0.5f*t5r;
                    t11i = zi[j00]-0.5f*t5i;
                    t12r = t1r-t5r;
                    t12i = t1i-t5i;
                    t13r = t2r-t5r;
                    t13i = t2i-t5i;
                    t14r = t3r-t5r;
                    t14i = t3i-t5i;
                    t15r = t4r-t5r;
                    t15i = t4i-t5i;
                    y1r = t11r+c1*t12r+c2*t13r+c3*t14r+c4*t15r;
                    y1i = t11i+c1*t12i+c2*t13i+c3*t14i+c4*t15i;
                    y2r = t11r+c2*t12r+c4*t13r+c5*t14r+c3*t15r;
                    y2i = t11i+c2*t12i+c4*t13i+c5*t14i+c3*t15i;
                    y3r = t11r+c3*t12r+c5*t13r+c2*t14r+c1*t15r;
                    y3i = t11i+c3*t12i+c5*t13i+c2*t14i+c1*t15i;
                    y4r = t11r+c4*t12r+c3*t13r+c1*t14r+c5*t15r;
                    y4i = t11i+c4*t12i+c3*t13i+c1*t14i+c5*t15i;
                    y5r = t11r+c5*t12r+c1*t13r+c4*t14r+c2*t15r;
                    y5i = t11i+c5*t12i+c1*t13i+c4*t14i+c2*t15i;
                    y6r = c10*t6r-c6*t7r+c9*t8r-c7*t9r+c8*t10r;
                    y6i = c10*t6i-c6*t7i+c9*t8i-c7*t9i+c8*t10i;
                    y7r = c9*t6r-c8*t7r+c6*t8r+c10*t9r-c7*t10r;
                    y7i = c9*t6i-c8*t7i+c6*t8i+c10*t9i-c7*t10i;
                    y8r = c8*t6r-c10*t7r-c7*t8r+c6*t9r+c9*t10r;
                    y8i = c8*t6i-c10*t7i-c7*t8i+c6*t9i+c9*t10i;
                    y9r = c7*t6r+c9*t7r-c10*t8r-c8*t9r-c6*t10r;
                    y9i = c7*t6i+c9*t7i-c10*t8i-c8*t9i-c6*t10i;
                    y10r = c6*t6r+c7*t7r+c8*t8r+c9*t9r+c10*t10r;
                    y10i = c6*t6i+c7*t7i+c8*t8i+c9*t9i+c10*t10i;
                    zr[j00] = zr[j00]+t1r+t2r+t3r+t4r+t5r;
                    zi[j00] = zi[j00]+t1i+t2i+t3i+t4i+t5i;
                    zr[j01] = y1r-y10i;
                    zi[j01] = y1i+y10r;
                    zr[j2] = y2r-y9i;
                    zi[j2] = y2i+y9r;
                    zr[j3] = y3r-y8i;
                    zi[j3] = y3i+y8r;
                    zr[j4] = y4r-y7i;
                    zi[j4] = y4i+y7r;
                    zr[j5] = y5r-y6i;
                    zi[j5] = y5i+y6r;
                    zr[j6] = y5r+y6i;
                    zi[j6] = y5i-y6r;
                    zr[j7] = y4r+y7i;
                    zi[j7] = y4i-y7r;
                    zr[j8] = y3r+y8i;
                    zi[j8] = y3i-y8r;
                    zr[j9] = y2r+y9i;
                    zi[j9] = y2i-y9r;
                    zr[j10] = y1r+y10i;
                    zi[j10] = y1i-y10r;
                    j00 += jstep;
                    j01 += jstep;
                    j2 += jstep;
                    j3 += jstep;
                    j4 += jstep;
                    j5 += jstep;
                    j6 += jstep;
                    j7 += jstep;
                    j8 += jstep;
                    j9 += jstep;
                    j10 += jstep;
                }
                it = i10+istep;
                i10 = i9+istep;
                i9 = i8+istep;
                i8 = i7+istep;
                i7 = i6+istep;
                i6 = i5+istep;
                i5 = i4+istep;
                i4 = i3+istep;
                i3 = i2+istep;
                i2 = i1+istep;
                i1 = i0+istep;
                i0 = it;
            }
            continue;
        }
        i11 = i10+iinc;
        if (i11>=imax) i11 = i11-imax;
        i12 = i11+iinc;
        if (i12>=imax) i12 = i12-imax;

        /* if factor is 13 */
        if (ifac==13) {
            if (mu==1) {
                c1 = P885;
                c2 = P568;
                c3 = P120;
                c4 = -P354;
                c5 = -P748;
                c6 = -P970;
                c7 = P464;
                c8 = P822;
                c9 = P992;
                c10 = P935;
                c11 = P663;
                c12 = P239;
            } else if (mu==2) {
                c1 = P568;
                c2 = -P354;
                c3 = -P970;
                c4 = -P748;
                c5 = P120;
                c6 = P885;
                c7 = P822;
                c8 = P935;
                c9 = P239;
                c10 = -P663;
                c11 = -P992;
                c12 = -P464;
            } else if (mu==3) {
                c1 = P120;
                c2 = -P970;
                c3 = -P354;
                c4 = P885;
                c5 = P568;
                c6 = -P748;
                c7 = P992;
                c8 = P239;
                c9 = -P935;
                c10 = -P464;
                c11 = P822;
                c12 = P663;
            } else if (mu==4) {
                c1 = -P354;
                c2 = -P748;
                c3 = P885;
                c4 = P120;
                c5 = -P970;
                c6 = P568;
                c7 = P935;
                c8 = -P663;
                c9 = -P464;
                c10 = P992;
                c11 = -P239;
                c12 = -P822;
            } else if (mu==5) {
                c1 = -P748;
                c2 = P120;
                c3 = P568;
                c4 = -P970;
                c5 = P885;
                c6 = -P354;
                c7 = P663;
                c8 = -P992;
                c9 = P822;
                c10 = -P239;
                c11 = -P464;
                c12 = P935;
            } else if (mu==6) {
                c1 = -P970;
                c2 = P885;
                c3 = -P748;
                c4 = P568;
                c5 = -P354;
                c6 = P120;
                c7 = P239;
                c8 = -P464;
                c9 = P663;
                c10 = -P822;
                c11 = P935;
                c12 = -P992;
            } else if (mu==7) {
                c1 = -P970;
                c2 = P885;
                c3 = -P748;
                c4 = P568;
                c5 = -P354;
                c6 = P120;
                c7 = -P239;
                c8 = P464;
                c9 = -P663;
                c10 = P822;
                c11 = -P935;
                c12 = P992;
            } else if (mu==8) {
                c1 = -P748;
                c2 = P120;
                c3 = P568;
                c4 = -P970;
                c5 = P885;
                c6 = -P354;
                c7 = -P663;
                c8 = P992;
                c9 = -P822;
                c10 = P239;
                c11 = P464;
                c12 = -P935;
            } else if (mu==9) {
                c1 = -P354;
                c2 = -P748;
                c3 = P885;
                c4 = P120;
                c5 = -P970;
                c6 = P568;
                c7 = -P935;
                c8 = P663;
                c9 = P464;
                c10 = -P992;
                c11 = P239;
                c12 = P822;
            } else if (mu==10) {
                c1 = P120;
                c2 = -P970;
                c3 = -P354;
                c4 = P885;
                c5 = P568;
                c6 = -P748;
                c7 = -P992;
                c8 = -P239;
                c9 = P935;
                c10 = P464;
                c11 = -P822;
                c12 = -P663;
            } else if (mu==11) {
                c1 = P568;
                c2 = -P354;
                c3 = -P970;
                c4 = -P748;
                c5 = P120;
                c6 = P885;
                c7 = -P822;
                c8 = -P935;
                c9 = -P239;
                c10 = P663;
                c11 = P992;
                c12 = P464;
            } else {
                c1 = P885;
                c2 = P568;
                c3 = P120;
                c4 = -P354;
                c5 = -P748;
                c6 = -P970;
                c7 = -P464;
                c8 = -P822;
                c9 = -P992;
                c10 = -P935;
                c11 = -P663;
                c12 = -P239;
            }
            for (l=0; l<m; l++) {
                j00 = i0;
                j01 = i1;
                j2 = i2;
                j3 = i3;
                j4 = i4;
                j5 = i5;
                j6 = i6;
                j7 = i7;
                j8 = i8;
                j9 = i9;
                j10 = i10;
                j11 = i11;
                j12 = i12;
                for (jt=0; jt<nt; jt++) {
                    t1r = zr[j01]+zr[j12];
                    t1i = zi[j01]+zi[j12];
                    t2r = zr[j2]+zr[j11];
                    t2i = zi[j2]+zi[j11];
                    t3r = zr[j3]+zr[j10];
                    t3i = zi[j3]+zi[j10];
                    t4r = zr[j4]+zr[j9];
                    t4i = zi[j4]+zi[j9];
                    t5r = zr[j5]+zr[j8];
                    t5i = zi[j5]+zi[j8];
                    t6r = zr[j6]+zr[j7];
                    t6i = zi[j6]+zi[j7];
                    t7r = zr[j01]-zr[j12];
                    t7i = zi[j01]-zi[j12];
                    t8r = zr[j2]-zr[j11];
                    t8i = zi[j2]-zi[j11];
                    t9r = zr[j3]-zr[j10];
                    t9i = zi[j3]-zi[j10];
                    t10r = zr[j4]-zr[j9];
                    t10i = zi[j4]-zi[j9];
                    t11r = zr[j5]-zr[j8];
                    t11i = zi[j5]-zi[j8];
                    t12r = zr[j6]-zr[j7];
                    t12i = zi[j6]-zi[j7];
                    t13r = zr[j00]-0.5f*t6r;
                    t13i = zi[j00]-0.5f*t6i;
                    t14r = t1r-t6r;
                    t14i = t1i-t6i;
                    t15r = t2r-t6r;
                    t15i = t2i-t6i;
                    t16r = t3r-t6r;
                    t16i = t3i-t6i;
                    t17r = t4r-t6r;
                    t17i = t4i-t6i;
                    t18r = t5r-t6r;
                    t18i = t5i-t6i;
                    y1r = t13r+c1*t14r+c2*t15r+c3*t16r+c4*t17r+c5*t18r;
                    y1i = t13i+c1*t14i+c2*t15i+c3*t16i+c4*t17i+c5*t18i;
                    y2r = t13r+c2*t14r+c4*t15r+c6*t16r+c5*t17r+c3*t18r;
                    y2i = t13i+c2*t14i+c4*t15i+c6*t16i+c5*t17i+c3*t18i;
                    y3r = t13r+c3*t14r+c6*t15r+c4*t16r+c1*t17r+c2*t18r;
                    y3i = t13i+c3*t14i+c6*t15i+c4*t16i+c1*t17i+c2*t18i;
                    y4r = t13r+c4*t14r+c5*t15r+c1*t16r+c3*t17r+c6*t18r;
                    y4i = t13i+c4*t14i+c5*t15i+c1*t16i+c3*t17i+c6*t18i;
                    y5r = t13r+c5*t14r+c3*t15r+c2*t16r+c6*t17r+c1*t18r;
                    y5i = t13i+c5*t14i+c3*t15i+c2*t16i+c6*t17i+c1*t18i;
                    y6r = t13r+c6*t14r+c1*t15r+c5*t16r+c2*t17r+c4*t18r;
                    y6i = t13i+c6*t14i+c1*t15i+c5*t16i+c2*t17i+c4*t18i;
                    y7r = c12*t7r-c7*t8r+c11*t9r-c8*t10r+c10*t11r-c9*t12r;
                    y7i = c12*t7i-c7*t8i+c11*t9i-c8*t10i+c10*t11i-c9*t12i;
                    y8r = c11*t7r-c9*t8r+c8*t9r-c12*t10r-c7*t11r+c10*t12r;
                    y8i = c11*t7i-c9*t8i+c8*t9i-c12*t10i-c7*t11i+c10*t12i;
                    y9r = c10*t7r-c11*t8r-c7*t9r+c9*t10r-c12*t11r-c8*t12r;
                    y9i = c10*t7i-c11*t8i-c7*t9i+c9*t10i-c12*t11i-c8*t12i;
                    y10r = c9*t7r+c12*t8r-c10*t9r-c7*t10r+c8*t11r+c11*t12r;
                    y10i = c9*t7i+c12*t8i-c10*t9i-c7*t10i+c8*t11i+c11*t12i;
                    y11r = c8*t7r+c10*t8r+c12*t9r-c11*t10r-c9*t11r-c7*t12r;
                    y11i = c8*t7i+c10*t8i+c12*t9i-c11*t10i-c9*t11i-c7*t12i;
                    y12r = c7*t7r+c8*t8r+c9*t9r+c10*t10r+c11*t11r+c12*t12r;
                    y12i = c7*t7i+c8*t8i+c9*t9i+c10*t10i+c11*t11i+c12*t12i;
                    zr[j00] = zr[j00]+t1r+t2r+t3r+t4r+t5r+t6r;
                    zi[j00] = zi[j00]+t1i+t2i+t3i+t4i+t5i+t6i;
                    zr[j01] = y1r-y12i;
                    zi[j01] = y1i+y12r;
                    zr[j2] = y2r-y11i;
                    zi[j2] = y2i+y11r;
                    zr[j3] = y3r-y10i;
                    zi[j3] = y3i+y10r;
                    zr[j4] = y4r-y9i;
                    zi[j4] = y4i+y9r;
                    zr[j5] = y5r-y8i;
                    zi[j5] = y5i+y8r;
                    zr[j6] = y6r-y7i;
                    zi[j6] = y6i+y7r;
                    zr[j7] = y6r+y7i;
                    zi[j7] = y6i-y7r;
                    zr[j8] = y5r+y8i;
                    zi[j8] = y5i-y8r;
                    zr[j9] = y4r+y9i;
                    zi[j9] = y4i-y9r;
                    zr[j10] = y3r+y10i;
                    zi[j10] = y3i-y10r;
                    zr[j11] = y2r+y11i;
                    zi[j11] = y2i-y11r;
                    zr[j12] = y1r+y12i;
                    zi[j12] = y1i-y12r;
                    j00 += jstep;
                    j01 += jstep;
                    j2 += jstep;
                    j3 += jstep;
                    j4 += jstep;
                    j5 += jstep;
                    j6 += jstep;
                    j7 += jstep;
                    j8 += jstep;
                    j9 += jstep;
                    j10 += jstep;
                    j11 += jstep;
                    j12 += jstep;
                }
                it = i12+istep;
                i12 = i11+istep;
                i11 = i10+istep;
                i10 = i9+istep;
                i9 = i8+istep;
                i8 = i7+istep;
                i7 = i6+istep;
                i6 = i5+istep;
                i5 = i4+istep;
                i4 = i3+istep;
                i3 = i2+istep;
                i2 = i1+istep;
                i1 = i0+istep;
                i0 = it;
            }
            continue;
        }
        i13 = i12+iinc;
        if (i13>=imax) i13 = i13-imax;
        i14 = i13+iinc;
        if (i14>=imax) i14 = i14-imax;
        i15 = i14+iinc;
        if (i15>=imax) i15 = i15-imax;

        /* if factor is 16 */
        if (ifac==16) {
            if (mu==1) {
                c1 = 1.0;
                c2 = P923;
                c3 = P382;
                c4 = P707;
            } else if (mu==3) {
                c1 = -1.0;
                c2 = P382;
                c3 = P923;
                c4 = -P707;
            } else if (mu==5) {
                c1 = 1.0;
                c2 = -P382;
                c3 = P923;
                c4 = -P707;
            } else if (mu==7) {
                c1 = -1.0;
                c2 = -P923;
                c3 = P382;
                c4 = P707;
            } else if (mu==9) {
                c1 = 1.0;
                c2 = -P923;
                c3 = -P382;
                c4 = P707;
            } else if (mu==11) {
                c1 = -1.0;
                c2 = -P382;
                c3 = -P923;
                c4 = -P707;
            } else if (mu==13) {
                c1 = 1.0;
                c2 = P382;
                c3 = -P923;
                c4 = -P707;
            } else {
                c1 = -1.0;
                c2 = P923;
                c3 = -P382;
                c4 = P707;
            }
            c5 = c1*c4;
            c6 = c1*c3;
            c7 = c1*c2;
            for (l=0; l<m; l++) {
                j00 = i0;
                j01 = i1;
                j2 = i2;
                j3 = i3;
                j4 = i4;
                j5 = i5;
                j6 = i6;
                j7 = i7;
                j8 = i8;
                j9 = i9;
                j10 = i10;
                j11 = i11;
                j12 = i12;
                j13 = i13;
                j14 = i14;
                j15 = i15;
                for (jt=0; jt<nt; jt++) {
                    t1r = zr[j00]+zr[j8];
                    t1i = zi[j00]+zi[j8];
                    t2r = zr[j4]+zr[j12];
                    t2i = zi[j4]+zi[j12];
                    t3r = zr[j00]-zr[j8];
                    t3i = zi[j00]-zi[j8];
                    t4r = c1*(zr[j4]-zr[j12]);
                    t4i = c1*(zi[j4]-zi[j12]);
                    t5r = t1r+t2r;
                    t5i = t1i+t2i;
                    t6r = t1r-t2r;
                    t6i = t1i-t2i;
                    t7r = zr[j01]+zr[j9];
                    t7i = zi[j01]+zi[j9];
                    t8r = zr[j5]+zr[j13];
                    t8i = zi[j5]+zi[j13];
                    t9r = zr[j01]-zr[j9];
                    t9i = zi[j01]-zi[j9];
                    t10r = zr[j5]-zr[j13];
                    t10i = zi[j5]-zi[j13];
                    t11r = t7r+t8r;
                    t11i = t7i+t8i;
                    t12r = t7r-t8r;
                    t12i = t7i-t8i;
                    t13r = zr[j2]+zr[j10];
                    t13i = zi[j2]+zi[j10];
                    t14r = zr[j6]+zr[j14];
                    t14i = zi[j6]+zi[j14];
                    t15r = zr[j2]-zr[j10];
                    t15i = zi[j2]-zi[j10];
                    t16r = zr[j6]-zr[j14];
                    t16i = zi[j6]-zi[j14];
                    t17r = t13r+t14r;
                    t17i = t13i+t14i;
                    t18r = c4*(t15r-t16r);
                    t18i = c4*(t15i-t16i);
                    t19r = c5*(t15r+t16r);
                    t19i = c5*(t15i+t16i);
                    t20r = c1*(t13r-t14r);
                    t20i = c1*(t13i-t14i);
                    t21r = zr[j3]+zr[j11];
                    t21i = zi[j3]+zi[j11];
                    t22r = zr[j7]+zr[j15];
                    t22i = zi[j7]+zi[j15];
                    t23r = zr[j3]-zr[j11];
                    t23i = zi[j3]-zi[j11];
                    t24r = zr[j7]-zr[j15];
                    t24i = zi[j7]-zi[j15];
                    t25r = t21r+t22r;
                    t25i = t21i+t22i;
                    t26r = t21r-t22r;
                    t26i = t21i-t22i;
                    t27r = t9r+t24r;
                    t27i = t9i+t24i;
                    t28r = t10r+t23r;
                    t28i = t10i+t23i;
                    t29r = t9r-t24r;
                    t29i = t9i-t24i;
                    t30r = t10r-t23r;
                    t30i = t10i-t23i;
                    t31r = t5r+t17r;
                    t31i = t5i+t17i;
                    t32r = t11r+t25r;
                    t32i = t11i+t25i;
                    t33r = t3r+t18r;
                    t33i = t3i+t18i;
                    t34r = c2*t29r-c6*t30r;
                    t34i = c2*t29i-c6*t30i;
                    t35r = t3r-t18r;
                    t35i = t3i-t18i;
                    t36r = c7*t27r-c3*t28r;
                    t36i = c7*t27i-c3*t28i;
                    t37r = t4r+t19r;
                    t37i = t4i+t19i;
                    t38r = c3*t27r+c7*t28r;
                    t38i = c3*t27i+c7*t28i;
                    t39r = t4r-t19r;
                    t39i = t4i-t19i;
                    t40r = c6*t29r+c2*t30r;
                    t40i = c6*t29i+c2*t30i;
                    t41r = c4*(t12r-t26r);
                    t41i = c4*(t12i-t26i);
                    t42r = c5*(t12r+t26r);
                    t42i = c5*(t12i+t26i);
                    y1r = t33r+t34r;
                    y1i = t33i+t34i;
                    y2r = t6r+t41r;
                    y2i = t6i+t41i;
                    y3r = t35r+t40r;
                    y3i = t35i+t40i;
                    y4r = t5r-t17r;
                    y4i = t5i-t17i;
                    y5r = t35r-t40r;
                    y5i = t35i-t40i;
                    y6r = t6r-t41r;
                    y6i = t6i-t41i;
                    y7r = t33r-t34r;
                    y7i = t33i-t34i;
                    y9r = t38r-t37r;
                    y9i = t38i-t37i;
                    y10r = t42r-t20r;
                    y10i = t42i-t20i;
                    y11r = t36r+t39r;
                    y11i = t36i+t39i;
                    y12r = c1*(t11r-t25r);
                    y12i = c1*(t11i-t25i);
                    y13r = t36r-t39r;
                    y13i = t36i-t39i;
                    y14r = t42r+t20r;
                    y14i = t42i+t20i;
                    y15r = t38r+t37r;
                    y15i = t38i+t37i;
                    zr[j00] = t31r+t32r;
                    zi[j00] = t31i+t32i;
                    zr[j01] = y1r-y15i;
                    zi[j01] = y1i+y15r;
                    zr[j2] = y2r-y14i;
                    zi[j2] = y2i+y14r;
                    zr[j3] = y3r-y13i;
                    zi[j3] = y3i+y13r;
                    zr[j4] = y4r-y12i;
                    zi[j4] = y4i+y12r;
                    zr[j5] = y5r-y11i;
                    zi[j5] = y5i+y11r;
                    zr[j6] = y6r-y10i;
                    zi[j6] = y6i+y10r;
                    zr[j7] = y7r-y9i;
                    zi[j7] = y7i+y9r;
                    zr[j8] = t31r-t32r;
                    zi[j8] = t31i-t32i;
                    zr[j9] = y7r+y9i;
                    zi[j9] = y7i-y9r;
                    zr[j10] = y6r+y10i;
                    zi[j10] = y6i-y10r;
                    zr[j11] = y5r+y11i;
                    zi[j11] = y5i-y11r;
                    zr[j12] = y4r+y12i;
                    zi[j12] = y4i-y12r;
                    zr[j13] = y3r+y13i;
                    zi[j13] = y3i-y13r;
                    zr[j14] = y2r+y14i;
                    zi[j14] = y2i-y14r;
                    zr[j15] = y1r+y15i;
                    zi[j15] = y1i-y15r;
                    j00 += jstep;
                    j01 += jstep;
                    j2 += jstep;
                    j3 += jstep;
                    j4 += jstep;
                    j5 += jstep;
                    j6 += jstep;
                    j7 += jstep;
                    j8 += jstep;
                    j9 += jstep;
                    j10 += jstep;
                    j11 += jstep;
                    j12 += jstep;
                    j13 += jstep;
                    j14 += jstep;
                    j15 += jstep;
                }
                it = i15+istep;
                i15 = i14+istep;
                i14 = i13+istep;
                i13 = i12+istep;
                i12 = i11+istep;
                i11 = i10+istep;
                i10 = i9+istep;
                i9 = i8+istep;
                i8 = i7+istep;
                i7 = i6+istep;
                i6 = i5+istep;
                i5 = i4+istep;
                i4 = i3+istep;
                i3 = i2+istep;
                i2 = i1+istep;
                i1 = i0+istep;
                i0 = it;
            }
            continue;
        }
    }
}


void pfa2cc (int isign, int idim, int n1, int n2, complex cz[])
/*****************************************************************************
Prime factor fft:  2-D complex to complex transforms, in place
******************************************************************************
Input:
isign       	sign of isign is the sign of exponent in fourier kernel
idim        	dimension to transform, either 1 or 2 (see notes)
n1          	1st (fast) dimension of array to be transformed (see notes)
n2          	2nd (slow) dimension of array to be transformed (see notes)
z           	array[n2][n1] of complex elements to be transformed in place

Output:
z		array[n2][n1] of complex elements transformed
******************************************************************************
Notes:
Only one (either the 1st or 2nd) dimension of the 2-D array is transformed.

If idim equals 1, then n2 transforms of n1 complex elements are performed; 
else, if idim equals 2, then n1 transforms of n2 complex elements are 
performed.

Although z appears in the argument list as a one-dimensional array,
z may be viewed as an n1 by n2 two-dimensional array:  z[n2][n1].

Let n denote the transform length, either n1 or n2, depending on idim.
Then, n must be factorable into mutually prime factors taken 
from the set {2,3,4,5,7,8,9,11,13,16}.  in other words,
    n = 2**p * 3**q * 5**r * 7**s * 11**t * 13**u
where
    0 <= p <= 4,  0 <= q <= 2,  0 <= r,s,t,u <= 1
is required for pfa2cc to yield meaningful results.  this
restriction implies that n is restricted to the range
    1 <= n <= 720720 (= 5*7*9*11*13*16)

To perform a two-dimensional transform of an n1 by n2 complex array 
(assuming that both n1 and n2 are valid "n"), stored with n1 fast 
and n2 slow:  pfa2cc(isign,1,n1,n2,z);  pfa2cc(isign,2,n1,n2,z);
******************************************************************************
References:  
Temperton, C., 1985, Implementation of a self-sorting
in-place prime factor fft algorithm:  Journal of
Computational Physics, v. 58, p. 283-299.

Temperton, C., 1988, A new set of minimum-add rotated
rotated dft modules: Journal of Computational Physics,
v. 75, p. 190-198.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/15/89
*****************************************************************************/
{
    int n,nt,k,kt;

    /* determine transform length, number of transforms, and strides */
    if (idim==1) {
        n = n1;
        nt = n2;
        k = 1;
        kt = n1;
    } else {
        n = n2;
        nt = n1;
        k = n1;
        kt = 1;
    }

    /* do multiple complex to complex transforms */
    pfamcc(isign,n,nt,k,kt,cz);
}

void pfa2cr (int isign, int idim, int n1, int n2, complex cz[], float rz[])
/*****************************************************************************
Prime factor fft:  2-D complex to real transforms
******************************************************************************
Input:
isign       sign of isign is the sign of exponent in fourier kernel
idim        dimension to transform, which must be either 1 or 2 (see notes)
n1          1st (fast) dimension of array to be transformed (see notes)
n2          2nd (slow) dimension of array to be transformed (see notes)
cz          array of complex values (may be equivalenced to rz)

Output:
rz          array of real values (may be equivalenced to cz)
******************************************************************************
Notes:
If idim equals 1, then n2 transforms of n1/2+1 complex elements to n1 real 
elements are performed; else, if idim equals 2, then n1 transforms of n2/2+1 
complex elements to n2 real elements are performed.

Although rz appears in the argument list as a one-dimensional array,
rz may be viewed as an n1 by n2 two-dimensional array:  rz[n2][n1].  
Likewise, depending on idim, cz may be viewed as either an n1/2+1 by 
n2 or an n1 by n2/2+1 two-dimensional array of complex elements.

Let n denote the transform length, either n1 or n2, depending on idim.
Because pfa2rc uses pfa2cc to do most of the work, n must be even 
and n/2 must be a valid length for pfa2cc.  The simplest way to
obtain a valid n is via n = npfar(nmin).  A more optimal n can be 
obtained with npfaro.
******************************************************************************
References:  
Press et al, 1988, Numerical Recipes in C, p. 417.

Also, see notes and references for function pfa2cc.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/13/89
Modified:  Dave Hale, Colorado School of Mines, 11/26/89
	Fixed bug that gave wrong answers for idim==2 and cz not 
	equivalenced to rz.  The bug was in copying the input
	to the output in the first section below - the j index
	was not being incremented along with i1.  Fixed by adding
	j++ after the i1++.
*****************************************************************************/
{
    int i1,i2,j,k,it,jt,kt,n,nt,itmul,itinc;
    float *z,*temp,tempr,tempi,sumr,sumi,difr,difi;
    double wr,wi,wpr,wpi,wtemp,theta;

    /* if transforming dimension 1 */
    if (idim==1) {

        /* copy input to output and fix dc and nyquist */
        z = (float*)cz;
        for (i2=0,jt=0,kt=0; i2<n2; i2++,jt+=n1,kt+=(n1+2)) {
            rz[jt+1] = z[kt]-z[kt+n1];
            rz[jt] = z[kt]+z[kt+n1];
            for (i1=2,j=jt+2,k=kt+2; i1<n1; i1++,j++,k++)
                rz[j] = z[k];
        }
        z = rz;

        /* set transform length, number of transforms and strides */
        n = n1;
        nt = n2;
        itmul = 1;
        itinc = n1;

    /* else, if transforming dimension 2 */
    } else {

        /* copy input to output and fix dc and nyquist */
        z = (float*)cz;
        for (i2=1; i2<n2/2; i2++) {
            for (i1=0,j=i2*n1*2; i1<n1*2; i1++,j++)
                rz[j] = z[j];
        }
        for (i1=0,j=n1*n2; i1<n1*2; i1+=2,j+=2) {
            rz[i1+1] = z[i1]-z[j];
            rz[i1] = z[i1]+z[j];
        }
        z = rz;

        /* set transform length, number of transforms and strides */
        n = n2;
        nt = n1;
        itmul = n1;
        itinc = 2;
    }

    /* initialize cosine-sine recurrence */
    theta = 2.0*PI/(double)n;
    if (isign>0) theta = -theta;
    wtemp = sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi = sin(theta);
    wr = 1.0+wpr;
    wi = wpi;

    /* twiddle transforms simultaneously */
    for (j=2,k=n-2; j<=n/2; j+=2,k-=2) {
        jt = j*itmul;
        kt = k*itmul;
        for (it=0; it<nt; it++) {
            sumr = z[jt]+z[kt];
            sumi = z[jt+1]+z[kt+1];
            difr = z[jt]-z[kt];
            difi = z[jt+1]-z[kt+1];
			tempr = (float)(wi*difr - wr*sumi);
            tempi = (float)(wi*sumi+wr*difr);
            z[jt] = sumr+tempr;
            z[jt+1] = difi+tempi;
            z[kt] = sumr-tempr;
            z[kt+1] = tempi-difi;
            jt += itinc;
            kt += itinc;
        }
        wtemp = wr;
        wr += wr*wpr-wi*wpi;
        wi += wi*wpr+wtemp*wpi;
    }

    /* if transforming dimension 1 */
    if (idim==1) {

        /* transform as complex elements */
        pfa2cc(isign,1,n1/2,n2,(complex*)z);

    /* else, if transforming dimension 2 */
    } else {

        /* transform as complex elements */
        pfa2cc(isign,2,n1,n2/2,(complex*)z);

        /* unmerge even and odd vectors */
		
		temp = (float*)ealloc1float(n1);
        for (i2=0; i2<n2; i2+=2) {
            for (i1=0,j=i2*n1+1; i1<n1; i1++,j+=2)
                temp[i1] = z[j];
            for (i1=0,j=i2*n1,k=i2*n1; i1<n1; i1++,j+=2,k++)
                z[k] = z[j];
            for (i1=0,j=(i2+1)*n1; i1<n1; i1++,j++)
                z[j] = temp[i1];
        }
        free1float(temp);
    }
}

void pfa2rc (int isign, int idim, int n1, int n2, float rz[], complex cz[])
/*****************************************************************************
Prime factor fft:  2-D real to complex transforms
******************************************************************************
Input:
isign       sign of isign is the sign of exponent in fourier kernel
idim        dimension to transform, which must be either 1 or 2 (see notes)
n1          1st (fast) dimension of array to be transformed (see notes)
n2          2nd (slow) dimension of array to be transformed (see notes)
rz          array of real values (may be equivalenced to cz)

Output:
cz          array of complex values (may be equivalenced to rz)
******************************************************************************
Notes:
If idim equals 1, then n2 transforms of n1 real elements to n1/2+1 complex 
elements are performed; else, if idim equals 2, then n1 transforms of n2 
real elements to n2/2+1 complex elements are performed.

Although rz appears in the argument list as a one-dimensional array,
rz may be viewed as an n1 by n2 two-dimensional array:  rz[n2][n1].  
Likewise, depending on idim, cz may be viewed as either an n1/2+1 by 
n2 or an n1 by n2/2+1 two-dimensional array of complex elements.

Let n denote the transform length, either n1 or n2, depending on idim.
Because pfa2rc uses pfa2cc to do most of the work, n must be even 
and n/2 must be a valid length for pfa2cc.  The simplest way to
obtain a valid n is via n = npfar(nmin).  A more optimal n can be 
obtained with npfaro.
******************************************************************************
References:  
Press et al, 1988, Numerical Recipes in C, p. 417.

Also, see notes and references for function pfa2cc.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/13/89
*****************************************************************************/
{
    int i1,i2,j,k,it,jt,kt,n,nt,itmul,itinc;
    float *z,*temp,tempr,tempi,sumr,sumi,difr,difi;
    double wr,wi,wpr,wpi,wtemp,theta;

    /* copy input to output while scaling */
    z = (float*)cz;
    for (i2=0,j=0; i2<n2; i2++)
        for (i1=0; i1<n1; i1++,j++)
            z[j] = 0.5f*rz[j];

    /* if transforming dimension 1 */
    if (idim==1) {

        /* transform as complex elements */
        pfa2cc(isign,1,n1/2,n2,cz);

        /* shift rows to make room for nyquist */
        z = (float*)cz;
        for (i2=n2-1; i2>0; i2--) {
            jt = i2*n1+n1-1;
            kt = jt+i2*2;
            for (i1=n1-1,j=jt,k=kt; i1>=0; i1--,j--,k--)
                z[k] = z[j];
        }

        /* set transform length, number of transforms and strides */
        n = n1;
        nt = n2;
        itmul = 1;
        itinc = n1+2;

    /* else, if transforming dimension 2 */
    } else {

        /* merge even and odd vectors */
        temp = z+n1*n2;
        for (i2=0; i2<n2; i2+=2) {
            for (i1=0,j=i2*n1; i1<n1; i1++,j++)
                temp[i1] = z[j];
            for (i1=0,j=(i2+1)*n1,k=i2*n1+1; i1<n1; i1++,j++,k+=2)
                z[k] = z[j];
            for (i1=0,j=i2*n1; i1<n1; i1++,j+=2)
                z[j] = temp[i1];
        }

        /* transform as complex elements */
        pfa2cc(isign,2,n1,n2/2,cz);

        /* set transform length, number of transforms and strides */
        n = n2;
        nt = n1;
        itmul = n1;
        itinc = 2;
    }

    /* fix dc and nyquist for each transform */
    for (it=0,j=0,k=n*itmul; it<nt; it++,j+=itinc,k+=itinc) {
        z[k] = 2.0f*(z[j]-z[j+1]);
        z[j] = 2.0f*(z[j]+z[j+1]);
        z[k+1] = 0.0f;
        z[j+1] = 0.0f;
    }

    /* initialize cosine-sine recurrence */
    theta = 2.0*PI/(double)n;
    if (isign<0) theta = -theta;
    wtemp = sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi = sin(theta);
    wr = 1.0+wpr;
    wi = wpi;

    /* twiddle transforms simultaneously */
    for (j=2,k=n-2; j<=n/2; j+=2,k-=2) {
        jt = j*itmul;
        kt = k*itmul;
        for (it=0; it<nt; it++) {
            sumr = z[jt]+z[kt];
            sumi = z[jt+1]+z[kt+1];
            difr = z[jt]-z[kt];
            difi = z[jt+1]-z[kt+1];
			tempr = (float)(wi*difr + wr*sumi);
			tempi = (float)(wi*sumi - wr*difr);
            z[jt] = sumr+tempr;
            z[jt+1] = difi+tempi;
            z[kt] = sumr-tempr;
            z[kt+1] = tempi-difi;
            jt += itinc;
            kt += itinc;
        }
        wtemp = wr;
        wr += wr*wpr-wi*wpi;
        wi += wi*wpr+wtemp*wpi;
    }
}

#ifdef TEST
main()
{
	int nmin,n,no,nfft,nffto;
	float cpu,total;
	complex *c=alloc1complex(720720);
	int npfao2 (int nmin, int nmax);
		
	for (n=0; n<720720; ++n)
		c[n].r = c[n].i = 0.0;

	for (nmin=npfa(100); nmin<=10000; nmin=npfa(nmin+1)) {
		n = npfa(nmin);
		for (nfft=0,total=0.0; total<1.0; ++nfft) {
			cpu = cpusec();
			pfacc(1,n,c);
			total += cpusec()-cpu+FLT_EPSILON;
		}
		no = npfao(nmin,2*nmin);
		for (nffto=0,total=0.0; total<1.0; ++nffto) {
			cpu = cpusec();
			pfacc(1,no,c);
			total += cpusec()-cpu+FLT_EPSILON;
		}
		printf("valid n=%d cost=%g optimal n=%d cost=%g\n",
			n,1.0/nfft,no,1.0/nffto);
	}
}
#endif /* TEST */






void intlin(int nin, float xin[], float yin[], float yinl, float yinr,
	int nout, float xout[], float yout[])
/*****************************************************************************
evaluate y(x) via linear interpolation of y(x[0]), y(x[1]), ...
******************************************************************************
Input:
nin		length of xin and yin arrays
xin		array[nin] of monotonically increasing or decreasing x values
yin		array[nin] of input y(x) values
yinl		value used to extraplate y(x) to left of input yin values
yinr		value used to extraplate y(x) to right of input yin values
nout		length of xout and yout arrays
xout		array[nout] of x values at which to evaluate y(x)

Output:
yout		array[nout] of linearly interpolated y(x) values
******************************************************************************
Notes:
xin values must be monotonically increasing or decreasing.

Extrapolation of the function y(x) for xout values outside the range
spanned by the xin values in performed as follows:

For monotonically increasing xin values,
yout=yinl if xout<xin[0], and yout=yinr if xout>xin[nin-1].

For monotonically decreasing xin values,
yout=yinl if xout>xin[0], and yout=yinr if xout<xin[nin-1].

If nin==1, then the monotonically increasing case is used.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
{
	static int idx;
	int jout;
	float x;

	/* if input x values are monotonically increasing, then */
	if (xin[0] <= xin[nin - 1]) {
		for (jout = 0; jout<nout; jout++) {
			x = xout[jout];
			if (x<xin[0])
				yout[jout] = yinl;
			else if (x>xin[nin - 1])
				yout[jout] = yinr;
			else if (x == xin[nin - 1] || nin == 1)
				yout[jout] = yin[nin - 1];
			else {
				xindex(nin, xin, x, &idx);
				yout[jout] = yin[idx] + (x - xin[idx])
					*(yin[idx + 1] - yin[idx])
					/ (xin[idx + 1] - xin[idx]);
			}
		}

		/* else, if input x values are monotonically decreasing, then */
	}
	else {
		for (jout = 0; jout<nout; jout++) {
			x = xout[jout];
			if (x>xin[0])
				yout[jout] = yinl;
			else if (x<xin[nin - 1])
				yout[jout] = yinr;
			else if (x == xin[nin - 1] || nin == 1)
				yout[jout] = yin[nin - 1];
			else {
				xindex(nin, xin, x, &idx);
				yout[jout] = yin[idx] + (x - xin[idx])
					*(yin[idx + 1] - yin[idx])
					/ (xin[idx + 1] - xin[idx]);
			}
		}
	}
}





void convolve_cwp(int lx, int ifx, float *x,
	int ly, int ify, float *y,
	int lz, int ifz, float *z)
	/*****************************************************************************
	Compute z = x convolved with y; i.e.,

	ifx+lx-1
	z[i] =   sum    x[j]*y[i-j]  ;  i = ifz,...,ifz+lz-1
	j=ifx
	******************************************************************************
	Input:
	lx		length of x array
	ifx		sample index of first x
	x		array[lx] to be convolved with y
	ly		length of y array
	ify		sample index of first y
	y		array[ly] with which x is to be convolved
	lz		length of z array
	ifz		sample index of first z

	Output:
	z		array[lz] containing x convolved with y
	******************************************************************************
	Notes:
	The x samples are contained in x[0], x[1], ..., x[lx-1]; likewise for
	the y and z samples.  The sample indices of the first x, y, and z values
	determine the location of the origin for each array.  For example, if
	z is to be a weighted average of the nearest 5 samples of y, one might
	use
	...
	x[0] = x[1] = x[2] = x[3] = x[4] = 1.0/5.0;
	conv(5,-2,x,lx,0,y,ly,0,z);
	...
	In this example, the filter x is symmetric, with index of first sample = -2.

	This function is optimized for architectures that can simultaneously perform
	a multiply, add, and one load from memory; e.g., the IBM RISC System/6000.
	Because, for each value of i, it accumulates the convolution sum z[i] in a
	scalar, this function is not likely to be optimal for vector architectures.
	******************************************************************************
	Author:  Dave Hale, Colorado School of Mines, 11/23/91
	*****************************************************************************/
#ifdef SIMPLE_CONV
{
	int ilx = ifx + lx - 1, ily = ify + ly - 1, ilz = ifz + lz - 1, i, j, jlow, jhigh;
	float sum;

	x -= ifx;  y -= ify;  z -= ifz;
	for (i = ifz; i <= ilz; ++i) {
		jlow = i - ily;  if (jlow<ifx) jlow = ifx;
		jhigh = i - ify;  if (jhigh>ilx) jhigh = ilx;
		for (j = jlow, sum = 0.0; j <= jhigh; ++j)
			sum += x[j] * y[i - j];
		z[i] = sum;
	}
}
#else
{
	int ilx = ifx + lx - 1, ily = ify + ly - 1, ilz = ifz + lz - 1,
		i, j, ilow, ihigh, jlow, jhigh;
	float sa, sb, xa, xb, ya, yb, *t;

	/* if x is longer than y, swap x and y */
	if (lx>ly) {
		i = ifx;  ifx = ify;  ify = i;
		i = ilx;  ilx = ily;  ily = i;
		i = lx;  lx = ly;  ly = i;
		t = x;  x = y;  y = t;
	}

	/* handle short x with special code */
	if (lx >= 1 && lx <= 30) {
		convolve_cwp_s(lx, ifx, x, ly, ify, y, lz, ifz, z);
		return;
	}

	/* adjust pointers for indices of first samples */
	x -= ifx;
	y -= ify;
	z -= ifz;

	/* OFF LEFT:  i < ify+ifx */

	/* zero output for all i */
	ilow = ifz;
	ihigh = ify + ifx - 1;  if (ihigh>ilz) ihigh = ilz;
	for (i = ilow; i <= ihigh; ++i)
		z[i] = 0.0;

	/* ROLLING ON:  ify+ifx <= i < ify+ilx */

	/* if necessary, do one i so that number of j in overlap is odd */
	if (i<ify + ilx && i <= ilz) {
		jlow = ifx;
		jhigh = i - ify;
		if ((jhigh - jlow) % 2) {
			sa = 0.0;
			for (j = jlow; j <= jhigh; ++j)
				sa += x[j] * y[i - j];
			z[i++] = sa;
		}
	}

	/* loop over pairs of i and j */
	ilow = i;
	ihigh = ilx + ify - 1;  if (ihigh>ilz) ihigh = ilz;
	jlow = ifx;
	jhigh = ilow - ify;
	for (i = ilow; i<ihigh; i += 2, jhigh += 2) {
		sa = sb = 0.0;
		xb = x[jhigh + 1];
		yb = 0.0;
		for (j = jhigh; j >= jlow; j -= 2) {
			sa += xb*yb;
			ya = y[i - j];
			sb += xb*ya;
			xa = x[j];
			sa += xa*ya;
			yb = y[i + 1 - j];
			sb += xa*yb;
			xb = x[j - 1];
		}
		z[i] = sa;
		z[i + 1] = sb;
	}

	/* if number of i is odd */
	if (i == ihigh) {
		jlow = ifx;
		jhigh = i - ify;
		sa = 0.0;
		for (j = jlow; j <= jhigh; ++j)
			sa += x[j] * y[i - j];
		z[i++] = sa;
	}

	/* MIDDLE:  ify+ilx <= i <= ily+ifx */

	/* determine limits for i and j */
	ilow = i;
	ihigh = ily + ifx;  if (ihigh>ilz) ihigh = ilz;
	jlow = ifx;
	jhigh = ilx;

	/* if number of j is even, do j in pairs with no leftover */
	if ((jhigh - jlow) % 2) {
		for (i = ilow; i<ihigh; i += 2) {
			sa = sb = 0.0;
			yb = y[i + 1 - jlow];
			xa = x[jlow];
			for (j = jlow; j<jhigh; j += 2) {
				sb += xa*yb;
				ya = y[i - j];
				sa += xa*ya;
				xb = x[j + 1];
				sb += xb*ya;
				yb = y[i - 1 - j];
				sa += xb*yb;
				xa = x[j + 2];
			}
			z[i] = sa;
			z[i + 1] = sb;
		}

		/* else, number of j is odd, so do j in pairs with leftover */
	}
	else {
		for (i = ilow; i<ihigh; i += 2) {
			sa = sb = 0.0;
			yb = y[i + 1 - jlow];
			xa = x[jlow];
			for (j = jlow; j<jhigh; j += 2) {
				sb += xa*yb;
				ya = y[i - j];
				sa += xa*ya;
				xb = x[j + 1];
				sb += xb*ya;
				yb = y[i - 1 - j];
				sa += xb*yb;
				xa = x[j + 2];
			}
			z[i] = sa + x[jhigh] * y[i - jhigh];
			z[i + 1] = sb + x[jhigh] * y[i + 1 - jhigh];
		}
	}

	/* if number of i is odd */
	if (i == ihigh) {
		sa = 0.0;
		for (j = jlow; j <= jhigh; ++j)
			sa += x[j] * y[i - j];
		z[i++] = sa;
	}

	/* ROLLING OFF:  ily+ifx < i <= ily+ilx */

	/* if necessary, do one i so that number of j in overlap is even */
	if (i <= ily + ilx && i <= ilz) {
		jlow = i - ily;
		jhigh = ilx;
		if (!((jhigh - jlow) % 2)) {
			sa = 0.0;
			for (j = jlow; j <= jhigh; ++j)
				sa += x[j] * y[i - j];
			z[i++] = sa;
		}
	}

	/* number of j is now even, so loop over both i and j in pairs */
	ilow = i;
	ihigh = ily + ilx;  if (ihigh>ilz) ihigh = ilz;
	jlow = ilow - ily;
	jhigh = ilx - 2; /* Dave's new patch */
	for (i = ilow; i<ihigh; i += 2, jlow += 2) {
		sa = sb = 0.0;
		xa = x[jlow];
		yb = 0.0;
		for (j = jlow; j<jhigh; j += 2) {
			sb += xa*yb;
			ya = y[i - j];
			sa += xa*ya;
			xb = x[j + 1];
			sb += xb*ya;
			yb = y[i - 1 - j];
			sa += xb*yb;
			xa = x[j + 2];
		}
		sb += xa*yb;
		ya = y[i - j];
		sa += xa*ya;
		xb = x[j + 1];
		sb += xb*ya;
		yb = y[i - 1 - j];
		sa += xb*yb;
		z[i] = sa;
		z[i + 1] = sb;
	}

	/* if number of i is odd */
	if (i == ihigh) {
		jlow = i - ily;
		jhigh = ilx;
		sa = 0.0;
		for (j = jlow; j <= jhigh; ++j)
			sa += x[j] * y[i - j];
		z[i++] = sa;
	}

	/* OFF RIGHT:  ily+ilx < i */

	/* zero output for all i */
	ilow = i;
	ihigh = ilz;
	for (i = ilow; i <= ihigh; ++i)
		z[i] = 0.0;
}



/* internal function optimized for short x */
static void convolve_cwp_s(int lx, int ifx, float *x,
	int ly, int ify, float *y,
	int lz, int ifz, float *z)
{
	int ilx = ifx + lx - 1, ily = ify + ly - 1, ilz = ifz + lz - 1,
		i, j, ilow, ihigh, jlow, jhigh;
	float x0, x1, x2, x3, x4, x5, x6, x7, x8, x9,
		x10, x11, x12, x13, x14, x15, x16, x17, x18, x19,
		x20, x21, x22, x23, x24, x25, x26, x27, x28, x29,
		ya, yb, z0, z1, sum;

	x -= ifx;
	y -= ify;
	z -= ifz;


	/* OFF LEFT:  i < ifx+ify */
	ilow = ifz;
	ihigh = ify + ifx - 1;  if (ihigh>ilz) ihigh = ilz;
	for (i = ilow; i <= ihigh; ++i)
		z[i] = 0.0;

	/* ROLLING ON:  ify+ifx <= i < ify+ilx */
	ilow = ify + ifx;  if (ilow<ifz) ilow = ifz;
	ihigh = ify + ilx - 1;  if (ihigh>ilz) ihigh = ilz;
	jlow = ifx;
	jhigh = ilow - ify;
	for (i = ilow; i <= ihigh; ++i, ++jhigh) {
		for (j = jlow, sum = 0.0; j <= jhigh; ++j)
			sum += x[j] * y[i - j];
		z[i] = sum;
	}

	/* MIDDLE:  ify+ilx <= i <= ily+ifx */
	ilow = ify + ilx;  if (ilow<ifz) ilow = ifz;
	ihigh = ily + ifx;  if (ihigh>ilz) ihigh = ilz;
	if (lx == 1) {
		x0 = x[ifx];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 2) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 3) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 4) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 5) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 6) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 7) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 8) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 9) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 10) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 11) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 12) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 13) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 14) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 15) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 16) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 17) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 18) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 19) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 20) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 21) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 22) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 23) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 24) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 25) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		x24 = x[ifx + 24];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i - ifx - 24];  z0 += x24*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 26) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		x24 = x[ifx + 24];
		x25 = x[ifx + 25];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i - ifx - 24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i - ifx - 25];  z0 += x25*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 27) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		x24 = x[ifx + 24];
		x25 = x[ifx + 25];
		x26 = x[ifx + 26];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i - ifx - 24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i - ifx - 25];  z0 += x25*ya;  z1 += x26*ya;
			yb = y[i - ifx - 26];  z0 += x26*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 28) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		x24 = x[ifx + 24];
		x25 = x[ifx + 25];
		x26 = x[ifx + 26];
		x27 = x[ifx + 27];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i - ifx - 24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i - ifx - 25];  z0 += x25*ya;  z1 += x26*ya;
			yb = y[i - ifx - 26];  z0 += x26*yb;  z1 += x27*yb;
			ya = y[i - ifx - 27];  z0 += x27*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 29) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		x24 = x[ifx + 24];
		x25 = x[ifx + 25];
		x26 = x[ifx + 26];
		x27 = x[ifx + 27];
		x28 = x[ifx + 28];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i - ifx - 24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i - ifx - 25];  z0 += x25*ya;  z1 += x26*ya;
			yb = y[i - ifx - 26];  z0 += x26*yb;  z1 += x27*yb;
			ya = y[i - ifx - 27];  z0 += x27*ya;  z1 += x28*ya;
			yb = y[i - ifx - 28];  z0 += x28*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 30) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		x24 = x[ifx + 24];
		x25 = x[ifx + 25];
		x26 = x[ifx + 26];
		x27 = x[ifx + 27];
		x28 = x[ifx + 28];
		x29 = x[ifx + 29];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i - ifx - 24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i - ifx - 25];  z0 += x25*ya;  z1 += x26*ya;
			yb = y[i - ifx - 26];  z0 += x26*yb;  z1 += x27*yb;
			ya = y[i - ifx - 27];  z0 += x27*ya;  z1 += x28*ya;
			yb = y[i - ifx - 28];  z0 += x28*yb;  z1 += x29*yb;
			ya = y[i - ifx - 29];  z0 += x29*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	if (ihigh >= ilow && (ihigh - ilow) % 2 == 0) {
		ilow = ihigh;
		jlow = ifx;
		jhigh = ilx;
		for (i = ilow; i <= ihigh; ++i) {
			for (j = jlow, sum = 0.0; j <= jhigh; ++j)
				sum += x[j] * y[i - j];
			z[i] = sum;
		}
	}

	/* ROLLING OFF:  ily+ifx < i <= ily+ilx */
	ilow = ily + ifx + 1;  if (ilow<ifz) ilow = ifz;
	ihigh = ily + ilx;  if (ihigh>ilz) ihigh = ilz;
	jlow = ilow - ily;
	jhigh = ilx;
	for (i = ilow; i <= ihigh; ++i, ++jlow) {
		for (j = jlow, sum = 0.0; j <= jhigh; ++j)
			sum += x[j] * y[i - j];
		z[i] = sum;
	}

	/* OFF RIGHT:  ily+ilx < i */
	ilow = ily + ilx + 1;  if (ilow<ifz) ilow = ifz;
	ihigh = ilz;
	for (i = ilow; i <= ihigh; ++i)
		z[i] = 0.0;
}
#endif



/******************************************************************************

Subroutine to apply a one-dimensional gaussian smoothing

******************************************************************************/
void gaussian1d_smoothing(int ns, int nsr, float *data)
/******************************************************************************
Input:
ns		number of samples in the input data
nsr		width (in samples) of the gaussian for which
amplitude > 0.5*max amplitude
data		1-D array[ns] of data to smooth

Output:
data		1-D array[ns] of smoothed data
******************************************************************************/
{
	int is;				/* loop counter */
	float sum = 0.0;
	float fcut;
	float r;
	float fcutr = 1.0f / nsr;
	static int n;
	static int mean;
	static float fcutl;
	static float s[401];		/* smoothing filter array */
	float *temp;			/* temporary array */

	/* allocate space */
	temp = alloc1float(ns);

	/* save input fcut */
	fcut = fcutr;

	/* don't smooth if nsr equal to zero */
	if (nsr == 0 || ns <= 1) return;

	/* if halfwidth more than 100 samples, truncate */
	if (nsr>100) fcut = 1.0f / 100;

	/* initialize smoothing function if not the same as the last one used */
	if (fcut != fcutl) {
		fcutl = fcut;

		/* set span of 3, at width of 1.5*exp(-PI*1.5**2)=1/1174 */
		n = (int)(3.0 / fcut + 0.5);
		n = 2 * n / 2 + 1;		/* make it odd for symmetry */

		/* mean is the index of the zero in the smoothing wavelet */
		mean = n / 2;

		/* s(n) is the smoothing gaussian */
		for (is = 1; is <= n; is++) {
			r = (float)(is - mean - 1);
			r = -r*r*fcut*fcut*PI;
			s[is - 1] = (float)exp(r);
		}

		/* normalize to unit area, will preserve DC frequency at full
		amplitude. Frequency at fcut will be half amplitude */
		for (is = 0; is<n; is++) sum += s[is];
		for (is = 0; is<n; is++) s[is] /= sum;
	}

	/* convolve by gaussian into buffer */
	if (1.01 / fcutr>(float)ns) {

		/* replace drastic smoothing by averaging */
		sum = 0.0;
		for (is = 0; is<ns; is++) sum += data[is];
		sum /= ns;
		for (is = 0; is<ns; is++) data[is] = sum;

	}
	else {

		/* convolve with gaussian */
		convolve_cwp(n, -mean, s, ns, -mean, data, ns, -mean, temp);

		/* copy filtered data back to output array */
		for (is = 0; is<ns; is++) data[is] = temp[is];
	}

	/* free allocated space */
	free1float(temp);
}



#ifdef TEST
static void convolve_cwp1(int lx, int ifx, float *x,
	int ly, int ify, float *y,
	int lz, int ifz, float *z)
{
	int ilx = ifx + lx - 1, ily = ify + ly - 1, ilz = ifz + lz - 1, i, j, jlow, jhigh;
	float sum;

	x -= ifx;  y -= ify;  z -= ifz;
	for (i = ifz; i <= ilz; ++i) {
		jlow = i - ily;  if (jlow<ifx) jlow = ifx;
		jhigh = i - ify;  if (jhigh>ilx) jhigh = ilx;
		for (j = jlow, sum = 0.0; j <= jhigh; ++j)
			sum += x[j] * y[i - j];
		z[i] = sum;
	}
}

main()
{
	int lx, ly, lz, ifx, ify, ifz, i;
	float *x, *y, *z, *z1;

	/* loop over tests */
	while (1) {

		/* make parameters */
		lx = 1 + 100 * franuni();
		ly = 1 + 100 * franuni();
		lz = 1 + 100 * franuni();
		ifx = -100 + 200 * franuni();
		ify = -100 + 200 * franuni();
		ifz = -100 + 200 * franuni();

		/* allocate space */
		x = alloc1float(lx);
		y = alloc1float(ly);
		z = alloc1float(lz);
		z1 = alloc1float(lz);

		/* fill x and y arrays */
		for (i = 0; i<lx; i++)
			x[i] = i + 1;
		for (i = 0; i<ly; i++)
			y[i] = i + 1;

		/* convolve and check */
		convolve_cwp1(lx, ifx, x, ly, ify, y, lz, ifz, z1);
		convolve_cwp(lx, ifx, x, ly, ify, y, lz, ifz, z);
		for (i = 0; i<lz; ++i)
			if (z[i] != z1[i]) break;
		if (i<lz) {
			printf("%10d %10d %10d %10d %10d %10d\n",
				lx, ifx, ly, ify, lz, ifz);
			printf("z1[%d]=%g != z[%d]=%g\n",
				i + ifz, z1[i], i + ifz, z[i]);
			pp1d(stdout, "simple", lz, ifz, z1);
			pp1d(stdout, "optimized", lz, ifz, z);
			exit(-1);
		}

		/* free space */
		free1float(x);
		free1float(y);
		free1float(z);
		free1float(z1);
	}
}
#endif






