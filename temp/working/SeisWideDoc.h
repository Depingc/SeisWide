

// SeisWideDoc.h : interface of the CSeisWideDoc class
//
/////////////////////////////////////////////////////////////////////////////
//#define DIG_POINT_MAX 100
//#define DIG_LAYER_MAX 3000
#define MOD_LAYER_MAX 50
#define MOD_POINTS_MAX 999
#define TX_POINTS_MAX 999
#define TX_CURVES_MAX 299
#include "segy.h"
#include "DIB.h"


class CSeisWideSrvrItem;

class CSeisWideDoc : public COleServerDoc
{
protected: // create from serialization only
	BOOL setGainCorr(segy *tr,int dformat, int tpp);
	BOOL setGainCorr(segy *tr, int dformat, int tpp, float sphericOrder, float attenuationCorr, float attenuationSeafloor);
	BOOL drawTrcInfo(CDC* pDC, float xkm, segy *tr, int *sPos, int *sLen, BOOL isDetail = FALSE);
	float GetContourDepth(float *zContour, int nx, double x0, double xmin, double dx);
	// float CSeisWideDoc::GetContourDepth(float zContour[], int nx, double x0, double xmin, double dx);
	BOOL rayIntersect(double a, double b, int nx, double xmin, double dx, double *x4, double *z4, double *aBar, float *zCont);
//	void rayIntersect(double a, double b);
	BOOL GetVelGrid(float fx, int nx, float dx, float fz, int nz, float dz, float *v);
//	BOOL GetVelTime(float * vt, double range, int it0, int nt);
	void colBarPos (double v, CPoint p, CPoint p1);
	void BackgrdFill(CDC* pDC, int n, double x[], double y[], double range, double offFill);
	void BackgrdFillArr(CDC* pDC, int nx, int nt, float **xArr, float **yArr, double *rangeArr);
	void floodFillArr(CDC* pDC, int nx, int nt, float **xArr, float **yArr, double *rangeArr);
	void floodFillArr(CDC* pDC, int nx, int nt, float **xArr, float **yArr, double *rangeArr, double offFill);
	//void floodFillArr2(CDC* pDC, int nx, int nt, float **xArr, float **yArr, double *rangeArr, double dt);

	//int getCountPolyPoint(int *lpPts,  int np);
	void pltFloodFill(CDC* pDC, CPoint *pPoly, int *lpPts,  int np);
	// void floodFillArr4(CDC* pDC, int nx, int nt, float **xArr, float *rangeArr, float ft,  float dt);
	BOOL floodFillArr5(CDC* pDC, int nx, int nt, float **xArr, float *rangeArr, 
								double xmin,  double xmax, double tmin,  double tmax, 
								COLORREF *colArr);

	BOOL floodFillArr5Multi(CDC* pDC, int nx, int nt, float **xArr, float *rangeArr, double xmin,  double xmax, double tmin,  double tmax, int actualMBPerPlotPanel=100);
	//BOOL floodFillArr24(CDC* pDC, int nx, int nt, float **xArr, float *rangeArr, double xmin,  double xmax, double tmin,  double tmax);

	BOOL wiggleArr(CDC* pDC, int nx, int nt, float **xArr, float *rangeArr, float *yArr);

	//void contourAmpFill(CDC* pDC, int nx, segydynamic tr[], double dist);
	void airvar(CDC* pDC, int n, double x[], double y[], double range);
	void airvarSmall(CDC* pDC, int n, double x[], double y[], double range);
	void airvarSmallNeg(CDC* pDC, int n, double x[], double y[], double range);
	void airvarD(CDC* pDC, int n, double x[], double y[], double range);
	void airvarDSmall(CDC* pDC, int n, double x[], double y[], double range);
	void airCol(CDC* pDC, int n, double x[], double y[], double range, double fillOff); //color fill once.
	void airColD(CDC* pDC, int n, double x[], double y[], double range, double fillOff); 
	void airCol1Small(CDC* pDC, int n, double x[], double y[], double range, double fillOff2); //color fill many.
	//void airColorSmall2011(CDC* pDC, int n, double x[], double y[], double range); //bi-color fill many.
	void airColorSmallArr(CDC* pDC, int nx, int nt, float **xArr, float *rangeArr, float ymin, float dt, double offFill);
//	void airColorSmallArr(CDC* pDC, int nx, int nt, float **xArr, float **yArr, float *rangeArr, COLORREF colPos, COLORREF colNeg, double offFill);
	void airColorSmallArr(CDC* pDC, int nx, int nt, float **xArr, float *rangeArr, float ft, float dt, COLORREF colPos, COLORREF colNeg, double offFill);
	//void airColorSmallArr2(CDC* pDC, int nx, int nt, double **xArr, double **yArr, double *rangeArr, COLORREF colPos, COLORREF colNeg, double offFill);
	void airColorBigArr(CDC* pDC, int nx, int nt, float **xArr, float *rangeArr, float ymin, float dt, double offFill);
//	void airColorBigArr(CDC* pDC, int nx, int nt, float **xArr, float **yArr, float *rangeArr, COLORREF colPos, COLORREF colNeg, double offFill);
	void airColorBigArr(CDC* pDC, int nx, int nt, float **xArr, float *rangeArr,  float ft, float dt, COLORREF colPos, COLORREF colNeg, double offFill);
	void airColD1Small(CDC* pDC, int n, double x[], double y[], double range, double fillOff);
	void bitmapFill(CDC* pDC, int n, double amp[], double y[], double range, int lastTrcPixelPlotted) ;
	void ClipModelDepreciated(float cmMin, float cmMax);
	int myClipPoints(int n, float x[], float z[], float x1[], float z1[]);
	// int myClipPoints2(int n, double x[], double z[]);

CSeisWideDoc();
	DECLARE_DYNCREATE(CSeisWideDoc)

// Attributes
public:

	BOOL InitializeSec();
//	pCartesion convLatlon2XY(double alon, double alat);
	void DrawControls(CDC *pDC);
	void DrawMarkers(CDC* pDC);

	BOOL MyInitAmp2(CString t);  //Initialize segy file named "t".
	BOOL myParas();
	void DrawAxes(CDC* pDC, CRect rect);
	void WipeDrawingsAll(CDC* pDC, CRect rect);

	void Tick(CDC* pDC);
	CSeisWideSrvrItem* GetEmbeddedItem()
		{ return (CSeisWideSrvrItem*)COleServerDoc::GetEmbeddedItem(); }
	BOOL isVisibleOrNot(double x, double y);
	BOOL MyDlgBound(double xOld, double yOld, double xNew, double yNew);

	// Note a dilemma: the VELMOD struct defines 2D arrays with indeces in a revease
	// way relative to a proper convention: e.g. xm[point][layer] , which does not
	// make it possible to simply copy memory blocks using &xm[layer][0] et al.
	// In the future one may reverse this
	struct VELMOD{
		int ncont;
		// int nlayer;  // I will need to disable this variaable. It is actually always ncont-1

