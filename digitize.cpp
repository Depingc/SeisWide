#include "stdafx.h"
#include "digitize.h"
#include "util.h"
#include "math.h"
#include "coords.h"
#include "colorHelper.h"
#include "xallocator.h"

CDigitize::CDigitize(void)
{
}

CDigitize::~CDigitize(void)
{
}








void CDigitize::myClipReducePointsTrans(CDigitize::DIGITIZE *digIn,
	CDigitize::DIGITIZE *digOut,
	SEI_QUAD seiq, CCoords::REDUCE *pred,
	double xShot, BOOL isDistanceDisplay, BOOL isMinusXShot,
	int drawmode, bool isReducedOutput)
{
	if (digIn->totCurve < 1) return;
	double x;
	digOut->initDigitizeArray(digIn->digPointMax, digIn->totCurve);
	double* xtemp = (double *)ealloc1double((size_t)digIn->digPointMax * 2);
	double* ztemp = (double *)ealloc1double((size_t)digIn->digPointMax * 2);

	for (int ii = 0; ii < digIn->totCurve; ii++) {
		int n = digIn->nPoint[ii];
		int i;
		for (i = 0; i < n; i++) {
			xtemp[i] = digIn->x[i][ii]; // ii: curve index;  i: point index per curve
			ztemp[i] = digIn->t[i][ii];
			if (myFloat(pred->rvred, 2) == 0.0f) {
				xtemp[i] *= digIn->transXMultFactor;
				xtemp[i] += digIn->transXShift;
				if (digIn->transIsRevYAxis) ztemp[i] = -ztemp[i];
				ztemp[i] *= digIn->transYMultFactor;
				ztemp[i] += digIn->transYShift;
			}
			else {
				x = CCoords::getOffsetFromXVal(xtemp[i], xShot, isDistanceDisplay, isMinusXShot);
				ztemp[i] -= CCoords::getReduceTimeEx2(x, drawmode, pred);
				//			ztemp[i] -= getReduceTimeR(getOffsetFromXVal(xtemp[i]), para.red.rvred);
			}
		}
		register int nLimit = min(2 * n, digIn->digPointMax * 2);
		n = myClipPoints2(n, nLimit, xtemp, ztemp, seiq);
		for (i = 0; i < n; i++) {
			digOut->x[i][ii] = (float)xtemp[i];
			digOut->t[i][ii] = (float)ztemp[i];
			if (isReducedOutput && myFloat(pred->rvred, 2) != 0.0f) {
				x = CCoords::getOffsetFromXVal(
					xtemp[i], xShot, isDistanceDisplay, isMinusXShot
				);
				digOut->t[i][ii] += (float)CCoords::getReduceTimeEx2(x, drawmode, pred);
			}
		}
		digOut->nPoint[ii] = n;
	} // for ii
	free1double(xtemp); xtemp = NULL;
	digOut->totCurve = digIn->totCurve;
	digOut->iCurveSel = digIn->iCurveSel;
	digOut->isDigitizing = digIn->isDigitizing;
	digOut->strComment = digIn->strComment;

	free1double(ztemp); ztemp = NULL;
}

void CDigitize::myClipReducePoints4(CDigitize::DIGITIZE *digIn, CDigitize::DIGITIZE *digOut,
	CCoords::SEI_COORD *pCoord, CCoords::REDUCE *pred, float xShotRay, bool isReducedOutput)
{
	//SEI_PAIR seiMin, seiMax;
	//seiMin.x = pCoord->X1();
	//seiMin.y = pCoord->Y1();
	//seiMax.x = pCoord->X2();
	//seiMax.y = pCoord->Y2();
	//
	CDigitize::myClipReducePointsTrans(digIn, digOut, pCoord->X12Y12(), pred,
		xShotRay, pCoord->isDistanceDisplay, pCoord->isMinusXShot,
		pCoord->drawmode, isReducedOutput);
	return;
}


// prefered way of drawing markers
//void CDigitize::DrawMarkersNotUsed(CDC* pDC, CDigitize::DIGITIZE *digMarker, CCoords *cd, CCoords::REDUCE *red,
//	int drawmode, float xShot, float thicknessLines)
//{
//
//	int j, k;
////	double xpos, ypos;
//
//	CString tstr;
//	CPen* oldPen;
//	int dPlt = (int)(min(cd->coord.logicalAxes.Width(), cd->coord.logicalAxes.Height()) / 350. * thicknessLines);
//
//	CDigitize pDg;
//	
//	CBrush brush, *oldBrush;
//	brush.CreateSolidBrush(GREEN);
//	CPoint *pltV = (CPoint *)alloc1CPoint(digMarker->DigPointMax);
//
//	CDigitize::DIGITIZE dig3;
//	CDigitize::myClipReducePointsTrans(digMarker, &dig3, cd->coord.X12Y12(), red,
//			xShot, cd->coord.isDistanceDisplay, cd->coord.isMinusXShot, drawmode, false);
//
//	for (k = 0; k < dig3.totCurve; k++) {
//			register int n = dig3.nPoint[k];
//			for (j = 0; j < n; j++) {
//				pltV[j] = cd->coord.mm_coord(dig3.x[j][k], dig3.t[j][k]);
//			} // for j
//
//			CPen redPen(PS_SOLID, 5, RED);
//			oldBrush = pDC->SelectObject(&brush);
//			oldPen = pDC->SelectObject(&redPen);
//			for (j = 0; j < n; j++)
//				pDC->Ellipse(
//				pltV[j].x - dPlt,
//				pltV[j].y - dPlt,
//				pltV[j].x + dPlt,
//				pltV[j].y + dPlt
//				);
//			pDC->SelectObject(oldBrush);
//			pDC->SelectObject(oldPen);
//	} // for k
//
//	brush.DeleteObject(); // this will be done automatically even if you do not do it here
//	free1(pltV);
//	dig3.freeDigAll();
//
//	return;
//}





BOOL CDigitize::myDigSaveAs2(CDigitize::DIGITIZE *dig2, bool isXInt)
{
	if (dig2->totCurve < 1 || !dig2->nPoint) return FALSE;

	if (dig2->isZeltFormat) {
		AfxMessageBox(_T("Sorry, cannot save in Zelt format here"));
		return FALSE;
	}
	//pDg.myDigSaveAsZelt(&dig, dig.xShotCache, cd.coord.isMinusXShot);

	// now save to either TXT or NAV format depending on digFormat is 0 or 2
	CString aName = dig2->digName;

	// now we only need to consider plain text format
	if (aName.IsEmpty()) {
		aName = _T("_digiAuto.");
		aName += dig2->getDigExtension();
	}

	float t1,t2;
	int fileFormat=0;
	FILE *stream;
	long i,j;
	static int failedCount = 0;

	CString tStr;
	CString aNameBak;
	aNameBak = aName+".bak";
	if( _tfopen_s(&stream, aName, _T("w") ) != 0) {
		tStr.Format(_T("Failed to open stream to write to for %s"), aName);
		if (failedCount) {
			failedCount++;
			AfxMessageBox(tStr, MB_ICONQUESTION);
		}
		return FALSE;
	}

	// there may be some points not in proper order. Let's correct for this
	myDigSort(dig2);

	if (!dig2->strComment.IsEmpty())
		_ftprintf_s(stream,_T("#%s\n"), (LPCTSTR)dig2->strComment);


	// first, we remove an irregularity
	for (i=0; i<dig2->totCurve; i++) {
		for (j=0; j<dig2->nPoint[i]; j++) {
			if (dig2->x[j][i] < ERRORVAL && dig2->t[j][i] < ERRORVAL) {
				this->myDigRemove(dig2, j, i); // this changes the num of points for this curve
				break;
			}
		}

		if (isFloatEqual3(dig2->curveVal[i], ERRORVAL)) {
			dig2->curveVal[i] = (float)i;
		}
		if (i > 0) {
			if (isFloatEqual3(dig2->curveVal[i], dig2->curveVal[i - 1]))
				dig2->curveVal[i] = (float)dig2->curveVal[i - 1] + 1.f;
		}
	} // for

	i = dig2->totCurve;  if (dig2->nPoint[i] > 0) dig2->totCurve++;


	if (dig2->isTraceFormat) {
		for (int ic = 0; ic < dig2->totCurve; ic++) {
			if (isFloatEqual(dig2->curveVal[ic], ERRORVAL))
				tStr.Format(_T("%g"), (double)(dig2->curveVal[ic] - ic));
			// why -ic? this way, we mark up the curve as a new curve in the text file
			else
				tStr.Format(_T("%g"), dig2->curveVal[ic]);

			//dig2->labelArr->Add(tStr);
		}
	}


   _ftprintf_s(stream,_T("# digitization save. To avoid auto sorting, just manually insert a point 0.001 apart in X.\n"));
   _ftprintf_s(stream,_T("#          X           Y LABEL\n"));

	for (i=0; i<dig2->totCurve; i++) {
		if (dig2->nPoint[i] < 1) continue;  // we will not save a curve with no point!
		tStr = dig2->getLabelString(i);
		for (j=0; j<dig2->nPoint[i]; j++) {
			t1 = dig2->x[j][i];
			t2 = dig2->t[j][i];
			if (isXInt)
				_ftprintf_s(stream, _T(" %d %11.5f %s"),(int)myRound(t1),t2, (LPCTSTR)tStr);
			else
				_ftprintf_s(stream, _T(" %11.5f %11.5f %s"), t1, t2, (LPCTSTR)tStr);


			_ftprintf_s(stream,_T("\n"));

		}
	}


	fclose( stream );
	dig2->modifiedtime = getFileModifiedTime(aName);
	return TRUE;
}

CString CDigitize::myDigSaveWithFFID(CDigitize::DIGITIZE *dig2, int countTrace, int *ptrace, float *pdist, int *pffid, float *poffset)
{
	CString tStr;
	if (countTrace < 1) {
		tStr.Format(_T("No trace info available."));
		AfxMessageBox(tStr, MB_ICONQUESTION);
		return BLANK;
	}





	float xp,tp;
	int fileFormat=0;
   long i,j;
   int kk;

   CString fout;
	//i = dig2->totCurve;  
	//if (dig2->nPoint[i] > 0) dig2->totCurve++;

	FILE *stream = NULL;
	CString aExt = dig2->getDigExtension();
	//fout = getSystemTempPathSlash() + GetBaseFileName(dig2->digName) + _T(".tmp.") + aExt;
	fout = getFileNameAppendTemp(dig2->digName, _T("_tmp"));
	if( _tfopen_s( &stream, fout, _T("w") ) != 0 ) {
		return BLANK;
   }

   sort2(countTrace, pdist, ptrace);
//   sort2(countTrace, poffset, ptrace); // you can only sort on one thing
//   sort2(countTrace, pffid, ptrace);
   _ftprintf_s(stream,_T("# Current digitized dump; trace FFID, DISTANCE and OFFSET are for the nearest trace\n"));
   _ftprintf_s(stream,_T("#        X         Y      TRACE      FFID  DISTANCE   OFFSET     RES/VAL     IVRAY  OTHERS\n"));
   register double xTrcKK1, xTrcKK2;
   int kk1, kk2;
   for (i=0; i<dig2->totCurve; i++) {
	   for (j=0; j<dig2->nPoint[i]; j++) {
		   xp = dig2->x[j][i];
		   tp = dig2->t[j][i];
		   

		   if (dig2->xMode == this->trace) {
			   // in trace mode,
			   kk = getClosestIndex(ptrace, countTrace, xp);
		   }
		   else if (dig2->xMode == this->distance) {
			   // dist mode
			   kk = getClosestIndex(pdist, countTrace, xp);
		   }
		   else {
			   // offset mode
			   kk = getClosestIndex(poffset, countTrace, xp);
		   }
		   if (kk > 0) {
			   kk1 = kk - 1;
			   kk2 = kk;
		   }
		   else {
			   kk1 = 0;
			   kk2 = 1;
		   }


		   if (dig2->xMode == this->trace) {
			   xTrcKK1 = ptrace[kk1];
			   xTrcKK2 = ptrace[kk2];
			   _ftprintf_s(stream,_T(" %9d"),(int)xp);
		   }
		   else if (dig2->xMode == this->distance) {
			   xTrcKK1 = pdist[kk1];
			   xTrcKK2 = pdist[kk2];
			   _ftprintf_s(stream,_T(" %9.3f"),xp);
		   }
		   else {
			   xTrcKK1 = poffset[kk1];
			   xTrcKK2 = poffset[kk2];
			   _ftprintf_s(stream,_T(" %9.3f"),xp);
		   }
		   _ftprintf_s(stream,_T(" %9.3f"), tp);
		   register double trc = ptrace[kk];
		   register double ffid = pffid[kk];
		   
		   if (!isFloatEqual(xp, (float)xTrcKK2)) {
			   register double dx = xTrcKK2 - xTrcKK1;
			   if (!isFloatEqual(dx, 0.)) {
				   // need to do interpolation since the digitized point is not exactly at the trace 
				   //double tFac = (range - modX1->dist) / dx;
				   //register double za = interpolLinear(modX1->depthMod[i], modX2->depthMod[i2], tFac);
				   register double tFac = (xp - xTrcKK1) / dx;
				   trc = interpolLinear(ptrace[kk1], ptrace[kk2], tFac);
				   ffid = interpolLinear(pffid[kk1], pffid[kk2], tFac);
			   }
		   }

		   _ftprintf_s(stream, _T("  %9d %9d %9.3f %9.3f %9.3f %9d"), 
			   (int)(0.05+trc), (int)(0.05+ffid), pdist[kk], poffset[kk], dig2->curveVal[i], dig2->ivray[i]);
		   // why I add 0.05 ?  It is because we want to print as 1195.3 and we need to wrap around with 1 decimal.
		   _ftprintf_s(stream,_T("  %s"), (LPCTSTR)dig2->getLabelString(i));
		   _ftprintf_s(stream,_T("\n"));

	   }
   }
	fclose( stream );
	return fout;
}
//
//BOOL CDigitize::myDigSaveWithFFIDDepreciated2(CDigitize::DIGITIZE *dig2, CString aName, CString timeSecName)
//{
//
//   int countTrace = 0;
//   int *ptrace = NULL;
//   float *pdist = NULL;
//   int *pffid = NULL;
//   float *poffset = NULL;
//
//   if (isFileExistNotDirectory(timeSecName) ) {
//   // if segy file name exists, try to save FFID numbers in addition to the distance km.
//		CFile file;
//		if (!file.Open(timeSecName, CFile::modeRead | CFile::typeBinary)) {
//			file.Close();
//			return FALSE;
//		}
//	   file.Seek(3200, CFile::begin);
//	   bhed bh;
//	   segy tr;
//	   if(file.Read(&bh, 400) < 400) {
//		   file.Close();
//		   return FALSE;
//	   }
//	   
//	   long len = 0;
//	   if(bh.format!=3)  len = bh.hns*4 + 240;
//	   else len = bh.hns*2 + 240;
//	   
//	   while (file.Read(&tr, 4)==4) {
//		   countTrace++;
//		   file.Seek(len-4, CFile::current);
//	   }
//	   
//	   ptrace = (int *)ealloc1int(countTrace+1);  // must add 1 more point for work space
//	   pdist = (float *)ealloc1float(countTrace+1);
//	   poffset = (float *)ealloc1float(countTrace+1);
//	   pffid = (int *)ealloc1int(countTrace+1);
//	   int j = 0;
//	   file.Seek(3600, CFile::begin);
//	   while (file.Read(&tr, 24)==24) {
//		   ptrace[j] = (int)tr.tracl;
//		   pdist[j] = getDistance(&tr);
//		   poffset[j] = getOffset(&tr);
//		   pffid[j] = (int)tr.fldr;
//		   j++;
//		   file.Seek(len-24, CFile::current);
//		 //  if (j>= countTrace) break;
//	   }
//	   countTrace = j;  // make sure it does not overflow.
//	   file.Close();
//   } // if (!draw.timeSecName.IsEmpty())
//
//   if (ptrace == NULL) {
//	   // segy cannot read. So we just need to show digitizing results with another function
//	   return this->myDigSaveAs(dig2, FALSE, FALSE, 0.);
//   }
//
//
//
//   myDigSaveWithFFID(dig2, countTrace, ptrace, pdist, pffid, poffset);
//	// AfxMessageBox(_T("Digitized points saved; you can still continue digitizing while looking at this file");
//	free1int (ptrace);
//	free1float (pdist);
//	free1float (poffset);
//	free1int (pffid);
//	
//
//	return TRUE;
//}

