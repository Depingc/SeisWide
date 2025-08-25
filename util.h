#pragma once
#include "segy.h"

#define MIL 1000000.f
#define OMIL 0.000001f
#define MINAMP 0.0000001f
#define MAXAMP 100000000.f
#define MINDIST 0.00001  // min distance or just 1 cm 
#define PIO 0.017453293f // This is PI/180.
#ifndef PI
 #define PI 3.141592653589793f
#endif
#define OPI 57.29577951 // This is 180/PI.
// #define deg2rad 0.017453293 // This is PI/180.
// #define rad2deg 57.29577951 // This is 180/PI.
#define ctan(a) (fabs(tan(a))>0.000001)?(1./tan(a)):99999.
#define NATURALLOG 2.71828
 
#define EARTH_RADIUS 6378206.0  // the Earth radius

// #define BETWN(a, b, c) ((a<=b && b<=c) || (a>=b && b>=c))?TRUE:FALSE
// isInsideDouble(double x, double a, double b) or isInsideFloat() are preferred, since BETWN(a, b, c) does not account for wrap around errors on the order of cm

#ifndef SGN
#define SGN(x) ((x) < 0 ? -1.0 : 1.0)
#endif

// #define SGN2(a) (double)(a>0) - (double)(a<0)   // this does not work
#define SAME_SIGN(a, b) (a < 0) == (b < 0)
inline int sgn(int a) { return (a == 0) ? 0 : (a<0 ? -1 : 1); }
inline int sgn(float a) { return (a == 0.f) ? 0 : (a<0.f ? -1 : 1); }
inline int sgn(double a) { return (a == 0.) ? 0 : (a<0. ? -1 : 1); }

// I cannot move these color numbers to CColorHelper.h since many other classes use them
const static COLORREF WHITE = RGB(255, 255, 255);
const static COLORREF BLACK = RGB(0, 0, 0);
const static COLORREF RED = RGB(255, 0, 0);
const static COLORREF GREEN = RGB(0, 255, 0);
const static COLORREF BLUE = RGB(0, 0, 255);
const static COLORREF GRAY = RGB(128, 128, 128);  // this is too light for unconstrained horizons, New
const static COLORREF GRAY2 = RGB(200, 200, 200); // this is suitable  for unconstrained horizons
const static COLORREF NAVY = RGB(0, 175, 239);
const static COLORREF ORANGE = RGB(0xff, 0x80, 0x00); // (255, 128, 0), New
const static COLORREF BROWN = RGB(0x72, 0x5b, 0x3f); // (114, 91, 63), New
// const static COLORREF YELLOW = RGB(255, 255, 0);  // my old yellow
// 20 = (2 �� 16^1) + (0 �� 16^0) = 32
const static COLORREF YELLOW = RGB(0xd0, 0xb0, 0x20); // (208, 176, 32)
/*
# define HSV_BLACK	{  0.0, 0.00, 0.00}
# define HSV_GRAY	{  0.0, 0.00, 0.50}
# define HSV_WHITE	{  0.0, 0.00, 1.00}

# define HSV_HUE1	{240.0, 1.00, 0.50}
# define HSV_HUE2	{120.0, 1.00, 0.50}
# define HSV_HUE3	{  0.0, 1.00, 0.50}

# define HSV_DRED	{  0.0, 1.00, 0.50}
# define HSV_BROWN	{ 30.0, 1.00, 0.30}
# define HSV_GREEN	{140.0, 1.00, 0.50}
# define HSV_BLUE	{240.0, 1.00, 0.70}

# define HSV_YELLOW	{ 70.0, 1.00, 0.50}
*/

const static float O255 = 0.0039215686f;  // ==1/255

const static CString TAB = _T("\t");
const static CString BLANK = _T("");
const static CString SPACE = _T(" ");
const static CString SPACE2 = _T("  ");
const static CString COMMA = _T(",");
const static CString QUOTE = _T("\"");
const static CString CARRAGE = _T("\r\n");
const static TCHAR CSLASH = _T('\\');
const static CString DOT = _T(".");



static unsigned char a2e[256] = {
	0, 1, 2, 3, 55, 45, 46, 47, 22, 5, 37, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 60, 61, 50, 38, 24, 25, 63, 39, 28, 29, 30, 31,
	64, 79, 127, 123, 91, 108, 80, 125, 77, 93, 92, 78, 107, 96, 75, 97,
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 122, 94, 76, 126, 110, 111,
	124, 193, 194, 195, 196, 197, 198, 199, 200, 201, 209, 210, 211, 212, 213, 214,
	215, 216, 217, 226, 227, 228, 229, 230, 231, 232, 233, 74, 224, 90, 95, 109,
	121, 129, 130, 131, 132, 133, 134, 135, 136, 137, 145, 146, 147, 148, 149, 150,
	151, 152, 153, 162, 163, 164, 165, 166, 167, 168, 169, 192, 106, 208, 161, 7,
	32, 33, 34, 35, 36, 21, 6, 23, 40, 41, 42, 43, 44, 9, 10, 27,
	48, 49, 26, 51, 52, 53, 54, 8, 56, 57, 58, 59, 4, 20, 62, 225,
	65, 66, 67, 68, 69, 70, 71, 72, 73, 81, 82, 83, 84, 85, 86, 87,
	88, 89, 98, 99, 100, 101, 102, 103, 104, 105, 112, 113, 114, 115, 116, 117,
	118, 119, 120, 128, 138, 139, 140, 141, 142, 143, 144, 154, 155, 156, 157, 158,
	159, 160, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183,
	184, 185, 186, 187, 188, 189, 190, 191, 202, 203, 204, 205, 206, 207, 218, 219,
	220, 221, 222, 223, 234, 235, 236, 237, 238, 239, 250, 251, 252, 253, 254, 255
};

