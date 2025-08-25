// dlgMigPS.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgMigPS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgMigPS dialog


CdlgMigPS::CdlgMigPS(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgMigPS::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgMigPS)
	m_dz = 0.0f;
	m_nx = 56;
	m_ltaper = 0;
	m_outFile = _T("");
	m_nxPad = 0;
	m_inFile = _T("");
	m_velSecName = _T("");
	//}}AFX_DATA_INIT
}


void CdlgMigPS::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgMigPS)
	DDX_Text(pDX, IDC_CONVERSION_DZ, m_dz);
	DDV_MinMaxFloat(pDX, m_dz, 1.e-005f, 10000.f);
	DDX_Text(pDX, IDC_MIGRATION_NX, m_nx);
	DDV_MinMaxInt(pDX, m_nx, 3, 99999);
	DDX_Text(pDX, IDC_MIGRATION_LTAPER, m_ltaper);
	DDX_Text(pDX, IDC_DEPTHSECNAME, m_outFile);
	DDX_Text(pDX, IDC_MIGRATION_NXPAD, m_nxPad);
	DDX_Text(pDX, IDC_TIMESECNAME, m_inFile);
	DDX_Text(pDX, IDC_VELSECNAME, m_velSecName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgMigPS, CDialog)
	//{{AFX_MSG_MAP(CdlgMigPS)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, &CdlgMigPS::OnBnClickedButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, &CdlgMigPS::OnBnClickedButtonBrowsev)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgMigPS message handlers

void CdlgMigPS::OnBnClickedButtonBrowset()
{
	// TODO: Add your control notification handler code here
}

void CdlgMigPS::OnBnClickedButtonBrowsev()
{
	// TODO: Add your control notification handler code here
}
