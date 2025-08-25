/* main.f -- translated by f2c (version 19961017).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Common Block Declarations */

struct {
    integer layer, iblk, id;
    real fid, fid1;
} blk1_;

#define blk1_1 blk1_

struct {
    real c__[23100]	/* was [10][210][11] */;
    integer ivg[2100]	/* was [10][210] */;
} blk2_;

#define blk2_1 blk2_

struct {
    real s[4200]	/* was [10][210][2] */, b[4200]	/* was [10][210][2] */
	    , vm[8400]	/* was [10][210][4] */;
} blk3_;

#define blk3_1 blk3_

struct {
    real xbnd[4200]	/* was [10][210][2] */;
    integer nblk[10], nlayer;
} blk4_;

#define blk4_1 blk4_

struct {
    real xm[1650]	/* was [11][150] */, zm[1650]	/* was [11][150] */, 
	    vf[3000]	/* was [10][150][2] */;
    integer nzed[11], nvel[20]	/* was [10][2] */;
    real xvel[3000]	/* was [10][150][2] */;
} blk5_;

#define blk5_1 blk5_

struct {
    real xr[500], zr[500], ar[1000]	/* was [500][2] */, vr[1000]	/* 
	    was [500][2] */, tr[500], vp[1000]	/* was [500][2] */, vs[1000]	
	    /* was [500][2] */;
} blk6_;

#define blk6_1 blk6_

struct {
    integer refll[11], ircbnd;
} blk7_;

#define blk7_1 blk7_

struct {
    integer irkc;
    real tol, hdenom, hmin;
    integer idump, isrkc, ifast;
} blk8_;

#define blk8_1 blk8_

struct {
    integer itx;
    real vred, time, timer;
} blk9_;

#define blk9_1 blk9_

struct {
    real step, smin, smax;
} blk10_;

#define blk10_1 blk10_

struct {
    real range[10000], tt[10000], rayid[10000], xshtar[10000], fidarr[10000];
    integer idray[2], ntt;
    real ray[20];
} blk11_;

#define blk11_1 blk11_

struct {
    real xmin, xmax, xtmin, xtmax, xmm;
    integer ndecix;
    real xscale;
    integer ntickx;
    real xmint, xmaxt, xtmint, xtmaxt, xmmt;
    integer ndecxt;
    real xscalt;
    integer ntckxt;
    real zmin, zmax, ztmin, ztmax, zmm;
    integer ndeciz;
    real zscale;
    integer ntickz;
    real tmin, tmax, ttmin, ttmax, tmm;
    integer ndecit;
    real tscale;
    integer ntickt;
    real symht, albht;
    integer iplots;
    real orig, sep;
    integer ircol, mcol[5], irrcol, itcol, ncol, colour[20];
} blk12_;

#define blk12_1 blk12_

struct {
    integer icasel;
    real dstepf;
    integer n2, n3, istop, nstepr;
} blk13_;

#define blk13_1 blk13_

struct {
    integer iwave, nccbnd, iccbnd, icbnd[11];
    real vsvp[2100]	/* was [10][210] */;
} blk14_;

#define blk14_1 blk14_

struct {
    integer isrch;
    real tang[40]	/* was [10][4] */;
} blk15_;

#define blk15_1 blk15_

struct {
    integer ntray, ntpts;
} blk16_;

#define blk16_1 blk16_

struct {
    integer ibsmth, nbsmth, npbnd;
    real cosmth[5500]	/* was [11][500] */, xsinc;
} blk17_;

#define blk17_1 blk17_

struct {
    integer nptbnd;
} blk18_;

#define blk18_1 blk18_

struct {
    integer ivarz[1500]	/* was [10][150] */, ivarv[3000]	/* was [10][
	    150][2] */, ivarf[100]	/* was [20][5] */, ivv[8400]	/* 
	    was [10][210][4] */, izv[8400]	/* was [10][210][4] */;
    real cz[16800]	/* was [10][210][4][2] */, cv[42000]	/* was [10][
	    210][4][5] */;
    integer partyp[200], nvar;
    real parorg[200];
} blk19_;

#define blk19_1 blk19_

struct {
    real apart[500000]	/* was [2500][200] */, fpart[40000]	/* was [200][
	    200] */, tobs[2500], uobs[2500], tcalc[2500], xcalc[2500], xscalc[
	    2500];
    integer ipinv[2500], ivray[20], narinv, icalc[2500], ircalc[2500];
} blk20_;

#define blk20_1 blk20_

struct {
    integer ninv;
    real xfinv[200], tfinv[200];
} blk21_;

#define blk21_1 blk21_

struct {
    integer iheadf[10];
    real hws, crit, dhw, tdhw;
    integer ihdwf, ihdw, nhskip;
} blk22_;

#define blk22_1 blk22_

struct {
    real dvmax, dsmax;
    integer idvmax, idsmax, ldvmax[10], ldsmax[11];
} blk23_;

#define blk23_1 blk23_

struct {
    real xpf[2517], tpf[2517], upf[2517];
    integer ipf[2517], ilshot[17];
} blk24_;

#define blk24_1 blk24_

struct {
    integer nfrefl, npfref[20];
    real xfrefl[100]	/* was [20][5] */, zfrefl[100]	/* was [20][5] */;
    integer ifcbnd;
} blk25_;

#define blk25_1 blk25_

struct {
    integer sample[37500]	/* was [250][150] */;
} blk26_;

#define blk26_1 blk26_

struct {
    real mtan[2000], btan[2000], mcotan[2000], bcotan[2000], factan;
} blktan_;

#define blktan_1 blktan_

struct {
    integer iplot, isep, iseg, nseg;
    real xwndow, ywndow;
    integer ibcol, ifcol;
    real sf;
} cplot_;

#define cplot_1 cplot_

/* Table of constant values */

static integer c__1 = 1;
static integer c__3 = 3;
static real c_b174 = (float)1.;
static real c_b214 = (float)0.;
static integer c_n1 = -1;


/*     version 1.3  Aug 1992 */

/*     ---------------------------------------------------------------- */
/*     |                                                              | */
/*     |            ********   R A Y I N V R   ********               | */
/*     |                                                              | */
/*     |            Two-dimensional Ray Tracing Program               | */
/*     |           for Traveltime Modeling and Inversion              | */
/*     |                                                              | */
/*     |                   Written by C. A. Zelt                      | */
/*     |                                                              | */
/*     |                Geological Survey of Canada                   | */
/*     |                  Ottawa, Canada K1A 0Y3                      | */
/*     |                                                              | */
/*     ---------------------------------------------------------------- */


/*     I/O units: */

/*        10 -- input:  program input parameters */

/*        11 -- output: summary ray tracing information for each ray */
/*                      traced */

/*        12 -- output: details of velocity model and each point */
/*                      of each ray traced */

/*        17 -- output: calculated traveltime-distance pairs */

/*        18 -- output: partial derivative and traveltime residual */
/*                      arrays for inversion */

/*        19 -- output: all Calcomp plot calls */

/*        20 -- input:  velocity model */

/*        22 -- output: namelist parameters */

/*        26 -- input:  observed traveltime-distance pairs */

/*        30 -- input:  floating reflectors */

/*        31 -- output: velocity model sampled on a unifrom grid */

/*        32 -- output: floating reflectors */

/*        33 -- output: summary info for each ray traced for input */
/*                      to RAYPLOT to plot minimum traveltime raypaths */

/*        34 -- output: summary info for each observation used for input */
/*                      to RAYPLOT to plot minimum traveltime raypaths */


/*     ---------------------------------------------------------------- */


