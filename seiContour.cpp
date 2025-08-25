/*********************** self documentation **********************/
/**************************************************************************
PSCONTOUR - draw contour of a two-dimensional array via PostScript

psContour	draw contour of a two-dimensional array via PostScript

***************************************************************************
Function Prototype:
void psContour (float c, int nx, float x[], int ny, float y[], float z[],
	float lcs, char *lcf, char *lcc, float *w);

***************************************************************************
Input:
c			contour value
nx			number of x-coordinates
x			array of x-coordinates (see notes below)
ny			number of y-coordinates
y			array of y-coordinates (see notes below)
lcs			font size of contour label
lcf			font name of contour label
lcc			color of contour label
LSB flag arrays (see Notes):
z			array of nx*ny z(x,y) values (see notes below)
w			array of nx*ny z(x,y) values (see notes below)

***************************************************************************
Notes:
The two-dimensional array z is actually passed as a one-dimensional
array containing nx*ny values, stored with nx fast and ny slow.

The x and y arrays define a grid that is not necessarily 
uniformly-sampled.  Linear interpolation of z values on the 
grid defined by the x and y arrays is used to determine z values 
between the gridpoints.
		 
The two least significant bits of z are used to mark intersections
of the contour with the x,y grid; therefore, the z values will
almost always be altered (slightly) by contour.

pscontour isolates the use of PostScript to four internal functions:

void coninit(void) - called before any contour drawing
void conmove(float x, float y) - moves current position to x,y
void condraw(float x, float y) - draws from current position to x,y
void condone(void) - called when contour drawing is done

These functions can usually be replaced with equivalent functions in
other graphics environments.

The w array is used to restrict the range of contour labeling that 
occurs only if w<1. 

As suggested in the reference, the following scheme is used to refer
to a cell of the two-dimensional array z:

                north (0)
      (ix,iy+1)	--------- (ix+1,iy+1)
                | cell  |
       west (3)	| ix,iy	| east (1)
                |       |
        (ix,iy) --------- (ix+1,iy)
                south (2)

***************************************************************************
Reference:
Cottafava, G. and Le Moli, G., 1969, Automatic contour map:
Commuincations of the ACM, v. 12, n. 7, July, 1969.

***************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/28/89
contour labeling added by: Zhenyue Liu, August 1993
***************************************************************************/
/**************** end self doc ********************************/
#include "stdafx.h"
#include "seiContour.h"
// #include "stdio.h"
#include "ealloc_d.h"

#define BTWN(a,b,c) (MIN((b),(c))<=(a) && (a)<MAX((b),(c)))
#define DELTA(a,b,c) ((b)!=(c)?((a)-(b))/((c)-(b)):1.0)

/* warning:  these bit masks are non-portable! */
#define SOUTH 01
#define WEST 02

#ifndef MAX
#define	MAX(x,y) ((x) > (y) ? (x) : (y))
#endif
#ifndef MIN
#define	MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#define NINT(x) ((int)((x)>0.0?(x)+0.5:(x)-0.5))

// #define PATHLIMIT 1500
#define PATHMAX 5000
#define SETS(z) (*((int *)(&(z))) |= SOUTH)
#define CLRS(z) (*((int *)(&(z))) &= ~SOUTH)
#define SSET(z) (*((int *)(&(z))) & SOUTH)
#define SETW(z) (*((int *)(&(z))) |= WEST)
#define CLRW(z) (*((int *)(&(z))) &= ~WEST)
#define WSET(z) (*((int *)(&(z))) & WEST)

/* forward declarations of functions defined internally */
CSeiContour::CSeiContour(CCoords::SEI_COORD *pCoordThis)
{
	pltContour = NULL;
	m_isContourColorFill = true;
	this->pCoord = pCoordThis;
}

CSeiContour::~CSeiContour()
{
	if (pltContour != NULL) free1CPoint(pltContour);
	pltContour = NULL;
}

