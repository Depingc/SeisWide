#include "stdafx.h"
#include "coords.h"
#include "util.h"
#include "math.h"
#include "ealloc_d.h"


CCoords::CCoords(void)
{
}

CCoords::~CCoords(void)
{
}

// get reduced time in sec based on offset information. NMO display is not 
double CCoords::getReduceTimeTraditional(double offset, int drawmode, double rvred)
{
	if (drawmode == 6) return 0.;  // this is trace display, without any reducing.
	if (drawmode == 10) return 0.;

	// double rvVred = !isFloatEqual(vred, 0.0)  ?   1./vred  : 0.0;
	//double rvVred = (vred > 0.1 && vred < 10.0)  ?   1./vred  : 0.0;
	return (drawmode != 0) ? 0.0 : rvred * fabs(offset);	   //Apply reduction velocity.
}

//double CCoords::getReduceHyperbola(double offset, CCoords::REDUCE *red) {
//	if (isFloatEqual3(red->targetDepth, 0.))
//		return 0.; // no need to do anything when target depth is water depth
//	if (isFloatEqual3((float)red->rvred, 0.f)) return 0.;
//
//	double xh = offset * 0.5;
//	double out = sqrt(xh*xh + red->targetDepth * red->targetDepth) - red->targetDepth;
//	return out * 2. * red->rvred;
//}

double CCoords::getReduceTimeEx2(double offset, int drawmode, CCoords::REDUCE *red)
{
	if (offset < 0.) offset = -offset;
	if (drawmode != 7) return getReduceTimeTraditional(offset, drawmode, red->rvred);

	//double vW = red->waterVelocityAboveObs;
	//double Zn = red->targetDepth;
	//double Zr = red->obsDepth;
	//double Zs = red->gunDepth;


	if (red->targetDepth<-10.f || red->targetDepth>60.f) {
		return 0.f;
	}
	if (red->waterVelocityAboveObs < 0.01 || red->waterVelocityAboveObs>2.0) 
		red->waterVelocityAboveObs = 1.441f;
	double rvVwater = 1. / red->waterVelocityAboveObs;

	double reducedTime = 0.0;
	double dS = red->targetDepth - red->gunDepth;
	double dR = red->targetDepth - red->obsDepth;  // dR can be zero for OBS
	double alpha = dR / dS;
	double X2 = offset / (1. + alpha);
	double X1 = X2 * alpha;

	reducedTime = sqrt(X1*X1 + dR*dR) + sqrt(X2*X2 + dS*dS) - (dR + dS);
	reducedTime *= red->rvred;
	reducedTime -= (red->obsDepth + red->gunDepth) * rvVwater;

	return reducedTime;
}

// for backward compatibility only
double CCoords::getReduceTimeExOld2(double offset, int drawmode, CCoords::REDUCE *red)
{
	//	if (red->rvred == 0. && red->vred > MINAMP) red->rvred = 1. / red->vred;

	if (offset < 0.) offset = -offset;
	if (drawmode != 7) return getReduceTimeTraditional(offset, drawmode, red->rvred);

	double vW = red->waterVelocityAboveObs;
	double Zw = red->targetDepth;
	double Zr = red->obsDepth;
	double Zs = red->gunDepth;

	double dR = Zw - Zr;
	if (Zw<-10.f || Zw>15.f) {
		return 0.f;
	}

	// double rvVred = !isFloatEqual(vred, 0.0)  ?   1./vred  : 0.0;
	if (vW < 0.01 || vW>2.0) vW = 1.441f;
	double rvVwater = 1. / vW;

	double reducedTime = 0.0;

	// for one side
	double X2 = offset * offset * 0.25; // half offset squared

	// for the other side
	double dS = Zw - Zs;
	double Y = 0.5 * offset * dS / dR;
	// half offset squared for the other half

	reducedTime = sqrt(X2 + dR*dR) + sqrt(Y*Y + dS*dS) - Zw*2. + Zr + Zs;
	reducedTime *= red->rvred;
//	reducedTime -= (Zr + 2.*Zs) * rvVwater;  // later I found 2.*  is wrong
	reducedTime -= (Zr + Zs) * rvVwater;

	return reducedTime;
}


BOOL CCoords::isXShotDefined(double xshot) {
	BOOL a = !_isnan(xshot);
	BOOL c = _finite(xshot);
	BOOL b = !isFloatAbsEqual((float)xshot, ERRORVAL);
	return  a && b && c;
}



double CCoords::getOffsetFromXVal(double x, double xShot, BOOL isDistanceDisplay, BOOL isMinusXShot) {
	if (!isXShotDefined(xShot))
		return x;  // we give up after one try

	if (isDistanceDisplay) {
		// now x is in distance mode, how do we get offset?
		if (isMinusXShot)
			xShot *= -1.;

		x -= xShot;
	}


	return x;
}
//
//int CCoords::mm_coordx(double x)
//{
//// A routine to convert my world coordinates (in km, sec) into logical coordinates of windows. 
//	// testing the swap
//	double a;
//	if (coord.isSwapX)
//		a = (coord.X2() - x)*coord.mm_xscale;
//	else
//		a = (x - coord.X1())*coord.mm_xscale;
//	return (int)myRound(coord.logicalAxes.left + a);
//}

//
//int CCoords::mm_coordy(double y, bool isTime)
//{
//	if (isTime)
//		return mm_coordt(y);
//	else
//		return mm_coordz(y);
//}

//
//int CCoords::mm_coordt(double t)
//{
//// A routine to convert my Y-axis world coordinates (in sec) into logical coordinates of windows. 
//	double a = (!coord.isFaceUp) ?  (t-coord.tmin)  :  (coord.tmax-t);
//	return (int)myRound(a*coord.mm_yscale+coord.logicalAxes.top);
//}

