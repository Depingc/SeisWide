// dlgTIMESEC1.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWideDoc.h"
#include "SeisWide.h"
#include "dlgTIMESEC1.h"
#include "dlgSegyColor.h"
#include "dlgSegyColor2.h"
#include "util.h"
#include "SegyWrapper.h"
#include "segyMeta.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgTIMESEC1 property page

IMPLEMENT_DYNCREATE(CdlgTIMESEC1, CPropertyPage)

CdlgTIMESEC1::CdlgTIMESEC1() : CPropertyPage(CdlgTIMESEC1::IDD)
, m_logScaleThreshold(70)
, m_isSwapX(FALSE)
, m_isSwapYLabel(FALSE)
, m_ampPower(0)
{
	//{{AFX_DATA_INIT(CdlgTIMESEC1)
	m_faceDown = -1;
	m_clip = 0.0;
	m_gain = 0.0;
	m_fillOff = 0.0; 
	m_timeSecTitle = BLANK;
	m_wiggle = -1;
	m_timeSecName = BLANK;
	m_offset = -1;

	m_xmax = 0.0;
	m_xmin = 0.0;
	m_xTick = 0.0;

	m_dist1 = 0.0f;
	m_dist2 = 0.0f;
	m_dist3 = 0.0f;
	m_isNegativeFill = FALSE;
	//}}AFX_DATA_INIT

	m_depthRefName = BLANK;  // make it empty so that color selection dialog will not search its directory
}

CdlgTIMESEC1::~CdlgTIMESEC1()
{
}

void CdlgTIMESEC1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgTIMESEC1)
	DDX_Text(pDX, IDC_TIMESEC_TITLE, m_timeSecTitle);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Radio(pDX, IDC_VIEW_OFFSET, m_offset);
	DDX_Text(pDX, IDC_VIEW__DIST1, m_dist1);
	DDX_Text(pDX, IDC_VIEW__DIST2, m_dist2);
	DDX_Text(pDX, IDC_VIEW__DIST3, m_dist3);

	DDX_Text(pDX, IDC_VIEW_UPDATE_XMIN, m_xmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMAX, m_xmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XTICK, m_xTick);
	DDX_Text(pDX, IDC_VIEW_UPDATE_GAIN, m_gain);
	DDX_Text(pDX, IDC_VIEW_UPDATE_CLIP, m_clip);
	DDX_Text(pDX, IDC_VIEW_UPDATE_FILLOFF, m_fillOff);
	DDV_MinMaxDouble(pDX, m_fillOff, 0., 10.);
	DDX_Radio(pDX, IDC_VIEW_WIGGLE, m_wiggle);
	DDX_Text(pDX, IDC_VIEW_LOGSCALE_THRESHOLD, m_logScaleThreshold);
	DDV_MinMaxInt(pDX, m_logScaleThreshold, 0, 100);

	DDX_Check(pDX, IDC_SEGY_AREAFILL_NEG, m_isNegativeFill);
	//}}AFX_DATA_MAP
	//	DDX_Text(pDX, IDC_PALETTENAME, m_colPalette);
	DDX_Check(pDX, IDC_IS_SWAPX, m_isSwapX);
	DDX_Check(pDX, IDC_IS_SWAPYLABEL, m_isSwapYLabel);
	DDX_Radio(pDX, IDC_VIEW_FACEDOWN, m_faceDown);
	DDX_Text(pDX, IDC_VIEW_AMPPOWER, m_ampPower);
}


BEGIN_MESSAGE_MAP(CdlgTIMESEC1, CPropertyPage)
	//{{AFX_MSG_MAP(CdlgTIMESEC1)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_COLOR, &CdlgTIMESEC1::OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_COLORBAR, &CdlgTIMESEC1::OnBnClickedButtonColorbar)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, &CdlgTIMESEC1::OnBnClickedButtonBrowset)
	ON_BN_CLICKED(IDC_VIEW_OFFSET, &CdlgTIMESEC1::OnBnClickedViewOffset)
	ON_BN_CLICKED(IDC_VIEW_DIST, &CdlgTIMESEC1::OnBnClickedViewDist)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgTIMESEC1 message handlers