/* Main program */ MAIN__()
{
    /* Initialized data */

    static real poisbl[50] = { (float)-99.,(float)-99.,(float)-99.,(float)
	    -99.,(float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)-99.,(
	    float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)
	    -99.,(float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)-99.,(
	    float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)
	    -99.,(float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)-99.,(
	    float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)
	    -99.,(float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)-99.,(
	    float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)
	    -99.,(float)-99.,(float)-99. };
    static integer nray[20] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	    -1,-1,-1,-1,-1 };
    static integer itt[160] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0 };
    static integer modi[10] = { 0,0,0,0,0,0,0,0,0,0 };
    static integer ishot[8] = { 0,0,0,0,0,0,0,0 };
    static integer ncbnd[20] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	    -1,-1,-1,-1,-1 };
    static integer nrbnd[20] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    static integer rbnd[150] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    static integer frbnd[20] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    static integer nsmax[20] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	    -1,-1,-1,-1,-1 };
    static integer iturn[20] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
    static integer irayt[320] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	    1 };
    static integer cbnd[150] = { -99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,
	    -99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,
	    -99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,
	    -99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,
	    -99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,
	    -99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,
	    -99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,
	    -99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,
	    -99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99,
	    -99,-99,-99,-99,-99,-99,-99,-99,-99,-99,-99 };
    static integer ibreak[20] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
    static integer ihead[10] = { 0,0,0,0,0,0,0,0,0,0 };
    static real amin[320] = { (float)181.,(float)181.,(float)181.,(float)181.,
	    (float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181. };
    static real amax[320] = { (float)181.,(float)181.,(float)181.,(float)181.,
	    (float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)
	    181.,(float)181.,(float)181.,(float)181.,(float)181.,(float)181.,(
	    float)181.,(float)181.,(float)181. };
    static real space[20] = { (float)0.,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)0.,(float)0. };
    static real zshot[8] = { (float)-9999.,(float)-9999.,(float)-9999.,(float)
	    -9999.,(float)-9999.,(float)-9999.,(float)-9999.,(float)-9999. };
    static real pois[10] = { (float)-99.,(float)-99.,(float)-99.,(float)-99.,(
	    float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)-99.,(float)
	    -99. };

    /* Format strings */
    static char fmt_25[] = "(/\002***  xmax not specified  ***\002/)";
    static char fmt_1[] = "(\002 \002/\002 \002/\002 \002)";
    static char fmt_1105[] = "(/\002***  array size error for number of mode\
l points  ***\002/)";
    static char fmt_15[] = "(i2,1x,10f7.2)";
    static char fmt_235[] = "(3x,10i7)";
    static char fmt_930[] = "(3f10.3,i10)";
    static char fmt_55[] = "(/\002***  no ray codes specified  ***\002/)";
    static char fmt_565[] = "(/\002***  error in array frbnd  ***\002/)";
    static char fmt_545[] = "(i2)";
    static char fmt_585[] = "(/\002***  error in f.in file  ***\002/)";
    static char fmt_895[] = "(\002 \002)";
    static char fmt_865[] = "(\002large velocity gradient in layers: \002,10\
0i4)";
    static char fmt_875[] = "(\002large slope change in boundaries:  \002,10\
0i4)";
    static char fmt_135[] = "(/\002***  max converting boundaries exceeded  \
***/\002)";
    static char fmt_35[] = "(\002shot  ray i.angle  f.angle   dist     dept\
h\002,1x,\002red.time  npts code\002)";
    static char fmt_45[] = "(/\002gr ray npt   x       z      ang1    ang2  \
  v1 \002,1x,\002   v2  lyr bk id iw\002)";
    static char fmt_125[] = "(/\002***  max reflecting boundaries exceeded  \
***\002/)";
    static char fmt_165[] = "(/\002***  reflect boundary greater than # of l\
ayers  ***\002/)";
    static char fmt_95[] = "(\002***  location of shot point outside model  \
***\002)";
    static char fmt_65[] = "(\002***  shot#\002,i4,\002 ray code\002,f5.1\
,\002 no rays traced  ***\002)";
    static char fmt_665[] = "(\002***  shot#\002,i4,\002 ray code\002,f5.1\
,\002 1 ray traced  ***\002)";
    static char fmt_896[] = "(\002***  pnrayf or pnobsf exceeded  ***\002)";
    static char fmt_335[] = "(f12.4,2i4,3f12.4)";
    static char fmt_995[] = "(/\002***  max number of rays reaching surface \
exceeded  ***\002/)";
    static char fmt_375[] = "(\002shot#\002,i4,\002:   ray code\002,f5.1,\
\002:   \002,i3,\002 rays traced \002,a1)";
    static char fmt_75[] = "(/\002***  possible inaccuracies in rngkta  **\
*\002)";
    static char fmt_785[] = "(/\002***  less than nray rays traced for \002,\
i4,\002 ray groups  ***\002)";
    static char fmt_775[] = "(/\002***  attempt to interpolate over ximax  *\
**\002)";
    static char fmt_935[] = "(/\002|----------------------------------------\
-----------\002,\002-------------|\002/\002|\002,64x,\002|\002)";
    static char fmt_905[] = "(\002|  total of \002,i5,\002 rays consisting o\
f \002,i7,\002 points were traced  |\002/\002|\002,64x,\002|\002)";
    static char fmt_915[] = "(\002|                   ***  no rays traced  *\
**\002,21x,\002|\002/\002|\002,64x,\002|\002)";
    static char fmt_925[] = "(\002|           model consists of \002,i2,\002\
 layers and \002,i3,\002 blocks           |\002/,\002|\002,64x,\002|\002/\
\002|----------------------------------------------------------\002,\002----\
--|\002/)";
    static char fmt_835[] = "(2i5)";
    static char fmt_805[] = "(i5,2f15.5)";
    static char fmt_815[] = "(5e12.5)";
    static char fmt_825[] = "(\002Number of data points used: \002,i8/\002RM\
S traveltime residual:    \002,f8.3/\002Normalized chi-squared:   \002,f10.3)"
	    ;
    static char fmt_968[] = "(\002 phase    npts   Trms   chi-squared\002\
/\002-----------------------------------\002)";
    static char fmt_969[] = "(i6,i8,f8.3,f10.3)";
    static char fmt_868[] = "(\002     shot  dir   npts   Trms   chi-square\
d\002/\002------------------------------------------\002)";
    static char fmt_869[] = "(f10.3,i3,i8,f8.3,f10.3)";
    static char fmt_105[] = "(/\002***  error in velocity model  ***\002/)";

    /* System generated locals */
    address a__1[3];
    integer i__1, i__2, i__3[3], i__4;
    real r__1, r__2, r__3, r__4;
    doublereal d__1, d__2;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    integer f_open(), s_rsne(), s_wsfe(), e_wsfe();
    /* Subroutine */ int s_stop();
    integer s_rsfe(), e_rsfe(), do_fio();
    double log();
    integer s_wsfi(), e_wsfi();
    /* Subroutine */ int s_cat();
    integer f_clos(), i_nint(), i_sign();
    double atan(), tan(), r_sign(), pow_dd(), sqrt();
    integer s_wsne();

    /* Local variables */
    static integer i__, j, j1, j2;
    static real am;
    static integer i33;
    static char cn[10];
    static integer is;
    static real xf, tf, uf;
    static integer ir, nx, nz, ia0, icc, idf;
    static real chi;
    static integer idl, icn, idr[16], isf, nsf;
    static real frz;
    static integer nrz, nrv, ist, ics, idt, nrg, npt;
    extern doublereal vel_();
    static integer irs;
    static real xwr, sum, xsn, xsc;
    static integer i2pt, n2pt;
    static real x2pt;
    static char flag__[1];
    static real ainc;
    static integer ibnd, ifam, iend;
    static char form[20];
    static integer ipos[7200], imod, ivel, iray, idot, isum;
    static real stol;
    static integer ntan, invr, ilyr, icnt, iord;
    static char ifmt0[1], ifmt1[4];
    static real hwsm, xsec, zsec;
    extern /* Subroutine */ int xzpt_();
    static real ra2pt[7200];
    static integer iblk1;
    static real ta2pt[7200];
    static integer nsfc;
    static real ho2pt[7200];
    extern /* Subroutine */ int auto_();
    static real pinc;
    static integer nc2pt, ii2pt, ni2pt, no2pt, ic2pt, i1ray;
    static real xobs, tpos, aort, xo2pt[500], tt2pt[7200], aainc;
    extern /* Subroutine */ int sort2_();
    static real sumx;
    static integer idata, iflag;
    static real aamin, aamax;
    static integer ibrka[160], idash, imodf;
    static real tatan[2000];
    static integer poisb[50], poisl[50];
    static real xshot[8], zsmth[500];
    static integer irays, istep;
    static real velht;
    static integer ifo2pt[500], nskip;
    static real dxmod, dzmod, xmmin, xmmax, xmin1d, xmax1d;
    static integer itrev;
    static real xsmax;
    static integer nhray;
    static real ximax, ttunc;
    static integer ncont, iconv, nrayl, irbnd, nshot, iunit, iflagi;
    static real aaimin;
    static integer iaxlab, ictbnd, icont, ifrbnd;
    static real angtan[2000], parunc[3];
    static integer ivraya[160];
    static real xshota[16], zshota[16];
    static integer ishotr[16], ishotw[16], irayps, iszero, itxout, nrskip, 
	    iroute, itxbox, modout, iraysl;
    static real velunc, bndunc;
    static integer ngroup, nrzmax, nrvmax;
    static real xshotc;
    static integer nfrefr;
    extern /* Subroutine */ int calmod_();
    static integer iflagm;
    static real zshift;
    extern /* Subroutine */ int pltmod_();
    static integer iflagp;
    static real xshotr, zshotr;
    static integer idsec, iflags, layer1;
    extern /* Subroutine */ int aldone_();
    static integer irpos, nrayr, iflagl, ihdwm, iflag2, irayf;
    static real xshotf;
    static integer i2flag;
    static real aminr, amaxr;
    static integer nco2pt, iturnt;
    static real angled, tt2min, denom, xdiff, angle;
    extern /* Subroutine */ int trace_(), ttime_(), pltray_(), cells_(), 
	    fxtinv_(), calprt_();
    static integer iflagw;
    extern /* Subroutine */ int empty_(), plttx_(), modwr_();
    static integer ntblk;
    static real trms;
    static integer nused, nars, iflagn;
    extern /* Subroutine */ int plotnd_();

    /* Namelist stuff */

    static Vardesc sep_dv = { "SEP", (char *)&blk12_1.sep, (ftnlen *)0, 4 };
    static ftnlen ray_dims[] = { 1, 20, 1 };
    static Vardesc ray_dv = { "RAY", (char *)blk11_1.ray, ray_dims, 4 };
    static Vardesc tmm_dv = { "TMM", (char *)&blk12_1.tmm, (ftnlen *)0, 4 };
    static Vardesc xmm_dv = { "XMM", (char *)&blk12_1.xmm, (ftnlen *)0, 4 };
    static Vardesc hws_dv = { "HWS", (char *)&blk22_1.hws, (ftnlen *)0, 4 };
    static Vardesc zmm_dv = { "ZMM", (char *)&blk12_1.zmm, (ftnlen *)0, 4 };
    static Vardesc itx_dv = { "ITX", (char *)&blk9_1.itx, (ftnlen *)0, 3 };
    static Vardesc frz_dv = { "FRZ", (char *)&frz, (ftnlen *)0, 4 };
    static Vardesc i2pt_dv = { "I2PT", (char *)&i2pt, (ftnlen *)0, 3 };
    static Vardesc n2pt_dv = { "N2PT", (char *)&n2pt, (ftnlen *)0, 3 };
    static Vardesc x2pt_dv = { "X2PT", (char *)&x2pt, (ftnlen *)0, 4 };
    static ftnlen cbnd_dims[] = { 1, 150, 1 };
    static Vardesc cbnd_dv = { "CBND", (char *)cbnd, cbnd_dims, 3 };
    static Vardesc ibnd_dv = { "IBND", (char *)&ibnd, (ftnlen *)0, 3 };
    static ftnlen amin_dims[] = { 1, 320, 1 };
    static Vardesc amin_dv = { "AMIN", (char *)amin, amin_dims, 4 };
    static ftnlen amax_dims[] = { 1, 320, 1 };
    static Vardesc amax_dv = { "AMAX", (char *)amax, amax_dims, 4 };
    static ftnlen rbnd_dims[] = { 1, 150, 1 };
    static Vardesc rbnd_dv = { "RBND", (char *)rbnd, rbnd_dims, 3 };
    static ftnlen modi_dims[] = { 1, 10, 1 };
    static Vardesc modi_dv = { "MODI", (char *)modi, modi_dims, 3 };
    static ftnlen mcol_dims[] = { 1, 5, 1 };
    static Vardesc mcol_dv = { "MCOL", (char *)blk12_1.mcol, mcol_dims, 3 };
    static Vardesc vred_dv = { "VRED", (char *)&blk9_1.vred, (ftnlen *)0, 4 };
    static ftnlen nray_dims[] = { 1, 20, 1 };
    static Vardesc nray_dv = { "NRAY", (char *)nray, nray_dims, 3 };
    static ftnlen pois_dims[] = { 1, 10, 1 };
    static Vardesc pois_dv = { "POIS", (char *)pois, pois_dims, 4 };
    static Vardesc step_dv = { "STEP", (char *)&blk10_1.step, (ftnlen *)0, 4 }
	    ;
    static Vardesc smin_dv = { "SMIN", (char *)&blk10_1.smin, (ftnlen *)0, 4 }
	    ;
    static Vardesc smax_dv = { "SMAX", (char *)&blk10_1.smax, (ftnlen *)0, 4 }
	    ;
    static Vardesc xmin_dv = { "XMIN", (char *)&blk12_1.xmin, (ftnlen *)0, 4 }
	    ;
    static Vardesc xmax_dv = { "XMAX", (char *)&blk12_1.xmax, (ftnlen *)0, 4 }
	    ;
    static Vardesc zmin_dv = { "ZMIN", (char *)&blk12_1.zmin, (ftnlen *)0, 4 }
	    ;
    static Vardesc xmmt_dv = { "XMMT", (char *)&blk12_1.xmmt, (ftnlen *)0, 4 }
	    ;
    static Vardesc zmax_dv = { "ZMAX", (char *)&blk12_1.zmax, (ftnlen *)0, 4 }
	    ;
    static Vardesc tmin_dv = { "TMIN", (char *)&blk12_1.tmin, (ftnlen *)0, 4 }
	    ;
    static Vardesc tmax_dv = { "TMAX", (char *)&blk12_1.tmax, (ftnlen *)0, 4 }
	    ;
    static Vardesc orig_dv = { "ORIG", (char *)&blk12_1.orig, (ftnlen *)0, 4 }
	    ;
    static Vardesc crit_dv = { "CRIT", (char *)&blk22_1.crit, (ftnlen *)0, 4 }
	    ;
    static Vardesc isep_dv = { "ISEP", (char *)&cplot_1.isep, (ftnlen *)0, 3 }
	    ;
    static Vardesc iseg_dv = { "ISEG", (char *)&cplot_1.iseg, (ftnlen *)0, 3 }
	    ;
    static Vardesc imod_dv = { "IMOD", (char *)&imod, (ftnlen *)0, 3 };
    static Vardesc ivel_dv = { "IVEL", (char *)&ivel, (ftnlen *)0, 3 };
    static Vardesc iray_dv = { "IRAY", (char *)&iray, (ftnlen *)0, 3 };
    static Vardesc idot_dv = { "IDOT", (char *)&idot, (ftnlen *)0, 3 };
    static Vardesc isum_dv = { "ISUM", (char *)&isum, (ftnlen *)0, 3 };
    static Vardesc stol_dv = { "STOL", (char *)&stol, (ftnlen *)0, 4 };
    static Vardesc ntan_dv = { "NTAN", (char *)&ntan, (ftnlen *)0, 3 };
    static Vardesc invr_dv = { "INVR", (char *)&invr, (ftnlen *)0, 3 };
    static Vardesc idata_dv = { "IDATA", (char *)&idata, (ftnlen *)0, 3 };
    static ftnlen ncbnd_dims[] = { 1, 20, 1 };
    static Vardesc ncbnd_dv = { "NCBND", (char *)ncbnd, ncbnd_dims, 3 };
    static Vardesc aamin_dv = { "AAMIN", (char *)&aamin, (ftnlen *)0, 4 };
    static Vardesc aamax_dv = { "AAMAX", (char *)&aamax, (ftnlen *)0, 4 };
    static Vardesc ibcol_dv = { "IBCOL", (char *)&cplot_1.ibcol, (ftnlen *)0, 
	    3 };
    static ftnlen space_dims[] = { 1, 20, 1 };
    static Vardesc space_dv = { "SPACE", (char *)space, space_dims, 4 };
    static ftnlen frbnd_dims[] = { 1, 20, 1 };
    static Vardesc frbnd_dv = { "FRBND", (char *)frbnd, frbnd_dims, 3 };
    static Vardesc albht_dv = { "ALBHT", (char *)&blk12_1.albht, (ftnlen *)0, 
	    4 };
    static Vardesc ifcol_dv = { "IFCOL", (char *)&cplot_1.ifcol, (ftnlen *)0, 
	    3 };
    static Vardesc idash_dv = { "IDASH", (char *)&idash, (ftnlen *)0, 3 };
    static Vardesc npbnd_dv = { "NPBND", (char *)&blk17_1.npbnd, (ftnlen *)0, 
	    3 };
    static ftnlen nrbnd_dims[] = { 1, 20, 1 };
    static Vardesc nrbnd_dv = { "NRBND", (char *)nrbnd, nrbnd_dims, 3 };
    static Vardesc imodf_dv = { "IMODF", (char *)&imodf, (ftnlen *)0, 3 };
    static Vardesc ifast_dv = { "IFAST", (char *)&blk8_1.ifast, (ftnlen *)0, 
	    3 };
    static ftnlen poisb_dims[] = { 1, 50, 1 };
    static Vardesc poisb_dv = { "POISB", (char *)poisb, poisb_dims, 3 };
    static Vardesc idump_dv = { "IDUMP", (char *)&blk8_1.idump, (ftnlen *)0, 
	    3 };
    static ftnlen ishot_dims[] = { 1, 8, 1 };
    static Vardesc ishot_dv = { "ISHOT", (char *)ishot, ishot_dims, 3 };
    static ftnlen nsmax_dims[] = { 1, 20, 1 };
    static Vardesc nsmax_dv = { "NSMAX", (char *)nsmax, nsmax_dims, 3 };
    static ftnlen irayt_dims[] = { 1, 320, 1 };
    static Vardesc irayt_dv = { "IRAYT", (char *)irayt, irayt_dims, 3 };
    static ftnlen poisl_dims[] = { 1, 50, 1 };
    static Vardesc poisl_dv = { "POISL", (char *)poisl, poisl_dims, 3 };
    static ftnlen iturn_dims[] = { 1, 20, 1 };
    static Vardesc iturn_dv = { "ITURN", (char *)iturn, iturn_dims, 3 };
    static ftnlen ivray_dims[] = { 1, 20, 1 };
    static Vardesc ivray_dv = { "IVRAY", (char *)blk20_1.ivray, ivray_dims, 3 
	    };
    static ftnlen xshot_dims[] = { 1, 8, 1 };
    static Vardesc xshot_dv = { "XSHOT", (char *)xshot, xshot_dims, 4 };
    static ftnlen zshot_dims[] = { 1, 8, 1 };
    static Vardesc zshot_dv = { "ZSHOT", (char *)zshot, zshot_dims, 4 };
    static Vardesc xtmin_dv = { "XTMIN", (char *)&blk12_1.xtmin, (ftnlen *)0, 
	    4 };
    static Vardesc xtmax_dv = { "XTMAX", (char *)&blk12_1.xtmax, (ftnlen *)0, 
	    4 };
    static Vardesc xmint_dv = { "XMINT", (char *)&blk12_1.xmint, (ftnlen *)0, 
	    4 };
    static Vardesc xmaxt_dv = { "XMAXT", (char *)&blk12_1.xmaxt, (ftnlen *)0, 
	    4 };
    static Vardesc ztmin_dv = { "ZTMIN", (char *)&blk12_1.ztmin, (ftnlen *)0, 
	    4 };
    static Vardesc ztmax_dv = { "ZTMAX", (char *)&blk12_1.ztmax, (ftnlen *)0, 
	    4 };
    static Vardesc ttmin_dv = { "TTMIN", (char *)&blk12_1.ttmin, (ftnlen *)0, 
	    4 };
    static Vardesc ttmax_dv = { "TTMAX", (char *)&blk12_1.ttmax, (ftnlen *)0, 
	    4 };
    static Vardesc symht_dv = { "SYMHT", (char *)&blk12_1.symht, (ftnlen *)0, 
	    4 };
    static Vardesc ircol_dv = { "IRCOL", (char *)&blk12_1.ircol, (ftnlen *)0, 
	    3 };
    static Vardesc itcol_dv = { "ITCOL", (char *)&blk12_1.itcol, (ftnlen *)0, 
	    3 };
    static Vardesc istop_dv = { "ISTOP", (char *)&blk13_1.istop, (ftnlen *)0, 
	    3 };
    static Vardesc isrch_dv = { "ISRCH", (char *)&blk15_1.isrch, (ftnlen *)0, 
	    3 };
    static Vardesc dvmax_dv = { "DVMAX", (char *)&blk23_1.dvmax, (ftnlen *)0, 
	    4 };
    static Vardesc dsmax_dv = { "DSMAX", (char *)&blk23_1.dsmax, (ftnlen *)0, 
	    4 };
    static Vardesc iplot_dv = { "IPLOT", (char *)&cplot_1.iplot, (ftnlen *)0, 
	    3 };
    static Vardesc irays_dv = { "IRAYS", (char *)&irays, (ftnlen *)0, 3 };
    static Vardesc istep_dv = { "ISTEP", (char *)&istep, (ftnlen *)0, 3 };
    static Vardesc velht_dv = { "VELHT", (char *)&velht, (ftnlen *)0, 4 };
    static Vardesc nskip_dv = { "NSKIP", (char *)&nskip, (ftnlen *)0, 3 };
    static Vardesc dxmod_dv = { "DXMOD", (char *)&dxmod, (ftnlen *)0, 4 };
    static Vardesc dzmod_dv = { "DZMOD", (char *)&dzmod, (ftnlen *)0, 4 };
    static Vardesc xmmin_dv = { "XMMIN", (char *)&xmmin, (ftnlen *)0, 4 };
    static Vardesc xmmax_dv = { "XMMAX", (char *)&xmmax, (ftnlen *)0, 4 };
    static Vardesc xmin1d_dv = { "XMIN1D", (char *)&xmin1d, (ftnlen *)0, 4 };
    static Vardesc xmax1d_dv = { "XMAX1D", (char *)&xmax1d, (ftnlen *)0, 4 };
    static Vardesc itrev_dv = { "ITREV", (char *)&itrev, (ftnlen *)0, 3 };
    static Vardesc xsmax_dv = { "XSMAX", (char *)&xsmax, (ftnlen *)0, 4 };
    static Vardesc nhray_dv = { "NHRAY", (char *)&nhray, (ftnlen *)0, 3 };
    static Vardesc ximax_dv = { "XIMAX", (char *)&ximax, (ftnlen *)0, 4 };
    static Vardesc ttunc_dv = { "TTUNC", (char *)&ttunc, (ftnlen *)0, 4 };
    static ftnlen ibreak_dims[] = { 1, 20, 1 };
    static Vardesc ibreak_dv = { "IBREAK", (char *)ibreak, ibreak_dims, 3 };
    static Vardesc iaxlab_dv = { "IAXLAB", (char *)&iaxlab, (ftnlen *)0, 3 };
    static Vardesc ndecit_dv = { "NDECIT", (char *)&blk12_1.ndecit, (ftnlen *)
	    0, 3 };
    static Vardesc ndecix_dv = { "NDECIX", (char *)&blk12_1.ndecix, (ftnlen *)
	    0, 3 };
    static Vardesc ndeciz_dv = { "NDECIZ", (char *)&blk12_1.ndeciz, (ftnlen *)
	    0, 3 };
    static Vardesc ndecxt_dv = { "NDECXT", (char *)&blk12_1.ndecxt, (ftnlen *)
	    0, 3 };
    static ftnlen poisbl_dims[] = { 1, 50, 1 };
    static Vardesc poisbl_dv = { "POISBL", (char *)poisbl, poisbl_dims, 4 };
    static ftnlen colour_dims[] = { 1, 20, 1 };
    static Vardesc colour_dv = { "COLOUR", (char *)blk12_1.colour, 
	    colour_dims, 3 };
    static Vardesc ntickx_dv = { "NTICKX", (char *)&blk12_1.ntickx, (ftnlen *)
	    0, 3 };
    static Vardesc xtmint_dv = { "XTMINT", (char *)&blk12_1.xtmint, (ftnlen *)
	    0, 4 };
    static Vardesc xtmaxt_dv = { "XTMAXT", (char *)&blk12_1.xtmaxt, (ftnlen *)
	    0, 4 };
    static Vardesc ntckxt_dv = { "NTCKXT", (char *)&blk12_1.ntckxt, (ftnlen *)
	    0, 3 };
    static Vardesc ntickz_dv = { "NTICKZ", (char *)&blk12_1.ntickz, (ftnlen *)
	    0, 3 };
    static Vardesc ntickt_dv = { "NTICKT", (char *)&blk12_1.ntickt, (ftnlen *)
	    0, 3 };
    static Vardesc ibsmth_dv = { "IBSMTH", (char *)&blk17_1.ibsmth, (ftnlen *)
	    0, 3 };
    static Vardesc nbsmth_dv = { "NBSMTH", (char *)&blk17_1.nbsmth, (ftnlen *)
	    0, 3 };
    static Vardesc xwndow_dv = { "XWNDOW", (char *)&cplot_1.xwndow, (ftnlen *)
	    0, 4 };
    static Vardesc ywndow_dv = { "YWNDOW", (char *)&cplot_1.ywndow, (ftnlen *)
	    0, 4 };
    static Vardesc irayps_dv = { "IRAYPS", (char *)&irayps, (ftnlen *)0, 3 };
    static Vardesc iszero_dv = { "ISZERO", (char *)&iszero, (ftnlen *)0, 3 };
    static Vardesc itxout_dv = { "ITXOUT", (char *)&itxout, (ftnlen *)0, 3 };
    static Vardesc nrskip_dv = { "NRSKIP", (char *)&nrskip, (ftnlen *)0, 3 };
    static Vardesc iroute_dv = { "IROUTE", (char *)&iroute, (ftnlen *)0, 3 };
    static Vardesc itxbox_dv = { "ITXBOX", (char *)&itxbox, (ftnlen *)0, 3 };
    static Vardesc modout_dv = { "MODOUT", (char *)&modout, (ftnlen *)0, 3 };
    static Vardesc iraysl_dv = { "IRAYSL", (char *)&iraysl, (ftnlen *)0, 3 };
    static Vardesc velunc_dv = { "VELUNC", (char *)&velunc, (ftnlen *)0, 4 };
    static Vardesc bndunc_dv = { "BNDUNC", (char *)&bndunc, (ftnlen *)0, 4 };

    static Vardesc *pltpar_vl[] = { &iplot_dv, &imod_dv, &ibnd_dv, &idash_dv, 
	    &ivel_dv, &iray_dv, &irays_dv, &irayps_dv, &idot_dv, &itx_dv, &
	    idata_dv, &iszero_dv, &itxout_dv, &istep_dv, &ibreak_dv, &
	    idump_dv, &isum_dv, &symht_dv, &velht_dv, &nskip_dv, &nrskip_dv, &
	    vred_dv, &dvmax_dv, &dsmax_dv, &isep_dv, &iroute_dv, &itcol_dv, &
	    ircol_dv, &mcol_dv, &itxbox_dv, &iseg_dv, &xwndow_dv, &ywndow_dv, 
	    &colour_dv, &ibcol_dv, &ifcol_dv, &modout_dv, &dxmod_dv, &
	    dzmod_dv, &modi_dv, &frz_dv, &xmmin_dv, &xmmax_dv, &xmin1d_dv, &
	    xmax1d_dv };
    static Namelist pltpar = { "PLTPAR", pltpar_vl, 45 };

    static Vardesc *axepar_vl[] = { &iaxlab_dv, &itrev_dv, &albht_dv, &
	    orig_dv, &sep_dv, &xmin_dv, &xmax_dv, &xtmin_dv, &xtmax_dv, &
	    xmm_dv, &ntickx_dv, &ndecix_dv, &xmint_dv, &xmaxt_dv, &xtmint_dv, 
	    &xtmaxt_dv, &xmmt_dv, &ntckxt_dv, &ndecxt_dv, &zmin_dv, &zmax_dv, 
	    &ztmin_dv, &ztmax_dv, &zmm_dv, &ntickz_dv, &ndeciz_dv, &tmin_dv, &
	    tmax_dv, &ttmin_dv, &ttmax_dv, &tmm_dv, &ntickt_dv, &ndecit_dv };
    static Namelist axepar = { "AXEPAR", axepar_vl, 33 };

    static Vardesc *trapar_vl[] = { &ishot_dv, &iraysl_dv, &irayt_dv, &
	    iturn_dv, &isrch_dv, &istop_dv, &ibsmth_dv, &imodf_dv, &xshot_dv, 
	    &zshot_dv, &ray_dv, &nray_dv, &space_dv, &amin_dv, &amax_dv, &
	    nsmax_dv, &aamin_dv, &aamax_dv, &stol_dv, &xsmax_dv, &step_dv, &
	    smin_dv, &smax_dv, &nrbnd_dv, &rbnd_dv, &ncbnd_dv, &cbnd_dv, &
	    pois_dv, &poisb_dv, &poisl_dv, &poisbl_dv, &crit_dv, &hws_dv, &
	    npbnd_dv, &nbsmth_dv, &nhray_dv, &frbnd_dv, &i2pt_dv, &n2pt_dv, &
	    x2pt_dv, &ifast_dv, &ntan_dv };
    static Namelist trapar = { "TRAPAR", trapar_vl, 42 };

    static Vardesc *invpar_vl[] = { &invr_dv, &ivray_dv, &ximax_dv, &ttunc_dv,
	     &velunc_dv, &bndunc_dv };
    static Namelist invpar = { "INVPAR", invpar_vl, 6 };

    /* Fortran I/O blocks */
    static cilist io___82 = { 0, 10, 0, (char *)&pltpar, 0 };
    static cilist io___83 = { 0, 10, 0, (char *)&axepar, 0 };
    static cilist io___84 = { 0, 10, 0, (char *)&trapar, 0 };
    static cilist io___85 = { 0, 10, 0, (char *)&invpar, 0 };
    static cilist io___86 = { 0, 6, 0, fmt_25, 0 };
    static cilist io___88 = { 0, 10, 0, fmt_1, 0 };
    static cilist io___89 = { 0, 6, 0, fmt_1105, 0 };
    static cilist io___96 = { 0, 0, 1, fmt_15, 0 };
    static cilist io___99 = { 0, 0, 1, fmt_15, 0 };
    static cilist io___101 = { 0, 0, 1, fmt_235, 0 };
    static cilist io___103 = { 0, 0, 1, fmt_15, 0 };
    static cilist io___104 = { 0, 0, 1, fmt_15, 0 };
    static cilist io___105 = { 0, 0, 1, fmt_235, 0 };
    static cilist io___106 = { 0, 0, 1, fmt_15, 0 };
    static cilist io___107 = { 0, 0, 1, fmt_15, 0 };
    static cilist io___108 = { 0, 0, 1, fmt_235, 0 };
    static cilist io___112 = { 0, 26, 0, fmt_930, 0 };
    static cilist io___114 = { 0, 6, 0, fmt_55, 0 };
    static cilist io___116 = { 0, 6, 0, fmt_565, 0 };
    static cilist io___117 = { 0, 30, 1, fmt_545, 0 };
    static cilist io___119 = { 0, 6, 0, fmt_585, 0 };
    static icilist io___122 = { 0, ifmt0, 0, "(i1)", 1, 1 };
    static icilist io___125 = { 0, cn, 0, ifmt1, 10, 1 };
    static cilist io___127 = { 0, 30, 0, form, 0 };
    static cilist io___128 = { 0, 30, 0, form, 0 };
    static cilist io___129 = { 0, 30, 0, form, 0 };
    static cilist io___130 = { 0, 6, 0, fmt_565, 0 };
    static cilist io___136 = { 0, 6, 0, fmt_895, 0 };
    static cilist io___137 = { 0, 11, 0, fmt_895, 0 };
    static cilist io___138 = { 0, 6, 0, fmt_865, 0 };
    static cilist io___139 = { 0, 11, 0, fmt_865, 0 };
    static cilist io___140 = { 0, 6, 0, fmt_875, 0 };
    static cilist io___141 = { 0, 11, 0, fmt_875, 0 };
    static cilist io___142 = { 0, 6, 0, fmt_895, 0 };
    static cilist io___143 = { 0, 11, 0, fmt_895, 0 };
    static cilist io___151 = { 0, 6, 0, fmt_135, 0 };
    static cilist io___153 = { 0, 11, 0, fmt_35, 0 };
    static cilist io___154 = { 0, 12, 0, fmt_45, 0 };
    static cilist io___156 = { 0, 6, 0, fmt_125, 0 };
    static cilist io___157 = { 0, 6, 0, fmt_165, 0 };
    static cilist io___173 = { 0, 11, 0, fmt_95, 0 };
    static cilist io___181 = { 0, 6, 0, fmt_125, 0 };
    static cilist io___182 = { 0, 11, 0, fmt_65, 0 };
    static cilist io___196 = { 0, 11, 0, fmt_65, 0 };
    static cilist io___197 = { 0, 11, 0, fmt_665, 0 };
    static cilist io___206 = { 0, 6, 0, fmt_896, 0 };
    static cilist io___228 = { 0, 33, 0, fmt_335, 0 };
    static cilist io___229 = { 0, 6, 0, fmt_995, 0 };
    static cilist io___234 = { 0, 6, 0, fmt_375, 0 };
    static cilist io___235 = { 0, 17, 0, fmt_930, 0 };
    static cilist io___236 = { 0, 6, 0, fmt_75, 0 };
    static cilist io___237 = { 0, 11, 0, fmt_75, 0 };
    static cilist io___238 = { 0, 6, 0, fmt_785, 0 };
    static cilist io___239 = { 0, 11, 0, fmt_785, 0 };
    static cilist io___240 = { 0, 6, 0, fmt_775, 0 };
    static cilist io___241 = { 0, 34, 0, fmt_335, 0 };
    static cilist io___243 = { 0, 6, 0, fmt_935, 0 };
    static cilist io___244 = { 0, 11, 0, fmt_935, 0 };
    static cilist io___245 = { 0, 6, 0, fmt_905, 0 };
    static cilist io___246 = { 0, 11, 0, fmt_905, 0 };
    static cilist io___247 = { 0, 6, 0, fmt_915, 0 };
    static cilist io___248 = { 0, 11, 0, fmt_915, 0 };
    static cilist io___249 = { 0, 6, 0, fmt_925, 0 };
    static cilist io___250 = { 0, 11, 0, fmt_925, 0 };
    static cilist io___252 = { 0, 18, 0, fmt_895, 0 };
    static cilist io___253 = { 0, 18, 0, fmt_835, 0 };
    static cilist io___254 = { 0, 18, 0, fmt_895, 0 };
    static cilist io___255 = { 0, 18, 0, fmt_805, 0 };
    static cilist io___256 = { 0, 18, 0, fmt_895, 0 };
    static cilist io___257 = { 0, 18, 0, fmt_815, 0 };
    static cilist io___258 = { 0, 18, 0, fmt_895, 0 };
    static cilist io___259 = { 0, 18, 0, fmt_815, 0 };
    static cilist io___260 = { 0, 18, 0, fmt_895, 0 };
    static cilist io___261 = { 0, 18, 0, fmt_815, 0 };
    static cilist io___266 = { 0, 18, 0, fmt_895, 0 };
    static cilist io___267 = { 0, 18, 0, fmt_825, 0 };
    static cilist io___268 = { 0, 18, 0, fmt_895, 0 };
    static cilist io___269 = { 0, 6, 0, fmt_895, 0 };
    static cilist io___270 = { 0, 6, 0, fmt_825, 0 };
    static cilist io___271 = { 0, 6, 0, fmt_895, 0 };
    static cilist io___272 = { 0, 11, 0, fmt_895, 0 };
    static cilist io___273 = { 0, 11, 0, fmt_825, 0 };
    static cilist io___274 = { 0, 11, 0, fmt_895, 0 };
    static cilist io___275 = { 0, 11, 0, fmt_968, 0 };
    static cilist io___276 = { 0, 6, 0, fmt_968, 0 };
    static cilist io___279 = { 0, 11, 0, fmt_969, 0 };
    static cilist io___280 = { 0, 6, 0, fmt_969, 0 };
    static cilist io___281 = { 0, 11, 0, fmt_895, 0 };
    static cilist io___282 = { 0, 6, 0, fmt_895, 0 };
    static cilist io___283 = { 0, 11, 0, fmt_868, 0 };
    static cilist io___284 = { 0, 6, 0, fmt_868, 0 };
    static cilist io___290 = { 0, 11, 0, fmt_869, 0 };
    static cilist io___291 = { 0, 6, 0, fmt_869, 0 };
    static cilist io___292 = { 0, 11, 0, fmt_895, 0 };
    static cilist io___293 = { 0, 6, 0, fmt_895, 0 };
    static cilist io___294 = { 0, 22, 0, (char *)&pltpar, 0 };
    static cilist io___295 = { 0, 22, 0, (char *)&axepar, 0 };
    static cilist io___296 = { 0, 22, 0, (char *)&trapar, 0 };
    static cilist io___297 = { 0, 22, 0, (char *)&invpar, 0 };
    static cilist io___298 = { 0, 6, 0, fmt_105, 0 };





