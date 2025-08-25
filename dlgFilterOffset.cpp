// dlgFilterOffset.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgFilterOffset.h"


// CdlgFilterOffset dialog

IMPLEMENT_DYNAMIC(CdlgFilterOffset, CDialog)

CdlgFilterOffset::CdlgFilterOffset(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgFilterOffset::IDD, pParent)
	, m_fLowa(0)
	, m_fHigha(0)
	, m_ta(0)
	, m_fLowb(0)
	, m_fHighb(0)
	, m_tb(0)
	, m_x1(0)
	, m_fLowax2(0)
	, m_fHighax2(0)
	, m_tax2(0)
	, m_fLowbx2(0)
	, m_fHighbx2(0)
	, m_tbx2(0)
	, m_x2(0)
	, m_fLowax3(0)
	, m_fHighax3(0)
	, m_tax3(0)
	, m_fLowbx3(0)
	, m_fHighbx3(0)
	, m_tbx3(0)
	, m_x3(0)
	, m_nameIn(_T(""))
	, m_nameOut(_T(""))
	, m_numTFilter(0)
{

}

CdlgFilterOffset::~CdlgFilterOffset()
{
}

void CdlgFilterOffset::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROCNAME_IN, m_nameIn);
	DDX_Text(pDX, IDC_PROCNAME_OUT, m_nameOut);

	DDX_Text(pDX, IDC_PROC_FILTERF1, m_fLowa);
	DDX_Text(pDX, IDC_PROC_FILTERF2, m_fHigha);
	DDX_Text(pDX, IDC_PROC_FILTERTA, m_ta);
	DDX_Text(pDX, IDC_PROC_FILTERF1B, m_fLowb);
	DDX_Text(pDX, IDC_PROC_FILTERF2B, m_fHighb);
	DDX_Text(pDX, IDC_PROC_FILTERTB, m_tb);
	DDX_Text(pDX, IDC_PROC_FILTERX1, m_x1);

	DDX_Text(pDX, IDC_PROC_FILTERF1X2, m_fLowax2);
	DDX_Text(pDX, IDC_PROC_FILTERF2X2, m_fHighax2);
	DDX_Text(pDX, IDC_PROC_FILTERTAX2, m_tax2);
	DDX_Text(pDX, IDC_PROC_FILTERF1BX2, m_fLowbx2);
	DDX_Text(pDX, IDC_PROC_FILTERF2BX2, m_fHighbx2);
	DDX_Text(pDX, IDC_PROC_FILTERTBX2, m_tbx2);
	DDX_Text(pDX, IDC_PROC_FILTERX2, m_x2);

	DDX_Text(pDX, IDC_PROC_FILTERF1X3, m_fLowax3);
	DDX_Text(pDX, IDC_PROC_FILTERF2X3, m_fHighax3);
	DDX_Text(pDX, IDC_PROC_FILTERTAX3, m_tax3);
	DDX_Text(pDX, IDC_PROC_FILTERF1BX3, m_fLowbx3);
	DDX_Text(pDX, IDC_PROC_FILTERF2BX3, m_fHighbx3);
	DDX_Text(pDX, IDC_PROC_FILTERTBX3, m_tbx3);
	DDX_Text(pDX, IDC_PROC_FILTERX3, m_x3);

	DDX_Text(pDX, IDC_PROC_NUM_TFILTER, m_numTFilter);
}


BEGIN_MESSAGE_MAP(CdlgFilterOffset, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, &CdlgFilterOffset::OnBnClickedButtonBrowset)
END_MESSAGE_MAP()


// CdlgFilterOffset message handlers

void CdlgFilterOffset::OnBnClickedButtonBrowset()
{
	// TODO: Add your control notification handler code here
}
