// dlgImportbitmap.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
#include "dlgImportbitmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgImportbitmap dialog


CdlgImportbitmap::CdlgImportbitmap(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgImportbitmap::IDD, pParent)
	, m_isOffset(FALSE)
{
	//{{AFX_DATA_INIT(CdlgImportbitmap)
	m_Z2 = 0.0f;
	m_Z1 = 0.0f;
	m_X2 = 0.0f;
	m_X1 = 0.0f;
	m_name = _T("");
	m_isDigitizing = TRUE;
	//}}AFX_DATA_INIT
}


void CdlgImportbitmap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgImportbitmap)
	DDX_Text(pDX, IDC_TIMESECNAME, m_name);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMIN, m_X1);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMAX, m_X2);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMIN, m_Z1);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMAX, m_Z2);
	//	DDX_Radio(pDX, IDC_VIEW_FACEDOWN, m_faceDown);
	//}}AFX_DATA_MAP
	DDX_Radio(pDX, IDC_VIEW_OFFSET, m_isOffset);
}


BEGIN_MESSAGE_MAP(CdlgImportbitmap, CDialog)
	//{{AFX_MSG_MAP(CdlgImportbitmap)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgImportbitmap message handlers

void CdlgImportbitmap::OnButtonBrowset() 
{
	CFileDialog dlg( TRUE,_T("Any image format"),_T("*.*"),
	                 NULL,
	                 _T("Bitmap file (*.*)|*.*|"));

	if( dlg.DoModal()==IDOK )
	{
		m_name = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_TIMESECNAME));
		pEdit->SetWindowText(m_name);
	}
	
}

BOOL CdlgImportbitmap::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CDialog::PreCreateWindow(cs);
}

BOOL CdlgImportbitmap::OnInitDialog() 
{
	CWnd* pwnd;
	if (!m_isDigitizing) {
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_XMIN); if (pwnd) pwnd->EnableWindow(FALSE); 
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_XMAX); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_TMIN); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_UPDATE_TMAX); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_OFFSET); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_DIST); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_VIEW_DIST2); if (pwnd) pwnd->EnableWindow(FALSE);
	}

	CDialog::OnInitDialog();
	
	//if (!isLoading)
	//{
	//	CWnd* pwnd;
	//	pwnd = GetDlgItem(IDC_TIMESECNAME); if (pwnd) pwnd->EnableWindow(FALSE);
	//	pwnd = GetDlgItem(IDC_VIEW_UPDATE_ZPROP); if (pwnd) pwnd->EnableWindow(FALSE);
	//	pwnd = GetDlgItem(IDC_BUTTON_BROWSET); if (pwnd) pwnd->EnableWindow(FALSE);
	//	
	//}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
