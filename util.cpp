#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>

#include "math.h"
#include "float.h"
#include "segy.h"
#include "string.h"
#include "suSwapByte.h"
#include "ealloc_d.h"
#include "util.h"

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr




/* Assumes sizeof(int) == 4

	ibm_to_float((int *) tr.data,
	(int *) tr.data, ns,
	endian);
	*/
void ibm_to_float(int from[], int to[], int n)
{
	register int fconv, fmant, i, t;

	for (i = 0; i < n; ++i) {
		fconv = from[i];

		// now the result is for ieee float in big-endian machine. We need to convert to little endian
		//to[i] = (fconv<<24) | ((fconv>>24)&0xff) |
		//		((fconv&0xff00)<<8) | ((fconv&0xff0000)>>8);

		/* next lines modified (M.J.Rutty 20/9/92) */
		/* if (fconv) { */
		/* fmant = 0x00ffffff & fconv; */

		fmant = 0x00ffffff & fconv;
		if (!fmant)
			fconv = 0;
		else {
			/* end modifications */
			t = (int)((0x7f000000 & fconv) >> 22) - 130;
			while (!(fmant & 0x00800000)) { --t; fmant <<= 1; }
			if (t > 254) fconv = (0x80000000 & fconv) | 0x7f7fffff;
			else if (t <= 0) fconv = 0;
			else fconv = (0x80000000 & fconv) | (t << 23) | (0x007fffff & fmant);
		}

		to[i] = fconv;
	}
	return;
}

void float_to_ibm(int from[], int to[], int n)
{

	register int fconv, fmant, i, t;
	if (!from || !to) return;
	for (i = 0; i < n; ++i) {
		fconv = from[i];


		// now convert to ibm format
		if (fconv) {
			fmant = (0x007fffff & fconv) | 0x00800000;
			t = (int)((0x7f800000 & fconv) >> 23) - 126;
			while (t & 0x3) { ++t; fmant >>= 1; }
			fconv = (0x80000000 & fconv) | (((t >> 2) + 64) << 24) | fmant;
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


void convSimple(int lx, int ifx, float *x,
	int ly, int ify, float *y,
	int lz, int ifz, float *z)
{
	// this function has problems
	int ilx = ifx + lx - 1, ily = ify + ly - 1, ilz = ifz + lz - 1, i, j, jlow, jhigh;
	double sum;

	x -= ifx;  y -= ify;  z -= ifz;
	for (i = ifz; i <= ilz; ++i) {
		double denom1 = 0.;
		double denom2 = 0.;
		int count1 = 0;
		int count2 = 0;

		jlow = i - ily;  if (jlow < ifx) jlow = ifx;
		jhigh = i - ify;  if (jhigh > ilx) jhigh = ilx;
		for (j = jlow, sum = 0.0; j <= jhigh; ++j) {
			sum += x[j] * y[i - j];

			// deping's modifications
			denom1 += x[j] * x[j];
			if (fabs(x[j]) > 0.f) count1++;
			denom2 += y[i - j] * y[i - j];
			if (fabs(y[i - j]) > 0.f) count2++;
		}

		if (count1 > 1) denom1 /= (double)count1;
		if (count2 > 1) denom2 /= (double)count2;

		z[i] = (float)sum;
		sum = denom1 * denom2;
		if (sum > 0.) z[i] /= (float)sqrt(sum);
	}
}













//double getReduceTimeExOld(double offset, int drawmode, double vred, double vWater, double Zwater, double Zs, double Zgun)
//{
//	if (drawmode != 7) return getReduceTimeEx(offset, drawmode, vred);
//
//	double rvVred = !isFloatEqual(vred, 0.0)  ?   1./vred  : 0.0;
//	if (Zwater<-10.f || Zwater>15.f) {
//		return 0.f;
//	}
//
//	double reducedTime = 0.0;
//
//
//	// for one side
//	double dZ = Zwater - Zs;
//	double X2 = offset * offset * 0.25; // half offset squared
//
//	// for the other side
//	double dW = Zwater - Zgun;
//	double Y = 0.5 * offset * dW / dZ;
//	// half offset squared for the other half
//
//	reducedTime=
//		sqrt(X2 + dZ*dZ) +
//		sqrt(Y*Y + dW*dW) -
//		Zwater*2.;
//		// note: the above is the John Shilmald display for NMO
//	reducedTime*=rvVred;
//
//	// a final correction, obsDepth~0.06km
//	if (vWater < 0.01 || vWater>2.0) vWater = 1.441f;
//	double rvVwater = 1. / vWater;
//	reducedTime -= Zs * (rvVwater - rvVred);
//
//	// the last correction is for airgun depth, around 0.011km
//	// reducedTime -= 2.0 * Zgun * rvVwater;
//	 reducedTime += 2.0 * Zgun * rvVwater;
//
//
//
//	return reducedTime;
//}

// old version before June 11, 2014
//double getReduceTimeExOld(double offset, int drawmode, double rvred, double vW, double Zw, double Zs, double Zg)
//{
//	if (offset < 0.) offset = -offset;
//	if (drawmode != 7) return getReduceTimeTraditional(offset, drawmode, rvred);
//
//	double dZ = Zw - Zs;
//	if (Zw<-10.f || Zw>15.f) {
//		return 0.f;
//	}
//
//	// double rvVred = !isFloatEqual(vred, 0.0)  ?   1./vred  : 0.0;
//	if (vW < 0.01 || vW>2.0) vW = 1.441f;
//	double rvVwater = 1. / vW;
//
//	double reducedTime = 0.0;
//
//	// for one side
//	double X2 = offset * offset * 0.25; // half offset squared
//
//	// for the other side
//	double dW = Zw - Zg;
//	double Y = 0.5 * offset * dW / dZ;
//	// half offset squared for the other half
//
//	reducedTime = sqrt(X2 + dZ*dZ) + sqrt(Y*Y + dW*dW) - Zw*2. + Zs + Zg;
//	reducedTime *= rvred;
//	reducedTime -= (Zs + 2.*Zg) * rvVwater;
//
//	return reducedTime;
//}


// new version on June 11, 2014
//double getReduceTimeEx2(double offset, int drawmode, double rvred, double vW, double Zw, double Zs, double Zg)
//{
//	if (offset < 0.) offset = -offset;
//	if (drawmode != 7) return getReduceTimeEx(offset, drawmode, rvred);
//
//	double dZ = Zw - Zs;
//	if (Zw<-10.f || Zw>15.f) {
//		return 0.f;
//	}
//
//
//	// Zw, Zg
//	if (vW < 0.01 || vW>2.0) vW = 1.441f;
//	double rvVwater = 1. / vW;
//
//	double reducedTime = 0.0;
//
//	double dW = Zw - Zg;
//	double alpha = dZ / dW;
//	double X2 = offset / (1. + alpha);
//	double X1 = X2 * alpha;
//
//	reducedTime = sqrt(X1*X1 + dZ*dZ) + sqrt(X2*X2 + dW*dW) - Zw*2. + Zs + Zg;
//		// at zero offset, the above is exactly zero
//
//
//	reducedTime *= rvred;
//	reducedTime -= (Zs + Zg) * rvVwater;
//
//	return reducedTime;
//}




// Not including the trailing back slash
CString getSeisWideExePath()
{
	CString name;
	int nPathLength = GetModuleFileName(NULL, name.GetBuffer(_MAX_PATH), _MAX_PATH);
	name.ReleaseBuffer();
	CString progPath = name.Left(name.ReverseFind('\\')).MakeLower();
	//	CString progPath; 
	if (!isDirectoryExist(progPath)) {
		progPath = getSystemTempPathSlash();
		progPath = progPath.Left(progPath.ReverseFind('\\'));
	}

	return addQuotes(progPath);
}
CString getSeisWideParaPath()
{
	//#if defined _M_IX86
	//	cout << _T(" (x86)");
	//#elif defined _M_X64
	//	cout << _T(" (x64)");
	//#elif defined _M_IA64

	CString name;
	int nPathLength = GetModuleFileName(NULL, name.GetBuffer(_MAX_PATH), _MAX_PATH);
	name.ReleaseBuffer();
	CString progPathBak = name.Left(name.ReverseFind('\\')).MakeLower();
	CString progPath;
	BOOL is64bitDebug = progPathBak.Find(_T("x64\\debug")) > 0;
	BOOL isDebug = progPathBak.Find(_T("debug")) > 0;
	if (is64bitDebug)
		progPath = progPathBak + _T("\\..\\..\\docs\\SeisWide64");
	else if (isDebug) {
		progPath = progPathBak + _T("\\..\\docs\\SeisWide32");
	}
	else {
		progPath = progPathBak;
	}

	if (!isDirectoryExist(progPath)) {
		progPath = getSystemTempPathSlash();
		progPath = progPath.Left(progPath.ReverseFind('\\'));
	}

	return progPath;
	// addQuotes() cannot be added to this string since commonly it is appended
	// by another string, and the quote is on the way.

}


CString getGravmodPath()
{
	CString name;
	int nPathLength = GetModuleFileName(NULL, name.GetBuffer(_MAX_PATH), _MAX_PATH);
	name.ReleaseBuffer();
	CString progPathBak = name.Left(name.ReverseFind('\\'));
	CString progPath;
	BOOL isDebug = progPathBak.Find(_T("Debug")) > 0;
	if (isDebug) {
		progPath = progPathBak + _T("\\..\\docs\\SeisWide\\others\\psGravmod\\gravExample");
	}
	else {
		progPath = progPathBak + _T("\\others\\psGravmod\\gravExample");
	}

	if (!isDirectoryExist(progPath))
		return getSystemTempPathSlash();

	return addQuotes(progPath);
}





//
//unsigned short Swap(unsigned short a){
//	return ((a >> 8) & 0x00ff) +
//		((a << 8) & 0xff00);
//}
//short Swap(short a){
//	return ((a >> 8) & 0x00ff) +
//		((a << 8) & 0xff00);
//}
//int Swap(int a){
//	return ((a >> 24) & 0x000000ff) +
//		((a >> 8) & 0x0000ff00) +
//		((a << 8) & 0x00ff0000) +
//		((a << 24) & 0xff000000);
//}
//
//unsigned int Swap(unsigned int a){
//	return ((a >> 24) & 0x000000ff) +
//		((a >> 8) & 0x0000ff00) +
//		((a << 8) & 0x00ff0000) +
//		((a << 24) & 0xff000000);
//}
//void Swap(float* tnf4)
//{  // please test if this functions work ok	
//	int *tni4 = (int *)tnf4;
//	*tni4 = (((*tni4 >> 24) & 0xff) | ((*tni4 & 0xff) << 24) |
//		((*tni4 >> 8) & 0xff00) | ((*tni4 & 0xff00) << 8));
//}



void SwapTapeHeader(bhed* bh){
	short *i2buf = (short *)&bh->jobid;
	int *i4int = (int *)&i2buf[0];
	swap_int_4(&i4int[0]);
	swap_int_4(&i4int[1]);
	swap_int_4(&i4int[2]);
	swap_short_2(&i2buf[6]);
	swap_short_2(&i2buf[7]);  // short nart

	int i;
	for (i = 8; i < 12; i++)
		swap_u_short_2((unsigned short *)&i2buf[i]);
	for (i = 12; i < 42; i++)
		swap_short_2(&i2buf[i]);

	swap_float_4(&bh->lattCorr);
	swap_float_4(&bh->longCorr);
	swap_float_4(&bh->drftrate);
	swap_float_4(&bh->obsPositionX);
	swap_int_4(&bh->ntrExtra);

	for (i = 46; i < 200; i++)
		swap_short_2(&i2buf[i]);
}


void SwapTraceHeader(bool isSegyCurrentlyPCFormat, bool isUseReducedTime, bool isDeadTrcExist, segy* tr){
	short *i2buf = (short *)&tr->tracl;
	int *i4int = (int *)&i2buf[0];
	int i;

	for (i = 0; i < 7; i++)
		swap_int_4(&i4int[i]);
	swap_short_2(&tr->trid);
	swap_short_2(&tr->nvs);
	swap_short_2(&tr->nhs);  // i2buf[16]
	swap_short_2(&tr->duse);  // i2buf[17]    data use: 1 = production; 2 = test
	for (i = 9; i < 17; i++)
		swap_int_4(&i4int[i]);
	swap_short_2(&i2buf[34]);
	swap_short_2(&i2buf[35]);

	//i4int[18:21] is at i2buf[36:42]
	for (i = 18; i < 22; i++)
		swap_int_4(&i4int[i]);

	// [54] is delrt
	for (i = 44; i < 57; i++)
		swap_short_2(&i2buf[i]);

	// [57] is unsigned short ns
	// [58] is unsigned short dt
	for (i = 57; i < 59; i++)
		swap_u_short_2((unsigned short *)&i2buf[i]);

	// [59] is short gain
	for (i = 59; i < 90; i++)
		swap_short_2(&i2buf[i]);

	//bool isSegyCurrentlyUnixFormat = tr->trid < 0 || tr->trid>1000;

	// need to change here
//	bool isUseCmpLocation = false;
	int* anInt = tr->cmp.getInt1();
	if (!isSegyCurrentlyPCFormat)
		swap_int_4(anInt);

	if (*anInt != 0 && *anInt > -MIL && *anInt < MIL) {
		tr->cmp.setInt1(*anInt);
	}
	else {
		// just use normal case
		swap_float_4(&tr->cmp.df[0]);
		swap_float_4(&tr->cmp.df[1]);
		swap_float_4(&tr->cmp.df[2]);
	}


	swap_float_4(&tr->f2);
	swap_float_4(&tr->ungpow);
	swap_float_4(&tr->unscale);

	//refraction reduced time  microsec  is disabled!
	//	swap_int_4(&i4int[45]); //refraction reduced time  microsec.
	swap_int_4(&tr->ntr); // ntr, num of traces
	swap_short_2(&tr->mark);

	if (isUseReducedTime) {
		swap_int_4(&tr->delrtEx); //refraction reduced time.
	}
	else {
		tr->delrtEx = 0;
	}
	if (!isDeadTrcExist) tr->trid = 1;  // all dead traces become activated.

	swap_short_2(&tr->idistopt);
	swap_float_4(&tr->trcdist);
	swap_float_4(&tr->trcOffset);

	// note: i2buf[109] stores the azimuth by Ian
}

//void SwapTraceHeader(segy* tr){
//	SwapTraceHeader(false, false, tr);
//}


CTime getFileModifiedTime(CString f)
{
	if (!isFileExistNotDirectory(f)) return zeroTime;

	CFile cfile;
	CFileStatus status;
	CTime t;
	cfile.Open(f, CFile::modeRead | CFile::shareDenyNone);
	if (cfile.GetStatus(status)) {
		t = status.m_mtime;
	}
	// cfile.Close();  // cannot close it since it was not opened
	return t;
}

// Does File exist and is not a directory
BOOL isFileExistNotDirectory(CString name)
{
	BOOL tt = PathFileExists(name) && !PathIsDirectory(name);
	return tt;

	LPCTSTR lpszFileName = name.GetBufferSetLength(name.GetLength());
	DWORD attrib;
	attrib = GetFileAttributes(lpszFileName);
	return((attrib != 0xFFFFFFFF) && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}

int CALLBACK BrowseCallbackProc(
	HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	TCHAR szDir[MAX_PATH];
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
	{
		// add your initialization code here
		if (GetCurrentDirectory(sizeof(szDir) / sizeof(TCHAR), szDir))
		{
			// WParam is TRUE since you are passing a path.
			// It would be FALSE if you were passing a pidl.
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);
		}
	}
	break;

	case BFFM_SELCHANGED:
	{
		SHGetPathFromIDList(
			reinterpret_cast<LPITEMIDLIST>(lParam), szDir);
		SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0,
			reinterpret_cast<LPARAM>(szDir));
	}
	break;
	}

	return 0;
}


CString BrowseFolderSlash(CString title)
{
	TCHAR t[MAX_PATH];
	LPMALLOC pMalloc = NULL;
	LPITEMIDLIST pidl = NULL;
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		ZeroMemory(&bi, sizeof(bi));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = 0;
		bi.pidlRoot = 0;
		// bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI | BIF_STATUSTEXT;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		bi.lpszTitle = title;
		bi.lpfn = BrowseCallbackProc;
		//	bi.lParam = TRUE;
		pidl = SHBrowseForFolder(&bi);

		if (pidl != NULL)
		{
			//SHGetPathFromIDList(pidl,
			//	t.GetBuffer(t.GetLength()+1));
			SHGetPathFromIDList(pidl, t);
			// UpdateData(FALSE);
			// free memory
			if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc) {
				pMalloc->Free(pidl);
				pMalloc->Release();
			}
		}
	}
	CString out;
	if (!isEndWith(t, _T("\\")))
		out.Format(_T("%s\\"), t);
	else
		out.Format(_T("%s"), t);
	//		t.Append(_T("\\"));

	return out;
}


bool BrowseFolderMaySlash(HWND h, CString title, CString &result)
{
	BROWSEINFO brwinfo = { 0 };
	brwinfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	brwinfo.lpszTitle = title;
	brwinfo.hwndOwner = h;// =
	LPITEMIDLIST pitemidl = SHBrowseForFolder(&brwinfo);
	if (pitemidl == 0)
		return false;

	// get the full path of the folder
	TCHAR path[MAX_PATH];
	if (SHGetPathFromIDList(pitemidl, path))
	{
		result = path;
	}
	IMalloc *pMalloc = 0;
	if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		pMalloc->Free(pitemidl);
		pMalloc->Release();
	}

	return true;
}

BOOL isDirectoryWritable(CString name)
{
	LPCTSTR lpszFileName = name.GetBufferSetLength(name.GetLength());
	DWORD attrib;
	attrib = GetFileAttributes(lpszFileName);
	return((attrib != 0xFFFFFFFF) && !(attrib & FILE_ATTRIBUTE_READONLY));
}

//BOOL isFileExistNotDirectory(CString name)
//{
//	LPCTSTR lpszFileName = name.GetBufferSetLength(name.GetLength());
//   DWORD attrib;
//   attrib = GetFileAttributes(lpszFileName);
//   return (attrib != 0xFFFFFFFF) ;
//}
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
	BOOL isExistTarget = CFile::GetStatus(strTarget, statusTarget); // static function
	CFileStatus statusCompare;
	BOOL isExistCompare = CFile::GetStatus(strCompare, statusCompare); // static function
	if (!isExistTarget || !isExistCompare) {
		return FALSE;
	}

	if (statusTarget.m_mtime >= statusCompare.m_mtime)
		return TRUE;

	return FALSE;
	//	CTime curTime = CTime::GetCurrentTime();

}

BOOL isSameFileSizeTime(CString strTarget, CString strCompare)
{
	// will return FALSE is any files have problems. 
	// will return TRUE only if the target is same time and size as the other file
	CFileStatus statusTarget;
	BOOL isExistTarget = CFile::GetStatus(strTarget, statusTarget); // static function
	CFileStatus statusCompare;
	BOOL isExistCompare = CFile::GetStatus(strCompare, statusCompare); // static function
	if (!isExistTarget || !isExistCompare) {
		return FALSE;
	}

	// there is a catch: it happens when an older dataset has set all file update time
	// to be the same. If r.in and rOBS7.in, rOBS6.in are all the same size and time
	// we cannot overwrite r.in any more if we compare time and size.
	CTime ct = CTime::GetCurrentTime();
	if (ct.GetYear() != statusCompare.m_mtime.GetYear())
		return FALSE;
	if (ct.GetYear() != statusTarget.m_mtime.GetYear())
		return FALSE;


	if (statusTarget.m_mtime == statusCompare.m_mtime
		&& statusTarget.m_size == statusCompare.m_size)
		return TRUE;

	return FALSE;
}

// since the path may be changed, we need to adjust path for a few other files according to the current .sei file
CString getSecNameRevised(CString secName, CString seiName)
{
	int pos, pos2;
	pos = seiName.Find(_T("."));
	if (pos == 0) return secName; // no change

	seiName = CString(GetPathFromFileSlash(seiName));

	// need to determine if the SEI file and SGY files are moved to a another directory. If so, revise.
	if (seiName.IsEmpty()) return secName; // no changes
	if (secName.IsEmpty()) return secName; // no changes
	pos = secName.Find(_T("."));
	pos2 = secName.Find(_T(".."));
	if (pos == 0 && pos2<0) {
		return seiName + secName.Mid(1);
	}
		
	// if there are no colon signs inside, no need to change anything. This will help prevent miss-use such as secName="a" et al.
	// and colons are prohibited from being inside actual base file names by Windows standard

	//  Need to handle network server \\some-server\some\root\XX\a.sei
	//  some user has mapped a network drive Z:\XX\  to equal to a 

	// if the seiName and secName are at diferent drive, we first change secName to that drive, and test again
	// HELP: I have not considered network drive here!
	TCHAR c;
	pos = secName.Find(_T(":"));
	if (pos == 1) {
		c = seiName.GetAt(0);
		if (c != secName.GetAt(0)) {
			secName.SetAt(0, c);
		}
	}

	if (isFileExistNotDirectory(secName))
		return secName;  // this means it is just a drive letter change
	
	CString baseName = getFileNameOnly(secName);

	// continue revising the secName
	secName.TrimRight(_T('\r'));
	secName.TrimRight(_T('\n'));
	pos = secName.Find(_T('\n'));
	int len = secName.GetLength();
	CString t, tNew, tOld;
	if (pos < 0) {
		// meaning that this is single file name.
		// need to get the starting directory to search for file, based on seiName
		tOld = GetPathFromFileSlash(secName);
		tNew = GetPathExistingFromFileSlash(seiName);

		// first, put the last path segment into the newer path, and see if the file exist
		pos = tOld.Mid(0, tOld.GetLength()-1).ReverseFind(CSLASH);
		if (pos > 0) {
			CString aSegment = tOld.Mid(pos + 1);
			t = tNew + aSegment;
			if (isFileExistNotDirectory(t + baseName))
				return t + baseName;
			else if (isDirectoryExist(t))
				return t + baseName; // still acceptable even the file does not exist
					// but the directory exist at least
		}

		return tNew + baseName;
	}
	else {
		// meaning that this string contains multiple lines, containing multiple file names
		CStringArray *arr = new CStringArray();
		SplitStringArr(secName, COMMA, arr, false);
		CStringArray *arr2 = new CStringArray();
		for (int i = 0; i < arr->GetCount(); i++) {
			arr2->Add(getSecNameRevised(arr->ElementAt(i), seiName));
		}
		t = convStringArrayToCString(arr2, true);
		arr->RemoveAll();
		arr2->RemoveAll();
		delete arr;
		delete arr2;
		return t;
	}

	// now the file not found in new dir, so we just keep the original sec file.
	return secName;
}

// strAppend will be inserted to the original filename while keeping the same extension
CString getFileNameAppend(CString aName, CString strAppend)
{
	if (strAppend.GetLength() < 1) return aName;
	CString tStr;
	int pathLen = aName.ReverseFind('.');
	if (pathLen > (-1)) {
		CString t = aName.Left(pathLen);
		tStr.Format(_T("%s%s%s"), t, strAppend, aName.Mid(pathLen));
		return tStr;
	}

	return aName + strAppend;
}

CString getFileNameNextVersion(CString aName) {
	CString preOut, out, tStr;
	CString aExt = DOT + GetFileExtension(aName);
	int len = aName.ReverseFind('.');
	if (len < 0)
		preOut = aName;
	else
		preOut = aName.Mid(0, len);
	int iVer = 0;
	tStr.Format(_T("%d"), iVer);
	out = preOut + tStr + aExt;
	while (isFileExistNotDirectory(out)) {
		iVer++;
		tStr.Format(_T("%d"), iVer);
		out = preOut + tStr + aExt;
	}
	return out;
}

// create a file name append in the temp directory instead of the current dir
CString getFileNameAppendTemp(CString aName, CString strAppend)
{
	CString tStr = getFileNameAppend(aName, strAppend);
	CString tExt = GetFileExtension(tStr);
	return getFileNameTemp(tStr, tExt, TRUE);

}

BOOL isEndWith(CString name, CString needle)
{
	if (name.MakeLower().Right(needle.MakeLower().GetLength()) == needle) return TRUE;
	return FALSE;
}

BOOL waitForFileUnlocked(CString f)
{
	int numMaxCheck = 10;
	int count = 0;
	CFile file;
	while (!file.Open(f, CFile::modeRead | CFile::shareDenyNone)) {
		// here, you should check if the error is "Access denied"
		// and probably break if it is anything else...
		Sleep(1000);
		count++;
		if (count > numMaxCheck) {
			// we give up after many times of trying
			return FALSE;
		}
	}
	file.Close();
	return TRUE;
}


double getFreeSpace(CString f)
{
	int pathLen = f.ReverseFind('\\');
	if (pathLen > (-1)) SetCurrentDirectory(f.Left(pathLen));

	ULARGE_INTEGER iwFreeSpace, iwTotalBytes;
	BOOL truefalse = GetDiskFreeSpaceEx(NULL, &iwFreeSpace, &iwTotalBytes, NULL);

	double dwFreeSpace = (DOUBLE)iwFreeSpace.LowPart;
	dwFreeSpace += ((DOUBLE)iwFreeSpace.HighPart)*ULONG_MAX;
	dwFreeSpace /= MIL;

	return dwFreeSpace;
}

double getMemoryUsagePhysPercent()
{
	// Use to convert bytes to KB
#define DIV 1024

	// Specify the width of the field in which to print the numbers. 
	// The asterisk in the format specifier "%*I64d" takes an integer 
	// argument and uses it to pad and right justify the number.
#define WIDTH 7

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	return statex.dwMemoryLoad;

	//_tprintf(TEXT("There is  %*ld percent of memory in use.\n"),
	//	WIDTH, statex.dwMemoryLoad);
	//_tprintf(TEXT("There are %*I64d total KB of physical memory.\n"),
	//	WIDTH, statex.ullTotalPhys / DIV);
	//_tprintf(TEXT("There are %*I64d free  KB of physical memory.\n"),
	//	WIDTH, statex.ullAvailPhys / DIV);
	//_tprintf(TEXT("There are %*I64d total KB of paging file.\n"),
	//	WIDTH, statex.ullTotalPageFile / DIV);
	//_tprintf(TEXT("There are %*I64d free  KB of paging file.\n"),
	//	WIDTH, statex.ullAvailPageFile / DIV);
	//_tprintf(TEXT("There are %*I64d total KB of virtual memory.\n"),
	//	WIDTH, statex.ullTotalVirtual / DIV);
	//_tprintf(TEXT("There are %*I64d free  KB of virtual memory.\n"),
	//	WIDTH, statex.ullAvailVirtual / DIV);


	//  Sample output:
	//  There is       51 percent of memory in use.
	//  There are 2029968 total KB of physical memory.
	//  There are  987388 free  KB of physical memory.
	//  There are 3884620 total KB of paging file.
	//  There are 2799776 free  KB of paging file.
	//  There are 2097024 total KB of virtual memory.
	//  There are 2084876 free  KB of virtual memory.
	//  There are       0 free  KB of extended memory.

}


// provide an output of a temporary file name under any specified sub-dir under a tree with an existing file 
// strExt: will be appended as an extension under /temp to the original filename 
//***************this has problem of double dots, pls fix.****************************//
CString getFileNameSubdir(CString aName, CString aSubdir, CString strExt, bool isCreate)
{
	if (aName.IsEmpty()) return _T(""); // this happens frequently
	CString tStr;
	strExt.Trim();
	aSubdir.Trim();
	if (strExt.GetAt(0) != '.') strExt = _T(".") + strExt;
	CString txPath;

	int pathLen = aName.ReverseFind('\\');
	if (pathLen > (-1)) {
		if (!isEndWith(aName.Left(pathLen), aSubdir))
			txPath.Format(_T("%s\\%s"), aName.Left(pathLen), aSubdir);
		else
			txPath.Format(_T("%s"), aName.Left(pathLen));
	}
	else {
		TCHAR curDir[_MAX_PATH];
		GetCurrentDirectory(_MAX_PATH, curDir);
		txPath = CString(curDir) + _T("\\") + aSubdir;
	}

		
	if (!isDirectoryExist(txPath)) {
		// create the sub dir
		if (!isCreate) return BLANK; // since we are not allowed to create anything, we return blank
		if (!CreateDirectory(txPath, NULL)) {
			// failed to create the sub dir. We use system temp dir instead
			tStr.Format(_T("%s\\%s.%s"), getSystemTempPathSlash(), GetBaseFileName(aName), strExt);
			return tStr;
		}
	}
		
	// now the sub dir exists
	int pathLen2 = aName.ReverseFind('.');
	if (pathLen2 > -1) {
		tStr.Format(_T("%s\\%s%s"), txPath, aName.Mid(pathLen + 1, pathLen2 - pathLen - 1), strExt);
	}
	else
		tStr.Format(_T("%s\\%s%s"), txPath, aName.Mid(pathLen + 1), strExt);
	////	return txPath + _T("\\") + aName.Mid(pathLen+1) + _T("." + strExt;
	//// cannot find the path
	//tStr.Format(_T("%s%s"), aName, strExt);
	return tStr;
}

CString getFileNameTemp(CString aName, CString strExt)
{
	CString tStr = getFileNameSubdir(aName, _T("temp"), strExt, true);
	tStr.Replace(_T("\\temp\\temp"), _T("\\temp"));
	return tStr;
}

CString getFileNameTempNospace(CString aName, CString strExt)
{
	if (aName.Find(SPACE) < 0) return getFileNameTemp(aName, strExt);

	// now, the file name has space. We do not want any space inside.
	// so we just use the system temp path
	CString out = getSystemTempPathSlash() + _T("seiTmpPdf.a");
	return getFileNameTemp(out, strExt);
}

BOOL isInsideTempDir(CString aName)
{
	CString str = aName;
	return str.FindOneOf(_T("\\temp\\")) >= 0;
}

CString getFileNameTemp(CString aName, CString strExt, BOOL isOverwrite)
{
	CString t = getFileNameTemp(aName, strExt);

	if (!isOverwrite) {
		// now the file exists, we want to make a different filename
		while (isFileExistNotDirectory(t)) {
			t = getFileNameAppend(t, _T("a"));
		}
	}
	return t;
}


// GetBaseFileName
// This helper function returns file name without extension, without path included
CString GetBaseFileName(CString sFileName)
{
	CString sBase = sFileName;
	int pos1 = sFileName.ReverseFind('\\');
	if (pos1 >= 0)
		sBase = sBase.Mid(pos1 + 1);

	int pos2 = sBase.ReverseFind('.');
	if (pos2 >= 0) {
		sBase = sBase.Mid(0, pos2);
	}
	return sBase;
}

// note: this will return a path ended with "\", so do not add another "\" to your file if you concat
CString GetPathFromFileSlash(CString sFileName) {
	int pos1 = sFileName.ReverseFind('\\');
	if (pos1 >= 0)
		return sFileName.Left(pos1 + 1);
	else return _T(".\\"); // when no path info exists, we just return .\ which means the current folder with slash at the end to be compatible with everything
}

// this will loop to parent to find an existing path
CString GetPathExistingFromFileSlash(CString sPathName) {
	CString out = sPathName;
	
	int pos = out.ReverseFind('\\');
	if (pos == out.GetLength() - 1)
		out = out.Mid(0, pos); // now sPathName will not have any slash trailing

	if (!isDirectoryExist(out)) {
		pos = out.ReverseFind('\\');
		if (pos >= 0)
			out = out.Mid(0, pos); // remove its last trailing slash again

		// now the directory may still not exist, but that's OK!
		return out;
	}
	else {
		return out + _T("\\");
	}
}

// GetFileExtension
// This helper function returns file extension by file name
CString GetFileExtension(CString sFileName)
{
	CString sExt;
	int pos = sFileName.ReverseFind('.');
	if (pos >= 0) {
		sExt = sFileName.Mid(pos + 1);
	}
	return sExt.MakeLower();
}

CString SetFileExtension(CString sFileName, CString strExt)
{
	if (strExt.Find(_T('.')) == 0) {
		strExt = strExt.Mid(1);
	}

	if (GetFileExtension(sFileName).CompareNoCase(strExt) != 0) {
		return GetPathFromFileSlash(sFileName) + GetBaseFileName(sFileName) + _T(".") + strExt;
	}

	return sFileName;
}

// this helper function returns modified file name with appended string and revised extension, and
// positioned in the standard temp directory of SeisWide
CString GetFileNameAppendExt(CString sFileName, CString strAppend, CString strExt) {
	
	return SetFileExtension(getFileNameAppend(sFileName, strAppend), strExt);
}





void getAllPathArray(CStringArray *m_arr, CString aPath) {
	// aPath must be absolute path, or with wildcard
	CString wildPath = (aPath.Find(_T("*"), 0) >= 0) ? aPath : aPath + _T("\\*.*");

	CFileFind *finder = new CFileFind();
	BOOL bResult = finder->FindFile(wildPath);
	while (bResult)
	{
		bResult = finder->FindNextFile();
		CString aName = (LPCTSTR)finder->GetFileName();
		if (aName == _T(".") || aName == _T("..")) continue;
		CString aPath2 = (LPCTSTR)finder->GetFilePath();
		m_arr->Add(aPath2);

		if (!isFileExistNotDirectory(aName)) {
			getAllPathArray(m_arr, aPath2);
		}
	}
	finder->Close();
	delete finder;
}