static unsigned char e2a[256] = {
	0, 1, 2, 3, 156, 9, 134, 127, 151, 141, 142, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 157, 133, 8, 135, 24, 25, 146, 143, 28, 29, 30, 31,
	128, 129, 130, 131, 132, 10, 23, 27, 136, 137, 138, 139, 140, 5, 6, 7,
	144, 145, 22, 147, 148, 149, 150, 4, 152, 153, 154, 155, 20, 21, 158, 26,
	32, 160, 161, 162, 163, 164, 165, 166, 167, 168, 91, 46, 60, 40, 43, 33,
	38, 169, 170, 171, 172, 173, 174, 175, 176, 177, 93, 36, 42, 41, 59, 94,
	45, 47, 178, 179, 180, 181, 182, 183, 184, 185, 124, 44, 37, 95, 62, 63,
	186, 187, 188, 189, 190, 191, 192, 193, 194, 96, 58, 35, 64, 39, 61, 34,
	195, 97, 98, 99, 100, 101, 102, 103, 104, 105, 196, 197, 198, 199, 200, 201,
	202, 106, 107, 108, 109, 110, 111, 112, 113, 114, 203, 204, 205, 206, 207, 208,
	209, 126, 115, 116, 117, 118, 119, 120, 121, 122, 210, 211, 212, 213, 214, 215,
	216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231,
	123, 65, 66, 67, 68, 69, 70, 71, 72, 73, 232, 233, 234, 235, 236, 237,
	125, 74, 75, 76, 77, 78, 79, 80, 81, 82, 238, 239, 240, 241, 242, 243,
	92, 159, 83, 84, 85, 86, 87, 88, 89, 90, 244, 245, 246, 247, 248, 249,
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 250, 251, 252, 253, 254, 255
};


//double getReduceTimeExOld(double offset, int drawmode, double rvred, double vW, double Zw, double Zs, double Zg);
//##########################################################



/**  UTM computation starts **/
const static double FalseEasting = 500000.0;
const static double ScaleFactor = 0.9996;
const static double EarthRadius = 6378137.0;
inline const double utmGetScaleFactor(double e)
{
	return ScaleFactor / cos(e / EarthRadius);
}

inline double utmSimpsonsNotused(double fromE, double fromN, double toE, double toN)
{
	double f1 = utmGetScaleFactor(fromE - FalseEasting);
	double f2 = utmGetScaleFactor(toE - FalseEasting);
	double fm = utmGetScaleFactor((fromE + toE) * 0.5 - FalseEasting);
	double dx = (toE - fromE) / 6 * (f1 + 4 * fm + f2);
	double dy = toN - fromN;
	return sqrt(pow(dx, 2) + pow(dy, 2));
}
/**  end UTM computation starts  **/


double calcDist(double long1, double lat1, double long2, double lat2);

//##########################################################
// ############### File related functions #################
CTime getFileModifiedTime(CString f);
BOOL isFileExistNotDirectory(CString name);

// isMyFileReadable() should be actually named as isMyFileAccessible()
inline BOOL isMyFileReadable(CString f) { 
	CFile file2;
	if (!file2.Open(f, CFile::modeRead | CFile::shareDenyNone))
		return FALSE;
	file2.Close();
	return TRUE;
}
CString BrowseFolderSlash(CString title);
bool BrowseFolderMaySlash(HWND h, CString title, CString &result);
BOOL isDirectoryWritable(CString name);
BOOL isDirectoryExist(CString name);
BOOL isFileNewer(CString strTarget, CString strCompare);
BOOL isSameFileSizeTime(CString strTarget, CString strCompare);
BOOL isEndWith(CString name, CString needle);
BOOL waitForFileUnlocked(CString f);

//general functions: no trailing back slashes appended!!!
CString getSeisWideExePath();
CString getSeisWideParaPath();  // usually different from above when debugging
CString getSecNameRevised(CString secName, CString seiName);
CString getFileNameAppend(CString aName, CString strAppend);
CString getFileNameNextVersion(CString aName);
CString getFileNameAppendTemp(CString aName, CString strAppend);
CString getFileNameSubdir(CString aName, CString aSubdir, CString strExt, bool isCreate);
CString getFileNameTemp(CString aName, CString strExt);
CString getFileNameTempNospace(CString aName, CString strExt);
CString getFileNameTemp(CString aName, CString strExt, BOOL isOverwrite);
BOOL isInsideTempDir(CString aName);
CString GetBaseFileName(CString sFileName);
inline CString getFileNameOnly(CString sFileName) {
	int i = sFileName.ReverseFind(CSLASH);
	if (i >= 0)
		return sFileName.Mid(1 + i);
	else
		return sFileName;
}

inline CString removeFileExtension(CString filename) {
	int lastdot = filename.ReverseFind(_T('.'));
	if (lastdot < 0) return filename;
	return filename.Mid(0, lastdot);
}

inline CString updateFileName(CString tOld, CString tNew) {
	int i = 0;
	int lenOld = tOld.GetLength();
	int lenNew = tNew.GetLength();
	CString tNewBak = tNew;
	CString tOldBak = tOld;
	tOld.MakeLower();
	tNew.MakeLower();
	while (i<lenOld && i<lenNew && tOld.GetAt(i) == tNew.GetAt(i)) {
		i++;
	}


/*	
tOld: H:\arcticAllDeping\Arctic2007-2011AllSB\sb2011\refl\
tNew: H:\arcticAllDeping\arcticAll2007-2016Others\Arctic2007-2011AllSB\sb2011\refl\
*/
	// now i points to a name not fitting
	CString out = tNew.Mid(0, i);
	int j = out.ReverseFind(CSLASH);
	if (j < 0) return tNewBak;
	int k = tNew.Find(CSLASH, j + 1);
	out = tNewBak.Mid(0, k) + CSLASH +  tOldBak.Mid(i);
	if (isDirectoryExist(out)) return out;

	j = tOld.Find(CSLASH, i + 1);
	k = tNew.Find(CSLASH, k + 1);
	if (j<0 || k < 0) return tNewBak;
	out = tNewBak.Mid(0, k) + CSLASH + tOldBak.Mid(j);
	if (isDirectoryExist(out)) return out;

	return tNewBak;
}
CString GetPathFromFileSlash(CString sFileName);
CString GetPathExistingFromFileSlash(CString sPathName);