		//int nzed[MOD_LAYER_MAX],nvel[MOD_POINTS_MAX][2];
		//float xm[MOD_POINTS_MAX][MOD_LAYER_MAX], zm[MOD_POINTS_MAX][MOD_LAYER_MAX], xvel[MOD_POINTS_MAX][MOD_LAYER_MAX][2],vf[MOD_POINTS_MAX][MOD_LAYER_MAX][2];
		//double depth[MOD_LAYER_MAX][500],vTop[MOD_LAYER_MAX][500],vBot[MOD_LAYER_MAX][500]; //for vel interpolation.
		//double depthTwtt[MOD_LAYER_MAX][500]; // added for storing vel interpolation results in TWTT domain, calculated field, not stored in file 
		//float tm[MOD_POINTS_MAX][MOD_LAYER_MAX];  // model control points in TWTT domain, calculated field, not stored in file

		// now this struct has been made with dynamic arrays, while the array names and dimensions are still the same.
		int *nzed, **nvel;
		float **xm, **zm, ***xvel,***vf;
		float **tm;  // model control points in TWTT domain, calculated field, not stored in file

		CString velSecName;
		// CString velSecOld; // we do not need velSecOld if we want velSecTimeLoaded
		CTime velSecTimeLoaded;
		// bool isTimeDepthSynced;
		int totCurve, *npArray; // num of points/curves in the floating reflectors
		float **xArray, **tArray;    // data for the floating reflectors.
		float *anisotropy;

		CString floatName, floatNameOld; //name of file for floating reflector in digitize format.
		CFileStatus floatReflStatus;
		BOOL isTraceAuto; // indicate if it will auto trace. Not saved

		// note: velMod.xmin and velMod.xmax can be different from para.xmin and para.xmax!
		// if model is smaller the display window, xminDisplay is diff from parax.min.
		// otherwise, xminDisplay,xmaxDisplay should be the same as  para.xmin and para.xmax
		// So everytime we display a model, we have re-calc the xminDisplay,xmaxDisplay, **depth,**vTop,**vBot, **depthTwtt
		double xmin,xmax;  // added for easier modeling. Not saved to .sei file.
		double zmin,zmax;  // added for easier modeling. Not saved to .sei file.
		double xminDisplay,xmaxDisplay;  // added for easier modeling. Not saved to .sei file. 
		int nxDisplay; //for vel interpolation.
		bool isDisplayInitialised; // model depth[][],depthTwtt[][] is not meshed even though the arrays are initialized
		double **depth,**vTop,**vBot; //for vel interpolation/color gridding. not saved
		double **depthTwtt; // added for storing vel interpolation/gridding results in TWTT domain, calculated field, not stored in file 

		VELMOD(): nxDisplay(0),ncont(0),isDisplayInitialised(false) {};
	} velMod;

	BOOL MyInitVel(VELMOD* velm, CString aVelName, double dz);
	BOOL MyInitVelGen();
	BOOL MyMouseClick(double xOld, double yOld, double xNew, double yNew);
	BOOL MyMouseClickDigitizing(double xOld, double yOld, double xNew, double yNew);
	BOOL MyMouseClickMode9(VELMOD *velm, double xOld, double yOld, double xNew, double yNew);
	CPoint mm_coord(double x, double y, int xLen, int yLen);
	CPoint mm_coord(double x, double y);
	int mm_coord(float x);
	int mm_coordy(float z);
	CPoint XY2pixel(double x, double y);
	double pixel2X(CPoint p);
	double pixel2Y(CPoint p);

	BOOL DrawTimeArr(CDC* pDC);
	BOOL DrawSectionArr(CDC* pDC, CString fSection, double ymin, double ymax);
	BOOL DrawSectionTrcArr(CDC* pDC, CString fSection, int trace1, int trace2);

	BOOL DrawColorbar(CDC* pDC);
	static CSeisWideDoc * GetDoc();
      
// Operations
public:
	// These are global variables for a specific document (or window)
	struct PARA{
		double gain, clip;  // these are valid for fill area or wiggle only. Not used in floodfill functions
		//double trcInt; // usually is the average trace interval in km
		double fillOff;  //fillOff is the shift in km for fill area usages.
		//double ratioDisplayWindow; // this is introduced for the user to resize the display window. The final trcIntOavgAmp
			// will multiple by  para.ratioDisplayWindow * pDC->getWindowExt.cx . This value is not saved to .sei file
			// but will be calculated in  myInitAmp() routines
		double tmin,tmax, tTick;
		double vred; // since this is saved in .SEI file, do not change it to float type, otherwise the .SEI file cannot be read properly!
		// double rvVred; // rvVred is usually dynamically calculated from 1./vred
		double xmin, xmax, xTick;
		BOOL isDistanceDisplay2; // isDistanceDisplay==1 means displaying in distance; isDistanceDisplay==0 means in offset.
		BOOL isDistanceLeftOfShot; // this controls how distance is calculated: if TRUE, Dist=xShot+offset. If FALSE, Dist=xShot-offset.
			// in the latter case, if display is in offset mode, once need this to properly display computed travel time curves

		// the followings are for controling the schroll buttons. They are calculated when initializing a section. Not saved
		// the rule is: if xminData==xmaxData, the schroll buttons cannot be determined.... not implemented
		//double xminData, xmaxData;
		//double tminData, tmaxData;
		//double zminData, zmaxData;

		int wiggle;  
		// para.wiggle==0=pure wiggle
		//				1=fill
		//				2=wiggle+fill
		//				3=bi-color fill
		//				4=Color fill
		//				5=color fill+wiggle
		//				6=contour
		//				7=contour color fill
		int isFaceUp; // 0=face down; 1=face up. Should change it to BOOL
		BOOL isNegativeFill; // for area fill only
		int trace1, trace2;
		// int traceStep;  // this does not work well. So need to change to another one
		BOOL traveltime;
		BOOL gainDist;
	//	BOOL sphericGain;	//sphericGain is spher. correction. Cannot be saved, since it only functions inside a routine only
		double baseGain;	//base gain used for time section plotting only.
		double basePower;

		 //for NMO display mostly, so that can be compared to reflection profile
		float obsDepth;
		float waterDepth; 
		float gunDepth2;  // used in raytracing() and getReductionVelocity function in NMO display
		float waterVelocityAboveObs; // used in SB offset calc

		float sphericOrder;	//sphericOrder is spherical correction order.
		float	attenuationCorr;
		float	attenuationSeafloor;

		// double lat1, lat2, long1, long2; 
		int tstart;  //temporary for DrawTimeTrc() only.
		BOOL overWrite; //temporary for DrawTimeTrc() only.
		PARA(): trace1(0), trace2(99999), obsDepth(0), waterDepth(0), gunDepth2(0), 
			waterVelocityAboveObs(1.441f),sphericOrder(0),attenuationCorr(0) {};
	} para;

	// VEL struct is for specifying parameters for plotting velocities that are defined by VELMOD struct
	struct VEL{
		double dv;  // note: this is used for plotting only, not to use it for data processing such as RMS velocity analysis
		double dxV;
		double zmin, zmax, zTick, dz;
		// double dt;  // for time-depth conversions and vice versa. If zero, we will use vel.dz instead. Not used yet.  If I enable thie line, Seiswide crash. Why???
		// double tmin, tmax, tTick; // tmin, tmax, tTick is for rms vel analysis, not initialized
		// double dv;  // vel.dv is used for rms velocity analysis only, not initialized
		double vmin, vmid, vmax;
		double cxExt, cxPos, cyExt, cyPos, vmid2, vmid3, vmid4;
		int vminR, vminG, vminB, vmidR, vmidG, vmidB, vmaxR, vmaxG, vmaxB,
			vmidR2, vmidR3, vmidR4, vmidG2, vmidG3, vmidG4, vmidB2, vmidB3, vmidB4;
		double dv1;

