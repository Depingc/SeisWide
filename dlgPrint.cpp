// dlgPrint.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgPrint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgPrint dialog


CdlgPrint::CdlgPrint(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgPrint::IDD, pParent)
	, m_compactToOnePerTrace(FALSE)
	, m_memoryMax(0)
	, m_removeLabels(false)
	, m_isNoLabelsInside(FALSE)
	, m_isNoLines(FALSE)
{
	//{{AFX_DATA_INIT(CdlgPrint)
	m_printX = 0.0f;
	m_printY = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgPrint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgPrint)
	DDX_Text(pDX, IDC_PRINTX, m_printX);
	DDV_MinMaxFloat(pDX, m_printX, 1.f, 10.f);
	DDX_Text(pDX, IDC_PRINTY, m_printY);
	DDV_MinMaxFloat(pDX, m_printY, 1.f, 10.f);
	//}}AFX_DATA_MAP
	DDX_Radio(pDX, IDC_RAD_ONEOBJECT, m_compactToOnePerTrace);
	DDX_Text(pDX, IDC_MEMORYMAX, m_memoryMax);
	DDV_MinMaxInt(pDX, m_memoryMax, 50, 5000);
	DDX_Check(pDX, IDC_CHECK_NOLABELS, m_removeLabels);
	DDX_Check(pDX, IDC_CHECK_NOLABELS_INSIDE, m_isNoLabelsInside);
	DDX_Check(pDX, IDC_CHECK_IS_NOLINES, m_isNoLines);
}


BEGIN_MESSAGE_MAP(CdlgPrint, CDialog)
	//{{AFX_MSG_MAP(CdlgPrint)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgPrint message handlers


void CdlgPrint::PreInitDialog()
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_compactToOnePerTrace < 0) {
		// we want to disable the two area fill boxes
		CWnd* Pfield = GetDlgItem( IDC_RAD_ONEOBJECT );
		if (Pfield)
			Pfield->EnableWindow( FALSE ); 
		Pfield = GetDlgItem( IDC_RADIO4 );
		if (Pfield)
			Pfield->EnableWindow( FALSE ); 
	}

	CDialog::PreInitDialog();
}