CString GetFileExtension(CString sFileName);
CString SetFileExtension(CString sFileName, CString strExt);
CString GetFileNameAppendExt(CString sFileName, CString strAppend, CString strExt);
double getFreeSpace(CString f); // return the free space in MB in the disk where the file f resides
double getMemoryUsagePhysPercent();
inline DWORDLONG getMemoryAvail() {
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	return (DWORDLONG)(statex.ullAvailPhys * 0.000001);
	//return statex.dwMemoryLoad;
}

// MFC related functions
inline CString getSystemTempPathSlash()
{
	TCHAR lpszSystemInfo[MAX_PATH];
	GetTempPath(MAX_PATH, lpszSystemInfo);
	return CString(lpszSystemInfo);
}
CString getGravmodPath();

// set current path to be the same as specified in f. 
inline BOOL setCurrentPathExactAs(CString f)
{
	int pathLen = f.ReverseFind(CSLASH);
	if (pathLen>(-1)) {
		SetCurrentDirectory(f.Left(pathLen));
		return TRUE;
	}

	return FALSE;
}

// input can be either a filename or a path name
inline BOOL setCurrentPathAs(CString name)
{
	if (!isFileExistNotDirectory(name))
		return setCurrentPathExactAs(name);

	int pathLen = name.ReverseFind(CSLASH);
	CString txPath = name.Left(pathLen);
	if (pathLen > (-1))
	{
		SetCurrentDirectory(txPath);
		return true;
	}
	return false;
}

inline CString getPathAbs(CString in, CString workPathSlash) {
	int i = in.Find(_T("."));
	int workLen = workPathSlash.GetLength();
	if (i > 0 || workLen<2) return in; // this is already an absolute path

	int ii = in.Find(_T(".."));
	CString tStr;
	if (i == 0) {
		// single dot case
		return workPathSlash + in.Mid(1);
	}
	else if (ii == 0) {
		// double dot case
		int k = workPathSlash.Mid(0, workLen - 1).ReverseFind(CSLASH);
		if (k>0) 
			return workPathSlash.Mid(0, k) + in.Mid(3);
	}

	// unknown case
	return in;
}


PROCESS_INFORMATION myUnicodeProcess(CString exeNamePath, CString workPath, bool isPause = true);
// PROCESS_INFORMATION myUnicodeProcess(CString exeNamePath, CString workPath);
BOOL myProcessSimple(CString exeNamePath);

bool RunFromMemory(PROCESS_INFORMATION *proc_i, BYTE* pByte, CString pPath); // testing
struct FILE_CACHE {
	CString fName;
	const static int maxBytes = 3000000;
	int bytesLoaded;
	BYTE bytes[maxBytes];
	FILE_CACHE() :
		bytesLoaded(0)
	{};
};
bool myUnicodeProcessCache(PROCESS_INFORMATION *proc_i, CString exeNamePath,
	CString workPath);


CString getLineFirst(CString aPath);
CString getLineSecond(CString aPath);
CString getLineFirstComment(CString aPath);
bool appendToTextFile(CString aFile, CString str);


bool isTxtMedicalLocus(CString aFile);


int getTxtNumColumns(CString cstr);
int getTxtFileNumColumns(CString aFile);

bool isTxtFile6Columns(CString aFile);
CString conv6ColTo3Col(CString aFile);
CString conv6ColTo3ColSubSeafloor(CString aFile);
CString conv6ColTo3ColSubBasement(CString aFile);

CString convStringArrayToCString(CStringArray* m_arr, bool isLineFeed);
bool saveStringArrayToFile(CStringArray* m_arr, FILE*);
bool saveStringArrayToFile(CStringArray* m_arr, CString);
bool saveStringArrayToFileAsciiUnix(CStringArray* m_arr, CString aFile);
bool showStringArrayToFile(CStringArray* m_arr);
BOOL isTrcInRange(int trc, CStringArray *trcAll);
BOOL isTrcInRange(CString strin, CStringArray *trcAll);
BOOL isValueDefined(float x, int n, float *xArray);

void setShotsFromDist(int *filej, int *shotk, float *shotDist, int numShot2Sum, int j, int k, float adist);
void filterTimeDomain(float *rData, int outLen, float f1, float f2, long tpp);
double removeTimeLine(float *rData, int itwmin, int itwmax, int itwdt);


void showStringInFile(CString str);
bool saveStringToFile(CString str, CString newName);
CString saveStringToTempFile(CString str);
CString saveStringToRegiFile(CString str);
CString regiTempFileName();
void showFileInNotePad(CString name);
void showFileInDefaultProg(CString fPathName);
void showStringInFile(CString str, CString f);
void showArrayInFile(float x[], int n, CString comment);
bool showArrayInFile(double *y, int n, CString title, double dx);
bool showArrayInFile(float *y, int n, CString title, double dx);
void showArrayInFile(CString str[], int n);
BOOL myShowHelp(CString strFile = _T(""));

bool setAppendToFileText(CString f, CString line);
// ############### End File related functions #################
//##########################################################

