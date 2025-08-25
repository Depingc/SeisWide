#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "float.h"
#include "segy.h"
#include "string.h"
#include "suSwapByte.h"
#include "ealloc_d.h"
#include "util.h"



BOOL isTimeDomainUtil(int mode) {
	//if true then in time domain.
	return mode<=0 || 	mode==5 ||
		mode==6 ||
		mode==7 ||
		mode==8 ||
		mode==9  ||
		mode==10
		; 
 
	// Note:
	// mode==10 can mean both a time or a depth domain depending on draw.labelAsTWTT.
	// but all other modes has only 1-to-1 fixed time or depth domain relationship
}


/* Assumes sizeof(int) == 4 

	ibm_to_float((int *) tr.data,
			     (int *) tr.data, ns,
			     endian);
*/
void ibm_to_float(int from[], int to[], int n)
{
    register int fconv, fmant, i, t;

    for (i=0;i<n;++i) {
		fconv = from[i];

		// now the result is for ieee float in big-endian machine. We need to convert to little endian
		//to[i] = (fconv<<24) | ((fconv>>24)&0xff) |
	//		((fconv&0xff00)<<8) | ((fconv&0xff0000)>>8);

		/* next lines modified (M.J.Rutty 20/9/92) */
		/* if (fconv) { */
		/* fmant = 0x00ffffff & fconv; */

		fmant = 0x00ffffff & fconv;
		if (!fmant)
		  fconv=0;
		else {
		  /* end modifications */
				t = (int) ((0x7f000000 & fconv) >> 22) - 130;
				while (!(fmant & 0x00800000)) { --t; fmant <<= 1; }
				if (t > 254) fconv = (0x80000000 & fconv) | 0x7f7fffff;
				else if (t <= 0) fconv = 0;
				else fconv = (0x80000000 & fconv) |(t << 23)|(0x007fffff & fmant);
			}

		to[i] = fconv;
    }
    return;
}

void float_to_ibm(int from[], int to[], int n)
{

	register int fconv, fmant, i, t;

    for (i=0;i<n;++i) {
		fconv = from[i];


		// now convert to ibm format
		if (fconv) {
				fmant = (0x007fffff & fconv) | 0x00800000;
				t = (int) ((0x7f800000 & fconv) >> 23) - 126;
				while (t & 0x3) { ++t; fmant >>= 1; }
				fconv = (0x80000000 & fconv) | (((t>>2) + 64) << 24) | fmant;
			}

		// for little endian machine only
 		//fconv = (fconv<<24) | ((fconv>>24)&0xff) |
 		//	((fconv&0xff00)<<8) | ((fconv&0xff0000)>>8);
		
		to[i] = fconv;
    }
	//SwapN((float *)&to[0], n);
    return;
}

#if defined(TEST_IBM)
main()
{
    float f;
    int i;

    f = 1.0; i = 0;
    float_to_ibm(&f,&i,1);
    printf("f = %f -> 0x%x\n",f,i);

    i = 0x41100000; f = 0.0;
    ibm_to_float(&i,&f,1);
    printf("i = 0x%x -> f = %f\n",i,f);
    exit(0);
}
#endif


void convSimple (int lx, int ifx, float *x,
	   int ly, int ify, float *y,
	   int lz, int ifz, float *z)
{
	// this function has problems
	int ilx=ifx+lx-1,ily=ify+ly-1,ilz=ifz+lz-1,i,j,jlow,jhigh;
	float sum;
	
	x -= ifx;  y -= ify;  z -= ifz;
	for (i=ifz; i<=ilz; ++i) {
		float denom1 = 0.f;
		float denom2 = 0.f;
		int count1 = 0;
		int count2 = 0;

		jlow = i-ily;  if (jlow<ifx) jlow = ifx;
		jhigh = i-ify;  if (jhigh>ilx) jhigh = ilx;
		for (j=jlow,sum=0.0; j<=jhigh; ++j) {
			sum += x[j]*y[i-j];

			// deping's modifications
			denom1 += x[j]*x[j];
			if (fabs(x[j]) > 0.f) count1++;
			denom2 += y[i-j]*y[i-j];
			if (fabs(y[i-j]) > 0.f) count2++;
		}

		if (count1 > 1) denom1 /= (float)count1;
		if (count2 > 1) denom2 /= (float)count2;

		z[i] = sum;
		sum = denom1 * denom2;
		 if (sum > 0.f) z[i] /= sqrt(sum);
	}
}











// get reduced time in sec based on offset information. NMO display is not 
float getReduceTimeEx(float offset, int drawmode, float vred)
{
	// will use a few global variable:
	// draw.mode, para.rvVred, para.waterDepth, para.gunDepth, para.obsDepth
	if (drawmode == 6) return 0.f;  // this is trace display, without any reducing.
	if (drawmode == 10) return 0.f;  

	double rvVred = !isFloatEqual(vred, 0.0f)  ?   1./vred  : 0.0;
	//double rvVred = (vred > 0.1 && vred < 10.0)  ?   1./vred  : 0.0;
	double reducedTime = 0.0;
	if (drawmode==0) {
		reducedTime = rvVred * fabs(offset);	   //Apply reduction velocity.
	}

	return (float)reducedTime;
}

float getReduceTimeEx(float offset, int drawmode, float vred, float vWater, float Zwater, float Zobs, float Zgun)
{
	if (drawmode != 7) return getReduceTimeEx(offset, drawmode, vred);

	double rvVred = !isFloatEqual(vred, 0.0f)  ?   1./vred  : 0.0;
	if (Zwater<-10.f || Zwater>15.f) {
		return 0.f;
	}

	double reducedTime = 0.0;
	// double tmpGun = waterDepth;
	double dz = Zwater - Zobs;
	double x2 = offset * offset * 0.25; // half offset squared
	reducedTime=sqrt(x2+ Zwater*Zwater) + sqrt(x2 + dz*dz) - Zwater*2.;
		// note: the above is the John Shilmald display for NMO
	reducedTime*=rvVred;

	// a final correction, obsDepth~0.06km
	if (vWater < 0.01 || vWater>2.0) vWater = 1.441f;
	double rvVwater = 1. / vWater;
	reducedTime -= Zobs * (rvVwater - rvVred);

	// the last correction is for airgun depth, around 0.011km
	reducedTime -= 2.0 * Zgun * rvVwater;



	return (float)reducedTime;
}



/**  UTM computation starts **/
const double FalseEasting = 500000.0;
double ScaleFactor = 0.9996;
double EarthRadius = 6378137.0;
double utmGetScaleFactor(double e)
{
	return ScaleFactor / cos(e / EarthRadius);
}

double utmSimpsons(double fromE, double fromN, double toE, double toN)
{
			double f1 = utmGetScaleFactor(fromE - FalseEasting);
			double f2 = utmGetScaleFactor(toE - FalseEasting);
			double fm = utmGetScaleFactor((fromE + toE) * 0.5 - FalseEasting);
			double dx = (toE - fromE) / 6 * (f1 + 4 * fm + f2);
			double dy = toN - fromN;
			return sqrt(pow(dx, 2) + pow(dy, 2));
}
/**  end UTM computation starts  **/


CString getSystemTempPath()
{
	TCHAR lpszSystemInfo[MAX_PATH];
	GetTempPath(MAX_PATH, lpszSystemInfo);
	return CString(lpszSystemInfo);
}


// Not including the trailing back slash
CString getSeisWidePath()
{
    CString name;
    int nPathLength =  GetModuleFileName (NULL,name.GetBuffer(_MAX_PATH), _MAX_PATH );
	name.ReleaseBuffer();
	CString progPathBak = name.Left(name.ReverseFind( '\\' ));
	CString progPath; 
	BOOL isDebug = progPathBak.Find(_T("Debug")) > 0;
	if (isDebug) {
		progPath = progPathBak + _T("\\..\\docs\\SeisWide7.1");
	} else {
		progPath = progPathBak;
	}

	if (!isDirectoryExist(progPath)) 
		progPath = getSystemTempPath();

	return progPath;
}


CString getGravmodPath() 
{
    CString name;
    int nPathLength =  GetModuleFileName (NULL,name.GetBuffer(_MAX_PATH), _MAX_PATH );
	name.ReleaseBuffer();
	CString progPathBak = name.Left(name.ReverseFind( '\\' ));
	CString progPath; 
	BOOL isDebug = progPathBak.Find(_T("Debug")) > 0;
	if (isDebug) {
		progPath = progPathBak + _T("\\..\\docs\\SeisWide7.1\\others\\psGravmod\\gravExample");
	}
	else {
		progPath = progPathBak + _T("\\others\\psGravmod\\gravExample");
	}

	if (!isDirectoryExist(progPath)) 
		return getSystemTempPath();

	return progPath;
}

BOOL IsUnix(short a) {
	return !(a>=0 && a<6); // here if the headers are all zeros (an abnormal situation), I think it should be PC format
}

BOOL IsFormatValid(short a) {
	return (a>=0 && a<6);  // here if the headers are all zeros (an abnormal situation), I think it should be PC format
}




unsigned short Swap(unsigned short a){
	return ((a>>8) & 0x00ff)+
		((a<<8) & 0xff00);
}
short Swap(short a){
	return ((a>>8) & 0x00ff)+
		((a<<8) & 0xff00);
}
int Swap(int a){
	return ((a>>24) & 0x000000ff) +
		((a>>8)  & 0x0000ff00) +
		((a<<8)  & 0x00ff0000) +
		((a<<24) & 0xff000000);
}

unsigned int Swap(unsigned int a){
	return ((a>>24) & 0x000000ff) +
		((a>>8)  & 0x0000ff00) +
		((a<<8)  & 0x00ff0000) +
		((a<<24) & 0xff000000);
}
void Swap(float* tnf4)
{  // please test if this functions work ok	
	int *tni4=(int *)tnf4;
	*tni4=(((*tni4>>24)&0xff) | ((*tni4&0xff)<<24) |
		((*tni4>>8)&0xff00) | ((*tni4&0xff00)<<8));  
}
void SwapN(short* a,const int n){
	// swaps n shorts in place
	for (int i=0;i<n;i++){
		int b=a[i];
		a[i]= ((b>>8) & 0x00ff) +
			((b<<8) & 0xff00);
	}
}
void SwapN(unsigned short* a,const int n){
	// swaps n shorts in place
	for (int i=0;i<n;i++){
		int b=a[i];
		a[i]= ((b>>8) & 0x00ff) +
			((b<<8) & 0xff00);
	}
}

void SwapN(int* a,const int n){
	// swaps n ints in place
	for (int i=0;i<n;i++){
		a[i]=(((a[i]>>24)&0xff) | ((a[i]&0xff)<<24) |
			((a[i]>>8)&0xff00) | ((a[i]&0xff00)<<8));  
	}
}

