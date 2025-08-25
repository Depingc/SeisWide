#include "stdafx.h"
#include "math.h"
#include "float.h"
#include "ealloc_d.h"
#include "colorHelper.h"

CColorHelper::CColorHelper()
{
}

CColorHelper::~CColorHelper()
{
}


int CColorHelper::getLineColorArray(CColorHelper::LINECOLORS *lineColors, CString f)
{  // return number of items, must be 11
	CString tStr;

	TCHAR curDir[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, curDir);
	CString aPath = (CString)curDir;

	//CString aPath = getWorkingPathSlash();
	//setCurrentPathAs(aPath); // we need to set the current path since velms can use relative path

//	if (!isFileExistNotDirectory(f)) return FALSE; // Note: for a new digitizing, it is OK if the file does not exist!!!

	if (lineColors->NUM_LINECOLORS != 11) {
		// this would be a case against my convention here. So we just set to constant green
		for (int i = 0; i < lineColors->NUM_LINECOLORS; i++) {
			lineColors->r[i] = 0;
			lineColors->g[i] = 255;
			lineColors->b[i] = 0;
		}
		return 0;
	}

	CString lineColFileName = GetPathFromFileSlash(f) + _T("linePalette.txt");
	if (!isFileExistNotDirectory(lineColFileName)) {

		// the color index must be lineColors->NUM_LINECOLORS == 11
		//static const int col_r[11] = {255,255, 255, 127,   0,   0,   0,   0,   0, 127, 255};
		//static const int col_g[11] = {000,127, 255, 255, 255, 255, 255, 127,   0,   0,   0};
		//static const int col_b[11] = {000,  0,   0,   0,   0, 127, 255, 255, 255, 255, 255};

		// since we have too many lines in red, so we want to make the first line to be green
		static const int col_r[11] = { 0, 127,  255, 255, 200, 127, 240, 0,   0,  127,255 };
		static const int col_g[11] = { 255,255, 211, 127, 10,  255, 240, 127, 0,  0,  0 };
		static const int col_b[11] = { 000, 0,  0,   0,   10,  127, 240, 255, 255,255,255 };
		memcpy(lineColors->r, col_r, sizeof(int) * 11);
		memcpy(lineColors->g, col_g, sizeof(int) * 11);
		memcpy(lineColors->b, col_b, sizeof(int) * 11);

		// now save
		CStringArray 	*arr = new CStringArray();
		tStr.Format(_T("#Color index file applied to the same directory as line drawings file."));
		arr->Add(tStr);
		tStr.Format(_T("#If more lines exist than colors, colors will be repeated starting from the 1st."));
		arr->Add(tStr);
		tStr.Format(_T("#r      g     b    (You may manually edit these texts.)"));
		arr->Add(tStr);

		for (int i = 0; i < 11; i++) {
			tStr.Format(_T("%3d  %3d  %3d"), lineColors->r[i], lineColors->g[i], lineColors->b[i]);
			arr->Add(tStr);
		}
		BOOL isTrue = saveStringArrayToFile(arr, lineColFileName);
		arr->RemoveAll();
		delete arr;
		return 11;
	}

	CStringArray *arr = getLinesFromFile(lineColFileName);
	INT_PTR j;
	if (arr != NULL && arr->GetSize() > 0) {
		for (int i = 0; i < lineColors->NUM_LINECOLORS; i++) {
			j = i;
			while (j >= arr->GetSize())
				j -= arr->GetSize();
			if (j < 0) break;  // this cannot happen
			_stscanf_s(arr->ElementAt(j).Trim(), _T("%d%d%d"), &lineColors->r[i], &lineColors->g[i], &lineColors->b[i]);
			if (lineColors->r[i] == 255 && lineColors->g[i] == 0 && lineColors->b[i] == 0) {
				// since red color is reserved for mouse selection, we do not want any pure red line here
				lineColors->r[i] = 200;
				lineColors->g[i] = 0;
				lineColors->b[i] = 0;
			}
		}
	}
	else {
		// if here, the file does not exist, we just set the defaults
		for (int i = 0; i < lineColors->NUM_LINECOLORS; i++) {
			lineColors->r[i] = 0;
			lineColors->g[i] = 255;
			lineColors->b[i] = 0;
		}
	}
	if (arr) {
		arr->RemoveAll();
		delete arr;
		arr = NULL;
	}
	return 11;
}


COLORREF CColorHelper::getVelocityColor(float v, CColorHelper::VELCOLOR *pvel)
{
	// convert velocity to colors based on the mapping in vel struct
	/*
	vmin = 1.45;
	vmid = 2.;
	vmid2 = 3.;
	vmid3 = 4.5;
	vmid4 = 5.0;
	vmid5 = 5.5;
	vmid6 = 6.2;
	vmid7 = 6.7;
	vmid8 = 7.3;
	vmax = 8.2;
	*/
//	return CColorHelper::WHITE;
	COLORREF cout = WHITE;

	if (v < pvel->vmin) {
	}
	else if (v <= pvel->vmid) {
		cout = interp1D_HSV(RGB(pvel->vminR, pvel->vminG, pvel->vminB), RGB(pvel->vmidR, pvel->vmidG, pvel->vmidB),
			pvel->vmin, pvel->vmid, v);
	}
	else if (v <= pvel->vmid2) {
		cout = interp1D_HSV(RGB(pvel->vmidR, pvel->vmidG, pvel->vmidB), RGB(pvel->vmidR2, pvel->vmidG2, pvel->vmidB2),
			pvel->vmid, pvel->vmid2, v);
	}
	else if (v <= pvel->vmid3) {
		cout = interp1D_HSV(RGB(pvel->vmidR2, pvel->vmidG2, pvel->vmidB2), RGB(pvel->vmidR3, pvel->vmidG3, pvel->vmidB3),
			pvel->vmid2, pvel->vmid3, v);
	}
	else if (v <= pvel->vmid4) {
		cout = interp1D_HSV(RGB(pvel->vmidR3, pvel->vmidG3, pvel->vmidB3), RGB(pvel->vmidR4, pvel->vmidG4, pvel->vmidB4),
			pvel->vmid3, pvel->vmid4, v);
	}
	else if (v <= pvel->vmid5) {
		cout = interp1D_HSV(RGB(pvel->vmidR4, pvel->vmidG4, pvel->vmidB4), RGB(pvel->vmidR5, pvel->vmidG5, pvel->vmidB5),
			pvel->vmid4, pvel->vmid5, v);
	}
	else if (v <= pvel->vmid6) {
		cout = interp1D_HSV(RGB(pvel->vmidR5, pvel->vmidG5, pvel->vmidB5), RGB(pvel->vmidR6, pvel->vmidG6, pvel->vmidB6),
			pvel->vmid5, pvel->vmid6, v);
	}
	else if (v <= pvel->vmid7) {
		cout = interp1D_HSV(RGB(pvel->vmidR6, pvel->vmidG6, pvel->vmidB6), RGB(pvel->vmidR7, pvel->vmidG7, pvel->vmidB7),
			pvel->vmid6, pvel->vmid7, v);
	}
	else if (v <= pvel->vmid8) {
		cout = interp1D_HSV(RGB(pvel->vmidR7, pvel->vmidG7, pvel->vmidB7), RGB(pvel->vmidR8, pvel->vmidG8, pvel->vmidB8),
			pvel->vmid7, pvel->vmid8, v);
	}
	else if (v <= pvel->vmid9) {
		cout = interp1D_HSV(RGB(pvel->vmidR8, pvel->vmidG8, pvel->vmidB8), RGB(pvel->vmidR9, pvel->vmidG9, pvel->vmidB9),
			pvel->vmid8, pvel->vmid9, v);
	}
	else if (v <= pvel->vmax) {
		cout = interp1D_HSV(RGB(pvel->vmidR9, pvel->vmidG9, pvel->vmidB9), RGB(pvel->vmaxR, pvel->vmaxG, pvel->vmaxB),
			pvel->vmid9, pvel->vmax, v);
	}
	else {
	}

	return cout;
}


