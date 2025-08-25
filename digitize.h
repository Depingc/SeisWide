#pragma once

#define MAXLINE 999     /* The max length of each text line input, for Zelt format only */
#define NP_MAX 200000    // max amount of points per line allowed, to avoid hacking crash

#include "segy.h"
#include "coords.h"
#include "ealloc_d.h"

class CDigitize
{
public:
   enum XMODE {
      trace,
	  distance,
      offset
   };

	// NOTE 2:  we do not want a default DIGITIZE dig struct, since CSeisWideDoc class already has one such default struct created.
	//          we can have such a member variable here, but it would cause confusion
	struct DIGITIZE{
		CString  digName;
	//	CString digNameDelayed;  // if not empty, the saving is delayed since saving to disk is slower
		XMODE xMode;  

		// the memory cache will refresh if the following 3 items are changed
		CTime modifiedtime; 
		CString  digNameCache;
		float xShotCache; // for caching purpose only, for decide if it needs to re-load the points from file
		// normally equals to the file modified time. But if it is 0, the DIGITIZE should be saved, otherwise it should be loaded from file. 

		// if array sizes are now dynamic, we should remove these limits? To be done.
		int digPointMax;
		int digLayerMax;

		// points definition. These arrays will be freeed in freeDigAll()
		int iCurveSel;
		int totCurve;
		int *nPoint;

		int *ivray;  //ray code
		float *curveVal; // can be either Zelt Resolution or value of the curve

		float **x, **t; // x[it][ic], this is reversed by mistake!
		// Note: t[it][ic] will not consider any reducing velocity at all, and 
		// will not consider if it is in time domain or depth domain. The exception
		// is when isZeltFormat==TRUE, which will treat the time to be absolute time.


		// to do: add (1) line thickness (hair, 0.5mm, 1mm); (2) segment type: points, lines, or polygons? 

		// labeling
		CString strComment; // Only 1 comment for each OBS 

		CStringArray *labelArr;  // This is used only in labeling, not for indicators for curve change et al.!
		// Note: starting in 2019, *labelArr is only used in labeling, not for defining
		// curve grouping/starting/ending. But those label texts cannot be output
		// if Seiswide's digitizing format is used. Only GMT format can take them
		// by putting >label text     at the beginning of each curve

		// when initialize a DIGITIZE instance, one just add BLANK to each curve.
		// If not added, labelArr will have fewer elements, and the index of 
		// labels would be mis-placed. Similarly, adding a new curve or
		// deleting curves will have the labelArr affected similarly.


		// zelt's formating
		BOOL isDigitizing; // if true, onscreen digitizing is going on.
		BOOL isZeltFormat;  // 0=text format; 1=Zelt format; 2=nav format
		short zeltBarForm; // control how a error bar should be displayed
			// e.g.  a star, a bar, a circle et al.
			//0=bar; 1=Circle (open); 2=Circle (filled); 3=Star.
		BOOL isTraceFormat; // if TRUE, we must sort on Y-values, in the process of formulating a segy trace. Normally FALSE
		BOOL isXValClicking; // if TRUE, the next click will be for assigning curveVal[iCurveSel]

		// sorting
		BOOL isSortedX;
		BOOL isSortedY;

		// for transform the curves/markers. Must be floating point since they are saved in SEI file
		float transXMultFactor;  // to multiply to the x-axis
		float transXShift;  // shift the x-axis by this km
		float transYMultFactor;  // to multiply to the y-axis
		float transYShift;  // shift the y-axis by this km
		BOOL  transIsRevYAxis;  // check to reverse the Y-axis for line-drawings and markers. This is useful for plotting gravity values

		// addition from SHP format
		int nShapeType;

		inline bool isCurveValExist(float v) {
			for (int ic = 0; ic < this->totCurve; ic++) {
				if (isFloatEqual3(v, curveVal[ic])) {
					return true;
				}
			} // for
			return false;
		}