void SwapN(long* a,const int n){
	// swaps n longs in place. tested, works great! June 30, 2006
	for (int i=0;i<n;i++){
		a[i]=(((a[i]>>24)&0xff) | ((a[i]&0xff)<<24) |
			((a[i]>>8)&0xff00) | ((a[i]&0xff00)<<8));  
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
void SwapN(unsigned long* a,const int n){
	// swaps n longs in place
	for (int i=0;i<n;i++){
		int b=a[i];
		a[i]= ((b>>24) & 0x000000ff) +
			((b>>8)  & 0x0000ff00) +
			((b<<8)  & 0x00ff0000) +
			((b<<24) & 0xff000000);
	}
}
void SwapN(float* tnf4,const int n)
{  // please test if this functions work ok	
	int *tni4;
	for (int i=0;i<n;i++){
		tni4=(int *)&tnf4[i];
		*tni4=(((*tni4>>24)&0xff) | ((*tni4&0xff)<<24) |
			((*tni4>>8)&0xff00) | ((*tni4&0xff00)<<8));  
	}  
}
void SwapTapeHeader(bhed* bh){
	short *i2buf = (short *) &bh->jobid;
	int *i4int = (int *) &i2buf[0];
	swap_int_4(&i4int[0]);
	swap_int_4(&i4int[1]);
	swap_int_4(&i4int[2]);
	swap_short_2(&i2buf[6]);
	swap_short_2(&i2buf[7]);  // short nart

	int i;
	for (i=8; i<12; i++)
		swap_u_short_2((unsigned short *)&i2buf[i]);
	for (i=12; i<42; i++)
		swap_short_2(&i2buf[i]);

	swap_float_4(&bh->lattCorr); 
	swap_float_4(&bh->longCorr); 
	swap_float_4(&bh->drftrate); 
	swap_float_4(&bh->obsPositionX); 

	for (i=44; i<200; i++)
		swap_short_2(&i2buf[i]);
}
void SwapN(segy * tr, short format){
	/* data sample format code:
		1 = floating point (4 bytes)
		2 = fixed point (4 bytes)
		3 = fixed point (2 bytes)
		4 = fixed point w/gain code (4 bytes) 
	*/
	if (format==1) SwapN((float *)&tr->dat.data[0], tr->ns);
	else
		if (format==2) SwapN((int *)&tr->dat.dataint[0], tr->ns);
	else
		if (format==3) SwapN((short *)&tr->dat.datashort[0], tr->ns);
}

void SwapTraceHeader(BOOL isUseReducedTime, BOOL isDeadTrcExist, segy* tr){
	short *i2buf = (short *) &tr->tracl;
	int *i4int = (int *) &i2buf[0];
	int i;
	
	for (i=0; i<7; i++)
		swap_int_4(&i4int[i]);
	swap_short_2(&tr->trid);
	swap_short_2(&tr->nvs);
	swap_short_2(&tr->nhs);  // i2buf[16]
	swap_short_2(&tr->duse);  // i2buf[17]    data use: 1 = production; 2 = test
	for (i=9; i<17; i++)
		swap_int_4(&i4int[i]);
	swap_short_2(&i2buf[34]);
	swap_short_2(&i2buf[35]);

	//i4int[18:21] is at i2buf[36:42]
	for (i=18; i<22; i++)
		swap_int_4(&i4int[i]);

	// [54] is delrt
	for (i=44; i<57; i++)
		swap_short_2(&i2buf[i]);

	// [57] is unsigned short ns
	// [58] is unsigned short dt
	for (i=57; i<59; i++)
		swap_u_short_2((unsigned short *)&i2buf[i]);

	// [59] is short gain
	for (i=59; i<90; i++)
		swap_short_2(&i2buf[i]);

// need to change here
	swap_float_4(&tr->d1); 
	swap_float_4(&tr->f1); 
	swap_float_4(&tr->d2); 
	swap_float_4(&tr->f2); 
	swap_float_4(&tr->ungpow); 
	swap_float_4(&tr->unscale); 

//refraction reduced time  microsec  is disabled!
//	swap_int_4(&i4int[45]); //refraction reduced time  microsec.
	swap_int_4(&tr->ntr); // ntr, num of traces
	swap_short_2(&tr->mark);

	if (isUseReducedTime) {
		swap_int_4(&tr->t1); //refraction reduced time.
	}
	else {
		tr->t1 = 0; 
	}
	if (!isDeadTrcExist) tr->trid = 1;  // all dead traces become activated.
	
	swap_short_2(&tr->idistopt);
	swap_float_4(&tr->trcdist); 

	// note: i2buf[109] stores the azimuth by Ian
}

void SwapTraceHeader(segy* tr){
	SwapTraceHeader(false, false, tr);
}


CTime getFileModifiedTime(CString f)
{
	if (!isFileExist(f)) return NULL;

	CFile cfile;
	CFileStatus status;
	CTime t;
	cfile.Open(f, CFile::modeRead);
	if (cfile.GetStatus(status) ) {
		t = status.m_mtime;
	}
	cfile.Close();
	return t;
}

// Does File exist and is not a directory
BOOL isFileExistNotDirectory(CString name)
{
	LPCTSTR lpszFileName = name.GetBufferSetLength(name.GetLength());
   DWORD attrib;
   attrib = GetFileAttributes(lpszFileName);
   return((attrib != 0xFFFFFFFF) && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}
BOOL isFileExist(CString name)
{
	LPCTSTR lpszFileName = name.GetBufferSetLength(name.GetLength());
   DWORD attrib;
   attrib = GetFileAttributes(lpszFileName);
   return (attrib != 0xFFFFFFFF) ;
}
BOOL isDirectoryExist(CString name)
{
	LPCTSTR lpszFileName = name.GetBufferSetLength(name.GetLength());
   DWORD attrib;
   attrib = GetFileAttributes(lpszFileName);
   return (attrib != INVALID_FILE_ATTRIBUTES);
}

BOOL isFileNewer(CString strTarget, CString strCompare) 
{
	// will return FALSE is any files have problems. 
	// will return TRUE only if the target is dated newer (or same time) than the other file
	CFileStatus statusTarget;
	BOOL isExistTarget = CFile::GetStatus( strTarget, statusTarget ); // static function
	CFileStatus statusCompare;
	BOOL isExistCompare = CFile::GetStatus( strCompare, statusCompare ); // static function
	if(!isExistTarget || !isExistCompare) {
		return FALSE;
	}

	if (statusTarget.m_mtime >= statusCompare.m_mtime)
		return TRUE;

	return FALSE;
//	CTime curTime = CTime::GetCurrentTime();

}


// since the path may be changed, we need to adjust path for a few other files according to the current .sei file
CString getSecNameRevised(CString secName, CString seiName)
{
	// need to determine if the SEI file and SGY files are moved to a another directory. If so, revise.
	if (seiName.IsEmpty() || secName.IsEmpty() || isFileExistNotDirectory(secName) ) return secName; // no changes
	if (!isFileExistNotDirectory(seiName)) return secName; // no changes

	CString t;

	// if there are no extension, we do not revise. This will help prevent miss-use such as secName="a" et al.
	t = GetFileExtension(secName);
	if (t.GetLength() < 2) return secName;

	// if the seiName and secName are at diferent drive, we first change secName to that drive, and test again
	if (seiName.GetAt(0) != secName.GetAt(0)) {
		secName.SetAt(0, seiName.GetAt(0));
		if (isFileExistNotDirectory(secName)) return secName;
	}


	// need to get the starting directory to search for file, based on seiName
	t = GetPathFromFile(seiName);
	CString outName;
	SearchAllFiles(t + _T("*.*"), secName, outName);
	if (outName != secName) {
		int tte = 1;
	}
	if (isFileExist(outName)) return outName;

	// now the file not found in new dir, so we just keep the original sec file.
	return secName;
}

// strAppend will be inserted to the original filename while keeping the same extension
CString getFileNameAppend(CString aName, CString strAppend)
{
	// if (aName == NULL) return strAppend;
	CString tStr;
	int pathLen = aName.ReverseFind( '.' );
	if(pathLen>(-1))
	{
		CString t = aName.Left(pathLen);
		tStr.Format(_T("%s%s%s"), t, strAppend, aName.Mid(pathLen) );
		return tStr;
		// return t + strAppend + aName.Mid(pathLen);
	}

	return aName + strAppend;
}

BOOL isEndWith(CString name, CString str)
{
	if ( name.Right(str.GetLength()) == str) return TRUE;
	return FALSE;
}

BOOL waitForFileUnlocked(CString f)
{
	int numMaxCheck = 10;
	int count = 0;
	CFile file;
//	if (!file.Open(f, CFile::modeRead | CFile::typeBinary)) {
	while(!file.Open(f, CFile::modeRead))
	{
		// here, you should check if the error is "Access denied"
		// and probably break if it is anything else...
		Sleep(1000);
		count++;
		if (count>numMaxCheck) {
			// we give up after many times of trying
			return FALSE;
		}
	}
	file.Close();
	return TRUE;
}


// provide an output of a temporary file name under any specified sub-dir under a tree with an existing file 
// strExt: will be appended as an extension under /temp to the original filename 
//***************this has problem of double dots, pls fix.****************************//
CString getFileNameSubdir(CString aName, CString aSubdir, CString strExt, bool isCreate)
{
	CString tStr;
	strExt.Trim();
	aSubdir.Trim();
	if (strExt.GetAt(0) != '.') strExt = _T(".") + strExt;
	int pathLen = aName.ReverseFind( '\\' );
	if(pathLen>(-1))
	{
		CString txPath;
		if (!isEndWith(aName.Left(pathLen), aSubdir))
			txPath.Format(_T("%s\\%s"), aName.Left(pathLen), aSubdir);
		else
			txPath.Format(_T("%s"), aName.Left(pathLen));
		if(!isDirectoryExist(txPath)) {
			// create the sub dir
			if (!isCreate) return ""; // since we are not allowed to create anything, we return blank
			if (!CreateDirectory(txPath, NULL)) {
				// failed to create the sub dir. We use system temp dir instead
				tStr.Format( _T("%s\\%s.%s"), getSystemTempPath(), GetBaseFileName(aName), strExt);
				return tStr;
			}
		}

		// now the sub dir exists
		int pathLen2 = aName.ReverseFind( '.' );
		if (pathLen2 > -1) {
			tStr.Format(_T("%s\\%s%s"), txPath, aName.Mid(pathLen+1, pathLen2-pathLen-1), strExt);
		}
		else
			tStr.Format(_T("%s\\%s%s"), txPath, aName.Mid(pathLen+1), strExt);
		//	return txPath + _T("\\") + aName.Mid(pathLen+1) + _T("." + strExt;
		return tStr;
	}

	// cannot find the path
	tStr.Format(_T("%s%s"), aName, strExt);
	return tStr;
}

CString getFileNameTemp(CString aName, CString strExt)
{
	return getFileNameSubdir(aName, _T("temp"), strExt, true);
}

CString getFileNameTemp(CString aName, CString strExt, BOOL isOverwrite)
{
	CString t = getFileNameTemp(aName, strExt);

	if (!isOverwrite) {
		// now the file exists, we want to make a different filename
		while (isFileExist(t)) {
			t = getFileNameAppend(t, _T("a"));
		}
	}
	return t;
}


// GetBaseFileName
// This helper function returns file name without extension
CString GetBaseFileName(CString sFileName)
{
  CString sBase = sFileName;
  int pos1 = sFileName.ReverseFind('\\');
  if(pos1>=0)
    sBase = sBase.Mid(pos1+1);

  int pos2 = sBase.ReverseFind('.');
  if(pos2>=0)
  {
    sBase = sBase.Mid(0, pos2);
  }
  return sBase;
}

// note: this will return a path ended with "\", so do not add another "\" to your file if you concat
CString GetPathFromFile(CString sFileName) {
  CString sBase = sFileName;
  int pos1 = sFileName.ReverseFind('\\');
  if(pos1>=0)
    sBase = sBase.Left(pos1+1);
  
  return sBase;
}

// GetFileExtension
// This helper function returns file extension by file name
CString GetFileExtension(CString sFileName)
{
  CString sExt;
  int pos = sFileName.ReverseFind('.');
  if(pos>=0)
  {
    sExt = sFileName.Mid(pos+1);
  }
  return sExt.MakeLower();
}

// this helper function returns modified file name with appended string and revised extension, and
// positioned in the standard temp directory of SeisWide
CString GetFileNameAppendExt(CString sFileName, CString strAppend, CString strExt) {
	return getFileNameTemp( getFileNameAppend(sFileName, strAppend), strExt );
}


BOOL setCurrentDirectoryFromFile(CString name)
{
	int pathLen = name.ReverseFind( '\\' );
	CString txPath = name.Left(pathLen);
	if(pathLen>(-1))
	{
		SetCurrentDirectory(txPath);
		return true;
	}
	return false;
}



void getAllPathArray(CStringArray *m_arr, CString aPath) {
	// aPath must be absolute path, or with wildcard
	CString wildPath = (aPath.Find(_T("*"), 0) >= 0) ? aPath : aPath + _T("\\*.*");
	
	CFileFind *finder = new CFileFind();
	BOOL bResult = finder->FindFile(wildPath);
	while (bResult)
	{
		   bResult = finder->FindNextFile();
		   CString aName = (LPCTSTR) finder->GetFileName();
		   if (aName == _T(".") || aName == _T("..")) continue;
		   CString aPath2 = (LPCTSTR) finder->GetFilePath();
		   m_arr->Add(aPath2);

		   if (!isFileExistNotDirectory(aName)) {
				getAllPathArray(m_arr, aPath2);
		   }
	}
	finder->Close();
}

CStringArray* getAllPathArray(CString aPath) {
	// aPath2 must be absolute path
	CStringArray 	*m_arr = new CStringArray();				// CString Array to hold Directory Structures
	m_arr->RemoveAll();					// Not really necessary - Just habit

	getAllPathArray(m_arr, aPath);

	return m_arr;
}


CStringArray* getLinesFromFile(CString aFile) {
	// aFile must have absolute path
	if (!isFileExistNotDirectory(aFile)) return NULL;

	FILE *fp;
	_TCHAR str [255];
	if (_tfopen_s(&fp, aFile, _T("r")) != 0) return NULL;
	CString cstr;
	CStringArray 	*m_arr = new CStringArray();				// CString Array to hold Directory Structures
	while (true) {
		TRY {
			if (_fgetts(str,255,fp) == NULL ) break;
			int len = (int)_tcslen(str);
			if (len <= 1 || str[0] == '#') continue; // we do not need comment lines
			cstr.Empty();
			cstr.Append(str);
			cstr.Replace(_T("\t"), _T(" "));
			cstr.Remove(_T('\r'));
			cstr.Remove(_T('\n'));
			cstr.Trim();
			if (cstr.GetLength() > 1) {
				m_arr->Add(cstr);
			}
		}
		CATCH_ALL(e)
		{
		}
		END_CATCH_ALL
	}
	fclose(fp);

	return m_arr;
}

void getLinesFromFile(CStringArray* m_arr, CString aFile, bool isNeedComments) {
	// aFile must have absolute path
	if (!isFileExistNotDirectory(aFile)) return;

	FILE *fp;
	TCHAR str [255];
	if (_tfopen_s(&fp, aFile, _T("r")) != 0) return;
	while (true) {
		TRY {
			if (_fgetts(str,255,fp) == NULL ) break;
			if (!isNeedComments && str[0] == '#') continue; // we do not need comment lines
			m_arr->Add(str);
		}
		CATCH_ALL(e)
		{ }
		END_CATCH_ALL
	}
	fclose(fp);
}

CString getLineFirst(CString aFile) {
	// aFile must have absolute path
	if (!isFileExistNotDirectory(aFile)) return "";

	FILE *fp;
	TCHAR str [255];
	if (_tfopen_s(&fp, aFile, _T("r")) != 0) return "";
	while (true) {
		TRY {
			if (_fgetts(str,255,fp) == NULL ) break;
			if (str[0] == '#') continue; // we do not need comment lines
			break;
		}
		CATCH_ALL(e)
		{ }
		END_CATCH_ALL
	}
	fclose(fp);
	return CString(str); 
}

CString getLineFirstComment(CString aFile) {
	// aFile must have absolute path
	if (!isFileExistNotDirectory(aFile)) return "";

	FILE *fp;
	TCHAR str [255];
	CString cstr;
	if (_tfopen_s(&fp, aFile, _T("r")) != 0) return "";
	while (true) {
		TRY {
			if (_fgetts(str,255,fp) == NULL ) break;
			cstr = CString(str);
			cstr.Trim();
			if (cstr.GetLength() < 2) {
				continue;  // the comment line must have more than 1 characters
			}

			if (cstr[0] != '#') break; // we need comment line as the first line
			break;
		}
		CATCH_ALL(e)
		{ }
		END_CATCH_ALL
	}
	fclose(fp);
	return cstr; 
}

bool saveStringArrayToFile(CStringArray* m_arr, CString aFile) {
	// aFile must have absolute path
	FILE *fp;
	if (_tfopen_s(&fp, aFile, _T("w")) != 0) return false;

	for (int i=0; i<m_arr->GetCount(); i++) {
		_ftprintf(fp, _T("%s\n"), m_arr->ElementAt(i));
	} // for i
	fclose(fp);
	return true;
}

bool showStringArrayToFile(CStringArray* m_arr) {
	CString newName = getSystemTempPath() + _T("seiTempFile.txt");
	bool out = saveStringArrayToFile(m_arr, newName);
	showFileInNotePad(newName);
	return out;
}


CStringArray* getLinesFromFileEndWith(CString aFile, float vPerturb) {
	// aFile must have absolute path
	if (!isFileExistNotDirectory(aFile)) return NULL;

	FILE *fp;
//	char str [255];
	CString cstr;
	float val;
	if (_tfopen_s(&fp, aFile, _T("r")) != 0) return NULL;
	CStringArray 	*m_arr = new CStringArray();				// CString Array to hold Directory Structures
	while (true) {
		TRY {
			
			if (_fgetts(cstr.GetBuffer(),255,fp) == NULL ) break;
			cstr.ReleaseBuffer();
			if (cstr.GetAt(0) == _T('#')) continue; // we do not need comment lines
			val = (float)_tstof(cstr.Right(6));
			if (fabs(val-vPerturb)<0.001f ) {
				m_arr->Add(cstr);
			}
		}
		CATCH_ALL(e)
		{ }
		END_CATCH_ALL

	} // while
	fclose(fp);

	return m_arr;
}

BOOL isTrcInRange(int trc, CStringArray *trcAll) {
	if (trcAll == NULL) return FALSE;
	CString strin;
	strin.Format(_T("%d"), trc);
	CString str;
	for (int i=0; i<trcAll->GetSize(); i++) {
		str = trcAll->ElementAt(i);
	//	if (str == NULL) continue; // this line causes exception. Why????
	//	int iii = str.Find(strin);
		if (str.Find(strin)==0) return TRUE;
	}
	return FALSE;
}

BOOL isTrcInRange(CString strin, CStringArray *trcAll) {
	if (trcAll == NULL) return FALSE;
	CString str;
	for (int i=0; i<trcAll->GetSize(); i++) {
		str = trcAll->ElementAt(i);
	//	if (str == NULL) continue; // this line causes exception. Why????
	//	int iii = str.Find(strin);
		if (str.Find(strin)==0) return TRUE;
	}
	return FALSE;
}

BOOL isValueDefined(float x, int n, float *xArray) {
	if (xArray==NULL || n<1) return FALSE;
	for (int i=0; i<n; i++) {
		if (x == xArray[i]) return TRUE;
	}

	return FALSE;
}


// set current path to be the same as specified in f. 
BOOL setCurrentPathSameAs(CString f)
{
	int pathLen = f.ReverseFind( '\\' );
	if(pathLen>(-1)) {
		SetCurrentDirectory(f.Left(pathLen));
		return TRUE;
	}

	return FALSE;
}

BOOL isFloatEqual(float a, float b) {
	// accurate up to 5 decimal points.
   if (fabs(a-b)<0.00001f)
	   return TRUE;
   else return FALSE;

}

BOOL isFloatEqual(double a, double b) {
	// accurate up to 3 decimal points.
   return isFloatEqual((float)a, (float)b);
}

BOOL isFloatAbsEqual(float a, float b) {
	// accurate up to 3 decimal points.
   if (fabs(fabs(a)-fabs(b))<0.00001f)
	   return TRUE;
   else return FALSE;

}

long long myRound(double x)
{
	// NOTE: then int is used as return, and the input is over 30mil or so, the return value is incorect. So I changed to long here
	// Note: if you print the result to a file with %d, it may not print antthing unless you cast it to (int) first.
	if(x>0.)
		return (long long)(x+0.5);
	else
		return (long long)(x-0.5);
}


double myFloat(double x)
{
	// this will trim the input decimal to only 5 decimal points.
	// otherwise the zoomed coords have too many decimal points for the user to use
	return myRound(x*100000.)*0.00001;
}

float myFloat(float x)
{
	// this will trim the input decimal to only 5 decimal points.
	// otherwise the zoomed coords have too many decimal points for the user to use
	//long long a = myRound(x*100000.);
	//double b = a * 0.00001;
	//float c = (float)b;
	return (float)(myRound(x*100000.)*0.00001);
}

bool isFloat1(double v, double vmin) {
	// can round to 1 decimal point against 2 decimal point
	// examples: 5.01 is not, 5.001 is. 
	return v>=vmin && myFloat1(v)==myFloat2(v);
}

float myFloat1(double x) {
	// round to 1 decimal point
	return (float)(myRound(x*10.)*0.1);
}

float myFloat2(double x) {
	// round to 2 decimal points
	return (float)(myRound(x*100.)*0.01);
}

float myFloat3(double x) {
	// round to 3 decimal points
	return (float)(myRound(x*1000.)*0.001f);
	//return (float)(floorf(x*1000.+0.5)*0.001);
}

float myFloat5(double x) {
	// round to 5 decimal points
	long long i = myRound(x*100000.);
	float a = (float)(i*0.00001 + 0.000001);
	return a;
}




  /**
     * Returns a "nice" number approximately equal to range Rounds
     * the number if round = true Takes the ceiling if round = false.
     *
     * @param range the data range
     * @param round whether to round the result
     * @return a "nice" number to be used for the data range
     */
    double niceNum(double range, bool round) {
		if (range < 0.) return -niceNum( fabs(range), round);

		// the following only works for positive numbers

        double exponent; /** exponent of range */
        double fraction; /** fractional part of range */
        double niceFraction; /** nice, rounded fraction */
 
        exponent = floor(log10(range));
        fraction = range / pow(10, exponent);
 
        if (round) {
                if (fraction < 1.5)
                    niceFraction = 1;
                else if (fraction < 3)
                    niceFraction = 2;
                else if (fraction < 7)
                    niceFraction = 5;
                else
                    niceFraction = 10;
        } else {
                if (fraction <= 1)
                    niceFraction = 1;
                else if (fraction <= 2)
                    niceFraction = 2;
                else if (fraction <= 5)
                    niceFraction = 5;
                else
                    niceFraction = 10;
        }
 
        return niceFraction * pow(10, exponent);
    }


// calc the approximate tick interval
//   nDiv: recommended number of ticks
//   amin, amax: bounds of the axis
//   returns the tick interval
float getTickRounded(double amin, double amax, int nDivMin, int nDivMax, double tickPrefered)
{
	double range = amax-amin;
	if (nDivMin<2) nDivMin = 2;
	if (nDivMax>50) nDivMax = 50;
	if (tickPrefered<= 0.00001) {
		// invalid tick, we use the middle
		tickPrefered = 2. * range / (nDivMin+nDivMax);
	}
	double out = (float)niceNum(tickPrefered, true);
	if (out< 0.00001f) out = 0.00001f;

	if (out < range/nDivMax ) out = range/nDivMax;
	else if (out > range/nDivMin ) out = range/nDivMin;
	if (out< 0.00001f) out = 0.00001f;
	
	out = (float)niceNum(out, false);

	return (float)out;
}

// xmin can be 1, which should be rounded to 0, and incremented by a regular tick value
// xmax cannot be rounded, otherwise 899 becomes 1000, which is not good
float getMinRounded(double amin, double amax, int nDiv) 
{
	double range = niceNum(amax-amin, false);
	if (nDiv<2) nDiv = 2;
	double tickSpacing = niceNum(range/(nDiv-1), true);
	return (float)(floor(amin / tickSpacing) * tickSpacing);
}

float getMaxRounded(double amin, double amax, int nDiv) 
{
	double range = niceNum(amax-amin, false);
	if (nDiv<2) nDiv = 2;
	double tickSpacing = niceNum(range/(nDiv-1), true);
	return (float)(ceil(amax / tickSpacing) * tickSpacing);
}


BOOL isInside(int ix, int ia, int ib) {
	if (ia < ib) {
		if (ix >= ia && ix <= ib) return TRUE;
	}
	else {
		if (ix >= ib && ix <= ia) return TRUE;
	}

	return FALSE;
}

BOOL isInsideFloat(float x, float a, float b) {
	if (myFloat(a) < myFloat(b)) {
		if (myFloat(x) >= myFloat(a) && myFloat(x) <= myFloat(b)) return TRUE;
	}
	else {
		if (myFloat(x) >= myFloat(b) && myFloat(x) <= myFloat(a)) return TRUE;
	}

	return FALSE;
}

BOOL isInsideDouble(double x, double a, double b) {
	if (a < b) {
		if (x >= a && x <= b) return TRUE;
	}
	else {
		if (x >= b && x <= a) return TRUE;
	}

	return FALSE;
}

BOOL isInsideDouble(double *x, float *a, float *b) {
	if ( *a < *b) {
		if (*x >= *a && *x <= *b) return TRUE;
	}
	else {
		if (*x >= *b && *x <= *a) return TRUE;
	}

	return FALSE;
}

BOOL isInsideFloat(float *x, float *a, float *b) {
	if ( *a < *b) {
		if (*x >= *a && *x <= *b) return TRUE;
	}
	else {
		if (*x >= *b && *x <= *a) return TRUE;
	}

	return FALSE;
}


BOOL isInsideEx(int ix, int ia, int ib) {
	if (ia < ib) {
		if (ix > ia && ix < ib) return TRUE;
	}
	else {
		if (ix > ib && ix < ia) return TRUE;
	}

	return FALSE;
}

BOOL isInsideExFloat(float x, float a, float b) {
	// note: in deciding if x<a , we need to consider only 3 decible points, otherwise the results are not very good
	if (myFloat(a) < myFloat(b)) {
		if (myFloat(x) > myFloat(a) && myFloat(x) < myFloat(b)) return TRUE;
	}
	else {
		if (myFloat(x) > myFloat(b) && myFloat(x) < myFloat(a)) return TRUE;
	}

	return FALSE;
}


float getDistance(segydynamic * tr){
	if (!tr) return 0.f;
	if (tr->idistopt == 8 || tr->idistopt == 9)
		return (float)tr->trcdist;
	else
		return (float)tr->cdp * 0.001f;
}


float getDistance(segy * tr){
	if (!tr) return 0.f;
	if (tr->idistopt == 8 || tr->idistopt == 9)
		return (float)tr->trcdist;
	else
		return (float)tr->cdp * 0.001f;
}

float getOffset(segy * tr) {
	if (!tr) return 0.f;
	if (abs(tr->trcOffset) >= 1.f) tr->trcOffset = 0.f;
	return (float)tr->offset * 0.001f + tr->trcOffset;
}

float getOffset(segydynamic * tr) {
	if (!tr) return 0.f;
	if (abs(tr->trcOffset) >= 1.f) tr->trcOffset = 0.f;
	return (float)tr->offset * 0.001f + tr->trcOffset;
}

void setOffset(segy * tr, float f) {
	if (tr != NULL) {
		tr->offset = (int)(f*1000.f);
		tr->trcOffset = (f*1000.f - tr->offset) * 0.001f;
	}
}

void setOffset(segydynamic * tr, float f) {
	if (!tr) return;
	if (tr != NULL) {
		tr->offset = (int)(f*1000.f);
		tr->trcOffset = (f*1000.f - tr->offset) * 0.001f;
	}
}


BOOL isXShotInSegyDefined2(float xshot) {
	BOOL a = !_isnan(xshot) ;
	BOOL c = _finite(xshot);
	BOOL b = !isFloatAbsEqual(xshot, 99999.f);
	return  a && b && c;
}

float getXShotInSegy(CString f) {
	float re = ERRORVAL;
	if (!isFileExist(f)) return re;
	bhed bh;
	//segy tr;
	BOOL isUnix = false;
	CFile file;
	if (!file.Open(f, CFile::modeRead | CFile::typeBinary)) {
	}				
	else {
		file.Seek(3200L, SEEK_SET);
		file.Read(&bh, 400);
		if (isUnix = IsUnix(bh.format)) SwapTapeHeader(&bh);
		re = bh.obsPositionX;

		file.Close();
	}
	return re;
}

bool setOBSPosForSegy(CString f, float xshot) 
{
	bool out = false;
	if (!isFileExistNotDirectory(f)) 
		return out;
	CFile file;
	if (!file.Open(f, CFile::modeReadWrite | CFile::typeBinary)) {
		return out;
	}

	file.Seek(3200L, CFile::begin);
	bhed bh;
	file.Read(&bh, 400);
	BOOL isUnix;
	if (isUnix = IsUnix(bh.format)) SwapTapeHeader(&bh);

	file.Seek(3284L, CFile::begin);  // this points actually to  bh.obsPositionX
	file.Write(&xshot, 4);
	out = true;
	file.Close();
	return out;
}

int getNumTraceInSegy(CString f) {
	CFile file;
	if (!file.Open(f, CFile::modeRead | CFile::typeBinary)) 
		return 0;

	return getNumTraceInSegy(&file);
}

int getNumTraceInSegy(CFile *file) {
	bhed bh;
	int num = 0;
	if (file != NULL) {
		ULONGLONG iold = file->Seek(0L, CFile::current);
		file->Seek(3200L, CFile::begin);
		file->Read(&bh, 400);
		if (IsUnix(bh.format)) SwapTapeHeader(&bh);
		int sampByte = (bh.format != 3) ?  4  :  2;
		int len = sampByte*bh.hns + 240;
		num = (int)myRound( (double)(file->GetLength() - 3600) / (double)len );
		file->Seek(iold, CFile::begin);
	}
	return num;
}

// extract cmp[]  and  dist[]  from depth segy file if it exist
// actual number of traces will be returned
int getCmpDistInSegy(CString f, int *cmp, float *dist, float *offset, int numMax) {
	if (cmp == NULL || dist == NULL) return false;
	if (numMax==0) numMax = 999999;

	bhed bh;
	segy tr;
	BOOL isUnix = false;
	int i = 0;
	CFile file;
	if (!file.Open(f, CFile::modeRead | CFile::typeBinary)) 
		return 0;

	file.Seek(3200L, SEEK_SET);
		file.Read(&bh, 400);
		if (isUnix = IsUnix(bh.format)) SwapTapeHeader(&bh);
		int len = (bh.format == 3) ? bh.hns*2    :    bh.hns*4;
		// note: here we cannot skip some headers since getDistance(&tr) will use header info at the end of it
		while (file.Read(&tr, 240L) == 240)
		{
			if (isUnix) SwapTraceHeader(&tr);
			cmp[i] = tr.cdp;
			dist[i] = getDistance(&tr);
			offset[i] = getOffset(&tr);
			//if (i==0 || !isFloatEqual(dist[i], dist[i-1])) {
			//	i++;
			//}
			// we try to avoid equal distance index
			i++;
			if (i >= numMax) break;  // no more traces can be read in
			file.Seek(len, CFile::current);
		} // while
	file.Close();
	return i;
}

int getFFIDInSegy(CString f, float *dist, int *ffid)
{
	bhed bh;
	segy tr;
	BOOL isUnix = false;
	int i = 0;
	CFile file;
	if (!file.Open(f, CFile::modeRead | CFile::typeBinary)) 
		return 0;
	file.Seek(3200L, SEEK_SET);
	file.Read(&bh, 400);
	if (isUnix = IsUnix(bh.format)) SwapTapeHeader(&bh);
	int len = (bh.format == 3) ? bh.hns*2    :    bh.hns*4;
	while (file.Read(&tr, 240L) == 240)
	{
		if (isUnix) SwapTraceHeader(&tr);
		ffid[i] = (tr.fldr > 0) ? tr.fldr  :  tr.ep;
		dist[i] = getDistance(&tr);
		if (i==0 || !isFloatEqual(dist[i], dist[i-1])) {
			i++;
		}
		// we try to avoid equal distance index
		file.Seek(len, CFile::current);
	} // while
	file.Close();
	return i;
}

// this function gets a list of all xshot values in txName file, and stores to *sArr, nothing else will be stored
#define MAXLINE 400     /* The max length of each text line input*/
BOOL getXshotListFromTxOut(CString txName, CStringArray *sArr) {
	if (sArr == NULL) return false;
	sArr->RemoveAll();
	CString tStr;

	FILE *stream;
	if( _tfopen_s( &stream, txName, _T("r") ) != 0) {
		return FALSE;
	}

	TCHAR line[MAXLINE];
   long n;
   float t1, t2, t3, xshotOld = ERRORVAL;
	while (_fgetts(line,MAXLINE,stream)!=NULL) {
		if (_tcslen(line) < 2) continue;
		if (line[0] == _T('#')) continue; // we do not need comment lines
		_stscanf_s(line,_T("%f %f %f %d"), &t1,&t2,&t3,&n);
	   if(n<0) {
		   //No traveltime curve in file
		   break;
	   }

	   if (isFloatEqual(0.f, t3) && n==0 && !isFloatEqual(xshotOld, t1))  {
		tStr.Format(_T("%f"), t1);
		sArr->Add(tStr);
		xshotOld = t1;
	   }
   }
   
   fclose( stream );
   return TRUE;

}

PROCESS_INFORMATION myProcess(CString exeNamePath, CString workPath, bool isPause)
{
/*
// NOTE: Since Unicode is introduced, I found that CString saved to file will be double bytes, such as:
	" d : \ p r o j e c t \ s e i s W i d e . n e t \ w o
	and will not execute it they are commands lines. So I require a CStringA to be input here!!!
	####SOLVED, by using UTF-8

	// If you want SeisWide to wait for this process to finish...
	if(WaitForInputIdle(proc_i.hProcess, INFINITE) == WAIT_TIMEOUT)
	{
		AfxMessageBox(_T("Failed due to time-out in raytracing") ;
		return; 
	}

 // note: I use IDLE_PRIORITY_CLASS since a fortran program may hang up there for a long time
*/

	// exeNamePath = exeNamePath + _T(" > a.txt";
	PROCESS_INFORMATION proc_i;  // info returned from CreateProcess
	STARTUPINFO sui;         // Process startup info
	// Most sui members will be 0
	ZeroMemory ((PVOID)&sui, sizeof(sui));
	sui.cb = sizeof (sui);
	//GetStartupInfo(&sui);
	//sui.dwFlags = STARTF_USESTDHANDLES;

	CString newName = getSystemTempPath() + _T("seiBatch.bat");
	CFile cfBatchFile( newName, CFile::modeCreate| CFile::modeWrite );
	// cfBatchFile.Write( CT2CA(exeNamePath, CP_UTF8), exeNamePath.GetLength() );
	cfBatchFile.Write("chcp 65001\r\n", 12); 
	CT2CA aStrUtf8(exeNamePath, CP_UTF8);
	cfBatchFile.Write(aStrUtf8 , strlen(aStrUtf8));

	
	if (isPause)
		cfBatchFile.Write("\r\n pause", 8); 
	cfBatchFile.Close();


	CString str;
	GetWindowsDirectory(str.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
	str.ReleaseBuffer();

	if (isDirectoryExist(workPath))
		SetCurrentDirectory(workPath);

    newName = _T("/c ") + newName;
	BOOL Suc = CreateProcess (str + _T("\\system32\\cmd.exe"),   // image name
		newName.GetBuffer(_MAX_PATH), // command line
		NULL,   // process security attributes
		NULL,   // thread security attributes
		FALSE,   // inherit handles
		CREATE_DEFAULT_ERROR_MODE|CREATE_NEW_CONSOLE|IDLE_PRIORITY_CLASS,
		NULL,   // environment block
		NULL,   // current directory
		&sui,   // STARTUPINFO
		&proc_i);   // PROCESS_INFORMATION
	newName.ReleaseBuffer();


	return proc_i;

}

PROCESS_INFORMATION myProcess(CString exeNamePath, CString workPath)
{
	return myProcess(exeNamePath, workPath, true);
}

BOOL myProcessSimple(CString exeNamePath)
{
/*
	// If you don't want SeisWide to wait for this process to finish...
	if(!Suc) AfxMessageBox(_T("Not able to load xxxx.exe");


  Note: exeNamePath.GetBuffer(_MAX_PATH) returns LPTSTR 
    which is equivalent to lpsz calculated as follows:
	LPTSTR lpsz = new TCHAR[theString.GetLength()+1];
	wcscpy_s( lpsz, theString );

*/

	PROCESS_INFORMATION proc_i;  // info returned from CreateProcess
	STARTUPINFO sui;         // Process startup info
	// Most sui members will be 0
	ZeroMemory ((PVOID)&sui, sizeof(sui));
	sui.cb = sizeof (sui);
	BOOL Suc = CreateProcess (NULL,   // image name
		exeNamePath.GetBuffer(_MAX_PATH), // command line
		NULL,   // process security attributes
		NULL,   // thread security attributes
		FALSE,   // inherit handles
		CREATE_DEFAULT_ERROR_MODE|CREATE_NEW_CONSOLE|IDLE_PRIORITY_CLASS,
		NULL,   // environment block
		NULL,   // current directory
		&sui,   // STARTUPINFO
		&proc_i);   // PROCESS_INFORMATION
	exeNamePath.ReleaseBuffer();
	return Suc;
}

void showFileInNotePad(CString name)
{


	WCHAR lpszSystemInfo[MAX_PATH];   /* buffer for concat'd. str. */ 
	int nSize = GetSystemDirectoryW(lpszSystemInfo, MAX_PATH);
	CString str(lpszSystemInfo);
	str += _T("\\notepad.exe");
//	str.Format(_T("%c:\\Program Files\\Windows NT\\Accessories\\notepad.exe"), lpszSystemInfo[0]);
	if (!isFileExist(str)) str = _T("notepad.exe");

	if(!myProcessSimple(str + _T(" ")+name)) 
		AfxMessageBox(_T("Not able to load with ")+str);

}

BOOL myShowHelp(CString strFile) {

    CString seiPath;
    int nPathLength =  GetModuleFileName (NULL,seiPath.GetBuffer(_MAX_PATH), _MAX_PATH );
	seiPath.ReleaseBuffer();
	if (nPathLength < 0) return FALSE;
	
	CString aPath = seiPath.Left(seiPath.ReverseFind( '\\' ));
	CString aPath2 = aPath.Left(aPath.ReverseFind( '\\' ));

	bool isDebugging = seiPath.Find(_T("Debug")) > 0;

	CString docPath = (isDebugging)  ?
		aPath2 + _T("\\docs\\SeisWideDist6.2\\docs\\") + strFile
		:
		aPath + _T("\\docs\\") + strFile;

	if (!isFileExist(docPath)) {
		seiPath.Format(_T("Could not find the help file: \n"), docPath);
		AfxMessageBox(seiPath);
	}
	else if (GetFileExtension(strFile) == _T("txt")) {
		showFileInNotePad(docPath);
	}
	else {
		ShellExecute(
			0, 
			_T("open"), 
			docPath, 
			0, 
			0, 
			SW_SHOWNORMAL) ;
	}

	return TRUE;
}

void showStringInFile(CString str)
{
	if (str.GetLength() == 0) return;
	CString newName = getSystemTempPath() + _T("aBatch.tmptxt");
	FILE *stream;
	if( _tfopen_s(&stream, newName, _T("w") ) != 0 )
	{
		return;
	}
	_ftprintf(stream, _T("%s"), str);
	fclose( stream );
	showFileInNotePad(newName);
}


void showStringInFile(CString str, CString f)
{
	if (str.GetLength() == 0) return;
	FILE *stream;
	if( _tfopen_s(&stream, f, _T("w") ) != 0 )
	{
		return;
	}
	_ftprintf(stream, _T("%s"), str);
	fclose( stream );
	showFileInNotePad(f);
}

void showArrayInFile(float x [], int n)
{
	if (x == NULL || n <= 0) return;
	CString newName = getSystemTempPath() + _T("aBatch.tmptxt");

	FILE *stream;
	if( _tfopen_s(&stream, newName, _T("w") ) != 0 )
	{
		return;
	}
	for (int i=0; i<n; i++) {
		_ftprintf(stream, _T("%12.6f\r\n"), x[i]);
	}
	fclose( stream );

	showFileInNotePad(newName);
}


void showArrayInFile(CString str [], int n)
{
	if (str == NULL) return;
	CString newName = getSystemTempPath() + _T("aBatch.tmptxt");

	FILE *stream;
	if( _tfopen_s(&stream, newName, _T("w") ) != 0 )
	{
		return;
	}
	for (int i=0; i<n; i++) {
		_ftprintf(stream, _T("%s\r\n"), str[i]);
	}
	fclose( stream );

	showFileInNotePad(newName);
}


/*=============================================================
   Procedure to reverse high and low bytes.
*/
unsigned short revnib(unsigned short value)
{
	unsigned short m2=255,m7=65280;
	return ((value>>8) & m2) ^ ((value<<8) & m7);
}

BOOL isFileReadOnly(CString f) {

	CFileStatus status;
	BOOL initFileStatus = CFile::GetStatus(f, status ); // static function
	 
	if(status.m_attribute & 1 )
		return true;

	return false;
}


BOOL FormatSegy(CString f)
{
	return TRUE; 

	// depreciated
	f.TrimLeft();
	f.TrimRight();
	if(f.IsEmpty()) return FALSE;
	return true;

	CString temp;
	TRY
	{
		bhed bh;
		BOOL isUnix = FALSE;
		int bytesRead;

		if (isFileReadOnly(f)) {
			CFile file;
			if (!file.Open(f, CFile::modeRead | CFile::typeBinary)) {
				return FALSE;
			}
			file.Seek(3200L, SEEK_SET);
			bytesRead = file.Read(&bh, 400);
			if(bytesRead < 400) 
			{
				file.Close();
				return FALSE;
			}
			if (isUnix = IsUnix(bh.format)) SwapTapeHeader(&bh);
			
			if(bh.format<0 || bh.format>5)
			{
				file.Close();
				return FALSE;
			}
			file.Close();
			return true;
		}
		
		int		m_shot1;
		int		m_shot2;
		int len;
		segy tr;
		BOOL HeadModified = FALSE;
		CFile file;
		if (!file.Open(f, CFile::modeReadWrite | CFile::typeBinary)) {
			return FALSE;
		}

		file.Seek(3200L, SEEK_SET);
		bytesRead = file.Read(&bh, 400);
		if(bytesRead < 400) 
		{
			file.Close();
			return false;
		}
		if (isUnix = IsUnix(bh.format)) SwapTapeHeader(&bh);
		
		if(bh.format<0 || bh.format>5)
		{
			temp.Format(_T("Found incorrect data format. Continue anyway?"));
			if(AfxMessageBox(temp, MB_YESNO|MB_ICONINFORMATION) != IDYES) return false;
			temp.Format(_T("Is the data in 4-byte floating point?"));
			if( AfxMessageBox(temp, MB_YESNO|MB_ICONINFORMATION) == IDYES)
				bh.format = 3;
			else 
				bh.format = 0;
			HeadModified = TRUE;
		}
		file.Read(&tr, 240L);
		if (bh.hns <= 0) 
		{
			bh.hns = tr.ns;
			temp.Format(_T("Numsamp in the trace header of the selected trace overwrite the section header because it's invalid there"));
			AfxMessageBox(temp);
			HeadModified = TRUE;
		}
		if (bh.hdt <= 0) 
		{
			bh.hdt = tr.dt;
			temp.Format(_T("Numsamp in the trace header of the selected trace overwrite the section header because it's invalid there"));
			AfxMessageBox(temp);
			HeadModified = TRUE;
		}
		if (bh.hns > SU_NFLTS)
		{
			temp.Format(_T("Num of samples per trace is greater than %d. Contact programer to increase this limit."), SU_NFLTS);
			AfxMessageBox(temp);
		}
		
		if(bh.format!=3)  
		{
			len = bh.hns*4 + 240;
		}
		else 
		{
			len = bh.hns*2 + 240;
		}
		
		//		long curBytesRead = file.GetLength();
		//		m_totNTrace = (long)((curBytesRead-3600)/len);
		
		if (HeadModified)
		{
			file.Seek(3200L, CFile::begin);	//point to bin section header.
			file.Write(&bh, 400);
		}
		
		m_shot1 = tr.tracl;
		file.Seek(3600L+len, CFile::begin);	//point to next trace.
		file.Read(&tr, 240L);
		m_shot2 = tr.tracl;
		
		// format if tracl has no data
		if((m_shot1==m_shot2) && (m_shot1==0))
		{
			file.Seek(3600L, CFile::begin);	//point back to 1st trace.
			while (file.Read(&tr, 240L) == 240)
			{
				tr.tracl = tr.tracf;
				file.Seek(-240, CFile::current);
				file.Write(&tr, 240L);
				file.Seek(len-240, CFile::current);
			}
		}
		
		if(tr.dt-bh.hdt != 0   && tr.dt!=0)
		{
			// found that we should change the dt in the section header instead!
			if (bh.hdt==0) {
				temp.Format(_T("Sample interval dt of the second trace header is %d while it's %d in the section header. Fix the section header?"), tr.dt, bh.hdt);
				if( AfxMessageBox(temp, MB_YESNO|MB_ICONINFORMATION) == IDYES)
				{
					file.Seek(3200L, CFile::begin);	//point to next trace.
					if (file.Read(&bh, 400) == 400)
					{
						bh.hdt = tr.dt;
						file.Seek(-400, CFile::current);
						file.Write(&bh, 400L);
					}
				}
			}
		}
		
		file.Close();
	}
	CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL
		
	return TRUE;

}

double v2rho(double v, double velRef[9], double denRef[9])
{
	//a routine to calculate the density from vel using formula of Nafe.
    // double c1=-.6997, c2=2.2302, c3=-.598, c4=.07036, c5=-.0028311, dens;
	// if (v < 1.6) return 1.03;
	//	return (-0.00283*v*v*v*v + 0.0704*v*v*v - 0.598*v*v + 2.23*v - 0.7);
	//	dens = (c5*v*v*v*v + c4*v*v*v + c3*v*v + c2*v + c1);
	//	if (dens > 3.3) dens = 3.3;
	//	else if (dens < 1.4) dens = 1.4;

	double dens;
	if(v<=velRef[0]) dens = denRef[0];
	else if(v<=velRef[1]) dens=denRef[0]+(denRef[1]-denRef[0])*(v-velRef[0])/(velRef[1]-velRef[0]);
	else if(v<=velRef[2]) dens=denRef[1]+(denRef[2]-denRef[1])*(v-velRef[1])/(velRef[2]-velRef[1]);
	else if(v<=velRef[3]) dens=denRef[2]+(denRef[3]-denRef[2])*(v-velRef[2])/(velRef[3]-velRef[2]);
	else if(v<=velRef[4]) dens=denRef[3]+(denRef[4]-denRef[3])*(v-velRef[3])/(velRef[4]-velRef[3]);
	else if(v<=velRef[5]) dens=denRef[4]+(denRef[5]-denRef[4])*(v-velRef[4])/(velRef[5]-velRef[4]);
	else if(v<=velRef[6]) dens=denRef[5]+(denRef[6]-denRef[5])*(v-velRef[5])/(velRef[6]-velRef[5]);
	else if(v<=velRef[7]) dens=denRef[6]+(denRef[7]-denRef[6])*(v-velRef[6])/(velRef[7]-velRef[6]);
	else if(v<=velRef[8]) dens=denRef[7]+(denRef[8]-denRef[7])*(v-velRef[7])/(velRef[8]-velRef[7]);
	else dens = denRef[8];

	return dens;
}

static unsigned char a2e[256] = {
	0,  1,  2,  3, 55, 45, 46, 47, 22,  5, 37, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 60, 61, 50, 38, 24, 25, 63, 39, 28, 29, 30, 31,
		64, 79,127,123, 91,108, 80,125, 77, 93, 92, 78,107, 96, 75, 97,
		240,241,242,243,244,245,246,247,248,249,122, 94, 76,126,110,111,
		124,193,194,195,196,197,198,199,200,201,209,210,211,212,213,214,
		215,216,217,226,227,228,229,230,231,232,233, 74,224, 90, 95,109,
		121,129,130,131,132,133,134,135,136,137,145,146,147,148,149,150,
		151,152,153,162,163,164,165,166,167,168,169,192,106,208,161,  7,
		32, 33, 34, 35, 36, 21,  6, 23, 40, 41, 42, 43, 44,  9, 10, 27,
		48, 49, 26, 51, 52, 53, 54,  8, 56, 57, 58, 59,  4, 20, 62,225,
		65, 66, 67, 68, 69, 70, 71, 72, 73, 81, 82, 83, 84, 85, 86, 87,
		88, 89, 98, 99,100,101,102,103,104,105,112,113,114,115,116,117,
		118,119,120,128,138,139,140,141,142,143,144,154,155,156,157,158,
		159,160,170,171,172,173,174,175,176,177,178,179,180,181,182,183,
		184,185,186,187,188,189,190,191,202,203,204,205,206,207,218,219,
		220,221,222,223,234,235,236,237,238,239,250,251,252,253,254,255
};

static unsigned char e2a[256] = {
	0,  1,  2,  3,156,  9,134,127,151,141,142, 11, 12, 13, 14, 15,
		16, 17, 18, 19,157,133,  8,135, 24, 25,146,143, 28, 29, 30, 31,
		128,129,130,131,132, 10, 23, 27,136,137,138,139,140,  5,  6,  7,
		144,145, 22,147,148,149,150,  4,152,153,154,155, 20, 21,158, 26,
		32,160,161,162,163,164,165,166,167,168, 91, 46, 60, 40, 43, 33,
		38,169,170,171,172,173,174,175,176,177, 93, 36, 42, 41, 59, 94,
		45, 47,178,179,180,181,182,183,184,185,124, 44, 37, 95, 62, 63,
		186,187,188,189,190,191,192,193,194, 96, 58, 35, 64, 39, 61, 34,
		195, 97, 98, 99,100,101,102,103,104,105,196,197,198,199,200,201,
		202,106,107,108,109,110,111,112,113,114,203,204,205,206,207,208,
		209,126,115,116,117,118,119,120,121,122,210,211,212,213,214,215,
		216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,
		123, 65, 66, 67, 68, 69, 70, 71, 72, 73,232,233,234,235,236,237,
		125, 74, 75, 76, 77, 78, 79, 80, 81, 82,238,239,240,241,242,243,
		92,159, 83, 84, 85, 86, 87, 88, 89, 90,244,245,246,247,248,249,
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57,250,251,252,253,254,255
};

void ebc2asc(unsigned char ascHead[], int len) {
	// this converts from IBM's DBCDIC format to ASCII format.

	for(int x=0; x<len; x++) {
		ascHead[x] = e2a[(unsigned char) ascHead[x]]; // translate from ebcdic to ascii 
    } 
} 

void asc2ebc(unsigned char ascHead[], int len) {
	// this converts from ASCII format to IBM's DBCDIC format.
	for(int x=0; x<len; x++) {
		ascHead[x] = a2e[(unsigned char) ascHead[x]]; // translate from ebcdic to ascii 
    } 
} 
bool isAsciiPure(unsigned char ascHead[], int len) {
	bool isAsciiPure = true;
	for (int i=0; i<len; i++) {
		if (ascHead[i]<0 || ascHead[i]>127) {
			isAsciiPure = false;
			break;
		}
	}
	return isAsciiPure;
}


int signum(float v) {
	return (v > 0.f) - (v < 0.f);
}
int signum(double v) {
	return (v > 0.) - (v < 0.);
}

/* Give an array, calculatre the median. This function is slow although it works, and not used any more.
*/
//double getMedian(float x [ ], long n)
//{
//	float *x1, result1;
//	long i,j,iSelect;
//
//	x1 = (float *)malloc(2*n*sizeof(float));
//	for (j=0; j<n-1; j++)
//	{
//		x1[j] = x[j];
//		iSelect = j;
//		for (i=j+1; i<n; i++)
//		{
//			if(x1[j]>x[i]) 
//			{
//				iSelect=i;
//				x1[j]=x[i];
//			}
//		}
//
//		//now at iSelect, dx[] is put into dx1[], and also the rest.
//		for (i=j+1; i<n+1; i++)
//		{
//			if(i < iSelect+1) x1[i]=x[i-1];
//			else x1[i]=x[i];
//		}
//
//		//now put x1[] back to x[].
//		for (i=j; i<n; i++)
//			x[i] = x1[i];
//	}
//
//	result1 = x1[(long)(n/2)];
//	free(x1);
//	return result1;
//}

/* this function is very slow*/
//double getMedian2(float *x, long n)
//{
//	float *x1, result1;
//	long i,j,iSelect;
//
//	x1 = (float *)malloc(2*n*sizeof(float));
//	for (j=0; j<n-1; j++)
//	{
//		x1[j] = x[j];
//		iSelect = j;
//		for (i=j+1; i<n; i++)
//		{
//			if(x1[j]>x[i]) 
//			{
//				iSelect=i;
//				x1[j]=x[i];
//			}
//		}
//
//		//now at iSelect, x[] is put into x1[], and also the rest.
//		for (i=j+1; i<n+1; i++)
//		{
//			if(i < iSelect+1) x1[i]=x[i-1];
//			else x1[i]=x[i];
//		}
//
//		//now put x1[] back to x[].
//		for (i=j; i<n; i++)
//			x[i] = x1[i];
//	}
//
//	result1 = x1[(long)(n/2)];
//	free(x1);
//	return result1;
//}




void splitMedia3(float a[], int n, float x, int &i, int &j)
{
	do
	{
		//scan from the left then scan from the right
		while (a[i] < x)i++;
		while (x < a[j])j--;
		//now swap values if they are in the wrong part:
		if (i <= j)
		{
			float t = a[i];
			a[i] = a[j];
			a[j] = t;
			i++;j--;
		}
		//and continue the scan until the pointers cross:
	} while (i <= j);
}


//void getMedian3(float a[], int n, int &k)
int getMedian3(float a[], int n)
{
	int L = 0;
	int R = n-1;
	int k = n / 2;
	int i;
	int j;
	while (L < R)
	{
		float x = a[k];
		i = L; j = R;
		splitMedia3(a, n, x, i, j);
		if (j < k)  L = i;
		if (k < i)  R = j; 
	}
	return k;
}

double getMedianAbsolute(float *x, long n)
{
	// Note: we cannot overwrite the original array dx[]

	float *x1 = (float *)malloc(n*sizeof(float));
	if (!x1) return 1.0;  // it fails to allocate memory
	double a;
	int j;
	for (j=0; j<n; j++)
	{
		x1[j] = (float)fabs(x[j]);
	}

	j = getMedian3(&x1[0], n);
	a = x1[j];

	// now the problem: if we have lots of zero points at the beginning and end of x[], it will give incorrect median. 
	// These zeros can be numerous number for modeled data or for trimmed data!
	//a = getAvgValue(x1, n);  
	// do not use average value since it will be incorrect a single value of MIL will destroy the results!
	float aMinValue =  (float)(a * 0.001);  // we treat anything below average/1000 will not be considered

	// we will need to calculate j1 and j2 
	int j1 = 0;
	for (j=0; j<n; j++) {
		if (x1[j] > aMinValue) {
			j1 = j;
			break;
		}
	} // for j

	int j2 = n - 1;
	for (j=n-1; j>=0; j--) {
		if (x1[j] > aMinValue) {
			j2 = j;
			break;
		}
	} // for j



	double out = 0.0;
	if (j2 <= j1) out = a;
	else if (j1==0 && j2==n-1) out = a;
	else {
		// now we need to re-calculate 
		n = j2-j1+1;
		int k = getMedian3(&x1[j1], n);
		out = x1[k+j1];
	}

	free(x1);
	return out;

}

double getMedianInt(int *x, long n)
{
	// Note: we cannot overwrite the original array dx[]
	float *x1 = (float *)malloc(n*sizeof(float));
	if (!x1) return 1.0;  // it fails to allocate memory

	int j;
	for (j=0; j<n; j++)
	{
		x1[j] = (float)x[j];
	}
	//double a = getMedian2(&x1[0], n);
	j = getMedian3(&x1[0], n);
	double a = x1[j];
	free(x1);
	return a;

}

double getMedianValue(double *x, long n) {
	float *x1 = (float *)malloc(n*sizeof(float));
	if (!x1) return 1.0;  // it fails to allocate memory
	int j;
	for (j=0; j<n; j++)
	{
		x1[j] = (float)x[j];
	}
	j = getMedian3(&x1[0], n);
	double a = x1[j];
	free(x1);
	return a;
}

float getMedianValue(float *x, long n) {
	int j = getMedian3(x, n);
	return x[j];
}

float getMaxValue(float *x, long n, float aThreshold)
{
	int j;
	float amax = -MIL*1000.0;
	for (j=0; j<n; j++)
	{
		if (x[j]<aThreshold && x[j] > amax) amax = x[j];
	}
	return amax;
}

float getMaxValue(float *x, long n)
{
	int j;
	double amax = -MIL*1000.0;
	for (j=0; j<n; j++)
	{
		if (x[j] > amax) amax = x[j];
	}
	return (float)amax;
}

double getMaxValue(double *x, long n)
{
	int j;
	double amax = -MIL*1000.0;
	for (j=0; j<n; j++)
	{
		if (x[j] > amax) amax = x[j];
	}
	return amax;
}

int getMaxIndex(float *x, long n)
{
	// find the index that corresponds to the max value
	int j;
	int indx = -1;
	float amax = -MIL*1000.0;
	for (j=0; j<n; j++)
	{
		if (x[j] > amax) {
			amax = x[j];
			indx = j;
		}
	}
	//float amed = (float)getMedianAbsolute(x, n);
	//if ( fabs(amed-amax) < MINAMP) indx = -1;
	return indx;
}

int getMinIndex(float *x, long n)
{
	// find the index that corresponds to the min value
	int j;
	int indx = -1;
	float amin = MIL*1000.0;
	for (j=0; j<n; j++)
	{
		if (x[j] < amin) {
			amin = x[j];
			indx = j;
		}
	}
	return indx;
}

float getMaxValueAbsolute(float *x, long n)
{
	int j;
	double amax = -MIL*1000.0;
	double ampLimit = getMedianAbsolute(&x[0], n) * 1000.;
	// this ampLimit is enforced since when calculating max amp to plot,
	// invalid numbers make a huge difference.
	for (j=0; j<n; j++) {
		if (_isnan(x[j])) continue;
		if ( fabs(x[j]) > ampLimit) continue;
		if (fabs(x[j]) > amax) amax = (float)fabs(x[j]);
	}
	return (float)amax;
}

double getMaxValueAbsolute(double *x, long n)
{
	int j;
	double amax = -MIL*1000.0;
//	double ampLimit = getMedianAbsolute(&x[0], n) * 1000.;
	for (j=0; j<n; j++)
	{
		if (_isnan(x[j])) continue;
//		if ( fabs(x[j]) > ampLimit) continue;
		if (fabs(x[j]) > amax) amax = (float)fabs(x[j]);
	}
	return amax;
}

int getMinXValueCPoint(CPoint *p, long n)
{
	LONGLONG ampMax = (int)MIL*1000;
	for (int i=0; i<n; i++) {
		if (ampMax > p[i].x) ampMax = p[i].x;
	}
	return (int)ampMax;
}

int getMaxXValueCPoint(CPoint *p, long n)
{
	int	ampLow = 0;
	for (int i=0; i<n; i++) {
		if (ampLow < p[i].x) ampLow = p[i].x;
	}
	return ampLow;
}

float getMinValue(float *x, long n)
{
	int j;
	float amin = MIL*1000.0;
	for (j=0; j<n; j++)
	{
		if (x[j] < amin) amin = x[j];
	}
	return amin;
}


double getMinValue(double *x, long n)
{
	int j;
	double amin = MIL*1000.0;
	for (j=0; j<n; j++)
	{
		if (x[j] < amin) amin = x[j];
	}
	return amin;
}

int getNearestTraceLeft(int *x, bool *isUseArr, int n, int px) 
{
	// this routine takes care of irregular trace spacing
	const int ierr = -9999999;
	int pxOut = ierr;
	for (int j=0; j<n; j++) {
		if (!isUseArr[j]) continue;
		if (x[j] >= px) continue;

		if (x[j] > pxOut) pxOut = x[j];
	}
	if (pxOut == ierr) pxOut = px - 10; // when error, we just need a reasonable any number

	return pxOut;
}

int getNearestTraceRight(int *x, bool *isUseArr, int n, int px) 
{
	// this routine takes care of irregular trace spacing
	const int ierr = 9999999;
	int pxOut = ierr;
	for (int j=0; j<n; j++) {
		if (!isUseArr[j]) continue;
		if (x[j] <= px) continue;

		if (x[j] < pxOut) pxOut = x[j];
	}
	if (pxOut == ierr) pxOut = px + 10; // when error, we just need a reasonable any number

	return pxOut;
}


double getValue4Color(double *x, long n)
{
	// if the first value is minus, get the min value for coloring
	// otherwise, get the max value for coloring
	if (x == NULL || n < 1) return 0.;
	else if (n == 1) return x[0];
	//else return x[(int)(n/2)];
	else if (x[0] < 0.) return getMinValue(x, n);
	else return getMaxValue(x, n);
}

float getValue4Color(float *x, long n)
{
	// if the first value is minus, get the min value for coloring
	// otherwise, get the max value for coloring
	if (x == NULL || n < 1) return 0.;
	else if (n == 1) return x[0];
	//else return x[(int)(n/2)];
	else if (x[0] < 0.) return getMinValue(x, n);
	else return getMaxValue(x, n);
}


float getAvgValue(float *x, long n) {
	int j;
	int j1 = 0;
	for (j=0; j<n; j++) {
		if (fabs(x[j]) > MINAMP) {
			j1 = j;
			break;
		}
	} // for j

	int j2 = n - 1;
	for (j=n-1; j>=0; j--) {
		if (fabs(x[j]) > MINAMP) {
			j2 = j;
			break;
		}
	} // for j

	if (j1 >= j2) return 0.f;

	double aAvg = 0.0;
	for (j=j1; j<=j2; j++)
	{
		aAvg += x[j];
	}
	if (j2-j1+1 > 1) 
		aAvg /= (double)(j2-j1+1);
	return (float)aAvg;
}

float getAvgValueAbsolute(float *x, long n) {
	// we will need to calculate j1 and j2 
	int j;
	int j1 = 0;
	for (j=0; j<n; j++) {
		if (fabs(x[j]) > MINAMP) {
			j1 = j;
			break;
		}
	} // for j

	int j2 = n - 1;
	for (j=n-1; j>=0; j--) {
		if (fabs(x[j]) > MINAMP) {
			j2 = j;
			break;
		}
	} // for j

	if (j1 >= j2) return 0.f;

	double aAvg = 0.0;
	for (j=j1; j<=j2; j++)
	{
		aAvg += fabs(x[j]);
	}
	if (j2-j1+1 > 1) 
		aAvg /= (double)(j2-j1+1);
	return (float)aAvg;
}

double getMedianFast(float *x, long n)
{
	int k = getMedian3(x, n);
	return x[k];

	// the following was a working function but not needed any more.

	// assume x[] already has absolute values
	if (n<1) return 0.;

	float result1;
	float aMax = 0.f;
	long j;
	long num = 0;

	result1 = 0.;
	for (j=0; j<n; j++)
	{
		if (aMax < x[j]) aMax = x[j];
		if (x[j] > 0.f) {
			result1 += x[j];
			num++;
		}
	}
	if (num > 0) result1 /= num;
	else return 0.f;

	// now, we have:  aMax=the max value;  result1=mean

	float aThreshold = result1 * 40.f;
	if (aMax < aThreshold) {
		return result1;
	}


	result1 = 0.;
	num = 0;
	for (j=0; j<n; j++)
	{
		if (aThreshold > x[j]) {
			result1 += x[j];
			num++;
		}
	}

	if (num > 0) result1 /= num;
	return result1;
}

/* given an distance array, and distance value, return the closest match index 
	for this distance. Assume dist[] in km is already sorted!!!
*/
int getClosestIndex(float dist[], int *n, float *x)
{
	if (dist==NULL) return -1;

	for (int j=1; j< *n; j++) {
		if ( *x< dist[j-1] + (dist[j]-dist[j-1])*.5) return j-1;
	}

	return *n-1;
}


int getClosestIndex(float *fy, float *dy, int *n, float *x, int *i0) 
{
	float y;
	if ( *i0 < 0) *i0 = 0;
	for (int j= *i0+1; j< *n; j++) {
		y = *fy + *dy  *  (j-1);
		if ( *x< y + *dy  *.5) return j-1;
	}

	return *n-1;
}



int getClosestIndex(double fy, double dy, int n, double x, int i0) 
{
	double y;
	if ( i0 < 0) i0 = 0;
	for (int j= i0+1; j< n; j++) {
		y = fy + dy  *  (j-1);
		if ( x< y + dy  *.5) return j-1;
	}

	return n-1;
}



// i0 is the starting index to search, so that the result must be >= i0
int getClosestIndex(double dist[], int *n, double *x, int *i0)
{
	if (dist==NULL) return -1;
	if ( *i0 < 0) *i0 = 0;
	for (int j= *i0+1; j< *n; j++) {
		if ( *x < dist[j-1] + (dist[j]-dist[j-1])*.5) return j-1;
	}

	return *n - 1;
}

int getClosestIndex(float dist[], int *n, float *x, int *i0)
{
	if (dist==NULL) return -1;
	if ( *i0 < 0) *i0 = 0;
	for (int j= *i0+1; j< *n; j++) {
		if ( *x < dist[j-1] + (dist[j]-dist[j-1])*.5f) return j-1;
	}

	return *n - 1;
}

int getClosestIndexRev(double dist[], int *n, double *x, int *i0)
{
	if (dist==NULL) return -1;
	if ( *i0 >= *n) *i0 = *n-1;
	if ( *i0 < 0) *i0 = 0;
	*i0 = 0;
	for (int j= *n - 1; j > *i0; j--) {
		if ( *x < dist[j-1] + (dist[j]-dist[j-1])*.5) return j-1;
	}

	return 0;
}


// log scale for color display
// note:  log10(1) = 0;  log10(10)=1; log10(1000)=3
//    but this function will return myLogScaling0(0)=0; myLogScaling0(9)=1; myLogScaling0(999)=3; 
//					myLogScaling0(-999)=-3 ...
double logScalingPure(double amp)
{
	return sgn(amp) * log10( 1. + fabs(amp) );
}

double logScalingInvPure(double ampLog)
{
	return sgn(ampLog) * ( pow(10., fabs(ampLog)) - 1. );
}



double logScaling2(double amp)
{
	double out = log10( 1. + fabs(amp) );
	// double b = fabs(amp)*factor;

	return sgn(amp)*out*out;
}

double logScalingInv2(double ampLog)
{
	return sgn(ampLog) * ( pow(10., sqrt(fabs(ampLog))) - 1. );
}

// for color interpolation!
int interpolate(int a, int b, int c, int d, double t, double s)
{
    return (int)(a*(1-t)*(1-s) + b*t*(1-s) + c*(1-t)*s + d*t*s);
}

double interpolate(double a, double b, double c, double d, double t, double s)
{
    return (a*(1-t)*(1-s) + b*t*(1-s) + c*(1-t)*s + d*t*s);
}

COLORREF interpolate(COLORREF a, COLORREF b, COLORREF c, COLORREF d, double t, double s)
{
    double fa = (1-t)*(1-s);
	double fb = t*(1-s);
	double fc = (1-t)*s;
	double fd = t*s;

	return RGB( (int)(GetRValue(a)*fa + GetRValue(b)*fb + GetRValue(c)*fc + GetRValue(d)*fd),
                (int)(GetGValue(a)*fa + GetGValue(b)*fb + GetGValue(c)*fc + GetGValue(d)*fd),
				(int)(GetBValue(a)*fa + GetBValue(b)*fb + GetBValue(c)*fc + GetBValue(d)*fd) );

}

//#include <gdiplus.h>
COLORREF getColorInverseDistMono(int **colDataArr, float *rangeArr, int nx, int ny, float ymin, float dy, float x, float y)
{
	// Given a 2D array of COLORREF format, look for the index of the 4 corners the surrounds the given point (x, y)
	// After that use inverse distance rule to get the COLORREF
	// NOTE: our routine allows X-axis irregularly spaced, while Y-axis is still regularly spaced, so there is no xmin and dx inputs.
	static COLORREF colWhite = RGB(255, 255, 255);
	if (colDataArr==NULL || rangeArr==NULL || nx<1 || ny<1 || dy==0.f) return colWhite;

	// note: rangeArr[] should be monotonically increasing or decreasing, what about if not? Then we should use the closest index only!
	int ix = -1;
	for (int i=0; i<nx-1; i++) {
		if ( x> rangeArr[i] && x< rangeArr[i+1]) {
			ix = i;
			break;
		}
	}

	double ay = (y - ymin)/dy;
	int iy = (int)ay;
	
	if (ix<0 || ix>nx-1 || iy<0 || iy>ny-1) return colWhite;

	int ix2 = ix+1;
	int iy2 = iy+1;
	if (ix2>nx-1) ix2=nx-1;
	if (iy2>ny-1) iy2=ny-1;

	double ax = rangeArr[ix2] - rangeArr[ix];
	double t = fabs(ax) < MINAMP  ?  0.  :  (x - rangeArr[ix]) / ax;
	if (t < 0.0) t = 0.0;
	else if (t>1.0) t = 1.0;

	double s = ay - iy;

	return interpolate((COLORREF)colDataArr[ix][iy], (COLORREF)colDataArr[ix2][iy], (COLORREF)colDataArr[ix][iy2], (COLORREF)colDataArr[ix2][iy2], t, s);

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

double getAmpAtXY(float **dataArr, float *rangeArr, int nx, int ny, float ymin, float dy, float x, float y)
{
	// Given a 2D array of amp, look for the index of the 4 corners the surrounds the given point (x, y)
	// After that use inverse distance rule to get the COLORREF
	// NOTE: our routine allows X-axis irregularly spaced, while Y-axis is still regularly spaced, so there is no xmin and dx inputs.
	if (dataArr==NULL || rangeArr==NULL || nx<1 || ny<1 || dy==0.f) return 0.;

	// note: rangeArr[] should be monotonically increasing or decreasing, what about if not? Then we should use the closest index only!
	int jx = -1;
	if (x<=rangeArr[0]) jx=0;
	else if (x>=rangeArr[nx-1]) jx=nx-1;
	else {
		for (int i=0; i<nx-1; i++) {
			if ( x> rangeArr[i] && x< rangeArr[i+1]) {
				jx = i;
				break;
			}
		}
	}

	if (jx<0) jx=0;
	else if (jx>nx-1) jx=nx-1;
	int jx2 = jx+1;
	if (jx2>nx-1) jx2=nx-1;
	double ax = rangeArr[jx2] - rangeArr[jx];
	double percentFromJx = fabs(ax) < MINAMP  ?  0.  :  (x - rangeArr[jx]) / ax;
	if (percentFromJx < 0.0) percentFromJx = 0.0;
	else if (percentFromJx>1.0) percentFromJx = 1.0;


	double ay = (y - ymin)/dy;
	int iy = (int)ay;
	if (iy<0) iy=0;
	else if (iy>ny-1) iy=ny-1;
	int iy2 = iy+1;
	if (iy2>ny-1) iy2=ny-1;

	return interpolate((double)dataArr[jx][iy], (double)dataArr[jx2][iy], (double)dataArr[jx][iy2], (double)dataArr[jx2][iy2], 
		percentFromJx, (double)(ay - iy));
}

double getAmpAtXY(float **dataArr, int nx, int ny, double ymin, double dy, double y,  int jx, int jx2, double percentFromJx)
{
	if (dataArr==NULL || nx<1 || ny<1 || dy==0.) return 0.;

	if (jx<0) jx=0;
	else if (jx>nx-1) jx=nx-1;
	if (jx2<0) jx2=0;
	else if (jx2>nx-1) jx2=nx-1;

	double ay = (y - ymin)/dy;
	int iy = (int)ay;
	if (iy<0) iy=0;
	else if (iy>ny-1) iy=ny-1;
	int iy2 = iy+1;
	if (iy2>ny-1) iy2=ny-1;

	return interpolate((double)dataArr[jx][iy], (double)dataArr[jx2][iy], (double)dataArr[jx][iy2], (double)dataArr[jx2][iy2], 
		percentFromJx, (double)(ay - iy));
}

void trimRight(TCHAR line[]) {
	// note: the last character is a line feed of carrage return
	for (int m=(int)_tcslen(line)-2; m>0; m--) {
		if (line[m]!=' ') {
			line[m+1] = '\0';
			return;
		}
	}
}



// The followings are all utils required from Numerical Recipes book:
#define NR_END 1
#define FREE_ARG char*
void nrerror(TCHAR error_text[])
/* Numerical Recipes standard error handler */
{
	_ftprintf(stderr,_T("Numerical Recipes run-time error...\n"));
	_ftprintf(stderr,_T("%s\n"),error_text);
	_ftprintf(stderr,_T("...now exiting to system...\n"));
	exit(1);
}
float *vector(long nl, long nh)
/* allocate a float vector with subscript range v[nl..nh] */
{
	float *v;
	v=(float *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(float)));
	if (!v) nrerror(_T("allocation failure in vector()"));
	return v-nl+NR_END;
}
int *ivector(long nl, long nh)
/* allocate an int vector with subscript range v[nl..nh] */
{
	int *v;
	v=(int *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(int)));
	if (!v) nrerror(_T("allocation failure in ivector()"));
	return v-nl+NR_END;
}
unsigned long *lvector(long nl, long nh)
/* allocate an unsigned long vector with subscript range v[nl..nh] */
{
	unsigned long *v;
	v=(unsigned long *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(long)));
	if (!v) nrerror(_T("allocation failure in lvector()"));
	return v-nl+NR_END;
}
void free_vector(float *v, long nl, long nh)
/* free a float vector allocated with vector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}
void free_ivector(int *v, long nl, long nh)
/* free an int vector allocated with ivector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}
void free_cvector(unsigned char *v, long nl, long nh)
/* free an unsigned char vector allocated with cvector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}
void free_lvector(unsigned long *v, long nl, long nh)
/* free an unsigned long vector allocated with lvector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}
void free_dvector(double *v, long nl, long nh)
/* free a double vector allocated with dvector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}

#define M 7
#define NSTACK 50
#define SWAP2(a,b) itemp=(a);(a)=(b);(b)=itemp;
void indexx(unsigned long n, float arr[], unsigned long indx[])
/*
Indexes an array arr[1..n], i.e., outputs the array indx[1..n] such that arr[indx[j]] is
in ascending order for j = 1, 2, . . . ,N. The input quantities n and arr are not changed.
*/
{
	unsigned long i,indxt,ir=n,itemp,j,k,l=1;
	int jstack=0,*istack;
	float a;
	istack=ivector(1,NSTACK);
	
	for (j=1;j<=n;j++) indx[j]=j;
	for (;;) {
		if (ir-l < M) {
			for (j=l+1;j<=ir;j++) {
				indxt=indx[j];
				a=arr[indxt];
				for (i=j-1;i>=l;i--) {
					if (arr[indx[i]] <= a) break;
					indx[i+1]=indx[i];
				}
				indx[i+1]=indxt;
			}
			if (jstack == 0) break;
			ir=istack[jstack--];
			l=istack[jstack--];
		} else {
			k=(l+ir) >> 1;
			SWAP2(indx[k],indx[l+1]);
			if (arr[indx[l]] > arr[indx[ir]]) {
				SWAP2(indx[l],indx[ir]);
			}
			if (arr[indx[l+1]] > arr[indx[ir]]) {
				SWAP2(indx[l+1],indx[ir]);
			}
			if (arr[indx[l]] > arr[indx[l+1]]) {
				SWAP2(indx[l],indx[l+1]);
			}
			i=l+1;
			j=ir;
			indxt=indx[l+1];
			a=arr[indxt];
			for (;;) {
				
				do i++; while (arr[indx[i]] < a);
				do j--; while (arr[indx[j]] > a);
				if (j < i) break;
				SWAP2(indx[i],indx[j]);
			}
			indx[l+1]=indx[j];
			indx[j]=indxt;
			jstack += 2;
			if (jstack > NSTACK) nrerror(_T("NSTACK too small in indexx."));
			if (ir-i+1 >= j-l) {
				istack[jstack]=ir;
				istack[jstack-1]=i;
				ir=j-1;
			} else {
				istack[jstack]=j-1;
				istack[jstack-1]=l;
				l=i;
			}
		}
	}
	free_ivector(istack,1,NSTACK);
}


void sort3(unsigned long n, float ra[], float rb[], float rc[])
/*
Sorts an array ra[1..n] into ascending numerical order while making the corresponding rearrangements
of the arrays rb[1..n] and rc[1..n]. An index table is constructed via the
routine indexx.
*/
{
//	void indexx(unsigned long n, float arr[], unsigned long indx[]);
	unsigned long j,*iwksp;
	float *wksp;
	iwksp=lvector(1,n);
	wksp=vector(1,n);
	indexx(n,ra,iwksp); // Make the index table.
	for (j=1;j<=n;j++) wksp[j]=ra[j]; // Save the array ra.
	for (j=1;j<=n;j++) ra[j]=wksp[iwksp[j]]; // Copy it back in rearranged order.
	for (j=1;j<=n;j++) wksp[j]=rb[j]; // Ditto rb.
	for (j=1;j<=n;j++) rb[j]=wksp[iwksp[j]];
	for (j=1;j<=n;j++) wksp[j]=rc[j]; // Ditto rc.
	for (j=1;j<=n;j++) rc[j]=wksp[iwksp[j]];
	free_vector(wksp,1,n);
	free_lvector(iwksp,1,n);
}

void sort2(unsigned long n, float ra[], float rb[])
/*
Sorts an array ra[0..n-1] into ascending numerical order while making the corresponding rearrangements
of the arrays rb[0..n-1]. An index table is constructed via the
routine indexx.

   NOTE: arrays must be n+1 long!!!
*/
{
//	void indexx(unsigned long n, float arr[], unsigned long indx[]);
	unsigned long j,*iwksp;
	float *wksp;
	iwksp=lvector(1,n);
	wksp=vector(1,n);

	// since Numerical recipe uses 1...n, but my arrays are 0...n-1
	// I convert first
	for (j=n; j>0; j--) {
		ra[j] = ra[j-1];
		rb[j] = rb[j-1];
	}

	indexx(n,ra,iwksp); // Make the index table.
	for (j=1;j<=n;j++) wksp[j]=ra[j]; // Save the array ra.
	for (j=1;j<=n;j++) ra[j]=wksp[iwksp[j]]; // Copy it back in rearranged order.
	for (j=1;j<=n;j++) wksp[j]=rb[j]; // Ditto rb.
	for (j=1;j<=n;j++) rb[j]=wksp[iwksp[j]];

	// I convert back
	for (j=0; j<n; j++) {
		ra[j] = ra[j+1];
		rb[j] = rb[j+1];
	}

	free_vector(wksp,1,n);
	free_lvector(iwksp,1,n);
}

void sort2(unsigned long n, float ra[], int ib[])
/*
Sorts an array ra[0..n-1] into ascending numerical order while making the corresponding rearrangements
of the arrays rb[0..n-1]. An index table is constructed via the
routine indexx.

   NOTE: arrays must be n+1 long!!!
*/
{
//	void indexx(unsigned long n, float arr[], unsigned long indx[]);
	unsigned long j,*iwksp;
	float *wksp;
	iwksp=lvector(1,n);
	wksp=vector(1,n);

	// since Numerical recipe uses 1...n, but my arrays are 0...n-1
	// I convert first
	for (j=n; j>0; j--) {
		ra[j] = ra[j-1];
		ib[j] = ib[j-1];
	}

	indexx(n,ra,iwksp); // Make the index table.
	for (j=1;j<=n;j++) wksp[j]=ra[j]; // Save the array ra.
	for (j=1;j<=n;j++) ra[j]=wksp[iwksp[j]]; // Copy it back in rearranged order.
	for (j=1;j<=n;j++) wksp[j]=(float)ib[j]; // Ditto rb.
	for (j=1;j<=n;j++) ib[j]=(int)myRound(wksp[iwksp[j]]);

	// I convert back
	for (j=0; j<n; j++) {
		ra[j] = ra[j+1];
		ib[j] = ib[j+1];
	}

	free_vector(wksp,1,n);
	free_lvector(iwksp,1,n);
}


// my simplest sorting routine
void sort(float dx [ ], long n)
{
	// since Numerical recipe uses 1...n, but my arrays are 0...n-1
	long j;
	float *ra;
	ra = vector(0, n);
	for (j=0; j<n; j++) {
		ra[j+1] = dx[j];
	}

	unsigned long *iwksp;
	iwksp=lvector(1,n);
	indexx(n,ra,iwksp); // Make the index table.

	for (j=0; j<n; j++) {
		dx[j] = ra[iwksp[j+1]];
	}
	free_lvector(iwksp,1,n);
	free_vector(ra,0,n);
}

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
void four1(float data[], unsigned long nn, int isign)
{
	//This function is taken from Numerical Recipe.

	unsigned long n, mmax, m, j, istep, i;
	double wtemp, wr,wpr,wpi,wi,theta;
	float tempr,tempi;

	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	//Here begins the Danielson-Lanczos section.
	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr= -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=(float)(wr*data[j]-wi*data[j+1]);
				tempi=(float)(wr*data[j+1]+wi*data[j]);
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}

void realft(float data[], unsigned long n, int isign)

/*
Calculates the Fourier transform of a set of n real-valued data points.
Replaces this data (which is stored in array data[1..n]) by the positive
frequency half of its complex Fourier transform.
The real-valued rst and last components of the complex transform are
returned as elements data[1] and data[2], respectively. n must be a power of 2.
This routine also calculates the
inverse transform of a complex data array if it is the transform of real data.
(Result in this case must be multiplied by 2/n.)       */
{
    unsigned long i,i1,i2,i3,i4,np3;
    float c1=0.5,c2,h1r,h1i,h2r,h2i;
    double wr,wi,wpr,wpi,wtemp,theta;  //Double precision for the trigonomet-
                                        // ric recurrences.

    theta=3.141592653589793/(double) (n>>1); // Initialize the recurrence.
    if (isign == 1) {
        c2 = -0.5;
        four1(data,n>>1,1); // The forward transform is here.
    } else {
        c2=0.5;                 //Otherwise set up for an inverse transform.
        theta = -theta;
    }
    wtemp=sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi=sin(theta);
    wr=1.0+wpr;
    wi=wpi;
    np3=n+3;
    for (i=2;i<=(n>>2);i++) {   //Case i=1 done separately below.
        i4=1+(i3=np3-(i2=1+(i1=i+i-1)));
        h1r=c1*(data[i1]+data[i3]); // The two separate transforms are separated out of data.
        h1i=c1*(data[i2]-data[i4]);
        h2r = -c2*(data[i2]+data[i4]);
        h2i=c2*(data[i1]-data[i3]);
        data[i1]=(float)(h1r+wr*h2r-wi*h2i); //Here they are recombined to form the true
                                    //transform of the original real data.
        data[i2]=(float)(h1i+wr*h2i+wi*h2r);
        data[i3]=(float)(h1r-wr*h2r+wi*h2i);
        data[i4] = (float)(-h1i+wr*h2i+wi*h2r);
        wr=(wtemp=wr)*wpr-wi*wpi+wr; // The recurrence.
        wi=wi*wpr+wtemp*wpi+wi;
    }
    if (isign == 1) {
        data[1] = (h1r=data[1])+data[2];  //Squeeze the rst and last data
                            //together to get them all within the original array.
        data[2] = h1r-data[2];
    } else {
        data[1]=c1*((h1r=data[1])+data[2]);
        data[2]=c1*(h1r-data[2]);
        four1(data,n>>1,-1); //inverse transform for the case isign=-1.
    }
}

#include "malloc.h"


int myClipPoints2(int n, double x[], double z[], double xmin, double xmax, double zmin, double zmax)
{
	// given x[] and z[], clip any points outside the current window area, and
	// output as x1[] and z1[]. Total number of points in the output is returned.
	// Note: the current window is defined in depth domain as: 
	// xmin, xmax, and zmin and zmax
	double* x1 = (double *)malloc((size_t)n*sizeof(double));
	double* z1 = (double *)malloc((size_t)n*sizeof(double));

	int i;
	int n1=0;

	// first we only deals with the xmin and xmax
	for (i=0; i<n; i++)
	{
		if(x[i]>=xmin && x[i]<=xmax)
		{
			x1[n1] = x[i];
			z1[n1] = z[i];
			n1++;
		}
		else if(i<n-1 && x[i]<xmin && x[i+1]>= xmin) {
			// now add the interception point for the left side
			x1[n1] = (float)xmin;
			z1[n1] = (float)(z[i]+(z[i+1]-z[i])*(xmin-x[i])/(x[i+1]-x[i]));
			n1++;
		}
		else if(i>0 && x[i-1]>=xmin && x[i]<xmin) {
			// now add the interception point for the left side
			x1[n1] = (float)xmin;
			z1[n1] = (float)(z[i-1]+(z[i]-z[i-1])*(xmin-x[i-1])/(x[i]-x[i-1]));
			n1++;
		}
		else if(i<n-1 && x[i]>=xmax && x[i+1]<xmax) {
			// now add the interception point for the left side
			x1[n1] = (float)xmax;
			z1[n1] = (float)(z[i]+(z[i+1]-z[i])*(xmax-x[i])/(x[i+1]-x[i]));
			n1++;
		}
		else if(i>0 && x[i-1]<xmax && x[i]>=xmax) {
			// now add the interception point for the right side
			x1[n1] = (float)xmax;
			z1[n1] = (float)(z[i-1]+(z[i]-z[i-1])*(xmax-x[i-1])/(x[i]-x[i-1]));
			n1++;
		}
	}

	// backup to x[] and z[]
	for (i=0; i<n1; i++) {
		x[i] = x1[i];
		z[i] = z1[i];
	}
	n = n1;

	// now we only deals with the zmin and zmax
	n1 = 0;
	for (i=0; i<n; i++) {
		if(z[i]>=zmin && z[i]<=zmax) {
			x1[n1] = x[i];
			z1[n1] = z[i];
			n1++;
		}
		else if(i<n-1 && z[i]<zmin && z[i+1]>= zmin) {
			// now add the interception point for the bottom side
			z1[n1] = (float)zmin;
			x1[n1] = (float)(x[i]+(x[i+1]-x[i])*(zmin-z[i])/(z[i+1]-z[i]));
			n1++;
		}
		else if(i>0 && z[i-1]>=zmin && z[i]<zmin) {
			// now add the interception point for the bottom side
			z1[n1] = (float)zmin;
			x1[n1] = (float)(x[i-1]+(x[i]-x[i-1])*(zmin-z[i-1])/(z[i]-z[i-1]));
			n1++;
		}
		else if(i>0 && z[i-1]<zmax && z[i]>=zmax) {
			// now add the interception point for the top side
			z1[n1] = (float)zmax;
			x1[n1] = (float)(x[i-1]+(x[i]-x[i-1])*(zmax-z[i-1])/(z[i]-z[i-1]));
			n1++;
		}
		else if(i<n-1 && z[i]>=zmax && z[i+1]<zmax) {
			// now add the interception point for the bottom side
			z1[n1] = (float)zmax;
			x1[n1] = (float)(x[i]+(x[i+1]-x[i])*(zmax-z[i])/(z[i+1]-z[i]));
			n1++;
		}
	}

	for (i=0; i<n1; i++) {
		x[i] = x1[i];
		z[i] = z1[i];
	}
	n = n1;

 	free(x1);
 	free(z1);

	return n;
}

int myClipPoints3(int n, float x[], float z[], double xmin, double xmax, double zmin, double zmax)
{
	double* xtemp = (double *)malloc((size_t)n*sizeof(double));
	double* ztemp = (double *)malloc((size_t)n*sizeof(double));
	int i;
	for (i=0; i<n; i++)
	{
		xtemp[i] = x[i];
		ztemp[i] = z[i];
	}
	int nNew = myClipPoints2(n, xtemp, ztemp, xmin, xmax, zmin, zmax);
	if (nNew < n) {
		for (i=0; i<n; i++)
		{
			x[i] = (float)xtemp[i];
			z[i] = (float)ztemp[i];
		}
	}
 	free(xtemp);
 	free(ztemp);

	return nNew;
}








/* 
* The following is for converting from lat long to cartesion coords
* assuming the origin is at the north pole
  */
#define PIOREV 57.29577951 // This is 180/PI.
#define RAD 6378206.0  // the Earth radius

// the origin
static double alon0 = -60.;
static double alat0 = 90.;  // NOTE: 90. is assumed in the following formula.

double* convLatlon2XY(double alon, double alat) {
	// convert (alat, alon) from polar stereographic projection to Cartesion coordinates (x, y)

	// define the reference point and longitude
//	double y0 = -2.*RAD*tan((45.-alat0*0.5)*PIO);

	double fact = 2.*RAD*cos(alat*PIO)/(1.+sin(alat*PIO));
	double *xy = new double[2];    // {x, y}
	xy[0] = fact*sin((alon-alon0)*PIO);
	xy[1] = -fact*(cos((alon-alon0)*PIO));
	return xy;
}

double* convXY2Latlon(double x, double y) {
	// convert (alat, alon) from polar stereographic projection to Cartesion coordinates (x, y)
	// define the reference point and longitude
	// double y0 = -2.*RAD*tan((45.-alat0*0.5)*PIO);

	double po = sqrt(x*x + (y*y));
	double c = 2. * atan(po/(2.*RAD));  // in gradient
	double *lonlat = new double[2];
	if (po<MINAMP) {
		lonlat[0] = alon0;
		lonlat[1] = alat0;
	}
	else {
		lonlat[0] = alon0 + atan(-x/y) * PIOREV; // in degrees
		if (lonlat[0] > 180.) lonlat[0] -= 360.;
		if (lonlat[0] < -180.) lonlat[0] += 360.;
		lonlat[1] = asin(cos(c)) * PIOREV; // in degrees
	}

	return lonlat;
}

void setShotsFromDist(int *filej, int *shotk, float *shotDist, int numShot2Sum, int j, int k, float adist) {
	// change the filej[] and shotk[] and shotDist[] according to a new distance value

	// first find the max distance in the array
	float adistMax = 0.f;
	int iForDistMax = -1;
	for (int i=0; i<numShot2Sum; i++) {
		if (shotDist[i]> adistMax) {
			adistMax = shotDist[i];
			iForDistMax = i;
		}
	}
	if (iForDistMax>=0 && adist<adistMax) {
		// now, the new shot at adist is better than the maximum distance in the array shotDist[]
		filej[iForDistMax] = j;
		shotk[iForDistMax] = k;
		shotDist[iForDistMax] = adist;
	}

}

void filterTimeDomain(float *rData, int outLen, float f1, float f2, long tpp) {
	double a,b, c0,c1,c2, d1,d2;
	int i;

	if(outLen<2 || tpp < 1) {
		return;
	}

	double dt = (double)tpp/(double)MIL;

	a=tan(PI*f1*dt);
	if (f2 < 0.f) f2 = 0.5f / (float)dt;
	b=tan(PI*f2*dt);
	c0 = -b/(1.+a)/(1.+b);
	c1 = 0.;
	c2 = -c0;
	d1 = ((1.+a)*(1.-b)+(1.-a)*(1.+b))/((1.+a)*(1.+b));
	d2 = -((1.-a)*(1.-b))/((1.+a)*(1.+b));
	
	// first remove the DC energy
	//float aMedian = (float)getMedian(&rData[0], outLen);	
	//for (i=0; i<outLen; i++) rData[i] -= aMedian;

	double temp = 0.;
	for (i=0; i<outLen; i++) {
		temp+= rData[i];
	}
	if(fabs(temp)>MINAMP){ 
		float temp2 = (float) (temp / (float)outLen);
		for (i=0; i<outLen; i++) rData[i] -= temp2;
	}


	float* tData = (float *)malloc((size_t)outLen*sizeof(float));
	tData[0] = (float)c0*rData[0];
	tData[1] = (float)(c0*rData[1]+c1*rData[0] + d1*tData[0]);
	for (i=2; i<outLen; i++)
		tData[i] = (float)(c0*rData[i]+c1*rData[i-1]+c2*rData[i-2] + d1*tData[i-1]+d2*tData[i-2]);
	for (i=0; i<outLen; i++)
		rData[i] = -tData[i];
	
	
	free(tData);
}

double removeTimeLine(float *rData, int itwmin, int itwmax, int itwdt) {
	// now we need to calculate remove the timeline within a small windows of twdt=0.08 sec 
	// this actual window should be between  itwmin and itwmax
	double sum2 = 0.;
	int itwminChosen = itwmin;
	int ii;

	for (ii=itwmin; ii<itwmax - itwdt; ii++) {
				double sum3 = 0.;
				for (int jj=ii; jj<ii+itwdt; jj++) {
					sum3 += (double)fabs(rData[jj]);
				}
				if (sum3 > sum2) {
					sum2 = sum3;
					itwminChosen = ii;
				}
	}
	
	// now do weighted trim in the window for removing a timeline
	int idt = (int)((double)itwdt * 0.15);
	int itwmaxChosen = itwminChosen + itwdt;
	for (ii=itwminChosen; ii<itwmaxChosen; ii++) {
				if (ii<itwminChosen+idt) {
					rData[ii] *= (float)(1. - (double)(ii-itwminChosen)/(double)idt);
				}
				else if (ii>itwmaxChosen-idt) {
					rData[ii] *= (float)(1. - (double)(itwmaxChosen - ii)/(double)idt);
				}
				else {
					rData[ii] = 0.f;
					//	tr.dat.data[ii] *= 100.f;
				}
	}
	return sum2;  // save for later
}


/*
################################################################## 
# calc_dist(lat1, long1, lat2, long2)
#
# This function performs a geodectic inverse computation for ellipsodial
# arc distances between two wells by the Forsythe-Andover-Lambert method.
#
# Note this function is based on the DISTAN fortran program written
# by Nick Stutfbergen (Navigation, BIO 01/84) and modified by 
# Phil Moir (03/85) and Rick McCallum (02/89)
#
# Modified slightly to work in PHP by Adam Kelloway, December, 2004
# and by Deping Chian, Mar 15, 2005
# returns a distance in KM
##################################################################
*/
	double calcDist(double long1, double lat1, double long2, double lat2) {
		// avoid overflow
		if (fabs(lat1-lat2)<0.00001 && fabs(long1-long2)<0.00001) return 0.0;
		
		// # Constants
		double AE=6378206.4;  
		double FL=1/294.9787;
		// double PI=3.1415925635;
		double DR=3.14159/180;
		double FLAG=1.0;

		if ((fabs(lat1-lat2) > 90) || (fabs(long2-long1) > 90)) {
			FLAG=2.0;
			lat1=lat1*0.5;
			lat2=lat2*0.5;
			long1=long1*0.5; 
			long2=long2*0.5;     
		}
		if (fabs(long2-long1) >180) {
			FLAG=4.0;
			lat1=lat1/4.0;
			lat2=lat2/4.0;
			long1=long1/4.0;
			long2=long2/4.0;
		}

		// # Convert values from degrees to Radians
		lat1*=DR; 
		long1*=DR; 
		lat2*=DR;  
		long2*=DR;      

		double PHM=(lat2+lat1)*0.5;
		double DPM=(lat2-lat1)*0.5;
		double DLON=long2-long1;
		double DLM=DLON*0.5;
	
		// #   CALCULATIONS
		double SPM=sin(PHM);
		double CPM=cos(PHM);
		double SDP=sin(DPM);
		double CDP=cos(DPM);
		double SDL=sin(DLM);
		double CDL=cos(DLM);

		// #   SPHERICAL ARC DISTANCE BY HALF-ANGLE FORMULA
		double DN=SDP*CDL;
		double DE=SDL*CPM;

		double SDH=sqrt(DN*DN+DE*DE);
		double CDH=sqrt(1.0-SDH*SDH);
		double SIND=2.0*SDH*CDH;
		double COSD=sqrt(1.0-SIND*SIND);

		// #   D=SPHERICAL ARC(RADS),SARC.DIST(METERS)
		double ARCD=atan2(SIND,COSD);
		double S=AE*ARCD;

		// #   SET UP ELLIPSOIDAL CORRECTION TERMS
		double VK = SPM*CDP;
		double VKK = SDP*CPM;

		double U=2.0*VK*VK/(CDH*CDH);
		double V=2.0*VKK*VKK/(SDH*SDH);
		double X=U+V;
		double Y=U-V;

		double T=ARCD/SIND;
		double E=30.0*COSD;
		double A=4.0*T*(8.0+T*E/15.0);
		double D=4.0*(6.0+T*T);
		double B=(D+D)*(-1);
		double C=T-(A+E)/2.0;

		double DS=-AE*SIND*(FL/4.0)*(T*X-3.0*Y);
		double DDS=AE*SIND*(FL*FL/64.0)*(X*(A+C*X)+Y*(B+E*Y)+D*X*Y);

		// #   ELLIPSOIDAL DIST=SPHERIACL DIST + 1ST AND 2ND ORDER TERMS
		double DIST=S+DS+DDS;

		double CHECK=fabs(DLM)+fabs(DPM);

		if (CHECK < 0.000001) {
			DIST=0.00;
		}

		DIST=DIST*0.001*FLAG;

		return DIST;
	}

//######################## End of calc_dist() ##################################


/*
TI2203C.txt   Convert Hex String to an Integer Value.
Category   :General
Platform    :All
Product    :C/C++  All

Description:
This document provides a method to read hexadecimal numbers.
The _tstoi() function ignores the A-F digits in a hexadecimal
number.  In fact, the first non-digit character in the string
passed to _tstoi() ends the conversion.
The following example converts a four-character text string that
represents a hexadecimal number into an integer.  It can be used
as a template to create other ASCII-to-number conversions.
The example code defines a C\C++ function called axtoi() that
does the conversion.  It includes a short main() that to test the
function.
*/
int axtoi(char *hexStg) {
  int n = 0;         // position in string
  int m = 0;         // position in digit[] to shift
  int count;         // loop index
  int intValue = 0;  // integer value of hex string
  int digit[5];      // hold values to convert
  while (n < 4) {
     if (hexStg[n]=='\0')
        break;
     if (hexStg[n] > 0x29 && hexStg[n] < 0x40 ) //if 0 to 9
        digit[n] = hexStg[n] & 0x0f;            //convert to int
     else if (hexStg[n] >='a' && hexStg[n] <= 'f') //if a to f
        digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
     else if (hexStg[n] >='A' && hexStg[n] <= 'F') //if A to F
        digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
     else break;
    n++;
  }
  count = n;
  m = n - 1;
  n = 0;
  while(n < count) {
     // digit[n] is value of hex digit at position n
     // (m << 2) is the number of positions to shift
     // OR the bits into return value
     intValue = intValue | (digit[n] << (m << 2));
     m--;   // adjust the position to set
     n++;   // next digit to process
  }
  return (intValue);
}








//##############################################################
//##############################################################
//##############################################################
//##############################################################
//##############################################################
//##############################################################
/* Copyright (c) Colorado School of Mines, 2008.*/
/* All rights reserved.                       */

/*********************** self documentation **********************/
/*****************************************************************************
BUTTERWORTH - Functions to design and apply Butterworth filters:

bfdesign	design a Butterworth filter
bfhighpass	apply a high-pass Butterworth filter 
bflowpass	apply a low-pass Butterworth filter 

******************************************************************************
Function Prototypes:
void bfhighpass (int npoles, float f3db, int n, float p[], float q[]);
void bflowpass (int npoles, float f3db, int n, float p[], float q[]);
void bfdesign (float fpass, float apass, float fstop, float astop,
	int *npoles, float *f3db);

******************************************************************************
bfdesign:
Input:
fpass		frequency in pass band at which amplitude is >= apass
apass		amplitude in pass band corresponding to frequency fpass
fstop 		frequency in stop band at which amplitude is <= astop
astop		amplitude in stop band corresponding to frequency fstop

Output:
npoles		number of poles
f3db		frequency at which amplitude is sqrt(0.5) (-3 db)

bfhighpass and bflowpass:
Input:
npoles		number of poles (and zeros); npoles>=0 is required
f3db		3 db frequency; nyquist = 0.5; 0.0<=f3db<=0.5 is required
n		length of p and q
p		array[n] to be filtered

Output:
q		filtered array[n] (may be equivalent to p)

******************************************************************************
Notes:
(1) Nyquist frequency equals 0.5

(2) The following conditions must be true:
	(0.0<fpass && fpass<0.5) &&
	(0.0<fstop && fstop<0.5) &&
	(fpass!=fstop) &&
	(0.0<astop && astop<apass && apass<1.0)

(3) if (fpass<fstop)

bfdesign:
Butterworth filter:  compute number of poles and -3 db frequency
for a low-pass or high-pass filter, given a frequency response
constrained at two frequencies.

******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
/**************** end self doc ********************************/

void
bfdesign (float fpass, float apass, float fstop, float astop,
	int *npoles, float *f3db)
/*****************************************************************************
Butterworth filter:  compute number of poles and -3 db frequency
for a low-pass or high-pass filter, given a frequency response
constrained at two frequencies.
******************************************************************************
Input:
fpass		frequency in pass band at which amplitude is >= apass
apass		amplitude in pass band corresponding to frequency fpass
fstop 		frequency in stop band at which amplitude is <= astop
astop		amplitude in stop band corresponding to frequency fstop

Output:
npoles		number of poles
f3db		frequency at which amplitude is sqrt(0.5) (-3 db)
******************************************************************************
Notes:
(1) Nyquist frequency equals 0.5

(2) The following conditions must be true:
	(0.0<fpass && fpass<0.5) &&
	(0.0<fstop && fstop<0.5) &&
	(fpass!=fstop) &&
	(0.0<astop && astop<apass && apass<1.0)

(3) if (fpass<fstop) 
		a low-pass filter is assumed
	else
		a high-pass filter is assumed
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
{
	float wpass,wstop,fnpoles,w3db;

	/* warp frequencies according to bilinear transform */
	wpass = (float)(2.0*tan(PI*fpass));
	wstop = (float)(2.0*tan(PI*fstop));

	/* if lowpass filter, then */
	if (fstop>fpass) {
		fnpoles = (float)log((1.0/(apass*apass)-1.0)/(1.0/(astop*astop)-1.0))
			/ (float)log(pow((double)(wpass/wstop),2.0));
		w3db = (float)(wpass/pow((1.0/(apass*apass)-1.0),0.5/fnpoles));

	/* else, if highpass filter, then */
	} else {
		
		fnpoles = (float)log((1.0/(apass*apass)-1.0)/(1.0/(astop*astop)-1.0))
			/ (float)log(pow((double)(wstop/wpass),2.0));
		w3db = (float)(wpass*pow((1.0/(apass*apass)-1.0),0.5/fnpoles));
	}

	/* determine integer number of poles */
	*npoles = 1+(int)fnpoles;

	/* determine (unwarped) -3 db frequency */
	*f3db = (float)(atan(0.5*w3db)/PI);
}

void
bfhighpass (int npoles, float f3db, int n, float p[], float q[])
/*****************************************************************************
Butterworth filter:  high-pass
******************************************************************************
Input:
npoles		number of poles (and zeros); npoles>=0 is required
f3db		3 db frequency; nyquist = 0.5; 0.0<=f3db<=0.5 is required
n		length of p and q
p		array[n] to be filtered

Output:
q		filtered array[n] (may be equivalent to p)
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
{
	int jpair,j;
	float r,scale,theta,a,b1,b2,pj,pjm1,pjm2,qjm1,qjm2;

	r = (float)( 2.0*tan(PI*fabs(f3db)) );
	if (npoles%2!=0) {
		scale = r+2.0f;
		a = 2.0f/scale;
		b1 = (r-2.0f)/scale;
		pj = 0.0f;
		qjm1 = 0.0f;
		for (j=0; j<n; j++) {
			pjm1 = pj;
			pj = p[j];
			q[j] = a*(pj-pjm1)-b1*qjm1;
			qjm1 = q[j];
		}
	} else {
		for (j=0; j<n; j++)
			q[j] = p[j];
	}
	for (jpair=0; jpair<npoles/2; jpair++) {
		theta = (float)( PI*(2*jpair+1)/(2*npoles) );
		scale = (float)( 4.0+4.0*r*sin(theta)+r*r );
		a = 4.0f/scale;
		b1 = (float)( (2.0*r*r-8.0)/scale );
		b2 = (float)( (4.0-4.0*r*sin(theta)+r*r)/scale );
		pjm1 = 0.0f;
		pj = 0.0f;
		qjm2 = 0.0f;
		qjm1 = 0.0f;
		for (j=0; j<n; j++) {
			pjm2 = pjm1;
			pjm1 = pj;
			pj = q[j];
			q[j] = (float)( a*(pj-2.0*pjm1+pjm2)-b1*qjm1-b2*qjm2 );
			qjm2 = qjm1;
			qjm1 = q[j];
		}
	}
}

void
bflowpass (int npoles, float f3db, int n, float p[], float q[])
/*****************************************************************************
Butterworth filter:  low-pass
******************************************************************************
Input:
npoles		number of poles (and zeros); npoles>=0 is required
f3db		3 db frequency; nyquist = 0.5; 0.0<=f3db<=0.5 is required
n		length of p and q
p		array[n] to be filtered

Output:
q		filtered array[n] (may be equivalent to p)
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/02/89
*****************************************************************************/
{
	int jpair,j;
	float r,scale,theta,a,b1,b2,pj,pjm1,pjm2,qjm1,qjm2;

	r = (float)( 2.0*tan(PI*fabs(f3db)) );
	if (npoles%2!=0) {
		scale = r+2.0f;
		a = (float)( r/scale );
		b1 = (float)( (r-2.0)/scale );
		pj = 0.0f;
		qjm1 = 0.0f;
		for (j=0; j<n; j++) {
			pjm1 = pj;
			pj = p[j];
			q[j] = a*(pj+pjm1)-b1*qjm1;
			qjm1 = q[j];
		}
	} else {
		for (j=0; j<n; j++)
			q[j] = p[j];
	}
	for (jpair=0; jpair<npoles/2; jpair++) {
		theta = (float)( PI*(2*jpair+1)/(2*npoles) );
		scale = (float)( 4.0+4.0*r*sin(theta)+r*r );
		a = (float)( r*r/scale );
		b1 = (float)( (2.0*r*r-8.0)/scale );
		b2 = (float)( (4.0-4.0*r*sin(theta)+r*r)/scale );
		pjm1 = 0.0f;
		pj = 0.0f;
		qjm2 = 0.0f;
		qjm1 = 0.0f;
		for (j=0; j<n; j++) {
			pjm2 = pjm1;
			pjm1 = pj;
			pj = q[j];
			q[j] = (float)( a*(pj+2.0*pjm1+pjm2)-b1*qjm1-b2*qjm2 );
			qjm2 = qjm1;
			qjm1 = q[j];
		}
	}
}




void setDistance(segy* tr, int distInMeter){
	// note: here cdp is the actual distance in meter
	tr->idistopt = 8; // by using 8 here, we indicate that we will use IBM floating data from now on.
	tr->trcdist = (float)distInMeter*0.001f;
}

void setDistance(segy* tr, float fDistKm){
	tr->idistopt = 8; // by using 8 here, we indicate that we will use IBM floating data from now on.
	tr->trcdist = fDistKm;
}

void setDistance(segydynamic* tr, float fDistKm){
	tr->idistopt = 8; // by using 8 here, we indicate that we will use IBM floating data from now on.
	tr->trcdist = fDistKm;
}

void setDistance(segy* tr, double dDistKm){
	tr->idistopt = 8; // by using 8 here, we indicate that we will use IBM floating data from now on.
	tr->trcdist = (float)dDistKm;
}


void setTraceDelay(segy* tr, int iTimeDelay){
	// will access segy headers:    short tr.delrt, int tr.t1
	if (iTimeDelay < MAXSHORT) {
		tr->delrt = iTimeDelay;
		tr->t1 = 0;
	}
	else {
		tr->delrt = 0;
		tr->t1 = iTimeDelay;
	}

	if (abs(tr->t1)>90000000) {
		tr->t1 = 0;
	}

}

void setTraceDelay(segydynamic* tr, int iTimeDelay){
	// will access segy headers:    short tr.delrt, int tr.t1
	if (iTimeDelay < MAXSHORT) {
		tr->delrt = iTimeDelay;
		tr->t1 = 0;
	}
	else {
		tr->delrt = 0;
		tr->t1 = iTimeDelay;
	}

	if (abs(tr->t1)>90000000) {
		tr->t1 = 0;
	}

}

float getTraceDelay(segy * tr){
	//if (tr->tracl == 383) {
	//	int rter = 1;
	//}
	// fix up first, since some programs ...
	//if (tr->t1 > 1000000) tr->t1 = 0;
	//else
	//	if (tr->t1 < -1000000) tr->t1 = 0;

	if (abs(tr->t1) > 10000)  tr->t1 = 0;
	float tShift = (tr->delrt+tr->t1)*.001f;
	if (fabs(tShift) > 99999.) {
		// we see an invalid initial time. So we ignore it.
		tShift = 0.;
	}

	return tShift;
}

// getTraceXVal(sw.trArr[iTrc+num], draw.mode, para.isDistanceDisplay);  NOTE: do not pass in &sw.trArr[iTrc+num]
float getTraceXVal(segydynamic *tr, int drawmode, BOOL isDistance) {
	if (drawmode == 6) return (float)tr->tracl;  // this is trace display
	
	return (isDistance==0)  ?  getOffset(tr)   :   getDistance(tr);
}

float getTraceDelay(segydynamic * trdyn){
	// fix up first, since some programs ...
	if (trdyn->t1 > 1000000) trdyn->t1 = 0;

	return (trdyn->delrt+trdyn->t1)*.001f;
}



float getTraceAmp(segydynamic * trdyn, float t, float sampRate){
	float out = 0.f;
	if ( sampRate > 0.0000001f ) {
		float ai =  ( t - getTraceDelay(trdyn) ) / sampRate;
		int i = (int)ai;
		if (i == trdyn->ns-1) {
			out = trdyn->data[i];
		}
		else if (i >= 0 && i < trdyn->ns-1) {
			out = trdyn->data[i] + (float)(ai - i) * (trdyn->data[i+1] - trdyn->data[i]);
		}
	} // if
	return out;
}

void setSegyTotalTrace(CFile *file, unsigned short numtotal) {
	if (file == NULL) return;
	if(numtotal>0) 
	{
		file->Seek(3212L, CFile::begin);
		file->Write(&numtotal, 2);
	}
}

void freeSegyArray(segydynamic *trArr, int nxmax) {
	// clean up before return
	if (trArr != NULL) {
		for (int jj=0; jj<nxmax; jj++) {
			if (trArr[jj].data != NULL) {
				free(trArr[jj].data); 
				trArr[jj].data = NULL;
			}
		}
		free(trArr); 
		trArr = NULL;
	} // if
}

// will return the total number of successful traces read into the array
// if return 0, meaning no traces read into the array       // not used
int loadSegy2ArrayDelete(CString f, segydynamic *trArr, bhed *bh, BOOL isIBMFloatSegy) {
	CFile file;
	segy tr;
	int j;
	if (!file.Open(f, CFile::modeRead | CFile::typeBinary)) {
		return 0;
	}
	file.Seek(3200, CFile::begin);
	file.Read(bh, 400L);
	int len;
	if (bh->format != 3)
		len = bh->hns*4 + 240;
	else
		len = bh->hns*2 + 240;
	int numsamp = bh->hns;

	int nxmax = 0;  // total number of traces in the input segy file
	while (file.Read(&tr, 4) == (UINT)4)
	{
		file.Seek(len-4, CFile::current);
		nxmax ++;
	}
	file.Seek(3600, CFile::begin);
	trArr = (segydynamic *)malloc( sizeof(segydynamic) * nxmax );
	for (j=0; j< nxmax; j++) {
		trArr[j].data = (float *)malloc(sizeof(float)* numsamp);
		if (ERROR == trArr[j].data) {
			freeSegyArray(trArr, j);
			AfxMessageBox(_T("Sorry, failed to allocate memory for loading all traces into memory."));
			file.Close();
			return 0;
		}
	} // for j

	int nxStore = 0;
	while (file.Read(&tr, len) == (UINT)len)
	{
		if(tr.trid == 2) //dead traces.
		{
			file.Seek(len, CFile::current); 
			continue ;
		}

		// assign only the trace header to the trArr[]
		memcpy( (void *)&trArr[nxStore], (void *)&tr, 240L);
		if(bh->format==3)
			for (j=0; j<numsamp; j++) trArr[nxStore].data[j] = (float)tr.dat.datashort[j];
		else if (bh->format==1) {
			if (tr.idistopt==8 || isIBMFloatSegy)
				ibm_to_float((int *) tr.dat.data, (int *) tr.dat.data, (int)numsamp);
			for (j=0; j<numsamp; j++) trArr[nxStore].data[j] = (float)tr.dat.data[j];
		}
		else if (bh->format==2) 
			for (j=0; j<numsamp; j++) trArr[nxStore].data[j] = (float)tr.dat.dataint[j];
		else break;

		double aMedian = getMedianFast(&trArr[nxStore].data[0], numsamp);	
		if (aMedian < MINAMP) continue;
		nxStore++;
	} // while file2.Read
	file.Close();

	// now remove any array elements not in use
	if (nxStore < nxmax) {
		for (j=nxStore; j<nxmax; j++) {
			free(trArr[j].data); trArr[j].data = NULL;
		} // for
	} // if

	return nxStore;
}



// depth[4000],vTop[4000],vBot[4000] // these 3 arrays must be big enough to expand
//	ncont = formatVelocityArray(dvUniform, velMod.ncont, 4000, (double *)&depth[0], (double *)&vTop[0], (double *)&vBot[0]);
int formatVelocityArray(float dvUniform, int ncont, int nmax, double depth[], double vTop[], double vBot[])
{
	if (dvUniform <= 0.f) dvUniform = 0.01f;
	if (ncont >= nmax) return ncont; // we cannot split any more since the array size limit is reached

	for (int i=0; i<ncont; i++) {  
		if (fabs(vBot[i] - vTop[i]) > dvUniform) {
			// if here, the velocity gradient is too great, we split the layer into smaller pieces 
			for (int icont = ncont-1; icont>i; icont--) {
				depth[icont+1] = depth[icont];
				vTop[icont+1] = vTop[icont]; // no meaning for vTop[ncont]
				vBot[icont+1] = vBot[icont];
			}
			depth[i+1] = depth[i] + dvUniform * (depth[i+1] - depth[i]) / (vBot[i] - vTop[i]);
			vTop[i+1] = vTop[i] + dvUniform;
			vBot[i+1] = vBot[i];
			vBot[i] = vTop[i+1];
			ncont ++;
			return formatVelocityArray(dvUniform, ncont, nmax, (double *)&depth[0], (double *)&vTop[0], (double *)&vBot[0]);
		}
	} // for i
	return ncont;
}

bool setAppendToFileText(CString f, CString line)
{
	if (line.GetLength() == 0) return false;
	CFile file;
	if (!file.Open(f, CFile::modeReadWrite|CFile::modeNoTruncate)) 
		return false;
	file.SeekToEnd();
	file.Write("\r\n", 2);
	file.Write(line, (UINT)(line.GetLength()*sizeof(TCHAR) ) );
	file.Close();
	return true;
}





//	isSuc = getTwttFromVz(dvUniform, velMod.ncont, 4000, (double *)&depth[0], (double *)&vTop[0], (double *)&vBot[0], nout, dz, twtt);
//	ncont = formatVelocityArray(dvUniform, ncont, 4000, (double *)&depth[0], (double *)&vTop[0], (double *)&vBot[0]);
bool getTwttFromVz(int ncont, double depth[], double vTop[], double vBot[], int nz, double fz, double dz, double *twtt) {
	// note: outputting twtt[iz] has iz=0 pointing to the depth at fz  km, not at the surface !
	// only fz=0 is tested well !
	if (twtt == NULL || ncont < 2 || nz < 2 || dz <= 0.f) return FALSE;

	int nzmaxModelAll = (int)(depth[ncont-1] / dz + 2.0);
	if (nzmaxModelAll < 2) return FALSE;
 

	if (nzmaxModelAll < nz) nzmaxModelAll = nz; // nzmaxModelAll should cover >= nz points for output
	double *twttAll = (double *)malloc(sizeof(double)*nzmaxModelAll);
	bool isError = false;

	int iz, icont;
	twttAll[0] = 0.f;
	for (iz=1; iz<nzmaxModelAll; iz++) {
		register double z = iz*dz;

		// first search for proper velocity for the current depth
		register double v = -1.;
		register bool isTakenCareOf = false;
		for (icont=0; icont<ncont-1; icont++) {
			register double z1 = depth[icont];
			register double z2 = depth[icont+1];
			if (z1 > z2) {
				// abnormal layering
				z1 = z2;
				z2 = depth[icont];
			}

			if (z >= z1 && z <= z2) {
				if (icont>0 && z-dz < z1 && z>=z1) {
					// the increment crosses the boundary at z1
					twttAll[iz] = twttAll[iz-1] + 
						2.*(z1-(z-dz))/vBot[icont-1] +
						2.*(z-z1)/vTop[icont];
				}
				else if (z>=z1 && z <= z2) {
					// it points inside the layer icont in between [z1, z2] 
					register double dzc = z2 - z1;
					if (dzc > 0.00001)
						v =  vTop[icont] + (vBot[icont] - vTop[icont]) / dzc * (z - z1);
					else
						v = (vTop[icont] + vBot[icont]) * 0.5;
					twttAll[iz] = twttAll[iz-1] + 2.*dz/v;
				} else {
					v = (float)vBot[ncont-2]; // note: last valid layer is index-ed at ncont-2!
					twttAll[iz] = twttAll[iz-1] + 2.*dz/v;
				}
				isTakenCareOf = true;
				break;
			}
		} // for icont

		if (!isTakenCareOf) {
			// since the top boundary is always 0, the only way that it is not taken care of
			// if that the point is below the last boundary
			v = (float)vBot[ncont-2]; // note: last valid layer is index-ed at ncont-2!
			twttAll[iz] = twttAll[iz-1] + 2.*dz/v;
		}
	} // for iz

	if (fz == 0.f) {
		memcpy((void *)&twtt[0], (void *)&twttAll[0], sizeof(double)*nz ); 
	}
	else {
		for (int izout=0; izout<nz; izout++) {
			double zout = izout*dz + fz;
			double zi = zout / dz;
			int izi = (int)zi;
			double frac = zi - izi;
			if (izi+1<nzmaxModelAll) {
				twtt[izout] = twttAll[izi] + (  (twttAll[izi+1] - twttAll[izi])*frac  );
			}
			else {
				// last point in the whole array???
				twtt[izout] = twttAll[izi];
			}
		} // for izout
	}

	free(twttAll);
	return !isError;
}

bool getTwttFromVz(int ncont, double depth[], double vTop[], double vBot[], int nz, double fz, double dz, float *twtt) {
	double *twttdouble = (double *)malloc(sizeof(double)*nz);
	bool isSuc = getTwttFromVz(ncont, depth, vTop, vBot, nz, fz, dz, twttdouble);
	for (int i=0; i<nz; i++) {
		twtt[i] = (float)twttdouble[i];
	}
	free(twttdouble);
	return isSuc;
}

bool getDepthFromVt(int ncont, double timeMod[], double vTop[], double vBot[], int nt, double dt, double *depth) {
	if (depth == NULL || ncont < 2 || nt < 2 || dt <= 0.f) return FALSE;

	int ntmaxModelAll = (int)(timeMod[ncont-1] / dt + 2.0);
	if (ntmaxModelAll < 2) return FALSE;
	if (ntmaxModelAll < nt) ntmaxModelAll = nt; // ntmaxModelAll should cover >= nt points for output
//	double *depthDouble = (double *)malloc(sizeof(double)*ntmaxModelAll);
	bool isError = false;

	depth[0] = 0.;
	int it, icont;
	for (it=1; it<ntmaxModelAll; it++) {
		register double t = it*dt;

		// first search for proper velocity for the current depth
		register double v = -1.;
		register bool isTakenCareOf = false;
		for (icont=0; icont<ncont-1; icont++) {
			register double t1 = timeMod[icont];
			register double t2 = timeMod[icont+1];
			if (t1 > t2) {
				// abnormal layering
				t1 = timeMod[icont+1];
				t2 = timeMod[icont];
			}

			if (t >= t1 && t <= t2) {
				if (icont>0 && t-dt < t1 && t>=t1) {
					// the increment crosses the boundary at t1
					depth[it] = depth[it-1] + 
						0.5*(t1-(t-dt))*vBot[icont-1] +
						0.5*(t-t1)*vTop[icont];
				}
				else if (t>=t1 && t <= t2) {
					// it points inside the layer icont in between [t1, t2] 
					register double dtc = t2 - t1;
					if (dtc > 0.00001)
						v =  vTop[icont] + (vBot[icont] - vTop[icont]) / dtc * (t - t1);
					else
						v = (vTop[icont] + vBot[icont]) * 0.5;
					depth[it] = depth[it-1] + 0.5*dt*v;
				} else {
					v = (float)vBot[ncont-2]; // note: last valid layer is index-ed at ncont-2!
					depth[it] = depth[it-1] + 0.5*dt*v;
				}
				isTakenCareOf = true;
				break;
			}
		} // for icont

		if (!isTakenCareOf) {
			// since the top boundary is always 0, the only way that it is not taken care of
			// if that the point is below the last boundary
			v = (float)vBot[ncont-2]; // note: last valid layer is index-ed at ncont-2!
			depth[it] = depth[it-1] + 0.5*dt*v;
		}
	} // for it

//	memcpy((void *)&depth[0], (void *)&depthDouble[0], sizeof(double)*nt ); 
	//for (it=0; it<nt; it++) {
	//	depth[it] = (float)depthDouble[it];
	//}

	//free(depthDouble);
	return !isError;
}



bool getTwttModFromZModAtDistance(int ncont, double depth[], double vTop[], double vBot[], double dz, double dist, double *twttMod) {
	// note: outputting twttMod[icont] with ***conversion***.
	// depth[], vTop[], vBot[]  must already have been calculated
	if (twttMod == NULL || ncont < 2 || dz <= 0.) return FALSE;

	int nzmaxModelAll = (int)(depth[ncont-1] / dz + 2.0);
	if (nzmaxModelAll < 2) return FALSE;

	double *tTrc = (double *)malloc(sizeof(double)*nzmaxModelAll);

	getTwttFromVz(ncont, (double *)&depth[0], 
				(double *)&vTop[0], (double *)&vBot[0], 
				nzmaxModelAll, 0., dz, 
				tTrc); 

	double val;
	for (int icont=0; icont<ncont; icont++) {
				// we want :  depth[icont] ==>  twttMod[icont]. Note: depth[ncont-1] is the deepest boundary in the model
				bool isSuc = false;
				twttMod[icont] = 0.f;  // initialize
				for (int iz=0; iz<nzmaxModelAll-1; iz++) {
					float z = (float)(iz*dz);
					if ( z + dz >= depth[icont] ) {
						// if here, the current iz is the closest to depth[icont] ==>  time[icont]
						// boundary twttMod[icont] is between tTrc[iz] and tTrc[iz+1]
						//twttMod[icont] = tTrc[iz] + (float)(  (tTrc[iz+1]-tTrc[iz])/dz * (depth[icont]-z)  );
						register double v = (icont < ncont-1)  ?  vBot[icont]  :  vBot[ncont-2];
						val = (depth[icont]-z) / v * 2.;
						twttMod[icont] = tTrc[iz] + val;
						// now we got TWTT writen in twttMod[icont]
						isSuc = true;
						break;
					}
					else if ( icont==ncont-1 && iz==nzmaxModelAll-2 && nzmaxModelAll>3) {
						// if here, the last boundary needs fixing
						twttMod[icont] = tTrc[nzmaxModelAll-3] + (   (tTrc[nzmaxModelAll-2]-tTrc[nzmaxModelAll-3])/dz * ( depth[icont]-dz*(nzmaxModelAll-3) )   );
						// now we got TWTT writen in twttMod[icont]
						isSuc = true;
						break;
					}
				} // for iz
				if (!isSuc) {
					if (icont>0) {
						twttMod[icont] = twttMod[icont-1] + ( 4.*(depth[icont]-depth[icont-1])/(vBot[icont-1]+vTop[icont-1]) );
					}
					//AfxMessageBox(_T("A boundary cannot be converted into TWTT domain");
				}
	} // for icont

	free(tTrc);
	return true;
}


bool getDepthModFromTwtt(int ncont, double timeMod[], double vTop[], double vBot[], double dt, double dist, double *depthMod) {
	// timeMod[], vTop[], vBot[]  must already have been calculated
	if (depthMod == NULL || ncont < 2 || dt <= 0.f) return FALSE;

	int ntmaxModelAll = (int)(timeMod[ncont-1] / dt + 2.0);
	if (ntmaxModelAll < 2) return FALSE;

	double *zTrc = (double *)malloc(sizeof(double)*ntmaxModelAll);

	getDepthFromVt(ncont, (double *)&timeMod[0], (double *)&vTop[0], (double *)&vBot[0], ntmaxModelAll, dt, zTrc); 

	double val;
	for (int icont=0; icont<ncont; icont++) {
				// we want :  time[icont] ==>  depthMod[icont]. Note: time[ncont-1] is the deepest boundary in the model
				register bool isSuc = false;
				depthMod[icont] = 0.f;  // initialize
				for (int it=0; it<ntmaxModelAll-1; it++) {
					register double t = it*dt;
					if ( t + dt >= timeMod[icont] ) {
						// if here, the current iz is the closest to depth[icont] ==>  time[icont]
						// the situation: the boundary is below zTrc[it] and above zTrc[it+1]
						//     we need to compensate for the space in between zTrc[it] and model boundary
						//     a linear interpolation has errors of ~0.001sec (because of varying velocity) which can have problem
						// double valTest = (zTrc[it+1]-zTrc[it])/dt * (timeMod[icont]-t);
						register double v = (icont < ncont-1)  ?  vBot[icont]  :  vBot[ncont-2];
						val = v * (timeMod[icont]-t) * 0.5;
						depthMod[icont] = zTrc[it] + val; 
						if (val > zTrc[it+1] - zTrc[it]) {
							int ifsd = 1;  // this would be prob
						}
						// now we got TWTT writen in twttMod[icont]
						isSuc = true;
						break;
					}
					else if ( icont==ncont-1 && it==ntmaxModelAll-2 && ntmaxModelAll>3) {
						// if here, the last boundary needs fixing
						val = (zTrc[ntmaxModelAll-2]-zTrc[ntmaxModelAll-3])/dt * (timeMod[icont]-dt*(ntmaxModelAll-3));
						depthMod[icont] = zTrc[ntmaxModelAll-3] + val;
						// now we got TWTT writen in twttMod[icont]
						isSuc = true;
						break;
					}
				} // for iz
				if (!isSuc) {
					if (icont>0) {
						val = 0.25*(timeMod[icont]-timeMod[icont-1])*(vBot[icont-1]+vTop[icont-1]);
						depthMod[icont] = depthMod[icont-1] + val;
					}
				}
	} // for icont

	free(zTrc);
	return true;
}


int getVelocityIntFromRms(int ntrms, float *tau, float *vrms, float *vintOut) {
	// tau[0] should be at the surface with zero time
	if (tau==NULL || vrms == NULL || vintOut == NULL || ntrms < 2) return 0;
	int it;

	float *vrmsTrc = (float *)malloc(sizeof(float)*ntrms);

	// now square it
	for (it=0; it<ntrms; it++) {
		vrmsTrc[it] = vrms[it] * vrms[it];
	}

	// now obtain the interval velocity square
	for (it=0; it<ntrms-1; it++) {
		if (tau[it+1] - tau[it] < 0.00001f) {
			vintOut[it] = vrmsTrc[it];
		}
		else {
			vintOut[it] = vrmsTrc[it+1] * tau[it+1] - vrmsTrc[it] * tau[it];
			vintOut[it] /= (tau[it+1] - tau[it]);
		}
	} // for it
	vintOut[ntrms-1] = vintOut[ntrms-2];
	free(vrmsTrc);

	// showArrayInFile(&vint[0], nt);

	for (it=0; it<ntrms; it++) {
		vintOut[it] = sqrt(vintOut[it]);
	}

	return ntrms;
}

bool getVelocityIntFromRms(int ntrms, float *tau, float *vrms, int nt, float dt, float ft, float *vint) {
	// the results of this routine is not perrfect. Recommend to Use the simpler instead.
	if (tau==NULL || vrms == NULL || vint == NULL || ntrms < 2 || nt < 2) return false;
	int itao,it, it2, it3;
	float t, delta;

	float *vrmsTrc = (float *)malloc(sizeof(float)*nt);

	// first do an interpolation of vrms[] and store into vrmsTrc[]
	for (it=0; it<nt; it++) {
		t = ft + it*dt;
		if (t > 3.674) {
			int ter = 1;
		}
		if (t <= tau[0]) {
			vrmsTrc[it] = vrms[0];
			continue;
		} // if t
		if (t > tau[ntrms-1] && it>0) {
			vrmsTrc[it] = vrmsTrc[it-1];
			continue;
		}
		vrmsTrc[it] = -1.f;
		for (itao=0; itao<ntrms-1; itao++) {
			if (t >= tau[itao] && t <= tau[itao+1]) {
				// ..... vrms[itao]...vrmsTrc[it].....vrms[itao+1]....
				//.......tau[itao]....t...............tau[itao+1].....
				delta = tau[itao+1] - tau[itao];
				if (delta < 0.00001f) {
					vrmsTrc[it] = vrms[itao];
				}
				else {
					vrmsTrc[it] = vrms[itao] + (vrms[itao+1] - vrms[itao]) * (t - tau[itao]) / delta;
				}
				break;
			} // if 
		} // for j
		if (vrmsTrc[it] < 0.f) {
			// problem
			int fdfsd =1;
		}
	} // for it

	// now square it
	for (it=0; it<nt; it++) {
		vrmsTrc[it] *= vrmsTrc[it];
	}


	// do a smooth using a 3-point filter
	//vint[0] = vrmsTrc[0];
	//for (it=1; it<nt; it++) {
	//	vint[it] = 0.f;
	//	for (it2=-1; it2<=1;it2++) {
	//		it3 = it+it2;
	//		if (it3>nt-1) it3 = nt-1;
	//		vint[it] += vrmsTrc[it3];
	//	} // for it2
	//	vint[it] = vrmsTrc[it] * 0.333333f;
	//} // for it
	//for (it=0; it<nt; it++) {
	//	vrmsTrc[it] = vint[it];
	//}



	// now obtain the interval velocity square
	vint[0] = vrmsTrc[0];
	for (it=1; it<nt; it++) {
		t = ft + it*dt;
		vint[it] = vrmsTrc[it] * t - vrmsTrc[it-1] * (t-dt);
		vint[it] /= dt;
		if (t>7.8f) {
			int ttty = 1;
		}
		if (vint[it] < 0.f || vint[it] > vint[it-1]*3.f) {
			vint[it] = vint[it-1];
		}
	}

	// showArrayInFile(&vint[0], nt);


	// do a smooth using a 3-point filter
	for (it=0; it<nt; it++) {
		vrmsTrc[it] = 0.f;
		for (it2=-1; it2<=1;it2++) {
			it3 = it+it2;
			if (it3<0) it3=0;
			else if (it3>nt-1) it3 = nt-1;
			vrmsTrc[it] += vint[it3];
		} // for it2
		vrmsTrc[it] = vrmsTrc[it] * 0.333333f;
	} // for it

	for (it=0; it<nt; it++) {
		vint[it] = sqrt(vrmsTrc[it]);
	}


	free(vrmsTrc);



	return true;
}


bool getVelocityRmsMod(int ncont, double depth[], double vTop[], double vBot[], float dz, float dist, double *twttMod, float *VrmsTopMod, float *VrmsBotMod) {
	// note: outputting VrmsMod[icont] at all model boundaries
	if (twttMod==NULL || VrmsTopMod == NULL || VrmsBotMod == NULL || ncont < 2 || dz <= 0.f) return FALSE;

	int nzmaxModelAll = (int)(depth[ncont-1] / dz + 2.0); // only plus 2 makes it complete
	if (nzmaxModelAll < 2) return FALSE;

	float *twttTrc = (float *)malloc(sizeof(float)*nzmaxModelAll);
	float *vintTrc = (float *)malloc(sizeof(float)*nzmaxModelAll);
	float *vrmsTrc = (float *)malloc(sizeof(float)*nzmaxModelAll);
	memset( (void *)&twttTrc[0], 0, sizeof(float)*nzmaxModelAll );
	memset( (void *)&vintTrc[0], 0, sizeof(float)*nzmaxModelAll );
	memset( (void *)&vrmsTrc[0], 0, sizeof(float)*nzmaxModelAll );

	double z;
	int iz;
	int icont;

	getTwttFromVz(ncont, (double *)&depth[0], 
				(double *)&vTop[0], (double *)&vBot[0], 
				nzmaxModelAll, 0.f, dz, 
				twttTrc); 
	twttMod[0] = 0.f;
	for (icont=1; icont<ncont; icont++) {
		twttMod[icont] = -1.f;
		for (iz=1; iz<nzmaxModelAll; iz++) {
			z = (double)(iz*dz);
			if (z < depth[icont]) continue;
			if (z > depth[icont]) {
				twttMod[icont] = (float)(twttTrc[iz-1] + (twttTrc[iz]-twttTrc[iz-1])/dz * (depth[icont] - (iz-1)*dz));
				break;
			}
		} // for iz
		if (twttMod[icont] < 0.f) {
			 // failed to obtain TWTT. this can happen when the model boundary is deeper than pointed by nzmaxModelAll
			// twttMod[icont] = twttMod[icont-1] + (twttTrc[nzmaxModelAll-1] - twttMod[icont-1]) * (depth[icont] - 
			twttMod[icont] = twttMod[icont-1];
		}
	} // for icont

	for (icont=0; icont<ncont-1; icont++) {
		// Note: depth[ncont-1] is the deepest boundary in the model
		float deltazMod = (float)(depth[icont+1] - depth[icont]);
		for (iz=0; iz<nzmaxModelAll; iz++) {
			z = (double)(iz*dz);
			if (z < depth[icont]) continue;
			if (z > depth[icont+1]) break;
			if (deltazMod > 0.00001f)
				vintTrc[iz] = (float)(vTop[icont] + (vBot[icont] - vTop[icont]) * (z-depth[icont]) / deltazMod);
			else
				vintTrc[iz] = (float)vTop[icont];
		} // for iz
	} // for icont
	//for (iz=0; iz<nzmaxModelAll; iz++) {
	//	if (vintTrc[iz] < 1.f) {
	//		int fsddf = 1;
	//	}
	//}

	float *nomi = (float *)malloc(sizeof(float)*nzmaxModelAll);
	float *denom = (float *)malloc(sizeof(float)*nzmaxModelAll);
	nomi[0] = 0.;
	denom[0] = 0.;
	for (iz=1; iz<nzmaxModelAll; iz++) {
		nomi[iz] = nomi[iz-1] + vintTrc[iz-1]*vintTrc[iz-1]*(twttTrc[iz]-twttTrc[iz-1]);
		denom[iz] = denom[iz-1]+ (twttTrc[iz]-twttTrc[iz-1]);
	} // for iz
	vrmsTrc[0] = vintTrc[0];
	for (iz=1; iz<nzmaxModelAll; iz++) {
		vrmsTrc[iz] = sqrt(nomi[iz] / denom[iz]);
	} // for iz
	free(nomi);
	free(denom);
	free(twttTrc);
	free(vintTrc);

	VrmsTopMod[0] = vrmsTrc[0]; // surface
	for (icont=1; icont<ncont-1; icont++) {
		VrmsTopMod[icont] = 0.f;  // initialize
		for (iz=1; iz<nzmaxModelAll; iz++) {
			z = (double)(iz*dz);
			if (z >= depth[icont]) {
				VrmsTopMod[icont] = (float)(vrmsTrc[iz-1] + (vrmsTrc[iz] - vrmsTrc[iz-1])/dz * (depth[icont] - (iz-1)*dz));
				break;
			} // if z
		} // for iz
	} // for icont

	for (icont=0; icont<ncont-1; icont++) {
		VrmsBotMod[icont] = 0.f;  // initialize
		for (iz=nzmaxModelAll-2; iz>=0; iz--) {
			z = (double)(iz*dz);
			if (z < depth[icont+1]) {
				VrmsBotMod[icont] = (float)(vrmsTrc[iz] + (vrmsTrc[iz+1] - vrmsTrc[iz])/dz * (depth[icont+1] - z));
				break;
			} // if z
		} // for iz
	} // for icont

	free(vrmsTrc);
	return true;
}



bool getVelocityRmsTrc(int ncont, double depth[], double vTop[], double vBot[], 
	float dz, int nzmaxModelAll, float *twttOut, float *vrmsOut) {
	if (ncont < 2 || dz <= 0.f || twttOut==NULL || vrmsOut==NULL) return false;
	memset( (void *)&twttOut[0], 0, sizeof(float)*nzmaxModelAll );
	memset( (void *)&vrmsOut[0], 0, sizeof(float)*nzmaxModelAll );

	float *vintTrc = (float *)malloc(sizeof(float)*nzmaxModelAll);
	memset( (void *)&vintTrc[0], 0, sizeof(float)*nzmaxModelAll );

	double z;
	int iz;
	int icont;

	getTwttFromVz(ncont, (double *)&depth[0], 
				(double *)&vTop[0], (double *)&vBot[0], 
				nzmaxModelAll, 0.f, dz, 
				twttOut); 
	for (icont=0; icont<ncont-1; icont++) {
		// Note: depth[ncont-1] is the deepest boundary in the model
		float deltazMod = (float)(depth[icont+1] - depth[icont]);
		for (iz=0; iz<nzmaxModelAll; iz++) {
			z = (double)(iz*dz);
			if (z < depth[icont]) continue;
			if (z > depth[icont+1]) break;
			if (deltazMod > 0.00001f)
				vintTrc[iz] = (float)(vTop[icont] + (vBot[icont] - vTop[icont]) * (z-depth[icont]) / deltazMod);
			else
				vintTrc[iz] = (float)vTop[icont];
		} // for iz
	} // for icont

	//for (iz=0; iz<nzmaxModelAll; iz++) {
	//	vintTrc[iz] = vintTrc[iz]*vintTrc[iz];
	//}

	float *nomi = (float *)malloc(sizeof(float)*nzmaxModelAll);
	float *denom = (float *)malloc(sizeof(float)*nzmaxModelAll);
	nomi[0] = 0.;
	denom[0] = 0.;
	for (iz=1; iz<nzmaxModelAll; iz++) {
		nomi[iz] = nomi[iz-1] + vintTrc[iz-1]*vintTrc[iz-1]*(twttOut[iz]-twttOut[iz-1]);
		denom[iz] = denom[iz-1]+ (twttOut[iz]-twttOut[iz-1]);
	} // for iz
	vrmsOut[0] = vintTrc[0];
	for (iz=1; iz<nzmaxModelAll; iz++) {
		vrmsOut[iz] = sqrt(nomi[iz] / denom[iz]);
	} // for iz
	free(nomi);
	free(denom);
	free(vintTrc);

	return true;
}







float getTwttAtZ(int ncont, double depth[], double vTop[], double vBot[], float z, double dz) {
	// OUTPUT 2-way time at one depth
	int nz = (int)myRound(z / dz) + 1;
	if (nz<1) return 0.f;
	float *twtt = (float *)malloc(nz*sizeof(float));
	getTwttFromVz(ncont, (double *)&depth[0], (double *)&vTop[0], (double *)&vBot[0], nz, 0.f, dz, twtt);
		// note: this function will produce nz values in twtt[0:nz-1]
	float t = twtt[nz-1];
	free(twtt);
	return t;
}

bool getDepthTrcAtTwttIntervals(int ncont, double depth[], double vTop[], double vBot[], 
	int nt, float ft, float dt, float depthTrc[]) {
	// OUTPUT to   depthTrc[]. 
		if (depthTrc==NULL || ncont<2 || nt<2) return false;

		float dz = (float)(vTop[0] * dt * 0.5);
		int nz = (int)myRound(depth[ncont-1] / dz);
		// note: since we do not know the starting depth, we start from 0km
		float *twtt = (float *)malloc(nz*sizeof(float));
		getTwttFromVz(ncont, (double *)&depth[0], (double *)&vTop[0], (double *)&vBot[0], nz, 0.f, dz, twtt);
		// note: this function will produce nz values in twtt[0:nz-1]
	
		float t, z, delta;
		for (int it=0; it<nt; it++) {
			t = ft + (float)it*dt;
			depthTrc[it] = -1.f;
			if (t > twtt[nz-1] && it>0) {
				delta = t - twtt[nz-1];
				depthTrc[it] = (float)(depthTrc[it-1] + dt * 0.5 * vBot[ncont-2]);
			}
			else {
				// now look for the matching t in twtt[0:nz-1]
				for (int iz=0; iz<nz-1; iz++) {
					if (t >= twtt[iz] && t <= twtt[iz+1]) {
						delta = twtt[iz+1] - twtt[iz];
						z = (float)iz*dz;
						depthTrc[it] = (delta > 0.0001f) ?  z + dz * (t - twtt[iz]) / delta  :  z;
						break;
					} // if t
				} // for iz
			}
		} // for it

		free(twtt);
		return true;
}

float getVFromVz(int ncont, double depth[], double vTop[], double vBot[], float z) {


	for (int icont=0; icont<ncont-1; icont++) {
		if (depth[icont] <= depth[icont+1]) {
			if (z>= depth[icont] && z<=depth[icont+1]) {
				float dz = (float)(depth[icont+1] - depth[icont]);
				return (dz > 0.00001f) ?
					(float)(vTop[icont] + (vBot[icont] - vTop[icont]) / dz * (z - depth[icont]))
					:
					(float)(vTop[icont] + vBot[icont]) * 0.5f;
			}
		}
		else {
			if (z<= depth[icont] && z>=depth[icont+1]) {
				return (float)(vTop[icont] + vBot[icont]) * 0.5f;
			}
		}
	} // for icont

	return 2.f;
}




bool loadParam(CString f, CString param, CStringArray *sArr)
{
	// Can be used to load any parameter from a .ini file or a fortran input parameter file
	// in this routine, no global variables are used
	if (sArr == NULL) return false;

	sArr->RemoveAll();

	int i;
	int lenParam = param.GetLength();
	CString cs, csDig;
	FILE *ifh;
	TCHAR line[255];
	//k = tx.nShot = 0;
	if (isFileExist(f)) {
		// now load the contents of this ray parameter file
		if (_tfopen_s(&ifh, f, _T("r")) != 0) return false;
		while (_fgetts(line, 255, ifh) != NULL) {
			cs.Format(_T("%s"), line);
			if (cs.GetLength() <= 0) continue;

			i = cs.Find(param);
			if (i>=0) {
				cs = cs.Mid(i+lenParam);
				cs = cs.Mid(cs.Find(_T('='))+1);
				// cs = cs.Right('=');
				//k = 0;
				for (i=0; i<100; i++) {
					int iWordEnd = cs.FindOneOf( _T(", "));
					if (iWordEnd < 0) break;
					csDig = cs.Left(iWordEnd);
					csDig.Trim();
					if (csDig.GetLength() == 0) {
						cs = cs.Mid(1);
						continue;
					}

					sArr->Add(csDig);

					//k++;
					cs = cs.Mid(iWordEnd+1);
				}
			} // if i
		} // while
		fclose(ifh);
	} // if isFileExist
	else {
		return false;
		// CString a;
		// a.Format(_T("Ray parameter file invalid: %s", draw.rayName);
		// this->myCaption(a);
	}
	return true;
}


//############## Hilbert transform ###################
// note: these arrays are all modified for 0-based
void convol(float* source, double* target, double* filt, int npt, int lfilt)
{
    int i, l;
    double yt;

    for (l=0; l<npt-lfilt+1; l++) {
        yt = 0.0;
		for (i=0; i<lfilt; i++) {
            yt += source[l+i]*filt[lfilt-1-i];
		}
        target[l] = yt;
    }

    /* shifting points */
    for (i=0; i<npt-lfilt; i++) {
        target[i] = 0.5*(target[i]+target[i+1]);
    }
    for (i=npt-lfilt-1; i>=0; i--)
        target[i+lfilt/2]=target[i];

    /* writing zeros */
    for (i=0; i<lfilt/2; i++) {
        target[i] = 0.0;
        target[npt-1-i] = 0.0;
    }
}


// note: these arrays are all 0-based.
//     Before calling this, you need to assign all arrays of   ampl ,   phase,  omega , and later free them
	//if (ampl == NULL) ampl = (double *)malloc( (size_t)( npt*sizeof(double)) );
	//if (phase == NULL) phase = (double *)malloc( (size_t)( npt*sizeof(double)) );
	//if (omega == NULL) omega = (double *)malloc( (size_t)( npt*sizeof(double)) );
bool hilbertTransform(float* x, double* ampl, double* phase, double *omega, int npt, int lfilt = 128)
{
	if (npt<=0 || x == NULL|| ampl == NULL|| phase == NULL|| omega == NULL) return false;

	double* xh = (double *)malloc( (size_t)( npt*sizeof(double)) );
	double* hilb = (double *)malloc( (size_t)( lfilt*sizeof(double)) );
    int i;
	double xt, xht;
	// double M_PI = 3.1415926;
	// static double pi = 3.1415926;
	static double opi = 1./PI;
    static double pi2 = 2*PI;
	static double opi2 = 1./pi2;

	//prepare the pure convoluation filter as (1/pi*t)
    for (i=0; i<lfilt; i++)
        hilb[i]=1./((i-lfilt/2)-0.5)*opi;

	//Initialize 
    for (i=0; i<npt; i++) {
        xh[i] = 0.0;
        ampl[i] = 0.0;
    }
    // npt=i-1;    // =LMAX;  
    if (npt < lfilt) {
        _ftprintf(stderr, _T("%d : insufficient data or decrease the filter window size\n"), npt);
	    return false;
    }

    /* Hilbert transform */
    convol(x, xh, hilb, npt, lfilt);

    /* Ampl and phase */
    for (i=lfilt/2; i<npt-lfilt/2; i++) {
        xt = x[i];
        xht = xh[i];
        ampl[i] = sqrt(xt*xt+xht*xht);
        phase[i] = atan2(xht ,xt);
        if (phase[i] < phase[i-1])
            omega[i] = phase[i]-phase[i-1]+pi2;
        else
            omega[i] = phase[i]-phase[i-1];
    }

	for (i=0; i<npt; i++) {
        omega[i] = omega[i]*opi2;
	}

	// some edge is trunctated
	for (i=0; i<lfilt/2+1; i++) {
        ampl[i] = phase[i] = omega[i] = 0.;
	}

	for (i=npt-lfilt/2; i<npt; i++) {
        ampl[i] = phase[i] = omega[i] = 0.;
	}


	free(xh);
	free(hilb);
	return true;
}


/* prototype of function used internally */
static void convs(int,int,float*,int,int,float*,int,int,float*);

void conv (int lx, int ifx, float *x,
	   int ly, int ify, float *y,
	   int lz, int ifz, float *z)
/*****************************************************************************
Compute z = x convolved with y; i.e.,

           ifx+lx-1
    z[i] =   sum    x[j]*y[i-j]  ;  i = ifz,...,ifz+lz-1
            j=ifx
******************************************************************************
Input:
lx		length of x array
ifx		sample index of first x
x		array[lx] to be convolved with y
ly		length of y array
ify		sample index of first y
y		array[ly] with which x is to be convolved
lz		length of z array
ifz		sample index of first z

Output:
z		array[lz] containing x convolved with y
******************************************************************************
Notes:
The x samples are contained in x[0], x[1], ..., x[lx-1]; likewise for
the y and z samples.  The sample indices of the first x, y, and z values
determine the location of the origin for each array.  For example, if
z is to be a weighted average of the nearest 5 samples of y, one might
use 
	...
	x[0] = x[1] = x[2] = x[3] = x[4] = 1.0/5.0;
	conv(5,-2,x,lx,0,y,ly,0,z);
	...
In this example, the filter x is symmetric, with index of first sample = -2.

This function is optimized for architectures that can simultaneously perform
a multiply, add, and one load from memory; e.g., the IBM RISC System/6000.
Because, for each value of i, it accumulates the convolution sum z[i] in a
scalar, this function is not likely to be optimal for vector architectures.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 11/23/91
*****************************************************************************/
/* prototype of function used internally */


#ifdef SIMPLE_CONV
{
	int ilx=ifx+lx-1,ily=ify+ly-1,ilz=ifz+lz-1,i,j,jlow,jhigh;
	float sum;
	
	x -= ifx;  y -= ify;  z -= ifz;
	for (i=ifz; i<=ilz; ++i) {
		jlow = i-ily;  if (jlow<ifx) jlow = ifx;
		jhigh = i-ify;  if (jhigh>ilx) jhigh = ilx;
		for (j=jlow,sum=0.0; j<=jhigh; ++j)
			sum += x[j]*y[i-j];
		z[i] = sum;
	}
}
#else
{
	int ilx=ifx+lx-1,ily=ify+ly-1,ilz=ifz+lz-1,
		i,j,ilow,ihigh,jlow,jhigh;
	float sa,sb,xa,xb,ya,yb,*t;

	/* if x is longer than y, swap x and y */
	if (lx>ly) {
		i = ifx;  ifx = ify;  ify = i;
		i = ilx;  ilx = ily;  ily = i;
		i = lx;  lx = ly;  ly = i;
		t = x;  x = y;  y = t;
	}
	
	/* handle short x with special code */
	if (lx>=1 && lx<=30) {
		convs(lx,ifx,x,ly,ify,y,lz,ifz,z);
		return;
	}
	
	/* adjust pointers for indices of first samples */
	x -= ifx;
	y -= ify;
	z -= ifz;
		
	/* OFF LEFT:  i < ify+ifx */
	
	/* zero output for all i */
	ilow = ifz;
	ihigh = ify+ifx-1;  if (ihigh>ilz) ihigh = ilz;
	for (i=ilow; i<=ihigh; ++i)
		z[i] = 0.0;

	/* ROLLING ON:  ify+ifx <= i < ify+ilx */
	
	/* if necessary, do one i so that number of j in overlap is odd */
	if (i<ify+ilx && i<=ilz) {
		jlow = ifx;
		jhigh = i-ify;
		if ((jhigh-jlow)%2) {
			sa = 0.0;
			for (j=jlow; j<=jhigh; ++j)
				sa += x[j]*y[i-j];
			z[i++] = sa;
		}
	}
	
	/* loop over pairs of i and j */
	ilow = i;
	ihigh = ilx+ify-1;  if (ihigh>ilz) ihigh = ilz;
	jlow = ifx;
	jhigh = ilow-ify;
	for (i=ilow; i<ihigh; i+=2,jhigh+=2) {
		sa = sb = 0.0;
		xb = x[jhigh+1];
		yb = 0.0;
		for (j=jhigh; j>=jlow; j-=2) {
			sa += xb*yb;
			ya = y[i-j];
			sb += xb*ya;
			xa = x[j];
			sa += xa*ya;
			yb = y[i+1-j];
			sb += xa*yb;
			xb = x[j-1];
		}
		z[i] = sa;
		z[i+1] = sb;
	}
	
	/* if number of i is odd */
	if (i==ihigh) {
		jlow = ifx;
		jhigh = i-ify;
		sa = 0.0;
		for (j=jlow; j<=jhigh; ++j)
			sa += x[j]*y[i-j];
		z[i++] = sa;
	}
	
	/* MIDDLE:  ify+ilx <= i <= ily+ifx */
	
	/* determine limits for i and j */
	ilow = i;
	ihigh = ily+ifx;  if (ihigh>ilz) ihigh = ilz;
	jlow = ifx;
	jhigh = ilx;
	
	/* if number of j is even, do j in pairs with no leftover */
	if ((jhigh-jlow)%2) {
		for (i=ilow; i<ihigh; i+=2) {
			sa = sb = 0.0;
			yb = y[i+1-jlow];
			xa = x[jlow];
			for (j=jlow; j<jhigh; j+=2) {
				sb += xa*yb;
				ya = y[i-j];
				sa += xa*ya;
				xb = x[j+1];
				sb += xb*ya;
				yb = y[i-1-j];
				sa += xb*yb;
				xa = x[j+2];
			}
			z[i] = sa;
			z[i+1] = sb;
		}
	
	/* else, number of j is odd, so do j in pairs with leftover */
	} else {
		for (i=ilow; i<ihigh; i+=2) {
			sa = sb = 0.0;
			yb = y[i+1-jlow];
			xa = x[jlow];
			for (j=jlow; j<jhigh; j+=2) {
				sb += xa*yb;
				ya = y[i-j];
				sa += xa*ya;
				xb = x[j+1];
				sb += xb*ya;
				yb = y[i-1-j];
				sa += xb*yb;
				xa = x[j+2];
			}
			z[i] = sa+x[jhigh]*y[i-jhigh];
			z[i+1] = sb+x[jhigh]*y[i+1-jhigh];
		}
	}
	
	/* if number of i is odd */
	if (i==ihigh) {
		sa = 0.0;
		for (j=jlow; j<=jhigh; ++j)
			sa += x[j]*y[i-j];
		z[i++] = sa;
	}

	/* ROLLING OFF:  ily+ifx < i <= ily+ilx */
	
	/* if necessary, do one i so that number of j in overlap is even */
	if (i<=ily+ilx && i<=ilz) {
		jlow = i-ily;
		jhigh = ilx;
		if (!((jhigh-jlow)%2)) {
			sa = 0.0;
			for (j=jlow; j<=jhigh; ++j)
				sa += x[j]*y[i-j];
			z[i++] = sa;
		}
	}
	
	/* number of j is now even, so loop over both i and j in pairs */
	ilow = i;
	ihigh = ily+ilx;  if (ihigh>ilz) ihigh = ilz;
	jlow = ilow-ily;
	jhigh = ilx-2; /* Dave's new patch */
        for (i=ilow; i<ihigh; i+=2,jlow+=2) {
                sa = sb = 0.0;
                xa = x[jlow];
                yb = 0.0;
                for (j=jlow; j<jhigh; j+=2) {
                        sb += xa*yb;
                        ya = y[i-j];
                        sa += xa*ya;
                        xb = x[j+1];
                        sb += xb*ya;
                        yb = y[i-1-j];
                        sa += xb*yb;
                        xa = x[j+2];
                }
                sb += xa*yb;
                ya = y[i-j];
                sa += xa*ya;
                xb = x[j+1];
                sb += xb*ya;
                yb = y[i-1-j];
                sa += xb*yb;
                z[i] = sa;
                z[i+1] = sb;
        }
	
	/* if number of i is odd */
	if (i==ihigh) {
		jlow = i-ily;
		jhigh = ilx;
		sa = 0.0;
		for (j=jlow; j<=jhigh; ++j)
			sa += x[j]*y[i-j];
		z[i++] = sa;
	}
	
	/* OFF RIGHT:  ily+ilx < i */
	
	/* zero output for all i */
	ilow = i;
	ihigh = ilz;
	for (i=ilow; i<=ihigh; ++i)
		z[i] = 0.0;
}

/* internal function optimized for short x */
void convs (int lx, int ifx, float *x,
	   int ly, int ify, float *y, 
	   int lz, int ifz, float *z)
{
	int ilx=ifx+lx-1,ily=ify+ly-1,ilz=ifz+lz-1,
		i,j,ilow,ihigh,jlow,jhigh;
	float x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,
		x10,x11,x12,x13,x14,x15,x16,x17,x18,x19,
		x20,x21,x22,x23,x24,x25,x26,x27,x28,x29,
		ya,yb,z0,z1,sum;
	
	x -= ifx;
	y -= ify;
	z -= ifz;
	
		
	/* OFF LEFT:  i < ifx+ify */
	ilow = ifz;
	ihigh = ify+ifx-1;  if (ihigh>ilz) ihigh = ilz;
	for (i=ilow; i<=ihigh; ++i)
		z[i] = 0.0;
	
	/* ROLLING ON:  ify+ifx <= i < ify+ilx */
	ilow = ify+ifx;  if (ilow<ifz) ilow = ifz;
	ihigh = ify+ilx-1;  if (ihigh>ilz) ihigh = ilz;
	jlow = ifx;
	jhigh = ilow-ify;
	for (i=ilow; i<=ihigh; ++i,++jhigh) {
		for (j=jlow,sum=0.0; j<=jhigh; ++j)
			sum += x[j]*y[i-j];
		z[i] = sum;
	}
	
	/* MIDDLE:  ify+ilx <= i <= ily+ifx */
	ilow = ify+ilx;  if (ilow<ifz) ilow = ifz;
	ihigh = ily+ifx;  if (ihigh>ilz) ihigh = ilz;
	if (lx==1) {
		x0 = x[ifx];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==2) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==3) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==4) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==5) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==6) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==7) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==8) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==9) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==10) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==11) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==12) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==13) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==14) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==15) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==16) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==17) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==18) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		x17 = x[ifx+17];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i-ifx-17];  z0 += x17*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==19) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		x17 = x[ifx+17];
		x18 = x[ifx+18];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i-ifx-17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i-ifx-18];  z0 += x18*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==20) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		x17 = x[ifx+17];
		x18 = x[ifx+18];
		x19 = x[ifx+19];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i-ifx-17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i-ifx-18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i-ifx-19];  z0 += x19*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==21) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		x17 = x[ifx+17];
		x18 = x[ifx+18];
		x19 = x[ifx+19];
		x20 = x[ifx+20];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i-ifx-17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i-ifx-18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i-ifx-19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i-ifx-20];  z0 += x20*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==22) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		x17 = x[ifx+17];
		x18 = x[ifx+18];
		x19 = x[ifx+19];
		x20 = x[ifx+20];
		x21 = x[ifx+21];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i-ifx-17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i-ifx-18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i-ifx-19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i-ifx-20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i-ifx-21];  z0 += x21*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==23) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		x17 = x[ifx+17];
		x18 = x[ifx+18];
		x19 = x[ifx+19];
		x20 = x[ifx+20];
		x21 = x[ifx+21];
		x22 = x[ifx+22];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i-ifx-17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i-ifx-18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i-ifx-19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i-ifx-20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i-ifx-21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i-ifx-22];  z0 += x22*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==24) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		x17 = x[ifx+17];
		x18 = x[ifx+18];
		x19 = x[ifx+19];
		x20 = x[ifx+20];
		x21 = x[ifx+21];
		x22 = x[ifx+22];
		x23 = x[ifx+23];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i-ifx-17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i-ifx-18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i-ifx-19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i-ifx-20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i-ifx-21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i-ifx-22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i-ifx-23];  z0 += x23*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==25) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		x17 = x[ifx+17];
		x18 = x[ifx+18];
		x19 = x[ifx+19];
		x20 = x[ifx+20];
		x21 = x[ifx+21];
		x22 = x[ifx+22];
		x23 = x[ifx+23];
		x24 = x[ifx+24];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i-ifx-17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i-ifx-18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i-ifx-19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i-ifx-20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i-ifx-21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i-ifx-22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i-ifx-23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i-ifx-24];  z0 += x24*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==26) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		x17 = x[ifx+17];
		x18 = x[ifx+18];
		x19 = x[ifx+19];
		x20 = x[ifx+20];
		x21 = x[ifx+21];
		x22 = x[ifx+22];
		x23 = x[ifx+23];
		x24 = x[ifx+24];
		x25 = x[ifx+25];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i-ifx-17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i-ifx-18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i-ifx-19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i-ifx-20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i-ifx-21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i-ifx-22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i-ifx-23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i-ifx-24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i-ifx-25];  z0 += x25*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==27) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		x17 = x[ifx+17];
		x18 = x[ifx+18];
		x19 = x[ifx+19];
		x20 = x[ifx+20];
		x21 = x[ifx+21];
		x22 = x[ifx+22];
		x23 = x[ifx+23];
		x24 = x[ifx+24];
		x25 = x[ifx+25];
		x26 = x[ifx+26];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i-ifx-17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i-ifx-18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i-ifx-19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i-ifx-20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i-ifx-21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i-ifx-22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i-ifx-23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i-ifx-24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i-ifx-25];  z0 += x25*ya;  z1 += x26*ya;
			yb = y[i-ifx-26];  z0 += x26*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==28) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		x17 = x[ifx+17];
		x18 = x[ifx+18];
		x19 = x[ifx+19];
		x20 = x[ifx+20];
		x21 = x[ifx+21];
		x22 = x[ifx+22];
		x23 = x[ifx+23];
		x24 = x[ifx+24];
		x25 = x[ifx+25];
		x26 = x[ifx+26];
		x27 = x[ifx+27];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i-ifx-17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i-ifx-18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i-ifx-19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i-ifx-20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i-ifx-21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i-ifx-22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i-ifx-23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i-ifx-24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i-ifx-25];  z0 += x25*ya;  z1 += x26*ya;
			yb = y[i-ifx-26];  z0 += x26*yb;  z1 += x27*yb;
			ya = y[i-ifx-27];  z0 += x27*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==29) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		x17 = x[ifx+17];
		x18 = x[ifx+18];
		x19 = x[ifx+19];
		x20 = x[ifx+20];
		x21 = x[ifx+21];
		x22 = x[ifx+22];
		x23 = x[ifx+23];
		x24 = x[ifx+24];
		x25 = x[ifx+25];
		x26 = x[ifx+26];
		x27 = x[ifx+27];
		x28 = x[ifx+28];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i-ifx-17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i-ifx-18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i-ifx-19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i-ifx-20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i-ifx-21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i-ifx-22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i-ifx-23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i-ifx-24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i-ifx-25];  z0 += x25*ya;  z1 += x26*ya;
			yb = y[i-ifx-26];  z0 += x26*yb;  z1 += x27*yb;
			ya = y[i-ifx-27];  z0 += x27*ya;  z1 += x28*ya;
			yb = y[i-ifx-28];  z0 += x28*yb;
			z[i+1] = z1;
			z[i] = z0;
		}
	} else if (lx==30) {
		x0 = x[ifx];
		x1 = x[ifx+1];
		x2 = x[ifx+2];
		x3 = x[ifx+3];
		x4 = x[ifx+4];
		x5 = x[ifx+5];
		x6 = x[ifx+6];
		x7 = x[ifx+7];
		x8 = x[ifx+8];
		x9 = x[ifx+9];
		x10 = x[ifx+10];
		x11 = x[ifx+11];
		x12 = x[ifx+12];
		x13 = x[ifx+13];
		x14 = x[ifx+14];
		x15 = x[ifx+15];
		x16 = x[ifx+16];
		x17 = x[ifx+17];
		x18 = x[ifx+18];
		x19 = x[ifx+19];
		x20 = x[ifx+20];
		x21 = x[ifx+21];
		x22 = x[ifx+22];
		x23 = x[ifx+23];
		x24 = x[ifx+24];
		x25 = x[ifx+25];
		x26 = x[ifx+26];
		x27 = x[ifx+27];
		x28 = x[ifx+28];
		x29 = x[ifx+29];
		for (i=ilow; i<=ihigh-1; i+=2) {
			ya = y[i+1-ifx];  z1 = x0*ya;
			yb = y[i-ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i-ifx-1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i-ifx-2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i-ifx-3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i-ifx-4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i-ifx-5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i-ifx-6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i-ifx-7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i-ifx-8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i-ifx-9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i-ifx-10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i-ifx-11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i-ifx-12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i-ifx-13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i-ifx-14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i-ifx-15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i-ifx-16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i-ifx-17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i-ifx-18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i-ifx-19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i-ifx-20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i-ifx-21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i-ifx-22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i-ifx-23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i-ifx-24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i-ifx-25];  z0 += x25*ya;  z1 += x26*ya;
			yb = y[i-ifx-26];  z0 += x26*yb;  z1 += x27*yb;
			ya = y[i-ifx-27];  z0 += x27*ya;  z1 += x28*ya;
			yb = y[i-ifx-28];  z0 += x28*yb;  z1 += x29*yb;
			ya = y[i-ifx-29];  z0 += x29*ya;
			z[i+1] = z1;
			z[i] = z0;
		}
	}
	if (ihigh>=ilow && (ihigh-ilow)%2==0) {
		ilow = ihigh;
		jlow = ifx;
		jhigh = ilx;
		for (i=ilow; i<=ihigh; ++i) {
			for (j=jlow,sum=0.0; j<=jhigh; ++j)
				sum += x[j]*y[i-j];
			z[i] = sum;
		}
	}
	
	/* ROLLING OFF:  ily+ifx < i <= ily+ilx */
	ilow = ily+ifx+1;  if (ilow<ifz) ilow = ifz;
	ihigh = ily+ilx;  if (ihigh>ilz) ihigh = ilz;
	jlow = ilow-ily;
	jhigh = ilx;
	for (i=ilow; i<=ihigh; ++i,++jlow) {
		for (j=jlow,sum=0.0; j<=jhigh; ++j)
			sum += x[j]*y[i-j];
		z[i] = sum;
	}
	
	/* OFF RIGHT:  ily+ilx < i */
	ilow = ily+ilx+1;  if (ilow<ifz) ilow = ifz;
	ihigh = ilz;
	for (i=ilow; i<=ihigh; ++i)
		z[i] = 0.0;
}
#endif

