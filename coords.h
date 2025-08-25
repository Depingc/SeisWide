#pragma once
#include "segy.h"
#include "util.h"  // do not include util.h header, since it is done the otherway
class CCoords
{
public:

	struct REDUCE{
		//for NMO display mostly, so that can be compared to reflection profile
		double vredDepreciated; // since this is saved in .SEI file, do not change it to float type, otherwise the .SEI file cannot be read properly!
		double rvred; // 1/vred, for convenience of computing, may

		float obsDepth;
		//float waterDepth; // this define this but do not need to be saved
		// note: This sometimes may not be assigned, but only assigned when needed;

		float targetDepth; // it is usually the same as waterDepth, but when waterdepth is small, the hyperbola
		// is still too evident, in which case one needs to increase the targetDepth to much greater

		float gunDepth;  // used in raytracing() and getReductionVelocity function in NMO display
		float waterVelocityAboveObs; // used in SB offset calc
		BOOL isAvgReducedNMO;  // an depreciated value, for backward compatibility only

		// for curvature minor tune up, added Dec. 18, 2018
		// based on https://en.wikipedia.org/wiki/Normal_distribution
		// f(x, mue, sigma2) = exp[-(x-mue)^2 / (2*sigma2)]  / sqrt(2*pi*sigma2)
		//double tuneupFactor; // usually [-1, 1], increment at 0.1sec
		//double tuneupMue; // usually in [5, 10], increment at 0.5
		//double tuneupSigma2; // usually in [0.2, 5.0], increment 0.1, but when >1, increment at 0.5
		// end curvature tune up



		inline void clone(REDUCE *redTo) {
			redTo->rvred = rvred;
			redTo->obsDepth = obsDepth;
			redTo->gunDepth = gunDepth;
			redTo->waterVelocityAboveObs = waterVelocityAboveObs;
			redTo->isAvgReducedNMO = isAvgReducedNMO;
			redTo->targetDepth = this->targetDepth;

			//redTo->tuneupFactor = this->tuneupFactor;
			//redTo->tuneupMue = this->tuneupMue;
			//redTo->tuneupSigma2 = this->tuneupSigma2;
		}
		inline double getVred() {
			return !isReduced() ? 0. : 1. / rvred;
		}
		inline bool isReduced() {
			return !isFloatEqual3((float)rvred, 0.f);
			// return pround(rvred, 2) != 0.0f;
		}
		inline CString toString(bool isNMO) {
			double vred = getVred();
			CString tStr;
			if (isReduced()) {
				if (isNMO) {
					tStr.Format(_T("T (s), RNMO (%.*f"), 3, (float)vred);
					tStr.TrimRight(_T(".0"));
					tStr += _T(" km/s)");
				}
				else {
					tStr.Format(_T("T - X / %.*f"), 3, (float)vred);
					tStr.TrimRight(_T(".0"));
					tStr += _T(" (s)");
				}
			}
			else {
				tStr.Format(_T("Time (s)"));
			}
			return tStr;
		}
		REDUCE() :
			vredDepreciated(0.), rvred(0.),
			isAvgReducedNMO(FALSE), obsDepth(0.f), gunDepth(0.f), targetDepth(0.f),
			waterVelocityAboveObs(1.441f)
			//,tuneupFactor(0.0),
			//tuneupMue(7.5),
			//tuneupSigma2(2.4)
		{};

		bool operator == (const REDUCE& p2) {
			const REDUCE& p1 = (*this);
			return p1.rvred == p2.rvred &&
				p1.obsDepth == p2.obsDepth &&
				p1.gunDepth == p2.gunDepth &&
				p1.waterVelocityAboveObs == p2.waterVelocityAboveObs &&
				p1.targetDepth == p2.targetDepth;
		}

