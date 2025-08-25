// dlgTfdn.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgTfdn.h"
#include "afxdialogex.h"


// CdlgTfdn dialog

IMPLEMENT_DYNAMIC(CdlgTfdn, CDialog)

CdlgTfdn::CdlgTfdn(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgTfdn::IDD, pParent)
	, m_infile(_T(""))
	, m_outFile(_T(""))
	, m_outFile2(_T(""))
	, m_specThreshald(0)
	, m_numMax(0)
	, m_xWinMix(0)
	, m_tWinMix(0)
	, m_isAllTraces(FALSE)
{

}

CdlgTfdn::~CdlgTfdn()
{
}

void CdlgTfdn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIMESECNAME, m_infile);
	DDX_Text(pDX, IDC_OUTFILE, m_outFile);
	DDX_Text(pDX, IDC_OUTFILE2, m_outFile2);
	DDX_Text(pDX, IDC_EDIT_NOISE_THRESHALD, m_specThreshald);
	DDV_MinMaxFloat(pDX, m_specThreshald, 0.1f, 99999.f);
	DDX_Text(pDX, IDC_EDIT_XWIN_NUMMAX, m_numMax);
	DDV_MinMaxInt(pDX, m_numMax, 2, 9999);
	DDX_Text(pDX, IDC_EDIT_XWIN, m_xWinMix);
	DDX_Text(pDX, IDC_EDIT_TWIN, m_tWinMix);
	DDX_Check(pDX, IDC_IS_ALLTRACES, m_isAllTraces);
}


BEGIN_MESSAGE_MAP(CdlgTfdn, CDialog)
END_MESSAGE_MAP()


// CdlgTfdn message handlers
