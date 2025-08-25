// dlgSegyInfo.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgSegyInfo.h"
#include "SeisWideDoc.h"
#include "segy.h"
#include "bmp24.h"
#include "util.h"
#include "float.h"
#include "math.h"
#include "MainFrm.h"
#include "segyWrapper.h"
#include "segyMeta.h"

//#include "suSwapByte.c"
//#include "CMyOpen.h"
//#define MAXLINE 400     /* The length of each line */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgSegyInfo dialog


CdlgSegyInfo::CdlgSegyInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgSegyInfo::IDD, pParent)
	, m_isAuto(FALSE)
	, m_strValue(BLANK)
{
	//{{AFX_DATA_INIT(CdlgSegyInfo)
	m_timeSecName = BLANK;
//	m_shot1 = 0;
	m_tmax = 0.0f;
	m_tmin = 0.0f;
	m_xmin = 0.0f;
	m_numSamp = 0;
	m_sampInt = 0;
	m_dataFormat = -1;
	m_totNTrace = 0;
	m_CDP = 0;
	m_FFID = 0;
	m_averageAmp = 0.0f;
	m_sDepth = 0.0f;
	m_gDepth = 0.0f;
	m_traceActive = 1;
	//	m_value = 0.f;
	//}}AFX_DATA_INIT
}

void CdlgSegyInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgSegyInfo)
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	//DDX_Text(pDX, IDC_VIEW_UPDATE_SHOT1, m_shot1);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMAX, m_tmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMIN, m_tmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMIN, m_xmin);
	DDX_Text(pDX, IDC_SEGYINFO_NUMSAMP, m_numSamp);
	DDX_Text(pDX, IDC_SEGYINFO_SAMPINT, m_sampInt);
	DDX_Radio(pDX, IDC_RADIO1, m_dataFormat);
	DDX_Text(pDX, IDC_TOTNTRACE, m_totNTrace);
	DDV_MinMaxInt(pDX, m_totNTrace, 0, 1000000);
	DDX_Text(pDX, IDC_VIEW_CDP, m_CDP);
	DDX_Text(pDX, IDC_VIEW_FFID, m_FFID);
	DDX_Text(pDX, IDC_AVERAGEAMP, m_averageAmp);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SDEPTH, m_sDepth);
	DDX_Text(pDX, IDC_VIEW_UPDATE_GDEPTH, m_gDepth);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TRACEACTIVE, m_traceActive);
	DDV_MinMaxInt(pDX, m_traceActive, 0, 99999);
	//	DDX_Text(pDX, IDC_EDIT_VALUE, m_value);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_AUTO, m_isAuto);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_strValue);
}


BEGIN_MESSAGE_MAP(CdlgSegyInfo, CDialog)
	//{{AFX_MSG_MAP(CdlgSegyInfo)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_EN_KILLFOCUS(IDC_VIEW_UPDATE_TRACEACTIVE, OnKillfocusViewUpdateTraceactive)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
//	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON8, OnButton8DumpMoreTraceInfo)
	ON_BN_CLICKED(IDC_BUTTON_EBCDIC, OnButtonEbcdic)
	//}}AFX_MSG_MAP
	ON_COMMAND(IDC_BUTTON_BROWSETPRE, OnBrowseT)
	ON_BN_CLICKED(IDC_BUTTON9, &CdlgSegyInfo::OnBnClickedButtonShottab)
	//ON_BN_CLICKED(IDC_BUTTON_TEST, &CdlgSegyInfo::OnBnClickedButtonTest)
	//ON_BN_CLICKED(IDC_BUTTON_TEST2, &CdlgSegyInfo::OnBnClickedButtonTest2)
	//ON_BN_CLICKED(IDC_BUTTON10, &CdlgSegyInfo::OnBnClickedButton10)
	//ON_BN_CLICKED(IDC_BUTTON11, &CdlgSegyInfo::OnBnClickedButton11)
	//	ON_BN_CLICKED(IDC_BUTTON_SEGYGEN, &CdlgSegyInfo::OnBnClickedButtonSegygen)
	//	ON_BN_CLICKED(IDC_BUTTON19, &CdlgSegyInfo::OnBnClickedButton19)
	ON_EN_CHANGE(IDC_EDIT_VALUE, &CdlgSegyInfo::OnEnChangeEditValue)
	//	ON_BN_CLICKED(IDC_BUTTON20, &CdlgSegyInfo::OnBnClickedButton20)
	ON_BN_CLICKED(IDC_BUTTON20, &CdlgSegyInfo::OnBnClickedButton20)
	ON_BN_CLICKED(IDC_BUTTON_TRACE, &CdlgSegyInfo::OnBnClickedButtonTrace)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgSegyInfo message handlers
void CdlgSegyInfo::OnBrowseT()
{


	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("Segy Time Section (*.sgy)|*.sgy|"));


	// #define MIL 1000000.
	if( dlg.DoModal()==IDOK ) {   
		m_timeSecName = dlg.GetPathName();
		UpdateData(FALSE);
		CalcSegyInfo(m_traceActive);
		UpdateData(FALSE);
	}
}

