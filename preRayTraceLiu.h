#pragma once
#include "ealloc_d.h"

/* define initial value for traveltime as an arbitrary large number */
#define INITIAL_T 999999

/* Structures used internally */
/* one  ray */
typedef struct RayStruct {
	float t;		/* time */
	float px,pz;		/* slowness componets */
	float x,z;		/* x,z coordinates */
	float q2,p2;	/* Cerveny's dynamic ray tracing solution */
	float v,dvdx,dvdz;	/* velocity and its derivatives */
	float dtv;	/* traveltime variation */
} Ray;

/* ray tracing geometry paramters */
typedef struct RaytrStruct {
	float xs;		/* source lateral location */	 
	float zs;		/* source depth location */	 
	int nt;			/* number of time samples in ray tracing */
	float dt;		/* time sample interval in ray tracing  */	 
	int na;			/* number of rays */
	float fa;		/* first take-off angle of rays */
	float da;		/* increment of take-off angles */
	float amin;		/* minimum emergence angle  */	 
	float amax;		/* maximum emergence angle  */	 
	float fac;		/* factor to determine extrapolation radius*/ 
} Raytr;

/* 2D section geometry parameters, moved to CSeisWideDoc.h */
typedef struct Geo2DStruct {
	int nx;			/* number of lateral samples  */
	float fx;		/* first lateral sample  */	 
	float dx;		/* lateral interval   */	 
	float odx;		/* 1 over dx   */	 
	int nz;			/* number of depth samples */
	float fz;		/* first depth sample  */	 
	float dz;		/* depth interval  */	
	float odz;		/* 1 over dz   */	 
}Geo2d;

void vintp(Geo2d geo2dv, float *v, float x, float z, float *u);
/*************************************************************************
 Function to support interpolation of a single fuction 
*************************************************************************
Input:
x,z		2D coordinate at which to interpolate
v		array(nz,nx) on unoform grids 
Output: 
u		v(x,z)
*************************************************************************
Author: Zhenyue Liu, CSM 1995
*************************************************************************/

void vel2Interp(Geo2d geo2dv, float *v, float *vxx, float *vxz, float *vzz,
	float x, float z,
	float *u, float *ux, float *uz, float *uxx, float *uxz, float *uzz);
/*************************************************************************
vel2Interp - Function to support interpolation of velocity and its
		derivatives	 
*************************************************************************
Input:
x,z		2D coordinate at which to interpolate 
v		velocity array(nz,nx) on unoform grids 
vxx,vxz,vzz	second derivaitve arrays(nz,nx) on uniform grids 

Output:  
u 	v(x,z)
ux  	dv/dx
uz  	dv/dz
uxx	ddv/dxdx
uxz 	ddv/dzdx
uzz	ddv/dzdz
*************************************************************************
Author: Zhenyue Liu, CSM 1995
*************************************************************************/

void eiknl(Geo2d geo2dt, float *t, float *ov2, float tmax);
/* 	compute traveltime in shadow zones by eikonal equation
  input
   t 		traveltimes form ray tracing
   ov2 		slowness squares
   tmax	 	upper limit of ordinary traveltime value
  output:
   t	traveltime (unchanged if t<=tmax)
*/

void dv2(Geo2d geo2dv, float *v, float *vxx, float *vxz, float *vzz);
/*calculate second DeriVatives from a 2D VELocity grid by finite difference*/
/*  	input: 	velocity v 
  	output: vxx,vxz and vzz  */	 

void voint(Geo2d geo2dv, float *v, Geo2d geo2dt, float *ov2, int npv, float *vo);

void ddt(float p, float q, float c, float s, float *dv, float v, float *d2t,
	float *cuv);
/************************************************************************
ddt - calculate second derivatives of traveltime with respect to x,y
************************************************************************/

void trans(int nz, int nxt, int nx0, float *v, float *vt);

/***********************************************************************/
void dfrungk(float v, float vx, float vz, float vxx, float vxz, float vzz,
	float x, float z, float px, float pz, float p2, float q2,
	float *dx, float *dz, float *dpx, float *dpz, float *dp2, float *dq2);

/************************ c = a+h*b ***********************************/
void sum2(float h, float a1, float a2, float a3, float a4, float a5, float a6,
	float b1, float b2, float b3, float b4, float b5, float b6,
	float *c1, float *c2, float *c3, float *c4, float *c5, float *c6);

void makeRay(Geo2d geo2dv, float *vel, float *vxx, float *vxz, float *vzz,
	Raytr raytr, float a0, int *nrs, Ray *ray, int npv, float *pv);
/*****************************************************************************
 Trace rays in uniformly sampled v(nz,nx)
*****************************************************************************
Input:
geo2dv		grid parameters of velocity 
v2		sampled velocity array
raytr		geometry parameters of ray tracing 
vxx,vxz,vzz	sampled second derivatives of velocity  
a0  		shooting angles at source point
npv		flag of velocity analysis
pv		velocity variation if npv>0

Output:
ray		pointer to ray parameters sampled at discrete ray steps
nrs		number of points on ray 
*****************************************************************************
Note:
The ray ends when it runs out of time or with the first step that is out
of the velocity boudary or out of emergence angle boundary
*****************************************************************************
Author: Zhenyue Liu, CSM, 1995.
****************************************************************************/ 

void raytime2d(Raytr raytr, Geo2d geo2dv, float *v, Geo2d geo2dt, float *t,
	int npv, float *vo, float *pv, float *tv, float *cs);
			   /****************************************************************************
			   raytime2d - calculate traveltimes by paraxial ray tracing
			   *****************************************************************************
			   Input:
			   geo2dt		grid parameters of traveltime 
			   geo2dv		grid parameters of velocity 
			   v2		sampled velocity array
			   raytr		geometry parameters of ray tarcing 
			   
				 Output:
				 t		traveltime 
				 *****************************************************************************
				 Note: when traveltime has multiple values, the one has the shortest 
				 ray path is chosen.
				 *****************************************************************************
				 Author: Zhenyue Liu, CSM 1995.
				 ****************************************************************************/ 

int Lius2draytracingMod(float *v, Geo2d geo2dv, Geo2d geo2dt,
	int nt, float dt, float fs, float ds, float aperx,
	float fa, int na, float da, float amin, float amax,
	float fac, float sourceDepth, int ek, int npv, FILE *jpfp, CString rayName);

BOOL Lius2draytracingSingle(float *v, Geo2d geo2dv, Geo2d geo2dt,
	int nt, float dt, float fxs, float *t, float aperx,
	float fa, int na, float da, float amin, float amax,
	float fac, float sourceDepth, int ek, int npv, FILE *jpfp);
