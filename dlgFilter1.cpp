// dlgFilter1.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgFilter1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgFilter1 dialog


CdlgFilter1::CdlgFilter1(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgFilter1::IDD, pParent)
	, m_isHilbert(FALSE)
	, m_numTFilter(2)
	//, m_obsDepth(0)
	//, m_velWater(0)
	, m_ta(0)
	, m_tb(0)
	, m_f1b(0)
	, m_f2b(0)
	, m_zerophase(FALSE)
	, m_tmin(0)
	, m_tmax(0)
	, m_basePower(0)
	, m_isTranspose(FALSE)
	, m_isMixAmpPhase(FALSE)
	, m_isIEEE_Floats(FALSE)
	, m_isLimitTime(FALSE)
{
	//{{AFX_DATA_INIT(CdlgFilter1)
	m_filter = FALSE;
	m_f1 = 0.0f;
	m_f2 = 0.0f;
	m_nameIn = _T("");
	m_nameOut = _T("");
	m_norm = FALSE;
	m_normValue = 0.0f;
	m_multiply = FALSE;
	m_multiplyValue = 0.0f;
	m_filterF = FALSE;
	m_ff0 = 0.0f;
	m_ff1 = 0.0f;
	m_ff2 = 0.0f;
	m_ff3 = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgFilter1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgFilter1)
	DDX_Check(pDX, IDC_PROC_FILTER, m_filter);
	DDX_Text(pDX, IDC_PROC_FILTERF1, m_f1);
	DDX_Text(pDX, IDC_PROC_FILTERF2, m_f2);
	DDX_Text(pDX, IDC_PROCNAME_IN, m_nameIn);
	DDX_Text(pDX, IDC_PROCNAME_OUT, m_nameOut);

	DDX_Check(pDX, IDC_PROC_NORM, m_norm);
	DDX_Text(pDX, IDC_PROC_NORMVALUE, m_normValue);
	DDV_MinMaxFloat(pDX, m_normValue, 1.e-002f, 1.e+008f);
	DDX_Check(pDX, IDC_PROC_MULTIPLY, m_multiply);
	DDX_Text(pDX, IDC_PROC_MULTIPLYVALUE, m_multiplyValue);
	DDV_MinMaxFloat(pDX, m_multiplyValue, -1.e+007f, 1.e+007f);
	DDX_Check(pDX, IDC_PROC_FILTER2, m_filterF);
	DDX_Check(pDX, IDC_PROC_ZEROPHASE, m_zerophase);
	DDX_Text(pDX, IDC_PROC_FILTERF10, m_ff0);
	DDX_Text(pDX, IDC_PROC_FILTERF11, m_ff1);
	DDX_Text(pDX, IDC_PROC_FILTERF12, m_ff2);
	DDX_Text(pDX, IDC_PROC_FILTERF13, m_ff3);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_PROC_HILBERT, m_isHilbert);
	DDX_Text(pDX, IDC_PROC_NUM_TFILTER, m_numTFilter);
	//DDX_Text(pDX, IDC_VIEW_OBSDEP, m_obsDepth);
	//DDX_Text(pDX, IDC_VIEW_VELWATER, m_velWater);
	DDX_Text(pDX, IDC_PROC_FILTERTA, m_ta);
	DDX_Text(pDX, IDC_PROC_FILTERTB, m_tb);
	DDX_Text(pDX, IDC_PROC_FILTERF1B, m_f1b);
	DDX_Text(pDX, IDC_PROC_FILTERF2B, m_f2b);
	DDX_Text(pDX, IDC_PROC_TMIN, m_tmin);
	DDX_Text(pDX, IDC_PROC_TMAX, m_tmax);
	DDX_Text(pDX, IDC_TIME_BASEPOWER, m_basePower);
	DDV_MinMaxFloat(pDX, m_basePower, -1000.f, 1000.f);
	DDX_Check(pDX, IDC_PROC_HORZ, m_isTranspose);
	DDX_Check(pDX, IDC_PROC_AMP_PHASE, m_isMixAmpPhase);
	DDX_Check(pDX, IDC_PROC_IEEE_FLOAT, m_isIEEE_Floats);
	DDX_Check(pDX, IDC_PROC_FILTER4, m_isLimitTime);
}


BEGIN_MESSAGE_MAP(CdlgFilter1, CDialog)
	//{{AFX_MSG_MAP(CdlgFilter1)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET2, OnButtonBrowset2)
	ON_BN_CLICKED(IDC_PROC_FILTER, OnProcFilter)
	ON_BN_CLICKED(IDC_PROC_FILTER2, OnProcFilter2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgFilter1 message handlers

void CdlgFilter1::OnButtonBrowset() 
{
	CFileDialog dlg( TRUE,_T("sgy; SEG"),_T("*.sgy; *.SEG"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Input Segy File (*.sgy;*.SEG)|*.sgy; *.SEG|"));

	int pathLen = m_nameIn.ReverseFind( '\\' );
	CString t = m_nameIn.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_nameIn = dlg.GetPathName();

		// since the input file is changed, shot1 and shot2 must be different
		//m_shot1 = 0;
		//m_shot2 = 99999;



		// update the output name
		pathLen = m_nameIn.ReverseFind( '.' );
		if(pathLen>(-1))
		{
			CString t = m_nameIn.Left(pathLen);
			m_nameOut = t + _T("f.sgy");
		}



		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_PROCNAME_IN));
		pEdit->SetWindowText(m_nameIn);
		pEdit = static_cast<CEdit*> (GetDlgItem(IDC_PROCNAME_OUT));
		pEdit->SetWindowText(m_nameOut);
	}
	t.ReleaseBuffer();
}

void CdlgFilter1::OnButtonBrowset2() 
{
#ifndef _MAC
	CFileDialog dlg( FALSE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Output Segy File (*.sgy)|*.sgy|"));
#else
	CFileDialog dlg( FALSE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Output Segy File  |sgy |"));
#endif
	if( dlg.DoModal()==IDOK )
	{
		m_nameOut = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_PROCNAME_OUT));
		pEdit->SetWindowText(m_nameOut);
	}
	
	
}

void CdlgFilter1::OnProcFilter() 
{
//	CWnd* pwnd;
//	pwnd = GetDlgItem(IDC_PROC_FILTER2); if (pwnd) pwnd->CheckDlgButton(FALSE);
	CheckDlgButton(IDC_PROC_FILTER2, MF_UNCHECKED);
//	menu->CheckMenuItem( ID_VIEW_SEGYINFO, MF_UNCHECKED);
	
}

void CdlgFilter1::OnProcFilter2() 
{
	CheckDlgButton(IDC_PROC_FILTER, MF_UNCHECKED);
	
}
