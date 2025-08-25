// CdlgAddData.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgAddData.h"
//#include "afxdialogex.h"


// CdlgAddData dialog

IMPLEMENT_DYNAMIC(CdlgAddData, CDialog)

CdlgAddData::CdlgAddData(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgAddData::IDD, pParent)
	, m_timeSecName(_T(""))
	, m_timeSecName2(_T(""))
	, m_isAutoScaling(FALSE)
	, m_outputSecName(_T(""))
{

}

CdlgAddData::~CdlgAddData()
{
}

void CdlgAddData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_2NDSECNAME, m_timeSecName2);
	DDX_Check(pDX, IDC_CHK_AMPSCALE, m_isAutoScaling);
	DDX_Text(pDX, IDC_OUTPUTSECNAME, m_outputSecName);
}


BEGIN_MESSAGE_MAP(CdlgAddData, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, &OnBnClickedButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSED, &OnBnClickedButtonBrowsed)
END_MESSAGE_MAP()


// CdlgAddData message handlers


void CdlgAddData::OnBnClickedButtonBrowset()
{
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("First Segy File (*.sgy)|*.sgy|"));
	if( dlg.DoModal()==IDOK )
	{
		m_timeSecName = dlg.GetPathName();
		UpdateData(FALSE);
	}
	
}


void CdlgAddData::OnBnClickedButtonBrowsed()
{
	CFileDialog dlg(TRUE, _T("sgy"), _T("*.sgy"),
		OFN_HIDEREADONLY,
		_T("Second Segy File (*.sgy)|*.sgy|"));
	if (dlg.DoModal() == IDOK)
	{
		m_timeSecName2 = dlg.GetPathName();
		UpdateData(FALSE);
	}
}