CStringArray* getAllPathArray(CString aPath) {
	// aPath2 must be absolute path
	CStringArray 	*m_arr = new CStringArray();				// CString Array to hold Directory Structures
	m_arr->RemoveAll();					// Not really necessary - Just habit

	getAllPathArray(m_arr, aPath);

	return m_arr;
}


// this function does not read unix text file properly?
CStringArray* getLinesFromFileWin(CString aFile) {
	// aFile must have absolute path
	if (!isFileExistNotDirectory(aFile)) return NULL;

	FILE *fp;
	_TCHAR str[255];
	if (_tfopen_s(&fp, aFile, _T("r")) != 0) return NULL;
	CString cstr;
	CStringArray 	*m_arr = new CStringArray();				// CString Array to hold Directory Structures
	while (true) {
		TRY{
			if (_fgetts(str, 255, fp) == NULL) break;
			int len = (int)_tcslen(str);
			if (len <= 1 || str[0] == '#') continue; // we do not need comment lines
			cstr.Empty();
			cstr.Append(str);
			cstr.Replace(TAB, SPACE);
			cstr.Remove(_T('\r'));
			cstr.Remove(_T('\n'));
			cstr = cstr.Trim();
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

// if numToken==1, we will only extract those lines for the first token
// if numToken==2, we will only extract those lines for the second token ...
CStringArray* getLinesFromString(CString str, bool isNeedComments, CString token, int numToken) {
	if (numToken < 0) numToken = 0;
	int i = -1;
	int count = 0;
	while (true) {
		i = str.Find(token, i+1);
		if (i<0) break; // no token found, so we will not do anything
		if (count + 1 >= numToken) {
			// we will take the current token
			int j = str.Find(token, i + 1);
			if (j < 0) j = str.GetLength() - 1;
			return getLinesFromCString(str.Mid(i, j - i + 1), isNeedComments);
		}
		count++;
		if (count > 188) {
			int refre = 1;
		}
	}
	return NULL;
}


// this will read in unix format as well
// please check if the result is NULL, to avoid exception
CStringArray* getLinesFromFile(CString aFile, bool isNeedComments) {
	// aFile must have absolute path
	if (!isFileExistNotDirectory(aFile)) return NULL;

	CString cstr = getTextFromFileUnix(aFile);
	CStringArray 	*m_arr = getLinesFromCString(cstr, isNeedComments);
	return m_arr;
}

CStringArray* getLinesFromCStringSlow(CString str, bool isNeedComments) {
	if (str.GetLength() < 1) return NULL;
	CStringArray 	*m_arr = new CStringArray();				// CString Array to hold Directory Structures

	int nWin = str.Find(CARRAGE);
	int nUnix = str.Find(_T("\n"));
	CString strLineFeed = (nUnix > 0 && nWin <= 0) ? _T("\n") : CARRAGE;

	while (!str.IsEmpty()) {
		CString one_line = str.SpanExcluding(strLineFeed);
		//one_line = one_line.Trim();
		if (one_line.GetLength() > 0 && (isNeedComments || one_line[0] != _T('#'))) {
			m_arr->Add(one_line.Trim());
		}

		str = str.Right(str.GetLength() - one_line.GetLength()).TrimLeft(strLineFeed);

		// str may have space at the front
		str.Trim();
	}

	return m_arr;

}

// We will auto convert all tabs into spaces
CStringArray* getLinesFromCString(CString str, bool isNeedComments) {
	if (str.GetLength() < 1) return NULL;
	str.Replace(TAB, SPACE);

	CStringArray 	*m_arr = new CStringArray();				// CString Array to hold Directory Structures

	int nWin = str.Find(CARRAGE);
	int nUnix = str.Find(_T("\n"));
	CString strLineFeed = (nUnix > 0 && nWin <= 0) ? _T("\n") : CARRAGE;
	int lenFeed = strLineFeed.GetLength();

	int i = 0;
	int len = str.GetLength();
	while (i < len) {
		register int i2 = str.Find(strLineFeed, i);
		register CString one_line = i2<0 ? str.Mid(i) :  str.Mid(i, i2-i); // when no line feed, we want to use all the rest of the string
		register int n = one_line.GetLength();
		if (one_line.GetLength() > 0 && (isNeedComments || one_line[0] != _T('#'))) {
			//m_arr->Add(one_line.Trim());
			m_arr->Add(one_line); // we choose not to trim every line, since kml/xml format would need spaces
		}
		i += n + lenFeed;
		//str = str.Right(str.GetLength() - one_line.GetLength()).TrimLeft(strLineFeed);
		//// str may have space at the front
		//str = str.Trim();
	}

	return m_arr;

}

// this function works for ascii or multi byte characters, and the output will be Unicode string
CString getTextFromFileUnix(CString aFile) {
	if (!isFileExistNotDirectory(aFile)) return BLANK;

	CString strOut;
	CFile file;
	if (file.Open(aFile, CFile::modeRead | CFile::shareDenyNone)) {
		UINT len = (UINT)file.GetLength() + 1; // if I do not add 1, sometimes I have unknown exception!!!
		char *buf = (char *)alloc1(len, 2);
			// note: if this size is 1, it is exception when ZeroMemory() or reading from file. Why?

		::ZeroMemory(buf, len); // ensure string is null terminated 
		file.Read(buf, len);

		TCHAR *tbuf = (TCHAR *)alloc1(len+1, sizeof(TCHAR)); // unicode character buffer 

		// convert ascii to unicode
		INT lv_Len = MultiByteToWideChar(CP_ACP, 0, buf, -1, tbuf, len);
		tbuf[len] = 0;  
		// this indicates the ending of string. If this is not done, sometimes there are
		// extra garbage at the end of the string
		
		strOut = CString(tbuf);
		if (strOut.Find(_T("\r\r")) > 0)
			strOut.Replace(_T("\r\r"), _T("\r")); 
		// a double carrage return can cause difficulty in parsing lines. It can happen when double conversio between PC and linux

		free1(tbuf);
		free1(buf);
	}
	file.Close();
	return strOut.Trim(); // since we have an extra space added at the end, we need to trim before outputting
}

// Why this function does not need to return any value. 
void getLinesFromFile(CStringArray* m_arr, CString aFile, bool isNeedComments, int numLinesDesired) {
	// aFile must have absolute path
	if (!m_arr || !isFileExistNotDirectory(aFile)) return;

	CString cstr = getTextFromFileUnix(aFile);
	CStringArray* arr = getLinesFromCString(cstr, isNeedComments);
	if (!arr) return;
	if (arr->GetCount() > 0) {
		// it is possible that no lines exist
		m_arr->RemoveAll();
		int count = 0;
		for (int i = 0; i < arr->GetCount(); i++) {
			register CString astr = arr->ElementAt(i).Trim();
			if (astr.GetLength() < 1) continue;
			if (!isNeedComments && astr.GetAt(0) == _T('<'))
				continue; // skip label lines for GMT format
			m_arr->Add(astr);
			count++;
			if (numLinesDesired > 0 && count >= numLinesDesired) break;
		}
	}
	arr->RemoveAll();
	delete arr;
}

CString getLineFirst(CString aFile) {
	// aFile must have absolute path
	if (!isFileExistNotDirectory(aFile)) return BLANK;
	
	FILE *fp;
	TCHAR str[255];
	if (_tfopen_s(&fp, aFile, _T("r")) != 0) return BLANK;
	while (true) {
		TRY{
			if (_fgetts(str, 255, fp) == NULL) break;
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

CString getLineSecond(CString aFile) {
	// aFile must have absolute path
	if (!isFileExistNotDirectory(aFile)) return BLANK;

	FILE *fp;
	TCHAR str[255];
	CString t;
	if (_tfopen_s(&fp, aFile, _T("r")) != 0) return BLANK;
	while (true) {
		if (_fgetts(str, 255, fp) == NULL) break;
		if (str[0] == '#') continue; // we do not need comment lines
		if (_fgetts(str, 255, fp) == NULL) {
			t = CString(str);
			t.Trim();
			if (t.GetLength() > 0) break;
		}
		else {
			t = CString(str);
			t.Trim();
		}
	}
	fclose(fp);
	return t;
}

bool appendToTextFile(CString aFile, CString str) {
	// aFile must have absolute path
	if (!isFileExistNotDirectory(aFile)) return false;

	FILE *fp;
	if (_tfopen_s(&fp, aFile, _T("a")) != 0) return false;

	TRY{
		_ftprintf(fp, _T("%s"), (LPCTSTR)str);
	}
		CATCH_ALL(e)
	{ }
	END_CATCH_ALL
		fclose(fp);
	return true;
}

CString getLineFirstComment(CString aFile) {
	// aFile must have absolute path
	if (!isFileExistNotDirectory(aFile)) return BLANK;

	FILE *fp;
	TCHAR str[255];
	CString cstr;
	if (_tfopen_s(&fp, aFile, _T("r")) != 0) return BLANK;
	while (true) {
		TRY{
			if (_fgetts(str, 255, fp) == NULL) break;
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

bool isTxtMedicalLocus(CString aFile) {
	CStringArray arr;
	getLinesFromFile(&arr, aFile, false, 1);
	if (arr.GetCount() > 0) {
		CString cstr = arr.ElementAt(0);
		arr.RemoveAll();
		cstr = cstr.Trim();
		if (cstr.Find(_T("LOCUS")) == 0) {
			return true;
		}
	}

	arr.RemoveAll();
	return false;
}


int getTxtNumColumns(CString cstr) {
	size_t j;
	cstr.Replace(TAB, SPACE);

	CStringArray *arr2 = new CStringArray();
	SplitStringArr(cstr, SPACE, arr2, false);
	size_t count2 = arr2->GetCount();

	// need to detect where the string starts
	for (j = 0; j < count2; j++) {
		CString cstr2 = arr2->ElementAt(j);
		if (cstr2.GetLength() < 1 || !IsNumeric(cstr2)) {
			break;
		}
	}
	arr2->RemoveAll();
	delete arr2;
	size_t out = j;
	return (int)out;
}

int getTxtFileNumColumns(CString aFile) {
	CStringArray arr;
	getLinesFromFile(&arr, aFile, false, 1);
	int out = getTxtNumColumns(arr.ElementAt(0));
	arr.RemoveAll();
	return out;
}

bool isTxtFile6Columns(CString aFile) {
	CStringArray arr;
	getLinesFromFile(&arr, aFile, false, 1);
	if (arr.GetCount() > 0) {
		CString cstr = arr.ElementAt(0);
		arr.RemoveAll();
		cstr.Replace(TAB, SPACE);
		SplitStringArr(cstr, SPACE, &arr, false);
		size_t count2 = arr.GetCount();
		if (count2 >= 6) {
			cstr = arr.ElementAt(5); // last one should be string
			for (int j = 0; j < 5; j++) {
				if (!IsNumeric(arr.ElementAt(j))) {
					arr.RemoveAll();
					return false;
				}
			}
			arr.RemoveAll();
			return true;
		}
	}

	arr.RemoveAll();
	return false;
}


CString conv6ColTo3Col(CString aFile) {
	CStringArray arr;
	CStringArray arrT;
	CStringArray arrOut;
	CString cstr;
	getLinesFromFile(&arr, aFile, false, 0); // read all lines while discarding all comment lines
	for (int i = 0; i < arr.GetCount(); i++) {
		cstr = arr.ElementAt(i).Trim();
		cstr.Replace(TAB, SPACE);
		if (cstr.GetLength() < 3) continue;
		SplitStringArr(cstr, SPACE, &arrT, false);
		if (arrT.GetCount() < 6) continue;
		register double v = StringToFloat(arrT.ElementAt(1));
		register double depth = StringToFloat(arrT.ElementAt(2));

		cstr.Format(_T("%f %f %s"), v, depth, arrT.ElementAt(5));
		arrOut.Add(cstr);

	}

	CString fOut = getFileNameAppend(aFile, _T("_3Col"));
	if (!saveStringArrayToFile(&arrOut, fOut))
		fOut = aFile;  // if cannot save, indicate it fails
	arr.RemoveAll();
	arrT.RemoveAll();
	arrOut.RemoveAll();

	return fOut;
}

CString conv6ColTo3ColSubSeafloor(CString aFile) {
	CStringArray arr;
	CStringArray arrT;
	CStringArray arrOut;
	CString cstr;
	getLinesFromFile(&arr, aFile, false, 0); // read all lines while discarding all comment lines
	bool isSeaSurfacePrev = false;
	double waterDepth = 0.;
	for (int i = 0; i < arr.GetCount(); i++) {
		cstr = arr.ElementAt(i).Trim();
		cstr.Replace(TAB, SPACE);
		if (cstr.GetLength() < 3) continue;
		SplitStringArr(cstr, SPACE, &arrT, false);
		if (arrT.GetCount() < 6) continue;
		register double v = StringToFloat(arrT.ElementAt(1));
		register double depth = StringToFloat(arrT.ElementAt(2));

		// identify if this line is for the sea surface
		if (fabs(v - 1.44) < 0.01) {
			isSeaSurfacePrev = true;
			continue;
		}

		if (isSeaSurfacePrev) {
			waterDepth = depth;
			isSeaSurfacePrev = false;
		}

		depth -= waterDepth;

		if (depth >= 0.) {
			cstr.Format(_T("%f %f %s"), v, depth, arrT.ElementAt(5));
			arrOut.Add(cstr);
		}

	}

	CString fOut = getFileNameAppend(aFile, _T("_3ColSubseafloor"));
	if (!saveStringArrayToFile(&arrOut, fOut))
		fOut = aFile;  // if cannot save, indicate it fails
	arr.RemoveAll();
	arrT.RemoveAll();
	arrOut.RemoveAll();

	return fOut;
}

/*
// calc based on basement depth in the comment line:

#Pseudo-well at Model Distance  35.000 (offset   0.000 km; V4 marker at   9.912 km)(Params:   4.699 km/s;  -1.000 km)
#TWT    VEL    DEPTH   VRMS  DBEPTH-BV4 MODEL (sec-km/s-km-km)
0.000  1.440  0.000  1.440  0.000      F:\arcticSBs\sb2007\09\model07_09t3.bin
4.401  1.510  3.245  1.475  0.000      F:\arcticSBs\sb2007\09\model07_09t3.bin
4.411  1.650  3.255  1.475  0.000      F:\arcticSBs\sb2007\09\model07_09t3.bin

*/
CString conv6ColTo3ColSubBasement(CString aFile) {
	CStringArray arr;
	CStringArray arrT;
	CStringArray arrOut;
	CString cstr;
	getLinesFromFile(&arr, aFile, true, 0); // read all lines while discarding all comment lines
	double basementDepth = -1.;
	bool isDataLine = false;
	for (int i = 0; i < arr.GetCount(); i++) {
		cstr = arr.ElementAt(i).Trim();
		cstr.Replace(TAB, SPACE);
		if (cstr.GetLength() < 3) continue;

		if (cstr.GetAt(0) == _T('#')) {
			// a comment line
			if (isDataLine) {
				// the previous line was data line, and this line is not. So we have to reset the basementDepth = -1.
				basementDepth = -1.;
				isDataLine = false;
			}

			int m = cstr.Find(_T("V4 marker"));
			if (m < 0) continue;
			int n = cstr.Find(_T("km"), m);
			if (n < 0) continue;
			CString tstr = cstr.Mid(m + 9, n - m - 7).Trim();
			CStringArray *parr = CString2LinesBySpace(cstr.Mid(m + 9, n - m - 7).Trim());
			if (parr->GetCount() < 0) continue;
			for (int j = 0; j < parr->GetCount(); j++) {
				if (IsNumeric(parr->ElementAt(j))) {
					basementDepth = StringToFloat(parr->ElementAt(j));
				}
			}
			parr->RemoveAll();
			delete parr;
			continue;
		}

		SplitStringArr(cstr, SPACE, &arrT, false);
		if (arrT.GetCount() < 6) continue;
		register double v = StringToFloat(arrT.ElementAt(1));
		register double depth = StringToFloat(arrT.ElementAt(2));

		depth -= basementDepth;

		if (depth >= 0.) {
			cstr.Format(_T("%f %f %s"), v, depth, arrT.ElementAt(5));
			arrOut.Add(cstr);
		}
		isDataLine = true;
	}

	CString fOut = getFileNameAppend(aFile, _T("_3ColSubBasement"));
	if (!saveStringArrayToFile(&arrOut, fOut))
		fOut = aFile;  // if cannot save, indicate it fails
	arr.RemoveAll();
	arrT.RemoveAll();
	arrOut.RemoveAll();

	return fOut;
}



CString convStringArrayToCString(CStringArray* m_arr, bool isLineFeed) {
	int i;
	int count = 0;
	for (i = 0; i < m_arr->GetCount(); i++) {
		count += m_arr->ElementAt(i).GetLength() + 2;
	} // for i

	TCHAR* cppArray = (TCHAR*)alloc1(count+1, sizeof(TCHAR)); /* Assume oStringArray is an object of CStringArray */
	memset(cppArray, 0, (count + 1)*sizeof(TCHAR));
	//int nBytes = 0;
	int nChars = 0;
	for (i = 0; i < m_arr->GetCount(); i++) {
		register CString oString = m_arr->ElementAt(i); /* Get CString object at given position */
		register int lenThis = oString.GetLength();
		memcpy(&cppArray[nChars], oString, lenThis*sizeof(TCHAR)); /* copy string over */
		nChars += lenThis;
		if (isLineFeed && i < m_arr->GetCount() - 1) {
			memcpy(&cppArray[nChars], CARRAGE, 2 * sizeof(TCHAR)); /* copy string over */
			nChars += 2;
		}
	}
	cppArray[nChars] = '0';

	CString out(cppArray, nChars);
	free1(cppArray);
	return out;
}

bool saveStringArrayToFile(CStringArray* m_arr, FILE* hFile) {
	// aFile must have absolute path
	if (!hFile) return false;
	CString tStr; // just a pointer
	TCHAR TF = _T('\n');
	for (int i = 0; i < m_arr->GetCount(); i++) {
		tStr = m_arr->ElementAt(i);
		tStr.Remove(_T('\r'));
		if (tStr.ReverseFind(TF) != tStr.GetLength() - 1)
			tStr += TF;
		// note: if I include \r\n here, the notepad will display correctly, but when it's copied and pasted to excel, it will contain double lines

		_ftprintf(hFile, _T("%s"), (LPCTSTR)tStr);
	} // for i
	return true;
}
bool saveStringArrayToFile(CStringArray* m_arr, CString aFile) {
	// aFile must have absolute path
	FILE *fp;
	if (_tfopen_s(&fp, aFile, _T("w")) != 0) return false;
	saveStringArrayToFile(m_arr, fp);
	fclose(fp);
	return true;
}

// aFile must have absolute path. All strings must be ascii only here.
bool saveStringArrayToFileAsciiUnix(CStringArray* m_arr, CString aFile) {
	int lenMax = 0;
	for (int i = 0; i < m_arr->GetCount(); i++) {
		lenMax = max(lenMax, m_arr->ElementAt(i).GetLength());
	}
	char *buf = (char *)alloc1(lenMax + 1, 1); // must add another char

	INT lv_Len;
	TCHAR TF = _T('\n');
	CFile file;
	if (file.Open(aFile, CFile::modeCreate | CFile::modeWrite)) {
		INT_PTR nLines = m_arr->GetCount();
		for (int i = 0; i < nLines; i++) {
			::ZeroMemory(buf, lenMax + 1); // ensure string is null terminated 
			CString aStr = m_arr->ElementAt(i);
			aStr.Remove(_T('\r'));  // unix text file does not have \r ?
			if (i < nLines - 1 && aStr.ReverseFind(TF) != aStr.GetLength() - 1)
				aStr += TF;
			// note: if I include \r\n here, the notepad will display correctly, but when it's copied and pasted to excel, it will contain double lines

			int len = aStr.GetLength();
			lv_Len = WideCharToMultiByte(
				CP_ACP, 0, aStr, -1, buf, len, NULL, NULL);

			//if (lv_Len < 0) lv_Len = 0;
			//if (lv_Len < in_MaxLen) buf[lv_Len] = 0;
			//else if (buf[in_MaxLen - 1]) buf[0] = 0;


			// found that the last chars of files contain \n \x1 and ? and the last 2 chars have problems
			// len--; // the last char is 0 and should be removed first
			while (buf[len - 1] == 63 
				|| buf[len - 1] <= 0 
				|| buf[len - 1] == '\x1'
				|| buf[len - 1] >126)
				len--;

			//char a = buf[lv_Len - 1];
			//char b = buf[lv_Len - 2];
			//char c = buf[lv_Len - 3];
			file.Write(buf, len);
		} // for i
	}  // if
	file.Close();



	free1(buf);
//	free1(tbuf);

		//FILE *fp;
		//if (_tfopen_s(&fp, aFile, _T("w")) != 0) return false;
		//CString tStr; // just a pointer
		//for (int i = 0; i < m_arr->GetCount(); i++) {
		//	tStr = m_arr->ElementAt(i);

		//	_ftprintf(fp, _T("%s"), tStr);
		//} // for i
		//fclose(fp);



	return true;
}

bool showStringArrayToFile(CStringArray* m_arr) {
	CString newName = getSystemTempPathSlash() + _T("seiTempFile.txt");
	bool out = saveStringArrayToFile(m_arr, newName);
	showFileInNotePad(newName);
	// showFileInDefaultProg(newName);
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
		TRY{

			if (_fgetts(cstr.GetBuffer(), 255, fp) == NULL) break;
			cstr.ReleaseBuffer();
			if (cstr.GetAt(0) == _T('#')) continue; // we do not need comment lines
			val = (float)_tstof(cstr.Right(6));
			if (fabs(val - vPerturb) < 0.001f) {
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
	for (int i = 0; i < trcAll->GetSize(); i++) {
		str = trcAll->ElementAt(i);
		//	if (str == NULL) continue; // this line causes exception. Why????
		//	int iii = str.Find(strin);
		if (str.Find(strin) == 0) return TRUE;
	}
	return FALSE;
}

BOOL isTrcInRange(CString strin, CStringArray *trcAll) {
	if (trcAll == NULL) return FALSE;
	CString str;
	for (int i = 0; i < trcAll->GetSize(); i++) {
		str = trcAll->ElementAt(i);
		//	if (str == NULL) continue; // this line causes exception. Why????
		//	int iii = str.Find(strin);
		if (str.Find(strin) == 0) return TRUE;
	}
	return FALSE;
}

BOOL isValueDefined(float x, int n, float *xArray) {
	if (xArray == NULL || n < 1) return FALSE;
	for (int i = 0; i<n; i++) {
		if (x == xArray[i]) return TRUE;
	}

	return FALSE;
}




BOOL isFloatEqual(float a, float b) {
	// accurate up to 5 decimal points.
	if (fabs(a - b) <= 0.00001f)
		return TRUE;
	else return FALSE;

}

BOOL isFloatEqual(float a, float b, float res) {
	return fabs(a - b) <= res;
}

BOOL isFloatEqual3(float a, float b) {
	// accurate up to 3 decimal points.
	if (fabs(a - b) <= 0.001f)
		return TRUE;
	else return FALSE;

}


BOOL isFloatEqual(double a, double b) {
	// accurate up to 3 decimal points.
	return isFloatEqual((float)a, (float)b);
}

BOOL isFloatAbsEqual(float a, float b) {
	// accurate up to 3 decimal points.
	if (fabs(fabs(a) - fabs(b)) <= 0.00001f)
		return TRUE;
	else return FALSE;

}

long long myRound(double x)
{
	// NOTE: then int is used as return, and the input is over 30mil or so, the return value is incorect. So I changed to long here
	// Note: if you print the result to a file with %d, it may not print antthing unless you cast it to (int) first.
	if (x > 0.)
		return (long long)(x + 0.5);
	else
		return (long long)(x - 0.5);
}


double myFloat(double x, int precision)
{
	// this will trim the input decimal to only 5 decimal points.
	// otherwise the zoomed coords have too many decimal points for the user to use
	return pround(x, precision);
}

float myFloat(float x, int precision)
{
	// this will trim the input decimal to only 5 decimal points.
	// otherwise the zoomed coords have too many decimal points for the user to use
	//long long a = myRound(x*100000.);
	//double b = a * 0.00001;
	//float c = (float)b;
//	return (float)(myRound(x*100000.)*0.00001);
	return pround(x, precision);

}

bool isFloat1DecOnly(double v, double vmin) {
	// can round to 1 decimal point against 2 decimal point
	// examples: 5.01 is not, 5.001 is. 
	return v >= vmin && myFloat(v, 1) == myFloat(v, 2);
}
//
//float myFloat1(double x) {
//	// round to 1 decimal point
//	//return (float)(myRound(x*10.)*0.1);
//	return (float)pround(x, 1);
//
//}

//float myFloat2(double x) {
//	// round to 2 decimal points
//	//return (float)(myRound(x*100.)*0.01);
//	return (float)pround(x, 2);
//
//}
//
//float myFloat3(double x) {
//	// round to 3 decimal points
////	return (float)(myRound(x*1000.)*0.001f);
//	//return (float)(floorf(x*1000.+0.5)*0.001);
//	return (float)pround(x, 3);
//
//}
//
//float myFloat4(double x) {
//	// round to 3 decimal points
//	//	return (float)(myRound(x*1000.)*0.001f);
//	//return (float)(floorf(x*1000.+0.5)*0.001);
//	return (float)pround(x, 4);
//
//}

//float myFloat5(double x) {
//	return (float)pround(x, 5);
//}

double myFloat8(double x) {
	// round to 8 decimal points
	// float ttt = pround(x, 8);
	return pround(x, 8);

	//long long i = myRound(x*10000000.);
	//float a = (float)(i*0.0000001);
	//return a;
}
//
//double pround(double x, int prec)
//{
//	// -- cut --
//		double power = 1.0;
//		int i;
//
//		if (prec > 0)
//			for (i = 0; i < prec; i++)
//				power *= 10.0;
//		else if (prec < 0)
//			for (i = 0; i < prec; i++)
//				power *= 0.1;
//
//		double y;
//
//		if (x > 0)
//			y = floor(x * power + 0.5);
//		else if (x < 0)
//			y = ceil(x * power - 0.5);
//
//		if (x == -0)
//			y = 0.;
//		else
//			y /= power;
////		y = (y + 0.0000000001) / power;
//
//		// fix up for a strange situ
//	//	float out = (float)y;
//		//int dy = (int)( (out - y)*power * 10. );
//		//if (dy > 0)
//		//	out = (float)(out - dy / power * 0.15);
//		//else {
//		//	int sdfsdfs = 1;
//		//}
//
//
//		return y;
//	// -- cut --
//}


float pround(float x, int prec)
{
	// -- cut --
	float power = 1.0f;
	int i;

	if (prec > 0)
		for (i = 0; i < prec; i++)
			power *= 10.0f;
	else if (prec < 0)
		for (i = 0; i < prec; i++)
			power *= 0.1f;

	float y;

	if (x > 0.f)
		y = (float)floor(x * power + 0.5f);
	else if (x < 0.f)
		y = (float)ceil(x * power - 0.5f);

	if (x == -0.f)
		y = 0.f;
	else
		y = y / power;

	return y;
	// -- cut --
}


double pround(double x, int prec)
{
	// -- cut --
	double power = 1.0;
	int i;

	if (prec > 0)
		for (i = 0; i < prec; i++)
			power *= 10.0;
	else if (prec < 0)
		for (i = 0; i < prec; i++)
			power *= 0.1;

	double y;

	if (x > 0.)
		y = floor(x * power + 0.5);
	else if (x < 0.f)
		y = ceil(x * power - 0.5);

	if (x == -0.)
		y = 0.;
	else
		y = y / power;

	return y;
	// -- cut --
}

//
//float round(double f, double prec)
//{
//	double temp = pow(10., prec);
//	float a =  (float)(floor(f*(1.0 / prec) + 0.5) / (1.0 / prec));
//	return (float)(floor(f*(1.0 / prec) + 0.5) / (1.0 / prec));
//}


/**
   * Returns a "nice" number approximately equal to range Rounds
   * the number if round = true Takes the ceiling if round = false.
   *
   * @param range the data range
   * @param round whether to round the result
   * @return a "nice" number to be used for the data range
   */
double niceNumRound(double range) {
	if (range < 0.) return -niceNumRound(fabs(range));
	if (range < MINAMP) return 0.;  // 0 cannot continue down here

	// the following only works for positive numbers

	double exponent; /** exponent of range */
	double fraction; /** fractional part of range */
	double niceFraction; /** nice, rounded fraction */

	exponent = floor(log10(range));
	//	exponent = countDigits()
	fraction = range / pow(10, exponent);

	if (fraction < 1.5)
		niceFraction = 1;
	else if (fraction < 3)
		niceFraction = 2;
	else if (fraction < 7)
		niceFraction = 5;
	else
		niceFraction = 10;

	return niceFraction * pow(10, exponent);
}

double niceNumMinMaxExp(double range, bool isMaxNeeded) {
	// the following only works for positive numbers
	double exponent; /** exponent of range */
	double fraction; /** fractional part of range */
	double niceFraction; /** nice, rounded fraction */

	exponent = floor(log10(range));
	fraction = range / pow(10, exponent);
	niceFraction = isMaxNeeded ? ceil(fraction) : floor(fraction);

	return niceFraction * pow(10, exponent);
}

double niceNumMinMax(double range, bool isMaxNeeded) {
	if (range < 0.) return -niceNumMinMax(fabs(range), isMaxNeeded);
	if (range < MINAMP) return 0.;  // 0 cannot continue down here
	if (fabs(range) > 100.)
		return 0.1 * niceNumMinMaxExp(range*10., isMaxNeeded);

	return niceNumMinMaxExp(range, isMaxNeeded);
}

// calc the approximate tick interval
//   nDiv: recommended number of ticks
//   amin, amax: bounds of the axis
//   returns the tick interval
float getTickRounded(double amin, double amax, int nDivMin, int nDivMax, double tickPrefered)
{
	double range = amax - amin;
	if (nDivMin < 2) nDivMin = 2;
	if (nDivMax > 50) nDivMax = 50;
	if (tickPrefered <= 0.00001) {
		// invalid tick, we use the middle
		tickPrefered = 2. * range / (nDivMin + nDivMax);
	}
	double out = (float)niceNumRound(tickPrefered);
	if (out < 0.00001f) out = 0.00001f;

	if (out < range / nDivMax) out = range / nDivMax;
	else if (out > range / nDivMin) out = range / nDivMin;
	if (out < 0.00001f) out = 0.00001f;

	out = (float)niceNumRound(out);

	// if tickPrefered==5, we do not want out=0.5
	// butif  tickPrefered==1 or 2, we want out=0.5
	if (tickPrefered == 5. && out < 1.)
		out = 1.;

	return (float)out;
}

// xmin can be 1, which should be rounded to 0, and incremented by a regular tick value
// xmax cannot be rounded, otherwise 899 becomes 1000, which is not good
float getMinRounded(double amin, double amax, int nDiv)
{
	double dx = niceNumMinMax(amax - amin, false);
	//if (nDiv < 2) nDiv = 2;
	// double tickSpacing = niceNumRound(range / (nDiv - 1));
	return (float)(floor(amin / dx) * dx);
}

float getMaxRounded(double amin, double amax, int nDiv)
{
	double dx = niceNumMinMax(amax - amin, true);
	//if (nDiv < 2) nDiv = 2;
	// double tickSpacing = niceNumRound(range / (nDiv - 1));
	return (float)(ceil(amax / dx) * dx);
}


// it can be very tedious to get a nice lable for the Y-axis, and X-axis
CString getNiceLabelFromValue(double y, double yTickNice)
{
	CString tStr;
	if (yTickNice < 0.1) {
		if (myFloat(y, 2) - y < 0.001)
			tStr.Format(_T("%4.2f"), (float)y);
		else
			tStr.Format(_T("%5.3f"), (float)y);
	}
	else if (yTickNice < 1.) {
		if (myFloat(y, 1) - y < 0.01)
			tStr.Format(_T("%3.1f"), (float)y);
		else
			tStr.Format(_T("%4.2f"), (float)y);
	}
	else {
		if (myRound(y) - y < 0.1)
			tStr.Format(_T("%i"), (int)y);  // casting it to (int)  is a must if you write it to integer string!
		else
			tStr.Format(_T("%3.1f"), (float)y);
	}
	return tStr;
}










PROCESS_INFORMATION myUnicodeProcess(CString exeNamePath, CString workPath, bool isPause)
{
	PROCESS_INFORMATION proc_i;  // info returned from CreateProcess
	bool isSuc = false;

	
	if (!isPause && exeNamePath.Find(_T("rayinvr.exe")) >= 0) {
		isSuc = myUnicodeProcessCache(&proc_i, exeNamePath, workPath);
		if (isSuc) return proc_i;
	}

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
//	static CString exeOld(exeNamePath);
//	static CString pathOld(workPath);
//	static bool pauseOld = isPause;

	CString newName = getSystemTempPathSlash() + _T("seiBatch.bat");

	// it is much slower to write a file
	CFile cfBatchFile(newName, CFile::modeCreate | CFile::modeWrite);
		// cfBatchFile.Write( CT2CA(exeNamePath, CP_UTF8), exeNamePath.GetLength() );
	cfBatchFile.Write("chcp 65001\r\n", 12);
	CT2CA aStrUtf8(exeNamePath, CP_UTF8);
	cfBatchFile.Write(aStrUtf8, (UINT)strlen(aStrUtf8));
	if (isPause)
		cfBatchFile.Write("\r\n pause", 8);
	cfBatchFile.Close();

	newName = _T("/c ") + newName;

	CString str;
	GetWindowsDirectory(str.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
	str.ReleaseBuffer();
	str += _T("\\system32\\cmd.exe");

	if (isDirectoryExist(workPath))
		SetCurrentDirectory(workPath);

	//STARTF_USESIZE;
	STARTUPINFO sui;         // Process startup info
	// Most sui members will be 0
	ZeroMemory((PVOID)&sui, sizeof(sui));
	sui.cb = sizeof(sui);
	sui.lpReserved = NULL;
	sui.dwFlags = 0;
	sui.cbReserved2 = 0;
	sui.lpReserved2 = NULL;
	sui.lpDesktop = NULL;
	sui.lpTitle = NULL;
	sui.dwX = 0;
	sui.dwY = 0;
	sui.dwXSize = 60;
	sui.dwYSize = 50;
	sui.dwXCountChars = 0;
	sui.dwYCountChars = 0;
	sui.dwFillAttribute = 0;
	sui.dwFlags = 0;
	sui.wShowWindow = 1;
	sui.hStdInput = NULL;
	sui.hStdOutput = NULL;
	sui.hStdError = NULL;

	BOOL Suc = CreateProcess(str,   // image name
		newName.GetBuffer(_MAX_PATH), // command line
		NULL,   // process security attributes
		NULL,   // thread security attributes
		FALSE,   // inherit handles
		(isPause ? CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE : CREATE_NO_WINDOW) | NORMAL_PRIORITY_CLASS,
		NULL,   // environment block
		NULL,   // current directory
		&sui,   // STARTUPINFO
		&proc_i);   // PROCESS_INFORMATION
	newName.ReleaseBuffer();


	return proc_i;

}


bool myUnicodeProcessCache(PROCESS_INFORMATION *proc_i, CString exeNamePath, CString workPath)
{
	if (isDirectoryExist(workPath))
		SetCurrentDirectory(workPath);
	static FILE_CACHE dosFileCache;
	if (dosFileCache.fName != exeNamePath || dosFileCache.bytesLoaded == 0) {
		FILE *fp;
		if (_tfopen_s(&fp, exeNamePath, _T("rb")) != 0)
			return false;

		// obtain file size:
		fseek(fp, 0, SEEK_END);
		long lSize = ftell(fp);
		rewind(fp);
		if (lSize > dosFileCache.maxBytes) {
			fclose(fp);
			return false;
		}

		// copy the file into the buffer:
		size_t result = fread(&dosFileCache.bytes[0], 1, lSize, fp);
		fclose(fp);

		// now success
		dosFileCache.fName = exeNamePath;
		dosFileCache.bytesLoaded = lSize;
	}

	BYTE *pByte = (BYTE *)alloc1(dosFileCache.bytesLoaded, 1);
	memcpy(pByte, &dosFileCache.bytes[0], dosFileCache.bytesLoaded);
	bool isSuc = RunFromMemory(proc_i, pByte, dosFileCache.fName); //Now start the exe in Memory..
	free1(pByte);
	return isSuc;
}

//PROCESS_INFORMATION myUnicodeProcess(CString exeNamePath, CString workPath)
//{
//	return myUnicodeProcess(exeNamePath, workPath, true);
//}

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
	ZeroMemory((PVOID)&sui, sizeof(sui));
	sui.cb = sizeof(sui);
	BOOL Suc = CreateProcess(NULL,   // image name
		exeNamePath.GetBuffer(_MAX_PATH), // command line
		NULL,   // process security attributes
		NULL,   // thread security attributes
		FALSE,   // inherit handles
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE | IDLE_PRIORITY_CLASS,
		NULL,   // environment block
		NULL,   // current directory
		&sui,   // STARTUPINFO
		&proc_i);   // PROCESS_INFORMATION
	exeNamePath.ReleaseBuffer();
	return Suc;
}

// http://stackoverflow.com/questions/9428456/how-to-terminate-a-process-created-by-createprocess

bool RunFromMemory(PROCESS_INFORMATION *proc_i, BYTE* pByte, CString pPath)
{
	IMAGE_DOS_HEADER IDH;
	memcpy(&IDH, pByte, sizeof(IDH));
	IMAGE_NT_HEADERS INH;
	memcpy(&INH, (void*)(pByte + IDH.e_lfanew), sizeof(INH));

	ULONG dwImageSize = INH.OptionalHeader.SizeOfImage;
	BYTE* pMemory = (BYTE*)alloc1(dwImageSize, 1);
	memset(pMemory, 0, dwImageSize);

	ULONG_PTR firstSection = (ULONG_PTR)((pByte + IDH.e_lfanew) + sizeof(IMAGE_NT_HEADERS));
	ULONG jmpSize = 0;
	if ((INH.OptionalHeader.SizeOfHeaders % INH.OptionalHeader.SectionAlignment) == 0) {
		jmpSize = INH.OptionalHeader.SizeOfHeaders;
	}
	else {
		jmpSize = INH.OptionalHeader.SizeOfHeaders / INH.OptionalHeader.SectionAlignment;
		jmpSize += 1;
		jmpSize *= INH.OptionalHeader.SectionAlignment;
	}

	BYTE* pFile = pMemory;
	memcpy(pFile, pByte, INH.OptionalHeader.SizeOfHeaders);
	pFile = (BYTE*)(pFile + jmpSize);

	IMAGE_SECTION_HEADER Sections[1000];
	memcpy(Sections, (char*)(firstSection), sizeof(IMAGE_SECTION_HEADER)*INH.FileHeader.NumberOfSections);
	for (ULONG dwSectionCount = 0; dwSectionCount < INH.FileHeader.NumberOfSections; dwSectionCount++) {
		jmpSize = 0;
		ULONG dwSectionSize = Sections[dwSectionCount].SizeOfRawData;
		memcpy(pFile, (char*)(pByte + Sections[dwSectionCount].PointerToRawData), dwSectionSize);

		if ((Sections[dwSectionCount].Misc.VirtualSize % INH.OptionalHeader.SectionAlignment) == 0) {
			jmpSize = Sections[dwSectionCount].Misc.VirtualSize;
		}
		else {
			jmpSize = Sections[dwSectionCount].Misc.VirtualSize / INH.OptionalHeader.SectionAlignment;
			jmpSize += 1;
			jmpSize *= INH.OptionalHeader.SectionAlignment;
		}
		pFile = (BYTE*)(pFile + jmpSize);
	}


	STARTUPINFO sui;
	//memset(&peStartUpInformation, 0, sizeof(STARTUPINFO));
	ZeroMemory((PVOID)&sui, sizeof(sui));
	sui.cb = sizeof(sui);
//	SECURITY_ATTRIBUTES secAttrib;
	DWORD previousProtection = 0;
	SIZE_T dwWritten = 0;

	bool isSuc = false;
	if (CreateProcess(NULL, 
		pPath.GetBuffer(_MAX_PATH),
		NULL,
		NULL, 
		false,
		CREATE_SUSPENDED | CREATE_NO_WINDOW,
		NULL, 
		NULL, 
		&sui,
		proc_i)) {

#ifdef _X86_
		CONTEXT pContext;
		memset(&pContext, 0, sizeof(CONTEXT));
		pContext.ContextFlags = CONTEXT_FULL;
		GetThreadContext(proc_i->hThread, &pContext);
		VirtualProtectEx(proc_i->hProcess,
			(void*)((DWORD)INH.OptionalHeader.ImageBase), 
			dwImageSize, PAGE_EXECUTE_READWRITE, 
			&previousProtection);
		WriteProcessMemory(proc_i->hProcess,
			(void*)((DWORD)INH.OptionalHeader.ImageBase), 
			pMemory, 
			dwImageSize, 
			&dwWritten);
		WriteProcessMemory(proc_i->hProcess,
			(void*)((DWORD)pContext.Ebx + 8), 
			&INH.OptionalHeader.ImageBase, 4, 
			&dwWritten);
		pContext.Eax = INH.OptionalHeader.ImageBase + INH.OptionalHeader.AddressOfEntryPoint;
		SetThreadContext(proc_i->hThread, &pContext);
		VirtualProtectEx(proc_i->hProcess, (void*)((DWORD)INH.OptionalHeader.ImageBase),
			dwImageSize, previousProtection, 0);
		ResumeThread(proc_i->hThread);
#endif
#ifdef _WIN64
		// Wow64SuspendThread(proc_i.hThread);
		WOW64_CONTEXT pContext;
		memset(&pContext, 0, sizeof(WOW64_CONTEXT));
		pContext.ContextFlags = WOW64_CONTEXT_FULL;
		Wow64GetThreadContext(proc_i->hThread, &pContext);
		VirtualProtectEx(proc_i->hProcess,
			(void*)(INH.OptionalHeader.ImageBase),
			dwImageSize, PAGE_EXECUTE_READWRITE,
			&previousProtection);
		WriteProcessMemory(proc_i->hProcess,
			(void*)(INH.OptionalHeader.ImageBase),
			pMemory,
			dwImageSize,
			&dwWritten);

		//WriteProcessMemory(proc_i.hProcess,
		//	(void*)((DWORD)pContext.Ebx + 8),
		//	&INH.OptionalHeader.ImageBase, 
		//	4, 
		//	&dwWritten);
		//int aa = sizeof(ULONGLONG);  // == 8
		//int bb = sizeof(DWORD);  // ==4
		//pContext.Eax = (DWORD)INH.OptionalHeader.ImageBase + INH.OptionalHeader.AddressOfEntryPoint;
		 
		Wow64SetThreadContext(proc_i->hThread, &pContext);
		VirtualProtectEx(proc_i->hProcess,
			(void*)(INH.OptionalHeader.ImageBase),
			dwImageSize, 
			previousProtection, 
			0);
		ResumeThread(proc_i->hThread);
#endif
		isSuc = true;
	}
	pPath.ReleaseBuffer();
	free1(pMemory);
	return isSuc;
}

//int main(int argc, char* argv[])
//{
//	HGLOBAL hResData;
//	HRSRC   hResInfo;
//	void	*pvRes;
//	DWORD dwSize;
//	char* lpMemory;
//	HMODULE hModule = GetModuleHandle(NULL);
//
//	if (((hResInfo = FindResource(hModule, MAKEINTRESOURCE(IDD_EXE1), "EXE")) != NULL) && ((hResData = LoadResource(hModule, hResInfo)) != NULL) && ((pvRes = LockResource(hResData)) != NULL))
//	{
//		dwSize = SizeofResource(hModule, hResInfo);
//		lpMemory = (char*)malloc(dwSize);
//		memset(lpMemory, 0, dwSize);
//		memcpy(lpMemory, pvRes, dwSize);
//		RunFromMemory(lpMemory, argv[0]);
//	}
//}

//int main(int argc, char * argv[])
//{
//	FILE *fp;
//	if (_tfopen_s(&fp, _T("MyFile.exe"), _T("rb")) != 0) return 1;
//
//	BYTE *pByte;
//
//	// obtain file size:
//	fseek(fp, 0, SEEK_END);
//	long lSize = ftell(fp);
//	rewind(fp);
//
//	// allocate memory to contain the whole file:
//	pByte = (BYTE *)alloc1(lSize, sizeof(BYTE));
//	if (!pByte) return 2;
//
//	// copy the file into the buffer:
//	size_t result = fread(pByte, 1, lSize, fp);
//	if (result != lSize) exit(3);
//	fclose(fp);
//
//	// the whole file is now loaded in the memory buffer.
//	PROCESS_INFORMATION peProcessInformation =
//		RunFromMemory(pByte, argv[0]); //Now start the exe in Memory..
//
//	free1(pByte);
//	return 0;
//}

void showFileInNotePad(CString name)
{
	if (!isFileExistNotDirectory(name)) return;  // CFileStatus fails to detect non-existing file status!!!
	CFileStatus status;
	BOOL initFileStatus = CFile::GetStatus(name, status); // static function
	if (status.m_size > 10 * MIL) {
		static bool isPrompt = true;
		if (isPrompt) {
			AfxMessageBox(name + _T(" is more than 10MB, and may be too big for Notepad. Now trying default text editor."), MB_ICONQUESTION);
			isPrompt = false; // we will only prompt this message once
		}
		showFileInDefaultProg(name); // this way, the user gets a chance to switch to another favorate editor of his.
		return;
	}


	WCHAR lpszSystemInfo[MAX_PATH];   /* buffer for concat'd. str. */
	int nSize = GetSystemDirectoryW(lpszSystemInfo, MAX_PATH);
	CString str(lpszSystemInfo);
	str += _T("\\notepad.exe");
	//	str.Format(_T("%c:\\Program Files\\Windows NT\\Accessories\\notepad.exe"), lpszSystemInfo[0]);
	if (!isFileExistNotDirectory(str)) str = _T("notepad.exe");

	if (!myProcessSimple(str + SPACE + name))
		AfxMessageBox(_T("Not able to load with ") + str, MB_ICONQUESTION);

}

#include "AutoUpdater.h"
BOOL myShowHelp(CString strFile) {
	CString tStr = _T("http://seiswide.hopto.org");

	CAutoUpdater au;
	if (au.InternetOkay() && au.GetUrlContents(tStr).GetLength() > 10) {
		ShellExecute(
			0,
			_T("open"),
			tStr,
			0,
			0,
			SW_SHOWNORMAL);
		return TRUE;
	}



	CString seiPath;
	int nPathLength = GetModuleFileName(NULL, seiPath.GetBuffer(_MAX_PATH), _MAX_PATH);
	seiPath.ReleaseBuffer();
	if (nPathLength < 0) return FALSE;

	CString aPath = seiPath.Left(seiPath.ReverseFind('\\'));
	CString aPath2 = aPath.Left(aPath.ReverseFind('\\'));

	bool isDebugging = seiPath.Find(_T("Debug")) > 0;

	CString docPath = (isDebugging) ?
		aPath2 + _T("\\..\\docs\\SeisWide\\docs\\")
		:
		aPath + _T("\\docs\\");

	if (!isFileExistNotDirectory(docPath + strFile)) {
		//seiPath.Format(_T("Could not find the help file: \n%s"), docPath);
		//AfxMessageBox(seiPath);
		strFile = _T("index,html");
	}
	else if (GetFileExtension(strFile) == _T("txt")) {
		showFileInNotePad(docPath + strFile);
	}
	else {
		ShellExecute(
			0,
			_T("open"),
			docPath + strFile,
			0,
			0,
			SW_SHOWNORMAL);
	}

	return TRUE;
}

void showFileInDefaultProg(CString fPathName)
{
	ShellExecute(
		0,
		_T("open"),
		fPathName,
		0,
		0,
		SW_SHOWNORMAL);
}

bool saveStringToFile(CString str, CString newName)
{
	if (str.GetLength() == 0) return false;
	FILE *stream;
	if (_tfopen_s(&stream, newName, _T("w")) != 0) {
		return false;
	}
	_ftprintf(stream, _T("%s"), (LPCTSTR)str);
	fclose(stream);
	return true;
}


CString saveStringToTempFile(CString str)
{
	CString newName = getSystemTempPathSlash() + _T("aBatch.tmptxt");
	if (saveStringToFile(str, newName))
		return newName;
	else
		return BLANK;
}


CString saveStringToRegiFile(CString str)
{
	if (str.GetLength() == 0) return BLANK;
	CString newName = regiTempFileName();

	if (saveStringToFile(str, newName))
		return newName;
	else
		return BLANK;
}

CString regiTempFileName() {
	return getSystemTempPathSlash() + _T("regiBatch.tmptxt");
}


void showStringInFile(CString str)
{
	showFileInNotePad(saveStringToTempFile(str));
}


void showStringInFile(CString str, CString f)
{
	if (str.GetLength() == 0) return;
	FILE *stream;
	if (_tfopen_s(&stream, f, _T("w")) != 0) {
		return;
	}
	_ftprintf(stream, _T("%s"), (LPCTSTR)str);
	fclose(stream);
	showFileInNotePad(f);
}

// comment will be put in front of the showing
void showArrayInFile(float x[], int n, CString comment)
{
	if (x == NULL || n <= 0) return;
	CString newName = getSystemTempPathSlash() + _T("aBatch.tmptxt");

	FILE *stream;
	if (_tfopen_s(&stream, newName, _T("w")) != 0) {
		return;
	}

	if (comment.GetLength() > 0) {
		_ftprintf(stream, _T("%s\r\n"), (LPCTSTR)comment);
	}

	for (int i = 0; i < n; i++) {
		_ftprintf(stream, _T("%5i  %12.6g\r\n"), i+1, x[i]);
	}
	fclose(stream);

	showFileInNotePad(newName);
}

bool showArrayInFile(double *y, int n, CString title, double dx)
{
	if (y == NULL || n <= 0) return false;
	CString newName = getSystemTempPathSlash() + _T("aBatch.tmptxt");

	FILE *stream;
	if (_tfopen_s(&stream, newName, _T("w")) != 0) {
		return false;
	}
	if (!title.IsEmpty())
		_ftprintf(stream, _T("%s\n"), (LPCTSTR)title);
	if (dx == 0.) dx = 1.;
	for (int i = 0; i < n; i++) {
		_ftprintf(stream, _T("%4.3f  %12.6f\n"), i*dx, (float)y[i]);
	}
	fclose(stream);

	showFileInNotePad(newName);
	return true;
}

bool showArrayInFile(float *y, int n, CString title, double dx)
{
	if (y == NULL || n <= 0) return false;
	CString newName = getSystemTempPathSlash() + _T("aBatch.tmptxt");

	FILE *stream;
	if (_tfopen_s(&stream, newName, _T("w")) != 0) {
		return false;
	}
	if (!title.IsEmpty())
		_ftprintf(stream, _T("%s\n"), (LPCTSTR)title);
	if (dx == 0.) dx = 1.;
	for (int i = 0; i < n; i++) {
		_ftprintf(stream, _T("%4.3f  %12.6f\n"), i*dx, (float)y[i]);
	}
	fclose(stream);

	showFileInNotePad(newName);
	return true;
}

void showArrayInFile(CString str[], int n)
{
	if (str == NULL) return;
	CString newName = getSystemTempPathSlash() + _T("aBatch.tmptxt");

	FILE *stream;
	if (_tfopen_s(&stream, newName, _T("w")) != 0) {
		return;
	}
	for (int i = 0; i < n; i++) {
		_ftprintf(stream, _T("%s\r\n"), (LPCTSTR)str[i]);
	}
	fclose(stream);

	showFileInNotePad(newName);
}


/*=============================================================
   Procedure to reverse high and low bytes.
   */
unsigned short revnib(unsigned short value)
{
	unsigned short m2 = 255, m7 = 65280;
	return ((value >> 8) & m2) ^ ((value << 8) & m7);
}

BOOL isFileReadOnly(CString f) {

	CFileStatus status;
	BOOL initFileStatus = CFile::GetStatus(f, status); // static function

	if (status.m_attribute & 1)
		return true;

	return false;
}

/* 
* 
# Correction to v2rho computation. This function should be called before v2rho().
# vRefMin vRefMax DenRef Layer(1-based) xMin  xMax
# 6.1     6.2     2.6    6              -150  10.
# Note: 1. This special conversion formula shall be applied only within specified layer in between [xMin, xMax]. 
#		2. If xMin>xMax, it will be applied to any x-values.
#		3. if layer<1, this rule applies to all layers
*/
double v2rho_corr(double v, double x, int layer, CStringArray* arr_corr)
{
	if (!arr_corr) return -1.;

	float refvmin, refvmax, refden, refxmin, refxmax; // these must be float, otherwise reading has problems.
	int reflayer;
	int len = (int)arr_corr->GetCount();


	if (len < 1) return -1.;

	for (int i = 0; i < len; i++) {
		if (_stscanf_s(arr_corr->ElementAt(i).Trim(), _T("%f%f%f%d%f%f"), &refvmin, &refvmax, &refden, &reflayer, &refxmin, &refxmax) == 0) continue;
		if (layer>0 && layer != reflayer) continue;
		if (refvmin > refvmax) refvmax = refvmin;
		if (v < refvmin || v > refvmax) continue;
		if ((refxmax - refxmin) > 0.001f && (x < refxmin || x > refxmax)) continue;

		// now we found it
		return refden;
	}

	// now no result is found
	return -1.;
}

double v2rho(double v, double velRef[], double denRef[], int len)
{
	//a routine to calculate the density from vel using formula of Nafe.
	// double c1=-.6997, c2=2.2302, c3=-.598, c4=.07036, c5=-.0028311, dens;
	// if (v < 1.6) return 1.03;
	//	return (-0.00283*v*v*v*v + 0.0704*v*v*v - 0.598*v*v + 2.23*v - 0.7);
	//	dens = (c5*v*v*v*v + c4*v*v*v + c3*v*v + c2*v + c1);
	//	if (dens > 3.3) dens = 3.3;
	//	else if (dens < 1.4) dens = 1.4;

	if (len > 14) len = 14;
	else if (len < 9) len = 9;

	double dens = 0.;
	//if (v <= velRef[0]) dens = denRef[0];
	//else if (v <= velRef[1]) dens = denRef[0] + (denRef[1] - denRef[0]) * (v - velRef[0]) / (velRef[1] - velRef[0]);
	//else if (v <= velRef[2]) dens = denRef[1] + (denRef[2] - denRef[1]) * (v - velRef[1]) / (velRef[2] - velRef[1]);
	//else if (v <= velRef[3]) dens = denRef[2] + (denRef[3] - denRef[2]) * (v - velRef[2]) / (velRef[3] - velRef[2]);
	//else if (v <= velRef[4]) dens = denRef[3] + (denRef[4] - denRef[3]) * (v - velRef[3]) / (velRef[4] - velRef[3]);
	//else if (v <= velRef[5]) dens = denRef[4] + (denRef[5] - denRef[4]) * (v - velRef[4]) / (velRef[5] - velRef[4]);
	//else if (v <= velRef[6]) dens = denRef[5] + (denRef[6] - denRef[5]) * (v - velRef[5]) / (velRef[6] - velRef[5]);
	//else if (v <= velRef[7]) dens = denRef[6] + (denRef[7] - denRef[6]) * (v - velRef[6]) / (velRef[7] - velRef[6]);
	//else if (v <= velRef[8]) dens = denRef[7] + (denRef[8] - denRef[7]) * (v - velRef[7]) / (velRef[8] - velRef[7]);
	//else {
	//	if (len == 9)
	//		dens = denRef[8];
	//	else {

	for (int j = 0; j < len; j++) {
		if (v <= velRef[j]) {
			if (j == 0) 
				dens = denRef[0];
			else {
				register double a = velRef[j] - velRef[j - 1];
				if (fabs(a) < OMIL) 
					dens = denRef[j - 1];
				else 
					dens = denRef[j - 1] + (denRef[j] - denRef[j - 1]) * (v - velRef[j - 1]) / a;
			}
			break;
		}
	}
	if (dens < 0.0001) 
		dens = denRef[len - 1];


	//	}
	//}


	return dens;
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
	if (v <= velRef[0]) dens = denRef[0];
	else if (v <= velRef[1]) dens = denRef[0] + (denRef[1] - denRef[0])*(v - velRef[0]) / (velRef[1] - velRef[0]);
	else if (v <= velRef[2]) dens = denRef[1] + (denRef[2] - denRef[1])*(v - velRef[1]) / (velRef[2] - velRef[1]);
	else if (v <= velRef[3]) dens = denRef[2] + (denRef[3] - denRef[2])*(v - velRef[2]) / (velRef[3] - velRef[2]);
	else if (v <= velRef[4]) dens = denRef[3] + (denRef[4] - denRef[3])*(v - velRef[3]) / (velRef[4] - velRef[3]);
	else if (v <= velRef[5]) dens = denRef[4] + (denRef[5] - denRef[4])*(v - velRef[4]) / (velRef[5] - velRef[4]);
	else if (v <= velRef[6]) dens = denRef[5] + (denRef[6] - denRef[5])*(v - velRef[5]) / (velRef[6] - velRef[5]);
	else if (v <= velRef[7]) dens = denRef[6] + (denRef[7] - denRef[6])*(v - velRef[6]) / (velRef[7] - velRef[6]);
	else if (v <= velRef[8]) dens = denRef[7] + (denRef[8] - denRef[7])*(v - velRef[7]) / (velRef[8] - velRef[7]);
	else dens = denRef[8];

	return dens;
}




bool isAsciiPure(unsigned char ascHead[], int len) {
	// we only test the first 80 chars. If they are ok, then it is pure. After 80 chars, there may be other chars that nobody cares. 
	// And they are indeed strange chars that not pure ascii, because the operaters/programs were old to handle them
	if (len > 80) len = 80;
	int count = 0;
	for (int i = 0; i < len; i++) {
		//  ascii 254 exists as what?
		// answer: we have to consider some older programs producing strange codes that no one cares.
		// 254 is solid rectangle and 255 is a blank, both will not display well in
		// pure ascii old screens, but ok to exist. 204, 144, 132 are also ok to exist.
		if ((ascHead[i] > 64 && ascHead[i] < 91) || (ascHead[i] > 96 && ascHead[i] < 123)) {
			count++;
		}
	}
	return count > len/10;
}
void autoAsc2ebc(unsigned char ascHead[], int len) {
	// this converts from ASCII format to IBM's DBCDIC format.
	if (!isAsciiPure(ascHead, 80)) {
		ebc2asc(ascHead, 3200);
		if (!isAsciiPure(ascHead, 80)) {
			asc2ebc(ascHead, 3200); // wrong conversion, so undo

			// now try the reverse direction
			asc2ebc(ascHead, 3200);
			if (!isAsciiPure(ascHead, 80)) {
				// still not good, undo
				ebc2asc(ascHead, 3200);
			}
		}
	}
}

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
			i++; j--;
		}
		//and continue the scan until the pointers cross:
	} while (i <= j);
}


//void getMedian3(float a[], int n, int &k)
int getMedian3(float a[], int n)
{
	int L = 0;
	int R = n - 1;
	int k = n / 2;
	int i;
	int j;
	while (L < R)
	{
		register float x = a[k];
		i = L; j = R;
		splitMedia3(a, n, x, i, j);
		if (j < k)  L = i;
		if (k < i)  R = j;
	}
	return k;
}


double getRmsValue(float *x, int n) {
	if (n < 1) return 0.f;
	int i;
	double fout = 0.;
	for (i = 0; i < n; i++) {
		fout += x[i] * x[i];
	}
	fout /= (double)n;
	return sqrt(fout);
}



double getAvgInterval(double x[], int nx) {
	int n, i;
	float *ranges = ealloc1float(nx);
	for (i = 0; i<nx - 1; i++)
		ranges[i] = (float)(x[i + 1] - x[i]);
	n = getMedian3(ranges, nx - 1);	//This is the average trace interval in km.
	double trcInterval = ranges[n];
	free1float(ranges);

	return trcInterval;
}

float getAvgInterval(float x[], int nx) {
	int n, i;
	float *ranges = ealloc1float(nx);
	for (i = 0; i<nx - 1; i++)
		ranges[i] = (float)(x[i + 1] - x[i]);
	n = getMedian3(ranges, nx - 1);	//This is the average trace interval in km.
	float out = ranges[n];
	free1float(ranges);

	return out;
}

double getMedianAbsolute(float x[], long n)
{
	// Note: we cannot overwrite the original array dx[]

	float *x1 = (float *)ealloc1float(n);
	if (!x1) return 1.0;  // it fails to allocate memory


	double amp;
	int j;
	int count = 0;
	for (j = 0; j < n; j++) {
		amp = fabs(x[j]);
		if (amp > MINAMP) {
			x1[count] = (float)amp;
			count++;
		}
	}
	sort1(x1, count);
	amp = x1[count / 2];
	free1float(x1);
	return amp;


	// the following are not needed
	j = getMedian3(&x1[0], n);
	double a;
	a = x1[j];

	// now the problem: if we have lots of zero points at the beginning and end of x[], it will give incorrect median. 
	// These zeros can be numerous number for modeled data or for trimmed data!
	//a = getAvgValue(x1, n);  
	// do not use average value since it will be incorrect a single value of MIL will destroy the results!
	float aMinValue = (float)(a * 0.001);  // we treat anything below average/1000 will not be considered

	// we will need to calculate j1 and j2 
	int j1 = 0;
	for (j = 0; j<n; j++) {
		if (x1[j] > aMinValue) {
			j1 = j;
			break;
		}
	} // for j

	int j2 = n - 1;
	for (j = n - 1; j >= 0; j--) {
		if (x1[j] > aMinValue) {
			j2 = j;
			break;
		}
	} // for j



	double out = 0.0;
	if (j2 <= j1) out = a;
	else if (j1 == 0 && j2 == n - 1) out = a;
	else {
		// now we need to re-calculate 
		n = j2 - j1 + 1;
		int k = getMedian3(&x1[j1], n);
		out = x1[k + j1];
	}

	free1float(x1);
	return out;

}

double getMedianInt(int *x, long n)
{
	// Note: we cannot overwrite the original array dx[]
	float *x1 = (float *)ealloc1float(n);
	if (!x1) return 1.0;  // it fails to allocate memory

	int j;
	for (j = 0; j < n; j++)
	{
		x1[j] = (float)x[j];
	}
	//double a = getMedian2(&x1[0], n);
	j = getMedian3(&x1[0], n);
	double a = x1[j];
	free1float(x1);
	return a;

}


// This median value calc has prob: if we have 80 traces with x[] very close, and 20 traces sparse apart, we want to ignore the counting of the 80 traces
float getMedianValue(float x[], long n) {
	if (n < 1) return 0.f;
	else if (n < 2) return x[0];

	float *x2 = (float *)ealloc1float(n);
	if (!x2) return 1.0;  // it fails to allocate memory
	memcpy(x2, x, n*sizeof(float));
	sort1(x2, n);

	// Suppose we have many big data gaps, and the rest of data is normal. If we take average, we will have a bad estimate.
	// but if we take median, when we have most of the traces in the same place, it will not be right as well.
	int n2 = n / 2;
	float a = x2[n2];
	//float a = n>5 ?
	//	(x2[2] + x2[n - 3]) * 0.5f
	//	:
	//	(x2[0] + x2[n - 1]) * 0.5f;

	free1float(x2);
	return a;
}

double getMedianValue(double x[], long n) {
	float *x2 = (float *)ealloc1float(n);
	if (!x2 || n==0) return 1.0;  // it fails to allocate memory
	for (int i = 0; i < n; i++) {
		x2[i] = (float)x[i];
	}
	double a = getMedianValue(x2, n);
	free1float(x2);
	return a;
}

//float getMedianValueRegular(float x[], long n) {
//	if (n < 1) return 0.f;
//	else if (n < 2) return x[0];
//
//	int j = getMedian3(x, n);
//	if (j<0 || j>n - 1) return 0.f; // invalid index
//
//	return x[j];
//}

//double getMedianValueRegular(double *x, long n) {
//	float *x1 = (float *)ealloc1float(n);
//	if (!x1) return 1.0;  // it fails to allocate memory
//	int j;
//	for (j = 0; j < n; j++) {
//		x1[j] = (float)x[j];
//	}
//	j = getMedian3(&x1[0], n);
//	double a = x1[j];
//	free1float(x1);
//	return a;
//}




float getMaxValue(float x[], long n, float aThreshold)
{
	int j;
	float amax = -MIL*1000.0;
	for (j = 0; j < n; j++) {
		if (x[j]<aThreshold && x[j] > amax) amax = x[j];
	}
	return amax;
}

float getMaxValue(float x[], long n)
{
	int j;
	double amax = -MIL*1000.0;
	for (j = 0; j<n; j++) {
		if (x[j] > amax) amax = x[j];
	}
	return (float)amax;
}

double getMaxValue(double x[], long n)
{
	int j;
	double amax = -MIL*1000.0;
	for (j = 0; j < n; j++) {
		if (!_isnan(x[j]) && x[j] > amax) amax = x[j];
	}
	return amax;
}

int getMaxIndex(float x[], long n)
{
	// find the index that corresponds to the max value
	int indx = 0;
	for (int j = 1; j<n; j++) {
		if (x[j] > x[indx]) indx = j;
	}
	return indx;
}

int getMinIndex(float x[], long n)
{
	// find the index that corresponds to the min value
	int indx = 0;
	for (int j = 1; j < n; j++)
		if (x[j] < x[indx]) indx = j;

	return indx;
}

int getMinIndex(int x[], long n)
{
	// find the index that corresponds to the min value
	int indx = 0;
	for (int j = 1; j < n; j++)
		if (x[j] < x[indx]) indx = j;

	return indx;
}


float getMaxValueAbsolute(float x[], long n)
{
	int j;
	double amax = -MIL*1000.0;
	//	double ampLimit = MIL;
	double ampLimit = getMedianAbsolute(&x[0], n) * 1000.;
	// this ampLimit is enforced since when calculating max amp to plot,
	// invalid numbers make a huge difference.
	for (j = 0; j < n; j++) {
		if (_isnan(x[j])) continue;
		if (fabs(x[j]) > ampLimit) continue;
		if (fabs(x[j]) > amax) amax = (float)fabs(x[j]);
	}
	return (float)amax;
}

double getMaxValueAbsolute(double x[], long n)
{
	int j;
	double amax = -MIL*1000.0;
	//	double ampLimit = getMedianAbsolute(&x[0], n) * 1000.;
	for (j = 0; j < n; j++) {
		if (_isnan(x[j])) continue;
		//		if ( fabs(x[j]) > ampLimit) continue;
		if (fabs(x[j]) > amax) amax = fabs(x[j]);
	}
	return amax;
}

int getMinXValueCPoint(CPoint p[], long n)
{
	LONGLONG ampMax = (int)MIL * 1000;
	for (int i = 0; i<n; i++) {
		if (ampMax > p[i].x) ampMax = p[i].x;
	}
	return (int)ampMax;
}

int getMaxXValueCPoint(CPoint p[], long n)
{
	int	ampLow = 0;
	for (int i = 0; i < n; i++) {
		if (ampLow < p[i].x) ampLow = p[i].x;
	}
	return ampLow;
}

// xCutAbs is needed since segy files may have invalid traces that have huge values
float getMinValue(float x[], long n, float xCutAbs)
{
	int j;
	float amin = MIL*1000.0;
	for (j = 0; j < n; j++) {
		if (isnan(x[j])) continue;
		if (fabs(x[j]) < xCutAbs && x[j] < amin) 
			amin = x[j];
	}
	return amin;
}


// xCutAbs is needed since segy files may have invalid traces that have huge values
double getMinValue(double x[], long n, double xCutAbs)
{
	int j;
	double amin = MIL*1000.0;
	for (j = 0; j < n; j++) {
		if (isnan(x[j])) continue;
		if (fabs(x[j]) < xCutAbs && x[j] < amin)
			amin = x[j];
	}
	return amin;
}

int getNearestTraceLeft(int x[], bool isUseArr[], int n, int px)
{
	// this routine takes care of irregular trace spacing
	const int ierr = -9999999;
	int pxOut = ierr;
	for (int j = 0; j<n; j++) {
		if (!isUseArr[j]) continue;
		if (x[j] >= px) continue;

		if (x[j] > pxOut) pxOut = x[j];
	}
	if (pxOut == ierr) pxOut = px - 10; // when error, we just need a reasonable any number

	return pxOut;
}

int getNearestTraceRight(int x[], bool isUseArr[], int n, int px)
{
	// this routine takes care of irregular trace spacing
	const int ierr = 9999999;
	int pxOut = ierr;
	for (int j = 0; j < n; j++) {
		if (!isUseArr[j]) continue;
		if (x[j] <= px) continue;

		if (x[j] < pxOut) pxOut = x[j];
	}
	if (pxOut == ierr) pxOut = px + 10; // when error, we just need a reasonable any number

	return pxOut;
}


double getValue4Color(double x[], long n)
{
	// if the first value is minus, get the min value for coloring
	// otherwise, get the max value for coloring
	if (x == NULL || n < 1) return 0.;
	else if (n == 1) return x[0];
	//else return x[(int)(n/2)];
	else if (x[0] < 0.) return getMinValue(x, n, MAXAMP);
	else return getMaxValue(x, n);
}

float getValue4Color(float x[], long n)
{
	// if the first value is minus, get the min value for coloring
	// otherwise, get the max value for coloring
	if (x == NULL || n < 1) return 0.;
	else if (n == 1) return x[0];
	//else return x[(int)(n/2)];
	else if (x[0] < 0.) return getMinValue(x, n, MAXAMP);
	else return getMaxValue(x, n);
}


float getAvgValueOld(float x[], long n) {
	int j;
	int j1 = 0;
	for (j = 0; j < n; j++) {
		if (fabs(x[j]) > MINAMP) {
			j1 = j;
			break;
		}
	} // for j

	int j2 = n - 1;
	for (j = n - 1; j >= 0; j--) {
		if (fabs(x[j]) > MINAMP) {
			j2 = j;
			break;
		}
	} // for j

	if (j1 >= j2) return 0.f;

	double aAvg = 0.0;
	for (j = j1; j <= j2; j++)
	{
		aAvg += x[j];
	}
	if (j2 - j1 + 1 > 1)
		aAvg /= (double)(j2 - j1 + 1);
	return (float)aAvg;
}

float getAvgValue(float x[], long n) {
	// we will only count non-zero values
	int count = 0;
	double aAvg = 0.0;
	double amp;
	for (int j = 0; j < n; j++) {
		amp = fabs(x[j]);
		if (amp > MINAMP) {
			aAvg += x[j];
			count++;
		}
	}
	if (count > 1)
		aAvg /= (double)count;
	return (float)aAvg;
}

double getAvgValue(double x[], long n) {
	// we will only count non-zero values
	int count = 0;
	double aAvg = 0.0;
	double amp;
	for (int j = 0; j < n; j++) {
		amp = fabs(x[j]);
		if (amp > MINAMP) {
			aAvg += x[j];
			count++;
		}
	}
	if (count > 1)
		aAvg /= (double)count;
	return aAvg;
}

float getAvgValueAbsoluteOld(float *x, long n) {
	// we will need to calculate j1 and j2 
	int j;
	int j1 = 0;
	for (j = 0; j < n; j++) {
		if (fabs(x[j]) > MINAMP) {
			j1 = j;
			break;
		}
	} // for j

	int j2 = n - 1;
	for (j = n - 1; j >= 0; j--) {
		if (fabs(x[j]) > MINAMP) {
			j2 = j;
			break;
		}
	} // for j

	if (j1 >= j2) return 0.f;

	double aAvg = 0.0;
	for (j = j1; j <= j2; j++)
	{
		aAvg += fabs(x[j]);
	}
	if (j2 - j1 + 1 > 1)
		aAvg /= (double)(j2 - j1 + 1);
	return (float)aAvg;
}


float getAvgValueAbsolute(float x[], long n) {
	// we will only count non-zero values

	int count = 0;
	double aAvg = 0.0;
	double amp;
	for (int j = 0; j < n; j++) {
		amp = fabs(x[j]);
		if (amp > MINAMP) {
			aAvg += amp;
			count++;
		}
	}
	if (count > 1)
		aAvg /= (double)count;
	return (float)aAvg;
}

//
//double getMedianFast(float *x, long n)
//{
//	int k = getMedian3(x, n);
//	return x[k];
//
//	// the following was a working function but not needed any more.
//
//	// assume x[] already has absolute values
//	if (n<1) return 0.;
//
//	float result1;
//	float aMax = 0.f;
//	long j;
//	long num = 0;
//
//	result1 = 0.;
//	for (j=0; j<n; j++)
//	{
//		if (aMax < x[j]) aMax = x[j];
//		if (x[j] > 0.f) {
//			result1 += x[j];
//			num++;
//		}
//	}
//	if (num > 0) result1 /= num;
//	else return 0.f;
//
//	// now, we have:  aMax=the max value;  result1=mean
//
//	float aThreshold = result1 * 40.f;
//	if (aMax < aThreshold) {
//		return result1;
//	}
//
//
//	result1 = 0.;
//	num = 0;
//	for (j=0; j<n; j++)
//	{
//		if (aThreshold > x[j]) {
//			result1 += x[j];
//			num++;
//		}
//	}
//
//	if (num > 0) result1 /= num;
//	return result1;
//}

/* given an distance array, and distance value, return the closest match index
	for this distance. Assume trace[] is already sorted!!!
	*/
int getClosestIndex(int trace[], int n, float x)
{
	if (trace == NULL) return -1;

	for (int j = 1; j < n; j++) {
		if (x < trace[j - 1] + (trace[j] - trace[j - 1])*.5) return j - 1;
	}

	return n - 1;
}

// dist[] do not be pre-sorted here!!!
int getClosestIndex(float dist[], int n, float x)
{
	return getClosestIndex(dist, n, x, 0);

	//if (dist == NULL) return -1;

	//for (int j = 1; j < n; j++) {
	//	if ((x >= dist[j - 1] && x <= dist[j])
	//		||
	//		(x <= dist[j - 1] && x >= dist[j])
	//		) {
	//		
	//		// now the x is in between j-1 and j. Which one is even closer?
	//		if (fabs(x - dist[j - 1]) < fabs(x - dist[j]))
	//			return j - 1;
	//		else
	//			return j;
	//	}
	//}

	//return n - 1;
}


int getClosestIndex(float fy, float dy, int n, float x, int i0)
{
	float y;
	if (i0 < 0) i0 = 0;
	for (int j = i0 + 1; j < n; j++) {
		y = fy + dy  *  (j - 1);
		if (x < y + dy  *.5) return j - 1;
	}

	return n - 1;
}



int getClosestIndex(double fy, double dy, int n, double x, int i0)
{
	double y;
	if (i0 < 0) i0 = 0;
	for (int j = i0 + 1; j < n; j++) {
		y = fy + dy  *  (j - 1);
		if (x < y + dy  *.5) return j - 1;
	}

	return n - 1;
}



// i0 is the starting index to search, so that the result must be >= i0
int getClosestIndex(double dist[], int n, double x, int i0)
{
	if (dist == NULL) return -1;
	if (i0 < 0) i0 = 0;
	for (int j = i0 + 1; j < n; j++) {
	//	if (x < dist[j - 1] + (dist[j] - dist[j - 1])*.5) return j - 1;


		if ((x >= dist[j - 1] && x <= dist[j])
			||
			(x <= dist[j - 1] && x >= dist[j])
			) {

			// now the x is in between j-1 and j. Which one is even closer?
			if (fabs(x - dist[j - 1]) < fabs(x - dist[j]))
				return j - 1;
			else
				return j;
		}


	}

	return n - 1;
}

int getClosestIndex(float dist[], int n, float x, int i0)
{
	if (dist == NULL) return -1;
	if (i0 < 0) i0 = 0;
	for (int j = i0 + 1; j < n; j++) {

	//	if (x < dist[j - 1] + (dist[j] - dist[j - 1])*.5f) return j - 1;
		if ((x >= dist[j - 1] && x <= dist[j])
			||
			(x <= dist[j - 1] && x >= dist[j])
			) {

			// now the x is in between j-1 and j. Which one is even closer?
			if (fabs(x - dist[j - 1]) < fabs(x - dist[j]))
				return j - 1;
			else
				return j;
		}


	}

	return n - 1;
}

int getClosestIndexRev_Deprec(double dist[], int n, double x, int i0)
{
	return getClosestIndex(dist, n, x, 0);

	//if (dist == NULL) return -1;
	//if (i0 >= n) i0 = n - 1;
	//if (i0 < 0) i0 = 0;
	//i0 = 0;
	//for (int j = n - 1; j > i0; j--) {
	//	if (x < dist[j - 1] + (dist[j] - dist[j - 1])*.5) return j - 1;
	//}

	//return 0;
}








COLORREF getGoodColorRand(bool isMainModelShowing)
{
	COLORREF cr;
	if (!isMainModelShowing) {
		double r1 = rand() / (RAND_MAX + 1.0);
		double r2 = rand() / (RAND_MAX + 1.0);
		double r3 = rand() / (RAND_MAX + 1.0);
		// the following line makes the colors very pure.
		int r = (int)(150 * (int)(r1 + 0.5));
		int g = (int)(220 * (int)(r2 + 0.5));
		int b = (int)(220 * (int)(r3 + 0.5));
		int num = r + g + b;
		if (num > 600 || num < 10) {
			// we do not want a pure white ray, since it may not be visible in some cases
			// r = g = b = 200;
			return getGoodColorRand(isMainModelShowing);
		}
		cr = RGB(r, g, b);
	}
	else cr = RGB(255, 0, 0);

	return cr;
}

//
//double getAmpAtXY(float **dataArr, float *rangeArr, int nx, int ny, float ymin, float dy, float x, float y)
//{
//	// Given a 2D array of amp, look for the index of the 4 corners the surrounds the given point (x, y)
//	// After that use inverse distance rule to get the COLORREF
//	// NOTE: our routine allows X-axis irregularly spaced, while Y-axis is still regularly spaced, so there is no xmin and dx inputs.
//	if (dataArr==NULL || rangeArr==NULL || nx<1 || ny<1 || dy==0.f) return 0.;
//
//	// note: rangeArr[] should be monotonically increasing or decreasing, what about if not? Then we should use the closest index only!
//	int jx = -1;
//	if (x<=rangeArr[0]) jx=0;
//	else if (x>=rangeArr[nx-1]) jx=nx-1;
//	else {
//		for (int i=0; i<nx-1; i++) {
//			if ( x> rangeArr[i] && x< rangeArr[i+1]) {
//				jx = i;
//				break;
//			}
//		}
//	}
//
//	if (jx<0) jx=0;
//	else if (jx>nx-1) jx=nx-1;
//	int jx2 = jx+1;
//	if (jx2>nx-1) jx2=nx-1;
//	double ax = rangeArr[jx2] - rangeArr[jx];
//	double percentFromJx = fabs(ax) < MINAMP  ?  0.  :  (x - rangeArr[jx]) / ax;
//	if (percentFromJx < 0.0) percentFromJx = 0.0;
//	else if (percentFromJx>1.0) percentFromJx = 1.0;
//
//
//	double ay = (y - ymin)/dy;
//	int iy = (int)ay;
//	if (iy<0) iy=0;
//	else if (iy>ny-1) iy=ny-1;
//	int iy2 = iy+1;
//	if (iy2>ny-1) iy2=ny-1;
//
//	return interpolate((double)dataArr[jx][iy], (double)dataArr[jx2][iy], (double)dataArr[jx][iy2], (double)dataArr[jx2][iy2], 
//		percentFromJx, (double)(ay - iy));
//}
//
//double getAmpAtXY(float **dataArr, int nx, int ny, double ymin, double dy, 
//				  double y,  
//				  int jx, int jx2, double percentFromJx)
//{
//	if (dataArr==NULL || nx<1 || ny<1 || dy==0.) return 0.;
//
//	if (jx<0) jx=0;
//	else if (jx>nx-1) jx=nx-1;
//	if (jx2<0) jx2=0;
//	else if (jx2>nx-1) jx2=nx-1;
//
//	double ay = (y - ymin)/dy;
//	int iy = (int)ay;
//	if (iy<0) iy=0;
//	else if (iy>ny-1) iy=ny-1;
//	int iy2 = iy+1;
//	if (iy2>ny-1) iy2=ny-1;
//
//	return interpolate((double)dataArr[jx][iy], (double)dataArr[jx2][iy], (double)dataArr[jx][iy2], (double)dataArr[jx2][iy2], 
//		percentFromJx, (double)(ay - iy));
//}


// Assuming regular interval in Y-axis for input data is dataArr[jx:jx2][0:ny] with dy as Y-increment, 
// this function will interpolate ampArr[0:nrow]
#include "sumig.h"
bool getAmpAtXY(float **dataArr, int nx, int ny, double ymin, double dy,
	int iTrace1, int iTrace2, double percentFromTr1,
	float *trPixel, float *yValPixel, int nyPixel,
	float *trDataPixel1, float *trDataPixel2, int &iTrace1Existing, int &iTrace2Existing)
{
	// note: yInterpol is increment time in sec for each pixel, while dy is for each data sample. They are different
	// iTrace1 and iTrace2 are indeces for the spanning traces, and they are not trace numbers
	// percentFromTr1 is percentage from the left side (i.e. iTrace1)
	// trDataPixel1[], trDataPixel2[], and trPixel[],yValPixel have the same length of nyPixel
	// after the finish of this function, iTrace1Existing=iTrace1, iTrace2Existing=iTrace2
	if (dataArr == NULL || nx < 1 || ny < 1 || dy == 0. || trPixel == NULL) return false;
	if (nyPixel < 1) return false;
	if (iTrace1 < 0) iTrace1 = 0;
	else if (iTrace1 > nx - 1) iTrace1 = nx - 1;
	if (iTrace2 < 0) iTrace2 = 0;
	else if (iTrace2 > nx - 1) iTrace2 = nx - 1;

	if (iTrace1 == iTrace1Existing) {
		// good, no need to change for trDataPixel1
	}
	else  if (iTrace1 == iTrace2Existing) {
		// memcpy trDataPixel2[] to trDataPixel1[]
		memcpy((void *)trDataPixel1, (void *)trDataPixel2, nyPixel*sizeof(float));
	}
	else {
		// by now, we can start interpolation
		ints8r(ny, // ntin
			(float)dy,   // dtin
			(float)ymin,   // ftin
			&dataArr[iTrace1][0], // yin[t=ntin], with yin[0] = y(fxin)
			0.f,  // yinl    value used to extrapolate yin values to left of yin[0]
			0.f,  // yinr	 value used to extrapolate yin values to right of yin[ntin-1]
			nyPixel,   // nxout
			yValPixel,  // ntout
			trDataPixel1);    // tout
	}

	if (iTrace2 == iTrace2Existing) {
		// good, no need to change for trDataPixel1
	}
	else  if (iTrace2 == iTrace1Existing) {
		// memcpy trDataPixel1[] to trDataPixel2[]
		memcpy((void *)trDataPixel2, (void *)trDataPixel1, nyPixel*sizeof(float));
	}
	else {
		// by now, we can start interpolation
		ints8r(ny, // ntin
			(float)dy,   // dtin
			(float)ymin,   // ftin
			&dataArr[iTrace2][0], // yin[t=ntin], with yin[0] = y(fxin)
			0.f,  // yinl    value used to extrapolate yin values to left of yin[0]
			0.f,  // yinr	 value used to extrapolate yin values to right of yin[ntin-1]
			nyPixel,   // nxout
			yValPixel,  // ntout
			trDataPixel2);    // tout
	}
	iTrace1Existing = iTrace1;
	iTrace2Existing = iTrace2;

	for (int irow = 0; irow < nyPixel; irow++) {
		trPixel[irow] = (float)(trDataPixel1[irow] + percentFromTr1 * (trDataPixel2[irow] - trDataPixel1[irow]));
	}

	return true;
}

// for a single output trace defined by ymin and yRate. percentFromJx is percentage from the left side (i.e. jx index)
// bilinear interpolation is used
bool getAmpAtXY(float **dataArr, int nx, int ny, double ymin, double dy,
	double yRate, double *ampArr, int nrow,
	int jx, int jx2, double percentFromJx)
{
	if (dataArr == NULL || nx < 1 || ny < 1 || dy == 0. || ampArr == NULL) return false;
	if (nrow < 1) return false;
	if (jx < 0) jx = 0;
	else if (jx > nx - 1) jx = nx - 1;
	if (jx2 < 0) jx2 = 0;
	else if (jx2 > nx - 1) jx2 = nx - 1;

	for (int irow = 0; irow<nrow; irow++) {
		register double ay = irow * yRate / dy;
		register int iy = (int)ay;

		if (iy < 0) iy = 0;
		else if (iy > ny - 1) iy = ny - 1;
		int iy2 = iy + 1;
		if (iy2>ny - 1) iy2 = ny - 1;

		ampArr[irow] = interpolate_d((double)dataArr[jx][iy],
			(double)dataArr[jx2][iy], (double)dataArr[jx][iy2],
			(double)dataArr[jx2][iy2],
			percentFromJx, (double)(ay - iy));
	}
	return true;
}
//
//bool getAmpAtXYSpline(float **dataArr, int nx, int ny, float ymin, float dy,
//	int jx, int jx2, float percentFromJx, int nPixel)
//{
//	if (dataArr == NULL || nx<1 || ny<1 || dy == 0. || 
//		ampPixel == NULL || timePixel==NULL) return false;
//	if (nPixel < 1) return false;
//	if (jx<0) jx = 0;
//	else if (jx>nx - 1) jx = nx - 1;
//	if (jx2<0) jx2 = 0;
//	else if (jx2>nx - 1) jx2 = nx - 1;
//
//	for (int irow = 0; irow<nPixel; irow++) {
//		ampPixel[irow] = amp1[irow] + percentFromJx*(ampPixel[irow] - amp1[irow]);
//	}
//	free1float(amp1);
//	return true;
//}

// good backup version using bilinear interpolation. But I need to improve it
bool getAmpAtXYGoodBack(float **dataArr, int nx, int ny, double ymin, double dy,
	double yRate, double *ampArr, int nrow,
	int jx, int jx2, double percentFromJx)
{
	if (dataArr == NULL || nx < 1 || ny < 1 || dy == 0. || ampArr == NULL) return false;
	if (nrow < 1) return false;
	if (jx < 0) jx = 0;
	else if (jx > nx - 1) jx = nx - 1;
	if (jx2 < 0) jx2 = 0;
	else if (jx2 > nx - 1) jx2 = nx - 1;

	for (int irow = 0; irow<nrow; irow++) {
		register double ay = irow * yRate / dy;
		register int iy = (int)ay;

		if (iy < 0) iy = 0;
		else if (iy > ny - 1) iy = ny - 1;
		int iy2 = iy + 1;
		if (iy2>ny - 1) iy2 = ny - 1;

		ampArr[irow] = interpolate_d((double)dataArr[jx][iy],
			(double)dataArr[jx2][iy], (double)dataArr[jx][iy2],
			(double)dataArr[jx2][iy2],
			percentFromJx, (double)(ay - iy));
	}
	return true;
}


void trimRight(TCHAR line[]) {
	// note: the last character is a line feed of carrage return
	for (int m = (int)_tcslen(line) - 2; m > 0; m--) {
		if (line[m] != ' ') {
			line[m + 1] = '\0';
			return;
		}
	}
}



// The followings are all utils required from Numerical Recipes book:
#define NR_END 2   // if here I use 1, sometimes there are exceptions and cannot get arround it
#define FREE_ARG char*
void nrerror(TCHAR error_text[])
/* Numerical Recipes standard error handler */
{
	_ftprintf(stderr, _T("Numerical Recipes run-time error...\n"));
	_ftprintf(stderr, _T("%s\n"), error_text);
	_ftprintf(stderr, _T("...now exiting to system...\n"));
	exit(1);
}
float *vector(long nl, long nh)
/* allocate a float vector with subscript range v[nl..nh] */
{
	float *v;
	v = (float *)ealloc1float((size_t)(nh + 1 + NR_END));
	if (!v) nrerror(_T("allocation failure in vector()"));
	//return v - nl + NR_END;
	return v;
}
int *ivector(long nl, long nh)
/* allocate an int vector with subscript range v[nl..nh] */
{
	int *v;
	v = (int *)ealloc1int((size_t)(nh + 1 + NR_END));
	if (!v) nrerror(_T("allocation failure in ivector()"));
//	return v - nl + NR_END;
	return v;
}
unsigned long *lvector(long nl, long nh)
/* allocate an unsigned long vector with subscript range v[nl..nh] */
{
	unsigned long *v;
	v = (unsigned long *)ealloc1long((size_t)(nh + 1 + NR_END));
	if (!v) nrerror(_T("allocation failure in lvector()"));
//	return v - nl + NR_END;
	return v;
}
void free_vector(float *v, long nl, long nh)
/* free a float vector allocated with vector() */
{
	//free1((FREE_ARG)(v + nl - NR_END));
	free1(v);
}
void free_ivector(int *v, long nl, long nh)
/* free an int vector allocated with ivector() */
{
	//free1((FREE_ARG)(v + nl - NR_END));
	free1(v);
}
void free_cvector(unsigned char *v, long nl, long nh)
/* free an unsigned char vector allocated with cvector() */
{
	//free1((FREE_ARG)(v + nl - NR_END));
	free1(v);
}
void free_lvector(unsigned long *v, long nl, long nh)
/* free an unsigned long vector allocated with lvector() */
{
//	free1((FREE_ARG)(v + nl - NR_END));
	free1(v);
}
void free_dvector(double *v, long nl, long nh)
/* free a double vector allocated with dvector() */
{
//	free1((FREE_ARG)(v + nl - NR_END));
	free1(v);
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
	if (n <= 0) return;

	unsigned long i, indxt, ir = n, itemp, j, k, l = 1;
	int jstack = 0, *istack;
	float a;
	istack = ivector(1, NSTACK);

	for (j = 1; j <= n; j++) indx[j] = j;
	for (;;) {
		if (ir - l < M) {
			for (j = l + 1; j <= ir; j++) {
				indxt = indx[j];
				a = arr[indxt];
				for (i = j - 1; i >= l; i--) {
					if (arr[indx[i]] <= a) break;
					indx[i + 1] = indx[i];
				}
				indx[i + 1] = indxt;
			}
			if (jstack == 0) break;
			ir = istack[jstack--];
			l = istack[jstack--];
		}
		else {
			k = (l + ir) >> 1;
			SWAP2(indx[k], indx[l + 1]);
			if (arr[indx[l]] > arr[indx[ir]]) {
				SWAP2(indx[l], indx[ir]);
			}
			if (arr[indx[l + 1]] > arr[indx[ir]]) {
				SWAP2(indx[l + 1], indx[ir]);
			}
			if (arr[indx[l]] > arr[indx[l + 1]]) {
				SWAP2(indx[l], indx[l + 1]);
			}
			i = l + 1;
			j = ir;
			indxt = indx[l + 1];
			a = arr[indxt];
			for (;;) {

				do i++; while (arr[indx[i]] < a);
				do j--; while (arr[indx[j]] > a);
				if (j < i) break;
				SWAP2(indx[i], indx[j]);
			}
			indx[l + 1] = indx[j];
			indx[j] = indxt;
			jstack += 2;
			if (jstack > NSTACK) nrerror(_T("NSTACK too small in indexx."));
			if (ir - i + 1 >= j - l) {
				istack[jstack] = ir;
				istack[jstack - 1] = i;
				ir = j - 1;
			}
			else {
				istack[jstack] = j - 1;
				istack[jstack - 1] = l;
				l = i;
			}
		}
	}
	free_ivector(istack, 1, NSTACK);
}


// my simplest sorting routine
void sortSei(float x[], long n)
{
	for (int i = 0; i < n-1; i++) {
		if (myFloat(fabs(x[i] - x[i + 1]), 3) == 0.001)
			return; 
	}
	sort1(x, n);
}

void sort1(float x[], long n)
{
	//return;

	// since Numerical recipe uses 1...n, but my arrays are 0...n-1
	long j;
	float *ra = vector(0, n);
	for (j = 0; j < n; j++) {
		ra[j + 1] = x[j];
	}

	unsigned long *iwksp = lvector(1, n);
	indexx(n, ra, iwksp); // Make the index table.

	for (j = 0; j < n; j++) {
		x[j] = ra[iwksp[j + 1]];
	}
	free_lvector(iwksp, 1, n);
	free_vector(ra, 0, n);
}

void sort3(unsigned long n, float ra[], float rb[], float rc[])
/*
Sorts an array ra[1..n] into ascending numerical order while making the corresponding rearrangements
of the arrays rb[1..n] and rc[1..n]. An index table is constructed via the
routine indexx.
*/
{
	//	void indexx(unsigned long n, float arr[], unsigned long indx[]);
	unsigned long j, *iwksp;
	float *wksp;
	iwksp = lvector(1, n);
	wksp = vector(1, n);
	indexx(n, ra, iwksp); // Make the index table.
	for (j = 1; j <= n; j++) wksp[j] = ra[j]; // Save the array ra.
	for (j = 1; j <= n; j++) ra[j] = wksp[iwksp[j]]; // Copy it back in rearranged order.
	for (j = 1; j <= n; j++) wksp[j] = rb[j]; // Ditto rb.
	for (j = 1; j <= n; j++) rb[j] = wksp[iwksp[j]];
	for (j = 1; j <= n; j++) wksp[j] = rc[j]; // Ditto rc.
	for (j = 1; j <= n; j++) rc[j] = wksp[iwksp[j]];
	free_vector(wksp, 1, n);
	free_lvector(iwksp, 1, n);
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
	unsigned long j, *iwksp;
	float *wksp;
	iwksp = lvector(1, n);
	wksp = vector(1, n);

	// since Numerical recipe uses 1...n, but my arrays are 0...n-1
	// I convert first
	for (j = n; j > 0; j--) {
		ra[j] = ra[j - 1];
		rb[j] = rb[j - 1];
	}

	indexx(n, ra, iwksp); // Make the index table.
	for (j = 1; j <= n; j++) wksp[j] = ra[j]; // Save the array ra.
	for (j = 1; j <= n; j++) ra[j] = wksp[iwksp[j]]; // Copy it back in rearranged order.
	for (j = 1; j <= n; j++) wksp[j] = rb[j]; // Ditto rb.
	for (j = 1; j <= n; j++) rb[j] = wksp[iwksp[j]];

	// I convert back
	for (j = 0; j < n; j++) {
		ra[j] = ra[j + 1];
		rb[j] = rb[j + 1];
	}

	free_vector(wksp, 1, n);
	free_lvector(iwksp, 1, n);
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
	unsigned long j, *iwksp;
	float *wksp;
	iwksp = lvector(1, n);
	wksp = vector(1, n);

	// since Numerical recipe uses 1...n, but my arrays are 0...n-1
	// I convert first
	for (j = n; j > 0; j--) {
		ra[j] = ra[j - 1];
		ib[j] = ib[j - 1];
	}

	indexx(n, ra, iwksp); // Make the index table.
	for (j = 1; j <= n; j++) wksp[j] = ra[j]; // Save the array ra.
	for (j = 1; j <= n; j++) ra[j] = wksp[iwksp[j]]; // Copy it back in rearranged order.
	for (j = 1; j <= n; j++) wksp[j] = (float)ib[j]; // Ditto rb.
	for (j = 1; j <= n; j++) ib[j] = (int)myRound(wksp[iwksp[j]]);

	// I convert back
	for (j = 0; j < n; j++) {
		ra[j] = ra[j + 1];
		ib[j] = ib[j + 1];
	}

	free_vector(wksp, 1, n);
	free_lvector(iwksp, 1, n);
}




void sortDistance(float **ampArr, float *rangeArray, int nx, int nt) {
	if (!ampArr || nx<2 || nt<2) return;

	//segy tr;
	//float *ranges = ealloc1float(headRead.getnx());

	//for (int i = 0; i < this->headRead.getnx(); i++) {
	//	ranges[i] = isOffset ?
	//		this->headRead.trArr[i].getOffset() :
	//		this->headRead.trArr[i].getDistance() +
	//		this->m_distShift;
	//}

	//int sampByte = (this->headRead.bh.format != 3) ? 4 : 2;
	int n, m;
	int len = sizeof(float) * nt;
	float *aDat = (float *)alloc1float(nt);
	for (n = 0; n < nx - 1; n++) {
		m = getMinIndex(&rangeArray[n], nx - n);

		// now m is not the index in trArr[], it must be added by an index shift of n! this a big programming trap!
		m += n;

		if (m > n) {
			// the range at m needs to swap with the trace at n. Note  trArr.data  is a separate pointer from trArr itself
			memcpy((void *)&aDat[0], (void *)&ampArr[n][0], len);
			memcpy((void *)&ampArr[n][0], (void *)&ampArr[m][0], len);
			memcpy((void *)&ampArr[m][0], (void *)&aDat[0], len);

			register float temp = rangeArray[n];
			rangeArray[n] = rangeArray[m];
			rangeArray[m] = temp;
		}
	} // for n

	free1float(aDat);
}


void four1(float data[], unsigned long nn, int isign)
{
	//This function is taken from Numerical Recipe.

	unsigned long n, mmax, m, j, istep, i;
	double wtemp, wr, wpr, wpi, wi, theta;
	float tempr, tempi;

	n = nn << 1;
	j = 1;
	for (i = 1; i < n; i += 2) {
		if (j > i) {
			SWAP(data[j], data[i]);
			SWAP(data[j + 1], data[i + 1]);
		}
		m = n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	//Here begins the Danielson-Lanczos section.
	mmax = 2;
	while (n > mmax) {
		istep = mmax << 1;
		theta = isign*(6.28318530717959 / mmax);
		wtemp = sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m = 1; m < mmax; m += 2) {
			for (i = m; i <= n; i += istep) {
				j = i + mmax;
				tempr = (float)(wr*data[j] - wi*data[j + 1]);
				tempi = (float)(wr*data[j + 1] + wi*data[j]);
				data[j] = data[i] - tempr;
				data[j + 1] = data[i + 1] - tempi;
				data[i] += tempr;
				data[i + 1] += tempi;
			}
			wr = (wtemp = wr)*wpr - wi*wpi + wr;
			wi = wi*wpr + wtemp*wpi + wi;
		}
		mmax = istep;
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
	unsigned long i, i1, i2, i3, i4, np3;
	float c1 = 0.5, c2, h1r, h1i, h2r, h2i;
	double wr, wi, wpr, wpi, wtemp, theta;  //Double precision for the trigonomet-
	// ric recurrences.

	theta = 3.141592653589793 / (double)(n >> 1); // Initialize the recurrence.
	if (isign == 1) {
		c2 = -0.5;
		four1(data, n >> 1, 1); // The forward transform is here.
	}
	else {
		c2 = 0.5;                 //Otherwise set up for an inverse transform.
		theta = -theta;
	}
	wtemp = sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi = sin(theta);
	wr = 1.0 + wpr;
	wi = wpi;
	np3 = n + 3;
	for (i = 2; i <= (n >> 2); i++) {   //Case i=1 done separately below.
		i4 = 1 + (i3 = np3 - (i2 = 1 + (i1 = i + i - 1)));
		h1r = c1*(data[i1] + data[i3]); // The two separate transforms are separated out of data.
		h1i = c1*(data[i2] - data[i4]);
		h2r = -c2*(data[i2] + data[i4]);
		h2i = c2*(data[i1] - data[i3]);
		data[i1] = (float)(h1r + wr*h2r - wi*h2i); //Here they are recombined to form the true
		//transform of the original real data.
		data[i2] = (float)(h1i + wr*h2i + wi*h2r);
		data[i3] = (float)(h1r - wr*h2r + wi*h2i);
		data[i4] = (float)(-h1i + wr*h2i + wi*h2r);
		wr = (wtemp = wr)*wpr - wi*wpi + wr; // The recurrence.
		wi = wi*wpr + wtemp*wpi + wi;
	}
	if (isign == 1) {
		data[1] = (h1r = data[1]) + data[2];  //Squeeze the rst and last data
		//together to get them all within the original array.
		data[2] = h1r - data[2];
	}
	else {
		data[1] = c1*((h1r = data[1]) + data[2]);
		data[2] = c1*(h1r - data[2]);
		four1(data, n >> 1, -1); //inverse transform for the case isign=-1.
	}
}











/*
* The following is for converting from lat long to cartesion coords
* assuming the origin is at the north pole
*/
// the origin
static double alon0 = -60.;
static double alat0 = 90.;  // NOTE: 90. is assumed in the following formula.

double* convLatlon2XY(double alon, double alat) {
	// convert (alat, alon) from polar stereographic projection to Cartesion coordinates (x, y)

	// define the reference point and longitude
	//	double y0 = -2.*RAD*tan((45.-alat0*0.5)*PIO);

	double fact = 2.*EARTH_RADIUS*cos(alat*PIO) / (1. + sin(alat*PIO));
	double *xy = (double *)ealloc1double(2);    // {x, y}
	xy[0] = fact*sin((alon - alon0)*PIO);
	xy[1] = -fact*(cos((alon - alon0)*PIO));
	return xy;
}

double* convXY2Latlon(double x, double y) {
	// convert (alat, alon) from polar stereographic projection to Cartesion coordinates (x, y)
	// define the reference point and longitude
	// double y0 = -2.*RAD*tan((45.-alat0*0.5)*PIO);

	double po = sqrt(x*x + (y*y));
	double c = 2. * atan(po / (2.*EARTH_RADIUS));  // in gradient
	double *lonlat = (double *)ealloc1double(2);
	if (po < MINAMP) {
		lonlat[0] = alon0;
		lonlat[1] = alat0;
	}
	else {
		lonlat[0] = alon0 + atan(-x / y) * OPI; // in degrees
		if (lonlat[0] > 180.) lonlat[0] -= 360.;
		if (lonlat[0] < -180.) lonlat[0] += 360.;
		lonlat[1] = asin(cos(c)) * OPI; // in degrees
	}

	return lonlat;
}

void setShotsFromDist(int *filej, int *shotk, float *shotDist, int numShot2Sum, int j, int k, float adist) {
	// change the filej[] and shotk[] and shotDist[] according to a new distance value

	// first find the max distance in the array
	float adistMax = 0.f;
	int iForDistMax = -1;
	for (int i = 0; i<numShot2Sum; i++) {
		if (shotDist[i]> adistMax) {
			adistMax = shotDist[i];
			iForDistMax = i;
		}
	}
	if (iForDistMax >= 0 && adist < adistMax) {
		// now, the new shot at adist is better than the maximum distance in the array shotDist[]
		filej[iForDistMax] = j;
		shotk[iForDistMax] = k;
		shotDist[iForDistMax] = adist;
	}

}

void filterTimeDomain(float *rData, int outLen, float f1, float f2, long tpp) {
	double a, b, c0, c1, c2, d1, d2;
	int i;

	if (outLen < 2 || tpp < 1) {
		return;
	}

	double dt = (double)tpp / (double)MIL;

	a = tan(PI*f1*dt);
	if (f2 < 0.f) f2 = 0.5f / (float)dt;
	b = tan(PI*f2*dt);
	c0 = -b / (1. + a) / (1. + b);
	c1 = 0.;
	c2 = -c0;
	d1 = ((1. + a)*(1. - b) + (1. - a)*(1. + b)) / ((1. + a)*(1. + b));
	d2 = -((1. - a)*(1. - b)) / ((1. + a)*(1. + b));

	// first remove the DC energy
	//float aMedian = (float)getMedian(&rData[0], outLen);	
	//for (i=0; i<outLen; i++) rData[i] -= aMedian;

	double temp = 0.;
	for (i = 0; i<outLen; i++) {
		temp += rData[i];
	}
	if (fabs(temp)>MINAMP){
		float temp2 = (float)(temp / (float)outLen);
		for (i = 0; i < outLen; i++) rData[i] -= temp2;
	}


	float* tData = (float *)ealloc1float((size_t)outLen);
	tData[0] = (float)c0*rData[0];
	tData[1] = (float)(c0*rData[1] + c1*rData[0] + d1*tData[0]);
	for (i = 2; i < outLen; i++)
		tData[i] = (float)(c0*rData[i] + c1*rData[i - 1] + c2*rData[i - 2] + d1*tData[i - 1] + d2*tData[i - 2]);
	for (i = 0; i < outLen; i++)
		rData[i] = -tData[i];


	free1float(tData);
}

double removeTimeLine(float *rData, int itwmin, int itwmax, int itwdt) {
	// now we need to calculate remove the timeline within a small windows of twdt=0.08 sec 
	// this actual window should be between  itwmin and itwmax
	double sum2 = 0.;
	int itwminChosen = itwmin;
	int ii;

	for (ii = itwmin; ii < itwmax - itwdt; ii++) {
		double sum3 = 0.;
		for (int jj = ii; jj < ii + itwdt; jj++) {
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
	for (ii = itwminChosen; ii < itwmaxChosen; ii++) {
		if (ii < itwminChosen + idt) {
			rData[ii] *= (float)(1. - (double)(ii - itwminChosen) / (double)idt);
		}
		else if (ii > itwmaxChosen - idt) {
			rData[ii] *= (float)(1. - (double)(itwmaxChosen - ii) / (double)idt);
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
	if (fabs(lat1 - lat2) < 0.00001 && fabs(long1 - long2) < 0.00001) return 0.0;

	// # Constants
	static const double AE = 6378206.4;
	static const double FL = 1 / 294.9787;
	// double PI=3.1415925635;  // already defined in other place
	static const double DR = 3.14159 / 180;
	double FLAG = 1.0;

	// solve some coord issues
	if (fabs(long2 - long1) > 90.) {
		if (long2<0. && long1>0.) long2 += 360.;
		else if (long2 > 0. && long1<0.) long1 += 360.;
		else {
			// now long1 and long2 are of the same signs
			if (long1 > 180. && long2 < 90.) long1 -= 360.;
			else if (long1 < 90. && long2 > 180.) long2 -= 360.;
		}
	}

	if ((fabs(lat1 - lat2) > 90) || (fabs(long2 - long1) > 90.)) {
		FLAG = 2.0;
		lat1 = lat1*0.5;
		lat2 = lat2*0.5;
		long1 = long1*0.5;
		long2 = long2*0.5;
	}
	if (fabs(long2 - long1) > 180) {
		FLAG = 4.0;
		lat1 = lat1 / 4.0;
		lat2 = lat2 / 4.0;
		long1 = long1 / 4.0;
		long2 = long2 / 4.0;
	}

	// # Convert values from degrees to Radians
	lat1 *= DR;
	long1 *= DR;
	lat2 *= DR;
	long2 *= DR;

	// double PHM=(lat2+lat1)*0.5;
	// double DLON=long2-long1;
	double DPM = (lat2 - lat1)*0.5;
	double DLM = (long2 - long1)*0.5;

	// #   CALCULATIONS
	// double SPM=sin(PHM);
	// double CDP=cos(DPM);
	// double SDL=sin(DLM);
	// double CDL=cos(DLM);
	double CPM = cos((lat2 + lat1)*0.5);
	double SDP = sin(DPM);

	// #   SPHERICAL ARC DISTANCE BY HALF-ANGLE FORMULA
	double DN = SDP*cos(DLM);
	double DE = sin(DLM)*CPM;

	double SDH = sqrt(DN*DN + DE*DE);
	double CDH = sqrt(1.0 - SDH*SDH);
	double SIND = 2.0*SDH*CDH;
	double COSD = sqrt(1.0 - SIND*SIND);

	// #   D=SPHERICAL ARC(RADS),SARC.DIST(METERS)
	double ARCD = atan2(SIND, COSD);
	// double S=AE*ARCD;

	// #   SET UP ELLIPSOIDAL CORRECTION TERMS
	double VK = sin((lat2 + lat1)*0.5)*cos(DPM);
	double VKK = SDP*CPM;

	double U = 2.0*VK*VK / (CDH*CDH);
	double V = 2.0*VKK*VKK / (SDH*SDH);
	double X = U + V;
	double Y = U - V;

	double T = ARCD / SIND;
	double E = 30.0*COSD;
	double A = 4.0*T*(8.0 + T*E / 15.0);
	double D = 4.0*(6.0 + T*T);
	//	double B=(D+D)*(-1);
	//	double C=T-(A+E)/2.0;

	double DS = -AE*SIND*(FL / 4.0)*(T*X - 3.0*Y);
	double DDS = AE*SIND*(FL*FL / 64.0)*(X*(A + (T - (A + E)*.5)*X) + Y*(-(D + D) + E*Y) + D*X*Y);

	// #   ELLIPSOIDAL DIST=SPHERIACL DIST + 1ST AND 2ND ORDER TERMS
	double DIST = AE*ARCD + DS + DDS;

	// double CHECK=fabs(DLM)+fabs(DPM);

	if (fabs(DLM) + fabs(DPM) < 0.000001) {
		DIST = 0.00;
	}

	DIST = DIST*0.001*FLAG;

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
		if (hexStg[n] == '\0')
			break;
		if (hexStg[n] > 0x29 && hexStg[n] < 0x40) //if 0 to 9
			digit[n] = hexStg[n] & 0x0f;            //convert to int
		else if (hexStg[n] >= 'a' && hexStg[n] <= 'f') //if a to f
			digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
		else if (hexStg[n] >= 'A' && hexStg[n] <= 'F') //if A to F
			digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
		else break;
		n++;
	}
	count = n;
	m = n - 1;
	n = 0;
	while (n < count) {
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
bfdesign(float fpass, float apass, float fstop, float astop,
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
	float wpass, wstop, fnpoles, w3db;

	/* warp frequencies according to bilinear transform */
	wpass = (float)(2.0*tan(PI*fpass));
	wstop = (float)(2.0*tan(PI*fstop));

	/* if lowpass filter, then */
	if (fstop > fpass) {
		fnpoles = (float)log((1.0 / (apass*apass) - 1.0) / (1.0 / (astop*astop) - 1.0))
			/ (float)log(pow((double)(wpass / wstop), 2.0));
		w3db = (float)(wpass / pow((1.0 / (apass*apass) - 1.0), 0.5 / fnpoles));

		/* else, if highpass filter, then */
	}
	else {

		fnpoles = (float)log((1.0 / (apass*apass) - 1.0) / (1.0 / (astop*astop) - 1.0))
			/ (float)log(pow((double)(wstop / wpass), 2.0));
		w3db = (float)(wpass*pow((1.0 / (apass*apass) - 1.0), 0.5 / fnpoles));
	}

	/* determine integer number of poles */
	*npoles = 1 + (int)fnpoles;

	/* determine (unwarped) -3 db frequency */
	*f3db = (float)(atan(0.5*w3db) / PI);
}

void
bfhighpass(int npoles, float f3db, int n, float p[], float q[])
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
	int jpair, j;
	float r, scale, theta, a, b1, b2, pj, pjm1, pjm2, qjm1, qjm2;

	r = (float)(2.0*tan(PI*fabs(f3db)));
	if (npoles % 2 != 0) {
		scale = r + 2.0f;
		a = 2.0f / scale;
		b1 = (r - 2.0f) / scale;
		pj = 0.0f;
		qjm1 = 0.0f;
		for (j = 0; j < n; j++) {
			pjm1 = pj;
			pj = p[j];
			q[j] = a*(pj - pjm1) - b1*qjm1;
			qjm1 = q[j];
		}
	}
	else {
		for (j = 0; j < n; j++)
			q[j] = p[j];
	}
	for (jpair = 0; jpair < npoles / 2; jpair++) {
		theta = (float)(PI*(2 * jpair + 1) / (2 * npoles));
		scale = (float)(4.0 + 4.0*r*sin(theta) + r*r);
		a = 4.0f / scale;
		b1 = (float)((2.0*r*r - 8.0) / scale);
		b2 = (float)((4.0 - 4.0*r*sin(theta) + r*r) / scale);
		pjm1 = 0.0f;
		pj = 0.0f;
		qjm2 = 0.0f;
		qjm1 = 0.0f;
		for (j = 0; j < n; j++) {
			pjm2 = pjm1;
			pjm1 = pj;
			pj = q[j];
			q[j] = (float)(a*(pj - 2.0*pjm1 + pjm2) - b1*qjm1 - b2*qjm2);
			qjm2 = qjm1;
			qjm1 = q[j];
		}
	}
}

void
bflowpass(int npoles, float f3db, int n, float p[], float q[])
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
	int jpair, j;
	float r, scale, theta, a, b1, b2, pj, pjm1, pjm2, qjm1, qjm2;

	r = (float)(2.0*tan(PI*fabs(f3db)));
	if (npoles % 2 != 0) {
		scale = r + 2.0f;
		a = (float)(r / scale);
		b1 = (float)((r - 2.0) / scale);
		pj = 0.0f;
		qjm1 = 0.0f;
		for (j = 0; j < n; j++) {
			pjm1 = pj;
			pj = p[j];
			q[j] = a*(pj + pjm1) - b1*qjm1;
			qjm1 = q[j];
		}
	}
	else {
		for (j = 0; j < n; j++)
			q[j] = p[j];
	}
	for (jpair = 0; jpair < npoles / 2; jpair++) {
		theta = (float)(PI*(2 * jpair + 1) / (2 * npoles));
		scale = (float)(4.0 + 4.0*r*sin(theta) + r*r);
		a = (float)(r*r / scale);
		b1 = (float)((2.0*r*r - 8.0) / scale);
		b2 = (float)((4.0 - 4.0*r*sin(theta) + r*r) / scale);
		pjm1 = 0.0f;
		pj = 0.0f;
		qjm2 = 0.0f;
		qjm1 = 0.0f;
		for (j = 0; j < n; j++) {
			pjm2 = pjm1;
			pjm1 = pj;
			pj = q[j];
			q[j] = (float)(a*(pj + 2.0*pjm1 + pjm2) - b1*qjm1 - b2*qjm2);
			qjm2 = qjm1;
			qjm1 = q[j];
		}
	}
}









//void setTraceDelayF(segy* tr, double timeDelay){
//	// will access segy headers:    short tr.delrt, int tr.t1
//	int iTimeDelay = (int)myRound(timeDelay * 1000);
//	if (iTimeDelay < MAXSHORT) {
//		// if the traditional unsigned short header can handle it, we use it
//		tr->delrt = iTimeDelay;
//		tr->delrtEx = 0;
//	}
//	else {
//		// if it cannot handle, we use our own extended header
//		tr->delrt = 0;
//		tr->delrtEx = iTimeDelay;
//	}
//
//	if (abs(tr->delrtEx) > 90000000) {
//		tr->delrtEx = 0;
//	}
//
//}
//
//void setTraceDelayF(segydynamic* tr, double timeDelay){
//	// will access segy headers:    short tr.delrt, int tr.t1
//	int iTimeDelay = (int)myRound(timeDelay * 1000);
//	if (iTimeDelay < MAXSHORT) {
//		tr->delrt = iTimeDelay;
//		tr->delrtEx = 0;
//	}
//	else {
//		tr->delrt = 0;
//		tr->delrtEx = iTimeDelay;
//	}
//
//	if (abs(tr->delrtEx) > 90000000.) {
//		tr->delrtEx = 0;
//	}
//
//}
//
//float getTraceDelay(segy * tr){
//	if (tr->delrtEx > 90000000.) tr->delrtEx = 0;
//
//	return (tr->delrt + tr->delrtEx)*.001f;
//}
//
//float getTraceDelay(segydynamic * trdyn){
//	// fix up first, since some programs ...
//	if (trdyn->delrtEx > 90000000.) trdyn->delrtEx = 0;
//
//	return (trdyn->delrt + trdyn->delrtEx)*.001f;
//}






//float getTraceXVal(segydynamic *tr, int drawmode, BOOL isDistance) {
//	if (drawmode == 6) return (float)tr->tracl;  // this is trace display
//
//	return (!isDistance) ? tr->getOffset() : tr->getDistance();
//}






// will return the total number of successful traces read into the array
// if return 0, meaning no traces read into the array       // not used
//int loadSegy2ArrayDelete(CString f, segydynamic *trArr, bhed *bh, BOOL isIBMFloatSegy) {
//	CFile file;
//	segy tr;
//	int j;
//	if (!file.Open(f, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
//		return 0;
//	}
//	file.Seek(3200, CFile::begin);
//	file.Read(bh, 400L);
//	int len;
//	if (bh->format != 3)
//		len = bh->hns*4 + 240;
//	else
//		len = bh->hns*2 + 240;
//	int numsamp = bh->hns;
//
//	int nxmax = 0;  // total number of traces in the input segy file
//	while (file.Read(&tr, 4) == (UINT)4)
//	{
//		file.Seek(len-4, CFile::current);
//		nxmax ++;
//	}
//	file.Seek(3600, CFile::begin);
//	trArr = (segydynamic *)malloc( sizeof(segydynamic) * nxmax );
//	for (j=0; j< nxmax; j++) {
//		trArr[j].data = (float *)malloc(sizeof(float)* numsamp);
//		if (ERROR == trArr[j].data) {
//			freeSegyArray(trArr, j);
//			AfxMessageBox(_T("Sorry, failed to allocate memory for loading all traces into memory."));
//			file.Close();
//			return 0;
//		}
//	} // for j
//
//	int nxStore = 0;
//	while (file.Read(&tr, len) == (UINT)len)
//	{
//		if(tr.trid == 2) //dead traces.
//		{
//			file.Seek(len, CFile::current); 
//			continue ;
//		}
//
//		// assign only the trace header to the trArr[]
//		memcpy( (void *)&trArr[nxStore], (void *)&tr, 240L);
//		if(bh->format==3)
//			for (j=0; j<numsamp; j++) trArr[nxStore].data[j] = (float)tr.dat.datashort[j];
//		else if (bh->format==1) {
//			if (tr.idistopt==8 || isIBMFloatSegy)
//				ibm_to_float((int *) tr.dat.data, (int *) tr.dat.data, (int)numsamp);
//			for (j=0; j<numsamp; j++) trArr[nxStore].data[j] = (float)tr.dat.data[j];
//		}
//		else if (bh->format==2) 
//			for (j=0; j<numsamp; j++) trArr[nxStore].data[j] = (float)tr.dat.dataint[j];
//		else break;
//
//		double aMedian = getMedianFast(&trArr[nxStore].data[0], numsamp);	
//		if (aMedian < MINAMP) continue;
//		nxStore++;
//	} // while file2.Read
//	file.Close();
//
//	// now remove any array elements not in use
//	if (nxStore < nxmax) {
//		for (j=nxStore; j<nxmax; j++) {
//			free(trArr[j].data); trArr[j].data = NULL;
//		} // for
//	} // if
//
//	return nxStore;
//}



bool setAppendToFileText(CString f, CString line)
{
	if (line.GetLength() == 0) return false;
	CFile file;
	if (!file.Open(f, CFile::modeReadWrite | CFile::modeNoTruncate))
		return false;
	file.SeekToEnd();
	file.Write("\r\n", 2);
	file.Write(line, (UINT)(line.GetLength()*sizeof(TCHAR)));
	file.Close();
	return true;
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
	if (isFileExistNotDirectory(f)) {
		// now load the contents of this ray parameter file
		if (_tfopen_s(&ifh, f, _T("r")) != 0) return false;
		bool isParamFound = false;
		while (_fgetts(line, 255, ifh) != NULL) {
			cs.Format(_T("%s"), line);
			if (cs.GetLength() <= 0) continue;

			i = cs.Find(param);
			if (i >= 0) {
				isParamFound = true;
				cs = cs.Mid(i + lenParam);
				cs = cs.Mid(cs.Find(_T('=')) + 1);

				// since we use comma as the deliminator, we want to make sure there is a comma at the end of the line. If not, we add one there
				i = cs.ReverseFind(',');
				if (i < cs.GetLength() - 2) {
					cs += ',';
				}


				//k = 0;
				for (i = 0; i < 100; i++) {
					cs.Trim();
					int iWordEnd = cs.FindOneOf(_T(", "));
					if (iWordEnd < 0) break;
					csDig = cs.Left(iWordEnd);
					csDig.Trim();
					if (csDig.GetLength() == 0) {
						cs = cs.Mid(1);
						continue;
					}
					sArr->Add(csDig);
					cs = cs.Mid(iWordEnd + 1);
				}
			} // if i
			else if (isParamFound) {
				// now test on the next line for any extra values
				i = cs.Find(_T("="));
				if (i < 0) {
					// since the second line does not contain an equal sign, it must be a continuation of param values
					for (i = 0; i < 100; i++) {
						int iWordEnd = cs.FindOneOf(_T(","));  //do not include the space!
						if (iWordEnd < 0) break;
						csDig = cs.Left(iWordEnd);
						csDig.Trim();
						if (csDig.GetLength() == 0) {
							cs = cs.Mid(1);
							continue;
						}
						sArr->Add(csDig);
						cs = cs.Mid(iWordEnd + 1);
					}
				}
				isParamFound = false;
			}
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

	for (l = 0; l < npt - lfilt + 1; l++) {
		yt = 0.0;
		for (i = 0; i < lfilt; i++) {
			yt += source[l + i] * filt[lfilt - 1 - i];
		}
		target[l] = yt;
	}

	/* shifting points */
	for (i = 0; i < npt - lfilt; i++) {
		target[i] = 0.5*(target[i] + target[i + 1]);
	}
	for (i = npt - lfilt - 1; i >= 0; i--)
		target[i + lfilt / 2] = target[i];

	/* writing zeros */
	for (i = 0; i < lfilt / 2; i++) {
		target[i] = 0.0;
		target[npt - 1 - i] = 0.0;
	}
}


// note: these arrays are all 0-based.
//     Before calling this, you need to assign all arrays of   ampl ,   phase,  omega , and later free them
//if (ampl == NULL) ampl = (double *)malloc( (size_t)( npt*sizeof(double)) );
//if (phase == NULL) phase = (double *)malloc( (size_t)( npt*sizeof(double)) );
//if (omega == NULL) omega = (double *)malloc( (size_t)( npt*sizeof(double)) );
bool hilbertTransform(float* x, double* ampl, double* phase, double *omega, int npt, int lfilt = 128)
{
	if (npt <= 0 || x == NULL || ampl == NULL || phase == NULL || omega == NULL) return false;

	double* xh = (double *)ealloc1double((size_t)(npt));
	double* hilb = (double *)ealloc1double((size_t)(lfilt));
	int i;
	double xt, xht;
	// double M_PI = 3.1415926;
	// static double pi = 3.1415926;
	static double opi = 1. / PI;
	static double pi2 = 2 * PI;
	static double opi2 = 1. / pi2;

	//prepare the pure convoluation filter as (1/pi*t)
	for (i = 0; i < lfilt; i++)
		hilb[i] = 1. / ((i - lfilt / 2) - 0.5)*opi;

	//Initialize 
	for (i = 0; i < npt; i++) {
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
	for (i = lfilt / 2; i < npt - lfilt / 2; i++) {
		xt = x[i];
		xht = xh[i];
		ampl[i] = sqrt(xt*xt + xht*xht);
		phase[i] = atan2(xht, xt);
		if (phase[i] < phase[i - 1])
			omega[i] = phase[i] - phase[i - 1] + pi2;
		else
			omega[i] = phase[i] - phase[i - 1];
	}

	for (i = 0; i < npt; i++) {
		omega[i] = omega[i] * opi2;
	}

	// some edge is trunctated
	for (i = 0; i < lfilt / 2 + 1; i++) {
		ampl[i] = phase[i] = omega[i] = 0.;
	}

	for (i = npt - lfilt / 2; i < npt; i++) {
		ampl[i] = phase[i] = omega[i] = 0.;
	}


	free1double(xh);
	free1double(hilb);
	return true;
}


/* prototype of function used internally */
static void convs(int, int, float*, int, int, float*, int, int, float*);

void conv(int lx, int ifx, float *x,
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
	int ilx = ifx + lx - 1, ily = ify + ly - 1, ilz = ifz + lz - 1,
		i, j, ilow, ihigh, jlow, jhigh;
	float sa, sb, xa, xb, ya, yb, *t;

	/* if x is longer than y, swap x and y */
	if (lx > ly) {
		i = ifx;  ifx = ify;  ify = i;
		i = ilx;  ilx = ily;  ily = i;
		i = lx;  lx = ly;  ly = i;
		t = x;  x = y;  y = t;
	}

	/* handle short x with special code */
	if (lx >= 1 && lx <= 30) {
		convs(lx, ifx, x, ly, ify, y, lz, ifz, z);
		return;
	}

	/* adjust pointers for indices of first samples */
	x -= ifx;
	y -= ify;
	z -= ifz;

	/* OFF LEFT:  i < ify+ifx */

	/* zero output for all i */
	ilow = ifz;
	ihigh = ify + ifx - 1;  if (ihigh > ilz) ihigh = ilz;
	for (i = ilow; i <= ihigh; ++i)
		z[i] = 0.0;

	/* ROLLING ON:  ify+ifx <= i < ify+ilx */

	/* if necessary, do one i so that number of j in overlap is odd */
	if (i < ify + ilx && i <= ilz) {
		jlow = ifx;
		jhigh = i - ify;
		if ((jhigh - jlow) % 2) {
			sa = 0.0;
			for (j = jlow; j <= jhigh; ++j)
				sa += x[j] * y[i - j];
			z[i++] = sa;
		}
	}

	/* loop over pairs of i and j */
	ilow = i;
	ihigh = ilx + ify - 1;  if (ihigh > ilz) ihigh = ilz;
	jlow = ifx;
	jhigh = ilow - ify;
	for (i = ilow; i < ihigh; i += 2, jhigh += 2) {
		sa = sb = 0.0;
		xb = x[jhigh + 1];
		yb = 0.0;
		for (j = jhigh; j >= jlow; j -= 2) {
			sa += xb*yb;
			ya = y[i - j];
			sb += xb*ya;
			xa = x[j];
			sa += xa*ya;
			yb = y[i + 1 - j];
			sb += xa*yb;
			xb = x[j - 1];
		}
		z[i] = sa;
		z[i + 1] = sb;
	}

	/* if number of i is odd */
	if (i == ihigh) {
		jlow = ifx;
		jhigh = i - ify;
		sa = 0.0;
		for (j = jlow; j <= jhigh; ++j)
			sa += x[j] * y[i - j];
		z[i++] = sa;
	}

	/* MIDDLE:  ify+ilx <= i <= ily+ifx */

	/* determine limits for i and j */
	ilow = i;
	ihigh = ily + ifx;  if (ihigh > ilz) ihigh = ilz;
	jlow = ifx;
	jhigh = ilx;

	/* if number of j is even, do j in pairs with no leftover */
	if ((jhigh - jlow) % 2) {
		for (i = ilow; i < ihigh; i += 2) {
			sa = sb = 0.0;
			yb = y[i + 1 - jlow];
			xa = x[jlow];
			for (j = jlow; j < jhigh; j += 2) {
				sb += xa*yb;
				ya = y[i - j];
				sa += xa*ya;
				xb = x[j + 1];
				sb += xb*ya;
				yb = y[i - 1 - j];
				sa += xb*yb;
				xa = x[j + 2];
			}
			z[i] = sa;
			z[i + 1] = sb;
		}

		/* else, number of j is odd, so do j in pairs with leftover */
	}
	else {
		for (i = ilow; i < ihigh; i += 2) {
			sa = sb = 0.0;
			yb = y[i + 1 - jlow];
			xa = x[jlow];
			for (j = jlow; j < jhigh; j += 2) {
				sb += xa*yb;
				ya = y[i - j];
				sa += xa*ya;
				xb = x[j + 1];
				sb += xb*ya;
				yb = y[i - 1 - j];
				sa += xb*yb;
				xa = x[j + 2];
			}
			z[i] = sa + x[jhigh] * y[i - jhigh];
			z[i + 1] = sb + x[jhigh] * y[i + 1 - jhigh];
		}
	}

	/* if number of i is odd */
	if (i == ihigh) {
		sa = 0.0;
		for (j = jlow; j <= jhigh; ++j)
			sa += x[j] * y[i - j];
		z[i++] = sa;
	}

	/* ROLLING OFF:  ily+ifx < i <= ily+ilx */

	/* if necessary, do one i so that number of j in overlap is even */
	if (i <= ily + ilx && i <= ilz) {
		jlow = i - ily;
		jhigh = ilx;
		if (!((jhigh - jlow) % 2)) {
			sa = 0.0;
			for (j = jlow; j <= jhigh; ++j)
				sa += x[j] * y[i - j];
			z[i++] = sa;
		}
	}

	/* number of j is now even, so loop over both i and j in pairs */
	ilow = i;
	ihigh = ily + ilx;  if (ihigh > ilz) ihigh = ilz;
	jlow = ilow - ily;
	jhigh = ilx - 2; /* Dave's new patch */
	for (i = ilow; i < ihigh; i += 2, jlow += 2) {
		sa = sb = 0.0;
		xa = x[jlow];
		yb = 0.0;
		for (j = jlow; j < jhigh; j += 2) {
			sb += xa*yb;
			ya = y[i - j];
			sa += xa*ya;
			xb = x[j + 1];
			sb += xb*ya;
			yb = y[i - 1 - j];
			sa += xb*yb;
			xa = x[j + 2];
		}
		sb += xa*yb;
		ya = y[i - j];
		sa += xa*ya;
		xb = x[j + 1];
		sb += xb*ya;
		yb = y[i - 1 - j];
		sa += xb*yb;
		z[i] = sa;
		z[i + 1] = sb;
	}

	/* if number of i is odd */
	if (i == ihigh) {
		jlow = i - ily;
		jhigh = ilx;
		sa = 0.0;
		for (j = jlow; j <= jhigh; ++j)
			sa += x[j] * y[i - j];
		z[i++] = sa;
	}

	/* OFF RIGHT:  ily+ilx < i */

	/* zero output for all i */
	ilow = i;
	ihigh = ilz;
	for (i = ilow; i <= ihigh; ++i)
		z[i] = 0.0;
}

/* internal function optimized for short x */
void convs(int lx, int ifx, float *x,
	int ly, int ify, float *y,
	int lz, int ifz, float *z)
{
	int ilx = ifx + lx - 1, ily = ify + ly - 1, ilz = ifz + lz - 1,
		i, j, ilow, ihigh, jlow, jhigh;
	float x0, x1, x2, x3, x4, x5, x6, x7, x8, x9,
		x10, x11, x12, x13, x14, x15, x16, x17, x18, x19,
		x20, x21, x22, x23, x24, x25, x26, x27, x28, x29,
		ya, yb, z0, z1, sum;

	x -= ifx;
	y -= ify;
	z -= ifz;


	/* OFF LEFT:  i < ifx+ify */
	ilow = ifz;
	ihigh = ify + ifx - 1;  if (ihigh > ilz) ihigh = ilz;
	for (i = ilow; i <= ihigh; ++i)
		z[i] = 0.0;

	/* ROLLING ON:  ify+ifx <= i < ify+ilx */
	ilow = ify + ifx;  if (ilow < ifz) ilow = ifz;
	ihigh = ify + ilx - 1;  if (ihigh > ilz) ihigh = ilz;
	jlow = ifx;
	jhigh = ilow - ify;
	for (i = ilow; i <= ihigh; ++i, ++jhigh) {
		for (j = jlow, sum = 0.0; j <= jhigh; ++j)
			sum += x[j] * y[i - j];
		z[i] = sum;
	}

	/* MIDDLE:  ify+ilx <= i <= ily+ifx */
	ilow = ify + ilx;  if (ilow < ifz) ilow = ifz;
	ihigh = ily + ifx;  if (ihigh > ilz) ihigh = ilz;
	if (lx == 1) {
		x0 = x[ifx];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 2) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 3) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 4) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 5) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 6) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 7) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 8) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 9) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 10) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 11) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 12) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 13) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 14) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 15) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 16) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 17) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 18) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 19) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 20) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 21) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 22) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 23) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 24) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 25) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		x24 = x[ifx + 24];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i - ifx - 24];  z0 += x24*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 26) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		x24 = x[ifx + 24];
		x25 = x[ifx + 25];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i - ifx - 24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i - ifx - 25];  z0 += x25*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 27) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		x24 = x[ifx + 24];
		x25 = x[ifx + 25];
		x26 = x[ifx + 26];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i - ifx - 24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i - ifx - 25];  z0 += x25*ya;  z1 += x26*ya;
			yb = y[i - ifx - 26];  z0 += x26*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 28) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		x24 = x[ifx + 24];
		x25 = x[ifx + 25];
		x26 = x[ifx + 26];
		x27 = x[ifx + 27];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i - ifx - 24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i - ifx - 25];  z0 += x25*ya;  z1 += x26*ya;
			yb = y[i - ifx - 26];  z0 += x26*yb;  z1 += x27*yb;
			ya = y[i - ifx - 27];  z0 += x27*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 29) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		x24 = x[ifx + 24];
		x25 = x[ifx + 25];
		x26 = x[ifx + 26];
		x27 = x[ifx + 27];
		x28 = x[ifx + 28];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i - ifx - 24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i - ifx - 25];  z0 += x25*ya;  z1 += x26*ya;
			yb = y[i - ifx - 26];  z0 += x26*yb;  z1 += x27*yb;
			ya = y[i - ifx - 27];  z0 += x27*ya;  z1 += x28*ya;
			yb = y[i - ifx - 28];  z0 += x28*yb;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	else if (lx == 30) {
		x0 = x[ifx];
		x1 = x[ifx + 1];
		x2 = x[ifx + 2];
		x3 = x[ifx + 3];
		x4 = x[ifx + 4];
		x5 = x[ifx + 5];
		x6 = x[ifx + 6];
		x7 = x[ifx + 7];
		x8 = x[ifx + 8];
		x9 = x[ifx + 9];
		x10 = x[ifx + 10];
		x11 = x[ifx + 11];
		x12 = x[ifx + 12];
		x13 = x[ifx + 13];
		x14 = x[ifx + 14];
		x15 = x[ifx + 15];
		x16 = x[ifx + 16];
		x17 = x[ifx + 17];
		x18 = x[ifx + 18];
		x19 = x[ifx + 19];
		x20 = x[ifx + 20];
		x21 = x[ifx + 21];
		x22 = x[ifx + 22];
		x23 = x[ifx + 23];
		x24 = x[ifx + 24];
		x25 = x[ifx + 25];
		x26 = x[ifx + 26];
		x27 = x[ifx + 27];
		x28 = x[ifx + 28];
		x29 = x[ifx + 29];
		for (i = ilow; i <= ihigh - 1; i += 2) {
			ya = y[i + 1 - ifx];  z1 = x0*ya;
			yb = y[i - ifx];  z0 = x0*yb;  z1 += x1*yb;
			ya = y[i - ifx - 1];  z0 += x1*ya;  z1 += x2*ya;
			yb = y[i - ifx - 2];  z0 += x2*yb;  z1 += x3*yb;
			ya = y[i - ifx - 3];  z0 += x3*ya;  z1 += x4*ya;
			yb = y[i - ifx - 4];  z0 += x4*yb;  z1 += x5*yb;
			ya = y[i - ifx - 5];  z0 += x5*ya;  z1 += x6*ya;
			yb = y[i - ifx - 6];  z0 += x6*yb;  z1 += x7*yb;
			ya = y[i - ifx - 7];  z0 += x7*ya;  z1 += x8*ya;
			yb = y[i - ifx - 8];  z0 += x8*yb;  z1 += x9*yb;
			ya = y[i - ifx - 9];  z0 += x9*ya;  z1 += x10*ya;
			yb = y[i - ifx - 10];  z0 += x10*yb;  z1 += x11*yb;
			ya = y[i - ifx - 11];  z0 += x11*ya;  z1 += x12*ya;
			yb = y[i - ifx - 12];  z0 += x12*yb;  z1 += x13*yb;
			ya = y[i - ifx - 13];  z0 += x13*ya;  z1 += x14*ya;
			yb = y[i - ifx - 14];  z0 += x14*yb;  z1 += x15*yb;
			ya = y[i - ifx - 15];  z0 += x15*ya;  z1 += x16*ya;
			yb = y[i - ifx - 16];  z0 += x16*yb;  z1 += x17*yb;
			ya = y[i - ifx - 17];  z0 += x17*ya;  z1 += x18*ya;
			yb = y[i - ifx - 18];  z0 += x18*yb;  z1 += x19*yb;
			ya = y[i - ifx - 19];  z0 += x19*ya;  z1 += x20*ya;
			yb = y[i - ifx - 20];  z0 += x20*yb;  z1 += x21*yb;
			ya = y[i - ifx - 21];  z0 += x21*ya;  z1 += x22*ya;
			yb = y[i - ifx - 22];  z0 += x22*yb;  z1 += x23*yb;
			ya = y[i - ifx - 23];  z0 += x23*ya;  z1 += x24*ya;
			yb = y[i - ifx - 24];  z0 += x24*yb;  z1 += x25*yb;
			ya = y[i - ifx - 25];  z0 += x25*ya;  z1 += x26*ya;
			yb = y[i - ifx - 26];  z0 += x26*yb;  z1 += x27*yb;
			ya = y[i - ifx - 27];  z0 += x27*ya;  z1 += x28*ya;
			yb = y[i - ifx - 28];  z0 += x28*yb;  z1 += x29*yb;
			ya = y[i - ifx - 29];  z0 += x29*ya;
			z[i + 1] = z1;
			z[i] = z0;
		}
	}
	if (ihigh >= ilow && (ihigh - ilow) % 2 == 0) {
		ilow = ihigh;
		jlow = ifx;
		jhigh = ilx;
		for (i = ilow; i <= ihigh; ++i) {
			for (j = jlow, sum = 0.0; j <= jhigh; ++j)
				sum += x[j] * y[i - j];
			z[i] = sum;
		}
	}

	/* ROLLING OFF:  ily+ifx < i <= ily+ilx */
	ilow = ily + ifx + 1;  if (ilow<ifz) ilow = ifz;
	ihigh = ily + ilx;  if (ihigh>ilz) ihigh = ilz;
	jlow = ilow - ily;
	jhigh = ilx;
	for (i = ilow; i <= ihigh; ++i, ++jlow) {
		for (j = jlow, sum = 0.0; j <= jhigh; ++j)
			sum += x[j] * y[i - j];
		z[i] = sum;
	}

	/* OFF RIGHT:  ily+ilx < i */
	ilow = ily + ilx + 1;  if (ilow < ifz) ilow = ifz;
	ihigh = ilz;
	for (i = ilow; i <= ihigh; ++i)
		z[i] = 0.0;
}
#endif