/*     version 1.3  Aug 1992 */

/*     parameter statement for RAYINVR */

/*     ---------------------------------------------------------------- */





/*     player - model layers */

/*     ppcntr - points defining a single model layer */
/*              (must be a multiple of 10) */

/*     ppvel - points at which upper and lower layer velocities defined */
/*             (must be a multiple of 10) */

/*     ptrap - trapezoids with a layer */

/*     pshot - shot points */

/*     prayf - ray groups for a single shot */

/*     ptrayf - ray groups for all shots */

/*     ppray - points defining a single ray */

/*     pnrayf - rays in a single group */

/*     pray - rays reaching the surface (not including the search mode) */

/*     prefl - reflecting boundaries for a single group */

/*     preflt - reflecting boundaries for all groups */

/*     pconv - converting boundaries for a single group */

/*     pconvt - converting boundaries for all groups */

/*     pnsmth - points defining smooth layer boundary */

/*     papois - blocks within which Poisson's ratio is altered */

/*     pnvar - model parameters varied in inversion */

/*     prayi - travel times used in inversion */

/*     pfrefl - floating refectors */

/*     ppfref - points defining a single floating reflector */

/*     pn2pt - iterations in two-point ray tracing search */

/*     pnobsf - travel times with the same integer code for a single shot 
*/

