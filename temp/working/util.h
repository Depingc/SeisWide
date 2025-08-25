#include "segy.h"

#define MIL 1000000.f
#define MINAMP 0.0000001f
#define PIO 0.017453293f // This is PI/180.
#define PI 3.1415926536f
#define ERRORVAL -99999.f
#define NATURALLOG 2.71828


BOOL isTimeDomainUtil(int mode);
float getReduceTimeEx(float offset, int drawmode, float vred);
//float getReduceTimeEx(float offset, int drawmode, float vred, float waterDepth, float obsDepth);
float getReduceTimeEx(float offset, int drawmode, float vred, float vWater, float Zwater, float Zobs, float Zgun);
double utmSimpsons(double fromE, double fromN, double toE, double toN);


CTime getFileModifiedTime(CString f);
BOOL isFileExist(CString name);
BOOL isFileExistNotDirectory(CString name);
BOOL isDirectoryExist(CString name);
BOOL isFileNewer(CString strTarget, CString strCompare);
BOOL isEndWith(CString name, CString str);
BOOL waitForFileUnlocked(CString f);

//segy related functions:
long long myRound(double x); 
bool isFloat1(double v, double vmin); // can round to 1 decimal point instead of 2
float myFloat1(double x); // round to 1 decimal point
float myFloat2(double x); // round to 2 decimal points
float myFloat3(double x); // round to 3 decimal points
float myFloat5(double x); // round to 5 decimal points, for cm handling
BOOL IsFormatValid(short a);
BOOL IsUnix(const short a);
void SwapTapeHeader(bhed* a);
void SwapTraceHeader(segy* tr);
void SwapTraceHeader(BOOL isUseReducedTime, BOOL isDeadTrcExist, segy* tr);
unsigned short Swap(unsigned short a);
short Swap(short a);
int Swap(int a);
unsigned int Swap(unsigned int a);
void SwapN(short* a,const int n);
void SwapN(unsigned short* a,const int n); 
void SwapN(long* a,const int n);
void SwapN(unsigned long* a,const int n);
void SwapN(float* tnf4,const int n);
void SwapN(segy * tr, short format);
void Swap(float* tnf4);
BOOL FormatSegy(CString f);

/* Butterworth filters */
void bfhighpass (int npoles, float f3db, int n, float p[], float q[]);
void bflowpass (int npoles, float f3db, int n, float p[], float q[]);
void bfdesign (float fpass, float apass, float fstop, float astop,
	int *npoles, float *f3db);

void ibm_to_float(int from[], int to[], int n);
void float_to_ibm(int from[], int to[], int n);

//general functions
CString getSeisWidePath();

CString getSecNameRevised(CString secName, CString seiName);
CString getFileNameAppend(CString aName, CString strAppend);
CString getFileNameSubdir(CString aName, CString aSubdir, CString strExt, bool isCreate);
CString getFileNameTemp(CString aName, CString strExt);
CString getFileNameTemp(CString aName, CString strExt, BOOL isOverwrite);
CString GetBaseFileName(CString sFileName);
CString GetPathFromFile(CString sFileName);
CString GetFileExtension(CString sFileName);
CString GetFileNameAppendExt(CString sFileName, CString strAppend, CString strExt);
double myFloat(double x);
float myFloat(float x);
BOOL isFloatEqual(float a, float b);
BOOL isFloatEqual(double a, double b);
BOOL isFloatAbsEqual(float a, float b);


double niceNum(double range, bool round);
float getMinRounded(double amin, double amax, int nDiv);
float getMaxRounded(double amin, double amax, int nDiv);
float getTickRounded(double amin, double amax, int nDivMin, int nDivMax, double tickPrefered);


BOOL isXShotInSegyDefined2(float xshot);

BOOL isInside(int ix, int ia, int ib);
BOOL isInsideFloat(float x, float a, float b);
BOOL isInsideDouble(double x, double a, double b);

BOOL isInsideDouble(double *x, float *a, float *b);  // this pointer version speeds up
BOOL isInsideFloat(float *x, float *a, float *b);