COLORREF CColorHelper::getVelocityColorDepreciatedDelete(float v, CColorHelper::VELCOLOR vel)
{
	// convert velocity to colors based on the mapping in vel struct
	int vR, vG, vB;
	if (v < vel.vmin) {
		vR = 255;
		vG = 255;
		vB = 255;
	}
	else if (v <= vel.vmid) {
		vR = (int)(vel.vminR + (vel.vmidR - vel.vminR) /
			(vel.vmid - vel.vmin)*(v - vel.vmin));
		vG = (int)(vel.vminG + (vel.vmidG - vel.vminG) /
			(vel.vmid - vel.vmin)*(v - vel.vmin));
		vB = (int)(vel.vminB + (vel.vmidB - vel.vminB) /
			(vel.vmid - vel.vmin)*(v - vel.vmin));
	}
	else if (v <= vel.vmid2) {
		vR = (int)(vel.vmidR + (vel.vmidR2 - vel.vmidR) /
			(vel.vmid2 - vel.vmid)*(v - vel.vmid));
		vG = (int)(vel.vmidG + (vel.vmidG2 - vel.vmidG) /
			(vel.vmid2 - vel.vmid)*(v - vel.vmid));
		vB = (int)(vel.vmidB + (vel.vmidB2 - vel.vmidB) /
			(vel.vmid2 - vel.vmid)*(v - vel.vmid));
	}
	else if (v <= vel.vmid3) {
		vR = (int)(vel.vmidR2 + (vel.vmidR3 - vel.vmidR2) /
			(vel.vmid3 - vel.vmid2)*(v - vel.vmid2));
		vG = (int)(vel.vmidG2 + (vel.vmidG3 - vel.vmidG2) /
			(vel.vmid3 - vel.vmid2)*(v - vel.vmid2));
		vB = (int)(vel.vmidB2 + (vel.vmidB3 - vel.vmidB2) /
			(vel.vmid3 - vel.vmid2)*(v - vel.vmid2));
	}
	else if (v <= vel.vmid4) {
		vR = (int)(vel.vmidR3 + (vel.vmidR4 - vel.vmidR3) /
			(vel.vmid4 - vel.vmid3)*(v - vel.vmid3));
		vG = (int)(vel.vmidG3 + (vel.vmidG4 - vel.vmidG3) /
			(vel.vmid4 - vel.vmid3)*(v - vel.vmid3));
		vB = (int)(vel.vmidB3 + (vel.vmidB4 - vel.vmidB3) /
			(vel.vmid4 - vel.vmid3)*(v - vel.vmid3));
	}

	else if (v <= vel.vmid5) {
		vR = (int)(vel.vmidR4 + (vel.vmidR5 - vel.vmidR4) /
			(vel.vmid5 - vel.vmid4)*(v - vel.vmid4));
		vG = (int)(vel.vmidG4 + (vel.vmidG5 - vel.vmidG4) /
			(vel.vmid5 - vel.vmid4)*(v - vel.vmid4));
		vB = (int)(vel.vmidB4 + (vel.vmidB5 - vel.vmidB4) /
			(vel.vmid5 - vel.vmid4)*(v - vel.vmid4));
	}


	else if (v <= vel.vmid6) {
		vR = (int)(vel.vmidR5 + (vel.vmidR6 - vel.vmidR5) /
			(vel.vmid6 - vel.vmid5)*(v - vel.vmid5));
		vG = (int)(vel.vmidG5 + (vel.vmidG6 - vel.vmidG5) /
			(vel.vmid6 - vel.vmid5)*(v - vel.vmid5));
		vB = (int)(vel.vmidB5 + (vel.vmidB6 - vel.vmidB5) /
			(vel.vmid6 - vel.vmid5)*(v - vel.vmid5));
	}



	else if (v <= vel.vmid7) {
		vR = (int)(vel.vmidR6 + (vel.vmidR7 - vel.vmidR6) /
			(vel.vmid7 - vel.vmid6)*(v - vel.vmid6));
		vG = (int)(vel.vmidG6 + (vel.vmidG7 - vel.vmidG6) /
			(vel.vmid7 - vel.vmid6)*(v - vel.vmid6));
		vB = (int)(vel.vmidB6 + (vel.vmidB7 - vel.vmidB6) /
			(vel.vmid7 - vel.vmid6)*(v - vel.vmid6));
	}

	else if (v <= vel.vmid8) {
		vR = (int)(vel.vmidR7 + (vel.vmidR8 - vel.vmidR7) /
			(vel.vmid8 - vel.vmid7)*(v - vel.vmid7));
		vG = (int)(vel.vmidG7 + (vel.vmidG8 - vel.vmidG7) /
			(vel.vmid8 - vel.vmid7)*(v - vel.vmid7));
		vB = (int)(vel.vmidB7 + (vel.vmidB8 - vel.vmidB7) /
			(vel.vmid8 - vel.vmid7)*(v - vel.vmid7));
	}



	else if (v <= vel.vmax) {
		vR = (int)(vel.vmidR8 + (vel.vmaxR - vel.vmidR8) /
			(vel.vmax - vel.vmid8)*(v - vel.vmid8));
		vG = (int)(vel.vmidG8 + (vel.vmaxG - vel.vmidG8) /
			(vel.vmax - vel.vmid8)*(v - vel.vmid8));
		vB = (int)(vel.vmidB8 + (vel.vmaxB - vel.vmidB8) /
			(vel.vmax - vel.vmid8)*(v - vel.vmid8));
	}
	else {
		vR = 255;
		vG = 255;
		vB = 255;
	}

	if (vR < 0)vR = 0;
	if (vG < 0)vG = 0;
	if (vB < 0)vB = 0;
	if (vR > 255) vR = 255;
	if (vG > 255) vG = 255;
	if (vB > 255) vB = 255;

	return RGB(vR, vG, vB);

}





