#include "stdafx.h"
#include "preRayTraceLiu.h"
#include "ealloc_d.h"

void vintp(Geo2d geo2dv,float *v,float x,float z,float *u) 
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
{

	int k0,k1,k2,k3,jx,jz,nx=geo2dv.nx,nz=geo2dv.nz;
	float fx=geo2dv.fx,odx=geo2dv.odx;
	float fz=geo2dv.fz,odz=geo2dv.odz;
	float ax,az,sx,sz;

/*	determine interpolate coefficients	*/
    ax = (x-fx)*odx;
    jx = (int)ax;
    if(jx<0) jx = 0;
    if(jx>nx-2) jx = nx-2;
    sx = ax-jx;
    az = (z-fz)*odz;
    jz = (int)az;
    if(jz<0) jz = 0;
    if(jz>nz-2) jz = nz-2;
    sz = az-jz;

/*	set the table of indices for interpolation	*/
    k0 = nz*jx+jz;
    k1 = k0+1;
    k2 = k0+nz;
    k3 = k2+1;

/*	interpolation	*/
    *u = (1.0f-sx)*((1.0f-sz)*v[k0]+sz*v[k1])
   			+sx*((1.0f-sz)*v[k2]+sz*v[k3]);

}

void vel2Interp(Geo2d geo2dv,float *v,float *vxx,float *vxz,float *vzz,
     float x,float z,
     float *u,float *ux,float *uz,float *uxx,float *uxz,float *uzz) 
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
{
	int k0,k1,k2,k3,jx,jz,nx=geo2dv.nx,nz=geo2dv.nz;
	float fx=geo2dv.fx,dx=geo2dv.dx,odx=geo2dv.odx;
	float fz=geo2dv.fz,dz=geo2dv.dz,odz=geo2dv.odz;
	float ax,az,sx,sz,sxx,szz,a0,a1,a2,a3;
	float g0,g1,g2,g3,gx0,gx1,gx2,gx3,gz0,gz1,gz2,gz3;

/*	determine interpolate coefficients	*/
	    ax = (x-fx)*odx;
	    jx = (int)ax;
	    if(jx<0) jx = 0;
	    if(jx>nx-2) jx = nx-2;
	    sx = ax-jx;
	    az = (z-fz)*odz;
	    jz = (int)az;
	    if(jz<0) jz = 0;
	    if(jz>nz-2) jz = nz-2;
	    sz = az-jz;

	    sxx = (float)(0.5*sx*(1.0-sx)*dx*dx);
	    szz = (float)(0.5*sz*(1.0-sz)*dz*dz);

	    a0 = (float)((1.0-sx)*(1.0-sz));
	    a1 = (float)((1.0-sx)*sz);
	    a2 = (float)(sx*(1.0-sz));
	    a3 = sx*sz;

/*	    set the table of indices for interpolation	*/
	    k0 = nz*jx+jz;
	    k1 = k0+1;
 	    k2 = k0+nz;
	    k3 = k2+1;

	    g0 = v[k0];
	    g1 = v[k1];
	    g2 = v[k2];
	    g3 = v[k3];
	    gx0 = vxx[k0];
	    gx1 = vxx[k1];
	    gx2 = vxx[k2];
	    gx3 = vxx[k3];
	    gz0 = vzz[k0];
	    gz1 = vzz[k1];
	    gz2 = vzz[k2];
	    gz3 = vzz[k3];

/*	interpolation	*/
	    *uxx = a0*gx0+a1*gx1+a2*gx2+a3*gx3;
	    *uxz = a0*vxz[k0]+a1*vxz[k1]+a2*vxz[k2]+a3*vxz[k3];
	    *uzz = a0*gz0+a1*gz1+a2*gz2+a3*gz3;

	    *u = a0*g0+a1*g1+a2*g2+a3*g3-(sxx*(*uxx)+szz*(*uzz));
	    *ux = ((1.0f-sz)*(g2-g0-sxx*(gx2-gx0)-szz*(gz2-gz0))
      		    +sz*(g3-g1-sxx*(gx3-gx1)-szz*(gz3-gz1)))*odx
      		    +(sx-0.5f)*dx*(*uxx);
	    *uz = ((1.0f-sx)*(g1-g0-sxx*(gx1-gx0)-szz*(gz1-gz0))
      		    +sx*(g3-g2-sxx*(gx3-gx2)-szz*(gz3-gz2)))*odz
      		    +(sz-0.5f)*dz*(*uzz);
}

