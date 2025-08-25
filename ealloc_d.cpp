#include "stdafx.h"
#include "ealloc_d.h"
#include "xallocator.h"



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
void *alloc1 (size_t n1, size_t size)
{
	// One strange thing: 		
	//  bool *isFFIDAbnormal = (bool*)alloc1(nx, 1);   // isFFIDAbnormal[i] caused exception sometimes and is unstable. Cannot solve it. So do not use it for bool type!!!

	void *p;

	if ((p=xmalloc(n1*size))==NULL)
		return NULL;
	return p;
}

/* re-allocate a 1-d array */
void *realloc1(void *v, size_t n1, size_t size)
{
	void *p;

	if ((p=xrealloc(v,n1*size))==NULL)
		return NULL;
	return p;
}

/* free a 1-d array */
void free1 (void *p)
{
	xfree(p);
	p = NULL;

}

/* allocate a 2-d array */
void **alloc2 (size_t n1, size_t n2, size_t size)
{
	size_t i2;
	void **p;

	if ((p=(void**)xmalloc(n2*sizeof(void*)))==NULL) 
		return NULL;
	if ((p[0]=(void*)xmalloc(n2*n1*size))==NULL) {
		xfree(p);
		return NULL;
	}
	for (i2=0; i2<n2; i2++)
		p[i2] = (char*)p[0]+size*n1*i2;
	return p;
}

/* free a 2-d array */
void free2 (void **p)
{
	if (p!=NULL) xfree(p[0]);
	xfree(p);
	p = NULL;
}

/* allocate a 3-d array */
void ***alloc3 (size_t n1, size_t n2, size_t n3, size_t size)
{
	size_t i3,i2;
	void ***p;

	if ((p=(void***)xmalloc(n3*sizeof(void**)))==NULL)
		return NULL;
	if ((p[0]=(void**)xmalloc(n3*n2*sizeof(void*)))==NULL) {
		xfree(p);
		return NULL;
	}
	if ((p[0][0]=(void*)xmalloc(n3*n2*n1*size))==NULL) {
		xfree(p[0]);
		xfree(p);
		return NULL;
	}
	for (i3=0; i3<n3; i3++) {
		p[i3] = p[0]+n2*i3;
		for (i2=0; i2<n2; i2++)
			p[i3][i2] = (char*)p[0][0]+size*n1*(i2+n2*i3);
	}
	return p;
}

/* free a 3-d array */
void free3 (void ***p)
{
	if (p!=NULL && p[0]!=NULL) xfree(p[0][0]);
	if (p!=NULL) xfree(p[0]);
	xfree(p);
	p = NULL;
}

/* allocate a 4-d array */
void ****alloc4 (size_t n1, size_t n2, size_t n3, size_t n4, size_t size)
{
	size_t i4,i3,i2;
	void ****p;

	if ((p=(void****)xmalloc(n4*sizeof(void***)))==NULL)
		return NULL;
	if ((p[0]=(void***)xmalloc(n4*n3*sizeof(void**)))==NULL) {
		xfree(p);
		return NULL;
	}
	if ((p[0][0]=(void**)xmalloc(n4*n3*n2*sizeof(void*)))==NULL) {
		xfree(p[0]);
		xfree(p);
		return NULL;
	}
	if ((p[0][0][0]=(void*)xmalloc(n4*n3*n2*n1*size))==NULL) {
		xfree(p[0][0]);
		xfree(p[0]);
		xfree(p);
		return NULL;
	}
	for (i4=0; i4<n4; i4++) {
		p[i4] = p[0]+i4*n3;
		for (i3=0; i3<n3; i3++) {
			p[i4][i3] = p[0][0]+n2*(i3+n3*i4);
			for (i2=0; i2<n2; i2++)
				p[i4][i3][i2] = (char*)p[0][0][0]+
						size*n1*(i2+n2*(i3+n3*i4));
		}
	}
	return p;
}

