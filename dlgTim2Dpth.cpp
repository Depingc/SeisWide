// dlgTim2Dpth.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgTim2Dpth.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgTim2Dpth dialog


CdlgTim2Dpth::CdlgTim2Dpth(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgTim2Dpth::IDD, pParent)
	, m_distShift(0)
	, m_modelsEx(_T(""))
{
	//{{AFX_DATA_INIT(CdlgTim2Dpth)
	m_dz = 0.0f;
	m_zmax = 0.0f;
	m_zmin = 0.0f;
	m_depthSecName = _T("");
	m_sphericOrder = 0.0f;
	m_timeSecName = _T("");
	m_xmax = 0.0f;
	m_xmin = 0.0f;
	m_velSecName = _T("");
	//}}AFX_DATA_INIT
}


void CdlgTim2Dpth::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgTim2Dpth)
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_VELSECNAME, m_velSecName);
	DDX_Text(pDX, IDC_DEPTHSECNAME, m_depthSecName);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMIN, m_xmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMAX, m_xmax);
	DDX_Text(pDX, IDC_DIST_SHIFT, m_distShift);
	DDX_Text(pDX, IDC_CONVERSION_ZMIN, m_zmin);
	DDV_MinMaxFloat(pDX, m_zmin, 0.f, 10000.f);
	DDX_Text(pDX, IDC_CONVERSION_ZMAX, m_zmax);
	DDV_MinMaxFloat(pDX, m_zmax, 1.e-004f, 10000.f);
	DDX_Text(pDX, IDC_CONVERSION_DZ2, m_dz);
	DDV_MinMaxFloat(pDX, m_dz, 1.e-005f, 6.5e-002f);
	DDX_Text(pDX, IDC_PROC_SPHERORDER, m_sphericOrder);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_VELMODEX, m_modelsEx);
}


BEGIN_MESSAGE_MAP(CdlgTim2Dpth, CDialog)
	//{{AFX_MSG_MAP(CdlgTim2Dpth)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSED, OnButtonBrowsed)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, OnButtonBrowsev)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgTim2Dpth message handlers

void CdlgTim2Dpth::OnButtonBrowset() 
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

void CdlgTim2Dpth::OnButtonBrowsed() 
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

void CdlgTim2Dpth::OnButtonBrowsev() 
{
	CFileDialog dlg(TRUE, _T(""), _T("*.vxml;*.bin"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("File name for model (*.vxml)|*.vxml|Older format (*.bin)|*.bin|"));
	//CFileDialog dlg(TRUE, _T("bin"), _T("*.bin"),
	//                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	//                 _T("SeisWide Binary Velocity Format (*.bin)|*.bin|"));
	if( dlg.DoModal()==IDOK )
	{
		m_velSecName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VELSECNAME));
		pEdit->SetWindowText(m_velSecName);
	}

	
}

