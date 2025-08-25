#pragma once
#include "segy.h"
#include "util.h"
//#include "base64.h"
#include "colorHelper.h"
#include "coords.h"

class CDCHelper
{
public:
	//int x, y;
	CDCHelper();
	~CDCHelper();

	static BOOL floodFillArr9(CDC* pDC, int nx, int nt, float **ampArr, float *rangeArr,
		double xmin, double xmax, double tmin, double tmax,
		CColorHelper::AMPCOLORDYN *ampColorDyn2, CCoords *pcd);

	static BOOL tessellateFill2Rect(CDC*, int, int, float **, float *, double, double, double, double,
		CColorHelper::AMPCOLORDYN *, CCoords *, int);
	static BOOL tessellateFill2(CDC*, int, int, float **, float *, double, double, double, double,
		CColorHelper::AMPCOLORDYN *, CCoords *, int);

	static BOOL wiggleArr(CDC* pDC, int nx, int nt, float **xArr, float *rangeArr, float *yArr, CCoords *pcd);
	static void airvarSmall(CDC* pDC, int n, double x[], double y[], double range, CCoords *pcd);

	static void airCol(CDC* pDC, int n, double x[], double y[], double range, double fillOff, 
		CCoords *pcd, CColorHelper::SEISBICOLOR *seisBiColor2);
	
	static void airColorSmallArr(CDC* pDC, int nx, int nt, float **xArr, float rangeArr[], 
		double ymin, double dt, double offFill,
		CCoords *pcd, CColorHelper::SEISBICOLOR *seisBiColor2);

	static void airColorSmallArr(CDC* pDC, int nx, int nt, float **xArr, float rangeArr[],
		double ft, double dt, COLORREF colPos, COLORREF colNeg, double offFill,
		CCoords *pcd);

	static void airColorSmallArr(CDC* pDC, int nx, int nt, float **xArr, float rangeArr[],
		double ft, double dt, COLORREF colPos, COLORREF colNeg, double offFill,
		CCoords *pcd, bool isFFIDAbNormal[]);

	static void airColorBigArr(CDC* pDC, int nx, int nt, float **xArr, float rangeArr[], 
		double ymin, double dt, double offFill,
		CCoords *pcd, CColorHelper::SEISBICOLOR *seisBiColor2);

	static void airColorBigArr(CDC* pDC, int nx, int nt, float **xArr, float rangeArr[], 
		double ft, double dt, COLORREF colPos, COLORREF colNeg, double offFill,
		CCoords *pcd);

	static void airColorBigArr(CDC* pDC, int nx, int nt, float **xArr, float rangeArr[],
		double ft, double dt, COLORREF colPos, COLORREF colNeg, double offFill,
		CCoords *pcd, bool isFFIDAbNormal[]);

	static void transformCDC(CDC *fromDC, CRect *fromRect, double fromYmin, CCoords::REDUCE fromRed,
		CDC *toDC, CCoords::REDUCE toRed,
		CCoords::SEI_COORD *pCoord, double xAtSource);

	static BOOL myCreateTempDC(CDC *pDCfrom, CDC *pDCto, CRect rectPixel, CRect rectLogical);


	// not used
	static void airCol1Small(CDC* pDC, int n, double x[], double y[], double range, 
		double fillOff2, double trcInterval,
		CCoords *pcd, CColorHelper::SEISBICOLOR *seisBiColor2);





private:

};



