#include "stdafx.h"
#include "preKDMig.h"

//#include "segy.h"
#include "ealloc_d.h"
#include "sumig.h"
#include "util.h"
#include "pfafft.h"

/* residual traveltime calculation based  on reference   time	*/
  void resit(int nx,float fx,float dx,int nz,int nr,float dr,
		float **tb,float **t,float x0)
{
	int ix,iz,jr;
	float xi,ar,sr,sr0;

	for(ix=0; ix<nx; ++ix){
		xi = fx+ix*dx-x0;
		ar = (float)fabs(xi)/dr;
		jr = (int)ar;
		sr = ar-jr;
		sr0 = 1.0f-sr;
		if(jr>nr-2) jr = nr-2;
		for(iz=0; iz<nz; ++iz)
			t[ix][iz] -= sr0*tb[jr][iz]+sr*tb[jr+1][iz];
	}
} 

/* lateral interpolation	*/

/* sum of two tables	*/
  void sum2(int nx,int nz,float a1,float a2,float **t1,float **t2,float **t)
{
	int ix,iz;

	for(ix=0; ix<nx; ++ix) 
		for(iz=0; iz<nz; ++iz)
			t[ix][iz] = a1*t1[ix][iz]+a2*t2[ix][iz];
}
 
void polint(float xa[], float ya[], int n, float x, float *y, float *dy, float *c, float *d)
//Given arrays xa[1..n] and ya[1..n], and given a value x, this routine returns a value y, and
//an error estimate dy. If P(x) is the polynomial of degree N _ 1 such that P(xai) = ya i ; i =
//1; : : : ; n, then the returned value y = P(x).
{
	int i,m,ns=1;
	float den,dif,dift,ho,hp,w;
	// float *c,*d;
	
	dif=(float)(fabs(x-xa[1]));
	// c=vector(1,n);
	// d=vector(1,n);
	for (i=1;i<=n;i++) 
	{ //Here we find the index ns of the closest table entry,
		if ( (dift=(float)fabs(x-xa[i])) < dif) {
			ns=i;
			dif=dift;
		}
		c[i]=ya[i]; // and initialize the tableau of c's and d's.
		d[i]=ya[i];
	}
	*y=ya[ns--]; //This is the initial approximation to y.
	for (m=1;m<n;m++) { //For each column of the tableau,
		for (i=1;i<=n-m;i++) { //we loop over the current c's and d's and update them.
			ho=xa[i]-x;
			hp=xa[i+m]-x;
			w=c[i+1]-d[i];
			den=ho-hp;
			// if ( (den=ho-hp) == 0.0) nrerror("Error in routine polint");
			//This error can occur only if two input xa's are (to within roundo_) identical.
			den=w/den;
			d[i]=hp*den; //Here the c's and d's are updated.
			c[i]=ho*den;
		}
		*y += (*dy=(2*ns < (n-m) ? c[ns+1] : d[ns--]));
	//After each column in the tableau is completed, we decide which correction, c or d,
	//we want to add to our accumulating value of y, i.e., which path to take through the
	//tableau|forking up or down. We do this in such a way as to take the most \straight
	//line" route through the tableau to its apex, updating ns accordingly to keep track of
	//where we are. This route keeps the partial approximations centered (insofar as possible)
	//on the target x. The last dy added is thus the error indication.
	}
//	free_vector(d,1,n);
//	free_vector(c,1,n);
}

  void sum5(int nx,int nz,float ds,float xsInWin,float **t0,float **t1,
	  float **t2,float **t3,float **t4,int ns, float **t)
{
	int ix,iz;
	float xx[6],yy[6],dy;
	float *c,*d;

	c = alloc1float(6);
	d = alloc1float(6);

	xx[1] = 0.f; xx[2] = ds; xx[3]=2.f*ds; xx[4]=3.f*ds; xx[5]=4.f*ds;
	for(ix=0; ix<nx; ++ix) 
		for(iz=0; iz<nz; ++iz)
		{
			yy[1] = t0[ix][iz];
			yy[2] = t1[ix][iz];
			yy[3] = t2[ix][iz];
			yy[4] = t3[ix][iz];
			yy[5] = t4[ix][iz];
			polint(xx,yy,ns,xsInWin, &t[ix][iz],&dy, c, d);
		}
	free1float(c);
	free1float(d);

}

