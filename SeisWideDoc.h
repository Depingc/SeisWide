

// SeisWideDoc.h : interface of the CSeisWideDoc class
//
/////////////////////////////////////////////////////////////////////////////
#include "segy.h"
#include "DIB.h"
#include "VelMod.h"
#include "digitize.h"
#include "coords.h"
#include  "ximage.h"
#include "rayHelper.h"
#include "colorHelper.h"
#include "OpenGLRendererFBO.h"
#include "denMod.h"

class CSeisWideSrvrItem;

class CSeisWideDoc : public COleServerDoc
{
	
public:


protected: // create from serialization only
	BOOL setGainCorr(segy *tr,int dformat, int tpp);
	BOOL setGainCorr(segy *tr, int dformat, int tpp, float sphericOrder, float attenuationCorr, float attenuationSeafloor);
	BOOL drawTrcInfo(CDC* pDC, float xkm, segy *tr, int *sPos, int *sLen, BOOL isDetail = FALSE);
	float GetContourDepth(float *zContour, int nx, double x0, double xmin, double dx);
	BOOL rayIntersect(double a, double b, int nx, double xmin, double dx, double *x4, double *z4, double *aBar, float *zCont);
	void colBarPos (double v, CPoint p, CPoint p1);

	void pltFloodFill(CDC* pDC, CPoint *pPoly, int *lpPts,  int np);

	BOOL floodFillArr5Multi(CDC* pDC, int nx, int nt, float **xArr, float *rangeArr, 
		double xminData,  double xmaxData, double tminData,  double tmaxData,
		CColorHelper::AMPCOLORDYN *ampColorDyn2);

CSeisWideDoc();
	DECLARE_DYNCREATE(CSeisWideDoc)

// Attributes
public:
	/*  This is useful for converting 2 bytes to 4 bytes, and vice verce. Better to move it under head struct!
	*/
	union TWO_FOUR{
		short hw[2];
		int iv;
	} ssmic;


	CCoords cd;
	CVelMod::VELMOD velMod;
	HEAD head;
	CRayHelper::TRAVELTIME tx;
	CRayHelper::RAYPATHS raypathsTwtt;
	CRayHelper::RAYPATHS raypaths;

	struct POISSONRAY {
		CString rayParaName;
		CTime modifiedRay;
		bool isPrompt;
		double poisson[MOD_LAYER_MAX];
		POISSONRAY() : modifiedRay(0)
		{
			memset(poisson, 0, MOD_LAYER_MAX * sizeof(double));
			isPrompt = true;
		}
	} poissonRay;


	void DrawMarkers(CDC* pDC);

	inline void clearTicks(CDC* pDC) {
		pDC->PatBlt(0, 0, cd.coord.logicalAxes.left, cd.coord.logicalAxes.bottom, PATCOPY);
		pDC->PatBlt(0, 0, cd.coord.viewLogicalTotal.right, cd.coord.viewLogicalTotal.top, PATCOPY);
		pDC->PatBlt(0, cd.coord.logicalAxes.bottom, cd.coord.viewLogicalTotal.right, cd.coord.viewLogicalTotal.bottom - cd.coord.logicalAxes.bottom, PATCOPY);

		pDC->PatBlt(cd.coord.logicalAxes.right, cd.coord.logicalAxes.bottom, 
			cd.coord.viewLogicalTotal.right - cd.coord.logicalAxes.right, 
			cd.coord.viewLogicalTotal.bottom - cd.coord.logicalAxes.bottom, PATCOPY);
	}

	void Tick(CDC* pDC);
	BOOL myParas();
	CSeisWideSrvrItem* GetEmbeddedItem()
		{ return (CSeisWideSrvrItem*)COleServerDoc::GetEmbeddedItem(); }

	BOOL MyMouseClickModelingDepth(SEI_QUAD* pQuad);
	BOOL MyMouseClickDenView(SEI_QUAD* pQuad);
	BOOL MyMouseClickDigitizing(SEI_QUAD* pQuad, CDigitize::DIGITIZE *pdig2);
	BOOL MyMouseClickModelingTimeMode9(CVelMod::VELMOD *velm, SEI_QUAD* pQuad);
	inline BOOL MyMouseClick(SEI_QUAD* pQuad) {
		bool isTrue;
		if (cd.coord.drawmode == 8) {
			return MyMouseClickDenView(pQuad);
		}
		else if (dig.isDigitizing) {
			isTrue = MyMouseClickDigitizing(pQuad, &dig);
			if (isTrue) dig.modifiedtime = CTime::GetCurrentTime();
			return isTrue;
		}
		else if (digMarker.isDigitizing) {
			isTrue = MyMouseClickDigitizing(pQuad, &digMarker);
			if (isTrue) digMarker.modifiedtime = CTime::GetCurrentTime();
			return isTrue;
		}

		if (cd.coord.drawmode == 9) {
			BOOL isTrue = MyMouseClickModelingTimeMode9(&velMod, pQuad);
			CVelMod vm;
			if (isTrue)
				vm.myModel_TWTT2Depth(&velMod, velColor.dz);
			return isTrue;
		}
		if (!cd.coord.isModelingMode()) return FALSE;

		return MyMouseClickModelingDepth(pQuad);
	}


	BOOL InitializeSec();
	static CSeisWideDoc * GetDoc();
      
// Operations
public:
	// These are global variables for a specific document (or window)
 	struct GDCSTRUCT {
		CDC* gDC;  //kept in memory when pDoc->draw.isSameDataDisplay = FALSE, but rewrite when TRUE.
		BOOL gDC_isInit; // not really used
		CCoords::REDUCE gDC_red;

		// added June 30, 2014
		// it is used only in (CView) showing the same gDC at a different reduction velocity
		CRect gDC_rect; // a backup of gRect for later quick drawing without reloading data
		double gDC_ymin;  // it is not affected by face up or down at all!
			// if face up, gDC_ymin corresponds to the bottom of gDC_rect
			// if face down, gDC_ymin corresponds to the top of gDC_rect

