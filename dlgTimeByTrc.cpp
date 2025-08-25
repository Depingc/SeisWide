// dlgTimeByTrc.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgTimeByTrc.h"
#include "segy.h"
#include "bmp24.h"
#include "util.h"
#include "dlgSegyColor.h"
#include "dlgSegyColor2.h"
#include "SegyWrapper.h"
#include "segyMeta.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// #define MIL 1000000.

/////////////////////////////////////////////////////////////////////////////
// CdlgTimeByTrc dialog


CdlgTimeByTrc::CdlgTimeByTrc(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgTimeByTrc::IDD, pParent)
	, m_vred(0)
//	, m_colPalette(BLANK)
	, m_logScaleThreshold(0)
	, m_isSwapX(FALSE)
	, m_ampPower(0)
{
	//{{AFX_DATA_INIT(CdlgTimeByTrc)
	m_faceDown = -1;
	m_clip = 0.0;
	m_gain = 0.0;
	m_shot1 = 0;
	m_shot2 = 0;

	m_wiggle = -1;
	m_timeSecTitle = BLANK;
	m_fillOff = 0.1;
	m_timeSecName = BLANK;
	m_tmax = 0.0f;
	m_tmin = 0.0f;
	m_tTick = 0.0f;
	m_overWrite = FALSE;
	m_tstart = 0;
	//}}AFX_DATA_INIT
}


void CdlgTimeByTrc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgTimeByTrc)
	DDX_Text(pDX, IDC_TIMESEC_TITLE, m_timeSecTitle);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SHOT1, m_shot1);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SHOT2, m_shot2);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMIN, m_tmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMAX, m_tmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TTICK3, m_tTick);
	DDV_MinMaxFloat(pDX, m_tTick, 0.f, 99999.f);
	DDX_Text(pDX, IDC_VIEW_UPDATE_GAIN, m_gain);
	DDX_Text(pDX, IDC_VIEW_UPDATE_CLIP, m_clip);
	DDX_Text(pDX, IDC_VIEW_UPDATE_FILLOFF, m_fillOff);
	DDV_MinMaxDouble(pDX, m_fillOff, 0., 10.);
	DDX_Radio(pDX, IDC_VIEW_WIGGLE, m_wiggle);
	DDX_Text(pDX, IDC_VIEW_LOGSCALE_THRESHOLD, m_logScaleThreshold);
	DDX_Radio(pDX, IDC_VIEW_FACEDOWN, m_faceDown);
	DDX_Check(pDX, IDC_CHECK1, m_overWrite);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TSTART, m_tstart);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_VIEW_UPDATE_VRED3, m_vred);
	// DDX_Text(pDX, IDC_PALETTENAME, m_colPalette);
	DDV_MinMaxInt(pDX, m_logScaleThreshold, 0, 100);
	DDX_Check(pDX, IDC_IS_SWAPX, m_isSwapX);
	DDX_Text(pDX, IDC_VIEW_AMPPOWER, m_ampPower);
}


BEGIN_MESSAGE_MAP(CdlgTimeByTrc, CDialog)
	//{{AFX_MSG_MAP(CdlgTimeByTrc)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_AUTO, OnButtonAuto)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_COLOR, &CdlgTimeByTrc::OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_COLORBAR, &CdlgTimeByTrc::OnBnClickedButtonColorbar)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgTimeByTrc message handlers

void CdlgTimeByTrc::OnButtonBrowset() 
{
	CFileDialog dlg(TRUE, _T("sgy; segy"), _T("*.sgy; *.segy"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Segy Time Section (*.sgy;*.segy)|*.sgy; *.segy|"));
	if( dlg.DoModal()==IDOK )
	{
		m_timeSecName = dlg.GetPathName();
//		FormatSegy(m_timeSecName);
		if (CalcSegyInfo()) UpdateData(FALSE);
	}
	
}

void CdlgTimeByTrc::OnButtonAuto() 
{
	if (CalcSegyInfo()) UpdateData(FALSE);
}

BOOL CdlgTimeByTrc::CalcSegyInfo()
{
	// m_tmin
	// m_tmax
	// m_tTick
	// m_shot1
	// m_shot2
	HEAD head;
	if (CSegyMeta::segyHeadInit(m_timeSecName, &head) != 1) return FALSE;

	//SegyWrapper sw(m_timeSecName, true); 
	//bool isTrue = sw.openTrace(0, 999999, 1, &head, 1, 0.);
	//if (isTrue) {
	m_tmin = (float)head.tminFile;
	m_tmax = (float)head.tmaxFile;
	m_tTick = (float)(m_tmax-m_tmin);
	if (m_tTick > 1.f) {
		m_tTick = (float)( (int)(m_tTick*0.1) );
		if(m_tTick < 1.f) m_tTick = 1.f;
	} else  {
		m_tTick = (float)myFloat(m_tTick, 3);
		m_tTick = m_tTick*0.1f;
	}
	m_shot1 = head.traces[0];
	m_shot2 = head.traces[head.numTraces-1];
	// }

	CSegyMeta::segyHeadFree(&head);

	return TRUE;
}

void CdlgTimeByTrc::OnBnClickedButtonColor()
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

void CdlgTimeByTrc::OnBnClickedButtonColorbar()
{
	//this->m_colPalette.Trim();
	//if (m_colPalette.IsEmpty()) {
	//		//TCHAR curDir[_MAX_PATH];
	//		//GetCurrentDirectory(_MAX_PATH, curDir);
	//		//m_colPalette.Format(_T("%scolPalette.txt"), curDir);
	//		GetCurrentDirectory(_MAX_PATH, m_colPalette.GetBuffer());
	//		m_colPalette.ReleaseBuffer();
	//		m_colPalette.Format(_T("%scolPalette.txt"), m_colPalette);
	//}

	CdlgSegyColor2 dlg;
	CString aName = GetPathFromFileSlash(this->m_timeSecName) + _T("colPalette.txt");
	dlg.m_colFileName = aName;
	//dlg.m_fileName = m_colPalette;
	INT_PTR ii = dlg.DoModal();
	if(ii == IDOK)
	{
		dlg.saveAmpColorToFile(aName);
		//m_colPalette = dlg.m_fileName;
	}
}