BOOL CSeiContour::contourFillArr(CDC* pDC, int nx, int nt, float **xArr, float *xx,
	double xmin, double xmax, double tmin, double tmax,
	CColorHelper::AMPCOLORDYN *ampColorDyn2, double amax)
{
	int j;
	if (nt < 2) return FALSE;
	CBrush brush;
	CBrush *oldBrush;
	CPen aPen;
	CPen *pOldPen;
	//float *xx = (float *)alloc1float(nx);
	float *yy = (float *)alloc1float(nt);
	float *zz = (float *)alloc1float(nx*nt);
	//float tShift0 = getTraceDelay(&sw.headRead.trArr[0]);  // need to check if other traces have different delay
	double dt = (tmax - tmin) / (double)(nt - 1);
	for (j = 0; j < nt; j++) {
		yy[j] = (float)(tmin + j * dt);
	}
	memcpy((void *)&zz[0], (void *)&xArr[0][0], sizeof(float)*nt*nx);
	float c; // for contouring
	float da = (float)(amax*2.0 / (float)ampColorDyn2->num);
	double amin = 0.0;
	for (c = (float)amin; c <= (float)amax; c += da) {
		//		register COLORREF ac = CColorHelper::getColorRef4Amp(c, ampColorDyn2->num, percentAmpExpanded, colArr);
		register COLORREF ac = ampColorDyn2->getColorRef4Amp(c);
		brush.CreateSolidBrush(ac);
		aPen.CreatePen(PS_SOLID, 1, ac);
		oldBrush = pDC->SelectObject(&brush);
		pOldPen = pDC->SelectObject(&aPen);
		psContour(pDC, c, nx, &xx[0], nt, &yy[0], &zz[0], FALSE);
		pDC->SelectObject(oldBrush);
		pDC->SelectObject(pOldPen);
		aPen.DeleteObject();
		brush.DeleteObject();
	} // for
	free1float(yy);
	free1float(zz);
	return TRUE;
}