		bool operator != (const REDUCE& p2) {
			const REDUCE& p1 = (*this);
			return p1.rvred != p2.rvred ||
				p1.obsDepth != p2.obsDepth ||
				p1.gunDepth != p2.gunDepth ||
				p1.waterVelocityAboveObs != p2.waterVelocityAboveObs ||
				p1.targetDepth != p2.targetDepth;
		}
	};

	struct SEI_COORD{
		int drawmode;
		// mode=-1: new document is created by user's explicit click, so a dialog box must be fired up
		// in OnDraw(). If not, it is a problem. This is now fixed correctly!

		//0: time section by km; 1: depth section; 2: velocity color map; 
		// 3: both depth section and vel color; 4: velocity contours. 5: no draw.
		// 6: time section by trace; 
		// 7: OnViewHydro; ID_MENU_VIEW_HYDRO  hydro time display. 
		// 8: Renabled for gravity model viewing--Jan. 2023! 
		// 9: OnViewVelocitycolortwtt; ID_VIEW_VELOCITYCOLORTWTT; Velocity contours will be drawn in TWTT domain.
		// 10:OnViewNmoVelocity  ID_VIEW_NMO_VELOCITY; RMS velocity display
		// 11:RMS velocity display (to be abandoned???)

		int drawmodeOld; // if drawmodeOld!=drawmode, meaning the mode is changed and the
		// thus time-depth conversion may need to perform before displaying

	//	bool isTraceDomain; // added June 30, 2014 ; to be inline function soon
		inline bool isTraceMode() {
			return drawmode == 6;
		}

		// bool isTimeDomain; // added June 30, 2014; to be inline function soon
		inline bool isTimeDomain() {
			return isTimeDomainUtil(drawmode) == TRUE;
		}

		inline bool isSeismicDisplay() {
			return drawmode <= 0 || drawmode == 1 || drawmode == 3 || drawmode > 5;
		}


		BOOL isDistanceDisplay; // linked to SEI file
		inline BOOL isOffsetDisplay() {
			// this function is computed value, and cannot be a variable
			return (drawmode == 0 || drawmode == 7) && !isDistanceDisplay;
		}

		// special case: in half offset mode, offsets are half-ed, and digitized points are not reduced,
		// but travel-time curved are still reduced. Zelt's mode cannot be this mode, evidently.
		//inline bool isHalfOffsetMode(CCoords::REDUCE *red) {
		//	return
		//		this->isDistanceDisplay && this->drawmode == 7 && 
		//		red->rvred > 0.1 && velMod.isVelModValid();
		//}

		BOOL isMinusXShot; // this controls how distance is calculated: 
		bool isFaceUp;
		BOOL isSwapX; // X-axis swapped ; required to use BOOL instead of bool since it is saved to SEI file
		BOOL isSwapYLabel; // Y-label swapped in terms of its position 
		int trace1, trace2; // added June 30, 2014
		double xmin, xmax;    // axisX1=cd.coord.xmin unless the user is zooming
		inline double X1() { return isTraceMode() ? trace1 : xmin; }
		inline double X2() { return isTraceMode() ? trace2 : xmax; }
		inline double xWidth() { return isTraceMode() ? (trace2 - trace1) : (xmax - xmin); }
	
		double m_tmin, m_tmax;  // I want to make these private, to avoid mis-use
		double m_zmin, m_zmax;
		inline double Y1() { return isTimeDomain() ? m_tmin : m_zmin; }
		inline double Y2() { return isTimeDomain() ? m_tmax : m_zmax; }
		inline double yHeight() { return isTimeDomain() ? (m_tmax - m_tmin) : (m_zmax - m_zmin); }