/*
// this function is not used, but good to use
CString CDigitize::myDigSaveAsPlainTrc(CDigitize::DIGITIZE *dig2, CString aName, CString timeSecName, BOOL isSort, BOOL isSortY, 
									   BOOL isDistanceDisplay2, double xmin, double xmax)
{
	CString tStr;
	if (dig2->isZeltFormat2) return BLANK;  // only plain text format is needed here.
	aName = GetFileNameAppendExt(aName, _T("_digi"), _T("tracl"));

	float t1,t2;
	int fileFormat=0;
	FILE *stream;
   int i,j;

   
   if( _tfopen_s( &stream, aName, _T("w") ) != 0 ) return BLANK;

   int countTrace = 0;
   float *ptrace = NULL;
   float *pdist = NULL;

   if (isFileExistNotDirectory(timeSecName) )  {
		CFile file;
		if (!file.Open(timeSecName, CFile::modeRead | CFile::typeBinary)) {
			return BLANK;
		}
	   file.Seek(3200, CFile::begin);
	   bhed bh;
	   segy tr;
	   if(file.Read(&bh, 400) < 400) {
		   fclose(stream);
		   file.Close();
		   return BLANK;
	   }
	   
	   long len = 0;
	   if(bh.format!=3)  len = bh.hns*4 + 240;
	   else len = bh.hns*2 + 240;
	   
	   while (file.Read(&tr, 4)==4) {
		   countTrace++;
		   file.Seek(len-4, CFile::current);
	   }
	   
	   ptrace = (float *)ealloc1float(countTrace+1);  // must add 1 more point for work space
	   pdist = (float *)ealloc1float(countTrace+1);
	   j = 0;
	   file.Seek(3600, CFile::begin);
	   while (file.Read(&tr, 240)==240) {
		   ptrace[j] = (float)tr.tracl;
		   pdist[j] = (isDistanceDisplay2)  ?   getDistance(&tr)  :  getOffset(&tr);  // in km
		   j++;
		   file.Seek(len-240, CFile::current);
		 //  if (j>= countTrace) break;
	   }
	   countTrace = j;  // make sure it does not overflow.
	   file.Close();
	   sort2(countTrace, pdist, ptrace);
   }
 



   // there may be some points not in proper order. Let's correct for this
   this->myDigSort(dig2, isSort, FALSE, xmin, xmax);

   if (isFileExistNotDirectory(timeSecName)) {
	   _ftprintf_s(stream,_T("#%s\n"), timeSecName);
	   // _ftprintf_s(stream,_T("#OFFSET(DIG'ED) TIME(S)  NUM  TRACE_NUM\n"));
   }
   for (i=0; i<dig2->totCurve; i++)
   {
	   tStr = dig2->getLabelString(i);
	   for (j=0; j<dig2->nPoint[i]; j++)
	   {
		   t1 = dig2->x[j][i];
		   t2 = dig2->t[j][i];

		   _ftprintf_s(stream,_T(" %11.5f %11.5f %s"),t1,t2,tStr);
		   if (pdist!=NULL && ptrace!=NULL) {
			   int kk = getClosestIndex(pdist, &countTrace, &t1);
			   if (kk>= 0) {
				   _ftprintf_s(stream,_T("  %11d"), (UINT)ptrace[kk]);
			   }
		   }
		   _ftprintf_s(stream,_T("\n"));
	   }
   }
	fclose( stream );

	free1float (ptrace);
	free1float (pdist);

	return aName;


}
*/


//
//void CDigitize::myDigAddCurve(DIGITIZE *dig2, double xShot) 
//{
//	int n = dig2->totCurve;
//	if (n >= dig2->digLayerMax) {
//		myDigArraySizeInc(dig2, 100, 10);
//	}
//
//	dig2->totCurve++;
//}

void CDigitize::myDigAddPoint(DIGITIZE *dig2, int &ic, double x, double t, double xShot) 
{
	// note: this changes draw.dig.nPoint[ic]
	if (ic >= dig2->totCurve) {
		dig2->myDigAddCurve(xShot);
		ic = dig2->totCurve - 1; // ic now points to the last curve!!!
		if (ic < 0) ic = 0;
	}
	if (dig2->nPoint[ic] >= dig2->digPointMax) {
		myDigArraySizeInc(dig2, 10000, 0);
		// increase the array size and copy the old data to the new array
	}

	int n = dig2->nPoint[ic];
	dig2->x[n][ic] = (float)myFloat(x);
	dig2->t[n][ic] = (float)myFloat(t);
	dig2->nPoint[ic]++;
}

bool CDigitize::myDigArraySizeInc(DIGITIZE *dig2, int np_inc, int nc_inc)
{
	CDigitize::DIGITIZE dig3;
	if (nc_inc < 0) nc_inc = 0;
	if (np_inc < 0) np_inc = 0;
	if (nc_inc == 0 && np_inc == 0) return true;

	int ncurve = dig2->digLayerMax + nc_inc;
	int npoint = dig2->digPointMax + np_inc;
	dig3.initDigitizeArray(npoint, ncurve);

	for (int ic = 0; ic < dig2->totCurve; ic++) {
		for (int j = 0; j < dig2->nPoint[ic]; j++) {
			dig3.x[j][ic] = dig2->x[j][ic];
			dig3.t[j][ic] = dig2->t[j][ic];
		}
		dig3.nPoint[ic] = dig2->nPoint[ic];
		dig3.ivray[ic] = dig2->ivray[ic];
		dig3.curveVal[ic] = dig2->curveVal[ic];
	}

	dig3.digName = dig2->digName;
//	dig3.labelArr->Copy(*dig2->labelArr);
	dig3.totCurve = dig2->totCurve;
	dig3.strComment = dig2->strComment;
	
	// now copy back 
	dig2->initDigitizeArray(npoint, ncurve);
	for (int ic = 0; ic < dig3.totCurve; ic++) {
		for (int j = 0; j < dig3.nPoint[ic]; j++) {
			dig2->x[j][ic] = dig3.x[j][ic];
			dig2->t[j][ic] = dig3.t[j][ic];
		}
		dig2->nPoint[ic] = dig3.nPoint[ic];
		dig2->ivray[ic] = dig3.ivray[ic];
		dig2->curveVal[ic] = dig3.curveVal[ic];
	}

	dig2->digName = dig3.digName;
//	dig2->labelArr->Copy(*dig3.labelArr);
	dig2->totCurve = dig3.totCurve;
	dig2->strComment = dig3.strComment;

	dig3.freeDigAll();
	return true;
}


// note: this changes dig2->nPoint[ic]
BOOL CDigitize::myDigRemove(CDigitize::DIGITIZE *dig2, int ip, int ic)
{
	// this function does not call any global variables, can be moved to util.cpp
	if (ic<0 || ic>dig2->totCurve-1) return FALSE;
	if (dig2->nPoint[ic]<1 || ip<0 || ip>dig2->nPoint[ic]-1) return FALSE;

	for (int j=ip; j<dig2->nPoint[ic]-1; j++) {
	   dig2->x[j][ic] = dig2->x[j+1][ic];
	   dig2->t[j][ic] = dig2->t[j+1][ic];
	} // j
	dig2->nPoint[ic]--;
	return TRUE;
}


BOOL CDigitize::digDeleteCurve(DIGITIZE *dig2, int ic)
{
	if (dig2->totCurve<2 || ic<0 || ic>dig2->totCurve-1) return FALSE;

	for (int i=ic; i<dig2->totCurve-1; i++) {
		dig2->nPoint[i] = dig2->nPoint[i+1];
		dig2->ivray[i] = dig2->ivray[i+1];
		dig2->curveVal[i] = dig2->curveVal[i + 1];
		dig2->labelArr->SetAt(i, dig2->labelArr->GetAt(i+1));
		for (int j=0; j<dig2->nPoint[i]; j++) {
			dig2->x[j][i] = dig2->x[j][i+1];
			dig2->t[j][i] = dig2->t[j][i+1];
		} // for j
	}
	dig2->totCurve--;

	return TRUE;
}


BOOL CDigitize::digSplitCurve(DIGITIZE *dig2, int ic, double xSplit) 
{ 
	// note: this changes draw.dig.totCurve
	if (ic<0 || ic>dig2->totCurve-1) return FALSE;
	if (dig2->nPoint[ic]<2) return FALSE;  // cannot divide a curve with 1 point
	this->myDigSort(dig2);

	int n = dig2->nPoint[ic];
	if (xSplit <= dig2->x[0][ic] || xSplit >= dig2->x[n-1][ic]) return FALSE;

	int insC = dig2->totCurve;  // new curve number
	dig2->nPoint[insC] = 0; // at the beginning there is only zero points for the new curve

	for (int j=1; j<n; j++) {
		if (dig2->x[j][ic] > xSplit) {
			for (int k=j; k<n; k++) {
				myDigAddPoint(dig2, insC, (double)dig2->x[k][ic], (double)dig2->t[k][ic]);
			}

			// get the ySplit for this xSplit point using linear interpolation
			double diff = dig2->x[j][ic] - dig2->x[j-1][ic];
			double ySplit = (fabs(diff) < MINAMP)  ?   dig2->t[j-1][ic]:
				dig2->t[j-1][ic] + (dig2->t[j][ic] - dig2->t[j-1][ic]) * (xSplit - dig2->x[j-1][ic]) / diff;
			dig2->nPoint[ic] = j;
			myDigAddPoint(dig2, ic, xSplit, ySplit);

			// if the newly inserted curve has only 1 point, we want to insert a new point at the beginning
			//if (n = dig2->nPoint[insC] < 2)
			//	myDigAddPoint(dig2, insC, xSplit, ySplit);

			break;
		}
	}

	if (dig2->nPoint[insC] > 0) {
		dig2->totCurve ++;
		dig2->iCurveSel = insC;
		dig2->curveVal[insC] = dig2->curveVal[ic] + 0.001f;  // adding 0.001f to make it recognized as a diff curve
		dig2->ivray[insC] = dig2->ivray[ic];
		dig2->labelArr->Add(dig2->getLabelTextOnly(ic) + _T("Split"));
	}

	return TRUE;
}

// This function will remove any duplicated x-values of *dig2 for curve ic; no pre-sorting is needed
void CDigitize::myDigRemoveDupX(CDigitize::DIGITIZE *dig2, int ic)
{
	if (ic<0 || ic>dig2->totCurve - 1) return;
	if (dig2->nPoint[ic]<1) return;
	if (!dig2->isSortedX) return;  // note: this function only remove duplicates along X-axis for each curve. If not sorting along X-axis, we cannot do it.

	float xres = 0.00001f; // we only want cm resolution
	int j = 0;

	do {
		j++;
		if (isFloatEqual(dig2->x[j][ic], dig2->x[j-1][ic], xres)) {
			// now remove 1 point
			if (myDigRemove(dig2, j, ic))
				j--;
		} // if
	} while (j < dig2->nPoint[ic]-1);
}

// This function will remove any duplicated y-values of *dig2 for curve ic; no pre-sorting is needed
void CDigitize::myDigRemoveDupY(CDigitize::DIGITIZE *dig2, int ic)
{
	if (ic<0 || ic>dig2->totCurve - 1) return;
	if (dig2->nPoint[ic]<1) return;
	if (!dig2->isSortedY) return;  // note: this function only remove duplicates along X-axis for each curve. If not sorting along X-axis, we cannot do it.

	float xres = 0.00001f; // we only want cm resolution
	int j = 0;

	do {
		j++;
		if (isFloatEqual(dig2->t[j][ic], dig2->t[j - 1][ic], xres)) {
			// now remove 1 point
			if (myDigRemove(dig2, j, ic))
				j--;
		} // if
	} while (j < dig2->nPoint[ic] - 1);
}

void CDigitize::myDigRemoveInside3(CDigitize::DIGITIZE *dig2, int ic, float xa, float xb)
{
	// now, we need to remove all dig points inside (bRange0, bRange)
	//      excluding bRange0 and bRange of course
	float dx = (float)fabs(0.1*(xb - xa));
	int j;
	j = 0;
	do {
		j++;
		register float x = dig2->x[j-1][ic];
		if (fabs(x - xa) < dx) continue;  // do not delete any points at xa or xb
		if (fabs(x - xb) < dx) continue;  // do not delete any points at xa or xb
		if ((x>xa && x<xb) || (x<xa && x>xb)) {
			// this point x should be removed
			if (myDigRemove(dig2, j - 1, ic))
				j--;
		} // if
	} while (j < dig2->nPoint[ic]-1);

	// after the above processing, I found that when a point is not at xa and xb 
	// but very close to them, the point was not removed.
	// So now only check for two points too close to each other
	double xres = 0.00001; // we only want cm resolution
	j = 0;
	do {
		j++;
		register float x = dig2->x[j-1][ic];
		if (fabs(x - xa) < xres) continue;  // do not delete any points at xa or xb
		if (fabs(x - xb) < xres) continue;  // do not delete any points at xa or xb
		register float dxa = (float)fabs(x - xa);
		register float dxb= (float)fabs(x - xb);
		if ((dxa > xres && dxa < dx)
			||
			(dxb > xres && dxb < dx))
		{
			// this point x should be removed
			if (myDigRemove(dig2, j - 1, ic))
				j--;
		}

	} while (j < dig2->nPoint[ic]-1);
}

