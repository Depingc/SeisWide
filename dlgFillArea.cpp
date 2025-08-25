// dlgFillArea.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgFillArea.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgFillArea dialog


CdlgFillArea::CdlgFillArea(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgFillArea::IDD, pParent)
	, m_isNoLabelsInside(FALSE)
	, m_isNoLines(FALSE)
	, m_isNoNodes(FALSE)
	, m_isSeismicPlot(FALSE)
	, m_XDecimate(0)
{
	//{{AFX_DATA_INIT(CdlgFillArea)
	m_noLablesOutside = TRUE;
//	m_CopyAsBitmap = TRUE;
	m_XLen = 0;
	m_YLen = 0;
	//}}AFX_DATA_INIT
	m_mode = 0;
}


void CdlgFillArea::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgFillArea)
	DDX_Check(pDX, IDC_CHECK_NOLABELS, m_noLablesOutside);
	// DDX_Check(pDX, IDC_CHECK_BITMAP, m_CopyAsBitmap);
	DDX_Text(pDX, IDC_EDIT_X, m_XLen);
	DDV_MinMaxInt(pDX, m_XLen, 10, 999990);
	DDX_Text(pDX, IDC_EDIT_Y, m_YLen);
	DDV_MinMaxInt(pDX, m_YLen, 10, 9999999);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_NOLABELS_INSIDE, m_isNoLabelsInside);
	DDX_Check(pDX, IDC_CHECK_IS_NOLINES, m_isNoLines);
	DDX_Check(pDX, IDC_CHECK_IS_NONODES, m_isNoNodes);
	DDX_Check(pDX, IDC_CHECK_IS_SEISPLOT, m_isSeismicPlot);
	DDX_Text(pDX, IDC_EDIT_DECIMATE, m_XDecimate);
	DDV_MinMaxFloat(pDX, m_XDecimate, 0.f, 1000.f);
}


BEGIN_MESSAGE_MAP(CdlgFillArea, CDialog)
	//{{AFX_MSG_MAP(CdlgFillArea)
//	ON_BN_CLICKED(IDC_CHECK_BITMAP, OnCheckBitmap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgFillArea message handlers

BOOL CdlgFillArea::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_mode == 1)
	{
	//	CButton* Check1 = (CButton*)GetDlgItem(IDC_CHECK_BITMAP);
		CWnd* pwnd;
		pwnd = GetDlgItem(IDC_RAD_ONEOBJECT); if (pwnd) pwnd->EnableWindow(FALSE);
		pwnd = GetDlgItem(IDC_RADIO4); if (pwnd) pwnd->EnableWindow(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CdlgFillArea::OnCheckBitmap() 
{
//	CButton* Check1 = (CButton*)GetDlgItem(IDC_CHECK_BITMAP);
	CWnd* pwnd;
		pwnd = GetDlgItem(IDC_RAD_ONEOBJECT); if (pwnd) pwnd->EnableWindow(TRUE);
		pwnd = GetDlgItem(IDC_RADIO4); if (pwnd) pwnd->EnableWindow(TRUE);
}