//int CCoords::mm_coordz(double z)
//{
//// A routine to convert my Y-axis world coordinates (in km) into logical coordinates of windows. 
//	double a = (!coord.isFaceUp) ?  (z-coord.zmin)  :  (coord.zmax-z);
//	return (int)myRound(a*coord.mm_yscale+coord.logicalAxes.top);
//}



//
//CPoint CCoords::mm_coord(double x, double y, bool isTime)
//{
//	if (isTime)
//		return mm_coordt(x, y);
//	else
//		return mm_coordz(x, y);
//}
//
//CPoint CCoords::mm_coordt(double x, double y)
//{
//// A routine to convert my world coordinates (in km, sec) into logical coordinates of windows. 
//// for facedown plot only: xmin->0, xmax->xLen;  ymin->0, ymax->yLen.
//	CPoint p;
//	double a;
//	
//	// testing the swap
//	if (coord.isSwapX)
//		a = (coord.X2() - x)*coord.mm_xscale;
//	else
//		a = (x - coord.X1())*coord.mm_xscale;
//
//
//	p.x = (LONG)myRound(coord.logicalAxes.left + a);
//
//	a = coord.isFaceUp ?
//		(coord.Y2() - y)*coord.mm_yscale
//		:
//		(y - coord.Y1())*coord.mm_yscale;
//
//	p.y = (LONG)myRound(a+coord.logicalAxes.top);
//
//	return p;
//
//}


//CPoint CCoords::mm_coordz(double x, double y)
//{
//// A routine to convert my world coordinates (in km, sec) into logical coordinates of windows. 
//// for facedown plot only: xmin->0, xmax->xLen;  ymin->0, ymax->yLen.
//	CPoint p;
//	double a;
//	// testing the swap
//	if (coord.isSwapX)
//		a = (coord.X2() - x)*coord.mm_xscale;
//	else
//		a = (x - coord.X1())*coord.mm_xscale;
//
//	p.x = (LONG)myRound(coord.logicalAxes.left + a);
//
//	a = coord.isFaceUp ?
//		(coord.Y2() - y)*coord.mm_yscale
//		:
//		(y - coord.Y1())*coord.mm_yscale;
//	p.y = (LONG)myRound(a+coord.logicalAxes.top);
//
//	return p;
//
//}




/*
// Normally this function returns 1 for screen displays. But for a device that is of 10 times higher resolution, this function will return 10.
Example 1 for calling font size:
		int numXLogical = (int)myRound(pFrame->fLabelDistance3 * cd.coord.device2Logical * CCoords::getFontAmp(pDC) * 6.);
		int numYLogical = (int)(numXLogical / xyRatio);
		logFontNum.lfWidth = numXLogical;
		logFontNum.lfHeight = numYLogical;
			// this will produce a text font size of 6 points
			// the reason for using it: a 6 point font in a 1200DPI PDF file is too small to see. So need to change it

Example 2:
		coord->logicalAxes.left = (LONG)(coord->device2Logical * 40. * CCoords::getFontAmp(pDC));	//minimum x,y for data plotting, in logical coordinates. // 3211
		coord->logicalAxes.top = (LONG)(coord->device2Logical * 35. * CCoords::getFontAmp(pDC));
		int rightBorderWidth = (int)(coord->device2Logical * 70. * CCoords::getFontAmp(pDC));
		int botBorderWidth = (int)(coord->device2Logical * 70. * CCoords::getFontAmp(pDC));
*/
//double CCoords::getDeviceResAdjust(CDC *pDC) 
//{
//	double pixelPerInch = pDC->GetDeviceCaps(LOGPIXELSX);
//	if (pixelPerInch < 1.) pixelPerInch = 96.;
//	return pixelPerInch * 0.01;
//}
//
//
//int CCoords::getDeviceResAdjust(CDC *pDC, int x)
//{
//	double a = CCoords::getDeviceResAdjust(pDC);
//	return (int)myRound(a * x);
//}

/*
	// Given 
	pDC , and coord->xmin, xmax, zmin, zmax (or tmin, tmax if time domain) (input only; this function will not modify them. Please do it yourself)
	// This function calculate and assign
	1. coord->logicalAxes and logicalTotal (will adjust slightly)
	3. coord->mm_xscale, coord.mm_yscale
	4. coord->xValuePerPixel
	No global variables will be accessed exception a function mm_coord() from pDoc->pCd instance of a class
*/
void CCoords::setCoordMappingFromWorld(CDC* pDC, int lableOutsideStat,
	bool isYLabelOnRight,
	double shrinkX, double shrinkY, CCoords::SEI_COORD *coord)
{
	CRect viewPixelsTotal;
	if(pDC->IsPrinting()) {
		//first query for printer paper size.
		viewPixelsTotal.left = viewPixelsTotal.top = 0;
		viewPixelsTotal.right = pDC->GetDeviceCaps(HORZRES);   // this func not available when exporting to bitmap as well
		viewPixelsTotal.bottom = pDC->GetDeviceCaps(VERTRES);
	} else {
		// GetClientRect() function only gives the screen size, even during printing!!!
		GetClientRect(WindowFromDC(pDC->m_hDC), &viewPixelsTotal);	 
		//note for rectDevicePixels, the bottom is positive: physical coords of screen.
	}

	// fix some occasional exceptions
	if (viewPixelsTotal.right < viewPixelsTotal.left + 30)
		viewPixelsTotal.right = viewPixelsTotal.left + 30;
	if (viewPixelsTotal.bottom < viewPixelsTotal.top + 20)
		viewPixelsTotal.bottom = viewPixelsTotal.top + 20;

	CSize pixelTotal(viewPixelsTotal.right, viewPixelsTotal.bottom);
	CSize logicalTotal((LONG)(pixelTotal.cx *coord->device2Logical), (LONG)(pixelTotal.cy *coord->device2Logical));

	setCoordMappingFromWorld(pDC, lableOutsideStat,
		isYLabelOnRight,
		pixelTotal, logicalTotal,
		shrinkX, shrinkY, coord);


}