		inline SEI_PAIR X12() {
			SEI_PAIR sp;
			if (isTraceMode()) {
				sp.x = trace1;
				sp.y = trace2;
			}
			else {
				sp.x = xmin;
				sp.y = xmax;
			}
			return sp;
		}
		inline SEI_PAIR Y12() {
			SEI_PAIR sp;
			if (isTimeDomain()) {
				sp.x = m_tmin;
				sp.y = m_tmax;
			}
			else {
				sp.x = m_zmin;
				sp.y = m_zmax;
			}
			return sp;
		}
		inline SEI_QUAD X12Y12() {
			SEI_QUAD sq;
			if (isTraceMode()) {
				sq.x1 = trace1;
				sq.x2 = trace2;
			}
			else {
				sq.x1 = xmin;
				sq.x2 = xmax;
			}

			if (isTimeDomain()) {
				sq.y1 = m_tmin;
				sq.y2 = m_tmax;
			}
			else {
				sq.y1 = m_zmin;
				sq.y2 = m_zmax;
			}
			return sq;
		}
		inline void setx(double xx1, double xx2) {
			if (isTraceMode()) {
				trace1 = (int)myRound(xx1);
				trace2 = (int)myRound(xx2);
			}
			else {
				xmin = xx1;
				xmax = xx2;
			}
		}

		// Note: before calling these functions, please make sure the 
		//    time- or depth-domains are already set. This has happened when
		//    the domains are just changed but the indicators have not updated yet
		//    and as a result, the setXXX functions here do not work properly!!!
		inline void sety(double yy1, double yy2) {
			if (isTimeDomain()) {
				m_tmin = yy1;
				m_tmax = yy2;
			}
			else {
				m_zmin = yy1;
				m_zmax = yy2;
			}
		}
		inline void setY1(double yy1) {
			if (isTimeDomain()) {
				m_tmin = yy1;
			}
			else {
				m_zmin = yy1;
			}
		}
		inline void setY2(double yy2) {
			if (isTimeDomain()) {
				m_tmax = yy2;
			}
			else {
				m_zmax = yy2;
			}
		}
		inline void set(double xx1, double yy1, double xx2, double yy2) {
			setx(xx1, xx2);
			sety(yy1, yy2);
		}

		inline BOOL isVisibleOrNot(double x, double y) {
			if (x< X1() - 0.00001 || x> X2() + 0.00001 ||
				y< Y1() - 0.00001 || y> Y2() + 0.00001) return(FALSE);
			return(TRUE);
		}

		inline BOOL isVisibleOrNot(double x) {
			if (x< X1() - 0.00001 || x> X2() + 0.00001) return(FALSE);
			return(TRUE);
		}

		inline BOOL isVisibleOrNotY(double y) {
			if (y< Y1() - 0.00001 || y> Y2() + 0.00001) return(FALSE);
			return(TRUE);
		}

		double xTick;
		double tTick;
		double zTick;
		inline double getYTick() {
			return (isTimeDomain() ? tTick : zTick);
		}

		CRect logicalAxes; 
		// notes: we always have xmin<xmax; but if isSwapX==TRUE, xmin would be on the right side of the axis, and vice versa
		//        we always have tmin<tmax; but if isFaceUp==TRUE, tmin would be at the bottom size of the axis, and vice versa


		CRect viewLogicalTotal;
		double device2Logical;
		// CRect viewPixelsTotal;  // device (i.e. screen or printer) coords. Remove redundency
	//	CRect deviceAxes;  //device coords. This is not the entire size, only the area inside the axes.
		//double pixelPerInch;
		// note: the device can be bigger. The view can be half of it. The axis is smaller than view area, or equal to view area if no lables are needed.

		CPoint m_point; //mouse click point.
		double xMouse, yMouse;	//mouse click point in km, saved for later usage such as Invalidate().
		// //only for spacial mapping in OnModelingExtractlayerinfor() .