/* free a 4-d array */
void free4 (void ****p)
{
	if (p!=NULL && p[0]!=NULL && p[0][0]!=NULL) xfree(p[0][0][0]);
	if (p!=NULL && p[0]!=NULL) xfree(p[0][0]);
	if (p!=NULL && p[0]!=NULL) xfree(p[0]);
	xfree(p);
	p = NULL;
}

/* allocate a 1-d array of ints */
int *alloc1int(size_t n1)
{
	return (int*)alloc1(n1,sizeof(int));
}
int *alloc1long(size_t n1)
{
	return (int*)alloc1(n1, sizeof(long));
}

/* re-allocate a 1-d array of ints */
int *realloc1int(int *v, size_t n1)
{
	return (int*)realloc1(v,n1,sizeof(int));
}

/* free a 1-d array of ints */
void free1int(int *p)
{
	free1(p);
}

/* allocate a 2-d array of ints */
int **alloc2int(size_t n1, size_t n2)
{
	return (int**)alloc2(n1,n2,sizeof(int));
}

/* free a 2-d array of ints */
void free2int(int **p)
{
	free2((void**)p);
}

/* allocate a 3-d array of ints */
int ***alloc3int(size_t n1, size_t n2, size_t n3)
{
	return (int***)alloc3(n1,n2,n3,sizeof(int));
}

/* free a 3-d array of ints */
void free3int(int ***p)
{
	free3((void***)p);
}

/* allocate a 1-d array of floats */
float *alloc1float(size_t n1)
{
	return (float*)alloc1(n1,sizeof(float));
}

/* re-allocate a 1-d array of floats */
float *realloc1float(float *v, size_t n1)
{
	return (float*)realloc1(v,n1,sizeof(float));
}

/* free a 1-d array of floats */
void free1float(float *p)
{
	free1(p);
}

/* allocate a 2-d array of floats */
float **alloc2float(size_t n1, size_t n2)
{
	return (float**)alloc2(n1,n2,sizeof(float));
}

/* free a 2-d array of floats */
void free2float(float **p)
{
	free2((void**)p);
}

/* allocate a 3-d array of floats */
float ***alloc3float(size_t n1, size_t n2, size_t n3)
{
	return (float***)alloc3(n1,n2,n3,sizeof(float));
}

/* free a 3-d array of floats */
void free3float(float ***p)
{
	free3((void***)p);
}

/* allocate a 1-d array of doubles */
double *alloc1double(size_t n1)
{
	return (double*)alloc1(n1,sizeof(double));
}

/* re-allocate a 1-d array of doubles */
double *realloc1double(double *v, size_t n1)
{
	return (double*)realloc1(v,n1,sizeof(double));
}


/* free a 1-d array of doubles */
void free1double(double *p)
{
	free1(p);
}

/* allocate a 2-d array of doubles */
double **alloc2double(size_t n1, size_t n2)
{
	return (double**)alloc2(n1,n2,sizeof(double));
}

/* free a 2-d array of doubles */
void free2double(double **p)
{
	free2((void**)p);
}

/* allocate a 3-d array of doubles */
double ***alloc3double(size_t n1, size_t n2, size_t n3)
{
	return (double***)alloc3(n1,n2,n3,sizeof(double));
}
 
/* free a 3-d array of doubles */
void free3double(double ***p)
{
	free3((void***)p);
}





/* allocate a 1-d array of complexs */
complex *alloc1complex(size_t n1)
{
	return (complex*)alloc1(n1,sizeof(complex));
}

/* re-allocate a 1-d array of complexs */
complex *realloc1complex(complex *v, size_t n1)
{
	return (complex*)realloc1(v,n1,sizeof(complex));
}

/* free a 1-d array of complexs */
void free1complex(complex *p)
{
	free1(p);
}

/* allocate a 2-d array of complexs */
complex **alloc2complex(size_t n1, size_t n2)
{
	return (complex**)alloc2(n1,n2,sizeof(complex));
}