		// additional 3 colors are needed, so we add here; Sept., 2011
		double vmid5, vmid6, vmid7, vmid8;
		int vmidR5, vmidG5, vmidB5;
		int vmidR6, vmidG6, vmidB6;
		int vmidR7, vmidG7, vmidB7;
		int vmidR8, vmidG8, vmidB8;

		float colBarVmin;
		float colBarVmax;
		VEL(): colBarVmin(0),colBarVmax(8) {};
	} vel;

	struct TRAVELTIME{
		int num[TX_CURVES_MAX][2], tot[2];	//num: number of points/curve for each side of the OBS. 
									//tot: no. of curves.
		float xShot;   // Profile source x-pos 
		// float gunDepth;  // raytracing can only to the airgun depth. Usually 0.011f
		float x[TX_POINTS_MAX][TX_CURVES_MAX][2],t[TX_POINTS_MAX][TX_CURVES_MAX][2];

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
		float shots[100][2]; // this sometimes store all the shot coordinates
		int ivray[100];
		int rayCodeShow;  // -1=Show all rays, otherwise 0-20 (show only if a ray code equals rayCodeShow). For ray codes>20, will first deduct  (int)(irrCol/20) * 20
		float rayCodeIn[100];
		int rayCodePerCurve[TX_CURVES_MAX][2]; // ray codes read in together with each ray in computed file ray path
			//note that, unfortunately, tx.x[] is always model distance in raytracing, but 
			// can be offset in digitizing if para.isDistanceDisplay==0.

		CStringArray *rayCodes;		// ray codes read in from the parameter file only	; this will be removed soon	
		int colour[100];		// colour codes read in from the parameter file only	
		int ncolour;
		int circleCurve; // the curve number that is to display as curve; shortcut: Control+L
	//	CStringArray *sarrayIndex;
	//	CStringArray *sarrayColor;	
		TRAVELTIME(): nShot(0),rayCodeShow(-1) {};
	} tx;

	struct COORD{
		CRect deviceAxes;  //device coords.
		double axisX1,axisY1,axisX2,axisY2; // client coord ; These are limits zoomed up by user.
			// axisX1=para.xmin unless the user is zooming
		// double zmin,zmax;  // client coord ;  // should remove them???

		CRect logicalAxes; 
		//int mm_xLen,mm_yLen;	//in logical units; width of plotting area, excluding anything outside of X-Y axes.
		//int mm_xmin, mm_ymin; //logical coords; minimum x,y for data plotting
		double mm_xscale, mm_yscale; //A constant for use in mm_coord(), set in OnDraw() to speed calculation.
			// converts from client (km, sec)  to  logical cooords for plotting
		double xValuePerPixel; // only for speed up and easier calculations


		CPoint m_point; //mouse click point.
		double xMouse, yMouse;	//mouse click point in km.
		// //only for spacial mapping in OnModelingExtractlayerinfor() .
	} coord;


	// this struct is used for area fill, wiggle, and bi-color area fill only
	struct AREAFILL {
		double clip_real;
		double fillOff;
		double gain_real;
	} areaFill;

	/*  This is useful for converting 2 bytes to 4 bytes, and vice verce. Better to move it under head struct!
	*/
	union TWO_FOUR{
		short hw[2];
		int iv;
	} ssmic;

	/*  This HEAD struct is useful for the currently active SEGY file. These numbers are
		saved as global so that they can be used many times without having to read them
		again and again.
	*/
	struct HEAD{
		unsigned char ascHead[3200];
		// double rv;  // this is dropped
		//int numsamp;
		//int len;	//trace length in no. of samples.
		//int tpp;	//sampling rate in micro-sec.
		//UINT format;  //data format. Its value can be different from that in bhed for convenience of work

		// convenient to use although this bhed struct can duplicate with the above info
		//bhed bh;  // NOTE: ####bh.format will be overwritten for output header ####
		//int ntrCached;             // number of traces cached in *trDyn
		//segydynamic *trDyn;  // this is a cache for speeding up loading segy data

		// optional 
		//int trcIndexOld; // the previously selected index for trace 
		//double dtOld; // the previously used changes in time, in sec
		//float offset[1500]; // for interactive moving around of traces in T-X offset domain
		//ULONGLONG offsetBytes[1500];
		//int offsetNum;

 		// for extracting extra meta-data for the segy...added aug. 26, 2010
		int numTraces;
		float tminFile;  // useful info
		float *dist;
		float *offsetArr;
		int *cmp;

	} head;


	struct RAYPATHS{
		CString  rayFileName;
		CTime rayFileModifiedTime;
		int totCurve;
		int npts_max;
		int *npts;
		int *irrcol;
		float **xr, **zr;
		RAYPATHS(): totCurve(0), npts_max(300) {}; // totCurve>0 is an indicator that it must be free-ed later.
	} raypaths;

	RAYPATHS raypathsTwtt;

	struct DIGITIZE{
		BOOL isZeltFormat2;  // if isZeltFormat==0, it's line-drawings digitized; otherwise, it's Zelt's format.
		int iCurveSel;
		int totCurve;
		CString  digName;
		CString strComment; // Only 1 comment for each OBS 
		BOOL noPromptLabel;  // not prompt for a new dialogbox for inputting line labels
		BOOL isSorted;
		BOOL isSortedY;

		// if array sizes are now dynamic, we should remove these limits? To be done.
		int DIG_POINT_MAX, DIG_LAYER_MAX;

		int *nPoint;
		int *ivray;
		float **x, **t, *digResolution;
		COLORREF *colArr;  // each line should be assigned by a fixed color. To be implemented!

		CStringArray *labelArr; // 
		BOOL isLabelShow;
		short barForm; // control how a error bar should be displayed
			// e.g.  a star, a bar, a circle et al.
	//	BOOL isModified;
		double twin_min; // tempoary
		double twin_max; // tempoary
		double twin_lag; // tempoary
		float  twin_lagWeight; //tempoary
		BOOL   twin_isEnergy; // tempoary
		int  twin_numSubdivide; // tempoary
		DIGITIZE(): totCurve(0),isLabelShow(TRUE),colArr(NULL) {}; // totCurve>0 is an indicator that it must be free-ed later.
	} dig;

	struct DRAW{
		int mode;	  //0: time section by km; 1: depth section; 2: velocity color map; 
		// 3: both depth section and vel color; 4: velocity contours. 5: no draw.
		// 6: time section by trace; 
		// 7: OnViewHydro; ID_MENU_VIEW_HYDRO  hydro time display. 
		// 8: Disabled; OnViewStackingvelocitymap; ID_VIEW_STACKINGVELOCITYMAP; stacking Velocity contours will be drawn.
		// 9: OnViewVelocitycolortwtt; ID_VIEW_VELOCITYCOLORTWTT; Velocity contours will be drawn in TWTT domain.
		// 10:OnViewNmoVelocity  ID_VIEW_NMO_VELOCITY; RMS velocity display
		// 11:RMS velocity display (to be abandoned???)
		// int modeTemp;	//only used before showing myParas() to determine what needs to be shown.