		double mm_xscale, mm_yscale; //A constant for use in mm_coord(), set in OnDraw() to speed calculation.
			// converts from client (km, sec)  to  logical cooords for plotting
		inline double dxPerPixel() {
			return device2Logical / mm_xscale;
		}
		inline double dyPerPixel() {
			return device2Logical / mm_yscale;
		}
		inline CRect getPixelsTotal() {
			double ov = 1. / (double)device2Logical;
			CRect pixelsTotal;
			pixelsTotal.top = 0;
			pixelsTotal.left = 0;
			pixelsTotal.right = (int)(viewLogicalTotal.right * ov + 0.5);
			pixelsTotal.bottom = (int)(viewLogicalTotal.bottom * ov + 0.5);

			// fix some occasional exceptions
			if (pixelsTotal.right < pixelsTotal.left + 10)
				pixelsTotal.right = pixelsTotal.left + 10;
			if (pixelsTotal.bottom < pixelsTotal.top + 10)
				pixelsTotal.bottom = pixelsTotal.top + 10;
			return pixelsTotal;
		}

		inline CRect getPixelsAxes() {
			double ov = 1. / (double)device2Logical;
			CRect pixelsAxes;
			pixelsAxes.top = (int)(logicalAxes.top * ov + 0.5);
			pixelsAxes.left = (int)(logicalAxes.left * ov + 0.5);
			pixelsAxes.right = (int)(logicalAxes.right * ov + 0.5);
			pixelsAxes.bottom = (int)(logicalAxes.bottom * ov + 0.5);

			// fix some occasional exceptions
			if (pixelsAxes.right < pixelsAxes.left + 10)
				pixelsAxes.right = pixelsAxes.left + 10;
			if (pixelsAxes.bottom < pixelsAxes.top + 10)
				pixelsAxes.bottom = pixelsAxes.top + 10;
			return pixelsAxes;
		}
		inline int getPenThicknessNormal() {
			return  (int)(min(logicalAxes.Width(), logicalAxes.Height()) * 0.0025);
		}

		// xOnScreen can be either in distance or offset mode
		inline double getOffsetFromXVal(double xOnScreen, double xShotInModel) {
			if (isTraceMode()) return 0.;
			if (!isXShotDefined(xShotInModel))
				return xOnScreen;  // we give up after one try

			if (isDistanceDisplay && isMinusXShot) {
				// now x is in distance mode, how do we get offset?
				return xShotInModel - xOnScreen;
			}
			else if (isDistanceDisplay && !isMinusXShot) {
				return xOnScreen - xShotInModel;
			}

			return xOnScreen;
		}

		inline double getReduceTimeEx2(double xOnScreen, double xShotInModel, CCoords::REDUCE *red)
		{
			double offset = getOffsetFromXVal(xOnScreen, xShotInModel);
			return CCoords::getReduceTimeEx2(offset, drawmode, red);
		}

		// why do we need this function? The raytraced TX pairs are distance only,
		// even you are displaying offset domains.
		inline double getOffsetFromDist(double dist, double xShot) {
			if (!isXShotDefined(xShot))
				return dist;  // we give up after one try

			if (isMinusXShot) {
				// now x is in distance mode, how do we get offset?
				return xShot - dist;
			}
			else if (!isMinusXShot) {
				return dist - xShot;
			}

			return dist;
		}



		inline long long myRound(double x)
		{
			// NOTE: then int is used as return, and the input is over 30mil or so, the return value is incorect. So I changed to long here
			// Note: if you print the result to a file with %d, it may not print antthing unless you cast it to (int) first.
			return (x > 0.) ? (long long)(x + 0.5) : (long long)(x - 0.5);
		}

		inline int mm_coordx(double x)
		{
			// A routine to convert my world coordinates (in km, sec) into logical coordinates of windows. 
			// testing the swap
			double a = isSwapX ? (X2() - x) : (x - X1());
			return (int)myRound(logicalAxes.left + a*mm_xscale);
		}

		inline void mm_coordx(float x[], int px[], int n) {
			if (n < 1 || !x || !px) return;
			double a;
			int iy;
			SEI_QUAD sq = X12Y12();

			if (isSwapX) {
				for (iy = 0; iy < n; iy++) {
					a = sq.x2 - x[iy];
					px[iy] = (int)myRound(logicalAxes.left + a * mm_xscale);
				}
			}
			else {
				for (iy = 0; iy < n; iy++) {
					a = x[iy] - sq.x1;
					px[iy] = (int)myRound(logicalAxes.left + a * mm_xscale);
				}
			}
		}