/* function */
void CSeiContour::psContour(CDC* pDC, float c, int nx, float x[], int ny, float y[], float z[], bool isContourColorFill)
/**************************************************************************
psContour - draw contour of a two-dimensional array via PostScript
***************************************************************************
Input:
c			contour value
nx			number of x-coordinates
x			array of x-coordinates (see notes below)
ny			number of y-coordinates
y			array of y-coordinates (see notes below)
lcs			font size of contour label
lcf			font name of contour label
lcc			color of contour label
Least Significat Bits:
z			array of nx*ny z(x,y) values (see notes below)
w			array of nx*ny z(x,y) values (see notes below)
***************************************************************************
Notes:
The two-dimensional array z is actually passed as a one-dimensional
array containing nx*ny values, stored with nx fast and ny slow.

The x and y arrays define a grid that is not necessarily 
uniformly-sampled.  Linear interpolation of z values on the 
grid defined by the x and y arrays is used to determine z values 
between the gridpoints.
		 
The two least significant bits of z are used to mark intersections
of the contour with the x,y grid; therefore, the z values will
almost always be altered (slightly) by contour.

pscontour isolates the use of PostScript to four internal functions:

void coninit(void) - called before any contour drawing
void conmove(float x, float y) - moves current position to x,y
void condraw(float x, float y) - draws from current position to x,y
void condone(void) - called when contour drawing is done

These functions can usually be replaced with equivalent functions in
other graphics environments.

The w array is used to restrict the range of contour labeling that 
occurs only if w<1. 

As suggested in the reference, the following scheme is used to refer
to a cell of the two-dimensional array z:

                north (0)
      (ix,iy+1)	--------- (ix+1,iy+1)
                | cell  |
       west (3)	| ix,iy	| east (1)
                |       |
        (ix,iy) --------- (ix+1,iy)
                south (2)

***************************************************************************
Reference:
Cottafava, G. and Le Moli, G., 1969, Automatic contour map:
Commuincations of the ACM, v. 12, n. 7, July, 1969.

***************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/28/89
contour labeling added by: Zhenyue Liu, June 1993
***************************************************************************/
{
	m_isContourColorFill = isContourColorFill;

	int ix,iy,non,cell,startcell;
	float d;
	float xmin = MIN(x[0],x[nx-1]), xmax = MAX(x[0],x[nx-1]);
	float ymin = MIN(y[0],y[ny-1]), ymax = MAX(y[0],y[ny-1]);
//	float xc=0.0, yc=0.0;	/* contour labeling centered at (xc,yc)	*/
 //	float xw, yw;	/* width and length of contour labeling */
	float xd, yd;	/* point on contour			*/
	float xdmin, xdmax, ydmin, ydmax; /* range of contour	*/
	int id;	/* =0 if a point on contour has been used as (xc,yc) */
	int cells=0;
//	char str[20];

	/* convert a number into a string */
	// sprintf(str,"%.*g",nplaces,c);

	/* determine length and width for printing the string */
 //	yw = (float)(lcs*0.55*((unsigned int) _tcslen(str)));
//	xw = (float)(lcs*0.8);
	

	/* restrict contour labeling from edges */
	for (iy=0; iy<ny-1; iy++)  
		//for (ix=0,cell=iy*nx; ix<nx-1; ix++,cell++) {
		//	if(x[ix]<xmin+2.0*xw || x[ix]>xmax-2.0*xw 
 	//			|| y[iy]<ymin+yw || y[iy]>ymax-yw)
  //				w[cell] += 1.;
 	//	}
 
	/* count intersections with cell boundaries */
	non = 0;

	/* find all the intersections */
	for (iy=0; iy<ny; iy++) {
		for (ix=0,cell=iy*nx; ix<nx; ix++,cell++) {

			/* check for intersection with west edge of cell */
			if (iy<ny-1 && BTWN(c,z[cell],z[cell+nx])) {
				SETW(z[cell]);  non++;
			} else {
				CLRW(z[cell]);
			}

			/* check for intersection with south edge of cell */
			if (ix<nx-1 && BTWN(c,z[cell],z[cell+1])) {
				SETS(z[cell]);  non++;
			} else {
				CLRS(z[cell]);
			}
		}
	}

	/* initialize contour drawing */
	coninit(pDC);

	/* follow contours intersecting north boundary */
	for (ix=0,startcell=(ny-1)*nx; non>0&&ix<nx-1; ix++,startcell++) {
		if (SSET(z[startcell])) {
			d = (float)(DELTA(c,z[startcell],z[startcell+1]));
			conmove(pDC, (1.0f-d)*x[ix]+d*x[ix+1],y[ny-1]);
			CLRS(z[startcell]);  non--;
			cell = startcell-nx;
			id = 1; 
			xdmin = xmax;
			xdmax = xmin;
			ydmin = ymax;
			ydmax = ymin;
			while (connect(pDC, c,nx,x,ny,y,z,&cell,&xd,&yd)){
				 non--;
				//if(w[cell]<0.5 && id) {
				//	xc = xd; yc = yd;
				//	cells = cell;
				//	id = 0;
				//}
				xdmin = MIN(xdmin,xd);
				xdmax = MAX(xdmax,xd);
				ydmin = MIN(ydmin,yd);
				ydmax = MAX(ydmax,yd);
			}
			//if(lcs>1 && id==0 && xdmax+ydmax-xdmin-ydmin>xw+yw) {
			//	wcell(nx,x,ny,y,w,cells,xc,yc,xw,yw);
 		//	//	labelc(xc-xw/2,yc-yw/2,xw,yw,str,lcs,lcf,lcc);
			//}
		}
	}

	/* follow contours intersecting east boundary */
	for (iy=0,startcell=nx-1; non>0&&iy<ny-1; iy++,startcell+=nx) {
		if (WSET(z[startcell])) {
			d = (float)DELTA(c,z[startcell],z[startcell+nx]);
			conmove(pDC, x[nx-1],(1.0f-d)*y[iy]+d*y[iy+1]);
			CLRW(z[startcell]);  non--;
			cell = startcell-1;
			id = 1; 
			xdmin = xmax;
			xdmax = xmin;
			ydmin = ymax;
			ydmax = ymin;
			while (connect(pDC, c,nx,x,ny,y,z,&cell,&xd,&yd)){
				 non--;
				//if(w[cell]<0.5 && id) {
				//	xc = xd; yc = yd;
				//	cells = cell;
				//	id = 0;
				//}
				xdmin = MIN(xdmin,xd);
				xdmax = MAX(xdmax,xd);
				ydmin = MIN(ydmin,yd);
				ydmax = MAX(ydmax,yd);
			}
			//if(lcs>1 && id==0 && xdmax+ydmax-xdmin-ydmin>xw+yw) {
			//	wcell(nx,x,ny,y,w,cells,xc,yc,xw,yw);
 		//	//	labelc(xc-xw/2,yc-yw/2,xw,yw,str,lcs,lcf,lcc);
			//}
		}
	}

	/* follow contours intersecting south boundary */
	for (ix=0,startcell=0; non>0&&ix<nx-1; ix++,startcell++) {
		if (SSET(z[startcell])) {
			d = (float)(DELTA(c,z[startcell],z[startcell+1]));
			conmove(pDC, (1.0f-d)*x[ix]+d*x[ix+1],y[0]);
			CLRS(z[startcell]);  non--;
			cell = startcell;
			id = 1; 
			xdmin = xmax;
			xdmax = xmin;
			ydmin = ymax;
			ydmax = ymin;
			while (connect(pDC, c,nx,x,ny,y,z,&cell,&xd,&yd)){
				 non--;
				//if(w[cell]<0.5 && id) {
				//	xc = xd; yc = yd;
				//	cells = cell;
				//	id = 0;
				//}
				xdmin = MIN(xdmin,xd);
				xdmax = MAX(xdmax,xd);
				ydmin = MIN(ydmin,yd);
				ydmax = MAX(ydmax,yd);
			}
			//if(lcs>1 && id==0 && xdmax+ydmax-xdmin-ydmin>xw+yw) {
			//	wcell(nx,x,ny,y,w,cells,xc,yc,xw,yw);
 		//	//	labelc(xc-xw/2,yc-yw/2,xw,yw,str,lcs,lcf,lcc);
			//}
		}		 
	}

	/* follow contours intersecting west boundary */
	for (iy=0,startcell=0; non>0&&iy<ny-1; iy++,startcell+=nx) {
		if (WSET(z[startcell])) {
			d = (float)(DELTA(c,z[startcell],z[startcell+nx]));
			conmove(pDC, x[0],(1.0f-d)*y[iy]+d*y[iy+1]);
			CLRW(z[startcell]);  non--;
			cell = startcell;
			id = 1; 
			xdmin = xmax;
			xdmax = xmin;
			ydmin = ymax;
			ydmax = ymin;
			while (connect(pDC, c,nx,x,ny,y,z,&cell,&xd,&yd)){
				 non--;
				//if(w[cell]<0.5 && id) {
				//	xc = xd; yc = yd;
				//	cells = cell;
				//	id = 0;
				//}
				xdmin = MIN(xdmin,xd);
				xdmax = MAX(xdmax,xd);
				ydmin = MIN(ydmin,yd);
				ydmax = MAX(ydmax,yd);
			}
			//if(lcs>1 && id==0 && xdmax+ydmax-xdmin-ydmin>xw+yw) {
			//	wcell(nx,x,ny,y,w,cells,xc,yc,xw,yw);
 		//	//	labelc(xc-xw/2,yc-yw/2,xw,yw,str,lcs,lcf,lcc);
			//}		
		}
	}

	/* follow interior contours */
	for (iy=1; iy<ny-1; iy++) {
		for (ix=0,startcell=iy*nx; non>0&&ix<nx-1; ix++,startcell++) {

			/* check south edge of cell */
			if (SSET(z[startcell])) {
				d = (float)(DELTA(c,z[startcell],z[startcell+1]));
				conmove(pDC, (1.0f-d)*x[ix]+d*x[ix+1],y[iy]);

				/* clear south edge where we started */
				CLRS(z[startcell]);  non--;

				cell = startcell;

				/* if another intersection exists in this cell */
				if (connect(pDC, c,nx,x,ny,y,z,&cell,&xd,&yd)) {

					/* set south edge so that we finish where we started */
					SETS(z[startcell]);  non++;
			
					/* follow the contour */
			id = 1; 
			xdmin = xmax;
			xdmax = xmin;
			ydmin = ymax;
			ydmax = ymin;
			while (connect(pDC, c,nx,x,ny,y,z,&cell,&xd,&yd)){
				 non--;
				//if(w[cell]<0.5 && id) {
				//	xc = xd; yc = yd;
				//	cells = cell;
				//	id = 0;
				//}
				xdmin = MIN(xdmin,xd);
				xdmax = MAX(xdmax,xd);
				ydmin = MIN(ydmin,yd);
				ydmax = MAX(ydmax,yd);
			}
			//if(lcs>1 && id==0 && xdmax+ydmax-xdmin-ydmin>xw+yw) {
			//	wcell(nx,x,ny,y,w,cells,xc,yc,xw,yw);
 		//	//	labelc(xc-xw/2,yc-yw/2,xw,yw,str,lcs,lcf,lcc);
			//}	
				} 		 
			}
		}
	}
	/* contour drawing is done */
	condone(pDC);
}