//
//bool CColorHelper::getColorRef4Amp(float ampls[], COLORREF colors[], int n, 
//	CColorHelper::AMPCOLORDYN *ampColorDyn2)
//{
//	if (n < 1) return false;
//	int ic, j;
//	if (ampColorDyn2->num == 0) {
//		for (j = 0; j < n; j++)
//			colors[j] = WHITE;
//		return true;
//	}
//
//
//	// this function will be very deep in loop. So we want it to be as fast as possible!!! So we drop complicated calculation
//	int iLast = ampColorDyn2->num - 1;
//	for (j = 0; j < n; j++) {
//		if (ampls[j] <= ampColorDyn2->percentAmp[0])
//			colors[j] = ampColorDyn2->rgb[0];
//		else if (ampls[j] >= ampColorDyn2->percentAmp[iLast])
//			colors[j] = ampColorDyn2->rgb[iLast];
//		else {
//			for (ic = 1; ic < ampColorDyn2->num; ic++) {
//				if (ampls[j] >= ampColorDyn2->percentAmp[ic - 1] 
//					&& ampls[j] <= ampColorDyn2->percentAmp[ic]) {
//					// register double delta = ampColorDyn2->percentAmp[i] - ampColorDyn2->percentAmp[i-1];
//					register double f = (ampls[j] - ampColorDyn2->percentAmp[ic - 1]) 
//						* ampColorDyn2->DeltaAmp[ic];
//					colors[j] = 
//						interpolateHSV(ampColorDyn2->rgb[ic - 1], ampColorDyn2->rgb[ic], (float)f);
//					break; // ic loop
//				}
//			} // for ic
//		}
//	} // for j
//
//
//	// now the rate is not covered. This cannot happen unless program errors
//	return true;
//}


//
//COLORREF CColorHelper::getColorRef4Amp(double ampl, CColorHelper::AMPCOLORDYN *ampColorDyn2)
//{
//	if (ampColorDyn2->num == 0) return WHITE;
//	if (ampl <= ampColorDyn2->percentAmp[0])
//		return ampColorDyn2->rgb[0];
//
//	int iLast = ampColorDyn2->num - 1;
//	if (ampl >= ampColorDyn2->percentAmp[iLast])
//		return ampColorDyn2->rgb[iLast];
//
//	for (int i = 1; i < ampColorDyn2->num; i++) {
//		if (ampl >= ampColorDyn2->percentAmp[i - 1] && ampl <= ampColorDyn2->percentAmp[i]) {
//			// this function will be very deep in loop. So we want it to be as fast as possible!!! So we drop complicated calculation
//			// register double delta = ampColorDyn2->percentAmp[i] - ampColorDyn2->percentAmp[i-1];
//			register double f = (ampl - ampColorDyn2->percentAmp[i - 1]) * ampColorDyn2->DeltaAmp[i];
//			return interpolateHSV(ampColorDyn2->rgb[i - 1], ampColorDyn2->rgb[i], (float)f);
//		}
//	}
//
//	// now the rate is not covered. This cannot happen unless program errors
//	return WHITE;
//}


// this function does not use   AMPCOLORDYN  struct, so it can have more than 11 colors or contours defined. Need more testing !!!
COLORREF CColorHelper::getColorRef4Amp(double ampl, int numColors, float percentAmp[], COLORREF colArr[])
{
//	static COLORREF white = RGB(255, 255, 255);

	if (numColors == 0) return WHITE;
	if (ampl <= percentAmp[0]) return colArr[0];

	int iLast = numColors - 1;
	if (ampl >= percentAmp[iLast]) return colArr[iLast];

	for (int i = 1; i < numColors; i++) {
		if (ampl >= percentAmp[i - 1] && ampl <= percentAmp[i]) {
			// this function will be very deep in loop. So we want it to be as fast as possible!!! So we drop complicated calculation
			register double delta = percentAmp[i] - percentAmp[i - 1];
			register double f = (delta == 0.f) ? 0.f : (ampl - percentAmp[i - 1]) / delta;
			register BYTE r = GetRValue(colArr[i - 1]);
			register BYTE g = GetGValue(colArr[i - 1]);
			register BYTE b = GetBValue(colArr[i - 1]);
			return RGB((int)(0.5 + r + (GetRValue(colArr[i]) - r) * f),
				(int)(0.5 + g + (GetGValue(colArr[i]) - g) * f),
				(int)(0.5 + b + (GetBValue(colArr[i]) - b) * f));
		}
	}

	// now the rate is not covered. This cannot happen unless program errors
	return WHITE;
}




//int CColorHelper::getColorIndex4AmpDepreciated(float ampl, float *percentAmpExpanded, int numContours) {
//	if (numContours == 0) return 0;
//	// int numContours = ampColorDyn.numContours;
//	if (ampl <= percentAmpExpanded[0])
//		return 0;
//	else if (ampl >= percentAmpExpanded[numContours - 1]) {
//		return numContours - 1;
//	}
//
//	for (int i = 1; i < numContours; i++) {
//
//		if (isInsideFloat(ampl, percentAmpExpanded[i - 1], percentAmpExpanded[i])) {
//			// this function will be very deep in loop. So we want it to be as fast as possible!!! So we drop complicated calculation
//			//if ( fabs(*amp-percentAmpExpanded[i-1]) < fabs(percentAmpExpanded[i]-*amp) ) return i-1;
//			//else return i;
//			return i - 1;
//		}
//	}
//
//	// now the rate is not covered. This cannot happen unless program erros
//	//	myCaption(_T("Not covered"));
//	return numContours / 2;
//}
//


