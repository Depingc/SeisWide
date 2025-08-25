#pragma once
#include "segy.h"
#include "util.h"
#include "VelMod.h"

class CRayHelper
{
public:
	//int x, y;
	CRayHelper();
	~CRayHelper();


	struct TRAVELTIME{
		int tot[2];	//tot: no. of curves.
		int Num[2][TX_CURVES_MAX]; //num: number of points for each curve for each side of the OBS. 
		float xShotRay;   // Profile source x-pos; Note: this cannot be changed to double since it is in CArchive already!!!
		// this is shared with pseudo well X-distance!!! NOT good, please separate them???
		// float gunDepth;  // raytracing can only to the airgun depth. Usually 0.011f
		//float xShotZelt;   // source x-pos for Zelt format only; It can be different from xShotRay which is for storing computed X-T curves
		float X[2][TX_POINTS_MAX][TX_CURVES_MAX], T[2][TX_POINTS_MAX][TX_CURVES_MAX];

		int iShot;  // this is the active shot number to consider. if -1, we plot all rays
		/*
		// note: I was trapped by a problem: Zelt's ray paramater has ishot=0,-1,1,2,
		(1) 0 - no rays are traced
		(2) -1 - rays are traced to the left only
		(3) 1 - rays are traced to the right only
		(4) 2 - rays are traced to the left and right
		*/
		int nShot;  // total num of active shots to consider, for displaying only
		// float zShot;
		float xzShot[2][TX_SHOTS_MAX]; // this sometimes store all the shot coordinates
		int ivray[TX_SHOTS_MAX];
		int rayCodeShow;  // -1=Show all rays, otherwise 0-20 (show only if a ray code equals rayCodeShow). For ray codes>20, will first deduct  (int)(irrCol/20) * 20
		float rayCodeIn[TX_SHOTS_MAX]; // mostly not used
		int RayCodePerCurve[2][TX_CURVES_MAX]; // ray codes read in together with each ray in computed file ray path
		//note that, unfortunately, tx.x[] is always model distance in raytracing, but 
		// can be offset in digitizing if cd.coord.isDistanceDisplay==0.

		CStringArray *rayCodes;		// ray codes read in from the parameter file only	; this will be removed soon	
		int colour[TX_SHOTS_MAX];		// colour codes read in from the parameter file only	
		int ncolour;
		int circleCurve; // the curve number that is to display as curve; shortcut: Control+L
		CTime txTimeLoaded;
		TRAVELTIME() : nShot(0), rayCodeShow(-1) {
			xShotRay = ERRORVAL;
			//xShotZelt = ERRORVAL;
			iShot = -1;  // this indicates undefined
			txTimeLoaded = 0;  // must be set to 0; if it the same as file modified time, it will not reload.
			for (int i = 0; i < TX_SHOTS_MAX; i++) {
				xzShot[1][i] = -1.f;
				ivray[i] = 0;
				rayCodeIn[i] = 0.f;
			}
			rayCodes = new CStringArray();
		};

		inline const bool isCurveReflection(int iCurve, int k) {
			double offsetMin = MIL;
			double offsetMax = 0.;
			if (k < 0) k = 0;
			if (k > 1) k = 1;
			if (iCurve<0 || iCurve>TX_CURVES_MAX - 1) return false;
			for (int j = 0; j < Num[k][iCurve]; j++) {
				register double xval = fabs(X[k][j][iCurve] - xShotRay);
				offsetMin = min(offsetMin, xval);
				offsetMax = max(offsetMax, xval);
			}
			return offsetMax > 0.001 ? offsetMin / offsetMax < 0.1 : false;
		}

		~TRAVELTIME() {
			rayCodes->RemoveAll();
			delete rayCodes;
			rayCodes = NULL;
		}
	};


	struct RAYPATHS{
		CString  rayFileName;
		CTime rayFileModifiedTime;
		int m_totCurve; // this variable is readonly to avoid problems.
			// the reason: at init stage, I set the array sizes according to m_totCurve. In destructor, I need to free the arrays using this number.
		// if it is modified, it causes memory leaks.
		//int npts_max_toDelete;
		int iShot; // for use displaying a certain shot only. If -1, it will display all shots
		int rayCodeShow;  // -1=Show all rays, otherwise 0-20 (show only if a ray code equals rayCodeShow). For ray codes>20, will first deduct  (int)(irrCol/20) * 20
		int npMax;
		int ncMax;
		int *npts;
		int *irrcol;
		float **xr, **zr;

