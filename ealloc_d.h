#pragma once

#ifndef PI
#define PI 3.141592653589793f
#endif


/* Copyright (c) Colorado School of Mines, 1996.*/
/* All rights reserved.                       */

/*********************** self documentation **********************/
/*****************************************************************************
ALLOC - Allocate and free multi-dimensional arrays

alloc1		allocate a 1-d array
realloc1	re-allocate a 1-d array
free1		free a 1-d array
alloc2		allocate a 2-d array
free2		free a 2-d array
alloc3		allocate a 3-d array
free3		free a 3-d array
alloc4		allocate a 4-d array
free4		free a 4-d array
alloc1int	allocate a 1-d array of ints
realloc1int	re-allocate a 1-d array of ints
free1int	free a 1-d array of ints
alloc2int	allocate a 2-d array of ints
free2int	free a 2-d array of ints
alloc3int	allocate a 3-d array of ints
free3int	free a 3-d array of ints
alloc1float	allocate a 1-d array of floats
realloc1float	re-allocate a 1-d array of floats
free1float	free a 1-d array of floats
alloc2float	allocate a 2-d array of floats
free2float	free a 2-d array of floats
alloc3float	allocate a 3-d array of floats
free3float	free a 3-d array of floats
alloc1double	allocate a 1-d array of doubles
realloc1double	re-allocate a 1-d array of doubles
free1double	free a 1-d array of doubles
alloc2double	allocate a 2-d array of doubles
free2double	free a 2-d array of doubles
alloc3double	allocate a 3-d array of doubles
free3double	free a 3-d array of doubles
alloc1complex	allocate a 1-d array of complexs
realloc1complex	re-allocate a 1-d array of complexs
free1complex	free a 1-d array of complexs
alloc2complex	allocate a 2-d array of complexs
free2complex	free a 2-d array of complexs
alloc3complex	allocate a 3-d array of complexs
free3complex	free a 3-d array of complexs

******************************************************************************
Function Prototypes:
void *alloc1 (size_t n1, size_t size);
void *realloc1 (void *v, size_t n1, size_t size);
void free1 (void *p);
void **alloc2 (size_t n1, size_t n2, size_t size);
void free2 (void **p);
void ***alloc3 (size_t n1, size_t n2, size_t n3, size_t size);
void free3 (void ***p);
void ****alloc4 (size_t n1, size_t n2, size_t n3, size_t n4, size_t size);
void free4 (void ****p);
int *alloc1int (size_t n1);
int *realloc1int (int *v, size_t n1);
void free1int (int *p);
int **alloc2int (size_t n1, size_t n2);
void free2int (int **p);
int ***alloc3int (size_t n1, size_t n2, size_t n3);
void free3int (int ***p);
float *alloc1float (size_t n1);
float *realloc1float (float *v, size_t n1);
void free1float (float *p);
float **alloc2float (size_t n1, size_t n2);
void free2float (float **p);
float ***alloc3float (size_t n1, size_t n2, size_t n3);
void free3float (float ***p);
double *alloc1double (size_t n1);
double *realloc1double (double *v, size_t n1);
void free1double (double *p);
double **alloc2double (size_t n1, size_t n2);
void free2double (double **p);
double ***alloc3double (size_t n1, size_t n2, size_t n3);
void free3double (double ***p);
complex *alloc1complex (size_t n1);
complex *realloc1complex (complex *v, size_t n1);
void free1complex (complex *p);
complex **alloc2complex (size_t n1, size_t n2);
void free2complex (complex **p);
complex ***alloc3complex (size_t n1, size_t n2, size_t n3);
void free3complex (complex ***p);

******************************************************************************
Notes:
The functions defined below are intended to simplify manipulation
of multi-dimensional arrays in scientific programming in C.  These
functions are useful only because true multi-dimensional arrays
in C cannot have variable dimensions (as in FORTRAN).  For example,
the following function IS NOT valid in C:
	void badFunc(a,n1,n2)
	float a[n2][n1];
	{
		a[n2-1][n1-1] = 1.0;
	}
However, the following function IS valid in C:
	void goodFunc(a,n1,n2)
	float **a;
	{
		a[n2-1][n1-1] = 1.0;
	}
Therefore, the functions defined below do not allocate true
multi-dimensional arrays, as described in the C specification.
Instead, they allocate and initialize pointers (and pointers to 
pointers) so that, for example, a[i2][i1] behaves like a 2-D array.

The array dimensions are numbered, which makes it easy to add 
functions for arrays of higher dimensions.  In particular,
the 1st dimension of length n1 is always the fastest dimension,
the 2nd dimension of length n2 is the next fastest dimension,
and so on.  Note that the 1st (fastest) dimension n1 is the 
first argument to the allocation functions defined below, but 
that the 1st dimension is the last subscript in a[i2][i1].
(This is another important difference between C and Fortran.)

The allocation of pointers to pointers implies that more storage
is required than is necessary to hold a true multi-dimensional array.
The fraction of the total storage allocated that is used to hold 
pointers is approximately 1/(n1+1).  This extra storage is unlikely
to represent a significant waste for large n1.

The functions defined below are significantly different from similar 
functions described by Press et al, 1988, Numerical Recipes in C.
In particular, the functions defined below:
	(1) Allocate arrays of arbitrary size elements.
	(2) Allocate contiguous storage for arrays.
	(3) Return NULL if allocation fails (just like malloc).
	(4) Do not provide arbitrary lower and upper bounds for arrays.

Contiguous storage enables an allocated multi-dimensional array to
be passed to a C function that expects a one-dimensional array.
For example, to allocate and zero an n1 by n2 two-dimensional array
of floats, one could use
	a = alloc2(n1,n2,sizeof(float));
	zeroFloatArray(n1*n2,a[0]);
where zeroFloatArray is a function defined as
	void zeroFloatArray(int n, float *a)
	{
		int i;
		for (i=0; i<n; i++)
			a[i] = 0.0;
	}

Internal error handling and arbitrary array bounds, if desired,
should be implemented in functions that call the functions defined 
below, with the understanding that these enhancements may limit 
portability.

******************************************************************************
Author:    	Dave Hale, Colorado School of Mines, 12/31/89
*****************************************************************************/
/**************** end self doc ********************************/