		//double gDC_xmin;  // this can be calculated from gDC_tmin and gDC_rect, this cannot be fully recovered from gDC_rect if we do not explicitly assigned?
		//				// actually it is not used yet
		//double gDC_xmax;  // this can be calculated from gDC_tmin and gDC_rect
		//			// actually it is not used yet
		//double gDC_ymax;  // bottomright point of the panel
		//			// actually it is not used yet
		GDCSTRUCT() : gDC(NULL), gDC_isInit(FALSE) {};
	};


	CDigitize::DIGITIZE dig; // used and shared for line drawings or on-screen digitizing
	CDigitize::DIGITIZE digMarker; // used only for drawing points, or for non-digitizing-related point displays
	// will need to free it in destruct. Will load only if mod time changed
	// CTime markerModifiedTime;
	DENMOD gden;

	// this struct is used for area fill, wiggle, and bi-color area fill only
	struct AREAFILL {
		double gain, clip;  // these are valid for fill area or wiggle only. Not used in floodfill functions
		double clip_real;
		double gain_real;
		double fillOff;  //fillOff is the shift in km for fill area usages.
		BOOL isNegativeFill; // for area fill only
		BOOL isFillSmall;
		// BOOL fillColor;  // not used
		AREAFILL() : clip(50.f)
			, gain(0.8)
			, fillOff(0.001) 
			, isNegativeFill(FALSE)	
			, isFillSmall(TRUE) //not saved in version 2.0. Fill every fill-area separately as opposed to fill each trace once only.
		{}
	} areaFill;

	struct DRAW{
		BOOL isSeisScapeAnimation;
		BOOL gainDist;
		double baseGain;	//base gain used for time section plotting only.
		double basePower;
		int wiggle;
		// para.wiggle==0=pure wiggle
		//				1=fill
		//				2=wiggle+fill
		//				3=bi-color fill
		//				4=Color fill
		//				5=color fill+wiggle
		//				6=opengl
		// int isFaceUp; // 0=face down; 1=face up. Should change it to BOOL
		BOOL isRayShowing;
		BOOL overWrite; //temporary for DrawTimeTrc() only.

		float sphericOrder;	//sphericOrder is spherical correction order.
		float	attenuationCorr;
		float	attenuationSeafloor;
		int tstart;  //temporary for DrawTimeTrc() only.
		int iLayerFocus;  //Layer number that gets the focus, Get by View::OnRButtonUp(). Starting from 0 
		int iTextFocus;  //text number that gets the focus, Get by View::OnRButtonUp().
		bool isTopBound;

		int nodeMode;		  //Mode for editting boundary and velocity nodes (0,1,2,3).
							 //if 1 then boundary mode; if 2 then vel mode; 3=text mode (disabled)

		int velFormat;	//Velocity format. Cannot put to the velocity struct 
			// 0=seiswide binary, 
			// 1=zelt's ascii with 4 decimals.
			// 2=3 decimal ascii;  (sorry, swapped due to traditional reasons)
			// 3=2 decimal ascii
			// 4=vxml format; 
		int clipBoardSizeX, clipBoardSizeY; //in mm.
		int progressBarX1, progressBarX2;
		int  traceSkippedInDisplay; 
		int  tracePlottedInDisplay;
		int TTCurveColor;  	
		float printX, printY;	//compression ratios for X and Y extent, with respect to full-page size.
		int memMaxMB;  // used in printing large bitmaps, disabled
		int raymode; // 0=P wave; 1,2=S wave, not used???
		int logScaleThreshold;
		CString timeSecName;
		CString depthSecName;
		CString plotTitle;
		CString velSecNamesEx;
		int ncontFlatten;
		CString rayName; //rayName will be copied to "r.in", for a separate fortran program "myrayinvr.exe".
		// note: if timeSecName = timeSecOld, no need to initize this time section timeSecName.
		//		Same is true for velSecOld and depthOld.
		CString vrmsTauNameDisabled; // V-Tau curve will dynamically be converted to interval velocity and overlain onto display
		CString seiFileName; //  not saved, but will update when opening the sei file... added for convenience of programming
		CString txRaytraceName;  // name for file for tx.in format; I cannot use draw.dig.xxx name since it is used differently
		CString strXLabelOverwrite; // if blank, it will draw as "Distance", 
				// if not blank, this value will be used instead of default.
				// It is dynamically assigned in velocity analysys e.g.
		CString strYLabelOverwrite;

		CCoords::REDUCE red;
		GDCSTRUCT gDCs;
		SEI_PAIR pt_addedR;  // if its isValid value is true, this point should be plotted on screen and then the should be set to false;

		BOOL isRaytracePrompt; // if TRUE, but not prompt for paras, but will pause after finished raytracing
		int restoreLevelCurrent; // not saved. initialize with -1. If -1, restore the most recent velocity backup (say level 7), and this number will be 7.
			// the next time of restore (Cntl+Z), it will restore level 6, and the level index will be reduced to 6. 
		//int restoreLevelMax;  // This is already moved to pFrame->psei->
		int iModelPeglet; // toggle peglet display when showing velocity models
		bool isShowTheoryModel;
		double modelUpInc; // def. 0.02km, not saved
		double tmpDxdyDrawn;  // user defined a valued on screen
		BOOL isColBar;
		BOOL isVelErr;
		BOOL isNodeChanged; // this affects nodes display only, not affecting segy or model displays.
			// note: if a model is displayed (cd.coord.drawmode==3, 9 et al.) are exceptions since will always has re-draw the velocity model nodes. So 

		BOOL isSameDataDisplay; //if TRUE then use the old mDC without reloading the segy or even model data.
		// Note: isSameDataDisplay is used for dealing with seismic amplitudes: if true, Seiswide will not
		//			load the seismic section again to save time.
		//		isSameNodeDisplay is useful for onscreen digitizing: if false, clicking will not refresh 
		//		anything already onscreen, only a new point is added onscreen. this save screen refreshing
		// 	Prefer to use  draw.isSameDataDisplay==FALSE,   instead of setting to blank for any one of:  CString timeSecOld, velSecOld, depthSecOld
		//  But setting draw.timeSecOld!=draw.timeSecName   would initialize the seismic section with amp initialization, while  draw.isSameDataDisplay==FALSE   would not initilize these data
		//  But setting velMod.velSecOld!=velMod.velSecName   would initialize the velocity bin file with color gridding initialization, while  draw.isSameDataDisplay==FALSE   would not initilize this

