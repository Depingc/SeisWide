// dlgDigResize.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
#include "dlgDigResize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgDigResize dialog


CdlgDigResize::CdlgDigResize(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgDigResize::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgDigResize)
	m_ratioResize = 1.0f;
	//}}AFX_DATA_INIT
}


void CdlgDigResize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgDigResize)
	DDX_Text(pDX, IDC_EDIT_EXPAND_RATIO, m_ratioResize);
	DDV_MinMaxFloat(pDX, m_ratioResize, 1.f, 9.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgDigResize, CDialog)
	//{{AFX_MSG_MAP(CdlgDigResize)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgDigResize message handlers