		const inline float getCurveValNext() {
			// calculate the next curveVal value. It should be the currently maximum value plus 1.
			float out;
			int ic = iCurveSel;
			if (ic >= 0 && ic < this->totCurve) {
				out = this->curveVal[ic];
				if (out >= 0) {
					out += 1.f;

					// now, if this value does not already exist, we just use it
					while (true) {
						if (isCurveValExist(out))
							out += 0.1f;
						else 
							return out;
					} // while


				} // if out
			} // if ic

			// we need to figure out the maximum curve value first.
			out = -MIL;
			for (ic = 0; ic < totCurve; ic++) {
				if (out < curveVal[ic])
					out = curveVal[ic];
			}
			if (out < 0) 
				out = (float)totCurve - 1.f;
			// we want the first value to be zero, so that it is the same as index
			return out + 1.f;
		}

		const inline int getIvrayNext() {
			// calculate the next ivray value. It should be the currently maximum value plus 1.
			int out = -(int)MIL;
			for (int ic = 0; ic < totCurve; ic++) {
				if (out < ivray[ic])
					out = ivray[ic];
			}
			return out + 1;
		}
		const inline float getXmin(int ic) {
			if (totCurve <= 0 || ic<0 || ic>totCurve - 1) return ERRORVAL;
			float out = 99999.f;
			for (int j = 0; j < nPoint[ic]; j++) {
				if (out > x[j][ic])
					out = x[j][ic];
			}
			return out;
		}
		const inline float getXmax(int ic) {
			if (totCurve <= 0 || ic<0 || ic>totCurve - 1) return -ERRORVAL;
			float out = ERRORVAL;
			for (int j = 0; j < nPoint[ic]; j++) {
				if (out < x[j][ic])
					out = x[j][ic];
			}
			return out;
		}

		const inline float getYmax() {
			if (totCurve <= 0) return 99999.f;
			float out = ERRORVAL;
			for (int ic = 0; ic < totCurve; ic++) {
				for (int j = 0; j < nPoint[ic]; j++) {
					if (out < t[j][ic])
						out = t[j][ic];
				}
			}
			return out;
		}

		const inline float getMedianInterval(int ic) {
			// Note: we cannot overwrite the original array dx[]
			int n = nPoint[ic];
			if (n < 2) return 0.f;
			float *x1 = (float *)ealloc1float(n);
			if (!x1) return 0.0f;  // it fails to allocate memory

			int j;
			for (j = 0; j < n-1; j++) {
				x1[j] = x[j+1][ic] - x[j][ic];
			}
			//double a = getMedian2(&x1[0], n);
			j = getMedian3(&x1[0], n-1);
			float a = x1[j];
			free1float(x1);
			return a;
		}
		const inline bool isDigLonLatPossible() {
			// make a guess
			for (int i = 0; i < totCurve; i++)
				for (int j = 0; j < nPoint[i]; j++)
					if (x[j][i]<-180.f || x[j][i]>180.f || t[j][i]<-180.f || t[j][i]>180.f)
						return false;
			return true;
		}

		const inline int getMaxPointsPerCurve() {
			return digPointMax;
			//int out = 0;
			//for (int i = 0; i < totCurve; i++)
			//	out = max(out, nPoint[i]);
			//return out;
		}

		inline void freeDigDataOnly() {
				if (this->nPoint != NULL) free1int(this->nPoint);
				if (this->ivray != NULL) free1int(this->ivray);
				if (this->curveVal != NULL) free1float(this->curveVal);
				if (this->x != NULL) free2float(this->x);
				if (this->t != NULL) free2float(this->t);

				this->nPoint = NULL;
				this->ivray = NULL;
				this->curveVal = NULL;
				this->x = NULL;
				this->t = NULL;

				if (this->labelArr != NULL) {
					this->labelArr->RemoveAll();
					delete this->labelArr;
					this->labelArr = NULL;
				}

				this->totCurve = 0;
		}

