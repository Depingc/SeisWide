#include "stdafx.h"
#include "math.h"
#include "float.h"
#include "ealloc_d.h"
#include "dcHelper.h"
#include "DIB.h"
#include "sumig.h"
#include "xallocator.h"

CDCHelper::CDCHelper() 
{
}

CDCHelper::~CDCHelper() 
{
}









//BOOL CDCHelper::floodFillArr8Old(CDC* pDC, int nx, int nt, float **ampArr, float *rangeArr,
//	double xmin, double xmax, double tmin, double tmax,
//	CColorHelper::AMPCOLORDYN *ampColorDyn2, CCoords *pcd, int memMaxMB)
//{
//	// will use globals: pcd->coord.ix1, ix2, iy1, iy2, x1, x2
//	// xmin and xmax may be different from pcd->coord.X1() and pcd->coord.X2()
//	if (nt < 2) return FALSE;
//
//	CString tStr;
//	int i, ix, iy, irow, icol;
//	// calculate how many rows and columns for the bitmap are needed: deviceAxes.Height() < 0 when displayed facing up
//
//	// bitmaps are always generated with positive ncol and nrow
//	double a;
//	a = (xmax - xmin) / pcd->coord.xWidth() * pcd->coord.logicalAxes.Width() / (double)pcd->coord.device2Logical;
//	int ncol = (int)ceil(a);
//	a = (tmax - tmin) / pcd->coord.yHeight() * pcd->coord.logicalAxes.Height() / (double)pcd->coord.device2Logical;
//	int nrow = (int)ceil(a);
//	ncol = abs(ncol) + 1;
//	nrow = abs(nrow) + 1;
//
//	// one can either load an existing bitmap file or create an empty bitmap object
//	CDib dib;
//
//	TRY{
//		// note: after this call, nrow or ncol can change
//		float reduceRate = dib.Create24(&ncol, &nrow, memMaxMB);
//		if (reduceRate == 0.f) {
//			//tStr.Format(_T("Failed to create bitmap with %d MB max memory. Try reduce max memory of usage."), draw.memMaxMB);
//			//// throw std::runtime_error(std::string("Failed to create %d GB bitmap in memory."));
//			//pFrame->myCaption(tStr);
//			return FALSE;
//		}
//
//		dib.SetPalette(pDC);
//
//		//################################
//		// note:  draw.trcInt  did not include trace skipping. So we have to re-calculate the allowable trace span width during display
//		float *dx = (float *)ealloc1float(nx);
//		for (ix = 1; ix < nx; ix++) {
//			dx[ix - 1] = (float)fabs(rangeArr[ix] - rangeArr[ix - 1]);
//		}
//		double traceSpanUse = 1. * getMedianValue(dx, nx - 1);	//This is the average trace interval in km.
//		free1float(dx);
//		double traceSpanMin = (xmax - xmin) * 0.005; 
//		// traceSpanMin guarantees that when zoomed up a lot, the colors span a larger interval
//		// otherwise each trace can have gap strips which can be ugly
//		if (traceSpanUse < traceSpanMin)  
//			traceSpanUse = traceSpanMin;
//		// traceSpanUse cannot be too big as well, otherwise when there is a large data gap, the display will 
//		// fill the gap with exceptionally visible data.
//		//################################
//
//		//double xRate = (pcd->coord.X2()-pcd->coord.X1()) /(double)(ncol-1);
//		double xRate = (xmax - xmin) / (double)(ncol - 1);
//		double tRate = (tmax - tmin) / (double)(nrow - 1);
//		// note: yRate is for each pixel increment, while dt is for each data sample. They are different
//		//yRate /= reduceRate;
//		double dt = (tmax - tmin) / (double)(nt - 1);
//	//	static COLORREF colWhite = RGB(255, 255, 255);
//		float *yValPixel = (float *)ealloc1float(nrow);
//		COLORREF *colors = (COLORREF *)alloc1(nrow, sizeof(COLORREF));
//		yValPixel[0] = (float)tmin;
//		for (irow = 1; irow < nrow; irow++) {
//			yValPixel[irow] = yValPixel[irow - 1] + (float)tRate;
//		}
//		float **ampArrPixel = (float **)ealloc2float(nrow, nx);
//		for (ix = 0; ix < nx; ix++) {
//			ints8r(nt, // ntin
//				(float)dt,   // dtin
//				(float)tmin,   // ftin
//				&ampArr[ix][0], // yin[t=ntin], with yin[0] = y(fxin)
//				0.f,  // yinl    value used to extrapolate yin values to left of yin[0]
//				0.f,  // yinr	 value used to extrapolate yin values to right of yin[ntin-1]
//				nrow,   // nxout
//				&yValPixel[0],  // ntout
//				&ampArrPixel[ix][0]);    // tout
//		}
//
//
//
//		int ix0 = 0;
//		double dataGapThreshold = traceSpanUse * 4;
//		for (icol = 0; icol < ncol; icol++) {
//			register double xdouble = xmin + icol * xRate;
//			ix0 = 0;
//			ix = getClosestIndex(rangeArr, nx, (float)xdouble, ix0);
//
//			// remove any data gaps
//			if (ix < 0) continue;
//
//			// these are very fast since they normally do not exist
//			register bool isInsideDatagap1 = ix + 1<nx
//				&& rangeArr[ix + 1] - rangeArr[ix] > dataGapThreshold
//				&& xdouble>rangeArr[ix]
//				;
//			register bool isInsideDatagap2 = ix - 1>0
//				&& rangeArr[ix] - rangeArr[ix - 1] > dataGapThreshold
//				&& xdouble<rangeArr[ix]
//				;
//
//			// start coloring
//			if (isInsideDatagap1 || isInsideDatagap2 || fabs(rangeArr[ix] - xdouble) > traceSpanUse) {
//				for (irow = 0; irow < nrow; irow++) {
//					dib.SetDibBit24(icol, irow, WHITE);
//				}
//			}
//			else {
//				// note: rangeArr[] should be monotonically increasing or decreasing, what about if not? Then we should use the closest index only!
//				register int jx = -1;
//				if (xdouble <= rangeArr[0]) jx = 0;
//				else if (xdouble >= rangeArr[nx - 1]) jx = nx - 1;
//				else {
//					for (i = 0; i<nx - 1; i++) {
//						if (xdouble >= rangeArr[i] && xdouble <= rangeArr[i + 1]) {
//							jx = i;
//							break;
//						}
//					}
//				}
//
//				if (jx < 0) {
//					// this would mean the data index cannot be found. Usually meaning no data in the display window. 
//					// because of the programming effort above, this has not happened, and should not.
//					jx = 0;
//				}
//				else if (jx > nx - 1) jx = nx - 1;
//
//				register int jx2 = jx + 1;
//				if (jx2>nx - 1) jx2 = nx - 1;
//
//				if (jx > 0 && jx2 < nx - 1) {
//					CColorHelper::getInterpol4Points(
//						rangeArr[jx - 1], rangeArr[jx], rangeArr[jx2], rangeArr[jx2 + 1],
//						ampArrPixel[jx - 1], ampArrPixel[jx], ampArrPixel[jx2], ampArrPixel[jx2 + 1],
//						(float)xdouble, nrow,
//						yValPixel);
//				}
//				else {
//					a = rangeArr[jx2] - rangeArr[jx];
//					register double percentFromJx = fabs(a) < MINAMP ? 0. : (xdouble - rangeArr[jx]) / a;
//					if (percentFromJx < 0.0) percentFromJx = 0.0;
//					else if (percentFromJx > 1.0) percentFromJx = 1.0;
//					for (irow = 0; irow < nrow; irow++) {
//						yValPixel[irow] = (float)(ampArrPixel[jx][irow] + 
//							percentFromJx * (ampArrPixel[jx2][irow] - ampArrPixel[jx][irow]));
//					}
//				}
//				ampColorDyn2->getColorRef4Amp(yValPixel, colors, nrow);
//				dib.SetDibBit24(icol, nrow, colors);
//			}
//		} // for icol
//		free1(colors);
//		free1float(yValPixel);
//		free2float(ampArrPixel);
//
//
//		// test for color filter
//		if (false) {
//			CColorHelper::hsv *hsv = dib.getHsvAll();
//			dib.filterLoG5(hsv);
//			free1(hsv);
//		}
//
//
//
//
//		// NOTE: ft (in sec) corresponds to the first data point in y. But pcd->coord.logicalAxes.top in client coordinate is the min Y-axis. They may be different!
//		// bool isTime = pcd->coord.isTimeDomain == TRUE;
//		CPoint p1 = pcd->coord.mm_coord(xmin, tmin);
//		CPoint p2 = pcd->coord.mm_coord(xmax, tmax);
//		iy = min(p1.y, p2.y);
//		p2.y = max(p1.y, p2.y);
//		p1.y = iy;
//
//		if (!dib.Draw(pDC, p1.x, p1.y, p2.x - p1.x, p2.y - p1.y, !pcd->coord.isFaceUp)) {
//			// throw  std::runtime_error("Could not draw bitmap.");   // no effect in this throw. Why???
//			throw;
//		}
//
//
//	}
//		CATCH_ALL(e)
//	{
//		//tStr.Format(_T("Failed to create bitmap with %d MB max memory."), memMaxMB);
//		return FALSE;
//	}
//	END_CATCH_ALL
//
//		if (tStr.GetLength() > 0)
//			AfxMessageBox(tStr, MB_ICONQUESTION);
//
//	return TRUE;
//}


BOOL CDCHelper::floodFillArr9(CDC* pDC, int nx, int nt, float **ampArr, float *rangeArr,
	double xmin, double xmax, double tmin, double tmax,
	CColorHelper::AMPCOLORDYN *ampColorDyn2, CCoords *pcd)
{

	// will use globals: pcd->coord.ix1, ix2, iy1, iy2, x1, x2
	// xmin and xmax may be different from pcd->coord.X1() and pcd->coord.X2()
	if (nt < 2) return FALSE;
	if (nx < 2) return FALSE; // if nx<2, and do not return, this routine will cause exception!!!

	CString tStr;
	int i, ix, iy, irow, icol;

	// calculate how many rows and columns for the bitmap are needed: deviceAxes.Height() < 0 when displayed facing up
	// bitmaps are always generated with positive ncol and nrow
	double a;
	a = (xmax - xmin) / pcd->coord.xWidth() * (double)pcd->coord.logicalAxes.Width() / (double)pcd->coord.device2Logical;
	int ncol = (int)ceil(fabs(a)) + 1;
	a = (tmax - tmin) / pcd->coord.yHeight() * (double)pcd->coord.logicalAxes.Height() / (double)pcd->coord.device2Logical;
	int nrow = (int)ceil(fabs(a)) + 1;

	//################################
	// note:  draw.trcInt  did not include trace skipping. So we have to re-calculate the allowable trace span width during display
	float *dx = (float *)ealloc1float(nx);
	for (ix = 1; ix < nx; ix++) {
		dx[ix - 1] = (float)fabs(rangeArr[ix] - rangeArr[ix - 1]);
	}
	double traceSpanUse = 1. * getMedianValue(dx, nx - 1);	//This is the average trace interval in km.
	free1float(dx);
	double traceSpanMin = (xmax - xmin) * 0.005;
	// traceSpanMin guarantees that when zoomed up a lot, the colors span a larger interval
	// otherwise each trace can have gap strips which can be ugly
	if (traceSpanUse < traceSpanMin)
//		traceSpanMin = traceSpanUse; // if I do not deal with this line, it may display very few traces in occasional cases
		traceSpanUse = traceSpanMin;
	// traceSpanUse cannot be too big as well, otherwise when there is a large data gap, the display will 
	// fill the gap with exceptionally visible data.
	//################################

	//double xRate = (pcd->coord.X2()-pcd->coord.X1()) /(double)(ncol-1);
	double xRate = (xmax - xmin) / (double)(ncol - 1);
	double tRate = (tmax - tmin) / (double)(nrow - 1);
	// note: yRate is for each pixel increment, while dt is for each data sample. They are different
	//yRate /= reduceRate;
	double dt = (tmax - tmin) / (double)(nt - 1);
	float *yValPixel = (float *)ealloc1float(nrow);
	COLORREF *colors = (COLORREF *)alloc1(nrow, sizeof(COLORREF));
	yValPixel[0] = (float)tmin;
	for (irow = 1; irow < nrow; irow++) {
		yValPixel[irow] = yValPixel[irow - 1] + (float)tRate;
	}
	float **ampArrPixel = (float **)ealloc2float(nrow, nx);
	float vl, vr;
	for (ix = 0; ix < nx; ix++) {
		vl = ampArr[ix][0];
		vr = ampArr[ix][nt - 1];
		ints8r(nt, // ntin
			(float)dt,   // dtin
			(float)tmin,   // ftin
			&ampArr[ix][0], // yin[t=ntin], with yin[0] = y(fxin)
			vl,  // yinl    value used to extrapolate yin values to left of yin[0]
			vr,  // yinr	 value used to extrapolate yin values to right of yin[ntin-1]
			nrow,   // nxout
			&yValPixel[0],  // ntout
			&ampArrPixel[ix][0]);    // tout
	}


	// one can either load an existing bitmap file or create an empty bitmap object
	CDib dib;

	TRY{
		// note: after this call, nrow or ncol can change
		float reduceRate = dib.Create24b(ncol, nrow);
	if (reduceRate == 0.f) {
		return FALSE;
	}

	dib.SetPalette(pDC);

	int ix0 = 0;
	double dataGapThreshold = traceSpanUse * 1.3; // Using >1.3 will create a data gap explicitly. Previously I used 4, and the data gap is filled with some big amplitudes.
	for (icol = 0; icol < ncol; icol++) {
		//if (icol < 1500) continue;
		//if (icol > 2000) break;
		register double xdouble = xmin + icol * xRate;
		if (xdouble < xmin || xdouble > xmax) continue;
		ix0 = 0;
		ix = getClosestIndex(rangeArr, nx, (float)xdouble, ix0);

		// remove any data gaps
		if (ix < 0) continue;

		// these are very fast since they normally do not exist
		register bool isInsideDatagap1 = ix + 1<nx
			&& rangeArr[ix + 1] - rangeArr[ix] > dataGapThreshold
			&& xdouble>rangeArr[ix]
			;
		register bool isInsideDatagap2 = ix - 1>0
			&& rangeArr[ix] - rangeArr[ix - 1] > dataGapThreshold
			&& xdouble<rangeArr[ix]
			;

		//isInsideDatagap1 = isInsideDatagap2 = false; // testing

		// start coloring
		if (isInsideDatagap1 || isInsideDatagap2 || fabs(rangeArr[ix] - xdouble) > traceSpanUse) {
			//for (irow = 0; irow < nrow; irow++) {
			//	dib.SetDibBit24(icol, irow, WHITE);  // we already pre-filled with WHITE values
			//}
		}
		else {
			// note: rangeArr[] should be monotonically increasing or decreasing, what about if not? Then we should use the closest index only!
			register int jx = -1;
			if (xdouble <= rangeArr[0]) jx = 0;
			else if (xdouble >= rangeArr[nx - 1]) jx = nx - 1;
			else {
				for (i = 0; i<nx - 1; i++) {
					if (xdouble >= rangeArr[i] && xdouble <= rangeArr[i + 1]) {
						jx = i;
						break;
					}
				}
			}

			if (jx < 0) {
				// this would mean the data index cannot be found. Usually meaning no data in the display window. 
				// because of the programming effort above, this has not happened, and should not.
				jx = 0;
			}
			else if (jx > nx - 1) jx = nx - 1;

			register int jx2 = jx + 1;
			if (jx2>nx - 1) jx2 = nx - 1;

			a = rangeArr[jx2] - rangeArr[jx];
			//if (fabs(a) < xRate) continue; // smaller than a pixel, skip it

			if (jx > 0 && jx2 < nx - 1) {
				CColorHelper::getInterpol4Points(
					rangeArr[jx - 1], rangeArr[jx], rangeArr[jx2], rangeArr[jx2 + 1],
					ampArrPixel[jx - 1], ampArrPixel[jx], ampArrPixel[jx2], ampArrPixel[jx2 + 1],
					(float)xdouble, nrow,
					yValPixel);
			}
			else if (fabs(a) < MINAMP) {
				// just copy the trace to yValPixel[]
				memcpy(yValPixel, &ampArrPixel[jx][0], nrow * sizeof(float));
			}
			else {
				// we can only use jx and jx2
				// note: the following codes can cause extra stuff outside the axis, and I
				// do not know why. So we can just ignore this part of energy for now!!!
				register double percentFromJx = (xdouble - rangeArr[jx]) / a;
				if (percentFromJx < 0.0 || percentFromJx > 1.0) continue;
				for (irow = 0; irow < nrow; irow++) {
					yValPixel[irow] = (float)(ampArrPixel[jx][irow] +
						percentFromJx * (ampArrPixel[jx2][irow] - ampArrPixel[jx][irow]));
				}
			}
			ampColorDyn2->getColorRef4Amp(yValPixel, colors, nrow);
			dib.SetDibBit24(icol, nrow, colors);
		}
	} // for icol
	free1(colors);
	free1float(yValPixel);
	free2float(ampArrPixel);


	// test for color filter
	if (false) {
		CColorHelper::hsv *hsv = dib.getHsvAll();
		dib.filterLoG5(hsv);
		free1(hsv);
	}




	// NOTE: ft (in sec) corresponds to the first data point in y. But pcd->coord.logicalAxes.top in client coordinate is the min Y-axis. They may be different!
	// bool isTime = pcd->coord.isTimeDomain == TRUE;
	CPoint p1 = pcd->coord.mm_coord(xmin, tmin);
	CPoint p2 = pcd->coord.mm_coord(xmax, tmax);
	iy = min(p1.y, p2.y);
	p2.y = max(p1.y, p2.y);
	p1.y = iy;

	if (!dib.Draw(pDC, p1.x, p1.y, p2.x - p1.x, p2.y - p1.y, !pcd->coord.isFaceUp)) {
		throw;
	}


	}
		CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL

	if (tStr.GetLength() > 0)
		AfxMessageBox(tStr, MB_ICONQUESTION);

	return TRUE;
}





