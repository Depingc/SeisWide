// dlgAddRange.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgAddRangeFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgAddRangeFile dialog


CdlgAddRangeFile::CdlgAddRangeFile(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgAddRangeFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgAddRangeFile)
	m_refName = _T("");
	m_assignMethod = 0;
	m_nameOffset = _T("");
	//}}AFX_DATA_INIT
}


void CdlgAddRangeFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgAddRange)
	DDX_Text(pDX, IDC_REFNAME, m_refName);
	DDX_Text(pDX, IDC_EDIT1, m_nameOffset);
	DDX_Radio(pDX, IDC_ASSIGN_METHOD, m_assignMethod);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgAddRangeFile, CDialog)
	//{{AFX_MSG_MAP(CdlgAddRangeFile)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON1_OFFSET, OnButton1Offset)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIO14, &CdlgAddRangeFile::OnBnClickedRadio14)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgAddRangeFile message handlers

void CdlgAddRangeFile::OnButtonBrowset() 
{
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Files (*.sgy)|*.sgy"));
	if( dlg.DoModal()==IDOK ) {
		m_refName = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CdlgAddRangeFile::OnButton1Offset() 
{
	// browse for an ascii file name to assign offset info.
	CFileDialog dlg( TRUE,_T("txt|sht"),_T("*.txt"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("2 col Ascii (*.txt) or shot table (*.sht) |*.txt|*.sht|"));
	if( dlg.DoModal()==IDOK ) {
		m_nameOffset = dlg.GetPathName();
		UpdateData(FALSE);
	}
	
}

void CdlgAddRangeFile::OnRadio1() 
{
	// click this will show up a few controls to assign offset/distance info.
	CWnd* pwnd;
	pwnd = GetDlgItem(IDC_EDIT1); if (pwnd) pwnd->EnableWindow(TRUE);
	pwnd = GetDlgItem(IDC_BUTTON1_OFFSET); if (pwnd) pwnd->EnableWindow(TRUE);

}

void CdlgAddRangeFile::OnBnClickedRadio14()
{
	// click this will show up a few controls to assign offset info.
	CWnd* pwnd;
	pwnd = GetDlgItem(IDC_EDIT1); if (pwnd) pwnd->EnableWindow(TRUE);
	pwnd = GetDlgItem(IDC_BUTTON1_OFFSET); if (pwnd) pwnd->EnableWindow(TRUE);
}
