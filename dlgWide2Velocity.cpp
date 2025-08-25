// dlgWide2Velocity.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgWide2Velocity.h"
#include "util.h"

// CdlgWide2Velocity dialog

IMPLEMENT_DYNAMIC(CdlgWide2Velocity, CDialog)

CdlgWide2Velocity::CdlgWide2Velocity(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgWide2Velocity::IDD, pParent)
	, m_rayTracing(BLANK)
	, m_velname(BLANK)
	, m_depthSecName(BLANK)
	, m_zmin(0)
	, m_zmax(0)
	, m_dz(0)
	, m_vPerturbMin(0)
	, m_vPerturbMax(0)
	, m_dv(0)
	, m_iLayerFocus(0)
	, m_secsmooth(0.06f)
	, m_iscoh(FALSE)
	, m_numDisplay(0)
	, m_basePower(0)
	, m_rayMode(0)
	, m_sumMethod(0)
	, m_floatName(BLANK)
	, m_xpercent(0)
	, m_isTwtt(FALSE)
	, m_offsetMax(0)
	, m_tmin(0)
	, m_tmax(0)
{

}

CdlgWide2Velocity::~CdlgWide2Velocity()
{
}

void CdlgWide2Velocity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_VELSECNAME, m_velname);
	DDX_Text(pDX, IDC_DEPTHSECNAME, m_depthSecName);
	DDX_Text(pDX, IDC_DLG_RAYTRACING, m_rayTracing);
	DDX_Text(pDX, IDC_FLOATNAME, m_floatName);
	DDX_Text(pDX, IDC_PROC_SPHERORDER, m_basePower);
	DDX_Text(pDX, IDC_PROC_OFFSETMAX, m_offsetMax);
	DDV_MinMaxFloat(pDX, m_offsetMax, 0.f, 99999.f);
	DDX_Text(pDX, IDC_CONVERSION_ZMIN, m_zmin);
	DDX_Text(pDX, IDC_CONVERSION_ZMAX, m_zmax);
	DDX_Text(pDX, IDC_CONVERSION_DZ, m_dz);
	DDX_Text(pDX, IDC_CONVERSION_VMIN, m_vPerturbMin);
	DDV_MinMaxFloat(pDX, m_vPerturbMin, 0.00001f, 9.f);
	DDX_Text(pDX, IDC_CONVERSION_VMAX, m_vPerturbMax);
	DDV_MinMaxFloat(pDX, m_vPerturbMax, 0.001f, 9.f);
	DDX_Text(pDX, IDC_CONVERSION_DV, m_dv);
	DDV_MinMaxFloat(pDX, m_dv, 0.000001f, 8.f);
	DDX_Text(pDX, IDC_EDIT_LAYERNO, m_iLayerFocus);
	DDV_MinMaxInt(pDX, m_iLayerFocus, 1, 99999);
	DDX_Radio(pDX, IDC_RADIO_MODE, m_rayMode);
	DDX_Check(pDX, IDC_ISCOH, m_iscoh);
	DDX_Text(pDX, IDC_EDIT_NUMDISPLAY, m_numDisplay);
	DDX_Check(pDX, IDC_ISTWTT, m_isTwtt);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMIN2, m_tmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMAX2, m_tmax);
	DDX_Text(pDX, IDC_PROC_TWINDOWSEC, m_secsmooth);

	DDX_Radio(pDX, IDC_RADIO_SUMMETHOD, m_sumMethod);
	DDX_Text(pDX, IDC_PROC_XPERCENT, m_xpercent);
	DDV_MinMaxInt(pDX, m_xpercent, 1, 100);


}


BEGIN_MESSAGE_MAP(CdlgWide2Velocity, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, &CdlgWide2Velocity::OnBnClickedButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSED, &CdlgWide2Velocity::OnBnClickedButtonBrowsed)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, &CdlgWide2Velocity::OnBnClickedButtonBrowsev)
	ON_BN_CLICKED(IDC_BUTTON_BROWSERIN, &CdlgWide2Velocity::OnBnClickedButtonBrowserin)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &CdlgWide2Velocity::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV2, &CdlgWide2Velocity::OnBnClickedButtonBrowsev2)
	ON_BN_CLICKED(IDC_RADIO2, &CdlgWide2Velocity::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CdlgWide2Velocity::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO_MODE, &CdlgWide2Velocity::OnBnClickedRadioMode)
END_MESSAGE_MAP()


// CdlgWide2Velocity message handlers

void CdlgWide2Velocity::OnBnClickedButtonBrowset()
{
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Time Section (*.sgy)|*.sgy|"));

	if( dlg.DoModal()==IDOK )
	{
		m_timeSecName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_TIMESECNAME));
		pEdit->SetWindowText(m_timeSecName);
	}

}

void CdlgWide2Velocity::OnBnClickedButtonBrowsed()
{
	CFileDialog dlg( FALSE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Mixed Segy Section (*.sgy)|*.sgy|"));
	if( dlg.DoModal()==IDOK )
	{
		m_depthSecName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DEPTHSECNAME));
		pEdit->SetWindowText(m_depthSecName);
	}
}

void CdlgWide2Velocity::OnBnClickedButtonBrowsev()
{
	CFileDialog dlg( TRUE,_T("bin"),_T("*.bin"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("SeisWide Binary Velocity Format (*.bin)|*.bin|"));

	int pathLen = m_velname.ReverseFind( '\\' );
	CString t = m_velname.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_velname = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VELSECNAME));
		pEdit->SetWindowText(m_velname);
	}
	t.ReleaseBuffer();}

void CdlgWide2Velocity::OnBnClickedButtonBrowserin()
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

void CdlgWide2Velocity::OnBnClickedButtonEdit()
{
	UpdateData(TRUE);
	showFileInNotePad(m_rayTracing);
}

void CdlgWide2Velocity::OnBnClickedButtonBrowsev2()
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


void CdlgWide2Velocity::OnBnClickedRadio2()
{
	// click this will show up a few controls to assign offset info.
	CWnd* pwnd;

	pwnd = GetDlgItem(IDC_STATIC_VEL); 
	if (pwnd) {
		pwnd->SetWindowText(_T("Vs/Vp ratios: min, max, dv (default: 0.1, 0.7, 0.1):"));
		m_vPerturbMin = 0.1f;
		m_vPerturbMax = 0.7f;
		m_dv = 0.05f;
		m_rayMode = 1;
		UpdateData(FALSE);
	}
	
}

void CdlgWide2Velocity::OnBnClickedRadio3()
{
	CWnd* pwnd;

	pwnd = GetDlgItem(IDC_STATIC_VEL); 
	if (pwnd) {
		pwnd->SetWindowText(_T("Vs/Vp ratios: min, max, dv (default: 0.1, 0.7, 0.1):"));
		m_vPerturbMin = 0.1f;
		m_vPerturbMax = 0.7f;
		m_dv = 0.05f;
		m_rayMode = 2;
		UpdateData(FALSE);
	}
}

void CdlgWide2Velocity::OnBnClickedRadioMode()
{
	// click this will show up a few controls to assign offset info.
	CWnd* pwnd;
	pwnd = GetDlgItem(IDC_STATIC_VEL); 
	if (pwnd) {
		pwnd->SetWindowText(_T("Vmin, Vmax, dv (Min, max, and interval for velocity perturbation, in km/s:"));  // default
		m_vPerturbMin = 1.7f;
		m_vPerturbMax = 2.5f;
		m_dv = 0.04f;
		m_rayMode = 0;
		UpdateData(FALSE);
	}
}