// testing ....
BOOL CDCHelper::tessellateFill2Rect(CDC* pDC, int nx, int ny, float **ampArr, float *ranges,
	double xmin, double xmax, double ymin, double ymax,
	CColorHelper::AMPCOLORDYN *ampColorDyn2, CCoords *pcd, int memMaxMB)
{
	// will use: pcd->coord.ix1, ix2, iy1, iy2, x1, x2
	// xmin and xmax may be different from pcd->coord.X1() and pcd->coord.X2()
	// note: ranges[] should be monotonically increasing or decreasing, what about if not? 
	if (nx<2 || ny < 2) return FALSE;

	CString tStr;
	int ix, iy;
	// calculate how many rows and columns for the bitmap are needed: deviceAxes.Height() < 0 when displayed facing up

	// bitmaps are always generated with positive ncol and nrow
	double a;
	a = (xmax - xmin) / pcd->coord.xWidth() * pcd->coord.logicalAxes.Width() / (double)pcd->coord.device2Logical;
	int ncol = (int)ceil(a);
	a = (ymax - ymin) / pcd->coord.yHeight() * pcd->coord.logicalAxes.Height() / (double)pcd->coord.device2Logical;
	int nrow = (int)ceil(a);
	ncol = abs(ncol) + 1;
	nrow = abs(nrow) + 1;

	// one can either load an existing bitmap file or create an empty bitmap object
	CDib dib;

	// note: after this call, nrow or ncol can change
	float reduceRate = dib.Create24b(ncol, nrow);
	if (reduceRate == 0.f) return FALSE;
	dib.SetPalette(pDC);

	//################################
	double dxPixel = (xmax - xmin) / (double)(ncol - 1);  // km per pixel
	double dyPixel = (ymax - ymin) / (double)(nrow - 1); // seconds per pixel
	double dy = (ymax - ymin) / (double)(ny - 1);
	int numSamp2Skip = (int)(dyPixel / dy + 0.5); // if 0, no need to skip and we can use memcpy for the entire trace
												  //################################
												  // first, we produce a newer version of the ampArr** array to avoid too dense data
	float **amp2 = (float **)ealloc2float(ny, nx);
	int nyBytes = ny * sizeof(float);

	//int ny2 = 1 + (numSamp2Skip == 0 ? ny : ny / (numSamp2Skip + 1));
	int ny2;
	if (numSamp2Skip>0) {
		for (ix = 0; ix < nx; ix++) {
			ny2 = 0;
			for (iy = 0; iy < ny; iy += numSamp2Skip + 1) {
				amp2[ix][ny2] = ampArr[ix][iy];
				ny2++;
			}
		}
	}
	else {
		memcpy(&amp2[0][0], &ampArr[0][0], nyBytes*nx);
		ny2 = ny;
	}


	// now skip along X axis if traces are too dense
	int nyBytes2 = ny2 * sizeof(float);
	float *ranges2 = (float *)ealloc1float(nx);
	int ix2 = 0;
	ranges2[ix2] = ranges[0];
	memcpy(&amp2[ix2][0], &amp2[0][0], nyBytes2);
	ix2++;
	for (ix = 1; ix < nx; ix++) {
		if (fabs(ranges[ix] - ranges2[ix2 - 1]) < dxPixel) continue; // skip this trace ix
		memcpy(&amp2[ix2][0], &amp2[ix][0], nyBytes2);
		ranges2[ix2] = ranges[ix];
		ix2++; // this index is now accepted for plotting
	}
	int nx2 = ix2;
	//################################
	double dy2 = (ymax - ymin) / (double)(ny2 - 1);
	float *y2 = (float *)ealloc1float(ny2);
	y2[0] = (float)ymin;
	for (iy = 1; iy < ny2; iy++) {
		y2[iy] = y2[iy - 1] + (float)dy2;
	}
	//################################
	int *pltx = (int *)alloc1int(nx2);
	int *plty = (int *)alloc1int(ny2);
	for (ix = 0; ix < nx2; ix++) {
		pltx[ix] = pcd->coord.mm_coordx(ranges2[ix]);
	}
	for (iy = 0; iy < ny2; iy++) {
		plty[iy] = pcd->coord.mm_coordy(y2[iy]);
	}
	//################################
	COLORREF *colors = (COLORREF *)alloc1(ny2, sizeof(COLORREF));
	TRIVERTEX *vertexArr = (TRIVERTEX *)alloc1(nx2*ny2, sizeof(TRIVERTEX));
	GRADIENT_RECT *gradRectArr = (GRADIENT_RECT *)alloc1((nx2-1)*(ny2-1), sizeof(GRADIENT_RECT));
	for (ix = 0; ix < nx2; ix++) {
		register int ixx = ix*ny2;
		ampColorDyn2->getColorRef4Amp(amp2[ix], colors, ny2);
		for (iy = 0; iy < ny2; iy++) {
			vertexArr[iy + ixx] = { pltx[ix], plty[iy],
				CColorHelper::RVal16(colors[iy]),
				CColorHelper::GVal16(colors[iy]),
				CColorHelper::BVal16(colors[iy]),
				0x0000 };
		}
	}

	for (ix = 0; ix < nx2 - 1; ix++) {
		register int ixx = ix*ny2;
		register int ixx2 = ix*(ny2 - 1);
		for (iy = 0; iy < ny2 - 1; iy++) {
			gradRectArr[ixx2 + iy] = { (ULONG)(ixx + iy), (ULONG)(ixx + ny2 + iy + 1) };
		}
	} // for ix

	pDC->GradientFill(vertexArr, nx2*ny2, gradRectArr, (ny2 - 1) * (nx2 - 1), GRADIENT_FILL_RECT_V);

	free1(colors);
	free1float(y2);
	free2float(amp2);
	free1float(ranges2);
	free1int(pltx);
	free1int(plty);
	free1(vertexArr);
	free1(gradRectArr);
	if (tStr.GetLength() > 0)
		AfxMessageBox(tStr, MB_ICONQUESTION);
	return TRUE;


	// testing
	//CPoint pp[2];
	//pp[0] = pcd->coord.mm_coord(xmin, ymin);
	//pp[1] = pcd->coord.mm_coord(xmax, ymax);
	//TRIVERTEX vertex[4];
	//vertex[0] = { pp[1].x/2 , pp[0].y , 0xff00,  0x6000, 0x0000, 0x0000 };
	////vertex[0].x = pp[1].x / 2;
	////vertex[0].y = pp[0].y;
	////vertex[0].Red = 0xff00;
	////vertex[0].Green = 0x8000;
	////vertex[0].Blue = 0x0000;
	////vertex[0].Alpha = 0x0000;
	//vertex[1] = { pp[0].x , pp[1].y / 2 , 0x0000,  0xa000, 0x0000, 0x0000 };
	//vertex[2] = { pp[1].x , pp[1].y / 2 , 0x9000,  0x0000, 0x9000, 0x0000 };
	//vertex[3] = { pp[1].x/2 , pp[1].y, 0x0000,  0x0000, 0xb000, 0x0000 };

	//// Create a GRADIENT_TRIANGLE structure that
	//// references the TRIVERTEX vertices.
	//GRADIENT_TRIANGLE gTriangle[2];
	//gTriangle[0] = { 0, 1, 2 };
	//gTriangle[1] = { 1, 2, 3 };
	////gTriangle.Vertex1 = 0;
	////gTriangle.Vertex2 = 1;
	////gTriangle.Vertex3 = 2;

	//// Draw a shaded triangle.
	//pDC->GradientFill(vertex, 4, gTriangle, 2, GRADIENT_FILL_TRIANGLE);


}