void xcor (int lx, int ifx, float *x,
	   int ly, int ify, float *y, 
	   int lz, int ifz, float *z)
/*****************************************************************************
Compute z = x cross-correlated with y; i.e.,

           ifx+lx-1
    z[i] =   sum    x[j]*y[i+j]  ;  i = ifz,...,ifz+lz-1
            j=ifx
******************************************************************************
Input:
lx		length of x array
ifx		sample index of first x
x		array[lx] to be cross-correlated with y
ly		length of y array
ify		sample index of first y
y		array[ly] with which x is to be cross-correlated
lz		length of z array
ifz		sample index of first z

Output:
z		array[lz] containing x cross-correlated with y
******************************************************************************
Notes:
See notes for convolution function conv().
This function performs cross-correlation by
(1) reversing the samples in the x array while copying
    them to a temporary array, and
(2) calling function conv() with ifx set to 1-ifx-lx.
Assuming that the overhead of reversing the samples in x is negligible,
this method enables cross-correlation to be performed as efficiently as
convolution, while reducing the amount of code that must be optimized
and maintained.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 11/23/91
*****************************************************************************/
{
	int i,j;
	float *xr;
	
	xr = alloc1float(lx);
	for (i=0,j=lx-1; i<lx; ++i,--j)
		xr[i] = x[j];
	conv(lx,1-ifx-lx,xr,ly,ify,y,lz,ifz,z); 
	free1float(xr);
}