BOOL CColorHelper::getDefaultAmpColor(CColorHelper::AMPCOLORDYN *pAmpCol,
	int colorScaleCode, bool isUpdatePercent)
{
	if (pAmpCol->num != 11) {
		pAmpCol->initAmpColorArray(11);
		//only output 11 colors!!!
	}
	int i;
	if (colorScaleCode == 0) {
		// black and white grey
		static const int bk_r[11] = { 255, 240, 215, 199, 189, 169, 119, 87, 63, 40, 0 };
		for (i = 0; i < pAmpCol->num; i++)
			pAmpCol->colorRef[i] = RGB(bk_r[i], bk_r[i], bk_r[i]);
	}
	else if (colorScaleCode == 1) {
		// blue and red tainted
		static const int col_r[11] = { 0, 0, 51, 153, 202, 255, 255, 255, 255, 254, 254 };
		static const int col_g[11] = { 102, 153, 153, 204, 203, 255, 254, 255, 202, 67, 0 };
		static const int col_b[11] = { 204, 255, 255, 255, 255, 255, 201, 152, 101, 65, 0 };
		for (i = 0; i < pAmpCol->num; i++)
			pAmpCol->colorRef[i] = RGB(col_r[i], col_g[i], col_b[i]);
	}
	else if (colorScaleCode == 2) {
		// more colors
		//static const int col_g[11] = {0, 100,  200, 255,  253, 255, 200, 112,   0, 32,    0};
		static const int col_r[11] = { 0, 20, 20, 20, 90, 255, 215, 185, 150, 200, 255 };
		static const int col_g[11] = { 255, 200, 80, 50, 51, 255, 215, 185, 125, 98, 0 };
		static const int col_b[11] = { 0, 60, 180, 205, 253, 255, 180, 100, 60, 0, 0 };
		for (i = 0; i < pAmpCol->num; i++)
			pAmpCol->colorRef[i] = RGB(col_r[i], col_g[i], col_b[i]);
	}
	else if (colorScaleCode == 4) {
		// bright-gray mode
		static const int bk_r[11] = { 255, 251, 248, 244, 240, 169, 119, 87, 63, 40, 0 };
		for (i = 0; i < pAmpCol->num; i++)
			pAmpCol->colorRef[i] = RGB(bk_r[i], bk_r[i], bk_r[i]);
	}
	else if (colorScaleCode == 5) {
		// Bright-Low Amp mode
		// static const int bk_r[11] = { 0, 40, 63, 87, 119, 169, 240, 244, 248, 251, 255 };
		static const int bk_r[11] = { 255, 251, 248, 244, 240, 86, 136, 168, 192, 215, 255 };
		for (i = 0; i < pAmpCol->num; i++)
			pAmpCol->colorRef[i] = RGB(bk_r[i], bk_r[i], bk_r[i]);
	}
	else if (colorScaleCode == 6) {
		// more colors 2: Blue-Black
		//static const int col_r[11] = { 0, 63, 119, 189, 215, 255, 201, 152, 101, 65, 0 };
		//static const int col_g[11] = { 0, 63, 119, 189, 215, 255, 254, 255, 202, 128, 0 };
		//static const int col_b[11] = { 0, 63, 119, 189, 215, 255, 255, 255, 255, 254, 254 };
		static const int col_r[11] = { 0, 65, 101, 152, 201, 255, 215, 189, 119, 63, 0 };
		static const int col_g[11] = { 0, 128, 202, 255, 254, 255, 215, 189, 119, 63, 0 };
		static const int col_b[11] = { 254, 254, 255, 255, 255, 255, 215, 189, 119, 63, 0 };

		for (i = 0; i < pAmpCol->num; i++)
			pAmpCol->colorRef[i] = RGB(col_r[i], col_g[i], col_b[i]);
	}
	else if (colorScaleCode == 7) {
		// more colors 2: Red-Blue-Yellow (i.e. yellow to be positive amp, red to be negative amp, blue to be zero
		static const int col_r[11] = { 255, 255, 255, 150, 65, 0,   65, 110, 255, 255, 255 };
		static const int col_g[11] = { 0,   67,  150, 150, 65, 0,   65, 110, 255, 255, 255 };
		static const int col_b[11] = { 0,   65,  150, 255, 255,255, 215,210, 119, 63,  0   };
		for (i = 0; i < pAmpCol->num; i++)
			pAmpCol->colorRef[i] = RGB(col_r[i], col_g[i], col_b[i]);
	}
	else if (colorScaleCode == 8) {
		// industry standard type: blue-white-red-yellow
		static const int col_r[11] = { 0,  20, 20, 20, 90, 255, 255, 255, 255, 255,255 };
		static const int col_g[11] = { 0,  30, 70, 80, 150,255, 100, 80,  50,  150,255 };
		static const int col_b[11] = {255, 200,150,100,151,255, 150, 100, 70,  40,  0 };
		for (i = 0; i < pAmpCol->num; i++)
			pAmpCol->colorRef[i] = RGB(col_r[i], col_g[i], col_b[i]);
	}
	else {
		// black and red tainted  colorScaleCode == 3
		static const int bk_r[11] = { 0, 63, 119, 189, 215, 255, 255, 255, 255, 254, 254 };
		static const int bk_g[11] = { 0, 63, 119, 189, 215, 255, 254, 255, 202, 128, 0 };
		static const int bk_b[11] = { 0, 63, 119, 189, 215, 255, 201, 152, 101, 65, 0 };
		for (i = 0; i < pAmpCol->num; i++)
			pAmpCol->colorRef[i] = RGB(bk_r[i], bk_g[i], bk_b[i]);
	}

	// need to backup the percentage before we lose them by initialize array
	float temp_p[11];
	if (pAmpCol->percentAmp == NULL) isUpdatePercent = true;
	else memcpy(temp_p, pAmpCol->percentAmp, sizeof(float) * 11);

	if (isUpdatePercent) {
		//static const float col_p[11] = {-1.f, -0.77f, -0.51f, -0.42f, -0.30f, 0.f,
		//	0.30f, 0.42f, 0.51f, 0.77f, 1.f};
		static const float col_p[11] = {-1.f, -0.9f, -0.7f, -0.4f, -0.1f, 0.f,
			0.1f, 0.4f, 0.7f, 0.9f, 1.f};
		//static const float col_p[11] = { -1.f, -0.35f, -0.3f, -0.2f, -0.1f, 0.f,
		//	0.1f, 0.2f, 0.3f, 0.35f, 1.f };
		memcpy(pAmpCol->percentAmp, col_p, sizeof(float) * 11);
	}
	else
		memcpy(pAmpCol->percentAmp, temp_p, sizeof(float) * 11);

	return TRUE;
}


//void CColorHelper::getAmpColorArrayFromSegyFile(
//	// CString fSegy, 
//	double ampMax, 
//	int logScaleThreshold,
//	CColorHelper::AMPCOLORDYN *pAmpColorDyn)
//{
//	 // only used in color mode
////	CString colFileName = GetPathFromFileSlash(pAmpColorDyn->fileName) + _T("colPalette.txt");
////	CColorHelper::initAmpColorArray(pAmpColorDyn, 11);
//	CColorHelper::getAmpColorArray(pAmpColorDyn, ampMax, false,
//		logScaleThreshold);
//}

BOOL CColorHelper::saveAmpColorToFile2(CColorHelper::AMPCOLORDYN *pAmpCol, CString f)
{
	CStringArray *cstrArr = new CStringArray();
	CString tStr;

	int num = pAmpCol->num;

	// there is a severe problem: when the user has specified non-mono values for pAmpColor11->percentAmp[]
	//tStr.Format(_T("%g %g %g"), pAmpColor11->percentAmp[4], pAmpColor11->percentAmp[5], pAmpColor11->percentAmp[6]);
	//AfxMessageBox(tStr);

	// prob: pAmpColor11->percentAmp[5] is always equals to pAmpColor11->percentAmp[6] 
	// in release version only, why???
	if (num > 5 && pAmpCol->percentAmp[5] == pAmpCol->percentAmp[6])
		pAmpCol->percentAmp[5] = 0.f;


	pAmpCol->percentAmp[0] = -1.f;
	for (int i = 1; i<num-1; i++) {
		if (pAmpCol->percentAmp[i + 1] <= pAmpCol->percentAmp[i])
			pAmpCol->percentAmp[i + 1] = pAmpCol->percentAmp[i] + 0.001f;
	}
	pAmpCol->percentAmp[num-1] = 1.f;
	tStr.Format(_T("#Auto generated color index file applied to all segy in the same directory only. Safe to delete."));	cstrArr->Add(tStr);
	tStr.Format(_T("#r      g     b    amp (relative; commonly start from -1  to 1; however, if amplitudes found all positive,"));	cstrArr->Add(tStr);
	tStr.Format(_T("#amp will be adjusted to 0-1 proportionally) (You may manually edit these texts.)"));	cstrArr->Add(tStr);
	tStr.Format(_T("#Note that actual color scaling of each segy file will also be adjusted in GUI (outside of this palette) "));	cstrArr->Add(tStr);
	tStr.Format(_T("#by setting 'log scale threshold' (default 70%%); amplitudes above threshold will use log scale "));	cstrArr->Add(tStr);
	tStr.Format(_T("#while below it will adopt linear scale. Lower thresjold to emphasize weaker events. Square amplitudes to suppress weaker events."));	cstrArr->Add(tStr);

	for (int i = 0; i<num; i++) {
		tStr.Format(_T("%3d  %3d  %3d  %f"), 
			GetRValue(pAmpCol->colorRef[i]),
			GetGValue(pAmpCol->colorRef[i]),
			GetBValue(pAmpCol->colorRef[i]),
			pAmpCol->percentAmp[i]);
		cstrArr->Add(tStr);
	}
	BOOL isTrue = saveStringArrayToFile(cstrArr, f);
	cstrArr->RemoveAll();
	delete cstrArr;

	return isTrue;
}


