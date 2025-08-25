// dlgUnix2PC.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
#include "dlgUnix2PC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgUnix2PC dialog


CdlgUnix2PC::CdlgUnix2PC(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgUnix2PC::IDD, pParent)
	, m_isConvertEbc2Ascii(FALSE)
	, m_isOffsetMM(FALSE)
	, m_isSwapIEEE_IBM(FALSE)
	, m_isResetZero(FALSE)
{
	//{{AFX_DATA_INIT(CdlgUnix2PC)
	m_gsca = FALSE;
	m_reNum = FALSE;
	m_useReducedTime = FALSE;
	m_isDeadTrcExisting = FALSE;
	//}}AFX_DATA_INIT
}


void CdlgUnix2PC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgUnix2PC)
	DDX_Check(pDX, IDC_CHK_GSCA, m_gsca);
	DDX_Check(pDX, IDC_CHK_RENUMBER, m_reNum);
	DDX_Check(pDX, IDC_CHK_USE_REDUCEDTIME, m_useReducedTime);
	DDX_Check(pDX, IDC_CHK_CONV_ISDEADTRC_EXIST, m_isDeadTrcExisting);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHK_CONV_EBC2ASC, m_isConvertEbc2Ascii);
	DDX_Check(pDX, IDC_CHK_CONV_ISOFFSETMM, m_isOffsetMM);
	DDX_Check(pDX, IDC_CHK_IEEE_IBM, m_isSwapIEEE_IBM);
	DDX_Check(pDX, IDC_CHK_RESET_ZERO, m_isResetZero);
}


BEGIN_MESSAGE_MAP(CdlgUnix2PC, CDialog)
	//{{AFX_MSG_MAP(CdlgUnix2PC)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgUnix2PC message handlers