		// another interesting mode: -1: prompt for file to open;  -2: do not propmt for file to open.
		int modeOld; // this is added for deciding the previous mode: if a user change a view mode, it will differ draw.mode. But once a view is displayed, it will have to equal draw.modeOld = draw.mode
		int iLayerFocus;  //Layer number that gets the focus, Get by View::OnRButtonUp().
		int iTextFocus;  //text number that gets the focus, Get by View::OnRButtonUp().
		//int iTopBound;	//Editting top boundary of a layer. (1: top; 0: bottom)
		bool isTopBound;

		int nodes;		  //Mode for editting boundary and velocity nodes (0,1,2,3).
							 //if 1 then boundary mode; if 2 then vel mode; 3=text mode

		int pick;		//Pick boundary or velocity layer.
		int remove;		//Remove a node point.
		int move;		//Move node points.
		int add;		//Add a node point.
		int pickOld;    //for right-mouse flipping; 0="pick", 1="remove", 2="move", 3="add". We backup mode using ptdraw().
		int velFormat;	//Velocity format. 0=seiswide binary, 1=zelt's ascii.
		//BOOL mapChanged, timeSecChanged, velSecChanged, depthSecChanged;
		int clipBoardSizeX, clipBoardSizeY; //in mm.
		// float thicknessLines; // default=1; 2=double thickness; 0.5=half thick
		int progressBarX1, progressBarX2;
		int  traceSkippedInDisplay; 
		int  tracePlottedInDisplay;
		int TTCurveColor;  	

		float printX, printY;	//compression ratios for X and Y extent, with respect to full-page size.
		int memMaxMB;  // used in printing large bitmaps
		double aveAmp;	//average amp; 
		double maxAmp;
		double trcInterval; // only for km display mode; for trace display mode, draw.trcInt=1

			// 0 for black, 1 for red TT curves; 2 for white, 3 for blue, 4 for random colors.
	//	CString bitmapName; //name of bitmap to load for display.
	//	CFileStatus velFileStatus;  //status of vel file at the time of last initializing. Not really used???

		int bigNodes; // 1,2,3     with 3 being the biggest node
		int raymode; // 0=P wave; 1,2=S wave
		CString timeSecName;
		CString depthSecName;
		CString plotTitle;
		CString velSecNamesEx;
	//	CString colorPaletteName;  // this is different from the colorbar for a velocity display!
	//	int numContours; // only used in segy contour display

		CString timeSecOld, depthSecOld;
		CString rayName; //rayName will be copied to "r.in", for a separate fortran program "myrayinvr.exe".
		// note: if timeSecName = timeSecOld, no need to initize this time section timeSecName.
		//		Same is true for velSecOld and depthOld.
		CString vrmsTauName; // V-Tau curve will dynamically be converted to interval velocity and overlain onto display
		CString seiFileName; //  not saved, but will update when opening the sei file... added for convenience of programming

		CString strXLabelOverwrite; // if blank, it will draw as "Distance", 
				// if not blank, this value will be used instead of default.
				// It is dynamically assigned in velocity analysys e.g.

		CString markerName; //name of file for markers (e.g. OBS) in free format (X-Y).
		CTime markerModifiedTime;
		DIGITIZE markerDigStruct; // will need to free it in destruct. Will load only if mod time changed

		float markerXMultFactor;  // to multiply to the x-axis
		float markerXShift;  // shift the x-axis by this km
		float markerYMultFactor;  // to multiply to the y-axis
		float markerYShift;  // shift the y-axis by this km
		BOOL  markerIsRevYAxis;  // check to reverse the Y-axis for line-drawings and markers. This is useful for plotting gravity values
		CString txRaytraceName;  // name for file for tx.in format; I cannot use dig.xxx name since it is used differently

		// BOOL traceStepPromptOld; //for backup during printing and bitmap generation.

		// CString headInfo;	//head info is the text (trace, FFID et al.) to display for each trace.

		int restoreLevelCurrent; // not saved. initialize with -1. If -1, restore the most recent velocity backup (say level 7), and this number will be 7.
			// the next time of restore (Cntl+Z), it will restore level 6, and the level index will be reduced to 6. 

		int restoreLevelMax;  // later will move to more global variable
		// int pixelThreshold;   // normally 1, later will move to more global variable
		int iModelPeglet; // toggle peglet display when showing velocity models
		bool isShowTheoryModel;


		float modelUpInc; // def. 0.02km, not saved
		float modelVelInc; // def. 0.02km/s, not saved
		float dxdyDrawn;  // user defined a valued on screen

		BOOL progressBar;
		BOOL traceStepPrompt; //if true, do not prompt if traces are too dense.depreciated.
		
		BOOL colBar;


		BOOL velErr;
		BOOL isFillSmall;
		BOOL fillColor;
		// int fontSize; // 1=regular (reg). 2=10% bigger; the final font size would be  *reg*10%

		BOOL isDigitizing; // if true, onscreen digitizing is going on.
		//BOOL extLineDrawing,extLineDrawingTim;	//Not saved in version 2. draw.velErr is disabled and not used.
			// note: if digitize=TRUE, extLineDrawing and extLineDrawingTim must be FALSE.
		// BOOL labelAsTWTT; //if true, time label is TWTT, only affect display.
		BOOL notRedoTT; //if TRUE, TT curves loaded correctly; If FALSE, prompt the user in OnDraw(), I tried to do this in DrawTime() but fails many times.

		// BOOL isDisplayStart;  // a para for telling OnSize() in ChildFrm.cpp  that this is just opened and no need to re-calc the data for window re-sizing!
		BOOL isSameNodeDisplay; // this affects nodes display only, not affecting segy or model displays.
			// note: if a model is displayed (draw.mode==3, 9 et al.) are exceptions since will always has re-draw the velocity model nodes. So 

		BOOL isSameDataDisplay; //if TRUE then use the old mDC without reloading the segy or even model data.
		// Note: isSameDataDisplay is used for dealing with seismic amplitudes: if true, Seiswide will not
		//			load the seismic section again to save time.
		//		isSameNodeDisplay is useful for onscreen digitizing: if false, clicking will not refresh 
		//		anything already onscreen, only a new point is added onscreen. this save screen refreshing
		// 	Prefer to use  draw.isSameDataDisplay==FALSE,   instead of setting to blank for any one of:  CString timeSecOld, velSecOld, depthSecOld
		//  But setting draw.timeSecOld!=draw.timeSecName   would initialize the seismic section with amp initialization, while  draw.isSameDataDisplay==FALSE   would not initilize these data
		//  But setting velMod.velSecOld!=velMod.velSecName   would initialize the velocity bin file with color gridding initialization, while  draw.isSameDataDisplay==FALSE   would not initilize this


//		BOOL copyAsBitmap; //if TRUE then copy segy to clipboard as bitmap. Not working yet.