		BOOL isCopying; //isCopying is initilized in contructor.
		BOOL isVectoring;
		BOOL isNoNodes;
		BOOL isNoBoundary,isNoColorMap,isNoOBSPos;
		BOOL isNoLableOutside;
		BOOL isNoLabelInSide;  // control if line labels should be plotted
		BOOL isNoOutSidePoints; // for excluding rays/lines in plots et al.
		BOOL isNoPromptLabel;  // not prompt for a new dialogbox for inputting line labels
		BOOL isNoHorzGrid;
		BOOL isSeismicPlot;
		BOOL isTerminate; // indicate the processing should stop asap. May not have been used at all
		float distShift;  // for display reflection data: shift distance without modifying trace header. Useful for multiple SBs on the same big reflection segy
		BOOL isSegyKeyXShift; // indicate if the arrow keys should respond to segy shifting
		float segyKeyXShift;  // arrow keys indicating X-Shifting in km
		float ampPower; // default 1.f means not raising power for display
		float xWell; // added/restored on Feb. 6, 2018. Each model has its own xWell, but we want to be able to move
		// current Window's xWell without affecting the model's xWell!!!

		// BOOL isShiftPressed;
		BOOL isPrintingW; // 0=not printing;  1=printing just started (coord mapping can change upon request); 2=printing on-going (coord mapping cannot be changed)
		BOOL isWinShown; // indicates if the current window is already shown. 
		// end moving to mainframe

		// tempoary values for different purposes
		double twin_min; // tempoary
		double twin_max; // tempoary
		double twin_lag; // tempoary
		float  twin_lagWeight; //tempoary
		BOOL   twin_isEnergy; // tempoary
		int  twin_numSubdivide; // tempoary

		inline bool isWiggleInDisplay() {
			return wiggle == 0 || wiggle == 2 || wiggle == 5;
		}

		DRAW() : 
			ncontFlatten(0),
			//numContours(50),
			logScaleThreshold(86),
			isSegyKeyXShift(FALSE),segyKeyXShift(0.f),
			isShowTheoryModel(false), isNoPromptLabel(FALSE),
			isNoOutSidePoints(TRUE), isNoHorzGrid(TRUE),
			isSeismicPlot(TRUE),
			tmpDxdyDrawn(0.f)
			, twin_isEnergy(FALSE)
			, twin_lag(0.04)
			, twin_lagWeight(0.9f)
			, twin_min(0.04)
			, twin_max(0.1)
			, twin_numSubdivide(5)
			, gainDist(FALSE)
			, baseGain(5.)
			, basePower(1.)
			, isRayShowing(FALSE)
			, wiggle(4)
			, sphericOrder(0), attenuationCorr(0), attenuationSeafloor(4.5f)
			, overWrite(FALSE)
			, xWell(ERRORVAL)
			, isWinShown(FALSE)
			, isSeisScapeAnimation(TRUE)
		{
			
			gDCs.gDC = new CDC();
			clipBoardSizeX = 200; //in mm.
			clipBoardSizeY = 160; // in mm.
			distShift = 0.f;
			isColBar = FALSE;
			ampPower = 1.f;
			isCopying = FALSE;
			isVectoring = FALSE;
			isNoNodes = FALSE;
			isPrintingW = FALSE;
			isNodeChanged = TRUE;
			isSameDataDisplay = FALSE;
			isNoColorMap = FALSE; //not saved in version 2.0.
			isNoBoundary = FALSE; //not saved in version 2.0.
			isNoNodes = FALSE;
			isNoOBSPos = FALSE; //not saved in version 2.0.
			iLayerFocus = -1;
			iModelPeglet = -1;
			isTopBound = true;
			isRaytracePrompt = TRUE;
			memMaxMB = 450;

			//to avoid accidentaly delete, reset these:
			modelUpInc = 0.01f; // def. 0.01km, not saved
			nodeMode = 1;  //if 1 then boundary mode; if 2 then vel mode.
			printX = 1.f;
			printY = 1.f;
			raymode = -1; // this will prompt selection of P or S wave velocities if no overwrite later
			restoreLevelCurrent = -1; // added June 14, 2010
			TTCurveColor = 4;
			isVelErr = FALSE; //not used in ver 3.3.
			strXLabelOverwrite = strYLabelOverwrite = _T("");
			isNoLabelInSide = isNoLableOutside = FALSE;
			velSecNamesEx = timeSecName = depthSecName = vrmsTauNameDisabled = _T("");
		};
		~DRAW() {
			gDCs.gDC->DeleteDC();
			delete gDCs.gDC;
			gDCs.gDC = NULL;
		}
	} draw;

/*
struct Date
{
    int day;
    int month;
    int year;
    Date(int d=0, int m=0, int y=0):day(d),
           month(m),
           year(y){}
};
Date d( 4, 2, 42 );
*/

	CColorHelper::VELCOLOR velColor;
	CColorHelper::LINECOLORS lineColors;
	CColorHelper::SEISBICOLOR seisBiColor2;

	//These parameters are for KD Migration
	struct KDMIG{
		BOOL ek,npv;
		float amax,amin,aperx,da,doff,angmax,dt,dxv,dzv,dxm,dxo;
		float fa,fac;
		float dxt,dzt,fs,ds; 
		int na,noff,nt,ns,ls;
		float off0,offmax,v0,dvz,fmax;
		float memMax;  // this is to disable!
		float tmaxRay;
		CString migLog; // log file name
		float sourceDepth;  // in meters, added by Deping for migrating wide-angle data
		KDMIG() : ek(TRUE) {
			ek = TRUE; 
			npv = FALSE;
			amax = 180.f; 
			amin = 0.f;
			doff = 99999.f; 
			angmax = 60.f;
			dzv = 0.004f;
			dxv = 0.0125f;
			ds = dxm = dxo = 0.025f;
			fa = -60.f; 
			da = 2.f; 
			na = 61;
			fac = 0.01f;
			tmaxRay = 12.f; 
			dt = 0.008f;
			ns = 1; 
			ls = 1;
			off0 = 0.f; 
			offmax = 4.f;
			v0 = 0.f;
			dvz = 0.f;
			fmax = 99999.f;
			aperx = 5.f;
			memMax = 32.f;
		};
	} kd;
	/*

	*/
	struct BUTTONS{
		BOOL m_pick, m_remove, m_move, m_add, m_duplicate, m_newLine, m_delLine;
	} bt;