void CCoords::setCoordMappingFromWorld(CDC* pDC, int lableOutsideStat,
	bool isYLabelOnRight,
	CSize pixelTotal,
	CSize logicalTotal,
	double shrinkX, double shrinkY,
	CCoords::SEI_COORD *coord)
{
	//	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetMapMode(MM_ISOTROPIC);

	if (shrinkX > 1.) pixelTotal.cx = (LONG)myRound(pixelTotal.cx / shrinkX);
	if (shrinkY > 1.) pixelTotal.cy = (LONG)myRound(pixelTotal.cy / shrinkY);
	if (pixelTotal.cy < 10) pixelTotal.cy = 10;  // this avoid crash
	if (pixelTotal.cx < 10) pixelTotal.cx = 10;
	pDC->SetViewportExt(pixelTotal.cx, pixelTotal.cy); // 1623x656

	coord->viewLogicalTotal = CRect(0, 0, logicalTotal.cx, logicalTotal.cy);
	pDC->SetWindowExt(logicalTotal.cx, logicalTotal.cy);	 // 44600x18000

	int mulOnePixel = (LONG)(coord->device2Logical * coord->getDefDisplayAdjust(pDC));
	CRect aLogicalAxes;
	if (lableOutsideStat>0) {
		aLogicalAxes.left = mulOnePixel;	//minimum x,y for data plotting, in logical coordinates. // 3211
		aLogicalAxes.top = mulOnePixel;	//these lines are put into the main routine.			// 1296
		aLogicalAxes.right = coord->viewLogicalTotal.right - mulOnePixel;
		aLogicalAxes.bottom = coord->viewLogicalTotal.bottom - mulOnePixel;
	}
	else {
		aLogicalAxes.top = (lableOutsideStat < 0) ? 12 : 25;
		aLogicalAxes.top *= mulOnePixel;

		int botBorderWidth = (lableOutsideStat < 0) ? 40 : 60;
		botBorderWidth *= mulOnePixel;

		int rightBorderWidth;
		if (!isYLabelOnRight) {
			aLogicalAxes.left = 40;
			rightBorderWidth = 2;
		}
		else {
			aLogicalAxes.left = 2;
			rightBorderWidth = 40;
		}
		aLogicalAxes.left *= mulOnePixel;
		rightBorderWidth *= mulOnePixel;

		aLogicalAxes.right = coord->viewLogicalTotal.right - rightBorderWidth;
		aLogicalAxes.bottom = coord->viewLogicalTotal.bottom - botBorderWidth;
	}

	coord->logicalAxes = CRect(aLogicalAxes);
	coord->mm_xscale = (double)coord->logicalAxes.Width() / (double)(coord->X2() - coord->X1());
	coord->mm_yscale = (double)coord->logicalAxes.Height() / (double)(coord->Y2() - coord->Y1());
	if (coord->mm_xscale < MINAMP) coord->mm_xscale = MINAMP;
	if (coord->mm_yscale < MINAMP) coord->mm_yscale = MINAMP;
}