		inline int mm_coordy(double y)
		{
			double a = (!isFaceUp) ? (y - Y1()) : (Y2() - y);
			return (int)myRound(a*mm_yscale + logicalAxes.top);
		}

		inline CPoint mm_coord(double x, double y)
		{
			// A routine to convert my world coordinates (in km, sec) into logical coordinates of windows. 
			// for facedown plot only: xmin->0, xmax->xLen;  ymin->0, ymax->yLen.
			CPoint p;
			double a;
			SEI_QUAD sq = X12Y12();
			
			// a = isSwapX ? (X2() - x) : (x - X1());
			a = isSwapX ? (sq.x2 - x) : (x - sq.x1);
			p.x = (LONG)myRound(logicalAxes.left + a*mm_xscale);
			 
			a = isFaceUp ? (sq.y2 - y) : (y - sq.y1);
			p.y = (LONG)myRound(a*mm_yscale + logicalAxes.top);

			return p;

		}

		inline void mm_coord(float x[], float y[], CPoint plt[], int n)
		{
			if (n < 1) return;
			CPoint p;
			double a;
			SEI_QUAD sq = X12Y12();
			int iy;

			if (isSwapX) {
				for (iy = 0; iy < n; iy++) {
					a = sq.x2 - x[iy];
					plt[iy].x = (LONG)myRound(logicalAxes.left + a*mm_xscale);
				}
			}
			else {
				for (iy = 0; iy < n; iy++) {
					a = x[iy] - sq.x1;
					plt[iy].x = (LONG)myRound(logicalAxes.left + a*mm_xscale);
				}
			}

			if (isFaceUp) {
				for (iy = 0; iy < n; iy++) {
					a = sq.y2 - y[iy];
					plt[iy].y = (LONG)myRound(a*mm_yscale + logicalAxes.top);
				}
			}
			else {
				for (iy = 0; iy < n; iy++) {
					a = y[iy] - sq.y1;
					plt[iy].y = (LONG)myRound(a*mm_yscale + logicalAxes.top);
				}
			}

			return;
		}


		//inline double pixel2XDepreciated(CPoint p)
		//{
		//	// input is device coord, not logical!  Output is client cooord in (km, sec)
		//	// NOTE:  Cpoint2X(CPoint(1,0))  is not the same as the km per pixel!!!
		//	if (p.x < 0) return 0.f;
		//	double t = (double)logicalAxes.Width();
		//	t = (fabs(t) > 0.0) ? device2Logical / t : 0.0;

		//	double kmLeftRight = (double)(p.x - (double)logicalAxes.left / (double)device2Logical)*(X2() - X1()) * t;
		//	// note: kmLeftRight is always positive when p.y is below the top of the plotting box!

		//	if (isSwapX)
		//		return X2() - kmLeftRight;

		//	return X1() + kmLeftRight;
		//}

		// this is less efficient than pixel2XY()
		//inline double pixel2YDepreciated(CPoint p)
		//{
		//	// input is device coord, not logical!  Output is client cooord in (km, sec)
		//	if (p.y < 0) return 0.f;
		//	double ov = 1. / (double)device2Logical;

		//	double t = (double)(logicalAxes.Height() * ov);
		//	t = (fabs(t) > 0.0) ? 1. / t : 0.0;
		//	double kmTopDown = (double)(p.y - logicalAxes.top * ov)*(Y2() - Y1()) * t;  // note: kmTopDown is always positive when p.y is below the top of the plotting box!
		//	if (isFaceUp)
		//		return Y2() - kmTopDown;
		//	return Y1() + kmTopDown;
		//}