float CdlgSegyInfo::GetAverageAmp(CString inFile)
{
	// note: inFile must not be opened already!!!
	float out = ERRORVAL;
	HEAD head;
	if (CSegyMeta::segyHeadInit(inFile, &head) != 1) return out;
	SegyWrapper sw(inFile);
	// sw.nxDesired = 100;
	if (sw.openTraceFlatWhole(100)) {
	//	int numsamp = head.numsampFiles;
		out = sw.getAvgAmp();
	} // if sw
	CSegyMeta::segyHeadFree(&head);

	return out;
}


double CdlgSegyInfo::GetMaxAmp(CString inFile)
{
	// note: inFile must not be opened already!!!
	double out = ERRORVAL;
	HEAD head;
	if (CSegyMeta::segyHeadInit(inFile, &head) != 1) return out;
	SegyWrapper sw(inFile);
	// sw.nxDesired = 100;
	if (sw.openTraceFlatWhole(100)) {
	//	int numsamp = head.numsampFiles;
		out = sw.getMaxAmp();
	} // if sw
	CSegyMeta::segyHeadFree(&head);

	return out;
}

void CdlgSegyInfo::CalcSegyInfo(int trc)
{
	//UpdateData(TRUE);
	if (!isFileExistNotDirectory(m_timeSecName)) return;
	HEAD head2;
	if (CSegyMeta::segyHeadInit(m_timeSecName, &head2) != 1) return;
	SegyWrapper sw(m_timeSecName);

	CString str;
	this->m_totNTrace = head2.numTraces;
	bool isTrue = sw.openTrace(trc, trc + 1, 1, &head2, 1, 0.);
	CSegyMeta::segyHeadFree(&head2);  // head2 must free manually, but head should not free
	if (!isTrue) {
		return;
	}

	int nxRead = sw.headRead.getnx();
	m_sampInt = (int)(sw.headRead.getdt() * MIL + 0.5);
	this->m_numSamp = sw.headRead.getns();

	if (nxRead < 1 || this->m_sampInt < 2) {
		SegyWrapper::freeSegyArray(&sw.headRead);
		str.Format(_T("Num of samples per trace: %d. Num of traces touched: %d."), this->m_sampInt, nxRead);
		AfxMessageBox(str);
	}

	this->m_dataFormat = sw.headRead.bh.format;
//	this->m_shot1 = tr.tracl;

	int ixSelected = 0;
	for (int ix = 0; ix < nxRead; ix++) {
		if (sw.headRead.trArr[ix].tracl == trc) {
			ixSelected = ix;
			break;
		}
	}

	this->m_xmin = sw.headRead.trArr[ixSelected].getOffset();
	this->m_tmin = sw.headRead.trArr[ixSelected].getTraceDelay();
	this->m_tmax = this->m_tmin + (float)((double)(this->m_numSamp-1)*(double)this->m_sampInt * OMIL);
	this->m_FFID = sw.headRead.trArr[ixSelected].fldr;
	this->m_CDP = sw.headRead.trArr[ixSelected].cdp;
	this->m_sDepth = (float)(sw.headRead.trArr[ixSelected].swdep /1000.f);
	this->m_gDepth = (float)(sw.headRead.trArr[ixSelected].gwdep /1000.f);
	this->m_averageAmp = GetAverageAmp(m_timeSecName);

	SegyWrapper::freeSegyArray(&sw.headRead);
	//UpdateData(FALSE);

}

void CdlgSegyInfo::OnButton1() 
{
	//	CalcSegyInfo();
	UpdateData(TRUE);
	//	FormatSegy(m_timeSecName);

}

//void CdlgSegyInfo::OnButton3() 
//{
//	UpdateData(TRUE);
//	CalcSegyInfo();
//	//	FormatSegy(m_timeSecName);
//	UpdateData(FALSE);
//
//}

void CdlgSegyInfo::OnKillfocusViewUpdateTraceactive() 
{
	//	m_traceActive
	UpdateData(TRUE); //the data is being retrieved from the controls of the dialog.
	CalcSegyInfo(m_traceActive);  //calculate from the segy.
	UpdateData(FALSE); //the data is being initialized from the existing variables in memory.

}