		// NOTE: the calling function must manually set dig2 = NULL, even though this function does it
		// because they are different numbers
		// no need to free these arrays manually, as all done in the destructor now!!!
		inline void freeDigAll()
		{
			// free all arrays for digitizing, used many times.
			this->freeDigDataOnly();
			this->nPoint = NULL;
			this->ivray = NULL;
			this->curveVal = NULL;
			this->x = NULL;
			this->t = NULL;
			this->labelArr = NULL;
		}

		const inline float getDigYMin() {
			float amin = MIL;
			for (int i = 0; i<this->totCurve; i++) {
				for (int j = 0; j<this->nPoint[i]; j++) {
					if (amin > this->t[j][i]) {
						amin = this->t[j][i];
					} // if
				} // j
			}
			return amin;
		}

		const inline float getDigYMax() {
			// this function does not call any global variables, can be moved to util.cpp
			float amax = -MIL;
			for (int i = 0; i<this->totCurve; i++) {
				for (int j = 0; j<this->nPoint[i]; j++) {
					if (amax < this->t[j][i]) {
						amax = this->t[j][i];
					} // if
				} // j
			}
			return amax;
		}
		inline void myDigAddCurve(double xShot) {
				int n = this->totCurve;
				if (n >= this->digLayerMax) {
					myDigArraySizeInc(this, 100, 10);
				}
				if (labelArr)
					labelArr->Add(BLANK);
				this->totCurve++;
				this->iCurveSel = this->totCurve - 1; // set the last curve to be current
		}
		const inline CString getDigExtension() {
			if (this->isZeltFormat) return _T("in");
			else return _T("txt");
		}
		const inline bool isOnePointPerCurve() {
			for (int i = 0; i < this->totCurve; i++) {
				if (this->nPoint[i] > 1)
					return false;
			}
			return true;
		}


		inline void copyDig(CDigitize::DIGITIZE *digTo) {
			if (!digTo->initDigitizeArray(this->digPointMax, this->totCurve)) return;

			int i, j;
			for (i = 0; i < this->totCurve; i++) {
				digTo->ivray[i] = this->ivray[i];
				digTo->nPoint[i] = this->nPoint[i];
				digTo->labelArr->Add(this->getLabelTextOnly(i));
				digTo->digName = this->digName;
				digTo->isZeltFormat = this->isZeltFormat;
				digTo->modifiedtime = this->modifiedtime;
				digTo->nShapeType = this->nShapeType;
				digTo->strComment = this->strComment;
				digTo->xMode = this->xMode;
				digTo->zeltBarForm = this->zeltBarForm;
				digTo->curveVal[i] = this->curveVal[i];
				digTo->transIsRevYAxis = this->transIsRevYAxis;
				digTo->transXMultFactor = this->transXMultFactor;
				digTo->transXShift = this->transXShift;
				digTo->transYShift = this->transYShift;
				for (j = 0; j < this->nPoint[i]; j++) {
					digTo->x[j][i] = this->x[j][i];
					digTo->t[j][i] = this->t[j][i];
				}
			}
			digTo->totCurve = this->totCurve;
			digTo->isDigitizing = this->isDigitizing;
		}


		inline BOOL initDigitizeArray(int npoint, int ncurve)
		{
			this->freeDigAll(); // this is important for avoiding memory leaks.

			ncurve += 30;
			npoint += 100;
			this->digLayerMax = max(ncurve, this->digLayerMax);
			this->digPointMax = max(npoint, this->digPointMax);

			this->nPoint = (int *)ealloc1int(this->digPointMax);
			this->ivray = (int *)ealloc1int(this->digLayerMax);
			this->curveVal = (float *)ealloc1float(this->digLayerMax);
			this->x = (float **)ealloc2float(this->digLayerMax, this->digPointMax);
			this->t = (float **)ealloc2float(this->digLayerMax, this->digPointMax);
			this->labelArr = new CStringArray();
			this->labelArr->RemoveAll();

			this->totCurve = 0;

			if (this->nPoint == NULL) return FALSE;

			// initilize some array values
			for (int j = 0; j<ncurve; j++) {
				this->ivray[j] = -1;
				this->nPoint[j] = 0;
			}


			for (int i = 0; i<ncurve; i++) {
				this->labelArr->Add(BLANK);
				this->curveVal[i] = ERRORVAL;
			}

			if (this->iCurveSel<0 || this->iCurveSel>99999) {
				// if the user is currently working on a curve, we do not want to change 
				// her focus point
				this->iCurveSel = -1;	//first curve.
			}

			this->modifiedtime = zeroTime;
			return TRUE;
		}