void CDigitize::myDigShift(CDigitize::DIGITIZE *dig2, float dx, float dt, float xLimit1, float xLimit2, int iCurveSel)
{

	int i, j;
	BOOL isShiftInRange = !isFloatEqual(xLimit1, xLimit2) && !(isFloatEqual(xLimit1, xLimit2) && isFloatEqual(xLimit1, 0.f));
	if (AfxMessageBox(_T("Only shift the selected curve? Select No to shift the all digitized points."), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		i = iCurveSel;
		for (j = 0; j < dig2->nPoint[i]; j++)
		{
			if (!isShiftInRange || (isShiftInRange && dig2->x[j][i] >= xLimit1 && dig2->x[j][i] <= xLimit2)) {
				dig2->x[j][i] += dx;
				dig2->t[j][i] += dt;
			}
		}
	}
	else {
		for (i = 0; i < dig2->totCurve; i++)
		{
			for (j = 0; j < dig2->nPoint[i]; j++)
			{
				if (!isShiftInRange || (isShiftInRange && dig2->x[j][i] >= xLimit1 && dig2->x[j][i] <= xLimit2)) {
					dig2->x[j][i] += dx;
					dig2->t[j][i] += dt;
				}
			}
		}

		//if (AfxMessageBox(_T("Do you want to shift the source position as well (by the same amount)?"), MB_YESNO | MB_ICONINFORMATION) == IDYES) {
		//	tx.xShotRay += dx;
		//	AfxMessageBox(_T("Since the source for TX picks is changed, you need to update the source position in the segy header as well. Use Processing | Define Source Distance to do this. Otherwise, you will not be able to load the coresponding TX picks the next time."));
		//}
	}

}

void CDigitize::myDigSortX2(CDigitize::DIGITIZE *dig2, double xmin, double xmax)
{
	// this function does not call any global variables, can be moved to other classes
	// only if there are 2 points spaced at 1.0, we will NOT sort this curve!!! This is designed for special occasions.
	// check if this function works ok????
	if (!dig2->isSortedX || dig2->totCurve<1 || !dig2->nPoint) return; // dig2->nPoint==NULL has occurred before
	float temp;
	   bool isSortNeeded;
	   bool isMinMaxSpecified = !isFloatEqual(xmin, xmax);
	   for (int ic=0; ic<dig2->totCurve; ic++) {
			isSortNeeded = false;
			for (int i = 0; i < dig2->nPoint[ic]-1; i++) {
				if (dig2->x[i][ic] > dig2->x[i + 1][ic] && myFloat(fabs(dig2->x[i][ic] - dig2->x[i + 1][ic]), 3)!=1.) {
					// only if there are 2 points spaced at 1.0, we will NOT sort this curve
					isSortNeeded = true;
					break;
				}
			}

			if (isSortNeeded) {
				// now sort
				for (int i = 0; i<dig2->nPoint[ic]; i++) {
					if (isMinMaxSpecified && (dig2->x[i][ic]<xmin || dig2->x[i][ic] > xmax)) continue;  // just to speed up

					register int min_n = i;
					for (int j = i + 1; j<dig2->nPoint[ic]; j++)
						if (dig2->x[j][ic] < dig2->x[min_n][ic]) min_n = j;

					// now swap
					temp = dig2->x[i][ic];
					dig2->x[i][ic] = dig2->x[min_n][ic];
					dig2->x[min_n][ic] = temp;

					temp = dig2->t[i][ic];
					dig2->t[i][ic] = dig2->t[min_n][ic];
					dig2->t[min_n][ic] = temp;
				} // i
			}

			myDigRemoveDupX(dig2, ic);
	   } // ic

}


void CDigitize::myDigSortY2(CDigitize::DIGITIZE *dig2, double ymin, double ymax)
{
	// only if there are 2 points spaced at 1.0, we will NOT sort this curve!!! This is designed for special occasions.
	// check if this function works ok????
	if (!dig2->isSortedY) return;
	float temp;
	bool isSortNeeded;
	bool isMinMaxSpecified = !isFloatEqual(ymin, ymax);
	for (int ic = 0; ic<dig2->totCurve; ic++) {
		isSortNeeded = false;
		for (int i = 0; i < dig2->nPoint[ic] - 1; i++) {
			if (dig2->t[i][ic] > dig2->t[i + 1][ic] && myFloat(fabs(dig2->t[i][ic] - dig2->t[i + 1][ic]), 3) != 1.) {
				// only if there are 2 points spaced at 1.0, we will NOT sort this curve
				isSortNeeded = true;
				break;
			}
		}

		if (isSortNeeded) {
			// now sort
			for (int i = 0; i<dig2->nPoint[ic]; i++) {
				if (isMinMaxSpecified && (dig2->t[i][ic]<ymin || dig2->t[i][ic] > ymax)) continue;  // just to speed up

				register int min_n = i;
				for (int j = i + 1; j<dig2->nPoint[ic]; j++)
					if (dig2->t[j][ic] < dig2->t[min_n][ic]) min_n = j;

				// now swap
				temp = dig2->t[i][ic];
				dig2->t[i][ic] = dig2->t[min_n][ic];
				dig2->t[min_n][ic] = temp;

				temp = dig2->x[i][ic];
				dig2->x[i][ic] = dig2->x[min_n][ic];
				dig2->x[min_n][ic] = temp;
			} // i
		}

		myDigRemoveDupY(dig2, ic);
	} // ic

}


void CDigitize::myDigSort(CDigitize::DIGITIZE *dig2)
{
	if (dig2->isSortedX)
		myDigSortX2(dig2, 0., 0.);
	else if (dig2->isSortedY)
		myDigSortY2(dig2, 0., 0.);
}


BOOL CDigitize::myDigSaveAsZelt(CDigitize::DIGITIZE *dig2, double xs, BOOL isMinusXShot)
{
	if (dig2->totCurve < 1 || !dig2->nPoint) return FALSE;
	if (!dig2->isZeltFormat) 
		return myDigSaveAs2(dig2, false);

	// now we only need to consider zelt format below
	float t1,t2,t3;
	short n, k;
	int fileFormat=0;
	FILE *stream;
	long i,j;
	CString tStr;
	//isSort = TRUE;
	//isSortY = FALSE;
	if( _tfopen_s(&stream, dig2->digName, _T("w") ) != 0) {
		tStr.Format(_T("Failed to open stream to write to for %s"), dig2->digName);
		AfxMessageBox(tStr, MB_ICONQUESTION);
		return FALSE;
	}

	i = dig2->totCurve;  
	// if (dig2->nPoint[i] > 0) dig2->totCurve++;

	// why do we need a backup file before overwriting ?
	// This is because our digitized array in memory only reads in for a 
	// particular source. When we save the digitized data, we do not want 
	// to overwrite data for a different shot.----- solved
	//
	myDigSort(dig2);
	CString aNameBak = dig2->digName+".bak";
	if (isFileExistNotDirectory(aNameBak) ) {

		// We first want to put all dig points that are not of the same source, and write as is.
		// We will EXCLUDE any dig points that have the same source as the active one.
		// This way, we can append the active data to the end of the file.
		FILE  *streamBak;

		if (_tfopen_s( &streamBak, aNameBak, _T("r") ) != 0) {
			TCHAR line[MAXLINE];
			BOOL firstTim=TRUE, textErr=FALSE ;
			while (_fgetts(line,MAXLINE,streamBak)!=NULL) {
				if (_tcslen(line) < 2) continue;
				else if (line[0] == '#') {
					// duplicate data except the end line
					_ftprintf_s( stream, _T("%s"), line);
				}
				else if (_stscanf_s(line,_T("%f%f%f%hd"),&t1, &t2, &t3, &k)==0) {
					textErr=TRUE; 
					continue;
				} 
				else if(k==0 && fabs(t2) == 1.0f) { 
					// Start one side of the OBS.
					//if here then decide the shot location.
					if (!CCoords::isXShotDefined(xs)) xs = t1;

					if(fabs(xs-t1)<=0.00001f) {
						// this shot needs to be excluded.
						while (_fgetts(line,MAXLINE,streamBak)!=NULL) {
							if (_tcslen(line) < 2) continue;
							if (line[0] == '#') continue; // we do not need comment lines
							if (_stscanf_s(line,_T("%f%f%f%hd"),&t1, &t2, &t3, &k)==0) break;
							if(k==0 && (fabs(t2) == 1.0f) && 
								fabs(xs-t1)>0.00001f) {
									break;
							}
							else if((fabs(t2)-1.0f)<0.00001f && k== -1)
							{ //end of the file.
								break;
							}
						} // while
					}
					if(k>= 0) _ftprintf_s( stream, _T("%s"), line);
				} else if(k== -1) { 
					//end of the file; we do not want the last line
					// since it will be appended later
					continue;
				} else {
					// duplicate data except the end line
					_ftprintf_s( stream, _T("%s"), line);
				}
			}
			fclose(streamBak);

		} // if (streamBak 
	} // if (isFileExistNotDirectory(dig2->digName)

	if (!dig2->strComment.IsEmpty()) {
		if (dig2->strComment.GetAt(0) != _T('#'))
			_ftprintf_s(stream,_T("#"));
		_ftprintf_s(stream, _T("%s"), (LPCTSTR)dig2->strComment);
		int len = dig2->strComment.GetLength();
		if (dig2->strComment.GetAt(len-1) != _T('\n'))
			_ftprintf_s(stream,_T("\n"));
	}


	CString strFormat = _T(" %9.3f %9.3f %9.3f %9d\n");
	_ftprintf_s( stream, strFormat, xs, -1.f, 0.f, 0);
	for (i=0; i<dig2->totCurve; i++) {
		if (dig2->nPoint[i] < 1) continue;  // we will not save a curve with only 0 point!
		t3 = dig2->curveVal[i];
		for (j=0; j<dig2->nPoint[i]; j++) {
			t1 = dig2->x[j][i];
			if (dig2->xMode == offset) {
				t1 = (float)(isMinusXShot ? xs - t1 : xs + t1);
			}
			if(t1>xs) continue;
			t2 = dig2->t[j][i]; //reduction velocity is already accounted for.
			_ftprintf_s(stream,strFormat,t1,t2,t3,dig2->ivray[i]);
		}
	}
	_ftprintf_s( stream, strFormat, xs, 1.f, 0.f, 0);
	for (i=0; i<dig2->totCurve; i++) {
		if (dig2->nPoint[i] < 1) continue;  // we will not save a curve with only 0 point!
		t3 = dig2->curveVal[i];
		for (j=0; j<dig2->nPoint[i]; j++) {
			t1 = dig2->x[j][i];
			if (dig2->xMode == offset) {
				t1 = (float)(isMinusXShot ? xs - t1 : xs + t1);
			}
			if(t1<xs) continue;
			t2 = dig2->t[j][i];
			_ftprintf_s(stream,strFormat,t1,t2,t3,dig2->ivray[i]);
		}
	}
	_ftprintf_s( stream, strFormat, 0.,0.,0.,-1 );
	fclose(stream);

	dig2->modifiedtime = getFileModifiedTime(dig2->digName);
	dig2->digNameCache = dig2->digName;

	return TRUE;

}

BOOL CDigitize::myDigSaveAsZeltFFID(CDigitize::DIGITIZE *dig2, CString outFile, double xs, BOOL isMinusXShot,
	int countTrace, int *ptrace, float *pdist, int *pffid, float *poffset)
{
	if (dig2->totCurve < 1 || !dig2->nPoint) return FALSE;
	if (!dig2->isZeltFormat)
		return myDigSaveAs2(dig2, false);

	// now we only need to consider zelt format below
	float t1, t2, t3;
	short n, k;
	int fileFormat = 0;
	FILE *stream;
	long i, j;
	CString tStr;
	if (_tfopen_s(&stream, outFile, _T("w")) != 0) {
		tStr.Format(_T("Failed to open stream to write to for %s"), dig2->digName);
		AfxMessageBox(tStr, MB_ICONQUESTION);
		return FALSE;
	}

//	sort2(countTrace, pdist, ptrace); // this sorting cause problem: it does not sort pffid and poffset!!!
	i = dig2->totCurve;

	myDigSort(dig2);
	CString aNameBak = dig2->digName + ".bak";
	if (isFileExistNotDirectory(aNameBak)) {
		// We first want to put all dig points that are not of the same source, and write as is.
		// We will EXCLUDE any dig points that have the same source as the active one.
		// This way, we can append the active data to the end of the file.
		FILE  *streamBak;

		if (_tfopen_s(&streamBak, aNameBak, _T("r")) != 0) {
			TCHAR line[MAXLINE];
			BOOL firstTim = TRUE, textErr = FALSE;
			while (_fgetts(line, MAXLINE, streamBak) != NULL) {
				if (_tcslen(line) < 2) continue;
				else if (line[0] == '#') {
					// duplicate data except the end line
					_ftprintf_s(stream, _T("%s"), line);
				}
				else if (_stscanf_s(line, _T("%f%f%f%hd"), &t1, &t2, &t3, &k) == 0) {
					textErr = TRUE;
					continue;
				}
				else if (k == 0 && fabs(t2) == 1.0f) {
					// Start one side of the OBS.
					//if here then decide the shot location.
					if (!CCoords::isXShotDefined(xs)) xs = t1;

					if (fabs(xs - t1) <= 0.00001f) {
						// this shot needs to be excluded.
						while (_fgetts(line, MAXLINE, streamBak) != NULL) {
							if (_tcslen(line) < 2) continue;
							if (line[0] == '#') continue; // we do not need comment lines
							if (_stscanf_s(line, _T("%f%f%f%hd"), &t1, &t2, &t3, &k) == 0) break;
							if (k == 0 && (fabs(t2) == 1.0f) &&
								fabs(xs - t1) > 0.00001f) {
								break;
							}
							else if ((fabs(t2) - 1.0f) < 0.00001f && k == -1)
							{ //end of the file.
								break;
							}
						} // while
					}
					if (k >= 0) _ftprintf_s(stream, _T("%s"), line);
				}
				else if (k == -1) {
					//end of the file; we do not want the last line
					// since it will be appended later
					continue;
				}
				else {
					// duplicate data except the end line
					_ftprintf_s(stream, _T("%s"), line);
				}
			}
			fclose(streamBak);

		} // if (streamBak 
	} // if (isFileExistNotDirectory(dig2->digName)

	if (!dig2->strComment.IsEmpty()) {
		if (dig2->strComment.GetAt(0) != _T('#'))
			_ftprintf_s(stream, _T("#"));
		_ftprintf_s(stream, _T("%s"), (LPCTSTR)dig2->strComment);
		int len = dig2->strComment.GetLength();
		if (dig2->strComment.GetAt(len - 1) != _T('\n'))
			_ftprintf_s(stream, _T("\n"));
	}
	else
		_ftprintf_s(stream, _T("#DISTANCE(KM) TIME(S)    DT(S)  RAY_CODE      FFID    OFFSET\n"));


	float anOffset;

	CString strFormat = _T(" %9.3f %9.3f %9.3f %9d %9d %9.3f\n");
	_ftprintf_s(stream, strFormat, xs, -1.f, 0.f, 0, 0, 0.f);
	for (i = 0; i < dig2->totCurve; i++) {
		if (dig2->nPoint[i] < 1) continue;  // we will not save a curve with only 0 point!
		t3 = dig2->curveVal[i];
		for (j = 0; j < dig2->nPoint[i]; j++) {
			t1 = dig2->x[j][i];
			if (dig2->xMode == offset) {
				anOffset = t1;
				t1 = (float)(isMinusXShot ? xs - t1 : xs + t1);
			}
			else {
				anOffset = (float)(isMinusXShot ? t1 + xs : t1 - xs);
			}
			if (t1 > xs) continue;




			int kk = getClosestIndex(pdist, countTrace, t1);
			int kk1, kk2;
			if (kk > 0) {
				kk1 = kk - 1;
				kk2 = kk;
			}
			else {
				kk1 = 0;
				kk2 = 1;
			}
			register double ffid = pffid[kk];
			if (!isFloatEqual(t1, (float)pdist[kk2])) {
				register double dx = pdist[kk2] - pdist[kk1];
				if (!isFloatEqual(dx, 0.)) {
					register double tFac = (t1 - pdist[kk1]) / dx;
					ffid = interpolLinear((double)pffid[kk1], (double)pffid[kk2], tFac);
				}
			}




			t2 = dig2->t[j][i]; //reduction velocity is already accounted for.
			_ftprintf_s(stream, strFormat, t1, t2, t3, dig2->ivray[i], (int)ffid, anOffset);
		}
	}
	_ftprintf_s(stream, strFormat, xs, 1.f, 0.f, 0, 0, 0.f);
	for (i = 0; i < dig2->totCurve; i++) {
		if (dig2->nPoint[i] < 1) continue;  // we will not save a curve with only 0 point!
		t3 = dig2->curveVal[i];
		for (j = 0; j < dig2->nPoint[i]; j++) {
			t1 = dig2->x[j][i];

			if (dig2->xMode == offset) {
				anOffset = t1;
				t1 = (float)(isMinusXShot ? xs - t1 : xs + t1);
			}
			else {
				anOffset = (float)(isMinusXShot ? t1 + xs : t1 - xs);
			}
			if (t1 < xs) continue;



			int kk = getClosestIndex(pdist, countTrace, t1);
			int kk1, kk2;
			if (kk > 0) {
				kk1 = kk - 1;
				kk2 = kk;
			}
			else {
				kk1 = 0;
				kk2 = 1;
			}
			register double ffid = pffid[kk];
			if (!isFloatEqual(t1, (float)pdist[kk2])) {
				register double dx = pdist[kk2] - pdist[kk1];
				if (!isFloatEqual(dx, 0.)) {
					register double tFac = (t1 - pdist[kk1]) / dx;
					ffid = interpolLinear((double)pffid[kk1], (double)pffid[kk2], tFac);
				}
			}



			t2 = dig2->t[j][i];
			_ftprintf_s(stream, strFormat, t1, t2, t3, dig2->ivray[i], (int)ffid, anOffset);
		}
	}
	_ftprintf_s(stream, strFormat, 0., 0., 0., -1, 0., 0.f);
	fclose(stream);

	dig2->modifiedtime = getFileModifiedTime(dig2->digName);
	dig2->digNameCache = dig2->digName;

	return TRUE;

}

/*
-9999.000     0.000     1     1.0300
0.000     0.000     1     1.0300
0.000     3.632     1     1.0300
-9999.000     3.632     1     1.0300
-9999.000     0.000     1     1.0300
0.000     0.000     2     1.0300
35.000     0.000     2     1.0300
40.000     0.000     2     1.0300
40.000     3.689     2     1.0300
35.000     3.689     2     1.0300
28.127     3.683     2     1.0300
18.286     3.674     2     1.0300
9.535     3.650     2     1.0300
0.671     3.632     2     1.0300
0.000     3.632     2     1.0300
*/
BOOL CDigitize::myDigitizingLoadTXT4Col(CDigitize::DIGITIZE * dig2, CStringArray *xzArray)
{
	if (!xzArray || xzArray->GetSize() < 1) return FALSE;

	CString aStr;
	int jArr; // for index of xzArray[]
	CString tStr;
	TCHAR ch[256];
	float x1, t1;
	int ic, ic0;
	BOOL isTextErr = FALSE;
	BOOL isFirstTim = TRUE;
	int jj = 0;  // for index of dig[jj][i]
	int ncurve = 0;
	int npoint = 0;

	//first decide how big the arrays should be.
	// NOTE#### How to decide if a new curve is encountered? It must be a different integer to mark up for new curves!!!!
	//jj = (int)xzArray->GetSize(); // testing line
	//if (jj <= 0) {
	//	return FALSE;
	//}

	// CString sToken = BLANK;
	for (jArr = 0; jArr<xzArray->GetSize(); jArr++) {
		aStr = xzArray->ElementAt(jArr).Trim();

		if (aStr.GetAt(0) != '-' && (aStr.GetAt(0)<'0' || aStr.GetAt(0)>'9')) continue;
		if (aStr.GetLength() < 2) continue;
		if (aStr.Find(_T(",")) > 0) {
			aStr.Replace(_T(","), SPACE);
		}

		if (_stscanf_s(aStr, _T("%f%f%d %s"), &x1, &t1, &ic, ch, (unsigned)_countof(ch)) == 0)
			isTextErr = TRUE;
		tStr = CString(ch).Trim();

		int tlen = tStr.GetLength();

		if (isTextErr) continue;

		if (isFirstTim) {
			isFirstTim = FALSE;
			jj++;
			ic0 = ic;
		}
		else if (ic != ic0) {
			//now a new curve is encountered and has already been read in.
			if (npoint<jj) npoint = jj;
			ncurve++;
			jj = 1; //must be 1 instead of 0 since already read in.
			ic0 = ic;
		}
		else  {
			jj++;
		}
	} // for jArr
	if (npoint<jj) npoint = jj;
	ncurve++;

	if (npoint > 20000) npoint = 20000;
	if (npoint < 1) {
		return FALSE;
	}

	CDigitize pDg;
	if (!dig2->initDigitizeArray(npoint, ncurve)) {
		//tStr.Format(_T("Sorry, cannot initialize digitizing array with ncurve=%d and npoint=%d.."), ncurve, npoint);
		//myCaption(tStr);
		return FALSE;
	}

	int ii = 0;
	jj = 0;
	// tStr0 = BLANK;
	isFirstTim = TRUE;
	for (jArr = 0; jArr<xzArray->GetSize(); jArr++) {
		aStr = xzArray->ElementAt(jArr).Trim();
		if (aStr.GetAt(0) != '-' && (aStr.GetAt(0)<'0' || aStr.GetAt(0)>'9')) continue;
		if (aStr.GetLength() < 2) continue;
		if (aStr.Find(_T(",")) > 0) {
			aStr.Replace(_T(","), SPACE);
		}

		if (_stscanf_s(aStr, _T("%f%f%d %s"), &x1, &t1, &ic, ch, (unsigned)_countof(ch)) == 0)
			isTextErr = TRUE;
		tStr = CString(ch).Trim();

		dig2->x[jj][ii] = x1;
		dig2->t[jj][ii] = t1;
		dig2->curveVal[ii] = (float)StringToFloat(tStr); // necessary to repeat this assignment

		if (isFirstTim) {
			isFirstTim = FALSE;
			jj++;
			ic0 = ic;
		}
		else if (ic != ic0) {
			//now a new curve is encountered and has already been read in.
			dig2->nPoint[ii] = jj;
			ii++;

			// we have to re-assign the values since the curve number is incremented
			dig2->labelArr->Add(tStr);
			dig2->x[0][ii] = x1;
			dig2->t[0][ii] = t1;

			//now the label has been overwritten so need to recover (only one label for each curve).
			ic0 = ic;
			jj = 1;
		}
		else jj++;
	} // for jArr

	//now write the last set.
	dig2->nPoint[ii] = jj;
	dig2->totCurve = ii + 1;

	// note: we cannot change dig2->iCurveSel since some routines rely on it to determine where to add more points
	if (dig2->iCurveSel <0 || dig2->iCurveSel > dig2->totCurve - 1)
		dig2->iCurveSel = dig2->totCurve - 1;

	return TRUE;
}




//BOOL CDigitize::myDigitizingLoadTXTUnix(CDigitize::DIGITIZE * dig2) {
//	return myDigitizingLoadTXTUnix2(dig2);
//}

// private function
void CDigitize::privateGetDigSize(CDigitize::DIGITIZE * dig2, CStringArray *xzArray, BOOL isY3, 
	int &numItems, int &ncurve, int &npoint)
{
	int jArr; // for index of xzArray[]
	CString aStr;
	CString tStr, tStr0;
	TCHAR ch[256];
	float x1, t1;
	float t0;
	BOOL isTextErr = FALSE;
	BOOL isFirstTim = TRUE;
	int jj = 0;  // for index of dig[jj][i]


	for (jArr = 0; jArr < xzArray->GetSize(); jArr++) {
		aStr = xzArray->ElementAt(jArr).Trim();
		//if (aStr.GetAt(0) == '>') {
		//	// this is a GMT type data file
		//	return privateGetDigSizeGMT(dig2, xzArray, ncurve, npoint);
		//}

		if (aStr.GetAt(0) != '-' && (aStr.GetAt(0) < '0' || aStr.GetAt(0) > '9')) continue;
		if (aStr.GetLength() < 2) continue;
		if (aStr.Find(_T(",")) > 0) {
			aStr.Replace(_T(","), SPACE);
		}
		if (aStr.Find(TAB) > 0) {
			aStr.Replace(TAB, SPACE);
		}

		if (numItems == 0) {
			numItems = SplitString(aStr, SPACE);
			break;
		}
	}


	for (jArr = 0; jArr<xzArray->GetSize(); jArr++) {
		aStr = xzArray->ElementAt(jArr).Trim();
		if (aStr.GetAt(0) != '-' && (aStr.GetAt(0)<'0' || aStr.GetAt(0)>'9')) continue;
		if (aStr.GetLength() < 2) continue;
		if (aStr.Find(_T(",")) > 0) {
			aStr.Replace(_T(","), SPACE);
		}
		if (aStr.Find(TAB) > 0) {
			aStr.Replace(TAB, SPACE);
		}

		if (isY3) {
			// if (_stscanf_s(aStr,_T("%f%f%f %[^\n]"),&x1, &t0, &t1, ch, _countof(ch))==0) isTextErr=TRUE; 
			if (_stscanf_s(aStr, _T("%f%f%f %s"), &x1, &t0, &t1, ch, (unsigned)_countof(ch)) == 0) isTextErr = TRUE;
		}
		else {
			if (numItems > 2) {
				if (_stscanf_s(aStr, _T("%f%f %s"), &x1, &t1, ch, (unsigned)_countof(ch)) == 0)
					isTextErr = TRUE;
				tStr = CString(ch).Trim();
			}
			else {
				if (_stscanf_s(aStr, _T("%f%f"), &x1, &t1) == 0)
					isTextErr = TRUE;
				tStr = _T("Point");
			}
		}

		int tlen = tStr.GetLength();

		if (isTextErr) continue;

		if (isFirstTim || isY3) {
			isFirstTim = FALSE;
			jj++;
			tStr0 = tStr;
		}
		else if (tStr.Compare(tStr0) != 0) {
			//now a new curve is encountered and has already been read in.
			if (npoint<jj) npoint = jj;
			ncurve++;
			jj = 1; //must be 1 instead of 0 since already read in.
			tStr0 = tStr;
		}
		else  {
			jj++;
		}
	} // for jArr
	if (npoint<jj) npoint = jj;
	ncurve++;
	if (npoint > NP_MAX) npoint = NP_MAX;

}

void CDigitize::privateGetDigSize_GMT(CStringArray *xzArray, 
	int &ncurve, int &npoint)
{
	int j; // for index of xzArray[]
	CString aStr;
	ncurve = 0;
	int count = 0;
	for (j = 0; j<xzArray->GetSize(); j++) {
		aStr = xzArray->ElementAt(j).Trim();
		if (aStr.GetAt(0) == '>') {
			npoint = max(count, npoint);
			ncurve++;
			count = 0;
		}

		if (aStr.GetAt(0) != '-' && (aStr.GetAt(0)<'0' || aStr.GetAt(0)>'9')) continue;
		if (aStr.GetLength() < 2) continue;
		count++;
	} // for jArr
	npoint = max(count, npoint);
	if (npoint > NP_MAX) npoint = NP_MAX;
	if (ncurve > MAXLINE) ncurve = MAXLINE;
}

BOOL CDigitize::myDigitizingLoadTXT_GMT(CDigitize::DIGITIZE * dig2, CStringArray *xzArrayFrom)
{
	if (!xzArrayFrom || xzArrayFrom->GetSize() < 1) return FALSE;
	CString aStr, bStr;

	int ncurve = 0;
	int npoint = 0;
	privateGetDigSize_GMT(xzArrayFrom, ncurve, npoint); // numItems, ncurve, npoint will be modified here
	if (npoint < 1) {
		return FALSE;
	}

	CDigitize pDg;
	if (!dig2->initDigitizeArray(npoint, ncurve)) {
		return FALSE;
	}

	int numCol = getTxtNumColumns(xzArrayFrom->ElementAt(0));

	TCHAR sbuff[256];
//	TCHAR ch[256];
	float x1, t1;
	int ii = -1;
	int jj = 0;  // for index of dig[jj][i]
	for (int jArr = 0; jArr<xzArrayFrom->GetSize(); jArr++) {
		if (jj >= NP_MAX) break;
		aStr = xzArrayFrom->ElementAt(jArr).Trim();
		if (aStr.GetAt(0) == '>') {
			if (ii>=0) dig2->nPoint[ii] = jj;
			ii++;
			dig2->labelArr->Add(aStr.Mid(1).Trim());
			dig2->curveVal[ii] = (float)ii;
			jj = 0;
		}
		if (ii < 0) ii = 0;  // this is a fix: when a file does not contain a line with >
		if (aStr.GetAt(0) != '-' && (aStr.GetAt(0)<'0' || aStr.GetAt(0)>'9')) continue;
		if (aStr.GetLength() < 2) continue;
		if (ii < 0) continue; // will not read any data until a new curve is defined
		if (ii > dig2->digLayerMax - 1 || jj > dig2->digPointMax - 1) break; // avoid out of bounds index
		if (aStr.Find(_T(",")) > 0) {
			aStr.Replace(_T(","), SPACE);
		}
		else if (aStr.Find(TAB) > 0) {
			aStr.Replace(TAB, SPACE);
		}


		try {
			if (numCol > 2) {
				if (_stscanf_s(aStr, _T("%f%f%s"), &x1, &t1, &sbuff, (unsigned)_countof(sbuff)) == 0) {
					continue;
				}
				if (ii == 0) {
					// only when reading the first curve, it is allowed to have 
					// a label for each point
					bStr = CString(sbuff).Trim();
					dig2->labelArr->Add(bStr);
					dig2->curveVal[ii] = (float)ii;
				}
			}
			else {
				if (_stscanf_s(aStr, _T("%f%f"), &x1, &t1) == 0) {
					continue;
				}
			}


			dig2->x[jj][ii] = x1;
			dig2->t[jj][ii] = t1;
			jj++;
		}
		catch (...) {
			// it's ok to have exception for a line
		}
	} // for jArr

	//now write the last set.
	dig2->nPoint[ii] = jj;
	dig2->totCurve = ii + 1;

	// note: we cannot change dig2->iCurveSel since some routines rely on it to determine where to add more points
	if (dig2->iCurveSel <0 || dig2->iCurveSel > dig2->totCurve - 1)
		dig2->iCurveSel = dig2->totCurve - 1;

	return TRUE;
}

BOOL CDigitize::myDigitizingLoadTXTUnix2(CDigitize::DIGITIZE * dig2, CStringArray *xzArray, BOOL isY3)
{
	if (!xzArray || xzArray->GetSize() < 1) return FALSE;
	CString aStr;
	for (int jArr = 0; jArr < xzArray->GetSize(); jArr++) {
		aStr = xzArray->ElementAt(jArr).Trim();
		if (aStr.GetAt(0) == '>') {
			// this is a GMT type data file
			return myDigitizingLoadTXT_GMT(dig2, xzArray);
		}
	}

	int numItems = 0;
	int ncurve = 0;
	int npoint = 0;
	privateGetDigSize(dig2, xzArray, isY3, numItems, ncurve, npoint); // numItems, ncurve, npoint will be modified here
	if (npoint < 1) {
		return FALSE;
	}
	if (numItems == 4) {
		return myDigitizingLoadTXT4Col(dig2, xzArray);
	}

	CDigitize pDg;
	if (!dig2->initDigitizeArray(npoint, ncurve)) {
		return FALSE;
	}

	TCHAR ch[256];
	float x1, t1;
	float t0;

	int ii = 0;
	int jj = 0;  // for index of dig[jj][i]
	CString tStr;
	CString tStr0 = BLANK;
	BOOL isTextErr = FALSE;
	BOOL isFirstTim = TRUE;
	for (int jArr = 0; jArr<xzArray->GetSize(); jArr++) {
		if (jj >= NP_MAX) break;
		aStr = xzArray->ElementAt(jArr).Trim();
		if (aStr.GetAt(0) != '-' && (aStr.GetAt(0)<'0' || aStr.GetAt(0)>'9')) continue;
		if (aStr.GetLength() < 2) continue;
		if (aStr.Find(_T(",")) > 0) {
			aStr.Replace(_T(","), SPACE);
		}
		tStr = BLANK; // need something there otherwise it cannot be set to CStringArray

		if (isY3) {
			if (_stscanf_s(aStr, _T("%f%f%f %s"), &x1, &t0, &t1, ch, (unsigned)_countof(ch)) == 0) continue;
		}
		else {
			if (numItems > 2) {
				if (_stscanf_s(aStr, _T("%f%f %s"), &x1, &t1, ch, (unsigned)_countof(ch)) == 0)
					isTextErr = TRUE;
				tStr = CString(ch).Trim();
			}
			else {
				if (_stscanf_s(aStr, _T("%f%f"), &x1, &t1) == 0)
					isTextErr = TRUE;
			}
		}

		dig2->x[jj][ii] = x1;
		dig2->t[jj][ii] = t1;

		if (isFirstTim || isY3) {
			isFirstTim = FALSE;
			jj++;
			dig2->labelArr->Add(tStr);
			
			dig2->curveVal[0] = IsNumeric(tStr) ? (float)StringToFloat(tStr) : 0.f;
			tStr0 = tStr;
		}
		else if (tStr.Compare(tStr0) != 0) {
			//now a new curve is encountered and has already been read in.
			dig2->nPoint[ii] = jj;
			ii++;

			// we have to re-assign the values since the curve number is incremented
			dig2->labelArr->Add(tStr);
			dig2->curveVal[ii] = IsNumeric(tStr) ? (float)StringToFloat(tStr) : (float)ii;
			dig2->x[0][ii] = x1;
			dig2->t[0][ii] = t1;

			//now the label has been overwritten so need to recover (only one label for each curve).
			tStr0 = tStr;
			jj = 1;
		}
		else jj++;
	} // for jArr

	//now write the last set.
	dig2->nPoint[ii] = jj;
	dig2->totCurve = ii + 1;

	// note: we cannot change dig2->iCurveSel since some routines rely on it to determine where to add more points
	if (dig2->iCurveSel <0 || dig2->iCurveSel > dig2->totCurve - 1)
		dig2->iCurveSel = dig2->totCurve - 1;

	return TRUE;
}


BOOL CDigitize::myDigitizingLoadTXTUnix2(CDigitize::DIGITIZE * dig2, BOOL isY3) {
	//  *****NOTE: any prevous initialization of dig2 will be removed in this routine!!!*****
	// NOTE: isY3 is introduced to read Y data which is not at the conventional second column but indexed at 3rd column
	CString f = dig2->digName;
	//if (!isFileExistNotDirectory(f)) {
	//	// this part is needed in case the dig() already exists but its file name is already removed by the user
	// DO NOT DO IT!!!  May 27, 2020
	//	dig2->freeDigAll();
	//	return FALSE;
	//}
	CTime aTimeFile = getFileModifiedTime(f);
	if (dig2->modifiedtime>0 && 
		dig2->digNameCache.Compare(f)==0 &&
		dig2->modifiedtime >= aTimeFile) return TRUE;
	
	CString tStr;
	if (dig2->modifiedtime>0 && aTimeFile > dig2->modifiedtime+100) {
		tStr.Format(_T("File on disk \n%s\n is newer than in memory. Do you want to load it and ignore any newer editting?"), f);
		if (AfxMessageBox(tStr, MB_YESNO | MB_ICONQUESTION) != IDYES) {
			dig2->modifiedtime = aTimeFile; // this line will make it silent
			return TRUE;
		}
	}

	if (dig2->modifiedtime == 0 && dig2->totCurve > 0)
		return FALSE; // this case has a valid model to be saved. So we cannot load the model from file; otherwise the un-saved info will be lost.

	// note: if a file is locked by Excel, getFileModifiedTime(f) returns 0!!! 

	CString fExt = GetFileExtension(f);
	if (fExt == _T("in")) return FALSE;   // if a Zelt format is read in as text format, it would cause exception
	
	if (fExt == _T("shp") || fExt == _T("shx") || fExt == _T("prj")) {
		SEI_PAIR pmin, pmax;
		if (!this->SHPOpenToDig(f, dig2, pmin, pmax)) return FALSE;
		dig2->modifiedtime = aTimeFile;
		return TRUE;
	}

	CStringArray *xzArray = new CStringArray();
	getLinesFromFile(xzArray, f, false, 0);
	if (!xzArray) return FALSE;
	BOOL isTrue = myDigitizingLoadTXTUnix2(dig2, xzArray, isY3);
	xzArray->RemoveAll();
	delete xzArray;
	xzArray = NULL;
	
	if (isTrue) {
		dig2->modifiedtime = aTimeFile;
		dig2->digNameCache = f;
	}
	return isTrue;
}

// will return the last xShot value. If -99999., it means no valid data read in
void CDigitize::myDigLoadZelt(CDigitize::DIGITIZE *dig2, double xs, bool isXsForced) {
	if (dig2->modifiedtime == getFileModifiedTime(dig2->digName) && 
		dig2->digNameCache == dig2->digName &&
		isFloatEqual3(dig2->xShotCache, (float)xs))
		return;
	

	if (dig2->modifiedtime == 0 && dig2->totCurve > 0)
		return; // this case has a valid model to be saved. So we cannot load the model from file; otherwise the un-saved info will be lost.

	CStringArray arr;  // do not make it as a pointer otherwise compiler will complain since it is not initialized
	getLinesFromFile(&arr, dig2->digName, true, 0);
	if (arr.GetSize() < 1) return;

	int i,k, k0, ncurve;
	float t1,t2,t3, t30;
	CString tStr;

	if (!CCoords::isXShotDefined(xs) && isXsForced) {
		return;
	}

	int i1=-1, i2=-1;  // find the starting and ending indexes for the current xShot


	if (CCoords::isXShotDefined(xs)) {
		for (i = 0; i < arr.GetSize(); i++) {
			tStr = arr.ElementAt(i);
			if (tStr.GetLength() < 2 || tStr.GetAt(0) == '#') continue;  // ignore any comment lines for now
			if (_stscanf_s(tStr, _T("%f%f%f%d"), &t1, &t2, &t3, &k) == 0) continue;
			if (k == 0 && fabs(t2) == 1.0f && isFloatEqual3((float)xs, t1)) {
				i1 = i;
				break;
			}
		}

		if (i1 < 0) {
			arr.RemoveAll();
			return;
		}
		i2 = -1; // initialize
		for (i = i1 + 1; i < arr.GetSize(); i++) {
			tStr = arr.ElementAt(i);
			if (tStr.GetLength() < 2 || tStr.GetAt(0) == '#') continue;  // ignore any comment lines for now
			if (_stscanf_s(tStr, _T("%f%f%f%d"), &t1, &t2, &t3, &k) == 0) continue;
			if (k == 0 && fabs(t2) == 1.0f && !isFloatEqual3((float)xs, t1)) {
				// another source found, so we shall not read any more lines
				i2 = i;
				break;
			}
			else if (k == -1 && isFloatEqual3(0.f, t2)) {
				// last line encountered
				i2 = i;
				break;
			}
		}
	}

	if (i1 < 1) i1 = 1;
	if (i2 < i1) i2 = arr.GetSize() - 1;

	if (!dig2->initDigitizeArray(999, 99)) {
		arr.RemoveAll();
		return;
	}


	// get all the comment lines before i1 index and use the strings as the general intro
	int i0 = 0;
	for (i=i1-1; i>=0; i--) {
		tStr = arr.ElementAt(i);
		if (tStr.GetLength() > 1 && tStr.GetAt(0) != '#') {
			i0 = i+1;
			break;
		}
	}
	dig2->strComment = BLANK;
	for (i=i0; i<i1; i++)
		dig2->strComment += arr.ElementAt(i);  // strComment already contains '\n' 


	// read all the data between i1 and i2
	k0 = -99999;
	t30 = (float)k0;
	for (i=i1; i<i2; i++) {
		tStr = arr.ElementAt(i);
		if (_stscanf_s(tStr,_T("%f%f%f%d"),&t1, &t2, &t3, &k)==0) continue;
		if (isFloatEqual3((float)t2, 9999.f)) continue; // Helen's convention for a dead point
		if ((k == 0 && isFloatAbsEqual(t2, 1.0f)) && k == 0) {
			// starting a OBS siding. NOTE:
			// it's possible that xs has no valid values, which can happen when no segy data on screen and only digitized points onscreen.
			xs = (float)t1;
		}
		else if(k!=k0 || t3!=t30 ) {
			// starting a new curve. NOTE:
			dig2->myDigAddCurve(xs);
		}

		if (k==0) continue;  // only when k==0, the line does not contain any points

		ncurve = dig2->totCurve - 1;
		this->myDigAddPoint(dig2, ncurve, t1, t2, xs);  // this does not add digResolution[] and ivray[]
		dig2->curveVal[ncurve] = t3;
		dig2->ivray[ncurve] = k; // ray code
		k0 = k;
		t30 = t3;
	}
	arr.RemoveAll();

	// remove all curves that have <=1 points
	int ic = 0;
	for (ic = 0; ic < dig2->totCurve; ic++) {
		if (dig2->nPoint[ic] < 1) {
			this->digDeleteCurve(dig2, ic); // ???
			//after a curve is removed, the nPoint[] is less, and we may have risk of dead loop
		}
	}

	// note: we cannot change dig2->iCurveSel since some routines rely on it to determine where to add more points
	if (dig2->iCurveSel <0 || dig2->iCurveSel > dig2->totCurve-1)
		dig2->iCurveSel = dig2->totCurve-1;

	// need to fix up 
	// consider: if a ray code dig2->ivray[ncurve]  is duplicated 
	// we then want to make up a different ray code for each 
	//for (ic = 0; ic < dig2->totCurve; ic++) {
	//	for (i = ic + 1; i < dig2->totCurve; i++) {
	//		if (dig2->ivray[ic] == dig2->ivray[i]) {
	//			// duplicated ray code
	//			dig2->ivray[i] += 100;
	//			break;
	//		}
	//	}
	//}

	dig2->xShotCache = (float)xs;
	dig2->modifiedtime = getFileModifiedTime(dig2->digName);
	dig2->digNameCache = dig2->digName;
	return;
}


bool CDigitize::getDigLimits(CDigitize::DIGITIZE *dig2, double &xmin, double &xmax, double &tmin, double &tmax)
{
	if (dig2->totCurve < 1) return false;
	int n = dig2->totCurve;
	double *xminArr = (double *)ealloc1double(n);
	double *xmaxArr = (double *)ealloc1double(n);
	double *tminArr = (double *)ealloc1double(n);
	double *tmaxArr = (double *)ealloc1double(n);

	int i, j;
	for (i = 0; i < n; i++) {
		int np = dig2->nPoint[i];
		double *vals = (double *)ealloc1double(np);

		for (j = 0; j < np; j++) {
			vals[j] = dig2->x[j][i];
		}
		xminArr[i] = getMinValue(vals, np, MAXAMP);
		xmaxArr[i] = getMaxValue(vals, np);

		for (j = 0; j < np; j++) {
			vals[j] = dig2->t[j][i];
		}
		tminArr[i] = getMinValue(vals, np, MAXAMP);
		tmaxArr[i] = getMaxValue(vals, np);
		free1double(vals);
	}

	xmin = getMinValue(xminArr, n, MAXAMP);
	xmax = getMaxValue(xmaxArr, n);
	tmin = getMinValue(tminArr, n, MAXAMP);
	tmax = getMaxValue(tmaxArr, n);

	//double amin = MIL*1000.0;
	//for (i=0; i<dig2->totCurve; i++) {
	//	if (amin > dig2->x[0][i]) {
	//		amin = dig2->x[0][i];
	//	} // if
	//}
	//*xmin = amin;

	//double amax = -MIL*1000.0;
	//for (i=0; i<dig2->totCurve; i++) {
	//	n = dig2->nPoint[i];
	//	if (n>0) {
	//		if (amax < dig2->x[n-1][i]) {
	//			amax = dig2->x[n-1][i];
	//		} // if
	//	}
	//}
	//*xmax = amax;

	//amin = MIL*1000.0;
	//for (i=0; i<dig2->totCurve; i++) {
	//	n = dig2->nPoint[i];
	//	for (j=0; j<n; j++) {
	//		if (amin > dig2->t[j][i]) {
	//			amin = dig2->t[j][i];
	//		} // if
	//	}
	//}
	//*tmin = amin;

	//amax = -MIL*1000.0;
	//for (i=0; i<dig2->totCurve; i++) {
	//	n = dig2->nPoint[i];
	//	for (j=0; j<n; j++) {
	//		if (amax < dig2->t[j][i]) {
	//			amax = dig2->t[j][i];
	//		} // if
	//	}
	//}
	//*tmax = amax;

	free1double(xminArr);
	free1double(xmaxArr);
	free1double(tminArr);
	free1double(tmaxArr);
	return true;
}

//
//float CDigitize::getDigYMin(CDigitize::DIGITIZE *dig2) {
//	// this function does not call any global variables, can be moved to util.cpp
//	//         if the struct CDigitize::DIGITIZE is moved to util.h
//	float amin = MIL;
//	for (int i=0; i<dig2->totCurve; i++) {
//		for (int j=0; j<dig2->nPoint[i]; j++) {
//			//if (_isnan((double)dig2->t[j][i])) {
//			//	int sdfsd = 1;
//			//}
//			if (amin > dig2->t[j][i]) {
//				amin = dig2->t[j][i];
//			} // if
//		} // j
//	}
//	return amin;
//}

//float CDigitize::getDigYMax(CDigitize::DIGITIZE *dig2) {
//	// this function does not call any global variables, can be moved to util.cpp
//	float amax = -MIL;
//	for (int i=0; i<dig2->totCurve; i++) {
//		for (int j=0; j<dig2->nPoint[i]; j++) {
//			if (amax < dig2->t[j][i]) {
//				amax = dig2->t[j][i];
//			} // if
//		} // j
//	}
//	return amax;
//}


float CDigitize::getDigXfromTLinear(CDigitize::DIGITIZE *dig2, float t) {
	// this function does not call any global variables, can be moved to util.cpp
	float x = dig2->x[0][0];
	int n = dig2->nPoint[0];
	if (n<1) return x;
	if (t >= dig2->t[n-1][0]) 
		return dig2->x[n-1][0];

	for (int j=0; j<n-1; j++) {
		if (isInsideFloat(t, dig2->t[j][0], dig2->t[j+1][0])) {
			// we now need to interpolate
			register float dt = dig2->t[j+1][0] - dig2->t[j][0];
			if (dt == 0.f) {
				x = dig2->x[j][0];
			}
			else {
				x = dig2->x[j][0] + (dig2->x[j+1][0] - dig2->x[j][0]) * (t - dig2->t[j][0])/dt;
			}
			break;
		} // if
	} // j
	return x;
}


float CDigitize::getDigTfromXLinear(CDigitize::DIGITIZE *dig2, float x) {
	float t = dig2->t[0][0];
	int n = dig2->nPoint[0];
	if (n<1) return t;
	if (x >= dig2->x[n - 1][0])
		return dig2->t[n - 1][0];

	for (int j = 0; j<n - 1; j++) {
		if (isInsideFloat(x, dig2->x[j][0], dig2->x[j + 1][0])) {
			// we now need to interpolate
			register float dx = dig2->x[j + 1][0] - dig2->x[j][0];
			if (dx == 0.f) {
				t = dig2->t[j][0];
			}
			else {
				t = dig2->t[j][0] + (dig2->t[j + 1][0] - dig2->t[j][0]) * (x - dig2->x[j][0]) / dx;
			}
			break;
		} // if
	} // j
	return t;
}

float CDigitize::getDigTfromXSpline(CDigitize::DIGITIZE *dig2, float x, int ic) {
	// ref:  https://en.wikipedia.org/wiki/Cubic_Hermite_spline
	// NOTE: this function assumes monotonical in dig2->x[][ic]
	int n = dig2->nPoint[ic];
	if (n<1) return dig2->t[0][ic];

	//if (x >= dig2->x[n - 1][ic])
	//	return dig2->t[n - 1][ic];

	if (x <= dig2->x[0][ic]) {
		float dx = dig2->x[1][ic] - dig2->x[n][ic];
		if (dx == 0.f) return dig2->t[0][ic];
		return dig2->t[0][ic] +
			(dig2->t[1][ic] - dig2->t[0][ic]) *
			(x - dig2->x[0][ic]) / dx;
	} else if (x >= dig2->x[n - 1][ic]) {
		float dx = dig2->x[n - 1][ic] - dig2->x[n - 2][ic];
		if (dx == 0.f) return dig2->t[n - 1][ic];
		return dig2->t[n - 2][ic] +
			(dig2->t[n - 1][ic] - dig2->t[n - 2][ic]) *
			(x - dig2->x[n - 2][ic]) / dx;
	}




	for (int j = 0; j<n - 1; j++) {
		if (isInsideFloat(x, dig2->x[j][ic], dig2->x[j + 1][ic])) {
			// we now need to interpolate
			register float dx = dig2->x[j + 1][ic] - dig2->x[j][ic];
			if (dx == 0.f) {
				return dig2->t[j][ic];
			}
			register double t = (x - dig2->x[j][ic]) / dx;
			register double h00 = (1. + 2.*t)*(1. - t)*(1. - t);
			register double h10 = t*(1. - t)*(1. - t);
			register double h01 = t*t*(3. - 2.*t);
			register double h11 = t*t*(t-1.);
			register double pk = dig2->t[j][ic];
			register double pk1 = dig2->t[j + 1][ic];

			register double mkNear = (pk1 - pk) / dx;
			register double mk = mkNear;
			if (j - 1 >= 0) {
				register float dx0 = dig2->x[j][ic] - dig2->x[j - 1][ic];
				register double pk0 = dig2->t[j - 1][ic];
				if (dx0 != 0.) {
					mk += (pk - pk0) / dx0;
					mk *= 0.5;
				}
			}

			register double mk1 = mkNear;
			if (j + 2 < n) {
				register float dx2 = dig2->x[j + 2][ic] - dig2->x[j + 1][ic];
				register double pk2 = dig2->t[j + 2][ic];
				if (dx2 != 0.) {
					mk1 += (pk2 - pk1) / dx2;
					mk1 *= 0.5;
				}
			}

			return (float)(h00*pk + h10*dx*mk + h01*pk1 + h11*dx*mk1);
		} // if
	} // j
	return dig2->t[0][ic];
}

float CDigitize::getDigXfromTSpline(CDigitize::DIGITIZE *dig2, float y, int ic) {
	// ref:  https://en.wikipedia.org/wiki/Cubic_Hermite_spline
	// NOTE: this function assumes monotonical in dig2->t[][ic]
	int n = dig2->nPoint[ic];
	if (n < 1) {
		return dig2->x[0][ic];
	}
	if (y <= dig2->t[0][ic]) {
		float dy = dig2->t[1][ic] - dig2->t[0][ic];
		if (dy == 0.f) return dig2->x[0][ic];
		return dig2->x[0][ic] +
			(dig2->x[1][ic] - dig2->x[0][ic]) *
			(y - dig2->t[0][ic]) / dy;
	}
	else if (y >= dig2->t[n - 1][ic]) {
		float dy = dig2->t[n - 1][ic] - dig2->t[n - 2][ic];
		if (dy == 0.f) return dig2->x[n - 1][ic];
		return dig2->x[n - 2][ic] + 
			(dig2->x[n - 1][ic] - dig2->x[n - 2][ic]) * 
			(y - dig2->t[n - 2][ic]) / dy;
	}

	for (int j = 0; j<n - 1; j++) {
		if (isInsideFloat(y, dig2->t[j][ic], dig2->t[j + 1][ic])) {
			// we now need to interpolate
			register float dy = dig2->t[j + 1][ic] - dig2->t[j][ic];
			if (dy == 0.f) {
				return dig2->x[j][ic];
			}
			register double t = (y - dig2->t[j][ic]) / dy;  // this t is not time!!!
			register double h00 = (1. + 2.*t)*(1. - t)*(1. - t);
			register double h10 = t*(1. - t)*(1. - t);
			register double h01 = t*t*(3. - 2.*t);
			register double h11 = t*t*(t - 1.);
			register double pk = dig2->x[j][ic];
			register double pk1 = dig2->x[j + 1][ic];

			register double mkNear = (pk1 - pk) / dy;
			register double mk = mkNear;
			if (j - 1 >= 0) {
				register float dy0 = dig2->t[j][ic] - dig2->t[j - 1][ic];
				register double pk0 = dig2->x[j - 1][ic];
				if (dy0 != 0.) {
					mk += (pk - pk0) / dy0;
					mk *= 0.5;
				}
			}

			register double mk1 = mkNear;
			if (j + 2 < n) {
				register float dy2 = dig2->t[j + 2][ic] - dig2->t[j + 1][ic];
				register double pk2 = dig2->x[j + 2][ic];
				if (dy2 != 0.) {
					mk1 += (pk2 - pk1) / dy2;
					mk1 *= 0.5;
				}
			}

			return (float)(h00*pk + h10*dy*mk + h01*pk1 + h11*dy*mk1);
		} // if
	} // j
	return dig2->x[0][ic];
}

int CDigitize::getCountNonMonoX(CDigitize::DIGITIZE *dig2, int ic) {
	// working
	int count = 0;
	if (ic < 0 || ic>= dig2->totCurve) return count;
	int n = dig2->nPoint[ic];
	if (n<1) return count;

	for (int j = 0; j<n - 1; j++) {
		if (dig2->x[j][ic] > dig2->x[j + 1][ic])
			count++;
	} // j
	return count;
}


// default layer number to return is 0;
int CDigitize::getClosestDigNum(CDigitize::DIGITIZE *digm, SEI_PAIR pNew)
{
	int iCurveOut = 0;
	if (digm->totCurve < 1) return iCurveOut;

	double distMin = MIL;
	for (int i=0; i<digm->totCurve; i++) {
		int n = digm->nPoint[i];
		if (n<1) continue;

		for (int j=0; j<n-1; j++) {
			if (pNew.x >= digm->x[j][i] && pNew.x <= digm->x[j+1][i]) {
				register double z = digm->t[j][i];
				register double z1 = digm->t[j+1][i];
				register double a = FindShortestDistanceToSegment((double)digm->x[j][i], z,  (double)digm->x[j+1][i], z1, pNew.x, pNew.y);
				if (distMin > a) {
					// found a closer boundary to use
					distMin = a;
					iCurveOut = i;
				}
				break;  // finished processing for this loop
			}
		} // for j
	} // for i

	return iCurveOut;
}

// This uses isFloatEqual() to determine exact index, at a system defined resolution of cm, or 0.00001 km
int CDigitize::getDigIndexExact(CDigitize::DIGITIZE *dig, int ic, float x, float xres) {
	if (ic<0 || ic > dig->totCurve - 1) return -1;
	int na = -1;
	for (int j = 0; j < dig->nPoint[ic]; j++) {
		//if (isFloatEqual(x, dig->x[j][ic]), xres) {
		//	na = j;
		//	break;
		//} // if
		if (isFloatEqual3(x, dig->x[j][ic])) {
			na = j;
			break;
		} // if
	} // for j
	return na;
}

bool CDigitize::digitize2Kml(CDigitize::DIGITIZE *dig, CString fOut, bool isLine)
{
	if (dig->totCurve < 1) return false;

	MSXML::IXMLDOMDocument2Ptr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		AfxMessageBox(_T("Failed to create the XML class instance"), MB_ICONQUESTION);
		return false;
	}

	if (pXMLDoc->loadXML(_T("<kml></kml>")) == VARIANT_FALSE) {
		ShowError(pXMLDoc);
		return false;
	}

	//Get the root element just created	
	MSXML::IXMLDOMElementPtr pXMLRootElem = pXMLDoc->GetdocumentElement();
	MSXML::IXMLDOMProcessingInstructionPtr pXMLProcessingNode =
		pXMLDoc->createProcessingInstruction("xml", " version='1.0' encoding='UTF-8'");

	pXMLRootElem->setAttribute(_T("xmlns"), _variant_t("http://www.opengis.net/kml/2.2"));
	MSXML::IXMLDOMElementPtr pElem = pXMLDoc->createElement(_T("Document"));
	pXMLRootElem->appendChild(pElem);
	MSXML::IXMLDOMElementPtr p = pXMLDoc->createElement(_T("name"));
	p->Puttext(_bstr_t("KmlFile for Seiswide's digitizing format"));
	pElem->appendChild(p);
	p = pXMLDoc->createElement(_T("description"));
	p->Puttext(_bstr_t("This file contains the navigation information..."));
	pElem->appendChild(p);

	CString aStyle = _T("_dig");
	appendPlacemarkStyle(pXMLDoc, pElem, aStyle, 0.6);
	aStyle = _T("#m") + aStyle;

	//p = pXMLDoc->createElement(_T("styleUrl"));
	//p->Puttext(_bstr_t(aStyle));
	//pElem->appendChild(p);

	// int nLines = dig->totCurve;
	CString comment, tStr;
	int nt, j;
	//if (dig->totCurve == 1 || (dig->totCurve>1 && dig->nPoint[0]==1)) {
	if (!isLine) {
		for (int n = 0; n < dig->totCurve; n++) {
			comment = dig->getLabelString(n);
			if (!comment.IsEmpty()) comment += _T("-");
			nt = dig->nPoint[n];
			for (j = 0; j < nt; j++) {
				// comment.Format(_T("%s%d"), tStr, j);
			//	int nnn = dig->labelArr->GetCount();
				//if (dig->totCurve==1 && dig->labelArr->GetCount() >= nt)
				comment = dig->getLabelString(j);

				appendPlacemarkPoint(pXMLDoc,
					pElem,
					dig->x[j][n],
					dig->t[j][n],
					0.,
					comment,
					aStyle);
			}
		}
	} 
	else  {
		double *xx = (double *)ealloc1double(dig->digPointMax);
		double *tt = (double *)ealloc1double(dig->digPointMax);

		for (int n = 0; n < dig->totCurve; n++) {
			int nt = dig->nPoint[n];
			if (nt < 2) continue;

			comment = dig->getLabelString(n);
			if (comment.IsEmpty()) comment.Format(_T("%d"), n);
			for (j = 0; j < nt; j++) {
				xx[j] = dig->x[j][n];
				tt[j] = dig->t[j][n];
			}
			appendPlacemarkLine(pXMLDoc,
				pElem,
				xx,
				tt,
				nt,
				comment,
				aStyle);
		} // for n
		free1double(xx);
		free1double(tt);
	}



	// Format the XML. This requires a style sheet
	MSXML::IXMLDOMDocument2Ptr loadXML;
	hr = loadXML.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		ShowError(loadXML);
		return false;
	}

	//We need to load the style sheet which will be used to indent the XMl properly.
	CString xslFile = getSeisWideParaPath() + _T("\\StyleSheet.xsl");
	if (loadXML->load(variant_t(xslFile)) == VARIANT_FALSE) {
		ShowError(loadXML);
		return false;
	}

	// now save to file
	//Create the final document which will be indented properly
	MSXML::IXMLDOMDocument2Ptr pXMLFormattedDoc;
	hr = pXMLFormattedDoc.CreateInstance(__uuidof(DOMDocument60));

	CComPtr<IDispatch> pDispatch;
	hr = pXMLFormattedDoc->QueryInterface(IID_IDispatch, (void**)&pDispatch);
	if (FAILED(hr)) {
		return false;
	}

	_variant_t	vtOutObject;
	vtOutObject.vt = VT_DISPATCH;
	vtOutObject.pdispVal = pDispatch;
	vtOutObject.pdispVal->AddRef();

	//Apply the transformation to format the final document	
	hr = pXMLDoc->transformNodeToObject(loadXML, vtOutObject);

	//By default it is writing the encoding = UTF-16. Let us change the encoding to UTF-8
	MSXML::IXMLDOMNodePtr pXMLFirstChild = pXMLFormattedDoc->GetfirstChild();				// <?xml version="1.0" encoding="UTF-8"?>
	MSXML::IXMLDOMNamedNodeMapPtr pXMLAttributeMap = pXMLFirstChild->Getattributes();		// A map of the a attributes (vesrsion, encoding) values (1.0, UTF-8) pair
	MSXML::IXMLDOMNodePtr pXMLEncodNode = pXMLAttributeMap->getNamedItem(_T("encoding"));
	pXMLEncodNode->PutnodeValue(_T("UTF-8"));												//encoding = UTF-8

	hr = pXMLFormattedDoc->save(fOut.AllocSysString());
	if (FAILED(hr)) {
		ShowError(pXMLFormattedDoc);
	}

	//	showFileInNotePad(newName);
	return true;
}