/* allocate a 1-d array */
void *alloc1 (size_t n1, size_t size);
void *realloc1(void *v, size_t n1, size_t size);
void free1 (void *p);
void **alloc2 (size_t n1, size_t n2, size_t size);
void free2 (void **p);
void ***alloc3 (size_t n1, size_t n2, size_t n3, size_t size);
void free3 (void ***p);
void ****alloc4 (size_t n1, size_t n2, size_t n3, size_t n4, size_t size);
void free4 (void ****p);
int *alloc1int(size_t n1);
int *realloc1int(int *v, size_t n1);
void free1int(int *p);
int **alloc2int(size_t n1, size_t n2);
void free2int(int **p);
int ***alloc3int(size_t n1, size_t n2, size_t n3);
void free3int(int ***p);
float *alloc1float(size_t n1);

float *realloc1float(float *v, size_t n1);
void free1float(float *p);
float **alloc2float(size_t n1, size_t n2);
void free2float(float **p);
float ***alloc3float(size_t n1, size_t n2, size_t n3);
void free3float(float ***p);
double *alloc1double(size_t n1);
double *realloc1double(double *v, size_t n1);
void free1double(double *p);
double **alloc2double(size_t n1, size_t n2);
void free2double(double **p);
double ***alloc3double(size_t n1, size_t n2, size_t n3);
void free3double(double ***p);


typedef struct _complexStruct { /* complex number */
	float r, i;
}  cwp_complex;
typedef struct _dcomplexStruct { /* double-precision complex number */
	double r, i;
}  cwp_dcomplex;
#define complex cwp_complex
#define dcomplex cwp_dcomplex



//typedef struct _complexStruct { /* complex number */
//	float r,i;
//} complex;
//complex *alloc1complex(size_t n1);

complex *alloc1complex(size_t n1);
complex *realloc1complex(complex *v, size_t n1);
void free1complex(complex *p);
complex **alloc2complex(size_t n1, size_t n2);
void free2complex(complex **p);
complex ***alloc3complex(size_t n1, size_t n2, size_t n3);
void free3complex(complex ***p);

// Deping addition for VC++
CPoint *alloc1CPoint(size_t n1);
CPoint **alloc2CPoint(size_t n1, size_t n2);
void free1CPoint(CPoint *p);
void free2CPoint(CPoint **p);
DWORD *alloc1DWORD(size_t n1);
void free1DWORD(DWORD *p);

//COLORREF **alloc2COLORREF(size_t n1, size_t n2);
//void free2COLORREF(COLORREF **p);



/* Copyright (c) Colorado School of Mines, 1997.*/
/* All rights reserved.                       */