		inline CString getLabelString(int i) {
			CString tStr;
			tStr.Format(_T("%g"), this->curveVal[i]);
			return tStr;
		}

		// this will only grab the text for this curve i, it can be blank, or anything else
		inline CString getLabelTextOnly(int i) {
			if (i > this->labelArr->GetCount() - 1) return BLANK;
			return (CString)this->labelArr->ElementAt(i);
		}

		inline double getReduceTimeWithDigitize(double offset, int drawmode, CCoords::REDUCE *red) {
			double out = 0.;
			if (this->totCurve != 1) {
				return CCoords::getReduceTimeEx2(offset, drawmode, red);
			}

			//for (int i = 0; i < this->totCurve; i++) {
			//	for (int j = 0; j < this->nPoint[i]; j++) {
			//		if (amax < this->t[j][i]) {
			//			amax = this->t[j][i];
			//		} // if
			//	} // j
			//}

			return out;
		}

		DIGITIZE() : isZeltFormat(FALSE), totCurve(0), zeltBarForm(0),
			isTraceFormat(FALSE), isXValClicking(FALSE),
			digPointMax(9999), digLayerMax(100),
			isSortedX(FALSE), isSortedY(FALSE), nShapeType(1),
			xShotCache(-99999.f),
			nPoint(NULL), curveVal(NULL), ivray(NULL), x(NULL), t(NULL), 
			labelArr(NULL),
			transXMultFactor(1.0), 
			transXShift(0),
			transYMultFactor(1.0), 
			transYShift(0), 
			transIsRevYAxis(FALSE)
		{
			isDigitizing = FALSE;
		}; // totCurve>0 is an indicator that it must be free-ed later.

		~DIGITIZE() {
			if (labelArr && labelArr->GetCount()>0) {
				labelArr->RemoveAll();
				delete labelArr;
			}

			// no need to set these arrays to NULL since here is the destructor
			if (x != NULL) free2float(x);
			if (t != NULL) free2float(t);
			if (curveVal != NULL) free1float(curveVal);
			if (nPoint != NULL) free1int(nPoint);
			if (ivray != NULL) free1int(ivray);
		}
	};


	CDigitize(void);
	~CDigitize(void);
	static void myClipReducePointsTrans(CDigitize::DIGITIZE *digIn, 
		CDigitize::DIGITIZE *digOut,
		SEI_QUAD seiq, CCoords::REDUCE *pred,
		double xShot, BOOL isDistanceDisplay, BOOL isMinusXShot,
		int drawmode, bool isReducedOutput);
	static void myClipReducePoints4(CDigitize::DIGITIZE *digIn, 
		CDigitize::DIGITIZE *digOut, 
		CCoords::SEI_COORD *pCoord, CCoords::REDUCE *pred, float xShotRay, bool isReducedOutput);
	static bool myDigArraySizeInc(DIGITIZE *dig2, int np_inc, int nc_inc);
	static int getCountNonMonoX(CDigitize::DIGITIZE *dig2, int ic);
	static int getDigIndexExact(CDigitize::DIGITIZE *dig, int ic, float x, float xres);
	static float getDigXfromTLinear(CDigitize::DIGITIZE *dig2, float t);
	static float getDigTfromXLinear(CDigitize::DIGITIZE *dig2, float x);
	static float getDigTfromXSpline(CDigitize::DIGITIZE *dig2, float x, int ic = 0);
	static float getDigXfromTSpline(CDigitize::DIGITIZE *dig2, float y, int ic = 0);

