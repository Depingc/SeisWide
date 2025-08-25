// dlgCohMix.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgCohMix.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgCohMix dialog


CdlgCohMix::CdlgCohMix(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgCohMix::IDD, pParent)
	, m_outputMethod(2)
	, m_xmin(0)
	, m_xmax(0)
	, m_numDivision(0)
	, m_f1(0)
	, m_f2(0)
	, m_dxMix2(0)
	, m_nBigLoop(0)
	, m_nSmooth(0)
{
	//{{AFX_DATA_INIT(CdlgCohMix)
	m_procTmax = 0.0f;
	m_procTmin = 0.0f;
	m_procTWindow = 0.0f;
	m_procVelmax = 0.0f;
	m_procVelmin = 0.0f;
	m_dxMix1 = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgCohMix::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgCohMix)
	DDX_Text(pDX, IDC_PROCNAME_IN, m_procNameIn);
	DDX_Text(pDX, IDC_PROCNAME_OUT, m_procNameOut);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMIN, m_xmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMAX, m_xmax);
	DDX_Text(pDX, IDC_PROC_TMAX, m_procTmax);
	DDX_Text(pDX, IDC_PROC_TMIN, m_procTmin);
	DDX_Text(pDX, IDC_PROC_DXMIX, m_dxMix1);
	DDV_MinMaxFloat(pDX, m_dxMix1, -1000.f, 1000.f);
	DDX_Text(pDX, IDC_PROC_DXMIX2, m_dxMix2);
	DDV_MinMaxFloat(pDX, m_dxMix2, -1000.f, 1000.f);
	DDX_Text(pDX, IDC_PROC_VELMIN, m_procVelmin);
	DDV_MinMaxFloat(pDX, m_procVelmin, -99999.f, 99999.f);
	DDX_Text(pDX, IDC_PROC_VELMAX, m_procVelmax);
	DDV_MinMaxFloat(pDX, m_procVelmax, -99999.f, 99999.f);
	DDX_Text(pDX, IDC_PROC_TWINDOW, m_procTWindow);
	DDV_MinMaxFloat(pDX, m_procTWindow, 0.f, 1.f);
	DDX_Text(pDX, IDC_PROC_NSMOOTH, m_nSmooth);
	//}}AFX_DATA_MAP
	DDX_Radio(pDX, IDC_PROC_METHOD, m_outputMethod);
	DDX_Text(pDX, IDC_PROC_FILTERF1, m_f1);
	DDX_Text(pDX, IDC_PROC_FILTERF2, m_f2);
	DDX_Text(pDX, IDC_PROC_NUMDIV, m_numDivision);
	DDV_MinMaxInt(pDX, m_numDivision, 0, 999);
	DDX_Text(pDX, IDC_PROC_NUMLOOP, m_nBigLoop);
	DDV_MinMaxInt(pDX, m_nBigLoop, 1, 99);
}


BEGIN_MESSAGE_MAP(CdlgCohMix, CDialog)
	//{{AFX_MSG_MAP(CdlgCohMix)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSETCOH, OnButtonBrowsetcoh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgCohMix message handlers

void CdlgCohMix::OnButtonBrowset() 
{
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Time Section (*.sgy)|*.sgy|"));
	if( dlg.DoModal()==IDOK )
	{
		m_procNameIn = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_PROCNAME_IN));
		pEdit->SetWindowText(m_procNameIn);
	}

	
}

void CdlgCohMix::OnButtonBrowsetcoh() 
{
	CFileDialog dlg( FALSE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Mixed Segy Section (*.sgy)|*.sgy|"));
	if( dlg.DoModal()==IDOK )
	{
		m_procNameOut = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_PROCNAME_OUT));
		pEdit->SetWindowText(m_procNameOut);
	}

	
}
