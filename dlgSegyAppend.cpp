// dlgSegyAppend.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgSegyAppend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgSegyAppend dialog


CdlgSegyAppend::CdlgSegyAppend(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgSegyAppend::IDD, pParent)
	, m_traces_insert(0)
{
	//{{AFX_DATA_INIT(CdlgSegyAppend)
	m_2ndName = _T("");
	m_1stName = _T("");
	m_reNumCDP = FALSE;	
	//}}AFX_DATA_INIT
}


void CdlgSegyAppend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgSegyAppend)
	DDX_Text(pDX, IDC_2NDSECNAME, m_2ndName);
	DDX_Text(pDX, IDC_TIMESECNAME, m_1stName);
	DDX_Text(pDX, IDC_TRACES_INS, m_traces_insert);
	DDX_Check(pDX, IDC_CHK_RENUMCDP, m_reNumCDP);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgSegyAppend, CDialog)
	//{{AFX_MSG_MAP(CdlgSegyAppend)
	ON_BN_CLICKED(IDC_BUTTON_BROWSED, OnButtonBrowsed)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgSegyAppend message handlers

void CdlgSegyAppend::OnButtonBrowsed() 
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY,
	                 _T("Second Segy File (*.sgy)|*.sgy|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY,
	                 _T("Second Segy File  |sgy |"));
#endif
	if( dlg.DoModal()==IDOK )
	{
		m_2ndName = dlg.GetPathName();
		UpdateData(FALSE);
	}
	
}

void CdlgSegyAppend::OnButtonBrowset() 
{
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("First Segy File (*.sgy)|*.sgy|"));
	if( dlg.DoModal()==IDOK )
	{
		m_1stName = dlg.GetPathName();
		UpdateData(FALSE);
	}
	
}