/*     pcol - colours for ray groups and observed travel times */

/*    pxgrid - number of grid points in x-direction for output of uniforml
y*/
/*             sampled velocity mdoel */

/*    pzgrid - number of grid points in z-direction for output of uniforml
y*/
/*             sampled velocity mdoel */

/*    pitan - number of intervals at which tangent function is pre-evaluat
ed*/
/*             and used for interpolation */




/*     version 1.3  Aug 1992 */

/*     common blocks for RAYINVR */

/*     ---------------------------------------------------------------- */







/*     initialize parameters */


    ntan = 90;
    blk8_1.ifast = 1;
    xmin1d = (float)-999999.;
    xmax1d = (float)-999999.;
    istep = 0;
    iroute = 1;
    xmmin = (float)-999999.;
    xmmax = (float)-999999.;
    frz = (float)0.;
    dxmod = (float)0.;
    dzmod = (float)0.;
    modout = 0;
    i2pt = 0;
    n2pt = 5;
    x2pt = (float)-1.;
    itxbox = 0;
    nhray = -1;
    blk12_1.ircol = 0;
    blk12_1.itcol = 0;
    cplot_1.isep = 0;
    velunc = (float).1;
    bndunc = (float).1;
    blk23_1.dvmax = (float)1e10;
    blk23_1.dsmax = (float)1e10;
    itrev = 0;
    xsmax = (float)0.;
    blk16_1.ntray = 0;
    blk16_1.ntpts = 0;
    blk15_1.isrch = 0;
    blk13_1.istop = 1;
    blk13_1.nstepr = 15;
    blk20_1.narinv = 0;
    nrskip = 1;
    itxout = 0;
    idata = 0;
    isum = 1;
    idash = 1;
    ivel = 0;
    iaxlab = 1;
    idot = 0;
    iszero = 0;
    ibnd = 1;
    imod = 1;
    iray = 1;
    irayps = 0;
    nskip = 0;
    invr = 0;
    imodf = 0;
    irays = 0;
    iraysl = 0;
    ncont = 1;
    iconv = 0;
    iflagi = 0;
    nrayl = 0;
    ifam = 0;
    ictbnd = 0;
    irbnd = 0;
    nshot = 0;
    ngroup = 0;
    ia0 = 1;
    ttunc = (float).01;
    stol = (float)-1.;
    velht = (float).7;
    aamin = (float)5.;
    aamax = (float)85.;
    aainc = (float).1;
    aaimin = (float)1.;
    ximax = (float)-1.;
    i33 = 0;

    o__1.oerr = 0;
    o__1.ounit = 10;
    o__1.ofnmlen = 4;
    o__1.ofnm = "r.in";
    o__1.orl = 0;
    o__1.osta = "old";
    o__1.oacc = 0;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    f_open(&o__1);

/*     read in program control parameters from unit 10 */

    s_rsne(&io___82);
/*      write(*,pltpar) */
    s_rsne(&io___83);
/*      write(*,axepar) */
    s_rsne(&io___84);
/*      write(*,trapar) */
    s_rsne(&io___85);
