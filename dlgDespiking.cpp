// dlgDespiking.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgDespiking.h"


// CdlgDespiking dialog

IMPLEMENT_DYNAMIC(CdlgDespiking, CDialog)

CdlgDespiking::CdlgDespiking(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgDespiking::IDD, pParent)
	, m_inFile(_T(""))
	, m_outFile(_T(""))
	, m_tlength(0)
	, m_gw(0)
	, m_trace1(0)
	, m_trace2(0)
	, m_isSpikeZeroing(FALSE)
	, m_dac(0)
	, m_isLTZ(FALSE)
	, m_tlag(0)
{

}

CdlgDespiking::~CdlgDespiking()
{
}

void CdlgDespiking::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIMESECNAME, m_inFile);
	DDX_Text(pDX, IDC_2NDSECNAME, m_outFile);
	DDX_Check(pDX, IDC_IS_SPIKE_ZEROING, m_isSpikeZeroing);
	DDX_Text(pDX, IDC_EDIT_DAC, m_dac);
	DDX_Text(pDX, IDC_EDIT_TLAG, m_tlag);
	DDX_Check(pDX, IDC_IS_SPIKE_ZEROING2, m_isLTZ);
	DDX_Text(pDX, IDC_EDIT_TLENGTH, m_tlength);
	DDX_Text(pDX, IDC_EDIT_GW, m_gw);
	DDX_Text(pDX, IDC_EDIT_TRACE1, m_trace1);
	DDX_Text(pDX, IDC_EDIT_TRACE2, m_trace2);
}


BEGIN_MESSAGE_MAP(CdlgDespiking, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, &CdlgDespiking::OnBnClickedButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSED, &CdlgDespiking::OnBnClickedButtonBrowsed)
END_MESSAGE_MAP()


// CdlgDespiking message handlers

void CdlgDespiking::OnBnClickedButtonBrowset()
{
	// browse for time section
	CFileDialog dlg( TRUE,_T("sgy; SEG"),_T("*.sgy; *.SEG"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Input Segy File (*.sgy;*.SEG)|*.sgy; *.SEG|"));
	
	int pathLen = m_inFile.ReverseFind( '\\' );
	CString t = this->m_inFile.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_inFile = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_TIMESECNAME));
		pEdit->SetWindowText(m_inFile);
	}
	t.ReleaseBuffer();
	
}

void CdlgDespiking::OnBnClickedButtonBrowsed()
{
	// browse for output file
	CFileDialog dlg( TRUE,_T("sgy; SEG"),_T("*.sgy; *.SEG"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Input Segy File (*.sgy;*.SEG)|*.sgy; *.SEG|"));
	
	int pathLen = m_outFile.ReverseFind( '\\' );
	CString t = this->m_outFile.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_outFile = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_2NDSECNAME));
		pEdit->SetWindowText(m_outFile);
	}
	t.ReleaseBuffer();
}