/*  
to find all files under C:\ , you have to use strFolderPath = _T("c:\*.*"
targetFileName can be full name with or without path
*/
void SearchAllFiles(CString& strFolderPath, CString& targetFileName, CString& outFileName)
{
	if (!outFileName.IsEmpty()) return; // if found, stop all searching, remember: this is a recursive search!

	CString baseFileFull;
	baseFileFull.Format(_T("\\%s.%s"), GetBaseFileName(targetFileName), GetFileExtension(targetFileName));

	// Create a CFileFind object
	CFileFind oFileFinder;

	// Initialise default values
	BOOL bFinish = FALSE;

	// Start search for all files in folder
	oFileFinder.FindFile((LPCTSTR)strFolderPath);

	// Loop until no file is found
	do
	{
		// Find next file in folder
		bFinish = oFileFinder.FindNextFile();

		// Check if it is not a . or .. directory
		if (oFileFinder.IsDots() != TRUE)
		{
			// Check if it is another folder
			if (oFileFinder.IsDirectory() == TRUE)
			{
				// Get path of new folder
				CString strNewFolderPath = oFileFinder.GetFilePath();

				// Append characters to string
				strNewFolderPath += _T("\\*.*");

				// Recurse function
				SearchAllFiles(strNewFolderPath, targetFileName, outFileName);
			}

			// It is a file
			else
			{
				// Store path and name of file  oFileFinder.GetFilePath()
				CString t = oFileFinder.GetFilePath();
				if (t.Find(baseFileFull) > 0) {
					outFileName = t;
					return;
				}
			}
		}
	}
	while(bFinish == TRUE);

	// End of search
	oFileFinder.Close();

	// now not find. So we just return the same
	return;
}



