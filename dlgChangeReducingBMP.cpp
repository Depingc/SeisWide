// dlgChangeReducingBMP.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
#include "dlgChangeReducingBMP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgChangeReducingBMP dialog


CdlgChangeReducingBMP::CdlgChangeReducingBMP(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgChangeReducingBMP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgChangeReducingBMP)
	m_bmpIn = _T("");
	m_bmpOut = _T("");
	m_bitSize = -1;
	m_velOut = 0.0f;
	m_velIn = 0.0f;
	m_col = 0;
	m_row = 0;
	m_sourceKm = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgChangeReducingBMP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgChangeReducingBMP)
	DDX_Text(pDX, IDC_BMPSECNAME, m_bmpIn);
	DDX_Text(pDX, IDC_DEPTHSECNAME, m_bmpOut);
	DDX_Text(pDX, IDC_VIEW_UPDATE_COL, m_col);
	DDX_Text(pDX, IDC_VIEW_UPDATE_ROW, m_row);
	DDX_Text(pDX, IDC_REDUCEVELOCITY_OLD, m_velIn);
	DDX_Text(pDX, IDC_REDUCEVELOCITY_NEW, m_velOut);
	DDX_Text(pDX, IDC_REDUCEVELOCITY_SOURCEKM, m_sourceKm);
	DDX_Radio(pDX, IDC_RADIO1, m_bitSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgChangeReducingBMP, CDialog)
	//{{AFX_MSG_MAP(CdlgChangeReducingBMP)
	ON_BN_CLICKED(IDC_BUTTON_BROWSED, OnButtonBrowsed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgChangeReducingBMP message handlers

void CdlgChangeReducingBMP::OnButtonBrowsed() 
{
	// TODO: Add your control notification handler code here
#ifndef _MAC
	CFileDialog dlg( FALSE,_T("bmp"),_T("*.bmp"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("bmp Output Section (*.bmp)|*.bmp|"));
#else
	CFileDialog dlg( FALSE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("bmp Output Section |bmp |"));
#endif
	if( dlg.DoModal()==IDOK )
	{
		m_bmpOut = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DEPTHSECNAME));
		pEdit->SetWindowText(m_bmpOut);

	}
	
}