int CSeiContour::connect(CDC* pDC, float c, int nx, float *x, int ny, float *y, float *z,
	int *pcell, float *pxd, float *pyd)
/*
connect draws a line from one intersection of the cell(ix,iy) =  z[*pcell]
to another intersection of the cell, provided the latter intersection exists,
and then clears the latter intersection and updates the cell pointer
connect returns 0 if the latter intersection does not exist or if the 
latter intersection is a grid boundary; otherwise returns 1.
*/
{
	int cell= *pcell, ix=cell%nx, iy=cell/nx;
	float d;

	/* if exiting north */
	if (SSET(z[cell+nx])) {
		cell += nx;  iy++;
		d = (float)(DELTA(c,z[cell],z[cell+1]));
		*pxd = (1.0f-d)*x[ix]+d*x[ix+1];
		*pyd = y[iy];
		condraw(pDC, *pxd,*pyd);
		CLRS(z[cell]);
		*pcell = cell;
		if (iy<ny-1)
			return(1);
		else
			return(0);

	/* else if exiting east */
	} else if (WSET(z[cell+1])) {
		cell += 1;  ix++;
		d = (float)(DELTA(c,z[cell],z[cell+nx]));
		*pxd = x[ix];
		*pyd = (1.0f-d)*y[iy]+d*y[iy+1];
		condraw(pDC, *pxd,*pyd);
		CLRW(z[cell]);
		*pcell = cell;
		if (ix<nx-1)
			return(1);
		else
			return(0);

	/* else if exiting south */
	} else if (SSET(z[cell])) {
		d = (float)(DELTA(c,z[cell],z[cell+1]));
		*pxd = (1.0f-d)*x[ix]+d*x[ix+1];
		*pyd = y[iy];
		condraw(pDC, *pxd,*pyd);
		CLRS(z[cell]);
		*pcell = cell-nx;
		if (iy>0)
			return(1);
		else
			return(0);

	/* else if exiting west */
	} else if (WSET(z[cell])) {
		d = (float)(DELTA(c,z[cell],z[cell+nx]));
		*pxd = x[ix];
		*pyd = (1.0f-d)*y[iy]+d*y[iy+1];
		condraw(pDC, *pxd,*pyd);
		CLRW(z[cell]);
		*pcell = cell-1;
		if (ix>0)
			return(1);
		else
			return(0);

	/* else if no intersection exists */
	} else {
		return(0);
	}
}