BOOL CDCHelper::tessellateFill2(CDC* pDC, int nx, int ny, float **ampArr, float *ranges,
	double xmin, double xmax, double ymin, double ymax,
	CColorHelper::AMPCOLORDYN *ampColorDyn2, CCoords *pcd, int memMaxMB)
{
	// will use: pcd->coord.ix1, ix2, iy1, iy2, x1, x2
	// xmin and xmax may be different from pcd->coord.X1() and pcd->coord.X2()
	// note: ranges[] should be monotonically increasing or decreasing, what about if not? 
	if (nx<2 || ny < 2) return FALSE;

	CString tStr;
	int ix, iy;
	// calculate how many rows and columns for the bitmap are needed: deviceAxes.Height() < 0 when displayed facing up

	// bitmaps are always generated with positive ncol and nrow
	double a;
	a = (xmax - xmin) / pcd->coord.xWidth() * pcd->coord.logicalAxes.Width() / (double)pcd->coord.device2Logical;
	int ncol = (int)ceil(a);
	a = (ymax - ymin) / pcd->coord.yHeight() * pcd->coord.logicalAxes.Height() / (double)pcd->coord.device2Logical;
	int nrow = (int)ceil(a);
	ncol = abs(ncol) + 1;
	nrow = abs(nrow) + 1;

	// one can either load an existing bitmap file or create an empty bitmap object
	CDib dib;

	// note: after this call, nrow or ncol can change
	float reduceRate = dib.Create24b(ncol, nrow);
	if (reduceRate == 0.f) return FALSE;
	dib.SetPalette(pDC);

	//################################
	// note:  draw.trcInt  did not include trace skipping. So we have to re-calculate the allowable trace span width during display
	float *dx = (float *)ealloc1float(nx);
	for (ix = 1; ix < nx; ix++) {
		dx[ix - 1] = (float)fabs(ranges[ix] - ranges[ix - 1]);
	}
	double traceSpanUse = 1. * getMedianValue(dx, nx - 1);	//This is the average trace interval in km.
	free1float(dx);
	double traceSpanMin = (xmax - xmin) * 0.005;
	// traceSpanMin guarantees that when zoomed up a lot, the colors span a larger interval
	// otherwise each trace can have gap strips which can be ugly
	if (traceSpanUse < traceSpanMin)
		traceSpanUse = traceSpanMin;
	// traceSpanUse cannot be too big as well, otherwise when there is a large data gap, the display will 
	// fill the gap with exceptionally visible data.
	//################################

	//################################
	double dxPixel = (xmax - xmin) / (double)(ncol - 1);  // km per pixel
	double dyPixel = (ymax - ymin) / (double)(nrow - 1); // seconds per pixel
	double dy = (ymax - ymin) / (double)(ny - 1);
	int numSamp2Skip = (int)(dyPixel / dy + 0.5); // if 0, no need to skip and we can use memcpy for the entire trace
												  //################################
												  // first, we produce a newer version of the ampArr** array to avoid too dense data
	float **amp2 = (float **)ealloc2float(ny, nx);
	int nyBytes = ny * sizeof(float);

	//int ny2 = 1 + (numSamp2Skip == 0 ? ny : ny / (numSamp2Skip + 1));
	int ny2;
	if (numSamp2Skip>0) {
		for (ix = 0; ix < nx; ix++) {
			ny2 = 0;
			for (iy = 0; iy < ny; iy += numSamp2Skip + 1) {
				amp2[ix][ny2] = ampArr[ix][iy];
				ny2++;
			}
		}
	}
	else {
		memcpy(&amp2[0][0], &ampArr[0][0], nyBytes*nx);
		ny2 = ny;
	}


	// now skip along X axis if traces are too dense
	int nyBytes2 = ny2 * sizeof(float);
	float *ranges2 = (float *)ealloc1float(nx);
	int ix2 = 0;
	ranges2[ix2] = ranges[0];
	memcpy(&amp2[ix2][0], &amp2[0][0], nyBytes2);
	ix2++;
	for (ix = 1; ix < nx; ix++) {
		if (fabs(ranges[ix] - ranges2[ix2 - 1]) < dxPixel) continue; // skip this trace ix
		memcpy(&amp2[ix2][0], &amp2[ix][0], nyBytes2);
		ranges2[ix2] = ranges[ix];
		ix2++; // this index is now accepted for plotting
	}
	int nx2 = ix2;
	//################################
	double dy2 = (ymax - ymin) / (double)(ny2 - 1);
	float *y2 = (float *)ealloc1float(ny2);
	y2[0] = (float)ymin;
	for (iy = 1; iy < ny2; iy++) {
		y2[iy] = y2[iy - 1] + (float)dy2;
	}
	//################################
	float **ampDerivative = (float **)ealloc2float(ny2, nx2);
	for (ix = 0; ix < nx2 - 1; ix++) {
		register double dx2 = ranges2[ix + 1] - ranges2[ix]; if (fabs(dx2) < MINAMP) dx2 = 1.;
		register double odx2 = 1. / dx2;
		for (iy = 0; iy < ny2; iy++) {
			ampDerivative[ix][iy] = (amp2[ix + 1][iy] - amp2[ix][iy]) * (float)odx2;
		}
	}
	memcpy(&ampDerivative[nx2 - 1][0], &ampDerivative[nx2 - 2][0], nyBytes2);
	//################################
	int *pltx = (int *)alloc1int(nx2);
	int *plty = (int *)alloc1int(ny2);
	for (ix = 0; ix < nx2; ix++) {
		pltx[ix] = pcd->coord.mm_coordx(ranges2[ix]);
	}
	for (iy = 0; iy < ny2; iy++) {
		plty[iy] = pcd->coord.mm_coordy(y2[iy]);
	}
	//################################
	COLORREF *colors = (COLORREF *)alloc1(ny2, sizeof(COLORREF));
	TRIVERTEX *vertexArr = (TRIVERTEX *)alloc1(nx2*ny2, sizeof(TRIVERTEX));
	GRADIENT_TRIANGLE *gradArr = (GRADIENT_TRIANGLE *)alloc1((nx2-1)*(ny2-1)* 2, sizeof(GRADIENT_TRIANGLE));
	for (ix = 0; ix < nx2; ix++) {
		ampColorDyn2->getColorRef4Amp(amp2[ix], colors, ny2);
		register int ixx = ix*ny2;
		for (iy = 0; iy < ny2; iy++) {
			vertexArr[iy + ixx] = { pltx[ix], plty[iy],
				CColorHelper::RVal16(colors[iy]),
				CColorHelper::GVal16(colors[iy]),
				CColorHelper::BVal16(colors[iy]),
				0x0000 };
		}
	}

	register bool isUpHill;
	for (ix = 0; ix < nx2 - 1; ix++) {
		register int ixx = ix*ny2;
		register int ixx2 = 2*ix*(ny2-1);
		for (iy = 0; iy < ny2 - 1; iy++) {
			if (ix > 0 && iy > 0) isUpHill =
				ampDerivative[ix - 1][iy] + ampDerivative[ix - 1][iy - 1] + ampDerivative[ix - 1][iy + 1] +
				ampDerivative[ix][iy] + ampDerivative[ix][iy - 1] + ampDerivative[ix][iy + 1] +
				ampDerivative[ix + 1][iy] + ampDerivative[ix + 1][iy - 1] + ampDerivative[ix + 1][iy + 1] > 0.;

			//if (ix > 0 && iy > 0) isUpHill =
			//	ampDerivative[ix - 1][iy] + ampDerivative[ix - 1][iy + 1] +
			//	ampDerivative[ix][iy] + ampDerivative[ix][iy + 1] +
			//	ampDerivative[ix+1][iy] + ampDerivative[ix+1][iy + 1] > 0.;
			else if (ix>0) isUpHill =
				ampDerivative[ix - 1][iy] + ampDerivative[ix][iy] > 0.;
			else isUpHill = ampDerivative[ix][iy] > 0.;
			//isUpHill = true;  // testing

			if (isUpHill) {
				// use up hill tesellation/pixel shader
				gradArr[ixx2 + iy * 2] = { (ULONG)(ixx + iy), (ULONG)(ixx + iy + 1), (ULONG)(ixx + ny2 + iy) };
				gradArr[ixx2 + iy * 2 + 1] = { (ULONG)(ixx + ny2 + iy), (ULONG)(ixx + iy + 1), (ULONG)(ixx + ny2 + iy + 1) };
			}
			else {
				// now use down hill tessellation/pixel shader
				gradArr[ixx2 + iy * 2] = { (ULONG)(ixx + iy), (ULONG)(ixx + iy + 1), (ULONG)(ixx + ny2 + iy + 1) };
				gradArr[ixx2 + iy * 2 + 1] = { (ULONG)(ixx + iy), (ULONG)(ixx + ny2 + iy + 1), (ULONG)(ixx + ny2 + iy) };
			}

			// a final fix for the 2 triangle: if trace span is too big, we will narrow it
			if (fabs(ranges2[ix + 1] - ranges2[ix]) > traceSpanUse) {
				gradArr[ixx2 + iy * 2] = { (ULONG)(ixx + iy), (ULONG)(ixx + iy + 1), (ULONG)(ixx + iy) };
				gradArr[ixx2 + iy * 2 + 1] = { (ULONG)(ixx + iy), (ULONG)(ixx + iy + 1), (ULONG)(ixx + iy + 1) };
			}

			// int dfs = 2 * ix*(ny2 - 1) + iy * 2 + 1;
		}
	} // for ix

	pDC->GradientFill(vertexArr, nx2*ny2, gradArr, (ny2 - 1) * (nx2 - 1) * 2, GRADIENT_FILL_TRIANGLE);

	free2float(ampDerivative);
	free1(colors);
	free1float(y2);
	free2float(amp2);
	free1float(ranges2);
	free1int(pltx);
	free1int(plty);
	free1(vertexArr);
	free1(gradArr);
	if (tStr.GetLength() > 0)
		AfxMessageBox(tStr, MB_ICONQUESTION);
	return TRUE;
}


BOOL CDCHelper::wiggleArr(CDC* pDC, int nx, int nt, float **xArr, float *rangeArr, float *yArr, CCoords *pcd)
{
	if (nt < 2) return FALSE;
	CPoint *plt = (CPoint *)alloc1CPoint(nt);
	CPen* oldPen;
	CPen blackPen(PS_SOLID, 0, BLACK);
	oldPen = pDC->SelectObject(&blackPen);

	double dx2Plot = pcd->coord.dxPerPixel() * 4.;
	double xPlotted = ERRORVAL;


	for (int i = 0; i < nx; i++) {
		if (fabs(rangeArr[i] - xPlotted) < dx2Plot) continue;
		xPlotted = rangeArr[i];
		pcd->coord.mm_coord(xArr[i], yArr, plt, nt);
		pDC->Polyline(plt, nt);
	} // for i

	pDC->SelectObject(oldPen);
	free1CPoint(plt);

	return TRUE;
}

void CDCHelper::airvarSmall(CDC* pDC, int n, double x[], double y[], double range, CCoords *pcd)
{
	if (n < 2) return;
	int numfa, i, j0, j1;
	double a, b, res = (double)pDC->GetDeviceCaps(HORZRES) / (double)pDC->GetWindowExt().cx;

	CPoint *pintAll, *pint, prange;
	pintAll = (CPoint *)alloc1CPoint(n);
	pint = (CPoint *)alloc1CPoint(2 * n);  //pint will use more than n points.
	if (pintAll == NULL || pint == NULL)
	{
		free1CPoint(pintAll);
		free1CPoint(pint);
		AfxMessageBox(_T("Insufficient memory available"));
		return;
	}

	bool isTime = pcd->coord.isTimeDomain() == TRUE;

	prange = pcd->coord.mm_coord(range, 0.);
	for (i = 0; i < n; i++)
		if (!_isnan(x[i]))
			pintAll[i] = pcd->coord.mm_coord(x[i], y[i]);
	CPen nullPen(PS_NULL, 0, WHITE);
	CBrush brush;
	brush.CreateSolidBrush(BLACK);
	CPen* oldPen;
	CBrush* oldBrush;
	//	int oldFill= pDC->SetPolyFillMode(ALTERNATE);
	oldPen = pDC->SelectObject(&nullPen);
	oldBrush = pDC->SelectObject(&brush);

	numfa = 0;
	j0 = (int)myRound((pintAll[0].x - prange.x)*res);
	if (j0 > 0)
	{
		pint[0] = pcd->coord.mm_coord(range, y[0]);
		pint[1] = pintAll[0];
		numfa += 2;
	}


	for (i = 1; i < n; i++) {
		j0 = (int)myRound((pintAll[i - 1].x - prange.x)*res);
		j1 = (int)myRound((pintAll[i].x - prange.x)*res);
		if (j1>0 && j0 > 0) {
			pint[numfa] = pintAll[i];
			numfa++;
		}
		else if (j1 > 0 && j0 <= 0) {
			a = x[i] - x[i - 1];
			if (fabs(a) < 0.0000001) b = y[i - 1];
			else
				b = y[i - 1] + (y[i] - y[i - 1])*(range - x[i - 1]) / a;
			if (b < y[i - 1]) b = y[i - 1];
			pint[numfa] = pcd->coord.mm_coord(range, b);
			pint[numfa + 1] = pintAll[i];
			numfa += 2;
		}
		else if (j1 == 0 && j0 > 0) {
			pint[numfa] = pintAll[i];
			pint[numfa + 1].x = prange.x;
			pint[numfa + 1].y = pintAll[i].y;
			numfa += 2;
			if (numfa > 2 && numfa < 100)
			{
				pDC->Polygon(pint, numfa);
			}
			numfa = 0;
		}
		else if (j1 < 0 && j0 >= 0) {
			a = x[i] - x[i - 1];
			if (fabs(a) < 0.0000001) b = y[i - 1];
			else
				b = y[i - 1] + (y[i] - y[i - 1])*(range - x[i - 1]) / a;
			if (b > y[i]) b = y[i];
			pint[numfa] = pcd->coord.mm_coord(range, b);
			numfa++;

			if (numfa>2 && numfa < 100) {
				pDC->Polygon(pint, numfa);
			}
			numfa = 0;
		}
		else numfa = 0;
	}

	pint[numfa] = pcd->coord.mm_coord(range, y[n - 1]);
	numfa++;
	if (numfa>2 && numfa < 100) pDC->Polygon(pint, numfa);

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	//	pDC->SetPolyFillMode(oldFill);
	free1CPoint(pintAll);
	free1CPoint(pint);

	nullPen.DeleteObject();
	brush.DeleteObject();
}