	struct BMP{
		//A bitmap of the same name can be either in time or depth domain.
		//Only one bitmap name is allowed for 1 document.
		CString bmpName;
		CxImage *ximage;
		int isInitializeNeeded; //if 1 then need initialization; 
				// 0=No need;  -1=During init, so do not refresh

		float xmin, ymin, xmax, ymax;   // CANNOT change to double since they are saved to .sei files.
			// correspond to the upper left and lower right corner. Their values can be tricky to obtain
			// they are true world coordinates
			// (bmp24.xmin, bmp24.tmin)  will correspond to  pixel zero
			// (bmp24.xmax, bmp24.tmax)  will correspond to  another pixel corner
		// NOTE: they must be float and not double, since they are stored in files that way!!!

		inline SEI_PAIR getWorldPoint(int ix, int iy) {
			SEI_PAIR seip;
			if (ximage) {
				// double rate = 1. / (ximage->GetWidth()-1);
				seip.x = xmin + ix*(xmax - xmin) / (double)(ximage->GetWidth()-1);
				seip.y = ymin + iy*(ymax - ymin) / (double)(ximage->GetHeight()-1);
				seip.isValid = true;
			}
			return seip;
		}

		inline int getPixelIndexX(double x) {
			int i = 0;
			if (ximage) {
				i = (int)((x - xmin) / (xmax - xmin) * (ximage->GetWidth()-1) );
			}
			return i;
		}

		inline int getPixelIndexY(double y) {
			int i = 0;
			if (ximage) {
				i = (int)((y - ymin) / (ymax - ymin) * (ximage->GetHeight() - 1));
			}
			return i;
		}

		// setWorldPoint() assumes the bitmap is all facing down, 
		//    modify the internal (xmin, ymin) and (xmax, ymax) such that
		//    the the pixel index (ix1, iy1) and (ix2, iy2) shall point to 
		//    world coord values of (x1, y1) and (x2, y2). 
		inline bool setWorldPoint(double x1, double y1, 
			double x2, double y2,
			int ix1, int iy1,
			int ix2, int iy2
			) {
			if (!ximage || ix1<0 || ix2<0 || 
				ix1==ix2 || iy1==iy2) return false;
			double xrate = (x2 - x1) / (double)(ix2 - ix1);
			xmin = (float)(x1 - ix1 * xrate);
			xmax = (float)(xmin + xrate * (double)(ximage->GetWidth()-1));

			double yrate = (y2 - y1) / (double)(iy2 - iy1);
			ymin = (float)(y1 - iy1 * yrate);
			ymax = (float)(ymin + yrate * (double)(ximage->GetHeight()-1));
			return true;
		}


		// modifyWorldPoint() will take (x1Old, y1Old) and (x2Old, y2Old) that were 
		//   based on old coord system (i.e. existing set of xmin, ymin, xmax, ymax)
		//   and modify this coord system to make the 2 points to 
		//   become (x1, y1) and (x2, y2)
		inline bool modifyWorldPoint(
			double x1, double y1,
			double x2, double y2,
			double x1Old, double y1Old,
			double x2Old, double y2Old
			) {
			if (!ximage || x1Old == x2Old || y1Old == y2Old) return false;
			if (x1Old < xmin || x2Old > xmax) return false;
			if (y1Old < ymin || y2Old > ymax) return false;

			// first find out the pixel index for the old slecified values
			int ix1 = getPixelIndexX(x1Old); // (int)(rate * (x1Old - xmin));
			int ix2 = getPixelIndexX(x2Old); // (int)(rate * (x2Old - xmin));
			int iy1 = getPixelIndexY(y1Old); // (int)(rate * (y1Old - ymin));
			int iy2 = getPixelIndexY(y2Old); // (int)(rate * (y2Old - ymin));
			if (ix1<0 || ix2<0 || iy1<0 || iy2<0 ||
				ix1 == ix2 || iy1 == iy2) return false;
			// note that pixel index shall not change even the mapping changes

			return setWorldPoint(x1, y1, x2, y2, ix1, iy1, ix2, iy2);
		}


		// DIB/BMP always goes downward/face down, but in plotting, it can either face up or face down.

		double vred;  // this keeps track of what vred the BMP is. One can change the reducing velocity based on this original vred. Not used yet.
		// int numBits; // can be 24, 32, or 1
		BMP() :
			ximage(NULL), isInitializeNeeded(1) {
			bmpName = _T("");
			xmin = 0;
			xmax = 100;
			ymin = 0;
			ymax = 100;
		}
	} bmp24;

	struct PROC{
		struct FILTERXT {
			float ta;
			float tb;
			float fLowa;
			float fHigha;
			float fLowb;
			float fHighb;
			float x1;

			float tax2;
			float tbx2;
			float fLowax2;
			float fHighax2;
			float fLowbx2;
			float fHighbx2;
			float x2;

			float tax3;
			float tbx3;
			float fLowax3;
			float fHighax3;
			float fLowbx3;
			float fHighbx3;
			float x3;