//CPoint mm_coord(float x, float y, float xa, float xb, float xc, float ya, float yb, float yc)
//{
//	CPoint p;
//	p.x = int(xa + xb*(x - xc) + 0.5);
//	p.y = int(ya + yb*(y - yc) + 0.5);
//	return p;
//}

void CSeiContour::coninit(CDC* pDC)
{ // called before any contour drawing
	//gsave();
	//newpath();
	pathcount = 0;
	if (pltContour != NULL) free1CPoint(pltContour);
	pltContour = (CPoint *)alloc1CPoint(PATHMAX);

}

void CSeiContour::conpolyline_polygon(CDC* pDC)
{
	if (pathcount == 0) return;
	//pDC->PolyPolygon(
	if (m_isContourColorFill) {
		int oldFIllMode = pDC->SetPolyFillMode(WINDING);
		pDC->Polygon(pltContour, pathcount);
		pDC->SetPolyFillMode(oldFIllMode);
	}
	else {
		pDC->Polyline(pltContour, pathcount);
	}
	pathcount = 0;
}

void CSeiContour::conmove(CDC* pDC, float x, float y)
{ // moves current position to x,y
	//if (++pathcount>PATHMAX) {
	//	stroke();
	//}
	//moveto(x,y);

	conpolyline_polygon(pDC);
	
	pltContour[pathcount] = pCoord->mm_coord(x, y);
	pathcount++;
}