// pixelTotal and  logicalTotal  always assume to be bottom as positive and only positive is considered, maybe I should have used CSize for these ?
// pixelTotal is NOT stored in *coord  but logicalTotal is assigned to *coord for convenient manipulation
// if lableOutsideStat==0, there is formal labels.
//    lableOutsideStat>0,  no labels at all
//    lableOutsideStat<0, very small labels at the top
void CCoords::setCoordMappingFromWorldOld(CDC* pDC, int lableOutsideStat,
		bool isYLabelOnRight,
		CSize pixelTotal,
		CSize logicalTotal,
		double shrinkX, double shrinkY,
		CCoords::SEI_COORD *coord)
{
	// GetDeviceCaps(hdc,*) returns the screen dimensions regardless of the window size. 
//	int logical2Device = 35;  // meaning: 1 pixel = 35 units logical
	// note: in theory, the logical2Device = 35   can be changed to other numbers as well. But many label size calculations are based on coords system with logical2Device = 35
	//       we cannot change it yet until those are resolved.

//	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetMapMode(MM_ISOTROPIC);
	
	
	if (shrinkX > 1.) pixelTotal.cx = (LONG)myRound(pixelTotal.cx/ shrinkX);
	if (shrinkY > 1.) pixelTotal.cy = (LONG)myRound(pixelTotal.cy/ shrinkY);
	if (pixelTotal.cy < 10) pixelTotal.cy = 10;  // this avoid crash
	if (pixelTotal.cx < 10) pixelTotal.cx = 10;
	pDC->SetViewportExt(pixelTotal.cx, pixelTotal.cy); // 1623x656

	coord->viewLogicalTotal = CRect(0, 0, logicalTotal.cx, logicalTotal.cy);
	pDC->SetWindowExt(logicalTotal.cx, logicalTotal.cy);	 // 44600x18000

	int mulOnePixel = (LONG)(coord->device2Logical * coord->getDefDisplayAdjust(pDC));
	if (lableOutsideStat>0) {
		coord->logicalAxes.left = 0;	//minimum x,y for data plotting, in logical coordinates. // 3211
		coord->logicalAxes.top = 0;	//these lines are put into the main routine.			// 1296
		coord->mm_xscale = (coord->viewLogicalTotal.right - coord->device2Logical) / (double)(coord->X2() - coord->X1());
		coord->mm_yscale = (coord->viewLogicalTotal.bottom - coord->device2Logical) / (double)(coord->Y2() - coord->Y1());
		// why we have to deduct logical2Device*1 from the denominator? Because otherwise, the axis rectangle cannot be plotted since it is all outside the plotting limits
	}
	else {
		// using 42 pixel for right border can save precious work space on screen. Cannot use smaller since there are vertical schrol bar on the right side
		//if (lableOutsideStat<0) {
		//	coord->logicalAxes.top = 12 * mulOnePixel;
		//}
		//else {
		//	coord->logicalAxes.top = 25 * mulOnePixel;
		//}
		coord->logicalAxes.top = (lableOutsideStat < 0) ? 12 : 25;
		coord->logicalAxes.top *= mulOnePixel;

		int botBorderWidth = (lableOutsideStat < 0) ? 40 : 60;
		botBorderWidth *= mulOnePixel;

		int rightBorderWidth = (coord->isSwapX) ? 42 : 42;
		if (isYLabelOnRight) rightBorderWidth = 32;
		rightBorderWidth *= mulOnePixel;

		coord->mm_xscale = (coord->viewLogicalTotal.right - rightBorderWidth) / (double)(coord->X2() - coord->X1());
		coord->mm_yscale = (coord->viewLogicalTotal.bottom - botBorderWidth) / (double)(coord->Y2() - coord->Y1());  // note: here 2312 is reserved for the top and bottom labeling space 
		coord->logicalAxes.left = mulOnePixel;
		if (!isYLabelOnRight)
			coord->logicalAxes.left *= 40;
	}
	if (coord->mm_xscale < MINAMP) coord->mm_xscale = MINAMP;

	// note: mm_coord() will use values like  coord.logicalAxes.top and  coord.logicalAxes.left, and coord.mm_xscale and coord.mm_yscale. So a redundency is calculation 
	//       can be avoided but you got to be careful. But I use the safest way here
	// bool isTime = isTimeDomain == TRUE;
	CPoint p = this->coord.mm_coord(coord->X1(), coord->Y1());
	CPoint p2 = this->coord.mm_coord(coord->X2(), coord->Y2());
	long i;
	if (p.x > p2.x) {
		// swap them
		i = p.x;
		p.x = p2.x;
		p2.x = i;
	}
	if (p.y > p2.y) {
		i = p.y;
		p.y = p2.y;
		p2.y = i;
	}
	// p is now the logical coordinates for the client area to plot

	int iy = min(p.y, p2.y);
	p2.y = max(p.y, p2.y);
	p.y = iy;
	coord->logicalAxes = CRect(p.x, p.y, p2.x, p2.y);
	//pDC->LPtoDP(&p,1);
	//pDC->LPtoDP(&p1,1);
	//coord->dxPerPixel = (double)(coord->X2() - coord->X1()) /
	//	(double)coord->logicalAxes.Width() * coord->device2Logical;
	//coord->dyPerPixel = (double)fabs(coord->Y2() - coord->Y1()) / 
	//	(double)coord->logicalAxes.Height() * coord->device2Logical;
}


//void CCoords::setCoordMappingFromWorldFinal(CDC* pDC,
//	int lableOutsideStat,
//	bool isYLabelOnRight,
//	CSize pixelTotal,
//	CSize logicalTotal,
//	CRect aLogicalAxes,
//	double shrinkX,
//	double shrinkY,
//	CCoords::SEI_COORD *coord) {
//
//	coord->logicalAxes = CRect(aLogicalAxes);
//	coord->mm_xscale = (double)coord->logicalAxes.Width() / (double)(coord->X2() - coord->X1());
//	coord->mm_yscale = (double)coord->logicalAxes.Height() / (double)(coord->Y2() - coord->Y1());
//	if (coord->mm_xscale < MINAMP) coord->mm_xscale = MINAMP;
//	if (coord->mm_yscale < MINAMP) coord->mm_yscale = MINAMP;
//}