void xcor(int lx, int ifx, float *x,
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
	int i, j;
	float *xr;

	xr = alloc1float(lx);
	for (i = 0, j = lx - 1; i < lx; ++i, --j)
		xr[i] = x[j];
	conv(lx, 1 - ifx - lx, xr, ly, ify, y, lz, ifz, z);
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
	} while (bFinish == TRUE);

	// End of search
	oFileFinder.Close();

	// now not find. So we just return the same
	return;
}

bool SearchReplaceAllFiles(CString strFolderPath, CString strTarget, CString strReplace, CString strCondition)
{
	if (strFolderPath.IsEmpty() || strTarget.IsEmpty()) return false; // if found, stop all searching, remember: this is a recursive search!

	//CString baseFileFull;
	//baseFileFull.Format(_T("\\%s.%s"), GetBaseFileName(targetFileName), GetFileExtension(targetFileName));

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
				SearchReplaceAllFiles(strNewFolderPath, strTarget, strReplace, strCondition);
			}

			// It is a file
			else
			{
				// Store path and name of file  oFileFinder.GetFilePath()
				CString t = oFileFinder.GetFilePath();

				if ((strCondition.IsEmpty() && GetBaseFileName(t).Find(strTarget) >= 0)
					||
					(!strCondition.IsEmpty() && GetBaseFileName(t).Find(strCondition) < 0)
					) {
					CString tNew = t;
					if (tNew.Replace(strTarget, strReplace)) {
						MoveFileEx(t, tNew, MOVEFILE_REPLACE_EXISTING);
					}
				}
			}
		}
	} while (bFinish == TRUE);

	// End of search
	oFileFinder.Close();

	// now not find. So we just return the same
	return true;

}

