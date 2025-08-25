// dlgHydrateDisp.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgHydrateDisp.h"
#include "dlgSegyColor.h"
#include "dlgSegyColor2.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgHydrateDisp dialog


CdlgHydrateDisp::CdlgHydrateDisp(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgHydrateDisp::IDD, pParent)

	, m_velName(BLANK)
	, m_digFileName(BLANK)
	, m_markerName(BLANK)
	, m_isNegativeFill(FALSE)
	, m_logScaleThreshold(0)
	, m_velWater(0)
	, m_gundepth(0)
	, m_isAvgReducedNMO(FALSE)
	, m_isSwapX(FALSE)
	, m_isSwapYLabel(FALSE)
	, m_ampPower(0)
	, m_targetDepth(0)
{
	//{{AFX_DATA_INIT(CdlgHydrateDisp)
	m_faceDown = -1;
	m_clip = 0.0;
	m_gain = 0.0;
	m_xmax = 0.0;
	m_xmin = 0.0;
	m_wiggle = -1;
	m_xTick = 0.0;
	//m_segyOrder = FALSE;
	m_timeSecTitle = BLANK;
	m_fillOff = 0.0;
	m_timeSecName = BLANK;
	m_obsDepth = 0.0f;
	// m_gunDepth = 0.011f;
	m_tmax = 0.0f;
	m_tmin = 0.0f;
	m_tTick = 0.0f;
	m_vRed = 0.0f;
	m_viewAsOffset = -1;
	m_travelTime = FALSE;

	m_gainDist = FALSE;
	m_baseGain = 0;
	m_basePower = 0;
	//}}AFX_DATA_INIT
}


void CdlgHydrateDisp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgHydrateDisp)
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_VELNAME, m_velName);
	DDX_Text(pDX, IDC_TIMESEC_TITLE, m_timeSecTitle);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMIN, m_xmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMAX, m_xmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XTICK, m_xTick);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMIN, m_tmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMAX, m_tmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TTICK3, m_tTick);
	DDX_Text(pDX, IDC_VIEW_UPDATE_GAIN, m_gain);
	DDX_Text(pDX, IDC_VIEW_UPDATE_CLIP, m_clip);
	DDX_Text(pDX, IDC_VIEW_UPDATE_FILLOFF, m_fillOff);
	DDV_MinMaxDouble(pDX, m_fillOff, 0., 10.);
	DDX_Text(pDX, IDC_VIEW_VRED, m_vRed);
	DDX_Text(pDX, IDC_VELWATER, m_velWater);
	DDX_Text(pDX, IDC_VIEW_OBSDEP, m_obsDepth);
	DDX_Text(pDX, IDC_GUNDEPTH, m_gundepth);
	DDX_Check(pDX, IDC_AVGMODE, m_isAvgReducedNMO);
	DDX_Radio(pDX, IDC_VIEW_WIGGLE, m_wiggle);
	DDX_Text(pDX, IDC_VIEW_LOGSCALE_THRESHOLD2, m_logScaleThreshold);
	DDV_MinMaxInt(pDX, m_logScaleThreshold, 0, 100);
	DDX_Radio(pDX, IDC_VIEW_OFFSET2, m_viewAsOffset);
	DDX_Check(pDX, IDC_TRAVELTIME3, m_travelTime);
	DDX_Check(pDX, IDC_TIME_GAINDIST, m_gainDist);
	DDX_Text(pDX, IDC_TIME_BASEGAIN, m_baseGain);
	//	DDV_MinMaxDouble(pDX, m_baseGain, 0.001., 99999.);
	DDX_Text(pDX, IDC_TIME_BASEPOWER, m_basePower);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_DLG_DIGNAME, m_digFileName);
	DDX_Text(pDX, IDC_MARKERLOCNAME, m_markerName);

	DDX_Radio(pDX, IDC_VIEW_FACEDOWN, m_faceDown);
	DDX_Check(pDX, IDC_SEGY_AREAFILL_NEG2, m_isNegativeFill);
	DDX_Check(pDX, IDC_IS_SWAPX, m_isSwapX);
	DDX_Check(pDX, IDC_IS_SWAPYLABEL, m_isSwapYLabel);
	DDX_Text(pDX, IDC_VIEW_AMPPOWER2, m_ampPower);
	DDX_Text(pDX, IDC_VIEW_TARGETDEP, m_targetDepth);
}


BEGIN_MESSAGE_MAP(CdlgHydrateDisp, CDialog)
	//{{AFX_MSG_MAP(CdlgHydrateDisp)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET2, OnBnClickedButtonBrowset2)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_COLOR, &CdlgHydrateDisp::OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_COLOR2, &CdlgHydrateDisp::OnBnClickedButtonColor2)
	ON_BN_CLICKED(IDC_RADIO16, &CdlgHydrateDisp::OnBnClickedRadio16)
	ON_BN_CLICKED(IDHELP, &CdlgHydrateDisp::OnBnClickedHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgHydrateDisp message handlers

void CdlgHydrateDisp::OnButtonBrowset() 
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

void CdlgHydrateDisp::OnBnClickedButtonBrowset2()
{
	CFileDialog dlg(TRUE, BLANK, _T("*.vxml;*.bin"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("File name for model (*.vxml)|*.vxml|Older format (*.bin)|*.bin|"));
	if (dlg.DoModal() == IDOK)
	{
		m_velName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VELNAME));
		pEdit->SetWindowText(m_velName);
	}	
}

void CdlgHydrateDisp::OnBnClickedButtonColor()
{
		CdlgSegyColor dlg3;
		
		dlg3.m_maxAmpR = seisBiColor.maxAmpR;
		dlg3.m_maxAmpG = seisBiColor.maxAmpG;
		dlg3.m_maxAmpB = seisBiColor.maxAmpB;
		dlg3.m_negAmpR = seisBiColor.negAmpR;
		dlg3.m_negAmpG = seisBiColor.negAmpG;
		dlg3.m_negAmpB = seisBiColor.negAmpB;

		if(dlg3.DoModal() == IDOK)
		{
			seisBiColor.maxAmpR = dlg3.m_maxAmpR;
			seisBiColor.maxAmpG = dlg3.m_maxAmpG;
			seisBiColor.maxAmpB = dlg3.m_maxAmpB;
			seisBiColor.negAmpR = dlg3.m_negAmpR;
			seisBiColor.negAmpG = dlg3.m_negAmpG;
			seisBiColor.negAmpB = dlg3.m_negAmpB;
		}
}

void CdlgHydrateDisp::OnBnClickedButtonColor2()
{
	//CString colFileName;

	//int pathLen = m_timeSecName.ReverseFind( '\\' );
	//if(pathLen >= 0) {
	//	CString txPath = m_timeSecName.Left(pathLen);
	//	colFileName = txPath + _T("\\colPalette.txt");
	//}


	CdlgSegyColor2 dlg;
	CString aName = GetPathFromFileSlash(this->m_timeSecName) + _T("colPalette.txt");
	dlg.m_colFileName = aName;
	// dlg.m_fileName = colFileName;
	if(dlg.DoModal() == IDOK)
	{
		dlg.saveAmpColorToFile(aName);
	}
}

void CdlgHydrateDisp::OnBnClickedRadio16()
{
	// TODO: Add your control notification handler code here
}


void CdlgHydrateDisp::OnBnClickedHelp()
{
	myShowHelp();
}