/*
Calling it is really easy - just create a CStringArray, and pass it in something like this:

CStringArray cStringArray;
int nElementCount = SplitString("22|37|egg|234|17", _T("|", cStringArray);

In this example, nElementCount will be 5, using a delimiter of "|", and the cStringArray will have 5 elements:
22
37
egg
234
17
*/
int SplitString(const CString& strInputString, const CString& strDelimiter, CStringArray& arrStringArray)
{
	int iPos = 0;
	int newPos = -1;
	int sizeS2 = strDelimiter.GetLength();
	int isize = strInputString.GetLength();

	CArray<INT, int> positions;

	newPos = strInputString.Find (strDelimiter, 0);

	if( newPos < 0 )
		return 0;

	int numFound = 0;

	while( newPos > iPos )
	{
		numFound++;
		positions.Add(newPos);
		iPos = newPos;
		newPos = strInputString.Find (strDelimiter, iPos + sizeS2);
	}

	for( int i=0; i <= positions.GetSize(); i++ )
	{
		CString s;
		if( i == 0 )
			s = strInputString.Mid( i, positions[i] );
		else
		{
			int offset = positions[i-1] + sizeS2;
			if( offset < isize )
			{
				if( i == positions.GetSize() )
					s = strInputString.Mid(offset);
				else if( i > 0 )
					s = strInputString.Mid( positions[i-1] + sizeS2, positions[i] - positions[i-1] - sizeS2 );
			}
		}
		arrStringArray.Add(s);
	}
	return numFound+1;
}




	BOOL IsNatural(const CString Str)
	{
		// Check each character of the string
		// If a character at a certain position is not a digit,
		// then the string is not a valid natural number
		for(int i = 0; i < Str.GetLength(); i++)
		{
			if (Str[i] == '-') continue;  // a negative sign is normal 
			if( Str[i] < '0' || Str[i] > '9' )
				return FALSE;
		}

		return TRUE;
	}

	BOOL IsNumeric(const CString Str)
	{
		// Make a copy of the original string to test it
		CString WithoutSeparator = Str;
		// Prepare to test for a natural number
		// First remove the decimal separator, if any
		WithoutSeparator.Replace(_T("."), _T(""));

		// If this number were natural, test it
		// If it is not even a natural number, then it can't be valid
		if( IsNatural(WithoutSeparator) == FALSE )
			return FALSE; // Return Invalid Number

		// Set a counter to 0 to counter the number of decimal separators
		int NumberOfSeparators = 0;

		// Check each charcter in the original string
		for(int i = 0; i < Str.GetLength(); i++)
		{
			// If you find a decimal separator, count it
			if( Str[i] == _T('.') )
				NumberOfSeparators++;
		}

		// After checking the string and counting the decimal separators
		// If there is more than one decimal separator,
		// then this cannot be a valid number
		if( NumberOfSeparators > 1 )
			return FALSE; // Return Invalid Number
		else // Otherwise, this appears to be a valid decimal number
			return TRUE;
	}

	int StringToInt(const CString Str)
	{
		// First check to see if this is a valid natural number
		BOOL IsValid = IsNatural(Str);

		// If this number is valid, then convert it
		if( IsValid == TRUE )
			return _tstoi(Str);
		else
		{
			// Return 0 to be nice
			return 0;
		}
	}

	double StringToFloat(const CString Str)
	{
		// First check to see if this is a valid number
		BOOL IsValid = IsNumeric(Str);

		// If this number is valid, then convert it
		if( IsValid == TRUE )
			return _tstof(Str);
		else
		{
			// Return 0 to be nice
			return 0;
		}
	}


	// this function returns the Fz gravity contribution from one side of a polygon cross-section, without multiplification of G*Rou