//yminExact, ymaxExact are the exact values of min and max box to plot, although they may not be the "nice" numbers to show. 
//   Note:  1. ymin and ymax already considered the time or depth domain issues correctly.
//          2. I have not pre-calculated the xminNice, xmaxNice values; this is because we may have issues of trace or distance/offset which may need separate handling. 
//             If you really want to do this, you need to test before commiting.
//          3. This routine is only called by ticks()  and is therefore put to the protected section in header file
//          4. ideltx, idelty are the length of ticks in logical coordinates. Labels should not overlay with them
void CCoords::DrawAxesTicksValuesDepre(CDC* pDC, CCoords::SEI_COORD *pCoord, int fontWidth, int fontHeight, BOOL isMainXSwap, BOOL isNoHorzGrid)
{
	CPoint p, tick[2];
	CString tStr;
	//int ideltx = (int)(-130 * mul); 
	//int idelty = (int)(120 * mul);

	// xmin can be 1, which should be rounded to 0, and incremented by a regular tick value
	// xmax cannot be rounded, otherwise 899 becomes 1000, which is not good
	double xminNice = getMinRounded(pCoord->X1(), pCoord->X2(), 10);
	double xmaxNice = (float)niceNumMinMax(pCoord->X2(), true);
	double xTickNice = getTickRounded(pCoord->X1(), pCoord->X2(), 2, 10, pCoord->xTick);
	if (xTickNice < (xmaxNice - xminNice)*0.05) xTickNice = (xmaxNice - xminNice)*0.05;
	else if (xTickNice >(xmaxNice - xminNice)*0.5) xTickNice = (xmaxNice - xminNice)*0.5;
	xTickNice = getTickRounded(pCoord->X1(), pCoord->X2(), 2, 10, xTickNice);

	if (xminNice < 0.) 
		xminNice = 0.;
	do {
		xminNice -= xTickNice;
	} while (xminNice > pCoord->X1());

	double yminNice = getMinRounded(pCoord->Y1(), pCoord->Y2(), 10);
	double yTickNice = getTickRounded(pCoord->Y1(), pCoord->Y2(), 1, 10, pCoord->getYTick());
	if (yminNice < 0.) yminNice = 0.;
	do {
		yminNice -= yTickNice;
	} while (yminNice > pCoord->Y1());

	// testing
	double mul = pCoord->getDefDisplayAdjust(pDC);
	int idelty = (int)(120 * mul);

	bool isTime = pCoord->isTimeDomain() == TRUE;
	double yPlot = pCoord->isFaceUp ? pCoord->Y1() : pCoord->Y2();
	double x = floor(xminNice);
	double xCurrent;
	int pixelX, pixelX2;
	do {
		// first, draw major ticks
		if (pCoord->isVisibleOrNot(x)) {
			tick[0] = pCoord->mm_coord(x, yPlot);	//delty is negative.
			tick[1].x = tick[0].x;
			tick[1].y = tick[0].y + (long)(1.5*idelty);

			tStr = getNiceLabelFromValue(x, xTickNice);

			int countS = tStr.GetLength();
			pDC->Polyline(tick, 2);

			// before drawing X-labels, we want to make sure they draw visibly.
			pixelX = tick[1].x - (countS*fontWidth) / 2;
			if (pixelX <  (int)ceil(mul)) pixelX = (int)ceil(mul);
			pixelX2 = pixelX + tStr.GetLength()*fontWidth + (int)ceil(mul);
			if (pixelX2 > pCoord->viewLogicalTotal.right)
				pixelX -= pixelX2 - pCoord->viewLogicalTotal.right 
					+ (int)ceil(3*mul*pCoord->device2Logical);  // 3 pixels off the right side

			// now the labels are all in good condition
			pDC->TextOut(pixelX, tick[1].y + (int)ceil(mul), tStr);
		}



		// now draw minor ticks
		double xMinorInc = getMinorIncNice(xTickNice);
		for (int i = 0; i < 9; i++) {
			xCurrent = x + xMinorInc*(i+1);
			if (xCurrent >= x + xTickNice) break; // it has reached the next major tick
			if (pCoord->isVisibleOrNot(xCurrent)) {
				tick[0] = pCoord->mm_coord(xCurrent, yPlot);
				tick[1].x = tick[0].x;
				tick[1].y = tick[0].y + idelty;
				pDC->Polyline(tick, 2);
			}
		}

		x += xTickNice;
	} while (x <= pCoord->X2() + xTickNice + 0.001);	  //Finish drawing xTicks





	CPen* pOldPen;
	CPen gridPen(PS_DOT, 0, RGB(200, 200, 200));




	double xLeftAxis;
	double xRightAxis;
	int ideltx = (int)(130 * mul);
	if (pCoord->isSwapYLabel) isMainXSwap = !isMainXSwap;
	if (isMainXSwap == pCoord->isSwapX) ideltx = -ideltx;
	if (isMainXSwap) {
		xLeftAxis = pCoord->X2();
		xRightAxis = pCoord->X1();
	}
	else {
		xLeftAxis = pCoord->X1();
		xRightAxis = pCoord->X2();
	}


	double y = yminNice;
	// why use -0.001 ? It is because if we do not, the tick for ymin may be missed since 
	//  pCoord->Y1() can be 3.000001
	if (y > pCoord->Y1()) y -= yTickNice;

	// now we need a fix: suppose ymin=3.7 and yTick=1, yTickNice becomes 3.5, 
	// we need to start from 3 instead of 3.5! 
	y = floor(y);


	do  {
		// first, draw major ticks
		if (y >= pCoord->Y1() - 0.001 &&  y <= pCoord->Y2() + 0.001) {
			tick[0] = pCoord->mm_coord(xLeftAxis, y);
			tick[1].x = tick[0].x + (long)(1.5*ideltx);
			tick[1].y = tick[0].y;

			tStr = getNiceLabelFromValue(y, yTickNice);
			pDC->MoveTo(tick[0]);
			pDC->LineTo(tick[1]);

			// before drawing X-labels, we want to make sure they draw visibly.
			if (isMainXSwap && !pCoord->isSwapX)
				pixelX = tick[1].x + tStr.GetLength()*fontWidth - (int)ceil(mul);
			else if (!isMainXSwap && pCoord->isSwapX)
				pixelX = tick[1].x + (int)ceil(mul);
			else
				pixelX = tick[1].x - tStr.GetLength()*fontWidth - (int)ceil(mul);

			if (pixelX <  (int)ceil(mul)) pixelX = (int)ceil(mul);
			pixelX2 = pixelX + tStr.GetLength()*abs(fontWidth) + (int)ceil(mul);
			if (pixelX2 > pCoord->viewLogicalTotal.right)
				pixelX -= pixelX2 - pCoord->viewLogicalTotal.right;

			// now the labels are all in good condition
			pDC->TextOut(pixelX, tick[1].y - fontHeight / 2, tStr);

			// draw grids
			if (!isNoHorzGrid && y > pCoord->Y1() && y < pCoord->Y2()) {
				tick[1].x = pCoord->mm_coordx(xRightAxis);
				pOldPen = pDC->SelectObject(&gridPen);
				pDC->MoveTo(tick[0]);
				pDC->LineTo(tick[1]);
				// pDC->Polyline(tick, 2);
				pDC->SelectObject(pOldPen);
			} // if y
		} // if y


		// now draw minor ticks
		double yCurrent;
		double yMinorInc = getMinorIncNice(yTickNice);
		for (int i = 0; i < 9; i++) {
			yCurrent = y + yMinorInc*(i+1);
			if (yCurrent >= y + yTickNice) break; // it has reached the next major tick
			if (yCurrent > 0.) {
				int sdfsd = 1;
			}
			if (pCoord->isVisibleOrNotY(yCurrent)) {
				tick[0] = pCoord->mm_coord(xLeftAxis, yCurrent);
				tick[1].x = tick[0].x + ideltx;
				tick[1].y = tick[0].y;
				pDC->Polyline(tick, 2);
			}
		}

		// now go to the next major tick
		y += yTickNice;
	} while (y <= pCoord->Y2() + yTickNice + 0.001);	  //Finish drawing yTicks


}

