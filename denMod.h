#pragma once
#include "util.h"

struct DENMOD {
	CString denName;
	CTime modifiedtime;
	CString aStr;
	//CTime modifiedLoaded; // normally equals to the file modified time. But if it is 0, the DENMOD should be saved, otherwise it should be loaded from file. 
	int ncont;  // if -1, it is an invalid model and need initializing. If 0, already initialized with proper arrays
	const static int MAX_NCONT = 299;
	const static int MAX_POINTS = 299;
	int ilayer[MAX_NCONT];
	int nzed[MAX_NCONT];
	float dm[MAX_NCONT], xm[MAX_NCONT][MAX_POINTS], zm[MAX_NCONT][MAX_POINTS], x1;

	inline void clipPolygons(float xmin, float xmax, float zmin, float zmax) {
		// for proper plotting, we need to clip all outside points
		for (int j = 0; j < this->ncont; j++) {
			this->nzed[j];
			this->xm[j];
			
		} // for j
	}
	inline BOOL loadDen(CString fname)
	{
		if (this->modifiedtime == getFileModifiedTime(this->denName))
			return TRUE;
		
		CStringArray* xzArray = new CStringArray();
		getLinesFromFile(xzArray, fname, false, 0);
		if (!xzArray) return FALSE;
		if (xzArray->GetSize() < 1) {
			delete xzArray;
			xzArray = NULL;
			return FALSE;
		}

		int n;
		float x[5], z[5], den1;

		int aj = 0; // tempo
		int jj = 0;  // for index of this->xm[jj][i]
		for (int jArr = 0; jArr < xzArray->GetSize(); jArr++) {
			aStr = xzArray->ElementAt(jArr).Trim();
			if (aStr.GetLength() < 2) continue;
			if (_stscanf_s(aStr, _T("%d%d%f%f"), &aj,&n,&x1,&den1) == 0) continue;
			if (jj > MAX_NCONT) break;
			if (jj == 42) {
				int dsfsd = 1;
			}
			this->ilayer[jj] = aj;
			this->nzed[jj] = n;
			this->dm[jj] = den1;
			int nlines = (int)(n / 5.0 + 0.95);
			for (int i = 0; i < nlines; i++) {
				jArr++;
				aStr = xzArray->ElementAt(jArr).Trim();
				if (_stscanf_s(aStr, _T("%f%f%f%f%f%f%f%f%f%f"), &x[0], &z[0], &x[1], &z[1], &x[2], &z[2], &x[3], &z[3], &x[4], &z[4]) == 0) continue;
				int k0 = i * 5;
				for (int k = k0; k < k0+5; k++) {
					if (k >= MAX_POINTS) break;
					if (k >= this->nzed[jj]) break;
					this->xm[jj][k] = x[k - k0];
					this->zm[jj][k] = z[k - k0];
				}
			}
			jj++;
		} // for jArr
		this->ncont = jj;
		xzArray->RemoveAll();
		delete xzArray;
		xzArray = NULL;

		TCHAR curDir[_MAX_PATH];
		GetCurrentDirectory(_MAX_PATH, curDir);
		this->denName = CString(curDir) + _T("\\") + GetBaseFileName(fname) + _T(".mod");
		this->modifiedtime = getFileModifiedTime(this->denName);
		return TRUE;
	}

	inline BOOL saveDen(CString fname)
	{
		CStringArray* arr = new CStringArray();
		if (!arr) return FALSE;
		CString aStr, bStr;
		int n;

		for (int j = 0; j < this->ncont; j++) {
			aStr.Format(_T("%2d%3d %6.4f %6.4f"), this->ilayer[j], this->nzed[j], this->dm[j], this->dm[j]);
			arr->Add(aStr);
			n = this->nzed[j];
			int nlines = (int)(n / 5.0 + 0.95);
			for (int i = 0; i < nlines; i++) {
				aStr = "";
				int k0 = i * 5;
				int count = 0;
				for (int k = k0; k < n; k++) {
					bStr.Format(_T("%9.3f%9.3f"), this->xm[j][k0 + count], this->zm[j][k0 + count]);
					aStr += bStr;
					count++;
					if (count >= 5) break; // only 5 pairs per line
				}
				arr->Add(aStr);
			}
		}
		saveStringArrayToFile(arr, fname);
		arr->RemoveAll();
		delete arr;
		arr = NULL;

		this->denName = fname; // fname may be different from the current denName
		return TRUE;
	}

	DENMOD() : ncont(-1) {
	};

	~DENMOD() {
	}
};