void eiknl(Geo2d geo2dt,float *t,float *ov2,float tmax) 
/* 	compute traveltime in shadow zones by eikonal equation
  input
   t 		traveltimes form ray tracing
   ov2 		slowness squares
   tmax	 	upper limit of ordinary traveltime value
  output:
   t	traveltime (unchanged if t<=tmax)
*/
{
	int nx=geo2dt.nx,nz=geo2dt.nz,ix,iz;
	float dx=geo2dt.dx,dz=geo2dt.dz;
	float tx2,tz2,t0,tl,tr,temp,odx2,*tt1,*tt2;

	tt1 = (float *)alloc1float(nx);
	tt2 = (float *)alloc1float(nx);

	odx2 = 1.0f/(dx*dx);

	for(ix=0; ix<nx; ++ix)
		tt1[ix] = t[ix*nz];

	for(iz=1; iz<nz; ++iz){ 
	    for(ix=0; ix<nx; ++ix) 
		tt2[ix] = t[ix*nz+iz];

	    for(ix=1; ix<nx; ++ix){
/*	if traveltime is abnormal and the upper is normal	*/
		t0 = tt1[ix];
		if(tt2[ix] > tmax && t0 <= tmax) {
			tl = tr = 0.0; 
			if(ix > 0) tl = tt1[ix-1];
			if(ix < nx-1) tr = tt1[ix+1];
                        tx2 = (tl-t0)*(tl-t0);
                        temp = (tr-t0)*(tr-t0);
                        if(tx2>temp) tx2 = temp;
                        temp = 0.25f*(tl-tr)*(tl-tr);
                        if(tx2>temp) tx2 = temp;
                        tx2 *= odx2;

			tz2 = 0.5f*(ov2[ix*nz+iz-1]+ov2[ix*nz+iz])-tx2;
			if(tz2 >= 0) tt2[ix] = t0+dz*(float)sqrt(tz2);
		}
	    }
	    for(ix=0; ix<nx; ++ix){
		t[ix*nz+iz] = tt2[ix];
		tt1[ix] = tt2[ix];
	    }
	}

	free1float(tt1);
	free1float(tt2);

}
void dv2(Geo2d geo2dv,float *v,float *vxx,float *vxz,float *vzz) 
/*calculate second DeriVatives from a 2D VELocity grid by finite difference*/
/*  	input: 	velocity v 
  	output: vxx,vxz and vzz  */	 
{ 
  	int  nx=geo2dv.nx, nz=geo2dv.nz, ix, iz, i; 	 
 	float   odx=geo2dv.odx,odz=geo2dv.odz,odxx,odzz,odxz; 
   	
	odxx = odx*odx; 
	odzz = odz*odz; 
	odxz = 0.25f*odx*odz; 

/*	initialize	*/
	for(ix=0; ix<nx; ++ix)    
 	    for(iz=0; iz<nz; ++iz){
 		i = ix*nz+iz;
		vxx[i] = 0.; 
		vxz[i] = 0.; 
		vzz[i] = 0.; 
	}
	
/*	finite difference 	*/
	for(ix=1; ix<nx-1; ++ix)    
 	    for(iz=1; iz<nz-1; ++iz){
 		i = ix*nz+iz;
 		vxx[i] = odxx*(v[i+nz]-2.0f*v[i]+v[i-nz]); 
 		vxz[i] = odxz*(v[i+nz+1]-v[i+nz-1]-v[i-nz+1]+v[i-nz-1]); 
      		vzz[i] = odzz*(v[i+1]-2.0f*v[i]+v[i-1]);
	}

}

void voint(Geo2d geo2dv,float *v,Geo2d geo2dt,float *ov2,int npv,float *vo)
{

	int nx=geo2dv.nx,nz=geo2dv.nz,nxo=geo2dt.nx,nzo=geo2dt.nz;
	int i,io,jx,jz,ixo,izo;
	float fx=geo2dv.fx,fz=geo2dv.fz,odx=geo2dv.odx,odz=geo2dv.odz;
	float fxo=geo2dt.fx,fzo=geo2dt.fz,dxo=geo2dt.dx,dzo=geo2dt.dz;
	float x,z,ax,az,sx,sz,temp;


	for(ixo=0,x=fxo; ixo<nxo; ++ixo,x+=dxo){
		ax = (x-fx)*odx;
		jx = (int)ax;
		sx = ax-jx;
		if(jx < 0) jx = 0;
		if(jx > nx-2) jx = nx-2;
		for(izo=0,z=fzo; izo<nzo; ++izo,z+=dzo){
			az = (z-fz)*odz;
			jz = (int)az;
			sz = az-jz;
			if(jz < 0) jz = 0;
			if(jz > nz-2) jz = nz-2;
			io = ixo*nzo+izo;
			i = jx*nz+jz;
		    	temp = (1.0f-sx)*((1.0f-sz)*v[i]+sz*v[i+1])
      			    	+sx*((1.0f-sz)*v[i+nz]+sz*v[i+nz+1]);
			if(npv) vo[io] = temp;
			ov2[io] = 1.0f/(temp*temp);
		}
	}
}

void ddt(float p,float q,float c,float s, float *dv,float v,float *d2t,
	float *cuv)
/************************************************************************
ddt - calculate second derivatives of traveltime with respect to x,y
************************************************************************/
{
 	float ov2,m11,m12,m22;

	ov2 = 1.0f/(v*v);
	m11 = p/q;

/*	calculate 2nd column of m	*/
	m12 = -(dv[0]*c-dv[1]*s)*ov2;
	m22 = -(dv[0]*s+dv[1]*c)*ov2;

/*	compute h*m*h^T	*/
	d2t[0] = m11*c*c+2.0f*m12*c*s+m22*s*s;
	d2t[1] = (m22-m11)*c*s+m12*(c*c-s*s);
	d2t[2] = m11*s*s-2.0f*m12*c*s+m22*c*c;

/*	compute the curvature of raypath	*/
	*cuv = (float)(fabs(m12)*v);

}