bool CDigitize::getKmlDimension(CString fKml, int& ncurve, int& npoints)
{
	MSXML::IXMLDOMDocument2Ptr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		AfxMessageBox(_T("Failed to create the XML class instance"), MB_ICONQUESTION);
		return false;
	}
	if (pXMLDoc->load(variant_t(fKml)) == VARIANT_FALSE) {
		ShowError(pXMLDoc);
		return false;
	}

	CString tStr;

	ncurve = 0;
	npoints = 0;
	TRY{
	//int j = getAttributeDef(pVelMod, _T("numHorizon"), 0);
		MSXML::IXMLDOMElementPtr pKml = pXMLDoc->GetdocumentElement();
		MSXML::IXMLDOMNodePtr pDoc = myGetSingleChildByName(pKml, _T("Document")); // Document node
		MSXML::IXMLDOMNodePtr p = myGetSingleChildByName(pDoc, _T("Folder"));
	//	MSXML::IXMLDOMNodePtr p = pDoc->get, _T("Folder"));

		if (p == NULL) {
				MSXML::IXMLDOMNodeListPtr pPoints = pXMLDoc->getElementsByTagName(_T("Placemark"));
				npoints = max(npoints, pPoints->length);
				ncurve = 1;
				return true;
		}

		MSXML::IXMLDOMNodeListPtr pFolders = pDoc->childNodes;
		if (pFolders == NULL) return false;
		long len = pFolders->length;
		BSTR bName;
		for (int iFolder = 0; iFolder < len; iFolder++) {
			int npointsInFolder = 0;
			MSXML::IXMLDOMNodePtr pChild = pFolders->Getitem(iFolder);
			bName = pChild->nodeName;
			CString strName(bName);
			if (strName.CompareNoCase(_T("Folder")) != 0) {
				continue; // this is not a Folder node, but it is other type such as Style
			}
			MSXML::IXMLDOMNodeListPtr pChildren = pChild->childNodes;
			for (int i = 0; i < pChildren->length; i++) {
				pChild = pChildren->Getitem(i);
				bName = pChild->nodeName;
				CString strName(bName);
				if (strName.CompareNoCase(_T("Placemark")) == 0) {
					npointsInFolder++;
				}
			}
			npoints = max(npoints, npointsInFolder);
			ncurve++;
		}
		return true;
	}
	CATCH_ALL(e) {
		return false;
	}
	END_CATCH_ALL
}