/*      write(*,invpar) */

    if (blk12_1.xmax < (float)-99998.) {
	s_wsfe(&io___86);
	e_wsfe();
	s_stop("", 0L);
    }

    if (imodf != 1) {
	iunit = 10;
	s_rsfe(&io___88);
	e_rsfe();
    } else {
	o__1.oerr = 0;
	o__1.ounit = 20;
	o__1.ofnmlen = 4;
	o__1.ofnm = "v.in";
	o__1.orl = 0;
	o__1.osta = "old";
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
	iunit = 20;
    }

    if (FALSE_) {
	s_wsfe(&io___89);
	e_wsfe();
	goto L9999;
    }

    nrzmax = 15;
    nrvmax = 15;
    for (icont = 1; icont <= 11; ++icont) {
	nrz = 1;
	j1 = 1;
	j2 = 10;
L11:
	if (nrz > nrzmax) {
	    goto L211;
	}
	io___96.ciunit = iunit;
	i__1 = s_rsfe(&io___96);
	if (i__1 != 0) {
	    goto L999;
	}
	i__1 = do_fio(&c__1, (char *)&ilyr, (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L999;
	}
	i__2 = j2;
	for (j = j1; j <= i__2; ++j) {
	    i__1 = do_fio(&c__1, (char *)&blk5_1.xm[icont + j * 11 - 12], (
		    ftnlen)sizeof(real));
	    if (i__1 != 0) {
		goto L999;
	    }
	}
	i__1 = e_rsfe();
	if (i__1 != 0) {
	    goto L999;
	}
	io___99.ciunit = iunit;
	i__1 = s_rsfe(&io___99);
	if (i__1 != 0) {
	    goto L999;
	}
	i__1 = do_fio(&c__1, (char *)&icnt, (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L999;
	}
	i__2 = j2;
	for (j = j1; j <= i__2; ++j) {
	    i__1 = do_fio(&c__1, (char *)&blk5_1.zm[icont + j * 11 - 12], (
		    ftnlen)sizeof(real));
	    if (i__1 != 0) {
		goto L999;
	    }
	}
	i__1 = e_rsfe();
	if (i__1 != 0) {
	    goto L999;
	}
	io___101.ciunit = iunit;
	i__1 = s_rsfe(&io___101);
	if (i__1 != 0) {
	    goto L99;
	}
	i__2 = j2;
	for (j = j1; j <= i__2; ++j) {
	    i__1 = do_fio(&c__1, (char *)&blk19_1.ivarz[icont + j * 10 - 11], 
		    (ftnlen)sizeof(integer));
	    if (i__1 != 0) {
		goto L99;
	    }
	}
	i__1 = e_rsfe();
	if (i__1 != 0) {
	    goto L99;
	}
	++nrz;
	if (icnt != 1) {
	    goto L211;
	}
	j1 += 10;
	j2 += 10;
	goto L11;
L211:
	nrv = 1;
	j1 = 1;
	j2 = 10;
L21:
	if (nrv > nrvmax) {
	    goto L311;
	}
	io___103.ciunit = iunit;
	i__1 = s_rsfe(&io___103);
	if (i__1 != 0) {
	    goto L999;
	}
	i__1 = do_fio(&c__1, (char *)&ilyr, (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L999;
	}
	i__2 = j2;
	for (j = j1; j <= i__2; ++j) {
	    i__1 = do_fio(&c__1, (char *)&blk5_1.xvel[icont + (j + 150) * 10 
		    - 1511], (ftnlen)sizeof(real));
	    if (i__1 != 0) {
		goto L999;
	    }
	}
	i__1 = e_rsfe();
	if (i__1 != 0) {
	    goto L999;
	}
	io___104.ciunit = iunit;
	i__1 = s_rsfe(&io___104);
	if (i__1 != 0) {
	    goto L999;
	}
	i__1 = do_fio(&c__1, (char *)&icnt, (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L999;
	}
	i__2 = j2;
	for (j = j1; j <= i__2; ++j) {
	    i__1 = do_fio(&c__1, (char *)&blk5_1.vf[icont + (j + 150) * 10 - 
		    1511], (ftnlen)sizeof(real));
	    if (i__1 != 0) {
		goto L999;
	    }
	}
	i__1 = e_rsfe();
	if (i__1 != 0) {
	    goto L999;
	}
	io___105.ciunit = iunit;
	i__1 = s_rsfe(&io___105);
	if (i__1 != 0) {
	    goto L999;
	}
	i__2 = j2;
	for (j = j1; j <= i__2; ++j) {
	    i__1 = do_fio(&c__1, (char *)&blk19_1.ivarv[icont + (j + 150) * 
		    10 - 1511], (ftnlen)sizeof(integer));
	    if (i__1 != 0) {
		goto L999;
	    }
	}
	i__1 = e_rsfe();
	if (i__1 != 0) {
	    goto L999;
	}
	++nrv;
	if (icnt != 1) {
	    goto L311;
	}
	j1 += 10;
	j2 += 10;
	goto L21;
L311:
	nrv = 1;
	j1 = 1;
	j2 = 10;
L31:
	if (nrv > nrvmax) {
	    goto L411;
	}
	io___106.ciunit = iunit;
	i__1 = s_rsfe(&io___106);
	if (i__1 != 0) {
	    goto L999;
	}
	i__1 = do_fio(&c__1, (char *)&ilyr, (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L999;
	}
	i__2 = j2;
	for (j = j1; j <= i__2; ++j) {
	    i__1 = do_fio(&c__1, (char *)&blk5_1.xvel[icont + (j + 300) * 10 
		    - 1511], (ftnlen)sizeof(real));
	    if (i__1 != 0) {
		goto L999;
	    }
	}
	i__1 = e_rsfe();
	if (i__1 != 0) {
	    goto L999;
	}
	io___107.ciunit = iunit;
	i__1 = s_rsfe(&io___107);
	if (i__1 != 0) {
	    goto L999;
	}
	i__1 = do_fio(&c__1, (char *)&icnt, (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L999;
	}
	i__2 = j2;
	for (j = j1; j <= i__2; ++j) {
	    i__1 = do_fio(&c__1, (char *)&blk5_1.vf[icont + (j + 300) * 10 - 
		    1511], (ftnlen)sizeof(real));
	    if (i__1 != 0) {
		goto L999;
	    }
	}
	i__1 = e_rsfe();
	if (i__1 != 0) {
	    goto L999;
	}
	io___108.ciunit = iunit;
	i__1 = s_rsfe(&io___108);
	if (i__1 != 0) {
	    goto L999;
	}
	i__2 = j2;
	for (j = j1; j <= i__2; ++j) {
	    i__1 = do_fio(&c__1, (char *)&blk19_1.ivarv[icont + (j + 300) * 
		    10 - 1511], (ftnlen)sizeof(integer));
	    if (i__1 != 0) {
		goto L999;
	    }
	}
	i__1 = e_rsfe();
	if (i__1 != 0) {
	    goto L999;
	}
	++nrv;
	if (icnt != 1) {
	    goto L411;
	}
	j1 += 10;
	j2 += 10;
	goto L31;
L411:
	++ncont;
/* L20: */
    }

L99:
    blk4_1.nlayer = ncont - 1;

/*     open I/O units */

    if (cplot_1.iplot == 0) {
	cplot_1.iplot = -1;
    }
    if (cplot_1.iplot == 2) {
	cplot_1.iplot = 0;
    }
    o__1.oerr = 0;
    o__1.ounit = 11;
    o__1.ofnmlen = 6;
    o__1.ofnm = "r1.out";
    o__1.orl = 0;
    o__1.osta = 0;
    o__1.oacc = 0;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    f_open(&o__1);
    if (blk8_1.idump == 1) {
	o__1.oerr = 0;
	o__1.ounit = 12;
	o__1.ofnmlen = 6;
	o__1.ofnm = "r2.out";
	o__1.orl = 0;
	o__1.osta = 0;
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
    }
    if (itxout > 0) {
	o__1.oerr = 0;
	o__1.ounit = 17;
	o__1.ofnmlen = 6;
	o__1.ofnm = "tx.out";
	o__1.orl = 0;
	o__1.osta = 0;
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
    }
    if (cplot_1.iplot <= 0) {
	o__1.oerr = 0;
	o__1.ounit = 19;
	o__1.ofnmlen = 5;
	o__1.ofnm = "p.out";
	o__1.orl = 0;
	o__1.osta = 0;
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
    }
    if (modout > 0) {
	o__1.oerr = 0;
	o__1.ounit = 31;
	o__1.ofnmlen = 5;
	o__1.ofnm = "v.out";
	o__1.orl = 0;
	o__1.osta = 0;
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
    }
    if (i2pt > 0 && iray == 2 && irays == 0 && irayps == 0 && cplot_1.iplot <=
	     0) {
	o__1.oerr = 0;
	o__1.ounit = 33;
	o__1.ofnmlen = 7;
	o__1.ofnm = "ra1.out";
	o__1.orl = 0;
	o__1.osta = 0;
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
	o__1.oerr = 0;
	o__1.ounit = 34;
	o__1.ofnmlen = 7;
	o__1.ofnm = "ra2.out";
	o__1.orl = 0;
	o__1.osta = 0;
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
	i33 = 1;
    }

/*     read in observed data */

    if (idata != 0 || invr == 1 || i2pt != 0) {

	o__1.oerr = 0;
	o__1.ounit = 26;
	o__1.ofnmlen = 5;
	o__1.ofnm = "tx.in";
	o__1.orl = 0;
	o__1.osta = "old";
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);

	isf = 1;
	nsf = 0;
	xshotc = (float)0.;
L910:
	s_rsfe(&io___112);
	do_fio(&c__1, (char *)&blk24_1.xpf[isf - 1], (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&blk24_1.tpf[isf - 1], (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&blk24_1.upf[isf - 1], (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&blk24_1.ipf[isf - 1], (ftnlen)sizeof(integer));
	e_rsfe();
	if (blk24_1.ipf[isf - 1] <= 0) {
	    ++nsf;
	    blk24_1.ilshot[nsf - 1] = isf;
	    xshotc = blk24_1.xpf[isf - 1];
	} else {
	    if (blk9_1.vred != (float)0.) {
		blk24_1.tpf[isf - 1] -= (r__1 = xshotc - blk24_1.xpf[isf - 1],
			 dabs(r__1)) / blk9_1.vred;
	    }
	}
	++isf;
	if (blk24_1.ipf[isf - 2] != -1) {
	    goto L910;
	}
    }

/*     determine if plot parameters for distance axis of travel */
/*     time plot are same as model distance parameters */

    if (blk12_1.xmint < (float)-1e3) {
	blk12_1.xmint = blk12_1.xmin;
    }
    if (blk12_1.xmaxt < (float)-1e3) {
	blk12_1.xmaxt = blk12_1.xmax;
    }
    if (blk12_1.xtmint < (float)-1e3) {
	blk12_1.xtmint = blk12_1.xtmin;
    }
    if (blk12_1.xtmaxt < (float)-1e3) {
	blk12_1.xtmaxt = blk12_1.xtmax;
    }
    if (blk12_1.xmmt < (float)-1e3) {
	blk12_1.xmmt = blk12_1.xmm;
    }
    if (blk12_1.ndecxt < -1) {
	blk12_1.ndecxt = blk12_1.ndecix;
    }
    if (blk12_1.ntckxt < 0) {
	blk12_1.ntckxt = blk12_1.ntickx;
    }

/*     calculate scale of each plot axis */

    blk12_1.xscale = (blk12_1.xmax - blk12_1.xmin) / blk12_1.xmm;
    blk12_1.xscalt = (blk12_1.xmaxt - blk12_1.xmint) / blk12_1.xmmt;
    blk12_1.zscale = -(blk12_1.zmax - blk12_1.zmin) / blk12_1.zmm;
    blk12_1.tscale = (blk12_1.tmax - blk12_1.tmin) / blk12_1.tmm;

    if (itrev == 1) {
	blk12_1.tscale = -blk12_1.tscale;
    }
    if (iroute != 1) {
	cplot_1.isep = 0;
	cplot_1.ibcol = 0;
    }
    if (cplot_1.isep == 2 && imod == 0 && iray == 0 && irays == 0) {
	cplot_1.isep = 3;
    }
    if (n2pt > 15) {
	n2pt = 15;
    }
    if (i2pt == 0) {
	x2pt = (float)0.;
    }
    if (x2pt < (float)0.) {
	x2pt = (blk12_1.xmax - blk12_1.xmin) / (float)2e3;
    }

    blk12_1.ncol = 0;
    for (i__ = 20; i__ >= 1; --i__) {
	if (blk12_1.colour[i__ - 1] >= 0) {
	    blk12_1.ncol = i__;
	    goto L1030;
	}
/* L1020: */
    }
L1030:
    if (blk12_1.ncol == 0) {
	blk12_1.colour[0] = 2;
	blk12_1.colour[1] = 3;
	blk12_1.colour[2] = 4;
	blk12_1.colour[3] = 5;
	blk12_1.colour[4] = 6;
	blk12_1.colour[5] = 8;
	blk12_1.colour[6] = 17;
	blk12_1.colour[7] = 27;
	blk12_1.colour[8] = 22;
	blk12_1.colour[9] = 7;
	blk12_1.ncol = 10;
    }
    for (i__ = 1; i__ <= 5; ++i__) {
	if (blk12_1.mcol[i__ - 1] < 0) {
	    blk12_1.mcol[i__ - 1] = cplot_1.ifcol;
	}
/* L1040: */
    }

    for (i__ = 1; i__ <= 20; ++i__) {
	if (blk11_1.ray[i__ - 1] < (float)1.) {
	    goto L50;
	}
	++ngroup;
/* L40: */
    }
L50:
    if (ngroup == 0) {
	s_wsfe(&io___114);
	e_wsfe();
	goto L900;
    }

    ifrbnd = 0;
    i__1 = ngroup;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (frbnd[i__ - 1] < 0 || frbnd[i__ - 1] > 20) {
	    s_wsfe(&io___116);
	    e_wsfe();
	    s_stop("", 0L);
	}
	if (frbnd[i__ - 1] > 0) {
	    ifrbnd = 1;
	}
/* L540: */
    }

    if (ifrbnd == 1) {
	o__1.oerr = 0;
	o__1.ounit = 30;
	o__1.ofnmlen = 4;
	o__1.ofnm = "f.in";
	o__1.orl = 0;
	o__1.osta = "old";
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
	blk25_1.nfrefl = 0;
L590:
	i__1 = s_rsfe(&io___117);
	if (i__1 != 0) {
	    goto L595;
	}
	i__1 = do_fio(&c__1, (char *)&nfrefr, (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L595;
	}
	i__1 = e_rsfe();
	if (i__1 != 0) {
	    goto L595;
	}
	if (nfrefr < 2 || nfrefr > 5) {
	    s_wsfe(&io___119);
	    e_wsfe();
	    s_stop("", 0L);
	}
	++blk25_1.nfrefl;
	blk25_1.npfref[blk25_1.nfrefl - 1] = nfrefr;
/*       Dal modification */
	iord = (integer) log((real) blk25_1.npfref[blk25_1.nfrefl - 1]) + 1;
	s_wsfi(&io___122);
	do_fio(&c__1, (char *)&iord, (ftnlen)sizeof(integer));
	e_wsfi();
/* Writing concatenation */
	i__3[0] = 2, a__1[0] = "(i";
	i__3[1] = 1, a__1[1] = ifmt0;
	i__3[2] = 1, a__1[2] = ")";
	s_cat(ifmt1, a__1, i__3, &c__3, 4L);
	s_wsfi(&io___125);
	do_fio(&c__1, (char *)&blk25_1.npfref[blk25_1.nfrefl - 1], (ftnlen)
		sizeof(integer));
	e_wsfi();
/* Writing concatenation */
	i__3[0] = 4, a__1[0] = "(3x,";
	i__3[1] = iord, a__1[1] = cn;
	i__3[2] = 5, a__1[2] = "f7.2)";
	s_cat(form, a__1, i__3, &c__3, 20L);
	s_rsfe(&io___127);
	i__1 = blk25_1.npfref[blk25_1.nfrefl - 1];
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&blk25_1.xfrefl[blk25_1.nfrefl + i__ * 20 - 
		    21], (ftnlen)sizeof(real));
	}
	e_rsfe();
	s_rsfe(&io___128);
	i__1 = blk25_1.npfref[blk25_1.nfrefl - 1];
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&blk25_1.zfrefl[blk25_1.nfrefl + i__ * 20 - 
		    21], (ftnlen)sizeof(real));
	}
	e_rsfe();
/* Writing concatenation */
	i__3[0] = 4, a__1[0] = "(3x,";
	i__3[1] = iord, a__1[1] = cn;
	i__3[2] = 3, a__1[2] = "i7)";
	s_cat(form, a__1, i__3, &c__3, 20L);
	s_rsfe(&io___129);
	i__1 = blk25_1.npfref[blk25_1.nfrefl - 1];
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&blk19_1.ivarf[blk25_1.nfrefl + i__ * 20 - 
		    21], (ftnlen)sizeof(integer));
	}
	e_rsfe();
/*       end Dal mod */
/*        read(30,555) (xfrefl(nfrefl,i),i=1,npfref(nfrefl)) */
/*        read(30,555) (zfrefl(nfrefl,i),i=1,npfref(nfrefl)) */
/*        read(30,575) (ivarf(nfrefl,i),i=1,npfref(nfrefl)) */
/* 555     format(3x,<npfref(nfrefl)>f7.2) */
/* 575     format(3x,<npfref(nfrefl)>i7) */
	goto L590;
L595:
	cl__1.cerr = 0;
	cl__1.cunit = 30;
	cl__1.csta = 0;
	f_clos(&cl__1);
	i__1 = ngroup;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (frbnd[i__ - 1] > blk25_1.nfrefl) {
		s_wsfe(&io___130);
		e_wsfe();
		s_stop("", 0L);
	    }
/* L550: */
	}
    }

/*     calculate velocity model parameters */

    calmod_(&ncont, pois, poisb, poisl, poisbl, &invr, &iflagm, &ifrbnd, &
	    xmin1d, &xmax1d);

    if (modout > 0) {

	if (xmmin < (float)-999998.) {
	    xmmin = blk12_1.xmin;
	}
	if (xmmax < (float)-999998.) {
	    xmmax = blk12_1.xmax;
	}

	if (dxmod == (float)0.) {
	    dxmod = (xmmax - xmmin) / (float)20.;
	}
	if (dzmod == (float)0.) {
	    dzmod = (blk12_1.zmax - blk12_1.zmin) / (float)10.;
	}

	r__1 = (xmmax - xmmin) / dxmod;
	nx = i_nint(&r__1);
	r__1 = (blk12_1.zmax - blk12_1.zmin) / dzmod;
	nz = i_nint(&r__1);

	if (modout == 2) {
	    i__1 = nz;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		i__2 = nx;
		for (j = 1; j <= i__2; ++j) {
		    blk26_1.sample[i__ + j * 250 - 251] = 0;
/* L4020: */
		}
/* L4010: */
	    }
	}

    }

    if (blk9_1.itx >= 3 && invr == 0) {
	blk9_1.itx = 2;
    }
    if (itxout == 3 && invr == 0) {
	itxout = 2;
    }
    if (invr == 0 && abs(idata) == 2) {
	idata = i_sign(&c__1, &idata);
    }
    if (iflagm == 1) {
	goto L9999;
    }

    if (blk23_1.idvmax > 0 || blk23_1.idsmax > 0) {
	s_wsfe(&io___136);
	e_wsfe();
	s_wsfe(&io___137);
	e_wsfe();
	if (blk23_1.idvmax > 0) {
	    s_wsfe(&io___138);
	    i__1 = blk23_1.idvmax;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&blk23_1.ldvmax[i__ - 1], (ftnlen)
			sizeof(integer));
	    }
	    e_wsfe();
	    s_wsfe(&io___139);
	    i__1 = blk23_1.idvmax;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&blk23_1.ldvmax[i__ - 1], (ftnlen)
			sizeof(integer));
	    }
	    e_wsfe();
	}
	if (blk23_1.idsmax > 0) {
	    s_wsfe(&io___140);
	    i__1 = blk23_1.idsmax;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&blk23_1.ldsmax[i__ - 1], (ftnlen)
			sizeof(integer));
	    }
	    e_wsfe();
	    s_wsfe(&io___141);
	    i__1 = blk23_1.idsmax;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		do_fio(&c__1, (char *)&blk23_1.ldsmax[i__ - 1], (ftnlen)
			sizeof(integer));
	    }
	    e_wsfe();
	}
	s_wsfe(&io___142);
	e_wsfe();
	s_wsfe(&io___143);
	e_wsfe();
    }

