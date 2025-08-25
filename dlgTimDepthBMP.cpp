// dlgTimDepthBMP.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
#include "dlgTimDepthBMP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgTimDepthBMP dialog


CdlgTimDepthBMP::CdlgTimDepthBMP(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgTimDepthBMP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgTimDepthBMP)
	m_zmax = 0.0f;
	m_zmin = 0.0f;
	m_xmax = 0.0f;
	m_xmin = 0.0f;
	m_velSecName = _T("");
	m_bmpIN = _T("");
	m_bmpOUT = _T("");
	m_col = 0;
	m_row = 0;
	m_bitSize = -1;
	//}}AFX_DATA_INIT
}


void CdlgTimDepthBMP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgTimDepthBMP)
	DDX_Text(pDX, IDC_BMPSECNAME, m_bmpIN);
	DDX_Text(pDX, IDC_VELSECNAME, m_velSecName);
	DDX_Text(pDX, IDC_DEPTHSECNAME, m_bmpOUT);
	DDX_Text(pDX, IDC_VIEW_UPDATE_COL, m_col);
	DDV_MinMaxInt(pDX, m_col, 10, 9999999);
	DDX_Text(pDX, IDC_VIEW_UPDATE_ROW, m_row);
	DDV_MinMaxInt(pDX, m_row, 10, 9999999);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMIN, m_xmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMAX, m_xmax);
	DDX_Text(pDX, IDC_CONVERSION_ZMIN, m_zmin);
	DDV_MinMaxFloat(pDX, m_zmin, 0.f, 10000.f);
	DDX_Text(pDX, IDC_CONVERSION_ZMAX, m_zmax);
	DDV_MinMaxFloat(pDX, m_zmax, 1.e-004f, 10000.f);

	DDX_Radio(pDX, IDC_RADIO1, m_bitSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgTimDepthBMP, CDialog)
	//{{AFX_MSG_MAP(CdlgTimDepthBMP)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, OnButtonBrowsev)
	ON_BN_CLICKED(IDC_BUTTON_BROWSED, OnButtonBrowsed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgTimDepthBMP message handlers

void CdlgTimDepthBMP::OnButtonBrowsev() 
{
	CFileDialog dlg(TRUE, _T(""), _T("*.vxml;*.bin"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("File name for model (*.vxml)|*.vxml|Older format (*.bin)|*.bin|"));
	if( dlg.DoModal()==IDOK )
	{
		m_velSecName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VELSECNAME));
		pEdit->SetWindowText(m_velSecName);
	}
	
}

void CdlgTimDepthBMP::OnButtonBrowsed() 
{
#ifndef _MAC
	CFileDialog dlg( FALSE,_T("bmp"),_T("*.bmp"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("bmp Depth Section (*.bmp)|*.bmp|"));
#else
	CFileDialog dlg( FALSE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("bmp Depth Section |bmp |"));
#endif
	if( dlg.DoModal()==IDOK )
	{
		m_bmpOUT = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DEPTHSECNAME));
		pEdit->SetWindowText(m_bmpOUT);
	}
	
}
