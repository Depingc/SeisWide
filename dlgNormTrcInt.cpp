// dlgNormTrcInt.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgNormTrcInt.h"
#include "afxdialogex.h"


// CdlgNormTrcInt dialog

IMPLEMENT_DYNAMIC(CdlgNormTrcInt, CDialog)

CdlgNormTrcInt::CdlgNormTrcInt(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DLG_SEGYNORM_TRCINT, pParent)
	, m_timeSecName(_T(""))
	, m_multFactor(1)
	, m_isAutoScaling(FALSE)
	, m_outputSecName(_T(""))
{

}

CdlgNormTrcInt::~CdlgNormTrcInt()
{
}

void CdlgNormTrcInt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_MULT_FACTOR, m_multFactor);
	DDX_Check(pDX, IDC_CHK_AMPSCALE, m_isAutoScaling);
	DDX_Text(pDX, IDC_OUTPUTSECNAME, m_outputSecName);
}


BEGIN_MESSAGE_MAP(CdlgNormTrcInt, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, &OnBnClickedButtonBrowset)
END_MESSAGE_MAP()


// CdlgNormTrcInt message handlers
void CdlgNormTrcInt::OnBnClickedButtonBrowset()
{
	CFileDialog dlg(TRUE, _T("sgy"), _T("*.sgy"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("First Segy File (*.sgy)|*.sgy|"));
	if (dlg.DoModal() == IDOK)
	{
		m_timeSecName = dlg.GetPathName();
		UpdateData(FALSE);
	}

}