		inline SEI_PAIR logical2XY(CPoint &pLogical) {
			// input is device coord, not logical!  Output is client cooord in (km, sec)
			// NOTE:  Cpoint2X(CPoint(1,0))  is not the same as the km per pixel!!!
			SEI_QUAD sq = X12Y12();
			SEI_PAIR sp;

			if (pLogical.x < 0) {
				pLogical.x = 0;
			}
			double t = (double)logicalAxes.Width();
			t = (fabs(t) > 0.0) ? 1. / t : 0.0;
			double kmLeftRight = (double)(pLogical.x - logicalAxes.left)*(sq.x2 - sq.x1) * t;
			// note: kmLeftRight is always positive when p.y is below the top of the plotting box!
			sp.x = isSwapX ? sq.x2 - kmLeftRight : sq.x1 + kmLeftRight;


			// now calc for Y-axis
			if (pLogical.y < 0) pLogical.y = 0;
			t = (double)logicalAxes.Height();
			t = (fabs(t) > 0.0) ? 1. / t : 0.0;
			double kmTopDown = (double)(pLogical.y - logicalAxes.top)*(sq.y2 - sq.y1) * t;  // note: kmTopDown is always positive when p.y is below the top of the plotting box!
			sp.y = isFaceUp ? sq.y2 - kmTopDown : sq.y1 + kmTopDown;

			return sp;
		}

		inline SEI_PAIR pixel2XY(CPoint &p)
		{
			// input is device coord, not logical!  Output is client cooord in (km, sec)
			// NOTE:  Cpoint2X(CPoint(1,0))  is not the same as the km per pixel!!!
			SEI_QUAD sq = X12Y12();
			double ov = 1. / (double)device2Logical;
			SEI_PAIR sp;

			if (p.x < 0) p.x = 0;
			double t = (double)logicalAxes.Width();
			t = (fabs(t) > 0.0) ? device2Logical / t : 0.0;
			double kmLeftRight = (double)(p.x - logicalAxes.left * ov)*(sq.x2 - sq.x1) * t;
			// note: kmLeftRight is always positive when p.y is below the top of the plotting box!
			sp.x = isSwapX ? sq.x2 - kmLeftRight : sq.x1 + kmLeftRight;

			// now calc for Y-axis
			if (p.y < 0) p.y = 0;
			t = (double)logicalAxes.Height();
			t = (fabs(t) > 0.0) ? device2Logical / t : 0.0;
			double kmTopDown = (double)(p.y - logicalAxes.top * ov)*(sq.y2 - sq.y1) * t;  // note: kmTopDown is always positive when p.y is below the top of the plotting box!
			sp.y = isFaceUp ? sq.y2 - kmTopDown : sq.y1 + kmTopDown;
			return sp;
		}


		inline CPoint XY2pixel(double x, double y) {
			// output is device coord
			// #########################WORK needed for faceup and swapX !!!!!!!!!!!!!!!!!!
			double ov = 1. / (double)device2Logical;
			CPoint p;

			double t = X2() - X1();
			t = (fabs(t) > 0.) ? 1. / t : 0.;
			double dxFromLeft = isSwapX ? (X2() - x) : (x - X1());
			p.x = (int)myRound(ov * (logicalAxes.left + logicalAxes.Width() *  dxFromLeft * t));

			t = Y2() - Y1();
			t = (fabs(t) > 0.) ? 1. / t : 0.;
			double a = isFaceUp ? (Y2() - y) : (y - Y1());
			p.y = (int)myRound(ov * (logicalAxes.top + logicalAxes.Height() *  a * t));
			return p;
		}


		// Note: input pixel is coordinate based, meaning (0, 0) is at the origin of the coordinate system, usually not at the original of screen or plots

		// note that this function does not consider axis. Otherwise it can be confusing
		inline CPoint pixel2logicalScreen(int pixelX, int pixelY) {
			CPoint p;
			//p.x = logicalAxes.left + (int)myRound(pixelX * (double)device2Logical);
			//p.y = logicalAxes.top + (int)myRound(pixelY * (double)device2Logical);
			p.x = (int)myRound(pixelX * (double)device2Logical);
			p.y = (int)myRound(pixelY * (double)device2Logical);
			return p;
		}

