#pragma once
#include "ealloc_d.h"


//#ifndef PI
//#define PI (3.141592653589793f)
//#endif
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif
#ifndef YES
#define YES (1)
#endif
#ifndef NO
#define NO (0)
#endif
#ifndef ABS
#define ABS(x) ((x) < 0 ? -(x) : (x))
#endif
#ifndef MAX
#define	MAX(x,y) ((x) > (y) ? (x) : (y))
#endif
#ifndef MIN
#define	MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#define NINT(x) ((int)((x)>0.0?(x)+0.5:(x)-0.5))
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

void stoepd (int n, double r[], double g[], double f[], double a[]);
void stoepf (int n, float r[], float g[], float f[], float a[]);

float fsinc (float x);
double dsinc (double x);

void intt8c (int ntable, float table[][8],
	int nxin, float dxin, float fxin, complex yin[],
	complex yinl, complex yinr, int nxout, float xout[], complex yout[]);
void intt8r (int ntable, float table[][8],
	int nxin, float dxin, float fxin, float yin[], float yinl, float yinr,
	int nxout, float xout[], float yout[]);
void mksinc (float d, int lsinc, float sinc[]);

/* these are used by both ints8c and ints8r */
#define LTABLE 8
#define NTABLE 513

void ints8c (int nxin, float dxin, float fxin, complex yin[], 
	complex yinl, complex yinr, int nxout, float xout[], complex yout[]);
void ints8r (int nxin, float dxin, float fxin, float yin[], 
	float yinl, float yinr, int nxout, float xout[], float yout[]);
void ress8c (int nxin, float dxin, float fxin, complex yin[], 
	complex yinl, complex yinr, 
	int nxout, float dxout, float fxout, complex yout[]);
void ress8r (int nxin, float dxin, float fxin, float yin[], 
	float yinl, float yinr, 
	int nxout, float dxout, float fxout, float yout[]);

//
//int npfao (int nmin, int nmax);
//int npfaro (int nmin, int nmax);
//void pfacc(int isign, int n, complex cz[]);
//void pfacr(int isign, int n, complex cz[], float rz[]);
//void pfa2cc(int isign, int idim, int n1, int n2, complex cz[]);
//void pfa2cr(int isign, int idim, int n1, int n2, complex cz[], float rz[]);
//void pfa2rc(int isign, int idim, int n1, int n2, float rz[], complex cz[]);
//void pfamcc(int isign, int n, int nt, int k, int kt, complex cz[]);


void xindex(int nx, float ax[], float x, int *index);
void mig1k (float k, float fmax, float speed, int nt, float dt, 
	float *v, complex *p, complex *q);

#ifndef TATable_H
	#define TATable_H
	typedef struct TATableStruct {
		int np;
		float *p;
		float *taumax;
		float *taumin;
		float *tturn;
		float **t;
		float **a;
	} TATable;
#endif

void mig1kPS (TATable *table, float k, float ffil[4], int ntflag,
	int nt, float dt, complex *cp,
	int ntau, float dtau, complex *cq);
TATable *tableta (int np, int ntau, float dtau, float vtau[],
	int nt, float dt);