double pround(double x, int precision);
float pround(float x, int precision);
long long myRound(double x);
bool isFloat1DecOnly(double v, double vmin); // can round to 1 decimal point instead of 2
double myFloat8(double x); // round to 8 decimal points, for cm handling
double myFloat(double x, int precision = 5);
float myFloat(float x, int precision = 5);
BOOL isFloatEqual(float a, float b);  // this defaults to 5 decimals, which is cm resolution
BOOL isFloatEqual(float a, float b, float res);  
BOOL isFloatEqual3(float a, float b); // this is m resolution
BOOL isFloatEqual(double a, double b); // this is cm resolution
BOOL isFloatAbsEqual(float a, float b); // this is cm resolution

inline void SWITCH_BOOL(BOOL &a, BOOL &b) {
	BOOL temp = a; 
	a = b; 
	b = temp;
}
inline void SWITCH_FLOAT(float &a, float &b) {
	float temp = a;
	a = b;
	b = temp;
}
inline void SWITCH_DOUBLE(double &a, double &b) {
	double temp = a;
	a = b;
	b = temp;
}
// NOTE: this is interchanging between a and b, and is very different from the Swap() which is Unix-PC swapping !!!



//##########################################################
//##############IBM-Unix-PC functions ########################
// BOOL IsUnix(const short a);
void SwapTapeHeader(bhed* a);
//void SwapTraceHeader(segy* tr);
void SwapTraceHeader(bool isSegyCurrentlyPCFormat, bool isUseReducedTime, bool isDeadTrcExist, segy* tr);


//unsigned short Swap(unsigned short a);
//short Swap(short a);
//int Swap(int a);
//unsigned int Swap(unsigned int a);
//void SwapN(unsigned short* a,const int n); 
//void SwapN(long* a,const int n);
//void SwapN(unsigned long* a,const int n);
//void SwapN(float* tnf4,const int n);
//void SwapN(DAT *dat, short format, const int ns);

inline void SwapN(short* a, const int n) {
	// swaps n shorts in place
	for (int i = 0; i < n; i++) {
		swap_short_2(&a[i]);
	}
}
inline void SwapN(unsigned short* a, const int n) {
	// swaps n shorts in place
	for (int i = 0; i < n; i++) {
		swap_u_short_2(&a[i]);
	}
}

inline void SwapN(int* a, const int n) {
	// swaps n ints in place
	for (int i = 0; i < n; i++) {
		swap_int_4(&a[i]);
	}
}

inline void SwapN(long* a, const int n) {
	// swaps n longs in place. tested, works great! June 30, 2006
	for (int i = 0; i < n; i++) {
		swap_long_4(&a[i]);
	}
	/*
	for (int i=0;i<n;i++){
	int b=a[i];
	a[i]= ((b>>24) & 0x000000ff) +
	((b>>8)  & 0x0000ff00) +
	((b<<8)  & 0x00ff0000) +
	((b<<24) & 0xff000000);
	}
	*/
}
inline void SwapN(unsigned long* a, const int n) {
	// swaps n longs in place
	for (int i = 0; i < n; i++) {
		swap_u_long_4(&a[i]);
		//int b = a[i];
		//a[i] = ((b >> 24) & 0x000000ff) +
		//	((b >> 8) & 0x0000ff00) +
		//	((b << 8) & 0x00ff0000) +
		//	((b << 24) & 0xff000000);
	}
}
inline void SwapN(float* tnf4, const int n)
{  // please test if this functions work ok	
   //	int *tni4;
	for (int i = 0; i < n; i++) {
		//if (i == 8272) {
		//	int sdfsd = 1;
		//}
		swap_float_4(&tnf4[i]);

		//tni4 = (int *)&tnf4[i];
		//*tni4 = (((*tni4 >> 24) & 0xff) | ((*tni4 & 0xff) << 24) |
		//	((*tni4 >> 8) & 0xff00) | ((*tni4 & 0xff00) << 8));
	}
}

inline void SwapN(DAT *dat, short format, const int ns) {
	/* data sample format code:
	1 or 5 = floating point (4 bytes); 5 means it is IEEE format with only a swap is enough, not converting from IBM float
	2 = fixed point (4 bytes)
	3 = fixed point (2 bytes)
	4 = fixed point w/gain code (4 bytes)
	Why do not need an extra ns instead of tr->ns ? It is because if in unix format, tr->ns should be swapped.
	So to avoid such problem, we add extra para.
	*/

	if (format == 1 || format == 5) SwapN((float *)&dat->data[0], ns);
	else if (format == 2) SwapN((int *)&dat->dataint[0], ns);
	else if (format == 3) SwapN((short *)&dat->datashort[0], ns);
}

//void SwapN(segy * tr, short format, const int ns);
// void Swap(float* tnf4);
//BOOL FormatSegy(CString f);


// output char* will have >=num_min chars
// NOTE: the output char* MUST be free-ed by  free1(), otherwise there will be exception!!!
inline char* getCharFromCString(CString tStr, size_t num_min) {
	size_t nLeng = (tStr.GetLength() + 1) * sizeof(TCHAR);
	if (nLeng < num_min)
		nLeng = num_min;  // this is important: otherwise it may have exception
	char *pOutput = (char*)alloc1(nLeng, sizeof(char));
	ZeroMemory(pOutput, nLeng);
	strcpy_s(pOutput, nLeng, CT2A(tStr));
	return pOutput;
}


inline void ebc2asc(unsigned char ascHead[], int len) {
	// this converts from IBM's DBCDIC format to ASCII format.

	for (int x = 0; x < len; x++) {
		ascHead[x] = e2a[(unsigned char)ascHead[x]]; // translate from ebcdic to ascii 
	}
}