void SearchAllFilesXTemp(CString& strFolderPath, CString& targetFileName, CString& outFileName)
{
	if (outFileName.IsEmpty()) return; // if found, stop all searching, remember: this is a recursive search!

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
				if (!isEndWith(strNewFolderPath, _T("\\temp"))) {
					// Append characters to string
					strNewFolderPath += _T("\\*.*");

					// Recurse function
					SearchAllFilesXTemp(strNewFolderPath, targetFileName, outFileName);
				}
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
	} while (bFinish == TRUE);

	// End of search
	oFileFinder.Close();

	// now not find. So we just return the same
	return;
}


/*
This function does not consider quoted values. If, for exmaple, a CSV file is comma delimited, 
but a quoted string contains comma, this function cannot solve that.
----This is solved by subsequently calling conditionArrQuotes() !!!

Calling this function is really easy - just create a CStringArray, and pass it in something like this:

CStringArray cStringArray;
int nElementCount = SplitString("22|37|egg|234|17", _T("|"), cStringArray);

In this example, nElementCount will be 5, using a delimiter of "|", and the cStringArray will have 5 elements:
22
37
egg
234
17
*/
bool SplitStringArr(CString strInputString, const CString& strDelimiter, CStringArray* arrStringArray, bool isKeepDoubleSpace)
{
	// note: if the delimiter is a space, we want to merge all consecutive space into one space.
	// but if it is not a space (e.g. a comma), we do not want to do it.
	strInputString.Replace(TAB, SPACE);

	// note: sometimes a double space cannot be removed for a proper work.
	if (!isKeepDoubleSpace) {
		while (strInputString.Find(SPACE2) >= 0)
			strInputString.Replace(SPACE2, SPACE);
	}

	int iPos = -1;  // oritinally this is 0, but it has prob when the first pos is 0 -- Deping
	int newPos = -1;
	int sizeS2 = strDelimiter.GetLength();
	int isize = strInputString.GetLength();

	CArray<INT, int> positions;

	newPos = strInputString.Find(strDelimiter, 0);

	if (newPos < 0) {
		// only 1 item exist
		arrStringArray->Add(strInputString);
		return true;
	}

	int numFound = 0;

	while (newPos > iPos) {
		numFound++;
		positions.Add(newPos);
		iPos = newPos;

		// deping addition, to treat successive spaces as one space
		//while (strInputString.Mid(iPos, sizeS2).Compare(strDelimiter) == 0)
		//	iPos += sizeS2;
		newPos = strInputString.Find(strDelimiter, iPos + sizeS2);
	}
	if (positions.GetSize() < 1) return false;

	arrStringArray->RemoveAll();
	for (int i = 0; i <= positions.GetSize(); i++) {
		CString s;
		if (i == 0)
			s = strInputString.Mid(i, positions[i]);
		else {
			int offset = positions[i - 1] + sizeS2;
			if (offset < isize) {
				if (i == positions.GetSize())
					s = strInputString.Mid(offset);
				else if (i > 0)
					s = strInputString.Mid(positions[i - 1] + sizeS2, positions[i] - positions[i - 1] - sizeS2);
			}
		}
		s.Trim();
		if (s.GetLength() < 1) continue;
		if (s.Find(_T("#")) != 0)
			arrStringArray->Add(s);
	}

	if (strDelimiter == QUOTE || strInputString.Find(QUOTE) < 0) return true; // since no quotes existing, we can simply use arFields.
	CStringArray 	*arFields2 = new CStringArray();
	conditionArrQuotes(arrStringArray, strDelimiter, arFields2);
	arrStringArray->RemoveAll();
	for (int i = 0; i < arFields2->GetCount(); i++) {
		arrStringArray->Add(arFields2->ElementAt(i));
	}
	arFields2->RemoveAll();
	delete arFields2;

	return true;
}