//c     sigma = density contrast (grams/cc) -- format f7.4
//c     n     = # of coordinates in polygon -- format i5
//c     r(i)  = x-axis coordinate (km) ------- format f9.3  note: it was f8.3....deping
//c     z(i)  = z-axis coordinate (km) ------- format f8.3  note: it was f8.3....deping
//C  F = G m1 m2 / r^2
//C     G = 6.67384*10^(-11)
	// NOT USED
float getGravFromOneSideOfPolygon(float xi, float zi, float xj, float zj, float Y1, float Y2)
{
	// the curve formula is:  z = m*x + n
	if (fabs(xj - xi) < MINAMP) return 0.f;

	double m = (zj - zi) / (xj - xi);
	double theta = atan(m);
	double c = sqrt(1 + m*m);
	double n = zi - m*xi;
	double costheta = cos(theta);
	double a = n * costheta;
	double a2 = a*a;
	double Y12 = Y1*Y1;
	double Y22 = Y2*Y2;

	// (x0, z0) is the reference coord
	double x0 = a * sin(theta);
	double z0 = a * costheta;

	double S = xi + x0;
	double T = xj + x0;
	double ri2 = xi*xi + zi*zi;
	double rj2 = xj*xj + zj*zj;

	double R1i = sqrt(ri2+Y12);
	double R1j = sqrt(rj2+Y12);
	double R2i = sqrt(ri2+Y22);
	double R2j = sqrt(rj2+Y22);

	double K = a / c;

	double result = T*log(rj2)-S*log(ri2)
		+ 2*K*atan(T/K) - 2*K*atan(S/K)
		+ S*log((Y1 + R1i)*(Y2+R2i))
		- T*log((Y1 + R1j)*(Y2+R2j))
		+ Y1/c*log((c*S+R1i) / (c*T+R1j))
		+ Y2/c*log((c*S+R2i) / (c*T+R2j))
		+ K*atan((a2+Y12+Y1*R1j) / (z0*T))
		+ K*atan((a2+Y22+Y2*R2j) / (z0*S))
		- K*atan((a2+Y12+Y1*R1i) / (z0*S))
		- K*atan((a2+Y22+Y2*R2j) / (z0*S));


	return 0.f;
}