/* free a 2-d array of complexs */
void free2complex(complex **p)
{
	free2((void**)p);
}

/* allocate a 3-d array of complexs */
complex ***alloc3complex(size_t n1, size_t n2, size_t n3)
{
	return (complex***)alloc3(n1,n2,n3,sizeof(complex));
}

/* free a 3-d array of complexs */
void free3complex(complex ***p)
{
	free3((void***)p);
}




// Deping addition for VC++
CPoint *alloc1CPoint(size_t n1)
{
	return (CPoint*)alloc1(n1, sizeof(CPoint));
}

/* allocate a 2-d array of CPoint */
CPoint **alloc2CPoint(size_t n1, size_t n2)
{
	return (CPoint**)alloc2(n1,n2,sizeof(CPoint));
}

void free1CPoint(CPoint *p)
{
	free1((void*)p);
}

/* free a 2-d array of floats */
void free2CPoint(CPoint **p)
{
	free2((void**)p);
}

DWORD *alloc1DWORD(size_t n1)
{
	return (DWORD*)alloc1(n1,sizeof(DWORD));
}
void free1DWORD(DWORD *p)
{
	free1((void*)p);
}


//COLORREF **alloc2COLORREF(size_t n1, size_t n2)
//{
//	return (COLORREF**)alloc2(n1,n2,sizeof(COLORREF));
//}
//void free2COLORREF(COLORREF **p)
//{
//	free2((void**)p);
//}



// copied from complex.c file:
complex cadd(complex a, complex b)
{
	complex c;
	c.r = a.r+b.r;
	c.i = a.i+b.i;
	return c;
}

complex csub(complex a, complex b)
{
	complex c;
	c.r = a.r-b.r;
	c.i = a.i-b.i;
	return c;
}

complex cmul(complex a, complex b)
{
	complex c;
	c.r = a.r*b.r-a.i*b.i;
	c.i = a.i*b.r+a.r*b.i;
	return c;
}

complex cdiv(complex a, complex b)
{
	complex c;
	float r,den;
	if (fabs(b.r)>=fabs(b.i)) {
		r = b.i/b.r;
		den = b.r+r*b.i;
		c.r = (a.r+r*a.i)/den;
		c.i = (a.i-r*a.r)/den;
	} else {
		r = b.r/b.i;
		den = b.i+r*b.r;
		c.r = (a.r*r+a.i)/den;
		c.i = (a.i*r-a.r)/den;
	}
	return c;
}

complex cmplx(float re, float im)
{
	complex c;
	c.r = re;
	c.i = im;
	return c;
}

complex conjg(complex z)
{
	complex c;
	c.r = z.r;
	c.i = -z.i;
	return c;
}

complex cneg(complex z)
{
	complex c;
	c.r = -z.r;
	c.i = -z.i;
	return c;
}

complex cinv(complex z)
{
	complex c;
	float s;
	s = 1.0f/(z.r*z.r+z.i*z.i);
	c.r = z.r*s;
	c.i = -z.i*s;
	return c;
}

complex cwp_csqrt(complex z)
{
	complex c;
	float x,y,w,r;
	if (z.r==0.0 && z.i==0.0) {
		c.r = c.i = 0.0;
		return c;
	} else {
		x = (float)fabs(z.r);
		y = (float)fabs(z.i);
		if (x>=y) {
			r = y/x;
			w = (float)sqrt(x)*(float)sqrt(0.5*(1.0+ sqrt(1.0f+r*r)));
		} else {
			r = x/y;
			w = (float)sqrt(y)*(float)sqrt(0.5*(r+ sqrt(1.0f+r*r)));
		}
		if (z.r>=0.0) {
			c.r = w;
			c.i = z.i/(2.0f*w);
		} else {
			c.i = (z.i>=0.0f) ? w : -w;
			c.r = z.i/(2.0f*c.i);
		}
		return c;
	}
}