// Good routine. Fixed some issues
int SplitString(CString strInputString, const CString& strDelimiter)
{
	CStringArray *arr = new CStringArray();
	SplitStringArr(strInputString, strDelimiter, arr, false);
	int numFound = (int)arr->GetCount();
	arr->RemoveAll();
	delete arr;
	return numFound;
}

CStringArray* CString2LinesBySpace(CString& str) {
	CStringArray *arr = new CStringArray();
	SplitStringArr(str, SPACE, arr, false);
	return arr;

	//return MyTokenizer(str, SPACE);
}


// this routine has problem of adding extra empty stuff.
CStringArray* MyTokenizer_prob(const CString& str, const CString& strDelimiters)
{
	CStringArray 	*arFields = new CStringArray();

	// Do not process empty strings.
	if (!str.IsEmpty() && !strDelimiters.IsEmpty()) {
		int nPosition = 0, nTotalFields = 0;

		do {
			int nOldPosition = nPosition;   // Store the previous position value.

			CString strField = str.Tokenize(strDelimiters, nPosition);
			if (nPosition != -1)
				nTotalFields += (nPosition - nOldPosition - strField.GetLength());
			else
				nTotalFields += (str.GetLength() + 1 - nOldPosition);

			// By using SetAtGrow(), empty strings are automatically added to the array.
			arFields->SetAtGrow(nTotalFields - 1, strField);
		} while (nPosition != -1 && nPosition <= str.GetLength());
	}

	// a catch: if an element contains one quote only, we need to find the other quote, and reduce the  string in between
	if (strDelimiters == QUOTE || str.Find(QUOTE) < 0) return arFields; // since no quotes existing, we can simply use arFields.
	CStringArray 	*arFields2 = new CStringArray();
	conditionArrQuotes(arFields, strDelimiters, arFields2);
	arFields->RemoveAll();
	delete arFields;
	return arFields2;
}