BOOL isInsideEx(int ix, int ia, int ib);
BOOL isInsideExFloat(float x, float a, float b);

float getOffset(segy * tr);
float getOffset(segydynamic * tr);
void setOffset(segy * tr, float f);
void setOffset(segydynamic * tr, float f);

float getDistance(segy * tr);
float getDistance(segydynamic * tr);
float getTraceDelay(segy * tr);
float getTraceDelay(segydynamic * trdyn);
float getTraceAmp(segydynamic * trdyn, float t, float sampRate);
float getTraceXVal(segydynamic *tr, int drawmode, BOOL isDistance);

float getXShotInSegy(CString f);
bool setOBSPosForSegy(CString f, float xshot);
int getNumTraceInSegy(CString f);
int getNumTraceInSegy(CFile *file);
int getCmpDistInSegy(CString f, int *cmp, float *dist, float *offset, int numMax);
int getFFIDInSegy(CString f, float *dist, int *ffid);
BOOL getXshotListFromTxOut(CString txName, CStringArray *sArr);


void setDistance(segy* tr, int distInMeter);
void setDistance(segy* tr, float fDistKm);
void setDistance(segydynamic* tr, float fDistKm);
void setDistance(segy* tr, double dDistKm);
void setTraceDelay(segy* tr, int iTimeDelay);
void setTraceDelay(segydynamic* tr, int iTimeDelay);
int loadSegy2ArrayDelete(CString f, segydynamic *trArr, bhed *bh, BOOL isIBMFloatSegy);  // not used
void freeSegyArray(segydynamic *trArr, int nxmax);

unsigned short revnib(unsigned short value);

// MFC related functions
void showFileInNotePad(CString name);
BOOL myShowHelp(CString strFile);
CString getSystemTempPath();
CString getGravmodPath();
BOOL setCurrentPathSameAs(CString f);
PROCESS_INFORMATION myProcess(CString exeNamePath, CString workPath, bool isPause);
PROCESS_INFORMATION myProcess(CString exeNamePath, CString workPath);
BOOL myProcessSimple(CString exeNamePath);
BOOL setCurrentDirectoryFromFile(CString name);



// Seiswide functions
double v2rho(double v, double velRef[9], double denRef[9]);

// math functions
void realft(float data[], unsigned long n, int isign);
void four1(float data[], unsigned long nn, int isign);
void ebc2asc(unsigned char ascHead[], int len);
void asc2ebc(unsigned char ascHead[], int len);
bool isAsciiPure(unsigned char ascHead[], int len);
int signum(float v);
int signum(double v);
//double getMedian(float x [ ], long n);
double getMedianInt(int *x, long n);
double getMedianValue(double *x, long n);
float getMedianValue(float *x, long n);

void splitMedia3(float a[], int n, float x, int &i, int &j);
//void getMedian3(float a[], int n, int &k);
int getMedian3(float a[], int n);

double getMedianAbsolute(float *dx, long n);
double getMedianFast(float *x, long n);
float getMaxValue(float *x, long n, float aThreshold);
float getMaxValue(float *x, long n);
double getMaxValue(double *x, long n);

int getMaxIndex(float *x, long n);
int getMinIndex(float *x, long n);
int getNearestTraceLeft(int *x, bool *isUseArr, int n, int px);
int getNearestTraceRight(int *x, bool *isUseArr, int n, int px);

float getMinValue(float *x, long n);
double getMinValue(double *x, long n);
double getValue4Color(double *x, long n);
float getValue4Color(float *x, long n);

float getMaxValueAbsolute(float *x, long n);
double getMaxValueAbsolute(double *x, long n);

int getMinXValueCPoint(CPoint *p, long n);
int getMaxXValueCPoint(CPoint *p, long n);

float getAvgValue(float *x, long n);
float getAvgValueAbsolute(float *x, long n);