complex cwp_cexp(complex z)
{
	float a;
	complex c;
	a = (float)exp(z.r);
	c.r = a*(float)cos(z.i);
	c.i = a*(float)sin(z.i);
	return c;
}

complex crmul(complex a, float x)
{
	complex c;
	c.r = x*a.r;
	c.i = x*a.i;
	return c;
}

float rcabs(complex z)
{
	float x, y, ans, temp;
	x = (float)fabs(z.r);
	y = (float)fabs(z.i);
	if (x == 0.0f)
		ans = y;
	else if (y == 0.0f)
		ans = x;
	else if (x>y) {
		temp = y / x;
		ans = x*(float)sqrt(1.0 + temp*temp);
	}
	else {
		temp = x / y;
		ans = y*(float)sqrt(1.0 + temp*temp);
	}
	return ans;
}




#ifdef TEST
main()
{
	short   *hv, **hm;
	int     *iv, **im;
	float   *fv, **fm;
	double  *dv, **dm;
	size_t i1, i2;
	size_t n1, n2;

	scanf("%d %*[^\n]", &n1);
	scanf("%d %*[^\n]", &n2);

	/* Exercise 1-d routines */
	hv = (short *) alloc1(n1, sizeof(short));
	iv = alloc1int(n1);
	fv = alloc1float(n1);
	dv = alloc1double(n1);

	for (i1 = 0; i1 < n1; ++i1) {
		hv[i1] = i1;
		iv[i1] = i1;
		fv[i1]  = (float) i1;
		dv[i1] = (double) i1;
	}

	printf("short vector:\n");
	for (i1 = 0; i1 < n1; ++i1) {
		printf("hv[%d] = %hd\n", i1, hv[i1]);
	}
	putchar('\n');

	printf("int vector:\n");
	for (i1 = 0; i1 < n1; ++i1) {
		printf("iv[%d] = %d\n", i1, iv[i1]);
	}
	putchar('\n');

	printf("float vector:\n");
	for (i1 = 0; i1 < n1; ++i1) {
		printf("fv[%d] = %f\n", i1, fv[i1]);
	}
	putchar('\n');

	printf("double vector:\n");
	for (i1 = 0; i1 < n1; ++i1) {
		printf("dv[%d] = %lf\n", i1, dv[i1]);
	}
	putchar('\n');


	free1(hv);
	free1int(iv);
	free1float(fv);
	free1double(dv);


	/* Exercise 2-d routines */
	hm = (short *) alloc2(n1, n2, sizeof(short));
	im = alloc2int(n1, n2);
	fm = alloc2float(n1, n2);
	dm = alloc2double(n1, n2);

	for (i2 = 0; i2 < n2; ++i2) {
		for (i1 = 0; i1 < n1; ++i1) {
			hm[i2][i1] = i1 + 2*i2;
			im[i2][i1] = i1 + 2*i2;
			fm[i2][i1] = (float) (i1 + 2*i2);
			dm[i2][i1] = (double) (i1 + 2*i2);
		}
	}

	printf("short matrix:\n");
	for (i2 = 0; i2 < n2; ++i2) {
		for (i1 = 0; i1 < n1; ++i1) {
			printf("hm[%d, %d] = %hd ", i2, i1, hm[i2][i1]);
		}
		putchar('\n');
	}
	putchar('\n');

	printf("int matrix:\n");
	for (i2 = 0; i2 < n2; ++i2) {
		for (i1 = 0; i1 < n1; ++i1) {
			printf("im[%d, %d] = %d ", i2, i1, im[i2][i1]);
		}
		putchar('\n');
	}
	putchar('\n');

	printf("float matrix:\n");
	for (i2 = 0; i2 < n2; ++i2) {
		for (i1 = 0; i1 < n1; ++i1) {
			printf("fm[%d, %d] = %f ", i2, i1, fm[i2][i1]);
		}
		putchar('\n');
	}
	putchar('\n');

	printf("double matrix:\n");
	for (i2 = 0; i2 < n2; ++i2) {
		for (i1 = 0; i1 < n1; ++i1) {
			printf("dm[%d, %d] = %lf ", i2, i1, dm[i2][i1]);
		}
		putchar('\n');
	}
	putchar('\n');

	free2(hm);
	free2int(im);
	free2float(fm);
	free2double(dm);

	exit(0);
}
#endif















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
void *ealloc1 (size_t n1, size_t size)
{
	void *p;

	if (ERROR == (p=alloc1(n1, size)))
	//	syserr("%s: malloc failed", __FILE__);
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* re-allocate a 1-d array */
void *erealloc1 (void *v, size_t n1, size_t size)
{
	void *p;

	if (ERROR == (p=realloc1(v, n1, size)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 2-d array */
void **ealloc2 (size_t n1, size_t n2, size_t size)
{
	void **p;

	if (ERROR == (p=alloc2(n1, n2, size)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 3-d array */
void ***ealloc3 (size_t n1, size_t n2, size_t n3, size_t size)
{
	void ***p;

	if (ERROR == (p=alloc3(n1, n2, n3, size)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 4-d array */
void ****ealloc4 (size_t n1, size_t n2, size_t n3, size_t n4, size_t size)
{
	void ****p;

	if (ERROR == (p=alloc4(n1, n2, n3, n4, size)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 1-d array of ints */
int *ealloc1int(size_t n1)
{
	int *p;

	if (ERROR == (p=alloc1int(n1)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}

/* allocate a 1-d array of ints */
int *ealloc1long(size_t n1)
{
	int *p;

	if (ERROR == (p = alloc1long(n1)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}

/* re-allocate a 1-d array of ints */
int *erealloc1int(int *v, size_t n1)
{
	int *p;

	if (ERROR == (p=realloc1int(v,n1)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 2-d array of ints */
int **ealloc2int(size_t n1, size_t n2)
{
	int **p;

	if (ERROR == (p=alloc2int(n1, n2)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 3-d array of ints */
int ***ealloc3int(size_t n1, size_t n2, size_t n3)
{
	int ***p;

	if (ERROR == (p=alloc3int(n1, n2, n3)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 1-d array of floats */
float *ealloc1float(size_t n1)
{
	float *p;

	if (ERROR == (p=alloc1float(n1)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* re-allocate a 1-d array of floats */
float *erealloc1float(float *v, size_t n1)
{
	float *p;

	if (ERROR == (p=realloc1float(v, n1)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 2-d array of floats */
float **ealloc2float(size_t n1, size_t n2)
{
	float **p;

	if (ERROR == (p=alloc2float(n1, n2)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 3-d array of floats */
float ***ealloc3float(size_t n1, size_t n2, size_t n3)
{
	float ***p;

	if (ERROR == (p=alloc3float(n1, n2, n3)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 1-d array of doubles */
double *ealloc1double(size_t n1)
{
	double *p;

	if (ERROR == (p=alloc1double(n1)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* re-allocate a 1-d array of doubles */
double *erealloc1double(double *v, size_t n1)
{
	double *p;

	if (ERROR == (p=realloc1double(v, n1)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 2-d array of doubles */
double **ealloc2double(size_t n1, size_t n2)
{
	double **p;

	if (ERROR == (p=alloc2double(n1, n2)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 3-d array of doubles */
double ***ealloc3double(size_t n1, size_t n2, size_t n3)
{
	double ***p;

	if (ERROR == (p=alloc3double(n1, n2, n3)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 1-d array of complexs */
complex *ealloc1complex(size_t n1)
{
	complex *p;

	if (ERROR == (p=alloc1complex(n1)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* re-allocate a 1-d array of complexs */
complex *erealloc1complex(complex *v, size_t n1)
{
	complex *p;

	if (ERROR == (p=realloc1complex(v, n1)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 2-d array of complexs */
complex **ealloc2complex(size_t n1, size_t n2)
{
	complex **p;

	if (ERROR == (p=alloc2complex(n1, n2)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}


/* allocate a 3-d array of complexs */
complex ***ealloc3complex(size_t n1, size_t n2, size_t n3)
{
	complex ***p;

	if (ERROR == (p=alloc3complex(n1, n2, n3)))
		AfxMessageBox(_T("Memory allocation failed"));
	return p;
}



#ifdef TEST
main()
{
	short   *hv, **hm;
	int     *iv, **im;
	float   *fv, **fm;
	double  *dv, **dm;
	size_t i1, i2;
	size_t n1, n2;

	scanf("%d %*[^\n]", &n1);
	scanf("%d %*[^\n]", &n2);

	/* Exercise 1-d routines */
	hv = (short *) ealloc1(n1, sizeof(short));
	iv = ealloc1int(n1);
	fv = ealloc1float(n1);
	dv = ealloc1double(n1);

	for (i1 = 0; i1 < n1; ++i1) {
		hv[i1] = i1;
		iv[i1] = i1;
		fv[i1]  = (float) i1;
		dv[i1] = (double) i1;
	}

	printf("short vector:\n");
	for (i1 = 0; i1 < n1; ++i1) {
		printf("hv[%d] = %hd\n", i1, hv[i1]);
	}
	putchar('\n');

	printf("int vector:\n");
	for (i1 = 0; i1 < n1; ++i1) {
		printf("iv[%d] = %d\n", i1, iv[i1]);
	}
	putchar('\n');

	printf("float vector:\n");
	for (i1 = 0; i1 < n1; ++i1) {
		printf("fv[%d] = %f\n", i1, fv[i1]);
	}
	putchar('\n');

	printf("double vector:\n");
	for (i1 = 0; i1 < n1; ++i1) {
		printf("dv[%d] = %lf\n", i1, dv[i1]);
	}
	putchar('\n');


	free1(hv);
	free1int(iv);
	free1float(fv);
	free1double(dv);


	/* Exercise 2-d routines */
	hm = (short **) ealloc2(n1, n2, sizeof(short));
	im = ealloc2int(n1, n2);
	fm = ealloc2float(n1, n2);
	dm = ealloc2double(n1, n2);

	for (i2 = 0; i2 < n2; ++i2) {
		for (i1 = 0; i1 < n1; ++i1) {
			hm[i2][i1] = i1 + 2*i2;
			im[i2][i1] = i1 + 2*i2;
			fm[i2][i1] = (float) (i1 + 2*i2);
			dm[i2][i1] = (double) (i1 + 2*i2);
		}
	}

	printf("short matrix:\n");
	for (i2 = 0; i2 < n2; ++i2) {
		for (i1 = 0; i1 < n1; ++i1) {
			printf("hm[%d, %d] = %hd ", i2, i1, hm[i2][i1]);
		}
		putchar('\n');
	}
	putchar('\n');

	printf("int matrix:\n");
	for (i2 = 0; i2 < n2; ++i2) {
		for (i1 = 0; i1 < n1; ++i1) {
			printf("im[%d, %d] = %d ", i2, i1, im[i2][i1]);
		}
		putchar('\n');
	}
	putchar('\n');

	printf("float matrix:\n");
	for (i2 = 0; i2 < n2; ++i2) {
		for (i1 = 0; i1 < n1; ++i1) {
			printf("fm[%d, %d] = %f ", i2, i1, fm[i2][i1]);
		}
		putchar('\n');
	}
	putchar('\n');

	printf("double matrix:\n");
	for (i2 = 0; i2 < n2; ++i2) {
		for (i1 = 0; i1 < n1; ++i1) {
			printf("dm[%d, %d] = %lf ", i2, i1, dm[i2][i1]);
		}
		putchar('\n');
	}
	putchar('\n');

	free2((void*)hm);
	free2int(im);
	free2float(fm);
	free2double(dm);

	return EXIT_SUCCESS;
}
#endif
