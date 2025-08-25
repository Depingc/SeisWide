// dlgInOut.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgInOut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgInOut dialog


CdlgInOut::CdlgInOut(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgInOut::IDD, pParent)
	, m_dac(0)
{
	//{{AFX_DATA_INIT(CdlgInOut)
	m_inFile = _T("");
	m_outFile = _T("");
	m_trace1 = 0;
	m_trace2 = 0;
	//}}AFX_DATA_INIT
}


void CdlgInOut::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgInOut)
	DDX_Text(pDX, IDC_TIMESECNAME, m_inFile);
	DDX_Text(pDX, IDC_2NDSECNAME, m_outFile);
	DDX_Text(pDX, IDC_EDIT_TRACE1, m_trace1);
	DDX_Text(pDX, IDC_EDIT_TRACE2, m_trace2);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_DAC, m_dac);
}


BEGIN_MESSAGE_MAP(CdlgInOut, CDialog)
	//{{AFX_MSG_MAP(CdlgInOut)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSED, OnButtonBrowsed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgInOut message handlers

void CdlgInOut::OnButtonBrowset() 
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("sgy; SEG"),_T("*.sgy; *.SEG"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Input Segy File (*.sgy;*.SEG)|*.sgy; *.SEG|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Input Segy File |sgy |"));
#endif
	
	int pathLen = m_inFile.ReverseFind( '\\' );
	CString t = m_inFile.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_inFile = dlg.GetPathName();
		UpdateData(FALSE);
	}
	t.ReleaseBuffer();
	
}

void CdlgInOut::OnButtonBrowsed() 
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
	
	int pathLen = m_outFile.ReverseFind( '\\' );
	CString t = m_outFile.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_outFile = dlg.GetPathName();
		UpdateData(FALSE);
	}
	t.ReleaseBuffer();
}