bool CDigitize::digitize2Kml(CStringArray *arIn, CString fOut)
{
	MSXML::IXMLDOMDocument2Ptr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		AfxMessageBox(_T("Failed to create the XML class instance"), MB_ICONQUESTION);
		return false;
	}

	if (pXMLDoc->loadXML(_T("<kml></kml>")) == VARIANT_FALSE) {
		ShowError(pXMLDoc);
		return false;
	}

	//Get the root element just created	
	MSXML::IXMLDOMElementPtr pXMLRootElem = pXMLDoc->GetdocumentElement();
	MSXML::IXMLDOMProcessingInstructionPtr pXMLProcessingNode =
		pXMLDoc->createProcessingInstruction("xml", " version='1.0' encoding='UTF-8'");

	pXMLRootElem->setAttribute(_T("xmlns"), _variant_t("http://www.opengis.net/kml/2.2"));
	MSXML::IXMLDOMElementPtr pElem = pXMLDoc->createElement(_T("Document"));
	pXMLRootElem->appendChild(pElem);
	MSXML::IXMLDOMElementPtr p = pXMLDoc->createElement(_T("name"));
	p->Puttext(_bstr_t("KmlFile for Seiswide's digitizing format"));
	pElem->appendChild(p);
	p = pXMLDoc->createElement(_T("description"));
	p->Puttext(_bstr_t("This file contains the navigation information..."));
	pElem->appendChild(p);

	CString aStyle = _T("_dig");
	appendPlacemarkStyle(pXMLDoc, pElem, aStyle, 0.6);
	aStyle = _T("#m") + aStyle;

	int nLines = (int)arIn->GetCount();
	CString tStr = arIn->ElementAt(0);
	CStringArray art;
	for (int n = 0; n < nLines; n++) {
		int isTrue = SplitStringArr(arIn->ElementAt(n), SPACE, &art, false);
		if (!isTrue) continue;
		int nt = (int)art.GetCount();
		if (nt < 2) continue;
		CString comment = BLANK;
		if (nt == 3) comment += art.ElementAt(2);
		else if (nt > 3) comment = art.ElementAt(2) + _T("-") + art.ElementAt(3);
		if (comment.IsEmpty()) comment.Format(_T("%d"), n + 1);
		appendPlacemarkPoint(pXMLDoc,
			pElem,
			StringToFloat(art.ElementAt(0)),
			StringToFloat(art.ElementAt(1)),
			0.,
			comment,
			aStyle
			);
	} // for n



	// Format the XML. This requires a style sheet
	MSXML::IXMLDOMDocument2Ptr loadXML;
	hr = loadXML.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		ShowError(loadXML);
		return false;
	}

	//We need to load the style sheet which will be used to indent the XMl properly.
	CString xslFile = getSeisWideParaPath() + _T("\\StyleSheet.xsl");
	if (loadXML->load(variant_t(xslFile)) == VARIANT_FALSE) {
		ShowError(loadXML);
		return false;
	}

	// now save to file
	//Create the final document which will be indented properly
	MSXML::IXMLDOMDocument2Ptr pXMLFormattedDoc;
	hr = pXMLFormattedDoc.CreateInstance(__uuidof(DOMDocument60));

	CComPtr<IDispatch> pDispatch;
	hr = pXMLFormattedDoc->QueryInterface(IID_IDispatch, (void**)&pDispatch);
	if (FAILED(hr)) {
		return false;
	}

	_variant_t	vtOutObject;
	vtOutObject.vt = VT_DISPATCH;
	vtOutObject.pdispVal = pDispatch;
	vtOutObject.pdispVal->AddRef();

	//Apply the transformation to format the final document	
	hr = pXMLDoc->transformNodeToObject(loadXML, vtOutObject);

	//By default it is writing the encoding = UTF-16. Let us change the encoding to UTF-8
	MSXML::IXMLDOMNodePtr pXMLFirstChild = pXMLFormattedDoc->GetfirstChild();				// <?xml version="1.0" encoding="UTF-8"?>
	MSXML::IXMLDOMNamedNodeMapPtr pXMLAttributeMap = pXMLFirstChild->Getattributes();		// A map of the a attributes (vesrsion, encoding) values (1.0, UTF-8) pair
	MSXML::IXMLDOMNodePtr pXMLEncodNode = pXMLAttributeMap->getNamedItem(_T("encoding"));
	pXMLEncodNode->PutnodeValue(_T("UTF-8"));												//encoding = UTF-8

	hr = pXMLFormattedDoc->save(fOut.AllocSysString());
	if (FAILED(hr)) {
		ShowError(pXMLFormattedDoc);
	}

	//	showFileInNotePad(newName);
	return true;
}



