// dlgDigitize.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgDigitize.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgDigitize dialog


CdlgDigitize::CdlgDigitize(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgDigitize::IDD, pParent)

	, m_isSort(0)
	, m_tlag(0)
	, m_tmin(0)
	, m_isEnergy(FALSE)
	, m_tmax(0)
	, m_numSubdivide(0)
	, m_lagWeight(0)
	, m_isMinusXShot(FALSE)
{
	//{{AFX_DATA_INIT(CdlgDigitize)
	m_digName = BLANK;
	m_digFileFormat = 0;
	m_points = 500;
	m_curves = 200;
	m_xs = 0.0f;
	m_comments = BLANK;
	//}}AFX_DATA_INIT
}


void CdlgDigitize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgDigitize)
	DDX_Text(pDX, IDC_EDIT_DIGITIZE, m_digName);
	DDX_Radio(pDX, IDC_DIG_FILEFORMAT, m_digFileFormat);
	DDX_Radio(pDX, IDC_RADIO_SORT, m_isSort);
	DDX_Text(pDX, IDC_EDIT_OBSDIST, m_xs);
	DDX_Text(pDX, IDC_DIG_COMMENTS, m_comments);
	DDX_Check(pDX, IDC_IS_MINUS_OFFSET, m_isMinusXShot);
	DDX_Text(pDX, IDC_TLAG, m_tlag);
	DDX_Text(pDX, IDC_LAG_WEIGHT, m_lagWeight);
	DDX_Text(pDX, IDC_TMIN, m_tmin);
	DDX_Text(pDX, IDC_EDIT_TMAX, m_tmax);
	DDX_Check(pDX, IDC_IS_ENERGY, m_isEnergy);
	DDX_Text(pDX, IDC_EDIT_NUMDIV, m_numSubdivide);

	DDX_Text(pDX, IDC_EDIT1, m_points);
	DDV_MinMaxInt(pDX, m_points, 10, 2000);
	DDX_Text(pDX, IDC_EDIT2, m_curves);
	DDV_MinMaxInt(pDX, m_curves, 10, 2000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgDigitize, CDialog)
	//{{AFX_MSG_MAP(CdlgDigitize)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_BROWSERIN, &CdlgDigitize::OnBnClickedButtonBrowserin)
	ON_BN_CLICKED(IDC_DIG_FILEFORMAT, &CdlgDigitize::OnBnClickedDigFileformat)
	ON_BN_CLICKED(IDC_RADIO2, &CdlgDigitize::OnBnClickedRadio2)
	ON_BN_CLICKED(IDHELP, &CdlgDigitize::OnBnClickedHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgDigitize message handlers

BOOL CdlgDigitize::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWnd* pwnd;
	if (m_mode == 1)
	{
		pwnd = GetDlgItem(IDC_EDIT1); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_EDIT2); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_DIG_FILEFORMAT); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_EDIT_DIGITIZE); if (pwnd) pwnd->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CdlgDigitize::OnBnClickedButtonBrowserin()
{
	CFileDialog dlg( TRUE,_T("in;txt"),_T("*.in; *.txt"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Digitize File (*.in;*.txt)|*.in;*.txt|"));


	int pathLen = m_digName.ReverseFind( '\\' );

	CString t = m_digName.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_digName = dlg.GetPathName();
		if (isEndWith(m_digName, _T(".txt"))) {
			this->m_digFileFormat = 0;
		}
		else {
			this->m_digFileFormat = 1;
		}

		UpdateData(FALSE);
	}
	
	t.ReleaseBuffer();
}

void CdlgDigitize::OnBnClickedDigFileformat()
{
	// the text format is selected
	// CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_EDIT_DIGITIZE));
	//if (isEndWith(m_digName, _T("digZelt.in"))) {
	//	m_digName.Replace(_T("digZelt.in"), _T("digi.txt"));
	//	pEdit->SetWindowText(m_digName);
	//}
	//else if (isEndWith(m_digName, _T("digZelta.in"))) {
	//	m_digName.Replace(_T("digZelta.in"), _T("digia.txt"));
	//	CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_EDIT_DIGITIZE));
	//	pEdit->SetWindowText(m_digName);
	//}
	if  (isEndWith(m_digName, _T(".in"))) {
		m_digName = SetFileExtension(m_digName, _T("txt"));
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_EDIT_DIGITIZE));
		pEdit->SetWindowText(m_digName);
	}
	UpdateData(TRUE);
}

void CdlgDigitize::OnBnClickedRadio2()
{
	// the Zelt's format is selected
	//CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_EDIT_DIGITIZE));
	//if (isEndWith(m_digName, _T("digi.txt"))) {
	//	m_digName.Replace(_T("digi.txt"), _T("digZelt.in"));
	//	pEdit->SetWindowText(m_digName);
	//}
	//else if (isEndWith(m_digName, _T("digia.txt"))) {
	//	m_digName.Replace(_T("digia.txt"), _T("digZelta.in"));
	//	pEdit->SetWindowText(m_digName);
	//}
	if (isEndWith(m_digName, _T(".txt"))) {
		m_digName = SetFileExtension(m_digName, _T("in"));
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_EDIT_DIGITIZE));
		pEdit->SetWindowText(m_digName);
	}
	UpdateData(TRUE);
}

void CdlgDigitize::OnBnClickedHelp()
{
	myShowHelp(_T("index.html"));
}