void conditionArrQuotes(CStringArray* in, CString strDelimiters, CStringArray* out) {
	out->RemoveAll();
	int k;
	if (strDelimiters == QUOTE) {
		for (int i = 0; i < in->GetSize(); i++) 
			out->Add(in->ElementAt(i));
	}
	else {
		for (int i = 0; i < in->GetSize(); i++) {
			register CString aline = in->ElementAt(i).Trim();
			if (aline.Find(QUOTE) == 0 && aline.Find(QUOTE, 1) < 0) {
				// now need to find the other quots
				register CString alineConcat = aline;
				for (int j = i + 1; j < in->GetSize(); j++) {
					register CString aline2 = in->ElementAt(j); // note: do not trim here since it is normal to have space prepended
					alineConcat += strDelimiters + aline2;
					i++;
					k = aline2.Find(QUOTE);
					if (k == 0 || (k>0 && aline2.GetAt(k-1)!= _T('\"')))  // a double quote would be treated as a regular character
						break;
				}
				out->Add(alineConcat);
			}
			else
				out->Add(aline);
		}
	}
}

CStringArray* CString2LinesByComma(CString& str) {
	CStringArray *arr = new CStringArray();
	SplitStringArr(str, COMMA, arr, false);
	return arr;

	//return MyTokenizer(str, _T(","));
}

// This function skips empty items, which I do not want
//CStringArray* CString2LinesByComma(CString str) {
//	CStringArray 	*m_arr = new CStringArray();
//
//	int i = 0;
//	for (
//		CString sItem = str.Tokenize(_T(","), i);
//		i >= 0;
//		sItem = str.Tokenize(_T(","), i))
//	{
//		m_arr->Add(sItem);
//	}
//
//	return m_arr;
//}


BOOL IsNumeric(const CString Str)
{
	// Make a copy of the original string to test it
	CString WithoutSeparator = Str;
	// Prepare to test for a natural number
	// First remove the decimal separator, if any
	//WithoutSeparator.Replace(_T("."), _T(""));
	//WithoutSeparator.Replace(_T("-"), _T(""));  // a sign should be removed

	// If this number were natural, test it
	// If it is not even a natural number, then it can't be valid
	if (IsNatural(WithoutSeparator) == FALSE)
		return FALSE; // Return Invalid Number

	// Set a counter to 0 to counter the number of decimal separators
	int NumberOfSeparators = 0;

	// Check each charcter in the original string
	for (int i = 0; i < Str.GetLength(); i++)
	{
		// If you find a decimal separator, count it
		if (Str[i] == _T('.'))
			NumberOfSeparators++;
	}

	// After checking the string and counting the decimal separators
	// If there is more than one decimal separator,
	// then this cannot be a valid number
	if (NumberOfSeparators > 1)
		return FALSE; // Return Invalid Number
	else // Otherwise, this appears to be a valid decimal number
		return TRUE;
}



// this function returns the Fz gravity contribution from one side of a polygon cross-section, 
// without multiplification of G*Rou
//c     sigma = density contrast (grams/cc) -- format f7.4
//c     n     = # of coordinates in polygon -- format i5
//c     r(i)  = x-axis coordinate (km) ------- format f9.3  note: it was f8.3....deping
//c     z(i)  = z-axis coordinate (km) ------- format f8.3  note: it was f8.3....deping
//C  F = G m1 m2 / r^2
//C     G = 6.67384*10^(-11)
// NOT USED
float getGravFromOneSideOfPolygonNot(float xi, float zi, float xj, float zj, float Y1, float Y2)
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

	double R1i = sqrt(ri2 + Y12);
	double R1j = sqrt(rj2 + Y12);
	double R2i = sqrt(ri2 + Y22);
	double R2j = sqrt(rj2 + Y22);

	double K = a / c;

	double result = T*log(rj2) - S*log(ri2)
		+ 2 * K*atan(T / K) - 2 * K*atan(S / K)
		+ S*log((Y1 + R1i)*(Y2 + R2i))
		- T*log((Y1 + R1j)*(Y2 + R2j))
		+ Y1 / c*log((c*S + R1i) / (c*T + R1j))
		+ Y2 / c*log((c*S + R2i) / (c*T + R2j))
		+ K*atan((a2 + Y12 + Y1*R1j) / (z0*T))
		+ K*atan((a2 + Y22 + Y2*R2j) / (z0*S))
		- K*atan((a2 + Y12 + Y1*R1i) / (z0*S))
		- K*atan((a2 + Y22 + Y2*R2j) / (z0*S));


	return 0.f;
}


/*
subroutine gz_poly(xs, zs, nsln, xv, zv, nvert, den, grav_z)
C Computers the vertical component of gravitational accesleration due to a
c polygon in a 2D space (x, z), or equivalently, due to an
c  infinetely-long polygonal cylinder striking in the Y direction in a
c 3D space (x,y,z).
c from: Won and Bevis (1987)
c ARGUMENTS:
c xs, zs (sent) vectors containing the station coordinates
c		(first station is at (xs(1), zs(1),) et al.)
c nstn (sent)	the number of stations
c XV, ZV (sent) vectors containing the coord of the polygon vertices in clockwise sequence
c nvert (sent)	the num of vertices (or sides) in the polygon
c den (sent) 	the polygon density
c grav_z (output)	vector containing z-component of gravitational
c               acceleration due to the polygon at each of the stations
c If tne routlne lS used to model one station per Call (ie. nsttn=1) then
c xs,zs and grav_z can be (undimensioned) variables rather than vectors.

c Units:
c  the system of units employed can be mod1fied by adjusting the value
c  assigned below to the parameter "con". If the chosen units for
c  gravity, length and density differ from the associated SI units by
c  factors C1, C2 and C3 respectively, then tne parameter "con" should be
c  set to (C1*C3/C1)*13.3464d-ll. Examples are given in the follow1ng
c  table:
c
c  Density Units    Gravity Units    Length Units    Con
c   gm/cm**3        mgals				km		13.3464d+0
c   gm/cm**3        mgals				m		13.3464d-3
c   gm/cm**3        mgals				kft		4.0680d+0
c   kg/m**3         mm/s**2				m		13.3464d-8

c SIGN CONVENTION:

c The z-axis is positive downwards, the x-ax1s is positive to the right.

Implicit real*8(a-h,o-z)
real*8 xv(nvert), zv(nvert),xs(nstn),zs(nstn),grav_z(nstn)
c--------------------------------------------------------------------------
parameter ( con=13.3464d0 )  ! edit this line to adJust units
c--------------------------------------------------------------------------
parameter ( pi=3.141592654d0, two_pi=2.0d0*pi)

*/

double gz_poly_norm_diff_side(double x1, double x2, double z1, double z2)
{
	// xv[] and zv[] must already be deducted the station coords
	static const double two_pi = 2.0*PI;

	if (x1 == 0. && z1 == 0.) return 0.;
	if (x2 == 0. && z2 == 0.) return 0.;

	double th1 = atan2(z1, x1);  // if use atan(), when x1==0, z1/x1 has problem!
	double th2 = atan2(z2, x2);

	if (!SAME_SIGN(z1, z2)) {
			register double test = x1 * z2 - x2 * z1;
			if (test > 0. && z1 >= 0)
				th2 += two_pi;
			else if (test < 0. && z2 >= 0.)
				th1 += two_pi;
			else if (test == 0.) return 0.;
	}

	double z21 = z2 - z1;
	double x21 = x2 - x1;
	double xz1 = x1 * x1 + z1 * z1;
	double xz2 = x2 * x2 + z2 * z2;

	register double gz = 0.;
	if (x21 == 0.)
		gz = 0.5 *x1 * log(xz2 / xz1);
	else
		gz = x21*(x1 * z2 - x2 * z1) *
		(th1 - th2 + 0.5*(z21 / x21)*log(xz2 / xz1)) /
		(x21*x21 + z21*z21);

	return gz;
}

// Output of gz_poly_norm_diff_close() will need to be multipled by 13.3464*density
double gz_poly_norm_diff_close(double *xv, double *zv, int nvert)
{
	// note: den can be negative since it may already has debucted 3.1
	// xv[] and zv[] must already be deducted the station coords
	if (nvert < 4) return false;
	double grav = 0.0;
	for (int ic = 0; ic < nvert; ic++) {
		grav += gz_poly_norm_diff_side(xv[ic], xv[ic + 1], zv[ic], zv[ic + 1]);
	} // for ic

	return grav;
}

// Output of gz_poly_norm_diff() will need to be multipled by 13.3464*density
double gz_poly_norm_diff_open(double *xv, double *zv, int nvert)
{
	// note: den can be negative since it may already has debucted 3.1
	// xv[] and zv[] must already be deducted the station coords
	if (nvert < 4) return false;
	static double two_pi = 2.0*PI;
	register double grav = 0.0;

	for (int ic = 0; ic < nvert; ic++) {
		register double gz = 0.;

		// register double x1 = xv[ic];  // translate origin
		// register double z1 = zv[ic];
		if (xv[ic] == 0. && zv[ic] == 0.) continue;

		register double x2, z2;
		if (ic == nvert - 1) {
			x2 = xv[0];
			z2 = zv[0];
		}
		else {
			x2 = xv[ic + 1];
			z2 = zv[ic + 1];
		}
		if (x2 == 0. && z2 == 0.) continue;

		register double th1 = atan2(zv[ic], xv[ic]);  // if use atan(), when x1==0, z1/x1 has problem!
		register double th2 = atan2(z2, x2);

		if (!SAME_SIGN(zv[ic], z2)) {
			register double test = xv[ic] * z2 - x2*zv[ic];
			if (test > 0. && zv[ic] >= 0)
				th2 += two_pi;
			else if (test < 0. && z2 >= 0.)
				th1 += two_pi;
			else if (test == 0.) continue;
		}

		// register double t12 = th1 - th2;
		register double z21 = z2 - zv[ic];
		register double x21 = x2 - xv[ic];
		register double xz1 = xv[ic] * xv[ic] + zv[ic] * zv[ic];
		register double xz2 = x2*x2 + z2*z2;
		if (x21 == 0.)
			gz = 0.5 *xv[ic] * log(xz2 / xz1);
		else
			gz = x21*(xv[ic] * z2 - x2*zv[ic]) *
			(th1 - th2 + 0.5*(z21 / x21)*log(xz2 / xz1)) /
			(x21*x21 + z21*z21);

		grav += gz;
	} // for ic

	return grav;
}


// Output of gz_poly_norm() will need to be multipled by 13.3464*density
//double gz_poly_norm(double x, double z, double *xv, double *zv, int nvert)
//{
//	// note: den can be negative since it may already has debucted 3.1
//	if (nvert < 4) return false;
//	static double two_pi = 2.0*PI;
//	register double grav = 0.0;
//
//	for (int ic = 0; ic < nvert; ic++) {
//		register double gz = 0.;
//
//		register double x1 = xv[ic] - x;  // translate origin
//		register double z1 = zv[ic] - z;
//		if (x1 == 0. && z1 == 0.) continue;
//
//		register double x2, z2;
//		if (ic == nvert - 1) {
//			x2 = xv[0] - x;
//			z2 = zv[0] - z;
//		}
//		else {
//			x2 = xv[ic + 1] - x;
//			z2 = zv[ic + 1] - z;
//		}
//		if (x2 == 0. && z2 == 0.) continue;
//
//		register double th1 = atan2(z1, x1);  // if use atan(), when x1==0, z1/x1 has problem!
//		register double th2 = atan2(z2, x2);
//
//		if (!sameSign((float)z1, (float)z2)) {
//			register double test = x1*z2 - x2*z1;
//			if (test > 0. && z1 >= 0)
//				th2 += two_pi;
//			else if (test < 0. && z2 >= 0.)
//				th1 += two_pi;
//			else if (test == 0.) continue;
//		}
//
//		// register double t12 = th1 - th2;
//		register double z21 = z2 - z1;
//		register double x21 = x2 - x1;
//		register double xz1 = x1*x1 + z1*z1;
//		register double xz2 = x2*x2 + z2*z2;
//		if (x21 == 0.)
//			gz = 0.5 *x1*log(xz2 / xz1);
//		else
//			gz = x21*(x1*z2 - x2*z1) *
//			(th1 - th2 + 0.5*(z21 / x21)*log(xz2 / xz1)) /
//			(x21*x21 + z21*z21);
//
//		grav += gz;
//	} // for ic
//
//	return grav;
//}

bool gz_poly(double *xs, double *zs, int nstn, double *xv, double *zv, double xvTmp[], double zvTmp[], 
	int nvert, double den, double *grav_z, bool isOutputReset)
{
	// note: den can be negative since it may already has debucted 3.1
	if (nstn < 1 || nvert < 4) return false;

	static double con = 13.3464; //edit this line to adjust units
	// static double two_pi = 2.0*PI;

	// if isOutputReset=true, we needed to first set the output arrays to zero.
	// This is for convenience. When you first call this routine, we may use true,
	// and all subsequent call to this routine should use false
	if (isOutputReset) {
		memset(grav_z, (int)'\0', nstn*sizeof(double));
	}

	for (int is = 0; is < nstn; is++) {
		for (int iv = 0; iv < nvert; iv++) {
			xvTmp[iv] = xv[iv] - xs[is];
			zvTmp[iv] = zv[iv] - zs[is];
		}
		if (xs[is] > 341.) {
			int das = 1;
		}
		grav_z[is] += con*den*gz_poly_norm_diff_open(xvTmp, zvTmp, nvert);
	} // for is

	return true;
}

bool gz_poly_old(double *xs, double *zs, int nstn, double *xv, double *zv, int nvert, double den, double *grav_z, bool isOutputReset)
{
	// note: den can be negative since it may already has debucted 3.1
	if (nstn < 1 || nvert < 4) return false;

	static double con = 13.3464; //edit this line to adjust units
	// static double pi = 3.141592654; 
	static double two_pi = 2.0*PI;

	// if isOutputReset=true, we needed to first set the output arrays to zero.
	// This is for convenience. When you first call this routine, we may use true,
	// and all subsequent call to this routine should use false
	if (isOutputReset) {
		memset(grav_z, (int)'\0', nstn*sizeof(double));
	}

	for (int is = 0; is < nstn; is++) {
		//	register double xst = xs[is];
		//	register double zst = zs[is];
		register double grav = 0.0;

		for (int ic = 0; ic < nvert; ic++) {
			register double gz = 0.;

			register double x1 = xv[ic] - xs[is];  // translate origin
			register double z1 = zv[ic] - zs[is];
			if (x1 == 0. && z1 == 0.) continue;

			register double x2, z2;
			if (ic == nvert - 1) {
				x2 = xv[0] - xs[is];
				z2 = zv[0] - zs[is];
			}
			else {
				x2 = xv[ic + 1] - xs[is];
				z2 = zv[ic + 1] - zs[is];
			}
			if (x2 == 0. && z2 == 0.) continue;

			register double th1 = atan2(z1, x1);  // if use atan(), when x1==0, z1/x1 has problem!
			register double th2 = atan2(z2, x2);

			if (!SAME_SIGN(z1, z2)) {
				register double test = x1*z2 - x2*z1;
				if (test > 0. && z1 >= 0)
					th2 += two_pi;
				else if (test < 0. && z2 >= 0.)
					th1 += two_pi;
				else if (test == 0.) continue;
			}

			// register double t12 = th1 - th2;
			register double z21 = z2 - z1;
			register double x21 = x2 - x1;
			register double xz1 = x1*x1 + z1*z1;
			register double xz2 = x2*x2 + z2*z2;
			if (x21 == 0.)
				gz = 0.5 *x1*log(xz2 / xz1);
			else
				gz = x21*(x1*z2 - x2*z1) *
				(th1 - th2 + 0.5*(z21 / x21)*log(xz2 / xz1)) /
				(x21*x21 + z21*z21);

			grav += gz;
		} // for ic

		grav_z[is] += con*den*grav;
	} // for is

	return true;
}




/* is target point inside a 2D polygon? */
/* will return 1=inside, 0=outside                */
bool is_inPoly(float *digx,				/* dig.x[][0] is the x points in polygon */
	float *digy,				/* dig.t[][0] is the y points in polygon */
	int npoints,                       /*   number of points in polygon        */
	float xt,                   /*   x (horizontal) of target point     */
	float yt)                   /*   y (vertical) of target point       */
{
	float xnew, ynew;
	float xold, yold;
	float x1, y1;
	float x2, y2;
	int i;
	bool inside = 0;

	if (npoints < 3) {
		return inside;
	}
	xold = digx[npoints - 1];
	yold = digy[npoints - 1];
	for (i = 0; i < npoints; i++) {
		xnew = digx[i];
		ynew = digy[i];
		if (xnew > xold) {
			x1 = xold;
			x2 = xnew;
			y1 = yold;
			y2 = ynew;
		}
		else {
			x1 = xnew;
			x2 = xold;
			y1 = ynew;
			y2 = yold;
		}
		if ((xnew < xt) == (xt <= xold)          /* edge "open" at one end */
			&& (yt - y1)*(x2 - x1) < (y2 - y1)*(xt - x1)) {
			inside = !inside;
		}
		xold = xnew;
		yold = ynew;
	}
	return inside;
}


/*
Computes ther magnetic anomaly at one or more stations due to an infinite
polygonal cylinder magnetized by the earth's magnetic field. The
cylinderstrikes parallel to the Y-axis,  and has a polygonal cross-
section in the X,Z plan. The anomalous magnetic field strength depends on
X and Z, out not on Y

from: Won and Bevis (1987)\ARGUMENTS:
c xs, es  (sent)  Vectors containing the styation coordinates
c nstn  (sent)  The number of !>tat Ions
c xv, zv (sent) vectors containing the coord of the polygon vertices in clockwise sequence
as viewed looking towards negative Y-axis.
c nvert (sent)	the num of vertices (or sides) in the polygon
c he (sent) 	The earth's total magnetic field strength
c anginc   the inclination of the earth's field (degrees)
c angstr   The strike of the cylinder (in degrees) measured counter-clockwise (looking down)
from magnetic north to the negative Y-axis
c suscept	The magnetic susceptibility of the cylinder
in emu, or four pi times the magnetic susceptibility given in SI units
c anom_z (output)	A vector containing the vertical magnetic anomaly at each station
c anom_x (output)	A vector containing the horizontal magnetic anomaly at each station
c anom_t (output)	A vector containing the total magnetic anomaly at each station

c If tne routlne lS used to model one station per Call (ie. nsttn=1) then
c xs,zs and grav_z can be (undimensioned) variables rather than vectors.

SIGN CONVENTION:
A right-handed coordinate system is assumed. Z values are positive downwards. The X- and
Z-components of the anomalous field strength are positive if these compoennt fields point in the
positive X and Z directions respectively, and nagative otherwise. The total magnetic
anomaly is positive if it reinforces the earth;s (ie. the external) field, and negative otherwise.

c Units:
The component and total anomalies will be returned in the same units
used to specify the earth's total field strength, and thus the user
is free to choose any units deemed convenient. Note that the way
in which the argument 'suscept' is interpreted does depend on the
system of units being employed. Spatial coordinates may be given in
any length unit, as long as the unit chosen is employed consistently
throughout.

i use si units
So if suscept=0.015 S.I. units,
*/