void trans(int nz,int nxt,int nx0,float *v,float *vt) 
{
	int ix,iz,i,i0;

	for(ix=0; ix<nxt; ++ix){
		i = ix*nz;
		i0 = (ix+nx0)*nz;
		for(iz=0; iz<nz; ++iz)
			vt[i+iz] = v[i0+iz];
	}
}

/***********************************************************************/
  void dfrungk(float v,float vx,float vz,float vxx,float vxz,float vzz,
     	float x,float z,float px,float pz,float p2,float q2,
      	float *dx,float *dz,float *dpx,float *dpz,float *dp2,float *dq2)
{

	float ov,vv,c,s,v11;

		ov = 0.0f*z+ 0.0f*x + 1.0f/v;
		vv = v*v;
		*dx = vv*px;
		*dz = vv*pz;
		*dpx = -ov*vx;
		*dpz = -ov*vz;

		c = v*pz;
		s = v*px;
		v11 = vxx*c*c-2.0f*vxz*c*s+vzz*s*s;
		*dp2 = -ov*v11*q2;
		*dq2 = vv*p2;

}

/************************ c = a+h*b ***********************************/
 void sum2(float h,float a1,float a2,float a3,float a4,float a5,float a6,
	float b1,float b2,float b3,float b4,float b5,float b6,
	float *c1,float *c2,float *c3,float *c4,float *c5,float *c6)
{

	*c1 = a1+h*b1;
	*c2 = a2+h*b2;
	*c3 = a3+h*b3;
	*c4 = a4+h*b4;
	*c5 = a5+h*b5;
	*c6 = a6+h*b6;

}

void makeRay(Geo2d geo2dv,float *vel,float *vxx,float *vxz,float *vzz,
    Raytr raytr,float a0,int *nrs,Ray *ray,int npv,float *pv) 
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
{
	int nx=geo2dv.nx,nz=geo2dv.nz,nt=raytr.nt;
	float fx=geo2dv.fx,fz=geo2dv.fz;
	float dt=raytr.dt,amin=raytr.amin,amax=raytr.amax;

	int it;
	float tzmin,tzmax,ov,lx,lz,h,h2,h6;

	float v,dvdx,dvdz,uxx,uxz,uzz,tzt,dtv;
	float x,z,px,pz,p2,q2,
	      dx,dz,dpx,dpz,dp2,dq2;
	float xt,zt,pxt,pzt,p2t,q2t,
	    dxt,dzt,dpxt,dpzt,dp2t,dq2t;

	/* velocity and derivatives at source	*/
	x = raytr.xs;
	z = raytr.zs;
	vel2Interp(geo2dv,vel,vxx,vxz,vzz,x,z,&v,&dvdx,&dvdz,&uxx,&uxz,&uzz);
	ov = 1.0f/v;

	p2 = 1.0;
	q2 = 0.0;
	*nrs = nt;

/*	compute slowness at source	*/
	px = (float)(ov*sin(a0));
	pz = (float)(ov*cos(a0));

/*	first ray step output	*/
	tzt = v*pz;
	ray[0].x = x;
	ray[0].z = z;
	ray[0].px = px;
	ray[0].pz = pz;
	ray[0].q2 = q2;
	ray[0].p2 = p2;
	ray[0].v = v;
	ray[0].dvdx = dvdx;
	ray[0].dvdz = dvdz;
	if(npv) ray[0].dtv = 0;

/*	compute minimum and maximum z-component of unit ray vector	*/
	tzmin = (float)cos(amax)-0.01f;
	tzmax = (float)cos(amin)+0.01f;

/*	determine fraction steps for Rung-Kuta	*/
	h = dt;		h2 = h*0.5f;	h6 = h/6.0f;

	lx = fx+(nx-1)*geo2dv.dx;
	lz = fz+(nz-1)*geo2dv.dz;

/*	loop over time steps	*/
	for(it=1; it<nt; ++it){
		if(x<fx || x>lx || z<fz || z>lz ||
      		   tzt<tzmin || tzt>tzmax) {
			it -= 1; 
			break;
		}

/*	step 1 of 4th-order Rung-Kuta	*/

/*	  compute k1 = f(y0)	*/
	    dfrungk(v,dvdx,dvdz,uxx,uxz,uzz,
      		x,z,px,pz,p2,q2,
      		&dx,&dz,&dpx,&dpz,&dp2,&dq2);
          
/*	  compute y1 = y0+0.5*h*k1	*/
	    sum2(h2,x,z,px,pz,p2,q2,
      		dx,dz,dpx,dpz,dp2,dq2,
      		&xt,&zt,&pxt,&pzt,&p2t,&q2t);

/*	velocity interpolation	*/
	vel2Interp(geo2dv,vel,vxx,vxz,vzz,
      	  xt,zt,&v,&dvdx,&dvdz,&uxx,&uxz,&uzz);

/*	step 2 of 4th-order Rung-Kuta	*/

/*	  compute k2 = f(y1)	*/
	    dfrungk(v,dvdx,dvdz,uxx,uxz,uzz,
      		xt,zt,pxt,pzt,p2t,q2t,
      		&dxt,&dzt,&dpxt,&dpzt,&dp2t,&dq2t);

/*	  compute y2 = y0+0.5*h*k2	*/
	    sum2(h2,x,z,px,pz,p2,q2,
      		dxt,dzt,dpxt,dpzt,dp2t,dq2t,
      		&xt,&zt,&pxt,&pzt,&p2t,&q2t);

/*	  compute k = k1+2.0*k2	*/
	    sum2(2.0,dx,dz,dpx,dpz,dp2,dq2,
      		dxt,dzt,dpxt,dpzt,dp2t,dq2t,
      		&dx,&dz,&dpx,&dpz,&dp2,&dq2);
 
/*	velocity interpolation	*/
	vel2Interp(geo2dv,vel,vxx,vxz,vzz,
      	  xt,zt,&v,&dvdx,&dvdz,&uxx,&uxz,&uzz);

/*	step 3 of 4th-order Rung-Kuta	*/

/*	  compute k3 = f(y2)	*/
	    dfrungk(v,dvdx,dvdz,uxx,uxz,uzz,
      		xt,zt,pxt,pzt,p2t,q2t,
      		&dxt,&dzt,&dpxt,&dpzt,&dp2t,&dq2t);

/*	  compute y3 = y0+h*k2	*/
 	    sum2(h,x,z,px,pz,p2,q2,
      		dxt,dzt,dpxt,dpzt,dp2t,dq2t,
      		&xt,&zt,&pxt,&pzt,&p2t,&q2t);

/*	  compute k = k1+2.0*k2+2.0*k3	*/
	    sum2(2.0,dx,dz,dpx,dpz,dp2,dq2,
      		dxt,dzt,dpxt,dpzt,dp2t,dq2t,
      		&dx,&dz,&dpx,&dpz,&dp2,&dq2);
 
/*	velocity interpolation	*/
	vel2Interp(geo2dv,vel,vxx,vxz,vzz,
      	  xt,zt,&v,&dvdx,&dvdz,&uxx,&uxz,&uzz);

/*	step 4 of 4th-order Rung-Kuta	*/

/*	  compute k4 = f(y3)	*/
	    dfrungk(v,dvdx,dvdz,uxx,uxz,uzz,
      		xt,zt,pxt,pzt,p2t,q2t,
      		&dxt,&dzt,&dpxt,&dpzt,&dp2t,&dq2t);

/*	  compute k = k1+2.0*k2+2.0*k3+k4	*/
	    sum2(1.0,dx,dz,dpx,dpz,dp2,dq2,
      		dxt,dzt,dpxt,dpzt,dp2t,dq2t,
      		&dx,&dz,&dpx,&dpz,&dp2,&dq2);
 
/*	  compute y4 = y0+h*k/6	*/
 	    sum2(h6,x,z,px,pz,p2,q2,
      		dx,dz,dpx,dpz,dp2,dq2,
      		&x,&z,&px,&pz,&p2,&q2);
         
/*	velocity interpolation	*/
	    vel2Interp(geo2dv,vel,vxx,vxz,vzz,
      	  	x,z,&v,&dvdx,&dvdz,&uxx,&uxz,&uzz);

/*	velocity itself interpolation	*/
	if(npv)	vintp(geo2dv,pv,x,z,&dtv);

		tzt = v*pz;

/*	save ray parameters */
		ray[it].x = x;
		ray[it].z = z;
		ray[it].px = px;
		ray[it].pz = pz;
		ray[it].p2 = p2;
		ray[it].q2 = q2;
		ray[it].v = v;
		ray[it].dvdx = dvdx;
		ray[it].dvdz = dvdz;
		if(npv)  ray[it].dtv = dtv/v;
	}
	*nrs = it; 

}
 