	void myDigAddPoint(CDigitize::DIGITIZE *dig2, int &ic, double x, double t, double xShot=0.0); // note: this changes draw.dig.nPoint[ic]
	BOOL myDigRemove(CDigitize::DIGITIZE *dig2, int ip, int ic); // note: this changes draw.dig.nPoint[ic]
	BOOL digDeleteCurve(DIGITIZE *dig2, int ic);
	BOOL digSplitCurve(DIGITIZE *dig2, int ic, double xSplit); // note: this changes draw.dig.totCurve
	void myDigRemoveInside3(CDigitize::DIGITIZE *dig2, int ic, float xa, float xb);
	void myDigShift(CDigitize::DIGITIZE *dig2, float dx, float dt, float xLimit1, float xLimit2, int iCurveSel);
	BOOL myDigSaveAs2(CDigitize::DIGITIZE *dig2, bool isXInt);
	BOOL myDigSaveAsZelt(CDigitize::DIGITIZE *dig2, double xs, BOOL isMinusXShot);
	BOOL myDigSaveAsZeltFFID(CDigitize::DIGITIZE *dig2, CString outFile, double xs, BOOL isMinusXShot, int countTrace, int *ptrace, float *pdist, int *pffid, float *poffset);
	CString myDigSaveWithFFID(CDigitize::DIGITIZE *dig2, int countTrace, int *ptrace, float *pdist, int *pffid, float *poffset);

	void myDigRemoveDupX(CDigitize::DIGITIZE *dig2, int ic); // note: this changes draw.dig.nPoint[ic]
	void myDigRemoveDupY(CDigitize::DIGITIZE *dig2, int ic); // note: this changes draw.dig.nPoint[ic]
	void myDigSortX2(CDigitize::DIGITIZE *dig2, double xmin=-99999., double xmax=99999.);
	void myDigSortY2(CDigitize::DIGITIZE *dig2, double ymin = -99999., double ymax = 99999.);
	void myDigSort(CDigitize::DIGITIZE *dig2);

	//BOOL myDigitizingLoadTXTUnix2(CDigitize::DIGITIZE * dig2, CStringArray *xzArray, BOOL isSortX = FALSE, BOOL isSortY = FALSE, BOOL isY3 = FALSE);
	//	BOOL myDigitizingLoadTXTUnix2(CDigitize::DIGITIZE * dig2, BOOL isSort = FALSE, BOOL isSortY = FALSE, BOOL isY3 = FALSE);
	//BOOL myDigitizingLoadTXTUnix(CDigitize::DIGITIZE * dig2);
	BOOL myDigitizingLoadTXT4Col(CDigitize::DIGITIZE * dig2, CStringArray *xzArray);
	BOOL myDigitizingLoadTXTUnix2(CDigitize::DIGITIZE * dig2, CStringArray *xzArray, BOOL isY3 = FALSE);
	BOOL myDigitizingLoadTXTUnix2(CDigitize::DIGITIZE * dig2, BOOL isY3 = FALSE);
	BOOL myDigitizingLoadTXT_GMT(CDigitize::DIGITIZE * dig2, CStringArray *xzArrayFrom);
	void myDigLoadZelt(CDigitize::DIGITIZE *dig2, double xs, bool isXsForced=true); 
	bool getDigLimits(CDigitize::DIGITIZE *dig2, double &xmin, double &xmax, double &tmin, double &tmax);
	int getClosestDigNum(CDigitize::DIGITIZE *digm, SEI_PAIR pNew);

	BOOL SHPOpenToDig(CString shapeFile, CDigitize::DIGITIZE *dig2, SEI_PAIR &pmin, SEI_PAIR &pmax);