		BOOL isCopying; //isCopying is initilized in contructor.
		BOOL isNoNodes;
		BOOL isNoBoundary,isNoColorMap,isNoOBSPos;
		// BOOL isNoSeismic;  // if true, no seismic amplitudes are copied, only useful for copying to clipboard 
		BOOL isNoLableOutside;
		BOOL isNoLabelInSide;  // control if line labels should be plotted
		BOOL isNoOutSidePoints; // for excluding rays/lines in plots et al.
		BOOL isTerminate; // indicate the processing should stop asap. May not have been used at all
		BOOL isShiftPressed;
		BOOL isControlPressed;
		BOOL isAPressed;
		int isPrinting; // 0=not printing;  1=printing just started (coord mapping can change upon request); 2=printing on-going (coord mapping cannot be changed)
		BOOL isShowRaycode;

		BOOL isSegyKeyXShift; // indicate if the arrow keys should respond to segy shifting
		float segyKeyXShift;  // Shifting in km

		BOOL isAvgEnergy; // 0 by default

		float distShift;  // for display reflection data: shift distance without modifying trace header. Useful for multiple SBs on the same big reflection segy
		int numContours; // default 50. Used in contouring plotting only.
		
		CDC* gDC;  //kept in memory when pDoc->draw.isSameDataDisplay = FALSE, but rewrite when TRUE.
		BOOL gDC_isDrawable;
		float gDC_vred;  // this indicates the current gDC has this reduction velocity. 
		CRect gDC_rect; // a backup of gRect for later quick drawing without reloading data
		float gDC_tmin;
	//	float gDC_tmax;  // this can be calculated from gDC_tmin and gDC_rect

		DRAW(): markerXMultFactor(1.0),markerXShift(0),
			markerYMultFactor(1.0),markerYShift(0),markerIsRevYAxis(FALSE),numContours(50),
			isSegyKeyXShift(FALSE),isShowTheoryModel(false),isNoLableOutside(FALSE),isNoLabelInSide(FALSE), dxdyDrawn(0.f)  {};
	} draw;

	struct SEGYMETA{
		CString secName;
		CDC* aDC;  //kept in memory
		float aDC_vred;  // this indicates the current gDC has this reduction velocity. 
		CRect aDC_rect; // a backup of gRect for later quick drawing without reloading data
		float aDC_tmin;
	//	float aDC_tmax;  // this can be calculated from gDC_tmin and gDC_rect
		SEGYMETA(): aDC(NULL),aDC_vred(0.f),aDC_tmin(0.f) {};
	} gSegyMeta;


	struct SEISBICOLOR2{
		int	maxAmpR, maxAmpG, maxAmpB;
		int negAmpR, negAmpG, negAmpB;
		int zeroAmpR,zeroAmpG,zeroAmpB;
		float colClipRatio; //not used.
	} seisBiColor2;

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
	} kd;

	struct BUTTONS{
		BOOL m_pick, m_remove, m_move, m_add, m_duplicate, m_newLine, m_delLine;
	} bt;

	struct AMPCOLORDYN{
		CString colFileName;
		int num; // num of colors in the arrays. Always 11 as we use it. But the actual colors plotted are interpolated between these 11 colors and so can be many more.
		int *r;
		int *g;
		int *b;
		float *percentAmp;

		int logScaleThreshold;
		//int numContours; // default 50. Used in contouring plotting only.
		//COLORREF *colArr; // this one contains expanded color table, with numContours values
		//AMPCOLORDYN(): colArr(NULL){};
		AMPCOLORDYN(): num(11),logScaleThreshold(70) {};
	} ampColorDyn;

	struct LINECOLORS{
		// CString lineColFileName;
		static const int NUM_LINECOLORS  = 11; // num of colors in the arrays. Always 11 as we use it. 
		int r[NUM_LINECOLORS];
		int g[NUM_LINECOLORS];
		int b[NUM_LINECOLORS];
	} lineColors;


	struct BMP24{
		//A bitmap of the same name can be either in time or depth domain.
		//Only one bitmap name is allowed for 1 document.
		CDC bmpDC; // must be freed in destructor: will not auto done in win95.  ***Depreciated***
		CDib dib;  // added on Sept, 11, 2011. recommended to use as opposed to bmpDC
			//  dib.m_pBIH->biWidth  will store pixel width
			//  dib.m_pBIH->biHeight  will store pixel height
			// (bmp24.xmin, bmp24.tmin)  will correspond to  pixel zero
			// (bmp24.xmax, bmp24.tmax)  will correspond to  another pixel corner
		BOOL isNeedInitialize; //if TRUE then need initialization; 
		CString bmp24Name;
		BOOL isDepth;   //FALSE: this bmp is a time section, TRUE: a depth section or a model.

		float xmin, ymin, xmax, ymax; 
			// correspond to the upper left and lower right corner. Their values can be tricky to obtain
			// DIB/BMP always goes downward/face down, but in plotting, it can either face up or face down.

		float vred;  // this keeps track of what vred the BMP is. One can change the reducing velocity based on this original vred. Not used yet.
		int biForm; // can be 24, 32, or 1
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
		} filterxt;
	} processing;



	CSize sizeDoc;
 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeisWideDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	protected:
	virtual COleServerItem* OnGetEmbeddedItem();
	//}}AFX_VIRTUAL

// Implementation
public:
	afx_msg void OnViewTimesectrc();
	void myClipPoints4(DIGITIZE *dig, double xmin, double xmax, double tmin, double tmax);
	void myClipPoints4(DIGITIZE *dig, COORD *coord2);

	void myShowFloatReflectors(CDC *pDC);
	BOOL myDigSaveAs(DIGITIZE *dig2, CString aName, BOOL isSort, BOOL isSortY, CString strRemarkLine = _T(""));
	BOOL myDigSaveAsZelt(DIGITIZE *dig2, CString aName, BOOL isSort, BOOL isSortY, CString strRemarkLine = _T(""));
	BOOL myDigSaveAsWithZplot(DIGITIZE *dig2, CString aName, BOOL isSort, BOOL isSortY, CString strRemarkLine = _T(""));  // this is left used, although the code is good
	BOOL myDigSaveAs(CString aName, CString aNameOrig);
	BOOL myDigSaveAs(CString aName);
	CString myDigSaveAsPlainTrc(DIGITIZE *dig2, CString aName, CString aNameOrig, BOOL isSort, BOOL isSortY);
	void myDigSort(DIGITIZE *dig2, BOOL isSortX = TRUE, BOOL isUpdateXPartial = FALSE);
	void myDigRemove(DIGITIZE *dig2, int ip, int ic); // note: this changes dig.nPoint[ic]
	void myDigRemoveDup(DIGITIZE *dig2, int ic); // note: this changes dig.nPoint[ic]
	float getDigXfromT(DIGITIZE *dig2, float t);

	float getDigYMin(DIGITIZE *dig2);
	float getDigYMax(DIGITIZE *dig2);

	double getFreeSpace(CString f);

	COLORREF getRayColor(int irayCol);
	COLORREF getVelocityColor(float v, VEL vel);
	// BOOL myCreateTempDC(CDC *pDCfrom, CDC *pDCto, CRect rectPixel, CRect rectClient);
	BOOL myCreateTempDC(CDC *pDCfrom, CDC *pDCto, CRect rectPixel, CRect rectClient);
	void DrawControlsEx(CDC *pDC);
	int ptdraw(int pick, int remove, int move, int add);

	BOOL showDigLineDraw(CDC *pDC);
	BOOL myShowTravelTimeInGDC(CDC *pDC, short isColorForced); // this enforces a random color for TT curves with 0 thickness
	BOOL myShowDigitizeWideAngle(CDC* pDC); // this draws TT curves with random color with thicker normal lines
	BOOL myShowTravelTime(CDC *pDC, short isColorForced, CPen *aPen); // this enforces a color for the TT curves with 0 thickness

	BOOL LoadBitmapInDC(CDC *pDC);
	void DrawBitmap(CDC *pDC);
	// void DrawBitmap(CDC * pDC, CRect rectLogicalAxes);
	void DrawRayPathDepreciated(CDC * pDC);
	void DrawRayPath(CDC* pDC);
	BOOL checkModelError(CString name);
	void mySetOBSPosition( float xshot);
	//BOOL DrawHydroTime(CDC* pDC);
	//BOOL DrawTimeTrc(CDC* pDC);
	BOOL MyInitAmpTrc2(CString t);
	BOOL CheckTime(long daySet);
	BOOL isTimeDomain();
	BOOL isModelDisplayed();
	float getXPosAll();
	BOOL myLoadTravelTime(CString txName);
	virtual ~CSeisWideDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