void raytime2d(Raytr raytr,Geo2d geo2dv,float *v,Geo2d geo2dt,float *t,
			   int npv,float *vo,float *pv,float *tv,float *cs) 
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
{
	int na=raytr.na,nt=raytr.nt,nxo=geo2dt.nx,nzo=geo2dt.nz,
		nx=geo2dv.nx,nz=geo2dv.nz;
	float fxo=geo2dt.fx,fzo=geo2dt.fz,dxo=geo2dt.dx,dzo=geo2dt.dz,
		odxo=geo2dt.odx,odzo=geo2dt.odz;
	float fac=raytr.fac,dt=raytr.dt,fa=raytr.fa,da=raytr.da,xs=raytr.xs;
	float *vxx,*vxz,*vzz,*s,zs=raytr.zs;
	int nrs;
	Ray *ray;
	
	
	int i,ia,ixo,izo;
	float a,xo,zo,exo,ezo;
	
	/*	variables used for extrapolation	*/
	int it,jt,nxf,nxe,nzf,nze;
	float tc,xoc,zoc,xc,zc,r1,v0=0.0,norm2,terr,vd1,cuv,
      		sd,sind,cosd,vd,pxd,pzd,r2,t1,t2,r1x,r2x,t1x,t2x,t2xz,
			p2d,q2d,gradv[2],d2t[3],dtvd,odt=1.0f/dt,xcosd=0.0,*tvd;
	float gc = 0.f;  // Deping's addition for receiver depth
	
	vxx = alloc1float(nx*nz);
	vxz = alloc1float(nx*nz);
	vzz = alloc1float(nx*nz);
	tvd = alloc1float(nt);
	s = alloc1float(nxo*nzo);
	ray = (Ray*)alloc1(nt,sizeof(Ray)); 
	
	/* compute second derivatives of velocity	*/
	dv2(geo2dv,v,vxx,vxz,vzz); 
	
	
	/*	maximum range of output points	*/
	exo = fxo+(nxo-1)*dxo;
	ezo = fzo+(nzo-1)*dzo;
	
	/*	initialize traveltime and raypath length	*/
	for(ixo=0; ixo<nxo; ++ixo) 
		for(izo=0; izo<nzo; ++izo){
			i = izo+ixo*nzo;
			t[i] = INITIAL_T;
			s[i] = INITIAL_T;
			if(npv) tv[i] = cs[i] = 0.0;
		}
		
		/* 	loop over shooting angles at source point 	*/
		for(ia=0,a=fa; ia<na; ++ia,a+=da){  
			
			/*		trace rays	*/
			makeRay(geo2dv,v,vxx,vxz,vzz,raytr,a,&nrs,ray,npv,pv);
			
			/*		extropolate to grids near central rays	*/
			v0 = ray[0].v;
			r2 = 0.0;
			xc = raytr.xs;
			zc = raytr.zs;
			sd = 0;
			vd1 = v0;
			if(npv) {
				dtvd = 0.0;
				for(it=1; it<nrs; ++it) {
					dtvd += 0.5f*dt*(ray[it].dtv+ray[it-1].dtv);
					tvd[it] = ray[it].v*dtvd;
				}
			}
			for (it=1; it<nrs; ++it) {
				xo = ray[it].x;
				zo = ray[it].z;
				vd = ray[it].v;
				sd = sd+0.5f*dt*(vd+vd1);
				vd1 = vd;
				
				/*		    	if the point is within the previous circle	*/
				if(r2 > (xc-xo)*(xc-xo)+(zc-zo)*(zc-zo)
					&& it != nrs-1) continue;
				
				/*		    	if the point is out of output range	*/
				if(xo<fxo || xo>exo || zo<fzo || zo>ezo) continue;

				
				xc = xo;
				zc = zo;
				q2d = ray[it].q2; 
				
				/*			if caustics	*/
				if(q2d == 0.0) {
					r2 = 0.0;
					continue;
				}
				
				vd = ray[it].v;
				pxd = ray[it].px;
				pzd = ray[it].pz;
				
				p2d = ray[it].p2;
				sind = vd*pxd;
				cosd = vd*pzd;
				gradv[0] = ray[it].dvdx;
				gradv[1] = ray[it].dvdz;
				
				/*			calculate second derivatives of traveltime*/
				ddt(p2d,q2d,cosd,sind,gradv,vd,d2t,&cuv);
				
				/*			determine radius for extrapolation	*/
				tc = it*dt;
				terr = tc*fac;
				norm2 = (float)sqrt(d2t[0]*d2t[0]+d2t[2]*d2t[2]+
					2.0*d2t[1]*d2t[1]);
				
				r2 = terr/norm2;
				r1 = (float)sqrt(r2);
				/*			keep ray-centered coordinate system regular */
				if(r1*cuv > 0.1f) r1 = 0.1f/cuv;
				
				/*			radius cannot be too large	*/
				if(r1 > 0.1*sd) r1 = 0.1f*sd;
				
				r2 = r1*r1;
				
				nxf = (int)((xc-r1-fxo)*odxo+0.9);
				if(nxf < 0) nxf = 0;
				nxe = (int)((xc+r1-fxo)*odxo+0.1);
				if(nxe >= nxo) nxe = nxo-1;
				/*       	fprintf(stderr,"x,z,t,r=%g %g %g %g\n",
				xc,zc,tc,r1);*/	 
				
				for(ixo=nxf; ixo<=nxe; ++ixo){
					xoc = fxo-xc+ixo*dxo;
					r2x = r2-xoc*xoc;
					if(r2x < 0) continue;
					
					r1x = (float)sqrt(r2x);
					t1x = tc+xoc*pxd;
					t2x = tc*xoc*xoc*d2t[0];
					t2xz = 2.0f*xoc*d2t[1];
					if(npv) xcosd = pzd+xoc*d2t[1];
					
					nzf = (int)((zc-r1x-fzo)*odzo+0.9);
					if(nzf < 0) nzf = 0;
					nze = (int)((zc+r1x-fzo)*odzo+0.1);
					if(nze>=nzo) nze = nzo-1;
					
					i = ixo*nzo;
					for(izo=nzf; izo<=nze; ++izo){ 
						/*				    if ray path is shorter	*/
						if(sd < s[i+izo] ) {
							zoc = fzo-zc+izo*dzo;
							t1 = t1x+zoc*pzd;
							t2 = t2x+zoc*(zoc*d2t[2]+t2xz);
							s[i+izo] = sd;
							t[i+izo] = t1*t1+tc*t2;
							if(npv) {
								jt = (int)((t1+0.5*t2)*odt+0.5);
								if(jt<0) jt = 0;
								if(jt>nrs-1) jt = nrs-1;
								tv[i+izo] = tvd[jt];
								cs[i+izo] = xcosd+d2t[2]*zoc;
							}
						}
					}
				}
			}
			
	}
	
	/*	square root of traveltimes */
	for (ixo=0; ixo<nxo; ++ixo){ 
		i = ixo*nzo;
		for (izo=0; izo<nzo; ++izo){  
			t[i+izo] = (float)max(0.0,t[i+izo]);
			if(t[i+izo] < 999) t[i+izo] = (float)sqrt(t[i+izo]);
			if(npv) cs[i+izo] *= vo[i+izo];
		}
	}
	
	/*	compute traveltime near source	*/
	nxf = (int)((xs-fxo)/dxo-1.5);
	if(nxf<0) nxf = 0;
	nxe = (int)((xs-fxo)/dxo+2.5);
	if(nxe>nxo-1) nxe = nxo-1;
	nzf = (int)((zs-fzo)/dzo-0.5);
	if(nzf<0) nzf = 0;
	nze = (int)((zs-fzo)/dzo+1.5);
	if(nze>=nzo) nze = nzo-1;
	for (ixo=nxf; ixo<=nxe; ++ixo){
		xo = fxo+ixo*dxo;
		i = ixo*nzo;
		for (izo=nzf; izo<=nze; ++izo){
			zo = fzo+izo*dzo;
			if(t[i+izo] > 999) 
				t[i+izo] = (float)(sqrt((xo-xs)*(xo-xs)+(zo-zs)*(zo-zs))/v0);
		}
	}
	
	free1float(vxx);
	free1float(vxz);
	free1float(vzz);
	free1float(s);
	free1((void*)ray);
}


