// dlgDEPTH1.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgDEPTH1.h"
#include "segy.h"
#include "util.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgDEPTH1 property page

IMPLEMENT_DYNCREATE(CdlgDEPTH1, CPropertyPage)

CdlgDEPTH1::CdlgDEPTH1() : CPropertyPage(CdlgDEPTH1::IDD)
, m_rayTracing(BLANK)
, m_digName(BLANK)
, m_markerName(BLANK)
, m_xMultFactor(1)
, m_xShift(0)
, m_yMultFactor(1)
, m_yShift(0)
, m_distShift(0)
, m_isShowLabel4LineDrawing(FALSE)
, m_markerRevYAxis(FALSE)
{
	//{{AFX_DATA_INIT(CdlgDEPTH1)
	//m_dz = 0.0;
	m_zmax = 0.0;
	m_zmin = 0.0;
	m_depthSecName = BLANK;
	m_zTick = 0.0;
	m_gainDist = FALSE;
	m_travelTime = FALSE;
	m_tmax = 0.0f;
	m_tmin = 0.0f;
	m_tTick = 0.0f;
	m_vred = 0.0f;
	m_baseGain = 0.0f;
	m_basePower = 0.0f;
	//}}AFX_DATA_INIT
}

CdlgDEPTH1::~CdlgDEPTH1()
{
}

void CdlgDEPTH1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgDEPTH1)
	//DDX_Text(pDX, IDC_CONVERSION_DZ2, m_dz);
	//DDV_MinMaxDouble(pDX, m_dz, 1.e-006, 100.0);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMIN, m_tmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMAX, m_tmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TTICK2, m_tTick);
	DDX_Text(pDX, IDC_DIST_SHIFT, m_distShift);
	DDX_Text(pDX, IDC_VIEW_UPDATE_VRED, m_vred);
	DDX_Check(pDX, IDC_TRAVELTIME2, m_travelTime);
	DDX_Check(pDX, IDC_TIME_GAINDIST, m_gainDist);
	DDX_Text(pDX, IDC_TIME_BASEGAIN, m_baseGain);
	DDX_Text(pDX, IDC_TIME_BASEPOWER, m_basePower);
	DDX_Text(pDX, IDC_DLG_RAYTRACING, m_rayTracing);
	DDX_Text(pDX, IDC_DEPTHSECNAME, m_depthSecName);
	DDX_Text(pDX, IDC_CONVERSION_ZMIN, m_zmin);
	DDX_Text(pDX, IDC_CONVERSION_ZMAX, m_zmax);
	DDX_Text(pDX, IDC_CONVERSION_ZTICK, m_zTick);
	DDX_Text(pDX, IDC_DLG_DIGNAME, m_digName);
	DDX_Check(pDX, IDC_IS_SHOWLABEL, m_isShowLabel4LineDrawing);
	DDX_Text(pDX, IDC_MARKERLOCNAME, m_markerName);
	DDX_Check(pDX, IDC_REV_POLARITY, m_markerRevYAxis);

	DDX_Text(pDX, IDC_XMULT, m_xMultFactor);
	DDX_Text(pDX, IDC_XSHIFT, m_xShift);
	DDX_Text(pDX, IDC_YMULT, m_yMultFactor);
	DDX_Text(pDX, IDC_YSHIFT, m_yShift);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgDEPTH1, CPropertyPage)
	//{{AFX_MSG_MAP(CdlgDEPTH1)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_BROWSERIN2, &CdlgDEPTH1::OnBnClickedButtonBrowserin2)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEDIG, &CdlgDEPTH1::OnBnClickedButtonBrowsedig)
	// ON_BN_CLICKED(IDC_BUTTON_COLOR2, &CdlgDEPTH1::OnBnClickedButtonColor2)
	ON_BN_CLICKED(IDC_BUTTON_BROWSED, &CdlgDEPTH1::OnBnClickedButtonBrowsed)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV3, &CdlgDEPTH1::OnBnClickedButtonBrowsev3)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgDEPTH1 message handlers


