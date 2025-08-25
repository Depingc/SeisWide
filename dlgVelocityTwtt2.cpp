// dlgVelocityTwtt2.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgVelocityTwtt2.h"
#include "dlgSegyColor2.h"
#include "util.h"
//#include "afxdialogex.h"


// dlgVelocityTwtt2 dialog

IMPLEMENT_DYNAMIC(dlgVelocityTwtt2, CDialog)

dlgVelocityTwtt2::dlgVelocityTwtt2(CWnd* pParent /*=NULL*/)
	: CDialog(dlgVelocityTwtt2::IDD, pParent)
	, m_plotTitle(BLANK)
	, m_timeSecName(BLANK)
	, m_digName(BLANK)
	, m_markerName(BLANK)
	, m_xMultFactor(1)
	, m_xShift(0)
	, m_yMultFactor(1)
	, m_yShift(0)
	, m_vrmsTauName(BLANK)
	, m_xmin(0)
	, m_xmax(0)
	, m_xTick(0)
	, m_tmin(0)
	, m_tmax(0)
	, m_tTick(0)
	, m_wiggle(2)
	, m_velSecName(BLANK)
//	, m_colPalette(BLANK)
	, m_modelEx(BLANK)
	, m_xWell(0)
	, m_velModExtras(BLANK)
	, m_distShift(0)
	, m_markerRevYAxis(FALSE)
	, m_logScaleThreshold(70)
	, m_isSwapX(FALSE)
	, m_isSwapYLabel(FALSE)
	, m_isNegativeFill(FALSE)
	, m_ampPower(0)
	, m_travelTime(FALSE)
{
	m_gain = 0.0;
	m_clip = 0.0;
	m_fillOff = 0.0;

}

dlgVelocityTwtt2::~dlgVelocityTwtt2()
{
}

void dlgVelocityTwtt2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIMESEC_TITLE, m_plotTitle);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_DLG_DIGNAME, m_digName);
	DDX_Text(pDX, IDC_MARKERLOCNAME, m_markerName);
	DDX_Text(pDX, IDC_XMULT, m_xMultFactor);
	DDX_Text(pDX, IDC_XSHIFT, m_xShift);
	DDX_Text(pDX, IDC_YMULT, m_yMultFactor);
	DDX_Text(pDX, IDC_YSHIFT, m_yShift);
	DDX_Text(pDX, IDC_VRMSTAUNAME, m_vrmsTauName);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMIN, m_xmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMAX, m_xmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XTICK, m_xTick);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMIN, m_tmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMAX, m_tmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TTICK3, m_tTick);
	DDX_Radio(pDX, IDC_VIEW_WIGGLE, m_wiggle);
	DDX_Text(pDX, IDC_VELSECNAME, m_velSecName);
	DDX_Text(pDX, IDC_XSHOT2, m_xWell);

	DDX_Text(pDX, IDC_VIEW_UPDATE_GAIN, m_gain);
	DDX_Text(pDX, IDC_VIEW_UPDATE_CLIP2, m_clip);
	DDX_Text(pDX, IDC_VIEW_UPDATE_FILLOFF, m_fillOff);
	DDV_MinMaxDouble(pDX, m_fillOff, 0., 10.);

	DDX_Text(pDX, IDC_EDIT_VELMODEX, m_velModExtras);
	DDX_Text(pDX, IDC_DIST_SHIFT, m_distShift);
	DDX_Check(pDX, IDC_REV_POLARITY, m_markerRevYAxis);
	DDX_Text(pDX, IDC_VIEW_LOGSCALE_THRESHOLD, m_logScaleThreshold);
	DDV_MinMaxInt(pDX, m_logScaleThreshold, 0, 100);
	DDX_Check(pDX, IDC_IS_SWAPX, m_isSwapX);
	DDX_Check(pDX, IDC_IS_SWAPYLABEL, m_isSwapYLabel);
	DDX_Check(pDX, IDC_SEGY_AREAFILL_NEG, m_isNegativeFill);
	DDX_Text(pDX, IDC_TIME_BASEPOWER, m_ampPower);
	DDX_Check(pDX, IDC_TRAVELTIME2, m_travelTime);
}


BEGIN_MESSAGE_MAP(dlgVelocityTwtt2, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, &dlgVelocityTwtt2::OnBnClickedButtonBrowsev)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, &dlgVelocityTwtt2::OnBnClickedButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEDIG, &dlgVelocityTwtt2::OnBnClickedButtonBrowsedig)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEVRMS, &dlgVelocityTwtt2::OnBnClickedButtonBrowsevrms)
	ON_BN_CLICKED(IDC_BUTTON_COLORBAR, &dlgVelocityTwtt2::OnBnClickedButtonColorbar)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV3, &dlgVelocityTwtt2::OnBnClickedButtonBrowsev3)
END_MESSAGE_MAP()


// dlgVelocityTwtt2 message handlers


void dlgVelocityTwtt2::OnBnClickedButtonBrowsev()
{
	//CFileDialog dlg( TRUE,_T("bin"),_T("*.bin"),
	//                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	//                 _T("Velocity Model File (*.bin)|*.bin|"));
	CFileDialog dlg(TRUE, m_velSecName, _T("*.vxml;*.bin"),
		OFN_HIDEREADONLY|OFN_EXPLORER,
		_T("Velocity Model File (*.vxml)|*.vxml|Older format (*.bin)|*.bin|"));

	if( dlg.DoModal()==IDOK )
	{
		this->m_velSecName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VELSECNAME));
		pEdit->SetWindowText(m_velSecName);
	}
}


void dlgVelocityTwtt2::OnBnClickedButtonBrowset()
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


void dlgVelocityTwtt2::OnBnClickedButtonBrowsedig()
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


void dlgVelocityTwtt2::OnBnClickedButtonBrowsevrms()
{
	CFileDialog dlg( TRUE,_T("txt"),_T("*.txt"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Vrms-Tau file; Digitize Format (*.txt)|*.txt|"));

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

void dlgVelocityTwtt2::OnBnClickedButtonColorbar()
{
	//this->m_colPalette.Trim();
	//if (m_colPalette.IsEmpty()) {
	//		//TCHAR curDir[_MAX_PATH];
	//		//GetCurrentDirectory(_MAX_PATH, curDir);
	//		GetCurrentDirectory(_MAX_PATH, m_colPalette.GetBuffer());
	//		// getPathFrom(this->m_timeSecName) + _T("colPalette.txt");
	//		m_colPalette.ReleaseBuffer();
	//		m_colPalette.Format(_T("%scolPalette.txt"), m_colPalette);
	//}
	//
	CdlgSegyColor2 dlg;
	CString aName = GetPathFromFileSlash(this->m_timeSecName) + _T("colPalette.txt");
	dlg.m_colFileName = aName;
//	dlg.m_fileName = m_colPalette;
	if(dlg.DoModal() == IDOK)
	{
		dlg.saveAmpColorToFile(aName);
	//	m_colPalette = dlg.m_fileName;
	}
}

void dlgVelocityTwtt2::OnBnClickedButtonBrowsev3()
{
	// user clicked the marker browser button
	CFileDialog dlg( TRUE,_T("txt"),_T("*.txt"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("X-Y Location File (free text format, 2 columns) (*.txt)|*.txt|"));


	int pathLen = m_markerName.ReverseFind( '\\' );
	CString t = m_markerName.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_markerName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_MARKERLOCNAME));
		pEdit->SetWindowText(m_markerName);
	}
	t.ReleaseBuffer();
}