/*
void floodFillArr5(CDC* pDC, int nx, int ny, float **xArr, float *rangeArr, 
				   float xmin, float xmax, float ymin, float ymax,
				   int ixmin, int ixmax, int iymin, int iymax, int memMaxMB, BOOL isFaceUp)
{
	if (ny <2) return;
	CString tStr;

	int nrowOrig, ncolOrig;
	int nrow = nrowOrig = abs(iymax - iymin) + 1;  // 11450
	int ncol = ncolOrig = abs(ixmax - ixmin) + 1;  // 9000

	// one can either load an existing bitmap file or create an empty bitmap object
	CDib dib;

	// note: after this call, nrow or ncol can change
	float reduceRate = dib.Create32Ex(&ncol, &nrow, !isFaceUp, memMaxMB);
	if (reduceRate == 0.f) {
		myCaption(_T("Could not initial bitmap "));
		return;
	}

	dib.SetPalette(pDC );

	//for(int r=0; r<19500; r++) {
	//	int pos = 3*r;
	//	m_Dib.SetDibBit(&pos, 0, 255, 0);
	//} 

	int ix, iy, irow, icol;
	int it, ic;
	float x;
	float y;

	if (ampColorDyn.numContours > 100) ampColorDyn.numContours = 100;

	// int **iAmpArr2d = (int **)alloc2int(nt, nx);
	COLORREF *colArr = (COLORREF *)malloc(ampColorDyn.numContours * sizeof(COLORREF));
	float *percentAmpExpanded = (float *)malloc(ampColorDyn.numContours * sizeof(float));
	if (!getExpandedColorTable(colArr, percentAmpExpanded, ampColorDyn.numContours)) {
		ampColorDyn.numContours = ampColorDyn.num; // if cannot be expanded, just use the original number of colors
	} // if
	

	float ampOld = -MIL;
	for (it=0; it<nt; it++) {
		for (ix=0; ix<nx; ix++) {
		   if (ix>0 && ampOld == xArr[ix][it]) {
				xArr[ix][it] = xArr[ix-1][it];
		   }
		   else {
				ampOld = xArr[ix][it];
				xArr[ix][it] = (float)getColorIndex4Amp(&xArr[ix][it], percentAmpExpanded);
		   }
	   } // for ix
	} // for it


	// note:  draw.trcInt  did not include trace skipping. So we have to re-calculate the allowable trace span width during display
	float *dx = (float *)ealloc1float(nx);
	for (ix=1; ix<nx; ix++) {
		dx[ix-1] = (float)fabs(rangeArr[ix]-rangeArr[ix-1]);
	}
	double traceSpanMax = 3.0 * getMedianValue(dx, nx-1);	//This is the average trace interval in km.
	free1float(dx);


	double deltax = xmax-xmin;
	double xRate = deltax*(double)ncolOrig / (double)ncol /(double)((ncol-1) * reduceRate);

	double deltay = ymax-ymin;
	double dy = deltay / (double)(ny-1);
	double yRate = deltay * (double)nrowOrig / (double)nrow /(double)((nrow-1) * reduceRate);

	int ix0 = 0;
	for (icol=0; icol<ncol; icol++) {
		x = (float)(xmin + icol * xRate);
		ix0 = 0;
		ix = getClosestIndex(rangeArr, &nx, &x, &ix0);
		if (ix<0 || fabs(rangeArr[ix] - x) > traceSpanMax) {
			for (irow=0; irow<nrow; irow++) {
				dib.SetDibBit32(&icol, &irow, 255, 255, 255);
			}
		}
		else {
			iy = 0;
			for (irow=0; irow<nrow; irow++) {
				y = (float)(ymin + irow * yRate);
				iy = getClosestIndex(&ymin, &dy, &ny, &y, &iy);
				ic = (int)xArr[ix][iy];
				// just to be safe
				if (ic <0) ic = 0;
				else if (ic > ampColorDyn.numContours-1)
					ic = ampColorDyn.numContours -1;
				dib.SetDibBit32(&icol, &irow, (BYTE)GetRValue(colArr[ic]), (BYTE)GetGValue(colArr[ic]), (BYTE)GetBValue(colArr[ic]));
			} // for irow
		}
	} // for icol

	//pDC->SetStretchBltMode(HALFTONE);

	// NOTE: ymin (in sec) corresponds to the first data point in y. But coord.logicalAxes.top in client coordinate is the min Y-axis. They may be different!
	if (! dib.Draw( pDC, 
		coord.logicalAxes.left, 
		coord.logicalAxes.top, 
		(int)myRound(deltax * coord.mm_xscale), 
		(int)myRound(deltay * coord.mm_yscale), !isFaceUp )
		) {
			AfxMessageBox(_T("Could not draw bitmap."));
			return;
	}

	free(colArr);
	free(percentAmpExpanded);
}
*/






