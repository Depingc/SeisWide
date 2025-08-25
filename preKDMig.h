#pragma once
// #include "ealloc_d.h"

/* residual traveltime calculation based  on reference   time	*/
void resit(int nx, float fx, float dx, int nz, int nr, float dr,
	float **tb, float **t, float x0);

/* lateral interpolation	*/
/* sum of two tables	*/
void sum2(int nx, int nz, float a1, float a2, float **t1, float **t2, float **t);
void polint(float xa[], float ya[], int n, float x, float *y, float *dy, float *c, float *d);
void sum5(int nx, int nz, float ds, float xsInWin, float **t0, float **t1,
	float **t2, float **t3, float **t4, int ns, float **t);

/* compute  reference traveltime and slowness	*/
void timeb(int nr, int nz, float dr, float dz, float fz, float a,
	float v0, float **t, float **p, float **cs0, float **ang);

void filt(float *trace, int nt, float dt, float fmax, int ls, int m, float *trf);
/* Low-pass filter, integration and phase shift for input data	 
   input: 
    trace(nt)	single seismic trace
   fmax	high cut frequency
    ls		ls=1, line source; ls=0, point source
  output:
    trace(nt) 	filtered and phase-shifted seismic trace 
    tracei(nt) 	filtered, integrated and phase-shifted seismic trace 
 */

void mig2d(float *trace, int nt, float ft, float dt,
	float sx, float gx, float **mig, float aperx,
	int nx, float fx, float dx, int nz, float fz, float dz,
	int ls, int mtmax, float dxm, float fmax, float angmax,
	float **tb, float **pb, float **cs0b, float **angb, int nr,
	float **tsum, int nzt, float fzt, float dzt, int nxt, float fxt, float dxt,
	int npv, float **cssum, float **tvsum, float **mig1);
/*****************************************************************************
Migrate one trace 
******************************************************************************
Input:
*trace		one seismic trace 
nt		number of time samples in seismic trace
ft		first time sample of seismic trace
dt		time sampleing interval in seismic trace
sx,gx		lateral coordinates of source and geophone 
aperx		lateral aperature in migration
nx,fx,dx,nz,fz,dz	dimension parameters of migration region
ls		=1 for line source; =0 for point source
mtmax		number of time samples in triangle filter
dxm		midpoint sampling interval
fmax		frequency-highcut for input trace	 
angmax		migration angle aperature from vertical 	 
tb,pb,cs0b,angb		reference traveltime, lateral slowness, cosine of 
		incident angle, and emergent angle
nr		number of lateral samples in reference quantities
tsum		sum of residual traveltimes from shot and receiver
nxt,fxt,dxt,nzt,fzt,dzt		dimension parameters of traveltime table
npv=0		flag of computing quantities for velocity analysis
cssume		sum of cosine of emergence angles from shot and recerver 
tvsum		sum of  traveltime variations from shot and recerver 

Output:
mig		migrated section
mig1		additional migrated section for velocity analysis if npv>0
*****************************************************************************/
