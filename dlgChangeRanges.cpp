// dlgChangeRanges.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgChangeRanges.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgChangeRanges dialog


CdlgChangeRanges::CdlgChangeRanges(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgChangeRanges::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgChangeRanges)
	m_refName = _T("");
	m_refShot = 0;
	m_refShot2 = 99999;
	m_distance = FALSE;
	m_offset = TRUE;
	m_option = 0;
	//}}AFX_DATA_INIT
}


void CdlgChangeRanges::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgChangeRanges)
	DDX_Text(pDX, IDC_REFNAME, m_refName);
	DDX_Check(pDX, IDC_CHECK_OFFSET, m_offset);
	DDX_Check(pDX, IDC_CHECK_DISTANCE, m_distance);
	DDX_Radio(pDX, IDC_RADIO1, m_option);
	DDX_Text(pDX, IDC_REFSHOT, m_refShot);
	DDX_Text(pDX, IDC_REFSHOT2, m_refShot2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgChangeRanges, CDialog)
	//{{AFX_MSG_MAP(CdlgChangeRanges)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgChangeRanges message handlers