void sort2(unsigned long n, float ra[], float rb[]);
void sort2(unsigned long n, float ra[], int ib[]);
int getClosestIndex(float dist [], int *n, float *x);
int getClosestIndex(float *fy, float *dy, int *n, float *x, int *i0);
int getClosestIndex(double fy, double dy, int n, double x, int i0);
int getClosestIndex(double dist[], int *n, double *x, int *i0);
int getClosestIndex(float dist[], int *n, float *x, int *i0);
int getClosestIndexRev(double dist[], int *n, double *x, int *i0);

int interpolate(int a, int b, int c, int d, double t, double s);
double logScaling2(double amp);
double logScalingInv2(double amp);
double logScalingPure(double amp);
double logScalingInvPure(double amp);
COLORREF interpolate(COLORREF a, COLORREF b, COLORREF c, COLORREF d, double t, double s);
COLORREF getColorInverseDistMono(int **colDataArr, float *rangeArr, int nx, int ny, float ymin, float dy, float x, float y);
double getAmpAtXY(float **dataArr, float *rangeArr, int nx, int ny, float ymin, float dy, float x, float y);
double getAmpAtXY(float **dataArr, int nx, int ny, double ymin, double dy, double y,  int jx, int jx2, double percentFromJx);

void trimRight(TCHAR line[]);
void sort(float dx[], long n);

int myClipPoints2(int n, double x[], double z[], double xmin, double xmax, double zmin, double zmax);
int myClipPoints3(int n, float x[], float z[], double xmin, double xmax, double zmin, double zmax);



double* convLatlon2XY(double alon, double alat);
double* convXY2Latlon(double x, double y);

void getAllPathArray(CStringArray *m_arr, CFileFind *finder, CString aPath);
CStringArray* getAllPathArray(CString aPath);
CStringArray* getLinesFromFile(CString aFile);
void getLinesFromFile(CStringArray* m_arr, CString aFile, bool isNeedComments);
CStringArray* getLinesFromFileEndWith(CString aFile, float vPerturb);

CString getLineFirst(CString aPath);
CString getLineFirstComment(CString aPath);

bool saveStringArrayToFile(CStringArray* m_arr, CString aFile);
bool showStringArrayToFile(CStringArray* m_arr);
BOOL isTrcInRange(int trc, CStringArray *trcAll);
BOOL isTrcInRange(CString strin, CStringArray *trcAll);
BOOL isValueDefined(float x, int n, float *xArray);

void setShotsFromDist(int *filej, int *shotk, float *shotDist, int numShot2Sum, int j, int k, float adist);
void filterTimeDomain(float *rData, int outLen, float f1, float f2, long tpp);
double removeTimeLine(float *rData, int itwmin, int itwmax, int itwdt);
double calcDist(double long1, double lat1, double long2, double lat2);
int axtoi(char *hexStg);


void showStringInFile(CString str);
void showStringInFile(CString str, CString f);
void showArrayInFile(float x [], int n);
void showArrayInFile(CString str [], int n);

void setSegyTotalTrace(CFile *file, unsigned short numtotal);
bool setAppendToFileText(CString f, CString line);

int formatVelocityArray(float dvUniform, int ncont, int nmax, double depth[], double vTop[], double vBot[]);
float getVFromVz(int ncont, double depth[], double vTop[], double vBot[], float z);
float getTwttAtZ(int ncont, double depth[], double vTop[], double vBot[], float z, double dz);
bool getTwttFromVz(int ncont, double depth[], double vTop[], double vBot[], int nz, double fz, double dz, double *twtt);
bool getTwttFromVz(int ncont, double depth[], double vTop[], double vBot[], int nz, double fz, double dz, float *twtt);

bool getTwttModFromZModAtDistance(int ncont, double depth[], double vTop[], double vBot[], double dz, double dist, double *twttMod);

bool getDepthTrcAtTwttIntervals(int ncont, double depth[], double vTop[], double vBot[], 
	int nt, float ft, float dt, float depthTrc[]);