/*     assign values to the arrays xshota, zshota and idr */
/*     and determine nshot */

    for (i__ = 1; i__ <= 8; ++i__) {
	if (ishot[i__ - 1] == -1 || ishot[i__ - 1] == 2) {
	    ++nshot;
	    xshota[nshot - 1] = xshot[i__ - 1];
	    zshota[nshot - 1] = zshot[i__ - 1];
	    idr[nshot - 1] = -1;
	    ishotw[nshot - 1] = -i__;
	    ishotr[nshot - 1] = (i__ << 1) - 1;
	}
	if (ishot[i__ - 1] == 1 || ishot[i__ - 1] == 2) {
	    ++nshot;
	    xshota[nshot - 1] = xshot[i__ - 1];
	    zshota[nshot - 1] = zshot[i__ - 1];
	    idr[nshot - 1] = 1;
	    ishotw[nshot - 1] = i__;
	    ishotr[nshot - 1] = i__ << 1;
	}
/* L290: */
    }

/*     calculate the z coordinate of shot points if not specified */
/*     by the user - assumed to be at the top of the first layer */

    zshift = (r__1 = blk12_1.zmax - blk12_1.zmin, dabs(r__1)) / (float)1e4;
    i__1 = nshot;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (zshota[i__ - 1] < (float)-1e3) {
	    if (xshota[i__ - 1] < blk4_1.xbnd[0] || xshota[i__ - 1] > 
		    blk4_1.xbnd[(blk4_1.nblk[0] + 420) * 10 - 2110]) {
		goto L300;
	    }
	    i__2 = blk4_1.nblk[0];
	    for (j = 1; j <= i__2; ++j) {
		if (xshota[i__ - 1] >= blk4_1.xbnd[(j + 210) * 10 - 2110] && 
			xshota[i__ - 1] <= blk4_1.xbnd[(j + 420) * 10 - 2110])
			 {
		    zshota[i__ - 1] = blk3_1.s[(j + 210) * 10 - 2110] * 
			    xshota[i__ - 1] + blk3_1.b[(j + 210) * 10 - 2110] 
			    + zshift;
		}
/* L310: */
	    }
	}
L300:
	;
    }

/*     assign default value to nray if not specified or */
/*     nray(1) if only it is specified and also ensure that nray<=pnrayf 
*/

    if (nray[0] < 0) {
	for (i__ = 1; i__ <= 20; ++i__) {
	    nray[i__ - 1] = 10;
/* L320: */
	}
    } else {
	if (nray[1] < 0) {
	    if (nray[0] > 200) {
		nray[0] = 200;
	    }
	    for (i__ = 2; i__ <= 20; ++i__) {
		nray[i__ - 1] = nray[0];
/* L330: */
	    }
	} else {
	    for (i__ = 1; i__ <= 20; ++i__) {
		if (nray[i__ - 1] < 0) {
		    nray[i__ - 1] = 10;
		}
		if (nray[i__ - 1] > 200) {
		    nray[i__ - 1] = 200;
		}
/* L340: */
	    }
	}
    }

/*     assign default value to stol if not specified by the user */

    if (stol < (float)0.) {
	stol = (blk12_1.xmax - blk12_1.xmin) / (float)3500.;
    }

/*     assign switch iconv and check array ncbnd for array */
/*     values greater than pconv */

    for (i__ = 1; i__ <= 20; ++i__) {
	if (ncbnd[i__ - 1] > 10) {
	    s_wsfe(&io___151);
	    e_wsfe();
	    goto L900;
	}
	if (ncbnd[i__ - 1] > -1) {
	    iconv = 1;
	}
/* L470: */
    }

/*     plot velocity model */

    if ((imod == 1 || iray > 0 || irays == 1) && cplot_1.isep < 2) {
	pltmod_(&ncont, &ibnd, &imod, &iaxlab, &ivel, &velht, &idash, &ifrbnd,
		 &idata, &iroute, &i33);
    }

/*     calculation of smooth layer boundaries */

    if (blk17_1.ibsmth > 0) {
	i__1 = blk4_1.nlayer + 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    zsmth[0] = (blk17_1.cosmth[i__ + 10] - blk17_1.cosmth[i__ - 1]) / 
		    blk17_1.xsinc;
	    i__2 = blk17_1.npbnd - 1;
	    for (j = 2; j <= i__2; ++j) {
		zsmth[j - 1] = (blk17_1.cosmth[i__ + (j + 1) * 11 - 12] - 
			blk17_1.cosmth[i__ + (j - 1) * 11 - 12]) / (
			blk17_1.xsinc * (float)2.);
/* L660: */
	    }
	    zsmth[blk17_1.npbnd - 1] = (blk17_1.cosmth[i__ + blk17_1.npbnd * 
		    11 - 12] - blk17_1.cosmth[i__ + (blk17_1.npbnd - 1) * 11 
		    - 12]) / blk17_1.xsinc;
	    i__2 = blk17_1.npbnd;
	    for (j = 1; j <= i__2; ++j) {
		blk17_1.cosmth[i__ + j * 11 - 12] = atan(zsmth[j - 1]);
/* L670: */
	    }
/* L680: */
	}
    }
    if (cplot_1.isep > 1 && blk17_1.ibsmth == 2) {
	blk17_1.ibsmth = 1;
    }

    s_wsfe(&io___153);
    e_wsfe();
    if (blk8_1.idump == 1) {
	s_wsfe(&io___154);
	e_wsfe();
    }

    if (nrskip < 1) {
	nrskip = 1;
    }
    if (blk22_1.hws < (float)0.) {
	blk22_1.hws = (blk12_1.xmax - blk12_1.xmin) / (float)25.;
    }
    hwsm = blk22_1.hws;
    blk22_1.crit /= (float)57.29577951;

    for (i__ = 1; i__ <= 20; ++i__) {
	if (nrbnd[i__ - 1] > 10) {
	    s_wsfe(&io___156);
	    e_wsfe();
	    goto L900;
	}
/* L260: */
    }

    for (i__ = 1; i__ <= 150; ++i__) {
	if (rbnd[i__ - 1] >= blk4_1.nlayer) {
	    s_wsfe(&io___157);
	    e_wsfe();
	}
/* L710: */
    }

    if (nsmax[0] < 0) {
	i__1 = ngroup;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    nsmax[i__ - 1] = 10;
/* L350: */
	}
    } else {
	if (nsmax[1] < 0 && ngroup > 1) {
	    i__1 = ngroup;
	    for (i__ = 2; i__ <= i__1; ++i__) {
		nsmax[i__ - 1] = nsmax[0];
/* L360: */
	    }
	}
    }

/*     assign default values to smin and smax if not specified */

    if (blk10_1.smin < (float)0.) {
	blk10_1.smin = (blk12_1.xmax - blk12_1.xmin) / (float)4500.;
    }
    if (blk10_1.smax < (float)0.) {
	blk10_1.smax = (blk12_1.xmax - blk12_1.xmin) / (float)15.;
    }

    if (ximax < (float)0.) {
	ximax = (blk12_1.xmax - blk12_1.xmin) / (float)20.;
    }
    ist = 0;
    iflagp = 0;

    if (blk8_1.ifast == 1) {
	if (ntan > 1000) {
	    ntan = 1000;
	}
	ntan <<= 1;
	ainc = (float)3.141592654 / (real) ntan;
	blktan_1.factan = (real) ntan / (float)3.141592654;

	i__1 = ntan;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    angtan[i__ - 1] = (real) (i__ - 1) * (float)3.141592654 / (real) 
		    ntan;
	    tatan[i__ - 1] = tan(angtan[i__ - 1]);
/* L6010: */
	}
	i__1 = ntan - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    blktan_1.mtan[i__ - 1] = (tatan[i__] - tatan[i__ - 1]) / ainc;
	    blktan_1.btan[i__ - 1] = tatan[i__ - 1] - blktan_1.mtan[i__ - 1] *
		     angtan[i__ - 1];
/* L6020: */
	}

	i__1 = ntan;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    tatan[i__ - 1] = (float)1. / tan(angtan[i__ - 1]);
/* L6030: */
	}
	i__1 = ntan - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    blktan_1.mcotan[i__ - 1] = (tatan[i__] - tatan[i__ - 1]) / ainc;
	    blktan_1.bcotan[i__ - 1] = tatan[i__ - 1] - blktan_1.mcotan[i__ - 
		    1] * angtan[i__ - 1];