inline void asc2ebc(unsigned char ascHead[], int len) {
	// this converts from ASCII format to IBM's DBCDIC format.
	for (int x = 0; x < len; x++) {
		ascHead[x] = a2e[(unsigned char)ascHead[x]]; // translate from ebcdic to ascii 
	}
}
bool isAsciiPure(unsigned char ascHead[], int len);
void autoAsc2ebc(unsigned char ascHead[], int len);

void ibm_to_float(int from[], int to[], int n);
void float_to_ibm(int from[], int to[], int n);
//##############End IBM-Unix-PC functions ########################
//##########################################################

/* Butterworth filters */
void bfhighpass (int npoles, float f3db, int n, float p[], float q[]);
void bflowpass (int npoles, float f3db, int n, float p[], float q[]);
void bfdesign (float fpass, float apass, float fstop, float astop,
	int *npoles, float *f3db);


inline CString addQuotes(CString str)
{
	// it already quoted, we will not quote again!
	int i = str.Find(QUOTE);
	if (i >= 0) {
		int len = str.GetLength();
		int j = str.ReverseFind(_T('\"'));
		if (i == 0 && j == len - 1) return str; // already quoted
		if (i>0) {
			// we need to escape this quote
			str = str.Mid(0, i) + QUOTE + QUOTE + str.Mid(i + 1);
		}
	}
	// ############Please test on the codes above !!!
	
	if (str.Find(QUOTE) <= 0 && str.Find(SPACE) > 0)
		str = QUOTE + str + QUOTE;
	return str;
}


inline CString stripQuotes(CString str)
{
	int i = str.Find(QUOTE);
	if (i < 0) return str;

	int j = str.Find(QUOTE, i+1);
	if (j < 0) {
		// since there is only 1 quote, we will use the data before that quote only
		if (i == 0)
			return str.Mid(1);  // "CLVFAQKLPGNDNSTATLCLGHHAVPNGTIVKTITNDQIEVTNA
		else
			return str.Mid(0, i); //   subtype:H3N2, collection date:9/25/2003"
	}
	if (j - i - 1 < 1) return BLANK;

	return str.Mid(i + 1, j - i - 1);
}

inline bool isDigit(TCHAR c) {
	return ((_T('0') <= c) && (c <= _T('9')));
}

inline BOOL IsNatural(const CString Str) {
	// Check each character of the string
	// If a character at a certain position is not a digit,
	// then the string is not a valid natural number
	for (int i = 0; i < Str.GetLength(); i++) {
		register WCHAR c = Str.GetAt(i);
		if (c == _T('-') || c == _T('.') || c == 65279) continue;  // a negative sign or a dot is normal 
		if (c < _T('0') || c > _T('9'))
			return FALSE;
	}
	return TRUE;
}
BOOL IsNumeric(const CString Str);

inline int StringToInt(const CString Str) {
	BOOL IsValid = IsNatural(Str);
	return (IsValid == TRUE) ? _tstoi(Str) : 0;
}

inline double StringToFloat(CString Str) {
	BOOL IsValid = IsNumeric(Str);
	for (int i = 0; i < Str.GetLength(); i++) {
		if (Str[i] == 65279) {
			// this is special char that makes it not able to parse: "ZERO WIDTH NO-BREAK SPACE"
			Str = Str.Mid(i + 1); // if happened at the beging of my string in one case, so I ignore that char
			break;
		}
	}
	return (IsValid == TRUE) ? _tstof(Str) : 0.;
}

inline BOOL isInsideLONG(LONG ix, LONG ia, LONG ib) {
	if (ia < ib) {
		if (ix >= ia && ix <= ib) return TRUE;
	}
	else {
		if (ix >= ib && ix <= ia) return TRUE;
	}
	return FALSE;
}

// NOTE: if I use the same function name here, the arguments of int as treated as
// the same as LONG, so a dead loop may occur
inline BOOL isInside(int ix, int ia, int ib) {
	if (ia < ib) {
		if (ix >= ia && ix <= ib) return TRUE;
	}
	else {
		if (ix >= ib && ix <= ia) return TRUE;
	}
	return FALSE;
}

static const double RES_FLOAT = 0.00001; // using cm resolution now
inline BOOL isInsideDouble(double x, double a, double b) {
	// a problem, sometimes values can be off by 1cm, in which case we still want it
	if (a < b) {
		if (x >= a- RES_FLOAT && x <= b + RES_FLOAT) return TRUE;
	}
	else {
		if (x >= b- RES_FLOAT && x <= a+ RES_FLOAT) return TRUE;
	}

	return FALSE;
}
inline BOOL isInsideFloat(float x, float a, float b) {
	return isInsideDouble((double)x, (double)a, (double)b);
}

inline void limitValueAbs(double &v, double vmax) {
	if (vmax < RES_FLOAT) vmax = RES_FLOAT;
	if (v < -vmax) v = -vmax;
	else if (v > vmax) v = vmax;
}
double niceNumMinMax(double range, bool isMaxNeeded);
double niceNumRound(double range);

// when calculating fpr ticks, loop thru using this increment
inline double getMinorIncNice(double tickNice) {
	double out;

	// now deals with peculiors
	if (tickNice == 5.) out = 1.;
	else if (tickNice == 0.5) out = .1;
	else if (tickNice == 0.05) out = .01;
	else if (tickNice == 50) out = 10.;
	else if (tickNice == 500) out = 100.;
	else if (tickNice == 5000) out = 1000.;
	else out = tickNice * 0.1;

	return out;
}
float getMinRounded(double amin, double amax, int nDiv);
float getMaxRounded(double amin, double amax, int nDiv);
float getTickRounded(double amin, double amax, int nDivMin, int nDivMax, double tickPrefered);
CString getNiceLabelFromValue(double y, double yTickNice);