			int num_tfilter;
			FILTERXT() {
				ta = 5.f;
				tb = 10.f;
				fLowa = 3.f;
				fHigha = 60.f;
				fLowb = 2.f;
				fHighb = 30.f;
				x1 = 0.f;

				tax2 = 5.f;
				tbx2 = 12.f;
				fLowax2 = 2.f;
				fHighax2 = 35.f;
				fLowbx2 = 1.f;
				fHighbx2 = 26.f;
				x2 = 20.f;
				tax3 = 5.f;
				tbx3 = 13.f;
				fLowax3 = 1.f;
				fHighax3 = 20.f;
				fLowbx3 = 0.5f;
				fHighbx3 = 20.f;
				x3 = 30.f;
				num_tfilter = 2;
				//ta = 4.f;
				//tb = 10.f;
				//fLowa = 3.f;
				//fHigha = 60.f;
				//fLowb = 3.f;
				//fHighb = 16.f;
				//x1 = 1.f;
				//tax2 = 5.f;
				//tbx2 = 11.f;
				//fLowax2 = 3.f;
				//fHighax2 = 50.f;
				//fLowbx2 = 3.f;
				//fHighbx2 = 15.f;
				//x2 = 2.f;
				//tax3 = 6.f;
				//tbx3 = 12.f;
				//fLowax3 = 3.f;
				//fHighax3 = 40.f;
				//fLowbx3 = 3.f;
				//fHighbx3 = 14.f;
				//x3 = 3.f;
			}
		} filterxt;
	} processing;

	CSize sizeDoc;
 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeisWideDoc)
	public:
	virtual void Serialize(CArchive& ar);
	protected:
	virtual COleServerItem* OnGetEmbeddedItem();
	//}}AFX_VIRTUAL

// Implementation
public:
	// special case: in half offset mode, offsets are half-ed, and digitized points are not reduced,
	// but travel-time curved are still reduced. Zelt's mode cannot be this mode, evidently.
	inline bool isHalfOffsetMode() {
		return false; // disable this function. Use it only for testing!!! Otherwise it may cause problems of understanding

		return 
			cd.coord.isDistanceDisplay && cd.coord.drawmode == 7 && draw.red.rvred > 0.1 && !cd.coord.isFaceUp && velMod.isVelModValid();
	}


	BOOL myDigSaveAs(CString aName, CString aNameOrig);
	BOOL myDigSaveAs(CString aName);

	void myCursorsAll(bool isBusy = false);
	//int getPtdraw(BOOL pick, BOOL remove, BOOL move, BOOL add);
	//void setPtdraw(int statNew, BOOL &pick, BOOL &remove, BOOL &move, BOOL &add);

	// why CView* is needed? It's because a CDOCUMENT can have several views. We could just assume the first view, but that would put restrictions
	void DrawControlsSegy(CDC *pDC, CColorHelper::AMPCOLORDYN *pAmpColorDyn, CView*);

	BOOL CSeisWideDoc::DrawSectionPrestack(CDC* pDC, HEAD_READ *pHeadRead,
		double xminPlot, double xmaxPlot,
		double yminPlot, double ymaxPlot,
		int drawmode, int ffid, int wiggle,
		CColorHelper::AMPCOLORDYN *ampColorDyn2, CView* pView);

	BOOL DrawSectionPrestack(CDC* pDC, CString fSection,
		double xminPlot, double xmaxPlot,
		double yminPlot, double ymaxPlot, 
		CColorHelper::AMPCOLORDYN *ampColorDyn2, CView*);

	BOOL DrawSectionArr(CDC* pDC, CString fSection, double yminPlot, double ymaxPlot, CColorHelper::AMPCOLORDYN *ampColorDyn2, CView*);
	BOOL DrawSectionArr3D(CDC* pDC, CString fSection, CString fHilt, CString fCohr, double ymin, double ymax, CView*);
	BOOL DrawSectionTrcArr(CDC* pDC, CString fSection, int trace1, int trace2, CColorHelper::AMPCOLORDYN *ampColorDyn2, CView*);
	BOOL DrawColorbar(CDC* pDC);
	void drawFloatReflectors(CDC *pDC);
	void DrawControlsEx(CDC *pDC);
	void DrawXImage(CDC *pDC);
	void DrawRayPath(CDC* pDC);

	//BOOL showDigLineDraw(CDC *pDC);
	BOOL myShowDigitizeZeltOnly(CDC* pDC); // this draws TT curves with random color with thicker normal lines
	BOOL myShowDigitizeTimeDomain_Depreciate(CDC* pDC); // this draws TT curves with random color with thicker normal lines
	BOOL myShowDigitize(CDC* pDC, CDigitize::DIGITIZE *dig2, bool isMarker, bool isReduce);
	inline double getXShotAll2(HEAD *aHead) {
		double out = ERRORVAL;
		if (aHead->numTraces > 0) {
			out = aHead->xshotInSegy;
			if (CCoords::isXShotDefined((float)out))
				return out;
		}
		if (dig.isZeltFormat) {
			out = dig.xShotCache;
			if (CCoords::isXShotDefined((float)out))
				return out;
		}

		// now consider tx.out
		CString txPath = getWorkingPathSlash() + _T("tx.out");
		if (!isDirectoryExist(txPath)) {
			txPath = _T("tx.out");
		}
		if (CRayHelper::myLoadTravelTime(txPath, &tx, (float)aHead->xshotInSegy)) { // this is fast since tx* is cached
			return tx.xShotRay;
		}
		return ERRORVAL;
	}
	BOOL myShowTravelTime(CDC *pDC, CPen *bPen); // this enforces a color for the TT curves with 0 thickness
	void myUpdateStatusBar();
	BOOL checkModelError(CString name);
	BOOL CheckTime(long daySet);
	BOOL isModelVisible(CVelMod::VELMOD *velm, double xShift);

	virtual ~CSeisWideDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	BOOL myModelRestore(BOOL isBackward = TRUE);
	//BOOL myMakeSameDrive(CString &fName, CString fNameRef);
	//BOOL myMakeSameNetwork(CString &fName, CString fNameRef);
	BOOL getLineColorArray();
	float getDepthAtSeafloor(float dist);
	BOOL KdmigGeneral(FILE* jpfp, CFile* file2, BOOL useExist, BOOL preStack, BOOL appendOutput);
	BOOL drawLineDrawing(CDC *pDC);
	void extraSerialize();
	BOOL reduction2TrcHeader(float vred);
	//{{AFX_MSG(CSeisWideDoc)


	void processingNormMulti(HEAD_READ *headRead, BOOL isNorm, float normValue, BOOL isMultiple, float exponentValue, float multiplyValue);
	void processingHilbertNorm(HEAD_READ *headRead, BOOL isHilbert, BOOL isTranspose, BOOL isHilbertMix);
	bool processingFilterTimeDomain(HEAD_READ *headRead, double f1a, double f2a);
	bool processingFilterTimeDomain(HEAD_READ *headRead, 
		double f1a, double f2a, 
		double f1b, double f2b, double ta, double tb);

	bool processingFilterFreqDomain(HEAD_READ *headRead, 
		double f1, double f2, double f3, double f4, 
		double ta, double tb, int num_tfilter, bool zerophase);
	void myModelingRaytrace(bool isForceDlg);

	void myToolbarUpOnly();
	void myToolbarDownOnly();

	afx_msg void OnEditBoundary();
	afx_msg void OnEditVelocity();
	afx_msg void OnEditNonodes();
	afx_msg void OnEditCopy();
	afx_msg void OnEditDisplaytuneupFillareaonce();
	afx_msg void OnEditLabelOutside();
	afx_msg void OnEditTuneupNoboundary();
	afx_msg void OnEditTuneupNocolormap();
	afx_msg void OnEditTuneupObspositions();
	afx_msg void OnEditImagestretchmode();
	afx_msg void OnEditDisplaytuneupBigmodelnodes();
	afx_msg void OnEditDisplaytuneupChangcolorttcurves();
	afx_msg void OnEditLabelInside();
	afx_msg void OnEditDisplaytuneupRemoveoutsideraypoints();
	afx_msg void OnEditPaste();
	afx_msg void OnEditDisplaytuneupRemovettoverlay();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnEditDisplaytuneupAdjustinglabelsizes();

	afx_msg void OnModelingExtractvzcurve();
	afx_msg void OnModelingMoveboundary();
	afx_msg void OnModelingRaypath();
	afx_msg void OnModelingDefinesourceposition();
	afx_msg void OnModelingExtractlayerinfor();
	afx_msg void OnModelLoadzeltsmodel();
	afx_msg void OnModelingLoadreidsmodel();
	afx_msg void OnModelNewmod();
	afx_msg void OnModelFlipmod();
	afx_msg void OnModelingSubsidence();
	afx_msg void OnModelShiftxz();
	afx_msg void OnModelingMergemodel();
	afx_msg void OnModelingGeneratesyntheticsegydata();
	afx_msg void OnModelingDosconsol();
	afx_msg void OnModelup();
	afx_msg void OnModeldown();
	afx_msg void OnModelingDisplaycircle();
	afx_msg void OnModelingAddsineshape();
	afx_msg void OnModelingDumpcmpTwttOld();
	afx_msg void OnModelingDumpcmpTwtt();
	afx_msg void OnModelingTogglepeglet();
	afx_msg void OnModelingToggleswavelabels();
	afx_msg void OnModelconversionRmsToIntNotUsingModel();
	afx_msg void OnModelconversionRmsToIntUsingModel();
	afx_msg void OnModelconversionRmsToIntTwtt();
	afx_msg void OnModelconversionIntToRmsvelocity();
	afx_msg void OnModelconversionTormsvelocity2();
	afx_msg void OnModelingTheoreticalcurves();
	afx_msg void OnModelPerturbvel();
	afx_msg void OnModelToggledetails();
	afx_msg void OnModelingDenblocks_Test();
	BOOL convVelocity2Density();
	afx_msg void OnModelingGravitymodelingDenplot();
	afx_msg void OnModelingMag();
	afx_msg void OnModelingFlattenhorizon();
	afx_msg void OnModelingRaytrace();
	afx_msg void OnModeltoolsGeneratekml();

	afx_msg void OnProcessingTracereorder();
	afx_msg void OnProcessingAddrangetosegy();
	afx_msg void OnProcessingDatareductionandfilter();
	afx_msg void OnProcessingLateralshift();
	afx_msg void OnProcessingCheckmodelintegrity();
	afx_msg void OnProcessingAppendasegyfile();
	afx_msg void OnProcessingReducemanual();
	afx_msg void OnProcessingUnix2pcbatch();
	afx_msg void OnProcessingPc2unixbatch();
	afx_msg void OnProcessingRawtosegy();
	afx_msg void OnProcessingMigps();
	afx_msg void OnProcessingKdmigpost();
