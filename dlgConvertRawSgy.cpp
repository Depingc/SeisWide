// dlgConvertRawSgy.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgConvertRawSgy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgConvertRawSgy dialog


CdlgConvertRawSgy::CdlgConvertRawSgy(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgConvertRawSgy::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgConvertRawSgy)
	m_outName = _T("");
	m_tabName = _T("");
	m_tmax = 0.0f;
	m_tmin = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgConvertRawSgy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgConvertRawSgy)
	DDX_Text(pDX, IDC_DLG_TABNAME, m_tabName);
	DDX_Text(pDX, IDC_EDIT_TMIN, m_tmin);
	DDX_Text(pDX, IDC_EDIT_TMAX, m_tmax);
	DDX_Text(pDX, IDC_DLG_OUTNAME, m_outName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgConvertRawSgy, CDialog)
	//{{AFX_MSG_MAP(CdlgConvertRawSgy)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEO, OnButtonBrowseo)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgConvertRawSgy message handlers

void CdlgConvertRawSgy::OnButtonBrowseo() 
{
	//new file dialog.
	CFileDialog dlgOut( FALSE,_T(""),_T("*.sgy"),
	        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_EXPLORER,
	       _T("Output Data Files (*.sgy)|*.sgy|"));
	dlgOut.m_ofn.lpstrTitle = _T("Select the output segy file");
	if(dlgOut.DoModal()==IDOK)
		m_outName = dlgOut.GetPathName();
	else
		return;
	CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DLG_OUTNAME));
	pEdit->SetWindowText(m_outName);

	delete dlgOut;
	
}

void CdlgConvertRawSgy::OnButtonBrowset() 
{
		CFileDialog dlg( TRUE,_T(""),_T("*.*"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_EXPLORER,
	                 _T("Input Shottable Files (*.*)|*.*|"));
//	TCHAR* buff=new TCHAR[65535];
	CString buff;
	// buff[0]='\0';
	dlg.m_ofn.lpstrTitle = _T("Select a shottable file for input to the conversion routine");
	// dlg.m_ofn.lpstrFile=buff;
	dlg.m_ofn.nMaxFile = 65535;//sizeof(buff);
	if(dlg.DoModal()==IDOK)
		m_tabName = dlg.GetPathName();
	else
		return;
	CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DLG_TABNAME));
	pEdit->SetWindowText(m_tabName);
	
	delete dlg;
	// delete [] buff;
	
}