BOOL CColorHelper::saveAmpColorToFile1(CColorHelper::AMPCOLORDYN *pAmpCol)
{
	CString colFileName = GetPathFromFileSlash(pAmpCol->fileName) + _T("colPalette.txt");
	return saveAmpColorToFile2(pAmpCol, colFileName);
}



//
//BOOL CColorHelper::getAmpColorArray2(
//	CColorHelper::AMPCOLORDYN *pAmpColor11,
//	int colorScaleCode, bool isUpdatePercent)
//{
//	// colorScaleCode = 0:  black and white; 1=bi-color; 2=quad-color
//	BOOL isSuc = FALSE;
//	int r, g, b;
//
//	// pAmpColor11->fileName must have been set during struct initializing
//	CString colFileName = GetPathFromFileSlash(pAmpColor11->fileName) + _T("colPalette.txt");
//
//	
//	CColorHelper::AMPCOLORDYN ampColorDlg(colFileName);
//
//
//	CStringArray *arr = getLinesFromFile(colFileName);
//	if (arr == NULL || arr->GetSize() < 2) {
//		// the color palette file is invalid, so we create a default
//		int num = 11;
//		if (!ampColorDlg.initAmpColorArray(num)){
//			arr->RemoveAll();
//			delete arr;
//			return FALSE;
//		}
//
//
//		if (colFileName.Find(_T("rms")) >= 0) {
//			//static const int rms_r[11] = {0,   64,  160, 216, 255, 255, 254, 254, 254, 254, 254};
//			//static const int rms_g[11] = {102, 147, 221, 246, 255, 224, 118,  57,  17,   1,   0};
//			//static const int rms_b[11] = {204, 211, 127,  41,   0,   1,   1,   1,   1,   1,   0};
//			static const int rms_r[11] = { 0, 21, 149, 255, 255, 255, 255, 255, 255, 254, 254 };
//			static const int rms_g[11] = { 102, 117, 194, 255, 246, 193, 148, 132, 97, 24, 0 };
//			static const int rms_b[11] = { 204, 206, 105, 0, 0, 1, 1, 1, 1, 0, 0 };
//			for (int i = 0; i < ampColorDlg.num; i++)
//				ampColorDlg.rgb[i] = RGB(rms_r[i], rms_g[i], rms_b[i]);
//			if (isUpdatePercent) {
//				static const float rms_p[11] =
//				{ 0.218636f, 0.453930f, 0.570588f, 0.591002f, 0.592703f, 0.594404f, 0.700166f,
//				0.804227f, 0.831786f, 0.870486f, 0.900000f };
//				memcpy(ampColorDlg.percentAmp, rms_p, sizeof(float) * 11);
//			}
//		}
//		else {
//			CColorHelper::getDefaultAmpColor(&ampColorDlg, colorScaleCode, isUpdatePercent);
//		}
//		ampColorDlg.num = num;
//		isSuc = TRUE;
//	}
//	else {
//		int num = (int)arr->GetSize();
//		if (ampColorDlg.initAmpColorArray(num)) {
//			for (int i = 0; i<num; i++) {
//				_stscanf_s(arr->ElementAt(i).Trim(), _T("%d%d%d%f"), &r, &g, &b, &ampColorDlg.percentAmp[i]);
//				ampColorDlg.rgb[i] = RGB(r, g, b);
//			}
//			ampColorDlg.num = num;
//			isSuc = TRUE;
//		}
//
//	} // else
//
//	if (arr) {
//		arr->RemoveAll();
//		delete arr;
//	}
//
//	// Note: Although ampColorDlg struct is designed to have any number of elements, but outputs 
//	// m_ampColorref[] and m_ampPercent[]  will only be 11 elements.
//	pAmpColor11->initAmpColorArray(11);
//	if (ampColorDlg.num == 11) {
//		for (int i = 0; i<11; i++) {
//			pAmpColor11->rgb[i] = ampColorDlg.rgb[i];
//			if (isUpdatePercent)
//				pAmpColor11->percentAmp[i] = ampColorDlg.percentAmp[i];
//		}
//	}
//	else if (ampColorDlg.num > 11) {
//		// now need to interpolate
//		int j;  // for indexing in ampColorDlg struct
//		for (int i = 0; i<11; i++) {
//			j = (int)((double)i * .1 * (double)(ampColorDlg.num - 1) + 0.5);
//			if (j > ampColorDlg.num - 1) j = ampColorDlg.num - 1;
//			pAmpColor11->rgb[i] = ampColorDlg.rgb[j];
//			if (isUpdatePercent)
//				pAmpColor11->percentAmp[i] = ampColorDlg.percentAmp[j];
//		}
//	}
//
//
//	ampColorDlg.freeAmpColorArray();
//
//	return isSuc;
//}