		inline CRect logical2Pixel(CRect* pLogical) {
			double ov = 1. / (double)device2Logical;
			CRect out;
			out.left = (int)myRound(pLogical->left * ov);
			out.top = (int)myRound(pLogical->top * ov);
			out.right = (int)myRound(pLogical->right * ov);
			out.bottom = (int)myRound(pLogical->bottom * ov);
			return out;
		}

		inline BOOL isModelingMode() {
			return (drawmode == 1 || drawmode == 2 || drawmode == 3 || drawmode == 4 || drawmode == 8 || drawmode == 9);
		}



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

		inline double getDefDisplayAdjust(CDC *pDC) {
			double pixelPerInch = pDC->GetDeviceCaps(LOGPIXELSX);
			if (pixelPerInch < 1.) pixelPerInch = 96.;
			return pixelPerInch * 0.01;
		}


		inline int getDeviceResAdjust(CDC *pDC, int x) {
			double a = getDefDisplayAdjust(pDC);
			return (int)myRound(a * x);
		}

		SEI_COORD() :
			drawmode(-1), drawmodeOld(-1),
			trace1(1),trace2(1000),
			isDistanceDisplay(FALSE), isMinusXShot(FALSE), m_tmin(0.), m_tmax(8.), m_zmin(0.), m_zmax(10.),
			logicalAxes(CRect(0, 100, 0, 100)),
			viewLogicalTotal(logicalAxes), device2Logical(35.),
			xmin(0),xmax(100.0),
			isFaceUp(false), isSwapX(false), isSwapYLabel(false),
			xTick(1.), tTick(1.), zTick(1.) 
			{ }
	} coord;
	// coord  is the only coordination variable to create, no further coord since each view has only 1 mapping rule.



	static BOOL isXShotDefined(double xshot);
	static double getOffsetFromXVal(double x, double xShot, BOOL isDistanceDisplay, BOOL isMinusXShot);
	//inline double pixel2XDepreciated(CPoint p) {
	//	// input is device coord, not logical!  Output is client cooord in (km, sec)
	//	// NOTE:  Cpoint2X(CPoint(1,0))  is not the same as the km per pixel!!!
	//	if (p.x < 0) return 0.f;
	//	double t = (double)coord.logicalAxes.Width();
	//	t = (fabs(t) > 0.0) ? coord.device2Logical / t : 0.0;
	//	double kmLeftRight = (double)(p.x - (double)coord.logicalAxes.left / (double)coord.device2Logical)*(coord.X2() - coord.X1()) * t;
	//	// note: kmLeftRight is always positive when p.y is below the top of the plotting box!

	//	if (coord.isSwapX)
	//		return coord.X2() - kmLeftRight;

	//	return coord.X1() + kmLeftRight;
	//}
	//inline double pixel2YDepreciated(CPoint p) {
	//	// input is device coord, not logical!  Output is client cooord in (km, sec)
	//	if (p.y < 0) return 0.f;
	//	double ov = 1. / (double)coord.device2Logical;

	//	double t = (double)(coord.logicalAxes.Height() * ov);
	//	t = (fabs(t) > 0.0) ? 1. / t : 0.0;
	//	double kmTopDown = (double)(p.y - coord.logicalAxes.top * ov)*(coord.Y2() - coord.Y1()) * t;  // note: kmTopDown is always positive when p.y is below the top of the plotting box!
	//	if (coord.isFaceUp)
	//		return coord.Y2() - kmTopDown;
	//	return coord.Y1() + kmTopDown;
	//}
	//inline CPoint XY2pixelDepreciated(double x, double y) {
	//	// output is device coord
	//	// #########################WORK needed for faceup and swapX !!!!!!!!!!!!!!!!!!
	//	double ov = 1. / (double)coord.device2Logical;
	//	CPoint p;