void CDCHelper::airCol(CDC* pDC, int n, double x[], double y[], double range, double offFill, 
	CCoords *pcd, CColorHelper::SEISBICOLOR *seisBiColor2)
{
	//note that offFill has to be passed into this routine, but not have to for airvar().
	//offFill is scaled by para.gainReal such that if offFill=1, only amplitudes spanning 
	//more than a trace interval get filled. Normally offfill=0.1.
	int numfa, i, j0, j1, pixelOffFill;
	double a, b, range1, res = (double)pDC->GetDeviceCaps(HORZRES) / (double)pDC->GetWindowExt().cx;
	CPen* oldPen;
	CBrush* oldBrush;

	CPoint *pintAll, *pint, prange, prange1;
	pintAll = (CPoint *)xmalloc(n*sizeof(CPoint));
	pint = (CPoint *)xmalloc(2 * n*sizeof(CPoint));  //pint will use more than n points.
	if (pintAll == NULL || pint == NULL) {
		AfxMessageBox(_T("Insufficient memory available"));
		return;
	}

	CPen nullPen(PS_NULL, 0, WHITE);
	CBrush brush, brushRed;
	brush.CreateSolidBrush(RGB(seisBiColor2->maxAmpR, seisBiColor2->maxAmpG, seisBiColor2->maxAmpB));
	brushRed.CreateSolidBrush(RGB(seisBiColor2->negAmpR, seisBiColor2->negAmpG, seisBiColor2->negAmpB));
	int oldFill = pDC->SetPolyFillMode(ALTERNATE);

	bool isTime = pcd->coord.isTimeDomain() == TRUE;

	for (i = 0; i < n; i++)
		if (!_isnan(x[i]))
			pintAll[i] = pcd->coord.mm_coord(x[i], y[i]);
	oldPen = pDC->SelectObject(&nullPen);

	//START FILL the left half.
	oldBrush = pDC->SelectObject(&brushRed);
	prange = pcd->coord.mm_coord(range, 0.);
	range1 = range - offFill; //real trace line.
	prange1 = pcd->coord.mm_coord(range1, 0.);
	pixelOffFill = (int)myRound((prange.x - prange1.x) * res); //no. of pixels to offset from range line.
	numfa = 0;

	j0 = (int)myRound((pintAll[0].x - prange.x)*res);
	if (j0 < -pixelOffFill) {
		pint[0] = pcd->coord.mm_coord(range1, y[0]);
		pint[1] = pintAll[0];
		numfa += 2;
	}

	for (i = 1; i<n; i++) {
		j0 = (int)myRound((pintAll[i - 1].x - prange.x)*res);
		j1 = (int)myRound((pintAll[i].x - prange.x)*res);
		if (j1>-pixelOffFill && j0 >= -pixelOffFill) continue; //do not fill anything.
		else if (j1 == -pixelOffFill && j0 >= -pixelOffFill)
		{ //this case is special for fill once: airColD1Small() does not need to consider this.
			//note that the shift to prange1.x is important: when pint[] is mapped to screen coordinates,
			//if VC++ finds two pint[] not identical, it'll fill something even though no pixel exists there.
			pint[numfa].y = pintAll[i].y;
			pint[numfa].x = prange1.x;
			numfa++;
		}
		else if (j1 < -pixelOffFill && j0 < -pixelOffFill) {
			pint[numfa] = pintAll[i];
			numfa++;
		}
		else if (j1 < -pixelOffFill && j0 >= -pixelOffFill) {
			a = x[i] - x[i - 1];
			if (fabs(a) < 0.0000001) b = y[i - 1];
			else
				b = y[i - 1] + (y[i] - y[i - 1])*(range1 - x[i - 1]) / a;
			if (b < y[i - 1]) b = y[i - 1];
			pint[numfa] = pcd->coord.mm_coord(range1, b);
			pint[numfa + 1] = pintAll[i];
			numfa += 2;
		}
		else if (j1 == -pixelOffFill && j0<-pixelOffFill) {
			pint[numfa] = pintAll[i];
			pint[numfa + 1].x = prange1.x;
			pint[numfa + 1].y = pintAll[i].y;
			numfa += 2;
		}
		else if (j1>-pixelOffFill && j0 < -pixelOffFill) {
			a = x[i] - x[i - 1];
			if (fabs(a) < 0.0000001) b = y[i - 1];
			else
				b = y[i - 1] + (y[i] - y[i - 1])*(range1 - x[i - 1]) / a;
			if (b > y[i]) b = y[i];
			pint[numfa] = pcd->coord.mm_coord(range1, b);
			numfa++;
		}
	}

	pint[numfa] = pcd->coord.mm_coord(range1, y[n - 1]);
	numfa++;
	if (numfa>2) pDC->Polygon(pint, numfa);
	pDC->SelectObject(oldBrush);

	//START FILL the right half.
	oldBrush = pDC->SelectObject(&brush);
	prange = pcd->coord.mm_coord(range, 0.);
	range1 = range + offFill; //real trace line.
	prange1 = pcd->coord.mm_coord(range1, 0.);
	pixelOffFill = (int)myRound((prange1.x - prange.x) * res); //no. of pixels to offset from range line.
	numfa = 0;

	j0 = (int)myRound((pintAll[0].x - prange.x)*res);
	if (j0 > pixelOffFill) {
		pint[0] = pcd->coord.mm_coord(range1, y[0]);
		pint[1] = pintAll[0];
		numfa += 2;
	}

	for (i = 1; i < n; i++) {
		j0 = (int)myRound((pintAll[i - 1].x - prange.x)*res);
		j1 = (int)myRound((pintAll[i].x - prange.x)*res);
		if (j1 < pixelOffFill && j0 <= pixelOffFill) continue; //do not fill anything.
		else if (j1 == pixelOffFill && j0 <= pixelOffFill)
		{ //this case is special for fill once: small fills do not need to consider this.
			pint[numfa].y = pintAll[i].y;
			pint[numfa].x = prange1.x;
			numfa++;
		}
		else if (j1 > pixelOffFill && j0 > pixelOffFill) {
			pint[numfa] = pintAll[i];
			numfa++;
		}
		else if (j1 > pixelOffFill && j0 <= pixelOffFill) {
			a = x[i] - x[i - 1];
			if (fabs(a) < 0.0000001) b = y[i - 1];
			else
				b = y[i - 1] + (y[i] - y[i - 1])*(range1 - x[i - 1]) / a;
			if (b < y[i - 1]) b = y[i - 1];
			pint[numfa] = pcd->coord.mm_coord(range1, b);
			pint[numfa + 1] = pintAll[i];
			numfa += 2;
		}
		else if (j1 == pixelOffFill && j0 > pixelOffFill) {
			pint[numfa] = pintAll[i];
			pint[numfa + 1].x = prange1.x;
			pint[numfa + 1].y = pintAll[i].y;
			numfa += 2;
		}
		else if (j1<pixelOffFill && j0>pixelOffFill) {
			a = x[i] - x[i - 1];
			if (fabs(a) < 0.0000001) b = y[i - 1];
			else
				b = y[i - 1] + (y[i] - y[i - 1])*(range1 - x[i - 1]) / a;
			if (b > y[i]) b = y[i];
			pint[numfa] = pcd->coord.mm_coord(range1, b);
			numfa++;
		}
	}

	pint[numfa] = pcd->coord.mm_coord(range1, y[n - 1]);
	numfa++;
	if (numfa > 2) pDC->Polygon(pint, numfa);
	pDC->SelectObject(oldBrush);

	pDC->SelectObject(oldPen);
	pDC->SetPolyFillMode(oldFill);

	xfree(pintAll);
	xfree(pint);
	nullPen.DeleteObject();
	brush.DeleteObject();
}


void CDCHelper::airCol1Small(CDC* pDC, int n, double x[], double y[], double range, 
	double offFill2, double trcInterval,
	CCoords *pcd, CColorHelper::SEISBICOLOR *seisBiColor2)
{
	
	// Note: please perform    trcInterval *= head.maxAmp / head.aveAmp*para.gain
	//  before calling this function.

	//note that offFill has to be passed into this routine, but not have to for airvar().
	//offFill is scaled by para.gainReal such that if offFill=1, only amplitudes spanning 
	//more than a trace interval get filled. Normally offfill=0.1.

	// note:  ampliutude  head.maxAmp  should be drawn using  
	//        RGB(seisBiColor.maxAmpR,seisBiColor.maxAmpG,seisBiColor.maxAmpB)
	// while the rest should be interpolated
	int numfa, i, j0, j1, pixelOffFill;
	double a, b, range1, res = (double)pDC->GetDeviceCaps(HORZRES) / (double)pDC->GetWindowExt().cx;

	CPen* oldPen;
	CBrush* oldBrush;
	bool isTime = pcd->coord.isTimeDomain() == TRUE;

	CPoint *pintAll, *pint, prange, prange1;
	pintAll = (CPoint *)alloc1CPoint(n);
	pint = (CPoint *)alloc1CPoint(2 * n);  //pint will use more than n points.
	if (pintAll == NULL || pint == NULL) {
		AfxMessageBox(_T("Insufficient memory available"));
		return;
	}

	int oldFill = pDC->SetPolyFillMode(ALTERNATE);
	for (i = 0; i < n; i++)
		if (!_isnan(x[i]))
			pintAll[i] = pcd->coord.mm_coord(x[i], y[i]);

	CPen varPen;

	//START FILL the left half.
	prange = pcd->coord.mm_coord(range, 0.);
	range1 = range - offFill2; //real trace line.
	prange1 = pcd->coord.mm_coord(range1, 0.);
	pixelOffFill = (int)myRound((prange.x - prange1.x) * res); //no. of pixels to offset from range line.
	numfa = 0;

	j0 = (int)myRound((pintAll[0].x - prange.x)*res);
	if (j0 < -pixelOffFill) {
		pint[0] = pcd->coord.mm_coord(range1, y[0]);
		pint[1] = pintAll[0];
		numfa += 2;
	}

	CBrush brushRed;
	brushRed.CreateSolidBrush(RGB(seisBiColor2->negAmpR, seisBiColor2->negAmpG,
		seisBiColor2->negAmpB));

	varPen.CreatePen(PS_NULL, 0, RGB(seisBiColor2->negAmpR, seisBiColor2->negAmpG, seisBiColor2->negAmpB));
	oldPen = pDC->SelectObject(&varPen);

	CPoint pZero = pcd->coord.mm_coord(range, 0);
	//		double trcInterval = (pcd->coord.isDistanceDisplay)  ?  head.distInterval  :  head.offsetInterval;
	CPoint pFull = pcd->coord.mm_coord(range - trcInterval, 0);
	for (i = 1; i < n; i++) {
		j0 = (int)myRound((pintAll[i - 1].x - prange.x)*res);
		j1 = (int)myRound((pintAll[i].x - prange.x)*res);
		if (j1 < -pixelOffFill && j0 < -pixelOffFill) {
			pint[numfa] = pintAll[i];
			numfa++;
		}
		else if (j1 < -pixelOffFill && j0 >= -pixelOffFill) {
			a = x[i] - x[i - 1];
			if (fabs(a) < 0.0000001) b = y[i - 1];
			else
				b = y[i - 1] + (y[i] - y[i - 1])*(range1 - x[i - 1]) / a;
			if (b < y[i - 1]) b = y[i - 1];
			pint[numfa] = pcd->coord.mm_coord(range1, b);
			pint[numfa + 1] = pintAll[i];
			numfa += 2;
		}
		else if (j1 == -pixelOffFill && j0 < -pixelOffFill) {
			pint[numfa] = pintAll[i];
			pint[numfa + 1].x = prange1.x;
			pint[numfa + 1].y = pintAll[i].y;
			numfa += 2;
			if (numfa>2 && numfa<100)
			{
				oldBrush = pDC->SelectObject(&brushRed);
				pDC->Polygon(pint, numfa);
				pDC->SelectObject(oldBrush);
			}
			numfa = 0;
		}
		else if (j1>-pixelOffFill && j0 < -pixelOffFill) {
			a = x[i] - x[i - 1];
			if (fabs(a) < 0.0000001) b = y[i - 1];
			else
				b = y[i - 1] + (y[i] - y[i - 1])*(range1 - x[i - 1]) / a;
			if (b > y[i]) b = y[i];
			pint[numfa] = pcd->coord.mm_coord(range1, b);
			numfa++;

			if (numfa>2 && numfa < 100) {
				oldBrush = pDC->SelectObject(&brushRed);
				pDC->Polygon(pint, numfa);
				pDC->SelectObject(oldBrush);
			}
			numfa = 0;
		}
		else numfa = 0;
	}

	pint[numfa] = pcd->coord.mm_coord(range1, y[n - 1]);
	numfa++;
	if (numfa>2 && numfa < 100) {
		oldBrush = pDC->SelectObject(&brushRed);
		pDC->Polygon(pint, numfa);
		pDC->SelectObject(oldBrush);
	}
	brushRed.DeleteObject();
	pDC->SelectObject(oldPen);
	varPen.DeleteObject(); // must delete before createPen again

	//START FILL the right half.
	CBrush brush;
	brush.CreateSolidBrush(RGB(seisBiColor2->maxAmpR, seisBiColor2->maxAmpG, seisBiColor2->maxAmpB));

	varPen.CreatePen(PS_NULL, 0, RGB(seisBiColor2->maxAmpR, seisBiColor2->maxAmpG, seisBiColor2->maxAmpB));
	oldPen = pDC->SelectObject(&varPen);

	prange = pcd->coord.mm_coord(range, 0.);
	range1 = range + offFill2; //real trace line.
	prange1 = pcd->coord.mm_coord(range1, 0.);
	pixelOffFill = (int)myRound((prange1.x - prange.x) * res); //no. of pixels to offset from range line.
	numfa = 0;

	j0 = (int)myRound((pintAll[0].x - prange.x)*res);
	if (j0 > pixelOffFill)
	{
		pint[0] = pcd->coord.mm_coord(range1, y[0]);
		pint[1] = pintAll[0];
		numfa += 2;
	}

	pFull = pcd->coord.mm_coord(range + trcInterval, 0);
	for (i = 1; i<n; i++) {
		j0 = (int)myRound((pintAll[i - 1].x - prange.x)*res);
		j1 = (int)myRound((pintAll[i].x - prange.x)*res);
		if (j1>pixelOffFill && j0 > pixelOffFill) {
			pint[numfa] = pintAll[i];
			numfa++;
		}
		else if (j1 > pixelOffFill && j0 <= pixelOffFill) {
			a = x[i] - x[i - 1];
			if (fabs(a) < 0.0000001) b = y[i - 1];
			else
				b = y[i - 1] + (y[i] - y[i - 1])*(range1 - x[i - 1]) / a;
			if (b < y[i - 1]) {
				b = y[i - 1];
			}
			pint[numfa] = pcd->coord.mm_coord(range1, b);
			pint[numfa + 1] = pintAll[i];
			numfa += 2;
		}
		else if (j1 == pixelOffFill && j0 > pixelOffFill) {
			pint[numfa] = pintAll[i];
			pint[numfa + 1].x = prange1.x;
			pint[numfa + 1].y = pintAll[i].y;
			numfa += 2;
			if (numfa > 2 && numfa < 100) {
				oldBrush = pDC->SelectObject(&brush);
				pDC->Polygon(pint, numfa);
				pDC->SelectObject(oldBrush);
			}
			numfa = 0;
		}
		else if (j1<pixelOffFill && j0>pixelOffFill) {
			a = x[i] - x[i - 1];
			if (fabs(a) < 0.0000001) b = y[i - 1];
			else
				b = y[i - 1] + (y[i] - y[i - 1])*(range1 - x[i - 1]) / a;
			if (b > y[i]) {
				b = y[i];
			}
			pint[numfa] = pcd->coord.mm_coord(range1, b);
			numfa++;

			if (numfa > 2 && numfa < 100) {
				oldBrush = pDC->SelectObject(&brush);
				pDC->Polygon(pint, numfa);
				pDC->SelectObject(oldBrush);
			}
			numfa = 0;
		}
		else numfa = 0;
	}

	pint[numfa] = pcd->coord.mm_coord(range1, y[n - 1]);
	numfa++;
	if (numfa>2 && numfa < 100) {
		oldBrush = pDC->SelectObject(&brush);
		pDC->Polygon(pint, numfa);
		pDC->SelectObject(oldBrush);
	}
	pDC->SetPolyFillMode(oldFill);
	brush.DeleteObject();

	pDC->SelectObject(oldPen);
	varPen.DeleteObject(); // must delete before createPen again

	free1CPoint(pintAll);
	free1CPoint(pint);
}