//	short i2buf[40000];

// Generated message map functions
protected:
	COLORREF getAmpRgbSimple(int fullAmpR, int fullAmpG, int fullAmpB);
	COLORREF getAmpRgb(CPoint *pint, int numfa, int xFull, int xZero);
	COLORREF getAmpRgb(float amp);
	bool getExpandedColorTableDepreciated(COLORREF *colArr, float *percentAmpExpanded, int numExpanded);
	int getColorIndex4AmpDepreciated(float *amp, float *percentAmpExpanded);
	COLORREF getColorRef4Amp(double amp, int numColors, float *percentAmp, COLORREF *colArr);

	COLORREF getAmpRgbPixel(float amp, float amin, float amax);
	COLORREF getAmpRgbPixel(float amp);
	int getAmpRgbIndex(COLORREF col);

	float getFirstXPosFromRayParam(CString f);
	BOOL myModelRestore(BOOL isBackward = TRUE);


	BOOL myChangeDrive(CString *driveNew);

	BOOL initAmpColorArray(int num);
	void freeAmpColorArray();
	BOOL getAmpColorArray(CString colFileName, double maxAmp);
	BOOL getLineColorArray();
	BOOL getAmpColorArray();
	float getDepthAtSeafloor(float dist);
	//void saveAmpColorToFile(CString colFileName);

	void freeModelArrayEx();

	void segyHeadFree();

	void velModFreeArrays(VELMOD *velm);
	void velModFreeArrays();
	void velModSetArrays(VELMOD *velMod2);
	bool velModCloneArrays(VELMOD *velMod, VELMOD *velModTo);
	void velModSetArrays();
	double velModUpdAnisotropy(double v, double anisotropy);
	int isPointAbove(VELMOD *velm, int icont, SEI_POINT pNew);

	int getClosestLayerNum(VELMOD *velm, SEI_POINT pNew);
	int getClosestDigNum(DIGITIZE *digm, SEI_POINT pNew);

	bool velModInsPoint(VELMOD *velm, int icont, int jIns, SEI_POINT pNew);
	bool velModDelPoint(VELMOD *velm, int icont, int jDel);
	bool velModVelDelPoint(VELMOD *velm, int icont, int jDel, bool isTop);
	bool velModVelInsPoint(VELMOD *velm, int icont, int jIns, bool isTop, float x, float v);

	bool velModInterceptPoint(VELMOD *velm, int icont, int jMove, SEI_POINT pNew);
	bool velModInterceptPointMode9(VELMOD *velm, int icont, int jMove, SEI_POINT pNew);

	void DrawAxesTicksValues(CDC* pDC, double yminExact, double ymaxExact, double yminNice, double yTickNice, int fontWidth, int fontHeight);


	BOOL KdmigGeneral(FILE* jpfp, CFile* file2, BOOL useExist, BOOL preStack, BOOL appendOutput);
	float myTime2Depth(float x, float  t, bool isTime2Depth=true);

	void myLineDrawing(CDC *pDC);
	void myLineDrawingTim(CDC *pDC);
	BOOL myLoadFloatReflectors();

	int totDays(int nYear,int nDay,CString t);
	void myVelModWholeInterpol();
	void myVelModInterpolDisplay();
	void myVelModInterpolDisplay(VELMOD *velm, double xmin, double xmax, double xShift);
	void extraSerialize();
	void ClipModeXmin(float cmMin);
	void ClipModeXmax(float cmMin);
	BOOL reduction2TrcHeader(float vred);
	//{{AFX_MSG(CSeisWideDoc)
	afx_msg void OnViewBoth();
	afx_msg void OnViewContours();
	afx_msg void OnViewTimesection();
	afx_msg void OnViewVelocitycolormap();
	afx_msg void OnViewHydro();
	afx_msg void OnViewDepthsection();
	afx_msg void OnProcessingAddrangetosegy();
	afx_msg void OnEditBoundary();
	afx_msg void OnEditVelocity();
	afx_msg void OnEditNonodes();
	afx_msg void OnToobarMove();
	afx_msg void OnToolbarRemove();
	afx_msg void OnToolbarPick();
	afx_msg void OnToolbarAdd();
	afx_msg void OnToolbarDelLine();
	//afx_msg void OnToolbarNewLine();
	afx_msg void OnEditCopy();
	afx_msg void OnToolbarSavev();
	afx_msg void OnToolbarSaveasv();
	afx_msg void OnDigitizingStart();
	afx_msg void OnDigitizingSavedig();
	afx_msg void OnDigitizingNewcurve();
	afx_msg void OnDigitizeFinishdigitizing();
	afx_msg void OnProcessingTracereorder();
	afx_msg void OnProcessingCohmix();
	afx_msg void OnToolbarNewmod();
	afx_msg void OnProcessingReverserange();
	afx_msg void OnProcessingLoadzeltsmodel();
	afx_msg void OnProcessingDatareductionandfilter();
	afx_msg void OnProcessingLateralshift();
	afx_msg void OnModelingRaytrace();
	afx_msg void OnModelingRaypath();
	afx_msg void OnViewSegyinfo();
	afx_msg void OnProcessingDefinesourceposition();
	afx_msg void OnEditNolables();
	afx_msg void OnEditTuneupNoboundary();
	afx_msg void OnEditTuneupNocolormap();
	afx_msg void OnEditTuneupObspositions();
	afx_msg void OnToolbarDuplicate();
	afx_msg void OnProcessingCheckmodelintegrity();
	afx_msg void OnToolbarResizemod();
	afx_msg void OnEditDisplaytuneupFillareaonce();
	afx_msg void OnDigitizeUndopoint();
	afx_msg void OnProcessingAppendasegyfile();
	afx_msg void OnProcessingReducemanual();
	afx_msg void OnViewSegypreview();
	afx_msg void OnModelingExtractvzcurve();
	afx_msg void OnProcessingMigps();
	afx_msg void OnProcessingKdmigpost();
	afx_msg void OnProcessingKdmigsimple();
	afx_msg void OnModelingExtractlayerinfor();
	afx_msg void OnToolbarRefresh();
	afx_msg void OnModelingLoadreidsmodel();
	afx_msg void OnProcessingLatlongtodistance();
	afx_msg void OnEditDisplaytuneupBigmodelnodes();
	afx_msg void OnEditDisplaytuneupChangcolorttcurves();
	afx_msg void OnProcessingUnix2pcbatch();
	afx_msg void OnProcessingPc2unixbatch();
	afx_msg void OnProcessingRawtosegy();
	afx_msg void OnViewTraceanalysis();
	afx_msg void OnDigitizingLoadexisting();
	afx_msg void OnEditDisplaytuneupExternalDrawings();
	afx_msg void OnProcessingTimeDepthDrawings();
	afx_msg void OnProcessingDumpdistance();
	afx_msg void OnModelingDenblocks();
	afx_msg void OnDigitizingMigratelinedrawings();
	afx_msg void OnDigitizingChangecurveres();
	afx_msg void OnDigitizingLoadzeltTx();
	afx_msg void OnDigitizingWhereani();
	afx_msg void OnDigitizingShiftdigiting();
	afx_msg void OnDigitizingChangeerrorbarAsBar();
	afx_msg void OnDigitizingChangeerrorbarAscircle();
	afx_msg void OnDigitizingChangeerrorbarAsstar();
	afx_msg void OnDigitizingChangeerrorbarAsfillcircle();
	afx_msg void OnEditDisplaytuneupRemoveoutsideraypoints();
	afx_msg void OnEditDisplaytuneupSkipcomplainingtracetoodense();
	afx_msg void OnToolbarFlipmod();
	afx_msg void OnModelingSubsidence();
	afx_msg void OnFileImportbitmap();
	afx_msg void OnFileResizebitmap();
	afx_msg void OnProcessingTdconvbmp();
	afx_msg void OnEditPaste();
	afx_msg void OnProcessingDataresample();
	afx_msg void OnEditDisplaytuneupRemovettoverlay();
	afx_msg void OnProcessingDebias();
	afx_msg void OnProcessingWiener();
	afx_msg void OnDigitizingResizeSegments();
	afx_msg void OnModelShiftxz();
	afx_msg void OnDigitizingSelectCurrent();
	afx_msg void OnDigitizingChangeerrorbarRedefinesource();
	afx_msg void OnDigitizingZeltsformatTogglezplot();
	afx_msg void OnEditUndo();
	afx_msg void OnProcessingChangereducevelocityforbmp();
	afx_msg void OnModelingMoveboundary();
	afx_msg void OnViewStackingvelocitymap();
	afx_msg void OnToolbarBiggerAmp();
	afx_msg void OnToolbarLessAmp();
	afx_msg void OnProcessingAppendmanysegyfiles();
	afx_msg void OnEditDisplaytuneupAdjustinglabelsizes();
	afx_msg void OnDigitizingSplit();
	afx_msg void OnDigitizingReviselinelabel();
	afx_msg void OnEditDisplaytuneupModelpropertiesLinedrawinglabel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void myLineDrawingLabelDepreciated(CDC * pDC);
	BOOL MyLoadDistTwttDrawings4John(CString inName);
	BOOL MyLoadDistTwttDrawings4John(CString sgyName, CString lstpcName, CString digName);
	int setShotsFromCoherency(CStringArray *sarray, int *filej, int *shotk, float *shotDist, int numShotIn, int numShotOut, int iwin1, int iwin2, int itauHalf, int itauInc, float freq1, float freq2, float distMaxPreferred);
	// Load xshot,zshot information from ray parameter file r.in
	int loadRayPara(void);
	void setXShotAll(void);

	// Get a color from a file for a given ray code. If not existing in the file, create the file. 
	COLORREF getRaycodeColor(int irrCol);
	COLORREF getRaycodeColor(int irrCol, BOOL isColorRecalc);
	COLORREF getRaycodeColor(float rayCode);

	bool myDigSaveWithFFID(CString aName, CString aNameOrig);
	// get reduced time in sec based on offset information
	// float getReduceTime(float xpos, bool isXposOffset);
	int formatVelocityArray2(float distInModel, double depth[], double vTop[], double vBot[]);
	// This will calculate a big list of contours as floating reflectors for raytracing, for NMO velocity analysis using semblence stack
	CString setContour4Raytrace(CString aFile, float xpos, float dz, float zmin, double *zmaxMod, float vminPerturb, float vmaxPerturb, float dvPerturb);
	CString setContour4Raytrace(CString aFile, float xpos, float dz, float zmin, double *zmaxMod, 
									  float vminPerturb, float vmaxPerturb, float dvPerturb, 
									  int *nArray, float *xArray, float *zArray );
