// dlgWide2Depth.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgWide2Depth.h"
#include "util.h"


// CdlgWide2Depth dialog

IMPLEMENT_DYNAMIC(CdlgWide2Depth, CDialog)

CdlgWide2Depth::CdlgWide2Depth(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgWide2Depth::IDD, pParent)
	
	, m_isRaytrace(FALSE)
	, m_tmin(0)
	, m_tmax(0)
	, m_stackWidth(0)
	, m_stackWidth2(0)
{

	m_dz = 0.0f;
	m_zmax = 0.0f;
	m_zmin = 0.0f;
	m_depthSecName = BLANK;
	m_sphericOrder = 0.0f;
	m_timeSecName = BLANK;
	m_xmax = 0.0f;
	m_xmin = 0.0f;
	m_velSecName = BLANK;

	m_floatName = BLANK;
	m_rayTracing = BLANK;
	m_rayMode = 0; // mode of raytracing, can be P or converted waves
	m_offsetMax = 10.f;
	m_isTwtt = FALSE;

}

CdlgWide2Depth::~CdlgWide2Depth()
{
}

void CdlgWide2Depth::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_VELSECNAME, m_velSecName);
	DDX_Text(pDX, IDC_DEPTHSECNAME, m_depthSecName);
	DDX_Text(pDX, IDC_DLG_RAYTRACING, m_rayTracing);
	DDX_Text(pDX, IDC_FLOATNAME, m_floatName);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMIN, m_xmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMAX, m_xmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMIN, m_tmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMAX, m_tmax);
	DDX_Text(pDX, IDC_CONVERSION_ZMIN, m_zmin);
	DDV_MinMaxFloat(pDX, m_zmin, 0.f, 10000.f);
	DDX_Text(pDX, IDC_CONVERSION_ZMAX, m_zmax);
	DDV_MinMaxFloat(pDX, m_zmax, 1.e-004f, 10000.f);
	DDX_Text(pDX, IDC_CONVERSION_DZ2, m_dz);
	DDV_MinMaxFloat(pDX, m_dz, 1.e-005f, 6.5e-002f);
	DDX_Radio(pDX, IDC_RADIO_MODE, m_rayMode);
	DDX_Text(pDX, IDC_PROC_OFFSETMAX, m_offsetMax);
	DDV_MinMaxFloat(pDX, m_offsetMax, 0.f, 99999.f);
	DDX_Check(pDX, IDC_ISTWTT, m_isTwtt);
	DDX_Text(pDX, IDC_PROC_STACKWIDTH, m_stackWidth);
	DDX_Text(pDX, IDC_PROC_STACKWIDTH2, m_stackWidth2);
	DDX_Check(pDX, IDC_ISRAYTRACE, m_isRaytrace);
	DDX_Text(pDX, IDC_PROC_SPHERORDER, m_sphericOrder);
}


BEGIN_MESSAGE_MAP(CdlgWide2Depth, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSED, OnButtonBrowsed)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, OnButtonBrowsev)
	ON_BN_CLICKED(IDC_BUTTON_BROWSERIN, &CdlgWide2Depth::OnBnClickedButtonBrowserin)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &CdlgWide2Depth::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV2, &CdlgWide2Depth::OnBnClickedButtonBrowsev2)
END_MESSAGE_MAP()


// CdlgWide2Depth message handlers

void CdlgWide2Depth::OnButtonBrowset() 
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Time Section (*.sgy)|*.sgy|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Time Section |sgy |"));
#endif
	if( dlg.DoModal()==IDOK )
	{
		m_timeSecName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_TIMESECNAME));
		pEdit->SetWindowText(m_timeSecName);
	}
	
	
}

void CdlgWide2Depth::OnButtonBrowsed() 
{
#ifndef _MAC
	CFileDialog dlg( FALSE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Depth Section (*.sgy)|*.sgy|"));
#else
	CFileDialog dlg( FALSE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Depth Section |sgy |"));
#endif
	if( dlg.DoModal()==IDOK )
	{
		m_depthSecName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DEPTHSECNAME));
		pEdit->SetWindowText(m_depthSecName);
	}

	
}

void CdlgWide2Depth::OnButtonBrowsev() 
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("bin"),_T("*.bin"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("SeisWide Binary Velocity Format (*.bin)|*.bin|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("SeisWide Binary Velocity Format |bin |"));
#endif
	if( dlg.DoModal()==IDOK )
	{
		m_velSecName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VELSECNAME));
		pEdit->SetWindowText(m_velSecName);
	}

	
}

void CdlgWide2Depth::OnBnClickedButtonBrowserin()
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("in"),_T("*.in"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Zelt's Ray Parameter File (*.in)|*.in|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Zelt's Ray Parameter File |in |"));
#endif


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
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DLG_RAYTRACING));
		pEdit->SetWindowText(m_rayTracing);
	}
	
	t.ReleaseBuffer();
}

void CdlgWide2Depth::OnBnClickedButtonEdit()
{
	UpdateData(TRUE);
	showFileInNotePad(m_rayTracing);
}

void CdlgWide2Depth::OnBnClickedButtonBrowsev2()
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("txt"),_T("*.txt"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("SeisWide Digitize Format (*.txt)|*.txt|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("SeisWide Digitize Format |txt |"));
#endif
	int pathLen = this->m_floatName.ReverseFind( '\\' );
	CString t = m_floatName.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}
	if( dlg.DoModal()==IDOK )
	{
		m_floatName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_FLOATNAME));
		pEdit->SetWindowText(m_floatName);
	}
	t.ReleaseBuffer();
}