//// this is the only function that connects to CdlgSegyColor2 to get the user selected color pallete
//BOOL CColorHelper::getAmpColorArray(
//	CColorHelper::AMPCOLORDYN *ampColorOut,
//	double amaxVal,
//	bool isPositive,
//	double logScaleThreshold)
//{
//
//	// ampColorOut->fileName must have been set during struct initializing
//	CString colFileName = GetPathFromFileSlash(ampColorOut->fileName) + _T("colPalette.txt");
//
//	//	CdlgSegyColor2 color2;
//	CColorHelper::AMPCOLORDYN ampColorIn(colFileName);  // must be 11 colors
//	if (!CColorHelper::getAmpColorArray2(&ampColorIn)) return FALSE;
//
//	// note that I have made sure that color2.ampColorDyn only has 11 items!!!
//	if (ampColorIn.num != 11) return FALSE;
//
//	if (!ampColorOut->initAmpColorArray(11)) return FALSE;
//
//	double delta;
//	double rate;
//	int i;
//	for (i = 0; i < ampColorOut->num; i++) {
//		ampColorOut->rgb[i] = ampColorIn.rgb[i];
//	}
//
//	if (colFileName.GetLength() > 3 && colFileName.Find(_T("rms")) < 0 && !isPositive) {
//		// now convert ampColorDyn.percent[i]  to   real amplitude levels
//		delta = ampColorIn.percentAmp[5] - ampColorIn.percentAmp[0];
//		rate = (fabs(delta)<0.001) ? 0. : 1. / delta;
//		ampColorOut->percentAmp[0] = -1.f;
//		ampColorOut->percentAmp[1] = -1.f + (ampColorIn.percentAmp[1] - ampColorIn.percentAmp[0])*(float)rate;
//		ampColorOut->percentAmp[2] = -1.f + (ampColorIn.percentAmp[2] - ampColorIn.percentAmp[0])*(float)rate;
//		ampColorOut->percentAmp[3] = -1.f + (ampColorIn.percentAmp[3] - ampColorIn.percentAmp[0])*(float)rate;
//		ampColorOut->percentAmp[4] = -1.f + (ampColorIn.percentAmp[4] - ampColorIn.percentAmp[0])*(float)rate;
//		ampColorOut->percentAmp[5] = 0.f;
//
//		delta = ampColorIn.percentAmp[10] - ampColorIn.percentAmp[5];
//		rate = (fabs(delta)<0.001) ? 0. : 1. / delta;
//		ampColorOut->percentAmp[6] = (ampColorIn.percentAmp[6] - ampColorIn.percentAmp[5])*(float)rate;
//		ampColorOut->percentAmp[7] = (ampColorIn.percentAmp[7] - ampColorIn.percentAmp[5])*(float)rate;
//		ampColorOut->percentAmp[8] = (ampColorIn.percentAmp[8] - ampColorIn.percentAmp[5])*(float)rate;
//		ampColorOut->percentAmp[9] = (ampColorIn.percentAmp[9] - ampColorIn.percentAmp[5])*(float)rate;
//		ampColorOut->percentAmp[10] = 1.f;
//	} // if
//	else {
//		for (i = 0; i < ampColorOut->num; i++) {
//			delta = ampColorIn.percentAmp[ampColorOut->num - 1] - ampColorIn.percentAmp[0];
//			rate = (fabs(delta)<0.001) ? 0. : 1. / delta;
//			ampColorOut->percentAmp[i] = (ampColorIn.percentAmp[i] - ampColorIn.percentAmp[0])*(float)rate; 
//		} // for i
//	}
//
//	double fac = logScaleThreshold * 0.01;
//	double logAmpCut = fac*logScalingPure(amaxVal);  // if amax=10,000 , logScalingPure(amax)=4
//	double ampCut = logScalingInvPure(logAmpCut);  // this way, the ampThreshald can be 100 when amax=10,000
//	double amax = logScalingPure(amaxVal);
//	for (i = 0; i < ampColorOut->num; i++) {
//		ampColorOut->percentAmp[i] *= (float)amax;
//	} // for i
//
//	ampColorIn.freeAmpColorArray();
//
//	for (i = 0; i < ampColorOut->num; i++) {
//		register double x = fabs(ampColorOut->percentAmp[i]);
//		if (x < logAmpCut) {
//			// use linear 
//			ampColorOut->percentAmp[i] = (float)(sgn(ampColorOut->percentAmp[i]) * ampCut * x / logAmpCut);
//		}
//		else {
//			ampColorOut->percentAmp[i] = (float)logScalingInvPure(ampColorOut->percentAmp[i]);
//		}
//	} // for i
//	 
//
//	for (i = 1; i < ampColorOut->num; i++) {
//		delta = ampColorOut->percentAmp[i] - ampColorOut->percentAmp[i - 1];
//		rate = (fabs(delta)<0.001) ? 0. : 1. / delta;
//		ampColorOut->DeltaAmp[i] = (float)rate;
//	}
//	ampColorOut->DeltaAmp[0] = 0.;
//
//
//	return TRUE;
//}

//
//bool CColorHelper::getExpandedColorTableDepreciated(COLORREF *colArr,
//	float percentAmpExpanded[],
//	int numExpanded,
//	CString colFileName, BOOL isAvgEnergy, double maxAmp, double logScaleThreshold) {
//
//	CColorHelper::AMPCOLORDYN ampColorDyn2(colFileName);
//	ampColorDyn2.initAmpColorArray(11);
//
//	// CColorHelper::getAmpColorArray(&ampColorDyn2, false); // we always read the color table from file whenever plotting starts
//	ampColorDyn2.getAmpColorArray(
//		isAvgEnergy ? maxAmp*maxAmp : maxAmp,
//		false,
//		logScaleThreshold);
//
//	int num = ampColorDyn2.num;
//	if (num == numExpanded) {
//		for (int i = 0; i < numExpanded; i++) {
//			colArr[i] = ampColorDyn2.colorRef[i]; // , ampColorDyn2.g[i], ampColorDyn2.b[i]);
//			percentAmpExpanded[i] = ampColorDyn2.percentAmp[i];
//		}
//		ampColorDyn2.freeAmpColorArray();
//		return true;
//	}
//
//	//int r, g, b;
//	for (int i = 0; i < numExpanded; i++) {
//		float ai = (float)i / (float)numExpanded * (float)(num - 1);
//
//		// ai is for the original color table, must be inside [0, num-1]
//
//		int iai = (int)ai;
//		if (iai > num - 1) {
//			iai = num - 1;
//			ai = (float)iai;
//		}
//
//		if (iai == ai || iai + 1 > num - 1) {
//			colArr[i] = ampColorDyn2.colorRef[iai];
//			percentAmpExpanded[i] = ampColorDyn2.percentAmp[iai];
//		}
//		else {
//			colArr[i] = CColorHelper::interpolateHSV(ampColorDyn2.colorRef[iai], ampColorDyn2.colorRef[iai + 1], (float)(ai - iai));
//			percentAmpExpanded[i] = ampColorDyn2.percentAmp[iai] +
//				(ampColorDyn2.percentAmp[iai + 1] - ampColorDyn2.percentAmp[iai])*(float)(ai - iai);
//		}
//
//	} // for i
//	ampColorDyn2.freeAmpColorArray();
//	return true;
//}


double CColorHelper::getInterpol4Points(double x,
	double x0, double x1, double x2, double x3,
	double y0, double y1, double y2, double y3)
{
	// we now need to interpolate
	double dx = x2 - x1;
	if (dx == 0.f) {
		return y1;
	}
	double t = (x - x1) / dx;
	double h00 = (1. + 2.*t)*(1. - t)*(1. - t);
	double h10 = t*(1. - t)*(1. - t);
	double t2 = t*t;
	double h01 = t2*(3. - 2.*t);
	double h11 = t2*(t - 1.);
	double mkNear = (y2 - y1) / dx;
	double mk = mkNear;
	double dx0 = x1 - x0;
	if (dx0 != 0.) {
		mk += (y1 - y0) / dx0;
		mk *= 0.5;
	}

	double mk1 = mkNear;
	double dx2 = x3 - x2;
	if (dx2 != 0.) {
		mk1 += (y3 - y2) / dx2;
		mk1 *= 0.5;
	}

	return h00*y1 + h10*dx*mk + h01*y2 + h11*dx*mk1;
}