/*********************** self documentation **********************/
/*****************************************************************************
EALLOC - Allocate and free multi-dimensional arrays with error reports.

ealloc1			allocate a 1d array
erealloc1		reallocate a 1d array
ealloc2			allocate a 2d array
ealloc3			allocate a 3d array
ealloc4			allocate a 4d array
ealloc1int		allocate a 1d array of ints
erealloc1int		reallocate a 1d array of ints
ealloc2int		allocate a 2d array of ints
ealloc3int		allocate a 3d array of ints
ealloc1float		allocate a 1d array of floats
erealloc1float		reallocate a 1d array of floats
ealloc2float		allocate a 2d array of floats
ealloc3float		allocate a 3d array of floats
ealloc1double		allocate a 1d array of doubles
erealloc1double		reallocate a 1d array of doubles
ealloc2double		allocate a 2d array of doubles
ealloc3double		allocate a 3d array of doubles
ealloc1complex		allocate a 1d array of complex values	
erealloc1complex	reallocate a 1d array of complex values
ealloc2complex		allocate a 2d array of complex values
ealloc3complex		allocate a 3d array of complex values

*****************************************************************************
Function Prototypes:
void *ealloc1 (size_t n1, size_t size);
void *erealloc1 (void *v, size_t n1, size_t size);
void **ealloc2 (size_t n1, size_t n2, size_t size);
void ***ealloc3 (size_t n1, size_t n2, size_t n3, size_t size);
void ****ealloc4 (size_t n1, size_t n2, size_t n3, size_t n4, size_t size);
int *ealloc1int(size_t n1);
int *erealloc1int(int *v, size_t n1);
int **ealloc2int(size_t n1, size_t n2);
int ***ealloc3int(size_t n1, size_t n2, size_t n3);
float *ealloc1float(size_t n1);
float *erealloc1float(float *v, size_t n1);
float **ealloc2float(size_t n1, size_t n2);
float ***ealloc3float(size_t n1, size_t n2, size_t n3);
double *ealloc1double(size_t n1);
double *erealloc1double(double *v, size_t n1);
double **ealloc2double(size_t n1, size_t n2);
double ***ealloc3double(size_t n1, size_t n2, size_t n3);
complex *ealloc1complex(size_t n1);
complex *erealloc1complex(complex *v, size_t n1);
complex **ealloc2complex(size_t n1, size_t n2);
complex ***ealloc3complex(size_t n1, size_t n2, size_t n3);

*****************************************************************************
Notes:
These routines simply call those in ../../cwp/lib/alloc.c and issue
an error message via the syserr routine if the underlying malloc
came up empty.  See alloc.c for notes on the routines.

The routines in ../../cwp/lib/alloc.c were written by Dave Hale.

*****************************************************************************
Author: Jack Cohen, Center for Wave Phenomena
*****************************************************************************/
/**************** end self doc ********************************/

//#include "par.h"
//#define ERROR	NULL

/* allocate a 1-d array */
void *ealloc1 (size_t n1, size_t size);
void *erealloc1 (void *v, size_t n1, size_t size);
void **ealloc2 (size_t n1, size_t n2, size_t size);
void ***ealloc3 (size_t n1, size_t n2, size_t n3, size_t size);
void ****ealloc4 (size_t n1, size_t n2, size_t n3, size_t n4, size_t size);
int *ealloc1int(size_t n1);
int *ealloc1long(size_t n1);
int *erealloc1int(int *v, size_t n1);
int **ealloc2int(size_t n1, size_t n2);
int ***ealloc3int(size_t n1, size_t n2, size_t n3);
float *ealloc1float(size_t n1);
float *erealloc1float(float *v, size_t n1);
float **ealloc2float(size_t n1, size_t n2);
float ***ealloc3float(size_t n1, size_t n2, size_t n3);
double *ealloc1double(size_t n1);
double *erealloc1double(double *v, size_t n1);
double **ealloc2double(size_t n1, size_t n2);
double ***ealloc3double(size_t n1, size_t n2, size_t n3);
complex *ealloc1complex(size_t n1);
complex *erealloc1complex(complex *v, size_t n1);
complex **ealloc2complex(size_t n1, size_t n2);
complex ***ealloc3complex(size_t n1, size_t n2, size_t n3);


/* TYPEDEFS */
typedef union { /* storage for arbitrary type */
	char s[8];
	short h;
	unsigned short u;
	long l;
	unsigned long v;
	int i;
	unsigned int p;
	float f;
	double d;
	unsigned int U : 16;
	unsigned int P : 32;
} Value;


/* complex number manipulation */
complex cadd(complex a, complex b);
complex csub(complex a, complex b);
complex cmul(complex a, complex b);
complex cdiv(complex a, complex b);
complex cmplx(float re, float im);
complex conjg(complex z);
complex cneg(complex z);
complex cinv(complex z);
complex cwp_csqrt(complex z);
complex cwp_cexp(complex z);
complex crmul(complex a, float x);
float rcabs(complex z);

dcomplex *alloc1dcomplex(size_t n1);
dcomplex *realloc1dcomplex(dcomplex *v, size_t n1);
dcomplex **alloc2dcomplex(size_t n1, size_t n2);
dcomplex ***alloc3dcomplex(size_t n1, size_t n2, size_t n3);

void free1dcomplex(dcomplex *p);
void free2dcomplex(dcomplex **p);
void free3dcomplex(dcomplex ***p);