void CDCHelper::airColorSmallArr(CDC* pDC, int nx, int nt, float **xArr, float rangeArr[],
	double ft, double dt, COLORREF colPos, COLORREF colNeg, double offFill,
	CCoords *pcd)
{
	// NOTE:  offFill   is not used. Please fix!!!!
	if (nt < 2 || nx<2) return;
	double odt = 1. / dt;

	int numfa, ix, it;
	int j0, j1;
	double a, b, res = (double)pDC->GetDeviceCaps(HORZRES) / (double)pDC->GetWindowExt().cx;
	int da;
	bool isTime = pcd->coord.isTimeDomain() == TRUE;

	float *yArray = (float *)ealloc1float(nt);
	for (it = 0; it < nt; it++) {
		yArray[it] = (float)(ft + it * dt);
		if (yArray[it] >= pcd->coord.Y2()) {
			nt = it + 1; // reduce it and break
			break; 
		}
	}

	CPoint **pintAll = (CPoint **)alloc2CPoint(nt, nx);
	CPoint *pint = (CPoint *)alloc1CPoint(2 * nt);  //pint will use more than nt points.
	if (pintAll == NULL || pint == NULL) {
		return;
	}
	int *pRangeArr = (int *)ealloc1int(nx);
	bool *isUseArr = (bool *)ealloc1int(nx);
	CString tStr;

	if (pcd->coord.isSwapX) {
		// a problem when swapX is in use: this would cause problem in this routine since
		// it has many comparisons of small-large pixels along X-axis. 
		// So the best approach would be to start from large X to small X in advance
		float a;
		for (ix = 0; ix < nx / 2; ix++) {
			a = rangeArr[ix];
			rangeArr[ix] = rangeArr[nx - 1 - ix];
			rangeArr[nx - 1 - ix] = a;
			for (it = 0; it < nt; it++) {
				a = xArr[ix][it];
				xArr[ix][it] = xArr[nx - 1 - ix][it];
				xArr[nx - 1 - ix][it] = a;
			}
		}
	}
	// Fron now on, no need to consider swapX any more


	//for (ix = 0; ix < nx; ix++) {
	//	pRangeArr[ix] = pcd->coord.mm_coordx((double)rangeArr[ix]);
	//}
	pcd->coord.mm_coordx(rangeArr, pRangeArr, nx);

	for (ix = 0; ix < nx; ix++) {
		//for (it = 0; it < nt; it++)
		//	pintAll[ix][it] = pcd->coord.mm_coord(xArr[ix][it], yArray[it]);
		pcd->coord.mm_coord(xArr[ix], yArray, pintAll[ix], nt);
	}

	int tracePlottedInDisplay = 0;
	int traceSkippedInDisplay = 0;
	register CPoint pPrev, pCurr;
	pPrev.y = pCurr.y = 0;
	pPrev.x = pRangeArr[0];
	pDC->LPtoDP(&pPrev);
	for (ix = 0; ix < nx; ix++) {
		pCurr = pRangeArr[ix];
		pDC->LPtoDP(&pCurr);
		if (ix>0 && abs(pPrev.x - pCurr.x) < 1) {
			traceSkippedInDisplay++;
			isUseArr[ix] = false;
		}
		else {
			pPrev = pCurr;
			isUseArr[ix] = true;
			tracePlottedInDisplay++;
		}
	}

	// now we want to clip so that no shadings are overlapped.
	//int k0, k, k2, m0, m2;
	//int ix1, ix2;
	//for (ix = 0; ix < nx; ix++) {
	//	if (ix == 0) {
	//		ix1 = 0;
	//		ix2 = 1;
	//		m0 = pRangeArr[0];
	//		m2 = (pRangeArr[0] + pRangeArr[1]) / 2;
	//	}
	//	else if (ix == nx - 1) {
	//		ix1 = nx - 2;
	//		ix2 = nx - 1;
	//		m0 = (pRangeArr[nx - 1] + pRangeArr[nx - 2]) / 2;
	//		m2 = pRangeArr[nx - 1];
	//	}
	//	else {
	//		ix1 = ix - 1;
	//		while (!isUseArr[ix1]) {
	//			ix1--;
	//			if (ix1 <= 0) break;
	//		}
	//		if (ix1 < 0) ix1 = 0;

	//		ix2 = ix + 1;
	//		while (!isUseArr[ix2]) {
	//			ix2++;
	//			if (ix2 >= nx - 1) break;
	//		}
	//		if (ix2 > nx - 1) ix2 = nx - 1;

	//		m0 = (pRangeArr[ix] + pRangeArr[ix1]) / 2;
	//		m2 = (pRangeArr[ix] + pRangeArr[ix2]) / 2;
	//	}



	//	for (it = 0; it < nt; it++) {
	//		k = pintAll[ix][it].x;
	//		k0 = pintAll[ix1][it].x;
	//		k2 = pintAll[ix2][it].x;

	//		// check left side only
	//		if (ix > 0) {
	//			if (k < k0 &&  k0 < m0) {
	//				pintAll[ix][it].x = k0;

	//				// now the related problem: if the trace on the right side also has great negative amp
	//				if (ix<nx - 1 && k2 < k0)
	//					pintAll[ix2][it].x = k0;
	//				continue;
	//			}
	//		} // if ix

	//		// now check right side only
	//		if (ix < nx - 1) {
	//			if (ix<nx - 1 && k>k2 && k2 > m2) {
	//				pintAll[ix][it].x = k2;

	//				// now the related problem: if the trace on the left side also has great positive amp
	//				if (ix>0 && k0>k2)
	//					pintAll[ix1][it].x = k2;
	//				continue;
	//			}
	//		} // if ix
	//	} // for it
	//} // for ix

	CPen* oldPen;
	CBrush* oldBrush;

	CBrush brushNeg;
	brushNeg.CreateSolidBrush(colNeg);

	CPen penNeg;
	penNeg.CreatePen(PS_NULL, 0, colNeg);

	CBrush brushPos;
	brushPos.CreateSolidBrush(colPos);

	CPen penPos;
	penPos.CreatePen(PS_NULL, 0, colPos);

	//int pOffFill = (int)myRound(pcd->coord.mm_xscale*offFill); // this does not work well. So I disable it
	int oldFill = pDC->SetPolyFillMode(ALTERNATE);
	for (ix = 0; ix < nx; ix++) {
		//if (ix>0 && (int)(ix / 10) * 10 == ix) {
		//	tStr.Format(_T("R%d of %d traces."), ix, nx);
		//	pFrame->myCaption(tStr);
		//}
		if (!isUseArr[ix]) continue;

		//START FILL the left half.
		//prange = pcd->mm_coord(rangeArr[ix],0.);
		//int cFull = pcd->mm_coord((float)(rangeArr[ix]-locInKm4MaxAmp)); 

		if (GetRValue(colNeg) != 255 || GetGValue(colNeg) != 255 || GetBValue(colNeg) != 255) {
			//pRangeArr[ix] -= pOffFill;
			numfa = 0;
			if (pintAll[ix][0].x < pRangeArr[ix]) {
				// if here, the first sample of the trace has negative values, implying we must start from the first point
				// a problem: I found that the first point is not exactly at the pcd->coord.tmin, beacause of the trace delay not exactly line up at
				//         sample timing?
				pint[numfa].x = pRangeArr[ix];
				pint[numfa].y = pintAll[ix][0].y;
				numfa++;
				pint[numfa] = pintAll[ix][0];
				numfa++;
			}


			oldBrush = pDC->SelectObject(&brushNeg);
			oldPen = pDC->SelectObject(&penNeg);
			for (it = 1; it < nt-1; it++) {
				j0 = (int)myRound((pintAll[ix][it - 1].x - pRangeArr[ix])*res);
				j1 = (int)myRound((pintAll[ix][it].x - pRangeArr[ix])*res);
				if (j1 >= 0 && j0 >= 0) {
					numfa = 0;
					continue; //do not fill anything.
				}
				if (j1 < 0 && j0 < 0) {
					pint[numfa] = pintAll[ix][it];
					numfa++;
				}
				else if (j1 < 0 && j0 >= 0) {
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < MINAMP) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					pint[numfa + 1] = pintAll[ix][it];
					numfa += 2;
				}
				else if (j1 == 0 && j0 < 0) {
					pint[numfa] = pintAll[ix][it];
					pint[numfa + 1].x = pRangeArr[ix];
					pint[numfa + 1].y = pintAll[ix][it].y;
					numfa += 2;
					if (numfa>2)
					{
						da = getMinXValueCPoint(pint, numfa) - pRangeArr[ix];
						if (abs(da) > 1) {
							pDC->Polygon(pint, numfa);
						}
					}
					numfa = 0;
				}
				else if (j1 > 0 && j0 < 0) {
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < MINAMP) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					numfa++;

					if (numfa>2) {
						da = getMinXValueCPoint(pint, numfa) - pRangeArr[ix];
						if (abs(da) > 1) {
							pDC->Polygon(pint, numfa);
						}
					}
					numfa = 0;
				}
			} // for it

			if (numfa > 2) {
				// interpolate the last point at it = nt-1
				// need to calculate x value
				double x1 = xArr[ix][nt - 2];
				double x2 = xArr[ix][nt - 1];
				double y = pcd->coord.Y2();
				double x = x1 + (x2 - x1) * odt * (y - yArray[nt - 2]);
				pint[numfa] = pcd->coord.mm_coord(x, y);
				numfa++;

				// there are left overs. 
				pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], y);
				numfa++;

				da = getMinXValueCPoint(pint, numfa) - pRangeArr[ix];
				if (abs(da) > 1) {
					pDC->Polygon(pint, numfa);
				}
			}
			pDC->SelectObject(oldBrush);
			pDC->SelectObject(oldPen);
			//pRangeArr[ix] += pOffFill;
		} // if

		//START FILL the right half.
		if (GetRValue(colPos) != 255 || GetGValue(colPos) != 255 || GetBValue(colPos) != 255) {
			//pRangeArr[ix] += pOffFill;
			numfa = 0;
			if (pintAll[ix][0].x > pRangeArr[ix]) {
				// if here, the first sample of the trace has positive value, implying we must start from the first point
				pint[numfa].x = pRangeArr[ix];
				pint[numfa].y = pintAll[ix][0].y;
				numfa++;
				pint[numfa] = pintAll[ix][0];
				numfa++;
			}
			oldBrush = pDC->SelectObject(&brushPos);
			oldPen = pDC->SelectObject(&penPos);
			for (it = 1; it < nt-1; it++) {
				j0 = (int)myRound((pintAll[ix][it - 1].x - pRangeArr[ix])*res);
				j1 = (int)myRound((pintAll[ix][it].x - pRangeArr[ix])*res);
				if (j1 <= 0 && j0 <= 0) {
					numfa = 0;
					continue; //do not fill anything.
				}
				if (j1>0 && j0 > 0)
				{
					pint[numfa] = pintAll[ix][it];
					numfa++;
				}
				else if (j1 > 0 && j0 <= 0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < 0.0000001) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					pint[numfa + 1] = pintAll[ix][it];
					numfa += 2;
				}
				else if (j1 == 0 && j0 > 0)
				{
					pint[numfa] = pintAll[ix][it];
					pint[numfa + 1].x = pRangeArr[ix];
					pint[numfa + 1].y = pintAll[ix][it].y;
					numfa += 2;
					if (numfa > 2)
					{
						da = getMaxXValueCPoint(pint, numfa) - pRangeArr[ix];
						if (abs(da) > 1) {
							pDC->Polygon(pint, numfa);
						}
					}
					numfa = 0;
				}
				else if (j1 < 0 && j0>0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < 0.0000001) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					numfa++;

					if (numfa>2)
					{
						da = getMaxXValueCPoint(pint, numfa) - pRangeArr[ix];
						if (abs(da) > 1) {
							pDC->Polygon(pint, numfa);
						}
					}
					numfa = 0;
				}
			} // for it

			if (numfa > 2) {
				// interpolate the last point at it = nt-1
				// need to calculate x value
					double x1 = xArr[ix][nt - 2];
					double x2 = xArr[ix][nt - 1];
					double y = pcd->coord.Y2();
					double x = x1 + (x2 - x1) * odt * (y - yArray[nt - 2]);
					pint[numfa] = pcd->coord.mm_coord(x, y);
					numfa++;

					// there are left overs. 
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], y);
					numfa++;

				da = getMaxXValueCPoint(pint, numfa) - pRangeArr[ix];
				if (abs(da) > 1) {
					pDC->Polygon(pint, numfa);
				}
			}
			pDC->SelectObject(oldBrush);
			pDC->SelectObject(oldPen);
			//pRangeArr[ix] -= pOffFill;
		} // if

	} // for ix

	pDC->SetPolyFillMode(oldFill);
	penNeg.DeleteObject(); // must delete before createPen again
	brushNeg.DeleteObject();
	penPos.DeleteObject(); // must delete before createPen again
	brushPos.DeleteObject();

	free2CPoint(pintAll);
	free1CPoint(pint);
	free1int(pRangeArr);
	free1(isUseArr);
	free1float(yArray);
	//tStr.Format(_T("%d traces plotted while %d traces skipped."), draw.tracePlottedInDisplay, draw.traceSkippedInDisplay);
	//pFrame->myCaption(tStr);

}


