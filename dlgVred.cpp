// dlgVred.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgVred.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgVred dialog


CdlgVred::CdlgVred(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgVred::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgVred)
	m_timeSecName = _T("");
	m_vred = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgVred::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgVred)
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_EDIT_VRED, m_vred);
	DDV_MinMaxFloat(pDX, m_vred, -999.f, 999.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgVred, CDialog)
	//{{AFX_MSG_MAP(CdlgVred)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, &CdlgVred::OnBnClickedButtonBrowset)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgVred message handlers



void CdlgVred::OnBnClickedButtonBrowset()
{
	CFileDialog dlg(TRUE, _T("sgy"), _T("*.sgy"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Segy Time Section (*.sgy)|*.sgy|"));
	if (dlg.DoModal() == IDOK) {
		m_timeSecName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_PROCNAME_IN));
		pEdit->SetWindowText(m_timeSecName);
	}
}