void CSeiContour::condraw(CDC* pDC, float x, float y)
{
	//lineto(x,y);
	//if (++pathcount>PATHMAX) {
	//	stroke();
	//	moveto(x,y);
	//	pathcount = 0;
	//}

	if (pathcount>=PATHMAX) {
		conpolyline_polygon(pDC);
	}
	else {
		pltContour[pathcount] = pCoord->mm_coord(x, y);
		pathcount ++;
	}
}

void CSeiContour::condone(CDC* pDC)
{
	//stroke();
	//grestore();
	if (pathcount>0) {
		conpolyline_polygon(pDC);
	}
	if (pltContour != NULL) {
		free1CPoint(pltContour);
		pltContour = NULL;
	}
}

/* 	draw a label on contour		*/
void CSeiContour::labelc(float x, float y, float xw, float yw, char *str,
	float size, char *font, char *color)
{
 //	stroke();
	//gsave();
	//newpath();
 //  	setcolor("white");
	//rectfill(x,y,xw,yw);
 //	setcolor(color);
	//setfont(font,size);
 //       moveto(x+xw,y);
 //	rotate(90.);
	//show(str);
	//rotate(-90.);
 //  	stroke();
	//grestore();
}

/*	update the w array	*/
void CSeiContour::wcell(int nx, float *x, int ny, float *y, float *w,
	int cell, float xc, float yc, float xw, float yw)
 
{
	int  ix=cell%nx, iy=cell/nx, ixl=ix, ixh=ix, iyl=iy, iyh=iy;
 
	while(x[ixl]>=xc-2.0*xw && x[ixl]<=xc+2.0*xw && ixl>0) ixl--;
	while(x[ixh]>=xc-2.0*xw && x[ixh]<=xc+2.0*xw && ixh<nx-1) ixh++;
	while(y[iyl]>=yc-1.5*yw && y[iyl]<=yc+1.5*yw && iyl>0) iyl--;
	while(y[iyh]>=yc-1.5*yw && y[iyh]<=yc+1.5*yw && iyh<ny-1) iyh++;

	for(iy=iyl; iy<=iyh; ++iy)
		for(ix=ixl,cell=iy*nx+ixl; ix<=ixh; ix++, cell++)
			w[cell] +=1.0; 

 }