void CColorHelper::getInterpol4Points(
	float x0, float x1, float x2, float x3,
	float y0[], float y1[], float y2[], float y3[],
	float x, int num,
	float yOut[])
{
	// we now need to interpolate
	memcpy(yOut, y1, num*sizeof(float));

	double dx = x2 - x1;
	if (dx == 0.) return;
	double t = (x - x1) / dx;
	double h00 = (1. + 2.*t)*(1. - t)*(1. - t);
	double h10 = t*(1. - t)*(1. - t);
	double h01 = t*t*(3. - 2.*t);
	double h11 = t*t*(t - 1.);

	double dx0 = x1 - x0;
	double dx2 = x3 - x2;
	double mk, mk1;
	for (int i = 0; i < num; i++) {
		if (y0[i] == 0. && y1[i] == 0. && y2[i] == 0. && y3[i] == 0.)
			continue;

		mk = mk1 = (y2[i] - y1[i]) / dx;
		if (dx0 != 0.) {
			mk += (y1[i] - y0[i]) / dx0;
			mk *= 0.5;
		}

		if (dx2 != 0.) {
			mk1 += (y3[i] - y2[i]) / dx2;
			mk1 *= 0.5;
		}
		yOut[i] = (float)(h00*y1[i] + h10*dx*mk + h01*y2[i] + h11*dx*mk1);
	}
}



COLORREF CColorHelper::HsvToRgbi(HsvColor hsv)
{
	COLORREF rgb;
	unsigned char region, p, q, t;
	unsigned int h, s, v, remainder;

	// converting to 16 bit to prevent overflow
	h = hsv.h;
	s = hsv.s;
	v = hsv.v;

	if (s == 0) {
		return RGB(v, v, v);
	}

	region = h / 43;
	remainder = (h - (region * 43)) * 6;

	p = (v * (255 - s)) >> 8;
	q = (v * (255 - ((s * remainder) >> 8))) >> 8;
	t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

	switch (region)
	{
	case 0:
		rgb = RGB(v, t, p);
		break;
	case 1:
		rgb = RGB(q, v, p);
		break;
	case 2:
		rgb = RGB(p, v, t);
		break;
	case 3:
		rgb = RGB(p, q, v);
		break;
	case 4:
		rgb = RGB(t, p, v);
		break;
	default:
		rgb = RGB(v, p, q);
		break;
	}

	return rgb;
}

CColorHelper::HsvColor CColorHelper::RgbToHsvi(COLORREF rgb)
{
	HsvColor hsv;
	unsigned char rgbMin, rgbMax;
	unsigned char r = GetRValue(rgb);
	unsigned char g = GetGValue(rgb);
	unsigned char b = GetBValue(rgb);;

	rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
	rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);

	hsv.v = rgbMax;
	if (hsv.v == 0) {
		hsv.h = 0;
		hsv.s = 0;
		return hsv;
	}

	hsv.s = 255 * ((long)(rgbMax - rgbMin)) / hsv.v;
	if (hsv.s == 0) {
		hsv.h = 0;
		return hsv;
	}

	if (rgbMax == r)
		hsv.h = 43 * (g - b) / (rgbMax - rgbMin);
	else if (rgbMax == g)
		hsv.h = 85 + 43 * (b - r) / (rgbMax - rgbMin);
	else
		hsv.h = 171 + 43 * (r - g) / (rgbMax - rgbMin);

	return hsv;
}
//
//CColorHelper::rgb CColorHelper::rgbRef2Dec2(COLORREF in)
//{
//	CColorHelper::rgb out;
//	out.r = GetRValue(in);
//	out.g = GetGValue(in);
//	out.b = GetBValue(in);
//	// const double f = 0.0039215686274509803921568627451; // == 1/255
//	out.r *= O255;
//	out.g *= O255;
//	out.b *= O255;
//	return out;
//}


CColorHelper::hsv CColorHelper::rgb2hsvf(COLORREF in)
{
	CColorHelper::rgb out;
	out.r = GetRValue(in);
	out.g = GetGValue(in);
	out.b = GetBValue(in);
	out.r *= O255;
	out.g *= O255;
	out.b *= O255;

	return rgb2hsvf(out);
}
CColorHelper::hsv CColorHelper::rgb2hsvf(CColorHelper::rgb in)
{
	CColorHelper::hsv         out;
	double      minf, maxf, delta;

	double r = in.r;
	double g = in.g;
	double b = in.b;

	minf = r < g ? r : g;
	minf = minf  < b ? minf : b;

	maxf = r > g ? r : g;
	maxf = maxf  > b ? maxf : b;

	out.v = maxf;                                // v
	delta = maxf - minf;
	if (delta < 0.00001)
	{
		out.s = 0;
		out.h = 0; // undefined, maybe nan?
		return out;
	}
	if (maxf > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
		out.s = (delta / maxf);                  // s
	}
	else {
		// if max is 0, then r = g = b = 0              
		// s = 0, v is undefined
		out.s = 0.0;
		out.h = NAN;                            // its now undefined
		return out;
	}
	if (r >= maxf)                           // > is bogus, just keeps compilor happy
		out.h = (g - b) / delta;        // between yellow & magenta
	else
		if (g >= maxf)
			out.h = 2.0 + (b - r) / delta;  // between cyan & yellow
		else
			out.h = 4.0 + (r - g) / delta;  // between magenta & cyan

	out.h *= 60.0;                              // degrees

	if (out.h < 0.0)
		out.h += 360.0;

	return out;
}


CColorHelper::rgb CColorHelper::hsv2rgbf(CColorHelper::hsv in)
{
	double      hh, p, q, t, ff;
	long        i;
	CColorHelper::rgb         out;
	
	if (in.s <= 0.0) {       // < is bogus, just shuts up warnings
		out.r = in.v;
		out.g = in.v;
		out.b = in.v;
		return out;
	}
	hh = in.h;
	if (hh >= 360.0) hh = 0.0;
	hh *= 0.01666666666666666666666666666667;   // hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));
	 
	switch (i) {
	case 0:
		out.r = in.v;
		out.g = t;
		out.b = p;
		break;
	case 1:
		out.r = q;
		out.g = in.v;
		out.b = p;
		break;
	case 2:
		out.r = p;
		out.g = in.v;
		out.b = t;
		break;

	case 3:
		out.r = p;
		out.g = q;
		out.b = in.v;
		break;
	case 4:
		out.r = t;
		out.g = p;
		out.b = in.v;
		break;
	case 5:
	default:
		out.r = in.v;
		out.g = p;
		out.b = q;
		break;
	}

	if (out.r < 0.) out.r = 0.;
	else if (out.r > 1.) out.r = 1.;

	if (out.g < 0.) out.g = 0.;
	else if (out.g > 1.) out.g = 1.;

	if (out.b < 0.) out.b = 0.;
	else if (out.b > 1.) out.b = 1.;


	return out;
}

COLORREF CColorHelper::interp1D_HSV(COLORREF ca, COLORREF cb, double xa, double xb, double x)
{
	double dx = xb - xa;
	if (fabs(dx) < MINAMP || x <= xa) return ca;
	if (x >= xb) return cb;

	double f = (x - xa) / dx;  // f is always <1 so we do not need to worry about overflowing color value
	return interpolateHSV(ca, cb, (float)f);
}

COLORREF CColorHelper::interp1D_RGB(COLORREF ca, COLORREF cb, double xa, double xb, double x)
{
	double dx = xb - xa;
	if (fabs(dx) < MINAMP || x <= xa) return ca;
	if (x >= xb) return cb;

	double f = (x - xa) / dx;  // f is always <1 so we do not need to worry about overflowing color value
	return interpolateRGB(ca, cb, (float)f);
}