//	afx_msg void OnProcessingKdmigsimple();
	afx_msg void OnProcessingLatlongtodistance();
	afx_msg void OnProcessingTimeDepthDrawings();
	// afx_msg void OnProcessingTdconvbmp();
	afx_msg void OnProcessingDataresample();
	afx_msg void OnProcessingDebias();
	afx_msg void OnProcessingWiener();
	afx_msg void OnProcessingChangereducevelocityforbmp();
	afx_msg void OnProcessingAppendmanysegyfiles();
	afx_msg void OnProcessingTimedepthConv();
	afx_msg void OnProcessingDatadespike();
	afx_msg void OnTimeTimesectionwithperturbedtravel();
	afx_msg void OnProcessingDumpsegylatlong();
	afx_msg void OnProcessingDecimatedatalines();

	void myUpdatePrevFrame(void);
	int myUpdateAllFramesExcept(CDocument *aDoc);
	int myCountAllFrames();
	// afx_msg void OnEditText();

	afx_msg void OnProcessingDatasplit();
	afx_msg void OnProcessingIdProcessingTracedub();
	afx_msg void OnProcessingSphericalcorrection();
	afx_msg void OnProcessingChopdata();
	afx_msg void OnProcessingAdddata();
	afx_msg void OnTimeRmsvelocityanalysis();
	afx_msg void OnMenuProcessingTrcBalancing();
	afx_msg void OnMenuTracebalanceXt();


	afx_msg void OnSonobuoyAutocalculateoffsetfromlat();
	afx_msg void OnSonobuoyLinearKm();
	afx_msg void OnSonobuoyPoly();
	afx_msg void OnSegyconversionIeeetoibmfloat();
	afx_msg void OnSegyconversionUnixtopcconvert();
	afx_msg void OnProcessingDatanormalize();
	afx_msg void OnProcessingFilterOffsetvarying();
	afx_msg void OnSonobuoyAssignoffsetfromsegy();
	afx_msg void OnProcessingGainadjust();
	afx_msg void OnProcessingTracesubtractalongcurve();
	afx_msg void OnProcessingSplitoddeventraces();
	afx_msg void OnReflectionFaultenhancingOld_simpleSubtract();
	afx_msg void OnReflectionFaultenhancing();
	void OnSonobuoyPolyTrace();
	afx_msg void OnReflectionCoherencyplot();
	afx_msg void OnWideNmostretching();
	afx_msg void OnSonobuoyShifttracenumber();
	afx_msg void OnProcessingDistanceadjustment();
	//afx_msg void OnReflectionFlattenboundary();



	afx_msg void OnProcessAsciiheader();
	afx_msg void OnProcessingTaupslantstack();
	afx_msg void OnProcessingTaupinv();
	afx_msg void OnProcessingTfdn();
	afx_msg void OnProcessingSpectrum();
	afx_msg void OnTimeForwideNmovelocity();

	
	afx_msg void OnBitmap2Segy();
	afx_msg void OnOffsetChosentrace();
	afx_msg void OnSonobuoyCopydata();
	afx_msg void OnSonobuoyOffsetjitteringremoval();
	afx_msg void OnProcessingReverserange();
	afx_msg void OnOptionsToggleclipchangewithgain();
	afx_msg void OnSonobuoyAutointerpolateinvalidoffsets();
	afx_msg void OnRaytraceDebug();
	afx_msg void OnWideNormalizetraceinterval();
	afx_msg void OnWideDefinesrc();

	afx_msg void OnToolsMapconverttime();
	afx_msg void OnToolDumppolynomialpoints();
	afx_msg void OnToolsPicknext();
	afx_msg void OnToolsDumplat();
	afx_msg void OnToolbarPlus();
	afx_msg void OnToolbarMinus();
	afx_msg void OnToobarMove();
	afx_msg void OnToolbarRemove();
	afx_msg void OnToolbarPick();
	afx_msg void OnToolbarAdd();
	afx_msg void OnToolbarSavev();
	afx_msg void OnToolbarSaveasv();
	afx_msg void OnToolbarRefresh2();
	afx_msg void OnToolbarDuplicate();
	afx_msg void OnToolbarResizemod();
	afx_msg void OnToolbarDelLine();  // delete current digitizing line or  modeling boundary
	afx_msg void OnToolbarBiggerAmp();
	afx_msg void OnToolbarLessAmp();
	afx_msg void OnToolbarRefreshAll();
	afx_msg void OnDigitizingSavedigffid();
	afx_msg void OnTimeForwide();

	afx_msg void OnViewTimesectrc();
	afx_msg void OnViewGravityCont2();
	afx_msg void OnViewVelocitycolortwtt();
	afx_msg void OnViewBoth();
	afx_msg void OnViewContours();
	afx_msg void OnViewTimesection();
	afx_msg void OnViewVelocitycolormap();
	afx_msg void OnViewHydro();
	afx_msg void OnViewDepthsection();
	afx_msg void OnViewStackingvelocitymap();
	afx_msg void OnViewSegyinfo();
	afx_msg void OnViewTraceanalysis();
	afx_msg void OnViewNmoVelocity();

	afx_msg void OnDigitizingLoadexisting();
	afx_msg void OnDigitizeUndopoint();
	afx_msg void OnDigitizingStart();
	afx_msg void OnDigitizingSavedig();
	afx_msg void OnDigitizingNewcurve();
	afx_msg void OnDigitizeFinishdigitizing();
	afx_msg void OnDigitizingSelectCurrent();
	afx_msg void OnDigitizingChangeerrorbarRedefinesource();
	afx_msg void OnDigitizingZeltsformatTogglezplot();
	afx_msg void OnDigitizingMigratelinedrawings();
	afx_msg void OnDigitizingChangecurveres();
	afx_msg void OnDigitizingLoadzeltTx();
	afx_msg void OnDigitizingWhereani();
	afx_msg void OnDigitizingShiftdigiting();
	afx_msg void OnDigitizingChangeerrorbarAsBar();
	afx_msg void OnDigitizingChangeerrorbarAscircle();
	afx_msg void OnDigitizingChangeerrorbarAsstar();
	afx_msg void OnDigitizingChangeerrorbarAsfillcircle();
	afx_msg void OnDigitizingResizeSegments();
	afx_msg void OnDigitizingSplit();
	afx_msg void OnDigitizingReviselinelabel();
	afx_msg void OnDigitizingZeltSwapsource();


	afx_msg void OnFileImportbitmap();
	// afx_msg void OnFileResizebitmap();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL MyLoadDistTwttDrawings4John(CString inName);
	BOOL MyLoadDistTwttDrawings4John(CString sgyName, CString lstpcName, CString digName);
	int setShotsFromCoherency(CStringArray *sarray, int *filej, int *shotk, float *shotDist, int numShotIn, int numShotOut, int iwin1, int iwin2, int itauHalf, int itauInc, float freq1, float freq2, float distMaxPreferred);
	// Load xshot,zshot information from ray parameter file r.in
	

	// Get a color from a file for a given ray code. If not existing in the file, create the file. 
	COLORREF getRaycodeColor(int irrCol);
	COLORREF getRaycodeColor(float rayCode);
	COLORREF getRaycodeColor(int irrCol, BOOL isColorRecalc);
	COLORREF getRayColor(int irayCol);

	// This will calculate a big list of contours as floating reflectors for raytracing, for NMO velocity analysis using semblence stack
	CString setContour4Raytrace(CString aFile, float xpos, float dz, float zmin, double *zmaxMod, float vminPerturb, float vmaxPerturb, float dvPerturb);
	CString setContour4Raytrace(CString aFile, float xpos, float dz, float zmin, double *zmaxMod, 
									  float vminPerturb, float vmaxPerturb, float dvPerturb, 
									  int *nArray, float *xArray, float *zArray );