/* compute  reference traveltime and slowness	*/
void timeb(int nr,int nz,float dr,float dz,float fz,float a,
	float v0,float **t,float **p,float **cs0,float **ang)
{
	int  ir,iz;
	float r,z,v,rc,oa,temp,rou,zc;


	if( a==0.0) {
		for(ir=0,r=0;ir<nr;++ir,r+=dr)
			for(iz=0,z=fz;iz<nz;++iz,z+=dz){
				rou = (float)sqrt(r*r+z*z);
				if(rou<dz) rou = dz;
				t[ir][iz] = rou/v0;
				p[ir][iz] = r/(rou*v0);
				cs0[ir][iz] = z/rou;
				ang[ir][iz] = (float)asin(r/rou);
			}
	} else {
		oa = 1.0f/a; 	zc = v0*oa;
		for(ir=0,r=0;ir<nr;++ir,r+=dr)
			for(iz=0,z=fz+zc;iz<nz;++iz,z+=dz){
				rou = (float)sqrt(r*r+z*z);
				v = v0+a*(z-zc);
				if(ir==0){ 
					t[ir][iz] = (float)(log(v/v0)*oa);
					p[ir][iz] = 0.0;
					ang[ir][iz] = 0.0;
					cs0[ir][iz] = 1.0;
				} else {
					rc = (r*r+z*z-zc*zc)/(2.0f*r);
					rou = (float)sqrt(zc*zc+rc*rc);
					t[ir][iz] = (float)log((v*(rou+rc))
						/(v0*(rou+rc-r)))*oa;
					p[ir][iz] = (float)sqrt(rou*rou-rc*rc)
						/(rou*v0);
					temp = v*p[ir][iz];
					if(temp>1.0) temp = 1.0;
					ang[ir][iz] = (float)asin(temp);
					cs0[ir][iz] = rc/rou;
				}
			}
	}
}