public:
	afx_msg void OnDigitizingRaycodes();
	afx_msg void OnToolbarRefreshAll();
	afx_msg void OnDigitizingSavedigffid();
	afx_msg void OnViewVelocitycolortwtt();
	afx_msg void OnTimeForwide();

	void freeDigAll();
	void freeDigAll(DIGITIZE *dig2);

	void freeRaypathsAll();
	void freeRaypathsAll(RAYPATHS *raypaths2);
	BOOL initRaypathsArray(int npoint, int ncurve);
	BOOL initRaypathsArray(RAYPATHS *raypaths2, int npoint, int ncurve);
	BOOL loadRayPathFromFile(CString f, RAYPATHS *raypaths2, int npoint, int ncurve);
	BOOL loadRayPathFromDepth2TWTT(CString fDepth, CString fTwtt, RAYPATHS *raypaths3, int npoint, int ncurve);

	BOOL initDigitizeArray(int npoint, int ncurve);
	BOOL initDigitizeArray(DIGITIZE *dig2, int npoint, int ncurve);
	BOOL myDigitizingLoadzeltTx(DIGITIZE *dig2);
	BOOL myDigitizingLoadzeltTx();
	void myCaption(CString text);
	CString getWorkingPath();


	float getReduceTime(float offset);
	float getReduceTime(float offset, float vred);
	float getOffsetFromXVal(double x);

	float getVelocityMin(int icont);
	float getVelocityMax(int icont);
	//int formatVelocityArray(float dvUniform, int ncont, double depth[], double vTop[], double vBot[]);
	double getXp(double p, int nz, float dz, float vTrc[]);
	double setTp(double p, int nz, float dz, float vTrc[]);
	double getXp2(float p, float z, int ncont, double depth[], double vTop[], double vBot[]);
	double setTp2(float p, float z, int ncont, double depth[], double vTop[], double vBot[]);
	bool getXp3(float p, float z, int ncont, double depth[], double vTop[], double vBot[], double xt[]);


	BOOL DrawCont(CDC* pDC);

	BOOL DrawVel(CDC* pDC, BOOL isBoundOnly = FALSE);  
	BOOL DrawVelocityDepth(CDC* pDC, VELMOD *velm, BOOL isBoundOnly, BOOL isNoNodes, float xShift, CPen* greenPen = NULL);  
	void DrawVelocityLabelDepth(CDC* pDC, VELMOD *velm, float xpos, float xShift = 0.f);

	BOOL DrawStackVelocity(CDC* pDC);



	void DrawModelDashCurve(CDC* pDC, VELMOD *velm, int numWaterLayers, double h);
	void DrawVelocityTheoreticalDepth(CDC* pDC, VELMOD *velm);

	void DrawVelocityLabelTwtt(CDC* pDC, VELMOD *velm, float xpos, float xShift = 0.f);
	void DrawVelocityLabelTwtt(CDC* pDC, float xpos);
	bool DrawVelocityTwtt(CDC* pDC, VELMOD *velm, BOOL isNoNodes, CPen* greenPen = NULL, float xShift = 0.f);
	bool DrawVelocityTwtt(CDC* pDC);
	afx_msg void OnTimeForwideNmovelocity();

	void myModelBackup();
	BOOL myModel_Depth2TWTT(VELMOD *velm, double dz);
	BOOL myModel_TWTT2Depth(VELMOD *velm, double dt);
	BOOL myModel_TWTT_SaveAs(VELMOD *velm, CString f, BOOL isBackup);
	BOOL myModelSaveAs(CString f);
	BOOL myModelSaveAs(VELMOD *velm, CString f);
	BOOL myModelSaveAs(VELMOD *velm, CString f, double dz, BOOL isBackup);
	bool myModelsMerge(VELMOD *velMod, double shiftKm, VELMOD *velMod2, double shiftKm2, double vTolerance);

	int myModelBoundaryDup(VELMOD *velm, int icontFocus);

	// Get depth for a point at xpos for a given velocity in a given layer
	double getDepthFromVel(double dist, double vel, int icont, double percent);
	double getDepthFromVel(VELMOD *velm, double dist, double vel, int icont, double percent, double zinc = 0.0);

	//	bool getVZCurveAtDist(float dist, double depth[], double vTop[], double vBot[]);
	bool getVZCurveAtDist(VELMOD *velm, double dist, double depth[], double vTop[], double vBot[], bool isAnisotropyApply = true);
	bool getVZCurveAtDist(float dist, double depth[], double vTop[], double vBot[], bool isAnisotropyApply = true);

	bool getTZCurveAtDist(VELMOD *velm, double dist, double time[], double vTop[], double vBot[], bool isAnisotropyApply = true);
	bool getTZCurveAtDist(float dist, double time[], double vTop[], double vBot[], bool isAnisotropyApply = true);
	float getAvgVelAtDistRange(VELMOD *velm, double x1, double x2, int icont);

	afx_msg void OnViewNmoVelocity();
	BOOL myLoadDigFile(void);
	BOOL myLoadDigFile(CString aName);

	BOOL myLoadDigFile(CString aName, DIGITIZE *dig2);

	afx_msg void OnModelingMergemodel();
	afx_msg void OnModelingGeneratesyntheticsegydata();
	bool isVelModValid(VELMOD *velm);
	bool isVelModValid(void);
	bool isVelModShown(VELMOD *velm);
	afx_msg void OnModelingDosconsol();
	afx_msg void OnTimeTimesectionwithperturbedtravel();

	void close_unused_documents(const CDocument* const pNotThisOne);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void OnDisplaytuneOverwritexlabel();
	afx_msg void OnProcessingTimedepthConv();
	bool saveModelZelt4(CString newName);
	bool saveModelZelt3(CString newName);
	bool saveModelZelt2(CString newName);

	bool loadModelZelt4(CString zeltName);
	bool loadModelZelt3(CString zeltName);
	bool loadModelZelt2(CString zeltName);
	bool myOpenNewFile(CString fName);


	afx_msg void OnModelingDisplaycircle();
	afx_msg void OnModelingAddsineshape();
	afx_msg void OnProcessingDatadespike();
	afx_msg void OnAcceleratorFontsize();
	afx_msg void OnAcceleratorFontminus();
	afx_msg void OnModelingDumpcmpTwtt();
