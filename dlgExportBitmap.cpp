// dlgExportBitmap.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
#include "dlgExportBitmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgExportBitmap dialog


CdlgExportBitmap::CdlgExportBitmap(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgExportBitmap::IDD, pParent)
	, m_removeLabels(FALSE)
	, m_isVectorize(FALSE)
	, m_isNoLabelsInside(FALSE)
	, m_numColors(0)
	, m_isNoLines(FALSE)
	, m_isNonodes(FALSE)
{
	//{{AFX_DATA_INIT(CdlgExportBitmap)
	m_xPixels = 4000;
	m_yPixels = 2000;
	m_biForm = -1;
	//}}AFX_DATA_INIT

	m_wiggle = 0;
}


void CdlgExportBitmap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgExportBitmap)
	DDX_Text(pDX, IDC_EDIT1, m_xPixels);
	DDV_MinMaxInt(pDX, m_xPixels, 1, 9999999);
	DDX_Text(pDX, IDC_EDIT2, m_yPixels);
	DDV_MinMaxInt(pDX, m_yPixels, 1, 9999999);
	DDX_Text(pDX, IDC_TIMESECNAME, m_nameBitmap);
	DDX_Radio(pDX, IDC_RADIO1, m_biForm);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_NOLABELS, m_removeLabels);
	DDX_Check(pDX, IDC_CHECK_VECTORIZE, m_isVectorize);
	DDX_Check(pDX, IDC_CHECK_NOLABELS_INSIDE2, m_isNoLabelsInside);
	DDX_Text(pDX, IDC_EDIT_NUMCOLORS, m_numColors);
	DDV_MinMaxInt(pDX, m_numColors, 1, 11);
	DDX_Check(pDX, IDC_CHECK_IS_NOLINES, m_isNoLines);
	DDX_Check(pDX, IDC_CHECK_IS_NONODES, m_isNonodes);
}


BEGIN_MESSAGE_MAP(CdlgExportBitmap, CDialog)
	//{{AFX_MSG_MAP(CdlgExportBitmap)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgExportBitmap message handlers

void CdlgExportBitmap::OnButtonBrowset() 
{
	CFileDialog dlg( TRUE,_T("bmp; BMP"),_T("*.bmp; *.BMP"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Output Bitmap File (*.bmp;*.BMP)|*.bmp; *.BMP|"));

	if( dlg.DoModal()==IDOK )
	{
		m_nameBitmap = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_TIMESECNAME));
		pEdit->SetWindowText(m_nameBitmap);
	}
	
}






BOOL CdlgExportBitmap::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CWnd* pwnd = GetDlgItem(IDC_EDIT_NUMCOLORS);
	if (pwnd) {
		pwnd->EnableWindow(m_wiggle != 1 && m_wiggle != 3);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