//     SEI_PAIR centroid = compute2DPolygonCentroid(polygon, vertexCount);
inline SEI_PAIR compute2DPolygonCentroid(const SEI_PAIR* vertices, int vertexCount)
{
	SEI_PAIR centroid;
	centroid.set(0., 0.);
	double signedArea = 0.0;
	double x0 = 0.0; // Current vertex X
	double y0 = 0.0; // Current vertex Y
	double x1 = 0.0; // Next vertex X
	double y1 = 0.0; // Next vertex Y
	double a = 0.0;  // Partial signed area

	// For all vertices except last
	int i = 0;
	for (i = 0; i < vertexCount - 1; ++i)
	{
		x0 = vertices[i].x;
		y0 = vertices[i].y;
		x1 = vertices[i + 1].x;
		y1 = vertices[i + 1].y;
		a = x0 * y1 - x1 * y0;
		signedArea += a;
		centroid.x += (x0 + x1) * a;
		centroid.y += (y0 + y1) * a;
	}

	// Do last vertex separately to avoid performing an expensive
	// modulus operation in each iteration.
	x0 = vertices[i].x;
	y0 = vertices[i].y;
	x1 = vertices[0].x;
	y1 = vertices[0].y;
	a = x0 * y1 - x1 * y0;
	signedArea += a;
	centroid.x += (x0 + x1) * a;
	centroid.y += (y0 + y1) * a;

	signedArea *= 0.5;
	centroid.x /= (6.0 * signedArea);
	centroid.y /= (6.0 * signedArea);

	return centroid;
}

//inline int numPlaces(int n) {
//	if (n == 0) return 1;
//	return floor(log10(abs(n))) + 1;
//}
inline int countDigits(int value)
{
	int result = 0;
	while (value != 0) {
		value /= 10;
		result++;
	}
	return result;
}
// int getNiceRate(double amin, double amax);

unsigned short revnib(unsigned short value);
int axtoi(char *hexStg);
void trimRight(TCHAR line[]);

// Seiswide functions
double v2rho_corr(double v, double x, int layer, CStringArray* arr_corr);
double v2rho(double v, double velRef[9], double denRef[9]);
double v2rho(double v, double velRef[], double denRef[], int len);

// math functions
void realft(float data[], unsigned long n, int isign);
void four1(float data[], unsigned long nn, int isign);

double getMedianInt(int *x, long n);
double getMedianValue(double x[], long n);
float getMedianValue(float x[], long n);
double getRmsValue(float *x, int n);

void splitMedia3(float a[], int n, float x, int &i, int &j);
int getMedian3(float a[], int n);

double getAvgInterval(double [], int);
float getAvgInterval(float [], int);

double getMedianAbsolute(float [], long n);
float getMaxValue(float [], long, float);
float getMaxValue(float [], long);
double getMaxValue(double [], long);

int getMaxIndex(float x[], long n);
int getMinIndex(float x[], long n);
int getMinIndex(int x[], long n);
int getNearestTraceLeft(int x[], bool isUseArr[], int n, int px);
int getNearestTraceRight(int x[], bool isUseArr[], int n, int px);

float getMinValue(float x[], long n, float);
double getMinValue(double x[], long n, double);
double getValue4Color(double x[], long n);
float getValue4Color(float x[], long n);

float getMaxValueAbsolute(float x[], long n);
double getMaxValueAbsolute(double x[], long n);

int getMinXValueCPoint(CPoint p[], long n);
int getMaxXValueCPoint(CPoint p[], long n);

float getAvgValueOld(float x[], long n);

float getAvgValue(float x[], long n);
double getAvgValue(double x[], long n);

float getAvgValueAbsoluteOld(float *x, long n);
float getAvgValueAbsolute(float x[], long n);

void sortSei(float x[], long n); // this sorts only if there is no points spaced at 0.001 in X!!!
void sort1(float x[], long n);
void sort2(unsigned long n, float ra[], float rb[]);
void sort2(unsigned long n, float ra[], int ib[]);
void sort3(unsigned long n, float ra[], float rb[], float rc[]);
void sortDistance(float **ampArr, float *rangeArray, int nx, int nt);

int getClosestIndex(int trace[], int n, float x);

//int getClosestIndex(double dist[], int n, float x);
int getClosestIndex(float dist [], int n, float x);
int getClosestIndex(float fy, float dy, int n, float x, int i0);
int getClosestIndex(double fy, double dy, int n, double x, int i0);
int getClosestIndex(double dist[], int n, double x, int i0);
int getClosestIndex(float dist[], int n, float x, int i0);
int getClosestIndexRev_Deprec(double dist[], int n, double x, int i0);


// bilinear color interpolation
inline int interpolate(int a, int b, int c, int d, double t, double s) {
	return (int)(a*(1 - t)*(1 - s) + b * t*(1 - s) + c * (1 - t)*s + d * t*s);
}

inline double interpolate_d(double a, double b, double c, double d, double t, double s) {
	return a * (1 - t)*(1 - s) + b * t*(1 - s) + c * (1 - t)*s + d * t*s;
}

inline double interpolLinear(double a, double b, double t) {
	return a * (1. - t) + b * t;
}
inline float interpolLinear(float a, float b, float t) {
	// return (float)interpolLinear((double)a, (double)b, (double)t);
	return a * (1.f - t) + b * t;
}
inline float interpolLinear(float xm[], float ym[], int n, float x) {
	if (n < 1) return 0.f;
	if (n == 1 || x <= xm[0]) return ym[0];
	if (x >= xm[n - 1]) return ym[n - 1];

	for (int i = 1; i < n; i++) {
		if (x >= xm[i - 1] && x <= xm[i]) {
			// this function will be very deep in loop. So we want it to be as fast as possible!!! So we drop complicated calculation
			// register double delta = ampColorDyn2->percentAmp[i] - ampColorDyn2->percentAmp[i-1];
			if (xm[i] == xm[i - 1]) return ym[i];

			float t = (x - xm[i - 1]) / (xm[i] - xm[i-1]);
			return interpolLinear(ym[i - 1], ym[i], t);
		}
	}
	return ym[0]; // this should never be reached
}

