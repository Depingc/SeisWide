// dlgReorganize.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgReorganize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgReorganize dialog


CdlgReorganize::CdlgReorganize(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgReorganize::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgReorganize)
	m_procNameIn = _T("");
	m_procNameOut = _T("");
	m_trcReverse = 0;
	//}}AFX_DATA_INIT
}


void CdlgReorganize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgReorganize)
	DDX_Text(pDX, IDC_PROCNAME_IN, m_procNameIn);
	DDX_Text(pDX, IDC_PROCNAME_OUT, m_procNameOut);
	DDX_Radio(pDX, IDC_RADIO_REVERSE, m_trcReverse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgReorganize, CDialog)
	//{{AFX_MSG_MAP(CdlgReorganize)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSETCOH, OnButtonBrowsetcoh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgReorganize message handlers

void CdlgReorganize::OnButtonBrowset() 
{
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Time Section (*.sgy)|*.sgy|"));

	int pathLen = m_procNameIn.ReverseFind( '\\' );
	CString t = m_procNameIn.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_procNameIn = dlg.GetPathName();
		UpdateData(FALSE);
	}
	t.ReleaseBuffer();

}

void CdlgReorganize::OnButtonBrowsetcoh() 
{
#ifndef _MAC
	CFileDialog dlg( FALSE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Output Segy Section (*.sgy)|*.sgy|"));
#else
	CFileDialog dlg( FALSE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Output Segy Section |sgy |"));
#endif


	int pathLen = m_procNameOut.ReverseFind( '\\' );
	CString t = m_procNameOut.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_procNameOut = dlg.GetPathName();
		UpdateData(FALSE);
	}
	t.ReleaseBuffer();

}