void CCoords::DrawAxesTicksValues(CDC* pDC, int fontWidth, int fontHeight, BOOL isMainXSwap, BOOL isNoHorzGrid)
{
	CPoint p, tick[2];
	CString tStr;
	// xmin can be 1, which should be rounded to 0, and incremented by a regular tick value
	// xmax cannot be rounded, otherwise 899 becomes 1000, which is not good
	double x1 = this->coord.X1();
	double x2 = this->coord.X2();
	double y1 = this->coord.Y1();
	double y2 = this->coord.Y2();

	double xminNice = getMinRounded(x1, x2, 10);
	double xmaxNice = (float)niceNumMinMax(x2, true);
	double xTickNice = getTickRounded(x1, x2, 2, 10, this->coord.xTick);
	if (xTickNice < (xmaxNice - xminNice)*0.05) xTickNice = (xmaxNice - xminNice)*0.05;
	else if (xTickNice >(xmaxNice - xminNice)*0.5) xTickNice = (xmaxNice - xminNice)*0.5;
	xTickNice = getTickRounded(x1, x2, 2, 10, xTickNice);

	//if (xminNice < 0.)
	//	xminNice = 0.;
	do {
		xminNice -= xTickNice;
	} while (xminNice > x1);

	double yminNice = getMinRounded(y1, y2, 10);
	double yTickNice = getTickRounded(y1, y2, 1, 10, this->coord.getYTick());
	if (yminNice < 0.) yminNice = 0.;
	do {
		yminNice -= yTickNice;
	} while (yminNice > y1);

	// testing
	double mul = this->coord.getDefDisplayAdjust(pDC);
	int idelty = (int)(120 * mul);

	// bool isTime = this->coord.isTimeMode() == TRUE;
	double yPlot = this->coord.isFaceUp ? y1 : y2;
	double x = floor(xminNice);
	double xCurrent;
	int pixelX, pixelX2;
	do {
		// first, draw major ticks
		if (this->coord.isVisibleOrNot(x)) {
			tick[0] = this->coord.mm_coord(x, yPlot);	//delty is negative.
			tick[1].x = tick[0].x;
			tick[1].y = tick[0].y + (long)(1.5*idelty);

			tStr = getNiceLabelFromValue(x, xTickNice);

			int countS = tStr.GetLength();
			// pDC->Polyline(tick, 2);
			pDC->MoveTo(tick[0]);  // MoveTo() should be faster than Polyline()
			pDC->LineTo(tick[1]);

			// before drawing X-labels, we want to make sure they draw visibly.
			pixelX = tick[1].x - (countS*fontWidth) / 2;
			if (pixelX <  (int)ceil(mul)) pixelX = (int)ceil(mul);
			pixelX2 = pixelX + tStr.GetLength()*fontWidth + (int)ceil(mul);
			if (pixelX2 > this->coord.viewLogicalTotal.right)
				pixelX -= pixelX2 - this->coord.viewLogicalTotal.right
				+ (int)ceil(3 * mul*this->coord.device2Logical);  // 3 pixels off the right side

															  // now the labels are all in good condition
			pDC->TextOut(pixelX, tick[1].y + (int)ceil(mul), tStr);
		}



		// now draw minor ticks
		double xMinorInc = getMinorIncNice(xTickNice);
		for (int i = 0; i < 9; i++) {
			xCurrent = x + xMinorInc * (i + 1);
			if (xCurrent >= x + xTickNice) break; // it has reached the next major tick
			if (this->coord.isVisibleOrNot(xCurrent)) {
				tick[0] = this->coord.mm_coord(xCurrent, yPlot);
				tick[1].x = tick[0].x;
				tick[1].y = tick[0].y + idelty;
				// pDC->Polyline(tick, 2);
				pDC->MoveTo(tick[0]);
				pDC->LineTo(tick[1]);
			}
		}

		x += xTickNice;
	} while (x <= x2 + xTickNice + 0.001);	  //Finish drawing xTicks










	double xLeftAxis;
	double xRightAxis;
	int ideltx = (int)(130 * mul);
	if (this->coord.isSwapYLabel) isMainXSwap = !isMainXSwap;
	if (isMainXSwap == this->coord.isSwapX) ideltx = -ideltx;
	if (isMainXSwap) {
		xLeftAxis = x2;
		xRightAxis = x1;
	}
	else {
		xLeftAxis = x1;
		xRightAxis = x2;
	}


	double y = yminNice;
	// why use -0.001 ? It is because if we do not, the tick for ymin may be missed since 
	//  pCoord->Y1() can be 3.000001
	if (y > y1) y -= yTickNice;

	// now we need a fix: suppose ymin=3.7 and yTick=1, yTickNice becomes 3.5, 
	// we need to start from 3 instead of 3.5! 
	y = floor(y);


	// draw grids
	if (!isNoHorzGrid) {
		CPen gridPen(PS_DOT, 0, RGB(180, 180, 180));
		CPen* pOldPen;
		pOldPen = pDC->SelectObject(&gridPen);
		do {
			if (y > y1 &&  y < y2) {
				tick[0] = this->coord.mm_coord(xLeftAxis, y);
				tick[1].x = tick[0].x + (long)(1.5*ideltx);
				tick[1].y = tick[0].y;
				tick[1].x = this->coord.mm_coordx(xRightAxis);
				pDC->MoveTo(tick[0]);
				pDC->LineTo(tick[1]);
			} // if y

			// now go to the next major tick
			y += yTickNice;
		} while (y <= y2 + yTickNice + 0.001);	  //Finish drawing yTicks
		pDC->SelectObject(pOldPen);

	   // need to reset starting y value
		y = yminNice;
		if (y > y1) y -= yTickNice;
		y = floor(y);
	}


	do {
		// first, draw major ticks
		if (y >= y1 - 0.001 &&  y <= y2 + 0.001) {
			tick[0] = this->coord.mm_coord(xLeftAxis, y);
			tick[1].x = tick[0].x + (long)(1.5*ideltx);
			tick[1].y = tick[0].y;

			tStr = getNiceLabelFromValue(y, yTickNice);
			pDC->MoveTo(tick[0]);
			pDC->LineTo(tick[1]);

			// before drawing X-labels, we want to make sure they draw visibly.
			if (isMainXSwap && !this->coord.isSwapX)
				pixelX = tick[1].x + tStr.GetLength()*fontWidth - (int)ceil(mul);
			else if (!isMainXSwap && this->coord.isSwapX)
				pixelX = tick[1].x + (int)ceil(mul);
			else
				pixelX = tick[1].x - tStr.GetLength()*fontWidth - (int)ceil(mul);

			if (pixelX <  (int)ceil(mul)) pixelX = (int)ceil(mul);
			pixelX2 = pixelX + tStr.GetLength()*abs(fontWidth) + (int)ceil(mul);
			if (pixelX2 > this->coord.viewLogicalTotal.right)
				pixelX -= pixelX2 - this->coord.viewLogicalTotal.right;

			// now the labels are all in good condition
			pDC->TextOut(pixelX, tick[1].y - fontHeight / 2, tStr);
		} // if y


		  // now draw minor ticks
		double yCurrent;
		double yMinorInc = getMinorIncNice(yTickNice);
		for (int i = 0; i < 9; i++) {
			yCurrent = y + yMinorInc * (i + 1);
			if (yCurrent >= y + yTickNice) break; // it has reached the next major tick
			if (yCurrent > 0.) {
				int sdfsd = 1;
			}
			if (this->coord.isVisibleOrNotY(yCurrent)) {
				tick[0] = this->coord.mm_coord(xLeftAxis, yCurrent);
				tick[1].x = tick[0].x + ideltx;
				tick[1].y = tick[0].y;
				pDC->MoveTo(tick[0]);
				pDC->LineTo(tick[1]);
			}
		}

		// now go to the next major tick
		y += yTickNice;
	} while (y <= y2 + yTickNice + 0.001);	  //Finish drawing yTicks


}