//inline double logScaling2_Deprec(double amp) {
//	double out = log10(1. + fabs(amp));
//	return sgn(amp)*out*out;
//}
//
//inline double logScalingInv2_Deprec(double ampLog)
//{
//	return sgn(ampLog) * (pow(10., sqrt(fabs(ampLog))) - 1.);
//}



// log scale for color display
// note:  log10(1) = 0;  log10(10)=1; log10(1000)=3
//    but this function will return myLogScaling0(0)=0; myLogScaling0(9)=1; myLogScaling0(999)=3; 
//					myLogScaling0(-999)=-3 ...
inline double logScalingPure(double ampl) {
	return sgn(ampl) * log10(1. + fabs(ampl));
}

inline double logScalingInvPure(double ampLog) {
	return sgn(ampLog) * (pow(10., fabs(ampLog)) - 1.);
}

// When "Low" is appended, it is for the low amplitude part

COLORREF getGoodColorRand(bool isMainModelShowing);

bool getAmpAtXY(float **dataArr, int nx, int ny, double ymin, double dy,
	int iTrace1, int iTrace2, double percentFromTr1,
	float *trPixel, float *yValPixel, int nyPixel,
	float *trDataPixel1, float *trDataPixel2, int &iTrace1Existing, int &iTrace2Existing);
bool getAmpAtXY(float **dataArr, int nx, int ny, double ymin, double dy,
				  double yRate, double *ampArr, int nrow, 
				  int jx, int jx2, double percentFromJx);
//bool getAmpAtXYSpline(float **dataArr, int nx, int ny, float ymin, float dy,
//	int jx, int jx2, float percentFromJx, float *timePixel, int nPixel);

double* convLatlon2XY(double alon, double alat);
double* convXY2Latlon(double x, double y);

// void getAllPathArray(CStringArray *m_arr, CFileFind *finder, CString aPath);
CStringArray* getAllPathArray(CString aPath);
CStringArray* getLinesFromFileWin(CString aFile);
CStringArray* getLinesFromFile(CString aFile, bool isNeedComments = false);
CStringArray* getLinesFromString(CString aFile, bool isNeedComments, CString token, int numToken);
void getLinesFromFile(CStringArray* m_arr, CString aFile, bool isNeedComments, int numLinesDesired);
CStringArray* getLinesFromFileEndWith(CString aFile, float vPerturb);
CStringArray* getLinesFromCStringSlow(CString str, bool isNeedComments);
CStringArray* getLinesFromCString(CString str, bool isNeedComments);
CString getTextFromFileUnix(CString aFile);




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
void SearchAllFilesXTemp(CString& strFolderPath, CString& targetFileName, CString& outFileName);


//bool SearchReplaceAllFiles(CString strFolderPath, CString strTarget, CString strReplace);
// note: if the condition is empty, the filename must contain strTarget for replacement to occur
// if condition is not empty, the file name must also not contain the condition before real replacement to occur
// Example:
// 		SearchReplaceAllFiles(_T("G:\\arcticSBs\\atlas\\atlasFinals\\Page3PdfFinal"), 
//			_T(".pdf"), _T("Page3.pdf"), _T("Page3"));


// these 2 functions do the same thing, which one works better?
void conditionArrQuotes(CStringArray* in, CString strDelimiters, CStringArray* out);
bool SplitStringArr(CString strInputString, const CString& strDelimiter, CStringArray* arrStringArray, bool isKeepDoubleSpace);
int SplitString(CString strInputString, const CString& strDelimiter);
CStringArray* MyTokenizer_Prob(const CString& str, const CString& strDelimiters);
CStringArray* CString2LinesBySpace(CString& str);
CStringArray* CString2LinesByComma(CString& str);

bool gz_poly(double *xs, double *zs, int nstn, double *xv, double *zv, double xsTmp[], double zsTmp[], 
	int nvert, double den, double *grav_z, bool isOutputReset);
bool gz_poly_old(double *xs, double *zs, int nstn, double *xv, double *zv, int nvert,
	double den, double *grav_z, bool isOutputReset);

bool is_inPoly(float *digx,				/* dig.x[] is the x points in polygon */
	float *digy,				/* dig.t[] is the y points in polygon */
	int npoints,                       /*   number of points in polygon        */
	float xt,                   /*   x (horizontal) of target point     */
	float yt);                   /*   y (vertical) of target point       */

bool m_poly(double *xs, double *zs, int nstn, double *xv, double *zv, int nvert,
	double he_suscept,
	double anginc,  //  0
	double angstr,  // 0, angles in degrees
//	double suscept,
	double *anom_z,
	double *anom_x,
	double *anom_t);






bool IsPointInside(CPoint *p, CRect *logicalAxes);

int IsPointAbove(float x1, float y1, float x2, float y2, float px, float py);
int IsPointAbove(SEI_PAIR p1, double x2, double y2, double px, double py);
int IsPointAbove(SEI_PAIR p1, double x2, double y2, SEI_PAIR p);
int IsPointAbove(SEI_QUAD line, SEI_PAIR p);
bool IsPointInBoundingBox(SEI_QUAD line, SEI_PAIR p);
bool LineSegmentIntersection(SEI_QUAD l1, SEI_QUAD l2, SEI_PAIR* intersection,
	double *m1, double *c1, double *m2, double *c2);