		inline const int getTotCurve() {
			return m_totCurve;
		}
		inline void setTotCurve(int n) {
			m_totCurve = n;
		}

		inline const int getNptsMax() {
			return this->npMax;

			//int out = 0;
			//for (int count = 0; count < m_totCurve; count++) {
			//	out = max(out, npts[count]);
			//}
			//return out;
		}

		inline void freeRaypathsAll() {
			if (npts != NULL) free1int(npts);
			if (irrcol != NULL) free1int(irrcol);
			if (xr != NULL) free2float(xr);
			if (zr != NULL) free2float(zr);
			npts = NULL;
			irrcol = NULL;
			xr = zr = NULL;
			setTotCurve(0);
		}

		inline BOOL initRaypathsArray(int npMax2, int ncMax2) {
			// Note: here I need to dynamically allocate TX_CURVES_MAX and TX_POINTS_MAX
			// since both can change wildly.
			if (npMax2 < 1 || ncMax2 < 1) return FALSE; // nothing to assign!
			if (npMax2 > MIL || ncMax2 > MIL) return FALSE; // prevent overly memory alloc
			freeRaypathsAll();
			this->npMax = npMax2;
			this->ncMax = ncMax2;
			if (npts == NULL) {
				npts = (int *)alloc1int(ncMax2);
				irrcol = (int *)alloc1int(ncMax2);
				xr = (float **)alloc2float(npMax2, ncMax2);
				zr = (float **)alloc2float(npMax2, ncMax2);
			}
			if (!npts) return FALSE;
			for (int n = 0; n < ncMax2; n++) {
				npts[n] = 0;
				irrcol[n] = 0;
			}
			return TRUE;
		}


		// double **xrClipCached, **zrClipCached;  
		// for speeding up display, each curve must be clipped, put to cache, 
		// and then displayed. Size of cache is always double the original size!
		RAYPATHS() : m_totCurve(0), 
			iShot(-1), rayCodeShow(-1),  // added June 20, 2020
			npMax(300), ncMax(100),
			npts(NULL),
			irrcol(NULL),
			xr(NULL), zr(NULL), rayFileModifiedTime(0) {
			m_totCurve = 0;
		}; // totCurve>0 is an indicator that it must be free-ed later.
	};
	//static BOOL initRaypathsArray(CString fFrom, CRayHelper::RAYPATHS *raypaths2);
	static BOOL loadRayPathFromFile(CRayHelper::RAYPATHS *raypaths2);


	static BOOL saveRayPath(CRayHelper::RAYPATHS *raypaths);

	// used
	static BOOL loadRayPathFromDepth2TWTT(
		CRayHelper::RAYPATHS *raypaths, 
		CRayHelper::RAYPATHS *raypathsTwt,
		float dxVDesired, float dz, CVelMod::VELMOD *velm, BOOL isShowDetails);

	// this detail function is not used, since it is too slow
	static BOOL loadRayPathFromDepth2TWTT_detail(
		CRayHelper::RAYPATHS *raypaths,
		CRayHelper::RAYPATHS *raypaths3, int npoint, int ncurve, 
		float dxVDesired, float dz, CVelMod::VELMOD *velm);


	//static int isPointsLeftofShotOld(CRayHelper::TRAVELTIME *tx2); //returns; -1=left; 0=plus; 1=right;
	static BOOL isMinusXShot(CRayHelper::TRAVELTIME *tx2);
	// note: an offset can be calculated by: xoffset=xshot-x; xshot+x; x-xshot for -1, 0, 1 cases
	static int loadRayPara(CString rayName, CRayHelper::TRAVELTIME *tx);
	static void setXShotAll(double xShotIn, CString rayName, CRayHelper::TRAVELTIME *tx, int iShot=0);
	static BOOL myLoadTravelTime(CString txName, CRayHelper::TRAVELTIME *tx, float);
	static double getXPosAll_deprec(CString, CString);

	bool setVelocityXmLimits(CVelMod::VELMOD * velm, CRayHelper::RAYPATHS *raypaths, int, int, bool);
	bool setVelocityXmLimits(CVelMod::VELMOD * velm, CRayHelper::RAYPATHS *raypaths, bool);




};