BOOL CdlgDEPTH1::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
/*	IDC_VIEW_UPDATE_SHOT1, IDC_VIEW_UPDATE_SHOT2, IDC_VIEW_UPDATE_SHOTSTEP
	IDC_VIEW_UPDATE_GAIN, IDC_VIEW_UPDATE_CLIP, IDC_VIEW_UPDATE_FILLOFF
IDC_VIEW_WIGGLE, IDC_RADIO2, IDC_RADIO3, IDC_RADIO4, IDC_RADIO8, IDC_SEGY_ORDER,

IDC_TIMESECNAME,  IDC_BUTTON_BROWSET
IDC_VIEW_UPDATE_TMIN, IDC_VIEW_UPDATE_TMAX, IDC_VIEW_UPDATE_TTICK2
IDC_VIEW_OFFSET2, IDC_VIEW_DIST, IDC_VIEW_UPDATE_VRED, IDC_TRAVELTIME2
IDC_TIME_GAINDIST, IDC_TIME_BASEGAIN, IDC_TIME_BASEPOWER, 

IDC_DEPTHSECNAME, IDC_BUTTON_BROWSED, IDC_CONVERSION_ZMIN, IDC_CONVERSION_ZMAX,IDC_CONVERSION_ZTICK
*/


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CdlgDEPTH1::OnBnClickedButtonBrowserin2()
{
		UpdateData(TRUE);
	CFileDialog dlg( TRUE,_T("in"),_T("*.in"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Zelt's Ray Parameter File (*.in)|*.in|"));

	int pathLen = m_rayTracing.ReverseFind( '\\' );

	CString t = m_rayTracing.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_rayTracing = dlg.GetPathName();
		//CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DLG_RAYTRACING));
		//pEdit->SetWindowText(m_rayTracing);
		// Apply to synchronize to file
		UpdateData(FALSE);
	}
	
	t.ReleaseBuffer();
}

void CdlgDEPTH1::OnBnClickedButtonBrowsedig()
{
	UpdateData(TRUE);
	CFileDialog dlg( TRUE,_T("txt"),_T("*.txt"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Line drawing File (*.txt)|*.txt|"));

	int pathLen = m_rayTracing.ReverseFind( '\\' );

	CString t = m_rayTracing.Left(pathLen);

	//LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	//t.ReleaseBuffer();

	if(pathLen>(-1)) {
		dlg.m_ofn.lpstrInitialDir = t;
	}

	if( dlg.DoModal()==IDOK ) {
		m_digName = dlg.GetPathName();

		// addition Mar. 27, 2014
		// if the txt file is 
		/*
#TWT    VEL    DEPTH   VRMS  DBEPTH-BV4 MODEL (sec-km/s-km-km)
 0.000  1.510  0.000  1.510  0.000      project\SeisWide.net\work73VS2012\docs\SeisWide\Demos\DemoVMod  
 6.520  1.520  4.939  1.515  0.000      project\SeisWide.net\work73VS2012\docs\SeisWide\Demos\DemoVMod  
 6.530  1.520  4.949  1.515  0.000      project\SeisWide.net\work73VS2012\docs\SeisWide\Demos\DemoVMod  
 6.816  2.200  5.211  1.531  0.000      project\SeisWide.net\work73VS2012\docs\SeisWide\Demos\DemoVMod  
 6.826  2.200  5.221  1.531  0.000      project\SeisWide.net\work73VS2012\docs\SeisWide\Demos\DemoVMod  
 7.151  2.300  5.589  1.572  0.000      project\SeisWide.net\work73VS2012\docs\SeisWide\Demos\DemoVMod  
 7.161  2.700  5.599  1.572  0.000      project\SeisWide.net\work73VS2012\docs\SeisWide\Demos\DemoVMod  
 or if if 3 column only
#VEL    DEPTH   MODEL (sec-km/s-km-km)
 1.510  0.000  project\SeisWide.net\work73VS2012\docs\SeisWide\Demos\DemoVMod  
 1.520  4.939  project\SeisWide.net\work73VS2012\docs\SeisWide\Demos\DemoVMod  

 if it is 6 colum, we need to first convert it to 3 column for proper plotting
 		*/

		int is6Columns = isTxtFile6Columns(m_digName);
		if (is6Columns) {
			// since it is 6 columns, we need to convert it to proper digitizing format for properly displaying

			if ( (t = conv6ColTo3Col(m_digName)) == m_digName) {
				AfxMessageBox(_T("6 columns detected, but cannot convert or save to 3 columns digitizing format."), MB_ICONQUESTION);
			}
			else {
				AfxMessageBox(_T("6 columns are converted and saved to 3 columns in digitizing format."), MB_ICONQUESTION);
				m_digName = t;
				showFileInNotePad(m_digName);
			}

			//CStringArray arr;
			//CStringArray arrT;
			//CStringArray arrOut;
			//CString cstr;
			//getLinesFromFile2(&arr, m_digName, false, 0);
			//bool isSeaSurfacePrev = false;
			//double waterDepth = 0.;
			//for (int i=0; i<arr.GetCount(); i++) {
			//	cstr = arr.ElementAt(i).Trim();
			//	cstr.Replace(TAB, SPACE);
			//	SplitString(cstr, SPACE, arrT);
			//	if (arrT.GetCount() < 6) continue;
			//	register double v = StringToFloat(arrT.ElementAt(1));
			//	register double depth = StringToFloat(arrT.ElementAt(2));

			//	// identify if this line is for the sea surface
			//	if ( fabs(v - 1.44) < 0.01) {
			//		isSeaSurfacePrev = true;
			//		continue;
			//	}

			//	if (isSeaSurfacePrev) {
			//		waterDepth = depth;
			//		isSeaSurfacePrev = false;
			//	}

			//	depth -= waterDepth;

			//	cstr.Format(_T("%f %f %s"), v, depth, arrT.ElementAt(5));
			//	arrOut.Add(cstr);

			//}

			//m_digName = getFileNameAppend(m_digName, _T("_3Col"));
			//if (saveStringArrayToFile(&arrOut, m_digName))
			//	showFileInNotePad(m_digName);
			//arr.RemoveAll();
			//arrT.RemoveAll();
			//arrOut.RemoveAll();
		}




		UpdateData(FALSE);
	}
	
}


void CdlgDEPTH1::OnBnClickedButtonBrowset()
{

}

void CdlgDEPTH1::OnBnClickedButtonBrowsed()
{
		UpdateData(TRUE);
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Depth Section (*.sgy)|*.sgy|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Depth Section |sgy |"));
#endif

	int pathLen = m_depthSecName.ReverseFind( '\\' );
	CString t = m_depthSecName.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_depthSecName = dlg.GetPathName();
		//CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DEPTHSECNAME));
		//pEdit->SetWindowText(m_depthSecName);
		// Apply to synchronize to file
		UpdateData(FALSE);
	}
	t.ReleaseBuffer();
}


BOOL CdlgDEPTH1::OnSetActive()
{
	
	CWnd* pwnd;
	if(mode<0) {
	}
	else if (isTimeDomainUtil(mode))
	{ //time section.
		pwnd = GetDlgItem(IDC_DEPTHSECNAME); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_BUTTON_BROWSED); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_CONVERSION_ZMIN); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_BUTTON_BROWSED); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_CONVERSION_ZMAX); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_CONVERSION_ZTICK); if (pwnd) pwnd->EnableWindow(FALSE); 
	}
	else if (!isTimeDomainUtil(mode))
	{ //depth section.
		pwnd = GetDlgItem(IDC_TIMESECNAME); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_BUTTON_BROWSET); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_TMIN); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_TMAX); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_TTICK2); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_OFFSET2); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_DIST); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_VRED); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_TIME_GAINDIST); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_TIME_BASEGAIN); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_TIME_BASEPOWER); if (pwnd) pwnd->EnableWindow(FALSE);
	}

	if(mode == 2 || mode == 4)
	{ //velocity model only.
		pwnd = GetDlgItem(IDC_DEPTHSECNAME); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_BUTTON_BROWSED); if (pwnd) pwnd->EnableWindow(FALSE);
	}
	
	return CPropertyPage::OnSetActive();
}


void CdlgDEPTH1::OnBnClickedButtonBrowsev3()
{
	// user clicked the marker browser button
	CFileDialog dlg( TRUE,_T("txt"),_T("*.txt;*.obs"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("X-Y Marker File (free text format, 2 columns) (*.txt;*.obs)|*.txt;*.obs|Observed points (*.obs)|*.obs||"));


	int pathLen = m_markerName.ReverseFind( '\\' );
	CString t = m_markerName.Left(pathLen);

	dlg.m_ofn.lpstrInitialDir = m_markerName;
	if (dlg.DoModal() == IDOK) {
		m_markerName = dlg.GetPathName();
		CEdit* pEdit = static_cast<CEdit*> (GetDlgItem(IDC_MARKERLOCNAME));
		pEdit->SetWindowText(m_markerName);
	}



	//LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	//if(pathLen>(-1))
	//{
	//	dlg.m_ofn.lpstrInitialDir = lpszFileName;
	//}

	//if( dlg.DoModal()==IDOK )
	//{
	//	m_markerName = dlg.GetPathName();
	//	CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_MARKERLOCNAME));
	//	pEdit->SetWindowText(m_markerName);
	//}
	//t.ReleaseBuffer();
}