/* L6040: */
	}
    }

    if (nshot == 0) {
	goto L1000;
    }

    i__1 = nshot;
    for (is = 1; is <= i__1; ++is) {
	++ist;
	blk1_1.id = idr[is - 1];
	blk1_1.fid = (real) blk1_1.id;
	xshotr = xshota[is - 1];
	zshotr = zshota[is - 1];
	if (ist == 1) {
	    xsec = xshotr;
	    zsec = zshotr;
	    idsec = blk1_1.id;
	    ics = 1;
	    i__2 = blk4_1.nlayer;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		blk15_1.tang[i__ - 1] = (float)999.;
		blk15_1.tang[i__ + 9] = (float)999.;
		blk15_1.tang[i__ + 19] = (float)999.;
		blk15_1.tang[i__ + 29] = (float)999.;
/* L810: */
	    }
	} else {
	    if ((r__1 = xshotr - xsec, dabs(r__1)) < (float).001 && (r__2 = 
		    zshotr - zsec, dabs(r__2)) < (float).001) {
		if (iflags == 1) {
		    goto L60;
		}
		ics = 0;
		if (blk1_1.id != idsec) {
		    i__2 = blk4_1.nlayer;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			blk15_1.tang[i__ - 1] = (float)999.;
			blk15_1.tang[i__ + 9] = (float)999.;
			blk15_1.tang[i__ + 19] = (float)999.;
			blk15_1.tang[i__ + 29] = (float)999.;
/* L830: */
		    }
		}
	    } else {
		xsec = xshotr;
		zsec = zshotr;
		idsec = blk1_1.id;
		ics = 1;
		i__2 = blk4_1.nlayer;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    blk15_1.tang[i__ - 1] = (float)999.;
		    blk15_1.tang[i__ + 9] = (float)999.;
		    blk15_1.tang[i__ + 19] = (float)999.;
		    blk15_1.tang[i__ + 29] = (float)999.;
/* L820: */
		}
	    }
	}
	if (ics == 1) {

	    xzpt_(&xshotr, &zshotr, &layer1, &iblk1, &iflags);

	    if (iflags == 1) {
		s_wsfe(&io___173);
		e_wsfe();
		goto L60;
	    }

	    if ((imod == 1 || iray > 0 || irays == 1) && cplot_1.isep > 1) {
		if (iflagp == 1) {
		    aldone_();
		}
		iflagp = 1;
		pltmod_(&ncont, &ibnd, &imod, &iaxlab, &ivel, &velht, &idash, 
			&ifrbnd, &idata, &iroute, &i33);
	    }

	}
	irbnd = 0;
	ictbnd = 0;

	i__2 = ngroup;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    if (iraysl == 1) {
		irpos = (ishotr[is - 1] - 1) * ngroup + i__;
		if (irayt[irpos - 1] == 0) {
		    irbnd += nrbnd[i__ - 1];
		    ictbnd += ncbnd[i__ - 1];
		    nrayr = 0;
		    goto L70;
		}
	    }
	    blk1_1.id = idr[is - 1];
	    blk1_1.fid = (real) blk1_1.id;
	    blk1_1.fid1 = blk1_1.fid;
	    for (j = 1; j <= 11; ++j) {
		blk7_1.refll[j - 1] = 0;
/* L250: */
	    }
	    for (j = 1; j <= 11; ++j) {
		blk14_1.icbnd[j - 1] = -1;
/* L251: */
	    }
	    ++ifam;
	    nrayr = nray[i__ - 1];
	    iflagl = 0;
	    ibrka[ifam - 1] = ibreak[i__ - 1];
	    ivraya[ifam - 1] = blk20_1.ivray[i__ - 1];
	    i__4 = blk4_1.nlayer;
	    for (j = 1; j <= i__4; ++j) {
		blk22_1.iheadf[j - 1] = ihead[j - 1];
/* L870: */
	    }
	    idl = (integer) blk11_1.ray[i__ - 1];
	    idt = (integer) ((blk11_1.ray[i__ - 1] - (real) idl) * (float)10. 
		    + (float).5);
	    if (idt == 2) {
		if (nrbnd[i__ - 1] > 9) {
		    s_wsfe(&io___181);
		    e_wsfe();
		    s_wsfe(&io___182);
		    do_fio(&c__1, (char *)&ishotw[is - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&blk11_1.ray[i__ - 1], (ftnlen)
			    sizeof(real));
		    e_wsfe();
		    nrayr = 0;
		    goto L69;
		}
		blk7_1.refll[0] = idl;
		if (nrbnd[i__ - 1] > 0) {
		    i__4 = nrbnd[i__ - 1];
		    for (j = 1; j <= i__4; ++j) {
			++irbnd;
			blk7_1.refll[j] = rbnd[irbnd - 1];
/* L230: */
		    }
		}
	    } else {
		if (nrbnd[i__ - 1] > 0) {
		    i__4 = nrbnd[i__ - 1];
		    for (j = 1; j <= i__4; ++j) {
			++irbnd;
			blk7_1.refll[j - 1] = rbnd[irbnd - 1];
/* L240: */
		    }
		}
	    }
	    if (idt == 3) {
		blk22_1.iheadf[idl - 1] = 1;
		blk22_1.ihdwf = 1;
		ihdwm = 1;
		blk22_1.hws = hwsm;
		blk22_1.tdhw = (float)0.;
		if (nhray < 0) {
		    nrayr = 200;
		} else {
		    nrayr = nhray;
		}
	    } else {
		blk22_1.ihdwf = -1;
		ihdwm = -1;
	    }
	    if (ncbnd[i__ - 1] > 0) {
		i__4 = ncbnd[i__ - 1];
		for (j = 1; j <= i__4; ++j) {
		    ++ictbnd;
		    blk14_1.icbnd[j - 1] = cbnd[ictbnd - 1];
/* L630: */
		}
	    }

	    if (blk12_1.ircol == 1) {
		blk12_1.irrcol = blk12_1.colour[(blk20_1.ivray[i__ - 1] - 1) %
			 blk12_1.ncol];
	    }
	    if (blk12_1.ircol == 2) {
		blk12_1.irrcol = blk12_1.colour[(is - 1) % blk12_1.ncol];
	    }
	    if (blk12_1.ircol == 3) {
		blk12_1.irrcol = blk12_1.colour[(i__ - 1) % blk12_1.ncol];
	    }

	    if (nrayr <= 0) {
		goto L69;
	    }

	    if (i2pt > 0) {
		iflag2 = 0;
		nsfc = 1;
		isf = blk24_1.ilshot[nsfc - 1];
L1110:
		xf = blk24_1.xpf[isf - 1];
		tf = blk24_1.tpf[isf - 1];
		uf = blk24_1.upf[isf - 1];
		irayf = blk24_1.ipf[isf - 1];
		if (irayf < 0) {
		    goto L1200;
		}
		if (irayf == 0) {
		    xshotf = xf;
		    idf = r_sign(&c_b174, &tf);
		    if ((r__1 = xshotr - xshotf, dabs(r__1)) < (float).001 && 
			    idr[is - 1] == idf) {
			i2flag = 1;
			++isf;
		    } else {
			i2flag = 0;
			++nsfc;
			isf = blk24_1.ilshot[nsfc - 1];
		    }
		} else {
		    if (i2flag == 1 && blk20_1.ivray[i__ - 1] == irayf) {
			iflag2 = 1;
			goto L1200;
		    }
		    ++isf;
		}
		goto L1110;
L1200:
		if (iflag2 == 0) {
		    nrayr = 0;
		    goto L69;
		}
	    }

	    auto_(&xshotr, &zshotr, &i__, &ifam, &idl, &idt, &aminr, &amaxr, &
		    aamin, &aamax, &layer1, &iblk1, &aainc, &aaimin, &nsmax[
		    i__ - 1], &iflag, &iturn[i__ - 1], &amin[ia0 - 1], &amax[
		    ia0 - 1], &ia0, &stol, &irays, &nskip, &idot, &irayps, &
		    xsmax, &istep);

	    if (iflag != 0) {
		s_wsfe(&io___196);
		do_fio(&c__1, (char *)&ishotw[is - 1], (ftnlen)sizeof(integer)
			);
		do_fio(&c__1, (char *)&blk11_1.ray[i__ - 1], (ftnlen)sizeof(
			real));
		e_wsfe();
		nrayr = 0;
		++nrayl;
		iflagl = 1;
		goto L69;
	    }
	    if (amaxr == aminr && blk22_1.ihdwf != 1) {
		if (nrayr > 1) {
		    s_wsfe(&io___197);
		    do_fio(&c__1, (char *)&ishotw[is - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&blk11_1.ray[i__ - 1], (ftnlen)
			    sizeof(real));
		    e_wsfe();
		    ++nrayl;
		    iflagl = 1;
		}
		nrayr = 1;
	    }
	    if (nrayr > 1) {
		if (idt == 2) {
		    if (amaxr <= aminr) {
			amaxr = (float)90.;
		    }
		}
		if (space[i__ - 1] > (float)0.) {
		    pinc = space[i__ - 1];
		} else {
		    if (idt == 2) {
			pinc = (float)2.;
		    } else {
			pinc = (float)1.;
		    }
		}
		d__1 = (doublereal) ((real) (nrayr - 1));
		d__2 = (doublereal) pinc;
		ainc = (amaxr - aminr) / pow_dd(&d__1, &d__2);
	    } else {
		pinc = (float)1.;
		ainc = (float)0.;
	    }
	    iend = 0;
	    blk21_1.ninv = 0;
	    blk25_1.ifcbnd = frbnd[i__ - 1];
	    nc2pt = 0;

	    if (i2pt > 0 && nrayr > 1) {
		ii2pt = i2pt;
		ni2pt = 1;
		no2pt = 0;
		nco2pt = 0;
		ic2pt = 0;

		nsfc = 1;
		isf = blk24_1.ilshot[nsfc - 1];
L1100:
		xf = blk24_1.xpf[isf - 1];
		tf = blk24_1.tpf[isf - 1];
		uf = blk24_1.upf[isf - 1];
		irayf = blk24_1.ipf[isf - 1];
		if (irayf < 0) {
		    goto L1199;
		}
		if (irayf == 0) {
		    xshotf = xf;
		    idf = r_sign(&c_b174, &tf);
		    if ((r__1 = xshotr - xshotf, dabs(r__1)) < (float).001 && 
			    idr[is - 1] == idf) {
			i2flag = 1;
			++isf;
		    } else {
			i2flag = 0;
			++nsfc;
			isf = blk24_1.ilshot[nsfc - 1];
		    }
		} else {
		    if (i2flag == 1 && blk20_1.ivray[i__ - 1] == irayf) {
			++no2pt;
			if (no2pt > 200) {
			    s_wsfe(&io___206);
			    e_wsfe();
			    goto L1199;
			}
			xo2pt[no2pt - 1] = xf;
			ifo2pt[no2pt - 1] = 0;
		    }
		    ++isf;
		}
		goto L1100;
L1199:
		if (no2pt == 0) {
		    ni2pt = 0;
		}
	    } else {
		ni2pt = 0;
		ii2pt = 0;
	    }

L91:
	    ir = 0;
	    nrg = 0;
	    blk22_1.ihdwf = ihdwm;
	    blk22_1.tdhw = (float)0.;
	    blk22_1.dhw = (float)0.;
	    i1ray = 1;
L90:
	    ++ir;
	    if (ir > nrayr && ni2pt <= 1) {
		goto L890;
	    }
	    if (i2pt == 0 && iend == 1) {
		goto L890;
	    }
	    blk7_1.ircbnd = 1;
	    blk14_1.iccbnd = 1;
	    blk14_1.iwave = 1;
	    blk22_1.ihdw = 0;
	    if (blk14_1.icbnd[0] == 0) {
		blk14_1.iwave = -blk14_1.iwave;
		blk14_1.iccbnd = 2;
	    }
	    blk18_1.nptbnd = 0;
	    blk14_1.nccbnd = 0;
	    blk1_1.id = idr[is - 1];
	    blk1_1.fid = (real) blk1_1.id;
	    blk1_1.fid1 = blk1_1.fid;
	    iturnt = 0;
	    if (nc2pt <= 1) {
		d__1 = (doublereal) ((real) (ir - 1));
		d__2 = (doublereal) pinc;
		angled = aminr + ainc * pow_dd(&d__1, &d__2);
		if (amaxr > aminr) {
		    if (angled > amaxr) {
			angled = amaxr;
			iend = 1;
		    }
		} else {
		    if (angled < amaxr) {
			angled = amaxr;
			iend = 1;
		    }
		}
		if (ir == nrayr && nrayr > 1) {
		    angled = amaxr;
		}
	    } else {
L891:
		++nco2pt;
		if (nco2pt > no2pt) {
		    goto L890;
		}
		if (ifo2pt[nco2pt - 1] != 0 && ii2pt > 0) {
		    goto L891;
		}
		xobs = xo2pt[nco2pt - 1];
		tt2min = (float)1e10;
		i__4 = nc2pt - 1;
		for (j = 1; j <= i__4; ++j) {
		    if (ra2pt[j - 1] >= xobs && ra2pt[j] <= xobs || ra2pt[j - 
			    1] <= xobs && ra2pt[j] >= xobs) {
			denom = ra2pt[j] - ra2pt[j - 1];
			if (denom != (float)0.) {
			    tpos = (tt2pt[j] - tt2pt[j - 1]) / denom * (xobs 
				    - ra2pt[j - 1]) + tt2pt[j - 1];
			} else {
			    tpos = (tt2pt[j] + tt2pt[j - 1]) / (float)2.;
			}
			if (tpos < tt2min) {
			    tt2min = tpos;
			    if (denom != (float)0.) {
				aort = (ta2pt[j] - ta2pt[j - 1]) / denom * (
					xobs - ra2pt[j - 1]) + ta2pt[j - 1];
			    } else {
				aort = (ta2pt[j] + ta2pt[j - 1]) / (float)2.;
			    }
			    if (blk22_1.ihdwf != 1) {
				angled = aort;
			    } else {
				blk22_1.tdhw = aort;
				blk22_1.hws = blk22_1.tdhw;
			    }
/* Computing MIN */
			    r__3 = (r__1 = xobs - ra2pt[j - 1], dabs(r__1)), 
				    r__4 = (r__2 = xobs - ra2pt[j], dabs(r__2)
				    );
			    xdiff = dmin(r__3,r__4);
			    if (xdiff < x2pt) {
				ifo2pt[nco2pt - 1] = 1;
			    }
			}
		    }
/* L892: */
		}
		if (tt2min > (float)1e9 || ifo2pt[nco2pt - 1] != 0 && ii2pt > 
			0) {
		    goto L891;
		}
	    }
	    angle = blk1_1.fid * ((float)90. - angled) / (float)57.29577951;
	    if (ir > 1 && blk22_1.ihdwf != 1) {
		if (angle == am) {
		    goto L90;
		}
	    }
	    am = angle;
	    if (blk1_1.fid1 * angle < (float)0.) {
		blk1_1.id = -blk1_1.id;
		blk1_1.fid = (real) blk1_1.id;
	    }
	    blk1_1.layer = layer1;
	    blk1_1.iblk = iblk1;
	    npt = 1;
	    blk6_1.xr[0] = xshotr;
	    blk6_1.zr[0] = zshotr;
	    blk6_1.ar[0] = (float)0.;
	    blk6_1.ar[500] = angle;
	    blk6_1.vr[0] = (float)0.;
	    blk6_1.vp[0] = (float)0.;
	    blk6_1.vs[0] = (float)0.;
	    blk6_1.vp[500] = vel_(&xshotr, &zshotr);
	    blk6_1.vs[500] = blk6_1.vp[500] * blk14_1.vsvp[layer1 + iblk1 * 
		    10 - 11];
	    if (blk14_1.iwave == 1) {
		blk6_1.vr[500] = blk6_1.vp[500];
	    } else {
		blk6_1.vr[500] = blk6_1.vs[500];
	    }
	    blk11_1.idray[0] = layer1;
	    blk11_1.idray[1] = 1;

	    if (ii2pt > 0) {
		irs = 0;
	    } else {
		irs = ir;
	    }
	    if (invr == 1 && irs > 0) {
		++blk21_1.ninv;
		i__4 = blk19_1.nvar;
		for (j = 1; j <= i__4; ++j) {
		    blk20_1.fpart[blk21_1.ninv + j * 200 - 201] = (float)0.;
/* L80: */
		}
	    }
	    ++nrg;
	    blk22_1.nhskip = 0;

	    trace_(&npt, &ifam, &irs, &iturnt, &invr, &xsmax, &iflag, &idl, &
		    idt, &iray, &ii2pt, &i1ray, &modout);

	    ttime_(&ishotw[is - 1], &xshotr, &npt, &irs, &angled, &ifam, itt, 
		    &iszero, &iflag);

/*              if(irs.eq.0.and.vr(npt,2).gt.0.) then */
	    if (irs == 0) {
		++ic2pt;
		if (blk22_1.ihdwf != 1) {
		    ta2pt[ic2pt - 1] = angled;
		} else {
		    ta2pt[ic2pt - 1] = blk22_1.tdhw - blk22_1.hws;
		}
		ra2pt[ic2pt - 1] = blk6_1.xr[npt - 1];
		tt2pt[ic2pt - 1] = blk9_1.timer;
		if (blk6_1.vr[npt + 499] <= (float)0.) {
		    tt2pt[ic2pt - 1] = (float)1e20;
		}
	    }

	    if ((iray == 1 || iray == 2 && blk6_1.vr[npt + 499] > (float)0.) 
		    && (ir - 1) % nrskip == 0 && irs > 0 || irays == 1 && irs 
		    == 0) {
		i__4 = max(nskip,blk22_1.nhskip);
		pltray_(&npt, &i__4, &idot, &irayps, &istep, &angled);
		if (i33 == 1) {
		    if (iszero == 1) {
			xwr = (r__1 = blk11_1.xshtar[blk11_1.ntt - 2] - xobs, 
				dabs(r__1));
		    } else {
			xwr = xobs;
		    }
		    s_wsfe(&io___228);
		    do_fio(&c__1, (char *)&blk11_1.xshtar[blk11_1.ntt - 2], (
			    ftnlen)sizeof(real));
		    do_fio(&c__1, (char *)&ivraya[ifam - 1], (ftnlen)sizeof(
			    integer));
		    do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&xwr, (ftnlen)sizeof(real));
		    do_fio(&c__1, (char *)&blk11_1.tt[blk11_1.ntt - 2], (
			    ftnlen)sizeof(real));
		    e_wsfe();
		}
	    }

	    if (blk6_1.vr[npt + 499] > (float)0. && irs > 0 && modout == 2) {
		cells_(&npt, &xmmin, &dxmod, &dzmod);
	    }

	    if (invr == 1 && irs > 0) {
		fxtinv_(&npt);
	    }

	    if (blk22_1.ihdwf == 0) {
		goto L890;
	    }
	    if (blk11_1.ntt > 10000) {
		s_wsfe(&io___229);
		e_wsfe();
		goto L890;
	    }
	    goto L90;

L890:

	    if (ii2pt > 0) {
		nc2pt = ic2pt;
		if (ni2pt > 1) {
		    sort2_(ta2pt, ipos, &nc2pt);
		    i__4 = nc2pt;
		    for (j = 1; j <= i__4; ++j) {
			ho2pt[j - 1] = ra2pt[j - 1];
/* L893: */
		    }
		    i__4 = nc2pt;
		    for (j = 1; j <= i__4; ++j) {
			ra2pt[j - 1] = ho2pt[ipos[j - 1] - 1];
/* L894: */
		    }
		    i__4 = nc2pt;
		    for (j = 1; j <= i__4; ++j) {
			ho2pt[j - 1] = tt2pt[j - 1];
/* L897: */
		    }
		    i__4 = nc2pt;
		    for (j = 1; j <= i__4; ++j) {
			tt2pt[j - 1] = ho2pt[ipos[j - 1] - 1];
/* L898: */
		    }
		}
		++ni2pt;
		if (ni2pt > n2pt) {
		    ii2pt = 0;
		}
		nco2pt = 0;
		goto L91;
	    }

	    if (blk21_1.ninv > 0) {
		calprt_(&xshotr, &i__, &ivraya[ifam - 1], &idr[is - 1], &
			ximax, &iflagw, &iszero, &x2pt);
	    }
	    if (iflagw == 1) {
		iflagi = 1;
	    }

	    if (iray > 0 || irays == 1) {
		empty_();
	    }

	    nrayr = nrg;
L69:
	    if (iflagl == 1) {
		*(unsigned char *)flag__ = '*';
	    } else {
		*(unsigned char *)flag__ = ' ';
	    }
	    s_wsfe(&io___234);
	    do_fio(&c__1, (char *)&ishotw[is - 1], (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&blk11_1.ray[i__ - 1], (ftnlen)sizeof(real))
		    ;
	    do_fio(&c__1, (char *)&nrayr, (ftnlen)sizeof(integer));
	    do_fio(&c__1, flag__, 1L);
	    e_wsfe();

	    if (blk11_1.ntt > 10000) {
		goto L1000;
	    }

L70:
	    ;
	}

	if (cplot_1.isep == 2 && (blk9_1.itx > 0 && blk11_1.ntt > 1 || idata 
		!= 0 || itxout > 0)) {
	    r__1 = (real) idr[is - 1];
	    plttx_(&ifam, itt, &iszero, &idata, &iaxlab, xshota, idr, &nshot, 
		    &itxout, ibrka, ivraya, &ttunc, &itrev, &xshotr, &r__1, &
		    itxbox, &iroute);
	}

L60:
	;
    }