#define ByteCopy( a, b, c )	memcpy( b, a, c )
#ifndef MAX
#  define MIN(a,b)      ((a<b) ? a : b)
#  define MAX(a,b)      ((a>b) ? a : b)
#endif

static int 	bBigEndian;


/************************************************************************/
/*                              SwapWord()                              */
/*                                                                      */
/*      Swap a 2, 4 or 8 byte word.                                     */
/************************************************************************/

void	CDigitize::SwapWord(int length, void * wordP)
{
	int		i;
	uchar	temp;

	for (i = 0; i < length / 2; i++)
	{
		temp = ((uchar *)wordP)[i];
		((uchar *)wordP)[i] = ((uchar *)wordP)[length - i - 1];
		((uchar *)wordP)[length - i - 1] = temp;
	}
}

/************************************************************************/
/*                          SHPWriteHeader()                            */
/*                                                                      */
/*      Write out a header for the .shp and .shx files as well as the	*/
/*	contents of the index (.shx) file.				*/
/************************************************************************/
void CDigitize::SHPWriteHeader(SHPHandle psSHP)

{
	uchar     	abyHeader[100];
	int		i;
	int32	i32;
	double	dValue;
	int32	*panSHX;

	/* -------------------------------------------------------------------- */
	/*      Prepare header block for .shp file.                             */
	/* -------------------------------------------------------------------- */
	for (i = 0; i < 100; i++)
		abyHeader[i] = 0;

	abyHeader[2] = 0x27;				/* magic cookie */
	abyHeader[3] = 0x0a;

	i32 = psSHP->nFileSize / 2;				/* file size */
	ByteCopy(&i32, abyHeader + 24, 4);
	if (!bBigEndian) SwapWord(4, abyHeader + 24);

	i32 = 1000;						/* version */
	ByteCopy(&i32, abyHeader + 28, 4);
	if (bBigEndian) SwapWord(4, abyHeader + 28);

	i32 = psSHP->nShapeType;				/* shape type */
	ByteCopy(&i32, abyHeader + 32, 4);
	if (bBigEndian) SwapWord(4, abyHeader + 32);

	dValue = psSHP->adBoundsMin[0];			/* set bounds */
	ByteCopy(&dValue, abyHeader + 36, 8);
	if (bBigEndian) SwapWord(8, abyHeader + 36);

	dValue = psSHP->adBoundsMin[1];
	ByteCopy(&dValue, abyHeader + 44, 8);
	if (bBigEndian) SwapWord(8, abyHeader + 44);

	dValue = psSHP->adBoundsMax[0];
	ByteCopy(&dValue, abyHeader + 52, 8);
	if (bBigEndian) SwapWord(8, abyHeader + 52);

	dValue = psSHP->adBoundsMax[1];
	ByteCopy(&dValue, abyHeader + 60, 8);
	if (bBigEndian) SwapWord(8, abyHeader + 60);

	dValue = psSHP->adBoundsMin[2];			/* z */
	ByteCopy(&dValue, abyHeader + 68, 8);
	if (bBigEndian) SwapWord(8, abyHeader + 68);

	dValue = psSHP->adBoundsMax[2];
	ByteCopy(&dValue, abyHeader + 76, 8);
	if (bBigEndian) SwapWord(8, abyHeader + 76);

	dValue = psSHP->adBoundsMin[3];			/* m */
	ByteCopy(&dValue, abyHeader + 84, 8);
	if (bBigEndian) SwapWord(8, abyHeader + 84);

	dValue = psSHP->adBoundsMax[3];
	ByteCopy(&dValue, abyHeader + 92, 8);
	if (bBigEndian) SwapWord(8, abyHeader + 92);

	/* -------------------------------------------------------------------- */
	/*      Write .shp file header.                                         */
	/* -------------------------------------------------------------------- */
	fseek(psSHP->fpSHP, 0, 0);
	fwrite(abyHeader, 100, 1, psSHP->fpSHP);

	/* -------------------------------------------------------------------- */
	/*      Prepare, and write .shx file header.                            */
	/* -------------------------------------------------------------------- */
	i32 = (psSHP->nRecords * 2 * sizeof(int32) + 100) / 2;   /* file size */
	ByteCopy(&i32, abyHeader + 24, 4);
	if (!bBigEndian) SwapWord(4, abyHeader + 24);

	fseek(psSHP->fpSHX, 0, 0);
	fwrite(abyHeader, 100, 1, psSHP->fpSHX);

	/* -------------------------------------------------------------------- */
	/*      Write out the .shx contents.                                    */
	/* -------------------------------------------------------------------- */
	panSHX = (int32 *)xmalloc(sizeof(int32) * 2 * psSHP->nRecords);

	for (i = 0; i < psSHP->nRecords; i++)
	{
		panSHX[i * 2] = psSHP->panRecOffset[i] / 2;
		panSHX[i * 2 + 1] = psSHP->panRecSize[i] / 2;
		if (!bBigEndian) SwapWord(4, panSHX + i * 2);
		if (!bBigEndian) SwapWord(4, panSHX + i * 2 + 1);
	}

	fwrite(panSHX, sizeof(int32) * 2, psSHP->nRecords, psSHP->fpSHX);

	xfree(panSHX);
}