private:
	// get next model control point after the xCurrent
	float getModelPointNext(float xCurrent);
public:
	afx_msg void OnProcessingDumpsegylatlong();
	afx_msg void OnProcessingDecimatedatalines();
	afx_msg void OnDigitizingZeltSwapsource();
	void clearGDC(void);
	afx_msg void OnSegyconversionUnixtopcconvert();
	afx_msg void OnModelup();
	afx_msg void OnModeldown();
	afx_msg void OnEditRedo();
	afx_msg void OnModelingTogglepeglet();
	void myUpdatePrevFrame(void);
	int myUpdateAllFramesExcept(CDocument *aDoc);
	afx_msg void OnEditText();
	afx_msg void OnModelingToggleswavelabels();
	afx_msg void OnProcessingDatasplit();
	afx_msg void OnProcessingIdProcessingTracedub();
	afx_msg void OnToolsMapconverttime();
	afx_msg void OnProcessingSphericalcorrection();
	afx_msg void OnProcessingChopdata();
	afx_msg void OnProcessingAdddata();
	afx_msg void OnTimeRmsvelocityanalysis();
	afx_msg void OnMenuProcessingTrcBalancing();
	afx_msg void OnModelconversionRmsToIntNotUsingModel();
	afx_msg void OnModelconversionRmsToIntUsingModel();
	afx_msg void OnModelconversionRmsToIntTwtt();
	afx_msg void OnModelconversionIntToRmsvelocity();
	afx_msg void OnModelconversionTormsvelocity2();
	afx_msg void OnSonobuoyAutocalculateoffsetfromlat();
	afx_msg void OnSonobuoyLinearKm();
	BOOL myDigitizingLoadTXTUnix(DIGITIZE * dig2, CString f, BOOL isSort, BOOL isSortY, BOOL isY3=FALSE);

	afx_msg void OnSonobuoyPoly();
	void OnSonobuoyPolyTrace();
	virtual void OnDocWindowActivate(BOOL bActivate);
	afx_msg void OnSegyconversionIeeetoibmfloat();
	afx_msg void OnProcessingDatanormalize();
	afx_msg void OnProcessingFilterOffsetvarying();
	afx_msg void OnSonobuoyAssignoffsetfromsegy();
	afx_msg void OnFileSaveasdefaults();
	afx_msg void OnProcessingGainadjust();
	afx_msg void OnProcessingTracesubtractalongcurve();
	afx_msg void OnProcessingSplitoddeventraces();
	afx_msg void OnReflectionFaultenhancing();
	afx_msg void OnReflectionCoherencyplot();
	afx_msg void OnToolDumppolynomialpoints();
	afx_msg void OnWideNmostretching();
	afx_msg void OnMenuTracebalanceXt();
	afx_msg void OnSonobuoyShifttracenumber();
	afx_msg void OnToolsPicknext();
	afx_msg void OnProcessingDistanceadjustment();
	void DrawRayPathTwtt(CDC * pDC);
	afx_msg void OnReflectionFlattenboundary();
	afx_msg void OnModelingTheoreticalcurves();
};

/////////////////////////////////////////////////////////////////////////////