// if (px, py) is on the line or the line is not defined well, we return -1
// if above, we return 1
// if below, we return 0
int IsPointAbove2(float x1, float y1, float x2, float y2, float px, float py)
{
	// note: x2>=x1 assumed, and if px is not in between x1 and x2, we force it to be false!  // disabled this
	// if (px<x1 || px>x2) return false;

    float dx = x2 - x1;
	if (fabs(dx) == 0.f) return -1;
	float dy = y2 - y1;
	float m1 = dy / dx;
    // y = mx + c
    // intercept c = y - mx
    float c1 = y1 - m1 * x1; // which is same as y2 - slope * x2

	if ((m1 * px + c1) == py) return -1;
    return (m1 * px + c1) >= py;
}

bool IsPointInBoundingBox(float x1, float y1, float x2, float y2, float px, float py)
{
    float left, top, right, bottom; // Bounding Box For Line Segment
    // For Bounding Box
    if(x1 < x2) {
        left = x1;
        right = x2;
    } else {
        left = x2;
        right = x1;
    }
    if(y1 < y2) {
        top = y1;
        bottom = y2;
    } else {
        top = y2;
        bottom = y1;
    }
 
    return (px+0.00001) >= left && 
		(px-0.00001) <= right && 
        (py+0.00001) >= top && 
		(py-0.00001) <= bottom;
}
 
bool LineIntersection(float l1x1, float l1y1, float l1x2, float l1y2,
                            float l2x1, float l2y1, float l2x2, float l2y2,
                            float *m1, float *c1, float *m2, float *c2,
                            float* intersection_X, float* intersection_Y)
{
    float dx, dy;
 
    dx = l1x2 - l1x1;
	if (fabs(dx) == 0.f) return false;
    dy = l1y2 - l1y1;
 
    *m1 = dy / dx;
    // y = mx + c
    // intercept c = y - mx
    *c1 = l1y1 - *m1 * l1x1; // which is same as y2 - slope * x2
 
    dx = l2x2 - l2x1;
	if (fabs(dx) == 0.f) return false;
    dy = l2y2 - l2y1;
 
    *m2 = dy / dx;

	// y = mx + c
    // intercept c = y - mx
    *c2 = l2y1 - *m2 * l2x1; // which is same as y2 - slope * x2
 
    if( (*m1 - *m2) == 0)
        return false;

    *intersection_X = (*c2 - *c1) / (*m1 - *m2);
    *intersection_Y = *m1 * *intersection_X + *c1;
	return true;
}
 

/*
nRet = LineSegmentIntersection(l1x1, l1y1, l1x2, l1y2,
                            l2x1, l2y1, l2x2, l2y2,
                            &m1, &c1, &m2, &c2, &intersection_X, &intersection_Y);
printf("Equation of line1: Y = %.2fX %c %.2f\n", m1, (c1 < 0) ? ' ' : '+',  c1);
printf("Equation of line2: Y = %.2fX %c %.2f\n", m2, (c2 < 0) ? ' ' : '+',  c2);
 
if(nRet == 0)
    printf("The two line segments do not intersect each other");
else
    printf("The two line segments intersect each other at %.2f, %.2f", intersection_X, intersection_Y);

Program to find the intersection point of two line segments:
Enter Line1 - X1: 1
Enter Line1 - Y1: 2
Enter Line1 - X2: 5
Enter Line1 - Y2: 7
Enter Line2 - X1: 3
Enter Line2 - Y1: 3
Enter Line2 - X2: 2.33
Enter Line2 - Y2: 3.66
Equation of line1: Y = 1.25X + 0.75
Equation of line2: Y = -0.99X + 5.96
The two line segments do not intersect each other
*/
bool LineSegmentIntersection(float l1x1, float l1y1, float l1x2, float l1y2,
                            float l2x1, float l2y1, float l2x2, float l2y2,
                            float *m1, float *c1, float *m2, float *c2,
                            float* intersection_X, float* intersection_Y)
{
    float dx, dy;
 
    dx = l1x2 - l1x1;
	if (fabs(dx) == 0.f) return false;
    dy = l1y2 - l1y1;
 
    *m1 = dy / dx;
    // y = mx + c
    // intercept c = y - mx
    *c1 = l1y1 - *m1 * l1x1; // which is same as y2 - slope * x2
 
    dx = l2x2 - l2x1;
	if (fabs(dx) == 0.f) return false;
    dy = l2y2 - l2y1;
 
    *m2 = dy / dx;
    // y = mx + c
    // intercept c = y - mx
    *c2 = l2y1 - *m2 * l2x1; // which is same as y2 - slope * x2
 
    if( (*m1 - *m2) == 0)
        return false;

	*intersection_X = (*c2 - *c1) / (*m1 - *m2);
    *intersection_Y = *m1 * *intersection_X + *c1;
    return IsPointInBoundingBox(l1x1, l1y1, l1x2, l1y2, *intersection_X, *intersection_Y) &&
			IsPointInBoundingBox(l2x1, l2y1, l2x2, l2y2, *intersection_X, *intersection_Y);
}
 





//######################################
//######################################
//######################################
//######################################
double FindShortestDistanceToSegment(double x1, double y1, double x2, double y2, double pointX, double pointY)
{
	// http://www.cprogramto.com/c-program-to-find-shortest-distance-between-point-and-line-segment/
    double diffX = x2 - x1;
    double diffY = y2 - y1;
    if ((diffX == 0.) && (diffY == 0.)) {
        diffX = pointX - x1;
        diffY = pointY - y1;
        return (diffX * diffX + diffY * diffY);
    }

    double t = ((pointX - x1) * diffX + (pointY - y1) * diffY) / (diffX * diffX + diffY * diffY);

    if (t < 0) {
        //point is nearest to the first point i.e x1 and y1
        diffX = pointX - x1;
        diffY = pointY - y1;
    } else if (t > 1.) {
        //point is nearest to the end point i.e x2 and y2
        diffX = pointX - x2;
        diffY = pointY - y2;
    } else {
        //if perpendicular line intersect the line segment.
        diffX = pointX - (x1 + t * diffX);
        diffY = pointY - (y1 + t * diffY);
    }

    //returning shortest distance; NOTE: sqrt() operation is removed for faster processing
    return (diffX * diffX + diffY * diffY);
}

    //int main()
    //{
    //    double distance;
    //    double lineX1, lineY1, lineX2, lineY2, pointX, pointY;
    //    lineX1 = 20;            //X1, Y1 are the first point of that line segment.
    //    lineY1 = 20;

    //    lineX2 = 40;            //X2, Y2 are the end point of that line segment
    //    lineY2 = 30;

    //    pointX = 37;            //pointX, pointY are the point of the reference point.
    //    pointY = 14;

    //    distance = FindDistanceToSegment(lineX1, lineY1, lineX2, lineY2, pointX, pointY);       //calling function to find the shortest distance
    //    cout<<"Distance = "<<distance;
    //    getch();
    //}


//
//
//
//int myClipPoints2013(int n, double x[], double z[], double xl, double xh, double yl, double zh) 
//{
//
//	int p[20],xl,xh,yl,yh,i,n,t[20];
//	float m[10];
//	int k=0;
//	for(i=0;i<2*n;i=i+2) {
//		m[i]=(float)(p[i+3]-p[i+1])/(p[i+2]-p[i]);
//		if(p[i]<xl || p[i]>xh || p[i+1]<yl || p[i+1]>yh)
//		{ 
//			if(p[i]<xl){
//				t[k+1]=p[i+1] +m[i]*(xl-p[i]);
//				t[k]=xl;
//			}
//			else if(p[i]>xh){
//				t[k+1]=p[i+1]+m[i]*(xh-p[i]);
//				t[k]=xh;
//			}
//			if(p[i+1]<yl)
//			{
//				t[k]=p[i]+(yl-p[i+1])/m[i];
//				t[k+1]=yl;
//			}
//			else if(p[i+1]>yh)
//			{
//				t[k]=p[i]+(yh-p[i+1])/m[i];
//				t[k+1]=yh;} k=k+2;
//		}
//		if(p[i]>=xl && p[i]<=xh && p[i+1]>=yl && p[i+1]<=yh)
//		{
//			t[k]=p[i];
//			t[k+1]=p[i+1];
//			k=k+2;
//		}
//		if(p[i+2]<xl || p[i+2]>xh || p[i+3]<yl || p[i+3]>yh)
//		{ 
//			if(p[i+2]<xl){
//				t[k+1]=p[i+3] +m[i]*(xl-p[i+2]);
//				t[k]=xl;
//			}
//			else if(p[i+2]>xh){
//				t[k+1]=p[i+3]+m[i]*(xh-p[i+2]);
//				t[k]=xh;
//			}
//			if(p[i+3]<yl)
//			{
//				t[k]=p[i+2]+(yl-p[i+3])/m[i];
//				t[k+1]=yl;
//			}
//			else if(p[i+3]>yh)
//			{
//				t[k]=p[i+2]+(yh-p[i+3])/m[i];
//				t[k+1]=yh;}k=k+2;
//		}
//	}
//	t[k]=t[0];
//	t[k+1]=t[1];
//
//
//	return 0;
//}