/************************************************************************/
/*                              SHPOpen()                               */
/*                                                                      */
/*      Open the .shp and .shx files based on the basename of the       */
/*      files or either file name.                                      */
/************************************************************************/

CDigitize::SHPHandle CDigitize::SHPOpen(const char * pszLayer, const char * pszAccess)

{
	char		*pszFullname, *pszBasename;
	SHPHandle		psSHP;

	uchar		*pabyBuf;
	int			i;
	double		dValue;

	/* -------------------------------------------------------------------- */
	/*      Ensure the access string is one of the legal ones.  We          */
	/*      ensure the result string indicates binary to avoid common       */
	/*      problems on Windows.                                            */
	/* -------------------------------------------------------------------- */
	if (strcmp(pszAccess, "rb+") == 0 || strcmp(pszAccess, "r+b") == 0
		|| strcmp(pszAccess, "r+") == 0)
		pszAccess = "r+b";
	else
		pszAccess = "rb";

	/* -------------------------------------------------------------------- */
	/*	Establish the byte order on this machine.			*/
	/* -------------------------------------------------------------------- */
	i = 1;
	if (*((uchar *)&i) == 1)
		bBigEndian = FALSE;
	else
		bBigEndian = TRUE;

	/* -------------------------------------------------------------------- */
	/*	Initialize the info structure.					*/
	/* -------------------------------------------------------------------- */
	psSHP = (SHPHandle)calloc(sizeof(SHPInfo), 1);

	psSHP->bUpdated = FALSE;

	/* -------------------------------------------------------------------- */
	/*	Compute the base (layer) name.  If there is any extension	*/
	/*	on the passed in filename we will strip it off.			*/
	/* -------------------------------------------------------------------- */
	pszBasename = (char *)xmalloc(strlen(pszLayer) + 5);
	strcpy(pszBasename, pszLayer);
	for (i = (int)strlen(pszBasename) - 1;
		i > 0 && pszBasename[i] != '.' && pszBasename[i] != '/'
		&& pszBasename[i] != '\\';
	i--) {
	}

	if (pszBasename[i] == '.')
		pszBasename[i] = '\0';

	/* -------------------------------------------------------------------- */
	/*	Open the .shp and .shx files.  Note that files pulled from	*/
	/*	a PC to Unix with upper case filenames won't work!		*/
	/* -------------------------------------------------------------------- */
	pszFullname = (char *)xmalloc(strlen(pszBasename) + 5);
	sprintf(pszFullname, "%s.shp", pszBasename);
	psSHP->fpSHP = fopen(pszFullname, pszAccess);
	if (psSHP->fpSHP == NULL)
	{
		sprintf(pszFullname, "%s.SHP", pszBasename);
		psSHP->fpSHP = fopen(pszFullname, pszAccess);
	}

	if (psSHP->fpSHP == NULL)
	{
		free(psSHP);
		xfree(pszBasename);
		xfree(pszFullname);
		return(NULL);
	}

	sprintf(pszFullname, "%s.shx", pszBasename);
	psSHP->fpSHX = fopen(pszFullname, pszAccess);
	if (psSHP->fpSHX == NULL)
	{
		sprintf(pszFullname, "%s.SHX", pszBasename);
		psSHP->fpSHX = fopen(pszFullname, pszAccess);
	}

	if (psSHP->fpSHX == NULL)
	{
		fclose(psSHP->fpSHP);
		free(psSHP);
		xfree(pszBasename);
		xfree(pszFullname);
		return(NULL);
	}

	xfree(pszFullname);
	xfree(pszBasename);

	/* -------------------------------------------------------------------- */
	/*  Read the file size from the SHP file.				*/
	/* -------------------------------------------------------------------- */
	pabyBuf = (uchar *)xmalloc(100);
	fread(pabyBuf, 100, 1, psSHP->fpSHP);

	psSHP->nFileSize = (pabyBuf[24] * 256 * 256 * 256
		+ pabyBuf[25] * 256 * 256
		+ pabyBuf[26] * 256
		+ pabyBuf[27]) * 2;

	/* -------------------------------------------------------------------- */
	/*  Read SHX file Header info                                           */
	/* -------------------------------------------------------------------- */
	fread(pabyBuf, 100, 1, psSHP->fpSHX);

	if (pabyBuf[0] != 0
		|| pabyBuf[1] != 0
		|| pabyBuf[2] != 0x27
		|| (pabyBuf[3] != 0x0a && pabyBuf[3] != 0x0d))
	{
		fclose(psSHP->fpSHP);
		fclose(psSHP->fpSHX);
		free(psSHP);

		return(NULL);
	}

	psSHP->nRecords = pabyBuf[27] + pabyBuf[26] * 256
		+ pabyBuf[25] * 256 * 256 + pabyBuf[24] * 256 * 256 * 256;
	psSHP->nRecords = (psSHP->nRecords * 2 - 100) / 8;

	psSHP->nShapeType = pabyBuf[32];

	if (psSHP->nRecords < 0 || psSHP->nRecords > 256000000)
	{
		/* this header appears to be corrupt.  Give up. */
		fclose(psSHP->fpSHP);
		fclose(psSHP->fpSHX);
		free(psSHP);

		return(NULL);
	}

	/* -------------------------------------------------------------------- */
	/*      Read the bounds.                                                */
	/* -------------------------------------------------------------------- */
	if (bBigEndian) SwapWord(8, pabyBuf + 36);
	memcpy(&dValue, pabyBuf + 36, 8);
	psSHP->adBoundsMin[0] = dValue;

	if (bBigEndian) SwapWord(8, pabyBuf + 44);
	memcpy(&dValue, pabyBuf + 44, 8);
	psSHP->adBoundsMin[1] = dValue;

	if (bBigEndian) SwapWord(8, pabyBuf + 52);
	memcpy(&dValue, pabyBuf + 52, 8);
	psSHP->adBoundsMax[0] = dValue;

	if (bBigEndian) SwapWord(8, pabyBuf + 60);
	memcpy(&dValue, pabyBuf + 60, 8);
	psSHP->adBoundsMax[1] = dValue;

	if (bBigEndian) SwapWord(8, pabyBuf + 68);		/* z */
	memcpy(&dValue, pabyBuf + 68, 8);
	psSHP->adBoundsMin[2] = dValue;

	if (bBigEndian) SwapWord(8, pabyBuf + 76);
	memcpy(&dValue, pabyBuf + 76, 8);
	psSHP->adBoundsMax[2] = dValue;

	if (bBigEndian) SwapWord(8, pabyBuf + 84);		/* z */
	memcpy(&dValue, pabyBuf + 84, 8);
	psSHP->adBoundsMin[3] = dValue;

	if (bBigEndian) SwapWord(8, pabyBuf + 92);
	memcpy(&dValue, pabyBuf + 92, 8);
	psSHP->adBoundsMax[3] = dValue;

	xfree(pabyBuf);

	/* -------------------------------------------------------------------- */
	/*	Read the .shx file to get the offsets to each record in 	*/
	/*	the .shp file.							*/
	/* -------------------------------------------------------------------- */
	psSHP->nMaxRecords = psSHP->nRecords;

	psSHP->panRecOffset =
		(int *)xmalloc(sizeof(int) * MAX(1, psSHP->nMaxRecords));
	psSHP->panRecSize =
		(int *)xmalloc(sizeof(int) * MAX(1, psSHP->nMaxRecords));

	pabyBuf = (uchar *)xmalloc(8 * MAX(1, psSHP->nRecords));
	fread(pabyBuf, 8, psSHP->nRecords, psSHP->fpSHX);

	for (i = 0; i < psSHP->nRecords; i++)
	{
		int32		nOffset, nLength;

		memcpy(&nOffset, pabyBuf + i * 8, 4);
		if (!bBigEndian) SwapWord(4, &nOffset);

		memcpy(&nLength, pabyBuf + i * 8 + 4, 4);
		if (!bBigEndian) SwapWord(4, &nLength);

		psSHP->panRecOffset[i] = nOffset * 2;
		psSHP->panRecSize[i] = nLength * 2;
	}
	xfree(pabyBuf);

	return(psSHP);
}

/************************************************************************/
/*                              SHPClose()                              */
/*								       	*/
/*	Close the .shp and .shx files.					*/
/************************************************************************/

void CDigitize::SHPClose(SHPHandle psSHP)

{
	/* -------------------------------------------------------------------- */
	/*	Update the header if we have modified anything.			*/
	/* -------------------------------------------------------------------- */
	if (psSHP->bUpdated)
	{
		SHPWriteHeader(psSHP);
	}

	/* -------------------------------------------------------------------- */
	/*      Free all resources, and close files.                            */
	/* -------------------------------------------------------------------- */
	xfree(psSHP->panRecOffset);
	xfree(psSHP->panRecSize);

	fclose(psSHP->fpSHX);
	fclose(psSHP->fpSHP);

	if (psSHP->pabyRec != NULL)
	{
		xfree(psSHP->pabyRec);
	}

	free(psSHP);
}




// just put any shape file name into shapeFile (unicode string), and this function will read it to *dig2
BOOL CDigitize::SHPOpenToDig(CString shapeFile, CDigitize::DIGITIZE *dig2, SEI_PAIR &pmin, SEI_PAIR &pmax)
{
	char * pszLayer = new char[255];
	ZeroMemory(pszLayer, 255);
	strcpy_s(pszLayer, 255, CT2A(shapeFile));

//	const char 	*pszPlus;
	// char * pszAccess = "rb";
	SHPHandle hSHP = this->SHPOpen(pszLayer, "rb");
	free(pszLayer);

	if (hSHP == NULL)
		return FALSE;

	double 	adfMinBound[4], adfMaxBound[4];
	int	nShapeType, nEntities, i, iPart, bValidate = 0, nInvalidCount = 0;
	SHPGetInfo(hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound);
	pmin.x = adfMinBound[0];
	pmin.y = adfMinBound[1];
	pmax.x = adfMaxBound[0];
	pmax.y = adfMaxBound[1];
//	printf("Shapefile Type: %s   # of Shapes: %d\n\n", SHPTypeName(nShapeType), nEntities);

	// need to convert from SHP tp *dig2 format
	if (!dig2->initDigitizeArray(200, nEntities)) {
		// Cannot init dig array
		dig2->freeDigAll();
		this->SHPClose(hSHP);
		return FALSE;
	}
	dig2->totCurve = nEntities;
	dig2->iCurveSel = 0;
	dig2->nShapeType = nShapeType;


	/* -------------------------------------------------------------------- */
	/*	Skim over the list of shapes, printing all the vertices.	*/
	/* -------------------------------------------------------------------- */
	int iDig = 0;
	for (i = 0; i < nEntities; i++) {
		int		j;
		SHPObject	*psShape = SHPReadObject(hSHP, i);
		if (psShape == NULL) {
			//fprintf(stderr, "Unable to read shape %d, terminating object reading.\n", i);
			break;
		}

		//printf("\nShape:%d (%s)  nVertices=%d, nParts=%d\n"
		//	"  Bounds:(%.15g,%.15g, %.15g)\n"
		//	"      to (%.15g,%.15g, %.15g)\n",
		//	i, SHPTypeName(psShape->nSHPType),
		//	psShape->nVertices, psShape->nParts,
		//	psShape->dfXMin, psShape->dfYMin, psShape->dfZMin,
		//	psShape->dfXMax, psShape->dfYMax, psShape->dfZMax);
		//if (psShape->nParts > 0 && psShape->panPartStart[0] != 0) {
		//	fprintf(stderr, "panPartStart[0] = %d, not zero as expected.\n",
		//		psShape->panPartStart[0]);
		//}

		for (j = 0, iPart = 1; j < psShape->nVertices; j++) {
			const char	*pszPartType = "";

			//if (j == 0 && psShape->nParts > 0)
			//	pszPartType = SHPPartTypeName(psShape->panPartType[0]);

			//if (iPart < psShape->nParts && psShape->panPartStart[iPart] == j) {
			//	pszPartType = SHPPartTypeName(psShape->panPartType[iPart]);
			//	iPart++;
			//	pszPlus = "+";
			//}
			//else
			//	pszPlus = " ";

			//printf("   %s (%.15g,%.15g, %.15g) %s \n",
			//	pszPlus,
			//	psShape->padfX[j],
			//	psShape->padfY[j],
			//	psShape->padfZ[j],
			//	pszPartType);

			this->myDigAddPoint(dig2, iDig, psShape->padfX[j], psShape->padfY[j]);
			// after this call, iDig may have changed!

			//psShape->padfZ[j] not existing in dig format
		}

		//float **x, **t; // x[it][ic], this is reversed by mistake!
		dig2->nPoint[iDig] = psShape->nVertices;
		dig2->ivray[iDig] = 0;  // not defined in shape format
		iDig++;

		SHPDestroyObject(psShape);
	}

	// now finish up
	this->SHPClose(hSHP);
	return TRUE;
}

/************************************************************************/
/*                             SHPGetInfo()                             */
/*                                                                      */
/*      Fetch general information about the shape file.                 */
/************************************************************************/

void CDigitize::SHPGetInfo(SHPHandle psSHP, int * pnEntities, int * pnShapeType,
double * padfMinBound, double * padfMaxBound)

{
	int		i;

	if (pnEntities != NULL)
		*pnEntities = psSHP->nRecords;

	if (pnShapeType != NULL)
		*pnShapeType = psSHP->nShapeType;

	for (i = 0; i < 4; i++)
	{
		if (padfMinBound != NULL)
			padfMinBound[i] = psSHP->adBoundsMin[i];
		if (padfMaxBound != NULL)
			padfMaxBound[i] = psSHP->adBoundsMax[i];
	}
}


/************************************************************************/
/*                             SfRealloc()                              */
/*                                                                      */
/*      A realloc cover function that will access a NULL pointer as     */
/*      a valid input.                                                  */
/************************************************************************/

void * CDigitize::SfRealloc(void * pMem, int nNewSize)

{
	if (pMem == NULL)
		return((void *)xmalloc(nNewSize));
	else
		return((void *)xrealloc(pMem, nNewSize));
}

/************************************************************************/
/*                          SHPReadObject()                             */
/*                                                                      */
/*      Read the vertices, parts, and other non-attribute information	*/
/*	for one shape.							*/
/************************************************************************/

CDigitize::SHPObject * CDigitize::SHPReadObject(SHPHandle psSHP, int hEntity)