COLORREF CColorHelper::interpolate(COLORREF a, COLORREF b, COLORREF c, COLORREF d, double t, double s)
{
	double fa = (1 - t)*(1 - s);
	double fb = t*(1 - s);
	double fc = (1 - t)*s;
	double fd = t*s;

	return RGB((int)(0.5 + GetRValue(a)*fa + GetRValue(b)*fb + GetRValue(c)*fc + GetRValue(d)*fd),
		(int)(0.5 + GetGValue(a)*fa + GetGValue(b)*fb + GetGValue(c)*fc + GetGValue(d)*fd),
		(int)(0.5 + GetBValue(a)*fa + GetBValue(b)*fb + GetBValue(c)*fc + GetBValue(d)*fd));

}

COLORREF getColorInverseDistMono(int **colDataArr, float *rangeArr, int nx, int ny, float ymin, float dy, float x, float y)
{
	// Given a 2D array of COLORREF format, look for the index of the 4 corners the surrounds the given point (x, y)
	// After that use inverse distance rule to get the COLORREF
	// NOTE: our routine allows X-axis irregularly spaced, while Y-axis is still regularly spaced, so there is no xmin and dx inputs.
	// static COLORREF colWhite = RGB(255, 255, 255);
	if (colDataArr == NULL || rangeArr == NULL || nx < 1 || ny < 1 || dy == 0.f) return WHITE;

	// note: rangeArr[] should be monotonically increasing or decreasing, what about if not? Then we should use the closest index only!
	int ix = -1;
	for (int i = 0; i<nx - 1; i++) {
		if (x> rangeArr[i] && x< rangeArr[i + 1]) {
			ix = i;
			break;
		}
	}

	double ay = (y - ymin) / dy;
	int iy = (int)ay;

	if (ix<0 || ix>nx - 1 || iy<0 || iy>ny - 1) return WHITE;

	int ix2 = ix + 1;
	int iy2 = iy + 1;
	if (ix2>nx - 1) ix2 = nx - 1;
	if (iy2 > ny - 1) iy2 = ny - 1;

	double ax = rangeArr[ix2] - rangeArr[ix];
	double t = fabs(ax) < MINAMP ? 0. : (x - rangeArr[ix]) / ax;
	if (t < 0.0) t = 0.0;
	else if (t > 1.0) t = 1.0;

	double s = ay - iy;
	
	return CColorHelper::interpolate((COLORREF)colDataArr[ix][iy], (COLORREF)colDataArr[ix2][iy],
		(COLORREF)colDataArr[ix][iy2], (COLORREF)colDataArr[ix2][iy2], t, s);

	// note the sequencial order of values is different
	// LinearGradientBrush 

	//COLORREF cornercolors[4];
	//cornercolors[0] = (COLORREF)colDataArr[ix][iy];
	//cornercolors[1] = (COLORREF)colDataArr[ix2][iy];
	//cornercolors[2] = (COLORREF)colDataArr[ix][iy2];
	//cornercolors[3] = (COLORREF)colDataArr[ix2][iy2];
	//return RGB( interpolate(GetRValue(cornercolors[0]), GetRValue(cornercolors[1]), GetRValue(cornercolors[2]), GetRValue(cornercolors[3]), t, s),
	//               interpolate(GetGValue(cornercolors[0]), GetGValue(cornercolors[1]), GetGValue(cornercolors[2]), GetGValue(cornercolors[3]), t, s),
	//               interpolate(GetBValue(cornercolors[0]), GetBValue(cornercolors[1]), GetBValue(cornercolors[2]), GetBValue(cornercolors[3]), t, s) );
}



int CColorHelper::interpolLinear(int a, int b, float t)
{
	return (int)(a * (1 - t) + b * t + 0.5);
}

double CColorHelper::interpolLinear(double a, double b, float t)
{
	return a * (1 - t) + b * t + 0.5;
}
COLORREF CColorHelper::interpolateHSV(COLORREF a, COLORREF b, float t)
{
	// 0.0 <= t <= 1.0
	HsvColor ca = RgbToHsvi(a);
	HsvColor cb = RgbToHsvi(b);
	HsvColor final;

	final.h = interpolLinear(ca.h, cb.h, t);
	final.s = interpolLinear(ca.s, cb.s, t);
	final.v = interpolLinear(ca.v, cb.v, t);
	return HsvToRgbi(final);
}

// this function seems not working perfect
//CColorHelper::rgb CColorHelper::interpolateHSVf(COLORREF a, COLORREF b, float t)
//{
//	// 0.0 <= t <= 1.0
//	//	return interpolateRGB(a, b, t);
//	hsv ca = rgb2hsvf(a);
//	hsv cb = rgb2hsvf(b);
//	hsv out;
//
//	out.h = interpolLinear(ca.h, cb.h, t);
//	out.s = interpolLinear(ca.s, cb.s, t);
//	out.v = interpolLinear(ca.v, cb.v, t);
//
//	return hsv2rgbf(out);
//}
CColorHelper::rgb CColorHelper::interpolateHSVf(CColorHelper::hsv ca, CColorHelper::hsv cb, float t)
{
	// 0.0 <= t <= 1.0
	hsv out;
	out.h = interpolLinear(ca.h, cb.h, t);
	out.s = interpolLinear(ca.s, cb.s, t);
	out.v = interpolLinear(ca.v, cb.v, t);
	return hsv2rgbf(out);
}


//COLORREF CColorHelper::interpolateHSVDec2(COLORREF a, COLORREF b, float t)
//{
//	rgb c = interpolateHSVf(a, b, t);
//	return RGB(
//		(unsigned char)(c.r * 255.),
//		(unsigned char)(c.g * 255.),
//		(unsigned char)(c.b * 255.)
//	);
//}

//COLORREF CColorHelper::interpolateHSV3_Delete(int rA, int gA, int bA,
//	int rB, int gB, int bB,
//	float t)
//{
//	// 0.0 <= t <= 1.0
//	return interpolateHSVi(RGB(rA, gA, bA), RGB(rB, gB, bB), t);
//}

COLORREF CColorHelper::interpolateRGB(COLORREF a, COLORREF b, float t)
{
	int rA = GetRValue(a);
	int gA = GetGValue(a);
	int bA = GetBValue(a);
	int rB = GetRValue(b);
	int gB = GetGValue(b);
	int bB = GetBValue(b);

	// 0.0 <= t <= 1.0
	int rRef = interpolLinear(rA, rB, t);
	int gRef = interpolLinear(gA, gB, t);
	int bRef = interpolLinear(bA, bB, t);

	return RGB(rRef, gRef, bRef);
}

CColorHelper::rgb CColorHelper::interpolateRGBf(COLORREF a, COLORREF b, float t)
{
	int rA = GetRValue(a);
	int gA = GetGValue(a);
	int bA = GetBValue(a);
	int rB = GetRValue(b);
	int gB = GetGValue(b);
	int bB = GetBValue(b);

	CColorHelper::rgb out;
	// 0.0 <= t <= 1.0
	out.r = interpolLinear(rA, rB, t) * O255;
	out.g = interpolLinear(gA, gB, t) * O255;
	out.b = interpolLinear(bA, bB, t) * O255;
	return out;
}