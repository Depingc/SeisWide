// DlgVelocityTwtt.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "DlgVelocityTwtt.h"


// CDlgVelocityTwtt dialog

IMPLEMENT_DYNAMIC(CDlgVelocityTwtt, CDialog)

CDlgVelocityTwtt::CDlgVelocityTwtt(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVelocityTwtt::IDD, pParent)
	, m_timeSecName(_T(""))
	, m_digName(_T(""))
	, m_vrmsTauName(_T(""))
	, m_wiggle(0)
	, m_isAveEnergy(FALSE)
{

}

CDlgVelocityTwtt::~CDlgVelocityTwtt()
{
}

void CDlgVelocityTwtt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIMESEC_TITLE, m_timeSecTitle);
	DDX_Text(pDX, IDC_VELSECNAME, m_velSecName);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMAX, m_xmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMIN, m_xmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XTICK, m_xTick);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMAX, m_tmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMIN, m_tmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TTICK3, m_tTick);
	DDV_MinMaxFloat(pDX, m_tTick, 0.f, 99999.f);
	// DDX_Text(pDX, IDC_VIEW_OBSDEP, m_zSource);
	DDV_MinMaxFloat(pDX, m_zSource, 0.f, 99999.f);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_DLG_DIGNAME, m_digName);
	DDX_Text(pDX, IDC_VRMSTAUNAME, m_vrmsTauName);
	DDX_Radio(pDX, IDC_VIEW_WIGGLE, m_wiggle);
	DDX_Check(pDX, IDC_AVGAMP, m_isAveEnergy);
}


BEGIN_MESSAGE_MAP(CDlgVelocityTwtt, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, &CDlgVelocityTwtt::OnBnClickedButtonBrowsev)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, &CDlgVelocityTwtt::OnBnClickedButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEDIG, &CDlgVelocityTwtt::OnBnClickedButtonBrowsedig)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEVRMS, &CDlgVelocityTwtt::OnBnClickedButtonBrowsevrms)
END_MESSAGE_MAP()


// CDlgVelocityTwtt message handlers

void CDlgVelocityTwtt::OnBnClickedButtonBrowsev()
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("bin"),_T("*.bin"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Velocity Model File (*.bin)|*.bin|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("X-Y Location File (free text format, 2 columns) |txt |"));
#endif


	if( dlg.DoModal()==IDOK )
	{
		this->m_velSecName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VELSECNAME));
		pEdit->SetWindowText(m_velSecName);
	}
}

void CDlgVelocityTwtt::OnBnClickedButtonBrowset()
{
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Vertical incidence time section (*.sgy)|*.sgy|"));

	if( dlg.DoModal()==IDOK )
	{
		this->m_timeSecName= dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_TIMESECNAME));
		pEdit->SetWindowText(m_timeSecName);
	}
}

void CDlgVelocityTwtt::OnBnClickedButtonBrowsedig()
{
	CFileDialog dlg( TRUE,_T("txt"),_T("*.txt"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Line drawings, Distance-TWTT pair per line (*.txt)|*.txt|"));

	if( dlg.DoModal()==IDOK )
	{
		this->m_digName= dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DLG_DIGNAME));
		pEdit->SetWindowText(m_digName);
	}
}


void CDlgVelocityTwtt::OnBnClickedButtonBrowsevrms()
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("txt"),_T("*.txt"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Vrms-Tau file; Digitize Format (*.txt)|*.txt|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Vrms-Tau file; Digitize Format |txt |"));
#endif


	int pathLen = this->m_vrmsTauName.ReverseFind( '\\' );
	CString t = m_vrmsTauName.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_vrmsTauName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VRMSTAUNAME));
		pEdit->SetWindowText(m_vrmsTauName);
	}
	t.ReleaseBuffer();	
}