	//	double t = coord.X2() - coord.X1();
	//	t = (fabs(t) > 0.) ? 1. / t : 0.;
	//	double dxFromLeft = coord.isSwapX ? (coord.X2() - x) : (x - coord.X1());
	//	p.x = (int)myRound(ov * (coord.logicalAxes.left + coord.logicalAxes.Width() *  dxFromLeft * t));

	//	t = coord.Y2() - coord.Y1();
	//	t = (fabs(t) > 0.) ? 1. / t : 0.;
	//	double a = coord.isFaceUp ? (coord.Y2() - y) : (y - coord.Y1());
	//	p.y = (int)myRound(ov * (coord.logicalAxes.top + coord.logicalAxes.Height() *  a * t));
	//	return p;
	//}
	//// Note: input pixel is coordinate based, meaning (0, 0) is at the original of the coordinate system, usually not at the original of screen or plots
	//inline CPoint pixel2logicalDepreciated(int pixelX, int pixelY) {
	//	CPoint p;
	//	p.x = coord.logicalAxes.left + (int)myRound(pixelX * (double)coord.device2Logical);
	//	p.y = coord.logicalAxes.top + (int)myRound(pixelY * (double)coord.device2Logical);
	//	return p;
	//}


	// if lableOutsideStat==0, there is formal labels.
	//    lableOutsideStat>0,  no labels at all
	//    lableOutsideStat<0, very small labels at the top
	void setCoordMappingFromWorld(CDC* pDC, 
		int lableOutsideStat, 
		bool isYLabelOnRight,
		double shrinkX, 
		double shrinkY, 
		CCoords::SEI_COORD *coord);
	void setCoordMappingFromWorldOld(CDC* pDC, 
		int lableOutsideStat,
		bool isYLabelOnRight,
		CSize pixelTotal,
		CSize logicalTotal,
		double shrinkX, double shrinkY, 
		CCoords::SEI_COORD *coord);
	void setCoordMappingFromWorld(CDC* pDC,
		int lableOutsideStat,
		bool isYLabelOnRight,
		CSize pixelTotal,
		CSize logicalTotal,
		double shrinkX, double shrinkY,
		CCoords::SEI_COORD *coord);

	void DrawAxesTicksValues(CDC* pDC, int fontWidth, int fontHeight, BOOL isMainXSwap, BOOL isNoHorzGrid);
	void DrawAxesTicksOffsets(CDC* pDC, int fontWidth, double xShotInSegy);
	void DrawAxesTicksDistance(CDC* pDC, int fontWidth, double xShotInSegy);

	static void DrawAxesTicksValuesDepre(CDC* pDC, CCoords::SEI_COORD *pCoord,
		int fontWidth, int fontHeight, BOOL isMainXSwap, BOOL isNoHorzGrid);
	static void DrawAxes(CDC* pDC, CCoords::SEI_COORD *pCoord, CRect rect, BOOL isAxisBlack);


	struct ZOOM{
		double xminOld,xmaxOld,yminOld,ymaxOld; // the very original axis limits before zooming
		double xTickOld, yTickOld;  // the original ticks before zooming
		int isZooming;  // >0: zooming to details; <0: unzooming to less details; 0: no zoom
		BOOL isMouseHand;
		ZOOM(): isZooming(0), isMouseHand(FALSE) {};
	} zoom;



	static double getReduceTimeTraditional(double offset, int drawmode, double rvred);
	static double getReduceTimeEx2(double offset, int drawmode, CCoords::REDUCE *red);
	//static double getReduceHyperbola(double offset, CCoords::REDUCE *red);

	static double getReduceTimeExOld2(double offset, int drawmode, CCoords::REDUCE *red);
	static int myClipPoints2b(int n, int nLimit, CPoint *plt, CCoords *pcd);

	CCoords(void);
	~CCoords(void);

};