void CDCHelper::airColorSmallArr(CDC* pDC, int nx, int nt, float **xArr, float rangeArr[],
	double ft, double dt, COLORREF colPos, COLORREF colNeg, double offFill,
	CCoords *pcd, bool isFFIDAbNormal[])
{
	if (nt < 2 || nx<2) return;
	double odt = 1. / dt;

	int numfa, ix, it;
	int j0, j1;
	double res = (double)pDC->GetDeviceCaps(HORZRES) / (double)pDC->GetWindowExt().cx;
	int da;
	bool isTime = pcd->coord.isTimeDomain() == TRUE;

	CPoint **pintAll = (CPoint **)alloc2CPoint(nt, nx);
	CPoint *pint = (CPoint *)alloc1CPoint(2 * nt);  //pint will use more than nt points.
	int *pRangeArr = (int *)ealloc1int(nx);
	bool *isUseArr = (bool *)ealloc1int(nx);
	float *yArray = (float *)ealloc1float(nt);
	CString tStr;
	if (pintAll == NULL || pint == NULL) {
		return;
	}
	for (it = 0; it < nt; it++)
		yArray[it] = (float)(ft + it * dt);

	if (pcd->coord.isSwapX) {
		// a problem when swapX is in use: this would cause problem in this routine since
		// it has many comparisons of small-large pixels along X-axis. 
		// So the best approach would be to start from large X to small X in advance
		float a;
		bool b;
		for (ix = 0; ix < nx / 2; ix++) {
			a = rangeArr[ix];
			rangeArr[ix] = rangeArr[nx - 1 - ix];
			rangeArr[nx - 1 - ix] = a;

			b = isFFIDAbNormal[ix];
			isFFIDAbNormal[ix] = isFFIDAbNormal[nx - 1 - ix];
			isFFIDAbNormal[nx - 1 - ix] = b;

			for (it = 0; it < nt; it++) {
				a = xArr[ix][it];
				xArr[ix][it] = xArr[nx - 1 - ix][it];
				xArr[nx - 1 - ix][it] = a;
			}
		}
	}
	// Fron now on, no need to consider swapX any more


	//for (ix = 0; ix < nx; ix++) {
	//	pRangeArr[ix] = pcd->coord.mm_coordx((double)rangeArr[ix]);
	//}
	pcd->coord.mm_coordx(rangeArr, pRangeArr, nx);

	for (ix = 0; ix < nx; ix++) {
		//for (it = 0; it < nt; it++)
		//	pintAll[ix][it] = pcd->coord.mm_coord(xArr[ix][it], yArray[it]);
		pcd->coord.mm_coord(xArr[ix], yArray, pintAll[ix], nt);
	}

	double a, b;
	int tracePlottedInDisplay = 0;
	int traceSkippedInDisplay = 0;
	register CPoint pPrev, pCurr;
	pPrev.y = pCurr.y = 0;
	pPrev.x = pRangeArr[0];
	pDC->LPtoDP(&pPrev);
	for (ix = 0; ix < nx; ix++) {
		pCurr = pRangeArr[ix];
		pDC->LPtoDP(&pCurr);
		if (ix>0 && abs(pPrev.x - pCurr.x) < 1) {
			traceSkippedInDisplay++;
			isUseArr[ix] = false;
		}
		else {
			pPrev = pCurr;
			isUseArr[ix] = true;
			tracePlottedInDisplay++;
		}
	}

	// now we want to clip so that no shadings are overlapped.
	//int k0, k, k2, m0, m2;
	//int ix1, ix2;
	//for (ix = 0; ix < nx; ix++) {
	//	if (ix == 0) {
	//		ix1 = 0;
	//		ix2 = 1;
	//		m0 = pRangeArr[0];
	//		m2 = (pRangeArr[0] + pRangeArr[1]) / 2;
	//	}
	//	else if (ix == nx - 1) {
	//		ix1 = nx - 2;
	//		ix2 = nx - 1;
	//		m0 = (pRangeArr[nx - 1] + pRangeArr[nx - 2]) / 2;
	//		m2 = pRangeArr[nx - 1];
	//	}
	//	else {
	//		ix1 = ix - 1;
	//		while (!isUseArr[ix1]) {
	//			ix1--;
	//			if (ix1 <= 0) break;
	//		}
	//		if (ix1 < 0) ix1 = 0;

	//		ix2 = ix + 1;
	//		while (!isUseArr[ix2]) {
	//			ix2++;
	//			if (ix2 >= nx - 1) break;
	//		}
	//		if (ix2 > nx - 1) ix2 = nx - 1;

	//		m0 = (pRangeArr[ix] + pRangeArr[ix1]) / 2;
	//		m2 = (pRangeArr[ix] + pRangeArr[ix2]) / 2;
	//	}



		//for (it = 0; it < nt; it++) {
		//	k = pintAll[ix][it].x;
		//	k0 = pintAll[ix1][it].x;
		//	k2 = pintAll[ix2][it].x;

		//	// check left side only
		//	if (ix > 0) {
		//		if (k < k0 &&  k0 < m0) {
		//			pintAll[ix][it].x = k0;

		//			// now the related problem: if the trace on the right side also has great negative amp
		//			if (ix<nx - 1 && k2 < k0)
		//				pintAll[ix2][it].x = k0;
		//			continue;
		//		}
		//	} // if ix

		//	  // now check right side only
		//	if (ix < nx - 1) {
		//		if (ix<nx - 1 && k>k2 && k2 > m2) {
		//			pintAll[ix][it].x = k2;

		//			// now the related problem: if the trace on the left side also has great positive amp
		//			if (ix>0 && k0>k2)
		//				pintAll[ix1][it].x = k2;
		//			continue;
		//		}
		//	} // if ix
		//} // for it
	//} // for ix

	  //	double locInKm4MaxAmp = head.aveAmp *2.0*draw.trcIntOavgAmp*para.gain / para.colorFillIllumination;
	  //double aMaxAmp = head.maxAmp/head.aveAmp *draw.trcInt*para.gain;
	  //double aAvgAmp = draw.trcInt*para.gain;

	CPen* oldPen;
	CBrush* oldBrush;

	CBrush brushNeg;
	brushNeg.CreateSolidBrush(colNeg);

	CPen penNeg;
	penNeg.CreatePen(PS_NULL, 0, colNeg);

	CBrush brushPos;
	brushPos.CreateSolidBrush(colPos);

	CPen penPos;
	penPos.CreatePen(PS_NULL, 0, colPos);

	CBrush brushBlack;
	brushBlack.CreateSolidBrush(BLACK);

	//int pOffFill = (int)myRound(pcd->coord.mm_xscale*offFill);
	int oldFill = pDC->SetPolyFillMode(ALTERNATE);
	for (ix = 0; ix < nx; ix++) {
		//if (ix>0 && (int)(ix / 10) * 10 == ix) {
		//	tStr.Format(_T("R%d of %d traces."), ix, nx);
		//	pFrame->myCaption(tStr);
		//}
		if (!isUseArr[ix]) continue;

		//START FILL the left half.
		//prange = pcd->mm_coord(rangeArr[ix],0.);
		//int cFull = pcd->mm_coord((float)(rangeArr[ix]-locInKm4MaxAmp)); 

		if (GetRValue(colNeg) != 255 || GetGValue(colNeg) != 255 || GetBValue(colNeg) != 255) {
			//pRangeArr[ix] -= pOffFill;
			numfa = 0;
			if (pintAll[ix][0].x < pRangeArr[ix]) {
				// if here, the first sample of the trace has negative values, implying we must start from the first point
				// a problem: I found that the first point is not exactly at the pcd->coord.tmin, beacause of the trace delay not exactly line up at
				//         sample timing?
				pint[numfa].x = pRangeArr[ix];
				pint[numfa].y = pintAll[ix][0].y;
				numfa++;
				pint[numfa] = pintAll[ix][0];
				numfa++;
			}


			oldBrush = pDC->SelectObject(&brushNeg);
			oldPen = pDC->SelectObject(&penNeg);
			for (it = 1; it < nt-1; it++) {
				j0 = (int)myRound((pintAll[ix][it - 1].x - pRangeArr[ix])*res);
				j1 = (int)myRound((pintAll[ix][it].x - pRangeArr[ix])*res);
				if (j1 >= 0 && j0 >= 0) {
					numfa = 0;
					continue; //do not fill anything.
				}
				if (j1 < 0 && j0 < 0) {
					pint[numfa] = pintAll[ix][it];
					numfa++;
				}
				else if (j1 < 0 && j0 >= 0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < MINAMP) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					pint[numfa + 1] = pintAll[ix][it];
					numfa += 2;
				}
				else if (j1 == 0 && j0 < 0)
				{
					pint[numfa] = pintAll[ix][it];
					pint[numfa + 1].x = pRangeArr[ix];
					pint[numfa + 1].y = pintAll[ix][it].y;
					numfa += 2;
					if (numfa>2)
					{
						da = getMinXValueCPoint(pint, numfa) - pRangeArr[ix];
						if (abs(da) > 1) {
							pDC->Polygon(pint, numfa);
						}
					}
					numfa = 0;
				}
				else if (j1 > 0 && j0 < 0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < MINAMP) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					numfa++;

					if (numfa>2)
					{
						da = getMinXValueCPoint(pint, numfa) - pRangeArr[ix];
						if (abs(da) > 1) {
							pDC->Polygon(pint, numfa);
						}
					}
					numfa = 0;
				}
			} // for it

			if (numfa > 2) {
				// interpolate the last point at it = nt-1
				// need to calculate x value
				double x1 = xArr[ix][nt - 2];
				double x2 = xArr[ix][nt - 1];
				double y = pcd->coord.Y2();
				double x = x1 + (x2 - x1) * odt * (y - yArray[nt - 2]);
				pint[numfa] = pcd->coord.mm_coord(x, y);
				numfa++;

				// there are left overs. 
				pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], y);
				numfa++;

				da = getMinXValueCPoint(pint, numfa) - pRangeArr[ix];
				if (abs(da) > 1) {
					pDC->Polygon(pint, numfa);
				}
			}
			pDC->SelectObject(oldBrush);
			pDC->SelectObject(oldPen);
			//pRangeArr[ix] += pOffFill;
		} // if

		  //START FILL the right half.
		if (GetRValue(colPos) != 255 || GetGValue(colPos) != 255 || GetBValue(colPos) != 255) {
			//pRangeArr[ix] += pOffFill;
			numfa = 0;
			if (pintAll[ix][0].x > pRangeArr[ix]) {
				// if here, the first sample of the trace has positive value, implying we must start from the first point
				pint[numfa].x = pRangeArr[ix];
				pint[numfa].y = pintAll[ix][0].y;
				numfa++;
				pint[numfa] = pintAll[ix][0];
				numfa++;
			}
			oldBrush = pDC->SelectObject(&brushPos);
			if (isFFIDAbNormal[ix])
				oldBrush = pDC->SelectObject(&brushBlack);
			else
				oldBrush = pDC->SelectObject(&brushPos);
			oldPen = pDC->SelectObject(&penPos);

			for (it = 1; it < nt-1; it++) {
				j0 = (int)myRound((pintAll[ix][it - 1].x - pRangeArr[ix])*res);
				j1 = (int)myRound((pintAll[ix][it].x - pRangeArr[ix])*res);
				if (j1 <= 0 && j0 <= 0) {
					numfa = 0;
					continue; //do not fill anything.
				}
				if (j1>0 && j0 > 0)
				{
					pint[numfa] = pintAll[ix][it];
					numfa++;
				}
				else if (j1 > 0 && j0 <= 0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < 0.0000001) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					pint[numfa + 1] = pintAll[ix][it];
					numfa += 2;
				}
				else if (j1 == 0 && j0 > 0)
				{
					pint[numfa] = pintAll[ix][it];
					pint[numfa + 1].x = pRangeArr[ix];
					pint[numfa + 1].y = pintAll[ix][it].y;
					numfa += 2;
					if (numfa > 2)
					{
						da = getMaxXValueCPoint(pint, numfa) - pRangeArr[ix];
						if (abs(da) > 1) {
							pDC->Polygon(pint, numfa);
						}
					}
					numfa = 0;
				}
				else if (j1 < 0 && j0>0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < 0.0000001) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					numfa++;

					if (numfa>2)
					{
						da = getMaxXValueCPoint(pint, numfa) - pRangeArr[ix];
						if (abs(da) > 1) {
							pDC->Polygon(pint, numfa);
						}
					}
					numfa = 0;
				}
			} // for it

			if (numfa > 2) {
				// interpolate the last point at it = nt-1
				// need to calculate x value
				double x1 = xArr[ix][nt - 2];
				double x2 = xArr[ix][nt - 1];
				double y = pcd->coord.Y2();
				double x = x1 + (x2 - x1) * odt * (y - yArray[nt - 2]);
				pint[numfa] = pcd->coord.mm_coord(x, y);
				numfa++;

				// there are left overs. 
				pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], y);
				numfa++;

				da = getMaxXValueCPoint(pint, numfa) - pRangeArr[ix];
				if (abs(da) > 1) {
					pDC->Polygon(pint, numfa);
				}
			}
			pDC->SelectObject(oldBrush);
			pDC->SelectObject(oldPen);
			//pRangeArr[ix] -= pOffFill;
		} // if

	} // for ix

	pDC->SetPolyFillMode(oldFill);
	penNeg.DeleteObject(); // must delete before createPen again
	brushNeg.DeleteObject();
	penPos.DeleteObject(); // must delete before createPen again
	brushPos.DeleteObject();

	free2CPoint(pintAll);
	free1CPoint(pint);
	free1int(pRangeArr);
	free1(isUseArr);
	free1float(yArray);
}

void CDCHelper::airColorSmallArr(CDC* pDC, int nx, int nt, float **xArr, float rangeArr[],
	double ymin, double dt, double offFill,
	CCoords *pcd, CColorHelper::SEISBICOLOR *seisBiColor2)
{
	COLORREF colPos = seisBiColor2->getColor(true); 
	COLORREF colNeg = seisBiColor2->getColor(false);
	airColorSmallArr(pDC, nx, nt, xArr, rangeArr, ymin, dt, colPos, colNeg, offFill, pcd);
}

