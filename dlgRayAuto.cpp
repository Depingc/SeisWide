// dlgRayAuto.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgRayAuto.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgRayAuto dialog


CdlgRayAuto::CdlgRayAuto(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgRayAuto::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgRayAuto)
	m_zmax = 0.0f;
	m_zmin = 0.0f;
	m_tmax = 0.0f;
	m_tmin = 0.0f;
	m_xmax = 0.0f;
	m_xmin = 0.0f;
	m_xshot = 0.001f;
	m_zshot = 0.0f;
	m_vred = 0.0f;
	m_newName = _T("");
	m_timeSecName = _T("");
	//}}AFX_DATA_INIT
}


void CdlgRayAuto::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgRayAuto)
	DDX_Text(pDX, IDC_EDIT_NEWNAME, m_newName);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMIN, m_xmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMAX, m_xmax);
	DDX_Text(pDX, IDC_CONVERSION_ZMIN, m_zmin);
	DDX_Text(pDX, IDC_CONVERSION_ZMAX, m_zmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMIN, m_tmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMAX, m_tmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_VRED, m_vred);
	DDX_Text(pDX, IDC_XSHOT, m_xshot);
	DDX_Text(pDX, IDC_ZSHOT, m_zshot);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgRayAuto, CDialog)
	//{{AFX_MSG_MAP(CdlgRayAuto)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgRayAuto message handlers

void CdlgRayAuto::OnButtonBrowset() 
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy File (*.sgy)|*.sgy|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy File |sgy |"));
#endif
	
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
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_TIMESECNAME));
		pEdit->SetWindowText(m_timeSecName);
	}
	t.ReleaseBuffer();
}
