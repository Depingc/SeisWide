// dlgLoadZelt.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgLoadZelt.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgLoadZelt dialog


CdlgLoadZelt::CdlgLoadZelt(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgLoadZelt::IDD, pParent)
	, m_is4Decimal(FALSE)
{
	//{{AFX_DATA_INIT(CdlgLoadZelt)
	m_velName = BLANK;
	m_ZeltName = BLANK;
	//}}AFX_DATA_INIT
}


void CdlgLoadZelt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgLoadZelt)
	DDX_Text(pDX, IDC_DLG_VELNAME, m_velName);
	DDX_Text(pDX, IDC_DLG_ZELTNAME, m_ZeltName);
	//}}AFX_DATA_MAP
	DDX_Radio(pDX, IDC_RADIO_4DECIMAL, m_is4Decimal);
}


BEGIN_MESSAGE_MAP(CdlgLoadZelt, CDialog)
	//{{AFX_MSG_MAP(CdlgLoadZelt)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEVASCII, OnButtonBrowsevascii)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, OnButtonBrowsev)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgLoadZelt message handlers

void CdlgLoadZelt::OnButtonBrowsevascii() 
{
	CFileDialog dlg( TRUE,BLANK,_T("*.in; *.vxml"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Zelt's Ascii Velocity (*.in) or XML format (*.vxml)|*.in; *.vxml|"));
	if( dlg.DoModal()==IDOK )
	{
		m_ZeltName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DLG_ZELTNAME));
		pEdit->SetWindowText(m_ZeltName);

		m_velName = SetFileExtension(getFileNameAppend(m_ZeltName, _T("Conv")), _T("vxml"));
		UpdateData(0);
	}
	
}

void CdlgLoadZelt::OnButtonBrowsev() 
{
	CFileDialog dlg( FALSE,BLANK,_T("*.bin; *.vxml"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	      //           _T("SeisWide Binary Velocity Format (*.bin)|*.bin|"));
					_T("File name for new model (*.vxml)|*.vxml|Older format (*.bin)|*.bin|"));
	if( dlg.DoModal()==IDOK )
	{
		m_velName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DLG_VELNAME));
		pEdit->SetWindowText(m_velName);
	}
	
}