bool m_poly(double *xs, double *zs, int nstn, double *xv, double *zv, int nvert,
	double he_suscept,
	double anginc,  //  90
	double angstr,  // 0, angles in degrees
	//	double suscept,
	double *anom_z,
	double *anom_x,
	double *anom_t)
{
	if (nstn < 1 || nvert < 4) return false;

	//	double pi = 3.141592654;
	static double two_pi = 2.0*PI;
	// static double dtr = PIO;
	static double c1 = sin(anginc*PIO);
	static double c2 = sin(angstr*PIO) * cos(anginc*PIO);

	//	double c3 = 2.*suscept*he;
	double c3 = 2.*he_suscept;
	for (int is = 0; is < nstn; is++) {
		register double xst = xs[is];
		register double zst = zs[is];
		register double hz = 0.;
		register double hx = 0.;

		for (int ic = 0; ic < nvert; ic++) {
			register double gz = 0.;

			register double x1 = xv[ic] - xst;  // translate origin
			register double z1 = zv[ic] - zst;
			if (x1 == 0. && z1 == 0.) continue;

			register double x2, z2;
			if (ic == nvert - 1) {
				x2 = xv[0] - xst;
				z2 = zv[0] - zst;
			}
			else {
				x2 = xv[ic + 1] - xst;
				z2 = zv[ic + 1] - zst;
			}
			if (x2 == 0. && z2 == 0.) continue;

			register double th1 = atan2(z1, x1);  // if use atan(), when x1==0, z1/x1 has problem!
			register double th2 = atan2(z2, x2);

			if (!SAME_SIGN(z1, z2)) {
				register double test = x1*z2 - x2*z1;
				if (test > 0. && z1 >= 0)
					th2 += two_pi;
				else if (test < 0. && z2 >= 0.)
					th1 += two_pi;
				else if (test == 0.) continue;
			}

			register double t12 = th1 - th2;
			register double z21 = z2 - z1;
			register double x21 = x2 - x1;
			register double x21s = x21*x21;
			register double z21s = z21*z21;
			register double xz12 = x1*z2 - x2*z1;

			register double r1s = x1*x1 + z1*z1;
			register double r2s = x2*x2 + z2*z2;
			register double r21s = x21*x21 + z21*z21;
			if (r1s == 0. || r21s == 0.) continue;
			register double r1n = 0.5 * log(r2s / r1s);
			register double p = (xz12 / r21s) * ((x1*x21 - z1 *z21) / r1s - (x2*x21 - z2*z21) / r2s);
			register double q = (xz12 / r21s) * ((x1*z21 + z1 *x21) / r1s - (x2*z21 + z2*x21) / r2s);

			register double dzz, dzx, dxz, dxx;
			if (x21 == 0.) {
				dzz = -p;
				dzx = q - z21s * r1n / r21s;
				dxz = q;
				dxx = p + z21s*t12 / r21s;
			}
			else {
				register double z21dx21, x21z21, fz, fx;
				z21dx21 = z21 / x21;
				x21z21 = x21*z21;
				fz = (t12 + z21dx21 *r1n) / r21s;
				fx = (t12 *z21dx21 - r1n) / r21s;
				dzz = -p + x21s*fz;
				dzx = q - x21z21*fz;
				dxz = q - x21s*fx;
				dxx = p + x21z21 * fx;
			}
			hz += c3*(c1*dzz + c2*dzx);
			hx += c3*(c1*dxz + c2*dxx);
		} // for ic

		if (anom_z)
			anom_z[is] = hz;
		if (anom_x)
			anom_x[is] = hx;
		if (anom_t)
			anom_t[is] = c1*hz + c2*hx;
	} // for is

	return true;
}

// if (px, py) is on the line or the line is not defined well, we return -1
// if above, we return 1
// if below, we return 0
int IsPointAbove(float x1, float y1, float x2, float y2, float px, float py)
{
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

int IsPointAbove(SEI_PAIR p1, double x2, double y2, double px, double py)
{
	SEI_QUAD line;
	line.x1 = p1.x;
	line.y1 = p1.y;
	line.x2 = x2;
	line.y2 = y2;

	SEI_PAIR p;
	p.x = px;
	p.y = py;
	return IsPointAbove(line, p);
}
int IsPointAbove(SEI_PAIR p1, double x2, double y2, SEI_PAIR p)
{
	SEI_QUAD line;
	line.x1 = p1.x;
	line.y1 = p1.y;
	line.x2 = x2;
	line.y2 = y2;
	return IsPointAbove(line, p);
}
int IsPointAbove(SEI_QUAD line, SEI_PAIR p)
{
	// note: x2>=x1 assumed, and if px is not in between x1 and x2, we force it to be false!  // disabled this
	// if (px<x1 || px>x2) return false;
	double dx = line.x2 - line.x1;
	if (fabs(dx) == 0.f) return -1;
	double dy = line.y2 - line.y1;
	double m1 = dy / dx;
	double c1 = line.y1 - m1 * line.x1; // which is same as y2 - slope * x2
	
	if ((m1 * p.x + c1) == p.y) return -1;
	return (m1 * p.x + c1) >= p.y;
}


bool IsPointInside(CPoint *p, CRect *logicalAxes)
{
	// the advantage of using CRect as the bounding box is that 
	// its leftside is always smaller than the right side, and bottom is always greater than top
	return p->x >= logicalAxes->left &&
		p->x <= logicalAxes->right &&
		p->y >= logicalAxes->top &&
		p->y <= logicalAxes->bottom;
}

// [x1,y1] and [x2,y2] can be in non-monotonical
bool IsPointInBoundingBox2_Depreciated(float x1, float y1, float x2, float y2, float px, float py)
{
	float left, top, right, bottom; // Bounding Box For Line Segment
	// For Bounding Box
	if (x1 < x2) {
		left = x1;
		right = x2;
	}
	else {
		left = x2;
		right = x1;
	}
	if (y1 < y2) {
		top = y1;
		bottom = y2;
	}
	else {
		top = y2;
		bottom = y1;
	}

	return (px + 0.00001) >= left &&
		(px - 0.00001) <= right &&
		(py + 0.00001) >= top &&
		(py - 0.00001) <= bottom;
}

bool IsPointInBoundingBox(SEI_QUAD line, SEI_PAIR p)
{
	double left, top, right, bottom; // Bounding Box For Line Segment
									// For Bounding Box
	if (line.x1 < line.x2) {
		left = line.x1;
		right = line.x2;
	}
	else {
		left = line.x2;
		right = line.x1;
	}
	if (line.y1 < line.y2) {
		top = line.y1;
		bottom = line.y2;
	}
	else {
		top = line.y2;
		bottom = line.y1;
	}

	return (p.x + 0.00001) >= left &&
		(p.x - 0.00001) <= right &&
		(p.y + 0.00001) >= top &&
		(p.y - 0.00001) <= bottom;
}

bool LineIntersection(float ax1, float ay1, float ax2, float ay2,
	float bx1, float by1, float bx2, float by2,
	float* X, float* Y)
{
	float m1, c1, m2, c2;
	float dxa, dxb, dya, dyb;

	dxa = ax2 - ax1;
	dxb = bx2 - bx1;
	if (dxa == 0.f || dxb == 0.f) return false;

	dya = ay2 - ay1;
	dyb = by2 - by1;
	if (dya == 0.f && dyb == 0.f) return false;

	m1 = dya / dxa;
	// y = mx + c
	// intercept c = y - mx
	c1 = ay1 - m1 * ax1; // which is same as y2 - slope * x2

	m2 = dyb / dxb;
	// y = mx + c
	// intercept c = y - mx
	c2 = by1 - m2 * bx1; // which is same as y2 - slope * x2

	if ((m1 - m2) == 0.f) return false;

	*X = (c2 - c1) / (m1 - m2);
	*Y = m1 * *X + c1;

	if (*X<ax1 || *X>ax2 || *X<bx1 || *X>bx2) return false;

	// no need to check *Y.
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
bool LineSegmentIntersection2_Depreciated(float l1x1, float l1y1, float l1x2, float l1y2,
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

	if ((*m1 - *m2) == 0)
		return false;

	*intersection_X = (*c2 - *c1) / (*m1 - *m2);
	*intersection_Y = *m1 * *intersection_X + *c1;
	return IsPointInBoundingBox2_Depreciated(l1x1, l1y1, l1x2, l1y2, *intersection_X, *intersection_Y) &&
		IsPointInBoundingBox2_Depreciated(l2x1, l2y1, l2x2, l2y2, *intersection_X, *intersection_Y);
}

bool LineSegmentIntersection(SEI_QUAD l1, SEI_QUAD l2, SEI_PAIR* intersection,
	double *m1, double *c1, double *m2, double *c2)
{
	double dx, dy;
	
	dx = l1.x2 - l1.x1;
	if (fabs(dx) == 0.f) return false;
	dy = l1.y2 - l1.y1;

	*m1 = dy / dx;
	// y = mx + c
	// intercept c = y - mx
	*c1 = l1.y1 - *m1 * l1.x1; // which is same as y2 - slope * x2

	dx = l2.x2 - l2.x1;
	if (fabs(dx) == 0.f) return false;
	dy = l2.y2 - l2.y1;

	*m2 = dy / dx;
	// y = mx + c
	// intercept c = y - mx
	*c2 = l2.y1 - *m2 * l2.x1; // which is same as y2 - slope * x2

	if ((*m1 - *m2) == 0)
		return false;

	intersection->x = (*c2 - *c1) / (*m1 - *m2);
	intersection->y = *m1 * intersection->x + *c1;
	return IsPointInBoundingBox(l1, *intersection) && IsPointInBoundingBox(l2, *intersection);
}

bool LineSegmentIntersection(SEI_QUAD l1, SEI_QUAD l2, SEI_PAIR* intersection)
{
	double m1, c1, m2, c2;
	return LineSegmentIntersection(l1, l2, intersection, &m1, &c1, &m2, &c2);

	//double dx, dy;

	//dx = l1.x2 - l1.x1;
	//if (fabs(dx) == 0.f) return false;
	//dy = l1.y2 - l1.y1;

	//*m1 = dy / dx;
	//// y = mx + c
	//// intercept c = y - mx
	//*c1 = l1.y1 - *m1 * l1.x1; // which is same as y2 - slope * x2

	//dx = l2.x2 - l2.x1;
	//if (fabs(dx) == 0.f) return false;
	//dy = l2.y2 - l2.y1;

	//*m2 = dy / dx;
	//// y = mx + c
	//// intercept c = y - mx
	//*c2 = l2.y1 - *m2 * l2.x1; // which is same as y2 - slope * x2

	//if ((*m1 - *m2) == 0)
	//	return false;

	//intersection->x = (*c2 - *c1) / (*m1 - *m2);
	//intersection->y = *m1 * intersection->x + *c1;
	//return IsPointInBoundingBox(l1, *intersection) && IsPointInBoundingBox(l2, *intersection);
}



int myClipPointsX(int n, int nLimit, double x[], double z[], SEI_PAIR seip)
{
	if (n < 1) return n; // return if no point exist

	double xmin = seip.x;
	double xmax = seip.y;
	double* x2 = (double *)alloc1double((size_t)nLimit);
	double* z2 = (double *)alloc1double((size_t)nLimit);
	// we need more array size since the eventual output may contain 
	// interpolated extra points

	int i;
	int n2 = 0;
	if (n < 2) {
		// when only 1 point, 
		if (x[0] >= xmin && x[0] <= xmax) {
			x2[n2] = x[0];
			z2[n2] = z[0];
			n2++;
		}
	}
	else {
		// first we only deals with the xmin and xmax, note that we have n>=2
		for (i = 0; i < n; i++) {
			if (n2 >= nLimit - 1) break;  // why use nLimit-1? because we may add an extra point
			register double dx = i == 0 ? x[i+1]-x[i] : x[i] - x[i - 1];
			if (x[i] >= xmin && x[i] <= xmax) {
				if (i>0 && x[i - 1]<xmin && fabs(dx)>MINDIST) {
					// since we only look backward, we need to check if the previous point 
					// is not visible. If so, we need to insert one point
					x2[n2] = xmin;
					z2[n2] = z[i - 1] + (z[i] - z[i - 1])*(xmin - x[i - 1]) / dx;
					n2++;
				}
				else if (i > 0 && x[i - 1] > xmax && fabs(dx) > MINDIST) {
					x2[n2] = xmax;
					z2[n2] = z[i - 1] + (z[i] - z[i - 1])*(xmax - x[i - 1]) / dx;
					n2++;
				}

				x2[n2] = x[i];
				z2[n2] = z[i];
				n2++;
				continue;
			}

			// now x[i] is not inside the plotting area
			// Note: each line can go from left to right, or from right to left, and has to be handled differently!!!
			if (i == 0) {
				// for the first point, we only need to handle 1 case of no point inside [xmin, xmax)
				if (x[i] < xmin && x[i + 1] >= xmax && fabs(dx)>=MINDIST) {
					x2[n2] = xmin;
					z2[n2] = z[i] + (z[i+1] - z[i])*(xmin - x[i]) / dx;
					n2++;
				}
			}
			else if (x[i - 1] < xmin && x[i] > xmax) {
				// special case catched on Aug. 25, 2020
				x2[n2] = xmin;
				z2[n2] = z[i - 1] + (z[i] - z[i - 1]) * (xmin - x[i - 1]) / dx;
				n2++;

				x2[n2] = xmax;
				z2[n2] = z[i - 1] + (z[i] - z[i - 1]) * (xmax - x[i - 1]) / dx;
				n2++;
			}
			else if (x[i - 1] < xmax && x[i] > xmax) {
				x2[n2] = xmax;
				z2[n2] = z[i - 1] + (z[i] - z[i - 1])*(xmax - x[i - 1]) / dx;
				n2++;
			}
			else if (x[i]<xmin) {
				if (x[i - 1] > xmax) {
					// an extra point is needed
					x2[n2] = xmax;
					z2[n2] = z[i - 1] + (z[i] - z[i - 1])*(xmax - x[i - 1]) / dx;
					n2++;
				}
				else if (x[i - 1] > xmin) {
					x2[n2] = xmin;
					z2[n2] = z[i - 1] + (z[i] - z[i - 1])*(xmin - x[i - 1]) / dx;
					n2++;
				}
			}
		}
	}  // if n<2


	// note: n2 may be greater than n but definitly smaller than nLimit
	memcpy((void *)&x[0], (void *)&x2[0], n2*sizeof(double));
	memcpy((void *)&z[0], (void *)&z2[0], n2*sizeof(double));
	free1double(x2);
	free1double(z2);

	return n2;
}


int myClipPointsZ(int n, int nLimit, double x[], double z[], SEI_PAIR seip)
{
	if (n < 1) return n; // return if no point exist
	double zmin = seip.x;
	double zmax = seip.y;

	double* x2 = (double *)alloc1double((size_t)nLimit);
	double* z2 = (double *)alloc1double((size_t)nLimit);
	// we need more array size since the eventual output may contain 
	// interpolated extra points
	int i;
	int n2;
	double dx, dz;
	double odz;

	// now we only deals with the zmin and zmax
	n2 = 0;

	if (n < 2) {
		// when only 1 point, 
		if (z[0] >= zmin && z[0] <= zmax) {
			x2[n2] = x[0];
			z2[n2] = z[0];
			n2++;
		}
	}
	else {
		for (i = 0; i < n; i++) {
			if (n2 >= nLimit - 1) {   // why use nLimit-1? because we may add an extra point
				break;
			}
			dx = i == 0 ? x[i + 1] - x[i] : x[i] - x[i - 1];
			dz = i == 0 ? z[i + 1] - z[i] : z[i] - z[i - 1];
			odz = dz == 0. ? 0. : 1. / dz;
			if (z[i] >= zmin && z[i] <= zmax) {
				if (i > 0 && z[i - 1]<zmin && fabs(dz)>MINDIST) {
					// since we only look backward, we need to check if the previous point 
					// is not visible. If so, we need to insert one point
					z2[n2] = zmin;
					x2[n2] = x[i - 1] + dx*(zmin - z[i - 1]) * odz;
					n2++;
				}
				else if (i > 0 && z[i - 1] > zmax && fabs(dz) > MINDIST) {
					x2[n2] = x[i - 1] + dx*(zmax - z[i - 1]) * odz;
					z2[n2] = zmax;
					n2++;
				}

				x2[n2] = x[i];
				z2[n2] = z[i];
				n2++;
				continue;
			}

			// now z[i] is not inside the plotting area
			if (i == 0) {
				// for the first point, we only need to handle 1 case of no point inside [xmin, xmax)
				if (z[i] < zmin && z[i + 1] >= zmax && fabs(dz) >= MINDIST) {
					x2[n2] = x[i] + (x[i + 1] - x[i])*(zmin - z[i]) / dz;
					z2[n2] = zmin;
					n2++;
				}
			}
			else if (z[i] > zmax) {
				if (z[i - 1] < zmin) {
					// an extra point is needed
					x2[n2] = x[i - 1] + dx*(zmin - z[i - 1]) * odz;
					z2[n2] = zmin;
					n2++;
				}
				else if (z[i - 1] < zmax) {
					x2[n2] = x[i - 1] + dx*(zmax - z[i - 1]) * odz;
					z2[n2] = zmax;
					n2++;
				}
			}
			else if (z[i]<zmin) {
				// now we know we have z[i] < zmin
				if (z[i - 1] > zmax) {
					// an extra point is needed
					x2[n2] = x[i - 1] + dx*(zmax - z[i - 1]) * odz;
					z2[n2] = zmax;
					n2++;
				}
				else if (z[i - 1] > zmin) {
					x2[n2] = x[i - 1] + dx*(zmin - z[i - 1]) * odz;
					z2[n2] = zmin;
					n2++;
				}
			}
		}
	} // if n<2

	// note: n2 may be greater than n but definitly smaller than nLimit
	//n = n2;
	memcpy((void *)&x[0], (void *)&x2[0], n2*sizeof(double));
	memcpy((void *)&z[0], (void *)&z2[0], n2*sizeof(double));

	free1double(x2);
	free1double(z2);

	return n2;
}

// nLimit are the array limits for *x and *z, while n is the number of valid data points 
//  in these arrays. Usually we have nLimit = 2*n. Clipped points are shall be copied back to
//  *x and *z, and the new values shall have number of points returned.
int myClipPoints2(int n, int nLimit, double x[], double z[], CRect rect)
{
	SEI_QUAD seiq;
	seiq.x1 = rect.left;
	seiq.x2 = rect.right;
	seiq.y1 = rect.top;
	seiq.y2 = rect.bottom;
	return myClipPoints2(n, nLimit, x, z, seiq);
}

int myClipPoints2(int n, int nLimit, double x[], double z[], SEI_QUAD seiq)
{
	// int nLimit = 2 * n; // to avoid mis-handling
	double a;
	if (seiq.x1 > seiq.x2) {
		a = seiq.x1;
		seiq.x1 = seiq.x2;
		seiq.x2 = a;
	}

	if (seiq.y1 > seiq.y2) {
		a = seiq.y1;
		seiq.y1 = seiq.y2;
		seiq.y2 = a;
	}
	SEI_PAIR seip;
	seip.x = seiq.x1;
	seip.y = seiq.x2;
	n = myClipPointsX(n, nLimit, x, z, seip);
	if (n > nLimit) n = nLimit;


	seip.x = seiq.y1;
	seip.y = seiq.y2;
	n = myClipPointsZ(n, nLimit, x, z, seip);
	if (n > nLimit) n = nLimit;


	return n;
}


//
//int myClipPoints2Depreciate(int n, int nLimit, double *x, double *z, double xmin, double xmax, double zmin, double zmax)
//{
//	//###### nLimit is usually 2*n, max number of points allowed in arrays x[] and z[]. 
//	//###### Please make sure the x[], z[] do not go beyond scope (nLimit)
////	SEI_QUAD seiq;
//	// given x[] and z[], clip any points outside the current window area, and
//	// output as x1[] and z1[]. Total number of points in the output is returned.
//	// Note: the current window is defined in depth domain as: 
//	// xmin, xmax, and zmin and zmax
//	// commonly we have   int nLimit = n*2; 
//	// We need to clip on X, and then on Z axis. Note that the points may not be in sequencial order, 
//	//    so that if you see a few points outside the bounds, subsequent points may still be inside the bounds!!!
//	double a;
//	if (xmin > xmax) {
//		a = xmin;
//		xmin = xmax;
//		xmax = a;
//	}
//
//	if (zmin > zmax) {
//		a = zmin;
//		zmin = zmax;
//		zmax = a;
//	}
//
//	SEI_PAIR seip;
//	seip.x = xmin;
//	seip.y = xmax;
//	n = myClipPointsX(n, nLimit, x, z, seip);
//
//	seip.x = zmin;
//	seip.y = zmax;
//	return myClipPointsZ(n, nLimit, x, z, seip);
//}

//
//int myClipPoints2OLD(int n, int nLimit, double *x, double *z, double xmin, double xmax, double zmin, double zmax)
//{
//	//###### nLimit is usually 2*n, max number of points allowed in arrays x[] and z[]. 
//	//###### Please make sure the x[], z[] do not go beyond scope (nLimit)
//
//	// given x[] and z[], clip any points outside the current window area, and
//	// output as x1[] and z1[]. Total number of points in the output is returned.
//	// Note: the current window is defined in depth domain as: 
//	// xmin, xmax, and zmin and zmax
//	// *****we assume   xmin < xmax   and zmin < zmax*****
//	// int NUMMAX = n*4;
//	double* x2 = (double *)ealloc1double((size_t)nLimit);
//	double* z2 = (double *)ealloc1double((size_t)nLimit);
//	double* xBac = (double *)ealloc1double((size_t)nLimit);
//	double* zBac = (double *)ealloc1double((size_t)nLimit);
//	// we need more array size since the eventual output may contain 
//	// interpolated extra points
//
//	int i;
//	int n2 = 0, nBac;
//	double odx, odz;
//
//	// first we only deals with the xmin and xmax
//	for (i = 0; i<n; i++) {
//		if (n2 >= nLimit) break;
//		if (x[i] >= xmin && x[i] <= xmax) {
//			x2[n2] = x[i];
//			z2[n2] = z[i];
//			n2++;
//		}
//		if (x[i] == xmin) continue;
//		if (x[i] == xmax) continue;
//		if (i == n - 1) {
//			if (x[i]>xmax && fabs(x[i] - x[i - 1]) >= 0.000001) {
//				// a special case: the last model point is beyong the display area. 
//				x2[n2] = (float)xmax;
//				z2[n2] = (float)(z[i - 1] + (z[i] - z[i - 1])*(xmax - x[i - 1]) / (x[i] - x[i - 1]));
//				n2++;
//			}
//			break;
//		}
//		if (fabs(x[i] - x[i + 1]) < 0.000001) continue;
//		if (x[i] < xmin && x[i + 1]<xmin) continue;
//		if (x[i]>xmax && x[i + 1] > xmax) continue;
//
//		odx = 1. / (x[i + 1] - x[i]);
//		if (x[i] < x[i + 1]) {
//			// the line going right-ward now
//			if (x[i]<xmin && x[i + 1]> xmax) {
//				// now add 2 interception points for the left and right sides
//				// we will consider a line going rightward
//				x2[n2] = (float)xmin;
//				z2[n2] = (float)(z[i] + (z[i + 1] - z[i])*(xmin - x[i]) * odx);
//				n2++;
//				x2[n2] = (float)xmax;
//				z2[n2] = (float)(z[i] + (z[i + 1] - z[i])*(xmax - x[i]) * odx);
//				n2++;
//			}
//			else if (x[i]<xmin && x[i + 1]> xmin) {
//				// now add the interception point for the left side
//				x2[n2] = (float)xmin;
//				z2[n2] = (float)(z[i] + (z[i + 1] - z[i])*(xmin - x[i]) * odx);
//				n2++;
//			}
//			else if (x[i]<xmax && x[i + 1]>xmax)  {
//				// now add the interception point for the left side
//				x2[n2] = (float)xmax;
//				z2[n2] = (float)(z[i] + (z[i + 1] - z[i])*(xmax - x[i]) * odx);
//				n2++;
//			}
//		}
//		else {
//			// the line going left-ward now
//			if (x[i + 1]<xmin && x[i]> xmax) {
//				// now add 2 interception points for the left and right sides
//				// we will consider a line going leftward
//				x2[n2] = (float)xmax;
//				z2[n2] = (float)(z[i] + (z[i + 1] - z[i])*(xmax - x[i]) * odx);
//				n2++;
//				x2[n2] = (float)xmin;
//				z2[n2] = (float)(z[i] + (z[i + 1] - z[i])*(xmin - x[i]) * odx);
//				n2++;
//			}
//			else if (x[i + 1]<xmin && x[i]>xmin) {
//				// now add the interception point for the left side
//				x2[n2] = (float)xmin;
//				z2[n2] = (float)(z[i] + (z[i + 1] - z[i])*(xmin - x[i]) * odx);
//				n2++;
//			}
//			else if (x[i + 1]<xmax && x[i]>xmax)  {
//				// now add the interception point for the left side
//				x2[n2] = (float)xmax;
//				z2[n2] = (float)(z[i] + (z[i + 1] - z[i])*(xmax - x[i]) * odx);
//				n2++;
//			}
//		}
//	}
//
//	// backup to x[] and z[]
//	memcpy((void *)&xBac[0], (void *)&x2[0], n2*sizeof(double));
//	memcpy((void *)&zBac[0], (void *)&z2[0], n2*sizeof(double));
//	nBac = n2;
//
//	// now we only deals with the zmin and zmax
//	n2 = 0;
//	for (i = 0; i<nBac; i++) {
//		if (n2 >= nLimit) break;
//		if (zBac[i] >= zmin && zBac[i] <= zmax) {
//			x2[n2] = xBac[i];
//			z2[n2] = zBac[i];
//			n2++;
//		}
//		if (zBac[i] == zmin) continue;
//		if (zBac[i] == zmax) continue;
//
//		if (i == nBac - 1) {
//			if (zBac[i]>zmax && fabs(zBac[i] - zBac[i - 1]) >= 0.000001) {
//				// a special case: the last model point is beyong the display area. 
//				x2[n2] = (float)(xBac[i - 1] + (xBac[i] - xBac[i - 1])*(zmax - zBac[i - 1]) / (zBac[i] - zBac[i - 1]));
//				z2[n2] = (float)zmax;
//				n2++;
//			}
//			break;
//		}
//		if (fabs(zBac[i] - zBac[i + 1]) < 0.000001) continue;
//		if (zBac[i] < zmin && zBac[i + 1]<zmin) continue;
//		if (zBac[i]>zmax && zBac[i + 1] > zmax) continue;
//
//
//		odz = 1. / (zBac[i + 1] - zBac[i]);
//		if (zBac[i] < zBac[i + 1]) {
//			// the line going down-ward now
//			if (zBac[i]<zmin && zBac[i + 1]> zmax){
//				// now add 2 interception points for the top and bottom sides
//				x2[n2] = (xBac[i] + (xBac[i + 1] - xBac[i])*(zmin - zBac[i]) * odz);
//				z2[n2] = zmin;
//				n2++;
//				x2[n2] = (xBac[i] + (xBac[i + 1] - xBac[i])*(zmax - zBac[i]) * odz);
//				z2[n2] = zmax;
//				n2++;
//			}
//			else if (zBac[i]<zmin && zBac[i + 1]> zmin){
//				// now add the interception point for the bottom side
//				x2[n2] = (xBac[i] + (xBac[i + 1] - xBac[i])*(zmin - zBac[i]) * odz);
//				z2[n2] = zmin;
//				n2++;
//			}
//			else if (zBac[i]<zmax && zBac[i + 1]>zmax) {
//				// now add the interception point for the top side
//				x2[n2] = (xBac[i] + (xBac[i + 1] - xBac[i])*(zmax - zBac[i]) * odz);
//				z2[n2] = zmax;
//				n2++;
//			}
//		}
//		else {
//			// the line going up-ward now
//			if (zBac[i + 1]<zmin && zBac[i]> zmax){
//				// now add 2 interception points for the top and bottom sides
//				x2[n2] = (xBac[i] + (xBac[i + 1] - xBac[i])*(zmax - zBac[i]) * odz);
//				z2[n2] = zmax;
//				n2++;
//				x2[n2] = (xBac[i] + (xBac[i + 1] - xBac[i])*(zmin - zBac[i]) * odz);
//				z2[n2] = zmin;
//				n2++;
//			}
//			else if (zBac[i + 1]<zmin && zBac[i]> zmin){
//				// now add the interception point for the bottom side
//				x2[n2] = (xBac[i] + (xBac[i + 1] - xBac[i])*(zmin - zBac[i]) * odz);
//				z2[n2] = zmin;
//				n2++;
//			}
//			else if (zBac[i + 1]<zmax && zBac[i]>zmax) {
//				// now add the interception point for the top side
//				x2[n2] = (xBac[i] + (xBac[i + 1] - xBac[i])*(zmax - zBac[i]) * odz);
//				z2[n2] = zmax;
//				n2++;
//			}
//		}
//	}
//
//	// note: n2 may be greater than n but definitly smaller than nLimit
//	n = n2;
//	memcpy((void *)&x[0], (void *)&x2[0], n2*sizeof(double));
//	memcpy((void *)&z[0], (void *)&z2[0], n2*sizeof(double));
//
//	free1double(x2);
//	free1double(z2);
//	free1double(xBac);
//	free1double(zBac);
//
//	// return myClipPoints2(n, n*2, x, z, xmin, xmax, zmin, zmax);
//	return n;
//}


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
	}
	else if (t > 1.) {
		//point is nearest to the end point i.e x2 and y2
		diffX = pointX - x2;
		diffY = pointY - y2;
	}
	else {
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



// ##########################################################
// ##########################################################
// ##########################################################
// ##########################################################
// ##########################################################
// XML common functions
CString ShowError(MSXML::IXMLDOMDocumentPtr pXMLDocument)
{
	CString sError = _T("MSXML::DomDocument::load failed. \n");
	sError.Format(_T("Error: %s"), pXMLDocument->parseError->Getreason().GetBSTR());
	AfxMessageBox(sError, MB_ICONQUESTION);
	return sError;
}
BOOL IsValid(MSXML::IXMLDOMNodePtr pParent)
{
	if (pParent == NULL)
		return FALSE;
	if (pParent.GetInterfacePtr() == NULL)
		return FALSE;
	return TRUE;
}

MSXML::IXMLDOMNodePtr myGetSingleChildByName(MSXML::IXMLDOMNodePtr pParent, CString name)
{
	if (!IsValid(pParent))
		return NULL;
	MSXML::IXMLDOMNodeListPtr pNodes = pParent->childNodes;
	int irter = pNodes->length;
	// getElementsByTagName(name);
	for (int i = 0; i < pNodes->length; i++) {
		register MSXML::IXMLDOMNodePtr pChild = pNodes->Getitem(i);
		BSTR bName = pChild->nodeName;
		CString strName(bName);
		if (strName.CompareNoCase(name) == 0) {
			return pChild;
		}
	}

	return NULL;
}


bool Is_Having_Attribute(MSXML::IXMLDOMNamedNodeMapPtr pAttrs, _bstr_t attr)
{
	if (pAttrs == NULL) return false;
	CString strAttr = CString(attr.GetBSTR());
	int bcl;
	for (bcl = 0; bcl < pAttrs->length; bcl++) {  // Check if the name is equal
		//
		if (strAttr.CompareNoCase(pAttrs->Getitem(bcl)->GetnodeName()) == 0) {
			return true;
		}
	}
	return false;
}


_variant_t getAttributeDef(MSXML::IXMLDOMNamedNodeMapPtr pAttrs, _bstr_t attr, _variant_t val)
{
	if (pAttrs == NULL) return val;
	if (Is_Having_Attribute(pAttrs, attr))
		return pAttrs->getNamedItem(attr)->nodeValue;

	return val;
}

_variant_t getAttributeDef(MSXML::IXMLDOMNodePtr pAttr, _variant_t val)
{
	return (pAttr == NULL) ? val : pAttr->nodeValue;
}
//
//_variant_t getAttributeDef(MSXML::IXMLDOMNodePtr pNode, _bstr_t attr, _variant_t val)
//{
//	// this is used for getting one of the document element's attributes
//	if (pNode == NULL) return val;
//	MSXML::IXMLDOMNamedNodeMapPtr pNodeMap;
//	return getAttributeDef(pNode->attributes, attr, val);
//	//return pNode->GetnodeValue();
//	//MSXML::IXMLDOMAttributePtr p = pNode->getAttributeNode(attr);
//	//return (p == NULL) ? val : p->nodeValue;
//}

_variant_t getAttributeDef(MSXML::IXMLDOMElementPtr pAttr, _bstr_t attr, _variant_t val)
{
	// this is used for getting one of the document element's attributes
	if (pAttr == NULL) return val;
	MSXML::IXMLDOMAttributePtr p = pAttr->getAttributeNode(attr);
	return (p == NULL) ? val : p->nodeValue;
}


// for general usage: add a single placeMark to a KML node
void appendPlacemarkPoint(MSXML::IXMLDOMDocument2Ptr pXMLDoc, MSXML::IXMLDOMElementPtr pElem,
	double lon, double lat, double depth, CString aName, CString aStyle)
{
	MSXML::IXMLDOMElementPtr p0 = pXMLDoc->createElement(_T("Placemark"));
	p0->setAttribute(_T("id"), _variant_t(aName));
	pElem->appendChild(p0);

	MSXML::IXMLDOMElementPtr p2, p3;

	p2 = pXMLDoc->createElement(_T("name"));
	p2->Puttext(_bstr_t(aName));
	p0->appendChild(p2);

	p2 = pXMLDoc->createElement(_T("description"));
	p2->Puttext(_bstr_t(aName));
	p0->appendChild(p2);

	p2 = pXMLDoc->createElement(_T("styleUrl"));
	p2->Puttext(_bstr_t(aStyle));
	p0->appendChild(p2);

	p2 = pXMLDoc->createElement(_T("Point"));
	p0->appendChild(p2);

	p3 = pXMLDoc->createElement(_T("altitudeMode"));
	p3->Puttext(_bstr_t("relativeToGround"));
	p2->appendChild(p3);


	p3 = pXMLDoc->createElement(_T("coordinates"));
	p3->Puttext(
		_bstr_t(lon) +
		_bstr_t(",") +
		_bstr_t(lat) +
		_bstr_t(",") +
		_bstr_t(depth)
		);
	p2->appendChild(p3);
	// end </Placemark>

}

void appendPlacemarkLine(MSXML::IXMLDOMDocument2Ptr pXMLDoc, MSXML::IXMLDOMElementPtr pElem,
	double lon[], double lat[], int np, CString aName, CString aStyle)
{
	int k= aName.ReverseFind('\\');
	if (k >= 0) {
		aName = aName.Mid(k+1);
	}
	MSXML::IXMLDOMElementPtr p0 = pXMLDoc->createElement(_T("Placemark"));
	p0->setAttribute(_T("id"), _variant_t(aName));
	pElem->appendChild(p0);

	MSXML::IXMLDOMElementPtr p2, p3;

	p2 = pXMLDoc->createElement(_T("name"));
	p2->Puttext(_bstr_t(aName));
	p0->appendChild(p2);

	p2 = pXMLDoc->createElement(_T("description"));
	p2->Puttext(_bstr_t(aName));
	p0->appendChild(p2);

	p2 = pXMLDoc->createElement(_T("styleUrl"));
	p2->Puttext(_bstr_t(aStyle));
	p0->appendChild(p2);

	p2 = pXMLDoc->createElement(_T("LineString"));
	p0->appendChild(p2);

	p3 = pXMLDoc->createElement(_T("coordinates"));
	CString tStr, strOut = _T("");
	for (int i = 0; i < np; i++) {
		tStr.Format(_T("%f,%f\n"), lon[i], lat[i]);
		strOut += tStr;
	}
	p3->Puttext(_bstr_t(strOut));
	p2->appendChild(p3);
	// end </Placemark>

}

/*
<Style id="s_2015_h1">
<IconStyle>
<color>7f7fff00</color>
<scale>0.472727</scale>
<Icon>
<href>http://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png</href>
</Icon>
<hotSpot x="20" y="2" xunits="pixels" yunits="pixels"/>
</IconStyle>
<LabelStyle>
<color>7fffaa00</color>
<scale>1.1</scale>
</LabelStyle>
</Style>
<Style id="s_2015">
<IconStyle>
<color>7f7fff00</color>
<scale>0.4</scale>
<Icon>
<href>http://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png</href>
</Icon>
<hotSpot x="20" y="2" xunits="pixels" yunits="pixels"/>
</IconStyle>
<LabelStyle>
<color>7fffaa00</color>
<scale>0.6</scale>
</LabelStyle>
</Style>
<StyleMap id="m_2015">
<Pair>
<key>normal</key>
<styleUrl>#s_2015</styleUrl>
</Pair>
<Pair>
<key>highlight</key>
<styleUrl>#s_2015_h1</styleUrl>
</Pair>
</StyleMap>

<Placemark id="1">
<name>1</name>
<description>1</description>
<styleUrl>#m_2015</styleUrl>
<Point>
<altitudeMode>relativeToGround</altitudeMode>
<coordinates>-7.94277,81.13672,0</coordinates>
</Point>
</Placemark>
*/
void appendPlacemarkStyle(MSXML::IXMLDOMDocument2Ptr pXMLDoc, MSXML::IXMLDOMElementPtr pElem,
	CString styleMidName, double labelSize) {
	int k = styleMidName.ReverseFind('\\');
	if (k >= 0) {
		styleMidName = styleMidName.Mid(k + 1);
	}
	MSXML::IXMLDOMElementPtr p1, p2, p3, p4;

	p1 = pXMLDoc->createElement(_T("Style"));
	p1->setAttribute(_T("id"), _variant_t(_T("s") + styleMidName));
		p2 = pXMLDoc->createElement(_T("IconStyle"));
			p3 = pXMLDoc->createElement(_T("color"));
			p3->Puttext(_bstr_t("7f7fff00"));
			p2->appendChild(p3);
			p3 = pXMLDoc->createElement(_T("scale"));
			p3->Puttext(_bstr_t("0.5"));
			p2->appendChild(p3);

			p3 = pXMLDoc->createElement(_T("Icon"));
				p4 = pXMLDoc->createElement(_T("href"));
				p4->Puttext(_bstr_t("http://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png"));
				p3->appendChild(p4);
			p2->appendChild(p3);

			p3 = pXMLDoc->createElement(_T("hotSpot"));
			p3->setAttribute(_T("x"), _variant_t("20"));
			p3->setAttribute(_T("y"), _variant_t("2"));
			p3->setAttribute(_T("xunits"), _variant_t("pixels"));
			p3->setAttribute(_T("yunits"), _variant_t("pixels"));
			p2->appendChild(p3);
		p1->appendChild(p2);
		p2 = pXMLDoc->createElement(_T("LabelStyle"));
			p3 = pXMLDoc->createElement(_T("color"));
			p3->Puttext(_bstr_t("7fffaa00"));
			p2->appendChild(p3);
			p3 = pXMLDoc->createElement(_T("scale"));
			p3->Puttext(_bstr_t(labelSize));
			p2->appendChild(p3);
		p1->appendChild(p2);

		p2 = pXMLDoc->createElement(_T("LineStyle"));
			p3 = pXMLDoc->createElement(_T("color"));
			p3->Puttext(_bstr_t("ff0000ff"));
			p2->appendChild(p3);
		p1->appendChild(p2);
	pElem->appendChild(p1);

	p1 = pXMLDoc->createElement(_T("Style"));
	p1->setAttribute(_T("id"), _variant_t(_T("s") + styleMidName + _T("_h1")));
		p2 = pXMLDoc->createElement(_T("IconStyle"));
			p3 = pXMLDoc->createElement(_T("color"));
			p3->Puttext(_bstr_t("7f7fff00"));
			p2->appendChild(p3);
			p3 = pXMLDoc->createElement(_T("scale"));
			p3->Puttext(_bstr_t("0.6"));
			p2->appendChild(p3);
			p3 = pXMLDoc->createElement(_T("Icon"));
				p4 = pXMLDoc->createElement(_T("href"));
				p4->Puttext(_bstr_t("http://maps.google.com/mapfiles/kml/pushpin/ylw-pushpin.png"));
			p3->appendChild(p4);
			p2->appendChild(p3);
			p3 = pXMLDoc->createElement(_T("hotSpot"));
			p3->setAttribute(_T("x"), _variant_t("20"));
			p3->setAttribute(_T("y"), _variant_t("2"));
			p3->setAttribute(_T("xunits"), _variant_t("pixels"));
			p3->setAttribute(_T("yunits"), _variant_t("pixels"));
			p2->appendChild(p3);
		p1->appendChild(p2);
		p2 = pXMLDoc->createElement(_T("LabelStyle"));
			p3 = pXMLDoc->createElement(_T("color"));
			p3->Puttext(_bstr_t("7fffaa00"));
			p2->appendChild(p3);
			p3 = pXMLDoc->createElement(_T("scale"));
			p3->Puttext(_bstr_t("1.0"));
			p2->appendChild(p3);
		p1->appendChild(p2);
	pElem->appendChild(p1);

	p1 = pXMLDoc->createElement(_T("StyleMap"));
	p1->setAttribute(_T("id"), _variant_t(_T("m") + styleMidName));
		p2 = pXMLDoc->createElement(_T("Pair"));
			p3 = pXMLDoc->createElement(_T("key"));
			p3->Puttext(_bstr_t("normal"));
			p2->appendChild(p3);
			p3 = pXMLDoc->createElement(_T("styleUrl"));
			p3->Puttext(_bstr_t("#s") + _bstr_t(styleMidName));
			p2->appendChild(p3);
		p1->appendChild(p2);

		p2 = pXMLDoc->createElement(_T("Pair"));
			p3 = pXMLDoc->createElement(_T("key"));
			p3->Puttext(_bstr_t("highlight"));
			p2->appendChild(p3);
			p3 = pXMLDoc->createElement(_T("styleUrl"));
			p3->Puttext(_bstr_t("#s") + _bstr_t(styleMidName)  + _bstr_t("h1"));
			p2->appendChild(p3);
		p1->appendChild(p2);

	pElem->appendChild(p1);
}





// I have also discovered another API that still returns Windows Version without any compatability Manifests required.But it takes a 2 - 3 miliseconds to execute so it is significantly slower than GetVersionEx API.
//#include <lm.h>
//bool GetWindowsVersionSlower(DWORD& major, DWORD& minor)
//{
//	LPBYTE pinfoRawData;
//	if (NERR_Success == NetWkstaGetInfo(NULL, 100, &pinfoRawData))
//	{
//		WKSTA_INFO_100 * pworkstationInfo = (WKSTA_INFO_100 *)pinfoRawData;
//		major = pworkstationInfo->wki100_ver_major;
//		minor = pworkstationInfo->wki100_ver_minor;
//		::NetApiBufferFree(pinfoRawData);
//		return true;
//	}
//	return false;
//}
//int _tmain(int argc, _TCHAR* argv[])
//{
//	DWORD major = 0;
//	DWORD minor = 0;
//	if (GetWindowsVersionSlower(major, minor))
//	{
//		wcout << _T("Major:") << major << _T("Minor:") << minor << endl;
//	}
//	return 0;
//}





//
//   FUNCTION: IsUserInAdminGroup()
//
//   PURPOSE: The function checks whether the primary access token of the 
//   process belongs to user account that is a member of the local 
//   Administrators group, even if it currently is not elevated.
//
//   RETURN VALUE: Returns TRUE if the primary access token of the process 
//   belongs to user account that is a member of the local Administrators 
//   group. Returns FALSE if the token does not.
//
//   EXCEPTION: If this function fails, it throws a C++ DWORD exception which 
//   contains the Win32 error code of the failure.
//
//   EXAMPLE CALL:
//     try 
//     {
//         if (IsUserInAdminGroup())
//             wprintf (L"User is a member of the Administrators group\n");
//         else
//             wprintf (L"User is not a member of the Administrators group\n");
//     }
//     catch (DWORD dwError)
//     {
//         wprintf(L"IsUserInAdminGroup failed w/err %lu\n", dwError);
//     }
//
BOOL IsUserInAdminGroup()
{
	BOOL fInAdminGroup = FALSE;
	DWORD dwError = ERROR_SUCCESS;
	HANDLE hToken = NULL;
	HANDLE hTokenToCheck = NULL;
	DWORD cbSize = 0;

	OSVERSIONINFO osver = { sizeof(osver) };

	// Open the primary access token of the process for query and duplicate.
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE,
		&hToken))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	// Determine whether system is running Windows Vista or later operating 
	// systems (major version >= 6) because they support linked tokens, but 
	// previous versions (major version < 6) do not.
	if (!GetVersionEx(&osver))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	if (osver.dwMajorVersion >= 6)
	{
		// Running Windows Vista or later (major version >= 6). 
		// Determine token type: limited, elevated, or default. 
		TOKEN_ELEVATION_TYPE elevType;
		if (!GetTokenInformation(hToken, TokenElevationType, &elevType,
			sizeof(elevType), &cbSize))
		{
			dwError = GetLastError();
			goto Cleanup;
		}

		// If limited, get the linked elevated token for further check.
		if (TokenElevationTypeLimited == elevType)
		{
			if (!GetTokenInformation(hToken, TokenLinkedToken, &hTokenToCheck,
				sizeof(hTokenToCheck), &cbSize))
			{
				dwError = GetLastError();
				goto Cleanup;
			}
		}
	}

	// CheckTokenMembership requires an impersonation token. If we just got a 
	// linked token, it already is an impersonation token.  If we did not get 
	// a linked token, duplicate the original into an impersonation token for 
	// CheckTokenMembership.
	if (!hTokenToCheck)
	{
		if (!DuplicateToken(hToken, SecurityIdentification, &hTokenToCheck))
		{
			dwError = GetLastError();
			goto Cleanup;
		}
	}

	// Create the SID corresponding to the Administrators group.
	BYTE adminSID[SECURITY_MAX_SID_SIZE];
	cbSize = sizeof(adminSID);
	if (!CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID,
		&cbSize))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	// Check if the token to be checked contains admin SID.
	// http://msdn.microsoft.com/en-us/library/aa379596(VS.85).aspx:
	// To determine whether a SID is enabled in a token, that is, whether it 
	// has the SE_GROUP_ENABLED attribute, call CheckTokenMembership.
	if (!CheckTokenMembership(hTokenToCheck, &adminSID, &fInAdminGroup))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

