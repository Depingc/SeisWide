// dlgLoadReid.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgLoadReid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgLoadReid dialog


CdlgLoadReid::CdlgLoadReid(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgLoadReid::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgLoadReid)
	m_velName = _T("");
	m_reidName = _T("");
	//}}AFX_DATA_INIT
}


void CdlgLoadReid::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgLoadReid)
	DDX_Text(pDX, IDC_DLG_VELNAME, m_velName);
	DDX_Text(pDX, IDC_DLG_ZELTNAME, m_reidName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgLoadReid, CDialog)
	//{{AFX_MSG_MAP(CdlgLoadReid)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, OnButtonBrowsev)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEVASCII, OnButtonBrowsevascii)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgLoadReid message handlers

void CdlgLoadReid::OnButtonBrowsev() 
{
	CFileDialog dlg( FALSE,_T("bin"),_T("*.bin"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("SeisWide Binary Velocity Format (*.bin)|*.bin|"));
	if( dlg.DoModal()==IDOK )
	{
		m_velName = dlg.GetPathName();
		UpdateData(FALSE);
	}
	
	
}

void CdlgLoadReid::OnButtonBrowsevascii() 
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("mod"),_T("*.mod"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Reid's Velocity Format (*.mod)|*.mod|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Reid's Velocity Format |mod |"));
#endif
	if( dlg.DoModal()==IDOK )
	{
		m_reidName = dlg.GetPathName();
		UpdateData(FALSE);
	}	
}
