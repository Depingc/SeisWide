// dlgAddRange.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgAddRange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgAddRange dialog


CdlgAddRange::CdlgAddRange(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgAddRange::IDD, pParent)
//	, m_refTrace(0)
, m_trace1(0)
, m_isGs(FALSE)
{
	//{{AFX_DATA_INIT(CdlgAddRange)
	m_refDx = 0.0125f;
	m_refRange0 = 0.0f;
	m_refName = _T("");
	//m_refCDPTrue = 0;
	m_assignMethod = 0;
	m_deadkm = TRUE;
	//m_nameOffset = _T("");
	//}}AFX_DATA_INIT
}


void CdlgAddRange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgAddRange)
	DDX_Text(pDX, IDC_REFNAME, m_refName);
	DDX_Text(pDX, IDC_REFRANGE0, m_refRange0);
	DDX_Text(pDX, IDC_REFDX, m_refDx);
	DDX_Check(pDX, IDC_CHECK1, m_deadkm);
	//DDX_Text(pDX, IDC_EDIT1, m_nameOffset);
	//DDX_Radio(pDX, IDC_REFCDPTRUE, m_refCDPTrue); // this not existing?
												  //}}AFX_DATA_MAP
												  //	DDX_Text(pDX, IDC_REF_TRACE, m_refTrace);
												  //	DDV_MinMaxInt(pDX, m_refTrace, 0, 999999);
	DDX_Radio(pDX, IDC_ASSIGN_METHOD, m_assignMethod);
	DDX_Text(pDX, IDC_TRACE1, m_trace1);
	DDX_Check(pDX, IDC_CHECK_IS_GS, m_isGs);
}


BEGIN_MESSAGE_MAP(CdlgAddRange, CDialog)
	//{{AFX_MSG_MAP(CdlgAddRange)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	//ON_BN_CLICKED(IDC_BUTTON1_OFFSET, OnButton1Offset)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIO14, &CdlgAddRange::OnBnClickedRadio14)
	ON_BN_CLICKED(IDC_ASSIGN_METHOD2, &CdlgAddRange::OnBnClickedAssignMethod2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgAddRange message handlers

void CdlgAddRange::OnButtonBrowset() 
{
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Files (*.sgy)|*.sgy|"));
	if( dlg.DoModal()==IDOK ) {
		m_refName = dlg.GetPathName();
		//if (CWnd::IsDlgButtonChecked(IDC_ASSIGN_METHOD)) m_assignMethod = 0;
		//if (CWnd::IsDlgButtonChecked(IDC_RADIO5)) m_assignMethod = 1;
		//if (CWnd::IsDlgButtonChecked(IDC_RADIO1)) m_assignMethod = 2;
		//if (CWnd::IsDlgButtonChecked(IDC_RADIO14) ) m_assignMethod = 3;
		UpdateData(FALSE);
	}
}
//
//void CdlgAddRange::OnButton1Offset() 
//{
//	// browse for an ascii file name to assign offset info.
//	CFileDialog dlg( TRUE,_T("txt|sht"),_T("*.txt"),
//	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
//	                 _T("2-3 col Ascii (*.txt) or shot table (*.sht) |*.txt|*.sht|"));
//	if( dlg.DoModal()==IDOK ) {
//		m_nameOffset = dlg.GetPathName();
//		//if (CWnd::IsDlgButtonChecked(IDC_RADIO1)) m_assignMethod = 2;
//		//if (CWnd::IsDlgButtonChecked(IDC_RADIO14) ) m_assignMethod = 3;
//
//		UpdateData(FALSE);
//	}
//	
//}

void CdlgAddRange::OnRadio1() 
{
	// click this will show up a few controls to assign offset/distance info.
	CWnd* pwnd;
	pwnd = GetDlgItem(IDC_EDIT1); if (pwnd) pwnd->EnableWindow(TRUE);
	pwnd = GetDlgItem(IDC_BUTTON1_OFFSET); if (pwnd) pwnd->EnableWindow(TRUE);

}

void CdlgAddRange::OnBnClickedRadio14()
{
	// click this will show up a few controls to assign offset info.
	CWnd* pwnd;
	pwnd = GetDlgItem(IDC_EDIT1); if (pwnd) pwnd->EnableWindow(TRUE);
	pwnd = GetDlgItem(IDC_BUTTON1_OFFSET); if (pwnd) pwnd->EnableWindow(TRUE);
}


void CdlgAddRange::OnBnClickedAssignMethod2()
{
	// TODO: Add your control notification handler code here
}