void filt(float *trace,int nt,float dt,float fmax,int ls,int m,float *trf)
/* Low-pass filter, integration and phase shift for input data	 
   input: 
    trace(nt)	single seismic trace
   fmax	high cut frequency
    ls		ls=1, line source; ls=0, point source
  output:
    trace(nt) 	filtered and phase-shifted seismic trace 
    tracei(nt) 	filtered, integrated and phase-shifted seismic trace 
 */
{
	static int nfft=0, itaper, nw, nwf;
	static float *taper, *amp, *ampi, dw;
	int  it, iw, itemp;
	float temp, ftaper, const2, *rt;
	complex *ct;

	int FSIZE = sizeof(float);
	fmax *= 2.0f*PI;
	ftaper = 0.1f*fmax;
	const2 = (float)sqrt(2.0f);

	if(nfft==0) {
        	/* Set up FFT parameters */
        	nfft = npfaro(nt+m, 2 * (nt+m));
        //	if (nfft >= MIN(SU_NFLTS, 720720))
        //      	err("Padded nt=%d -- too big", nfft);

        	nw = nfft/2 + 1;
		dw = 2.0f*PI/(nfft*dt);

		itaper = (int)(0.5f+ftaper/dw);
		taper = ealloc1float(2*itaper+1);
		for(iw=-itaper; iw<=itaper; ++iw){
			temp = (float)iw/(1.0f+itaper); 
			taper[iw+itaper] = (1-temp)*(1-temp)*(temp+2)/4;
		}

		nwf = (int)(0.5+(float)(fmax/dw));
		if(nwf>nw-itaper-1) nwf = nw-itaper-1;
		amp = ealloc1float(nwf+itaper+1);
		ampi = ealloc1float(nwf+itaper+1);
		amp[0] = ampi[0] = 0.;
		for(iw=1; iw<=nwf+itaper; ++iw){
			amp[iw] = (float)sqrt(dw*iw)/nfft;
			ampi[iw] = 0.5f/(1.f-(float)cos(iw*dw*dt));
		}
	}

        /* Allocate fft arrays */
        rt   = ealloc1float(nfft);
        ct   = ealloc1complex(nw);

        memcpy(rt, trace, nt*FSIZE);
        memset((void *) (rt + nt), (int) '\0', (nfft-nt)*FSIZE); 
        pfarc(1, nfft, rt, ct);

	for(iw=nwf-itaper;iw<=nwf+itaper;++iw){
		itemp = iw-(nwf-itaper);
		ct[iw].r = taper[itemp]*ct[iw].r; 
		ct[iw].i = taper[itemp]*ct[iw].i; 
	}
	for(iw=nwf+itaper+1;iw<nw;++iw){
		ct[iw].r = 0.; 
		ct[iw].i = 0.; 
	}

       	if(!ls){
		for(iw=0; iw<=nwf+itaper; ++iw){
			/* phase shifts PI/4 	*/
			temp = (ct[iw].r-ct[iw].i)*amp[iw]*const2;
			ct[iw].i = (ct[iw].r+ct[iw].i)*amp[iw]*const2;
			ct[iw].r = temp;
		    }
	} else {
		for(iw=0; iw<=nwf+itaper; ++iw){
			ct[iw].i = ct[iw].i*amp[iw];
			ct[iw].r = ct[iw].r*amp[iw];
		}
	}              
        pfacr(-1, nfft, ct, rt);
		
        /* Load traces back in */
	for (it=0; it<nt; ++it) trace[it] = rt[it];

        /* Integrate traces   */
	for(iw=0; iw<=nwf+itaper; ++iw){
		ct[iw].i = ct[iw].i*ampi[iw];
		ct[iw].r = ct[iw].r*ampi[iw];
	}
        pfacr(-1, nfft, ct, rt);
        for (it=0; it<m; ++it)  trf[it] = rt[nfft-m+it];
        for (it=0; it<nt+m; ++it)  trf[it+m] = rt[it];

	free1float(rt);
	free1complex(ct);
}


  void mig2d(float *trace,int nt,float ft,float dt,
	float sx,float gx,float **mig,float aperx,
  	int nx,float fx,float dx,int nz,float fz,float dz,
	int ls,int mtmax,float dxm,float fmax,float angmax,
	float **tb,float **pb,float **cs0b,float **angb,int nr,
	float **tsum,int nzt,float fzt,float dzt,int nxt,float fxt,float dxt,
	int npv,float **cssum,float **tvsum,float **mig1)
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
{
	int nxf,nxe,nxtf,nxte,ix,iz,iz0,izt0,nzp,jrs,jrg,jz,jt,mt,jx;
	float xm,x,dis,rxz,ar,srs,srg,srs0,srg0,sigp,z0,rdz,ampd,res0,
	      angs,angg,cs0s,cs0g,ax,ax0,pmin,
	      odt=1.0f/dt,pd,az,sz,sz0,at,td,res,temp;
	float **tmt,**ampt,**ampti,**ampt1=NULL,*tm,*amp,*ampi,*amp1=NULL,
		*tzt,*trf,*zpt;

	tmt = alloc2float(nzt,nxt);
	ampt = alloc2float(nzt,nxt);
	ampti = alloc2float(nzt,nxt);
	tm = alloc1float(nzt);
	tzt = alloc1float(nzt);
	amp = alloc1float(nzt);
	ampi = alloc1float(nzt);
	zpt = alloc1float(nxt);
	trf = alloc1float(nt+2*mtmax);
	if(npv) {
		ampt1 = alloc2float(nzt,nxt);
		amp1 = alloc1float(nzt);
	}

	z0 = (fz-fzt)/dzt;
	rdz = dz/dzt;
	pmin = 1.0f/(2.0f*dxm*fmax);
	
	filt(trace,nt,dt,fmax,ls,mtmax,trf);

	xm = 0.5f*(sx+gx);
	rxz = (angmax==90)?0.0f:1.0f/(float)tan(angmax*PI/180.f);
	nxtf = (int)((xm-aperx-fxt)/dxt);
	if(nxtf<0) nxtf = 0;
	nxte = (int)((xm+aperx-fxt)/dxt+1);
	if(nxte>=nxt) nxte = nxt-1;

	/* compute amplitudes and filter length	*/
	for(ix=nxtf; ix<=nxte; ++ix){
		x = fxt+ix*dxt;
		dis = (xm>=x)?xm-x:x-xm;
		izt0 = (int)(((dis-dxt)*rxz-fzt)/dzt-1);
		if(izt0<0) izt0 = 0;
		if(izt0>=nzt) izt0 = nzt-1;

		ar = (sx>=x)?(sx-x)/dx:(x-sx)/dx;
		jrs = (int)ar;
		if(jrs>nr-2) jrs = nr-2;
		srs = ar-jrs;
		srs0 = 1.0f-srs;
		ar = (gx>=x)?(gx-x)/dx:(x-gx)/dx;
		jrg = (int)ar;
		if(jrg>nr-2) jrg = nr-2;
		srg = ar-jrg;
		srg0 = 1.0f-srg;
		sigp = ((sx-x)*(gx-x)>0)?1.0f:-1.0f;
		zpt[ix] = fzt+(nzt-1)*dzt;

		for(iz=izt0; iz<nzt; ++iz){
			angs = srs0*angb[jrs][iz]+srs*angb[jrs+1][iz]; 
			angg = srg0*angb[jrg][iz]+srg*angb[jrg+1][iz]; 
			cs0s = srs0*cs0b[jrs][iz]+srs*cs0b[jrs+1][iz]; 
			cs0g = srg0*cs0b[jrg][iz]+srg*cs0b[jrg+1][iz]; 
			ampd = (cs0s+cs0g)*(float)cos(0.5f*(angs-angg));
			if(ampd<0.0f) ampd = -ampd;
			ampt[ix][iz] = ampd;

			pd = srs0*pb[jrs][iz]+srs*pb[jrs+1][iz]+sigp 
			     *(srg0*pb[jrg][iz]+srg*pb[jrg+1][iz]);
			if(pd<0.0) pd = -pd;
			temp = pd*dxm*odt;
			if(temp<1) temp = 1.0f;
			if(temp>mtmax) temp = (float)mtmax;
			ampti[ix][iz] = ampd/(temp*temp);
			tmt[ix][iz] = temp;
			if(pd<pmin && zpt[ix]>fzt+(nzt-1.1)*dzt) 
				zpt[ix] = fzt+iz*dzt;

		    if(npv){
			if(cssum[ix][iz]<1.0) 
			     ampt1[ix][iz] = 0; 
			else 
			     ampt1[ix][iz] = tvsum[ix][iz]/cssum[ix][iz];
		    }
		}
	}

	nxf = (int)((xm-aperx-fx)/dx+0.5);
	if(nxf<0) nxf = 0;
	nxe = (int)((xm+aperx-fx)/dx+0.5);
	if(nxe>=nx) nxe = nx-1;
	
	/* interpolate amplitudes and filter length along lateral	*/
	for(ix=nxf; ix<=nxe; ++ix){
		x = fx+ix*dx;
		dis = (xm>=x)?xm-x:x-xm;
		izt0 = (int)((dis*rxz-fzt)/dzt);
		if(izt0<0) izt0 = 0;
		if(izt0>=nzt) izt0 = nzt-1;
		iz0 = (int)((dis*rxz-fz)/dz);
		if(iz0<0) iz0 = 0;
		if(iz0>=nz) iz0 = nz-1;

		ax = (x-fxt)/dxt;
		jx = (int)ax;
		ax = ax-jx;
		if(ax<=0.01f) ax = 0.f;
		if(ax>=0.99f) ax = 1.0f;
		ax0 = 1.0f-ax;
		if(jx>nxte-1) jx = nxte-1;
		if(jx<nxtf) jx = nxtf;

		ar = (sx>=x)?(sx-x)/dx:(x-sx)/dx;
		jrs = (int)ar;
		if(jrs>nr-2) jrs = nr-2;
		srs = ar-jrs;
		srs0 = 1.0f-srs;
		ar = (gx>=x)?(gx-x)/dx:(x-gx)/dx;
		jrg = (int)ar;
		if(jrg>nr-2) jrg = nr-2;
		srg = ar-jrg;
		srg0 = 1.0f-srg;

		for(iz=izt0; iz<nzt; ++iz){
		    tzt[iz] = ax0*tsum[jx][iz]+ax*tsum[jx+1][iz]
				+srs0*tb[jrs][iz]+srs*tb[jrs+1][iz]
				+srg0*tb[jrg][iz]+srg*tb[jrg+1][iz];

		    amp[iz] = ax0*ampt[jx][iz]+ax*ampt[jx+1][iz];
		    ampi[iz] = ax0*ampti[jx][iz]+ax*ampti[jx+1][iz];
		    tm[iz] = ax0*tmt[jx][iz]+ax*tmt[jx+1][iz];

		    if(npv) 
		    	amp1[iz] = ax0*ampt1[jx][iz]+ax*ampt1[jx+1][iz];

		}

		nzp = (int)((ax0*zpt[jx]+ax*zpt[jx+1]-fz)/dz+1.5);
		if(nzp<iz0) nzp = iz0;
		if(nzp>nz) nzp = nz;

		/* interpolate along depth if operater aliasing 	*/
		for(iz=iz0; iz<nzp; ++iz) {
			az = z0+iz*rdz;
			jz = (int)az;
			if(jz>=nzt-1) jz = nzt-2;
			sz = az-jz;
			sz0 = 1.0f-sz;
			td = sz0*tzt[jz]+sz*tzt[jz+1];
			at = (td-ft)*odt+mtmax;
			jt = (int)at;
			if(jt > mtmax && jt < nt+mtmax-1){
			    ampd = sz0*ampi[jz]+sz*ampi[jz+1];
			    mt = (int)(0.5+sz0*tm[jz]+sz*tm[jz+1]);
			    res = at-jt;
			    res0 = 1.0f-res;
 			    temp = (res0*(-trf[jt-mt]+2.0f*trf[jt]-trf[jt+mt]) 
				+res*(-trf[jt-mt+1]+2.0f*trf[jt+1]
				-trf[jt+mt+1]))*ampd;
			    mig[ix][iz] += temp;

			    if(npv) 
				mig1[ix][iz]  += temp
					*(sz0*amp1[jz]+sz*amp1[jz+1]);
			}
		}

		/* interpolate along depth if not operater aliasing 	*/
		for(iz=nzp; iz<nz; ++iz) {
			az = z0+iz*rdz;
			jz = (int)az;
			if(jz>=nzt-1) jz = nzt-2;
			sz = az-jz;
			sz0 = 1.0f-sz;
			td = sz0*tzt[jz]+sz*tzt[jz+1];
			at = (td-ft)*odt;
			jt = (int)at;
			if(jt > 0 && jt < nt-1){
			    ampd = sz0*amp[jz]+sz*amp[jz+1];
			    res = at-jt;
			    res0 = 1.0f-res;
 			    temp = (res0*trace[jt]+res*trace[jt+1])*ampd; 
			    mig[ix][iz] += temp;
			    if(npv) 
				mig1[ix][iz]  += temp
					*(sz0*amp1[jz]+sz*amp1[jz+1]);
			}
		}

	}

	free2float(ampt);
	free2float(ampti);
	free2float(tmt);
	free1float(amp);
	free1float(ampi);
	free1float(zpt);
	free1float(tm);
	free1float(tzt);
	free1float(trf);
	if(npv) {
		free1float(amp1);
		free2float(ampt1);
	}
}