	static bool digitize2Kml(CStringArray *arIn, CString fOut);
	static bool digitize2Kml(CDigitize::DIGITIZE *dig, CString fOut, bool isLine);

	static bool getKmlDimension(CString fKml, int& ncurve, int& npoints);




private:
	void privateGetDigSize(CDigitize::DIGITIZE * dig2, CStringArray *xzArray, BOOL isY3,
		int &numItems, int &pcurve, int &ppoint);
	void privateGetDigSize_GMT(CStringArray *xzArray, 
		int &pcurve, int &ppoint);

	/************************************************************************/
	/*                             SHP Support.                             */
	/************************************************************************/

	typedef unsigned char uchar;

	#if UINT_MAX == 65535
		typedef long	      int32;
	#else
		typedef int	      int32;
	#endif

	static void	SwapWord(int length, void * wordP);

	typedef	struct
	{
		FILE        *fpSHP;
		FILE	*fpSHX;

		int		nShapeType;				/* SHPT_* */

		int		nFileSize;				/* SHP file */

		int         nRecords;
		int		nMaxRecords;
		int		*panRecOffset;
		int		*panRecSize;

		double	adBoundsMin[4];
		double	adBoundsMax[4];

		int		bUpdated;

		unsigned char *pabyRec;
		int         nBufSize;
	} SHPInfo;

	typedef SHPInfo * SHPHandle;

	/* -------------------------------------------------------------------- */
	/*      Shape types (nSHPType)                                          */
	/* -------------------------------------------------------------------- */
#define SHPT_NULL	0
#define SHPT_POINT	1
#define SHPT_ARC	3
#define SHPT_POLYGON	5
#define SHPT_MULTIPOINT	8
#define SHPT_POINTZ	11
#define SHPT_ARCZ	13
#define SHPT_POLYGONZ	15
#define SHPT_MULTIPOINTZ 18
#define SHPT_POINTM	21
#define SHPT_ARCM	23
#define SHPT_POLYGONM	25
#define SHPT_MULTIPOINTM 28
#define SHPT_MULTIPATCH 31


	/* -------------------------------------------------------------------- */
	/*      Part types - everything but SHPT_MULTIPATCH just uses           */
	/*      SHPP_RING.                                                      */
	/* -------------------------------------------------------------------- */

#define SHPP_TRISTRIP	0
#define SHPP_TRIFAN	1
#define SHPP_OUTERRING	2
#define SHPP_INNERRING	3
#define SHPP_FIRSTRING	4
#define SHPP_RING	5

	/* -------------------------------------------------------------------- */
	/*      SHPObject - represents on shape (without attributes) read       */
	/*      from the .shp file.                                             */
	/* -------------------------------------------------------------------- */
	typedef struct
	{
		int		nSHPType;

		int		nShapeId; /* -1 is unknown/unassigned */

		int		nParts;
		int		*panPartStart;
		int		*panPartType;

		int		nVertices;
		double	*padfX;
		double	*padfY;
		double	*padfZ;
		double	*padfM;

		double	dfXMin;
		double	dfYMin;
		double	dfZMin;
		double	dfMMin;

		double	dfXMax;
		double	dfYMax;
		double	dfZMax;
		double	dfMMax;

	//	int		bMeasureIsUsed;
	} SHPObject;
	const char * SHPTypeName(int nSHPType);
	const char * SHPPartTypeName(int nPartType);
	void SHPDestroyObject(SHPObject * psShape);

	static void CDigitize::SHPWriteHeader(SHPHandle psSHP);
	SHPHandle SHPOpen(const char * pszShapeFile, const char * pszAccess);
	void SHPClose(SHPHandle hSHP);
	void SHPGetInfo(SHPHandle psSHP, int * pnEntities, int * pnShapeType,
		double * padfMinBound, double * padfMaxBound);
	static void * SfRealloc(void * pMem, int nNewSize);
	SHPObject * SHPReadObject(SHPHandle hSHP, int iShape);

};