Cleanup:
	// Centralized cleanup for all allocated resources.
	if (hToken)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}
	if (hTokenToCheck)
	{
		CloseHandle(hTokenToCheck);
		hTokenToCheck = NULL;
	}

	// Throw the error if something failed in the function.
	if (ERROR_SUCCESS != dwError)
	{
		throw dwError;
	}

	return fInAdminGroup;
}




// 
//   FUNCTION: IsRunAsAdmin()
//
//   PURPOSE: The function checks whether the current process is run as 
//   administrator. In other words, it dictates whether the primary access 
//   token of the process belongs to user account that is a member of the 
//   local Administrators group and it is elevated.
//
//   RETURN VALUE: Returns TRUE if the primary access token of the process 
//   belongs to user account that is a member of the local Administrators 
//   group and it is elevated. Returns FALSE if the token does not.
//
//   EXCEPTION: If this function fails, it throws a C++ DWORD exception which 
//   contains the Win32 error code of the failure.
//
//   EXAMPLE CALL:
//     try 
//     {
//         if (IsRunAsAdmin())
//             wprintf (L"Process is run as administrator\n");
//         else
//             wprintf (L"Process is not run as administrator\n");
//     }
//     catch (DWORD dwError)
//     {
//         wprintf(L"IsRunAsAdmin failed w/err %lu\n", dwError);
//     }
//
BOOL IsRunAsAdmin()
{
	BOOL fIsRunAsAdmin = FALSE;
	DWORD dwError = ERROR_SUCCESS;
	PSID pAdministratorsGroup = NULL;

	// Allocate and initialize a SID of the administrators group.
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	if (!AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pAdministratorsGroup))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	// Determine whether the SID of administrators group is enabled in 
	// the primary access token of the process.
	if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

Cleanup:
	// Centralized cleanup for all allocated resources.
	if (pAdministratorsGroup)
	{
		FreeSid(pAdministratorsGroup);
		pAdministratorsGroup = NULL;
	}

	// Throw the error if something failed in the function.
	if (ERROR_SUCCESS != dwError)
	{
		throw dwError;
	}

	return fIsRunAsAdmin;
}


//
//   FUNCTION: IsProcessElevated()
//
//   PURPOSE: The function gets the elevation information of the current 
//   process. It dictates whether the process is elevated or not. Token 
//   elevation is only available on Windows Vista and newer operating 
//   systems, thus IsProcessElevated throws a C++ exception if it is called 
//   on systems prior to Windows Vista. It is not appropriate to use this 
//   function to determine whether a process is run as administartor.
//
//   RETURN VALUE: Returns TRUE if the process is elevated. Returns FALSE if 
//   it is not.
//
//   EXCEPTION: If this function fails, it throws a C++ DWORD exception 
//   which contains the Win32 error code of the failure. For example, if 
//   IsProcessElevated is called on systems prior to Windows Vista, the error 
//   code will be ERROR_INVALID_PARAMETER.
//
//   NOTE: TOKEN_INFORMATION_CLASS provides TokenElevationType to check the 
//   elevation type (TokenElevationTypeDefault / TokenElevationTypeLimited /
//   TokenElevationTypeFull) of the process. It is different from 
//   TokenElevation in that, when UAC is turned off, elevation type always 
//   returns TokenElevationTypeDefault even though the process is elevated 
//   (Integrity Level == High). In other words, it is not safe to say if the 
//   process is elevated based on elevation type. Instead, we should use 
//   TokenElevation.
//
//   EXAMPLE CALL:
//     try 
//     {
//         if (IsProcessElevated())
//             wprintf (L"Process is elevated\n");
//         else
//             wprintf (L"Process is not elevated\n");
//     }
//     catch (DWORD dwError)
//     {
//         wprintf(L"IsProcessElevated failed w/err %lu\n", dwError);
//     }
//
BOOL IsProcessElevated()
{
	BOOL fIsElevated = FALSE;
	DWORD dwError = ERROR_SUCCESS;
	HANDLE hToken = NULL;

	// Open the primary access token of the process with TOKEN_QUERY.
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	// Retrieve token elevation information.
	TOKEN_ELEVATION elevation;
	DWORD dwSize;
	if (!GetTokenInformation(hToken, TokenElevation, &elevation,
		sizeof(elevation), &dwSize))
	{
		// When the process is run on operating systems prior to Windows 
		// Vista, GetTokenInformation returns FALSE with the 
		// ERROR_INVALID_PARAMETER error code because TokenElevation is 
		// not supported on those operating systems.
		dwError = GetLastError();
		goto Cleanup;
	}

	fIsElevated = elevation.TokenIsElevated;

Cleanup:
	// Centralized cleanup for all allocated resources.
	if (hToken)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}

	// Throw the error if something failed in the function.
	if (ERROR_SUCCESS != dwError)
	{
		throw dwError;
	}

	return fIsElevated;
}


//
//   FUNCTION: GetProcessIntegrityLevel()
//
//   PURPOSE: The function gets the integrity level of the current process. 
//   Integrity level is only available on Windows Vista and newer operating 
//   systems, thus GetProcessIntegrityLevel throws a C++ exception if it is 
//   called on systems prior to Windows Vista.
//
//   RETURN VALUE: Returns the integrity level of the current process. It is 
//   usually one of these values:
//
//     SECURITY_MANDATORY_UNTRUSTED_RID (SID: S-1-16-0x0)
//     Means untrusted level. It is used by processes started by the 
//     Anonymous group. Blocks most write access. 
//
//     SECURITY_MANDATORY_LOW_RID (SID: S-1-16-0x1000)
//     Means low integrity level. It is used by Protected Mode Internet 
//     Explorer. Blocks write acess to most objects (such as files and 
//     registry keys) on the system. 
//
//     SECURITY_MANDATORY_MEDIUM_RID (SID: S-1-16-0x2000)
//     Means medium integrity level. It is used by normal applications 
//     being launched while UAC is enabled. 
//
//     SECURITY_MANDATORY_HIGH_RID (SID: S-1-16-0x3000)
//     Means high integrity level. It is used by administrative applications 
//     launched through elevation when UAC is enabled, or normal 
//     applications if UAC is disabled and the user is an administrator. 
//
//     SECURITY_MANDATORY_SYSTEM_RID (SID: S-1-16-0x4000)
//     Means system integrity level. It is used by services and other 
//     system-level applications (such as Wininit, Winlogon, Smss, etc.)  
//
//   EXCEPTION: If this function fails, it throws a C++ DWORD exception 
//   which contains the Win32 error code of the failure. For example, if 
//   GetProcessIntegrityLevel is called on systems prior to Windows Vista, 
//   the error code will be ERROR_INVALID_PARAMETER.
//
//   EXAMPLE CALL:
//     try 
//     {
//         DWORD dwIntegrityLevel = GetProcessIntegrityLevel();
//     }
//     catch (DWORD dwError)
//     {
//         wprintf(L"GetProcessIntegrityLevel failed w/err %lu\n", dwError);
//     }
//
DWORD GetProcessIntegrityLevel()
{
	DWORD dwIntegrityLevel = 0;
	DWORD dwError = ERROR_SUCCESS;
	HANDLE hToken = NULL;
	DWORD cbTokenIL = 0;
	PTOKEN_MANDATORY_LABEL pTokenIL = NULL;

	// Open the primary access token of the process with TOKEN_QUERY.
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	// Query the size of the token integrity level information. Note that 
	// we expect a FALSE result and the last error ERROR_INSUFFICIENT_BUFFER
	// from GetTokenInformation because we have given it a NULL buffer. On 
	// exit cbTokenIL will tell the size of the integrity level information.
	if (!GetTokenInformation(hToken, TokenIntegrityLevel, NULL, 0, &cbTokenIL))
	{
		if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
		{
			// When the process is run on operating systems prior to Windows 
			// Vista, GetTokenInformation returns FALSE with the 
			// ERROR_INVALID_PARAMETER error code because TokenElevation 
			// is not supported on those operating systems.
			dwError = GetLastError();
			goto Cleanup;
		}
	}

	// Now we allocate a buffer for the integrity level information.
	pTokenIL = (TOKEN_MANDATORY_LABEL *)LocalAlloc(LPTR, cbTokenIL);
	if (pTokenIL == NULL)
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	// Retrieve token integrity level information.
	if (!GetTokenInformation(hToken, TokenIntegrityLevel, pTokenIL,
		cbTokenIL, &cbTokenIL))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	// Integrity Level SIDs are in the form of S-1-16-0xXXXX. (e.g. 
	// S-1-16-0x1000 stands for low integrity level SID). There is one and 
	// only one subauthority.
	dwIntegrityLevel = *GetSidSubAuthority(pTokenIL->Label.Sid, 0);

Cleanup:
	// Centralized cleanup for all allocated resources.
	if (hToken)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}
	if (pTokenIL)
	{
		LocalFree(pTokenIL);
		pTokenIL = NULL;
		cbTokenIL = 0;
	}

	// Throw the error if something failed in the function.
	if (ERROR_SUCCESS != dwError)
	{
		throw dwError;
	}

	return dwIntegrityLevel;
}


#pragma endregion


//
//   FUNCTION: ReportError(LPWSTR, DWORD)
//
//   PURPOSE: Display an error dialog for the failure of a certain function.
//
//   PARAMETERS:
//   * pszFunction - the name of the function that failed.
//   * dwError - the Win32 error code. Its default value is the calling 
//   thread's last-error code value.
//
//   NOTE: The failing function must be immediately followed by the call of 
//   ReportError if you do not explicitly specify the dwError parameter of 
//   ReportError. This is to ensure that the calling thread's last-error code 
//   value is not overwritten by any calls of API between the failing 
//   function and ReportError.
//
void ReportError(LPCWSTR pszFunction, DWORD dwError)
{
	wchar_t szMessage[200];
	if (-1 != swprintf_s(szMessage, ARRAYSIZE(szMessage),
		L"%s failed w/err 0x%08lx", pszFunction, dwError))
	{
		MessageBox(NULL, szMessage, L"Error", MB_ICONERROR);
	}
}

void ReportError(LPCWSTR pszFunction)
{
	wchar_t szMessage[200];
	if (-1 != swprintf_s(szMessage, ARRAYSIZE(szMessage),
		L"%s failed w/err 0x%08lx", pszFunction, GetLastError()))
	{
		MessageBox(NULL, szMessage, L"Error", MB_ICONERROR);
	}
}

BOOL DoElevateToAdmin() {
	// Check the current process's "run as administrator" status.
	BOOL fIsRunAsAdmin;
	try
	{
		fIsRunAsAdmin = IsRunAsAdmin();
	}
	catch (DWORD dwError)
	{
		ReportError(L"IsRunAsAdmin", dwError);
		return FALSE;
	}

	// Elevate the process if it is not run as administrator.
	if (!fIsRunAsAdmin)
	{
		wchar_t szPath[MAX_PATH];
		if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
		{
			// Launch itself as administrator.
			SHELLEXECUTEINFO sei = { sizeof(sei) };
			sei.lpVerb = L"runas";
			sei.lpFile = szPath;
			//    sei.hwnd = hWnd;
			sei.nShow = SW_NORMAL;

			AfxMessageBox(_T("SeisWide now needs to temporarily have admin rights for Windows' protected areas."), MB_ICONQUESTION);
			if (!ShellExecuteEx(&sei))
			{
				DWORD dwError = GetLastError();
				if (dwError == ERROR_CANCELLED)
				{
					// The user refused to allow privileges elevation.
					// Do nothing ...
					return FALSE;
				}
			}
			else
			{
				//   EndDialog(hWnd, TRUE);  // Quit itself
			}
		}
	}


	try
	{
		fIsRunAsAdmin = IsRunAsAdmin();
		//if (fIsRunAsAdmin)
		//             AfxMessageBox(L"The process is running as administrator", MB_OK);
		return fIsRunAsAdmin;
	}
	catch (DWORD dwError)
	{
		ReportError(L"IsRunAsAdmin", dwError);
		return FALSE;
	}

}






CTime ctringToCTime(CString str) {
	//int i = 0; // substring index to extract
	//while (AfxExtractSubString(sToken, str, i,_T('/')))
	//{   
	//   //.. 
	//   //work with sToken
	//   //..
	//   i++;
	//}
	CString sToken = _T("");
	AfxExtractSubString(sToken, str, 0, _T('/'));
	int month = _ttoi(sToken);
	AfxExtractSubString(sToken, str, 1, _T('/'));
	int day = _ttoi(sToken);
	AfxExtractSubString(sToken, str, 2, _T('/'));
	int year = _ttoi(sToken);

	if (year < 2000) year = 2000;
	if (month < 1) month = 1;
	if (day < 1) day = 1;
	CTime out = CTime(year, month, day, 0, 0, 0);
	return out;
}

CString dateTimeToCString(CTime aTime) {
	CString s = aTime.Format(_T("%m/%d/%Y"));
	return s;
}



int totDays(int nYear, int nDay, CString t)
{

	int tot_days, temp = ((nYear - 1992) % 4) != 0 ? 28 : 29;
	if (nYear < 1000) nYear += 1900;
	if (t == _T("Jan")) tot_days = nDay;
	else if (t == _T("Feb")) tot_days = 31 + nDay;
	else if (t == _T("Mar")) tot_days = 31 + temp + nDay;
	else if (t == _T("Apr")) tot_days = 31 * 2 + temp + nDay;
	else if (t == _T("May")) tot_days = 31 * 2 + 30 + temp + nDay;
	else if (t == _T("Jun")) tot_days = 31 * 3 + 30 + temp + nDay;
	else if (t == _T("Jul")) tot_days = 31 * 3 + 30 * 2 + temp + nDay;
	else if (t == _T("Aug")) tot_days = 31 * 4 + 30 * 2 + temp + nDay;
	else if (t == _T("Sep")) tot_days = 31 * 5 + 30 * 2 + temp + nDay;
	else if (t == _T("Oct")) tot_days = 31 * 5 + 30 * 3 + temp + nDay;
	else if (t == _T("Nov")) tot_days = 31 * 6 + 30 * 3 + temp + nDay;
	else if (t == _T("Dec")) tot_days = 31 * 6 + 30 * 4 + temp + nDay;
	else return -1;

	return tot_days;
}



//
//using namespace std;
//# include "r8lib.hpp"
//double *shepard_basis_1d(int nd, double xd[], double p, int ni, double xi[])
//
////****************************************************************************80
////
////  Purpose:
////
////    SHEPARD_BASIS_1D evaluates a 1D Shepard basis function.
////
////  Licensing:
////
////    This code is distributed under the GNU LGPL license.
////
////  Modified:
////
////    02 July 2015
////
////  Author:
////
////    John Burkardt
////
////  Reference:
////
////    Donald Shepard,
////    A two-dimensional interpolation function for irregularly spaced data,
////    ACM '68: Proceedings of the 1968 23rd ACM National Conference,
////    ACM, pages 517-524, 1969.
////
////  Parameters:
////
////    Input, int ND, the number of data points.
////
////    Input, double XD[ND], the data points.
////
////    Input, double P, the power.
////
////    Input, int NI, the number of interpolation points.
////
////    Input, double XI[NI], the interpolation points.
////
////    Output, double SHEPARD_BASIS_1D[NI*ND], the basis functions at the interpolation 
////    points.
//// 
//{
//	double *bk;
//	int i;
//	int j;
//	double s;
//	double *w;
//	int z;
//
//	w = new double[nd];
//	bk = new double[ni*nd];
//
//	for (i = 0; i < ni; i++)
//	{
//		if (p == 0.0)
//		{
//			for (j = 0; j < nd; j++)
//			{
//				w[j] = 1.0 / (double)(nd);
//			}
//		}
//		else
//		{
//			z = -1;
//			for (j = 0; j < nd; j++)
//			{
//				w[j] = r8_abs(xi[i] - xd[j]);
//				if (w[j] == 0.0)
//				{
//					z = j;
//					break;
//				}
//			}
//
//			if (z != -1)
//			{
//				for (j = 0; j < nd; j++)
//				{
//					w[j] = 0.0;
//				}
//				w[z] = 1.0;
//			}
//			else
//			{
//				for (j = 0; j < nd; j++)
//				{
//					w[j] = 1.0 / pow(w[j], p);
//				}
//				s = r8vec_sum(nd, w);
//				for (j = 0; j < nd; j++)
//				{
//					w[j] = w[j] / s;
//				}
//			}
//		}
//		for (j = 0; j < nd; j++)
//		{
//			bk[i + j*ni] = w[j];
//		}
//	}
//
//	delete[] w;
//
//	return bk;
//}
//
//void shepard_value_1d(int nd, double xd[], double yd[], double p, int ni,
//	double xi[], double yi[])
//{
//	//****************************************************************************80
//	//
//	//  Purpose:
//	//
//	//    SHEPARD_VALUE_1D evaluates a 1D Shepard interpolant.
//	//
//	//  Licensing:
//	//
//	//    This code is distributed under the GNU LGPL license.
//	//
//	//  Modified:
//	//
//	//    01 October 2012
//	//
//	//  Author:
//	//
//	//    John Burkardt
//	//
//	//  Reference:
//	//
//	//    Donald Shepard,
//	//    A two-dimensional interpolation function for irregularly spaced data,
//	//    ACM '68: Proceedings of the 1968 23rd ACM National Conference,
//	//    ACM, pages 517-524, 1969.
//	//
//	//  Parameters:
//	//
//	//    Input, int ND, the number of data points.
//	//
//	//    Input, double XD[ND], the data points.
//	//
//	//    Input, double YD[ND], the data values.
//	//
//	//    Input, double P, the power.
//	//
//	//    Input, int NI, the number of interpolation points.
//	//
//	//    Input, double XI[NI], the interpolation points.
//	//
//	//    Output, double SHEPARD_VALUE_1D[NI], the interpolated values.
//	//
//	int i;
//	int j;
//	int k;
//	double s;
//	double *w;
//	int z;
//
//	w = new double[nd];
//
//	for (i = 0; i < ni; i++)
//	{
//		if (p == 0.0)
//		{
//			for (j = 0; j < nd; j++)
//			{
//				w[j] = 1.0 / (double)(nd);
//			}
//		}
//		else
//		{
//			z = -1;
//			for (j = 0; j < nd; j++)
//			{
//				w[j] = r8_abs(xi[i] - xd[j]);
//				if (w[j] == 0.0)
//				{
//					z = j;
//					break;
//				}
//			}
//
//			if (z != -1)
//			{
//				for (j = 0; j < nd; j++)
//				{
//					w[j] = 0.0;
//				}
//				w[z] = 1.0;
//			}
//			else
//			{
//				for (j = 0; j < nd; j++)
//				{
//					w[j] = 1.0 / pow(w[j], p);
//				}
//				s = r8vec_sum(nd, w);
//				for (j = 0; j < nd; j++)
//				{
//					w[j] = w[j] / s;
//				}
//			}
//		}
//		yi[i] = r8vec_dot_product(nd, w, yd);
//	}
//	delete[] w;
//
//	return;
//}
//