bool LineSegmentIntersection(SEI_QUAD l1, SEI_QUAD l2, SEI_PAIR* intersection);
bool LineIntersection(float ax1, float ay1, float ax2, float ay2,
	float bx1, float by1, float bx2, float by2,
	float* intersection_X, float* intersection_Y);


bool IsPointInBoundingBox2_Depreciated(float x1, float y1, float x2, float y2, float px, float py);
bool LineSegmentIntersection2_Depreciated(float l1x1, float l1y1, float l1x2, float l1y2,
                            float l2x1, float l2y1, float l2x2, float l2y2,
                            float *m1, float *c1, float *m2, float *c2,
                            float* intersection_X, float* intersection_Y);


// int myClipPoints2Old(int n, double x[], double z[], double xmin, double xmax, double zmin, double zmax);
//int myClipPoints2(int n, float x[], float z[], double xmin, double xmax, double zmin, double zmax);
//int myClipPoints2OLD(int n, int nLimit, double *x, double *z, double xmin, double xmax, double zmin, double zmax);
//int myClipPoints2Depreciate(int n, int nLimit, double *x, double *z, double xmin, double xmax, double zmin, double zmax);
int myClipPoints2(int n, int nLimit, double x[], double z[], CRect seiq);
int myClipPoints2(int n, int nLimit, double x[], double z[], SEI_QUAD seiq);
int myClipPointsX(int n, int nLimit, double x[], double z[], SEI_PAIR seip);
//int myClipPointsX(int n, int nLimit, double *x, double *z, double xmin, double xmax);
int myClipPointsZ(int n, int nLimit, double x[], double z[], SEI_PAIR seip);

double FindShortestDistanceToSegment(double x1, double y1, double x2, double y2, double pointX, double pointY);

CTime ctringToCTime(CString str);
CString dateTimeToCString(CTime aTime);



// XML common functions
CString ShowError(MSXML::IXMLDOMDocumentPtr pXMLDocument);
MSXML::IXMLDOMNodePtr myGetSingleChildByName(MSXML::IXMLDOMNodePtr pParent, CString name);
bool Is_Having_Attribute(MSXML::IXMLDOMNamedNodeMapPtr pAttrs, _bstr_t attr);
_variant_t getAttributeDef(MSXML::IXMLDOMNamedNodeMapPtr pAttrs, _bstr_t attr, _variant_t val);
_variant_t getAttributeDef(MSXML::IXMLDOMNodePtr pAttr, _variant_t val);
// _variant_t getAttributeDef(MSXML::IXMLDOMNodePtr pNode, _bstr_t attr, _variant_t val);

_variant_t getAttributeDef(MSXML::IXMLDOMElementPtr pAttr, _bstr_t attr, _variant_t val);
// end XML functions


void appendPlacemarkLine(MSXML::IXMLDOMDocument2Ptr pXMLDoc, MSXML::IXMLDOMElementPtr pElem,
	double lon[], double lat[], int np, CString aName, CString aStyle);
void appendPlacemarkPoint(MSXML::IXMLDOMDocument2Ptr pXMLDoc, MSXML::IXMLDOMElementPtr pElem,
	double lon, double lat, double depth, CString aName, CString aStyle);
void appendPlacemarkStyle(MSXML::IXMLDOMDocument2Ptr pXMLDoc, MSXML::IXMLDOMElementPtr pElem,
	CString styleMidName, double labelSize);



// for auto program online update
BOOL IsUserInAdminGroup();
BOOL IsRunAsAdmin();
BOOL IsProcessElevated();
DWORD GetProcessIntegrityLevel();
void ReportError(LPCWSTR pszFunction, DWORD dwError);
void ReportError(LPCWSTR pszFunction);
BOOL DoElevateToAdmin();
// end update functions


int totDays(int nYear, int nDay, CString t);

// https://stackoverflow.com/questions/30349214/bitmap-padding-bytes-in-opengl-pixelbufferobject
// Bitmap may need zero padding for each row
inline int conditionPowerTwo(int in) {
	int out = 1024;
	while (out < in) {
		out += 4;
	}
	return out;
}



inline BOOL myMakeSameNetwork(CString& fName, CString fNameRef) {
	// this function is not tested since I do not have a network path to test yet. But it should work
	if (fName.Trim().GetLength() < 4) return FALSE;
	fName = fName.Trim();
	fNameRef = fNameRef.Trim();
	if (fNameRef.GetLength() < 4) return TRUE;
	if (fNameRef.Find(_T("\\\\")) != 0) return FALSE;
	int i = fNameRef.Find(_T("\\"), 2);
	if (i < 0) return FALSE;
	CString networkPathRootRef = fNameRef.Mid(0, i);

	if (fName.Find(_T("\\\\")) != 0) return FALSE;
	int j = fNameRef.Find(_T("\\"), 2);
	if (j < 0) return FALSE;
	fName = networkPathRootRef + fName.Mid(j);
	return TRUE;
}

inline BOOL myMakeSameDrive(CString& fName, CString fNameRef)
{
	if (fName.Trim().GetLength() < 2) return FALSE;
	if (fNameRef.Trim().GetLength() < 2) return TRUE;
	if (fNameRef.Trim().Find(_T("\\\\")) == 0)
		return myMakeSameNetwork(fName, fNameRef);

	if (fName.GetAt(0) == fNameRef.GetAt(0)) return TRUE;
	fName.SetAt(0, fNameRef.GetAt(0));
	return TRUE;
}

//double *shepard_basis_1d(int nd, double xd[], double p, int ni, double xi[]);
////double *shepard_value_1d(int nd, double xd[], double yd[], double p, int ni, double xi[]);
//void shepard_value_1d(int, double[], double[], double, int, double[], double[]);