void CDCHelper::airColorBigArr(CDC* pDC, int nx, int nt, float **xArr, float rangeArr[],
	double ft, double dt, COLORREF colPos, COLORREF colNeg, double offFill,
	CCoords *pcd)
{
	// NOTE:  offFill   is not used. Please fix!!!!
	if (nt < 2 || nx<2) return;
	double odt = 1. / dt;
	int numfa, ix, it;
	int j0, j1;
	double a, b, res = (double)pDC->GetDeviceCaps(HORZRES) / (double)pDC->GetWindowExt().cx;
	int da;
	bool isTime = pcd->coord.isTimeDomain() == TRUE;

	CPoint **pintAll = (CPoint **)alloc2CPoint(nt, nx);
	CPoint *pint = (CPoint *)alloc1CPoint(2 * nt);  //pint will use more than nt points.
	int *pRangeArr = (int *)ealloc1int(nx);
	bool *isUseArr = (bool *)ealloc1int(nx);
	float *yArray = (float *)ealloc1float(nt);
	CString tStr;
	if (pintAll == NULL || pint == NULL) {
		return;
	}

	for (it = 0; it < nt; it++)
		yArray[it] = (float)(ft + it*dt);

	if (pcd->coord.isSwapX) {
		// a problem when swapX is in use: this would cause problem in this routine since
		// it has many comparisons of small-large pixels along X-axis. 
		// So the best approach would be to start from large X to small X in advance
		float a;
		for (ix = 0; ix < nx / 2; ix++) {
			a = rangeArr[ix];
			rangeArr[ix] = rangeArr[nx - 1 - ix];
			rangeArr[nx - 1 - ix] = a;
			for (it = 0; it < nt; it++) {
				a = xArr[ix][it];
				xArr[ix][it] = xArr[nx - 1 - ix][it];
				xArr[nx - 1 - ix][it] = a;
			}
		}
	}
	// Fron now on, no need to consider swapX any more

	pcd->coord.mm_coordx(rangeArr, pRangeArr, nx);
	for (ix = 0; ix < nx; ix++) {
		// pRangeArr[ix] = pcd->coord.mm_coordx((double)rangeArr[ix]);
		//for (it = 0; it < nt; it++)
		//	pintAll[ix][it] = pcd->coord.mm_coord((double)xArr[ix][it], yArray[it]);
		pcd->coord.mm_coord(xArr[ix], yArray, pintAll[ix], nt);
	}

	int tracePlottedInDisplay = 0;
	int traceSkippedInDisplay = 0;
	register CPoint pPrev, pCurr;
	pPrev.y = pCurr.y = 0;
	pPrev.x = pRangeArr[0];
	pDC->LPtoDP(&pPrev);
	for (ix = 0; ix < nx; ix++) {
		pCurr = pRangeArr[ix];
		pDC->LPtoDP(&pCurr);
		if (ix>0 && abs(pPrev.x - pCurr.x) < 1) {
			traceSkippedInDisplay++;
			isUseArr[ix] = false;
		}
		else {
			pPrev = pCurr;
			isUseArr[ix] = true;
			tracePlottedInDisplay++;
		}
	}

	// now we want to clip so that no shadings are overlapped.
	int k0, k, k2, m0, m2;
	int ix1, ix2;
	for (ix = 0; ix < nx; ix++) {
		if (ix == 0) {
			ix1 = 0;
			ix2 = 1;
			m0 = pRangeArr[0];
			m2 = (pRangeArr[0] + pRangeArr[1]) / 2;
		}
		else if (ix == nx - 1) {
			ix1 = nx - 2;
			ix2 = nx - 1;
			m0 = (pRangeArr[nx - 1] + pRangeArr[nx - 2]) / 2;
			m2 = pRangeArr[nx - 1];
		}
		else {
			ix1 = ix - 1;
			while (!isUseArr[ix1]) {
				ix1--;
				if (ix1 <= 0) break;
			}
			if (ix1 < 0) ix1 = 0;

			ix2 = ix + 1;
			while (!isUseArr[ix2]) {
				ix2++;
				if (ix2 >= nx - 1) break;
			}
			if (ix2 > nx - 1) ix2 = nx - 1;

			m0 = (pRangeArr[ix] + pRangeArr[ix1]) / 2;
			m2 = (pRangeArr[ix] + pRangeArr[ix2]) / 2;
		}


		for (it = 0; it < nt; it++) {
			k = pintAll[ix][it].x;
			k0 = pintAll[ix1][it].x;
			k2 = pintAll[ix2][it].x;

			// check left side only
			if (ix > 0) {
				if (k < k0 &&  k0 < m0) {
					pintAll[ix][it].x = k0;

					// now the related problem: if the trace on the right side also has great negative amp
					if (ix<nx - 1 && k2 < k0)
						pintAll[ix2][it].x = k0;
					continue;
				}
			} // if ix

			// now check right side only
			if (ix < nx - 1) {
				if (ix<nx - 1 && k>k2 && k2 > m2) {
					pintAll[ix][it].x = k2;

					// now the related problem: if the trace on the left side also has great positive amp
					if (ix>0 && k0>k2)
						pintAll[ix1][it].x = k2;
					continue;
				}
			} // if ix
		} // for it
	} // for ix

	//	double locInKm4MaxAmp = head.aveAmp *2.0*draw.trcIntOavgAmp*para.gain / para.colorFillIllumination;
	//double aMaxAmp = head.maxAmp/head.aveAmp *draw.trcInt*para.gain;
	//double aAvgAmp = draw.trcInt*para.gain;

	CPen* oldPen;
	CBrush* oldBrush;
	CBrush brushNeg;
	brushNeg.CreateSolidBrush(colNeg);

	CPen penNeg;
	penNeg.CreatePen(PS_NULL, 0, colNeg);

	CBrush brushPos;
	brushPos.CreateSolidBrush(colPos);

	CPen penPos;
	penPos.CreatePen(PS_NULL, 0, colPos);

	//int pOffFill = (int)myRound(pcd->coord.mm_xscale*offFill);
	for (ix = 0; ix < nx; ix++) {
		//if (ix>0 && (int)(ix / 10) * 10 == ix) {
		//	tStr.Format(_T("R%d of %d traces."), ix, nx);
		//	pFrame->myCaption(tStr);
		//}
		if (!isUseArr[ix]) continue;

		//START FILL the left half.
		if (GetRValue(colNeg) != 255 || GetGValue(colNeg) != 255 || GetBValue(colNeg) != 255) {
			//pRangeArr[ix] -= pOffFill;
			numfa = 0;
			if (pintAll[ix][0].x < pRangeArr[ix]) {
				// if here, the first sample of the trace has negative values, implying we must start from the first point
				// a problem: I found that the first point is not exactly at the pcd->coord.tmin, beacause of the trace delay not exactly line up at
				//         sample timing?
				pint[numfa].x = pRangeArr[ix];
				pint[numfa].y = pintAll[ix][0].y;
				numfa++;
				pint[numfa] = pintAll[ix][0];
				numfa++;
			}


			for (it = 1; it < nt-1; it++) {
				j0 = (int)myRound((pintAll[ix][it - 1].x - pRangeArr[ix])*res);
				j1 = (int)myRound((pintAll[ix][it].x - pRangeArr[ix])*res);
				if (j1 >= 0 && j0 >= 0) {
					pint[numfa].x = pRangeArr[ix];
					pint[numfa].y = pintAll[ix][it].y;
					numfa++;
					continue;
				}
				if (j1 < 0 && j0 < 0) {
					pint[numfa] = pintAll[ix][it];
					numfa++;
				}
				else if (j1 < 0 && j0 >= 0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < MINAMP) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					pint[numfa + 1] = pintAll[ix][it];
					numfa += 2;
				}
				else if (j1 == 0 && j0 < 0)
				{
					pint[numfa] = pintAll[ix][it];
					pint[numfa + 1].x = pRangeArr[ix];
					pint[numfa + 1].y = pintAll[ix][it].y;
					numfa += 2;
				}
				else if (j1>0 && j0 < 0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < MINAMP) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					numfa++;
				}
			} // for it

			double x1 = xArr[ix][nt - 2];
			double x2 = xArr[ix][nt - 1];
			double y = pcd->coord.Y2();
			double x = x1 + (x2 - x1) / dt * (y - yArray[nt - 2]);
			pint[numfa] = pcd->coord.mm_coord(x, y);
			numfa++;

			// there are left overs. 
			pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], y);
			numfa++;

			da = getMinXValueCPoint(pint, numfa) - pRangeArr[ix];
			if (abs(da) > 1) {
				int oldFill = pDC->SetPolyFillMode(ALTERNATE);
				oldBrush = pDC->SelectObject(&brushNeg);
				oldPen = pDC->SelectObject(&penNeg);
				pDC->Polygon(pint, numfa);
				pDC->SelectObject(oldBrush);
				pDC->SelectObject(oldPen);
				pDC->SetPolyFillMode(oldFill);
			}
			//pRangeArr[ix] += pOffFill;
		} // if

		//START FILL the right half.
		if (GetRValue(colPos) != 255 || GetGValue(colPos) != 255 || GetBValue(colPos) != 255) {
			//pRangeArr[ix] += pOffFill;
			numfa = 0;
			if (pintAll[ix][0].x > pRangeArr[ix]) {
				// if here, the first sample of the trace has positive value, implying we must start from the first point
				pint[numfa].x = pRangeArr[ix];
				pint[numfa].y = pintAll[ix][0].y;
				numfa++;
				pint[numfa] = pintAll[ix][0];
				numfa++;
			}
			for (it = 1; it < nt-1; it++) {
				j0 = (int)myRound((pintAll[ix][it - 1].x - pRangeArr[ix])*res);
				j1 = (int)myRound((pintAll[ix][it].x - pRangeArr[ix])*res);
				if (j1 <= 0 && j0 <= 0) {
					pint[numfa].x = pRangeArr[ix];
					pint[numfa].y = pintAll[ix][it].y;
					numfa++;
					continue;
				}
				if (j1>0 && j0 > 0)
				{
					pint[numfa] = pintAll[ix][it];
					numfa++;
				}
				else if (j1 > 0 && j0 <= 0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < 0.0000001) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					pint[numfa + 1] = pintAll[ix][it];
					numfa += 2;
				}
				else if (j1 == 0 && j0 > 0)
				{
					pint[numfa] = pintAll[ix][it];
					pint[numfa + 1].x = pRangeArr[ix];
					pint[numfa + 1].y = pintAll[ix][it].y;
					numfa += 2;
				}
				else if (j1 < 0 && j0>0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < 0.0000001) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					numfa++;
				}
			} // for it

			double x1 = xArr[ix][nt - 2];
			double x2 = xArr[ix][nt - 1];
			double y = pcd->coord.Y2();
			double x = x1 + (x2 - x1) / dt * (y - yArray[nt - 2]);
			pint[numfa] = pcd->coord.mm_coord(x, y);
			numfa++;

			// there are left overs. 
			pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], y);
			numfa++;

			da = getMaxXValueCPoint(pint, numfa) - pRangeArr[ix];
			if (abs(da) > 1) {
				int oldFill = pDC->SetPolyFillMode(ALTERNATE);
				oldBrush = pDC->SelectObject(&brushPos);
				oldPen = pDC->SelectObject(&penPos);
				pDC->Polygon(pint, numfa);
				pDC->SelectObject(oldBrush);
				pDC->SelectObject(oldPen);
				pDC->SetPolyFillMode(oldFill);
			}
			//pRangeArr[ix] -= pOffFill;
		} // if

	} // for ix

	penNeg.DeleteObject(); // must delete before createPen again
	brushNeg.DeleteObject();
	penPos.DeleteObject(); // must delete before createPen again
	brushPos.DeleteObject();

	free2CPoint(pintAll);
	free1CPoint(pint);
	free1int(pRangeArr);
	free1(isUseArr);
	free1float(yArray);
	//tStr.Format(_T("%d traces plotted while %d traces skipped."), draw.tracePlottedInDisplay, draw.traceSkippedInDisplay);
	//pFrame->myCaption(tStr);
}



void CDCHelper::airColorBigArr(CDC* pDC, int nx, int nt, float **xArr, float rangeArr[],
	double ymin, double dt, double offFill,
	CCoords *pcd, CColorHelper::SEISBICOLOR *seisBiColor2)
{
	COLORREF colPos = seisBiColor2->getColor(true);
	COLORREF colNeg = seisBiColor2->getColor(false);
	airColorBigArr(pDC, nx, nt, xArr, rangeArr, ymin, dt, colPos, colNeg, offFill, pcd);
}


