// dlgMergeModels.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgMergeModels.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgMergeModels dialog


CdlgMergeModels::CdlgMergeModels(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgMergeModels::IDD, pParent)
	, m_shiftKm2(0)
	, m_vTolerancePerc(0)
	, m_shiftKm(0)
{
	//{{AFX_DATA_INIT(CdlgMergeModels)
	m_velSecName = _T("");
	m_velSecName2 = _T("");
	//}}AFX_DATA_INIT
}


void CdlgMergeModels::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgMergeModels)
	DDX_Text(pDX, IDC_VELSECNAME, m_velSecName);
	DDX_Text(pDX, IDC_SHIFT_KM, m_shiftKm);
	DDX_Text(pDX, IDC_VELSECNAME2, m_velSecName2);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_SEPARATION_KM, m_shiftKm2);
	DDX_Text(pDX, IDC_VTOLERANCE_PERC, m_vTolerancePerc);
}


BEGIN_MESSAGE_MAP(CdlgMergeModels, CDialog)
	//{{AFX_MSG_MAP(CdlgMergeModels)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET2, OnButtonBrowset2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgMergeModels message handlers

void CdlgMergeModels::OnButtonBrowset() 
{
	//CFileDialog dlg(TRUE, _T(""), _T("*.vxml;*.bin"),
	//	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	//	_T("File name for model (*.vxml)|*.vxml|Older format (*.bin)|*.bin|"));
	CFileDialog dlg(TRUE, _T("bin"), _T("*.bin"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("First Model to Merge (*.bin)|*.bin|"));
	if( dlg.DoModal()==IDOK )
	{
		m_velSecName = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CdlgMergeModels::OnButtonBrowset2() 
{
	CFileDialog dlg( TRUE,_T("bin"),_T("*.bin"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("First Model to Merge (*.bin)|*.bin|"));
	if( dlg.DoModal()==IDOK )
	{
		m_velSecName2 = dlg.GetPathName();
		UpdateData(FALSE);
	}
	
}