BOOL CdlgSegyInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CdlgSegyInfo::OnButton2() 
{
	// the user clicked the "Show Ascii Header Info" button
	//show ascii header info.
	UpdateData(TRUE);

	CString  newName;
	CString tStr;

	CFile file;
	if (!file.Open(m_timeSecName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
		return;
	}

	UINT bytesRead = file.Read(tStr.GetBuffer(3200), 3200L);
	if(bytesRead < 3200L) 
	{
		tStr.ReleaseBuffer();
		file.Close();
		AfxMessageBox(_T("Ascii section header read wrong"), MB_ICONQUESTION);
		return;
	}
	file.Close();
	tStr.ReleaseBuffer();
	showStringInFile(tStr);



	//char lpszSystemInfo[MAX_PATH];   /* buffer for concat'd. str. */ 
	//nSize = GetTempPath(MAX_PATH, lpszSystemInfo);
	//nSize =	GetTempPath(_MAX_PATH, newName.GetBuffer());
	//newName.ReleaseBuffer();
	//newName += _T(".tempAsciiHeader.txt");




	//FILE *stream;
	////TCHAR *ac = newName.GetBuffer();
	//if( _t_tfopen_s(&stream, newName, _T("w") ) != 0 )
	//{
	//	AfxMessageBox( _T("Can not open a temporaty file to hold the ascii header") );
	//	return;
	//}
	//newName.ReleaseBuffer();
	//for (i=0; i<3200; i++)
	//	tStr.Format(_T("%c"), ascHead[i]);
	//fclose( stream );

	//showFileInNotePad(newName);
}


void CdlgSegyInfo::OnButtonEbcdic() 
{
	// the user clicked the "Show EBCDIC Header" button
	CString  newName;
	CString tStr;

	if(!isFileExistNotDirectory(m_timeSecName)) return;
	CFile file;
	if (!file.Open(m_timeSecName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
		return;
	}
	UINT bytesRead = file.Read(tStr.GetBuffer(3200), 3200L);
	file.Close();
	if(bytesRead < 3200L)
	{
		AfxMessageBox(_T("Ascii section header read wrong"), MB_ICONQUESTION);
		return;
	}

	ebc2asc( (unsigned char *)tStr.GetBuffer(3200), 3200);

}

void CdlgSegyInfo::OnButton8DumpMoreTraceInfo() 
{
	if (!this->m_isAuto)
		UpdateData(TRUE);  
		// this is needed. If not enabled this line, any manual change to the dialogbox's field for m_timeSecName will not be reflected here.
	// Note: without the if statement, it will crash when no dialigbox is displayed and it wants to update screen data!

	// 	the user clicked the "Dump extra trace info" button
	int len,i;
	unsigned char ascHead[3400];
	segy tr;
	bhed bh;
	BOOL isUnix = false;

	double avgAmp = this->GetAverageAmp(m_timeSecName);
	double maxAmp = this->GetMaxAmp(m_timeSecName);

	CFile file;
	if (!file.Open(m_timeSecName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
		return;
	}

	UINT bytesRead = file.Read(ascHead, 3200L);
	bytesRead = file.Read(&bh, 400);
	if(bytesRead < 400) {
		file.Close();
		AfxMessageBox(_T("Binary section header read wrong"), MB_ICONQUESTION);
		return;
	}

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->statMain.myCursor(true);

	CStringArray *cstrArr = new CStringArray();
	cstrArr->Add(CSegyMeta::segyFormatToString(bh.format));

	if (isUnix = IsUnix(bh.format)) SwapTapeHeader(&bh);

	m_numSamp = bh.hns;
	int sizeofSamp = 0;
	if(bh.format!=3)  {
		sizeofSamp = 4;
	}
	else {
		sizeofSamp = 2;
	}
	len = m_numSamp*sizeofSamp + 240;

	if (bh.hdt == 0) bh.hdt = bh.dto;
	m_sampInt = bh.hdt;



	CString tStr, tStr2;
	tStr.Format(_T("Segy file name: %s\n"), m_timeSecName);
	cstrArr->Add(tStr);
	
	autoAsc2ebc(ascHead, 3200);

	// sometimes the ascii headers only contain C and line numbers, in which case we do not want to display them line by line
	int countUsefulChars = 0;
	for (int i = 0; i < 3200; i++) {
		// note: here I tried isalpha(), isasii() and isnumeric() functions, and only isalpha() works for this purpose of counting useful characters.
		if (ascHead[i] != 'C' && isalpha(ascHead[i])) {
			countUsefulChars++;
		}
	}

	int countC = 0;
	for (int i = 0; i < 40; i++) {
		if (ascHead[i * 80] == 'C') {
			ascHead[i * 80] = '\t';
			countC++;
		}
	}

	if (countUsefulChars>20 && countC > 6) {
		//CStringArray *arr2 = new CStringArray();
		CStringArray arr2;
		if (SplitStringArr(CString(ascHead), TAB, &arr2, false)) {
			for (int i = 0; i < arr2.GetSize(); i++) {
				cstrArr->Add(' ' + arr2.ElementAt(i));
			}
		}
		else
			cstrArr->Add(ascHead);
		arr2.RemoveAll();
	//	delete arr2;
	}
	else
		cstrArr->Add(ascHead);

	//tStr = CA2T((char *)ascHead);

	tStr.Format(_T("\nnumTraces=%d, numSamp=%d; sampInt=%d; TraceLengthInBytes=%d\nAverageAmp: %g; MaxAmp: %g"),
		bh.getnx(), m_numSamp, m_sampInt, len, (float)avgAmp, (float)maxAmp);
	cstrArr->Add(tStr);

	tStr.Format(_T("obsPositionX=%g; gunDelay=%d; offsetClockReset=%d; refdayClockReset=%d; refhrClockReset=%d; refminClockReset=%d; refsecClockReset=%d; latCorr=%g; lonCorr=%g; drftrate=%g")
		, bh.obsPositionX
		, bh.gunDelay
		, bh.offsetClockReset
		, bh.refdayClockReset
		, bh.refhrClockReset
		, bh.refminClockReset
		, bh.refsecClockReset
		, bh.lattCorr
		, bh.longCorr
		, bh.drftrate
	);
	cstrArr->Add(tStr);


	tStr.Format(_T("SP=Shot point; tmin,tmax=time in sec; sDepth=depth of source in km (not used? May differ)"));
	cstrArr->Add(tStr);

	bool isSegyXshiftNeeded = this->m_xShift != 0.f;
	static double lat0 = 90.;
	static double long0 = -90.;
	static double oarc = 1. / 360000.;

	i = 0;
	DWORD fileLength = (DWORD) file.GetLength();
	double glong, glat;
	double glong2, glat2;
	double slong, slat;
	bool isFirst = true;
	try {
		while (file.Read(&tr, 240L) == 240L) {
			if (isUnix) {
				SwapTraceHeader(false, false, false, &tr);
			}
			m_xmin = tr.getOffset();
			m_tmin = tr.getTraceDelay();
			m_tmax = m_tmin + (float)((double)(m_numSamp-1)*(double)m_sampInt * OMIL);
			m_sDepth = (float)(tr.swdep * 0.001f);
			m_gDepth = (float)(tr.gwdep * 0.001f);

			if (tr.counit == 2) {
				glong = tr.gx * oarc;
				glat = tr.gy * oarc;
				slong = tr.sx * oarc;
				slat = tr.sy * oarc;

				glong2 = tr.gx * OMIL;
				glat2 = tr.gy * OMIL;
			}
			else if (tr.counit == 0) {
				glong = tr.gx * OMIL;
				glat = tr.gy * OMIL;
				slong = tr.sx * OMIL;
				slat = tr.sy * OMIL;

				glong2 = tr.gx * oarc;
				glat2 = tr.gy * oarc;
			}
			else {
				glong = tr.gx;
				glat = tr.gy;
				slong = tr.sx;
				slat = tr.sy;

				glong2 = tr.gx * oarc;
				glat2 = tr.gy * oarc;
			}

			//if (glong > 180.f) glong -= 360.; // this cause error when UTM coords are used
			//if (slong > 180.f) slong -= 360.;

			if (isFirst) {
				if (tr.counit == 0)
					tStr.Format(_T("gLon/gLat and sLon/sLat are for coords (i.e. gx,gy divided by 1 million as instructed in header). gLon2/gLat2 assume division by 360,000."));
				else if (tr.counit == 2)
					tStr.Format(_T("gLon/gLat and sLon/sLat are arc coords (i.e. gx,gy divided by 360,000 as instructed in header). gLon2/gLat2 assume division by million."));
				else
					tStr.Format(_T("gLon/gLat and sLon/sLat are GMT coords. Last 2 columns assume division by 360,000."));
				cstrArr->Add(tStr);
				if (isSegyXshiftNeeded) {
					tStr.Format(_T("To match current display, segy file is shifted horizontally by %g km. See (DistShift) for shifted distance, while DIST represents original value in file."), this->m_xShift);
					cstrArr->Add(tStr);
					tStr.Format(_T("Trace  FFID    SP     CDP   CDPT   DIST (DistShift)       offset     tmin     tmax    year  day  hr min sec  msec      gDepth       gLon         gLat      sDepth      sLon         sLat       gLon2        gLat2"));
				}
				else
					tStr.Format(_T("Trace  FFID    SP     CDP   CDPT   MODEL_DIST  offset     tmin     tmax    year  day  hr min sec  msec   gDepth      gLon         gLat          sDepth      sLon         sLat       gLon2        gLat2"));
				cstrArr->Add(tStr);
				isFirst = false;
			}
			//polex = calcDist(long0, glat, glong, glat);
			//if (glong < long0) polex *= -1.;
			//poley = calcDist(glong, lat0, glong, glat);

			register float aDist = tr.getDistance();
			if (isSegyXshiftNeeded)
				tStr.Format(_T("%5d %5d %5d %7d %5d %11.5f (%11.5f) %9.5f %9.5f %9.5f %5d %4d %3d %3d %3d %5d %9.5f %9.5f %11.6f %11.6f %11.6f %11.6f %11.6f %11.6f"), 
				tr.tracl,tr.fldr,tr.ep,tr.cdp, tr.cdpt, aDist, aDist+m_xShift,m_xmin,m_tmin,m_tmax,
				tr.year,tr.day,tr.hour,tr.minute,tr.sec, tr.timbas,
				m_gDepth, glong, glat, m_sDepth, (float)slong, (float)slat, (float)glong2, (float)glat2
				);
			else
				tStr.Format(_T("%5d %5d %5d %7d %5d %11.5f %9.5f %9.5f %9.5f %5d %4d %3d %3d %3d %5d %9.5f %9.5f %11.6f %11.6f %11.6f %11.6f %11.6f %11.6f"), 
				tr.tracl,tr.fldr,tr.ep,tr.cdp,tr.cdpt,aDist,m_xmin,m_tmin,m_tmax,
				tr.year,tr.day,tr.hour,tr.minute,tr.sec, tr.timbas,
				m_gDepth, glong, glat, m_sDepth, (float)slong, (float)slat, (float)glong2, (float)glat2
				);
			cstrArr->Add(tStr);
			i ++;


			// note we cannot use tr.ns to replace m_numSamp
			DWORD pos = (DWORD) file.Seek(m_numSamp * sizeofSamp, CFile::current);	//point to next trace.

		}
	}
	catch (CFileException *e) {
	}

	int nxmaxFile = CSegyMeta::getNumTraceInSegy(&file);
	file.Close();

	pFrame->statMain.myCursor(false);

	int sdfsd = bh.getnx();
	if (sdfsd != nxmaxFile && !isUnix) {
		if (file.Open(m_timeSecName, CFile::modeWrite | CFile::typeBinary)) {
			// now let's update the header only if it is not unix format
			file.Seek(3200L, CFile::begin);
			bh.setnx(nxmaxFile);
			file.Write(&bh, 400L);
			file.Close();
		}
		tStr.Format(_T("Number of traces in the file header is adjusted to %d from %d."), nxmaxFile, sdfsd);
		AfxMessageBox(tStr);
	}

	showStringArrayToFile(cstrArr);
	cstrArr->RemoveAll();
	delete cstrArr;

}

//
//int totDays(int nYear, int nDay, CString t)
//{
//	int tot_days;
//	int temp=((nYear-1992)%4)!=0?28:29;
//	//	int temp=((nYear-92)%4)!=0?28:29;
//	if(nYear<1000) nYear+=2000;
//	if(t == _T("Jan")) tot_days = nDay;
//	else if (t == _T("Feb")) tot_days = 31+nDay;
//	else if (t == _T("Mar")) tot_days = 31+temp+nDay;
//	else if (t == _T("Apr")) tot_days = 31*2+temp+nDay;
//	else if (t == _T("May")) tot_days = 31*2+30+temp+nDay;
//	else if (t == _T("Jun")) tot_days = 31*3+30+temp+nDay;
//	else if (t == _T("Jul")) tot_days = 31*3+30*2+temp+nDay;
//	else if (t == _T("Aug")) tot_days = 31*4+30*2+temp+nDay;
//	else if (t == _T("Sep")) tot_days = 31*5+30*2+temp+nDay;
//	else if (t == _T("Oct")) tot_days = 31*5+30*3+temp+nDay;
//	else if (t == _T("Nov")) tot_days = 31*6+30*3+temp+nDay;
//	else if (t == _T("Dec")) tot_days = 31*6+30*4+temp+nDay;
//	else return -1;
//
//	return tot_days;
//
//}

#include "ShotTable.h"
void CdlgSegyInfo::OnBnClickedButtonShottab()
{
	UpdateData(TRUE);

	// This button will create shot table format
	int len,i;
	unsigned char ascHead[3400];
	segy tr;
	bhed bh;
	BOOL isUnix = false;

	static double obslat = 44 + (46.692 + 46.717) / 60 / 2;
	static double obslong = -55 - (40.88 + 40.833) / 60 / 2;
	static BOOL isByMil = TRUE;
	CShotTable dlg;
	dlg.m_lat = obslat;
	dlg.m_long = obslong;
	if (dlg.DoModal() == IDOK) {
		obslat = dlg.m_lat;
		obslong = dlg.m_long;
	}
	else return;
	//double divFactor = isByMil ? 1./MIL : 1./360000.;
	double divFactor = 0.001;

	CFile file;
	if (!file.Open(m_timeSecName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
		return;
	}

	UINT bytesRead = file.Read(ascHead, 3200L);
	bytesRead = file.Read(&bh, 400);
	if(bytesRead < 400) 
	{
		file.Close();
		AfxMessageBox(_T("Binary section header read wrong"), MB_ICONQUESTION);
		return;
	}
	if (isUnix = IsUnix(bh.format)) SwapTapeHeader(&bh);

	m_numSamp = bh.hns;
	int sizeofSamp = 0;
	if(bh.format!=3) {
		sizeofSamp = 4;
	}
	else {
		sizeofSamp = 2;
	}
	len = m_numSamp*sizeofSamp + 240;

	m_sampInt = bh.hdt;



	//for (i=0; i<3200; i++) _ftprintf_s(stream, "%c", ascHead[i]);
	CStringArray *cstrArr = new CStringArray();
	CString tStr;
	tStr.Format(_T(" Note: for creating a shot table similar to Dalhousie's OBS shot table.")); cstrArr->Add(tStr);
	tStr = CString(ascHead);
	// tStr.Truncate(80);
	cstrArr->Add(tStr);

	//rewind(stream);
	CString ID = this->m_timeSecName;

	int calday1=totDays(2007,21, _T("Jun")), calhr1=16, calmin1=21;
	double calsec1 = 19.;
	int calday2=totDays(2007,22, _T("Jun")), calhr2=23, calmin2=37, gundelay=35;
	double calsec2 = 0.;

	/* Convert time to decimal hour & calculate drift rate */
	double t1=calday1*24+calhr1+calmin1/60.0+calsec1/3600;
	double t2=calday2*24+calhr2+calmin2/60.0+calsec2/3600;
	double driftRate = 1346.093 /(t2-t1);
	//	driftRate = 0.;

	// and lastly, the distance in meters from GPS location to the actual gun location
	// double gunOffsetBehind = 69.2;  // for line 7,9
	double gunOffsetBehind = -69.2;  // for line 11

	/* Line 1 */
	tStr.Format(_T("%s"), ID); cstrArr->Add(tStr);
	/* Line 2 */
	tStr.Format(_T("%12.6f%12.6f  OBS/Sonobuoy Ref. latitude and longitude"),obslat, obslong); cstrArr->Add(tStr);
	/* Line 3 */
	tStr.Format(_T("%3d%3d%3d%3.0f.         Time of clock reset"),calday1, calhr1, calmin1, calsec1); cstrArr->Add(tStr);
	/* Line 4 */
	tStr.Format(_T("   %8.4f            Clock drift rate (ms/hr)"), driftRate); cstrArr->Add(tStr);
	/* Line 5 */
	tStr.Format(_T(" %4d                 Delay in firing gun (interger msec)"), gundelay); cstrArr->Add(tStr);
	/* Line 6 */
	tStr.Format(_T("    0                 Offset(interger msec):obs&ship clocks")); cstrArr->Add(tStr);
	/* Line 7 */
	tStr.Format(_T(" SHOT DAY HR MM     SEC       LATITUDE   LONGITUDE   OFFSET   DEPTH      CDP     DISTANCE OFFSET_CALC    OFFSET_DIFF")); cstrArr->Add(tStr);

	//	tStr.Format(_T("numSamp=%9d\nsampInt=%9d\nTraceLengthInBytes=%9d", m_numSamp, m_sampInt,len);
	//	tStr.Format(_T("Trace  FFID    SP     CDP  CDPBak   offset     tmin     tmax       sDepth     sLong     sLat     gDepth     gLong     gLat    year  day  hr min sec\n");

	//	file.Seek(3600L + len*7289, CFile::begin);	//point to next trace.
	//	file.Seek(3600L, CFile::begin);	//point to next trace.
	i = 0;
	DWORD fileLength = (DWORD) file.GetLength();
	float glong, glat;
	//double xmin_old = 0.;
	//double aitime_old = 0.;
	double offset;
	double offset_calc = 0.;
	try {
		while (file.Read(&tr, 240L) == 240L) {
			if (isUnix) {
				SwapTraceHeader(false, false, false, &tr);
			}
			offset = tr.getOffset();
			m_tmin = tr.getTraceDelay();
			m_tmax = m_tmin + (float)((double)(m_numSamp-1)*(double)m_sampInt/(double)MIL);
			//	m_FFID = tr.fldr;
			//	m_CDP = tr.cdp;
			m_sDepth = (float)(tr.swdep * 0.001f);
			m_gDepth = (float)(tr.gwdep * 0.001f);

			/*
			Shot table example:

			Hudson  98-042              Test    line                     #              1   
			44.354252  -63.046967  OBS Ref. latitude and longitude
			246 12 48  0.         Time of clock calibration
			0.0000            Clock drift rate (ms/hr)
			0                 Delay in firing guns (integer msec)
			0                 Offset(integer msec):obs&ship clocks
			SHOT DAY HR MM     SEC   LATITUDE   LONGITUDE    RANGE  DEPTH
			1001 246 16  8 59.7425  44.323433  -63.129883   -7.444  164.5
			1002 246 17 39 59.7727  44.322548  -63.129883   -7.490  161.5
			1003 246 17 40 59.7366  44.324322  -63.127918   -7.259  160.5
			*/

			offset_calc = (float)calcDist(obslong, obslat, (tr.gx*divFactor), (tr.gy*divFactor));
			//if (tr.gx*divFactor < obslong) {
			//	offset = -offset; // we define west of OBS as negative offset
			//	offset_calc = -offset_calc;
			//}

			// actual shot location should be corrected since the GPS location is offset: gunOffsetBehind
			// NOTE: the ship's direction matters for the sign of the correct. Also we usually treat west of the OBS as negative distance
			// for Line 007, ship goes from east to west, 
			//m_xmin += (float)(gunOffsetBehind/1000.0);

			/*------------------------------------------------------/
			/	Shot time correction due to OBS clock drift         /
			/  or calculation of shot time according to obs clock /
			/---------------------------------------------------*/
			// timing correction
			/* original shot time in decimal hour */
			double aitime=tr.day*24+tr.hour+tr.minute/60.0+(tr.sec + tr.timbas/1000.0)/3600.0;
			double tdrift=(aitime-t1)*driftRate;  // output milisecond
			/* gun delay, drift and stactic correction (in sec) */
		//	double tcor=(gundelay+tdrift)/1000.0;
			double asec = tr.sec + tr.timbas/1000.0;

			///* check if time corrected second is >= 60 or < 0 */
			//while (asec >= 60) {
			//	asec=asec-60;
			//	tr.minute=tr.minute+1;
			//}
			//while (asec < 0) {
			//	asec=asec+60;
			//	tr.minute=tr.minute-1;
			//}

			///* check if time corrected minute is >= 60  or < 0 */
			//while (tr.minute >= 60) {
			//	tr.minute=tr.minute-60;
			//	tr.hour=tr.hour+1;
			//}
			//while (tr.minute < 0) {
			//	tr.minute=tr.minute+60;
			//	tr.hour=tr.hour-1;
			//}

			///* check if time corrected hour is >= 24  or < 0 */
			//while (tr.hour >= 24) {
			//	tr.hour=tr.hour-24;
			//	tr.day=tr.day+1;
			//}
			//while (tr.hour < 0) {
			//	tr.hour=tr.hour+24;
			//	tr.day=tr.day-1;
			//}
			
			glong = (float)(tr.gx*divFactor);
			if (glong > 180.f) glong -= 360.f; // this cause error with UTM
			glat = (float)(tr.gy*divFactor);

			// correction in unit of hour
			aitime = tr.day * 24 + tr.hour + tr.minute / 60.0 + (tr.sec + tr.timbas / 1000.0) / 3600.0;
			//if (xmin_old != 0.) {
			//	register double delta = (aitime - aitime_old); // in hour
			//	offset_var_speed = (aitime - aitime_old) == 0. ? 0. : (getOffset(&tr) - xmin_old) / delta / 3600. * 1000.;  // in m/s
			//}
			//else offset_var_speed = 0.;
			//xmin_old = getOffset(&tr);
			//aitime_old = aitime;

			register float aDist = tr.getDistance();

			// writing
			tStr.Format(_T("%5d %3d %2d %2d %11.6f %11.6f %11.6f %8.5f %6.1f %9d %11.6f %11.6f %11.6f"),
				tr.fldr, tr.day, tr.hour, tr.minute, (float)asec, glat, glong, 
				(float)offset, m_sDepth, tr.cdp, aDist,
				(float)offset_calc , 
				(float)(fabs(offset) - fabs(offset_calc))
				);
			cstrArr->Add(tStr);
			i ++;


			// note we cannot use tr.ns to replace m_numSamp
			DWORD pos = (DWORD) file.Seek(m_numSamp * sizeofSamp, CFile::current);	//point to next trace.

		}
	}
	catch (CFileException *e) {
	}

	file.Close();
	showStringArrayToFile(cstrArr);
	cstrArr->RemoveAll();
	delete cstrArr;

	if (tr.ns != m_numSamp) {
		AfxMessageBox(_T("Number of sample in the segy header is different from the trace header. The former value is used."), MB_ICONQUESTION);
		//	int kkk = 0;
	}

	//showFileInNotePad(newName);
}
//
//
//int CdlgSegyInfo::totDays(int nYear, int nDay, CString t)
//{
//	//	t = _T("Sep");  //only for DOBS 2 Scotia Margin experiment.
//	int tot_days;
//	//	int temp=((nYear-1992)%4)!=0?28:29;
//	int temp=((nYear-92)%4)!=0?28:29;
//	if(nYear<1000) nYear+=2000;
//	if(t == _T("Jan")) tot_days = nDay;
//	else if (t == _T("Feb")) tot_days = 31+nDay;
//	else if (t == _T("Mar")) tot_days = 31+temp+nDay;
//	else if (t == _T("Apr")) tot_days = 31*2+temp+nDay;
//	else if (t == _T("May")) tot_days = 31*2+30+temp+nDay;
//	else if (t == _T("Jun")) tot_days = 31*3+30+temp+nDay;
//	else if (t == _T("Jul")) tot_days = 31*3+30*2+temp+nDay;
//	else if (t == _T("Aug")) tot_days = 31*4+30*2+temp+nDay;
//	else if (t == _T("Sep")) tot_days = 31*5+30*2+temp+nDay;
//	else if (t == _T("Oct")) tot_days = 31*5+30*3+temp+nDay;
//	else if (t == _T("Nov")) tot_days = 31*6+30*3+temp+nDay;
//	else if (t == _T("Dec")) tot_days = 31*6+30*4+temp+nDay;
//	else return -1;
//
//	return tot_days;
//
//}

//long CdlgSegyInfo::myRound(double x)
//{
//	if(x>0.)
//		return (long)(x+0.5);
//	else
//		return (long)(x-0.5);
//}



void CdlgSegyInfo::OnEnChangeEditValue()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the  mask.

	UpdateData(TRUE); //the data is being retrieved from the controls of the dialog.
}




//#include "log4cpp\include\log4cpp\Category.hh"
//#include "log4cpp\include\log4cpp\PropertyConfigurator.hh"
void CdlgSegyInfo::OnBnClickedButton20()
{
	// testing
	//int ncurve = 0;
	//int npoints = 0;
	//CDigitize::getKmlDimension(_T("H:\\arcticAllDeping\\arcticWork2016\\nav\\NP_22.shp.point.kml"),
	//	ncurve, npoints);
	//return;

	UpdateData(TRUE); //the data is being retrieved from the controls of the dialog.
	//		SearchReplaceAllFiles(_T("G:\\arcticSBs\\atlas\\atlasFinals\\Page2PdfFinal"), _T("_"), _T("-"));
	//		SearchReplaceAllFiles(_T("G:\\arcticSBs\\atlas\\Page2"), _T("_"), _T("-"));
	//SearchReplaceAllFiles(_T("G:\\arcticSBs\\atlas\\atlasFinals\\Page3PdfFinal"), 
	//	_T(".pdf"), _T("Page3.pdf"), _T("Page3"));
	//SearchReplaceAllFiles(_T("G:\\arcticSBs\\atlas\\atlasFinals\\Page1PdfFinal"),
	//	_T("sb"), BLANK, BLANK);


	if (m_strValue.IsEmpty()) {
		return;
	}


	CLicenseCheck ls;

	CString str = this->m_timeSecName.Trim();
	CString hardWareId;
	CString strUsageMode = _T("REFL");
	CString licenseName =  _T("Deping Chian, Chian Consulting Inc.");

	// parse it
	AfxExtractSubString(hardWareId, str, 0, _T(':'));
	AfxExtractSubString(strUsageMode, str, 1, _T(':'));
	AfxExtractSubString(licenseName, str, 2, _T(':'));

	char * license = ls.getPersonalKey();
	CString thePersonalKey(license);
	free1(license);

	if (this->m_strValue.Compare(thePersonalKey) == 0) {
		CString t2;

		if (hardWareId.GetLength() > 6) {
			// only when the user enters the correct personal key, I will calculate the license file.
			// int nDaysAllowed = 30;  // IMPORTANT!!!!!!!!!!!!!!!!!!!!!!!!!!
			t2 = ls.createActLicense(hardWareId, strUsageMode, licenseName, this->m_traceActive);
			showStringInFile(t2);

			// testing to parse the license code:
			if (ls.parseLicenseFile2(ls.decodeActLicense(t2))) {
				t2.Format(_T("Confirmed the generated %s license: %d days, valid between %s and %s, %s"),
					ls.licenseEnum2Str(ls.licenseStruct.type),
					ls.licenseStruct.nDaysAllowed,
					dateTimeToCString(ls.licenseStruct.offerTimeStart),
					dateTimeToCString(ls.licenseStruct.offerTimeExpire),
					ls.licenseStruct.strLicenseName
				);
			}
			else {
				// if failed, prompt for correct way
				t2.Format(_T("Example: FD37-A58D-0000:FULL:Chian Consulting Inc."));
			}
		}
		else
			t2.Format(_T("Example: FD37-A58D-0000:FULL:Chian Consulting Inc."));
		AfxMessageBox(t2);
	}

}


void CdlgSegyInfo::OnBnClickedButtonTrace()
{
	// Dump trace values
	UpdateData(TRUE);
	 
	HEAD head;
	if (CSegyMeta::segyHeadInit(m_timeSecName, &head) != 1) return;

	CString strHead;
	strHead.Format(
		_T("#Information for file based on the first 100 traces: %s\n#Average & max amp=%g, %g,\n#Average distance & offset intervals=%g, %g, Min & max offsets=%g, %g,\n#Min & max trace=%d, %d,\n#Min & max time=%g, %g\n\n"),
		m_timeSecName,
		head.aveAmp,
		head.maxAmp,
		head.distIntervalFile,
		head.offsetIntervalFile,
		head.offsetMin(),
		head.offsetMax(),
		head.traceMin(),
		head.traceMax(),
		head.tminFile,
		head.tmaxFile);
	if (m_traceActive < head.traceMin()) m_traceActive = head.traceMin();
	else if (m_traceActive > head.traceMax()) m_traceActive = head.traceMax();
	SegyWrapper sw(m_timeSecName);
	if (sw.openTrace(m_traceActive, m_traceActive+1, 1, &head, 1, 0.)) {
		int nxRead = sw.headRead.getnx();
		this->m_numSamp = sw.headRead.getns();
		m_sampInt = (int)(sw.headRead.getdt() * MIL + 0.5);
		this->m_totNTrace = head.numTraces;
		this->m_dataFormat = sw.headRead.bh.format;


		int ixSelected = 0;
		for (int ix = 0; ix < nxRead; ix++) {
			if (sw.headRead.trArr[ix].tracl == m_traceActive) {
				ixSelected = ix;
				break;
			}
		}

		this->m_xmin = sw.headRead.trArr[ixSelected].getOffset();
		this->m_tmin = sw.headRead.trArr[ixSelected].getTraceDelay();
		this->m_tmax = this->m_tmin + (float)((double)(this->m_numSamp - 1)*(double)this->m_sampInt * OMIL);
		this->m_FFID = sw.headRead.trArr[ixSelected].fldr;
		this->m_CDP = sw.headRead.trArr[ixSelected].cdp;
		this->m_sDepth = (float)(sw.headRead.trArr[ixSelected].swdep / 1000.f);
		this->m_gDepth = (float)(sw.headRead.trArr[ixSelected].gwdep / 1000.f);
		this->m_averageAmp = GetAverageAmp(m_timeSecName);
		bool isSegyXshiftNeeded = this->m_xShift != 0.f;
		
		CString strTrace;
		strTrace.Format(_T("#Dump for trace %d, numsamp=%d, startTime=%g s, maxTime=%g s, sampInterval=%d, \n# avgAmp=%g, maxAmp=%g, avgAmpFile=%g"),
			m_traceActive, m_numSamp, m_tmin, m_tmax, m_sampInt, 
			sw.getAvgAmpTrace(m_traceActive), sw.getMaxAmpTrace(m_traceActive), m_averageAmp);

		showArrayInFile(sw.headRead.trArr[0].data, sw.headRead.getns(), strHead + strTrace, sw.headRead.getdt());
	} // if sw
	CSegyMeta::segyHeadFree(&head);

	UpdateData(FALSE);

}