int Lius2draytracingMod(float *v, Geo2d geo2dv, Geo2d geo2dt,
	int nt, float dt, float fs, float ds, float aperx,
	float fa, int na, float da, float amin, float amax,
	float fac, float sourceDepth, int ek, int npv, FILE *jpfp, CString rayName)
{
	/*
	* Author:  Zhenyue Liu, 10/11/94,  Colorado School of Mines
	*
	* This modified routine only outputs t[nxt*nzt].
	*
	* References:
	*
	* Beydoun, W. B., and Keho, T. H., 1987, The paraxial ray method:
	*   Geophysics, vol. 52, 1639-1653.
	*
	* Cerveny, V., 1985, The application of ray tracing to the numerical
	*   modeling of seismic wavefields in complex structures, in Dohr, G.,
	*   ED., Seismic shear waves (part A: Theory): Geophysical Press,
	*   Number 15 in Handbook of Geophysical Exploration, 1-124.
	*
	*/
	/**************** end self doc ********************************/

	int	nat, nxo, nzo, nx, nz, nxt, nx0, mx;
	float	xs, exs, fxo, fzo, dxo, dzo, exo, ezo,
		ea, eat, fat, tmax,
		fx, fz, dx, dz, ex, ez, fxt, ext,
		*vt, *ov2, *pvt, *pv, *tv, *cs, *vo, *t;
	int ixs0, nsize, isize, ms;
	Raytr raytr;

	nx = geo2dv.nx; nz = geo2dv.nz;
	if (nx < 3 || nz < 3)
	{
		_ftprintf_s(jpfp, _T("  Raytracing failed since nxv<=3 or nzv<=3 "));
		AfxMessageBox(_T("Raytracing's nx and nz must be not less than 3!"), MB_ICONQUESTION);
		return 0;
	}
	fx = geo2dv.fx; fz = geo2dv.fz;
	dx = geo2dv.dx; dz = geo2dv.dz;
	ex = fx + (nx - 1)*dx;
	ez = fz + (nz - 1)*dz;

	nxo = geo2dt.nx; nzo = geo2dt.nz;
	if (nxo < 3 || nzo<3)
	{
		_ftprintf_s(jpfp, _T("  Raytracing failed since nxo<=3 or nzo<=3 "));
		return 0;
	}
	fxo = geo2dt.fx; fzo = geo2dt.fz;
	dxo = geo2dt.dx; dzo = geo2dt.dz;
	exo = fxo + (nxo - 1)*dxo;
	ezo = fzo + (nzo - 1)*dzo;

	exs = exo;
	mx = (int)(2.0*aperx / dx + 2.99);
	if (mx>nx) mx = nx;

	tmax = (nt - 1)*dt;
	if (amax > 180 || amin < 0)
	{
		_ftprintf_s(jpfp, _T("  Raytracing failed since amin and amax not within 0-180 degrees "));
		return 0;
	}
	fa *= PI / 180;
	da *= PI / 180;
	ea = fa + (na - 1)*da;
	amin *= PI / 180;
	amax *= PI / 180;
	raytr.nt = nt;		raytr.dt = dt;
	raytr.da = da;		raytr.fac = fac;
	raytr.amin = amin;	raytr.amax = amax;
	raytr.zs = sourceDepth;


	ms = 10;
	ixs0 = 0; isize = 0; nsize = nzo*nxo;
	//End of parameter section

	/* ensure sources and output are in velocity grid	*/
	if (fx > fxo || ex < exs || fz>0)
	{
		_ftprintf_s(jpfp, _T("  Raytracing failed since source lies outside of specified x grid "));
		return 0;
	}
	if (fx > fxo || ex<exo || fz>fzo || ez < ezo)
	{
		_ftprintf_s(jpfp, _T("  Raytracing failed since output trace lies outside of specified x grid "));
		return 0;
	}

	/* allocate space */
	ov2 = (float *)alloc1float(nxo*nzo);
	t = (float *)alloc1float(nxo*nzo);
	if (ov2 == NULL || t == NULL)
	{
		_ftprintf_s(jpfp, _T("  Raytracing failed since memory not allocated "));
		return 0;
	}

	// these arrays actually not used.
	if (npv) {
		pv = alloc1float(nx*nz);
		tv = alloc1float(nxo*nzo);
		vo = alloc1float(nxo*nzo);
		cs = alloc1float(nxo*nzo);
	}

	/*interpolate velocity and compute slowness squares  */
	// output ov2[nxo*nzo], which is 1/(v*v).
	voint(geo2dv, v, geo2dt, ov2, npv, vo);

	CFile filet(rayName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);

	xs = fs;
	int scount = 0;
	while (xs <= exo)
	{
		/* redefine the velocity model due to aperture	*/
		//	temp = fxo;
		//	if(xs<temp) temp = xs;
		//	if(xs-aperx>temp) temp = xs-aperx;
		nx0 = (int)((fxo - fx) / dx); // fx is always smaller than fxo.
		fxt = fx + nx0*dx;
		//	temp = exo;
		//	if(xs>temp) temp = xs;
		//	if(xs+aperx<temp) temp = xs+aperx;
		nxt = 1 + (int)((exo - fx) / dx + 0.99) - nx0;
		ext = fxt + (nxt - 1)*dx;
		//note the above that I made fx always smaller than fxo, so I simplify.

		vt = alloc1float(nxt*nz);
		if (vt == NULL)
		{
			_ftprintf_s(jpfp, _T("  Raytracing failed since memory vt not allocated "));
			free1float(ov2);
			return FALSE;
		}
		if (npv) pvt = alloc1float(nxt*nz);

		/* reducing velocity voluem due to aperture:
		* output vt[] which is from v[]: covering 2*aperx wide area only: from nx0 to nx0+nxt */
		trans(nz, nxt, nx0, v, vt);
		if (npv) trans(nz, nxt, nx0, pv, pvt);

		/* determine range of take-off angles	*/
		/*
		fat = fa;
		nat = na;
		if(xs==fxt && fat<0) {
		fat = 0.;
		nat = (int)(ea/da+1.5);
		} else if(xs==ext && ea>0)
		nat = (int)(1.5-fa/da);
		*/
		fat = fa;
		eat = ea;
		nat = na;
		if (fat < -(PI / 2)) fat = -PI / 2;
		if (eat >(PI / 2)) eat = PI / 2;
		if (xs == fxt && fat < 0) {
			fat = 0.;
			nat = (int)(eat / da + 1.5);
		}
		else if (xs == ext && eat>0)
			nat = (int)(1.5 - fa / da);

		/* update geometry information	*/
		raytr.xs = xs;		raytr.zs = sourceDepth;
		raytr.na = nat;		raytr.fa = fat;

		geo2dv.nx = nxt;	geo2dv.fx = fxt;

		/* compute traveltime by paraxial ray tarcing	*/
		raytime2d(raytr, geo2dv, vt, geo2dt, t, npv, vo, pvt, tv, cs);

		/*make up in shadow zones by eikonal equation	*/
		if (ek) eiknl(geo2dt, t, ov2, tmax);
		free1float(vt);

		/*write traveltime	*/
		// int numwritten = fwrite(t,sizeof(float),nxo*nzo,tfp);
		filet.Write(t, (UINT)(nxo*nzo*sizeof(float)));
		scount++;
		filet.Flush();

		_ftprintf_s(jpfp, _T(" traveltime computed for xs=%g\n"), xs);
		fflush(jpfp);

		xs += ds;
	}

	_ftprintf_s(jpfp, _T(" finish program rayt2d\n\n"));
	free1float(ov2);
	free1float(t);
	filet.Close();
	return scount;
}