public:
	inline void clearGDC(CDC* aDC) {
		if (aDC != NULL) {
			CRect rect;
			aDC->GetClipBox(&rect);
			aDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		}
	}
	//BOOL initDigitizeArray(int npoint, int ncurve);
	CString getWorkingPathSlash();

	void myClipReducePoints4Depreciate(CDigitize::DIGITIZE *digIn, CDigitize::DIGITIZE *digOut, CCoords::REDUCE *pred, bool isReducedOutput);
	double getXp(double p, int nz, float dz, float vTrc[]);
	double setTp(double p, int nz, float dz, float vTrc[]);
	double getXp2(float p, float z, int ncont, double depth[], double vTop[], double vBot[]);
	double setTp2(float p, float z, int ncont, double depth[], double vTop[], double vBot[]);
	bool getXp3(float p, float z, int ncont, double depth[], double vTop[], double vBot[], double xt[]);
	BOOL DrawVelocityColormapDepth(CDC* pDC, CVelMod::VELMOD *velm, BOOL isNoNodes, double xShift, int iLayerFocus=0);  
	BOOL DrawVelocityBoundaryDepth(CDC* pDC, CVelMod::VELMOD *velm, BOOL isNoNodes, double xShift, CPen* pBluePen = NULL, int iLayerFocus = 0);
	BOOL DrawVel(CDC* pDC);  
	BOOL DrawVelocityLabelDepth(CDC* pDC, CVelMod::VELMOD *velm, double xShift, COLORREF cr);
	BOOL DrawStackVelocity(CDC* pDC);
	void DrawTempPoint(CDC* pDC, SEI_PAIR seip);
	void DrawVelocityTheoreticalDepth(CDC* pDC, CVelMod::VELMOD *velm);
	void DrawVelocityLabelTwtt2(CDC* pDC, CVelMod::VELMOD *velm, double xShift, COLORREF cr);
	BOOL DrawVelocityTwtt(CDC* pDC, CVelMod::VELMOD *velm, BOOL isNoNodes, 
		double xShift, 
		CPen* pBluePen = NULL, int iLayerFocus = 0);
	BOOL CSeisWideDoc::DrawVelocityUncontroledLeft(CDC* pDC, CVelMod::VELMOD *velm, double xShift);
	BOOL CSeisWideDoc::DrawVelocityUncontroledRight(CDC* pDC, CVelMod::VELMOD *velm, double xShift);
	bool DrawVelocityTwtt(CDC* pDC);

	// Get depth for a point at xpos for a given velocity in a given layer
	double getDepthFromVel(double dist, double vel, int icont, double percent);
	void close_unused_documents(const CDocument* const pNotThisOne);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void OnDisplaytuneOverwritexlabel();

	bool myOpenNewFile(CString fName);