{
	SHPObject		*psShape;

	/* -------------------------------------------------------------------- */
	/*      Validate the record/entity number.                              */
	/* -------------------------------------------------------------------- */
	if (hEntity < 0 || hEntity >= psSHP->nRecords)
		return(NULL);

	/* -------------------------------------------------------------------- */
	/*      Ensure our record buffer is large enough.                       */
	/* -------------------------------------------------------------------- */
	if (psSHP->panRecSize[hEntity] + 8 > psSHP->nBufSize)
	{
		psSHP->nBufSize = psSHP->panRecSize[hEntity] + 8;
		psSHP->pabyRec = (uchar *)SfRealloc(psSHP->pabyRec, psSHP->nBufSize);
	}

	/* -------------------------------------------------------------------- */
	/*      Read the record.                                                */
	/* -------------------------------------------------------------------- */
	fseek(psSHP->fpSHP, psSHP->panRecOffset[hEntity], 0);
	fread(psSHP->pabyRec, psSHP->panRecSize[hEntity] + 8, 1, psSHP->fpSHP);

	/* -------------------------------------------------------------------- */
	/*	Allocate and minimally initialize the object.			*/
	/* -------------------------------------------------------------------- */
	psShape = (SHPObject *)calloc(1, sizeof(SHPObject));
	psShape->nShapeId = hEntity;

	memcpy(&psShape->nSHPType, psSHP->pabyRec + 8, 4);
	if (bBigEndian) SwapWord(4, &(psShape->nSHPType));

	/* ==================================================================== */
	/*  Extract vertices for a Polygon or Arc.				*/
	/* ==================================================================== */
	if (psShape->nSHPType == SHPT_POLYGON || psShape->nSHPType == SHPT_ARC
		|| psShape->nSHPType == SHPT_POLYGONZ
		|| psShape->nSHPType == SHPT_POLYGONM
		|| psShape->nSHPType == SHPT_ARCZ
		|| psShape->nSHPType == SHPT_ARCM
		|| psShape->nSHPType == SHPT_MULTIPATCH)
	{
		int32		nPoints, nParts;
		int    		i, nOffset;

		/* -------------------------------------------------------------------- */
		/*	Get the X/Y bounds.						*/
		/* -------------------------------------------------------------------- */
		memcpy(&(psShape->dfXMin), psSHP->pabyRec + 8 + 4, 8);
		memcpy(&(psShape->dfYMin), psSHP->pabyRec + 8 + 12, 8);
		memcpy(&(psShape->dfXMax), psSHP->pabyRec + 8 + 20, 8);
		memcpy(&(psShape->dfYMax), psSHP->pabyRec + 8 + 28, 8);

		if (bBigEndian) SwapWord(8, &(psShape->dfXMin));
		if (bBigEndian) SwapWord(8, &(psShape->dfYMin));
		if (bBigEndian) SwapWord(8, &(psShape->dfXMax));
		if (bBigEndian) SwapWord(8, &(psShape->dfYMax));

		/* -------------------------------------------------------------------- */
		/*      Extract part/point count, and build vertex and part arrays      */
		/*      to proper size.                                                 */
		/* -------------------------------------------------------------------- */
		memcpy(&nPoints, psSHP->pabyRec + 40 + 8, 4);
		memcpy(&nParts, psSHP->pabyRec + 36 + 8, 4);

		if (bBigEndian) SwapWord(4, &nPoints);
		if (bBigEndian) SwapWord(4, &nParts);

		psShape->nVertices = nPoints;
		//psShape->padfX = (double *)calloc(nPoints, sizeof(double));
		//psShape->padfY = (double *)calloc(nPoints, sizeof(double));
		//psShape->padfZ = (double *)calloc(nPoints, sizeof(double));
		//psShape->padfM = (double *)calloc(nPoints, sizeof(double));
		psShape->padfX = (double *)ealloc1double(nPoints);
		psShape->padfY = (double *)ealloc1double(nPoints);
		psShape->padfZ = (double *)ealloc1double(nPoints);
		psShape->padfM = (double *)ealloc1double(nPoints);
		memset(psShape->padfX, 0, nPoints*sizeof(double));
		memset(psShape->padfY, 0, nPoints*sizeof(double));
		memset(psShape->padfZ, 0, nPoints*sizeof(double));
		memset(psShape->padfM, 0, nPoints*sizeof(double));

		psShape->nParts = nParts;
		// psShape->panPartStart = (int *)calloc(nParts, sizeof(int));
		psShape->panPartStart = (int *)ealloc1int(nParts);
		memset(psShape->panPartStart, 0, nParts*sizeof(int));

		//psShape->panPartType = (int *)calloc(nParts, sizeof(int));
		psShape->panPartType = (int *)ealloc1int(nParts);
		memset(psShape->panPartType, 0, nParts*sizeof(int));

		for (i = 0; i < nParts; i++)
			psShape->panPartType[i] = SHPP_RING;

		/* -------------------------------------------------------------------- */
		/*      Copy out the part array from the record.                        */
		/* -------------------------------------------------------------------- */
		memcpy(psShape->panPartStart, psSHP->pabyRec + 44 + 8, 4 * nParts);
		for (i = 0; i < nParts; i++)
		{
			if (bBigEndian) SwapWord(4, psShape->panPartStart + i);
		}

		nOffset = 44 + 8 + 4 * nParts;

		/* -------------------------------------------------------------------- */
		/*      If this is a multipatch, we will also have parts types.         */
		/* -------------------------------------------------------------------- */
		if (psShape->nSHPType == SHPT_MULTIPATCH)
		{
			memcpy(psShape->panPartType, psSHP->pabyRec + nOffset, 4 * nParts);
			for (i = 0; i < nParts; i++)
			{
				if (bBigEndian) SwapWord(4, psShape->panPartType + i);
			}

			nOffset += 4 * nParts;
		}

		/* -------------------------------------------------------------------- */
		/*      Copy out the vertices from the record.                          */
		/* -------------------------------------------------------------------- */
		for (i = 0; i < nPoints; i++)
		{
			memcpy(psShape->padfX + i,
				psSHP->pabyRec + nOffset + i * 16,
				8);

			memcpy(psShape->padfY + i,
				psSHP->pabyRec + nOffset + i * 16 + 8,
				8);

			if (bBigEndian) SwapWord(8, psShape->padfX + i);
			if (bBigEndian) SwapWord(8, psShape->padfY + i);
		}

		nOffset += 16 * nPoints;

		/* -------------------------------------------------------------------- */
		/*      If we have a Z coordinate, collect that now.                    */
		/* -------------------------------------------------------------------- */
		if (psShape->nSHPType == SHPT_POLYGONZ
			|| psShape->nSHPType == SHPT_ARCZ
			|| psShape->nSHPType == SHPT_MULTIPATCH)
		{
			memcpy(&(psShape->dfZMin), psSHP->pabyRec + nOffset, 8);
			memcpy(&(psShape->dfZMax), psSHP->pabyRec + nOffset + 8, 8);

			if (bBigEndian) SwapWord(8, &(psShape->dfZMin));
			if (bBigEndian) SwapWord(8, &(psShape->dfZMax));

			for (i = 0; i < nPoints; i++)
			{
				memcpy(psShape->padfZ + i,
					psSHP->pabyRec + nOffset + 16 + i * 8, 8);
				if (bBigEndian) SwapWord(8, psShape->padfZ + i);
			}

			nOffset += 16 + 8 * nPoints;
		}

		/* -------------------------------------------------------------------- */
		/*      If we have a M measure value, then read it now.  We assume      */
		/*      that the measure can be present for any shape if the size is    */
		/*      big enough, but really it will only occur for the Z shapes      */
		/*      (options), and the M shapes.                                    */
		/* -------------------------------------------------------------------- */
		if (psSHP->panRecSize[hEntity] + 8 >= nOffset + 16 + 8 * nPoints)
		{
			memcpy(&(psShape->dfMMin), psSHP->pabyRec + nOffset, 8);
			memcpy(&(psShape->dfMMax), psSHP->pabyRec + nOffset + 8, 8);

			if (bBigEndian) SwapWord(8, &(psShape->dfMMin));
			if (bBigEndian) SwapWord(8, &(psShape->dfMMax));

			for (i = 0; i < nPoints; i++)
			{
				memcpy(psShape->padfM + i,
					psSHP->pabyRec + nOffset + 16 + i * 8, 8);
				if (bBigEndian) SwapWord(8, psShape->padfM + i);
			}
		}

	}

	/* ==================================================================== */
	/*  Extract vertices for a MultiPoint.					*/
	/* ==================================================================== */
	else if (psShape->nSHPType == SHPT_MULTIPOINT
		|| psShape->nSHPType == SHPT_MULTIPOINTM
		|| psShape->nSHPType == SHPT_MULTIPOINTZ)
	{
		int32		nPoints;
		int    		i, nOffset;

		memcpy(&nPoints, psSHP->pabyRec + 44, 4);
		if (bBigEndian) SwapWord(4, &nPoints);

		psShape->nVertices = nPoints;
		psShape->padfX = (double *)ealloc1double(nPoints);
		psShape->padfY = (double *)ealloc1double(nPoints);
		psShape->padfZ = (double *)ealloc1double(nPoints);
		psShape->padfM = (double *)ealloc1double(nPoints);
		memset(psShape->padfX, 0, nPoints*sizeof(double));
		memset(psShape->padfY, 0, nPoints*sizeof(double));
		memset(psShape->padfZ, 0, nPoints*sizeof(double));
		memset(psShape->padfM, 0, nPoints*sizeof(double));
		for (i = 0; i < nPoints; i++)
		{
			memcpy(psShape->padfX + i, psSHP->pabyRec + 48 + 16 * i, 8);
			memcpy(psShape->padfY + i, psSHP->pabyRec + 48 + 16 * i + 8, 8);

			if (bBigEndian) SwapWord(8, psShape->padfX + i);
			if (bBigEndian) SwapWord(8, psShape->padfY + i);
		}

		nOffset = 48 + 16 * nPoints;

		/* -------------------------------------------------------------------- */
		/*	Get the X/Y bounds.						*/
		/* -------------------------------------------------------------------- */
		memcpy(&(psShape->dfXMin), psSHP->pabyRec + 8 + 4, 8);
		memcpy(&(psShape->dfYMin), psSHP->pabyRec + 8 + 12, 8);
		memcpy(&(psShape->dfXMax), psSHP->pabyRec + 8 + 20, 8);
		memcpy(&(psShape->dfYMax), psSHP->pabyRec + 8 + 28, 8);

		if (bBigEndian) SwapWord(8, &(psShape->dfXMin));
		if (bBigEndian) SwapWord(8, &(psShape->dfYMin));
		if (bBigEndian) SwapWord(8, &(psShape->dfXMax));
		if (bBigEndian) SwapWord(8, &(psShape->dfYMax));

		/* -------------------------------------------------------------------- */
		/*      If we have a Z coordinate, collect that now.                    */
		/* -------------------------------------------------------------------- */
		if (psShape->nSHPType == SHPT_MULTIPOINTZ)
		{
			memcpy(&(psShape->dfZMin), psSHP->pabyRec + nOffset, 8);
			memcpy(&(psShape->dfZMax), psSHP->pabyRec + nOffset + 8, 8);

			if (bBigEndian) SwapWord(8, &(psShape->dfZMin));
			if (bBigEndian) SwapWord(8, &(psShape->dfZMax));

			for (i = 0; i < nPoints; i++)
			{
				memcpy(psShape->padfZ + i,
					psSHP->pabyRec + nOffset + 16 + i * 8, 8);
				if (bBigEndian) SwapWord(8, psShape->padfZ + i);
			}

			nOffset += 16 + 8 * nPoints;
		}

		/* -------------------------------------------------------------------- */
		/*      If we have a M measure value, then read it now.  We assume      */
		/*      that the measure can be present for any shape if the size is    */
		/*      big enough, but really it will only occur for the Z shapes      */
		/*      (options), and the M shapes.                                    */
		/* -------------------------------------------------------------------- */
		if (psSHP->panRecSize[hEntity] + 8 >= nOffset + 16 + 8 * nPoints)
		{
			memcpy(&(psShape->dfMMin), psSHP->pabyRec + nOffset, 8);
			memcpy(&(psShape->dfMMax), psSHP->pabyRec + nOffset + 8, 8);

			if (bBigEndian) SwapWord(8, &(psShape->dfMMin));
			if (bBigEndian) SwapWord(8, &(psShape->dfMMax));

			for (i = 0; i < nPoints; i++)
			{
				memcpy(psShape->padfM + i,
					psSHP->pabyRec + nOffset + 16 + i * 8, 8);
				if (bBigEndian) SwapWord(8, psShape->padfM + i);
			}
		}
	}

	/* ==================================================================== */
	/*      Extract vertices for a point.                                   */
	/* ==================================================================== */
	else if (psShape->nSHPType == SHPT_POINT
		|| psShape->nSHPType == SHPT_POINTM
		|| psShape->nSHPType == SHPT_POINTZ)
	{
		int	nOffset;

		psShape->nVertices = 1;
		//psShape->padfX = (double *)calloc(1, sizeof(double));
		//psShape->padfY = (double *)calloc(1, sizeof(double));
		//psShape->padfZ = (double *)calloc(1, sizeof(double));
		//psShape->padfM = (double *)calloc(1, sizeof(double));
		psShape->padfX = (double *)ealloc1double(1);
		psShape->padfY = (double *)ealloc1double(1);
		psShape->padfZ = (double *)ealloc1double(1);
		psShape->padfM = (double *)ealloc1double(1);
		memset(psShape->padfX, 0, 1*sizeof(double));
		memset(psShape->padfY, 0, 1*sizeof(double));
		memset(psShape->padfZ, 0, 1*sizeof(double));
		memset(psShape->padfM, 0, 1*sizeof(double));

		memcpy(psShape->padfX, psSHP->pabyRec + 12, 8);
		memcpy(psShape->padfY, psSHP->pabyRec + 20, 8);

		if (bBigEndian) SwapWord(8, psShape->padfX);
		if (bBigEndian) SwapWord(8, psShape->padfY);

		nOffset = 20 + 8;

		/* -------------------------------------------------------------------- */
		/*      If we have a Z coordinate, collect that now.                    */
		/* -------------------------------------------------------------------- */
		if (psShape->nSHPType == SHPT_POINTZ)
		{
			memcpy(psShape->padfZ, psSHP->pabyRec + nOffset, 8);

			if (bBigEndian) SwapWord(8, psShape->padfZ);

			nOffset += 8;
		}

		/* -------------------------------------------------------------------- */
		/*      If we have a M measure value, then read it now.  We assume      */
		/*      that the measure can be present for any shape if the size is    */
		/*      big enough, but really it will only occur for the Z shapes      */
		/*      (options), and the M shapes.                                    */
		/* -------------------------------------------------------------------- */
		if (psSHP->panRecSize[hEntity] + 8 >= nOffset + 8)
		{
			memcpy(psShape->padfM, psSHP->pabyRec + nOffset, 8);

			if (bBigEndian) SwapWord(8, psShape->padfM);
		}

		/* -------------------------------------------------------------------- */
		/*      Since no extents are supplied in the record, we will apply      */
		/*      them from the single vertex.                                    */
		/* -------------------------------------------------------------------- */
		psShape->dfXMin = psShape->dfXMax = psShape->padfX[0];
		psShape->dfYMin = psShape->dfYMax = psShape->padfY[0];
		psShape->dfZMin = psShape->dfZMax = psShape->padfZ[0];
		psShape->dfMMin = psShape->dfMMax = psShape->padfM[0];
	}

	return(psShape);
}



/************************************************************************/
/*                            SHPTypeName()                             */
/************************************************************************/

const char * CDigitize::SHPTypeName(int nSHPType)

{
	switch (nSHPType)
	{
	case SHPT_NULL:
		return "NullShape";

	case SHPT_POINT:
		return "Point";

	case SHPT_ARC:
		return "Arc";

	case SHPT_POLYGON:
		return "Polygon";

	case SHPT_MULTIPOINT:
		return "MultiPoint";

	case SHPT_POINTZ:
		return "PointZ";

	case SHPT_ARCZ:
		return "ArcZ";

	case SHPT_POLYGONZ:
		return "PolygonZ";

	case SHPT_MULTIPOINTZ:
		return "MultiPointZ";

	case SHPT_POINTM:
		return "PointM";

	case SHPT_ARCM:
		return "ArcM";

	case SHPT_POLYGONM:
		return "PolygonM";

	case SHPT_MULTIPOINTM:
		return "MultiPointM";

	case SHPT_MULTIPATCH:
		return "MultiPatch";

	default:
		return "UnknownShapeType";
	}
}

/************************************************************************/
/*                          SHPPartTypeName()                           */
/************************************************************************/

const char * CDigitize::SHPPartTypeName(int nPartType)

{
	switch (nPartType)
	{
	case SHPP_TRISTRIP:
		return "TriangleStrip";

	case SHPP_TRIFAN:
		return "TriangleFan";

	case SHPP_OUTERRING:
		return "OuterRing";

	case SHPP_INNERRING:
		return "InnerRing";

	case SHPP_FIRSTRING:
		return "FirstRing";

	case SHPP_RING:
		return "Ring";

	default:
		return "UnknownPartType";
	}
}

/************************************************************************/
/*                          SHPDestroyObject()                          */
/************************************************************************/

void CDigitize::SHPDestroyObject(SHPObject * psShape)

{
	if (psShape == NULL)
		return;

	if (psShape->padfX != NULL)
		free1double(psShape->padfX);
	if (psShape->padfY != NULL)
		free1double(psShape->padfY);
	if (psShape->padfZ != NULL)
		free1double(psShape->padfZ);
	if (psShape->padfM != NULL)
		free1double(psShape->padfM);

	if (psShape->panPartStart != NULL)
		free1int(psShape->panPartStart);
	if (psShape->panPartType != NULL)
		free1int(psShape->panPartType);

	free(psShape);
}
