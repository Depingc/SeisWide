// dlgMigLineDraw.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgMigLineDraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgMigLineDraw dialog


CdlgMigLineDraw::CdlgMigLineDraw(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgMigLineDraw::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgMigLineDraw)
	m_aamax = 90.0f;
	m_aamin = 0.0f;
	m_dv = 0.1f;
	m_dxv = 0.0f;
	m_maxTrc = 100;
	m_nLayer = 0;
	m_sDepth = 0.0f;
	m_sDist = 0.0f;
	m_tErr = 0.05f;
	m_xErr = 0.5f;
	m_migName = _T("");
	m_velName = _T("");
	m_append = TRUE;
	//}}AFX_DATA_INIT
}


void CdlgMigLineDraw::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgMigLineDraw)
	DDX_Text(pDX, ID_DIGITIZE_MIGAAMAX, m_aamax);
	DDV_MinMaxFloat(pDX, m_aamax, 1.e-002f, 90.1f);
	DDX_Text(pDX, ID_DIGITIZE_MIGAAMIN, m_aamin);
	DDV_MinMaxFloat(pDX, m_aamin, 0.f, 90.f);
	DDX_Text(pDX, ID_DIGITIZE_MIGDV, m_dv);
	DDV_MinMaxFloat(pDX, m_dv, 1.e-002f, 1.f);
	DDX_Text(pDX, ID_DIGITIZE_MIGDXV, m_dxv);
	DDV_MinMaxFloat(pDX, m_dxv, 1.e-005f, 100.f);
	DDX_Text(pDX, ID_DIGITIZE_MIGMAXTRC, m_maxTrc);
	DDV_MinMaxInt(pDX, m_maxTrc, 10, 10000);
	DDX_Text(pDX, ID_DIGITIZE_MIGNLAYER, m_nLayer);
	DDV_MinMaxInt(pDX, m_nLayer, 0, 40);
	DDX_Text(pDX, ID_DIGITIZE_MIGSDEPTH, m_sDepth);
	DDV_MinMaxFloat(pDX, m_sDepth, 0.f, 10000.f);
	DDX_Text(pDX, ID_DIGITIZE_MIGSDIST, m_sDist);
	DDX_Text(pDX, ID_DIGITIZE_MIGTERR, m_tErr);
	DDV_MinMaxFloat(pDX, m_tErr, 1.e-003f, 100.f);
	DDX_Text(pDX, ID_DIGITIZE_MIGXERR, m_xErr);
	DDV_MinMaxFloat(pDX, m_xErr, 1.e-003f, 100.f);
	DDX_Text(pDX, IDC_MIGSECNAME, m_migName);
	DDX_Text(pDX, IDC_VELSECNAME, m_velName);
	DDX_Check(pDX, IDC_CHECK1, m_append);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgMigLineDraw, CDialog)
	//{{AFX_MSG_MAP(CdlgMigLineDraw)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, OnButtonBrowsev)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV2, OnButtonBrowsev2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgMigLineDraw message handlers

void CdlgMigLineDraw::OnButtonBrowsev() 
{
	CFileDialog dlg(TRUE, _T(""), _T("*.vxml;*.bin"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("File name for model (*.vxml)|*.vxml|Older format (*.bin)|*.bin|"));
	//CFileDialog dlg(TRUE, _T("bin"), _T("*.bin"),
	//					 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	//					 _T("SeisWide Binary Velocity Format (*.bin)|*.bin|"));
	if( dlg.DoModal()==IDOK )
	{
		m_velName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VELSECNAME));
		pEdit->SetWindowText(m_velName);
	}
}

void CdlgMigLineDraw::OnButtonBrowsev2() 
{
	#ifndef _MAC
		CFileDialog dlg( TRUE,_T("txt"),_T("*.txt"),
						 OFN_NOREADONLYRETURN,
						 _T("Append File For Output (*.txt)|*.txt|"));
	#else
		CFileDialog dlg( TRUE,NULL,NULL,
						 OFN_NOREADONLYRETURN,
						 _T("Append File For Output |txt |"));
	#endif
	if( dlg.DoModal()==IDOK )
	{
		m_migName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_MIGSECNAME));
		pEdit->SetWindowText(m_migName);
	}
}