BOOL CdlgTIMESEC1::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CWnd* pwnd;
	if(mode == 2 || mode == 4)
	{ //velocity model only.
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_SHOT1); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_SHOT2); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_SHOTSTEP); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_GAIN); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_CLIP); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_FILLOFF); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_WIGGLE); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_RADIO2); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_RADIO3); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_RADIO4); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_RADIO5); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_RADIO6); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_RADIO7); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_FACEDOWN); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_FACEUP); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_SEGY_ORDER); if (pwnd) pwnd->EnableWindow(FALSE);

	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CdlgTIMESEC1::OnBnClickedButtonColor()
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


void CdlgTIMESEC1::OnBnClickedButtonColorbar()
{
	//this->m_colPalette.Trim();
	//if (m_colPalette.IsEmpty()) {
	//		//TCHAR curDir[_MAX_PATH];
	//		//m_colPalette.GetBuffer();
	//		GetCurrentDirectory(_MAX_PATH, m_colPalette.GetBuffer());
	//		m_colPalette.ReleaseBuffer();
	//		m_colPalette.Format(_T("%scolPalette.txt"), m_colPalette);
	//}

	CdlgSegyColor2 dlg;
	// dlg.m_timeSecName = this->m_timeSecName;
	// CString m_colPalette;  // this is designed to have no GUI to control it. It must exist in the same dir as a time section
	//	dlg.m_colPalette = GetPathFromFileSlash(draw.timeSecName) + _T("colPalette.txt");

	CString tStr = GetBaseFileName(m_depthRefName).IsEmpty()  ?  this->m_timeSecName  :  m_depthRefName;
	CString aName = GetPathFromFileSlash(tStr) + _T("colPalette.txt");
	dlg.m_colFileName = aName;
	if(dlg.DoModal() == IDOK) {
		dlg.saveAmpColorToFile(aName);  
	}

	// note: now all the color GUI is in the dialog itself. 
}

void CdlgTIMESEC1::OnBnClickedButtonBrowset()
{
	UpdateData(TRUE);
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Time Section (*.sgy)|*.sgy|"));

	int pathLen = m_timeSecName.ReverseFind( '\\' );
	CString t = m_timeSecName.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_timeSecName = dlg.GetPathName();
		// Apply to synchronize to file
		UpdateData(FALSE);
	}
	t.ReleaseBuffer();
}

void CdlgTIMESEC1::OnBnClickedViewOffset()
{
	UpdateData(TRUE);
	HEAD head;
	if (CSegyMeta::segyHeadInit(m_timeSecName, &head) != 1) return;

	SegyWrapper sw(this->m_timeSecName, true);
	//sw.nxDesired = 3;
	if (sw.openTrace(0, 99999, 3, &head, 1, 0.)) {
		m_dist1 = sw.headRead.trArr[0].getOffset();
		m_dist2 = sw.headRead.trArr[1].getOffset();
		m_dist3 = sw.headRead.trArr[2].getOffset();
		m_offset = 0;
		UpdateData(FALSE);
	}
	CSegyMeta::segyHeadFree(&head);
}

void CdlgTIMESEC1::OnBnClickedViewDist()
{
	UpdateData(TRUE);
	HEAD head;
	if (CSegyMeta::segyHeadInit(m_timeSecName, &head) != 1) return;
	SegyWrapper sw(this->m_timeSecName, true);
	if (sw.openTrace(0, 99999, 3, &head, 1, 0.)) {
		m_dist1 = sw.headRead.trArr[0].getDistance();
		m_dist2 = sw.headRead.trArr[1].getDistance();
		m_dist3 = sw.headRead.trArr[2].getDistance();
		m_offset = 1;
		UpdateData(FALSE);
	}
	CSegyMeta::segyHeadFree(&head);
}