// unique: for converting model in TWTT domain to depth domain
bool getDepthFromVt(int ncont, double timeMod[], double vTop[], double vBot[], int nt, double dt, double *depth);
bool getDepthModFromTwtt(int ncont, double timeMod[], double vTop[], double vBot[], double dt, double dist, double *depthMod);


bool getVelocityRmsMod(int ncont, double depth[], double vTop[], double vBot[], float dz, float dist, double *twttMod, float *VrmsTopMod, float *VrmsBotMod);
bool getVelocityRmsTrc(int ncont, double depth[], double vTop[], double vBot[], 
	float dz, int nzmaxModelAll, float *twttOut, float *vrmsOut);
bool getVelocityIntFromRms(int ntrms, float *tau, float *vrms, int nt, float dt, float ft, float *vint);

int getVelocityIntFromRms(int ntrms, float *tau, float *vrms, float *vint2);

	/*  nv_max, nz, np_max
bool sei_saveArrays(CString fstr, int nv_max, int nz, int **isei_npAll, float **sei_zAll, float ***xwAll, float ***twAll);
	np_max, nz, nv_max
		isei_npAll[iv][iz] , iz=[0,nz] , iv=[0,nv_max]
		sei_zAll[iv][iz] , iz=[0,nz] , iv=[0,nv_max]
		xwAll[iv][iz][i],  i=[0,  isei_npAll[iv][iz  ], iz=[0,nz] , iv=[0,nv_max]
		twAll[iv][iz][i]
bool sei_loadArrays(CString fstr, int nv_max, int nz, int **isei_npAll, float **sei_zAll, float ***xwAll, float ***twAll);
	*/



bool loadParam(CString f, CString param, CStringArray *sArr);
void convol(float* source, double* target, double* filt, int npt, int lfilt);
bool hilbertTransform(float* x, double* ampl, double* phase, double *omega, int npt, int lfilt);

void convSimple (int lx, int ifx, float *x,
	   int ly, int ify, float *y,
	   int lz, int ifz, float *z);
void conv (int lx, int ifx, float *x,
	   int ly, int ify, float *y,
	   int lz, int ifz, float *z);
void xcor (int lx, int ifx, float *x,
	   int ly, int ify, float *y, 
	   int lz, int ifz, float *z);


void SearchAllFiles(CString& strFolderPath, CString& targetFileName, CString& outFileName);

int SplitString(const CString& strInputString, const CString& strDelimiter, CStringArray& arrStringArray);
	BOOL IsNatural(const CString Str);
	BOOL IsNumeric(const CString Str);
	int StringToInt(const CString Str);
	double StringToFloat(const CString Str);

float getGravFromOneSideOfPolygon(float *x1, float *z1, float *x2, float *z2, float *Y1, float *Y2);

//void floodFillArr5(CDC* pDC, int nx, int ny, float **xArr, float *rangeArr, 
//				   float xmin, float xmax, float ymin, float ymax,
//				   int ixmin, int ixmax, int iymin, int iymax, int memMaxMB, BOOL isFaceUp);







int IsPointAbove2(float x1, float y1, float x2, float y2, float px, float py);
bool IsPointInBoundingBox(float x1, float y1, float x2, float y2, float px, float py);
bool LineIntersection(float l1x1, float l1y1, float l1x2, float l1y2,
                            float l2x1, float l2y1, float l2x2, float l2y2,
                            float *m1, float *c1, float *m2, float *c2,
                            float* intersection_X, float* intersection_Y);
bool LineSegmentIntersection(float l1x1, float l1y1, float l1x2, float l1y2,
                            float l2x1, float l2y1, float l2x2, float l2y2,
                            float *m1, float *c1, float *m2, float *c2,
                            float* intersection_X, float* intersection_Y);


double FindShortestDistanceToSegment(double x1, double y1, double x2, double y2, double pointX, double pointY);

//const CString CreateUniquieID();
//UINT64 get_cpuid(void);
//const CString getSystemID();


//CString PrintMACaddress(unsigned char MACData[]);
//CString GetMACaddress(void);