BOOL Lius2draytracingSingle(float *v, Geo2d geo2dv, Geo2d geo2dt,
	int nt, float dt, float fxs, float *t, float aperx,
	float fa, int na, float da, float amin, float amax,
	float fac, float sourceDepth, int ek, int npv, FILE *jpfp)
{
	/*
	* Author:  Zhenyue Liu, 10/11/94,  Colorado School of Mines
	*
	* This modified routine only outputs t[nxt*nzt].
	*
	* References:
	*
	* Beydoun, W. B., and Keho, T. H., 1987, The paraxial ray method:
	*   Geophysics, vol. 52, 1639-1653.
	*
	* Cerveny, V., 1985, The application of ray tracing to the numerical
	*   modeling of seismic wavefields in complex structures, in Dohr, G.,
	*   ED., Seismic shear waves (part A: Theory): Geophysical Press,
	*   Number 15 in Handbook of Geophysical Exploration, 1-124.
	*
	*/
	/**************** end self doc ********************************/

	int	nat, nxo, nzo, nx, nz, nxt, nx0, mx;
	float	xs, exs, fxo, fzo, dxo, dzo, exo, ezo,
		ea, fat, tmax, temp,
		fx, fz, dx, dz, ex, ez, fxt, ext,
		*vt, *ov2, *pvt, *pv, *tv, *cs, *vo;
	int ixs0, nsize, isize, ms;
	Raytr raytr;

	nx = geo2dv.nx; nz = geo2dv.nz;
	if (nx < 3 || nz < 3)
	{
		_ftprintf_s(jpfp, _T("  Raytracing failed since nxv<=3 or nzv<=3 "));
		AfxMessageBox(_T("Raytracing's nx and nz must be not less than 3!"), MB_ICONQUESTION);
		return FALSE;
	}
	fx = geo2dv.fx; fz = geo2dv.fz;
	dx = geo2dv.dx; dz = geo2dv.dz;
	ex = fx + (nx - 1)*dx;
	ez = fz + (nz - 1)*dz;

	nxo = geo2dt.nx; nzo = geo2dt.nz;
	if (nxo < 3 || nzo<3)
	{
		_ftprintf_s(jpfp, _T("  Raytracing failed since nxo<=3 or nzo<=3 "));
		return FALSE;
	}
	fxo = geo2dt.fx; fzo = geo2dt.fz;
	dxo = geo2dt.dx; dzo = geo2dt.dz;
	exo = fxo + (nxo - 1)*dxo;
	ezo = fzo + (nzo - 1)*dzo;

	exs = fxs;
	mx = (int)(2.0*aperx / dx + 2.99);
	if (mx>nx) mx = nx;

	tmax = (nt - 1)*dt;
	if (amax > 180 || amin<0)
	{
		_ftprintf_s(jpfp, _T("  Raytracing failed since amin and amax not within 0-180 degrees "));
		return FALSE;
	}
	fa *= PI / 180;
	da *= PI / 180;
	ea = fa + (na - 1)*da;
	amin *= PI / 180;
	amax *= PI / 180;
	raytr.nt = nt;		raytr.dt = dt;
	raytr.da = da;		raytr.fac = fac;
	raytr.amin = amin;	raytr.amax = amax;
	raytr.zs = sourceDepth;

	ms = 10;
	ixs0 = 0; isize = 0; nsize = nzo*nxo;
	//End of parameter section

	/* ensure sources and output are in velocity grid	*/
	if (fx>fxs || ex < exs || fz>0)
	{
		_ftprintf_s(jpfp, _T("  Raytracing failed since source lies outside of specified x grid "));
		return FALSE;
	}
	if (fx > fxo || ex<exo || fz>fzo || ez < ezo)
	{
		_ftprintf_s(jpfp, _T("  Raytracing failed since output trace lies outside of specified x grid "));
		return FALSE;
	}

	/* allocate space */
	ov2 = (float *)alloc1float(nxo*nzo);
	if (ov2 == NULL)
	{
		_ftprintf_s(jpfp, _T("  Raytracing failed since memory ov2 not allocated "));
		return FALSE;
	}


	if (npv) {
		pv = alloc1float(nx*nz);
		tv = alloc1float(nxo*nzo);
		vo = alloc1float(nxo*nzo);
		cs = alloc1float(nxo*nzo);
	}

	/*interpolate velocity and compute slowness squares  */
	// output ov2[nxo*nzo], which is 1/(v*v).
	voint(geo2dv, v, geo2dt, ov2, npv, vo);

	/* redefine the velocity model due to aperture	*/
	xs = fxs;
	temp = fxo;
	if (xs < temp) temp = xs;
	if (xs - aperx > temp) temp = xs - aperx;
	nx0 = (int)((temp - fx) / dx);
	fxt = fx + nx0*dx;
	temp = exo;
	if (xs > temp) temp = xs;
	if (xs + aperx < temp) temp = xs + aperx;
	nxt = 1 + (int)((temp - fx) / dx + 0.99) - nx0;
	ext = fxt + (nxt - 1)*dx;

	vt = alloc1float(nxt*nz);
	if (vt == NULL)
	{
		_ftprintf_s(jpfp, _T("  Raytracing failed since memory vt not allocated "));
		free1float(ov2);
		return FALSE;
	}
	if (npv) pvt = alloc1float(nxt*nz);

	/* reducing velocity voluem due to aperture:
	* output vt[] which is from v[]: covering 2*aperx wide area only: from nx0 to nx0+nxt */
	trans(nz, nxt, nx0, v, vt);
	if (npv) trans(nz, nxt, nx0, pv, pvt);

	/* determine range of take-off angles	*/
	fat = fa;
	nat = na;
	if (xs == fxt && fat < 0) {
		fat = 0.;
		nat = (int)(ea / da + 1.5);
	}
	else if (xs == ext && ea>0)
		nat = (int)(1.5 - fa / da);


	/* update geometry information	*/
	raytr.xs = xs;		raytr.zs = sourceDepth;
	raytr.na = nat;		raytr.fa = fat;
	geo2dv.nx = nxt;	geo2dv.fx = fxt;

	/* compute traveltime by paraxial ray tarcing	*/
	raytime2d(raytr, geo2dv, vt, geo2dt, t, npv, vo, pvt, tv, cs);

	/*make up in shadow zones by eikonal equation	*/
	if (ek) eiknl(geo2dt, t, ov2, tmax);

	free1float(vt);
	free1float(ov2);

	return TRUE;
}
