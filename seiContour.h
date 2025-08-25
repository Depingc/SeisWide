#pragma once
#include "coords.h"
#include "colorHelper.h"
//#include "segy.h"
//#include "util.h"

class CSeiContour
{
	int pathcount;
	CPoint *pltContour;
	bool m_isContourColorFill;

public:

	CSeiContour(CCoords::SEI_COORD *pCoordThis);
	~CSeiContour();

	CCoords::SEI_COORD *pCoord;
	BOOL contourFillArr(CDC* pDC, int nx, int nt, float **xArr, float *rangeArr,
		double xmin, double xmax, double tmin, double tmax,
		CColorHelper::AMPCOLORDYN *ampColorDyn2, double amax);

	void conpolyline_polygon(CDC* pDC);
	void coninit(CDC* pDC);
	void conmove(CDC* pDC, float x, float y);
	void condraw(CDC* pDC, float x, float y);
	void condone(CDC* pDC);
	int connect(CDC* pDC, float c, int nx, float *x, int ny, float *y, float *z,
		int *pcell, float *pxd, float *pyd);
	void labelc(float x, float y, float xw, float yw, char *str,
		float size, char *font, char *color);
	void wcell(int nx, float *x, int ny, float *y, float *w,
		int cell, float xc, float yc, float xw, float yw);
	void psContour(CDC* pDC, float c, int nx, float x[], int ny, float y[], float z[], bool isContourColorFill = true);
};