L1000:
    if ((cplot_1.isep < 2 || cplot_1.isep == 3) && (blk9_1.itx > 0 && 
	    blk11_1.ntt > 1 || idata != 0 || itxout > 0)) {
	if (cplot_1.isep > 0 && blk12_1.iplots == 1) {
	    aldone_();
	}
	plttx_(&ifam, itt, &iszero, &idata, &iaxlab, xshota, idr, &nshot, &
		itxout, ibrka, ivraya, &ttunc, &itrev, &xshotr, &c_b174, &
		itxbox, &iroute);
    }

    if (itxout > 0) {
	s_wsfe(&io___235);
	do_fio(&c__1, (char *)&c_b214, (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&c_b214, (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&c_b214, (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&c_n1, (ftnlen)sizeof(integer));
	e_wsfe();
    }

    if (blk8_1.irkc == 1) {
	s_wsfe(&io___236);
	e_wsfe();
	s_wsfe(&io___237);
	e_wsfe();
    }

    if (nrayl > 0) {
	s_wsfe(&io___238);
	do_fio(&c__1, (char *)&nrayl, (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___239);
	do_fio(&c__1, (char *)&nrayl, (ftnlen)sizeof(integer));
	e_wsfe();
    }

    if (iflagi == 1) {
	s_wsfe(&io___240);
	e_wsfe();
    }

    if (i33 == 1) {
	i__1 = blk20_1.narinv;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    s_wsfe(&io___241);
	    do_fio(&c__1, (char *)&blk20_1.xscalc[i__ - 1], (ftnlen)sizeof(
		    real));
	    i__4 = (i__2 = blk20_1.icalc[i__ - 1], abs(i__2));
	    do_fio(&c__1, (char *)&i__4, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&blk20_1.ircalc[i__ - 1], (ftnlen)sizeof(
		    integer));
	    do_fio(&c__1, (char *)&blk20_1.xcalc[i__ - 1], (ftnlen)sizeof(
		    real));
	    do_fio(&c__1, (char *)&blk20_1.tcalc[i__ - 1], (ftnlen)sizeof(
		    real));
	    e_wsfe();
/* L1033: */
	}
    }

    if (modout > 0) {
	modwr_(&modout, &dxmod, &dzmod, modi, &ifrbnd, &frz, &xmmin, &xmmax);
    }

L900:
    ntblk = 0;
    i__1 = blk4_1.nlayer;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ntblk += blk4_1.nblk[i__ - 1];
/* L920: */
    }

    if (isum > 0) {
	s_wsfe(&io___243);
	e_wsfe();
    }
    s_wsfe(&io___244);
    e_wsfe();
    if (blk16_1.ntpts > 0) {
	if (isum > 0) {
	    s_wsfe(&io___245);
	    do_fio(&c__1, (char *)&blk16_1.ntray, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&blk16_1.ntpts, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	s_wsfe(&io___246);
	do_fio(&c__1, (char *)&blk16_1.ntray, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&blk16_1.ntpts, (ftnlen)sizeof(integer));
	e_wsfe();
    } else {
	if (isum > 0) {
	    s_wsfe(&io___247);
	    e_wsfe();
	}
	s_wsfe(&io___248);
	e_wsfe();
    }
    if (isum > 0) {
	s_wsfe(&io___249);
	do_fio(&c__1, (char *)&blk4_1.nlayer, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ntblk, (ftnlen)sizeof(integer));
	e_wsfe();
    }
    s_wsfe(&io___250);
    do_fio(&c__1, (char *)&blk4_1.nlayer, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&ntblk, (ftnlen)sizeof(integer));
    e_wsfe();

    if (invr == 1) {

	o__1.oerr = 0;
	o__1.ounit = 18;
	o__1.ofnmlen = 5;
	o__1.ofnm = "i.out";
	o__1.orl = 0;
	o__1.osta = 0;
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);

	parunc[0] = bndunc;
	parunc[1] = velunc;
	parunc[2] = bndunc;
	s_wsfe(&io___252);
	e_wsfe();
	s_wsfe(&io___253);
	do_fio(&c__1, (char *)&blk20_1.narinv, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&blk19_1.nvar, (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___254);
	e_wsfe();
	i__1 = blk19_1.nvar;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    s_wsfe(&io___255);
	    do_fio(&c__1, (char *)&blk19_1.partyp[i__ - 1], (ftnlen)sizeof(
		    integer));
	    do_fio(&c__1, (char *)&blk19_1.parorg[i__ - 1], (ftnlen)sizeof(
		    real));
	    do_fio(&c__1, (char *)&parunc[blk19_1.partyp[i__ - 1] - 1], (
		    ftnlen)sizeof(real));
	    e_wsfe();
/* L801: */
	}
	s_wsfe(&io___256);
	e_wsfe();
	i__1 = blk20_1.narinv;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    s_wsfe(&io___257);
	    i__2 = blk19_1.nvar;
	    for (j = 1; j <= i__2; ++j) {
		do_fio(&c__1, (char *)&blk20_1.apart[i__ + j * 2500 - 2501], (
			ftnlen)sizeof(real));
	    }
	    e_wsfe();
/* L840: */
	}
	s_wsfe(&io___258);
	e_wsfe();
	s_wsfe(&io___259);
	i__1 = blk20_1.narinv;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    r__1 = blk20_1.tobs[i__ - 1] - blk20_1.tcalc[i__ - 1];
	    do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
	}
	e_wsfe();
	s_wsfe(&io___260);
	e_wsfe();
	s_wsfe(&io___261);
	i__1 = blk20_1.narinv;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_fio(&c__1, (char *)&blk20_1.uobs[i__ - 1], (ftnlen)sizeof(real)
		    );
	}
	e_wsfe();

	if (blk20_1.narinv > 1) {
	    sum = (float)0.;
	    sumx = (float)0.;
	    i__1 = blk20_1.narinv;
	    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing 2nd power */
		r__1 = blk20_1.tobs[i__ - 1] - blk20_1.tcalc[i__ - 1];
		sum += r__1 * r__1;
/* Computing 2nd power */
		r__1 = (blk20_1.tobs[i__ - 1] - blk20_1.tcalc[i__ - 1]) / 
			blk20_1.uobs[i__ - 1];
		sumx += r__1 * r__1;
/* L850: */
	    }
	    trms = sqrt(sum / (real) blk20_1.narinv);
	    chi = sumx / (real) (blk20_1.narinv - 1);
	    s_wsfe(&io___266);
	    e_wsfe();
	    s_wsfe(&io___267);
	    do_fio(&c__1, (char *)&blk20_1.narinv, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&trms, (ftnlen)sizeof(real));
	    do_fio(&c__1, (char *)&chi, (ftnlen)sizeof(real));
	    e_wsfe();
	    s_wsfe(&io___268);
	    e_wsfe();
	    s_wsfe(&io___269);
	    e_wsfe();
	    s_wsfe(&io___270);
	    do_fio(&c__1, (char *)&blk20_1.narinv, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&trms, (ftnlen)sizeof(real));
	    do_fio(&c__1, (char *)&chi, (ftnlen)sizeof(real));
	    e_wsfe();
	    s_wsfe(&io___271);
	    e_wsfe();
	    s_wsfe(&io___272);
	    e_wsfe();
	    s_wsfe(&io___273);
	    do_fio(&c__1, (char *)&blk20_1.narinv, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&trms, (ftnlen)sizeof(real));
	    do_fio(&c__1, (char *)&chi, (ftnlen)sizeof(real));
	    e_wsfe();
	    s_wsfe(&io___274);
	    e_wsfe();
	    if (isum > 1) {

		s_wsfe(&io___275);
		e_wsfe();
		if (isum == 3) {
		    s_wsfe(&io___276);
		    e_wsfe();
		}
		nused = 0;
		j = 0;
L951:
		++j;
		sum = (float)0.;
		sumx = (float)0.;
		nars = 0;
		i__1 = blk20_1.narinv;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    if ((i__2 = blk20_1.icalc[i__ - 1], abs(i__2)) == j) {
/* Computing 2nd power */
			r__1 = blk20_1.tobs[i__ - 1] - blk20_1.tcalc[i__ - 1];
			sum += r__1 * r__1;
/* Computing 2nd power */
			r__1 = (blk20_1.tobs[i__ - 1] - blk20_1.tcalc[i__ - 1]
				) / blk20_1.uobs[i__ - 1];
			sumx += r__1 * r__1;
			++nars;
			++nused;
		    }
/* L952: */
		}
		if (nars > 0) {
		    trms = sqrt(sum / (real) nars);
		}
		if (nars > 1) {
		    chi = sumx / (real) (nars - 1);
		}
		if (nars > 0) {
		    s_wsfe(&io___279);
		    do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&nars, (ftnlen)sizeof(integer));
		    do_fio(&c__1, (char *)&trms, (ftnlen)sizeof(real));
		    do_fio(&c__1, (char *)&chi, (ftnlen)sizeof(real));
		    e_wsfe();
		    if (isum == 3) {
			s_wsfe(&io___280);
			do_fio(&c__1, (char *)&j, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&nars, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&trms, (ftnlen)sizeof(real));
			do_fio(&c__1, (char *)&chi, (ftnlen)sizeof(real));
			e_wsfe();
		    }
		}
		if (nused >= blk20_1.narinv) {
		    goto L953;
		}
		goto L951;
L953:
		s_wsfe(&io___281);
		e_wsfe();
		if (isum == 3) {
		    s_wsfe(&io___282);
		    e_wsfe();
		}

		s_wsfe(&io___283);
		e_wsfe();
		if (isum == 3) {
		    s_wsfe(&io___284);
		    e_wsfe();
		}
		xsn = blk20_1.xscalc[0];
		icn = i_sign(&c__1, blk20_1.icalc);
		i__1 = nshot;
		for (j = 1; j <= i__1; ++j) {
		    iflagn = 1;
		    xsc = xsn;
		    icc = icn;
		    sum = (float)0.;
		    sumx = (float)0.;
		    nars = 0;
		    i__2 = blk20_1.narinv;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			if (blk20_1.xscalc[i__ - 1] == xsc && i_sign(&c__1, &
				blk20_1.icalc[i__ - 1]) == icc && 
				blk20_1.icalc[i__ - 1] != 0) {
/* Computing 2nd power */
			    r__1 = blk20_1.tobs[i__ - 1] - blk20_1.tcalc[i__ 
				    - 1];
			    sum += r__1 * r__1;
/* Computing 2nd power */
			    r__1 = (blk20_1.tobs[i__ - 1] - blk20_1.tcalc[i__ 
				    - 1]) / blk20_1.uobs[i__ - 1];
			    sumx += r__1 * r__1;
			    ++nars;
			    blk20_1.icalc[i__ - 1] = 0;
			} else {
			    if (iflagn == 1 && blk20_1.icalc[i__ - 1] != 0) {
				xsn = blk20_1.xscalc[i__ - 1];
				icn = i_sign(&c__1, &blk20_1.icalc[i__ - 1]);
				iflagn = 0;
			    }
			}
/* L852: */
		    }
		    if (nars > 0) {
			trms = sqrt(sum / (real) nars);
		    }
		    if (nars > 1) {
			chi = sumx / (real) (nars - 1);
		    }
		    if (nars > 0) {
			s_wsfe(&io___290);
			do_fio(&c__1, (char *)&xsc, (ftnlen)sizeof(real));
			do_fio(&c__1, (char *)&icc, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&nars, (ftnlen)sizeof(integer));
			do_fio(&c__1, (char *)&trms, (ftnlen)sizeof(real));
			do_fio(&c__1, (char *)&chi, (ftnlen)sizeof(real));
			e_wsfe();
			if (isum == 3) {
			    s_wsfe(&io___291);
			    do_fio(&c__1, (char *)&xsc, (ftnlen)sizeof(real));
			    do_fio(&c__1, (char *)&icc, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&nars, (ftnlen)sizeof(
				    integer));
			    do_fio(&c__1, (char *)&trms, (ftnlen)sizeof(real))
				    ;
			    do_fio(&c__1, (char *)&chi, (ftnlen)sizeof(real));
			    e_wsfe();
			}
		    }
/* L851: */
		}
		s_wsfe(&io___292);
		e_wsfe();
		if (isum == 3) {
		    s_wsfe(&io___293);
		    e_wsfe();
		}
	    }
	}
    }

    if (blk12_1.iplots == 1) {
	plotnd_();
    }

L9999:
    if (blk8_1.idump == 1) {

	o__1.oerr = 0;
	o__1.ounit = 22;
	o__1.ofnmlen = 5;
	o__1.ofnm = "n.out";
	o__1.orl = 0;
	o__1.osta = 0;
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);

	s_wsne(&io___294);
	s_wsne(&io___295);
	s_wsne(&io___296);
	s_wsne(&io___297);
    }

    s_stop("", 0L);

L999:
    s_wsfe(&io___298);
    e_wsfe();

    goto L9999;

} /* MAIN__ */

/* Main program alias */ int main_ () { MAIN__ (); }