// draw only major ticks of offsets at the top when in distance mode
void CCoords::DrawAxesTicksOffsets(CDC* pDC, int fontWidth, double xShotInSegy)
{
	if (this->coord.drawmode != 0 && this->coord.drawmode != 7 && !this->coord.isDistanceDisplay) return;
	if (!this->isXShotDefined(xShotInSegy)) return;

	double offset1 = this->coord.getOffsetFromDist(this->coord.X1(), xShotInSegy);
	double offset2 = this->coord.getOffsetFromDist(this->coord.X2(), xShotInSegy);

	CPoint p, tick[2];
	CString tStr;
	double xminNice = getMinRounded(offset1, offset2, 10);
	double xmaxNice = (float)niceNumMinMax(offset2, true);
	double xTickNice = getTickRounded(offset1, offset2, 2, 10, this->coord.xTick);
	if (xTickNice < (xmaxNice - xminNice) * 0.05) xTickNice = (xmaxNice - xminNice) * 0.05;
	else if (xTickNice > (xmaxNice - xminNice) * 0.5) xTickNice = (xmaxNice - xminNice) * 0.5;
	xTickNice = getTickRounded(offset1, offset2, 2, 10, xTickNice);

	//if (xminNice < 0.)
	//	xminNice = 0.;
	do {
		xminNice -= xTickNice;
	} while (xminNice > offset1);
	double mul = this->coord.getDefDisplayAdjust(pDC);
	int idelty = (int)(120 * mul);
	double 	x = floor(xminNice);
	double xDist;
	double y1 = this->coord.Y1();
	double y2 = this->coord.Y2();
	double 	yPlot = this->coord.isFaceUp ? y2 : y1;
	int pixelX, pixelX2;
	do {
		xDist = this->coord.isMinusXShot ? x - xShotInSegy : x + xShotInSegy;
		if (this->coord.isVisibleOrNot(xDist)) {
			tick[0] = this->coord.mm_coord(xDist, yPlot);	//delty is negative.
			tick[1].x = tick[0].x;
			tick[1].y = tick[0].y - (long)(1.5 * idelty);

			tStr = getNiceLabelFromValue(x, xTickNice);

			int countS = tStr.GetLength();
			// pDC->Polyline(tick, 2);
			pDC->MoveTo(tick[0]);  // MoveTo() should be faster than Polyline()
			pDC->LineTo(tick[1]);

			// before drawing X-labels, we want to make sure they draw visibly.
			pixelX = tick[1].x - (countS * fontWidth) / 2;
			if (pixelX < (int)ceil(mul)) pixelX = (int)ceil(mul);
			pixelX2 = pixelX + tStr.GetLength() * fontWidth + (int)ceil(mul);
			if (pixelX2 > this->coord.viewLogicalTotal.right)
				pixelX -= pixelX2 - this->coord.viewLogicalTotal.right
				+ (int)ceil(3 * mul * this->coord.device2Logical);  // 3 pixels off the right side

															  // now the labels are all in good condition
			// pDC->TextOut(pixelX, tick[1].y + (int)ceil(mul), tStr);
			if (this->coord.isVisibleOrNot(xDist))
				pDC->TextOut(pixelX, 0, tStr);
		}

		x += xTickNice;
	} while (x <= offset2 + xTickNice + 0.001);	  //Finish drawing xTicks

}