private:
	// get next model control point after the xCurrent
	float getModelPointNext(float xCurrent);
//	void ReflectionCoherencyplotTrc();
	bool processingTfdn(HEAD_READ *headRead, int drawmode, BOOL isDistanceDisplay, 
		double x1, double xWinMix, double t1, double tWinMix, double specThreshald, int numTrcMax);
	bool processingCoherencyCenter(HEAD_READ *headRead,
		int drawmode, bool isDistanceDisplay, int multByCohMode, int numSmooth,
		double dxMix1, double dxMix2, double dxAvgInterval, double ymin, double ymax, double tWindowFull,
		double slopeMin, double slopeMax);
	bool processingCoherencyCenter(HEAD_READ *headRead,
		int drawmode, bool isDistanceDisplay, int multByCohMode, int numSmooth,
		double dxMix1, double dxMix2, double dxAvgInterval, double ymin, double ymax, double tWindowFull,
		double slopeMin, double slopeMax,
		double, double);

	bool processingCoherencyCenterTrc(HEAD_READ *headRead, int multByCohMode, int numSmooth,
		int numTraceMix1, int numTraceMix2,
		double ymin, double ymax, double tWinCenter,
		double slopeMin, double slopeMax);

	void DrawRayPathTwtt(CDC * pDC);
	double getXminFile(void);
	double getXmaxFile(void);
	int writeHeader4ZeroTrcNum(CString f);
	CString myDrawmode2String();

public:

	afx_msg void OnToolsCalcPoisson();
	afx_msg void OnModelingSethorizonlimitswithraypaths();

	//void StartOpenGL(CView*, CDC*, int nx, int nt, 
	//	float **xArr, float,
	//	float [], float [],
	//	double xmin, double xmax, double tmin, double tmax,
	//	CColorHelper::AMPCOLORDYN *ampColorDyn2, double, double);

	void StartOpenGL2(CView* pView, CDC *pDC, float **ampArr,
		float x[], int nx, float y0, float dy, int ny,
		CColorHelper::AMPCOLORDYN *ampColorDyn2, CCoords::SEI_COORD *pcd);

	void StartOpenGL2_OLD(CView*, CDC*, int, int,
		float**,
		float[], float[],
		CColorHelper::AMPCOLORDYN*,
		CCoords::SEI_COORD*);

	void StartOpenGL3_2DOnly(CView*, CDC*, int, int, CColorHelper::rgb **, float[], float[], CCoords::SEI_COORD*);
	OpenGLRendererFBO m_GLWin;

	//void myFileOpenproject(CString fSeiProject);  // moved to CMainFrame class

	afx_msg void OnReflectionAssociatekmtotracefromfile();
	afx_msg void OnDigitizeCurvestosegy();
	afx_msg void OnDigitizeEndcurvestosegy();
	afx_msg void OnDigitizeDefineXval();
	afx_msg void OnModeltoolsDumpvelocityrangetokml();
	afx_msg void OnBitmapsTimelineremoval();
	afx_msg void OnProcessingResetsamplerate();
	afx_msg void OnDigitizingShowfile();
	afx_msg void OnToolsInterpInvalidT0();
	afx_msg void OnDigitizingSwitchdigitizingfile();
	afx_msg void OnDigitizingStartmarker();
	afx_msg void OnFileOpenproject();
	afx_msg void OnFileSaveproject();
	afx_msg void OnFileCloseproject();
	afx_msg void OnReflectionZt();
	afx_msg void OnWideOffsetsfromsrc();
	afx_msg void OnDigitizingSavehalfuncertainty();
	BOOL DrawDenMod(CDC* pDC, DENMOD* pden, int iFocus);
};

/////////////////////////////////////////////////////////////////////////////
