// dlgTraceBalanceXT.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgTraceBalanceXT.h"


// CdlgTraceBalanceXT dialog

IMPLEMENT_DYNAMIC(CdlgTraceBalanceXT, CDialog)

CdlgTraceBalanceXT::CdlgTraceBalanceXT(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgTraceBalanceXT::IDD, pParent)
	, m_inFile(_T(""))
	, m_outFile(_T(""))
	, m_window_sliding(0)
	, m_numTraceMix(0)
	, m_dxMix(0)
{

}

CdlgTraceBalanceXT::~CdlgTraceBalanceXT()
{
}

void CdlgTraceBalanceXT::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CdlgTraceBalanceXT)
	DDX_Text(pDX, IDC_INFILE, m_inFile);
	DDX_Text(pDX, IDC_OUTFILE, m_outFile);
	DDX_Text(pDX, IDC_PROC_WINDOW_SLIDING, m_window_sliding);
	DDX_Text(pDX, IDC_PROC_TRCMIX, m_numTraceMix);
	DDX_Text(pDX, IDC_PROC_DXMIX, m_dxMix);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CdlgTraceBalanceXT, CDialog)
END_MESSAGE_MAP()


// CdlgTraceBalanceXT message handlers