// draw only major ticks of distance at the top when in offset mode
void CCoords::DrawAxesTicksDistance(CDC* pDC, int fontWidth, double xShotInSegy)
{
	if (this->coord.drawmode != 0 && this->coord.drawmode != 7 && this->coord.isDistanceDisplay) return;
	if (!this->isXShotDefined(xShotInSegy)) return;

	double dist1 = this->coord.X1() + xShotInSegy;
	double dist2 = this->coord.X2() + xShotInSegy;

	CPoint p, tick[2];
	CString tStr;
	double xminNice = getMinRounded(dist1, dist2, 10);
	double xmaxNice = (float)niceNumMinMax(dist2, true);
	double xTickNice = getTickRounded(dist1, dist2, 2, 10, this->coord.xTick);
	if (xTickNice < (xmaxNice - xminNice) * 0.05) xTickNice = (xmaxNice - xminNice) * 0.05;
	else if (xTickNice > (xmaxNice - xminNice) * 0.5) xTickNice = (xmaxNice - xminNice) * 0.5;
	xTickNice = getTickRounded(dist1, dist2, 2, 10, xTickNice);

	//if (xminNice < 0.)
	//	xminNice = 0.;
	do {
		xminNice -= xTickNice;
	} while (xminNice > dist1);
	double mul = this->coord.getDefDisplayAdjust(pDC);
	int idelty = (int)(120 * mul);
	double 	x = floor(xminNice);
	double xOffset;
	double y1 = this->coord.Y1();
	double y2 = this->coord.Y2();
	double 	yPlot = this->coord.isFaceUp ? y2 : y1;
	int pixelX, pixelX2;
	do {
		xOffset = this->coord.getOffsetFromDist(x, xShotInSegy);
		if (this->coord.isVisibleOrNot(xOffset)) {
			tick[0] = this->coord.mm_coord(xOffset, yPlot);	//delty is negative.
			tick[1].x = tick[0].x;
			tick[1].y = tick[0].y - (long)(1.5 * idelty);

			tStr = getNiceLabelFromValue(x, xTickNice);

			int countS = tStr.GetLength();
			// pDC->Polyline(tick, 2);
			pDC->MoveTo(tick[0]);  // MoveTo() should be faster than Polyline()
			pDC->LineTo(tick[1]);

			// before drawing X-labels, we want to make sure they draw visibly.
			pixelX = tick[1].x - (countS * fontWidth) / 2;
			if (pixelX < (int)ceil(mul)) pixelX = (int)ceil(mul);
			pixelX2 = pixelX + tStr.GetLength() * fontWidth + (int)ceil(mul);
			if (pixelX2 > this->coord.viewLogicalTotal.right)
				pixelX -= pixelX2 - this->coord.viewLogicalTotal.right
				+ (int)ceil(3 * mul * this->coord.device2Logical);  // 3 pixels off the right side

															  // now the labels are all in good condition
			// pDC->TextOut(pixelX, tick[1].y + (int)ceil(mul), tStr);
			if (this->coord.isVisibleOrNot(xOffset))
				pDC->TextOut(pixelX, 0, tStr);
		}

		x += xTickNice;
	} while (x <= dist2 + xTickNice + 0.001);	  //Finish drawing xTicks

}

void CCoords::DrawAxes(CDC* pDC, CCoords::SEI_COORD *pCoord, CRect rect, BOOL isAxisGray)
{
	// NOTE: leave rect to be non-global, since we may want to draw axes multiple locations in the same window
	CPen* pOldPen;
	//int i = (int)myRound((rect.right-rect.left)*0.0004);
	//if (i<2) i=2;
	CPen whitePen(PS_SOLID, 1, RGB(202, 202, 202));
	CPen blackPen(PS_SOLID, 1, BLACK);

	// Use white border only if seismic is shown in the clipboard
	//bool isSeismicShown =
	//	(pCoord->isTimeDomain() && !timeSecName.IsEmpty())
	//	||
	//	(!pCoord->isTimeDomain() && !depthSecName.IsEmpty())
	//	;

	if (isAxisGray)
		pOldPen = pDC->SelectObject(&whitePen);  // the background of a segy display is white. If we the border is black, it does not look good. But we do not want to use a white color as well.
	else
		pOldPen = pDC->SelectObject(&blackPen);
	CPoint p[5];
	p[0] = CPoint(rect.left, rect.top);
	p[1] = CPoint(rect.right, rect.top);
	p[2] = CPoint(rect.right, rect.bottom);
	p[3] = CPoint(rect.left, rect.bottom);
	p[4] = p[0];
	if (!isAxisGray)
		p[4].y += 1;  // why this? because if not, the vectorizing programs may treat it as a 
	// polygon and may try to close it with a color
	pDC->Polyline(p, 5);
	pDC->SelectObject(pOldPen);

}



// NOTE: after this clipping, there may be more than n points, but it should be less than 2*n points.
// so please make sure *plt has 2*n points
int CCoords::myClipPoints2b(int n, int nLimit, CPoint *plt, CCoords *pcd)
{
	double* x = (double *)ealloc1double((size_t)nLimit);
	double* z = (double *)ealloc1double((size_t)nLimit);
	for (int i = 0; i < n; i++) {
		x[i] = plt[i].x;
		z[i] = plt[i].y;
	}
	
	int n2 = myClipPoints2(n, nLimit, x, z, pcd->coord.viewLogicalTotal);
	if (n2 > nLimit) n2 = nLimit;
	for (int i = 0; i < n2; i++) {
		if (i < n) {
			plt[i].x = (int)x[i];
			plt[i].y = (int)z[i];
		}
	}
	free1double(x);
	free1double(z);
	return n2;
}