void CDCHelper::airColorBigArr(CDC* pDC, int nx, int nt, float **xArr, float rangeArr[],
	double ft, double dt, COLORREF colPos, COLORREF colNeg, double offFill,
	CCoords *pcd, bool isFFIDAbNormal[])
{
	// NOTE:  offFill   is not used. Please fix!!!!
	if (nt < 2 || nx<2) return;
	double odt = 1. / dt;
	int numfa, ix, it;
	int j0, j1;
	double res = (double)pDC->GetDeviceCaps(HORZRES) / (double)pDC->GetWindowExt().cx;
	int da;
	bool isTime = pcd->coord.isTimeDomain() == TRUE;

	CPoint **pintAll = (CPoint **)alloc2CPoint(nt, nx);
	CPoint *pint = (CPoint *)alloc1CPoint(2 * nt);  //pint will use more than nt points.
	int *pRangeArr = (int *)ealloc1int(nx);
	bool *isUseArr = (bool *)ealloc1int(nx);
	float *yArray = (float *)ealloc1float(nt);
	CString tStr;
	if (pintAll == NULL || pint == NULL) {
		return;
	}

	for (it = 0; it < nt; it++)
		yArray[it] = (float)(ft + it * dt);

	if (pcd->coord.isSwapX) {
		// a problem when swapX is in use: this would cause problem in this routine since
		// it has many comparisons of small-large pixels along X-axis. 
		// So the best approach would be to start from large X to small X in advance
		float a;
		bool b;
		for (ix = 0; ix < nx / 2; ix++) {
			a = rangeArr[ix];
			rangeArr[ix] = rangeArr[nx - 1 - ix];
			rangeArr[nx - 1 - ix] = a;

			b = isFFIDAbNormal[ix];
			isFFIDAbNormal[ix] = isFFIDAbNormal[nx - 1 - ix];
			isFFIDAbNormal[nx - 1 - ix] = b;

			for (it = 0; it < nt; it++) {
				a = xArr[ix][it];
				xArr[ix][it] = xArr[nx - 1 - ix][it];
				xArr[nx - 1 - ix][it] = a;
			}
		}
	}
	// Fron now on, no need to consider swapX any more

	for (ix = 0; ix < nx; ix++) {
		pRangeArr[ix] = pcd->coord.mm_coordx((double)rangeArr[ix]);
		for (it = 0; it < nt; it++)
			pintAll[ix][it] = pcd->coord.mm_coord((double)xArr[ix][it], yArray[it]);
	}

	int tracePlottedInDisplay = 0;
	int traceSkippedInDisplay = 0;
	register CPoint pPrev, pCurr;
	pPrev.y = pCurr.y = 0;
	pPrev.x = pRangeArr[0];
	pDC->LPtoDP(&pPrev);
	for (ix = 0; ix < nx; ix++) {
		pCurr = pRangeArr[ix];
		pDC->LPtoDP(&pCurr);
		if (ix>0 && abs(pPrev.x - pCurr.x) < 1) {
			traceSkippedInDisplay++;
			isUseArr[ix] = false;
		}
		else {
			pPrev = pCurr;
			isUseArr[ix] = true;
			tracePlottedInDisplay++;
		}
	}

	// now we want to clip so that no shadings are overlapped.
	int k0, k, k2, m0, m2;
	int ix1, ix2;
	double a, b;
	for (ix = 0; ix < nx; ix++) {
		if (ix == 0) {
			ix1 = 0;
			ix2 = 1;
			m0 = pRangeArr[0];
			m2 = (pRangeArr[0] + pRangeArr[1]) / 2;
		}
		else if (ix == nx - 1) {
			ix1 = nx - 2;
			ix2 = nx - 1;
			m0 = (pRangeArr[nx - 1] + pRangeArr[nx - 2]) / 2;
			m2 = pRangeArr[nx - 1];
		}
		else {
			ix1 = ix - 1;
			while (!isUseArr[ix1]) {
				ix1--;
				if (ix1 <= 0) break;
			}
			if (ix1 < 0) ix1 = 0;

			ix2 = ix + 1;
			while (!isUseArr[ix2]) {
				ix2++;
				if (ix2 >= nx - 1) break;
			}
			if (ix2 > nx - 1) ix2 = nx - 1;

			m0 = (pRangeArr[ix] + pRangeArr[ix1]) / 2;
			m2 = (pRangeArr[ix] + pRangeArr[ix2]) / 2;
		}


		for (it = 0; it < nt; it++) {
			k = pintAll[ix][it].x;
			k0 = pintAll[ix1][it].x;
			k2 = pintAll[ix2][it].x;

			// check left side only
			if (ix > 0) {
				if (k < k0 &&  k0 < m0) {
					pintAll[ix][it].x = k0;

					// now the related problem: if the trace on the right side also has great negative amp
					if (ix<nx - 1 && k2 < k0)
						pintAll[ix2][it].x = k0;
					continue;
				}
			} // if ix

			  // now check right side only
			if (ix < nx - 1) {
				if (ix<nx - 1 && k>k2 && k2 > m2) {
					pintAll[ix][it].x = k2;

					// now the related problem: if the trace on the left side also has great positive amp
					if (ix>0 && k0>k2)
						pintAll[ix1][it].x = k2;
					continue;
				}
			} // if ix
		} // for it
	} // for ix

	  //	double locInKm4MaxAmp = head.aveAmp *2.0*draw.trcIntOavgAmp*para.gain / para.colorFillIllumination;
	  //double aMaxAmp = head.maxAmp/head.aveAmp *draw.trcInt*para.gain;
	  //double aAvgAmp = draw.trcInt*para.gain;

	CPen* oldPen;
	CBrush* oldBrush;
	CBrush brushNeg;
	brushNeg.CreateSolidBrush(colNeg);

	CPen penNeg;
	penNeg.CreatePen(PS_NULL, 0, colNeg);

	CBrush brushPos;
	brushPos.CreateSolidBrush(colPos);

	CPen penPos;
	penPos.CreatePen(PS_NULL, 0, colPos);

	CBrush brushBlack;
	brushBlack.CreateSolidBrush(BLACK);

	int pOffFill = (int)myRound(pcd->coord.mm_xscale*offFill);
	for (ix = 0; ix < nx; ix++) {
		//if (ix>0 && (int)(ix / 10) * 10 == ix) {
		//	tStr.Format(_T("R%d of %d traces."), ix, nx);
		//	pFrame->myCaption(tStr);
		//}
		if (!isUseArr[ix]) continue;

		//START FILL the left half.
		if (GetRValue(colNeg) != 255 || GetGValue(colNeg) != 255 || GetBValue(colNeg) != 255) {
			pRangeArr[ix] -= pOffFill;
			numfa = 0;
			if (pintAll[ix][0].x < pRangeArr[ix]) {
				// if here, the first sample of the trace has negative values, implying we must start from the first point
				// a problem: I found that the first point is not exactly at the pcd->coord.tmin, beacause of the trace delay not exactly line up at
				//         sample timing?
				pint[numfa].x = pRangeArr[ix];
				pint[numfa].y = pintAll[ix][0].y;
				numfa++;
				pint[numfa] = pintAll[ix][0];
				numfa++;
			}


			for (it = 1; it < nt; it++) {
				j0 = (int)myRound((pintAll[ix][it - 1].x - pRangeArr[ix])*res);
				j1 = (int)myRound((pintAll[ix][it].x - pRangeArr[ix])*res);
				if (j1 >= 0 && j0 >= 0) {
					pint[numfa].x = pRangeArr[ix];
					pint[numfa].y = pintAll[ix][it].y;
					numfa++;
					continue;
				}
				if (j1 < 0 && j0 < 0) {
					pint[numfa] = pintAll[ix][it];
					numfa++;
				}
				else if (j1 < 0 && j0 >= 0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < MINAMP) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					pint[numfa + 1] = pintAll[ix][it];
					numfa += 2;
				}
				else if (j1 == 0 && j0 < 0)
				{
					pint[numfa] = pintAll[ix][it];
					pint[numfa + 1].x = pRangeArr[ix];
					pint[numfa + 1].y = pintAll[ix][it].y;
					numfa += 2;
				}
				else if (j1>0 && j0 < 0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < MINAMP) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					numfa++;
				}
			} // for it

			pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], yArray[nt - 1]);
			numfa++;
			da = getMinXValueCPoint(pint, numfa) - pRangeArr[ix];
			if (abs(da) > 1) {
				int oldFill = pDC->SetPolyFillMode(ALTERNATE);
				oldBrush = pDC->SelectObject(&brushNeg);
				oldPen = pDC->SelectObject(&penNeg);
				pDC->Polygon(pint, numfa);
				pDC->SelectObject(oldBrush);
				pDC->SelectObject(oldPen);
				pDC->SetPolyFillMode(oldFill);
			}
			pRangeArr[ix] += pOffFill;
		} // if

		  //START FILL the right half.
		if (GetRValue(colPos) != 255 || GetGValue(colPos) != 255 || GetBValue(colPos) != 255) {
			pRangeArr[ix] += pOffFill;
			numfa = 0;
			if (pintAll[ix][0].x > pRangeArr[ix]) {
				// if here, the first sample of the trace has positive value, implying we must start from the first point
				pint[numfa].x = pRangeArr[ix];
				pint[numfa].y = pintAll[ix][0].y;
				numfa++;
				pint[numfa] = pintAll[ix][0];
				numfa++;
			}
			for (it = 1; it < nt; it++) {
				j0 = (int)myRound((pintAll[ix][it - 1].x - pRangeArr[ix])*res);
				j1 = (int)myRound((pintAll[ix][it].x - pRangeArr[ix])*res);
				if (j1 <= 0 && j0 <= 0) {
					pint[numfa].x = pRangeArr[ix];
					pint[numfa].y = pintAll[ix][it].y;
					numfa++;
					continue;
				}
				if (j1>0 && j0 > 0)
				{
					pint[numfa] = pintAll[ix][it];
					numfa++;
				}
				else if (j1 > 0 && j0 <= 0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < 0.0000001) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					pint[numfa + 1] = pintAll[ix][it];
					numfa += 2;
				}
				else if (j1 == 0 && j0 > 0)
				{
					pint[numfa] = pintAll[ix][it];
					pint[numfa + 1].x = pRangeArr[ix];
					pint[numfa + 1].y = pintAll[ix][it].y;
					numfa += 2;
				}
				else if (j1 < 0 && j0>0)
				{
					a = xArr[ix][it] - xArr[ix][it - 1];
					if (fabs(a) < 0.0000001) b = yArray[it - 1];
					else
						b = yArray[it - 1] + (yArray[it] - yArray[it - 1])*(rangeArr[ix] - xArr[ix][it - 1]) / a;
					if (b > yArray[it]) b = yArray[it];
					else if (b < yArray[it - 1]) b = yArray[it - 1];
					pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], b);
					numfa++;
				}
			} // for it

			pint[numfa] = pcd->coord.mm_coord(rangeArr[ix], yArray[nt - 1]);
			numfa++;
			da = getMaxXValueCPoint(pint, numfa) - pRangeArr[ix];
			if (abs(da) > 1) {
				int oldFill = pDC->SetPolyFillMode(ALTERNATE);
				oldBrush = pDC->SelectObject(&brushPos);
				if (isFFIDAbNormal[ix])
					oldBrush = pDC->SelectObject(&brushBlack);
				else
					oldBrush = pDC->SelectObject(&brushPos);
				oldPen = pDC->SelectObject(&penPos);

				pDC->Polygon(pint, numfa);
				pDC->SelectObject(oldBrush);
				pDC->SelectObject(oldPen);
				pDC->SetPolyFillMode(oldFill);
			}
			pRangeArr[ix] -= pOffFill;
		} // if

	} // for ix

	penNeg.DeleteObject(); // must delete before createPen again
	brushNeg.DeleteObject();
	penPos.DeleteObject(); // must delete before createPen again
	brushPos.DeleteObject();

	free2CPoint(pintAll);
	free1CPoint(pint);
	free1int(pRangeArr);
	free1(isUseArr);
	free1float(yArray);
	//tStr.Format(_T("%d traces plotted while %d traces skipped."), draw.tracePlottedInDisplay, draw.traceSkippedInDisplay);
	//pFrame->myCaption(tStr);
}


// This will transform the display *fromDC  to  *toDC, allowing for changes in reducing velocity fromRed to  toRed.
// A proper sourceat xAtSource  must be considered, assuming the coord mapping has not changed so the same *pCoord can be used
// This will also consider swapping up-down and left-right
void CDCHelper::transformCDC(CDC *fromDC, CRect *fromRect, double fromYmin, CCoords::REDUCE fromRed,
	CDC *toDC, CCoords::REDUCE toRed,
	CCoords::SEI_COORD *pCoord, double xAtSource)
{
	CPoint cps[3];
	CBitmap bm;
	ZeroMemory(&bm, sizeof(bm));
	double x1;
	double x2;

	// 1. Plot data to the left side of the source
	double dx = pCoord->xmax - pCoord->xmin;
	if (fabs(dx) < MINAMP) return;
	double odx = 1. / dx;
	double perc;
	if (xAtSource > pCoord->xmin) {
		x2 = (xAtSource > pCoord->xmax) ? pCoord->xmax : xAtSource;
		//perc = (x2 - pCoord->xmin) / (pCoord->xmax - pCoord->xmin);
		perc = (xAtSource > pCoord->xmax) ? 1. : (xAtSource - pCoord->xmin) * odx;
		if (pCoord->isSwapX) {
			x1 = x2;
			x2 = pCoord->xmin;
		}
		else {
			x1 = pCoord->xmin;
		}
	}
	else {
		x1 = (xAtSource < pCoord->xmin) ? pCoord->xmin : xAtSource;
		// perc = (toPCoord->xmax - x1) / (pCoord->xmax - pCoord->xmin);
		perc = (xAtSource < pCoord->xmin) ? 1. : (pCoord->xmax - xAtSource) *odx;

		if (pCoord->isSwapX) {
			x2 = x1;
			x1 = pCoord->xmax;
		}
		else {
			x2 = pCoord->xmax;
		}
	}

	double tRed1 = CCoords::getReduceTimeEx2(
		pCoord->getOffsetFromXVal(x1, xAtSource),
		pCoord->drawmode, &toRed);
	double tRed2 = CCoords::getReduceTimeEx2(
		pCoord->getOffsetFromXVal(x2, xAtSource),
		pCoord->drawmode, &toRed);
	double tRedGDC1 = CCoords::getReduceTimeEx2(
		pCoord->getOffsetFromXVal(x1, xAtSource),
		pCoord->drawmode, &fromRed);
	double tRedGDC2 = CCoords::getReduceTimeEx2(
		pCoord->getOffsetFromXVal(x2, xAtSource),
		pCoord->drawmode, &fromRed);


	double tShifted = pCoord->Y1() - fromYmin;
	if (pCoord->isFaceUp) {
		// one just needs to change the destination Y values for face up
		cps[0] = pCoord->mm_coord(x1, pCoord->Y2() + tRedGDC1 - tRed1 - tShifted);
		cps[1] = pCoord->mm_coord(x2, pCoord->Y2() + tRedGDC2 - tRed2 - tShifted);
		cps[2] = pCoord->mm_coord(x1, pCoord->Y1() + tRedGDC1 - tRed1 - tShifted);
	}
	else {
		cps[0] = pCoord->mm_coord(x1, pCoord->Y1() + tRedGDC1 - tRed1 - tShifted);
		cps[1] = pCoord->mm_coord(x2, pCoord->Y1() + tRedGDC2 - tRed2 - tShifted);
		cps[2] = pCoord->mm_coord(x1, pCoord->Y2() + tRedGDC1 - tRed1 - tShifted);
	}

	toDC->PlgBlt(cps, fromDC,
		fromRect->left + (LONG)(fromRect->Width() * (1.0 - perc)),
		fromRect->top,
		(int)(perc * fromRect->Width()),
		fromRect->Height(),
		bm, 0, 0);



	DeleteObject(bm);
}




BOOL CDCHelper::myCreateTempDC(CDC *pDCfrom, CDC *pDCto, CRect rectPixel, CRect rectLogical)
{
	// note: rectPixel is in the pixels
	// rectLogical is the user coords to be mapped into rect1 pixels. Not in km/sec but int only
	// pDCto->DeleteDC();  // in case it is already initialized, we delete it first otherwise it has memory leak
	// make sure you pDCto->DeleteDC() yourself if it already contains data
	//               and do not call DeleteDC() function if it is not inialized to avoid exception
	if (pDCfrom == NULL) return FALSE;
	if (!pDCto) return FALSE;
	BOOL trueFalse = pDCto->CreateCompatibleDC(pDCfrom);
	if (!trueFalse) return FALSE;

	CBitmap m_bmp; //deleted every time a routine has finished using it.
	trueFalse = m_bmp.CreateCompatibleBitmap(pDCfrom, rectPixel.right, rectPixel.bottom);
	pDCto->SelectObject(&m_bmp);
	pDCto->SetMapMode(MM_ANISOTROPIC);
	pDCto->SetViewportExt(rectPixel.right, rectPixel.bottom);
	pDCto->SetWindowExt(rectLogical.right, rectLogical.bottom);	 //for rectClient, logical, himetric coords.
	pDCto->SetBkMode(TRANSPARENT); //to avoid texts overlap each other.
	CBrush brBackground(::GetSysColor(COLOR_WINDOW));
	pDCto->FillRect(rectLogical, &brBackground);
	m_bmp.DeleteObject();
	return TRUE;
}


